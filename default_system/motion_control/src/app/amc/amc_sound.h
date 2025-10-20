//
// Created by yj.choe on 2021-07-29.
//

#ifndef MASTER_CONTROLLER_AMC_SOUND_H
#define MASTER_CONTROLLER_AMC_SOUND_H

#include "ALSADevice.h"
#include "amc_emulator.h"
#include <thread>
#include <chrono>
#include <sys/time.h>

using namespace std::chrono_literals;

#define SOUND_CHANNEL 9

namespace amc {
    class Sound {

    public :
        Sound ()
                : status_{0}
        {
            for(int i = 0; i < SOUND_CHANNEL; i++){
                alsa_device_[i] = std::make_unique<ALSADevice>("default", wav_dir[i], sound_time[i],
                                                               buffer_setting[i], sleep_time_ms[i], this);
                if(!alsa_device_[i]->initialize()){
                    alsa_device_[i] = nullptr;
                }
            }

            start_service();
            sound_cmd = 0;
        }
        ~Sound() ;

        bool start_service();
        void stop_service();
        void update_as_thread();
        void play();

        int8_t get_oht_status();
		int8_t set_oht_status(int cmd);
		int sound_cmd;

    private:

        std::string wav_dir[SOUND_CHANNEL] = {
                "sound/Driving.wav",
                "sound/Trans.wav",
                "sound/Pause.wav",
                "sound/Detect.wav",
                "sound/ObsWarnning.wav",
                "sound/Obstacle.wav",
                "sound/Error.wav",
                "sound/Lookdown.wav",
                "sound/Oscillation.wav"
        };

        //play time(ms)
        int sound_time[SOUND_CHANNEL] = {
                6000,
                7000,
                1000,
                1000,
                1000,
                3000,
                3000,
		        1000,	// test
		        1000	// test
        };

        //buffer_setting
        int buffer_setting[SOUND_CHANNEL] = {
                10, 10, 10, 10, 10, 10, 10, 10, 10
        };

        //sleep_time_ms
        int sleep_time_ms[SOUND_CHANNEL] = {
                5, 5, 5, 5, 5, 5, 5, 5, 5
        };

        int status_;

        std::unique_ptr<ALSADevice> alsa_device_[SOUND_CHANNEL];
        bool thread_running_{ true };
        std::unique_ptr<std::thread> main_thread_;
    };
}

namespace amc::sound {
    constexpr uint8_t drive{0};
    constexpr uint8_t trans{1};
    constexpr uint8_t pause{2};
    constexpr uint8_t detect{3};
    constexpr uint8_t obswarning{4};
    constexpr uint8_t obstacle{5};
    constexpr uint8_t error{6};
    constexpr uint8_t lookdown{7};
    constexpr uint8_t oscillation{8};
    constexpr uint8_t channels{9};
}

#endif //MASTER_CONTROLLER_AMC_SOUND_H
