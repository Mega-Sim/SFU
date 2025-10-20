/**
@file    TransControl.h
@version OHT 7.0
@brief   TransControl Class Header File
*/
//---------------------------------------------------------------------------

#ifndef TransControlH
#define TransControlH

#include <stdio.h>
#include "VirtualTransControl.h"
#include "HWSet.h"
#include "ExecuteInfo.h"
#include "LogHandler.h" // Log 작성 목적
#include "Def_TransIniParam.h"
//#include "LIniFile.h"
#include "Def_DefaultIniParam.h"
#include "Def_MCCParam.h"
#include "DiagManager.h"

#include "OHTMainStatus.h"

#include "AutoTeaching.h"

//#include "HWCommon.h"

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
#define HOIST_MOVELIMIT_P           4000.0        // mm   3200
#define HOIST_MOVELIMIT_N           -1.5          // mm
#define SHIFT_MOVELIMIT_P           20.0          // mm   -18(1호기) //450
#define SHIFT_MOVELIMIT_N           -20.0         // mm   +18(1호기)
#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
#define HOIST_MOVELIMIT_P           5400          // 기존 레티클이 4000 이었으나 이를 5500으로 늘림
#define HOIST_MOVELIMIT_N           -1.5          // mm
#define SHIFT_MOVELIMIT_P           25.0          // mm   -18(1호기) //450
#define SHIFT_MOVELIMIT_N           -25.0         // mm   +18(1호기)
#elif((OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S))
#define HOIST_MOVELIMIT_P           4000.0        // mm   3200    //
#define HOIST_MOVELIMIT_N           -1.5    	  // mm
#define SHIFT_MOVELIMIT_P           478.5 	//483.0     	// SW Limit은 P1L 호환성으로 인해 (O-EQ) 478.5 / (EQ) -444mm (기구 Limit(O-EQ) 486mm/ (EQ) -447mm)
#define SHIFT_MOVELIMIT_N           -444.0	//-441.0        //
#elif(OHT_NAME == OHT_NAME_STD_V85)
#define HOIST_MOVELIMIT_P           5400.0        // mm   3200
#define HOIST_MOVELIMIT_N           -7.0         // mm
#define SHIFT_MOVELIMIT_P           405.0          // mm   -18(1ȣ?⩠//450
#define SHIFT_MOVELIMIT_N           -405.0         // mm   +18(1ȣ?⩍
#else
#define HOIST_MOVELIMIT_P           4000.0        // mm   3200    //
#define HOIST_MOVELIMIT_N           -1.5    	  // mm
#define SHIFT_MOVELIMIT_P           481.0 	//455.0     	// 추후 변경 필요.S/W Limit (O-EQ) 478.5mm / (EQ) 444mm (기구 Limit(O-EQ) 481.5mm / (EQ) 447mm)
#define SHIFT_MOVELIMIT_N           -445.0	//-465.0        //
#endif

#if(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
#define SHIFT_IDEALGEARRATIO            4096.0000000
#else
#define SHIFT_IDEALGEARRATIO             682.6666667
#endif
#define SHIFT_IDEALGEARRATIO_MIN         0.1

#define SHIFT_IDEAL2REALRATIO            1.0
#define SHIFT_IDEAL2REALRATIO_MIN        0.8

#define HOIST_MOVELIMIT_P_MAX_STB              150.0
#define HOIST_MOVELIMIT_N_MIN_STB              95.0
#define SHIFT_MOVELIMIT_N_MIN_STB_R            460.0
#define SHIFT_MOVELIMIT_N_MIN_STB_L            -410.0
// --------------------------------------------------------------------------
// LOAD 작업 STEP 관련 Define
// --------------------------------------------------------------------------
#ifdef USE_TRANS_PROFILE_VER2
#if(OHT_NAME == OHT_NAME_STD_V85)

#define STEP_L_EQPIO_CONNECT                0
#define STEP_L_LOAD_READY                   1
#define STEP_L_EQPIO_READY                  2

/* euv added */
#define STEP_L_MOVE_PRE_SHIFT_BEFORE_DOWN	3
#define STEP_L_CHECK_PRE_SHIFT_BEFORE_DOWN	4
#define STEP_L_MOVE_PRE_ROTATE_BEFORE_DOWN	5
#define STEP_L_CHECK_PRE_ROTATE_BEFORE_DOWN	6

#define STEP_L_MOVE_SHIFTROTATE             7
#define STEP_L_CHECK_MOVE_SHIFTROTATE       8
#define STEP_L_MOVE_DOWN_HOIST_FAST         9
#define STEP_L_CHECK_MOVE_DOWN_HOIST_FAST   10
#define STEP_L_MOVE_DOWN_HOIST_FAST_SLOWDOWN	11
#define STEP_L_CHECK_MOVE_DOWN_HOIST_FAST_SLOWDOWN	12
#define STEP_L_MOVE_DOWN_HOIST_SLOW         13
#define STEP_L_CHECK_MOVE_DOWN_HOIST_SLOW   14
#define STEP_L_CHECK_ARRIVE_TEACHING_POINT  15
#define STEP_L_CHECK_BEFORE_RELEASE_FOUP    16
#define STEP_L_RELEASE_FOUP                 17
#define STEP_L_MOVE_UP_HOIST_SLOW           18
#define STEP_L_CHECK_MOVE_UP_HOIST_SLOW     19
#define STEP_L_MOVE_UP_HOIST_FAST           20
#define STEP_L_CHECK_MOVE_UP_HOIST_FAST     21
#define STEP_L_MOVE_UP_HOIST_FAST_SLOWDOWN           22
#define STEP_L_CHECK_MOVE_UP_HOIST_FAST_SLOWDOWN     23
#define STEP_L_MOVE_UP_HOIST_HOME           24
#define STEP_L_CHECK_MOVE_UP_HOIST_HOME     25
#define STEP_L_MOVE_SHIFTROTATE_HOME        26
#define STEP_L_CHECK_MOVE_SHIFTROTATE_HOME  27
#define STEP_L_PIO_COMPLETE                 28
#define STEP_L_LOAD_COMPLETE                29
#elif((OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S))
#define STEP_L_EQPIO_CONNECT                0
#define STEP_L_LOAD_READY                   1
#define STEP_L_EQPIO_READY                  2
#define STEP_L_MOVE_SHIFTROTATE             3
#define STEP_L_CHECK_MOVE_SHIFTROTATE       4
#define STEP_L_MOVE_DOWN_HOIST_FAST         5
#define STEP_L_CHECK_MOVE_DOWN_HOIST_FAST   6
#define STEP_L_MOVE_DOWN_HOIST_FAST_SLOWDOWN	7
#define STEP_L_CHECK_MOVE_DOWN_HOIST_FAST_SLOWDOWN	8
#define STEP_L_MOVE_DOWN_HOIST_SLOW         9
#define STEP_L_CHECK_MOVE_DOWN_HOIST_SLOW   10
#define STEP_L_CHECK_ARRIVE_TEACHING_POINT  11
#define STEP_L_CHECK_BEFORE_RELEASE_FOUP    12
#define STEP_L_RELEASE_FOUP                 13
#define STEP_L_MOVE_UP_HOIST_SLOW           14
#define STEP_L_CHECK_MOVE_UP_HOIST_SLOW     15
#define STEP_L_MOVE_UP_HOIST_FAST           16
#define STEP_L_CHECK_MOVE_UP_HOIST_FAST     17
#define STEP_L_MOVE_UP_HOIST_FAST_SLOWDOWN           18
#define STEP_L_CHECK_MOVE_UP_HOIST_FAST_SLOWDOWN     19
#define STEP_L_MOVE_UP_HOIST_HOME           20
#define STEP_L_CHECK_MOVE_UP_HOIST_HOME     21
#define STEP_L_MOVE_SHIFTROTATE_HOME        22
#define STEP_L_CHECK_MOVE_SHIFTROTATE_HOME  23
#define STEP_L_PIO_COMPLETE                 24
#define STEP_L_LOAD_COMPLETE                25
#endif

#else
#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))

#define STEP_L_EQPIO_CONNECT                0
#define STEP_L_LOAD_READY                   1
#define STEP_L_EQPIO_READY                  2

/* euv added */
#define STEP_L_MOVE_PRE_SHIFT_BEFORE_DOWN	3
#define STEP_L_CHECK_PRE_SHIFT_BEFORE_DOWN	4
#define STEP_L_MOVE_PRE_ROTATE_BEFORE_DOWN	5
#define STEP_L_CHECK_PRE_ROTATE_BEFORE_DOWN	6

#define STEP_L_MOVE_SHIFTROTATE             7
#define STEP_L_CHECK_MOVE_SHIFTROTATE       8
#define STEP_L_MOVE_DOWN_HOIST_FAST         9
#define STEP_L_CHECK_MOVE_DOWN_HOIST_FAST   10
#define STEP_L_MOVE_DOWN_HOIST_SLOW         11
#define STEP_L_CHECK_MOVE_DOWN_HOIST_SLOW   12
#define STEP_L_CHECK_ARRIVE_TEACHING_POINT  13
#define STEP_L_CHECK_BEFORE_RELEASE_FOUP    14
#define STEP_L_RELEASE_FOUP                 15
#define STEP_L_MOVE_UP_HOIST_SLOW           16
#define STEP_L_CHECK_MOVE_UP_HOIST_SLOW     17
#define STEP_L_MOVE_UP_HOIST_FAST           18
#define STEP_L_CHECK_MOVE_UP_HOIST_FAST     19
#define STEP_L_MOVE_UP_HOIST_HOME           20
#define STEP_L_CHECK_MOVE_UP_HOIST_HOME     21
#define STEP_L_MOVE_SHIFTROTATE_HOME        22
#define STEP_L_CHECK_MOVE_SHIFTROTATE_HOME  23
#define STEP_L_PIO_COMPLETE                 24
#define STEP_L_LOAD_COMPLETE                25


#else

#define STEP_L_EQPIO_CONNECT                0
#define STEP_L_LOAD_READY                   1
#define STEP_L_EQPIO_READY                  2
#define STEP_L_MOVE_SHIFTROTATE             3
#define STEP_L_CHECK_MOVE_SHIFTROTATE       4
#define STEP_L_MOVE_DOWN_HOIST_FAST         5
#define STEP_L_CHECK_MOVE_DOWN_HOIST_FAST   6
#define STEP_L_MOVE_DOWN_HOIST_SLOW         7
#define STEP_L_CHECK_MOVE_DOWN_HOIST_SLOW   8
#define STEP_L_CHECK_ARRIVE_TEACHING_POINT  9
#define STEP_L_CHECK_BEFORE_RELEASE_FOUP    10
#define STEP_L_RELEASE_FOUP                 11
#define STEP_L_MOVE_UP_HOIST_SLOW           12
#define STEP_L_CHECK_MOVE_UP_HOIST_SLOW     13
#define STEP_L_MOVE_UP_HOIST_FAST           14
#define STEP_L_CHECK_MOVE_UP_HOIST_FAST     15
#define STEP_L_MOVE_UP_HOIST_HOME           16
#define STEP_L_CHECK_MOVE_UP_HOIST_HOME     17
#define STEP_L_MOVE_SHIFTROTATE_HOME        18
#define STEP_L_CHECK_MOVE_SHIFTROTATE_HOME  19
#define STEP_L_PIO_COMPLETE                 20
#define STEP_L_LOAD_COMPLETE                21

#endif
#endif
//---------------------------------------------------------------------------

// --------------------------------------------------------------------------
// UNLOAD 작업 STEP 관련 Define
// --------------------------------------------------------------------------

#ifdef USE_TRANS_PROFILE_VER2
#define STEP_U_HOISTSENSOR_ORIGIN_RECOVERY  -1
#define STEP_U_EQPIO_CONNECT                0
#define STEP_U_UNLOAD_READY                 1
#define STEP_U_EQPIO_READY                 	2
#define STEP_U_MOVE_SHIFTROTATE             3
#define STEP_U_CHECK_MOVE_SHIFTROTATE       4
#define STEP_U_MOVE_DOWN_HOIST_FAST         5
#define STEP_U_CHECK_MOVE_DOWN_HOIST_FAST   6
#define STEP_U_MOVE_DOWN_HOIST_FAST_SLOWDOWN	7
#define STEP_U_CHECK_MOVE_DOWN_HOIST_FAST_SLOWDOWN	8
#define STEP_U_MOVE_DOWN_HOIST_SLOW         9
#define STEP_U_CHECK_MOVE_DOWN_HOIST_SLOW   10
#define STEP_U_CHECK_ARRIVE_TEACHING_POINT  11
#define STEP_U_GRIP_FOUP                    12
#define STEP_U_MOVE_UP_HOIST_SLOW           13
#define STEP_U_CHECK_MOVE_UP_HOIST_SLOW     14
#define STEP_U_MOVE_UP_HOIST_FAST           15
#define STEP_U_CHECK_MOVE_UP_HOIST_FAST     16
#define STEP_U_MOVE_UP_HOIST_FAST_SLOWDOWN           17
#define STEP_U_CHECK_MOVE_UP_HOIST_FAST_SLOWDOWN     18
#define STEP_U_MOVE_UP_HOIST_HOME           19
#define STEP_U_CHECK_MOVE_UP_HOIST_HOME     20
#define STEP_U_MOVE_SHIFTROTATE_HOME        21
#define STEP_U_CHECK_MOVE_SHIFTROTATE_HOME  22
#define STEP_U_PIO_COMPLETE                 23
#define STEP_U_CHECK_ANTIDROP               24
#define STEP_U_UNLOAD_COMPLETE              25

#else
#define STEP_U_HOISTSENSOR_ORIGIN_RECOVERY  -1
#define STEP_U_EQPIO_CONNECT                0
#define STEP_U_UNLOAD_READY                 1
#define STEP_U_EQPIO_READY                 	2
#define STEP_U_MOVE_SHIFTROTATE             3
#define STEP_U_CHECK_MOVE_SHIFTROTATE       4
#define STEP_U_MOVE_DOWN_HOIST_FAST         5
#define STEP_U_CHECK_MOVE_DOWN_HOIST_FAST   6
#define STEP_U_MOVE_DOWN_HOIST_SLOW         7
#define STEP_U_CHECK_MOVE_DOWN_HOIST_SLOW   8
#define STEP_U_CHECK_ARRIVE_TEACHING_POINT  9
#define STEP_U_GRIP_FOUP                    10
#define STEP_U_MOVE_UP_HOIST_SLOW           11
#define STEP_U_CHECK_MOVE_UP_HOIST_SLOW     12
#define STEP_U_MOVE_UP_HOIST_FAST           13
#define STEP_U_CHECK_MOVE_UP_HOIST_FAST     14
#define STEP_U_MOVE_UP_HOIST_HOME           15
#define STEP_U_CHECK_MOVE_UP_HOIST_HOME     16
#define STEP_U_MOVE_SHIFTROTATE_HOME        17
#define STEP_U_CHECK_MOVE_SHIFTROTATE_HOME  18
#define STEP_U_PIO_COMPLETE                 19
#define STEP_U_CHECK_ANTIDROP               20
#define STEP_U_UNLOAD_COMPLETE              21
#endif
//---------------------------------------------------------------------------


#define EUV_MANUAL_SHIFT_ACCTIME          550   //msec
#define EUV_MANUAL_SHIFT_MAX_SPEED        0.475
#define EUV_MANUAL_MULTIPLY_34_10P        0.1

//---------------------------------------------------------------------------
//	Step Rotate Origin (EUV)
//---------------------------------------------------------------------------
#define CMD_ROTATE_RESET				0
#define CMD_ROTATE_ORIGIN_CMD			1
#define CMD_ROTATE_SM_PROG				2

#define STEP_ROTATE_NO_WORK     		0
#define STEP_ROTATE_SHIFT_GO    		1
#define STEP_ROTATE_SHIFT_WAIT			2
#define STEP_ROTATE_ORIGIN_GO			3
#define STEP_ROTATE_ORIGIN_WAIT			4
#define STEP_ROTATE_SHIFT_RETURN_GO		5
#define STEP_ROTATE_SHIFT_RETURN_WAIT	6

//---------------------------------------------------------------------------
//	Step Rotate (EUV)
//---------------------------------------------------------------------------
#define CMD_EUV_ROT_RESET				0
#define CMD_EUV_ROT_MOVE_CMD			1
#define CMD_EUV_ROT_SM_PROG				2

#define STEP_EUV_ROT_NO_WORK     		0
#define STEP_EUV_ROT_SHIFT_GO    		1
#define STEP_EUV_ROT_SHIFT_WAIT			2
#define STEP_EUV_ROT_ROTATION_GO		3
#define STEP_EUV_ROT_ROTATION_WAIT		4
#define STEP_EUV_ROT_SHIFT_RETURN_GO	5
#define STEP_EUV_ROT_SHIFT_RETURN_WAIT	6


// --------------------------------------------------------------------------
// Move Teaching Point 작업 STEP 관련 Define
// --------------------------------------------------------------------------
#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))

#define STEP_MOVE_TP_READY                          0
/* add */
#define STEP_MOVE_TP_PRE_SHIFTROTATE				1
#define STEP_MOVE_TP_CHECK_SHIFTROTATE              2
#define STEP_MOVE_TP_MOVE_SHIFTROTATE               3
#define STEP_MOVE_TP_CHECK_MOVE_SHIFTROTATE         4
#define STEP_MOVE_TP_MOVE_DOWN_HOIST                5
#define STEP_MOVE_TP_CHECK_MOVE_DOWN_HOIST          6
#define STEP_MOVE_TP_COMPLETE                       7

#else

#define STEP_MOVE_TP_READY                          0
#define STEP_MOVE_TP_MOVE_SHIFTROTATE               1
#define STEP_MOVE_TP_CHECK_MOVE_SHIFTROTATE         2
#define STEP_MOVE_TP_MOVE_DOWN_HOIST                3
#define STEP_MOVE_TP_CHECK_MOVE_DOWN_HOIST          4
#define STEP_MOVE_TP_COMPLETE                       5

#endif

//---------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Set Hoist Origin 작업 STEP 관련 Define
// --------------------------------------------------------------------------
#define STEP_SET_H_CHECK_HOIST_HOME                 0
#define STEP_SET_H_MOVE_DOWN_UNTIL_HOIST_HOME_OFF   1
#define STEP_SET_H_CHECK_HOIST_HOME_OFF				2
#define STEP_SET_H_MOVE_UP_TO_NEW_HOME              3
#define STEP_SET_H_MOVE_UP_BY_xMM                   4
#define STEP_SET_H_MOVE_UP_BY_xpMM					5
#define STEP_SET_H_SET_HOIST_ORIGIN                 6
#define STEP_SET_H_SET_HOIST_ORIGIN_COMPLETE        7
//---------------------------------------------------------------------------


// --------------------------------------------------------------------------
// Check_ShiftRotate Step 관련 Define
// --------------------------------------------------------------------------
#define STEP_CHECK_SR_ARRIVE_HOME                   1
#define STEP_CHECK_SR_MOVING                        2
#define STEP_CHECK_SR_INCREASE_TRANS_STEP           3

// --------------------------------------------------------------------------
// MOVE_ALL_HOME 작업 STEP 관련 Define
// --------------------------------------------------------------------------
#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))&& (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))

#define STEP_MOVE_HOME_READY                        0
#define STEP_MOVE_HOME_HAND_ORIGIN                  1
#define STEP_MOVE_HOME_SHUTTER_ORIGIN               2
#define STEP_MOVE_HOME_HOIST_MOVE                   3
#define STEP_MOVE_HOME_HOIST_CHECK                  4
#define STEP_MOVE_HOME_SHIFT_MOVE                   5
#define STEP_MOVE_HOME_SHIFT_CHECK                  6

/* add EUV POD 확인 된 경우 */
#define STEP_MOVE_HOME_PRE_SHIFT_MOVE				7
#define STEP_MOVE_HOME_PRE_SHIFT_CHECK				8

#define STEP_MOVE_HOME_ROTATE_MOVE                  9
#define STEP_MOVE_HOME_ROTATE_CHECK                 10

/* add Shift가 Home 위치가 아닌 경우 */
#define STEP_MOVE_HOME_RETURN_SHIFT_MOVE			11
#define STEP_MOVE_HOME_RETURN_SHIFT_CHECK			12

#define STEP_MOVE_HOME_HAND_SHUTTER_MOVE            13
#define STEP_MOVE_HOME_COMPLETE                     14

#else

#define STEP_MOVE_HOME_READY                        0
#define STEP_MOVE_HOME_HAND_ORIGIN                  1
#define STEP_MOVE_HOME_SHUTTER_ORIGIN               2
#define STEP_MOVE_HOME_HOIST_MOVE                   3
#define STEP_MOVE_HOME_HOIST_CHECK                  4
#define STEP_MOVE_HOME_SHIFT_MOVE                   5
#define STEP_MOVE_HOME_SHIFT_CHECK                  6
#define STEP_MOVE_HOME_ROTATE_MOVE                  7
#define STEP_MOVE_HOME_ROTATE_CHECK                 8
#define STEP_MOVE_HOME_HAND_SHUTTER_MOVE            9
#define STEP_MOVE_HOME_COMPLETE                    10

#endif

// --------------------------------------------------------------------------
// 펌웨어 업데이트 후 오토모드 전환과정 define
// --------------------------------------------------------------------------
#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))

#define STEP_AUTOCHANGE_READY					0
#define STEP_AUTOCHANGE_HAND_ORIGIN				1
#define STEP_AUTOCHANGE_SHUTTER_ORIGIN			2
#define STEP_AUTOCHANGE_HOIST_MOVE				3
#define STEP_AUTOCHANGE_HOIST_CHECK				4
#define STEP_AUTOCHANGE_SHIFT_MOVE				5
#define STEP_AUTOCHANGE_SHIFT_CHECK				6

#define STEP_AUTOCHANGE_PRE_SHIFT_MOVE				7
#define STEP_AUTOCHANGE_PRE_SHIFT_CHECK			8

#define STEP_AUTOCHANGE_ROTATE_MOVE				9
#define STEP_AUTOCHANGE_ROTATE_CHECK			10

#define STEP_AUTOCHANGE_RETURN_SHIFT_MOVE		11
#define STEP_AUTOCHANGE_RETURN_SHIFT_CHECK		12

#define STEP_AUTOCHANGE_HAND_SHUTTER_MOVE		13
#define STEP_AUTOCHANGE_B4CHECK_MARK            14
#define STEP_AUTOCHANGE_MARK_SELECT				15
#define STEP_AUTOCHANGE_NODE_MARK_RUN           16
#define STEP_AUTOCHANGE_NODE_MARK_CHECK         17
#define STEP_AUTOCHANGE_STATION_MARK_RUN		18
#define STEP_AUTOCHANGE_STATION_MARK_CHECK		19
#define STEP_AUTOCHANGE_QR_LEFT_MARK_RUN        20
#define STEP_AUTOCHANGE_QR_RIGHT_MARK_RUN       21

#define STEP_AUTOCHANGE_COMPLETE				22

#else

//#define STEP_AUTOCHANGE_PRE_RESET				0
#define STEP_AUTOCHANGE_READY					0
#define STEP_AUTOCHANGE_HAND_ORIGIN				1
#define STEP_AUTOCHANGE_SHUTTER_ORIGIN			2
#define STEP_AUTOCHANGE_HOIST_MOVE				3
#define STEP_AUTOCHANGE_HOIST_CHECK				4
#define STEP_AUTOCHANGE_SHIFT_MOVE				5
#define STEP_AUTOCHANGE_SHIFT_CHECK				6

#define STEP_AUTOCHANGE_ROTATE_MOVE				7
#define STEP_AUTOCHANGE_ROTATE_CHECK			8
#define STEP_AUTOCHANGE_HAND_SHUTTER_MOVE		9
#define STEP_AUTOCHANGE_B4CHECK_MARK            10

#define STEP_AUTOCHANGE_MARK_SELECT				11

#define STEP_AUTOCHANGE_NODE_MARK_RUN           15
#define STEP_AUTOCHANGE_NODE_MARK_CHECK         16

#define STEP_AUTOCHANGE_STATION_MARK_RUN		20
#define STEP_AUTOCHANGE_STATION_MARK_CHECK		21

#define STEP_AUTOCHANGE_QR_LEFT_MARK_RUN        23
#define STEP_AUTOCHANGE_QR_RIGHT_MARK_RUN       24

#define STEP_AUTOCHANGE_COMPLETE				25

#endif

/*
#define STEP_AUTOCHANGE_FINAL_RESET				16
#define STEP_AUTOCHANGE_AUTO_MODE				17
#define STEP_AUTOCHANGE_COMPLETE				18
*/
#define STEP_AUTOCHANGE_RETRY_MAX			3
#define Offset_Data_CNT						5
struct Hoist_Teaching_Point_Load
{
	double Hoist_Teaching_Offset[Offset_Data_CNT];
	double Final_Hoist_Teaching_Offset;
	double Original_Hoist_Teaching_Point;
	int Hoist_Teaching_Offset_Count;
};

struct Hoist_Teaching_Point_Unload
{
	double Hoist_Teaching_Offset[Offset_Data_CNT];
	double Final_Hoist_Teaching_Offset;
	double Original_Hoist_Teaching_Point;
	int Hoist_Teaching_Offset_Count;
};

#define HOIST_HOME_INPOS_LIMIT	2.0 // home을 sensor에서 10mm 위로 잡은 경우 6mm 쳐짐 발생시 10-6 = 4mm 오차 발생 margin 으로 7mm


/**
@class   TransControl
@date    2012.11.07
@author  LSJ
@brief   TransControl 관련 Class
*/
class TransControl:public VirtualTransControl
{
private:
	LogUnit *pLogUnit;
	LogUnit *pLogAbnormal;
	LogUnit *pLogMCC;

	FDCUnit *pFDCUnit;

private:
    HWSet *m_pHWSet;
    EQPIO *m_pEQPIO;
	DEFAULT_PARAM_SET *m_defualtparam;

	AutoTeaching	*m_AutoTeaching;    //auto Teacing puting 20170101
	DiagManager*	m_DiagManager;
	OHTMainStatus *m_pOHTMainStatus;

	EXECUTE_INFO *m_pExecuteInfo;

    VirtualDrivingControl	*m_pVirtualDrivingControl;

	HWCommon *m_pHWCommon;

	MCC_TRANS_PARAM_SET	m_mccparam;
	/*
	bool m_bUse_EQPIO;
	bool m_bUse_GripperSen;
	bool m_bUse_OscillationSen;
	bool m_bUse_HandPushSen;
	bool m_bUse_LookdownSen;
	bool m_bUse_ShutterControl;
	bool m_bUse_HandSensor;
	bool m_bUse_HoistAlign;
	bool m_bUse_STBSen;
	bool m_bUse_UTBSen;
	bool m_bUse_UTBFoupCheck; //UTB Unload시에 티칭Point까지 이동해서 Foup을 체크할지 ...
	bool m_bUse_IgnorePIOError;
	bool m_bUse_TransSound;
	bool m_bUse_ObsDetectSound;
	*/
	int m_nTransSound;

    int m_nTransStep;
    int m_nTransType;
    int m_nEqType;
    int m_nSTBDirection; //0:EQ,STK 1:RIGHT 2:LEFT
	int m_nPIOTimeoverLevel;

	int m_nTransHoistOriginCount;
	int m_nTransStepHoistRecovery;
	int m_SetHomenCount;

    DWORD  m_dwLREQ_STTime; // Start Time
    DWORD m_dwLookdn_STTime;
    DWORD  m_dwTotalTrans_STTime; //총 trans time
	DWORD m_dwSTBData_Time;  //som
    DWORD m_dwSTBTrigger_Time;  //som

	double m_dHoist_TeachingPos; //티칭 값
	double m_dHoist_TeachingPos_Original; //HJH : Teaching Auto Adjustment
	double m_dShift_TeachingPos;
    double m_dRotate_TeachingPos;
    int m_nRotate_Pos;

    double m_dCmdHoistPos;          ///< MoveHoist할 때 명령을 준 값
    double m_dCmdHoistStartPos;     ///< Hoist가 움직이기 시작한 Point
    double m_dCmdHoistSpeed;
    double m_dCmdFinal_HoistPos;    ///< Down된 후 최종 Hoist Position(티칭값 아님)
    double m_dCmdShiftPos;
    double m_dCmdRotatePos;
    double m_dHoistSlowDownPos;  

    //속도 변경관련
    double m_dHoistLevel;
    double m_dShiftLevel;

    /*******************************************************/
	//HJH : Teaching Auto Adjustment
	Hoist_Teaching_Point_Load 	m_Hoist_TeachingData_Load[1000000];
	Hoist_Teaching_Point_Unload m_Hoist_TeachingData_Unload[1000000];
	/*******************************************************/
	// Teaching 상태 확인 코드 (임시 Test)
	bool m_bHoistCheck;
	int  m_StationID;
	int m_MoniStationID;
	/*******************************************************/

    int  m_ParamChehckError;
	bool m_bTransHoistRecovery;        //E635 등 에러 리밋관련된 에러 발생시 리커버리 처리할 플래그
	bool m_bTransHoistOriginRecovery;  // Hoist Home 센서가 안들어올 경우 리커버리 처리 해 줄것.

	//puting Hoist SmallAdd 적용
	int Move_Hoist_SmallAdd(double dAcc_mmpermsec2, double dDec_mmpermsec2, double dV_mmpermsec, double dCmdHoistPos, int dTriggerIONum, double dSmallDist, bool bIsTriggerOnRisingEdge=false);   // move hoist by small add

	UINT m_nPIOType;     //LimJY_20160122 : For IR+RF PIO (0:IR, 1:RF, 2:Hybrid 5.0GHz)
	UINT m_nPIOID;       //LimJY_20160122 : For IR+RF PIO
	UINT m_nPIOCS;       //LimJY_20160122 : For IR+RF PIO
	UINT m_nPIOChannel;  //LimJY_20160122 : For IR+RF PIO


//   bool m_StatusSet.bIsOnProcess;

    //Lookdown 관련 처리
	bool  bLookdown_Detected;

	bool  bCheck_SpecialEQ;

    int m_nErrCount_GripAbNormalON;
	int m_nErrCount_GripAbNormalOFF;

	int m_nErrCount_HandOpenAbNormalON;
	int m_nErrCount_HandCloseAbNormalON;

    bool m_bIsSTBTypeTrans;
    bool m_bIsSTKTypeTrans;

    bool m_bPIOAutoRecovery;

	bool m_bFDCHandPushFlag;
	bool m_bFDCHandAlarmFlag;
	bool m_bFDCOcsillationFlag;

    DWORD dwMoveSRStartTime;            // Shift, Rotate 구동 시작 시긴 기록 변수
	int nStep_SR_Check;                 // Shift, Rotate 구동 완료 여부 Check Step 변수


	bool m_bOSL_Detected;
	bool m_bOSL_Detected_Check;
	DWORD m_dwOSL_STTime;
	DWORD m_dwOSL_CheckTime;

	bool m_IsCheckPause;

	bool m_bHandPIOGoSignal_Detected;
	DWORD m_dwHandPIOGoSignal_CheckTime;
	int nHandPIOGosignalPauseCount;

	int Check_ESOffTime(int nPeriodTime, int nStep);
	int Check_HOAVBLOffTime(int nPeriodTime, int nStep);
	int Check_ReadyOffTime(int nPeriodTime, int nStep);

	int Check_LREQOnOffTime(int nPeriodTime, bool bCheckStatus, int nStep);
	int Check_UREQOnOffTime(int nPeriodTime, bool bCheckStatus, int nStep);
	int Check_L_U_REQStatus_Down(int nTransType, int nTransStep);
	int Check_L_U_REQStatus_Up(int nTransType, int nTransStep);	

    int Check_EQStatus(int nTransType, int nTransStep);
//    void TransValueSetting();
    int ReadyTrans();
    int Check_State();
    int Move_ShiftRotate(int nMoveCase);
    int Check_ShiftRotate();

	int Move_EUV_Shift();
	int Check_EUV_Shift();
	
	int Move_EUV_Rotate(int nMoveCase);
	int Check_EUV_Rotate();
    int Check_Sensor(bool bLookDnCheck, int nHoistState, int nPreStep, int nNextStep, bool bHoistPauseUse = true);
    void SaveHoistMoveCmd(double dTmpStPos, double dTmpEdPos, double dTmpEdVel);
    int Move_Hoist_HomePosition(double dAccel, double dDecel, double dVel);
    int Move_Hoist_HomeSensorOffPosition(double dAccel, double dDecel, double dVel);

    int Move_Hoist(int nMoveCase);
    int Check_Hoist(int nMotionType);
#ifdef USE_TRANS_PROFILE_VER2
    int Check_Hoist_Ver2(int nMotionType);
	int Move_Hoist_Ver2(int nMoveCase);
	DWORD dwHoistHomeSensorTime;
#endif

#ifdef OHT_HIGH_RAIL_CHECK_HOME
	int Cmd_SmalladdHoistHome();
#endif
	
    int CheckNMoveTeachingPoint();
    int MoveHoist_forTeachingPoint(int nRetryCount, double dVel_mmpermsec, double dCurPos);
    int Check_BeforeReleaseFoup();
	int Check_Complete();
	int AutoRecovery_Trans(int Error_AV, int nStep, int RecoveryType =0);
	void CheckCarrierInterlock(void);

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85))
	int rotateRetry();
#endif


 	int ReadyAllHome();     // 2017-01-01, Add by Labeler

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
    // EUV POD는 회전 동작(Origin, Move) 시 간섭으로 Shift 10mm 안쪽 이동 후 동작이 필요.
    int m_iCmdEuvRotateOrg;     // EUV POD Rotation Origin Command
    int m_iStepEuvRotateOrg;    // EUV POD Rotation Origin Run Step

    int m_iCmdEuvRotateMov;     // EUV POD Rotation Move Command
    int m_iStepEuvRotateMov;    // EUV POD Rotation Move Run Step
    int m_iTargetPosEuvRotateMov;
#endif

//Auto Teaching 관련 함수
	int ReadyTrans_AutoTeaching();
	int Check_State_AutoTeaching();
	int Check_Sensor_AutoTeaching(bool bLookDnCheck, int nHoistState, int nPreStep, int nNextStep);

    int Move_ShiftRotate_AutoTeaching(int nMoveCase);
	int Check_ShiftRotate_AutoTeaching(int nMoveCase,bool &bNexstep);
	int Move_Hoist_AutoTeaching(int nMoveCase);
	int Check_Hoist_AutoTeaching(int nMotionType, bool &bNexstep);
	int CheckNMoveTeachingPoint_AutoTeaching();
	int Check_Complete_AutoTeacing();
	void LimitRetryCheck(int nPortType, double dCurrentPosition, double dOffset);

	double m_HoistSmallAddPistance;

	//동기화 처리 관련 함수
	void TransCMDExuteFunc(bool bFlag);

	void Cal_HoistOffset_Load(double dCurrentHoistPosition, double dCmdHoistPosition, double dCmdHoistPositionOriginal, int nStationNumber);
	void Cal_HoistOffset_Unload(double dCurrentHoistPosition, double dCmdHoistPosition, double dCmdHoistPositionOriginal, int nStationNumber);


protected:
    virtual EXECUTE_ACK Stop(void);
	virtual EXECUTE_ACK Reset(void);
	virtual	EXECUTE_ACK	TurnOff(void);	///< 기동 Off
	virtual	EXECUTE_ACK	TurnON(void);	///< 기동 ON

public:
    TransControl();
    ~TransControl();

    int TransInit();
    int Cmd_Load();
    int Cmd_Unload();
	int Cmd_MoveTP();

	int Cmd_AutoSetHome(int &nTransStep,bool bNoCheckFlag =false);

	int Cmd_AutoTeaching();  //puting 20161218

    int Cmd_MoveAllHome();  // 2017-01-01, Add by Labeler

	int Cmd_AutoMode_Change();	// shkim. added

	int Cmd_AutoTeaching_NullType();  //puting 20180130

#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
    // EUV POD는 회전 동작(Origin, Move) 시 간섭으로 Shift 10mm 안쪽 이동 후 동작이 필요.
    void Set_CmdEuvRotateOrigin(int iCmd);
    void Set_CmdEuvRotateOrigin();
    bool IsRun_EuvRotateOrigin();
    int Seq_EuvRotate_Origin();

    void Set_CmdEuvRotateMove(int iCmd, int nTargetPos);
    void Set_CmdEuvRotateMove(int nTargetPos);
    bool IsRun_CmdEuvRotateMoveState();
    int Seq_EuvRotate_Move();
#endif

	bool bAutoModeSkip;

	void MarkTry();
    
    EXECUTE_ACK InsertGeneralCmd(void *pTransInfo); ///<논의 필요:EXECUTE_TRANS_INFO
    EXECUTE_ACK SetGeneralCmd(EXECUTE_TRANS_INFO sTransInfo);
    UINT        ExecuteJob(void);
    UINT        CheckAbnormalOnWait(void);      ///< 대기 시 이상 현상 확인 함수 (Step 형태로 구현 - Thread가 돌림)
    UINT        CheckAlways(void);      ///< 항시 이상 현상 확인 함수 (Step 형태로 구현 - Thread가 돌림)
    bool        DeleteCmd(void *pCmd);                      ///< 할당 된 명령을 소멸함
    void*       LoadParam(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory);
	bool        LoadParam_New(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory);
    bool        RequestParamUpdate(void* pParameter);
    bool        IsParamUpdated();
    

	//2 파라미터 관련 함수
	// by zzang9un 2013.08.18 : Virtual 함수 구현 부분
    int ChangeUse(int UseIndex, bool YesOrNo);
    
    TRANSE_DIFFERENCE getDifference();  ///< Rail 부터 Jig 바닥면까지의 Offset
	bool    loadParameter(char* szFilePath, TRANS_PARAM_SET* pParamSet);    ///< 파라미터 불러오는 함수
	bool    loadParameterRecovery(char* szFilePath, TRANS_PARAM_SET* pParamSet);    ///< 파라미터 불러오는 함수
private:
	//2 파라미터 관련 함수
	//bool    LoadParameter(char* szFilePath, TRANS_PARAM_SET* pParamSet);    ///< 파라미터 불러오는 함수
	bool    saveParameter(char* szFilePath, TRANS_PARAM_SET* pParamSet);    ///< 파라미터 저장하는 함수
	bool    updateParameter();                                          ///< 파라미터 업데이트 하는 함수

	bool 	Load_ReticleInTeaching(UINT CurStationID,RETICLE_TEACH_OFFSET* pRiticleTeachingOffset);
	bool    Load_SpecialEQParameter(UINT CurStationID,TRANS_PARAM_SET* pParamSet);

public:
    TRANS_PARAM_SET *m_pParamSet;
    TRANS_PARAM_SET *m_pTmpParamSet;
    
    TRANS_USE_PART  m_TransUse; ///< Trans 동작 시 사용되는 Use 변수

	void 	Reset_TransControl();
	bool    m_bSlipringFlag;
	int 	HoistCnt;
	UINT	GetCycleCount(void)		 {return HoistCnt;}
	bool 	bHoistJig;
    int     nHoistTeaching;
};
//---------------------------------------------------------------------------
#endif
