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
// Created by sy1135.jeong1696 on 2017-07-27.
//
#include <array>
#include <exception>
#include "sephi_ipc.h"
#include "sephi_task.h"
#include "ecat_data_struct.h"
#include "ec_servo_emulator.h"
#include "ec_servo_motor.h"

namespace sephi
{
    const double cycletime = ECAT_EMULATION_CYCLETIME_NS / 1e9;
    const int POS_LOG_WINDOW_SIZE = 5;
    struct ServoMotorData {
        ServoMotorData() {
            memset(&in, 0, sizeof(in));
            memset(&out, 0, sizeof(out));
            following_axis = -1;
            updated = false;
            masteraxis = nullptr;
            last_ma_pos = 0;
        }
        void update() {
            std::lock_guard<std::mutex> guard(mtx_);
            if (updated) return;
            if (following_axis != -1) { // update master axis first
                masteraxis->update();
                out.velocity = masteraxis->in.velocity;
            }
            // 0. get position
            pos_unit_t pos;
            if (mode == ServoOpMode::CSP) {
                pos = out.position;
            } else if (mode == ServoOpMode::CSV) {
                pos = in.position + out.velocity * cycletime;
            } else {
                //throw std::runtime_error("Emulator does not support controlmode except CSP, CSV");
                return;
            }

            // 1. update position & velocity
            in.position = pos;
            in.velocity = calcVelocity(pos);

            // 2. mark updatement
            updated = true;
        }
        std::mutex mtx_;
        Cia402PdoIn in;
        Cia402PdoOut out;
        ServoOpMode mode;
        int32_t following_axis; // 속도 추종 master 축
        ServoMotorData *masteraxis;
        pos_unit_t poslog[POS_LOG_WINDOW_SIZE]; // 속도 계산을 위한 지난 위치 log
        pos_unit_t last_ma_pos; // last moving average position
        bool updated;
    private:
        // First-Order-Approximation을 하니 jitter가 심해서 windowing후 moving average를 취하고 moving average의 변화로 속도를 추정했음
        vel_unit_t calcVelocity(pos_unit_t pos) {
            // update poslog
            memmove(poslog+1, poslog, sizeof(pos_unit_t)*(POS_LOG_WINDOW_SIZE-1)); // last one is in poslog[0]
            poslog[0] = pos;
            // calc moving average
            int64_t sum = 0;
            for(auto log : poslog) sum += log;
            pos_unit_t ma_pos = (sum / POS_LOG_WINDOW_SIZE);
            vel_unit_t vel = (ma_pos - last_ma_pos) / cycletime;
            last_ma_pos = ma_pos;
            return vel;
        }
    };
    class EcMasterEmulator : public Task<bool> {
        enum Constants {
            MOTOR_COUNT = 4,
        };

    public:
        EcMasterEmulator() : conn_(VEL_DISPLAY_HOST(), VEL_DISPLAY_PORT())
        {
            pdo_arrival_event_ = std::make_unique<IpcCondition>(EM_CYCLIC_EVENT_KEY(), true);
        }

        ~EcMasterEmulator()
        {
        }

        ServoMotorData& getServoData(int axis)
        {
            return motors_[axis];
        }

        bool setMasterAxis(int slave_axis, int master_axis)
        {
            auto &slave = motors_[slave_axis];
            if (slave.following_axis != -1 && master_axis == -1) {
                slave.out.velocity = 0;
            }
            slave.following_axis = master_axis;
            if (master_axis == -1) {
                slave.masteraxis = nullptr;
            } else {
                slave.masteraxis = &motors_[master_axis];
            }
            return true;
        }
        bool setServoMode(int axis, ServoOpMode new_mode) {
            motors_[axis].mode = new_mode;
            return true;
        }

    private:
        void svc() {
            auto wake_up_at = std::chrono::system_clock::now();
            auto duration = std::chrono::nanoseconds(ECAT_EMULATION_CYCLETIME_NS);
            uint32_t count = 0;
            while (!stop_) {
                wake_up_at += duration;
                std::this_thread::sleep_until(wake_up_at);
                if (pdo_arrival_event_ == nullptr){
                    break; // main thread에서 먼저 finalize 했을 수 있음
                }
                // state update
                for(auto& motor : motors_) motor.updated = false;
                for(auto& motor : motors_) {
                    motor.update();
                }
                if (conn_.isConnected() && ++count % 100 == 0) {
                    vel_unit_t vels[MOTOR_COUNT];
                    for(int i = 0; i < MOTOR_COUNT; i++) vels[i] = motors_[i].in.velocity;
                    if (conn_.send(vels, sizeof(vels)) < 0) {
                        SEP_LOGT("EC Plot server has been disconnected.\n");
                    }
                }
                pdo_arrival_event_->wakeUp();
            }
        }
        std::unique_ptr<IpcCondition> pdo_arrival_event_; // cyclic event
        // servo data list
        std::array<ServoMotorData, MOTOR_COUNT> motors_;
        // socket
        StreamConnector conn_;

    };
    Poco::SingletonHolder<EcMasterEmulator> the_ecmaster_emulator;


    bool EcServoEmulator::requestSlave(int axis, Cia402PdoIn *&inpdo, Cia402PdoOut *&outpdo, std::mutex*& mtx)
    {
        try {
            ServoMotorData& motor = the_ecmaster_emulator.get()->getServoData(axis);
            inpdo = &motor.in;
            outpdo = &motor.out;
            mtx = &motor.mtx_;
            return true;
        } catch (...) {
            throw std::runtime_error("axis range error.");
        }
    }

    bool EcServoEmulator::setMasterAxis(int axis, int master_axis)
    {
        return the_ecmaster_emulator.get()->setMasterAxis(axis, master_axis);
    }

    bool EcServoEmulator::setServoMode(int axis, ServoOpMode new_mode)
    {
        return the_ecmaster_emulator.get()->setServoMode(axis, new_mode);
    }

    bool EcServoEmulator::start()
    {
        the_ecmaster_emulator.get()->start();
    }
}