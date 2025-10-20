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
// Created by sy1135.jeong1696 on 2017-11-04.
//
#include <boost/algorithm/string/replace.hpp>
#include <yaml-cpp/yaml.h>
#include <ecmaster_service.h>
#include "ecat_emulator.h"
#include "yaskawa_servo_model.h"
#include "oht_io_model.h"
#include "ecmaster_service.h"
#include "oht/oht_simulator.h"
#include "onboard_io_emulator.h"

namespace sephi
{
    struct AsyncJobData {
        enum Result{
            ERROR = -1,
            NO_ERROR = 0,
            NOT_DONE = 1
        };
        AsyncJobData(int client_id, int port_id, bool isUpload, int index, int subindex, uint8_t *data, int datalen);
        ~AsyncJobData();
        bool operator==(const AsyncJobData& other) { return transfer_id == other.transfer_id; }
        bool is_upload;
        int index;
        int subindex;
        int datalen;
        int port_id;
        int result;
        uint32_t transfer_id;
        uint8_t buff[PortData::BUFFER_SIZE];
        EcMailboxTrans* mbx_buffer; // NOTE! this buffer is INVALID when emulation mode
    };

    AsyncJobData::AsyncJobData(int client_id, int _port_id, bool isUpload, int _index, int _subindex, uint8_t *data, int _datalen)
            : port_id(_port_id), is_upload(isUpload), index(_index), subindex(_subindex), datalen(_datalen)
    {
        static uint32_t _transfer_id = 0;
        transfer_id = _transfer_id++;
        result = NOT_DONE;
        if (is_upload && data == nullptr) {
            data = buff;
        }
        if (sephi::the_ec_emulator.get()->emulationMode()){
            mbx_buffer = nullptr;
            if (!is_upload) {
                memcpy(buff, data, datalen);
            }
        } else {
#if defined(UES_ACONTIS)
            EC_T_MBXTFER_DESC mbxTferDesc = {PortData::BUFFER_SIZE};
            mbxTferDesc.pbyMbxTferDescData = buff;
            mbx_buffer = ecatMbxTferCreate(&mbxTferDesc);
            if (!is_upload) {
                OsMemcpy(mbx_buffer->pbyMbxTferData, data, datalen);
            }
            mbx_buffer->dwTferId = transfer_id;
            mbx_buffer->dwClntId = client_id;
            mbx_buffer->dwDataLen = datalen;
#endif
        }
    }

    AsyncJobData::~AsyncJobData()
    {
        if (mbx_buffer != nullptr) {
#if defined(UES_ACONTIS)
            ecatMbxTferDelete(mbx_buffer);
#endif
        }
    }



    ECatEmulator::ECatEmulator()
        :emulation_mode_enabled_(false), master_(nullptr), onboard_io_(nullptr)
    { }

    bool ECatEmulator::loadENI(const string& eni_file_name)
    {
        SEP_REQUIRE_RET(access(eni_file_name.c_str(), R_OK)==0, false);
        // load ENI yaml file & construct network data
        try {
            auto eni = YAML::LoadFile(eni_file_name);
            auto slaves_eni = eni["EtherCATConfig"]["Config"]["Slave"];
            for( auto i = 0; i < slaves_eni.size(); i++) {
                // slave model instance 생성
                const auto& slave_eni = slaves_eni[i];
                const auto& info = slave_eni["Info"];
                const auto& pd = slave_eni["ProcessData"];
                auto vendor_id = info["VendorId"].as<int>();
                auto prod_code = info["ProductCode"].as<int>();
                auto slavetype = ECatSlaveModel::getSlaveType(vendor_id, prod_code);
                switch(slavetype) {
                    case SlaveType::CIA402_COMPLIANT_SERVO:
                        slaves_.push_back(std::make_unique<YaskawaServoModel>());
                        break;
                    case SlaveType::CIA402_COMPLIANT_SERVO2:    // yjchoe
                        slaves_.push_back(std::make_unique<YaskawaServoModel>());
                        break;
                    case SlaveType::FINE_STK_IO:
                        //slaves_.push_back(std::make_unique<IOModel>());
                        break;
                    case SlaveType::FINE_OHT_FRONT_IO:
                        slaves_.push_back(std::make_unique<OHTFrontIOModel>());
                        break;
                    case SlaveType::FINE_OHT_REAR_IO:
                        slaves_.push_back(std::make_unique<OHTRearIOModel>());
                        break;
                    default:
                        SEP_LOGE("enifle=%s, unknow slave type %d", eni_file_name.c_str(), slavetype);
                }
                auto &slave = slaves_.back();
                slave->set(
                        info["Name"].as<string>().c_str(),
                        slavetype,
                        vendor_id,
                        prod_code,
                        info["PhysAddr"].as<int>(),
                        rxpdo_ + pd["Send"]["BitStart"].as<int>() / 8,
                        pd["Send"]["BitStart"].as<int>(),
                        pd["Send"]["BitLength"].as<int>(),
                        txpdo_ + pd["Recv"]["BitStart"].as<int>() / 8,
                        pd["Recv"]["BitStart"].as<int>(),
                        pd["Recv"]["BitLength"].as<int>()
                );

                // pdo entry  등록
                // \todo bit offset 정상적으로 계산 되는 지 확인할 것
                for(auto& p : {std::make_pair<bool,const YAML::Node>(true, pd["RxPdo"]),  std::make_pair<bool,const YAML::Node>(false, pd["TxPdo"])}) {
                    int bitoffset = 0;
                    auto entries = (p.second.IsSequence()) ? p.second[0]["Entry"] : p.second["Entry"];
                    for (auto i = 0; i < entries.size(); i++) {
                        auto entry = entries[i];
                        auto index = hexstring2val(entry["Index"].as<string>().substr(2));
                        if (index != 0) { // index 0 is for marking end of entries
                            slave->registerPdoEntry(p.first, // rx tx
                                                    index,
                                                    entry["SubIndex"].as<int>(),
                                                    entry["BitLen"].as<int>(),
                                                    bitoffset,
                                                    entry["Name"] ? entry["Name"].as<string>() : "_PAD_"
                            );
                            bitoffset += entry["BitLen"].as<int>();
                        }
                    }
                }
            }
        } catch(std::exception& e) {
            SEP_LOGE("enifle=%s\n %s", eni_file_name.c_str(), e.what());
            return false;
        }

        return true;
    }

    bool ECatEmulator::initTargetSystem(const string& model_file_name)
    {
        SEP_REQUIRE_RET(access(model_file_name.c_str(), R_OK)==0, false);
        auto config = YAML::LoadFile(model_file_name);


        // load model data
        auto target_system_name = config["TargetSystem"].as<string>();
        if (target_system_name == "Stocker") {
            //target_system_ = std::make_unique<StockerSystem>();
        } else if (target_system_name == "OHT") {
            target_system_ = std::make_unique<OHTSystem>();
        } else {
            SEP_LOGE("Taget %s Simulation System is not implemented!\n", target_system_name );
            throw std::runtime_error("Taget Simulation System is not implemented!");
        }

        return target_system_->initialize(*this, model_file_name);
    }

    bool ECatEmulator::updateMasterInfo(ECatSlaveInfo& _slave)
    {
        SEP_REQUIRE_RET(emulation_mode_enabled_==true, true); // skip this on observer mode

        // master service update
        auto& slaves_ = master_->slaves_;
        auto& servos_ = master_->servos_;
        auto& serials_ = master_->serials_;
        //auto& stocker_io_ = master_->stocker_io_;
        auto client_id = master_->client_id;
        auto& in_pdo = master_->in_pdo;
        auto& out_pdo = master_->out_pdo;

        // add slave
        slaves_.push_back(_slave);
        auto& slave = slaves_.back();

        // assign message queue
        slave.msgq = new ECatSlaveMsgQ(EM_MSG_Q_KEY_SLAVE(slave.slave_id).c_str(), true);

        // context 설정
        if (slave.type == SlaveType::CIA402_COMPLIANT_SERVO || slave.type == SlaveType::CIA402_COMPLIANT_SERVO2) {  // yjchoe
            //CiA402Servo(int ec_client_id, int32_t slave_id, uint8_t* in_pdo, uint8_t* out_pdo, int bus_cycle_us);
            servos_.emplace_back(client_id, slave.slave_id, in_pdo + slave.in_pd_offset, out_pdo + slave.out_pd_offset, bus_cycle_us_);
            slave.context = servos_.size()-1; // servo_ index
        } else if (slave.type == SlaveType::FINE_SERIAL) {
            serials_.emplace_back(client_id, slave.slave_id, in_pdo + slave.in_pd_offset,
                                  out_pdo + slave.out_pd_offset, master_->getSerialPortCount(slave), slave.type);
            slave.context = serials_.size()-1; // serial io index
        } else if (slave.type == SlaveType::FINE_STK_IO) {
            //stocker_io_ = &slave;
        } else if (slave.type == SlaveType::FINE_OHT_FRONT_IO) {
            serials_.emplace_back(client_id, slave.slave_id,
                                  in_pdo + slave.in_pd_offset + slave.in_pd_length - sizeof(ECatSerialData),
                                  out_pdo + slave.out_pd_offset + slave.out_pd_length - sizeof(ECatSerialData),
                                  ECatSerialData::OHT_FRONT_SERIAL_COUNT, slave.type);
            slave.context = serials_.size()-1; // serial io index
        } else if (slave.type == SlaveType::FINE_OHT_REAR_IO) {
            serials_.emplace_back(client_id, slave.slave_id,
                                  in_pdo + slave.in_pd_offset + slave.in_pd_length - sizeof(ECatSerialData),
                                  out_pdo + slave.out_pd_offset + slave.out_pd_length - sizeof(ECatSerialData),
                                  ECatSerialData::OHT_REAR_SERIAL_COUNT, slave.type);
            slave.context = serials_.size()-1; // serial io index
        }

        // dump slave info
        slave.print();

    }

    bool ECatEmulator::setSlaveInfo()
    {
        ECatSlaveInfo slaveinfo;
        int slave_id = 0;
        for(auto& slave: slaves_) {
            slaveinfo.slave_id = slave_id++;
            slave->setId(slaveinfo.slave_id);
            slave->getSlaveInfo(&slaveinfo);
            updateMasterInfo(slaveinfo);
        }
        return true;
    }

    bool ECatEmulator::initialize(EcMasterSvc *ecmaster, const string& eni_file_name, const string& model_file_name, uint32_t bus_cycle_us, bool as_observer)
    {
        emulation_mode_enabled_ = ! as_observer;
        bus_cycle_us_ = bus_cycle_us;
        master_ = ecmaster;

        // init pdo memory
        memset(rxpdo_buffer_, 0, sizeof(rxpdo_buffer_));
        memset(txpdo_buffer_, 0, sizeof(txpdo_buffer_));

        // map pdo
        if (emulation_mode_enabled_) {
            rxpdo_ = rxpdo_buffer_;
            txpdo_ = txpdo_buffer_;
            master_->in_pdo = getProcessImageInputPtr();
            master_->out_pdo = getProcessImageOutputPtr();
        } else {
            rxpdo_ = master_->out_pdo;
            txpdo_ = master_->in_pdo;
        }

        // load eni
        SEP_REQUIRE_RET(loadENI(eni_file_name), false);
        // load state
        //string model_file_name = "../config/model_data.yml"; -- for test
        uint32_t slave_id = 0;
        for(auto& slave: slaves_) {
            slave->setId(slave_id++);
            slave->loadModelData(model_file_name, slave->getSlaveTypeName());
            slave->initialize();
        }
        // init onboad io emulator
        onboard_io_ = std::make_unique<OnBoardIOEmulator>(model_file_name.c_str());

        // set slave information to ecmaster
        SEP_REQUIRE_RET(setSlaveInfo(), false);

        // init simulator
        SEP_REQUIRE_RET(initTargetSystem(model_file_name), false);

        return true;
    }

    uint8_t* ECatEmulator::getProcessImageInputPtr()
    {
        return txpdo_;
    }

    uint8_t* ECatEmulator::getProcessImageOutputPtr()
    {
        return rxpdo_;
    }

    void ECatEmulator::doRxJobs()// receives updated stats
    {
        // receive pdo
        // slave model들이 직접 rxpdo_ buffer를 이용하므로 수신 동작 없음
    }

    void ECatEmulator::doTxJobs()// send new datagram
    {
        // send pdo and trigger slaves to update
        // slave model들이 직접 txpdo_ buffer이용하므로 send 동작 없음
        // triggering slave models to update
        for(auto& slave : slaves_) {
            slave->update();
        }

        // target update
        target_system_->update();

    }

    ECatSlaveModel& ECatEmulator::getSlaveById(int id)
    {
        for(auto& slave: slaves_) {
            if (slave->id() == id) return *slave;
        }
        throw std::runtime_error("No maching slave with id");
    }

    CoeValue::Type ECatEmulator::coeEntryType(int id, int index, int subindex)
    {
        return getSlaveById(id).coeEntryType(index, subindex);
    }


    int ECatEmulator::coeSdoDownload(int id, int index, int subindex, uint8_t *data, int datalen, ThreadCondition *waiter)
    {
        std::unique_ptr<ThreadCondition> cond;
        if (waiter == nullptr) {
            cond = std::make_unique<ThreadCondition>();
            waiter = cond.get();
        }
        MailBoxData mb(waiter, index, subindex);
        mb.datalen = datalen;
        memcpy(&mb.data, data, datalen);
        getSlaveById(id).putMailboxData(mb);
        waiter->wait();
        return mb.result;
    }

    int ECatEmulator::coeSdoUpload_i(int id, int index, int subindex, uint8_t *data, int datalen, int* uploadlen, ThreadCondition *waiter)
    {
        unsigned int outlen;
        int res = coeSdoUpload(id, index, subindex, data, datalen, &outlen, waiter);
        if (uploadlen) *uploadlen = outlen;
        return res;
    }

    int ECatEmulator::coeSdoUpload(int id, int index, int subindex, uint8_t *data, int datalen, unsigned int* uploadlen, ThreadCondition* waiter)
    {
        std::unique_ptr<ThreadCondition> cond;
        if (waiter == nullptr) {
            cond = std::make_unique<ThreadCondition>();
            waiter = cond.get();
        }

        MailBoxData mb(waiter, index, subindex, true);
        mb.datalen = datalen;
        memcpy(&mb.data, data, datalen);
        getSlaveById(id).putMailboxData(mb);
        waiter->wait();
        memcpy(data, &mb.data, mb.datalen);
        *uploadlen = mb.datalen;
        return mb.result;
    }

    int ECatEmulator::addAsyncJob(int id, AsyncJobData *job)
    {
        async_jobs_.emplace_back(std::make_pair(job, MailBoxData(nullptr, job->index, job->subindex, job->is_upload)));
        auto& mb = async_jobs_.back().second;
        mb.result = AsyncJobData::NOT_DONE;
        mb.datalen = job->datalen;
        memcpy(&mb.data, job->buff, job->datalen);
        getSlaveById(id).putMailboxData(mb);
        return 0;
    }

    void ECatEmulator::doAsyncJobs()
    {
        for(auto it = async_jobs_.begin(); it != async_jobs_.end(); it = async_jobs_.erase(it)) {
            auto job = it->first;
            auto& mb = it->second;
            // copy async job result to receiver
            if (job->is_upload) {
                memcpy(job->buff, mb.data.arr, mb.datalen);
            }
            // job state will be change from NOT_DONE to result
            job->result = mb.result;
        }
    }

}
