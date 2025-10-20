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
#include "ecmaster_typedef.h"
#include "ecat_cia402_servo_test.h"
#include "emul/ecat_emulator.h"
#ifdef USE_ETHERLAB
#include "slave_pdos.h"
#endif
namespace sephi
{

    CiA402Servo::CiA402Servo(int client_id, int32_t slave_id, uint8_t* in_pdo, uint8_t* out_pdo, int bus_cycle_us)
            : ec_client_id_(client_id), slave_id_(slave_id), in_pdo_(in_pdo), out_pdo_(out_pdo),
              bus_cycle_us_(bus_cycle_us), state_(Cia402StatusWord::NOT_READY_TO_SWITCH_ON)
    {
        prev_errorcode = 0;
        // motion param
        prof_velocity_ = 1 << 21;

        SEP_LOGT("servo[%d] out_pdo = %p, in_pdo = %p\n", slave_id, out_pdo_, in_pdo_);
    }

    bool CiA402Servo::waitForStateReady(int state1)
    {
        Cia402StatusWord *stat = (Cia402StatusWord*)&state1;
        SEP_LOGT("waiting for new state %s\n",  stat->getStateString(state1));
        while(true) {
            auto state = statusWord().getState();
            if (state == state1  ) {
                return true;
            }
            if (statusWord().fault) {
                SEP_LOGE("FALT: %s\n", statusWord().getStateString());
                return false;
            }
            usleep(bus_cycle_us_);
        }
    }

    bool CiA402Servo::clearFault()
    {
        int timeout_us = COE_SDO_TIMEOUT_MS * 1000;
        if (statusWord().fault) {
            SEP_LOGT("tring to FAULT_RESET\n");
        }
        controlWord().command &= ~(Cia402ControlWord::FAULT_RESET);
        usleep(bus_cycle_us_*100);
        while(statusWord().fault && timeout_us > 0) {
            controlWord().command = Cia402ControlWord::FAULT_RESET;
            usleep(bus_cycle_us_*10);
            timeout_us -= bus_cycle_us_;
        }

        return !statusWord().fault;
    }

    bool CiA402Servo::initServo(ServoOpMode op_mode)
    {
        setOperationMode(op_mode);
        usleep(bus_cycle_us_*2);
        return enableServo();
    }

    void CiA402Servo::followActual()
    {
        outPdo().position = inPdo().position;
        outPdo().velocity = 0;
        outPdo().torque = 0;
    }

    bool CiA402Servo::enableServo()
    {
        auto printState = [this]() {
            std::bitset<16> b(*(uint16_t *)in_pdo_);
            SEP_LOGT("Servo[%d].state = %s; 0x%04x 0b%s\n", slave_id_, statusWord().getStateString(), statusWord().ref(), b.to_string().c_str());
        };

        state_ = statusWord().getState();
        printState();

        if (state_ == Cia402StatusWord::OPERATION_ENABLED) {
            return true;
        }

        followActual();

        // clear fault
        SEP_REQUIRE_RET(clearFault(), false);



        state_ = statusWord().getState();
        printState();

        //SEP_LOGD("Test Conmmand... ")

        if (state_ != Cia402StatusWord::SWITCHED_ON) {
            if (state_ != Cia402StatusWord::READY_TO_SWITCH_ON) {
                // Wait for status change
                SEP_REQUIRE_RET(waitForStateReady(Cia402StatusWord::SWITCH_ON_DISABLED), false);
                // Shutdown
                controlWord().command = Cia402ControlWord::SHUTDOWN;
                SEP_REQUIRE_RET(waitForStateReady(Cia402StatusWord::READY_TO_SWITCH_ON), false);

                // Switch on
                controlWord().command = Cia402ControlWord::SWITCH_ON;

                // Note: Automatic transition to Enable Operation state after executing SWITCHED ON state functionality.
                SEP_REQUIRE_RET(waitForStateReady(Cia402StatusWord::SWITCHED_ON), false);
            }
            state_ = statusWord().getState();
        }

        // Set operation enabled
        controlWord().command = Cia402ControlWord::ENABLE_OP;
        SEP_LOGT("Wait for state to be OPERATION_ENABLED\n");
        SEP_REQUIRE_RET(waitForStateReady(Cia402StatusWord::OPERATION_ENABLED), false);
        printState();

        return true;
    }

    bool CiA402Servo::setProfilePositionParam(uint32_t prof_velocity, uint32_t prof_acc, uint32_t prof_dec)
    {
        //prof_velocity_ = prof_velocity;
        setCoeSdoValue(COE_PROF_VEL, 0, prof_velocity);
        setCoeSdoValue(COE_PROF_VEL_MAX, 0, prof_velocity);
        setCoeSdoValue(COE_PROF_ACC, 0, prof_acc);
        setCoeSdoValue(COE_PROF_DEC, 0, prof_dec);
        setCoeSdoValue(COE_QSTOP_DEC, 0, prof_dec*2);
        if (prof_acc != 0xFFFFFFFF) {
            prof_acc_ = prof_acc;
            //setCoeSdoValue16(COE_PROF_ACC, 0, prof_acc);
        }
        if (prof_dec != 0xFFFFFFFF) {
            prof_dec_ = prof_dec;
            //setCoeSdoValue16(COE_PROF_DEC, 0, prof_dec);
        }
        return true;
    }

    bool CiA402Servo::setProfilePosition(int32_t target)
    {

        outPdo().position = target;
        setProfilePositionParam(40000, 10000, 10000);
        //outPdo().prof_velocity = prof_velocity_;
        controlWord().command |= (1 << 4);

        int timeout_us = COE_SDO_TIMEOUT_MS * 1000;
        const int PP_TARGET_ACKNOWLEDGE_BIT = 12;
        while(!statusWord().getBit(PP_TARGET_ACKNOWLEDGE_BIT) && timeout_us > 0) {
            usleep(bus_cycle_us_);
            timeout_us -= bus_cycle_us_;
        }
        SEP_LOGT("acknowledge = %d\n", bool(statusWord().getBit(PP_TARGET_ACKNOWLEDGE_BIT)));
        controlWord().command &= ~(1 << 4);
        statusWord().target_reached = 0; // in_pdo 수신 전에 체크가 일어 날 수 있으므로 clear해 줌
        return statusWord().getBit(PP_TARGET_ACKNOWLEDGE_BIT);
    }

    bool CiA402Servo::getStatusBit(int bit)
    {
        return bool(statusWord().getBit(bit));
    }

    bool CiA402Servo::targetReached()
    {
        return statusWord().target_reached;
    }

    bool CiA402Servo::quickStop()
    {
        controlWord().command = Cia402ControlWord::QUICK_STOP;
        return true;
    }
    bool CiA402Servo::shutdown()
    {
        controlWord().command = Cia402ControlWord::SHUTDOWN;
        return true;
    }

    bool CiA402Servo::setOperationMode(ServoOpMode newmode)
    {
        uint8_t op_mode = static_cast<uint8_t>(newmode);
        //return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, COE_MODE_OF_OPERATION,0, &op_mode, 1, COE_SDO_TIMEOUT_MS, 0 );
        outPdo().setopmode = op_mode;   // 210201 yjchoe, pdo set operation mode
        return true;
    }

    int  CiA402Servo::getOperationMode()
    {
        return inPdo().op_mode;
    }

#if defined(USE_ACONTIS)
    int getCoeEntryDescription(int client_id, int slave_id, uint16_t index, uint8_t subindex, EC_T_COE_ENTRYDESC& entrydesc)
    {
        uint8_t byValueInfoType = EC_COE_ENTRY_ObjAccess
                                  | EC_COE_ENTRY_ObjCategory
                                  | EC_COE_ENTRY_PdoMapping
                                  | EC_COE_ENTRY_UnitType
                                  | EC_COE_ENTRY_DefaultValue
                                  | EC_COE_ENTRY_MinValue
                                  | EC_COE_ENTRY_MaxValue;

        const int CROD_ENTRYDESC_SIZE = 100;
        static uint32_t uniqueTransferId = 0;

        /* mailbox transfer object for Entry description upload */
        EC_T_MBXTFER_DESC   mbxTferDesc             = {0};
        mbxTferDesc.dwMaxDataLen        = CROD_ENTRYDESC_SIZE;
        mbxTferDesc.pbyMbxTferDescData  = (EC_T_BYTE*)&entrydesc;

        EC_T_MBXTFER* mbxBuffer = ecatMbxTferCreate(&mbxTferDesc);
        SEP_REQUIRE_RET(EC_NULL != mbxBuffer, EC_E_NOMEMORY);

        mbxBuffer->dwClntId     = client_id;
        mbxBuffer->dwDataLen    = mbxBuffer->MbxTferDesc.dwMaxDataLen;
        mbxBuffer->dwTferId     = uniqueTransferId++;

        auto res = ecatCoeGetEntryDesc(mbxBuffer, slave_id, index, subindex, byValueInfoType, COE_SDO_TIMEOUT_MS);
        ecatMbxTferDelete(mbxBuffer);
        return res;
    }
#endif // USE_ACONTIS



    CoeValue::Type CiA402Servo::getCoeEntryType(uint16_t index, uint8_t subindex)
    {
        if (the_ec_emulator.get()->emulationMode()) { // emulation mode hooking
            the_ec_emulator.get()->coeEntryType(slave_id_, index, subindex);
        }
#if defined(USE_ACONTIS)
        EC_T_COE_ENTRYDESC entrydesc;
        SEP_REQUIRE_RET(EC_E_NOERROR == getCoeEntryDescription(ec_client_id_, slave_id_, index, subindex, entrydesc), CoeValue::UNKNOWN);
        switch (entrydesc.wDataType) {
            case DEFTYPE_BOOLEAN:
            case DEFTYPE_BIT1:
            case DEFTYPE_BIT2:
            case DEFTYPE_BIT3:
            case DEFTYPE_BIT4:
            case DEFTYPE_BIT5:
            case DEFTYPE_BIT6:
            case DEFTYPE_BIT7:
            case DEFTYPE_BIT8:
            case DEFTYPE_UNSIGNED8: return CoeValue::UINT8;
            case DEFTYPE_INTEGER8: return CoeValue::SINT8;
            case DEFTYPE_UNSIGNED16: return CoeValue::UINT16;
            case DEFTYPE_INTEGER16: return CoeValue::SINT16;
            case DEFTYPE_UNSIGNED32: return CoeValue::UINT32;
            case DEFTYPE_INTEGER32: return CoeValue::SINT32;
            default:
                SEP_LOGE("Unsupported type %d\n", entrydesc.wDataType);
                return CoeValue::UNKNOWN;
        }
#elif defined(USE_ETHERLAB)
        SdoInfo sdoinfo;
        SEP_REQUIRE_RET(getCia402SdoInfo(index, subindex, sdoinfo), CoeValue::UNKNOWN);
        return sdoinfo.type;
#endif
    }

    CoeValue CiA402Servo::readCoeValue(uint16_t index, uint16_t subindex)
    {
        CoeValue val;
        val.type = getCoeEntryType(index, subindex);
        SEP_REQUIRE_RET(val.type != CoeValue::UNKNOWN, val);

        EcDWord outlen;
        SEP_REQUIRE_RET(EC_E_NOERROR == ecatCoeSdoUploadEx(slave_id_, index, subindex, (uint8_t*)&val.u32, val.dataLength(), &outlen, COE_SDO_TIMEOUT_MS, 0 ), val);
        return val;
    }

    bool CiA402Servo::writeCoeValue(uint16_t index, uint16_t subindex, const CoeValue &val)
    {
        SEP_REQUIRE_RET(val.type != CoeValue::UNKNOWN, false);
        return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, index, subindex, (uint8_t *)&val.u32, val.dataLength(), COE_SDO_TIMEOUT_MS, 0);

    }

    int8_t CiA402Servo::getCoeSdoValue8(uint16_t index, uint16_t subindex)
    {
        int8_t val;
        EcDWord outlen;
        ecatCoeSdoUploadEx(slave_id_, index, subindex, (uint8_t*)&val, sizeof(val), &outlen, COE_SDO_TIMEOUT_MS, 0 );
        return val;
    }

    int8_t CiA402Servo::getCoeSdoValue(int32_t slave_id, uint16_t index, uint16_t subindex)
    {
        int8_t val;
        EcDWord outlen;
        ecatCoeSdoUploadEx(slave_id, index, subindex, (uint8_t*)&val, sizeof(val), &outlen, COE_SDO_TIMEOUT_MS, 0 );
        return val;
    }

    int16_t CiA402Servo::getCoeSdoValue16(uint16_t index, uint16_t subindex)
    {
        int16_t val;
        EcDWord outlen;
        ecatCoeSdoUploadEx(slave_id_, index, subindex, (uint8_t*)&val, sizeof(val), &outlen, COE_SDO_TIMEOUT_MS, 0 );
        return val;
    }

    int32_t CiA402Servo::getCoeSdoValue32(uint16_t index, uint16_t subindex)
    {
        int32_t val;
        EcDWord outlen;
        ecatCoeSdoUploadEx(slave_id_, index, subindex, (uint8_t*)&val, sizeof(val), &outlen, COE_SDO_TIMEOUT_MS, 0 );
        return val;
    }

    bool CiA402Servo::setCoeSdoValue(uint16_t index, uint16_t subindex, int8_t value)
    {
        return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, index, subindex, (uint8_t *)&value, sizeof(value), COE_SDO_TIMEOUT_MS, 0);
    }

    bool CiA402Servo::setCoeSdoValue(uint16_t index, uint16_t subindex, int16_t value)
    {
        return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, index, subindex, (uint8_t *)&value, sizeof(value), COE_SDO_TIMEOUT_MS, 0);
    }

    bool CiA402Servo::setCoeSdoValue(uint16_t index, uint16_t subindex, int32_t value)
    {
        return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, index, subindex, (uint8_t *)&value, sizeof(value), COE_SDO_TIMEOUT_MS, 0);
    }

    bool CiA402Servo::setCoeSdoValue(uint16_t index, uint16_t subindex, uint8_t value)
    {
        return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, index, subindex, (uint8_t *)&value, sizeof(value), COE_SDO_TIMEOUT_MS, 0);
    }

    bool CiA402Servo::setCoeSdoValue(uint16_t index, uint16_t subindex, uint16_t value)
    {
        return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, index, subindex, (uint8_t *)&value, sizeof(value), COE_SDO_TIMEOUT_MS, 0);
    }

    bool CiA402Servo::setCoeSdoValue(uint16_t index, uint16_t subindex, uint32_t value)
    {
        return EC_E_NOERROR == ecatCoeSdoDownloadEx(slave_id_, index, subindex, (uint8_t *)&value, sizeof(value), COE_SDO_TIMEOUT_MS, 0);
    }

    bool CiA402Servo::makeSwitchOnDisabledState()
    {
        SEP_LOGT("Making slave %d into 'Switch on Disabled State' \n", slave_id_);
        // fault 상태라면 fault를 clear하면 됨. 나머지 경우는 quick stop.
        if (statusWord().fault) {
            return clearFault();
        } else {
            //quickStop();
            shutdown();
            return waitForStateReady(Cia402StatusWord::SWITCH_ON_DISABLED);
        }
    }

    bool CiA402Servo::setSlaveEcatState(uint16_t newstate)
    {
#if defined(USE_ACONTIS)
        SEP_LOGT("Trying to change slave(%d) ecat state to %d\n", slave_id_, newstate);
        uint16_t curr_state;
        auto res =  ecatGetSlaveState(slave_id_, &curr_state, &newstate);
        SEP_LOGT("res = %d, current state = %d, new state = %d\n", res, curr_state, newstate);
        return res == EC_E_NOERROR;
#elif defined(USE_ETHERLAB)
        SEP_LOGE("setSlaveEcatState for etherlab stack : Not Implemented!");
        return false;
#endif
    }

    // user parameter 변경 전에 servo를 Switch-on-Disabled상태에서 파라미터를 바꿔야 함.
    bool CiA402Servo::enableUserParameter()
    {
        SEP_LOGT("Enabling User Parameter Configuration on slave %d\n", slave_id_);
        return setCoeSdoValue(COE_USER_PARAM_CONFIG, 0, 1);
    }

    bool CiA402Servo::resetAbsEncoder()
    {
        SEP_LOGT("Reset Abs Encoder on slave %d\n", slave_id_); //COE_SERVO_ADJUST
        typedef struct _Command{
            uint8_t rsv0;
            uint8_t rsv1;
            uint8_t cmd;
            uint8_t size;
            uint32_t addr;
            uint16_t data;
        } __attribute__((packed)) Command;

        auto doStep = [&](const Command &cmd) {
            // request service code
            ecatCoeSdoDownloadEx(slave_id_, COE_SERVO_ADJUST, 1, (uint8_t *) &cmd, sizeof(cmd), COE_SDO_TIMEOUT_MS,
                               0);
            int8_t status = 0;
            do {
                usleep(500000);
                status = getCoeSdoValue8(COE_SERVO_ADJUST, 2);
                //printf("status = %d\n", status);   ///CIS
            } while (status == -1);
        };

        Command cmd;
        cmd.cmd = 1;
        cmd.size = 2;
        cmd.addr = 0x00002000;
        cmd.data = 0x1008;
        SEP_TRI("ServoPack Adjust Request.\n");
        doStep(cmd);

        // 조정 준비
        cmd.addr = 0x00002001;
        cmd.data = 0x0002;
        SEP_TRI("ServoPack Adjust Prepare.\n");
        doStep(cmd);

        // 조정 실행
        cmd.data = 0x0001;
        SEP_TRI("ServoPack Adjust Run.\n");
        doStep(cmd);

        SEP_TRI("ServoPack Adjust Done.\n");
        return true;
    }

    bool CiA402Servo::saveParams(COE_STORE_OPTIONS opt)
    {
        SEP_LOGT("Save params to non-volatile memory of slave %d, opt = %d\n", slave_id_, opt);
        int32_t dat = 0x65766173; // evas
        return setCoeSdoValue(COE_STORE_PARAM, opt, dat);
    }

    bool CiA402Servo::restoreDefaultParams(COE_STORE_OPTIONS opt)
    {
        SEP_LOGT("Restore default params of slave %d, opt = %d\n", slave_id_, opt);
        int32_t  dat = 0x64616F6C; //daol
        return setCoeSdoValue(COE_RESTORE_PARAM, opt, dat);
    }

    int32_t CiA402Servo::getPosition()
    {
        return inPdo().position;
    }

    int32_t CiA402Servo::getRefPosition()
    {
        return outPdo().position;
    }
	
	int32_t CiA402Servo::getRefVelocity()
    {
        return outPdo().velocity;
    }

    int32_t CiA402Servo::getVelocity()
    {
        return inPdo().velocity;
    }

    int16_t CiA402Servo::getTorque()
    {
        return inPdo().torque;
    }

    vel_unit_t CiA402Servo::getVelocityDemand()
    {
        //return inPdo().velocity_demand;
        return true;
    }

    bool CiA402Servo::setPositionTarget(int32_t target)
    {
        outPdo().position = target;
        return true;
    }

    bool CiA402Servo::setVelocityTarget(int32_t target)
    {
        outPdo().velocity = target;
        return true;
    }

    bool CiA402Servo::setTorqueTarget(int16_t target)
    {
        outPdo().torque = target;
        return true;
    }

    bool CiA402Servo::setMax_Velocity(vel_unit_t target)
    {
        //outPdo().max_prof_velocity = target;
        return true;
    }

    bool CiA402Servo::setACC_rate(vel_unit_t acc_rate)
    {
        //outPdo().prof_acceleration = acc_rate;
        return true;
    }

    bool CiA402Servo::setDCC_rate(vel_unit_t dcc_rate)
    {
        //outPdo().prof_deceleration = dcc_rate;
        return true;
    }

    bool CiA402Servo::setQuickStopRate(vel_unit_t quick_stop_rate)
    {
        //outPdo().quick_stop_deceleration = quick_stop_rate;
        return true;
    }

}
