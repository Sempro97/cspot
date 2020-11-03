#include "AC101AudioSink.h"
#define typeof(x) __typeof__(x)
#include "driver/i2s.h"

#include "freertos/task.h"
#include "freertos/ringbuf.h"

RingbufHandle_t dataBuffer;

static void i2sFeed(void *pvParameters)
{
    while (true)
    {
        size_t itemSize;
        char *item = (char *)xRingbufferReceiveUpTo(dataBuffer, &itemSize, portMAX_DELAY, 512);
        if (item != NULL)
        {
            //vTaskDelay(1);
            size_t written = 0;
            while (written < itemSize)
            {
                i2s_write((i2s_port_t)0, item, itemSize, &written, portMAX_DELAY);
            }
            vRingbufferReturnItem(dataBuffer, (void *)item);
        }
    }
}

AC101AudioSink::AC101AudioSink()
{
    // // Disable software volume control, all handled by ::volumeChanged
    // softwareVolumeControl = false;
    // dataBuffer = xRingbufferCreate(4096 * 8, RINGBUF_TYPE_BYTEBUF);

    // i2s_config_t i2s_config = {

    //     .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX), // Only TX
    //     .sample_rate = 44100,
    //     .bits_per_sample = (i2s_bits_per_sample_t)16,
    //     .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, //2-channels
    //     .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S,
    //     .intr_alloc_flags = 0, //Default interrupt priority
    //     .dma_buf_count = 8,
    //     .dma_buf_len = 512,
    //     .use_apll = true,
    //     .tx_desc_auto_clear = true //Auto clear tx descriptor on underflow
    // };

    // i2s_pin_config_t pin_config = {
    //     .bck_io_num = 27,
    //     .ws_io_num = 26,
    //     .data_out_num = 25,
    //     .data_in_num = -1 //Not used
    // };

    // dac = &dac_a1s;

    // dac->init(0, 0, &i2s_config);
    // dac->speaker(false);
    // dac->power(ADAC_ON);
    // xTaskCreatePinnedToCore(&i2sFeed, "i2sFeed", 4096, NULL, 10, NULL, 1);
}

AC101AudioSink::~AC101AudioSink()
{
}

void AC101AudioSink::volumeChanged(uint16_t volume) {
    // dac->volume(volume, volume);
}

void AC101AudioSink::feedPCMFrames(std::vector<uint8_t> &data)
{
    // xRingbufferSend(dataBuffer, &data[0], data.size(), portMAX_DELAY);
}