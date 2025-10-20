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
// Created by master on 17. 4. 18.
//

#ifndef SEPMASTER_ECAT_CIA402_SERVO_H
#define SEPMASTER_ECAT_CIA402_SERVO_H

#include "ecat_cia402.h"
namespace sephi
{
    /**\addtogroup grp_cia402
     * \{
     */
    // master process에서 servo 기능을 테스트 하기 위해 만든 class
    class CiA402Servo {
    public:
        CiA402Servo(int ec_client_id, int32_t slave_id, uint8_t* in_pdo, uint8_t* out_pdo, int bus_cycle_us);
        bool clearFault();
        bool initServo(ServoOpMode op_mode);
        bool enableServo();
        void followActual();
        bool setOperationMode(ServoOpMode mode);
        int  getOperationMode();
        bool setPositionTarget(int32_t target_pos);
        bool setVelocityTarget(int32_t target_vel);
        bool setTorqueTarget(int16_t target_trq);
        bool setMax_Velocity(vel_unit_t max_vel);
        bool setACC_rate(vel_unit_t acc_rate);
        bool setDCC_rate(vel_unit_t dcc_rate);
        bool setQuickStopRate(vel_unit_t quick_stop_rate);

        int32_t getPosition();
        int32_t getRefPosition();
		int32_t getRefVelocity();
        int32_t getVelocity();
        int16_t getTorque();
        vel_unit_t getVelocityDemand();
        bool quickStop();
        bool shutdown();
        bool setProfilePosition(int32_t target);
        bool setProfilePositionParam(uint32_t prof_velocity, uint32_t prof_acc = 0xFFFFFFFF, uint32_t prof_dec = 0xFFFFFFFF);
        bool targetReached();
        bool getStatusBit(int bit);
        int getSlaveId() const { return slave_id_;}

        bool makeSwitchOnDisabledState();
        bool setSlaveEcatState(uint16_t newstate);
        CoeValue::Type getCoeEntryType(uint16_t index, uint8_t subindex = 0);
        CoeValue readCoeValue(uint16_t index, uint16_t subindex = 0);
        bool writeCoeValue(uint16_t index, uint16_t subindex, const CoeValue &val);
        int8_t getCoeSdoValue8(uint16_t index, uint16_t subindex);
        int8_t getCoeSdoValue(int32_t slave_id, uint16_t index, uint16_t subindex);
        int16_t getCoeSdoValue16(uint16_t index, uint16_t subindex = 0);
        int32_t getCoeSdoValue32(uint16_t index, uint16_t subindex = 0);
        bool setCoeSdoValue(uint16_t index, uint16_t subindex, int8_t value);
        bool setCoeSdoValue(uint16_t index, uint16_t subindex, int16_t value);
        bool setCoeSdoValue(uint16_t index, uint16_t subindex, int32_t value);
        bool setCoeSdoValue(uint16_t index, uint16_t subindex, uint8_t value);
        bool setCoeSdoValue(uint16_t index, uint16_t subindex, uint16_t value);
        bool setCoeSdoValue(uint16_t index, uint16_t subindex, uint32_t value);
        bool enableUserParameter();
        bool resetAbsEncoder();
        bool saveParams(COE_STORE_OPTIONS opt);
        bool restoreDefaultParams(COE_STORE_OPTIONS opt);
    private:
        uint8_t* in_pdo_;
        uint8_t* out_pdo_;
        int bus_cycle_us_;
        int32_t slave_id_;
        int32_t bus_index_;   //CISmulti
        int ec_client_id_;
        Cia402StatusWord::STATE state_;

        // motion param
        uint32_t prof_velocity_;
        uint32_t prof_acc_;
        uint32_t prof_dec_;
        Cia402PdoOut& outPdo() { return *(Cia402PdoOut*)out_pdo_;}
        Cia402PdoIn& inPdo() { return *(Cia402PdoIn*)in_pdo_;}


    public:
        Cia402ControlWord& controlWord() { return *(Cia402ControlWord*)out_pdo_;}
        Cia402StatusWord& statusWord() { return *(Cia402StatusWord*)in_pdo_;}
        uint16_t errorcode(){ return inPdo().error_code; }
        bool waitForStateReady(int state1);
        // error
        uint16_t prev_errorcode;
    };


}
#endif //SEPMASTER_ECAT_CIA402_SERVO_H
