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

#include "oht_io_port_map.h"
using namespace sephi;
using namespace sephi::oht;

const char* sephi::oht::getOhtIoPortString(unsigned portid)
{
    switch(portid){
        case _obstacle_area1_sel: return "_obstacle_area1_sel";
        case _obstacle_area2_sel: return "_obstacle_area2_sel";
        case _obstacle_area3_sel: return "_obstacle_area3_sel";
        case _obstacle_area4_sel: return "_obstacle_area4_sel";
        case _obstacle_area5_sel: return "_obstacle_area5_sel";
        case _ohtdetect_area1_sel: return "_ohtdetect_area1_sel";
        case _ohtdetect_area2_sel: return "_ohtdetect_area2_sel";
        case _ohtdetect_area3_sel: return "_ohtdetect_area3_sel";
        case _ohtdetect_area4_sel: return "_ohtdetect_area4_sel";
        case _ohtdetect_area5_sel: return "_ohtdetect_area5_sel";
        case _oht_en: return "_oht_en";
        case _obs_en: return "_obs_en";
        case qrr1_in1: return "qrr1_in1";
        case bcr1_in2: return "bcr1_in2";
        case qrr2_in3: return "qrr2_in3";
        case bcr2_in4: return "bcr2_in4";
        case driving_stop_in1: return "driving_stop_in1";
        case trans_stop_in2: return "trans_stop_in2";
        case servo_brake1: return "servo_brake1";
        case servo_brake2: return "servo_brake2";
        case servo_brake3: return "servo_brake3";
        case servo_brake4: return "servo_brake4";
        case limit_in1: return "limit_in1";
        case limit_in2: return "limit_in2";
        case _obstacle_power_reset: return "_obstacle_power_reset";
        case _ohtdetect_power_reset: return "_ohtdetect_power_reset";

        case _ohtdetect_short: return "_ohtdetect_short";
        case _ohtdetect_long: return "_ohtdetect_long";
        case _ohtdetect_middle: return "_ohtdetect_middle";
        case _ohtdetect_fail: return "_ohtdetect_fail";
        case _stop_driving_front: return "_stop_driving_front";
        case _stop_driving_precheck: return "_stop_driving_precheck";
        case bcr_driving_ok: return "bcr_driving_ok";
        case bcr_driving_ng: return "bcr_driving_ng";
        case _stop_trans_front: return "_stop_trans_front";
        case _stop_trans_rear: return "_stop_trans_rear";
        case bcr_trans_ok: return "bcr_trans_ok";
        case bcr_trans_ng: return "bcr_trans_ng";
        case obs_status: return "obs_status";
        case oht_status: return "oht_status";
        case qrr1_st1: return "qrr1_st1";
        case bcr1_st2: return "bcr1_st2";
        case qrr2_st3: return "qrr2_st3";
        case bcr2_st4: return "bcr2_st4";
        case driving_stop_st1: return "driving_stop_st1";
        case trans_stop_st2: return "trans_stop_st2";
        case _obstacle_long: return "_obstacle_long";
        case _obstacle_middle: return "_obstacle_middle";
        case _obstacle_short: return "_obstacle_short";
        case _obstacle_fail: return "_obstacle_fail";
/*
        case steer_front_left: return "steer_front_left";
        //case SWV: return "SWV";
        case cid_reserved2: return "cid_reserved2";
        case NFAULT_F: return "NFAULT_F";
        case cid_reserved1: return "cid_reserved1";
        case shutter_front_open_sen: return "shutter_front_open_sen";
        case cid_pass_ok: return "cid_pass_ok";
        case cid_link_complete: return "cid_link_complete";
        case cid_status: return "cid_status";
        case cid_interlock: return "cid_interlock";
        case CID_Power_ST: return "CID_Power_ST";
        case LD_Power_ST: return "LD_Power_ST";
        case steer_front_right: return "steer_front_right";
        case shutter_front_close_sen: return "shutter_front_close_sen";
        case _lookdown_error: return "_lookdown_error";
        case _lookdown_detect: return "_lookdown_detect";
        case Reserved_Input1_M: return "Reserved_Input1_M";
        case Reserved_Input2_M: return "Reserved_Input2_M";
        case Reserved_Input3_M: return "Reserved_Input3_M";
        case Reserved_Input4_M: return "Reserved_Input4_M";
        case slide_home_sensor: return "slide_home_sensor";
        case bumper_press: return "bumper_press";
        case _stb_detect_left: return "_stb_detect_left";
        case _stb_detect_right: return "_stb_detect_right";
        case _foup_cover_open_detect: return "_foup_cover_open_detect";
        case shutter_front_alarm: return "shutter_front_alarm";
        case shutter_front_inposition: return "shutter_front_inposition";
        case SNSOUT_F: return "SNSOUT_F";
        //case LAN_PWR_ST: return "LAN_PWR_ST";
        case hand_es: return "hand_es";

            // Front Output
        //case lamp_yellow: return "lamp_yellow";
        case _lookdown_area1_sel: return "_lookdown_area1_sel";
        case lamp_red: return "lamp_red";
        case LAMP_RIGHT_M: return "LAMP_RIGHT_M";
        case lamp_green : return "lamp_green";
        case cid_reserved_1: return "cid_reserved_1";
        case LAMP_LEFT_M: return "LAMP_LEFT_M";
        case safety_clear: return "safety_clear";
        case cid_left_select: return "cid_left_select";
        case cid_right_select: return "cid_right_select";
        case cid_occup_req: return "cid_occup_req";
        case cid_pass_complete: return "cid_pass_complete";
        case cid_manual: return "cid_manual";
        case CID_RESERVED_OUT3_M : return "CID_RESERVED_OUT3_M";
        case cid_reserved_2 : return "cid_reserved_2";
        case Reserved_Output1_M : return "Reserved_Output1_M";
        case LD_Power_EN : return "LD_Power_EN";
        case safety_control: return "safety_control";
        case Reserved_Output2_M: return "Reserved_Output2_M";
        case Reserved_Output3_M: return "Reserved_Output3_M";
        case Reserved_Output4_M: return "Reserved_Output4_M";
        case _lookdown_area4_sel: return "_lookdown_area4_sel";
        case _lookdown_area2_sel: return "_lookdown_area2_sel";
        case _lookdown_area3_sel: return "_lookdown_area3_sel";
        case steer_front_torque0: return "steer_front_torque0";
        case steer_front_torque1: return "steer_front_torque1";
        case steer_front_dir: return "steer_front_dir";
        case shutter_front_reset: return "shutter_front_reset";
        case shutter_front_close: return "shutter_front_close";
        case shutter_front_open: return "shutter_front_open";
        case shutter_front_origin: return "shutter_front_origin";
        case shutter_front_free : return "shutter_front_free";
        case CID_Power_EN: return "CID_Power_EN";
        case V12_PWM : return "V12_PWM";
        //case STEP_STEP : return "STEP_STEP";
        //case LAN_PWR_EN : return "LAN_PWR_EN";

            // Rear Input
        case hand_pio_go: "hand_pio_go";
        case hand_inposition: "hand_inposition";
        case hand_alarm: "hand_alarm";
        case hand_gripper_right: "hand_gripper_right";
        case hand_gripper_left: "hand_gripper_left";
        case hand_close_sen: "hand_close_sen";
        case hand_open_sen: "hand_open_sen";
        case hand_push_sen: "hand_push_sen";
        case Hand_Align_Sensor_M: "Hand_Align_Sensor_M";
        //case SWV_Rear: "SWV_Rear";
        case eq_pio_load_req: "eq_pio_load_req";
        case eq_pio_unload_req: "eq_pio_unload_req";
        case eq_pio_ready: "eq_pio_ready";
        case eq_pio_ho_avbl: "eq_pio_ho_avbl";
        case eq_pio_es: "eq_pio_es";
        case eq_pio_right_go: "eq_pio_right_go";
        case eq_pio_left_go: "eq_pio_left_go";
        //case _hoist_home_sensor: "_hoist_home_sensor";
        case _hoist_belt_detect_1: "_hoist_belt_detect_1";
        case _hoist_belt_detect_2: "_hoist_belt_detect_2";
        case steer_rear_left: "steer_rear_left";
        case steer_rear_right: "steer_rear_right";
        case shutter_rear_close_sen: "shutter_rear_close_sen";
        case shutter_rear_open_sen: "shutter_rear_open_sen";
        case REMOCON_CONNECT_M: "REMOCON_CONNECT_M";
        case _fan_fail: "_fan_fail";
        case EXTRA_INPUT1_M: "EXTRA_INPUT1_M";
        case EXTRA_INPUT2_M: "EXTRA_INPUT2_M";
        case HOIST_ALIGN_SENSOR_M: "HOIST_ALIGN_SENSOR_M";
        case nFAULT_R: "nFAULT_R";
        case SNSOUT_R: "SNSOUT_R";
        case _smps_short_check: "_smps_short_check";
        case shutter_rear_alarm: "shutter_rear_alarm";
        case shutter_rear_inposition: "shutter_rear_inposition";
        //case OSCILLATION_SENSOR_M: "OSCILLATION_SENSOR_M";
        case _oscillation_sensor: "_oscillation_sensor"; // 20190507 insert lsj
        case _foup_detect: "_foup_detect";
        //case E_STOP_M: "E_STOP_M";
        case emo_press: "emo_press";
        case pio_input3: "pio_input3";
        case pio_input5: "pio_input5";
        case _pio_input6: "_pio_input6";

            // Rear output
        case hand_pio_select: return "hand_pio_select";
        case hand_close: return "hand_close";
        case hand_open: return "hand_open";
        case hand_origin: return "hand_origin";
        case hand_reset: return "hand_reset";
        case hand_free: return "hand_free";
        case hand_reserved_6: return "hand_reserved_6";
        case hand_reserved_7: return "hand_reserved_7";
        case hand_reserved_8: return "hand_reserved_8";
        case hand_trigger_out: return "hand_trigger_out";
        case eq_pio_valid: return "eq_pio_valid";
        case eq_pio_cs0: return "eq_pio_cs0";
        case eq_pio_cs1: return "eq_pio_cs1";
        case eq_pio_cs2: return "eq_pio_cs2";
        case eq_pio_cs3: return "eq_pio_cs3";
        case eq_pio_tr_req: return "eq_pio_tr_req";
        case eq_pio_busy: return "eq_pio_busy";
        case eq_pio_complete: return "eq_pio_complete";
        case _eq_pio_left_select: return "_eq_pio_left_select";
        case _eq_pio_right_select: return "_eq_pio_right_select";
        case hoist_ext_brake_release: return "hoist_ext_brake_release";
        case driving_brake_release: return "driving_brake_release";
        case EXTRA_OUTPUT1_M: return "EXTRA_OUTPUT1_M";
        case EXTRA_OUTPUT2_M: return "EXTRA_OUTPUT2_M";
        case EXTRA_OUTPUT3_M: return "EXTRA_OUTPUT3_M";
        case steer_rear_torque0: return "steer_rear_torque0";
        case steer_rear_torque1: return "steer_rear_torque1";
        case steer_rear_dir: return "steer_rear_dir";
        case shutter_rear_reset: return "shutter_rear_reset";
        case shutter_rear_close: return "shutter_rear_close";
        case shutter_rear_open:  return "shutter_rear_open";
        case shutter_rear_origin: return "shutter_rear_origin";
        case shutter_rear_free: return "shutter_rear_free";
        case HAND_POWER_RESET_M: return "HAND_POWER_RESET_M";

        default: return "UNKNOWN...";*/
    }
}
