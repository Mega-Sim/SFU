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

#include "servo_motor.h"
#include "ecmaster_rpc_client.h"
#include <cmath>

namespace sephi
{
    ServoMotor::ServoMotor()
    {
        // default values
        op_mode_ = ServoOpMode::NO_MODE;
        axis_no_ = -1; // not assigned
        enc_type_ = EncoderType::ABS_ENC; // absolute type by default
        gear_ratio_ = 1.0; // encoder pulse 지령은 deg * pulse_per_unit_ * gear_ratio_가 됨
        pulse_per_unit_ = (1 << 20)/360.0; // 20bit res encoder by default
        unit_name_ = "deg";
        pos_range_.first = 0.0; pos_range_.second = 0.0; // pos limit is not set if first >= second
        vel_range_.first = -MAX_VELOCITY_DPS; // default
        vel_range_.second= MAX_VELOCITY_DPS; // default
        acc_range_.first = -MAX_ACCEL_DPSS;
        acc_range_.second = MAX_ACCEL_DPSS;
        trq_range_.first = -MAX_TORQUE_RATE_x1000; // 무제한
        trq_range_.second = MAX_TORQUE_RATE_x1000;
        tolerance_[0] = tolerance_[1] = tolerance_[2] = 0; // by default tolerance is not set
        gain_[0] = gain_[1] = gain_[2] = 0; // 제어기 Kp, Ki, Kd; 추후에 사용
        dir_ = MotorDirection::UNKNOWN;

        // 아래 값들 중 position값은  initialize 함수에서 현재 값으로 초기화 되어야 한다.
        home_pos_ = 0; // by default not set
        actual_[0] = actual_[1] = actual_[2] = 0;
        ref_pos_.val = 0;
        follow_error_tolerance_ = FOLLOW_ERROR_TOL_x1000 / 1000.0;
        near_zero_velocity_ = NEAR_ZERO_VELOCITY_x1000 / 1000.0;
        overtrq_counter_.reset(OVER_TRQ_WINDOW_SIZE);

    }

    ServoMotor::~ServoMotor()
    {

    }

    bool ServoMotor::setUserUnit(const char* unitname, double pulse_per_unit)
    {
        unit_name_ = unitname;
        pulse_per_unit_ = pulse_per_unit;
        return true;
    }

    const char* ServoMotor::getUserUnitName()
    {
        return unit_name_.c_str();
    }

    double ServoMotor::getPulsePerUnit()
    {
        return pulse_per_unit_;
    }


    bool ServoMotor::setReferencePP(double p, double* v, double* a, bool immediate) // PP{p, vmax, accmax},
    {
        SEP_REQUIRE_IMPL_RET(false);
    }

    bool ServoMotor::setReferenceCSP(double p, double* voffs, double* toffs) //  CSP{p, voff, toff}
    {
        SEP_REQUIRE_IMPL_RET(false);
    }

    bool ServoMotor::setReferenceCSV(double v, double* voffs, double* toffs) // CSV
    {
        SEP_REQUIRE_IMPL_RET(false);
    }

    bool ServoMotor::inPosition()
    {
        SEP_REQUIRE_IMPL_RET(false);
    }
}
