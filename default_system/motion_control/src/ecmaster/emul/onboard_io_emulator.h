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

#ifndef SEPMASTER_ONBOARD_IO_EMULATOR_H
#define SEPMASTER_ONBOARD_IO_EMULATOR_H

#include "ecat_serial.h"
namespace sephi {

    class OnBoardIOEmulator : public ECatSerialSlave, public IMultiportSerialInterface {
    public:
        OnBoardIOEmulator(const char* model_file_name);

        bool transferRxTx() {return true;}
        bool setBaudRate(int portid, uint32_t baudrate) {return true;}
        bool setDataBit(int portid, uint8_t val) {return true;}
        bool setStopBit(int portid, uint8_t val) {return true;}
        bool setParity(int portid, uint8_t val) {return true;}
    public: // simulator side serial api
        int send(int portid, const void* data, int length);
        int receive(int portid, void* data, int length);
        // simulator side dio
        uint8_t* DO();
        uint8_t* DI();
        // simulator side aio
        int16_t& AO(int port);
        int16_t& AI(int port);

    private:
        string name_;
        std::unique_ptr<SepShm> shm_;
        int ai_count_, ai_bytes_;
        int di_count_, di_bytes_;
        int ao_count_, ao_bytes_;
        int do_count_, do_bytes_;
    };

}

#endif //SEPMASTER_ONBOARD_IO_EMULATOR_H
