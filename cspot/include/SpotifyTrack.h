#ifndef SPOTIFYTRACK_H
#define SPOTIFYTRACK_H

#include <memory>
#include <vector>
#include <iostream>
#include "MercuryManager.h"
#include "ProtoHelper.h"
#include "Utils.h"
#include "MercuryResponse.h"
#include <fstream>
#include "Crypto.h"
#include <functional>
#include "ChunkedAudioStream.h"
#include "TrackReference.h"
#include <cassert>


class SpotifyTrack
{
private:
    std::shared_ptr<MercuryManager> manager;
    void trackInformationCallback(std::unique_ptr<MercuryResponse> response);
    void episodeInformationCallback(std::unique_ptr<MercuryResponse> response);
    void requestAudioKey(std::vector<uint8_t> fileId, std::vector<uint8_t> trackId, int32_t trackDuration);
    bool countryListContains(std::string countryList, std::string country);
    bool canPlayTrack(std::vector<Restriction> &restrictions);
    Track trackInfo;
    Episode episodeInfo;

    std::vector<uint8_t> fileId;
    std::vector<uint8_t> currentChunkData;
    std::vector<uint8_t> currentChunkHeader;
public:
    SpotifyTrack(std::shared_ptr<MercuryManager> manager, std::shared_ptr<TrackReference> trackRef);
    ~SpotifyTrack();
    uint64_t reqSeqNum = -1;
    std::function<void()> loadedTrackCallback;
    std::unique_ptr<ChunkedAudioStream> audioStream;
    audioKeyCallback audioKeyLambda;
    mercuryCallback responseLambda;
};


#endif