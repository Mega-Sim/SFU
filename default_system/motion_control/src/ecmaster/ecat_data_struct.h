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
// Created by master on 17. 3. 29.
//

#ifndef SEPMASTER_ECAT_DATA_STRUCT_H_H
#define SEPMASTER_ECAT_DATA_STRUCT_H_H
#include "ecmaster_config.h"
namespace sephi {
    /**\ingroup grp_ecmaster
     * \defgroup grp_ecat_data EtherCAT service data structures
     * \brief EtherCAT service 이용에 사용되는 데이터 구조들
     * \{
     */
    // type definition
    typedef int32_t pos_unit_t; // 서보 position 지령 타입
    typedef int32_t vel_unit_t; // 서보 velocity unit
    typedef uint32_t acc_unit_t; // profile acc unit type
    typedef int16_t trq_unit_t; // 서보 torque unit

    struct PosUnit {
        union {
            int64_t val;
            int32_t i32val[2];
        };
        PosUnit():val(0){}
        PosUnit(int64_t val64) : val(val64){}
        PosUnit(int32_t h, int32_t l){i32val[0] = l; i32val[1] = h;}
        int32_t& low() { return i32val[0]; } // \note little endian only
        int32_t& high() { return i32val[1]; }
    };

    struct ECatResource {
        enum Type { SERVO, DIO, SERIAL };
        Type type;      ///< resource type
        pid_t user_pid; ///< resource user process's pid
        /// todo: pointer to resource
    };

    enum class ControlTarget : int {
        POS,
        VEL,
        ACC
    };

    enum class MotorDirection : int {
        CW,
        CCW,
        UNKNOWN
    };

    enum class EncoderType : int {
        ABS_ENC,    // absolute encoder
        INC_ENC,    // incremental encoder
        MT_INC_ENC, // multi-turn incremental encoder
    };

    enum class PowerType : int {
        DC_270V,
        AC_100V,
        AC_200V,
        AC3_200V,
        UNKNOWN,
    };

    enum class ServoOpMode : int8_t {
        NO_MODE = 0,    ///< no mode change/no mode assigned
        PP = 1,         ///< profile position mode
        PV = 3,         ///< profile velocity mode
        PT = 4,         ///< profile torque mode
        HOME = 6,       ///< homing mode
        IP = 7,         ///< interpolated position mode
        CSP = 8,        ///< cyclic sync. position mode
        CSV = 9,        ///< cyclic sync. velocity mode
        CST = 10,       ///< cyclic sync. torque mode
    };

    enum class ECatState {
        INIT,           ///< init mode
        PRE_OP,         ///< pre-operational mode
        SAFE_OP,        ///< safe-operational mode
        OP              ///< operational mode
    };


    enum class ServoControlState {
        NONE,           ///< not ready. initial state
        FAULT_ACTIVE,   ///< fault reaction active state; 제어전원 ON, 서보 ON
        FAULT,          ///< 제어 전원 ON, 서보 OFF
        SW_ON_DISABLED, ///< switch on disabled
        SW_ON_READY,    ///< ready to switch on
        SW_ON,          ///< switch on. 서보 OFF
        OP,             ///< operation enabled, 서보 ON
        QSTOP_ACTIVE,   ///< quick stop active, 서보 ON
    };


    struct ECatDIO {
        uint32_t id;
    };

    struct ECatSerial {
        uint32_t id;
    };

    struct ECatPdoEntry {
        uint16_t index;
        uint8_t subindex;
        uint8_t bit_lenth;
        uint32_t val()
        {
            return  (index << 16) + (subindex << 8) + bit_lenth;
        }
    };

    struct CoeValue {
        enum Type : int {
            UNKNOWN = -1,
            SINT8   = 0,
            UINT8   = 1,
            SINT16  = 2,
            UINT16  = 3,
            SINT32  = 4,
            UINT32  = 5,
            ARRAY   = 6,
            BOOL    = 7,
        };
        int dataLength() const {
            switch (type) {
                case UNKNOWN: return 0;
                case BOOL:
                case SINT8:
                case UINT8: return 1;
                case SINT16:
                case UINT16: return 2;
                case SINT32:
                case UINT32: return 4;
                case ARRAY:  return 16; // 16byte array
            }
        }
        static Type getType(const std::string& name){
            if (name == "BOOL") return BOOL;
            if (name == "SINT") return SINT8;
            if (name == "USINT") return UINT8;
            if (name == "INT") return SINT16;
            if (name == "UINT") return UINT16;
            if (name == "DINT") return SINT32;
            if (name == "UDINT") return UINT32;
            if (name == "ARRAY") return ARRAY;
            return UNKNOWN;
        }
    public:
        union {
            int8_t i8;
            int16_t i16;
            int32_t i32;
            uint8_t u8;
            uint16_t u16;
            uint32_t u32;
            uint8_t arr[16];
        };
        Type type;
    };

    // SDO message structure
    struct ECatSdoMsg {
        struct SdoMsg {
            int      timeout;   // timeout ms
            uint16_t index;     // coe index
            uint16_t subindex;  // coe subindex
            uint16_t length;    // payload length
            uint8_t data[EM_MSG_Q_DATA_SIZE]; // payload
        };
        struct RscRequestMsg {
            pid_t pid;
        };
        struct RscResponseMsg {
            uint32_t in_pd_offset;
            uint32_t out_pd_offset;
            uint32_t bus_cycle_us;
        };
        enum Command {
            REQUEST_SLAVE   = 0x0000,
            RELEASE_SLAVE,
            SDO_DOWNLOAD,
            SDO_UPLOAD,
            SET_BITS,
            CLEAR_BITS,
            SET_MASTER_AXIS,
            RECOVER_OP_MODE,
            REMOVE_HANDLE,
            RESPONSE = 0x100,
            RESPONSE_REQUEST_SLAVE = 0x100,
            RESPONSE_RELEASE_SLAVE,
            RESPONSE_SDO_DOWNLOAD,
            RESPONSE_SDO_UPLOAD,
            RESPONSE_SET_BITS,
            RESPONSE_CLEAR_BITS,
            RESPONSE_SET_VELOCITY_MASTER_AXIS,
        };
        uint32_t command;
        int32_t slave_id;
        union {
            SdoMsg sdo;
            RscRequestMsg req;
            RscResponseMsg res;
            int result;
            void* handle;
            uint16_t word_bits;
            int master_axis;
        };

    public:
        ECatSdoMsg(){sdo.timeout = COE_SDO_TIMEOUT_MS;}
        ECatSdoMsg(Command _command): command(_command){sdo.timeout = COE_SDO_TIMEOUT_MS;}
        ECatSdoMsg(Command _command, uint32_t _slave_id): command(_command), slave_id(_slave_id){sdo.timeout = COE_SDO_TIMEOUT_MS;}
    };

    // SERVO -> ATLAS
    struct Cia402PdoIn {
        uint16_t status;
        trq_unit_t torque;
        vel_unit_t velocity;
        pos_unit_t position;
        pos_unit_t following_error;
        uint16_t error_code;
        uint8_t  op_mode;
    } __attribute__((packed));

    // ATLAS -> SERVO
    struct Cia402PdoOut {
        uint16_t control;
        uint8_t setopmode;
        trq_unit_t torque;
        vel_unit_t velocity;
        pos_unit_t position;
    } __attribute__((packed));

    template <int analog_port, int digital_port>
    struct ECatIoInPdo {
        uint16_t  ain[analog_port];
        uint8_t   din[digital_port];
    } __attribute__((packed));

    template <int analog_port, int digital_port>
    struct ECatIoOutPdo {
        uint16_t  aout[analog_port];
        uint8_t   dout[digital_port];
    } __attribute__((packed));

    typedef ECatIoInPdo<4,6> StockerIoPdoIn;
    typedef ECatIoOutPdo<4,5> StockerIoPdoOut;
    typedef ECatIoInPdo<5,4> OhtFrontIoPdoIn;
    typedef ECatIoOutPdo<10,6> OhtFrontIoPdoOut;
    typedef ECatIoInPdo<5,4> OhtOnIoPdoIn;
    typedef ECatIoOutPdo<10,6> OhtOnIoPdoOut;

    typedef ECatIoInPdo<5,6> OhtRearIoPdoIn;
    typedef ECatIoOutPdo<10,6> OhtRearIoPdoOut;
    typedef SepMsgQ<ECatSdoMsg, EM_SLAVE_MSG_Q_SIZE> ECatSlaveMsgQ;

    /// \}

    struct IMultiportSerialInterface {
        IMultiportSerialInterface() = default;
        virtual int send(int portid, const void* data, int length) = 0;
        virtual int receive(int portid, void* data, int length) = 0;
    };

    // serial port
    struct PortData {
        enum {
            //BUFFER_SIZE = 4 // 115200bps =  11.52 * (1 start bit + 8 data bit + 1 stop bit) * 1000 ms --> 초당 11.52byte consume 가능 ==> 11byte for data, 1byte for length
            BUFFER_SIZE = 15     // yjchoe, 중간 데이터 손실로 인해 변경
        };
        int length;
        int8_t data[BUFFER_SIZE];

        PortData() : length(0) {}

        void clear() { length = 0; }

        int put(void *in, int size) {
            if (size < 0) return -1;
            auto copysize = (size + length <= BUFFER_SIZE) ? size : BUFFER_SIZE - length;
            memcpy(data+length, in, copysize);
            length += copysize;
            return copysize;
        }
        int size() const { return length; }
        int remains() const { return BUFFER_SIZE - length; }
    };


    struct ECatSerialData {
        enum {
            SERIAL_PORT_COUNT_PER_SLAVE = 8, ///  \todo make flexible
            OHT_FRONT_SERIAL_COUNT = SERIAL_PORT_COUNT_PER_SLAVE,
            OHT_REAR_SERIAL_COUNT = SERIAL_PORT_COUNT_PER_SLAVE, 
            ECAT_SERIAL_PAYLOAD_LENGTH = 15, // \todo 11.52k limit이면 11로 줄여야...(pdo 구조도 바뀌어야 함.)
            QUEUE_BUFFER_SIZE   = 512,
            MAX_SERIAL_OVER_ECAT_PACKET_SIZE = PortData::BUFFER_SIZE - 1,
        };

        uint8_t pkt_cnt[SERIAL_PORT_COUNT_PER_SLAVE];
        uint8_t pkt_cnt_echo[SERIAL_PORT_COUNT_PER_SLAVE];
        struct Data {
            uint8_t length;
            uint8_t payload[ECAT_SERIAL_PAYLOAD_LENGTH];
        } __attribute__((packed)) data[SERIAL_PORT_COUNT_PER_SLAVE];
    }__attribute__((packed));

};
#endif //SEPMASTER_ECAT_DATA_STRUCT_H_H
