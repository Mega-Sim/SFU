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
#include <cmath>
#include "yaskawa_servo_model.h"
#include "ecat_emulator.h"
namespace sephi
{

    // motor model
    ServoMotorModel::ServoMotorModel()
    {
        home_offset_ = 0;
        setPosition(0);
        setParam(6000, 12000);
        bus_cycle_ = 1e-6 * the_ec_emulator.get()->busCycleUs();
    }

    void ServoMotorModel::setParam(double max_vel_rpm, double max_acc_rpm_s)
    {
        max_v_ = max_vel_rpm/60.0*(1 << 20);
        max_a_ = max_acc_rpm_s/60.0*(1 << 20);
    }

    void ServoMotorModel::stateUpdate(bool vel_only)
    {
        prev_pos_ = pos_;
        if (vel_only) {
            if (t_vel > vel_) {
                if ((t_vel - vel_)/bus_cycle_ <= max_a_) {
                    vel_ = t_vel;
                } else {
                    vel_ += max_a_*bus_cycle_;
                }
            } else {
                if ((vel_ - t_vel)/bus_cycle_ <= -max_a_) {
                    vel_ = t_vel;
                } else {
                    vel_ -= max_a_*bus_cycle_;
                }
            }
            pos_ += vel_ * bus_cycle_;
        } else {
            if (t_pos > pos_) {
                if ((t_pos - pos_)/bus_cycle_ <= max_v_) {
                    vel_ = (t_pos-pos_)/bus_cycle_;
                    pos_ = t_pos;
                } else {
                    vel_ = max_v_;
                    pos_ += max_v_*bus_cycle_;
                }
            } else {
                if ((pos_ - t_pos)/bus_cycle_ <= max_v_) {
                    vel_ = (t_pos-pos_)/bus_cycle_;
                    pos_ = t_pos;
                } else {
                    vel_ = -max_v_;
                    pos_ += -max_v_*bus_cycle_;
                }
            }
        }

    }

    void ServoMotorModel::setPosition(int32_t initial_pos)
    {
        pos_ = t_pos = prev_pos_ = initial_pos;
        vel_ = t_vel = 0;
        trq = 0;
    }

    void ServoMotorModel::setHomeOffset(int32_t home_offset)
    {
        //SEP_LOGD("ServoMotorModel::setHomeOffset curr pos = %d, home = %d, new home =%d", pos_, home_offset_, home_offset);
        SEP_LOGD("Reset Encoder!");
        pos_ = 0; //pos_ - home_offset_ + home_offset;
        home_offset_ = 0; //home_offset;
        t_pos = prev_pos_ = pos_;
        vel_ = t_vel = 0;
        stateUpdate(false);
    }

    void ServoMotorModel::updatePosition(int32_t targetpos)
    {
        t_pos = targetpos;
        stateUpdate(false);
    }

    void ServoMotorModel::updateVelocity(int32_t target_vel)
    {
        t_vel = target_vel;
        stateUpdate(true);
    }

    void YaskawaServoModel::printStatus()
    {
        super::printStatus();
        printf("\tCia402 Status: [0x%04x] %s\n", state_->status, ((Cia402StatusWord*)(&state_->status))->getStateString());
        printf("\tMode of Operation: %d\n", state_->op_mode);
        printf("\tEncoder : %d, vel:%d, target reached = %d\n", motor_.posVal(), state_->velocity, ((Cia402StatusWord*)(&state_->status))->target_reached);
    }

    void YaskawaServoModel::initialize()
    {
        main_power_ = true;  // true for debugging
        motor_power_ = true;
        ctrl_ = (Cia402PdoOut*)rxpdo_;
        state_ = (Cia402PdoIn*)txpdo_;
        state_->op_mode = static_cast<uint8_t>(ServoOpMode::NO_MODE);
        state_->status = 0;
        ctrl_->control = 0;
        prev_state_ = *state_;
        bus_cycle_ = 1e-6 * the_ec_emulator.get()->busCycleUs();
        pp_done_ = true;
        t_ = 0;
        quick_stop_ = false;
    }

    /*
    void YaskawaServoModel::mainPowerOn()
    {
        main_power_ = true;
    }

    void YaskawaServoModel::mainPowerOff()
    {
        Cia402StatusWord status(state_->status);
        status.setState(Cia402StatusWord::STATE::SWITCH_ON_DISABLED);
        state_->status = status.ref();
        main_power_ = false;
    }

    void YaskawaServoModel::motorPowerOn()
    {
        motor_power_ = true;
    }

    void YaskawaServoModel::motorPowerOff()
    {
        Cia402StatusWord status(state_->status);
        if (status.getState() == Cia402StatusWord::STATE::OPERATION_ENABLED) { // disable operation
            status.setState(Cia402StatusWord::STATE::SWITCHED_ON);
            state_->status = status.ref();
        }
        main_power_ = false;
    }
*/

    void YaskawaServoModel::makeFault(uint16_t error_code)
    {
        SEP_LOGT("%s: make servo fault with error code 0x%x", name_.c_str(), error_code);
        Cia402StatusWord& status = *(Cia402StatusWord*)&state_->status;
        status.fault = 1;
        state_->error_code = error_code;
    }

    void YaskawaServoModel::setPdoValue(const string& objname, CoeValue& val)
    {
        // \todo implement
        if (objname == "Position") {
            state_->position = val.i32;
            motor_.setPosition(val.i32);
            SEP_LOGD("YaskawaServoModel::set position = %d\n", state_->position);
        } else {
            SEP_LOGE("%s: setPdoValue of %s is not implemented!\n", this->getModelName(), objname.c_str());
        }
    }

    void YaskawaServoModel::handleCia402StateMachine()
    {
        Cia402StatusWord& status = *(Cia402StatusWord*)&state_->status;
        Cia402ControlWord command(ctrl_->control);
        Cia402StatusWord oldstate = status;
        auto cia402state = status.getState();

        if ( cia402state == Cia402StatusWord::NOT_READY_TO_SWITCH_ON) { // when power on, automatically changes to switch-on-disabled
            status.setState(Cia402StatusWord::STATE::SWITCH_ON_DISABLED);
        } else if (cia402state == Cia402StatusWord::STATE::SWITCH_ON_DISABLED) {
            if (command.command == Cia402ControlWord::COMMAND::SHUTDOWN) {
                status.setState(Cia402StatusWord::STATE::READY_TO_SWITCH_ON);
            }
        } else if (cia402state == Cia402StatusWord::STATE::READY_TO_SWITCH_ON) {
            if (command.command == Cia402ControlWord::COMMAND::SWITCH_ON) {
                status.setState(Cia402StatusWord::STATE::SWITCHED_ON);
            } else if (!main_power_ ) {
                status.setState(Cia402StatusWord::STATE::SWITCH_ON_DISABLED);
            }
        } else if (cia402state == Cia402StatusWord::STATE::SWITCHED_ON) {
            if (command.command == Cia402ControlWord::COMMAND::ENABLE_OP) {
                status.setState(Cia402StatusWord::STATE::OPERATION_ENABLED);
            } else if (command.command == Cia402ControlWord::COMMAND::SHUTDOWN) {
                status.setState(Cia402StatusWord::STATE::READY_TO_SWITCH_ON);
            } else if (!main_power_ || command.command == Cia402ControlWord::COMMAND::QUICK_STOP) {
                status.setState(Cia402StatusWord::STATE::SWITCH_ON_DISABLED);
            }
        } else if (cia402state == Cia402StatusWord::STATE::OPERATION_ENABLED) {
            if (command.command == Cia402ControlWord::COMMAND::DISABLE_OP) {
                status.setState(Cia402StatusWord::STATE::SWITCHED_ON);
            } else if (command.command == Cia402ControlWord::COMMAND::SHUTDOWN) {
                status.setState(Cia402StatusWord::STATE::READY_TO_SWITCH_ON);
            } else if (!main_power_ || command.command == Cia402ControlWord::COMMAND::QUICK_STOP) {
                status.setState(Cia402StatusWord::STATE::SWITCH_ON_DISABLED);
            }
        } else if (cia402state == Cia402StatusWord::STATE::FAULT) {
            if (command.command == Cia402ControlWord::COMMAND::FAULT_RESET) {
                status.setState(Cia402StatusWord::STATE::SWITCH_ON_DISABLED);
            }
        }
        else { // all other case will be comme into faut
            status.setState(Cia402StatusWord::STATE::FAULT);
        }

#if 1  // for debugging
        if(cia402state != status.getState()){
            SEP_LOGI("Slave_%d cia402 state =%04x -> %04x (%s)\n", id_, cia402state, status.ref(), status.getStateString());
        }
#endif
    }

/*
    void YaskawaServoModel::handleQuickStop()
    {
        Cia402StatusWord& status = *(Cia402StatusWord*)&state_->status;
        Cia402ControlWord& command = *(Cia402ControlWord*)&ctrl_->control;
        if (quick_stop_) {
            if (t_ >= qs_.Duration()) {
                SEP_LOGI("Slave %d quick stop done!\n", addr_);
                status.setState(Cia402StatusWord::STATE::SWITCH_ON_DISABLED);
                quick_stop_ = false;
                status.quick_stop = 0;
            } else {
                motor_.updateVelocity(qs_.Pos(t_));
                t_ += bus_cycle_;
            }
        } else if (command.command == Cia402ControlWord::COMMAND::QUICK_STOP) {
            SEP_LOGI("Slave %d goes into quick stop!\n", addr_);
            uint32_t  dec = findSdoByObjId(COE_QSTOP_DEC)->u32 * 1e4;
            qs_.SetMax(dec);
            qs_.SetProfile(motor_.velVal(), 0);
            t_ = 0;
            status.setState(Cia402StatusWord::STATE::SWITCH_ON_DISABLED);
            command.command = 0; // reset quick stop
            // invalidate pp_
            pp_done_ = true;
            quick_stop_ = true;
            status.quick_stop = 1;
        }
    }
*/


    void YaskawaServoModel::handlePP()
    {
        Cia402StatusWord& status = *(Cia402StatusWord*)&(state_->status);
        Cia402ControlWord& command = *(Cia402ControlWord*)&(ctrl_->control);

        // set point (bit 4), ignoring bit 5, 9
        if (command.getBit(4)) {
            // profile setup
            int32_t target = ctrl_->position;
            uint32_t vel = 0; //ctrl_->prof_velocity;
            uint32_t acc = findSdoByObjId(COE_PROF_ACC)->u32;
            uint32_t dec = findSdoByObjId(COE_PROF_DEC)->u32;
            SEP_LOGI("***** Slave %d PP motion target=%d, vel=%d acc=%d!\n", addr_, target, vel, acc);
            pp_.SetMax(vel, acc*1e4, dec*1e4);
            pp_.SetProfile(motor_.posVal(), target, motor_.velVal(), 0);
            t_ = 0;
            pp_done_ = false;
            // set point acknowledged
            status.setBit(12,true);
            // clear target reached status
            status.target_reached = 0;
            // clear command
            command.command = 0;
            SEP_LOGT("> set point bit = %d\n", command.getBit(4));
        } else if (!pp_done_) { // position update
            status.setBit(12,false); // clear set point ack
            motor_.updatePosition(pp_.Pos(t_));
            t_ += bus_cycle_;
            if (t_ >= pp_.Duration()) {
                SEP_LOGI("***** Slave %d PP motion done!\n", addr_);
                motor_.updatePosition(pp_.Pos(pp_.Duration()));
                pp_done_ = true;
                status.target_reached = 1; // target reached
                SEP_LOGT(">>> set point bit = %d\n", command.getBit(4));
            }
        }

    }

    void YaskawaServoModel::handleCSP()
    {
        Cia402StatusWord& status = *(Cia402StatusWord*)&(state_->status);
        //if (id_ == 3) SEP_LOGT("[%d] pos = %d\n", id_, ctrl_->position);
        motor_.updatePosition(ctrl_->position);
        int32_t pos_window = findSdoByObjId(COE_POS_WINDOW)->i32;
        status.target_reached = (abs(ctrl_->position - motor_.posVal()) <= pos_window) ? 1 : 0;
    }

    void YaskawaServoModel::handleCSV()
    {
        Cia402StatusWord& status = *(Cia402StatusWord*)&(state_->status);
        motor_.updateVelocity(ctrl_->velocity);
        int32_t vel_window = findSdoByObjId(COE_VEL_WINDOW)->i32;
        status.target_reached = (abs(ctrl_->position - motor_.velVal()) <= vel_window) ? 1 : 0;
    }

    void YaskawaServoModel::processSDO()
    {
        Cia402StatusWord&  status = *(Cia402StatusWord*)&(state_->status);
        super::processSDO();
    }

    void YaskawaServoModel::processPDO()
    {
        Cia402StatusWord&  status = *(Cia402StatusWord*)&(state_->status);


        // treat state machine
        handleCia402StateMachine();

        // when operation enabled, do servo control job
        if (status.getState() == Cia402StatusWord::STATE::OPERATION_ENABLED) {
            switch (static_cast<ServoOpMode >(state_->op_mode)) {
                case ServoOpMode::PP:
                    handlePP();
                    break;
                case ServoOpMode::CSP:
                    handleCSP();
                    break;
                case ServoOpMode::CSV:
                    handleCSV();
                    //SEP_LOGT("vel cmd = %d, actual = %d\n", ctrl_->velocity, motor_.velVal());
                    break;
                default:
                    SEP_LOGE("Emulator does not support op_mode:%d\n", state_->op_mode);
            }
            state_->position = motor_.posVal();
            state_->velocity = motor_.velVal();
            // backup state
            prev_state_ = *state_;
        } else {
            motor_.setPosition(motor_.posVal());
            state_->position = motor_.posVal();
            state_->velocity = motor_.velVal();
        }
        //if (id_==0) SEP_LOGD("YaskawaServoModel::position = %d, ref = %d\n", state_->position, ctrl_->position);

    }

    bool YaskawaServoModel::handleMailBox(MailBoxData& mbox)
    {
        Cia402StatusWord&  status = *(Cia402StatusWord*)&(state_->status);
        switch (mbox.index){
            case COE_POS_LIMIT:
                SEP_LOGE("Set Pos %s: %d, current enc=%d\n", (mbox.subindex==1)?"min":"max", mbox.data.i32, motor_.posVal());
                return super::handleMailBox(mbox);
            case COE_MODE_OF_OPERATION:
                state_->op_mode = mbox.data.u8;
                return super::handleMailBox(mbox);
            case COE_HOME_OFFSET:
                if (status.getState() == Cia402StatusWord::STATE::SWITCH_ON_DISABLED ) {
                    SEP_LOGD("Home offset data: %d, upload=%d\n", mbox.data.i32, mbox.upload);
                    // need to synchronize shared memory, master stack, coe dictionary and motor model state
                    mbox.data.i32 = 0; // disable home offset function for test
                    if (!mbox.upload) {
                        motor_.setHomeOffset(mbox.data.i32);
                        state_->position = motor_.posVal();
                        state_->velocity = 0;
                        ctrl_->position = state_->position;
                        ctrl_->velocity = 0;
                    } else {
                        state_->position = motor_.posVal();
                    }
                    SEP_LOGD("YaskawaServoModel::set position = %d\n", state_->position);
                    return super::handleMailBox(mbox);
                } else {
                    SEP_LOGE("[%s] fail to set home offset. State is not SWITCH_ON_DISABLED\n", name_.c_str());
                    return false;
                }
            default:
                return super::handleMailBox(mbox);
        }
    }


}