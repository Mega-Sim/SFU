//
// Created by yj.choe on 2021-07-29.
//

#ifndef MASTER_CONTROLLER_ALSADEVICE_H
#define MASTER_CONTROLLER_ALSADEVICE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API
#include <alsa/asoundlib.h>
#include <alsa/control.h>
#include </usr/include/alsa/pcm.h>

namespace amc
{
    class Sound;

    struct wave_header {
        struct {
            uint32_t magic;
            uint32_t length;
            uint32_t type;
        } hdr;
        struct {
            uint32_t type;
            uint32_t length;
        } chunk1;
        struct {
            uint16_t format;
            uint16_t channels;
            uint32_t rate;
            uint32_t bytes_per_sec;
            uint16_t sample_size;
            uint16_t sample_bits;
        } body;
        struct {
            uint32_t type;
            uint32_t length;
        } chunk;
    };

    class ALSADevice {
    private:

        snd_pcm_t *handle_;
        std::string device_name_;
        std::string path_;
        FILE* file_;
        snd_pcm_format_t format_;
        unsigned long BUFFER_SIZE;

        size_t bytesRead_;
        unsigned int bytes_per_sample_;
        unsigned int sample_size_;
        unsigned int channels_;
        unsigned int rate_;
        unsigned int buffer_time;

        int file_size;
        int play_time_ms_;
        int buffer_setting_;
        int sleep_time_ms_;

        int8_t* buffer_;

        enum _snd_pcm_stream type_;                      // SND_PCM_STREAM_CAPTURE | SND_PCM_STREAM_PLAYBACK
        Sound* sound_;

    public:
        ALSADevice(
                std::string device_name, std::string path, int play_time_ms,
                int buffer_setting, int sleep_time_ms, Sound* sound)
                :   device_name_(device_name),
                    path_(path),
                    format_(SND_PCM_FORMAT_S16_LE),
                    type_(SND_PCM_STREAM_PLAYBACK),
                    rate_(44100),
                    channels_(1),
                    BUFFER_SIZE(4096),
                    buffer_(NULL),
                    play_time_ms_(play_time_ms),
                    buffer_setting_(buffer_setting),
                    sleep_time_ms_(sleep_time_ms),
                    sound_(sound){
        }

        ~ALSADevice() {
            if(buffer_ != NULL) {
                delete buffer_;
                fclose(file_);
                snd_pcm_drain(handle_);
                snd_pcm_close(handle_);
            }
        }

        bool initialize();
        int set_hwparams(snd_pcm_hw_params_t *params, snd_pcm_access_t access);

        void play_file();
        int check_wav_file(const char* file);

    };
}

#endif //MASTER_CONTROLLER_ALSADEVICE_H
