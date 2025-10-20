/**
@file    Remocon.h
@version OHT 7.0
@brief   Remocon Class Header File
		 추후 적용시 변경해야 함.
*/

#ifndef RemoconH
#define RemoconH

#include "HWCommon.h"
#include "CommHandler.h"
#include "ErrorCode.h"

/*
HandTP 프로토콜 구성
HDR[1] + LEN[1] + DATA[n] + CRC[2]
HDR : 0xA5
LEN : DATA 의 길이
CRC : DATA의 CRC, CRC16

HandTP 프로토콜 구성

[IR]
통신 Type[1] + CMD[1] + Sub CMD[1] + EndFlag[1]
통신 Type : IR or RF
CMD : 주 명령
Sub CMD : 보조 명령
EndFlag : 패킷 마지막 확인.

[RF]
통신 Type[1] + Machin ID[2] + CMD[1] + Sub CMD[1] + Cycle No[1] + Packet No[2] + Data[8] + Checksum[1] + EndFlag[1]
통신 Type : IR or RF
Machin ID : 0~65535
CMD : 주 명령
Sub CMD : 보조 명령
Cycle No : Cycle list 번호
Packet No : 
Data : 데이터 
Checksum : MachinID 부터 Data 를 1byte 더한 값 overflow 버림.
EndFlag : 패킷 마지막 확인.

*/
#define COM1                        1 // 1
#define HANDTP_BAUDRATE             38400 //9600
#define HDR                         -91
#define	CR					        0x0D
#define	LF					        0x0A
#define POLYNOMIAL                  0x8005

#define OHT_TO_LINKER_DATA_LENTH    18
#define HANDY_TP_PACKET_SIZE		16



#define END_FLAG                    (0xA9)
#define EMPTY_BUFFER                (0x00)
#define IR_ESTOP_1st				(0xFF)
#define IR_ESTOP_2nd				(0xAA)
#define IR_RUNSTOP		            (0xBB)
#define IR_2_4G						(0x24)
#define IR_5G						(0x50)

#define IR_VHL_LINK_INFO_REQ		(0x01)
#define IR_VHL_LINK_INFO_ACK		(0x02)
#define IR_CON_VHL_QUERY 			(0xC3)
#define IR_CON_VHL_ACK				(0xC4)


/* Packet 구조
|CMD  | CMD | VHL | VHL |Packet|Data |Check|End  | 
|main | sub | Type| ID  |Number|     |Sum  |Flag |
|1Byte|1Byte|1Byte|2Byte|2Byte |8Byte|1Byte|1Byte|
CRC는 linker만 사용
*/

// CMD main define
#define REMOTE_TYPE_IR              (0xF1)
#define REMOTE_TYPE_RF              (0xF2)

// CMD sub define : RF
#define REQ_OHT_STATUS		0x01
#define CYCLE_CMD			0x02
#define MANUAL_KEY			0x03
#define GLOBAL_KEY			0x04
#define REQ_OHT_IO_INFO     0x05
#define REQ_OHT_STATUS_BG	0x06



// MANUAL_KEY - Key 버튼 값

#define HANDTP_CMD_TYPE_QUICK	0x01
#define HANDTP_CMD_TYPE_NORMAL	0x02
#define HANDTP_CMD_TYPE_EXCEPT	0x03


// VHL Type define
#define OHT_FOUP			0x01
#define OHT_RETICLE			0x02
#define OHT_PATROL			0x03

//HandTP Protocol Error Define
#define ERR_HANDTP_HDR                1
#define ERR_HANDTP_REMOTE_TYPE        -1
#define ERR_HANDTP_READ_LENGTH        -2
#define ERR_HANDTP_CRC16              -3
#define ERR_HANDTP_CHECKSUM           -4
#define ERR_HANDTP_NO_DATA			  -5

// HandTP Status Step
#define HANDTP_STATUS_FIRST	    0x01
#define HANDTP_STATUS_SECOND 	0x02
#define HANDTP_STATUS_THIRD 	0x03

// HandTP IO Info Step
#define HANDTP_IO_INFO_FIRST	0x01
#define HANDTP_IO_INFO_SECOND 	0x02
#define HANDTP_IO_INFO_THIRD 	0x03

//Cycle Commnad Define
#define CYC_CLEAR						(0)
//#define CYC_GO_MARK_FORWARD					(1)
#define CYC_MOVE						(2)
#define CYC_MOVE_UNLOAD					(3)
#define CYC_MOVE_LOAD					(4)
#define CYC_MTL_OUT						(5)
#define CYC_HAND_HP						(6)//hand home position
#define CYC_MAINT_DRIVING				(7)//저속 주행
#define CYC_NOMOVE_UNLOAD               (8)
#define CYC_NOMOVE_LOAD                 (9)
#define CYC_MOVE_ALL_HOME               (10)
#define CYC_AUTO_MODE_CHANGE            (11)
#define CYC_MOVE_AUTO_HOISTHOME         (12)


#define CYC_TEACHING_SAVE				(14)//이재 위치 티칭
#define CYC_MOVE_TEACHING_POSITION		(15)//Cycle 015 : 이재 티칭 위치 이동

#define CYC_GET_LOOKDOWN_LEVEL          (16)
#define CYC_SET_LOOKDOWN_LEVEL          (17)

#define CYC_MOVE_HOIST_TEACHING			(31)// Cycle 031 ? Hoist teaching 위치 이동
//#define CYC_JOG_ROTATE_HOME				(40)//Cycle 040 ? Rotate 원점 이동(저속)
#define CYC_MOVE_ROTATE_TEACHING		(41)//Cycle 041 ? Rotate teaching 위치 이동
//#define CYC_JOG_SHIFT_HOME				(50)//Cycle 050 ? Shift 원점 이동(저속)
#define CYC_MOVE_SHIFT_TEACHING			(51)//Cycle 051 ? Shift teaching 위치 이동



//#define CYC_MAPSET_DOWN					(100)//Cycle 100 ? Map Set 수신
#define CYC_MAPSET_UPLOAD				(101)//Cycle 101 ? Map Set 송신
#define CYC_MAPSET_DOWN_TEACHING        (102)           // 조정용 Map Down
#define CYC_MAPSET_DOWN_OPERATION       (103)           // 운영용 Map Down

#define CYC_MAP_MAKE_NOFILE				(110)//Cycle 110 ? Map Make
#define CYC_MAP_MAKE_FILE               (111)

#define CYC_AUTO_TEACHING_NOFILE        (112)
#define CYC_AUTO_TEACHING_FILE          (113)
#define CYC_AUTO_TEACHING_DRV_OFFSET    (114)

#define CYC_DRIVING_F_SERVO_OFF			(200)//Front servo Off
#define CYC_DRIVING_F_SERVO_ON			(201)//Front servo On
#define CYC_DRIVING_R_SERVO_OFF			(202)//Rear servo Off
#define CYC_DRIVING_R_SERVO_ON			(203)//Rear servo On

#define CYC_RAILSUPPORT_HOME			(204)//Out trigger Home
#define CYC_RAILSUPPORT_WORK			(205)//Out trigger Work
#define CYC_SHUTTER_ORIGIN				(206)//SHUTTER Origin

#define CYC_GO_MARK_FORWARD       		(208)//주행노드 마크
#define CYC_GO_TRANS_MARK_FORWARD		(209)//station mark
#define CYC_GO_QR_LEFT_MARK_FORWARD		(210)//QR Left mark
#define CYC_GO_QR_RIGHT_MARK_FORWARD	(211)//QR Right mark

#define CYC_HAND_OPEN					(230)//Cycle 060 ? Hand Grip 열림
#define CYC_HAND_CLOSE					(231)//Cycle 061 ? Hand Grip 닫힘
#define CYC_HAND_HOMING					(232)//hand home position
#define CYC_HAND_ENABLE					(233)// 추가
#define CYC_HAND_FREE					(234)// 추가
#define CYC_HAND_PIO_SELECT_OFF			(235)// 추가
#define CYC_HAND_PIO_SELECT_ON			(236)// 추가
#define CYC_HAND_PWR_OFF				(237)// 추가
#define CYC_HAND_PWR_ON					(238)// 추가
#define CYC_HAND_PWR_RST				(239)// 추가
#define CYC_HAND_RESET					(240)// 추가

#define CYC_HOIST_SERVO_ON				(250)// 추가
#define CYC_HOIST_SERVO_OFF				(251)// 추가
#define CYC_HOIST_ORIGIN_SETTING		(252)// 추가


#define CYC_JOG_HOIST_HOME				(500)//Cycle 030 ? Hoist 원점 이동(저속)
#define CYC_JOG_HOIST_STOP				(501)//  추가

#define CYC_JOG_SHIFT_HOME				(502)//추가
#define CYC_JOG_SHIFT_STOP				(503)//추가

#define CYC_JOG_ROTATE_HOME				(504)// 추가
#define CYC_JOG_ROTATE_STOP				(505)// 추가

#define CYC_LOOKDOWN_DISABLE			(506)// 추가
#define CYC_LOOKDOWN_ENABLE				(507)// 추가

#define CYC_SHIFT_ORIGIN_SETTING		(508)// 추가
#define CYC_SHIFT_SERVO_OFF				(509)// 추가
#define CYC_SHIFT_SERVO_ON				(510)// 추가

#define CYC_SHUTTER_OPEN				(600)//Cycle 090 ? Shutter 열림
#define CYC_SHUTTER_CLOSE				(601)//Cycle 091 ? Shutter 닫힘
#define CYC_SHUTTER_ALARM_RST			(602)// 추가
#define CYC_SHUTTER_ENABLE				(603)// 추가
#define CYC_SHUTTER_FREE				(604)// 추가

#define CYC_STEER_F_ENABLE				(605)// 추가
#define CYC_STEER_F_FREE				(606)//추가
#define CYC_STEER_LEFT					(607)//추가
#define CYC_STEER_R_ENABLE				(608)//추가
#define CYC_STEER_R_FREE				(609)//추가
#define CYC_STEER_RIGHT					(610)//추가
#define CYC_TRACTION_STEER_LEFT			(611)//추가
#define CYC_TRACTION_STEER_RIGHT		(612)//추가

//** PIO ***
#define CYC_LEFT_PIO_SELECT_ON			(700)//추가
#define CYC_LEFT_PIO_SELECT_OFF			(701)//추가
#define CYC_LEFT_PIO_VALID_ON			(702)//추가
#define CYC_LEFT_PIO_VALID_OFF			(703)//추가
#define CYC_LEFT_PIO_TR_REQ_ON			(704)//추가
#define CYC_LEFT_PIO_TR_REQ_OFF			(705)//추가
#define CYC_LEFT_PIO_BUSY_ON			(706)//추가
#define CYC_LEFT_PIO_BUSY_OFF			(707)//추가
#define CYC_LEFT_PIO_COMPT_ON			(708)//추가
#define CYC_LEFT_PIO_COMPT_OFF			(709)//추가
#define CYC_LEFT_PIO_ALL_OFF			(710)//추가

#define CYC_RIGHT_PIO_SELECT_ON			(720)//추가
#define CYC_RIGHT_PIO_SELECT_OFF		(721)//추가
#define CYC_RIGHT_PIO_VALID_ON			(722)//추가
#define CYC_RIGHT_PIO_VALID_OFF			(723)//추가
#define CYC_RIGHT_PIO_TR_REQ_ON			(724)//추가
#define CYC_RIGHT_PIO_TR_REQ_OFF		(725)//추가
#define CYC_RIGHT_PIO_BUSY_ON			(726)//추가
#define CYC_RIGHT_PIO_BUSY_OFF			(727)//추가
#define CYC_RIGHT_PIO_COMPT_ON			(728)//추가
#define CYC_RIGHT_PIO_COMPT_OFF			(729)//추가
#define CYC_RIGHT_PIO_ALL_OFF			(730)//추가

#define CYC_ROTATE_0					(750)//추가
#define CYC_ROTATE_90					(751)//추가
#define CYC_ROTATE_180					(752)//추가
#define CYC_ROTATE_270					(753)//현재 미사용
#define CYC_ROTATE_HOMING				(754)//추가
#define CYC_ROTATE_GO_ORIGIN			(755)//추가
#define CYC_ROTATE_RESET				(756)//추가
#define CYC_ROTATE_FREE					(757)//추가
#define CYC_ROTATE_ENABLE				(758)//추가
#define CYC_ROTATE_SERVO_OFF			(759)//추가
#define CYC_ROTATE_SERVO_ON				(760)//추가
#define CYC_ROTATE_ORIGIN_SETTING		(761)//추가

////////////////////////////////////////////////
//** 원점 OFFSET SETTING **
//#define TPCMD_HOIST_ORIGIN_SETTING      0x5171
//#define TPCMD_SHIFT_ORIGIN_SETTING      0x5172
//#define TPCMD_HOIST_SETLIMIT_NEG        0x5175
//#define TPCMD_HOIST_SETLIMIT_POSI       0x5176
//#define TPCMD_SHIFT_SETLIMIT_NEG        0x5177
//#define TPCMD_SHIFT_SETLIMIT_POSI       0x5178
////////////////////////////////////////////////

#define CYC_CID_LEFT_SEL				(800)// 추가
#define CYC_CID_RIGHT_SEL				(801)// 추가
#define CYC_CID_OCCUPREQ				(802)// 추가
#define CYC_CID_PASSCOMPT				(803)// 추가
#define CYC_CID_MANUAL					(804)// 추가
#define CYC_CID_OPERATION				(805)// 추가
#define CYC_CID_RESERVE_1				(806)// 추가
#define CYC_CID_RESERVE_2				(807)// 추가
#define CYC_CID_LEFT_SEL_OFF			(808)// 추가
#define CYC_CID_RIGHT_SEL_OFF			(809)// 추가
#define CYC_CID_OCCUPREQ_OFF			(810)// 추가
#define CYC_CID_PASSCOMPT_OFF			(811)// 추가
#define CYC_CID_MANUAL_OFF				(812)// 추가
#define CYC_CID_OPERATION_OFF			(813)// 추가
#define CYC_CID_RESERVE_1_OFF			(814)// 추가
#define CYC_CID_RESERVE_2_OFF			(815)// 추가

#define CYC_OHT_MANUALMODE				(900)//Cycle 999 ? VHL MODE 변경
#define CYC_OHT_AUTOMODE				(901)//Cycle 999 ? VHL MODE 변경
#define CYC_OHT_FORCE_PP				(910)//Cycle 999 ? VHL MODE 변경

// 2016-12-25, Add by Labeler, Longrun Cycle 추가
#define CYC_TRANS_LONGRUN_START         (911)
#define CYC_TRANS_LONGRUN_STOP          (912)
#define CYC_HAND_LONGRUN_START          (913)
#define CYC_HAND_LONGRUN_STOP           (914)
#define CYC_SHUTTER_LONGRUN_START       (915)
#define CYC_SHUTTER_LONGRUN_STOP        (916)
#define CYC_STEER_LONGRUN_START         (917)
#define CYC_STEER_LONGRUN_STOP          (918)

#define CYC_VHL_ERR_RESET				(999)//Cycle 110 ? Map Make

/*
 Day 	: 2017.01.10
 Author : Labeler
 brief 	: define Variable for Manual Command
*/
#define MANUAL_KEY_JOG_SLOW_DRIVING_FW			0x01
#define MANUAL_KEY_JOG_SLOW_DRIVING_BW			0x02
#define MANUAL_KEY_JOG_MIDDLE_DRIVING_FW		0x03
#define MANUAL_KEY_JOG_MIDDLE_DRIVING_BW		0x04
#define MANUAL_KEY_DRIVING_SPARE1				0x05
#define MANUAL_KEY_DRIVING_SPARE2				0x06

#define MANUAL_KEY_JOG_HOIST_UP					0x07
#define MANUAL_KEY_JOG_HOIST_DOWN				0x08
#define MANUAL_KEY_HOIST_HOME					0x09
#define MANUAL_KEY_HOIST_SPARE1					0x0A
#define MANUAL_KEY_HOIST_SPARE2					0x0B

#define MANUAL_KEY_JOG_SLIDE_LEFT				0x0C
#define MANUAL_KEY_JOG_SLIDE_RIGHT				0x0D
#define MANUAL_KEY_SLIDE_HOME					0x0E
#define MANUAL_KEY_SLIDE_SPARE1					0x0F
#define MANUAL_KEY_SLIDE_SPARE2					0x10

#define MANUAL_KEY_JOG_ROTATE_CCW				0x11
#define MANUAL_KEY_JOG_ROTATE_CW				0x12
#define MANUAL_KEY_ROTATE_HOME					0x13
#define MANUAL_KEY_ROTATE_ORIGIN				0x14
#define MANUAL_KEY_ROTATE_SPARE2				0x15

#define MANUAL_KEY_STEER_LEFT					0x16
#define MANUAL_KEY_STEER_RIGHT					0x17
#define MANUAL_KEY_STEER_SPARE1					0x18
#define MANUAL_KEY_STEER_SPARE2					0x19

#define MANUAL_KEY_HAND_OPEN					0x1A
#define MANUAL_KEY_HAND_CLOSE					0x1B
#define MANUAL_KEY_HAND_ORIGIN					0x1C
#define MANUAL_KEY_HAND_RESET					0x1D
#define MANUAL_KEY_HAND_FREE					0x1E
#define MANUAL_KEY_HAND_ENABLE					0x1F

#define MANUAL_KEY_SHUTTER_OPEN					0x20
#define MANUAL_KEY_SHUTTER_CLOSE				0x21
#define MANUAL_KEY_SHUTTER_ORIGIN				0x22
#define MANUAL_KEY_SHUTTER_RESET				0x23
#define MANUAL_KEY_SHUTTER_FREE					0x24
#define MANUAL_KEY_SHUTTER_ENABLE				0x25

#define MANUAL_KEY_OUT_TRIGGER_WORK				0x26
#define MANUAL_KEY_OUT_TRIGGER_HOME				0x27
#define MANUAL_KEY_OUT_TRIGGER_FREE				0x28
#define MANUAL_KEY_OUT_TRIGGER_ENABLE			0x29

#define MANUAL_KEY_MARK_DRIVING					0x2A
#define MANUAL_KEY_MARK_TRANS					0x2B
#define MANUAL_KEY_MARK_QR_LEFT					0x2C
#define MANUAL_KEY_MARK_QR_RIGHT				0x2D

#define MANUAL_KEY_FORCE_PP						0x90
#define MANUAL_KEY_MODE_TO_AUTO					0x91
#define MANUAL_KEY_MODE_TO_MANUAL				0x92
#define MANUAL_KEY_ESTOP						0x93
#define MANUAL_KEY_RESET						0x94

#define MANUAL_KEY_TP_PAUSE						0x95
#define MANUAL_KEY_TP_RESUME					0x96



#define HANDTP_ERR_NO_ERR					0		// Error 없는 상태

// 1 ~ 10 : Is_Executable() 반환 값
#define HANDTP_ERR_MODE_AUTO            	1		// OHT가 Auto Mode 상태
#define HANDTP_ERR_OHT_LOADING				2		// OHT가 Load 중
#define HANDTP_ERR_OHT_UNLOADING			3		// OHT가 Unload 중
//#define HANDTP_ERR_OHT_AUTO_TEACHING		3		// OHT가 Auto Teaching 진행 중
#define HANDTP_ERR_OHT_GOING				4		// OHT가 주행 중
#define HANDTP_ERR_F_DRV_RUN				5		// Front Driving이 동작 중
#define HANDTP_ERR_R_DRV_RUN				6		// Rear Driving이 동작 중
#define HANDTP_ERR_HOIST_RUN				7		// Hoist 축이 동작 중
#define HANDTP_ERR_SHIFT_RUN				8		// Shift 축이 동작 중
#define HANDTP_ERR_ROTATE_RUN				9		// Rotate 축이 동작 중

// 11 ~ 20 : OHT Status 관련 Error
#define HANDTP_ERR_OHT_ERR					11		// OHT에 Error가 있는 상태
#define HANDTP_ERR_OHT_NOT_INIT				12		// OHT가 Init이 아닌 상태
#define HANDTP_ERR_NO_MARK					13		// OHT가 No Mark 상태
#define HANDTP_ERR_OHT_LONGRUN				20		// OHT가 Longrun 동작 중

// 21 ~ 30 : Node, Station, Tag 관련 Error
#define HANDTP_ERR_CUR_NODE_NOT_EXIST		21		// Current Node 정보가 없는 상태
#define HANDTP_ERR_CUR_STATION_NOT_EXIST	22		// Current Station 정보가 없는 상태
#define HANDTP_ERR_NOT_INPUT_TARGET_INFO	23		// 목적 위치 정보가 입력되지 않음
#define HANDTP_ERR_NOT_INPUT_STATION_ID		24		// Station ID 정보가 입력 되지 않음
#define HANDTP_ERR_NOT_INPUT_PORT_TYPE		25		// Port Type 정보가 입력 되지 않음

// 31 ~ 40 : Driving Axis 관련 Error
#define HANDTP_ERR_AXIS1_F_ERROR_LIMIT		31		// F_Drv Error Limit
#define HANDTP_ERR_AXIS1_R_ERROR_LIMIT		32		// R_Drv Error Limit
#define HANDTP_ERR_AXIS1_F_AMP_FAULT		33		// F_Drv Amp Fault
#define HANDTP_ERR_AXIS1_R_AMP_FAULT		34		// R_Drv Amp Fault
#define HANDTP_ERR_STEER_DIFER_DIR			35		// Steer Direction Differ

// 41 ~ 50 : Hoist Axis 관련 Error
// 41번은 Hand TP, Linker ETX라 사용하면 안됨
#define HANDTP_ERR_HOIST_NOT_HOME			42		// Hoist Axis가 Home Position이 아님
#define HANDTP_ERR_OSCILLATION_OFF			43		// Ocsillation이 감지 안됨
#define HANDTP_ERR_HOIST_N_LIMIT			44		// Hoist Negative Limit
#define HANDTP_ERR_HOIST_P_LIMIT			45		// Hoist Position Limit

// 51 ~ 60 : Shift Axis 관련 Error
#define HANDTP_ERR_SHIFT_NOT_HOME			51		// Shift Axis가 Home Position이 아님
#define HANDTP_ERR_SHIFT_N_LIMIT			52		// Shift Negative Limit
#define HANDTP_ERR_SHIFT_P_LIMIT			53		// Shift Positive Limit

// 61 ~ 70 : Rotate Axis 관련 Error
#define HANDTP_ERR_ROTATE_NOT_INPOSITION	61		// Rotate Axis가 0도 or 180도 가 아님
#define HANDTP_ERR_ROTATE_N_LIMIT			62		// Rotate Negative Limit
#define HANDTP_ERR_ROTATE_P_LIMIT			63		// Rotate Positive Limit
#define HANDTP_ERR_ROTATE_NOT_ORIGIN		64		// Rotate Not Origin

// 71 ~ 80 : Hand 관련 Error
#define HANDTP_ERR_HAND_NOT_CLOSE			71		// Hand Unit이 Close 상태가 아님
#define HANDTP_ERR_HAND_NOT_OPEN			72		// Hand Unit이 Open 상태가 아님
#define HANDTP_ERR_HAND_NOT_ORIGIN			73		// Hand Unit이 Origin 상태가 아님

// 81 ~ 90 : Shutter 관련 Error
#define HANDTP_ERR_SHUTTER_NOT_OPEN			81		// Shutter가 Open 상태가 아님
#define HANDTP_ERR_SHUTTER_NOT_CLOSE		82		// Shutter가 close 상태가 아님
#define HANDTP_ERR_SHUTTER_NOT_ORIGIN		83		// Shutter가 Origin 상태가 아님

// 91 ~ 100 : Out Trigger 관련 Error
#define HANDTP_ERR_OUT_TRIGGER_NOT_HOME		91		// Out Trigger가 Home 상태가 아님

// 101 ~ 110 : Carrier 관련 Error
#define HANDTP_ERR_CARRIER_EXIST			101		// 재하가 감지 됨
#define HANDTP_ERR_CARRIER_MISMATCH			102		// 재하 감지 상태 비정상

// 200 ~ 255 : 기타 관련 Error
#define HANDTP_ERR_OCS_PAUSED				200		// Ocs Pause 상태
#define HANDTP_ERR_TP_PAUSED				201		// TP Pause 상태
#define HANDTP_ERR_CID_PAUSED				202		// CID Pause 상태
#define HANDTP_ERR_CANT_PAUSE_RESUME		203		// Pause 해제 불가


#define HANDTP_ERR_NOT_TRANS_LONGRUN		211		// Trans Longrun 중이 아님
#define HANDTP_ERR_NOT_HAND_LONGRUN			212		// Hand Longrun 중이 아님
#define HANDTP_ERR_NOT_SHUTTER_LONGRUN		213		// Shutter Longrun 중이 아님
#define HANDTP_ERR_NOT_STEER_LONGRUN		214		// Steer Longrun 중이 아님

#define HANDTP_ERR_RECV_DATA_ERROR          241
#define HANDTP_ERR_VHL_INFO_ERROR           242


#define HANDTP_ERR_NO_USE_CMD				255		// 미사용 Command

/*
#define HANDTP_OHT_MODE_AUTO            1
#define HANDTP_OHT_LOADING              2
#define HANDTP_OHT_UNLOADING            3
#define HANDTP_OHT_GOING                4
#define HANDTP_OHT_DRV_F_RUNNING        5
#define HANDTP_OHT_DRV_R_RUNNING        6
#define HANDTP_OHT_HOIST_RUNNING        7
#define HANDTP_OHT_SHIFT_RUNNING        8
#define HANDTP_OHT_ROTATE_RUNNINH       9
#define HANDTP_OHT_HOIST_LIMIT          10
#define HANDTP_OHT_SHIFT_LIMIT          11
#define HANDTP_OHT_RAIL_NOT_HOME        12
#define HANDTP_OHT_DRV_RUNNING          13
#define HANDTP_OHT_SHUTTER_OPEN         14
#define HANDTP_OHT_OSCILL_OFF           15
#define HANDTP_OHT_HAND_LIMIT_SENSOR    16
#define HANDTP_OHT_SHUTTER_CLOSE        17
#define HANDTP_OHT_HAND_GRIP_ON         18
#define HANDTP_OHT_HOIST_NOT_ORIGIN     19
#define HANDTP_OHT_SHIFT_NOT_ORIGIN     20
#define HANDTP_OHT_FOUP_DETECT          21
#define HANDTP_OHT_MOTOR_ERROR          22
#define HANDTP_OHT_NOT_INIT             23
#define HANDTP_OHT_NO_MARK_INFO         24
#define HANDTP_OHT_BEFORE_AUTO          25
#define HANDTP_OHT_NO_NODE_ID           26
#define HANDTP_RECV_DATA_ERROR          27
#define HANDTP_OHT_RUNNING              28
#define HANDTP_OHT_MODE_MANUAL          30
#define HANDTP_OHT_NO_STATION_INFO      50

#define HANDTP_CANT_MANUAL_KEY          60
#define HANDTP_CANT_CYCLE               61
#define HANDTP_CANT_GLOBAL_KEY          62

#define HANDTP_VHL_INFO_ERROR           99
*/
#define HANDTP_DATA_MIN_LENGTH          1


#define HANDTP_LONGRUN_NONE             0
#define HANDTP_LONGRUN_TRANS            1
#define HANDTP_LONGRUN_HAND             2
#define HANDTP_LONGRUN_SHUTTER          3
#define HANDTP_LONGRUN_STEER            4
/**
@class   Remocon
@date    2012.10.22
@author
@brief   Remocon 관련 Class
*/
class Remocon
{
private:
	Comm_Port *m_pCommPort;

	bool bConnect;
	int m_nComPortNum;
	int m_nBaudRate;

public:
	Remocon(HWCommon *pHWCommon);
	~Remocon();

	bool IsConnect();

	int OpenCom();
	int CloseCom();
	int ReOpenCom();

	int GetID(unsigned char* pReadBuff);
	int SendTriger(unsigned char* ackData, int length);
//	WORD crc16(WORD crc, BYTE *ptr,WORD size);       //미사용
};

#endif   //RemoconH


