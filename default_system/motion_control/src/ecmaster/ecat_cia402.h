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
// Created by master on 17. 4. 11.
//

#ifndef SEPMASTER_ECAT_CIA402_H
#define SEPMASTER_ECAT_CIA402_H
#include <bitset>         // std::bitset
#include <array>
#include <tuple>
#include "ecat_data_struct.h"


namespace sephi {
    /**\ingroup grp_ecmaster
     * \defgroup grp_cia402 CiA402 data structures
     * \brief CiA402 device profile 지원을 위한 구조체들
     * \{
     */
    struct Cia402ControlWord {
        Cia402ControlWord() = default;
        Cia402ControlWord(uint16_t cmd): command(cmd){}
        uint16_t command;
        enum COMMAND : uint16_t {
            SHUTDOWN = 0x06,
            SWITCH_ON = 0x07,
            FAULT_RESET = 0x80,
            ENABLE_OP = 0x0f,
            DISABLE_OP = 0x0e,
            QUICK_STOP = 0x02,
        };
        bool getBit(int bit)
        {
            return bool(command & (1<<bit));
        }
        void setBit(int bit, bool val) {
            if (val){
                command |= (1<<bit);
            } else {
                command &= ~(1<<bit);
            }
        }
        const char* commandString()
        {
            switch(command) {
                case SHUTDOWN: return "SHUTDOWN";
                case SWITCH_ON: return "SWITCH_ON";
                case FAULT_RESET: return "FAULT_RESET";
                case ENABLE_OP: return "ENABLE_OP";
                case DISABLE_OP: return "DISABLE_OP";
                case QUICK_STOP: return "QUICK_STOP";
                default: break;
            }
           return "UNKNOWN";
        }
    };

    struct Cia402StatusWord {
        Cia402StatusWord() = default;
        Cia402StatusWord(uint16_t st){ *(uint16_t*)this = st;}

        uint16_t ready_to_switch_on:1;     // # 0
        uint16_t switched_on:1;            // # 1
        uint16_t operation_enabled:1;      // # 2
        uint16_t fault:1;                  // # 3

        uint16_t voltage_enabled:1;        // # 4
        uint16_t quick_stop:1;             // # 5
        uint16_t switch_on_disabled:1;     // # 6
        uint16_t warning:1;                // # 7

        uint16_t manufacturer_specific_1:1;// # 8
        uint16_t remote:1;                 // # 9
        uint16_t target_reached:1;         // # 10 CSP Target Position Ignored
        uint16_t internal_limit_active:1;  // # 11

        uint16_t fault_over_temperature:1; // # 12 PP set point acknowledged
        uint16_t operation_mode_specific:1;// # 13 CSP Following Error
        uint16_t fault_over_current:1;     // # 14
        uint16_t fault_under_voltage:1;    // # 15


        enum STATE :int {
            NOT_READY_TO_SWITCH_ON,
            SWITCH_ON_DISABLED,
            READY_TO_SWITCH_ON,
            SWITCHED_ON,
            OPERATION_ENABLED,
            QUICK_STOP,
            FAULT_REACTION_ACTIVE,
            FAULT,
            FAULT_OVER_VOLTAGE,
            FAULT_OVER_TEMPERATURE,
            FAULT_OVER_CURRENT,
            FAULT_UNDER_VOLTAGE,
            UNKNOWN_STATE
        };

        operator int() { return *(uint16_t*)this; }
        uint16_t& ref(){ return *(uint16_t*)this;}
        bool getBit(int bit)
        {
            return bool(*((uint16_t*)this) & (1<<bit));
        }
        void setBit(int bit, bool val) {
            uint16_t& bits = *(uint16_t*)this;
            if (val){
                bits |= (1<<bit);
            } else {
                bits &= ~(1<<bit);
            }
        }

        void setState(STATE st)
        {
            switch (st) {
                case NOT_READY_TO_SWITCH_ON:
                    ready_to_switch_on = 0;
                    switched_on = 0;
                    operation_enabled = 0;
                    fault = 0;
                    switch_on_disabled = 0;
                    break;
                case SWITCH_ON_DISABLED:
                    ready_to_switch_on = 0;
                    operation_enabled = 0;
                    switched_on = 0;
                    fault  = 0;
                    switch_on_disabled = 1;
                    break;
                case READY_TO_SWITCH_ON:
                    switched_on = 0;
                    operation_enabled = 0;
                    fault = 0;
                    switch_on_disabled = 0;
                    quick_stop = 1;
                    ready_to_switch_on = 1;
                    break;
                case SWITCHED_ON:
                    operation_enabled = 0;
                    switch_on_disabled = 0;
                    fault = 0;
                    quick_stop = 1;
                    ready_to_switch_on = 1;
                    switched_on = 1;
                    break;
                case OPERATION_ENABLED:
                    fault = 0;
                    switch_on_disabled = 0;
                    ready_to_switch_on = 1;
                    switched_on = 1;
                    operation_enabled = 1;
                    quick_stop = 1;
                    break;
                case QUICK_STOP:
                    fault = 0;
                    switch_on_disabled = 0;
                    quick_stop = 0;
                    ready_to_switch_on = 1;
                    switched_on = 1;
                    operation_enabled = 1;
                    break;
                case FAULT_REACTION_ACTIVE:
                    switch_on_disabled = 0;
                    ready_to_switch_on = 1;
                    switched_on = 1;
                    operation_enabled = 1;
                    fault = 1;
                    break;
                case FAULT:
                    switch_on_disabled = 0;
                    ready_to_switch_on = 0;
                    switched_on = 0;
                    operation_enabled = 0;
                    fault = 1;
                    break;
                case FAULT_OVER_VOLTAGE:
                    fault_over_current = 1;
                    fault_under_voltage = 1;
                    switch_on_disabled = 0;
                    ready_to_switch_on = 0;
                    switched_on = 0;
                    operation_enabled = 0;
                    fault = 1;
                    break;
                case FAULT_OVER_TEMPERATURE:
                    fault_over_temperature = 1;
                    switch_on_disabled = 0;
                    ready_to_switch_on = 0;
                    switched_on = 0;
                    operation_enabled = 0;
                    fault = 1;
                    break;
                case FAULT_OVER_CURRENT:
                    fault_over_current = 0;
                    switch_on_disabled = 0;
                    ready_to_switch_on = 0;
                    switched_on = 0;
                    operation_enabled = 0;
                    fault = 1;
                    break;
                case FAULT_UNDER_VOLTAGE:
                    fault_under_voltage = 0;
                    switch_on_disabled = 0;
                    ready_to_switch_on = 0;
                    switched_on = 0;
                    operation_enabled = 0;
                    fault = 1;
                    break;
            }
        }


        STATE getState()
        {
            if (!ready_to_switch_on &&
                !switched_on &&
                !operation_enabled &&
                !fault &&
                !switch_on_disabled)
                return NOT_READY_TO_SWITCH_ON;
            else if (!ready_to_switch_on &&
                     !operation_enabled &&
                     !switched_on &&
                     !fault &&
                     switch_on_disabled)
                return SWITCH_ON_DISABLED;
            else if (!switched_on &&
                     !operation_enabled &&
                     !fault &&
                     !switch_on_disabled &&
                     quick_stop &&
                     ready_to_switch_on)
                return READY_TO_SWITCH_ON;
            else if (!operation_enabled &&
                     !switch_on_disabled &&
                     !fault &&
                     quick_stop &&
                     ready_to_switch_on &&
                     switched_on)
                return SWITCHED_ON;
            else if (!fault &&
                     !switch_on_disabled &&
                     ready_to_switch_on &&
                     switched_on &&
                     operation_enabled &&
                     quick_stop)
                return OPERATION_ENABLED;
            else if (!fault &&
                     !switch_on_disabled &&
                     !quick_stop &&
                     ready_to_switch_on &&
                     switched_on &&
                     operation_enabled)
                return QUICK_STOP;
            else if (!switch_on_disabled &&
                     ready_to_switch_on &&
                     switched_on &&
                     operation_enabled &&
                     fault)
                return FAULT_REACTION_ACTIVE;
            else if (!switch_on_disabled &&
                     !ready_to_switch_on &&
                     !switched_on &&
                     !operation_enabled &&
                     fault)
                return FAULT;
            else if (fault &&
                     fault_over_current &&
                     fault_under_voltage)
                return FAULT_OVER_VOLTAGE;
            else if (fault &&
                     fault_over_temperature)
                return FAULT_OVER_TEMPERATURE;
            else if (fault &&
                     fault_over_current)
                return FAULT_OVER_CURRENT;
            else if (fault &&
                     fault_under_voltage)
                return FAULT_UNDER_VOLTAGE;

            else {
                printf("UNKNOWN_STATE : /%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/",ready_to_switch_on,switched_on,operation_enabled, fault, voltage_enabled, quick_stop,switch_on_disabled,warning, manufacturer_specific_1,
                       remote,target_reached,internal_limit_active,fault_over_temperature,operation_mode_specific,  fault_over_current, fault_under_voltage );
                /*
                sw |= ready_to_switch_on<<0;
                sw |= switched_on<<1;
                sw |= operation_enabled<<2;
                sw |= 1<<3;
                sw |= voltage_enabled<<4;
                sw |= quick_stop<<5;
                sw |= switch_on_disabled<<6;
                sw |= warning<<7;
                sw |= manufacturer_specific_1<<8;
                sw |= remote<<9;
                sw |= target_reached<<10;
                sw |= internal_limit_active<<11;
                sw |= fault_over_temperature<<12;
                sw |= operation_mode_specific<<13;
                sw |= fault_over_current<<14;
                sw |= fault_under_voltage<<15;
                sw |= fault<<16;

                return sw;*/
                return UNKNOWN_STATE;
            }
        }

        inline const char* getStateString(int _state = -1)
        {
            static std::array<const char *, UNKNOWN_STATE + 1> desc = {
                    "NOT_READY_TO_SWITCH_ON",
                    "SWITCH_ON_DISABLED",
                    "READY_TO_SWITCH_ON",
                    "SWITCHED_ON",
                    "OPERATION_ENABLED",
                    "QUICK_STOP",
                    "FAULT_REACTION_ACTIVE",
                    "FAULT",
                    "FAULT_OVER_VOLTAGE",
                    "FAULT_OVER_TEMPERATURE",
                    "FAULT_OVER_CURRENT",
                    "FAULT_UNDER_VOLTAGE",
                    "UNKNOWN_STATE"
            };
            int state = (_state<0) ? getState() : _state;
            if (state < 0  || state >= UNKNOWN_STATE) state = UNKNOWN_STATE;
            return desc[state];
        }

        inline static const char* getString(int bit)
        {
            static std::array<const char *, 16> desc = {
                    "ready_to_switch_on",      // # 0
                    "switched_on",             // # 1
                    "operation_enabled",       // # 2
                    "fault",                   // # 3
                    "voltage_enabled",         // # 4
                    "quick_stop",              // # 5
                    "switch_on_disabled",      // # 6
                    "warning",                 // # 7
                    "manufacturer_specific_1", // # 8
                    "remote",                  // # 9
                    "target_reached",          // # 10 CSP Target Position Ignored
                    "internal_limit_active",   // # 11
                    "fault_over_temperature",  // # 12
                    "operation_mode_specific", // # 13 CSP Following Error
                    "fault_over_current",      // # 14
                    "fault_under_voltage",     // # 15
            };
            return desc[bit];
        }

    };

    enum COE_DICT_INDEX : uint16_t {
        COE_STORE_PARAM  = 0x1010,  // 1: save all, 2: save comm., 3: save app., 4: save manufacturer.
        COE_RESTORE_PARAM= 0x1011,  // COE_STORE_PARAM과 동일
        COE_SM_RX_PDO_ASSIGN = 0x1C12, // 초기값 x1601
        COE_SM_TX_PDO_ASSIGN = 0x1C13, // 초기값 x1A01

        COE_SERVO_PARAM     = 0x2000, // 0x2000~0x26ff
        COE_POWER_DC        = 0x2001, // dXddd
        COE_POWER_3P        = 0x200B, // 0Xddd
        COE_USER_PARAM_CONFIG   = 0x2700,
        COE_POS_UNIT        = 0x2701,
        COE_VEL_UNIT        = 0x2702,
        COE_ACC_UNIT        = 0x2703,
        COE_TRQ_UNIT        = 0x2704,
        COE_SERVO_ADJUST    = 0x2710,

        COE_QSTOP_OPTION    = 0x605A,   // 0~4; 2
        COE_SHUTDOWN_OPTION = 0x605B,// 0~1; 0
        COE_DISABLE_OPTION  = 0x605C, // 0~1; 1
        COE_HALT_OPTION     = 0x605D,  // 0~4; 1
        COE_MODE_OF_OPERATION = 0x6060,// 0~10; 0
        COE_MODE_OF_OPERATION_DISP = 0x6061,
        COE_POS_DEMAND   = 0x6062,   // pos unit
        COE_POS_ACTUAL   = 0x6064,   // pos unit
        COE_FOLLOWING_ERROR_WINDOW = 0x6065, // pos unit
        COE_POS_WINDOW   = 0x6067,   // pos unit; 30
        COE_VEL_DEMAND   = 0x606B,   // vel unit
        COE_VEL_ACTUAL   = 0x606C,   // vel unit
        COE_VEL_WINDOW   = 0x606D,   // vel unit
        COE_TRQ_TARGET   = 0x6071,   // trq unit
        COE_TRQ_MAX      = 0x6072,   // trq unit
        COE_TRQ_DEMAND   = 0x6074,   // trq unit
        COE_RATED_TRQ    = 0x6076,   // mN
        COE_TRQ_ACTUAL   = 0x6077,   // trq unit
        COE_POS_TARGET   = 0x607A,   // pos unit
        COE_HOME_OFFSET  = 0x607C,   // pos unit
        COE_POS_LIMIT    = 0x607D,   // pos unit.  1: min, 2: max
        COE_PROF_VEL_MAX = 0x607F,   // vel unit,
        COE_PROF_VEL     = 0x6081,   // vel unit
        COE_PROF_ACC     = 0x6083,   // acc unit
        COE_PROF_DEC     = 0x6084,   // acc unit
        COE_QSTOP_DEC    = 0x6085,   // acc unit
        COE_TRQ_SLOPE    = 0x6087,   // trq unit / s
        COE_HOME_METHOD  = 0x6098,   // 0~35; 35
        COE_VEL_OFFSET   = 0x60B1,   // vel unit
        COE_TRQ_OFFSET   = 0x60B2,   // TRQ UNIT, -2^15~2^15
        COE_TOUCH_FUNC   = 0x60B8,   // 0~0xFFFF; 0
        COE_FOLLOWING_ERROR_ACTUAL = 0x60F4, // pos unit
        COE_DIGITAL_INP  = 0x60FD, // UDINT
        COE_DIGITAL_OUT  = 0x60FE, // 1: output data, 2: bitmask
        COE_VEL_TARGET   = 0x60FF, // vel unit
    };

    enum COE_STORE_OPTIONS : uint8_t {
        COE_SAVE_ALL_PARAM = 1,
        COE_SAVE_COMM_PARAM = 2,    // 0x1000~0x1FFF
        COE_SAVE_APP_PARAM = 3,     // 0x2700~0x6FFF
        COE_SAVE_SERVO_PARAM = 4,   // 0x2000~0x26FF
        COE_RESTORE_ALL_PARAM = 1,
        COE_RESTORE_COMM_PARAM = 2,    // 0x1000~0x1FFF
        COE_RESTORE_APP_PARAM = 3,     // 0x2700~0x6FFF
        COE_RESTORE_SERVO_PARAM = 4,   // 0x2000~0x26FF
    };

    ///\}
}

#endif //SEPMASTER_ECAT_CIA402_H
