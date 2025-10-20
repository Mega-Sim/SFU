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
// Created by sem1 on 17. 4. 26.
//

#include "robotic_module.h"
#include "ecmaster_task.h"
namespace sephi {

    RoboticModule::RoboticModule(const char *name)
            : exit_(false), motion_enable_(false), svc_(nullptr),
              pdo_arrival_event_(nullptr), motion_event_(mutex_),
              use_external_pdo_task_(false), initialized_(false), user_units_(MAX_MOTOR_AXIS_COUNT)
    {
        name_ = name;
        if (ECAT_EMULATION_MODE) {
            EcServoEmulator::start();
        }
    }

    RoboticModule::~RoboticModule() {
        finalize();
    }

    bool RoboticModule::initialize() {
        if (use_external_pdo_task_) {
            return true; // 다른 곳에서 PDO update를 할 것임
        }
        pdo_arrival_event_ = std::make_unique<IpcCondition>(EM_CYCLIC_EVENT_KEY());
        SEP_REQUIRE_RET(pdo_arrival_event_ != nullptr, false);
        initialized_ = true;

        beginPdoUpdateTask();
        enableMotion();
        return true;
    }

    bool RoboticModule::finalize() {
        SEP_LOGT("%s called\n", __PRETTY_FUNCTION__);
        disableMotion();
        endPdoUpdateTask();
        return true;
    }

    bool RoboticModule::reenable()
    {
        for(auto& motor : motors_) {
            if (!motor->enable()) return false;
        }
        enableMotion();
        return true;
    }

    bool RoboticModule::enableServo(int axis) {
        SEP_REQUIRE_RET(axis >= 0 && axis < MAX_MOTOR_AXIS_COUNT, false);
        return motors_[axis]->enable();
    }

    bool RoboticModule::disableServo(int axis) {
        SEP_REQUIRE_RET(axis >= 0 && axis < MAX_MOTOR_AXIS_COUNT, false);
        return motors_[axis]->disable();
    }


    bool RoboticModule::setUserUnit(int axis, const char* unitname, double  pulse_per_unit)
    {
        SEP_REQUIRE_THROW(axis >= 0 && axis < MAX_MOTOR_AXIS_COUNT);
        if (initialized_) {
            SEP_LOGE("User unit must be set before initialization");
            return false;
        }
        user_units_[axis] = {unitname, pulse_per_unit};
        return true;
    }

    const char* RoboticModule::getUserUnitName(int axis)
    {
        SEP_REQUIRE_THROW(axis >= 0 && axis < motors_.size());
        return motors_[axis]->getUserUnitName();
    }

    double RoboticModule::getPulsePerUnit(int axis)
    {
        SEP_REQUIRE_THROW(axis >= 0 && axis < motors_.size());
        return motors_[axis]->getPulsePerUnit();
    }

    EcServoMotor* RoboticModule::getMotor(int axis) {
        SEP_REQUIRE_RET(axis >= 0 && axis < motors_.size(), nullptr);
        return motors_[axis].get();
    }

    bool RoboticModule::beginPdoUpdateTask()
    {
        svc_ = std::make_unique<std::thread>([this](){this->pdoUpdateTask();});
        return svc_!= nullptr;
    }

    void RoboticModule::endPdoUpdateTask()
    {
        if (svc_) {
            exit_ = true;
            svc_->join();
            svc_ = nullptr;
        }
    }
}
