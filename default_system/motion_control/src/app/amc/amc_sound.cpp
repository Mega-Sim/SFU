//
// Created by yj.choe on 2021-07-29.
//

//#include "sephi/log/logger.h"
#include "amc_sound.h"

using namespace amc;

void Sound::update_as_thread()
{
    auto start_time = std::chrono::system_clock::now();
    auto end_time = start_time;
    auto execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    while (thread_running_) {

        start_time = std::chrono::system_clock::now();
        if(get_oht_status() != -1){
            play();
        }
        end_time = std::chrono::system_clock::now();
        execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        if(execution_time.count() > 10000) {
            //LOG("main", info) << "Long Duration Sound thread : " << execution_time.count() << "ms";
        }
        std::this_thread::sleep_for(100ms);
    }
}
bool Sound::start_service()
{
    thread_running_ = true;

    if (main_thread_ == nullptr) {
        main_thread_ = std::make_unique<std::thread>(&Sound::update_as_thread, this);
        main_thread_->detach();
    }

    return (main_thread_ != nullptr);
}


void Sound::stop_service()
{
    thread_running_ = false;
}

void Sound::play()
{
    double		time1,time2,time3;
    struct   timeval	tv1,tv2;
    gettimeofday(&tv1, NULL);

    if(alsa_device_[status_] != NULL) {
        alsa_device_[status_]->play_file();
    }

    gettimeofday(&tv2, NULL);

    time1 = (double) tv1.tv_sec + ((double) tv1.tv_usec / 1000000.0);
    time2 = (double) tv2.tv_sec + ((double) tv2.tv_usec / 1000000.0);
    time3 = time2 - time1;
    //printf("%.1fsec\n", time3);
}

int8_t Sound::get_oht_status()
{
    int8_t tmp = -1;
    
    switch(sound_cmd) {
        case 1 :
            tmp  = sound::drive;
            break;
        case 2 :
            tmp = sound::trans;
            break;
        case 3:
            tmp = sound::pause;
            break;
        case 4:
            tmp  = sound::detect;
            break;
        case 5:
            tmp  = sound::obswarning;
            break;
        case 6:
            tmp  = sound::obstacle;
            break;
        case 7:
            tmp  = sound::error;
            break;
        case 8:
            tmp  = sound::lookdown;
            break;
        case 9:
            tmp  = sound::oscillation;
            break;
        default:
            break;
    }
    //ToDo : job pause sound
    if(tmp > -1)
        status_ = tmp;

    return tmp;
}

int8_t Sound::set_oht_status(int cmd)
{
	sound_cmd = cmd;
	return 0;
}

Sound::~Sound()
{
    thread_running_ = false;
    main_thread_.reset();
}
