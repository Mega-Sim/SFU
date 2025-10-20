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
// Created by sy1135.jeong1696 on 2017-11-03.
//

#include <yaml-cpp/yaml.h>
#include "ecat_slave.h"
#include "slave_info.h"
#include "ecmaster_service.h"
using namespace std;
namespace sephi {

    bool ECatSlaveModel::getSlaveInfo(ECatSlaveInfo* _slaveinfo)
    {
        ECatSlaveInfo& slaveinfo = *_slaveinfo;
        //slaveinfo.slave_id is set by ecat emulator;
        slaveinfo.station_address = this->addr_;
        slaveinfo.alias_address = slaveinfo.slave_id+1;
        slaveinfo.vendor_id = vendor_id_;
        slaveinfo.product_code = prod_code_;
        slaveinfo.context = -1L;
        slaveinfo.user_pid = 0;
        slaveinfo.following_axis = -1;
        slaveinfo.type = static_cast<SlaveType>(type_);
        slaveinfo.in_pd_offset = tx_bit_offset_ / 8;
        slaveinfo.out_pd_offset = rx_bit_offset_ / 8;
        slaveinfo.in_pd_length = tx_bit_length_ / 8;
        slaveinfo.out_pd_length = rx_bit_length_ / 8;
        return true;
    }

    CoeValue::Type ECatSlaveModel::coeEntryType(int index, int subindex)
    {
        auto sdo = findSdoByObjId(index, subindex);
        return (sdo == nullptr) ? CoeValue::UNKNOWN : sdo->type;
    }

    bool ECatSlaveModel::loadModelData(const string& model_file, const char* model_name)
    {
        try {
            auto config = YAML::LoadFile(model_file);
            // load model data
            auto entries = config[model_name]["CoE"];
            for (auto i = 0; i < entries.size(); i++) {
                auto entry = entries[i];
                int index = hexstring2val(entry["Index"].as<string>().substr(2));
                int subindex = entry["SubIndex"].as<int>();
                sdo_map_[coekey(index, subindex)] = CoeValueEx(
                        entry["Name"].as<string>().c_str(),
                        CoeValue::getType(entry["DataType"].as<string>()),
                        entry["Value"].as<unsigned>()
                );
            }
            // load instance data
            auto instances = config[model_name]["Instance"];
            for (auto addr = instances.begin(); addr != instances.end(); addr++) {
                if (this->addr_ != addr->first.as<int>()) continue;

                for(auto it = addr->second.begin(); it != addr->second.end(); it++) {
                    CoeValueEx* obj = findSdoByName(it->first.as<string>());
                    if (obj != nullptr) {
                        obj->u32 = it->second.as<unsigned>();
                    } else {
                        CoeValue val;
                        val.u32 = it->second.as<unsigned>();
                        setPdoValue(it->first.as<string>(), val);
                    }
                }
            }

        } catch(std::exception& e) {
            SEP_LOGE("%s: Fail to load config from file %s with error %s\n", name_.c_str(), model_file, e.what() );
            return false;
        }
        return true;

    }
    void ECatSlaveModel::set(const string& name, int32_t type, int32_t vendor, int32_t prod, uint32_t physaddr, void* rxpdo, int rx_bit_offset, int rx_bit_len, void* txpdo, int tx_bit_offset, int tx_bit_len)
    {
        id_ = -1; // not set
        name_ = name;
        type_ = type;
        vendor_id_ = vendor;
        prod_code_ = prod;
        addr_ = physaddr;
        rxpdo_ = (uint8_t*)rxpdo;
        rx_bit_offset_ = rx_bit_offset;
        rx_bit_length_ = rx_bit_len;
        txpdo_ = (uint8_t*)txpdo;
        tx_bit_offset_ = tx_bit_offset;
        tx_bit_length_ = tx_bit_len;

        initialize();

        SEP_LOGT("EcatSlaveModel addr=%d, rxpdo = 0x%08x, txpdo = 0x%08x\n", addr_, rxpdo_, txpdo_);
        SEP_LOGT("               rx offset/size= %d/%d, tx offset/size=%d/%d\n", rx_bit_offset_/8, rx_bit_length_/8, tx_bit_offset_/8, tx_bit_length_/8);
    }

    CoeValueEx* ECatSlaveModel::findSdoByName(const string& objname)
    {
        for(auto& kv : sdo_map_) {
            if (kv.second.name == objname) return &kv.second;
        }
        return nullptr;
    }

    CoeValueEx* ECatSlaveModel::findSdoByObjId(int index, int subindex)
    {
        auto objid = coekey(index, subindex);
        if (sdo_map_.find(objid) != sdo_map_.end()) {
            return &sdo_map_[objid];
        }
        return nullptr;
    }

    const char* ECatSlaveModel::getSlaveTypeName()
    {
        if (type_ == SlaveType::CIA402_COMPLIANT_SERVO) return "YaskawaServo";
        if (type_ == SlaveType::CIA402_COMPLIANT_SERVO2) return "RSA Single-Servo"; // yjchoe
        if (type_ == SlaveType::CIA402_COMPLIANT_SERVO_D) return "RSA Dual-Servo"; // yjchoe
        if (type_ == SlaveType::CIA402_COMPLIANT_SERVO_T) return "RSA Triple-Servo"; // yjchoe
        if (type_ == SlaveType::FINE_STK_IO) return "FINE_STK_IO";
        if (type_ == SlaveType::FINE_OHT_FRONT_IO) return "FINE_OHT_FRONT_IO";
        if (type_ == SlaveType::FINE_OHT_REAR_IO) return "FINE_OHT_REAR_IO";
        return "Unknown";
    }

    int32_t ECatSlaveModel::getSlaveType(uint32_t vendor_id, uint32_t product_code)
    {
        SlaveType type = SlaveType::UNKNOWN;
        if (vendor_id == ecvendor_yaskawa && product_code == 0x2200301) {
            type = SlaveType::CIA402_COMPLIANT_SERVO;
        } else if (vendor_id == ecvendor_rsa) {
            type = SlaveType::CIA402_COMPLIANT_SERVO2;  // yjchoe
        } else if (vendor_id == ecvendor_fine) {
            if (product_code == ecprodcode_fine_stocker_io) {
                type = SlaveType::FINE_STK_IO;
            } else if (product_code == ecprodcode_fine_oht_front) {
                type = SlaveType::FINE_OHT_FRONT_IO;
            } else if (product_code == ecprodcode_fine_oht_rear) {
                type = SlaveType::FINE_OHT_REAR_IO;
            }
        }
        return static_cast<int32_t>(type);
    }

    void ECatSlaveModel::putMailboxData(MailBoxData& mbx)
    {
        mbox_.push_back(&mbx);
    }

    void ECatSlaveModel::registerPdoEntry(bool rx, int index, int subindex, int bitlen, int bitoffset, const std::string& name)
    {
        pdo_map_.emplace(std::make_pair(coekey(index,subindex), PdoEntry(index, subindex, bitlen, bitoffset, name, rx)));
    }

    void ECatSlaveModel::registerSdoEntry(int index, int subindex, CoeValue::Type type, const std::string& name, uint32_t initial_value)
    {
        sdo_map_.emplace(std::make_pair(coekey(index, subindex), CoeValueEx(name.c_str(), type, initial_value)));
    }

    void ECatSlaveModel::processSDO()
    {
        while(!mbox_.empty()) {
            auto &mb = mbox_.front();
            handleMailBox(*mb);
            if (mb->waiter){
                mb->waiter->wakeup();
            }
            mbox_.pop_front();
        }
    }

    bool ECatSlaveModel::handleMailBox(MailBoxData& mbox)
    {
        auto sdo = findSdoByObjId(mbox.index, mbox.subindex);
        if (sdo != nullptr) {
            if (mbox.upload) {
                mbox.data.u32 = sdo->u32;
                mbox.datalen = sdo->dataLength();
            } else {
                sdo->u32 = mbox.data.u32;
            }
            mbox.result = 0;
            SEP_LOGI("%s: CoE object 0x%04x:%d set to %d\n", this->getModelName(), mbox.index, mbox.subindex, mbox.data.u32);
            return true;
        } else {
            mbox.result = -1;
            SEP_LOGE("%s: CoE object 0x%04x:%d cannot be handled\n", this->getModelName(), mbox.index, mbox.subindex);
            return false;
        }
    }

    void ECatSlaveModel::update()
    {
        processPDO();
        processSDO();
    }

    void ECatSlaveModel::printStatus()
    {
        printf("[ Slave Info : %s id =%d, addr=%d ]\n", getModelName(), id_, addr_);
        //printf("\tRx: addr = %p, bit offset = %d, bit length = %d\n", rxpdo_, rx_bit_offset_, rx_bit_length_);
        //printf("\tTx: addr = %p, bit offset = %d, bit length = %d\n", txpdo_, tx_bit_offset_, tx_bit_length_);
    }

    void ECatSlaveModel::dumpObjectDictionary()
    {
        printf("---- %s 0x%x CoE Objects  ------\n", getModelName(), this->addr_);
        printf(">> RX pdo\n");
        for(auto& p : pdo_map_) { // p.frist = key, p.second = entry
            auto &e = p.second;
            if (!e.rx) continue;
            printf("[0x%04x:%2d] %3d %3d %s\n", e.index, e.subindex, e.bitoffset, e.bitlen, e.name.c_str());
        }
        printf(">> TX pdo\n");
        for(auto& p : pdo_map_) { // p.frist = key, p.second = entry
            auto &e = p.second;
            if (e.rx) continue;
            printf("[0x%04x:%2d] %3d %3d %s\n", e.index, e.subindex, e.bitoffset, e.bitlen, e.name.c_str());
        }

        printf(">> SDO state map, type, name, init val \n");
        for(auto& kv : sdo_map_) { // p.frist = key, p.second = entry
            int index = 0xFFFF &(kv.first >> 16);
            int subindex = 0x0000FFFF & kv.first;
            auto &e = kv.second;
            printf("[0x%04x:%2d] %d %s %u\n", index, subindex, e.type, e.name.c_str(), e.u32);
        }

    }
}
