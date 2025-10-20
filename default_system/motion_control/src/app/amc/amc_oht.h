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
// Created by sem1 on 17. 8. 2.
//

#ifndef SEPMASTER_AMC_OHT_H
#define SEPMASTER_AMC_OHT_H

#include <string>
#include "amc_emulator.h"
#include "io_board.h"
#include "oht_config.h"
#include "oht_controller.h"
#include "oht_behavior_base.h"
#include "ecmaster_rpc_client.h"
// susi
#include "Susi4.h"
#include "susi_atlas.h"
#include "max11609.h"
#include "amc_sound.h"
#include "patchlib.h"
#include <bitset>
#define GETPORTID(x) controller.getportid(x)    // 테스트 필요

#define TRACE_CNT_ITER							1
#define Trace_Data_Buffer_Size					750001
#define MTrace_Data_Buffer_Size					80001
#define Trace_Data_Count_M						25
#define Trace_Data_Transfer_Buffer_Size_C		61
#define Trace_Data_Transfer_Buffer_Size_M		51

namespace amc
{
    using namespace sephi;
    using namespace sephi::oht;
    using std::unique_ptr;
    using std::make_unique;
    using std::unique_ptr;
    using std::make_unique;
    using std::array;
    using std::pair;
    using std::make_pair;
    using namespace std::chrono_literals;
    using std::this_thread::sleep_for;

    namespace {
        const int64_t OHT_ISR_CYCLETIME_NS = 1000000;
        const int64_t OHT_DEC_TO_ACC_DELAY_NS = 1000000000;

        const int MONITORING_SENSOR_NUM  = 4;
        const int AMC_AXIS               = 4;
        const double SECTOMSEC           = 1000.0;

        const int VELOCITY_0_0_MPS = 0;
        const int VELOCITY_0_1_MPS = 11343;
        const int VELOCITY_0_15_MPS = 17014;
        const int VELOCITY_0_2_MPS = 22686;
        const int VELOCITY_0_3_MPS = 34029;
        const int VELOCITY_0_4_MPS = 45372;
        const int VELOCITY_0_5_MPS = 56716;
        const int VELOCITY_0_6_MPS = 68059;
        const int VELOCITY_0_7_MPS = 79402;
        const int VELOCITY_0_8_MPS = 90745;
        const int VELOCITY_0_9_MPS = 102088;
        const int VELOCITY_1_0_MPS = 113431;
        const int VELOCITY_1_1_MPS = 124774;
        const int VELOCITY_1_2_MPS = 136117;
        const int VELOCITY_1_3_MPS = 147461;
        const int VELOCITY_1_4_MPS = 158804;
        const int VELOCITY_1_5_MPS = 170147;
        const int VELOCITY_1_6_MPS = 181490;
        const int VELOCITY_1_7_MPS = 192833;
        const int VELOCITY_1_8_MPS = 204176;
        const int VELOCITY_1_9_MPS = 215519;
        const int VELOCITY_2_0_MPS = 226862;
        const int VELOCITY_2_1_MPS = 238205;
        const int VELOCITY_2_2_MPS = 249549;
        const int VELOCITY_2_3_MPS = 260892;
        const int VELOCITY_2_4_MPS = 272235;
        const int VELOCITY_2_5_MPS = 283578;
        const int VELOCITY_2_6_MPS = 294921;
        const int VELOCITY_2_7_MPS = 306264;
        const int VELOCITY_2_8_MPS = 317607;
        const int VELOCITY_2_9_MPS = 328950;
        const int VELOCITY_3_0_MPS = 340294;
        const int VELOCITY_3_1_MPS = 351637;
        const int VELOCITY_3_2_MPS = 362980;
        const int VELOCITY_3_3_MPS = 374323;
        const int VELOCITY_3_4_MPS = 385666;
        const int VELOCITY_3_5_MPS = 397009;
        const int VELOCITY_3_6_MPS = 408352;
        const int VELOCITY_3_7_MPS = 419695;
        const int VELOCITY_3_8_MPS = 431038;
        const int VELOCITY_3_9_MPS = 442382;
        const int VELOCITY_4_0_MPS = 453725;
        const int VELOCITY_4_1_MPS = 465068;
        const int VELOCITY_4_2_MPS = 476411;
        const int VELOCITY_4_3_MPS = 487754;
        const int VELOCITY_4_4_MPS = 499097;
        const int VELOCITY_4_5_MPS = 510440;
        const int VELOCITY_4_6_MPS = 521783;
        const int VELOCITY_4_7_MPS = 533127;
        const int VELOCITY_4_8_MPS = 544470;
        const int VELOCITY_4_9_MPS = 555813;
        const int VELOCITY_5_0_MPS = 567156;
        const int VELOCITY_5_1_MPS = 578499;
        const int VELOCITY_5_2_MPS = 589842;
        const int VELOCITY_7_0_MPS = 794017;
        const int VELOCITY_8_0_MPS = 907448;

        const double SPEED_4_0_MPS2      = 453724.73;
        const double SPEED_3_5_MPS2      = 397009.14;
        const double SPEED_3_3_MPS2      = 374322.90;
        const double SPEED_3_0_MPS2      = 340293.55;
        const double SPEED_2_5_MPS2      = 283577.96;
        const double SPEED_2_0_MPS2      = 226862.37;
        const double SPEED_1_2_MPS2      = 136117.42;
        const double SPEED_1_5_MPS2      = 170146.77;
        const double SPEED_1_0_MPS2      = 113431.18;
        const double SPEED_0_7_MPS2      = 79401.82;
        const double SPEED_0_5_MPS2      = 56715.59;

        // Sick
        const int SICK_OBS_SPEED_GRADE_3_BIT      = 0x8; // b 1000
        const int SICK_OBS_SPEED_GRADE_5_BIT      = 0x9; // b 1001
        const int SICK_OBS_SPEED_GRADE_7_BIT      = 0xB; // b 1011
        const int SICK_OBS_SPEED_GRADE_8_BIT      = 0xF; // b 1111
        const int SICK_OBS_SPEED_GRADE_STEP_OUT   = 0x7; // b 0111

        // HYOKUYO
        const int HOKUYO_OBS_SPEED_GRADE_3_BIT    = 0x8; // b 1000
        const int HOKUYO_OBS_SPEED_GRADE_5_BIT    = 0xC; // b 1100
        const int HOKUYO_OBS_SPEED_GRADE_7_BIT    = 0xE; // b 1110
        const int HOKUYO_OBS_SPEED_GRADE_8_BIT    = 0xF; // b 1111
        const int HOKUYO_OBS_SPEED_GRADE_STEP_OUT = 0xA; // b 1010


        const int DETECT_INFO_TYPE_ONLY_UBG       = 0;
        const int DETECT_INFO_TYPE_ONLY_PBS       = 1;
        const int DETECT_INFO_TYPE_BOTH           = 2;

        const int PAUSED_NONE                     = 0;
        const int PAUSED_BY_TP                    = 1;
        const int PAUSED_BY_OCS                   = 2;

        const int STATUS_NORMAL                   = 0;
        const int STATUS_OBS_STOP                 = 1;
        const int STATUS_PBS_STOP                 = 2;
        const int STATUS_OHT_PAUSED               = 3;
        const int STATUS_PBS_DETECT				  = 4;

        const int OHT_DETECT_N_CHANGE_OFFST       = 900;

        const int ACC_MODE                        = 1;
        const int CRUISE_MODE                     = 2;
        const int DCC_MODE                        = -1;
        const int STOP_MODE                       = 0;
        const int NONE_MODE                       = -2;

        // Optional Node
        //const int CHANGENODE_DIST_LIMIT           = 11473.41619; //100 * Model_PULSETOMM;
        const int OPT_TAG                         = 0x05;
        const int OPT_DISTANCE                    = 0x06;
        const int OPT_COMBO                       = 0x07;

        const int forcedNodeCountDistLimit        = 200;

        // OHT Model ID
        const int OHT_STD_01                      = 30;
        const int OHT_NAME_V81                    = 31;
        const int OHT_NAME_V85S                   = 35; // 향후 수정 필요
        const int OHT_NAME_V82                    = 36;
        const int OHT_NAME_V86                    = 37;
        const int OHT_NAME_V82MASK                = 38;
        const int OHT_NAME_V90                    = 39;
        const int OHT_NAME_MASK                   = 6;
        const int OHT_NAME_P4MASK                 = 9;
        const int NEO_TRAY                        = 13;
        const int NEO_MAGAZINE                    = 14;
        const int NEO_SSD                         = 15;
        const int NEO_CASSETTE                    = 16;
        const int FOSB_4WD                        = 17;
        const int FOSB_2WD                        = 18;

        // Power
        const double POWER3P3_CHECK_Percent       = 5;
        const double POWER5P_CHECK_Percent        = 5;
        const double POWER12P_CHECK_Percent       = 10;

        const int POWER_CHECK_COUNT                = 30;

        const double POWER3P3_REF                 = 330;
        const double POWER5P_REF                  = 500;
        const double POWER12P_REF                 = 1200;
    }

    enum Event_State
    {
        NO_EVENT,
        STOP_EVENT,
        E_STOP_EVENT,
        ABORT_EVENT,
        ENC_OVF_EVENT,
        PAUSE_EVENT,
    };
    enum SENSOR_ID
    {
        HOKUYO,
        SICK,
    };


    enum Event_Source
    {
        ST_NONE             = 0x0000,   //normal operation
        ST_HOME_SWITCH      = 0x0001,   //home sensor detection
        ST_POS_LIMIT        = 0x0002,   //positive sensor detection
        ST_NEG_LIMIT        = 0x0004,   //negative sensor detection
        ST_AMP_FAULT        = 0x0008,   //Amp fault detection
        //ST_A_LIMIT          = 0x0010,   //acc/dcc limit over
        ST_SYSTEM_INSEC     = 0x0010,   //dc voltage limit over
        ST_V_LIMIT          = 0x0020,   //vel limit over
        ST_X_NEG_LIMIT      = 0x0040,   //negative limit over
        ST_X_POS_LIMIT      = 0x0080,   //positive limit over
        ST_ERROR_LIMIT      = 0x0100,   //position error limit over
        ST_PC_COMMAND       = 0x0200,   //event set detection - stop / e-stop..
        ST_OUT_OF_FRAMES    = 0x0400,   //frame buffer full
        ST_AMP_POWER_ONOFF  = 0x0800,   //Amp disable
        ST_ENC_OVF          = 0x1000,
        ST_HWINPOSITION     = 0x2000,
        ST_SWINPOSITION     = 0x4000,
        ST_MUST_REBOOT      = 0x8000,
    };

    // AMC_CMD_BIT_ANALYSIS
    typedef enum _MOTION_CONTROLLER_CMD_TYPE
    {
        CMD_TYPE_SET_OUTBIT                     = 0,
        CMD_TYPE_CLEAR_NODE_COUNT               = 1,
        CMD_TYPE_MOVE_AXIS_0                    = 2,
        CMD_TYPE_MOVE_AXIS_1                    = 3,
        CMD_TYPE_MOVE_AXIS_2                    = 4,
        CMD_TYPE_MOVE_AXIS_3                    = 5,
        CMD_TYPE_SET_DETECT_INFO                = 6,
        CMD_TYPE_SET_GEAR_RATIO_OF_DRIVING      = 7,
        CMD_TYPE_SET_DECEL_LIMIT_OF_DRIVING     = 8,
        CMD_TYPE_COMPENSATE_REAR_ENCODER        = 9,
        CMD_TYPE_PAUSE_CONTROL                  = 10,
        CMD_TYPE_SET_SLOPE_TIME                 = 11,
        CMD_TYPE_SET_SLOPE_TIME_RELEASE         = 12,
        CMD_TYPE_SET_FINAL_ARRIVAL              = 13,
        CMD_TYPE_SET_FINAL_ARRIVAL_RELEASE      = 14,
        CMD_TYPE_SET_NODE_COUNT                 = 15,
        CMD_TYPE_SET_FOLLOWS_MOVEDS_RELEASE     = 16,
        CMD_TYPE_SET_FOLLOWS_MOVEDS             = 17,
        CMD_TYPE_SET_SOFT_STOP_MOTION           = 18,
        CMD_TYPE_SET_SOFT_STOP_MOTION_RELEASE   = 19,
        CMT_TYPE_SET_UBG_SETTING_FLAG           = 20,
        CMD_TYPE_SET_LOW_VIBRATION              = 21,
        CMD_TYPE_SET_LOW_VIBRATION_RELEASE      = 22,
        CMD_TYPE_CLEAR_FAULT                    = 23,
        CMD_TYPE_GET_TORQUE                     = 24,
        CMD_TYPE_SET_TORQUE                     = 25,
    }MOTION_CONTROLLER_CMD_TYPE;

    enum AMC_CMD_AXIS
    {
        AMC_CMD_AXIS_NONE               = 0,   // 명령 없음
        AMC_CMD_AXIS_RESET              = 1,   // 축 초기화
        AMC_CMD_AXIS_AMP_ENABLE         = 2,   // Servo On
        AMC_CMD_AXIS_AMP_DISABLE        = 3,   // Servo Off
        AMC_CMD_AXIS_MOVE_P             = 4,
        AMC_CMD_AXIS_MOVE_N             = 5,
        AMC_CMD_AXIS_MOVE_S             = 6,
        AMC_CMD_AXIS_MOVE_DS            = 7,
        AMC_CMD_AXIS_MOVE_TIME_P        = 8,
        AMC_CMD_AXIS_MOVE_TIME_N        = 9,
        AMC_CMD_AXIS_MOVE_TIME_S        = 10,
        AMC_CMD_AXIS_MOVE_TIME_DS       = 11,
        AMC_CMD_AXIS_SET_HOME           = 12,  // 현재 위치를 0으로 만듬
        AMC_CMD_AXIS_SET_OFFSET         = 13,  // Offset 값 적용 (절대치 엔코더: 입력되는 위치를 0으로 만듬) --> Distance 에 값 입력
        AMC_CMD_AXIS_SET_POS_LIMIT      = 14,  // Positive SW Limit --> Distance 에 값 입력
        AMC_CMD_AXIS_SET_NEG_LIMIT      = 15,  // Negative SW Limit --> Distance 에 값 입력
        AMC_CMD_AXIS_RESET_LIMIT        = 16,  // Reset Limit --> Limit 사용하지 않음(또는 제일 큰 값으로 설정) --> 입력 인자 없음
		AMC_CMD_AXIS_MOVE_ESTOP		= 17
    };

    // CMD_ADDITIONAL_INFO의 CmdType define
    enum OHT_DETECT_CMD_PATH
    {
        OHT_DETECT_CMD_PATH_DIRECTION_NO_USE 		= 0,	// 미사용: 예전 없음
        OHT_DETECT_CMD_PATH_DIRECTION_NORMAL		= 1,	// 직진 구간: 예전 N
        OHT_DETECT_CMD_PATH_DIRECTION_LEFT			= 2,	// 왼쪽: 예전 L
        OHT_DETECT_CMD_PATH_DIRECTION_RIGHT 		= 3,	// 오른쪽: 예전 R
        OHT_DETECT_CMD_PATH_DIRECTION_SHORT	    	= 4,	// N분기 사용: 예전 S
        OHT_DETECT_CMD_PATH_DIRECTION_BOTH			= 5,	// 짧은 S자형 곡선: 예전 B
        OHT_DETECT_CMD_PATH_DIRECTION_NARROW		= 6,	// 레일 간격이 900mm 이하일때의 짧은 S자형 곡선: 예전 V
        OHT_DETECT_CMD_PATH_DIRECTION_NORMAL_HILL	= 7,
        OHT_DETECT_CMD_PATH_DIRECTION_RESERVED2		= 8,
        OHT_DETECT_CMD_PATH_DIRECTION_RESERVED3		= 9,
        OHT_DETECT_CMD_PATH_DIRECTION_RESERVED4		= 10,
        OHT_DETECT_CMD_PATH_DIRECTION_N_LEFT    	= 11,
        OHT_DETECT_CMD_PATH_DIRECTION_N_RIGHT   	= 12,
        OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_LEFT   = 13,
        OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_RIGHT  = 14,
        OHT_DETECT_CMD_PATH_DIRECTION_RESERVED5		= 15
    };

    enum OHT_DETECT {
        OHT_DETECT_NO_USE       = 0xF,
        OHT_DETECT_NORMAL_VALUE = 0xE,
        OHT_DETECT_LEFT_VALUE   = 0xD,
        OHT_DETECT_RIGHT_VALUE  = 0xC,
        OHT_DETECT_SHORT_VALUE  = 0xB,
        OHT_DETECT_BOTH_VALUE   = 0xA,
        OHT_DETECT_NARROW_VALUE = 0x9,
        OHT_DETECT_HILL_VALUE   = 0x8,
        OHT_DETECT_RESERVED2    = 0x7,
        OHT_DETECT_RESERVED3    = 0x6,
        OHT_DETECT_RESERVED4    = 0x5,
        OHT_DETECT_NL_VALUE     = 0x4,
        OHT_DETECT_NR_VALUE     = 0x3,
        OHT_DETECT_DOUBLE_LEFT  = 0x2,
        OHT_DETECT_DOUBLE_RIGHT = 0x1,
        OHT_DETECT_RESERVED5    = 0,
    };

    enum OHT_DETECT_CMD {
        OBS_DETECT_CMD_PATH_DIRECTION_NO_USE = 0,
        OBS_DETECT_CMD_PATH_DIRECTION_NORMAL = 1,
        OBS_DETECT_CMD_PATH_DIRECTION_LEFT = 2,
        OBS_DETECT_CMD_PATH_DIRECTION_RIGHT = 3,
        OBS_DETECT_CMD_PATH_DIRECTION_SHORT = 4,
        OBS_DETECT_CMD_PATH_DIRECTION_BOTH = 5,
        OBS_DETECT_CMD_PATH_DIRECTION_NARROW = 6,
        OBS_DETECT_CMD_PATH_DIRECTION_RESERVED1 = 7,
        OBS_DETECT_CMD_PATH_DIRECTION_RESERVED2 = 8,
        OBS_DETECT_CMD_PATH_DIRECTION_RESERVED3 = 9,
        OBS_DETECT_CMD_PATH_DIRECTION_RESERVED4 = 10,
        OBS_DETECT_CMD_PATH_DIRECTION_N_LEFT = 11,
        OBS_DETECT_CMD_PATH_DIRECTION_N_RIGHT = 12,
        OBS_DETECT_CMD_PATH_DIRECTION_DOUBLE_LEFT = 13,
        OBS_DETECT_CMD_PATH_DIRECTION_DOUBLE_RIGHT = 14,
        OBS_DETECT_CMD_PATH_DIRECTION_RESERVED5 = 15,
    };
    
    enum HOKUYO_OBS_DETECT {
        HOKUYO_OBS_DETECT_NO_USE = 0xF,			//1111 -> 10000 -> 0x10 = 16
        HOKUYO_OBS_DETECT_NORMAL_VALUE = 0xE,   //1110 -> 10001 -> 0x11 = 17
        HOKUYO_OBS_DETECT_BOTH_VALUE = 0xD,  	//1101 -> 10010 -> 0x12 = 18
        HOKUYO_OBS_DETECT_LEFT_VALUE = 0xC,		//1100 -> 10011 -> 0x13 = 19
        HOKUYO_OBS_DETECT_RIGHT_VALUE = 0xB,	//1011 -> 10100 -> 0x14 = 20
        HOKUYO_OBS_DETECT_RESERVED1 = 0xA,
        HOKUYO_OBS_DETECT_RESERVED2 = 0x9,
        HOKUYO_OBS_DETECT_RESERVED3 = 0x8,
        HOKUYO_OBS_DETECT_RESERVED4 = 0x7,
        HOKUYO_OBS_DETECT_RESERVED5 = 0x6,
        HOKUYO_OBS_DETECT_RESERVED6 = 0x5,
        HOKUYO_OBS_DETECT_RESERVED7 = 0x4,
        HOKUYO_OBS_DETECT_RESERVED8 = 0x3,
        HOKUYO_OBS_DETECT_RESERVED9 = 0x2,
        HOKUYO_OBS_DETECT_RESERVED10 = 0x1,
        HOKUYO_OBS_DETECT_RESERVED11 = 0,
    };

    enum SICK_OBS_DETECT {
        SICK_OBS_DETECT_NORMAL_VALUE = 0x0,
	    SICK_OBS_DETECT_BOTH_VALUE = 0x1,
	    SICK_OBS_DETECT_LEFT_VALUE = 0x2,
	    SICK_OBS_DETECT_RIGHT_VALUE = 0x3,
	    SICK_OBS_DETECT_RESERVED1 = 0x4,
	    SICK_OBS_DETECT_RESERVED2 = 0x5,
	    SICK_OBS_DETECT_RESERVED3 = 0x6,
	    SICK_OBS_DETECT_RESERVED4 =	0x7,
	    SICK_OBS_DETECT_RESERVED5 =	0x8,
	    SICK_OBS_DETECT_RESERVED6 =	0x9,
	    SICK_OBS_DETECT_RESERVED7 =	0xA,
	    SICK_OBS_DETECT_RESERVED8 =	0xB,
	    SICK_OBS_DETECT_RESERVED9 =	0xC,
	    SICK_OBS_DETECT_RESERVED10 = 0xD,
    	SICK_OBS_DETECT_RESERVED11 = 0xE,
	    SICK_OBS_DETECT_RESERVED12 = 0xF,
    };

    enum OHT_SPEED_GRADE {
        OHT_SPEED_GRADE_1_BIT = 0x0,			//Not use
        OHT_SPEED_GRADE_2_BIT = 0x0,			//Not use
        OHT_SPEED_GRADE_3_BIT = 0x8,			//1000 -> 0111 = 0x7
        OHT_SPEED_GRADE_4_BIT = 0x9,			//1001 -> 0110 = 0x6
        OHT_SPEED_GRADE_5_BIT = 0xD,			//1101 -> 0010 = 0x2
        OHT_SPEED_GRADE_6_BIT = 0xC,			//1100 -> 0011 = 0x3
        OHT_SPEED_GRADE_7_BIT = 0xE,			//1110 -> 0001 = 0x1
        OHT_SPEED_GRADE_8_BIT = 0xF,			//1111 -> 0000 = 0x0
    };

    enum SERVO_AXIS_TYPE {
        SINGLE_SERVO,
        DUAL_SERVO,
        TRIPLE_SERVO,
        QUAD_SERVO,
    };
    ////////////////////////////////////////////////////////////////////////
// structure / union
////////////////////////////////////////////////////////////////////////
    typedef struct _SyncMotion {
        int Master;
        int Slave;
        int Flag;
        float Gain;
    }SYNC_MOTION;

    typedef union {
        int io_all;
        struct{
            unsigned int io_bit_00 : 1;
            unsigned int io_bit_01 : 1;
            unsigned int io_bit_02 : 1;
            unsigned int io_bit_03 : 1;
            unsigned int io_bit_04 : 1;
            unsigned int io_bit_05 : 1;
            unsigned int io_bit_06 : 1;
            unsigned int io_bit_07 : 1;
            unsigned int io_bit_08 : 1;
            unsigned int io_bit_09 : 1;
            unsigned int io_bit_10 : 1;
            unsigned int io_bit_11 : 1;
            unsigned int io_bit_12 : 1;
            unsigned int io_bit_13 : 1;
            unsigned int io_bit_14 : 1;
            unsigned int io_bit_15 : 1;
            unsigned int io_bit_16 : 1;
            unsigned int io_bit_17 : 1;
            unsigned int io_bit_18 : 1;
            unsigned int io_bit_19 : 1;
            unsigned int io_bit_20 : 1;
            unsigned int io_bit_21 : 1;
            unsigned int io_bit_22 : 1;
            unsigned int io_bit_23 : 1;
            unsigned int io_bit_24 : 1;
            unsigned int io_bit_25 : 1;
            unsigned int io_bit_26 : 1;
            unsigned int io_bit_27 : 1;
            unsigned int io_bit_28 : 1;
            unsigned int io_bit_29 : 1;
            unsigned int io_bit_30 : 1;
            unsigned int io_bit_31 : 1;
        }bits;
    }Io_Structure;

    //Trigger Command
    typedef struct _TRIGGER_CTRL_INFO
    {
        unsigned int StartOnSpeedDown       : 1;        ///<명령 인가 시점( 0:바로 시작, 1:이전 프로파일의 감속 시점에 명령 인가
        unsigned int UseCurrSpeed           : 2;        ///<사용할 속도( 0: 명령 받은 속도, 1: 현재 속도, 2:Min(명령속도, 현재속도), 4:MAX(명령속도, 현재속도) )
        unsigned int UseSmallAdd            : 1;        ///<Small Add ( 0: NoUse, 1: Use )
        unsigned int TriggerIONumber        : 8;        ///< I/O 번호 (0~255): ②
        unsigned int IsTriggerOnRisingEdge  : 1;        ///<Triger Type ( 0: Falling Edge, 1: Rising Edge )
        unsigned int AdditionalDist	        : 19;       ///< 1회 추가 거리 ( 0~2^19 / 방향 정보는 최종 타겟 거리에서 획득): 524288cts --> 주행: 4599mm
    }TRIGGER_CTRL_INFO;

    typedef union _TRIGGER_CTRL_INFO_UNION
    {
        TRIGGER_CTRL_INFO TriggerInfo;  ///< AMC_CMD.Cmd의 bitfield
        int Cmd;
    }TRIGGER_CTRL_INFO_UNION;

    typedef struct
    {
        volatile int g_on;                  // pwm on/off flag
        volatile int in_pos_flag;           // In_Position Flag

        volatile float  error_z0;           // current position error
        volatile int jtpos;                 // current actual position
        volatile float  velfbz0;            // velocity feedback buffer
        volatile double dtBox;              // distance channel

        volatile int event_st;              // Define Event
        volatile int axis_source;           // Axis event define

        volatile int EncoderOffset;

        volatile char curve_limitst;
        volatile int curve_limit;
        volatile int limit_curve_chk_cnt;

        volatile int actvel_margin;
        volatile int actvel_marginst;
        volatile int actvel_margintime;

        volatile double dProfFinalPos;
        volatile double dPosCmd;

        volatile float fScurveRatio;
    } CONTROLPARAM;

    // AMC Trace
    typedef struct _Queue {
        int front;
        int rear;
        int queArr[Trace_Data_Buffer_Size];
    } Queue;

    typedef struct _MQueue {
        int front;
        int rear;
        int queArr[MTrace_Data_Buffer_Size];
    } MQueue;


    class Amc4OHT;
    class OhtIo: public Behavior {
        using parent = Behavior;
        void update(){}
    public:
        OhtIo(OHTController* controller):parent(controller){ name_ = "ohtio"; }
        friend class Amc4OHT;
    };

    class filter {
    public:
        void DetectFilter(unsigned new_val, unsigned &filter_val, unsigned count_limit);
    private:
        unsigned temp_val;
        unsigned cnt;
    };

    class Amc4OHT : public AmcEmulator {

    public:
        Amc4OHT();
        ~Amc4OHT();
        void finalize();
        void printState();
        void stopPulse(int ax,double dcc);

        std::unique_ptr<amc::Sound> sound_; // Alsa, 210830 yjchoe

        void log_delete();
        void remove_file_in_list_day(std::vector<std::string> list, uint32_t duration);
        void remove_file_in_list_hour(std::vector<std::string> list, uint32_t duration);


    protected:
        //  io wrapper
        bool readRawDi(unsigned portid){ return oht_io_->readRawDi(portid);}
        bool readRawDo(unsigned portid){ return oht_io_->readRawDo(portid);}
        bool writeRawDo(unsigned portid, bool value){ return oht_io_->writeRawDo(portid,value);}
        bool readDi(unsigned portid){ return oht_io_->readDi(portid);}
        bool readDo(unsigned portid){ return oht_io_->readDo(portid);}
        bool writeDo(unsigned portid, bool value){ return oht_io_->writeDo(portid,value);}
        bool checkDi(unsigned portid, bool expected_value){ return oht_io_->checkDi(portid, expected_value);}

        //internal function
        void statusUpdate_thread(void);
        void logUpdate_thread(void);
        void statusUpdate1ms(void);
        void registerAntiCollisionControlTimer(void);
		void updateAntiCollisionControl(void);
		void updateAntiCollisionControl_test(void);
		double Get_Velocity_by_Area(void);
		void setCollisionDetector();
		void check_Event(int ax);
        void set_abort_event(int ax);
        void update_control_State(int ax);
        void check_Servo(int ax);
        void check_SWInPosition(int ax);
        void check_SwLimit(int ax);
        void check_Envelope(int ax);

        void update_Io_InData(void);
        void get_IoIn_RawData(uint32_t* data);
        void get_IoOut_Data(uint32_t* data);
        void set_IoOut_Data(uint32_t* data);

        int set_Amp_Enable(int ax, bool state);
        int reset_Amp_Fault(int ax);

        void set_Position(int ax, int pos);

        void do_AxisCmd(AMC_CMD_RAW& amc_cmd, int ax);

        /* sub functions */
        int GetBit(int Source, int BitNum);
        int GetBit(uint32_t *IO_arr, int BitNum);
        int GetPos(void);
        void Check_Rear_servo_Off(void);
        void Check_Front_Rear_servo_On(void);

        int CalBit(uint32_t *IO_arr, int BIT_Index, int BIT_Size);   // 200811 yjchoe
        double CalVelRatio(double Vel, int OHT_ID);
        void MoveStopMotion(int select_curve);
        void MoveDistanceMotion(int select_curve);
        void MoveSmallAdd();
        void Trace_Execute_1(int ax, int ACC, int DCC, int VEL, int POS);
        int DynamicACC(double vel);
        void CurrentSpeedMode(int ax, double vel, double prev_vel);
        //void CurrentSpeedMode(int ax);

        int ConvertOHTDetectInfoMainToAmc(char DetectInfo);
        int ConvertOBSDetectInfoMainToAmc(char DetectInfo);

        void Init_ChangeNodeInfo();

        unsigned short crc16_ccitt(const char *but, int len);   //CIS  20191210

        // Voltage Monitoring, 210618 yjchoe
        int Onboard_Voltage(int ch);
        void CheckNodeCountStatus(void);
        void Check_Monitoring_System(void);

        //service function
        virtual void statusUpdate(void);
        virtual void Reserved_Function(void);

        virtual void Servo_On(void);                //24 - Stocker & oht
        virtual void Servo_Off(void);               //25 - Stocker & oht
        virtual void Alarm_Reset(void);             //26 - Stocker & oht
        virtual void Alarm_Set(void);               //27 - Stocker & oht

        virtual void Clear_Status(void);            //28 - Stocker & oht
        virtual void Get_Frames_Clear(void);        //66 - Stocker & oht

        virtual void Get_SW_Upper_Limit(void);      //76 - Stocker & oht
        virtual void Put_SW_Upper_Limit(void);      //77 - Stocker & oht
        virtual void Get_SW_Lower_Limit(void);      //78 - Stocker & oht
        virtual void Put_SW_Lower_Limit(void);      //79 - Stocker & oht

        virtual void Get_SyncMap(void);             //89 - oht
        virtual void Get_SyncOn(void);              //90 - oht
        virtual void Get_SyncOff(void);             //91 - oht
        virtual void Get_SyncGain(void);            //92 - oht

        virtual void Exist_Check(void);             //111 - Stocker & oht
        virtual void Set_Oht_Model_id(void);        //112 - oht
        virtual void Get_Oht_Model_id(void);        //113 - oht

        virtual void AcceptString(void);            //152 - Stocker & oht

        virtual void UploadParamToIPC(void);        //153 - Stocker & oht
        virtual void DnloadParamFromIPC(void);      //154 - Stocker & oht

        virtual void SetEncoderOffset(void);        //157 - oht
        virtual void GetEncoderOffset(void);        //158 - oht
        virtual void _dsp_param_flush(void);        //159 - Stocker & oht
        virtual void WatchdogOperations(void);      //160 - oht

        virtual void FSExchange(void);              //171 - oht

        virtual void TraceUpdate_1(void);           //178 - oht
        virtual void TraceUpdate(void);             //179 - oht
        virtual void GetAMCData70(void);            //180 - oht

        virtual void Get_Vel_Curve(void);           //183 - oht
        virtual void Get_Actvel_Margin(void);       //185 - oht

        virtual void SetIOCount(void);              //189 - oht

        virtual void DoAutopatch(void);             //199 - oht
        virtual void GetVersion(void);              //200 - oht
        virtual void GetEcmVersion(void);              //201 - oht
        virtual void StopAll(void);              //201 - oht

        void InitializeIO();
        void InitializeOhtModel();

        //void SetDetectorOutput(int OHTSettingArea, int OBSSettingArea, char OHTDetectType, char OBSDetectType);
        void SetDetectorOutput(int OHTSettingArea, int OBSSettingArea);     // 230202 yjchoe

    public:
        /*
        enum AMC_DI_NUM                    //TODO: Trigger Sensor를 상위에서 자유롭게 선택할 수 있어야 함
        {//0-base
            DRIVING_FRONT       = 54,
            DRIVING_PRECHECK    = 55,
            TRANS_FRONT         = 56,
            TRANS_REAR          = 57,
            HOIST_HOME          = 73,   // add hoist home, 210616 yjchoe
        };  // OHT Model에 따른 I/O 변경을 Onboard I/O에서 대응 가능한지 확인 필요
        */

        struct OHTPara {
            array<short,4> home_level;
            array<short,4> positive_level;
            array<short,4> negative_level;

            array<short,4> amp_fault_st;
            array<short,4> pos_limit_st;
            array<short,4> neg_limit_st;
            array<short,4> home_limit_st;
            array<short,4> error_limit_st;

            array<short,4> swupper_limit_st;
            array<short,4> swlower_limit_st;

            array<short,4> event_type;
            array<short,4> stop_rate;
        };
        void set_Move_Dist_Pulse(int ax, int dist,int vel, int acc, int dcc);
        void set_Move_Dist_mm(int ax, double dist,double vel, double acc, double dcc);
        void get_Io(int num);

        short VERSION_DSP;
        short VER1;
        short VER2;
        short VER3;

////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////
        int trace_cnt_iteration;
        int trace_on_flag[TOTAL_AXIS_NUM];
        int Trace_Update_Status;

////////////////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////////////////
        void QueueInit(Queue *pq);
        int QIsEmpty(Queue *pq);
        int NextPosIdx(int pos);
        void Enqueue(Queue *pq, int data);
        int Dequeue(Queue *pq);
        void MQueueInit(MQueue *pq);
        int MQIsEmpty(MQueue *pq);
        int MNextPosIdx(int pos);
        void MEnqueue(MQueue *pq, int data);
        int MDequeue(MQueue *pq);
        //void Trace_Execute(CONTROLBUF * pCB, int ax);
        void Trace_Execute(int ax);
	// semes
    protected:
        OhtIoConfig ioconfig_;
        OHTController controller;
        unique_ptr<OhtIo> oht_io_;
        SingleAxisAmcDrive* front_wheel;
        SingleAxisAmcDriveRear* rear_wheel;
        SingleAxisAmc* hoist;
        SingleAxisAmc* slide;
        array<SingleAxisAmc*,4> oht_;
        array<EcServoMotor*,4> motors;
        EcOHTFrontIo* oht_front_io;
        EcOHTRearIo* oht_rear_io;
        OhtOnboardIO* oht_onboard_io;
        oht::OhtConfig* config;
		// susi
		//unique_ptr<fpx::susi_atlas> susi;
        unique_ptr<fpx::max11609> pMax { nullptr };  // Voltage Monitoring, 210618 yjchoe
        unique_ptr<fpx::susi_atlas> pSusi { nullptr };

        const MarkDetector* getDetector(int di_num);

        bool stop_;
        int oht_pulse_ratio_;
        int emo_status;
        int io_in_count;
        int io_out_count;
        uint32_t IO_Input[8];
        uint32_t IO_Output[8];
        OHTPara oht_para_;
        SYNC_MOTION sync_motion_;

        std::array<CONTROLPARAM, 4> ctr_param;
        std::unique_ptr<std::thread> oht_isr_;
        //std::unique_ptr<std::thread> oht_log_;
        std::mutex oht_isr_lock_;
        DETECT_INFO detector_set_cmd_;
        std::chrono::system_clock::time_point oht_dec_stop_time_;

//      Cia402StatusWord slaveStatus;   // 210707 yjchoe
////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////
        int Count_1ms_INT_4_Debug;
        int AMC_Follow_Control_Status;
        int AMC_Pause_Control;
        int PauseCmd;
        int PauseBy;
        int PauseFlag;
        int Model_OHT_ID;
        int Model_OHT_DETECT_WRITE_BIT_MASK;
        int Model_OBS_DETECT_WRITE_BIT_MASK;
        int Model_DRIVINGPRECHECK;
        int Model_DRIVINGFRONTNUM;
        int Model_TRANSFRONT;
        int Model_TRANSREAR;
        int Model_HOISTHOME;
        int Model_OHT_DETECT_WRITE_BIT_SCALE;
        int Model_OBS_DETECT_WRITE_BIT_SCALE;
        int Model_OHT_DETECT_READ_BIT_SCALE;
        int Model_OBS_DETECT_READ_BIT_SCALE;
        int Model_FOLLOWSDCC;

        int Model_OBS_SENSOR_MAKER;
        int Model_OHT_SENSOR_ENABLE;

        int Model_OHT_Service;

        int Model_FILTER_4_NODECNT;

        int Model_AXIS_STOP_COUNT_LIMIT;

        int Model_DIST_ARRIVED_LEVEL1;
        int Model_DIST_ARRIVED_LEVEL2;

        int OHT_DETECT_N_VALUE_Flag;
        int OHT_DETECT_N_Distance;

        double Model_PULSETOMM;
        double Model_PULSETOMM_HOIST;
        double Model_PULSETOMM_SLIDE;

        double Model_SPEED_GRADE_1_VELOCITY;
        double Model_SPEED_GRADE_2_VELOCITY;
        double Model_SPEED_GRADE_3_VELOCITY;
        double Model_SPEED_GRADE_4_VELOCITY;
        double Model_SPEED_GRADE_5_VELOCITY;
        double Model_SPEED_GRADE_6_VELOCITY;
        double Model_SPEED_GRADE_7_VELOCITY;
        double Model_SPEED_GRADE_8_VELOCITY;

        double Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
        double Model_MOVE_S_DCC_BY_SENSOR_CURVE;
        double Model_MOVE_S_DCC_BY_SENSOR_ETC;
        double Model_MOVE_DS_ACC_BY_SENSOR_CURVE;
        double Model_MOVE_DS_DCC_BY_SENSOR_CURVE;
        double Model_MOVE_DS_DCC_BY_SENSOR_ETC;
        int MoveDS_Flag;
        int MoveS_Flag;
        int Move_Flag;
        int Debug_Flag;
        int OHT_Detect_ChangeNodeValue;
        int OHT_Detect_AreaSetting;
        int OHT_Detect_AreaSetting_For_Curve;
        int OHT_Detect_AreaSetting_Flag;
        int OHT_Detect_AreaSetting_Flag2;
        int OHT_PBS_IgnoreSettingFlag;
        int OHT_PBS_IgnoreFlag;
        int OHT_PBS_IgnoreChangeNodeValue;
        int PBS_Ignore_Flag;
        int OBS_Detect_AreaSetting;
        int tmp_OBS_Detect_AreaSetting;
        double OHT_velocity_Level;
        double OBS_velocity_Level;
        int AMC_Stop_count;
        int Rear_servo_Off_flag;
        double Sensor_velocity_Level;
        double tmpFollowsACC;
        double FollowsACC;
        double FollowsDCC;
        double FollowsDCC_CMD;
        double FollowsVEL;
		double FollowsPOS;
		double FollowsDIS;
        double FollowsACC_Prev;
        double FollowsDCC_Prev;
        double FollowsVEL_Prev;
        double TargetVEL;
        double TargetACC;
        double TargetDCC;
        struct DrivingCommand {
            double FollowsVEL;
            double TargetPOS_abs;
            double TargetVEL;
            double TargetACC;
            double TargetDCC;
            bool enable_anticollision;
        } drv_cmd;
        enum AntiCollisionControlState { ACC_S0, ACC_S1, ACC_S2} acc_state_;
        double FollowsPOS_Prev;
        int Follows_MoveDS_Flag;
        int Flag_FollowCtrl_Off_Edge;
        int Flag_FollowCtrl_Off_Edge_Count;
        int OBS_Distance;
        int OBS_Ignore_Distance;
        double Target_Velocity_by_Sensor;
        double Target_velocity_cmd;
        int MoveDS_Wait_Count_After_Servo_On;
        int MoveDS_Wait_Count_After_Servo_On_Flag;
        int MoveDS_Wait_Count_After_Stop_Flag;
        int MoveDS_Wait_Count_After_Stop;
        TRIGGER_CTRL_INFO_UNION TriggerCtrlInfoParsing[4];
        double ACC_For_Marking[4];
        double DCC_For_Marking[4];
        double VEL_For_Marking[4];
        int POS_For_Marking[4];
        int POS_For_Marking_Final[4];
        int UseSmallAdd_For_Marking[4];
        int IoNumber_For_Marking[4];
        int IoType_For_Marking[4];
        int DistanceAddition_For_Marking[4];
        int DistanceLimit_For_Marking[4];
        int JtPos_During_Marking[4];
        int Marking_Process_Flag[4];
        int Marking_Process_Trigger_Flag[4];
        int DirectionFlag_For_Marking[4];
        int FlagIO_For_Marking[4];
        int Follow_Enable_Flag;
        int OHT_Sensor_value;
        int OBS_Sensor_value;

        int TorqueLimit_Request;
        int TorqueLimit_Response;
        int Path_Slope_Status;
		int Servo_Off_type;
		bool MoveDSExecute;
		int Final_Arrival_Flag;
        int Improve_stop_motion_flag;
        int bStop_motion_flag;
        int bIsPosSoftStopToMark;
        unsigned obs_sensor_val;
        unsigned oht_sensor_val;
        int LowVibration_Flag;
        int UBG_Setting_flag;

        int OBS_SENSOR_VEL_LONG;
        int OBS_SENSOR_VEL_MIDDLE;
        int PATH_SLOPE_DCC;
        double STOP_MOTION_DCC_LOW;
        double STOP_MOTION_DCC_HIGH;
        int DYNAMIC_ACC_1st;
        int DYNAMIC_ACC_2nd;
        int DYNAMIC_ACC_3rd;
        int DYNAMIC_ACC_4th;
        int FlagFirstNode;

        int currentSpeedMode[4];
        int acc_count[4];
        int dcc_count[4];
        int cruise_count[4];
        int Acc_Offset;
        int Dcc_Offset;
        int Cruise_Offset;
        double prev_vel[4];

        double ACC_CHANGE_SPEED1;
        double ACC_CHANGE_SPEED2;
        double ACC_CHANGE_SPEED3;

        int EnableChangeNodeMode;
        int ChangeNodeSetFlag;
        int SetAreaPosition;
        int SetAreaPosition_for_combo;
        int ChangeNodeComboSetFlag;
        int ChangeNodeOHTDetectArea;
        int ChangeNodeOBSDetectArea;
        int SelectChangeNode;
        int CHANGENODE_DIST_LIMIT;
        OPTIONAL_NODE_INFO OptNodeInfo_;

        bool runningflag[4];
        int trqflag;
        int stop_cnt;
        double ActualVel[4];
        int dc_voltage[4];
        double Power_Upper_Limit[4];
        double Power_Lower_Limit[4];

        bool reset_status[4];

        void Check_refpos(void);
        int prev_pos;
        int detectcnt;
        bool overtarget;

        //void check_actpos(int ax);
        void check_actpos();
        int32_t prev_outpos[4];
        double prevACC[4];
        double prevDCC[4];
        double prevVEL[4];
        double prevPOS[4];

        filter ohtdetect;
        filter obsdetect;

        bool cmd_flag;
        int cmd_delay;
    };
}

#endif //SEPMASTER_AMC_OHT_H


