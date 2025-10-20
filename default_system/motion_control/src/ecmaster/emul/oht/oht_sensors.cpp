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
// Created by root on 17. 12. 6.
//

#include "oht_simulator.h"
using namespace sephi;
using namespace sephi::oht_sim;

const unsigned SENSOR_DETECTED = 1; // change value to 0 when active low
const unsigned SENSOR_NOT_DETECTED  = 1-SENSOR_DETECTED;

void OHTSystem::triggerSteer()
{
    auto job = [](OHTSystem*self, int state) {
        if (state == static_cast<int>(SteeringState ::LEFT)) {
            self->oht_.steer_state = SteeringState::RIGHT;
        } else {
            self->oht_.steer_state = SteeringState::LEFT;
        }
        SEP_LOGT("steering trigger command exec: %d\n", static_cast<int>(self->oht_.steer_state));
    };
    SEP_LOGT("steering trigger command reserve: %d\n", static_cast<int>(oht_.steer_state));
    delayed_jobs_.push_back(TimeoutJob(TimeoutJob::RTP_STEER, job, static_cast<int>(oht_.steer_state)));
    oht_.steer_state = SteeringState::TRANSIENT;
}

void OHTSystem::updateDrivngSensors()
{
    // abbreviations
    auto oht_offset = oht_.location.second;
    auto& curr_link = oht_.location.first;

    // precheck : node mark detection
    // turn on when node.x - node_mark_size/2 <=  oht.x + precheck2stop <= node.x + node_mark_size/2
    if ( fabs(curr_link->distance - param_.precheck2stop - oht_offset ) <= param_.node_mark ){
        onboard_do_->_stop_driving_precheck = SENSOR_DETECTED;
    } else {
        onboard_do_->_stop_driving_precheck = SENSOR_NOT_DETECTED;
    }

    // driving stop
    if ( curr_link->distance - oht_offset <= param_.node_mark/2 || oht_offset <= param_.node_mark/2 ){
        onboard_do_->_stop_driving_front = SENSOR_DETECTED;
        if (oht_offset <= param_.node_mark/2) {
            driving_bcr_->setData(curr_link->from);
        } else {
            driving_bcr_->setData(curr_link->to);
        }
    } else {
        onboard_do_->_stop_driving_front = SENSOR_NOT_DETECTED;
    }

    // station marking sensors
    auto rear_margin = param_.station_mark-param_.station_mark_front;
    auto sta  = curr_link->getStation(oht_offset, param_.station_mark_front, rear_margin);
    if (sta == nullptr){
        double remain = curr_link->distance - oht_offset;
        //SEP_LOGT("remain = %f\n", remain);
        if (remain <  param_.station_mark_front) { // find from next link
            auto next_link = map_.getLink(curr_link->to);
            oht_offset -= curr_link->distance;
            sta = next_link->getStation(oht_offset, param_.station_mark_front, rear_margin);
            //SEP_LOGT("oht offset = %f, sta = %p\n", oht_offset, sta);

        } else if (oht_offset < rear_margin) { // find prev
            auto prev_link = map_.getLinkTo(curr_link->from);
            oht_offset += prev_link->distance;
            sta = prev_link->getStation(oht_offset, param_.station_mark_front, rear_margin);
        }
    }
    if (sta != nullptr) {
        onboard_do_->_stop_trans_front = SENSOR_DETECTED;
        if ( fabs(sta->offset - oht_offset) <= param_.station_stop_mark/2 ) {
            onboard_do_->_stop_trans_rear = SENSOR_DETECTED;
            trans_bcr_->setData(sta->id);
        } else {
            onboard_do_->_stop_trans_rear = SENSOR_NOT_DETECTED;
        }
    } else {
        onboard_do_->_stop_trans_front = SENSOR_NOT_DETECTED;
        onboard_do_->_stop_trans_rear = SENSOR_NOT_DETECTED;
    }

    // steering sensors; ignore steer_rear_dir, trq0, trq1
    if (oht_.steer_state != SteeringState::TRANSIENT) {
        // steer_front_dir 0: left, 1: right
        if (front_di_->steer_front_dir == 0 && oht_.steer_state != SteeringState::LEFT
            || front_di_->steer_front_dir == 1 && oht_.steer_state != SteeringState::RIGHT) {
            triggerSteer();
        }
    }
    auto setSteerDO = [this](unsigned left, unsigned right) {
        front_do_->steer_front_left = left;
        front_do_->steer_front_right = right;
        rear_do_->steer_rear_left = left;
        rear_do_->steer_rear_right = right;
    };
    if (oht_.steer_state == SteeringState::LEFT) {
        setSteerDO(1,0);
    } else if (oht_.steer_state == SteeringState::RIGHT) {
        setSteerDO(0,1);
    } else {
        setSteerDO(0,0);
    }

    // homing status
    rear_do_->_hoist_home_sensor = !fabs(oht_.hoist <= oht_.inpos[OHTInfo::HOIST]);
    front_do_->slide_home_sensor =  fabs(oht_.slide <= oht_.inpos[OHTInfo::SLIDE]);

    // bcr check
    for(auto bcr : {driving_bcr_.get(), trans_bcr_.get()}) {
        bcr->checkTrigger();
    }
    onboard_do_->bcr_driving_ok = SENSOR_NOT_DETECTED;
    onboard_do_->bcr_driving_ng = SENSOR_DETECTED;
    onboard_do_->bcr_trans_ok = SENSOR_NOT_DETECTED;
    onboard_do_->bcr_trans_ng = SENSOR_DETECTED;
}

void OHTSystem::clearPioSignal()
{
    rear_do_->eq_pio_load_req = 0;
    rear_do_->eq_pio_unload_req = 0;
    rear_do_->eq_pio_ready = 0;

    rear_do_->eq_pio_right_go = 0;
    rear_do_->eq_pio_left_go = 0;

    // stationary signals
    rear_do_->eq_pio_ho_avbl = 0;
    rear_do_->eq_pio_es = 0;
}

void OHTSystem::updateEQportSensors()
{
    struct CleanUp {
        CleanUp(OHTSystem* _self):self_(_self), hand_push(0) {}
        ~CleanUp() {
            self_->rear_do_->hand_push_sen = hand_push;
            //if(hand_push) SEP_LOGT("[updateEQportSensors] ~Clean Up: hand_push_sen = %d", self_->rear_do_->hand_push_sen);
        }
        unsigned  hand_push;
    private:
        OHTSystem* self_;
    } cleanup(this);


    // abbreviations
    auto& oht_offset = oht_.location.second;

    if (current_station_ == OHTStation::INVALID_STATION_ID) { // not connected
        clearPioSignal();
        return;
    }

    // EQ port location w.r.t. OHT location
    auto& eq = station_map_[current_station_];
    auto& eqpio = eq.pio;

    // check if mark completed
    if (!eq.isOhtInStation(oht_offset)) return;
    if (!eq.isValidStation()) return; // can't load or unload

    // signal to EQ port
    eqpio.setCSn(0, rear_di_->eq_pio_cs0);
    eqpio.setCSn(1, rear_di_->eq_pio_cs1);
    eqpio.setCSn(2, rear_di_->eq_pio_cs2);
    eqpio.setCSn(3, rear_di_->eq_pio_cs3);
    eqpio.setValid(rear_di_->eq_pio_valid);
    eqpio.setTrReq(rear_di_->eq_pio_tr_req);
    eqpio.setBusy(rear_di_->eq_pio_busy);
    eqpio.setComplete(rear_di_->eq_pio_complete);
    eqpio.update(); // delayed job

    // detect carrier transfer
    if (eq.inLoadSequence() && oht_.has_foup  || eq.inUnloadSequence() && !oht_.has_foup) {
        if (eq.checkCarrierTransfer(oht_, foup_list_)) { // transferred
            oht_.has_foup = !oht_.has_foup;
        }
    }

    // read out result signal
    rear_do_->eq_pio_load_req = (eq.inLoadSequence()) ? eqpio.getLoadUnloadReq() : 0;
    rear_do_->eq_pio_unload_req = (!eq.inLoadSequence()) ? eqpio.getLoadUnloadReq() : 0;
    rear_do_->eq_pio_ready = eqpio.getReady();

    rear_do_->eq_pio_right_go = rear_di_->_eq_pio_right_select ? 0 : 1;
    rear_do_->eq_pio_left_go = rear_di_->_eq_pio_left_select ? 0 : 1;

    // stationary signals
    rear_do_->eq_pio_ho_avbl = 1; // always true in normal case
    rear_do_->eq_pio_es = 1; // always true in normal case

    // update hand push state
    cleanup.hand_push = eq.isHandPushed(oht_);
    return;
}

void OHTSystem::updateSTBportSensors()
{

}

void OHTSystem::triggerGrip()
{
    auto job = [](OHTSystem*self, int state) {
        if (state == static_cast<int>(HandOpenCloseState::OPEN)) {
            self->oht_.hand_state = HandOpenCloseState::CLOSE;
        } else {
            self->oht_.hand_state = HandOpenCloseState::OPEN;
        }
    };
    delayed_jobs_.push_back(TimeoutJob(TimeoutJob::RTP_GRIP, job, static_cast<int>(oht_.hand_state)));
}

void OHTSystem::triggerShutter()
{
    auto job = [](OHTSystem*self, int state) {
        if (state == static_cast<int>(ShutterOpenCloseState::OPEN)) {
            self->oht_.shutter_state = ShutterOpenCloseState::CLOSE;
        } else {
            self->oht_.shutter_state = ShutterOpenCloseState::OPEN;
        }
    };
    delayed_jobs_.push_back(TimeoutJob(TimeoutJob::RTP_SHUTTER, job, static_cast<int>(oht_.shutter_state)));
}

// update sensor output related with transportation
void OHTSystem::updateTransSensors()
{
    // hand pio
    //if (rear_di_->hand_pio_select) {
        rear_do_->hand_pio_go = 1;      //should be always on to make TP ready
    //}

    auto updateHandOpenCloseState = [this](HandOpenCloseState& state, bool command_flag, HandOpenCloseState target_state) {
        if (command_flag) {
            if (state != HandOpenCloseState::TRANSIENT && state != target_state) {
                    triggerGrip();
                state = HandOpenCloseState::TRANSIENT;
            }
        }
    };

    auto updatesShutterOpenCloseState = [this](ShutterOpenCloseState& state, bool command_flag, ShutterOpenCloseState target_state) {
        if (command_flag) {
            if (state != ShutterOpenCloseState::TRANSIENT && state != target_state) {
                    triggerShutter();
                state = ShutterOpenCloseState::TRANSIENT;
            }
        }
    };

    // process open/close commands
    updateHandOpenCloseState(oht_.hand_state, rear_di_->hand_close, HandOpenCloseState::CLOSE);
    updateHandOpenCloseState(oht_.hand_state, rear_di_->hand_open, HandOpenCloseState::OPEN);
    updateHandOpenCloseState(oht_.hand_state, rear_di_->hand_origin, HandOpenCloseState::ORIGIN);
    updatesShutterOpenCloseState(oht_.shutter_state, rear_di_->shutter_rear_close, ShutterOpenCloseState::CLOSE);
    updatesShutterOpenCloseState(oht_.shutter_state, rear_di_->shutter_rear_open, ShutterOpenCloseState::OPEN);
    updatesShutterOpenCloseState(oht_.shutter_state, rear_di_->shutter_rear_origin, ShutterOpenCloseState::ORIGIN);
    updatesShutterOpenCloseState(oht_.shutter_state, front_di_->shutter_front_close, ShutterOpenCloseState::CLOSE);
    updatesShutterOpenCloseState(oht_.shutter_state, front_di_->shutter_front_open, ShutterOpenCloseState::OPEN);
    updatesShutterOpenCloseState(oht_.shutter_state, front_di_->shutter_front_origin, ShutterOpenCloseState::ORIGIN);

    // alarm state
    if (rear_di_->hand_reset) {
        rear_do_->hand_alarm = 0;
    }
    if (rear_di_->shutter_rear_reset) {
        rear_do_->shutter_rear_alarm = 0;
    }
    if (front_di_->shutter_front_reset) {
        front_do_->shutter_front_alarm = 0;
    }

    auto setHandDO = [this](unsigned inposition, unsigned open_sen, unsigned close_sen, unsigned grip_left, unsigned grip_right) {
        rear_do_->hand_inposition = inposition;
        rear_do_->hand_open_sen = open_sen;
        rear_do_->hand_close_sen = close_sen;
        rear_do_->hand_gripper_left = grip_left;
        rear_do_->hand_gripper_right = grip_right;
    };

    // update open/close state output
    if (oht_.hand_state == HandOpenCloseState::OPEN) {
        setHandDO(1,1,0,0,0);
    } else if (oht_.hand_state == HandOpenCloseState::CLOSE) {
        setHandDO(1,0,1,1,1);
    } else {
        setHandDO(0,0,0,0,0);
    }

    auto setShutterDO = [this](unsigned inposition, unsigned open_sen, unsigned close_sen) {
        rear_do_->shutter_rear_inposition = inposition;
        rear_do_->shutter_rear_open_sen = open_sen;
        rear_do_->shutter_rear_close_sen = close_sen;
        front_do_->shutter_front_inposition = inposition;
        front_do_->shutter_front_open_sen = open_sen;
        front_do_->shutter_front_close_sen = close_sen;
    };
    // shutter state
    if (oht_.shutter_state == ShutterOpenCloseState::OPEN) {
        setShutterDO(1,1,0);
    } else if (oht_.shutter_state == ShutterOpenCloseState::CLOSE) {
        setShutterDO(1,0,1);
    } else {
        setShutterDO(0,0,0);
    }

    // foup detect
    front_do_->_foup_detect = !(oht_.has_foup && !rear_do_->_hoist_home_sensor && front_do_->slide_home_sensor);
}

// other sensors to be modelled
void OHTSystem::updateAuxSensors()
{
    // lookdown sensors
    front_do_->_lookdown_detect = 1; // not detected
    front_do_->_lookdown_error = 1;

    //utb/stb detect sensors
    front_do_->_utb_detect = 1; // not detected
    front_do_->_stb_detect_left = 1; // not detected
    front_do_->_stb_detect_right = 1;
    front_do_->_oscillation_sensor = 1;

    //safety related
    front_do_->emo_press = emo_press_state_ ? 1 : 0;
    rear_do_->_hoist_belt_detect_1 = 1; // not detected
    rear_do_->_hoist_belt_detect_2 = 1;
    rear_do_->_foup_cover_open_detect = 1; // not detected
    rear_do_->_fan_fail = 1; // not detected
    rear_do_->_smps_short_check = 1; // not detected

    onboard_do_->_obstacle_fail = 1;
    onboard_do_->_obstacle_long = 1;
    onboard_do_->_obstacle_middle = 1;
    onboard_do_->_obstacle_short = 1;

    // other oht detection
    if (other_oht_.isEnabled()) {
        other_oht_.update();
        OHTMap::Location oht_loc(oht_.location.first->from, oht_.location.first->to, oht_.location.second);
        auto other_oht_loc = other_oht_.travel().getLocation();
        double distance = map_.calcDistanceTo(oht_loc, other_oht_loc);
        onboard_do_->_ohtdetect_long = 1;
        onboard_do_->_ohtdetect_middle = 1;
        onboard_do_->_ohtdetect_short = 1;
        onboard_do_->_ohtdetect_fail = 1;

        bool detected(true);
        if (distance <= OHTInfo::OHT_SENS_RANGE_1) {
            onboard_do_->_ohtdetect_short = 0;
        } else if (distance <= OHTInfo::OHT_SENS_RANGE_3) {
            onboard_do_->_ohtdetect_long = 0;
            onboard_do_->_ohtdetect_middle = 0;
            onboard_do_->_ohtdetect_short = 0;
        } else if (distance <= OHTInfo::OHT_SENS_RANGE_4) {
            onboard_do_->_ohtdetect_middle = 0;
            onboard_do_->_ohtdetect_short = 0;
        } else if (distance <= OHTInfo::OHT_SENS_RANGE_5) {
            onboard_do_->_ohtdetect_middle = 0;
        } else if (distance <= OHTInfo::OHT_SENS_RANGE_6) {
            onboard_do_->_ohtdetect_long = 0;
            onboard_do_->_ohtdetect_middle = 0;
        } else if (distance <= OHTInfo::OHT_SENS_RANGE_7) {
            onboard_do_->_ohtdetect_long = 0;
        } else  {
            detected = false;
        }
        DebugState::set(DebugState::OHT_DISTANCE_MM, distance);
//        if (detected) {
//            SEP_LOGT("oht_distance=%.3f  f:%d l:%d m:%d s:%d\n",
//                     distance, onboard_do_->_ohtdetect_fail, onboard_do_->_ohtdetect_long,
//                     onboard_do_->_ohtdetect_middle, onboard_do_->_ohtdetect_short);
//        }
    } else {
        onboard_do_->_ohtdetect_fail = 1;
        onboard_do_->_ohtdetect_long = 1;
        onboard_do_->_ohtdetect_middle = 1;
        onboard_do_->_ohtdetect_short = 1;
    }
}
