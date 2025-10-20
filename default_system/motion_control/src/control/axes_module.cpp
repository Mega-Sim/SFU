 /* SEMES CONFIDENTIAL
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
// Created by sem1 on 17. 5. 1.
//

#include "axes_module.h"
#include "ecmaster_task.h"

#define TRAJ_LOG_ENABLE

#define SINGLE_AX_LOG(fmt, ...) SEP_LOGT_PREFIX(this->name_.c_str(), fmt, __VA_ARGS__)

namespace sephi
{
    AxesModule::AxesModule(const char* name, ServoOpMode cmode)
        : parent(name), control_mode_(cmode), id_base_(0)
    {
    }

    // 단축 모듈
    SingleAxis::SingleAxis(const char* name, ServoOpMode cmode, bool use_external_pdo_task)
        : AxesModule(name,cmode), motion_(nullptr), csp_motion_(nullptr), csv_motion_(nullptr), pp_motion_(nullptr),
          pos_limit_{-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity()}
    {
        paused_ = false;
        immediate_motion_change_ = false;
        iterative_motion_ = false;
        last_mc_id_ = Command::NONE;
        motors_.reserve(1);
        motors_.clear();
        use_external_pdo_task_ = use_external_pdo_task;
        t_ = t_offset_ = t_save_ = 0;
        // add default event handler
        addEventHandler(EV_SERVO_FAULT, onServoFault);
    }

    bool SingleAxis::initialize(int axis_no, const char* motor_model, double gear_ratio,  bool enable_on_init)
    {
        // motor 설정
        SEP_REQUIRE_RET(gear_ratio > 0, false);
        motors_.emplace_back(std::make_unique<EcServoMotor>());
        auto motor = motors_[0].get();
        if (user_units_[0].first != "") {
            motor->setUserUnit(user_units_[0].first.c_str(), user_units_[0].second);
        }

        SEP_REQUIRE_RET(motor->initialize(axis_no, control_mode_, motor_model, enable_on_init), false);
        motor->setGearRatio(gear_ratio);

        motor->setVelLimit(-ServoMotor::MAX_VELOCITY_DPS, ServoMotor::MAX_VELOCITY_DPS);
        motor->setAccLimit(-ServoMotor::MAX_ACCEL_DPSS, ServoMotor::MAX_ACCEL_DPSS);

        // motion 생성
        SEP_REQUIRE_RET(createMotions(), false);
        motion_ = nullptr;

        // log파일 생성
#ifdef TRAJ_LOG_ENABLE
        //traj_logger_ = std::make_unique<TrajLogger>(name_.c_str());   // 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe
#endif

        // reference set
        pos_ = ref_pos_ = motors_[0]->getActual()[0];
        vel_ = ref_vel_ = 0;
        trq_ = 0;

        // task 구동
        return parent::initialize();
    }

    int SingleAxis::onServoFault(IEventHost*ins, int evid, void *arg)
    {
        auto axis = (SingleAxis*)ins;
        if (evid == EV_SERVO_FAULT) {
            SEP_LOGE("%s servo fault! Disable motion now.", axis->name());
            axis->disableServo();
            axis->disableMotion();
        }
        return 0;
    }

    bool SingleAxis::changeControlMode(ServoOpMode new_cmode)
    {
        SEP_REQUIRE_RET(motion_ == nullptr && getMotionState() == Motion::IDLE, false); // for safety
        control_mode_ = new_cmode;
        return motors_[0]->setServoMode(new_cmode);
    }

    bool SingleAxis::finalize()
    {
        SINGLE_AX_LOG("%s called\n", __PRETTY_FUNCTION__);
    }
	//semes
    bool SingleAxis::reenable()
    {
        SEP_REQUIRE_RET(enableServo(0), false);
        motion_enable_ = true;
        return true;
    }

    bool SingleAxis::enableServo(int axis)
    {
        if(motors_[axis]->isEnabled() == false)
        {
           pos_ = ref_pos_ = motors_[0]->getActual()[0];
            vel_ = ref_vel_ = 0;
        }
        SEP_REQUIRE_RET(RoboticModule::enableServo(axis), false);

        return true;   
    }

    bool SingleAxis::clearFaults()
    {
        SEP_REQUIRE_RET(motors_[0]->clearFaults(), false);
        bool motion_enabled = motion_enable_;
        if(!motion_enabled)
            motion_enable_ = true;      // 210201 yjchoe, Fault Clear 후 Motion 미수행 개선
        if (motors_[0]->isEnabled()) {
            return motors_[0]->enable();
        }
        return true;
    }

	// semes
    void SingleAxis::disableMotion()
    {
        motion_enable_ = false;
        motion_ = nullptr;
    }

    bool SingleAxis::setMax(double max_vel, double max_acc, double min_acc, bool set_motor_limit)
    {
        SEP_REQUIRE_RET(max_vel > 0, false);
        auto motor = motors_[0].get();
        if (set_motor_limit) motor->setVelLimit(-max_vel, max_vel);
        if (csp_motion_ != nullptr) csp_motion_->setSpdLimit(max_vel);
        if (csv_motion_ != nullptr) csv_motion_->setSpdLimit(max_vel);
        if (pp_motion_ != nullptr) pp_motion_->setSpdLimit(max_vel);
        if (max_acc > 0) {
            min_acc = (min_acc <= 0) ? max_acc : min_acc;
            if (set_motor_limit) motor->setAccLimit(min_acc, max_acc);
            if (csp_motion_ != nullptr) csp_motion_->setAccLimit(min_acc, max_acc);
            if (csv_motion_ != nullptr) csv_motion_->setAccLimit(min_acc, max_acc);
            if (pp_motion_ != nullptr)  pp_motion_->setAccLimit(min_acc, max_acc);
        }
        return true;
    }

    bool SingleAxis::setMaxRPM(double max_vel_rpm, double max_acc_rpm, double max_dec_rpm, bool set_motor_limit)
    {
        return setMax(max_vel_rpm*6, max_acc_rpm*6, max_dec_rpm*6, set_motor_limit);
    }

    bool SingleAxis::setMaxTrq(uint16_t max_trq)
    {
        auto motor = motors_[0].get();
        return motor->setMaxTrq(max_trq);
    }

    bool SingleAxis::getMaxTrq(uint16_t max_trq)
    {
        auto motor = motors_[0].get();
        return motor->getMaxTrq(max_trq);
    }

    bool SingleAxis::setPosLimit(double pos_min, double pos_max)
    {
        auto motor = motors_[0].get();
        pos_limit_.first = pos_min;
        pos_limit_.second = pos_max;
        //SINGLE_AX_LOG("%s setPosLimit %f, %f\n", name_.c_str(), pos_min, pos_max);
        motor->setPosLimit(pos_min, pos_max);
        if (ECAT_EMULATION_MODE) {
            ref_pos_ = pos_min;
        }
    }

    bool SingleAxis::createMotions()
    {
        // csp & csv motions are changable
        if (control_mode_ == ServoOpMode::CSP || control_mode_ == ServoOpMode::CSV) {
            csp_motion_ = make_unique<CSPMotion>(motors_[0].get());
            csv_motion_ = make_unique<CSVMotion>(motors_[0].get());
        } else if (control_mode_ == ServoOpMode::PP) {
            pp_motion_ = make_unique<PPMotion>(motors_[0].get());
        } else if (control_mode_ == ServoOpMode::CST) {
        } else {
            return false;
        }
        return true;
    }

    void SingleAxis::pdoUpdateTask()
    {
        if (!ECAT_EMULATION_MODE) {
            //addIntoRealtimeSchedule(USER_ROBOT_MODULE_PRIO);
        }
        setCpuAffinity(2);
		addIntoRealtimeSchedule(USER_RT_APP_PRIO);
        log_count = 0;
        while(!exit_) {
            pdo_arrival_event_->wait(0.1);// check needed 20190528
            auto t0 = std::chrono::system_clock::now();
            // \todo overload 처리할 것
//            if (getMotor(0)->checkOverload() < 0) {
//               // SEP_TRE("[%s] OVERLOAD DETECTED!\n", name_.c_str());
//            }
//            if (getMotor(0)->checkOverload() == -1) {
//                stop(3500, true);
//                SEP_TRE("[%s] E_FOLLOW_ERROR\n", name_.c_str());
//            }

            auto& motor = motors_[0];
            if (motor->isFault() && motion_enable_) {
                setEvent(EV_SERVO_FAULT);
                SINGLE_AX_LOG("Detect Servo Fault![0x%x]", motor->getErrcode());
            }

            if(motors_[0]->isEnabled() == false)
            {
                disableMotion();
                pos_ = ref_pos_ = motors_[0]->getActual()[0];
                vel_ = ref_vel_ = 0;
            }
            else
            {
                enableMotion();
            }

            if (!motion_enable_) {
                continue;
            }
            if (commandq_.size() > 0) {
                //auto t0 = std::chrono::system_clock::now();
                processCommand();
                //auto dur = std::chrono::system_clock::now() - t0;
                //SEP_LOGT("Process duration = %.2fms", dur.count()/1e6);
            }

            if (motion_ != nullptr) {
                //if(getMotor(0)->enabled_ && getMotor(1)->enabled_)
                updateReference();
//                else
//                    SEP_LOGE("Motor is not Enabled\n");
            } else {
                stayReference();
            }

            auto dur = std::chrono::system_clock::now() - t0;
            if (dur.count() > 1e6)
                SEP_LOGT("pdoUpdateTask time over : %.2fms", dur.count()/1e6);
        }
    }

    bool SingleAxis::processCommand()
    {

        std::lock_guard<std::mutex> lock(mutex_);
        if(commandq_.size() > 0)
        {
            auto& cmd = commandq_.front();
            //SINGLE_AX_LOG("cmdq size = %d, cmd = %d\n", commandq_.size(), cmd.code);

            if (cmd.code == Command::STOP || cmd.code == Command::ESTOP || cmd.code == Command::FSTOP) {
                current_motion_command_.code = cmd.code;
                onStop(cmd);
                current_motion_command_.code = Command::NONE;
                paused_ = false;
                commandq_.pop_front();
            } else if (cmd.code == Command::CLEAR) {
                commandq_.clear();
                paused_ = false;
            } else if (cmd.code == Command::PAUSE) {
                onPause(cmd);
                commandq_.pop_front();
            } else if (cmd.code == Command::RESUME) {
                onResume(cmd);
                commandq_.pop_front();
            } else if ((motion_ == nullptr || immediate_motion_change_) && !paused_) {
                current_motion_command_ = cmd;
                iterative_motion_ = false;
                switch (cmd.code) {
                    case Command::MOVE_I:  iterative_motion_ =  true; /* fall through */
                    case Command::MOVE_D:   onMoveD(cmd); break;
                    case Command::MOVE_A:   onMoveA(cmd); break;
                    case Command::ROTATE:   onRotate(cmd); break;
                }
                if (motion_ != nullptr) {
                    motion_->setId(cmd.id);// wait_for때 사용하기 위해
                    motion_->setStopMotionFlag(false);
                } else {
                    current_motion_command_.code = Command::NONE;
                }
                commandq_.pop_front();
            }
        }
        return true;
    }

    bool SingleAxis::resetEncoder(bool soft, uint8_t AxisType)
    {
        auto& motor = motors_[0];
        bool rtn = false;
        if(motion_ == nullptr) {
            //bool motion_enabled = motion_enable_;
            disableMotion();

            rtn = motors_[0]->resetEncoder(soft, AxisType);
            // update actual state
            auto actual = motor->getActual();   // reset 이후의 현재값을 업데이트
            ref_pos_ = actual[0];
            ref_vel_ = actual[1];

            //if (motion_enabled) { // AMFLM-2 횡전개로 Servo off 시 상시 disable 시킴
            enableMotion();
            //}
        }
        return rtn;
    }

    void SingleAxis::stayReference()
    {
        auto& motor = motors_[0];
        // update actual state
        auto actual = motor->getActual();
        pos_ = actual[0];
        vel_ = actual[1];
        trq_ = actual[2];

        if (vel_ > 200 && motor->getSlaveId() == 0) {
            //SINGLE_AX_LOG("motion status : motion_[%d], ref_pos_[%.0f]", (motion_ == nullptr), ref_pos_);
        }

        if (!motor->isFault()) {
            if (control_mode_ == ServoOpMode::CSP) {
                motor->setReferenceCSP(ref_pos_);
				//motor->setReferenceCSV(ref_vel_);
            } else if (control_mode_ == ServoOpMode::CSV) {
                motor->setReferenceCSV(ref_vel_);
            }
        }
    }

    bool SingleAxis::updateReference()
    {
        auto& motor = motors_[0];
        bool motion_done = false;
        //t_ = motion_->getElapsedTime() + motor->getBusCycleUs()*1e-6;
        t_ += motor->getBusCycleUs()*1e-6;  // thread 지연 시 이전 시간과의 차이 만큼의 타겟 생성으로 변경

        // update actual state
        auto actual = motor->getActual();
        pos_ = actual[0];
        vel_ = actual[1];
        trq_ = actual[2];

        if (motor->isFault()) {
            return false;
        }

        // update servo reference
        if (control_mode_ == ServoOpMode::CSP) {
            motion_done = (t_ >= motion_->getDuration());
            auto t = (motion_done) ? motion_->getDuration() : t_;
            ref_pos_ = motion_->getPos(t);
            ref_vel_ = motion_->getVel(t);

		//SEP_LOGT("3333333333333333333333 ref_pos:%f, pos_:%f", ref_pos_, pos_);
            auto prev_state = motion_->getState();
            motion_->updateState(t);
            if (prev_state == Motion::IN_DEC && motion_->getState() == Motion::IN_CONST_V) {
                setEvent(EV_DEC_END);
            } else if (prev_state == Motion::IN_CONST_V && motion_->getState() == Motion::IN_ACC) {
                setEvent(EV_ACC_BEGIN);
            }
            motor->setReferenceCSP(ref_pos_);
			motor->setReferenceCSV(ref_vel_);   // RSA 긴급정지 시 사용, 220808 yjchoe
            motion_done = motion_done && motor->inPosition();
        } else if (control_mode_ == ServoOpMode::CSV) {
            motion_done = t_ >= motion_->getDuration();
            auto t = (motion_done) ? motion_->getDuration() : t_;
            ref_pos_ = motion_->getPos(t);
            ref_vel_ = (ref_pos_ - pos_)*1 + motion_->getVel(t);

            motor->setReferenceCSV(ref_vel_);
            motion_done = motion_done && motor->inPosition();
            if(motion_done)
            {
                ref_vel_ = motion_->getVel(t);
            }
        } else if (control_mode_ == ServoOpMode::PP) {
            ref_pos_ = pos_ + motor->getPosError();
            motion_done = motor->inPosition();
        }

        // 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe
        /*// 로그 write 시 지연 가능성에 대비하여 주기 증가 1->5, 220718 yjchoe
        if (log_count++ >= 4) {
            logTrajectory();
            log_count = 0;
        }
*/
        // motion done check
        if (motion_done) {
            if (iterative_motion_) {
                iterative_motion_ = false;
                auto max_a = current_motion_command_.max_a * .1;
                auto max_d = current_motion_command_.max_d * .1;
                auto max_v = current_motion_command_.max_v;
                double stop_time = vel_ / max_a;
                auto stop_dist = sign(vel_) * 0.5 * max_a * stop_time * stop_time;
                //printf("motion_dome\n");  ///CIS
                Command stop_cmd(current_motion_command_.id, Command::MOVE_D, stop_dist, max_v, max_a, max_d, -1);
                onMoveD(stop_cmd);
                motion_->setId(current_motion_command_.id);
                motion_->setStopMotionFlag(false);
            } else {
                last_mc_id_ = motion_->getId();
                current_motion_command_.code = Command::NONE;
//                SINGLE_AX_LOG("motion %d is done.\n", motion_->getId());
                motion_->setStopMotionFlag(false); // clear flag
                motion_ = nullptr;
                t_ = 0; // motion time 초기화
                motion_event_.wakeup();
            }
        }
        return true;
    }

    int SingleAxis::getCurrentMotionId()
    {
        return (motion_ == nullptr) ? -1 : motion_->getId();
    }

    Motion::MotionState SingleAxis::getMotionState(int axis)
    {
        return (motion_ == nullptr) ? Motion::IDLE : motion_->getState();
        //return motion_->getState();
    }

    // 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe
    /*void SingleAxis::logTrajectory()
    {
        if (traj_logger_ != nullptr) {
            if (t_ < t_save_) { // new motion ?
                t_offset_ += t_save_;
            }
            t_save_ = t_;
            //LogItem item({t_ + t_offset_, ref_pos_, ref_vel_, pos_, vel_, trq_, getMotor(0)->getPosError(), detect_});
            LogItem item({t_ + t_offset_, ref_pos_, ref_vel_, pos_, vel_, trq_, getMotor(0)->getActErr()});
            item.mc_id = motion_->getId();
            traj_logger_->write(item);
        }
    }*/

    double SingleAxis::validateTargetPosition(double p, const char* msg)
    {
        if (pos_limit_.first == pos_limit_.second) return p; //no limit check
        if (p < pos_limit_.first) {
            SINGLE_AX_LOG("axis[%d] likely to touch lower limit.  p=%f low limit=%f %s\n", motors_[0]->axis_no_, p, pos_limit_.first, msg);
            return pos_limit_.first;
        } else if (p > pos_limit_.second) {
            SINGLE_AX_LOG("axis[%d] likely to touch upper limit. p=%f upper limit=%f %s\n", motors_[0]->axis_no_, p, pos_limit_.second, msg);
            return pos_limit_.second;
        }
        return p;
    }

    bool SingleAxis::onMoveD(Command& cmd)
    {
        auto pos = (cmd.code == Command::FSTOP) ?  motors_[0]->getActual()[0] : ref_pos_; //SBHONG
        double v_end = (cmd.code == Command::MOVE_I) ? cmd.v_end : 0;
        auto vel0 = ref_vel_; //vel_;
        t_ = 0; // motion time 초기화
        //SEP_LOGD("[CSP motion %s before validation] pos = %f, pos limits = %f %f\n", name(), pos, pos_limit_.first, pos_limit_.second);
      //  cmd.target = validateTargetPosition(pos+cmd.target, "MoveDistance") - pos;
        if (control_mode_ == ServoOpMode::CSP || control_mode_ == ServoOpMode::CSV) {
          //  printf("[CSP motion %s] pos = %f, target = %f, ref_pos_ = %f, max_v = %f, max_a = %f, max_d = %f\n", name(), pos, cmd.target, ref_pos_, cmd.max_v, cmd.max_a, cmd.max_d);
            csp_motion_->setAccLimit(cmd.max_d, cmd.max_a);
            
        // printf("Front Wheel Status = %d\n, dpram_.map.DPRamAxisMap[OHT_F].AD_AxisSource & ST_AMP_POWER_ONOFF\n");
         //printf("Rear Wheel Status = %d\n, dpram_.map.DPRamAxisMap[OHT_R].AD_AxisSource & ST_AMP_POWER_ONOFF\n");
            if (cmd.code == Command::MOVE_I || cmd.duration<0) {
                csp_motion_->setMoveSpeed(pos, pos + cmd.target, cmd.max_v, vel0,  v_end);
            } else if (!csp_motion_->setMoveDuration(pos, pos+cmd.target, cmd.duration, vel0)) {
                SEP_LOGE("[%s] Fail to make velocity profile given parameters.", name_.c_str());
                last_mc_id_ = cmd.id;
                motion_event_.wakeup();
                return false;
            }
            motion_ = csp_motion_.get();
        } else {
            pos = motors_[0]->getActual()[0];
            SINGLE_AX_LOG("[PP motion] pos = %f, target = %f\n", pos, cmd.target);
            if (cmd.code == Command::MOVE_I || cmd.duration < 0) {
                pp_motion_->setMoveSpeed(pos + cmd.target, cmd.max_v);
            } else {
                pp_motion_->setMoveDuration(pos, pos+cmd.target, cmd.duration, vel0);
            }
            motion_ = pp_motion_.get();
        }
        // resume을 위한 설정
        if (!paused_) { // pause profile 생성 하면서 덮어 쓰는 것을 방지 하기 위함.
            current_motion_command_.code = Command::MOVE_A; // 원래 목적지로 갈 수 있게 모션 변경
            current_motion_command_.target = pos + cmd.target; // 목적지 저장
            current_motion_command_.duration = -1;  // 시간은 무시
        }
        motion_->setId(cmd.id);
        //SINGLE_AX_LOG("move_d[motion id = %d] created. pos=%.0f, target=%.0f, motion_= %p, duration = %lf\n",
        //              motion_->getId(), pos, pos+cmd.target, motion_, motion_->getDuration());
        //SINGLE_AX_LOG("move_d[motion id = %d]  vmax = %.0f, v0=%.0f, vf=%.0f, amax=%.0f, dmax=%.0f\n",
        //              motion_->getId(), cmd.max_v, vel0, v_end, cmd.max_a, cmd.max_d);

        if (motion_->getDuration() > 10) {
            SINGLE_AX_LOG("Long duration... %.2fs", motion_->getDuration());
        }
        motion_->beginMotion();

        return true;
    }

    bool SingleAxis::onMoveA(Command& cmd)
    {
        auto pos = ref_pos_;
        auto vel0 = ref_vel_; //vel_;
        t_ = 0; // motion time 초기화
        //auto pos = motors_[0]->getActual()[0];  //pos_;
        //auto vel0 = motors_[0]->getActual()[1]; //vel_;

        //SINGLE_AX_LOG("move_a, ref pos/vel=%.0f/%.0f, feedback pos/vel=%.0f/%.0f\n",
        //              ref_pos_, ref_vel_, motors_[0]->getActual()[0], motors_[0]->getActual()[1]);

     //   cmd.target = validateTargetPosition(cmd.target, "Move Absolute");
        if (control_mode_ == ServoOpMode::CSP || control_mode_ == ServoOpMode::CSV) {
            csp_motion_->setAccLimit(cmd.max_d, cmd.max_a);
            if (cmd.duration<0) {
                csp_motion_->setMoveSpeed(pos, cmd.target, cmd.max_v, vel0);
            } else if (!csp_motion_->setMoveDuration(pos, cmd.target, cmd.duration, vel0)) {
                SEP_LOGE("[%s] Fail to make velocity profile given parameters.", name_.c_str());
                last_mc_id_ = cmd.id;
                motion_event_.wakeup();
                return false;
            }
            motion_ = csp_motion_.get();
        } else {
            pp_motion_->setAccLimit(cmd.max_d, cmd.max_a);
            if (cmd.duration < 0) {
                pp_motion_->setMoveSpeed(cmd.target, cmd.max_v);
            } else {
                pp_motion_->setMoveDuration(pos, cmd.target, cmd.duration, vel0);
            }
            pp_motion_->beginMotion();
            motion_ = pp_motion_.get();
        }
        if (motion_) motion_->setId(cmd.id);
        //SINGLE_AX_LOG("move_a[motion id = %d] created. pos=%.0f, target=%.0f, v0=%.0f, vmax=%.0f, a=%.0f, d=%.0f, motion_= %p, duration = %lf\n",
        //              motion_->getId(), pos, cmd.target, vel0, cmd.max_v, cmd.max_a, cmd.max_d,  motion_, motion_->getDuration());
        return true;
    }

    bool SingleAxis::onRotate(const Command& cmd)
    {
        auto vel = motors_[0]->getActual()[1];
        csv_motion_->setAccLimit(cmd.max_d, cmd.max_a);
        if (cmd.duration<0) {
            csv_motion_->setRototate(vel, cmd.target);
        } else if (!csv_motion_->setRotateDuration(vel, cmd.target, cmd.duration)) {
            SEP_LOGE("[%s] Fail to make velocity profile given parameters.", name_.c_str());
            last_mc_id_ = cmd.id;
            motion_event_.wakeup();
            return false;
        }
        csv_motion_->beginMotion();
        motion_ = csv_motion_.get();
        motion_->setId(cmd.id);
        return true;
    }

    // paused_ flag를 set한다.
    // 구동 죽인 모션인 있다면 정지 profile로 정지 시킨다.
    bool SingleAxis::onPause(const Command& cmd)
    {
        SINGLE_AX_LOG("on Pause! current motion id = %d\n", current_motion_command_.id);
        if (paused_ || motion_ == nullptr) {
            SINGLE_AX_LOG("%s","no need to pause\n");
            paused_ = true;
            return true;
        }
        paused_ = true;
        auto newcmd = current_motion_command_;
        // 현재 가속도로 정지할 수 있는 이동 거리만큼 구한다.
        auto stop_time = vel_ / newcmd.max_a;
        auto stop_dist = sign(vel_) * 0.5 * newcmd.max_a * stop_time * stop_time;
        newcmd.target = stop_dist;

        onMoveD(newcmd);
        motion_->setId(cmd.id);
        return true;
    }

    // paused_ flag를 reset한다.
    // current_motion_command가 있다면 replan한다.
    bool SingleAxis::onResume(const Command& cmd)
    {

        SINGLE_AX_LOG("on Resume!  resume motion id = %d\n", current_motion_command_.id);
        if (!paused_ || current_motion_command_.code == Command::MOVE_A) {
            SINGLE_AX_LOG("%s", "no need to resume\n");
            paused_ = false;
            return true;
        }
        paused_ = false;
        onMoveA(current_motion_command_);
        motion_->setId(current_motion_command_.id);
        return true;
    }

    bool SingleAxis::onStop(const Command& cmd)
    {
        //SINGLE_AX_LOG("%s", "onStop begin\n");
        if (control_mode_ == ServoOpMode::CSP || control_mode_ == ServoOpMode::CSV) {
            //  printf("[CSP motion %s] pos = %f, target = %f, ref_pos_ = %f, max_v = %f, max_a = %f, max_d = %f\n", name(), pos, cmd.target, ref_pos_, cmd.max_v, cmd.max_a, cmd.max_d);

            //printf("cmd.max_d : %f\n", cmd.max_d);
            if (cmd.code == Command::STOP) {
                double max_v = -1, max_a = cmd.max_a, max_d = cmd.max_d;
                checkCommandParam(max_v, max_a, max_d); // v,a,d의 min/max limit check
//            if (control_mode_ == ServoOpMode::CSV) {
//                Command stop_cmd(cmd.id, Command::ROTATE, 0, max_v, max_a, max_d, -1);
//                onRotate(stop_cmd);
//                motion_->setId(cmd.id);
//                SINGLE_AX_LOG("Stop rotating. curr vel = %lf\n", vel_);
//            } else
                if (current_motion_command_.code != Command::NONE) {
                    // 현재 가속도로 정지할 수 있는 이동 거리만큼 구한다.
                    //        vel_ = motors_[0]->getActual()[1];
                    double stop_time = ref_vel_ / max_d;
                    auto stop_dist = sign(ref_vel_)* 0.5 * max_d * stop_time * stop_time;
                    Command stop_cmd(cmd.id, Command::MOVE_D, stop_dist, max_v, max_a, max_d, -1);
                    onMoveD(stop_cmd);
                    motion_->setId(cmd.id);
                    motion_->setStopMotionFlag(true); // mark stopmotion
                    //SINGLE_AX_LOG("Stop move. curr vel = %lf, stop move = %lf\n", vel_, stop_dist);
                }
            } else if (cmd.code == Command::ESTOP) { // 비상 정지
                auto &motor = motors_[0];
                motor->stop();
                motion_ = NULL;
                SINGLE_AX_LOG("%s", "Emergent stop!\n");
            } else if (cmd.code == Command::FSTOP) {
                double max_v = -1, max_a = cmd.max_a, max_d = cmd.max_d;
                checkCommandParam(max_v, max_a, max_d); // v,a,d의 min/max limit check
                if (current_motion_command_.code != Command::NONE) {
                    // 현재 가속도로 정지할 수 있는 이동 거리만큼 구한다.
                    //        vel_ = motors_[0]->getActual()[1];
                    double stop_time = ref_vel_ / max_d;
                    auto stop_dist = sign(ref_vel_)* 0.5 * max_d * stop_time * stop_time;
                    Command stop_cmd(cmd.id, Command::FSTOP, stop_dist, max_v, max_a, max_d, -1);
                    onMoveD(stop_cmd);
                    motion_->setId(cmd.id);
                    motion_->setStopMotionFlag(true); // mark stopmotion
                    //SINGLE_AX_LOG("Stop move. curr vel = %lf, stop move = %lf\n", vel_, stop_dist);
                }
            }
        }

        current_motion_command_.code = Command::NONE;
        //SINGLE_AX_LOG("%s", "onStop end\n");
        return true;
    }

    void SingleAxis::checkCommandParam(double& max_v, double &max_a, double &max_d)
    {
        auto& motor = motors_[0];

        if (max_v < 0 || max_v > motor->getVelLimit().second){
            max_v = motor->getVelLimit().second;
        }
        /*
        if (max_a < 0 || max_a > motor->getAccLimit().second) {
            max_a = motor->getAccLimit().second;
        }
        if (max_d < 0 || max_d > fabs(motor->getAccLimit().first)) {
            max_d = fabs(max_a);
        }
         */
        // Acc Limit, yjchoe
        if (max_a <= motor->getAccLimit().first) {
            max_a = motor->getAccLimit().first;
        } else if (max_a >= motor->getAccLimit().second) {
            max_a = motor->getAccLimit().second;
        }
        // Dcc Limit
        if (max_d <= motor->getAccLimit().first) {
            max_d = motor->getAccLimit().first;
        } else if (max_d >= motor->getAccLimit().second) {
            max_d = motor->getAccLimit().second;
        }
    }

    int SingleAxis::moveDistance(double dist, double max_v, double max_a, double max_d, double duration)
    {
        if (!motors_[0]->isEnabled()) { /*("%s\n", "motor is not enabled!");*/ return false;}
        checkCommandParam(max_v, max_a, max_d);
       // printf("%f_%f_%f\n", max_v, max_a, max_d);
        std::lock_guard<std::mutex> lock(mutex_);
        commandq_.emplace_back(generateCommandId(), Command::MOVE_D, dist, max_v, max_a, max_d, duration);
        return commandq_.back().id;
    }

    int SingleAxis::moveDistanceIterative(double dist, double v_end, double max_v, double max_a, double max_d)
    {
        SEP_REQUIRE_RET(control_mode_ == ServoOpMode::CSP || control_mode_ == ServoOpMode::PP, false);
        if (!motors_[0]->isEnabled()) { /*SINGLE_AX_LOG("%s\n", "motor is not enabled!");*/ return false;}
        checkCommandParam(max_v, max_a, max_d);
        std::lock_guard<std::mutex> lock(mutex_);
        commandq_.emplace_back(generateCommandId(), Command::MOVE_I, dist, max_v, max_a, max_d, v_end);
        return commandq_.back().id;
    }


    int SingleAxis::moveTo(double pos, double max_v, double max_a, double max_d, double duration)
    {
        SEP_REQUIRE_RET(control_mode_ == ServoOpMode::CSP || control_mode_ == ServoOpMode::PP, false);
        if (!motors_[0]->isEnabled()) { /*SINGLE_AX_LOG("%s\n", "motor is not enabled!");*/ return false;}
        checkCommandParam(max_v, max_a, max_d);
        std::lock_guard<std::mutex> lock(mutex_);
        commandq_.emplace_back(generateCommandId(), Command::MOVE_A, pos, max_v, max_a, max_d, duration);
        return commandq_.back().id;
    }

    int SingleAxis::rotateAt(double vel, double max_v, double max_a, double max_d, double duration)
    {
        SEP_REQUIRE_RET(control_mode_ == ServoOpMode::CSV, -1);
        if (!motors_[0]->isEnabled()) { /*SINGLE_AX_LOG("%s\n", "motor is not enabled!");*/ return false;}
        checkCommandParam(max_v, max_a, max_d);
        if (vel > max_v) {
            vel = max_v;
        } else if (vel < -max_v) {
            vel = -max_v;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        commandq_.emplace_back(generateCommandId(), Command::ROTATE, vel, max_v, max_a, max_d, duration);
        return commandq_.back().id;
    }

    bool SingleAxis::pause()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        commandq_.emplace_back(generateCommandId(), Command::PAUSE, 0, 0, 0, 0, 0);
        return true;

    }

    bool SingleAxis::resume()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        commandq_.emplace_back(generateCommandId(), Command::RESUME, 0, 0, 0, 0, 0);
        return true;
    }

    bool SingleAxis::stop(double dec, int emergent)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!motors_[0]->isEnabled()) { /*SINGLE_AX_LOG("%s\n", "motor is not enabled!");*/ return false;}
        if (motion_ != nullptr && motion_->isInStopMotion()) { /*SINGLE_AX_LOG("%s", "Already in Stop motion.\n");*/ }
        if (emergent == 0)
        {
            commandq_.emplace_back(generateCommandId(), Command::STOP, 0, 0, dec, dec, 0);
        } else if (emergent == 1){
            commandq_.emplace_back(generateCommandId(), Command::ESTOP, 0, 0, dec, dec, 0);
        } else if (emergent == 2){
            commandq_.emplace_back(generateCommandId(), Command::FSTOP, 0, 0, dec, dec, 0);
        }
        //commandq_.emplace_back(generateCommandId(), (emergent ==1) ? Command::ESTOP : ((emergent == 0) ? Command::STOP : Command::FSTOP), 0, 0, dec, dec, 0);
        return true;
    }

    bool SingleAxis::clearCommand()
    {
        mutex_.lock();
        if (!motion_enable_ || !motors_[0]->isEnabled()) {
            commandq_.clear();
            paused_ = false;
        } else {
            commandq_.emplace_back(generateCommandId(), Command::CLEAR, 0, 0, 0, 0, 0);
            //SEP_LOGT("cmd size : %d\n", commandq_.size());
            mutex_.unlock();
            do { // wait until clear command queue
                usleep(100);
            } while (commandq_.size() > 0);
        }
        mutex_.unlock();
        return true;
    }

    bool SingleAxis::waitForMotionDone(int command_id)
    {
        while(last_mc_id_ != command_id && !motion_event_.timeToExit()) { // \todo 중간에 빠져 나올 수 있게 처리
            motion_event_.wait();
        }
        return true;
    }

    void SingleAxis::OhtDetectLv(double lv)
    {
        detect_ = (double)lv;
    }

    ISlaveAxis::ISlaveAxis(ServoOpMode slave_cmode) : master_id_(-1), slave_ctrl_mode_(slave_cmode)
    {
        if (slave_cmode != ServoOpMode::CST && slave_cmode != ServoOpMode::CSV) {
            SEP_LOGE("Invalid slave control mode. Slave should be either CST or CSV!");
            SEP_LOGI("Slave control mode is changed to CST");
            slave_ctrl_mode_ = ServoOpMode::CST;
        }
    }

    PairedAxes::PairedAxes(const char* name, ServoOpMode cmode, ServoOpMode slave_cmode)
            : SingleAxis(name, cmode), ISlaveAxis(slave_cmode)
    {
        motors_.reserve(2);
        if (slave_cmode != ServoOpMode::CST && slave_cmode != ServoOpMode::CSV) {
            SEP_LOGE("Invalid slave control mode. Slave should be either CST or CSV!");
            SEP_LOGI("Slave control mode is changed to CST");
            slave_ctrl_mode_ = ServoOpMode::CST;
        }
    }


    // paired axes 초기화
    bool PairedAxes::initialize(int master_axis, int slave_axis, const char* master_model, const char* slave_model, double master_gear_ratio)
    {

        // motor 설정
        SEP_REQUIRE_RET(master_gear_ratio > 0, false);
        motors_.emplace_back(make_unique<EcServoMotor>());
        motors_.emplace_back(make_unique<EcServoMotor>());
        auto master_motor = motors_[0].get();
        auto slave_motor = motors_[1].get();
        for(auto i = 0; i < 2; ++i) {
            if (user_units_[i].first != "") {
                motors_[i]->setUserUnit(user_units_[i].first.c_str(), user_units_[i].second);
            }
        }
        SEP_REQUIRE_RET(master_motor->initialize(master_axis, control_mode_, master_model), false);
        SEP_REQUIRE_RET(slave_motor->initialize(slave_axis, slave_ctrl_mode_, slave_model), false); // slave 축은 속도제어 모드로..
        //SEP_REQUIRE_RET(slave_motor->initialize(slave_axis, ServoOpMode::CST, slave_model), false); // slave 축은 토크제어 모드로..
        master_motor->setGearRatio(master_gear_ratio);
        master_id_ = master_motor->getSlaveId();
        slave_motor->setMasterAxis(master_id_);

        for(auto& motor : motors_) {
            motor->setVelLimit(-ServoMotor::MAX_VELOCITY_DPS, ServoMotor::MAX_VELOCITY_DPS);
            motor->setAccLimit(-ServoMotor::MAX_ACCEL_DPSS, ServoMotor::MAX_ACCEL_DPSS);
        }

        // motion 생성
        SEP_REQUIRE_RET(createMotions(), false);

        // log파일 생성
        //traj_logger_ = std::make_unique<TrajLogger>(name_.c_str());   // 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe

        // reference set
        pos_ = ref_pos_ = motors_[0]->getActual()[0];
        ref_vel_ = 0;

        // task 구동
        return parent::initialize();
    }

    bool PairedAxes::finalize()
    {
        SEP_LOGT("%s called\n", __PRETTY_FUNCTION__);

    }

    bool PairedAxes::enableServo(int axis)
    {
        SEP_REQUIRE_RET(RoboticModule::enableServo(0), false);
        SEP_REQUIRE_RET(RoboticModule::enableServo(1), false);
        auto& slave_motor = motors_[1];
        auto& master_motor = motors_[0];
        slave_motor->setMasterAxis(master_motor->getAxisNo());
        pos_ = ref_pos_ = motors_[0]->getActual()[0];
        return true;
    }

    bool PairedAxes::disableServo(int axis)
    {
        SEP_REQUIRE_RET(RoboticModule::disableServo(0), false);
        SEP_REQUIRE_RET(RoboticModule::disableServo(1), false);
        return true;
    }

    bool PairedAxes::clearFaults()
    {
        SEP_REQUIRE_RET(motors_[0]->clearFaults(), false);
        SEP_REQUIRE_RET(motors_[1]->clearFaults(), false);
        if (motors_[0]->isEnabled()) {
            SEP_REQUIRE_RET(motors_[0]->enable() && motors_[1]->enable(), false);
        }
        return true;
    }

// 모션 스레드 지연에 영향을 주어 삭제, 220727 yjchoe
    /*void PairedAxes::logTrajectory()
    {
        auto& slave_motor = motors_[1];
        auto actual = slave_motor->getActual();
        slave_pos_ = actual[0];
        slave_vel_ = actual[1];
        slave_trq_ = actual[2];

        if (traj_logger_ != nullptr) {
            LogItem item({t_, ref_pos_, ref_vel_, pos_, vel_, trq_, slave_pos_, slave_vel_, slave_trq_});
            item.mc_id = motion_->getId();
            traj_logger_->write(item);
        }
    }*/

    bool PairedAxes::follow(int master_id)
    {
        auto& slave = motors_[1];
        return slave->setMasterAxis(master_id_);
    }

}
