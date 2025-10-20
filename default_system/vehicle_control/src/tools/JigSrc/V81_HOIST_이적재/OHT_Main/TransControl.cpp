// ---------------------------------------------------------------------------
/**
 @file    TransControl.cpp
 @version OHT 7.0
 @brief   TransControl Class CPP File
 */
#include <IniFiles.hpp>
#include <Filectrl.hpp>
#include <math.h>
#include "OHTMain.h"
#include "TransControl.h"
#include "Def_TPCommand.h"

#define NORMAL_TYPE 0
#define DEF_NORMAL_TYPE 1
#define DEF_TRACKER_SHIFT_TYPE 2
#define OHTRUN_TYPE NORMAL_TYPE // msec
#define TRACKER_DELAY_TIME 15000    // msec

#define MAX_BYTE_SIZE_OF_TRANS_LOG  1024*10

#define  EQPIO_BASE_CHANNEL         178     //LimJY_20160119 : For IR+RF PIO 시작 채널 178 (178 ~ 217 사용가능)

#define HOIST_POSITIVE_LIMIT_ON_WAIT    10.0      // mm
#if(OHT_NAME == OHT_NAME_STD_V85)
#define HOIST_NEGATIVE_LIMIT_ON_WAIT    -7.0    // mm
#else
#define HOIST_NEGATIVE_LIMIT_ON_WAIT    -1.0    // mm
#endif
#define HOIST_LIMIT_ON_WAIT             10.0  // mm

#define HOIST_HIGH_SPEED_SHIFT_LIMIT		310.0

#define HOIST_HIGH_SPEED_SHIFT_LIMIT		310.0   //추후 삭제 유무 확인 puting

#define SHIFT_POSITIVE_LIMIT_ON_WAIT    20.0      // mm
#define SHIFT_NEGATIVE_LIMIT_ON_WAIT    -20.0 // mm
#define SHIFT_LIMIT_ON_WAIT             20.0  // mm


#define HOIST_ABNOMARL_MOVE_SPEED_LIMIT         0.2     // mm/msec
#define SHIFT_ABNOMARL_MOVE_SPEED_LIMIT         0.2     // mm/msec
#define ROTATE_ABNOMARL_MOVE_SPEED_LIMIT         0.2     // mm/msec



// 2016-11-28, Add by Labeler
#define ROTATE_MOVELIMIT_P           1900        // Degree
#define ROTATE_MOVELIMIT_N          -100         // Degree
#define ROTATE_MOVELIMIT             1          //

#define DIST_HOIST_HOME_MARGIN      1.0     // mm
#define DIST_SHIFT_HOME_MARGIN      1.0     // mm

#define OUTTRIGGER_IS_LENG    100.0    //55.0
// ---------------------------------------------------------------------------
// TransControl 관련 Define
#define NAME_TRANSFILE              "TransParam.ini"

// Hoist 저속 구간 길이
#define DEF_nUpstate_SpeedDown_Offset    60.0    //mm
#define DEF_nDnState_UP_SpeedDown_Offset 50.0    //mm
#define DEF_nDnState_DN_SpeedDown_Offset 45.0    //mm

#define LOOKDN_DETECT_TIMEOVER     8000    //msec

// by zzang9un 2013.09.27 : Lookdown 감지 범위 추가
// by LSJ 2013.10.09 : Lookdown 감지 범위 늘림
//#define LOOKDOWN_DETECT_RANGE           200.0 ///< Lookdown 감지 범위
#define LOOKDOWN_DETECT_RANGE           350.0 ///< Lookdown 감지 범위
#define LOOKDOWN_DETECT_RANGE_MAX       1000.0 ///< Lookdown 감지 범위

#define TRANS_TIMEOVER             60000   //30000   //msec
#define OSCILLATIONCHECKDIST       15.0     //mm
#define HOIST_MOVINPOS_LIMIT       25.0  //mm 임시
#define HOIST_MOVINPOS_LIMIT_LOAD_DOWN	13.0

#define SHIFTROTATE_HOME2TP            1 //ShiftRotate ORIGN -> TPOINT
#define SHIFTROTATE_TP2HOME            2 //ShiftRotate TPOINT -> ORIGIN

#define AUTORECOVERYTYPE_E84           0  //E84 Auto Recovery
#define AUTORECOVERYTYPE_BLANK         1  //공출하
#define AUTORECOVERYTYPE_DOUBLE        2  //이중반입


///@todo 추후 값 변경 필요
#define SHIFT_FAST_MOVE_DIST       80.0      //80mm
#define OSCILLATION_IGNORE_DIST    250.0   //mm
#define IS_STB_LIMIT               200.0   //mm
#define IS_STK_LIMIT               1000.0   //mm
#define IS_FAST_MOVE_LIMIT         2000.0   //mm

#define HOIST_DOWN_STATE     0
#define HOIST_UP_STATE       1

#define MOVETP_RETRYCOUNT         30
#define AUTOTEACHING_MOVETP_RETRYCOUNT         40

#define STBUTBSensorOFFTime        3000    //msec
#define STBUTBSensorONTime        3000    //msec
//#define RAILSUPPORT_HOMING_TIMEOUT        10000    //msec
// ---------------------------------------------------------------------------
// Content : TRANS_MOVING_CASE
// ---------------------------------------------------------------------------
#define HOIST_DOWN_H2TO            1 //ORIGIN -> TPOINT-A
#define HOIST_DOWN_TO2TP           2 //TPOINT-A -> TPOINT
#define HOIST_DOWN_H2TP            3 //ORIGIN -> TPOINT
#define HOIST_UP_TP2TO             4 //TPOINT -> TPOINT-A
#define HOIST_UP_TP2HO             5 //TPOINT -> ORIGIN-A
#define HOIST_UP_TO2HO             6 //TPOINT-A -> ORIGIN-A
#define HOIST_UP_HO2H              7 //ORIGIN-A -> ORIGIN
#define HOIST_UP_TO2H              8 //TPOINT-A -> ORIGIN
#define HOIST_DOWN_1               9
#define HOIST_UP_1                 10
#define HOIST_UP_x                 11
#define HOIST_UP_xp                12
#define HOIST_UP_2H                13

#ifdef USE_TRANS_PROFILE_VER2
/*************************************
Velocity profile (TP -> H)
          /------------\ <-- SDP (slow down position)
         /              \
 /------/                \----\
/                              \
TP      TO               HO     H
*************************************/                                                
#define HOIST_DOWN_H2SDP		14	// Home to slow down position
#define HOIST_DOWN_SDP2TO		15	// slow down position to teaching offset
#define HOIST_UP_TO2SDP			16	// teaching offset to slow down position
#define HOIST_UP_SDP2HO			17	// slow down position to home offset
#endif

#define SHIFT_SPEED_RATIO      1.0
#define SHIFT_MAX_SPEED        0.475*SHIFT_SPEED_RATIO ///< Shift 이적재 최고 속도(mm/msec) : 0.475

#define MULTIPLY_34_80P        0.8         // UNIT:80%
#define MULTIPLY_34_70P        0.7         // UNIT:70%
#define MULTIPLY_34_50P        0.5         // UNIT:70%
#define MULTIPLY_34_30P        0.3         // UNIT:30%
#define MULTIPLY_34_20P        0.2         // UNIT:20%
#define MULTIPLY_34_10P        0.1         // UNIT:10%
#define MULTIPLY_34_03P        0.03         // UNIT:3%
#define MULTIPLY_34_100P       1.0         // UNIT:100%

#define SHIFT_NC_ACCTIME       400   //msec
#define SHIFT_ACCTIME          600   //msec

#if((OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S) ||(OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
#define HOIST_SPEED_RATIO      0.8573       // OHT V7.0 감속비 12.86:1
#else
#define HOIST_SPEED_RATIO      1.0      // OHT V7.0 감속비 15:1
#endif

#ifdef USE_TRANS_PROFILE_VER2
#if(OHT_NAME == OHT_NAME_STD_V85)
#define HOIST_MAX_SPEED	2.75
#define HOIST_ESTOP_DECEL       (HOIST_MAX_SPEED / 400.0)   // msec^2
#define SHIFT_ESTOP_DECEL       (SHIFT_MAX_SPEED / 280.0)   // msec^2
#else
#define HOIST_MAX_SPEED        2.14*HOIST_SPEED_RATIO ///< Hoist 이적재 최고 속도(mm/msec) : 02.14
#define HOIST_ESTOP_DECEL       (HOIST_MAX_SPEED / 280.0)   // msec^2
#define SHIFT_ESTOP_DECEL       (SHIFT_MAX_SPEED / 280.0)   // msec^2
#endif
#else
#define HOIST_MAX_SPEED        2.14*HOIST_SPEED_RATIO ///< Hoist 이적재 최고 속도(mm/msec) : 02.14

// by zzang9un 2013.09.27 : Hoist E-Stop 감속도 변경(400.0 -> 280.0)
#define HOIST_ESTOP_DECEL       (HOIST_MAX_SPEED / 280.0)   // msec^2
#define SHIFT_ESTOP_DECEL       (SHIFT_MAX_SPEED / 280.0)   // msec^2
#endif
#define HOIST_TURNOFF_DECEL       (HOIST_MAX_SPEED / 280.0)   // msec^2
#define SHIFT_TURNOFF_DECEL       (SHIFT_MAX_SPEED / 280.0)   // msec^2


#define MULTIPLY_2_100P        1.0         // UNIT:100%
#define MULTIPLY_2_95P         0.95        // UNIT:90%
#define MULTIPLY_2_90P         0.90        // UNIT:90%
#define MULTIPLY_2_80P         0.8         // UNIT:80%
#define MULTIPLY_2_70P         0.7         // UNIT:70%
#define MULTIPLY_2_60P         0.6         // UNIT:60%
#define MULTIPLY_2_50P         0.5         // UNIT:50%
#define MULTIPLY_2_40P         0.4         // UNIT:40%
#define MULTIPLY_2_30P         0.3         // UNIT:30%
#define MULTIPLY_2_20P         0.2         // UNIT:20%
#define MULTIPLY_2_15P         0.15        // UNIT:15%
#define MULTIPLY_2_10P         0.1         // UNIT:10%
#define MULTIPLY_2_6P          0.06        // UNIT:6%
#define MULTIPLY_2_5P          0.05        // UNIT:5%
#define MULTIPLY_2_4P          0.04        // UNIT:4%
#define MULTIPLY_2_3P          0.03        // UNIT:3%
#define MULTIPLY_2_2P          0.02        // UNIT:2%
#define MULTIPLY_2_1P          0.01        // UNIT:1%
#define MULTIPLY_2_05P          0.005        // UNIT:1%

#define HOIST_ACCTIME_50        50  //0.05     // UNIT:50MSEC
#define HOIST_ACCTIME_100      100 //0.15     // UNIT:150MSEC
#define HOIST_ACCTIME_150      150 //0.15     // UNIT:150MSEC
#define HOIST_ACCTIME_200      200 //0.15     // UNIT:150MSEC
#define HOIST_ACCTIME_250      250 //0.15     // UNIT:150MSEC
#define HOIST_ACCTIME_300      300 //0.3     // UNIT:300MSEC
#define HOIST_ACCTIME_500      500 //0.3     // UNIT:300MSEC
#define HOIST_ACCTIME_600      600 //0.6     // UNIT:600MSEC
#define HOIST_ACCTIME_700      700 //0.3     // UNIT:300MSEC
#define HOIST_ACCTIME_1000     1000 //0.3     // UNIT:300MSEC

#if(OHT_NAME == OHT_NAME_STD_V85)
#define HOIST_POS_LIMIT            6000.0
#else
#define HOIST_POS_LIMIT            5100.0
#endif


#define HOIST_NEG_LIMIT            (HOIST_MOVELIMIT_N-2.0)  //2nd Axis(Hoist)  Negative Software Limit
#define SHIFT_POS_LIMIT             (SHIFT_MOVELIMIT_P+10)  //3rd Axis(Shift)  Positive Software Limit  151086.0 / 144000.0
#define SHIFT_NEG_LIMIT             (SHIFT_MOVELIMIT_N-10)  //3rd Axis(Shift)  Negative Software Limit  -121354.0 / -121000.0

#define INPOS_LIMIT                2.0  //500.0
#define RETICLE_SHIFT_INPOS_LIMIT  0.3  //500.0
#define HOIST_POS_LIMIT_FORDRV     10.0 //5000.0      //주행시 Hoist Limit 설정
#define HOIST_NEG_LIMIT_FORDRV     -1.0 //-5000.0      //주행시 Hoist Limit 설정
#define SHIFT_POS_LIMIT_FORWAIT     20.0   //20000.0      //주행시 Shift Limit 설정
#define SHIFT_NEG_LIMIT_FORWAIT     -20.0    //-20000.0
#define HOIST_POS_LIMIT_FORARRIVED     10.0 //5000.0      //정지시 Hoist Limit 설정
#define HOIST_POS_LIMIT_TRANSOFFSET     35.0 //5000.0      //이적재시 Hoist Limit설정 시 Offset


// ---------------------------------------------------------------------------
// Content : Hoist, Shift, Rotate Origin Position
// ---------------------------------------------------------------------------
#define HOIST_ORIGIN_LIMIT        HOIST_HOME_INPOS_LIMIT  //500.0
#define SHIFT_ORIGIN_LIMIT         2.0  //500.0
#define ROTATE_ORIGIN_LIMIT        2.0  //550.0

// ---------------------------------------------------------------------------
// Content : Hoist, Shift Axis value
// ---------------------------------------------------------------------------
#define HOIST_POISTION_ON_PULSE0       -1000
#define HOIST_POISTION1_PULSE_MIN   50
#define HOIST_POISTION_ON_USER0        -3
#define HOIST_POISTION1_USER_MIN    50

#define HOIST_POISTION_ON_PULSE1       250000
#define HOIST_POISTION2_PULSE_MIN   100
#define HOIST_POISTION_ON_USER1        732
#define HOIST_POISTION2_USER_MIN    100

#define HOIST_POISTION_ON_PULSE2       550000
#define HOIST_POISTION3_PULSE_MIN   150
#define HOIST_POISTION_ON_USER2        1594
#define HOIST_POISTION3_USER_MIN    150

#define HOIST_POISTION_ON_PULSE3       1240000
#define HOIST_POISTION4_PULSE_MIN   200
#define HOIST_POISTION_ON_USER3        3400
#define HOIST_POISTION4_USER_MIN    200

#define HOIST_HOME_OFFSET_RAIL2JIG  0//200
#define HOIST_HOME_SEARCH_OFFSET    0

#define HOIST_ACCEL_MAXLIMIT       6000
#define HOIST_ACCEL_MAXLIMIT_MIN   5000
#define HOIST_ACCEL_MAXLIMIT_MAX   7000

#define HOIST_ACCEL_MINLIMIT       3000
#define HOIST_ACCEL_MINLIMIT_MIN   2000
#define HOIST_ACCEL_MINLIMIT_MAX   4000

#define HOIST_DECEL_MAXLIMIT       6000
#define HOIST_DECEL_MAXLIMIT_MIN   5000
#define HOIST_DECEL_MAXLIMIT_MAX   7000

#define HOIST_DECEL_MINLIMIT       3000
#define HOIST_DECEL_MINLIMIT_MIN   2000
#define HOIST_DECEL_MINLIMIT_MAX   4000

#define HOIST_SPEED_MAXLIMIT      400
#define HOIST_SPEED_MAXLIMIT_MIN  350
#define HOIST_SPEED_MAXLIMIT_MAX  450

#define HOIST_SPEED_MINLIMIT      10
#define HOIST_SPEED_MINLIMIT_MIN  5
#define HOIST_SPEED_MINLIMIT_MAX  15

#define HOIST_ABNORMALMOVESPEEDMARGIN      1.0
#define HOIST_ABNORMALMOVESPEEDMARGIN_MIN  0.8
#define HOIST_ABNORMALMOVESPEEDMARGIN_MAX  1.2

#define HOIST_POSITION_MAXLIMIT          1500
#define HOIST_POSITION_MAXLIMIT_MIN      1200
#define HOIST_POSITION_MAXLIMIT_MAX      1700

#define HOIST_POSITION_MINLIMIT          1000
#define HOIST_POSITION_MINLIMIT_MIN      800
#define HOIST_POSITION_MINLIMIT_MAX      1200

#define HOIST_JOGPOSITION_MAXLIMIT       500
#define HOIST_JOGPOSITION_MAXLIMIT_MIN   400
#define HOIST_JOGPOSITION_MAXLIMIT_MAX   600

#define HOIST_JOGPOSITION_MINLIMIT       200
#define HOIST_JOGPOSITION_MINLIMIT_MIN   150
#define HOIST_JOGPOSITION_MINLIMIT_MAX   250

#define HOIST_ACCEL_DEFAULT              4500
#define HOIST_DECEL_DEFAULT              4500
#define HOIST_EMERGENCYDECEL_DEFAULT     4000


#define SHIFT_HOME_OFFSET_MTL2JIG         0.0
#define SHIFT_HOME_SEARCH_OFFSET         0

#define SHIFT_ACCEL_MAXLIMIT             40
#define SHIFT_ACCEL_MAXLIMIT_MIN         35
#define SHIFT_ACCEL_MAXLIMIT_MAX         45

#define SHIFT_ACCEL_MINLIMIT             20
#define SHIFT_ACCEL_MINLIMIT_MIN         15
#define SHIFT_ACCEL_MINLIMIT_MAX         25

#define SHIFT_DECEL_MAXLIMIT             50
#define SHIFT_DECEL_MAXLIMIT_MIN         45
#define SHIFT_DECEL_MAXLIMIT_MAX         55

#define SHIFT_DECEL_MINLIMIT             25
#define SHIFT_DECEL_MINLIMIT_MIN         20
#define SHIFT_DECEL_MINLIMIT_MAX         30

#define SHIFT_SPEED_MAXLIMIT             200
#define SHIFT_SPEED_MAXLIMIT_MIN         180
#define SHIFT_SPEED_MAXLIMIT_MAX         220

#define SHIFT_SPEED_MINLIMIT             80
#define SHIFT_SPEED_MINLIMIT_MIN         70
#define SHIFT_SPEED_MINLIMIT_MAX         90

#define SHIFT_ABNORMALMOVESPEEDMARGIN       1.0
#define SHIFT_ABNORMALMOVESPEEDMARGIN_MIN   0.8
#define SHIFT_ABNORMALMOVESPEEDMARGIN_MAX   1.2

#define SHIFT_POSITION_MAXLIMIT          1500
#define SHIFT_POSITION_MAXLIMIT_MIN      1400
#define SHIFT_POSITION_MAXLIMIT_MAX      1600

#define SHIFT_POSITION_MINLIMIT          200
#define SHIFT_POSITION_MINLIMIT_MIN      150
#define SHIFT_POSITION_MINLIMITMAX       250

#define SHIFT_JOGPOSITION_MAXLIMIT       2000
#define SHIFT_JOGPOSITION_MAXLIMIT_MIN   1800
#define SHIFT_JOGPOSITION_MAXLIMIT_MAX   2200

#define SHIFT_JOGPOSITION_MINLIMIT       1000
#define SHIFT_JOGPOSITION_MINLIMIT_MIN   900
#define SHIFT_JOGPOSITION_MINLIMIT_MAX   1100

#define SHIFT_ACCEL_DEFAULT              2500
#define SHIFT_DECEL_DEFAULT              2500
#define SHIFT_EMERGENCYDECEL_DEFAULT     2000

// ---------------------------------------------------------------------------
// Content : Trans use Part
// ---------------------------------------------------------------------------
#define EQPIO_DEFAULT            true
#define GRIPPERSEN_DEFAULT       true
#define OSCILLATIONSEN_DEFAULT   true
#define HANDPUSHSEN_DEFAULT      true
#define LOOKDOWNSEN_DEFAULT      false
#define SHUTTERCONTROL_DEFAULT   true
#define HANDSEN_DEFAULT          true
#define HOISTALIGN_DEFAULT       true
#define STBSEN_DEFAULT           true
#define UTBSEN_DEFAULT           true
#define UTBFOUPCHECK_DEFAULT     false
#define IGNOREPIOERROR_DEFAULT   false
#define TRANSSOUND_DEFAULT       false
#define OBSDETECTSOUND_DEFAULT   false
#define DRIVINGSOUND_DEFAULT	 false
#define EQPIOTYPE_DEFAULT	     false
#define OSCILLATIONSENPAUSE_DEFAULT	     false

// ---------------------------------------------------------------------------
// Content : Trans define Part
// ---------------------------------------------------------------------------
#define HOIST_POSITIVE_LIMIT_ON_WAIT_MAX       15.0
#define HOIST_NEGATIVE_LIMIT_ON_WAIT_MIN       -5.0
#define SHIFT_POSITIVE_LIMIT_ON_WAIT_MAX       25.0
#define SHIFT_NEGATIVE_LIMIT_ON_WAIT_MIN       -25.0
#define HOIST_ABNOMARL_MOVE_SPEED_LIMIT_MAX    0.5
#define SHIFT_ABNOMARL_MOVE_SPEED_LIMIT_MAX    0.5
#define HOIST_MOVELIMIT_P_MAX                  6000 //KYH Test 3500
#define HOIST_MOVELIMIT_N_MIN                  -1.0
#define SHIFT_MOVELIMIT_P_MAX                  550
#define SHIFT_MOVELIMIT_N_MIN                  -550
#define DIST_HOIST_HOME_MARGIN_MAX             2.0
#define DIST_SHIFT_HOME_MARGIN_MAX             3.0


#define DEF_nUpstate_SpeedDown_Offset_MAX      200
#define DEF_nDnState_UP_SpeedDown_Offset_MAX   200
#define DEF_nDnState_DN_SpeedDown_Offset_MAX   200
#define DEF_nDnState_UP_SpeedDown_Offset_SpecialEQ_MAX   400

#define LOOKDN_DETECT_TIMEOVER_MAX             8500
#define TRANS_TIMEOVER_MAX                     70000
#define OSCILLATIONCHECKDIST_MAX               20
#define HOIST_MOVINPOS_LIMIT_MAX               30
#define SHIFT_FAST_MOVE_DIST_MAX               100
#define OSCILLATION_IGNORE_DIST_MAX            300.0
#define IS_STB_LIMIT_MAX                       250.0
#define IS_STK_LIMIT_MAX                       1500.0
#define IS_FAST_MOVE_LIMIT_MAX                 2500.0
#define STBUTBSensorOFFTime_MAX                3300.0
#define STBUTBSensorONTime_MAX                 3300.0
//#define RAILSUPPORT_HOMING_TIMEOUT_MAX         12000.0
#define SHIFT_SPEED_RATIO_MAX                  3.0
#define SHIFT_ACCTIME_MAX                      750
#if(OHT_NAME == OHT_NAME_STD_V85)
#define HOIST_MAX_SPEED_MAX                    2.75
#else
#define HOIST_MAX_SPEED_MAX                    2.67
#endif
#define HOIST_POS_LIMIT_MAX                    6000.0
#define HOIST_NEG_LIMIT_MIN                    -3.0
#define SHIFT_POS_LIMIT_MAX                    500.0
#define SHIFT_NEG_LIMIT_MIN                    -500
#define INPOS_LIMIT_MAX                        2.3
#define SHIFT_POS_LIMIT_FORWAIT_MAX            25.0
#define SHIFT_NEG_LIMIT_FORWAIT_MIN            -25.0
#define HOIST_POS_LIMIT_TRANSOFFSET_MAX        35.0
#define HOIST_ORIGIN_LIMIT_MAX                 3.0

// by zzang9un 2013.08.30 : Data 범위 체크 값
#define MIN_PIOTIMELEVEL            1
#define MAX_PIOTIMELEVEL            10

// 2016-11-28, Add by Labeler, Rotate 관련 변수 추가
#define ROTATE_EXEC_TIMEOUT         6000
#define ROTATE_EXEC_TIMEOUT_MAX     10000

// 로그 작성 함수
#define MAX_BYTE_SIZE_OF_TRANS_LOG  1024*10
#define ADD_LOG(...)    WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)
#define RESET_LOG       CLEAR_LOG(pLogUnit)
//#define ADD_ERR_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\AbnormalError.log", __VA_ARGS__)
#define ADD_ERR_LOG(...)      WRITE_LOG_UNIT(pLogAbnormal, __VA_ARGS__)
#define ADD_MD_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\MonitoringDetail.log", __VA_ARGS__)  //AOHC-253 Monitoring 항목 추가 2
#define ADD_MCC_LOG(...)      WRITE_LOG_UNIT(pLogMCC, __VA_ARGS__)
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)
#define SEND_WARN(...)				writeFDCUnit_WARN(pFDCUnit, __VA_ARGS__)

#define HOIST_SMALL_ADD_HANDPUSHIO    		 6
#define HOIST_SMALL_ADD_HANDPUSH_DISTANCE    2.0


#if(OHT_NAME == OHT_NAME_STD_V85)      //임시
#define HOIST_SMALL_ADD_HOISTHOME    	76
#define HOIST_SMALL_ADD_HOISTHOME_DISTANCE	6.0
#else
#define HOIST_SMALL_ADD_HOISTHOME		74
#define HOIST_SMALL_ADD_HOISTHOME_DISTANCE	6.0
#endif


#define HOIST_SMALL_ADD_HOISTHOME_LIMIT	-4.0


#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
#define MOVETP_RETRYCOUNT_DISTANCE         	 40
#else
#define MOVETP_RETRYCOUNT_DISTANCE         	 30
#endif

#define FOUP_DETECT_CNT 2
#define FOUP_DETECT_TIME 20

// ---------------------------------------------------------------------------
// Content : EUV Special Define value
// ---------------------------------------------------------------------------
#define EUV_FIRST_ROTATION_POSITION	10.0

//HJH : Teaching Auto Adjustment

void TransControl::Cal_HoistOffset_Load(double dCurrentHoistPosition, double dCmdHoistPosition, double dCmdHoistPositionOriginal, int nStationNumber)
{
	double dOffset = dCmdHoistPosition - dCurrentHoistPosition;
	double dCurSpeed = fabs(m_pHWSet->m_pHoistAxis->GetCurrentVelocity());

	if((dOffset >= -1.0) && (dOffset <= 1.0))
	{

	}
	else
	{
		if((dOffset >= -30.0) && (dOffset <= 25.0))
		{
			if(m_Hoist_TeachingData_Load[nStationNumber].Hoist_Teaching_Offset_Count >= Offset_Data_CNT)
			{
				m_Hoist_TeachingData_Load[nStationNumber].Hoist_Teaching_Offset_Count = 0;
				ADD_MD_LOG("Station: %d, Hoist_Teaching_Offset_Count: %lf",
				nStationNumber,
				m_Hoist_TeachingData_Load[nStationNumber].Hoist_Teaching_Offset_Count);

				for(int i = 0; i <Offset_Data_CNT; i++)
				{
					m_Hoist_TeachingData_Load[m_StationID].Hoist_Teaching_Offset[i] = 0.0;
				}

			}
			m_Hoist_TeachingData_Load[nStationNumber].Hoist_Teaching_Offset_Count++;
			m_Hoist_TeachingData_Load[nStationNumber].Hoist_Teaching_Offset[m_Hoist_TeachingData_Load[nStationNumber].Hoist_Teaching_Offset_Count-1] = dCmdHoistPositionOriginal - dCurrentHoistPosition;
			m_Hoist_TeachingData_Load[nStationNumber].Original_Hoist_Teaching_Point = dCmdHoistPositionOriginal;

			ADD_MD_LOG("[Load] station: %d , offset: %lf - %lf = %lf , count: %d",
			nStationNumber, dCmdHoistPosition, dCurrentHoistPosition, dOffset,	m_Hoist_TeachingData_Load[nStationNumber].Hoist_Teaching_Offset_Count);
			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_LOAD_TEACHING_ABNORMAL_CNT);

		}
	}
}


void TransControl::Cal_HoistOffset_Unload(double dCurrentHoistPosition, double dCmdHoistPosition, double dCmdHoistPositionOriginal, int nStationNumber)
{
	double dOffset = dCmdHoistPosition - dCurrentHoistPosition;
	double dCurSpeed = fabs(m_pHWSet->m_pHoistAxis->GetCurrentVelocity());

	if((dOffset >= -1.0) && (dOffset <= 1.0))
	{

	}
	else
	{
		if((dOffset >= -30.0) && (dOffset <= 25.0))
		{
			if(m_Hoist_TeachingData_Unload[nStationNumber].Hoist_Teaching_Offset_Count >= Offset_Data_CNT)
			{
				m_Hoist_TeachingData_Unload[nStationNumber].Hoist_Teaching_Offset_Count = 0;
				ADD_MD_LOG("Station: %d, Hoist_Teaching_Offset_Count: %lf", nStationNumber, m_Hoist_TeachingData_Unload[nStationNumber].Hoist_Teaching_Offset_Count);

				for(int i = 0; i <Offset_Data_CNT; i++)
				{
					m_Hoist_TeachingData_Unload[m_StationID].Hoist_Teaching_Offset[i] = 0.0;
				}
			}
			m_Hoist_TeachingData_Unload[nStationNumber].Hoist_Teaching_Offset_Count++;
			m_Hoist_TeachingData_Unload[nStationNumber].Hoist_Teaching_Offset[m_Hoist_TeachingData_Unload[nStationNumber].Hoist_Teaching_Offset_Count-1] = dCmdHoistPositionOriginal - dCurrentHoistPosition;
			m_Hoist_TeachingData_Unload[nStationNumber].Original_Hoist_Teaching_Point = dCmdHoistPositionOriginal;

			ADD_MD_LOG("[Unload] station: %d , offset: %lf - %lf = %lf , count: %d",
			nStationNumber, dCmdHoistPosition, dCurrentHoistPosition, dOffset,	m_Hoist_TeachingData_Unload[nStationNumber].Hoist_Teaching_Offset_Count);
			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_UNLOAD_TEACHING_ABNORMAL_CNT);

		}
	}
}




/**
 @brief   TransControl Class 생성자
 @author  LSJ
 @date    2012.11.07
 @param   pHWSet : HWSet Class의 포인터
 */
TransControl::TransControl()
{
	pLogUnit = getLogUnit("D:\\log\\Trans.log", MAX_BYTE_SIZE_OF_TRANS_LOG);
	pLogAbnormal = getLogUnit("D:\\log\\AbnormalError.log", MAX_BYTE_SIZE_OF_TRANS_LOG);
	pLogMCC = getLogUnit("D:\\log\\MCC.log", MAX_BYTE_SIZE_OF_TRANS_LOG);
	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	m_pHWSet = OHTMainForm->GetHWSet();
	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_defualtparam = OHTMainForm->GetDefaultParam();

	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();

	//LimJY_20160122 : For IR+RF PIO
    // 혼용 PIO Sensor 사용으로 sTransInfo.TransCmd.PIODirection을 Type 지정정보로 사용됨.
    // PIO Sensor가 Left에 하나만 장착되므로 생성시 지정함.
	// Comport Open을 위해 우선 지정.
	m_pEQPIO = m_pHWSet->m_pLeftEQPIO;

    m_bSlipringFlag = false;
	m_nTransSound = 0;
	m_nTransHoistOriginCount =0;
	m_bPIOAutoRecovery = false;
	m_pTmpParamSet = NULL;
	m_ParamChehckError = NO_ERR;

	m_pParamSet = new TRANS_PARAM_SET();
	memset(m_pParamSet, 0x00, sizeof(TRANS_PARAM_SET));
	loadParameterRecovery("DataFiles\\Trans.ini", m_pParamSet); // m_ParamSet

	// 파라미터를 읽어서 각 축 기어비를 설정: 임태웅 2013.7.25
	m_pHWSet->m_pHoistAxis->SetHoistParam(m_defualtparam->HoistDefault);
	m_pHWSet->m_pShiftAxis->SetGearRatio(m_defualtparam->ShiftDefault);

	TransInit();

	//Auto Teaching 생성.
	m_AutoTeaching = new AutoTeaching();


	m_pHWCommon         = OHTMainForm->GetHWCommon();

    // 2017-01-13, Add by Labeler, DrivingControl Class 접근용 변수 추가
    m_pVirtualDrivingControl    = OHTMainForm->GetDrivingControl();

	CheckCarrierInterlock();

	bAutoModeSkip = false;

	m_bHoistCheck = false;

	m_DiagManager = DiagManager::getInstance();

	m_MoniStationID=0;

	//HJH : Teaching Auto Adjustment
    int cnt = 0;
	for(int i = 0; i < 1000000; i++)
	{
		for(int j = 0; j < Offset_Data_CNT; j++)
		{
			m_Hoist_TeachingData_Load[i].Hoist_Teaching_Offset[j] = 0.0;
			m_Hoist_TeachingData_Unload[i].Hoist_Teaching_Offset[j] = 0.0;
		}
		m_Hoist_TeachingData_Load[i].Final_Hoist_Teaching_Offset = 0.0;
		m_Hoist_TeachingData_Load[i].Original_Hoist_Teaching_Point = 0.0;
		m_Hoist_TeachingData_Load[i].Hoist_Teaching_Offset_Count = 0;
		m_Hoist_TeachingData_Unload[i].Final_Hoist_Teaching_Offset = 0.0;
		m_Hoist_TeachingData_Unload[i].Original_Hoist_Teaching_Point = 0.0;
		m_Hoist_TeachingData_Unload[i].Hoist_Teaching_Offset_Count = 0;


	}

	m_bFDCHandPushFlag = true;
	m_bFDCHandAlarmFlag = true;
	m_bFDCOcsillationFlag = true;
	HoistCnt = OHTMainForm->nLongRunCount;
	bHoistJig = false;
	nHoistTeaching = 3023;
}
// ---------------------------------------------------------------------------

/**
 @brief   TransControl Class 소멸자
 @author  LSJ
 @date    2012.11.07
 */
TransControl::~TransControl()
{
	// delete pLogUnit;

	Stop();
//#if(  (OHT_NAME == OHT_NAME_STD))
//		m_pHWSet->m_pHoistAxis->HoistExternalBrakeReleaseOnOFF(false);
//#endif

//    saveParameter("DataFiles\\Trans.ini", m_pParamSet);
    delete m_pParamSet;
}
// ---------------------------------------------------------------------------

/**
 @brief   TransControl Class에서 사용하는 변수들 초기화하고 ini파일에서 관련 셋팅 값을 읽어들임.
 @author  LSJ
 @date    2012.11.08
 @return  문제 없이 수행하면 0, 문제가 발생하면 해당 에러코드를 리턴함.
 */
int TransControl::TransInit()
{
    int nError = NO_ERR;

    // Parameter로 저장된 Use값을 먼저 설정한다.
    m_TransUse = m_pParamSet->TransUsePart;


	m_nTransStep = 0;
	m_nTransStepHoistRecovery = 0;
	m_SetHomenCount=0;
    m_nTransType = TRANSTYPE_NONE;
    m_nEqType = 0;
    m_nSTBDirection = 0; // 0:EQ,STK 1:RIGHT 2:LEFT
    m_nPIOTimeoverLevel = 0;

	m_dwLREQ_STTime = 0; // Start Time
    m_dwLookdn_STTime = 0;
	m_dwTotalTrans_STTime = 0; // 총 trans time

	m_dHoist_TeachingPos = 0.0; // 티칭 값
	//HJH : Teaching Auto Adjustment
	m_dHoist_TeachingPos_Original = 0.0;
	m_dShift_TeachingPos = 0.0;
    m_dRotate_TeachingPos = 0.0;
	m_nRotate_Pos = 0;

    m_dCmdHoistPos = 0.0; // 실제 명령을 준 값
	m_dCmdHoistStartPos = 0.0;
    m_dCmdHoistSpeed = 0.0;
	m_dCmdFinal_HoistPos = 0.0; // 실제 Hoist의 티칭 명령값..
    m_dCmdShiftPos = 0.0;
    m_dCmdRotatePos = 0.0;

    // 속도 변경관련
	m_dHoistLevel = 1.0;
	m_dShiftLevel = 1.0;

	//IR+RF 혼용 PIO 관련
    m_nPIOType = 0;     //LimJY_20160122 : For IR+RF PIO (0:IR, 1:RF, 2:Hybrid 5.0GHz)
	m_nPIOID = 0;
	m_nPIOCS = 0;
	m_nPIOChannel = 0;

    // Lookdown 관련 처리
	bLookdown_Detected = false;

	//오실레이션관련처리
	m_bOSL_Detected  =false;
	m_bOSL_Detected_Check = false;
	m_dwOSL_STTime = timeGetTime();
	m_dwOSL_CheckTime = timeGetTime();

	m_IsCheckPause = false;

	m_bHandPIOGoSignal_Detected = false;
	m_dwHandPIOGoSignal_CheckTime = timeGetTime();
	nHandPIOGosignalPauseCount = 0;

	m_nErrCount_GripAbNormalON = 0;
	m_nErrCount_GripAbNormalOFF = 0;

	m_nErrCount_HandOpenAbNormalON = 0;
	m_nErrCount_HandCloseAbNormalON= 0;

// 2016-12-05, Add by Labeler, Check_ShiftRotate 관련 변수 초기화
    nStep_SR_Check = 0;
    dwMoveSRStartTime = 0;

    m_bIsSTBTypeTrans = false;
	m_bIsSTKTypeTrans = false;

	//특수 Port 적용(Fosb 전용 Port - 간섭으로 적용)
	bCheck_SpecialEQ = false;

	m_bTransHoistOriginRecovery = false;
	m_bTransHoistRecovery = false;

    m_StatusSet.bIsOnProcess = false;
    m_StatusSet.byStatus = STS_INIT;

    if(m_pHWSet->m_pShiftAxis->IsAMPEnable() == false)
    {
        m_pHWSet->m_pShiftAxis->SetAMPEnable(true);
    }
    if(m_pHWSet->m_pHoistAxis->IsAMPEnable() == false)
	{
		m_pHWSet->m_pHoistAxis->SetAMPEnable(true);
    }

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
    if(m_pHWSet->m_pRotate->IsAMPEnable() == false)
    {
        m_pHWSet->m_pRotate->Enable();
    }
#endif


#if(  (OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
		m_pHWSet->m_pHoistAxis->HoistExternalBrakeReleaseOnOFF(true);
#endif


#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
    m_iCmdEuvRotateOrg = CMD_ROTATE_RESET;      // EUV POD Rotation Origin Command
    m_iStepEuvRotateOrg = STEP_ROTATE_NO_WORK;  // EUV POD Rotation Origin Run Step

    m_iCmdEuvRotateMov = CMD_EUV_ROT_RESET;     // EUV POD Rotation Move Command
    m_iStepEuvRotateMov = STEP_EUV_ROT_NO_WORK; // EUV POD Rotation Move Run Step
    m_iTargetPosEuvRotateMov = 0;
#endif

	m_mccparam.dHoist_TeachingPos = 0.0;
	m_mccparam.dShift_TeachingPos = 0.0;
	m_mccparam.nEqType = 0;
	m_mccparam.dHoist_Acc = 0.0;
	m_mccparam.dHoist_Dec = 0.0;
	m_mccparam.dHoist_Vel = 0.0;
	m_mccparam.dShift_Acc = 0.0;
	m_mccparam.dShift_Dec = 0.0;
	m_mccparam.dShift_Vel = 0.0;
	m_mccparam.dHoist_StartPos = 0.0;
	m_mccparam.dHoist_EndPos = 0.0;
	m_mccparam.dHoist_PushOnPos = 0.0;
	m_mccparam.nHandPush_RetryCount = 0;
	m_mccparam.nOscillation_Count = 0;

	m_mccparam.dwHoistDownTime = 0;
	m_mccparam.dwHoistUpTime = 0;

	m_mccparam.dHoist_HomeOffPos = 0.0;
	m_mccparam.dHoist_HomeOnPos = 0.0;
	m_mccparam.dShift_HomeOffPos = 0.0;
	m_mccparam.dShift_HomeOnPos = 0.0;
	m_mccparam.dFOUPCover_OffPos = 0.0;
	m_mccparam.dFOUPCover_OnPos = 0.0;
	m_mccparam.dFOUPDetect_OffPos = 0.0;
	m_mccparam.dFOUPDetect_OnPos = 0.0;
	m_mccparam.dLeftSTBDetect_OnPos = 0.0;
	m_mccparam.dLeftSTBDetect_OffPos = 0.0;
	m_mccparam.dRightSTBDetect_OnPos = 0.0;
	m_mccparam.dRightSTBDetect_OffPos = 0.0;

	m_bFDCHandPushFlag = true;
	m_bFDCHandAlarmFlag = true;
	m_bFDCOcsillationFlag = true;
 #if(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
	m_mccparam.bRotate_Moving = false;

	m_mccparam.bpreRotate0 = m_pHWCommon->m_Input.Rotate_0_Sensor;
	m_mccparam.bpreRotate180 = m_pHWCommon->m_Input.Rotate_180_Sensor;
	m_mccparam.nRotate0OnDegree = 0;
	m_mccparam.nRotate0OffDegree = 0;
	m_mccparam.nRotate180OnDegree = 0;
	m_mccparam.nRotate180OffDegree = 0;
	m_mccparam.nRotateMovingTime = 0;
 #endif
	m_mccparam.bpreShiftHomeSensor = m_pHWSet->m_pShiftAxis->IsShiftHome();
	m_mccparam.bpreFOUPCoverSensor = m_pHWSet->m_pFoupCoverOpenDetect->IsDetect();
	m_mccparam.bpreFOUPDetectSensor = m_pHWSet->m_pInnerFoup->IsFoupDetect();
	m_mccparam.bpreSTBLeftDetectSensor = m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect();
	m_mccparam.bpreSTBRightDetectSensor = m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect();

	m_mccparam.bpreHoistHomeSensor = m_pHWSet->m_pHoistAxis->IsHoistHome();

    return nError;
}
// ---------------------------------------------------------------------------

/**
 @brief   상위에서 받은 이적재 정보를 해당 변수에 담는다.
 @author  LSJ
 @date    2012.11.09
 */
EXECUTE_ACK TransControl::SetGeneralCmd(EXECUTE_TRANS_INFO sTransInfo)
{
	/*******************************************************/
	// Teaching 상태 확인 코드 (임시 Test)
	m_bHoistCheck = false;

	m_StationID = sTransInfo.TransCmd.CurStationID;
    /*******************************************************/

	m_nTransType = sTransInfo.TransType;
	m_nEqType = sTransInfo.TransCmd.PortType;

	m_bTransHoistRecovery = false;
    m_bTransHoistOriginRecovery = false;
	m_MoniStationID =0;

    ADD_LOG("SetGeneralCmd TransType: %d, EqType :%d", m_nTransType, m_nEqType);


    // STB 방향 설정
	switch(sTransInfo.TransCmd.PortType)
	{
	case PORT_TYPE_STB_L:
		m_nSTBDirection = STB_DIR_LEFT;
        break;

	case PORT_TYPE_STB_R:
		m_nSTBDirection = STB_DIR_RIGHT;
        break;

    default:
        m_nSTBDirection = STB_DIR_NONE;
        break;
	}

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.08.15 : 티칭 값 설정
    // by zzang9un 2013.09.10 : Hoist, Shift 단위 변경(1/1000m -> 1/10000m)
	//HJH : Teaching Auto Adjustment
	m_dHoist_TeachingPos = sTransInfo.TransCmd.Tposition_Hoist / 10.0;
	m_dHoist_TeachingPos_Original = sTransInfo.TransCmd.Tposition_Hoist / 10.0;
	m_dShift_TeachingPos = sTransInfo.TransCmd.Tposition_Shift / 10.0;
// 2016-12-05, Add by Labeler, ROHT의 경우 Rotate 관련 Position 정보 할당 추가

	m_mccparam.dHoist_TeachingPos = m_dHoist_TeachingPos;
	m_mccparam.dShift_TeachingPos = m_dShift_TeachingPos;
	m_mccparam.nEqType = m_nEqType;

	double dOffsetData = 0.0;

	if(bHoistJig == true)
	{
		m_nEqType = PORT_TYPE_EQ;
		m_dHoist_TeachingPos = nHoistTeaching;
		m_dHoist_TeachingPos_Original = nHoistTeaching;
		m_TransUse.EQPIO = false;
		m_bIsSTBTypeTrans = false;
    }


	//Load
	if(m_nTransType == 1)
	{
		if(m_dHoist_TeachingPos_Original == m_Hoist_TeachingData_Load[m_StationID].Original_Hoist_Teaching_Point)
		{
			if(m_Hoist_TeachingData_Load[m_StationID].Hoist_Teaching_Offset_Count >= Offset_Data_CNT)
			{

				for(int i = 0; i <Offset_Data_CNT; i++)
				{
					dOffsetData += m_Hoist_TeachingData_Load[m_StationID].Hoist_Teaching_Offset[i];
				}

				m_Hoist_TeachingData_Load[m_StationID].Final_Hoist_Teaching_Offset = dOffsetData / (double)Offset_Data_CNT;
			}

		}
		else
		{
			m_Hoist_TeachingData_Load[m_StationID].Hoist_Teaching_Offset_Count = 0;
			m_Hoist_TeachingData_Load[m_StationID].Final_Hoist_Teaching_Offset = 0.0;

			for(int i = 0; i <Offset_Data_CNT; i++)
			{
				m_Hoist_TeachingData_Load[m_StationID].Hoist_Teaching_Offset[i] = 0.0;
			}
			m_Hoist_TeachingData_Load[m_StationID].Original_Hoist_Teaching_Point = m_dHoist_TeachingPos;

		}
	}
	//Unload
	else if(m_nTransType == 2)
	{

		if(m_dHoist_TeachingPos_Original == m_Hoist_TeachingData_Unload[m_StationID].Original_Hoist_Teaching_Point)
		{
			if(m_Hoist_TeachingData_Unload[m_StationID].Hoist_Teaching_Offset_Count >= Offset_Data_CNT)
			{

				for(int i = 0; i <Offset_Data_CNT; i++)
				{
					dOffsetData += m_Hoist_TeachingData_Unload[m_StationID].Hoist_Teaching_Offset[i];
				}

				m_Hoist_TeachingData_Unload[m_StationID].Final_Hoist_Teaching_Offset = dOffsetData / (double)Offset_Data_CNT;
			}
		}
		else
		{
			m_Hoist_TeachingData_Unload[m_StationID].Hoist_Teaching_Offset_Count = 0;
			m_Hoist_TeachingData_Unload[m_StationID].Final_Hoist_Teaching_Offset = 0.0;

			for(int i = 0; i <Offset_Data_CNT; i++)
			{
				m_Hoist_TeachingData_Unload[m_StationID].Hoist_Teaching_Offset[i] = 0.0;
			}
			m_Hoist_TeachingData_Unload[m_StationID].Original_Hoist_Teaching_Point = m_dHoist_TeachingPos;

		}
    }



	if(m_Hoist_TeachingData_Load[m_StationID].Final_Hoist_Teaching_Offset != 0)
	{
		ADD_MD_LOG("[Load_5_Average]%lf - %lf = %lf", m_dHoist_TeachingPos_Original,m_Hoist_TeachingData_Load[m_StationID].Final_Hoist_Teaching_Offset,m_dHoist_TeachingPos);
	}
	if(m_Hoist_TeachingData_Unload[m_StationID].Final_Hoist_Teaching_Offset != 0)
	{
		ADD_MD_LOG("[Unload_5_Average]%lf - %lf = %lf", m_dHoist_TeachingPos_Original,m_Hoist_TeachingData_Unload[m_StationID].Final_Hoist_Teaching_Offset,m_dHoist_TeachingPos);
	}

	//m_dHoist_TeachingPos = m_dHoist_TeachingPos_Original - m_Hoist_TeachingData[m_StationID].Final_Hoist_Teaching_Offset;
	ADD_LOG("[Load]OffsetCnt[%d], Final_Hoist_Teaching_Offset[%lf][%lf/%lf/%lf/%lf/%lf], Hoist_TeachingPos[%lf]", m_Hoist_TeachingData_Load[m_StationID].Hoist_Teaching_Offset_Count, m_Hoist_TeachingData_Load[m_StationID].Final_Hoist_Teaching_Offset,
	m_Hoist_TeachingData_Load[m_StationID].Hoist_Teaching_Offset[0],
	m_Hoist_TeachingData_Load[m_StationID].Hoist_Teaching_Offset[1],
	m_Hoist_TeachingData_Load[m_StationID].Hoist_Teaching_Offset[2],
	m_Hoist_TeachingData_Load[m_StationID].Hoist_Teaching_Offset[3],
	m_Hoist_TeachingData_Load[m_StationID].Hoist_Teaching_Offset[4],
	m_dHoist_TeachingPos);

	ADD_LOG("[Unload]OffsetCnt[%d], Final_Hoist_Teaching_Offset[%lf][%lf/%lf/%lf/%lf/%lf], Hoist_TeachingPos[%lf]", m_Hoist_TeachingData_Unload[m_StationID].Hoist_Teaching_Offset_Count, m_Hoist_TeachingData_Unload[m_StationID].Final_Hoist_Teaching_Offset,
	m_Hoist_TeachingData_Unload[m_StationID].Hoist_Teaching_Offset[0],
	m_Hoist_TeachingData_Unload[m_StationID].Hoist_Teaching_Offset[1],
	m_Hoist_TeachingData_Unload[m_StationID].Hoist_Teaching_Offset[2],
	m_Hoist_TeachingData_Unload[m_StationID].Hoist_Teaching_Offset[3],
	m_Hoist_TeachingData_Unload[m_StationID].Hoist_Teaching_Offset[4],
	m_dHoist_TeachingPos);
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))

	if(sTransInfo.TransCmd.PortType ==  PORT_TYPE_STB_L)
	{
		m_StatusSet.uError = ERR_STB_L_LOCATION_FORMAT;
		ADD_LOG("EUV VHL STB JOB NG!");

	}else if(sTransInfo.TransCmd.PortType == PORT_TYPE_STB_R)
	{
		m_StatusSet.uError = ERR_STB_R_LOCATION_FORMAT;
		ADD_LOG("EUV VHL STB JOB NG!");
	}

	m_dRotate_TeachingPos = (double)sTransInfo.TransCmd.Tposition_Rotate;

	if(sTransInfo.TransCmd.Tposition_Rotate < 450.0)
	{
		ADD_LOG("m_dRotate_TeachingPos: %f Diff:%d",m_dRotate_TeachingPos,m_defualtparam->RotateDefault.nDeviceDifference);
		m_dRotate_TeachingPos += m_defualtparam->RotateDefault.nDeviceDifference;
	}
	else if(sTransInfo.TransCmd.Tposition_Rotate < 1350.0)
	{
		ADD_LOG("m_dRotate_TeachingPos: %f Diff:%d",m_dRotate_TeachingPos,m_defualtparam->RotateDefault.nDeviceDifference_90);
		m_dRotate_TeachingPos += m_defualtparam->RotateDefault.nDeviceDifference_90;
	}
	else if(sTransInfo.TransCmd.Tposition_Rotate < 1900.0)
	{
		ADD_LOG("m_dRotate_TeachingPos: %f Diff:%d",m_dRotate_TeachingPos,m_defualtparam->RotateDefault.nDeviceDifference_180);
        m_dRotate_TeachingPos += m_defualtparam->RotateDefault.nDeviceDifference_180;
	}

    // 2017-03-27, Reticle OHT Shift Teaching Position 위치 0.5 미만일 경우, 0.0으로 변경
    if(fabs(m_dShift_TeachingPos) < 0.5)
    {
        m_dShift_TeachingPos = 0.0;
	}

   	// Port별 Offset 적용  puting 2017.6.25//
	 RETICLE_TEACH_OFFSET pTempRiticleTeachingOffset;

	 if(Load_ReticleInTeaching(m_StationID,&pTempRiticleTeachingOffset))
	 {
		ADD_LOG("Reticle Offset Load Shift %f, Hoist %f, Rotate %d",
		pTempRiticleTeachingOffset.dPortOffset_Shift,pTempRiticleTeachingOffset.dPortOffset_Hoist,pTempRiticleTeachingOffset.nPortOffset_Rotate);

		m_dShift_TeachingPos += pTempRiticleTeachingOffset.dPortOffset_Shift;
		m_dHoist_TeachingPos += pTempRiticleTeachingOffset.dPortOffset_Hoist;
		m_dRotate_TeachingPos += pTempRiticleTeachingOffset.nPortOffset_Rotate;


	 }else
		ADD_LOG("Reticle Offset File Load fail!: %d:",m_StationID);

#endif

	if((m_nTransType == TRANSTYPE_LOAD)
	|| (m_nTransType== TRANSTYPE_UNLOAD)
	|| (m_nTransType== TRANSTYPE_MOVET)
	|| (m_nTransType== TRANSTYPE_AUTOTEACHING)
	|| (m_nTransType== TRANSTYPE_AUTOTEACHING_NULLTYPE))
	{
		if(m_nEqType == 0x00)
			m_StatusSet.uError  =  ERR_DATA_TEACHING_NOEXIST;
	}

    // 명령 값이 실행 가능 범위 이상일 경우
	if(m_dHoist_TeachingPos > HOIST_MOVELIMIT_P)
	{
        m_StatusSet.uError = ERR_HOIST_OUT_OF_P_LIMIT;
		ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
					ERR_HOIST_OUT_OF_P_LIMIT,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					m_dHoist_TeachingPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					HOIST_MOVELIMIT_P);
	}

    if(m_dHoist_TeachingPos < 0.0)
	{
		m_StatusSet.uError = ERR_HOIST_OUT_OF_N_LIMIT;
		ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
						ERR_HOIST_OUT_OF_N_LIMIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_dHoist_TeachingPos,
						m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
						HOIST_MOVELIMIT_N);
	}


	if(m_dShift_TeachingPos > SHIFT_MOVELIMIT_P)
		{
		m_StatusSet.uError = ERR_SHIFT_OUT_OF_P_LIMIT;
			ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
						ERR_SHIFT_OUT_OF_P_LIMIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_dShift_TeachingPos,
						m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
						SHIFT_MOVELIMIT_P);
		}

	if(m_dShift_TeachingPos < SHIFT_MOVELIMIT_N)
		{
		m_StatusSet.uError = ERR_SHIFT_OUT_OF_N_LIMIT;
                     ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
						ERR_SHIFT_OUT_OF_N_LIMIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_dShift_TeachingPos,
						m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
						SHIFT_MOVELIMIT_N);
		}



	//20180114 STB 낙하로 인해 Hoist Min Max값 Shift Min값 추가.  puting 추후에 파라미터로 빼서 관리 필요(기구측 기준값 정리이후, 다른포트타입 포함)
	if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
	{
		if(m_dHoist_TeachingPos > HOIST_MOVELIMIT_P_MAX_STB)
		{
			ADD_LOG("[SetGeneralCmd]SHIFT_OUT_OF_P_LIMIT");
			m_StatusSet.uError = ERR_HOIST_OUT_OF_P_LIMIT;
                        ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
					ERR_HOIST_OUT_OF_P_LIMIT,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					m_dHoist_TeachingPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					HOIST_MOVELIMIT_P);
        }


		if(m_dHoist_TeachingPos < HOIST_MOVELIMIT_N_MIN_STB)
		{
			ADD_LOG("[SetGeneralCmd]SHIFT_OUT_OF_N_LIMIT");
			m_StatusSet.uError = ERR_HOIST_OUT_OF_N_LIMIT;
                        ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
						ERR_HOIST_OUT_OF_N_LIMIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_dHoist_TeachingPos,
						m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
						HOIST_MOVELIMIT_N);
        }



		if(m_nEqType == PORT_TYPE_STB_R)
		{
			if(m_dShift_TeachingPos < SHIFT_MOVELIMIT_N_MIN_STB_R)
			{
				ADD_LOG("[SetGeneralCmd][STBR]SHIFT_OUT_OF_N_LIMIT");
				m_StatusSet.uError = ERR_SHIFT_OUT_OF_N_LIMIT;
            }

		}
		else
		{
			if(m_dShift_TeachingPos > SHIFT_MOVELIMIT_N_MIN_STB_L)
			{
				ADD_LOG("[SetGeneralCmd][STBL]SHIFT_OUT_OF_P_LIMIT");
				m_StatusSet.uError = ERR_SHIFT_OUT_OF_P_LIMIT;
			}
		}
	}

// 2016-12-05, Add by Labeler, ROHT의 경우 Rotate 관련 Position Limit Interlock 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
    if(m_dRotate_TeachingPos > (ROTATE_MOVELIMIT_P + 100))
        m_StatusSet.uError = ERR_ROTATE_LIMIT_ON_ERR;

	if(m_dRotate_TeachingPos < (ROTATE_MOVELIMIT_N - 100))
        m_StatusSet.uError = ERR_ROTATE_LIMIT_OFF_ERR;
#endif

    if(m_StatusSet.uError != NO_ERR)
    {
		m_StatusSet.byStatus = STS_ERROR;
        m_nTransType = TRANSTYPE_NONE;

        m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
        m_pHWSet->m_pShiftAxis->MoveStop(SHIFT_ESTOP_DECEL);
// 2016-12-05, Add by Labeler, ROHT의 경우 Rotate Stop 명령 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
        m_pHWSet->m_pRotate->Stop();
#endif
        return EXECUTE_ACK_ON_ERR;
	}
    // [END] by zzang9un 2013.08.15 : 티칭 값 설정
    // -------------------------------------------

    // -------------------------------------------
	// [BEGIN] by zzang9un 2013.08.15 : Lookdown 값 설정
	if((m_pParamSet->TransUsePart.LookdownSen == true) && (m_nEqType == PORT_TYPE_EQ || m_nEqType == PORT_TYPE_EQ_X))
	{
		m_TransUse.LookdownSen = true;
	}
	else
	{
		if((m_pExecuteInfo->ExecuteMode == EXECUTE_MODE_AUTO) && (m_nEqType == PORT_TYPE_EQ || m_nEqType == PORT_TYPE_EQ_X))
		{
			m_TransUse.LookdownSen = true;
			m_pParamSet->TransUsePart.LookdownSen = true;
		}
		else
		{
			m_TransUse.LookdownSen = false;
		}
	}

	if(m_TransUse.LookdownSen == true)
	{
		m_pHWSet->m_pLookdown->AreaSelect(sTransInfo.TransCmd.LookDownLevel);
	}		

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.08.26 : EQ PIO 설정
	if((m_pParamSet->TransUsePart.EQPIO == true) && (m_nEqType == PORT_TYPE_EQ || m_nEqType == PORT_TYPE_EQ_X || m_nEqType == PORT_TYPE_STOCKER))
	{
		// by zzang9un 2013.08.26 : TP에서 EQ PIO를 끄기 원하는 경우에만 false 처리
        if(sTransInfo.TransCmd.TransOption.Info.EQPIODisable_TP)
            m_TransUse.EQPIO = false;
        else
            m_TransUse.EQPIO = true;
    }
	else
    {
		if((m_pExecuteInfo->ExecuteMode == EXECUTE_MODE_AUTO) && (m_nEqType == PORT_TYPE_EQ || m_nEqType == PORT_TYPE_EQ_X || m_nEqType == PORT_TYPE_STOCKER))
		{
			m_TransUse.EQPIO = true;
			m_pParamSet->TransUsePart.EQPIO = true;

		}			
		else
		{
	        m_TransUse.EQPIO = false;
		}			
		
	}

    // [END] by zzang9un 2013.08.26 : EQ PIO 설정
    // -------------------------------------------

    // Oscillation 설정(센서 Use 설정이 true인 경우 Command 값에 따르도록 수정)
    if(m_pParamSet->TransUsePart.OscillationSen == true)
   	{
        m_TransUse.OscillationSen = true;  //8.0에서는 무조건 파라미터에 의존
   	}		
    else
   	{
		if(m_pExecuteInfo->ExecuteMode == EXECUTE_MODE_AUTO)
		{
			m_TransUse.OscillationSen = true;
			m_pParamSet->TransUsePart.OscillationSen = true;
		}			
		else
		{
			m_TransUse.OscillationSen = false;
			m_pParamSet->TransUsePart.OscillationSen = false;
		}
   	}		


	//Specail EQ Port 적용 - FOSB 적용으로 간섭발생으로 적용함 2018 09 12

	if((m_nEqType == PORT_TYPE_EQ || m_nEqType == PORT_TYPE_EQ_X) &&(m_dHoist_TeachingPos >  1700))
		bCheck_SpecialEQ  =  Load_SpecialEQParameter(m_StationID, m_pParamSet);
	else
        bCheck_SpecialEQ = false;


    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.08.15 : Hoist, Shift Level 설정
    if(sTransInfo.TransCmd.HoistSpeedLevel > 100)
        sTransInfo.TransCmd.HoistSpeedLevel = 100; // Test
    if(sTransInfo.TransCmd.HoistSpeedLevel < 10)
        sTransInfo.TransCmd.HoistSpeedLevel = 10; // Test
    if(sTransInfo.TransCmd.ShiftSpeedLevel > 100)
        sTransInfo.TransCmd.ShiftSpeedLevel = 100; // Test
    if(sTransInfo.TransCmd.ShiftSpeedLevel < 10)
        sTransInfo.TransCmd.ShiftSpeedLevel = 10; // Test

    m_dHoistLevel = (sTransInfo.TransCmd.HoistSpeedLevel) / 100.0; // Test
    m_dShiftLevel = (sTransInfo.TransCmd.ShiftSpeedLevel) / 100.0; // TEST
    // [END] by zzang9un 2013.08.15 : Hoist, Shift Level 설정
    // -------------------------------------------

	// PIO Auto Recovery 설정
	// m_bPIOAutoRecovery = sTransInfo.TransCmd.TransOption.Info.AutoRecovery;
    if(sTransInfo.TransCmd.TransOption.Info.AutoRecovery == true)
		m_bPIOAutoRecovery = true;
    else
        m_bPIOAutoRecovery = false;

    // UTB 센서 설정
	if((m_pParamSet->TransUsePart.UTBFoupCheck == true) && (m_nEqType == PORT_TYPE_UTB))
        m_TransUse.UTBFoupCheck = true;
    else
        m_TransUse.UTBFoupCheck = false;

    // PIO Time Level 설정(1~10)
    // by zzang9un 2013.09.11 : 배열 인덱스로 변경하기 위해 1을 뺌
    if(sTransInfo.TransCmd.PIOTimeLevel < MIN_PIOTIMELEVEL)
        m_nPIOTimeoverLevel = MIN_PIOTIMELEVEL - 1;
	else if(sTransInfo.TransCmd.PIOTimeLevel > MAX_PIOTIMELEVEL)
        m_nPIOTimeoverLevel = MAX_PIOTIMELEVEL - 1;
    else
		m_nPIOTimeoverLevel = sTransInfo.TransCmd.PIOTimeLevel - 1;
//혼용 PIO
	if(m_TransUse.EQPIO == true)
    {
		m_nPIOID = sTransInfo.TransCmd.PIOID;

        //LimJY_20160122 : For IR+RF PIO (0:IR, 1:RF, 2:Hybrid 5.0GHz)
		String strPIOCha = "";
        UINT nPIOCha = 0;
		strPIOCha.sprintf(L"%X", m_nPIOID);
		nPIOCha = m_nPIOID % 100;

        if(m_nPIOID > 0)
        {
            if(OHTMainForm->Get_EQPIO_Type() == TYPE_PIO_HYBRID)
            {
				m_nPIOType = TYPE_PIO_HYBRID;

				if(sTransInfo.TransCmd.PIOChannel ==0)
				{
					m_nPIOChannel = nPIOCha % 40;   // RF Type PIO Channel Range 0 ~ 39 (40)
					m_nPIOChannel = EQPIO_BASE_CHANNEL + m_nPIOChannel;
				}
				else if(sTransInfo.TransCmd.PIOChannel >= EQPIO_BASE_CHANNEL && sTransInfo.TransCmd.PIOChannel < (EQPIO_BASE_CHANNEL+40) )
				{
					m_nPIOChannel = sTransInfo.TransCmd.PIOChannel;
				}
				else  //사용하지 않는 채널 번호가 적힌 경우
				{
					m_nPIOChannel = nPIOCha % 40;
					m_nPIOChannel = EQPIO_BASE_CHANNEL + m_nPIOChannel;
				}


				ADD_LOG("type: %d, ID: %d, Channel: %d ",m_nPIOType, m_nPIOID, m_nPIOChannel);
            }
            else if(OHTMainForm->Get_EQPIO_Type() == TYPE_PIO_RF)
            {
                m_nPIOType = TYPE_PIO_RF;
                if(nPIOCha > 80)
                {
					m_nPIOChannel = nPIOCha - 80;
				}
                else
                {
                    m_nPIOChannel = nPIOCha;
                }
            }
            else
			{
                m_nPIOType = TYPE_PIO_IR;
            }
        }
        else
        {
            m_nPIOType = TYPE_PIO_IR;
        }

        m_nPIOCS = sTransInfo.TransCmd.PIOCS;
    }


	if(OHTMainForm->Get_EQPIO_Type() == TYPE_PIO_IR)
	{
        // PIO 방향 설정
//        if(sTransInfo.TransCmd.PIODirection == LEFT_SIDE)
		   //	m_pEQPIO = m_pHWSet->m_pLeftEQPIO;
//		else
//			m_pEQPIO = m_pHWSet->m_pRightEQPIO;
	}

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
    m_bIsSTBTypeTrans = false;
#else
    // STB 이적재 설정
    if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (fabs(m_dHoist_TeachingPos) < fabs((double)IS_STB_LIMIT)))
        m_bIsSTBTypeTrans = true;
    else
        m_bIsSTBTypeTrans = false;
#endif

    // Stocker 설정
    m_bIsSTKTypeTrans = sTransInfo.TransCmd.TransOption.Info.HandDetectEQ;

	if(m_bIsSTKTypeTrans == false)
    {
        if((m_nEqType == PORT_TYPE_EQ) && (fabs(m_dHoist_TeachingPos) < fabs((double)IS_STK_LIMIT)))
            m_bIsSTKTypeTrans = true;
        else
            m_bIsSTKTypeTrans = false;
    }

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.08.15 : 이적재 사운드 설정
	m_nTransSound = m_pParamSet->TransUsePart.TransSound;


    if(m_TransUse.TransSound == true)
    {
		switch(m_nTransSound)
        {
        case 0:
            break;
		case 1:
			if((m_nTransType == TRANSTYPE_LOAD) || (m_nTransType ==TRANSTYPE_UNLOAD))
            	m_pHWSet->Sound_On(SETSOUND_TRANS);
            break;
		case 2:
			if((m_nTransType == TRANSTYPE_LOAD) || (m_nTransType ==TRANSTYPE_UNLOAD))
			{
				if(m_nEqType == PORT_TYPE_EQ)
                	m_pHWSet->Sound_On(SETSOUND_TRANS);
            }
            break;
		}
    }
    // [END] by zzang9un 2013.08.15 : 이적재 사운드 설정
    // -------------------------------------------

	if(m_TransUse.LookdownSen == true)
	{
		if(m_pHWSet->m_pLookdown->IsError() == true)
		{
		   m_pHWSet->m_pLookdown->PowerControl(PWR_RST);
		}
	}
#if( (OHT_NAME == OHT_NAME_STD_V81S) ||(OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V80))
	if(m_nTransType == TRANSTYPE_UNLOAD)
	{

		if((m_nTransHoistOriginCount > 2) && (m_defualtparam->bHoistHomeSensorOriginRetry ==true))
		{
			m_pHWSet->m_pHoistAxis->SetNegativeSWLimit(HOIST_NEG_LIMIT-5);
			m_nTransStep = -1;
		}

		m_nTransHoistOriginCount = 0;  // Load/ Unload마다 클리어처리.  ?? 저네 누적으로 할지??
	}

#endif

	ADD_LOG(
		"[SetGenCmd] H:%05.2lf, S:%05.2lf, R: %05.21f, E:%d, LD:%d, PIO:%d, Osc:%d, HLev:%f, SLev:%f, PIODir:%d, PIOTime:%d, AR:%d StationID:%d",
		 m_dHoist_TeachingPos, m_dShift_TeachingPos, m_dRotate_TeachingPos, m_StatusSet.uError, m_TransUse.LookdownSen, m_TransUse.EQPIO, m_TransUse.OscillationSen,
		 m_dHoistLevel, m_dShiftLevel, sTransInfo.TransCmd.PIODirection, m_nPIOTimeoverLevel, m_bPIOAutoRecovery,m_StationID);

	// Status 변경
	switch(m_nTransType)
	{
	case TRANSTYPE_LOAD:
		m_StatusSet.byStatus = STS_LOADING;
//		TransCMDExuteFunc(false);
		ADD_LOG("TRANSTYPE_LOAD");
        break;
	case TRANSTYPE_UNLOAD:
		ADD_LOG("STS_UNLOADING");
		m_StatusSet.byStatus = STS_UNLOADING;
//		TransCMDExuteFunc(false);
		break;
    case TRANSTYPE_MOVET:
		m_StatusSet.byStatus = STS_RUNNING;
		break;
	case TRANSTYPE_AUTOSET_HOME:
		m_pHWSet->m_pHoistAxis->SetNegativeSWLimit(HOIST_NEG_LIMIT-5);
		m_StatusSet.byStatus = STS_AUTO_TRANS_HOMING;
		break;

	case TRANSTYPE_MOVE_ALL_HOME:
		m_StatusSet.byStatus = STS_AUTO_TRANS_HOMING;
		break;

	case TRANSTYPE_AUTOTEACHING:
		m_StatusSet.byStatus = STS_AUTO_TEACHING;
		break;

	case TRANSTYPE_AUTOTEACHING_NULLTYPE:
		m_StatusSet.byStatus = STS_AUTO_TEACHING_NULLTYPE;
		break;

	case TRANSTYPE_AUTO_CHANGE:
  		m_StatusSet.byStatus = STS_AUTO_TRANS_HOMING;
		break;
	}




	m_pExecuteInfo->bPopListOfCmd = false;
	ADD_LOG("bPopListOfCmd");
    return EXECUTE_ACK_NO_ERR;
}

void TransControl::TransCMDExuteFunc(bool bFlag)
{
	 m_pExecuteInfo->FlagCmdExcute.bCmdTransCMDExcute = bFlag;
}
/**
 @brief   Load 시퀀스 명령 수행(설비에 반송물을 내려 놓는 작업)
 @author  LSJ
 @date    2012.11.07
 @return  Load 명령을 문제 없이 수행하면 0, 문제가 발생하면 해당 에러코드를 리턴함.
 */
int TransControl::Cmd_Load()
{
	m_TransUse.EQPIO = false;
	int nError = 0;
	DWORD dwStTime = 0;
	DWORD dwPIOWaitTime = 0;

	static DWORD dwTackTime = timeGetTime();
	static DWORD dwHandTackTime = timeGetTime();
	static DWORD dwBeforePIOTime = timeGetTime();
	static DWORD dwAfterPIOTime = timeGetTime();
	SYSTEMTIME CurTime;
	GetLocalTime(&CurTime);

	if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_nTransStep > STEP_L_EQPIO_READY)) //&& (m_pEQPIO->IsValidOn() == true))
	{
		nError = Check_EQStatus(TRANSTYPE_LOAD, m_nTransStep);
	}

	if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_nTransStep > STEP_L_EQPIO_READY) && (m_nTransStep < STEP_L_MOVE_DOWN_HOIST_SLOW))// && (m_pEQPIO->IsValidOn() == true))
	{
		nError = Check_L_U_REQStatus_Down(TRANSTYPE_LOAD, m_nTransStep);
   	}		

	if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_nTransStep > STEP_L_CHECK_MOVE_UP_HOIST_SLOW) && (m_nTransStep < STEP_L_PIO_COMPLETE))// && (m_pEQPIO->IsValidOn() == true))
   	{
        nError = Check_L_U_REQStatus_Up(TRANSTYPE_LOAD, m_nTransStep);
   	}		

    if((nError == NO_ERR) && (m_nTransStep > STEP_L_EQPIO_READY) && (m_dwTotalTrans_STTime != 0))
    {
        // ** 2축원점 명령 후 간섭으로 인해 원점복귀가 30초동안 안될경우 에러..발생.
        // ** Lookdown Timeover 8초 포함
        // ** PIO 사용할경우 Ready off wait time 포함
		if(m_TransUse.EQPIO == true)
        {
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwTotalTrans_STTime) >
				(DWORD)(m_pParamSet->TransDefine.nTrans_Timeover))//+m_pEQPIO->GetReadyOffTimeover(m_nPIOTimeoverLevel)))
			{
				nError = ERR_TRANS_AXIS2HOME_TIMEOVER;
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%f/%f",
							ERR_TRANS_AXIS2HOME_TIMEOVER,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_dCmdHoistPos,
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
           	}
        }
        else
        {
            if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwTotalTrans_STTime) > (DWORD)m_pParamSet->TransDefine.nTrans_Timeover)
           	{				
				nError = ERR_TRANS_AXIS2HOME_TIMEOVER;
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%f/%f",
							ERR_TRANS_AXIS2HOME_TIMEOVER,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_dCmdHoistPos,
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
           	}
        }
    }

    switch(m_nTransStep)
	{
	case STEP_L_EQPIO_CONNECT:
		dwTackTime = timeGetTime();
        if((nError == NO_ERR) && (m_TransUse.EQPIO == true))
        {
            if(OHTMainForm->Get_EQPIO_Type() > TYPE_PIO_IR)
            {
				if(OHTMainForm->Get_EQPIO_Type() == TYPE_PIO_HYBRID)
                {
					//LimJY_20160119 : For IR+RF PIO m_nPIOType 인자 추가

					//nError = m_pEQPIO->Set_RFPIO_Data_RS232_Hybrid(m_nPIOType, m_nPIOID, m_nPIOChannel, m_nPIOCS, m_defualtparam->VHLName);

					//if(m_pEQPIO->GetRFPIOConnectStep()==8)
					//{
                        m_nTransStep++;
					//}
                }
                else
                {
                    // RF-PIO ID, Channel Setting
					//nError = m_pEQPIO->Set_RFPIO_Data_RS232_RF(m_nPIOID, m_nPIOChannel, m_nPIOCS);

					//if(m_pEQPIO->GetRFPIOConnectStep()==8)
					//{
                        m_nTransStep++;
					//}
                }
            }
            else
            {
                m_nTransStep++;
            }
        }
        else
        {
            m_nTransStep++;
		}

		if(m_nTransStep == STEP_L_LOAD_READY)
		{
			ADD_MCC_LOG(",%s,TRANS,LD READY,Start,FOUP,%06d,%d,%06d,%d,PIO ID,%d,PIO Channel,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_nPIOID, m_nPIOChannel);
		}
		break;

    case STEP_L_LOAD_READY:
        if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsCloseOutputOn() == true))
        {
            nError = m_pHWSet->m_pHand->IO_Init();
        }
        if(nError == NO_ERR)
        {
			nError = ReadyTrans();
        }
        if(nError == NO_ERR)
		{
			nError = Check_State();
		}
        // 2016-12-07, Add by Labeler
        // ROHT의 경우 Rotate Retry Logic 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
        if(nError == NO_ERR)
        {
            nError = rotateRetry();
        }
#endif
		if(nError == NO_ERR)
		{
#if((OHT_NAME != OHT_NAME_STD_V80_RETICLE) && (OHT_NAME != OHT_NAME_STD_V85))
		nError = Move_ShiftRotate(SHIFTROTATE_HOME2TP);
		if(nError != NO_ERR) break;
#endif
			m_nTransStep++;
            dwBeforePIOTime = timeGetTime();
       	}
        break;
		
	case STEP_L_EQPIO_READY:
        if((nError == NO_ERR) && (m_TransUse.EQPIO == true))
		{
			//nError = m_pEQPIO->SequenceBeforeLoad(m_nPIOTimeoverLevel);
        }
        if(nError == NO_ERR)
        {
            if(m_TransUse.EQPIO == true)
			{
				//if(m_pEQPIO->GetLoadPIOStep() == 14)
				//{
					m_nTransStep++;
				//}
			}
            else
			{
				m_nTransStep++;
			}
		}

		if(m_nTransStep == STEP_L_MOVE_SHIFTROTATE)
		{
			ADD_MCC_LOG(",%s,TRANS,LD READY,End,FOUP,%06d,%d,%06d,%d,Hoist P Limit,%6.1lf,Hoist N Limit,%6.1lf,Shift P Limit,%6.1lf,Shift N Limit,%6.1lf,Before Load PIO Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					HOIST_MOVELIMIT_P, HOIST_MOVELIMIT_N,
					SHIFT_MOVELIMIT_P, SHIFT_MOVELIMIT_N,
					OHTUTILITY::PreventOverflowGetTimeMs(dwBeforePIOTime));
		}
        break;
		
#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))

	case STEP_L_MOVE_PRE_SHIFT_BEFORE_DOWN:		// EUV Only
	{
#if(OHT_NAME == OHT_NAME_STD_V85)
        // EUV재하가 있고, Rotate 동작이 필요할 경우 10mm이동해야 함
		if((m_pHWCommon->m_Input.POD_EUV_1 == ON) &&
//				(m_pHWCommon->m_Input.POD_EUV_2 == ON) &&
				(fabs(m_dRotate_TeachingPos - m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > ROTATE_INPOS_LIMIT))
		{
//			nError = Move_EUV_Shift();

			if(nError == NO_ERR)
			{

				m_nTransStep++;
			}
		}
		else
		{
			m_nTransStep = STEP_L_MOVE_SHIFTROTATE;
		}
#else
//		if(m_pHWCommon->m_Input.POD_EUV_1 != m_pHWCommon->m_Input.POD_EUV_2)
//		{
//			nError = ERR_EUVROHT_DIFF_EUVSENSINGS;
//		}
		if((m_pHWCommon->m_Input.POD_EUV_1 == ON) &&
			(m_pHWCommon->m_Input.POD_EUV_2 == ON) &&
			(fabs(m_dRotate_TeachingPos - m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > ROTATE_INPOS_LIMIT))
		{
			//nError = Move_EUV_Shift();

			if(nError == NO_ERR)
			{

				m_nTransStep++;
			}
		}
		else
		{
			m_nTransStep = STEP_L_MOVE_SHIFTROTATE;
		}

#endif

	}
	break;

	// shift 10mm이동이 끝나기를 기다림 
	case STEP_L_CHECK_PRE_SHIFT_BEFORE_DOWN:	// EUV Only
	{
#if(OHT_NAME == OHT_NAME_STD_V85)
//		nError = Check_EUV_Shift();
#else
		nError = Check_EUV_Shift();
#endif

		if(nError == NO_ERR)
			m_nTransStep++;
	}
	break;

	case STEP_L_MOVE_PRE_ROTATE_BEFORE_DOWN:
	{
		nError = Move_EUV_Rotate(SHIFTROTATE_HOME2TP);

		if(nError == NO_ERR)
		{
			m_nTransStep++;
		}

	}
	break;

	case STEP_L_CHECK_PRE_ROTATE_BEFORE_DOWN:
	{
		nError = Check_EUV_Rotate();

		if(nError == NO_ERR)
			m_nTransStep++;
	}
	break;
#endif

	case STEP_L_MOVE_SHIFTROTATE:
        if(nError == NO_ERR)
		{
			//nError = Move_ShiftRotate(SHIFTROTATE_HOME2TP);
		}
        if(nError == NO_ERR)
		{
			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,LD MOVE SHIFTROTATE,Start,FOUP,%06d,%d,%06d,%d,Shift Acc,%2.5lf,Shift Dec,%2.5lf,Shift Vel,%6.1lf,Shift Cur Pos,%6.1lf,Shift Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dShift_Acc, m_mccparam.dShift_Dec,
					m_mccparam.dShift_Vel,
					m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
					m_mccparam.dShift_TeachingPos);
		}
        break;

	case STEP_L_CHECK_MOVE_SHIFTROTATE:
		if(nError == NO_ERR)
		{
			//nError = Check_ShiftRotate();
			m_nTransStep++;
		}
		if(m_nTransStep == STEP_L_MOVE_DOWN_HOIST_FAST)
		{
			ADD_MCC_LOG(",%s,TRANS,LD MOVE SHIFTROTATE,End,FOUP,%06d,%d,%06d,%d,Shift Cur Pos,%6.1lf,Shift Pos Err,%6.1lf,Shift Home Off Pos,%6.1lf,FOUP Cover On Pos,%6.1lf,FOUP Detect Off Pos,%6.1lf,Left STB Detect On Pos,%6.1lf,Right STB Detect On Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
					m_pHWSet->m_pShiftAxis->GetFollowingError(),
					m_mccparam.dShift_HomeOffPos,
					m_mccparam.dFOUPCover_OnPos,
					m_mccparam.dFOUPDetect_OffPos,
					m_mccparam.dLeftSTBDetect_OnPos,
					m_mccparam.dRightSTBDetect_OnPos);
		}
		break;

	case STEP_L_MOVE_DOWN_HOIST_FAST: // 2축 고속 이동 : teaching point-offset까지 이동
        if(nError == NO_ERR)
		{
			nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_L_MOVE_DOWN_HOIST_FAST, STEP_L_MOVE_DOWN_HOIST_FAST);
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected ==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_MOVE_DOWN_HOIST_FAST))
       	{
            break;
       	}
        if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Move_Hoist_Ver2(HOIST_DOWN_H2SDP);
#else
			nError = Move_Hoist(HOIST_DOWN_H2TO);
#endif
		}
		if(nError == NO_ERR)
		{
			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,LD MOVE DOWN HOIST FAST,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
			m_mccparam.dwHoistDownTime = timeGetTime();
       	}
		break;

	case STEP_L_CHECK_MOVE_DOWN_HOIST_FAST: // 2축 이동완료 확인: teaching point-offset
        if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_L_MOVE_DOWN_HOIST_FAST, STEP_L_MOVE_DOWN_HOIST_FAST_SLOWDOWN);
#else
		    nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_L_MOVE_DOWN_HOIST_FAST, STEP_L_MOVE_DOWN_HOIST_SLOW);
#endif
		}
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_CHECK_MOVE_DOWN_HOIST_FAST))
		{
			break;
		}
		
        if(nError == NO_ERR)
       	{
 #ifdef USE_TRANS_PROFILE_VER2
			nError = Check_Hoist_Ver2(HOIST_DOWN_H2SDP);
			if(m_nTransStep == STEP_L_MOVE_DOWN_HOIST_FAST_SLOWDOWN)
			{
				ADD_MCC_LOG(",%s,TRANS,LD MOVE DOWN HOIST FAST,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf,Hoist Home Off Pos,%6.1lf",
						m_defualtparam->VHLName,
						m_pOHTMainStatus->StatusCommon.CurStationID,
						m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
						m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
						m_pHWSet->m_pHoistAxis->GetCurrentVelocity(),
							m_mccparam.dHoist_HomeOffPos);
			}
#else
			nError = Check_Hoist(HOIST_DOWN_H2TO);
			if(m_nTransStep == STEP_L_MOVE_DOWN_HOIST_SLOW)
			{
				ADD_MCC_LOG(",%s,TRANS,LD MOVE DOWN HOIST FAST,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf,Hoist Home Off Pos,%6.1lf",
						m_defualtparam->VHLName,
						m_pOHTMainStatus->StatusCommon.CurStationID,
						m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
						m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
						m_pHWSet->m_pHoistAxis->GetCurrentVelocity(),
						m_mccparam.dHoist_HomeOffPos);
			}
#endif
		}

        break;
#ifdef USE_TRANS_PROFILE_VER2
 	case STEP_L_MOVE_DOWN_HOIST_FAST_SLOWDOWN: // 2축 고속 이동 : teaching point-offset까지 이동
        if(nError == NO_ERR)
        {
			nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_L_MOVE_DOWN_HOIST_FAST, STEP_L_MOVE_DOWN_HOIST_FAST_SLOWDOWN);
		}

		if((bLookdown_Detected == true) || (m_bOSL_Detected ==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_MOVE_DOWN_HOIST_FAST_SLOWDOWN))
		{
			break;
		}

        if(nError == NO_ERR)
		{
			nError = Move_Hoist_Ver2(HOIST_DOWN_SDP2TO);
		}
		if(nError == NO_ERR)
		{
			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,LD MOVE DOWN HOIST FAST SLOWDOWN,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
       	}
		break;

	case STEP_L_CHECK_MOVE_DOWN_HOIST_FAST_SLOWDOWN: // 2축 이동완료 확인: teaching point-offset
        if(nError == NO_ERR)
       	{
			nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_L_MOVE_DOWN_HOIST_FAST_SLOWDOWN, STEP_L_MOVE_DOWN_HOIST_SLOW);
		}
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_CHECK_MOVE_DOWN_HOIST_FAST_SLOWDOWN))
		{
			break;
		}
		
        if(nError == NO_ERR)
       	{
			nError = Check_Hoist_Ver2(HOIST_DOWN_SDP2TO);
		}

		if(m_nTransStep == STEP_L_MOVE_DOWN_HOIST_SLOW)
		{
			ADD_MCC_LOG(",%s,TRANS,LD MOVE DOWN HOIST FAST SLOWDOWN,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf",
						m_defualtparam->VHLName,
						m_pOHTMainStatus->StatusCommon.CurStationID,
						m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
						m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
						m_pHWSet->m_pHoistAxis->GetCurrentVelocity());
		}
        break;
#endif
	case STEP_L_MOVE_DOWN_HOIST_SLOW: // 저속 이동 : teaching point
        if(nError == NO_ERR)
		{

			nError = Check_Sensor(false, HOIST_DOWN_STATE, STEP_L_MOVE_DOWN_HOIST_SLOW, STEP_L_MOVE_DOWN_HOIST_SLOW);

		}
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_MOVE_DOWN_HOIST_SLOW))
		{
			break;
		}
		
        if(nError == NO_ERR)
       	{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Move_Hoist_Ver2(HOIST_DOWN_TO2TP);
#else
			nError = Move_Hoist(HOIST_DOWN_TO2TP);
#endif
       	}			
		if(nError == NO_ERR)
		{
			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,LD MOVE DOWN HOIST SLOW,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5f,Hoist Dec,%2.5f,Hoist Vel,%2.5lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
		}
        break;

    case STEP_L_CHECK_MOVE_DOWN_HOIST_SLOW: // 2축 이동완료 확인
        if(nError == NO_ERR)
       	{
			nError = Check_Sensor(false, HOIST_DOWN_STATE, STEP_L_MOVE_DOWN_HOIST_SLOW, STEP_L_CHECK_ARRIVE_TEACHING_POINT);
		}
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_CHECK_MOVE_DOWN_HOIST_SLOW))
		{
			break;
		}
		
        if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
			nError =Check_Hoist_Ver2(HOIST_DOWN_TO2TP);
#else
			nError = Check_Hoist(HOIST_DOWN_TO2TP);
#endif

		}
		if(m_nTransStep == STEP_L_CHECK_ARRIVE_TEACHING_POINT)
		{
			ADD_MCC_LOG(",%s,TRANS,LD MOVE DOWN HOIST SLOW,End,FOUP,%06d,%d,%06d,%d,Push On Pos,%6.1lf,Hoist Down Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_PushOnPos,
					OHTUTILITY::PreventOverflowGetTimeMs(m_mccparam.dwHoistDownTime));
		}
        break;

	case STEP_L_CHECK_ARRIVE_TEACHING_POINT: // Hand Push Sensor Check(2축 down)
		if((nError == NO_ERR) && (m_TransUse.HandPushSen == true))
		{
			ADD_MCC_LOG(",%s,TRANS,LD CHECK ARRIVE TEACHING POINT,Start,FOUP,%06d,%d,%06d,%d,Hoist Cur Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition());
			nError = CheckNMoveTeachingPoint();
		}
        if(nError == NO_ERR)
        {
            m_nTransStep++;
			m_dwLREQ_STTime = timeGetTime();
			ADD_MCC_LOG(",%s,TRANS,LD CHECK ARRIVE TEACHING POINT,End,FOUP,%06d,%d,%06d,%d,Hoist Cur Pos,%6.1lf,Push Retry Count,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.nHandPush_RetryCount);
        }
        break;

	case STEP_L_CHECK_BEFORE_RELEASE_FOUP: // Hand Open 전 Check(L_REQ, STB, UTB Sensor)
        if(nError == NO_ERR)
		{
			ADD_MCC_LOG(",%s,TRANS,LD CHECK BEFORE RELEASE,Start,FOUP,%06d,%d,%06d,%d",
				m_defualtparam->VHLName,
				m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
				m_pOHTMainStatus->StatusCommon.CurNodeID,
				m_pOHTMainStatus->StatusCommon.CurNodeOffset);
			nError = Check_BeforeReleaseFoup();
			ADD_MCC_LOG(",%s,TRANS,LD CHECK BEFORE RELEASE,End,FOUP,%06d,%d,%06d,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset);
       	}
		break;

    case STEP_L_RELEASE_FOUP: // Hand Open
		if((nError == NO_ERR) && (m_TransUse.EQPIO == true) )//&& (m_pEQPIO->IsUnloadREQOn() == true))
		{
			Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
//			if(m_pEQPIO->IsUnloadREQOn() == true)
//			{
//				nError = ERR_PIO_UREQ_ABNORMAL_ON_STRETCHED;
//				ADD_LOG("ERR_PIO_UREQ_ABNORMAL_ON_STRETCHED_1");
//				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
//									ERR_PIO_UREQ_ABNORMAL_ON_STRETCHED,
//									m_pOHTMainStatus->StatusCommon.CurNodeID,
//									m_pExecuteInfo->ExecutePositionInfo.NextNode,
//									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//									m_pOHTMainStatus->StatusCommon.ExistFoup,
//									m_pOHTMainStatus->StatusCommon.StopStationID,
//									m_nTransStep);
//			}
		}	
        if(nError == NO_ERR)
		{
			ADD_MCC_LOG(",%s,TRANS,LD RELEASE,Start,FOUP,%06d,%d,%06d,%d,Hoist Cur Pos,%6.1lf",
				m_defualtparam->VHLName,
				m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
				m_pOHTMainStatus->StatusCommon.CurNodeID,
				m_pOHTMainStatus->StatusCommon.CurNodeOffset,
				m_pHWSet->m_pHoistAxis->GetCurrentPosition());
			dwHandTackTime = timeGetTime();
            nError = m_pHWSet->m_pHand->AutoOpen();
       	}			
        if((nError == ERR_HAND_OPEN_TIMEOVER) || (nError == ERR_HAND_ALARM))
        {
            Sleep(100);
            nError = m_pHWSet->m_pHand->AutoOpen();
        }
        if(nError == NO_ERR)
		{
			ADD_LOG("test_load/%06d/%.02f/%.02f/%d/%f", m_pExecuteInfo->ExecutePositionInfo.CurrStation, m_dHoist_TeachingPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_nEqType, (m_pHWSet->m_pHoistAxis->GetCurrentPosition()-m_dHoist_TeachingPos));

			m_nTransStep++;
			//HJH : Teaching Auto Adjustment
			Cal_HoistOffset_Load(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_dHoist_TeachingPos, m_dHoist_TeachingPos_Original, m_StationID);
			ADD_MCC_LOG(",%s,TRANS,LD RELEASE,End,FOUP,%06d,%d,%06d,%d,Hoist Cur Pos,%6.1lf,Release Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					OHTUTILITY::PreventOverflowGetTimeMs(dwHandTackTime));
			SEND_WARN("MCC/15/1/6/%06d/%06d/%d/%d/%06d/%d",
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					OHTUTILITY::PreventOverflowGetTimeMs(dwHandTackTime));

			if((m_TransUse.EQPIO == true) && (m_bIsSTKTypeTrans == false))
			{
				//m_pEQPIO->Init_SequenceForMoving();
			}

       	}
        break;

	case STEP_L_MOVE_UP_HOIST_SLOW: // 2축 이동 : teaching point ~ teaching point-x
        if(nError == NO_ERR)
       	{
			nError = Check_Sensor(true, HOIST_UP_STATE, STEP_L_MOVE_UP_HOIST_SLOW, STEP_L_MOVE_UP_HOIST_SLOW);
		}
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_MOVE_UP_HOIST_SLOW))
		{
			break;
		}
		
        if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Move_Hoist_Ver2(HOIST_UP_TP2TO);
#else
			nError = Move_Hoist(HOIST_UP_TP2TO);
#endif
       	}		
//        if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_bIsSTKTypeTrans == false))
//        {
////            m_pEQPIO->Init_SequenceForMoving();
//            nError = m_pEQPIO->SequenceForMoving(m_nPIOTimeoverLevel);
//        }
        if(nError == NO_ERR)
       	{
			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,LD MOVE UP HOIST SLOW,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%2.5lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
			m_mccparam.dwHoistUpTime = timeGetTime();
		}
		break;
		
	case STEP_L_CHECK_MOVE_UP_HOIST_SLOW: // 2축 이동완료 확인 : teaching point ~ teaching point-x
        if(nError == NO_ERR)
        {
        	nError = Check_Sensor(true, HOIST_UP_STATE, STEP_L_MOVE_UP_HOIST_SLOW, STEP_L_MOVE_UP_HOIST_FAST);
		}
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_CHECK_MOVE_UP_HOIST_SLOW))
		{
			break;
		}
		
        if(nError == NO_ERR)
       	{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Check_Hoist_Ver2(HOIST_UP_TP2TO);
#else
			nError = Check_Hoist(HOIST_UP_TP2TO);
#endif
       	}			
//        if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_bIsSTKTypeTrans == false))
//        {
//            nError = m_pEQPIO->SequenceForMoving(m_nPIOTimeoverLevel);
//		}
		if(m_nTransStep == STEP_L_MOVE_UP_HOIST_FAST)
		{
			ADD_MCC_LOG(",%s,TRANS,LD MOVE UP HOIST SLOW,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity());
		}
		break;
		
    case STEP_L_MOVE_UP_HOIST_FAST: // 2축 이동 : teaching point-x ~ 원점-x지점
        if(nError == NO_ERR)
		{
            nError = Check_Sensor(true, HOIST_UP_STATE, STEP_L_MOVE_UP_HOIST_FAST, STEP_L_MOVE_UP_HOIST_FAST);
		}
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_MOVE_UP_HOIST_FAST))
		{
			break;
		}
		
		if(nError == NO_ERR)
        {
            if(m_bIsSTBTypeTrans == true)
			{
#ifdef USE_TRANS_PROFILE_VER2
				nError = Move_Hoist_Ver2(HOIST_UP_TO2H);
#else
				nError = Move_Hoist(HOIST_UP_TO2H);
#endif
           	}
            else
			{
#ifdef USE_TRANS_PROFILE_VER2
				nError = Move_Hoist_Ver2(HOIST_UP_TO2SDP);
#else
				nError = Move_Hoist(HOIST_UP_TO2HO);
#endif
	       	}				
		}
		if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_bIsSTKTypeTrans == false)
		&&(m_dCmdFinal_HoistPos - m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load)
		{
			//nError = m_pEQPIO->SequenceForMoving(m_nPIOTimeoverLevel);
			dwAfterPIOTime = timeGetTime();
		}
		if(nError == NO_ERR)
       	{
			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,LD MOVE UP HOIST FAST,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
       	}			
        break;
		
    case STEP_L_CHECK_MOVE_UP_HOIST_FAST: // 2축 이동완료 확인 : teaching point-x ~ 원점-x지점
        if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
            nError = Check_Sensor(true, HOIST_UP_STATE, STEP_L_MOVE_UP_HOIST_FAST, STEP_L_MOVE_UP_HOIST_FAST_SLOWDOWN);
#else
            nError = Check_Sensor(true, HOIST_UP_STATE, STEP_L_MOVE_UP_HOIST_FAST, STEP_L_MOVE_UP_HOIST_HOME);
#endif
		}
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_CHECK_MOVE_UP_HOIST_FAST))
		{
			break;
		}
		
        if(nError == NO_ERR)
        {
            if(m_bIsSTBTypeTrans == true)
			{
#ifdef USE_TRANS_PROFILE_VER2
				nError = Check_Hoist_Ver2(HOIST_UP_TO2H);
#else
				nError = Check_Hoist(HOIST_UP_TO2H);
#endif
           	}
            else
           	{
#ifdef USE_TRANS_PROFILE_VER2
				nError = Check_Hoist_Ver2(HOIST_UP_TO2SDP);
#else
				nError = Check_Hoist(HOIST_UP_TO2HO);
#endif
			}
		}
		if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_bIsSTKTypeTrans == false)
		&&(m_dCmdFinal_HoistPos - m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load)
		{
			//nError = m_pEQPIO->SequenceForMoving(m_nPIOTimeoverLevel);
		}
#ifdef USE_TRANS_PROFILE_VER2
		if(m_nTransStep == STEP_L_MOVE_UP_HOIST_FAST_SLOWDOWN)
		{
			ADD_MCC_LOG(",%s,TRANS,LD MOVE UP HOIST FAST,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf,Hoist Up Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity(),
					OHTUTILITY::PreventOverflowGetTimeMs(m_mccparam.dwHoistUpTime));

		}
#else
		if(m_nTransStep == STEP_L_MOVE_UP_HOIST_HOME)
		{
			ADD_MCC_LOG(",%s,TRANS,LD MOVE UP HOIST FAST,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf,Hoist Up Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity(),
					OHTUTILITY::PreventOverflowGetTimeMs(m_mccparam.dwHoistUpTime));

		}
#endif
		break;
 #ifdef USE_TRANS_PROFILE_VER2
     case STEP_L_MOVE_UP_HOIST_FAST_SLOWDOWN: // 2축 이동 : teaching point-x ~ 원점-x지점
        if(nError == NO_ERR)
       	{
            nError = Check_Sensor(true, HOIST_UP_STATE, STEP_L_MOVE_UP_HOIST_FAST, STEP_L_MOVE_UP_HOIST_FAST_SLOWDOWN);
		}
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_MOVE_UP_HOIST_FAST_SLOWDOWN))
		{
			break;
		}
		
        if(nError == NO_ERR && m_bIsSTBTypeTrans == false)
		{
			nError = Move_Hoist_Ver2(HOIST_UP_SDP2HO);
		}
		if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_bIsSTKTypeTrans == false))
		{
			//nError = m_pEQPIO->SequenceForMoving(m_nPIOTimeoverLevel);
		}
		if(nError == NO_ERR)
		{
			if(m_bIsSTBTypeTrans == true)
			{
				m_nTransStep = STEP_L_MOVE_SHIFTROTATE_HOME;
			}
			else
			{
				 m_nTransStep++;
				 ADD_MCC_LOG(",%s,TRANS,LD MOVE UP HOIST FAST SLOWDOWN,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
			}
		}
		break;
		
	case STEP_L_CHECK_MOVE_UP_HOIST_FAST_SLOWDOWN: // 2축 이동완료 확인 : teaching point-x ~ 원점-x지점
        if(nError == NO_ERR)
		{
            nError = Check_Sensor(true, HOIST_UP_STATE, STEP_L_MOVE_UP_HOIST_FAST_SLOWDOWN, STEP_L_MOVE_UP_HOIST_HOME);
		}
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_CHECK_MOVE_UP_HOIST_FAST_SLOWDOWN))
		{
			break;
		}
		
        if(nError == NO_ERR)
        {
                nError = Check_Hoist_Ver2(HOIST_UP_SDP2HO);
        }
		if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_bIsSTKTypeTrans == false))
        {
			//nError = m_pEQPIO->SequenceForMoving(m_nPIOTimeoverLevel);
		}

		if(m_nTransStep == STEP_L_MOVE_UP_HOIST_HOME)
		{
			ADD_MCC_LOG(",%s,TRANS,LD MOVE UP HOIST FAST SLOWDOWN,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf,Hoist Up Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity(),
					OHTUTILITY::PreventOverflowGetTimeMs(m_mccparam.dwHoistUpTime));

		}
        break;
#endif		
	case STEP_L_MOVE_UP_HOIST_HOME: // 2축 이동 : 원점-x지점 ~ 원점
		if(nError == NO_ERR)
        {
            if(m_bIsSTBTypeTrans == false)
            {
                nError = Check_Sensor(false, HOIST_UP_STATE, STEP_L_MOVE_UP_HOIST_HOME, STEP_L_MOVE_UP_HOIST_HOME);

				
				if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_MOVE_UP_HOIST_HOME))
				{
					break;
				}
				
				if(nError == NO_ERR)
				{
#ifdef USE_TRANS_PROFILE_VER2
					nError = Move_Hoist_Ver2(HOIST_UP_HO2H);
#else
					nError = Move_Hoist(HOIST_UP_HO2H);
#endif
				}
            }
        }
        if((nError == NO_ERR) && (m_TransUse.EQPIO == true))
		{
			nError = m_pEQPIO->SequenceForMoving(m_nPIOTimeoverLevel);
        }
        if(nError == NO_ERR)
        {
            if(m_bIsSTBTypeTrans == false)
            {
                m_nTransStep++;
            }
            else
            {
                m_nTransStep += 2;
            }
		}

		if(m_nTransStep == STEP_L_CHECK_MOVE_UP_HOIST_HOME)
		{
			ADD_MCC_LOG(",%s,TRANS,LD MOVE UP HOIST HOME,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%2.5lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
		}
        break;
		
    case STEP_L_CHECK_MOVE_UP_HOIST_HOME: // 2축 이동완료 확인
        if(nError == NO_ERR)
       	{
            nError = Check_Sensor(false, HOIST_UP_STATE, STEP_L_MOVE_UP_HOIST_HOME, STEP_L_MOVE_SHIFTROTATE_HOME);
	   	}			
		// bLookDnCheck,nHoistState,dCmdHoistPos,nPreStep,nNextStep)
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_L_CHECK_MOVE_UP_HOIST_HOME))
		{
			break;
		}
		
        if(nError == NO_ERR)
       	{
 #ifdef USE_TRANS_PROFILE_VER2
			nError = Check_Hoist_Ver2(HOIST_UP_HO2H);
#else
			nError = Check_Hoist(HOIST_UP_HO2H);
#endif
       	}			
        if((nError == NO_ERR) && (m_TransUse.EQPIO == true))
        {
			//nError = m_pEQPIO->SequenceForMoving(m_nPIOTimeoverLevel);
		}

		if(m_nTransStep == STEP_L_MOVE_SHIFTROTATE_HOME)
		{
			ADD_MCC_LOG(",%s,TRANS,LD MOVE UP HOIST HOME,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity());
		}
        break;
		
	case STEP_L_MOVE_SHIFTROTATE_HOME: // 3,4축 TPoint ~ 원점 이동..
        if(nError == NO_ERR)
		{
           // nError = Move_ShiftRotate(SHIFTROTATE_TP2HOME);
       	}			
        if((nError == NO_ERR) && (m_TransUse.EQPIO == true))
		{
			//nError = m_pEQPIO->SequenceForMoving(m_nPIOTimeoverLevel);
        }
        if(nError == NO_ERR)
       	{
			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,LD MOVE SHIFTROTATE HOME,Start,FOUP,%06d,%d,%06d,%d,Shift Acc,%2.5lf,Shift Dec,%2.5lf,Shift Vel,%6.1lf,Shift Cur Pos,%6.1lf,Shift Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dShift_Acc, m_mccparam.dShift_Dec,
					m_mccparam.dShift_Vel,
					m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
					m_mccparam.dShift_TeachingPos);
       	}			
        break;

	case STEP_L_CHECK_MOVE_SHIFTROTATE_HOME: // 3,4축 이동완료 확인
        if(nError == NO_ERR)
       	{
			//ADD_LOG("Load Compt");
			//nError = Check_ShiftRotate();
			m_nTransStep++;
		}
		// 2016-12-04, Add by Labeler
		// Step이 Check_ShiftRotate() 함수에서 증가 시킨 경우에만, 아래 조건 Check
		if(m_nTransStep == STEP_L_PIO_COMPLETE)
        {
            if((nError == NO_ERR) && (m_TransUse.EQPIO == true))
            {
              //  ADD_LOG("Load Compt");
                nError = m_pEQPIO->SequenceForMoving(m_nPIOTimeoverLevel);
			}
			ADD_MCC_LOG(",%s,TRANS,LD MOVE SHIFTROTATE HOME,End,FOUP,%06d,%d,%06d,%d,Shift Cur Pos,%6.1lf,Shift Pos Err,%6.1lf,Shift Home On Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
					m_pHWSet->m_pShiftAxis->GetFollowingError(),
					m_mccparam.dShift_HomeOnPos);
		}
        break;

	case STEP_L_PIO_COMPLETE: // PIO
//		if((nError == NO_ERR) && (m_TransUse.EQPIO == true))
//		{
//
//			if(m_pEQPIO->GetPIOStep() < 9)
//				{
//                    dwStTime = timeGetTime();
//                    dwPIOWaitTime = m_pEQPIO->GetReadyOffTimeover(m_nPIOTimeoverLevel);
//
//					while(m_pEQPIO->GetPIOStep() != 9)
//                    {
//                        nError = m_pEQPIO->SequenceForMoving(m_nPIOTimeoverLevel);
//
//                        if(nError != NO_ERR)
//                       	{
//							break;
//                       	}
//                        if(OHTUTILITY::PreventOverflowGetTimeMs(dwStTime) > dwPIOWaitTime)
//                       	{
//                            m_pEQPIO->Init_SequenceForMoving();
//                            nError = ERR_PIO_READY_ABNORMAL_ON_TA3;
//				            ADD_LOG("ERR_PIO_READY_ABNORMAL_ON_TA3_4");
//							ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/2/%d",
//									ERR_PIO_READY_ABNORMAL_ON_TA3,
//									m_pOHTMainStatus->StatusCommon.CurNodeID,
//									m_pExecuteInfo->ExecutePositionInfo.NextNode,
//									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//									m_pOHTMainStatus->StatusCommon.ExistFoup,
//									m_pOHTMainStatus->StatusCommon.StopStationID,
//									m_pEQPIO->GetPIOStep());
//                            break;
//                       	}
//                        Sleep(10);
//                    }
//                }
//		}
		
        if(nError == NO_ERR)
        {
//            if(m_TransUse.EQPIO == true)
//			{
			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,LD DONE,Start,FOUP,%06d,%d,%06d,%d",
						m_defualtparam->VHLName,
						m_pOHTMainStatus->StatusCommon.CurStationID,
						m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset);
//            }
//            else
//            {
//                m_nTransStep++;
//            }
        }
        break;
		
	case STEP_L_LOAD_COMPLETE:
		if(nError == NO_ERR)
		{
			ADD_LOG("[Load] TackTime(%d)",(timeGetTime() - dwTackTime));
			nError = Check_Complete();
			ADD_MCC_LOG(",%s,TRANS,LD DONE,End,FOUP,%06d,%d,%06d,%d,After Load PIO Time,%d,Oscillation Count,%d,Hoist Home On Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					OHTUTILITY::PreventOverflowGetTimeMs(dwAfterPIOTime),
					m_mccparam.nOscillation_Count,
					m_mccparam.dHoist_HomeOnPos);
			m_mccparam.nOscillation_Count = 0;
			m_mccparam.dHoist_HomeOnPos = 0.0;
		}
        break;
    }

	if(nError != NO_ERR)
    {
		if( (m_bPIOAutoRecovery == true) &&
			(((nError >= ERR_PIO_READY_ABNORMAL_ON_TD1) && (nError <= ERR_PIO_READY_ABNORMAL_OFF_TA2)) ||
			((nError >= ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3) && (nError <= ERR_PIO_READY_ABNORMAL_ON_TA3)))
		)
        {
			ADD_LOG("PIO AR[Step=%d] : Start(E=%d)", m_nTransStep, nError);

			// AutoRecovery 수행
			nError = AutoRecovery_Trans(nError, m_nTransStep);

            if(nError != NO_ERR)
            {
				ADD_LOG("PIO AR Fail[Step=%d] : E(%d)", m_nTransStep, nError);
				m_StatusSet.uError = nError;
                m_StatusSet.byStatus = STS_ERROR;
			}
		}
		else if((nError == ERR_STB_FOUP_EXIST) ||
				 (nError == ERR_UTB_FOUP_EXIST) ||
				 (nError == ERR_UTB_X_FOUP_EXIST))
		{
			ADD_LOG("B/D AR[Step=%d] : Start(E=%d)", m_nTransStep, nError);

			// AutoRecovery 수행, 이중반입
			nError = AutoRecovery_Trans(nError, m_nTransStep,AUTORECOVERYTYPE_DOUBLE);

			if(nError != NO_ERR)
			{
				ADD_LOG("B/D AR Fail[Step=%d] : E	(%d)", m_nTransStep, nError);
				m_StatusSet.uError = nError;
				m_StatusSet.byStatus = STS_ERROR;
			}
        }
		else
		{
			ADD_LOG("Step=%d : E(%d) AR(%d)", m_nTransStep, nError, m_bPIOAutoRecovery);
            m_StatusSet.uError = nError;
            m_StatusSet.byStatus = STS_ERROR;
		}
    }

	if(nError != NO_ERR)
    {
        m_StatusSet.uError = nError;
        m_StatusSet.byStatus = STS_ERROR;

		// shkim. pio errorlog save
		//m_pEQPIO->Save_HybridPIO_Data();

	}



    return nError;
}

/**
 @brief   Unload 시퀀스 명령 수행(설비에서 반송물을 가져가는 작업)
 @author  LSJ
 @date    2012.11.07
 @return  Unload 명령을 문제 없이 수행하면 0, 문제가 발생하면 해당 에러코드를 리턴함.
 */
int TransControl::Cmd_Unload()
{
	m_TransUse.EQPIO = false;
	int nError = NO_ERR;
	static DWORD dwTackTime = timeGetTime();
	static DWORD dwHandTackTime = timeGetTime();
	static DWORD dwBeforePIOTime = timeGetTime();
	static DWORD dwAfterPIOTime = timeGetTime();
	SYSTEMTIME CurTime;
	GetLocalTime(&CurTime);

	if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_nTransStep > STEP_U_EQPIO_READY) && (m_pEQPIO->IsValidOn() == true))
	{
		nError = Check_EQStatus(TRANSTYPE_UNLOAD, m_nTransStep);
	}		

    if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_nTransStep > STEP_U_EQPIO_READY) && (m_nTransStep < STEP_U_MOVE_DOWN_HOIST_SLOW) && (m_pEQPIO->IsValidOn() == true))
	{
		nError = Check_L_U_REQStatus_Down(TRANSTYPE_UNLOAD, m_nTransStep);
	}	

	if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_nTransStep > STEP_U_CHECK_MOVE_UP_HOIST_SLOW) && (m_nTransStep < STEP_U_PIO_COMPLETE) && (m_pEQPIO->IsValidOn() == true))   	
	{
		nError = Check_L_U_REQStatus_Up(TRANSTYPE_UNLOAD, m_nTransStep);   	
	}		

	if((nError == NO_ERR) && (m_nTransStep > STEP_U_EQPIO_READY) && (m_dwTotalTrans_STTime != 0))
	{
        // ** 2축원점 명령 후 간섭으로 인해 원점복귀가 30초동안 안될경우 에러..발생.
        // ** Lookdown Timeover 8초 포함
        // ** PIO 사용할경우 Ready off wait time 포함
        if(m_TransUse.EQPIO == true)
		{
            if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwTotalTrans_STTime) >
				(DWORD)(m_pParamSet->TransDefine.nTrans_Timeover + m_pEQPIO->GetReadyOffTimeover(m_nPIOTimeoverLevel)))
           	{
                nError = ERR_TRANS_AXIS2HOME_TIMEOVER;
				ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%f/%f",
							ERR_TRANS_AXIS2HOME_TIMEOVER,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_dCmdHoistPos,
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
           	}				
        }
        else
		{
            if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwTotalTrans_STTime) > (DWORD)m_pParamSet->TransDefine.nTrans_Timeover)
           	{
                nError = ERR_TRANS_AXIS2HOME_TIMEOVER;
				ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%f/%f",
							ERR_TRANS_AXIS2HOME_TIMEOVER,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_dCmdHoistPos,
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
           	}				
		}
	}
	//HJH : Teaching Auto Adjustment
	static bool preIsPushOn = false;
	if((preIsPushOn == false) && (m_pHWSet->m_pHand->IsPushOn() == true))
	{
		ADD_LOG("[Unload Hand Push Sensor ON] POS : %lf, VEL : %lf", m_pHWSet->m_pHoistAxis->GetCurrentPosition(), fabs(m_pHWSet->m_pHoistAxis->GetCurrentVelocity()));
	}
	preIsPushOn = m_pHWSet->m_pHand->IsPushOn();

    switch(m_nTransStep)
	{
	 case STEP_U_HOISTSENSOR_ORIGIN_RECOVERY:
            nError = Cmd_AutoSetHome(m_nTransStepHoistRecovery, true);
			if(nError == NO_ERR)
			{
				if(m_nTransStepHoistRecovery == STEP_SET_H_SET_HOIST_ORIGIN_COMPLETE)
					m_nTransStep++;
            }
		break;


	 case STEP_U_EQPIO_CONNECT:
		dwTackTime = timeGetTime();
//		if((nError == NO_ERR) && (m_TransUse.EQPIO == true))
//        {
//			if(OHTMainForm->Get_EQPIO_Type() > TYPE_PIO_IR)
//            {
//				if(OHTMainForm->Get_EQPIO_Type() == TYPE_PIO_HYBRID)
//				{
//                    // RF-PIO ID, Channel Setting
//                    //LimJY_20160119 : For IR+RF PIO m_nPIOType 인자 추가
//					nError = m_pEQPIO->Set_RFPIO_Data_RS232_Hybrid(m_nPIOType, m_nPIOID, m_nPIOChannel, m_nPIOCS,m_defualtparam->VHLName);
//
//                    if(m_pEQPIO->GetRFPIOConnectStep()==8)
//                    {
//                        m_nTransStep++;
//                    }
//                }
//                else
//                {
//                    // RF-PIO ID, Channel Setting
//                    nError = m_pEQPIO->Set_RFPIO_Data_RS232_RF(m_nPIOID, m_nPIOChannel, m_nPIOCS);
//
//                    if(m_pEQPIO->GetRFPIOConnectStep()==8)
//                    {
//                        m_nTransStep++;
//                    }
//                }
//            }
//            else
//            {
//                m_nTransStep++;
//            }
//        }
//        else
//        {
//            m_nTransStep++;
//		}

		m_nTransStep++;
		if(m_nTransStep == STEP_U_UNLOAD_READY)
		{
			ADD_MCC_LOG(",%s,TRANS,ULD READY,Start,FOUP,%06d,%d,%06d,%d,PIO ID,%d,PIO Channel,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_nPIOID, m_nPIOChannel);
        }
		break;
	case STEP_U_UNLOAD_READY:
		if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsCloseOutputOn() == true))
		{

			nError = m_pHWSet->m_pHand->IO_Init();
		}

		if(nError == NO_ERR)
		{

			nError = ReadyTrans();
		}
		if(nError == NO_ERR)
		{
		;
			nError = Check_State();
		}
		// UTB가 없더라도 직접 움직여서 확인한다.
//		if((m_TransUse.UTBFoupCheck == true) && (nError == ERR_UTB_FOUP_NOTEXIST))
//		{
//            nError = NO_ERR;
//		}
        if(nError == NO_ERR)
		{

			nError = m_pHWSet->m_pHand->AutoOpen();
       	}			
        if(nError == ERR_HAND_OPEN_TIMEOVER)
		{

            nError = m_pHWSet->m_pHand->AutoOpen();
       	}
        // 2016-12-07, Add by Labeler
        // ROHT의 경우 Rotate Retry Logic 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
        if(nError == NO_ERR)
		{
			nError = rotateRetry();
        }
#endif
        if(nError == NO_ERR)
		{
#if(OHT_NAME != OHT_NAME_STD_V80_RETICLE) && (OHT_NAME != OHT_NAME_STD_V85)
		nError = Move_ShiftRotate(SHIFTROTATE_HOME2TP);
		if(nError != NO_ERR) break;
#endif

			m_nTransStep++;
			dwBeforePIOTime = timeGetTime();
		}
        break;
		
	case STEP_U_EQPIO_READY:
        if((nError == NO_ERR) && (m_TransUse.EQPIO == true))
		{
			nError = m_pEQPIO->SequenceBeforeUnload(m_nPIOTimeoverLevel);
        }
        if(nError == NO_ERR)
        {
            if(m_TransUse.EQPIO == true)
            {
                if(m_pEQPIO->GetUnloadPIOStep() == 14)
				{

					m_nTransStep++;
                }
            }
            else
			{

                m_nTransStep++;
            }
		}
		if(m_nTransStep == STEP_U_MOVE_SHIFTROTATE)
		{
			ADD_MCC_LOG(",%s,TRANS,ULD READY,End,FOUP,%06d,%d,%06d,%d,Hoist P Limit,%6.1lf,Hoist N Limit,%6.1lf,Shift P Limit,%6.1lf,Shift N Limit,%6.1lf,Before Unload PIO Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					HOIST_MOVELIMIT_P, HOIST_MOVELIMIT_N,
					SHIFT_MOVELIMIT_P, SHIFT_MOVELIMIT_N,
					OHTUTILITY::PreventOverflowGetTimeMs(dwBeforePIOTime));
		}
        break;
		
	case STEP_U_MOVE_SHIFTROTATE: // 3,4축 teaching point까지 이동
//		if(nError == NO_ERR)
//		{
//			//nError = Move_ShiftRotate(SHIFTROTATE_HOME2TP);
//		}
        if(nError == NO_ERR)
		{

			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE SHIFTROTATE,Start,FOUP,%06d,%d,%06d,%d,Shift Acc,%2.5lf,Shift Dec,%2.5lf,Shift Vel,%6.1lf,Shift Cur Pos,%6.1lf,Shift Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dShift_Acc, m_mccparam.dShift_Dec,
					m_mccparam.dShift_Vel,
					m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
					m_mccparam.dShift_TeachingPos);
       	}			
        break;
		
    case STEP_U_CHECK_MOVE_SHIFTROTATE: // 3,4축 이동완료 확인
		if(nError == NO_ERR)
		{

			m_nTransStep++;
			//nError = Check_ShiftRotate();
		}
		if(m_nTransStep == STEP_U_MOVE_DOWN_HOIST_FAST)
		{
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE SHIFTROTATE,End,FOUP,%06d,%d,%06d,%d,Shift Cur Pos,%6.1lf,Shift Pos Err,%6.1lf,Shift Home Off Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
					m_pHWSet->m_pShiftAxis->GetFollowingError(),
					m_mccparam.dShift_HomeOffPos);
		}
		break;

	case STEP_U_MOVE_DOWN_HOIST_FAST: // 2축 고속 이동 : teaching point-x까지 이동
		if(nError == NO_ERR)
		{
			nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_U_MOVE_DOWN_HOIST_FAST, STEP_U_MOVE_DOWN_HOIST_FAST);

		}
		
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_MOVE_DOWN_HOIST_FAST))
		{
			break;
		}

        if(nError == NO_ERR)
        {
#ifdef USE_TRANS_PROFILE_VER2
			nError = Move_Hoist_Ver2(HOIST_DOWN_H2SDP);
#else
		nError = Move_Hoist(HOIST_DOWN_H2TO);
#endif
        }
		if(nError == NO_ERR)
		{


			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE DOWN HOIST FAST,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
			m_mccparam.dwHoistDownTime = timeGetTime();
       	}			
		break;

	case STEP_U_CHECK_MOVE_DOWN_HOIST_FAST: // 2축 이동완료 확인: teaching point-x
		if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_U_MOVE_DOWN_HOIST_FAST, STEP_U_MOVE_DOWN_HOIST_FAST_SLOWDOWN);
#else
            nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_U_MOVE_DOWN_HOIST_FAST, STEP_U_MOVE_DOWN_HOIST_SLOW);
#endif
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_CHECK_MOVE_DOWN_HOIST_FAST))
		{
			break;
		}
        if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Check_Hoist_Ver2(HOIST_DOWN_H2SDP);
			if(m_nTransStep == STEP_U_MOVE_DOWN_HOIST_FAST_SLOWDOWN)
			{

				ADD_MCC_LOG(",%s,TRANS,ULD MOVE DOWN HOIST FAST,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf,Hoist Home Off Pos,%6.1lf",
						m_defualtparam->VHLName,
						m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
						m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
						m_pHWSet->m_pHoistAxis->GetCurrentVelocity(),
						m_mccparam.dHoist_HomeOffPos);
			}
#else
			nError = Check_Hoist(HOIST_DOWN_H2TO);
			if(m_nTransStep == STEP_U_MOVE_DOWN_HOIST_SLOW)
			{

			ADD_MCC_LOG(",%s,TRANS,ULD MOVE DOWN HOIST FAST,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf,Hoist Home Off Pos,%6.1lf",
						m_defualtparam->VHLName,
						m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
						m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity(),
					m_mccparam.dHoist_HomeOffPos);
			}
#endif
       	}			
        break;
#ifdef USE_TRANS_PROFILE_VER2
    case STEP_U_MOVE_DOWN_HOIST_FAST_SLOWDOWN: // 2축 고속 이동 : teaching point-x까지 이동
		if(nError == NO_ERR)
        {
			nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_U_MOVE_DOWN_HOIST_FAST, STEP_U_MOVE_DOWN_HOIST_FAST_SLOWDOWN);
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_MOVE_DOWN_HOIST_FAST_SLOWDOWN))
		{
			break;
		}
        if(nError == NO_ERR)
        {
            nError = Move_Hoist_Ver2(HOIST_DOWN_SDP2TO);
		}
        if(nError == NO_ERR)
		{

			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE DOWN HOIST FAST SLOWDOWN,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
			m_mccparam.dwHoistDownTime = timeGetTime();
       	}			
		break;

	case STEP_U_CHECK_MOVE_DOWN_HOIST_FAST_SLOWDOWN: // 2축 이동완료 확인: teaching point-x
		if(nError == NO_ERR)
		{
            nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_U_MOVE_DOWN_HOIST_FAST_SLOWDOWN, STEP_U_MOVE_DOWN_HOIST_SLOW);
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_CHECK_MOVE_DOWN_HOIST_FAST_SLOWDOWN))
		{
			break;
		}
        if(nError == NO_ERR)
       	{
            nError = Check_Hoist_Ver2(HOIST_DOWN_SDP2TO);
		}
		if(m_nTransStep == STEP_U_MOVE_DOWN_HOIST_SLOW)
		{

			ADD_MCC_LOG(",%s,TRANS,ULD MOVE DOWN HOIST FAST SLOWDOWN,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity());
		}
        break;

#endif
    case STEP_U_MOVE_DOWN_HOIST_SLOW: // 저속 이동 : teaching point
        if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Check_Sensor(false, HOIST_DOWN_STATE, STEP_U_MOVE_DOWN_HOIST_SLOW, STEP_U_MOVE_DOWN_HOIST_SLOW);
#else
			nError = Check_Sensor(false, HOIST_DOWN_STATE, STEP_U_MOVE_DOWN_HOIST_SLOW, STEP_U_MOVE_DOWN_HOIST_SLOW);
#endif
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true)|| (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_MOVE_DOWN_HOIST_SLOW))
		{
			break;
		}
        if(nError == NO_ERR)
       	{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Move_Hoist_Ver2(HOIST_DOWN_TO2TP);
#else
			nError = Move_Hoist(HOIST_DOWN_TO2TP);
#endif
       	}			
        if(nError == NO_ERR)
		{

			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE DOWN HOIST SLOW,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%2.5lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
       	}			
        break;

    case STEP_U_CHECK_MOVE_DOWN_HOIST_SLOW: // 2축 이동완료 확인
        if(nError == NO_ERR)
       	{
            nError = Check_Sensor(false, HOIST_DOWN_STATE, STEP_U_MOVE_DOWN_HOIST_SLOW, STEP_U_CHECK_ARRIVE_TEACHING_POINT);
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true)|| (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_CHECK_MOVE_DOWN_HOIST_SLOW))
		{
			break;
		}
        if(nError == NO_ERR)
       	{
 #ifdef USE_TRANS_PROFILE_VER2
			nError = Check_Hoist_Ver2(HOIST_DOWN_TO2TP);
#else
			nError = Check_Hoist(HOIST_DOWN_TO2TP);
#endif
		}
		if(m_nTransStep == STEP_U_CHECK_ARRIVE_TEACHING_POINT)
		{

			ADD_MCC_LOG(",%s,TRANS,ULD MOVE DOWN HOIST SLOW,End,FOUP,%06d,%d,%06d,%d,Push On Pos,%6.1lf,Hoist Down Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_PushOnPos,
					OHTUTILITY::PreventOverflowGetTimeMs(m_mccparam.dwHoistDownTime));
		}
        break;

    case STEP_U_CHECK_ARRIVE_TEACHING_POINT: // Hand Push Sensor Check(2축 down)
        if((nError == NO_ERR) && (m_TransUse.HandPushSen == true))
		{
			ADD_MCC_LOG(",%s,TRANS,ULD CHECK ARRIVE TEACHING POINT,Start,FOUP,%06d,%d,%06d,%d,Hoist Cur Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition());
            nError = CheckNMoveTeachingPoint();
       	}			
        // 2011.08.25 By LSJ TP OHT UTB 동작 시 Hand Push On 될때까지 내려가서 반송 물이 없을 때 Hand Close를 하면 Guide가 땅에 닿아서 핸드 Unit이 흔들리는 것 방지하기 위헤 Hand Close Skip
//		if((m_TransUse.UTBFoupCheck == true) && (nError == ERR_HAND_PUSHSEN_NOTON))
//		{
//            nError = NO_ERR;
//		}
        if(nError == NO_ERR)
		{

			m_nTransStep++;
			m_dwLREQ_STTime = timeGetTime();
			ADD_MCC_LOG(",%s,TRANS,ULD CHECK ARRIVE TEACHING POINT,End,FOUP,%06d,%d,%06d,%d,Hoist Cur Pos,%6.1lf,Push Retry Count,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.nHandPush_RetryCount);
        }
        break;
        
    case STEP_U_GRIP_FOUP: // Hand Close
		if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_pEQPIO->IsLoadREQOn() == true))
		{
			Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
			if(m_pEQPIO->IsLoadREQOn() == true)
			{
				nError = ERR_PIO_LREQ_ABNORMAL_ON_STRETCHED;
				ADD_LOG("ERR_PIO_LREQ_ABNORMAL_ON_STRETCHED");																						
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_LREQ_ABNORMAL_ON_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nTransStep);
			}
		}
		if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_pEQPIO->IsUnloadREQOn() == false))
		{
			Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
			if(m_pEQPIO->IsUnloadREQOn() == false)
			{
				nError = ERR_PIO_UREQ_ABNORMAL_OFF_STRETCHIED;
				ADD_LOG("ERR_PIO_UREQ_ABNORMAL_OFF_STRETCHIED");																										
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_UREQ_ABNORMAL_OFF_STRETCHIED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nTransStep);
			}
		}
        if(nError == NO_ERR)
		{
			ADD_MCC_LOG(",%s,TRANS,ULD GRIP,Start,FOUP,%06d,%d,%06d,%d,Hoist Cur Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition());
			dwHandTackTime = timeGetTime();
			nError = m_pHWSet->m_pHand->AutoClose();

			if(nError == ERR_HAND_ALARM)
			{
				if(m_pHWSet->m_pHand->IsLeftGripOn() && m_pHWSet->m_pHand->IsRightGripOn() &&  m_pHWSet->m_pHand->IsClose())
					nError = m_pHWSet->m_pHand->AutoClose();
			}
		}


        if(nError == NO_ERR)
		{

		   	ADD_LOG("test_unload/%06d/%.02f/%.02f/%d/%f", m_pExecuteInfo->ExecutePositionInfo.CurrStation, m_dHoist_TeachingPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_nEqType, (m_pHWSet->m_pHoistAxis->GetCurrentPosition()-m_dHoist_TeachingPos));

		   	m_nTransStep++;
			//HJH : Teaching Auto Adjustment
			Cal_HoistOffset_Unload(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_dHoist_TeachingPos, m_dHoist_TeachingPos_Original, m_StationID);
		    ADD_MCC_LOG(",%s,TRANS,ULD GRIP,End,FOUP,%06d,%d,%06d,%d,Hoist Cur Pos,%6.1lf,Grip Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					OHTUTILITY::PreventOverflowGetTimeMs(dwHandTackTime));
			SEND_WARN("MCC/16/1/6/%06d/%06d/%d/%d/%06d/%d",
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					OHTUTILITY::PreventOverflowGetTimeMs(dwHandTackTime));
       	}			
        break;

    case STEP_U_MOVE_UP_HOIST_SLOW: // 2축 이동 : teaching point ~ teaching point-x
        if(nError == NO_ERR)
       	{
            nError = Check_Sensor(true, HOIST_UP_STATE, STEP_U_MOVE_UP_HOIST_SLOW, STEP_U_MOVE_UP_HOIST_SLOW);
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true)|| (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_MOVE_UP_HOIST_SLOW))
		{
			break;
		}
        if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Move_Hoist_Ver2(HOIST_UP_TP2TO);
#else
			nError = Move_Hoist(HOIST_UP_TP2TO);
#endif
       	}			
        if(nError == NO_ERR)
		{

			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE UP HOIST SLOW,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%2.5lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
			m_mccparam.dwHoistUpTime = timeGetTime();
       	}			
		break;

    case STEP_U_CHECK_MOVE_UP_HOIST_SLOW: // 2축 이동완료 확인
        if(nError == NO_ERR)
       	{
            nError = Check_Sensor(true, HOIST_UP_STATE, STEP_U_MOVE_UP_HOIST_SLOW, STEP_U_MOVE_UP_HOIST_FAST);
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_CHECK_MOVE_UP_HOIST_SLOW))
		{
			break;
		}
        if(nError == NO_ERR)
       	{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Check_Hoist_Ver2(HOIST_UP_TP2TO);
#else
			nError = Check_Hoist(HOIST_UP_TP2TO);
#endif

		}
		if(m_nTransStep == STEP_U_MOVE_UP_HOIST_FAST)
		{

			ADD_MCC_LOG(",%s,TRANS,ULD MOVE UP HOIST SLOW,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity());
		}
        break;

    case STEP_U_MOVE_UP_HOIST_FAST: // 2축 이동 : teaching point-x ~ 원점-x지점
        if(nError == NO_ERR)
		{
			nError = Check_Sensor(true, HOIST_UP_STATE, STEP_U_MOVE_UP_HOIST_FAST, STEP_U_MOVE_UP_HOIST_FAST);
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_MOVE_UP_HOIST_FAST))
		{
			break;
		}
        if(nError == NO_ERR)
        {
            if(m_bIsSTBTypeTrans == true)
           	{
#ifdef USE_TRANS_PROFILE_VER2
				nError = Move_Hoist_Ver2(HOIST_UP_TO2H);
#else
				nError = Move_Hoist(HOIST_UP_TO2H);
#endif
           	}				
			else
           	{
#ifdef USE_TRANS_PROFILE_VER2
				nError = Move_Hoist_Ver2(HOIST_UP_TO2SDP);
#else
				nError = Move_Hoist(HOIST_UP_TO2HO);
#endif
           	}				
        }
        if(nError == NO_ERR)
		{

			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE UP HOIST FAST,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
       	}			
        break;

    case STEP_U_CHECK_MOVE_UP_HOIST_FAST: // 2축 이동완료 확인
        if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Check_Sensor(true, HOIST_UP_STATE, STEP_U_MOVE_UP_HOIST_FAST, STEP_U_MOVE_UP_HOIST_FAST_SLOWDOWN);
#else
            nError = Check_Sensor(true, HOIST_UP_STATE, STEP_U_MOVE_UP_HOIST_FAST, STEP_U_MOVE_UP_HOIST_HOME);
#endif
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_CHECK_MOVE_UP_HOIST_FAST))
		{
			break;
		}
        if(nError == NO_ERR)
        {
            if(m_bIsSTBTypeTrans == true)
           	{
				
#ifdef USE_TRANS_PROFILE_VER2
				nError = Check_Hoist_Ver2(HOIST_UP_TO2H);
#else
				nError = Check_Hoist(HOIST_UP_TO2H);
#endif
			}
			else
			{
#ifdef USE_TRANS_PROFILE_VER2
				nError = Check_Hoist_Ver2(HOIST_UP_TO2SDP);
#else
				nError = Check_Hoist(HOIST_UP_TO2HO);
#endif
			}
		}
#ifdef USE_TRANS_PROFILE_VER2
		if(m_nTransStep == STEP_U_MOVE_UP_HOIST_FAST_SLOWDOWN)
		{

			ADD_MCC_LOG(",%s,TRANS,ULD MOVE UP HOIST FAST,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf,Hoist Up Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity(),
					OHTUTILITY::PreventOverflowGetTimeMs(m_mccparam.dwHoistUpTime));
		}
#else
		if(m_nTransStep == STEP_U_MOVE_UP_HOIST_HOME)
		{

			ADD_MCC_LOG(",%s,TRANS,ULD MOVE UP HOIST FAST,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf,Hoist Up Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity(),
					OHTUTILITY::PreventOverflowGetTimeMs(m_mccparam.dwHoistUpTime));
		}
#endif
        break;
#ifdef USE_TRANS_PROFILE_VER2
     case STEP_U_MOVE_UP_HOIST_FAST_SLOWDOWN: // 2축 이동 : teaching point-x ~ 원점-x지점
        if(nError == NO_ERR)
       	{
			nError = Check_Sensor(true, HOIST_UP_STATE, STEP_U_MOVE_UP_HOIST_FAST, STEP_U_MOVE_UP_HOIST_FAST_SLOWDOWN);
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) ||(m_nTransStep != STEP_U_MOVE_UP_HOIST_FAST_SLOWDOWN))
		{
			break;
		}
		if(nError == NO_ERR && m_bIsSTBTypeTrans == false)
		{
			nError = Move_Hoist_Ver2(HOIST_UP_SDP2HO);
		}
		if(nError == NO_ERR)
		{
			if(m_bIsSTBTypeTrans == true)
			{
				ADD_LOG("STEP : %d",m_nTransStep);
				m_nTransStep = STEP_U_MOVE_SHIFTROTATE_HOME;
			}
			else
			{
					ADD_LOG("STEP : %d",m_nTransStep);
				m_nTransStep++;
				ADD_MCC_LOG(",%s,TRANS,ULD MOVE UP HOIST FAST SLOWDOWN,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
			}
		}
		break;

	case STEP_U_CHECK_MOVE_UP_HOIST_FAST_SLOWDOWN: // 2축 이동완료 확인
		if(nError == NO_ERR)
		{
			nError = Check_Sensor(true, HOIST_UP_STATE, STEP_U_MOVE_UP_HOIST_FAST_SLOWDOWN, STEP_U_MOVE_UP_HOIST_HOME);
		}
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_CHECK_MOVE_UP_HOIST_FAST_SLOWDOWN))
		{
			break;
		}
		if(nError == NO_ERR)
		{
			nError = Check_Hoist_Ver2(HOIST_UP_SDP2HO);
		}
		if(m_nTransStep == STEP_U_MOVE_UP_HOIST_HOME)
		{
			ADD_LOG("STEP : %d",m_nTransStep);
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE UP HOIST FAST SLOWDOWN,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf,Hoist Up Time,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity(),
					OHTUTILITY::PreventOverflowGetTimeMs(m_mccparam.dwHoistUpTime));
		}
		break;
#endif
    case STEP_U_MOVE_UP_HOIST_HOME: // 2축 이동 : 원점-x지점 ~ 원점
        if(nError == NO_ERR)
		{
            if(m_bIsSTBTypeTrans == false)
            {
                if(nError == NO_ERR)
				{
                    nError = Check_Sensor(false, HOIST_UP_STATE, STEP_U_MOVE_UP_HOIST_HOME, STEP_U_MOVE_UP_HOIST_HOME);
				}
				if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_MOVE_UP_HOIST_HOME))
				{
					break;
				}
                if(nError == NO_ERR)
               	{
#ifdef USE_TRANS_PROFILE_VER2
					nError = Move_Hoist_Ver2(HOIST_UP_HO2H);
#else
					nError = Move_Hoist(HOIST_UP_HO2H);
#endif
               	}					
                if(nError == NO_ERR)
				{
					ADD_LOG("STEP : %d",m_nTransStep);
                    m_nTransStep++;
               	}					
            }
            else
            {
				if(nError == NO_ERR)
				{
					ADD_LOG("STEP : %d",m_nTransStep);
					m_nTransStep += 2;
               	}					
            }
		}
		if(m_nTransStep == STEP_U_CHECK_MOVE_UP_HOIST_HOME)
		{
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE UP HOIST HOME,Start,FOUP,%06d,%d,%06d,%d,Hoist Acc,%2.5lf,Hoist Dec,%2.5lf,Hoist Vel,%2.5lf,Hoist Cur Pos,%6.1lf,Hoist Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_Acc, m_mccparam.dHoist_Dec,
					m_mccparam.dHoist_Vel,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_mccparam.dHoist_TeachingPos);
		}
        break;

	case STEP_U_CHECK_MOVE_UP_HOIST_HOME: // 2축 이동완료 확인
        if(nError == NO_ERR)
        {
            if(m_bIsSTBTypeTrans == false)
            {
                if(nError == NO_ERR)
               	{
                    nError = Check_Sensor(false, HOIST_UP_STATE, STEP_U_MOVE_UP_HOIST_HOME, STEP_U_MOVE_SHIFTROTATE_HOME);
               	}					
				if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_U_CHECK_MOVE_UP_HOIST_HOME))
               	{
                    break;
               	}					
                if(nError == NO_ERR)
               	{
#ifdef USE_TRANS_PROFILE_VER2
					nError = Check_Hoist_Ver2(HOIST_UP_HO2H);
#else
					nError = Check_Hoist(HOIST_UP_HO2H);
#endif
               	}					
            }
            else
            {
                if(nError == NO_ERR)
				{
					ADD_LOG("STEP : %d",m_nTransStep);
					m_nTransStep++;
               	}					
            }
		}
		if(m_nTransStep == STEP_U_MOVE_SHIFTROTATE_HOME)
		{
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE UP HOIST HOME,End,FOUP,%06d,%d,%06d,%d,Hoist Start Pos,%6.1lf,Hoist End Pos,%6.1lf,Hoist Cur Pos,%6.1lf,Hoist Cur Speed,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dHoist_StartPos, m_mccparam.dHoist_EndPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					m_pHWSet->m_pHoistAxis->GetCurrentVelocity());
		}
        break;

    case STEP_U_MOVE_SHIFTROTATE_HOME: // 3,4축 TPoint ~ 원점 이동..
		if(nError == NO_ERR)
       	{
           // nError = Move_ShiftRotate(SHIFTROTATE_TP2HOME);
	
        }
        if(nError == NO_ERR)
		{
			ADD_LOG("STEP : %d",m_nTransStep);
			m_nTransStep++;
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE SHIFTROTATE HOME,Start,FOUP,%06d,%d,%06d,%d,Shift Acc,%2.5lf,Shift Dec,%2.5lf,Shift Vel,%6.1lf,Shift Cur Pos,%6.1lf,Shift Teaching Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_mccparam.dShift_Acc, m_mccparam.dShift_Dec,
					m_mccparam.dShift_Vel,
					m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
					m_mccparam.dShift_TeachingPos);
       	}
        break;

    case STEP_U_CHECK_MOVE_SHIFTROTATE_HOME: // 3,4축 이동완료 확인
        if(nError == NO_ERR)
       	{
			//nError = Check_ShiftRotate();
			m_nTransStep++;
       	}

		
		if((nError == NO_ERR) && (m_TransUse.EQPIO == true)&& (m_bIsSTKTypeTrans == false) && (m_pEQPIO->GetAfterUnloadPIOStep() < 10))
		{
//			nError = m_pEQPIO->SequenceAfterUnload(m_nPIOTimeoverLevel);
//			   dwAfterPIOTime = timeGetTime();
		}

        // 2016-12-04, Add by Labeler
		// Step이 Check_ShiftRotate() 함수에서 증가 시킨 경우에만, 아래 조건 Check
		if(m_nTransStep == STEP_U_PIO_COMPLETE)
		{
            ADD_LOG("STEP : %d",m_nTransStep);
			m_TransUse.UTBFoupCheck = false;
			if((nError == NO_ERR) && (m_nTransStep == STEP_U_PIO_COMPLETE) && (m_TransUse.UTBFoupCheck == true) &&
                (m_pHWSet->m_pHand->IsOpen() == true) && (m_pHWSet->m_pHand->IsRightGripOn() == false) &&
                (m_pHWSet->m_pHand->IsLeftGripOn() == false))
			{
				//m_pHWSet->m_pShutter->SendCloseCmd();
				//nError = ERR_UTB_FOUP_NOTEXIST;
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_UTB_FOUP_NOTEXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect());
			}
			ADD_MCC_LOG(",%s,TRANS,ULD MOVE SHIFTROTATE HOME,End,FOUP,%06d,%d,%06d,%d,Shift Cur Pos,%6.1lf,Shift Pos Err,%6.1lf,Shift Home On Pos,%6.1lf,FOUP Cover Off Pos,%6.1lf,FOUP Detect On Pos,%6.1lf,Left STB Detect Off Pos,%6.1lf,Right STB Detect Off Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
					m_pHWSet->m_pShiftAxis->GetFollowingError(),
					m_mccparam.dShift_HomeOnPos,
					m_mccparam.dFOUPCover_OffPos,
					m_mccparam.dFOUPDetect_OnPos,
					m_mccparam.dLeftSTBDetect_OffPos,
					m_mccparam.dRightSTBDetect_OffPos);

        }
		break;

    case STEP_U_PIO_COMPLETE: // PIO
        if((nError == NO_ERR) && (m_TransUse.EQPIO == true))
       	{
            nError = m_pEQPIO->SequenceAfterUnload(m_nPIOTimeoverLevel);
       	}			
		if(m_TransUse.EQPIO == true)
        {
            if((nError == NO_ERR) && (m_pEQPIO->GetAfterUnloadPIOStep() == 10))
           	{
                m_nTransStep++;
				ADD_MCC_LOG(",%s,TRANS,ULD DONE,Start,FOUP,%06d,%d,%06d,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset);
           	}				
        }
        else
        {
            if(nError == NO_ERR)
			{
				ADD_LOG("STEP : %d",m_nTransStep);
                m_nTransStep++;
				ADD_MCC_LOG(",%s,TRANS,ULD DONE,Start,FOUP,%06d,%d,%06d,%d",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset);
			}
        }
        break;

	case STEP_U_CHECK_ANTIDROP: // shutter close : Manual Mode일때만, Auto일때는 주행할때
		///@todo 추후 Manual일때만 Shutter 닫는 걸로 바꿈.
//	if(!m_defualtparam->PreShutter)
//	{
//
//		if((nError == NO_ERR) && (m_TransUse.ShutterControl == true))
//		{
//			//nError = m_pHWSet->m_pShutter->SendCloseCmd();
//
//			// RailSupport 동작
//			//ADD_LOG("[STEP_U_CHECK_ANTIDROP] E : %d(Shutter Close)", nError);
//			//ADD_LOG("Unload Compt");
//
//// 2016-11-30, Labeler
//#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
//			if(nError == NO_ERR)
//			{
//				while ((nError = m_pHWSet->m_pShutter->CheckComplete()) == NEXT_CHECK)
//				{
//					Sleep(10);
//				}
//			}
//#else
//			if(nError == NO_ERR)
//			{
//
//				while ((nError = m_pHWSet->m_pShutter->CheckComplete()) == NEXT_CHECK)
//				{
//					Sleep(10);
//				}
//
//			}
//#endif
//		}
//
//
//	}
//	else   //Shutter 사전동작
//	{
//
//#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
//		if(nError == NO_ERR)
//		{
//			while ((nError = m_pHWSet->m_pShutter->CheckComplete()) == NEXT_CHECK)
//			{
//				Sleep(10);
//			}
//		}
//#else
		//nError = m_pHWSet->m_pShutter->SendCloseCmd();

//		if(nError == NO_ERR)
//		{
//			if(m_defualtparam->OutriggerHomeSenIgnor ==false)
//			{
//				if(m_pHWSet->m_pRailSupport->IsHomePos() !=true)
//				{
//					ADD_LOG("[STEP_U_CHECK_ANTIDROP m_pRailSupport] %f", m_dShift_TeachingPos);
//					if(nError == NO_ERR)
//					{
//						m_pHWSet->m_pRailSupport->SendMoveHomePosCmd();
//					}
//
//					while ((nError = m_pHWSet->m_pRailSupport->CheckComplete()) == NEXT_CHECK)
//					{
//						Sleep(10);
//					}
//
//				}
//			}
//		}
//#endif
//	}


		if(nError == NO_ERR)
		{
			ADD_LOG("%d",m_nTransStep);
			m_nTransStep++;
		}
		break;

    case STEP_U_UNLOAD_COMPLETE:
        if(nError == NO_ERR)
		{
			ADD_LOG("[Unload] TackTime(%d)",(timeGetTime() - dwTackTime));
			nError = Check_Complete();
			ADD_MCC_LOG(",%s,TRANS,ULD DONE,End,FOUP,%06d,%d,%06d,%d,After Unload PIO Time,%d,Oscillation Count,%d,Hoist Home On Pos,%6.1lf",
					m_defualtparam->VHLName,
					m_pOHTMainStatus->StatusCommon.CurStationID,
					m_mccparam.nEqType,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					OHTUTILITY::PreventOverflowGetTimeMs(dwAfterPIOTime),
					m_mccparam.nOscillation_Count,
					m_mccparam.dHoist_HomeOnPos);
			m_mccparam.nOscillation_Count = 0;
			m_mccparam.dHoist_HomeOnPos = 0.0;
       	}			
        break;
    }

    if(nError != NO_ERR)
	{
		ADD_LOG("Check_Complete err");

    	if( (m_bPIOAutoRecovery == true) && 
			((nError >= ERR_PIO_READY_ABNORMAL_ON_TD1) && (nError <= ERR_PIO_READY_ABNORMAL_OFF_TA2) ||
			(nError >= ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3) && (nError <= ERR_PIO_READY_ABNORMAL_ON_TA3))
		)
		{
			ADD_LOG("PIO AR[Step=%d] : Start(E=%d)", m_nTransStep, nError);

            // AutoRecovery 수행
			nError = AutoRecovery_Trans(nError, m_nTransStep);

            if(nError != NO_ERR)
            {
				ADD_LOG("PIO AR[Step=%d] : E(%d)", m_nTransStep, nError);
                m_StatusSet.uError = nError;
                m_StatusSet.byStatus = STS_ERROR;
            }
		}
		else if((nError == ERR_STB_FOUP_NOTEXIST) ||
				 (nError == ERR_UTB_FOUP_NOTEXIST) ||
				 (nError == ERR_UTB_X_FOUP_NOTEXIST))
		{
			ADD_LOG("B/D AR[Step=%d] : Start(E=%d)", m_nTransStep, nError);

			// AutoRecovery 수행, 공출하.
			nError = AutoRecovery_Trans(nError, m_nTransStep,AUTORECOVERYTYPE_BLANK);

			if(nError != NO_ERR)
			{
				ADD_LOG("B/D AR Fail[Step=%d] : E(%d)", m_nTransStep, nError);
				m_StatusSet.uError = nError;
				m_StatusSet.byStatus = STS_ERROR;
			}
		}
        else
        {
			ADD_LOG("Step=%d : E(%d) AR(%d)", m_nTransStep, nError, m_bPIOAutoRecovery);
			m_StatusSet.uError = nError;
            m_StatusSet.byStatus = STS_ERROR;
        }
    }

    if(nError != NO_ERR)
    {
        m_StatusSet.uError = nError;
        m_StatusSet.byStatus = STS_ERROR;

		// shkim. pio errorlog save
		//m_pEQPIO->Save_HybridPIO_Data();
    }
    return nError;
}

int TransControl::Cmd_MoveTP()
{
	int nError = 0;

	switch(m_nTransStep)
	{
    case STEP_MOVE_TP_READY:
		if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsCloseOutputOn() == true))
            nError = m_pHWSet->m_pHand->IO_Init();
        if(nError == NO_ERR)
            nError = ReadyTrans();
        if(nError == NO_ERR)
            nError = Check_State();
        if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsLeftGripOn() == false) && (m_pHWSet->m_pHand->IsRightGripOn() == false))
            nError = m_pHWSet->m_pHand->AutoOpen();
        if(nError == NO_ERR)
            m_nTransStep++;
        break;
#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	case STEP_MOVE_TP_PRE_SHIFTROTATE:
	{
#if(OHT_NAME == OHT_NAME_STD_V85)
//		nError = Move_EUV_Shift();

		// EUV재하가 있고, Rotate 동작이 필요할 경우 10mm이동해야 함
		if((m_pHWCommon->m_Input.POD_EUV_1 == ON) &&
//				(m_pHWCommon->m_Input.POD_EUV_2 == ON) &&
				(fabs(m_dRotate_TeachingPos - m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > ROTATE_INPOS_LIMIT))
		{
			nError = Move_EUV_Rotate(SHIFTROTATE_HOME2TP);

			if(nError == NO_ERR)
			{
				m_nTransStep++;
			}
		}
		else
		{
			m_nTransStep = STEP_MOVE_TP_MOVE_SHIFTROTATE;
		}
#else
    nError = Move_EUV_Shift();
	if(nError == NO_ERR)
	{
		m_nTransStep++;
	}
#endif

	}
	break;
	case STEP_MOVE_TP_CHECK_SHIFTROTATE:
	{
#if(OHT_NAME == OHT_NAME_STD_V85)
		nError = Check_EUV_Rotate();
#else
		nError = Check_EUV_Shift();
#endif
		if(nError == NO_ERR)
			m_nTransStep++;
	}
	break;
#endif
	case STEP_MOVE_TP_MOVE_SHIFTROTATE: // 3,4축 teaching point까지 이동
        if(nError == NO_ERR)
			nError = Move_ShiftRotate(SHIFTROTATE_HOME2TP);
        if(nError == NO_ERR)
            m_nTransStep++;
        break;
    case STEP_MOVE_TP_CHECK_MOVE_SHIFTROTATE: // 3,4축 이동완료 확인
        if(nError == NO_ERR)
            nError = Check_ShiftRotate();
        break;
	case STEP_MOVE_TP_MOVE_DOWN_HOIST: // 2축 고속 이동 : teaching point-x까지 이동
        if(nError == NO_ERR)
			nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_MOVE_TP_MOVE_DOWN_HOIST, STEP_MOVE_TP_CHECK_MOVE_DOWN_HOIST);
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_MOVE_TP_MOVE_DOWN_HOIST))
            break;
        if(nError == NO_ERR)
			nError = Move_Hoist(HOIST_DOWN_H2TP);
        if(nError == NO_ERR)
            m_nTransStep++;
        break;
    case STEP_MOVE_TP_CHECK_MOVE_DOWN_HOIST: // 2축 이동완료 확인: teaching point-x
        if(nError == NO_ERR)
			nError = Check_Sensor(true, HOIST_DOWN_STATE, STEP_MOVE_TP_MOVE_DOWN_HOIST, STEP_MOVE_TP_CHECK_MOVE_DOWN_HOIST);
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != STEP_MOVE_TP_CHECK_MOVE_DOWN_HOIST))
			break;
        if(nError == NO_ERR)
			nError = Check_Hoist(HOIST_DOWN_H2TP);
        break;
    case STEP_MOVE_TP_COMPLETE:
        if(nError == NO_ERR)
            nError = Check_Complete();
        break;
    }

	return nError;

}

//오토티칭 영점 조정 18
int TransControl::Cmd_AutoTeaching_NullType()
{
  	int nError = 0;

	bool bNextStep = false;
	static int iCount = 0;
	static int ResponseCheckCount = 0;

	DWORD dwCurrTime = timeGetTime();
	static DWORD dwAutoTeacingJigTime =  timeGetTime();  //연결 확인이 되었을시, 일정 시간 확인하여 명령 주는 부분.
//	bool bAutoTeachingCmd = false;   //VHL -> Jig 명령이 있을 경우 True

	char SendBuf[20] = {0,};
	char CmpBuf[20] = {0,};

	//AT Jig와 통신받는 부분 갱신.
	if((m_AutoTeaching->IsConnect()) && (m_nTransStep > N_AUTO_TEACHING_READY))
	{
		nError = m_AutoTeaching->UpdateATData();   //일정 시간동안 응답이 없을 경우 Error 발생할 것.

	}

	switch(m_nTransStep)
	{
	 //기본값 Hoist 확인 (5mm보다 커야됨. 등등.)       // 티칭위치보다 20mm 이전값 저장(음수인경우는 에러 처리).
	 //CheckAutoTeachningReady() : 현재 Foup 들고있는지 확인, AutoTeaching Jig와 통신유무 확인 등
	  //0.각종 명령 초기화 단계.
	 case N_AUTO_TEACHING_READY:
		if(nError == NO_ERR)
			nError = m_AutoTeaching->ServerBind(AUTO_TEACH_MY_SERVER_PORT, m_defualtparam->AutoTeachingIP);  //AT와 통신 시작
	 	if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsCloseOutputOn() == true))
            nError = m_pHWSet->m_pHand->IO_Init();
        if(nError == NO_ERR)
			nError = ReadyTrans_AutoTeaching();      //Auto Teaching Jig와 통신하는 부분 추가할 것.
        if(nError == NO_ERR)
			nError = Check_State_AutoTeaching();
		if(nError == NO_ERR)
		{
			m_AutoTeaching->iCmd = CMD_INT;
            sprintf(SendBuf, "%s",BGE);
			m_AutoTeaching->SetAutoTeachingJigResponse();
			m_nTransStep = N_AUTO_TEACHING_CHECK_BEGIN;

			dwAutoTeacingJigTime =  timeGetTime();
			iCount =0;
			ResponseCheckCount = 0;
		}
		break;

	 case  N_AUTO_TEACHING_CHECK_BEGIN:
		if(m_AutoTeaching->iCmd == CMD_READY)
		{
			ADD_LOG("Reset 명령");
			sprintf(SendBuf, "%s",RST);
			m_nTransStep = N_AUTO_TEACHING_MOVE_INIT_SHIFTROTATE;


            ResponseCheckCount = 0;
			dwAutoTeacingJigTime =  timeGetTime();

		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwAutoTeacingJigTime) >CMD_RETRY)
			{
				 ResponseCheckCount++;
				 dwAutoTeacingJigTime =  timeGetTime();
             	 sprintf(SendBuf, "%s",BGE);
			}
			else if(ResponseCheckCount >3)
			{
              	nError = ERR_AUTOTEACHING_RESPONCE;
            }
		}
		break;

	  //시작 위치 이동 Check(기본 Map 파일 참조, 상위에서 명령 주기)
	 case N_AUTO_TEACHING_MOVE_INIT_SHIFTROTATE:
		if(nError ==NO_ERR)
		   nError =	Move_ShiftRotate_AutoTeaching(SHIFTROTATE_H2TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = N_AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_SHIFTROTATE;
		break;
	 case N_AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_SHIFTROTATE:
		 if(nError == NO_ERR)
			nError = Check_ShiftRotate_AutoTeaching(SHIFTROTATE_H2TP_AT,bNextStep);
		 if((nError == NO_ERR) &&(bNextStep ==true))
			m_nTransStep = N_AUTO_TEACHING_MOVE_INIT_DOWN_HOIST;
		break;
	 case N_AUTO_TEACHING_MOVE_INIT_DOWN_HOIST:
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(true, HOIST_DOWN_STATE, AUTO_TEACHING_MOVE_INIT_DOWN_HOIST, N_AUTO_TEACHING_MOVE_INIT_DOWN_HOIST);
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != N_AUTO_TEACHING_MOVE_INIT_DOWN_HOIST))
			break;
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_DOWN_H2TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = N_AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_DOWN_HOIST;
		break;
	 case N_AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_DOWN_HOIST:
		if(nError == NO_ERR)
		{
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
				nError = Check_Sensor_AutoTeaching(true, HOIST_DOWN_STATE, N_AUTO_TEACHING_MOVE_INIT_DOWN_HOIST, N_AUTO_TEACHING_TEACHING_CMD);
			else
				nError = Check_Sensor_AutoTeaching(true, HOIST_DOWN_STATE, N_AUTO_TEACHING_MOVE_INIT_DOWN_HOIST, N_AUTO_TEACHING_F_TEACHING_CMD);
        }
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != N_AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_DOWN_HOIST))
			break;
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_DOWN_H2TP_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
				m_nTransStep = N_AUTO_TEACHING_TEACHING_CMD;   //m_nTransStep = AUTO_TEACHING_P_TEACHING_CMD;    //추가 로직 수정
			else
				m_nTransStep = N_AUTO_TEACHING_F_TEACHING_CMD;    //추가 로직 수정
		}
		break;


    //////////////////////////////////////////////
	  //1.---------Focus 확인 및 이동
    //////////////////////////////////////////////
	case N_AUTO_TEACHING_F_TEACHING_CMD:    // Focus Teaching 1회 (FT) ---AT와 통신
			m_AutoTeaching->iCmd = CMD_INT;
//			memmove(SendBuf, FT, 4);
			if(m_nEqType == PORT_TYPE_STOCKER)
	            sprintf(SendBuf, "%s",FT_STK);
			else
				sprintf(SendBuf, "%s",FT_EQ);
//			m_AutoTeaching->AutoTeaching_SendCommand(FT);
			//상태값을 초기에 설정하고 나머지는 덥어 씌울것.

			ResponseCheckCount = 0;
			dwAutoTeacingJigTime =  timeGetTime();

			m_nTransStep = N_AUTO_TEACHING_F_TEACHING_WAIT;
			ADD_LOG("Focus 명령");
		break;

	case N_AUTO_TEACHING_F_TEACHING_WAIT:
	 // 명령완료 조건 필요 ///?????????????????????????????????????
		if(m_AutoTeaching->iCmd == CMD_RESPONSE)
			{
				m_AutoTeaching->iCmd = CMD_INT;
				// 수신된 Hoist 값에 대해 완료 여부 판단 필요
				m_AutoTeaching->dHoist_Offset = m_AutoTeaching->fHoist;
				if(m_AutoTeaching->fHoist == 0.0)    // FT : Hoist 이동 완료
				{
					m_nTransStep = N_AUTO_TEACHING_TEACHING_CMD;
					ADD_LOG("Focus 현위치 유지");
				}
				else    // FT : Hoist 이동 필요
				{
					m_nTransStep = N_AUTO_TEACHING_MOVE_FOCUS_CMD;
					ADD_LOG("Focus 이동 %f",m_AutoTeaching->dHoist_Offset);

                }
			}
			else
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwAutoTeacingJigTime) >CMD_RETRY)
				{
					 ResponseCheckCount++;
//					 dwAutoTeacingJigTime =  timeGetTime();
					 if(m_nEqType == PORT_TYPE_STOCKER)
						sprintf(SendBuf, "%s",FT_STK);
					 else
						sprintf(SendBuf, "%s",FT_EQ);
				}
				else if(ResponseCheckCount >5)
				{
					nError = ERR_AUTOTEACHING_RESPONCE;
				}
            }
		break;

	case N_AUTO_TEACHING_MOVE_FOCUS_CMD:

		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_MOVE_TP_AT);
		if(nError == NO_ERR)
		{
			ADD_LOG("AUTO_TEACHING_MOVE_FOCUS_CMD");
			m_nTransStep = N_AUTO_TEACHING_MOVE_FOCUS_WAIT;
        }
		break;

	case N_AUTO_TEACHING_MOVE_FOCUS_WAIT:
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_MOVE_TP_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
			{
                ADD_LOG("AUTO_TEACHING_MOVE_FOCUS_WAIT");
				m_nTransStep = N_AUTO_TEACHING_TEACHING_CMD;
			}
		break;


	//2. --------------AT 반복 구간 ---시작-------------------//
	   // AT 명령 이후 과정 정리 필요
		/*
		1. AT 명령 전송 후 Drive, Slide 방향 보정값 수신
		   - 수신 데이터를 통해 이동/완료 여부 판단
		2. Rail Support 이동(주행축 이동 있을 시에만)
		3. Dirve 방향 보정값 이동
		4. Slide 보정값 이동
		(1번에서 완료 판단 여부에 따라 2 ~ 6번 반복 수행)
		*/

	case N_AUTO_TEACHING_TEACHING_CMD:         // Auto Teaching 반복 (AT)   ---AT와 통신
			m_AutoTeaching->iCmd = CMD_INT;

			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
			{
				if(m_AutoTeaching->bSTBTeaching  ==true)
					sprintf(SendBuf, "%s",FT_STB);
				else
				{
					if(m_nEqType == PORT_TYPE_STB_L)
						sprintf(SendBuf, "%s",AT_STB_L);
					else
                        sprintf(SendBuf, "%s",AT_STB_R);
				}
			}
			else
			{
				if(m_nEqType == PORT_TYPE_STOCKER)
					sprintf(SendBuf, "%s",AT_STK);
				else
					sprintf(SendBuf, "%s",AT_EQ);
			}

//			sprintf(SendBuf, "%s",AT);      // 오토티칭 명령을 전송

			ResponseCheckCount = 0;
			dwAutoTeacingJigTime =  timeGetTime();
			m_nTransStep = N_AUTO_TEACHING_TEACHING_WAIT;	// 통신 수신준비
    		break;

	case N_AUTO_TEACHING_TEACHING_WAIT:       // 티칭 결과값이 넘어오면, 오차범위내에 들어갔는지 확인하고,
		if(m_AutoTeaching->iCmd == CMD_RESPONSE)
			{
				iCount++;
				m_AutoTeaching->iCmd = CMD_INT;
				//fSlide,fDrive Auto teacing Jig 에서 받아오는 값.

				m_AutoTeaching->bSTBTeaching  = false;  //STB

				m_AutoTeaching->fSlide_Offset = m_AutoTeaching->fSlide;
				m_AutoTeaching->fDrive_Offset = m_AutoTeaching->fDrive;


				if((m_AutoTeaching->fSlide == 0.0) && (m_AutoTeaching->fDrive == 0.0) && (m_AutoTeaching->fHoist == 0.0))	// 티칭 완료
				{
					iCount = 0;
					m_nTransStep = N_AUTO_TEACHING_Z_TEACHING_CMD;
					ADD_LOG("티칭 성공!");
				}
				else if(iCount > 10)
				{
					iCount = 0;
					nError = ERR_AUTOTEACHING_TEACHING_FAIL;     //에러 발생
					ADD_LOG("티칭 실패!");
				}
				else
				{

					if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
					{
						m_dHoist_TeachingPos = m_dHoist_TeachingPos + m_AutoTeaching->fHoist;  //STB 작업으로 추가함.
						m_nTransStep = N_AUTO_TEACHING_MOVE_UP_HOIST_STB;    // 2. 주행 축 이동
					}
					else
					{
						if(m_AutoTeaching->fDrive != 0.0)
						{
							// 현재 목표값 저장
							ADD_LOG("[No %02d] 주행방향으로 Move :(%.3f)", iCount, m_AutoTeaching->fDrive);
							m_nTransStep = AUTO_TEACHING_MOVE_X_POS_CMD;    // 2. 주행 축 이동

						}
						else if(m_AutoTeaching->fSlide != 0.0)
						{
							// 현재 목표값 저장


							ADD_LOG("[No %02d] Slide방향으로 Move :(%.3f)", iCount,  m_AutoTeaching->fSlide);
							m_nTransStep = N_AUTO_TEACHING_MOVE_Y_POS_CMD;    // 2. 슬라이드 이동
						}
					}
				}
			}
			else
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwAutoTeacingJigTime) > CMD_RETRY_AT)
				{
					 ResponseCheckCount++;

					if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
					{
       					if(m_AutoTeaching->bSTBTeaching  ==true)
							sprintf(SendBuf, "%s",FT_STB);
						else
						{
							if(m_nEqType == PORT_TYPE_STB_L)
								sprintf(SendBuf, "%s",AT_STB_L);
							else
								sprintf(SendBuf, "%s",AT_STB_R);
						}
					}
					else
					{
						if(m_nEqType == PORT_TYPE_STOCKER)
							sprintf(SendBuf, "%s",AT_STK);
						else
							sprintf(SendBuf, "%s",AT_EQ);
					}
//					 sprintf(SendBuf, "%s",AT);
				}
				else if(ResponseCheckCount >15)
				{
					nError = ERR_AUTOTEACHING_RESPONCE;
				}
            }
		break;

	//STB 작업시에만
	case N_AUTO_TEACHING_MOVE_UP_HOIST_STB:
		ADD_LOG("[AUTO_TEACHING_MOVE_UP_HOIST_STB]");
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_UP_TP2H_AT);
		if(nError == NO_ERR)
			m_nTransStep = N_AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB;
		break;

	case N_AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB:
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(false, HOIST_UP_STATE, N_AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB, N_AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB);
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_UP_TP2H_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{

           if(m_AutoTeaching->fDrive_Offset != 0.0)
			{
				// 현재 목표값 저장
				ADD_LOG("주행방향으로 Move :(%.3f)", m_AutoTeaching->fDrive_Offset);
				m_nTransStep = AUTO_TEACHING_MOVE_X_POS_CMD;    // 2. 주행 축 이동

			}
			else if(m_AutoTeaching->fSlide_Offset != 0.0)
			{
				// 현재 목표값 저장
				ADD_LOG("Slide방향으로 Move :(%.3f)", m_AutoTeaching->fSlide_Offset);
				m_nTransStep = N_AUTO_TEACHING_MOVE_Y_POS_CMD;    // 2. 슬라이드 이동
			}
			else
			{
				ADD_LOG("Hoist방향으로 Move");
				m_nTransStep = N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB;    // 2. 슬라이드 이동
            }
		}

		break;


	case N_AUTO_TEACHING_MOVE_X_POS_CMD:         //------------주행축 이동 ----------//   Limit 거리 제한 둘것.. 초기 위치로 부터(+- 알파.)
        if(nError == NO_ERR)
			nError = m_AutoTeaching->ATDrivingMove(m_nEqType);
		if(nError == NO_ERR)
		{
			if(m_AutoTeaching->fSlide_Offset == 0.0)
			{
				if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
					m_nTransStep = N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB;
				else
					m_nTransStep = N_AUTO_TEACHING_TEACHING_CMD;
            }
			else
				m_nTransStep = N_AUTO_TEACHING_MOVE_Y_POS_CMD;
        }
		break;

	case N_AUTO_TEACHING_MOVE_Y_POS_CMD:
		//Retry 여부 체크
		LimitRetryCheck(m_nEqType, m_pHWSet->m_pShiftAxis->GetCurrentPosition(), m_AutoTeaching->fSlide_Offset);
		nError=m_AutoTeaching->CheckSTBShiftLimitError(m_nEqType,(m_pHWSet->m_pShiftAxis->GetCurrentPosition() +  m_AutoTeaching->fSlide_Offset));
		if(nError ==NO_ERR)
		   nError =	Move_ShiftRotate_AutoTeaching(SHIFTROTATE_MOVE_TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = N_AUTO_TEACHING_MOVE_Y_POS_WAIT;
		break;
	case N_AUTO_TEACHING_MOVE_Y_POS_WAIT:        //이동 후 진동 감쇠기에 의해 수렴시간 동안 대기 이후 반복 수행 할 것.
		if(nError == NO_ERR)
			nError = Check_ShiftRotate_AutoTeaching(SHIFTROTATE_MOVE_TP_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
				m_nTransStep = N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB;
			else
				m_nTransStep = N_AUTO_TEACHING_TEACHING_CMD;
        }
		break;

    //STB 작업시에만
	case N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB:
		Sleep(500);
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(false, HOIST_DOWN_STATE, N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB, N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB);
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_DOWN_H2TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = N_AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB;
		break;
	 case N_AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB:
	  	if(nError == NO_ERR)
		   nError = Check_Sensor_AutoTeaching(false, HOIST_DOWN_STATE, N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB, N_AUTO_TEACHING_TEACHING_CMD);
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_DOWN_H2TP_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
			m_nTransStep = N_AUTO_TEACHING_TEACHING_CMD;    //추가 로직 수정
		break;


	 //3. ---------Hoist 축 이동
	case N_AUTO_TEACHING_Z_TEACHING_CMD:        // Z축 티칭은 한번만 하고 넘어간다   ---AT와 통신
				m_AutoTeaching->iCmd = CMD_INT;
				sprintf(SendBuf, "%s",ZT);
//				memmove(SendBuf, ZT, 3);
				ResponseCheckCount = 0;
				dwAutoTeacingJigTime =  timeGetTime();
				m_nTransStep = N_AUTO_TEACHING_Z_TEACHING_WAIT;
				ADD_LOG("Z-Teaching 실행함");	// 통신 이벤트로 넘어가므로 타이머 불필요
 		break;

	case N_AUTO_TEACHING_Z_TEACHING_WAIT:
		if(m_AutoTeaching->iCmd == CMD_RESPONSE)    //Timeout or 통신 Check부분 추가 필요. 초기부분에서 .
			{
				m_AutoTeaching->iCmd = CMD_INT;
				//센서 측은 Check하지 않음.
				ADD_LOG("Z-Teaching값 수신됨. Hoist 이동거리(상대값) : %f", m_AutoTeaching->fHoist);

				m_AutoTeaching->dHoist_Offset = m_AutoTeaching->fHoist;

				if(nError == NO_ERR)
				{
					if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
					{
						nError = m_AutoTeaching->CheckSTBHoistLimitError(m_nEqType,(m_dHoist_TeachingPos + m_AutoTeaching->fHoist + 3.0));
						if(nError == NO_ERR)
						{                       					
							m_dHoist_TeachingPos = m_dHoist_TeachingPos + m_AutoTeaching->fHoist + 3.0;  //STB 작업으로 추가함.
							ADD_LOG("명령값 : %f", m_dHoist_TeachingPos);
	//						if(m_nEqType == PORT_TYPE_STB_L)
	//							m_AutoTeaching->fSlide_Offset = m_AutoTeaching->dSTB_L_Offset - VISION_OFFSET_SHIFT;
	//						else
	//							m_AutoTeaching->fSlide_Offset = m_AutoTeaching->dSTB_R_Offset - VISION_OFFSET_SHIFT;//	m_AutoTeaching->fSlide_Offset = TEACHING_OFFSET_SHIFT_R - VISION_OFFSET_SHIFT;

							m_nTransStep = N_AUTO_TEACHING_MOVE_UP_HOIST_STB_Z;
						}
						// Auto Teaching Z-teaching 값이 N-Limit 범위 밖일 때, 95mm 까지 이동, 에러 미처리
						else if((m_dHoist_TeachingPos + m_AutoTeaching->fHoist + 3.0 < HOIST_MOVELIMIT_N_MIN_STB) &&
								(m_dHoist_TeachingPos + m_AutoTeaching->fHoist + 3.0 > m_dHoist_TeachingPos))
						{
							
							nError = NO_ERR;
							m_dHoist_TeachingPos = HOIST_MOVELIMIT_N_MIN_STB;
							ADD_LOG("95mm N_Limit 이내 명령-> 95mm 위치로 이동"); 
							ADD_LOG("명령값 : %f", m_dHoist_TeachingPos);
							m_nTransStep = N_AUTO_TEACHING_MOVE_UP_HOIST_STB_Z;	
						}
					}
					else
					{
                     	if(nError == NO_ERR)
							nError = Move_Hoist_AutoTeaching(HOIST_MOVE_TP_AT);
                        if(nError == NO_ERR)
							m_nTransStep = N_AUTO_TEACHING_CHECK_PUSH_HAND;
					}

                }

			}
			else
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwAutoTeacingJigTime) >CMD_RETRY)
				{
					 ResponseCheckCount++;
//					 dwAutoTeacingJigTime =  timeGetTime();
					 sprintf(SendBuf, "%s",ZT);
				}
				else if(ResponseCheckCount >10)
				{
					nError = ERR_AUTOTEACHING_RESPONCE;
				}
            }
		break;
    	//STB 작업시에만
	case N_AUTO_TEACHING_MOVE_UP_HOIST_STB_Z:
		Sleep(500);
		ADD_LOG("[AUTO_TEACHING_MOVE_UP_HOIST_STB_Z]");
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_UP_TP2H_AT);
		if(nError == NO_ERR)
			m_nTransStep = N_AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB_Z;
		break;

	case N_AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB_Z:
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(false, HOIST_UP_STATE, N_AUTO_TEACHING_MOVE_UP_HOIST_STB_Z, N_AUTO_TEACHING_MOVE_UP_HOIST_STB_Z);
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_UP_TP2H_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{
			m_nTransStep = N_AUTO_TEACHING_MOVE_Y_POS_CMD_Z;    // 2. 주행 축 이동
		}

		break;

	case N_AUTO_TEACHING_MOVE_Y_POS_CMD_Z:
		nError=m_AutoTeaching->CheckSTBShiftLimitError(m_nEqType,(m_pHWSet->m_pShiftAxis->GetCurrentPosition() +  m_AutoTeaching->fSlide_Offset));
		if(nError ==NO_ERR)
		   nError =	Move_ShiftRotate_AutoTeaching(SHIFTROTATE_MOVE_TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = N_AUTO_TEACHING_MOVE_Y_POS_WAIT_Z;
		break;
	case N_AUTO_TEACHING_MOVE_Y_POS_WAIT_Z:        //이동 후 진동 감쇠기에 의해 수렴시간 동안 대기 이후 반복 수행 할 것.
		if(nError == NO_ERR)
			nError = Check_ShiftRotate_AutoTeaching(SHIFTROTATE_MOVE_TP_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{
			m_nTransStep = N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z;
		}
		break;


	//STB 작업시에만
	case N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z:
		Sleep(500);
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(false, HOIST_DOWN_STATE, N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z, N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z);
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_DOWN_H2TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = N_AUTO_TEACHING_CHECK_PUSH_HAND;
		break;

	case N_AUTO_TEACHING_CHECK_PUSH_HAND:        // Hand Push센서 on될때까지 이동.
			if(nError == NO_ERR)
				nError = Check_Hoist_AutoTeaching(HOIST_MOVE_TP_AT,bNextStep);
			if((nError == NO_ERR) &&(bNextStep ==true))
			{
				nError = CheckNMoveTeachingPoint_AutoTeaching();

				if(nError == NO_ERR)
					m_nTransStep = N_AUTO_TEACHING_SAVE_CURRENT_POS_CMD;
			}
		break;

	//4. ---------Teaching을 Memory에 저장.
	case N_AUTO_TEACHING_SAVE_CURRENT_POS_CMD:        // 티칭값 저장.
	//EQ Port와 STB Port와 구분하여 저장할 것.(STB는 주행축도 저장.)
			//EQPort인경우, STB Port인 경우
			//if(m_AutoTeaching->Total_fDrive_Offset != 0.0)
			//{
				if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
				{
					m_AutoTeaching->QRDrvingTrigger(m_nEqType);
					Sleep(500);
					if(!m_AutoTeaching->QRDataCheck(m_nEqType,m_pExecuteInfo->ExecutePositionInfo.CurrStation))
					{
					   nError =  ERR_AUTOTEACHING_DRIVING_LIMIT;
					}

				}
		   //}
			ADD_LOG("AUTO_TEACHING_SAVE_CURRENT_POS_CMD");	// 통신 이벤트로 넘어가므로 타이머 불필요
			m_AutoTeaching->ATSave_NullType_Data(m_pExecuteInfo->ExecutePositionInfo.CurrStation,m_nEqType,m_AutoTeaching->Total_fDrive_Offset,m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_pHWSet->m_pShiftAxis->GetCurrentPosition(),m_defualtparam->AutoTeachingIP);
			m_nTransStep = N_AUTO_TEACHING_CHECK_HAND_CLOSE_OPEN; //soming : Hoist 위아래 seq 추가.
		   //	m_nTransStep = AUTO_TEACHING_CHECK_HAND_CLOSE;
		break;


	 //////////////////////////////////////////////////////////////////////
	//5.Auto Teaching 원점 복귀
	case N_AUTO_TEACHING_CHECK_HAND_CLOSE_OPEN:
			ADD_LOG("AUTO_TEACHING_CHECK_HAND_CLOSE_OPEN");	// 통신 이벤트로 넘어가므로 타이머 불필요
			nError = m_pHWSet->m_pHand->ManualClose();
			Sleep(707);

			if(nError == NO_ERR)
				 m_nTransStep = N_AUTO_TEACHING_MOVE_COMP_UP_HOIST;
		break;


	case N_AUTO_TEACHING_MOVE_COMP_UP_HOIST:
		ADD_LOG("[AUTO_TEACHING_MOVE_COMP_UP_HOIST");
//		if(nError == NO_ERR)
//			nError = Check_Sensor_AutoTeaching(true, HOIST_DOWN_STATE, AUTO_TEACHING_MOVE_COMP_UP_HOIST, AUTO_TEACHING_MOVE_COMP_UP_HOIST);
//		if((bLookdown_Detected == true) || (m_nTransStep != AUTO_TEACHING_MOVE_INIT_DOWN_HOIST))
//			break;
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_UP_TP2H_AT);
		if(nError == NO_ERR)
			m_nTransStep = N_AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST;

		 Sleep(1000);
		break;
	case N_AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST:
//		ADD_LOG("[AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST");
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(false, HOIST_UP_STATE, N_AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST, N_AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST);
//		if((bLookdown_Detected == true) || (m_nTransStep != AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST))
//			break;
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_UP_TP2H_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{
		/*
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
				m_nTransStep = AUTO_TEACHING_U_MOVE_DOWN_HOIST_FAST;     //case 10단계 동작 시작
			else
		*/
				m_nTransStep = N_AUTO_TEACHING_MOVE_COMP_HOME_SHIFTROTATE;
		}

		break;

	case N_AUTO_TEACHING_MOVE_COMP_HOME_SHIFTROTATE:
		ADD_LOG("[AUTO_TEACHING_MOVE_COMP_HOME_SHIFTROTATE");
		if(nError ==NO_ERR)
		   nError =	Move_ShiftRotate_AutoTeaching(SHIFTROTATE_TP2H_AT);
		if(nError == NO_ERR)
			m_nTransStep= N_AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_HOME_SHIFTROTATE;
		break;
	case N_AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_HOME_SHIFTROTATE:
//		ADD_LOG("[AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_HOME_SHIFTROTATE");
		 if(nError == NO_ERR)
			nError = Check_ShiftRotate_AutoTeaching(SHIFTROTATE_TP2H_AT,bNextStep);
		 if((nError == NO_ERR) &&(bNextStep ==true))
			m_nTransStep = N_AUTO_TEACHING_CHECK_ANTIDROP;
		break;
	case N_AUTO_TEACHING_CHECK_ANTIDROP:        // Shutter Close + Outttigger Home
		ADD_LOG("[AUTO_TEACHING_CHECK_ANTIDROP");
		nError = m_pHWSet->m_pShutter->SendCloseCmd();
		if(nError == NO_ERR)
		{
			ADD_LOG("[AUTO_TEACHING_CHECK_ANTIDROP_2 m_pRailSupport] %f", m_dShift_TeachingPos);
			while ((nError = m_pHWSet->m_pShutter->CheckComplete()) == NEXT_CHECK)
			{
				Sleep(10);
			}
		}

		if(nError == NO_ERR)
			m_nTransStep = AUTO_TEACHING_COMPLETE;
		break;

	//6. Auto Teaching 완료
	case N_AUTO_TEACHING_COMPLETE:
	ADD_LOG("[AUTO_TEACHING_COMPLETE");
		if(nError == NO_ERR)
		{
			sprintf(SendBuf, "%s",END);
			m_AutoTeaching->ServerClose();  //AT와 통신 종료
		}
		if(nError == NO_ERR)
		{
			nError = Check_Complete_AutoTeacing();
		}
		break;
    }



	///Auto Teaching Jig에 명령주는 부분 추가할것.
	if(nError == NO_ERR)
	{
		if(memcmp(SendBuf, CmpBuf, 10) != 0)
		{
			ADD_LOG("AutoTeaching_SendCommand %s 명령", SendBuf);

			m_AutoTeaching->AutoTeaching_SendCommand(SendBuf);
			dwAutoTeacingJigTime =  timeGetTime();

		}
		else if(OHTUTILITY::PreventOverflowGetTimeMs(dwAutoTeacingJigTime) > CMD_RETRY_ST)
		{
			ADD_LOG("AutoTeaching_SendCommand Status");
			m_AutoTeaching->AutoTeaching_SendCommand(ST);
			dwAutoTeacingJigTime =  timeGetTime();
		}
	}


	if(nError != NO_ERR)
	{
        ADD_LOG("AUTO_TEACHING_STEP:%d,nError:%d", m_nTransStep, nError);
		m_AutoTeaching->ServerClose();  //에러발생시 통신연결 해제. AT와 통신 종료

		m_StatusSet.uError = nError;
		m_StatusSet.byStatus = STS_ERROR;
	}

	return nError;
}

int TransControl::Cmd_AutoTeaching()
{
	int nError = 0;

	bool bNextStep = false;
	static int iCount = 0;
	static int ResponseCheckCount = 0;

	DWORD dwCurrTime = timeGetTime();
	static DWORD dwAutoTeacingJigTime =  timeGetTime();  //연결 확인이 되었을시, 일정 시간 확인하여 명령 주는 부분.
//	bool bAutoTeachingCmd = false;   //VHL -> Jig 명령이 있을 경우 True

	char SendBuf[20] = {0,};
	char CmpBuf[20] = {0,};

	///< 상시 0) Auto Teaching Jig와 통신으로 Data를 갱신하는 부분.
	if((m_AutoTeaching->IsConnect()) && (m_nTransStep > AUTO_TEACHING_READY))
	{
		nError = m_AutoTeaching->UpdateATData();   //일정 시간동안 응답이 없을 경우 Error 발생할 것.

	}

	///< 상시 0) Auto Teaching Jig의 IP정보가 맞는지 Check하는 부분(NullTypeCheck에서 확인됨)
	if(m_AutoTeaching->IsCheckIPCompare(m_defualtparam->AutoTeachingIP) ==false)
	{
   		nError = ERR_AUTOTEACHING_IP_DISMATCH;
	}


	switch(m_nTransStep)
	{
	 //기본값 Hoist 확인 (5mm보다 커야됨. 등등.)       // 티칭위치보다 20mm 이전값 저장(음수인경우는 에러 처리).
	 //CheckAutoTeachningReady() : 현재 Foup 들고있는지 확인, AutoTeaching Jig와 통신유무 확인 등
	 ///< 0.각종 VHL 내부 처리 초기화 처리 단계.
	 case AUTO_TEACHING_READY:
		if(nError == NO_ERR)
			nError = m_AutoTeaching->ServerBind(AUTO_TEACH_MY_SERVER_PORT, m_defualtparam->AutoTeachingIP);  //AT와 통신 시작
		if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsCloseOutputOn() == true))
			nError = m_pHWSet->m_pHand->IO_Init();
		if(nError == NO_ERR)
			nError = ReadyTrans_AutoTeaching();
		if(nError == NO_ERR)
			nError = Check_State_AutoTeaching();
		if(nError == NO_ERR)
		{
			m_AutoTeaching->iCmd = CMD_INT;
            sprintf(SendBuf, "%s",BGE);
			m_AutoTeaching->SetAutoTeachingJigResponse();
			m_nTransStep = AUTO_TEACHING_CHECK_BEGIN;

			dwAutoTeacingJigTime =  timeGetTime();
			iCount =0;
			ResponseCheckCount = 0;
		}
		break;
     ///< 1.Auto Teaching Jig 초기화 명령 단계.
	 case  AUTO_TEACHING_CHECK_BEGIN:
		if(m_AutoTeaching->iCmd == CMD_READY)
		{
			ADD_LOG("Reset 명령");
			sprintf(SendBuf, "%s",RST);
			m_nTransStep = AUTO_TEACHING_MOVE_INIT_SHIFTROTATE;


            ResponseCheckCount = 0;
			dwAutoTeacingJigTime =  timeGetTime();

		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwAutoTeacingJigTime) >CMD_RETRY)
			{
				 ResponseCheckCount++;
				 dwAutoTeacingJigTime =  timeGetTime();
             	 sprintf(SendBuf, "%s",BGE);
			}
			else if(ResponseCheckCount >3)
			{
              	nError = ERR_AUTOTEACHING_RESPONCE;
            }
		}
		break;

	 ///< 1.Default Teaching 위치로 이동하는 단계(VHL Map File 참조하여 Default Teaching 위치 이동)
	 case AUTO_TEACHING_MOVE_INIT_SHIFTROTATE:
		if(nError ==NO_ERR)
		   nError =	Move_ShiftRotate_AutoTeaching(SHIFTROTATE_H2TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_SHIFTROTATE;
		break;
	 case AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_SHIFTROTATE:
		 if(nError == NO_ERR)
			nError = Check_ShiftRotate_AutoTeaching(SHIFTROTATE_H2TP_AT,bNextStep);
		 if((nError == NO_ERR) &&(bNextStep ==true))
			m_nTransStep = AUTO_TEACHING_MOVE_INIT_DOWN_HOIST;
		break;
	 case AUTO_TEACHING_MOVE_INIT_DOWN_HOIST:
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(true, HOIST_DOWN_STATE, AUTO_TEACHING_MOVE_INIT_DOWN_HOIST, AUTO_TEACHING_MOVE_INIT_DOWN_HOIST);
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) || (m_nTransStep != AUTO_TEACHING_MOVE_INIT_DOWN_HOIST))
			break;
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_DOWN_H2TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_DOWN_HOIST;
		break;
	 case AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_DOWN_HOIST:
		if(nError == NO_ERR)
		{
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
				nError = Check_Sensor_AutoTeaching(true, HOIST_DOWN_STATE, AUTO_TEACHING_MOVE_INIT_DOWN_HOIST, AUTO_TEACHING_TEACHING_CMD);
			else
				nError = Check_Sensor_AutoTeaching(true, HOIST_DOWN_STATE, AUTO_TEACHING_MOVE_INIT_DOWN_HOIST, AUTO_TEACHING_F_TEACHING_CMD);
        }
		if((bLookdown_Detected == true) || (m_bOSL_Detected==true) || (m_bHandPIOGoSignal_Detected==true) ||(m_nTransStep != AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_DOWN_HOIST))
			break;
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_DOWN_H2TP_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
				m_nTransStep = AUTO_TEACHING_TEACHING_CMD;   //m_nTransStep = AUTO_TEACHING_P_TEACHING_CMD;    //추가 로직 수정
			else
				m_nTransStep = AUTO_TEACHING_F_TEACHING_CMD;    //추가 로직 수정
		}
		break;

	 ///< 2.FOcus 확인(Hoist 위치가 실제 Teaching 위치로부터 20mm 떠있어야됨. Focus Teaching 1회 (FT) 보정 시도)
	 case AUTO_TEACHING_F_TEACHING_CMD:
			m_AutoTeaching->iCmd = CMD_INT;

			if(m_nEqType == PORT_TYPE_STOCKER)
	            sprintf(SendBuf, "%s",FT_STK);
			else
				sprintf(SendBuf, "%s",FT_EQ);

			ResponseCheckCount = 0;
			dwAutoTeacingJigTime =  timeGetTime();

			m_nTransStep = AUTO_TEACHING_F_TEACHING_WAIT;
			ADD_LOG("Focus 명령");
		break;

	case AUTO_TEACHING_F_TEACHING_WAIT:
		if(m_AutoTeaching->iCmd == CMD_RESPONSE)
			{
				m_AutoTeaching->iCmd = CMD_INT;
				m_AutoTeaching->dHoist_Offset = m_AutoTeaching->fHoist;
				if(m_AutoTeaching->fHoist == 0.0)    // FT : Hoist 이동 완료
				{
					m_nTransStep = AUTO_TEACHING_TEACHING_CMD;
					ADD_LOG("Focus 현위치 유지");
				}
				else    // FT : Hoist 이동 필요
				{
					m_nTransStep = AUTO_TEACHING_MOVE_FOCUS_CMD;
					ADD_LOG("Focus 이동 %f",m_AutoTeaching->dHoist_Offset);

                }
			}
			else
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwAutoTeacingJigTime) >CMD_RETRY)
				{
					 ResponseCheckCount++;
//					 dwAutoTeacingJigTime =  timeGetTime();
					 if(m_nEqType == PORT_TYPE_STOCKER)
						sprintf(SendBuf, "%s",FT_STK);
					 else
						sprintf(SendBuf, "%s",FT_EQ);
				}
				else if(ResponseCheckCount >5)
				{
					nError = ERR_AUTOTEACHING_RESPONCE;
				}
            }
		break;

	case AUTO_TEACHING_MOVE_FOCUS_CMD:
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_MOVE_TP_AT);
		if(nError == NO_ERR)
		{
			ADD_LOG("AUTO_TEACHING_MOVE_FOCUS_CMD");
			m_nTransStep = AUTO_TEACHING_MOVE_FOCUS_WAIT;
        }
		break;

	case AUTO_TEACHING_MOVE_FOCUS_WAIT:
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_MOVE_TP_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
			{
                ADD_LOG("AUTO_TEACHING_MOVE_FOCUS_WAIT");
				m_nTransStep = AUTO_TEACHING_TEACHING_CMD;
			}
		break;


	//2. --------------AT 반복 구간 ---시작-------------------//
	   // AT 명령 이후 과정 정리 필요
		/*
		1. AT 명령 전송 후 Drive, Slide 방향 보정값 수신
		   - 수신 데이터를 통해 이동/완료 여부 판단
		2. Rail Support 이동(주행축 이동 있을 시에만)
		3. Dirve 방향 보정값 이동
		4. Slide 보정값 이동
		(1번에서 완료 판단 여부에 따라 2 ~ 6번 반복 수행)
		*/
   ///< 3.Auto Teaching 확인
	case AUTO_TEACHING_TEACHING_CMD:         // Auto Teaching 반복 (AT)   ---AT와 통신
			m_AutoTeaching->iCmd = CMD_INT;

			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
			{
				if(m_AutoTeaching->bSTBTeaching  ==true)
					sprintf(SendBuf, "%s",FT_STB);
				else
				{
					if(m_nEqType == PORT_TYPE_STB_L)
						sprintf(SendBuf, "%s",AT_STB_L);
					else
                        sprintf(SendBuf, "%s",AT_STB_R);
				}
			}
			else
			{
				if(m_nEqType == PORT_TYPE_STOCKER)
					sprintf(SendBuf, "%s",AT_STK);
				else
					sprintf(SendBuf, "%s",AT_EQ);
			}

//			sprintf(SendBuf, "%s",AT);      // 오토티칭 명령을 전송

			ResponseCheckCount = 0;
			dwAutoTeacingJigTime =  timeGetTime();
			m_nTransStep = AUTO_TEACHING_TEACHING_WAIT;	// 통신 수신준비
    		break;

	case AUTO_TEACHING_TEACHING_WAIT:       // 티칭 결과값이 넘어오면, 오차범위내에 들어갔는지 확인하고,
		if(m_AutoTeaching->iCmd == CMD_RESPONSE)
			{
				iCount++;
				m_AutoTeaching->iCmd = CMD_INT;
				//fSlide,fDrive Auto teacing Jig 에서 받아오는 값.

				m_AutoTeaching->bSTBTeaching  = false;  //STB

				m_AutoTeaching->fSlide_Offset = m_AutoTeaching->fSlide;
				m_AutoTeaching->fDrive_Offset = m_AutoTeaching->fDrive;


				if((m_AutoTeaching->fSlide == 0.0) && (m_AutoTeaching->fDrive == 0.0) && (m_AutoTeaching->fHoist == 0.0))	// 티칭 완료
				{
					iCount = 0;
					m_nTransStep = AUTO_TEACHING_Z_TEACHING_CMD;
					ADD_LOG("티칭 성공!");
				}
				else if(iCount > 10)
				{
					iCount = 0;
					nError = ERR_AUTOTEACHING_TEACHING_FAIL;     //에러 발생
					ADD_LOG("티칭 실패!");
				}
				else
				{

					if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
					{
						m_dHoist_TeachingPos = m_dHoist_TeachingPos + m_AutoTeaching->fHoist;  //STB 작업으로 추가함.
					   	m_nTransStep = AUTO_TEACHING_MOVE_UP_HOIST_STB;    // 2. 주행 축 이동
					}
					else
					{
						if(m_AutoTeaching->fDrive != 0.0)
						{
							// 현재 목표값 저장
							ADD_LOG("[No %02d] 주행방향으로 Move :(%.3f)", iCount, m_AutoTeaching->fDrive);
							m_nTransStep = AUTO_TEACHING_MOVE_X_POS_CMD;    // 2. 주행 축 이동

						}
						else if(m_AutoTeaching->fSlide != 0.0)
						{
							// 현재 목표값 저장


							ADD_LOG("[No %02d] Slide방향으로 Move :(%.3f)", iCount,  m_AutoTeaching->fSlide);
							m_nTransStep = AUTO_TEACHING_MOVE_Y_POS_CMD;    // 2. 슬라이드 이동
						}
					}
				}
			}
			else
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwAutoTeacingJigTime) > CMD_RETRY_AT)
				{
					 ResponseCheckCount++;

					if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
					{
       					if(m_AutoTeaching->bSTBTeaching  ==true)
							sprintf(SendBuf, "%s",FT_STB);
						else
						{
							if(m_nEqType == PORT_TYPE_STB_L)
								sprintf(SendBuf, "%s",AT_STB_L);
							else
								sprintf(SendBuf, "%s",AT_STB_R);
						}
					}
					else
					{
						if(m_nEqType == PORT_TYPE_STOCKER)
							sprintf(SendBuf, "%s",AT_STK);
						else
							sprintf(SendBuf, "%s",AT_EQ);
					}
//					 sprintf(SendBuf, "%s",AT);
				}
				else if(ResponseCheckCount >15)
				{
					nError = ERR_AUTOTEACHING_RESPONCE;
				}
            }
		break;

	//STB 작업시에만
	case AUTO_TEACHING_MOVE_UP_HOIST_STB:
		ADD_LOG("[AUTO_TEACHING_MOVE_UP_HOIST_STB]");
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_UP_TP2H_AT);
		if(nError == NO_ERR)
			m_nTransStep = AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB;
		break;

	case AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB:
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(false, HOIST_UP_STATE, AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB, AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB);
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_UP_TP2H_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{

           if(m_AutoTeaching->fDrive_Offset != 0.0)
			{
				// 현재 목표값 저장
				ADD_LOG("주행방향으로 Move :(%.3f)", m_AutoTeaching->fDrive_Offset);
				m_nTransStep = AUTO_TEACHING_MOVE_X_POS_CMD;    // 2. 주행 축 이동

			}
			else if(m_AutoTeaching->fSlide_Offset != 0.0)
			{
				// 현재 목표값 저장
				ADD_LOG("Slide방향으로 Move :(%.3f)", m_AutoTeaching->fSlide_Offset);
				m_nTransStep = AUTO_TEACHING_MOVE_Y_POS_CMD;    // 2. 슬라이드 이동
			}
			else
			{
				ADD_LOG("Hoist방향으로 Move");
				m_nTransStep = AUTO_TEACHING_MOVE_DOWN_HOIST_STB;    // 2. 슬라이드 이동
            }
		}

		break;

    //------------주행축 이동 ----------//   Limit 거리 제한 둘것.. 초기 위치로 부터(+- 알파.)
	case AUTO_TEACHING_MOVE_X_POS_CMD:
        if(nError == NO_ERR)
			nError = m_AutoTeaching->ATDrivingMove(m_nEqType);
		if(nError == NO_ERR)
		{
			if(m_AutoTeaching->fSlide_Offset == 0.0)
			{
				if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
					m_nTransStep = AUTO_TEACHING_MOVE_DOWN_HOIST_STB;
				else
					m_nTransStep = AUTO_TEACHING_TEACHING_CMD;
            }
			else
				m_nTransStep = AUTO_TEACHING_MOVE_Y_POS_CMD;
        }
		break;
	 //Slide축 이동
	case AUTO_TEACHING_MOVE_Y_POS_CMD:
		//Retry 여부 체크
		LimitRetryCheck(m_nEqType, m_pHWSet->m_pShiftAxis->GetCurrentPosition(), m_AutoTeaching->fSlide_Offset);
		nError = m_AutoTeaching->CheckSTBShiftLimitError(m_nEqType,(m_pHWSet->m_pShiftAxis->GetCurrentPosition() +  m_AutoTeaching->fSlide_Offset));
		if(nError ==NO_ERR)
		   nError =	Move_ShiftRotate_AutoTeaching(SHIFTROTATE_MOVE_TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = AUTO_TEACHING_MOVE_Y_POS_WAIT;
		break;
	case AUTO_TEACHING_MOVE_Y_POS_WAIT:        //이동 후 진동 감쇠기에 의해 수렴시간 동안 대기 이후 반복 수행 할 것.
		if(nError == NO_ERR)
			nError = Check_ShiftRotate_AutoTeaching(SHIFTROTATE_MOVE_TP_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
				m_nTransStep = AUTO_TEACHING_MOVE_DOWN_HOIST_STB;
			else
				m_nTransStep = AUTO_TEACHING_TEACHING_CMD;
        }
		break;

    //STB 작업시에만
	case AUTO_TEACHING_MOVE_DOWN_HOIST_STB:
		Sleep(700);
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(false, HOIST_DOWN_STATE, AUTO_TEACHING_MOVE_DOWN_HOIST_STB, AUTO_TEACHING_MOVE_DOWN_HOIST_STB);
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_DOWN_H2TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB;
		break;
	 case AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB:
	  	if(nError == NO_ERR)
		   nError = Check_Sensor_AutoTeaching(false, HOIST_DOWN_STATE, AUTO_TEACHING_MOVE_DOWN_HOIST_STB, AUTO_TEACHING_TEACHING_CMD);
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_DOWN_H2TP_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
			m_nTransStep = AUTO_TEACHING_TEACHING_CMD;    //추가 로직 수정
		break;


	 ///< 4.Auto Teaching Hoist축 최종 위치 확인
	case AUTO_TEACHING_Z_TEACHING_CMD:        // Z축 티칭은 한번만 하고 넘어간다   ---AT와 통신
				m_AutoTeaching->iCmd = CMD_INT;
				sprintf(SendBuf, "%s",ZT);
//				memmove(SendBuf, ZT, 3);
				ResponseCheckCount = 0;
				dwAutoTeacingJigTime =  timeGetTime();
				m_nTransStep = AUTO_TEACHING_Z_TEACHING_WAIT;
				ADD_LOG("Z-Teaching 실행함");	// 통신 이벤트로 넘어가므로 타이머 불필요
 		break;

	case AUTO_TEACHING_Z_TEACHING_WAIT:
		if(m_AutoTeaching->iCmd == CMD_RESPONSE)    //Timeout or 통신 Check부분 추가 필요. 초기부분에서 .
			{
				m_AutoTeaching->iCmd = CMD_INT;
				//센서 측은 Check하지 않음.
				ADD_LOG("Z-Teaching값 수신됨. Hoist 이동거리(상대값) : %f", m_AutoTeaching->fHoist);

				m_AutoTeaching->dHoist_Offset = m_AutoTeaching->fHoist;

				if(nError == NO_ERR)
				{
					if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
					{
						nError = m_AutoTeaching->CheckSTBHoistLimitError(m_nEqType,(m_dHoist_TeachingPos + m_AutoTeaching->fHoist + 3.0));
						if(nError == NO_ERR)
						{
							m_dHoist_TeachingPos = m_dHoist_TeachingPos + m_AutoTeaching->fHoist + 3.0;  //STB 작업으로 추가함.
							ADD_LOG("명령값 : %f", m_dHoist_TeachingPos);
							m_nTransStep = AUTO_TEACHING_MOVE_UP_HOIST_STB_Z;
						}
						else if((m_dHoist_TeachingPos + m_AutoTeaching->fHoist + 3.0 < HOIST_MOVELIMIT_N_MIN_STB) &&
								(m_dHoist_TeachingPos + m_AutoTeaching->fHoist + 3.0 > m_dHoist_TeachingPos))
						{
							
							nError = NO_ERR;
							m_dHoist_TeachingPos = HOIST_MOVELIMIT_N_MIN_STB;
							ADD_LOG("95mm N_Limit 이내 명령-> 95mm 위치로 이동"); 
							ADD_LOG("명령값 : %f", m_dHoist_TeachingPos);
							m_nTransStep = AUTO_TEACHING_MOVE_UP_HOIST_STB_Z;
						}
					}
					else
					{

						if(nError == NO_ERR)
							nError = Move_Hoist_AutoTeaching(HOIST_MOVE_TP_AT);
                        if(nError == NO_ERR)
							m_nTransStep = AUTO_TEACHING_CHECK_PUSH_HAND;
					}

                }

			}
			else
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwAutoTeacingJigTime) >CMD_RETRY)
				{
					 ResponseCheckCount++;
//					 dwAutoTeacingJigTime =  timeGetTime();
					 sprintf(SendBuf, "%s",ZT);
				}
				else if(ResponseCheckCount >10)
				{
					nError = ERR_AUTOTEACHING_RESPONCE;
				}
            }
		break;
    	//STB 작업시에만
	case AUTO_TEACHING_MOVE_UP_HOIST_STB_Z:
		Sleep(500);
		ADD_LOG("[AUTO_TEACHING_MOVE_UP_HOIST_STB_Z]");
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_UP_TP2H_AT);
		if(nError == NO_ERR)
			m_nTransStep = AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB_Z;
		break;

	case AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB_Z:
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(false, HOIST_UP_STATE, AUTO_TEACHING_MOVE_UP_HOIST_STB_Z, AUTO_TEACHING_MOVE_UP_HOIST_STB_Z);
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_UP_TP2H_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{
			m_nTransStep = AUTO_TEACHING_MOVE_Y_POS_CMD_Z;    // 2. 주행 축 이동
		}

		break;

	case AUTO_TEACHING_MOVE_Y_POS_CMD_Z:
		nError=m_AutoTeaching->CheckSTBShiftLimitError(m_nEqType,(m_pHWSet->m_pShiftAxis->GetCurrentPosition() +  m_AutoTeaching->fSlide_Offset));
		if(nError ==NO_ERR)
		   nError =	Move_ShiftRotate_AutoTeaching(SHIFTROTATE_MOVE_TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = AUTO_TEACHING_MOVE_Y_POS_WAIT_Z;
		break;
	case AUTO_TEACHING_MOVE_Y_POS_WAIT_Z:        //이동 후 진동 감쇠기에 의해 수렴시간 동안 대기 이후 반복 수행 할 것.
		if(nError == NO_ERR)
			nError = Check_ShiftRotate_AutoTeaching(SHIFTROTATE_MOVE_TP_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{
			m_nTransStep = AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z;
		}
		break;


	//STB 작업시에만
	case AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z:
		Sleep(500);
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(false, HOIST_DOWN_STATE, AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z, AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z);
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_DOWN_H2TP_AT);
		if(nError == NO_ERR)
			m_nTransStep = AUTO_TEACHING_CHECK_PUSH_HAND;
		break;

	case AUTO_TEACHING_CHECK_PUSH_HAND:        // Hand Push센서 on될때까지 이동.
			if(nError == NO_ERR)
				nError = Check_Hoist_AutoTeaching(HOIST_MOVE_TP_AT,bNextStep);
			if((nError == NO_ERR) &&(bNextStep ==true))
			{
				nError = CheckNMoveTeachingPoint_AutoTeaching();

				if(nError == NO_ERR)
					m_nTransStep = AUTO_TEACHING_SAVE_CURRENT_POS_CMD;
			}
		break;

	//5. ---------Teaching을 Memory에 저장.
	case AUTO_TEACHING_SAVE_CURRENT_POS_CMD:        // 티칭값 저장.
	//EQ Port와 STB Port와 구분하여 저장할 것.(STB는 주행축도 저장.)
			//EQPort인경우, STB Port인 경우
			ADD_LOG("AUTO_TEACHING_SAVE_CURRENT_POS_CMD");	// 통신 이벤트로 넘어가므로 타이머 불필요
			//if(m_AutoTeaching->Total_fDrive_Offset != 0.0)
			//{
				if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
				{
					m_AutoTeaching->QRDrvingTrigger(m_nEqType);
					Sleep(500);
					if(!m_AutoTeaching->QRDataCheck(m_nEqType,m_pExecuteInfo->ExecutePositionInfo.CurrStation))
					{
					   nError =  ERR_AUTOTEACHING_DRIVING_LIMIT;
					}

				}
			//}

			if(nError == NO_ERR)
				m_AutoTeaching->ATSave_Current_Pos(m_pExecuteInfo->ExecutePositionInfo.CurrStation,m_nEqType,m_AutoTeaching->Total_fDrive_Offset,m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_pHWSet->m_pShiftAxis->GetCurrentPosition());
			if(nError == NO_ERR)
				m_nTransStep = AUTO_TEACHING_CHECK_HAND_CLOSE_OPEN; //soming : Hoist 위아래 seq 추가.
		   //	m_nTransStep = AUTO_TEACHING_CHECK_HAND_CLOSE;
		break;


	 //////////////////////////////////////////////////////////////////////
	//5.Auto Teaching 원점 복귀
	case AUTO_TEACHING_CHECK_HAND_CLOSE_OPEN:
			ADD_LOG("AUTO_TEACHING_CHECK_HAND_CLOSE_OPEN");	// 통신 이벤트로 넘어가므로 타이머 불필요
			nError = m_pHWSet->m_pHand->ManualClose();
			Sleep(707);

			if(nError == NO_ERR)
				 m_nTransStep = AUTO_TEACHING_MOVE_COMP_UP_HOIST;
		break;


	case AUTO_TEACHING_MOVE_COMP_UP_HOIST:
//		ADD_LOG("[AUTO_TEACHING_MOVE_COMP_UP_HOIST");
//		if(nError == NO_ERR)
//			nError = Check_Sensor_AutoTeaching(true, HOIST_DOWN_STATE, AUTO_TEACHING_MOVE_COMP_UP_HOIST, AUTO_TEACHING_MOVE_COMP_UP_HOIST);
//		if((bLookdown_Detected == true) || (m_nTransStep != AUTO_TEACHING_MOVE_INIT_DOWN_HOIST))
//			break;
		if(nError == NO_ERR)
			nError = Move_Hoist_AutoTeaching(HOIST_UP_TP2H_AT);
		if(nError == NO_ERR)
			m_nTransStep = AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST;

		 Sleep(1000);
		break;
	case AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST:
//		ADD_LOG("[AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST");
		if(nError == NO_ERR)
			nError = Check_Sensor_AutoTeaching(false, HOIST_UP_STATE, AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST, AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST);
//		if((bLookdown_Detected == true) || (m_nTransStep != AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST))
//			break;
		if(nError == NO_ERR)
			nError = Check_Hoist_AutoTeaching(HOIST_UP_TP2H_AT,bNextStep);
		if((nError == NO_ERR) &&(bNextStep ==true))
		{
		/*
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
				m_nTransStep = AUTO_TEACHING_U_MOVE_DOWN_HOIST_FAST;     //case 10단계 동작 시작
			else
		*/
				m_nTransStep = AUTO_TEACHING_MOVE_COMP_HOME_SHIFTROTATE;
		}

		break;

	case AUTO_TEACHING_MOVE_COMP_HOME_SHIFTROTATE:
		ADD_LOG("[AUTO_TEACHING_MOVE_COMP_HOME_SHIFTROTATE");
		if(nError ==NO_ERR)
		   nError =	Move_ShiftRotate_AutoTeaching(SHIFTROTATE_TP2H_AT);
		if(nError == NO_ERR)
			m_nTransStep= AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_HOME_SHIFTROTATE;
		break;
	case AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_HOME_SHIFTROTATE:
//	ADD_LOG("[AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_HOME_SHIFTROTATE");
		 if(nError == NO_ERR)
			nError = Check_ShiftRotate_AutoTeaching(SHIFTROTATE_TP2H_AT,bNextStep);
		 if((nError == NO_ERR) &&(bNextStep ==true))
			m_nTransStep = AUTO_TEACHING_CHECK_ANTIDROP;
		break;
	case AUTO_TEACHING_CHECK_ANTIDROP:        // Shutter Close + Outttigger Home
	ADD_LOG("[AUTO_TEACHING_CHECK_ANTIDROP");
		nError = m_pHWSet->m_pShutter->SendCloseCmd();
		if(nError == NO_ERR)
		{

			ADD_LOG("[AUTO_TEACHING_CHECK_ANTIDROP_2] %f", m_dShift_TeachingPos);
			while ((nError = m_pHWSet->m_pShutter->CheckComplete()) == NEXT_CHECK)
			{
				Sleep(10);
			}


		}

		if(nError == NO_ERR)
			m_nTransStep = AUTO_TEACHING_COMPLETE;
		break;

	//6. Auto Teaching 완료
	case AUTO_TEACHING_COMPLETE:
	ADD_LOG("[AUTO_TEACHING_COMPLETE");
		if(nError == NO_ERR)
		{
			sprintf(SendBuf, "%s",END);
			m_AutoTeaching->ServerClose();  //AT와 통신 종료
		}
		if(nError == NO_ERR)
		{
			nError = Check_Complete_AutoTeacing();
		}
		break;
    }



	///Auto Teaching Jig에 명령주는 부분 추가할것.
	if(nError == NO_ERR)
	{
		if(memcmp(SendBuf, CmpBuf, 10) != 0)
		{
			ADD_LOG("AutoTeaching_SendCommand %s 명령", SendBuf);

			m_AutoTeaching->AutoTeaching_SendCommand(SendBuf);
			dwAutoTeacingJigTime =  timeGetTime();

		}
		else if(OHTUTILITY::PreventOverflowGetTimeMs(dwAutoTeacingJigTime) > CMD_RETRY_ST)
		{
			ADD_LOG("AutoTeaching_SendCommand Status");
			m_AutoTeaching->AutoTeaching_SendCommand(ST);
			dwAutoTeacingJigTime =  timeGetTime();
		}
	}


	if(nError != NO_ERR)
	{
        ADD_LOG("AUTO_TEACHING_STEP:%d,nError:%d", m_nTransStep, nError);
		m_AutoTeaching->ServerClose();  //에러발생시 통신연결 해제. AT와 통신 종료

		m_StatusSet.uError = nError;
		m_StatusSet.byStatus = STS_ERROR;
	}

    return nError;
}





/**
 @brief   Hoist origin을 자동으로 set해주는 함수
 @author  Kunjin
 @date    2014.12.12
 @param
 @return
*/
int TransControl::Cmd_AutoSetHome(int &nTransStep, bool bNoCheckFlag)
{
	int nError = 0;
//	bool bOscillation = m_pHWSet->m_pOcsillation->IsDetect();
	bool bHoistHome = m_pHWSet->m_pHoistAxis->IsHoistHome();

	static DWORD dwTimeOfAutoSetHome = timeGetTime();

	if(m_pHWSet->m_pHoistAxis->GetCurrentPosition() < HOIST_MOVELIMIT_N)
	{
		ADD_LOG("[AutoSet] Err_Hoist Out Of N Limit");
		nError = ERR_HOIST_OUT_OF_N_LIMIT;
		ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
						ERR_HOIST_OUT_OF_N_LIMIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_dHoist_TeachingPos,
						m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
						HOIST_MOVELIMIT_N);
	}


	if(m_pHWSet->m_pHoistAxis->GetCurrentPosition() > 15.0)
	{
		nError = ERR_HOIST_OUT_OF_P_LIMIT;
		ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
					ERR_HOIST_OUT_OF_P_LIMIT,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					m_dHoist_TeachingPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
					HOIST_MOVELIMIT_P);
	}

	if((m_pHWSet->m_pHand->IsRightGripOn() == true) && (m_pHWSet->m_pHand->IsLeftGripOn() == true))
	{
	   if((m_pHWSet->m_pShutter->IsOpen(FRONT) == false) ||
		  (m_pHWSet->m_pShutter->IsOpen(REAR) == false))
		  {
			nError = ERR_SHUTTER_AUTO_INIT_ABNORMAL_CASE;
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SHUTTER_AUTO_INIT_ABNORMAL_CASE,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					m_pHWSet->m_pShutter->IsOpen(FRONT),
					m_pHWSet->m_pShutter->IsClose(FRONT),
					m_pHWSet->m_pShutter->IsOpen(REAR),
					m_pHWSet->m_pShutter->IsClose(REAR));
          }
	}

	if(nError == NO_ERR)
	{
		if(((m_pHWSet->m_pHand->IsRightGripOn() == true) && (m_pHWSet->m_pHand->IsLeftGripOn() == true))
		||((m_pHWSet->m_pHand->IsRightGripOn() == false) && (m_pHWSet->m_pHand->IsLeftGripOn() == false)))
		{
			switch(nTransStep)
			{
			case STEP_SET_H_CHECK_HOIST_HOME:    //  Hoist home sensor를 확인
				if(m_pHWSet->m_pHoistAxis->IsRunning() == OFF)
				{

					if(bHoistHome == ON)
					{

						nError = m_pHWSet->m_pHoistAxis->SetHome();
						Sleep(20);
						m_pHWSet->m_pHoistAxis->SetAMPEnable(true);

						nTransStep = STEP_SET_H_MOVE_DOWN_UNTIL_HOIST_HOME_OFF;

					}
					else
					{

						if(m_bTransHoistOriginRecovery ==false)
							m_bTransHoistOriginRecovery = true;
						else
							nError = ERR_HOIST_NOTORIGIN;

						if(nError == NO_ERR)
						{
							nError = m_pHWSet->m_pHoistAxis->SetHome();
								Sleep(20);
							m_pHWSet->m_pHoistAxis->SetAMPEnable(true);


							nTransStep = STEP_SET_H_MOVE_UP_BY_xpMM;   // xp : x plus (xp>x)
		//					ADD_LOG("[SetOrigin] Next: STEP_SET_H_MOVE_UP_BY_xpMM");
							ADD_LOG("[SetOrigin] Next: STEP_SET_H_MOVE_UP_BY_xpMM");

						}
					}
				}
				break;

			case STEP_SET_H_MOVE_DOWN_UNTIL_HOIST_HOME_OFF:   //  Hoist home sensor가 OFF 될 때까지 Hand를 1mm씩 내림
				if(nError == NO_ERR && m_pHWSet->m_pHoistAxis->IsHoistHome()==ON)
				{
						if(m_pHWSet->m_pHoistAxis->GetCurrentPosition() > 10.0)
						{
							nError = ERR_HOIST_OUT_OF_P_LIMIT;
							ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
										ERR_HOIST_OUT_OF_P_LIMIT,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_dHoist_TeachingPos,
										m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
										HOIST_MOVELIMIT_P);
						}
                        else
							nError = Move_Hoist(HOIST_DOWN_1);
				}
				if(nError == NO_ERR)
				{
					dwTimeOfAutoSetHome = timeGetTime();
					nTransStep++;
				}
	//			Sleep(1000);
				break;
			case STEP_SET_H_CHECK_HOIST_HOME_OFF:     //  Hoist home sensor OFF 확인 후, OFF이면 다음 step
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwTimeOfAutoSetHome) > 1000)
				{
					if(nError == NO_ERR && m_pHWSet->m_pHoistAxis->IsHoistHome()==OFF)
					{
						nTransStep++;
						ADD_LOG("[SetOrigin] STEP_SET_H_CHECK_HOIST_HOME_OFF completed");
					}
					else
					{
						nTransStep = STEP_SET_H_MOVE_DOWN_UNTIL_HOIST_HOME_OFF;
					}
				}
				break;
			case STEP_SET_H_MOVE_UP_TO_NEW_HOME:   // Hand unit을 1mm씩 7회 올림 (v7.0 설계 기준 hoist와 hand 간 5mm 간격 유지)
//				if(nError == NO_ERR && m_pHWSet->m_pHoistAxis->IsHoistHome()==OFF)
//				{
					if(OHTUTILITY::PreventOverflowGetTimeMs(dwTimeOfAutoSetHome) > 1000)
					{
						if((m_pHWSet->m_pHand->IsRightGripOn() == true) && (m_pHWSet->m_pHand->IsLeftGripOn() == true))
						{

							if(m_SetHomenCount<(7+m_defualtparam->nHoistHomeSensorOriginConutAdd))
							{
								if(m_pHWSet->m_pHoistAxis->GetCurrentPosition() < -4.0)
								{
									nError = m_pHWSet->m_pHoistAxis->SetHome();
									Sleep(20);
									m_pHWSet->m_pHoistAxis->SetAMPEnable(true);
									break;
								}

								nError = Move_Hoist(HOIST_UP_1);
								dwTimeOfAutoSetHome = timeGetTime();
								m_SetHomenCount++;
							}
							else
							{
								ADD_LOG("[SetOrigin] STEP_SET_H_MOVE_UP_TO_NEW_HOME completed %d",m_defualtparam->nHoistHomeSensorOriginConutAdd);
								nTransStep = STEP_SET_H_SET_HOIST_ORIGIN;
							}


						}
						else
						{
							if(m_SetHomenCount<(10+m_defualtparam->nHoistHomeSensorOriginConutAdd))
							{
								if(m_pHWSet->m_pHoistAxis->GetCurrentPosition() < -4.0)
								{
									nError = m_pHWSet->m_pHoistAxis->SetHome();
									Sleep(20);
									m_pHWSet->m_pHoistAxis->SetAMPEnable(true);
									break;
								}

								nError = Move_Hoist(HOIST_UP_1);
								dwTimeOfAutoSetHome = timeGetTime();
								m_SetHomenCount++;
							}
							else
							{
								ADD_LOG("[SetOrigin] STEP_SET_H_MOVE_UP_TO_NEW_HOME completed %d",m_defualtparam->nHoistHomeSensorOriginConutAdd);
								nTransStep = STEP_SET_H_SET_HOIST_ORIGIN;
							}
						}
					}
//				}
				break;
			case STEP_SET_H_MOVE_UP_BY_xMM:    // x mm 만큼 hand 올림
			case STEP_SET_H_MOVE_UP_BY_xpMM:    // x plus mm 만큼 hand 올림 (hand의 위치가 더 밑인 경우)
				if(nError == NO_ERR && m_pHWSet->m_pHoistAxis->IsRunning() == OFF)
				{
	//				if(m_nTransStep==STEP_SET_H_MOVE_UP_BY_xMM)
					nError = Move_Hoist(HOIST_UP_x);
	//				else
	//					nError = Move_Hoist(HOIST_UP_xp);
					Sleep(1000);
	//				while(1)
	//				{
	//					if(m_pHWSet->m_pHoistAxis->IsRunning() == OFF)
	//						break;
	//				}
					nTransStep = STEP_SET_H_CHECK_HOIST_HOME;
				}

				break;
			case STEP_SET_H_SET_HOIST_ORIGIN:     //  현재의 위치에서 Hoist origin을 설정
				if(nError == NO_ERR && bHoistHome == ON && m_pHWSet->m_pHoistAxis->IsHoistHome()==ON)
				{

					nError = m_pHWSet->m_pHoistAxis->SetHome();
					nTransStep = STEP_SET_H_SET_HOIST_ORIGIN_COMPLETE;
					Sleep(20);
					m_pHWSet->m_pHoistAxis->SetAMPEnable(true);

					ADD_LOG("[SetOrigin] All steps completed. CurPos: %.1lf", m_pHWSet->m_pHoistAxis->GetCurrentPosition());
				}
				else
				{
					nError = ERR_HOIST_NOTORIGIN;
					ADD_LOG("[SetOrigin] Step : STEP_SET_HOIST_ORIGIN, E : ERR_HOIST_NOTORIGIN");
				}
				break;
			case STEP_SET_H_SET_HOIST_ORIGIN_COMPLETE:     //  현재의 위치에서 Hoist origin을 설정 완료
				if(bNoCheckFlag ==false)
				{
					m_StatusSet.byStatus = STS_COMPLETE;
					m_StatusSet.bIsOnProcess = false;
				}
				break;
			}
		}
		else if((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
		{
			nError = ERR_GRIP_SEN_MISMATCH;
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_GRIP_SEN_MISMATCH,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHand->IsLeftGripOn(),
							m_pHWSet->m_pHand->IsRightGripOn());
		}
	}


	if(nError != NO_ERR)
	{
		if(bNoCheckFlag ==false)
		{
			m_StatusSet.byStatus = STS_ERROR;
			m_StatusSet.bIsOnProcess = false;
			m_StatusSet.uError = nError;
		}
	}
	return nError;
}



/**
 @brief   이적재 중 ES 신호가 Off 되는지를 체크하는 함수
 @author  KKS
 @date    2014.08.27
 @param   Time out
 @return  ES 신호 On 상태이면 0, Off되면 해당 에러코드를 리턴함.
*/
int TransControl::Check_ESOffTime(int nPeriodTime, int nStep)
{
	static bool ES_Abnomal_Check = false;
	static DWORD m_dwESAbnormalOffWaitTime = timeGetTime();
	
	if(m_pEQPIO->IsESOn() == false)
	{
		if(ES_Abnomal_Check == false)
		{
			m_dwESAbnormalOffWaitTime = timeGetTime();
			ES_Abnomal_Check = true;			
		}
		
		if(ES_Abnomal_Check == true && OHTUTILITY::PreventOverflowGetTimeMs(m_dwESAbnormalOffWaitTime) > nPeriodTime)
		{
			ES_Abnomal_Check = false;
			ADD_LOG("ES_OFFED, Check_EQStatus, Step(%d), Time(%d), IgnorePIOFlag(%d)",
				nStep,
				OHTUTILITY::PreventOverflowGetTimeMs(m_dwESAbnormalOffWaitTime), 
				m_TransUse.IgnorePIOError);
			return RTN_FAIL;			
		}
	}			
	else
	{
		ES_Abnomal_Check = false;		
	}

	return RTN_OK;
}

/**
 @brief   이적재 중 HOAVBL 신호가 Off 되는지를 체크하는 함수
 @author  KKS
 @date    2014.08.27
 @param   Time out
 @return  HOAVBL 신호 On 상태이면 0, Off되면 해당 에러코드를 리턴함.
*/
int TransControl::Check_HOAVBLOffTime(int nPeriodTime, int nStep)
{
	static bool HOAVBL_Abnomal_Check = false;
	static DWORD m_dwHOAVBLAbnormalOffWaitTime = timeGetTime();	

	if(m_pEQPIO->IsHOAVBLOn() == false)
	{
		if(HOAVBL_Abnomal_Check == false)
		{
			m_dwHOAVBLAbnormalOffWaitTime = timeGetTime();
			HOAVBL_Abnomal_Check = true;
		}
		
		if(HOAVBL_Abnomal_Check == true && OHTUTILITY::PreventOverflowGetTimeMs(m_dwHOAVBLAbnormalOffWaitTime) > nPeriodTime)
		{
			HOAVBL_Abnomal_Check = false;

			ADD_LOG("HO_AVBL_OFFED, Check_EQStatus, Step(%d), Time(%d), IgnorePIOFlag(%d)",
				nStep,
				OHTUTILITY::PreventOverflowGetTimeMs(m_dwHOAVBLAbnormalOffWaitTime),
				m_TransUse.IgnorePIOError);
			return RTN_FAIL;
		}
	}			
	else
	{
		HOAVBL_Abnomal_Check = false;		
	}

	return RTN_OK;
}

/**
 @brief   이적재 중 Ready 신호가 Off 되는지를 체크하는 함수
 @author  KKS
 @date    2014.08.27
 @param   Time out
 @return  Ready 신호 On 상태이면 0, Off되면 해당 에러코드를 리턴함.
*/
int TransControl::Check_ReadyOffTime(int nPeriodTime, int nStep)
{
	static bool READY_Abnomal_Check = false;
	static DWORD m_dwREADYAbnormalOffWaitTime = timeGetTime();

	if(m_pEQPIO->IsReadyOn() == false)
	{
		if(READY_Abnomal_Check == false)
		{
			m_dwREADYAbnormalOffWaitTime = timeGetTime();
			READY_Abnomal_Check = true;
		}

		if(READY_Abnomal_Check == true && OHTUTILITY::PreventOverflowGetTimeMs(m_dwREADYAbnormalOffWaitTime) > nPeriodTime)
		{
			READY_Abnomal_Check = false;

			ADD_LOG("READY_OFFED, Check_EQStatus, Step(%d), Time(%d), IgnorePIOFlag(%d)",
				nStep,
				OHTUTILITY::PreventOverflowGetTimeMs(m_dwREADYAbnormalOffWaitTime),
				m_TransUse.IgnorePIOError);
			return RTN_FAIL;
		}
	}
	else
	{
		READY_Abnomal_Check = false;
	}

	return RTN_OK;
}

/**
 @brief   이적재 중 ES 신호와 HO_AVBL 신호가 Off 되는지를 체크하는 함수
 @author  LSJ
 @date    2012.11.09
 @param   nTransType(Load 명령인지 Unload 명령인지 선택)
 @param   nTransStep(ES 신호와 HO_AVBL 신호를 어느 이적재 Step까지 볼것인지 결정)
 @return  ES 신호와 HO_AVBL 신호가 On 상태이면 0, Off되면 해당 에러코드를 리턴함.
 */
int TransControl::Check_EQStatus(int nTransType, int nTransStep)
{
    int nError = 0;
	
	if(m_TransUse.IgnorePIOError == false)
    {
		if((nError==NO_ERR) && (Check_ESOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
		{
			if(nTransType == TRANSTYPE_LOAD)
			{
				nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHING;	// default
				ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_STRETCHING_1");				
			
				if(nTransStep > STEP_L_EQPIO_READY && nTransStep <= STEP_L_CHECK_ARRIVE_TEACHING_POINT)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHING;
					ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_STRETCHING_2");									
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_L_RELEASE_FOUP)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHED;				
					ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_STRETCHED_1");														
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_RELEASE_FOUP && nTransStep <= STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_FOLDING;				
					ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_FOLDING_1");																			
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_TA3;				
					ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_TA3_1");																								
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
			}					
			else
			{
				nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHING;	// default
				ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_STRETCHING_3");								
				
				if(nTransStep > STEP_U_EQPIO_READY && nTransStep <= STEP_U_CHECK_ARRIVE_TEACHING_POINT)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHING;
					ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_STRETCHING_4");									
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_ES_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_U_GRIP_FOUP)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHED;				
					ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_STRETCHED_2");																			
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_ES_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_GRIP_FOUP && nTransStep <= STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_FOLDING;				
					ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_FOLDING_2");																								
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_ES_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_TA3;				
					ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_TA3_2");																													
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_ES_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}	
			}				
		}

		if((nError==NO_ERR) && (Check_HOAVBLOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
		{
			if(nTransType == TRANSTYPE_LOAD)			
			{
				nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING;	// default
				ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING_1");								
				
				if(nTransStep > STEP_L_EQPIO_READY && nTransStep <= STEP_L_CHECK_ARRIVE_TEACHING_POINT)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING;
					ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING_2");													
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_L_RELEASE_FOUP)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED;				
					ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED_1");																		
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_RELEASE_FOUP && nTransStep <= STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING;				
					ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING_1");																							
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3;				
					ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3_1");																												
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}				
			}					
			else
			{
				nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING;	// default
				ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING_3");												
				
				if(nTransStep > STEP_U_EQPIO_READY && nTransStep <= STEP_U_CHECK_ARRIVE_TEACHING_POINT)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING;
					ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING_4");													
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_U_GRIP_FOUP)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED;				
					ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED_2");																							
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_GRIP_FOUP && nTransStep <= STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING;				
					ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING_2");																												
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3;				
					ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3_2");																																	
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}	
			}	
		}

		if((nError==NO_ERR) && (m_pEQPIO->Check_GoOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
		{
			if(nTransType == TRANSTYPE_LOAD)			
			{			
				nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING;		// default
				ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING_1");
				
				if(nTransStep > STEP_L_EQPIO_READY && nTransStep <= STEP_L_CHECK_ARRIVE_TEACHING_POINT)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING;
					ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING_2");					
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_L_RELEASE_FOUP)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED;				
					ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED_1");														
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_RELEASE_FOUP && nTransStep <= STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING;				
					ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING_1");																			
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3;				
					ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3_1");																								
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}			
			}					
			else
			{
				nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING;		// default
				ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING_3");				
				
				if(nTransStep > STEP_U_EQPIO_READY && nTransStep <= STEP_U_CHECK_ARRIVE_TEACHING_POINT)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING;
					ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING_4");					
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_U_GRIP_FOUP)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED;				
					ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED_2");																			
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_GRIP_FOUP && nTransStep <= STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING;				
					ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING_2");																								
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3;				
					ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3_2");																													
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}	
			}	
		}			

		if((nError==NO_ERR) && (m_pEQPIO->IsCompleteOn() == false) && (Check_ReadyOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
		{
			if(nTransType == TRANSTYPE_LOAD)
			{
				nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHING;			// default
				ADD_LOG("ERR_PIO_READY_ABNORMAL_OFF_STRETCHING_1");									
				
				if(nTransStep > STEP_L_EQPIO_READY && nTransStep <= STEP_L_CHECK_ARRIVE_TEACHING_POINT)
				{
					nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHING;
					ADD_LOG("ERR_PIO_READY_ABNORMAL_OFF_STRETCHING_2");														
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_READY_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_L_RELEASE_FOUP)
				{
					nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHIED;				
					ADD_LOG("ERR_PIO_READY_ABNORMAL_OFF_STRETCHIED_1");														
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_READY_ABNORMAL_OFF_STRETCHIED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_RELEASE_FOUP && nTransStep <= STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_READY_ABNORMAL_OFF_FOLDING;				
					ADD_LOG("ERR_PIO_READY_ABNORMAL_OFF_FOLDING_1");																			
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_READY_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = NO_ERR;				
				}
					
			}					
			else
			{
				nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHING;			// default
				ADD_LOG("ERR_PIO_READY_ABNORMAL_OFF_STRETCHING_3");													
				
				if(nTransStep > STEP_U_EQPIO_READY && nTransStep <= STEP_U_CHECK_ARRIVE_TEACHING_POINT)
				{
					nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHING;
					ADD_LOG("ERR_PIO_READY_ABNORMAL_OFF_STRETCHING_4");														
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_READY_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_U_GRIP_FOUP)
				{
					nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHIED;				
					ADD_LOG("ERR_PIO_READY_ABNORMAL_OFF_STRETCHIED_2");																			
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_READY_ABNORMAL_OFF_STRETCHIED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_GRIP_FOUP && nTransStep <= STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = ERR_PIO_READY_ABNORMAL_OFF_FOLDING;				
					ADD_LOG("ERR_PIO_READY_ABNORMAL_OFF_FOLDING_2");																								
					ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_READY_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
				}
				else if(nTransStep > STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
				{
					nError = NO_ERR;				
				}				
			}	
		}			
    }
    else
    {
        switch(nTransType)
        {
        case TRANSTYPE_LOAD:
            if((nTransStep > STEP_L_EQPIO_READY) && (nTransStep < STEP_L_CHECK_MOVE_UP_HOIST_FAST))
			{
				if((nError==NO_ERR) && (Check_ESOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHING;		// default
					ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_STRETCHING_5");									
					
					if(nTransStep > STEP_L_EQPIO_READY && nTransStep <= STEP_L_CHECK_ARRIVE_TEACHING_POINT)
					{
						nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHING;
						ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_STRETCHING_6");										
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_L_RELEASE_FOUP)
					{
						nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHED;				
						ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_STRETCHED_3");																				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_RELEASE_FOUP && nTransStep <= STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_ES_ABNORMAL_OFF_FOLDING;				
						ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_FOLDING_3");																									
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_ES_ABNORMAL_OFF_TA3;				
						ADD_LOG("ERR_PIO_ES_ABNORMAL_OFF_TA3_3");																														
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
				}

				if((nError==NO_ERR) && (Check_HOAVBLOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING;	// default
					ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING_5");													
				
					if(nTransStep > STEP_L_EQPIO_READY && nTransStep <= STEP_L_CHECK_ARRIVE_TEACHING_POINT)
					{
						nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING;
						ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING_6");														
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_L_RELEASE_FOUP)
					{
						nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED;				
						ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED_3");																								
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_RELEASE_FOUP && nTransStep <= STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING;				
						ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING_3");																													
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3;				
						ADD_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3_3");																																		
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}				
				}

				if((nError==NO_ERR) && (m_pEQPIO->Check_GoOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING;	// default
					ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING_5");					
					
					if(nTransStep > STEP_L_EQPIO_READY && nTransStep <= STEP_L_CHECK_ARRIVE_TEACHING_POINT)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING;
						ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING_6");						
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_L_RELEASE_FOUP)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED;				
						ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED_3");																				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_RELEASE_FOUP && nTransStep <= STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING;				
						ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING_3");																									
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3;				
						ADD_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3_3");																														
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}			
				}		

				if((nError==NO_ERR) && (m_pEQPIO->IsCompleteOn() == false) && (Check_ReadyOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
				{
					nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHING;		// default
					
					if(nTransStep > STEP_L_EQPIO_READY && nTransStep <= STEP_L_CHECK_ARRIVE_TEACHING_POINT)
					{
						nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHING;
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_READY_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_L_RELEASE_FOUP)
					{
						nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHIED;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_READY_ABNORMAL_OFF_STRETCHIED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_RELEASE_FOUP && nTransStep <= STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_READY_ABNORMAL_OFF_FOLDING;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_READY_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = NO_ERR;
					}					
				}			
			}
			break;
		case TRANSTYPE_UNLOAD:
			if((nTransStep > STEP_U_EQPIO_READY) && (nTransStep < STEP_U_CHECK_MOVE_UP_HOIST_FAST))
			{
				if((nError==NO_ERR) && (Check_ESOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHING;		// default
					
					if(nTransStep > STEP_U_EQPIO_READY && nTransStep <= STEP_U_CHECK_ARRIVE_TEACHING_POINT)
					{
						nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHING;
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_ES_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_U_GRIP_FOUP)
					{
						nError = ERR_PIO_ES_ABNORMAL_OFF_STRETCHED;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_ES_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_GRIP_FOUP && nTransStep <= STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_ES_ABNORMAL_OFF_FOLDING;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_ES_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_ES_ABNORMAL_OFF_TA3;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_ES_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
				}

				if((nError==NO_ERR) && (Check_HOAVBLOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING;	// default
					
					if(nTransStep > STEP_U_EQPIO_READY && nTransStep <= STEP_U_CHECK_ARRIVE_TEACHING_POINT)
					{
						nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING;
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_U_GRIP_FOUP)
					{
						nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_GRIP_FOUP && nTransStep <= STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}				
				}


				if((nError==NO_ERR) && (m_pEQPIO->Check_GoOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING;	// default
					
					if(nTransStep > STEP_U_EQPIO_READY && nTransStep <= STEP_U_CHECK_ARRIVE_TEACHING_POINT)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING;
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_U_GRIP_FOUP)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_GRIP_FOUP && nTransStep <= STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}			
				}		

				if((nError==NO_ERR) && (Check_ReadyOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, nTransStep) == RTN_FAIL))
				{
					nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHING;		// default
					if(nTransStep > STEP_U_EQPIO_READY && nTransStep <= STEP_U_CHECK_ARRIVE_TEACHING_POINT)
					{
						nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHING;
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_READY_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_CHECK_ARRIVE_TEACHING_POINT && nTransStep <= STEP_U_GRIP_FOUP)
					{
						nError = ERR_PIO_READY_ABNORMAL_OFF_STRETCHIED;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_READY_ABNORMAL_OFF_STRETCHIED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_GRIP_FOUP && nTransStep <= STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = ERR_PIO_READY_ABNORMAL_OFF_FOLDING;				
						ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_READY_ABNORMAL_OFF_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
					}
					else if(nTransStep > STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
					{
						nError = NO_ERR;
					}
				}			
			}
			break;
		}
	}

    return nError;
}

int TransControl::Check_LREQOnOffTime(int nPeriodTime, bool bCheckStatus, int nStep)
{
	int nError = RTN_OK;
	
	static bool LREQ_OnOff_Abnomal_Check = false;	
	static DWORD m_dwLREQ_OnOff_Abnomal_Check_Time = timeGetTime();	


	if(m_pEQPIO->IsLoadREQOn() == bCheckStatus)
	{
		if(LREQ_OnOff_Abnomal_Check == false)
		{
			m_dwLREQ_OnOff_Abnomal_Check_Time = timeGetTime();			
			LREQ_OnOff_Abnomal_Check = true;			
		}				
		
		if(LREQ_OnOff_Abnomal_Check == true && OHTUTILITY::PreventOverflowGetTimeMs(m_dwLREQ_OnOff_Abnomal_Check_Time) > nPeriodTime)
		{
			LREQ_OnOff_Abnomal_Check = false;			
			
            ADD_LOG("ERR_Check_LREQOnOffTime, Step(%d), Time(%d)",
				nStep,
				OHTUTILITY::PreventOverflowGetTimeMs(m_dwLREQ_OnOff_Abnomal_Check_Time));
        	return RTN_FAIL;							
		}
	}			
	else
	{
		LREQ_OnOff_Abnomal_Check = false;		
	}

	return nError;
}

int TransControl::Check_UREQOnOffTime(int nPeriodTime, bool bCheckStatus, int nStep)
{
	int nError = RTN_OK;
	
	static bool UREQ_OnOff_Abnomal_Check = false;	
	static DWORD m_dwUREQ_OnOff_Abnomal_Check_Time = timeGetTime();	
	
	if(m_pEQPIO->IsUnloadREQOn() == bCheckStatus)
	{
		if(UREQ_OnOff_Abnomal_Check == false)
		{
			m_dwUREQ_OnOff_Abnomal_Check_Time = timeGetTime();			
			UREQ_OnOff_Abnomal_Check = true;			
		}				
		
		if(UREQ_OnOff_Abnomal_Check == true && OHTUTILITY::PreventOverflowGetTimeMs(m_dwUREQ_OnOff_Abnomal_Check_Time) > nPeriodTime)
		{
			UREQ_OnOff_Abnomal_Check = false;						
			
            ADD_LOG("ERR_Check_LREQOnOffTime, Step(%d), Time(%d)",
				nStep,
				OHTUTILITY::PreventOverflowGetTimeMs(m_dwUREQ_OnOff_Abnomal_Check_Time));
			return RTN_FAIL;			
		}
	}			
	else
	{
		UREQ_OnOff_Abnomal_Check = false;		
	}

	return nError;
}

int TransControl::Check_L_U_REQStatus_Up(int nTransType, int nTransStep)
{
    int nError = 0;

    switch(nTransType)
    {
    case TRANSTYPE_LOAD:
		if(nError==NO_ERR)
		{
			if(Check_LREQOnOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, true, nTransStep) == RTN_FAIL)
			{
				nError = ERR_PIO_LREQ_ABNORMAL_ON_FOLDING_L;
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_LREQ_ABNORMAL_ON_FOLDING_L,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
			}
		}			
		if(nError==NO_ERR)
		{
			if(Check_UREQOnOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, true, nTransStep) == RTN_FAIL)
			{
				nError = ERR_PIO_UREQ_ABNORMAL_ON_FOLDING;				
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_UREQ_ABNORMAL_ON_FOLDING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
			}
		}			
        break;
    case TRANSTYPE_UNLOAD:
		if(nError==NO_ERR)
		{
			if(Check_LREQOnOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, true, nTransStep) == RTN_FAIL)
			{
				nError = ERR_PIO_LREQ_ABNORMAL_ON_FOLDING_U;
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_LREQ_ABNORMAL_ON_FOLDING_U,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
			}
		}			
        break;
    }

    return nError;
}

int TransControl::Check_L_U_REQStatus_Down(int nTransType, int nTransStep)
{
    int nError = 0;

    switch(nTransType)
    {
    case TRANSTYPE_LOAD:
		if(nError == NO_ERR)
		{
			if(Check_UREQOnOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, true, nTransStep) == RTN_FAIL)
			{
				nError = ERR_PIO_UREQ_ABNORMAL_ON_STRETCHING;
				ADD_LOG("ERR_PIO_UREQ_ABNORMAL_ON_STRETCHING");												
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_UREQ_ABNORMAL_ON_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
			}
		}			
        break;
    case TRANSTYPE_UNLOAD:
		if(nError == NO_ERR)
		{
			if(Check_LREQOnOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, true, nTransStep) == RTN_FAIL)
			{
				nError = ERR_PIO_LREQ_ABNORMAL_ON_STRETCHING;			
				ADD_LOG("ERR_PIO_LREQ_ABNORMAL_ON_STRETCHING");												
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/2/%d",
									ERR_PIO_LREQ_ABNORMAL_ON_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
			}
		}			
		if(nError == NO_ERR)		
		{
			if(Check_UREQOnOffTime(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME, false, nTransStep) == RTN_FAIL)
			{
				nError = ERR_PIO_UREQ_ABNORMAL_OFF_STRETCHING;			
				ADD_LOG("ERR_PIO_UREQ_ABNORMAL_OFF_STRETCHING");																
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_UREQ_ABNORMAL_OFF_STRETCHING,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									nTransStep);
			}
		}			
        break;
    }

    return nError;
}

/**
 @brief   Hoist와 Shift, Rotate 축의 SW 리미트를 설정하고, Shutter와 Hand의 상태를 파악하여 초기화 함.
 @author  LSJ
 @date    2012.11.09
 @return  이적재할 준비가 잘 되어 있으면 0, 문제가 있으면 해당 에러코드 리턴.
 */
int TransControl::ReadyTrans()
{
	int nError = 0;
	DWORD dwStartTime = 0;

	// set hoist limit
	if(nError == NO_ERR)
	{
			nError = m_pHWSet->m_pHoistAxis->SetPositiveSWLimit(fabs(m_dHoist_TeachingPos) + HOIST_POS_LIMIT_TRANSOFFSET);

	}

	if(nError == NO_ERR)
	{
			nError = m_pHWSet->m_pHoistAxis->SetNegativeSWLimit(HOIST_NEG_LIMIT);


	}

//	if(nError == NO_ERR)
//    {
//        if(m_dShift_TeachingPos >= 0.0)
//        {
//			nError = m_pHWSet->m_pShiftAxis->SetPositiveSWLimit(m_dShift_TeachingPos + SHIFT_POS_LIMIT_FORWAIT);
//
//            if(nError == NO_ERR)
//                nError = m_pHWSet->m_pShiftAxis->SetNegativeSWLimit(SHIFT_NEG_LIMIT_FORWAIT);
//        }
//		else
//		{
//			nError = m_pHWSet->m_pShiftAxis->SetPositiveSWLimit(SHIFT_POS_LIMIT_FORWAIT);
//			if(nError == NO_ERR)
//                nError = m_pHWSet->m_pShiftAxis->SetNegativeSWLimit(m_dShift_TeachingPos + SHIFT_NEG_LIMIT_FORWAIT);
//        }
//    }

    // 2016-12-05, Add by Labeler
    // Rotate의 경우, 0 ~ 180도 사이로의 이동 값만 받도록 처리
#if(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
    if(nError == NO_ERR)
    {
		if((m_dRotate_TeachingPos > (ROTATE_MOVELIMIT_P + 100))
        || (m_dRotate_TeachingPos < (ROTATE_MOVELIMIT_N - 100)))
        {
            nError = ERR_ROTATE_LIMIT_ON_ERR;
        }
	}


	if(m_defualtparam->IsRotateSensor == true)
	{
		if(m_pHWSet->m_pRotate->IsRunning() == false)
		{
	   //	ADD_ERR_LOG("CheckAlways : %d :Count:%d",m_pHWSet->m_pRotate->GetCurrentPositionDegree(), nRotateCheckSensorCount);
			if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_0) < ROTATE_INPOS_CHECK_SENSOR)
			{

				if(m_pHWSet->m_pRotate->IsRotate0Sensor() == OFF)
				{
					ADD_ERR_LOG("ReadyTrans Ratoate 0: %d",m_pHWSet->m_pRotate->GetCurrentPositionDegree());
					return ERR_ROTATE_0_TIMEOVER;
                }

			}
			else if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_90) <ROTATE_INPOS_CHECK_SENSOR)
			{
				if(m_pHWSet->m_pRotate->IsRotate90Sensor() == OFF)
				{
					ADD_ERR_LOG("ReadyTrans Ratoate 90: %d",m_pHWSet->m_pRotate->GetCurrentPositionDegree());
					return ERR_ROTATE_90_TIMEOVER;
				}
			}
			else if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_180) <ROTATE_INPOS_CHECK_SENSOR)
			{
				if(m_pHWSet->m_pRotate->IsRotate180Sensor() == OFF)
				{
					ADD_ERR_LOG("ReadyTrans Ratoate 180: %d",m_pHWSet->m_pRotate->GetCurrentPositionDegree());
					return ERR_ROTATE_180_TIMEOVER;
   				}
			}
			else if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_270) <ROTATE_INPOS_CHECK_SENSOR)
			{
				if(m_pHWSet->m_pRotate->IsRotate270Sensor() == OFF)
				{
					return ERR_ROTATE_270_TIMEOVER;
				}
			}
		}
	}
#endif


    // -------------------------------------------
//	double dHoistPositive = 0.0;
//    double dHoistNegative = 0.0;
//	double dShiftPositive = 0.0;
//    double dShiftNegative = 0.0;
//
//    m_pHWSet->m_pHoistAxis->GetSWLimit(dHoistPositive, dHoistNegative);
//    m_pHWSet->m_pShiftAxis->GetSWLimit(dShiftPositive, dShiftNegative);
//	ADD_LOG("[H_P : %lf, H_N : %lf, S_P : %lf, S_N : %lf]", dHoistPositive, dHoistNegative, dShiftPositive, dShiftNegative);
    // -------------------------------------------


//	if(nError == NO_ERR)
//    {
//        if(m_pHWSet->m_pShiftAxis->IsShiftHome() == false)
//        {
//			if((fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < INPOS_LIMIT))
//            {
//				nError = ERR_SHIFT_HOMESEN_NOTON;
//				ADD_LOG("[ReadyTrans] E : %d(ERR_SHIFT_HOMESEN_NOTON)", nError);
//				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
//							ERR_SHIFT_HOMESEN_NOTON,
//							m_pOHTMainStatus->StatusCommon.CurNodeID,
//							m_pExecuteInfo->ExecutePositionInfo.NextNode,
//							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//							m_pOHTMainStatus->StatusCommon.ExistFoup,
//							m_pOHTMainStatus->StatusCommon.StopStationID,
//							m_nTransType,
//							m_nTransStep,
//							m_pHWSet->m_pShiftAxis->IsShiftHome());
//            }
//        }
//	}

    // 2016-12-30, Add by Labeler, Rotate Position Check 변경
    // 기존 0도만 가능에서 0도, 180도 같이 가능하도록 변경
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
    if(nError == NO_ERR)
	{
		if( (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
		&&  (fabs(ROTATE_POS_90 - m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
		&&  (fabs(ROTATE_POS_180 - m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
        {
			nError = ERR_ROTATE_NOT_INPOSITION;
			ADD_LOG("[ReadyTrans] E : %d(ERR_ROTATE_NOT_INPOSITION)", nError);
		}
    }
#endif

	// shutter check
//	if(nError == NO_ERR)
//	{
//		if(m_TransUse.ShutterControl == true)
//		{
//			if((m_pHWSet->m_pShutter->IsOpen(FRONT) != m_pHWSet->m_pShutter->IsOpen(REAR)) ||
//				(m_pHWSet->m_pShutter->IsClose(FRONT) != m_pHWSet->m_pShutter->IsClose(REAR)))
//            {
//				nError = ERR_SHUTTER_DIFFSTATUS;
//				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
//							ERR_SHUTTER_DIFFSTATUS,
//							m_pOHTMainStatus->StatusCommon.CurNodeID,
//							m_pExecuteInfo->ExecutePositionInfo.NextNode,
//							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//							m_pOHTMainStatus->StatusCommon.ExistFoup,
//							m_pOHTMainStatus->StatusCommon.StopStationID,
//							m_pHWSet->m_pShutter->IsOpen(FRONT),
//							m_pHWSet->m_pShutter->IsClose(FRONT),
//							m_pHWSet->m_pShutter->IsOpen(REAR),
//							m_pHWSet->m_pShutter->IsClose(REAR));
//            }
//
//            if(nError != NO_ERR)
//            {
//                nError = 0;
//                dwStartTime = timeGetTime();
//  				if(!m_defualtparam->PreShutter)
//				{
//					while(m_pHWSet->m_pShutter->IsOpen(FRONT) != m_pHWSet->m_pShutter->IsOpen(REAR))
//					{
//						if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)SHUTTER_MOTION_LIMIT_TIME)
//							break;
//						Sleep(10);
//					}
//				}else
//				{
//					while(!((m_pHWSet->m_pShutter->IsOpen(FRONT) == true) &&
//						  (m_pHWSet->m_pShutter->IsOpen(REAR)==true)))
//					{
//						if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)SHUTTER_MOTION_LIMIT_TIME)
//							break;
//						Sleep(10);
//					}
//				}
//                if((m_pHWSet->m_pShutter->IsOpen(FRONT) != m_pHWSet->m_pShutter->IsOpen(REAR)) ||
//                    (m_pHWSet->m_pShutter->IsClose(FRONT) != m_pHWSet->m_pShutter->IsClose(REAR)))
//				{
//					nError = ERR_SHUTTER_DIFFSTATUS;
//                    ADD_LOG("[ReadyTrans] E : %d(ERR_SHUTTER_DIFFSTATUS)", nError);
//					ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
//							ERR_SHUTTER_DIFFSTATUS,
//							m_pOHTMainStatus->StatusCommon.CurNodeID,
//							m_pExecuteInfo->ExecutePositionInfo.NextNode,
//							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//							m_pOHTMainStatus->StatusCommon.ExistFoup,
//							m_pOHTMainStatus->StatusCommon.StopStationID,
//							m_pHWSet->m_pShutter->IsOpen(FRONT),
//							m_pHWSet->m_pShutter->IsClose(FRONT),
//							m_pHWSet->m_pShutter->IsOpen(REAR),
//							m_pHWSet->m_pShutter->IsClose(REAR));
//                }
//            }
//        }
//	}


	// hand check
    if(nError == NO_ERR)
	{

		if(m_TransUse.HandSensor == true)
		{

			if(m_pHWSet->m_pHand->IsOpen() == m_pHWSet->m_pHand->IsClose())
			{

                nError = m_pHWSet->m_pHand->Origin();
                if(nError)
				{

					if((m_pHWSet->m_pHand->IsOpen() == true) && (m_pHWSet->m_pHand->IsClose() == true))
					{
						nError = ERR_HAND_LIMIT_SEN_ALL_ON;
						ADD_LOG("[ReadyTrans] E : %d(ERR_HAND_LIMIT_SEN_ALL_ON)", nError);
						ADD_FDC_LOG("ERR/%d/1/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_HAND_LIMIT_SEN_ALL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
					}

					if((m_pHWSet->m_pHand->IsOpen() == false) && (m_pHWSet->m_pHand->IsClose() == false))
                    {
                        if(m_bSlipringFlag)
                        {
                            m_bSlipringFlag = false;
							nError = ERR_SEN_ABNORMAL_HAND;
							ADD_LOG("[ReadyTrans] E : %d(ERR_SEN_ABNORMAL_HAND)", nError);
                            ADD_LOG("[ReadyTrans] E : ERR_HAND_LIMIT_SEN_ALL_OFF)", nError);
							ADD_FDC_LOG("ERR/%d/1/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_SEN_ABNORMAL_HAND,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
                        }
                        else
                        {
                            nError = ERR_HAND_LIMIT_SEN_ALL_OFF;
                            ADD_LOG("[ReadyTrans] E : %d(ERR_HAND_LIMIT_SEN_ALL_OFF)", nError);
							ADD_FDC_LOG("ERR/%d/1/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_HAND_LIMIT_SEN_ALL_OFF,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
                        }

                    }
                }
                else
                    Sleep(500);
            }
            if(nError == NO_ERR)
			{

                if(m_pHWSet->m_pHand->IsInposition() == false)
				{

					if(m_pHWSet->m_pHand->IsClose() == true)
                    {
                        nError = m_pHWSet->m_pHand->Origin();
                        if(nError)
                        {
                            nError = ERR_HAND_INPOSITION_ERROR;
							ADD_LOG("[ReadyTrans] E : %d(ERR_HAND_INPOSITION_ERROR)", nError);
							ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_HAND_INPOSITION_ERROR,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_pHWSet->m_pHand->IsInposition(),
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose());
                        }
                        else
                            Sleep(500);
                    }
                    else
                    {
                        if((m_nTransType == TRANSTYPE_UNLOAD) && (m_pHWSet->m_pHand->IsRightGripOn() == false) &&
                            (m_pHWSet->m_pHand->IsLeftGripOn() == false))
                        {
                            nError = m_pHWSet->m_pHand->Origin();
                            if(nError)
                            {
                                nError = ERR_HAND_INPOSITION_ERROR;
                                ADD_LOG("[ReadyTrans] E : %d(ERR_HAND_INPOSITION_ERROR)", nError);
								ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_HAND_INPOSITION_ERROR,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_pHWSet->m_pHand->IsInposition(),
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose());
                            }
                            else
                                Sleep(500);
                        }
                        else
                        {
                            nError = ERR_HAND_INPOSITION_ERROR;
                            ADD_LOG("[ReadyTrans] E : %d(ERR_HAND_INPOSITION_ERROR)", nError);
							ADD_FDC_LOG("ERR/%d/3/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_HAND_INPOSITION_ERROR,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_pHWSet->m_pHand->IsInposition(),
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose());
                        }
                    }
                }
            }

            if(nError == NO_ERR)
			{

				if(m_pHWSet->m_pHand->IsAlarm() == true)
                {
					nError = m_pHWSet->m_pHand->AlarmReset();

				}
			}
        }
	}


//	if(nError == NO_ERR)
//	{
//		if(m_defualtparam->PreShutter)
//		{
//			if((m_pHWSet->m_pShutter->IsOpen(FRONT) == false) || (m_pHWSet->m_pShutter->IsOpen(REAR) == false))
//			{
//				dwStartTime = timeGetTime();
//				while ((nError = m_pHWSet->m_pShutter->CheckComplete()) == NEXT_CHECK)
//				{
//					if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)SHUTTER_MOTION_LIMIT_TIME)
//						break;
//                    Sleep(10);
//                }
//			}
//		}

//		if(((m_pHWSet->m_pShutter->IsClose(FRONT) == true) || (m_pHWSet->m_pShutter->IsClose(REAR) == true))
//		&& ((m_nTransType == TRANSTYPE_UNLOAD) || (m_nTransType == TRANSTYPE_MOVET)
//		|| ((m_nTransType == TRANSTYPE_LOAD) && (m_pHWSet->m_pInnerFoup->IsFoupDetect() == true) &&
//		(m_pHWSet->m_pHand->IsLeftGripOn() == true) && (m_pHWSet->m_pHand->IsRightGripOn() == true))))
//		{
//
//			nError = m_pHWSet->m_pShutter->Open_Cmd();
//			ADD_LOG("[ReadyTrans] E : %d(Shutter Open)", nError);
//			// RailSupport 동작
//			if(nError == NO_ERR)
//			{
//// 2016-11-30, Labeler
//
//				while ((nError = m_pHWSet->m_pShutter->CheckComplete()) == NEXT_CHECK)
//                {
//                    Sleep(10);
//                }
//
//			}
//		}
//        else
//		{
//
//				m_pHWSet->m_pShutter->CheckComplete();
//		}
//	}


    if((nError == NO_ERR) && (m_TransUse.EQPIO == true))
	{
		ADD_LOG("8");
        nError = m_pEQPIO->PIOEnable();
    }

    return nError;
}

/**
 @brief   Hoist와 Shift, Rotate 축의 SW 리미트를 설정하고, Shutter와 Hand의 상태를 파악하여 초기화 함.
 @author  LSJ
 @date    2012.11.09
 @return  이적재할 준비가 잘 되어 있으면 0, 문제가 있으면 해당 에러코드 리턴.
 */
int TransControl::Check_State()
{
    int nError = 0;
    int nCount = 0;

	DWORD dwStartTime = 0;
	bool bIsSenON = false;
    double dInposLimit = INPOS_LIMIT;

    switch(m_nTransType)
    {
    case TRANSTYPE_LOAD:

// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
#else
        // -------------------------------------------
        // [BEGIN] by zzang9un 013.08.05 : UTB 체크
        if(nError == NO_ERR)
        {
            if((m_nEqType == PORT_TYPE_UTB) && (m_TransUse.UTBSen == true))
            {
                // UTB Load시 이중반입 방지차원에서 Foup 없어도 몇번더 체크함.
                if(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect() == false) // Foup 없음
                {
                    nCount = 0;
                    while(nCount < 4)
                    {
                        if(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect()) // Foup 존재
                        {
                            nError = ERR_UTB_FOUP_EXIST;
							ADD_LOG("[Check_State] E : %d(ERR_UTB_FOUP_EXIST)", nError);
							ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_UTB_FOUP_EXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect());
                            break;
                        }
                        nCount++;
                        Sleep(10);
                    }
                }
                else
                {
                    nError = ERR_UTB_FOUP_EXIST;
					ADD_LOG("[Check_State] E : %d(ERR_UTB_FOUP_EXIST)", nError);
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_UTB_FOUP_EXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect());
                }
            }
        }
        // [END] by zzang9un 013.08.05 : UTB 체크
        // -------------------------------------------
#endif
        // -------------------------------------------
		// [BEGIN] by zzang9un 013.08.05 : STB 체크
// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
#else
        if((nError == NO_ERR) && ((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R)) && (m_TransUse.STBSen == true))
        {
			switch(m_nSTBDirection)
            {
            case STB_DIR_RIGHT:
                if(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect() == false)
                {
                    nCount = 0;
                    while(nCount < FOUP_DETECT_CNT)
                    {
                        if(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect())
						{
                            nCount = FOUP_DETECT_CNT;
                            nError = ERR_STB_FOUP_EXIST;
							ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_EXIST)", nError);
							ADD_FDC_LOG("ERR/%d/1/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_EXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
                        }
                        else
                        {
                            nCount++;
                            Sleep(50);
                        }
                    }
                }
                else
                {
					nError = ERR_STB_FOUP_EXIST;
					ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_EXIST)", nError);
					ADD_FDC_LOG("ERR/%d/1/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_EXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
                }
				break;
			case STB_DIR_LEFT:
                if(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect() == false)
                {
                    nCount = 0;
                    while(nCount < FOUP_DETECT_CNT)
                    {
                        if(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect())
                        {
                            nCount = FOUP_DETECT_CNT;
							nError = ERR_STB_FOUP_EXIST;
							ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_EXIST)", nError);
							ADD_FDC_LOG("ERR/%d/2/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_EXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
                        }
                        else
                        {
                            nCount++;
                            Sleep(FOUP_DETECT_TIME);
                        }
                    }
                }
                else
                {
					nError = ERR_STB_FOUP_EXIST;
					ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_EXIST)", nError);
					ADD_FDC_LOG("ERR/%d/2/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_EXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
                }
				break;
            }
        }
        // [END] by zzang9un 013.08.05 : STB 체크
        // -------------------------------------------
#endif

        // Loading시 원점에서 Hand Push Sensor ON이면 에러 발생
        if((nError == NO_ERR) && (m_TransUse.HandPushSen == true))
		{

			if(m_pHWSet->m_pHand->IsPushOn() == true)
			{
				nCount = 0;

				while(m_pHWSet->m_pHand->IsPushOn() == true)
				{
                    if(nCount > 40)
                        break;
                    nCount++;
                    Sleep(10);
                }

				if(m_pHWSet->m_pHand->IsPushOn() == true)
				{
					nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
					ADD_LOG("[Check_State] E : %d(ERR_HAND_PUSHSEN_ABNORMAL_ON)", nError);
				        ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
										ERR_HAND_PUSHSEN_ABNORMAL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsPushOn(),
										m_pHWSet->m_pHoistAxis->GetCurrentPosition());
				}

            }
        }


		if((nError == NO_ERR) && ((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true)) &&
			(m_pHWSet->m_pInnerFoup->IsFoupDetect() == false))
		{
            DWORD dwStartTime = 0;
            static int nCount = 0;
            dwStartTime = timeGetTime();
            while(1)
            {
                if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 500)
                    break;

                if(m_pHWSet->m_pInnerFoup->IsFoupDetect() == false)
                {
                    nCount++;
                    if(nCount > 5)
                        break;
                }
                Sleep(10);
            }

			if(nCount > 5)
			{
                nCount = 0;
                nError = ERR_INNER_FOUP_NOT_DETECT;
				ADD_LOG("[Check_State] E : %d(ERR_INNER_FOUP_NOT_DETECT)", nError);
            }
		}

		break;


    case TRANSTYPE_UNLOAD:
	case TRANSTYPE_SCAN:
// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
#else
		if(nError == NO_ERR)
		{
			if((m_nEqType == PORT_TYPE_UTB) && (m_TransUse.UTBSen == true))
			{
				// Foup이 존재하더라도 한번더 체크..
				if(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect()) // Foup 존재
				{
					nCount = 0;
					while(nCount < 4)
					{
						if(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect() == false) // Foup 존재하지 않음.
						{
							nError = ERR_UTB_FOUP_NOTEXIST;
							ADD_LOG("[Check_State] E : %d(ERR_UTB_FOUP_NOTEXIST)", nError);
							ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_UTB_FOUP_NOTEXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect());
							break;

						}
						nCount++;
						Sleep(10);
					}
				}
				else
				{
					nError = ERR_UTB_FOUP_NOTEXIST;
					ADD_LOG("[Check_State] E : %d(ERR_UTB_FOUP_NOTEXIST)", nError);
					ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_UTB_FOUP_NOTEXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect());
				}
			}
		}
#endif

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
#else
		if((nError == NO_ERR) && ((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R)) && (m_TransUse.STBSen == true))
		{
			switch(m_nSTBDirection)
			{
			case STB_DIR_RIGHT:
				if(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect())
				{
					nCount = 0;
					while(nCount < 4)
					{
						if(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect() == false)
						{
							nCount = 4;
							nError = ERR_STB_FOUP_NOTEXIST;
							ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_NOTEXIST)", nError);
							ADD_FDC_LOG("ERR/%d/1/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_NOTEXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
						}
						else
						{
							nCount++;
							Sleep(50);
						}
					}
				}
				else
				{
					nError = ERR_STB_FOUP_NOTEXIST;
					ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_NOTEXIST)", nError);
					ADD_FDC_LOG("ERR/%d/1/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_NOTEXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
				}
				break;
			case STB_DIR_LEFT:
				if(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect())
				{
					nCount = 0;
					while(nCount < FOUP_DETECT_CNT)
					{
						if(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect() == false)
						{
							nCount = FOUP_DETECT_CNT;
							nError = ERR_STB_FOUP_NOTEXIST;
							ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_NOTEXIST)", nError);
							ADD_FDC_LOG("ERR/%d/2/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_NOTEXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
						}
						else
						{
							nCount++;
							Sleep(FOUP_DETECT_TIME);
						}
					}
				}
				else
				{
					nError = ERR_STB_FOUP_NOTEXIST;
					ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_NOTEXIST)", nError);
					ADD_FDC_LOG("ERR/%d/2/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_NOTEXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
				}
				break;
			}
		}
#endif
		// Unloading시 원점에서 Hand Push Sensor ON이면 에러 발생
		if((nError == NO_ERR) && (m_TransUse.HandPushSen == true))
		{

			if(m_pHWSet->m_pHand->IsPushOn() == true)
			{
				nCount = 0;

				while(m_pHWSet->m_pHand->IsPushOn() == true)
				{
					if(nCount > 40)
						break;
					nCount++;
					Sleep(10);
				}

				if(m_pHWSet->m_pHand->IsPushOn() == true)
				{
					nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
					ADD_LOG("[Check_State] E : %d(ERR_HAND_PUSHSEN_ABNORMAL_ON)", nError);
				        ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
										ERR_HAND_PUSHSEN_ABNORMAL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsPushOn(),
										m_pHWSet->m_pHoistAxis->GetCurrentPosition());
				}

			}
		}

		// Unloading 전 : Foup 존재할 시==>에러 발생.

		if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsRightGripOn() == false) && (m_pHWSet->m_pHand->IsLeftGripOn() == false) &&
			(m_pHWSet->m_pInnerFoup->IsFoupDetect() == true))
		{
			DWORD dwStartTime = 0;
			static int nCount = 0;
			dwStartTime = timeGetTime();
			while(1)
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 500)
					break;
				if(m_pHWSet->m_pInnerFoup->IsFoupDetect() == true)
				{
					nCount++;
					if(nCount > 5)
						break;
				}
				Sleep(10);
			}

			if(nCount > 5)
			{
				nCount = 0;
				//nError = ERR_INNER_FOUP_DETECT;
				ADD_LOG("[Check_State] E : %d(ERR_INNER_FOUP_DETECT)", nError);
			}
		}
		break;

	}

	if(nError == NO_ERR)
	{

		if(nError == NO_ERR)
		{
			if(((m_pHWSet->m_pShutter->IsClose(FRONT) == true) || (m_pHWSet->m_pShutter->IsClose(REAR) == true))
			&& ((m_nTransType == TRANSTYPE_UNLOAD)
			|| ((m_nTransType == TRANSTYPE_LOAD) && (m_pHWSet->m_pInnerFoup->IsFoupDetect() == true) &&
			(m_pHWSet->m_pHand->IsLeftGripOn() == true) && (m_pHWSet->m_pHand->IsRightGripOn() == true))))
				nError = m_pHWSet->m_pShutter->Open();
		}



	}

    switch(m_nTransType)
    {
    case TRANSTYPE_LOAD:
        if((nError == NO_ERR) && (m_TransUse.GripperSen == true))
        {

            if((m_pHWSet->m_pHand->IsRightGripOn() == false) || (m_pHWSet->m_pHand->IsLeftGripOn() == false))
            {
                if(m_bSlipringFlag)
                {
                m_bSlipringFlag = false;
				nError = ERR_SEN_ABNORMAL_HAND;
                ADD_LOG("[Check_State] E : %d(ERR_SEN_ABNORMAL_HAND)", nError);
                ADD_LOG("[Check_State] ERR_HAND_CAST_NOTEXIST");
					ADD_FDC_LOG("ERR/%d/2/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_SEN_ABNORMAL_HAND,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
                }
                else
                {
				nError = ERR_HAND_CAST_NOTEXIST;
				ADD_LOG("[Check_State] E : %d(ERR_HAND_CAST_NOTEXIST)", nError);
					ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
											ERR_HAND_CAST_NOTEXIST,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pExecuteInfo->ExecutePositionInfo.NextNode,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_pOHTMainStatus->StatusCommon.ExistFoup,
											m_pOHTMainStatus->StatusCommon.StopStationID,
											m_pHWSet->m_pHand->IsOpen(),
											m_pHWSet->m_pHand->IsClose(),
											m_pHWSet->m_pHand->IsLeftGripOn(),
											m_pHWSet->m_pHand->IsRightGripOn());
                }
            }
        }
        if(nError == NO_ERR)
		{

			if(m_pHWSet->m_pHand->IsClose() == false)
            {
                nError = ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD;
				ADD_LOG("[Check_State] E : %d(ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD)", nError);
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsOpen(),
							m_pHWSet->m_pHand->IsClose());
            }
        }
        if(nError == NO_ERR)
		{

			if(m_pHWSet->m_pHand->IsOpen() == true)
            {
                nError = ERR_HAND_OPEN_SEN_ON_BEFORE_LOAD;
                ADD_LOG("[Check_State] ERROR : %d(ERR_HAND_OPEN_SEN_ON_BEFORE_LOAD)", nError);
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_OPEN_SEN_ON_BEFORE_LOAD,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsOpen(),
							m_pHWSet->m_pHand->IsClose());
            }
        }
        break;
    case TRANSTYPE_UNLOAD:
    case TRANSTYPE_SCAN:
        if((nError == NO_ERR) && (m_TransUse.GripperSen == true))
        {
            if((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
            {
                nError = ERR_HAND_CAST_EXIST;
				ADD_LOG("[Check_State] E : %d(ERR_HAND_CAST_EXIST)", nError);
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
											ERR_HAND_CAST_EXIST,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pExecuteInfo->ExecutePositionInfo.NextNode,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_pOHTMainStatus->StatusCommon.ExistFoup,
											m_pOHTMainStatus->StatusCommon.StopStationID,
											m_pHWSet->m_pHand->IsOpen(),
											m_pHWSet->m_pHand->IsClose(),
											m_pHWSet->m_pHand->IsLeftGripOn(),
											m_pHWSet->m_pHand->IsRightGripOn());
            }
        }
        break;
    }

    if((nError == NO_ERR) && (m_nTransType != TRANSTYPE_MOVET))
	{
        if(m_nTransType == TRANSTYPE_LOAD)
			dInposLimit = HOIST_HOME_INPOS_LIMIT * 2.0;
        else
            dInposLimit = HOIST_HOME_INPOS_LIMIT;

// 2016-12-08, Add by Labeler
// Hoist Home Sensor 관련 Interlock 변경
//		if((m_TransUse.HoistHomeSen == true) && (m_pHWSet->m_pHoistAxis->IsHoistHome() == false) &&
//            (fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > dInposLimit))

		if((m_TransUse.HoistHomeSen == true) &&
		((m_pHWSet->m_pHoistAxis->IsHoistHome() == false) || (fabs(m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) > dInposLimit)))
		{

            dwStartTime = timeGetTime();
			bIsSenON = false;
            while(1)
            {
                if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 3000) // 3sec
						break;

				if((m_pHWSet->m_pHoistAxis->IsHoistHome() == true) && (fabs(m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) < dInposLimit))
				{
                    bIsSenON = true;
                    break;
                }
                else
                    Sleep(10);
            }
            if(bIsSenON == false)
            {
                nError = ERR_HOISTALIGNSEN_NOT_ON;
				ADD_LOG("[Check_State] E : %d(ERR_HOISTALIGNSEN_NOT_ON)", nError);
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
							ERR_HOISTALIGNSEN_NOT_ON,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHoistAxis->IsHoistHome(),
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
            }
        }
    }

    if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsGoSigOn() == false))
    {
        nCount = 0;
        while(m_pHWSet->m_pHand->IsGoSigOn() == false)
        {
			if(m_pHWSet->m_pHand->IsGoSigOn() == true) // Foup 존재
			{
                nCount = 0;
                break;
            }
            else
            {
                nCount++;
            }
            if(nCount > 5)
            {
                nError = ERR_HANDPIO_GO_OFF;
                nCount = 0;
                ADD_LOG("[Check_State] E : %d(ERR_HANDPIO_GO_OFF)", nError);
				ADD_FDC_LOG("ERR/%d/1/6/%06d/%06d/%d/%d/%06d/%d",
							ERR_HANDPIO_GO_OFF,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsGoSigOn());
                break;
            }
            Sleep(10);
        }
	}

    if((nError == NO_ERR) && (m_nTransType != TRANSTYPE_MOVET))
	{

		if(fabs(m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) > HOIST_ORIGIN_LIMIT)
		{
            nError = ERR_HOIST_NOTORIGIN;
			ADD_LOG("[Check_State] E : %d(ERR_HOIST_NOTORIGIN) %f", nError,m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition());
                        ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%f/%f",
						ERR_HOIST_NOTORIGIN,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_nTransType,
						m_nTransStep,
						fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()),
						HOIST_ORIGIN_LIMIT);
        }
    }

    if(nError == NO_ERR)
    {
        if(m_pHWSet->m_pHoistAxis->IsAMPEnable() == false)
        {
            nError = ERR_AXIS2_SERVO_OFFED;
			ADD_LOG("[Check_State] E : %d(ERR_AXIS2_SERVO_OFFED)", nError);
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
							ERR_AXIS2_SERVO_OFFED,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHoistAxis->IsAMPEnable(),
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
		}

//		if((nError == NO_ERR) && (m_pHWSet->m_pShiftAxis->IsAMPEnable() == false))
//		{
//			nError = ERR_AXIS3_SERVO_OFFED;
//			ADD_LOG("[Check_State] E : %d(ERR_AXIS3_SERVO_OFFED)", nError);
//			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
//							ERR_AXIS3_SERVO_OFFED,
//							m_pOHTMainStatus->StatusCommon.CurNodeID,
//							m_pExecuteInfo->ExecutePositionInfo.NextNode,
//							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//							m_pOHTMainStatus->StatusCommon.ExistFoup,
//							m_pOHTMainStatus->StatusCommon.StopStationID,
//							m_nTransType,
//							m_nTransStep,
//							m_pHWSet->m_pShiftAxis->IsAMPEnable(),
//							m_pHWSet->m_pShiftAxis->GetCurrentPosition());
//		}

// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate Origin 신호 처리 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
		if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsAMPEnable() == false))
		{
			nError = ERR_AXIS4_SERVO_OFFED;
			ADD_LOG("[Check_State] E : %d(ERR_AXIS4_SERVO_OFFED)", nError);
		}
#endif
	}



	return nError;
}


int TransControl::Move_EUV_Shift()
{
	int nError = 0;
	double dVel_mmpermsec = 0.0;
	double dAccel_mmpermsec2 = 0.0;

    DWORD dwTimeOfShutterOpen = timeGetTime();

	//사전동작관련 내용  Interlock 추가 20170123
	if(m_defualtparam->PreShutter)
	{
        if ((m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE) &&
            ((m_pHWSet->m_pShutter->IsOpen(FRONT) == false) || (m_pHWSet->m_pShutter->IsOpen(REAR) == false)))
        {
            if(m_pHWSet->m_pShutter->IsClose(BOTH) == true)
            {
                nError = m_pHWSet->m_pShutter->Open();
            }

            dwTimeOfShutterOpen = timeGetTime();

            while(!(m_pHWSet->m_pShutter->IsOpen(BOTH)))
            {
                if(OHTUTILITY::PreventOverflowGetTimeMs(dwTimeOfShutterOpen) >2000)
                {
                    nError = m_pHWSet->m_pShutter->CheckSensor(SHUTTER_NO_POS);
                    break;
                }
            }
        }
	}
    if(nError != NO_ERR) return nError;

	dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_10P;
	dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;

	nError = m_pHWSet->m_pShiftAxis->MoveDistance
		 (dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, EUV_FIRST_ROTATION_POSITION);

	if(nError == NO_ERR)
	{
		m_dCmdShiftPos = EUV_FIRST_ROTATION_POSITION;
	}
	
	return nError;

}


int TransControl::Move_EUV_Rotate(int nMoveCase)
{
	int nError = 0;

	switch(nMoveCase)
	{
	case SHIFTROTATE_HOME2TP:
		{
			nError = m_pHWSet->m_pRotate->MoveAbsolute((int)m_dRotate_TeachingPos);

			if(nError == NO_ERR)
			{
				m_dCmdRotatePos = m_dRotate_TeachingPos;
			}
		}
		break;

	case SHIFTROTATE_TP2HOME:
		{
			nError = m_pHWSet->m_pRotate->MoveAbsolute((int)0);

			if(nError == NO_ERR)
			{
				m_dCmdRotatePos = 0.0;
			}
		}
		break;
	}		 

	return nError;

}


/**
 @brief   Shift축과 Rotate축을 해당되는 위치로 이동시키는 함수
 @author  LSJ
 @date    2012.11.10
 @param   nMoveCase(어디에서 시작해서 어디로 가는 이동인지 선택)
 @param   SHIFTROTATE_HOME2TP : 원점 위치에서 티칭 위치, SHIFTROTATE_TP2HOME : 티칭 위치에서 원점 위치
 @return  Shift, Rotate 축 이동에 문제가 없으면 0, 문제가 있으면 해당 에러코드 리턴.
 */
int TransControl::Move_ShiftRotate(int nMoveCase)
{
    int nError = 0;
	double dVel_mmpermsec = 0.0;
	double dAccel_mmpermsec2 = 0.0;
    bool bIsHighSpeed = false;
    // static int nCount = 0;

	DWORD dwTimeOfShutterOpen = timeGetTime();

	//사전동작관련 내용  Interlock 추가 20170123
	if(m_defualtparam->PreShutter)
	{
		 if((m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE) && (m_nTransType != TRANSTYPE_SMALLADD_HOIST_HOME)
            && ((m_pHWSet->m_pHand->IsRightGripOn() == false) || (m_pHWSet->m_pHand->IsLeftGripOn() == false)))
		 {
				if(m_pHWSet->m_pShutter->IsClose(BOTH) == true)
					nError = m_pHWSet->m_pShutter->Open();

				dwTimeOfShutterOpen = timeGetTime();
				while(!(m_pHWSet->m_pShutter->IsOpen(BOTH)))
				{
				  if(OHTUTILITY::PreventOverflowGetTimeMs(dwTimeOfShutterOpen)>2000)
				  {
						nError = m_pHWSet->m_pShutter->CheckSensor(SHUTTER_NO_POS);
						break;
				  }
                }

		 }
	}

    if(nError != NO_ERR) return nError;

    // STK Side Port 작업시에는 고속작업..
    if(fabs(m_dShift_TeachingPos) > fabs((double)SHIFT_FAST_MOVE_DIST))
        bIsHighSpeed = true;

    switch(nMoveCase)
	{
    case SHIFTROTATE_HOME2TP: // 원점에서 Teaching Point까지 이동
        // 2016-11-28, Add by Labeler, ROHT의 경우, Rotate 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
		if((nError==NO_ERR)&&(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree() - (int)m_dRotate_TeachingPos) > ROTATE_MOVELIMIT))
        {
			nError = m_pHWSet->m_pRotate->MoveAbsolute((int)m_dRotate_TeachingPos);
			m_dCmdRotatePos = m_dRotate_TeachingPos;
		}
#endif

		switch(m_nTransType)
        {
        case TRANSTYPE_LOAD:
            if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (bIsHighSpeed == true))
			{
                dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_100P * m_dShiftLevel*1.3;
				dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;
				if(nError == NO_ERR)
                    nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec,
					m_dShift_TeachingPos);
            }
            else
			{

				if( fabs(m_dShift_TeachingPos) > 50.0 )
					dVel_mmpermsec = SHIFT_MAX_SPEED *  m_pParamSet->TransDefine.dShift_Speed_Ratio_50Up_Load;
				else
					dVel_mmpermsec = SHIFT_MAX_SPEED * m_pParamSet->TransDefine.dShift_Speed_Ratio_50Down_Load;
				dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;

				//dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_70P;
				//dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_NC_ACCTIME;
                if(nError == NO_ERR)
                    nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec,
					m_dShift_TeachingPos);
			}
            break;
        case TRANSTYPE_UNLOAD:
            if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (bIsHighSpeed == true))
            {
                dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_100P*1.3;
				dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_NC_ACCTIME;
                if(nError == NO_ERR)
                    nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec,
                    m_dShift_TeachingPos);
            }
            else
			{
				/*
				if( fabs(m_dShift_TeachingPos) > 50.0 )
					dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_70P;
				else
					dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_30P;
				dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_NC_ACCTIME;
				*/
				dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_70P;
				dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_NC_ACCTIME;
                if(nError == NO_ERR)
                    nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec,
                    m_dShift_TeachingPos);
            }
            break;
        case TRANSTYPE_MOVET:
            dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_10P;
            dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;
            if(nError == NO_ERR)
				nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dShift_TeachingPos);
            break;
        }


		if(nError == NO_ERR)
        {
            m_dCmdShiftPos = m_dShift_TeachingPos;
            m_dCmdRotatePos = m_dRotate_TeachingPos;
            // 2016-12-06, Add by Labeler
            // T/P로 이동 시, Rotate Step 할당 및 시간 Check
        	dwMoveSRStartTime = timeGetTime();
			nStep_SR_Check = STEP_CHECK_SR_MOVING;
		}

        break;
	case SHIFTROTATE_TP2HOME: // Teaching Point에서 원점까지 이동..
        switch(m_nTransType)
        {
        case TRANSTYPE_LOAD:
            if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (bIsHighSpeed == true))
            {
                dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_100P*1.3;
				dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_NC_ACCTIME;
                if(nError == NO_ERR)
                    nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);
            }
            else
            {
				/*
				if( fabs(m_dShift_TeachingPos) > 50.0 )
					dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_70P;
				else
					dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_30P;
				dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_NC_ACCTIME;
				*/

				dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_70P;
				dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_NC_ACCTIME;
                if(nError == NO_ERR)
                    nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);
            }
            break;
		case TRANSTYPE_UNLOAD:
            if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (bIsHighSpeed == true))
			{
				dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_100P * m_dShiftLevel*1.3;
                dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;
				if(nError == NO_ERR)
                    nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);
            }
            else
            {

				if( fabs(m_dShift_TeachingPos) > 50.0 )
					dVel_mmpermsec = SHIFT_MAX_SPEED *  m_pParamSet->TransDefine.dShift_Speed_Ratio_50Up_UnLoad;
				else
					dVel_mmpermsec = SHIFT_MAX_SPEED * m_pParamSet->TransDefine.dShift_Speed_Ratio_50Down_UnLoad;
				dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;


//				dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_70P;
//				dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_NC_ACCTIME;
                if(nError == NO_ERR)
                    nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);
            }
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
            if ((nError == NO_ERR) &&
				(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) != ROTATE_POS_0) &&
				(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) != ROTATE_POS_180))
            {
				if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) < ROTATE_POS_90)
                {
					nError = m_pHWSet->m_pRotate->MoveAbsolute(ROTATE_POS_0);
					m_dCmdRotatePos = ROTATE_POS_0;
                }
                else
                {
					nError = m_pHWSet->m_pRotate->MoveAbsolute(ROTATE_POS_180);
					m_dCmdRotatePos = ROTATE_POS_180;
                }
            }
#endif
            break;

        case TRANSTYPE_MOVE_ALL_HOME:
            if( fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) > 50.0 )
                dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_30P;
            else
                dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_10P;

            dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;

            if(nError == NO_ERR)
                nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
            // if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() == true))
//                nError = m_pHWSet->m_pRotate->MoveAbsolute(0);
#endif
			break;

		case TRANSTYPE_AUTO_CHANGE:
            if( fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) > 50.0 )
                dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_30P;
            else
				dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_10P;

            dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;

            if(nError == NO_ERR)
                nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
//            if(nError == NO_ERR)
            //    nError = m_pHWSet->m_pRotate->MoveAbsolute(0);
#endif
            break;

		}

        // 2016-11-28, Rotate, Add by Labeler, ROHT의 경우 Rotate 추가로 인해, Rotate 0 도로 이동 추가

        if(nError == NO_ERR)
        {
            m_dCmdShiftPos = 0.0;

            if((m_nTransType == TRANSTYPE_MOVE_ALL_HOME) || (m_nTransType == TRANSTYPE_AUTO_CHANGE))
                m_dCmdRotatePos = 0.0;
			else
			{
				if((nMoveCase !=  SHIFTROTATE_TP2HOME) && (m_nTransType != TRANSTYPE_UNLOAD))
					m_dCmdRotatePos = m_dRotate_TeachingPos;
			}
            //            m_dCmdRotatePos = 0.0;
            // 2016-12-05, Shift, Rotate Home 위치 이동 시작 시간 및 Check Step 변수 설정
			dwMoveSRStartTime = timeGetTime();
			nStep_SR_Check = STEP_CHECK_SR_ARRIVE_HOME;
			m_mccparam.bRotate_Moving = true;
		}
        break;
	}

	m_mccparam.dShift_Acc = dAccel_mmpermsec2;
	m_mccparam.dShift_Dec = dAccel_mmpermsec2;
	m_mccparam.dShift_Vel = dVel_mmpermsec;

    return nError;
}

/**
 @brief   Shift축과 Rotate축이 명령 받은 위치로 이동이 완료되었는지 확인하는 함수
 @author  Labeler
 @date    2016.12.05
 @return  에러가 없으면 0, 에러가 발생되면 에러코드 리턴.
 */
 #if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
int TransControl::Check_ShiftRotate()
{
	int nError = NO_ERR;
	static bool bActOutTriggerCmd = false;
    static int nCntGripMisMatch = 0;
    static int nCntNotDetectGrip = 0;
	static int nCntDetectGrip = 0;
	static int nCntNotHandInpo = 0;
    static int nCntNotHandCloseDetect = 0;
    static int nCntShutterNotOpen = 0;
	DWORD dwSleepTime = 0;

    // Shift, Rotate 축 구동 중, 일정 시간 이상, Grip Sensor가 미스매치 날 경우 Error 처리
    if(m_pHWSet->m_pInnerFoup->IsFoupDetect() == true)
    {
        if((m_pHWSet->m_pHand->IsLeftGripOn() != true) || (m_pHWSet->m_pHand->IsRightGripOn() != true))
         {
            if(nCntNotDetectGrip > 20)
            {
                if(m_bSlipringFlag)
                {
                    nCntNotDetectGrip = 0;
                    m_bSlipringFlag = false;
					nError = ERR_SEN_ABNORMAL_HAND;
                    ADD_LOG("[Check_ShiftRotate]E:%d(ERR_SEN_ABNORMAL_HAND)",nError);
                    ADD_LOG("[Check_ShiftRotate]ERR_HAND_CAST_NOTEXIST");
					ADD_FDC_LOG("ERR/%d/3/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_SEN_ABNORMAL_HAND,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
                }
                else
                {
                    nCntNotDetectGrip = 0;
                    nError = ERR_HAND_CAST_NOTEXIST;
                    ADD_LOG("[Check_ShiftRotate]E:%d(ERR_HAND_CAST_NOTEXIST)",nError);
					ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
											ERR_HAND_CAST_NOTEXIST,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pExecuteInfo->ExecutePositionInfo.NextNode,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_pOHTMainStatus->StatusCommon.ExistFoup,
											m_pOHTMainStatus->StatusCommon.StopStationID,
											m_pHWSet->m_pHand->IsOpen(),
											m_pHWSet->m_pHand->IsClose(),
											m_pHWSet->m_pHand->IsLeftGripOn(),
											m_pHWSet->m_pHand->IsRightGripOn());
                }

            }
            else
                nCntNotDetectGrip++;
        }
        else
            nCntNotDetectGrip = 0;
	}
    else
    {
//        if((m_pHWSet->m_pHand->IsLeftGripOn() == true) || (m_pHWSet->m_pHand->IsRightGripOn() == true))
//        {
//            if(nCntDetectGrip > 20)
//            {
//                nError = ERR_HAND_CAST_EXIST;
//                nCntDetectGrip = 0;
//            }
//            else
//                nCntDetectGrip++;
//        }
//        else
//            nCntDetectGrip = 0;
    }

    /*
    if(m_pHWSet->m_pHand->IsLeftGripOn() != m_pHWSet->m_pHand->IsRightGripOn())
    {
        if(nCntGripMisMatch > 20)
        {
            nError = ERR_GRIP_SEN_MISMATCH;
            nCntGripMisMatch = 0;
        }
        else
            nCntGripMisMatch++;
    }
    else
    {
        nCntGripMisMatch = 0;
    }
    */




    if((m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE))
    {
        if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsInposition() == false))
        {
            if(nCntNotHandInpo > 20)
            {
                nError = ERR_HAND_INPOSITION_ERROR;
                nCntNotHandInpo = 0;
				ADD_FDC_LOG("ERR/%d/4/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_HAND_INPOSITION_ERROR,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_pHWSet->m_pHand->IsInposition(),
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose());
            }
            else
                nCntNotHandInpo++;
        }
        else if(m_pHWSet->m_pHand->IsInposition() == true)
        {
            nCntNotHandInpo = 0;
        }

		if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsRunning() == true))
		{
			if(nCntShutterNotOpen > 5)
			{
                nError = m_pHWSet->m_pShutter->CheckSensor(SHUTTER_OPEN_POS);
                nCntShutterNotOpen = 0;
            }

            if(m_pHWSet->m_pShutter->CheckSensor(SHUTTER_OPEN_POS) != NO_ERR )
            {
                nCntShutterNotOpen++;
            }
            else
			{
                nCntShutterNotOpen = 0;
            }
        }
        else
        {
            nCntShutterNotOpen = 0;
        }
    }

    if((nError == NO_ERR)
    && (((m_nTransType == TRANSTYPE_LOAD) && (m_nTransStep == STEP_L_CHECK_MOVE_SHIFTROTATE))
    || ((m_nTransType == TRANSTYPE_UNLOAD) && (m_nTransStep == STEP_U_CHECK_MOVE_SHIFTROTATE_HOME))))
    {
        if((m_pHWSet->m_pHand->IsClose() != true) || (m_pHWSet->m_pHand->IsOpen() == true))
        {

            ADD_LOG("[Check_ShiftRotate] Hand Close : %d", m_pHWSet->m_pHand->IsClose());
            if(nCntNotHandCloseDetect > 20)
			{
                nError = ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD;
                nCntNotHandCloseDetect = 0;
				ADD_FDC_LOG("ERR/%d/2/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsOpen(),
							m_pHWSet->m_pHand->IsClose());
            }
            else
            {
                nCntNotHandCloseDetect++;
            }
        }
        else
        {
			nCntNotHandCloseDetect = 0;
        }
    }


	// 2016-12-08, Add by Labeler
	// Rotate 동작 중, Hoist 축이 Home 상태가 아닐 경우 에러 처리

	if((nError == NO_ERR) && (m_TransUse.HoistHomeSen == true) &&
	((m_pHWSet->m_pHoistAxis->IsHoistHome() == false) || (fabs(m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) >= HOIST_HOME_INPOS_LIMIT)))
		{
        DWORD dwStartTime = timeGetTime();

		while(1)
        {
//            if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 3000) // 3sec
		   if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 3000) // Shift 움직임으로 Hoist Home sensor 흔들리는 문제 해결 위해, shift chattering 2sec ->3sec 증가 soming
			{
				nError = ERR_HOISTALIGNSEN_NOT_ON;
                ADD_LOG("[Check_ShiftRotate] E : %d(ERR_HOISTALIGNSEN_NOT_ON)", nError);
				ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
							ERR_HOISTALIGNSEN_NOT_ON,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHoistAxis->IsHoistHome(),
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
                break;
			}

			if((m_pHWSet->m_pHoistAxis->IsHoistHome() == true) && (fabs(m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) < HOIST_HOME_INPOS_LIMIT))
			{
                break;
            }

            Sleep(10);
        }
    }

    // Step 동작 전, Error 발생한 경우, Return 처리
    if(nError != NO_ERR)
    {
//        nCntGripMisMatch = 0;
        nCntDetectGrip = 0;
        nCntNotDetectGrip = 0;
        nCntNotHandInpo = 0;
        nCntNotHandCloseDetect = 0;
        nCntShutterNotOpen = 0;
        return nError;
    }

	switch(nStep_SR_Check)
	{
		case STEP_CHECK_SR_ARRIVE_HOME:  //
		{
            bool bRotateArrived = false;
			if((m_dCmdRotatePos != 0.0) && (m_dCmdShiftPos != 0.0))
			{
				nStep_SR_Check = STEP_CHECK_SR_MOVING;
				break;
			}

            if((m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE))
			{

                if(m_dCmdRotatePos == 0.0)
				{
					if((fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > ROTATE_INPOS_LIMIT) || (m_pHWSet->m_pRotate->IsRunning() == true))
                    {
                        if(OHTUTILITY::PreventOverflowGetTimeMs(dwMoveSRStartTime) > ROTATE_EXEC_TIMEOUT)
                        {
                            nError = ERR_ROTATE_NOT_0;
    //                        nCntGripMisMatch = 0;
                            nCntDetectGrip = 0;
                            nCntNotDetectGrip = 0;
                            nCntNotHandInpo = 0;
                            nCntNotHandCloseDetect = 0;
                            nCntShutterNotOpen = 0;
                            return nError;
                        }
                    }
                    else
                    {
                        bRotateArrived = true;
                    }
                }
                else
                {
                    bRotateArrived = true;
                }
            }
            else
            {
                bRotateArrived = true;
            }

//			if((nError == NO_ERR) && (m_dCmdShiftPos == 0.0))
//			{
//
////				if((m_pHWSet->m_pShiftAxis->IsShiftHome() == false) || (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) >= RETICLE_SHIFT_INPOS_LIMIT) || (m_pHWSet->m_pShiftAxis->IsRunning() == true))
////				{
////					if(OHTUTILITY::PreventOverflowGetTimeMs(dwMoveSRStartTime) > 5000)
////					{
////						nError = ERR_SHIFT_HOMESEN_NOTON;
////						ADD_LOG("[Check_ShiftRotate] E : %d(ERR_SHIFT_HOMESEN_NOTON)", nError);
////                        bActOutTriggerCmd = false;
//////                        nCntGripMisMatch = 0;
////                        nCntDetectGrip = 0;
////                        nCntNotDetectGrip = 0;
////                        nCntNotHandInpo = 0;
////                        nCntNotHandCloseDetect = 0;
////                        nCntShutterNotOpen = 0;
////						ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
////									ERR_SHIFT_HOMESEN_NOTON,
////									m_pOHTMainStatus->StatusCommon.CurNodeID,
////									m_pExecuteInfo->ExecutePositionInfo.NextNode,
////									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
////									m_pOHTMainStatus->StatusCommon.ExistFoup,
////									m_pOHTMainStatus->StatusCommon.StopStationID,
////									m_nTransType,
////									m_nTransStep,
////									m_pHWSet->m_pShiftAxis->IsShiftHome());
////						return nError;
////					}
////				}
////				else if(bRotateArrived == true)
////				{
////					bActOutTriggerCmd = false;
////    			 	nStep_SR_Check = STEP_CHECK_SR_MOVING;
////				}
//			}
        }
		break;


		case STEP_CHECK_SR_MOVING:  // 구동 중 Check
		{

            // 2016-12-06, Add by Labeler
            // Teaching Point로 Roate 이동 명령 발생 시, 일정 시간 내, 도착되지 않을 경우 Error 처리
            if(((TRANSTYPE_LOAD == m_nTransType) && (STEP_L_CHECK_MOVE_SHIFTROTATE == m_nTransStep))
            || ((TRANSTYPE_UNLOAD == m_nTransType) && (STEP_U_CHECK_MOVE_SHIFTROTATE == m_nTransStep))
            || ((TRANSTYPE_MOVET == m_nTransType) && (STEP_MOVE_TP_CHECK_MOVE_SHIFTROTATE == m_nTransStep)))
            {
				if(fabs(m_dCmdRotatePos - (double)m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > ROTATE_INPOS_LIMIT)
				{
					if(OHTUTILITY::PreventOverflowGetTimeMs(dwMoveSRStartTime) > ROTATE_EXEC_TIMEOUT)
					{
						nError = ERR_TRANS_AXIS3MOVE_TIMEOVER;
						ADD_LOG("[Check_ShiftRotate] E : %d(ERR_TRANS_AXIS3MOVE_TIMEOVER 1)", nError);
//                        nCntGripMisMatch = 0;
                        nCntDetectGrip = 0;
						nCntNotDetectGrip = 0;
						nCntNotHandInpo = 0;
                        nCntNotHandCloseDetect = 0;
                        nCntShutterNotOpen = 0;
						return nError;
					}
				}
            }

			if((m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE))
			{
				if(m_pHWSet->m_pRotate->IsOrigin() == false)
				{
					break;
                }
            }

			if(m_pHWSet->m_pRotate->IsRunning() == true)
			{
				break;
            }
			else if(m_pHWSet->m_pShiftAxis->IsRunning() == true)
            {
                break;
            }
			else if((m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE)
            && (fabs(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree())-fabs((int)m_dCmdRotatePos)) > ROTATE_INPOS_LIMIT))
			{
				break;
			}
			else if((m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE)
            && (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition() - m_dCmdShiftPos) > RETICLE_SHIFT_INPOS_LIMIT))
            {
                if(OHTUTILITY::PreventOverflowGetTimeMs(dwMoveSRStartTime) > 5000)
                {
                    nError = ERR_TRANS_AXIS3MOVE_TIMEOVER;
					ADD_LOG("[Check_ShiftRotate] E : %d(ERR_TRANS_AXIS3MOVE_TIMEOVER 2)", nError);
                    nCntDetectGrip = 0;
                    nCntNotDetectGrip = 0;
                    nCntNotHandInpo = 0;
                    nCntNotHandCloseDetect = 0;
                    nCntShutterNotOpen = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%d/%f",
								ERR_TRANS_AXIS3MOVE_TIMEOVER,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pExecuteInfo->ExecutePositionInfo.NextNode,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_pOHTMainStatus->StatusCommon.ExistFoup,
								m_pOHTMainStatus->StatusCommon.StopStationID,
								m_nTransType,
								m_nTransStep,
								m_pHWSet->m_pShiftAxis->GetCurrentPosition());
                    return nError;
                }

                break;
            }
			else
			{
				// ROHT의 경우, Rotate, Shift 동작 완료 후 Shutter Close
				if((m_nTransType == TRANSTYPE_UNLOAD)
				&& (m_StatusSet.uStep == STEP_U_CHECK_MOVE_SHIFTROTATE_HOME)
				&& (m_dCmdRotatePos == 0.0))
				{
					m_pHWSet->m_pShutter->SendCloseCmd();
				}
				nStep_SR_Check = STEP_CHECK_SR_INCREASE_TRANS_STEP;
			}
        }
		break;

		case STEP_CHECK_SR_INCREASE_TRANS_STEP:	 // 이적재 Step 증가
		{
        	if((m_nTransStep == STEP_L_CHECK_MOVE_SHIFTROTATE)||(m_nTransStep == STEP_U_CHECK_MOVE_SHIFTROTATE))
			{
                dwSleepTime = timeGetTime();
				while(1)
				{
					Sleep(10);
					// by zzang9un 2012.12.06 : STB L, R 구분
					if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
					{
						if(OHTUTILITY::PreventOverflowGetTimeMs(dwSleepTime) > 200)
						{
							m_nTransStep++;
                            nStep_SR_Check++;   // 다시 해당 함수 들어올 경우를 막기 위해 증가
//                            nCntGripMisMatch = 0;
                            nCntDetectGrip = 0;
                            nCntNotDetectGrip = 0;
                            nCntNotHandInpo = 0;
                            nCntNotHandCloseDetect = 0;
                            nCntShutterNotOpen = 0;

							ADD_MCC_LOG(",%s,TRANS,ROTATE,End,FOUP,%06d,%d,%06d,%d,Rotate Cmd Pos,%d,Rotate Moving Time,%d,Rotate 0 On Degree,%d,Rotate 0 Off Degree,%d,Rotate 180 On Degree,%d,Rotate 180 Off Degree,%d",
										m_defualtparam->VHLName,
										m_pOHTMainStatus->StatusCommon.CurStationID,
										m_mccparam.nEqType,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_dCmdRotatePos,
										m_mccparam.nRotateMovingTime,
										m_mccparam.nRotate0OnDegree,
										m_mccparam.nRotate0OffDegree,
										m_mccparam.nRotate180OnDegree,
										m_mccparam.nRotate180OffDegree);

							m_mccparam.nRotateMovingTime = 0;
							m_mccparam.nRotate0OnDegree = 0;
							m_mccparam.nRotate0OffDegree = 0;
							m_mccparam.nRotate180OnDegree = 0;
							m_mccparam.nRotate180OffDegree = 0;
							break;
						}
					}
					else
					{
						if(OHTUTILITY::PreventOverflowGetTimeMs(dwSleepTime) > 100)
						{
							m_nTransStep++;
                            nStep_SR_Check++;   // 다시 해당 함수 들어올 경우를 막기 위해 증가
//                            nCntGripMisMatch = 0;
                            nCntDetectGrip = 0;
                            nCntNotDetectGrip = 0;
                            nCntNotHandInpo = 0;
                            nCntNotHandCloseDetect = 0;
                            nCntShutterNotOpen = 0;

							ADD_MCC_LOG(",%s,TRANS,ROTATE,End,FOUP,%06d,%d,%06d,%d,Rotate Cmd Pos,%d,Rotate Moving Time,%d,Rotate 0 On Degree,%d,Rotate 0 Off Degree,%d,Rotate 180 On Degree,%d,Rotate 180 Off Degree,%d",
										m_defualtparam->VHLName,
										m_pOHTMainStatus->StatusCommon.CurStationID,
										m_mccparam.nEqType,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_dCmdRotatePos,
										m_mccparam.nRotateMovingTime,
										m_mccparam.nRotate0OnDegree,
										m_mccparam.nRotate0OffDegree,
										m_mccparam.nRotate180OnDegree,
										m_mccparam.nRotate180OffDegree);

							m_mccparam.nRotateMovingTime = 0;
							m_mccparam.nRotate0OnDegree = 0;
							m_mccparam.nRotate0OffDegree = 0;
							m_mccparam.nRotate180OnDegree = 0;
							m_mccparam.nRotate180OffDegree = 0;
							break;
						}
					}
				}
			}
			else
			{
				m_nTransStep++;
                nStep_SR_Check++;   // 다시 해당 함수 들어올 경우를 막기 위해 증가
//                nCntGripMisMatch = 0;
                nCntDetectGrip = 0;
                nCntNotDetectGrip = 0;
                nCntNotHandInpo = 0;
                nCntNotHandCloseDetect = 0;
                nCntShutterNotOpen = 0;

				ADD_MCC_LOG(",%s,TRANS,ROTATE,End,FOUP,%06d,%d,%06d,%d,Rotate Cmd Pos,%d,Rotate Moving Time,%d,Rotate 0 On Degree,%d,Rotate 0 Off Degree,%d,Rotate 180 On Degree,%d,Rotate 180 Off Degree,%d",
							m_defualtparam->VHLName,
							m_pOHTMainStatus->StatusCommon.CurStationID,
							m_mccparam.nEqType,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_dCmdRotatePos,
							m_mccparam.nRotateMovingTime,
							m_mccparam.nRotate0OnDegree,
							m_mccparam.nRotate0OffDegree,
							m_mccparam.nRotate180OnDegree,
							m_mccparam.nRotate180OffDegree);

				m_mccparam.nRotateMovingTime = 0;
				m_mccparam.nRotate0OnDegree = 0;
				m_mccparam.nRotate0OffDegree = 0;
				m_mccparam.nRotate180OnDegree = 0;
				m_mccparam.nRotate180OffDegree = 0;
			}
        }
		break;

		default:
		break;
	}

   /*
	static int nRotateCheckSensorCount	= 0;
	if(m_defualtparam->IsRotateSensor == true)
	{
		if(m_pHWSet->m_pRotate->IsRunning() == false)
		{
	   //	ADD_ERR_LOG("CheckAlways : %d :Count:%d",m_pHWSet->m_pRotate->GetCurrentPositionDegree(), nRotateCheckSensorCount);
			if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_0) < ROTATE_INPOS_CHECK_SENSOR)
			{

				if(m_pHWSet->m_pRotate->IsRotate0Sensor() == OFF)
				{
					nRotateCheckSensorCount++;
					if(nRotateCheckSensorCount > 50)
					{
						ADD_ERR_LOG("Check_ShiftRotate Ratoate 0: %d",m_pHWSet->m_pRotate->GetCurrentPositionDegree());
						return ERR_ROTATE_0_TIMEOVER;
					}
				}else
					nRotateCheckSensorCount	= 0;
			}
			else if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_90) <ROTATE_INPOS_CHECK_SENSOR)
			{
				if(m_pHWSet->m_pRotate->IsRotate90Sensor() == OFF)
				{
					nRotateCheckSensorCount++;
					if(nRotateCheckSensorCount > 50)
					{
						ADD_ERR_LOG("Check_ShiftRotate Ratoate 90: %d",m_pHWSet->m_pRotate->GetCurrentPositionDegree());
						return ERR_ROTATE_90_TIMEOVER;
					}
				}else
					nRotateCheckSensorCount	= 0;
			}
			else if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_180) <ROTATE_INPOS_CHECK_SENSOR)
			{
				if(m_pHWSet->m_pRotate->IsRotate180Sensor() == OFF)
				{
					nRotateCheckSensorCount++;
					if(nRotateCheckSensorCount > 50)
					{
						ADD_ERR_LOG("Check_ShiftRotate Ratoate 180: %d",m_pHWSet->m_pRotate->GetCurrentPositionDegree());
						return ERR_ROTATE_180_TIMEOVER;
					}
				}else
					nRotateCheckSensorCount	= 0;
			}
			else if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_270) <ROTATE_INPOS_CHECK_SENSOR)
			{
				if(m_pHWSet->m_pRotate->IsRotate270Sensor() == OFF)
				{
					nRotateCheckSensorCount++;
					if(nRotateCheckSensorCount > 50)
						return ERR_ROTATE_270_TIMEOVER;
				}else
					nRotateCheckSensorCount	= 0;
			}
			else
				nRotateCheckSensorCount	= 0;
		}else
			nRotateCheckSensorCount	= 0;
	}
	*/

    if(nError != NO_ERR)
    {
//        nCntGripMisMatch = 0;
        nCntDetectGrip = 0;
        nCntNotDetectGrip = 0;
        nCntNotHandInpo = 0;
        nCntNotHandCloseDetect = 0;
        nCntShutterNotOpen = 0;
        bActOutTriggerCmd = false;
    }

	if((m_mccparam.bpreRotate0 == false) && (m_pHWCommon->m_Input.Rotate_0_Sensor == true))
	{
		m_mccparam.nRotate0OnDegree = m_pHWSet->m_pRotate->GetCurrentPositionDegree();
	}

	if((m_mccparam.bpreRotate0 == true) && (m_pHWCommon->m_Input.Rotate_0_Sensor == false))
	{
		m_mccparam.nRotate0OffDegree = m_pHWSet->m_pRotate->GetCurrentPositionDegree();
	}

	if((m_mccparam.bpreRotate180 == false) && (m_pHWCommon->m_Input.Rotate_180_Sensor == true))
	{
		m_mccparam.nRotate180OnDegree = m_pHWSet->m_pRotate->GetCurrentPositionDegree();
	}

	if((m_mccparam.bpreRotate180 == true) && (m_pHWCommon->m_Input.Rotate_180_Sensor == false))
	{
		m_mccparam.nRotate180OffDegree = m_pHWSet->m_pRotate->GetCurrentPositionDegree();
	}

	m_mccparam.bpreRotate0 = m_pHWCommon->m_Input.Rotate_0_Sensor;
	m_mccparam.bpreRotate180 = m_pHWCommon->m_Input.Rotate_180_Sensor;

	if(m_mccparam.bRotate_Moving == true)
	{
		if((m_pHWSet->m_pRotate->IsRunning() == false) &&
		(fabs(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree())-fabs((int)m_dCmdRotatePos)) < ROTATE_INPOS_LIMIT))
		{
			m_mccparam.nRotateMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwMoveSRStartTime);
			m_mccparam.bRotate_Moving = false;
		}
	}

	return nError;
}
#else
/**
 @brief   Shift축과 Rotate축이 명령 받은 위치로 이동이 완료되었는지 확인하는 함수
 @author  LSJ
 @date    2012.11.10
 @return  에러가 없으면 0, 에러가 발생되면 에러코드 리턴.
 */
 int TransControl::Check_ShiftRotate()
{
    int nError = 0;

	DWORD dwStartTime = 0;
	DWORD dwSleepTime = 0;
	bool bIsError = true;

    if(m_dCmdShiftPos == 0.0)
	{


		if(m_pHWSet->m_pShiftAxis->IsShiftHome() == false)
        {
            dwStartTime = timeGetTime();
			bIsError = true;
            while(1)
            {
				//MCC Sensor On/Off시 Shift Position 감시---------------------------
				if((m_mccparam.bpreFOUPCoverSensor == true) && (m_pHWSet->m_pFoupCoverOpenDetect->IsDetect() == false))
				{
					m_mccparam.dFOUPCover_OffPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
				}

				if((m_mccparam.bpreFOUPDetectSensor == false) && (m_pHWSet->m_pInnerFoup->IsFoupDetect() == true))
				{
					m_mccparam.dFOUPDetect_OnPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
				}

				if((m_mccparam.bpreSTBLeftDetectSensor == true) && (m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect() == false))
				{
					m_mccparam.dLeftSTBDetect_OnPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
				}

				if((m_mccparam.bpreSTBRightDetectSensor == true) && (m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect() == false))
				{
					m_mccparam.dRightSTBDetect_OnPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
				}
				m_mccparam.bpreFOUPCoverSensor = m_pHWSet->m_pFoupCoverOpenDetect->IsDetect();
				m_mccparam.bpreFOUPDetectSensor = m_pHWSet->m_pInnerFoup->IsFoupDetect();
				m_mccparam.bpreSTBLeftDetectSensor = m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect();
				m_mccparam.bpreSTBRightDetectSensor = m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect();
				//-----------------------------------------------------------------------
				Sleep(10);
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 5000)
                    break;
                if((m_pHWSet->m_pShiftAxis->IsShiftHome() == true) || (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < 30.0))
                {
                    bIsError = false;
                    break;
                }
            }
//			if((bIsError == true) && (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) > INPOS_LIMIT))
//            {
//				nError = ERR_SHIFT_HOME_NOT_POSITIONERROR;
//				ADD_LOG("[Check_ShiftRotate] E : %d(ERR_SHIFT_HOMESEN_NOTON)", nError);
//				ADD_FDC_LOG("ERR/%d/3/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
//							ERR_SHIFT_HOMESEN_NOTON,
//							m_pOHTMainStatus->StatusCommon.CurNodeID,
//							m_pExecuteInfo->ExecutePositionInfo.NextNode,
//							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//							m_pOHTMainStatus->StatusCommon.ExistFoup,
//							m_pOHTMainStatus->StatusCommon.StopStationID,
//							m_nTransType,
//							m_nTransStep,
//							m_pHWSet->m_pShiftAxis->IsShiftHome());
//                return nError;
//            }
		}

	}
        //MCC Sensor On/Off시 Shift Position 감시---------------------------
	if((m_mccparam.bpreShiftHomeSensor == true) && (m_pHWSet->m_pShiftAxis->IsShiftHome() == false))
	{
		m_mccparam.dShift_HomeOffPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
	}

	if((m_mccparam.bpreShiftHomeSensor == false) && (m_pHWSet->m_pShiftAxis->IsShiftHome() == true))
	{
		m_mccparam.dShift_HomeOnPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
	}

	if((m_mccparam.bpreFOUPCoverSensor == false) && (m_pHWSet->m_pFoupCoverOpenDetect->IsDetect() == true))
	{
		m_mccparam.dFOUPCover_OnPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
	}

	if((m_mccparam.bpreFOUPDetectSensor == true) && (m_pHWSet->m_pInnerFoup->IsFoupDetect() == false))
	{
		m_mccparam.dFOUPDetect_OffPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
	}

	if((m_mccparam.bpreSTBLeftDetectSensor == false) && (m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect() == true))
	{
		m_mccparam.dLeftSTBDetect_OffPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
	}

	if((m_mccparam.bpreSTBRightDetectSensor == false) && (m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect() == true))
	{
		m_mccparam.dRightSTBDetect_OffPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
	}

	m_mccparam.bpreShiftHomeSensor = m_pHWSet->m_pShiftAxis->IsShiftHome();
	m_mccparam.bpreFOUPCoverSensor = m_pHWSet->m_pFoupCoverOpenDetect->IsDetect();
	m_mccparam.bpreFOUPDetectSensor = m_pHWSet->m_pInnerFoup->IsFoupDetect();
	m_mccparam.bpreSTBLeftDetectSensor = m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect();
	m_mccparam.bpreSTBRightDetectSensor = m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect();
	//-----------------------------------------------------------------------

	if(m_pHWSet->m_pShiftAxis->IsRunning() == true)
    {
        return nError;
    }

    if(fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition() - m_dCmdShiftPos) > INPOS_LIMIT)
    {
		return nError;
    }

	if((m_nTransStep == STEP_L_CHECK_MOVE_SHIFTROTATE)||(m_nTransStep == STEP_U_CHECK_MOVE_SHIFTROTATE))
	{
		dwSleepTime = timeGetTime();
		while(1)
		{
			Sleep(10);
			// by zzang9un 2012.12.06 : STB L, R 구분
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
			{
				//if(OHTUTILITY::PreventOverflowGetTimeMs(dwSleepTime) > 200)
				//{
					m_nTransStep++;
					break;
				//}
			}
			else
			{
				//if(OHTUTILITY::PreventOverflowGetTimeMs(dwSleepTime) > 100)
				//{
					m_nTransStep++;
					break;
				//}
			}
		}
	}
	else
	{
		m_nTransStep++;
	}
    return nError;

}
#endif

int TransControl::Check_EUV_Shift()
{
	bool bIsError = true;
	DWORD dwStartTime = timeGetTime();
	
	while(1)
	{
		Sleep(10);
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 5000)
			break;
		
		if(fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition() - m_dCmdShiftPos) <= RETICLE_SHIFT_INPOS_LIMIT)
		{
			bIsError = false;
			break;
		}
	}

	if(bIsError == true)
		return ERR_SHIFT_MOVE_TIME_OUT;
	else
		return NO_ERR;
}

int TransControl::Check_EUV_Rotate()
{
	bool bIsError = true;
	DWORD dwStartTime = timeGetTime();
	int nError = NO_ERR;

	static int nCntNotHandInpo = 0;
	static int nCntShutterNotOpen = 0;

	while(1)
	{
		Sleep(10);
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 10000)
			break;

		if((m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE))
		{
			if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsInposition() == false))
			{
				if(nCntNotHandInpo > 20)
				{
					nError = ERR_HAND_INPOSITION_ERROR;
					nCntNotHandInpo = 0;
				}
				else
					nCntNotHandInpo++;
			}
			else if(m_pHWSet->m_pHand->IsInposition() == true)
			{
				nCntNotHandInpo = 0;
			}

			if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsRunning() == true))
			{
				if(nCntShutterNotOpen > 5)
				{
					nError = m_pHWSet->m_pShutter->CheckSensor(SHUTTER_OPEN_POS);
					nCntShutterNotOpen = 0;
				}

				if(m_pHWSet->m_pShutter->CheckSensor(SHUTTER_OPEN_POS) != NO_ERR )
				{
					nCntShutterNotOpen++;
				}
				else
				{
					nCntShutterNotOpen = 0;
				}
			}
			else
			{
				nCntShutterNotOpen = 0;
			}
		}


		if(nError != NO_ERR)
		{
            break;
		}

		if(fabs(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree())-fabs((int)m_dCmdRotatePos)) <= ROTATE_INPOS_LIMIT)
		{
			if((m_pHWSet->m_pRotate->IsRunning() == false) && (m_pHWSet->m_pRotate->IsOrigin() == true))
			{
				bIsError = false;
				break;
			}
		}
	}

	ADD_LOG("[Check_EUV_Rotate] %d",OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));

	if(bIsError == true)
		return ERR_ROTATE_MOVE_TIME_OUT;
	else
		return nError;
}


/**
 @brief   이적재 동작 중 LookDown 센서 상태 확인하여 Hoist 동작을 멈추거나 다시 동작 시키고, Hand의 상태에 따라 재하 센서를 감시하고 Oscillation 센서를 감시하는 함수
 @author  LSJ
 @date    2012.11.10
 @param   bLookDnCheck : Looddown 센서 감지를 확인할지 결정, 감시하면 true, 감시하지 않으면 false
 @param   nHoistState : Hoist의 상태가 Down하고 있는 상태인지, Up 하고 있는 상태인지 여부
 @param   nPreStep : Hoist의 상태가 명령 받은 위치까지 도착하지 못했을 때 Step을 명시함
 @param   nNextStep : Hoist의 상태가 명령 받은 위치까지 도착했을 때 Step을 명시함
 @return  센서들에 문제가 없으면 0, 문제가 발생되면 해당 에러 코드 리턴
//AOHC-125 Auto 전환 전, 자가 진단 기능 추가 -> Check_Sensor 함수에 bHoistPauseUse 추가
*/
int TransControl::Check_Sensor(bool bLookDnCheck, int nHoistState, int nPreStep, int nNextStep, bool bHoistPauseUse)
{

    int nError = 0;
    static bool bCheckLookdownArea = false;
	bool bLookDnSenCheck = bLookDnCheck;
	static int nOsciCount = 0;
    static int nOsciPauseCount = 0;
	static int nLoadPushCount = 0;
	static int nUnloadPushCount = 0;
	static int nHandPIOGoCount = 0;
	static int nHandPosCount = 0;
	double dOscillationCheckDist = 0.0;
	static int nshutterCount =0;
	static int nHandPIOPauseCount =0;

	static int nHandCloseSenOff_Before_Load = 0;
	static int nHandOpenSenOff_In_Load = 0;
	static int nHandOpenSenOn_In_UnLoad = 0;
	static int nHandCloseSenOff_In_UnLoad = 0;
	static int nHandOpenSenOff_Before_UnLoad = 0;
	static int nHandCloseOn_Before_UnLoad = 0;



    /*******************************************************/
	// Teaching 상태 확인 코드 (임시 Test)
	double dCurrentPosition = 0.0;
    /*******************************************************/
	// Lookdown Detect check
	if(m_TransUse.LookdownSen == true)
    {
		if(bLookDnSenCheck == true)
        {
			if(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) < m_pParamSet->TransDefine.dLookdn_Detect_Check_Dist)
                bLookDnSenCheck = false;
        }

		if(bLookDnSenCheck == true)
		{
			if(m_pHWSet->m_pLookdown->IsError() == true)
            {
				nError = ERR_LOOKDOWN_SENSOR_FAIL;
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
							ERR_LOOKDOWN_SENSOR_FAIL,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pLookdown->IsError(),
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
			}
            else if(m_pHWSet->m_pLookdown->IsDetect() == true)
            {
				if(bLookdown_Detected == false)
				{
                    m_IsCheckPause =true; //Pause 동작시 발동
					bLookdown_Detected = true;
					m_dwLookdn_STTime = timeGetTime();
                    nError = m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
					m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_LP);
					 ADD_MD_LOG("LP St : %d",m_StationID);
                    if(m_TransUse.ObsDetectSound == true)
                        m_pHWSet->Sound_On(SETSOUND_LOOKDOWN);

					ADD_LOG("Lookdn ON[Step=%d]: cmdPos=%.02f curPos = %.02f", m_nTransStep, m_dCmdHoistPos,
                        m_pHWSet->m_pHoistAxis->GetCurrentPosition());
					ADD_MCC_LOG(",%s,TRANS,LOOKDOWN DETECT,Start,FOUP,%06d,%d,%06d,%d,LookDown Hoist Pos,%f,LookDown Shift Pos,%f",
								m_defualtparam->VHLName,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
								m_pHWSet->m_pShiftAxis->GetCurrentPosition());
                }
                else
                {
                    if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLookdn_STTime) > (DWORD)m_pParamSet->TransDefine.nLookdn_Detect_Timeover)
                    {
                        bCheckLookdownArea = false;
                        m_pHWSet->m_pLookdown->CheckSensorArea();
                        m_pHWSet->m_pLookdown->PBSDetect_ReadStop();
                        ADD_LOG("[error] lookdown error");
                        nError = ERR_LOOKDOWN_DETECT;
						ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
									ERR_LOOKDOWN_DETECT,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nTransType,
									m_nTransStep,
									m_pHWSet->m_pLookdown->IsDetect(),
									m_pHWSet->m_pHoistAxis->GetCurrentPosition());
						ADD_MCC_LOG(",%s,TRANS,LOOKDOWN DETECT,End,FOUP,%06d,%d,%06d,%d,LookDown Hoist Pos,%f,LookDown Shift Pos,%f,LookDown Stop Time,%d",
									m_defualtparam->VHLName,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
									m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
									OHTUTILITY::PreventOverflowGetTimeMs(m_dwLookdn_STTime));
				}
					else if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLookdn_STTime) > ((DWORD)m_pParamSet->TransDefine.nLookdn_Detect_Timeover-LOOKDOWNLOGTIME))
                    {
                         if(bCheckLookdownArea == false)
                         {
							ADD_LOG("[before 2sec] read start");
                            bCheckLookdownArea = true;
                            m_pHWSet->m_pLookdown->PBSDetect_ReadStart();
                         }
                         else
						 {
                            m_pHWSet->m_pLookdown->CheckSensorArea();
                         }

                    }
                    else
                    {
                        bCheckLookdownArea = false;
                    }
                                   
				}
            }
            else // nLookDownSensor==OFF
            {
				if(bCheckLookdownArea)
                {
					bCheckLookdownArea = false;
					ADD_LOG("Lookdn detect On->Off");
                    m_pHWSet->m_pLookdown->PBSDetect_ReadStop();
                }
				if(bLookdown_Detected == true)   //  bLookdown_Detected  EQPIO 사용하는곳에 룩다운감지시 스탭넘어가는거 막아둘 것.
				{
                    if(m_pHWSet->m_pHoistAxis->IsRunning() == ON)
                        return nError;
					bLookdown_Detected = false;
					ADD_MCC_LOG(",%s,TRANS,LOOKDOWN DETECT,End,FOUP,%06d,%d,%06d,%d,LookDown Hoist Pos,%f,LookDown Shift Pos,%f,LookDown Stop Time,%d",
								m_defualtparam->VHLName,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
								m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
								OHTUTILITY::PreventOverflowGetTimeMs(m_dwLookdn_STTime));

					ADD_LOG("Lookdn OFF1[Step=%d]: cmdPos=%.02f curPos = %.02f", m_nTransStep, m_dCmdHoistPos,
                        m_pHWSet->m_pHoistAxis->GetCurrentPosition());
                }
            }
		}
        else
		{
			if(bLookdown_Detected == true)
            {
                if(m_pHWSet->m_pHoistAxis->IsRunning() == ON)
                    return nError;
				bLookdown_Detected = false;


				ADD_LOG("Lookdn OFF2[Step=%d]: cmdPos=%.02f curPos = %.02f", m_nTransStep, m_dCmdHoistPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition());
            }
        }
    }

	// Oscillation Sensor Check..
	if((nError == NO_ERR) && (m_TransUse.OscillationSen == true))
    {
        if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
		{
			if(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > OSCILLATION_IGNORE_DIST)
			{
				// by zzang9un 2013.08.06 : Oscillation이 true가 되면 hand가 흔들린 경우임
				if(m_pHWSet->m_pOcsillation->IsDetect() == true)
				{
					nOsciCount++;
					m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_OC);
					m_mccparam.nOscillation_Count++;
					if(m_MoniStationID != m_StationID)
					{
						ADD_MD_LOG("OC Cur Station 1 : %d",m_StationID );
					  //	m_MoniStationID=0;
					    m_MoniStationID = m_StationID ;
					}
					if(nOsciCount > 5)
                    {
						nError = ERR_OSCILLATION_ERROR;
						ADD_LOG("Check_Sensor[Step=%d]: ERR_OSCILLATION_ERROR", m_nTransStep);
                        nOsciCount = 0;
						ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%f/%f",
									ERR_OSCILLATION_ERROR,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_pHWSet->m_pOcsillation->IsDetect(),
									fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()),
									OSCILLATION_IGNORE_DIST);
					}
					if(m_bFDCOcsillationFlag == true)
					{
						SEND_WARN("WARN/13/1/5/%06d/%06d/%d/%d/%06d",
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
						m_bFDCOcsillationFlag =false;
                    }
                }
            }
            else
                nOsciCount = 0;
        }
        else
        {
			dOscillationCheckDist = (double)m_pParamSet->TransDefine.dOscillation_Check_Dist;

			if(m_pParamSet->TransUsePart.OscillationSenPause == true)   //오실레이션 Pause기능 사용유무 확인
			{

				if((fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) < (fabs(m_dHoist_TeachingPos) - dOscillationCheckDist)) &&
					(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > OSCILLATION_IGNORE_DIST))
				{
					if(m_pHWSet->m_pOcsillation->IsDetect() == true)
					{
						m_dwOSL_CheckTime = timeGetTime();

						if(m_bOSL_Detected_Check ==true)
						{

							nOsciCount++;
							m_mccparam.nOscillation_Count++;
							if(nOsciCount > 5)
							{
								nError = ERR_OSCILLATION_ERROR;
								ADD_LOG("Check_Sensor[Step=%d]: ERR_OSCILLATION_ERROR", m_nTransStep);
								nOsciCount = 0;
								ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%d/%f/%f",
											ERR_OSCILLATION_ERROR,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pExecuteInfo->ExecutePositionInfo.NextNode,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_pOHTMainStatus->StatusCommon.ExistFoup,
											m_pOHTMainStatus->StatusCommon.StopStationID,
											m_pHWSet->m_pOcsillation->IsDetect(),
											fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()),
											OSCILLATION_IGNORE_DIST);
							}
							if(m_bFDCOcsillationFlag == true)
							{
								SEND_WARN("WARN/13/2/5/%06d/%06d/%d/%d/%06d",
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pExecuteInfo->ExecutePositionInfo.NextNode,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_pOHTMainStatus->StatusCommon.ExistFoup,
											m_pOHTMainStatus->StatusCommon.StopStationID);
								m_bFDCOcsillationFlag =false;
							}
						}
						else if(m_bOSL_Detected == false)
						{

                            nOsciPauseCount++;
							m_mccparam.nOscillation_Count++;
                            ADD_LOG("nOsciPauseCount : %d",nOsciPauseCount);
							if(nOsciPauseCount > 5)
                            {
								nOsciPauseCount = 0;
                                m_dwOSL_STTime = timeGetTime();
								m_bOSL_Detected = true;
								m_IsCheckPause = true;

							    nError = m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
								m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_OC);
								if(m_MoniStationID != m_StationID)
								{
									ADD_MD_LOG("OC Cur Station 3 : %d",m_StationID );
									m_MoniStationID = m_StationID ;
								}
								m_pHWSet->Sound_On(SETSOUND_OSCILLATION);
                                 ADD_LOG("OSL ON[Step=%d]: cmdPos=%.02f curPos = %.02f", m_nTransStep, m_dCmdHoistPos,
								m_pHWSet->m_pHoistAxis->GetCurrentPosition());
								ADD_MCC_LOG(",%s,TRANS,OSCILLATION PAUSE,Start,FOUP,%06d,%d,%06d,%d,Oscillation Pause Hoist Pos,%6.1lf",
											m_defualtparam->VHLName,
											m_pOHTMainStatus->StatusCommon.CurStationID,
											m_mccparam.nEqType,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_pHWSet->m_pHoistAxis->GetCurrentPosition());
                            }

						}
						else
						{
							if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwOSL_STTime) > 40000) //40초동안 유지될경우 에러 발생
							{
								nError = ERR_OSCILLATION_ERROR;
								ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%f/%f",
											ERR_OSCILLATION_ERROR,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pExecuteInfo->ExecutePositionInfo.NextNode,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_pOHTMainStatus->StatusCommon.ExistFoup,
											m_pOHTMainStatus->StatusCommon.StopStationID,
											m_pHWSet->m_pOcsillation->IsDetect(),
											fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()),
											OSCILLATION_IGNORE_DIST);
								ADD_MCC_LOG(",%s,TRANS,OSCILLATION PAUSE,End,FOUP,%06d,%d,%06d,%d,Oscillation Pause Time,%d",
											m_defualtparam->VHLName,
											m_pOHTMainStatus->StatusCommon.CurStationID,
											m_mccparam.nEqType,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											OHTUTILITY::PreventOverflowGetTimeMs(m_dwOSL_STTime));
						}
						}
					}
					else // nLookDownSensor==OFF
					{
						nOsciCount = 0;
                        nOsciPauseCount = 0;
						if(m_bOSL_Detected == true)   //  bLookdown_Detected  EQPIO 사용하는곳에 룩다운감지시 스탭넘어가는거 막아둘 것.
						{
							if(m_pHWSet->m_pHoistAxis->IsRunning() == ON)
								return nError;


							if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwOSL_CheckTime) < 10000) //40초동안 유지될경우 에러 발생
								return nError;
							else
							{
								m_bOSL_Detected = false;
								m_bOSL_Detected_Check = true;
							}
						
							ADD_LOG("OSL OFF1[Step=%d]: cmdPos=%.02f curPos = %.02f", m_nTransStep, m_dCmdHoistPos,
								m_pHWSet->m_pHoistAxis->GetCurrentPosition());
							ADD_MCC_LOG(",%s,TRANS,OSCILLATION PAUSE,End,FOUP,%06d,%d,%06d,%d,Oscillation Pause Time,%d",
											m_defualtparam->VHLName,
											m_pOHTMainStatus->StatusCommon.CurStationID,
											m_mccparam.nEqType,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											OHTUTILITY::PreventOverflowGetTimeMs(m_dwOSL_STTime));
						}
					}
				}

			}
			else //설정이 안된경우
			{
				if((fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) < (fabs(m_dHoist_TeachingPos) - dOscillationCheckDist)) &&
					(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > OSCILLATION_IGNORE_DIST))
				{
					// by zzang9un 2013.08.06 : Oscillation이 true가 되면 hand가 흔들린 경우임
					if(m_pHWSet->m_pOcsillation->IsDetect() == true)
					{
						
						if(m_MoniStationID != m_StationID)
						{
							ADD_MD_LOG("OC Cur Station 3 : %d",m_StationID );
							m_MoniStationID = m_StationID ;

						}
						nOsciCount++;
						m_mccparam.nOscillation_Count++;
						if(nOsciCount > 5)
						{
							nError = ERR_OSCILLATION_ERROR;
							ADD_LOG("Check_Sensor[Step=%d]: ERR_OSCILLATION_ERROR", m_nTransStep);
							nOsciCount = 0;
							ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%f/%f",
										ERR_OSCILLATION_ERROR,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_pHWSet->m_pOcsillation->IsDetect(),
										fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()),
										OSCILLATION_IGNORE_DIST);
						}
						if(m_bFDCOcsillationFlag == true)
						{
							SEND_WARN("WARN/13/3/5/%06d/%06d/%d/%d/%06d",
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID);
							m_bFDCOcsillationFlag =false;
						}
					}
				}
				else
					nOsciCount = 0;
			}

			/*
			if((fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) < (fabs(m_dHoist_TeachingPos) - dOscillationCheckDist)) &&
				(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > OSCILLATION_IGNORE_DIST))
			{
				// by zzang9un 2013.08.06 : Oscillation이 true가 되면 hand가 흔들린 경우임
				if(m_pHWSet->m_pOcsillation->IsDetect() == true)
				{
					nOsciCount++;
					if(nOsciCount > 5)
					{
						nError = ERR_OSCILLATION_ERROR;
						ADD_LOG("Check_Sensor[Step=%d]: ERR_OSCILLATION_ERROR", m_nTransStep);
						nOsciCount = 0;
					}
				}
			}
			else
				nOsciCount = 0;
				*/
		}
    }
    else
		nOsciCount = 0;





	if((m_pHWSet->m_pHand->IsGoSigOn() ==false) &&
	  (fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > OSCILLATION_IGNORE_DIST))
	{
		if(nHandPIOGosignalPauseCount > 1)
		{
			ADD_LOG("Check_Sensor[Step=%d]: ERR_HANDPIO_GO_OFF", m_nTransStep);
			nError = ERR_HANDPIO_GO_OFF;
		}
		else if(m_bHandPIOGoSignal_Detected == false)
		{
			nHandPIOPauseCount++;

			if(nHandPIOPauseCount > 2)
			{
				nHandPIOPauseCount = 0;
				nHandPIOGosignalPauseCount++;
				m_IsCheckPause =true; //Pause 동작시 발동
				m_bHandPIOGoSignal_Detected = true;
				m_dwHandPIOGoSignal_CheckTime = timeGetTime();
				nError = m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_HANDPIOP);
				 ADD_MD_LOG("HandPIOPause St : %d",m_StationID);
				if(m_TransUse.ObsDetectSound == true)
					m_pHWSet->Sound_On(SETSOUND_OSCILLATION);

				ADD_LOG("HandPIOGoSignal Off[Step=%d]: cmdPos=%.02f curPos = %.02f", m_nTransStep, m_dCmdHoistPos,
					m_pHWSet->m_pHoistAxis->GetCurrentPosition());
			}
		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwHandPIOGoSignal_CheckTime) > 700) //700msec동안 유지될경우 에러 발생
				nError = ERR_HANDPIO_GO_OFF;
		}

	}
	else
	{
		nHandPIOPauseCount = 0;

		if(m_bHandPIOGoSignal_Detected==true)
		{
		   if(m_pHWSet->m_pHoistAxis->IsRunning() == ON)
				return nError;

		   m_bHandPIOGoSignal_Detected = false;

			ADD_LOG("HandPIOGoSignal On[Step=%d]: cmdPos=%.02f curPos = %.02f", m_nTransStep, m_dCmdHoistPos,
				m_pHWSet->m_pHoistAxis->GetCurrentPosition());
        }

		if((nError == NO_ERR) && (m_TransUse.GripperSen == true))
		{

			if(m_pHWSet->m_pHand->IsOpen() == true)
			{
				if((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
				{
					m_nErrCount_GripAbNormalON++;
					if(m_nErrCount_GripAbNormalON > 20) // 200msec
					{
						m_nErrCount_GripAbNormalON = 0;
						nError = ERR_GRIPPER_SEN_ON_IN_HAND_OPEN;
						ADD_LOG("Check_Sensor[Step=%d]: ERR_GRIPPER_SEN_ON_IN_HAND_OPEN", m_nTransStep);
					ADD_FDC_LOG("ERR/%d/1/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_GRIPPER_SEN_ON_IN_HAND_OPEN,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
					}
				}
				else
					m_nErrCount_GripAbNormalON = 0;
			}

			else if(m_pHWSet->m_pHand->IsClose() == true)
			{
				if((m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE) && (m_nTransType != TRANSTYPE_SMALLADD_HOIST_HOME)
				&& ((m_pHWSet->m_pHand->IsRightGripOn() == false) || (m_pHWSet->m_pHand->IsLeftGripOn() == false)))
				{
					m_nErrCount_GripAbNormalOFF++;
					if(m_nErrCount_GripAbNormalOFF > 20) // 200msec
					{
						m_nErrCount_GripAbNormalOFF = 0;
						nError = ERR_GRIPPER_SEN_OFF_IN_HAND_CLOSE;
						ADD_LOG("Check_Sensor[Step=%d]: ERR_GRIPPER_SEN_OFF_IN_HAND_CLOSE", m_nTransStep);
					ADD_FDC_LOG("ERR/%d/1/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_GRIPPER_SEN_OFF_IN_HAND_CLOSE,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
					}
				}
				else
					m_nErrCount_GripAbNormalOFF = 0;
			}
		}
		else
		{
			m_nErrCount_GripAbNormalON = 0;
			m_nErrCount_GripAbNormalOFF = 0;
		}

		// by zzang9un 2013.09.24 : Load시 에러 추가
		if((nError == NO_ERR) && (m_nTransType == TRANSTYPE_LOAD) && (nNextStep <= STEP_L_CHECK_MOVE_DOWN_HOIST_SLOW))
		{

			if(m_pHWSet->m_pHand->IsClose() == false)
			{
				nHandCloseSenOff_Before_Load++;
				if(nHandCloseSenOff_Before_Load > 7) // 48msec
				{
					nHandCloseSenOff_Before_Load = 0;
					nError = ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD;
					ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD", m_nTransStep);
				ADD_FDC_LOG("ERR/%d/3/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsOpen(),
							m_pHWSet->m_pHand->IsClose());
				}

			}else
				nHandCloseSenOff_Before_Load = 0;

			if(m_pHWSet->m_pHand->IsOpen() == true)
			{
				m_nErrCount_HandOpenAbNormalON++;
				if(m_nErrCount_HandOpenAbNormalON > 7) // 48msec
				{
					m_nErrCount_HandOpenAbNormalON = 0;
					nError = ERR_HAND_OPEN_SEN_ON_BEFORE_LOAD;
					ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_OPEN_SEN_ON_BEFORE_LOAD", m_nTransStep);
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_OPEN_SEN_ON_BEFORE_LOAD,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsOpen(),
							m_pHWSet->m_pHand->IsClose());
				}
			}else
					m_nErrCount_HandOpenAbNormalON = 0;
		}else
		{
			m_nErrCount_HandOpenAbNormalON = 0;
			nHandCloseSenOff_Before_Load = 0;
		}


	

		if((nError == NO_ERR) && (m_nTransType == TRANSTYPE_UNLOAD) && (nNextStep <= STEP_U_CHECK_MOVE_DOWN_HOIST_SLOW))
		{

			if(m_pHWSet->m_pHand->IsOpen() == false)
			{
				nHandOpenSenOff_Before_UnLoad++;
				if(nHandOpenSenOff_Before_UnLoad>7)
				{
					nHandOpenSenOff_Before_UnLoad= 0;
					nError = ERR_HAND_OPEN_SEN_OFF_BEFORE_UNLOAD;
					ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_OPEN_SEN_OFF_BEFORE_UNLOAD", m_nTransStep);
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_OPEN_SEN_OFF_BEFORE_UNLOAD,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsOpen(),
							m_pHWSet->m_pHand->IsClose());
				}
			}else
				nHandOpenSenOff_Before_UnLoad = 0;

			if(m_pHWSet->m_pHand->IsClose() == true)
			{
				nHandCloseOn_Before_UnLoad++;
				if(nHandCloseOn_Before_UnLoad >7)
				{
					nHandCloseOn_Before_UnLoad = 0;
					nError = ERR_HAND_CLOSE_SEN_ON_BEFORE_UNLOAD;
					ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_CLOSE_SEN_ON_BEFORE_UNLOAD", m_nTransStep);
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_CLOSE_SEN_ON_BEFORE_UNLOAD,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsOpen(),
							m_pHWSet->m_pHand->IsClose());
				}

			}else
				nHandCloseOn_Before_UnLoad = 0;
		}else
		{
			nHandOpenSenOff_Before_UnLoad = 0;
			nHandCloseOn_Before_UnLoad = 0;
		}

		if((nError == NO_ERR) && (m_nTransType == TRANSTYPE_UNLOAD) && (nNextStep >= STEP_U_MOVE_UP_HOIST_SLOW))
		{

			if(m_pHWSet->m_pHand->IsOpen() == true)
			{
				nHandOpenSenOn_In_UnLoad++;
				if(nHandOpenSenOn_In_UnLoad > 7)
				{
					nHandOpenSenOn_In_UnLoad = 0;
					nError = ERR_HAND_OPEN_SEN_ON_IN_UNLOAD;
					ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_OPEN_SEN_ON_IN_UNLOAD", m_nTransStep);
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_OPEN_SEN_ON_IN_UNLOAD,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsOpen(),
							m_pHWSet->m_pHand->IsClose());
				}
			}
			else
			   nHandOpenSenOn_In_UnLoad = 0;

			if(m_pHWSet->m_pHand->IsClose() == false)
			{
				nHandCloseSenOff_In_UnLoad++;
				if(nHandCloseSenOff_In_UnLoad >7)
				{
					nHandCloseSenOff_In_UnLoad = 0;
					nError = ERR_HAND_CLOSE_SEN_OFF_IN_UNLOAD;
					ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_CLOSE_SEN_OFF_IN_UNLOAD", m_nTransStep);
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_CLOSE_SEN_OFF_IN_UNLOAD,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsOpen(),
							m_pHWSet->m_pHand->IsClose());
				}

			}
			else
				nHandCloseSenOff_In_UnLoad = 0;
		}else
		{
			nHandOpenSenOn_In_UnLoad = 0;
			nHandCloseSenOff_In_UnLoad = 0;
		}

		if((nError == NO_ERR) && (m_TransUse.HandPushSen == true))
		{
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true))
			{
				if((m_nTransType == TRANSTYPE_LOAD) &&
				((m_dCmdFinal_HoistPos - m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load) &&
				((m_nTransStep <= STEP_L_CHECK_MOVE_DOWN_HOIST_FAST) || (m_nTransStep > STEP_L_CHECK_MOVE_UP_HOIST_FAST)))
				{

					if(m_pHWSet->m_pHand->IsPushOn() == true)
					{
						nLoadPushCount++;
						if(nLoadPushCount > 5)
						{
							nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
							ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_PUSHSEN_ABNORMAL_ON", m_nTransStep);
							nLoadPushCount = 0;
						ADD_FDC_LOG("ERR/%d/3/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
										ERR_HAND_PUSHSEN_ABNORMAL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsPushOn(),
										m_pHWSet->m_pHoistAxis->GetCurrentPosition());
						}
					}
					else
						nLoadPushCount = 0;
				}

				if((m_nTransType == TRANSTYPE_UNLOAD) &&
				((m_dCmdFinal_HoistPos - m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset) &&
				((m_nTransStep <= STEP_U_CHECK_MOVE_DOWN_HOIST_FAST) || (m_nTransStep >	STEP_U_MOVE_UP_HOIST_HOME)))
				{
	
					if(m_pHWSet->m_pHand->IsPushOn() == true)
					{
						nUnloadPushCount++;
						if(nUnloadPushCount > 5)
						{
							nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
							ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_PUSHSEN_ABNORMAL_ON", m_nTransStep);
							nUnloadPushCount = 0;
							ADD_FDC_LOG("ERR/%d/4/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
										ERR_HAND_PUSHSEN_ABNORMAL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsPushOn(),
										m_pHWSet->m_pHoistAxis->GetCurrentPosition());
						}
					}

					else
						nUnloadPushCount = 0;
				}
			}
			else
			{
				if((m_nTransType == TRANSTYPE_LOAD) &&
				((m_dCmdFinal_HoistPos - m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load) &&
				((m_nTransStep <= STEP_L_CHECK_MOVE_DOWN_HOIST_FAST) || (m_nTransStep >= STEP_L_MOVE_UP_HOIST_FAST)))
				{

					if(m_pHWSet->m_pHand->IsPushOn() == true)
					{
						nLoadPushCount++;
						if(nLoadPushCount > 5)
						{
							nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
							ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_PUSHSEN_ABNORMAL_ON", m_nTransStep);
							nLoadPushCount = 0;
						ADD_FDC_LOG("ERR/%d/5/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
										ERR_HAND_PUSHSEN_ABNORMAL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsPushOn(),
										m_pHWSet->m_pHoistAxis->GetCurrentPosition());
						}
					}

					else
						nLoadPushCount = 0;
				}
				if((m_nTransType == TRANSTYPE_UNLOAD) &&
				   ((m_dCmdFinal_HoistPos - m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset) &&
				   ((m_nTransStep <= STEP_U_CHECK_MOVE_DOWN_HOIST_FAST) || (m_nTransStep >= STEP_U_MOVE_UP_HOIST_FAST)))
				{
	
					if(m_pHWSet->m_pHand->IsPushOn() == true)
					{
						nUnloadPushCount++;
						if(nUnloadPushCount > 5)
						{
							nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
							ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_PUSHSEN_ABNORMAL_ON", m_nTransStep);
							nUnloadPushCount = 0;
                                                        ADD_FDC_LOG("ERR/%d/6/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
										ERR_HAND_PUSHSEN_ABNORMAL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsPushOn(),
										m_pHWSet->m_pHoistAxis->GetCurrentPosition());
						}
					}

					else
						nUnloadPushCount = 0;
				}
			}

		}
	}

	//Pause 동작을 타면 모두 만족할 경우 해제 처리.(lookdown OSL Detect, Hand PIO모두 만족했을 경운
	if(m_IsCheckPause == true)
	{
		if((bLookdown_Detected ==false) &&  (m_bOSL_Detected ==false) && (m_bHandPIOGoSignal_Detected ==false))
		{
				m_IsCheckPause = false;
		   // by zzang9un 2013.09.24 : Lookdown 해제 시 이적재 사운드로 변경
				if(m_nTransSound != 0)
					m_pHWSet->Sound_On(SETSOUND_TRANS);
				else
					m_pHWSet->Sound_Off();

				switch(nHoistState)
				{
				case HOIST_DOWN_STATE:
					if(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > fabs(m_dCmdHoistPos))
						m_nTransStep = nNextStep;
					else
						m_nTransStep = nPreStep;
					break;
				case HOIST_UP_STATE:
					if(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) < fabs(m_dCmdHoistPos))
						m_nTransStep = nNextStep;
					else
						m_nTransStep = nPreStep;
					break;
				}
		}
	}

	// Shutter Open 상태여야 할 때 Shutter 리미트 센서 감시
    if((nError == NO_ERR) && (m_TransUse.ShutterControl == true))
	{
        if(((m_nTransType == TRANSTYPE_MOVE_ALL_HOME) || (m_nTransType == TRANSTYPE_AUTO_CHANGE))
        && (m_pHWSet->m_pHoistAxis->IsHoistHome() == true)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
        && (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < INPOS_LIMIT)
        && (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) < ROTATE_INPOS_LIMIT))
#else
		&& (m_pHWSet->m_pShiftAxis->IsShiftHome() == true))
#endif
        {
			return NO_ERR;
        }

		if(m_pHWSet->m_pShutter->CheckSensor(SHUTTER_OPEN_POS) !=NO_ERR)
		{
			nshutterCount++;

			if(nshutterCount>5)
			{
				nError = m_pHWSet->m_pShutter->CheckSensor(SHUTTER_OPEN_POS);
				nshutterCount =0;
			}
		}
		else
			nshutterCount =0;
	}

	return nError;
}
// ---------------------------------------------------------------------------

/**
 @brief   Hoist 축에 명령을 줄 때 해당 위치와 속도를 저장하여 차후 명령 완료 체크 시 사용함.
 @author  LSJ
 @date    2012.11.10
 @param   dTmpStPos(명령 수행 시 Hoist 시작 위치)
 @param   dTmpEdPos(명령 수행 시 Hoist 명령 위치)
 @param   dTmpEdVel_mmpermsec(명령 수행 시 Hoist 명령 속도)
 @return  센서들에 문제가 없으면 0, 문제가 발생되면 해당 에러 코드 리턴
 */
void TransControl::SaveHoistMoveCmd(double dTmpStPos, double dTmpEdPos, double dTmpEdVel)
{
	m_dCmdHoistStartPos = dTmpStPos;
    m_dCmdHoistPos = dTmpEdPos;

	m_dCmdHoistSpeed = dTmpEdVel;

}
// ---------------------------------------------------------------------------
/**
 @brief   Hoist축을 해당되는 위치로 이동시키는 함수
 @author  LSJ
 @date    2012.11.10
 @param   nMoveCase(어디에서 시작해서 어디로 가는 이동인지 선택)
 @return  Hoist 축 이동에 문제가 없으면 0, 문제가 있으면 해당 에러코드 리턴.
 */
int TransControl::Move_Hoist(int nMoveCase)
{
	int nError = 0;

	double dVel_mmpermsec = 0.0;
	double dAccel_mmpermsec2 = 0.0;
	double dDecel_mmpermsec2 = 0.0;
	double dTmpHoist_TeachingPos = 0.0;

	//사전동작관련 내용  Interlock 추가 20170123
	if(m_defualtparam->PreShutter)
	{
         if((m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE) &&
		 ((m_pHWSet->m_pShutter->IsOpen(FRONT) == false) || (m_pHWSet->m_pShutter->IsOpen(REAR) == false)))
		 {

			nError = m_pHWSet->m_pShutter->Open();

		 }
	}
    if(nError != NO_ERR) return nError;

	switch(nMoveCase)
	{
	case HOIST_DOWN_H2TO: // 원점에서 Teaching Point-x까지 고속 이동..

		switch(m_nTransType)
		{
		case TRANSTYPE_LOAD:
			if((m_dHoist_TeachingPos - m_pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset_Load) <= 0.0)
			{
				dTmpHoist_TeachingPos = m_dHoist_TeachingPos;
			}
			else
			{
				dTmpHoist_TeachingPos = m_dHoist_TeachingPos - m_pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset_Load;
			}

			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true))
			{
				//STB는 별도 처리, STB는 원복하지 않음(변경 후 파라미터 30mm로 거리값 짧게 변경한 기준으로 확인  20171030

				if((m_dHoist_TeachingPos - 40.0) <= 0.0)
				{
					dTmpHoist_TeachingPos = m_dHoist_TeachingPos;
				}
				else
				{
					dTmpHoist_TeachingPos = m_dHoist_TeachingPos - 40.0;
				}


				// by zzang9un 2013.01.24 : 기존 보다 속도 1/2 줄임
				dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_20P; // MULTIPLY_2_50P

				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
			}
			else if(m_nEqType == PORT_TYPE_UTB)
			{

				dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_60P;
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;

			}
			else
			{
				// EQ, LOADER인 경우
				dVel_mmpermsec = HOIST_MAX_SPEED * m_dHoistLevel;

				if(fabs(m_dHoist_TeachingPos) > fabs((double)m_pParamSet->TransDefine.dFast_Move_Limit))
				{

					dVel_mmpermsec = dVel_mmpermsec * MULTIPLY_2_90P;
					dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
					dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
				}
				else
				{

					dVel_mmpermsec = dVel_mmpermsec * MULTIPLY_2_60P;
					dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
					dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				}
			}
			SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), dTmpHoist_TeachingPos, dVel_mmpermsec);
			nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
			break;

		case TRANSTYPE_UNLOAD:
		case TRANSTYPE_AUTOTEACHING:     //soming
			if((m_dHoist_TeachingPos - m_pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset) <= 0.0)
			{
				dTmpHoist_TeachingPos = m_dHoist_TeachingPos;
			}
			else
			{
				dTmpHoist_TeachingPos = m_dHoist_TeachingPos - m_pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset;
			}

			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true))
			{
				if((m_dHoist_TeachingPos - (m_pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset*0.5)) <= 0.0)
				{
					dTmpHoist_TeachingPos = m_dHoist_TeachingPos;
				}
				else
				{
					dTmpHoist_TeachingPos = m_dHoist_TeachingPos - (m_pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset*0.5);
				}

				dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_20P; // MULTIPLY_2_80P
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
		   }
			else
			{
				dVel_mmpermsec = HOIST_MAX_SPEED * m_dHoistLevel;

				if(fabs(m_dHoist_TeachingPos) > fabs((double)IS_FAST_MOVE_LIMIT))
				{

					dVel_mmpermsec = dVel_mmpermsec * MULTIPLY_2_100P;
					dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
					dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				}
				else
				{

					dVel_mmpermsec = dVel_mmpermsec * MULTIPLY_2_70P;
					dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
					dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				}
			}
			SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), dTmpHoist_TeachingPos, dVel_mmpermsec);
			nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
			break;
		}
		break;

	case HOIST_DOWN_TO2TP: // Teaching Point-x에서 Teaching Point까지 저속 이동
		switch(m_nTransType)
		{
		case TRANSTYPE_LOAD:
				if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true))
				{

					dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_1P;
					dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
					dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_50;
				}
				else
				{

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))

				    dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_5P;
					dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
					dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
					dVel_mmpermsec = dVel_mmpermsec * (0.01 * m_pParamSet->TransDefine.nDnState_DN_SpeedDown_Speed_Load);
#else
					dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_5P;
					dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
					dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
					dVel_mmpermsec = dVel_mmpermsec * (0.01 * m_pParamSet->TransDefine.nDnState_DN_SpeedDown_Speed_Load);
#endif


				   }
			break;
			case TRANSTYPE_UNLOAD:
			case TRANSTYPE_AUTOTEACHING:  //soming
				if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true))
				{

					dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_1P;
					dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
					dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_50;
				}
				else
				{

					dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_5P;
					dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
					dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				   }
			break;

		}

		SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_dHoist_TeachingPos, dVel_mmpermsec);

		m_dCmdFinal_HoistPos = m_dCmdHoistPos;

		nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
		break;

	case HOIST_DOWN_H2TP:

		dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_3P;
		dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
		dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;

		SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_dHoist_TeachingPos, dVel_mmpermsec);

		m_dCmdFinal_HoistPos = m_dCmdHoistPos;

		nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
		break;

	case HOIST_UP_HO2H: // 2축 이동 : 원점-x지점 ~ 원점
		switch(m_nTransType)
		{
			case TRANSTYPE_LOAD:
				dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_6P;
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
			break;
			case TRANSTYPE_UNLOAD:
				dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_6P;
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
			break;
		}

		SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), 0.0, dVel_mmpermsec);

		nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
		break;

	case HOIST_UP_TP2TO: // Teaching Point에서 Teaching Point-x 까지 저속 이동

				dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_3P;
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;

		if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true))
		{
			switch(m_nTransType)
			{
				case TRANSTYPE_LOAD:
					SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_dCmdFinal_HoistPos - (m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load*0.5), dVel_mmpermsec);
				break;
				case TRANSTYPE_UNLOAD:
				case TRANSTYPE_AUTOTEACHING:     //soming 추가
					// STB 저속구간 수정 (jhoun.lim)
					dVel_mmpermsec = HOIST_MAX_SPEED * 0.007; // 0.7%
					dAccel_mmpermsec2 = dVel_mmpermsec / 70;
					dDecel_mmpermsec2 = dVel_mmpermsec / 70;
					SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_dCmdFinal_HoistPos - ( 7 + HOIST_MOVINPOS_LIMIT), dVel_mmpermsec); // 저속구간 7mm
					//SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_dCmdFinal_HoistPos - (m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset*0.5), dVel_mmpermsec);
					break;
			}

		}
		else
		{
			switch(m_nTransType)
			{
				case TRANSTYPE_LOAD:

				dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_3P;
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
					SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), (m_dCmdFinal_HoistPos - m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load), dVel_mmpermsec);
				break;
				case TRANSTYPE_UNLOAD:

				dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_3P;

				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
					SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), (m_dCmdFinal_HoistPos - m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset), dVel_mmpermsec);
#else
				if(fabs(m_dHoist_TeachingPos) < 1700)
				{

                     dVel_mmpermsec = dVel_mmpermsec * 0.3;
                     SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), (m_dCmdFinal_HoistPos - 30), dVel_mmpermsec);


				}
				else
				{
					if(bCheck_SpecialEQ ==true)
					{
						dVel_mmpermsec = dVel_mmpermsec *(double)(m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Percent_SpecialEQ/100.0);
						SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), (m_dCmdFinal_HoistPos - m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_SpecialEQ), dVel_mmpermsec);
					}
					else
					{
                      	SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), (m_dCmdFinal_HoistPos - m_pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset), dVel_mmpermsec);
					}

				}
#endif
				break;
			}
		}

		nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);

		m_dwTotalTrans_STTime = timeGetTime();
		break;

	case HOIST_UP_TO2HO: // Teaching Point-x에서 원점-x까지 고속 이동
		switch(m_nTransType)
		{
		case TRANSTYPE_LOAD:
			dVel_mmpermsec = HOIST_MAX_SPEED * m_dHoistLevel;

			if(fabs(m_dHoist_TeachingPos) > fabs((double)IS_FAST_MOVE_LIMIT))
			{
				dVel_mmpermsec = dVel_mmpermsec * MULTIPLY_2_100P;
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
			}
			else
			{
				dVel_mmpermsec = dVel_mmpermsec * MULTIPLY_2_70P;
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
			}

			SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_pParamSet->TransDefine.nUpstate_SpeedDown_Offset_Load,
				dVel_mmpermsec);

			nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
			break;

		case TRANSTYPE_UNLOAD:
			dVel_mmpermsec = HOIST_MAX_SPEED * m_dHoistLevel;

			if(fabs(m_dHoist_TeachingPos) > fabs((double)IS_FAST_MOVE_LIMIT))
			{
				dVel_mmpermsec = dVel_mmpermsec * MULTIPLY_2_70P;
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_600;
			}
			else
			{
				dVel_mmpermsec = dVel_mmpermsec * MULTIPLY_2_50P;
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
			}

			SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_pParamSet->TransDefine.nUpstate_SpeedDown_Offset,
				dVel_mmpermsec);

			nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
			break;
		}
		break;

	case HOIST_UP_TO2H: // Teaching Point-x에서 원점까지 고속 이동
		switch(m_nTransType)
		{
		case TRANSTYPE_LOAD:
		case TRANSTYPE_UNLOAD:
		case TRANSTYPE_AUTOTEACHING:   //soming 추가
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
			{
				if(m_nTransType == TRANSTYPE_UNLOAD)
				{
					dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_20P;
				}
				else
				{
					dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_40P;
				}
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500; // HOIST_ACCTIME_50
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500; // HOIST_ACCTIME_50

				// stb 고속구간 수정 (jhoun.lim)
				if(m_nTransType == TRANSTYPE_UNLOAD)
				{
					dVel_mmpermsec = HOIST_MAX_SPEED * 0.25; // 25%
					dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_250;
					dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_250;
				}
			}
			else
			{
				dVel_mmpermsec = (HOIST_MAX_SPEED * MULTIPLY_2_20P * m_dHoistLevel);
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_300;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
			}
			SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), 0.0, dVel_mmpermsec);
			nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
			break;
		}
		break;
    // 2017-01-02, Add by Labeler
    case HOIST_UP_2H:
    	dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_6P;
		dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
		dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
#if(OHT_NAME == OHT_NAME_STD_V85)
		SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), 100.0, dVel_mmpermsec);
		nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
#else

		SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), 0.0, dVel_mmpermsec);
		nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
#endif
        break;

	case HOIST_DOWN_1:    // hoist를 위 또는 아래로 일정량 움직임..hoist home auto setting 시 사용
	case HOIST_UP_1:
	case HOIST_UP_x:
	case HOIST_UP_xp:
		double dMoveDist_1mm = 1.0;
		double dMoveDist_xmm = 5.0;
		double dMoveDist_xpmm = 30.0;

		dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_3P;
		dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
		dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;

		if(nMoveCase == HOIST_DOWN_1)
			m_dCmdHoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition() + dMoveDist_1mm;
		else if(nMoveCase == HOIST_UP_1)
			m_dCmdHoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition() - dMoveDist_1mm;
		else if(nMoveCase == HOIST_UP_x)
			m_dCmdHoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition() - dMoveDist_xmm;
		else if(nMoveCase == HOIST_UP_xp)
			m_dCmdHoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition() - dMoveDist_xpmm;

		nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);

		break;

	}

	m_mccparam.dHoist_Acc = dAccel_mmpermsec2;
	m_mccparam.dHoist_Dec = dDecel_mmpermsec2;
	m_mccparam.dHoist_Vel = dVel_mmpermsec;

	m_mccparam.dHoist_StartPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
	return nError;
}
/*********************
// stb
// Ʋ?ՠteaching position: 112mm
// Ö?렴eaching position: 143mm
// Ö?Ҡteaching position: 101mm 
*********************/

/*********************
// stk
// Ʋ?ՠteaching position: 640mm
// Ö?렴eaching position: 745mm
// Ö?Ҡteaching position: 500mm 
*********************/

/*********************
// short eq (1800mm Ǐ)
// Ʋ?ՠteaching position: 1687mm
// Ö?렴eaching position: 1738mm
// Ö?Ҡteaching position: 1654mm 
*********************/

/*********************
// middle eq (3200mm Ǐ)
// Ʋ?ՠteaching position: 3028mm
// Ö?렴eaching position: 3111mm
// Ö?Ҡteaching position: 2998mm
*********************/

/*********************
// long eq ()
// Ʋ?ՠteaching position: 3028mm
// Ö?렴eaching position: 3111mm
// Ö?Ҡteaching position: 2998mm 
*********************/
#ifdef USE_TRANS_PROFILE_VER2
int TransControl::Move_Hoist_Ver2(int nMoveCase)
{
	int nError = 0;
	double dVel_mmpermsec = 0.0;
	double dAccel_mmpermsec2 = 0.0;
	double dDecel_mmpermsec2 = 0.0;
	double dStartPosition_mm = 0.0;
	double dDestinationPosition_mm = 0.0;
	double dTmpHoist_MaxSpeed = 0.0;
	double dTmpMovePos;
	double dHoist_Up_TP2TO_Speed_Unload;
	double dHoist_Up_TP2TO_Speed_Load;
	TRANS_DEFINE	*TransDefine;

	TransDefine = &m_pParamSet->TransDefine;
	m_dHoistSlowDownPos = 0.0;
	dStartPosition_mm = m_pHWSet->m_pHoistAxis->GetCurrentPosition();

//	ADD_HOIST_POS_LOG("%f", dStartPosition_mm);
	if(m_defualtparam->PreShutter)
	{
		if((m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE) && (m_nTransType != TRANSTYPE_SMALLADD_HOIST_HOME) && 
		((m_pHWSet->m_pShutter->IsOpen(FRONT) == false) || (m_pHWSet->m_pShutter->IsOpen(REAR) == false)))
		{

			nError = m_pHWSet->m_pShutter->Open();
		}
	}

	if(nError != NO_ERR) return nError;

	if(m_nTransType == TRANSTYPE_LOAD)
	{
		switch(nMoveCase)
		{
		case HOIST_DOWN_H2SDP:
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true) || m_nEqType == PORT_TYPE_UTB)
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dDestinationPosition_mm = m_dHoist_TeachingPos - TransDefine->nHoistMoveUp_HomeOffset_STB_Load;
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.15;
				dAccel_mmpermsec2 = 0.0015;
				dDecel_mmpermsec2 = 0.0015;
#else
				dDestinationPosition_mm = m_dHoist_TeachingPos - TransDefine->nHoistMoveDown_TeachingOffset_STB_Load;
				dVel_mmpermsec = 0.367;
				dAccel_mmpermsec2 = 0.001223;
				dDecel_mmpermsec2 = 0.001223;
#endif
			}
			else
			{
				dDestinationPosition_mm = m_dHoist_TeachingPos - TransDefine->nHoistMoveDown_TeachingOffset_Load;
				if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){
#if(OHT_NAME == OHT_NAME_STD_V85)
					dVel_mmpermsec = HOIST_MAX_SPEED * 0.45;
					dAccel_mmpermsec2 = 0.0035;
					dDecel_mmpermsec2 = 0.0035;
#else
					dVel_mmpermsec = 1.2;
					dAccel_mmpermsec2 = 0.0033;
					dDecel_mmpermsec2 = 0.0027;
#endif
				}
				else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
				{
#if(OHT_NAME == OHT_NAME_STD_V85)
					dVel_mmpermsec = HOIST_MAX_SPEED * 0.6;
					dAccel_mmpermsec2 = 0.0033;
					dDecel_mmpermsec2 = 0.0033;
#else
					dVel_mmpermsec = 1.65;
					dAccel_mmpermsec2 = 0.0033;
					dDecel_mmpermsec2 = 0.0027;
#endif
				}
				else
				{
#if(OHT_NAME == OHT_NAME_STD_V85)
					dVel_mmpermsec = HOIST_MAX_SPEED * 0.8;
#else
					dVel_mmpermsec = 2.2;
#endif
					dAccel_mmpermsec2 = 0.006;
					if(fabs(m_dShift_TeachingPos) > HOIST_HIGH_SPEED_SHIFT_LIMIT) dDecel_mmpermsec2 = 0.003;
					else dDecel_mmpermsec2 = 0.006;
				}
			}

			if(dDestinationPosition_mm <= 0.0) dDestinationPosition_mm = m_dHoist_TeachingPos;

			dTmpHoist_MaxSpeed =  sqrt((2*dDestinationPosition_mm*dAccel_mmpermsec2*dDecel_mmpermsec2)/(dAccel_mmpermsec2+dDecel_mmpermsec2));
			if(dTmpHoist_MaxSpeed > dVel_mmpermsec) dTmpHoist_MaxSpeed = dVel_mmpermsec;
			m_dHoistSlowDownPos = (dTmpHoist_MaxSpeed * dTmpHoist_MaxSpeed) / (2*dDecel_mmpermsec2);

			ADD_LOG("[Load] HOIST_DOWN_H2SDP, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;

		case HOIST_DOWN_SDP2TO:
			dDestinationPosition_mm = m_dHoist_TeachingPos;
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true) || m_nEqType == PORT_TYPE_UTB)
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.01;
				dAccel_mmpermsec2 = 0.0015;
				dDecel_mmpermsec2 = 0.0015;
#else
				dVel_mmpermsec = 0.0367;
				dAccel_mmpermsec2 = 0.001223;
				dDecel_mmpermsec2 = 0.001223;
#endif
			}
			else
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){
					//dVel_mmpermsec = HOIST_MAX_SPEED * 0.05;
					dAccel_mmpermsec2 = 0.0035;
					dDecel_mmpermsec2 = 0.0035;
				}
				else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
				{
					//dVel_mmpermsec = HOIST_MAX_SPEED * 0.05;
					dAccel_mmpermsec2 = 0.0033;
					dDecel_mmpermsec2 = 0.0033;
				}
				else
				{
					//dVel_mmpermsec = HOIST_MAX_SPEED * 0.05;
					dAccel_mmpermsec2 = 0.006;
					if(fabs(m_dShift_TeachingPos) > HOIST_HIGH_SPEED_SHIFT_LIMIT) dDecel_mmpermsec2 = 0.003;
					else dDecel_mmpermsec2 = 0.006;
				}
#else
				dVel_mmpermsec = 0.04;
				if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){
					dAccel_mmpermsec2 = 0.0033;
					dDecel_mmpermsec2 = 0.0027;
				}
				else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
				{
					dAccel_mmpermsec2 = 0.0033;
					dDecel_mmpermsec2 = 0.0027;
				}
				else
				{
					dAccel_mmpermsec2 = 0.006;
					if(fabs(m_dShift_TeachingPos) > HOIST_HIGH_SPEED_SHIFT_LIMIT) dDecel_mmpermsec2 = 0.003;
					else dDecel_mmpermsec2 = 0.006;
				}
#endif
			}

			ADD_LOG("[Load] HOIST_DOWN_SDP2TO, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;

		case HOIST_DOWN_TO2TP:
			dDestinationPosition_mm = m_dHoist_TeachingPos;
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true) || m_nEqType == PORT_TYPE_UTB)
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.01;
				dAccel_mmpermsec2 = 0.001;
				dDecel_mmpermsec2 = 0.001;
#else
				dVel_mmpermsec = 0.0367;
				dAccel_mmpermsec2 = 0.000367;
				dDecel_mmpermsec2 = 0.000367;
#endif
			}
			else
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				dAccel_mmpermsec2 = 0.001;
				dDecel_mmpermsec2 = 0.001;
#else
				dVel_mmpermsec = 0.04;
				dAccel_mmpermsec2 = 0.00005;
				dDecel_mmpermsec2 = 0.00005;
#endif

			}

			ADD_LOG("[Load] HOIST_DOWN_TO2TP, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;

		case HOIST_UP_TP2TO:
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true) || m_nEqType == PORT_TYPE_UTB)
			{
				dDestinationPosition_mm = TransDefine->nHoistMoveUp_TeachingOffset_STB_Load;
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				dAccel_mmpermsec2 = 0.001;
				dDecel_mmpermsec2 = 0.001;
#else
				dVel_mmpermsec = 0.055;
				dAccel_mmpermsec2 = 0.000367;
				dDecel_mmpermsec2 = 0.000367;
#endif
			}
			else
			{
				dDestinationPosition_mm = TransDefine->nHoistMoveUp_TeachingOffset_Load;
#if(OHT_NAME == OHT_NAME_STD_V85)
				dHoist_Up_TP2TO_Speed_Load = HOIST_MAX_SPEED * 0.03;
#else
				dHoist_Up_TP2TO_Speed_Load = 0.1;
#endif
				dVel_mmpermsec = dHoist_Up_TP2TO_Speed_Load;
				dAccel_mmpermsec2 = 0.001;
				dDecel_mmpermsec2 = 0.001;
			}

			ADD_LOG("[Load] HOIST_UP_TP2TO, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;

		case HOIST_UP_TO2SDP:
			dDestinationPosition_mm = TransDefine->nHoistMoveUp_HomeOffset_Load;
			if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.5;
				dAccel_mmpermsec2 = 0.004;
				dDecel_mmpermsec2 = 0.004;
#else
				dVel_mmpermsec = 1.284;
				dAccel_mmpermsec2 = 0.0062;
				dDecel_mmpermsec2 = 0.0062;
#endif
			}
			else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.7;
				dAccel_mmpermsec2 = 0.006;
				dDecel_mmpermsec2 = 0.006;
#else
				dVel_mmpermsec = 1.835;
				dAccel_mmpermsec2 = 0.0062;
				dDecel_mmpermsec2 = 0.0062;
#endif
			}
			else
			{
				if(fabs(m_dShift_TeachingPos) > HOIST_HIGH_SPEED_SHIFT_LIMIT){
#if(OHT_NAME == OHT_NAME_STD_V85)
					dVel_mmpermsec = HOIST_MAX_SPEED * 0.8;
#else
					dVel_mmpermsec = 2.2;
#endif
					dAccel_mmpermsec2 = 0.006;
					dDecel_mmpermsec2 = 0.006;
				}
				else{
#if(OHT_NAME == OHT_NAME_STD_V85)
					dVel_mmpermsec = HOIST_MAX_SPEED * 1.0;
#else
					dVel_mmpermsec = 2.75;
#endif
					dAccel_mmpermsec2 = 0.006;
					dDecel_mmpermsec2 = 0.006;
				}
			}


			dTmpMovePos = (m_pHWSet->m_pHoistAxis->GetCurrentPosition()-TransDefine->nHoistMoveUp_HomeOffset_Load)+
				(dHoist_Up_TP2TO_Speed_Load*dHoist_Up_TP2TO_Speed_Load/(dAccel_mmpermsec2*2));

			   //		((m_pParamSet->TransDefine.dHoist_Up_TP2TO_Speed_Load*m_pParamSet->TransDefine.dHoist_Up_TP2TO_Speed_Load)/(2*m_pParamSet->TransDefine.dHoist_Up_TO2HO_AccTime_Load));
			dTmpHoist_MaxSpeed = sqrt((2*dTmpMovePos*dAccel_mmpermsec2*dDecel_mmpermsec2)/(dAccel_mmpermsec2+dDecel_mmpermsec2));
			if(dTmpHoist_MaxSpeed > dVel_mmpermsec) dTmpHoist_MaxSpeed = dVel_mmpermsec;
			m_dHoistSlowDownPos = (dTmpHoist_MaxSpeed * dTmpHoist_MaxSpeed) / (2*dDecel_mmpermsec2);


			ADD_LOG("[Load] HOIST_UP_TO2SDP, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;

		case HOIST_UP_SDP2HO:
			dDestinationPosition_mm = 0.0;
#if(OHT_NAME == OHT_NAME_STD_V85)
			if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				dAccel_mmpermsec2 = 0.004;
				dDecel_mmpermsec2 = 0.004;
			}
			else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
			{
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				dAccel_mmpermsec2 = 0.006;
				dDecel_mmpermsec2 = 0.006;
			}
			else
			{
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				dAccel_mmpermsec2 = 0.006;
				dDecel_mmpermsec2 = 0.006;
			}
#else
			dVel_mmpermsec = 0.184;
			if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){
				dAccel_mmpermsec2 = 0.0062;
				dDecel_mmpermsec2 = 0.0062;
			}
			else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
			{
				dAccel_mmpermsec2 = 0.0062;
				dDecel_mmpermsec2 = 0.0062;
			}
			else
			{
				dAccel_mmpermsec2 = 0.006;
				dDecel_mmpermsec2 = 0.006;
			}
#endif
			ADD_LOG("[Load] HOIST_UP_SDP2HO, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;

		case HOIST_UP_TO2H:
			dDestinationPosition_mm = 0.0;
#if(OHT_NAME == OHT_NAME_STD_V85)
			dVel_mmpermsec = HOIST_MAX_SPEED * 0.3;
#else
			dVel_mmpermsec = 0.734;
#endif
			dAccel_mmpermsec2 = 0.0015;
			dDecel_mmpermsec2 = 0.0015;

			ADD_LOG("[Load] HOIST_UP_TO2H, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
#ifdef OHT_HIGH_RAIL_CHECK_HOME
			nError = Move_Hoist_HomePosition(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec);
			return nError;
#else
			break;
#endif

		case HOIST_UP_HO2H:
			dDestinationPosition_mm = 0.0;
#if(OHT_NAME == OHT_NAME_STD_V85)
			dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
			dAccel_mmpermsec2 = 0.001;
			dDecel_mmpermsec2 = 0.001;
#else
			dVel_mmpermsec = 0.184;
			dAccel_mmpermsec2 = 0.0006;
			dDecel_mmpermsec2 = 0.0006;
#endif
			ADD_LOG("[Load] HOIST_UP_HO2H, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
#ifdef OHT_HIGH_RAIL_CHECK_HOME
			nError = Move_Hoist_HomePosition(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec);
			return nError;
#else
			break;
#endif

		}
	}

	else if((m_nTransType == TRANSTYPE_UNLOAD) || (m_nTransType == TRANSTYPE_AUTOTEACHING))
	{
		switch(nMoveCase)
		{
		case HOIST_DOWN_H2SDP:
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true) || m_nEqType == PORT_TYPE_UTB)
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dDestinationPosition_mm = m_dHoist_TeachingPos - TransDefine->nHoistMoveUp_HomeOffset_STB_Unload;
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.15;
				dAccel_mmpermsec2 = 0.0012;
				dDecel_mmpermsec2 = 0.0012;
#else
				dDestinationPosition_mm = m_dHoist_TeachingPos - TransDefine->nHoistMoveDown_TeachingOffset_STB_Unload;
				dVel_mmpermsec = 0.367;
				dAccel_mmpermsec2 = 0.00123;
				dDecel_mmpermsec2 = 0.00123;
#endif
			}
			else
			{
				dDestinationPosition_mm = m_dHoist_TeachingPos - TransDefine->nHoistMoveDown_TeachingOffset_Unload;
				if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){
#if(OHT_NAME == OHT_NAME_STD_V85)
					dVel_mmpermsec = HOIST_MAX_SPEED * 0.5;
					dAccel_mmpermsec2 = 0.004;
					dDecel_mmpermsec2 = 0.004;
#else
					dVel_mmpermsec = 1.285;
					dAccel_mmpermsec2 = 0.0062;
					dDecel_mmpermsec2 = 0.0062;
#endif
				}
				else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
				{
#if(OHT_NAME == OHT_NAME_STD_V85)
					dVel_mmpermsec = HOIST_MAX_SPEED * 0.7;
					dAccel_mmpermsec2 = 0.006;
					dDecel_mmpermsec2 = 0.006;
#else
					dVel_mmpermsec = 1.835;
					dAccel_mmpermsec2 = 0.0062;
					dDecel_mmpermsec2 = 0.0062;
#endif
				}
				else
				{

					if(fabs(m_dShift_TeachingPos) > HOIST_HIGH_SPEED_SHIFT_LIMIT) {
#if(OHT_NAME == OHT_NAME_STD_V85)
						dVel_mmpermsec = HOIST_MAX_SPEED * 0.8;
#else
						dVel_mmpermsec = 2.2;
#endif
						dAccel_mmpermsec2 = 0.006;
						dDecel_mmpermsec2 = 0.003;
					}
					else {
#if(OHT_NAME == OHT_NAME_STD_V85)
						dVel_mmpermsec = HOIST_MAX_SPEED * 1.0;
#else
						dVel_mmpermsec = 2.75;
#endif
						dAccel_mmpermsec2 = 0.006;
						dDecel_mmpermsec2 = 0.006;
					}
				}
			}

			if(dDestinationPosition_mm <= 0.0) dDestinationPosition_mm = m_dHoist_TeachingPos;


			dTmpHoist_MaxSpeed =  sqrt((2*dDestinationPosition_mm*dAccel_mmpermsec2*dDecel_mmpermsec2)/(dAccel_mmpermsec2+dDecel_mmpermsec2));
			if(dTmpHoist_MaxSpeed > dVel_mmpermsec) dTmpHoist_MaxSpeed = dVel_mmpermsec;
			m_dHoistSlowDownPos = (dTmpHoist_MaxSpeed * dTmpHoist_MaxSpeed) / (2*dDecel_mmpermsec2);

			ADD_LOG("[Unload] HOIST_DOWN_H2SDP, dTmpHoist_MaxSpeed:%f,m_dHoistSlowDownPos:%f",dTmpHoist_MaxSpeed,m_dHoistSlowDownPos);
			ADD_LOG("[Unload] HOIST_DOWN_H2SDP, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;

		case HOIST_DOWN_SDP2TO:
			dDestinationPosition_mm = m_dHoist_TeachingPos;
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true) || m_nEqType == PORT_TYPE_UTB)
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.07;
				dAccel_mmpermsec2 = 0.0012;
				dDecel_mmpermsec2 = 0.0012;
#else
				dVel_mmpermsec = 0.0367;
				dAccel_mmpermsec2 = 0.000367;
				dDecel_mmpermsec2 = 0.000367;
#endif
			}
			else
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){
					//dVel_mmpermsec = HOIST_MAX_SPEED * 0.05;
					dAccel_mmpermsec2 = 0.004;
					dDecel_mmpermsec2 = 0.004;
				}
				else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
				{
					//dVel_mmpermsec = HOIST_MAX_SPEED * 0.05;
					dAccel_mmpermsec2 = 0.006;
					dDecel_mmpermsec2 = 0.006;
				}
				else
				{
					//dVel_mmpermsec = HOIST_MAX_SPEED * 0.05;
					dAccel_mmpermsec2 = 0.006;
					if(fabs(m_dShift_TeachingPos) > HOIST_HIGH_SPEED_SHIFT_LIMIT) dDecel_mmpermsec2 = 0.003;
					else dDecel_mmpermsec2 = 0.006;
				}
#else
				dVel_mmpermsec = 0.0367;
				if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){

					dAccel_mmpermsec2 = 0.0062;
					dDecel_mmpermsec2 = 0.0062;
				}
				else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
				{

					dAccel_mmpermsec2 = 0.0062;
					dDecel_mmpermsec2 = 0.0062;
				}
				else
				{

					dAccel_mmpermsec2 = 0.006;
					if(fabs(m_dShift_TeachingPos) > HOIST_HIGH_SPEED_SHIFT_LIMIT) dDecel_mmpermsec2 = 0.003;
					else dDecel_mmpermsec2 = 0.006;
				}
#endif
			}

			ADD_LOG("[Unload] HOIST_DOWN_SDP2TO, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;

		case HOIST_DOWN_TO2TP:
			dDestinationPosition_mm = m_dHoist_TeachingPos;
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true) || m_nEqType == PORT_TYPE_UTB)
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.07;
				dAccel_mmpermsec2 = 0.001;
				dDecel_mmpermsec2 = 0.001;
#else

				dVel_mmpermsec = 0.0367;
				dAccel_mmpermsec2 = 0.000367;
				dDecel_mmpermsec2 = 0.000367;
#endif
			}
			else
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				dAccel_mmpermsec2 = 0.001;
				dDecel_mmpermsec2 = 0.001;
#else
				dVel_mmpermsec = 0.0367;
				dAccel_mmpermsec2 = 0.002;
				dDecel_mmpermsec2 = 0.002;
#endif
			}

			ADD_LOG("[Unload] HOIST_DOWN_TO2TP, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;

		case HOIST_UP_TP2TO:
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true) || m_nEqType == PORT_TYPE_UTB)
			{
				dDestinationPosition_mm = TransDefine->nHoistMoveUp_TeachingOffset_STB_Unload;
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				dAccel_mmpermsec2 = 0.001;
				dDecel_mmpermsec2 = 0.001;
#else
				dVel_mmpermsec = 0.0257;
				dAccel_mmpermsec2 = 0.000367;
				dDecel_mmpermsec2 = 0.000367;
#endif
			}
			else
			{
				dDestinationPosition_mm = TransDefine->nHoistMoveUp_TeachingOffset_Unload;
#if(OHT_NAME == OHT_NAME_STD_V85)
				dHoist_Up_TP2TO_Speed_Unload = HOIST_MAX_SPEED * 0.03;
				dVel_mmpermsec = dHoist_Up_TP2TO_Speed_Unload;
				dAccel_mmpermsec2 = 0.001;
				dDecel_mmpermsec2 = 0.001;
#else
				dHoist_Up_TP2TO_Speed_Unload = 0.0165;
				dVel_mmpermsec = dHoist_Up_TP2TO_Speed_Unload;
				dAccel_mmpermsec2 = 0.000367;
				dDecel_mmpermsec2 = 0.000367;
#endif
			}

			ADD_LOG("[Unload] HOIST_UP_TP2TO, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;

		case HOIST_UP_TO2SDP:
			dDestinationPosition_mm = TransDefine->nHoistMoveUp_HomeOffset_Unload;
			if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.45;
				dAccel_mmpermsec2 = 0.0035;
				dDecel_mmpermsec2 = 0.0035;
#else
				dVel_mmpermsec = 0.92;
				dAccel_mmpermsec2 = 0.00428;
				dDecel_mmpermsec2 = 0.00214;
#endif
			}
			else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.6;
				dAccel_mmpermsec2 = 0.004;
				dDecel_mmpermsec2 = 0.002;
#else
				dVel_mmpermsec = 1.285;
				dAccel_mmpermsec2 = 0.00428;
				dDecel_mmpermsec2 = 0.00214;
#endif
			}
			else
			{
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.8;
#else
				dVel_mmpermsec = 2.2;
#endif
				dAccel_mmpermsec2 = 0.006;
				dDecel_mmpermsec2 = 0.0045;
			}
			dTmpMovePos = (m_pHWSet->m_pHoistAxis->GetCurrentPosition()-TransDefine->nHoistMoveUp_HomeOffset_Unload)+
				(dHoist_Up_TP2TO_Speed_Unload*dHoist_Up_TP2TO_Speed_Unload/(dAccel_mmpermsec2*2));

			dTmpHoist_MaxSpeed = sqrt((2*dTmpMovePos*dAccel_mmpermsec2*dDecel_mmpermsec2)/(dAccel_mmpermsec2+dDecel_mmpermsec2));
			if(dTmpHoist_MaxSpeed > dVel_mmpermsec) dTmpHoist_MaxSpeed = dVel_mmpermsec;
			m_dHoistSlowDownPos = (dTmpHoist_MaxSpeed * dTmpHoist_MaxSpeed) / (2*dDecel_mmpermsec2);

			ADD_LOG("[Unload] HOIST_UP_TO2SDP, dTmpHoist_MaxSpeed:%f,m_dHoistSlowDownPos:%f",dTmpHoist_MaxSpeed,m_dHoistSlowDownPos);
			ADD_LOG("[Unload] HOIST_UP_TO2SDP, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;

		case HOIST_UP_SDP2HO:
			dDestinationPosition_mm = 0.0;
#if(OHT_NAME == OHT_NAME_STD_V85)
			if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				dAccel_mmpermsec2 = 0.0035;
				dDecel_mmpermsec2 = 0.0035;
			}
			else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
			{
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				dAccel_mmpermsec2 = 0.004;
				dDecel_mmpermsec2 = 0.002;
			}
			else
			{
				dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
				dAccel_mmpermsec2 = 0.006;
				dDecel_mmpermsec2 = 0.0045;
			}
#else
			if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit){
				dVel_mmpermsec = 0.184;
				dAccel_mmpermsec2 = 0.00428;
				dDecel_mmpermsec2 = 0.00214;
			}
			else if(m_dHoist_TeachingPos < TransDefine->dFast_Move_Limit_Ver85h)
			{
				dVel_mmpermsec = 0.184;
				dAccel_mmpermsec2 = 0.00428;
				dDecel_mmpermsec2 = 0.00214;
			}
			else
			{
				dVel_mmpermsec = 0.184;
				dAccel_mmpermsec2 = 0.006;
				dDecel_mmpermsec2 = 0.0045;
			}
#endif
			ADD_LOG("[Unload] HOIST_UP_SDP2HO, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f", 
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
			break;
			
		case HOIST_UP_TO2H:
			dDestinationPosition_mm = 0.0;
#if(OHT_NAME == OHT_NAME_STD_V85)
			dVel_mmpermsec = HOIST_MAX_SPEED * 0.15;
			dAccel_mmpermsec2 = 0.001;
			dDecel_mmpermsec2 = 0.001;
#else
			dVel_mmpermsec = 0.46;
			dAccel_mmpermsec2 = 0.00184;
			dDecel_mmpermsec2 = 0.00184;
#endif
			ADD_LOG("[Unload] HOIST_UP_TO2H, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
#ifdef OHT_HIGH_RAIL_CHECK_HOME
			nError = Move_Hoist_HomePosition(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec);
			return nError;
#else
			break;
#endif
			
		case HOIST_UP_HO2H:
			dDestinationPosition_mm = 0.0;
#if(OHT_NAME == OHT_NAME_STD_V85)
			dVel_mmpermsec = HOIST_MAX_SPEED * 0.03;
			dAccel_mmpermsec2 = 0.001;
			dDecel_mmpermsec2 = 0.001;
#else
			dVel_mmpermsec = 0.184;
			dAccel_mmpermsec2 = 0.0006;
			dDecel_mmpermsec2 = 0.0006;
#endif


			ADD_LOG("[Unload] HOIST_UP_HO2H, Vel:%f, Acc:%f, Dec:%f, CurPos:%f, DesPos:%f",
				dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2, dStartPosition_mm, dDestinationPosition_mm);
#ifdef OHT_HIGH_RAIL_CHECK_HOME
			nError = Move_Hoist_HomePosition(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec);
			return nError;
#else
			break;
#endif

		}
		
	}
	else
	{	switch(nMoveCase)
		{
		case HOIST_UP_2H:
			// 1. check hoist home sensor on/off
	     		// 2. home sensor?? off?? ??? small add?? hoist home 8?? move up
	     		// 3. home sensor?? on ?? ??? small add?? hoist home off ???? move down (??? 10mm), off ??? 2?? ???? ????
#if(OHT_NAME == OHT_NAME_STD_V85)
				dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_10P;
#else
	     		dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_6P;
#endif
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
#ifdef OHT_HIGH_RAIL_CHECK_HOME
				if(m_pHWSet->m_pHoistAxis->IsHoistHome() == false)
				{
					ADD_LOG("IsHoistHome OFF");
					nError = Move_Hoist_HomePosition(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec);
                	dwHoistHomeSensorTime = timeGetTime();
				}
	     		else
				{
					ADD_LOG("IsHoistHome ON");
					nError = Move_Hoist_HomeSensorOffPosition(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec);
					if(nError == NO_ERR)
					{
						nError = Move_Hoist_HomePosition(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec);
                    	dwHoistHomeSensorTime = timeGetTime();
					}
					else
					{
						ADD_LOG("Small add fail.. couldn't go home off position");
						//nError = error code..
					}

	     		}

				return nError;
#else
			SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), 0.0, dVel_mmpermsec);
			nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
			break;
#endif
			case HOIST_DOWN_1:    // hoist를 위 또는 아래로 일정량 움직임..hoist home auto setting 시 사용
			case HOIST_UP_1:
			case HOIST_UP_x:
			case HOIST_UP_xp:
				double dMoveDist_1mm = 1.0;
				double dMoveDist_xmm = 5.0;
				double dMoveDist_xpmm = 30.0;

				dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_3P;
				dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
				dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;

				if(nMoveCase == HOIST_DOWN_1)
					m_dCmdHoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition() + dMoveDist_1mm;
				else if(nMoveCase == HOIST_UP_1)
					m_dCmdHoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition() - dMoveDist_1mm;
				else if(nMoveCase == HOIST_UP_x)
					m_dCmdHoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition() - dMoveDist_xmm;
				else if(nMoveCase == HOIST_UP_xp)
					m_dCmdHoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition() - dMoveDist_xpmm;

				nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);

			break;

		}
	}

	SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), dDestinationPosition_mm, dVel_mmpermsec);
	nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
	m_mccparam.dHoist_Acc = dAccel_mmpermsec2;
	m_mccparam.dHoist_Dec = dDecel_mmpermsec2;
	m_mccparam.dHoist_Vel = dVel_mmpermsec;

	m_mccparam.dHoist_StartPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition();

	return nError;
}
#endif

/**
 @brief   Hoist축이 명령 받은 위치로 이동이 완료되었는지 확인하는 함수
 @author  LSJ
 @date    2012.11.12
 @param   nMoveCase(어디에서 시작해서 어디로 가는 이동인지 선택)
 @return  에러가 없으면 0, 에러가 발생되면 에러코드 리턴.
 */
int TransControl::Check_Hoist(int nMotionType)
{
	int nError = 0;

	double dStartPos = fabs(m_dCmdHoistStartPos);
    double dEndPos = fabs(m_dCmdHoistPos);
    double dCurPos = fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition());
    double dEndSpeed = fabs(m_dCmdHoistSpeed);
    double dCurSpeed = fabs(m_pHWSet->m_pHoistAxis->GetCurrentVelocity());
    bool bRunning = false;
	static int nErrCount = 0;
	static int nErrCount_2 = 0;
	static int nErrCount_3 = 0;
	static int nCount = 0;

    bRunning = (m_pHWSet->m_pHoistAxis->IsRunning() == true) ? true : false;
	

	if(m_TransUse.HoistHomeSen == true)
	{

		if((m_dCmdHoistPos ==  m_dHoist_TeachingPos) && (m_nTransType != TRANSTYPE_MOVET)  &&(m_nTransType != TRANSTYPE_MOVE_ALL_HOME) && (m_nTransType != TRANSTYPE_AUTO_CHANGE))
        {
#if((OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
			if(m_pHWSet->m_pHoistAxis->IsHoistHome() == true)
#else
			if((m_pHWSet->m_pHoistAxis->IsHoistHome() == true) && (m_pHWSet->m_pHand->IsGoSigOn()==true))
#endif
			{
				nErrCount_2++;
				if(nErrCount_2 > 15)
				{
					nError = ERR_HOISTALIGNSEN_ABNORMAL_ON;
					ADD_LOG("Check_Hoist[Step=%d]: ERR_HOISTALIGNSEN_ABNORMAL_ON", m_nTransStep);
					ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
							ERR_HOISTALIGNSEN_ABNORMAL_ON,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHoistAxis->IsHoistHome(),
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
                }
			}else
				nErrCount_2 = 0;
        }
		else if(dCurPos == 0.0)
        {
			if((m_pHWSet->m_pHoistAxis->IsHoistHome() == false) && (dCurPos > HOIST_HOME_INPOS_LIMIT))
			{
				nErrCount++;
                if(nErrCount > 100)
                {
                    nError = ERR_HOISTALIGNSEN_NOT_ON;
					ADD_LOG("Check_Hoist[Step=%d]: ERR_HOISTALIGNSEN_NOT_ON", m_nTransStep);
					ADD_FDC_LOG("ERR/%d/3/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
							ERR_HOISTALIGNSEN_NOT_ON,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHoistAxis->IsHoistHome(),
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
				}
            }
            else
                nErrCount = 0;
        }
		else
		{
			nErrCount_2 = 0;
			nErrCount = 0;
		}
    }
	else
	{
		nErrCount_2 = 0;
		nErrCount = 0;
	}

	if(nError == NO_ERR)
	{
		switch(nMotionType)
		{
			// stop check
		case HOIST_DOWN_TO2TP:     
		case HOIST_DOWN_H2TP:
			// 티칭 값이 길 때 Push 센서를 보고 바로 정지함.
#if((OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
			if(m_pHWSet->m_pHand->IsPushOn() == true)
#else
			if((m_pHWSet->m_pHand->IsPushOn() == true) && (m_pHWSet->m_pHand->IsGoSigOn()==true))
#endif
			{

			   if(m_dCmdHoistPos - m_pHWSet->m_pHoistAxis->GetCurrentPosition() > 3.0)
			   {
					nCount++;
					if(nCount > 2)
					{
						nError = m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL*1.5);
						m_dCmdFinal_HoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
						while(bRunning){
							Sleep(10);
							bRunning = (m_pHWSet->m_pHoistAxis->IsRunning() == true) ? true : false;
						 }
						ADD_LOG("Check_Hoist[Step=%d]: HOIST_DOWN_H2TP HandPushOn ", m_nTransStep);
						m_nTransStep++;
						return nError;
					}
				}
			}
			else
				nCount = 0;

			if(dCurPos < (dStartPos + dEndPos) / 2.0)
				return nError;
			if(fabs(dEndPos - dCurPos) > INPOS_LIMIT)
				return nError;
			if(bRunning == true)
				return nError;
			break;
		case HOIST_UP_HO2H:
			if((m_mccparam.bpreHoistHomeSensor == false) && (m_pHWSet->m_pHoistAxis->IsHoistHome() == true))
			{
				m_mccparam.dHoist_HomeOnPos = dCurPos;
			}
			m_mccparam.bpreHoistHomeSensor = m_pHWSet->m_pHoistAxis->IsHoistHome();

			if(dCurPos > (dStartPos + dEndPos) / 2.0)
				return nError;
			if(fabs(dEndPos - dCurPos) > INPOS_LIMIT)
				return nError;
			if(bRunning == true)
				return nError;
			break;
			// moving check
		case HOIST_DOWN_H2TO:
			if((m_mccparam.bpreHoistHomeSensor == true) && (m_pHWSet->m_pHoistAxis->IsHoistHome() == false))
			{
				m_mccparam.dHoist_HomeOffPos = dCurPos;
			}
			m_mccparam.bpreHoistHomeSensor = m_pHWSet->m_pHoistAxis->IsHoistHome();

			if(dCurPos < (dStartPos + dEndPos) / 2.0)
				return nError;

			if(fabs(dEndPos - dCurPos) > HOIST_MOVINPOS_LIMIT)
				return nError;

			if(bRunning == true)
			{
				// if (dCurSpeed > dEndSpeed*1.5)
				if(dCurSpeed > dEndSpeed*1.25)
					return nError;
			}
			break;
		case HOIST_UP_TP2TO:
			if(dCurPos > (dStartPos + dEndPos) / 1.5)
			{
				ADD_LOG(" %d, %d, %d",dCurPos,dStartPos,dEndPos)
				return nError;
			}

			if(fabs(dEndPos - dCurPos) > HOIST_MOVINPOS_LIMIT)
				return nError;
			if(bRunning == true)
			{
				// if (dCurSpeed > dEndSpeed*1.5)
				if(dCurSpeed > dEndSpeed*1.25)
					return nError;
			}
			break;
		case HOIST_UP_TP2HO:
		case HOIST_UP_TO2HO:
		case HOIST_UP_TO2H:
                        if((m_mccparam.bpreHoistHomeSensor == false) && (m_pHWSet->m_pHoistAxis->IsHoistHome() == true))
			{
				m_mccparam.dHoist_HomeOnPos = dCurPos;
			}
			m_mccparam.bpreHoistHomeSensor = m_pHWSet->m_pHoistAxis->IsHoistHome();
			if(dCurPos > (dStartPos + dEndPos) / 2.0)
				return nError;
			if(fabs(dEndPos - dCurPos) > HOIST_MOVINPOS_LIMIT)
				return nError;
			if(bRunning == true)
			{
				// if (dCurSpeed > dEndSpeed*1.5)
				if(dCurSpeed > dEndSpeed*1.25)
					return nError;
			}
			break;

		case HOIST_UP_2H:
			if(dCurPos > (dStartPos + dEndPos) / 2.0)
			{
				if(dCurPos > 0.01)
					return nError;
			}
			if(fabs(dEndPos - dCurPos) > INPOS_LIMIT)
			{
				return nError;
			}
			if(bRunning == true)
			{
//				// if (dCurSpeed > dEndSpeed*1.5)
//				if(dCurSpeed > dEndSpeed*1.25)
					return nError;
			}
			break;
		}
	}

	if(nError == NO_ERR)
	{
		m_nTransStep++;
		m_mccparam.dHoist_EndPos = dCurPos;
	}

    return nError;
}

#ifdef USE_TRANS_PROFILE_VER2

int TransControl::Check_Hoist_Ver2(int nMotionType)
{
	int nError = 0;

	double dStartPos = fabs(m_dCmdHoistStartPos);
	double dEndPos = fabs(m_dCmdHoistPos);
	double dCurPos = fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition());
	double dEndSpeed = fabs(m_dCmdHoistSpeed);
	double dCurSpeed = fabs(m_pHWSet->m_pHoistAxis->GetCurrentVelocity());
	bool bRunning = false;
	static int nErrCount = 0;
	static int nErrCount_2 = 0;
	static int nCount = 0;
	double dVel_mmpermsec;
	double dAccel_mmpermsec2;
	double dDecel_mmpermsec2;
	bRunning = (m_pHWSet->m_pHoistAxis->IsRunning() == true) ? true : false;

	double dCurPos2 = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
//	static DWORD dwHoistHomeSensorTime = timeGetTime();

//	ADD_HOIST_POS_LOG("%f", dCurPos2);
	if(m_TransUse.HoistHomeSen == true)
	{

		if((m_dCmdHoistPos ==  m_dHoist_TeachingPos) && (m_nTransType != TRANSTYPE_MOVET)  &&(m_nTransType != TRANSTYPE_MOVE_ALL_HOME)
		&&(m_nTransType != TRANSTYPE_SMALLADD_HOIST_HOME)  && (m_nTransType != TRANSTYPE_AUTO_CHANGE))
		{
#if((OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
			if(m_pHWSet->m_pHoistAxis->IsHoistHome() == true)
#else
			if((m_pHWSet->m_pHoistAxis->IsHoistHome() == true) && (m_pHWSet->m_pHand->IsGoSigOn()==true))
#endif
			{
				nErrCount_2++;
				if(nErrCount_2 > 15)
				{
					nError = ERR_HOISTALIGNSEN_ABNORMAL_ON;
					ADD_LOG("Check_Hoist[Step=%d]: ERR_HOISTALIGNSEN_ABNORMAL_ON", m_nTransStep);
				}
			}
			else
				nErrCount_2 = 0;
		}
		else if(dCurPos == 0.0)
		{
			if((m_pHWSet->m_pHoistAxis->IsHoistHome() == false) && (dCurPos > HOIST_HOME_INPOS_LIMIT))
			{
				nErrCount++;
				if(nErrCount > 100)
				{
					nError = ERR_HOISTALIGNSEN_NOT_ON;
					ADD_LOG("Check_Hoist[Step=%d]: ERR_HOISTALIGNSEN_NOT_ON", m_nTransStep);
				}
			}
			else
				nErrCount = 0;
		}
		else
		{
			nErrCount_2 = 0;
			nErrCount = 0;
		}
	}
	else
	{
		nErrCount_2 = 0;
		nErrCount = 0;
	}

	if(nError == NO_ERR)
	{
		switch(nMotionType)
		{
		// stop check
		case HOIST_DOWN_H2SDP:
			if((m_mccparam.bpreHoistHomeSensor == true) && (m_pHWSet->m_pHoistAxis->IsHoistHome() == false))
			{
				m_mccparam.dHoist_HomeOffPos = dCurPos;
			}
			m_mccparam.bpreHoistHomeSensor = m_pHWSet->m_pHoistAxis->IsHoistHome();

			if(fabs(dEndPos - dCurPos) > m_dHoistSlowDownPos){
				return nError;
			}
			ADD_LOG("HOIST_DOWN_H2SDP end. Hoist Position: %f", dCurPos);
			break;

		case HOIST_DOWN_SDP2TO:
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true) || m_nEqType == PORT_TYPE_UTB)
			{
				if(m_nTransType == TRANSTYPE_LOAD){
					if(fabs(dEndPos - dCurPos) > m_pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_STB_Load){
						return nError;
					}
				}
				else if(m_nTransType == TRANSTYPE_UNLOAD){
					if(fabs(dEndPos - dCurPos) > m_pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_STB_Unload){
						return nError;
					}
				}
			}
			else
			{
				if(m_nTransType == TRANSTYPE_LOAD){
					if(fabs(dEndPos - dCurPos) > m_pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_Load){
						return nError;
					}
				}
				else if(m_nTransType == TRANSTYPE_UNLOAD){
					if(fabs(dEndPos - dCurPos) > m_pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_Unload){
						return nError;
					}
				}
			}
			ADD_LOG("HOIST_DOWN_SDP2TO end. Hoist Position: %f", dCurPos);
			break;

		case HOIST_DOWN_TO2TP:
		case HOIST_DOWN_H2TP:
			if(m_pHWSet->m_pHand->IsPushOn() == true)
			{
				if(m_dCmdHoistPos - m_pHWSet->m_pHoistAxis->GetCurrentPosition() > 3.0)
				{
					nCount++;
					if(nCount > 2)
					{
						nError = m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL*1.5);
						m_dCmdFinal_HoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
						while(bRunning){
							Sleep(10);
							bRunning = (m_pHWSet->m_pHoistAxis->IsRunning() == true) ? true : false;
						}
						ADD_LOG("Check_Hoist[Step=%d]: HOIST_DOWN_H2TP HandPushOn ", m_nTransStep);
						m_nTransStep++;
						return nError;
					}
				}
			}
			else nCount = 0;

			if(dCurPos < (dStartPos + dEndPos) / 2.0)
				return nError;
			if(fabs(dEndPos - dCurPos) > INPOS_LIMIT)
				return nError;
			if(bRunning == true)
				return nError;
			break;

		case HOIST_UP_TP2TO:
			if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R) || (m_bIsSTBTypeTrans == true) || m_nEqType == PORT_TYPE_UTB)
			{
            			if(m_nTransType == TRANSTYPE_LOAD){
					if(fabs(dStartPos - dCurPos) < m_pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_STB_Load){
						return nError;
					}
				}
				else if(m_nTransType == TRANSTYPE_UNLOAD){
					if(fabs(dStartPos - dCurPos) < m_pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_STB_Unload){
						return nError;
					}
				}
			}
			else
			{
        			if(m_nTransType == TRANSTYPE_LOAD){
					if(fabs(dStartPos - dCurPos) < m_pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_Load){
						return nError;
					}
				}
				else if(m_nTransType == TRANSTYPE_UNLOAD){
					if(fabs(dStartPos - dCurPos) < m_pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_Unload){
						return nError;
					}
				}
			}
			ADD_LOG("HOIST_UP_TP2TO end. Hoist Position: %f", dCurPos);
			break;

		case HOIST_UP_TO2SDP:
				if(fabs(dEndPos - dCurPos) > m_dHoistSlowDownPos){
					//ADD_LOG("HOIST_UP_TO2SDP end");
					return nError;
				}
				ADD_LOG("HOIST_UP_TO2SDP end. Hoist Position: %f", dCurPos);
			break;

		case HOIST_UP_SDP2HO:
			if((m_mccparam.bpreHoistHomeSensor == false) && (m_pHWSet->m_pHoistAxis->IsHoistHome() == true))
			{
				m_mccparam.dHoist_HomeOnPos = dCurPos;
			}
			m_mccparam.bpreHoistHomeSensor = m_pHWSet->m_pHoistAxis->IsHoistHome();

			if(m_nTransType == TRANSTYPE_LOAD){
				if(fabs(dEndPos - dCurPos) > m_pParamSet->TransDefine.nHoistMoveUp_HomeOffset_Load){
					return nError;
				}
			}
			else if(m_nTransType == TRANSTYPE_UNLOAD){
				if(fabs(dEndPos - dCurPos) > m_pParamSet->TransDefine.nHoistMoveUp_HomeOffset_Unload){
					return nError;
				}
			}
			ADD_LOG("HOIST_UP_SDP2HO end. Hoist Position: %f", dCurPos);
			break;

#ifdef OHT_HIGH_RAIL_CHECK_HOME
		case HOIST_UP_HO2H: // small add?Π?Ϸፊ
		case HOIST_UP_TO2H:
		case HOIST_UP_2H:
			if(m_pHWSet->m_pHoistAxis->IsHoistHome() == true)
			{

				if(m_pHWSet->m_pHoistAxis->IsRunning() == false)
				{
					if(OHTUTILITY::PreventOverflowGetTimeMs(dwHoistHomeSensorTime) > 500)
					{
						m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
						m_pHWSet->m_pHoistAxis->SetHoistOriginFromSmalladd(true);
						ADD_LOG("HOIST_UP_Home end_1. Hoist Position: %f T:%d", dCurPos2,OHTUTILITY::PreventOverflowGetTimeMs(dwHoistHomeSensorTime));
                        m_dCmdFinal_HoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
						break;
					}
				}

				if(OHTUTILITY::PreventOverflowGetTimeMs(dwHoistHomeSensorTime) > 6000)
				{
					m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
					ADD_LOG("HOIST_UP_Home end_2. Hoist Position: %f T:%d", dCurPos2,OHTUTILITY::PreventOverflowGetTimeMs(dwHoistHomeSensorTime));
                    m_dCmdFinal_HoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
					break;
				}

				return nError;
			}
			else
			{
				dwHoistHomeSensorTime = timeGetTime();

				if(dCurPos2 < -2.0)
				{
 					nError = ERR_HOISTALIGNSEN_NOT_ON;
				}
				return nError;
			}
#else
		case HOIST_UP_HO2H:
			if(dCurPos > (dStartPos + dEndPos) / 2.0)
				return nError;
			if(fabs(dEndPos - dCurPos) > INPOS_LIMIT)
				return nError;
			if(bRunning == true)
				return nError;
		break;

		case HOIST_UP_TO2H:
                        if((m_mccparam.bpreHoistHomeSensor == false) && (m_pHWSet->m_pHoistAxis->IsHoistHome() == true))
			{
				m_mccparam.dHoist_HomeOnPos = dCurPos;
			}
			m_mccparam.bpreHoistHomeSensor = m_pHWSet->m_pHoistAxis->IsHoistHome();

			if(dCurPos > (dStartPos + dEndPos) / 2.0)
				return nError;
			if(fabs(dEndPos - dCurPos) > HOIST_MOVINPOS_LIMIT)
				return nError;
			if(bRunning == true)
			{
				// if (dCurSpeed > dEndSpeed*1.5)
				if(dCurSpeed > dEndSpeed*1.25)
				return nError;
			}
			break;

		case HOIST_UP_2H:
			if(dCurPos > (dStartPos + dEndPos) / 2.0)
			{
				if(dCurPos > 0.01)
					return nError;
			}
			if(fabs(dEndPos - dCurPos) > HOIST_MOVINPOS_LIMIT)
			{
				return nError;
			}
			if(bRunning == true)
			{
				// if (dCurSpeed > dEndSpeed*1.5)
				if(dCurSpeed > dEndSpeed*1.25)
					return nError;
			}
			break;
#endif
		}

	}

	if(nError == NO_ERR)
	{
		m_nTransStep++;
		m_mccparam.dHoist_EndPos = dCurPos;
	}

	return nError;
}
#endif
/**
 @brief   Hand가 티칭 위치에 도달하여 Hand Push 센서가 On 될때까지 체크하면서 Hoist 축을 계속적으로 움직이는 함수
 @author  LSJ
 @date    2012.11.12
 @return  이동에 문제가 없으면 0, 에러가 발생되면 에러코드 리턴.
 */
int TransControl::CheckNMoveTeachingPoint()
{
    bool bRetry = true;
    int nError = 0;
    int nRetryCount = 1;
    int nMaxRetryCount = 0;
	double dVel_mmpermsec = 0.0;
 	double dCurPos_for_Retry = 0.0;
    int nCount = 0;
	AnsiString strLog = "";
	DWORD dwStTime = 0;
	DWORD dwSTime = timeGetTime();
	DWORD dwHandPIOGisignalTime = timeGetTime();

	double dAccel_mmpermsec2 = 0.0;
	int nCheckCount = 0;
	bool bHandPIOGoOffCheck = false;


	if(m_pHWSet->m_pHand->IsPushOn() == true)
	{
		m_dCmdFinal_HoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
		m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);

		m_mccparam.dHoist_PushOnPos = m_dCmdFinal_HoistPos;

		return nError;
	}
	else
	{
		nCount = 0;
		while(m_pHWSet->m_pHand->IsPushOn() == false)
		{
			nCount++;
		   if(nCount > 5) // 250msec
		   {
				break;
		   }
		Sleep(50);
  		}
	}

	dwSTime = timeGetTime();

	switch(m_nTransType)
	{
		case TRANSTYPE_LOAD:
		if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsPushOn() == false))
		{

#if(OHT_NAME == OHT_NAME_STD_V85)
			   dVel_mmpermsec = 0.02; // 등속 구간
			   SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_pHWSet->m_pHoistAxis->GetCurrentPosition() + MOVETP_RETRYCOUNT_DISTANCE, dVel_mmpermsec);
			   m_dCmdFinal_HoistPos = m_dCmdHoistPos;
			   dAccel_mmpermsec2 = 0.0002;
			   nError = Move_Hoist_SmallAdd(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos, HOIST_SMALL_ADD_HANDPUSHIO, HOIST_SMALL_ADD_HANDPUSH_DISTANCE, true);
			   // Small Add 명령 실행을 위한 Sleep 추가
#else
               dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_1P; // 등속 구간
			   SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_pHWSet->m_pHoistAxis->GetCurrentPosition() + MOVETP_RETRYCOUNT_DISTANCE, dVel_mmpermsec);
			   m_dCmdFinal_HoistPos = m_dCmdHoistPos;
			   dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
			   nError = Move_Hoist_SmallAdd(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos, HOIST_SMALL_ADD_HANDPUSHIO, HOIST_SMALL_ADD_HANDPUSH_DISTANCE);
			   // Small Add 명령 실행을 위한 Sleep 추가
#endif
			   Sleep(50);

			   while((OHTUTILITY::PreventOverflowGetTimeMs(dwSTime) < 5000))
			   {

					if(m_pHWSet->m_pHand->IsPushOn() == true)
						nCheckCount++;


					if(m_pHWSet->m_pHand->IsGoSigOn()==false)
					{

						if(bHandPIOGoOffCheck == false)
						{
							ADD_LOG("[TeachingP]HandPIOGoOff cmdPos=%.02f curPos = %.02f", m_dCmdHoistPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition());
							m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
						}

						if(OHTUTILITY::PreventOverflowGetTimeMs(dwHandPIOGisignalTime) >500)
						{
                           	return ERR_HANDPIO_GOSIG_NOT_ON;
						}
						bHandPIOGoOffCheck = true;

						continue;
					}
					else
					{
						dwHandPIOGisignalTime = timeGetTime();
						if(bHandPIOGoOffCheck == true)
						{
							ADD_LOG("[TeachingP]HandPIOGoOn cmdPos=%.02f curPos = %.02f", m_dCmdHoistPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition());
#if(OHT_NAME == OHT_NAME_STD_V85)
							dVel_mmpermsec = 0.02; // 등속 구간
							SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_pHWSet->m_pHoistAxis->GetCurrentPosition() + MOVETP_RETRYCOUNT_DISTANCE, dVel_mmpermsec);
							m_dCmdFinal_HoistPos = m_dCmdHoistPos;
							dAccel_mmpermsec2 = 0.0002;
							nError = Move_Hoist_SmallAdd(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos, HOIST_SMALL_ADD_HANDPUSHIO, HOIST_SMALL_ADD_HANDPUSH_DISTANCE, true);

#else
						   dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_1P; // 등속 구간
						   SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_pHWSet->m_pHoistAxis->GetCurrentPosition() + MOVETP_RETRYCOUNT_DISTANCE, dVel_mmpermsec);
						   m_dCmdFinal_HoistPos = m_dCmdHoistPos;
						   dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
						   nError = Move_Hoist_SmallAdd(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos, HOIST_SMALL_ADD_HANDPUSHIO, HOIST_SMALL_ADD_HANDPUSH_DISTANCE);
#endif
						}
						bHandPIOGoOffCheck = false;
                    }


					if((m_pHWSet->m_pHand->IsPushOn() == true) &&(m_pHWSet->m_pHoistAxis->IsRunning() == false))
					{
						 // 센서가 100mec 동안 ON 유지
						if(nCheckCount >= 10)
						{
							 m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
							 ADD_LOG("[Load#1]POS:%f CNT:%d", m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
							 break;
						}
					}
					// 비정상적인 경우 메인에서 Move Stop 명령 수행 (보통 센서 인식 후 80msec 이후에 정지함, 가감속 및 속도 변경시 해당 값 수정 필요)
					if(nCheckCount == 20)
					{
						m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
						ADD_LOG("[Load#2]ABNORMAL POS:%f CNT:%d", m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
					}

					if(m_pHWSet->m_pHoistAxis->GetCurrentPosition() > m_dCmdFinal_HoistPos - HOIST_SMALL_ADD_HANDPUSH_DISTANCE)
					{
						bRetry = false;
						break;
					 }
					Sleep(10);
			   }

			   if(m_pHWSet->m_pHand->IsPushOn() == false)
			   {
						// Hand PIO 노이즈 유입으로 인한 Hand Push Retry 추가 기능 구현
						if(bRetry)
						{
							dCurPos_for_Retry = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
							nMaxRetryCount = m_dCmdFinal_HoistPos - m_pHWSet->m_pHoistAxis->GetCurrentPosition();

							if(nMaxRetryCount>0)
							{
								nRetryCount = 1;
					   			dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_1P;
								nError = MoveHoist_forTeachingPoint(nRetryCount, dVel_mmpermsec, dCurPos_for_Retry);
								while(1)
								{
									if(m_pHWSet->m_pHoistAxis->IsRunning() == true)
										continue;

									if(nRetryCount > nMaxRetryCount)
										 break;

									if(m_pHWSet->m_pHand->IsPushOn() == true)
									{
										nRetryCount++;
										nError = MoveHoist_forTeachingPoint(nRetryCount, dVel_mmpermsec, dCurPos_for_Retry);
										ADD_LOG("Retry:%2d, CurPos:%f", nRetryCount, m_pHWSet->m_pHoistAxis->GetCurrentPosition());
										break;
									}
									nRetryCount++;
									nError = MoveHoist_forTeachingPoint(nRetryCount, dVel_mmpermsec, dCurPos_for_Retry);
									if(nError)
										break;
									Sleep(200);
								}

								  if(m_pHWSet->m_pHand->IsPushOn() == false)
								  {
									   ADD_LOG("CheckNMoveTeachingPoint : ERR_HAND_PUSHSEN_NOTON %d", m_nTransStep);
									   ADD_LOG("[Load#4]CmdPos:%f CurPos:%f CNT:%d", m_dCmdHoistPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
									   ADD_FDC_LOG("ERR/%d/1/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f/%f",
												ERR_HAND_PUSHSEN_NOTON,
												m_pOHTMainStatus->StatusCommon.CurNodeID,
												m_pExecuteInfo->ExecutePositionInfo.NextNode,
												m_pOHTMainStatus->StatusCommon.CurNodeOffset,
												m_pOHTMainStatus->StatusCommon.ExistFoup,
												m_pOHTMainStatus->StatusCommon.StopStationID,
												m_nTransType,
												m_nTransStep,
												m_pHWSet->m_pHand->IsPushOn(),
												m_dCmdHoistPos,
												m_pHWSet->m_pHoistAxis->GetCurrentPosition());
									   return ERR_HAND_PUSHSEN_NOTON;
								  }
							}
							else
							{
								ADD_LOG("Retry Count is Zero");
							}

						}
						else
						{
						       	ADD_LOG("CheckNMoveTeachingPoint : ERR_HAND_PUSHSEN_NOTON %d", m_nTransStep);
								ADD_LOG("[Load#5]CmdPos:%f CurPos:%f CNT:%d", m_dCmdHoistPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
								ADD_FDC_LOG("ERR/%d/1/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f/%f",
												ERR_HAND_PUSHSEN_NOTON,
												m_pOHTMainStatus->StatusCommon.CurNodeID,
												m_pExecuteInfo->ExecutePositionInfo.NextNode,
												m_pOHTMainStatus->StatusCommon.CurNodeOffset,
												m_pOHTMainStatus->StatusCommon.ExistFoup,
												m_pOHTMainStatus->StatusCommon.StopStationID,
												m_nTransType,
												m_nTransStep,
												m_pHWSet->m_pHand->IsPushOn(),
												m_dCmdHoistPos,
												m_pHWSet->m_pHoistAxis->GetCurrentPosition());
								return ERR_HAND_PUSHSEN_NOTON;
						}
			   }
			   else
			   {
						m_dCmdFinal_HoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
				//    ADD_LOG("test_load/%06d/%.02f/%.02f/%d/%d", m_pExecuteInfo->ExecutePositionInfo.CurrStation, m_dHoist_TeachingPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_nEqType, (m_pHWSet->m_pHoistAxis->GetCurrentPosition()-m_dHoist_TeachingPos));
                 		ADD_LOG("[Load#6]CmdPos:%f CurPos:%f CNT:%d", m_dCmdHoistPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
				}
			}
	  break;

	 case TRANSTYPE_UNLOAD:
	  if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsPushOn() == false))
	  {
		   dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_1P;
		   SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_pHWSet->m_pHoistAxis->GetCurrentPosition() + MOVETP_RETRYCOUNT_DISTANCE, dVel_mmpermsec);
		   m_dCmdFinal_HoistPos = m_dCmdHoistPos;
		   dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
#if(OHT_NAME == OHT_NAME_STD_V85)
			nError = Move_Hoist_SmallAdd(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos, HOIST_SMALL_ADD_HANDPUSHIO, HOIST_SMALL_ADD_HANDPUSH_DISTANCE, true);
#else
 			nError = Move_Hoist_SmallAdd(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos, HOIST_SMALL_ADD_HANDPUSHIO, HOIST_SMALL_ADD_HANDPUSH_DISTANCE);
#endif
				   // Small Add 명령 실행을 위한 Sleep 추가
		   Sleep(50);
		   while((OHTUTILITY::PreventOverflowGetTimeMs(dwSTime) < 5000))
		   {

				if(m_pHWSet->m_pHand->IsPushOn() == true)
				 nCheckCount++;



				if(m_pHWSet->m_pHand->IsGoSigOn()==false)
				{
					if(bHandPIOGoOffCheck == false)
					{
						ADD_LOG("[TeachingP]HandPIOGoOff cmdPos=%.02f curPos = %.02f", m_dCmdHoistPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition());
						m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
					}

					if(OHTUTILITY::PreventOverflowGetTimeMs(dwHandPIOGisignalTime) >500)
					{
						return ERR_HANDPIO_GOSIG_NOT_ON;
					}
					bHandPIOGoOffCheck = true;

					continue;
				}
				else
				{
					dwHandPIOGisignalTime = timeGetTime();
					if(bHandPIOGoOffCheck == true)
					{
						ADD_LOG("[TeachingP]HandPIOGoOn cmdPos=%.02f curPos = %.02f", m_dCmdHoistPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition());
						dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_1P;
						SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_pHWSet->m_pHoistAxis->GetCurrentPosition() + MOVETP_RETRYCOUNT_DISTANCE, dVel_mmpermsec);
						m_dCmdFinal_HoistPos = m_dCmdHoistPos;
						dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
#if(OHT_NAME == OHT_NAME_STD_V85)
						nError = Move_Hoist_SmallAdd(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos, HOIST_SMALL_ADD_HANDPUSHIO, HOIST_SMALL_ADD_HANDPUSH_DISTANCE, true);
#else
						nError = Move_Hoist_SmallAdd(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos, HOIST_SMALL_ADD_HANDPUSHIO, HOIST_SMALL_ADD_HANDPUSH_DISTANCE);
#endif
					}
					bHandPIOGoOffCheck = false;
				}

				if((m_pHWSet->m_pHand->IsPushOn() == true) &&(m_pHWSet->m_pHoistAxis->IsRunning() == false))
				{
					  // 센서가 100mec 동안 ON 유지
					if(nCheckCount >= 10)
					{
						 m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
						 ADD_LOG("[UnLoad#1]POS:%f CNT:%d", m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
						 break;
					}
				}
				// 비정상적인 경우 메인에서 Move Stop 명령 수행 (보통 센서 인식 후 80msec 이후에 정지함, 가감속 및 속도 변경시 해당 값 수정 필요)
				if(nCheckCount == 20)
				{
					 m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
					 ADD_LOG("[UnLoad#2]ABNORMAL POS:%f CNT:%d", m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
				}

				if(m_pHWSet->m_pHoistAxis->GetCurrentPosition() > m_dCmdFinal_HoistPos - HOIST_SMALL_ADD_HANDPUSH_DISTANCE)
				{
					bRetry = false;
					break;
				}
				Sleep(10);
		   }

		   if(m_pHWSet->m_pHand->IsPushOn() == false)
		   {
				// Hand PIO 노이즈 유입으로 인한 Hand Push Retry 추가 기능 구현
				if(bRetry)
				{
					dCurPos_for_Retry = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
					nMaxRetryCount = m_dCmdFinal_HoistPos - m_pHWSet->m_pHoistAxis->GetCurrentPosition();

					if(nMaxRetryCount>0)
					{
						nRetryCount = 1;
						dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_1P; // 등속 구간
						nError = MoveHoist_forTeachingPoint(nRetryCount, dVel_mmpermsec, dCurPos_for_Retry);
						while(1)
						{
							if(m_pHWSet->m_pHoistAxis->IsRunning() == true)
								continue;

							if(nRetryCount > nMaxRetryCount)
							 	break;

							if(m_pHWSet->m_pHand->IsPushOn() == true)
							{
								nRetryCount++;
								nError = MoveHoist_forTeachingPoint(nRetryCount, dVel_mmpermsec, dCurPos_for_Retry);
								ADD_LOG("Retry:%2d, CurPos:%f", nRetryCount, m_pHWSet->m_pHoistAxis->GetCurrentPosition());
								break;
							}

							nRetryCount++;
							nError = MoveHoist_forTeachingPoint(nRetryCount, dVel_mmpermsec, dCurPos_for_Retry);
							if(nError)
								break;
							 Sleep(200);
						}

						 if(m_pHWSet->m_pHand->IsPushOn() == false)
						  {
								ADD_LOG("CheckNMoveTeachingPoint : ERR_HAND_PUSHSEN_NOTON %d", m_nTransStep);
								ADD_LOG("[UnLoad#4]CmdPos:%f CurPos:%f CNT:%d", m_dCmdHoistPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
								if(((m_nEqType == PORT_TYPE_STB_L) && (m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect()==false)) ||
								((m_nEqType == PORT_TYPE_STB_R) && (m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect()==false)))
									return ERR_STB_FOUP_CHECK_NOEXIST;
								else
								{
									ADD_FDC_LOG("ERR/%d/2/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f/%f",
												ERR_HAND_PUSHSEN_NOTON,
												m_pOHTMainStatus->StatusCommon.CurNodeID,
												m_pExecuteInfo->ExecutePositionInfo.NextNode,
												m_pOHTMainStatus->StatusCommon.CurNodeOffset,
												m_pOHTMainStatus->StatusCommon.ExistFoup,
												m_pOHTMainStatus->StatusCommon.StopStationID,
												m_nTransType,
												m_nTransStep,
												m_pHWSet->m_pHand->IsPushOn(),
												m_dCmdHoistPos,
												m_pHWSet->m_pHoistAxis->GetCurrentPosition());
									return ERR_HAND_PUSHSEN_NOTON;
						  }

					}

					}
					else
					{
						ADD_LOG("Retry Count is Zero");
					}

				}
				else
				{
					ADD_LOG("CheckNMoveTeachingPoint : ERR_HAND_PUSHSEN_NOTON %d", m_nTransStep);
					ADD_LOG("[UnLoad#5]CmdPos:%f CurPos:%f CNT:%d", m_dCmdHoistPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
					if(((m_nEqType == PORT_TYPE_STB_L) && (m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect()==false)) ||
						((m_nEqType == PORT_TYPE_STB_R) && (m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect()==false)))
						return ERR_STB_FOUP_CHECK_NOEXIST;
					else
					{
						ADD_FDC_LOG("ERR/%d/2/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f/%f",
												ERR_HAND_PUSHSEN_NOTON,
												m_pOHTMainStatus->StatusCommon.CurNodeID,
												m_pExecuteInfo->ExecutePositionInfo.NextNode,
												m_pOHTMainStatus->StatusCommon.CurNodeOffset,
												m_pOHTMainStatus->StatusCommon.ExistFoup,
												m_pOHTMainStatus->StatusCommon.StopStationID,
												m_nTransType,
												m_nTransStep,
												m_pHWSet->m_pHand->IsPushOn(),
												m_dCmdHoistPos,
												m_pHWSet->m_pHoistAxis->GetCurrentPosition());
						return ERR_HAND_PUSHSEN_NOTON;
				}
		   }
		   }
		   else
		   {
				//정상일 경우에도 무조건 찍을것.
				m_dCmdFinal_HoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
			//    ADD_LOG("test_Unload/%06d/%.02f/%.02f/%d/%d", m_pExecuteInfo->ExecutePositionInfo.CurrStation, m_dHoist_TeachingPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_nEqType);
				ADD_LOG("[UnLoad#6]CmdPos:%f CurPos:%f CNT:%d", m_dCmdHoistPos, m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
		   }
	  }
	  break;
	 }

 //m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
	m_mccparam.nHandPush_RetryCount = nRetryCount;

 	return nError;
}



/**
 @brief   Hoist축을 Small Add로 움직이면서 해당되는 위치로 이동시키는 함수
 @author
 @date    2015.08.28
 @param   acc, dcc, v, target pose : Moving Parameter
 @return  Hoist 축 이동에 문제가 없으면 0, 문제가 있으면 해당 에러코드 리턴.
 */
int TransControl::Move_Hoist_SmallAdd(double dAcc_mmpermsec2, double dDec_mmpermsec2, double dV_mmpermsec, double dCmdHoistPos, int dTriggerIONum, double dSmallDist, bool bIsTriggerOnRisingEdge)
{
	int nError = NO_ERR;

	USER_MOVE_DS_ADDITIONAL_INFO AdditionalInfo;

	memset(&AdditionalInfo, 0x00, sizeof(USER_MOVE_DS_ADDITIONAL_INFO));

	AdditionalInfo.bUseSmallAdd              = true;
	AdditionalInfo.byTriggerIONumber         = dTriggerIONum;
	AdditionalInfo.bIsTriggerOnRisingEdge    = bIsTriggerOnRisingEdge;
	AdditionalInfo.dSmallDistance            = dSmallDist;

	nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAcc_mmpermsec2, dDec_mmpermsec2, dV_mmpermsec, dCmdHoistPos, AdditionalInfo);

	return nError;
}


/**
 @brief   Hand가 티칭 위치에 도달하여 Hand Push 센서가 On 될때까지 체크하면서 Hoist 축을 계속적으로 움직이는 함수
 @author  LSJ
 @date    2012.11.12
 @param   nRetryCount(Hand Push 센서 On 될때 까지 현재 몇번 Retry를 하고 있는지에 대한 횟수 현황)
 @param   dVel_mmpermsec(Hoist 이동 속도)
 @param   dCurPos(Hoist 현재 위치)
 @return  이동에 문제가 없으면 0, 에러가 발생되면 에러코드 리턴.
 */
int TransControl::MoveHoist_forTeachingPoint(int nRetryCount, double dVel_mmpermsec, double dCurPos)
{
    int nError = 0;
    double dAccel_mmpermsec2 = 0.0;

    SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), dCurPos + (1.0*double(nRetryCount)), dVel_mmpermsec);
	m_dCmdFinal_HoistPos = m_dCmdHoistPos;
    dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_150;
    nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);

    return nError;
}

/**
 @brief   반송물을 Release하기 전에 설비에 반송물이 잘 안착이 되었고, Track Buffer에 반송물이 잘 안착되었는지 센서 확인하는 함수
 @author  LSJ
 @date    2012.11.12
 @return  L_Req 꺼지고, UTB나 STB 센서가 Off 되면 0, 그렇지 않으면 해당 에러코드 리턴.
 */
int TransControl::Check_BeforeReleaseFoup()
{
    int nError = 0;

	DWORD dwStartTime = 0;

	if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_pEQPIO->IsUnloadREQOn() == true))
	{
		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
		if(m_pEQPIO->IsUnloadREQOn() == true)
		{
			nError = ERR_PIO_UREQ_ABNORMAL_ON_STRETCHED;
			ADD_LOG("ERR_PIO_UREQ_ABNORMAL_ON_STRETCHED_2");
			ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_UREQ_ABNORMAL_ON_STRETCHED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nTransStep);
		}
	}

	if((nError == NO_ERR) && (m_TransUse.EQPIO == true) && (m_pEQPIO->IsLoadREQOn() == true))
	{
		DWORD dwPIOWaitTime = 0;

		dwPIOWaitTime = m_pEQPIO->GetLREQOffTimeover(m_nPIOTimeoverLevel);

        if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLREQ_STTime) > dwPIOWaitTime)
        {
            nError = ERR_PIO_LREQ_ABNORMAL_ON_STRETCHIED;
			ADD_LOG("Check_BeforeReleaseFoup : ERR_PIO_LREQ_ABNORMAL_ON_STRETCHIED");
			ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/1/%d",
									ERR_PIO_LREQ_ABNORMAL_ON_STRETCHIED,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nTransStep);
        }
        else
			return nError;
	}

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)   || (OHT_NAME == OHT_NAME_STD_V85))
#else
    if(nError == NO_ERR)
	{
        if(m_nEqType == PORT_TYPE_UTB)
        {
            if((m_TransUse.UTBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect() == false))
            {
                dwStartTime = timeGetTime();
                while(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect() == false)
                {
					if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)STBUTBSensorOFFTime)
                        break;
                    Sleep(10);
                }
                if(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect() == false)
                {
                    nError = ERR_UTBSEN_NOT_OFF;
					ADD_LOG("Check_BeforeReleaseFoup : ERR_UTBSEN_NOT_OFF");
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_UTBSEN_NOT_OFF,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect());
                }
            }
        }
        else if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
        {
            switch(m_nSTBDirection)
            {
            case STB_DIR_RIGHT:
                if((m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect() == false))
                {
                    dwStartTime = timeGetTime();
                    while(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect() == false)
                    {
                        if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)STBUTBSensorOFFTime)
                            break;
                        Sleep(10);
                    }
                    if(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect() == false)
                    {
                        nError = ERR_STBSEN_NOT_OFF;
						ADD_LOG("Check_BeforeReleaseFoup : ERR_STBSEN_NOT_OFF");
						ADD_FDC_LOG("ERR/%d/1/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STBSEN_NOT_OFF,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
                    }
				}
                break;
            case STB_DIR_LEFT:
                if((m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect() == false))
                {
                    dwStartTime = timeGetTime();
                    while(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect() == false)
                    {
                        if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)STBUTBSensorOFFTime)
                            break;
                        Sleep(10);
                    }

                    if(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect() == false)
                    {
                        nError = ERR_STBSEN_NOT_OFF;
						ADD_LOG("Check_BeforeReleaseFoup : ERR_STBSEN_NOT_OFF");
						ADD_FDC_LOG("ERR/%d/2/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STBSEN_NOT_OFF,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
                    }
                }
                break;
            }
		}
	}
#endif

	if(nError == NO_ERR)
		m_nTransStep++;

	return nError;
}

/**
 @brief   이적재 명령이 완료되어 EQ PIO 연결을 끊고, Lookdown 센서를 끄고 Hoist와 Shift 축의 리미트 값을 초기화 하고 이적재 상태 플래그를 클리어 하는 함수
 @author  LSJ
 @date    2012.11.12
 @return  상태를 변화하는데 문제가 없으면 0, 그렇지 않으면 해당 에러코드 리턴.
 */
int TransControl::Check_Complete()
{
    int nError = 0;
	int nCount = 0;
	double dCurPos = fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition());
	DWORD dwStartTime = 0;

	//nError = m_pEQPIO->PIODisable(true);

//	 m_TransUse.STBSen =false; //임시
    if(nError == NO_ERR)
		nError = m_pHWSet->m_pLookdown->Disable();

    // 2 HOIST HOME, SHIFT HOME 센서 확인
	if((nError == NO_ERR) && (m_nTransType != TRANSTYPE_MOVET))
    {
		if((m_pHWSet->m_pHoistAxis->IsHoistHome() == false) && (dCurPos > HOIST_HOME_INPOS_LIMIT))// 이적재 동작 후에 Hoist 원점 아니면서 최대 위치 벗어나는 경우 soming
		{
			nError = ERR_HOISTALIGNSEN_NOT_ON;
            ADD_LOG("Check_Complete: ERR_HOISTALIGNSEN_NOT_ON");
			ADD_FDC_LOG("ERR/%d/4/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
							ERR_HOISTALIGNSEN_NOT_ON,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHoistAxis->IsHoistHome(),
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
		}

//		if(m_pHWSet->m_pShiftAxis->IsShiftHome() == false)
//		{
//			nError = ERR_SHIFT_HOMESEN_NOTON;
//			ADD_LOG("Check_Complete: ERR_SHIFT_HOMESEN_NOTON");
//			ADD_FDC_LOG("ERR/%d/4/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
//							ERR_SHIFT_HOMESEN_NOTON,
//							m_pOHTMainStatus->StatusCommon.CurNodeID,
//							m_pExecuteInfo->ExecutePositionInfo.NextNode,
//							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//							m_pOHTMainStatus->StatusCommon.ExistFoup,
//							m_pOHTMainStatus->StatusCommon.StopStationID,
//							m_nTransType,
//							m_nTransStep,
//							m_pHWSet->m_pShiftAxis->IsShiftHome());
//		}

// 2016-12-05, Add by Labeler, Rotate Position Check
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
        if((fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
		&& (fabs(ROTATE_POS_90 - m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
		&& (fabs(ROTATE_POS_180 - m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
        {
            nError = ERR_ROTATE_NOT_INPOSITION;
			ADD_LOG("[Check_Complete] E : %d(ERR_ROTATE_NOT_INPOSITION)", nError);
		}

		if(m_defualtparam->IsRotateSensor == true)
		{
			if(m_pHWSet->m_pRotate->IsRunning() == false)
			{
		   //	ADD_ERR_LOG("CheckAlways : %d :Count:%d",m_pHWSet->m_pRotate->GetCurrentPositionDegree(), nRotateCheckSensorCount);
				if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_0) < ROTATE_INPOS_CHECK_SENSOR)
				{

					if(m_pHWSet->m_pRotate->IsRotate0Sensor() == OFF)
					{
						ADD_ERR_LOG("Check_Complete Ratoate 0: %d",m_pHWSet->m_pRotate->GetCurrentPositionDegree());
						return ERR_ROTATE_0_TIMEOVER;
					}

				}
				else if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_90) <ROTATE_INPOS_CHECK_SENSOR)
				{
					if(m_pHWSet->m_pRotate->IsRotate90Sensor() == OFF)
					{
						ADD_ERR_LOG("Check_Complete Ratoate 90: %d",m_pHWSet->m_pRotate->GetCurrentPositionDegree());
						return ERR_ROTATE_90_TIMEOVER;
					}
				}
				else if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_180) <ROTATE_INPOS_CHECK_SENSOR)
				{
					if(m_pHWSet->m_pRotate->IsRotate180Sensor() == OFF)
					{
						ADD_ERR_LOG("Check_Complete Ratoate 180: %d",m_pHWSet->m_pRotate->GetCurrentPositionDegree());
						return ERR_ROTATE_180_TIMEOVER;
					}
				}
				else if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()-ROTATE_POS_270) <ROTATE_INPOS_CHECK_SENSOR)
				{
					if(m_pHWSet->m_pRotate->IsRotate270Sensor() == OFF)
					{
						return ERR_ROTATE_270_TIMEOVER;
					}
				}
			}
		}


#endif


    }

// 2016-11-28, Add by Labeler, ROHT의 경우, UTB, STB 미사용
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
#else
   // 2 STB, UTB 확인
    if((nError == NO_ERR) && (m_nTransType == TRANSTYPE_UNLOAD))
    {
        if(m_nEqType == PORT_TYPE_UTB)
        {
            if((m_TransUse.UTBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect()))
            {
				dwStartTime = timeGetTime();
                while(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect())
                {
                    if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)STBUTBSensorOFFTime)
                        break;
                    Sleep(10);
                }
                if(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect())
                {
                    nError = ERR_UTB_FOUP_CHECK_EXIST;
                    ADD_LOG("Check_Complete: ERR_UTB_FOUP_EXIST");
					ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_UTB_FOUP_EXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect());
                }
            }
        }
        else if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
		{
            switch(m_nSTBDirection)
            {
            case STB_DIR_RIGHT:
                if((m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect()))
                {
                    dwStartTime = timeGetTime();
                    while(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect())
                    {
                        if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)STBUTBSensorOFFTime)
                            break;
                        Sleep(10);
                    }
                    if(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect())
                    {
                        nError = ERR_STB_FOUP_CHECK_EXIST;
                        ADD_LOG("Check_Complete: ERR_STB_FOUP_EXIST");
						ADD_FDC_LOG("ERR/%d/3/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_EXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
                    }
                }
                break;
            case STB_DIR_LEFT:
                if((m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect()))
                {
                    dwStartTime = timeGetTime();

                    while(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect())
                    {
                        if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)STBUTBSensorOFFTime)
                            break;
                        Sleep(10);
                    }

                    if(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect())
                    {
                        nError = ERR_STB_FOUP_CHECK_EXIST;
                        ADD_LOG("Check_Complete: ERR_STB_FOUP_EXIST");
						ADD_FDC_LOG("ERR/%d/4/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_EXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
                    }
                }
                break;
            }
        }
	}
    if((nError == NO_ERR) && (m_nTransType == TRANSTYPE_LOAD))
    {
        if(m_nEqType == PORT_TYPE_UTB)
        {
            if((m_TransUse.UTBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect() == false))
            {
                dwStartTime = timeGetTime();
                while(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect() == false)
                {
                    if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)STBUTBSensorONTime)
                        break;
                    Sleep(10);
                }
                if(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect() == false)
                {
					nError = ERR_UTB_FOUP_CHECK_NOEXIST;
                    ADD_LOG("Check_Complete: ERR_UTB_FOUP_NOTEXIST");
					ADD_FDC_LOG("ERR/%d/3/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
										ERR_UTB_FOUP_NOTEXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect());
                }
            }
        }
        else if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
        {
            switch(m_nSTBDirection)
            {
            case STB_DIR_RIGHT:
                if((m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect() == false))
                {
                    dwStartTime = timeGetTime();
                    while(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect() == false)
                    {
                        if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)STBUTBSensorONTime)
                            break;
                        Sleep(10);
                    }
                    if(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect() == false)
                    {
						nError = ERR_STB_FOUP_CHECK_NOEXIST;
                        ADD_LOG("Check_Complete: ERR_STB_FOUP_NOTEXIST");
						ADD_FDC_LOG("ERR/%d/3/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_NOTEXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
                    }
                }
				break;
            case STB_DIR_LEFT:
                if((m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect() == false))
                {
                    dwStartTime = timeGetTime();
                    while(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect() == false)
                    {
                        if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)STBUTBSensorONTime)
                            break;
                        Sleep(10);
                    }
                    if(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect() == false)
                    {
                        nError = ERR_STB_FOUP_CHECK_NOEXIST;
                        ADD_LOG("Check_Complete: ERR_STB_FOUP_NOTEXIST");
						ADD_FDC_LOG("ERR/%d/4/10/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d",
										ERR_STB_FOUP_NOTEXIST,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_nSTBDirection,
										m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect(),
										m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect());
                    }
                }
                break;
            }
        }
    }
#endif

	// 2 RailSupport 동작
	if(m_nTransType != TRANSTYPE_MOVET)
	{

		// 2 원점에서 Hand Push Sensor ON이면 에러 발생
		if((nError == NO_ERR) && (m_TransUse.HandPushSen == true))
		{

			if(m_pHWSet->m_pHand->IsPushOn() == true)
			{
				nCount = 0;

				while(m_pHWSet->m_pHand->IsPushOn() == true)
				{
					if(nCount > 4)
						break;
					nCount++;
					Sleep(10);
				}

				if(m_pHWSet->m_pHand->IsPushOn() == true)
				{
					nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
					ADD_LOG("Check_Complete: ERR_HAND_PUSHSEN_ABNORMAL_ON");
					nCount = 0;
					ADD_FDC_LOG("ERR/%d/7/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
										ERR_HAND_PUSHSEN_ABNORMAL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsPushOn(),
										m_pHWSet->m_pHoistAxis->GetCurrentPosition());
				}

			}
		}
	}

    // 2 이적재 완료 후 상황에 맞지 않게 Grip Sensor가 되어 있으면 에러 발생
	if((nError == NO_ERR) && (m_TransUse.GripperSen == true))
	{

		if(m_pHWSet->m_pHand->IsOpen() == true)
		{
            if((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
            {
                while((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
                {
                    if(m_nErrCount_GripAbNormalON > 5)
                        break;
                    m_nErrCount_GripAbNormalON++;
                    Sleep(10);
                }
                if((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
                {
                    nError = ERR_GRIPPER_SEN_ON_IN_HAND_OPEN;
                    ADD_LOG("Check_Complete: ERR_GRIPPER_SEN_ON_IN_HAND_OPEN");
                    m_nErrCount_GripAbNormalON = 0;
					ADD_FDC_LOG("ERR/%d/2/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_GRIPPER_SEN_ON_IN_HAND_OPEN,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
                }
            }
		}

		else if(m_pHWSet->m_pHand->IsClose() == true)
		{
			if((m_pHWSet->m_pHand->IsRightGripOn() == false) || (m_pHWSet->m_pHand->IsLeftGripOn() == false))
			{
                while((m_pHWSet->m_pHand->IsRightGripOn() == false) || (m_pHWSet->m_pHand->IsLeftGripOn() == false))
				{
                    if(m_nErrCount_GripAbNormalOFF > 5)
                        break;
                    m_nErrCount_GripAbNormalOFF++;
                    Sleep(10);
				}
                if((m_pHWSet->m_pHand->IsRightGripOn() == false) || (m_pHWSet->m_pHand->IsLeftGripOn() == false))
                {
                    nError = ERR_GRIPPER_SEN_OFF_IN_HAND_CLOSE;
					ADD_LOG("Check_Complete: ERR_GRIPPER_SEN_OFF_IN_HAND_CLOSE");
                    m_nErrCount_GripAbNormalOFF = 0;
					ADD_FDC_LOG("ERR/%d/2/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_GRIPPER_SEN_OFF_IN_HAND_CLOSE,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
                }
            }
		}
    }

	// 2 Loading 후 : Foup 존재시==>에러 발생.

	if((nError == NO_ERR) && (m_nTransType == TRANSTYPE_LOAD) && (m_pHWSet->m_pHand->IsRightGripOn() == false) &&
		(m_pHWSet->m_pHand->IsLeftGripOn() == false) && (m_pHWSet->m_pInnerFoup->IsFoupDetect()))
		{
			DWORD dwStartTime = 0;
			static int nCount = 0;
			dwStartTime = timeGetTime();
			while(1)
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 500)
					break;



				if(m_pHWSet->m_pInnerFoup->IsFoupDetect())
                {
                    nCount++;
					if(nCount > 5)
                        break;
                }
                Sleep(10);


			}

			if(nCount > 5)
			{
				nCount = 0;
				//nError = ERR_INNER_FOUP_DETECT;
				ADD_LOG("Check_Complete: ERR_INNER_FOUP_DETECT");
			}
		}


	// Unloading 후 : Foup 존재하지 않을 시==>에러 발생.

		if((nError == NO_ERR) && (m_nTransType == TRANSTYPE_UNLOAD) && ((m_pHWSet->m_pHand->IsRightGripOn() == true) ||
			(m_pHWSet->m_pHand->IsLeftGripOn() == true)) && (m_pHWSet->m_pInnerFoup->IsFoupDetect() == false))

		{
            DWORD dwStartTime = 0;
            static int nCount = 0;
            dwStartTime = timeGetTime();
            while(1)
            {
                if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 500)
                    break;



				if(m_pHWSet->m_pInnerFoup->IsFoupDetect() == false)
                {
                    nCount++;
					if(nCount > 5)
                        break;
                }
                Sleep(10);


			}

            if(nCount > 5)
            {
                nCount = 0;
                nError = ERR_INNER_FOUP_NOT_DETECT;
                ADD_LOG("Check_Complete: ERR_INNER_FOUP_NOT_DETECT");
            }
		}

	if(nError == NO_ERR)
	{
//		if(m_pHWSet->m_pShiftAxis->IsShiftHome() == false)
//        {
//			if((fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < INPOS_LIMIT))
//            {
//				nError = ERR_SHIFT_HOMESEN_NOTON;
//				ADD_LOG("Check_Complete: ERR_SHIFT_HOMESEN_NOTON");
//				ADD_FDC_LOG("ERR/%d/5/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
//							ERR_SHIFT_HOMESEN_NOTON,
//							m_pOHTMainStatus->StatusCommon.CurNodeID,
//							m_pExecuteInfo->ExecutePositionInfo.NextNode,
//							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//							m_pOHTMainStatus->StatusCommon.ExistFoup,
//							m_pOHTMainStatus->StatusCommon.StopStationID,
//							m_nTransType,
//							m_nTransStep,
//							m_pHWSet->m_pShiftAxis->IsShiftHome());
//			}
//		}
    }

	// add by jhoun.lim 18.07.11, hand open/close sensor checking in complete step (Check_Complete)
	nCount = 0;
	if((nError == NO_ERR) && (m_nTransType == TRANSTYPE_LOAD))
	{
		//ADD_LOG("Check_Complete: add by jhoun.lim");
		//bool isOpen = m_pHWSet->m_pHand->IsOpen();
		//bool isClose = m_pHWSet->m_pHand->IsClose();
		//ADD_LOG("is open : %d, is close : %d", isOpen, isClose);


		while(m_pHWSet->m_pHand->IsOpen() == false)
		{
			if(nCount > 5)
				break;
			nCount++;
			Sleep(10);
			//ADD_LOG("num count hand open sensor off : %d", nCount);
		}
		if(nCount > 5)
        {
			nError = ERR_HAND_OPEN_SEN_OFF_IN_LOAD;
			ADD_LOG("Check_Complete: ERR_HAND_OPEN_SEN_OFF_IN_LOAD");
            nCount = 0;
			ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_OPEN_SEN_OFF_IN_LOAD,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsOpen(),
							m_pHWSet->m_pHand->IsClose());
        }

		while(m_pHWSet->m_pHand->IsClose() == true)
		{
			if(nCount > 5)
				break;
			nCount++;
			Sleep(10);
			//ADD_LOG("num count hand close sensor on : %d", nCount);
		}
		if(nCount > 5)
		{
			nError = ERR_HAND_CLOSE_SEN_ON_IN_LOAD;
			ADD_LOG("Check_Complete: ERR_HAND_CLOSE_SEN_ON_IN_LOAD");
			nCount = 0;
			ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_CLOSE_SEN_ON_IN_LOAD,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pHand->IsOpen(),
							m_pHWSet->m_pHand->IsClose());
		}
	}


    // 2 by zzang9un 2012.12.19 : Reset 대신 Set으로 변경
    m_pHWSet->m_pShiftAxis->SetNegativeSWLimit(SHIFT_NEG_LIMIT);
    m_pHWSet->m_pShiftAxis->SetPositiveSWLimit(SHIFT_POS_LIMIT);
    m_pHWSet->m_pHoistAxis->SetNegativeSWLimit(HOIST_NEG_LIMIT);
    m_pHWSet->m_pHoistAxis->SetPositiveSWLimit(HOIST_POS_LIMIT);

    // -------------------------------------------
    // by zzang9un 2012.12.19 : Neg Limit 에러로 인해 로그 추가
//    double dHoistPositive = 0.0;
//    double dHoistNegative = 0.0;
//    double dShiftPositive = 0.0;
//    double dShiftNegative = 0.0;
//
//    m_pHWSet->m_pHoistAxis->GetSWLimit(dHoistPositive, dHoistNegative);
//    m_pHWSet->m_pShiftAxis->GetSWLimit(dShiftPositive, dShiftNegative);
    // -------------------------------------------

    m_pHWSet->Sound_Off();

    if(nError == NO_ERR)
    {
        switch(m_nTransType)
        {
        case TRANSTYPE_LOAD:
			m_StatusSet.byStatus = STS_LOADED;
			HoistCnt++;
            break;
        case TRANSTYPE_UNLOAD:
			m_StatusSet.byStatus = STS_UNLOADED;
			HoistCnt++;
            break;
		case TRANSTYPE_MOVET:
			m_StatusSet.byStatus = STS_COMPLETE;
			break;

		case TRANSTYPE_SMALLADD_HOIST_HOME:
        case TRANSTYPE_MOVE_ALL_HOME:
			m_StatusSet.byStatus = STS_COMPLETE;
			break;

		case TRANSTYPE_AUTOTEACHING:
			m_StatusSet.byStatus = STS_AUTO_TAUGHT;
			break;
        }
    }
    else
    {
        m_StatusSet.uError = nError;
        m_StatusSet.byStatus = STS_ERROR;
    }

    if(m_nTransType != TRANSTYPE_AUTO_CHANGE)
	    m_StatusSet.bIsOnProcess = false;

    ///@todo 상태 변화 및 아래 내용은 ExecuteThread에서 구현 필요(Complete Flag 보고..)
    /*
     EQTYPE IsTypeEQ;

     if (m_nEqType == TYPE_EQ)
     IsTypeEQ = EQ;
     else if (m_nEqType == TYPE_UTB)
     IsTypeEQ = UTB;
     else if (m_nEqType == TYPE_STB)
     {
     switch (m_nSTBDirection)
     {
     case STBDIRE_RIGHT:
     IsTypeEQ = STB_R;
     break;
     case STBDIRE_LEFT:
     IsTypeEQ = STB_L;
	 break;
     }
     }

     switch (m_nTransType)
     {
     case TRANSTYPE_LOAD:
     m_Monitoring->dSum_HoistEncoder += fabs(m_dHoist_TeachingPos) * 2.0;
     m_Monitoring->dSum_ShiftEncoder += fabs(m_dShift_TeachingPos) * 2.0;
     m_Monitoring->dSum_RotateEncoder += fabs(m_dRotate_TeachingPos) * 2.0;

	 if (m_dCmdFinal_HoistPos != 0.0)
	 m_LocManage->Save_FinalLoc_2Axis(IsTypeEQ, m_dHoist_TeachingPos, m_dCmdFinal_HoistPos);
     pTransCmdManage->InitCmdList();
     m_IOControl->Sound_Off();
     m_OHTStatus->bPositionComplete = true;
     Update_ID_Status(STATUS_LOADED);
     break;

     case TRANSTYPE_UNLOAD:
     m_Monitoring->dSum_HoistEncoder += fabs(m_dHoist_TeachingPos) * 2.0;
	 m_Monitoring->dSum_ShiftEncoder += fabs(m_dShift_TeachingPos) * 2.0;
     m_Monitoring->dSum_RotateEncoder += fabs(m_dRotate_TeachingPos) * 2.0;

	 if (m_dCmdFinal_HoistPos != 0.0)
	 m_LocManage->Save_FinalLoc_2Axis(IsTypeEQ, m_dHoist_TeachingPos, m_dCmdFinal_HoistPos);
     // 091106 JYB
     // if(m_nEqType==TYPE_EQ) m_LocManage->Save_Unload_Hoist_Position();
     pTransCmdManage->InitCmdList();
     m_IOControl->Sound_Off();
     m_OHTStatus->bPositionComplete = true;
	 Update_ID_Status(STATUS_UNLOADED);
     break;
     }
     */
    return nError;
}

/**
 @brief   긴급 정지
 @author  ehoto
 @date    2012.11.27
 @return  EXECUTE_ACK_NO_ERR     // 명령 실행 완료 응답
 */
EXECUTE_ACK TransControl::Stop(void)
{
	m_StatusSet.byStatus = STS_ERROR;
	m_pExecuteInfo->bPopListOfCmd = false;

	if(m_pExecuteInfo->QuickCmdEStopCommander == ESTOP_BY_TP)
	{
		m_StatusSet.uError = ERR_TP_ESTOP;  // - E_Stop 호출은 OCS 도 가능하므로 내용 추가 필요
	}
	else
	{
		if(m_pExecuteInfo->QuickCmdEStopType == EXIT_ROUTE_OCS_PATH)		    	m_StatusSet.uError = ERR_OCS_ESTOP1;
		else if(m_pExecuteInfo->QuickCmdEStopType == OHT_EXIST_IN_COLLISON_AREA)	m_StatusSet.uError = ERR_OCS_ESTOP2;
		else if(m_pExecuteInfo->QuickCmdEStopType == CARRIER_EXIST_4_UNLOADING)	    m_StatusSet.uError = ERR_OCS_ESTOP3;
		else if(m_pExecuteInfo->QuickCmdEStopType == NOT_CARRIER_4_LOADING)		    m_StatusSet.uError = ERR_OCS_ESTOP4;
		else if(m_pExecuteInfo->QuickCmdEStopType == TRUNOFF_STOP)					m_StatusSet.uError = NO_ERR;
		else if(m_pExecuteInfo->QuickCmdEStopType == TRUNON)						m_StatusSet.uError = NO_ERR;
		else																		m_StatusSet.uError = ERR_OCS_ESTOP5;

		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					m_StatusSet.uError,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}

    m_nTransType = TRANSTYPE_NONE;

    m_StatusSet.bIsOnProcess = false;

    m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
	m_pHWSet->m_pShiftAxis->MoveStop(SHIFT_ESTOP_DECEL);

	TransCMDExuteFunc(false);

// 2016-12-05, Add by Labeler, ROHT의 경우 Rotate Stop 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
    m_pHWSet->m_pRotate->Stop();
#endif

    return EXECUTE_ACK_NO_ERR;
}

/**
 @brief   기동 ON
 @author  ehoto
 @date    2012.11.27
 @return  EXECUTE_ACK_NO_ERR     // 명령 실행 완료 응답
 */
EXECUTE_ACK TransControl::TurnON(void)
{
	m_StatusSet.bIsOnProcess = false;
	m_StatusSet.byStatus = STS_INIT;

	TransInit();

}

/**
 @brief   기동 Off
 @author  ehoto
 @date    2012.11.27
 @return  EXECUTE_ACK_NO_ERR     // 명령 실행 완료 응답
 */
EXECUTE_ACK TransControl::TurnOff(void)
{
	m_StatusSet.byStatus = STS_TURN_OFF;
	m_pExecuteInfo->bPopListOfCmd = false;

	if(m_pExecuteInfo->QuickCmdEStopCommander == ESTOP_BY_TP)
	{
		m_StatusSet.uError = ERR_TP_ESTOP;  // - E_Stop 호출은 OCS 도 가능하므로 내용 추가 필요
	}
	else
	{
		if(m_pExecuteInfo->QuickCmdEStopType == EXIT_ROUTE_OCS_PATH)		    	m_StatusSet.uError = ERR_OCS_ESTOP1;
		else if(m_pExecuteInfo->QuickCmdEStopType == OHT_EXIST_IN_COLLISON_AREA)	m_StatusSet.uError = ERR_OCS_ESTOP2;
		else if(m_pExecuteInfo->QuickCmdEStopType == CARRIER_EXIST_4_UNLOADING)	    m_StatusSet.uError = ERR_OCS_ESTOP3;
		else if(m_pExecuteInfo->QuickCmdEStopType == NOT_CARRIER_4_LOADING)		    m_StatusSet.uError = ERR_OCS_ESTOP4;
		else if(m_pExecuteInfo->QuickCmdEStopType == TRUNOFF_STOP)					m_StatusSet.uError = NO_ERR;
		else if(m_pExecuteInfo->QuickCmdEStopType == TRUNON)						m_StatusSet.uError = NO_ERR;
		else																		m_StatusSet.uError = ERR_OCS_ESTOP5;

		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					m_StatusSet.uError,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}

    m_nTransType = TRANSTYPE_NONE;

    m_StatusSet.bIsOnProcess = false;

	m_pHWSet->m_pHoistAxis->MoveStop(HOIST_TURNOFF_DECEL);
	m_pHWSet->m_pShiftAxis->MoveStop(SHIFT_TURNOFF_DECEL);

	m_pHWSet->m_pLeftEQPIO->PIODisable(true);
//	m_pHWSet->m_pRightEQPIO->PIODisable(true);

	TransCMDExuteFunc(false);

// 2016-12-05, Add by Labeler, ROHT의 경우 Rotate Stop 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
    m_pHWSet->m_pRotate->Stop();
#endif

    return EXECUTE_ACK_NO_ERR;
}


/**
 @brief   초기화
 @author  ehoto
 @date    2012.11.27
 @return  EXECUTE_ACK_NO_ERR     // 명령 실행 완료 응답
 */
EXECUTE_ACK TransControl::Reset(void)
{
	DWORD dwCurrTime = 0;
	int nError = NO_ERR;

    // 이적재 축 서보 오프 및 Reset
    if(m_pHWSet->m_pShiftAxis->GetError() != NO_ERR)
    {
        m_pHWSet->m_pShiftAxis->SetAMPEnable(false);
        m_pHWSet->m_pShiftAxis->Reset();
    }
    if(m_pHWSet->m_pHoistAxis->GetError() != NO_ERR)
	{
		m_pHWSet->m_pHoistAxis->SetAMPEnable(false);
		m_pHWSet->m_pHoistAxis->Reset();
    }

    // 2016-12-03, Add by Labeler, ROHT의 경우, Rotate Reset 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
    if(m_pHWSet->m_pRotate->GetError() != NO_ERR)
    {
		m_pHWSet->m_pRotate->Disable();
    }
#endif

	dwCurrTime = timeGetTime();
	while(OHTUTILITY::PreventOverflowGetTimeMs(dwCurrTime) < 200)
	{
        if((m_pHWSet->m_pShiftAxis->GetError() == NO_ERR)
        && (m_pHWSet->m_pHoistAxis->GetError() == NO_ERR)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
        && (m_pHWSet->m_pRotate->GetError() == NO_ERR)
#endif
            )

			break;
        Sleep(5);
    }

    // Hoist / Shift 서보 온
    if(m_pHWSet->m_pShiftAxis->IsAMPEnable() == false)
    {
        m_pHWSet->m_pShiftAxis->SetAMPEnable(true);
    }
    if(m_pHWSet->m_pHoistAxis->IsAMPEnable() == false)
	{
		m_pHWSet->m_pHoistAxis->SetAMPEnable(true);
    }
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
    if(m_pHWSet->m_pRotate->IsAMPEnable() == false)
    {
        m_pHWSet->m_pRotate->Enable();
    }
#endif
#if( (OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
		m_pHWSet->m_pHoistAxis->HoistExternalBrakeReleaseOnOFF(true);
#endif

    dwCurrTime = timeGetTime();
	while(OHTUTILITY::PreventOverflowGetTimeMs(dwCurrTime) < 200)
	{
        if((m_pHWSet->m_pShiftAxis->IsAMPEnable() == true)
        && (m_pHWSet->m_pHoistAxis->IsAMPEnable() == true)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
        && (m_pHWSet->m_pRotate->IsAMPEnable() == true)
#endif
        )
            break;
        Sleep(5);
    }

    TransInit();

    // by zzang9un 2012.12.19 : 2,3축 SW Limit 설정
    m_pHWSet->m_pShiftAxis->SetNegativeSWLimit(SHIFT_NEG_LIMIT);
    m_pHWSet->m_pShiftAxis->SetPositiveSWLimit(SHIFT_POS_LIMIT);
    m_pHWSet->m_pHoistAxis->SetNegativeSWLimit(HOIST_NEG_LIMIT);
    m_pHWSet->m_pHoistAxis->SetPositiveSWLimit(HOIST_POS_LIMIT);

	// By elibeli2 2013.08.21 PIO Disable
	m_pHWSet->m_pLeftEQPIO->PIODisable(true);
//	m_pHWSet->m_pRightEQPIO->PIODisable(true);

	TransCMDExuteFunc(false);

	m_StatusSet.uError = nError;

    return EXECUTE_ACK_NO_ERR;
}

/**
 @brief   명령 삽입
 @author  ehoto
 @date    2012.11.27
 @return  EXECUTE_ACK_NO_ERR: 성공시  /  EXECUTE_ACK_BUSY: 바쁠 경우  /  EXECUTE_ACK_NOT_AVAILABLE_CMD : 불가능 경우
 */
EXECUTE_ACK TransControl::InsertGeneralCmd(void *pTransInfo)
{
	ADD_LOG("test1");
    EXECUTE_TRANS_INFO *pTmpTransInfo = (EXECUTE_TRANS_INFO*)pTransInfo;
	EXECUTE_TRANS_INFO TmpTransInfo;
    EXECUTE_ACK TmpExecuteAck;

    memset(&TmpTransInfo, 0x00, sizeof(TmpTransInfo));

    CARRIER_OFFSET CarrierOffset;

    CarrierOffset = m_pParamSet->CarrierOffset[0];

    /* // [BEGIN] by zzang9un 2013.08.15 : 기존 대입문
     TmpTransInfo.TransType      = pTmpTransInfo->TransType;
     TmpTransInfo.TransTagID     = pTmpTransInfo->TransTagID;
     TmpTransInfo.PortType       = pTmpTransInfo->PortType;
     TmpTransInfo.STBDir         = pTmpTransInfo->STBDir;
     TmpTransInfo.Hoist          = pTmpTransInfo->Hoist + CarrierOffset.dHosit;
     TmpTransInfo.Shift          = pTmpTransInfo->Shift + CarrierOffset.dShift;
     TmpTransInfo.Rotate         = pTmpTransInfo->Rotate;
     TmpTransInfo.LookDown       = pTmpTransInfo->LookDown;
     TmpTransInfo.TransSound     = pTmpTransInfo->TransSound;
     TmpTransInfo.Oscillation    = pTmpTransInfo->Oscillation;
     TmpTransInfo.HoistLevel     = pTmpTransInfo->HoistLevel;
     TmpTransInfo.SlideLevel     = pTmpTransInfo->SlideLevel;
     TmpTransInfo.PIODirection   = pTmpTransInfo->PIODirection;
     TmpTransInfo.PIOTimeLevel   = pTmpTransInfo->PIOTimeLevel;
     TmpTransInfo.HandDetectEQ   = pTmpTransInfo->HandDetectEQ;
     */ // [END] by zzang9un 2013.08.15 : 기존 대입문

    // by zzang9un 2013.08.15 : Trans Command 복사
    memcpy(&TmpTransInfo, pTmpTransInfo, sizeof(EXECUTE_TRANS_INFO));

    // by zzang9un 2013.08.15 : CarrierOffset 처리
	// by zzang9un 2013.09.10 : 단위 변경 적용(1/1000m -> 1/10000m)
    TmpTransInfo.TransCmd.Tposition_Hoist += (int)(CarrierOffset.dHosit * 10.0);
    TmpTransInfo.TransCmd.Tposition_Shift += (int)(CarrierOffset.dShift * 10.0);

    if(m_StatusSet.bIsOnProcess == true)
	{
        // by zzang9un 2013.03.25 : 주석 추가 for debug
		ADD_LOG("[InsertGenCmd] EXECUTE_ACK_BUSY");
        TmpExecuteAck = EXECUTE_ACK_BUSY;
    }
    else
    {
		RESET_LOG;

		TransInit();
		ADD_LOG("test2");
		TmpExecuteAck = SetGeneralCmd(TmpTransInfo);


		if(TmpExecuteAck == EXECUTE_ACK_NO_ERR)
        {
			delete pTmpTransInfo;

//            double dHoistPositive = 0.0;
//            double dHoistNegative = 0.0;
//            double dShiftPositive = 0.0;
//			double dShiftNegative = 0.0;
//
//			m_pHWSet->m_pHoistAxis->GetSWLimit(dHoistPositive, dHoistNegative);
//			m_pHWSet->m_pShiftAxis->GetSWLimit(dShiftPositive, dShiftNegative);

			m_pExecuteInfo->AccumulationData.dHoistEncoder += (TmpTransInfo.TransCmd.Tposition_Hoist*2);
			m_pExecuteInfo->AccumulationData.dShiftEncoder += (TmpTransInfo.TransCmd.Tposition_Shift*2);

//            ADD_LOG("[H_E:%d, S_E:%d], [H_P:%.1lf, H_N:%.1lf, S_P:%.1lf, S_N:%.1lf], [TransType:%d, H:%d, S:%d, AR:%d] level[H:%d, S:%d]",
//				m_pHWSet->m_pHoistAxis->GetError(),	m_pHWSet->m_pShiftAxis->GetError(), dHoistPositive, dHoistNegative, dShiftPositive,
//				dShiftNegative, TmpTransInfo.TransType,	TmpTransInfo.TransCmd.Tposition_Hoist, TmpTransInfo.TransCmd.Tposition_Shift,
//				m_bPIOAutoRecovery, TmpTransInfo.TransCmd.HoistSpeedLevel, TmpTransInfo.TransCmd.ShiftSpeedLevel);

			ADD_LOG("[H_E:%d, S_E:%d], [TransType:%d, H:%d, S:%d, AR:%d] level[H:%d, S:%d]",
				m_pHWSet->m_pHoistAxis->GetError(),	m_pHWSet->m_pShiftAxis->GetError(), TmpTransInfo.TransType,	TmpTransInfo.TransCmd.Tposition_Hoist, TmpTransInfo.TransCmd.Tposition_Shift,
				m_bPIOAutoRecovery, TmpTransInfo.TransCmd.HoistSpeedLevel, TmpTransInfo.TransCmd.ShiftSpeedLevel);
		}
        m_StatusSet.bIsOnProcess = true;
    }

    return TmpExecuteAck;

}

/**
 @brief   명령 반복 실행
 @author  ehoto
 @date    2012.11.27
 @return  반복 수행
 */
UINT TransControl::ExecuteJob(void)
{
	int nError = NO_ERR;

    // ================================================================================
	// 1 0. 이상 혹은 완료 처리
	if((m_StatusSet.uError != NO_ERR)) // @ 불필요한 호출 응답
	{
		ADD_LOG("0. 이상 혹은 완료 처리 [%d]", m_StatusSet.uError);
		m_StatusSet.byStatus = STS_ERROR;
        Stop();
    }
    // ================================================================================
    if((m_StatusSet.uError == NO_ERR) && (m_StatusSet.bIsOnProcess == true))
	{

		switch(m_nTransType)
        {
		case TRANSTYPE_LOAD:
            if(m_StatusSet.byStatus == STS_LOADING)
			{

                nError = Cmd_Load();
            }
            break;

        case TRANSTYPE_UNLOAD:
            if(m_StatusSet.byStatus == STS_UNLOADING)
			{
				nError = Cmd_Unload();
            }
            break;

        case TRANSTYPE_MOVET:
            nError = Cmd_MoveTP();
            break;

		case TRANSTYPE_SCAN:
			break;

		case TRANSTYPE_AUTOSET_HOME:
			nError = Cmd_AutoSetHome(m_nTransStep);
			break;

		case TRANSTYPE_AUTOTEACHING:
			if(m_StatusSet.byStatus == STS_AUTO_TEACHING)
			{
				nError =  Cmd_AutoTeaching();
			}
			break;

		case TRANSTYPE_AUTOTEACHING_NULLTYPE:
			if(m_StatusSet.byStatus == STS_AUTO_TEACHING_NULLTYPE)
			{
				nError =  Cmd_AutoTeaching_NullType();
			}
			break;

        // 2017-01-01, Add by Labeler
        // Hoist, Shift, Rotate, Hand, Shutter 모두 Home 이동 기능 추가
        case TRANSTYPE_MOVE_ALL_HOME:
            nError = Cmd_MoveAllHome();
            break;
            
		case TRANSTYPE_SMALLADD_HOIST_HOME:
#ifdef OHT_HIGH_RAIL_CHECK_HOME
			nError = Cmd_SmalladdHoistHome();
#else
            nError = ERR_TAG_CMDTYPE;
#endif
		break;

		case TRANSTYPE_AUTO_CHANGE:
			nError = Cmd_AutoMode_Change();
			break;
		}
    }

	if(nError != NO_ERR)
	{
        m_StatusSet.uError = nError;
	}

    return m_StatusSet.uError;
}

/**
 @brief   대기 중 이상 상태 확인 : 외부 사용
 @author  ehoto
 @date    2013.02.07
 @return  문제 있을 경우 에러 반환, 그 외에는 NO_ERR(0) 반환
 */
UINT TransControl::CheckAbnormalOnWait(void)
{
	DWORD dwCurrTime = timeGetTime();
	static DWORD dwLastTimeOfHandPushOnState = timeGetTime();


	// [ AXIS ] //
	bool bHoistHome = m_pHWSet->m_pHoistAxis->IsHoistHome();
	static bool bPreHoistHome = m_pHWSet->m_pHoistAxis->IsHoistHome();
	bool bShiftHome = m_pHWSet->m_pShiftAxis->IsShiftHome();
	static bool bPreShiftHome = m_pHWSet->m_pShiftAxis->IsShiftHome();
	bool bHoistRunning = m_pHWSet->m_pHoistAxis->IsRunning();
	bool bShiftRunning = m_pHWSet->m_pShiftAxis->IsRunning();
	bool bHoistEnabled = m_pHWSet->m_pHoistAxis->IsAMPEnable();
	bool bShiftEnabled = m_pHWSet->m_pShiftAxis->IsAMPEnable();
	double dHoistPosition = m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition();
	double dShiftPosition = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
	double dHoistSpeed = m_pHWSet->m_pHoistAxis->GetCurrentVelocity();
	double dShiftSpeed = m_pHWSet->m_pShiftAxis->GetCurrentVelocity();

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
	bool bRotateRunning = m_pHWSet->m_pRotate->IsRunning();
    bool bRotateEnabled = m_pHWSet->m_pRotate->IsAMPEnable();
    double dRotatePosition = m_pHWSet->m_pRotate->GetCurrentPositionDegree();
    double dRotateSpeed = m_pHWSet->m_pRotate->GetCurrentVelocity();
#endif

	// [ HAND ] //

	bool bHandOpen = m_pHWSet->m_pHand->IsOpen();
	bool bHandClose = m_pHWSet->m_pHand->IsClose();
	bool bPushOn = m_pHWSet->m_pHand->IsPushOn();

	bool bRightGripOn = m_pHWSet->m_pHand->IsRightGripOn();
	bool bLeftGripOn = m_pHWSet->m_pHand->IsLeftGripOn();
	bool bHandGoSig = m_pHWSet->m_pHand->IsGoSigOn();
	bool bHandAlarm = m_pHWSet->m_pHand->IsAlarm();

	// [ FOUP ] //
	bool bInnerFoupDetect = m_pHWSet->m_pInnerFoup->IsFoupDetect();
	static bool bPreInnerFoupDetect = m_pHWSet->m_pInnerFoup->IsFoupDetect();



	// [ SHUTTER ] //
	bool bShutterFrontOpen = m_pHWSet->m_pShutter->IsOpen(FRONT);
	bool bShutterFrontClose = m_pHWSet->m_pShutter->IsClose(FRONT);
	bool bShutterRearOpen = m_pHWSet->m_pShutter->IsOpen(REAR);
	bool bShutterRearClose = m_pHWSet->m_pShutter->IsClose(REAR);
	bool bShutterBothOpen = bShutterFrontOpen & bShutterRearOpen;
	bool bShutterBothClose = bShutterFrontClose & bShutterRearClose;



    // [Oscilation] //
	bool bOscilation = m_pHWSet->m_pOcsillation->IsDetect();

//	static int nCountOfRailSupportHome	= 0;
	static int nCountOfHoistAlign	= 0;
	static int nCountOfShiftHome	= 0;
	static int nCountOfHandLimitAllOn	= 0;
	static int nCountOfHandLimitAllOff	= 0;
	static int nCountOfHandCastExist	= 0;
	static int nCountOfHandCastNotExist	= 0;
	static int nCountOfHandSenAllOff	= 0;
	static int nCountOfHandGoSigOff		= 0;
	static int nCountOfShutterDiff		= 0;
	static int nCountOfShutterNotClose	= 0;
	static int nCountOfShutterNotOpen	= 0;
	static int nCountOfHandGrip			= 0;

	static int nCountOfShifPos			= 0;

	static int nCountOfShifCount		= 0;
	static int nCountOfHoistCount 		= 0;





#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
	static int nCountOfRotatePos    = 0;
#endif
	static UINT nFDCCurNode = 0;

	if(m_pOHTMainStatus->StatusCommon.CurNodeID != nFDCCurNode)
	{
		nFDCCurNode = m_pOHTMainStatus->StatusCommon.CurNodeID;
        m_bFDCHandPushFlag = true;
	}


	// 2 [ HOIST ]
    // Hoist 가 원점이 아닐 경우 에러 발생
	// Hoist 가 Limit를 벗어날 경우 에러 발생: 30mm
    // Hoist 가 서보 오프 되었을 때 에러 발생
	// Hoist 가 움직일 경우 에러 발생


	if(bHoistHome != bPreHoistHome)
	{
		if(bPreHoistHome ==ON)
		{
			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_HO);
		}
		//사전 로그
	}

	bPreHoistHome = bHoistHome;

	if((bHoistHome == false) && (fabs(dHoistPosition) > HOIST_HOME_INPOS_LIMIT))   //거리값으로 변경 20171221
	{
		nCountOfHoistAlign++;
		m_nTransHoistOriginCount++;

		if(nCountOfHoistAlign > 65)
		//if(nCountOfHoistAlign > 200)// 16*200mm -> Hoist Home Sensor Chattering 길게 추가 soming
		{
			nCountOfHoistAlign = 0;
			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HOISTALIGNSEN_NOT_ON");
			ADD_FDC_LOG("ERR/%d/5/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
							ERR_HOISTALIGNSEN_NOT_ON,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHoistAxis->IsHoistHome(),
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
			return ERR_HOISTALIGNSEN_NOT_ON;
		}
	}
	else
	{
		if(bHoistHome == false)
			m_nTransHoistOriginCount++;

		nCountOfHoistAlign = 0;
	}
	if(bHoistEnabled == false)
	{

		ADD_ERR_LOG("CheckAbnormalOnWait : ERR_AXIS2_SERVO_OFFED");
		ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
							ERR_AXIS2_SERVO_OFFED,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHoistAxis->IsAMPEnable(),
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
		return ERR_AXIS2_SERVO_OFFED;
	}



    if(bHoistRunning == true)
    {
		ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HOIST_NOTORIGIN");
		ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/0/0/%f/%f",
						ERR_HOIST_NOTORIGIN,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()),
						HOIST_ORIGIN_LIMIT);
        return ERR_HOIST_NOTORIGIN;
    }
	if((dHoistPosition > HOIST_POSITIVE_LIMIT_ON_WAIT) || (dHoistPosition < HOIST_NEGATIVE_LIMIT_ON_WAIT))
	{
		nCountOfHoistCount++;
		if(nCountOfHoistCount > 3)
		{
			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HOIST_ABNORMAL_MOVING %f", dHoistPosition);
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%f/%f",
							ERR_HOIST_ABNORMAL_MOVING,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							dHoistPosition,
							dHoistSpeed);
			return ERR_HOIST_ABNORMAL_MOVING;
		}
	}
	else
		nCountOfHoistCount = 0;


	// 2 [ SHIFT ]
    // Shift 가 원점이 아닐 경우 에러 발생
    // Shift 가 Limit를 벗어날 경우 에러 발생
    // Shift 가 서보 오프 되었을 때 에러 발생
	// Shift 가 움직일 경우 에러 발생
//	if(bShiftHome == false)
//	{
//		if( bPreShiftHome!= bShiftHome)
//		{
//			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_SH);   //사전 로그 추가
//		}
//
//		nCountOfShiftHome++;
//		if(nCountOfShiftHome > 40)
//		{
//			nCountOfShiftHome = 0;
//			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_SHIFT_HOMESEN_NOTON");
//			ADD_FDC_LOG("ERR/%d/6/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
//						ERR_SHIFT_HOMESEN_NOTON,
//						m_pOHTMainStatus->StatusCommon.CurNodeID,
//						m_pExecuteInfo->ExecutePositionInfo.NextNode,
//						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//						m_pOHTMainStatus->StatusCommon.ExistFoup,
//						m_pOHTMainStatus->StatusCommon.StopStationID,
//						m_nTransType,
//						m_nTransStep,
//						bShiftHome);
//			return ERR_SHIFT_HOMESEN_NOTON;
//		}
//	}
//	else
//		nCountOfShiftHome = 0;

	bPreShiftHome = bShiftHome;

//	if(bShiftEnabled == false)
//	{
//
//		ADD_ERR_LOG("CheckAbnormalOnWait : ERR_AXIS3_SERVO_OFFED");
//		ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
//							ERR_AXIS3_SERVO_OFFED,
//							m_pOHTMainStatus->StatusCommon.CurNodeID,
//							m_pExecuteInfo->ExecutePositionInfo.NextNode,
//							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//							m_pOHTMainStatus->StatusCommon.ExistFoup,
//							m_pOHTMainStatus->StatusCommon.StopStationID,
//							m_nTransType,
//							m_nTransStep,
//							m_pHWSet->m_pShiftAxis->IsAMPEnable(),
//							m_pHWSet->m_pShiftAxis->GetCurrentPosition());
//		return ERR_AXIS3_SERVO_OFFED;
//	}

	if(bShiftRunning == true)
	{
		nCountOfShifPos++;
		if(nCountOfShifPos > 15)
		{
			nCountOfShifPos = 0;
			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_SHIFT_NOTORIGIN");
			ADD_FDC_LOG("ERR/%d/1/6/%06d/%06d/%d/%d/%06d/%d",
						ERR_SHIFT_NOTORIGIN,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						bShiftRunning);
			return ERR_SHIFT_NOTORIGIN;
		}

	}
	else
		nCountOfShifPos = 0;

	if((dShiftPosition > SHIFT_POSITIVE_LIMIT_ON_WAIT) || (dShiftPosition < SHIFT_NEGATIVE_LIMIT_ON_WAIT))
	{
		nCountOfShifCount++;
		if(nCountOfShifCount > 3)
		{
			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_SHIFT_ABNORMAL_MOVING %f",dShiftPosition);
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%f/%f",
							ERR_SHIFT_ABNORMAL_MOVING,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							dShiftPosition,
							dShiftSpeed);
			return ERR_SHIFT_ABNORMAL_MOVING;
		}
	}
	else
		nCountOfShifCount = 0;




/*2018.10.12 Rotate Position Log 추가 #Jrespect.im
TransControl::CheckAbnormalOnWait=>ExecuteThread  sleep(5)
Rotate::IsRunning=>StatusCheckThread              sleep(40)
Thread 주기가 다름 Chattering 확인 필요  5->8로 변경
*/
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
	if(bRotateEnabled == false)
    {
		ADD_ERR_LOG("CheckAbnormalOnWait : ERR_AXIS4_SERVO_OFFED");
		return ERR_AXIS4_SERVO_OFFED;
	}

    if(bRotateRunning == true)
	{
		nCountOfRotatePos++;
		ADD_ERR_LOG("CheckAbnormalOnWait : Rotate Position : %f", dRotatePosition);
		if(nCountOfRotatePos > 15)	//2018.11.13 : Code Review 협의 8 -> 15
		{
			nCountOfRotatePos = 0;
			ADD_ERR_LOG("CheckAbnormalOnWait : ROTATE_MOTOR_RUNNING");
			return ERR_ROTATE_ALARM;
		}
	}
	else
		nCountOfRotatePos = 0;

	if( (fabs(dRotatePosition) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
	&&  (fabs(ROTATE_POS_90 - dRotatePosition) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
	&&  (fabs(ROTATE_POS_180 - dRotatePosition) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
    {
		ADD_ERR_LOG("CheckAbnormalOnWait : ERR_ROTATE_NOT_0"); //AbnormalError.log에 남음
		ADD_ERR_LOG("CheckAbnormalOnWait : Rotate Position : %f", dRotatePosition);
		ADD_LOG("CheckAbnormalOnWait : ERR_ROTATE_NOT_0");
		return ERR_ROTATE_NOT_0;
    }
#endif

    // 2 [ HAND ]
    // Hand 열림 닫힘 센서가 둘다 On 이면 에러 발생
    // Hand 열림 닫힘 센서가 둘다 Off 이면 에러 발생
    // FOUP 있을 때 닫힘 센서 Off 이면 에러 발생
    // FOUP 있을 때 열림 센서 On 이면 에러 발생
    // FOUP 있을 때 재하 센서 Off 이면 에러 발생
    // FOUP 없을 때 닫힘 센서 On 이면 에러 발생
    // FOUP 없을 때 열림 센서 Off 이면 에러 발생
    // FOUP 없을 때 재하 센서 On 이면 에러 발생
    // PUSH 센서 ON 이면 에러 발생
    // Board 전원 Off 시 에러 발생  // ( 2초 이상 ? )

	if(bHandOpen == true && bHandClose == true)
	{
		m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_HS);   //사전 로그 추가
		nCountOfHandLimitAllOn++;
		if(nCountOfHandLimitAllOn > 10)
		{
			nCountOfHandLimitAllOn = 0;
			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_LIMIT_SEN_ALL_ON");
			ADD_FDC_LOG("ERR/%d/2/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_HAND_LIMIT_SEN_ALL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
			return ERR_HAND_LIMIT_SEN_ALL_ON;
		}
	}
	else
		nCountOfHandLimitAllOn = 0;


	if(bHandOpen == false && bHandClose == false)
	{
		nCountOfHandLimitAllOff++;
		if((nCountOfHandLimitAllOff > 10)&&(bInnerFoupDetect))
		{
            if(m_bSlipringFlag)
            {
                nCountOfHandLimitAllOff = 0;
                m_bSlipringFlag = false;
                ADD_ERR_LOG("CheckAbnormalOnWait : ERR_SEN_ABNORMAL_HAND");
                ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_LIMIT_SEN_ALL_OFF");
				ADD_FDC_LOG("ERR/%d/4/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_SEN_ABNORMAL_HAND,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
				return ERR_SEN_ABNORMAL_HAND;
            }
            else
            {
                nCountOfHandLimitAllOff = 0;
                ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_LIMIT_SEN_ALL_OFF");
				ADD_FDC_LOG("ERR/%d/2/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_HAND_LIMIT_SEN_ALL_OFF,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
			    return ERR_HAND_LIMIT_SEN_ALL_OFF;
            }
   		}
        if((nCountOfHandLimitAllOff > 50)&&(!bInnerFoupDetect))
        {
            if(m_bSlipringFlag)
            {
                nCountOfHandLimitAllOff = 0;
				m_bSlipringFlag = false;
				ADD_ERR_LOG("CheckAbnormalOnWait : ERR_SEN_ABNORMAL_HAND");
                ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_LIMIT_SEN_ALL_OFF");
				ADD_FDC_LOG("ERR/%d/4/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_SEN_ABNORMAL_HAND,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
                return ERR_SEN_ABNORMAL_HAND;
            }
            else
            {
                nCountOfHandLimitAllOff = 0;
                ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_LIMIT_SEN_ALL_OFF");
				ADD_FDC_LOG("ERR/%d/2/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_HAND_LIMIT_SEN_ALL_OFF,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
			    return ERR_HAND_LIMIT_SEN_ALL_OFF;
            }

        }

	}
	else
	{
		nCountOfHandLimitAllOff = 0;
	}


	if(bInnerFoupDetect == true)
	{ // FOUP 있을 때
		
		nCountOfHandCastNotExist = 0;
		if ((bHandClose == true) && (bHandOpen == false) && (bRightGripOn == true) && (bLeftGripOn == true))
	    {
            nCountOfHandCastExist = 0;
        }		
		else
		{
			if(bInnerFoupDetect !=bPreInnerFoupDetect)
			{
            	m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_IF);
			}
			nCountOfHandCastExist++;
        }	

        if ((bHandClose == true) || (bRightGripOn == true) || (bLeftGripOn == true))
        {
            if(nCountOfHandCastExist > 40)   //Foup 있을 때 HandCastExist>40 일 때 Hand Cast에러
            {
                nCountOfHandCastExist = 0;
                ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_CAST_EXIST");
				ADD_FDC_LOG("ERR/%d/3/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
											ERR_HAND_CAST_EXIST,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pExecuteInfo->ExecutePositionInfo.NextNode,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_pOHTMainStatus->StatusCommon.ExistFoup,
											m_pOHTMainStatus->StatusCommon.StopStationID,
											m_pHWSet->m_pHand->IsOpen(),
											m_pHWSet->m_pHand->IsClose(),
											m_pHWSet->m_pHand->IsLeftGripOn(),
											m_pHWSet->m_pHand->IsRightGripOn());
                return ERR_HAND_CAST_EXIST;
            }
        }
        else
        {
            if(nCountOfHandCastExist > 200)   //Foup이 없을 때 Foupdetect>200 일 때 Foup Detect에러
		    {
			    nCountOfHandCastExist = 0;
				ADD_ERR_LOG("CheckAbnormalOnWait : ERR_INNER_FOUP_DETECT");
				//return ERR_INNER_FOUP_DETECT;
		    }
        }

	
	}
    else
	{ // FOUP 없을 때
		nCountOfHandCastExist = 0;
		if((bHandClose == true) || (bHandOpen == false) || (bRightGripOn == true) || (bLeftGripOn == true))
		{
			//사전 로그
			if(bInnerFoupDetect !=bPreInnerFoupDetect)
			{
				m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_IF_B);
			}

			nCountOfHandCastNotExist++;
		}
		else
			nCountOfHandCastNotExist = 0;

		if(nCountOfHandCastNotExist > 20)
		{
            if(m_bSlipringFlag)
            {
                nCountOfHandCastNotExist = 0;
                m_bSlipringFlag = false;
				ADD_ERR_LOG("CheckAbnormalOnWait : ERR_SEN_ABNORMAL_HAND");
                ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_CAST_NOTEXIST");
				ADD_FDC_LOG("ERR/%d/6/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_SEN_ABNORMAL_HAND,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
                return ERR_SEN_ABNORMAL_HAND;
            }
            else
            {
			    nCountOfHandCastNotExist = 0;
		    	ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_CAST_NOTEXIST");
				ADD_FDC_LOG("ERR/%d/3/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
											ERR_HAND_CAST_NOTEXIST,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pExecuteInfo->ExecutePositionInfo.NextNode,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_pOHTMainStatus->StatusCommon.ExistFoup,
											m_pOHTMainStatus->StatusCommon.StopStationID,
											m_pHWSet->m_pHand->IsOpen(),
											m_pHWSet->m_pHand->IsClose(),
											m_pHWSet->m_pHand->IsLeftGripOn(),
											m_pHWSet->m_pHand->IsRightGripOn());
		    	return ERR_HAND_CAST_NOTEXIST;
            }

		}
	}

	bPreInnerFoupDetect = bInnerFoupDetect;


	if(bPushOn == true)
	{

		if(OHTUTILITY::PreventOverflowGetTimeMs(dwLastTimeOfHandPushOnState) > 1500)

		{
			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_PUSHSEN_ABNORMAL_ON");
			//return ERR_HAND_PUSHSEN_ABNORMAL_ON;
			if(m_bFDCHandPushFlag == true)
			{
				SEND_WARN("WARN/50/1/6/%06d/%06d/%d/%d/%06d/%d",
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						bPushOn);
				m_bFDCHandPushFlag = false;
			}
		}

    }
    else
    {
        dwLastTimeOfHandPushOnState = dwCurrTime;
	}

	if(bHandGoSig == false && bHandAlarm == false && bHandClose == false && bHandOpen == false && bRightGripOn == false &&
		bLeftGripOn == false)
	{
		nCountOfHandSenAllOff++;
		if(nCountOfHandSenAllOff > 50)
		{
			nCountOfHandSenAllOff = 0;
			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_SENSOR_OFFERR");
			ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
						ERR_HAND_SENSOR_OFFERR,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						bRightGripOn,
						bLeftGripOn);
			return ERR_HAND_SENSOR_OFFERR;
		}
	}
	else
		nCountOfHandSenAllOff = 0;

	if(bHandGoSig == false)
	{
		m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_HS_C);   //사전 로그 추가

		nCountOfHandGoSigOff++;
		if(nCountOfHandGoSigOff > 50)
		{
			nCountOfHandGoSigOff = 0;
			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HANDPIO_GOSIG_NOT_ON");
			ADD_FDC_LOG("ERR/%d/1/6/%06d/%06d/%d/%d/%06d/%d",
						ERR_HANDPIO_GOSIG_NOT_ON,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						bHandGoSig);
			return ERR_HANDPIO_GOSIG_NOT_ON;
		}
	}
	else
		nCountOfHandGoSigOff = 0;


	if(!(m_defualtparam->PreShutter))          //셔터 사전동작 미사용
	{

	// 2 [ SHUTTER ]
	// SHUTTER 열림 닫힘 센서가 둘다 On 이면 에러 발생
	// SHUTTER 열림 닫힘 센서가 둘다 Off 이면 에러 발생
	// FOUP 있을 때 닫혀 있지 않으면 에러 발생
	// FOUP 없을 때 열려 있지 않으면 에러 발생
	if(bShutterFrontOpen == bShutterFrontClose || bShutterRearOpen == bShutterRearClose)
	{
		nCountOfShutterDiff++;
		if(nCountOfShutterDiff > 15)
		{
			nCountOfShutterDiff = 0;
			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_SHUTTER_DIFFSTATUS");
			ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SHUTTER_DIFFSTATUS,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							bShutterFrontOpen,
							bShutterFrontClose,
							bShutterRearOpen,
							bShutterRearClose);
			return ERR_SHUTTER_DIFFSTATUS;
		}
	}
	else
		nCountOfShutterDiff = 0;


	if(bInnerFoupDetect == true && bShutterBothClose == false)
	{
		nCountOfShutterNotClose++;
		if(nCountOfShutterNotClose > 15)
		{
			nCountOfShutterNotClose = 0;
			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_SHUTTER_NOT_CLOSE");
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
						ERR_SHUTTER_NOT_CLOSE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						bShutterFrontOpen,
						bShutterFrontClose,
						bShutterRearOpen,
						bShutterRearClose);
			return ERR_SHUTTER_NOT_CLOSE;
		}
	}
	else
		nCountOfShutterNotClose = 0;

	if(bInnerFoupDetect == false && bShutterBothOpen == false)
	{
		nCountOfShutterNotOpen++;
		if(nCountOfShutterNotOpen > 30)
		{
			nCountOfShutterNotOpen = 0;
			ADD_ERR_LOG("CheckAbnormalOnWait : ERR_SHUTTER_NOTORIGIN");
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
						ERR_SHUTTER_NOTORIGIN,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						bShutterFrontOpen,
						bShutterFrontClose,
						bShutterRearOpen,
						bShutterRearClose);
			return ERR_SHUTTER_NOTORIGIN;
		}
	}
	else
		nCountOfShutterNotOpen = 0;



	}

	// 1 FOUP 재하 센서 상태 확인

	// 2 Hand FOUP 존재 시 Hand 닫혀 있지 않을 때
	if(bRightGripOn == ON && bLeftGripOn == ON)
	{
		if((bHandClose == OFF) || (bHandOpen == ON))
		{
			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_HS_B);   //사전 로그 추가

			nCountOfHandGrip++;
			if(nCountOfHandGrip > 20)
			{
				nCountOfHandGrip = 0;
				ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_CAST_EXIST");
				ADD_FDC_LOG("ERR/%d/4/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
											ERR_HAND_CAST_EXIST,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pExecuteInfo->ExecutePositionInfo.NextNode,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_pOHTMainStatus->StatusCommon.ExistFoup,
											m_pOHTMainStatus->StatusCommon.StopStationID,
											m_pHWSet->m_pHand->IsOpen(),
											m_pHWSet->m_pHand->IsClose(),
											m_pHWSet->m_pHand->IsLeftGripOn(),
											m_pHWSet->m_pHand->IsRightGripOn());
				return ERR_HAND_CAST_EXIST;
			}
		}
		else
			nCountOfHandGrip = 0;

	}
	else
		nCountOfHandGrip = 0;




	// 20131009 By LSJ : 함수 내에 중복되는 부분 있어서 제거함.
	// 2 Shift 원점 일 때 Shift Home 센서안 들어오면 에러
	/*
	if(fabs(dShiftPosition) < DIST_SHIFT_HOME_MARGIN && bShiftHome == OFF)
	{
		ADD_ERR_LOG("CheckAbnormalOnWait : ERR_SHIFT_HOMESEN_NOTON");
		return ERR_SHIFT_HOMESEN_NOTON;
	}

	// 2     Foup 이(Inner FOUP 센서/SHIFT,Hoist원점) 있을(없을) 때
	// 2 Right/Left 둘다 ON(OFF) 아닌 경우
	if(bHoistHome == ON && bShiftHome == ON)
	{
		if(bInnerFoupDetect == ON)
		{
			if(bRightGripOn == OFF || bLeftGripOn == OFF)
			{
				ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_CAST_EXIST");
				return ERR_HAND_CAST_EXIST;
			}
		}
		else
		{
			if(bRightGripOn == ON || bLeftGripOn == ON)
			{
				ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HAND_CAST_NOTEXIST");
				return ERR_HAND_CAST_NOTEXIST;// + 1000 * 5;
			}
		}
	}

	// 2 Hoist 원점 일 때 Hoist Home 센서안 들어오면 에러
	if(fabs(dHoistPosition) < DIST_HOIST_HOME_MARGIN && bHoistHome == OFF)
	{
		ADD_ERR_LOG("CheckAbnormalOnWait : ERR_HOISTALIGNSEN_NOT_ON");
		return ERR_HOISTALIGNSEN_NOT_ON;
	}
	// 2 Shift 원점 일 때 Shift Home 센서안 들어오면 에러
	if(fabs(dShiftPosition) < DIST_SHIFT_HOME_MARGIN && bShiftHome == OFF)
	{
		ADD_ERR_LOG("CheckAbnormalOnWait : ERR_SHIFT_HOMESEN_NOTON");
		return ERR_SHIFT_HOMESEN_NOTON;
	}
	*/

	return NO_ERR;
}

/**
 @brief   대기 중 이상 상태 확인 : 외부 사용
 @author  ehoto
 @date    2013.02.07
 @return  문제 있을 경우 에러 반환, 그 외에는 NO_ERR(0) 반환
 */
UINT TransControl::CheckAlways(void)
{
	// [ TIME ] //
	int nTmpError = NO_ERR;
    DWORD dwCurrTime = timeGetTime();
	static DWORD dwLastTimeOfHandNormalState = timeGetTime();
    static DWORD dwLastTimeOfHandGoSignalOn = timeGetTime();
	static DWORD dwTimeOfLastOscilationNormal = timeGetTime();
// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate 관련 시간 Check 변수 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
    static DWORD dwLastTimeOfAxisNormalState[3] =
    {timeGetTime(), };
#else
   static DWORD dwLastTimeOfAxisNormalState[2] =
   {timeGetTime(), };
#endif



	static DWORD dwTimeOfLastShutterInPosition[2] =
	{timeGetTime(), };



	// [ HAND ] //

	bool bHandOpen = m_pHWSet->m_pHand->IsOpen();
	bool bHandClose = m_pHWSet->m_pHand->IsClose();
    bool bSlipring = bHandOpen & bHandClose ;

    bool bRightGripOn = m_pHWSet->m_pHand->IsRightGripOn();
    bool bLeftGripOn = m_pHWSet->m_pHand->IsLeftGripOn();
    bool bHandGoSig = m_pHWSet->m_pHand->IsGoSigOn();
	bool bHandAlarm = m_pHWSet->m_pHand->IsAlarm();
	bool bHandPowerOn = m_pHWSet->m_pHand->IsHandPowerOn();

	// [ AXIS ] //
    bool bHoistHome = m_pHWSet->m_pHoistAxis->IsHoistHome();
    bool bShiftHome = m_pHWSet->m_pShiftAxis->IsShiftHome();
	bool bHoistRunning = m_pHWSet->m_pHoistAxis->IsRunning();
    bool bShiftRunning = m_pHWSet->m_pShiftAxis->IsRunning();
    double dHoistPosition = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
	double dShiftPosition = m_pHWSet->m_pShiftAxis->GetCurrentPosition();
    double dHoistSpeed = m_pHWSet->m_pHoistAxis->GetCurrentVelocity();
	double dShiftSpeed = m_pHWSet->m_pShiftAxis->GetCurrentVelocity();
// 2016-12,05, Add by Labeler, ROHT의 경우 Rotate 정보 처리 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
	bool bRotateRunning = m_pHWSet->m_pRotate->IsRunning();
    int dRotatePosition = m_pHWSet->m_pRotate->GetCurrentPositionDegree();
	int dRotateSpeed = m_pHWSet->m_pRotate->GetCurrentVelocity();

#endif


	// [ SHUTTER ] //
	bool bShutterFrontOpen = m_pHWSet->m_pShutter->IsOpen(FRONT);
	bool bShutterFrontClose = m_pHWSet->m_pShutter->IsClose(FRONT);
	bool bShutterRearOpen = m_pHWSet->m_pShutter->IsOpen(REAR);
	bool bShutterRearClose = m_pHWSet->m_pShutter->IsClose(REAR);


    // [Oscilation] //
	bool bOscilationNone = m_pHWSet->m_pOcsillation->IsDetect();

	static int nCountOfHandLimitAllOn	= 0;
	static int nCountOfFrontShutterLimitAllOn	= 0;
	static int nCountOfRearShutterLimitAllOn	= 0;
	static int nCountOfHandGripInLoaded	= 0;
	static int nCountOfHandGripInUnloaded	= 0;

     static CStepTimer timer1;

	if((m_pExecuteInfo->ExecuteMode == EXECUTE_MODE_MANUAL) && (m_pExecuteInfo->bJogInterlockIgnore == true))
	{
		return NO_ERR;
	}
    //슬립링 Hand Open & close 동시 ON일 때 SlipringFlag = 1 2sec 동안 유지.
    if(bSlipring)
    {
        m_bSlipringFlag=true;
    }
    if(timer1.OnDelay(m_bSlipringFlag, 2000))
    {
	    timer1.OnDelay(false);
	    m_bSlipringFlag=false;
    }
    // 1 0. 이상 상황 조치
	// 2 ( 이곳에서 에러를 발생 시키지 않고 필요시 아래서 발생 )
	// 2 Hand PIO 전원 이상 확인
	if(bHandGoSig == false && bHandAlarm == false && bHandClose == false && bHandOpen == false && bRightGripOn == false &&
		bLeftGripOn == false)
	{
		if(bHandPowerOn == true)
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwLastTimeOfHandNormalState) > 2000)
			{
				m_pHWSet->m_pHand->HandPowerControl(PWR_OFF);
			}
        }
        else
        {
            dwLastTimeOfHandNormalState = dwCurrTime;
        }
	}
    else
    {
		dwLastTimeOfHandNormalState = dwCurrTime;
	}

    if(bHandGoSig == false)
    {
        m_pHWSet->m_pHand->IO_Init();
    }
    else
    {
        dwLastTimeOfHandGoSignalOn = dwCurrTime;
    }

	if((bHandAlarm == true) && (m_bFDCHandAlarmFlag == true))
	{
		SEND_WARN("WARN/51/1/6/%06d/%06d/%d/%d/%06d/%d",
				m_pOHTMainStatus->StatusCommon.CurNodeID,
				m_pExecuteInfo->ExecutePositionInfo.NextNode,
				m_pOHTMainStatus->StatusCommon.CurNodeOffset,
				m_pOHTMainStatus->StatusCommon.ExistFoup,
				m_pOHTMainStatus->StatusCommon.StopStationID,
				bHandAlarm);
		m_bFDCHandAlarmFlag = false;
	}

    // 1 2. AXIS 확인

    // 2 축 에러 확인
	nTmpError = m_pHWSet->m_pHoistAxis->GetError();
    if(nTmpError != NO_ERR)
        return nTmpError;

    nTmpError = m_pHWSet->m_pShiftAxis->GetError();
	if(nTmpError != NO_ERR)
		return nTmpError;

// 2016-12,05, Add by Labeler, ROHT의 경우 Rotate 에러 처리 추가

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
    nTmpError = m_pHWSet->m_pRotate->GetError();
    if(nTmpError != NO_ERR)
		return nTmpError;
#endif

    // 2     축이 외력에 의해 움직일 경우 에러 발생
    if(bHoistRunning == false && fabs(dHoistSpeed) > HOIST_ABNOMARL_MOVE_SPEED_LIMIT)
    {
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwLastTimeOfAxisNormalState[0]) > 100)
        {
			ADD_ERR_LOG("CheckAlways : ERR_HOIST_ABNORMAL_MOVING");
			ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%f/%f",
							ERR_HOIST_ABNORMAL_MOVING,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							dHoistPosition,
							dHoistSpeed);
			return ERR_HOIST_ABNORMAL_MOVING;
		}
    }
    else
    {
        dwLastTimeOfAxisNormalState[0] = dwCurrTime;
    }

    if(bShiftRunning == false && fabs(dShiftSpeed) > SHIFT_ABNOMARL_MOVE_SPEED_LIMIT)
	{
        if(OHTUTILITY::PreventOverflowGetTimeMs(dwLastTimeOfAxisNormalState[1]) > 100)
        {
			ADD_ERR_LOG("CheckAlways : ERR_SHIFT_ABNORMAL_MOVING");
			ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%f/%f",
							ERR_SHIFT_ABNORMAL_MOVING,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							dShiftPosition,
							dShiftSpeed);
            return ERR_SHIFT_ABNORMAL_MOVING;
        }
    }
    else
    {
        dwLastTimeOfAxisNormalState[1] = dwCurrTime;
    }

// 2016-12,05, Add by Labeler, ROHT의 경우 Rotate 에러 처리 추가
// Motor로 부터 받는 속도 Check해 보아서 최적된, 속도로 LIMIT 변경 필요
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
	if((bRotateRunning == false)
    && (((m_nTransType == TRANSTYPE_LOAD) && ((m_nTransStep < STEP_L_LOAD_READY) || (m_nTransStep > STEP_L_CHECK_MOVE_SHIFTROTATE_HOME)))
	   ||((m_nTransType == TRANSTYPE_UNLOAD) && ((m_nTransStep < STEP_U_UNLOAD_READY) || (m_nTransStep > STEP_U_CHECK_MOVE_SHIFTROTATE_HOME))))
	&& (fabs((int)dRotatePosition) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference)))
	&& (fabs(ROTATE_POS_90 - (int)dRotatePosition) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_90)))
	&& (fabs(ROTATE_POS_180 - (int)dRotatePosition) > (ROTATE_INPOS_LIMIT + fabs(m_defualtparam->RotateDefault.nDeviceDifference_180))))
	{
        if(OHTUTILITY::PreventOverflowGetTimeMs(dwLastTimeOfAxisNormalState[2]) > 100)
        {
			ADD_ERR_LOG("CheckAlways : ERR_ROTATE_ABNORMAL_MOVING");
			ADD_LOG("CheckAlways : ERR_ROTATE_ABNORMAL_MOVING, Rotate Pos : %d", dRotatePosition );
            return ERR_ROTATE_NOT_0;
        }
    }
    else
    {
        dwLastTimeOfAxisNormalState[2] = dwCurrTime;
    }
#endif

	// 2     이동 리미트를 벗어났을 때 에러 발생
    if(bHoistRunning == true)
    {
        if(dHoistPosition > HOIST_MOVELIMIT_P && fabs(dHoistSpeed) > 0.0)
        {
            ADD_ERR_LOG("CheckAlways : ERR_HOIST_OUT_OF_P_LIMIT");
			ADD_FDC_LOG("ERR/%d/3/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
						ERR_HOIST_OUT_OF_P_LIMIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_dHoist_TeachingPos,
						m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
						HOIST_MOVELIMIT_P);
            return ERR_HOIST_OUT_OF_P_LIMIT;
        }
        if(dHoistPosition < HOIST_MOVELIMIT_N && fabs(dHoistSpeed) > 0.0)
        {
			ADD_ERR_LOG("CheckAlways : ERR_HOIST_OUT_OF_N_LIMIT");
			ADD_FDC_LOG("ERR/%d/3/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
						ERR_HOIST_OUT_OF_N_LIMIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_dHoist_TeachingPos,
						m_pHWSet->m_pHoistAxis->GetCurrentPosition(),
						HOIST_MOVELIMIT_N);
			return ERR_HOIST_OUT_OF_N_LIMIT;
        }
    }
    if(bShiftRunning == true)
	{

		if(dShiftPosition > SHIFT_MOVELIMIT_P && fabs(dShiftSpeed) > 0.0)
		{
			ADD_ERR_LOG("CheckAlways : ERR_SHIFT_OUT_OF_P_LIMIT");
				ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
						ERR_SHIFT_OUT_OF_P_LIMIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_dShift_TeachingPos,
						m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
						SHIFT_MOVELIMIT_P);
			return ERR_SHIFT_OUT_OF_P_LIMIT;
		}
		if(dShiftPosition < SHIFT_MOVELIMIT_N && fabs(dShiftSpeed) > 0.0)
		{
			ADD_ERR_LOG("CheckAlways : ERR_SHIFT_OUT_OF_N_LIMIT");
                        ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%f/%f/%f",
						ERR_SHIFT_OUT_OF_N_LIMIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_dShift_TeachingPos,
						m_pHWSet->m_pShiftAxis->GetCurrentPosition(),
						SHIFT_MOVELIMIT_N);
			return ERR_SHIFT_OUT_OF_N_LIMIT;
		}
    }

// 2016-12,05, Add by Labeler, ROHT의 경우 Rotate 에러 처리 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
    if(bRotateRunning == true)
    {
        if(dRotatePosition > ROTATE_MOVELIMIT_P && dRotateSpeed > 0.0)
        {
            ADD_ERR_LOG("CheckAlways : ERR_ROTATE_POSITIVE_LIMIT_OVER");
            return ERR_ROTATE_LIMIT_OFF_ERR;
        }
		if(dRotatePosition < ROTATE_MOVELIMIT_N && dRotateSpeed < 0.0)
        {
            ADD_ERR_LOG("CheckAlways : ERR_ROTATE_NEGATIVE_LIMIT_OVER");
            return ERR_ROTATE_LIMIT_OFF_ERR;
        }
    }
#endif

	// 1 센서 이상 확인
	// 2 Rail Support 가 Home, Work Pos 가 동시에 들어올 때 에러 처리

    // [ HAND ] //
	// 2 Hand 열림 닫힘 센서가 둘다 On 이면 에러 발생

	if(bHandOpen == true && bHandClose == true)
	{
		nCountOfHandLimitAllOn++;
		if(nCountOfHandLimitAllOn > 10)
		{
			nCountOfHandLimitAllOn = 0;
			ADD_ERR_LOG("CheckAlways : ERR_HAND_LIMIT_SEN_ALL_ON");
			ADD_FDC_LOG("ERR/%d/3/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_HAND_LIMIT_SEN_ALL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
			return ERR_HAND_LIMIT_SEN_ALL_ON;
		}
	}
	else
		nCountOfHandLimitAllOn = 0;

    // 2 Hand Board 전원 Off 시 에러 발생
	if((OHTUTILITY::PreventOverflowGetTimeMs(dwLastTimeOfHandNormalState) > 10000) && bHandGoSig == false && bHandAlarm == false && bHandClose == false && bHandOpen == false && bRightGripOn == false &&
        bLeftGripOn == false)
    {
		dwLastTimeOfHandNormalState = dwCurrTime;
        ADD_ERR_LOG("CheckAlways : ERR_HAND_SENSOR_OFFERR");
		ADD_FDC_LOG("ERR/%d/2/7/%06d/%06d/%d/%d/%06d/%d/%d",
					ERR_HAND_SENSOR_OFFERR,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					bRightGripOn,
					bLeftGripOn);
        return ERR_HAND_SENSOR_OFFERR;
    }
    // 2 Hand Go Signal Off
	if(bHandGoSig == false && (OHTUTILITY::PreventOverflowGetTimeMs(dwLastTimeOfHandGoSignalOn) > 10000))
    {
		dwLastTimeOfHandGoSignalOn = dwCurrTime;
		ADD_ERR_LOG("CheckAlways : ERR_HANDPIO_GOSIG_NOT_ON");
		ADD_FDC_LOG("ERR/%d/2/6/%06d/%06d/%d/%d/%06d/%d",
						ERR_HANDPIO_GOSIG_NOT_ON,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						bHandGoSig);
        return ERR_HANDPIO_GOSIG_NOT_ON;
    }

	// 2 SHUTTER 열림 닫힘 센서가 둘다 On 이면 에러 발생
    if(bShutterFrontOpen == ON && bShutterFrontClose == ON)
	{
		nCountOfFrontShutterLimitAllOn++;
		if(nCountOfFrontShutterLimitAllOn > 10)
		{
			nCountOfFrontShutterLimitAllOn = 0;
			ADD_ERR_LOG("CheckAlways : ERR_SEN_ABNORMAL_SHUTTER_FRONT");
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
						ERR_SEN_ABNORMAL_SHUTTER_FRONT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_nTransType,
						m_nTransStep,
						bShutterFrontOpen,
						bShutterFrontClose);
			return ERR_SEN_ABNORMAL_SHUTTER_FRONT;
		}
	}
	else
		nCountOfFrontShutterLimitAllOn = 0;
    if(bShutterRearOpen == ON && bShutterRearClose == ON)
	{
		nCountOfRearShutterLimitAllOn++;
		if(nCountOfRearShutterLimitAllOn > 10)
		{
			nCountOfRearShutterLimitAllOn = 0;
			ADD_ERR_LOG("CheckAlways : ERR_SEN_ABNORMAL_SHUTTER_REAR");
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
						ERR_SEN_ABNORMAL_SHUTTER_REAR,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_nTransType,
						m_nTransStep,
						bShutterRearOpen,
						bShutterRearClose);
			return ERR_SEN_ABNORMAL_SHUTTER_REAR;
		}
	}
	else
		nCountOfRearShutterLimitAllOn = 0;

    // 2 Shutter 센서가 감지 되지 않고 일정 시간 지속 시 에러 발생
    if(bShutterFrontOpen == ON || bShutterFrontClose == ON)
    {
		dwTimeOfLastShutterInPosition[0] = dwCurrTime;
    }
    else
    {
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwTimeOfLastShutterInPosition[0]) > 10000)
        {
            dwTimeOfLastShutterInPosition[0] = dwCurrTime;
			ADD_ERR_LOG("CheckAlways : ERR_SEN_ABNORMAL_SHUTTER_FRONT");
			ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
						ERR_SEN_ABNORMAL_SHUTTER_FRONT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_nTransType,
						m_nTransStep,
						bShutterFrontOpen,
						bShutterFrontClose);
			return ERR_SEN_ABNORMAL_SHUTTER_FRONT;
        }
    }
    if(bShutterRearOpen == ON || bShutterRearClose == ON)
    {
        dwTimeOfLastShutterInPosition[1] = dwCurrTime;
    }
	else
    {
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwTimeOfLastShutterInPosition[1]) > 10000)
        {
			dwTimeOfLastShutterInPosition[1] = dwCurrTime;
            ADD_ERR_LOG("CheckAlways : ERR_SEN_ABNORMAL_SHUTTER_REAR");
			ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
						ERR_SEN_ABNORMAL_SHUTTER_REAR,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_nTransType,
						m_nTransStep,
						bShutterRearOpen,
						bShutterRearClose);
            return ERR_SEN_ABNORMAL_SHUTTER_REAR;
		}
	}


    // 2 Gripper 센서 확인
    if(m_StatusSet.byStatus == STS_LOADED && (bRightGripOn == true || bLeftGripOn == true))
	{
		nCountOfHandGripInLoaded++;
		if(nCountOfHandGripInLoaded > 10)
		{
			nCountOfHandGripInLoaded = 0;
			ADD_ERR_LOG("CheckAlways : ERR_GRIP_SEN_ON_IN_LOADED");
			ADD_FDC_LOG("ERR/%d/1/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_GRIP_SEN_ON_IN_LOADED,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
			return ERR_GRIP_SEN_ON_IN_LOADED;
		}
	}
	else
		nCountOfHandGripInLoaded = 0;

    if(m_StatusSet.byStatus == STS_UNLOADED && (bRightGripOn == false || bLeftGripOn == false))
	{
		nCountOfHandGripInUnloaded++;
		if(nCountOfHandGripInUnloaded > 10)
		{
            if(m_bSlipringFlag)
            {
                nCountOfHandGripInUnloaded =0;
                m_bSlipringFlag = false;
				ADD_ERR_LOG("CheckAlways : ERR_SEN_ABNORMAL_HAND");
                ADD_ERR_LOG("CheckAlways : ERR_GRIP_SEN_OFF_IN_UNLOADED");
				ADD_FDC_LOG("ERR/%d/7/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_SEN_ABNORMAL_HAND,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
                return ERR_SEN_ABNORMAL_HAND;
            }
			nCountOfHandGripInUnloaded = 0;
			ADD_ERR_LOG("CheckAlways : ERR_GRIP_SEN_OFF_IN_UNLOADED");
			ADD_FDC_LOG("ERR/%d/1/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_GRIP_SEN_OFF_IN_UNLOADED,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
			return ERR_GRIP_SEN_OFF_IN_UNLOADED;
		}
	}
	else
		nCountOfHandGripInUnloaded = 0;
// (m_TransUse.OscillationSen == true)
	// 2 Oscilation 확인
	if	( (m_pExecuteInfo->ExecuteMode == EXECUTE_MODE_AUTO && m_StatusSet.bIsOnProcess == false && bOscilationNone == true && dHoistPosition > OSCILLATION_IGNORE_DIST)
		|| (m_pExecuteInfo->ExecuteMode == EXECUTE_MODE_MANUAL && m_pParamSet->TransUsePart.OscillationSen == true && m_StatusSet.bIsOnProcess == false && bOscilationNone == true && dHoistPosition > OSCILLATION_IGNORE_DIST))
	{
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwTimeOfLastOscilationNormal) > 10000)
        {
            dwTimeOfLastOscilationNormal = dwCurrTime;
            ADD_ERR_LOG("CheckAlways : ERR_OSCILLATION_ERROR");
			ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%f/%f",
						ERR_OSCILLATION_ERROR,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						bOscilationNone,
						dHoistPosition,
						OSCILLATION_IGNORE_DIST);
			return ERR_OSCILLATION_ERROR;
        }
		else if(OHTUTILITY::PreventOverflowGetTimeMs(dwTimeOfLastOscilationNormal) > 100 && m_pHWSet->m_pHoistAxis->IsRunning() == true)
        {
            dwTimeOfLastOscilationNormal = dwCurrTime;
			ADD_ERR_LOG("CheckAlways : ERR_OSCILLATION_ERROR");
			ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%f/%f",
						ERR_OSCILLATION_ERROR,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						bOscilationNone,
						dHoistPosition,
						OSCILLATION_IGNORE_DIST);
			return ERR_OSCILLATION_ERROR;
		}
		else if(dwCurrTime - dwTimeOfLastOscilationNormal > 100)
		{
			if(m_bFDCOcsillationFlag == true)
			{
				SEND_WARN("WARN/13/4/5/%06d/%06d/%d/%d/%06d",
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID);
				m_bFDCOcsillationFlag =false;
			}
		}
	}
    else
    {
		dwTimeOfLastOscilationNormal = dwCurrTime;
    }

	return NO_ERR;
}

/**
 @brief   명령 삭제
 @author  ehoto
 @date    2013.02.12
 @return  명령 삭제 성공 시 true, 실패 시 false 반환
 */
bool TransControl::DeleteCmd(void *pCmd)
{
    bool bReturn = true;
    try
    {
        EXECUTE_TRANS_INFO*sTmpManualControlInfo = (EXECUTE_TRANS_INFO*)pCmd;
        delete sTmpManualControlInfo;
	}
    catch(...)
    {
        bReturn = false;
    }
    return bReturn;
}
// ---------------------------------------------------------------------------
bool TransControl::loadParameterRecovery(char* szFilePath, TRANS_PARAM_SET* pParamSet)
{
	bool bSuccess = true;
	int nTmpValue = 0;
	int nResult = 0;
    double dTmpValue = 0.0;
	bool bTmpValue = false;
	double dResult = 0.0;
	char strTempValue[8]= {0,};
	bool bCheckParam = false;

    LIniFile *pIniFile;
    pIniFile = NULL;
    pIniFile = new LIniFile(szFilePath);

	if(!pIniFile->GetIniFileExist())
    {
		delete pIniFile;
		pIniFile = NULL;
		bSuccess = false;
	}

	if(pIniFile != NULL)
	{

		pIniFile->ReadString("Version","IniVersion", "00", strTempValue);
		if(StrToIntA(strTempValue) <= StrToIntA(pParamSet->Version))
			bSuccess = false;
		else
		{
			memmove(&pParamSet->Version, strTempValue, strlen(strTempValue)); //Return Value
			memset(strTempValue, 0, sizeof(strTempValue));
	    }


		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("HOIST", "AccelMax", HOIST_ACCEL_MAXLIMIT, HOIST_ACCEL_MAXLIMIT_MIN, HOIST_ACCEL_MAXLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				pParamSet->HoistAxis.Limit.Accel.MaxValue = dTmpValue;
			}
			else
			{
            	bSuccess = false;
            }

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("HOIST", "AccelMin", HOIST_ACCEL_MINLIMIT, HOIST_ACCEL_MINLIMIT_MIN, HOIST_ACCEL_MINLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				pParamSet->HoistAxis.Limit.Accel.MinValue = dTmpValue;
			}
			else
			{
            	bSuccess = false;
            }

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("HOIST", "DecelMax", HOIST_DECEL_MAXLIMIT, HOIST_DECEL_MAXLIMIT_MIN, HOIST_DECEL_MAXLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				pParamSet->HoistAxis.Limit.Decel.MaxValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("HOIST", "DecelMin", HOIST_DECEL_MINLIMIT, HOIST_DECEL_MINLIMIT_MIN, HOIST_DECEL_MINLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				pParamSet->HoistAxis.Limit.Decel.MinValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("HOIST", "SpeedMax", HOIST_SPEED_MAXLIMIT, HOIST_SPEED_MAXLIMIT_MIN, HOIST_SPEED_MAXLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				pParamSet->HoistAxis.Limit.Speed.MaxValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = 	pIniFile->ReadDoubleMinMaxVerification("HOIST", "SpeedMin", HOIST_SPEED_MINLIMIT, HOIST_SPEED_MINLIMIT_MIN, HOIST_SPEED_MINLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				pParamSet->HoistAxis.Limit.Speed.MinValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("HOIST", "dAbnormalMoveSpeedMargin", HOIST_ABNORMALMOVESPEEDMARGIN,HOIST_ABNORMALMOVESPEEDMARGIN_MIN, HOIST_ABNORMALMOVESPEEDMARGIN_MAX, &dTmpValue);
			if(bCheckParam)
			{
				pParamSet->HoistAxis.Limit.dAbnormalMoveSpeedMargin = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("HOIST", "PositionMax", HOIST_POSITION_MAXLIMIT, HOIST_POSITION_MAXLIMIT_MIN,HOIST_POSITION_MAXLIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				pParamSet->HoistAxis.Limit.Position.MaxValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("HOIST", "PositionMin", HOIST_POSITION_MINLIMIT, HOIST_POSITION_MINLIMIT_MIN,HOIST_POSITION_MINLIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				 pParamSet->HoistAxis.Limit.Position.MinValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("HOIST", "JogPositionMax", HOIST_JOGPOSITION_MAXLIMIT, HOIST_JOGPOSITION_MAXLIMIT_MIN,HOIST_JOGPOSITION_MAXLIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				 pParamSet->HoistAxis.Limit.JogPosition.MaxValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("HOIST", "JogPositionMin", HOIST_JOGPOSITION_MINLIMIT, HOIST_JOGPOSITION_MINLIMIT_MIN,HOIST_JOGPOSITION_MINLIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				 pParamSet->HoistAxis.Limit.JogPosition.MinValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("HOIST", "dDefaultAccel", HOIST_ACCEL_DEFAULT, pParamSet->HoistAxis.Limit.Accel.MinValue,pParamSet->HoistAxis.Limit.Accel.MaxValue, &dTmpValue);
			if(bCheckParam)
			{
				 pParamSet->HoistAxis.Default.dDefaultAccel = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("HOIST", "dDefaultDecel", HOIST_DECEL_DEFAULT, pParamSet->HoistAxis.Limit.Decel.MinValue,pParamSet->HoistAxis.Limit.Decel.MaxValue, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->HoistAxis.Default.dDefaultDecel = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("HOIST", "dEmergencyDecel", HOIST_EMERGENCYDECEL_DEFAULT, pParamSet->HoistAxis.Limit.Decel.MinValue,pParamSet->HoistAxis.Limit.Decel.MaxValue, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->HoistAxis.Default.dEmergencyDecel = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("SHIFT", "AccelMax", SHIFT_ACCEL_MAXLIMIT, SHIFT_ACCEL_MAXLIMIT_MIN, SHIFT_ACCEL_MAXLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->ShiftAxis.Limit.Accel.MaxValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("SHIFT", "AccelMin", SHIFT_ACCEL_MINLIMIT, SHIFT_ACCEL_MINLIMIT_MIN, SHIFT_ACCEL_MINLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->ShiftAxis.Limit.Accel.MinValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("SHIFT", "DecelMax", SHIFT_DECEL_MAXLIMIT, SHIFT_DECEL_MAXLIMIT_MIN, SHIFT_DECEL_MAXLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Limit.Decel.MaxValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("SHIFT", "DecelMin", SHIFT_DECEL_MINLIMIT, SHIFT_DECEL_MINLIMIT_MIN, SHIFT_DECEL_MINLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Limit.Decel.MinValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("SHIFT", "SpeedMax", SHIFT_SPEED_MAXLIMIT, SHIFT_SPEED_MAXLIMIT_MIN, SHIFT_SPEED_MAXLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Limit.Speed.MaxValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("SHIFT", "SpeedMin", SHIFT_SPEED_MINLIMIT, SHIFT_SPEED_MINLIMIT_MIN, SHIFT_SPEED_MINLIMIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Limit.Speed.MinValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("SHIFT", "dAbnormalMoveSpeedMargin", SHIFT_ABNORMALMOVESPEEDMARGIN,SHIFT_ABNORMALMOVESPEEDMARGIN_MIN, SHIFT_ABNORMALMOVESPEEDMARGIN_MAX, &dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Limit.dAbnormalMoveSpeedMargin = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("SHIFT", "PositionMax", SHIFT_POSITION_MAXLIMIT, SHIFT_POSITION_MAXLIMIT_MIN,SHIFT_POSITION_MAXLIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Limit.Position.MaxValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

        if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("SHIFT", "PositionMin", SHIFT_POSITION_MINLIMIT, SHIFT_POSITION_MINLIMIT_MIN,SHIFT_POSITION_MINLIMITMAX, &dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Limit.Position.MinValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("SHIFT", "JogPositionMax", SHIFT_JOGPOSITION_MAXLIMIT, SHIFT_JOGPOSITION_MAXLIMIT_MIN,SHIFT_JOGPOSITION_MAXLIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Limit.JogPosition.MaxValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("SHIFT", "JogPositionMin", SHIFT_JOGPOSITION_MINLIMIT, SHIFT_JOGPOSITION_MINLIMIT_MIN,SHIFT_JOGPOSITION_MINLIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Limit.JogPosition.MinValue = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("SHIFT", "dDefaultAccel", SHIFT_ACCEL_DEFAULT, pParamSet->ShiftAxis.Limit.Accel.MinValue,pParamSet->ShiftAxis.Limit.Accel.MaxValue, &dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Default.dDefaultAccel = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("SHIFT", "dDefaultDecel", SHIFT_DECEL_DEFAULT, pParamSet->ShiftAxis.Limit.Decel.MinValue,pParamSet->ShiftAxis.Limit.Decel.MaxValue, &dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Default.dDefaultDecel = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("SHIFT", "dEmergencyDecel", SHIFT_EMERGENCYDECEL_DEFAULT, pParamSet->ShiftAxis.Limit.Decel.MinValue,pParamSet->ShiftAxis.Limit.Decel.MaxValue, &dTmpValue);
			if(bCheckParam)
			{
				   pParamSet->ShiftAxis.Default.dEmergencyDecel = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


         // 3. TRANS USE_SET
		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "EQPIO", EQPIO_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				   pParamSet->TransUsePart.EQPIO = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "GripperSen", GRIPPERSEN_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				   pParamSet->TransUsePart.GripperSen = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "OscillationSen", OSCILLATIONSEN_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				   pParamSet->TransUsePart.OscillationSen = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "HandPushSen", HANDPUSHSEN_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.HandPushSen = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "LookdownSen", LOOKDOWNSEN_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.LookdownSen = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "ShutterControl", SHUTTERCONTROL_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.ShutterControl = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "HandSensor", HANDSEN_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.HandSensor = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "HoistAlign", HOISTALIGN_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.HoistHomeSen = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

// 2016-12-05, Add by Labeler, ROHT의 경우, STB, UTB 미사용
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
        pParamSet->TransUsePart.STBSen = false;
        pParamSet->TransUsePart.UTBSen = false;
		pParamSet->TransUsePart.UTBFoupCheck = false;

//		// 2016-11-28, Add by Labeler, Rotate 명령 Time Interlock용 Parameter 변수 추가
//		if(bSuccess)
//		{
//			bCheckParam = p	pIniFile->ReadIntegerMaxCheck("DEFINE", "nRotate_Exec_Timeout", ROTATE_EXEC_TIMEOUT, ROTATE_EXEC_TIMEOUT_MAX, &nTmpValue);
//			if(bCheckParam)
//			{
//				  pParamSet->TransDefine.nRotate_Exec_Timeout = nTmpValue;
//			}
//			else
//			{
//				bSuccess = false;
//			}
//
//		}

#else
		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "STBSen", STBSEN_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.STBSen = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "UTBSen", UTBSEN_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.UTBSen = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "UTBFoupCheck", UTBFOUPCHECK_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.UTBFoupCheck = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


#endif
		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "IgnorePIOError", IGNOREPIOERROR_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.IgnorePIOError = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "TransSound", TRANSSOUND_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.TransSound = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "ObsDetectSound", OBSDETECTSOUND_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.ObsDetectSound = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "DrivingSound", DRIVINGSOUND_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.DrivingSound = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("USE_SET", "EQPIOType", TYPE_PIO_HYBRID,TYPE_PIO_IR , TYPE_PIO_HYBRID, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.EQPIOType = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadBoolVerification("USE_SET", "OscillationSenPause", OSCILLATIONSENPAUSE_DEFAULT, &bTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransUsePart.OscillationSenPause = bTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

        // 4. TRANS DEFINE

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHoist_Positive_Limit_OnWait", HOIST_POSITIVE_LIMIT_ON_WAIT,HOIST_NEGATIVE_LIMIT_ON_WAIT_MIN,HOIST_POSITIVE_LIMIT_ON_WAIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHoist_Positive_Limit_OnWait = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =   pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHoist_Negative_Limit_OnWait", HOIST_NEGATIVE_LIMIT_ON_WAIT,HOIST_NEGATIVE_LIMIT_ON_WAIT_MIN, HOIST_POSITIVE_LIMIT_ON_WAIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHoist_Negative_Limit_OnWait = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

 		if(bSuccess)
		{
			bCheckParam =   pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Positive_Limit_OnWait", SHIFT_POSITIVE_LIMIT_ON_WAIT,SHIFT_NEGATIVE_LIMIT_ON_WAIT_MIN,SHIFT_POSITIVE_LIMIT_ON_WAIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Positive_Limit_OnWait = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =   pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Negative_Limit_OnWait", SHIFT_NEGATIVE_LIMIT_ON_WAIT,SHIFT_NEGATIVE_LIMIT_ON_WAIT_MIN,SHIFT_POSITIVE_LIMIT_ON_WAIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Negative_Limit_OnWait = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =   pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHoist_Abnormal_Move_Speed_Limit", HOIST_ABNOMARL_MOVE_SPEED_LIMIT,HOIST_ABNOMARL_MOVE_SPEED_LIMIT,HOIST_ABNOMARL_MOVE_SPEED_LIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHoist_Abnormal_Move_Speed_Limit = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =   pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Abnormal_Move_Speed_Limit", SHIFT_ABNOMARL_MOVE_SPEED_LIMIT,SHIFT_ABNOMARL_MOVE_SPEED_LIMIT,SHIFT_ABNOMARL_MOVE_SPEED_LIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Abnormal_Move_Speed_Limit = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}



 		if(bSuccess)
		{
			bCheckParam =   pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHoist_MoveLimit_P", HOIST_MOVELIMIT_P, HOIST_MOVELIMIT_N_MIN,HOIST_MOVELIMIT_P_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHoist_MoveLimit_P = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

 		if(bSuccess)
		{
			bCheckParam =    pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHoist_MoveLimit_N", HOIST_MOVELIMIT_N, HOIST_MOVELIMIT_N_MIN,HOIST_MOVELIMIT_P_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHoist_MoveLimit_N = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_MoveLimit_P", SHIFT_MOVELIMIT_P, SHIFT_MOVELIMIT_N_MIN,SHIFT_MOVELIMIT_P_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_MoveLimit_P = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_MoveLimit_N", SHIFT_MOVELIMIT_N, SHIFT_MOVELIMIT_N_MIN,SHIFT_MOVELIMIT_P_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_MoveLimit_N = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

 		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dDist_Hoist_Home_Margin", DIST_HOIST_HOME_MARGIN, DIST_HOIST_HOME_MARGIN,DIST_HOIST_HOME_MARGIN_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dDist_Hoist_Home_Margin = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

 		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dDist_Shift_Home_Margin", DIST_SHIFT_HOME_MARGIN, DIST_SHIFT_HOME_MARGIN,DIST_SHIFT_HOME_MARGIN_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dDist_Shift_Home_Margin = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

 		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nUpstate_SpeedDown_Offset", DEF_nUpstate_SpeedDown_Offset,20,DEF_nUpstate_SpeedDown_Offset_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nUpstate_SpeedDown_Offset = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nDnState_UP_SpeedDown_Offset", DEF_nDnState_UP_SpeedDown_Offset,20,DEF_nDnState_UP_SpeedDown_Offset_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

 		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nDnState_DN_SpeedDown_Offset", DEF_nDnState_DN_SpeedDown_Offset,20,DEF_nDnState_DN_SpeedDown_Offset_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}
        	//Load Unload 거리값 구분
		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nUpstate_SpeedDown_Offset_Load", pParamSet->TransDefine.nUpstate_SpeedDown_Offset,20,DEF_nUpstate_SpeedDown_Offset_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nUpstate_SpeedDown_Offset_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nDnState_UP_SpeedDown_Offset_Load", pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset,20,DEF_nDnState_UP_SpeedDown_Offset_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nDnState_DN_SpeedDown_Offset_Load", pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset,20,DEF_nDnState_DN_SpeedDown_Offset_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nLookdn_Detect_Timeover", LOOKDN_DETECT_TIMEOVER, 4000,LOOKDN_DETECT_TIMEOVER_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nLookdn_Detect_Timeover = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dLookdn_Detect_Check_Dist", LOOKDOWN_DETECT_RANGE, 100.0,LOOKDOWN_DETECT_RANGE_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dLookdn_Detect_Check_Dist = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nTrans_Timeover", TRANS_TIMEOVER, 30000,TRANS_TIMEOVER_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nTrans_Timeover = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dOscillation_Check_Dist", OSCILLATIONCHECKDIST, 5.0,OSCILLATIONCHECKDIST_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dOscillation_Check_Dist = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHoist_MovInpos_Limit", HOIST_MOVINPOS_LIMIT, 15.0,HOIST_MOVINPOS_LIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHoist_MovInpos_Limit = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Fast_Move_Dist", SHIFT_FAST_MOVE_DIST, 40.0,SHIFT_FAST_MOVE_DIST_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Fast_Move_Dist = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dOscillation_Ignore_Dist", OSCILLATION_IGNORE_DIST, 150.0,OSCILLATION_IGNORE_DIST_MAX,&dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dOscillation_Ignore_Dist = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dFast_Move_Limit", IS_FAST_MOVE_LIMIT, 1000.0,IS_FAST_MOVE_LIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dFast_Move_Limit = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dFast_Move_Limit_Ver85h", IS_FAST_MOVE_LIMIT_MAX, 1000.0,5000.0, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dFast_Move_Limit_Ver85h = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nSTBUTBSensorOFFTime", STBUTBSensorOFFTime, 2500,STBUTBSensorOFFTime_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nSTBUTBSensorOFFTime = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nSTBUTBSensorONTime", STBUTBSensorONTime, 2500,STBUTBSensorONTime_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nSTBUTBSensorONTime = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Speed_Ratio", SHIFT_SPEED_RATIO, 0.7,SHIFT_SPEED_RATIO_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Speed_Ratio = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_AccTime", SHIFT_ACCTIME, 300.0,SHIFT_ACCTIME_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_AccTime = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHoist_Max_Speed", HOIST_MAX_SPEED, 1.8,HOIST_MAX_SPEED_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHoist_Max_Speed = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHoist_Pos_Limit", HOIST_POS_LIMIT, 3100.0, HOIST_POS_LIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHoist_Pos_Limit = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHoist_Neg_Limit", HOIST_NEG_LIMIT, HOIST_NEG_LIMIT_MIN, 0.0, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHoist_Neg_Limit = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Pos_Limit", SHIFT_POS_LIMIT, 0.0,SHIFT_POS_LIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Pos_Limit = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Neg_Limit", SHIFT_NEG_LIMIT, SHIFT_NEG_LIMIT_MIN,0.0, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Neg_Limit = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dInpos_Limit", INPOS_LIMIT, 0.1, INPOS_LIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dInpos_Limit = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Pos_Limit_ForWait", SHIFT_POS_LIMIT_FORWAIT, 10.0,SHIFT_POS_LIMIT_FORWAIT_MAX,&dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Pos_Limit_ForWait = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Neg_Limit_ForWait", SHIFT_NEG_LIMIT_FORWAIT, SHIFT_NEG_LIMIT_FORWAIT_MIN,-10.0,&dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Neg_Limit_ForWait = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHoist_Pos_Limit_TransOffset", HOIST_POS_LIMIT_TRANSOFFSET, 20.0,HOIST_POS_LIMIT_TRANSOFFSET_MAX,&dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHoist_Pos_Limit_TransOffset = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHoist_Origin_Limit", HOIST_ORIGIN_LIMIT, 1.0,HOIST_ORIGIN_LIMIT_MAX, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHoist_Origin_Limit = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nDnState_DN_SpeedDown_Speed_Load", 100, 30,100, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nDnState_DN_SpeedDown_Speed_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Speed_Ratio_50Down_Load", 0.1, 0.0,1.0, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Speed_Ratio_50Down_Load = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Speed_Ratio_50Up_Load", 0.3, 0.0,1.0, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Speed_Ratio_50Up_Load = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Speed_Ratio_50Down_UnLoad", 0.1, 0.0,1.0, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Speed_Ratio_50Down_UnLoad = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dShift_Speed_Ratio_50Up_UnLoad", 0.3, 0.0,1.0, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dShift_Speed_Ratio_50Up_UnLoad = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("DEFINE", "dHandPushRetryPos", MOVETP_RETRYCOUNT_DISTANCE, 10.0, 50.0, &dTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.dHandPushRetryPos = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nUpstate_SpeedDown_Offset_Load", pParamSet->TransDefine.nUpstate_SpeedDown_Offset,20,DEF_nUpstate_SpeedDown_Offset_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nUpstate_SpeedDown_Offset_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nDnState_UP_SpeedDown_Offset_Load", pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset,20,DEF_nDnState_UP_SpeedDown_Offset_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nDnState_DN_SpeedDown_Offset_Load", pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset,20,DEF_nDnState_DN_SpeedDown_Offset_MAX, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveUp_HomeOffset_Load", 50, 0, 1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveUp_HomeOffset_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveUp_TeachingOffset_Load", 50,0,1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveDown_TeachingOffset_Load", 50,0,	1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveUp_HomeOffset_Unload", 50,0,	1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveUp_HomeOffset_Unload = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveUp_TeachingOffset_Unload", 50,0,	1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_Unload = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveDown_TeachingOffset_Unload", 50, 0,	1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_Unload = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveUp_HomeOffset_STB_Load", 50, 0,	1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveUp_HomeOffset_STB_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveUp_TeachingOffset_STB_Load", 50, 0,	1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_STB_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveDown_TeachingOffset_STB_Load", 50,0, 1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_STB_Load = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveUp_HomeOffset_STB_Unload", 50,	0,1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveUp_HomeOffset_STB_Unload = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveUp_TeachingOffset_STB_Unload", 50,	0,1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_STB_Unload = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("DEFINE", "nHoistMoveDown_TeachingOffset_STB_Unload", 50, 0,	1000, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_STB_Unload = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("CARRIER_TYPE", "CarrierType", CARRIER_NOTDEFINED, CARRIER_FOUP,CARRIER_FOSB, &nTmpValue);
			if(bCheckParam)
			{
				  pParamSet->CarrierType = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
        	OHTMainForm->Set_EQPIO_Type(m_pParamSet->TransUsePart.EQPIOType); //혼용 PIO

	   /*
		// 5. CarrierOffset
		char szHoist[8];
		memset(szHoist, 0, sizeof(szHoist));
		memcpy(szHoist, "Hoist_", 6);
		char szShift[8];
		memset(szShift, 0, sizeof(szShift));
		memcpy(szShift, "Shift_", 6);
		for(int n = 0; n < CARRIER_OFFSET_MAX_COUNT; n++)
		{
			szHoist[6] = n + '0';
			szShift[6] = n + '0';
			pIniFile->ReadDoubleMinCheck("CARRIER", szHoist, 0.0, 0.0, &dTmpValue);
			pParamSet->CarrierOffset[n].dHosit = dTmpValue;
			pIniFile->ReadDoubleMinCheck("CARRIER", szShift, 0.0, 0.0, &dTmpValue);
			pParamSet->CarrierOffset[n].dShift = dTmpValue;
		}
         */



		delete pIniFile;




    }
    else
	{
        bSuccess = false;
		
	}


	if(bSuccess ==false)
	{
		bCheckParam = loadParameter("DataFiles\\Trans.bgk", pParamSet);
		if(bCheckParam)
			saveParameter("DataFiles\\Trans.ini", pParamSet);
		else
		   m_ParamChehckError = ERR_PARADATA_LOAD_FAIL;
	}
	else
	{
		saveParameter("DataFiles\\Trans.bgk", pParamSet);
		m_ParamChehckError = NO_ERR;
	}


    return bSuccess;
}

/**
 @brief   명령 삭제
 @author  soo583
 @date    2013.07.11
 @param   szFilePath(파일 경로)
 @return  성공 시 true / 실패 시 false
 */
bool TransControl::loadParameter(char* szFilePath, TRANS_PARAM_SET* pParamSet)
{
    bool bSuccess = true;
	int nTmpValue = 0;
    int nResult = 0;
	double dTmpValue = 0.0;
	bool bTmpValue = false;
	double dResult = 0.0;
	char strTempValue[8]= {0,};

    LIniFile *pIniFile;
	pIniFile = NULL;
    pIniFile = new LIniFile(szFilePath);

	if(!pIniFile->GetIniFileExist())
    {
		delete pIniFile;
		pIniFile = NULL;
		bSuccess = false;
    }

	if(pIniFile != NULL)
	{

		pIniFile->ReadString("Version","IniVersion", "00", strTempValue);
		memmove(&pParamSet->Version, strTempValue, strlen(strTempValue)); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		pIniFile->ReadDoubleMinMaxCheck("HOIST", "AccelMax", HOIST_ACCEL_MAXLIMIT, HOIST_ACCEL_MAXLIMIT_MIN, HOIST_ACCEL_MAXLIMIT_MAX,
            &dTmpValue);
        pParamSet->HoistAxis.Limit.Accel.MaxValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("HOIST", "AccelMin", HOIST_ACCEL_MINLIMIT, HOIST_ACCEL_MINLIMIT_MIN, HOIST_ACCEL_MINLIMIT_MAX,
            &dTmpValue);
        pParamSet->HoistAxis.Limit.Accel.MinValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("HOIST", "DecelMax", HOIST_DECEL_MAXLIMIT, HOIST_DECEL_MAXLIMIT_MIN, HOIST_DECEL_MAXLIMIT_MAX,
            &dTmpValue);
        pParamSet->HoistAxis.Limit.Decel.MaxValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("HOIST", "DecelMin", HOIST_DECEL_MINLIMIT, HOIST_DECEL_MINLIMIT_MIN, HOIST_DECEL_MINLIMIT_MAX,
            &dTmpValue);
		pParamSet->HoistAxis.Limit.Decel.MinValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("HOIST", "SpeedMax", HOIST_SPEED_MAXLIMIT, HOIST_SPEED_MAXLIMIT_MIN, HOIST_SPEED_MAXLIMIT_MAX,
            &dTmpValue);
        pParamSet->HoistAxis.Limit.Speed.MaxValue = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("HOIST", "SpeedMin", HOIST_SPEED_MINLIMIT, HOIST_SPEED_MINLIMIT_MIN, HOIST_SPEED_MINLIMIT_MAX,
			&dTmpValue);
        pParamSet->HoistAxis.Limit.Speed.MinValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("HOIST", "dAbnormalMoveSpeedMargin", HOIST_ABNORMALMOVESPEEDMARGIN,
			HOIST_ABNORMALMOVESPEEDMARGIN_MIN, HOIST_ABNORMALMOVESPEEDMARGIN_MAX, &dTmpValue);
        pParamSet->HoistAxis.Limit.dAbnormalMoveSpeedMargin = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("HOIST", "PositionMax", HOIST_POSITION_MAXLIMIT, HOIST_POSITION_MAXLIMIT_MIN,
            HOIST_POSITION_MAXLIMIT_MAX, &dTmpValue);
        pParamSet->HoistAxis.Limit.Position.MaxValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("HOIST", "PositionMin", HOIST_POSITION_MINLIMIT, HOIST_POSITION_MINLIMIT_MIN,
            HOIST_POSITION_MINLIMIT_MAX, &dTmpValue);
        pParamSet->HoistAxis.Limit.Position.MinValue = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("HOIST", "JogPositionMax", HOIST_JOGPOSITION_MAXLIMIT, HOIST_JOGPOSITION_MAXLIMIT_MIN,
            HOIST_JOGPOSITION_MAXLIMIT_MAX, &dTmpValue);
        pParamSet->HoistAxis.Limit.JogPosition.MaxValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("HOIST", "JogPositionMin", HOIST_JOGPOSITION_MINLIMIT, HOIST_JOGPOSITION_MINLIMIT_MIN,
            HOIST_JOGPOSITION_MINLIMIT_MAX, &dTmpValue);
        pParamSet->HoistAxis.Limit.JogPosition.MinValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("HOIST", "dDefaultAccel", HOIST_ACCEL_DEFAULT, pParamSet->HoistAxis.Limit.Accel.MinValue,
            pParamSet->HoistAxis.Limit.Accel.MaxValue, &dTmpValue);
        pParamSet->HoistAxis.Default.dDefaultAccel = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("HOIST", "dDefaultDecel", HOIST_DECEL_DEFAULT, pParamSet->HoistAxis.Limit.Decel.MinValue,
            pParamSet->HoistAxis.Limit.Decel.MaxValue, &dTmpValue);
        pParamSet->HoistAxis.Default.dDefaultDecel = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("HOIST", "dEmergencyDecel", HOIST_EMERGENCYDECEL_DEFAULT, pParamSet->HoistAxis.Limit.Decel.MinValue,
            pParamSet->HoistAxis.Limit.Decel.MaxValue, &dTmpValue);
        pParamSet->HoistAxis.Default.dEmergencyDecel = dTmpValue;


        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "AccelMax", SHIFT_ACCEL_MAXLIMIT, SHIFT_ACCEL_MAXLIMIT_MIN, SHIFT_ACCEL_MAXLIMIT_MAX,
            &dTmpValue);
        pParamSet->ShiftAxis.Limit.Accel.MaxValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "AccelMin", SHIFT_ACCEL_MINLIMIT, SHIFT_ACCEL_MINLIMIT_MIN, SHIFT_ACCEL_MINLIMIT_MAX,
            &dTmpValue);
        pParamSet->ShiftAxis.Limit.Accel.MinValue = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("SHIFT", "DecelMax", SHIFT_DECEL_MAXLIMIT, SHIFT_DECEL_MAXLIMIT_MIN, SHIFT_DECEL_MAXLIMIT_MAX,
            &dTmpValue);
        pParamSet->ShiftAxis.Limit.Decel.MaxValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "DecelMin", SHIFT_DECEL_MINLIMIT, SHIFT_DECEL_MINLIMIT_MIN, SHIFT_DECEL_MINLIMIT_MAX,
            &dTmpValue);
        pParamSet->ShiftAxis.Limit.Decel.MinValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "SpeedMax", SHIFT_SPEED_MAXLIMIT, SHIFT_SPEED_MAXLIMIT_MIN, SHIFT_SPEED_MAXLIMIT_MAX,
            &dTmpValue);
        pParamSet->ShiftAxis.Limit.Speed.MaxValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "SpeedMin", SHIFT_SPEED_MINLIMIT, SHIFT_SPEED_MINLIMIT_MIN, SHIFT_SPEED_MINLIMIT_MAX,
            &dTmpValue);
        pParamSet->ShiftAxis.Limit.Speed.MinValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "dAbnormalMoveSpeedMargin", SHIFT_ABNORMALMOVESPEEDMARGIN,
            SHIFT_ABNORMALMOVESPEEDMARGIN_MIN, SHIFT_ABNORMALMOVESPEEDMARGIN_MAX, &dTmpValue);
        pParamSet->ShiftAxis.Limit.dAbnormalMoveSpeedMargin = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "PositionMax", SHIFT_POSITION_MAXLIMIT, SHIFT_POSITION_MAXLIMIT_MIN,
            SHIFT_POSITION_MAXLIMIT_MAX, &dTmpValue);
        pParamSet->ShiftAxis.Limit.Position.MaxValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "PositionMin", SHIFT_POSITION_MINLIMIT, SHIFT_POSITION_MINLIMIT_MIN,
            SHIFT_POSITION_MINLIMITMAX, &dTmpValue);
		pParamSet->ShiftAxis.Limit.Position.MinValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "JogPositionMax", SHIFT_JOGPOSITION_MAXLIMIT, SHIFT_JOGPOSITION_MAXLIMIT_MIN,
            SHIFT_JOGPOSITION_MAXLIMIT_MAX, &dTmpValue);
		pParamSet->ShiftAxis.Limit.JogPosition.MaxValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "JogPositionMin", SHIFT_JOGPOSITION_MINLIMIT, SHIFT_JOGPOSITION_MINLIMIT_MIN,
            SHIFT_JOGPOSITION_MINLIMIT_MAX, &dTmpValue);
        pParamSet->ShiftAxis.Limit.JogPosition.MinValue = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "dDefaultAccel", SHIFT_ACCEL_DEFAULT, pParamSet->ShiftAxis.Limit.Accel.MinValue,
            pParamSet->ShiftAxis.Limit.Accel.MaxValue, &dTmpValue);
        pParamSet->ShiftAxis.Default.dDefaultAccel = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "dDefaultDecel", SHIFT_DECEL_DEFAULT, pParamSet->ShiftAxis.Limit.Decel.MinValue,
			pParamSet->ShiftAxis.Limit.Decel.MaxValue, &dTmpValue);
        pParamSet->ShiftAxis.Default.dDefaultDecel = dTmpValue;

        pIniFile->ReadDoubleMinMaxCheck("SHIFT", "dEmergencyDecel", SHIFT_EMERGENCYDECEL_DEFAULT, pParamSet->ShiftAxis.Limit.Decel.MinValue,
            pParamSet->ShiftAxis.Limit.Decel.MaxValue, &dTmpValue);
        pParamSet->ShiftAxis.Default.dEmergencyDecel = dTmpValue;

        // 3. TRANS USE_SET
        pIniFile->ReadBool("USE_SET", "EQPIO", EQPIO_DEFAULT, &bTmpValue);
        pParamSet->TransUsePart.EQPIO = bTmpValue;

        pIniFile->ReadBool("USE_SET", "GripperSen", GRIPPERSEN_DEFAULT, &bTmpValue);
        pParamSet->TransUsePart.GripperSen = bTmpValue;

		pIniFile->ReadBool("USE_SET", "OscillationSen", OSCILLATIONSEN_DEFAULT, &bTmpValue);
        pParamSet->TransUsePart.OscillationSen = bTmpValue;

        pIniFile->ReadBool("USE_SET", "HandPushSen", HANDPUSHSEN_DEFAULT, &bTmpValue);
        pParamSet->TransUsePart.HandPushSen = bTmpValue;

        pIniFile->ReadBool("USE_SET", "LookdownSen", LOOKDOWNSEN_DEFAULT, &bTmpValue);
        pParamSet->TransUsePart.LookdownSen = bTmpValue;

        pIniFile->ReadBool("USE_SET", "ShutterControl", SHUTTERCONTROL_DEFAULT, &bTmpValue);
        pParamSet->TransUsePart.ShutterControl = bTmpValue;

        pIniFile->ReadBool("USE_SET", "HandSensor", HANDSEN_DEFAULT, &bTmpValue);
        pParamSet->TransUsePart.HandSensor = bTmpValue;

        pIniFile->ReadBool("USE_SET", "HoistAlign", HOISTALIGN_DEFAULT, &bTmpValue);
		pParamSet->TransUsePart.HoistHomeSen = bTmpValue;
// 2016-12-05, Add by Labeler, ROHT의 경우, STB, UTB 미사용
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
        pParamSet->TransUsePart.STBSen = false;
		pParamSet->TransUsePart.UTBSen = false;
        pParamSet->TransUsePart.UTBFoupCheck = false;

#else
		pIniFile->ReadBool("USE_SET", "STBSen", STBSEN_DEFAULT, &bTmpValue);
		pParamSet->TransUsePart.STBSen = bTmpValue;

		pIniFile->ReadBool("USE_SET", "UTBSen", UTBSEN_DEFAULT, &bTmpValue);
		pParamSet->TransUsePart.UTBSen = bTmpValue;

		pIniFile->ReadBool("USE_SET", "UTBFoupCheck", UTBFOUPCHECK_DEFAULT, &bTmpValue);
		pParamSet->TransUsePart.UTBFoupCheck = bTmpValue;
#endif

        pIniFile->ReadBool("USE_SET", "IgnorePIOError", IGNOREPIOERROR_DEFAULT, &bTmpValue);
        pParamSet->TransUsePart.IgnorePIOError = bTmpValue;

		pIniFile->ReadBool("USE_SET", "TransSound", TRANSSOUND_DEFAULT, &bTmpValue);
		pParamSet->TransUsePart.TransSound = bTmpValue;

		pIniFile->ReadBool("USE_SET", "ObsDetectSound", OBSDETECTSOUND_DEFAULT, &bTmpValue);
		pParamSet->TransUsePart.ObsDetectSound = bTmpValue;

		pIniFile->ReadBool("USE_SET", "DrivingSound", DRIVINGSOUND_DEFAULT, &bTmpValue);
		pParamSet->TransUsePart.DrivingSound = bTmpValue;

		pIniFile->ReadInteger("USE_SET", "EQPIOType", TYPE_PIO_HYBRID, &nTmpValue);
		pParamSet->TransUsePart.EQPIOType = nTmpValue;

		pIniFile->ReadBool("USE_SET", "OscillationSenPause", OSCILLATIONSENPAUSE_DEFAULT, &bTmpValue);
		pParamSet->TransUsePart.OscillationSenPause = bTmpValue;

		// 4. TRANS DEFINE
        pIniFile->ReadDoubleMaxCheck("DEFINE", "dHoist_Positive_Limit_OnWait", HOIST_POSITIVE_LIMIT_ON_WAIT,
			HOIST_POSITIVE_LIMIT_ON_WAIT_MAX, &dTmpValue);
        pParamSet->TransDefine.dHoist_Positive_Limit_OnWait = dTmpValue;

        pIniFile->ReadDoubleMinCheck("DEFINE", "dHoist_Negative_Limit_OnWait", HOIST_NEGATIVE_LIMIT_ON_WAIT,
            HOIST_NEGATIVE_LIMIT_ON_WAIT_MIN, &dTmpValue);
        pParamSet->TransDefine.dHoist_Negative_Limit_OnWait = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dShift_Positive_Limit_OnWait", SHIFT_POSITIVE_LIMIT_ON_WAIT,
            SHIFT_POSITIVE_LIMIT_ON_WAIT_MAX, &dTmpValue);
		pParamSet->TransDefine.dShift_Positive_Limit_OnWait = dTmpValue;

        pIniFile->ReadDoubleMinCheck("DEFINE", "dShift_Negative_Limit_OnWait", SHIFT_NEGATIVE_LIMIT_ON_WAIT,
            SHIFT_NEGATIVE_LIMIT_ON_WAIT_MIN, &dTmpValue);
		pParamSet->TransDefine.dShift_Negative_Limit_OnWait = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dHoist_Abnormal_Move_Speed_Limit", HOIST_ABNOMARL_MOVE_SPEED_LIMIT,
            HOIST_ABNOMARL_MOVE_SPEED_LIMIT_MAX, &dTmpValue);
        pParamSet->TransDefine.dHoist_Abnormal_Move_Speed_Limit = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dShift_Abnormal_Move_Speed_Limit", SHIFT_ABNOMARL_MOVE_SPEED_LIMIT,
            SHIFT_ABNOMARL_MOVE_SPEED_LIMIT_MAX, &dTmpValue);
        pParamSet->TransDefine.dShift_Abnormal_Move_Speed_Limit = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dHoist_MoveLimit_P", HOIST_MOVELIMIT_P, HOIST_MOVELIMIT_P_MAX, &dTmpValue);
        pParamSet->TransDefine.dHoist_MoveLimit_P = dTmpValue;

        pIniFile->ReadDoubleMinCheck("DEFINE", "dHoist_MoveLimit_N", HOIST_MOVELIMIT_N, HOIST_MOVELIMIT_N_MIN, &dTmpValue);
        pParamSet->TransDefine.dHoist_MoveLimit_N = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dShift_MoveLimit_P", SHIFT_MOVELIMIT_P, SHIFT_MOVELIMIT_P_MAX, &dTmpValue);
		pParamSet->TransDefine.dShift_MoveLimit_P = dTmpValue;

        pIniFile->ReadDoubleMinCheck("DEFINE", "dShift_MoveLimit_N", SHIFT_MOVELIMIT_N, SHIFT_MOVELIMIT_N_MIN, &dTmpValue);
        pParamSet->TransDefine.dShift_MoveLimit_N = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dDist_Hoist_Home_Margin", DIST_HOIST_HOME_MARGIN, DIST_HOIST_HOME_MARGIN_MAX, &dTmpValue);
        pParamSet->TransDefine.dDist_Hoist_Home_Margin = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dDist_Shift_Home_Margin", DIST_SHIFT_HOME_MARGIN, DIST_SHIFT_HOME_MARGIN_MAX, &dTmpValue);
        pParamSet->TransDefine.dDist_Shift_Home_Margin = dTmpValue;

        pIniFile->ReadIntegerMaxCheck("DEFINE", "nUpstate_SpeedDown_Offset", DEF_nUpstate_SpeedDown_Offset,
            DEF_nUpstate_SpeedDown_Offset_MAX, &nTmpValue);
		pParamSet->TransDefine.nUpstate_SpeedDown_Offset = nTmpValue;

        pIniFile->ReadIntegerMaxCheck("DEFINE", "nDnState_UP_SpeedDown_Offset", DEF_nDnState_UP_SpeedDown_Offset,
            DEF_nDnState_UP_SpeedDown_Offset_MAX, &nTmpValue);
		pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset = nTmpValue;

        pIniFile->ReadIntegerMaxCheck("DEFINE", "nDnState_DN_SpeedDown_Offset", DEF_nDnState_DN_SpeedDown_Offset,
			DEF_nDnState_DN_SpeedDown_Offset_MAX, &nTmpValue);
		pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset = nTmpValue;

		//Load Unload 거리값 구분
		pIniFile->ReadIntegerMaxCheck("DEFINE", "nUpstate_SpeedDown_Offset_Load", pParamSet->TransDefine.nUpstate_SpeedDown_Offset,
			DEF_nUpstate_SpeedDown_Offset_MAX, &nTmpValue);
		pParamSet->TransDefine.nUpstate_SpeedDown_Offset_Load = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nDnState_UP_SpeedDown_Offset_Load", pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset,
			DEF_nDnState_UP_SpeedDown_Offset_MAX, &nTmpValue);
		pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nDnState_DN_SpeedDown_Offset_Load", pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset,
			DEF_nDnState_DN_SpeedDown_Offset_MAX, &nTmpValue);
		pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset_Load = nTmpValue;
		//-------------------

        pIniFile->ReadIntegerMaxCheck("DEFINE", "nLookdn_Detect_Timeover", LOOKDN_DETECT_TIMEOVER, LOOKDN_DETECT_TIMEOVER_MAX, &nTmpValue);
		pParamSet->TransDefine.nLookdn_Detect_Timeover = nTmpValue;

		pIniFile->ReadDoubleMaxCheck("DEFINE", "dLookdn_Detect_Check_Dist", LOOKDOWN_DETECT_RANGE, LOOKDOWN_DETECT_RANGE_MAX, &dTmpValue);
		pParamSet->TransDefine.dLookdn_Detect_Check_Dist = dTmpValue;


        pIniFile->ReadIntegerMaxCheck("DEFINE", "nTrans_Timeover", TRANS_TIMEOVER, TRANS_TIMEOVER_MAX, &nTmpValue);
        pParamSet->TransDefine.nTrans_Timeover = nTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dOscillation_Check_Dist", OSCILLATIONCHECKDIST, OSCILLATIONCHECKDIST_MAX, &dTmpValue);
        pParamSet->TransDefine.dOscillation_Check_Dist = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dHoist_MovInpos_Limit", HOIST_MOVINPOS_LIMIT, HOIST_MOVINPOS_LIMIT_MAX, &dTmpValue);
        pParamSet->TransDefine.dHoist_MovInpos_Limit = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dShift_Fast_Move_Dist", SHIFT_FAST_MOVE_DIST, SHIFT_FAST_MOVE_DIST_MAX, &dTmpValue);
        pParamSet->TransDefine.dShift_Fast_Move_Dist = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dOscillation_Ignore_Dist", OSCILLATION_IGNORE_DIST, OSCILLATION_IGNORE_DIST_MAX,
			&dTmpValue);
		pParamSet->TransDefine.dOscillation_Ignore_Dist = dTmpValue;


        pIniFile->ReadDoubleMaxCheck("DEFINE", "dFast_Move_Limit", IS_FAST_MOVE_LIMIT, IS_FAST_MOVE_LIMIT_MAX, &dTmpValue);
		pParamSet->TransDefine.dFast_Move_Limit = dTmpValue;

		 pIniFile->ReadDoubleMaxCheck("DEFINE", "dFast_Move_Limit_Ver85h", IS_FAST_MOVE_LIMIT_MAX, 5000, &dTmpValue);
        pParamSet->TransDefine.dFast_Move_Limit_Ver85h = dTmpValue;
	

        pIniFile->ReadIntegerMaxCheck("DEFINE", "nSTBUTBSensorOFFTime", STBUTBSensorOFFTime, STBUTBSensorOFFTime_MAX, &nTmpValue);
        pParamSet->TransDefine.nSTBUTBSensorOFFTime = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nSTBUTBSensorONTime", STBUTBSensorONTime, STBUTBSensorONTime_MAX, &nTmpValue);
		pParamSet->TransDefine.nSTBUTBSensorONTime = nTmpValue;


        pIniFile->ReadDoubleMaxCheck("DEFINE", "dShift_Speed_Ratio", SHIFT_SPEED_RATIO, SHIFT_SPEED_RATIO_MAX, &dTmpValue);
        pParamSet->TransDefine.dShift_Speed_Ratio = dTmpValue;

		pIniFile->ReadDoubleMaxCheck("DEFINE", "dShift_AccTime", SHIFT_ACCTIME, SHIFT_ACCTIME_MAX, &dTmpValue);
        pParamSet->TransDefine.dShift_AccTime = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dHoist_Max_Speed", HOIST_MAX_SPEED, HOIST_MAX_SPEED_MAX, &dTmpValue);
        pParamSet->TransDefine.dHoist_Max_Speed = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dHoist_Pos_Limit", HOIST_POS_LIMIT, HOIST_POS_LIMIT_MAX, &dTmpValue);
        pParamSet->TransDefine.dHoist_Pos_Limit = dTmpValue;

		pIniFile->ReadDoubleMinCheck("DEFINE", "dHoist_Neg_Limit", HOIST_NEG_LIMIT, HOIST_NEG_LIMIT_MIN, &dTmpValue);
		pParamSet->TransDefine.dHoist_Neg_Limit = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dShift_Pos_Limit", SHIFT_POS_LIMIT, SHIFT_POS_LIMIT_MAX, &dTmpValue);
        pParamSet->TransDefine.dShift_Pos_Limit = dTmpValue;

		pIniFile->ReadDoubleMinCheck("DEFINE", "dShift_Neg_Limit", SHIFT_NEG_LIMIT, SHIFT_NEG_LIMIT_MIN, &dTmpValue);
		pParamSet->TransDefine.dShift_Neg_Limit = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dInpos_Limit", INPOS_LIMIT, INPOS_LIMIT_MAX, &dTmpValue);
		pParamSet->TransDefine.dInpos_Limit = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dShift_Pos_Limit_ForWait", SHIFT_POS_LIMIT_FORWAIT, SHIFT_POS_LIMIT_FORWAIT_MAX,
            &dTmpValue);
        pParamSet->TransDefine.dShift_Pos_Limit_ForWait = dTmpValue;

        pIniFile->ReadDoubleMinCheck("DEFINE", "dShift_Neg_Limit_ForWait", SHIFT_NEG_LIMIT_FORWAIT, SHIFT_NEG_LIMIT_FORWAIT_MIN,
            &dTmpValue);
        pParamSet->TransDefine.dShift_Neg_Limit_ForWait = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dHoist_Pos_Limit_TransOffset", HOIST_POS_LIMIT_TRANSOFFSET, HOIST_POS_LIMIT_TRANSOFFSET_MAX,
            &dTmpValue);
		pParamSet->TransDefine.dHoist_Pos_Limit_TransOffset = dTmpValue;

        pIniFile->ReadDoubleMaxCheck("DEFINE", "dHoist_Origin_Limit", HOIST_ORIGIN_LIMIT, HOIST_ORIGIN_LIMIT_MAX, &dTmpValue);
        pParamSet->TransDefine.dHoist_Origin_Limit = dTmpValue;

//// 2016-11-28, Add by Labeler, Rotate 명령 Time Interlock용 Parameter 변수 추가
//#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
//		pIniFile->ReadIntegerMaxCheck("DEFINE", "nRotate_Exec_Timeout", ROTATE_EXEC_TIMEOUT, ROTATE_EXEC_TIMEOUT_MAX, &nTmpValue);
//		pParamSet->TransDefine.nRotate_Exec_Timeout = nTmpValue;
//
//
//#endif
        	//puting
		pIniFile->ReadIntegerMaxCheck("DEFINE", "nDnState_DN_SpeedDown_Speed_Load", 100, 100, &nTmpValue);
		pParamSet->TransDefine.nDnState_DN_SpeedDown_Speed_Load = nTmpValue;


		pIniFile->ReadDoubleMinMaxCheck("DEFINE", "dShift_Speed_Ratio_50Down_Load", 0.1, 0.0,1.0, &dTmpValue);
		pParamSet->TransDefine.dShift_Speed_Ratio_50Down_Load = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("DEFINE", "dShift_Speed_Ratio_50Up_Load", 0.3, 0.0,1.0, &dTmpValue);
		pParamSet->TransDefine.dShift_Speed_Ratio_50Up_Load = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("DEFINE", "dShift_Speed_Ratio_50Down_UnLoad", 0.1, 0.0,1.0, &dTmpValue);
		pParamSet->TransDefine.dShift_Speed_Ratio_50Down_UnLoad = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("DEFINE", "dShift_Speed_Ratio_50Up_UnLoad", 0.3, 0.0,1.0, &dTmpValue);
		pParamSet->TransDefine.dShift_Speed_Ratio_50Up_UnLoad = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("DEFINE", "dHandPushRetryPos", MOVETP_RETRYCOUNT_DISTANCE, 10.0, 50.0, &dTmpValue);
		pParamSet->TransDefine.dHandPushRetryPos = dTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nUpstate_SpeedDown_Offset_Load", pParamSet->TransDefine.nUpstate_SpeedDown_Offset,
			DEF_nUpstate_SpeedDown_Offset_MAX, &nTmpValue);
		pParamSet->TransDefine.nUpstate_SpeedDown_Offset_Load = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nDnState_UP_SpeedDown_Offset_Load", pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset,
			DEF_nDnState_UP_SpeedDown_Offset_MAX, &nTmpValue);
		pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nDnState_DN_SpeedDown_Offset_Load", pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset,
			DEF_nDnState_DN_SpeedDown_Offset_MAX, &nTmpValue);
		pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset_Load = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveUp_HomeOffset_Load", 50,	1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveUp_HomeOffset_Load = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveUp_TeachingOffset_Load", 50,	1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_Load = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveDown_TeachingOffset_Load", 50,	1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_Load = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveUp_HomeOffset_Unload", 50, 1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveUp_HomeOffset_Unload = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveUp_TeachingOffset_Unload", 50,1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_Unload = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveDown_TeachingOffset_Unload", 50,	1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_Unload = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveUp_HomeOffset_STB_Load", 50,	1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveUp_HomeOffset_STB_Load = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveUp_TeachingOffset_STB_Load", 50,	1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_STB_Load = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveDown_TeachingOffset_STB_Load", 50,	1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_STB_Load = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveUp_HomeOffset_STB_Unload", 50,	1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveUp_HomeOffset_STB_Unload = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveUp_TeachingOffset_STB_Unload", 50,	1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_STB_Unload = nTmpValue;

		pIniFile->ReadIntegerMaxCheck("DEFINE", "nHoistMoveDown_TeachingOffset_STB_Unload", 50,	1000, &nTmpValue);
		pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_STB_Unload = nTmpValue;

	
		 /*
		// 5. CarrierOffset
		char szHoist[8];
		memset(szHoist, 0, sizeof(szHoist));
		memcpy(szHoist, "Hoist_", 6);
		char szShift[8];
		memset(szShift, 0, sizeof(szShift));
		memcpy(szShift, "Shift_", 6);
		for(int n = 0; n < CARRIER_OFFSET_MAX_COUNT; n++)
		{
			szHoist[6] = n + '0';
			szShift[6] = n + '0';
			pIniFile->ReadDoubleMinCheck("CARRIER", szHoist, 0.0, 0.0, &dTmpValue);
			pParamSet->CarrierOffset[n].dHosit = dTmpValue;
			pIniFile->ReadDoubleMinCheck("CARRIER", szShift, 0.0, 0.0, &dTmpValue);
			pParamSet->CarrierOffset[n].dShift = dTmpValue;
		}

		*/
		pIniFile->ReadInteger("CARRIER_TYPE", "CarrierType", CARRIER_FOUP, &nTmpValue);
		pParamSet->CarrierType = nTmpValue;

		delete pIniFile;

		OHTMainForm->Set_EQPIO_Type(m_pParamSet->TransUsePart.EQPIOType); //혼용 PIO


	}
	else
	{
        bSuccess = false;
		strcpy(pParamSet->Version, "00");

        pParamSet->HoistAxis.Limit.Accel.MaxValue = HOIST_ACCEL_MAXLIMIT;
        pParamSet->HoistAxis.Limit.Accel.MinValue = HOIST_ACCEL_MINLIMIT;
        pParamSet->HoistAxis.Limit.Decel.MaxValue = HOIST_DECEL_MAXLIMIT;
        pParamSet->HoistAxis.Limit.Decel.MinValue = HOIST_DECEL_MINLIMIT;
		pParamSet->HoistAxis.Limit.Speed.MaxValue = HOIST_SPEED_MAXLIMIT;
        pParamSet->HoistAxis.Limit.Speed.MinValue = HOIST_SPEED_MINLIMIT;
        pParamSet->HoistAxis.Limit.dAbnormalMoveSpeedMargin = HOIST_ABNORMALMOVESPEEDMARGIN;
        pParamSet->HoistAxis.Limit.Position.MaxValue = HOIST_POSITION_MAXLIMIT;
        pParamSet->HoistAxis.Limit.Position.MinValue = HOIST_POSITION_MINLIMIT;
        pParamSet->HoistAxis.Limit.JogPosition.MaxValue = HOIST_JOGPOSITION_MAXLIMIT;
		pParamSet->HoistAxis.Limit.JogPosition.MinValue = HOIST_JOGPOSITION_MINLIMIT;
        pParamSet->HoistAxis.Default.dDefaultAccel = HOIST_ACCEL_DEFAULT;
        pParamSet->HoistAxis.Default.dDefaultDecel = HOIST_DECEL_DEFAULT;
        pParamSet->HoistAxis.Default.dEmergencyDecel = HOIST_EMERGENCYDECEL_DEFAULT;

//        pParamSet->ShiftAxis.GearRatio.dIdealGearRatio = SHIFT_IDEALGEARRATIO;
//        pParamSet->ShiftAxis.GearRatio.dIdeal2RealRatio = SHIFT_IDEAL2REALRATIO;
//        pParamSet->ShiftAxis.GearRatio.dDeviceDifference = SHIFT_HOME_OFFSET_MTL2JIG;
//        pParamSet->ShiftAxis.GearRatio.nHomeSearchCtsOffset = SHIFT_HOME_SEARCH_OFFSET;

        pParamSet->ShiftAxis.Limit.Accel.MaxValue = SHIFT_ACCEL_MAXLIMIT;
        pParamSet->ShiftAxis.Limit.Accel.MinValue = SHIFT_ACCEL_MINLIMIT;
        pParamSet->ShiftAxis.Limit.Decel.MaxValue = SHIFT_DECEL_MAXLIMIT;
        pParamSet->ShiftAxis.Limit.Decel.MinValue = SHIFT_DECEL_MINLIMIT;
        pParamSet->ShiftAxis.Limit.Speed.MaxValue = SHIFT_SPEED_MAXLIMIT;
        pParamSet->ShiftAxis.Limit.Speed.MinValue = SHIFT_SPEED_MINLIMIT;
        pParamSet->ShiftAxis.Limit.dAbnormalMoveSpeedMargin = SHIFT_ABNORMALMOVESPEEDMARGIN;
        pParamSet->ShiftAxis.Limit.Position.MaxValue = SHIFT_POSITION_MAXLIMIT;
        pParamSet->ShiftAxis.Limit.Position.MinValue = SHIFT_POSITION_MINLIMIT;
        pParamSet->ShiftAxis.Limit.JogPosition.MaxValue = SHIFT_JOGPOSITION_MAXLIMIT;
        pParamSet->ShiftAxis.Limit.JogPosition.MinValue = SHIFT_JOGPOSITION_MINLIMIT;
        pParamSet->ShiftAxis.Default.dDefaultAccel = SHIFT_ACCEL_DEFAULT;
        pParamSet->ShiftAxis.Default.dDefaultDecel = SHIFT_DECEL_DEFAULT;
        pParamSet->ShiftAxis.Default.dEmergencyDecel = SHIFT_EMERGENCYDECEL_DEFAULT;

        pParamSet->TransUsePart.EQPIO = EQPIO_DEFAULT;
        pParamSet->TransUsePart.GripperSen = GRIPPERSEN_DEFAULT;
        pParamSet->TransUsePart.OscillationSen = OSCILLATIONSEN_DEFAULT;
        pParamSet->TransUsePart.HandPushSen = HANDPUSHSEN_DEFAULT;
		pParamSet->TransUsePart.LookdownSen = LOOKDOWNSEN_DEFAULT;
		pParamSet->TransUsePart.ShutterControl = SHUTTERCONTROL_DEFAULT;
        pParamSet->TransUsePart.HandSensor = HANDSEN_DEFAULT;
        pParamSet->TransUsePart.HoistHomeSen = HOISTALIGN_DEFAULT;

// 2016-12-05, Add by Labeler, ROHT의 경우 STB, UTB 미사용
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
		pParamSet->TransUsePart.STBSen = false;
        pParamSet->TransUsePart.UTBSen = false;
		pParamSet->TransUsePart.UTBFoupCheck = false;
#else
		pParamSet->TransUsePart.STBSen = STBSEN_DEFAULT;
        pParamSet->TransUsePart.UTBSen = UTBSEN_DEFAULT;
		pParamSet->TransUsePart.UTBFoupCheck = UTBFOUPCHECK_DEFAULT;
#endif
		pParamSet->TransUsePart.IgnorePIOError = IGNOREPIOERROR_DEFAULT;
		pParamSet->TransUsePart.TransSound = TRANSSOUND_DEFAULT;
		pParamSet->TransUsePart.ObsDetectSound = OBSDETECTSOUND_DEFAULT;
		pParamSet->TransUsePart.DrivingSound = DRIVINGSOUND_DEFAULT;
		pParamSet->TransUsePart.EQPIOType = TYPE_PIO_HYBRID;

		pParamSet->TransUsePart.OscillationSenPause =OSCILLATIONSENPAUSE_DEFAULT;

        pParamSet->TransDefine.dHoist_Positive_Limit_OnWait = HOIST_POSITIVE_LIMIT_ON_WAIT;
		pParamSet->TransDefine.dHoist_Negative_Limit_OnWait = HOIST_NEGATIVE_LIMIT_ON_WAIT;
        pParamSet->TransDefine.dShift_Positive_Limit_OnWait = SHIFT_POSITIVE_LIMIT_ON_WAIT;
        pParamSet->TransDefine.dShift_Negative_Limit_OnWait = SHIFT_NEGATIVE_LIMIT_ON_WAIT;
        pParamSet->TransDefine.dHoist_Abnormal_Move_Speed_Limit = HOIST_ABNOMARL_MOVE_SPEED_LIMIT;
        pParamSet->TransDefine.dShift_Abnormal_Move_Speed_Limit = SHIFT_ABNOMARL_MOVE_SPEED_LIMIT;
        pParamSet->TransDefine.dHoist_MoveLimit_P = HOIST_MOVELIMIT_P;
        pParamSet->TransDefine.dHoist_MoveLimit_N = HOIST_MOVELIMIT_N;
		pParamSet->TransDefine.dShift_MoveLimit_P = SHIFT_MOVELIMIT_P;
        pParamSet->TransDefine.dShift_MoveLimit_N = SHIFT_MOVELIMIT_N;
        pParamSet->TransDefine.dDist_Hoist_Home_Margin = DIST_HOIST_HOME_MARGIN;
        pParamSet->TransDefine.dDist_Shift_Home_Margin = DIST_SHIFT_HOME_MARGIN;
        pParamSet->TransDefine.nUpstate_SpeedDown_Offset = DEF_nUpstate_SpeedDown_Offset;
        pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset = DEF_nDnState_UP_SpeedDown_Offset;
		pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset = DEF_nDnState_DN_SpeedDown_Offset;
		//Load Unload 거리값 구분
		pParamSet->TransDefine.nUpstate_SpeedDown_Offset_Load = DEF_nUpstate_SpeedDown_Offset;
		pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load = DEF_nDnState_UP_SpeedDown_Offset;
		pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset_Load = DEF_nDnState_DN_SpeedDown_Offset;

//		pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_SpecialEQ = DEF_nDnState_UP_SpeedDown_Offset;
//		pParamSet->TransDefine.nDnState_UP_SpeedDown_Percent_SpecialEQ = 100;

		pParamSet->TransDefine.nLookdn_Detect_Timeover = LOOKDN_DETECT_TIMEOVER;
		pParamSet->TransDefine.dLookdn_Detect_Check_Dist = LOOKDOWN_DETECT_RANGE;
        pParamSet->TransDefine.nTrans_Timeover = TRANS_TIMEOVER;
        pParamSet->TransDefine.dOscillation_Check_Dist = OSCILLATIONCHECKDIST;
        pParamSet->TransDefine.dHoist_MovInpos_Limit = HOIST_MOVINPOS_LIMIT;
        pParamSet->TransDefine.dShift_Fast_Move_Dist = SHIFT_FAST_MOVE_DIST;
		pParamSet->TransDefine.dOscillation_Ignore_Dist = OSCILLATION_IGNORE_DIST;
        pParamSet->TransDefine.dFast_Move_Limit = IS_FAST_MOVE_LIMIT;
        pParamSet->TransDefine.dFast_Move_Limit_Ver85h = 4000;
        pParamSet->TransDefine.nSTBUTBSensorOFFTime = STBUTBSensorOFFTime;
		pParamSet->TransDefine.nSTBUTBSensorONTime = STBUTBSensorONTime;


        pParamSet->TransDefine.dShift_Speed_Ratio = SHIFT_SPEED_RATIO;
        pParamSet->TransDefine.dShift_AccTime = SHIFT_ACCTIME;
        pParamSet->TransDefine.dHoist_Max_Speed = HOIST_MAX_SPEED;
        pParamSet->TransDefine.dHoist_Pos_Limit = HOIST_POS_LIMIT;
        pParamSet->TransDefine.dHoist_Neg_Limit = HOIST_NEG_LIMIT;
		pParamSet->TransDefine.dShift_Pos_Limit = SHIFT_POS_LIMIT;
        pParamSet->TransDefine.dShift_Neg_Limit = SHIFT_NEG_LIMIT;
		pParamSet->TransDefine.dInpos_Limit = INPOS_LIMIT;
        pParamSet->TransDefine.dShift_Pos_Limit_ForWait = SHIFT_POS_LIMIT_FORWAIT;
        pParamSet->TransDefine.dShift_Neg_Limit_ForWait = SHIFT_NEG_LIMIT_FORWAIT;
        pParamSet->TransDefine.dHoist_Pos_Limit_TransOffset = HOIST_POS_LIMIT_TRANSOFFSET;
        pParamSet->TransDefine.dHoist_Origin_Limit = HOIST_ORIGIN_LIMIT;






		pParamSet->TransDefine.nHoistMoveUp_HomeOffset_Load = 50;  ///< Hoist Up ?à??¸ Offset
		pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_Load = 25;  ///< Hoist Up ?à?À۠:?ӱ??? Offset
		pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_Load = 25;  ///< Hoist Down ?à??¸ :?ӱ??? Offset

		pParamSet->TransDefine.nHoistMoveUp_HomeOffset_Unload = 50;  ///< Hoist Up ?à??¸ Offset
		pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_Unload = 5;  ///< Hoist Up ?à?À۠:?ӱ??? Offset
		pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_Unload = 15;  ///< Hoist Down ?à??¸ :?ӱ??? Offset

		pParamSet->TransDefine.nHoistMoveUp_HomeOffset_STB_Load = 50;  ///< Hoist Up ?à??¸ Offset_STB
		pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_STB_Load = 25;  ///< Hoist Up ?à?À۠:?ӱ??? Offset_STB
		pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_STB_Load = 70;  ///< Hoist Down ?à??¸ :?ӱ??? Offset_STB

		pParamSet->TransDefine.nHoistMoveUp_HomeOffset_STB_Unload = 50;  ///< Hoist Up ?à??¸ Offset_STB
		pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_STB_Unload = 25;  ///< Hoist Up ?à?À۠:?ӱ??? Offset_STB
		pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_STB_Unload = 50;  ///< Hoist Down ?à??¸ :?ӱ??? Offset_STB


// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate 관련 Time Interlock 추가
//#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
//		pParamSet->TransDefine.nRotate_Exec_Timeout =  ROTATE_EXEC_TIMEOUT;
//
//#endif
		pParamSet->TransDefine.nDnState_DN_SpeedDown_Speed_Load = 100;

		pParamSet->TransDefine.dShift_Speed_Ratio_50Down_Load = 0.1;
		pParamSet->TransDefine.dShift_Speed_Ratio_50Up_Load = 0.3;
		pParamSet->TransDefine.dShift_Speed_Ratio_50Down_UnLoad = 0.1;
		pParamSet->TransDefine.dShift_Speed_Ratio_50Up_UnLoad = 0.3;

		pParamSet->TransDefine.dHandPushRetryPos = MOVETP_RETRYCOUNT_DISTANCE;
		/*
		// 5. CarrierOffset
		for(int n = 0; n < CARRIER_OFFSET_MAX_COUNT; n++)
		{
			pParamSet->CarrierOffset[n].dHosit = 0.0;
			pParamSet->CarrierOffset[n].dShift = 0.0;
		}
        */
        pParamSet->CarrierType = CARRIER_FOUP;				
    }

    return bSuccess;
}


bool TransControl::saveParameter(char* szFilePath, TRANS_PARAM_SET* pParamSet)
{
	bool bResult = false;

	LIniFile *m_lWriteIniFile;
    m_lWriteIniFile = NULL;
	m_lWriteIniFile = new LIniFile(szFilePath);

    if(!m_lWriteIniFile->GetIniFileExist())
    {
		delete m_lWriteIniFile;
		m_lWriteIniFile = NULL;
	}

	if(m_lWriteIniFile != NULL)
	{
		m_lWriteIniFile->DeleteIniFile();

        // 1. Hoist Param

		m_lWriteIniFile->WriteString("Version","IniVersion", pParamSet->Version);

		m_lWriteIniFile->WriteDouble("HOIST", "AccelMax", pParamSet->HoistAxis.Limit.Accel.MaxValue);
		m_lWriteIniFile->WriteDouble("HOIST", "AccelMin", pParamSet->HoistAxis.Limit.Accel.MinValue);
		m_lWriteIniFile->WriteDouble("HOIST", "DecelMax", pParamSet->HoistAxis.Limit.Decel.MaxValue);
		m_lWriteIniFile->WriteDouble("HOIST", "DecelMin", pParamSet->HoistAxis.Limit.Decel.MinValue);
		m_lWriteIniFile->WriteDouble("HOIST", "SpeedMax", pParamSet->HoistAxis.Limit.Speed.MaxValue);
		m_lWriteIniFile->WriteDouble("HOIST", "SpeedMin", pParamSet->HoistAxis.Limit.Speed.MinValue);
		m_lWriteIniFile->WriteDouble("HOIST", "dAbnormalMoveSpeedMargin", pParamSet->HoistAxis.Limit.dAbnormalMoveSpeedMargin);
		m_lWriteIniFile->WriteDouble("HOIST", "PositionMax", pParamSet->HoistAxis.Limit.Position.MaxValue);
		m_lWriteIniFile->WriteDouble("HOIST", "PositionMin", pParamSet->HoistAxis.Limit.Position.MinValue);
		m_lWriteIniFile->WriteDouble("HOIST", "JogPositionMax", pParamSet->HoistAxis.Limit.JogPosition.MaxValue);
		m_lWriteIniFile->WriteDouble("HOIST", "JogPositionMin", pParamSet->HoistAxis.Limit.JogPosition.MinValue);
		m_lWriteIniFile->WriteDouble("HOIST", "dDefaultAccel", pParamSet->HoistAxis.Default.dDefaultAccel);
		m_lWriteIniFile->WriteDouble("HOIST", "dDefaultDecel", pParamSet->HoistAxis.Default.dDefaultDecel);
		m_lWriteIniFile->WriteDouble("HOIST", "dEmergencyDecel", pParamSet->HoistAxis.Default.dEmergencyDecel);

        // 2. Shift Param
//        m_lWriteIniFile->WriteDouble("SHIFT", "dIdealGearRatio", pParamSet->ShiftAxis.GearRatio.dIdealGearRatio);
//        m_lWriteIniFile->WriteDouble("SHIFT", "dIdeal2RealRatio", pParamSet->ShiftAxis.GearRatio.dIdeal2RealRatio);
//        m_lWriteIniFile->WriteDouble("SHIFT", "dHomeOffsetOfMTLToJigOnUserUnit", pParamSet->ShiftAxis.GearRatio.dDeviceDifference);
//        m_lWriteIniFile->WriteInteger("SHIFT", "dHomeSearchOffset", pParamSet->ShiftAxis.GearRatio.nHomeSearchCtsOffset);
		m_lWriteIniFile->WriteDouble("SHIFT", "AccelMax", pParamSet->ShiftAxis.Limit.Accel.MaxValue);
		m_lWriteIniFile->WriteDouble("SHIFT", "AccelMin", pParamSet->ShiftAxis.Limit.Accel.MinValue);
		m_lWriteIniFile->WriteDouble("SHIFT", "DecelMax", pParamSet->ShiftAxis.Limit.Decel.MaxValue);
		m_lWriteIniFile->WriteDouble("SHIFT", "DecelMin", pParamSet->ShiftAxis.Limit.Decel.MinValue);
		m_lWriteIniFile->WriteDouble("SHIFT", "SpeedMax", pParamSet->ShiftAxis.Limit.Speed.MaxValue);
		m_lWriteIniFile->WriteDouble("SHIFT", "SpeedMin", pParamSet->ShiftAxis.Limit.Speed.MinValue);
		m_lWriteIniFile->WriteDouble("SHIFT", "dAbnormalMoveSpeedMargin", pParamSet->ShiftAxis.Limit.dAbnormalMoveSpeedMargin);
		m_lWriteIniFile->WriteDouble("SHIFT", "PositionMax", pParamSet->ShiftAxis.Limit.Position.MaxValue);
		m_lWriteIniFile->WriteDouble("SHIFT", "PositionMin", pParamSet->ShiftAxis.Limit.Position.MinValue);
		m_lWriteIniFile->WriteDouble("SHIFT", "JogPositionMax", pParamSet->ShiftAxis.Limit.JogPosition.MaxValue);
		m_lWriteIniFile->WriteDouble("SHIFT", "JogPositionMin", pParamSet->ShiftAxis.Limit.JogPosition.MinValue);
		m_lWriteIniFile->WriteDouble("SHIFT", "dDefaultAccel", pParamSet->ShiftAxis.Default.dDefaultAccel);
		m_lWriteIniFile->WriteDouble("SHIFT", "dDefaultDecel", pParamSet->ShiftAxis.Default.dDefaultDecel);
		m_lWriteIniFile->WriteDouble("SHIFT", "dEmergencyDecel", pParamSet->ShiftAxis.Default.dEmergencyDecel);

        // 3. TRANS USE_SET
        // by zzang9un 2013.08.17 : 모든 Use 변수를 저장하도록 변경
        m_lWriteIniFile->WriteBool("USE_SET", "EQPIO", pParamSet->TransUsePart.EQPIO);
        m_lWriteIniFile->WriteBool("USE_SET", "GripperSen", pParamSet->TransUsePart.GripperSen);
        m_lWriteIniFile->WriteBool("USE_SET", "OscillationSen", pParamSet->TransUsePart.OscillationSen);
        m_lWriteIniFile->WriteBool("USE_SET", "HandPushSen", pParamSet->TransUsePart.HandPushSen);
		m_lWriteIniFile->WriteBool("USE_SET", "LookdownSen", pParamSet->TransUsePart.LookdownSen);
		m_lWriteIniFile->WriteBool("USE_SET", "ShutterControl", pParamSet->TransUsePart.ShutterControl);
        m_lWriteIniFile->WriteBool("USE_SET", "HandSensor", pParamSet->TransUsePart.HandSensor);
        m_lWriteIniFile->WriteBool("USE_SET", "HoistAlign", pParamSet->TransUsePart.HoistHomeSen);
        m_lWriteIniFile->WriteBool("USE_SET", "STBSen", pParamSet->TransUsePart.STBSen);
        m_lWriteIniFile->WriteBool("USE_SET", "UTBSen", pParamSet->TransUsePart.UTBSen);
        m_lWriteIniFile->WriteBool("USE_SET", "UTBFoupCheck", pParamSet->TransUsePart.UTBFoupCheck);
        m_lWriteIniFile->WriteBool("USE_SET", "IgnorePIOError", pParamSet->TransUsePart.IgnorePIOError);
		m_lWriteIniFile->WriteBool("USE_SET", "TransSound", pParamSet->TransUsePart.TransSound);
		m_lWriteIniFile->WriteBool("USE_SET", "ObsDetectSound", pParamSet->TransUsePart.ObsDetectSound);
		m_lWriteIniFile->WriteBool("USE_SET", "DrivingSound", pParamSet->TransUsePart.DrivingSound);
		m_lWriteIniFile->WriteInteger("USE_SET", "EQPIOType", pParamSet->TransUsePart.EQPIOType);
		m_lWriteIniFile->WriteBool("USE_SET", "OscillationSenPause", pParamSet->TransUsePart.OscillationSenPause);

		// 4. TRANS DEFINE
        /* // [BEGIN] by zzang9un 2013.08.18 : Default 값을 Ini 파일에 write하지 않는 방법은 주석 처리
		 // 디폴트 값을 알 수 없기 때문에 임시로 주석 처리 함
		 m_lWriteIniFile->WriteDiffInteger("DEFINE", "nTracker_Delay_Time", pParamSet->TransDefine.nTracker_Delay_Time, TRACKER_DELAY_TIME);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dHoist_Positive_Limit_OnWait", pParamSet->TransDefine.dHoist_Positive_Limit_OnWait, HOIST_POSITIVE_LIMIT_ON_WAIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dHoist_Negative_Limit_OnWait", pParamSet->TransDefine.dHoist_Negative_Limit_OnWait, HOIST_NEGATIVE_LIMIT_ON_WAIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_Positive_Limit_OnWait", pParamSet->TransDefine.dShift_Positive_Limit_OnWait, SHIFT_POSITIVE_LIMIT_ON_WAIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_Negative_Limit_OnWait", pParamSet->TransDefine.dShift_Negative_Limit_OnWait, SHIFT_NEGATIVE_LIMIT_ON_WAIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dHoist_Abnormal_Move_Speed_Limit", pParamSet->TransDefine.dHoist_Abnormal_Move_Speed_Limit, HOIST_ABNOMARL_MOVE_SPEED_LIMIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_Abnormal_Move_Speed_Limit", pParamSet->TransDefine.dShift_Abnormal_Move_Speed_Limit, SHIFT_ABNOMARL_MOVE_SPEED_LIMIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dHoist_MoveLimit_P", pParamSet->TransDefine.dHoist_MoveLimit_P, HOIST_MOVELIMIT_P);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dHoist_MoveLimit_N", pParamSet->TransDefine.dHoist_MoveLimit_N, HOIST_MOVELIMIT_N);
		 m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_MoveLimit_P", pParamSet->TransDefine.dShift_MoveLimit_P, SHIFT_MOVELIMIT_P);
		 m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_MoveLimit_N", pParamSet->TransDefine.dShift_MoveLimit_N, SHIFT_MOVELIMIT_N);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dDist_Hoist_Home_Margin", pParamSet->TransDefine.dDist_Hoist_Home_Margin, DIST_HOIST_HOME_MARGIN);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dDist_Shift_Home_Margin", pParamSet->TransDefine.dDist_Shift_Home_Margin, DIST_SHIFT_HOME_MARGIN);
         m_lWriteIniFile->WriteDiffInteger("DEFINE", "nUpstate_SpeedDown_Offset", pParamSet->TransDefine.nUpstate_SpeedDown_Offset, DEF_nUpstate_SpeedDown_Offset);
		 m_lWriteIniFile->WriteDiffInteger("DEFINE", "nDnState_UP_SpeedDown_Offset", pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset, DEF_nDnState_UP_SpeedDown_Offset);
         m_lWriteIniFile->WriteDiffInteger("DEFINE", "nDnState_DN_SpeedDown_Offset", pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset, DEF_nDnState_DN_SpeedDown_Offset);
         m_lWriteIniFile->WriteDiffInteger("DEFINE", "nLookdn_Detect_Timeover", pParamSet->TransDefine.nLookdn_Detect_Timeover, LOOKDN_DETECT_TIMEOVER);
		 m_lWriteIniFile->WriteDiffInteger("DEFINE", "nTrans_Timeover", pParamSet->TransDefine.nTrans_Timeover, TRANS_TIMEOVER);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dOscillation_Check_Dist", pParamSet->TransDefine.dOscillation_Check_Dist, OSCILLATIONCHECKDIST);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dHoist_MovInpos_Limit", pParamSet->TransDefine.dHoist_MovInpos_Limit, HOIST_MOVINPOS_LIMIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_Fast_Move_Dist", pParamSet->TransDefine.dShift_Fast_Move_Dist, SHIFT_FAST_MOVE_DIST);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dOscillation_Ignore_Dist", pParamSet->TransDefine.dOscillation_Ignore_Dist, OSCILLATION_IGNORE_DIST);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dSTB_Limit", pParamSet->TransDefine.dSTB_Limit, IS_STB_LIMIT);
		 m_lWriteIniFile->WriteDiffDouble("DEFINE", "dSTK_Limit", pParamSet->TransDefine.dSTK_Limit, IS_STK_LIMIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dFast_Move_Limit", pParamSet->TransDefine.dFast_Move_Limit, IS_FAST_MOVE_LIMIT);
         m_lWriteIniFile->WriteDiffInteger("DEFINE", "nSTBUTBSensorOFFTime", pParamSet->TransDefine.nSTBUTBSensorOFFTime, STBUTBSensorOFFTime);
         m_lWriteIniFile->WriteDiffInteger("DEFINE", "nSTBUTBSensorONTime", pParamSet->TransDefine.nSTBUTBSensorONTime, STBUTBSensorONTime);
         m_lWriteIniFile->WriteDiffInteger("DEFINE", "nRailsupport_Homing_TimeOut", pParamSet->TransDefine.nRailsupport_Homing_TimeOut, RAILSUPPORT_HOMING_TIMEOUT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_Speed_Ratio", pParamSet->TransDefine.dShift_Speed_Ratio, SHIFT_SPEED_RATIO);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_AccTime", pParamSet->TransDefine.dShift_AccTime, SHIFT_ACCTIME);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dHoist_Max_Speed", pParamSet->TransDefine.dHoist_Max_Speed, HOIST_MAX_SPEED);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dHoist_Pos_Limit", pParamSet->TransDefine.dHoist_Pos_Limit, HOIST_POS_LIMIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dHoist_Neg_Limit", pParamSet->TransDefine.dHoist_Neg_Limit, HOIST_NEG_LIMIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_Pos_Limit", pParamSet->TransDefine.dShift_Pos_Limit, SHIFT_POS_LIMIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_Neg_Limit", pParamSet->TransDefine.dShift_Neg_Limit, SHIFT_NEG_LIMIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dInpos_Limit", pParamSet->TransDefine.dInpos_Limit, INPOS_LIMIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_Pos_Limit_ForWait", pParamSet->TransDefine.dShift_Pos_Limit_ForWait, SHIFT_POS_LIMIT_FORWAIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dShift_Neg_Limit_ForWait", pParamSet->TransDefine.dShift_Neg_Limit_ForWait, SHIFT_NEG_LIMIT_FORWAIT);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dHoist_Pos_Limit_TransOffset", pParamSet->TransDefine.dHoist_Pos_Limit_TransOffset, HOIST_POS_LIMIT_TRANSOFFSET);
         m_lWriteIniFile->WriteDiffDouble("DEFINE", "dHoist_Origin_Limit", pParamSet->TransDefine.dHoist_Origin_Limit, HOIST_ORIGIN_LIMIT);
		 */ // [END] by zzang9un 2013.08.18 : Default 값을 Ini 파일에 write하지 않는 방법은 주석 처리

		m_lWriteIniFile->WriteDouble("DEFINE", "dHoist_Positive_Limit_OnWait", pParamSet->TransDefine.dHoist_Positive_Limit_OnWait);
        m_lWriteIniFile->WriteDouble("DEFINE", "dHoist_Negative_Limit_OnWait", pParamSet->TransDefine.dHoist_Negative_Limit_OnWait);
        m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Positive_Limit_OnWait", pParamSet->TransDefine.dShift_Positive_Limit_OnWait);
        m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Negative_Limit_OnWait", pParamSet->TransDefine.dShift_Negative_Limit_OnWait);
		m_lWriteIniFile->WriteDouble("DEFINE", "dHoist_Abnormal_Move_Speed_Limit", pParamSet->TransDefine.dHoist_Abnormal_Move_Speed_Limit);
        m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Abnormal_Move_Speed_Limit", pParamSet->TransDefine.dShift_Abnormal_Move_Speed_Limit);
        m_lWriteIniFile->WriteDouble("DEFINE", "dHoist_MoveLimit_P", pParamSet->TransDefine.dHoist_MoveLimit_P);
		m_lWriteIniFile->WriteDouble("DEFINE", "dHoist_MoveLimit_N", pParamSet->TransDefine.dHoist_MoveLimit_N);
		m_lWriteIniFile->WriteDouble("DEFINE", "dShift_MoveLimit_P", pParamSet->TransDefine.dShift_MoveLimit_P);
        m_lWriteIniFile->WriteDouble("DEFINE", "dShift_MoveLimit_N", pParamSet->TransDefine.dShift_MoveLimit_N);
        m_lWriteIniFile->WriteDouble("DEFINE", "dDist_Hoist_Home_Margin", pParamSet->TransDefine.dDist_Hoist_Home_Margin);
        m_lWriteIniFile->WriteDouble("DEFINE", "dDist_Shift_Home_Margin", pParamSet->TransDefine.dDist_Shift_Home_Margin);
		m_lWriteIniFile->WriteInteger("DEFINE", "nUpstate_SpeedDown_Offset", pParamSet->TransDefine.nUpstate_SpeedDown_Offset);
        m_lWriteIniFile->WriteInteger("DEFINE", "nDnState_UP_SpeedDown_Offset", pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset);
		m_lWriteIniFile->WriteInteger("DEFINE", "nDnState_DN_SpeedDown_Offset", pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset);
        //Load Unload 거리값 구분
		m_lWriteIniFile->WriteInteger("DEFINE", "nUpstate_SpeedDown_Offset_Load", pParamSet->TransDefine.nUpstate_SpeedDown_Offset_Load);
		m_lWriteIniFile->WriteInteger("DEFINE", "nDnState_UP_SpeedDown_Offset_Load", pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_Load);
		m_lWriteIniFile->WriteInteger("DEFINE", "nDnState_DN_SpeedDown_Offset_Load", pParamSet->TransDefine.nDnState_DN_SpeedDown_Offset_Load);

		//특수 Port 적용
//	 	m_lWriteIniFile->WriteInteger("DEFINE", "nDnState_UP_SpeedDown_Offset_SpecialEQ", pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_SpecialEQ);
//		m_lWriteIniFile->WriteInteger("DEFINE", "nDnState_UP_SpeedDown_Percent_SpecialEQ", pParamSet->TransDefine.nDnState_UP_SpeedDown_Percent_SpecialEQ);

		m_lWriteIniFile->WriteInteger("DEFINE", "nLookdn_Detect_Timeover", pParamSet->TransDefine.nLookdn_Detect_Timeover);
		m_lWriteIniFile->WriteDouble("DEFINE", "dLookdn_Detect_Check_Dist", pParamSet->TransDefine.dLookdn_Detect_Check_Dist);
        m_lWriteIniFile->WriteInteger("DEFINE", "nTrans_Timeover", pParamSet->TransDefine.nTrans_Timeover);
        m_lWriteIniFile->WriteDouble("DEFINE", "dOscillation_Check_Dist", pParamSet->TransDefine.dOscillation_Check_Dist);
        m_lWriteIniFile->WriteDouble("DEFINE", "dHoist_MovInpos_Limit", pParamSet->TransDefine.dHoist_MovInpos_Limit);
        m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Fast_Move_Dist", pParamSet->TransDefine.dShift_Fast_Move_Dist);
		m_lWriteIniFile->WriteDouble("DEFINE", "dOscillation_Ignore_Dist", pParamSet->TransDefine.dOscillation_Ignore_Dist);
        m_lWriteIniFile->WriteDouble("DEFINE", "dFast_Move_Limit", pParamSet->TransDefine.dFast_Move_Limit);
        m_lWriteIniFile->WriteDouble("DEFINE", "dFast_Move_Limit_Ver85h", pParamSet->TransDefine.dFast_Move_Limit_Ver85h);
        m_lWriteIniFile->WriteInteger("DEFINE", "nSTBUTBSensorOFFTime", pParamSet->TransDefine.nSTBUTBSensorOFFTime);
        m_lWriteIniFile->WriteInteger("DEFINE", "nSTBUTBSensorONTime", pParamSet->TransDefine.nSTBUTBSensorONTime);
		m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Speed_Ratio", pParamSet->TransDefine.dShift_Speed_Ratio);
        m_lWriteIniFile->WriteDouble("DEFINE", "dShift_AccTime", pParamSet->TransDefine.dShift_AccTime);
        m_lWriteIniFile->WriteDouble("DEFINE", "dHoist_Max_Speed", pParamSet->TransDefine.dHoist_Max_Speed);
        m_lWriteIniFile->WriteDouble("DEFINE", "dHoist_Pos_Limit", pParamSet->TransDefine.dHoist_Pos_Limit);
        m_lWriteIniFile->WriteDouble("DEFINE", "dHoist_Neg_Limit", pParamSet->TransDefine.dHoist_Neg_Limit);
        m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Pos_Limit", pParamSet->TransDefine.dShift_Pos_Limit);
        m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Neg_Limit", pParamSet->TransDefine.dShift_Neg_Limit);
        m_lWriteIniFile->WriteDouble("DEFINE", "dInpos_Limit", pParamSet->TransDefine.dInpos_Limit);
        m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Pos_Limit_ForWait", pParamSet->TransDefine.dShift_Pos_Limit_ForWait);
        m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Neg_Limit_ForWait", pParamSet->TransDefine.dShift_Neg_Limit_ForWait);
        m_lWriteIniFile->WriteDouble("DEFINE", "dHoist_Pos_Limit_TransOffset", pParamSet->TransDefine.dHoist_Pos_Limit_TransOffset);
        m_lWriteIniFile->WriteDouble("DEFINE", "dHoist_Origin_Limit", pParamSet->TransDefine.dHoist_Origin_Limit);


// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate 관련 Interlock 추가
//#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
//		m_lWriteIniFile->WriteInteger("DEFINE", "nRotate_Exec_Timeout", pParamSet->TransDefine.nRotate_Exec_Timeout);
//
//#endif
       	m_lWriteIniFile->WriteInteger("DEFINE", "nDnState_DN_SpeedDown_Speed_Load", pParamSet->TransDefine.nDnState_DN_SpeedDown_Speed_Load);


		m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Speed_Ratio_50Down_Load", pParamSet->TransDefine.dShift_Speed_Ratio_50Down_Load);
		m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Speed_Ratio_50Up_Load", pParamSet->TransDefine.dShift_Speed_Ratio_50Up_Load);
		m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Speed_Ratio_50Down_UnLoad", pParamSet->TransDefine.dShift_Speed_Ratio_50Down_UnLoad);
		m_lWriteIniFile->WriteDouble("DEFINE", "dShift_Speed_Ratio_50Up_UnLoad", pParamSet->TransDefine.dShift_Speed_Ratio_50Up_UnLoad);
		m_lWriteIniFile->WriteDouble("DEFINE", "dHandPushRetryPos", pParamSet->TransDefine.dHandPushRetryPos);

	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveUp_HomeOffset_Load", pParamSet->TransDefine.nHoistMoveUp_HomeOffset_Load);
	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveUp_TeachingOffset_Load", pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_Load);
	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveDown_TeachingOffset_Load", pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_Load);
	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveUp_HomeOffset_Unload", pParamSet->TransDefine.nHoistMoveUp_HomeOffset_Unload);
	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveUp_TeachingOffset_Unload", pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_Unload);
	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveDown_TeachingOffset_Unload", pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_Unload);
	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveUp_HomeOffset_STB_Load", pParamSet->TransDefine.nHoistMoveUp_HomeOffset_STB_Load);
	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveUp_TeachingOffset_STB_Load", pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_STB_Load);
	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveDown_TeachingOffset_STB_Load", pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_STB_Load);
	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveUp_HomeOffset_STB_Unload", pParamSet->TransDefine.nHoistMoveUp_HomeOffset_STB_Unload);
	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveUp_TeachingOffset_STB_Unload", pParamSet->TransDefine.nHoistMoveUp_TeachingOffset_STB_Unload);
	m_lWriteIniFile->WriteInteger("DEFINE", "nHoistMoveDown_TeachingOffset_STB_Unload", pParamSet->TransDefine.nHoistMoveDown_TeachingOffset_STB_Unload);

		// 5. CarrierOffset
        char szHoist[8];
        memset(szHoist, 0, sizeof(szHoist));
        memcpy(szHoist, "Hoist_", 6);
        char szShift[8];
        memset(szShift, 0, sizeof(szShift));
        memcpy(szShift, "Shift_", 6);
        for(int n = 0; n < CARRIER_OFFSET_MAX_COUNT; n++)
        {
			szHoist[6] = n + '0';
            szShift[6] = n + '0';
			m_lWriteIniFile->WriteDiffDouble("CARRIER", szHoist, pParamSet->CarrierOffset[n].dHosit, 0.0);
			m_lWriteIniFile->WriteDiffDouble("CARRIER", szShift, pParamSet->CarrierOffset[n].dShift, 0.0);
		}

        m_lWriteIniFile->WriteInteger("CARRIER_TYPE", "CarrierType", pParamSet->CarrierType);

        delete m_lWriteIniFile;
    }

}

bool TransControl::updateParameter()
{
    TRANS_PARAM_SET* pTmpParamSet;

	if(m_pTmpParamSet != NULL)
    {
        pTmpParamSet = m_pParamSet;
		m_pParamSet = m_pTmpParamSet;
		m_pTmpParamSet = NULL;
        delete pTmpParamSet;
        return true;
    }
    else
    {
        return false;
    }
}

/**
 @brief   파라미터 로드
 @author  임태웅
 @date    2013.05.30
 @return  성공 하면 파라미터 포인터 리턴, 실패하면 NULL 리턴
 */
void* TransControl::LoadParam(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory)
{
    bool bSuccess = false;
    TRANS_PARAM_SET* pTmpParamSet = new TRANS_PARAM_SET();

    // 2 Back Up
    saveParameter(BackUpFileNameAndDirectory, m_pParamSet);

    // 2 Load
    bSuccess = loadParameter(NewFileNameAndDirectory, pTmpParamSet);

	// 2 성공 실패 여부 확인
    if(bSuccess == true)
    {
    }
    else
    {
		delete pTmpParamSet;
        pTmpParamSet = NULL;
    }

    return (void*)pTmpParamSet;

}

/*
	이 함수는 위 것이 원래 있던 부분이며, 읽어보고 새로이 만든것이 아래 부분이다.

	(새로 만든이유)
	(1) m_pParamSet 은 public 으로 있기 때문에 여기에 바로 로딩해야 외부 모듈에서 참조하는 경우 반영될 수 있다
	(2) pTmpParamSet 은 의미 없다
	(3) LoadParameter() 를 실제 호출하는 부분을 보면 ( TransControl()  ) EQPIO 관련 값을 읽어야 한다
*/
bool TransControl::LoadParam_New(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory)
{
	bool bSuccess = false;

	// Back Up
	saveParameter(BackUpFileNameAndDirectory, m_pParamSet);

	// Load
	bSuccess = loadParameter(NewFileNameAndDirectory, m_pParamSet);

	if(bSuccess == true)
	{
		OHTMainForm->Set_EQPIO_Type(m_pParamSet->TransUsePart.EQPIOType); //혼용 PIO
	}

	return bSuccess;
}

/**
 @brief   파라미터 업데이트 요청
 @author  임태웅
 @date    2013.05.30
 @return  성공 하면 파라미터 포인터 리턴, 실패하면 NULL 리턴
 */
bool TransControl::RequestParamUpdate(void* pParameter)
{
    if(pParameter != NULL)
    {
        m_pTmpParamSet = (TRANS_PARAM_SET*)pParameter;
        return true;
    }
    else
    {
        return false;
    }
}
/**
 @brief   파라미터 업데이트 요청
 @author  임태웅
 @date    2013.05.30
 @return  파라미터가 포인터 리턴, 실패하면 NULL 리턴
 */
bool TransControl::IsParamUpdated()
{
	if(m_pTmpParamSet == NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}
// ------------------------------------------------------------------------------

/**
 @brief   Use 파라미터를 변경하고 그 내용을 파라미터에 저장하는 함수
 @author  zzang9un
 @date    2013.08.18
 @return  정상인 경우 0, 에러인 경우 에러 코드 리턴
 */
int TransControl::ChangeUse(int UseIndex, bool YesOrNo)
{
	int Error = NO_ERR;

    switch(UseIndex)
    {
    case USE_SET_BCR:
        break;

    case USE_SET_PBSSENSOR:
        break;

	case USE_SET_SOUNDALARM:
        break;

    case USE_SET_LAMPINDI:
		break;
	case USE_SET_SHUTTERCONTROL:
		m_pParamSet->TransUsePart.ShutterControl = YesOrNo;
		break;

    case USE_SET_EQPIO:
        m_pParamSet->TransUsePart.EQPIO = YesOrNo;
		break;

	case USE_SET_HANDLIMITSENSOR:
        break;

    case USE_SET_HANDCONTROL:
        m_pParamSet->TransUsePart.HandSensor = YesOrNo;
        break;

    case USE_SET_STEERINGCONTROL:
        break;

    case USE_SET_LOOKDOWNSENSOR:
        m_pParamSet->TransUsePart.LookdownSen = YesOrNo;
        break;

    case USE_SET_HANDPUSHSENCHK:
        m_pParamSet->TransUsePart.HandPushSen = YesOrNo;
        break;

    case USE_SET_GRIPPERSEN:
        m_pParamSet->TransUsePart.GripperSen = YesOrNo;
        break;

    case USE_SET_HOISTHOMESENSOR:
		m_pParamSet->TransUsePart.HoistHomeSen = YesOrNo;
        break;

    case USE_SET_OSCILLATIONSEN:
		m_pParamSet->TransUsePart.OscillationSen = YesOrNo;
        break;

    case USE_SET_ERRSOUND:
		break;

	case USE_SET_DRIVINGSOUND:
		m_pParamSet->TransUsePart.DrivingSound = YesOrNo;
        break;

    case USE_SET_TRANSSOUND:
		m_pParamSet->TransUsePart.TransSound = YesOrNo;
        break;

    case USE_SET_OBSDETECTSOUND:
        m_pParamSet->TransUsePart.ObsDetectSound = YesOrNo;
        break;

	case USE_SET_PIOIGNOREERROR:
		m_pParamSet->TransUsePart.IgnorePIOError = YesOrNo;
        break;

    case USE_SET_HOISTHOME_PIOERR:
        break;

    case USE_SET_NEXTMOTION_34AXIS:
        break;

    case USE_SET_LOCSAVE_TRANS_ING:
		break;

	case USE_SET_PUSHABNORMALONCHK:
        break;

    case USE_SET_CROSSRAILINTERLOCK:
        break;

	case USE_SET_INNERFOUPDETECTSEN:
        break;

	case USE_SET_OBSIGNORESOUND:
		break;




// 2016-12-05, Add by Labeler, ROHT의 경우, UTB, STB 미사용으로 별도처리
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
    case USE_SET_UTBSEN:
    case USE_SET_UTBFOUPCHECK:
	case USE_SET_STBSEN:
//	case USE_SET_OUTTRIGGER:
	case USE_SET_OSCILLATION_PAUSE:
		break;
#else
    case USE_SET_UTBSEN:
        m_pParamSet->TransUsePart.UTBSen = YesOrNo;
        break;

    case USE_SET_UTBFOUPCHECK:
        m_pParamSet->TransUsePart.UTBFoupCheck = YesOrNo;
        break;

    case USE_SET_STBSEN:
        m_pParamSet->TransUsePart.STBSen = YesOrNo;
		break;


	case USE_SET_OSCILLATION_PAUSE:
		m_pParamSet->TransUsePart.OscillationSenPause = YesOrNo;
		break;
#endif
    default:
        Error = ERR_USE_NOEXIST;
        break;
    }

    if(Error == NO_ERR)
    {
		saveParameter("DataFiles\\Trans.ini", m_pParamSet);
		ADD_LOG("[Param] Save Use Param, Index:%d, OnOff:%d", UseIndex, YesOrNo);
    }

	return Error;
}

TRANSE_DIFFERENCE TransControl::getDifference()
{
    TRANSE_DIFFERENCE TransDifference;
	if(m_defualtparam != NULL)
    {
		TransDifference.dHoist = m_defualtparam->HoistDefault.dDeviceDifference;
		TransDifference.dShift = m_defualtparam->ShiftDefault.dDeviceDifference;
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
		TransDifference.dRotate = m_defualtparam->RotateDefault.nDeviceDifference;
		TransDifference.dRotate_90 = m_defualtparam->RotateDefault.nDeviceDifference_90;
		TransDifference.dRotate_180 = m_defualtparam->RotateDefault.nDeviceDifference_180;
#endif
	}
	else
    {
        TransDifference.dHoist = 320;
        TransDifference.dShift = 0;
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
		TransDifference.dRotate = 0;
		TransDifference.dRotate_90 = 0;
		TransDifference.dRotate_180 = 0;
#endif
    }
    return TransDifference;
}

/**
 @brief   PIO나 이중반입, 공반송 에러 관련 Auto Recovery 실행 함수
 @author  이승준
 @date    2013.07.23
 @param   Error_AV : AutoRecovery를 하게 되는 Error Code
 @return  함수 실행 시 문제 없으면 0, 문제가 발생되면 해당 에러 코드 리턴
 */
int TransControl::AutoRecovery_Trans(int Error_AV, int nStep, int RecoveryType)
{
	int nError = NO_ERR;
    double dVel_mmpermsec = 0.0;
    double dAccel_mmpermsec2 = 0.0;
    DWORD dwStartTime = 0;


	if(m_pHWSet->m_pShiftAxis->IsRunning()) // Teaching point로 명령을 받고 움직이는 경우
	{
		dwStartTime = timeGetTime();
		do{
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 5000) break;
			Sleep(10);
		}
		while(m_pHWSet->m_pShiftAxis->IsRunning());
	}
	m_pHWSet->m_pShiftAxis->MoveStop(SHIFT_ESTOP_DECEL); // Teaching point로 명령은 받았으나 아직 motor가 움직이지 않은 경우

	Sleep(20);


    if(fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) > INPOS_LIMIT)
	{
        dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_30P;
		dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;
        if(nError == NO_ERR)
            nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);
        dwStartTime = timeGetTime();
        while(1)
        {
            if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 5000)
                break;
			if((fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) <= INPOS_LIMIT) && (m_pHWSet->m_pShiftAxis->IsRunning()==false))
				break;
        }
	}



	if(nError == NO_ERR)
	{
		if(RecoveryType == AUTORECOVERYTYPE_E84)
		{

			if(m_nTransType == TRANSTYPE_LOAD)
			{
				if((Error_AV >= ERR_PIO_READY_ABNORMAL_ON_TD1) && (Error_AV <= ERR_PIO_READY_ABNORMAL_OFF_TA2))
				{

					nError = m_pHWSet->m_pShutter->Close();

					if(nError != NO_ERR)
					{
						ADD_LOG("PIO AR[E=%d] : Shutter Close", nError);
					}
				}
				else
				{
					Sleep(10);
					nError = Check_Complete();
				}
			}

			if(m_nTransType == TRANSTYPE_UNLOAD)
			{
				if(!((Error_AV >= ERR_PIO_READY_ABNORMAL_ON_TD1) && (Error_AV <= ERR_PIO_READY_ABNORMAL_OFF_TA2)))
				{

					nError = m_pHWSet->m_pShutter->Close();

					Sleep(10);
					nError = Check_Complete();

					if(nError != NO_ERR)
					{
						ADD_LOG("PIO AR[E=%d] : Shutter Close", nError);
					}
				}
			}

			m_pHWSet->m_pLeftEQPIO->PIODisable(true);
//			m_pHWSet->m_pRightEQPIO->PIODisable(true);

		}
		else if(RecoveryType ==AUTORECOVERYTYPE_BLANK)
		{
        	nError = m_pHWSet->m_pShutter->Open();
		}
		else if(RecoveryType ==AUTORECOVERYTYPE_DOUBLE)
		{
			nError = m_pHWSet->m_pShutter->Close();

		}
	}

	if(nError == NO_ERR)
	{

		ADD_LOG("PIO&B/D AR[Step=%d][E:%d][Type:%d] : Status Change(STS_AUTORECOVERY_XX)", m_nTransStep, Error_AV,RecoveryType);
		if(RecoveryType ==AUTORECOVERYTYPE_BLANK)
		{
			m_StatusSet.byStatus = STS_AUTORECOVERY_BLANKTRANS;
			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_EMPTY); //AOHC-196 사전 로그 추가
        }
		else if(RecoveryType ==AUTORECOVERYTYPE_DOUBLE)
		{
			m_StatusSet.byStatus = STS_AUTORECOVERY_DOUBLETRANS;
			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_DOUBLE); //AOHC-196 사전 로그 추가
		}
		else
		{
			m_StatusSet.byStatus = STS_AUTORECOVERY;
			m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_E84TM); //AOHC-196 사전 로그 추가

		}

			m_StatusSet.uError = Error_AV;
			m_StatusSet.bIsOnProcess = false;
			TransCMDExuteFunc(false);

    }
    else
		ADD_LOG("PIO&B/D AR[E=%d] : Fail", nError);

    return nError;
}

/**
 @brief   해당 클래스 Init 시 재하 상태 확인 함수
 @author  이승준
 @date    2013.11.11
 @param
 @return
 */
void TransControl::CheckCarrierInterlock(void)
{
	// [ HAND ] //

	bool bHandOpen = m_pHWSet->m_pHand->IsOpen();
	bool bHandClose = m_pHWSet->m_pHand->IsClose();
	bool bRightGripOn = m_pHWSet->m_pHand->IsRightGripOn();
	bool bLeftGripOn = m_pHWSet->m_pHand->IsLeftGripOn();

	// [ FOUP ] //
	bool bInnerFoupDetect = m_pHWSet->m_pInnerFoup->IsFoupDetect();

	if( bHandClose && !bRightGripOn && !bLeftGripOn && bInnerFoupDetect )
	{
		//m_StatusSet.uError = ERR_INNER_FOUP_DETECT;
	}
	if( (m_StatusSet.uError == NO_ERR) && ( bHandClose && bRightGripOn && bLeftGripOn && !bInnerFoupDetect ) )
	{
		m_StatusSet.uError = ERR_INNER_FOUP_NOT_DETECT;
	}
	if( (m_StatusSet.uError == NO_ERR) && ( bHandOpen && bRightGripOn && bLeftGripOn ) )
	{
		//if( bInnerFoupDetect )
		   //	m_StatusSet.uError = ERR_INNER_FOUP_DETECT;
		//else
		//{
			m_StatusSet.uError = ERR_GRIPPER_SEN_ON_IN_HAND_OPEN;
			ADD_FDC_LOG("ERR/%d/3/11/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d/%d/%d",
										ERR_GRIPPER_SEN_ON_IN_HAND_OPEN,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										m_nTransType,
										m_nTransStep,
										m_pHWSet->m_pHand->IsOpen(),
										m_pHWSet->m_pHand->IsClose(),
										m_pHWSet->m_pHand->IsLeftGripOn(),
										m_pHWSet->m_pHand->IsRightGripOn());
		//}
	}
	if( (m_StatusSet.uError == NO_ERR) && ( bHandOpen && !bRightGripOn && !bLeftGripOn && bInnerFoupDetect) )
	{
		//m_StatusSet.uError = ERR_INNER_FOUP_DETECT;
	}
}

/**
 @brief   Rotate Retry 동작 함수
 @author  Labeler
 @date    2016.12.07
 @return  동작 중 Error 발생 시 해당 Error return
          미 발생 시 NO_ERR return
*/
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
int TransControl::rotateRetry()
{
	int nError = NO_ERR;
	DWORD dwStartTime = 0;

	if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsAMPEnable() != true))
	{
		dwStartTime = timeGetTime();
		nError = m_pHWSet->m_pRotate->Enable();

		if(nError == NO_ERR)
		{
			while(m_pHWSet->m_pRotate->IsAMPEnable() != true)
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 2000)
				{
					nError = ERR_ROTATE_EXEC_TIMEOVER;
					break;
				}

				if(m_StatusSet.uError != NO_ERR)
				{
					nError = m_StatusSet.uError;
					break;
				}

				Sleep(10);
			}
		}
	}

	if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsOrigin() != true))
	{
        // Rotate 동작은 Shutter Open 상태에서만 가능
    	if((m_pHWSet->m_pShutter->IsOpen(FRONT) != true) || (m_pHWSet->m_pShutter->IsOpen(REAR) != true))
    	{
    		nError = ERR_SHUTTER_NOTORIGIN;
    	}
        else if(m_pHWSet->m_pHoistAxis->IsHoistHome() != true)
        {
            nError = ERR_HOISTALIGNSEN_NOT_ON;
        }
        else if((m_nTransType == TRANSTYPE_LOAD)
        && ((m_pHWSet->m_pHand->IsLeftGripOn() != true) || (m_pHWSet->m_pHand->IsRightGripOn() != true)))
        {
            nError = ERR_GRIP_SEN_MISMATCH;
        }
        else
        {
    		dwStartTime = timeGetTime();
    		nError = m_pHWSet->m_pRotate->Zeroing();
        }


       	if(nError == NO_ERR)
		{
            // 2016-12-12, Add by Labeler
            // Rotate Origin 명령 발생 후 Running 상태 되기 까지 시간이 걸려, 1초간 대기 Logic 추가
			while(m_pHWSet->m_pRotate->IsRunning() != true)
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 1000)
				{
					nError = ERR_ROTATE_EXEC_TIMEOVER;
					break;
				}

				if(m_StatusSet.uError != NO_ERR)
				{
					nError = m_StatusSet.uError;
					break;
				}
				Sleep(10);
			}
		}

		if(nError == NO_ERR)
		{
            dwStartTime = timeGetTime();

			while((m_pHWSet->m_pRotate->IsOrigin() != true) || (m_pHWSet->m_pRotate->IsRunning() == true))
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 2000)
				{
					nError = ERR_ROTATE_EXEC_TIMEOVER;
					break;
				}

				if(m_StatusSet.uError != NO_ERR)
				{
					nError = m_StatusSet.uError;
					break;
				}
				Sleep(10);
			}
		}
	}

	return nError;
}

#endif

//Auto Teaching 관련 함수
int TransControl::ReadyTrans_AutoTeaching()
{
	int nError = 0;
	DWORD dwStartTime = 0;

    // set hoist limit
    if(nError == NO_ERR)
		nError = m_pHWSet->m_pHoistAxis->SetPositiveSWLimit(fabs(m_dHoist_TeachingPos) + HOIST_POS_LIMIT_TRANSOFFSET);

    if(nError == NO_ERR)
        nError = m_pHWSet->m_pHoistAxis->SetNegativeSWLimit(HOIST_NEG_LIMIT);

	if(nError == NO_ERR)
    {
        if(m_dShift_TeachingPos >= 0.0)
        {
            nError = m_pHWSet->m_pShiftAxis->SetPositiveSWLimit(m_dShift_TeachingPos + SHIFT_POS_LIMIT_FORWAIT);

            if(nError == NO_ERR)
                nError = m_pHWSet->m_pShiftAxis->SetNegativeSWLimit(SHIFT_NEG_LIMIT_FORWAIT);
        }
		else
        {
            nError = m_pHWSet->m_pShiftAxis->SetPositiveSWLimit(SHIFT_POS_LIMIT_FORWAIT);
			if(nError == NO_ERR)
                nError = m_pHWSet->m_pShiftAxis->SetNegativeSWLimit(m_dShift_TeachingPos + SHIFT_NEG_LIMIT_FORWAIT);
        }
    }

	// -------------------------------------------
//	double dHoistPositive = 0.0;
//	double dHoistNegative = 0.0;
//	double dShiftPositive = 0.0;
//	double dShiftNegative = 0.0;
//
//	m_pHWSet->m_pHoistAxis->GetSWLimit(dHoistPositive, dHoistNegative);
//	m_pHWSet->m_pShiftAxis->GetSWLimit(dShiftPositive, dShiftNegative);
//	ADD_LOG("[H_P : %lf, H_N : %lf, S_P : %lf, S_N : %lf]", dHoistPositive, dHoistNegative, dShiftPositive, dShiftNegative);
	// -------------------------------------------


	if(nError == NO_ERR)
	{
		if(m_pHWSet->m_pShiftAxis->IsShiftHome() == false)
		{
			if((fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < INPOS_LIMIT))
			{
				nError = ERR_SHIFT_HOMESEN_NOTON;
				ADD_LOG("[ReadyTrans] E : %d(ERR_SHIFT_HOMESEN_NOTON)", nError);
			}
		}
	}


	// shutter check
	if(nError == NO_ERR)
	{
		if(m_TransUse.ShutterControl == true)
		{
			if((m_pHWSet->m_pShutter->IsOpen(FRONT) != m_pHWSet->m_pShutter->IsOpen(REAR)) ||
				(m_pHWSet->m_pShutter->IsClose(FRONT) != m_pHWSet->m_pShutter->IsClose(REAR)))
			{
				nError = ERR_SHUTTER_DIFFSTATUS;
			}

			if(nError != NO_ERR)
			{
				nError = 0;
				dwStartTime = timeGetTime();
				if(!m_defualtparam->PreShutter)
				{
					while(m_pHWSet->m_pShutter->IsOpen(FRONT) != m_pHWSet->m_pShutter->IsOpen(REAR))
					{
						if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)SHUTTER_MOTION_LIMIT_TIME)
							break;
						Sleep(10);
					}
				}else
				{
					while(!((m_pHWSet->m_pShutter->IsOpen(FRONT) == true) &&
						  (m_pHWSet->m_pShutter->IsOpen(REAR)==true)))
					{
						if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)SHUTTER_MOTION_LIMIT_TIME)
							break;
						Sleep(10);
					}
				}
				if((m_pHWSet->m_pShutter->IsOpen(FRONT) != m_pHWSet->m_pShutter->IsOpen(REAR)) ||
					(m_pHWSet->m_pShutter->IsClose(FRONT) != m_pHWSet->m_pShutter->IsClose(REAR)))
				{
					nError = ERR_SHUTTER_DIFFSTATUS;
					ADD_LOG("[ReadyTrans] E : %d(ERR_SHUTTER_DIFFSTATUS)", nError);
				}
			}
		}
	}


	// hand check
	if(nError == NO_ERR)
	{
		if(m_TransUse.HandSensor == true)
		{
			if(m_pHWSet->m_pHand->IsOpen() == m_pHWSet->m_pHand->IsClose())
            {
                nError = m_pHWSet->m_pHand->Origin();
                if(nError)
				{
					if((m_pHWSet->m_pHand->IsOpen() == true) && (m_pHWSet->m_pHand->IsClose() == true))
					{
						nError = ERR_HAND_LIMIT_SEN_ALL_ON;
						ADD_LOG("[ReadyTrans] E : %d(ERR_HAND_LIMIT_SEN_ALL_ON)", nError);
					}
					if((m_pHWSet->m_pHand->IsOpen() == false) && (m_pHWSet->m_pHand->IsClose() == false))
                    {
                        if(m_bSlipringFlag)
                        {
                            m_bSlipringFlag = false;
							nError = ERR_SEN_ABNORMAL_HAND;
							ADD_LOG("[ReadyTrans] E : %d(ERR_SEN_ABNORMAL_HAND)", nError);
                            ADD_LOG("[ReadyTrans] E : ERR_HAND_LIMIT_SEN_ALL_OFF)", nError);
                        }
                        else
                        {
                            nError = ERR_HAND_LIMIT_SEN_ALL_OFF;
                            ADD_LOG("[ReadyTrans] E : %d(ERR_HAND_LIMIT_SEN_ALL_OFF)", nError);
                        }
                    }
                }
                else
                    Sleep(500);
            }
            if(nError == NO_ERR)
            {
                if(m_pHWSet->m_pHand->IsInposition() == false)
				{
					if(m_pHWSet->m_pHand->IsClose() == true)
                    {
                        nError = m_pHWSet->m_pHand->Origin();
                        if(nError)
                        {
                            nError = ERR_HAND_INPOSITION_ERROR;
							ADD_LOG("[ReadyTrans] E : %d(ERR_HAND_INPOSITION_ERROR)", nError);
                        }
                        else
                            Sleep(500);
                    }
                    else
                    {
						if((m_pHWSet->m_pHand->IsRightGripOn() == false) &&
							(m_pHWSet->m_pHand->IsLeftGripOn() == false))
                        {
                            nError = m_pHWSet->m_pHand->Origin();
                            if(nError)
                            {
                                nError = ERR_HAND_INPOSITION_ERROR;
                                ADD_LOG("[ReadyTrans] E : %d(ERR_HAND_INPOSITION_ERROR)", nError);
                            }
                            else
                                Sleep(500);
                        }
                        else
                        {
                            nError = ERR_HAND_INPOSITION_ERROR;
                            ADD_LOG("[ReadyTrans] E : %d(ERR_HAND_INPOSITION_ERROR)", nError);
                        }
                    }
                }
            }

            if(nError == NO_ERR)
			{
                if(m_pHWSet->m_pHand->IsAlarm() == true)
                {
                    nError = m_pHWSet->m_pHand->AlarmReset();
                }
            }
        }
    }

	if(nError == NO_ERR)
	{
		if((m_pHWSet->m_pShutter->IsClose(FRONT) == true) || (m_pHWSet->m_pShutter->IsClose(REAR) == true))
		{

			nError = m_pHWSet->m_pShutter->SendOpenCmd();

			ADD_LOG("[ReadyTrans] E : %d(Shutter Open)", nError);
			// RailSupport 동작
			if(nError == NO_ERR)
			{

				while ((nError = m_pHWSet->m_pShutter->CheckComplete()) == NEXT_CHECK)
				{
					Sleep(10);
				}

			}
		}
	}

	if(nError == NO_ERR)
	{

		//기본값 입력
	   if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
	   {
//			if(m_nEqType == PORT_TYPE_STB_L)
//			{
//				m_dShift_TeachingPos -= m_AutoTeaching->dSTB_L_Offset;
////				m_dHoist_TeachingPos = STB_L_HOIST_DIS;
//			}
//			else
//			{
//				m_dShift_TeachingPos -= m_AutoTeaching->dSTB_R_Offset;
////				m_dHoist_TeachingPos = STB_R_HOIST_DIS;
//            }

    		m_AutoTeaching->bSTBTeaching = true;
	   }
	   else
	   {
		   m_AutoTeaching->bSTBTeaching = false;
		   m_dHoist_TeachingPos -= DEFAULT_OFFSET_EQ;
	   }

		m_dHoist_TeachingPos -=  DEFAULT_OFFSET;
       	if(m_dHoist_TeachingPos < 5 )
			nError = ERR_HOIST_N_ERROR_AUTOTEACHING;



	}
	return nError;

}
int TransControl::Check_State_AutoTeaching()
{
    int nError = 0;
    int nCount = 0;

	DWORD dwStartTime = 0;
	bool bIsSenON = false;
    double dInposLimit = INPOS_LIMIT;

    switch(m_nTransType)
    {
    case TRANSTYPE_LOAD:

// 2016-11-30, Labeler

        // -------------------------------------------
        // [BEGIN] by zzang9un 013.08.05 : UTB 체크
        if(nError == NO_ERR)
        {
            if((m_nEqType == PORT_TYPE_UTB) && (m_TransUse.UTBSen == true))
            {
                // UTB Load시 이중반입 방지차원에서 Foup 없어도 몇번더 체크함.
                if(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect() == false) // Foup 없음
                {
                    nCount = 0;
                    while(nCount < 4)
                    {
                        if(m_pHWSet->m_pTrackBufferDetect->IsUTBFoupDetect()) // Foup 존재
                        {
                            nError = ERR_UTB_FOUP_EXIST;
							ADD_LOG("[Check_State] E : %d(ERR_UTB_FOUP_EXIST)", nError);
                            break;
                        }
                        nCount++;
                        Sleep(10);
                    }
                }
                else
                {
                    nError = ERR_UTB_FOUP_EXIST;
					ADD_LOG("[Check_State] E : %d(ERR_UTB_FOUP_EXIST)", nError);
                }
            }
        }
        // [END] by zzang9un 013.08.05 : UTB 체크
        // -------------------------------------------

        // -------------------------------------------
		// [BEGIN] by zzang9un 013.08.05 : STB 체크
// 2016-11-30, Labeler

        if((nError == NO_ERR) && ((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R)) && (m_TransUse.STBSen == true))
        {
            switch(m_nSTBDirection)
            {
            case STB_DIR_RIGHT:
                if(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect() == false)
                {
                    nCount = 0;
                    while(nCount < FOUP_DETECT_CNT)
                    {
                        if(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect())
						{
                            nCount = FOUP_DETECT_CNT;
                            nError = ERR_STB_FOUP_EXIST;
							ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_EXIST)", nError);
                        }
                        else
                        {
                            nCount++;
                            Sleep(FOUP_DETECT_TIME);
                        }
                    }
                }
                else
                {
					nError = ERR_STB_FOUP_EXIST;
					ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_EXIST)", nError);
                }
				break;
			case STB_DIR_LEFT:
                if(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect() == false)
                {
                    nCount = 0;
                    while(nCount < FOUP_DETECT_CNT)
                    {
                        if(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect())
                        {
                            nCount = FOUP_DETECT_CNT;
							nError = ERR_STB_FOUP_EXIST;
							ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_EXIST)", nError);
                        }
                        else
                        {
                            nCount++;
                            Sleep(FOUP_DETECT_TIME);
                        }
                    }
                }
                else
                {
					nError = ERR_STB_FOUP_EXIST;
					ADD_LOG("[Check_State] E : %d(ERR_STB_FOUP_EXIST)", nError);
                }
				break;
            }
        }
        // [END] by zzang9un 013.08.05 : STB 체크
        // -------------------------------------------


        // Loading시 원점에서 Hand Push Sensor ON이면 에러 발생
        if((nError == NO_ERR) && (m_TransUse.HandPushSen == true))
		{

			if(m_pHWSet->m_pHand->IsPushOn() == true)
			{
				nCount = 0;
				while(m_pHWSet->m_pHand->IsPushOn() == true)
				{
                    if(nCount > 4)
                        break;
                    nCount++;
                    Sleep(10);
                }

				if(m_pHWSet->m_pHand->IsPushOn() == true)
				{
					nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
					ADD_LOG("[Check_State] E : %d(ERR_HAND_PUSHSEN_ABNORMAL_ON)", nError);
				}
			}
		}


		if((nError == NO_ERR) && ((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true)) &&
			(m_pHWSet->m_pInnerFoup->IsFoupDetect() == false))
		{
            DWORD dwStartTime = 0;
            static int nCount = 0;
            dwStartTime = timeGetTime();
            while(1)
            {
                if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 500)
                    break;

                if(m_pHWSet->m_pInnerFoup->IsFoupDetect() == false)
                {
                    nCount++;
                    if(nCount > 5)
                        break;
                }
                Sleep(10);
            }

			if(nCount > 5)
			{
                nCount = 0;
                nError = ERR_INNER_FOUP_NOT_DETECT;
				ADD_LOG("[Check_State] E : %d(ERR_INNER_FOUP_NOT_DETECT)", nError);
            }
		}

		break;

	}

	if(nError == NO_ERR)
	{

		if(nError == NO_ERR)
		{
			if(((m_pHWSet->m_pShutter->IsClose(FRONT) == true) || (m_pHWSet->m_pShutter->IsClose(REAR) == true))
			&& ((m_nTransType == TRANSTYPE_UNLOAD)
			|| ((m_nTransType == TRANSTYPE_LOAD) && (m_pHWSet->m_pInnerFoup->IsFoupDetect() == true) &&
			(m_pHWSet->m_pHand->IsLeftGripOn() == true) && (m_pHWSet->m_pHand->IsRightGripOn() == true))))
				nError = m_pHWSet->m_pShutter->Open();
		}
	}

    switch(m_nTransType)
    {
    case TRANSTYPE_LOAD:
        if((nError == NO_ERR) && (m_TransUse.GripperSen == true))
        {
            if((m_pHWSet->m_pHand->IsRightGripOn() == false) || (m_pHWSet->m_pHand->IsLeftGripOn() == false))
            {

                if(m_bSlipringFlag)
                {
                    m_bSlipringFlag = false;
					nError = ERR_SEN_ABNORMAL_HAND;
					ADD_LOG("[Check_State] E : %d(ERR_SEN_ABNORMAL_HAND)", nError);
                    ADD_LOG("[Check_State]ERR_HAND_CAST_NOTEXIST)");
                }
                else
                {
                    nError = ERR_HAND_CAST_NOTEXIST;
				    ADD_LOG("[Check_State] E : %d(ERR_HAND_CAST_NOTEXIST)", nError);
                }

            }
        }
        if(nError == NO_ERR)
		{

			if(m_pHWSet->m_pHand->IsClose() == false)
            {
                nError = ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD;
				ADD_LOG("[Check_State] E : %d(ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD)", nError);
            }
        }
        if(nError == NO_ERR)
		{

			if(m_pHWSet->m_pHand->IsOpen() == true)
            {
                nError = ERR_HAND_OPEN_SEN_ON_BEFORE_LOAD;
                ADD_LOG("[Check_State] ERROR : %d(ERR_HAND_OPEN_SEN_ON_BEFORE_LOAD)", nError);
            }
        }
        break;
    }

    if((nError == NO_ERR) && (m_nTransType != TRANSTYPE_MOVET))
    {
		if(m_nTransType == TRANSTYPE_LOAD)
			dInposLimit = HOIST_HOME_INPOS_LIMIT + 2.0;
        else
            dInposLimit = HOIST_HOME_INPOS_LIMIT;

// 2016-12-08, Add by Labeler
// Hoist Home Sensor 관련 Interlock 변경
//		if((m_TransUse.HoistHomeSen == true) && (m_pHWSet->m_pHoistAxis->IsHoistHome() == false) &&
//            (fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > dInposLimit))

		if((m_TransUse.HoistHomeSen == true) &&
		((m_pHWSet->m_pHoistAxis->IsHoistHome() == false) || (fabs(m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) > dInposLimit)))
		{
            dwStartTime = timeGetTime();
            bIsSenON = false;
            while(1)
            {
                if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 3000) // 3sec
						break;

				if((m_pHWSet->m_pHoistAxis->IsHoistHome() == true) && (fabs(m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) < dInposLimit))
				{
                    bIsSenON = true;
                    break;
                }
                else
					Sleep(10);
            }
            if(bIsSenON == false)
            {
                nError = ERR_HOISTALIGNSEN_NOT_ON;
				ADD_LOG("[Check_State] E : %d(ERR_HOISTALIGNSEN_NOT_ON)", nError);
            }
        }
    }


    if((nError == NO_ERR) && (m_nTransType != TRANSTYPE_MOVET))
	{

		if(fabs(m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) > HOIST_ORIGIN_LIMIT)
        {
            nError = ERR_HOIST_NOTORIGIN;
			ADD_LOG("[Check_State] E : %d(ERR_HOIST_NOTORIGIN %f)", nError,m_pHWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition());
        }
	}

    if(nError == NO_ERR)
    {
		if(m_pHWSet->m_pHoistAxis->IsAMPEnable() == false)
        {
            nError = ERR_AXIS2_SERVO_OFFED;
			ADD_LOG("[Check_State] E : %d(ERR_AXIS2_SERVO_OFFED)", nError);
		}

		if((nError == NO_ERR) && (m_pHWSet->m_pShiftAxis->IsAMPEnable() == false))
		{
			nError = ERR_AXIS3_SERVO_OFFED;
			ADD_LOG("[Check_State] E : %d(ERR_AXIS3_SERVO_OFFED)", nError);
		}


// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate Origin 신호 처리 추가

	}


	return nError;

}
int TransControl::Check_Sensor_AutoTeaching(bool bLookDnCheck, int nHoistState, int nPreStep, int nNextStep)
{
   int nError = 0;

	bool bLookDnSenCheck = bLookDnCheck;
	static int nOsciCount = 0;
	static int nLoadPushCount = 0;
	static int nUnloadPushCount = 0;
	static int nHandPIOGoCount = 0;
	static int nHandPosCount = 0;
	double dOscillationCheckDist = 0.0;
	static int nshutterCount =0;

	// Lookdown Detect check
	if(m_TransUse.LookdownSen == true)
    {
        if(bLookDnSenCheck == true)
        {
			if(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) < m_pParamSet->TransDefine.dLookdn_Detect_Check_Dist)
                bLookDnSenCheck = false;
        }

        if(bLookDnSenCheck == true)
        {
            if(m_pHWSet->m_pLookdown->IsError() == true)
            {
                nError = ERR_LOOKDOWN_SENSOR_FAIL;
			}
            else if(m_pHWSet->m_pLookdown->IsDetect() == true)
            {
                if(bLookdown_Detected == false)
                {
					bLookdown_Detected = true;
					m_dwLookdn_STTime = timeGetTime();
                    nError = m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
                    if(m_TransUse.ObsDetectSound == true)
                        m_pHWSet->Sound_On(SETSOUND_LOOKDOWN);

					ADD_LOG("Lookdn ON[Step=%d]: cmdPos=%.02f curPos = %.02f", m_nTransStep, m_dCmdHoistPos,
                        m_pHWSet->m_pHoistAxis->GetCurrentPosition());
                }
                else
                {
                    if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLookdn_STTime) > (DWORD)m_pParamSet->TransDefine.nLookdn_Detect_Timeover)
                        nError = ERR_LOOKDOWN_DETECT;
				}
            }
            else // nLookDownSensor==OFF
            {
				if(bLookdown_Detected == true)   //  bLookdown_Detected  EQPIO 사용하는곳에 룩다운감지시 스탭넘어가는거 막아둘 것.
				{
                    if(m_pHWSet->m_pHoistAxis->IsRunning() == ON)
                        return nError;
					bLookdown_Detected = false;

                    // by zzang9un 2013.09.24 : Lookdown 해제 시 이적재 사운드로 변경
					if(m_nTransSound != 0)
                        m_pHWSet->Sound_On(SETSOUND_TRANS);
                    else
                        m_pHWSet->Sound_Off();

                    switch(nHoistState)
                    {
                    case HOIST_DOWN_STATE:
                        if(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > fabs(m_dCmdHoistPos))
                            m_nTransStep = nNextStep;
                        else
                            m_nTransStep = nPreStep;
                        break;
                    case HOIST_UP_STATE:
                        if(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) < fabs(m_dCmdHoistPos))
                            m_nTransStep = nNextStep;
                        else
							m_nTransStep = nPreStep;
                        break;
                    }
					ADD_LOG("Lookdn OFF1[Step=%d]: cmdPos=%.02f curPos = %.02f", m_nTransStep, m_dCmdHoistPos,
                        m_pHWSet->m_pHoistAxis->GetCurrentPosition());
                }
            }
        }
        else
        {
//            if(bLookdown_Detected == true)
//            {
//                if(m_pHWSet->m_pHoistAxis->IsRunning() == ON)
//                    return nError;
//                bLookdown_Detected = false;
//
//                // by zzang9un 2013.09.24 : Lookdown 해제 시 이적재 사운드로 변경
//				if(m_nTransSound != 0)
//                    m_pHWSet->Sound_On(SETSOUND_TRANS);
//                else
//                    m_pHWSet->Sound_Off();
//
//                switch(nHoistState)
//                {
//                case HOIST_DOWN_STATE:
//                    if(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > fabs(m_dCmdHoistPos))
//                        m_nTransStep = nNextStep;
//                    else
//						m_nTransStep = nPreStep;
//                    break;
//                case HOIST_UP_STATE:
//                    if(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) < fabs(m_dCmdHoistPos))
//                        m_nTransStep = nNextStep;
//                    else
//                        m_nTransStep = nPreStep;
//                    break;
//                }
//				ADD_LOG("Lookdn OFF2[Step=%d]: cmdPos=%.02f curPos = %.02f", m_nTransStep, m_dCmdHoistPos,
//                    m_pHWSet->m_pHoistAxis->GetCurrentPosition());
//            }
        }
    }

	if((nError == NO_ERR) && (m_TransUse.GripperSen == true))
	{
		if(m_pHWSet->m_pHand->IsOpen() == true)
		{
            if((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
            {
				m_nErrCount_GripAbNormalON++;
                if(m_nErrCount_GripAbNormalON > 20) // 200msec
                {
					m_nErrCount_GripAbNormalON = 0;
                    nError = ERR_GRIPPER_SEN_ON_IN_HAND_OPEN;
					ADD_LOG("Check_Sensor[Step=%d]: ERR_GRIPPER_SEN_ON_IN_HAND_OPEN", m_nTransStep);
                }
            }
            else
				m_nErrCount_GripAbNormalON = 0;
		}
		else if(m_pHWSet->m_pHand->IsClose() == true)
		{
			if((m_pHWSet->m_pHand->IsRightGripOn() == false) || (m_pHWSet->m_pHand->IsLeftGripOn() == false))
            {
				m_nErrCount_GripAbNormalOFF++;
				if(m_nErrCount_GripAbNormalOFF > 20) // 200msec
				{
                    m_nErrCount_GripAbNormalOFF = 0;
                    nError = ERR_GRIPPER_SEN_OFF_IN_HAND_CLOSE;
					ADD_LOG("Check_Sensor[Step=%d]: ERR_GRIPPER_SEN_OFF_IN_HAND_CLOSE", m_nTransStep);
                }
            }
            else
                m_nErrCount_GripAbNormalOFF = 0;
        }
    }
    else
    {
        m_nErrCount_GripAbNormalON = 0;
        m_nErrCount_GripAbNormalOFF = 0;
	}
	/*  soming
		if(m_pHWSet->m_pHand->IsClose() == false)
		{
			nError = ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD;
			ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD", m_nTransStep);
		}

		if(m_pHWSet->m_pHand->IsOpen() == true)
		{
			m_nErrCount_HandOpenAbNormalON++;
			if(m_nErrCount_HandOpenAbNormalON > 3) // 48msec
			{
				m_nErrCount_HandOpenAbNormalON = 0;
				nError = ERR_HAND_OPEN_SEN_ON_BEFORE_LOAD;
				ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_OPEN_SEN_ON_BEFORE_LOAD", m_nTransStep);
			}
		}else
				m_nErrCount_HandOpenAbNormalON = 0;

	  */

	if(m_pHWSet->m_pHoistAxis->GetCurrentPosition() < (m_dHoist_TeachingPos - TEACHING_OFFSET_HOIST_AT)) //soming Autoteaching sleep 삭제 위한 조건 추가
	{
		if((nError == NO_ERR) && (m_TransUse.HandPushSen == true))
		{
				if(m_pHWSet->m_pHand->IsPushOn() == true)
				{
					nLoadPushCount++;
					if(nLoadPushCount > 5)
					{
						nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
						ADD_LOG("Check_Sensor[Step=%d]: ERR_HAND_PUSHSEN_ABNORMAL_ON", m_nTransStep);
						nLoadPushCount = 0;
					}
				}
				else
					nLoadPushCount = 0;

		}
	}


    // Oscillation Sensor Check..
	if((nError == NO_ERR) && (m_TransUse.OscillationSen == true))
    {
        if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
        {
            if(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > OSCILLATION_IGNORE_DIST)
            {
                // by zzang9un 2013.08.06 : Oscillation이 true가 되면 hand가 흔들린 경우임
                if(m_pHWSet->m_pOcsillation->IsDetect() == true)
				{
					m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_OC);
                    nOsciCount++;
                    if(nOsciCount > 5)
                    {
						nError = ERR_OSCILLATION_ERROR;
						ADD_LOG("Check_Sensor[Step=%d]: ERR_OSCILLATION_ERROR", m_nTransStep);
                        nOsciCount = 0;
                    }
                }
            }
            else
                nOsciCount = 0;
        }
        else
        {
            dOscillationCheckDist = (double)m_pParamSet->TransDefine.dOscillation_Check_Dist;

            if((fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) < (fabs(m_dHoist_TeachingPos) - dOscillationCheckDist)) &&
                (fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) > OSCILLATION_IGNORE_DIST))
            {
                // by zzang9un 2013.08.06 : Oscillation이 true가 되면 hand가 흔들린 경우임
                if(m_pHWSet->m_pOcsillation->IsDetect() == true)
				{
					m_DiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_OC);
                    nOsciCount++;
                    if(nOsciCount > 5)
                    {
						nError = ERR_OSCILLATION_ERROR;
						ADD_LOG("Check_Sensor[Step=%d]: ERR_OSCILLATION_ERROR", m_nTransStep);
                        nOsciCount = 0;
                    }
				}
			}
            else
                nOsciCount = 0;
        }
    }
    else
        nOsciCount = 0;


	// Shutter Open 상태여야 할 때 Shutter 리미트 센서 감시
    if((nError == NO_ERR) && (m_TransUse.ShutterControl == true))
	{
		if(m_pHWSet->m_pShutter->CheckSensor(SHUTTER_OPEN_POS) !=NO_ERR)
		{
			nshutterCount++;

			if(nshutterCount>5)
			{
				nError = m_pHWSet->m_pShutter->CheckSensor(SHUTTER_OPEN_POS);
				nshutterCount =0;
			}
		}
		else
			nshutterCount =0;
	}

	return nError;

}

int TransControl::Move_ShiftRotate_AutoTeaching(int nMoveCase)
{
	int nError = 0;
	double dVel_mmpermsec = 0.0;
	double dAccel_mmpermsec2 = 0.0;
	bool bIsHighSpeed = false;
	// static int nCount = 0;

	switch(nMoveCase)
	{
	case SHIFTROTATE_H2TP_AT: // 원점에서 Teaching Point까지 이동
		// 2016-11-28, Add by Labeler, ROHT의 경우, Rotate 추가
		dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_20P;
		dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;
		if(nError == NO_ERR)
			nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dShift_TeachingPos);


		if(nError == NO_ERR)
		{
			m_dCmdShiftPos = m_dShift_TeachingPos;
		}
		break;
	case SHIFTROTATE_TP2H_AT: // Teaching Point에서 원점까지 이동..
		dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_20P;
		dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;
		if(nError == NO_ERR)
				nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);

		if(nError == NO_ERR)
		{
			m_dCmdShiftPos = 0.0;
        }
		break;
	case SHIFTROTATE_MOVE_TP_AT: // 원점에서 Teaching Point까지 이동
		// 2016-11-28, Add by Labeler, ROHT의 경우, Rotate 추가

		dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_10P;
		dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;

		ADD_LOG("AUTO_TEACHING_MOVE_FOCUS_CMD Crr :%f + Offset :%f",m_pHWSet->m_pShiftAxis->GetCurrentPosition(),m_AutoTeaching->fSlide_Offset);

		m_dCmdShiftPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition() +  m_AutoTeaching->fSlide_Offset;

		if(nError == NO_ERR)
			nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, m_dCmdShiftPos);

		break;
    }

    return nError;

}
int TransControl::Check_ShiftRotate_AutoTeaching(int nMoveCase, bool &bNexstep)
{
 int nError = 0;

	DWORD dwStartTime = 0;
	DWORD dwSleepTime = 0;
	bool bIsError = true;

	//Home 복귀시.

	switch(nMoveCase)
	{
		case SHIFTROTATE_H2TP_AT: // 원점에서 Teaching Point까지 이동

//		if(fabs(m_dShift_TeachingPos) > OUTTRIGGER_IS_LENG)
//		{
//			if(m_pHWSet->m_pRailSupport->IsHomePos() == true)
//			{
//				nError = ERR_AUTOTEACHING_ABNORMAL_OUTTRIGGER;
//			}
//        }


		break;
		case SHIFTROTATE_TP2H_AT:

		// Shift가 들어오는 중 일정 범위 안에 들어오면 Rail Support를 내리는 명령을 준다..


		if(m_pHWSet->m_pShiftAxis->IsShiftHome() == false)
        {
            dwStartTime = timeGetTime();
            bIsError = true;
            while(1)
            {
				Sleep(10);
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 10000)
                    break;
				if((m_pHWSet->m_pShiftAxis->IsShiftHome() == true) || (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < INPOS_LIMIT))
                {
					bIsError = false;
                    break;
                }
            }
            if((bIsError == true) && (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) > INPOS_LIMIT))
            {
				nError = ERR_SHIFT_HOMESEN_NOTON;
				ADD_LOG("[Check_ShiftRotate] E : %d(ERR_SHIFT_HOMESEN_NOTON)", nError);
                return nError;
            }
		}

		break;
		case SHIFTROTATE_MOVE_TP_AT:
		break;
    }
//    if(m_dCmdShiftPos == 0.0)
//	{
//
//
//    }

    if(m_pHWSet->m_pShiftAxis->IsRunning() == true)
        return nError;

    if(fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition() - m_dCmdShiftPos) > INPOS_LIMIT)
		return nError;

	bNexstep = true;
//	m_nTransStep++;

    return nError;


}
int TransControl::Move_Hoist_AutoTeaching(int nMoveCase)
{
 	int nError = 0;

	double dVel_mmpermsec = 0.0;
	double dAccel_mmpermsec2 = 0.0;
	double dDecel_mmpermsec2 = 0.0;
	double dTmpHoist_TeachingPos = 0.0;

	switch(nMoveCase)
	{

	case HOIST_DOWN_H2TP_AT: // 2축 이동 : 원점 -> Teahcing
		if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
			dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_2P;
		else
			dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_10P;

		dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
		dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;

		SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), m_dHoist_TeachingPos, dVel_mmpermsec);

		m_dCmdFinal_HoistPos = m_dCmdHoistPos;

		nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
		break;

	case HOIST_UP_TP2H_AT: // 2축 이동 :  Teahcing -> 원점

		if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
			dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_2P;
		else
			dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_20P;

		dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
		dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;

		SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), 0.0, dVel_mmpermsec);

		nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);

		if(nError == NO_ERR)
		{
			m_dCmdHoistPos = 0.0;
		}
		break;


	case HOIST_MOVE_TP_AT:    // hoist를 위 또는 아래로 일정량 움직임.
//		double dMoveDist_1mm = 1.0; //AT에서 받은값으로 사용할것 puting

		dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_1P;
		dAccel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;
		dDecel_mmpermsec2 = dVel_mmpermsec / HOIST_ACCTIME_500;



		SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), (m_pHWSet->m_pHoistAxis->GetCurrentPosition() + m_AutoTeaching->dHoist_Offset), dVel_mmpermsec);

		m_dCmdHoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition() + m_AutoTeaching->dHoist_Offset;
		m_dHoist_TeachingPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition() + m_AutoTeaching->dHoist_Offset;

		nError = m_pHWSet->m_pHoistAxis->MoveDistance(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos);
  		break;
	}

	return nError;

}
int TransControl::Check_Hoist_AutoTeaching(int nMotionType, bool &bNexstep)
{
	int nError = 0;

	double dStartPos = fabs(m_dCmdHoistStartPos);
    double dEndPos = fabs(m_dCmdHoistPos);
    double dCurPos = fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition());
    double dEndSpeed = fabs(m_dCmdHoistSpeed);
    double dCurSpeed = fabs(m_pHWSet->m_pHoistAxis->GetCurrentVelocity());
    bool bRunning = false;
	static int nErrCount = 0;

	bRunning = (m_pHWSet->m_pHoistAxis->IsRunning() == true) ? true : false;

	bNexstep = false;

//	ADD_LOG("[Check_Hoist_AutoTeaching]Start%f, End:%f, Cur:%f  Running:%d", dStartPos,dEndPos,dCurPos,bRunning);

    switch(nMotionType)
    {

	case HOIST_DOWN_H2TP_AT:      // 2축 이동 : 원점 -> Teahcing
		// 티칭 값이 길 때 Push 센서를 보고 바로 정지함.

		if(m_pHWSet->m_pHand->IsPushOn() == true)
		{
			nError = m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL*1.5);
			return ERR_AUTOTEACHING_ABNORMAL_HANDPUSH;
		}
        if(dCurPos < (dStartPos + dEndPos) / 2.0)
            return nError;
        if(fabs(dEndPos - dCurPos) > INPOS_LIMIT)
            return nError;
        if(bRunning == true)
            return nError;
		break;

	case HOIST_UP_TP2H_AT:          // 2축 이동 :  Teahcing -> 원점
//		if(dCurPos > (dStartPos + dEndPos) / 2.0)
//            return nError;
        if(fabs(dEndPos - dCurPos) > INPOS_LIMIT)
            return nError;
        if(bRunning == true)
            return nError;
		break;

	 case HOIST_MOVE_TP_AT:        // hoist를 위 또는 아래로 일정량 움직임.
	    if(fabs(dEndPos - dCurPos) > INPOS_LIMIT)
			return nError;
		if(bRunning == true)
			return nError;
		break;
    }

    if(m_TransUse.HoistHomeSen == true)
    {
        if(m_dCmdHoistPos == m_dHoist_TeachingPos)
        {
            if(m_pHWSet->m_pHoistAxis->IsHoistHome() == true)
            {
                nError = ERR_HOISTALIGNSEN_ABNORMAL_ON;
				ADD_LOG("Check_Hoist[Step=%d]: ERR_HOISTALIGNSEN_ABNORMAL_ON", m_nTransStep);
            }
        }
        else if(m_dCmdHoistPos == 0.0)
        {
			if((m_pHWSet->m_pHoistAxis->IsHoistHome() == false) && (dCurPos > HOIST_HOME_INPOS_LIMIT))
            {
                nErrCount++;
                if(nErrCount > 100)
                {
                    nError = ERR_HOISTALIGNSEN_NOT_ON;
					ADD_LOG("Check_Hoist[Step=%d]: ERR_HOISTALIGNSEN_NOT_ON", m_nTransStep);
                }
                else
                    return nError;
            }
            else
                nErrCount = 0;
        }
        else
            nErrCount = 0;
    }
    else
        nErrCount = 0;

	if(nError == NO_ERR)
	{
//		m_nTransStep++;
		bNexstep =true;
	}

    return nError;

}
void TransControl::LimitRetryCheck(int nPortType, double dCurrentPosition, double dOffset)
{
	double dLimitCheck = dCurrentPosition + dOffset;

	if(nPortType == PORT_TYPE_STB_R)
	{
		//STB_R Limit 조건인 경우 Retry 판단
		if(dLimitCheck  > SHIFT_MOVELIMIT_P)
		{
			ADD_LOG("[RSTB][LimitRetryCheck]Shift Out Of P Limit");
			if(dCurrentPosition <= (SHIFT_MOVELIMIT_P-1))
			{
				m_AutoTeaching->fSlide_Offset = SHIFT_MOVELIMIT_P-0.5-dCurrentPosition;
				ADD_LOG("[RSTB][LimitRetryCheck]Retry, Current Position : %.1f", dCurrentPosition);
				ADD_LOG("[RSTB][LimitRetryCheck]CmdPosition : %.1f",dCurrentPosition + m_AutoTeaching->fSlide_Offset );
			}

		}
		else if(dLimitCheck < SHIFT_MOVELIMIT_N_MIN_STB_R)
		{
			ADD_LOG("[RSTB][LimitRetryCheck]Shift Out Of N Limit");
			if(dCurrentPosition >= SHIFT_MOVELIMIT_N_MIN_STB_R+1)
			{
				m_AutoTeaching->fSlide_Offset = SHIFT_MOVELIMIT_N_MIN_STB_R+0.5-dCurrentPosition;
				ADD_LOG("[RSTB][LimitRetryCheck]Retry Success, Current Position : %.1f",dCurrentPosition);
				ADD_LOG("[RSTB][LimitRetryCheck]CmdPosition : %.1f",dCurrentPosition + m_AutoTeaching->fSlide_Offset );
			}

		}
	}
	else if(nPortType == PORT_TYPE_STB_L)
	{
		//STB_L Limit 조건인 경우 Retry 판단
		if(dLimitCheck < SHIFT_MOVELIMIT_N)
		{
			ADD_LOG("[LSTB][LimitRetryCheck]Shift Out Of N Limit");
			if(dCurrentPosition >= SHIFT_MOVELIMIT_N+1)
			{
				m_AutoTeaching->fSlide_Offset =  SHIFT_MOVELIMIT_N+0.5-dCurrentPosition;
				ADD_LOG("[LSTB][LimitRetryCheck]Retry Success, Current Position : %.1f",dCurrentPosition);
				ADD_LOG("[LSTB][LimitRetryCheck]CmdPosition : %.1f",dCurrentPosition + m_AutoTeaching->fSlide_Offset );

			}
		}
		else if(dLimitCheck > SHIFT_MOVELIMIT_N_MIN_STB_L)
		{
			ADD_LOG("[LSTB][LimitRetryCheck]Shift Out Of P Limit");
			if(dCurrentPosition <= SHIFT_MOVELIMIT_N_MIN_STB_L-1)
			{
				m_AutoTeaching->fSlide_Offset = SHIFT_MOVELIMIT_N_MIN_STB_L-0.5-dCurrentPosition;
				ADD_LOG("[LSTB][LimitRetryCheck]Retry Success, Current Position : %.1f",dCurrentPosition);
				ADD_LOG("[LSTB][LimitRetryCheck]CmdPosition : %.1f",dCurrentPosition + m_AutoTeaching->fSlide_Offset );
			}
		}

	}
}

int TransControl::Check_Complete_AutoTeacing()
{
   int nError = 0;
    int nCount = 0;
	DWORD dwStartTime = 0;

	//nError = m_pEQPIO->PIODisable(true);

//	 m_TransUse.STBSen =false; //임시
    if(nError == NO_ERR)
		nError = m_pHWSet->m_pLookdown->Disable();

    // 2 HOIST HOME, SHIFT HOME 센서 확인
	if((nError == NO_ERR) && (m_nTransType != TRANSTYPE_MOVET))
    {
        if(m_pHWSet->m_pHoistAxis->IsHoistHome() == false)
        {
            nError = ERR_HOISTALIGNSEN_NOT_ON;
            ADD_LOG("Check_Complete: ERR_HOISTALIGNSEN_NOT_ON");
		}

		if(m_pHWSet->m_pShiftAxis->IsShiftHome() == false)
		{
			nError = ERR_SHIFT_HOMESEN_NOTON;
			ADD_LOG("Check_Complete: ERR_SHIFT_HOMESEN_NOTON");
		}
	}

// 2016-11-28, Add by Labeler, ROHT의 경우, UTB, STB 미사용

   // 2 STB, UTB 확인
    if((nError == NO_ERR))
    {
        if((m_nEqType == PORT_TYPE_STB_L) || (m_nEqType == PORT_TYPE_STB_R))
		{
            switch(m_nSTBDirection)
            {
            case STB_DIR_RIGHT:
                if((m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect()))
                {
                    dwStartTime = timeGetTime();
                    while(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect())
                    {
                        if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)STBUTBSensorOFFTime)
                            break;
                        Sleep(10);
                    }
                    if(m_pHWSet->m_pTrackBufferDetect->IsRightSTBFoupDetect())
                    {
                        nError = ERR_STB_FOUP_CHECK_EXIST;
                        ADD_LOG("Check_Complete: ERR_STB_FOUP_EXIST");
                    }
                }
                break;
            case STB_DIR_LEFT:
                if((m_TransUse.STBSen == true) && (m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect()))
                {
                    dwStartTime = timeGetTime();

                    while(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect())
                    {
                        if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)STBUTBSensorOFFTime)
                            break;
                        Sleep(10);
                    }

                    if(m_pHWSet->m_pTrackBufferDetect->IsLeftSTBFoupDetect())
                    {
                        nError = ERR_STB_FOUP_CHECK_EXIST;
                        ADD_LOG("Check_Complete: ERR_STB_FOUP_EXIST");
                    }
                }
                break;
            }
        }
	}

	// 2 RailSupport 동작
	if(m_nTransType != TRANSTYPE_MOVET)
	{

		// 2 원점에서 Hand Push Sensor ON이면 에러 발생
		if((nError == NO_ERR) && (m_TransUse.HandPushSen == true))
		{
			if(m_pHWSet->m_pHand->IsPushOn() == true)
			{
				nCount = 0;

				while(m_pHWSet->m_pHand->IsPushOn() == true)
				{
					if(nCount > 4)
						break;
					nCount++;
					Sleep(10);
				}

				if(m_pHWSet->m_pHand->IsPushOn() == true)
				{
					nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
					ADD_LOG("Check_Complete: ERR_HAND_PUSHSEN_ABNORMAL_ON");
					nCount = 0;
				}
			}
		}
	}

    // 2 이적재 완료 후 상황에 맞지 않게 Grip Sensor가 되어 있으면 에러 발생
	if((nError == NO_ERR) && (m_TransUse.GripperSen == true))
	{
		if(m_pHWSet->m_pHand->IsOpen() == true)
		{
            if((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
            {
                while((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
                {
                    if(m_nErrCount_GripAbNormalON > 5)
                        break;
                    m_nErrCount_GripAbNormalON++;
                    Sleep(10);
                }
                if((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
                {
                    nError = ERR_GRIPPER_SEN_ON_IN_HAND_OPEN;
                    ADD_LOG("Check_Complete: ERR_GRIPPER_SEN_ON_IN_HAND_OPEN");
                    m_nErrCount_GripAbNormalON = 0;
                }
            }
		}

		else if(m_pHWSet->m_pHand->IsClose() == true)
		{
			if((m_pHWSet->m_pHand->IsRightGripOn() == false) || (m_pHWSet->m_pHand->IsLeftGripOn() == false))
			{
                while((m_pHWSet->m_pHand->IsRightGripOn() == false) || (m_pHWSet->m_pHand->IsLeftGripOn() == false))
				{
                    if(m_nErrCount_GripAbNormalOFF > 5)
                        break;
                    m_nErrCount_GripAbNormalOFF++;
                    Sleep(10);
				}
                if((m_pHWSet->m_pHand->IsRightGripOn() == false) || (m_pHWSet->m_pHand->IsLeftGripOn() == false))
                {
                    nError = ERR_GRIPPER_SEN_OFF_IN_HAND_CLOSE;
					ADD_LOG("Check_Complete: ERR_GRIPPER_SEN_OFF_IN_HAND_CLOSE");
                    m_nErrCount_GripAbNormalOFF = 0;
                }
            }
		}
    }

	// Auto Teaching완료 후, Foup 존재하지 않을 시==>에러 발생.
		if((nError == NO_ERR) && ((m_pHWSet->m_pHand->IsRightGripOn() == true) || (m_pHWSet->m_pHand->IsLeftGripOn() == true))
		 && (m_pHWSet->m_pInnerFoup->IsFoupDetect() == false))
		{
			DWORD dwStartTime = 0;
			static int nCount = 0;
			dwStartTime = timeGetTime();
			while(1)
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 500)
					break;

				if(m_pHWSet->m_pInnerFoup->IsFoupDetect() == false)
				{
					nCount++;
					if(nCount > 5)
						break;
				}
				Sleep(10);

			}

			if(nCount > 5)
			{
				nCount = 0;
				nError = ERR_INNER_FOUP_NOT_DETECT;
				ADD_LOG("Check_Complete: ERR_INNER_FOUP_NOT_DETECT");
			}
		}

	if(nError == NO_ERR)
	{
		if(m_pHWSet->m_pShiftAxis->IsShiftHome() == false)
		{
			if((fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < INPOS_LIMIT))
			{
				nError = ERR_SHIFT_HOMESEN_NOTON;
				ADD_LOG("Check_Complete: ERR_SHIFT_HOMESEN_NOTON");
			}
		}
	}


	// 2 by zzang9un 2012.12.19 : Reset 대신 Set으로 변경
	m_pHWSet->m_pShiftAxis->SetNegativeSWLimit(SHIFT_NEG_LIMIT);
	m_pHWSet->m_pShiftAxis->SetPositiveSWLimit(SHIFT_POS_LIMIT);
	m_pHWSet->m_pHoistAxis->SetNegativeSWLimit(HOIST_NEG_LIMIT);
	m_pHWSet->m_pHoistAxis->SetPositiveSWLimit(HOIST_POS_LIMIT);

	// -------------------------------------------
	// by zzang9un 2012.12.19 : Neg Limit 에러로 인해 로그 추가
//	double dHoistPositive = 0.0;
//	double dHoistNegative = 0.0;
//	double dShiftPositive = 0.0;
//	double dShiftNegative = 0.0;
//
//	m_pHWSet->m_pHoistAxis->GetSWLimit(dHoistPositive, dHoistNegative);
//	m_pHWSet->m_pShiftAxis->GetSWLimit(dShiftPositive, dShiftNegative);
	// -------------------------------------------

	m_pHWSet->Sound_Off();

	if(nError == NO_ERR)
	{
		m_StatusSet.byStatus = STS_AUTO_TAUGHT;
	}
	else
	{
		m_StatusSet.uError = nError;
		m_StatusSet.byStatus = STS_ERROR;
	}

	m_StatusSet.bIsOnProcess = false;

	return nError;
}
int TransControl::CheckNMoveTeachingPoint_AutoTeaching()
{
    int nError = 0;

    int nRetryCount = 1;
	double dVel_mmpermsec = 0.0;
	double dCurPos_for_URetry = 0.0;
    int nCount = 0;
	AnsiString strLog = "";
	DWORD dwStTime = 0;


    //추후에 Hoist축 Add Small 명령 추가시, 변경해 줄것. puting
	if(m_pHWSet->m_pHand->IsPushOn() == false)
    {
		nCount = 0;
		while(m_pHWSet->m_pHand->IsPushOn() == false)
		{
			nCount++;
			if(nCount > 10) // 500msec
			{
				break;
			}
			Sleep(50);
		}
	}
	else
	{
		nCount = 0;
		while(m_pHWSet->m_pHand->IsPushOn() == true)
		{
			nCount++;
			if(nCount > 5) // 250msec
			{
				break;
			}
			Sleep(50);
		}
		return nError;

	}


	// false일 경우 수행하는 부분
	if(m_pHWSet->m_pHand->IsPushOn() == false)
	{

		nRetryCount = 1;
		dVel_mmpermsec = HOIST_MAX_SPEED * MULTIPLY_2_05P;
		nError = MoveHoist_forTeachingPoint(nRetryCount, dVel_mmpermsec, m_dHoist_TeachingPos);
		while(1)
		{
			if(m_pHWSet->m_pHoistAxis->IsRunning() == true)
				continue;
			if(nRetryCount > AUTOTEACHING_MOVETP_RETRYCOUNT)
				break;

			if(m_pHWSet->m_pHand->IsPushOn() == true)
			{
				nRetryCount++;
				nError = MoveHoist_forTeachingPoint(nRetryCount, dVel_mmpermsec, m_dHoist_TeachingPos);
				break;
			}
			nRetryCount++;
			nError = MoveHoist_forTeachingPoint(nRetryCount, dVel_mmpermsec, m_dHoist_TeachingPos);
			Sleep(500);

		}

		if((m_pHWSet->m_pHand->IsPushOn() == false) || (nRetryCount > AUTOTEACHING_MOVETP_RETRYCOUNT - 1))
		{
			dwStTime = timeGetTime();

			while(m_pHWSet->m_pHand->IsPushOn() == false)
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwStTime) > 5000)
					break;
				Sleep(100);
			}

			if(m_pHWSet->m_pHand->IsPushOn() == false)
			{
				ADD_LOG("CheckNMoveTeachingPoint : ERR_HAND_PUSHSEN_NOTON", m_nTransStep);
				nError = ERR_HAND_PUSHSEN_NOTON;
			}

		}
	}

	return nError;


}
/**
 @brief   Hoist, Shuft, Rotate, Hand, Shutter Home 이동
 @author  Labeler
 @date    2017.01.01
 @return  동작 중 Error 발생 시 해당 Error return
          미 발생 시 NO_ERR return
*/
int TransControl::Cmd_MoveAllHome()
{
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
    static int nRotateInpoCnt = 0;
#endif
    static DWORD dwCheckTime = 0;
    int nError = NO_ERR;

    switch(m_nTransStep)
    {
	case STEP_MOVE_HOME_READY:
        // 원점 이동 전 Check
        if(nError == NO_ERR)
        {
            nError = ReadyAllHome();
        }
        if(nError == NO_ERR)
        {
            m_nTransStep++;
		}
	break;

	case STEP_MOVE_HOME_HAND_ORIGIN:
		if(nError == NO_ERR)
		{
			if(m_pHWSet->m_pHand->IsReady() == false)
			{
				nError = m_pHWSet->m_pHand->AutoInitialize();
			}
		}

		if(nError == NO_ERR)
		{
			if((m_pHWSet->m_pHand->IsReady() == true) && (m_pHWSet->m_pHand->IsClose() != true))
			{
				if(m_pHWSet->m_pShutter->ReadyToOperation() != NO_ERR)
				{
					nError = m_pHWSet->m_pHand->ManualClose();
				}
				else
				{
					m_nTransStep++;
				}
			}
			else if((m_pHWSet->m_pHand->IsReady() == true) && (m_pHWSet->m_pHand->IsClose() == true))
			{
				m_nTransStep++;
			}
		}
	break;

	case STEP_MOVE_HOME_SHUTTER_ORIGIN:
		if(nError == NO_ERR)
		{

			if(m_pHWSet->m_pShutter->ReadyToOperation() != NO_ERR)
			{

				if((m_pHWSet->m_pShutter->IsClose(BOTH)== true) && (m_pHWSet->m_pHoistAxis->IsHoistHome() == false))
					nError = ERR_HOISTALIGNSEN_NOT_ON;
				else
					nError = m_pHWSet->m_pShutter->Origin();
			}
			else if(m_pHWSet->m_pShutter->IsClose(BOTH)== true)
			{
                nError = m_pHWSet->m_pShutter->Open();
            }

			if(nError == NO_ERR)
			{
				m_nTransStep++;
			}
		}
	break;

    case STEP_MOVE_HOME_HOIST_MOVE:
        // Hoist 축 Home 이동
        if(nError == NO_ERR)
       	{
            nError = Check_Sensor(true, HOIST_UP_STATE, STEP_MOVE_HOME_HOIST_MOVE, STEP_MOVE_HOME_HOIST_MOVE);
       	}
        if(nError == NO_ERR)
       	{
 #ifdef USE_TRANS_PROFILE_VER2
			nError = Move_Hoist_Ver2(HOIST_UP_2H);
#else
			nError = Move_Hoist(HOIST_UP_2H);
#endif
       	}
        if(nError == NO_ERR)
       	{
            m_nTransStep++;
            dwCheckTime = (DWORD)timeGetTime();
       	}
    break;

    case STEP_MOVE_HOME_HOIST_CHECK:
        // Hoist 축 Home 이동 Check
        if(nError == NO_ERR)
       	{
            nError = Check_Sensor(true, HOIST_UP_STATE, STEP_MOVE_HOME_HOIST_MOVE, STEP_MOVE_HOME_SHIFT_MOVE);
       	}
        if(nError == NO_ERR)
        {
  #ifdef USE_TRANS_PROFILE_VER2
		nError = Check_Hoist_Ver2(HOIST_UP_2H);
  #else
  		nError = Check_Hoist(HOIST_UP_2H);
  #endif
        }

        if((nError == NO_ERR) && (OHTUTILITY::PreventOverflowGetTimeMs(dwCheckTime) > 60000))
        {
            nError = ERR_TRANS_AXIS2MOVE_TIMEOVER;
            ADD_LOG("[Move All Home Step=%d]: ERR_TRANS_AXIS2MOVE_TIMEOVER", m_nTransStep);
			ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%d/%f",
								ERR_TRANS_AXIS2MOVE_TIMEOVER,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pExecuteInfo->ExecutePositionInfo.NextNode,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_pOHTMainStatus->StatusCommon.ExistFoup,
								m_pOHTMainStatus->StatusCommon.StopStationID,
								m_nTransType,
								m_nTransStep,
								m_pHWSet->m_pHoistAxis->GetCurrentPosition());
        }
    break;

    case STEP_MOVE_HOME_SHIFT_MOVE:
        // Shift, Rotate 축 원점 이동
        if(nError == NO_ERR)
       	{
            nError = Move_ShiftRotate(SHIFTROTATE_TP2HOME);
        }
        if(nError == NO_ERR)
       	{
			m_nTransStep++;
            dwCheckTime = (DWORD)timeGetTime();
       	}
    break;

    case STEP_MOVE_HOME_SHIFT_CHECK:
        // Shift, Rotate 축 원점 이동 Check
        if(nError == NO_ERR)
       	{
			nError = Check_ShiftRotate();
       	}

        if((nError == NO_ERR) && (OHTUTILITY::PreventOverflowGetTimeMs(dwCheckTime) > 10000))
        {
            nError = ERR_TRANS_AXIS3MOVE_TIMEOVER;
            ADD_LOG("[Move All Home Step=%d]: ERR_TRANS_AXIS3MOVE_TIMEOVER", m_nTransStep);
            ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%d/%d/%f",
								ERR_TRANS_AXIS3MOVE_TIMEOVER,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pExecuteInfo->ExecutePositionInfo.NextNode,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_pOHTMainStatus->StatusCommon.ExistFoup,
								m_pOHTMainStatus->StatusCommon.StopStationID,
								m_nTransType,
								m_nTransStep,
								m_pHWSet->m_pShiftAxis->GetCurrentPosition());
        }
    break;



#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))

	// Shift 10mm move & Check
	case STEP_MOVE_HOME_PRE_SHIFT_MOVE:
	{
#if(OHT_NAME == OHT_NAME_STD_V85)
		m_nTransStep = STEP_MOVE_HOME_ROTATE_MOVE;
#else
		if((m_pHWCommon->m_Input.POD_EUV_1 == ON) && (m_pHWCommon->m_Input.POD_EUV_2 == ON))
		{
			nError = Move_EUV_Shift();
			if(nError == NO_ERR)
			{
				m_nTransStep++;
			}
		}
		else if(m_pHWCommon->m_Input.POD_EUV_1 != m_pHWCommon->m_Input.POD_EUV_2)
		if(m_pHWCommon->m_Input.POD_EUV_1 != m_pHWCommon->m_Input.POD_EUV_2)
		{
			nError = ERR_EUVROHT_DIFF_EUVSENSINGS;
		}
		else
		{
			m_nTransStep = STEP_MOVE_HOME_ROTATE_MOVE;
		}
#endif

	}
	break;
	case STEP_MOVE_HOME_PRE_SHIFT_CHECK:
	{
#if(OHT_NAME == OHT_NAME_STD_V85)
#else
		nError = Check_EUV_Shift();
#endif

		if(nError == NO_ERR)
			m_nTransStep++;
	}
	break;
#endif

    case STEP_MOVE_HOME_ROTATE_MOVE:
 #if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
        if(nError == NO_ERR)
        {
            if(m_pHWSet->m_pShutter->IsOpen(BOTH) != true)
            {
                nError = m_pHWSet->m_pShutter->Open();
            }
        }

        if((nError == NO_ERR) && (m_pHWSet->m_pShutter->IsOpen(BOTH) == true))
        {
            if(m_pHWSet->m_pRotate->IsOrigin() == true)
            {
                nError = m_pHWSet->m_pRotate->MoveAbsolute(0);
            }
            else
            {
                nError = m_pHWSet->m_pRotate->Zeroing();
            }

            if(nError == NO_ERR)
            {
                dwMoveSRStartTime = timeGetTime();
                nRotateInpoCnt = 0;
                m_nTransStep++;
            }
        }
#else
        m_nTransStep++;
#endif
    break;

    case STEP_MOVE_HOME_ROTATE_CHECK:
 #if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
        if(nError == NO_ERR)
        {
            if(m_pHWSet->m_pShutter->IsOpen(BOTH) != true)
                nError = ERR_SHUTTER_NOTORIGIN;
        }

        if(nError == NO_ERR)
        {
            if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > ROTATE_INPOS_LIMIT)
            {
                nRotateInpoCnt = 0;
                if(OHTUTILITY::PreventOverflowGetTimeMs(dwMoveSRStartTime) > (ROTATE_EXEC_TIMEOUT * 4))
                {
                    nError = ERR_ROTATE_NOT_0;
                }
            }
            else
            {
                if(nRotateInpoCnt > 100)
                    m_nTransStep++;
                else
                    nRotateInpoCnt++;
            }
        }
#else
        m_nTransStep++;
#endif
    break;

#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	case STEP_MOVE_HOME_RETURN_SHIFT_MOVE:
	{
		if(fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) <= RETICLE_SHIFT_INPOS_LIMIT)
		{
	// shift Home??
			m_nTransStep = STEP_MOVE_HOME_HAND_SHUTTER_MOVE;
		}
		else
		{	/* add Shift가 Home 위치가 아닌 경우 */
			double dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_10P;
			double dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;
			nError = m_pHWSet->m_pShiftAxis->MoveDistance
				 (dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);

			m_dCmdShiftPos = 0.0; // target position

			m_nTransStep++;
		}
	}
	break;
	case STEP_MOVE_HOME_RETURN_SHIFT_CHECK:
	{
		nError = Check_EUV_Shift();
		if(nError == NO_ERR)
			m_nTransStep++;
	}
	break;
#endif
    case STEP_MOVE_HOME_HAND_SHUTTER_MOVE:
        // Shutter Check 및 Close
        if(nError == NO_ERR)
        {
            if((m_pHWSet->m_pHand->IsRightGripOn() == true) && (m_pHWSet->m_pHand->IsLeftGripOn() == true)
            && (m_pHWSet->m_pInnerFoup->IsFoupDetect() == true))
            {
                nError = m_pHWSet->m_pShutter->Close();
            }
            else if((m_pHWSet->m_pHand->IsRightGripOn() == false) && (m_pHWSet->m_pHand->IsLeftGripOn() == false)
                && (m_pHWSet->m_pInnerFoup->IsFoupDetect() == false))
            {
                nError = m_pHWSet->m_pShutter->Open();
                if(nError == NO_ERR)
                {
                    nError = m_pHWSet->m_pHand->AutoOpen();
                }
            }
            else
            {
                nError = ERR_GRIP_SEN_MISMATCH;
        		ADD_LOG("[Move All Home Step=%d] E : %d(Carrier Detect Mismatch)", m_nTransStep, nError);
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_GRIP_SEN_MISMATCH,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHand->IsLeftGripOn(),
							m_pHWSet->m_pHand->IsRightGripOn());
            }

            if(nError == NO_ERR)
            {
                m_nTransStep++;
            }
        }
    break;

    case STEP_MOVE_HOME_COMPLETE:
        // Complete
        if(nError == NO_ERR)
       	{
			nError = Check_Complete();

       	}
    break;
    }

    if(nError != NO_ERR)
    {
        m_StatusSet.uError = nError;
        m_StatusSet.byStatus = STS_ERROR;
    }
    return nError;
}

#ifdef OHT_HIGH_RAIL_CHECK_HOME
int TransControl::Cmd_SmalladdHoistHome()
{
	int nError = NO_ERR;
	static DWORD dwCheckTime = 0;

	if(m_nTransStep == STEP_MOVE_HOME_READY) {
		m_nTransStep = STEP_MOVE_HOME_HOIST_MOVE;
		ADD_LOG("[SmalladdHoistHome] m_nTransStep: %d",m_nTransStep);


		if((m_pHWSet->m_pHand->IsOpen() == true)&&
		  ((m_pHWSet->m_pHand->IsLeftGripOn() == true) || (m_pHWSet->m_pHand->IsRightGripOn() == true)))
			nError = ERR_GRIPPER_SEN_ON_IN_HAND_OPEN;
		else if(m_pHWSet->m_pHand->IsLeftGripOn() != m_pHWSet->m_pHand->IsRightGripOn())
			nError = ERR_GRIP_SEN_MISMATCH;
		else if(m_pHWSet->m_pHand->IsOpen() == m_pHWSet->m_pHand->IsClose())
			nError = ERR_SEN_ABNORMAL_HAND;
		else if((m_pHWSet->m_pHand->IsLeftGripOn() == true) && (m_pHWSet->m_pHand->IsRightGripOn() == true) &&
		   ((m_pHWSet->m_pShutter->IsOpen(FRONT) == false) || (m_pHWSet->m_pShutter->IsOpen(REAR) == false)))
		   {
				nError = ERR_SHUTTER_NOTORIGIN;
           }
	}


	if(nError == NO_ERR)
	{
		switch(m_nTransStep)
		{
			case STEP_MOVE_HOME_HOIST_MOVE:
				if(nError == NO_ERR) {
					nError = Check_Sensor(true, HOIST_UP_STATE, STEP_MOVE_HOME_HOIST_MOVE, STEP_MOVE_HOME_HOIST_MOVE);
					ADD_LOG("Check_Sensor 1");
				}
				if(nError == NO_ERR) {
					nError = Move_Hoist_Ver2(HOIST_UP_2H);
					ADD_LOG("move hoist");
				}
				if(nError == NO_ERR) {
					dwCheckTime = (DWORD)timeGetTime();
					m_nTransStep++;
					ADD_LOG("STEP_MOVE_HOME_HOIST_CHECK");
				}
				break;
			case STEP_MOVE_HOME_HOIST_CHECK:
				if(nError == NO_ERR) {
					nError = Check_Sensor(true, HOIST_UP_STATE, STEP_MOVE_HOME_HOIST_MOVE, STEP_MOVE_HOME_HOIST_MOVE);
					ADD_LOG("STEP_MOVE_HOME_HOIST_CHECK Check_Sensor");
				}
				if(nError == NO_ERR) nError = Check_Hoist_Ver2(HOIST_UP_2H);
				if((nError == NO_ERR) && (OHTUTILITY::PreventOverflowGetTimeMs(dwCheckTime) > 60000))
				{
					nError = ERR_TRANS_AXIS2MOVE_TIMEOVER;
							ADD_LOG("[Move All Home Step=%d]: ERR_TRANS_AXIS2MOVE_TIMEOVER", m_nTransStep);
				}
				break;
		}
	}

	if(nError != NO_ERR)
	{
		m_StatusSet.uError = nError;
		m_StatusSet.byStatus = STS_ERROR;
	}

	if(m_nTransStep > STEP_MOVE_HOME_HOIST_CHECK)
	{
		m_pHWSet->m_pHoistAxis->SetHoistOriginFromSmalladd(true);

		ADD_LOG("Hoist home complete");
		m_StatusSet.byStatus = STS_COMPLETE;
		m_StatusSet.bIsOnProcess = false;
		//m_nTransStep = STEP_MOVE_HOME_READY; InsertGeneralCmd ???? 0????
	}
	return nError;
}
#endif

int TransControl::Cmd_AutoMode_Change()
{
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
    static int nRotateInpoCnt = 0;
#endif
    static DWORD dwCheckTime = 0;
	int nError = NO_ERR;


	switch(m_nTransStep)
	{
	case STEP_AUTOCHANGE_READY:
		// 원점 이동 전 Check
		if(nError == NO_ERR)
		{
			nError = ReadyAllHome();
		}
		if(nError == NO_ERR)
		{
			m_nTransStep++;
		}
	break;

	case STEP_AUTOCHANGE_HAND_ORIGIN:
		if(nError == NO_ERR)
		{
			if(m_pHWSet->m_pHand->IsReady() == false)
			{
				nError = m_pHWSet->m_pHand->AutoInitialize();
			}
		}

		if(nError == NO_ERR)
		{
			if((m_pHWSet->m_pHand->IsReady() == true) && (m_pHWSet->m_pHand->IsClose() != true))
			{
				if(m_pHWSet->m_pShutter->ReadyToOperation() != NO_ERR)
				{
					nError = m_pHWSet->m_pHand->ManualClose();
				}
				else
				{
					m_nTransStep++;
				}
			}
			else if((m_pHWSet->m_pHand->IsReady() == true) && (m_pHWSet->m_pHand->IsClose() == true))
			{
				m_nTransStep++;
			}
		}
	break;

	case STEP_AUTOCHANGE_SHUTTER_ORIGIN:
		if(nError == NO_ERR)
		{
			if(m_pHWSet->m_pShutter->ReadyToOperation() != NO_ERR)
			{
				if((m_pHWSet->m_pShutter->IsClose(BOTH)== true) && (m_pHWSet->m_pHoistAxis->IsHoistHome() == false))
					nError = ERR_HOISTALIGNSEN_NOT_ON;
				else
					nError = m_pHWSet->m_pShutter->Origin();
			}
			else if(m_pHWSet->m_pShutter->IsClose(BOTH)== true)
			{
                nError = m_pHWSet->m_pShutter->Open();
            }

			if(nError == NO_ERR)
			{
				m_nTransStep++;
			}
		}
	break;

	case STEP_AUTOCHANGE_HOIST_MOVE:
		// Hoist 축 Home 이동
		if(nError == NO_ERR)
		{
			nError = Check_Sensor(true, HOIST_UP_STATE, STEP_AUTOCHANGE_HOIST_MOVE, STEP_AUTOCHANGE_HOIST_MOVE);
		}
		if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Move_Hoist_Ver2(HOIST_UP_2H);
#else
			nError = Move_Hoist(HOIST_UP_2H);
#endif
		}
		if(nError == NO_ERR)
		{
			m_nTransStep++;
            dwCheckTime = (DWORD)timeGetTime();
		}
	break;

	case STEP_AUTOCHANGE_HOIST_CHECK:
		// Hoist 축 Home 이동 Check
		if(nError == NO_ERR)
		{
			nError = Check_Sensor(true, HOIST_UP_STATE, STEP_AUTOCHANGE_HOIST_MOVE, STEP_AUTOCHANGE_SHIFT_MOVE);
		}
		if(nError == NO_ERR)
		{
#ifdef USE_TRANS_PROFILE_VER2
			nError = Check_Hoist_Ver2(HOIST_UP_2H);
#else
			nError = Check_Hoist(HOIST_UP_2H);
#endif
		}

        if((nError == NO_ERR) && (OHTUTILITY::PreventOverflowGetTimeMs(dwCheckTime) > 20000))
        {
            nError = ERR_TRANS_AXIS2MOVE_TIMEOVER;
            ADD_LOG("[Auto Change Step=%d]: ERR_TRANS_AXIS2MOVE_TIMEOVER", m_nTransStep);
			ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/%d/%d/%f",
								ERR_TRANS_AXIS2MOVE_TIMEOVER,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pExecuteInfo->ExecutePositionInfo.NextNode,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_pOHTMainStatus->StatusCommon.ExistFoup,
								m_pOHTMainStatus->StatusCommon.StopStationID,
								m_nTransType,
								m_nTransStep,
								m_pHWSet->m_pHoistAxis->GetCurrentPosition());
        }
	break;

	case STEP_AUTOCHANGE_SHIFT_MOVE:
		// Shift, Rotate 축 원점 이동
		if(nError == NO_ERR)
		{
			nError = Move_ShiftRotate(SHIFTROTATE_TP2HOME);
		}
		if(nError == NO_ERR)
		{
			m_nTransStep++;
            dwCheckTime = (DWORD)timeGetTime();
		}
	break;

	case STEP_AUTOCHANGE_SHIFT_CHECK:
		// Shift, Rotate 축 원점 이동 Check
		if(nError == NO_ERR)
		{
			nError = Check_ShiftRotate();
		}

        if((nError == NO_ERR) && (OHTUTILITY::PreventOverflowGetTimeMs(dwCheckTime) > 10000))
        {
            nError = ERR_TRANS_AXIS3MOVE_TIMEOVER;
            ADD_LOG("[Auto Change Step=%d]: ERR_TRANS_AXIS3MOVE_TIMEOVER", m_nTransStep);
			ADD_FDC_LOG("ERR/%d/3/8/%06d/%06d/%d/%d/%06d/%d/%d/%f",
								ERR_TRANS_AXIS3MOVE_TIMEOVER,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pExecuteInfo->ExecutePositionInfo.NextNode,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_pOHTMainStatus->StatusCommon.ExistFoup,
								m_pOHTMainStatus->StatusCommon.StopStationID,
								m_nTransType,
								m_nTransStep,
								m_pHWSet->m_pShiftAxis->GetCurrentPosition());
        }
	break;



#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))&& (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))

	case STEP_AUTOCHANGE_PRE_SHIFT_MOVE:
	{
#if(OHT_NAME == OHT_NAME_STD_V85)
#else
		nError = Move_EUV_Shift();
#endif

		if(nError == NO_ERR)
		{				
			m_nTransStep++;
		}
	}
	break;
	case STEP_AUTOCHANGE_PRE_SHIFT_CHECK:
	{
#if(OHT_NAME == OHT_NAME_STD_V85)
		nError = Check_EUV_Shift();
#else
#endif
		if(nError == NO_ERR)
			m_nTransStep++;
	}
	break;
#endif

	case STEP_AUTOCHANGE_ROTATE_MOVE:
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
		if(nError == NO_ERR)
		{
			if(m_pHWSet->m_pShutter->IsOpen(BOTH) != true)
			{
				nError = m_pHWSet->m_pShutter->Open();
			}
		}

		if((nError == NO_ERR) && (m_pHWSet->m_pShutter->IsOpen(BOTH) == true))
		{
			if(m_pHWSet->m_pRotate->IsOrigin() == true)
			{
				nError = m_pHWSet->m_pRotate->MoveAbsolute(0);
			}
			else
			{
				nError = m_pHWSet->m_pRotate->Zeroing();
			}

			if(nError == NO_ERR)
			{
				dwMoveSRStartTime = timeGetTime();
				nRotateInpoCnt = 0;
				m_nTransStep++;
			}
		}
#else
		m_nTransStep++;
#endif
	break;

	case STEP_AUTOCHANGE_ROTATE_CHECK:
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
		if(nError == NO_ERR)
		{
			if(m_pHWSet->m_pShutter->IsOpen(BOTH) != true)
				nError = ERR_SHUTTER_NOTORIGIN;
		}

		if(nError == NO_ERR)
		{
			if(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > ROTATE_INPOS_LIMIT)
			{
				nRotateInpoCnt = 0;
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwMoveSRStartTime) > (ROTATE_EXEC_TIMEOUT * 4))
				{
					nError = ERR_ROTATE_NOT_0;
				}
			}
			else
			{
				if(nRotateInpoCnt > 100)
					m_nTransStep++;
				else
					nRotateInpoCnt++;
			}
		}
#else
		m_nTransStep++;
#endif
	break;

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	case STEP_AUTOCHANGE_RETURN_SHIFT_MOVE:
	{
		if(fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) <= RETICLE_SHIFT_INPOS_LIMIT)
		{
			m_nTransStep = STEP_MOVE_HOME_HAND_SHUTTER_MOVE;
		}
		else
		{	/* add Shift가 Home 위치가 아닌 경우 */
			double dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_10P;
			double dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_ACCTIME;
			nError = m_pHWSet->m_pShiftAxis->MoveDistance
				 (dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);
			m_dCmdShiftPos = 0.0; // target position
			m_nTransStep++;
		}		
	}
	break;
	case STEP_AUTOCHANGE_RETURN_SHIFT_CHECK:
	{
		nError = Check_EUV_Shift();
		if(nError == NO_ERR)
			m_nTransStep++;
	}
	break;
#endif

	case STEP_AUTOCHANGE_HAND_SHUTTER_MOVE:
		// Shutter Check 및 Close
		if(nError == NO_ERR)
		{
			if((m_pHWSet->m_pHand->IsRightGripOn() == true) && (m_pHWSet->m_pHand->IsLeftGripOn() == true)
			&& (m_pHWSet->m_pInnerFoup->IsFoupDetect() == true))
			{
				nError = m_pHWSet->m_pShutter->Close();
			}
			else if((m_pHWSet->m_pHand->IsRightGripOn() == false) && (m_pHWSet->m_pHand->IsLeftGripOn() == false)
                 && (m_pHWSet->m_pInnerFoup->IsFoupDetect() == false))
			{
				nError = m_pHWSet->m_pShutter->Open();
				if(nError == NO_ERR)
				{
					nError = m_pHWSet->m_pHand->AutoOpen();
				}
			}
			else
			{
				nError = ERR_GRIP_SEN_MISMATCH;
        		ADD_LOG("[Auto Change Step=%d] E : %d(Carrier Detect Mismatch)", m_nTransStep, nError);
				ADD_FDC_LOG("ERR/%d/2/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_GRIP_SEN_MISMATCH,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHand->IsLeftGripOn(),
							m_pHWSet->m_pHand->IsRightGripOn());
			}

			if(nError == NO_ERR)
			{
				m_nTransStep++;
			}
		}
	break;

    case STEP_AUTOCHANGE_B4CHECK_MARK:
		if(nError == NO_ERR)
		{
			nError = Check_Complete();

		}
		if(nError == NO_ERR)
		{
		   m_nTransStep++;
		   m_dwSTBData_Time = timeGetTime(); //som
		   m_dwSTBTrigger_Time = timeGetTime();
		}
    break;

	case STEP_AUTOCHANGE_MARK_SELECT:
        if(m_pHWSet->m_pLeftQRR->Connect() == true || m_pHWSet->m_pRightQRR->Connect() == true)
		{
			m_pHWSet->m_pLeftQRR->SetStaticQRDataTrigger(false);
			ADD_LOG("m_pLeftQRR SetStaticQRDataTrigger false");

			m_pHWSet->m_pRightQRR->SetStaticQRDataTrigger(false);
			ADD_LOG("m_pRightQRR SetStaticQRDataTrigger false");
			m_dwSTBData_Time = timeGetTime(); //som
			m_dwSTBTrigger_Time = timeGetTime();

		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwSTBTrigger_Time) > 250)
			{
				m_pHWSet->m_pLeftQRR->SetStaticQRDataTrigger(true);
				ADD_LOG("m_pLeftQRR SetStaticQRDataTrigger");

				m_pHWSet->m_pRightQRR->SetStaticQRDataTrigger(true);
				ADD_LOG("m_pRightQRR SetStaticQRDataTrigger");

				m_dwSTBTrigger_Time = timeGetTime();
			}
		}

		if(m_pHWCommon->m_Input.STOP_Driving_Front == true)
        {
			m_nTransStep = STEP_AUTOCHANGE_NODE_MARK_RUN;
		}
		else if((m_pHWCommon->m_Input.STOP_Trans_Front == true) && (m_pHWCommon->m_Input.STOP_Trans_Rear == true))
		{
			m_nTransStep = STEP_AUTOCHANGE_STATION_MARK_RUN;
		}
		else if(m_pHWSet->m_pLeftQRR->Connect() == true)
		{   ADD_LOG("Connect() == true left");
			m_nTransStep = STEP_AUTOCHANGE_QR_LEFT_MARK_RUN;
		}
		else if(m_pHWSet->m_pRightQRR->Connect()==true)
		{   ADD_LOG("Connect() == true Right");
			m_nTransStep = STEP_AUTOCHANGE_QR_RIGHT_MARK_RUN;
		}
		else
		{
			ADD_LOG("%d", OHTUTILITY::PreventOverflowGetTimeMs(m_dwSTBData_Time));
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwSTBData_Time) > 1000)
			{
				nError = ERR_MARK_COMMAND;
				m_dwSTBData_Time = timeGetTime();
				ADD_LOG("[Auto Change Step=%d] Not Find Stop Sensor", m_nTransStep);
			        ADD_FDC_LOG("ERR/%d/1/6/%06d/%06d/%d/%d/%06d/%06x/%d",
						ERR_MARK_COMMAND,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						m_pExecuteInfo->ExecutePositionInfo.CurrStation);
			}

		}
	break;

	case STEP_AUTOCHANGE_NODE_MARK_RUN:
 		if(m_StatusSet.uError == NO_ERR)
		{
			 m_pHWSet->m_pLeftQRR->SetStaticQRDataTrigger(false);
			 ADD_LOG("m_pLeftQRR SetStaticQRDataTrigger Node Mark");
			 m_pHWSet->m_pRightQRR->SetStaticQRDataTrigger(false);
			 ADD_LOG("m_pRightQRR SetStaticQRDataTrigger Node Mark");
			 nError = m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_NODE);
        }

        if(nError != NO_ERR)
        {
    		ADD_LOG("[Auto Change Step=%d] E : %d(Driving Mark Fail)", m_nTransStep, nError);
            break;
        }
        else if(m_pVirtualDrivingControl->GetError() != NO_ERR)
        {
            nError = m_pVirtualDrivingControl->GetError();
    		ADD_LOG("[Auto Change Step=%d] E : %d(Driving Mark Fail)", m_nTransStep, nError);
            break;
        }

        if((m_pVirtualDrivingControl->GetStatus() == STS_COMPLETE) && (m_pVirtualDrivingControl->IsOnProcess() == false))
        {
            m_nTransStep = STEP_AUTOCHANGE_COMPLETE;
        }
	break;

	case STEP_AUTOCHANGE_STATION_MARK_RUN:
		if(m_StatusSet.uError == NO_ERR)
		{
			m_pHWSet->m_pLeftQRR->SetStaticQRDataTrigger(false);
			ADD_LOG("m_pLeftQRR SetStaticQRDataTrigger Station Mark");
			m_pHWSet->m_pRightQRR->SetStaticQRDataTrigger(false);
		  	ADD_LOG("m_pRightQRR SetStaticQRDataTrigger Station Mark");
			nError = m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_STATION_STOP);
		}

        if(nError != NO_ERR)
        {
    		ADD_LOG("[Auto Change Step=%d] E : %d(Station Mark Fail)", m_nTransStep, nError);
            break;
        }
        else if(m_pVirtualDrivingControl->GetError() != NO_ERR)
        {
            nError = m_pVirtualDrivingControl->GetError();
    		ADD_LOG("[Auto Change Step=%d] E : %d(Station Mark Fail)", m_nTransStep, nError);
            break;
        }

        if((m_pVirtualDrivingControl->GetStatus() == STS_COMPLETE) && (m_pVirtualDrivingControl->IsOnProcess() == false))
        {
            m_nTransStep = STEP_AUTOCHANGE_COMPLETE;
        }
	break;

	case STEP_AUTOCHANGE_QR_LEFT_MARK_RUN:
		if(m_StatusSet.uError == NO_ERR)
		{

			nError = m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_STATION_QR_LEFT);
				ADD_LOG("STEP_AUTOCHANGE_QR_LEFT_MARK_RUN");
		}
		if(nError != NO_ERR)
		{
			ADD_LOG("[Auto Change Step=%d] E : %d(QR_Left Mark Fail)", m_nTransStep, nError);
			break;
		}
		else if(m_pVirtualDrivingControl->GetError() != NO_ERR)
		{
			nError = m_pVirtualDrivingControl->GetError();
			ADD_LOG("[Auto Change Step=%d] E : %d(QR_Left Mark Fail)", m_nTransStep, nError);
			break;
		}

		if((m_pVirtualDrivingControl->GetStatus() == STS_COMPLETE) && (m_pVirtualDrivingControl->IsOnProcess() == false))
		{
			m_nTransStep = STEP_AUTOCHANGE_COMPLETE;
		}
	break;

	case STEP_AUTOCHANGE_QR_RIGHT_MARK_RUN:
	if(m_StatusSet.uError == NO_ERR)
		{
			nError = m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_STATION_QR_RIGHT);
		}
		if(nError != NO_ERR)
		{
			ADD_LOG("[Auto Change Step=%d] E : %d(QR_Right Mark Fail)", m_nTransStep, nError);
			break;
		}
		else if(m_pVirtualDrivingControl->GetError() != NO_ERR)
		{
			nError = m_pVirtualDrivingControl->GetError();
			ADD_LOG("[Auto Change Step=%d] E : %d(QR_Right Mark Fail)", m_nTransStep, nError);
			break;
		}

		if((m_pVirtualDrivingControl->GetStatus() == STS_COMPLETE) && (m_pVirtualDrivingControl->IsOnProcess() == false))
		{
			m_nTransStep = STEP_AUTOCHANGE_COMPLETE;
		}
	break;
	case STEP_AUTOCHANGE_COMPLETE:
        if(nError == NO_ERR)
        {
			m_StatusSet.byStatus = STS_AUTO_COMPLETE;			
            m_StatusSet.bIsOnProcess = false;
        }
    break;
	}

    if(nError != NO_ERR)
    {
        m_StatusSet.uError = nError;
        m_StatusSet.byStatus = STS_ERROR;
    }

    return nError;
}


void TransControl::MarkTry(void)
{
	bAutoModeSkip = true;

/*
	if(m_pHWCommon->m_Input.STOP_Driving_Front == true)
	{
	   ADD_LOG("[Param Apply] Driving Mark Try...");
			
	   if(m_StatusSet.uError == NO_ERR)
	   {
	   	 ADD_LOG("[Param Apply] MANUAL_MARK_CMD_NODE");
		 m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_NODE);
	   }
	   else
       {
		 ADD_LOG("[Param Apply] Err. Mark Not Try");
	   }
	}
	else if((m_pHWCommon->m_Input.STOP_Trans_Front == true) && (m_pHWCommon->m_Input.STOP_Trans_Rear == true))
	{
	   if(m_StatusSet.uError == NO_ERR)
	   {
	     ADD_LOG("[Param Apply] MANUAL_MARK_CMD_STATION_STOP");
		 m_pVirtualDrivingControl->Manual_Mark(MANUAL_MARK_CMD_STATION_STOP);
	   }
	   else
       {
		 ADD_LOG("[Param Apply] Err. Mark Not Try");
       }
	}
*/
}

/**
 @brief   Hand, Shutter 상태 Check
          Hoist, Shift, Rotate S/W Limit 설정
 @author  Labeler
 @date    2017.01.01
 @return  Home 이동 할 준비가 잘 되어 있으면 0, 문제가 있으면 해당 에러코드 리턴.
 */
int TransControl::ReadyAllHome()
{
	int nError = NO_ERR;
    int nCount = 0;
	DWORD dwStartTime = 0;

    // 1. Set Hoist, Shift limit
    if(nError == NO_ERR)
       	nError = m_pHWSet->m_pHoistAxis->SetPositiveSWLimit(fabs(m_pHWSet->m_pHoistAxis->GetCurrentPosition()) + HOIST_POS_LIMIT_TRANSOFFSET);

    if(nError == NO_ERR)
        nError = m_pHWSet->m_pHoistAxis->SetNegativeSWLimit(HOIST_NEG_LIMIT);

	if(nError == NO_ERR)
    {
        double dShift_CurPos = m_pHWSet->m_pShiftAxis->GetCurrentPosition();

        if(dShift_CurPos >= 0.0)
        {
            nError = m_pHWSet->m_pShiftAxis->SetPositiveSWLimit(dShift_CurPos + SHIFT_POS_LIMIT_FORWAIT);

            if(nError == NO_ERR)
                nError = m_pHWSet->m_pShiftAxis->SetNegativeSWLimit(SHIFT_NEG_LIMIT_FORWAIT);
        }
        else
        {
            nError = m_pHWSet->m_pShiftAxis->SetPositiveSWLimit(SHIFT_POS_LIMIT_FORWAIT);
            if(nError == NO_ERR)
                nError = m_pHWSet->m_pShiftAxis->SetNegativeSWLimit(dShift_CurPos + SHIFT_NEG_LIMIT_FORWAIT);
        }
    }

	// 2. hand check
	if(nError == NO_ERR)
	{
		if(m_TransUse.HandSensor == true)
		{
            // 2.1. Hand Go signal Check
            if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsGoSigOn() == false))
            {
                nCount = 0;
                while(m_pHWSet->m_pHand->IsGoSigOn() == false)
                {
                    if(m_pHWSet->m_pHand->IsGoSigOn() == true) // Foup 존재
                    {
                        nCount = 0;
                        break;
                    }
                    else
                    {
                        nCount++;
                    }
                    if(nCount > 5)
                    {
                        nError = ERR_HANDPIO_GO_OFF;
                        nCount = 0;
                        ADD_LOG("[ReadyAllHome] E : %d(ERR_HANDPIO_GO_OFF)", nError);
						ADD_FDC_LOG("ERR/%d/2/6/%06d/%06d/%d/%d/%06d/%d",
									ERR_HANDPIO_GO_OFF,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_pHWSet->m_pHand->IsGoSigOn());
                        break;
                    }
                    Sleep(10);
                }
            }

            // 2.2. Hand Grip Mismatch Check
            if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsLeftGripOn() != m_pHWSet->m_pHand->IsRightGripOn()))
            {
                nCount = 0;
                while(m_pHWSet->m_pHand->IsLeftGripOn() != m_pHWSet->m_pHand->IsRightGripOn())
                {
                    if(nCount > 10)
                    {
                        nError = ERR_GRIP_SEN_MISMATCH;
                        nCount = 0;
                        ADD_LOG("[ReadyAllHome] E : %d(ERR_GRIP_SEN_MISMATCH)", nError);
						ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_GRIP_SEN_MISMATCH,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHand->IsLeftGripOn(),
							m_pHWSet->m_pHand->IsRightGripOn());
                        break;
                    }
                    else
                        nCount++;
                    Sleep(10);
                }
            }

            // 2.3. Hand Open, Close Position 모두 미감지(또는 감지) 시 Origin 수행
			if((nError == NO_ERR) && (m_pHWSet->m_pHand->IsOpen() == m_pHWSet->m_pHand->IsClose()))
            {
                nError = m_pHWSet->m_pHand->Origin();
                if(nError != NO_ERR)
                    ADD_LOG("[ReadyAllHome] E : %d(Hand Origin)", nError);
			}

            // 2.4. Hand Open, Close Position에서 미 Origin 상태일 경우, Origin 수행
            if(nError == NO_ERR)
            {
                if(m_pHWSet->m_pHand->IsReady() == false)
				{
                    nError = m_pHWSet->m_pHand->Origin();
                    if(nError != NO_ERR)
                        ADD_LOG("[ReadyAllHome] E : %d(Hand Origin)", nError);
                }
            }

            // 2.5. Hand Open Position일 때, Gripper 상태 Check
            if(nError == NO_ERR)
            {
                if(m_pHWSet->m_pHand->IsOpen() == true)
				{
                    if((m_pHWSet->m_pHand->IsLeftGripOn() == true) || (m_pHWSet->m_pHand->IsRightGripOn() == true))
                    {
                        nError = ERR_GRIPPER_SEN_ON_IN_HAND_OPEN;
                        ADD_LOG("[ReadyAllHome] E : %d(ERR_GRIPPER_SEN_ON_IN_HAND_OPEN)", nError);
                    }
                    else if(m_pHWSet->m_pInnerFoup->IsFoupDetect() == true)
                    {
						//nError = ERR_INNER_FOUP_DETECT;
						//ADD_LOG("[ReadyAllHome] E : %d(ERR_INNER_FOUP_DETECT)", nError);
					}
                }
            }

            // 2.6. Hand Alarm 상태 Check
            if(nError == NO_ERR)
            {
                if(m_pHWSet->m_pHand->IsAlarm() == true)
                {
                    nError = m_pHWSet->m_pHand->AlarmReset();
                    if(nError != NO_ERR)
                        ADD_LOG("[ReadyAllHome] E : %d(Hand Alarm reset)", nError);
                }
            }
        }
    }


	// 3. Shutter Checkshutter
    if(nError == NO_ERR)
	{
        if(m_TransUse.ShutterControl == true)
        {
            // 3.1. Shutter Position Mis-match Check
			if((m_pHWSet->m_pShutter->IsOpen(FRONT) != m_pHWSet->m_pShutter->IsOpen(REAR)) ||
				(m_pHWSet->m_pShutter->IsClose(FRONT) != m_pHWSet->m_pShutter->IsClose(REAR)))
            {
				nError = ERR_SHUTTER_DIFFSTATUS;
            }

            if(nError != NO_ERR)
			{
                nError = 0;
                dwStartTime = timeGetTime();
                while(m_pHWSet->m_pShutter->IsOpen(FRONT) != m_pHWSet->m_pShutter->IsOpen(REAR))
                {
					if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)SHUTTER_MOTION_LIMIT_TIME)
						break;
                    Sleep(10);
                }

                if((m_pHWSet->m_pShutter->IsOpen(FRONT) != m_pHWSet->m_pShutter->IsOpen(REAR)) ||
                    (m_pHWSet->m_pShutter->IsClose(FRONT) != m_pHWSet->m_pShutter->IsClose(REAR)))
                {
					nError = ERR_SHUTTER_DIFFSTATUS;
                    ADD_LOG("[ReadyAllHome] E : %d(ERR_SHUTTER_DIFFSTATUS)", nError);
            }
        }

            // 3.2. Shutter Unknown Position Check
            if((nError == NO_ERR)
            && (m_pHWSet->m_pShutter->IsClose(FRONT) == false) && (m_pHWSet->m_pShutter->IsClose(REAR) == false)
            && (m_pHWSet->m_pShutter->IsOpen(FRONT) == false) && (m_pHWSet->m_pShutter->IsOpen(REAR) == false))
            {
                nError = ERR_SHR_F_NO_SEN_AND_R_NO_SEN;
                ADD_LOG("[ReadyAllHome] E : %d(ERR_SHR_F_NO_SEN_AND_R_NO_SEN)", nError);
				ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
							ERR_SHR_F_NO_SEN_AND_R_NO_SEN,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_pHWSet->m_pShutter->IsOpen(FRONT),
							m_pHWSet->m_pShutter->IsClose(FRONT),
							m_pHWSet->m_pShutter->IsOpen(REAR),
							m_pHWSet->m_pShutter->IsClose(REAR));
		    }

            // 3.3. Hoist, Shift, Rotate 중 하나라도 Home이 아닐 경우 Shutter Open
			if(nError == NO_ERR)
			{
				if(((m_pHWSet->m_pShutter->IsClose(FRONT) == true) || (m_pHWSet->m_pShutter->IsClose(REAR) == true))
		                && ((m_pHWSet->m_pHoistAxis->IsHoistHome() == false)
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
		        || (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) > INPOS_LIMIT)
		        || (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) > ROTATE_INPOS_LIMIT)
#else
		        || (m_pHWSet->m_pShiftAxis->IsShiftHome() == false)
#endif
				))
				{
	                if(m_pHWSet->m_pShutter->ReadyToOperation() == NO_ERR)
                	{
		                nError = m_pHWSet->m_pShutter->Open_Cmd();
                        if(nError != NO_ERR)
			                ADD_LOG("[ReadyAllHome] E : %d(Shutter Open)", nError);
                	}
                	else
                	{
                    	nError = m_pHWSet->m_pShutter->Origin();
                        if(nError != NO_ERR)
		                    ADD_LOG("[ReadyAllHome] E : %d(Shutter Origin)", nError);
        	        }

					if(nError == NO_ERR)
					{
		                while ((nError = m_pHWSet->m_pShutter->CheckComplete()) == NEXT_CHECK)
		                {
		                    Sleep(10);
		                }
		    		}
				}
    		}
        }
	}

    // 4. Hoist, Shift, Rotate Amp Off Check
    if(nError == NO_ERR)
    {
        if(m_pHWSet->m_pHoistAxis->IsAMPEnable() == false)
        {
            nError = ERR_AXIS2_SERVO_OFFED;
			ADD_LOG("[Check_State] E : %d(ERR_AXIS2_SERVO_OFFED)", nError);
			ADD_FDC_LOG("ERR/%d/3/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
							ERR_AXIS2_SERVO_OFFED,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pHoistAxis->IsAMPEnable(),
							m_pHWSet->m_pHoistAxis->GetCurrentPosition());
		}

		if((nError == NO_ERR) && (m_pHWSet->m_pShiftAxis->IsAMPEnable() == false))
		{
			nError = ERR_AXIS3_SERVO_OFFED;
			ADD_LOG("[Check_State] E : %d(ERR_AXIS3_SERVO_OFFED)", nError);
                        ADD_FDC_LOG("ERR/%d/3/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%f",
							ERR_AXIS3_SERVO_OFFED,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nTransType,
							m_nTransStep,
							m_pHWSet->m_pShiftAxis->IsAMPEnable(),
							m_pHWSet->m_pShiftAxis->GetCurrentPosition());
		}

		if(m_pHWSet->m_pHoistAxis->GetCurrentPosition() > 180.0)
		{
			nError = ERR_HOIST_NOTORIGIN;
			ADD_LOG("[Check_State] E : %d(ERR_AXIS2_SERVO_OFFED)", nError);
		}

// 2016-11-28, Add by Labeler, ROHT의 경우 Rotate Origin 신호 처리 추가
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
		if((nError == NO_ERR) && (m_pHWSet->m_pRotate->IsAMPEnable() == false))
		{
			nError = ERR_AXIS4_SERVO_OFFED;
			ADD_LOG("[Check_State] E : %d(ERR_AXIS4_SERVO_OFFED)", nError);
		}
#endif
	}
    return nError;
}


void TransControl::Reset_TransControl()
{
	OHTMainForm->Set_EQPIO_Type(m_pParamSet->TransUsePart.EQPIOType); //혼용 PIO	

	TransInit();

	CheckCarrierInterlock();
}



bool TransControl::Load_SpecialEQParameter(UINT CurStationID, TRANS_PARAM_SET* pParamSet)
{
	bool bResult = false;
	int nTmpValue = 0;
    int nTestOffset =0;
    int nTestPercent =0;
	double dTmpValue = 0.0;

	char strStationID[20] = {0,};

	sprintf(strStationID, "%u",CurStationID);


	LIniFile *pIniFile;
    pIniFile = NULL;
	pIniFile = new LIniFile("DataFiles\\SpecialEQ.ini");

    if(!pIniFile->GetIniFileExist())
    {
        delete pIniFile;
		pIniFile = NULL;

    }


	if(pIniFile != NULL)
	{

		pIniFile->ReadInteger("SpecialEQ", strStationID, 0, &nTmpValue);
		if(nTmpValue !=0)
		{

            pIniFile->ReadIntegerMinMaxCheck("SpecialEQ", "nDnState_UP_SpeedDown_Offset_SpecialEQ", 50, 0, 200,&nTestOffset);
            pIniFile->ReadIntegerMinMaxCheck("SpecialEQ", "nDnState_UP_SpeedDown_Percent_SpecialEQ", 100, 1, 100,&nTestPercent);

        // SpecialEQ에서 offset, speed ratio read
            ADD_LOG("offset : %d, speed ratio : %d",nTestOffset, nTestPercent);

            pParamSet->TransDefine.nDnState_UP_SpeedDown_Offset_SpecialEQ = nTestOffset;
            pParamSet->TransDefine.nDnState_UP_SpeedDown_Percent_SpecialEQ = nTestPercent;

			bResult = true;

		}

        delete pIniFile;
	}


	return  bResult;
 }

bool TransControl::Load_ReticleInTeaching(UINT CurStationID,RETICLE_TEACH_OFFSET* pRiticleTeachingOffset)
{
	bool bResult = false;
	int nTmpValue = 0;
	double dTmpValue = 0.0;

	char strStationID[20] = {0,};

	sprintf(strStationID, "%u",CurStationID);


	LIniFile *pIniFile;
    pIniFile = NULL;
	pIniFile = new LIniFile("DataFiles\\ReInTe.txt");

    if(!pIniFile->GetIniFileExist())
    {
        delete pIniFile;
		pIniFile = NULL;

    }

	if(pIniFile != NULL)
	{

		pIniFile->ReadIntegerMinMaxCheck(strStationID, "Rotate", 0, -20, 20,&nTmpValue);
		pRiticleTeachingOffset->nPortOffset_Rotate = nTmpValue;

		pIniFile->ReadDoubleMinMaxCheck(strStationID, "Hoist", 0.0, -3.0, 3.0,&dTmpValue);
		pRiticleTeachingOffset->dPortOffset_Hoist = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck(strStationID, "Shift", 0.0, -3.0, 3.0,&dTmpValue);
		pRiticleTeachingOffset->dPortOffset_Shift = dTmpValue;

		bResult = true;
		delete pIniFile;
	}



	return  bResult;
 }
 
 
#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
// EUV POD는 회전 동작(Origin, Move) 시 간섭으로 Shift 10mm 안쪽 이동 후 동작이 필요.
void TransControl::Set_CmdEuvRotateOrigin(int iCmd)
{
    m_iCmdEuvRotateOrg = iCmd;
    ADD_LOG("[EUV POD] Rotate Origin: Set Command");
    return;
}

void TransControl::Set_CmdEuvRotateOrigin()
{
    if(IsRun_EuvRotateOrigin() == false)
    {
        Set_CmdEuvRotateOrigin(CMD_ROTATE_ORIGIN_CMD);
    }
    return;
}

bool TransControl::IsRun_EuvRotateOrigin()
{
    bool bRun = false;
    bRun |= (m_iCmdEuvRotateOrg != CMD_ROTATE_RESET);
    bRun |= (m_iStepEuvRotateOrg != STEP_ROTATE_NO_WORK);
    return bRun;
}

int TransControl::Seq_EuvRotate_Origin()
{
    int nError = NO_ERR;
    static DWORD dwStartTime = 0;

	// Ccmmand process
	switch(m_iCmdEuvRotateOrg)
	{
		case CMD_ROTATE_RESET:
		{
			m_iStepEuvRotateOrg = STEP_ROTATE_NO_WORK;
			return NO_ERR;
		}
		break;

		case CMD_ROTATE_ORIGIN_CMD:
		{
			ADD_LOG("[EUV POD] Rotate Origin: SM Start");
            m_iCmdEuvRotateOrg = CMD_ROTATE_SM_PROG;
			m_iStepEuvRotateOrg = STEP_ROTATE_SHIFT_GO;
			return NO_ERR;
		}
		break;

		case CMD_ROTATE_SM_PROG:
		{
			if((m_StatusSet.uError != NO_ERR) || (m_StatusSet.byStatus == STS_ERROR))
            {
                m_iCmdEuvRotateOrg = CMD_ROTATE_RESET;
                m_iStepEuvRotateOrg = STEP_ROTATE_NO_WORK;
                return NO_ERR;
            }
		}
	}

	// step process
	switch(m_iStepEuvRotateOrg)
	{
		case  STEP_ROTATE_NO_WORK:
		{
            if(m_iCmdEuvRotateOrg != CMD_ROTATE_RESET)
            {
                m_iCmdEuvRotateOrg = CMD_ROTATE_RESET;
            }
		}
		break;

		case  STEP_ROTATE_SHIFT_GO:
		{
#if(OHT_NAME == OHT_NAME_STD_V85)
#else
            nError = Move_EUV_Shift();
#endif

            ADD_LOG("[EUV POD] Rotate Origin: Move Shift E(%d)", nError);

            if(nError == NO_ERR)
            {
                m_iStepEuvRotateOrg++;
            }
		}
		break;

		case  STEP_ROTATE_SHIFT_WAIT:
		{
#if(OHT_NAME == OHT_NAME_STD_V85)
#else
			nError = Check_EUV_Shift();
#endif

            ADD_LOG("[EUV POD] Rotate Origin: Check Shift E(%d)", nError);

			if(nError == NO_ERR)
            {
                m_iStepEuvRotateOrg++;
            }
		}
		break;

		case  STEP_ROTATE_ORIGIN_GO:
		{
			nError = m_pHWSet->m_pRotate->Zeroing();
            ADD_LOG("[EUV POD] Rotate Origin: Run Zeroing E(%d)", nError);

			if(nError == NO_ERR)
			{
                dwStartTime = timeGetTime();
				m_iStepEuvRotateOrg++;
			}
		}
		break;

		case STEP_ROTATE_ORIGIN_WAIT:
		{
            if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 1000)
            {
                // Wait 1 sec...
                if ((m_pHWSet->m_pRotate->IsOrigin() == true) &&
                    (fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree()) < ROTATE_INPOS_LIMIT))
                {
                    m_iStepEuvRotateOrg++;
                    ADD_LOG("[EUV POD] Rotate Origin: Zeroing Done");
                }
                else if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 10000)
                {
                    nError = ERR_ROTATE_MOVE_TIME_OUT;
                }
            }
		}
		break;

		case  STEP_ROTATE_SHIFT_RETURN_GO:
		{
            double dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_30P;
            double dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_NC_ACCTIME;
            nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);

			if(nError == NO_ERR)
			{
                dwStartTime = timeGetTime();
				m_iStepEuvRotateOrg++;
                ADD_LOG("[EUV POD] Rotate Origin: Return to Shift Origin Pos");
			}
		}
		break;

		case  STEP_ROTATE_SHIFT_RETURN_WAIT:
		{
			if ((m_pHWSet->m_pShiftAxis->IsShiftHome() == true) &&
                (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < RETICLE_SHIFT_INPOS_LIMIT))
            {
                m_iStepEuvRotateOrg = STEP_ROTATE_NO_WORK;
                ADD_LOG("[EUV POD] Rotate Origin: Shift move Done");
            }
            else if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 10000)
            {
				nError = ERR_SHIFT_HOMESEN_NOTON;
            }
		}
		break;
	}

    if(nError != NO_ERR)
    {
        m_StatusSet.uError = nError;
        m_StatusSet.byStatus = STS_ERROR;

        ADD_LOG("EuvRotate Origin Fail[Step=%d] : E(%d)", m_iStepEuvRotateOrg, nError);
        m_iStepEuvRotateOrg = STEP_ROTATE_NO_WORK;
    }

	return nError;
}


void TransControl::Set_CmdEuvRotateMove(int iCmd, int nTargetPos)
{
    m_iCmdEuvRotateMov = iCmd;
    m_iTargetPosEuvRotateMov = nTargetPos;
    ADD_LOG("[EUV POD] Rotate Move : Set Command (Target= %d)", m_iTargetPosEuvRotateMov);
    return;
}

void TransControl::Set_CmdEuvRotateMove(int nTargetPos)
{
    if(IsRun_CmdEuvRotateMoveState() == false)
    {
        Set_CmdEuvRotateMove(CMD_EUV_ROT_MOVE_CMD, nTargetPos);
    }
    return;
}

bool TransControl::IsRun_CmdEuvRotateMoveState()
{
    bool bRun = false;
    bRun |= (m_iCmdEuvRotateMov != CMD_EUV_ROT_RESET);
    bRun |= (m_iStepEuvRotateMov != STEP_EUV_ROT_NO_WORK);
    return bRun;
}

int TransControl::Seq_EuvRotate_Move()
{
    int nError = NO_ERR;
    static DWORD dwStartTime = 0;

    // command process
	switch(m_iCmdEuvRotateMov)
	{
		case CMD_EUV_ROT_RESET:
		{
			m_iStepEuvRotateMov = STEP_EUV_ROT_NO_WORK;
			return NO_ERR;
		}
		break;

		case CMD_EUV_ROT_MOVE_CMD:
		{
			ADD_LOG("[EUV POD] Rotate TP Move: SM Start (Target= %d)", m_iTargetPosEuvRotateMov);
            m_iCmdEuvRotateMov = CMD_EUV_ROT_SM_PROG;
			m_iStepEuvRotateMov = STEP_EUV_ROT_SHIFT_GO;
			return NO_ERR;
		}
		break;

		case CMD_EUV_ROT_SM_PROG:
		{
			if((m_StatusSet.uError != NO_ERR) || (m_StatusSet.byStatus == STS_ERROR))
            {
                m_iCmdEuvRotateOrg = CMD_ROTATE_RESET;
                m_iStepEuvRotateOrg = STEP_ROTATE_NO_WORK;
                return NO_ERR;
            }
		}
	}

    // step process
	switch(m_iStepEuvRotateMov)
	{
		case  STEP_EUV_ROT_NO_WORK:
		{
			if(m_iCmdEuvRotateMov != CMD_EUV_ROT_RESET)
            {
                m_iCmdEuvRotateMov = CMD_EUV_ROT_RESET;
            }
		}
		break;

        case  STEP_EUV_ROT_SHIFT_GO:
		{
#if(OHT_NAME == OHT_NAME_STD_V85)
#else
            nError = Move_EUV_Shift();
#endif
            ADD_LOG("[EUV POD] Rotate Move: Move Shift E(%d)", nError);

            if(nError == NO_ERR)
            {
                m_iStepEuvRotateMov++;
            }
        }
        break;

        case STEP_EUV_ROT_SHIFT_WAIT:
		{
#if(OHT_NAME == OHT_NAME_STD_V85)
#else
			nError = Check_EUV_Shift();
#endif
            ADD_LOG("[EUV POD] Rotate Move: Check Shift E(%d)", nError);

			if(nError == NO_ERR)
            {
                m_iStepEuvRotateMov++;
            }
        }
        break;

        case STEP_EUV_ROT_ROTATION_GO:
        {
            nError = m_pHWSet->m_pRotate->MoveAbsolute((int)m_iTargetPosEuvRotateMov);
            ADD_LOG("[EUV POD] Rotate Move: Run Rotate Move E(%d)", nError);

			if(nError == NO_ERR)
            {
                dwStartTime = timeGetTime();
                m_iStepEuvRotateMov++;
            }
        }
        break;

        case STEP_EUV_ROT_ROTATION_WAIT:
        {
            if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 1000)
            {
                // Wait 1 sec...
                if (fabs(fabs(m_pHWSet->m_pRotate->GetCurrentPositionDegree())-fabs((int)m_iTargetPosEuvRotateMov)) <= ROTATE_INPOS_LIMIT)
                {
                    m_iStepEuvRotateMov++;
                    ADD_LOG("[EUV POD] Rotate Move: Move Done");
                }
                else if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 10000)
                {
                    nError = ERR_ROTATE_MOVE_TIME_OUT;
                }
            }
        }
        break;

        case STEP_EUV_ROT_SHIFT_RETURN_GO:
        {
            double dVel_mmpermsec = SHIFT_MAX_SPEED * MULTIPLY_34_30P;
            double dAccel_mmpermsec2 = dVel_mmpermsec / SHIFT_NC_ACCTIME;
            nError = m_pHWSet->m_pShiftAxis->MoveDistance(dAccel_mmpermsec2, dAccel_mmpermsec2, dVel_mmpermsec, 0.0);

			if(nError == NO_ERR)
			{
                dwStartTime = timeGetTime();
				m_iStepEuvRotateMov++;
                ADD_LOG("[EUV POD] Rotate Move: Return to Shift Origin Pos");
			}
        }
        break;

        case STEP_EUV_ROT_SHIFT_RETURN_WAIT:
        {
            if ((m_pHWSet->m_pShiftAxis->IsShiftHome() == true) &&
                (fabs(m_pHWSet->m_pShiftAxis->GetCurrentPosition()) < RETICLE_SHIFT_INPOS_LIMIT))
            {
                m_iStepEuvRotateMov = STEP_EUV_ROT_NO_WORK;
                ADD_LOG("[EUV POD] Rotate Move: Shift move Done");
            }
            else if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 10000)
            {
                nError = ERR_SHIFT_HOMESEN_NOTON;
            }
        }
        break;
    }

    if(nError != NO_ERR)
    {
        m_StatusSet.uError = nError;
        m_StatusSet.byStatus = STS_ERROR;

        ADD_LOG("Euv Rotate Move Fail[Step=%d] : E(%d)", m_iStepEuvRotateMov, nError);
        m_iStepEuvRotateMov = STEP_EUV_ROT_NO_WORK;
    }

	return nError;
}

#endif

int TransControl::Move_Hoist_HomePosition(double dAccel, double dDecel, double dVel)
{

	int nError = 0;
	unsigned int nHoistHomeIONum ;
	double dAccel_mmpermsec2 = dAccel;
	double dDecel_mmpermsec2 = dDecel;
	double dVel_mmpermsec = dVel;
	int nCheckCount = 0;
	DWORD dwSTime = timeGetTime();
	bool bIsHoistHome;
	double dSmalladdDis;

	m_dCmdFinal_HoistPos = m_dCmdHoistPos;

	nHoistHomeIONum = HOIST_SMALL_ADD_HOISTHOME;

	if(m_pHWSet->m_pHoistAxis->IsHoistHome() == false)
	{
		SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), HOIST_SMALL_ADD_HOISTHOME_LIMIT, dVel_mmpermsec);
		nError = Move_Hoist_SmallAdd(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos, nHoistHomeIONum, HOIST_SMALL_ADD_HOISTHOME_DISTANCE, false);
	}
	else
	{
		ADD_LOG("Small add fail.. alreay home, Pos: %f", m_pHWSet->m_pHoistAxis->GetCurrentPosition());
		m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
	}
	 // Small Add ?� ?Ǉ; 'Ǒ Sleep ß??
	//Sleep(50);

	return nError;
}



int TransControl::Move_Hoist_HomeSensorOffPosition(double dAccel, double dDecel, double dVel)
{

	int nError = 0;
	unsigned int nHoistHomeIONum ;
	double dAccel_mmpermsec2 = dAccel;
	double dDecel_mmpermsec2 = dDecel;
	double dVel_mmpermsec = dVel;
	int nCheckCount = 0;
	DWORD dwSTime = timeGetTime();
	bool bIsHoistHome;

	m_dCmdFinal_HoistPos = m_dCmdHoistPos;

	nHoistHomeIONum = HOIST_SMALL_ADD_HOISTHOME;

	if(m_pHWSet->m_pHoistAxis->IsHoistHome() == true)
	{
		SaveHoistMoveCmd(m_pHWSet->m_pHoistAxis->GetCurrentPosition(), 30.0, dVel_mmpermsec);
		// off ??? 2mm ???
		nError = Move_Hoist_SmallAdd(dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos, nHoistHomeIONum, 3.0, true);
		ADD_LOG(
		"[Move to home off pos] Acc:%05.5lf, Del:%05.5lf, Vel:%05.5lf, Pos:%05.5lf, IO:%d",
		 dAccel_mmpermsec2, dDecel_mmpermsec2, dVel_mmpermsec, m_dCmdHoistPos, nHoistHomeIONum);
	}
	else
	{
		m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
	}
	 // Small Add ??? ????; '?? Sleep ???
	//Sleep(50);
	 
	while((OHTUTILITY::PreventOverflowGetTimeMs(dwSTime) < 30000))
	{
		bIsHoistHome = m_pHWSet->m_pHoistAxis->IsHoistHome();
		//ADD_LOG("[Small Add] HomeSensor,POS/%d/%f", bIsHoistHome, m_pHWSet->m_pHoistAxis->GetCurrentPosition());
		if(bIsHoistHome == false)

			nCheckCount++;

		if((bIsHoistHome == false) &&(m_pHWSet->m_pHoistAxis->IsRunning() == false))
		{
			//if(nCheckCount >= 50)
			//{
				 m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
				 ADD_LOG("[SmallAddPOS:%f CNT:%d", m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
				 break;
			//}
		}
		// ??d?????? ??? ????￿? Move Stop ??? ???? (???? ???? ??�?? 80msec ?????? d????, ?????? ?? ??? ????? ??? ?? ??d ???)

		if(nCheckCount == 600) // 6sec ???? ???
		{
			m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
			ADD_LOG("[SmallAdd]ABNORMAL POS:%f CNT:%d", m_pHWSet->m_pHoistAxis->GetCurrentPosition(), nCheckCount);
		}

		Sleep(10);
	}

	if(m_pHWSet->m_pHoistAxis->IsHoistHome() == true) {
		m_pHWSet->m_pHoistAxis->MoveStop(HOIST_ESTOP_DECEL);
		ADD_LOG("Small add fail..  Pos: %f", m_pHWSet->m_pHoistAxis->GetCurrentPosition());
	}
	else
	{
		m_dCmdFinal_HoistPos = m_pHWSet->m_pHoistAxis->GetCurrentPosition();
		ADD_LOG("[SmallAdd_Pos] = %f", m_dCmdFinal_HoistPos);
	}
	return nError;
}


