
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
// Created by root on 17. 12. 11.
//
#include <unordered_map>
#include "oht_behaviors.h"
#include "oht_controller.h"

extern void logStackTrace();
using namespace sephi;
using namespace sephi::oht;
using namespace sephi::oht::behavior;

//static std::set<unsigned> bit_inversion_ports_ = {
//        _stop_driving_front,
//        _stop_driving_precheck,
//        _stop_trans_front,
//        _stop_trans_rear,
//
//        bcr_driving_ok,
//        bcr_driving_ng,
//        bcr_trans_ok,
//        bcr_trans_ng,
//
//		_obs_en,
//		_oht_en,
//};

//static std::set<unsigned> active_low_ports_ = {

        // OhtOnboard DI
//        _stop_trans_front            ,  //active low
//        _stop_trans_rear             ,  //active low
//        _stop_driving_front          ,  //active low
//        _stop_driving_precheck       ,  //active low
/*
        _obstacle_long               ,  //active low
        _obstacle_middle             ,  //active low
        _obstacle_short              ,  //active low
        _obstacle_fail               ,  //active low
        _ohtdetect_long              ,  //active low
        _ohtdetect_middle            ,  //active low
        _ohtdetect_short             ,  //active low
        _ohtdetect_fail              ,  //active low

        // OhtOnboard DO
        _obstacle_area1_sel          ,
        _obstacle_area2_sel          ,
        _obstacle_area3_sel          ,
        _obstacle_area4_sel          ,
        _ohtdetect_area1_sel         ,
        _ohtdetect_area2_sel         ,
        _ohtdetect_area3_sel         ,
        _ohtdetect_area4_sel         ,

        // OhtFront DO
        //_hand_power_reset            , 20190501 lsj delete
        //_lookdown_power_reset        , 20190501 lsj delete
        _lookdown_area1_sel          ,
        _lookdown_area2_sel          ,
        _lookdown_area3_sel          ,
        _lookdown_area4_sel          ,

        // OhtFront DI
        _lookdown_error              ,  //; active low
        _lookdown_detect             ,  //; active low
        emo_press                    ,
        //_utb_detect                  ,  //; active low 20190501 lsj delete
        _foup_detect                 ,  //; active low
        _stb_detect_left             ,  //; active low
        _stb_detect_right            ,  //; active low
        _oscillation_sensor          , //20190502 delete lsj

        // OhtRear DO
        _eq_pio_right_select         ,  //
        _eq_pio_left_select          ,  //

        // OhtRear DI
        //_hoist_home_sensor           ,  //; active low
        _hoist_belt_detect_1         ,  //; active low
        _hoist_belt_detect_2         ,  //; active low
        _foup_cover_open_detect      ,  //; active low 20190502 delete lsj
        _fan_fail                    ,  //; active low
        _smps_short_check            ,  //; active low
        _pio_input6                  ,  //; active low
        slide_home_sensor            ,
        HOIST_ALIGN_SENSOR_M         ,
*/
//};

//static std::unordered_map<unsigned, bool> io_val_map = {
// OhtOnboard DO
/*        {_obstacle_area1_sel,      false},
        {_obstacle_area2_sel,      false},
        {_obstacle_area3_sel,      false},
        {_obstacle_area4_sel,      false},
        {_obstacle_area5_sel,      false},
        {_ohtdetect_area1_sel,     false},
        {_ohtdetect_area2_sel,     false},
        {_ohtdetect_area3_sel,     false},
        {_ohtdetect_area4_sel,     false},
        {_ohtdetect_area5_sel,     false},
        {_oht_en,                  false},
        {_obs_en,                  false},
        {qrr1_in1,                 false},
        {bcr1_in2,                 false},
        {qrr2_in3,                 false},
        {bcr2_in4,                 false},
        {driving_stop_in1,         false},
        {trans_stop_in2,           false},

        {servo_brake1,             false},
        {servo_brake2,             false},
        {servo_brake3,             false},
        {servo_brake4,             false},
        {limit_in1,                false},
        {limit_in2,                false},
        {_obstacle_power_reset,    false},
        {_ohtdetect_power_reset,   false},

// OhtOnboard DI
        {_ohtdetect_short,         false},
        {_ohtdetect_long,          false},
        {_ohtdetect_middle,        false},
        {_ohtdetect_fail,          false},
        {_stop_driving_front,      false},
        {_stop_driving_precheck,   false},
        {bcr_driving_ok,           false},
        {bcr_driving_ng,           false},
        {_stop_trans_front,        false},
        {_stop_trans_rear,         false},
        {bcr_trans_ok,             false},
        {bcr_trans_ng,             false},
        {obs_status,               false},
        {oht_status,               false},
        {driving_stop_st1,         false},
        {trans_stop_st2,           false},
        {qrr1_st1,                 false},
        {bcr1_st2,                 false},
        {qrr2_st3,                 false},
        {bcr2_st4,                 false},
        {_obstacle_long,           false},
        {_obstacle_middle,         false},
        {_obstacle_short,          false},
        {_obstacle_fail,           false},

// OhtFront DO
        //{lamp_yellow,              false},
        {_lookdown_area1_sel,      false},
        {lamp_red,                 false},
        {LAMP_RIGHT_M,             false}, // 20190502 insert lsj
        {lamp_green,               false},
        {cid_reserved_1,           false},
        {safety_clear,             false},
        //{_hand_power_reset,        false}, 20190501 lsj delete
        {cid_left_select,          false},
        {cid_right_select,         false},
        {cid_occup_req,            false},
        {cid_pass_complete,        false},
        {cid_manual,               false},
        {CID_RESERVED_OUT3_M,      false}, // 20190502 insert lsj
        {cid_reserved_2,           false},
        {Reserved_Output1_M,       false}, // 20190502 insert lsj
        {LD_Power_EN,              false}, // 20190502 insert lsj
        //{_lookdown_power_reset,    false}, 20190501 lsj delete
        {safety_control,           false},
        {Reserved_Output2_M,       false}, // 20190502 insert lsj
        {Reserved_Output3_M,       false}, // 20190502 insert lsj
        {Reserved_Output4_M,       false}, // 20190502 insert lsj
        {_lookdown_area4_sel,      false},
        {_lookdown_area2_sel,      false},
        {_lookdown_area3_sel,      false},
        {steer_front_torque0,      false},
        {steer_front_torque1,      false},
        {steer_front_dir,          false},
        {shutter_front_reset,      false},
        {shutter_front_close,      false},
        {shutter_front_open,       false},
        {shutter_front_origin,     false},
        {shutter_front_free,       false},
        {CID_Power_EN,             false}, // 20190501 lsj insert
        {V12_PWM,                  false}, // 20190501 lsj insert
        //{STEP_STEP,                false}, // 20190501 lsj insert
        //{LAN_PWR_EN,               false}, // 20190501 lsj insert
        //{utb_detect_sensor,        false}, 20190501 lsj delete

// OhtFront DI
        {steer_front_left,         false},
        //{SWV,                      false}, // 20190502 insert lsj
        {cid_reserved2,            false},
        {NFAULT_F,                 false}, // 20190502 insert lsj
        {cid_reserved1,            false}, // 20190502 insert lsj
        //{outrigger_home_sen,       false}, 20190502 delete lsj
        //{emo_press,                false}, 20190502 delete lsj
        //{outrigger_work_sen,       false}, 20190502 delete lsj
        {shutter_front_open_sen,   false},
        {cid_pass_ok,              false},
        {cid_link_complete,        false},
        {cid_status,               false},
        {cid_interlock,            false},
        {CID_Power_ST,             false}, // 20190502 insert lsj
        {LD_Power_ST,              false}, // 20190502 insert lsj
        {steer_front_right,        false},
        {shutter_front_close_sen,  false},
        {_lookdown_error,          false},
        {_lookdown_detect,         false},
        {Reserved_Input1_M,        false}, // 20190502 insert lsj
        {Reserved_Input2_M,        false}, // 20190502 insert lsj
        {Reserved_Input3_M,        false}, // 20190502 insert lsj
        {Reserved_Input4_M,        false}, // 20190502 insert lsj
        {slide_home_sensor,        false},
        {bumper_press,             false}, // 20190502 insert lsj
        //{_utb_detect,              false}, 20190502 delete lsj
        //{_foup_detect,             false}, 20190502 delete lsj
        {_stb_detect_left,         false},
        {_stb_detect_right,        false},
        {_foup_detect,             false}, // 20190502 insert lsj
        //{_oscillation_sensor,      false}, 20190502 delete lsj
        {shutter_front_alarm,      false},
        {shutter_front_inposition, false},
        {SNSOUT_F,                 false}, // 20190501 insert lsj
        //{LAN_PWR_ST,               false}, // 20190501 insert lsj

// OhtRear DO
        {hand_pio_select,          false},
        {hand_close,               false},
        {hand_open,                false},
        {hand_origin,              false},
        {hand_reset,               false},
        {hand_free,                false},
        {hand_reserved_6,          false},
        {hand_reserved_7,          false},
        {hand_reserved_8,          false},
        {eq_pio_valid,             false},
        {eq_pio_cs0,               false},
        {eq_pio_cs1,               false},
        {eq_pio_cs2,               false},
        {eq_pio_cs3,               false},
        {eq_pio_tr_req,            false},
        {eq_pio_busy,              false},
        {eq_pio_complete,          false},
        {_eq_pio_right_select,     false},
        {_eq_pio_left_select,      false},
        {hoist_ext_brake_release,  false},
        {driving_brake_release,    false},
        {EXTRA_OUTPUT1_M,          false}, // 20190502 insert lsj
        {EXTRA_OUTPUT2_M,          false}, // 20190502 insert lsj
        {EXTRA_OUTPUT3_M,          false}, // 20190502 insert lsj
        {steer_rear_torque0,       false},
        {steer_rear_torque1,       false},
        {steer_rear_dir,           false},
        {shutter_rear_reset,       false},
        {shutter_rear_close,       false},
        {shutter_rear_open,        false},
        {shutter_rear_origin,      false},
        {shutter_rear_free,        false},
        {HAND_POWER_RESET_M,       false}, // 20190502 insert lsj



// OhtRear DI
        {hand_pio_go,              false},
        {hand_inposition,          false},
        {hand_alarm,               false},
        {hand_gripper_right,       false},
        {hand_gripper_left,        false},
        {hand_close_sen,           false},
        {hand_open_sen,            false},
        {hand_push_sen,            false},
        {hand_es,                  false},
        {Hand_Align_Sensor_M,      false}, // 20190502 insert lsj
        //{SWV_Rear,                 false}, // 20190502 insert lsj
        {eq_pio_load_req,          false},
        {eq_pio_unload_req,        false},
        {eq_pio_ready,             false},
        {eq_pio_ho_avbl,           false},
        {eq_pio_es,                false},
        {eq_pio_right_go,          false},
        {eq_pio_left_go,           false},
        //{_hoist_home_sensor,       false},
        {_hoist_belt_detect_1,     false},
        {_hoist_belt_detect_2,     false},
        {steer_rear_left,          false},
        {steer_rear_right,         false},
        {shutter_rear_close_sen,   false},
        {shutter_rear_open_sen,    false},
        {REMOCON_CONNECT_M,        false}, // 20190502 insert lsj
        //{handtp_connect,           false}, 20190502 delete lsj
        {_foup_cover_open_detect,  false}, // 20190502 delete lsj
        {_fan_fail,                false},
        //{tp_enable,                false}, 20190502 delete lsj
        {EXTRA_INPUT1_M,           false}, // 20190502 insert lsj
        {EXTRA_INPUT2_M,           false}, // 20190502 insert lsj
        {HOIST_ALIGN_SENSOR_M,     false}, // 210602 yjchoe
        {nFAULT_R,                 false}, // 20190502 insert lsj
        {SNSOUT_R,                 false}, // 20190502 insert lsj
        {_smps_short_check,        false},
        {shutter_rear_alarm,       false},
        {shutter_rear_inposition,  false},
        //{OSCILLATION_SENSOR_M,     false}, // 20190502 insert lsj 20190507 delete lsj
        {_oscillation_sensor,      false},
        {_foup_detect,             false}, // 20190502 insert lsj
        {emo_press,                false},
        //{E_STOP_M,                 false}, // 20190502 delete lsj
        {pio_input3,               false},
        {pio_input5,               false},
        {_pio_input6,              false},*/
//};


static void detectIoToggle(unsigned portid, bool value)
{
    try {
        if (io_val_map[portid] != value) {
          //  SEP_LOGT("IO toggle: %s changes into %d", getOhtIoPortString(portid), value);
            //SEP_LOGT("IO toggle: %s changes into %d", owner_->ioConfig_.getportstring(portid).c_str(), value);
            io_val_map[portid] = value;
        }
    } catch (...) {
        SEP_LOGE("Invalid port id 0x%08x", portid);
    }

}

ActiveObject::ActiveObject()
{
    thread_stop_ = false;
    initialized_ = false;
    thread_paused_ = false;
    update_cycletime_ = UPDATE_CYCLE_US*1e-6;
}

bool ActiveObject::runAsThread(int cycle_us)
{
    th_ = std::make_unique<std::thread>([this, cycle_us](){
        initialize();
        while(!this->thread_stop_) {
            lock_.lock();
            this->update();
            lock_.unlock();
            usleep(cycle_us);
        }
    });
    return th_ != nullptr;
}

bool ActiveObject::isPaused()
{
    return th_ && thread_paused_;
}

/*
bool ActiveObject::pauseThread()
{
    if (th_) {
        if (!isPaused()) {
            thread_paused_ = true;
            lock_.lock();
        }
        return true;
    }
    return false;
}

bool ActiveObject::resumeThread()
{
    if (th_) {
        if (isPaused()) {
            lock_.unlock();
            thread_paused_ = false;
        }
        return true;
    }
    return false;
}
*/

void ActiveObject::stopThread()
{
    thread_stop_ = true;
    if (th_) th_->join();
    th_.reset();
}


Behavior::Behavior(OHTController *controller)
    :owner_(controller),  map_(controller->map_), config_(controller->config_),
     onboard_io_(controller->onboard_io_), front_io_(*controller->front_io_), rear_io_(*controller->rear_io_)
{
    name_ = "Behavior";
}

bool Behavior::readRawDi(unsigned portid)
{
    return owner_->readRawDi(portid);
}

bool Behavior::readRawDo(unsigned portid)
{
    return owner_->readRawDo(portid);
}

bool Behavior::writeRawDo(unsigned portid, bool value)
{
    return owner_->writeRawDo(portid, value);
}

bool Behavior::checkDi(unsigned portid, bool expected_value)
{
    if (readDi(portid) != expected_value) {
        //SEP_LOGE("%s is not %d\n", getOhtIoPortString(portid), expected_value);
        SEP_LOGE("%s is not %d\n", owner_->ioConfig_.getportstring(portid).c_str(), expected_value);
        return false;
    }
    return true;
}

bool Behavior::checkDiAny(std::initializer_list<std::pair<unsigned,bool>>& sensors)
{
    for(auto& sensor: sensors) {
        if (readDi(sensor.first) == sensor.second) return true;
    }
    return  false;
}

bool Behavior::checkDiAll(std::initializer_list<std::pair<unsigned,bool>>& sensors)
{
    for(auto& sensor: sensors) {
        if (readDi(sensor.first) != sensor.second) return false;
    }
    return  true;
}

bool Behavior::readDi(unsigned portid)
{
    return owner_->readDi(portid);
}

bool Behavior::readDo(unsigned portid)
{
    return owner_->readDo(portid);
}

bool Behavior::writeDo(unsigned portid, bool value)
{
   // printf("TEST!!!!!!!!!!!!!!!!!!! owner_:%p\n",owner_);
    return owner_->writeDo(portid, value);
}

/*
bool Behavior::checkRawDi(unsigned portid, bool expected_value)
{
    if (readRawDi(portid) != expected_value) {
        SEP_LOGE("%s is not %d\n", getOhtIoPortString(portid), expected_value);
        return false;
    }
    return true;
}
 */

int Behavior::pollSensors(int timeout_ms,std::initializer_list<std::pair<unsigned,bool>> sensors, bool all)
{
    timer_.setTs(timeout_ms);
    while(!timer_.isTimeout()) {
        auto sensed = (all) ? checkDiAll(sensors) : checkDiAny(sensors);
        if (sensed) {
            return ReturnCode::NO_ERROR;
        }
        sleep_for(1ms);
    }
    string logmsg;
    for(auto& kv: sensors) {
        char buff[1024];
        //sprintf(buff, "%s(expected=%d,real=%d) ", getOhtIoPortString(kv.first), kv.second, readDi(kv.first));
        sprintf(buff, "%s(expected=%d,real=%d) ", owner_->ioConfig_.getportstring(kv.first).c_str(), kv.second, readDi(kv.first));
        logmsg += buff;
    }
    SEP_LOGE("%s Timeout error\n", logmsg.c_str());
    return ReturnCode::ERROR;
}


double Behavior::getPosmm(int axis)
{
    return owner_->getPosmm(axis);
}

double Behavior::getPos(int axis)
{
    return owner_->getPos(axis);
}

bool OHTController::readRawDi(unsigned portid)
{
    unsigned board = portid >> 24;
    unsigned port_bit = portid & 0x00FFFFFF;
    bool ret(false);

    if (portid == 0)    return ret;

    switch(board){
        case OhtIoBoardId::OHT_ONBOARD_DI:  ret = onboard_io_.readDi(port_bit); break;
        case OhtIoBoardId::OHT_FRONT_DI:    ret = front_io_->readDi(port_bit); break;
        case OhtIoBoardId::OHT_REAR_DI:     ret = rear_io_->readDi(port_bit); break;
        default:
            throw std::runtime_error("[Read raw Di] Invalid Board Type");
    }

    //detectIoToggle(portid, ret);

    if (bit_inversion_ports_.find(portid) != bit_inversion_ports_.end()) {
        return !ret;
    } else {
        return ret;
    }

}

bool OHTController::readRawDo(unsigned portid)
{
    unsigned board = portid >> 24;
    unsigned port_bit = portid & 0x00FFFFFF;

    bool ret(false);
    switch(board){
        case OhtIoBoardId::OHT_ONBOARD_DO:  ret = onboard_io_.readDo(port_bit); break;
        case OhtIoBoardId::OHT_FRONT_DO:    ret = front_io_->readDo(port_bit); break;
        case OhtIoBoardId::OHT_REAR_DO:     ret = rear_io_->readDo(port_bit); break;
        default:
            throw std::runtime_error("[ReadDo] Invalid Board Type");
    }

    if (bit_inversion_ports_.find(portid) != bit_inversion_ports_.end()) {
        return !ret;
    } else {
        return ret;
    }

}

bool OHTController::writeRawDo(unsigned portid, bool value)
{
    unsigned board = portid >> 24;
    unsigned port_bit = portid & 0x00FFFFFF;

    if (portid == 0)    return 0;

    if (bit_inversion_ports_.find(portid) != bit_inversion_ports_.end()) {
        value = !value;
    }

    //detectIoToggle(portid, value);

    switch(board){
        case OhtIoBoardId::OHT_ONBOARD_DO:  return onboard_io_.writeDo(port_bit, value);
        case OhtIoBoardId::OHT_FRONT_DO:    return front_io_->writeDo(port_bit, value);
        case OhtIoBoardId::OHT_REAR_DO:     return rear_io_->writeDo(port_bit, value);
        default:
            throw std::runtime_error("Invalid Board Type");
    }
}


bool OHTController::readDi(unsigned portid)
{
    bool ret = readRawDi(portid);
    if (active_low_ports_.find(portid) != active_low_ports_.end()) {
        return !ret;
    } else {
        return ret;
    }
}

bool OHTController::readDo(unsigned portid)
{
    bool ret = readRawDo(portid);
    if (active_low_ports_.find(portid) != active_low_ports_.end()) {
        return !ret;
    } else {
        return ret;
    }
}

bool OHTController::writeDo(unsigned portid, bool value)
{
    //SEP_LOGT("[%s] write %d\n", getOhtIoPortString(portid), value);
    SEP_LOGT("[%s] write %d\n", ioConfig_.getportstring(portid).c_str(), value);
    if (active_low_ports_.find(portid) != active_low_ports_.end()) {
        value = !value;
    }
    return writeRawDo(portid, value);
}
