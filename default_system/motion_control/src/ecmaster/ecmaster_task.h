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
// do cyclic process
#ifndef SEPMASTER_ECMASTER_TASK_H
#define SEPMASTER_ECMASTER_TASK_H

#include "ecmaster_config.h"
#include "ecat_data_struct.h"
#include "sephi_task.h"

namespace sephi {
    /**\addtogroup grp_ecmaster
     * \{
     */
    class EcMasterSvc;
    struct EmTaskParam {
        EcMasterSvc* master_svc;
        // cycle 주기 (us)
        uint32_t bus_cycle_ns;
        // wake up용 condition variable key
        // emulation model
        bool emulation_mode;
    };

    class EmTask : public Task<EmTaskParam> {
    public:
        using time_point = std::chrono::time_point<std::chrono::system_clock>;

    //protected: -- debug
        void svc();
    };

    class EmEmulatorTask : public EmTask {
    protected:
        void svc();
        void svc2();
    };


    /// \}

    extern int ecReceiveFrames(int &bPrevCycProcessed, int &nOverloadCounter);
    extern int ecSendFrames(bool &exit_loop);
}

#endif //SEPMASTER_ECMASTER_TASK_H
