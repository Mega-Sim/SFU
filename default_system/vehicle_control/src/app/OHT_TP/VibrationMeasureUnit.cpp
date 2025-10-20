//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VibrationMeasureUnit.h"
#include "OHTInfoUnit.h"
#include "FFTViewer.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Chart"
#pragma link "Series"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma link "cgauges"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
String txtSteerStatus[3] = {"MOVE","LEFT","RIGHT"};
String txtSpeedMode[4] = {"NORMAL","ACCEL","DECEL","CRUISE"};
String txtUBGSensorArea[8]={"No Detect","N","L","R","S","B","V","P"};
String txtOBSSensorArea[15]={"No Detect","N","L","R","S","B","V","STB_L","STB_R","N_R","N_L","U_L","U_R","BAY","ALL"};

char chSteerStatus[3] = {'M','L','R'};
int nSavingTime[10] = {10, 20, 30, 60, 120, 180, 300, 480, 600, 900};
int nSamplingTime[5] = {25, 50, 100, 500, 1000};


AnsiString CycleTimeToString(int t)
{
	AnsiString strTime;

	if(t < 60)
		strTime.sprintf("%d sec", t);
	else
		strTime.sprintf("%d min", t/60);

	return strTime;
}

AnsiString SamplingTimeToString(int t)
{
	AnsiString strTime;

	strTime.sprintf("%d ms", t);

	return strTime;
}

//---------------------------------------------------------------------------
TfrmVibrationMeasurement *frmVibrationMeasurement;
//---------------------------------------------------------------------------
__fastcall TfrmVibrationMeasurement::TfrmVibrationMeasurement(TComponent* Owner)
	: TForm(Owner)
{

	SeriesFrontMotor->DrawAllPoints = false;
	SeriesRearMotor->DrawAllPoints = false;
	SeriesTransMotor->DrawAllPoints = false;
	//SeriesMotorPos->DrawAllPoints = false;
	SeriesVibRMS->DrawAllPoints = false;
	SeriesVibX->DrawAllPoints = false;
	SeriesVibY->DrawAllPoints = false;
	SeriesVibZ->DrawAllPoints = false;
	//SeriesVibPos->DrawAllPoints = false;

	String strOHTPath = "";
	m_fTimeOffset = 0.0;
	MeasurementInitialization();
	strOHTPath = ExtractFileDir(Application->ExeName);
	edtSaveFilePath->Text = strOHTPath + "\\VIB";
	mStrLogFileName = edtSaveFilePath->Text;

	nSensorConnectionStep = VIB_SENSOR_DISCONNECTED;
	nSensorConnectionCheckCnt = 0;
	mBackUpUdpListenerInterval = 0; //MeasurementInitialization()으로 들어가면 안됨
	dVibFilterThreshold = edtThresholdOutput->Text.ToDouble();

	OpenDialog->Filter = "Vib log file (*.csv)|*.csv";
	cgGauge->MaxValue = 0;
	cgGauge->Progress = 0;

	btFileOpen->Enabled = true;
	btStopDraw->Enabled = false;
	m_bStopDrawChart = false;

	m_nSaveDataNum = 0;

	FILE* fs = NULL;

	m_dVibMax = 0;
	m_nMaxCnt = 0;


	for(int i=0; i<10; i++)
		cbbSavingTime->Items->Add(CycleTimeToString(nSavingTime[i]));

	//bIsDriveTrans = false;


}
//---------------------------------------------------------------------------
String TfrmVibrationMeasurement::GetLogFileName(String strFileName)
{
	String fileName = "";
	SYSTEMTIME LocalTime;
	GetLocalTime(&LocalTime);
	strFileName += '_';
	fileName.sprintf(L"%s%02d%02d%02d%02d%02d.csv", strFileName,/*LocalTime.wYear,*/ LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);
	//strFilePath = "";
	return fileName;
}
//---------------------------------------------------------------------------
void TfrmVibrationMeasurement::MeasurementInitialization()
{
	// init

    bVibMeasDrawLine = false;
	bVibMeasMouseDown = false;

	//dVibTime_sec = 0.0;
	dVelTime_sec = 0;
	//dVelTime_sec = dVibTime_sec + m_fTimeOffset + BOTTOMAXIS_RESOLUTION_SEC_VEL;
	dVibTime_sec = dVelTime_sec - m_fTimeOffset - BOTTOMAXIS_RESOLUTION_SEC_VEL;
	dMinVibTime_sec = 0.0;
	

	//nSampleInScreen = (int)(BOTTOMAXIS_SEC_MAX/BOTTOMAXIS_RESOLUTION);

	chartDrivingMotorSpeed->BottomAxis->Automatic = false;
	chartDrivingMotorSpeed->BottomAxis->Minimum = 0;
	chartDrivingMotorSpeed->BottomAxis->Maximum = BOTTOMAXIS_SEC_MAX;

	chartVibRms->BottomAxis->Automatic = false;
	chartVibRms->BottomAxis->Minimum = 0;
	chartVibRms->BottomAxis->Maximum = BOTTOMAXIS_SEC_MAX;

	ScrollBarVibTime->Position = 0;
	ScrollBarVibTime->Min = 0;
	ScrollBarVibTime->Max = 1;

	SeriesFrontMotor->Clear();
	SeriesRearMotor->Clear();
	SeriesTransMotor->Clear();
	SeriesVibRMS->Clear();
	SeriesVibX->Clear();
	SeriesVibY->Clear();
	SeriesVibZ->Clear();
	SeriesVibPos->Clear();
	SeriesVibPosRange->Clear();
	SeriesMotorPos->Clear();
	SeriesMotorPosRange->Clear();

	AvsDataBuffer.clear();
	AvsSpecOutBuffer.clear();

	ListBoxSpecOutList->Clear();
	SeriesSpecOutPoint->Clear();
}


//---------------------------------------------------------------------------
void __fastcall TfrmVibrationMeasurement::btnSensorConnectionClick(TObject *Sender)

{
	if(nSensorConnectionStep == VIB_SENSOR_DISCONNECTED)
	{
		btnSensorConnection->Caption = "Connecting...";
		nSensorConnectionStep = VIB_SENSOR_CONNECTING;

		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_VIB_DATA_LINK);
		delete m_CMDControl;

		TimerCheckConnect->Interval = 100; // 5초 후에도 connection 안되면 취소
		TimerCheckConnect->Enabled = true;
	}
	else if(nSensorConnectionStep ==VIB_SENSOR_CONNECTED)
	{
		btnVibMeasureStart->Enabled = true;

		btnSensorConnection->Caption = "Connect";
		LabelStatus->Caption = "Status: Disconnected to sensor" ;
		nSensorConnectionStep = VIB_SENSOR_DISCONNECTED;

		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_VIB_DATA_CLOSE);
		delete m_CMDControl;

		MainForm->tmrUdpListen->Interval = mBackUpUdpListenerInterval;
	}
}
//---------------------------------------------------------------------------

void TfrmVibrationMeasurement::Receive_AVS_Response(int nResponse)
{
	if(nResponse == TagID::CMD_TP_VIB_DATA_LINK)
	{
		TimerCheckConnect->Enabled = false;
		btnSensorConnection->Caption = "Disconnect";
		LabelStatus->Caption = "Status: Connected to sensor" ;
		nSensorConnectionStep = VIB_SENSOR_CONNECTED;
		mBackUpUdpListenerInterval = MainForm->tmrUdpListen->Interval;
		MainForm->tmrUdpListen->Interval = NUM_OF_VIB_SAMPLE / 2;

		btnVibMeasureStart->Enabled = true;
		btnVibMeasureStop->Enabled = true;
		tsPlayback->Enabled = false;
	}
	else if(nResponse == TagID::CMD_TP_VIB_DATA_CLOSE)
	{
		nSensorConnectionStep = VIB_SENSOR_DISCONNECTED;
		LabelStatus->Caption = "Status: Disconnected to sensor" ;
		btnSensorConnection->Caption = "Connect";
		btnVibMeasureStart->Enabled = false;
		btnVibMeasureStop->Enabled = false;
		tsPlayback->Enabled = true;
    }
}

//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::btnVibMeasureStartClick(TObject *Sender)
{
	if(nSensorConnectionStep != VIB_SENSOR_CONNECTED) return;
	bool bRealTimeLogSave;
	btnVibMeasureStart->Enabled = false;
	edtVelTimeOffset->Enabled = false;
	try{
		int nTimeOffset = 1000 * edtVelTimeOffset->Text.ToDouble();
		String strTemp = "";
		nTimeOffset = (((double)nTimeOffset / NUM_OF_VIB_SAMPLE) + 0.5);
		m_fTimeOffset = ((double)(nTimeOffset * NUM_OF_VIB_SAMPLE))/1000.0;//edtVelTimeOffset->Text.ToDouble();
		strTemp.sprintf(L"%.3f",m_fTimeOffset);
		edtVelTimeOffset->Text = strTemp;
	}
	catch(...)
	{
		m_fTimeOffset = 0;
	}

	MeasurementInitialization();

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_VIB_DATA_REQ_START);
	mBackUpUdpListenerInterval = MainForm->tmrUdpListen->Interval;
	MainForm->tmrUdpListen->Interval = NUM_OF_VIB_SAMPLE / 2;
	delete m_CMDControl;

	LabelStatus->Caption = "Status: Started receiving" ;

	btnSensorConnection->Enabled = false;
	tmrSendAlive->Enabled = true;
	m_dVibMax = 0;
	m_nMaxCnt = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::btnVibMeasureStopClick(TObject *Sender)

{
	if(nSensorConnectionStep != VIB_SENSOR_CONNECTED) return;

	bool bRealTimeLogSave;
	btnChangeSaveFilePath->Enabled = bRealTimeLogSave;
	edtSaveFilePath->Enabled = bRealTimeLogSave;
	btnVibMeasureStart->Enabled = true;
	edtVelTimeOffset->Enabled = true;
	if(bRealTimeLogSave)
	{
		fflush(fp);
		fclose(fp);
	}

 	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_VIB_DATA_REQ_STOP);
	delete m_CMDControl;
	MainForm->tmrUdpListen->Interval = mBackUpUdpListenerInterval;

	LabelStatus->Caption = "Status: Stoped receiving" ;

	btnSensorConnection->Enabled = true;
	tmrSendAlive->Enabled = false;
}

//---------------------------------------------------------------------------
void TfrmVibrationMeasurement::DrawCheckLineDisplayInfo(int ValueIndex, double X)
{
	unsigned int NodeId;
	int NodeOffset;
	double VelFrontMotor;// = SeriesMotorPos->YValue[ValueIndex];
	BYTE SteerF;
	BYTE SteerR;
	BYTE SpeedMode;
	int UBGLV;
	int UBGRegion;
	int OBSLV;
	int OBSRegion;

	String strData;
	
	list<AVS_Data>::iterator iterAvsDataBuffer;

	iterAvsDataBuffer = AvsDataBuffer.begin();

	for(int i = 0; i < ValueIndex; i++) {
		iterAvsDataBuffer++;
	}
	NodeId = iterAvsDataBuffer->CurNodeID;
	NodeOffset = iterAvsDataBuffer->CurNodeOffset;
	SteerR = iterAvsDataBuffer->SteerRear;
	SteerF = iterAvsDataBuffer->SteerFront;
    SpeedMode = iterAvsDataBuffer->SpeedMode;
	VelFrontMotor = iterAvsDataBuffer->VelFrontMotor;

	UBGLV =  iterAvsDataBuffer->UBGDetectLevel;
	UBGRegion =  iterAvsDataBuffer->UBGDetectRegion;
	OBSLV = iterAvsDataBuffer->OBSDetectLevel;
	OBSRegion = iterAvsDataBuffer->OBSDetectRegion;

	GetUBGLVtoSensor(UBGLV);
	GetUBGRegionToSensor(UBGRegion);
	GetOBSLVtoSensor(OBSLV);
	GetOBSRegionToSensor(OBSRegion);

	edtCurrentNode->Text = NodeId;
	edtOffsetDistance->Text = NodeOffset;
	strData.sprintf(L"%.02f", VelFrontMotor);
	edtVhlVel->Text = strData;
	edtSteerFront->Text = txtSteerStatus[SteerF];
	edtSteerRear->Text = txtSteerStatus[SteerR];
	edtSpeedMode->Text = txtSpeedMode[SpeedMode];

	SeriesVibPos->Clear();
	SeriesMotorPos->Clear();
	SeriesMotorPos->AddXY(X,chartDrivingMotorSpeed->LeftAxis->Maximum,"",clRed);
	SeriesMotorPos->AddXY(X,chartDrivingMotorSpeed->LeftAxis->Minimum,"",clRed);
	SeriesVibPos->AddXY(X,chartVibRms->LeftAxis->Maximum,"",clRed);
	SeriesVibPos->AddXY(X,chartVibRms->LeftAxis->Minimum,"",clRed);

	if(FFTViewerForm->Showing)
	{
		SeriesVibPosRange->Clear();
		SeriesMotorPosRange->Clear();
		SeriesMotorPosRange->AddXY(X+1,chartDrivingMotorSpeed->LeftAxis->Maximum,"",clRed);
		SeriesMotorPosRange->AddXY(X+1,chartDrivingMotorSpeed->LeftAxis->Minimum,"",clRed);
		SeriesVibPosRange->AddXY(X+1,chartVibRms->LeftAxis->Maximum,"",clRed);
		SeriesVibPosRange->AddXY(X+1,chartVibRms->LeftAxis->Minimum,"",clRed);
	}

	bVibMeasDrawLine = true;

}



//---------------------------------------------------------------------------
void __fastcall TfrmVibrationMeasurement::chartVibRmsClickSeries(TCustomChart *Sender,
		  TChartSeries *Series, int ValueIndex, TMouseButton Button, TShiftState Shift,
          int X, int Y)
{
	String strData;
	String tible = Series->Title;

	if("SpecOver" == Series->Title)
	{
		double x;
		x = Series->XValue[ValueIndex];
		for(int i = 0; i < SeriesVibRMS->Count(); i++)
		{
			if(x == SeriesVibRMS->XValue[i]){
				ValueIndex = i;
            	break;
			}
		}
	}
	if((ValueIndex < 0) || (ValueIndex >= SeriesVibRMS->Count())) return;

	DrawCheckLineDisplayInfo(ValueIndex, SeriesVibRMS->XValue[ValueIndex]);

	strData.sprintf(L"%.02f", SeriesVibX->YValue[ValueIndex]);
	edtVibX->Text = strData;
	strData.sprintf(L"%.02f", SeriesVibY->YValue[ValueIndex]);
	edtVibY->Text = strData;
	strData.sprintf(L"%.02f", SeriesVibZ->YValue[ValueIndex]);
	edtVibZ->Text = strData;

	strData.sprintf(L"%.02f", SeriesVibRMS->YValue[ValueIndex]);
	edtVibRMS->Text = strData;

	if(FFTViewerForm->Showing)
	{
		FFTViewerForm->initForm();
		FFTViewerForm->SetStartTime(ValueIndex);
		FFTViewerForm->SetEndTime(ValueIndex + AVS_FREQ);
		FFTViewerForm->SetSeriesData((TFastLineSeries*)Series);
		FFTViewerForm->ExecuteFFT();

		FFTViewerForm->Show();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::chartDrivingMotorSpeedClickSeries(TCustomChart *Sender,
		  TChartSeries *Series, int ValueIndex, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{

	if(ValueIndex == 0) return;
	DrawCheckLineDisplayInfo(ValueIndex, Series->XValue[ValueIndex]);

	String strData;
	strData.sprintf(L"%.02f", SeriesVibX->YValue[ValueIndex]);
	edtVibX->Text = strData;
	strData.sprintf(L"%.02f", SeriesVibY->YValue[ValueIndex]);
	edtVibY->Text = strData;
	strData.sprintf(L"%.02f", SeriesVibZ->YValue[ValueIndex]);
	edtVibZ->Text = strData;
	strData.sprintf(L"%.02f", SeriesVibRMS->YValue[ValueIndex]);
	edtVibRMS->Text = strData;

}
//---------------------------------------------------------------------------



void __fastcall TfrmVibrationMeasurement::ScrollBarVibTimeChange(TObject *Sender)

{
	float bottom_axis_min;
	float bottom_axis_max;
	// Position= 0 ~ SeriesFrontMotor->Count()[150] - 10s/0.1s[100] + 1
	bottom_axis_min = dMinVibTime_sec + ScrollBarVibTime->Position*BOTTOMAXIS_RESOLUTION_SEC_VEL;
	bottom_axis_max = bottom_axis_min + BOTTOMAXIS_SEC_MAX;
	chartVibRms->BottomAxis->Minimum = 0;
	chartVibRms->BottomAxis->Maximum = bottom_axis_max;
	chartVibRms->BottomAxis->Minimum = bottom_axis_min;

	chartDrivingMotorSpeed->BottomAxis->Minimum = 0;
	chartDrivingMotorSpeed->BottomAxis->Maximum = bottom_axis_max;
	chartDrivingMotorSpeed->BottomAxis->Minimum = bottom_axis_min;
}
//---------------------------------------------------------------------------



void __fastcall TfrmVibrationMeasurement::cbVelFrontClick(TObject *Sender)
{
	SeriesFrontMotor->Visible = cbVelFront->Checked ? true:false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::cbVelRearClick(TObject *Sender)
{
	SeriesRearMotor->Visible = cbVelRear->Checked ? true:false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::cbVelTransClick(TObject *Sender)
{
	SeriesTransMotor->Visible = cbVelTrans->Checked ? true:false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::cbVibRMSClick(TObject *Sender)
{
	SeriesVibRMS->Visible = cbVibRMS->Checked ? true:false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::cbVibXClick(TObject *Sender)
{
	SeriesVibX->Visible = cbVibX->Checked ? true:false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::cbVibYClick(TObject *Sender)
{
	SeriesVibY->Visible = cbVibY->Checked ? true:false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::cbVibZClick(TObject *Sender)
{
	SeriesVibZ->Visible = cbVibZ->Checked ? true:false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::TimerCheckConnectTimer(TObject *Sender)

{
	static int nConnectCount = 0;

	if(nConnectCount > 50)
	{
		TimerCheckConnect->Enabled = false;
		btnSensorConnection->Caption = "Connect";
		LabelStatus->Caption = "Status: Disconnected to sensor" ;
		nSensorConnectionStep = VIB_SENSOR_DISCONNECTED;
		ShowMessage("센서 연결 실패! 연결을 확인 해 주세요");
		nConnectCount = 0;
	}
	else
	{
		nConnectCount++;
	}
}
//---------------------------------------------------------------------------
// temp
#include <mmsystem.h>
void TfrmVibrationMeasurement::DisplayVibrationData(AVS_VIBRATION_DATA_INFO VibrationInfo, bool bAnalysys)
{
	int dwTimeStart = timeGetTime();

	AVS_Data TempAVSData;
	String strData;

	float vibX;
	float vibY;
	float vibZ;
	float vibRms;
	double VelFrontMotor;
	int nOffset;
	static int nPreOffset = 0;
	UINT nTag;
	float preVibRms = 0;

	WORD nSteerF;
	WORD nSteerR;
	WORD nSpeedMode;

	static int tempIndex_vib = 0;
	static int tempIndex_vel = 0;

	VelFrontMotor = VibrationInfo.VelFrontMotor;
	nOffset = VibrationInfo.CurNodeOffset;
	nTag = VibrationInfo.CurNodeID;

	if(VibrationInfo.SteerFrontL == 1) nSteerF = 1;
	else if(VibrationInfo.SteerFrontR == 1) nSteerF = 2;
	else nSteerF = 0;

	if(VibrationInfo.SteerRearL == 1) nSteerR = 1;
	else if(VibrationInfo.SteerRearR == 1) nSteerR = 2;
	else nSteerR = 0;

	nSpeedMode = (WORD)VibrationInfo.SpeedMode;


	//ubg, obs
	bool bUBGLong   = VibrationInfo.UBGInputLong;
	bool bUBGMiddle = VibrationInfo.UBGInputMiddle;
	bool bUBGShort  = VibrationInfo.UBGInputShort;
	bool bUBGFail   = VibrationInfo.UBGInputFail;

	bool bUBGSel1  = VibrationInfo.UBGOutputSel1;
	bool bUBGSel2  = VibrationInfo.UBGOutputSel2;
	bool bUBGSel3  = VibrationInfo.UBGOutputSel3;
	bool bUBGSel4  = VibrationInfo.UBGOutputSel4;

	bool bOBSLong   = VibrationInfo.OBSInputLong;
	bool bOBSMiddle = VibrationInfo.OBSInputMiddle;
	bool bOBSShort  = VibrationInfo.OBSInputShort;
	bool bOBSFail   = VibrationInfo.OBSInputFail;

	bool bOBSSel1  = VibrationInfo.OBSOutputSel1;
	bool bOBSSel2  = VibrationInfo.OBSOutputSel2;
	bool bOBSSel3  = VibrationInfo.OBSOutputSel3;
	bool bOBSSel4  = VibrationInfo.OBSOutputSel4;

	int nUBGDetectLevel = GetUBGSensorLevel(bUBGLong, bUBGMiddle, bUBGShort, bUBGFail);
	int nUBGDetectRegion = GetUBGSensorRegion(bUBGSel1, bUBGSel2, bUBGSel3, bUBGSel4);
	int nOBSDetectLevel = GetOBSSensorLevel(bOBSLong, bOBSMiddle, bOBSShort, bOBSFail);
	int nOBSDetectRegion = GetOBSSensorRegion(bOBSSel1, bOBSSel2, bOBSSel3, bOBSSel4);

//	//Trans Sensor, Step, Pos ...
//	 bool HandPushSen 			= VibrationInfo.HandPushSen;
//	 bool InputHandOpen			= VibrationInfo.InputHandOpen;
//	 bool InputHandClose		= VibrationInfo.InputHandClose;
//	 bool InputHandGripLeft		= VibrationInfo.InputHandGripLeft;
//	 bool InputHandGripRight	= VibrationInfo.InputHandGripRight;
//	 bool InputHandInposition	= VibrationInfo.InputHandInposition;
//	 bool HoistHomeSen			= VibrationInfo.HoistHomeSen;
//
//
//	 //Trans Step, Pos
//	 int TranStepDetail 		= VibrationInfo.TranStepDetail;
//	 int TransTypeDetail 		= VibrationInfo.TransTypeDetail;
//	 double TransHoistPos		= VibrationInfo.TransHoistPos;
//	 double VelTransMotor 		= VibrationInfo.VelTransMotor;

	//Vibration Max Data
	float MaxX = 0;
	float MaxY = 0;
	float MaxZ = 0;
	float MaxRMS = 0;


	//진동 50개 Data + OHT Log Data 저장, 계산 부분
	for(int i = 0; i < NUM_OF_SAMPLE; i++)
	{
		vibX = VibrationInfo.fX[i];
		vibY = VibrationInfo.fY[i];

		if(!bAnalysys)
			vibZ = VibrationInfo.fZ[i]-1;
		else
		{
			vibZ = VibrationInfo.fZ[i];
		}
		vibRms = sqrt((vibX*vibX + vibY*vibY + (vibZ)*(vibZ))/3);

		//50개 중 가장 큰 값으로 Graph, SpecOut List 추가
		if(MaxRMS<=vibRms)
		{
			MaxX = vibX;
			MaxY = vibY;
			MaxZ = vibZ;
			MaxRMS = vibRms;
		}

		//파일저장시
		if(bIsFileOpen)
		{
			m_nSaveDataNum++;
//			if(!bIsDriveTrans)
//			{
				//주행 Data
				fprintf(fp, "%d,%06d,%d,%.02f,%c,%c,%d,%d,%d,%d,%.02f,%.02f,%.02f,%.02f,%d\n",
				m_nSaveDataNum, nTag, nOffset, VelFrontMotor, chSteerStatus[nSteerF], chSteerStatus[nSteerR],
				nUBGDetectLevel, nUBGDetectRegion, nOBSDetectLevel, nOBSDetectRegion,
				vibRms, vibX, vibY, vibZ, VibrationInfo.SpeedMode);
//			}
//			else
//			{
//				//이적재용 Data
//				fprintf(fp, "%d,%06d,%d,%d,%.02f,%.02f,%d,%d,%d,%d,%d,%d,%d,%.02f,%.02f,%.02f,%.02f\n",
//				m_nSaveDataNum, nTag, TransTypeDetail, TranStepDetail, TransHoistPos, VelTransMotor,
//				HandPushSen,  InputHandOpen, InputHandClose, InputHandGripLeft, InputHandGripRight,
//				InputHandInposition,  HoistHomeSen,
//				vibRms, vibX, vibY, vibZ);
//			}


			//설정된 시간(갯수)을 초과하면 파일 저장 종료

			if(m_nSaveDataNum >= nSavingTime[cbbSavingTime->ItemIndex]*1000)
			{
				bIsFileOpen = false;
				cbbSavingTime->Enabled = true;
				btnChangeSaveFilePath->Enabled = true;
				edtSaveFilePath->Enabled = true;
				tmrUIUpdate->Enabled = false;
				btLogSaveStart->Caption = "시작";
				//chkDriveTrans->Enabled  = true;
				fflush(fp);
				fclose(fp);
			}
		}

	}

	//그래프용 Buffer
	TempAVSData.fX = MaxX;
	TempAVSData.fY = MaxY;
	TempAVSData.fZ = MaxZ;
	TempAVSData.fRMS = MaxRMS;
	TempAVSData.CurNodeID = nTag;
	TempAVSData.CurNodeOffset = nOffset;
	TempAVSData.SteerFront = nSteerF;
	TempAVSData.SteerRear = nSteerR;
	TempAVSData.VelFrontMotor = VelFrontMotor;
	TempAVSData.UBGDetectLevel = nUBGDetectLevel;
	TempAVSData.UBGDetectRegion = nUBGDetectRegion;
	TempAVSData.OBSDetectLevel = nOBSDetectLevel;
	TempAVSData.OBSDetectRegion = nOBSDetectRegion;
	TempAVSData.SpeedMode = nSpeedMode;


	//진동그래프
	if(SeriesVibRMS->Count()+1 >= MAX_VIBRATION_CNT) {
		SeriesVibRMS->Delete(0);
		SeriesVibX->Delete(0);
		SeriesVibY->Delete(0);
		SeriesVibZ->Delete(0);
		AvsDataBuffer.pop_front();
	}

	dVibTime_sec += BOTTOMAXIS_RESOLUTION_SEC_VIB * 50;
	SeriesVibX->AddXY(dVibTime_sec,MaxX,"",clBlack);
	SeriesVibY->AddXY(dVibTime_sec,MaxY,"",clBlack);
	SeriesVibZ->AddXY(dVibTime_sec,MaxZ,"",clBlack);
	SeriesVibRMS->AddXY(dVibTime_sec,MaxRMS,"",clBlack);
	AvsDataBuffer.push_back(TempAVSData);

	if(dVibTime_sec > BOTTOMAXIS_SEC_MAX)
	{
		chartDrivingMotorSpeed->BottomAxis->Minimum = 0;
		chartDrivingMotorSpeed->BottomAxis->Maximum = dVibTime_sec;
		chartDrivingMotorSpeed->BottomAxis->Minimum = dVibTime_sec-BOTTOMAXIS_SEC_MAX;

		chartVibRms->BottomAxis->Minimum = 0;
		chartVibRms->BottomAxis->Maximum = chartDrivingMotorSpeed->BottomAxis->Maximum;
		chartVibRms->BottomAxis->Minimum = chartDrivingMotorSpeed->BottomAxis->Minimum;
	}

	//속도 그래프
	if(SeriesFrontMotor->Count()+1 >= MAX_VELOCITY_CNT)
	{
		SeriesFrontMotor->Delete(0);
	}
    dVelTime_sec += BOTTOMAXIS_RESOLUTION_SEC_VEL;
	SeriesFrontMotor->AddXY(dVelTime_sec,VelFrontMotor,String(nTag),clBlack);

	//Threshold 값 이상, Spec Out 리스트에 저장
	if(dVibFilterThreshold <= MaxRMS){
		if((nPreOffset == nOffset) /*&& (nPreTag == nTag)*/
		&&(SeriesSpecOutPoint->Count() != 0))
		{
			if(preVibRms < MaxRMS)
			{
				AnsiString strLog;
				int nCnt = SeriesSpecOutPoint->Count();
				double dValue = SeriesSpecOutPoint->XValue[SeriesSpecOutPoint->Count()-1];
				SeriesSpecOutPoint->XValue[SeriesSpecOutPoint->Count()-1] = dVibTime_sec;
			}
		}
		else
		{
			if(ListBoxSpecOutList->Count + 1 >= SPEC_OUT_LIST_MAX_COUNT) {
				ListBoxSpecOutList->Items->Delete(0);
				SeriesSpecOutPoint->Delete(0);
				AvsSpecOutBuffer.pop_front();
			}
			strData.sprintf(L"Node: %d, Offset: %d(mm), 진동: %.02f(G)", nTag,nOffset,MaxRMS);
			nPreOffset = nOffset;
			preVibRms = MaxRMS;
			ListBoxSpecOutList->Items->Add(strData);
			m_nMaxCnt++;
			SeriesSpecOutPoint->AddXY(dVibTime_sec, MaxRMS + 0.05,"",clBlack);
			AvsSpecOutBuffer.push_back(TempAVSData);
			ListBoxSpecOutList->ItemIndex = ListBoxSpecOutList->Items->Count - 1;
		}

		if(MaxRMS >= m_dVibMax)
			m_dVibMax = MaxRMS;
	}

	MaxX = 0;
	MaxY = 0;
	MaxZ = 0;
	MaxRMS = 0;

	//온도, 배터리 표시
	AnsiString strTmp;

	//1. 온도
	double fTemperature = (double)VibrationInfo.temperature/10.0;

	strTmp.sprintf("%.1f", fTemperature);
	lbTemperature->Caption = strTmp;
	cgTemperature->Progress = fTemperature;

	if(VibrationInfo.temperature > 400)
		cgTemperature->ForeColor = clRed;
	else if(VibrationInfo.temperature > 300)
		cgTemperature->ForeColor = clYellow;
	else
		cgTemperature->ForeColor = clLime;

	//2. 배터리
	int nBat = 0;
	const static double _maxVoltage = 4130;
	const static double _minVoltage = 3000;

	if(VibrationInfo.battery > (int)_maxVoltage)
		nBat = 100;
	else if(VibrationInfo.battery < (int)_minVoltage)
		nBat = 0;
	else
	{
		nBat = (int)(100* (double)(VibrationInfo.battery - _minVoltage) / (double)(_maxVoltage - _minVoltage));
	}

	strTmp.sprintf("%d", nBat);
	lbBattery->Caption = strTmp;
	cgBattery->Progress = nBat;

	if(nBat < 10)
		cgBattery->ForeColor = clRed;
	else if(nBat < 30)
		cgBattery->ForeColor = clYellow;
	else
		cgBattery->ForeColor = clLime;


	dMinVibTime_sec = SeriesVibRMS->XValue[0];

	if(SeriesSpecOutPoint->Count() > 0){
		while(dMinVibTime_sec > SeriesSpecOutPoint->XValue[0])
		{
			AvsSpecOutBuffer.pop_front();
			SeriesSpecOutPoint->Delete(0);
			ListBoxSpecOutList->Items->Delete(0);

			if(SeriesSpecOutPoint->Count() == 0) break;
		}
	}

	//ScrollBar 표시
	if(dVibTime_sec >= BOTTOMAXIS_SEC_MAX+BOTTOMAXIS_RESOLUTION_SEC_VIB)
	{
		ScrollBarVibTime->Max = SeriesFrontMotor->Count() - (int)(BOTTOMAXIS_SEC_MAX/BOTTOMAXIS_RESOLUTION_SEC_VEL) + 1;
	}
	ScrollBarVibTime->Position = ScrollBarVibTime->Max;

	//Node, 조향, Offset값 표시하기
	edtCurrentNode->Text = VibrationInfo.CurNodeID;
	edtOffsetDistance->Text = VibrationInfo.CurNodeOffset;
	edtSteerFront->Text = txtSteerStatus[nSteerF];
	edtSteerRear->Text = txtSteerStatus[nSteerR];
	edtVhlVel->Text = VibrationInfo.VelFrontMotor;
	edtTransVel->Text = VibrationInfo.VelTransMotor;

	//UBG,OBS Sensor Input
	pnlOhtLong->Color = bUBGLong? clLime : clWhite;
	pnlOhtMiddle->Color = bUBGMiddle ? clLime : clWhite;
	pnlOhtShort->Color = bUBGShort ? clLime : clWhite;
	pnlOhtFail->Color = bUBGFail ? clRed : clWhite;

	pnlObsLong->Color = bOBSLong ? clLime : clWhite;
	pnlObsMiddle->Color = bOBSMiddle ? clLime : clWhite;
	pnlObsShort->Color = bOBSShort ? clLime : clWhite;
	pnlObsFail->Color = bOBSFail ? clRed : clWhite;

	//주행상황 표시하기
	edtSpeedMode->Text = txtSpeedMode[nSpeedMode];

	//UBG,OBS Region표시
	edtUBGRegion->Text =  txtUBGSensorArea[nUBGDetectRegion];
	edtOBSRegion->Text =  txtOBSSensorArea[nOBSDetectRegion];
	strData.sprintf(L"%.2f",m_dVibMax);
	edtMax->Text = strData;
	edtCnt->Text = m_nMaxCnt;

}

//---------------------------------------------------------------------------


void __fastcall TfrmVibrationMeasurement::btnThresholdClick(TObject *Sender)
{
	double inputThreshold;

	try{
		inputThreshold = edtThresholdInput->Text.ToDouble();
		if(inputThreshold < 0.0 || inputThreshold > 2.0){
			 Application->MessageBox(L"0.0G~2.0G 사이 값을 넣어주세요", L"Error", MB_OK);
		}
		else
		{
			dVibFilterThreshold = inputThreshold;
			edtThresholdOutput->Text = inputThreshold;
		}
	}
	catch(exception e)
	{
		Application->MessageBox(L"값을 다시 확인해주세요 (0.0~2.0)", L"Error", MB_OK);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::btnSpecOutListResetClick(TObject *Sender)

{
	ListBoxSpecOutList->Clear();
	SeriesSpecOutPoint->Clear();
	AvsSpecOutBuffer.clear();
}
//---------------------------------------------------------------------------



void __fastcall TfrmVibrationMeasurement::ListBoxSpecOutListDblClick(TObject *Sender)

{
	int indexSelectedItem =  ListBoxSpecOutList->ItemIndex;
	double time;
	double bottom_axis_min;
	double bottom_axis_max;
	int pos;
	String strData;
	double x;
	int ValueIndex;

	time = SeriesSpecOutPoint->XValue[indexSelectedItem];

	bottom_axis_min = time - BOTTOMAXIS_SEC_MAX / 2;
	if(bottom_axis_min < 0.0) bottom_axis_min = 0.0;
	else if (bottom_axis_min < dMinVibTime_sec) {
		bottom_axis_min = dMinVibTime_sec;
	}
	bottom_axis_max = bottom_axis_min + BOTTOMAXIS_SEC_MAX;
	if(bottom_axis_max > dVelTime_sec)
	{
		bottom_axis_max = dVibTime_sec;
		bottom_axis_min = dVelTime_sec - BOTTOMAXIS_SEC_MAX;
    }

	chartVibRms->BottomAxis->Minimum = 0;
	chartDrivingMotorSpeed->BottomAxis->Minimum = 0;

	chartVibRms->BottomAxis->Maximum = bottom_axis_max;
	chartDrivingMotorSpeed->BottomAxis->Maximum = bottom_axis_max;
	chartVibRms->BottomAxis->Minimum = bottom_axis_min;
	chartDrivingMotorSpeed->BottomAxis->Minimum = bottom_axis_min;

	pos = (bottom_axis_min - dMinVibTime_sec)/BOTTOMAXIS_RESOLUTION_SEC_VEL;
	if(pos > ScrollBarVibTime->Max) pos = ScrollBarVibTime->Max;

	ScrollBarVibTime->Position = pos;

	x = SeriesSpecOutPoint->XValue[indexSelectedItem];
	list<AVS_Data>::iterator iterAvsSpecOutBuffer;

	iterAvsSpecOutBuffer = AvsSpecOutBuffer.begin();

	for(int i = 0; i< indexSelectedItem; i++)
	{
		iterAvsSpecOutBuffer++;
	}

	for(int i = 0; i < SeriesVibRMS->Count(); i++)
	{
		if(x == SeriesVibRMS->XValue[i]){
			ValueIndex = i;
			break;
		}
	}
	strData.sprintf(L"%.02f", iterAvsSpecOutBuffer->fX);
	edtVibX->Text = strData;
	strData.sprintf(L"%.02f", iterAvsSpecOutBuffer->fY);
	edtVibY->Text = strData;
	strData.sprintf(L"%.02f", iterAvsSpecOutBuffer->fZ);
	edtVibZ->Text = strData;
	strData.sprintf(L"%.02f", iterAvsSpecOutBuffer->fRMS);
	edtVibRMS->Text = strData;

	GetUBGLVtoSensor(iterAvsSpecOutBuffer->UBGDetectLevel);
	GetUBGRegionToSensor(iterAvsSpecOutBuffer->UBGDetectRegion);
	GetOBSLVtoSensor(iterAvsSpecOutBuffer->OBSDetectLevel);
	GetOBSRegionToSensor(iterAvsSpecOutBuffer->OBSDetectRegion);

	edtCurrentNode->Text = iterAvsSpecOutBuffer->CurNodeID;
	edtOffsetDistance->Text = iterAvsSpecOutBuffer->CurNodeOffset;
	strData.sprintf(L"%.02f", iterAvsSpecOutBuffer->VelFrontMotor);
	edtVhlVel->Text = strData;
	edtSteerFront->Text = txtSteerStatus[iterAvsSpecOutBuffer->SteerFront];
	edtSteerRear->Text = txtSteerStatus[iterAvsSpecOutBuffer->SteerRear];
	edtSpeedMode->Text = txtSpeedMode[iterAvsSpecOutBuffer->SpeedMode];

	if((ValueIndex < 0) || (ValueIndex >= SeriesVibRMS->Count())) return;


	SeriesVibPos->Clear();
	SeriesMotorPos->Clear();
	SeriesMotorPos->AddXY(SeriesVibRMS->XValue[ValueIndex],chartDrivingMotorSpeed->LeftAxis->Maximum,"",clRed);
	SeriesMotorPos->AddXY(SeriesVibRMS->XValue[ValueIndex],chartDrivingMotorSpeed->LeftAxis->Minimum,"",clRed);
	SeriesVibPos->AddXY(SeriesVibRMS->XValue[ValueIndex],chartVibRms->LeftAxis->Maximum,"",clRed);
	SeriesVibPos->AddXY(SeriesVibRMS->XValue[ValueIndex],chartVibRms->LeftAxis->Minimum,"",clRed);

	if(FFTViewerForm->Showing)
	{
		SeriesVibPosRange->Clear();
		SeriesMotorPosRange->Clear();
		SeriesMotorPosRange->AddXY(SeriesVibRMS->XValue[ValueIndex]+1,chartDrivingMotorSpeed->LeftAxis->Maximum,"",clRed);
		SeriesMotorPosRange->AddXY(SeriesVibRMS->XValue[ValueIndex]+1,chartDrivingMotorSpeed->LeftAxis->Minimum,"",clRed);
		SeriesVibPosRange->AddXY(SeriesVibRMS->XValue[ValueIndex]+1,chartVibRms->LeftAxis->Maximum,"",clRed);
		SeriesVibPosRange->AddXY(SeriesVibRMS->XValue[ValueIndex]+1,chartVibRms->LeftAxis->Minimum,"",clRed);
	}

	bVibMeasDrawLine = true;

}



//---------------------------------------------------------------------------


void __fastcall TfrmVibrationMeasurement::FormClose(TObject *Sender, TCloseAction &Action)

{
	if(nSensorConnectionStep != VIB_SENSOR_DISCONNECTED)
	{
		bool bRealTimeLogSave;
		btnChangeSaveFilePath->Enabled = bRealTimeLogSave;
		edtSaveFilePath->Enabled = bRealTimeLogSave;
		btnVibMeasureStart->Enabled = true;
		if(bRealTimeLogSave)
		{
			fflush(fp);
			fclose(fp);
		}

		btnSensorConnection->Caption = "Connect";
		LabelStatus->Caption = "Status: Disconnected to sensor" ;
		nSensorConnectionStep = VIB_SENSOR_DISCONNECTED;

		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_VIB_DATA_CLOSE);
		delete m_CMDControl;

		MainForm->tmrUdpListen->Interval = mBackUpUdpListenerInterval;
	}

	tmrSendAlive->Enabled = false;
}
//---------------------------------------------------------------------------



void __fastcall TfrmVibrationMeasurement::cbRealTimeLogClick(TObject *Sender)
{
	bool bRealTimeLogSave;
	btnChangeSaveFilePath->Enabled = bRealTimeLogSave;
	edtSaveFilePath->Enabled = bRealTimeLogSave;
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::btnChangeSaveFilePathClick(TObject *Sender)

{
	String strPath = "";
	strPath = ExtractFileDir(edtSaveFilePath->Text);
	SaveDialog->Title = "Select Directory and File Name";
	SaveDialog->FileName = ExtractFileName(edtSaveFilePath->Text);
	SaveDialog->InitialDir = strPath;
	SaveDialog->Filter = "Vibration Log(*.csv)|+;*.csv";
	//edtSaveFilePath->Text = strOHTPath + "\\VIB";
	//mStrLogFileName = edtSaveFilePath->Text;
	if(SaveDialog->Execute()){
		mStrLogFileName = SaveDialog->FileName;
	}
	edtSaveFilePath->Text = mStrLogFileName;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::btnInitDataClick(TObject *Sender)
{
	MeasurementInitialization();
}
//---------------------------------------------------------------------------
void __fastcall TfrmVibrationMeasurement::FormShow(TObject *Sender)
{
	btnVibMeasureStart->Enabled = false;
	btnVibMeasureStop->Enabled = false;
	btnSensorConnection->Enabled = true;
}
//---------------------------------------------------------------------------

#include <fstream>

void __fastcall TfrmVibrationMeasurement::btFileOpenClick(TObject *Sender)
{
//	if(bIsDriveTrans)
//		return;

	m_dVibMax = 0;
	m_nMaxCnt = 0;
	MeasurementInitialization();
	cgGauge->MaxValue = 0;
	cgGauge->Progress = 0;
	m_bStopDrawChart = false;

	bool m_bStopFlag  = false;

	if(OpenDialog->Execute())
	{
		AVS_VIBRATION_DATA_INFO drawData;
		const int COL_COUNT = 10;
		char buf[256] = {0,};
		float dRMS = 0.0f, dVel = 0.0f, dX = 0.0f, dY = 0.0f, dZ = 0.0f;
		int nTimeOffset, nNodeID, nOffset = 0;
		int retValue = 0;
		char steerFL = 0, steerRL = 0;
		int UBGLV = 0, UBGRegion = 0;
		int OBSLV = 0, OBSRegion = 0;
		int SPEEDMODE = 0;


		//파일 열기
		if(fs != NULL)
			fclose(fs);

		fs = _wfopen(OpenDialog->FileName.c_str(), L"r");

		if(fs != NULL)
		{
			//데이터 갯수 구함
			int line = 0;

			while(fscanf(fs, "%s", buf) != EOF)
			{
				line++;
			}

			fseek(fs, 0, SEEK_SET);

			//첫줄은 버림
			retValue = fscanf(fs, "%s", buf);
			line--;

			//Progress bar 셋팅
			cgGauge->MaxValue = line;

			//버튼 활성화 변경
			btFileOpen->Enabled = false;
			btStopDraw->Enabled = true;
			btInitData->Enabled = false;
			btnRestart->Enabled = false;

			//두번째 줄부터 읽음 %d,%06d,%d,%.02f,%c,%c,%.02f,%.02f,%.02f,%.02f
			while(retValue != EOF)
			{
				//Stop 버튼 눌렀을 때 탈출
				if(m_bStopDrawChart == true)
				{
					btnRestart->Enabled = true;
					m_bStopDrawChart = false;
					m_bStopFlag = true;
					break;
				}

				//설정한 샘플갯수만큼 그림
				for(int sample = 0; sample < NUM_OF_SAMPLE; sample++)
				{
					char buf[256] = {0,};
					retValue = fscanf(fs, "%s", buf);

					for(int i=0; buf[i] != 0; i++)
					{
						if(buf[i] == ',') buf[i] = ' ';
					}

					//n은 디버그용도
					int n = sscanf(buf, "%d %d %d %f %c %c %d %d %d %d %f %f %f %f %d",
						&nTimeOffset,
						&nNodeID,
						&nOffset,
						&dVel,
						&steerFL,
						&steerRL,
						&UBGLV,
						&UBGRegion,
						&OBSLV,
						&OBSRegion,
						&dRMS,
						&dX,
						&dY,
						&dZ,
						&SPEEDMODE);


					drawData.CurNodeID = nNodeID;
					drawData.CurNodeOffset = nOffset;
					drawData.VelFrontMotor = dVel;
					drawData.VelTransMotor = 0; //임시

					drawData.fX[sample] = dX;
					drawData.fY[sample] = dY;
					drawData.fZ[sample] = dZ;


					if(steerFL == 'R')
					{
						drawData.SteerFrontR = 1;
						drawData.SteerFrontL = 0;
					}
					else if(steerFL == 'L')
					{
						drawData.SteerFrontL = 1;
						drawData.SteerFrontR = 0;
					}
					else
					{
						drawData.SteerFrontL = drawData.SteerFrontR = 0;
					}

					if(steerRL == 'R')
					{
						drawData.SteerRearL = 0;
						drawData.SteerRearR = 1;
					}
					else if(steerRL == 'L')
					{
						drawData.SteerRearL = 1;
						drawData.SteerRearR = 0;
					}
					else
						drawData.SteerRearL = drawData.SteerRearR = 0;

					cgGauge->AddProgress(1);

					AnsiString strLoadCounter;
					strLoadCounter.sprintf("%d/%d", cgGauge->Progress, line);
					lbLoadCounter->Caption = strLoadCounter;
				}


				GetUBGLVtoSensor(UBGLV, &drawData);
				GetUBGRegionToSensor(UBGRegion, &drawData);
				GetOBSLVtoSensor(OBSLV, &drawData);
				GetOBSRegionToSensor(OBSRegion, &drawData);


				drawData.SpeedMode = SPEEDMODE;
				//차트 그리기
				DisplayVibrationData(drawData,true);
				Application->ProcessMessages();
			}

			//버튼 활성화 변경
			if(m_bStopFlag == false)
			{
				btStopDraw->Enabled = false;
				btFileOpen->Enabled = true;
				btInitData->Enabled = true;
				btnRestart->Enabled = true;
			}
		}

		if(m_bStopFlag == false)
			fclose(fs);
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmVibrationMeasurement::btInitDataClick(TObject *Sender)
{
	MeasurementInitialization();

	cgGauge->MaxValue = 0;
	cgGauge->Progress = 0;
	lbLoadCounter->Caption = "0 / 0";
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::btStopDrawClick(TObject *Sender)
{
	m_bStopDrawChart = true;
	btnRestart->Enabled = true;
	btFileOpen->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::btExecuteFFTClick(TObject *Sender)
{
	FFTViewerForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::edtSaveFilePathMouseEnter(TObject *Sender)

{
	edtSaveFilePath->Hint = edtSaveFilePath->Text;
	edtSaveFilePath->ShowHint = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::edtSaveFilePathMouseLeave(TObject *Sender)

{
    edtSaveFilePath->ShowHint = false;
}
//---------------------------------------------------------------------------


void __fastcall TfrmVibrationMeasurement::btLogSaveStartClick(TObject *Sender)
{
	m_dVibMax = 0;
	m_nMaxCnt = 0;
	if(bIsFileOpen == false)
	{
		try
		{
			mStrLogFileName = AnsiString(GetLogFileName(edtSaveFilePath->Text));
			fp = fopen(mStrLogFileName.c_str(), "a");
			if(fp == NULL) bIsFileOpen = false;
			else
			{
				cbbSavingTime->Enabled = false;
				btnChangeSaveFilePath->Enabled = false;
				edtSaveFilePath->Enabled = false;
				tmrUIUpdate->Enabled = true;

				//chkDriveTrans->Enabled = false;
				bIsFileOpen = true;
				m_nSaveDataNum = 0;
//				if(!bIsDriveTrans)
					fprintf(fp, "DataCount,TagID,Offset(mm),Vel(mm/s),Steer(F),Steer(R),UbgLV,UbgRegion,ObsLV,ObsRegion,RMS,X,Y,Z,SPEEDMODE\n");
//				else
//					fprintf(fp, "DataCount,TagID,TransType,TransStep,Position,Velocity,HandPushSen,HandOpen,HandClose,GripLeft,GripRight,Inposition,HomeSen,rms,x,y,z\n");

			}

		}catch(...)
		{
			if(fp != NULL)
				fclose(fp);
		}
	}
	else
	{
		bIsFileOpen = false;
		cbbSavingTime->Enabled = true;
		btnChangeSaveFilePath->Enabled = true;
		edtSaveFilePath->Enabled = true;
		btLogSaveStart->Caption = "시작";
		tmrUIUpdate->Enabled = false;
		//chkDriveTrans->Enabled = true;
		fflush(fp);
		fclose(fp);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::tmrUIUpdateTimer(TObject *Sender)
{
	if(bIsFileOpen == true)
	{
		AnsiString str;
		int t = (nSavingTime[cbbSavingTime->ItemIndex]*1000 - m_nSaveDataNum) / 1000;
		str.sprintf("잔여:%d sec", t);

		btLogSaveStart->Caption = str;
	}
	else
	{
		btLogSaveStart->Caption = "시작";
	}

	//Connect 상태 업데이트
	if(LabelStatus->Caption == "Status: Disconnected to sensor")
	{
		LabelStatus->Transparent = false;
		LabelStatus->Color = clRed;
	}
	else if(LabelStatus->Caption == "Status: Connected to sensor")
	{
		LabelStatus->Transparent = false;
		LabelStatus->Color = clLime;
	}
	else
	{
		LabelStatus->Transparent = true;
		LabelStatus->Color = clBtnFace;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::tmrSendAliveTimer(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_VIB_DATA_ALIVE);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------


int  TfrmVibrationMeasurement::GetUBGSensorLevel(bool bLong, bool bMiddle, bool bShort, bool bFail)
{

	if(!bLong && !bMiddle && !bShort)
		return 8;


	if(bLong && !bMiddle && !bShort)
        return 7;


	if(bLong && bMiddle && !bShort)
        return 6;


	if(!bLong && bMiddle && !bShort)
        return 5;


	if(!bLong && bMiddle && bShort)
        return 4;


	if(bLong && bMiddle && bShort)
		return 3;


	if(bLong && !bMiddle && bShort)
		return 1;

	if(bFail)
		return 0;

}

int  TfrmVibrationMeasurement::GetUBGSensorRegion(bool Sel1, bool Sel2, bool Sel3, bool Sel4)
{

	int nValue  =  0;

	nValue =  		  1 * Sel1
				 +    2 * Sel2
				 +    4 * Sel3
				 +	  8 * Sel4;

	edtUBGRegion->Text =  nValue;

	return nValue;
}


int  TfrmVibrationMeasurement::GetOBSSensorLevel(bool bLong, bool bMiddle, bool bShort, bool bFail)
{


	if(!bLong && !bMiddle && !bShort)
		return 8;

	if(bLong && !bMiddle && !bShort)
		return 7;

	if(bLong && bMiddle && !bShort)
		return 6;

	if(bLong && bMiddle && bShort)
		return 3;



    return 0;


}


int  TfrmVibrationMeasurement::GetOBSSensorRegion(bool Sel1, bool Sel2, bool Sel3, bool Sel4)
{

	int nValue = 0;

		nValue =	  1 * Sel1
				 +    2 * Sel2
				 +    4 * Sel3
				 +	  8 * Sel4;

	edtOBSRegion->Text = nValue;

	return nValue;

}


int TfrmVibrationMeasurement::GetUBGLVtoSensor(int LV, AVS_VIBRATION_DATA_INFO* VibrationInfo)
{
	bool bUBGInputlong = 0;
	bool bUBGInputMiddle = 0;
	bool bUBGInputShort = 0;
	bool bUBGInputFail = 0;

	switch(LV)
	{
	case 8:
		bUBGInputlong = 0;
		bUBGInputMiddle = 0;
		bUBGInputShort = 0;
	break;
	case 7:
		bUBGInputlong = 1;
		bUBGInputMiddle = 0;
		bUBGInputShort = 0;
	break;
	case 6:
		bUBGInputlong = 1;
		bUBGInputMiddle = 1;
		bUBGInputShort = 0;
	break;
	case 5:
		bUBGInputlong = 0;
		bUBGInputMiddle = 1;
		bUBGInputShort = 0;
	break;
	case 4:
		bUBGInputlong = 0;
		bUBGInputMiddle = 1;
		bUBGInputShort = 1;
	break;
	case 3:
		bUBGInputlong = 1;
		bUBGInputMiddle = 1;
		bUBGInputShort = 1;
	break;
	case 1:
		bUBGInputlong = 1;
		bUBGInputMiddle = 0;
		bUBGInputShort = 1;
	break;
	case 0:
		bUBGInputFail = 1;
	break;
	default:
	break;
	}

	if(VibrationInfo == NULL)
	{
		pnlOhtLong->Color = bUBGInputlong? clLime : clWhite;
		pnlOhtMiddle->Color = bUBGInputMiddle ? clLime : clWhite;
		pnlOhtShort->Color = bUBGInputShort ? clLime : clWhite;
		pnlOhtFail->Color = bUBGInputFail ? clRed : clWhite;
	}
	else
	{
		VibrationInfo->UBGInputLong = bUBGInputlong;
		VibrationInfo->UBGInputMiddle = bUBGInputMiddle;
		VibrationInfo->UBGInputShort = bUBGInputShort;
		VibrationInfo->UBGInputFail = bUBGInputFail;
	}

}

int TfrmVibrationMeasurement::GetUBGRegionToSensor(int Region, AVS_VIBRATION_DATA_INFO* VibrationInfo)
{

	bool sel1 = (Region & 0x1) ? 1:0;
	bool sel2 = (Region & 0x2) ? 1:0;
	bool sel3 = (Region & 0x4) ? 1:0;
	bool sel4 = (Region & 0x8) ? 1:0;


	if(VibrationInfo == NULL)
	{
		edtUBGRegion->Text =  txtUBGSensorArea[Region];
	}
	else
	{
		VibrationInfo->UBGOutputSel1 = sel1;
		VibrationInfo->UBGOutputSel2 = sel2;
		VibrationInfo->UBGOutputSel3 = sel3;
		VibrationInfo->UBGOutputSel4 = sel4;
    }

}

int TfrmVibrationMeasurement::GetOBSLVtoSensor(int LV, AVS_VIBRATION_DATA_INFO* VibrationInfo)
{

	bool bOBSInputlong = 0;
	bool bOBSInputMiddle = 0;
	bool bOBSInputShort = 0;
	bool bOBSInputFail = 0;


	switch(LV)
	{
	case 8:
		bOBSInputlong = 0;
		bOBSInputMiddle = 0;
		bOBSInputShort = 0;
	break;
	case 7:
		bOBSInputlong = 1;
		bOBSInputMiddle = 0;
		bOBSInputShort = 0;
	break;
	case 6:
		bOBSInputlong = 1;
		bOBSInputMiddle = 1;
		bOBSInputShort = 0;
	break;
	case 3:
		bOBSInputlong = 1;
		bOBSInputMiddle = 1;
		bOBSInputShort = 1;
	break;
	case 0:
	break;
	default:
		bOBSInputFail = 1;
	break;
	}

	if(VibrationInfo == NULL)
	{
		pnlObsLong->Color =  bOBSInputlong ? clLime : clWhite;
		pnlObsMiddle->Color =  bOBSInputMiddle ? clLime : clWhite;
		pnlObsShort->Color =  bOBSInputShort ? clLime : clWhite;
		pnlObsFail->Color = bOBSInputFail ? clRed : clWhite;
	}
	else
	{
		VibrationInfo->OBSInputLong = bOBSInputlong;
		VibrationInfo->OBSInputMiddle = bOBSInputMiddle;
		VibrationInfo->OBSInputShort = bOBSInputShort;
		VibrationInfo->OBSInputFail = bOBSInputFail;
	}

}

int TfrmVibrationMeasurement::GetOBSRegionToSensor(int Region, AVS_VIBRATION_DATA_INFO* VibrationInfo)
{
	bool sel1 = (Region & 0x1) ? 1:0;
	bool sel2 = (Region & 0x2) ? 1:0;
	bool sel3 = (Region & 0x4) ? 1:0;
	bool sel4 = (Region & 0x8) ? 1:0;

	if(VibrationInfo == NULL)
	{
			edtOBSRegion->Text =  txtOBSSensorArea[Region];
	}
	else
	{
		VibrationInfo->OBSOutputSel1 = (Region & 0x1) ? 1:0;
		VibrationInfo->OBSOutputSel2 = (Region & 0x2) ? 1:0;
		VibrationInfo->OBSOutputSel3 = (Region & 0x4) ? 1:0;
		VibrationInfo->OBSOutputSel4 = (Region & 0x8) ? 1:0;
    }


}

void __fastcall TfrmVibrationMeasurement::btnRestartClick(TObject *Sender)
{
//		if(bIsDriveTrans)
//			return;

		AVS_VIBRATION_DATA_INFO drawData;
		const int COL_COUNT = 10;
		char buf[256] = {0,};
		float dRMS = 0.0f, dVel = 0.0f, dX = 0.0f, dY = 0.0f, dZ = 0.0f;
		int nTimeOffset, nNodeID, nOffset = 0;
		int retValue = 0;
		char steerFL = 0, steerRL = 0;
		int UBGLV = 0, UBGRegion = 0;
		int OBSLV = 0, OBSRegion = 0;
		int SPEEDMODE = 0;
		bool m_bStopFlag = false;

		btnRestart->Enabled = false;

		if(fs != NULL)
		{

			//버튼 활성화 변경
			btFileOpen->Enabled = false;
			btStopDraw->Enabled = true;
			btInitData->Enabled = false;

			//두번째 줄부터 읽음 %d,%06d,%d,%.02f,%c,%c,%.02f,%.02f,%.02f,%.02f
			while(retValue != EOF)
			{
				//Stop 버튼 눌렀을 때 탈출
				if(m_bStopDrawChart == true)
				{
					m_bStopDrawChart = false;
					m_bStopFlag = true;
					break;
				}

				//설정한 샘플갯수만큼 그림
				for(int sample = 0; sample < NUM_OF_SAMPLE; sample++)
				{
					char buf[256] = {0,};
					retValue = fscanf(fs, "%s", buf);

					for(int i=0; buf[i] != 0; i++)
					{
						if(buf[i] == ',') buf[i] = ' ';
					}

					//n은 디버그용도
					int n = sscanf(buf, "%d %d %d %f %c %c %d %d %d %d %f %f %f %f %d",
						&nTimeOffset,
						&nNodeID,
						&nOffset,
						&dVel,
						&steerFL,
						&steerRL,
						&UBGLV,
						&UBGRegion,
						&OBSLV,
						&OBSRegion,
						&dRMS,
						&dX,
						&dY,
						&dZ,
						&SPEEDMODE);


					drawData.CurNodeID = nNodeID;
					drawData.CurNodeOffset = nOffset;
					drawData.VelFrontMotor = dVel;

					drawData.fX[sample] = dX;
					drawData.fY[sample] = dY;
					drawData.fZ[sample] = dZ;

					if(steerFL == 'R')
					{
						drawData.SteerFrontR = 1;
						drawData.SteerFrontL = 0;
					}
					else if(steerFL == 'L')
					{
						drawData.SteerFrontL = 1;
						drawData.SteerFrontR = 0;
					}
					else
					{
						drawData.SteerFrontL = drawData.SteerFrontR = 0;
					}

					if(steerRL == 'R')
					{
						drawData.SteerRearL = 0;
						drawData.SteerRearR = 1;
					}
					else if(steerRL == 'L')
					{
						drawData.SteerRearL = 1;
						drawData.SteerRearR = 0;
					}
					else
						drawData.SteerRearL = drawData.SteerRearR = 0;

					cgGauge->AddProgress(1);

					AnsiString strLoadCounter;
					strLoadCounter.sprintf("%d/%d", cgGauge->Progress, cgGauge->MaxValue);
					lbLoadCounter->Caption = strLoadCounter;
				}


				GetUBGLVtoSensor(UBGLV, &drawData);
				GetUBGRegionToSensor(UBGRegion,&drawData);
				GetOBSLVtoSensor(OBSLV, &drawData);
				GetOBSRegionToSensor(OBSRegion, &drawData);

				drawData.SpeedMode = (BYTE)SPEEDMODE;
				//차트 그리기
				DisplayVibrationData(drawData,true);
				Application->ProcessMessages();
			}

			//버튼 활성화 변경
			if(m_bStopFlag == false)
			{
				btStopDraw->Enabled = false;
				btFileOpen->Enabled = true;
				btInitData->Enabled = true;
				btnRestart->Enabled = true;
			}


		}

		if(m_bStopFlag == false)
				fclose(fs);

}



//---------------------------------------------------------------------------



