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
// Created by root on 17. 11. 9.
//

#include <yaml-cpp/yaml.h>
#include <boost/algorithm/string/join.hpp>
#include <emul/ecat_emulator.h>
#include "stocker_simulator.h"

namespace sephi
{
    using namespace stk_sim;

    bool StockerSystem::loadConfiguration(const string &model_file)
    {
        auto config = YAML::LoadFile(model_file)["Systems"]["Stocker"];
        // set frame info
        frame_size_[0] = config["FrameSize"]["x"].as<double>();
        frame_size_[1] = config["FrameSize"]["y"].as<double>();
        frame_size_[2] = config["FrameSize"]["z"].as<double>();
        shelf_size_[0] = config["ShelfSize"]["x"].as<double>();
        shelf_size_[1] = config["ShelfSize"]["y"].as<double>();
        shelf_size_[2] = config["ShelfSize"]["z"].as<double>();
        shelf_blank_[0] = config["ShelfSize"]["blank_x"].as<double>();
        shelf_blank_[1] = config["ShelfSize"]["blank_y"].as<double>();
        port_blank_[0] = config["ShelfSize"]["blank_x"].as<double>();
        port_blank_[1] = config["ShelfSize"]["port_y"].as<double>();

        std::cout << config["Shelves"].size() << std::endl;
        // load shelves
        for (auto i = 0; i < config["Shelves"].size(); i++) {
            shelf_map_.emplace(std::make_pair(
                config["Shelves"][i]["id"].as<string>(),
                Shelf(this, config["Shelves"][i]["id"].as<string>(),
                      config["Shelves"][i]["z"].as<double>(),
                      config["Shelves"][i]["x"].as<double>(),
                      config["Shelves"][i]["r"].as<double>(),
                      config["Shelves"][i]["y"].as<double>()) ));
        }

        // set robot
        robot_.setPPMM(0, config["Robot"]["PPMM"]["z"].as<double>(), config["Robot"]["Offset"]["z"].as<double>(), frame_size_[2]);
        robot_.setPPMM(1, config["Robot"]["PPMM"]["x"].as<double>(), config["Robot"]["Offset"]["x"].as<double>(), frame_size_[0]);
        robot_.setPPMM(2, config["Robot"]["PPMM"]["r"].as<double>(), 0, 181); // \todo : make it configurable
        robot_.setPPMM(3, config["Robot"]["PPMM"]["y"].as<double>(), 0, 405); // \todo : make it configurable
        robot_.setSize(config["Robot"]["Size"]["fork_width"].as<double>(), config["Robot"]["Size"]["fork_offset"].as<double>());
        distance_to_shelf_ = config["Robot"]["Offset"]["robot_shelf_distance"].as<double>();
        x_inposition_margin_ = (shelf_blank_[0]-config["Robot"]["Size"]["fork_width"].as<double>())/2;
        shelf_plate_thickness_ = config["ShelfSize"]["thickness"].as<double>();

        // load foups
        for(auto i = 0; i < config["Foups"].size(); i++) {
            foups_.push_back(config["Foups"][i].as<string>());
            if (foups_.back() == "Robot") {
                robot_.has_foup = true;
            } else {
                shelf_map_[foups_.back()].has_foup = true;
            }
        }

        return true;
    }

    bool StockerSystem::initialize(ECatEmulator &emul, const string &model_file)
    {
        SEP_REQUIRE_RET(loadConfiguration(model_file), false);
        // config variables
        for(auto& slave : emul.slaves_){
            if (slave->type() == SlaveType::CIA402_COMPLIANT_SERVO || slave->type() == SlaveType::CIA402_COMPLIANT_SERVO2) {    // yjchoe
                axes_[slave->id()] = (YaskawaServoModel*)slave.get();
            } else if (slave->type() == SlaveType::FINE_STK_IO) {
                stk_io_ = (StockerIOModel*)slave.get();
            } else {
                SEP_LOGW("Stocker System, Unused slave %s\n", slave->getModelName());
            }
        }
        // io map; note! stk_io_.do is controller's output, hence, is map to simulator's input
        di_ = (StockerDI*)&stk_io_->DO();
        do_ = (StockerDO*)&stk_io_->DI();

        // event map
        ext_event_map_["estop"] = false;
        ext_event_map_["interlock_key"] = true;
        ext_event_map_["safety_door_close"] = false;

        // status
        crash_status_ = 0;
        curr_shelf_ = nullptr;

        /// initial state
        di_->safety_door_lock = 0;

        return true;
    }

    int StockerSystem::getEncoderPulseValue(int axis)
    {
        auto& motor = axes_[axis]->motor();
        return motor.posVal();
    }

    void StockerSystem::setEncoderPulseValue(int axis, int new_pulse)
    {
        auto& motor = axes_[axis]->motor();
        SEP_LOGT("StockerSystem::setEncoderPulseValue : current = %d, new = %d", motor.posVal(), new_pulse);
        motor.setPosition(new_pulse);
    }

    string StockerSystem::getFoupLocations()
    {
        return boost::algorithm::join(foups_, ",");
    }

    void StockerSystem::pressEStop()
    {
        ext_event_map_["estop"] = true;
    }

    void StockerSystem::turnInterlockKey(bool disable)
    {
        ext_event_map_["interlock_key"] = !disable;
    }

    void StockerSystem::closeSafetyDoor(bool close)
    {
        ext_event_map_["safety_door_close"] = close;
    }

    void StockerSystem::printState()
    {
        auto pulse2deg = [](int32_t pulse){return 360.0*pulse/(1<<20);}; //
        printf("[ Stocker System State ]\n");
        double robot_pos[] = {robot_.z(), robot_.x(), robot_.r(), robot_.y()};
        for(int i = 0; i < axes_.size(); i++){
            auto& axis = axes_[i];
            printf("axis[%d] pos=%.2f deg/%.3f mm (%d pulse), vel=%.2f deg/s\n",
                   axis->id(), pulse2deg(axis->motor().posVal()), robot_pos[i],
                   axis->motor().posVal(), pulse2deg(axis->motor().velVal()));
        }
        printf("DI: 0x%016lx\n", stk_io_->DO_val());
        printf("DO: 0x%016lx\n", stk_io_->DI_val());
        printf("\t mc (n): %u\n", do_->_mc_status);
        printf("\t estop (n): %u\n", do_->_estop_status);
        printf("\t int lock (p): %u\n", do_->interlock_disable_key_status);
        printf("\t fork ld r(n): %u\n", do_->_fork_load_sensor_r);
        printf("\t fork ld l(n): %u\n", do_->_fork_load_sensor_l);
        printf("\t door lock(n): %u\n", do_->_safety_door_lock_status);
        printf("\t door close(n): %u\n", do_->_safety_door_close_status);
        printf("\t empt unload(n): %u\n", do_->_empty_unload_status);
        printf("\t dbl load(p): %u\n", do_->double_load_status);
        printf("\t PLC(n): %u\n", do_->_safety_plc_status);
        printf("\t light curt(n): %u\n", do_->_light_curtain_status);
        printf("\t PLC alarm(p): %u\n", do_->safety_plc_alarm);
        printf("\t robot crashed(p): %d\n", crash_status_);
        printf("\t foups: %s\n", getFoupLocations().c_str());
    }

    Shelf* StockerSystem::getCurrentShelf()
    {
        for(auto& kv : shelf_map_) {
            // check if robot is in shelf range
            if (inRange(robot_.z(), kv.second.loc[0], shelf_size_[2] / 2) &&
                inRange(robot_.x(), kv.second.loc[1], shelf_size_[0] / 2) &&
                inRange(robot_.r(), kv.second.loc[2], 30.0) ) {
                return &kv.second;
            }
        }
        return nullptr;
    }

    void StockerSystem::modelUpdate()
    {
        // robot location compute
        robot_.updateEncoder(axes_[0]->motor().posVal(),
                             axes_[1]->motor().posVal(),
                             axes_[2]->motor().posVal(),
                             axes_[3]->motor().posVal());
        curr_shelf_ = getCurrentShelf();

        // interference check; detect crash
        crash_status_ = 0;
        if (robot_.checkCrash()) { // frame collision
            crash_status_ = 1;
            SEP_LOGE("Robot has crashed to Stocker Frame!\n");
        } else if(curr_shelf_ != nullptr) {
            if (curr_shelf_->checkCollision()) { // shelf collision
                crash_status_ = 1;
            } else if (robot_.y() >= 300) { // foup transfer; only when fork axis is extended more than 300mm
                if (robot_.has_foup) {
                    if (robot_.z() < curr_shelf_->loc[0] && robot_.movingDown()) {
                        robot_.has_foup = false;
                        curr_shelf_->has_foup = true;
                        foups_.remove("Robot");
                        foups_.emplace_back(curr_shelf_->id);
                    }
                } else if (curr_shelf_->has_foup) {
                    if (robot_.z() > curr_shelf_->loc[0] && robot_.movingUp()) {
                        robot_.has_foup = true;
                        curr_shelf_->has_foup = false;
                        foups_.remove(curr_shelf_->id);
                        foups_.emplace_back("Robot");
                    }
                }
            }
        }
    }

    void StockerSystem::outputUpdate()
    {
        // external event
        do_->_estop_status = !ext_event_map_["estop"];
        //... \todo : need to check interlock status update logic
        do_->interlock_disable_key_status = ext_event_map_["interlock_key"];
        do_->_safety_door_close_status = ext_event_map_["safety_door_close"];

        // echo di
        do_->_safety_door_lock_status = !di_->safety_door_lock;
        if (di_->light_curtain_sensor_muting_0) { // light curtain is turned off
            do_->_light_curtain_status = 1;  // do not detect (active low)
        } else {
            if (curr_shelf_ == nullptr || curr_shelf_->type != Shelf::PORT_TYPE) { do_->_light_curtain_status = 1; }
            else { // shelf is not null & type is manual port
                do_->_light_curtain_status = (robot_.y() < 30) ? 1 : 0;
            }
        }
        do_->safety_plc_alarm = !do_->_light_curtain_status;
        do_->_fork_load_sensor_r = !robot_.has_foup;
        do_->_fork_load_sensor_l = !robot_.has_foup;
        if (di_->empty_unload_detector) {
            do_->_empty_unload_status = !robot_.has_foup;
        } else {
            do_->_empty_unload_status = 1;
        }
        if (di_->double_load_detector) {
            do_->double_load_status = (curr_shelf_==nullptr) ? 0 : curr_shelf_->inLoadUnloadPosition();
        } else {
            do_->double_load_status = 0;
        }

        if (di_->safety_reset) {
            do_->_safety_plc_status =
                di_->mc_control
                && do_->interlock_disable_key_status
                && !do_->_safety_door_lock_status
                && !do_->_safety_door_close_status
                && do_->_estop_status
                && do_->_light_curtain_status;

            do_->_mc_status =
                di_->mc_control
                && !do_->_safety_door_lock_status
                //&& !do_->_safety_door_lock_status
                && do_->_estop_status
                && do_->_light_curtain_status
                && do_->_safety_plc_status;
        }
    }

    void StockerSystem::update()
    {
        modelUpdate();
        outputUpdate();
    }

}