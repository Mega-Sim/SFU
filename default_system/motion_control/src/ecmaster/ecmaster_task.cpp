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
// Created by master on 17. 3. 30.
//

#include <chrono>
#include "time_profile.h"
#include "ecmaster_task.h"
#include "ecmaster_service.h"


namespace sephi {

	class TimingMeasure {
	public:
		enum {TIMING_MEASURE_COUNT = 60000};
        TimingMeasure(): exec_("execution"), cycle_("cycle") {}
        void measureTiming() {
		    auto now = std::chrono::system_clock::now();
		    exec_.beginInterval(now);
		    cycle_.probeCycle(now);
        }

        void logTiming() {
            auto now = std::chrono::system_clock::now();
            exec_.endInterval(now);
            if (cycle_.count < TIMING_MEASURE_COUNT) return;
            // output timing stats
            SEP_LOGT("== CYCLE TIME  min / ave / max / std (us) ===");
            char buff[128];
            SEP_LOGT("%s", cycle_.statistics(buff, sizeof(buff), 1e6));
            SEP_LOGT("%s", exec_.statistics(buff, sizeof(buff), 1e6));
            exec_.reset();
            cycle_.reset();
        }
        
	private:
        TimeProfileItem exec_;
		TimeProfileItem cycle_;
	};

    void EmTask::svc()
    {
        auto wake_up_at = std::chrono::system_clock::now();
        auto duration = std::chrono::nanoseconds(task_param_.bus_cycle_ns);
        auto acyclic_job_time_limit = duration * 3/4;
        int nOverloadCounter  = 0;
        int bPrevCycProcessed;
        uint res;
        bool exit_loop(false);
        int cpuIndex = 3;
        TimingMeasure tm;

        // set cpu affinity
        setCpuAffinity(cpuIndex);
        // make into realtime thread; requires superuser privilege
        addIntoRealtimeSchedule(JOBS_THREAD_PRIO);

        while(!stop_) {
			tm.measureTiming();
            // process all received frames (read new input values)
            res = ecReceiveFrames(bPrevCycProcessed, nOverloadCounter);

            task_param_.master_svc->doPdoJobs();

            res = ecSendFrames(exit_loop);
            if (exit_loop) {
                break;
            }

			tm.logTiming();
			
            // sleep_until...
            wake_up_at += duration;
            std::this_thread::sleep_until(wake_up_at);
        }
    }
}
