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

#if(OHT_NAME == OHT_NAME_STD_V80)
#define HOIST_MOVELIMIT_P           4000.0        // mm
#define HOIST_MOVELIMIT_N           -1.5    	  // mm
#define SHIFT_MOVELIMIT_P           478.5 	//483.0     	// SW Limit은 P1L 호환성으로 인해 (O-EQ) 478.5 / (EQ) -444mm (기구 Limit(O-EQ) 486mm/ (EQ) -447mm)
#define SHIFT_MOVELIMIT_N           -444.0	//-441.0         //
#define SHIFT_TEACHINGLIMIT_P       478.5
#define SHIFT_TEACHINGLIMIT_N      -444.0
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
#define HOIST_MOVELIMIT_P           4000.0        // mm   3200
#define HOIST_MOVELIMIT_N           -1.5          // mm
#define SHIFT_MOVELIMIT_P           20.0          // mm   -18(1호기) //450
#define SHIFT_MOVELIMIT_N           -20.0         // mm   +18(1호기)
#define SHIFT_TEACHINGLIMIT_P       SHIFT_MOVELIMIT_P
#define SHIFT_TEACHINGLIMIT_N       SHIFT_MOVELIMIT_N
#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
#define HOIST_MOVELIMIT_P           6000          // 기존 레티클이 4000 이었으나 이를 5500으로 늘림
#define HOIST_MOVELIMIT_N           -1.5          // mm
#define SHIFT_MOVELIMIT_P           25.0          // mm   -18(1호기) //450
#define SHIFT_MOVELIMIT_N           -25.0         // mm   +18(1호기)
#define SHIFT_TEACHINGLIMIT_P       20.0
#define SHIFT_TEACHINGLIMIT_N      -20.0
#elif((OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85S))
#define HOIST_MOVELIMIT_P           4000.0        // mm   3200    //
#define HOIST_MOVELIMIT_N           -1.5    	  // mm
#define SHIFT_MOVELIMIT_P           478.5 	//483.0     	// SW Limit은 P1L 호환성으로 인해 (O-EQ) 478.5 / (EQ) -444mm (기구 Limit(O-EQ) 486mm/ (EQ) -447mm)
#define SHIFT_MOVELIMIT_N           -444.0	//-441.0        //
#define SHIFT_TEACHINGLIMIT_P       SHIFT_MOVELIMIT_P
#define SHIFT_TEACHINGLIMIT_N       SHIFT_MOVELIMIT_N
#elif(OHT_NAME == OHT_NAME_STD_V85)
#define HOIST_MOVELIMIT_P           6000.0        // mm   3200
#define HOIST_MOVELIMIT_N           -7.0         // mm
#define SHIFT_MOVELIMIT_P           405.0          // mm   -18(1ȣ?⩠//450
#define SHIFT_MOVELIMIT_N           -405.0         // mm   +18(1ȣ?⩍
#define SHIFT_TEACHINGLIMIT_P       200.0
#define SHIFT_TEACHINGLIMIT_N      -200.0
#elif(OHT_NAME == OHT_NAME_STD_V86)
#define HOIST_MOVELIMIT_P           6000.0        // mm   3200
#define HOIST_MOVELIMIT_N           -7.0         // mm
#define SHIFT_MOVELIMIT_P           271.0// UTB 티칭 길이 때문에 SLIDE HW 변경으로 SW Limit 변경 310.0 -> 271.0
#define SHIFT_MOVELIMIT_N           -351.0// UTB 티칭 길이 때문에 SLIDE HW 변경으로 SW Limit 변경 -310.0 -> -351.0
#define SHIFT_TEACHINGLIMIT_P       300.0          // mm   8.5 대비 스토퍼 간격 늘림
#define SHIFT_TEACHINGLIMIT_N      -300.0          // mm   8.5 대비 스토퍼 간격 늘림
#elif(OHT_NAME == OHT_NAME_STD_V82)
#define HOIST_MOVELIMIT_P           4000.0        // mm   3200    //
#define HOIST_MOVELIMIT_N           -7.0    	  // mm        // Limit 범위 복구(Hoist Motor Error 발생, SmallAdd 명령에 따른 센서 가변 Error 추가)
#define SHIFT_MOVELIMIT_P           478.5 	//483.0     	// SW Limit은 P1L 호환성으로 인해 (O-EQ) 478.5 / (EQ) -444mm (기구 Limit(O-EQ) 486mm/ (EQ) -447mm)
#if(USE_MAC == 1)
#define SHIFT_MOVELIMIT_N 			-36.0     //MAC Slide 기구 Limit 486mm / -40mm
#else
#define SHIFT_MOVELIMIT_N           -444.0	//-441.0        //
#endif
#define SHIFT_TEACHINGLIMIT_P       SHIFT_MOVELIMIT_P
#define SHIFT_TEACHINGLIMIT_N       SHIFT_MOVELIMIT_N
#else
#define HOIST_MOVELIMIT_P           4000.0        // mm   3200    //
#define HOIST_MOVELIMIT_N           -1.5    	  // mm
#define SHIFT_MOVELIMIT_P           481.0 	//455.0     	// 추후 변경 필요.S/W Limit (O-EQ) 478.5mm / (EQ) 444mm (기구 Limit(O-EQ) 481.5mm / (EQ) 447mm)
#define SHIFT_MOVELIMIT_N           -445.0	//-465.0        //
#define SHIFT_TEACHINGLIMIT_P       SHIFT_MOVELIMIT_P
#define SHIFT_TEACHINGLIMIT_N       SHIFT_MOVELIMIT_N
#endif

#define HOIST_MOVELIMIT_N_SMALLADD           9.0    	  // mm        // SmallAdd 명령에 따른 센서 가변 Error 추가 11mm-> 9mm


#if(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
#define SHIFT_IDEALGEARRATIO            4096.0000000
#elif(OHT_NAME == OHT_NAME_STD_V82)
#define SHIFT_IDEALGEARRATIO             546.1333333
#elif (OHT_NAME == OHT_NAME_STD_V86)
#define SHIFT_IDEALGEARRATIO             819.19999995 // 기어비  3단 -> 2단 변경으로 546.1333333 * 1.5 soming
#elif (OHT_NAME == OHT_NAME_P4_MASK)
#define SHIFT_IDEALGEARRATIO             10000.0000000
#else
#define SHIFT_IDEALGEARRATIO             682.6666667
#endif
#define SHIFT_IDEALGEARRATIO_MIN         0.1

#define SHIFT_IDEAL2REALRATIO            1.0
#define SHIFT_IDEAL2REALRATIO_MIN        0.8

#define HOIST_MOVELIMIT_P_MAX_STB              1000//150.0
#define HOIST_MOVELIMIT_N_MIN_STB              80//95.0

#if(USE_MAC == 1)
#define SHIFT_MOVELIMIT_N_MIN_STB_R            445.0    //MAC 실사용 Stroke 455mm
#else
#define SHIFT_MOVELIMIT_N_MIN_STB_R            460.0
#endif

#define SHIFT_MOVELIMIT_N_MIN_STB_L            -410.0


#define SHIFT_MOVELIMIT_P_TEMP     481.0
#if(USE_MAC == 1)
#define SHIFT_MOVELIMIT_N_TEMP	   -36.0
#else
#define SHIFT_MOVELIMIT_N_TEMP    -445.0
#endif


// --------------------------------------------------------------------------
// LOAD 작업 STEP 관련 Define
// --------------------------------------------------------------------------

#if(OHT_NAME == OHT_NAME_P4_MASK)
#define STEP_L_EQPIO_CONNECT                		0
#define STEP_L_LOAD_READY                   		1
#define STEP_L_EQPIO_READY                  		2
#define STEP_L_MOVE_HOIST_HOME              		3
#define STEP_L_CHECK_MOVE_HOIST_HOME				4
#define STEP_L_SHUTTER_OPEN                 		5
#define STEP_L_MOVE_SHIFTROTATE             		6
#define STEP_L_CHECK_MOVE_SHIFTROTATE       		7
#define STEP_L_MOVE_DOWN_HOIST_FAST         		8
#define STEP_L_CHECK_MOVE_DOWN_HOIST_FAST   		9
#define STEP_L_MOVE_DOWN_HOIST_SLOW         		10       // TO2TL, Roller Entrance > load point
#define STEP_L_CHECK_MOVE_DOWN_HOIST_SLOW   		11
#define STEP_L_CHECK_ARRIVE_HAND_LOAD_POINT 		12
#define STEP_L_MOVE_HANDRMR_WORK            		13
#define STEP_L_MOVE_DOWN_HOIST_SLOW_2       		14      // TL2TP, Load Point > TP
#define STEP_L_CHECK_MOVE_DOWN_HOIST_SLOW_2 		15
#define STEP_L_CHECK_ARRIVE_TEACHING_POINT  		16
#define STEP_L_CHECK_BEFORE_RELEASE_FOUP    		17
#define STEP_L_RELEASE_FOUP                 		18
#define STEP_L_MOVE_UP_HOIST_SLOW           		19
#define STEP_L_CHECK_MOVE_UP_HOIST_SLOW     		20
#define STEP_L_MOVE_UP_HOIST_FAST           		21
#define STEP_L_CHECK_MOVE_UP_HOIST_FAST     		22
#define STEP_L_MOVE_UP_HOIST_HOME           		23
#define STEP_L_CHECK_MOVE_UP_HOIST_HOME     		24
#define STEP_L_MOVE_SHIFTROTATE_HOME        		25
#define STEP_L_CHECK_MOVE_SHIFTROTATE_HOME  		26
#define STEP_L_PIO_COMPLETE                 		27
#define STEP_L_CHECK_ANTIDROP               		28
#define STEP_L_MOVE_HOIST_ANTIDROP          		29
#define STEP_L_CHECK_MOVE_HOIST_ANTIDROP    		30
#define STEP_L_LOAD_COMPLETE                		31
#else
//AOHC-288
#define STEP_L_EQPIO_CONNECT                		0
#define STEP_L_LOAD_READY                   		1
#define STEP_L_EQPIO_READY                  		2
#define STEP_L_MOVE_PRE_SHIFT_BEFORE_DOWN			3
#define STEP_L_CHECK_PRE_SHIFT_BEFORE_DOWN			4
#define STEP_L_MOVE_PRE_ROTATE_BEFORE_DOWN			5
#define STEP_L_CHECK_PRE_ROTATE_BEFORE_DOWN			6
#define STEP_L_MOVE_SHIFTROTATE             		7
#define STEP_L_CHECK_MOVE_SHIFTROTATE       		8
#define STEP_L_MOVE_DOWN_HOIST_FAST         		9
#define STEP_L_CHECK_MOVE_DOWN_HOIST_FAST   		10
#define STEP_L_MOVE_DOWN_HOIST_FAST_SLOWDOWN		11
#define STEP_L_CHECK_MOVE_DOWN_HOIST_FAST_SLOWDOWN	12
#define STEP_L_MOVE_DOWN_HOIST_SLOW         		13
#define STEP_L_CHECK_MOVE_DOWN_HOIST_SLOW   		14
#define STEP_L_CHECK_ARRIVE_TEACHING_POINT  		15
#define STEP_L_CHECK_BEFORE_RELEASE_FOUP    		16
#define STEP_L_RELEASE_FOUP                 		17
#define STEP_L_MOVE_UP_HOIST_SLOW           		18
#define STEP_L_CHECK_MOVE_UP_HOIST_SLOW     		19
#define STEP_L_MOVE_UP_HOIST_FAST           		20
#define STEP_L_CHECK_MOVE_UP_HOIST_FAST     		21
#define STEP_L_MOVE_UP_HOIST_FAST_SLOWDOWN          22
#define STEP_L_CHECK_MOVE_UP_HOIST_FAST_SLOWDOWN    23
#define STEP_L_MOVE_UP_HOIST_HOME           		24
#define STEP_L_CHECK_MOVE_UP_HOIST_HOME     		25
#define STEP_L_MOVE_SHIFTROTATE_HOME       		 	26
#define STEP_L_CHECK_MOVE_SHIFTROTATE_HOME  		27
#define STEP_L_PIO_COMPLETE                 		28
#define STEP_L_LOAD_COMPLETE                		29
#endif
//---------------------------------------------------------------------------

// --------------------------------------------------------------------------
// UNLOAD 작업 STEP 관련 Define
// --------------------------------------------------------------------------
#if(OHT_NAME == OHT_NAME_P4_MASK)
#define STEP_U_HOISTSENSOR_ORIGIN_RECOVERY				-1
#define STEP_U_EQPIO_CONNECT                			0
#define STEP_U_UNLOAD_READY                 			1
#define STEP_U_EQPIO_READY                 				2
#define STEP_U_MOVE_HOIST_HOME              			3
#define STEP_U_CHECK_MOVE_HOIST_HOME					4
#define STEP_U_SHUTTER_OPEN                 			5
#define STEP_U_MOVE_SHIFTROTATE             			6
#define STEP_U_CHECK_MOVE_SHIFTROTATE       			7
#define STEP_U_MOVE_DOWN_HOIST_FAST         			8
#define STEP_U_CHECK_MOVE_DOWN_HOIST_FAST   			9
#define STEP_U_MOVE_DOWN_HOIST_SLOW         			10
#define STEP_U_CHECK_MOVE_DOWN_HOIST_SLOW   			11
#define STEP_U_CHECK_ARRIVE_TEACHING_POINT  			12
#define STEP_U_GRIP_FOUP                    			13
#define STEP_U_MOVE_UP_HOIST_SLOW           			14
#define STEP_U_CHECK_MOVE_UP_HOIST_SLOW     			15
#define STEP_U_CHECK_ARRIVE_HAND_LOAD_POINT 			16
#define STEP_U_MOVE_HANDRMR_HOME            			17
#define STEP_U_MOVE_UP_HOIST_SLOW_2         			18
#define STEP_U_CHECK_MOVE_UP_HOIST_SLOW_2   			19
#define STEP_U_MOVE_UP_HOIST_FAST           			20
#define STEP_U_CHECK_MOVE_UP_HOIST_FAST     			21
#define STEP_U_MOVE_UP_HOIST_HOME           			22
#define STEP_U_CHECK_MOVE_UP_HOIST_HOME     			23
#define STEP_U_MOVE_SHIFTROTATE_HOME        			24
#define STEP_U_CHECK_MOVE_SHIFTROTATE_HOME  			25
#define STEP_U_PIO_COMPLETE                 			26
#define STEP_U_CHECK_ANTIDROP               			27
#define STEP_U_MOVE_HOIST_ANTIDROP          			28
#define STEP_U_CHECK_MOVE_HOIST_ANTIDROP    			29
#define STEP_U_UNLOAD_COMPLETE              			30
#else
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
#define STEP_U_MOVE_PRE_ROTATE_BEFORE_HOME  21
#define STEP_U_CHECK_PRE_ROTATE_BEFORE_HOME  22
#define STEP_U_MOVE_SHIFTROTATE_HOME        23
#define STEP_U_CHECK_MOVE_SHIFTROTATE_HOME  24
#define STEP_U_PIO_COMPLETE                 25
#define STEP_U_CHECK_ANTIDROP               26
#define STEP_U_UNLOAD_COMPLETE              27
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
#define STEP_ROTATE_ORIGIN_COMPLETE     7


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
#define STEP_EUV_ROT_SHIFT_MOVE_COMPLETE 7


// --------------------------------------------------------------------------
// Move Teaching Point 작업 STEP 관련 Define
// --------------------------------------------------------------------------
#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))

#define STEP_MOVE_TP_READY                          0
/* add */
#define STEP_MOVE_TP_PRE_SHIFTROTATE				1
#define STEP_MOVE_TP_CHECK_SHIFTROTATE              2
#define STEP_MOVE_TP_MOVE_SHIFTROTATE               3
#define STEP_MOVE_TP_CHECK_MOVE_SHIFTROTATE         4
#define STEP_MOVE_TP_MOVE_DOWN_HOIST                5
#define STEP_MOVE_TP_CHECK_MOVE_DOWN_HOIST          6
#define STEP_MOVE_TP_COMPLETE                       7

#elif(OHT_NAME==OHT_NAME_P4_MASK)	//mgchoi. 2022.08.22. P4 MaskOHT 용
#define STEP_MOVE_TP_READY                          0
#define STEP_MOVE_TP_MOVE_SHIFTROTATE               1
#define STEP_MOVE_TP_CHECK_MOVE_SHIFTROTATE         2
#define STEP_MOVE_TP_MOVE_DOWN_HOIST                3
#define STEP_MOVE_TP_CHECK_MOVE_DOWN_HOIST          4
#define STEP_MOVE_TP_MOVE_DOWN_HOIST_SLOW           5
#define STEP_MOVE_TP_CHECK_MOVE_DOWN_HOIST_SLOW     6
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
#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))&& (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))

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
#if(OHT_NAME==OHT_NAME_P4_MASK)
//for P4 Mask. mgchoi
#define STEP_MOVE_HOME_HOIST_MOVE_ANTIDROP2HOME		   11
#define STEP_MOVE_HOME_HOIST_CHECK_ANTIDROP2HOME	   12
#define STEP_MOVE_HOME_HOIST_MOVE_HOME2ANTIDROP		   13
#define STEP_MOVE_HOME_HOIST_CHECK_HOME2ANTIDROP	   14
#endif

#endif

// --------------------------------------------------------------------------
// 펌웨어 업데이트 후 오토모드 전환과정 define
// --------------------------------------------------------------------------
#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))

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

#define STEP_AUTOTEACHING_STB_SHIT_MOVE_10MM                  	0
#define STEP_AUTOTEACHING_STB_MEASURE                  			1
#define STEP_AUTOTEACHING_STB_MOVE_DRIVING            			2
#define STEP_AUTOTEACHING_STB_READY		            			3
#define STEP_AUTOTEACHING_STB_MOVE_SHIFT            			4
#define STEP_AUTOTEACHING_STB_CHECK_MOVE_SHIFT           		5
#define STEP_AUTOTEACHING_STB_MOVE_DOWN_HOIST          			6
#define STEP_AUTOTEACHING_STB_CHECK_MOVE_DOWN_HOIST    			7
#define STEP_AUTOTEACHING_STB_CHECK_PUSH_HAND		           	8
#define STEP_AUTOTEACHING_STB_TEACHING_SAVE            			9
#define STEP_AUTOTEACHING_STB_MOVE_UP_HOIST_HOME       		    10
#define STEP_AUTOTEACHING_STB_CHECK_MOVE_UP_HOIST_HOME     		11
#define STEP_AUTOTEACHING_STB_MOVE_SHIFT_HOME            		12
#define STEP_AUTOTEACHING_STB_CHECK_MOVE_SHIFT_HOME           	13
#define STEP_AUTOTEACHING_STB_COMPLETE                          14


// --------------------------------------------------------------------------
// Materail Type Check Seq
// --------------------------------------------------------------------------
#define STEP_CHECK_STATE_MATERIAL_TYPE                      0
#define STEP_READY_MATERIAL_TYPE							1
#define STEP_MATERIAL_TYPE_HOIST_HOME_MOVE                  2
#define STEP_CHECK_MATERIAL_TYPE_HOIST_HOME_MOVE            3
#define STEP_MATERIAL_TYPE_ROTATE_HOME_MOVE           		4
#define STEP_CHECK_MATERIAL_TYPE_ROTATE_HOME_MOVE     		5
#define STEP_MATERIAL_TYPE_SHIFT_HOME_MOVE           		6
#define STEP_CHECK_MATERIAL_TYPE_SHIFT_HOME_MOVE     		7
#define STEP_CHECK_COMPLETE_MATERIAL_TYPE                   8

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

#if(OHT_NAME == OHT_NAME_STD_V82 || OHT_NAME == OHT_NAME_STD_V86 || OHT_NAME == OHT_NAME_P4_MASK)
#define HOIST_HOME_INPOS_LIMIT	7.0 // home을 sensor에서 10mm 위로 잡은 경우 6mm 쳐짐 발생시 10-6 = 4mm 오차 발생 margin 으로 7mm

#else
#define HOIST_HOME_INPOS_LIMIT	2.0

#endif


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
    LogUnit *pLogMDetail;
	LogUnit *pLogAutoRecovery;
	FDCUnit *pFDCUnit;

private:
    HWSet *m_pHWSet;
    EQPIO *m_pEQPIO;
	DEFAULT_PARAM_SET *m_defualtparam;
	DEFAULT_LINE_PARAM_SET *m_LineParamSet;   //AOHC-285

	TRANS_PROFILEDETAIL m_Transprofiledetail[10]; //AOHC-294

	int m_nDnState_UP_SpeedDown_Offset_SpecialEQ;
	int m_nDnState_UP_SpeedDown_Percent_SpecialEQ;
	
	std::vector<UINT> arrUsingSpecialEQ;
	std::map<UINT, RETICLE_TEACH_OFFSET*> ReticleInTeachingMap;

	AutoTeaching	*m_AutoTeaching;    //auto Teacing puting 20170101
	DiagManager*	m_DiagManager;

	OHTMainStatus *m_pOHTMainStatus;

	EXECUTE_INFO *m_pExecuteInfo;

    VirtualDrivingControl	*m_pVirtualDrivingControl;

	HWCommon *m_pHWCommon;

	MCC_TRANS_PARAM_SET	m_mccparam;

	bool m_bTransSound;
	bool m_bTransEQ_Sound;
	bool m_bTransSTK_Sound;
	bool m_bTransSTB_Sound;

    int m_nTransStep;
    int m_nTransType;
    int m_nEqType;
    int m_nSTBDirection; //0:EQ,STK 1:RIGHT 2:LEFT
	int m_nPIOTimeoverLevel;
	int m_nSpecialEQLevel; //AOHC-294

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
	bool m_bLookdown_Detected_Checked;

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
	bool m_bOSL_Detected_Checked;
	DWORD m_dwOSL_STTime;
	DWORD m_dwOSL_CheckTime;

	bool m_IsCheckPause;

	bool m_bHandPIOGoSignal_Detected;
	DWORD m_dwHandPIOGoSignal_CheckTime;
	DWORD m_dwHandPushON_CheckTime;
	bool m_bHandPushCheck;
	bool m_bEQHandPushCheck;
	int m_nHandPushCountCheck;
	//bool m_bHandPushCountCheck;
	int nHandPIOGosignalPauseCount;
	int m_nHandPIOPauseCnt; //AOHC-288
	int m_nHandPIOARCount;
	bool m_bHandPIOAR;
	bool m_bHandPIOARStart;

	double m_dX;
	double m_dY;
	double m_dZ;

	int m_nVisionRetryCnt;
	bool m_bIsAutoTeachingFail;

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

    //AOHC-285
	void Check_ShiftRotateTransStepAdd();
	void Check_HoistTransStepAdd();
	void Check_Hoist_Ver2TransStepAdd();

	int Move_EUV_Shift();
	int Check_EUV_Shift();
	
	int Move_EUV_Rotate(int nMoveCase);
	int Check_EUV_Rotate();
#if(OHT_NAME != OHT_NAME_P4_MASK)
    int Check_Sensor(bool bLookDnCheck, int nHoistState, int nPreStep, int nNextStep, bool bHoistPauseUse = true);
#else
	int Check_Sensor(bool bLookDnCheck, int nHoistState, int nPreStep, int nNextStep);	//mgchoi. 2022.08.24 P4 MaskOHT
#endif
    void SaveHoistMoveCmd(double dTmpStPos, double dTmpEdPos, double dTmpEdVel);
    int Move_Hoist_HomePosition(double dAccel, double dDecel, double dVel);
    int Move_Hoist_HomeSensorOffPosition(double dAccel, double dDecel, double dVel);

    int Move_Hoist(int nMoveCase);
    int Check_Hoist(int nMotionType);

	int Check_Hoist_Ver2(int nMotionType);
	int Move_Hoist_Ver2(int nMoveCase);
	DWORD dwHoistHomeSensorTime;

	//22.01.03 JHJ MCC 함수 분리
	void CheckHoistHomeMCC();

#ifdef OHT_HIGH_RAIL_CHECK_HOME
	int Cmd_SmalladdHoistHome();
#endif
	
    int CheckNMoveTeachingPoint();
    int MoveHoist_forTeachingPoint(int nRetryCount, double dVel_mmpermsec, double dCurPos);
    int Check_BeforeReleaseFoup();
	int Check_Complete();
	int AutoRecovery_Trans(int Error_AV, int nStep, int RecoveryType = 0);
	void CheckCarrierInterlock(void);

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))
	int rotateRetry();
#endif


 	int ReadyAllHome();     // 2017-01-01, Add by Labeler

#if(((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
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
	int Check_Complete_AutoTeaching();
	void LimitRetryCheck(int nPortType, double dCurrentPosition, double dOffset);
	double m_HoistSmallAddPistance;

	//동기화 처리 관련 함수
	void TransCMDExuteFunc(bool bFlag);

	void Cal_HoistOffset_Load(double dCurrentHoistPosition, double dCmdHoistPosition, double dCmdHoistPositionOriginal, int nStationNumber);
	void Cal_HoistOffset_Unload(double dCurrentHoistPosition, double dCmdHoistPosition, double dCmdHoistPositionOriginal, int nStationNumber);

	bool bCheckTransParam;	//AOHC-358 파라미터 적용 오류 시, 에러 발생

	bool m_nShutterRecovery_Cnt;
	bool m_nShiftHomeRecovery_Cnt;

	bool m_nShiftRotateRecovery_Cnt;
	DWORD m_nShiftRotateRecovery_Time;

	bool m_nHandPushRecovery_Cnt;
	int m_nHoistHomeRetry_Cnt;
	bool m_HandARStatus;
	int m_HandARCount;
	int m_HandPushARCount;
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

#if(OHT_NAME == OHT_NAME_P4_MASK)
	/// mgchoi. 2022-08-18. 마스크 OHT 용 임시 함수 ///
	int CheckNMoveLoadPoint();
	int Check_RMR_WORK();
	int Check_RMR_HOME();
	int MoveHoist_forAntiDrop(int MotionType);
	int Check_Hoist_AntiDrop(int nMotionType, DWORD dwStTime);
	int Cmd_AutoSetHome();
	int Check_Hoist_Home_Sensor();
	void setCarrierType(int Carrier);
	int AutoRecovery_for_Mask(int nStep);
	void CheckCarrierType();
	float getTransSmallAddDistForLH(int type);  // 0: Load/Unload Position, 1: Antidrop
#endif

	int Cmd_AutoSetHome(int &nTransStep,bool bNoCheckFlag =false);
	int Cmd_AutoTeaching();  //puting 20161218
	int Cmd_AutoTeachingVision();
	int Cmd_MoveAllHome();  // 2017-01-01, Add by Labeler
	int Cmd_MoveHoist2CheckMaterialType();
	int Cmd_AutoMode_Change();	// shkim. added
	int Cmd_AutoTeaching_NullType();  //puting 20180130


#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86)) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
    // EUV POD는 회전 동작(Origin, Move) 시 간섭으로 Shift 10mm 안쪽 이동 후 동작이 필요.
    void Set_CmdEuvRotateOrigin(int iCmd);
    void Set_CmdEuvRotateOrigin();
	bool IsRun_EuvRotateOrigin();
	int Get_StepEuvRotateOrg();
	int Seq_EuvRotate_Origin();

	void Set_CmdEuvRotateMove(int iCmd, int nTargetPos);
    void Set_CmdEuvRotateMove(int nTargetPos);
	bool IsRun_CmdEuvRotateMoveState();
	int Get_StepEuvRotateMov();
	int Seq_EuvRotate_Move();
#endif

	bool bAutoModeSkip;

    EXECUTE_ACK InsertGeneralCmd(void *pTransInfo); ///<논의 필요:EXECUTE_TRANS_INFO
//	void MarkTry();                  //미사용

    EXECUTE_ACK SetGeneralCmd(EXECUTE_TRANS_INFO sTransInfo);
    UINT        ExecuteJob(void);
    UINT        CheckAbnormalOnWait(void);      ///< 대기 시 이상 현상 확인 함수 (Step 형태로 구현 - Thread가 돌림)
    UINT        CheckAlways(void);      ///< 항시 이상 현상 확인 함수 (Step 형태로 구현 - Thread가 돌림)
    bool        DeleteCmd(void *pCmd);                      ///< 할당 된 명령을 소멸함
    void*       LoadParam(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory);
	bool        LoadParam_New(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory);
    bool        RequestParamUpdate(void* pParameter);
	bool        IsParamUpdated();

	bool LoadParamTransprofiledetail(char* szPath, TRANS_PROFILEDETAIL  *pParamSet);   //AOHC-294

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
	bool    Load_SpecialEQParameter();
	bool	Set_SpecialEQParameter();

	bool 	Load_ReticleInTeaching();
	bool 	TryGet_ReticleInTeaching(RETICLE_TEACH_OFFSET** pRetTeach);

	double getMaxSpeed(double dAccel, double dDecel, double dDist, double dCurSpeed = 0.0, double dTargetSpeed = 0.0);	///< 가감속 거리 조건에서의 최대 속도를 구한다.

public:
    TRANS_PARAM_SET *m_pParamSet;
    TRANS_PARAM_SET *m_pTmpParamSet;
    
    TRANS_USE_PART  m_TransUse; ///< Trans 동작 시 사용되는 Use 변수

	void 	Reset_TransControl();
	bool    m_bSlipringFlag;
	double m_dHoistLowSpeedDist;
	unsigned char szSSID[32];
	int Calc_STBTeachingData(double, double, double, double*, double*, double*);

	int GetTransStep();
	int GetTransType();
	bool GetAutoTeachingFail();


	bool m_bTransSound_Use;
	static double calcMaxSpeedDynamic(double velIn, const double* dynamicVel, const double* dynamicAcc, const int numDynamicArea, const double decel, const double constTime, double dist);

	#if(OHT_NAME == OHT_NAME_P4_MASK)
	/// mgchoi. 2022-08-18. 마스크 OHT 용 임시 변수 ///
    int m_nCarrierType_Mask;
	int m_nCarrierType_OCS;
	int m_nPIO_AR_Step_For_Mask;
	DWORD m_dwTrans_StepTime;
	///////////////////////////////////////////////////
	#endif


};
//---------------------------------------------------------------------------
#endif
