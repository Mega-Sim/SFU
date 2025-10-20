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
// Created by root on 17. 12. 5.
//

#ifndef SEPMASTER_ONBOARD_IO_H
#define SEPMASTER_ONBOARD_IO_H

#include "io_board.h"
#include "oht_io_config.h"

namespace sephi
{

    class OhtOnboardIOEmul : public IoBoard<OHT_ONBOARD_IO,-1,4,(38+7)/8,4,(22+7)/8> { // slave type, slave id, ai port, di port, ao port, do port
    public:
        virtual bool initialize();
        bool isEmulationMode() { return is_emulation_mode_; }

    protected:
        bool is_emulation_mode_;
    };


    class OhtOnboardIO : public OhtOnboardIOEmul {
        enum Constants{
            DI_POLL_CYCLE_MS = 10
        };
        using super = OhtOnboardIOEmul;
        enum SensorId {
            OBS,
            OHT_DET,
            QRR1,
            QRR2,
            BCR1,
            BCR2
        };
    public:
        OhtOnboardIO();
        ~OhtOnboardIO();
        bool initialize() override ;
        bool readDi(unsigned portbit) override ;
        bool readDiCache(unsigned portbit);
        bool readDo(unsigned portbit) override ;
        bool writeDo(unsigned portbit, bool val) override ;
        //static bool initializeGpioMap(bool intialize = false);
        OhtIoConfig ioConfig_;
    private:
        bool exit_;
        std::unique_ptr<std::thread> poll_th_;
        void poll();
        void synchrosizeSharedMemory(uint64_t* pdo, int index, bool val);
    };


}

#endif //SEPMASTER_ONBOARD_IO_H
