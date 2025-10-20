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
// Created by sem1 on 17. 8. 10.
//

#ifndef SEPMASTER_EC_STOCKER_IO_H
#define SEPMASTER_EC_STOCKER_IO_H
#include "ec_servo_motor.h"
#include "ecmaster_rpc_client.h"

namespace  sephi
{
    enum IOBoarType {
        // ethercat slave type
        STOCKER_IO,
        OHT_FRONT_IO,
        OHT_REAR_IO,
        // on-board type
        ONBOARD_TYPE    = 0x1000,
        OHT_ONBOARD_IO,
    };
    template <int type, int IO_SLAVE_ID, int AI_PORT_COUNT, int DI_PORT_COUNT, int AO_PORT_COUNT, int DO_PORT_COUNT>
    class IoBoard {
    public:
        using IoPdoIn = ECatIoInPdo<AI_PORT_COUNT, DI_PORT_COUNT>;
        using IoPdoOut = ECatIoOutPdo<AO_PORT_COUNT, DO_PORT_COUNT>;
        static const int ai_port_count = AI_PORT_COUNT;
        static const int di_port_count = DI_PORT_COUNT;
        static const int ao_port_count = AO_PORT_COUNT;
        static const int do_port_count = DO_PORT_COUNT;
        std::unique_ptr<ECatSlaveMsgQ> msg_recvq_;
        std::mutex msgq_mtx_;
    public:
        IoBoard(int slaveid = IO_SLAVE_ID): shm_(nullptr), out_pdo_(nullptr), in_pdo_(nullptr),
                                        slave_id_(slaveid), initialized_(false)
        {
            if (type == STOCKER_IO)
                name_ = "STOCKER_IO";
            else if (type == OHT_FRONT_IO)
                name_ = "OHT_FRONT_IO";
            else if (type == OHT_REAR_IO)
                name_ = "OHT_REAR_IO";
            else if (type == OHT_ONBOARD_IO)
                name_ = "OHT_ONBOARD_IO";
        }

        virtual ~IoBoard()
        {
            initialized_ = false;
            in_pdo_ = nullptr;
            out_pdo_ = nullptr;
            if (!ECAT_EMULATION_MODE) {
                if (shm_ != nullptr) shm_.reset(nullptr);
                if (type < ONBOARD_TYPE ) {
                    the_em_rpc_client.get()->releaseSlave(slave_id_);
                }
            }
        }

        virtual bool initialize(int axis)
        {
            ECatSdoMsg::RscResponseMsg res;
            // slave 공유 메모리 정보를 얻음
            if (!ECAT_EMULATION_MODE) {
                if (type < ONBOARD_TYPE) {
                    SEP_REQUIRE_RET(the_em_rpc_client.get()->requestSlave(slave_id_, res), false);
                    SEP_REQUIRE_RET(res.in_pd_offset != 0xFFFFFFFF, false);
                    axis_no_ = axis;
                    msg_recvq_ = std::make_unique<ECatSlaveMsgQ>(EM_MSG_Q_KEY_SLAVE(axis).c_str());
                    shm_ = std::make_unique<SepShm>(EM_SHM_KEY());
                    SEP_REQUIRE_RET(shm_ != nullptr && msg_recvq_ != nullptr, false);
                    out_pdo_ = (IoPdoOut *) (shm_->address + EM_SHM_SECTION_SIZE + res.out_pd_offset);
                    in_pdo_ = (IoPdoIn *) (shm_->address + res.in_pd_offset);
                } else {
                    throw  std::runtime_error("Need implementation.");
                }
            } else {
                static IoPdoOut pdoOut;
                static IoPdoIn pdoIn;
                out_pdo_ = &pdoOut;
                in_pdo_ = &pdoIn;
            }
            initialized_ = true;
            return true;
        }

        virtual uint16_t readAi(int port)
        {
            if (!initialized_) {
                throw std::runtime_error(name_ + " Not initialized");
            }
            if (port < 0 || port >= AI_PORT_COUNT) {
                throw std::runtime_error(name_ + " AI port range error");
            }
            return in_pdo_->ain[port];
        }

        virtual bool writeAo(int port, uint16_t val)
        {
            if (port < 0 || port >= AO_PORT_COUNT) {
                throw std::runtime_error(name_ + " AO port range error");
            }
            out_pdo_->aout[port] = val;
            return true;
        }
        virtual bool readDi(unsigned portbit) { return readDi(portbit>>16, portbit & 0x0000FFFF);}
        virtual bool readDi(int port, int bit)
        {
            if (!initialized_) {
                throw std::runtime_error(name_+" initialized");
            }
            if (port < 0 || port >= DI_PORT_COUNT) {
                throw std::runtime_error(name_+" DI port range error");
            }
            uint64_t& di = *(uint64_t*)in_pdo_->din;
            return di & (1ULL << (port*8+bit));
        }
        virtual bool readDo(unsigned portbit) { return readDo(portbit>>16, portbit & 0x0000FFFF);}
        virtual bool readDo(int port, int bit)
        {
            if (!initialized_) {
                throw std::runtime_error(name_+" initialized");
            }
            if (port < 0 || port >= DO_PORT_COUNT) {
                throw std::runtime_error(name_+" DO port range error");
            }
            uint64_t& dout = *(uint64_t*)out_pdo_->dout;
            return dout & (1ULL << (port*8+bit));
        }

        virtual bool writeDo(unsigned portbit, bool val) { return writeDo(portbit >> 16, portbit &0x0000FFFF, val);}
        virtual bool writeDo(int port, int bit, bool val)
        {
            if (port < 0 || port >= DO_PORT_COUNT) {
                throw std::runtime_error(name_+" DO port range error");
            }
            uint64_t& dout = *(uint64_t*)out_pdo_->dout;
            //dout &= ~(1ULL << (port*8+bit));
            if (val) {
                dout |= (1ULL << (port*8+bit));
            } else {
                dout &= ~(1ULL << (port*8+bit));
            }

            return true;
        }

        virtual uint64_t readDi64()
        {
            return *(uint64_t*)in_pdo_->din;
        }

        virtual uint64_t readDo64()
        {
            return *(uint64_t*)out_pdo_->dout;
        }

        virtual ECatSlaveMsgQ* getSendQ()
        {
            if (ECAT_EMULATION_MODE){
                throw std::runtime_error("Servo is under Emulation Mode");
            }

            static std::mutex mtx;
            static std::unique_ptr<ECatSlaveMsgQ> sendq = nullptr;
            std::lock_guard<std::mutex> guard(mtx);
            if (sendq == nullptr) {
                sendq = std::make_unique<ECatSlaveMsgQ>(EM_MSG_Q_KEY());
            }
            return sendq.get();
        }

        virtual int coeSdoUpload(uint16_t index, uint16_t subindex, uint8_t* val, int len)
        {
            ECatSdoMsg msg(ECatSdoMsg::SDO_UPLOAD, axis_no_);
            msg.sdo.index = index;
            msg.sdo.subindex = subindex;
            msg.sdo.length  = len;

            std::lock_guard<std::mutex> guard(msgq_mtx_);
            SEP_REQUIRE_RET(getSendQ()->put(msg), -1);

            ECatSdoMsg res;
            SEP_REQUIRE_RET(msg_recvq_->get(res), -1);
            SEP_REQUIRE_RET(res.command == res.RESPONSE_SDO_UPLOAD, -1);
            SEP_REQUIRE_RET(res.result != -1, -1);

            memcpy(val, res.sdo.data, res.sdo.length);
            return res.sdo.length;
        }

        virtual bool coeSdoDownload(uint16_t index, uint16_t subindex, const uint8_t *val, int len)
        {
            ECatSdoMsg msg(ECatSdoMsg::SDO_DOWNLOAD, axis_no_);
            msg.sdo.index = index;
            msg.sdo.subindex = subindex;
            msg.sdo.length  = len;
            memcpy(msg.sdo.data, val, len);

            std::lock_guard<std::mutex> guard(msgq_mtx_);
            SEP_REQUIRE_RET(getSendQ()->put(msg), false);

            ECatSdoMsg res;
            SEP_REQUIRE_RET(msg_recvq_->get(res), false);
            SEP_REQUIRE_RET(res.command == res.RESPONSE_SDO_DOWNLOAD, false);
            return res.result != -1;
        }
    protected:
        std::unique_ptr<SepShm> shm_;
        std::string name_;
        IoPdoOut* out_pdo_;
        IoPdoIn* in_pdo_;
        int bus_cycle_us_;
        int slave_id_;
        bool initialized_;
        int axis_no_;
    };

    // slave type, slave id, ai port, di port byte, ao port, do port byte
    typedef IoBoard<STOCKER_IO, 4,4,6,4,5> EcStockerIo;

    /// 3axis platform
    //typedef IoBoard<OHT_FRONT_IO,4,5,6,8,8> EcOHTFrontIo;
    //typedef IoBoard<OHT_REAR_IO,3,5,8,10,6> EcOHTRearIo;

    /// 4axis
    typedef IoBoard<OHT_REAR_IO,4,5,8,10,6> EcOHTRearIo;
    typedef IoBoard<OHT_FRONT_IO,5,5,6,8,8> EcOHTFrontIo;




}
#endif //SEPMASTER_EC_STOCKER_IO_H
