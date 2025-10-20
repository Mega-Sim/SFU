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
#include "emul/ecat_emulator.h"
#include "oht_simulator.h"
#include "emul/onboard_io_emulator.h"
namespace  sephi
{
    using namespace oht_sim;


    OHTSystem::TimeoutJob::TimeoutJob(int timeout_ms, void (*_func)(OHTSystem*, int), int intarg)
    {
        timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(timeout_ms);
        func = _func;
        int_arg = intarg;
    }

    bool OHTSystem::TimeoutJob::isTimeOut()
    {
        return (std::chrono::system_clock::now() >= timeout);
    }

    void OHTSystem::doDelayedJobs()
    {
        auto job = delayed_jobs_.begin();
        while(job != delayed_jobs_.end()){
            if (job->isTimeOut()) {
                job->func(this, job->int_arg);
                job = delayed_jobs_.erase(job);
            } else {
                ++job;
            }
        }
    }

    bool OHTSystem::loadConfiguration(const string &model_file)
    {
        auto config = YAML::LoadFile(model_file)["Systems"]["OHT"];
        // load map data path
        string map_data_path = "../" + config["MapFilePath"].as<string>();

        // map loading
        map_.loadMapData(map_data_path);

        // load oht initial location
        oht_.location.first = map_.getLink(config["InitialLocation"]["from"].as<uint32_t>(), config["InitialLocation"]["to"].as<uint32_t>());
        oht_.location.second = config["InitialLocation"]["offset"].as<double>();

        // load foup list
        for (auto i = 0; i < config["FoupList"].size(); i++) {
            if (config["FoupList"][i].as<string>() == "OHT") {
                oht_.has_foup = true;
                foup_list_.push_back(0xFFFFFFFF);
            } else {
                foup_list_.push_back(config["FoupList"][i].as<uint32_t>());
            }
        }
        // load robot parameters
        oht_.ppmm[OHTInfo::WHEEL] = config["Robot"]["PPMM"]["wheel"].as<double>() * (1<<7); // adjust 13 bit parameter for 20 bit param
        oht_.ppmm[OHTInfo::SLIDE] = config["Robot"]["PPMM"]["slide"].as<double>() * (1<<7);
        oht_.offset[OHTInfo::HOIST] = config["Robot"]["Offset"]["hoist"].as<double>();
        oht_.offset[OHTInfo::SLIDE] = config["Robot"]["Offset"]["slide"].as<double>();
        oht_.inpos[OHTInfo::WHEEL] = config["Robot"]["limits"]["pos"]["inpos"][0].as<double>();
        oht_.inpos[OHTInfo::HOIST] = config["Robot"]["limits"]["pos"]["inpos"][1].as<double>();
        oht_.inpos[OHTInfo::SLIDE] = config["Robot"]["limits"]["pos"]["inpos"][2].as<double>();

        std::array<double,7> mm_list, pulse_list;
        for(auto i = 0; i < 7; ++i) {
            mm_list[i] = config["Robot"]["PPMM"]["hoist"]["mm"][i].as<double>();
            pulse_list[i] = config["Robot"]["PPMM"]["hoist"]["pulse"][i].as<double>();
        }
        oht_.hoist_unit_conv.initialize(pulse_list, mm_list);


        // load size parameter
        param_.precheck2stop = config["Robot"]["Size"]["precheck2stop"].as<double>();
        param_.node_mark = config["MarkSize"]["node"].as<double>();
        param_.station_mark = config["MarkSize"]["station"].as<double>();
        param_.station_mark_front = config["MarkSize"]["station_front_part"].as<double>();
        param_.station_stop_mark = config["MarkSize"]["station_stop"].as<double>();

        // init oht hand, grip state
        if (oht_.has_foup) {
            oht_.hand_state = HandOpenCloseState::CLOSE;
            oht_.shutter_state = ShutterOpenCloseState::CLOSE;
        } else {
            oht_.hand_state = HandOpenCloseState::OPEN;
            oht_.shutter_state = ShutterOpenCloseState::OPEN;
        }
        oht_.steer_state = SteeringState::LEFT; // initial state
        return true;
    }

    void OHTSystem::initStations()
    {
        auto findTeachData = [this](uint32_t stid) {
            auto dat = map_.teach_map.find(stid);
            if (dat != map_.teach_map.end()) {
                return &dat->second;
            } else {
                return static_cast<OHTMap::TeachData*>(nullptr);
            }
        };

        auto hasFoup = [this](uint32_t stid) {
            for(auto id : foup_list_) {
                if (id == stid) return true;
            }
            return false;
        };

        for(auto& kv: map_.station_map) {
            station_map_.emplace(std::make_pair(kv.first, OHTStation(&kv.second, findTeachData(kv.first))));
            station_map_[kv.first].setFoupState(hasFoup(kv.second.id));
        }

    }

    bool OHTSystem::initialize(ECatEmulator &emul, const string &model_file)
    {
        current_station_ = OHTStation::INVALID_STATION_ID;
        SEP_REQUIRE_RET(loadConfiguration(model_file), false);


        // config variables
        for(auto& slave : emul.slaves_){
            if (slave->type() == SlaveType::CIA402_COMPLIANT_SERVO || slave->type() == SlaveType::CIA402_COMPLIANT_SERVO2) {    // yjchoe
                axes_[slave->id()] = (YaskawaServoModel *) slave.get();
            } else if (slave->type() == SlaveType::FINE_OHT_FRONT_IO) {
                front_io_ = (OHTFrontIOModel*)slave.get();
            } else if (slave->type() == SlaveType::FINE_OHT_REAR_IO) {
                rear_io_ = (OHTRearIOModel*)slave.get();
            } else {
                SEP_LOGW("Stocker System, Unused slave %s\n", slave->getModelName());
            }
        }

        // io map;
        front_di_ = (OhtFrontDI*)&front_io_->DO();
        front_do_ = (OhtFrontDO*)&front_io_->DI();
        rear_di_ = (OhtRearDI*)&rear_io_->DO();
        rear_do_ = (OhtRearDO*)&rear_io_->DI();
        auto onboard_io = the_ec_emulator.get()->onboardIo();
        onboard_di_ = (OhtOnboardDI*) onboard_io->DI();
        onboard_do_ = (OhtOnboardDO*) onboard_io->DO();

        // serial sensors
        driving_bcr_ = std::make_unique<BL_185>(static_cast<IMultiportSerialInterface*>(onboard_io), ONBOARD_BCR1);
        trans_bcr_ = std::make_unique<BL_185>(static_cast<IMultiportSerialInterface*>(onboard_io), ONBOARD_BCR2);

        // event map
        ext_event_map_["estop"] = false;
        ext_event_map_["interlock_key"] = true;
        ext_event_map_["safety_door_close"] = false;

        // initialize stations
        initStations();

        // aux variables.
        emo_press_state_ = false;
        other_oht_.setMap(map_);
        DebugState::set(DebugState::OHT_DISTANCE_MM, 100000);

        return true;
    }

    void OHTSystem::validateCurrentStation()
    {
        auto _xor = [](bool x, bool y) { return bool(int(x)^int(y)); };
        // abbreviations
        auto& oht_offset = oht_.location.second;
        auto& curr_link = oht_.location.first;
        auto rear_margin = param_.station_mark-param_.station_mark_front;

        if (current_station_ == OHTStation::INVALID_STATION_ID) { // check if pio communication begin
            if (_xor(rear_di_->_eq_pio_left_select, rear_di_->_eq_pio_right_select)) {
                auto sta = curr_link->getStation(oht_offset, param_.station_mark_front, rear_margin);
                if (sta == nullptr || sta->teach == nullptr ) {
                    // No station available or No teach data exists
                    return;
                }
                if (oht_.has_foup && station_map_[sta->id].hasFoup() || !oht_.has_foup && !station_map_[sta->id].hasFoup()) {
                    // skip if transfer is not possible
                    return;
                }
                current_station_ = sta->id;
                station_map_[current_station_].pio.connect();
                SEP_LOGI("Station %d is connected\n", current_station_);
            }
        } else { // check if pio communication end
            if (rear_di_->_eq_pio_left_select == 1 && rear_di_->_eq_pio_right_select == 1) {
                station_map_[current_station_].pio.disconnect();
                SEP_LOGI("Station %d is disconnected\n", current_station_);
                current_station_ = OHTStation::INVALID_STATION_ID;
            }
        }
    }

    void OHTSystem::updateOhtLocation()
    {
        // read motor encoder and update oht position
        oht_.setHoistEncoder(axes_[2]->motor().posVal());
        oht_.setSlideEncoder(axes_[3]->motor().posVal());
        // driving motor can be reset any time
        double displacement = (axes_[0]->motor().posVal() - axes_[0]->motor().prevPosVal())/oht_.ppmm[OHTInfo::WHEEL];
        oht_.moveDistance(map_, displacement);

        validateCurrentStation();
    }

    void OHTSystem::update()
    {
        doDelayedJobs();
        updateOhtLocation();
        updateDrivngSensors();
        updateEQportSensors();
        updateSTBportSensors();
        updateTransSensors();
        updateAuxSensors();
    }

    void OHTSystem::finalize()
    {

    }

    void OHTSystem::printState()
    {

        // dump DI DO
        printf("Onboard DI:\n");
        printf("[ 0]_obstacle_long      : %u\n", onboard_do_->_obstacle_long);
        printf("[ 1]_obstacle_middle    : %u\n", onboard_do_->_obstacle_middle);
        printf("[ 2]_obstacle_short     : %u\n", onboard_do_->_obstacle_short);
        printf("[ 3]_obstacle_fail      : %u\n", onboard_do_->_obstacle_fail);
        printf("[ 4]_ohtdetect_long     : %u\n", onboard_do_->_ohtdetect_long);
        printf("[ 5]_ohtdetect_middle   : %u\n", onboard_do_->_ohtdetect_middle);
        printf("[ 6]_ohtdetect_short    : %u\n", onboard_do_->_ohtdetect_short);
        printf("[ 7]_ohtdetect_fail     : %u\n", onboard_do_->_ohtdetect_fail);
        printf("[ 8]_stop_driving_front : %u\n", onboard_do_->_stop_driving_front);
        printf("[ 9]_stop_driving_precheck: %u\n", onboard_do_->_stop_driving_precheck);
        printf("[10]bcr_driving_ok      : %u\n", onboard_do_->bcr_driving_ok);
        printf("[11]bcr_driving_ng      : %u\n", onboard_do_->bcr_driving_ng);
        printf("[12]_stop_trans_front   : %u\n", onboard_do_->_stop_trans_front);
        printf("[13]_stop_trans_rear    : %u\n", onboard_do_->_stop_trans_rear);
        printf("[14]bcr_trans_ok        : %u\n", onboard_do_->bcr_trans_ok);
        printf("[15]bcr_trans_ng        : %u\n", onboard_do_->bcr_trans_ng);
        printf("[16]home_sensor1        : %u\n", onboard_do_->home_sensor1);
        printf("[17]positive_limit1     : %u\n", onboard_do_->positive_limit1);
        printf("[18]negative_limit1     : %u\n", onboard_do_->negative_limit1);
        printf("[19]home_sensor2        : %u\n", onboard_do_->home_sensor2);
        printf("[20]positive_limit2     : %u\n", onboard_do_->positive_limit2);
        printf("[21]negative_limit2     : %u\n", onboard_do_->negative_limit2);
        printf("[22]home_sensor3        : %u\n", onboard_do_->home_sensor3);
        printf("[23]positive_limit3     : %u\n", onboard_do_->positive_limit3);
        printf("[24]negative_limit3     : %u\n", onboard_do_->negative_limit3);
        printf("[25]home_sensor4        : %u\n", onboard_do_->home_sensor4);
        printf("[26]positive_limit4     : %u\n", onboard_do_->positive_limit4);
        printf("[27]negative_limit4     : %u\n", onboard_do_->negative_limit4);
        printf("[28]limit_st1           : %u\n", onboard_do_->limit_st1);
        printf("[29]limit_st2           : %u\n", onboard_do_->limit_st2);
        printf("[30]obs_status          : %u\n", onboard_do_->obs_status);
        printf("[31]oht_status          : %u\n", onboard_do_->oht_status);
        printf("[32]qrr1_st1            : %u\n", onboard_do_->qrr1_st1);
        printf("[33]bcr1_st2            : %u\n", onboard_do_->bcr1_st2);
        printf("[34]qrr2_st3            : %u\n", onboard_do_->qrr2_st3);
        printf("[35]bcr2_st4            : %u\n", onboard_do_->bcr2_st4);
        printf("[36]driving_stop_st1    : %u\n", onboard_do_->driving_stop_st1);
        printf("[37]trans_stop_st2      : %u\n", onboard_do_->trans_stop_st2);

        printf("Onboard DO:\n");
        printf("[ 0]_obstacle_area1_sel : %u\n", onboard_di_->_obstacle_area1_sel);
        printf("[ 1]_obstacle_area2_sel : %u\n", onboard_di_->_obstacle_area2_sel);
        printf("[ 2]_obstacle_area3_sel : %u\n", onboard_di_->_obstacle_area3_sel);
        printf("[ 3]_obstacle_area4_sel : %u\n", onboard_di_->_obstacle_area4_sel);
        printf("[ 4]_ohtdetect_area1_sel: %u\n", onboard_di_->_ohtdetect_area1_sel);
        printf("[ 5]_ohtdetect_area2_sel: %u\n", onboard_di_->_ohtdetect_area2_sel);
        printf("[ 6]_ohtdetect_area3_sel: %u\n", onboard_di_->_ohtdetect_area3_sel);
        printf("[ 7]_ohtdetect_area4_sel: %u\n", onboard_di_->_ohtdetect_area4_sel);
        printf("[ 8]servo_brake1        : %u\n", onboard_di_->servo_brake1);
        printf("[ 9]servo_brake2        : %u\n", onboard_di_->servo_brake2);
        printf("[10]servo_brake3        : %u\n", onboard_di_->servo_brake3);
        printf("[11]servo_brake4        : %u\n", onboard_di_->servo_brake4);
        printf("[12]limit_in1           : %u\n", onboard_di_->limit_in1);
        printf("[13]limit_in2           : %u\n", onboard_di_->limit_in2);
        printf("[14]_obstacle_power_reset: %u\n", onboard_di_->_obstacle_power_reset);
        printf("[15]_ohtdetect_power_reset: %u\n", onboard_di_->_ohtdetect_power_reset);
        printf("[16]qrr1_in1            : %u\n", onboard_di_->qrr1_in1);
        printf("[17]bcr1_in2            : %u\n", onboard_di_->bcr1_in2);
        printf("[18]qrr2_in3            : %u\n", onboard_di_->qrr2_in3);
        printf("[19]bcr2_in4            : %u\n", onboard_di_->bcr2_in4);
        printf("[20]driving_stop_in1    : %u\n", onboard_di_->driving_stop_in1);
        printf("[21]trans_stop_in2      : %u\n", onboard_di_->trans_stop_in2);


        string status = getStatusAsYaml();
        printf("%s\n",status.c_str());

    }

    void OHTSystem::setOhtLocation(uint32_t parent, uint32_t next, double offset)
    {
        auto link = map_.getLink(parent, next);
        if (link != nullptr && offset >= 0 && offset < link->distance) {
            oht_.location.first = link;
            oht_.location.second = offset;
        } else {
            printf("Invalid oht location data!\n");
        }
    }

    string OHTSystem::getStatusAsYaml()
    {
#define __BEGIN_MAP(key)             status << YAML::Key << #key << YAML::Value << YAML::BeginMap;
#define __END_MAP()                  status << YAML::EndMap;
#define __ADD_KEY_VALUE(key, value)  status << YAML::Key << #key << YAML::Value << (value);
        YAML::Emitter status;
        auto& oht_offset = oht_.location.second;
        auto& curr_link = oht_.location.first;
        auto rear_margin = param_.station_mark-param_.station_mark_front;
        auto other_oht_loc = other_oht_.travel().getLocation();

        status << YAML::BeginMap;
        // oht location : { from, to, displacemnt}
        __BEGIN_MAP(oht);
            __ADD_KEY_VALUE(speed,  axes_[0]->velActual()/oht_.ppmm[OHTInfo::WHEEL]);
            __BEGIN_MAP(location);
                __ADD_KEY_VALUE(from, oht_.location.first->from);
                __ADD_KEY_VALUE(to, oht_.location.first->to);
                __ADD_KEY_VALUE(disp, oht_.location.second);
                __ADD_KEY_VALUE(remain, map_.getLink(oht_.location.first->from,oht_.location.first->to)->distance - oht_.location.second);
            __END_MAP();
            __BEGIN_MAP(position);
                __ADD_KEY_VALUE(hoist, oht_.hoist_unit_conv.pulse2mm(axes_[2]->posActual()/(1<<7)));
                __ADD_KEY_VALUE(slide, axes_[3]->posActual()/oht_.ppmm[OHTInfo::SLIDE]);
            __END_MAP();
            __BEGIN_MAP(other_oht);
                __ADD_KEY_VALUE(enabled, other_oht_.isEnabled());
                if (other_oht_.isEnabled()) {
                    __ADD_KEY_VALUE(from, other_oht_loc.from);
                    __ADD_KEY_VALUE(to, other_oht_loc.to);
                    __ADD_KEY_VALUE(disp, other_oht_loc.displacement);
                }
            __END_MAP();
            __BEGIN_MAP(dio);
                // driving io
                __ADD_KEY_VALUE(_stop_driving_precheck, onboard_do_->_stop_driving_precheck);
                __ADD_KEY_VALUE(_stop_driving_front, onboard_do_->_stop_driving_front);
                __ADD_KEY_VALUE(_stop_trans_front, onboard_do_->_stop_trans_front);
                __ADD_KEY_VALUE(_stop_trans_rear, onboard_do_->_stop_trans_rear);
                __ADD_KEY_VALUE(steer_front_left, front_do_->steer_front_left);
                __ADD_KEY_VALUE(steer_front_right, front_do_->steer_front_right);
                __ADD_KEY_VALUE(steer_rear_left, rear_do_->steer_rear_left);
                __ADD_KEY_VALUE(steer_rear_right, rear_do_->steer_rear_right);

                // trans in
                __ADD_KEY_VALUE(slide_home_sensor, front_do_->slide_home_sensor);
                __ADD_KEY_VALUE(_hoist_home_sensor, rear_do_->_hoist_home_sensor);
                __ADD_KEY_VALUE(hand_open_sen, rear_do_->hand_open_sen);
                __ADD_KEY_VALUE(hand_close_sen, rear_do_->hand_close_sen);
                __ADD_KEY_VALUE(hand_gripper_left, rear_do_->hand_gripper_left);
                __ADD_KEY_VALUE(hand_gripper_right, rear_do_->hand_gripper_right);
                __ADD_KEY_VALUE(hand_push_sen, rear_do_->hand_push_sen);
                __ADD_KEY_VALUE(hand_inposition, rear_do_->hand_inposition);
                __ADD_KEY_VALUE(hand_alarm, rear_do_->hand_alarm);
                __ADD_KEY_VALUE(hand_pio_go, rear_do_->hand_pio_go);
                __ADD_KEY_VALUE(shutter_front_open_sen, front_do_->shutter_front_open_sen);
                __ADD_KEY_VALUE(shutter_front_close_sen, front_do_->shutter_front_close_sen);
                __ADD_KEY_VALUE(shutter_front_inposition, front_do_->shutter_front_inposition);
                __ADD_KEY_VALUE(shutter_front_alarm, front_do_->shutter_front_alarm);
                __ADD_KEY_VALUE(shutter_rear_open_sen, rear_do_->shutter_rear_open_sen);
                __ADD_KEY_VALUE(shutter_rear_close_sen, rear_do_->shutter_rear_close_sen);
                __ADD_KEY_VALUE(shutter_rear_inposition, rear_do_->shutter_rear_inposition);
                __ADD_KEY_VALUE(shutter_rear_alarm, rear_do_->shutter_rear_alarm);
                __ADD_KEY_VALUE(_foup_detect, front_do_->_foup_detect);
                __ADD_KEY_VALUE(_lookdown_detect, front_do_->_lookdown_detect);
                __ADD_KEY_VALUE(_lookdown_error, front_do_->_lookdown_error);

                // trans out
                __ADD_KEY_VALUE(steer_front_dir, front_di_->steer_front_dir);
                __ADD_KEY_VALUE(steer_front_torque0, front_di_->steer_front_torque0);
                __ADD_KEY_VALUE(steer_front_torque1, front_di_->steer_front_torque1);
                __ADD_KEY_VALUE(shutter_front_reset, front_di_->shutter_front_reset);
                __ADD_KEY_VALUE(shutter_front_close, front_di_->shutter_front_close);
                __ADD_KEY_VALUE(shutter_front_open, front_di_->shutter_front_open);
                __ADD_KEY_VALUE(shutter_front_origin, front_di_->shutter_front_origin);
                __ADD_KEY_VALUE(shutter_front_free, front_di_->shutter_front_free);
                __ADD_KEY_VALUE(shutter_rear_reset, rear_di_->shutter_rear_reset);
                __ADD_KEY_VALUE(shutter_rear_close, rear_di_->shutter_rear_close);
                __ADD_KEY_VALUE(shutter_rear_open, rear_di_->shutter_rear_open);
                __ADD_KEY_VALUE(shutter_rear_origin, rear_di_->shutter_rear_origin);
                __ADD_KEY_VALUE(shutter_rear_free, rear_di_->shutter_rear_free);
                __ADD_KEY_VALUE(hand_pio_select, rear_di_->hand_pio_select);
                __ADD_KEY_VALUE(hand_reset, rear_di_->hand_reset);
                __ADD_KEY_VALUE(hand_close, rear_di_->hand_close);
                __ADD_KEY_VALUE(hand_open, rear_di_->hand_open);
                __ADD_KEY_VALUE(hand_origin, rear_di_->hand_origin);
                __ADD_KEY_VALUE(hand_free, rear_di_->hand_free);
                __ADD_KEY_VALUE(_lookdown_power_reset, front_di_->_lookdown_power_reset);
                __ADD_KEY_VALUE(_lookdown_area1_sel, front_di_->_lookdown_area1_sel);
                __ADD_KEY_VALUE(_lookdown_area2_sel, front_di_->_lookdown_area2_sel);
                __ADD_KEY_VALUE(_lookdown_area3_sel, front_di_->_lookdown_area3_sel);
                __ADD_KEY_VALUE(_lookdown_area4_sel, front_di_->_lookdown_area4_sel);

                // eqpio
                __ADD_KEY_VALUE(eq_pio_load_req, rear_do_->eq_pio_load_req);
                __ADD_KEY_VALUE(eq_pio_unload_req, rear_do_->eq_pio_unload_req);
                __ADD_KEY_VALUE(eq_pio_ho_avbl, rear_do_->eq_pio_ho_avbl);
                __ADD_KEY_VALUE(eq_pio_es, rear_do_->eq_pio_es);
                __ADD_KEY_VALUE(eq_pio_right_go, rear_do_->eq_pio_right_go);
                __ADD_KEY_VALUE(eq_pio_left_go, rear_do_->eq_pio_left_go);
                __ADD_KEY_VALUE(eq_pio_ready, rear_do_->eq_pio_ready);
                __ADD_KEY_VALUE(eq_pio_cs0, rear_di_->eq_pio_cs0);
                __ADD_KEY_VALUE(eq_pio_valid, rear_di_->eq_pio_valid);
                __ADD_KEY_VALUE(eq_pio_tr_req, rear_di_->eq_pio_tr_req);
                __ADD_KEY_VALUE(eq_pio_busy, rear_di_->eq_pio_busy);
                __ADD_KEY_VALUE(eq_pio_complete, rear_di_->eq_pio_complete);
                __ADD_KEY_VALUE(_eq_pio_left_select, rear_di_->_eq_pio_left_select);
                __ADD_KEY_VALUE(_eq_pio_right_select, rear_di_->_eq_pio_right_select);

                // servo & power
                __ADD_KEY_VALUE(safety_control, front_di_->safety_control);
                __ADD_KEY_VALUE(safety_clear, front_di_->safety_clear);
                __ADD_KEY_VALUE(_hand_power_reset, front_di_->_hand_power_reset);


                // detect sensors
                __ADD_KEY_VALUE(_obstacle_area1_sel, onboard_di_->_obstacle_area1_sel);
                __ADD_KEY_VALUE(_obstacle_area2_sel, onboard_di_->_obstacle_area2_sel);
                __ADD_KEY_VALUE(_obstacle_area3_sel, onboard_di_->_obstacle_area3_sel);
                __ADD_KEY_VALUE(_obstacle_area4_sel, onboard_di_->_obstacle_area4_sel);
                __ADD_KEY_VALUE(_ohtdetect_area1_sel, onboard_di_->_ohtdetect_area1_sel);
                __ADD_KEY_VALUE(_ohtdetect_area2_sel, onboard_di_->_ohtdetect_area2_sel);
                __ADD_KEY_VALUE(_ohtdetect_area3_sel, onboard_di_->_ohtdetect_area3_sel);
                __ADD_KEY_VALUE(_ohtdetect_area4_sel, onboard_di_->_ohtdetect_area4_sel);

                __ADD_KEY_VALUE(_obstacle_long, onboard_do_->_obstacle_long);
                __ADD_KEY_VALUE(_obstacle_middle, onboard_do_->_obstacle_middle);
                __ADD_KEY_VALUE(_obstacle_short, onboard_do_->_obstacle_short);
                __ADD_KEY_VALUE(_obstacle_fail, onboard_do_->_obstacle_fail);
                __ADD_KEY_VALUE(_ohtdetect_long, onboard_do_->_ohtdetect_long);
                __ADD_KEY_VALUE(_ohtdetect_middle, onboard_do_->_ohtdetect_middle);
                __ADD_KEY_VALUE(_ohtdetect_short, onboard_do_->_ohtdetect_short);
                __ADD_KEY_VALUE(_ohtdetect_fail, onboard_do_->_ohtdetect_fail);
                __ADD_KEY_VALUE(bcr_trans_ok, onboard_do_->bcr_trans_ok);
                __ADD_KEY_VALUE(bcr_trans_ng, onboard_do_->bcr_trans_ng);
                __ADD_KEY_VALUE(bcr_driving_ok, onboard_do_->bcr_driving_ok);
                __ADD_KEY_VALUE(bcr_driving_ng, onboard_do_->bcr_driving_ng);

                __ADD_KEY_VALUE(lamp_yellow, front_di_->lamp_yellow);
                __ADD_KEY_VALUE(lamp_red, front_di_->lamp_red);
                __ADD_KEY_VALUE(lamp_green, front_di_->lamp_green);
                __ADD_KEY_VALUE(cid_reserved_1, front_di_->cid_reserved_1);
                __ADD_KEY_VALUE(cid_left_select, front_di_->cid_left_select);
                __ADD_KEY_VALUE(cid_right_select, front_di_->cid_right_select);
                __ADD_KEY_VALUE(cid_occup_req, front_di_->cid_occup_req);
                __ADD_KEY_VALUE(cid_pass_complete, front_di_->cid_pass_complete);
                __ADD_KEY_VALUE(cid_manual, front_di_->cid_manual);
                __ADD_KEY_VALUE(cid_reserved_2, front_di_->cid_reserved_2);
                __ADD_KEY_VALUE(utb_detect_sensor, front_di_->utb_detect_sensor);


                __ADD_KEY_VALUE(cid_reserved2, front_do_->cid_reserved2);
                __ADD_KEY_VALUE(cid_reserved1, front_do_->cid_reserved1);
                __ADD_KEY_VALUE(outrigger_home_sen, front_do_->outrigger_home_sen);
                __ADD_KEY_VALUE(emo_press, front_do_->emo_press);
                __ADD_KEY_VALUE(outrigger_work_sen, front_do_->outrigger_work_sen);
                __ADD_KEY_VALUE(cid_pass_ok, front_do_->cid_pass_ok);
                __ADD_KEY_VALUE(cid_link_complete, front_do_->cid_link_complete);
                __ADD_KEY_VALUE(cid_status, front_do_->cid_status);
                __ADD_KEY_VALUE(cid_interlock, front_do_->cid_interlock);
                __ADD_KEY_VALUE(_utb_detect, front_do_->_utb_detect);
                __ADD_KEY_VALUE(bumper_press, front_do_->bumper_press);
                __ADD_KEY_VALUE(_stb_detect_left, front_do_->_stb_detect_left);
                __ADD_KEY_VALUE(_stb_detect_right, front_do_->_stb_detect_right);
                __ADD_KEY_VALUE(_oscillation_sensor, front_do_->_oscillation_sensor);

                __ADD_KEY_VALUE(hoist_ext_brake_release, rear_di_->hoist_ext_brake_release);
                __ADD_KEY_VALUE(driving_brake_release, rear_di_->driving_brake_release);
                __ADD_KEY_VALUE(steer_rear_torque0, rear_di_->steer_rear_torque0);
                __ADD_KEY_VALUE(steer_rear_torque1, rear_di_->steer_rear_torque1);
                __ADD_KEY_VALUE(steer_rear_dir, rear_di_->steer_rear_dir);


                __ADD_KEY_VALUE(_hoist_belt_detect_1, rear_do_->_hoist_belt_detect_1);
                __ADD_KEY_VALUE(_hoist_belt_detect_2, rear_do_->_hoist_belt_detect_2);
                __ADD_KEY_VALUE(handtp_connect, rear_do_->handtp_connect);
                __ADD_KEY_VALUE(_foup_cover_open_detect, rear_do_->_foup_cover_open_detect);
                __ADD_KEY_VALUE(_fan_fail, rear_do_->_fan_fail);
                __ADD_KEY_VALUE(tp_enable, rear_do_->tp_enable);
                __ADD_KEY_VALUE(_smps_short_check, rear_do_->_smps_short_check);
                __ADD_KEY_VALUE(pio_input3, rear_do_->pio_input3);
                __ADD_KEY_VALUE(pio_input5, rear_do_->pio_input5);
                __ADD_KEY_VALUE(_pio_input6, rear_do_->_pio_input6);

        __END_MAP();
            __ADD_KEY_VALUE(has_foup, oht_.has_foup);
        __END_MAP();
        // foup locations : [ station id, oht ]
        __ADD_KEY_VALUE(foups, foup_list_);
        // nearest station
        auto near_sta = curr_link->getStation(oht_offset, param_.station_mark_front, rear_margin);
        if (near_sta != nullptr &&  near_sta->teach != nullptr) {
            auto& sta = station_map_[near_sta->id];
            auto& pio = sta.pio;
            __BEGIN_MAP(station);
                __ADD_KEY_VALUE(id, sta.getID());
                __ADD_KEY_VALUE(hoist, sta.getHoistOffset());
                __ADD_KEY_VALUE(slide, sta.getSlideOffset());
                __ADD_KEY_VALUE(has_foup, sta.hasFoup());
            __END_MAP();
        }
        status << YAML::EndMap;
        return status.c_str();
#undef __ADD_KEY_VALUE
#undef __END_MAP
#undef __BEGIN_MAP
    }


}
