/**
@file    Def_HW.h
$Rev:: 605           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-02-25 1#$:  Date of last commit
@version OHT 7.0
@brief   HW에 공통으로 사용될 상수, 변수를 정의한 Header File
*/

#ifndef Def_HWH
#define Def_HWH

#include "Def_OHT.h"
#include "HoistDistanceConverter.h"
// -------------------------------------------
// IO 관련 구조체
// -------------------------------------------

/**
@brief   HWCommon에서 사용하는 Input 72개
@author  zzang9un
@date    2012.10.15
@note    Bit Filed에서 unsigned char로 변경
*/
typedef struct sINPUT_COMMON_
{
	// BYTE INPUT[72];
	BYTE INPUT[96];
} sINPUT_COMMON;

/**
@brief   HWCommon에서 사용하는 Input 64개
@author  zzang9un
@date    2012.10.15
@note    Bit Filed에서 unsigned char로 변경
*/

typedef struct sINPUT_V70_R01_
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
	BYTE Reserved_48;
	BYTE MC_Status;
	BYTE HANDTP_Connect;
	BYTE Reserved_51;
	BYTE STOP_Driving_Front;
	BYTE STOP_Driving_PreCheck;
	BYTE STOP_Trans_Front;
	BYTE STOP_Trans_Rear;
	BYTE BCR_Driving_OK;
	BYTE BCR_Driving_NG;
	BYTE BCR_Trans_OK;
	BYTE BCR_Trans_NG;
	BYTE HOIST_Belt_Detect;
	BYTE RailSupport_Home;
	BYTE Railsupport_Work;
	BYTE MAC_Detect;
	BYTE SMPS_Short;
	//TG 추가
	BYTE CID_Link_Compt_IN;
	BYTE CID_Interlock_IN;
	BYTE CID_Pass_OK_IN;
	BYTE CID_Status_IN;
	BYTE CID_Reserved_IN1;
	BYTE CID_Reserved_IN2;
	BYTE Outtrigger_F_Alarm_IN;
	BYTE Outtrigger_F_Home_IN;
	BYTE Outtrigger_F_Work_IN;
	BYTE Outtrigger_R_Alarm_IN;
	BYTE Outtrigger_R_Home_IN;
	BYTE Outtrigger_R_Work_IN;
	BYTE Rotate_L_Limit;
	BYTE Rotate_R_Limit;
	BYTE Rotate_Inposition;
	BYTE Rotate_Alarm;
}sINPUT_V70_R01;

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

/**
@brief   Reticle OHT가 HWCommon에서 사용하는 Input
@author  Labeler
@date    2016.12.09
@note    Bit Filed에서 unsigned char로 변경
*/
typedef struct sINPUT_RETICLE
{
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
	BYTE PIO_Input3;
	BYTE PIO_Input5;
	BYTE PIO_Input6;
	BYTE SMPS_Short;
	//TG 추가
	BYTE CID_Link_Compt_IN;
	BYTE CID_Interlock_IN;
	BYTE CID_Pass_OK_IN;
	BYTE CID_Status_IN;
	BYTE CID_Reserved_IN1;
	BYTE CID_Reserved_IN2;
	BYTE Outtrigger_Reserved71;
	BYTE Outtrigger_Reserved72;
	BYTE Outtrigger_Reserved73;
	BYTE Outtrigger_Reserved74;
	BYTE HOIST_Home;
	BYTE Rotate_Reserved76;
	BYTE Rotate_Reserved77;
	BYTE Rotate_Reserved78;
	BYTE Rotate_Reserved79;
	BYTE Rotate_Reserved80;
}sINPUT_RETICLE;


typedef struct sINPUT_V85_EUV_
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
	BYTE CID_Pass_OK_IN;
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

} sINPUT_V85_EUV;


typedef struct sINPUT_V81_S_
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
	BYTE CID_Pass_OK_IN;
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
}sINPUT_V81_S;


typedef struct sINPUT_CommonIO_
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
	BYTE CID_Pass_OK_IN;
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
}sINPUT_CommonIO;


/**
@brief   HWCommon에서 사용하는 Output 72개
@author  LSJ
@date    2012.10.23
*/
typedef struct sOUTPUT_COMMON_
{
	BYTE OUTPUT[96];
} sOUTPUT_COMMON;

/**
@brief   HWCommon에서 사용하는 Output 64개
@author  LSJ
@date    2012.10.23
*/

typedef struct sOUTPUT_V70_R01_
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
	BYTE LAMP_Yellow;
	BYTE LAMP_Green;
	BYTE HAND_Power_Reset;
	BYTE BRAKE_Driving_Release;
	BYTE SAFETY_Clear;
	BYTE MC_Control;
	BYTE HOIST_Ext_Brake_Release;
	BYTE SERIAL_Select_1;
	BYTE SERIAL_Select_2;
	BYTE RailSupport_TRQ_0;
	BYTE RailSupport_TRQ_1;
	BYTE RailSupport_Dir;
	BYTE Reserved_1;

	BYTE OUT_CID_Left_Select_OUT;    //64
    BYTE OUT_CID_Right_Select_OUT;   //65
    BYTE OUT_CID_Occup_Req_OUT;
    BYTE OUT_CID_PASS_Compt_OUT;
    BYTE OUT_CID_Manual_OUT;
    BYTE OUT_CID_Operation_OUT;
    BYTE OUT_CID_Reserved_OUT1;      //70
    BYTE OUT_CID_Reserved_OUT2;
	
    BYTE OUT_Outtrigger_F_Start_OUT;
    BYTE OUT_Outtrigger_F_DIR_OUT;
    BYTE OUT_Outtrigger_F_Brake_OUT;
    BYTE OUT_Outtrigger_F_Speed_OUT; //75
    BYTE OUT_Outtrigger_R_Start_OUT;
    BYTE OUT_Outtrigger_R_DIR_OUT;
    BYTE OUT_Outtrigger_R_Brake_OUT;
    BYTE OUT_Outtrigger_R_Speed_OUT;

} sOUTPUT_V70_R01;

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

/**
@brief   Reticle OHT가 HWCommon에서 사용하는 Output 64개
@author  Labeler
@date    2016.12.09
*/
typedef struct sOUTPUT_RETICLE_
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
	BYTE LAMP_Yellow;
	BYTE LAMP_Green;
	BYTE HAND_Power_Reset;
	BYTE BRAKE_Driving_Release;
	BYTE SAFETY_Clear;
	BYTE SAFETY_Reset;
	BYTE HOIST_Ext_Brake_Release;
	BYTE Reserved59;
	BYTE Reserved60;
	BYTE Reserved61;
	BYTE Reserved62;
	BYTE Reserved63;
	BYTE Reserved64;
	//TG 추가
	BYTE CID_Left_Select_OUT;
	BYTE CID_Right_Select_OUT;
	BYTE CID_Occup_Req_OUT;
	BYTE CID_PASS_Compt_OUT;
	BYTE CID_Manual_OUT;
	BYTE CID_Operation_OUT;
	BYTE CID_Reserved_OUT1; //동일 lane link 금지
	BYTE CID_Reserved_OUT2;
	BYTE Outtrigger_Reserved73;
	BYTE Outtrigger_Reserved74;
	BYTE Outtrigger_Reserved75;
	BYTE Outtrigger_Reserved76;
	BYTE Outtrigger_Reserved77;
	BYTE Outtrigger_Reserved78;
	BYTE Outtrigger_Reserved79;
	BYTE Outtrigger_Reserved80;
} sOUTPUT_RETICLE;

typedef struct sOUTPUT_V85_EUV_
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
}  sOUTPUT_V85_EUV;


typedef struct sOUTPUT_V81_S_
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

} sOUTPUT_V81_S;


typedef struct sOUTPUT_CommonIO_
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

} sOUTPUT_CommonIO;

/**
@brief   기타 Go 관련 Command(OCS->OHT) 구조체.
@author  zzang9un
@date    2013.03.18
@note    MTL, Map Make, Clean 명령에 사용한다.
*/
typedef union uINPUT_
{
	sINPUT_COMMON		InputCommon;
	sINPUT_V70_R01		InputV70_R01;
	sINPUT_V85_EUV      Input_EUV;
	sINPUT_RETICLE      Input_Reticle;
	sINPUT_V81_S      	Input_V81S;
	sINPUTV81           Input_V81;
} uINPUT;

/**
@brief   기타 Go 관련 Command(OCS->OHT) 구조체.
@author  zzang9un
@date    2013.03.18
@note    MTL, Map Make, Clean 명령에 사용한다.
*/
typedef union uOUTPUT_
{
	sOUTPUT_COMMON		OutputCommon;
	sOUTPUT_V70_R01		OutputV70_R01;
	sOUTPUT_V85_EUV    	Output_EUV;
	sOUTPUT_RETICLE     Output_Reticle;
	sOUTPUT_V81_S     	Output_V81S;
	sOUTPUTV81          Output_V81;
} uOUTPUT;

/**
@brief   HW Common 클래스에서 COM Port index로 사용하기 위한 Enum 변수
@author  LSJ
@date    2012.11.05
*/
//enum COMPORT
//{
//  COM_REMOCON = 0,
//  COM_HID,
//  COM_LEFT_QRR,
//  COM_RIGHT_QRR,
//  COM_TRANS_BCR,
//  COM_DRIVING_BCR,
//  COM_HW_SELECT,
//  COM_OHTCOMM,
//  COM_TCU,
//  COM_ROTATE,
//  COM_LCD_MODULE,
//  COM_SERVOSETUP
//};
enum COMPORT
{
    COM_REMOCON = 1,
    COM_HID = 2,
    COM_RIGHT_QRR = 3,  // 임시로 좌우 변경하여 사용
    COM_LEFT_QRR = 4,   // 임시로 좌우 변경하여 사용 
    COM_DRIVING_BCR = 5,
    COM_TRANS_BCR = 6,
    COM_HW_SELECT,
    COM_OHTCOMM,
    COM_TCU,
    COM_ROTATE,
    COM_LCD_MODULE,
    COM_SERVOSETUP
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
//---------------------------------------------------------------------------
#define STEER_NO_COMMAND            0
#define STEER_LEFT_COMMAND          1
#define STEER_RIGHT_COMMAND         2
#define STEER_ORIGIN_COMMAND        3
#define STEER_RESET_COMMAND         4

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

#define HAND_ORIGIN_LIMIT_TIME  7000            //msec
#define HAND_MOTION_LIMIT_TIME  1500            //msec

#define HAND_GRIPON_WAIT_TIME   3000            //msec

//---------------------------------------------------------------------------
// Shutter Unit 관련 Define
#define SHUTTER_NO_COMMAND          0
#define SHUTTER_OPEN_COMMAND          1
#define SHUTTER_CLOSE_COMMAND       2
#define SHUTTER_ORIGIN_COMMAND      3
#define SHUTTER_RESET_COMMAND       4

#define SHUTTER_NO_POS             0
#define SHUTTER_OPEN_POS           1
#define SHUTTER_CLOSE_POS          2

#define SHUTTER_PWRRST_LIMIT_TIME   10000           //msec
#define SHUTTER_MOTION_LIMIT_TIME   2000            //msec
// ---------------------------------------------------------------------------
// Lamp 관련 Define
#define DEF_RED         0
#define DEF_YELLOW      1
#define DEF_GREEN       2

#define SETLAMP_ALL_OFF            0
#define SETLAMP_ALL_ON             1
#define SETLAMP_Y_ON               2
#define SETLAMP_G_ON               3
#define SETLAMP_R_ON               4
#define SETLAMP_Y_FLICKER          5
#define SETLAMP_G_FLICKER          6
#define SETLAMP_R_FLICKER          7
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
//---------------------------------------------------------------------------
// TransControl 관련 Define
#define NAME_TRANSFILE              "TransParam.ini"

// Hoist 저속 구간 길이
#define DEF_nUpstate_SpeedDown_Offset    60    //mm
#define DEF_nDnState_UP_SpeedDown_Offset 50    //mm
#define DEF_nDnState_DN_SpeedDown_Offset 60    //45    //mm

#define LOOKDN_DETECT_TIMEOVER     8000    //msec
#define TRANS_TIMEOVER             60000   //30000   //msec
#define OSCILLATIONCHECKDIST       15     //mm
#define HOIST_MOVINPOS_LIMIT       25  //mm 임시

#define SHIFTROTATE_HOME2TP            1 //ShiftRotate ORIGN -> TPOINT
#define SHIFTROTATE_TP2HOME            2 //ShiftRotate TPOINT -> ORIGIN

///@todo 추후 값 변경 필요
#define SHIFT_FAST_MOVE_DIST       80      //80mm
#define OSCILLATION_IGNORE_DIST    250.0   //mm
#define IS_STB_LIMIT               200.0   //mm
#define IS_STK_LIMIT               500.0   //mm

#define HOIST_DOWN_STATE     0
#define HOIST_UP_STATE       1

#define MOVETP_RETRYCOUNT         10

#define STBUTBSensorOFFTime        3000    //msec
#define  RAILSUPPORT_HOMING_TIMEOUT        10000    //msec
//---------------------------------------------------------------------------
// Content : TRANS_MOVING_CASE
//---------------------------------------------------------------------------
#define HOIST_DOWN_H2TO            1 //ORIGIN -> TPOINT-A
#define HOIST_DOWN_TO2TP           2 //TPOINT-A -> TPOINT
#define HOIST_DOWN_H2TP            3 //ORIGIN -> TPOINT
#define HOIST_UP_TP2TO             4 //TPOINT -> TPOINT-A
#define HOIST_UP_TP2HO             5 //TPOINT -> ORIGIN-A
#define HOIST_UP_TO2HO             6 //TPOINT-A -> ORIGIN-A
#define HOIST_UP_HO2H              7 //ORIGIN-A -> ORIGIN
#define HOIST_UP_TO2H              8 //TPOINT-A -> ORIGIN

// by zzang9un 2012.12.18 : 속도 올림
#define SHIFT_SPEED_RATIO       2.5
#define SHIFT_MAX_SPEED        (0.19 * SHIFT_SPEED_RATIO) ///< Shift 이적재 최고 속도(mm/msec)

#define MULTIPLY_34_70P        0.7         // UNIT:70%
#define MULTIPLY_34_30P        0.3         // UNIT:30%
#define MULTIPLY_34_10P        0.1         // UNIT:10%
#define MULTIPLY_34_100P       1.0         // UNIT:100%

#define SHIFT_ACCTIME          550   //msec

#define HOIST_MAX_SPEED        2.67*0.5 //2.67 ///< Hoist 이적재 최고 속도(mm/msec)

#define MULTIPLY_2_95P         0.95        // UNIT:95%
#define MULTIPLY_2_80P         0.8         // UNIT:80%
#define MULTIPLY_2_60P         0.6         // UNIT:60%
#define MULTIPLY_2_40P         0.4         // UNIT:40%
#define MULTIPLY_2_20P         0.2         // UNIT:20%
#define MULTIPLY_2_10P         0.1         // UNIT:10%
#define MULTIPLY_2_6P          0.06        // UNIT:6%
#define MULTIPLY_2_3P          0.03        // UNIT:3%
#define MULTIPLY_2_2P          0.02        // UNIT:2%

#define HOIST_ACCTIME_50        50  //0.05     // UNIT:250MSEC
#define HOIST_ACCTIME_250      250 //0.25     // UNIT:250MSEC
#define HOIST_ACCTIME_500      500 //0.5     // UNIT:500MSEC


#define HOIST_POS_LIMIT            5100.0  //2nd Axis(Hoist)  Positive Software Limit
#define HOIST_NEG_LIMIT            -2.0          //2nd Axis(Hoist)  Negative Software Limit
#define SHIFT_POS_LIMIT         490.0          //3rd Axis(Shift)  Positive Software Limit  151086.0 / 144000.0
#define SHIFT_NEG_LIMIT         -490.0         //3rd Axis(Shift)  Negative Software Limit  -121354.0 / -121000.0

#define INPOS_LIMIT                2.0  //500.0
#define HOIST_POS_LIMIT_FORDRV     10.0 //5000.0      //주행시 Hoist Limit 설정
#define HOIST_NEG_LIMIT_FORDRV     -2.0 //-5000.0      //주행시 Hoist Limit 설정
#define SHIFT_POS_LIMIT_FORWAIT     20.0   //20000.0      //주행시 Shift Limit 설정
#define SHIFT_NEG_LIMIT_FORWAIT     -20.0    //-20000.0
#define HOIST_POS_LIMIT_FORARRIVED     10.0 //5000.0      //정지시 Hoist Limit 설정
#define HOIST_POS_LIMIT_TRANSOFFSET     30.0 //5000.0      //이적재시 Hoist Limit설정 시 Offset

//---------------------------------------------------------------------------
//  Content : Hoist, Shift, Rotate Origin Position
//---------------------------------------------------------------------------
//extern double getHoistOriginLimit(void);
#define HOIST_ORIGIN_LIMIT	CHoistConverter::getHoistOriginLimit()
#define SHIFT_ORIGIN_LIMIT         2.0  //500.0
#define ROTATE_ORIGIN_LIMIT        2.0  //550.0
#define SHIFT_DRV_MOVE_NEGLIMIT    -30.0 //-16000.0 //파티션쪽 28mm=-16989
#define SHIFT_DRV_MOVE_POSLIMIT    20.0 //11000.0 //파티션쪽 19.5mm=11832
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

