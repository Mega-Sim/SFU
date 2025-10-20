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
// Created by master on 17. 4. 13.
//
#include "emul/ecat_emulator.h"
#include "ecmaster_service.h"


namespace sephi
{

    static bool checkIfUserAlive(pid_t userpid)
    {
        char buff[32];
        snprintf(buff, sizeof(buff), "/proc/%d", userpid);
        return access(buff, R_OK) == 0;
    }

    static void doPrintHex(const char*msg, const void* data, int len) {
        uint8_t *u8 = (uint8_t*)data;
        printf("[[ %s ]]\n", msg);
        for(int i = 0; i < len; i++) {
            if (i>0 && i%4 == 0) printf(" ");
            printf("%02x ", u8[i]);
        }
        printf("\n");
    }

    static void printHex(const char*msg, const void* data, int len) {
#if STK_IO_LOG_ENABLE
        doPrintHex(msg,data,len);
#endif
    }


    bool EcMasterSvc::readOhtFrontInData(OhtFrontIoPdoIn &in)
    {
        SEP_REQUIRE_RET(oht_front_io_ != nullptr, false);
        memcpy(&in, in_pdo+oht_front_io_->in_pd_offset, sizeof(in));

        return true;
    }

    bool EcMasterSvc::readOhtRearInData(OhtRearIoPdoIn &in)
    {
        SEP_REQUIRE_RET(oht_rear_io_ != nullptr, false);
        memcpy(&in, in_pdo+oht_rear_io_->in_pd_offset, sizeof(in));

        return true;
    }

    bool EcMasterSvc::readOhtFrontOutData(OhtFrontIoPdoOut &out)
    {
        SEP_REQUIRE_RET(oht_front_io_ != nullptr, false);
        memcpy(&out, shm_->address + EM_SHM_SECTION_SIZE + oht_front_io_->out_pd_offset, sizeof(out));

        return true;
    }

    bool EcMasterSvc::readOhtRearOutData(OhtRearIoPdoOut &out)
    {
        SEP_REQUIRE_RET(oht_rear_io_ != nullptr, false);
        memcpy(&out, shm_->address + EM_SHM_SECTION_SIZE + oht_rear_io_->out_pd_offset, sizeof(out));

        return true;
    }

    bool EcMasterSvc::writeOhtFrontOutData(const OhtFrontIoPdoOut &out)
    {
        SEP_REQUIRE_RET(oht_front_io_ != nullptr, false);

        memcpy(shm_->address + EM_SHM_SECTION_SIZE + oht_front_io_->out_pd_offset, &out, sizeof(out));
        return true;
    }

    bool EcMasterSvc::writeOhtRearOutData(const OhtRearIoPdoOut &out)
    {
        SEP_REQUIRE_RET(oht_rear_io_ != nullptr, false);

        memcpy(shm_->address + EM_SHM_SECTION_SIZE + oht_rear_io_->out_pd_offset, &out, sizeof(out));
        return true;
    }

    bool EcMasterSvc::readStockerInData(StockerIoPdoIn &in)
    {
        SEP_REQUIRE_RET(stocker_io_ != nullptr, false);
        //STK_IO_LOGT("readStockerInData\n");
        memcpy(&in, in_pdo+stocker_io_->in_pd_offset, sizeof(in));

        printHex("readStockerIn", &in, sizeof(in));
        return true;
    }
    bool EcMasterSvc::readStockerOutData(StockerIoPdoOut &in)
    {
        SEP_REQUIRE_RET(stocker_io_ != nullptr, false);
        //STK_IO_LOGT("readStockerOutData\n");
        memcpy(&in, shm_->address + EM_SHM_SECTION_SIZE + stocker_io_->out_pd_offset, sizeof(in));
        printHex("readStockerOut", &in, sizeof(in));
        return true;
    }

    bool EcMasterSvc::writeStockerOutData(const StockerIoPdoOut &out)
    {
        SEP_REQUIRE_RET(stocker_io_ != nullptr, false);
        STK_IO_LOGT("writeStockerOutData\n");
        memcpy(shm_->address + EM_SHM_SECTION_SIZE + stocker_io_->out_pd_offset, &out, sizeof(out));
        printHex("writeStockerOut", &out, sizeof(out));
        return true;
    }

    bool EcMasterSvc::handleRequestMessage(const ECatSdoMsg& msg, ECatSdoMsg &res)
    {
        res.command = msg.command + msg.RESPONSE;
        res.slave_id = -1;

        if (msg.slave_id >= 0 && msg.slave_id < slaves_.size()) {
            auto& slave = slaves_[msg.slave_id];
            if (slave.user_pid != 0 && !checkIfUserAlive(slave.user_pid)) { // collect zombie resource
                SEP_LOGT("Collecting resource from zombie pid %d\n", slave.user_pid);
                slave.user_pid = 0;
            }
            if (slave.user_pid == 0) {
                slave.user_pid = msg.req.pid;
                res.slave_id = msg.slave_id;
                res.res.in_pd_offset = slave.in_pd_offset;
                res.res.out_pd_offset = slave.out_pd_offset;
                res.res.bus_cycle_us = opt_->dwBusCycleTimeUsec;
                SEP_LOGI("slave %d is allocated to %d!\n", slave.slave_id, slave.user_pid );
            } else {
                SEP_LOGE("requested slave id (%d) is already in use. \n", msg.slave_id);
                return false;
            }
        } else {
            SEP_LOGE("requested slave id (%d) range invalid. \n", msg.slave_id);
            return false;
        }
        return true;

    }

    bool EcMasterSvc::handleReleaseMessage(const ECatSdoMsg& msg, ECatSdoMsg &res)
    {
        res.command = msg.command + msg.RESPONSE;
        res.slave_id = -1;

        if (msg.slave_id >= 0 && msg.slave_id < slaves_.size()) {
            auto& slave = slaves_[msg.slave_id];
            if (slave.user_pid == msg.req.pid) {
                slave.user_pid = 0;
                res.slave_id = msg.slave_id;
                if (slave.context != -1) {
                    servos_[slave.context].controlWord().command = Cia402ControlWord::QUICK_STOP;
                }
                SEP_LOGI("slave %d is released!\n", slave.slave_id);
            } else {
                SEP_LOGE("release slave id (%d) owner is different. owner pid=%d, your pid=%d\n", msg.slave_id, slave.user_pid, msg.req.pid);
                return false;
            }
        } else {
            SEP_LOGE("release slave id (%d) range invalid. \n", msg.slave_id);
            return false;
        }
        return true;
    }

    bool EcMasterSvc::handleSdoMessage(const ECatSdoMsg& msg)
    {
        ECatSdoMsg res(static_cast<ECatSdoMsg::Command>(msg.command + msg.RESPONSE), msg.slave_id);
        auto& slave = slaves_[msg.slave_id];
        int32_t index;

        index = msg.sdo.index;   //CISmulti
        if(slave.module_num == 1) {
            //int module_num = slave.slave_id - slave.bus_index;
            if (msg.sdo.index >= 0x2000 && msg.sdo.index < 0x3000)
                index = msg.sdo.index + 0x1000;
            else if (msg.sdo.index >= 0x6000)
                index = msg.sdo.index + 0x800;
        } else if (slave.module_num == 2) {
            if (msg.sdo.index >= 0x2000 && msg.sdo.index < 0x3000)
                index = msg.sdo.index + 0x2000;
            else if (msg.sdo.index >= 0x6000)
                index = msg.sdo.index + 0x1000;
        }

        if (msg.command == ECatSdoMsg::SDO_DOWNLOAD) {
            EC_T_DWORD data = ecatCoeSdoDownloadEx(slave.bus_index, index, msg.sdo.subindex,
                                 (uint8_t *)&msg.sdo.data, msg.sdo.length, msg.sdo.timeout, 0);

            if (EC_E_NOERROR == data) {
                res.result = 0;
            } else {
                SEP_LOGE("Fail to download sdo to slave %d: index = 0x%x, sub=%d\n", msg.slave_id, index, msg.sdo.subindex);
                res.result = -1;
            }

        } else if (msg.command == ECatSdoMsg::SDO_UPLOAD) {
            EcDWord outlen;
            if (EC_E_NOERROR == ecatCoeSdoUploadEx(slave.bus_index, index, msg.sdo.subindex,
                                                 (uint8_t *)&res.sdo.data, msg.sdo.length, &outlen, msg.sdo.timeout, 0)){
                res.sdo.length = (uint16_t)outlen;
            } else {
                SEP_LOGE("Fail to upload sdo to slave %d\n", msg.slave_id);
                res.result = -1;
            }

        } else {
            SEP_LOGE("Code should not reach here.\n");
            return false;
        }
        slave.msgq->put(res);
        return true;
    }

    bool EcMasterSvc::handleBitOpMessage(const ECatSdoMsg& msg)
    {
        ECatSdoMsg res(static_cast<ECatSdoMsg::Command>(msg.command + msg.RESPONSE), msg.slave_id);
        auto& slave = slaves_[msg.slave_id];
        auto& servo = servos_[slave.context];
        if (msg.command == msg.CLEAR_BITS) {
            servo.statusWord().ref() &= ~msg.word_bits;
            SEP_LOGT("Clearing status 0x%x\n", msg.word_bits);
        } else {
            servo.statusWord().ref() |= msg.word_bits;
            SEP_LOGT("Setting status 0x%x\n", msg.word_bits);
        }
        res.result = 0;
        slave.msgq->put(res);
        return true;
    }


    bool EcMasterSvc::handleSetMasterAxisMessage(const ECatSdoMsg &msg)
    {
        ECatSdoMsg res(static_cast<ECatSdoMsg::Command>(msg.command + msg.RESPONSE), msg.slave_id);
        auto& slave = slaves_[msg.slave_id];

        res.result = -1;

        if (msg.master_axis < 0) {
            SEP_LOGT("Unfollow axis %d\n", msg.slave_id);
            slave.following_axis = -1;
            auto& servo = servos_[slave.context];
            servo.setVelocityTarget(0); // for stop axis
            servo.setTorqueTarget(0);
            res.result = 0;
        } else if (msg.master_axis >= slaves_.size()) {
            SEP_LOGE("Master Axis range (%d) is invalid!\n", msg.master_axis);
        } else {
            auto &sync_slave = slaves_[msg.master_axis];
            if (slave.type != SlaveType::CIA402_COMPLIANT_SERVO
            && slave.type != SlaveType::CIA402_COMPLIANT_SERVO2
            && slave.type != SlaveType::CIA402_COMPLIANT_SERVO_D
            && slave.type != SlaveType::CIA402_COMPLIANT_SERVO_T) {  // yjchoe
                SEP_LOGE("Slave %d is not servo drive\n", slave.slave_id);
            } else if (sync_slave.type != SlaveType::CIA402_COMPLIANT_SERVO
            && sync_slave.type != SlaveType::CIA402_COMPLIANT_SERVO2
            && sync_slave.type != SlaveType::CIA402_COMPLIANT_SERVO_D
            && sync_slave.type != SlaveType::CIA402_COMPLIANT_SERVO_T) { // yjchoe
                SEP_LOGE("Master Slave %d is not servo drive\n", sync_slave.slave_id);
            } else if (sync_slave.following_axis != -1) {
                SEP_LOGE("Master Slave %d is following %d. Circulation is prevented.\n", sync_slave.slave_id, sync_slave.following_axis);
            } else {
                slave.following_axis = msg.master_axis;
                SEP_LOGT("slave %d is following %d\n", slave.slave_id, slave.following_axis);
                res.result = 0;
            }
        }
        slave.msgq->put(res);
        return true;
    }

    bool EcMasterSvc::handleMessages()
    {
        ECatSdoMsg msg;
        bool ret;
        while(true) {
            if (!msgq_->get(msg, 1.0)) {
                break;
            }
            ret = true; // by default
            switch(msg.command) {
                case ECatSdoMsg::RECOVER_OP_MODE:
                    SEP_LOGT("handleMessages: ECMaster State change to OP\n");
                    changeECatState(eEcatState_OP);
                    ret = true;
                    recover_in_process_ = false;
                    break;
                case ECatSdoMsg::CLEAR_BITS:/* fall through */
                case ECatSdoMsg::SET_BITS:
                    ret = handleBitOpMessage(msg);
                    break;
                case ECatSdoMsg::SET_MASTER_AXIS:
                    ret = handleSetMasterAxisMessage(msg);
                    break;
                case ECatSdoMsg::SDO_DOWNLOAD: /* fall through */
                case ECatSdoMsg::SDO_UPLOAD: {
#if 0 // async execution
                    auto job = new AsyncJobHandle();
                    *job =  std::async(std::launch::async, [this, &msg, &job](){
                        SEP_LOGT("Async SDO job %p begins\n", job );
                        bool result = handleSdoMessage(msg);
                        ECatSdoMsg req(ECatSdoMsg::REMOVE_HANDLE); req.handle = job;
                        msgq_->put(req);
                        return result;
                    });
                    async_jobs_.insert(job);
#else
                    ret = handleSdoMessage(msg);
#endif
                    break;
                }
                case ECatSdoMsg::REMOVE_HANDLE: {
                    auto job = (AsyncJobHandle*)msg.handle;
                    SEP_LOGT("Async SDO job %p erased\n", job );
                    async_jobs_.erase(job);
                    delete job;
                    break;
                }
                default:
                    SEP_LOGE("Unexpected sdo message command - %d\n ", msg.command);
            }
            if (!ret) {
                SEP_LOGW("Fail to process sdo message command %d\n", msg.command);
            }
        }
        return true;
    }
}
