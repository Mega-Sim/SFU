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
	BYTE HOIST_Belt_Detect2	: 1;     //AOHC-286
	BYTE TP_Enable			: 1;     //AOHC-286
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
	BYTE HOIST_Belt_Detect1	: 1;     //AOHC-286
	BYTE PIO_Input3			: 1;     //AOHC-286
	BYTE PIO_Input5	: 1;             //AOHC-286
	BYTE PIO_Input6	: 1;             //AOHC-286
	BYTE SMPS_Short	: 1;
} OHT80_R01_INPUT2_MASK;

 //TG 추가
typedef struct OHT80_R01_INPUT3_MASK_
   {
	BYTE CID_Link_Compt_IN : 1;
	BYTE CID_Interlock_IN : 1;
	BYTE CID_Completion_ACK : 1;
	BYTE CID_Status_IN : 1;
	BYTE CID_NoRegistration : 1;
	BYTE CID_Reserved_IN2 : 1;
	BYTE Rotate_0_Sensor : 1;        //AOHC-286
	BYTE Rotate_90_Sensor : 1;        //AOHC-286
	BYTE Rotate_180_Sensor : 1;       //AOHC-286
	BYTE Rotate_270_Sensor : 1;       //AOHC-286
	BYTE Reverved_74 : 1;             //AOHC-286
	BYTE Reverved_75 : 1;             //AOHC-286
	BYTE Reverved_76 : 1;             //AOHC-286
	BYTE Reverved_77 : 1;             //AOHC-286
	BYTE Reverved_78 : 1;            //AOHC-286
	BYTE Reverved_79 : 1;            //AOHC-286
 } OHT80_R01_INPUT3_MASK;

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

typedef struct RETICLE_OHT_V80_INPUT2_MASK_
{
	BYTE OHTDETECT_Long		    : 1;
	BYTE OHTDETECT_Middle	    : 1;
	BYTE OHTDETECT_Short	    : 1;
	BYTE OHTDETECT_Fail		    : 1;
	BYTE LOOKDOWN_Detect	    : 1;
	BYTE LOOKDOWN_Error		    : 1;
	BYTE SHIFT_Home			    : 1;
	BYTE Reserved_40	   			: 1;      //AOHC-286
	BYTE Reserved_41			    : 1;      //AOHC-286
	BYTE Reserved_42			    : 1;      //AOHC-286
	BYTE FOUP_Detect		    : 1;
	BYTE OSCILLATION		    : 1;
	BYTE EMO_Press			    : 1;
	BYTE BUMPER_Press		    : 1;
	BYTE FAN_Fail			    : 1;
	BYTE Reserved_48		    : 1;
	BYTE Reserved_49		    : 1;
	BYTE HANDTP_Connect		    : 1;
	BYTE Reserved_51			: 1;
	BYTE STOP_Driving_Front	    : 1;
	BYTE STOP_Driving_PreCheck	: 1;
	BYTE STOP_Trans_Front		: 1;
	BYTE STOP_Trans_Rear		: 1;
	BYTE BCR_Driving_OK		    : 1;
	BYTE BCR_Driving_NG		    : 1;
	BYTE BCR_Trans_OK		    : 1;
	BYTE BCR_Trans_NG		    : 1;
	BYTE HOIST_Belt_Detect1		: 1;     //AOHC-286
	BYTE PIO_Input3				: 1;     //AOHC-286
	BYTE PIO_Input5				: 1;     //AOHC-286
	BYTE PIO_Input6				: 1;      //AOHC-286
	BYTE SMPS_Short	            : 1;
} RETICLE_OHT_V80_INPUT2_MASK;

 typedef struct RETICLE_OHT_V80_INPUT3_MASK
{
	BYTE CID_Link_Compt_IN      : 1;
	BYTE CID_Interlock_IN       : 1;
	BYTE CID_Completion_ACK 	: 1;
	BYTE CID_Status_IN          : 1;
	BYTE CID_NoRegistration       : 1;
	BYTE CID_Reserved_IN2       : 1;
	BYTE Rotate_0_Sensor	  	: 1;
	BYTE Rotate_90_Sensor	  	: 1;
	BYTE Rotate_180_Sensor	  	: 1;
	BYTE Rotate_270_Sensor	  	: 1;
	BYTE HOIST_Home			    : 1;
	BYTE Reserved_75            : 1;
	BYTE Reserved_76         	: 1;  //AOHC-286
	BYTE Reserved_77         	: 1;   //AOHC-286
	BYTE Reserved_78      		: 1;   //AOHC-286
	BYTE Reserved_79           	: 1;   //AOHC-286
} RETICLE_OHT_V80_INPUT3_MASK;


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
	BYTE Reserved_7				: 1;	//AOHC-286

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
	BYTE Reserved_40	   		: 1;      //AOHC-286
	BYTE Reserved_41			: 1;      //AOHC-286
	BYTE Reserved_42			: 1;      //AOHC-286
	BYTE FOUP_Detect			: 1;
	BYTE OSCILLATION			: 1;

	BYTE EMO_Press				: 1;
	BYTE BUMPER_Press			: 1;
	BYTE FAN_Fail				: 1;
	BYTE Reserved_48			: 1; 	 //AOHC-286

	// 16 - 31
	BYTE Reserved_49	  		: 1;	 //AOHC-286
	BYTE HANDTP_Connect			: 1;
	BYTE Reserved_51		 	: 1;	 //AOHC-286;
	BYTE STOP_Driving_Front		: 1;

	BYTE STOP_Driving_PreCheck	: 1;
	BYTE STOP_Trans_Front		: 1;
	BYTE STOP_Trans_Rear		: 1;
	BYTE BCR_Driving_OK			: 1;

	BYTE BCR_Driving_NG			: 1;
	BYTE BCR_Trans_OK			: 1;
	BYTE BCR_Trans_NG			: 1;
	BYTE HOIST_Belt_Detect1		: 1;     //AOHC-286

	BYTE PIO_Input3				: 1;
	BYTE PIO_Input5				: 1;
	BYTE PIO_Input6				: 1;
	BYTE SMPS_Short				: 1;

}RETICLE_EUV_OHT_V80_INPUT2_MASK;

 //TG 추가



typedef struct RETICLE_EUV_OHT_V80_INPUT3_MASK_
{
	// 0 - 15

		//TG 추가
	BYTE CID_Link_Compt_IN		: 1;
	BYTE CID_Interlock_IN		: 1;
	BYTE CID_Completion_ACK		: 1;
	BYTE CID_Status_IN			: 1;

	BYTE CID_NoRegistration		: 1;
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
	BYTE Reserved_79           	: 1;   //AOHC-286

}RETICLE_EUV_OHT_V80_INPUT3_MASK;

typedef struct FOUP_V81_INPUT1_MASK_
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
	BYTE HOIST_Belt_Detect2	: 1;   //AOHC-286
	BYTE TP_Enable			: 1;   //AOHC-286
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
	BYTE HOIST_Belt_Detect1	: 1;     //AOHC-286
	BYTE PIO_Input3			: 1;     //AOHC-286
	BYTE PIO_Input5	: 1;             //AOHC-286
	BYTE PIO_Input6	: 1;             //AOHC-286
	BYTE SMPS_Short	: 1;
} FOUP_V81_INPUT2_MASK;

 //TG 추가
typedef struct FOUP_V81_INPUT3_MASK_
{
	BYTE CID_Link_Compt_IN : 1;
	BYTE CID_Interlock_IN : 1;
	BYTE CID_Completion_ACK : 1;
	BYTE CID_Status_IN : 1;
	BYTE CID_NoRegistration : 1;
	BYTE CID_Reserved_IN2 : 1;
	BYTE Rotate_0_Sensor : 1;        //AOHC-286
	BYTE Rotate_90_Sensor : 1;        //AOHC-286
	BYTE Rotate_180_Sensor : 1;       //AOHC-286
	BYTE Rotate_270_Sensor : 1;       //AOHC-286
	BYTE BlackBox_Error : 1;             //AOHC-286
	BYTE Reverved_75 : 1;             //AOHC-286
	BYTE Reverved_76 : 1;             //AOHC-286
	BYTE Reverved_77 : 1;             //AOHC-286
	BYTE Reverved_78 : 1;            //AOHC-286
	BYTE Reverved_79 : 1;            //AOHC-286
 } FOUP_V81_INPUT3_MASK;

 // 2022-03-22,
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

}STANDARD_OHT_V86_INPUT3_MASK;



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
	BYTE CID_NoRegistration		: 1;
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
	BYTE CID_NoRegistration		: 1;
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


typedef struct STANDARD_OHT_V85S_INPUT1_MASK_
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
	BYTE OBSTACLE_Input_Extend  : 1;
	BYTE OBSTACLE_Long			: 1;
	BYTE OBSTACLE_Middle		: 1;


}STANDARD_OHT_V85S_INPUT1_MASK;



typedef struct STANDARD_OHT_V85S_INPUT2_MASK_
{
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
	BYTE STB_Detect_Left	    : 1;
	BYTE STB_Detect_Right	    : 1;
	BYTE UTB_Detect			    : 1;
	BYTE FOUP_Detect		    : 1;
	BYTE OSCILLATION		    : 1;
	BYTE EMO_Press			    : 1;
	BYTE BUMPER_Press		    : 1;

	BYTE FAN_Fail			    : 1;
	BYTE HOIST_Belt_Detect2	    : 1;
	BYTE Reserved_51	    : 1;
	BYTE HANDTP_Connect				: 1;  //TP Enable
	BYTE FOUP_Cover_Open_Detect		    : 1;
	BYTE STOP_Driving_Front	: 1;
	BYTE STOP_Driving_PreCheck	    : 1;
	BYTE STOP_Trans_Front		: 1;

	BYTE STOP_Trans_Rear		: 1;
	BYTE BCR_Driving_OK			: 1;
	BYTE BCR_Driving_NG		    : 1;
	BYTE BCR_Trans_OK		    : 1;
	BYTE BCR_Trans_NG		    : 1;
	BYTE HOIST_Belt_Detect1		: 1;
	BYTE PIO_Input3	    		: 1;

} STANDARD_OHT_V85S_INPUT2_MASK;

typedef struct STANDARD_OHT_V85S_INPUT3_MASK_
{
	// 0 - 15
	BYTE PIO_Input5	   			: 1;
	BYTE PIO_Input6	    		: 1;
	BYTE SMPS_Short	            : 1;
	BYTE CID_Link_Compt_IN		: 1;
	BYTE CID_Interlock_IN		: 1;
	BYTE CID_Completion_ACK		: 1;
	BYTE CID_Status_IN			: 1;
	BYTE CID_NoRegistration		: 1;
	BYTE CID_Reserved_IN2		: 1;


	BYTE HOIST_Home				:1;
	BYTE Blackbox_DI2			:1;
	BYTE Blackbox_DI1 			:1;
	BYTE Blackbox_DI0			:1;

	BYTE SPARE_1				: 1;
	BYTE SPARE_2				: 1;
	BYTE SPARE_3				: 1;
	BYTE SPARE_4				: 1;
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
	BYTE SPARE_19				: 1;

}STANDARD_OHT_V85S_INPUT3_MASK;

typedef struct STANDARD_OHT_V90_INPUT1_MASK_
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
	BYTE OBSTACLE_Input_Extend  : 1;
	BYTE OBSTACLE_Long			: 1;
	BYTE OBSTACLE_Middle		: 1;


}STANDARD_OHT_V90_INPUT1_MASK;



typedef struct STANDARD_OHT_V90_INPUT2_MASK_
{
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
	BYTE STB_Detect_Left	    : 1;
	BYTE STB_Detect_Right	    : 1;
	BYTE UTB_Detect			    : 1;
	BYTE FOUP_Detect		    : 1;
	BYTE OSCILLATION		    : 1;
	BYTE EMO_Press			    : 1;
	BYTE BUMPER_Press		    : 1;

	BYTE FAN_Fail			    : 1;
	BYTE HOIST_Belt_Detect2	    : 1;
	BYTE Reserved_51	    : 1;
	BYTE HANDTP_Connect				: 1;  //TP Enable
	BYTE FOUP_Cover_Open_Detect		    : 1;
	BYTE STOP_Driving_Front	: 1;
	BYTE STOP_Driving_PreCheck	    : 1;
	BYTE STOP_Trans_Front		: 1;

	BYTE STOP_Trans_Rear		: 1;
	BYTE BCR_Driving_OK			: 1;
	BYTE BCR_Driving_NG		    : 1;
	BYTE BCR_Trans_OK		    : 1;
	BYTE BCR_Trans_NG		    : 1;
	BYTE HOIST_Belt_Detect1		: 1;
	BYTE PIO_Input3	    		: 1;

} STANDARD_OHT_V90_INPUT2_MASK;

typedef struct STANDARD_OHT_V90_INPUT3_MASK_
{
	// 0 - 15
	BYTE PIO_Input5	   			: 1;
	BYTE PIO_Input6	    		: 1;
	BYTE SMPS_Short	            : 1;
	BYTE CID_Link_Compt_IN		: 1;
	BYTE CID_Interlock_IN		: 1;
	BYTE CID_Completion_ACK		: 1;
	BYTE CID_Status_IN			: 1;
	BYTE CID_NoRegistration		: 1;
	BYTE CID_Reserved_IN2		: 1;


	BYTE HOIST_Home				:1;
	BYTE Blackbox_DI2			:1;
	BYTE Blackbox_DI1 			:1;
	BYTE Blackbox_DI0			:1;

	BYTE SPARE_1				: 1;
	BYTE SPARE_2				: 1;
	BYTE SPARE_3				: 1;
	BYTE SPARE_4				: 1;
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
	BYTE SPARE_19				: 1;

}STANDARD_OHT_V90_INPUT3_MASK;
// 2016-12-03, Add by Labeler


//mgchoi. 2022.08.23 P4 MaskOHT용 Mask InputMask 정의
typedef struct P4_MASK_OHT_INPUT1_MASK_
{
	BYTE HAND_Flange_Check       : 1;
	BYTE HAND_Alarm              : 1;
	BYTE HAND_Right_Grip         : 1;
	BYTE HAND_Left_Grip          : 1;
	BYTE HAND_Work               : 1;
	BYTE HAND_Home               : 1;
	BYTE HAND_LoadPosition       : 1;
	BYTE HAND_UnloadPosition     : 1;

	BYTE HAND_ES			     : 1;
	BYTE HAND_PIO_Go             : 1;
	BYTE EQPIO_Load_REQ          : 1;
	BYTE EQPIO_Unload_REQ        : 1;
	BYTE EQPIO_Ready             : 1;
	BYTE EQPIO_HO_AVBL           : 1;
	BYTE EQPIO_ES                : 1;
	BYTE EQPIO_Right_Go          : 1;

	BYTE EQPIO_Left_Go           : 1;
	BYTE STEER_F_Left            : 1;
	BYTE STEER_R_Left            : 1;
	BYTE STEER_F_Right           : 1;
	BYTE STEER_R_Right           : 1;
	BYTE SHUTTER_F_Close         : 1;
	BYTE ANTIDROP_POD_Placement1 : 1;
	BYTE SHUTTER_F_Open          : 1;

	BYTE ANTIDROP_POD_Placement2 : 1;
	BYTE SHUTTER_F_Alarm         : 1;
	BYTE Reserved_27             : 1;
	BYTE SHUTTER_F_Inposition    : 1;
	BYTE Reserved_29             : 1;
	BYTE Reserved_30             : 1;
	BYTE OBSTACLE_Long           : 1;
	BYTE OBSTACLE_Middle         : 1;
}P4_MASK_OHT_INPUT1_MASK;

typedef struct P4_MASK_OHT_INPUT2_MASK_
{
	BYTE OBSTACLE_Short          : 1;
	BYTE OBSTACLE_Fail           : 1;
	BYTE Reserved_35	         : 1;
	BYTE OHTDETECT_Long          : 1;
    BYTE OHTDETECT_Middle        : 1;
	BYTE OHTDETECT_Short         : 1;
    BYTE OHTDETECT_Fail          : 1;
	BYTE LOOKDOWN_Detect         : 1;

	BYTE LOOKDOWN_Error          : 1;
	BYTE FFU_Fail  		         : 1;
	BYTE Reserved_43 			 : 1;
	BYTE Reserved_44			 : 1;
	BYTE UTB_Detect              : 1;
	BYTE EUV_POD_Detect          : 1;
	BYTE OSCILLATION             : 1;
	BYTE EMO_Press               : 1;

	BYTE BUMPER_Press            : 1;
	BYTE FFU_Fail_2              : 1;
	BYTE SLIDE_LIMIT_NEG         : 1;
	BYTE Reserved_52             : 1;
	BYTE Reserved_53             : 1;
	BYTE RSP200_Detect           : 1;
	BYTE STOP_Driving_Front      : 1;
	BYTE STOP_Driving_PreCheck   : 1;

	BYTE STOP_Trans_Front        : 1;
	BYTE STOP_Trans_Rear         : 1;
	BYTE BCR_Driving_OK          : 1;
	BYTE BCR_Driving_NG          : 1;
	BYTE BCR_Trans_OK            : 1;
	BYTE BCR_Trans_NG            : 1;
	BYTE SLIDE_LIMIT_POS         : 1;
	BYTE Reserved_64             : 1;

} P4_MASK_OHT_INPUT2_MASK;

typedef struct P4_MASK_OHT_INPUT3_MASK_
{
	BYTE Reserved_65             : 1;
	BYTE Reserved_66             : 1;
	BYTE SMPS_Short				 : 1;
	BYTE HOIST_Home              : 1;
	BYTE HOIST_EUV_POD_Home      : 1;
	BYTE HOIST_RSP200_Home       : 1;
	BYTE HOIST_RSP150_Home       : 1;
	BYTE Reserved_72             : 1;

	BYTE Reserved_73             : 1;
	BYTE SHIFT_Home              : 1;
	BYTE Reserved_75             : 1;
	BYTE Reserved_76             : 1;
	BYTE Reserved_77             : 1;
	BYTE POD_Detect              : 1;
	BYTE RSP150_Detect           : 1;
	BYTE ANTIDROP_POD_Placement3 : 1;
	BYTE ANTIDROP_POD_Placement4 : 1;

	BYTE CID_Link_Compt_IN		: 1;
	BYTE CID_Interlock_IN		: 1;
	BYTE CID_Completion_ACK		: 1;
	BYTE CID_Status_IN			: 1;
	BYTE CID_NoRegistration		: 1;
	BYTE CID_Reserved_IN2		: 1;

	BYTE PIO_Input3			: 1;     //AOHC-286
	BYTE PIO_Input5	: 1;             //AOHC-286
	BYTE PIO_Input6	: 1;             //AOHC-286
} P4_MASK_OHT_INPUT3_MASK;
#endif
