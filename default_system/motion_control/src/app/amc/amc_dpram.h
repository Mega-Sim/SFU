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
// Created by sem1 on 17. 7. 25.
// IPC와의 DPRAM을 emulating한다.
//

#ifndef SEPMASTER_AMC_DPRAM_H
#define SEPMASTER_AMC_DPRAM_H

#include "sephi_ipc.h"
#include "sephi_task.h"
#include "amc_datastruct.h"

namespace amc
{
    namespace {
        const int DPRAM_SIZE = 1024;
        const uint16_t DPRAM_PORT = 9000;
    }

    struct DPRamStruct {
        union {
            uint8_t mem[DPRAM_SIZE];
            DPRamMemoryMap map;
        };
    public:
        DPRamStruct(){
            memset(mem, 0, sizeof(mem));
        }
        // member interfaces
        auto& Cmd_Dpram() { return *(COMMAND_DRRAM*)&mem[384]; }
       // auto& Amc_Data() { return *(AMC_DATA*)&mem[192]; }
        //auto& Amc_Cmd() { return *(AMC_CMD*)&mem[192]; }
        auto& Amc_Data() { return *(AMC_DATA_RAW*)&mem[192]; }
        auto& Amc_Cmd() { return *(AMC_CMD_RAW*)&mem[192]; }
        auto* Dpram_buf() { return (int*)&mem[192];}
        auto& Dl_Param() { return *(DOWNLOAD_PARAM*)&mem[576]; }
        auto& Exchange_str() { return *(EXCHANGE_STRING*)&mem[576]; }
        auto& Move_X_Param() { return *(MOVE_X_PARAM*)&mem[592]; }
        //ADD
    };

    class IAmcDispacher {
    public:
        virtual bool dispatch(bool initial_sync) = 0;
        virtual bool stop_all(void) = 0;
    };
    struct DPRamConnection;
    struct DPRamParam {
        DPRamStruct* ram;
    };
    class DPRamTask : public sephi::Task<DPRamParam> {
    public:
        DPRamTask(DPRamStruct *ram, IAmcDispacher *dispatcher);
        void update();
    private:
        void svc();
        std::atomic_bool need_tx_;
        IAmcDispacher* dispatcher_;
    };

}
#endif //SEPMASTER_AMC_DPRAM_H
