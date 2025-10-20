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

#ifndef SEPMASTER_OHT_IO_MODEL_H
#define SEPMASTER_OHT_IO_MODEL_H

#include "simple_circular_queue.h"
#include "stocker_io_model.h"

namespace sephi
{
    /// @addtogroup grp_ecmaster_emul
    /// @{

    ///@ brief Slave with Serial
    class SerialIOSlaveModel : public StockerIOModel, public IMultiportSerialInterface
    {

    public:
        enum { SERIAL_CHUNK_SIZE = PortData::BUFFER_SIZE - 1, SERIAL_BUFF_SIZE = 1024};
        using super = StockerIOModel;
        SerialIOSlaveModel():serial_count_(0), rx_data_(nullptr), tx_data_(nullptr), rxqueues_(0), txqueues_(0){}
        void initialize();
        bool loadModelData(const string& model_file, const char* model_name);
        void processPDO();

        // user api
        int send(int port_id, const void* buff, int len);
        int receive(int port_id, void* buff, int len);

    private:
        bool isReadyToSend(int portid);
        bool isReadyToReceive(int portid);
        int serial_count_; // serial port count
        ECatSerialData *rx_data_; // master sending data  to slave
        ECatSerialData *tx_data_; // master receiving data from slave
        std::vector<SimpleQueue<SERIAL_BUFF_SIZE>> rxqueues_;
        std::vector<SimpleQueue<SERIAL_BUFF_SIZE>> txqueues_;
    };

    /// @brief OHT Front I/O 모델
    class OHTFrontIOModel : public SerialIOSlaveModel
    {
        using super = SerialIOSlaveModel;

    };

    /// @brief OHT Rear I/O 모델
    class OHTRearIOModel : public SerialIOSlaveModel
    {
        using super = SerialIOSlaveModel;
    };

    /// @}
}
#endif //SEPMASTER_OHT_IO_MODEL_H
