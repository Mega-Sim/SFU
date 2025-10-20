//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SensorSettingUnit.h"
#include "OHTInfoUnit.h"
#include "Def_TP.h"
#include "CMDControlUnit.h"
#include "Utility.h"
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

    for(int i=0; i<SOSLAB_Sensor_Data_Num; i++)
    {
        strSensorData->Cells[0][i+1] = i+1;
    }

	Btn_Check_Start->Enabled = true;
	Btn_Check_Stop->Enabled = false;
    btnSave->Enabled = false;
	Btn_repair_Stop->Enabled = false;
	Btn_repair_Start->Enabled = true;
	Btn_repair_Clear->Enabled = false;
    Series1->Clear();

    Sensor_Debug->Lines->Clear();
    edtInform->Text = " ";
	SensorDataTimer->Interval = 250;
	m_nResultCheck = 0;

	m_nResultCheckCount = 0;
}

void __fastcall TOBS_UBG_Form::SensorData_ComboBoxChange(TObject *Sender)
{
    if(SensorData_ComboBox->ItemIndex == 0)
    {
        nSensorType = UBG_OBSTACLE_SENSOR;
        Chart1->Title->Text->Text = "장애물 센서 데이터 값 (Y축 데이터)";
        edtInform->Text = "※센서 인식 기준 값 : Left/Right 24 이상!!";
		result_pnl->Visible = true;
		result_pnl_Fine->Visible = false;
    }
    else if(SensorData_ComboBox->ItemIndex == 1)
    {
        nSensorType = UBG_OHTDETECT_SENSOR;
        Chart1->Title->Text->Text = "대차 센서 데이터 값 (Y축 데이터)";
        edtInform->Text = "※센서 인식 기준 값 : Left/Right 54 이상!!";
		result_pnl->Visible = true;
		result_pnl_Fine->Visible = false;
	}
	else if(SensorData_ComboBox->ItemIndex == 2)
	{
		nSensorType = UBG_OHTDETECT_SOSLAB_SENSOR;     //SOSLAB 대차센서
		Chart1->Title->Text->Text = "SOSLAB 대차 센서 데이터 값 (Y축 데이터)";
        edtInform->Text = "※센서 인식 기준 값 : Left/Right 106 이상!!";
		result_pnl->Visible = true;
		result_pnl_Fine->Visible = false;
	}
	else if(SensorData_ComboBox->ItemIndex == 3)
	{
		nSensorType = UBG_OBSTACLE_SOSLAB_SENSOR;    //SOSLAB 장애물 센서
        Chart1->Title->Text->Text = "SOSLAB 장애물 센서 데이터 값 (Y축 데이터)";
        edtInform->Text = "※센서 인식 기준 값 : Left/Right 46 이상!!";
		result_pnl->Visible = true;
		result_pnl_Fine->Visible = false;
    }
	else if(SensorData_ComboBox->ItemIndex == 4)
	{
		nSensorType = PBS_LOOKDOWN_SENSOR;     //룩다운센서
		Chart1->Title->Text->Text = "룩다운 센서";
		edtInform->Text = "※센서 인식 기준 값 : 미정!!";
		result_pnl_Fine->Visible = true;
		result_pnl->Visible = false;
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
	int nSOSLABDistance[SOSLAB_Sensor_Data_Num] = {0,};
	int nLookDownDistance[LookDown_Data_Num] = {0,};

    Series1->Clear();

   	Chart1->LeftAxis->Minimum = 0;
	Chart1->LeftAxis->Maximum = 6000;



	if((nSensorType == UBG_OHTDETECT_SOSLAB_SENSOR) || (nSensorType == UBG_OBSTACLE_SOSLAB_SENSOR))
	{
		 //SOSLAB 로직
		 Chart1->BottomAxis->Maximum = SOSLAB_Sensor_Data_Num;
		for(int i=0; i<SOSLAB_Sensor_Data_Num ; i++)
		{
		   if(nSensorType == UBG_OHTDETECT_SOSLAB_SENSOR)
		   {
			   nSOSLABDistance[i] = Cal_Dis(i, gm_OHTStatus->m_OHTStatus.DistanceSensorData.nSOSLABUBG[i], 1);
		   }
		   else if(nSensorType == UBG_OBSTACLE_SOSLAB_SENSOR)
		   {
			   nSOSLABDistance[i] = Cal_Dis(i, gm_OHTStatus->m_OHTStatus.DistanceSensorData.nSOSLABOBS[i], 1);
		   }
		   else
		   {
			   nSOSLABDistance[i] = 0;
		   }

   		   strSensorData->Cells[1][i+1] = nSOSLABDistance[i];
		   Series1->AddXY(i, nSOSLABDistance[i], i+1, clRed);
		}
        Check_CenterPos_SOSLAB(nSOSLABDistance);

	}
	else if(nSensorType == PBS_LOOKDOWN_SENSOR)
	{
		//룩다운 로직

		//차트 크기 설정
		Chart1->LeftAxis->Maximum = 1000;  // Y축은 JIG 기준 거리값이 1000 미만 (보통 500)
		Chart1->BottomAxis->Minimum = 30;  // X축은 LookDown 패턴상 43~60 으로 설정됨
		Chart1->BottomAxis->Maximum =70;   // 추가마진 10으로 Min 30, Max 70으로 설정

		for(int i=0; i<LookDown_Data_Num ; i++)
		{
			nLookDownDistance[i] = gm_OHTStatus->m_OHTStatus.DistanceSensorData.nPBS[i];
			strSensorData->Cells[1][i+1] = nLookDownDistance[i];
			Series1->AddXY(i, nLookDownDistance[i], i+1, clRed);

			TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
			String strLog;
			strLog.sprintf(L"cnt : %d, dSensorDegree : %f, nDistance: %d",i, i*1.5, nLookDownDistance[i]);
			m_LogFile->WriteLogFile("TP_LookDown", strLog);
			delete m_LogFile;
		}

		Check_CenterPos_LookDown(nLookDownDistance);

	}
	else
	{
		//기존 로직
		Chart1->BottomAxis->Maximum = Sensor_Data_Num;
     	for(int i=0; i<Sensor_Data_Num ; i++)
		{
		   if(nSensorType == UBG_OBSTACLE_SENSOR)
		   {
			   nDistance[i] = Cal_Dis(i, gm_OHTStatus->m_OHTStatus.DistanceSensorData.nOBS[i], 0);
		   }
		   else if(nSensorType == UBG_OHTDETECT_SENSOR)
		   {
			   nDistance[i] = Cal_Dis(i, gm_OHTStatus->m_OHTStatus.DistanceSensorData.nUBG[i], 0);
		   }
		   else
		   {
			   nDistance[i] = 0;
		   }
		   strSensorData->Cells[1][i+1] = nDistance[i];
		   Series1->AddXY(i, nDistance[i], i+1, clRed);
		}

		if((nSensorType == UBG_OBSTACLE_SENSOR) || (nSensorType == UBG_OHTDETECT_SENSOR))
		Check_CenterPos(nDistance);
    }



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

	if((nSensorType == UBG_OBSTACLE_SENSOR) || (nSensorType == UBG_OHTDETECT_SENSOR) || (nSensorType == UBG_OHTDETECT_SOSLAB_SENSOR) || (nSensorType == UBG_OBSTACLE_SOSLAB_SENSOR)) // 센서 타입이 Hokuyo 장애물 대차일때만, Area 설정
	{
    m_CMDControl->MakeTPCmd2OHT(TPCMD_OBSTACLE_AREA2);
    Sensor_Debug->Lines->Add("Set UBG OBS Detect Sensor Area : 2");

    m_CMDControl->MakeTPCmd2OHT(TPCMD_OHTDETECT_AREA1);
    Sensor_Debug->Lines->Add("Set UBG OHT Detect Sensor Area : 1");
	}
	else if(nSensorType == UBG_OBSTACLE_SOSLAB_SENSOR)
    {
        m_CMDControl->MakeTPCmd2OHT(TPCMD_OHTDETECT_AREA1);
        Sensor_Debug->Lines->Add("Ste UBG OHT Detect Sensor Area : 1");

        m_CMDControl->MakeTPCmd2OHT(TPCMD_OHTDETECT_AREA15);
        Sensor_Debug->Lines->Add("Ste UBG OHT Detect Sensor Area : 15");
    }

	else if(nSensorType == PBS_LOOKDOWN_SENSOR) //룩다운 패턴설정
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_LOOKDOWN_AREA1);
		Sensor_Debug->Lines->Add("Set LookDown Sensor Area : 1");
	}

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
	//Lookdown 기능추가 timy
	else if(nSensorType == PBS_LOOKDOWN_SENSOR)
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_PBS_SENSOR_REQUEST_START);
		Sensor_Debug->Lines->Add("Request UBG OHT Detect Sensor Data");
	}
	else if(nSensorType == UBG_OHTDETECT_SOSLAB_SENSOR)
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_UBG_SOSLAB_SENSOR_REQUEST_START);
		Sensor_Debug->Lines->Add("Request SOSLAB UBG OHT Detect Sensor Data");
	}
	else if(nSensorType == UBG_OBSTACLE_SOSLAB_SENSOR)
	{
		m_CMDControl->MakeTPCmd2OHT(TPCMD_OBS_SOSLAB_SENSOR_REQUEST_START);
		Sensor_Debug->Lines->Add("Request SOSLAB OBS OHT Detect Sensor Data");
	}

    delete m_CMDControl;

	m_nResult = 0;
    SensorDataTimer->Enabled = true;
    SensorData_ComboBox->Enabled = false;
	Btn_Check_Start->Enabled = false;
	Btn_Check_Stop->Enabled = true;
	Btn_repair_Start->Enabled = false;
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
	Btn_repair_Start->Enabled = true;
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
	else if(nSensorType == UBG_OHTDETECT_SOSLAB_SENSOR)
	{
		Sensor_Debug->Lines->Add("Stop SOSLAB UBG OHT Detect Sensor Data");
	}
	else if(nSensorType == UBG_OBSTACLE_SOSLAB_SENSOR)
	{
		Sensor_Debug->Lines->Add("Stop SOSLAB OBS OHT Detect Sensor Data");
	}
	else if(nSensorType == PBS_LOOKDOWN_SENSOR)
	{
		Sensor_Debug->Lines->Add("Stop LookDown Sensor Data");
	}
    delete m_CMDControl;
}
//---------------------------------------------------------------------------

bool TOBS_UBG_Form::Check_CenterPos_SOSLAB(int *data)
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


    static int nPreResultSOSLAB = 0;

    if(nSensorType == UBG_OBSTACLE_SOSLAB_SENSOR)
	{
		if(data[SOSLAB_Sensor_Data_Center] >= MaxLimit)
		{
			//센터로부터 Left 방향
			for(int i = SOSLAB_Sensor_Data_Center-1; i >= 0; i--)
			{
                //MaxLimit, MinLimit은 감지 level 위치(SOSLAB은 변경해줘야될수도 있음)
                if(data[i] <= MaxLimit && data[i] >= MinLimit)
                {
                    left_cnt++;
					if(left_cnt == 1)    //???
					{
						start_x_pos = i;  //감지 시작 지점
                    }
                    else if(left_cnt == SOSLAB_OBS_Sensor_Offset)
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

			//센터로부터 Right 방향
            for(int j = SOSLAB_Sensor_Data_Center+1; j < SOSLAB_Sensor_Data_Num; j++)
			{
				//MaxLimit, MinLimit은 감지 level 위치(SOSLAB은 변경해줘야될수도 있음)
                if(data[j] <= MaxLimit && data[j] >= MinLimit)
                {
                    right_cnt++;
                    if(right_cnt == 1)
                    {
                        end_x_pos = j;  //감지 끝 지점
                    }
                    else if(right_cnt == SOSLAB_OBS_Sensor_Offset)
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
				left_diff = (SOSLAB_Sensor_Data_Center) - start_x_pos;
                right_diff = end_x_pos - (SOSLAB_Sensor_Data_Center);
                center_pos = left_diff - right_diff;
                //Sensor_Debug->Lines->Add(str.sprintf("[right] %d - [left] %d = [center] %d", right_diff, left_diff, center_pos));

                edt_SensorLeft->Text = str.sprintf("%d", left_diff);
                edt_SensorRight->Text = str.sprintf("%d", right_diff);
                edt_SensorCenter->Text = str.sprintf("%d", center_pos);

                if(center_pos >= SOSLABOHT_Sensor_Margin_Neg && center_pos <= SOSLABOHT_Sensor_Margin_Pos)
                {
					if(left_diff >= SOSLABSensorOBSOhtMin &&  right_diff >= SOSLABSensorOBSOhtMin)
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
    else if(nSensorType == UBG_OHTDETECT_SOSLAB_SENSOR)
    {
		if(data[SOSLAB_Sensor_Data_Center] <= MaxLimit && data[SOSLAB_Sensor_Data_Center] >= MinLimit)
        {
			for(int i = SOSLAB_Sensor_Data_Center-1; i >= 0; i--)
            {
				if(data[i] >= MaxLimit)
                {
                    left_cnt++;
                    if(left_cnt == 1)
					{
                        start_x_pos = i;
                    }
                    else if(left_cnt == SOSLAB_OHT_Sensor_Offset)
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

            for(int j = SOSLAB_Sensor_Data_Center+1; j < SOSLAB_Sensor_Data_Num; j++)
            {
                if(data[j] >= MaxLimit)
                {
                    right_cnt++;
                    if(right_cnt == 1)
                    {
                        end_x_pos = j;
                    }
                    else if(right_cnt == SOSLAB_OHT_Sensor_Offset)
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
				left_diff = (SOSLAB_Sensor_Data_Center) - start_x_pos;
                right_diff = end_x_pos - (SOSLAB_Sensor_Data_Center);
                center_pos = left_diff - right_diff;
                //Sensor_Debug->Lines->Add(str.sprintf("[right] %d - [left] %d = [center] %d", right_diff, left_diff, center_pos));

                edt_SensorLeft->Text = str.sprintf("%d", left_diff);
                edt_SensorRight->Text = str.sprintf("%d", right_diff);
                edt_SensorCenter->Text = str.sprintf("%d", center_pos);

                if(center_pos >= SOSLABOHT_Sensor_Margin_Neg && center_pos <= SOSLABOHT_Sensor_Margin_Pos)
                {
                    if(left_diff >= SOSLABSensorUbgOhtMin &&  right_diff >= SOSLABSensorUbgOhtMin)
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
			if(data[SOSLAB_Sensor_Data_Center] < MinLimit)
			{
				m_nResult = SENSOR_INSTALL_RESULT_NG_OHT_POSITION;
			}
			else
			{
				m_nResult = SENSOR_INSTALL_RESULT_NG_OHT_VERTICAL_LEVEL;
			}
        }
	}

	if(nPreResultSOSLAB != m_nResult)
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
			case SENSOR_INSTALL_RESULT_NG_FINE_DETECT_SENSOR:
				edt_SensorSettingResult->Color = clRed;
				edt_SensorSettingResult->Text = "영점조절 및 SETUP JIG 확인 필요";
				Sensor_Debug->Lines->Add("X 또는Y축이 정상범위를 초과하였습니다. 조치 후 영점조절을 다시 해주세요.");
				break;
        }
    }
    nPreResultSOSLAB = m_nResult;
}

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
				left_diff = (Sensor_Data_Center-1) - start_x_pos;  //left 미감지 영역
                right_diff = end_x_pos - (Sensor_Data_Center-1);   //Right 미감지 영역
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
			case SENSOR_INSTALL_RESULT_NG_FINE_DETECT_SENSOR:
				edt_SensorSettingResult->Color = clRed;
				edt_SensorSettingResult->Text = "영점조절 및 SETUP JIG 확인 필요";
				Sensor_Debug->Lines->Add("X 또는Y축이 정상범위를 초과하였습니다. 조치 후 영점조절을 다시 해주세요.");
				break;
        }
    }
    nPreResult = m_nResult;
}
//---------------------------------------------------------------------------
bool TOBS_UBG_Form::Check_CenterPos_LookDown(int *data)
{
	bool check_success = false;
	int check_cnt = 0;

    AnsiString str = "";

	static int nPreResultLookDown = 0;

	TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
	String strLog;


	for(int i=0; i < LookDown_Data_Num; i++)
	{

		if(i >= 44 && i <= 59)      //44번~ 59번까지 유효 데이터
		{
			if(data[i] >= 400 && data[i] <= 5000) // 거리값이 400mm~1000인 유효 데이터

			check_cnt++;


			strLog.sprintf(L"cnt : %d, nDistance: %d, check_cnt: %d", i, data[i], check_cnt);
			m_LogFile->WriteLogFile("TP_LookDown", strLog);


			if(check_cnt == 16)
			{
				strLog.sprintf(L"OK, cnt : %d, nDistance: %d, check_cnt: %d", i, data[i], check_cnt);
				m_LogFile->WriteLogFile("TP_LookDown", strLog);
				check_success = true;  //감지 끝 지점
				break;
			}

		}
	}

	if(check_success)
	{

		m_nResult = SENSOR_INSTALL_RESULT_OK;
	}
	else
	{
		m_nResult = SENSOR_INSTALL_RESULT_NG_REFLECTOR_BAD;
	}


	if(nPreResultLookDown != m_nResult)
    {
		switch(m_nResult)
        {
            case SENSOR_INSTALL_RESULT_OK:
                edt_SensorSettingResult->Color = clGreen;
                edt_SensorSettingResult->Text = "GOOD";
				Sensor_Debug->Lines->Add("[OK] 센서 설정 정상");
                break;

			case SENSOR_INSTALL_RESULT_NG_REFLECTOR_BAD:
                edt_SensorSettingResult->Color = clRed;
				edt_SensorSettingResult->Text = "BAD";
				Sensor_Debug->Lines->Add("[NG] 각도 조정 필요");
				break;
        }
    }
	nPreResultLookDown = m_nResult;

	delete m_LogFile;
}
//---------------------------------------------------------------------------
int TOBS_UBG_Form::Cal_Dis(int cnt, int dis, int type)
{
    int nDistance = 0;
    double dSensorDegree = 0.0;

    if(type == 0)
	{
		//HOKUYO
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
	else if(type == 1)
	{


			//SOSLAB LAB
			nDistance = (dis & 0xffff0000) >> 16;

			/*
			//직선 거리값 확인 Data 확인 후 비교 필요(현재 가진 정보 : 단위 mm)
			if(cnt <= SOSLAB_Sensor_Data_Center)
			{
				dSensorDegree = (double)(SOSLAB_SensorUnitDegree * cnt);
				dSensorDegree = (double)((dSensorDegree * MATH_PI)/180.0);
				nDistance = (double)nDistance * Cos(dSensorDegree);
			}
			else
			{
				dSensorDegree = 180.0 - (double)(SOSLAB_SensorUnitDegree * cnt);
				dSensorDegree = (double)((dSensorDegree * MATH_PI)/180.0);
				nDistance = (double)nDistance * Cos(dSensorDegree);
			}
			*/
    }
    else
    {
        if(cnt <= Sensor_Data_Center) // 미사용
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

void __fastcall TOBS_UBG_Form::Btn_repair_StartClick(TObject *Sender)
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

	int nDetectCheckArea[5] = {0,};

	nDetectCheckArea[1] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel == 0) ? 1 : 0;
	nDetectCheckArea[2] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel == 0) ? 2 : 0;
	nDetectCheckArea[3] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel == 0) ? 4 : 0;
	nDetectCheckArea[4] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel == 0) ? 8 : 0;

	nDetectCheckArea[0] = nDetectCheckArea[1] + nDetectCheckArea[2] + nDetectCheckArea[3] + nDetectCheckArea[4];


	if(nDetectCheckArea[0] !=0)
	{
		MainForm->Warnning_Display(clRed, "OHT Detect Sensor is Not Init. Please Retry After ReMark.");
		Sensor_Debug->Lines->Add("OHT Detect Sensor is Not Init. Please Retry After ReMark.");
		return;
    }

	TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

	m_CMDControl->MakeTPCmd2OHT(TPCMD_UBG_SENSOR_REQUEST_START);
	Sensor_Debug->Lines->Add("Request UBG OHT Detect Sensor Data");

	delete m_CMDControl;


    //결과 Check
	m_nResultCheck = 0;
	m_nResultCheckCount = 0;
    SensorDataTimer2->Enabled = true;
	//SensorData_ComboBox->Enabled = false;
	Btn_repair_Stop->Enabled = true;
	Btn_repair_Start->Enabled = false;
	Btn_Check_Start->Enabled = false;
	Btn_repair_Clear->Enabled = true;

}
//---------------------------------------------------------------------------

void __fastcall TOBS_UBG_Form::SensorDataTimer2Timer(TObject *Sender)
{
    int nDistance[Sensor_Data_Num] = {0,};
    Series1->Clear();

   	Chart1->LeftAxis->Minimum = 0;
	Chart1->LeftAxis->Maximum = 6000;

	int CheckCount = 0;

    for(int i=0; i<Sensor_Data_Num ; i++)
    {

		nDistance[i] = gm_OHTStatus->m_OHTStatus.DistanceSensorData.nUBG[i];

		if(nDistance[i] !=1024)
		{
           CheckCount++;
        }
	}

	Check_CenterPos2(CheckCount); //내용 변경 필요
}
//---------------------------------------------------------------------------

void __fastcall TOBS_UBG_Form::Btn_repair_StopClick(TObject *Sender)
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
	m_CMDControl->MakeTPCmd2OHT(TPCMD_DISTANCE_SENSOR_REQUEST_STOP);

	delete m_CMDControl;

	SensorDataTimer2->Enabled = false;
	//SensorData_ComboBox->Enabled = true;
	Btn_Check_Start->Enabled = true;
	Btn_repair_Stop->Enabled = false;
	Btn_repair_Start->Enabled = true;
	Btn_repair_Clear->Enabled = false;
}

bool TOBS_UBG_Form::Check_CenterPos2(int data)
{
    AnsiString str = "";
	int nDetectCheckArea[5] = {0,};

	nDetectCheckArea[1] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel == 0) ? 1 : 0;
	nDetectCheckArea[2] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel == 0) ? 2 : 0;
	nDetectCheckArea[3] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel == 0) ? 4 : 0;
	nDetectCheckArea[4] = (gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel == 0) ? 8 : 0;

	nDetectCheckArea[0] = nDetectCheckArea[1] + nDetectCheckArea[2] + nDetectCheckArea[3] + nDetectCheckArea[4];


	if(nDetectCheckArea[0] !=0)
	{
		m_nResultCheck = 0;
		m_nResultCheckCount = 0;
		edt_SensorCheckResult->Color = clGray;
		edt_SensorCheckResult->Text = "점검종료";

		SensorDataTimer2->Enabled = false;
		Btn_Check_Start->Enabled = true;
		Btn_repair_Stop->Enabled = false;
		Btn_repair_Start->Enabled = true;
		return false;
	}

	if(data !=0)
	{
		m_nResultCheckCount++;
		if(m_nResultCheckCount >= Edt_repair_Count->Text.ToInt())
			m_nResultCheck = SENSOR_INSTALL_RESULT_NG_OHT_VERTICAL_LEVEL;
		else
			m_nResultCheck = SENSOR_INSTALL_RESULT_OK;
	}
	else
	{
		if(m_nResultCheck == 0)
			m_nResultCheck = SENSOR_INSTALL_RESULT_OK;
		else if(m_nResultCheckCount >= Edt_repair_Count->Text.ToInt())
			m_nResultCheck = SENSOR_INSTALL_RESULT_NG_OHT_VERTICAL_LEVEL;
		// 시작이후에 문제생기면 문제 유지.. ==> 재확인 필요시, Claer 버튼 누룬 후 재확인 할 것
	}


		switch(m_nResultCheck)
		{
			case 0:
				edt_SensorCheckResult->Color = clGray;
				edt_SensorCheckResult->Text = "INIT";
				Sensor_Debug->Lines->Add("[INIT] 초기화 상태상");
				break;

			case SENSOR_INSTALL_RESULT_OK:
				edt_SensorCheckResult->Color = clGreen;
				edt_SensorCheckResult->Text = "GOOD";
				str.sprintf("[OK] 센서 설정 정상 %d/%d", m_nResultCheckCount,data);
				Sensor_Debug->Lines->Add(str);
				break;

			case SENSOR_INSTALL_RESULT_NG_OHT_VERTICAL_LEVEL:
				edt_SensorCheckResult->Color = clRed;
				edt_SensorCheckResult->Text = "BAD";
				Sensor_Debug->Lines->Add("[NG] I/O MissMatch(보드 점검 확인 필요)");
				str.sprintf("[NG] I/O MissMatch(보드 점검 확인 필요) %d/%d", m_nResultCheckCount,data);
				Sensor_Debug->Lines->Add(str);
				break;
		}

}

//---------------------------------------------------------------------------

void __fastcall TOBS_UBG_Form::Btn_repair_ClearClick(TObject *Sender)
{
	m_nResultCheck = 0;
	m_nResultCheckCount = 0;
	edt_SensorCheckResult->Color = clGray;
	edt_SensorCheckResult->Text = "INIT";
	Sensor_Debug->Lines->Add("[INIT] 초기화 상태상");

}
//---------------------------------------------------------------------------



