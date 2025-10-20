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


#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <string>
#include <vector>
#include <mutex>
#include "log.h"
#include "sephi_ipc.h"
#include "simple_circular_queue.h"

namespace sephi {

    typedef boost::shared_ptr<boost::asio::serial_port> serial_port_ptr;
    using StopBits = boost::asio::serial_port_base::stop_bits;
    using Parity = boost::asio::serial_port_base::parity;
    using FlowControl = boost::asio::serial_port_base::flow_control;

    class SerialPort {
    public:
        enum Constants:int {SERIAL_PORT_READ_BUF_SIZE = 256};
    public:
        SerialPort(void);
        SerialPort(const SerialPort &p) = delete;
        SerialPort &operator=(const SerialPort &p) = delete;
        virtual ~SerialPort(void);

        virtual bool start(const char *com_port_name, int baud_rate = 115200,
                            int character_size = 8,
                            StopBits::type _stop_bit = StopBits::one,
                            Parity::type _parity = Parity::none,
                            FlowControl::type  _flow_control = FlowControl::none);
        virtual void stop();
        const string& port_name() const { return port_name_; }

        int write(const std::string &buf);
        int write(const char *buf, const int size);
        bool read(char *buf, const int size, int *read_size);
        bool readTimeout(char *buf, const int size, int *read_size, int64_t timeout_ms = -1, int end_of_line = 0xffff);
        void clearBuffer();

    protected:
        virtual void asyncReadSome();
        virtual void onReceive(const boost::system::error_code &ec, size_t bytes_transferred);

    protected:
        std::string port_name_;
        boost::asio::io_service io_service_;
        serial_port_ptr port_;
        char read_buf_raw_[SERIAL_PORT_READ_BUF_SIZE];
        std::mutex lock_;
        ThreadCondition cond_;
        SimpleQueue<SERIAL_PORT_READ_BUF_SIZE> queue_;
    };
}
#endif //__SERIAL_PORT_H__
