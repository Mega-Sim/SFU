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

#ifndef SEPMASTER_EC_SERVO_MOTOR_H
#define SEPMASTER_EC_SERVO_MOTOR_H


#include "servo_motor.h"
#include "ec_servo_emulator.h"
namespace  sephi
{
    /** \addtogroup grp_servo_motor
     * \{
     */
    namespace {
        const int64_t ECAT_EMULATION_CYCLETIME_NS = 1000000;
        bool ECAT_EMULATION_MODE  = false;
    }

    enum SERVO_AXIS_TYPE {
        SINGLE_SERVO,
        DUAL_SERVO,
        TRIPLE_SERVO,
        QUAD_SERVO,
    };

    // CiA402 compliant ethercat servo drive interface
    class EcServoMotor : public ServoMotor {
        using parent = ServoMotor;
    public:
        EcServoMotor();
        ~EcServoMotor();

        void printInpdoPos();
        bool adjustingCommand(uint8_t AxisType = 0);
        bool adjustingCommandRSA(uint8_t AxisType = 0);

        bool initialize(int axis, ServoOpMode mode, const char* model = nullptr,  bool enable_on_init = true) override;
        bool finalize() override;
        bool enable() override;
        bool disable() override;
        bool isFault() const {return (statusWord().fault != 0); }

        ServoOpMode getServoMode() override;
        bool setServoMode(ServoOpMode new_mode) override;
        bool setPosLimit(double min, double max) override; // soft limit
        bool setVelLimit(double min, double max) override; // profile vel
        bool setAccLimit(double min, double max) override; // profile acc/dec/
        bool setMaxTrq(uint16_t max) override;
        bool getMaxTrq(uint16_t trq) override;
        bool setOverloadCondition(double over_trq_duration, double following_error_tolerance, double near_zero_speed) override;
        int checkOverload() override;
        bool setToleranceMargin(ControlTarget mode, double new_val) override;
        bool setMasterAxis(int slave_id) override;
        bool resetMasterAxis() override;

        // control
        bool isEnabled();
        bool shutdown();
        ServoState getServoState() override;
        bool stop() override;
        bool brakeOn() override;
        bool brakeOff() override;
        bool isBrakeOn() override;
        bool setHome() override;
        bool isHome() override;
        bool resetEncoder(bool soft = 1, uint8_t AxisType = 0) override;   // soft - 0: ABS, 1: INC, AxisType - 0: Single, 1:Dual
        bool clearFaults() override;
        bool loadHomeOffset();

        bool setReferencePP(double p, double* v=nullptr, double* a=nullptr, bool immediate = true) override; // PP{p, vmax, accmax},
        bool setReferenceCSP(double p, double* voffs=nullptr, double* toffs=nullptr) override; //  CSP{p, voff, toff}
        bool setReferenceCSV(double v, double* voffs=nullptr, double* toffs=nullptr) override; // CSV
        const Vector3& getActual() override; // pos, vel, acc
        bool inPosition() override { return (emulation_mode_) ? true : statusWord().target_reached; }
        double getPosError() override;
        double getVelError() override;
        double getRefPulse();
        double getRefPos() override;
        double getRefVel() override;
        uint16_t getErrcode() override;
        double getActPos() override;
        double getActVel() override;
        double getActErr() override;
        double getRawActErr() override;
        int32_t getLoopbackPos() override;

        // ecat
        int getSlaveId(){ return axis_no_; }
        int getBusCycleUs() const { return  bus_cycle_us_; }
      

        std::unique_ptr<ECatSlaveMsgQ> msg_recvq_;
        std::mutex msgq_mtx_; // synchronous한 message queue 통신이 되도록
        std::unique_ptr<SepShm> shm_;
        Cia402PdoOut* out_pdo_;
        Cia402PdoIn* in_pdo_;
        int bus_cycle_us_;
        bool initialized_;
        bool enabled_;
        bool emulation_mode_; // ECMaster에 연결하지 않고 emulation할지 여부 (motion test 용)
        int servo_type_;
        int32_t enc_offset_;

        // status 관리
        Cia402ControlWord& controlWord() { return *(Cia402ControlWord*)out_pdo_;}
        Cia402StatusWord& statusWord() const { return *(Cia402StatusWord*)in_pdo_;}
        bool waitForStateReady(int state);
        bool clearStatusBits(uint16_t bits);
        bool setStatusBits(uint16_t);
        bool getStatusBits(uint16_t);

        static ECatSlaveMsgQ* getSendQ();
        bool disableStatus;  // 210806 yjchoe, add disable flag
    public:
        // mail box 통신
        int coeSdoUpload(uint16_t index, uint16_t subindex, uint8_t* val, int len);
        bool coeSdoDownload(uint16_t index, uint16_t subindex, const uint8_t *val, int len);
        friend class RoboticModule;
    };

    /// \}
}
#endif //SEPMASTER_EC_SERVO_MOTOR_H
