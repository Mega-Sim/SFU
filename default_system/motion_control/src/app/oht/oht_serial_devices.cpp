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
#include "ecmaster_config.h"
#include "oht_serial_devices.h"
using namespace sephi;
using namespace sephi::oht;
using Poco::Net::Socket;
using Poco::Net::StreamSocket;
using Poco::Timespan;
using namespace std::chrono_literals;
using std::this_thread::sleep_for;

/*
static const char* __to_str(int i) {
    static char str[16];
    sprintf(str, "%d", i);
    return str;
}
*/

TcpSerialDevice::TcpSerialDevice(const char* port_nick, uint16_t sock_port_id)
    : name_{port_nick}, sock_port_id_{ sock_port_id}, conn_("localhost", sock_port_id), emulation_mode_{false}
{
    SepShm onboard(OHT_ONBOARD_IO_SHM_KEY());
    char *emul_state = onboard.address + onboard.size() - 4;
    emulation_mode_ = (memcmp(emul_state, "emul",4)==0);
    SEP_LOGT("[%s] is under %s mode\n", port_nick, emulation_mode_ ? "emulation" : "real");
    SEP_LOGT("[%s] connected to port number %d - result = %d\n", port_nick, sock_port_id, conn_.isConnected());
}

TcpSerialDevice::~TcpSerialDevice()
{
}

int TcpSerialDevice::readTcp(void* buff, int len, int timeout_ms)
{
    Timespan timeout(timeout_ms/1000, (timeout_ms%1000)*1000); // sec, usec
    Socket::SocketList readList{conn_.socket()}, writeList, exceptList;
    Socket::select(readList, writeList, exceptList, timeout);
    return (readList.size() > 0) ? conn_.receive(buff, len) : -1;
}

OnboardSerial::OnboardSerial(const char* port_nick, uint16_t sock_port_id, const char*dev_path, int baud_rate)
    : TcpSerialDevice(port_nick, sock_port_id), SerialPort()
{
    if (!emulation_mode_) {
        SerialPort::start(dev_path, baud_rate);
    }
}
OnboardSerial::~OnboardSerial()
{
    if (!emulation_mode_) {
        SerialPort::stop();
    }
}

/*
void OnboardSerial::on_receive_(const std::string &data)
{
    SEP_LOGD("SerialPort[%s] receive: %s", port_name_.c_str(), data.c_str());
    recv_queue_.push(data.data(), data.length());
}
*/

bool OnboardSerial::clearReadBuffer()
{
    if (emulation_mode_) {
        char buff[1024];
        readTcp(buff, sizeof(buff), 0);
    }
    recv_queue_.clear();
    return true;
}

int OnboardSerial::read(void* buff, int len)
{
    if (emulation_mode_) {
        return readTcp(buff, len, 10);
    } else {
        if (recv_queue_.size() == 0) sleep_for(10ms);
        return recv_queue_.pop(buff, len);
    }
}

int OnboardSerial::write(void* buff, int len)
{
    if (emulation_mode_) {
        return writeTcp(buff, len);
    } else {
        return SerialPort::write((const char *) buff, len);
    }
}

uint32_t BcrBL185::readId()
{
    clearReadBuffer();
    char data[16] = {'L', 'O', 'N', '\r'};
    if (OnboardSerial::write(data, 4) < 0) {
        SEP_LOGT("BCR[%s] fail to write\n", name());
        return INVALID_ID;
    }
    auto len = read(data, sizeof(data));
    if (len > 0) {
        char *start = strchr(data, 0x02); // 0x02, value, \n,\r
        SEP_REQUIRE_RET(start != nullptr, INVALID_ID);
        uint32_t value;
        SEP_REQUIRE_RET(1==sscanf(start+1, "%u", &value), INVALID_ID);
        SEP_LOGT("BCR[%s] id = %u\n", name(), value);
        return value;
    }
    return INVALID_ID;
}