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

#ifndef SEPMASTER_ROBOT_COMMON_H
#define SEPMASTER_ROBOT_COMMON_H

#include "log.h"
#include "control_common.h"
#include "servo_motor.h"
#include "velocity_profile.h"
#include <chrono>
#include <memory>



namespace sephi
{
    using std::unique_ptr;
    using std::make_unique;
    using namespace KDL;

    /** \ingroup grp_motion
     *  \defgroup grp_servo_motion Servo Motion
     *  \brief  서보모터 단위의 모션 (aka. Joint Motion)
     *  \details
     *  ServoMotion 객체는 대상 모터의 "의미있는" 동작에 대한 추상화임
     *   의미있는 행위 목록
     *    - angular displacement (move)
     *    - rotate
     *    - torque (keep torque)
     *    - stop
     *   위의 모션들은 대상 모터의 제어 모드에 따라 다르게 구현되어야 함
     *   \verbatim
                    move          rotate        torq           stop
          CSP    profile          inf profile   N/A            profile
          PP     cmd               N/A          N/A            cmd
          CSV    profie+feedback  profile       N/A            profile
          CST    N/A               N/A          follow         e-stop
        \endverbatim

     *   가정들)
     *    - motion은 동작 수행을 위한 reference를 생성
     *    - reference를 인가하는 것은 robot module의 책임
     *    - motion life cycle동안 제어 모드가 바뀌지 않음
     *    - motion은 단위 동작의 life cycle만 책임. motion 조합을 통한 복잡한 동작은 robot module에서 관리
     *    Motion life cycle
     *      - 객체 생성
     *      - Set Targt : 프로파일 생성을 위한 목표값 설정
     *      - Init Servo : 서버를 해당 모션을 수행하기 위한 제어 상태로 변경
     *      - do motion : by robot module
     *
     *    모션 계층 구조
     *
     *    ServoMotion (추상 클래스 )
     *     - CSPMotion
     *     - CSVMotion
     *     - CSTMotion
     *     - PPMotion
     */
    class ServoMotion : public Motion {
    public:
        ServoMotion(ServoMotor *axis, ControlMode cmode, MotionType mtype);
        virtual ~ServoMotion(){}
        // set up
        bool importMotorLimit(); // import spd and acc limit from motor param
        bool setSpdLimit(double max);
        bool setAccLimit(double min, double max);
        void setId(int _id) { id_ = _id;}

        // status
        int getId(){ return  id_; }
        MotionType getType() { return type_; }
        //MotionState getState() { return state_; }
        MotionState getState()
        {
            if(this != nullptr)
                return state_;
            else
                return MotionState::IDLE;
        }
        virtual double getDuration() = 0;
        std::chrono::nanoseconds getTimeOffset() {return time_offset_.time_since_epoch();}
        void resetTimeOffset(){ time_offset_ = std::chrono::system_clock::now(); }
        double getElapsedTime(); // time offset부터 현재까지의 시간

        virtual bool beginMotion() { resetTimeOffset(); state_ = MotionState::GOING; return true;}
        virtual bool stopMotion() { return motor_->stop();}
        virtual bool updateState(double t) = 0; // pure virtual로 만들기 위해서

        // get reference
        virtual double getPos(double t){return 0;}
        virtual double getVel(double t){return 0;}
        virtual double getAcc(double t){return 0;}

        bool isInStopMotion() { return is_stop_motion_;}
        void setStopMotionFlag(bool _stop) { is_stop_motion_ = _stop; }
    protected:
        bool forward_dir_; // 모션이 encoder 증가 방향인지..
        atomic_bool is_stop_motion_; // whether if stop motion
        // motor
        ServoMotor* motor_;
        // profile
        Range vel_range_;
        Range acc_range_;
        time_point_ns time_offset_; // profile evaluation time offset
    };

    class CSPMotion : public ServoMotion
    {
    public:
        CSPMotion(ServoMotor *axis, MotionType mtype = AMOVE);

        bool setMove(double pos1, double pos2, double vel1 = 0, double vel2 = 0);
        bool setMoveDuration(double pos1, double pos2, double duration, double vel1 = 0, double vel2 = 0);
        bool setMoveSpeed(double pos1, double pos2, double newspeed, double vel1 = 0, double vel2 = 0);
        bool updateState(double t);

        virtual double getDuration();

        double getPos(double t);
        double getVel(double t);
        double getAcc(double t);

    private:
        //VelocityProfile_Trap trapezoid_profile_;
        TrVelocityProfile trapezoid_profile_;
        TrVelocityProfile dec_profile_;
        bool need_dec_;
    };

    class CSVMotion : public ServoMotion
    {
    public:
        CSVMotion(ServoMotor *axis, MotionType mtype = ROTATE);
        bool setRotateDuration(double vel1, double vel2, double duration);
        bool setRototate(double vel1, double vel2);
        bool updateState(double t);

        virtual double getDuration() { return rect_profile_.Duration();}

        double getVel(double t){return rect_profile_.Pos(t);}
        double getAcc(double t){return rect_profile_.Vel(t);}

    public:
        VelocityProfile_Rectangular rect_profile_;
    };

    class PPMotion : public ServoMotion
    {
    public:
        PPMotion(ServoMotor *axis, MotionType mtype = AMOVE);
        bool setMove(double pos, bool immediate = true);
        bool setMoveSpeed(double pos, double newspeed);
        bool setMoveDuration(double pos1, double pos2, double duration, double v0);
        virtual bool beginMotion();
        bool updateState(double t);
        virtual double getDuration() { return -1;} // undefined
    private:
        double target_;
        double speed_;
        bool immediate_start_;
        Range scaled_acc_range_;
        TrVelocityProfile trapezoid_profile_;
    };
    /// \}
}
#endif //SEPMASTER_ROBOT_COMMON_H
