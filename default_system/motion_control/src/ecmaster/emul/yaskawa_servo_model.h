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
// Created by sem1 on 17. 11. 6.
//

#ifndef SEPMASTER_YASKAWA_SERVO_MODEL_H
#define SEPMASTER_YASKAWA_SERVO_MODEL_H

#include "ecat_slave.h"
#include "velocity_profile.h"
namespace sephi
{
    /// @addtogroup grp_ecmaster_emul
    /// @{

    /** \brief Servo motor encoder model
        \details Servomotor의 encoder update를 모사하기 위한 class
        구현된 모델은 kinematic model만 포함하고 있으며, 적분기는 단순 Euler method를 사용한다.
   */
    struct ServoMotorModel {
        // motor state
        int32_t t_pos;
        int32_t t_vel;
        int16_t trq;
    public:
        ServoMotorModel();
        void setParam(double max_vel_rpm, double max_acc_rpm_s);
        void setPosition(int32_t initial_pos);
        void setHomeOffset(int32_t home_offset);
        void updatePosition(int32_t target_pos);
        void updateVelocity(int32_t target_vel);
        int32_t posVal() const { return pos_;}
        int32_t prevPosVal() const { return prev_pos_;}
        int32_t velVal() const { return vel_;}

    private:
        void stateUpdate(bool vel_only);
        double max_v_, max_a_; // pulse/s, pulse/s^2
        int32_t pos_, prev_pos_, vel_;
        int32_t home_offset_;
        double bus_cycle_;
    };

    /** \brief Yaskawa Servo Drive를 모사한다.
    \details Yaskwaka 서보팩의 CiA402 compliant 제어 기능 중, Profile Position, Cyclic Synchronous Position, Cyclic Synchronous Velocity 모드를 모사한다.

     \todo CiA402 command/status 동작 debugging.
    */
    class YaskawaServoModel : public ECatSlaveModel
    {
        using super = ECatSlaveModel;
    public:
        void initialize();
        void setPdoValue(const string& objname, CoeValue& val);
        void processPDO();
        void processSDO();
        void handleCia402StateMachine();
        void handlePP();
        void handleCSP();
        void handleCSV();
        //void handleQuickStop();
        bool handleMailBox(MailBoxData& mbox);
        void printStatus();
        ServoMotorModel& motor() { return motor_; }

        // behaviors
    public:
        //void mainPowerOn();
        //void mainPowerOff();
        //void motorPowerOn();
        //void motorPowerOff();

        /// test menu
        void makeFault(uint16_t error_code);

        // states
    public:
        double posActual() { return state_->position; }
        double posRef() { return ctrl_->position; }
        double velActual() { return state_->velocity; }
        double velRef() { return ctrl_->velocity; }

    private:

        // pdo map
        Cia402PdoIn* state_;
        Cia402PdoOut* ctrl_;

        // servo states
        Cia402PdoIn prev_state_;
        // power state
        bool main_power_; // main circuit power
        bool motor_power_; // motor power

        // motor state
        ServoMotorModel motor_;
        double bus_cycle_;
        TrVelocityProfile pp_; // velocity profile for PP motion. immediate change mode only
        KDL::VelocityProfile_Rectangular qs_; //quick stop profile
        double t_; // pp motion time
        bool pp_done_;
        bool quick_stop_;
    };
    /// @}
}

#endif //SEPMASTER_YASKAWA_SERVO_MODEL_H
