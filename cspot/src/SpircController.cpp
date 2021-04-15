#include "SpircController.h"

SpircController::SpircController(std::shared_ptr<MercuryManager> manager, std::string username, std::shared_ptr<AudioSink> audioSink)
{

    this->manager = manager;
    this->player = std::make_unique<Player>(manager, audioSink);
    this->state = std::make_unique<PlayerState>(manager->timeProvider);
    this->username = username;

    player->endOfFileCallback = [=]() {
        if (state->nextTrack())
        {
            loadTrack();
        }
    };

    player->setVolume(MAX_VOLUME);
    subscribe();
}

void SpircController::subscribe()
{
    mercuryCallback responseLambda = [=](std::unique_ptr<MercuryResponse> res) {
        // this->trackInformationCallback(std::move(res));
        sendCmd(MessageType::kMessageTypeHello);
        printf("Sent kMessageTypeHello!\n");
    };
    mercuryCallback subLambda = [=](std::unique_ptr<MercuryResponse> res) {
        this->handleFrame(res->parts[0]);
    };

    manager->execute(MercuryType::SUB, "hm://remote/user/" + this->username + "/", responseLambda, subLambda);
}

void SpircController::handleFrame(std::vector<uint8_t> &data)
{
    state->remoteFrame = decodePb<Frame>(data);

    switch (state->remoteFrame.typ.value())
    {
    case MessageType::kMessageTypeNotify:
    {
        printf("Notify frame\n");
        // Pause the playback if another player took control
        if (state->isActive() && state->remoteFrame.device_state->is_active.value())
        {
            state->setActive(false);
            notify();
            player->cancelCurrentTrack();
        }
        break;
    }
    case MessageType::kMessageTypeSeek:
    {
        printf("Seek command\n");
        state->updatePositionMs(state->remoteFrame.position.value());
        this->player->seekMs(state->remoteFrame.position.value());
        notify();
        break;
    }
    case MessageType::kMessageTypeVolume:
        state->setVolume(state->remoteFrame.volume.value());
        player->setVolume(state->remoteFrame.volume.value());
        notify();
        break;
    case MessageType::kMessageTypePause:
    {
        printf("Pause command\n");
        player->pause();
        state->setPlaybackState(PlaybackState::Paused);
        notify();

        break;
    }
    case MessageType::kMessageTypePlay:
        player->play();
        state->setPlaybackState(PlaybackState::Playing);
        notify();
        break;
    case MessageType::kMessageTypeNext:
        if (state->nextTrack())
        {
            loadTrack();
        } else {
            player->cancelCurrentTrack();
        }
        notify();
        break;
    case MessageType::kMessageTypePrev:
        state->prevTrack();
        loadTrack();
        notify();
        break;
    case MessageType::kMessageTypeLoad:
    {
        for (int x = 0; x < data.size(); x++) {
            printf("%d, ", data[x]);
        }
        printf("\nLoad frame!\n");

        state->setActive(true);
        state->setPlaybackState(PlaybackState::Loading);

        // Every sane person on the planet would expect std::move to work here.
        // And it does... on every single platform EXCEPT for ESP32 for some reason.
        // For which it corrupts memory and makes printf fail. so yeah. its cursed.
        state->updateTracks();
        loadTrack();
        this->notify();
        break;
    }
    case MessageType::kMessageTypeReplace:
    {
        printf("Got replace frame\n");
        break;
    }
    case MessageType::kMessageTypeShuffle:
    {
        printf("Got shuffle frame\n");
        state->setShuffle(state->remoteFrame.state->shuffle.value());
        this->notify();
        break;
    }
    case MessageType::kMessageTypeRepeat:
    {
        printf("Got repeat frame\n");
        state->setRepeat(state->remoteFrame.state->repeat.value());
        this->notify();
        break;
    }
    default:
        break;
    }
}

void SpircController::loadTrack()
{
    state->setPlaybackState(PlaybackState::Loading);
    std::function<void()> loadedLambda = [=]() {
        // Loading finished, notify that playback started
        state->setPlaybackState(PlaybackState::Playing);
        this->notify();
    };

    player->handleLoad(state->getCurrentTrack(), loadedLambda);
}

void SpircController::notify()
{
    this->sendCmd(MessageType::kMessageTypeNotify);
}

void SpircController::sendCmd(MessageType typ)
{
    // Serialize current player state
    auto encodedFrame = state->encodeCurrentFrame(typ);

    mercuryCallback responseLambda = [=](std::unique_ptr<MercuryResponse> res) {};
    auto parts = mercuryParts({encodedFrame});
    this->manager->execute(MercuryType::SEND, "hm://remote/user/" + this->username + "/", responseLambda, parts);
}