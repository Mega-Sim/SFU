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
// Created by sem1 on 17. 7. 21.
//

#include "ecat_serial.h"
#include "ecmaster_service.h"
#include "emul/ecat_emulator.h"
namespace sephi
{
#define SERIAL_LOGD(...) SEP_LOGD(__VA_ARGS__)

    // EtherCAT SerialSlave Implementation
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    static ECatSerialData __dummy_pdo;
    ECatSerialSlave::ECatSerialSlave() : recv_data_(__dummy_pdo), send_data_(__dummy_pdo) // to avoid reference initialization error
    {
    }

    ECatSerialSlave::ECatSerialSlave(int client_id, int32_t slave_id, uint8_t* in_pdo, uint8_t* out_pdo, int portcount, SlaveType type)
            : ec_client_id_(client_id), slave_id_(slave_id), in_pdo_(in_pdo), out_pdo_(out_pdo), port_count_(portcount), type_(type),
              rx_buffers_(portcount), tx_buffers_(portcount), recv_data_(*(ECatSerialData*)in_pdo), send_data_(*(ECatSerialData*)out_pdo)
    {
       // SEP_LOGT("Ecat Serial Slave [%d] recv_data = %p, send_data = %p", slave_id_, in_pdo, out_pdo);
        // initialize send data packet state
        for(auto i = 0; i < portcount; ++i) {
            if (the_ec_emulator.get()->emulationMode()) {
                send_data_.pkt_cnt[i] = 0;
                send_data_.pkt_cnt_echo[i] = 0;
            } else {
                send_data_.pkt_cnt[i] = recv_data_.pkt_cnt_echo[i];
                send_data_.pkt_cnt_echo[i] = recv_data_.pkt_cnt[i];
            }
        }
        memset(send_data_.data, 0, sizeof(ECatSerialData));
    }

    ECatSerialSlave::Queue * ECatSerialSlave::getRxQueue(int port_index)
    {
        SEP_REQUIRE_RET(port_index >= 0 && port_index < port_count_, nullptr);
        return &rx_buffers_[port_index];
    }

    ECatSerialSlave::Queue * ECatSerialSlave::getTxQueue(int port_index)
    {
        SEP_REQUIRE_RET(port_index >= 0 && port_index < port_count_, nullptr);
        return &tx_buffers_[port_index];
    }


    bool ECatSerialSlave::setBaudRate(int portid, uint32_t baudrate)
    {
        return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, 0x8010, portid*5 + 1, (EcByte*)&baudrate, 4, COE_SDO_TIMEOUT_MS, 0);
    }

    bool ECatSerialSlave::setDataBit(int portid, uint8_t val)
    {
        return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, 0x8010, portid*5 + 2, (EcByte*)&val, 1, COE_SDO_TIMEOUT_MS, 0);
    }

    bool ECatSerialSlave::setStopBit(int portid, uint8_t val)
    {
        return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, 0x8010, portid*5 + 3, (EcByte*)&val, 1, COE_SDO_TIMEOUT_MS, 0);
    }

    bool ECatSerialSlave::setParity(int portid, uint8_t val)
    {
        return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, 0x8010, portid*5 + 4, (EcByte*)&val, 1, COE_SDO_TIMEOUT_MS, 0);
    }

    bool ECatSerialSlave::isReadyToSend(int portid)
    {
        /// \todo Send error check
        //return (send_data_.pkt_cnt[portid] == recv_data_.pkt_cnt_echo[portid]
        //        || send_data_.pkt_cnt[portid] == (recv_data_.pkt_cnt_echo[portid] + 1)%256);
        return (send_data_.pkt_cnt[portid] == recv_data_.pkt_cnt_echo[portid]);
    }

    bool ECatSerialSlave::isReadyToReceive(int portid)
    {
        /// \todo Receive error check; ex. recv_data.length == 0?
        //return recv_data_.pkt_cnt[portid] == (send_data_.pkt_cnt_echo[portid]+1)%256;
        return (recv_data_.pkt_cnt[portid] != send_data_.pkt_cnt_echo[portid]);
    }

    void ECatSerialSlave::dumpStates(const char* prefix)
    {
        if (slave_id_ == 0) {
            int i = 0; // port id
           // SERIAL_LOGD("%sEcat Serial[%d:%d] dump - send_pkt_cnt = %d, recv_pkt_cnt_echo = %d\n", prefix, slave_id_, i,
           //          send_data_.pkt_cnt[i], recv_data_.pkt_cnt_echo[i]);
        }
    }

    bool ECatSerialSlave::transferRxTx()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        for(int i = 0; i < tx_buffers_.size(); ++i ) {
            auto &tx_buffer = tx_buffers_[i];
            /*  CIS
            if(i==0)
                SERIAL_LOGD("send_data.pkt_cnt : %d , recv_data_pkt_cnt_echo : %d , tx_buffer.size : %d",send_data_.pkt_cnt[i], recv_data_.pkt_cnt_echo[i],tx_buffer.size() );
                if(tx_buffer.size() > 0)
                    SERIAL_LOGD("Size = %d!!!!!!!!!!!!!!!!!!!!!!!",tx_buffer.size());
            */
            if (!isReadyToSend(i) || tx_buffer.size() == 0) { // slave buffer가 준비 안됐거나 보낼 데이터가 없는 경우
                if (tx_buffer.size() > 0) {
                    //SERIAL_LOGD("Cant send. ecat Serial[%d] send_pkt_cnt = %d, recv_pkt_cnt_echo = %d\n", i, send_data_.pkt_cnt[i], recv_data_.pkt_cnt_echo[i]);
                };
            } else {
                send_data_.data[i].length = tx_buffer.pop(send_data_.data[i].payload, ECatSerialData::MAX_SERIAL_OVER_ECAT_PACKET_SIZE-1);
                send_data_.pkt_cnt[i] = (send_data_.pkt_cnt[i] + 1)%256;
            }

            // Master RX : transfer data from slave
            if (!isReadyToReceive(i) || rx_buffers_[i].remains() < ECatSerialData::MAX_SERIAL_OVER_ECAT_PACKET_SIZE) { // incomming data가 없는 경우
                ;
            } else {
                rx_buffers_[i].push(recv_data_.data[i].payload, recv_data_.data[i].length);
                send_data_.pkt_cnt_echo[i] = recv_data_.pkt_cnt[i];
            }
        }
        return true;
    }


    // EtherCat Serial Service Implementations
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    int  EcMasterSvc::getSerialPortCount(const ECatSlaveInfo &slave)
    {
        // \todo need to update : read from ESC configurations
        return ECatSerialData::SERIAL_PORT_COUNT_PER_SLAVE;
    }

    bool  EcMasterSvc::dumpIoSlaveInfo(const ECatSlaveInfo &slave)
    {
#if defined(USE_ACONTIS)
        const int MAX_NUM_OF_SLAVE_VARS = 128;
        EC_T_PROCESS_VAR_INFO_EX var_info_ex[MAX_NUM_OF_SLAVE_VARS];
        EC_T_WORD wSlaveInpVarInfoNumOf;
        if (emGetSlaveInpVarInfoNumOf(INSTANCE_MASTER_DEFAULT, EC_TRUE, slave.station_address, &wSlaveInpVarInfoNumOf) != EC_E_NOERROR)
        {
            SEP_LOGT("emGetSlaveInpVarInfoNumOf failed");
        }

        SEP_LOGT("emGetSlaveInpVarInfoNumOf : %d", wSlaveInpVarInfoNumOf);

        EC_T_WORD wReadEntries;
        if (emGetSlaveInpVarInfoEx(
                INSTANCE_MASTER_DEFAULT,
                EC_TRUE,
                slave.station_address,
                wSlaveInpVarInfoNumOf,
                var_info_ex,
                &wReadEntries
        ) != EC_E_NOERROR) {
            SEP_LOGT("emGetSlaveInpVarInfoEx failed");
        }

        EC_T_WORD wSlaveOutpVarInfoNumOf;
        if (emGetSlaveOutpVarInfoNumOf(INSTANCE_MASTER_DEFAULT, EC_TRUE, slave.station_address, &wSlaveOutpVarInfoNumOf) != EC_E_NOERROR)
        {
            SEP_LOGT("emGetSlaveInpVarInfoNumOf failed");
        }

        SEP_LOGT("emGetSlaveInpVarInfoNumOf : %d", wSlaveOutpVarInfoNumOf);

        if (emGetSlaveOutpVarInfoEx(
                INSTANCE_MASTER_DEFAULT,
                EC_TRUE,
                slave.station_address,
                wSlaveOutpVarInfoNumOf,
                var_info_ex+wSlaveInpVarInfoNumOf,
                &wReadEntries
        ) != EC_E_NOERROR) {
            SEP_LOGT("emGetSlaveInpVarInfoEx failed");
        }

        for (int i = 0; i < (wSlaveInpVarInfoNumOf+wSlaveOutpVarInfoNumOf); i++) {
            EC_T_PROCESS_VAR_INFO_EX* pEn = &var_info_ex[i];
            SEP_LOGT("=============================");
            SEP_LOGT("VAR_NUM   : %d", i);
            SEP_LOGT("szName    : %s", pEn->szName);
            SEP_LOGT("wDataType : %d", pEn->wDataType);
            SEP_LOGT("nBitSize  : %d", pEn->nBitSize);
            SEP_LOGT("nBitOffs  : %d", pEn->nBitOffs);
        }
#else
        SEP_LOGW("Current EtherCAT master stack does not support dump.");
#endif
        return true;
    }

    bool EcMasterSvc::doSerialDataTransaction() {
        if (ECAT_SERIAL_SLAVE_ENABLE) {
            for (auto &serial_slave : serials_) {
                serial_slave.transferRxTx();
            }
        }
        return true;
    }

}
