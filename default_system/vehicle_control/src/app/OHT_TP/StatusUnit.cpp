//---------------------------------------------------------------------------

#include <string>
#include <vcl.h>
#include <mmsystem.h>
#include <stdio.h>
#include <IniFiles.hpp>
#pragma hdrstop

#include "StatusUnit.h"
#include "OHTInfoUnit.h"
#include "MainUnit.h"
#include "MapMakerUnit.h"
#include "FoupOnlineCMDUnit.h"
#include "EUVOnlineCMDUnit.h"
#include "IOMapUnit.h"
#include "Define.h"
#include "FileUnit.h"
#include "IOTest.h"
#include "PIOUnit.h"
#include "OHTSysInfoUnit.h"
#include "Def_TPCommand.h"
#include "WaitUnit.h"
#include "CMDControlUnit.h"
#include "TeachingUnit.h"
#include "Utility.h"
#include "ConversionUnit.h"
#include "HoistParamUnit.h"
#include "ErrorUnit.h"
#include "E23PIOUnit.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TStatusForm *StatusForm;
//---------------------------------------------------------------------------
__fastcall TStatusForm::TStatusForm(TComponent* Owner)
   : TForm(Owner)
{
   bReadFlg = false;
   g_dwTransStartTime=0;

}
//---------------------------------------------------------------------------
void __fastcall TStatusForm::FormShow(TObject *Sender)
{
    StatusForm->Top = MainForm->Top;
    StatusForm->Left = MainForm->Left + MainForm->Width;

	tmDisplayUpdate->Interval = 50;
    tmDisplayUpdate->Enabled = true;
}
//---------------------------------------------------------------------------
int TStatusForm::Set_UBG_Output(int nOut1, int nOut2, int nOut3, int nFail)
{
   int nError=0;

/*
   if((nOut1==OFF)&&(nOut2==ON)&&(nOut3==ON)) gm_OHTStatus->nUBGDetectLevel=1;
   else if((nOut1==ON)&&(nOut2==OFF)&&(nOut3==ON)) gm_OHTStatus->nUBGDetectLevel=2;
   else if((nOut1==OFF)&&(nOut2==OFF)&&(nOut3==ON)) gm_OHTStatus->nUBGDetectLevel=3;
   else if((nOut1==ON)&&(nOut2==ON)&&(nOut3==OFF)) gm_OHTStatus->nUBGDetectLevel=4;
   else if((nOut1==OFF)&&(nOut2==ON)&&(nOut3==OFF)) gm_OHTStatus->nUBGDetectLevel=5;
   else if((nOut1==ON)&&(nOut2==OFF)&&(nOut3==OFF)) gm_OHTStatus->nUBGDetectLevel=6;
   else if((nOut1==OFF)&&(nOut2==OFF)&&(nOut3==OFF)) gm_OHTStatus->nUBGDetectLevel=-1;
*/

   if(nFail==ON) gm_OHTStatus->nUBGDetectLevel=-1; //fail

   else if((nOut1==OFF)&&(nOut2==ON)&&(nOut3==ON)) gm_OHTStatus->nUBGDetectLevel=1;

   else if((nOut1==OFF)&&(nOut2==OFF)&&(nOut3==ON)) gm_OHTStatus->nUBGDetectLevel=2;

   else if((nOut1==ON)&&(nOut2==OFF)&&(nOut3==ON)) gm_OHTStatus->nUBGDetectLevel=3;

   else if((nOut1==ON)&&(nOut2==OFF)&&(nOut3==OFF)) gm_OHTStatus->nUBGDetectLevel=4;

   else if((nOut1==OFF)&&(nOut2==OFF)&&(nOut3==OFF)) gm_OHTStatus->nUBGDetectLevel=5;

   else if((nOut1==OFF)&&(nOut2==ON)&&(nOut3==OFF)) gm_OHTStatus->nUBGDetectLevel=6;

   else gm_OHTStatus->nUBGDetectLevel=0; //미감지

   return nError;
}
//---------------------------------------------------------------------------
/**
@brief   OHT로부터 받은 IO Data를 Analysis하는 함수
@author  puting
@date    2012.12.14
*/
void TStatusForm::Analysis_AMC_IOData()
{
    UINT nInput[3], nOutput[3];     //7.0

    for(int i=0; i<3; i++)
    {
        // by zzang9un 2013.03.19 : IO Data 가져오기
		nInput[i]  = gm_OHTStatus->m_OHTStatus.Input[i];
        nOutput[i] = gm_OHTStatus->m_OHTStatus.Output[i];
    }

    for (int i = 0; i < 3; i++)
    {
   	    for (int j = 0; j < 32; j++)
        {
			gm_OHTStatus->IOInput->nInData[i][j] = ((nInput[i] >> j) & 0x01) ? 0x01 : 0x00;
            gm_OHTStatus->IOOutput->nOutData[i][j] = ((nOutput[i] >> j) & 0x01) ? 0x01 : 0x00;
        }
    }

	// by zzang9un 2012.11.26 : 입출력의 경우 구조체에 반영함
	for (int k = 0; k < 96; k++)
	{
		if(k < 32)
		{
			gm_OHTStatus->IOInput->uInput.InputCommon.INPUT[k] = ((nInput[0] >> k) & 0x01) ? 0x01 : 0x00;
			gm_OHTStatus->IOOutput->uOutput.OutputCommon.OUTPUT[k] = ((nOutput[0] >> k) & 0x01) ? 0x01 : 0x00;
		}
		else if((k >= 32)&&(k < 64))
		{
			gm_OHTStatus->IOInput->uInput.InputCommon.INPUT[k] = ((nInput[1] >> (k-32)) & 0x01) ? 0x01 : 0x00;
			gm_OHTStatus->IOOutput->uOutput.OutputCommon.OUTPUT[k] = ((nOutput[1] >> (k-32)) & 0x01) ? 0x01 : 0x00;
		}
		else if((k >= 64)&&(k < 96))
		{
			gm_OHTStatus->IOInput->uInput.InputCommon.INPUT[k] = ((nInput[2] >> (k-64)) & 0x01) ? 0x01 : 0x00;
			gm_OHTStatus->IOOutput->uOutput.OutputCommon.OUTPUT[k] = ((nOutput[2] >> (k-64)) & 0x01) ? 0x01 : 0x00;
		}
	}

	//Type별 IO 정보를 공통으로 사용하는 I.O로 맵핑
	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_80) && (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP))
	 {
//			gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition  =gm_OHTStatus->IOInput->uInput.InputV70_R01.HAND_Inposition;
		//추가할것.

         	 	////AOHC-286
     //Input
		gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition                           = gm_OHTStatus->IOInput->uInput.Input_V80.HAND_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm                                = gm_OHTStatus->IOInput->uInput.Input_V80.HAND_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip                           = gm_OHTStatus->IOInput->uInput.Input_V80.HAND_Right_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip                            = gm_OHTStatus->IOInput->uInput.Input_V80.HAND_Left_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close                                = gm_OHTStatus->IOInput->uInput.Input_V80.HAND_Close;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open                                 = gm_OHTStatus->IOInput->uInput.Input_V80.HAND_Open;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Push                                 = gm_OHTStatus->IOInput->uInput.Input_V80.HAND_Push;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home		                          = gm_OHTStatus->IOInput->uInput.Input_V80.HOIST_Home;		// 미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_ES                                   = 0;//gm_OHTStatus->IOInput->uInput.Input_V80.HNAD_ES;               // 미사용
		gm_OHTStatus->IOInput->uInputCommon.HAND_PIO_Go                               = gm_OHTStatus->IOInput->uInput.Input_V80.HAND_PIO_Go;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ                            = gm_OHTStatus->IOInput->uInput.Input_V80.EQPIO_Load_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ                          = gm_OHTStatus->IOInput->uInput.Input_V80.EQPIO_Unload_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready                               = gm_OHTStatus->IOInput->uInput.Input_V80.EQPIO_Ready;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL                             = gm_OHTStatus->IOInput->uInput.Input_V80.EQPIO_HO_AVBL;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES                                  = gm_OHTStatus->IOInput->uInput.Input_V80.EQPIO_ES;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Left_Go   		                  = gm_OHTStatus->IOInput->uInput.Input_V80.EQPIO_Left_Go;   		//	BYTE EQPIO_Right_Go;미사용
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left                              = gm_OHTStatus->IOInput->uInput.Input_V80.STEER_F_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left                              = gm_OHTStatus->IOInput->uInput.Input_V80.STEER_R_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right                             = gm_OHTStatus->IOInput->uInput.Input_V80.STEER_F_Right;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right                             = gm_OHTStatus->IOInput->uInput.Input_V80.STEER_R_Right;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close                           = gm_OHTStatus->IOInput->uInput.Input_V80.SHUTTER_F_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close                           = gm_OHTStatus->IOInput->uInput.Input_V80.SHUTTER_R_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open                            = gm_OHTStatus->IOInput->uInput.Input_V80.SHUTTER_F_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open                            = gm_OHTStatus->IOInput->uInput.Input_V80.SHUTTER_R_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V80.SHUTTER_F_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V80.SHUTTER_R_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V80.SHUTTER_F_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V80.SHUTTER_R_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Input_Extend    	              = 0;//gm_OHTStatus->IOInput->uInput.Input_V80.OBSTACLE_Input_Extend;     	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Long                             = gm_OHTStatus->IOInput->uInput.Input_V80.OBSTACLE_Long;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Middle                           = gm_OHTStatus->IOInput->uInput.Input_V80.OBSTACLE_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Short                            = gm_OHTStatus->IOInput->uInput.Input_V80.OBSTACLE_Short;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Fail                             = gm_OHTStatus->IOInput->uInput.Input_V80.OBSTACLE_Fail;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Input_Extend                    = 0;//gm_OHTStatus->IOInput->uInput.Input_V80.OHTDETECT_Input_Extend;   	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Long                            = gm_OHTStatus->IOInput->uInput.Input_V80.OHTDETECT_Long;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Middle                          = gm_OHTStatus->IOInput->uInput.Input_V80.OHTDETECT_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Short                           = gm_OHTStatus->IOInput->uInput.Input_V80.OHTDETECT_Short;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Fail                            = gm_OHTStatus->IOInput->uInput.Input_V80.OHTDETECT_Fail;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect                           = gm_OHTStatus->IOInput->uInput.Input_V80.LOOKDOWN_Detect;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error                            = gm_OHTStatus->IOInput->uInput.Input_V80.LOOKDOWN_Error;
		gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home                                = gm_OHTStatus->IOInput->uInput.Input_V80.SHIFT_Home;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left                           = gm_OHTStatus->IOInput->uInput.Input_V80.STB_Detect_Left;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right                          = gm_OHTStatus->IOInput->uInput.Input_V80.STB_Detect_Right;
		gm_OHTStatus->IOInput->uInputCommon.UTB_Detect                                = gm_OHTStatus->IOInput->uInput.Input_V80.UTB_Detect;				// 미사용
		gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect                               = gm_OHTStatus->IOInput->uInput.Input_V80.FOUP_Detect;
		gm_OHTStatus->IOInput->uInputCommon.OSCILLATION                               = gm_OHTStatus->IOInput->uInput.Input_V80.OSCILLATION;
		gm_OHTStatus->IOInput->uInputCommon.EMO_Press                                 = gm_OHTStatus->IOInput->uInput.Input_V80.EMO_Press;
		gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press                              = gm_OHTStatus->IOInput->uInput.Input_V80.BUMPER_Press;
		gm_OHTStatus->IOInput->uInputCommon.FAN_Fail                                  = gm_OHTStatus->IOInput->uInput.Input_V80.FAN_Fail;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect1                        = gm_OHTStatus->IOInput->uInput.Input_V80.HOIST_Belt_Detect1;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect2                        = gm_OHTStatus->IOInput->uInput.Input_V80.HOIST_Belt_Detect2;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.TP_Enable                                 = gm_OHTStatus->IOInput->uInput.Input_V80.TP_Enable;				// 미사용;
		gm_OHTStatus->IOInput->uInputCommon.HANDTP_Connect                            = gm_OHTStatus->IOInput->uInput.Input_V80.HANDTP_Connect;
		gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect                         = gm_OHTStatus->IOInput->uInput.Input_V80.FOUP_Cover_Open_Detect;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_Front                        = gm_OHTStatus->IOInput->uInput.Input_V80.STOP_Driving_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_PreCheck                     = gm_OHTStatus->IOInput->uInput.Input_V80.STOP_Driving_PreCheck;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Front                          = gm_OHTStatus->IOInput->uInput.Input_V80.STOP_Trans_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Rear                           = gm_OHTStatus->IOInput->uInput.Input_V80.STOP_Trans_Rear;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_OK                            = gm_OHTStatus->IOInput->uInput.Input_V80.BCR_Driving_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_NG                            = gm_OHTStatus->IOInput->uInput.Input_V80.BCR_Driving_NG;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_OK                              = gm_OHTStatus->IOInput->uInput.Input_V80.BCR_Trans_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_NG                              = gm_OHTStatus->IOInput->uInput.Input_V80.BCR_Trans_NG;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_1                                = gm_OHTStatus->IOInput->uInput.Input_V80.PIO_Input3;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_2                                = gm_OHTStatus->IOInput->uInput.Input_V80.PIO_Input5;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_3                                = gm_OHTStatus->IOInput->uInput.Input_V80.PIO_Input6;
		gm_OHTStatus->IOInput->uInputCommon.SMPS_Short                                = gm_OHTStatus->IOInput->uInput.Input_V80.SMPS_Short;
		gm_OHTStatus->IOInput->uInputCommon.CID_Link_Compt_IN                         = gm_OHTStatus->IOInput->uInput.Input_V80.CID_Link_Compt_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Interlock_IN                          = gm_OHTStatus->IOInput->uInput.Input_V80.CID_Interlock_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Pass_OK_IN                            = gm_OHTStatus->IOInput->uInput.Input_V80.CID_Completion_ACK;
		gm_OHTStatus->IOInput->uInputCommon.CID_Status_IN                             = gm_OHTStatus->IOInput->uInput.Input_V80.CID_Status_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN1                          = gm_OHTStatus->IOInput->uInput.Input_V80.CID_Reserved_IN1;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN2                          = gm_OHTStatus->IOInput->uInput.Input_V80.CID_Reserved_IN2;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_0_Sensor                           = gm_OHTStatus->IOInput->uInput.Input_V80.Rotate_0_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_90_Sensor                          = gm_OHTStatus->IOInput->uInput.Input_V80.Rotate_90_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_180_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V80.Rotate_180_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_270_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V80.Rotate_270_Sensor; //미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_Reserved_1                           = gm_OHTStatus->IOInput->uInput.Input_V80.Reverved_74;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home_2                              = gm_OHTStatus->IOInput->uInput.Input_V80.Reverved_75;     // 미사용
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V80.Reverved_76;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V80.Reverved_77;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_1                                 = gm_OHTStatus->IOInput->uInput.Input_V80.Reverved_78;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_2                                 = gm_OHTStatus->IOInput->uInput.Input_V80.Reverved_79;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_5                                   = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_5;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_6                                   = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_6;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_7                                   = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_7;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_8                                   = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_8;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_9                                   = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_9;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_10                                  = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_10;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_11                                  = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_11;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_12                                  = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_12;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_13                                  = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_13;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_14                                  = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_14;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_15                                  = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_15;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_16                                  = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_16;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_17                                  = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_17;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_18                                  = gm_OHTStatus->IOInput->uInput.Input_V80.SPARE_18;

		//Output
        gm_OHTStatus->IOOutput->uOutputCommon.HAND_Close                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.HAND_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Open                                    = gm_OHTStatus->IOOutput->uOutput.Output_V80.HAND_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Origin                                  = gm_OHTStatus->IOOutput->uOutput.Output_V80.HAND_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reset                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.HAND_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Free                                    = gm_OHTStatus->IOOutput->uOutput.Output_V80.HAND_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved6                               = gm_OHTStatus->IOOutput->uOutput.Output_V80.HAND_Reserved6;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved7                               = gm_OHTStatus->IOOutput->uOutput.Output_V80.HAND_Reserved7;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved8                               = gm_OHTStatus->IOOutput->uOutput.Output_V80.HAND_Reserved8;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_HandTrigger                             = 0;//gm_OHTStatus->IOOutput->uOutput.Output_V80.HAND_HandTrigger;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_PIO_Select                              = gm_OHTStatus->IOOutput->uOutput.Output_V80.HAND_PIO_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid                                  = gm_OHTStatus->IOOutput->uOutput.Output_V80.EQPIO_Valid;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.EQPIO_CS_0;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.EQPIO_CS_1;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_2                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.EQPIO_CS_2;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_3                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.EQPIO_CS_3;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req                                 = gm_OHTStatus->IOOutput->uOutput.Output_V80.EQPIO_TR_Req;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.EQPIO_Busy;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete                               = gm_OHTStatus->IOOutput->uOutput.Output_V80.EQPIO_Complete;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select                            = gm_OHTStatus->IOOutput->uOutput.Output_V80.EQPIO_Left_Select;      //	BYTE EQPIO_Right_Select; 미사용
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V80.STEER_F_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V80.STEER_F_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V80.STEER_R_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V80.STEER_R_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V80.STEER_F_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V80.STEER_R_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V80.SHUTTER_F_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V80.SHUTTER_R_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V80.SHUTTER_F_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V80.SHUTTER_R_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V80.SHUTTER_F_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V80.SHUTTER_R_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V80.SHUTTER_F_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V80.SHUTTER_R_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V80.SHUTTER_F_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V80.SHUTTER_R_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80.OBSTACLE_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80.OBSTACLE_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80.OBSTACLE_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80.OBSTACLE_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area5_Sel                           = 0;//gm_OHTStatus->IOOutput->uOutput.Output_V80.OBSTACLE_Area5_Sel;     //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V80.OBSTACLE_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V80.OHTDETECT_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V80.OHTDETECT_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V80.OHTDETECT_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V80.OHTDETECT_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area5_Sel                          = 0;//gm_OHTStatus->IOOutput->uOutput.Output_V80.OHTDETECT_Area5_Sel;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Power_Reset                        = gm_OHTStatus->IOOutput->uOutput.Output_V80.OHTDETECT_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80.LOOKDOWN_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80.LOOKDOWN_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80.LOOKDOWN_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80.LOOKDOWN_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V80.LOOKDOWN_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Red                                     = gm_OHTStatus->IOOutput->uOutput.Output_V80.LAMP_Red;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Right                            = gm_OHTStatus->IOOutput->uOutput.Output_V80.LAMP_Yellow_Right;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Left                             = gm_OHTStatus->IOOutput->uOutput.Output_V80.LAMP_Yellow_Left;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.LAMP_Green;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset                             = gm_OHTStatus->IOOutput->uOutput.Output_V80.HAND_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.BRAKE_Driving_Release                        = gm_OHTStatus->IOOutput->uOutput.Output_V80.BRAKE_Driving_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Bumper_Clear                                 = gm_OHTStatus->IOOutput->uOutput.Output_V80.Bumper_Clear;
		gm_OHTStatus->IOOutput->uOutputCommon.SAFETY_Reset                                 = gm_OHTStatus->IOOutput->uOutput.Output_V80.SAFETY_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HOIST_Ext_Brake_Release                      = gm_OHTStatus->IOOutput->uOutput.Output_V80.HOIST_Ext_Brake_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.QRR_Power                                    = gm_OHTStatus->IOOutput->uOutput.Output_V80.QRR_Power;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved62                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_60;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved63                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_61;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved64                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_62;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved65                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_63;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON                           = gm_OHTStatus->IOOutput->uOutput.Output_V80.CID_Left_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON                          = gm_OHTStatus->IOOutput->uOutput.Output_V80.CID_Right_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy                                = gm_OHTStatus->IOOutput->uOutput.Output_V80.CID_Occupancy;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion                          = gm_OHTStatus->IOOutput->uOutput.Output_V80.CID_Pass_Completion;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal                                 = gm_OHTStatus->IOOutput->uOutput.Output_V80.CID_Abnormal;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode                           = gm_OHTStatus->IOOutput->uOutput.Output_V80.CID_Operation_Mode;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag                               = gm_OHTStatus->IOOutput->uOutput.Output_V80.CID_Common_Tag; 		//동일 lane link 금지
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Reserved_OUT2                            = gm_OHTStatus->IOOutput->uOutput.Output_V80.CID_Reserved_OUT2;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_74                                     = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_72;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_75                                     = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_73;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_74                      			   = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_74;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_75                      			   = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_75;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_76;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_77;
		gm_OHTStatus->IOOutput->uOutputCommon.BlackBox_Output                              = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_78;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_79                                     = gm_OHTStatus->IOOutput->uOutput.Output_V80.Reserved_79;


	 }
	 else if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_80) && (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV))
	 {

	  	////AOHC-286
     //Input
        gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.HAND_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm                                = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.HAND_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.HAND_Right_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.HAND_Left_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close                                = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.HAND_Close;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open                                 = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.HAND_Open;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Push                                 = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.HAND_Push;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home		                          = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.HOIST_Home;		// 미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_ES                                   = 0;
		gm_OHTStatus->IOInput->uInputCommon.HAND_PIO_Go                               = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.HAND_PIO_Go;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.EQPIO_Load_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ                          = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.EQPIO_Unload_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready                               = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.EQPIO_Ready;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL                             = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.EQPIO_HO_AVBL;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES                                  = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.EQPIO_ES;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Left_Go   		                  = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.EQPIO_Left_Go;   		//	BYTE EQPIO_Right_Go;미사용
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left                              = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.STEER_F_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left                              = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.STEER_R_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right                             = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.STEER_F_Right;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right                             = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.STEER_R_Right;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SHUTTER_F_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SHUTTER_R_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SHUTTER_F_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SHUTTER_R_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SHUTTER_F_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SHUTTER_R_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SHUTTER_F_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SHUTTER_R_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Input_Extend    	              = 0;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Long                             = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.OBSTACLE_Long;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Middle                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.OBSTACLE_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Short                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.OBSTACLE_Short;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Fail                             = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.OBSTACLE_Fail;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Input_Extend                    = 0;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Long                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.OHTDETECT_Long;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Middle                          = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.OHTDETECT_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Short                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.OHTDETECT_Short;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Fail                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.OHTDETECT_Fail;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.LOOKDOWN_Detect;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.LOOKDOWN_Error;
		gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home                                = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SHIFT_Home;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.Reserved_40;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right                          = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.Reserved_41;
		gm_OHTStatus->IOInput->uInputCommon.UTB_Detect                                = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.Reserved_42;				// 미사용
		gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect                               = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.FOUP_Detect;
		gm_OHTStatus->IOInput->uInputCommon.OSCILLATION                               = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.OSCILLATION;
		gm_OHTStatus->IOInput->uInputCommon.EMO_Press                                 = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.EMO_Press;
		gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press                              = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.BUMPER_Press;
		gm_OHTStatus->IOInput->uInputCommon.FAN_Fail                                  = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.FAN_Fail;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect1                        = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.HOIST_Belt_Detect1;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect2                        = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.Reserved_48;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.TP_Enable                                 = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.Reserved_49;				// 미사용;
		gm_OHTStatus->IOInput->uInputCommon.HANDTP_Connect                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.HANDTP_Connect;
		gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect                         = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.Reserved_51;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_Front                        = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.STOP_Driving_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_PreCheck                     = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.STOP_Driving_PreCheck;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Front                          = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.STOP_Trans_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Rear                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.STOP_Trans_Rear;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_OK                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.BCR_Driving_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_NG                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.BCR_Driving_NG;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_OK                              = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.BCR_Trans_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_NG                              = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.BCR_Trans_NG;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_1                                = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.PIO_Input3;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_2                                = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.PIO_Input5;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_3                                = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.PIO_Input6;
		gm_OHTStatus->IOInput->uInputCommon.SMPS_Short                                = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SMPS_Short;
		gm_OHTStatus->IOInput->uInputCommon.CID_Link_Compt_IN                         = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.CID_Link_Compt_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Interlock_IN                          = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.CID_Interlock_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Pass_OK_IN                            = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.CID_Completion_ACK;
		gm_OHTStatus->IOInput->uInputCommon.CID_Status_IN                             = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.CID_Status_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN1                          = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.CID_Reserved_IN1;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN2                          = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.CID_Reserved_IN2;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_0_Sensor                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.Rotate_0_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_90_Sensor                          = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.Rotate_90_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_180_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.Rotate_180_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_270_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.Rotate_270_Sensor; //미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_Reserved_1                           = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.Hand_Reserved7;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home_2                              = 0;   // 미사용
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SHUTTER_F_Middle_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.SHUTTER_R_Middle_Close;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_1                                 = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.POD_EUV_1;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_2                                 = gm_OHTStatus->IOInput->uInput.Input_V80_EUV.POD_EUV_2;


		//Output
        gm_OHTStatus->IOOutput->uOutputCommon.HAND_Close                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.HAND_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Open                                    = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.HAND_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Origin                                  = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.HAND_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reset                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.HAND_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Free                                    = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.HAND_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved6                               = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.HAND_Reserved6;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved7                               = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.HAND_Reserved7;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved8                               = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.HAND_Reserved8;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_HandTrigger                             = 0;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_PIO_Select                              = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.HAND_PIO_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid                                  = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.EQPIO_Valid;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.EQPIO_CS_0;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.EQPIO_CS_1;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_2                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.EQPIO_CS_2;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_3                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.EQPIO_CS_3;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req                                 = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.EQPIO_TR_Req;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.EQPIO_Busy;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete                               = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.EQPIO_Complete;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select                            = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.EQPIO_Left_Select;      //	BYTE EQPIO_Right_Select; 미사용
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.STEER_F_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.STEER_F_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.STEER_R_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.STEER_R_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.STEER_F_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.STEER_R_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_F_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_R_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_F_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_R_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_F_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_R_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_F_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_R_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_F_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_R_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.OBSTACLE_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.OBSTACLE_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.OBSTACLE_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.OBSTACLE_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area5_Sel                           = 0;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.OBSTACLE_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.OHTDETECT_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.OHTDETECT_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.OHTDETECT_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.OHTDETECT_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area5_Sel                          = 0;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Power_Reset                        = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.OHTDETECT_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.LOOKDOWN_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.LOOKDOWN_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.LOOKDOWN_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.LOOKDOWN_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.LOOKDOWN_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Red                                     = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.LAMP_Red;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Right                            = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.LAMP_Yellow_Right;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Left                             = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.LAMP_Yellow_Left;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.LAMP_Green;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset                             = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.HAND_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.BRAKE_Driving_Release                        = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.BRAKE_Driving_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Bumper_Clear                                 = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.Bumper_Clear;
		gm_OHTStatus->IOOutput->uOutputCommon.SAFETY_Reset                                 = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SAFETY_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HOIST_Ext_Brake_Release                      = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.HOIST_Ext_Brake_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.QRR_Power                                    = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.QRR_Power;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved62                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.Reserved60;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved63                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.Reserved61;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved64                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.Reserved62;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved65                                   = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.Reserved63;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON                           = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.CID_Left_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON                          = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.CID_Right_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy                                = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.CID_Occupancy;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion                          = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.CID_Pass_Completion;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal                                 = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.CID_Abnormal;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode                           = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.CID_Operation_Mode;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag                               = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.CID_Common_Tag; 		//동일 lane link 금지
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Reserved_OUT2                            = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.CID_Reserved_OUT2;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_F_Middle_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V80_EUV.SHUTTER_R_Middle_Close;

     }
	 else if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_81) && (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP))
	 {
	 	////AOHC-286
     //Input
        gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition                           = gm_OHTStatus->IOInput->uInput.Input_V81.HAND_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm                                = gm_OHTStatus->IOInput->uInput.Input_V81.HAND_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip                           = gm_OHTStatus->IOInput->uInput.Input_V81.HAND_Right_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip                            = gm_OHTStatus->IOInput->uInput.Input_V81.HAND_Left_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close                                = gm_OHTStatus->IOInput->uInput.Input_V81.HAND_Close;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open                                 = gm_OHTStatus->IOInput->uInput.Input_V81.HAND_Open;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Push                                 = gm_OHTStatus->IOInput->uInput.Input_V81.HAND_Push;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home		                          = gm_OHTStatus->IOInput->uInput.Input_V81.HOIST_Home;		// 미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_ES                                   = 0;//gm_OHTStatus->IOInput->uInput.Input_V81.HNAD_ES;               // 미사용
		gm_OHTStatus->IOInput->uInputCommon.HAND_PIO_Go                               = gm_OHTStatus->IOInput->uInput.Input_V81.HAND_PIO_Go;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ                            = gm_OHTStatus->IOInput->uInput.Input_V81.EQPIO_Load_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ                          = gm_OHTStatus->IOInput->uInput.Input_V81.EQPIO_Unload_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready                               = gm_OHTStatus->IOInput->uInput.Input_V81.EQPIO_Ready;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL                             = gm_OHTStatus->IOInput->uInput.Input_V81.EQPIO_HO_AVBL;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES                                  = gm_OHTStatus->IOInput->uInput.Input_V81.EQPIO_ES;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Left_Go   		                  = gm_OHTStatus->IOInput->uInput.Input_V81.EQPIO_Left_Go;   		//	BYTE EQPIO_Right_Go;미사용
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left                              = gm_OHTStatus->IOInput->uInput.Input_V81.STEER_F_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left                              = gm_OHTStatus->IOInput->uInput.Input_V81.STEER_R_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right                             = gm_OHTStatus->IOInput->uInput.Input_V81.STEER_F_Right;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right                             = gm_OHTStatus->IOInput->uInput.Input_V81.STEER_R_Right;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close                           = gm_OHTStatus->IOInput->uInput.Input_V81.SHUTTER_F_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close                           = gm_OHTStatus->IOInput->uInput.Input_V81.SHUTTER_R_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open                            = gm_OHTStatus->IOInput->uInput.Input_V81.SHUTTER_F_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open                            = gm_OHTStatus->IOInput->uInput.Input_V81.SHUTTER_R_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V81.SHUTTER_F_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V81.SHUTTER_R_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V81.SHUTTER_F_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V81.SHUTTER_R_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Input_Extend    	              = 0;//gm_OHTStatus->IOInput->uInput.Input_V81.OBSTACLE_Input_Extend;     	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Long                             = gm_OHTStatus->IOInput->uInput.Input_V81.OBSTACLE_Long;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Middle                           = gm_OHTStatus->IOInput->uInput.Input_V81.OBSTACLE_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Short                            = gm_OHTStatus->IOInput->uInput.Input_V81.OBSTACLE_Short;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Fail                             = gm_OHTStatus->IOInput->uInput.Input_V81.OBSTACLE_Fail;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Input_Extend                    = 0;//gm_OHTStatus->IOInput->uInput.Input_V81.OHTDETECT_Input_Extend;   	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Long                            = gm_OHTStatus->IOInput->uInput.Input_V81.OHTDETECT_Long;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Middle                          = gm_OHTStatus->IOInput->uInput.Input_V81.OHTDETECT_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Short                           = gm_OHTStatus->IOInput->uInput.Input_V81.OHTDETECT_Short;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Fail                            = gm_OHTStatus->IOInput->uInput.Input_V81.OHTDETECT_Fail;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect                           = gm_OHTStatus->IOInput->uInput.Input_V81.LOOKDOWN_Detect;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error                            = gm_OHTStatus->IOInput->uInput.Input_V81.LOOKDOWN_Error;
		gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home                                = gm_OHTStatus->IOInput->uInput.Input_V81.SHIFT_Home;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left                           = gm_OHTStatus->IOInput->uInput.Input_V81.STB_Detect_Left;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right                          = gm_OHTStatus->IOInput->uInput.Input_V81.STB_Detect_Right;
		gm_OHTStatus->IOInput->uInputCommon.UTB_Detect                                = gm_OHTStatus->IOInput->uInput.Input_V81.UTB_Detect;				// 미사용
		gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect                               = gm_OHTStatus->IOInput->uInput.Input_V81.FOUP_Detect;
		gm_OHTStatus->IOInput->uInputCommon.OSCILLATION                               = gm_OHTStatus->IOInput->uInput.Input_V81.OSCILLATION;
		gm_OHTStatus->IOInput->uInputCommon.EMO_Press                                 = gm_OHTStatus->IOInput->uInput.Input_V81.EMO_Press;
		gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press                              = gm_OHTStatus->IOInput->uInput.Input_V81.BUMPER_Press;
		gm_OHTStatus->IOInput->uInputCommon.FAN_Fail                                  = gm_OHTStatus->IOInput->uInput.Input_V81.FAN_Fail;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect1                        = gm_OHTStatus->IOInput->uInput.Input_V81.HOIST_Belt_Detect1;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect2                        = gm_OHTStatus->IOInput->uInput.Input_V81.HOIST_Belt_Detect2;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.TP_Enable                                 = gm_OHTStatus->IOInput->uInput.Input_V81.TP_Enable;				// 미사용;
		gm_OHTStatus->IOInput->uInputCommon.HANDTP_Connect                            = gm_OHTStatus->IOInput->uInput.Input_V81.HANDTP_Connect;
		gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect                         = gm_OHTStatus->IOInput->uInput.Input_V81.FOUP_Cover_Open_Detect;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_Front                        = gm_OHTStatus->IOInput->uInput.Input_V81.STOP_Driving_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_PreCheck                     = gm_OHTStatus->IOInput->uInput.Input_V81.STOP_Driving_PreCheck;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Front                          = gm_OHTStatus->IOInput->uInput.Input_V81.STOP_Trans_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Rear                           = gm_OHTStatus->IOInput->uInput.Input_V81.STOP_Trans_Rear;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_OK                            = gm_OHTStatus->IOInput->uInput.Input_V81.BCR_Driving_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_NG                            = gm_OHTStatus->IOInput->uInput.Input_V81.BCR_Driving_NG;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_OK                              = gm_OHTStatus->IOInput->uInput.Input_V81.BCR_Trans_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_NG                              = gm_OHTStatus->IOInput->uInput.Input_V81.BCR_Trans_NG;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_1                                = gm_OHTStatus->IOInput->uInput.Input_V81.PIO_Input3;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_2                                = gm_OHTStatus->IOInput->uInput.Input_V81.PIO_Input5;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_3                                = gm_OHTStatus->IOInput->uInput.Input_V81.PIO_Input6;
		gm_OHTStatus->IOInput->uInputCommon.SMPS_Short                                = gm_OHTStatus->IOInput->uInput.Input_V81.SMPS_Short;
		gm_OHTStatus->IOInput->uInputCommon.CID_Link_Compt_IN                         = gm_OHTStatus->IOInput->uInput.Input_V81.CID_Link_Compt_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Interlock_IN                          = gm_OHTStatus->IOInput->uInput.Input_V81.CID_Interlock_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Pass_OK_IN                            = gm_OHTStatus->IOInput->uInput.Input_V81.CID_Completion_ACK;
		gm_OHTStatus->IOInput->uInputCommon.CID_Status_IN                             = gm_OHTStatus->IOInput->uInput.Input_V81.CID_Status_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN1                          = gm_OHTStatus->IOInput->uInput.Input_V81.CID_Reserved_IN1;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN2                          = gm_OHTStatus->IOInput->uInput.Input_V81.CID_Reserved_IN2;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_0_Sensor                           = gm_OHTStatus->IOInput->uInput.Input_V81.Rotate_0_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_90_Sensor                          = gm_OHTStatus->IOInput->uInput.Input_V81.Rotate_90_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_180_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V81.Rotate_180_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_270_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V81.Rotate_270_Sensor; //미사용
		gm_OHTStatus->IOInput->uInputCommon.BlackBox_Error                            = gm_OHTStatus->IOInput->uInput.Input_V81.BlackBox_Error;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home_2                              = gm_OHTStatus->IOInput->uInput.Input_V81.Reverved_75;     // 미사용
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V81.Reverved_76;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V81.Reverved_77;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_1                                 = gm_OHTStatus->IOInput->uInput.Input_V81.Reverved_78;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_2                                 = gm_OHTStatus->IOInput->uInput.Input_V81.Reverved_79;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_5                                   = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_5;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_6                                   = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_6;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_7                                   = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_7;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_8                                   = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_8;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_9                                   = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_9;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_10                                  = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_10;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_11                                  = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_11;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_12                                  = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_12;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_13                                  = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_13;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_14                                  = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_14;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_15                                  = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_15;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_16                                  = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_16;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_17                                  = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_17;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_18                                  = gm_OHTStatus->IOInput->uInput.Input_V81.SPARE_18;

		//Output
        gm_OHTStatus->IOOutput->uOutputCommon.HAND_Close                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.HAND_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Open                                    = gm_OHTStatus->IOOutput->uOutput.Output_V81.HAND_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Origin                                  = gm_OHTStatus->IOOutput->uOutput.Output_V81.HAND_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reset                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.HAND_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Free                                    = gm_OHTStatus->IOOutput->uOutput.Output_V81.HAND_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved6                               = gm_OHTStatus->IOOutput->uOutput.Output_V81.HAND_Reserved6;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved7                               = gm_OHTStatus->IOOutput->uOutput.Output_V81.HAND_Reserved7;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved8                               = gm_OHTStatus->IOOutput->uOutput.Output_V81.HAND_Reserved8;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_HandTrigger                             = 0;//gm_OHTStatus->IOOutput->uOutput.Output_V81.HAND_HandTrigger;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_PIO_Select                              = gm_OHTStatus->IOOutput->uOutput.Output_V81.HAND_PIO_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid                                  = gm_OHTStatus->IOOutput->uOutput.Output_V81.EQPIO_Valid;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.EQPIO_CS_0;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.EQPIO_CS_1;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_2                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.EQPIO_CS_2;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_3                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.EQPIO_CS_3;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req                                 = gm_OHTStatus->IOOutput->uOutput.Output_V81.EQPIO_TR_Req;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.EQPIO_Busy;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete                               = gm_OHTStatus->IOOutput->uOutput.Output_V81.EQPIO_Complete;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select                            = gm_OHTStatus->IOOutput->uOutput.Output_V81.EQPIO_Left_Select;      //	BYTE EQPIO_Right_Select; 미사용
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V81.STEER_F_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V81.STEER_F_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V81.STEER_R_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V81.STEER_R_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V81.STEER_F_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V81.STEER_R_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V81.SHUTTER_F_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V81.SHUTTER_R_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V81.SHUTTER_F_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V81.SHUTTER_R_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V81.SHUTTER_F_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V81.SHUTTER_R_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V81.SHUTTER_F_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V81.SHUTTER_R_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V81.SHUTTER_F_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V81.SHUTTER_R_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81.OBSTACLE_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81.OBSTACLE_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81.OBSTACLE_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81.OBSTACLE_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area5_Sel                           = 0;//gm_OHTStatus->IOOutput->uOutput.Output_V81.OBSTACLE_Area5_Sel;     //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V81.OBSTACLE_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V81.OHTDETECT_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V81.OHTDETECT_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V81.OHTDETECT_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V81.OHTDETECT_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area5_Sel                          = 0;//gm_OHTStatus->IOOutput->uOutput.Output_V81.OHTDETECT_Area5_Sel;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Power_Reset                        = gm_OHTStatus->IOOutput->uOutput.Output_V81.OHTDETECT_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81.LOOKDOWN_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81.LOOKDOWN_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81.LOOKDOWN_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81.LOOKDOWN_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V81.LOOKDOWN_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Red                                     = gm_OHTStatus->IOOutput->uOutput.Output_V81.LAMP_Red;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Right                            = gm_OHTStatus->IOOutput->uOutput.Output_V81.LAMP_Yellow_Right;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Left                             = gm_OHTStatus->IOOutput->uOutput.Output_V81.LAMP_Yellow_Left;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.LAMP_Green;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset                             = gm_OHTStatus->IOOutput->uOutput.Output_V81.HAND_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.BRAKE_Driving_Release                        = gm_OHTStatus->IOOutput->uOutput.Output_V81.BRAKE_Driving_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Bumper_Clear                                 = gm_OHTStatus->IOOutput->uOutput.Output_V81.Bumper_Clear;
		gm_OHTStatus->IOOutput->uOutputCommon.SAFETY_Reset                                 = gm_OHTStatus->IOOutput->uOutput.Output_V81.SAFETY_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HOIST_Ext_Brake_Release                      = gm_OHTStatus->IOOutput->uOutput.Output_V81.HOIST_Ext_Brake_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.QRR_Power                                    = gm_OHTStatus->IOOutput->uOutput.Output_V81.QRR_Power;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved62                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_60;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved63                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_61;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved64                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_62;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved65                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_63;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON                           = gm_OHTStatus->IOOutput->uOutput.Output_V81.CID_Left_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON                          = gm_OHTStatus->IOOutput->uOutput.Output_V81.CID_Right_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy                                = gm_OHTStatus->IOOutput->uOutput.Output_V81.CID_Occupancy;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion                          = gm_OHTStatus->IOOutput->uOutput.Output_V81.CID_Pass_Completion;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal                                 = gm_OHTStatus->IOOutput->uOutput.Output_V81.CID_Abnormal;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode                           = gm_OHTStatus->IOOutput->uOutput.Output_V81.CID_Operation_Mode;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag                               = gm_OHTStatus->IOOutput->uOutput.Output_V81.CID_Common_Tag; 		//동일 lane link 금지
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Reserved_OUT2                            = gm_OHTStatus->IOOutput->uOutput.Output_V81.CID_Reserved_OUT2;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_74                                     = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_72;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_75                                     = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_73;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_74                      			   = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_74;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_75                      			   = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_75;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_76;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_77;
		gm_OHTStatus->IOOutput->uOutputCommon.BlackBox_Output                                     = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_78;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_79                                     = gm_OHTStatus->IOOutput->uOutput.Output_V81.Reserved_79;

     }
	 else if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_81) && (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
	 {
	 	////AOHC-286
	    //Input
        gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition                           = gm_OHTStatus->IOInput->uInput.Input_V81S.HAND_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm                                = gm_OHTStatus->IOInput->uInput.Input_V81S.HAND_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip                           = gm_OHTStatus->IOInput->uInput.Input_V81S.HAND_Right_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip                            = gm_OHTStatus->IOInput->uInput.Input_V81S.HAND_Left_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close                                = gm_OHTStatus->IOInput->uInput.Input_V81S.HAND_Close;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open                                 = gm_OHTStatus->IOInput->uInput.Input_V81S.HAND_Open;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Push                                 = gm_OHTStatus->IOInput->uInput.Input_V81S.HAND_Push;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home		                          = gm_OHTStatus->IOInput->uInput.Input_V81S.HOIST_Home;		// 미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_ES                                   = gm_OHTStatus->IOInput->uInput.Input_V81S.HNAD_ES;               // 미사용
		gm_OHTStatus->IOInput->uInputCommon.HAND_PIO_Go                               = gm_OHTStatus->IOInput->uInput.Input_V81S.HAND_PIO_Go;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ                            = gm_OHTStatus->IOInput->uInput.Input_V81S.EQPIO_Load_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ                          = gm_OHTStatus->IOInput->uInput.Input_V81S.EQPIO_Unload_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready                               = gm_OHTStatus->IOInput->uInput.Input_V81S.EQPIO_Ready;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL                             = gm_OHTStatus->IOInput->uInput.Input_V81S.EQPIO_HO_AVBL;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES                                  = gm_OHTStatus->IOInput->uInput.Input_V81S.EQPIO_ES;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Left_Go   		                  = gm_OHTStatus->IOInput->uInput.Input_V81S.EQPIO_Left_Go;   		//	BYTE EQPIO_Right_Go;미사용
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left                              = gm_OHTStatus->IOInput->uInput.Input_V81S.STEER_F_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left                              = gm_OHTStatus->IOInput->uInput.Input_V81S.STEER_R_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right                             = gm_OHTStatus->IOInput->uInput.Input_V81S.STEER_F_Right;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right                             = gm_OHTStatus->IOInput->uInput.Input_V81S.STEER_R_Right;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close                           = gm_OHTStatus->IOInput->uInput.Input_V81S.SHUTTER_F_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close                           = gm_OHTStatus->IOInput->uInput.Input_V81S.SHUTTER_R_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open                            = gm_OHTStatus->IOInput->uInput.Input_V81S.SHUTTER_F_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open                            = gm_OHTStatus->IOInput->uInput.Input_V81S.SHUTTER_R_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V81S.SHUTTER_F_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V81S.SHUTTER_R_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V81S.SHUTTER_F_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V81S.SHUTTER_R_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Input_Extend    	              = gm_OHTStatus->IOInput->uInput.Input_V81S.OBSTACLE_Input_Extend;     	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Long                             = gm_OHTStatus->IOInput->uInput.Input_V81S.OBSTACLE_Long;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Middle                           = gm_OHTStatus->IOInput->uInput.Input_V81S.OBSTACLE_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Short                            = gm_OHTStatus->IOInput->uInput.Input_V81S.OBSTACLE_Short;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Fail                             = gm_OHTStatus->IOInput->uInput.Input_V81S.OBSTACLE_Fail;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Input_Extend                    = gm_OHTStatus->IOInput->uInput.Input_V81S.OHTDETECT_Input_Extend;   	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Long                            = gm_OHTStatus->IOInput->uInput.Input_V81S.OHTDETECT_Long;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Middle                          = gm_OHTStatus->IOInput->uInput.Input_V81S.OHTDETECT_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Short                           = gm_OHTStatus->IOInput->uInput.Input_V81S.OHTDETECT_Short;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Fail                            = gm_OHTStatus->IOInput->uInput.Input_V81S.OHTDETECT_Fail;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect                           = gm_OHTStatus->IOInput->uInput.Input_V81S.LOOKDOWN_Detect;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error                            = gm_OHTStatus->IOInput->uInput.Input_V81S.LOOKDOWN_Error;
		gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home                                = gm_OHTStatus->IOInput->uInput.Input_V81S.SHIFT_Home;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left                           = gm_OHTStatus->IOInput->uInput.Input_V81S.STB_Detect_Left;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right                          = gm_OHTStatus->IOInput->uInput.Input_V81S.STB_Detect_Right;
		gm_OHTStatus->IOInput->uInputCommon.UTB_Detect                                = gm_OHTStatus->IOInput->uInput.Input_V81S.UTB_Detect;				// 미사용
		gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect                               = gm_OHTStatus->IOInput->uInput.Input_V81S.FOUP_Detect;
		gm_OHTStatus->IOInput->uInputCommon.OSCILLATION                               = gm_OHTStatus->IOInput->uInput.Input_V81S.OSCILLATION;
		gm_OHTStatus->IOInput->uInputCommon.EMO_Press                                 = gm_OHTStatus->IOInput->uInput.Input_V81S.EMO_Press;
		gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press                              = gm_OHTStatus->IOInput->uInput.Input_V81S.BUMPER_Press;
		gm_OHTStatus->IOInput->uInputCommon.FAN_Fail                                  = gm_OHTStatus->IOInput->uInput.Input_V81S.FAN_Fail;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect1                        = gm_OHTStatus->IOInput->uInput.Input_V81S.HOIST_Belt_Detect1;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect2                        = gm_OHTStatus->IOInput->uInput.Input_V81S.HOIST_Belt_Detect2;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.TP_Enable                                 = gm_OHTStatus->IOInput->uInput.Input_V81S.TP_Enable;				// 미사용;
		gm_OHTStatus->IOInput->uInputCommon.HANDTP_Connect                            = gm_OHTStatus->IOInput->uInput.Input_V81S.HANDTP_Connect;
		gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect                         = gm_OHTStatus->IOInput->uInput.Input_V81S.FOUP_Cover_Open_Detect;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_Front                        = gm_OHTStatus->IOInput->uInput.Input_V81S.STOP_Driving_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_PreCheck                     = gm_OHTStatus->IOInput->uInput.Input_V81S.STOP_Driving_PreCheck;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Front                          = gm_OHTStatus->IOInput->uInput.Input_V81S.STOP_Trans_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Rear                           = gm_OHTStatus->IOInput->uInput.Input_V81S.STOP_Trans_Rear;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_OK                            = gm_OHTStatus->IOInput->uInput.Input_V81S.BCR_Driving_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_NG                            = gm_OHTStatus->IOInput->uInput.Input_V81S.BCR_Driving_NG;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_OK                              = gm_OHTStatus->IOInput->uInput.Input_V81S.BCR_Trans_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_NG                              = gm_OHTStatus->IOInput->uInput.Input_V81S.BCR_Trans_NG;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_1                                = gm_OHTStatus->IOInput->uInput.Input_V81S.Reserved_1;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_2                                = gm_OHTStatus->IOInput->uInput.Input_V81S.Reserved_2;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_3                                = gm_OHTStatus->IOInput->uInput.Input_V81S.Reserved_3;
		gm_OHTStatus->IOInput->uInputCommon.SMPS_Short                                = gm_OHTStatus->IOInput->uInput.Input_V81S.SMPS_Short;
		gm_OHTStatus->IOInput->uInputCommon.CID_Link_Compt_IN                         = gm_OHTStatus->IOInput->uInput.Input_V81S.CID_Link_Compt_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Interlock_IN                          = gm_OHTStatus->IOInput->uInput.Input_V81S.CID_Interlock_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Pass_OK_IN                            = gm_OHTStatus->IOInput->uInput.Input_V81S.CID_Completion_ACK;
		gm_OHTStatus->IOInput->uInputCommon.CID_Status_IN                             = gm_OHTStatus->IOInput->uInput.Input_V81S.CID_Status_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN1                          = gm_OHTStatus->IOInput->uInput.Input_V81S.CID_Reserved_IN1;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN2                          = gm_OHTStatus->IOInput->uInput.Input_V81S.CID_Reserved_IN2;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_0_Sensor                           = gm_OHTStatus->IOInput->uInput.Input_V81S.Rotate_0_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_90_Sensor                          = gm_OHTStatus->IOInput->uInput.Input_V81S.Rotate_90_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_180_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V81S.Rotate_180_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_270_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V81S.Rotate_270_Sensor; //미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_Reserved_1                           = gm_OHTStatus->IOInput->uInput.Input_V81S.HNAD_Reserved_1;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home_2                              = gm_OHTStatus->IOInput->uInput.Input_V81S.HOIST_Home_2;     // 미사용
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V81S.SHUTTER_F_Middle_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V81S.SHUTTER_R_Middle_Close;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_1                                 = gm_OHTStatus->IOInput->uInput.Input_V81S.POD_EUV_1;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_2                                 = gm_OHTStatus->IOInput->uInput.Input_V81S.POD_EUV_2;
		gm_OHTStatus->IOInput->uInputCommon.BlackBox_Error                            = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_5;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_6                                   = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_6;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_7                                   = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_7;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_8                                   = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_8;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_9                                   = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_9;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_10                                  = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_10;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_11                                  = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_11;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_12                                  = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_12;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_13                                  = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_13;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_14                                  = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_14;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_15                                  = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_15;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_16                                  = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_16;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_17                                  = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_17;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_18                                  = gm_OHTStatus->IOInput->uInput.Input_V81S.SPARE_18;

		//Output
        gm_OHTStatus->IOOutput->uOutputCommon.HAND_Close                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HAND_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Open                                    = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HAND_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Origin                                  = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HAND_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reset                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HAND_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Free                                    = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HAND_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved6                               = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HAND_Reserved6;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved7                               = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HAND_Reserved7;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved8                               = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HAND_Reserved8;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_HandTrigger                             = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HAND_HandTrigger;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_PIO_Select                              = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HAND_PIO_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid                                  = gm_OHTStatus->IOOutput->uOutput.Output_V81S.EQPIO_Valid;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.EQPIO_CS_0;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.EQPIO_CS_1;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_2                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.EQPIO_CS_2;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_3                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.EQPIO_CS_3;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req                                 = gm_OHTStatus->IOOutput->uOutput.Output_V81S.EQPIO_TR_Req;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.EQPIO_Busy;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete                               = gm_OHTStatus->IOOutput->uOutput.Output_V81S.EQPIO_Complete;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select                            = gm_OHTStatus->IOOutput->uOutput.Output_V81S.EQPIO_Left_Select;      //	BYTE EQPIO_Right_Select; 미사용
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V81S.STEER_F_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V81S.STEER_F_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V81S.STEER_R_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V81S.STEER_R_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V81S.STEER_F_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V81S.STEER_R_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_F_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_R_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_F_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_R_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_F_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_R_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_F_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_R_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_F_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_R_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OBSTACLE_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OBSTACLE_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OBSTACLE_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OBSTACLE_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area5_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OBSTACLE_Area5_Sel;     //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OBSTACLE_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OHTDETECT_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OHTDETECT_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OHTDETECT_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OHTDETECT_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area5_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OHTDETECT_Area5_Sel;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Power_Reset                        = gm_OHTStatus->IOOutput->uOutput.Output_V81S.OHTDETECT_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81S.LOOKDOWN_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81S.LOOKDOWN_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81S.LOOKDOWN_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V81S.LOOKDOWN_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V81S.LOOKDOWN_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Red                                     = gm_OHTStatus->IOOutput->uOutput.Output_V81S.LAMP_Red;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Right                            = gm_OHTStatus->IOOutput->uOutput.Output_V81S.LAMP_Yellow_Right;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Left                             = gm_OHTStatus->IOOutput->uOutput.Output_V81S.LAMP_Yellow_Left;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.LAMP_Green;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset                             = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HAND_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.BRAKE_Driving_Release                        = gm_OHTStatus->IOOutput->uOutput.Output_V81S.BRAKE_Driving_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Bumper_Clear                                 = gm_OHTStatus->IOOutput->uOutput.Output_V81S.Bumper_Clear;
		gm_OHTStatus->IOOutput->uOutputCommon.SAFETY_Reset                                 = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SAFETY_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HOIST_Ext_Brake_Release                      = gm_OHTStatus->IOOutput->uOutput.Output_V81S.HOIST_Ext_Brake_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.QRR_Power                                    = gm_OHTStatus->IOOutput->uOutput.Output_V81S.QRR_Power;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved62                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.Reserved62;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved63                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.Reserved63;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved64                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.Reserved64;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved65                                   = gm_OHTStatus->IOOutput->uOutput.Output_V81S.Reserved65;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON                           = gm_OHTStatus->IOOutput->uOutput.Output_V81S.CID_Left_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON                          = gm_OHTStatus->IOOutput->uOutput.Output_V81S.CID_Right_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy                                = gm_OHTStatus->IOOutput->uOutput.Output_V81S.CID_Occupancy;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion                          = gm_OHTStatus->IOOutput->uOutput.Output_V81S.CID_Pass_Completion;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal                                 = gm_OHTStatus->IOOutput->uOutput.Output_V81S.CID_Abnormal;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode                           = gm_OHTStatus->IOOutput->uOutput.Output_V81S.CID_Operation_Mode;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag                               = gm_OHTStatus->IOOutput->uOutput.Output_V81S.CID_Common_Tag; 		//동일 lane link 금지
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Reserved_OUT2                            = gm_OHTStatus->IOOutput->uOutput.Output_V81S.CID_Reserved_OUT2;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_74                                      = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SPARE_74;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_75                                      = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SPARE_75;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_F_Middle_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SHUTTER_R_Middle_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.BlackBox_Output                               = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SPARE_78;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_79                                      = gm_OHTStatus->IOOutput->uOutput.Output_V81S.SPARE_79;

	 }
	 else if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_85) && (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
	 {

		gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition                           = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm                                = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip                           = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Right_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip                            = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Left_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close                                = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Close;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open                                 = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Open;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Push                                 = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Push;
		gm_OHTStatus->IOInput->uInputCommon.HNAD_Reserved_1		                      = gm_OHTStatus->IOInput->uInput.Input_V85S.HNAD_Reserved_1;		// 미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_ES                                   = gm_OHTStatus->IOInput->uInput.Input_V85S.HNAD_ES;               // 미사용
		gm_OHTStatus->IOInput->uInputCommon.HAND_PIO_Go                               = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_PIO_Go;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ                            = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_Load_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ                          = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_Unload_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready                               = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_Ready;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL                             = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_HO_AVBL;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES                                  = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_ES;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Left_Go   		                  = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_Left_Go;   		//	BYTE EQPIO_Right_Go;미사용
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left                              = gm_OHTStatus->IOInput->uInput.Input_V85S.STEER_F_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left                              = gm_OHTStatus->IOInput->uInput.Input_V85S.STEER_R_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right                             = gm_OHTStatus->IOInput->uInput.Input_V85S.STEER_F_Right;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right                             = gm_OHTStatus->IOInput->uInput.Input_V85S.STEER_R_Right;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close                           = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_F_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close                           = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_R_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open                            = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_F_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open                            = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_R_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_F_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_R_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_F_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_R_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Input_Extend    	              = gm_OHTStatus->IOInput->uInput.Input_V85S.OBSTACLE_Input_Extend;     	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Long                             = gm_OHTStatus->IOInput->uInput.Input_V85S.OBSTACLE_Long;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Middle                           = gm_OHTStatus->IOInput->uInput.Input_V85S.OBSTACLE_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Short                            = gm_OHTStatus->IOInput->uInput.Input_V85S.OBSTACLE_Short;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Fail                             = gm_OHTStatus->IOInput->uInput.Input_V85S.OBSTACLE_Fail;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Input_Extend                    = gm_OHTStatus->IOInput->uInput.Input_V85S.OHTDETECT_Input_Extend;   	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Long                            = gm_OHTStatus->IOInput->uInput.Input_V85S.OHTDETECT_Long;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Middle                          = gm_OHTStatus->IOInput->uInput.Input_V85S.OHTDETECT_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Short                           = gm_OHTStatus->IOInput->uInput.Input_V85S.OHTDETECT_Short;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Fail                            = gm_OHTStatus->IOInput->uInput.Input_V85S.OHTDETECT_Fail;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect                           = gm_OHTStatus->IOInput->uInput.Input_V85S.LOOKDOWN_Detect;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error                            = gm_OHTStatus->IOInput->uInput.Input_V85S.LOOKDOWN_Error;
		gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home                                = gm_OHTStatus->IOInput->uInput.Input_V85S.SHIFT_Home;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left                           = gm_OHTStatus->IOInput->uInput.Input_V85S.STB_Detect_Left;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right                          = gm_OHTStatus->IOInput->uInput.Input_V85S.STB_Detect_Right;
		gm_OHTStatus->IOInput->uInputCommon.UTB_Detect                                = gm_OHTStatus->IOInput->uInput.Input_V85S.UTB_Detect;				// 미사용
		gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect                               = gm_OHTStatus->IOInput->uInput.Input_V85S.FOUP_Detect;
		gm_OHTStatus->IOInput->uInputCommon.OSCILLATION                               = gm_OHTStatus->IOInput->uInput.Input_V85S.OSCILLATION;
		gm_OHTStatus->IOInput->uInputCommon.EMO_Press                                 = gm_OHTStatus->IOInput->uInput.Input_V85S.EMO_Press;
		gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press                              = gm_OHTStatus->IOInput->uInput.Input_V85S.BUMPER_Press;
		gm_OHTStatus->IOInput->uInputCommon.FAN_Fail                                  = gm_OHTStatus->IOInput->uInput.Input_V85S.FAN_Fail;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect2                        = gm_OHTStatus->IOInput->uInput.Input_V85S.HOIST_Belt_Detect2;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_51                        		  = gm_OHTStatus->IOInput->uInput.Input_V85S.Reserved_51;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.HANDTP_Connect                            = gm_OHTStatus->IOInput->uInput.Input_V85S.HANDTP_Connect;				// 미사용;
		gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect                         = gm_OHTStatus->IOInput->uInput.Input_V85S.FOUP_Cover_Open_Detect;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_Front                        = gm_OHTStatus->IOInput->uInput.Input_V85S.STOP_Driving_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_PreCheck                     = gm_OHTStatus->IOInput->uInput.Input_V85S.STOP_Driving_PreCheck;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Front                          = gm_OHTStatus->IOInput->uInput.Input_V85S.STOP_Trans_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Rear                           = gm_OHTStatus->IOInput->uInput.Input_V85S.STOP_Trans_Rear;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_OK                            = gm_OHTStatus->IOInput->uInput.Input_V85S.BCR_Driving_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_NG                            = gm_OHTStatus->IOInput->uInput.Input_V85S.BCR_Driving_NG;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_OK                              = gm_OHTStatus->IOInput->uInput.Input_V85S.BCR_Trans_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_NG                              = gm_OHTStatus->IOInput->uInput.Input_V85S.BCR_Trans_NG;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect1                        = gm_OHTStatus->IOInput->uInput.Input_V85S.HOIST_Belt_Detect1;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_1                                = gm_OHTStatus->IOInput->uInput.Input_V85S.PIO_Input3;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_2                                = gm_OHTStatus->IOInput->uInput.Input_V85S.PIO_Input5;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_3                                = gm_OHTStatus->IOInput->uInput.Input_V85S.PIO_Input6;
		gm_OHTStatus->IOInput->uInputCommon.SMPS_Short                                = gm_OHTStatus->IOInput->uInput.Input_V85S.SMPS_Short;
		gm_OHTStatus->IOInput->uInputCommon.CID_Link_Compt_IN                         = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_Link_Compt_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Interlock_IN                          = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_Interlock_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Pass_OK_IN                            = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_Completion_ACK;
		gm_OHTStatus->IOInput->uInputCommon.CID_Status_IN                             = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_Status_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN1                          = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_NoRegistration;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN2                          = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_Reserved_IN2;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home                          	  = gm_OHTStatus->IOInput->uInput.Input_V85S.HOIST_Home;
		gm_OHTStatus->IOInput->uInputCommon.Blackbox_DI2                          	  = gm_OHTStatus->IOInput->uInput.Input_V85S.Blackbox_DI2;
		gm_OHTStatus->IOInput->uInputCommon.Blackbox_DI1                          	  = gm_OHTStatus->IOInput->uInput.Input_V85S.Blackbox_DI1;
		gm_OHTStatus->IOInput->uInputCommon.Blackbox_DI0                          	  = gm_OHTStatus->IOInput->uInput.Input_V85S.Blackbox_DI0;

//		gm_OHTStatus->IOInput->uInputCommon.Rotate_0_Sensor                           = gm_OHTStatus->IOInput->uInput.Input_V85S.Rotate_0_Sensor;
//		gm_OHTStatus->IOInput->uInputCommon.Rotate_0_Sensor                          = gm_OHTStatus->IOInput->uInput.Input_V85S.Rotate_90_Sensor;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_3                         = gm_OHTStatus->IOInput->uInput.Input_V85S.Rotate_180_Sensor;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_4                         = gm_OHTStatus->IOInput->uInput.Input_V85S.Rotate_270_Sensor; //미사용
		gm_OHTStatus->IOInput->uInputCommon.SPARE_5                                 = gm_OHTStatus->IOInput->uInput.Input_V85S.SPARE_5;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_6                                 = gm_OHTStatus->IOInput->uInput.Input_V85S.SPARE_6;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_7                            		  = gm_OHTStatus->IOInput->uInput.Input_V85S.SPARE_7;

		//Output
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Close                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Open                                    = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Origin                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reset                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Free                                    = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved6                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Reserved6;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved7                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Reserved7;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved8                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Reserved8;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_HandTrigger                             = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_HandTrigger;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_PIO_Select                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_PIO_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_Valid;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_CS_0;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_CS_1;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_2                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_CS_2;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_3                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_CS_3;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_TR_Req;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_Busy;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_Complete;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select                            = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_Left_Select;      //	BYTE EQPIO_Right_Select; 미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Right_Select                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_Right_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_F_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_F_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_R_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_R_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_F_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_R_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved40                          		   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Reserved40;     //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area5_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Area5_Sel;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Power_Reset                        = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LOOKDOWN_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LOOKDOWN_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LOOKDOWN_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LOOKDOWN_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LOOKDOWN_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Red                                     = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Red;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Right                            = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Yellow_Right;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Green;
//		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Red_Left                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Red_Left;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Left                             = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Yellow_Left;
//		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green_Left                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Green_Left;
//		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Green;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset                             = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.BRAKE_Driving_Release                        = gm_OHTStatus->IOOutput->uOutput.Output_V85S.BRAKE_Driving_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Bumper_Clear                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Bumper_Clear;
		gm_OHTStatus->IOOutput->uOutputCommon.SAFETY_Reset                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SAFETY_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HOIST_Ext_Brake_Release                      = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HOIST_Ext_Brake_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved64                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Reserved64;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved65                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Reserved65;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area5_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Area5_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.Hoist_Steer_PWR_Enable                       = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Hoist_Steer_PWR_Enable;
		gm_OHTStatus->IOOutput->uOutputCommon.IO_PWR_Enable                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.IO_PWR_Enable;
		gm_OHTStatus->IOOutput->uOutputCommon.EQ_PWR_Enable                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQ_PWR_Enable;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Left_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Right_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Occupancy;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Pass_Completion;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Abnormal;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Operation_Mode;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Common_Tag; 		//동일 lane link 금지
		gm_OHTStatus->IOOutput->uOutputCommon.CID_LinkRequest                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_LinkRequest;
		gm_OHTStatus->IOOutput->uOutputCommon.Blackbox_DO2                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Blackbox_DO2;
		gm_OHTStatus->IOOutput->uOutputCommon.Blackbox_DO1                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Blackbox_DO1;
		gm_OHTStatus->IOOutput->uOutputCommon.Blackbox_DO0                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Blackbox_DO0;
//		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Middle_Close;
//		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Middle_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_78                             		   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SPARE_78;

	 }
	 else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_82)
	 {

		gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition                           = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm                                = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip                           = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Right_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip                            = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Left_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close                                = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Close;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open                                 = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Open;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Push                                 = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_Push;
		gm_OHTStatus->IOInput->uInputCommon.HNAD_Reserved_1		                      = gm_OHTStatus->IOInput->uInput.Input_V85S.HNAD_Reserved_1;		// 미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_ES                                   = gm_OHTStatus->IOInput->uInput.Input_V85S.HNAD_ES;               // 미사용
		gm_OHTStatus->IOInput->uInputCommon.HAND_PIO_Go                               = gm_OHTStatus->IOInput->uInput.Input_V85S.HAND_PIO_Go;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ                            = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_Load_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ                          = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_Unload_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready                               = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_Ready;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL                             = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_HO_AVBL;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES                                  = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_ES;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Left_Go   		                  = gm_OHTStatus->IOInput->uInput.Input_V85S.EQPIO_Left_Go;   		//	BYTE EQPIO_Right_Go;미사용
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left                              = gm_OHTStatus->IOInput->uInput.Input_V85S.STEER_F_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left                              = gm_OHTStatus->IOInput->uInput.Input_V85S.STEER_R_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right                             = gm_OHTStatus->IOInput->uInput.Input_V85S.STEER_F_Right;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right                             = gm_OHTStatus->IOInput->uInput.Input_V85S.STEER_R_Right;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close                           = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_F_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close                           = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_R_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open                            = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_F_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open                            = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_R_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_F_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_R_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_F_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V85S.SHUTTER_R_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Input_Extend    	              = gm_OHTStatus->IOInput->uInput.Input_V85S.OBSTACLE_Input_Extend;     	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Long                             = gm_OHTStatus->IOInput->uInput.Input_V85S.OBSTACLE_Long;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Middle                           = gm_OHTStatus->IOInput->uInput.Input_V85S.OBSTACLE_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Short                            = gm_OHTStatus->IOInput->uInput.Input_V85S.OBSTACLE_Short;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Fail                             = gm_OHTStatus->IOInput->uInput.Input_V85S.OBSTACLE_Fail;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Input_Extend                    = gm_OHTStatus->IOInput->uInput.Input_V85S.OHTDETECT_Input_Extend;   	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Long                            = gm_OHTStatus->IOInput->uInput.Input_V85S.OHTDETECT_Long;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Middle                          = gm_OHTStatus->IOInput->uInput.Input_V85S.OHTDETECT_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Short                           = gm_OHTStatus->IOInput->uInput.Input_V85S.OHTDETECT_Short;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Fail                            = gm_OHTStatus->IOInput->uInput.Input_V85S.OHTDETECT_Fail;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect                           = gm_OHTStatus->IOInput->uInput.Input_V85S.LOOKDOWN_Detect;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error                            = gm_OHTStatus->IOInput->uInput.Input_V85S.LOOKDOWN_Error;
		gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home                                = gm_OHTStatus->IOInput->uInput.Input_V85S.SHIFT_Home;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left                           = gm_OHTStatus->IOInput->uInput.Input_V85S.STB_Detect_Left;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right                          = gm_OHTStatus->IOInput->uInput.Input_V85S.STB_Detect_Right;
		gm_OHTStatus->IOInput->uInputCommon.UTB_Detect                                = gm_OHTStatus->IOInput->uInput.Input_V85S.UTB_Detect;				// 미사용
		gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect                               = gm_OHTStatus->IOInput->uInput.Input_V85S.FOUP_Detect;
		gm_OHTStatus->IOInput->uInputCommon.OSCILLATION                               = gm_OHTStatus->IOInput->uInput.Input_V85S.OSCILLATION;
		gm_OHTStatus->IOInput->uInputCommon.EMO_Press                                 = gm_OHTStatus->IOInput->uInput.Input_V85S.EMO_Press;
		gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press                              = gm_OHTStatus->IOInput->uInput.Input_V85S.BUMPER_Press;
		gm_OHTStatus->IOInput->uInputCommon.FAN_Fail                                  = gm_OHTStatus->IOInput->uInput.Input_V85S.FAN_Fail;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect2                        = gm_OHTStatus->IOInput->uInput.Input_V85S.HOIST_Belt_Detect2;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_51                        		  = gm_OHTStatus->IOInput->uInput.Input_V85S.Reserved_51;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.HANDTP_Connect                            = gm_OHTStatus->IOInput->uInput.Input_V85S.HANDTP_Connect;				// 미사용;
		gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect                         = gm_OHTStatus->IOInput->uInput.Input_V85S.FOUP_Cover_Open_Detect;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_Front                        = gm_OHTStatus->IOInput->uInput.Input_V85S.STOP_Driving_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_PreCheck                     = gm_OHTStatus->IOInput->uInput.Input_V85S.STOP_Driving_PreCheck;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Front                          = gm_OHTStatus->IOInput->uInput.Input_V85S.STOP_Trans_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Rear                           = gm_OHTStatus->IOInput->uInput.Input_V85S.STOP_Trans_Rear;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_OK                            = gm_OHTStatus->IOInput->uInput.Input_V85S.BCR_Driving_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_NG                            = gm_OHTStatus->IOInput->uInput.Input_V85S.BCR_Driving_NG;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_OK                              = gm_OHTStatus->IOInput->uInput.Input_V85S.BCR_Trans_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_NG                              = gm_OHTStatus->IOInput->uInput.Input_V85S.BCR_Trans_NG;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect1                        = gm_OHTStatus->IOInput->uInput.Input_V85S.HOIST_Belt_Detect1;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_1                                = gm_OHTStatus->IOInput->uInput.Input_V85S.PIO_Input3;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_2                                = gm_OHTStatus->IOInput->uInput.Input_V85S.PIO_Input5;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_3                                = gm_OHTStatus->IOInput->uInput.Input_V85S.PIO_Input6;
		gm_OHTStatus->IOInput->uInputCommon.SMPS_Short                                = gm_OHTStatus->IOInput->uInput.Input_V85S.SMPS_Short;
		gm_OHTStatus->IOInput->uInputCommon.CID_Link_Compt_IN                         = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_Link_Compt_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Interlock_IN                          = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_Interlock_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Pass_OK_IN                            = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_Completion_ACK;
		gm_OHTStatus->IOInput->uInputCommon.CID_Status_IN                             = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_Status_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN1                          = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_NoRegistration;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN2                          = gm_OHTStatus->IOInput->uInput.Input_V85S.CID_Reserved_IN2;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home                          	  = gm_OHTStatus->IOInput->uInput.Input_V85S.HOIST_Home;
		gm_OHTStatus->IOInput->uInputCommon.Blackbox_DI2                          	  = gm_OHTStatus->IOInput->uInput.Input_V85S.Blackbox_DI2;
		gm_OHTStatus->IOInput->uInputCommon.Blackbox_DI1                          	  = gm_OHTStatus->IOInput->uInput.Input_V85S.Blackbox_DI1;
		gm_OHTStatus->IOInput->uInputCommon.Blackbox_DI0                          	  = gm_OHTStatus->IOInput->uInput.Input_V85S.Blackbox_DI0;

//		gm_OHTStatus->IOInput->uInputCommon.Rotate_0_Sensor                           = gm_OHTStatus->IOInput->uInput.Input_V85S.Rotate_0_Sensor;
//		gm_OHTStatus->IOInput->uInputCommon.Rotate_0_Sensor                           = gm_OHTStatus->IOInput->uInput.Input_V85S.Rotate_90_Sensor;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_3                         	      = gm_OHTStatus->IOInput->uInput.Input_V85S.Rotate_180_Sensor;
//		gm_OHTStatus->IOInput->uInputCommon.SPARE_4                         	      = gm_OHTStatus->IOInput->uInput.Input_V85S.Rotate_270_Sensor; //미사용
		gm_OHTStatus->IOInput->uInputCommon.SPARE_5                                   = gm_OHTStatus->IOInput->uInput.Input_V85S.SPARE_5;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_6                                   = gm_OHTStatus->IOInput->uInput.Input_V85S.SPARE_6;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_7                            		  = gm_OHTStatus->IOInput->uInput.Input_V85S.SPARE_7;

		//Output
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Close                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Open                                    = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Origin                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reset                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Free                                    = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved6                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Reserved6;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved7                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Reserved7;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved8                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Reserved8;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_HandTrigger                             = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_HandTrigger;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_PIO_Select                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_PIO_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_Valid;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_CS_0;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_CS_1;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_2                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_CS_2;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_3                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_CS_3;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_TR_Req;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_Busy;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_Complete;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select                            = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_Left_Select;      //	BYTE EQPIO_Right_Select; 미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Right_Select                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQPIO_Right_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_F_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_F_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_R_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_R_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_F_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85S.STEER_R_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved40                          		   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Reserved40;     //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area5_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Area5_Sel;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Power_Reset                        = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OHTDETECT_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LOOKDOWN_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LOOKDOWN_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LOOKDOWN_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LOOKDOWN_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LOOKDOWN_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Red                                     = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Red;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Right                            = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Yellow_Right;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Green;
//		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Red_Left                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Red_Left;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Left                             = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Yellow_Left;
//		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green_Left                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Green_Left;
//		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.LAMP_Green;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset                             = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HAND_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.BRAKE_Driving_Release                        = gm_OHTStatus->IOOutput->uOutput.Output_V85S.BRAKE_Driving_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Bumper_Clear                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Bumper_Clear;
		gm_OHTStatus->IOOutput->uOutputCommon.SAFETY_Reset                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SAFETY_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HOIST_Ext_Brake_Release                      = gm_OHTStatus->IOOutput->uOutput.Output_V85S.HOIST_Ext_Brake_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved64                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Reserved64;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved65                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Reserved65;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area5_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.OBSTACLE_Area5_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.Hoist_Steer_PWR_Enable                       = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Hoist_Steer_PWR_Enable;
		gm_OHTStatus->IOOutput->uOutputCommon.IO_PWR_Enable                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.IO_PWR_Enable;
		gm_OHTStatus->IOOutput->uOutputCommon.EQ_PWR_Enable                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.EQ_PWR_Enable;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Left_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Right_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy                                = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Occupancy;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion                          = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Pass_Completion;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Abnormal;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode                           = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Operation_Mode;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag                               = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_Common_Tag; 		//동일 lane link 금지
		gm_OHTStatus->IOOutput->uOutputCommon.CID_LinkRequest                              = gm_OHTStatus->IOOutput->uOutput.Output_V85S.CID_LinkRequest;
		gm_OHTStatus->IOOutput->uOutputCommon.Blackbox_DO2                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Blackbox_DO2;
		gm_OHTStatus->IOOutput->uOutputCommon.Blackbox_DO1                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Blackbox_DO1;
		gm_OHTStatus->IOOutput->uOutputCommon.Blackbox_DO0                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85S.Blackbox_DO0;
//		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_F_Middle_Close;
//		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SHUTTER_R_Middle_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_78                             		   = gm_OHTStatus->IOOutput->uOutput.Output_V85S.SPARE_78;

	 }
	 else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_86)
	 {
		gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HAND_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm                                = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HAND_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HAND_Right_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HAND_Left_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close                                = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HAND_Close;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open                                 = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HAND_Open;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Push                                 = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HAND_Push;
		gm_OHTStatus->IOInput->uInputCommon.HNAD_Reserved_1		                      = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HNAD_Reserved_1;		// 미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_ES                                   = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HNAD_ES;               // 미사용
		gm_OHTStatus->IOInput->uInputCommon.HAND_PIO_Go                               = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HAND_PIO_Go;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.EQPIO_Load_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ                          = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.EQPIO_Unload_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready                               = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.EQPIO_Ready;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL                             = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.EQPIO_HO_AVBL;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES                                  = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.EQPIO_ES;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Right_Go                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.EQPIO_Right_Go;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Left_Go   		                  = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.EQPIO_Left_Go;   		//	BYTE EQPIO_Right_Go;미사용
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left                              = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.STEER_F_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left                              = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.STEER_R_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right                             = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.STEER_F_Right;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right                             = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.STEER_R_Right;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SHUTTER_F_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SHUTTER_R_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SHUTTER_F_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SHUTTER_R_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SHUTTER_F_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SHUTTER_R_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SHUTTER_F_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SHUTTER_R_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Input_Extend    	              = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.OBSTACLE_Input_Extend;     	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Long                             = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.OBSTACLE_Long;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Middle                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.OBSTACLE_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Short                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.OBSTACLE_Short;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Fail                             = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.OBSTACLE_Fail;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Input_Extend                    = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.OHTDETECT_Input_Extend;   	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Long                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.OHTDETECT_Long;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Middle                          = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.OHTDETECT_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Short                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.OHTDETECT_Short;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Fail                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.OHTDETECT_Fail;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.LOOKDOWN_Detect;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.LOOKDOWN_Error;
		gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home                                = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SHIFT_Home;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.STB_Detect_Left;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right                          = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.STB_Detect_Right;
		gm_OHTStatus->IOInput->uInputCommon.UTB_Detect                                = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.UTB_Detect;				// 미사용
		gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect                               = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.FOUP_Detect;
		gm_OHTStatus->IOInput->uInputCommon.OSCILLATION                               = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.OSCILLATION;
		gm_OHTStatus->IOInput->uInputCommon.EMO_Press                                 = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.EMO_Press;
		gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press                              = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.BUMPER_Press;
		gm_OHTStatus->IOInput->uInputCommon.FAN_Fail                                  = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.FAN_Fail;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect2                        = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HOIST_Belt_Detect2;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_51                        		  = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.Reserved_51;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.HANDTP_Connect                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HANDTP_Connect;				// 미사용;
		gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect                         = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.POD_EUV_1;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_Front                        = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.STOP_Driving_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_PreCheck                     = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.STOP_Driving_PreCheck;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Front                          = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.STOP_Trans_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Rear                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.STOP_Trans_Rear;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_OK                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.BCR_Driving_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_NG                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.BCR_Driving_NG;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_OK                              = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.BCR_Trans_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_NG                              = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.BCR_Trans_NG;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect1                        = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HOIST_Belt_Detect1;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_1                                = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.PIO_Input3;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_2                                = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.PIO_Input5;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_3                                = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.PIO_Input6;
		gm_OHTStatus->IOInput->uInputCommon.SMPS_Short                                = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SMPS_Short;
		gm_OHTStatus->IOInput->uInputCommon.CID_Link_Compt_IN                         = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.CID_Link_Compt_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Interlock_IN                          = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.CID_Interlock_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Pass_OK_IN                            = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.CID_Completion_ACK;
		gm_OHTStatus->IOInput->uInputCommon.CID_Status_IN                             = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.CID_Status_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN1                          = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.CID_NoRegistration;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN2                          = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.CID_Reserved_IN2;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home                          	  = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.HOIST_Home;
		gm_OHTStatus->IOInput->uInputCommon.Blackbox_DI2                          	  = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.Blackbox_DI2;
		gm_OHTStatus->IOInput->uInputCommon.Blackbox_DI1                          	  = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.Blackbox_DI1;
		gm_OHTStatus->IOInput->uInputCommon.Blackbox_DI0                          	  = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.Blackbox_DI0;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_0_Sensor                           = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.Rotate_0_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_90_Sensor                          = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.Rotate_90_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_180_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.Rotate_180_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_270_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.Rotate_270_Sensor; //미사용
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SHUTTER_F_Middle_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SHUTTER_R_Middle_Close;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_1                                 = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SPARE_7;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_2                                 = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.POD_EUV_2;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_9                                   = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SPARE_9;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_10                                  = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SPARE_10;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_11                           		  = gm_OHTStatus->IOInput->uInput.Input_V86_EUV.SPARE_11;

		//Output
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Close                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HAND_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Open                                    = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HAND_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Origin                                  = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HAND_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reset                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HAND_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Free                                    = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HAND_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved6                               = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HAND_Reserved6;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved7                               = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HAND_Reserved7;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved8                               = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HAND_Reserved8;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_HandTrigger                             = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HAND_HandTrigger;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_PIO_Select                              = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HAND_PIO_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid                                  = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.EQPIO_Valid;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.EQPIO_CS_0;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.EQPIO_CS_1;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_2                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.EQPIO_CS_2;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_3                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.EQPIO_CS_3;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req                                 = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.EQPIO_TR_Req;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.EQPIO_Busy;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete                               = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.EQPIO_Complete;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select                            = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.EQPIO_Left_Select;      //	BYTE EQPIO_Right_Select; 미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Right_Select                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.EQPIO_Right_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.STEER_F_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.STEER_F_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.STEER_R_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.STEER_R_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.STEER_F_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.STEER_R_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_F_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_R_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_F_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_R_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_F_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_R_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_F_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_R_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_F_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_R_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OBSTACLE_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OBSTACLE_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OBSTACLE_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OBSTACLE_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved40                          		   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.Reserved40;     //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OBSTACLE_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OHTDETECT_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OHTDETECT_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OHTDETECT_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OHTDETECT_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area5_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OHTDETECT_Area5_Sel;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Power_Reset                        = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OHTDETECT_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.LOOKDOWN_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.LOOKDOWN_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.LOOKDOWN_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.LOOKDOWN_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.LOOKDOWN_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Red                                     = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.LAMP_Red;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Right                            = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.LAMP_Yellow_Right;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.LAMP_Green;
//		gm_OHTStatus->IOOutput->uOutputCommon.Reserved56                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.Reserved56;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Left                             = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.LAMP_Yellow_Left;
//		gm_OHTStatus->IOOutput->uOutputCommon.Reserved58                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.Reserved58;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset                             = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HAND_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.BRAKE_Driving_Release                        = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.BRAKE_Driving_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Bumper_Clear                                 = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.Bumper_Clear;
		gm_OHTStatus->IOOutput->uOutputCommon.SAFETY_Reset                                 = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SAFETY_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HOIST_Ext_Brake_Release                      = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.HOIST_Ext_Brake_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved64                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.Reserved64;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved65                                   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.Reserved65;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area5_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.OBSTACLE_Area5_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.Hoist_Steer_PWR_Enable                       = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.Hoist_Steer_PWR_Enable;
		gm_OHTStatus->IOOutput->uOutputCommon.IO_PWR_Enable                                = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.IO_PWR_Enable;
		gm_OHTStatus->IOOutput->uOutputCommon.EQ_PWR_Enable                                = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.EQ_PWR_Enable;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.CID_Left_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON                          = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.CID_Right_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy                                = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.CID_Occupancy;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion                          = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.CID_Pass_Completion;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal                                 = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.CID_Abnormal;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode                           = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.CID_Operation_Mode;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag                               = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.CID_Common_Tag; 		//동일 lane link 금지
		gm_OHTStatus->IOOutput->uOutputCommon.CID_LinkRequest                              = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.CID_LinkRequest;
		gm_OHTStatus->IOOutput->uOutputCommon.Blackbox_DO2                                 = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.Blackbox_DO2;
		gm_OHTStatus->IOOutput->uOutputCommon.Blackbox_DO1                                 = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.Blackbox_DO1;
		gm_OHTStatus->IOOutput->uOutputCommon.Blackbox_DO0                                 = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.Blackbox_DO0;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_F_Middle_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SHUTTER_R_Middle_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_78                             		   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SPARE_78;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_79                             		   = gm_OHTStatus->IOOutput->uOutput.Output_V86_EUV.SPARE_79;

	 }
	 else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_85)
	 {
		 ////AOHC-286
		 //Input
		gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HAND_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm                                = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HAND_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HAND_Right_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HAND_Left_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close                                = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HAND_Close;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open                                 = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HAND_Open;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Push                                 = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HAND_Push;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home		                          = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HOIST_Home;		// 미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_ES                                   = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HNAD_ES;               // 미사용
		gm_OHTStatus->IOInput->uInputCommon.HAND_PIO_Go                               = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HAND_PIO_Go;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.EQPIO_Load_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ                          = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.EQPIO_Unload_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready                               = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.EQPIO_Ready;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL                             = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.EQPIO_HO_AVBL;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES                                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.EQPIO_ES;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Left_Go   		                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.EQPIO_Left_Go;   		//	BYTE EQPIO_Right_Go;미사용
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left                              = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.STEER_F_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left                              = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.STEER_R_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right                             = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.STEER_F_Right;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right                             = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.STEER_R_Right;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SHUTTER_F_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SHUTTER_R_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SHUTTER_F_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SHUTTER_R_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SHUTTER_F_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SHUTTER_R_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SHUTTER_F_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SHUTTER_R_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Input_Extend    	              = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.OBSTACLE_Input_Extend;     	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Long                             = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.OBSTACLE_Long;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Middle                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.OBSTACLE_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Short                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.OBSTACLE_Short;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Fail                             = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.OBSTACLE_Fail;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Input_Extend                    = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.OHTDETECT_Input_Extend;   	// 미사용
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Long                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.OHTDETECT_Long;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Middle                          = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.OHTDETECT_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Short                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.OHTDETECT_Short;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Fail                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.OHTDETECT_Fail;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.LOOKDOWN_Detect;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.LOOKDOWN_Error;
		gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home                                = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SHIFT_Home;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.STB_Detect_Left;
		gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right                          = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.STB_Detect_Right;
		gm_OHTStatus->IOInput->uInputCommon.UTB_Detect                                = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.UTB_Detect;				// 미사용
		gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect                               = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.FOUP_Detect;
		gm_OHTStatus->IOInput->uInputCommon.OSCILLATION                               = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.OSCILLATION;
		gm_OHTStatus->IOInput->uInputCommon.EMO_Press                                 = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.EMO_Press;
		gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press                              = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.BUMPER_Press;
		gm_OHTStatus->IOInput->uInputCommon.FAN_Fail                                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.FAN_Fail;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect1                        = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HOIST_Belt_Detect1;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Belt_Detect2                        = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HOIST_Belt_Detect2;	// Reserved48;
		gm_OHTStatus->IOInput->uInputCommon.TP_Enable                                 = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.TP_Enable;				// 미사용;
		gm_OHTStatus->IOInput->uInputCommon.HANDTP_Connect                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HANDTP_Connect;
		gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect                         = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.FOUB_Cover_Detect;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_Front                        = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.STOP_Driving_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_PreCheck                     = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.STOP_Driving_PreCheck;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Front                          = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.STOP_Trans_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Rear                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.STOP_Trans_Rear;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_OK                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.BCR_Driving_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_NG                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.BCR_Driving_NG;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_OK                              = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.BCR_Trans_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_NG                              = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.BCR_Trans_NG;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_1                                = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.Reserved_1;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_2                                = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.Reserved_2;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_3                                = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.Reserved_3;
		gm_OHTStatus->IOInput->uInputCommon.SMPS_Short                                = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SMPS_Short;
		gm_OHTStatus->IOInput->uInputCommon.CID_Link_Compt_IN                         = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.CID_Link_Compt_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Interlock_IN                          = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.CID_Interlock_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Pass_OK_IN                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.CID_Completion_ACK;
		gm_OHTStatus->IOInput->uInputCommon.CID_Status_IN                             = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.CID_Status_IN;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN1                          = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.CID_Reserved_IN1;
		gm_OHTStatus->IOInput->uInputCommon.CID_Reserved_IN2                          = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.CID_Reserved_IN2;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_0_Sensor                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.Rotate_0_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_90_Sensor                          = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.Rotate_90_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_180_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.Rotate_180_Sensor;
		gm_OHTStatus->IOInput->uInputCommon.Rotate_270_Sensor                         = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.Rotate_270_Sensor; //미사용
		gm_OHTStatus->IOInput->uInputCommon.HNAD_Reserved_1                           = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HNAD_Reserved_1;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home_2                              = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.HOIST_Home_2;     // 미사용
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SHUTTER_F_Middle_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Middle_Close                    = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SHUTTER_R_Middle_Close;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_1                                 = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.POD_EUV_1;
		gm_OHTStatus->IOInput->uInputCommon.POD_EUV_2                                 = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.POD_EUV_2;
		gm_OHTStatus->IOInput->uInputCommon.BlackBox_Error                            = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_5;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_6                                   = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_6;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_7                                   = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_7;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_8                                   = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_8;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_9                                   = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_9;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_10                                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_10;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_11                                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_11;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_12                                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_12;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_13                                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_13;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_14                                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_14;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_15                                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_15;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_16                                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_16;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_17                                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_17;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_18                                  = gm_OHTStatus->IOInput->uInput.Input_V85_EUV.SPARE_18;

		//Output
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Close                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HAND_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Open                                    = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HAND_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Origin                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HAND_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reset                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HAND_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Free                                    = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HAND_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved6                               = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HAND_Reserved6;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved7                               = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HAND_Reserved7;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reserved8                               = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HAND_Reserved8;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_HandTrigger                             = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HAND_HandTrigger;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_PIO_Select                              = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HAND_PIO_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.EQPIO_Valid;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.EQPIO_CS_0;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.EQPIO_CS_1;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_2                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.EQPIO_CS_2;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_3                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.EQPIO_CS_3;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.EQPIO_TR_Req;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.EQPIO_Busy;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete                               = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.EQPIO_Complete;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select                            = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.EQPIO_Left_Select;      //	BYTE EQPIO_Right_Select; 미사용
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.STEER_F_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.STEER_F_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.STEER_R_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.STEER_R_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.STEER_F_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.STEER_R_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_F_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_R_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_F_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_R_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_F_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_R_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_F_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_R_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_F_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_R_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OBSTACLE_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OBSTACLE_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OBSTACLE_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OBSTACLE_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area5_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OBSTACLE_Area5_Sel;     //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OBSTACLE_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OHTDETECT_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OHTDETECT_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OHTDETECT_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OHTDETECT_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area5_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OHTDETECT_Area5_Sel;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Power_Reset                        = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.OHTDETECT_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.LOOKDOWN_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.LOOKDOWN_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.LOOKDOWN_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.LOOKDOWN_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.LOOKDOWN_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Red                                     = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.LAMP_Red;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Right                            = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.LAMP_Yellow_Right;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Left                             = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.LAMP_Yellow_Left;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.LAMP_Green;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset                             = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HAND_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.BRAKE_Driving_Release                        = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.BRAKE_Driving_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Bumper_Clear                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.Bumper_Clear;
		gm_OHTStatus->IOOutput->uOutputCommon.SAFETY_Reset                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SAFETY_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HOIST_Ext_Brake_Release                      = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.HOIST_Ext_Brake_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.QRR_Power                                    = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.QRR_Power;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved62                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.Reserved62;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved63                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.Reserved63;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved64                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.Reserved64;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved65                                   = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.Reserved65;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Left_Select_ON                           = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.CID_Left_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Right_Select_ON                          = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.CID_Right_Select_ON;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Occupancy                                = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.CID_Occupancy;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Pass_Completion                          = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.CID_Pass_Completion;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Abnormal                                 = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.CID_Abnormal;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Operation_Mode                           = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.CID_Operation_Mode;
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Common_Tag                               = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.CID_Common_Tag; 		//동일 lane link 금지
		gm_OHTStatus->IOOutput->uOutputCommon.CID_Reserved_OUT2                            = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.CID_Reserved_OUT2;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_74                                      = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SPARE_74;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_75                                      = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SPARE_75;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_F_Middle_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Middle_Close                       = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SHUTTER_R_Middle_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.BlackBox_Output                             = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SPARE_78;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_79                                      = gm_OHTStatus->IOOutput->uOutput.Output_V85_EUV.SPARE_79;
	 }
	 else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_10)  //P4 Mask
	 {
		//Input
		gm_OHTStatus->IOInput->uInputCommon.HAND_Inposition                           = gm_OHTStatus->IOInput->uInput.Input_MASK.HAND_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Alarm                                = gm_OHTStatus->IOInput->uInput.Input_MASK.HAND_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip                           = gm_OHTStatus->IOInput->uInput.Input_MASK.HAND_Right_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip                            = gm_OHTStatus->IOInput->uInput.Input_MASK.HAND_Left_Grip;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close								  = gm_OHTStatus->IOInput->uInput.Input_MASK.HAND_Home;
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open								  = gm_OHTStatus->IOInput->uInput.Input_MASK.HAND_Work;
		gm_OHTStatus->IOInput->uInputCommon.HNAD_ES                                   = gm_OHTStatus->IOInput->uInput.Input_MASK.HAND_ES;               // 미사용
		gm_OHTStatus->IOInput->uInputCommon.HAND_PIO_Go                               = gm_OHTStatus->IOInput->uInput.Input_MASK.HAND_PIO_Go;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Load_REQ                            = gm_OHTStatus->IOInput->uInput.Input_MASK.EQPIO_Load_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Unload_REQ                          = gm_OHTStatus->IOInput->uInput.Input_MASK.EQPIO_Unload_REQ;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Ready                               = gm_OHTStatus->IOInput->uInput.Input_MASK.EQPIO_Ready;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_HO_AVBL                             = gm_OHTStatus->IOInput->uInput.Input_MASK.EQPIO_HO_AVBL;
		gm_OHTStatus->IOInput->uInputCommon.EQPIO_ES                                  = gm_OHTStatus->IOInput->uInput.Input_MASK.EQPIO_ES;

		gm_OHTStatus->IOInput->uInputCommon.EQPIO_Left_Go   		                  = gm_OHTStatus->IOInput->uInput.Input_MASK.EQPIO_Left_Go;
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left                              = gm_OHTStatus->IOInput->uInput.Input_MASK.STEER_F_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left                              = gm_OHTStatus->IOInput->uInput.Input_MASK.STEER_R_Left;
		gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right                             = gm_OHTStatus->IOInput->uInput.Input_MASK.STEER_F_Right;
		gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right                             = gm_OHTStatus->IOInput->uInput.Input_MASK.STEER_R_Right;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close                           = gm_OHTStatus->IOInput->uInput.Input_MASK.SHUTTER_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open                            = gm_OHTStatus->IOInput->uInput.Input_MASK.SHUTTER_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_MASK.SHUTTER_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_MASK.SHUTTER_Inposition;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close                           = gm_OHTStatus->IOInput->uInput.Input_MASK.SHUTTER_Close;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open                            = gm_OHTStatus->IOInput->uInput.Input_MASK.SHUTTER_Open;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Alarm                           = gm_OHTStatus->IOInput->uInput.Input_MASK.SHUTTER_Alarm;
		gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Inposition                      = gm_OHTStatus->IOInput->uInput.Input_MASK.SHUTTER_Inposition;

		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Long                             = gm_OHTStatus->IOInput->uInput.Input_MASK.OBSTACLE_Long;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Middle                           = gm_OHTStatus->IOInput->uInput.Input_MASK.OBSTACLE_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Short                            = gm_OHTStatus->IOInput->uInput.Input_MASK.OBSTACLE_Short;
		gm_OHTStatus->IOInput->uInputCommon.OBSTACLE_Fail                             = gm_OHTStatus->IOInput->uInput.Input_MASK.OBSTACLE_Fail;

		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Long                            = gm_OHTStatus->IOInput->uInput.Input_MASK.OHTDETECT_Long;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Middle                          = gm_OHTStatus->IOInput->uInput.Input_MASK.OHTDETECT_Middle;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Short                           = gm_OHTStatus->IOInput->uInput.Input_MASK.OHTDETECT_Short;
		gm_OHTStatus->IOInput->uInputCommon.OHTDETECT_Fail                            = gm_OHTStatus->IOInput->uInput.Input_MASK.OHTDETECT_Fail;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect                           = gm_OHTStatus->IOInput->uInput.Input_MASK.LOOKDOWN_Detect;
		gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error                            = gm_OHTStatus->IOInput->uInput.Input_MASK.LOOKDOWN_Error;

		gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home                                = gm_OHTStatus->IOInput->uInput.Input_MASK.SHIFT_Home;
		gm_OHTStatus->IOInput->uInputCommon.UTB_Detect                                = gm_OHTStatus->IOInput->uInput.Input_MASK.UTB_Detect;
		gm_OHTStatus->IOInput->uInputCommon.OSCILLATION                               = gm_OHTStatus->IOInput->uInput.Input_MASK.OSCILLATION;
		gm_OHTStatus->IOInput->uInputCommon.EMO_Press                                 = gm_OHTStatus->IOInput->uInput.Input_MASK.EMO_Press;
		gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press                              = gm_OHTStatus->IOInput->uInput.Input_MASK.BUMPER_Press;

		gm_OHTStatus->IOInput->uInputCommon.Reserved_51                        		  = gm_OHTStatus->IOInput->uInput.Input_MASK.Reserved_52;	// Reserved48;

//		gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect                         = gm_OHTStatus->IOInput->uInput.Input_MASK.POD_EUV_1;

		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_Front                        = gm_OHTStatus->IOInput->uInput.Input_MASK.STOP_Driving_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Driving_PreCheck                     = gm_OHTStatus->IOInput->uInput.Input_MASK.STOP_Driving_PreCheck;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Front                          = gm_OHTStatus->IOInput->uInput.Input_MASK.STOP_Trans_Front;
		gm_OHTStatus->IOInput->uInputCommon.STOP_Trans_Rear                           = gm_OHTStatus->IOInput->uInput.Input_MASK.STOP_Trans_Rear;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_OK                            = gm_OHTStatus->IOInput->uInput.Input_MASK.BCR_Driving_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Driving_NG                            = gm_OHTStatus->IOInput->uInput.Input_MASK.BCR_Driving_NG;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_OK                              = gm_OHTStatus->IOInput->uInput.Input_MASK.BCR_Trans_OK;
		gm_OHTStatus->IOInput->uInputCommon.BCR_Trans_NG                              = gm_OHTStatus->IOInput->uInput.Input_MASK.BCR_Trans_NG;

		gm_OHTStatus->IOInput->uInputCommon.Reserved_1                                = gm_OHTStatus->IOInput->uInput.Input_MASK.Reserved_64;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_2                                = gm_OHTStatus->IOInput->uInput.Input_MASK.Reserved_65;
		gm_OHTStatus->IOInput->uInputCommon.Reserved_3                                = gm_OHTStatus->IOInput->uInput.Input_MASK.Reserved_66;
		gm_OHTStatus->IOInput->uInputCommon.SMPS_Short                                = gm_OHTStatus->IOInput->uInput.Input_MASK.SMPS_Short;
		gm_OHTStatus->IOInput->uInputCommon.HOIST_Home                          	  = gm_OHTStatus->IOInput->uInput.Input_MASK.HOIST_Home;

		gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home                                = gm_OHTStatus->IOInput->uInput.Input_MASK.SHIFT_Home;

		gm_OHTStatus->IOInput->uInputCommon.SPARE_9                                   = gm_OHTStatus->IOInput->uInput.Input_MASK.SPARE_9;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_10                                  = gm_OHTStatus->IOInput->uInput.Input_MASK.SPARE_10;
		gm_OHTStatus->IOInput->uInputCommon.SPARE_11                           		  = gm_OHTStatus->IOInput->uInput.Input_MASK.SPARE_11;
		gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect                               = gm_OHTStatus->IOInput->uInput.Input_MASK.Pod_Detect;

		//Output
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Origin                                  = gm_OHTStatus->IOOutput->uOutput.Output_MASK.HAND_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Reset                                   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.HAND_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Free                                    = gm_OHTStatus->IOOutput->uOutput.Output_MASK.HAND_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_HandTrigger                             = gm_OHTStatus->IOOutput->uOutput.Output_MASK.HAND_HandTrigger;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_PIO_Select                              = gm_OHTStatus->IOOutput->uOutput.Output_MASK.HAND_PIO_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Valid                                  = gm_OHTStatus->IOOutput->uOutput.Output_MASK.EQPIO_Valid;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_0                                   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.EQPIO_CS_0;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_1                                   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.EQPIO_CS_1;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_2                                   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.EQPIO_CS_2;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_CS_3                                   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.EQPIO_CS_3;        //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_TR_Req                                 = gm_OHTStatus->IOOutput->uOutput.Output_MASK.EQPIO_TR_Req;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Busy                                   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.EQPIO_Busy;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Complete                               = gm_OHTStatus->IOOutput->uOutput.Output_MASK.EQPIO_Complete;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Left_Select                            = gm_OHTStatus->IOOutput->uOutput.Output_MASK.EQPIO_Left_Select;
		gm_OHTStatus->IOOutput->uOutputCommon.EQPIO_Right_Select                           = gm_OHTStatus->IOOutput->uOutput.Output_MASK.EQPIO_Right_Select;   // 미사용
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_MASK.STEER_F_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_MASK.STEER_F_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_0                                = gm_OHTStatus->IOOutput->uOutput.Output_MASK.STEER_R_TRQ_0;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_TRQ_1                                = gm_OHTStatus->IOOutput->uOutput.Output_MASK.STEER_R_TRQ_1;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_F_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_MASK.STEER_F_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.STEER_R_Dir                                  = gm_OHTStatus->IOOutput->uOutput.Output_MASK.STEER_R_Dir;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SHUTTER_F_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Reset                              = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SHUTTER_R_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SHUTTER_F_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Close                              = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SHUTTER_R_Close;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SHUTTER_F_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Open                               = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SHUTTER_R_Open;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SHUTTER_F_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Origin                             = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SHUTTER_R_Origin;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_F_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SHUTTER_F_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.SHUTTER_R_Free                               = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SHUTTER_R_Free;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OBSTACLE_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OBSTACLE_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OBSTACLE_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OBSTACLE_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved40                          		   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.Reserved40;     //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OBSTACLE_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OHTDETECT_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OHTDETECT_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OHTDETECT_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OHTDETECT_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area5_Sel                          = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OHTDETECT_Area5_Sel;    //미사용
		gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Power_Reset                        = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OHTDETECT_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_MASK.LOOKDOWN_Area1_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_MASK.LOOKDOWN_Area2_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_MASK.LOOKDOWN_Area3_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_MASK.LOOKDOWN_Area4_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Power_Reset                         = gm_OHTStatus->IOOutput->uOutput.Output_MASK.LOOKDOWN_Power_Reset;
//		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Red                                     = gm_OHTStatus->IOOutput->uOutput.Output_MASK.LAMP_Red;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Right                            = gm_OHTStatus->IOOutput->uOutput.Output_MASK.LAMP_Yellow_Right;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Green                                   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.LAMP_Green;
		gm_OHTStatus->IOOutput->uOutputCommon.LAMP_Yellow_Left                             = gm_OHTStatus->IOOutput->uOutput.Output_MASK.LAMP_Yellow_Left;
		gm_OHTStatus->IOOutput->uOutputCommon.HAND_Power_Reset                             = gm_OHTStatus->IOOutput->uOutput.Output_MASK.HAND_Power_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.BRAKE_Driving_Release                        = gm_OHTStatus->IOOutput->uOutput.Output_MASK.BRAKE_Driving_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Bumper_Clear                                 = gm_OHTStatus->IOOutput->uOutput.Output_MASK.Bumper_Clear;
		gm_OHTStatus->IOOutput->uOutputCommon.SAFETY_Reset                                 = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SAFETY_Reset;
		gm_OHTStatus->IOOutput->uOutputCommon.HOIST_Ext_Brake_Release                      = gm_OHTStatus->IOOutput->uOutput.Output_MASK.HOIST_Ext_Brake_Release;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved64                                   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.Reserved64;
		gm_OHTStatus->IOOutput->uOutputCommon.Reserved65                                   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.Reserved65;
		gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area5_Sel                           = gm_OHTStatus->IOOutput->uOutput.Output_MASK.OBSTACLE_Area5_Sel;
		gm_OHTStatus->IOOutput->uOutputCommon.Hoist_Steer_PWR_Enable                       = gm_OHTStatus->IOOutput->uOutput.Output_MASK.Hoist_Steer_PWR_Enable;
		gm_OHTStatus->IOOutput->uOutputCommon.IO_PWR_Enable                                = gm_OHTStatus->IOOutput->uOutput.Output_MASK.IO_PWR_Enable;
		gm_OHTStatus->IOOutput->uOutputCommon.EQ_PWR_Enable                                = gm_OHTStatus->IOOutput->uOutput.Output_MASK.EQ_PWR_Enable;

		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_78                             		   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SPARE_78;
		gm_OHTStatus->IOOutput->uOutputCommon.SPARE_79                             		   = gm_OHTStatus->IOOutput->uOutput.Output_MASK.SPARE_79;
	 }


	IOMapForm->IO_Display();
}
//---------------------------------------------------------------------------

void __fastcall TStatusForm::CmdBuffClear1Click(TObject *Sender)
{
   String strTmpMessage;

   strTmpMessage = "Cmd Buffer Clear ... OK?";

   if(Application->MessageBox(strTmpMessage.w_str(),L"Confirm",MB_OKCANCEL) == IDOK)
   {
	  TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
	  m_CMDControl->MakeTPCmd2OHT(TPCMD_CMD_BUFFER_CLEAR);
	  delete m_CMDControl;

	  MainForm->DataDisplay(COLOR_BLACK, "Cmd Buffer Clear");
   }
}
//---------------------------------------------------------------------------

void __fastcall TStatusForm::CmdBuffSave1Click(TObject *Sender)
{
   String strTmpMessage;

   strTmpMessage = "Cmd Buffer Save ... OK?";

   if(Application->MessageBox(strTmpMessage.w_str(),L"Confirm",MB_OKCANCEL) == IDOK)
   {
      TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
      m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_CMDBUFF_SAVE);
      delete m_CMDControl;

      MainForm->DataDisplay(COLOR_BLACK, "Cmd Buffer Save");
   }

}
//---------------------------------------------------------------------------

/**
@brief   OHT MOTOR상태를 분석하기 위한 함수
@author  puting
@date    2012.12.13
@param   MOTORINFO
*/
void TStatusForm::AnalysisMotorInfo(MSG_STATUS_OHT2TP *pCmdStatus)
{
    gm_OHTStatus->bReceiveStatus = true;

    if(MainForm->pbCommStatus->Visible == false)
        MainForm->pbCommStatus->Visible = true;

    MainForm->CommOK();

    // by zzang9un 2012.11.26 : Hand, Shutter Origin 체크하는 부분 추가
    gm_OHTStatus->bIs_Hand_Origin = pCmdStatus->Data.OriginInfo.Hand;
    gm_OHTStatus->bIs_Shutter_Origin = pCmdStatus->Data.OriginInfo.Shutter;

    // by zzang9un 2012.11.24 : Motor 정보
    //*** 1축 : Driving Front ***
    // by zzang9un 2012.11.24 : OHT로부터 mm로 받기 때문에 펄스값은 환산해서 대입
    gm_OHTStatus->Motor_Driving_F->dPosition_cts = pCmdStatus->Driving_F.Position * 113.3736777;
    gm_OHTStatus->Motor_Driving_F->dPosition_mm  = pCmdStatus->Driving_F.Position;

	/*
		바퀴지름 125mm임. 따라서 원 둘레는 392.5mm 임
		이동 속도(mm/s)를 이용하여 계산하면
		=> 1분에 392.5 mm 를 가면 1rpm
		=> (이동속도 * 60) / 392.5 하면 됨
	*/
	gm_OHTStatus->Motor_Driving_F->dVelocity_rpm = (pCmdStatus->Driving_F.Speed * 60) / 392.5;
	gm_OHTStatus->Motor_Driving_F->dVelocity_mpersec= pCmdStatus->Driving_F.Speed;    //
    gm_OHTStatus->Motor_Driving_F->dVelocity_ctspermsec= 0;
    gm_OHTStatus->Motor_Driving_F->nError = pCmdStatus->Driving_F.ErrorCode;
    gm_OHTStatus->Motor_Driving_F->nErrorCode = pCmdStatus->Driving_F.ErrorCode;
    gm_OHTStatus->Motor_Driving_F->bIs_ServoON = pCmdStatus->Driving_F.AMPEnable;
    gm_OHTStatus->Motor_Driving_F->nAxis_Running = pCmdStatus->Driving_F.IsRunning;

	gm_OHTStatus->Motor_Driving_F->nAxis_source_code = pCmdStatus->Driving_F.ErrorCode;


    //*** 2축 : Driving Rear ***
    gm_OHTStatus->Motor_Driving_R->dPosition_cts = pCmdStatus->Driving_R.Position * 113.3736777;
    gm_OHTStatus->Motor_Driving_R->dPosition_mm  = pCmdStatus->Driving_R.Position;
	gm_OHTStatus->Motor_Driving_R->dVelocity_rpm = (pCmdStatus->Driving_R.Speed * 60) / 392.5;
    gm_OHTStatus->Motor_Driving_R->dVelocity_mpersec=pCmdStatus->Driving_R.Speed;
    gm_OHTStatus->Motor_Driving_R->dVelocity_ctspermsec=0;
    gm_OHTStatus->Motor_Driving_R->nError = pCmdStatus->Driving_R.ErrorCode;
    gm_OHTStatus->Motor_Driving_R->nErrorCode = pCmdStatus->Driving_R.ErrorCode;
    gm_OHTStatus->Motor_Driving_R->bIs_ServoON = pCmdStatus->Driving_R.AMPEnable;
    gm_OHTStatus->Motor_Driving_R->nAxis_Running = pCmdStatus->Driving_R.IsRunning;

	gm_OHTStatus->Motor_Driving_R->nAxis_source_code = pCmdStatus->Driving_R.ErrorCode;


    //*** 3축 : Hoist ***
	gm_OHTStatus->Motor_Hoist->dPosition_cts = pCmdStatus->dHoist_pulse;
    // by zzang9un 2013.09.09 : 프로토콜 v2.9에 의해 단위 수정함(1/10000m)
    gm_OHTStatus->Motor_Hoist->dPosition_mm  = pCmdStatus->Hoist.Position / 10.0;
    gm_OHTStatus->Motor_Hoist->dVelocity_rpm = 0;
    gm_OHTStatus->Motor_Hoist->dVelocity_mpersec=pCmdStatus->Hoist.Speed;
    gm_OHTStatus->Motor_Hoist->dVelocity_ctspermsec=0;
    gm_OHTStatus->Motor_Hoist->nError = pCmdStatus->Hoist.ErrorCode;
    gm_OHTStatus->Motor_Hoist->nErrorCode = pCmdStatus->Hoist.ErrorCode;
    gm_OHTStatus->Motor_Hoist->bIs_ServoON = pCmdStatus->Hoist.AMPEnable;
    gm_OHTStatus->Motor_Hoist->nAxis_Running = pCmdStatus->Hoist.IsRunning;

	gm_OHTStatus->Motor_Hoist->nAxis_source_code = pCmdStatus->Hoist.ErrorCode;


    //*** 4축 : Shift ***
    gm_OHTStatus->Motor_Shift->dPosition_cts = pCmdStatus->Shift.Position * 682.6666667;
    // by zzang9un 2013.09.09 : 프로토콜 v2.9에 의해 단위 수정함(1/10000m)
    gm_OHTStatus->Motor_Shift->dPosition_mm  = pCmdStatus->Shift.Position / 10.0;
    gm_OHTStatus->Motor_Shift->dVelocity_rpm = 0;
    gm_OHTStatus->Motor_Shift->dVelocity_mpersec=pCmdStatus->Shift.Speed;
    gm_OHTStatus->Motor_Shift->dVelocity_ctspermsec=0;
    gm_OHTStatus->Motor_Shift->nError = pCmdStatus->Shift.ErrorCode;
    gm_OHTStatus->Motor_Shift->nErrorCode = pCmdStatus->Shift.ErrorCode;
    gm_OHTStatus->Motor_Shift->bIs_ServoON = pCmdStatus->Shift.AMPEnable;
    gm_OHTStatus->Motor_Shift->nAxis_Running = pCmdStatus->Shift.IsRunning;

	gm_OHTStatus->Motor_Shift->nAxis_source_code = pCmdStatus->Shift.ErrorCode;



    // 2016-12-01, Add by Labeler
    // Rotate 관련 표시 추가
    gm_OHTStatus->Motor_Rotate->dPosition_mm  = pCmdStatus->Rotate.Position;
    gm_OHTStatus->Motor_Rotate->dVelocity_rpm = 0;
    gm_OHTStatus->Motor_Rotate->dVelocity_mpersec = pCmdStatus->Rotate.Speed;
    gm_OHTStatus->Motor_Rotate->dVelocity_ctspermsec = 0;
    gm_OHTStatus->Motor_Rotate->nError = pCmdStatus->Rotate.ErrorCode;
    gm_OHTStatus->Motor_Rotate->nErrorCode = pCmdStatus->Rotate.ErrorCode;
    gm_OHTStatus->Motor_Rotate->bIs_ServoON = pCmdStatus->Rotate.AMPEnable;
    gm_OHTStatus->Motor_Rotate->nAxis_Running = pCmdStatus->Rotate.IsRunning;
    gm_OHTStatus->Motor_Rotate->bIs_Origin = (bool)pCmdStatus->Rotate.IsOrigin;
}
//---------------------------------------------------------------------------
/**
@brief   OHT Use Data 상태를 분석하기 위한 함수
@author  puting
@date    2012.12.13
@param  Usa Data
*/
void TStatusForm::Analysis_Use_Data(uCMD_USE_DATA *pUseInfo)
{
   //puting 2012.11.29
   gm_OHTStatus->CheckOnOff->bUse_BCR             = (pUseInfo->Info.BCR ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_PBSSensor       = (pUseInfo->Info.PBSSensor ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_SoundAlarm      = (pUseInfo->Info.SoundAlarm ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_LampIndi        = (pUseInfo->Info.Lamp ==1) ? true : false;

   gm_OHTStatus->CheckOnOff->bUse_ShutterControl  = (pUseInfo->Info.ShutterControl ==1) ? true : false;

   gm_OHTStatus->nIs_DirectLogSave                =  pUseInfo->Info.DirectLogSave;
   gm_OHTStatus->CheckOnOff->bUse_EQPIO           = (pUseInfo->Info.EQ_PIO ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_STBSensor       = (pUseInfo->Info.STBSensor ==1) ? true : false;

   gm_OHTStatus->CheckOnOff->bUse_HandLimitSensorCheck = (pUseInfo->Info.HandLimitSensorCheck ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_HandControl          = (pUseInfo->Info.HandControl ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_SteeringControl      = (pUseInfo->Info.SteeringControl ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_UTBSensor            = (pUseInfo->Info.UTBSensor ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_LookDownSensor       = (pUseInfo->Info.LookDownSensor ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_HandPushSensorCheck  = (pUseInfo->Info.HandPushSensorCheck ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_GripperSen           = (pUseInfo->Info.GripperSen ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_HoistAlignSen        = (pUseInfo->Info.HoistAlignSensor ==1) ? true : false;

   gm_OHTStatus->CheckOnOff->bUse_OscillationSen = (pUseInfo->Info.OscillationSensor ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_ErrSound       = (pUseInfo->Info.ErrSound ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_DrivingSound   = (pUseInfo->Info.DrivingSound ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_TransSound     = (pUseInfo->Info.TransSound ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_ObsDetectSound = (pUseInfo->Info.ObsDetectSound ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_OhtDetectSound = (pUseInfo->Info.OhtDetectSound ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_IgnorePIOError        = (pUseInfo->Info.IgnorePIOError ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_HoistHomePos_PIOError = (pUseInfo->Info.HoistHomePos_PIOError ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_ObsIgnoreSound        = (pUseInfo->Info.ObsIgnoreSound ==1) ? true : false;

   gm_OHTStatus->CheckOnOff->bUse_LocSave_Trans_ing     = (pUseInfo->Info.LocSave_Trans_ing ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_PushAbnormalOnCheck   = (pUseInfo->Info.PushAbnormalOnCheck ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_UTBFoupCheck          = (pUseInfo->Info.UTBFoupCheck ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_InnerFoupDetectSen    = (pUseInfo->Info.InnerFoupDetectSen ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_TransEQSound     = (pUseInfo->Info.TransEQSound ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_TransSTKSound     = (pUseInfo->Info.TransSTKSound ==1) ? true : false;
   gm_OHTStatus->CheckOnOff->bUse_TransSTBSound     = (pUseInfo->Info.TransSTBSound ==1) ? true : false;

}
//---------------------------------------------------------------------------

/**
@brief   StatusForm Display를 Update하는 Timer 함수
@author  zzang9un
@date    2013.03.18
@param   Sender : TObject
*/
void __fastcall TStatusForm::tmDisplayUpdateTimer(TObject *Sender)
{
    String strPreErrString;

    panCMDType->Caption = gm_OHTStatus->nTPCommandType;

    // by zzang9un 2012.11.26 : Mode에 따라 출력하는 부분 수정
    panMode->Caption = GetStringMode(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode);

    // by zzang9un 2012.11.26 : Status 표시 부분 추가
	panStatus->Caption = GetStringStatus(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status);

	// by zzang9un 2013.01.31 : Node ID는 Hex로 출력함(6자리로 제한함)
	// by zzang9un 2013.04.10 : CurNodeOffset 출력

	/* shkim. 2016.03.14 화면표시 10진수로 변경
	panCurrentNodeID->Caption = IntToHex((int)gm_OHTStatus->m_OHTStatus.Data.CurNodeID, 6) +
								"(" + String(gm_OHTStatus->m_OHTStatus.Data.CurNodeOffset) + ")";
    panTargetNodeID->Caption = IntToHex((int)gm_OHTStatus->m_OHTStatus.Data.StopNodeID, 6);
    panCurrentStationID->Caption = IntToHex((int)gm_OHTStatus->m_OHTStatus.Data.CurStationID, 6);
    panTargetStationID->Caption = IntToHex((int)gm_OHTStatus->m_OHTStatus.Data.StopStationID, 6) +
        "[" + String(gm_OHTStatus->m_OHTStatus.Data.StopStationOffset) + "]";
    */

	panCurrentNodeID->Caption = IntToStr((int)gm_OHTStatus->m_OHTStatus.Data.CurNodeID) +
								"(" + String(gm_OHTStatus->m_OHTStatus.Data.CurNodeOffset) + ")";

	panTargetNodeID->Caption = IntToStr((int)gm_OHTStatus->m_OHTStatus.Data.StopNodeID);
	panCurrentStationID->Caption = IntToStr((int)gm_OHTStatus->m_OHTStatus.Data.CurStationID);
    panTargetStationID->Caption = IntToStr((int)gm_OHTStatus->m_OHTStatus.Data.StopStationID) +
        "[" + String(gm_OHTStatus->m_OHTStatus.Data.StopStationOffset) + "]";

    //**CmdCount
    panCmdNumber->Caption = IntToStr(gm_OHTStatus->nCmdCount);
    if(gm_OHTStatus->nCmdCount==0)
    {
        if(gm_OHTStatus->bSendMapData==true)
        {
            WaitForm->Close();
            gm_OHTStatus->bSendMapData=false;
        }
        panCmdNumber->Color = COLOR_BLACK;
    }
    else
    {
        panCmdNumber->Color = COLOR_FUCHSIA;
    }

    // 에러 유무에 따라 panel 색상 변경
    if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode)
        panErrCode->Color = COLOR_RED;
    else
        panErrCode->Color = COLOR_BLACK;

    // **Errorcode
    panErrCode->Caption = gm_OHTStatus->m_OHTStatus.Data.ErrorCode;

    // by zzang9un 2013.02.17 : 에러 코드를 ini파일에서 불러오는 부분 임시 추가
    // 추후 별도 클래스나 함수로 나눠야 함
    if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode)
    {
        String ErrorCodeFileName;
        ErrorCodeFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_ERRORCODE;

        TIniFile *ErrIni = new TIniFile(ErrorCodeFileName);

        String strErrorCode = "";
        strErrorCode.sprintf(L"E%04d", gm_OHTStatus->m_OHTStatus.Data.ErrorCode);

        gm_OHTStatus->strErrorString = ErrIni->ReadString("ErrorCode", strErrorCode, "코드없음");
    }


    if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode)
    {
        // by zzang9un 2013.02.21 : 에러 코드가 있는 경우
        if(memoStatus->Visible == false)
            memoStatus->Visible = true;

        if(strPreErrString != gm_OHTStatus->strErrorString)
        {
            memoStatus->Clear();
            memoStatus->Color = COLOR_RED;
            memoStatus->Lines->Add(gm_OHTStatus->strOHTVer+"["+gm_OHTStatus->strOHTType+"]");
            memoStatus->Lines->Add(gm_OHTStatus->strErrorString);

            strPreErrString = gm_OHTStatus->strErrorString;
        }
    }
    else
    {
        // by zzang9un 2013.02.21 : 에러 코드가 없는 경우
		memoStatus->Visible = false;
		memoStatus->Clear();
		memoStatus->Color = COLOR_WHITE;
		panOHTVer->Visible = true;
		panOHTVer->Caption = gm_OHTStatus->strOHTVer+"["+gm_OHTStatus->strOHTType+"]";

		strPreErrString = NULL;
    }

    // by zzang9un 2013.03.19 : 각 함수 호출
    Analysis_AMC_IOData();

    AnalysisMotorInfo(&gm_OHTStatus->m_OHTStatus);

    Analysis_Use_Data(&gm_OHTStatus->m_OHTStatus.UseInfo);

    // by zzang9un 2013.03.26 : OHT Version 정보 표시
    ///@todo 향후 MainForm으로 위치 이동 필요
    String OHTProgramVersion;

    OHTProgramVersion.sprintf(L"OHT[Ver %u.%u.%u.%u]",
        gm_OHTStatus->m_OHTStatus.OHTVer.Info.Major,
        gm_OHTStatus->m_OHTStatus.OHTVer.Info.Minor,
        gm_OHTStatus->m_OHTStatus.OHTVer.Info.Release,
        gm_OHTStatus->m_OHTStatus.OHTVer.Info.Build);
    MainForm->panOHTVer->Caption = OHTProgramVersion;

    // by zzang9un 2013.10.07 : AMC Version 정보 표시
    String AMCVersion;
    int nAMCVersion = gm_OHTStatus->m_OHTStatus.AMCVer.Data;

    int nAMCVerMajor = nAMCVersion / 10000;
    int nAMCVerMinor = (nAMCVersion - (nAMCVerMajor * 10000)) / 100;
    int nAMCVerRelease = (nAMCVersion - (nAMCVerMajor * 10000)) % 100;

    AMCVersion.sprintf(L"AMC[Ver %d.%d.%02d]", nAMCVerMajor, nAMCVerMinor, nAMCVerRelease);
	MainForm->panAMCVer->Caption = AMCVersion;

	// 24. 04. 05. oht type 정보 표시
	MainForm->panOHTType->Caption = gm_OHTStatus->strOHTVer+"["+gm_OHTStatus->strOHTType+"]";

	String MapVersion;

	if(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust == ON)
    	MapVersion.sprintf(L"Map[Ver %d.%d.AdJust]", gm_OHTStatus->m_OHTStatus.MapVer.Info.Major, gm_OHTStatus->m_OHTStatus.MapVer.Info.Minor);
	else
		MapVersion.sprintf(L"Map[Ver %d.%d]", gm_OHTStatus->m_OHTStatus.MapVer.Info.Major, gm_OHTStatus->m_OHTStatus.MapVer.Info.Minor);

	MainForm->panMap->Caption = MapVersion;
	MainForm->SetMapVersion(MapVersion);

}
//---------------------------------------------------------------------------

void __fastcall TStatusForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    tmDisplayUpdate->Enabled = false;
}
//---------------------------------------------------------------------------

