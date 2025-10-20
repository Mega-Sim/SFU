#ifndef InputMaskH
#define InputMaskH

#include <stdio.h>
#include "Def_OHT.h"

// by zzang9un 2013.07.09 : Input Data Mask를 위한 변수

typedef struct OHT80_R01_INPUT1_MASK_
{
	BYTE HAND_Inposition	: 1;
	BYTE HAND_Alarm			: 1;
	BYTE HAND_Right_Grip	: 1;
	BYTE HAND_Left_Grip		: 1;
	BYTE HAND_Close			: 1;
	BYTE HAND_Open			: 1;
	BYTE HAND_Push			: 1;

	BYTE HOIST_Home			: 1;

	BYTE HAND_PIO_Go		: 1;
	BYTE EQPIO_Load_REQ		: 1;
	BYTE EQPIO_Unload_REQ	: 1;
	BYTE EQPIO_Ready		: 1;
	BYTE EQPIO_HO_AVBL		: 1;
	BYTE EQPIO_ES			: 1;
	BYTE EQPIO_Right_Go		: 1;
	BYTE EQPIO_Left_Go		: 1;

	BYTE STEER_F_Left		: 1;
	BYTE STEER_R_Left		: 1;
	BYTE STEER_F_Right		: 1;
	BYTE STEER_R_Right		: 1;
	BYTE SHUTTER_F_Close	: 1;
	BYTE SHUTTER_R_Close	: 1;
	BYTE SHUTTER_F_Open		: 1;
	BYTE SHUTTER_R_Open		: 1;

	BYTE SHUTTER_F_Alarm	: 1;
	BYTE SHUTTER_R_Alarm	: 1;
	BYTE SHUTTER_F_Inposition	: 1;
	BYTE SHUTTER_R_Inposition	: 1;
	BYTE OBSTACLE_Long			: 1;
	BYTE OBSTACLE_Middle		: 1;
	BYTE OBSTACLE_Short			: 1;
	BYTE OBSTACLE_Fail			: 1;
}OHT80_R01_INPUT1_MASK;

typedef struct OHT80_R01_INPUT2_MASK_
{
	BYTE OHTDETECT_Long		: 1;
	BYTE OHTDETECT_Middle	: 1;
	BYTE OHTDETECT_Short	: 1;
	BYTE OHTDETECT_Fail		: 1;
	BYTE LOOKDOWN_Detect	: 1;
	BYTE LOOKDOWN_Error		: 1;
	BYTE SHIFT_Home			: 1;
	BYTE STB_Detect_Left	: 1;
	BYTE STB_Detect_Right	: 1;
	BYTE UTB_Detect			: 1;
	BYTE FOUP_Detect		: 1;
	BYTE OSCILLATION		: 1;
	BYTE EMO_Press			: 1;
	BYTE BUMPER_Press		: 1;
	BYTE FAN_Fail			: 1;
	BYTE Reserved_48		: 1;
	BYTE MC_Status			: 1;
	BYTE HANDTP_Connect		: 1;
	BYTE FOUP_Cover_Open_Detect		: 1;
	BYTE STOP_Driving_Front	: 1;
	BYTE STOP_Driving_PreCheck	: 1;
	BYTE STOP_Trans_Front		: 1;
	BYTE STOP_Trans_Rear		: 1;
	BYTE BCR_Driving_OK		: 1;
	BYTE BCR_Driving_NG		: 1;
	BYTE BCR_Trans_OK		: 1;
	BYTE BCR_Trans_NG		: 1;
	BYTE HOIST_Belt_Detect	: 1;
	BYTE RailSupport_Home	: 1;
	BYTE Railsupport_Work	: 1;
	BYTE MAC_Detect	: 1;
	BYTE SMPS_Short	: 1;
} OHT80_R01_INPUT2_MASK;

 //TG 추가
typedef struct OHT80_R01_INPUT3_MASK_
   {
	BYTE CID_Link_Compt_IN : 1;
	BYTE CID_Interlock_IN : 1;
	BYTE CID_Completion_ACK : 1;
	BYTE CID_Status_IN : 1;
	BYTE CID_Reserved_IN1 : 1;
	BYTE CID_Reserved_IN2 : 1;
	BYTE Outtrigger_F_Alarm_IN : 1;
	BYTE Outtrigger_F_Home_IN : 1;
	BYTE Outtrigger_F_Work_IN : 1;
	BYTE Outtrigger_R_Alarm_IN : 1;
	BYTE Outtrigger_R_Home_IN : 1;
	BYTE Outtrigger_R_Work_IN : 1;
	BYTE Rotate_L_Limit : 1;
	BYTE Rotate_R_Limit : 1;
	BYTE Rotate_Inposition : 1;
	BYTE Rotate_Alarm : 1;
 } OHT80_R01_INPUT3_MASK;


typedef struct FOUP_V81_INPUT1_MASK_                  // JIG INPUT IO Timy
{
	BYTE HAND_Inposition	: 1;
	BYTE HAND_Alarm			: 1;
	BYTE HAND_Right_Grip	: 1;
	BYTE HAND_Left_Grip		: 1;
	BYTE HAND_Close			: 1;
	BYTE HAND_Open			: 1;
	BYTE HAND_Push			: 1;
	BYTE Reserved_7			: 1;	//BYTE HOIST_Home			: 1;

	BYTE HAND_PIO_Go		: 1;
	BYTE ROTATE_Inposition  : 1;//BYTE EQPIO_Load_REQ		: 1;
	BYTE ROTATE_Alarm		: 1;//BYTE EQPIO_Unload_REQ	: 1;
	BYTE ROTATE_90			: 1;//BYTE EQPIO_Ready		: 1;
	BYTE EQPIO_HO_AVBL		: 1;
	BYTE EQPIO_ES			: 1;
	BYTE EQPIO_Right_Go		: 1;
	BYTE EQPIO_Left_Go		: 1;

	BYTE STEER_F_Left		: 1;
	BYTE STEER_R_Left		: 1;
	BYTE STEER_F_Right		: 1;
	BYTE STEER_R_Right		: 1;
	BYTE SHUTTER_F_Close	: 1;
	BYTE SHUTTER_R_Close	: 1;
	BYTE SHUTTER_F_Open		: 1;
	BYTE SHUTTER_R_Open		: 1;

	BYTE SHUTTER_F_Alarm	: 1;
	BYTE SHUTTER_R_Alarm	: 1;
	BYTE SHUTTER_F_Inposition	: 1;
	BYTE SHUTTER_R_Inposition	: 1;
	BYTE OBSTACLE_Long			: 1;
	BYTE OBSTACLE_Middle		: 1;
	BYTE OBSTACLE_Short			: 1;
	BYTE OBSTACLE_Fail			: 1;
}FOUP_V81_INPUT1_MASK;

typedef struct FOUP_V81_INPUT2_MASK_
{
	BYTE OHTDETECT_Long		: 1;
	BYTE OHTDETECT_Middle	: 1;
	BYTE OHTDETECT_Short	: 1;
	BYTE OHTDETECT_Fail		: 1;
	BYTE LOOKDOWN_Detect	: 1;
	BYTE LOOKDOWN_Error		: 1;
	BYTE SHIFT_Home			: 1;
	BYTE STB_Detect_Left	: 1;
	BYTE STB_Detect_Right	: 1;
	BYTE UTB_Detect			: 1;
	BYTE FOUP_Detect		: 1;
	BYTE OSCILLATION		: 1;
	BYTE EMO_Press			: 1;
	BYTE BUMPER_Press		: 1;
	BYTE FAN_Fail			: 1;
	BYTE Reserved_48		: 1;
	BYTE MC_Status			: 1;
	BYTE HANDTP_Connect		: 1;
	BYTE FOUP_Cover_Open_Detect		: 1;
	BYTE STOP_Driving_Front	: 1;
	BYTE STOP_Driving_PreCheck	: 1;
	BYTE STOP_Trans_Front		: 1;
	BYTE STOP_Trans_Rear		: 1;
	BYTE BCR_Driving_OK		: 1;
	BYTE BCR_Driving_NG		: 1;
	BYTE BCR_Trans_OK		: 1;
	BYTE BCR_Trans_NG		: 1;
	BYTE HOIST_Belt_Detect	: 1;
	BYTE ROTATE_0 			: 1;//BYTE RailSupport_Home	: 1;
	BYTE ROTATE_180			: 1;//BYTE Railsupport_Work	: 1;
	BYTE ROTATE_270			: 1;//BYTE MAC_Detect	: 1;
	BYTE SMPS_Short	: 1;
} FOUP_V81_INPUT2_MASK;

 //TG 추가
typedef struct FOUP_V81_INPUT3_MASK_
   {
	BYTE CID_Link_Compt_IN : 1;
	BYTE CID_Interlock_IN : 1;
	BYTE CID_Completion_ACK : 1;
	BYTE CID_Status_IN : 1;
	BYTE CID_Reserved_IN1 : 1;
	BYTE CID_Reserved_IN2 : 1;
	BYTE Reserved_70 : 1;
	BYTE Reserved_71 : 1;
	BYTE Reserved_72 : 1;
	BYTE Reserved_73 : 1;
	BYTE HOIST_Home  : 1; //BYTE Reserved_74 : 1;
	BYTE Reserved_75 : 1;
	BYTE Reserved_76 : 1;
	BYTE EQPIO_Load_REQ		: 1; //BYTE Reserved_79 : 1;  임시 IO 차용 (컴파일 문제로)
	BYTE EQPIO_Unload_REQ	: 1; //BYTE Reserved_80 : 1;
	BYTE EQPIO_Ready		: 1; //BYTE Reserved_81 : 1;
 } FOUP_V81_INPUT3_MASK;
// 2019-03-16, Add by puting
// Reticle OHT용 Input Mask 추가 8.5
typedef struct STANDARD_OHT_V81S_INPUT1_MASK_
{
	BYTE HAND_Inposition		: 1;
	BYTE HAND_Alarm				: 1;
	BYTE HAND_Right_Grip		: 1;
	BYTE HAND_Left_Grip			: 1;
	BYTE HAND_Close		    	: 1;
	BYTE HAND_Open		    	: 1;
	BYTE HAND_Push		    	: 1;
	BYTE HOIST_Home        		: 1;

	BYTE HNAD_ES         		: 1;
	BYTE HAND_PIO_Go	    	: 1;
	BYTE EQPIO_Load_REQ	    	: 1;
	BYTE EQPIO_Unload_REQ   	: 1;
	BYTE EQPIO_Ready	    	: 1;
	BYTE EQPIO_HO_AVBL	    	: 1;
	BYTE EQPIO_ES		    	: 1;
	BYTE EQPIO_Left_Go	    	: 1;

	BYTE STEER_F_Left	    	: 1;
	BYTE STEER_R_Left		    : 1;
	BYTE STEER_F_Right		    : 1;
	BYTE STEER_R_Right		    : 1;
	BYTE SHUTTER_F_Close	    : 1;
	BYTE SHUTTER_R_Close	    : 1;
	BYTE SHUTTER_F_Open		    : 1;
	BYTE SHUTTER_R_Open		    : 1;

	BYTE SHUTTER_F_Alarm	    : 1;
	BYTE SHUTTER_R_Alarm	    : 1;
	BYTE SHUTTER_F_Inposition	: 1;
	BYTE SHUTTER_R_Inposition	: 1;
	BYTE OBSTACLE_Input_Extend  : 1;
	BYTE OBSTACLE_Long			: 1;
	BYTE OBSTACLE_Middle		: 1;
	BYTE OBSTACLE_Short			: 1;

}STANDARD_OHT_V81S_INPUT1_MASK;



typedef struct STANDARD_OHT_V81S_INPUT2_MASK_
{
	BYTE OBSTACLE_Fail			: 1;
    BYTE OHTDETECT_Input_Extend : 1;
	BYTE OHTDETECT_Long		    : 1;
	BYTE OHTDETECT_Middle	    : 1;
	BYTE OHTDETECT_Short	    : 1;
	BYTE OHTDETECT_Fail		    : 1;
	BYTE LOOKDOWN_Detect	    : 1;
	BYTE LOOKDOWN_Error		    : 1;

	BYTE SHIFT_Home			    : 1;
	BYTE STB_Detect_Left	    : 1;
	BYTE STB_Detect_Right	    : 1;
	BYTE UTB_Detect			    : 1;
	BYTE FOUP_Detect		    : 1;
	BYTE OSCILLATION		    : 1;
	BYTE EMO_Press			    : 1;
	BYTE BUMPER_Press		    : 1;

	BYTE FAN_Fail			    : 1;
	BYTE HOIST_Belt_Detect1	    : 1;
	BYTE HOIST_Belt_Detect2	    : 1;
	BYTE TP_Enable				: 1;  //TP Enable
	BYTE HANDTP_Connect		    : 1;
	BYTE FOUP_Cover_Open_Detect	: 1;
	BYTE STOP_Driving_Front	    : 1;
	BYTE STOP_Driving_PreCheck	: 1;

	BYTE STOP_Trans_Front		: 1;
	BYTE STOP_Trans_Rear		: 1;
	BYTE BCR_Driving_OK		    : 1;
	BYTE BCR_Driving_NG		    : 1;
	BYTE BCR_Trans_OK		    : 1;
	BYTE BCR_Trans_NG		    : 1;
	BYTE Reserved_1	    		: 1;
	BYTE Reserved_2	   			: 1;


} STANDARD_OHT_V81S_INPUT2_MASK;

typedef struct STANDARD_OHT_V81S_INPUT3_MASK_
{
	// 0 - 15
	BYTE Reserved_3	    		: 1;
	BYTE SMPS_Short	            : 1;
	BYTE CID_Link_Compt_IN		: 1;
	BYTE CID_Interlock_IN		: 1;
	BYTE CID_Completion_ACK		: 1;
	BYTE CID_Status_IN			: 1;
	BYTE CID_Reserved_IN1		: 1;
	BYTE CID_Reserved_IN2		: 1;

	BYTE Rotate_0_Sensor	  	: 1;
	BYTE Rotate_90_Sensor	  	: 1;
	BYTE Rotate_180_Sensor	  	: 1;
	BYTE Rotate_270_Sensor	  	: 1;
	BYTE HNAD_Reserved_1				: 1;
	BYTE HOIST_Home_2			: 1;		// IO배치는 이 라인부터 틀어진다
	BYTE SHUTTER_F_Middle_Close	: 1;
	BYTE SHUTTER_R_Middle_Close	: 1; 		// Outtrigger_Reserved71;

	BYTE POD_EUV_1				: 1; 		// Outtrigger_Reserved72;
	BYTE POD_EUV_2				: 1;
	BYTE SPARE_5				: 1;
	BYTE SPARE_6				: 1;
	BYTE SPARE_7				: 1;
	BYTE SPARE_8				: 1;
	BYTE SPARE_9				: 1;
	BYTE SPARE_10				: 1;

	BYTE SPARE_11				: 1;
	BYTE SPARE_12				: 1;
	BYTE SPARE_13				: 1;

	BYTE SPARE_14				: 1;
	BYTE SPARE_15				: 1;
	BYTE SPARE_16				: 1;
	BYTE SPARE_17				: 1;
	BYTE SPARE_18				: 1;

}STANDARD_OHT_V81S_INPUT3_MASK;

// 2019-03-16, Add by puting
// Reticle OHT용 Input Mask 추가 8.5
typedef struct STANDARD_OHT_V85_INPUT1_MASK_
{
	BYTE HAND_Inposition		: 1;
	BYTE HAND_Alarm				: 1;
	BYTE HAND_Right_Grip		: 1;
	BYTE HAND_Left_Grip			: 1;
	BYTE HAND_Close		    	: 1;
	BYTE HAND_Open		    	: 1;
	BYTE HAND_Push		    	: 1;
	BYTE HNAD_Reserved_1        : 1;

	BYTE HNAD_ES         		: 1;
	BYTE HAND_PIO_Go	    	: 1;
	BYTE EQPIO_Load_REQ	    	: 1;
	BYTE EQPIO_Unload_REQ   	: 1;
	BYTE EQPIO_Ready	    	: 1;
	BYTE EQPIO_HO_AVBL	    	: 1;
	BYTE EQPIO_ES		    	: 1;
	BYTE EQPIO_Left_Go	    	: 1;

	BYTE STEER_F_Left	    	: 1;
	BYTE STEER_R_Left		    : 1;
	BYTE STEER_F_Right		    : 1;
	BYTE STEER_R_Right		    : 1;
	BYTE SHUTTER_F_Close	    : 1;
	BYTE SHUTTER_R_Close	    : 1;
	BYTE SHUTTER_F_Open		    : 1;
	BYTE SHUTTER_R_Open		    : 1;

	BYTE SHUTTER_F_Alarm	    : 1;
	BYTE SHUTTER_R_Alarm	    : 1;
	BYTE SHUTTER_F_Inposition	: 1;
	BYTE SHUTTER_R_Inposition	: 1;
	BYTE OBSTACLE_Input_Extend  : 1;
	BYTE OBSTACLE_Long			: 1;
	BYTE OBSTACLE_Middle		: 1;
	BYTE OBSTACLE_Short			: 1;

}STANDARD_OHT_V85_INPUT1_MASK;



typedef struct STANDARD_OHT_V85_INPUT2_MASK_
{
	BYTE OBSTACLE_Fail			: 1;
    BYTE OHTDETECT_Input_Extend : 1;
	BYTE OHTDETECT_Long		    : 1;
	BYTE OHTDETECT_Middle	    : 1;
	BYTE OHTDETECT_Short	    : 1;
	BYTE OHTDETECT_Fail		    : 1;
	BYTE LOOKDOWN_Detect	    : 1;
	BYTE LOOKDOWN_Error		    : 1;

	BYTE SHIFT_Home			    : 1;
	BYTE STB_Detect_Left	    : 1;
	BYTE STB_Detect_Right	    : 1;
	BYTE UTB_Detect			    : 1;
	BYTE FOUP_Detect		    : 1;
	BYTE OSCILLATION		    : 1;
	BYTE EMO_Press			    : 1;
	BYTE BUMPER_Press		    : 1;

	BYTE FAN_Fail			    : 1;
	BYTE HOIST_Belt_Detect1	    : 1;
	BYTE HOIST_Belt_Detect2	    : 1;
	BYTE TP_Enable				: 1;  //TP Enable
	BYTE HANDTP_Connect		    : 1;
	BYTE FOUP_Cover_Open_Detect	: 1;
	BYTE STOP_Driving_Front	    : 1;
	BYTE STOP_Driving_PreCheck	: 1;

	BYTE STOP_Trans_Front		: 1;
	BYTE STOP_Trans_Rear		: 1;
	BYTE BCR_Driving_OK		    : 1;
	BYTE BCR_Driving_NG		    : 1;
	BYTE BCR_Trans_OK		    : 1;
	BYTE BCR_Trans_NG		    : 1;
	BYTE Reserved_1	    		: 1;
	BYTE Reserved_2	   			: 1;


} STANDARD_OHT_V85_INPUT2_MASK;

typedef struct STANDARD_OHT_V85_INPUT3_MASK_
{
	// 0 - 15
	BYTE Reserved_3	    		: 1;
	BYTE SMPS_Short	            : 1;
	BYTE CID_Link_Compt_IN		: 1;
	BYTE CID_Interlock_IN		: 1;
	BYTE CID_Completion_ACK		: 1;
	BYTE CID_Status_IN			: 1;
	BYTE CID_Reserved_IN1		: 1;
	BYTE CID_Reserved_IN2		: 1;

	BYTE Rotate_0_Sensor	  	: 1;
	BYTE Rotate_90_Sensor	  	: 1;
	BYTE Rotate_180_Sensor	  	: 1;
	BYTE Rotate_270_Sensor	  	: 1;
	BYTE HOIST_Home				: 1;
	BYTE HOIST_Home_2			: 1;		// IO배치는 이 라인부터 틀어진다
	BYTE SHUTTER_F_Middle_Close	: 1;
	BYTE SHUTTER_R_Middle_Close	: 1; 		// Outtrigger_Reserved71;

	BYTE POD_EUV_1				: 1; 		// Outtrigger_Reserved72;
	BYTE POD_EUV_2				: 1;
	BYTE SPARE_5				: 1;
	BYTE SPARE_6				: 1;
	BYTE SPARE_7				: 1;
	BYTE SPARE_8				: 1;
	BYTE SPARE_9				: 1;
	BYTE SPARE_10				: 1;

	BYTE SPARE_11				: 1;
	BYTE SPARE_12				: 1;
	BYTE SPARE_13				: 1;

	BYTE SPARE_14				: 1;
	BYTE SPARE_15				: 1;
	BYTE SPARE_16				: 1;
	BYTE SPARE_17				: 1;
	BYTE SPARE_18				: 1;

}STANDARD_OHT_V85_INPUT3_MASK;

typedef struct STANDARD_OHT_V86_INPUT1_MASK_
{   //10개
	BYTE HAND_Inposition		: 1;
	BYTE HAND_Alarm				: 1;
	BYTE HAND_Right_Grip		: 1;
	BYTE HAND_Left_Grip			: 1;
	BYTE HAND_Close		    	: 1;
	BYTE HAND_Open		    	: 1;
	BYTE HAND_Push		    	: 1;
	BYTE HNAD_Reserved_1        : 1;
	BYTE HNAD_ES         		: 1;
	BYTE HAND_PIO_Go	    	: 1;
	//10개
	BYTE EQPIO_Load_REQ	    	: 1;
	BYTE EQPIO_Unload_REQ   	: 1;
	BYTE EQPIO_Ready	    	: 1;
	BYTE EQPIO_HO_AVBL	    	: 1;
	BYTE EQPIO_ES		    	: 1;
	BYTE EQPIO_Right_Go	    	: 1;
	BYTE EQPIO_Left_Go	    	: 1;
	BYTE STEER_F_Left	    	: 1;
	BYTE STEER_R_Left		    : 1;
	BYTE STEER_F_Right		    : 1;
	//12개
	BYTE STEER_R_Right		    : 1;
	BYTE SHUTTER_F_Close	    : 1;
	BYTE SHUTTER_R_Close	    : 1;
	BYTE SHUTTER_F_Open		    : 1;
	BYTE SHUTTER_R_Open		    : 1;
	BYTE SHUTTER_F_Alarm	    : 1;
	BYTE SHUTTER_R_Alarm	    : 1;
	BYTE SHUTTER_F_Inposition	: 1;
	BYTE SHUTTER_R_Inposition	: 1;
	BYTE OBSTACLE_Input_Extend  : 1;
	BYTE OBSTACLE_Long			: 1;
	BYTE OBSTACLE_Middle		: 1;
}STANDARD_OHT_V86_INPUT1_MASK;



typedef struct STANDARD_OHT_V86_INPUT2_MASK_
{   //10개
	BYTE OBSTACLE_Short			: 1;
	BYTE OBSTACLE_Fail			: 1;
	BYTE OHTDETECT_Input_Extend : 1;
	BYTE OHTDETECT_Long		    : 1;
	BYTE OHTDETECT_Middle	    : 1;
	BYTE OHTDETECT_Short	    : 1;
	BYTE OHTDETECT_Fail		    : 1;
	BYTE LOOKDOWN_Detect	    : 1;
	BYTE LOOKDOWN_Error		    : 1;
	BYTE SHIFT_Home			    : 1;
	//10개
	BYTE STB_Detect_Left	    : 1;
	BYTE STB_Detect_Right	    : 1;
	BYTE UTB_Detect			    : 1;
	BYTE FOUP_Detect		    : 1;
	BYTE OSCILLATION		    : 1;
	BYTE EMO_Press			    : 1;
	BYTE BUMPER_Press		    : 1;
	BYTE FAN_Fail			    : 1;
	BYTE HOIST_Belt_Detect2	    : 1;
	BYTE Reserved_51	    	: 1;
	//12개
	BYTE HANDTP_Connect			: 1;  //TP Enable
	BYTE POD_EUV_1				: 1;
	BYTE STOP_Driving_Front		: 1;
	BYTE STOP_Driving_PreCheck	: 1;
	BYTE STOP_Trans_Front		: 1;
	BYTE STOP_Trans_Rear		: 1;
	BYTE BCR_Driving_OK			: 1;
	BYTE BCR_Driving_NG		    : 1;
	BYTE BCR_Trans_OK		    : 1;
	BYTE BCR_Trans_NG		    : 1;
	BYTE HOIST_Belt_Detect1		: 1;
	BYTE PIO_Input3	    		: 1;

} STANDARD_OHT_V86_INPUT2_MASK;

typedef struct STANDARD_OHT_V86_INPUT3_MASK_
{
	// 10개
	BYTE PIO_Input5	   			: 1;
	BYTE PIO_Input6	    		: 1;
	BYTE SMPS_Short	            : 1;
	BYTE CID_Link_Compt_IN		: 1;
	BYTE CID_Interlock_IN		: 1;
	BYTE CID_Completion_ACK		: 1;
	BYTE CID_Status_IN			: 1;
	BYTE CID_NoRegistration		: 1;
	BYTE CID_Reserved_IN2		: 1;
	BYTE HOIST_Home				: 1;
	//10개
	BYTE Blackbox_DI2			: 1;
	BYTE Blackbox_DI1 			: 1;
	BYTE Blackbox_DI0			: 1;
	BYTE Rotate_0_Sensor	  	: 1;
	BYTE Rotate_90_Sensor	  	: 1;
	BYTE Rotate_180_Sensor	  	: 1;
	BYTE Rotate_270_Sensor	  	: 1;
	BYTE SHUTTER_F_Middle_Close	: 1;
	BYTE SHUTTER_R_Middle_Close	: 1; 		// Outtrigger_Reserved71;
	BYTE SPARE_7					: 1; 		// Outtrigger_Reserved72;

	//12개
	BYTE POD_EUV_2				: 1;
	BYTE Reserved_1				: 1;
	BYTE Reserved_2				: 1;
	BYTE Reserved_3				: 1;
	BYTE SPARE_12				: 1;
	BYTE SPARE_13				: 1;
	BYTE SPARE_14				: 1;
	BYTE SPARE_15				: 1;
	BYTE SPARE_16				: 1;
	BYTE SPARE_17				: 1;
	BYTE SPARE_18				: 1;
	BYTE SPARE_19				: 1;

}STANDARD_OHT_V86_INPUT3_MASK;


// 2016-12-03, Add by Labeler
// Reticle OHT용 Input Mask 추가
typedef struct RETICLE_OHT_V80_INPUT1_MASK_
{
	BYTE HAND_Inposition	: 1;
	BYTE HAND_Alarm			: 1;
	BYTE HAND_Right_Grip	: 1;
	BYTE HAND_Left_Grip		: 1;
	BYTE HAND_Close		    	: 1;
	BYTE HAND_Open		    	: 1;
	BYTE HAND_Push		    	: 1;
    BYTE Reserved_7             : 1;

	BYTE HAND_PIO_Go	    	: 1;
	BYTE EQPIO_Load_REQ	    	: 1;
	BYTE EQPIO_Unload_REQ   	: 1;
	BYTE EQPIO_Ready	    	: 1;
	BYTE EQPIO_HO_AVBL	    	: 1;
	BYTE EQPIO_ES		    	: 1;
	BYTE EQPIO_Right_Go	    	: 1;
	BYTE EQPIO_Left_Go	    	: 1;

	BYTE STEER_F_Left	    	: 1;
	BYTE STEER_R_Left		    : 1;
	BYTE STEER_F_Right		    : 1;
	BYTE STEER_R_Right		    : 1;
	BYTE SHUTTER_F_Close	    : 1;
	BYTE SHUTTER_R_Close	    : 1;
	BYTE SHUTTER_F_Open		    : 1;
	BYTE SHUTTER_R_Open		    : 1;

	BYTE SHUTTER_F_Alarm	    : 1;
	BYTE SHUTTER_R_Alarm	    : 1;
	BYTE SHUTTER_F_Inposition	: 1;
	BYTE SHUTTER_R_Inposition	: 1;
	BYTE OBSTACLE_Long			: 1;
	BYTE OBSTACLE_Middle		: 1;
	BYTE OBSTACLE_Short			: 1;
	BYTE OBSTACLE_Fail			: 1;
}RETICLE_OHT_V80_INPUT1_MASK;


typedef struct RETICLE_EUV_OHT_V80_INPUT1_MASK_
{
	// 0 - 15
	BYTE HAND_Inposition		: 1;
	BYTE HAND_Alarm				: 1;
	BYTE HAND_Right_Grip		: 1;
	BYTE HAND_Left_Grip			: 1;
	
	BYTE HAND_Close				: 1;
	BYTE HAND_Open				: 1;
	BYTE HAND_Push				: 1;
	BYTE HOIST_Align			: 1;		//	 Hand_Reserved7;
	
	BYTE HAND_PIO_Go			: 1;
	BYTE EQPIO_Load_REQ			: 1;			
	BYTE EQPIO_Unload_REQ		: 1;
	BYTE EQPIO_Ready			: 1;
	
	BYTE EQPIO_HO_AVBL			: 1;
	BYTE EQPIO_ES				: 1;
    BYTE EQPIO_Right_Go			: 1;
	BYTE EQPIO_Left_Go			: 1;

	// 16 - 31
	BYTE STEER_F_Left			: 1;
	BYTE STEER_R_Left			: 1;
	BYTE STEER_F_Right			: 1;
	BYTE STEER_R_Right			: 1; 	

	BYTE SHUTTER_F_Close		: 1;
	BYTE SHUTTER_R_Close		: 1;
	BYTE SHUTTER_F_Open			: 1;
	BYTE SHUTTER_R_Open			: 1;
	
	BYTE SHUTTER_F_Alarm		: 1;			
	BYTE SHUTTER_R_Alarm		: 1;
	BYTE SHUTTER_F_Inposition	: 1;			
	BYTE SHUTTER_R_Inposition	: 1;
	
	BYTE OBSTACLE_Long			: 1;
	BYTE OBSTACLE_Middle		: 1;
	BYTE OBSTACLE_Short			: 1;
	BYTE OBSTACLE_Fail			: 1; 
	
}RETICLE_EUV_OHT_V80_INPUT1_MASK;


typedef struct RETICLE_OHT_V80_INPUT2_MASK_
{
	BYTE OHTDETECT_Long		    : 1;
	BYTE OHTDETECT_Middle	    : 1;
	BYTE OHTDETECT_Short	    : 1;
	BYTE OHTDETECT_Fail		    : 1;
	BYTE LOOKDOWN_Detect	    : 1;
	BYTE LOOKDOWN_Error		    : 1;
	BYTE SHIFT_Home			    : 1;
	BYTE STB_Detect_Left	    : 1;
	BYTE STB_Detect_Right	    : 1;
	BYTE UTB_Detect			    : 1;
	BYTE FOUP_Detect		    : 1;
	BYTE OSCILLATION		    : 1;
	BYTE EMO_Press			    : 1;
	BYTE BUMPER_Press		    : 1;
	BYTE FAN_Fail			    : 1;
	BYTE Reserved_48		    : 1;
	BYTE MC_Status			    : 1;
	BYTE HANDTP_Connect		    : 1;
	BYTE FOUP_Cover_Open_Detect	: 1;
	BYTE STOP_Driving_Front	    : 1;
	BYTE STOP_Driving_PreCheck	: 1;
	BYTE STOP_Trans_Front		: 1;
	BYTE STOP_Trans_Rear		: 1;
	BYTE BCR_Driving_OK		    : 1;
	BYTE BCR_Driving_NG		    : 1;
	BYTE BCR_Trans_OK		    : 1;
	BYTE BCR_Trans_NG		    : 1;
	BYTE HOIST_Belt_Detect	    : 1;
	BYTE RailSupport_Home	    : 1;
	BYTE Railsupport_Work	    : 1;
	BYTE MAC_Detect	            : 1;
	BYTE SMPS_Short	            : 1;
} RETICLE_OHT_V80_INPUT2_MASK;

typedef struct RETICLE_EUV_OHT_V80_INPUT2_MASK_
{
	// 0 - 15
			
	BYTE OHTDETECT_Long			: 1;
	BYTE OHTDETECT_Middle		: 1;
	BYTE OHTDETECT_Short		: 1;			
	BYTE OHTDETECT_Fail			: 1;
	
	BYTE LOOKDOWN_Detect		: 1;
	BYTE LOOKDOWN_Error			: 1;
	BYTE SHIFT_Home				: 1;
	BYTE STB_Detect_Left		: 1;	// STB_Left				: 1;		// Reserved40;	
	
	BYTE STB_Detect_Right		: 1;	// STB_Right				: 1; 	// Reserved41;
	BYTE UTB_Detect				: 1;	// Reserved42;
	BYTE FOUP_Detect			: 1;			
	BYTE OSCILLATION			: 1;
	
	BYTE EMO_Press				: 1; 		
	BYTE BUMPER_Press			: 1;
	BYTE FAN_Fail				: 1;
	BYTE HOIST_Belt				: 1;	// Reserved48;			

	// 16 - 31
	BYTE MC_Status				: 1;	// TP_Enable				: 1; 	// Reserved49;
	BYTE HANDTP_Connect			: 1;	
	BYTE FOUP_Cover_Open_Detect : 1;	// FOUB_Cover				: 1;	// Reserved51;
	BYTE STOP_Driving_Front		: 1;
	
	BYTE STOP_Driving_PreCheck	: 1;
	BYTE STOP_Trans_Front		: 1;	
	BYTE STOP_Trans_Rear		: 1;			
	BYTE BCR_Driving_OK			: 1;

	BYTE BCR_Driving_NG			: 1;
	BYTE BCR_Trans_OK			: 1;	
	BYTE BCR_Trans_NG			: 1;
	BYTE HOIST_Belt_Detect		: 1;	// HOIST_Belt_Detect1		: 1;	
	
	BYTE PIO_Input3				: 1;
	BYTE PIO_Input5				: 1;
	BYTE PIO_Input6				: 1;			
	BYTE SMPS_Short				: 1;
	
}RETICLE_EUV_OHT_V80_INPUT2_MASK;

 //TG 추가
typedef struct RETICLE_OHT_V80_INPUT3_MASK
{
	BYTE CID_Link_Compt_IN      : 1;
	BYTE CID_Interlock_IN       : 1;
	BYTE CID_Completion_ACK 	: 1;
	BYTE CID_Status_IN          : 1;
	BYTE CID_Reserved_IN1       : 1;
	BYTE CID_Reserved_IN2       : 1;
	BYTE Rotate_0_Sensor	  	: 1;
	BYTE Rotate_90_Sensor	  	: 1;
	BYTE Rotate_180_Sensor	  	: 1;
	BYTE Rotate_270_Sensor	  	: 1;
	BYTE HOIST_Home			    : 1;
	BYTE Reserved_75            : 1;
	BYTE Rotate_L_Limit         : 1;
	BYTE Rotate_R_Limit         : 1;
	BYTE Rotate_Inposition      : 1;
	BYTE Rotate_Alarm           : 1;
} RETICLE_OHT_V80_INPUT3_MASK;


typedef struct RETICLE_EUV_OHT_V80_INPUT3_MASK_
{
	// 0 - 15

		//TG 추가
	BYTE CID_Link_Compt_IN		: 1;
	BYTE CID_Interlock_IN		: 1;	
	BYTE CID_Completion_ACK		: 1;
	BYTE CID_Status_IN			: 1; 
	
	BYTE CID_Reserved_IN1		: 1;
	BYTE CID_Reserved_IN2		: 1;	
	BYTE Rotate_0_Sensor	  	: 1;
	BYTE Rotate_90_Sensor	  	: 1;

	BYTE Rotate_180_Sensor	  	: 1;
	BYTE Rotate_270_Sensor	  	: 1;
	BYTE HOIST_Home				: 1;
	BYTE SHUTTER_F_Middle_Close	: 1;		// IO배치는 이 라인부터 틀어진다
	
	BYTE SHUTTER_R_Middle_Close	: 1;	
	BYTE POD_EUV_1				: 1; 		// Outtrigger_Reserved71;
	BYTE POD_EUV_2				: 1; 		// Outtrigger_Reserved72;
	BYTE SPARE_5				: 1;

	// 16 - 31
	BYTE SPARE_6				: 1;
	BYTE SPARE_7				: 1;
	BYTE SPARE_8				: 1;
	BYTE SPARE_9				: 1;
	
	BYTE SPARE_10				: 1;
	BYTE SPARE_11				: 1;
	BYTE SPARE_12				: 1;
	BYTE SPARE_13				: 1;
	
	BYTE SPARE_14				: 1;
	BYTE SPARE_15				: 1;			
	BYTE SPARE_16				: 1;
	BYTE SPARE_17				: 1;
	
	BYTE SPARE_18				: 1;
	BYTE SPARE_19				: 1;

}RETICLE_EUV_OHT_V80_INPUT3_MASK;
#endif
