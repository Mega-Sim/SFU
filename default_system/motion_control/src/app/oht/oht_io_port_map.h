
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
// Created by root on 17. 12. 18.
//

#ifndef SEPMASTER_OHT_IO_PORT_MAP_H
#define SEPMASTER_OHT_IO_PORT_MAP_H

#include <utility>
namespace sephi {
    namespace oht {
        enum OhtIoBoardId {
            OHT_ONBOARD_DO, // 0
            OHT_ONBOARD_DI, // 1
            OHT_FRONT_DO,   // 2
            OHT_FRONT_DI,   // 3
            OHT_REAR_DO,    // 4
            OHT_REAR_DI     // 5
        };
        constexpr unsigned makePortId(unsigned board, unsigned port, unsigned bit){ return ( (board<<24) + (port<<16) + bit);}
        const char* getOhtIoPortString(unsigned portid);
        enum OhtIoPortId : unsigned {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// OhtOnboard DO //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            _obstacle_area1_sel            = makePortId(0,1,0),
            _obstacle_area2_sel            = makePortId(0,1,1),
            _obstacle_area3_sel            = makePortId(0,1,2),
            _obstacle_area4_sel            = makePortId(0,1,3),
            _obstacle_area5_sel            = makePortId(0,1,4),

            _ohtdetect_area1_sel           = makePortId(0,1,5),
            _ohtdetect_area2_sel           = makePortId(0,1,6),
            _ohtdetect_area3_sel           = makePortId(0,1,7),
            _ohtdetect_area4_sel           = makePortId(0,1,8),
            _ohtdetect_area5_sel           = makePortId(0,1,9),

            _oht_en                        = makePortId(0,1,10),
            _obs_en                        = makePortId(0,1,11),
            qrr1_in1                       = makePortId(0,1,12), //not used
            bcr1_in2                       = makePortId(0,1,13), //not used
            qrr2_in3                       = makePortId(0,1,14), //not used
            bcr2_in4                       = makePortId(0,1,15),  //not used

            driving_stop_in1               = makePortId(0,1,16),  //not used
            trans_stop_in2                 = makePortId(0,1,17), //not used

            servo_brake1                   = makePortId(0,4,9),  //not used
            servo_brake2                   = makePortId(0,4,10), //not used
            servo_brake3                   = makePortId(0,4,11), //not used
            servo_brake4                   = makePortId(0,4,12), //not used
            limit_in1                      = makePortId(0,4,17), //not used
            limit_in2                      = makePortId(0,4,19), //not used
            _obstacle_power_reset          = makePortId(0,1,22), //0: on, 1: off
            _ohtdetect_power_reset         = makePortId(0,1,23), //0: on, 1: off

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// OhtOnboard DI //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            _ohtdetect_short               = makePortId(1,1,18),
            _ohtdetect_long                = makePortId(1,1,19),
            _ohtdetect_middle              = makePortId(1,1,20),
            _ohtdetect_fail                = makePortId(1,1,21),
            _stop_driving_front            = makePortId(1,1,22),
            _stop_driving_precheck         = makePortId(1,1,23),
            bcr_driving_ok                 = makePortId(1,1,24),
            bcr_driving_ng                 = makePortId(1,1,25),
            _stop_trans_front              = makePortId(1,1,26),
            _stop_trans_rear               = makePortId(1,1,27),
            bcr_trans_ok                   = makePortId(1,1,28),
            bcr_trans_ng                   = makePortId(1,1,29),
            oht_status                     = makePortId(1,1,30),
            obs_status                     = makePortId(1,1,31),
            driving_stop_st1               = makePortId(1,2,0),
            trans_stop_st2                 = makePortId(1,2,1),
            qrr1_st1                       = makePortId(1,2,2),
            bcr1_st2                       = makePortId(1,2,3),
            qrr2_st3                       = makePortId(1,2,4),
            bcr2_st4                       = makePortId(1,2,5),
            _obstacle_long                 = makePortId(1,2,6),
            _obstacle_middle               = makePortId(1,2,7),
            _obstacle_short                = makePortId(1,2,8),
            _obstacle_fail                 = makePortId(1,2,9),

			home_sensor1 				   = makePortId(1,3,19),
			positive_limit1 			   = makePortId(1,3,20),
			negative_limit1 			   = makePortId(1,3,21),
			home_sensor2 				   = makePortId(1,3,24),
			positive_limit2 		       = makePortId(1,3,25),
			negative_limit2 			   = makePortId(1,3,27),
			home_sensor3 				   = makePortId(1,3,28),
			positive_limit3 			   = makePortId(1,3,29),
			negative_limit3 			   = makePortId(1,3,30),
			home_sensor4 				   = makePortId(1,3,31),
			positive_limit4 		       = makePortId(1,2,99),
			negative_limit4 		       = makePortId(1,2,98),
			limit_st1 				       = makePortId(1,4,18),
			limit_st2 				       = makePortId(1,4,20),
/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// OhtFront DO ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			cid_left_select                 = makePortId(2, 0, 0),
			cid_manual                      = makePortId(2, 0, 1),
			cid_occup_req                   = makePortId(2, 0, 2),
			cid_pass_complete               = makePortId(2, 0, 3),
			cid_reserved_1                  = makePortId(2, 0, 4),
			CID_RESERVED_OUT3_M             = makePortId(2, 0, 5),
			cid_reserved_2                  = makePortId(2, 0, 6),
			cid_right_select                = makePortId(2, 0, 7),

			hand_reset                      = makePortId(2, 1, 0),
			hand_close                      = makePortId(2, 1, 1),
			hand_free                       = makePortId(2, 1, 2),
			hand_open                       = makePortId(2, 1, 3),
			hand_origin                     = makePortId(2, 1, 4),
			hand_reserved_6                 = makePortId(2, 1, 5),
			hand_reserved_7                 = makePortId(2, 1, 6),
			hand_reserved_8                 = makePortId(2, 1, 7),

			hand_pio_select                 = makePortId(2, 2, 0),
			hand_trigger_out                = makePortId(2, 2, 1),
			//_obstacle_area1_sel             = makePortId(2, 2, 2),    // onboard
			//_obstacle_area2_sel             = makePortId(2, 2, 3),    // onboard
			//_obstacle_area3_sel             = makePortId(2, 2, 4),    // onboard
			//_obstacle_area4_sel             = makePortId(2, 2, 5),    // onboard
			//_obstacle_area5_sel             = makePortId(2, 2, 6),    // onboard
			safety_clear                    = makePortId(2, 2, 7),

            shutter_front_reset             = makePortId(2, 3, 0),
            shutter_front_close             = makePortId(2, 3, 1),
            shutter_front_open              = makePortId(2, 3, 2),
            shutter_front_origin            = makePortId(2, 3, 3),
            shutter_front_free              = makePortId(2, 3, 4),
            steer_front_torque0             = makePortId(2, 3, 5),
            steer_front_torque1             = makePortId(2, 3, 6),
            steer_front_dir                 = makePortId(2, 3, 7),

            _lookdown_area1_sel             = makePortId(2, 4, 0),
            _lookdown_area2_sel             = makePortId(2, 4, 1),
            _lookdown_area3_sel             = makePortId(2, 4, 2),
            _lookdown_area4_sel             = makePortId(2, 4, 3),
            EXTRA_OUTPUT1_M                 = makePortId(2, 4, 4),
            EXTRA_OUTPUT2_M                 = makePortId(2, 4, 5),
            EXTRA_OUTPUT3_M                 = makePortId(2, 4, 6),
            CID_Power_EN                    = makePortId(2, 4, 7),

            // HAND_PIO_Power_EN                 = makePortId(2, 5, 0),
            LD_Power_EN                     = makePortId(2, 5, 1),
            //_obs_en                         = makePortId(2, 5, 2),    // onboard
            V12_PWM                         = makePortId(2, 5, 3),
            driving_brake_release           = makePortId(2, 5, 4),
            HAND_POWER_RESET_M              = makePortId(2, 5, 5),
            hoist_ext_brake_release         = makePortId(2, 5, 6),
            safety_control                  = makePortId(2, 5, 7),

            Spare_Output2_M                      = makePortId(2, 6, 0),

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// OhtFront DI ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            cid_interlock                   = makePortId(3, 0, 0),
            cid_link_complete               = makePortId(3, 0, 1),
            cid_pass_ok                     = makePortId(3, 0, 2),
            cid_reserved1                   = makePortId(3, 0, 3),
            cid_reserved2                   = makePortId(3, 0, 4),
            cid_status                      = makePortId(3, 0, 5),
            hand_gripper_left               = makePortId(3, 0, 6),
            hand_gripper_right              = makePortId(3, 0, 7),

            hand_alarm                      = makePortId(3, 1, 0),
            Hand_Align_Sensor_M             = makePortId(3, 1, 1),
            hand_close_sen                  = makePortId(3, 1, 2),
            hand_es                         = makePortId(3, 1, 3),
            hand_inposition                 = makePortId(3, 1, 4),
            hand_open_sen                   = makePortId(3, 1, 5),
            hand_pio_go                     = makePortId(3, 1, 6),
            hand_push_sen                   = makePortId(3, 1, 7),

            //_obstacle_long                  = makePortId(3, 2, 0),    // onboard
            //_obstacle_middle                = makePortId(3, 2, 1),    // onboard
            //_obstacle_short                 = makePortId(3, 2, 2),    // onboard
            //_obstacle_fail                  = makePortId(3, 2, 3),    // onboard
            // _obstacle_input_extend         = makePortId(3, 2, 4),    // onboard
            bumper_press                    = makePortId(3, 2, 5),
            shutter_front_close_sen         = makePortId(3, 2, 6),
            shutter_front_middle_close_sen         = makePortId(3, 2, 7),

            shutter_front_open_sen          = makePortId(3, 3, 0),
            shutter_front_alarm             = makePortId(3, 3, 1),
            shutter_front_inposition        = makePortId(3, 3, 2),
            NFAULT_F                        = makePortId(3, 3, 3),
            SNSOUT_F                        = makePortId(3, 3, 4),
            steer_front_left                = makePortId(3, 3, 5),
            steer_front_right               = makePortId(3, 3, 6),
            _lookdown_error                 = makePortId(3, 3, 7),

            _lookdown_detect                = makePortId(3, 4, 0),
            EXTRA_INPUT1_M                  = makePortId(3, 4, 1),
            EXTRA_INPUT2_M                  = makePortId(3, 4, 2),
            HOIST_ALIGN_SENSOR_M            = makePortId(3, 4, 3),
            _hoist_belt_detect_1            = makePortId(3, 4, 4),
            _hoist_belt_detect_2            = makePortId(3, 4, 5),
            _oscillation_sensor             = makePortId(3, 4, 6),
            CID_Power_ST                    = makePortId(3, 4, 7),

            HAND_PIO_Power_ST                  = makePortId(3, 5, 0),
            LD_Power_ST                     = makePortId(3, 5, 1),
            //obs_status                      = makePortId(3, 5, 2),    // onboard
            emo_press                       = makePortId(3, 5, 3),
            Spare_Input2_M                       = makePortId(3, 5, 4),

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// OhtRear DO /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            _eq_pio_right_select            = makePortId(4, 0, 0),
            _eq_pio_left_select             = makePortId(4, 0, 1),
            eq_pio_busy                     = makePortId(4, 0, 2),
            eq_pio_complete                 = makePortId(4, 0, 3),
            eq_pio_cs0                      = makePortId(4, 0, 4),
            eq_pio_cs1                      = makePortId(4, 0, 5),
            eq_pio_cs2                      = makePortId(4, 0, 6),
            eq_pio_cs3                      = makePortId(4, 0, 7),

            eq_pio_tr_req                   = makePortId(4, 1, 0),
            eq_pio_valid                    = makePortId(4, 1, 1),
            lamp_green                      = makePortId(4, 1, 2),
            LAMP_LEFT_M                     = makePortId(4, 1, 3),  // yellow left
            LAMP_RIGHT_M                    = makePortId(4, 1, 4),  // yellow right
            lamp_red                        = makePortId(4, 1, 5),
            blackbox_do0                   = makePortId(4, 1, 6),
            blackbox_do1                   = makePortId(4, 1, 7),

            blackbox_do2                   = makePortId(4, 2, 0),
            //_ohtdetect_area1_sel            = makePortId(4, 2, 1),    // onboard
            //_ohtdetect_area2_sel            = makePortId(4, 2, 2),    // onboard
            //_ohtdetect_area3_sel            = makePortId(4, 2, 3),    // onboard
            //_ohtdetect_area4_sel            = makePortId(4, 2, 4),    // onboard
            //_ohtdetect_area5_sel            = makePortId(4, 2, 5),    // onboard
            Reserved_Output1_M              = makePortId(4, 2, 6),
            Reserved_Output2_M              = makePortId(4, 2, 7),

            Reserved_Output3_M              = makePortId(4, 3, 0),
            Reserved_Output4_M              = makePortId(4, 3, 1),
            shutter_rear_reset              = makePortId(4, 3, 2),
            shutter_rear_close              = makePortId(4, 3, 3),
            shutter_rear_open               = makePortId(4, 3, 4),
            shutter_rear_origin             = makePortId(4, 3, 5),
            shutter_rear_free               = makePortId(4, 3, 6),
            steer_rear_torque0              = makePortId(4, 3, 7),

            steer_rear_torque1              = makePortId(4, 4, 0),
            steer_rear_dir                  = makePortId(4, 4, 1),
            Driving_Stop_EN                = makePortId(4, 4, 2),    // onboard - driving_stop_in1
            Hoist_Steer_Power_EN           = makePortId(4, 4, 3),    // onboard
            IO_Power_EN                    = makePortId(4, 4, 4),    // onboard
            //_oht_en                         = makePortId(4, 4, 5),    // onboard
            //bcr1_in2                        = makePortId(4, 4, 6),    // onboard
            //bcr2_in4                        = makePortId(4, 4, 7),    // onboard

            //qrr1_in1                        = makePortId(4, 5, 0),    // onboard
            //qrr1_in3                        = makePortId(4, 5, 1),    // onboard
            Spare_Output1                   = makePortId(4, 5, 2),    // onboard


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// OhtRear DI /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            eq_pio_right_go                 = makePortId(5, 0, 0),
            eq_pio_left_go                  = makePortId(5, 0, 1),
            eq_pio_es                       = makePortId(5, 0, 2),
            eq_pio_ho_avbl                  = makePortId(5, 0, 3),
            pio_input3                      = makePortId(5, 0, 4),
            pio_input5                      = makePortId(5, 0, 5),
            _pio_input6                     = makePortId(5, 0, 6),
            eq_pio_load_req                 = makePortId(5, 0, 7),

            eq_pio_ready                    = makePortId(5, 1, 0),
            eq_pio_unload_req               = makePortId(5, 1, 1),
            blackbox_di0                    = makePortId(5, 1, 2),
            blackbox_di1                    = makePortId(5, 1, 3),
            blackbox_di2                    = makePortId(5, 1, 4),
            REMOCON_CONNECT_M               = makePortId(5, 1, 5),
            //_ohtdetect_long                 = makePortId(5, 1, 6),    // onboard
            //_ohtdetect_middle               = makePortId(5, 1, 7),    // onboard

            //_ohtdetect_short                = makePortId(5, 2, 0),    // onboard
            //_ohtdetect_fail                 = makePortId(5, 2, 1),    // onboard
            //_ohtdetect_input_extend         = makePortId(5, 2, 2),
            Reserved_Input1_M               = makePortId(5, 2, 3),
            Reserved_Input2_M               = makePortId(5, 2, 4),
            Reserved_Input3_M               = makePortId(5, 2, 5),
            Reserved_Input4_M               = makePortId(5, 2, 6),
            //_stop_driving_front             = makePortId(5, 2, 7),    // onboard

            //_stop_driving_precheck          = makePortId(5, 3, 0),    // onboard
            //bcr_driving_ok                  = makePortId(5, 3, 1),    // onboard
            //bcr_driving_ng                  = makePortId(5, 3, 2),    // onboard
            //_stop_trans_front               = makePortId(5, 3, 3),    // onboard
            //_stop_trans_rear                = makePortId(5, 3, 4),    // onboard
            //bcr_trans_ok                    = makePortId(5, 3, 5),    // onboard
            //bcr_trans_ng                    = makePortId(5, 3, 6),    // onboard
            _stb_detect_left                = makePortId(5, 3, 7),

            _stb_detect_right               = makePortId(5, 4, 0),
            shutter_rear_close_sen          = makePortId(5, 4, 1),
            shutter_rear_middle_close_sen   = makePortId(5, 4, 2),
            shutter_rear_open_sen           = makePortId(5, 4, 3),
            shutter_rear_alarm              = makePortId(5, 4, 4),
            shutter_rear_inposition         = makePortId(5, 4, 5),
            nFAULT_R                        = makePortId(5, 4, 6),
            SNSOUT_R                        = makePortId(5, 4, 7),

            steer_rear_left                 = makePortId(5, 5, 0),
            steer_rear_right                = makePortId(5, 5, 1),
            Driving_Stop_ST                 = makePortId(5, 5, 2),    // onboard - driving_stop_st1
            Hoist_Steer_Power_ST            = makePortId(5, 5, 3),
            IO_Power_ST                     = makePortId(5, 5, 4),
            //oht_status                      = makePortId(5, 5, 5),    // onboard
            //bcr1_st2                        = makePortId(5, 5, 6),    // onboard
            //bcr2_st4                        = makePortId(5, 5, 7),    // onboard

            //qrr1_st1                        = makePortId(5, 6, 0),    // onboard
            //qrr2_st3                        = makePortId(5, 6, 1),    // onboard
            _utb_detect                     = makePortId(5, 6, 2),
            slide_home_sensor               = makePortId(5, 6, 3),
            _foup_cover_open_detect         = makePortId(5, 6, 4),
            _foup_detect                    = makePortId(5, 6, 5),
            _fan_fail                       = makePortId(5, 6, 6),
            _smps_short_check               = makePortId(5, 6, 7),

            spare_input1                       = makePortId(5, 7, 0),
        */};
    }
}
#endif //SEPMASTER_OHT_IO_PORT_MAP_H
