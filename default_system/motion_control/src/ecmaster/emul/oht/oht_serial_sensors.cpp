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

#include "oht_serial_sensors.h"

using namespace sephi;
using namespace sephi::oht_sim;

VirtualSerialDevice::VirtualSerialDevice(IMultiportSerialInterface* io_slave, int port_index)
        : io_slave_(io_slave), port_index_(port_index)
{ }

int VirtualSerialDevice::send(void* buff, int len)
{
    return io_slave_->send(port_index_, buff, len);
}

int VirtualSerialDevice::receive(void* buff, int len)
{
    return io_slave_->receive(port_index_, buff, len);
}

void BL_185::checkTrigger()
{
    char data[16];
    if ( read_value_ != 0xFFFFFFFF && receive(data, sizeof(data)) > 0 && strncmp(data, "LON", 3)==0) {
     //   SEP_LOGT("BL_185: LON\n");
        auto len = snprintf(data, sizeof(data), "\x02%u\r\r\n", read_value_);
        send(data, len);
    }
}

// send barcode data when new value comes
void BL_185::setData(uint32_t barcode_value)
{
    if (barcode_value == read_value_) return;
    read_value_ = barcode_value;
    char data[16];
    auto len = snprintf(data, sizeof(data), "\x02%u\r\r\n", read_value_);
    send(data, len);
    SEP_LOGT("BL_185: send %s\n", data);
}
