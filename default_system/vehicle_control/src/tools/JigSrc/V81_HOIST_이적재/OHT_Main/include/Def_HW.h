
/**
@file    Def_HW.h
@version OHT 7.0
@brief   HW에 공통으로 사용될 상수, 변수를 정의한 Header File
*/

#ifndef Def_HWH
#define Def_HWH

#include "Def_OHT.h"
#include "ErrorCode.h"


// -------------------------------------------
// IO 관련 구조체
// -------------------------------------------

/**
@brief   Input Bit Field 구조
         Input 0~63
@author  zzang9un
@date    2012.11.26
*/
typedef struct sINPUT_BITFIELD_
{
	UINT64 HAND_Inposition         : 1; // 0
    UINT64 HAND_Alarm              : 1; // 1
    UINT64 HAND_Right_Grip         : 1; // 2
    UINT64 HAND_Left_Grip          : 1; // 3
	UINT64 HAND_Close              : 1; // 4
    UINT64 HAND_Open               : 1; // 5
	UINT64 HAND_Push               : 1; // 6
	UINT64 HOIST_Home              : 1; // 7
    UINT64 HAND_PIO_Go             : 1; // 8
    UINT64 EQPIO_Load_REQ          : 1; // 9
    UINT64 EQPIO_Unload_REQ        : 1; // 10
    UINT64 EQPIO_Ready             : 1; // 11
    UINT64 EQPIO_HO_AVBL           : 1; // 12
    UINT64 EQPIO_ES                : 1; // 13
    UINT64 EQPIO_Left_Go           : 1; // 14
    UINT64 EQPIO_Right_Go          : 1; // 15
    UINT64 STEER_F_Left            : 1; // 16
	UINT64 STEER_R_Left            : 1; // 17
	UINT64 STEER_F_Right           : 1; // 18
    UINT64 STEER_R_Right           : 1; // 19
    UINT64 SHUTTER_F_Close         : 1; // 20
	UINT64 SHUTTER_R_Close         : 1; // 21
    UINT64 SHUTTER_F_Open          : 1; // 22
    UINT64 SHUTTER_R_Open          : 1; // 23
    UINT64 SHUTTER_F_Alarm         : 1; // 24
    UINT64 SHUTTER_R_Alarm         : 1; // 25
    UINT64 SHUTTER_F_Inposition    : 1; // 26
    UINT64 SHUTTER_R_Inposition    : 1; // 27
    UINT64 OBSTACLE_Long           : 1; // 28
    UINT64 OBSTACLE_Middle         : 1; // 29
    UINT64 OBSTACLE_Short          : 1; // 30
    UINT64 OBSTACLE_Fail           : 1; // 31
    UINT64 OHTDETECT_Long          : 1; // 32
    UINT64 OHTDETECT_Middle        : 1; // 33
    UINT64 OHTDETECT_Short         : 1; // 34
    UINT64 OHTDETECT_Fail          : 1; // 35
    UINT64 LOOKDOWN_Detect         : 1; // 36
    UINT64 LOOKDOWN_Error          : 1; // 37
    UINT64 SHIFT_Home              : 1; // 38
    UINT64 STB_Detect_Left         : 1; // 39
    UINT64 STB_Detect_Right        : 1; // 40
    UINT64 UTB_Detect              : 1; // 41
	UINT64 FOUP_Detect             : 1; // 42
	UINT64 OSCILLATION             : 1; // 43
	UINT64 EMO_Press               : 1; // 44
    UINT64 BUMPER_Press            : 1; // 45
    UINT64 FAN_Fail                : 1; // 46
    UINT64 STB_OHT_Detect_Left     : 1; // 47
    UINT64 STB_OHT_Detect_Right    : 1; // 48
	UINT64 HANDTP_Connect          : 1; // 49
    UINT64 STOP_Driving_PreCheck   : 1; // 50
    UINT64 MC_Status               : 1; // 51
    UINT64 STOP_Driving_Front      : 1; // 52
    UINT64 STOP_Trans_Front        : 1; // 53
    UINT64 STOP_Trans_Rear         : 1; // 54
    UINT64 BCR_Driving_OK          : 1; // 55
    UINT64 BCR_Driving_NG          : 1; // 56
    UINT64 BCR_Trans_OK            : 1; // 57
    UINT64 BCR_Trans_NG            : 1; // 58
    UINT64 OHT_COMM_CD             : 1; // 59
    UINT64 RailSupport_Home      : 1; // 60
	UINT64 Railsupport_Work      : 1; // 61
    UINT64 MAC_Detect      : 1; // 62
    UINT64 SMPS_Short      : 1; // 63
} sINPUT_BITFIELD;

/**
@brief   Input(0~63) Bit Field 사용을 위한 Union 구조체
@author  zzang9un
@date    2012.11.26
*/
typedef union uINPUT_BITFIELD_
{
	sINPUT_BITFIELD Info;
    UINT64 Data;
} uINPUT_BITFIELD;



/**
@brief   HWCommon에서 사용하는 Input 64개
@author  zzang9un
@date    2012.10.15
@note    Bit Filed에서 unsigned char로 변경
*/
#if (OHT_NAME == OHT_NAME_STD_V80)
typedef struct sINPUTR01_
{
	BYTE HAND_Inposition;
	BYTE HAND_Alarm;
	BYTE HAND_Right_Grip;
	BYTE HAND_Left_Grip;
	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Push;
	BYTE HOIST_Home;
	BYTE HAND_PIO_Go;
	BYTE EQPIO_Load_REQ;
	BYTE EQPIO_Unload_REQ;
	BYTE EQPIO_Ready;
	BYTE EQPIO_HO_AVBL;
	BYTE EQPIO_ES;
	BYTE EQPIO_Right_Go;
	BYTE EQPIO_Left_Go;
	BYTE STEER_F_Left;
	BYTE STEER_R_Left;
	BYTE STEER_F_Right;
	BYTE STEER_R_Right;
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;
	BYTE SHUTTER_F_Alarm;
	BYTE SHUTTER_R_Alarm;
	BYTE SHUTTER_F_Inposition;
	BYTE SHUTTER_R_Inposition;
	BYTE OBSTACLE_Long;
	BYTE OBSTACLE_Middle;
	BYTE OBSTACLE_Short;
	BYTE OBSTACLE_Fail;

	BYTE OHTDETECT_Long;
	BYTE OHTDETECT_Middle;
	BYTE OHTDETECT_Short;
	BYTE OHTDETECT_Fail;
	BYTE LOOKDOWN_Detect;
	BYTE LOOKDOWN_Error;
	BYTE SHIFT_Home;
	BYTE STB_Detect_Left;
	BYTE STB_Detect_Right;
	BYTE UTB_Detect;
	BYTE FOUP_Detect;
	BYTE OSCILLATION;
	BYTE EMO_Press;
	BYTE BUMPER_Press;
	BYTE FAN_Fail;
	BYTE HOIST_Belt_Detect2;
	BYTE TP_Enable;
	BYTE HANDTP_Connect;
	BYTE FOUP_Cover_Open_Detect;
	BYTE STOP_Driving_Front;
	BYTE STOP_Driving_PreCheck;
	BYTE STOP_Trans_Front;
	BYTE STOP_Trans_Rear;
	BYTE BCR_Driving_OK;
	BYTE BCR_Driving_NG;
	BYTE BCR_Trans_OK;
	BYTE BCR_Trans_NG;
	BYTE HOIST_Belt_Detect1;
	BYTE PIO_Input3;
	BYTE PIO_Input5;
	BYTE PIO_Input6;
	BYTE SMPS_Short;
	//TG 추가
	BYTE CID_Link_Compt_IN;
	BYTE CID_Interlock_IN;
	BYTE CID_Completion_ACK;
	BYTE CID_Status_IN;
	BYTE CID_Reserved_IN1;
	BYTE CID_Reserved_IN2;
	BYTE Rotate_0_Sensor;
	BYTE Rotate_90_Sensor;
	BYTE Rotate_180_Sensor;
	BYTE Rotate_270_Sensor;
	BYTE Reverved_74;
	BYTE Reverved_75;
	BYTE Reverved_76;
	BYTE Reverved_77;
	BYTE Reverved_78;
	BYTE Reverved_79;

}sINPUTR01;

#elif (OHT_NAME == OHT_NAME_STD_V81)
typedef struct sINPUTV81_
{
	BYTE HAND_Inposition;
	BYTE HAND_Alarm;
	BYTE HAND_Right_Grip;
	BYTE HAND_Left_Grip;
	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Push;
	BYTE HOIST_Home;
	BYTE HAND_PIO_Go;
	BYTE EQPIO_Load_REQ;
	BYTE EQPIO_Unload_REQ;
	BYTE EQPIO_Ready;
	BYTE EQPIO_HO_AVBL;
	BYTE EQPIO_ES;
	BYTE EQPIO_Right_Go;
	BYTE EQPIO_Left_Go;
	BYTE STEER_F_Left;
	BYTE STEER_R_Left;
	BYTE STEER_F_Right;
	BYTE STEER_R_Right;
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;
	BYTE SHUTTER_F_Alarm;
	BYTE SHUTTER_R_Alarm;
	BYTE SHUTTER_F_Inposition;
	BYTE SHUTTER_R_Inposition;
	BYTE OBSTACLE_Long;
	BYTE OBSTACLE_Middle;
	BYTE OBSTACLE_Short;
	BYTE OBSTACLE_Fail;

	BYTE OHTDETECT_Long;
	BYTE OHTDETECT_Middle;
	BYTE OHTDETECT_Short;
	BYTE OHTDETECT_Fail;
	BYTE LOOKDOWN_Detect;
	BYTE LOOKDOWN_Error;
	BYTE SHIFT_Home;
	BYTE STB_Detect_Left;
	BYTE STB_Detect_Right;
	BYTE UTB_Detect;
	BYTE FOUP_Detect;
	BYTE OSCILLATION;
	BYTE EMO_Press;
	BYTE BUMPER_Press;
	BYTE FAN_Fail;
	BYTE HOIST_Belt_Detect2;
	BYTE TP_Enable;
	BYTE HANDTP_Connect;
	BYTE FOUP_Cover_Open_Detect;
	BYTE STOP_Driving_Front;
	BYTE STOP_Driving_PreCheck;
	BYTE STOP_Trans_Front;
	BYTE STOP_Trans_Rear;
	BYTE BCR_Driving_OK;
	BYTE BCR_Driving_NG;
	BYTE BCR_Trans_OK;
	BYTE BCR_Trans_NG;
	BYTE HOIST_Belt_Detect1;
	BYTE PIO_Input3;
	BYTE PIO_Input5;
	BYTE PIO_Input6;
	BYTE SMPS_Short;
	//TG 추가
	BYTE CID_Link_Compt_IN;
	BYTE CID_Interlock_IN;
	BYTE CID_Completion_ACK;
	BYTE CID_Status_IN;
	BYTE CID_Reserved_IN1;
	BYTE CID_Reserved_IN2;
	BYTE Rotate_0_Sensor;
	BYTE Rotate_90_Sensor;
	BYTE Rotate_180_Sensor;
	BYTE Rotate_270_Sensor;
	BYTE Reverved_74;
	BYTE Reverved_75;
	BYTE Reverved_76;
	BYTE Reverved_77;
	BYTE Reverved_78;
	BYTE Reverved_79;

}sINPUTV81;
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
typedef struct sRETICLEINPUT_
{
	// 0 - 19
	BYTE HAND_Inposition;
	BYTE HAND_Alarm;
	BYTE HAND_Right_Grip;
	BYTE HAND_Left_Grip;
	BYTE HAND_Close;
			
	BYTE HAND_Open;
	BYTE HAND_Push;
	BYTE Hand_Reserved7;
	BYTE HAND_PIO_Go;
	BYTE EQPIO_Load_REQ;
			
	BYTE EQPIO_Unload_REQ;
	BYTE EQPIO_Ready;
	BYTE EQPIO_HO_AVBL;
	BYTE EQPIO_ES;
	BYTE EQPIO_Right_Go;
			
	BYTE EQPIO_Left_Go;
	BYTE STEER_F_Left;
	BYTE STEER_R_Left;
	BYTE STEER_F_Right;
	BYTE STEER_R_Right;

	// 20 - 39
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;
	BYTE SHUTTER_F_Alarm;
			
	BYTE SHUTTER_R_Alarm;
	BYTE SHUTTER_F_Inposition;
	BYTE SHUTTER_R_Inposition;
	BYTE OBSTACLE_Long;
	BYTE OBSTACLE_Middle;
			
	BYTE OBSTACLE_Short;
	BYTE OBSTACLE_Fail;

	BYTE OHTDETECT_Long;
	BYTE OHTDETECT_Middle;
	BYTE OHTDETECT_Short;
			
	BYTE OHTDETECT_Fail;
	BYTE LOOKDOWN_Detect;
	BYTE LOOKDOWN_Error;
	BYTE SHIFT_Home;
	BYTE Reserved40;

	// 40 - 59
	BYTE Reserved41;
	BYTE Reserved42;
	BYTE FOUP_Detect;
	BYTE OSCILLATION;
	BYTE EMO_Press;
			
	BYTE BUMPER_Press;
	BYTE FAN_Fail;
	BYTE Reserved48;
	BYTE Reserved49;
	BYTE HANDTP_Connect;
			
	BYTE Reserved51;
	BYTE STOP_Driving_Front;
	BYTE STOP_Driving_PreCheck;
	BYTE STOP_Trans_Front;
	BYTE STOP_Trans_Rear;
			
	BYTE BCR_Driving_OK;
	BYTE BCR_Driving_NG;
	BYTE BCR_Trans_OK;
	BYTE BCR_Trans_NG;
	BYTE HOIST_Belt_Detect1;

	// 60 - 79
	BYTE PIO_Input3;
	BYTE PIO_Input5;
	BYTE PIO_Input6;
	BYTE SMPS_Short;
	//TG 추가
	BYTE CID_Link_Compt_IN;
	BYTE CID_Interlock_IN;
	BYTE CID_Completion_ACK;
	BYTE CID_Status_IN;
	BYTE CID_Reserved_IN1;
	BYTE CID_Reserved_IN2;
	BYTE Rotate_0_Sensor;
	BYTE Rotate_90_Sensor;
	BYTE Rotate_180_Sensor;
	BYTE Rotate_270_Sensor;
	BYTE HOIST_Home;
	BYTE Rotate_Reserved76;
	BYTE Rotate_Reserved77;
	BYTE Rotate_Reserved78;
	BYTE Rotate_Reserved79;
	BYTE Rotate_Reserved80;
}sRETICLEINPUT;

#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))

typedef struct sRETICLEINPUT_
{
	// 0 - 15
	BYTE HAND_Inposition;
	BYTE HAND_Alarm;
	BYTE HAND_Right_Grip;
	BYTE HAND_Left_Grip;
			
	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Push;
	BYTE HOIST_Align;		//   Hand_Reserved7;
			
	BYTE HAND_PIO_Go;
	BYTE EQPIO_Load_REQ;
	BYTE EQPIO_Unload_REQ;
	BYTE EQPIO_Ready;
			
	BYTE EQPIO_HO_AVBL;
	BYTE EQPIO_ES;
	BYTE EQPIO_Right_Go;
	BYTE EQPIO_Left_Go;

	// 16 - 31
	BYTE STEER_F_Left;
	BYTE STEER_R_Left;
	BYTE STEER_F_Right;
	BYTE STEER_R_Right;
			
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;
			
	BYTE SHUTTER_F_Alarm;
	BYTE SHUTTER_R_Alarm;
	BYTE SHUTTER_F_Inposition;
	BYTE SHUTTER_R_Inposition;
			
	BYTE OBSTACLE_Long;
	BYTE OBSTACLE_Middle;
	BYTE OBSTACLE_Short;
	BYTE OBSTACLE_Fail;

	// 32 - 47
	BYTE OHTDETECT_Long;
	BYTE OHTDETECT_Middle;
	BYTE OHTDETECT_Short;
	BYTE OHTDETECT_Fail;
			
	BYTE LOOKDOWN_Detect;
	BYTE LOOKDOWN_Error;
	BYTE SHIFT_Home;
	BYTE STB_Detect_Left;		// Reserved40;
			
	BYTE STB_Detect_Right;		// Reserved41;
	BYTE UTB_Detect;	// Reserved42;
	BYTE FOUP_Detect;
	BYTE OSCILLATION;
			
	BYTE EMO_Press;
	BYTE BUMPER_Press;
	BYTE FAN_Fail;
	BYTE HOIST_Belt;	// Reserved48;

	// 48 - 63
	BYTE TP_Enable;		// Reserved49;
	BYTE HANDTP_Connect;
	BYTE FOUB_Cover_Detect;	// Reserved51;
	BYTE STOP_Driving_Front;
			
	BYTE STOP_Driving_PreCheck;
	BYTE STOP_Trans_Front;
	BYTE STOP_Trans_Rear;
	BYTE BCR_Driving_OK;
			
	BYTE BCR_Driving_NG;
	BYTE BCR_Trans_OK;
	BYTE BCR_Trans_NG;
	BYTE HOIST_Belt_Detect1;
			
	BYTE PIO_Input3;
	BYTE PIO_Input5;
	BYTE PIO_Input6;
	BYTE SMPS_Short;

	// 64 - 79
		//TG 추가
	BYTE CID_Link_Compt_IN;
	BYTE CID_Interlock_IN;
	BYTE CID_Completion_ACK;
	BYTE CID_Status_IN;
			
	BYTE CID_Reserved_IN1;
	BYTE CID_Reserved_IN2;
	BYTE Rotate_0_Sensor;
	BYTE Rotate_90_Sensor;
	BYTE Rotate_180_Sensor;
	BYTE Rotate_270_Sensor;
	BYTE HOIST_Home;
	BYTE SHUTTER_F_Middle_Close;
			
	BYTE SHUTTER_R_Middle_Close;
	BYTE POD_EUV_1;
	BYTE POD_EUV_2;
	BYTE SPARE_5;

	BYTE SPARE_6;
	BYTE SPARE_7;
	BYTE SPARE_8;
	BYTE SPARE_9;
			
	BYTE SPARE_10;
	BYTE SPARE_11;
	BYTE SPARE_12;
	BYTE SPARE_13;
			
	BYTE SPARE_14;
	BYTE SPARE_15;
	BYTE SPARE_16;
	BYTE SPARE_17;
			
	BYTE SPARE_18;
	BYTE SPARE_19;
}sRETICLEINPUT;

 #elif(OHT_NAME == OHT_NAME_STD_V85)
typedef struct sSTANDARDINPUT_V85_
{
	// 10개(No mapping)
	BYTE HAND_Inposition;
	BYTE HAND_Alarm;
	BYTE HAND_Right_Grip;
	BYTE HAND_Left_Grip;
	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Push;
	BYTE HNAD_Reserved_1;		// 미사용
	BYTE HNAD_ES;               // 미사용
	BYTE HAND_PIO_Go;


	// 10개(No mapping)
	BYTE EQPIO_Load_REQ;
	BYTE EQPIO_Unload_REQ;
	BYTE EQPIO_Ready;
	BYTE EQPIO_HO_AVBL;
	BYTE EQPIO_ES;
	BYTE EQPIO_Left_Go;   		//	BYTE EQPIO_Right_Go;미사용
	BYTE STEER_F_Left;
	BYTE STEER_R_Left;
	BYTE STEER_F_Right;
	BYTE STEER_R_Right;

	// 10개(No mapping)
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;
	BYTE SHUTTER_F_Alarm;
	BYTE SHUTTER_R_Alarm;
	BYTE SHUTTER_F_Inposition;
	BYTE SHUTTER_R_Inposition;
	BYTE OBSTACLE_Input_Extend;     	// 미사용
	BYTE OBSTACLE_Long;

	// 10개(No mapping)
	BYTE OBSTACLE_Middle;
	BYTE OBSTACLE_Short;
	BYTE OBSTACLE_Fail;
    BYTE OHTDETECT_Input_Extend;   	// 미사용
	BYTE OHTDETECT_Long;
	BYTE OHTDETECT_Middle;
	BYTE OHTDETECT_Short;
	BYTE OHTDETECT_Fail;
	BYTE LOOKDOWN_Detect;
	BYTE LOOKDOWN_Error;



	// 10개(No mapping)
	BYTE SHIFT_Home;
	BYTE STB_Detect_Left;
	BYTE STB_Detect_Right;
	BYTE UTB_Detect;				// 미사용
	BYTE FOUP_Detect;
	BYTE OSCILLATION;
	BYTE EMO_Press;
	BYTE BUMPER_Press;
	BYTE FAN_Fail;
	BYTE HOIST_Belt_Detect1;	// Reserved48;



	// 10개(No mapping)
	BYTE HOIST_Belt_Detect2;	// Reserved48;
	BYTE TP_Enable;				// 미사용;
	BYTE HANDTP_Connect;
	BYTE FOUB_Cover_Detect;
	BYTE STOP_Driving_Front;
	BYTE STOP_Driving_PreCheck;
	BYTE STOP_Trans_Front;
	BYTE STOP_Trans_Rear;
	BYTE BCR_Driving_OK;
	BYTE BCR_Driving_NG;



	// 10개(No mapping)
	BYTE BCR_Trans_OK;
	BYTE BCR_Trans_NG;
	BYTE Reserved_1;
	BYTE Reserved_2;
	BYTE Reserved_3;
	BYTE SMPS_Short;
	BYTE CID_Link_Compt_IN;
	BYTE CID_Interlock_IN;
	BYTE CID_Completion_ACK;
	BYTE CID_Status_IN;


	// 10개(No mapping)
	BYTE CID_Reserved_IN1;
	BYTE CID_Reserved_IN2;
	BYTE Rotate_0_Sensor;
	BYTE Rotate_90_Sensor;
	BYTE Rotate_180_Sensor;
	BYTE Rotate_270_Sensor; //미사용
	BYTE HOIST_Home;
	BYTE HOIST_Home_2;     // 미사용
	BYTE SHUTTER_F_Middle_Close;
	BYTE SHUTTER_R_Middle_Close;


	// 10개(No mapping)
	BYTE POD_EUV_1;
	BYTE POD_EUV_2;
	BYTE SPARE_5;
	BYTE SPARE_6;
	BYTE SPARE_7;
	BYTE SPARE_8;
	BYTE SPARE_9;
	BYTE SPARE_10;
	BYTE SPARE_11;
	BYTE SPARE_12;

    // 6개(No mapping)  == 96
	BYTE SPARE_13;
	BYTE SPARE_14;
	BYTE SPARE_15;
	BYTE SPARE_16;
	BYTE SPARE_17;
	BYTE SPARE_18;

}sSTANDARDINPUT_V85;
#elif(OHT_NAME == OHT_NAME_STD_V81S)
typedef struct sSTANDARDINPUT_V81S_
{
	// 10개(No mapping)
	BYTE HAND_Inposition;
	BYTE HAND_Alarm;
	BYTE HAND_Right_Grip;
	BYTE HAND_Left_Grip;
	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Push;
	BYTE HOIST_Home;		// 미사용
	BYTE HNAD_ES;               // 미사용
	BYTE HAND_PIO_Go;


	// 10개(No mapping)
	BYTE EQPIO_Load_REQ;
	BYTE EQPIO_Unload_REQ;
	BYTE EQPIO_Ready;
	BYTE EQPIO_HO_AVBL;
	BYTE EQPIO_ES;
	BYTE EQPIO_Left_Go;   		//	BYTE EQPIO_Right_Go;미사용
	BYTE STEER_F_Left;
	BYTE STEER_R_Left;
	BYTE STEER_F_Right;
	BYTE STEER_R_Right;

	// 10개(No mapping)
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;
	BYTE SHUTTER_F_Alarm;
	BYTE SHUTTER_R_Alarm;
	BYTE SHUTTER_F_Inposition;
	BYTE SHUTTER_R_Inposition;
	BYTE OBSTACLE_Input_Extend;     	// 미사용
	BYTE OBSTACLE_Long;

	// 10개(No mapping)
	BYTE OBSTACLE_Middle;
	BYTE OBSTACLE_Short;
	BYTE OBSTACLE_Fail;
    BYTE OHTDETECT_Input_Extend;   	// 미사용
	BYTE OHTDETECT_Long;
	BYTE OHTDETECT_Middle;
	BYTE OHTDETECT_Short;
	BYTE OHTDETECT_Fail;
	BYTE LOOKDOWN_Detect;
	BYTE LOOKDOWN_Error;



	// 10개(No mapping)
	BYTE SHIFT_Home;
	BYTE STB_Detect_Left;
	BYTE STB_Detect_Right;
	BYTE UTB_Detect;				// 미사용
	BYTE FOUP_Detect;
	BYTE OSCILLATION;
	BYTE EMO_Press;
	BYTE BUMPER_Press;
	BYTE FAN_Fail;
	BYTE HOIST_Belt_Detect1;	// Reserved48;



	// 10개(No mapping)
	BYTE HOIST_Belt_Detect2;	// Reserved48;
	BYTE TP_Enable;				// 미사용;
	BYTE HANDTP_Connect;
	BYTE FOUP_Cover_Open_Detect;
	BYTE STOP_Driving_Front;
	BYTE STOP_Driving_PreCheck;
	BYTE STOP_Trans_Front;
	BYTE STOP_Trans_Rear;
	BYTE BCR_Driving_OK;
	BYTE BCR_Driving_NG;



	// 10개(No mapping)
	BYTE BCR_Trans_OK;
	BYTE BCR_Trans_NG;
	BYTE Reserved_1;
	BYTE Reserved_2;
	BYTE Reserved_3;
	BYTE SMPS_Short;
	BYTE CID_Link_Compt_IN;
	BYTE CID_Interlock_IN;
	BYTE CID_Completion_ACK;
	BYTE CID_Status_IN;


	// 10개(No mapping)
	BYTE CID_Reserved_IN1;
	BYTE CID_Reserved_IN2;
	BYTE Rotate_0_Sensor;
	BYTE Rotate_90_Sensor;
	BYTE Rotate_180_Sensor;
	BYTE Rotate_270_Sensor; //미사용
	BYTE HNAD_Reserved_1;
	BYTE HOIST_Home_2;     // 미사용
	BYTE SHUTTER_F_Middle_Close;
	BYTE SHUTTER_R_Middle_Close;


	// 10개(No mapping)
	BYTE POD_EUV_1;
	BYTE POD_EUV_2;
	BYTE SPARE_5;
	BYTE SPARE_6;
	BYTE SPARE_7;
	BYTE SPARE_8;
	BYTE SPARE_9;
	BYTE SPARE_10;
	BYTE SPARE_11;
	BYTE SPARE_12;

    // 6개(No mapping)  == 96
	BYTE SPARE_13;
	BYTE SPARE_14;
	BYTE SPARE_15;
	BYTE SPARE_16;
	BYTE SPARE_17;
	BYTE SPARE_18;

}sSTANDARDINPUT_V81S;
#else
typedef struct sINPUT_
{
	BYTE HAND_Inposition;
	BYTE HAND_Alarm;
	BYTE HAND_Right_Grip;
    BYTE HAND_Left_Grip;
	BYTE HAND_Close;
	BYTE HAND_Open;
    BYTE HAND_Push;
    BYTE HOIST_Home;

    BYTE HAND_PIO_Go;
    BYTE EQPIO_Load_REQ;
    BYTE EQPIO_Unload_REQ;
    BYTE EQPIO_Ready;
    BYTE EQPIO_HO_AVBL;
    BYTE EQPIO_ES;
    BYTE EQPIO_Right_Go;
    BYTE EQPIO_Left_Go;

	BYTE STEER_F_Left;
    BYTE STEER_R_Left;
    BYTE STEER_F_Right;
    BYTE STEER_R_Right;
    BYTE SHUTTER_F_Close;
    BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
    BYTE SHUTTER_R_Open;

    BYTE SHUTTER_F_Alarm;
    BYTE SHUTTER_R_Alarm;
    BYTE SHUTTER_F_Inposition;
    BYTE SHUTTER_R_Inposition;
    BYTE OBSTACLE_Long;
    BYTE OBSTACLE_Middle;
    BYTE OBSTACLE_Short;
    BYTE OBSTACLE_Fail;

    BYTE OHTDETECT_Long;
    BYTE OHTDETECT_Middle;
    BYTE OHTDETECT_Short;
	BYTE OHTDETECT_Fail;
    BYTE LOOKDOWN_Detect;
	BYTE LOOKDOWN_Error;
    BYTE SHIFT_Home;
    BYTE STB_Detect_Left;

    BYTE STB_Detect_Right;
    BYTE UTB_Detect;
    BYTE FOUP_Detect;
    BYTE OSCILLATION;
    BYTE EMO_Press;
    BYTE BUMPER_Press;
    BYTE FAN_Fail;
	BYTE STB_OHT_Detect_Left;

	BYTE STB_OHT_Detect_Right;
    BYTE HANDTP_Connect;
    BYTE STOP_Driving_PreCheck;
    BYTE MC_Status;
	BYTE STOP_Driving_Front;
    BYTE STOP_Trans_Front;
    BYTE STOP_Trans_Rear;
	BYTE BCR_Driving_OK;

    BYTE BCR_Driving_NG;
    BYTE BCR_Trans_OK;
    BYTE BCR_Trans_NG;
    BYTE OHT_COMM_CD;
	BYTE RailSupport_Home;
	BYTE Railsupport_Work;
	BYTE MAC_Detect;
	BYTE SMPS_Short;
} sINPUT;
#endif

/**
@brief   HWCommon에서 사용하는 Output 64개
@author  LSJ
@date    2012.10.23
*/
#if ( (OHT_NAME == OHT_NAME_STD_V80))
typedef struct sOUTPUTR01_
{
	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Origin;
	BYTE HAND_Reset;
	BYTE HAND_Free;
	BYTE HAND_Reserved6;
	BYTE HAND_Reserved7;
	BYTE HAND_Reserved8;
	BYTE HAND_PIO_Select;
	BYTE EQPIO_Valid;
	BYTE EQPIO_CS_0;

    //11 ~ 20
	BYTE EQPIO_CS_1;
	BYTE EQPIO_CS_2;
	BYTE EQPIO_CS_3;
	BYTE EQPIO_TR_Req;
	BYTE EQPIO_Busy;
	BYTE EQPIO_Complete;
	BYTE EQPIO_Left_Select;
	BYTE EQPIO_Right_Select;
	BYTE STEER_F_TRQ_0;
	BYTE STEER_F_TRQ_1;

	//21 ~ 30
	BYTE STEER_R_TRQ_0;
	BYTE STEER_R_TRQ_1;
	BYTE STEER_F_Dir;
	BYTE STEER_R_Dir;
	BYTE SHUTTER_F_Reset;
	BYTE SHUTTER_R_Reset;
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;

	//31 ~ 40
	BYTE SHUTTER_F_Origin;
	BYTE SHUTTER_R_Origin;
	BYTE SHUTTER_F_Free;
	BYTE SHUTTER_R_Free;
	BYTE OBSTACLE_Area1_Sel;
	BYTE OBSTACLE_Area2_Sel;
	BYTE OBSTACLE_Area3_Sel;
	BYTE OBSTACLE_Area4_Sel;
	BYTE OBSTACLE_Power_Reset;
	BYTE OHTDETECT_Area1_Sel;

	//41 ~ 50
	BYTE OHTDETECT_Area2_Sel;
	BYTE OHTDETECT_Area3_Sel;
	BYTE OHTDETECT_Area4_Sel;
	BYTE OHTDETECT_Power_Reset;
	BYTE LOOKDOWN_Area1_Sel;
	BYTE LOOKDOWN_Area2_Sel;
	BYTE LOOKDOWN_Area3_Sel;
	BYTE LOOKDOWN_Area4_Sel;
	BYTE LOOKDOWN_Power_Reset;
	BYTE LAMP_Red;

	BYTE LAMP_Yellow_Right;
	BYTE LAMP_Green;
	BYTE HAND_Power_Reset;
	BYTE BRAKE_Driving_Release;
	BYTE Bumper_Clear;
	BYTE SAFETY_Reset;
	BYTE HOIST_Ext_Brake_Release;
	BYTE QRR_POWER;
	BYTE LAMP_Yellow_Left;
	BYTE Reserved_60;
	BYTE Reserved_61;
	BYTE Reserved_62;
	BYTE Reserved_63;

	//TG 추가
//	BYTE CID_Left_Select_OUT;
//	BYTE CID_Right_Select_OUT;
	BYTE CID_Left_Select_ON;
	BYTE CID_Right_Select_ON;
	BYTE CID_Occupancy;
	BYTE CID_Pass_Completion;
	BYTE CID_Abnormal;
	BYTE CID_Operation_Mode;
	BYTE CID_Common_Tag; //동일 lane link 금지
	BYTE CID_Reserved_OUT2;
	BYTE Outtrigger_F_Start_OUT;
	BYTE Outtrigger_F_DIR_OUT;
	BYTE Outtrigger_F_Brake_OUT;
	BYTE Outtrigger_F_Speed_OUT;
	BYTE Outtrigger_Reserved_76;
	BYTE Outtrigger_Reserved_77;
	BYTE Outtrigger_Reserved_78;
	BYTE Outtrigger_Reserved_79;
} sOUTPUTR01;
#elif ((OHT_NAME == OHT_NAME_STD_V81))
typedef struct sOUTPUTV81_
{
	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Origin;
	BYTE HAND_Reset;
	BYTE HAND_Free;
	BYTE HAND_Reserved6;
	BYTE HAND_Reserved7;
	BYTE HAND_Reserved8;
	BYTE HAND_PIO_Select;
	BYTE EQPIO_Valid;
	BYTE EQPIO_CS_0;

    //11 ~ 20
	BYTE EQPIO_CS_1;
	BYTE EQPIO_CS_2;
	BYTE EQPIO_CS_3;
	BYTE EQPIO_TR_Req;
	BYTE EQPIO_Busy;
	BYTE EQPIO_Complete;
	BYTE EQPIO_Left_Select;
	BYTE EQPIO_Right_Select;
	BYTE STEER_F_TRQ_0;
	BYTE STEER_F_TRQ_1;

	//21 ~ 30
	BYTE STEER_R_TRQ_0;
	BYTE STEER_R_TRQ_1;
	BYTE STEER_F_Dir;
	BYTE STEER_R_Dir;
	BYTE SHUTTER_F_Reset;
	BYTE SHUTTER_R_Reset;
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;

	//31 ~ 40
	BYTE SHUTTER_F_Origin;
	BYTE SHUTTER_R_Origin;
	BYTE SHUTTER_F_Free;
	BYTE SHUTTER_R_Free;
	BYTE OBSTACLE_Area1_Sel;
	BYTE OBSTACLE_Area2_Sel;
	BYTE OBSTACLE_Area3_Sel;
	BYTE OBSTACLE_Area4_Sel;
	BYTE OBSTACLE_Power_Reset;
	BYTE OHTDETECT_Area1_Sel;

	//41 ~ 50
	BYTE OHTDETECT_Area2_Sel;
	BYTE OHTDETECT_Area3_Sel;
	BYTE OHTDETECT_Area4_Sel;
	BYTE OHTDETECT_Power_Reset;
	BYTE LOOKDOWN_Area1_Sel;
	BYTE LOOKDOWN_Area2_Sel;
	BYTE LOOKDOWN_Area3_Sel;
	BYTE LOOKDOWN_Area4_Sel;
	BYTE LOOKDOWN_Power_Reset;
	BYTE LAMP_Red;

	BYTE LAMP_Yellow_Right;
	BYTE LAMP_Green;
	BYTE HAND_Power_Reset;
	BYTE BRAKE_Driving_Release;
	BYTE Bumper_Clear;
	BYTE SAFETY_Reset;
	BYTE HOIST_Ext_Brake_Release;
	BYTE QRR_POWER;
	BYTE LAMP_Yellow_Left;
	BYTE Reserved_60;
	BYTE Reserved_61;
	BYTE Reserved_62;
	BYTE Reserved_63;

	//TG 추가
//	BYTE CID_Left_Select_OUT;
//	BYTE CID_Right_Select_OUT;
	BYTE CID_Left_Select_ON;
	BYTE CID_Right_Select_ON;
	BYTE CID_Occupancy;
	BYTE CID_Pass_Completion;
	BYTE CID_Abnormal;
	BYTE CID_Operation_Mode;
	BYTE CID_Common_Tag; //동일 lane link 금지
	BYTE CID_Reserved_OUT2;
	BYTE Reserved_72;
	BYTE Reserved_73;
	BYTE Reserved_74;
	BYTE Reserved_75;
	BYTE Reserved_76;
	BYTE Reserved_77;
	BYTE Reserved_78;
	BYTE Reserved_79;
} sOUTPUTV81;
#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))

typedef struct sRETICLEOUTPUT_
{
	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Origin;
	BYTE HAND_Reset;
	BYTE HAND_Free;
	BYTE HAND_Reserved6;
	BYTE HAND_Reserved7;
	BYTE HAND_Reserved8;
	BYTE HAND_PIO_Select;
	BYTE EQPIO_Valid;
	BYTE EQPIO_CS_0;
	BYTE EQPIO_CS_1;
	BYTE EQPIO_CS_2;
	BYTE EQPIO_CS_3;
	BYTE EQPIO_TR_Req;
	BYTE EQPIO_Busy;
	BYTE EQPIO_Complete;
	BYTE EQPIO_Left_Select;
	BYTE EQPIO_Right_Select;
	BYTE STEER_F_TRQ_0;
	BYTE STEER_F_TRQ_1;
	BYTE STEER_R_TRQ_0;
	BYTE STEER_R_TRQ_1;
	BYTE STEER_F_Dir;
	BYTE STEER_R_Dir;
	BYTE SHUTTER_F_Reset;
	BYTE SHUTTER_R_Reset;
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;
	BYTE SHUTTER_F_Origin;

	BYTE SHUTTER_R_Origin;
	BYTE SHUTTER_F_Free;
	BYTE SHUTTER_R_Free;
	BYTE OBSTACLE_Area1_Sel;
	BYTE OBSTACLE_Area2_Sel;
	BYTE OBSTACLE_Area3_Sel;
	BYTE OBSTACLE_Area4_Sel;
	BYTE OBSTACLE_Power_Reset;
	BYTE OHTDETECT_Area1_Sel;
	BYTE OHTDETECT_Area2_Sel;
	BYTE OHTDETECT_Area3_Sel;
	BYTE OHTDETECT_Area4_Sel;
	BYTE OHTDETECT_Power_Reset;
	BYTE LOOKDOWN_Area1_Sel;
	BYTE LOOKDOWN_Area2_Sel;
	BYTE LOOKDOWN_Area3_Sel;
	BYTE LOOKDOWN_Area4_Sel;
	BYTE LOOKDOWN_Power_Reset;
	BYTE LAMP_Red;
	BYTE LAMP_Yellow_Right;
	BYTE LAMP_Green;
	BYTE HAND_Power_Reset;
	BYTE BRAKE_Driving_Release;
	BYTE Bumper_Clear;
	BYTE SAFETY_Reset;
	BYTE HOIST_Ext_Brake_Release;
	BYTE QRR_Power;
	BYTE LAMP_Yellow_Left;
	BYTE Reserved60;
	BYTE Reserved61;
	BYTE Reserved62;
	BYTE Reserved63;
	//TG 추가
	BYTE CID_Left_Select_ON;
	BYTE CID_Right_Select_ON;
	BYTE CID_Occupancy;
	BYTE CID_Pass_Completion;
	BYTE CID_Abnormal;
	BYTE CID_Operation_Mode;
	BYTE CID_Common_Tag; //동일 lane link 금지
	BYTE CID_Reserved_OUT2;
	BYTE Outtrigger_Reserved73;
	BYTE Outtrigger_Reserved74;
	BYTE Outtrigger_Reserved75;
	BYTE Outtrigger_Reserved76;
	BYTE Outtrigger_Reserved77;
	BYTE Outtrigger_Reserved78;
	BYTE Outtrigger_Reserved79;
	BYTE Outtrigger_Reserved80;
} sRETICLEOUTPUT;
	
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	
typedef struct sRETICLEOUTPUT_
{
	// 0 - 15
	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Origin;
	BYTE HAND_Reset;

	BYTE HAND_Free;
	BYTE HAND_Reserved6;
	BYTE HAND_Reserved7;
	BYTE HAND_Reserved8;
			
	BYTE HAND_PIO_Select;
	BYTE EQPIO_Valid;
	BYTE EQPIO_CS_0;
	BYTE EQPIO_CS_1;
			
	BYTE EQPIO_CS_2;
	BYTE EQPIO_CS_3;
	BYTE EQPIO_TR_Req;
	BYTE EQPIO_Busy;

	// 16 - 31
	BYTE EQPIO_Complete;
	BYTE EQPIO_Left_Select;
	BYTE EQPIO_Right_Select;
	BYTE STEER_F_TRQ_0;

	BYTE STEER_F_TRQ_1;
	BYTE STEER_R_TRQ_0;
	BYTE STEER_R_TRQ_1;
	BYTE STEER_F_Dir;
			
	BYTE STEER_R_Dir;
	BYTE SHUTTER_F_Reset;
	BYTE SHUTTER_R_Reset;
	BYTE SHUTTER_F_Close;
			
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;
	BYTE SHUTTER_F_Origin;

	// 32 - 47
	BYTE SHUTTER_R_Origin;
	BYTE SHUTTER_F_Free;
	BYTE SHUTTER_R_Free;
	BYTE OBSTACLE_Area1_Sel;
			
	BYTE OBSTACLE_Area2_Sel;
	BYTE OBSTACLE_Area3_Sel;
	BYTE OBSTACLE_Area4_Sel;
	BYTE OBSTACLE_Power_Reset;
			
	BYTE OHTDETECT_Area1_Sel;
	BYTE OHTDETECT_Area2_Sel;
	BYTE OHTDETECT_Area3_Sel;
	BYTE OHTDETECT_Area4_Sel;
			
	BYTE OHTDETECT_Power_Reset;
	BYTE LOOKDOWN_Area1_Sel;
	BYTE LOOKDOWN_Area2_Sel;
	BYTE LOOKDOWN_Area3_Sel;

	// 48 - 63
	BYTE LOOKDOWN_Area4_Sel;
	BYTE LOOKDOWN_Power_Reset;
	BYTE LAMP_Red;
	BYTE LAMP_Yellow_Right;
	BYTE LAMP_Green;
	BYTE HAND_Power_Reset;
	BYTE BRAKE_Driving_Release;
	BYTE Bumper_Clear;
	BYTE SAFETY_Reset;
	BYTE HOIST_Ext_Brake_Release;
	BYTE QRR_Power;
	BYTE LAMP_Yellow_Left;
	BYTE Reserved60;
	BYTE Reserved61;
	BYTE Reserved62;
	BYTE Reserved63;


	// 64 - 79
	BYTE CID_Left_Select_ON;
	BYTE CID_Right_Select_ON;
	BYTE CID_Occupancy;
	BYTE CID_Pass_Completion;
			
	BYTE CID_Abnormal;
	BYTE CID_Operation_Mode;
	BYTE CID_Common_Tag; //동일 lane link 금지
	BYTE CID_Reserved_OUT2;

	BYTE SPARE_6;
	BYTE SPARE_7;
	BYTE SPARE_8;
	BYTE SHUTTER_F_Middle_Close;
			
	BYTE SHUTTER_R_Middle_Close;
	BYTE SPARE_9;
	BYTE SPARE_10;
	BYTE SPARE_11;
			
	// 80 - 95
	BYTE SPARE_12;
	BYTE SPARE_13;
	BYTE SPARE_14;
	BYTE SPARE_15;
			
	BYTE SPARE_16;
	BYTE SPARE_17;
	BYTE SPARE_18;
	BYTE SPARE_19;
			
	BYTE SPARE_20;
	BYTE SPARE_21;
	BYTE SPARE_22;
	BYTE SPARE_23;

	BYTE SPARE_24;
	BYTE SPARE_25;
	BYTE SPARE_26;
	BYTE SPARE_27;
} sRETICLEOUTPUT;

#elif(OHT_NAME == OHT_NAME_STD_V85)
typedef struct sSTANDARDOUTPUT_V85_
{

	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Origin;
	BYTE HAND_Reset;
	BYTE HAND_Free;
	BYTE HAND_Reserved6;
	BYTE HAND_Reserved7;
	BYTE HAND_Reserved8;
    BYTE HAND_HandTrigger;    //미사용
	BYTE HAND_PIO_Select;
	BYTE EQPIO_Valid;
	BYTE EQPIO_CS_0;
	BYTE EQPIO_CS_1;

	BYTE EQPIO_CS_2;        //미사용
	BYTE EQPIO_CS_3;        //미사용
	BYTE EQPIO_TR_Req;
	BYTE EQPIO_Busy;
	BYTE EQPIO_Complete;
	BYTE EQPIO_Left_Select;      //	BYTE EQPIO_Right_Select; 미사용


	BYTE STEER_F_TRQ_0;
	BYTE STEER_F_TRQ_1;
	BYTE STEER_R_TRQ_0;
	BYTE STEER_R_TRQ_1;
	BYTE STEER_F_Dir;
	BYTE STEER_R_Dir;

	BYTE SHUTTER_F_Reset;
	BYTE SHUTTER_R_Reset;
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;
	BYTE SHUTTER_F_Origin;
	BYTE SHUTTER_R_Origin;
	BYTE SHUTTER_F_Free;
	BYTE SHUTTER_R_Free;

	BYTE OBSTACLE_Area1_Sel;
	BYTE OBSTACLE_Area2_Sel;
	BYTE OBSTACLE_Area3_Sel;
	BYTE OBSTACLE_Area4_Sel;
	BYTE OBSTACLE_Area5_Sel;     //미사용
	BYTE OBSTACLE_Power_Reset;

	BYTE OHTDETECT_Area1_Sel;
	BYTE OHTDETECT_Area2_Sel;
	BYTE OHTDETECT_Area3_Sel;
	BYTE OHTDETECT_Area4_Sel;
    BYTE OHTDETECT_Area5_Sel;    //미사용
	BYTE OHTDETECT_Power_Reset;

	BYTE LOOKDOWN_Area1_Sel;
	BYTE LOOKDOWN_Area2_Sel;
	BYTE LOOKDOWN_Area3_Sel;
	BYTE LOOKDOWN_Area4_Sel;
	BYTE LOOKDOWN_Power_Reset;

	BYTE LAMP_Red;
	BYTE LAMP_Yellow_Right;
	BYTE LAMP_Yellow_Left;
	BYTE LAMP_Green;
	BYTE HAND_Power_Reset;
	BYTE BRAKE_Driving_Release;
	BYTE Bumper_Clear;
	BYTE SAFETY_Reset;
	BYTE HOIST_Ext_Brake_Release;
	BYTE QRR_Power;

	BYTE Reserved60;
	BYTE Reserved61;
	BYTE Reserved62;
	BYTE Reserved63;


	// 64 - 79
	BYTE CID_Left_Select_ON;
	BYTE CID_Right_Select_ON;
	BYTE CID_Occupancy;
	BYTE CID_Pass_Completion;
	BYTE CID_Abnormal;
	BYTE CID_Operation_Mode;
	BYTE CID_Common_Tag; 		//동일 lane link 금지
	BYTE CID_Reserved_OUT2;

	BYTE SPARE_6;
	BYTE SPARE_7;
	BYTE SHUTTER_F_Middle_Close;
	BYTE SHUTTER_R_Middle_Close;
	BYTE SPARE_8;
	BYTE SPARE_9;
	BYTE SPARE_10;
	BYTE SPARE_11;

	// 80 - 95
	BYTE SPARE_12;
	BYTE SPARE_13;
	BYTE SPARE_14;
	BYTE SPARE_15;

	BYTE SPARE_16;
	BYTE SPARE_17;
	BYTE SPARE_18;
	BYTE SPARE_19;

	BYTE SPARE_20;
	BYTE SPARE_21;
	BYTE SPARE_22;
	BYTE SPARE_23;

	BYTE SPARE_24;
	BYTE SPARE_25;
	BYTE SPARE_26;
	BYTE SPARE_27;
} sSTANDARDOUTPUT_V85;
#elif(OHT_NAME == OHT_NAME_STD_V81S)
typedef struct sSTANDARDOUTPUT_V81S_
{

	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Origin;
	BYTE HAND_Reset;
	BYTE HAND_Free;
	BYTE HAND_Reserved6;
	BYTE HAND_Reserved7;
	BYTE HAND_Reserved8;
    BYTE HAND_HandTrigger;    //미사용
	BYTE HAND_PIO_Select;
	BYTE EQPIO_Valid;
	BYTE EQPIO_CS_0;
	BYTE EQPIO_CS_1;

	BYTE EQPIO_CS_2;        //미사용
	BYTE EQPIO_CS_3;        //미사용
	BYTE EQPIO_TR_Req;
	BYTE EQPIO_Busy;
	BYTE EQPIO_Complete;
	BYTE EQPIO_Left_Select;      //	BYTE EQPIO_Right_Select; 미사용


	BYTE STEER_F_TRQ_0;
	BYTE STEER_F_TRQ_1;
	BYTE STEER_R_TRQ_0;
	BYTE STEER_R_TRQ_1;
	BYTE STEER_F_Dir;
	BYTE STEER_R_Dir;

	BYTE SHUTTER_F_Reset;
	BYTE SHUTTER_R_Reset;
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;
	BYTE SHUTTER_F_Origin;
	BYTE SHUTTER_R_Origin;
	BYTE SHUTTER_F_Free;
	BYTE SHUTTER_R_Free;

	BYTE OBSTACLE_Area1_Sel;
	BYTE OBSTACLE_Area2_Sel;
	BYTE OBSTACLE_Area3_Sel;
	BYTE OBSTACLE_Area4_Sel;
	BYTE OBSTACLE_Area5_Sel;     //미사용
	BYTE OBSTACLE_Power_Reset;

	BYTE OHTDETECT_Area1_Sel;
	BYTE OHTDETECT_Area2_Sel;
	BYTE OHTDETECT_Area3_Sel;
	BYTE OHTDETECT_Area4_Sel;
    BYTE OHTDETECT_Area5_Sel;    //미사용
	BYTE OHTDETECT_Power_Reset;

	BYTE LOOKDOWN_Area1_Sel;
	BYTE LOOKDOWN_Area2_Sel;
	BYTE LOOKDOWN_Area3_Sel;
	BYTE LOOKDOWN_Area4_Sel;
	BYTE LOOKDOWN_Power_Reset;

	BYTE LAMP_Red;
	BYTE LAMP_Yellow_Right;
	BYTE LAMP_Yellow_Left;
	BYTE LAMP_Green;
	BYTE HAND_Power_Reset;
	BYTE BRAKE_Driving_Release;
	BYTE Bumper_Clear;
	BYTE SAFETY_Reset;
	BYTE HOIST_Ext_Brake_Release;
	BYTE QRR_Power;

	BYTE Reserved60;
	BYTE Reserved61;
	BYTE Reserved62;
	BYTE Reserved63;


	// 64 - 79
	BYTE CID_Left_Select_ON;
	BYTE CID_Right_Select_ON;
	BYTE CID_Occupancy;
	BYTE CID_Pass_Completion;
	BYTE CID_Abnormal;
	BYTE CID_Operation_Mode;
	BYTE CID_Common_Tag; 		//동일 lane link 금지
	BYTE CID_Reserved_OUT2;

	BYTE SPARE_6;
	BYTE SPARE_7;
	BYTE SHUTTER_F_Middle_Close;
	BYTE SHUTTER_R_Middle_Close;
	BYTE SPARE_8;
	BYTE SPARE_9;
	BYTE SPARE_10;
	BYTE SPARE_11;

	// 80 - 95
	BYTE SPARE_12;
	BYTE SPARE_13;
	BYTE SPARE_14;
	BYTE SPARE_15;

	BYTE SPARE_16;
	BYTE SPARE_17;
	BYTE SPARE_18;
	BYTE SPARE_19;

	BYTE SPARE_20;
	BYTE SPARE_21;
	BYTE SPARE_22;
	BYTE SPARE_23;

	BYTE SPARE_24;
	BYTE SPARE_25;
	BYTE SPARE_26;
	BYTE SPARE_27;
} sSTANDARDOUTPUT_V81S;

#else
typedef struct sOUTPUT_
{
	BYTE HAND_Close;
	BYTE HAND_Open;
	BYTE HAND_Origin;
	BYTE HAND_Reset;
	BYTE HAND_Free;
	BYTE HAND_Reserved6;
	BYTE HAND_Reserved7;
	BYTE HAND_Reserved8;

	BYTE HAND_PIO_Select;
	BYTE EQPIO_Valid;
	BYTE EQPIO_CS_0;
	BYTE EQPIO_CS_1;
	BYTE EQPIO_TR_Req;
	BYTE EQPIO_Busy;
	BYTE EQPIO_Complete;
	BYTE EQPIO_Right_Select;

	BYTE EQPIO_Left_Select;
	BYTE STEER_F_TRQ_0;
	BYTE STEER_F_TRQ_1;
	BYTE STEER_R_TRQ_0;
	BYTE STEER_R_TRQ_1;
	BYTE STEER_F_Dir;
	BYTE STEER_R_Dir;
	BYTE SHUTTER_F_Reset;

	BYTE SHUTTER_R_Reset;
	BYTE SHUTTER_F_Close;
	BYTE SHUTTER_R_Close;
	BYTE SHUTTER_F_Open;
	BYTE SHUTTER_R_Open;
	BYTE SHUTTER_F_Origin;
	BYTE SHUTTER_R_Origin;
	BYTE SHUTTER_F_Free;

	BYTE SHUTTER_R_Free;
	BYTE OBSTACLE_Area1_Sel;
	BYTE OBSTACLE_Area2_Sel;
	BYTE OBSTACLE_Area3_Sel;
	BYTE OBSTACLE_Area4_Sel;
	BYTE OBSTACLE_Power_Reset;
	BYTE OHTDETECT_Area1_Sel;
	BYTE OHTDETECT_Area2_Sel;

	BYTE OHTDETECT_Area3_Sel;
	BYTE OHTDETECT_Area4_Sel;
	BYTE OHTDETECT_Power_Reset;
	BYTE LOOKDOWN_Area1_Sel;
	BYTE LOOKDOWN_Area2_Sel;
	BYTE LOOKDOWN_Area3_Sel;
	BYTE LOOKDOWN_Area4_Sel;
	BYTE LOOKDOWN_Power_Reset;

	BYTE LAMP_Red;
	BYTE LAMP_Yellow;
	BYTE LAMP_Green;
	BYTE STB_Right_Work;
	BYTE STB_Left_Work;
	BYTE HAND_Power_Reset;
	BYTE BRAKE_Driving_Release;
	BYTE Bumper_Clear;

	BYTE MC_Control;
	BYTE LCD_Display_Reset;
	BYTE SERIAL_Select_1;
	BYTE SERIAL_Select_2;
	BYTE RailSupport_TRQ_0;
	BYTE RailSupport_TRQ_1;
	BYTE RailSupport_Dir;
	BYTE Reserved_1;
} sOUTPUT;
#endif


/**
@brief   SetBit함수에서 Outbit index로 사용하기 위한 Enum 변수
@author  LSJ
@date    2012.10.23
*/
#if((OHT_NAME == OHT_NAME_STD_V80))
enum OUTPUT
{
	OUT_HAND_CLOSE = 0,         // 0
	OUT_HAND_OPEN,
	OUT_HAND_ORIGIN,
	OUT_HAND_RESET,
	OUT_HAND_FREE,
	OUT_HAND_RESERVED6,         // 5
	OUT_HAND_RESERVED7,
	OUT_HAND_RESERVED8,
	OUT_HAND_PIO_SEL,
	OUT_EQPIO_VALID,
	OUT_EQPIO_CS_0,             // 10
	OUT_EQPIO_CS_1,
	OUT_EQPIO_CS_2,
	OUT_EQPIO_CS_3,
	OUT_EQPIO_TR_REQ,
	OUT_EQPIO_BUSY,
	OUT_EQPIO_COMPLETE,
	OUT_EQPIO_RIGHT_SEL,        // 15
	OUT_EQPIO_LEFT_SEL,
	OUT_STEER_F_TRQ_0,
	OUT_STEER_F_TRQ_1,
	OUT_STEER_R_TRQ_0,
	OUT_STEER_R_TRQ_1,          // 20
	OUT_STEER_F_DIR,
	OUT_STEER_R_DIR,
	OUT_SHUTTER_F_RESET,
	OUT_SHUTTER_R_RESET,
	OUT_SHUTTER_F_CLOSE,        // 25
	OUT_SHUTTER_R_CLOSE,
	OUT_SHUTTER_F_OPEN,
	OUT_SHUTTER_R_OPEN,
	OUT_SHUTTER_F_ORIGIN,
	OUT_SHUTTER_R_ORIGIN,       // 30
	OUT_SHUTTER_F_FREE,
	OUT_SHUTTER_R_FREE,
	OUT_OBSTACLE_AREA1_SEL,
	OUT_OBSTACLE_AREA2_SEL,
	OUT_OBSTACLE_AREA3_SEL,     // 35
	OUT_OBSTACLE_AREA4_SEL,
	OUT_OBSTACLE_POWER_RESET,
	OUT_OHTDETECT_AREA1_SEL,
	OUT_OHTDETECT_AREA2_SEL,
	OUT_OHTDETECT_AREA3_SEL,    // 40
	OUT_OHTDETECT_AREA4_SEL,
	OUT_OHTDETECT_POWER_RESET,
	OUT_LOOKDOWN_AREA1_SEL,
	OUT_LOOKDOWN_AREA2_SEL,
	OUT_LOOKDOWN_AREA3_SEL,     // 45
	OUT_LOOKDOWN_AREA4_SEL,
	OUT_LOOKDOWN_POWER_RESET,
	OUT_LAMP_RED,

	//51 ~ 63
	OUT_LAMP_YELLOW_RIGHT,
	OUT_LAMP_GREEN,
	OUT_HAND_POWER_RESET,
	OUT_BRAKE_DRIVING_RELEASE,
	OUT_BUMPER_CLEAR,
	OUT_SAFETY_RESET,
	OUT_HOIST_EXT_BRAKE_RELEASE,
	OUT_QRR_POWER,
	OUT_LAMP_YELLOW_LEFT,
	OUT_RESERVED_60,
	OUT_RESERVED_61,
	OUT_RESERVED_62,
	OUT_RESERVED_63,

	// CID 관련 Output
	OUT_CID_Left_Select_ON,    //64
	OUT_CID_Right_Select_ON,   //65
	OUT_CID_Occupancy,
	OUT_CID_Pass_Completion,	
	OUT_CID_Abnormal,
	OUT_CID_Operation_Mode,
	OUT_CID_Common_Tag,		//70
	OUT_CID_Reserved_OUT2,
	OUT_Outtrigger_F_Start_OUT,
	OUT_Outtrigger_F_DIR_OUT,
	OUT_Outtrigger_F_Brake_OUT,
	OUT_Outtrigger_F_Speed_OUT, //75
//	OUT_Outtrigger_R_Start_OUT,
	OUT_Outtrigger_RESERVED_76,
//	OUT_Outtrigger_R_DIR_OUT,
	OUT_Outtrigger_RESERVED_77,
//	OUT_Outtrigger_R_Brake_OUT,
	OUT_Outtrigger_RESERVED_78,
//	OUT_Outtrigger_R_Speed_OUT,
	OUT_Outtrigger_RESERVED_79
};
#elif(OHT_NAME == OHT_NAME_STD_V81)
enum OUTPUT
{
	OUT_HAND_CLOSE = 0,         // 0
	OUT_HAND_OPEN,
	OUT_HAND_ORIGIN,
	OUT_HAND_RESET,
	OUT_HAND_FREE,
	OUT_HAND_RESERVED6,         // 5
	OUT_HAND_RESERVED7,
	OUT_HAND_RESERVED8,
	OUT_HAND_PIO_SEL,
	OUT_EQPIO_VALID,
	OUT_EQPIO_CS_0,             // 10
	OUT_EQPIO_CS_1,
	OUT_EQPIO_CS_2,
	OUT_EQPIO_CS_3,
	OUT_EQPIO_TR_REQ,
	OUT_EQPIO_BUSY,
	OUT_EQPIO_COMPLETE,
	OUT_EQPIO_RIGHT_SEL,        // 15
	OUT_EQPIO_LEFT_SEL,
	OUT_STEER_F_TRQ_0,
	OUT_STEER_F_TRQ_1,
	OUT_STEER_R_TRQ_0,
	OUT_STEER_R_TRQ_1,          // 20
	OUT_STEER_F_DIR,
	OUT_STEER_R_DIR,
	OUT_SHUTTER_F_RESET,
	OUT_SHUTTER_R_RESET,
	OUT_SHUTTER_F_CLOSE,        // 25
	OUT_SHUTTER_R_CLOSE,
	OUT_SHUTTER_F_OPEN,
	OUT_SHUTTER_R_OPEN,
	OUT_SHUTTER_F_ORIGIN,
	OUT_SHUTTER_R_ORIGIN,       // 30
	OUT_SHUTTER_F_FREE,
	OUT_SHUTTER_R_FREE,
	OUT_OBSTACLE_AREA1_SEL,
	OUT_OBSTACLE_AREA2_SEL,
	OUT_OBSTACLE_AREA3_SEL,     // 35
	OUT_OBSTACLE_AREA4_SEL,
	OUT_OBSTACLE_POWER_RESET,
	OUT_OHTDETECT_AREA1_SEL,
	OUT_OHTDETECT_AREA2_SEL,
	OUT_OHTDETECT_AREA3_SEL,    // 40
	OUT_OHTDETECT_AREA4_SEL,
	OUT_OHTDETECT_POWER_RESET,
	OUT_LOOKDOWN_AREA1_SEL,
	OUT_LOOKDOWN_AREA2_SEL,
	OUT_LOOKDOWN_AREA3_SEL,     // 45
	OUT_LOOKDOWN_AREA4_SEL,
	OUT_LOOKDOWN_POWER_RESET,
	OUT_LAMP_RED,

	//51 ~ 63
	OUT_LAMP_YELLOW_RIGHT,
	OUT_LAMP_GREEN,
	OUT_HAND_POWER_RESET,
	OUT_BRAKE_DRIVING_RELEASE,
	OUT_BUMPER_CLEAR,
	OUT_SAFETY_RESET,
	OUT_HOIST_EXT_BRAKE_RELEASE,
	OUT_QRR_POWER,
	OUT_LAMP_YELLOW_LEFT,
	OUT_RESERVED_60,
	OUT_RESERVED_61,
	OUT_RESERVED_62,
	OUT_RESERVED_63,

	// CID 관련 Output
	OUT_CID_Left_Select_ON,    //64
	OUT_CID_Right_Select_ON,   //65
	OUT_CID_Occupancy,
	OUT_CID_Pass_Completion,
	OUT_CID_Abnormal,
	OUT_CID_Operation_Mode,
	OUT_CID_Common_Tag,		//70
	OUT_CID_Reserved_OUT2,
	OUT_SPARE_72,
	OUT_SPARE_73,
	OUT_SPARE_74,
	OUT_SPARE_75, //75
	OUT_SPARE_76,
	OUT_SPARE_77,
	OUT_SPARE_78,
	OUT_SPARE_79,
};
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
enum OUTPUT
{
	OUT_HAND_CLOSE = 0,         // 0
	OUT_HAND_OPEN,
	OUT_HAND_ORIGIN,
	OUT_HAND_RESET,
	OUT_HAND_FREE,
	OUT_HAND_RESERVED6,         // 5
	OUT_HAND_RESERVED7,
	OUT_HAND_RESERVED8,
	OUT_HAND_PIO_SEL,
	OUT_EQPIO_VALID,
	OUT_EQPIO_CS_0,             // 10
	OUT_EQPIO_CS_1,
	OUT_EQPIO_CS_2,
	OUT_EQPIO_CS_3,
	OUT_EQPIO_TR_REQ,
	OUT_EQPIO_BUSY,
	OUT_EQPIO_COMPLETE,
	OUT_EQPIO_RIGHT_SEL,        // 15
	OUT_EQPIO_LEFT_SEL,
	OUT_STEER_F_TRQ_0,
	OUT_STEER_F_TRQ_1,
	OUT_STEER_R_TRQ_0,
	OUT_STEER_R_TRQ_1,          // 20
	OUT_STEER_F_DIR,
	OUT_STEER_R_DIR,
	OUT_SHUTTER_F_RESET,
	OUT_SHUTTER_R_RESET,
	OUT_SHUTTER_F_CLOSE,        // 25
	OUT_SHUTTER_R_CLOSE,
	OUT_SHUTTER_F_OPEN,
	OUT_SHUTTER_R_OPEN,
	OUT_SHUTTER_F_ORIGIN,
	OUT_SHUTTER_R_ORIGIN,       // 30
	OUT_SHUTTER_F_FREE,
	OUT_SHUTTER_R_FREE,
	OUT_OBSTACLE_AREA1_SEL,
	OUT_OBSTACLE_AREA2_SEL,
	OUT_OBSTACLE_AREA3_SEL,     // 35
	OUT_OBSTACLE_AREA4_SEL,
	OUT_OBSTACLE_POWER_RESET,
	OUT_OHTDETECT_AREA1_SEL,
	OUT_OHTDETECT_AREA2_SEL,
	OUT_OHTDETECT_AREA3_SEL,    // 40
	OUT_OHTDETECT_AREA4_SEL,
	OUT_OHTDETECT_POWER_RESET,
	OUT_LOOKDOWN_AREA1_SEL,
	OUT_LOOKDOWN_AREA2_SEL,
	OUT_LOOKDOWN_AREA3_SEL,     // 45
	OUT_LOOKDOWN_AREA4_SEL,
	OUT_LOOKDOWN_POWER_RESET,
	OUT_LAMP_RED,
	OUT_LAMP_YELLOW_RIGHT,
	OUT_LAMP_GREEN,
	OUT_HAND_POWER_RESET,
	OUT_BRAKE_DRIVING_RELEASE,
	OUT_BUMPER_CLEAR,           // 55
	OUT_SAFETY_RESET,
	OUT_HOIST_EXT_BRAKE_RELEASE,
	OUT_QRR_POWER,
	OUT_LAMP_YELLOW_LEFT,
	OUT_RESERVED_60,
	OUT_RESERVED_61,
	OUT_RESERVED_62,
	OUT_RESERVED_63,
	// CID 관련 Output
	OUT_CID_Left_Select_ON,    //64
	OUT_CID_Right_Select_ON,   //65
	OUT_CID_Occupancy,
	OUT_CID_Pass_Completion,	
	OUT_CID_Abnormal,
	OUT_CID_Operation_Mode,
	OUT_CID_Common_Tag,		//70
	OUT_CID_Reserved_OUT2,
	OUT_Outtrigger_F_Start_OUT,
	OUT_Outtrigger_F_DIR_OUT,
	OUT_Outtrigger_F_Brake_OUT,
	OUT_Outtrigger_F_Speed_OUT, //75
	OUT_Outtrigger_R_Start_OUT,
	OUT_Outtrigger_R_DIR_OUT,
	OUT_Outtrigger_R_Brake_OUT,
	OUT_Outtrigger_R_Speed_OUT,
};
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
enum OUTPUT
{
	// 0 - 15
	OUT_HAND_CLOSE = 0, 		// 0
	OUT_HAND_OPEN,
	OUT_HAND_ORIGIN,
	OUT_HAND_RESET,
	
	OUT_HAND_FREE,
	OUT_HAND_RESERVED6, 		// 5
	OUT_HAND_RESERVED7,
	OUT_HAND_RESERVED8,
	
	OUT_HAND_PIO_SEL,
	OUT_EQPIO_VALID,
	OUT_EQPIO_CS_0, 			// 10
	OUT_EQPIO_CS_1,
	
	OUT_EQPIO_CS_2,
	OUT_EQPIO_CS_3,
	OUT_EQPIO_TR_REQ,
	OUT_EQPIO_BUSY,				// 15

	// 16 - 31
	OUT_EQPIO_COMPLETE,
    OUT_EQPIO_RIGHT_SEL,
	OUT_EQPIO_LEFT_SEL,
	OUT_STEER_F_TRQ_0,
	
	OUT_STEER_F_TRQ_1,			// 20
	OUT_STEER_R_TRQ_0,
	OUT_STEER_R_TRQ_1,
	OUT_STEER_F_DIR,
	
	OUT_STEER_R_DIR,
	OUT_SHUTTER_F_RESET,		// 25
	OUT_SHUTTER_R_RESET,
	OUT_SHUTTER_F_CLOSE,		
	
	OUT_SHUTTER_R_CLOSE,		
	OUT_SHUTTER_F_OPEN,
	OUT_SHUTTER_R_OPEN,
	OUT_SHUTTER_F_ORIGIN,

	// 32 - 47
	OUT_SHUTTER_R_ORIGIN,		
	OUT_SHUTTER_F_FREE,	
	OUT_SHUTTER_R_FREE,
	OUT_OBSTACLE_AREA1_SEL,
	
	OUT_OBSTACLE_AREA2_SEL,
	OUT_OBSTACLE_AREA3_SEL,	
	OUT_OBSTACLE_AREA4_SEL,		
	OUT_OBSTACLE_POWER_RESET,
	
	OUT_OHTDETECT_AREA1_SEL,
	OUT_OHTDETECT_AREA2_SEL,	
	OUT_OHTDETECT_AREA3_SEL,	
	OUT_OHTDETECT_AREA4_SEL,	
	
	OUT_OHTDETECT_POWER_RESET,
	OUT_LOOKDOWN_AREA1_SEL,
	OUT_LOOKDOWN_AREA2_SEL,
	OUT_LOOKDOWN_AREA3_SEL, 

	// 48 - 63	
	OUT_LOOKDOWN_AREA4_SEL,		// 50
	OUT_LOOKDOWN_POWER_RESET,	
	OUT_LAMP_RED,
	OUT_LAMP_YELLOW_RIGHT,
	
	OUT_LAMP_GREEN,
	OUT_HAND_POWER_RESET,	
	OUT_BRAKE_DRIVING_RELEASE,
	OUT_BUMPER_CLEAR,
	
	OUT_SAFETY_RESET,
	OUT_HOIST_EXT_BRAKE_RELEASE,
	OUT_QRR_POWER,
	OUT_LAMP_YELLOW_LEFT,

	OUT_SPARE2, 
	OUT_SPARE3,	
	OUT_SPARE4,	
	OUT_SPARE5,

	// 64 - 79
		// CID 관련 Output
	OUT_CID_Left_Select_ON,    
	OUT_CID_Right_Select_ON,	
	OUT_CID_Occupancy,
	OUT_CID_Pass_Completion,
	
	OUT_CID_Abnormal,			
	OUT_CID_Operation_Mode,	
	OUT_CID_Common_Tag, 	
	OUT_CID_Reserved_OUT2,

	OUT_SPARE_6,
	OUT_SPARE_7,
	OUT_SPARE_8,
	OUT_SHUTTER_F_MIDDLE_CLOSE, 
	
	OUT_SHUTTER_R_MIDDLE_CLOSE, 	
	OUT_SPARE_9,
	OUT_SPARE_10,
	OUT_SPARE_11,

	// 80 - 95
	/* 아웃트리거 관련 정의 꼭 확인해봐야 한다 */ 
	OUT_Outtrigger_F_Start_OUT,
	OUT_Outtrigger_F_DIR_OUT,	
	OUT_Outtrigger_F_Brake_OUT,
	OUT_Outtrigger_F_Speed_OUT,
	
	OUT_Outtrigger_R_Start_OUT,
	OUT_Outtrigger_R_DIR_OUT,	
	OUT_Outtrigger_R_Brake_OUT,	
	OUT_Outtrigger_R_Speed_OUT,

/*
	OUT_SPARE_12,				
	OUT_SPARE_13,
	OUT_SPARE_14,
	OUT_SPARE_15,
	
	OUT_SPARE_16,
	OUT_SPARE_17,				
	OUT_SPARE_18,
	OUT_SPARE_19,
*/
	OUT_SPARE_20,
	OUT_SPARE_21,
	OUT_SPARE_22,
	OUT_SPARE_23,

	OUT_SPARE_24,
	OUT_SPARE_25,
	OUT_SPARE_26,
	OUT_SPARE_27,
};
#elif(OHT_NAME == OHT_NAME_STD_V85)
enum OUTPUT
{
	// 0 - 15
	OUT_HAND_CLOSE = 0, 		// 0
	OUT_HAND_OPEN,
	OUT_HAND_ORIGIN,
	OUT_HAND_RESET,

	OUT_HAND_FREE,
	OUT_HAND_RESERVED6, 		// 5
	OUT_HAND_RESERVED7,
	OUT_HAND_RESERVED8,
	OUT_HAND_TRIGGER  ,   //추가

	OUT_HAND_PIO_SEL,
	OUT_EQPIO_VALID,
	OUT_EQPIO_CS_0, 			// 10
	OUT_EQPIO_CS_1,

	OUT_EQPIO_CS_2,
	OUT_EQPIO_CS_3,
	OUT_EQPIO_TR_REQ,
	OUT_EQPIO_BUSY,				// 15

	// 16 - 31
	OUT_EQPIO_COMPLETE,
	OUT_EQPIO_LEFT_SEL,
	OUT_STEER_F_TRQ_0,

	OUT_STEER_F_TRQ_1,			// 20
	OUT_STEER_R_TRQ_0,
	OUT_STEER_R_TRQ_1,
	OUT_STEER_F_DIR,

	OUT_STEER_R_DIR,
	OUT_SHUTTER_F_RESET,		// 25
	OUT_SHUTTER_R_RESET,
	OUT_SHUTTER_F_CLOSE,

	OUT_SHUTTER_R_CLOSE,
	OUT_SHUTTER_F_OPEN,
	OUT_SHUTTER_R_OPEN,
	OUT_SHUTTER_F_ORIGIN,

	// 32 - 47
	OUT_SHUTTER_R_ORIGIN,
	OUT_SHUTTER_F_FREE,
	OUT_SHUTTER_R_FREE,
	OUT_OBSTACLE_AREA1_SEL,

	OUT_OBSTACLE_AREA2_SEL,
	OUT_OBSTACLE_AREA3_SEL,
	OUT_OBSTACLE_AREA4_SEL,
	OUT_OBSTACLE_AREA5_SEL,
	OUT_OBSTACLE_POWER_RESET,

	OUT_OHTDETECT_AREA1_SEL,
	OUT_OHTDETECT_AREA2_SEL,
	OUT_OHTDETECT_AREA3_SEL,
	OUT_OHTDETECT_AREA4_SEL,
	OUT_OHTDETECT_AREA5_SEL,
	OUT_OHTDETECT_POWER_RESET,
	OUT_LOOKDOWN_AREA1_SEL,
	OUT_LOOKDOWN_AREA2_SEL,
	OUT_LOOKDOWN_AREA3_SEL,

	// 48 - 63
	OUT_LOOKDOWN_AREA4_SEL,		// 50
	OUT_LOOKDOWN_POWER_RESET,
	OUT_LAMP_RED,
	OUT_LAMP_YELLOW_RIGHT,
	OUT_LAMP_YELLOW_LEFT,

	OUT_LAMP_GREEN,
	OUT_HAND_POWER_RESET,
	OUT_BRAKE_DRIVING_RELEASE,
	OUT_BUMPER_CLEAR,            //범퍼 클리어

	OUT_SAFETY_RESET,
	OUT_HOIST_EXT_BRAKE_RELEASE,
	OUT_QRR_POWER,


	OUT_SPARE2,
	OUT_SPARE3,
	OUT_SPARE4,
	OUT_SPARE5,

	// 64 - 79
		// CID 관련 Output
	OUT_CID_Left_Select_ON,
	OUT_CID_Right_Select_ON,
	OUT_CID_Occupancy,
	OUT_CID_Pass_Completion,

	OUT_CID_Abnormal,
	OUT_CID_Operation_Mode,
	OUT_CID_Common_Tag,
	OUT_CID_Reserved_OUT2,

	OUT_SPARE_6,
	OUT_SPARE_7,
	OUT_SHUTTER_F_MIDDLE_CLOSE,

	OUT_SHUTTER_R_MIDDLE_CLOSE,
	OUT_SPARE_9,
	OUT_SPARE_10,
	OUT_SPARE_11,

	// 80 - 95
	OUT_SPARE_12,
	OUT_SPARE_13,
	OUT_SPARE_14,
	OUT_SPARE_15,

	OUT_SPARE_16,
	OUT_SPARE_17,
	OUT_SPARE_18,
	OUT_SPARE_19,
	OUT_SPARE_20,
	OUT_SPARE_21,
	OUT_SPARE_22,
	OUT_SPARE_23,

	OUT_SPARE_24,
	OUT_SPARE_25,
	OUT_SPARE_26,
	OUT_SPARE_27,
};
#elif(OHT_NAME == OHT_NAME_STD_V81S)
enum OUTPUT
{
	// 0 - 15
	OUT_HAND_CLOSE = 0, 		// 0
	OUT_HAND_OPEN,
	OUT_HAND_ORIGIN,
	OUT_HAND_RESET,

	OUT_HAND_FREE,
	OUT_HAND_RESERVED6, 		// 5
	OUT_HAND_RESERVED7,
	OUT_HAND_RESERVED8,
	OUT_HAND_TRIGGER  ,   //추가

	OUT_HAND_PIO_SEL,
	OUT_EQPIO_VALID,
	OUT_EQPIO_CS_0, 			// 10
	OUT_EQPIO_CS_1,

	OUT_EQPIO_CS_2,
	OUT_EQPIO_CS_3,
	OUT_EQPIO_TR_REQ,
	OUT_EQPIO_BUSY,				// 15

	// 16 - 31
	OUT_EQPIO_COMPLETE,
	OUT_EQPIO_LEFT_SEL,
	OUT_STEER_F_TRQ_0,

	OUT_STEER_F_TRQ_1,			// 20
	OUT_STEER_R_TRQ_0,
	OUT_STEER_R_TRQ_1,
	OUT_STEER_F_DIR,

	OUT_STEER_R_DIR,
	OUT_SHUTTER_F_RESET,		// 25
	OUT_SHUTTER_R_RESET,
	OUT_SHUTTER_F_CLOSE,

	OUT_SHUTTER_R_CLOSE,
	OUT_SHUTTER_F_OPEN,
	OUT_SHUTTER_R_OPEN,
	OUT_SHUTTER_F_ORIGIN,

	// 32 - 47
	OUT_SHUTTER_R_ORIGIN,
	OUT_SHUTTER_F_FREE,
	OUT_SHUTTER_R_FREE,
	OUT_OBSTACLE_AREA1_SEL,

	OUT_OBSTACLE_AREA2_SEL,
	OUT_OBSTACLE_AREA3_SEL,
	OUT_OBSTACLE_AREA4_SEL,
	OUT_OBSTACLE_AREA5_SEL,
	OUT_OBSTACLE_POWER_RESET,

	OUT_OHTDETECT_AREA1_SEL,
	OUT_OHTDETECT_AREA2_SEL,
	OUT_OHTDETECT_AREA3_SEL,
	OUT_OHTDETECT_AREA4_SEL,
	OUT_OHTDETECT_AREA5_SEL,
	OUT_OHTDETECT_POWER_RESET,
	OUT_LOOKDOWN_AREA1_SEL,
	OUT_LOOKDOWN_AREA2_SEL,
	OUT_LOOKDOWN_AREA3_SEL,

	// 48 - 63
	OUT_LOOKDOWN_AREA4_SEL,		// 50
	OUT_LOOKDOWN_POWER_RESET,
	OUT_LAMP_RED,
	OUT_LAMP_YELLOW_RIGHT,
	OUT_LAMP_YELLOW_LEFT,

	OUT_LAMP_GREEN,
	OUT_HAND_POWER_RESET,
	OUT_BRAKE_DRIVING_RELEASE,
	OUT_BUMPER_CLEAR,            //범퍼 클리어

	OUT_SAFETY_RESET,
	OUT_HOIST_EXT_BRAKE_RELEASE,
	OUT_QRR_POWER,


	OUT_SPARE2,
	OUT_SPARE3,
	OUT_SPARE4,
	OUT_SPARE5,

	// 64 - 79
		// CID 관련 Output
	OUT_CID_Left_Select_ON,
	OUT_CID_Right_Select_ON,
	OUT_CID_Occupancy,
	OUT_CID_Pass_Completion,

	OUT_CID_Abnormal,
	OUT_CID_Operation_Mode,
	OUT_CID_Common_Tag,
	OUT_CID_Reserved_OUT2,

	OUT_SPARE_6,
	OUT_SPARE_7,
	OUT_SHUTTER_F_MIDDLE_CLOSE,

	OUT_SHUTTER_R_MIDDLE_CLOSE,
	OUT_SPARE_9,
	OUT_SPARE_10,
	OUT_SPARE_11,

	// 80 - 95
	OUT_SPARE_12,
	OUT_SPARE_13,
	OUT_SPARE_14,
	OUT_SPARE_15,

	OUT_SPARE_16,
	OUT_SPARE_17,
	OUT_SPARE_18,
	OUT_SPARE_19,

	OUT_SPARE_20,
	OUT_SPARE_21,
	OUT_SPARE_22,
	OUT_SPARE_23,

	OUT_SPARE_24,
	OUT_SPARE_25,
	OUT_SPARE_26,
	OUT_SPARE_27,
};
#else
enum OUTPUT
{
    OUT_HAND_CLOSE = 0,         // 0
	OUT_HAND_OPEN,
	OUT_HAND_ORIGIN,
	OUT_HAND_RESET,
	OUT_HAND_FREE,
	OUT_HAND_RESERVED6,         // 5
	OUT_HAND_RESERVED7,
	OUT_HAND_RESERVED8,
	OUT_HAND_PIO_SEL,
	OUT_EQPIO_VALID,
	OUT_EQPIO_CS_0,             // 10
	OUT_EQPIO_CS_1,
	OUT_EQPIO_TR_REQ,
	OUT_EQPIO_BUSY,
	OUT_EQPIO_COMPLETE,
	OUT_EQPIO_RIGHT_SEL,        // 15
	OUT_EQPIO_LEFT_SEL,
    OUT_STEER_F_TRQ_0,
    OUT_STEER_F_TRQ_1,
    OUT_STEER_R_TRQ_0,
    OUT_STEER_R_TRQ_1,          // 20
    OUT_STEER_F_DIR,
    OUT_STEER_R_DIR,
    OUT_SHUTTER_F_RESET,
    OUT_SHUTTER_R_RESET,
    OUT_SHUTTER_F_CLOSE,        // 25
    OUT_SHUTTER_R_CLOSE,
    OUT_SHUTTER_F_OPEN,
    OUT_SHUTTER_R_OPEN,
    OUT_SHUTTER_F_ORIGIN,
    OUT_SHUTTER_R_ORIGIN,       // 30
    OUT_SHUTTER_F_FREE,
    OUT_SHUTTER_R_FREE,
    OUT_OBSTACLE_AREA1_SEL,
    OUT_OBSTACLE_AREA2_SEL,
    OUT_OBSTACLE_AREA3_SEL,     // 35
    OUT_OBSTACLE_AREA4_SEL,
    OUT_OBSTACLE_POWER_RESET,
    OUT_OHTDETECT_AREA1_SEL,
    OUT_OHTDETECT_AREA2_SEL,
    OUT_OHTDETECT_AREA3_SEL,    // 40
    OUT_OHTDETECT_AREA4_SEL,
    OUT_OHTDETECT_POWER_RESET,
    OUT_LOOKDOWN_AREA1_SEL,
    OUT_LOOKDOWN_AREA2_SEL,
    OUT_LOOKDOWN_AREA3_SEL,     // 45
    OUT_LOOKDOWN_AREA4_SEL,
    OUT_LOOKDOWN_POWER_RESET,
    OUT_LAMP_RED,
    OUT_LAMP_YELLOW,
    OUT_LAMP_GREEN,             // 50
    OUT_STB_R_WORK,
    OUT_STB_L_WORK,
    OUT_HAND_POWER_RESET,
    OUT_BRAKE_DRIVING_RELEASE,
	OUT_BUMPER_CLEAR,           // 55
    OUT_MC_CONTROL,
    OUT_SPARE1,
    OUT_SERIAL_SELECT_1,
    OUT_SERIAL_SELECT_2,
	OUT_RAILSUPPORT_TRQ_0,      // 60
    OUT_RAILSUPPORT_TRQ_1,
    OUT_RAILSUPPORT_DIR,
    OUT_SPARE2
};
#endif


/**
@brief   HW Common 클래스에서 COM Port index로 사용하기 위한 Enum 변수
@author  LSJ
@date    2012.11.05
*/
enum COMPORT
{
    COM_HANDTP = 1,
    COM_HID = 2,
    COM_RIGHT_QRR = 3,  // 임시로 좌우 변경하여 사용
    COM_LEFT_QRR = 4,   // 임시로 좌우 변경하여 사용 
    COM_DRIVING_BCR = 5,
    COM_TRANS_BCR = 6,
	COM_SHUTTER_SETUP,
	COM_OHT_DETECT,
	COM_OBSTACLE,
	COM_ROTATE,
	COM_RF_PIO,
	COM_SERVOSETUP,
	COM_LOOKDOWN,
	COM_CID = 14,   //14
	COM_LCD = 15

};

//---------------------------------------------------------------------------
// Obstacle, OHT Detect 센서 감지 입력
#define DEF_LONG        0
#define DEF_MIDDLE      1
#define DEF_SHORT       2


//---------------------------------------------------------------------------
// Front/Rear Define
//---------------------------------------------------------------------------
#define FRONT           0 ///< Front
#define REAR            1 ///< Rear
#define BOTH            2 ///< Both(Front & Rear)
#define LEFT           	0 ///< Front
#define RIGHT           1 ///< Rear
#define OR            	3 ///< Both(Front & Rear)
//---------------------------------------------------------------------------
#define STEER_NO_COMMAND            0
#define STEER_LEFT_COMMAND          1
#define STEER_RIGHT_COMMAND         2
#define STEER_ORIGIN_COMMAND        3
#define STEER_RESET_COMMAND         4
#define STEER_FRONTLEFT_COMMAND     5
#define STEER_FRONTRIGHT_COMMAND    6
#define STEER_REARLEFT_COMMAND      7
#define STEER_REARRIGHT_COMMAND     8

#define STEER_NO_POS                0
#define STEER_LEFT_POS              1
#define STEER_RIGHT_POS             2

#define STEER_PWRRST_LIMIT_TIME     10000 ///< msec
#define STEER_MOTION_LIMIT_TIME     5000  ///< msec

#define NEXT_CHECK                  -1
//---------------------------------------------------------------------------
// RailSupport Unit 관련 Define
#define RAILSUPPORT_NO_COMMAND          0
#define RAILSUPPORT_HOME_COMMAND        1
#define RAILSUPPORT_WORK_COMMAND        2
#define RAILSUPPORT_ORIGIN_COMMAND      3
#define RAILSUPPORT_RESET_COMMAND       4

#define RAILSUPPORT_PWRRST_LIMIT_TIME   10000           //msec
#define RAILSUPPORT_MOTION_LIMIT_TIME   5000            //msec
//---------------------------------------------------------------------------
// Hand Unit 관련 Define
#define HAND_NO_COMMAND         0
#define HAND_OPEN_COMMAND       1
#define HAND_CLOSE_COMMAND      2
#define HAND_ORIGIN_COMMAND     3
#define HAND_RESET_COMMAND      4

#define HAND_ORIGIN_LIMIT_TIME  6000            //msec  4초에 동작 마진누고 6초로 설정
#define HAND_MOTION_LIMIT_TIME  2000            //msec

#define HAND_GRIPON_WAIT_TIME   3000            //msec

//---------------------------------------------------------------------------
// Shutter Unit 관련 Define
#define SHUTTER_NO_COMMAND          0
#define SHUTTER_OPEN_COMMAND          1
#define SHUTTER_CLOSE_COMMAND       2
#define SHUTTER_ORIGIN_COMMAND      3
#define SHUTTER_RESET_COMMAND       4
#define SHUTTER_MIDDLE_COMMAND      5

#define SHUTTER_NO_POS             0
#define SHUTTER_OPEN_POS           1
#define SHUTTER_CLOSE_POS          2

// by zzang9un 2013.07.18 : Shutter origin 타임 늘림 (10초->15초)
#define SHUTTER_PWRRST_LIMIT_TIME   15000           //msec
#define SHUTTER_MOTION_LIMIT_TIME   4000            //msec
// ---------------------------------------------------------------------------
// Lamp 관련 Define
#define DEF_RED         	  0
//#define DEF_YELLOW      1
#define DEF_YELLOW_RIGHT      1
#define DEF_GREEN       	  2
#define DEF_YELLOW_LEFT		  3

#define SETLAMP_ALL_OFF         			    0
#define SETLAMP_ALL_ON             				1
#define SETLAMP_Y_ON              			    2
#define SETLAMP_G_ON               				3
#define SETLAMP_R_ON               				4
#define SETLAMP_Y_FLICKER          				5
#define SETLAMP_G_FLICKER         				6
#define SETLAMP_R_FLICKER         				7
#define SETLAMP_R_ON_G_FLICKER    				8
#define SETLAMP_G_ON_Y_FLICKER     				9
#define SETLAMP_Y_ON_G_ON     	   			   10
#define SETLAMP_G_FLICKER_Y_FLICKER     	   11
#define SETLAMP_G_FLICKER_Y_L_FLICKER     	   12
#define SETLAMP_G_FLICKER_Y_R_FLICKER     	   13
// ---------------------------------------------------------------------------
// Sound 관련 Define
#define SETSOUND_NONE              0
#define SETSOUND_DRIVING           1
#define SETSOUND_TRANS             2
#define SETSOUND_ERROR             3
#define SETSOUND_OBS               4
#define SETSOUND_DETECT            5
#define SETSOUND_PAUSE             6
#define SETSOUND_OBSWARNNING       7
#define SETSOUND_LOOKDOWN          8
#define SETSOUND_OSCILLATION   	   9

//---------------------------------------------------------------------------

#define INPOS_LIMIT                2.0  //500.0	//@toto: 영환이와 이야기 후 삭제
#define INPOS_LIMIT_SMALLADD_HOME 9.0

#define HOIST_SMALL_ADD_HOISTHOME_DISTANCE	4.0
#define HOIST_SMALL_ADD_HOISTHOME_LIMIT	-4.0


//---------------------------------------------------------------------------
//  Content : Pickup Temperature Error Limit
//---------------------------------------------------------------------------
#define PICKUPTEMPERLIMIT          65.0   //50.0  : 2011.09.16 변경함
//---------------------------------------------------------------------------
//  Content : IPC Temperature Error Limit
//---------------------------------------------------------------------------
#define IPCTEMPERLIMIT             50.0

#define STOPTAG_LEN_AUTO    140.0


#endif  // Def_HWH

