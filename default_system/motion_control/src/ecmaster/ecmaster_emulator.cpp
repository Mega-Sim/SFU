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
// Created by sem1 on 17. 11. 2.
//

#include "emul/ecat_emulator.h"
#include "emul/onboard_io_emulator.h"
#include "ecmaster_service.h"

namespace sephi
{
    Poco::SingletonHolder<ECatEmulator> the_ec_emulator;
    bool EcMasterSvc::initEmulator(bool observer_mode)
    {
        ECatEmulator* emul = the_ec_emulator.get();
        SEP_REQUIRE_RET(emul->initialize(this, opt_->enifile, opt_->modelfile, opt_->dwBusCycleTimeUsec, observer_mode), false);

        // start serial over ehtercat service
        if ( !observer_mode && serials_.size() > 0) {
            startSerialOverEtherCATService();
        }
        return true;
    }

    // this thread emulate ecat stack-side jobs
    void EmEmulatorTask::svc2()
    {
        auto wake_up_at = std::chrono::system_clock::now();
        auto duration = std::chrono::nanoseconds(task_param_.bus_cycle_ns);
        ECatEmulator *emul = the_ec_emulator.get();

        addIntoRealtimeSchedule(JOBS_THREAD_PRIO);

        while(!stop_) {

            // write output values of current cycle, by sending all cyclic frames
            emul->doTxJobs();

            // async jobs
            emul->doAsyncJobs();

            // sleep_until...
            wake_up_at += duration;
            std::this_thread::sleep_until(wake_up_at);
        }
    }

    void EmEmulatorTask::svc()
    {
        auto wake_up_at = std::chrono::system_clock::now();
        auto duration = std::chrono::nanoseconds(task_param_.bus_cycle_ns);
        auto acyclic_job_time_limit = duration * 3/4;
        int nOverloadCounter  = 0;
        int bPrevCycProcessed;
        uint res;
        int cpuIndex = 0;
        ECatEmulator *emul = the_ec_emulator.get();

        std::thread update_task([this](){svc2();});

        // set cpu affinity
        //setCpuAffinity(cpuIndex);
        // make into realtime thread; requires superuser privilege
        addIntoRealtimeSchedule(JOBS_THREAD_PRIO);

        while(!stop_) {

            // process all received frames (read new input values)
            emul->doRxJobs();

            task_param_.master_svc->doPdoJobs();

            // sleep_until...
            wake_up_at += duration;
            std::this_thread::sleep_until(wake_up_at);
        }

        update_task.join();
    }


    // rpc handler

}
