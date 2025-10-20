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

#ifndef SEPMASTER_ECAT_SERIAL_H
#define SEPMASTER_ECAT_SERIAL_H

#include <vector>
#include <mutex>
#include <deque>
#include "simple_circular_queue.h"
#include "ecmaster_app.h"


namespace sephi
{

    // RFC2217 Service Handling
    class ECatSerialSlave;
    class ECatSlaveInfo;
    struct RFC2217ServiceParam {
        std::deque<ECatSerialSlave>* serials;
    };
    struct SerialConfig {
        int baudrate;
        int databit;
        int stopbit;
        int parity;
    };
    class RFC2217ServiceTask : public Task<RFC2217ServiceParam> {
    public:
        bool initialize();
        void finalize();
        bool set_mode = false;
    private:
        void redirect2tty();
	    void set_baudrate();
	    void loadSerialConfig();
        void svc();
        ThreadCondition exit_;
        std::map<unsigned, SerialConfig> serialcfg;
    };

    class ECatSerialSlave {
    public:

        using Queue = SimpleQueue<ECatSerialData::QUEUE_BUFFER_SIZE, std::mutex>;
        ECatSerialSlave();
        ECatSerialSlave(int ec_client_id, int32_t slave_id, uint8_t* in_pdo, uint8_t* out_pdo, int portcount, SlaveType type);
        virtual bool transferRxTx();
        virtual bool setBaudRate(int portid, uint32_t baudrate);
        virtual bool setDataBit(int portid, uint8_t val);
        virtual bool setStopBit(int portid, uint8_t val);
        virtual bool setParity(int portid, uint8_t val);

        int portCount() const { return rx_buffers_.size();}
        bool isReadyToSend(int portid);
        bool isReadyToReceive(int portid);
        int getSlaveId() const { return slave_id_;}
        SlaveType getSlaveType() const {return type_;}
        Queue * getRxQueue(int port_index);
        Queue * getTxQueue(int port_index);

        // for debugging
        void dumpStates(const char* prefix="");

    protected:
        SlaveType type_;
        uint8_t* in_pdo_;
        uint8_t* out_pdo_;
        ECatSerialData &send_data_; // master sending data  to slave
        ECatSerialData &recv_data_; // master receiving data from slave
        int32_t slave_id_;
        int ec_client_id_;
        int port_count_;
        std::deque<Queue> tx_buffers_; // buffer for each serial port : master to slave.port[]
        std::deque<Queue> rx_buffers_;  // buffer for each serial port : slave.port[] to master
        std::mutex mtx_;
    };


}
#endif //SEPMASTER_ECAT_SERIAL_H
