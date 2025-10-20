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
// Created by sem1 on 17. 11. 6.
//
#include <yaml-cpp/yaml.h>
#include "oht_io_model.h"
#include "ecat_serial.h"

namespace sephi
{
#define SERIAL_LOGD(...) SEP_LOGD(__VA_ARGS__)

    void SerialIOSlaveModel::initialize()
    {
        super::initialize();
    }

    bool SerialIOSlaveModel::loadModelData(const string& model_file, const char* model_name)
    {
        SEP_REQUIRE_RET(super::loadModelData(model_file, model_name), false);
        auto config = YAML::LoadFile(model_file);
        // load model data
        serial_count_ = config[model_name]["SERIAL"].as<int>();
        tx_data_ = (ECatSerialData*)(txpdo_ + tx_bit_length_/8 - sizeof(ECatSerialData));
        rx_data_ = (ECatSerialData*)(rxpdo_ + rx_bit_length_/8 - sizeof(ECatSerialData));

        SEP_LOGT("Serial IO Emulator slave %d: rx = %p, tx = %p\n", id_, rx_data_, tx_data_);
        // construct queue
        rxqueues_.resize(serial_count_);
        txqueues_.resize(serial_count_);

        for(auto i = 0; i < serial_count_; ++i) {
            tx_data_->pkt_cnt[i] = 0;
            tx_data_->pkt_cnt_echo[i] = 0;
        }
        memset(tx_data_->data, 0, sizeof(ECatSerialData));
        return true;
    }

    bool SerialIOSlaveModel::isReadyToSend(int portid)
    {
        auto& send_data_ = *tx_data_;
        auto& recv_data_ = *rx_data_;
        return (send_data_.pkt_cnt[portid] == recv_data_.pkt_cnt_echo[portid]
                || send_data_.pkt_cnt[portid] == (recv_data_.pkt_cnt_echo[portid] + 1)%256);
    }

    bool SerialIOSlaveModel::isReadyToReceive(int portid)
    {
        auto& send_data_ = *tx_data_;
        auto& recv_data_ = *rx_data_;
        return recv_data_.pkt_cnt[portid] == (send_data_.pkt_cnt_echo[portid]+1)%256;
    }


    void SerialIOSlaveModel::processPDO()
    {
        auto& send_data_ = *tx_data_;
        auto& recv_data_ = *rx_data_;

        for(int i = 0; i < serial_count_; ++i ) {
            auto &tx_buffer = txqueues_[i];
            // Master TX : transfer data to slave
            if (!isReadyToSend(i) || tx_buffer.size() == 0) { // slave buffer가 준비 안됐거나 보낼 데이터가 없는 경우
                if (tx_buffer.size() > 0) {
                    SERIAL_LOGD("Model cant send. ecat Serial[%d] send_pkt_cnt = %d, recv_pkt_cnt_echo = %d\n", i, send_data_.pkt_cnt[i], recv_data_.pkt_cnt_echo[i]);
                }
                ;
            } else {
                send_data_.data[i].length = tx_buffer.pop(send_data_.data[i].payload, SERIAL_CHUNK_SIZE);
                send_data_.pkt_cnt[i] = (send_data_.pkt_cnt[i] + 1)%256;
                SERIAL_LOGD("SerialModel[%d:%d] Send... snd =%d, rcv echo=%d, len=%d, data:%d %d %d %d", id_, i,  send_data_.pkt_cnt[i], recv_data_.pkt_cnt_echo[i],
                         send_data_.data[i].length, send_data_.data[i].payload[0], send_data_.data[i].payload[1], send_data_.data[i].payload[2], send_data_.data[i].payload[3]);
            }

            // Master RX : transfer data from slave
            if (!isReadyToReceive(i) || rxqueues_[i].remains() < SERIAL_CHUNK_SIZE) { // incomming data가 없는 경우
                ;
            } else {
                rxqueues_[i].push(recv_data_.data[i].payload, recv_data_.data[i].length);
                send_data_.pkt_cnt_echo[i] = recv_data_.pkt_cnt[i];
                SERIAL_LOGD("SerialModel[%d:%d] Recv... rcv=%d, snd echo=%d, len=%d, data:%d %d %d %d", id_, i,  recv_data_.pkt_cnt[i], send_data_.pkt_cnt_echo[i],
                         recv_data_.data[i].length, recv_data_.data[i].payload[0], recv_data_.data[i].payload[1], recv_data_.data[i].payload[2], recv_data_.data[i].payload[3]);
            }
        }

    }

    // user api
    int SerialIOSlaveModel::send(int port_id, const void* buff, int len)
    {
        int writable_size = txqueues_[port_id].remains();
        if (len > writable_size) {
            len = writable_size;
        }
        if (len > 0) {
            SEP_LOGT("Tx[%d] port=%d, len=%d\n", id_, port_id);
            return txqueues_[port_id].push(buff, len);
        }
        return 0;
    }

    int SerialIOSlaveModel::receive(int port_id, void* buff, int len)
    {
        return rxqueues_[port_id].pop(buff, len);
    }

}