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
// Created by root on 18. 1. 9.
//

#ifndef SEPMASTER_OHT_SERIAL_DEVICES_H
#define SEPMASTER_OHT_SERIAL_DEVICES_H

#include <array>
#include "ecat_data_struct.h"
#include "emul/oht_io_model.h"

namespace sephi {
    namespace oht_sim {
        class VirtualSerialDevice {
        public:
            VirtualSerialDevice(IMultiportSerialInterface* io_slave, int port_index);
            int send(void* buff, int len);
            int receive(void* buff, int len);
        private:
            IMultiportSerialInterface* io_slave_;
            int port_index_;
        };

        // bar code reader
        class BL_185 : public VirtualSerialDevice {
            using super = VirtualSerialDevice;
        public:
            BL_185(IMultiportSerialInterface* io_slave, int port_index)
                    : super (io_slave, port_index), read_value_(0xFFFFFFFF) {}
            void checkTrigger();
            void setData(uint32_t barcode_value);
        private:
            uint32_t read_value_; // value already sent
        };
    }
}

#endif //SEPMASTER_OHT_SERIAL_DEVICES_H
