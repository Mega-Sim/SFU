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
// Created by sem1 on 17. 5. 3.
//

#include "axes_module.h"
#include "ecmaster_task.h"

namespace sephi
{

    MultiAxes::MultiAxes(const char* name, int axis_count, ServoOpMode opmode)
            : AxesModule(name, opmode)
    {
        axes_.reserve(axis_count);
        axes_.clear();
        for(int i = 0; i < axis_count; ++i) {
            char axis_name[32];
            sprintf(axis_name, "axis_%d", i);
            axes_.emplace_back( std::make_unique<SingleAxis>(axis_name, opmode, true)); // using external task thread
        }
        init_param_.reserve(axis_count);
        init_param_.clear();
    }

    MultiAxes::~MultiAxes()
    {
        finalize();
    }

    double MultiAxes::getPos(int axis)
    {
    	return axes_[axis]->getPos(0);
    }

    double MultiAxes::getVel(int axis)
    {
    	return axes_[axis]->getVel(0);
    }

    bool MultiAxes::addAxis(int index, int axis_no, const char* motor_model, double gear_ratio)
    {
        SEP_REQUIRE_RET(index >= 0 && index < axes_.size(), false);
        SEP_REQUIRE_RET(gear_ratio > 0, false);
        init_param_[index] = std::make_tuple(axis_no, motor_model, gear_ratio);
        return true;
    }

    bool MultiAxes::initialize( bool enable_on_init)
    {
        int index = 0;
        for(auto& axis: axes_) {
            if (user_units_[index].first != "") {
                motors_[index]->setUserUnit(user_units_[index].first.c_str(), user_units_[index].second);
            }
            auto& param = init_param_[index++];
            if (axis->initialize(std::get<0>(param), std::get<1>(param), std::get<2>(param), enable_on_init) != true) {
                SEP_TRE("Fail to initialize %s\n", axis->name_.c_str());
                return false;
            }
        }

        // start PDO task
        parent::initialize();

        return true;
    }

    bool MultiAxes::finalize()
    {
        SEP_LOGT("%s called\n", __PRETTY_FUNCTION__);
        endPdoUpdateTask();
        for(auto& axis : axes_) {
            axis->finalize();
        }
        return true;
    }

    bool MultiAxes::reenable()
    {
        for(auto& axis: axes_) {
            if(!axis->reenable()) return false;
        }
        motion_enable_ = true;
        return true;
    }

    bool MultiAxes::enableServo(int axis)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), false);
        return axes_[axis]->enableServo(0);
    }

    bool MultiAxes::disableServo(int axis)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), false);
        return axes_[axis]->disableServo(0);
    }

    bool MultiAxes::clearFaults()
    {
        for(auto& ax : axes_) {
            SEP_REQUIRE_RET(ax->clearFaults(), false);
        }
        return true;
    }

    void MultiAxes::enableMotion()
    {
        for(auto& ax : axes_) {
            ax->enableMotion();
        }
        motion_enable_ = true;
    }

    void MultiAxes::enableMotion(int axis)
    {
        axes_[axis]->enableMotion();
        motion_enable_ = true;
    }


    void MultiAxes::disableMotion()
    {
        for(auto& ax : axes_) {
            ax->disableMotion();
        }
        motion_enable_ = false;
    }

    /*
    bool MultiAxes::isMotionEnabled() const
    {
        return motion_enable_;
    }
     */

    bool MultiAxes::setMax(int axis, double max_vel, double max_acc, double max_dec, bool set_motor_limit)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), false);
        return axes_[axis]->setMax(max_vel, max_acc, max_dec, set_motor_limit);
    }

    bool MultiAxes::setMaxRPM(int axis, double max_vel_rpm, double max_acc_rpm, double max_dec_rpm, bool set_motor_limit)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), false);
        return axes_[axis]->setMaxRPM(max_vel_rpm, max_acc_rpm, max_dec_rpm, set_motor_limit);
    }

    bool MultiAxes::setPosLimit(int axis, double pos_min, double pos_max)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), false);
        return axes_[axis]->setPosLimit(pos_min,pos_max);
    }

    EcServoMotor* MultiAxes::getMotor(int axis)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), nullptr);
        return axes_[axis]->getMotor(0);
    }

    // robot api interface
    double MultiAxes::calcLongestDuration(Command::Code code, std::initializer_list<MxMcParam>& params)
    {
        double longest_duration = 0;
        for(auto& cmd: params) {
            auto motor = getMotor(cmd.axis);
            double max_v = cmd.max_v, max_a = cmd.max_a, max_d = cmd.max_d;
            CSPMotion motion(motor);
            axes_[cmd.axis]->checkCommandParam(max_v, max_a, max_d);
            if (max_v < fabs( motor->getActual()[1])) {
                max_v = fabs( motor->getActual()[1]); // 감속해야하는 경우이므로 현재  속도를  최대 속도로 둠.
            }
            motion.setSpdLimit(max_v);
            motion.setAccLimit(max_d, max_a);

            if (code == Command::MOVE_D) {
                auto pos = motor->getActual()[0];
                motion.setMoveSpeed(pos , pos+cmd.pos, max_v);
            } else if (code == Command::MOVE_A) {
                auto pos = motor->getActual()[0];
                motion.setMoveSpeed(pos , cmd.pos, max_v);
            }
            SEP_LOGT("[axis %d] calcLongestDuration : %f\n",cmd.axis, motion.getDuration() );
            if (motion.getDuration() > longest_duration) {
                longest_duration = motion.getDuration();
            }
        }
        SEP_LOGT("Final calcLongestDuration : %f\n",longest_duration );
        return longest_duration;
    }

    MultiAxesResult MultiAxes::moveDistance(std::initializer_list<MxMcParam> params)
    {
        MultiAxesResult result;
        double longest_duration = calcLongestDuration(Command::MOVE_D, params);
        for(auto& cmd: params){
            if (cmd.axis >= 0 && cmd.axis < axes_.size()) {
                auto dur = (cmd.duration > 0) ? cmd.duration : longest_duration;
                result.emplace_back(cmd.axis, axes_[cmd.axis]->moveDistance(cmd.pos, cmd.max_v, cmd.max_a, cmd.max_d, dur));
            } else {
                result.emplace_back(cmd.axis, -1); // 실패
            }
        }
        return std::move(result);
    }

    MultiAxesResult MultiAxes::moveTo(std::initializer_list<MxMcParam> params)
    {
        MultiAxesResult result;
        double longest_duration = calcLongestDuration(Command::MOVE_A, params);
        for(auto& cmd: params){
            if (cmd.axis >= 0 && cmd.axis < axes_.size()) {
                auto dur = (cmd.duration > 0) ? cmd.duration : longest_duration;
                result.emplace_back(cmd.axis, axes_[cmd.axis]->moveTo(cmd.pos, cmd.max_v, cmd.max_a, cmd.max_d, dur));
            } else {
                result.emplace_back(cmd.axis, -1);
            }
        }
        return std::move(result);
    }

    MultiAxesResult MultiAxes::stop(std::initializer_list<int> axis_list, int emergent)
    {
        MultiAxesResult result;
        for(auto axis: axis_list){
            if (axis >= 0 && axis < axes_.size()) {
                result.emplace_back(axis, axes_[axis]->stop(emergent));
            } else {
                result.emplace_back(axis, false);
            }
        }
        return std::move(result);
    }

    MultiAxesResult MultiAxes::clearCommand(std::initializer_list<int> axis_list)
    {
        MultiAxesResult result;
        for(auto axis: axis_list){
            if (axis >= 0 && axis < axes_.size()) {
                result.emplace_back(axis, axes_[axis]->clearCommand());
            } else {
                result.emplace_back(axis, false);
            }
        }
        return std::move(result);
    }

    bool MultiAxes::stopAll(double dec, int emergent)
    {
        bool ret = true;
        for(auto& axis : axes_) {
            ret = ret && axis->stop(dec, emergent);
        }
        return ret;
    }

    bool MultiAxes::clearCommand()
    {
        bool ret = true;
        for(auto& axis : axes_) {
            ret = axis->clearCommand() && ret;
        }
        return ret;
    }

    bool MultiAxes::pause()
    {
        bool ret = true;
        for(auto& axis : axes_) {
            ret = axis->pause() && ret;
        }
        return ret;
    }

    bool MultiAxes::resume()
    {
        bool ret = true;
        for(auto& axis : axes_) {
            ret = axis->resume() && ret;
        }
        return ret;
    }


    void MultiAxes::setMotionChangeMode(bool immediate)
    {
        for(auto& axis : axes_) {
            axis->setMotionChangeMode(immediate);
        }
    }

    bool MultiAxes::changeControlMode(int axis, ServoOpMode new_cmode)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), -1);
        return axes_[axis]->changeControlMode(new_cmode);
    }

    int MultiAxes::moveDistance(int axis, const double dist, double max_v, double max_a, double max_d, double duration)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), -1);
        return axes_[axis]->moveDistance(dist, max_v, max_a, max_d, duration);
    }

    int MultiAxes::moveTo(int axis, double pos, double max_v, double max_a, double max_d, double duration)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), -1);
        return axes_[axis]->moveTo(pos, max_v, max_a, max_d, duration);
    }

    int MultiAxes::rotateAt(int axis, double vel, double max_v, double max_a, double max_d, double duration)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), -1);
        return axes_[axis]->rotateAt(vel, max_v, max_a, max_d, duration);
    }

    bool MultiAxes::stop(int axis, double dec, int emergent)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), false);
        return axes_[axis]->stop(dec, emergent);
    }

    bool MultiAxes::clearCommand(int axis)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), false);
        return axes_[axis]->clearCommand();
    }

    bool MultiAxes::waitForMotionDone(int axis, int command_id)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), false);
        return axes_[axis]->waitForMotionDone(command_id);
    }

    int MultiAxes::lastMotionCommandId(int axis)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), Command::NONE);
        return axes_[axis]->lastMotionCommandId();
    }

    int MultiAxes::getCurrentMotionId(int axis)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), -1);
        return axes_[axis]->getCurrentMotionId();
    }

    Motion::MotionState MultiAxes::getMotionState(int axis)
    {
        SEP_REQUIRE_RET(axis>=0 && axis < axes_.size(), Motion::IDLE);
        return axes_[axis]->getMotionState();
    }

    void MultiAxes::pdoUpdateTask()
    {
        if (!ECAT_EMULATION_MODE) {
            addIntoRealtimeSchedule(USER_ROBOT_MODULE_PRIO);
        }

        int a = 0; // for break

        while(!exit_) {
            pdo_arrival_event_->wait(0.1);
            if (!motion_enable_) {
                continue;
            }
            for(auto& axisptr : axes_) {
                auto &axis = *axisptr;
                // \todo overload 처리할 것
                if (axis.getMotor(0)->checkOverload() < 0) {
                    SEP_TRE("[%s] OVERLOAD DETECTED!\n", axis.name_.c_str());
                }

                if (axis.commandq_.size() > 0) {
                    axis.processCommand();
                }

                if (axis.motion_ != nullptr) {
                    axis.updateReference();
                } else {
                    axis.stayReference();
                }
                if (axis.getMotor(0)->isFault()) {
                    disableMotion();
                    break;
                }
            }
        }

    }

}

