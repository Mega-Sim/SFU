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
// Created by root on 17. 11. 29.
//

#ifndef SEPMASTER_OHT_IO_MAP_H
#define SEPMASTER_OHT_IO_MAP_H
namespace sephi {
    namespace oht_sim {
        // digital input bit field; input is machine's view, that is, controller's output
        struct OhtOnboardDI {
            unsigned _obstacle_area1_sel:1;
            unsigned _obstacle_area2_sel:1;
            unsigned _obstacle_area3_sel:1;
            unsigned _obstacle_area4_sel:1;
            unsigned _ohtdetect_area1_sel:1;
            unsigned _ohtdetect_area2_sel:1;
            unsigned _ohtdetect_area3_sel:1;
            unsigned _ohtdetect_area4_sel:1;
            unsigned servo_brake1:1;
            unsigned servo_brake2:1;
            unsigned servo_brake3:1;
            unsigned servo_brake4:1;
            unsigned limit_in1:1;
            unsigned limit_in2:1;
            unsigned _obstacle_power_reset:1;
            unsigned _ohtdetect_power_reset:1;
            unsigned qrr1_in1:1;
            unsigned bcr1_in2:1;
            unsigned qrr2_in3:1;
            unsigned bcr2_in4:1;
            unsigned driving_stop_in1:1;
            unsigned trans_stop_in2:1;
        }__attribute__((packed));


        struct OhtOnboardDO {
            unsigned _obstacle_long:1;
            unsigned _obstacle_middle:1;
            unsigned _obstacle_short:1;
            unsigned _obstacle_fail:1;
            unsigned _ohtdetect_long:1;
            unsigned _ohtdetect_middle:1;
            unsigned _ohtdetect_short:1;
            unsigned _ohtdetect_fail:1;
            unsigned _stop_driving_front:1;
            unsigned _stop_driving_precheck:1;
            unsigned bcr_driving_ok:1;
            unsigned bcr_driving_ng:1;
            unsigned _stop_trans_front:1;
            unsigned _stop_trans_rear:1;
            unsigned bcr_trans_ok:1;
            unsigned bcr_trans_ng:1;
            unsigned home_sensor1:1;
            unsigned positive_limit1:1;
            unsigned negative_limit1:1;
            unsigned home_sensor2:1;
            unsigned positive_limit2:1;
            unsigned negative_limit2:1;
            unsigned home_sensor3:1;
            unsigned positive_limit3:1;
            unsigned negative_limit3:1;
            unsigned home_sensor4:1;
            unsigned positive_limit4:1;
            unsigned negative_limit4:1;
            unsigned limit_st1:1;
            unsigned limit_st2:1;
            unsigned obs_status:1;
            unsigned oht_status:1;
            unsigned qrr1_st1:1;
            unsigned bcr1_st2:1;
            unsigned qrr2_st3:1;
            unsigned bcr2_st4:1;
            unsigned driving_stop_st1:1;
            unsigned trans_stop_st2:1;
        }__attribute__((packed));

        
        // digital input bit field; input is machine's view, that is, controller's output
        struct OhtFrontDI {
            unsigned lamp_yellow:1;              // (0,0)
            unsigned :2;                         // (0,1~2), (reserved_output7~8)
            unsigned _lookdown_area1_sel:1;      // (0,3)
            unsigned lamp_red:1;                 // (0,4)
            unsigned :1;                         // (0,5), (lamp_right)
            unsigned lamp_green:1;               // (0,6)
            unsigned cid_reserved_1:1;           // (0,7)
            unsigned :1;                         // (1,0), (lamp_left)
            unsigned safety_clear:1;             // (1,1)
            unsigned :1;                         // (1,2), (mc_control)
            unsigned _hand_power_reset:1;        // (1,3)
            unsigned cid_left_select:1;          // (1,4)
            unsigned cid_right_select:1;         // (1,5)
            unsigned cid_occup_req:1;            // (1,6)
            unsigned cid_pass_complete:1;        // (1,7)
            unsigned cid_manual:1;               // (2,0)
            unsigned CID_RESERVED_OUT3_M:1;      // (2,1), (cid_reserved_3)
            unsigned cid_reserved_2:1;           // (2,2)
            unsigned :1;                         // (2,3), (Reserved_Output1)
            unsigned _lookdown_power_reset:1;    // (2,4)
            unsigned safety_control:1;           // (2,5)
            unsigned :5;                         // (2,6~7), (3,0~2), (reserved_output2~6)
            unsigned _lookdown_area4_sel:1;      // (3,3)
            unsigned _lookdown_area2_sel:1;      // (3,4)
            unsigned _lookdown_area3_sel:1;      // (3,5)
            unsigned steer_front_torque0:1;      // (3,6)
            unsigned steer_front_torque1:1;      // (3,7)
            unsigned steer_front_dir:1;          // (4,0) 0: left, 1: right
            unsigned shutter_front_reset:1;      // (4,1)
            unsigned shutter_front_close:1;      // (4,2)
            unsigned shutter_front_open:1;       // (4,3)
            unsigned shutter_front_origin:1;     // (4,4)
            unsigned shutter_front_free:1;       // (4,5)
            unsigned utb_detect_sensor:1;        // (4,6)
        }__attribute__((packed));

        // digital output bit field; output is machine's view, that is, controller's input
        struct OhtFrontDO {
            unsigned :1;                         // (0,0), (f_mout)
            unsigned steer_front_left:1;         // (0,1)
            unsigned :1;                         // (0,2), (swv)
            unsigned cid_reserved2:1;            // (0,3)
            unsigned :1;                         // (0,4)
            unsigned cid_reserved1:1;            // (0,5)
            unsigned outrigger_home_sen:1;       // (0,6)
            unsigned emo_press:1;               // (0,7) ; active low
            unsigned outrigger_work_sen:1;       // (1,0)
            unsigned :1;                         // (1,1), (mc_monitor)
            unsigned shutter_front_open_sen:1;   // (1,2)
            unsigned cid_pass_ok:1;              // (1,3)
            unsigned cid_link_complete:1;        // (1,4)
            unsigned cid_status:1;               // (1,5)
            unsigned cid_interlock:1;            // (1,6)
            unsigned :1;                         // (1,7), (cid_power_st)
            unsigned :1;                         // (2,0), (ld_power_st)
            unsigned steer_front_right:1;        // (2,1)
            unsigned shutter_front_close_sen:1;  // (2,2)
            unsigned _lookdown_error:1;          // (2,3) ; active low
            unsigned _lookdown_detect:1;         // (2,4) ; active low
            unsigned :8;                         // (2,5~7), (3,0~4), (reserved_input1~8)
            unsigned slide_home_sensor:1;        // (3,5)
            unsigned _utb_detect:1;              // (3,6) ; active low
            unsigned _foup_detect:1;             // (3,7) ; active low
            unsigned bumper_press:1;             // (4,0)
            unsigned _stb_detect_left:1;         // (4,1) ; active low
            unsigned _stb_detect_right:1;        // (4,2) ; active low
            unsigned _oscillation_sensor:1;      // (4,3)
            unsigned shutter_front_alarm:1;      // (4,4)
            unsigned shutter_front_inposition:1; // (4,5)
        }__attribute__((packed));

        // digital input bit field; input is machine's view, that is, controller's output
        struct OhtRearDI {
            unsigned hand_pio_select:1;          // (0,0) always on
            unsigned hand_close:1;               // (0,1)
            unsigned hand_open:1;                // (0,2)
            unsigned hand_origin:1;              // (0,3) move to close position
            unsigned hand_reset:1;               // (0,4) clear alarm
            unsigned hand_free:1;                // (0,5) release. torque  = 0
            unsigned hand_reserved_6:1;          // (0,6)
            unsigned hand_reserved_7:1;          // (0,7)
            unsigned hand_reserved_8:1;          // (1,0)
            unsigned eq_pio_valid:1;             // (1,1)
            unsigned eq_pio_cs0:1;               // (1,2)
            unsigned eq_pio_cs1:1;               // (1,3)
            unsigned eq_pio_cs2:1;               // (1,4)
            unsigned eq_pio_cs3:1;               // (1,5)
            unsigned eq_pio_tr_req:1;            // (1,6)
            unsigned eq_pio_busy:1;              // (1,7)
            unsigned eq_pio_complete:1;          // (2,0)
            unsigned _eq_pio_left_select:1;      // (2,1), (pio1_select)
            unsigned _eq_pio_right_select:1;     // (2,2), (pio2_select)
            unsigned :1;                         // (2,3), (hoist_brake_sig)
            unsigned hoist_ext_brake_release:1;  // (2,4)
            unsigned driving_brake_release:1;    // (2,5)
            unsigned :3;                         // (2,6~7), (3,0), (extra_output1~3)
            unsigned steer_rear_torque0:1;       // (3,1)
            unsigned steer_rear_torque1:1;       // (3,2)
            unsigned steer_rear_dir:1;           // (3,3)
            unsigned shutter_rear_reset:1;       // (3,4)
            unsigned shutter_rear_close:1;       // (3,5)
            unsigned shutter_rear_open:1;        // (3,6)
            unsigned shutter_rear_origin:1;      // (3,7)
            unsigned shutter_rear_free:1;        // (4,0)
        }__attribute__((packed));

        // digital output bit field; output is machine's view, that is, controller's input
        struct OhtRearDO {
            unsigned hand_pio_go:1;              // (0,0) always ON if hand_pio_select && hand and oht is communicationable
            unsigned hand_inposition:1;          // (0,1) when finished open or close motion
            unsigned hand_alarm:1;               // (0,2)
            unsigned hand_gripper_right:1;       // (0,3) 1 when gripped
            unsigned hand_gripper_left:1;        // (0,4)
            unsigned hand_close_sen:1;           // (0,5)
            unsigned hand_open_sen:1;            // (0,6)
            unsigned hand_push_sen:1;            // (0,7)
            unsigned :1;                         // (1,0), (hand_align)
            unsigned :1;                         // (1,1), (swv)
            unsigned eq_pio_load_req:1;          // (1,2)
            unsigned eq_pio_unload_req:1;        // (1,3)
            unsigned eq_pio_ready:1;             // (1,4)
            unsigned eq_pio_ho_avbl:1;           // (1,5)
            unsigned eq_pio_es:1;                // (1,6)
            unsigned eq_pio_right_go:1;          // (1,7)
            unsigned eq_pio_left_go:1;           // (2,0)
            unsigned _hoist_home_sensor:1;       // (2,1) ; active low
            unsigned _hoist_belt_detect_1:1;     // (2,2) ; active low
            unsigned _hoist_belt_detect_2:1;     // (2,3) ; active low
            unsigned steer_rear_left:1;          // (2,4)
            unsigned steer_rear_right:1;         // (2,5)
            unsigned shutter_rear_close_sen:1;   // (2,6) anit-drop
            unsigned shutter_rear_open_sen:1;    // (2,7)
            unsigned handtp_connect:1;           // (3,0)
            unsigned _foup_cover_open_detect:1;  // (3,1) ; active low
            unsigned _fan_fail:1;                // (3,2) ; active low
            unsigned tp_enable:1;                // (3,3)
            unsigned :4;                         // (3,4~7), (extra_input1~2, nfault, snsout)
            unsigned _smps_short_check:1;        // (4,0) ; active low
            unsigned shutter_rear_alarm:1;       // (4,1)
            unsigned shutter_rear_inposition:1;  // (4,2)
            unsigned pio_input3:1;               // (4,3)
            unsigned pio_input5:1;               // (4,4)
            unsigned _pio_input6:1;              // (4,5) ; active low
        }__attribute__((packed));
    }
}
#endif //SEPMASTER_OHT_IO_MAP_H
