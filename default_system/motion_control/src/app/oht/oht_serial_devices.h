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

#include "sephi_ipc.h"
#include "simple_circular_queue.h"
#include "serial_port.h"

namespace sephi {
    namespace oht {
        class TcpSerialDevice {
        public:
            TcpSerialDevice(const char* port_nick, uint16_t sock_port_id);
            virtual ~TcpSerialDevice();
            int writeTcp(void* buff, int len) { return conn_.send(buff, len); }
            int readTcp(void* buff, int len, int timeout_ms);
            const char* name() { return name_.c_str(); }
        protected:
            string name_;
            uint16_t sock_port_id_;
            StreamConnector conn_;
            bool emulation_mode_;
        };

        class OnboardSerial : public TcpSerialDevice, public SerialPort {
            using super = TcpSerialDevice;
        public:
            OnboardSerial(const char* port_nick, uint16_t sock_port_id, const char*dev_path = nullptr, int baud_rate = 9600);
            ~OnboardSerial();
            bool clearReadBuffer();
            int read(void* buff, int len);
            int write(void* buff, int len);

        protected:
            SimpleQueue<SERIAL_PORT_READ_BUF_SIZE, std::mutex> recv_queue_;
            // serial data receive handler
            //void on_receive_(const std::string &data);
        };

        class BcrBL185 : public OnboardSerial {
        public:
            enum BAR_CODE_VALUE : unsigned { INVALID_ID = 0xFFFFFFFF};
            using super = OnboardSerial;
        public:
            BcrBL185(const char* port_nick, uint16_t sock_port_id, const char*dev_path = nullptr, int baud_rate = 9600) : super(port_nick, sock_port_id, dev_path, baud_rate) {}
            uint32_t readId();
        };

    }
}
#endif //SEPMASTER_OHT_SERIAL_DEVICES_H
