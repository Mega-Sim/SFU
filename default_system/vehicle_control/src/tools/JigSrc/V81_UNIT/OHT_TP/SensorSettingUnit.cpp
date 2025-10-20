//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SensorSettingUnit.h"
#include "OHTInfoUnit.h"
#include "Def_TP.h"
#include "CMDControlUnit.h"
#include "Def_DataType.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "perfgrap"
#pragma link "Chart"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma link "Series"
#pragma resource "*.dfm"
TOBS_UBG_Form *OBS_UBG_Form;
//---------------------------------------------------------------------------
__fastcall TOBS_UBG_Form::TOBS_UBG_Form(TComponent* Owner)
    : TForm(Owner)
{
    nSensorType = DISTANCE_SENSOR_DISABLE;

    strSensorData->Cells[0][0] = "Step";
    strSensorData->Cells[1][0] = "거리[mm]";

    for(int i=0; i<Sensor_Data_Num; i++)
    {
        strSensorData->Cells[0][i+1] = i+1;
    }

	Btn_Check_Start->Enabled = true;
	Btn_Check_Stop->Enabled = false;
    btnSave->Enabled = false;
    Series1->Clear();

    Sensor_Debug->Lines->Clear();
    edtInform->Text = " ";
    SensorDataTimer->Interval = 250;
}

void __fastcall TOBS_UBG_Form::SensorData_ComboBoxChange(TObject *Sender)
{
    if(SensorData_ComboBox->ItemIndex == 0)
    {
        nSensorType = UBG_OBSTACLE_SENSOR;
        Chart1->Title->Text->Text = "장애물 센서 데이터 값 (Y축 데이터)";
        edtInform->Text = "※센서 인식 기준 값 : Left/Right 24 이상!!";
    }
    else if(SensorData_ComboBox->ItemIndex == 1)
    {
        nSensorType = UBG_OHTDETECT_SENSOR;
        Chart1->Title->Text->Text = "대차 센서 데이터 값 (Y축 데이터)";
        edtInform->Text = "※센서 인식 기준 값 : Left/Right 54 이상!!";
    }
    else
    {
        nSensorType = DISTANCE_SENSOR_DISABLE;
        Chart1->Title->Text->Text = "센서 데이터 값";
        edtInform->Text = " ";
    }
}
//---------------------------------------------------------------------------
void __fastcall TOBS_UBG_Form::SensorDataTimerTimer(TObject *Sender)
{
    int nDistance[Sensor_Data_Num] = {0,};
    Series1->Clear();

   	Chart1->LeftAxis->Minimum = 0;
	Chart1->LeftAxis->Maximum = 6000;

    for(int i=0; i<Sensor_Data_Num ; i++)
    {
       if(nSensorType == UBG_OBSTACLE_SENSOR)
       {
           nDistance[i] = Cal_Dis(i, gm_OHTStatus->m_OHTStatus.DistanceSensorData.nOBS[i], 1);
       }
       else if(nSensorType == UBG_OHTDETECT_SENSOR)
       {
           nDistance[i] = Cal_Dis(i, gm_OHTStatus->m_OHTStatus.DistanceSensorData.nUBG[i], 1);
       }
       else
       {
           nDistance[i] = 0;
       }
       strSensorData->Cells[1][i+1] = nDistance[i];
       Series1->AddXY(i, nDistance[i], i+1, clRed);
    }
    Check_CenterPos(nDistance);
}
//---------------------------------------------------------------------------

void __fastcall TOBS_UBG_Form::Btn_Check_StartClick(TObject *Sender)
{
    if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL)
	{
		MainForm->Warnning_Display(clRed, "OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
        Sensor_Debug->Lines->Add("OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		return;
	}

    if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE))
    {
        MainForm->Warnning_Display(clRed, "OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
        Sensor_Debug->Lines->Add("OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		return;
    }

    TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

    m_CMDControl->MakeTPCmd2OHT(TPCMD_OBSTACLE_AREA2);
    Sensor_Debug->Lines->Add("Set UBG OBS Detect Sensor Area : 2");

    m_CMDControl->MakeTPCmd2OHT(TPCMD_OHTDETECT_AREA1);
    Sensor_Debug->Lines->Add("Set UBG OHT Detect Sensor Area : 1");

    if(nSensorType == UBG_OBSTACLE_SENSOR)
    {

        m_CMDControl->MakeTPCmd2OHT(TPCMD_OBS_SENSOR_REQUEST_START);
        Sensor_Debug->Lines->Add("Request UBG OBS Detect Sensor Data");
    }
    else if(nSensorType == UBG_OHTDETECT_SENSOR)
    {
        m_CMDControl->MakeTPCmd2OHT(TPCMD_UBG_SENSOR_REQUEST_START);
        Sensor_Debug->Lines->Add("Request UBG OHT Detect Sensor Data");
    }
    delete m_CMDControl;

	m_nResult = 0;
    SensorDataTimer->Enabled = true;
    SensorData_ComboBox->Enabled = false;
	Btn_Check_Start->Enabled = false;
	Btn_Check_Stop->Enabled = true;
	btnSave->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TOBS_UBG_Form::Btn_Check_StopClick(TObject *Sender)
{
    if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL)
	{
		MainForm->Warnning_Display(clRed, "OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
        Sensor_Debug->Lines->Add("OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		return;
	}
    if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE))
    {
        MainForm->Warnning_Display(clRed, "OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
        Sensor_Debug->Lines->Add("OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		return;
    }

    SensorDataTimer->Enabled = false;
    SensorData_ComboBox->Enabled = true;
    Btn_Check_Start->Enabled = true;
	Btn_Check_Stop->Enabled = false;
	btnSave->Enabled = false;

    TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_DISTANCE_SENSOR_REQUEST_STOP);

    if(nSensorType == UBG_OBSTACLE_SENSOR)
    {
        Sensor_Debug->Lines->Add("Stop UBG OBS Detect Sensor Data");
    }
    else if(nSensorType == UBG_OHTDETECT_SENSOR)
    {
        Sensor_Debug->Lines->Add("Stop UBG OHT Detect Sensor Data");
    }
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

bool TOBS_UBG_Form::Check_CenterPos(int *data)
{
    bool left_success = false;
    bool right_success = false;
    int left_cnt = 0;
    int right_cnt = 0;
    int start_x_pos = 0;
    int end_x_pos = 0;
    int left_diff = 0;
    int right_diff = 0;
    int center_pos = 0;
    AnsiString str = "";


    static int nPreResult = 0;

    if(nSensorType == UBG_OBSTACLE_SENSOR)
    {
        if(data[Sensor_Data_Center-1] >= MaxLimit)
        {
            for(int i = Sensor_Data_Center-2; i >= 0; i--)
            {
                if(data[i] <= MaxLimit && data[i] >= MinLimit)
                {
                    left_cnt++;
                    if(left_cnt == 1)
                    {
                        start_x_pos = i;
                    }
                    else if(left_cnt == OBS_Sensor_Offset)
                    {
                        left_success = true;
                        break;
                    }
                }
                else
                {
                    left_cnt = 0;
                }
            }

            for(int j = Sensor_Data_Center; j < Sensor_Data_Num; j++)
            {
                if(data[j] <= MaxLimit && data[j] >= MinLimit)
                {
                    right_cnt++;
                    if(right_cnt == 1)
                    {
                        end_x_pos = j;
                    }
                    else if(right_cnt == OBS_Sensor_Offset)
                    {
                        right_success = true;
                        break;
                    }
                }
                else
                {
                    left_cnt = 0;
                }
            }

            edt_start_pos->Text = str.sprintf("%d", start_x_pos);
            edt_end_pos->Text = str.sprintf("%d", end_x_pos);

            if(left_success && right_success)
            {
                left_diff = (Sensor_Data_Center-1) - start_x_pos;
                right_diff = end_x_pos - (Sensor_Data_Center-1);
                center_pos = left_diff - right_diff;
                //Sensor_Debug->Lines->Add(str.sprintf("[right] %d - [left] %d = [center] %d", right_diff, left_diff, center_pos));

                edt_SensorLeft->Text = str.sprintf("%d", left_diff);
                edt_SensorRight->Text = str.sprintf("%d", right_diff);
                edt_SensorCenter->Text = str.sprintf("%d", center_pos);

                if(center_pos >= OBS_Sensor_Margin_Neg && center_pos <= OBS_Sensor_Margin_Pos)
                {
                    if(left_diff >= SensorUbgObsMin &&  right_diff >= SensorUbgObsMin)
                    {
						m_nResult = SENSOR_INSTALL_RESULT_OK;
                    }
                    else
                    {
						m_nResult = SENSOR_INSTALL_RESULT_NG_SENSOR_LEVEL_LOW;
                    }
                }
                else
                {
                    if(center_pos > 0)
                    {
						m_nResult = SENSOR_INSTALL_RESULT_NG_MOVE_LEFT;
                    }
                    else
                    {
						 m_nResult = SENSOR_INSTALL_RESULT_NG_MOVE_RIGHT;
                    }
                }
            }
            else
            {
				m_nResult = SENSOR_INSTALL_RESULT_NG_REFLECTOR_BAD;
            }
        }
        else
        {
			m_nResult = SENSOR_INSTALL_RESULT_NG_OBS_VERTICAL_LEVEL;
        }
    }
    else if(nSensorType == UBG_OHTDETECT_SENSOR)
    {
        if(data[Sensor_Data_Center-1] <= MaxLimit && data[Sensor_Data_Center-1] >= MinLimit)
        {
            for(int i = Sensor_Data_Center-2; i >= 0; i--)
            {
                if(data[i] >= MaxLimit)
                {
                    left_cnt++;
                    if(left_cnt == 1)
                    {
                        start_x_pos = i;
                    }
                    else if(left_cnt == OHT_Sensor_Offset)
					{
                        left_success = true;
                        break;
                    }
                }
                else
                {
                    left_cnt = 0;
                }
            }

            for(int j = Sensor_Data_Center; j < Sensor_Data_Num; j++)
            {
                if(data[j] >= MaxLimit)
                {
                    right_cnt++;
                    if(right_cnt == 1)
                    {
                        end_x_pos = j;
                    }
                    else if(right_cnt == OHT_Sensor_Offset)
                    {
                        right_success = true;
                        break;
                    }
                }
                else
                {
                    left_cnt = 0;
                }
            }

            edt_start_pos->Text = str.sprintf("%d", start_x_pos);
            edt_end_pos->Text = str.sprintf("%d", end_x_pos);

            if(left_success && right_success)
            {
                left_diff = (Sensor_Data_Center-1) - start_x_pos;
                right_diff = end_x_pos - (Sensor_Data_Center-1);
                center_pos = left_diff - right_diff;
                //Sensor_Debug->Lines->Add(str.sprintf("[right] %d - [left] %d = [center] %d", right_diff, left_diff, center_pos));

                edt_SensorLeft->Text = str.sprintf("%d", left_diff);
                edt_SensorRight->Text = str.sprintf("%d", right_diff);
                edt_SensorCenter->Text = str.sprintf("%d", center_pos);

                if(center_pos >= OHT_Sensor_Margin_Neg && center_pos <= OHT_Sensor_Margin_Pos)
                {
                    if(left_diff >= SensorUbgOhtMin &&  right_diff >= SensorUbgOhtMin)
                    {
						m_nResult = SENSOR_INSTALL_RESULT_OK;
                    }
                    else
                    {
						m_nResult = SENSOR_INSTALL_RESULT_NG_SENSOR_LEVEL_LOW;
                    }
                }
                else
                {
                    if(center_pos > 0)
                    {
						m_nResult = SENSOR_INSTALL_RESULT_NG_MOVE_LEFT;
                    }
                    else
                    {
						 m_nResult = SENSOR_INSTALL_RESULT_NG_MOVE_RIGHT;
                    }
                }
            }
            else
			{
				m_nResult = SENSOR_INSTALL_RESULT_NG_REFLECTOR_BAD;
            }
        }
        else
		{
			if(data[Sensor_Data_Center-1] < MinLimit)
			{
				m_nResult = SENSOR_INSTALL_RESULT_NG_OHT_POSITION;
			}
			else
			{
				m_nResult = SENSOR_INSTALL_RESULT_NG_OHT_VERTICAL_LEVEL;
			}
        }
	}

	if(nPreResult != m_nResult)
    {
        switch(m_nResult)
        {
            case SENSOR_INSTALL_RESULT_OK:
                edt_SensorSettingResult->Color = clGreen;
                edt_SensorSettingResult->Text = "GOOD";
                Sensor_Debug->Lines->Add("[OK] 센서 설정 정상");
                break;

            case SENSOR_INSTALL_RESULT_NG_SENSOR_LEVEL_LOW:
                edt_SensorSettingResult->Color = clRed;
                edt_SensorSettingResult->Text = "BAD";
                Sensor_Debug->Lines->Add("[NG] 좌/우 센서 인식 값 미달, 센서 상/하 각도 및 OHT 위치 확인 필요");
                break;

            case SENSOR_INSTALL_RESULT_NG_MOVE_LEFT:
                edt_SensorSettingResult->Color = clRed;
                edt_SensorSettingResult->Text = "BAD";
                Sensor_Debug->Lines->Add("[NG] 센서를 왼쪽 방향으로 이동해주세요.");
                break;

            case SENSOR_INSTALL_RESULT_NG_MOVE_RIGHT:
                edt_SensorSettingResult->Color = clRed;
                edt_SensorSettingResult->Text = "BAD";
                Sensor_Debug->Lines->Add("[NG] 센서를 오른쪽 방향으로 이동해 주세요.");
                break;

            case SENSOR_INSTALL_RESULT_NG_REFLECTOR_BAD:
                edt_SensorSettingResult->Color = clRed;
                edt_SensorSettingResult->Text = "Debug Log 확인 필요";
                Sensor_Debug->Lines->Add("[NG] Test Jig 반사판 상태 점검 필요");
                break;

            case SENSOR_INSTALL_RESULT_NG_OBS_VERTICAL_LEVEL:
                edt_SensorSettingResult->Color = clRed;
				edt_SensorSettingResult->Text = "Debug Log 확인 필요";
                Sensor_Debug->Lines->Add("[NG] 장애물 감지 센서 광이 중심부를 가리키고 있지 않음, 센서 상하 조정 필요");
                break;

			case SENSOR_INSTALL_RESULT_NG_OHT_POSITION:
				edt_SensorSettingResult->Color = clRed;
				edt_SensorSettingResult->Text = "Debug Log 확인 필요";
				Sensor_Debug->Lines->Add("[NG] OHT 감지 센서와 반사판 JIG의 거리가 가까움 (Spec : 718 ~ 918mm)");
				break;

			case SENSOR_INSTALL_RESULT_NG_OHT_VERTICAL_LEVEL:
				edt_SensorSettingResult->Color = clRed;
				edt_SensorSettingResult->Text = "Debug Log 확인 필요";
				Sensor_Debug->Lines->Add("[NG] OHT 감지 센서 광이 중심부를 가리키고 있지 않음, 센서 상하 조정 필요");
				break;
        }
    }
    nPreResult = m_nResult;
}
//---------------------------------------------------------------------------
int TOBS_UBG_Form::Cal_Dis(int cnt, int dis, char type)
{
    int nDistance = 0;
    double dSensorDegree = 0.0;

    if(type == 0)
    {
        if(cnt <= Sensor_Data_Center)
        {
            dSensorDegree = (double)(SensorUnitDegree * cnt);
            dSensorDegree = (double)((dSensorDegree * MATH_PI)/180.0);
            nDistance = (double)dis * Cos(dSensorDegree);
        }
        else
        {
            dSensorDegree = 180.0 - (double)(SensorUnitDegree * cnt);
            dSensorDegree = (double)((dSensorDegree * MATH_PI)/180.0);
            nDistance = (double)dis * Cos(dSensorDegree);
        }
    }
    else
    {
        if(cnt <= Sensor_Data_Center)
        {
            dSensorDegree = (double)(SensorUnitDegree * cnt);
            dSensorDegree = (double)((dSensorDegree * MATH_PI)/180.0);
            nDistance = (double)dis * Sin(dSensorDegree);
        }
        else
        {
            dSensorDegree = 180.0 - (double)(SensorUnitDegree * cnt);
            dSensorDegree = (double)((dSensorDegree * MATH_PI)/180.0);
            nDistance = (double)dis * Sin(dSensorDegree);
        }
    }

    return nDistance;
}
//---------------------------------------------------------------------------
void __fastcall TOBS_UBG_Form::FormDestroy(TObject *Sender)
{
    SensorDataTimer->Enabled = false;
    SensorData_ComboBox->Enabled = true;
    Btn_Check_Start->Enabled = true;
	Btn_Check_Stop->Enabled = false;
	btnSave->Enabled = false;

    TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_DISTANCE_SENSOR_REQUEST_STOP);
    Sensor_Debug->Lines->Add("Stop UBG OHT Detect Sensor Data");
    delete m_CMDControl;
}
//---------------------------------------------------------------------------


void __fastcall TOBS_UBG_Form::btnSaveClick(TObject *Sender)
{

	if(SensorDataTimer->Enabled == true)
	{
       	int iPacketSize = sizeof(PKT_OHT2TP_OACIS_INFO);
		int iReturn;

		PKT_OHT2TP_OACIS_INFO 	SEND_PACKET;
		memset(&SEND_PACKET, 0, iPacketSize);	// 패킷 메모리 클리어

		SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
		memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);
		SEND_PACKET.TAG.Value = TagID::CMD_OACIS_INFO_SAVE;


		if(SensorData_ComboBox->ItemIndex == 0)
			SEND_PACKET.BODY.Type = CATEGORY_OBSDECT;
		else if(SensorData_ComboBox->ItemIndex == 1)
			SEND_PACKET.BODY.Type = CATEGORY_VHLDECT;

		SEND_PACKET.BODY.TypeNum = 1;
		SEND_PACKET.BODY.nData1 = m_nResult;
		SEND_PACKET.BODY.nData2 = StrToInt(edt_SensorCenter->Text);
		SEND_PACKET.BODY.nData3 = StrToInt(edt_SensorLeft->Text);
		SEND_PACKET.BODY.nData4 = StrToInt(edt_SensorRight->Text);


		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
		delete m_CMDControl;
	}

}
//---------------------------------------------------------------------------


