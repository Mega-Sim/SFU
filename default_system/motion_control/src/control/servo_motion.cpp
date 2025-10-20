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
// Created by sem1 on 17. 4. 27.
//
#include "servo_motion.h"

#define MOT_LOGT(...) SEP_LOGT(__VA_ARGS__)

extern void logStackTrace();
namespace sephi
{
    ServoMotion::ServoMotion(ServoMotor *axis, ControlMode cmode, MotionType mtype)
        : forward_dir_(true), motor_(axis)
    {
        cmode_ =cmode;
        type_ = mtype;
        is_stop_motion_ = false;
        importMotorLimit();
    }

    bool ServoMotion::importMotorLimit()
    {
        // parameter import
        vel_range_ = motor_->getVelLimit();
        acc_range_ = motor_->getAccLimit();
        return true;
    }

    bool ServoMotion::setSpdLimit(double max)
    {
        vel_range_.first = -max;
        vel_range_.second = max;
        return true;
    }

    bool ServoMotion::setAccLimit(double min, double max)
    {
        if(motor_->getAxisNo() == 0)
        {
            // not use, yjchoe
            /*
            if(2000>min) min = 2000;
            if(3500<min) min = 3500;

            if(1000>max) max = 1000;
            if(2000<max) max = 2000;
             */
        }
        acc_range_.first = min;
        acc_range_.second = max;
        return true;
    }

    double ServoMotion::getElapsedTime()
    {
        auto elapsed = std::chrono::system_clock::now() - time_offset_;
        return elapsed.count() / 1e9;
    }

    CSPMotion::CSPMotion(ServoMotor *axis, MotionType mtype)
        : ServoMotion(axis, ControlMode::CSP, mtype)
    {
        std::string tmp = "dec";
        tmp += axis->getName();
        need_dec_ = false;
        dec_profile_.SetMax(vel_range_.second, acc_range_.first, acc_range_.second);
        dec_profile_.SetName(tmp.c_str());
        trapezoid_profile_.SetMax(vel_range_.second, acc_range_.first, acc_range_.second);
        trapezoid_profile_.SetName(axis->getName());
    }

    bool CSPMotion::updateState(double t)
    {
        if (need_dec_) {
            if (t < dec_profile_.Duration()) {
                state_ = (forward_dir_) ? MotionState::IN_DEC : MotionState::IN_ACC;
                return true;
            }
            t -= dec_profile_.Duration();
        }
        auto acc = trapezoid_profile_.Acc(t);
        if (acc > 0 ){
            state_ = (forward_dir_) ? MotionState::IN_ACC : MotionState::IN_DEC;
        } else if (acc < 0) {
            state_ = (!forward_dir_) ? MotionState::IN_ACC : MotionState::IN_DEC;
        } else {
            state_ = MotionState::IN_CONST_V;
        }
        return true;
    }

    bool CSPMotion::setMove(double pos1, double pos2, double vel1, double vel2)
    {
        //MOT_LOGT("[%s] CSPMotion::setMove\n", motor_->getName());
        need_dec_ = false;
        trapezoid_profile_.SetMax(vel_range_.second, acc_range_.first, acc_range_.second);
        trapezoid_profile_.SetProfile(pos1, pos2, vel1, vel2);
        state_ = MotionState::IDLE;
        forward_dir_ = (pos2 >= pos1);
        time_offset_ =  std::chrono::system_clock::now();
        return true;
    }

    bool CSPMotion::setMoveDuration(double pos1, double pos2, double duration, double vel1, double vel2)
    {
        MOT_LOGT("[%s] CSPMotion::setMoveDuration\n", motor_->getName());
        need_dec_ = false;
        trapezoid_profile_.SetMax(vel_range_.second, acc_range_.first, acc_range_.second);
        trapezoid_profile_.SetProfileDuration(pos1, pos2, duration, vel1, vel2);
        state_ = MotionState::IDLE;
        forward_dir_ = (pos2 >= pos1);
        time_offset_ =  std::chrono::system_clock::now();
        return true;
    }
#define OHT_LOGT1(...) SEP_LOGT(__VA_ARGS__)
    bool CSPMotion::setMoveSpeed(double pos1, double pos2, double newspeed, double vel1, double vel2)
    {
        need_dec_ = false; // 별도의 감속 프로파일이 필요한지 여부; 현재 속도보다 타겟 속도의 절대값이 작은 경우  필요하다.

        if (pos1 > pos2) {
            newspeed = -fabs(newspeed);
        }

        if (fabs(vel1) > fabs(newspeed)) {
            need_dec_ = true;
        }
/////////////////////////////////////////////////////////////////////////////
//        if (vel1 > 0 && vel1 > newspeed && newspeed >= 0 ) {
//            need_dec_ = true;
//        } else if (vel1 < 0 && vel1 < newspeed && newspeed <= 0) {
//            need_dec_ = true;
//        }
/////////////////////////////////////////////////////////////////////////////
        //MOT_LOGT("[%s] CSPMotion::setMoveSpeed param : newspeed = %f, vel1 = %f, pos1 = %f, pos2 = %f = %c\n", motor_->getName(), newspeed, vel1, pos1, pos2);
        if( vel1 != 0)
        {
			double temp_dec_time = fabs(pos2 - pos1) * 2 / fabs(vel1);
			double temp_dcc = fabs(vel1 / temp_dec_time);
			double temp_dec_dist = 0;
			if (temp_dcc > acc_range_.first) {   //현재 감속도로 멈출 수 없으면	
                if(temp_dcc > 10000)
                    temp_dcc = 10000;											
				acc_range_.first = temp_dcc;
                acc_range_.second = temp_dcc;
				newspeed = vel1;
                temp_dec_time = fabs(vel1) / acc_range_.first;
				temp_dec_dist = 0.5 * vel1 * temp_dec_time * 1.001;				
				pos2 = pos1 + temp_dec_dist;
				need_dec_ = false;
			}
		}

        if (need_dec_ == true) {
            //double dec_time_limit = 2.285714;   // 최대 감속 시간 : 8000/3500=2.285714, 8m/s에서 3.5m/s^2로 감속할때 걸리는 시간
            double dec_time_limit = 2.5;   // 최대 감속 시간 : 2500/1000=2.5, 2.5m/s에서 1.0m/s^2로 감속할때 걸리는 시간(Platform OHT)
            double dec_time = fabs(vel1 - newspeed) /  acc_range_.first;    // 타겟 감속도로 감속하는 시간
            if (dec_time >= dec_time_limit) {
                dec_time = dec_time_limit;      // dec time limit
                acc_range_.first = fabs(vel1) / dec_time;   // 현재 속도에서 정지까지 감속도를 다시 계산
                //MOT_LOGT("[%s] Over dec!\n", motor_->getName());
            }
            double dec_dist =  0.5 * (vel1 + newspeed) * dec_time * 1.001; // multiply 1+epsilon to avoid numerical error of dec_profile_
            double prev_acc = acc_range_.first;

//            if (motor_->getAxisNo() == 0) {
//                // 정지 후 후진
//                if (pos1 > pos2) {
//                    dec_time = fabs(vel1) /  acc_range_.first;  // 정지까지 도달하는 시간
//                    dec_dist = 0.5 * vel1 * dec_time * 1.001;   // 정지까지 도달하는 거리
//                    MOT_LOGT("Back")
//                }
//                // 감속 후 등속
//                else if (pos1 + dec_dist >= pos2 && pos1 < pos2) {   // 현재 위치 + 감속 거리가 최종 타겟을 초과하는 경우
//                    auto prev_time = dec_time;
//                    dec_time = fabs(pos2 - pos1 - 1) * 2 / vel1;        // 현재 위치에서 정지까지 도달하는 시간
//                    acc_range_.first = fabs(vel1) / dec_time;   // 현재 속도에서 정지까지 감속도를 다시 계산
//                    if (acc_range_.first >= 4000) acc_range_.first = 4000;
//                    auto prev_dist = dec_dist;
//                    dec_dist = 0.5 * vel1 * dec_time * 1.001;
//                    MOT_LOGT("[%s] Over target! pos1=%f, dec_dist=%f->%f, pos2=%f, dec_time=%f->%f, dec=%f\n",
//                             motor_->getName(), pos1, prev_dist, dec_dist, pos2, prev_time, dec_time, acc_range_.first);
//                    MOT_LOGT("[%s] change dec! %f -> %f\n", motor_->getName(), prev_acc, acc_range_.first);
//                }
//            }
            double dec_target = pos1 + dec_dist;

            dec_profile_.SetMax(vel1, acc_range_.first, acc_range_.second);
            dec_profile_.SetProfileVelocity(pos1, dec_target, fabs(vel1), vel1, newspeed);
            trapezoid_profile_.SetMax(newspeed, acc_range_.first, acc_range_.second);
            trapezoid_profile_.SetProfileVelocity(dec_target, pos2, newspeed, newspeed, vel2);

            //MOT_LOGT("[%s] CSPMotion::setMoveSpeed:  vel1 = %f, newspd=%f, dectime =%f, dec_dist=%f, t1 = %f, t2=%f\n",
            //         motor_->getName(), vel1, newspeed, dec_time, dec_dist, dec_profile_.Duration(), trapezoid_profile_.Duration());
        } else {
            trapezoid_profile_.SetMax(vel_range_.second, acc_range_.first, acc_range_.second);
            trapezoid_profile_.SetProfileVelocity(pos1, pos2, newspeed, vel1, vel2);
        }
        state_ = MotionState::IDLE;
        forward_dir_ = (pos2 >= pos1);
        time_offset_ =  std::chrono::system_clock::now();
        return true;
    }

    double CSPMotion::getDuration()
    {
        if (need_dec_) {
            return dec_profile_.Duration() + trapezoid_profile_.Duration();
        }
        return trapezoid_profile_.Duration();
    }

    double CSPMotion::getPos(double t)
    {
        if (need_dec_){
            static unsigned cnt = 0;
            return ( t <= dec_profile_.Duration()) ? dec_profile_.Pos(t) : trapezoid_profile_.Pos(t-dec_profile_.Duration());
        } else {
        return trapezoid_profile_.Pos(t);
        }
    }

    double CSPMotion::getVel(double t)
    {
        if (need_dec_){
            return ( t <= dec_profile_.Duration()) ? dec_profile_.Vel(t) : trapezoid_profile_.Vel(t-dec_profile_.Duration());
        } else {
        return trapezoid_profile_.Vel(t);
        }
    }

    double CSPMotion::getAcc(double t)
    {
        if (need_dec_){
            return ( t <= dec_profile_.Duration()) ? dec_profile_.Acc(t) : trapezoid_profile_.Acc(t-dec_profile_.Duration());
        } else {
        return trapezoid_profile_.Acc(t);
        }
    }

    CSVMotion::CSVMotion(ServoMotor *axis, MotionType mtype)
        : ServoMotion(axis, ControlMode::CSV, mtype)
    {
        rect_profile_.SetMax(acc_range_.second);
    }

    bool CSVMotion::updateState(double t)
    {
        return true;
    }

    bool CSVMotion::setRotateDuration(double vel1, double vel2, double duration)
    {
        rect_profile_.SetMax(acc_range_.second);
        rect_profile_.SetProfileDuration(vel1, vel2, duration);
        //SEP_REQUIRE_RET(duration == rect_profile_.Duration(), false);
        state_ = MotionState::IDLE;
        time_offset_ = std::chrono::system_clock::now();
        return true;
    }

    bool CSVMotion::setRototate(double vel1, double vel2)
    {
        rect_profile_.SetMax(acc_range_.second);
        rect_profile_.SetProfile(vel1, vel2);
        state_ = MotionState::IDLE;
        time_offset_ =  std::chrono::system_clock::now();
        return true;
    }

    PPMotion::PPMotion(ServoMotor *axis, MotionType mtype)
        :ServoMotion(axis, ControlMode::PP, mtype)
    {
    }

    bool PPMotion::setMove(double pos, bool immediate)
    {
        target_ = pos;
        speed_ = vel_range_.second;
        state_ = MotionState::IDLE;
        immediate_start_ = immediate;
        time_offset_ =  std::chrono::system_clock::now();
    }

    bool PPMotion::setMoveSpeed(double pos, double newspeed)
    {
        newspeed = fabs(newspeed);
        target_ = pos;
        speed_ =  (newspeed <= vel_range_.second) ? newspeed : vel_range_.second; // limit
        state_ = MotionState::IDLE;
        scaled_acc_range_ = acc_range_;
        immediate_start_ = true; // always immediate change
        time_offset_ =  std::chrono::system_clock::now();
    }

    bool PPMotion::setMoveDuration(double pos1, double pos2, double duration, double vel1)
    {
        MOT_LOGT("[%s] PPMotion::setMoveDuration\n", motor_->getName());
        trapezoid_profile_.SetMax(vel_range_.second, acc_range_.first, acc_range_.second);
        trapezoid_profile_.SetProfileDuration(pos1, pos2, duration, vel1, 0);
        if (trapezoid_profile_.Duration() > 0 ) {
            speed_ = trapezoid_profile_.scaledV;
            scaled_acc_range_.first = trapezoid_profile_.scaledD;
            scaled_acc_range_.second = trapezoid_profile_.scaledA;
        } else {
            SEP_LOGW("[%s] PPMotion::setMoveDuration Schedule Failed!!!!!!!\n", motor_->getName());
            scaled_acc_range_ = acc_range_;
            speed_ = vel_range_.second;
        }
        target_ = pos2;
        immediate_start_ = true;
        state_ = MotionState::IDLE;
        time_offset_ =  std::chrono::system_clock::now();
        return true;
    }

    bool PPMotion::beginMotion()
    {
        // enable before start
        SEP_REQUIRE_RET(motor_->getServoState() == ServoState::OPERATION_ENABLED, false);
        resetTimeOffset();
        if (motor_->getServoMode() != ServoOpMode::PP) {
            motor_->setServoMode(ServoOpMode::PP);
        }
        motor_->setAccLimit(scaled_acc_range_.first, scaled_acc_range_.second);
        return motor_->setReferencePP(target_, &speed_, nullptr, immediate_start_);
    }

    bool PPMotion::updateState(double t)
    {
        if (motor_->inPosition()) {
            state_ = MotionState::IDLE;
        } else {
            state_ = MotionState::GOING;
        }
    }

}
