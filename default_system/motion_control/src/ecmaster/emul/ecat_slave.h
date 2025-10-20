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
// Created by sem1 on 17. 11. 3.
//

#ifndef SEPMASTER_ECAT_SLAVE_H
#define SEPMASTER_ECAT_SLAVE_H
#include <string>
#include <map>
#include <list>
#include "ecat_cia402.h"

namespace sephi
{
    /// @addtogroup grp_ecmaster_emul
    /// @{

    struct PdoEntry {
        uint16_t index;     ///< COE index
        uint16_t subindex;  ///< COE sub-index
        uint16_t bitlen;    ///< bit length
        uint16_t bitoffset; ///< bit offset
        std::string name;   ///< entry name
        bool rx;            ///< true: rx, false: tx
        PdoEntry() = default;
        PdoEntry(uint16_t _index, uint16_t _subindex, uint16_t _bitlen, uint16_t _bitoffset, const std::string _name, bool _rx) :
                index(_index), subindex(_subindex), bitlen(_bitlen), bitoffset(_bitoffset), name(_name), rx(_rx)  {}
    };

    // SDO message data
    struct MailBoxData {
        MailBoxData(ThreadCondition*_waiter, uint16_t _index, uint16_t _subindex, bool _upload = false)
                : waiter(_waiter), index(_index), subindex(_subindex), result(0), upload(_upload){
            memset(data.arr, 0, sizeof(data.arr));
        }
        ThreadCondition* waiter;    ///< waiting thread's condition handle
        uint16_t index;     ///< COE index
        uint16_t subindex;  ///< COE sub-index
        int datalen;
        bool upload; ///< upload to master
        union Payload{
            int8_t  i8;
            int16_t i16;
            int32_t i32;
            uint8_t u8;
            uint16_t u16;
            uint32_t u32;
            uint8_t  arr[16];
        } data; ///< payload data.
        int32_t result;
    };

    struct CoeValueEx : public CoeValue{
        std::string name;
    public:
        CoeValueEx() = default;
        CoeValueEx(const char* _name, CoeValue::Type _type, uint32_t init_val = 0){
            name = _name;
            type = _type;
            u32 = init_val;
        }
    };

    static inline uint32_t coekey(int index, int subindex = 0) {
        return ((index<<16) + subindex);
    }
    static inline uint32_t hexstring2val(const std::string& str) {
        std::stringstream ss;
        uint32_t ret;
        ss << std::hex << str;
        ss >> ret;
        return ret;
    };

    class ECatSlaveInfo;

    /** @brief EtherCAT Slave Controller를 모사하는 class
    @details EtherCAT slave device의 공통적인 data transaction 기능을 모사한다.
    Slave device 모델을 객체화 할 수 있도록 각 slave 별 PDO data 영역과 MailBox 데이터를 관리한다.
    */
    class ECatSlaveModel{
    public:
        ECatSlaveModel() = default;
        void set(const string& name, int32_t type, int32_t vendor, int32_t prod, uint32_t physaddr, void* rxpdo, int rx_bit_offset, int rx_bit_len, void* txpdo, int tx_bit_offset, int tx_bit_len);
        void registerPdoEntry(bool rx, int index, int subindex, int bitlen, int bitoffset, const std::string& name);
        void registerSdoEntry(int index, int subindex, CoeValue::Type type, const std::string& name, uint32_t initial_value = 0);
        void putMailboxData(MailBoxData& mbx);
        CoeValue::Type coeEntryType(int index, int subindex);
        virtual void initialize() {}; // called from set()
        virtual bool loadModelData(const string& model_file, const char* model_name);
        virtual void setPdoValue(const string& objname, CoeValue& val){ throw std::runtime_error("Not implemented");}
        virtual void processPDO() {/* implement model specific PDO processing routine here */}
        virtual bool handleMailBox(MailBoxData& mbox);
        virtual void processSDO();
        virtual void update();

        virtual void printStatus();
        void dumpObjectDictionary();
        const char* getModelName() { return name_.c_str(); }
        const char* getSlaveTypeName();
        void setId(uint32_t id) {id_ = id;}
        uint32_t id() const { return id_;}
        int32_t  type() const { return type_;}
        CoeValueEx* findSdoByName(const string& objname);
        CoeValueEx* findSdoByObjId(int index, int subindex = 0);
    public:
        bool getSlaveInfo(ECatSlaveInfo* slaveinfo);
        static int32_t getSlaveType(uint32_t vendor_id, uint32_t product_code);
    protected:
        // slave information
        int id_;
        string name_;
        int32_t type_;
        int32_t vendor_id_;
        int32_t prod_code_;
        uint32_t addr_; // slave physical address
        uint8_t* rxpdo_; // master --> slave. outpdo대응. 코딩 시 유의
        uint8_t* txpdo_; // slave --> master. inpdo대응. 코딩 시 유의
        int rx_bit_offset_;
        int rx_bit_length_;
        int tx_bit_offset_;
        int tx_bit_length_;
        // mailbox data
        std::list<MailBoxData*> mbox_;
        std::map<uint32_t, PdoEntry> pdo_map_; // key = index<<16+subindex
        std::map<uint32_t, CoeValueEx> sdo_map_; // key = index<<16+subindex
    };

    /// @}
}
#endif //SEPMASTER_ECAT_SLAVE_H
