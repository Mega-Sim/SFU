// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "IOMapUnit.h"
#include "OHTInfoUnit.h"
#include "Define.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "ChkExecUnit.h"
#include "Def_TP.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TIOMapForm *IOMapForm;
TCHKOHT UseOHTCheck;


// ---------------------------------------------------------------------------
__fastcall TIOMapForm::TIOMapForm(TComponent *Owner)
    :
    TForm(Owner)
{
    pgcIOMap->ActivePageIndex = 0;

	strgridUseOnOff->ColCount = CHECKONOFF_COL;
	strgridUseOnOff->RowCount = CHECKONOFF_ROW;

	strGridMotor->ColCount = MOTORGRID_COL;
	strGridMotor->RowCount = MOTORGRID_ROW;
	
	iInputNumCount = 0;
	iOutputNumCount = 0;
}
// ---------------------------------------------------------------------------

/**
 @brief   IO맵 초기화 하는 함수

 ~~ Ini 파일(IOMap.ini)에서 OHT 버전에 맞는 IO Map을 불러온다. ~~

 OHT 에서 받은 IO Map 데이터를 넣는다.

 @author  zzang9un
 @date    2013.02.18
 */
void TIOMapForm::InitIOMap(IO_MAP_FIELD* m_pIOMapField)
{
	if(m_pIOMapField->InputNum <=0 || m_pIOMapField->OutputNum <= 0)
	{
		//I/O갯수를 잘못 받아옴
	}
	else
	{
		strGridInput->RowCount = m_pIOMapField->InputNum;
		strGridOutput->RowCount = m_pIOMapField->OutputNum;

		for(int i = 0; i < m_pIOMapField->InputNum; i++)
		{
			strGridInput->Cells[0][i] = IntToStr(i+1);//번호
			strGridInput->Cells[COL_INPUTNAME][i] = m_pIOMapField->InputField[i];//필드명
		}

		if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_MAC)
		{
			strGridInput->Cells[COL_INPUTNAME][45] = "MAC_Detect";    //V8.2 MAC인 경우 IO Name 변경
		}

		for(int i = 0; i < m_pIOMapField->OutputNum; i++)
		{
			strGridOutput->Cells[0][i] = IntToStr(i+1);//번호
			strGridOutput->Cells[COL_OUTPUTNAME][i] = m_pIOMapField->OutputField[i];//필드명
		}
	}


	// -------------------------------------------
	// [BEGIN] by zzang9un 2013.08.18 : Use Grid 설정
    int Col = 1;
    strgridUseOnOff->Cells[Col][USE_SET_BCR] = "Use BCR";
    strgridUseOnOff->Cells[Col][USE_SET_PBSSENSOR] = "Use PBS Sensor";
    strgridUseOnOff->Cells[Col][USE_SET_SOUNDALARM] = "Use SoundAlarm";
    strgridUseOnOff->Cells[Col][USE_SET_LAMPINDI] = "Use LampIndi";
    strgridUseOnOff->Cells[Col][USE_SET_SHUTTERCONTROL] = "Use Shutter Control";
    strgridUseOnOff->Cells[Col][USE_SET_EQPIO] = "Use EQPIO";
    strgridUseOnOff->Cells[Col][USE_SET_STBSEN] = "Use STB Sensor";
    strgridUseOnOff->Cells[Col][USE_SET_HANDLIMITSENSOR] = "Use HandLimitSensor";
    strgridUseOnOff->Cells[Col][USE_SET_HANDCONTROL] = "Use HandControl";
    strgridUseOnOff->Cells[Col][USE_SET_STEERINGCONTROL] = "Use SteeringControl";
    strgridUseOnOff->Cells[Col][USE_SET_UTBSEN] = "Use UTB Sensor";
    strgridUseOnOff->Cells[Col][USE_SET_LOOKDOWNSENSOR] = "Use LooKDownSensor";
    strgridUseOnOff->Cells[Col][USE_SET_HANDPUSHSENCHK] = "Use HandPushSensor";
    strgridUseOnOff->Cells[Col][USE_SET_GRIPPERSEN] = "Use GripperSensor";
    strgridUseOnOff->Cells[Col][USE_SET_HOISTHOMESENSOR] = "Use HoistAlignSensor";
    strgridUseOnOff->Cells[Col][USE_SET_OSCILLATIONSEN] = "Use Oscillation Sen";
    strgridUseOnOff->Cells[Col][USE_SET_ERRSOUND] = "Use ErrorSound";
    strgridUseOnOff->Cells[Col][USE_SET_DRIVINGSOUND] = "Use DrivingSound";
    strgridUseOnOff->Cells[Col][USE_SET_TRANSSOUND] = "Use TransSound";
    strgridUseOnOff->Cells[Col][USE_SET_OBSDETECTSOUND] = "Use ObsDetectSound";
	strgridUseOnOff->Cells[Col][USE_SET_OHTDETECTSOUND] = "Use OhtDetectSound";
    strgridUseOnOff->Cells[Col][USE_SET_PIOIGNOREERROR] = "Use Ignore PIO Error";
    strgridUseOnOff->Cells[Col][USE_SET_HOISTHOME_PIOERR] = "Use HoistHomePos_PIOErr";
    strgridUseOnOff->Cells[Col][USE_SET_OBSIGNORESOUND] = "Use ObsIgnoreWarnSound";
    strgridUseOnOff->Cells[Col][USE_SET_LOCSAVE_TRANS_ING] = "Use LocSave_Trans_ing";
    strgridUseOnOff->Cells[Col][USE_SET_PUSHABNORMALONCHK] = "Use Push Abnormal On Chk";
    strgridUseOnOff->Cells[Col][USE_SET_UTBFOUPCHECK] = "Use UTB Foup Check";
    strgridUseOnOff->Cells[Col][USE_SET_INNERFOUPDETECTSEN] = "Use Inner Foup Check";
	strgridUseOnOff->Cells[Col][USE_SET_TRANSEQSOUND] = "Use Trans_EQ_Sound";
	strgridUseOnOff->Cells[Col][USE_SET_TRANSSTKSOUND] = "Use Trans_STK_Sound";
	strgridUseOnOff->Cells[Col][USE_SET_TRANSSTBSOUND] = "Use Trans_STB_Sound";
    // [END] by zzang9un 2013.08.18 : Use Grid 설정
    // -------------------------------------------

    // Motor Information
    Col = 1;
    int Row = 0;
    strGridMotor->Cells[Col][Row++] = "Driving Front";
    strGridMotor->Cells[Col][Row++] = " - Pos[cts]";
    strGridMotor->Cells[Col][Row++] = " - Vel[cts/msec]";
    strGridMotor->Cells[Col][Row++] = " - Vel[m/sec]";
    strGridMotor->Cells[Col][Row++] = " - Vel[rpm]";
    strGridMotor->Cells[Col][Row++] = " - Error";
    strGridMotor->Cells[Col][Row++] = " - ErrorCode";
    strGridMotor->Cells[Col][Row++] = " - Axis Running";

    strGridMotor->Cells[Col][Row++] = "Driving Rear";
    strGridMotor->Cells[Col][Row++] = " - Pos[cts]";
    strGridMotor->Cells[Col][Row++] = " - Vel[cts/msec]";
    strGridMotor->Cells[Col][Row++] = " - Vel[m/sec]";
    strGridMotor->Cells[Col][Row++] = " - Vel[rpm]";
    strGridMotor->Cells[Col][Row++] = " - Error";
    strGridMotor->Cells[Col][Row++] = " - ErrorCode";
    strGridMotor->Cells[Col][Row++] = " - Axis Running";

    strGridMotor->Cells[Col][Row++] = "Hoist";
    strGridMotor->Cells[Col][Row++] = " - Pos[cts]";
    strGridMotor->Cells[Col][Row++] = " - Vel[cts/msec]";
    strGridMotor->Cells[Col][Row++] = " - Vel[m/sec]";
    strGridMotor->Cells[Col][Row++] = " - Vel[rpm]";
    strGridMotor->Cells[Col][Row++] = " - Error";
    strGridMotor->Cells[Col][Row++] = " - ErrorCode";
    strGridMotor->Cells[Col][Row++] = " - Axis Running";

    strGridMotor->Cells[Col][Row++] = "Shift";
    strGridMotor->Cells[Col][Row++] = " - Pos[cts]";
    strGridMotor->Cells[Col][Row++] = " - Vel[cts/msec]";
    strGridMotor->Cells[Col][Row++] = " - Vel[m/sec]";
    strGridMotor->Cells[Col][Row++] = " - Vel[rpm]";
    strGridMotor->Cells[Col][Row++] = " - Error";
    strGridMotor->Cells[Col][Row++] = " - ErrorCode";
    strGridMotor->Cells[Col][Row++] = " - Axis Running";

    strGridMotor->Cells[Col][Row++] = "Rotate";
    strGridMotor->Cells[Col][Row++] = " - Pos[cts]";
    strGridMotor->Cells[Col][Row++] = " - Vel[cts/msec]";
    strGridMotor->Cells[Col][Row++] = " - Vel[m/sec]";
    strGridMotor->Cells[Col][Row++] = " - Vel[rpm]";
    strGridMotor->Cells[Col][Row++] = " - Error";
    strGridMotor->Cells[Col][Row++] = " - ErrorCode";
    strGridMotor->Cells[Col][Row++] = " - Axis Running";

    if(gm_OHTStatus->nProgram_Mode == MODE_SETUP || gm_OHTStatus->nProgram_Mode == MODE_WORKER)// 개발자 또는 운영자
        strGridOutput->Enabled = true;
    else
        strGridOutput->Enabled = false;

	// shkim add
	iInputNumCount 	= m_pIOMapField->InputNum;
	iOutputNumCount = m_pIOMapField->OutputNum;
}

// ---------------------------------------------------------------------------
/**
 @brief  IO 상태(On/off)를 Display 하기 위한 함수
 @author  puting
 @date    2012.12.12
 @param   IO Status Display
 */
void TIOMapForm::IO_Display()
{
	int k, j;
	int count = 0;
    if(IOMapForm->Visible == true)
	{
	
		count = 3 ;	// bit analisys -> 32bit * 3

	
		switch(pgcIOMap->ActivePageIndex)
		{
		case 0: // *** PMAC Input ***
			for(int i = 0; i < count; i++)
			{
                for(int j = 0; j < 32; j++)
				{
                    if(i == 0)
                    {
						strGridInput->Cells[2][j] = (gm_OHTStatus->IOInput->nInData[i][j] == ON) ? "ON" : "OFF";
						strGridInput->Cells[3][j] = gm_OHTStatus->IOInput->nInData[i][j];
					}
					else if(i == 1)
					{
						strGridInput->Cells[2][j + 32] = (gm_OHTStatus->IOInput->nInData[i][j] == ON) ? "ON" : "OFF";
						strGridInput->Cells[3][j + 32] = gm_OHTStatus->IOInput->nInData[i][j];
					}
					else
					{
						/*
							shkim. 이 부분은 이전에는 j < 16으로 아래와 같이 갯수가 고정되어 있었음 (80개)
							그러나 IO 갯수는 유동적이므로 64 ~ 96개에 적용 가능하도록 수정하였다.
						*/
						// if( j < 16)
						if (j < (iInputNumCount - 64))
						{
							strGridInput->Cells[2][j + 64] = (gm_OHTStatus->IOInput->nInData[i][j] == ON) ? "ON" : "OFF";
							strGridInput->Cells[3][j + 64] = gm_OHTStatus->IOInput->nInData[i][j];
						}
                    }
                }
            }
            break;

		case 1: // *** PMAC Output ***    ///puting
			for(int i = 0; i < count; i++)
			{
                for(int j = 0; j < 32; j++)
				{

                    // if(i==0) strGridOutput->Cells[2][j] = (gm_OHTStatus->IOOutput->nOutData[i][j]==ON) ? "ON" : "OFF";
                    // else strGridOutput->Cells[2][j+32] = (gm_OHTStatus->IOOutput->nOutData[i][j]==ON) ? "ON" : "OFF";
                    if(i == 0)
						strGridOutput->Cells[2][j] = gm_OHTStatus->IOOutput->nOutData[i][j];
					else if(i == 1)
						strGridOutput->Cells[2][j + 32] = gm_OHTStatus->IOOutput->nOutData[i][j];
					else
					{
						/*
							shkim. 이 부분은 이전에는 j < 16으로 아래와 같이 갯수가 고정되어 있었음 (80개)
							그러나 IO 갯수는 유동적이므로 64 ~ 96개에 적용 가능하도록 수정하였다.
						*/
						// if( j < 16)
						if (j < (iOutputNumCount - 64))
						{
							strGridOutput->Cells[2][j + 64] = gm_OHTStatus->IOOutput->nOutData[i][j];
						}
                    }
                }
            }
            break;

        case 2: // Check On/OFF
            k = 2;
			j = 0;

            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_BCR == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_PBSSensor == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_SoundAlarm == true) ? "ON" : "OFF";
			strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_LampIndi == true) ? "ON" : "OFF";

			strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_ShutterControl == true) ? "ON" : "OFF";
	
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_EQPIO == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_STBSensor == true) ? "ON" : "OFF";

            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_HandLimitSensorCheck == true) ?
                "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_HandControl == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_SteeringControl == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_UTBSensor == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_LookDownSensor == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_HandPushSensorCheck == true) ?
                "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_GripperSen == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_HoistAlignSen == true) ? "ON" : "OFF";

            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_OscillationSen == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_ErrSound == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_DrivingSound == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_TransSound == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_ObsDetectSound == true) ? "ON" : "OFF";
			strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_OhtDetectSound == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_IgnorePIOError == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_HoistHomePos_PIOError == true) ?
                "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_ObsIgnoreSound == true) ? "ON" : "OFF";

            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_LocSave_Trans_ing == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_PushAbnormalOnCheck == true) ?
                "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_UTBFoupCheck == true) ? "ON" : "OFF";
            strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_InnerFoupDetectSen == true) ?
                "ON" : "OFF";
			strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_TransEQSound == true) ? "ON" : "OFF";
			strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_TransSTKSound == true) ? "ON" : "OFF";
			strgridUseOnOff->Cells[k][j++] = (gm_OHTStatus->CheckOnOff->bUse_TransSTBSound == true) ? "ON" : "OFF";

            break;

        case 3: // Motor Status
            k = 2;
            j = 1;

            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Driving_F->dPosition_cts);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Driving_F->dVelocity_ctspermsec);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Driving_F->dVelocity_mpersec);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Driving_F->dVelocity_rpm);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Driving_F->nError);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Driving_F->nErrorCode);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Driving_F->nAxis_Running);

            j++;
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Driving_R->dPosition_cts);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Driving_R->dVelocity_ctspermsec);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Driving_R->dVelocity_mpersec);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Driving_R->dVelocity_rpm);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Driving_R->nError);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Driving_R->nErrorCode);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Driving_R->nAxis_Running);

            j++;
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Hoist->dPosition_cts);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Hoist->dVelocity_ctspermsec);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Hoist->dVelocity_mpersec);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Hoist->dVelocity_rpm);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Hoist->nError);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Hoist->nErrorCode);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Hoist->nAxis_Running);

            j++;
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Shift->dPosition_cts);
			strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Shift->dVelocity_ctspermsec);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Shift->dVelocity_mpersec);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Shift->dVelocity_rpm);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Shift->nError);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Shift->nErrorCode);
			strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Shift->nAxis_Running);

            j++;
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Rotate->dPosition_cts);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Rotate->dVelocity_ctspermsec);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Rotate->dVelocity_mpersec);
            strGridMotor->Cells[k][j++] = FloatToStr(gm_OHTStatus->Motor_Rotate->dVelocity_rpm);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Rotate->nError);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Rotate->nErrorCode);
            strGridMotor->Cells[k][j++] = IntToStr(gm_OHTStatus->Motor_Rotate->nAxis_Running);
        }
    }
}

// ---------------------------------------------------------------------------
void __fastcall TIOMapForm::strGridInputDrawCell(TObject *Sender,
    int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
    int i;
    String strText;
    if(State.Contains(gdFixed))
    {
        strGridInput->Canvas->Brush->Color = clBtnFace;
        strGridInput->Canvas->Font->Color  = clWindowText;
        strGridInput->Canvas->FillRect(Rect);
        Frame3D(strGridInput->Canvas, Rect, clBtnHighlight, clBtnShadow, 1);
    }
    else if(ACol == 2)
    {
        if(strGridInput->Cells[ACol][ARow] == "ON")
        {
            strGridInput->Canvas->Brush->Color = (TColor)0x00C080FF;
            strGridInput->Canvas->FillRect(Rect);
        }
        else
        {
            strGridInput->Canvas->Brush->Color = COLOR_WHITE;
            strGridInput->Canvas->FillRect(Rect);
        }
    }
    else
    {
        strGridInput->Canvas->Brush->Color = COLOR_WHITE;
        strGridInput->Canvas->FillRect(Rect);

    }
    strText = strGridInput->Cells[ACol][ARow];
    strGridInput->Canvas->Font->Color = COLOR_BLACK;
    strGridInput->Canvas->TextRect(Rect, Rect.Left, Rect.Top, strText);
}
// ---------------------------------------------------------------------------

void __fastcall TIOMapForm::strGridOutputDrawCell(TObject *Sender,
    int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
    int i;
    String strText;
    if(State.Contains(gdFixed))
    {
        strGridOutput->Canvas->Brush->Color = clBtnFace;
        strGridOutput->Canvas->Font->Color  = clWindowText;
        strGridOutput->Canvas->FillRect(Rect);
        Frame3D(strGridOutput->Canvas, Rect, clBtnHighlight, clBtnShadow, 1);
    }
    else if(ACol == 2)
    {
        if(strGridOutput->Cells[ACol][ARow] == "1")
        {
            strGridOutput->Canvas->Brush->Color = (TColor)0x00C080FF;
            strGridOutput->Canvas->FillRect(Rect);
        }
        else
        {
            strGridOutput->Canvas->Brush->Color = COLOR_WHITE;
            strGridOutput->Canvas->FillRect(Rect);
        }
    }
    else
    {
        strGridOutput->Canvas->Brush->Color = COLOR_WHITE;
        strGridOutput->Canvas->FillRect(Rect);

    }
    strText = strGridOutput->Cells[ACol][ARow];
    strGridOutput->Canvas->Font->Color = COLOR_BLACK;
    strGridOutput->Canvas->TextRect(Rect, Rect.Left, Rect.Top, strText);

}
// ---------------------------------------------------------------------------

void __fastcall TIOMapForm::strgridUseOnOffDrawCell(TObject *Sender,
    int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
    int i;
    String strText;
    if(State.Contains(gdFixed))
    {
        strgridUseOnOff->Canvas->Brush->Color = clBtnFace;
        strgridUseOnOff->Canvas->Font->Color  = clWindowText;
        strgridUseOnOff->Canvas->FillRect(Rect);
        Frame3D(strgridUseOnOff->Canvas, Rect, clBtnHighlight, clBtnShadow, 1);
    }
    else if(ACol == 2)
    {
        if(strgridUseOnOff->Cells[ACol][ARow] == "ON")
        {
            strgridUseOnOff->Canvas->Brush->Color = (TColor)0x00C080FF;
            strgridUseOnOff->Canvas->FillRect(Rect);
        }
        else
        {
            strgridUseOnOff->Canvas->Brush->Color = COLOR_WHITE;
            strgridUseOnOff->Canvas->FillRect(Rect);
        }
    }
    else
    {
        strgridUseOnOff->Canvas->Brush->Color = COLOR_WHITE;
        strgridUseOnOff->Canvas->FillRect(Rect);

    }
    strText = strgridUseOnOff->Cells[ACol][ARow];
    strgridUseOnOff->Canvas->Font->Color = COLOR_BLACK;
    strgridUseOnOff->Canvas->TextRect(Rect, Rect.Left, Rect.Top, strText);

}
// ---------------------------------------------------------------------------

void __fastcall TIOMapForm::popupCheckONOFFPopup(TObject *Sender)
{
    if(strgridUseOnOff->Cells[strgridUseOnOff->Col][strgridUseOnOff->Row] == "ON")
    {
        mniSetON->Visible  = false;
        mniSetOFF->Visible = true;
    }
    else
    {
        mniSetON->Visible  = true;
        mniSetOFF->Visible = false;
    }
}

// ---------------------------------------------------------------------------
/**
 @brief   OHT Use Data Grid의 Popup창을 열기 위한 함수
 @author  puting
 @date    2012.12.14
 @param   OHT Use Data Popup
 */
void __fastcall TIOMapForm::strgridUseOnOffClick(TObject *Sender)
{
    if(strgridUseOnOff->Col != 2)
        popupCheckONOFF->AutoPopup = false;
    else
        popupCheckONOFF->AutoPopup = true;
}
// ---------------------------------------------------------------------------

/**
@brief   OHT USE DATA ON를 하기 위한 함수
@author  puting
@date    2012.12.13
@param   OHT USE DATA ON
*/
void __fastcall TIOMapForm::mniSetONClick(TObject *Sender)
{
    if(UseOHTCheck.Is_Executable() == NO_ERR)
        SendUseONOFF(strgridUseOnOff->Row, ON);
}

// ---------------------------------------------------------------------------

/**
@brief   OHT USE DATA OFF를 하기 위한 함수
@author  puting
@date    2012.12.14
@param   OHT USE DATA OFF
*/
void __fastcall TIOMapForm::mniSetOFFClick(TObject *Sender)
{
    if(UseOHTCheck.Is_Executable() == NO_ERR)
        SendUseONOFF(strgridUseOnOff->Row, OFF);
}
// ---------------------------------------------------------------------------

/**
@brief   OHT USE DATA Control을 하기 위한 함수
@author  zzang9un
@date    2013.03.18
@param   Index : 변경할 Grid의 Index
@param   OnOff : On/Off Flag
*/
void TIOMapForm::SendUseONOFF(int Index, int OnOff)
{
/*
	2016.06.16 shkim original code locked...

    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    TPCmd.TPCmd.Data1 = Index;

    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

    if(OnOff == true)
        m_CMDControl->MakeTPCmd2OHT(TPCMD_USE_ON, &TPCmd);
    else
        m_CMDControl->MakeTPCmd2OHT(TPCMD_USE_OFF, &TPCmd);
*/

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_IO_SET_Cmd2OHT(IO_TYPE::ON_OFF_TYPE, Index, OnOff);
	delete m_CMDControl;


    
    /* // [BEGIN] by zzang9un 2013.08.18 : 기존 소스 주석 처리
    switch(Index)
    {
    case 1:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_BCR, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_BCR, (String)OnOff);
        break;

    case 2:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_PBSSENSOR, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_PBSSENSOR, (String)OnOff);
        break;
        
    case 3:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_SOUNDALARM, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_SOUNDALARM, (String)OnOff);
        break;
        
    case 4:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_LAMPINDI, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_LAMPINDI, (String)OnOff);
        break;
        
    case 5:
        if(gm_OHTStatus->nProgram_Mode == MODE_SETUP)// 개발자
        {
            m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_SHUTTERCONTROL, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_SHUTTERCONTROL, (String)OnOff);
        }
        break;
        
    case 6:
        // m_CMDControl->MakeCommand2OHT(DIRECT_LOG_SAVE, &TPCmd);
        break;
        
    case 7:
        if(gm_OHTStatus->nProgram_Mode == MODE_SETUP)// 개발자
        {
            m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_EQPIO, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_EQPIO, (String)OnOff);
        }
        break;
        
    case 8:
        if(gm_OHTStatus->nProgram_Mode == MODE_SETUP)// 개발자
        {
            m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_STBSEN, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_STBSEN, (String)OnOff);
        }
        break;
        
    case 9:
        if(gm_OHTStatus->nProgram_Mode == MODE_SETUP)// 개발자
        {
            m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_HANDLIMITSENSORCHK, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_HANDLIMITSENSORCHK, (String)OnOff);
        }
        break;
        
    case 10:
        if(gm_OHTStatus->nProgram_Mode == MODE_SETUP)// 개발자
        {
            m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_HANDCONTROL, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_HANDCONTROL, (String)OnOff);
        }
        break;
        
    case 11:
        if(gm_OHTStatus->nProgram_Mode == MODE_SETUP)// 개발자
        {
            m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_STEERINGCONTROL, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_STEERINGCONTROL, (String)OnOff);
        }
        break;
        
    case 12:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_UTBSEN, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_UTBSEN, (String)OnOff);
        break;
        
    case 13:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_LOOKDOWNSENSOR, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_LOOKDOWNSENSOR, (String)OnOff);
        break;
        
    case 14:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_HANDPUSHSENCHK, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_HANDPUSHSENCHK, (String)OnOff);
        break;
        
    case 15:
        if(gm_OHTStatus->nProgram_Mode == MODE_SETUP)// 개발자
        {
            m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_GRIPPERSEN, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_GRIPPERSEN, (String)OnOff);
        }
        break;
        
    case 16:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_HOISTALIGNSEN, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_HOISTALIGNSEN, (String)OnOff);
        break;
        
    case 17:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_OSCILLATIONSEN, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_OSCILLATIONSEN, (String)OnOff);
        break;
        
    case 18:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_ERRSOUND, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_ERRSOUND, (String)OnOff);
        break;
        
    case 19:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_DRIVINGSOUND, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_DRIVINGSOUND, (String)OnOff);
        break;
        
    case 20:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_TRANSSOUND, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USELRANSSOUND, (String)OnOff);
        break;
        
    case 21:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_OBSDETECTSOUND, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_OBSDETECTSOUND, (String)OnOff);
        break;
        
    case 22:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_PIOIGNOREERROR, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_PIOIGNOREERROR, (String)OnOff);
        break;
        
    case 23:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_HOISTHOME_PIOERR, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_HOISTHOME_PIOERR, (String)OnOff);
        break;
        
    case 24:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_OBSIGNORESOUND, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_OBSIGNORESOUND, (String)OnOff);
        break;
        
    case 25:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_LOCSAVE_TRANS_ING, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_LOCSAVELRANS_ING, (String)OnOff);
        break;
        
    case 26:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_PUSHABNORMALONCHK, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_PUSHABNORMALONCHK, (String)OnOff);
        break;
        
    case 27:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_CROSSRAILINTERLOCK, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(MAP_USE_CROSSRAIL, (String)OnOff);
        break;
        
    case 28:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_UTBFOUPCHECK, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_UTBFOUPCHECK, (String)OnOff);
        break;
        
    case 29:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_INNERFOUPDETECTSEN, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_MAP_USE_INNERFOUPDETECTSEN, (String)OnOff);
        break;
    }
    */ // [END] by zzang9un 2013.08.18 : 기존 소스 주석 처리

    delete m_CMDControl;
}

// ---------------------------------------------------------------------------
void __fastcall TIOMapForm::FormShow(TObject *Sender)
{
//    InitIOMap();
}
// ---------------------------------------------------------------------------

void __fastcall TIOMapForm::strGridMotorDrawCell(TObject *Sender, int ACol,
    int ARow, TRect &Rect, TGridDrawState State)
{
    int i;
    String strText;
    if(State.Contains(gdFixed))
    {
        strGridMotor->Canvas->Brush->Color = clBtnFace;
        strGridMotor->Canvas->Font->Color  = clWindowText;
        strGridMotor->Canvas->FillRect(Rect);
        Frame3D(strGridMotor->Canvas, Rect, clBtnHighlight, clBtnShadow, 1);
    }
    else if(ACol == 1)
    {
        if((ARow == 0) || (ARow == 8) || (ARow == 16) || (ARow == 24) || (ARow == 32))
        {
            strGridMotor->Canvas->Brush->Color = COLOR_YELLOW;
            strGridMotor->Canvas->FillRect(Rect);
        }
        else
        {
            strGridMotor->Canvas->Brush->Color = COLOR_WHITE;
            strGridMotor->Canvas->FillRect(Rect);
        }
    }
    else
    {
        strGridMotor->Canvas->Brush->Color = COLOR_WHITE;
        strGridMotor->Canvas->FillRect(Rect);

    }
    strText = strGridMotor->Cells[ACol][ARow];
    strGridMotor->Canvas->Font->Color = COLOR_BLACK;
    strGridMotor->Canvas->TextRect(Rect, Rect.Left, Rect.Top, strText);

}

// ---------------------------------------------------------------------------
/**
@brief   OHT OUTPUT Grid의 Popup창을 열기 위한 함수
@author  puting
@date    2012.12.14
@param   OHT OUTPUT Popup
*/
void __fastcall TIOMapForm::strGridOutputClick(TObject *Sender)
{
    if(strGridOutput->Col != 2)
        popupOutput->AutoPopup = false;
    else
        popupOutput->AutoPopup = true;
}

// ---------------------------------------------------------------------------
void __fastcall TIOMapForm::popupOutputPopup(TObject *Sender)
{
    String strData;
    if(strGridOutput->Cells[strGridOutput->Col][strGridOutput->Row] == "1")
    {
        mniSetTrue->Visible = false;
        strData.sprintf(L"Output%d => 0", strGridOutput->Row);
        mniSetFalse->Caption = strData;
        mniSetFalse->Visible = true;
    }
    else
    {
        strData.sprintf(L"Output%d => 1", strGridOutput->Row);
        mniSetTrue->Caption  = strData;
        mniSetTrue->Visible  = true;
        mniSetFalse->Visible = false;
    }

}

// ---------------------------------------------------------------------------
/**
@brief   OHT SET_OUTPUT ON를 하기 위한 함수
@author  puting
@date    2012.12.14
@param   Sender : TObject
*/
void __fastcall TIOMapForm::mniSetTrueClick(TObject *Sender)
{
    if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_MANUAL)
	{
		/*  2016.06.16 shkim original code locked...
		uTP_CMD TPCmd; // TP Command
        memset(&TPCmd, 0x00, sizeof(uTP_CMD));

		TPCmd.TPCmd.Data1 = strGridOutput->Row;
        TPCmd.TPCmd.Data2 = true;


        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SET_OUTPUT, &TPCmd);
		delete m_CMDControl;
		*/

		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		m_CMDControl->MakeTP_IO_SET_Cmd2OHT(IO_TYPE::ZERO_OR_ONE_TYPE, strGridOutput->Row, 1);
		delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
/**
@brief   OHT SET_OUTPUT OFF를 하기 위한 함수
@author  puting
@date    2012.12.14
@param   OHT SET_OUTPUT OFF
*/
void __fastcall TIOMapForm::mniSetFalseClick(TObject *Sender)
{
    if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_MANUAL)
    {
		// puting 2012.12.03 공통cmd 구조체 초기화

		/*  2016.06.16 shkim original code locked...
        uTP_CMD TPCmd; // TP Command
        memset(&TPCmd, 0x00, sizeof(uTP_CMD));

        TPCmd.TPCmd.Data1 = strGridOutput->Row;
        TPCmd.TPCmd.Data2 = false;

        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_RESET_OUTPUT, &TPCmd);
		delete m_CMDControl;
		*/

		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		m_CMDControl->MakeTP_IO_SET_Cmd2OHT(IO_TYPE::ZERO_OR_ONE_TYPE, strGridOutput->Row, 0);
		delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

