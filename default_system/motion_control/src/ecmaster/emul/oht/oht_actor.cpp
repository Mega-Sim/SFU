/*
 * SEMES CONFIDENTIAL
 *
 * Copyright (c) 2017 SEMES Co.,Ltd.
 * All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains the property
 * of SEMES and its suppliers, if any. The intellectual and technical concepts
 * contained herein are proprietary to SEMES and its suppliers and may be
 * covered by S.Korea and Foreign Patents, patents in process, and are
 * protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from SEMES.
 */


//
// Created by root on 17. 11. 29.
//

#include "oht_actor.h"
#include "emul/ecat_emulator.h"
using namespace sephi;
using namespace sephi::oht_sim;

OHTActor::OHTActor()
{
    thread_stop_ = false;
#ifdef OHT_MAP_TEST
    update_cycletime_ = 1e-3; // 1ms
#else
        update_cycletime_ = the_ec_emulator.get()->busCycleUs() * 1e-6;
#endif
}

bool OHTActor::runAsThread()
{
    th_ = std::make_unique<std::thread>([this](){
       addIntoRealtimeSchedule(MAIN_THREAD_PRIO+2);
       while(!this->thread_stop_) {
           usleep(update_cycletime_*1e6);
           this->update();
       }
    });
    return th_ != nullptr;
}


void OHTActor::stopThread()
{
    thread_stop_ = true;
    if (th_) th_->join();
    th_.reset();
}