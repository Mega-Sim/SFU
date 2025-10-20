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
char chSteerStatus[3] = {'M','L','R'};
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
   //	for(int i=0; i < chbMotorGraph->Items->Count; i++) chbMotorGraph->Checked[i] = true;
   // for(int i=0; i < chbVibGraph->Items->Count; i++) chbVibGraph->Checked[i] = true;
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
	SeriesMotorPos->Clear();

	CurNodeIdList.clear();
	CurNodeOffset.clear();
	SteerFront.clear();
	SteerRear.clear();


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

		bool bRealTimeLogSave;
		bRealTimeLogSave = cbRealTimeLog->Checked;
		btnChangeSaveFilePath->Enabled = bRealTimeLogSave;
		edtSaveFilePath->Enabled = bRealTimeLogSave;
		cbRealTimeLog->Enabled = true;
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
		LabelStatus->Caption = "Status: Disconnected to sensor" ;
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
	bRealTimeLogSave = cbRealTimeLog->Checked;
	btnChangeSaveFilePath->Enabled = false;
	edtSaveFilePath->Enabled = false;
	cbRealTimeLog->Enabled = false;
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


	if(bRealTimeLogSave)
	{
    	try
		{
			mStrLogFileName = AnsiString(GetLogFileName(edtSaveFilePath->Text));
			fp = fopen(mStrLogFileName.c_str(), "a");
			if(fp == NULL) bIsFileOpen = false;
			else
			{
				bIsFileOpen = true;
//				fprintf(fp, "TimeOffset bitween Vel data and Vib data: %.3f(s)", m_fTimeOffset);
				fprintf(fp, "DataCount,TagID,Offset(mm),Vel(mm/s),Steer(F),Steer(R),RMS,X,Y,Z\n");
				gDataCnt = 0;

			}

		}catch(...)
		{
			if(fp != NULL)
				fclose(fp);
		}
	}

	MeasurementInitialization();

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_VIB_DATA_REQ_START);
	mBackUpUdpListenerInterval = MainForm->tmrUdpListen->Interval;
	MainForm->tmrUdpListen->Interval = NUM_OF_VIB_SAMPLE / 2;
	delete m_CMDControl;

	LabelStatus->Caption = "Status: Started receiving" ;

	btnSensorConnection->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::btnVibMeasureStopClick(TObject *Sender)

{
	if(nSensorConnectionStep != VIB_SENSOR_CONNECTED) return;

	bool bRealTimeLogSave;
	bRealTimeLogSave = cbRealTimeLog->Checked;
	btnChangeSaveFilePath->Enabled = bRealTimeLogSave;
	edtSaveFilePath->Enabled = bRealTimeLogSave;
	cbRealTimeLog->Enabled = true;
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
}

//---------------------------------------------------------------------------
void TfrmVibrationMeasurement::DrawCheckLineDisplayInfo(int ValueIndex, double X)
{
	unsigned int NodeId;
	int NodeOffset;
	double VelFrontMotor;// = SeriesMotorPos->YValue[ValueIndex];
	BYTE SteerF;
	BYTE SteerR;

	String strData;
	
	list<unsigned int>::iterator iterPosNodeId;
	list<int>::iterator iterNodeOffset;
	list<BYTE>::iterator iterSteerFront;
	list<BYTE>::iterator iterSteerRear;
	iterPosNodeId = CurNodeIdList.begin();
	iterNodeOffset = CurNodeOffset.begin();
	iterSteerFront = SteerFront.begin();
	iterSteerRear = SteerRear.begin();
	for(int i = 0; i < ValueIndex; i++) {
		iterPosNodeId++;
		iterNodeOffset++;
		iterSteerFront++;
		iterSteerRear++;
	}
	NodeId = *iterPosNodeId;
	NodeOffset = *iterNodeOffset;//(CurNodeOffset.begin() + ValueIndex);
	SteerR = *iterSteerRear;
	SteerF = *iterSteerFront;
	VelFrontMotor = SeriesFrontMotor->YValue[ValueIndex];

	edtCurrentNode->Text = NodeId;
	edtOffsetDistance->Text = NodeOffset;
	strData.sprintf(L"%.02f", VelFrontMotor);
	edtVhlVel->Text = strData;
	edtSteerFront->Text = txtSteerStatus[SteerF];
	edtSteerRear->Text = txtSteerStatus[SteerR];

	SeriesVibPos->Clear();
	SeriesMotorPos->Clear();
    //SeriesSpecOutPoint->Clear();
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
	DrawCheckLineDisplayInfo(ValueIndex/NUM_OF_SAMPLE - m_fTimeOffset / BOTTOMAXIS_RESOLUTION_SEC_VEL, SeriesVibRMS->XValue[ValueIndex]);

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
void TfrmVibrationMeasurement::DisplayVibrationData(AVS_VIBRATION_DATA_INFO VibrationInfo)
{
	int dwTimeStart = timeGetTime();

	float vibX;
	float vibY;
	float vibZ;
	float vibRms;
	double VelFrontMotor;
	int nOffset;
	static int nPreOffset = 0;
	UINT nTag;
	//static UINT nPreTag = 0;
	float preVibRms=0;

	WORD nSteerF;
	WORD nSteerR;

	String strData;

	static int tempIndex_vib = 0;
	static int tempIndex_vel = 0;

	//그래프 출력
	VelFrontMotor = VibrationInfo.VelFrontMotor;
	nOffset = VibrationInfo.CurNodeOffset;
	nTag = VibrationInfo.CurNodeID;

	if(VibrationInfo.SteerFrontL == 1) nSteerF = 1;
	else if(VibrationInfo.SteerFrontR == 1) nSteerF = 2;
	else nSteerF = 0;

	if(VibrationInfo.SteerRearL == 1) nSteerR = 1;
	else if(VibrationInfo.SteerRearR == 1) nSteerR = 2;
	else nSteerR = 0;

	for(int i = 0; i < NUM_OF_SAMPLE; i++)
	{
		vibX = VibrationInfo.fX[i];
		vibY = VibrationInfo.fY[i];
		vibZ = VibrationInfo.fZ[i]-1;
		vibRms = sqrt((vibX*vibX + vibY*vibY + vibZ*vibZ)/3);

		if(SeriesVibRMS->Count()+1 >= MAX_VIBRATION_CNT) {
			SeriesVibRMS->Delete(0);
			SeriesVibX->Delete(0);
			SeriesVibY->Delete(0);
			SeriesVibZ->Delete(0);
		}

		SeriesVibX->AddXY(dVibTime_sec,vibX,"",clBlack);
		SeriesVibY->AddXY(dVibTime_sec,vibY,"",clBlack);
		SeriesVibZ->AddXY(dVibTime_sec,vibZ,"",clBlack);
		SeriesVibRMS->AddXY(dVibTime_sec,vibRms,"",clBlack);
		//SeriesVibRMS->AddXY(dVibTime_sec,vibRms,String(nTag),clBlack);

		if(dVibFilterThreshold <= vibRms){
			if((nPreOffset == nOffset) /*&& (nPreTag == nTag)*/
			&&(SeriesSpecOutPoint->Count() != 0))
			{
				if(preVibRms < vibRms)
				{
					AnsiString strLog;
					int nCnt = SeriesSpecOutPoint->Count();
					double dValue = SeriesSpecOutPoint->XValue[SeriesSpecOutPoint->Count()-1];
					SeriesSpecOutPoint->XValue[SeriesSpecOutPoint->Count()-1] = dVibTime_sec;
				}
			}
			else{
				if(ListBoxSpecOutList->Count + 1 >= SPEC_OUT_LIST_MAX_COUNT) {
					ListBoxSpecOutList->Items->Delete(0);
					SeriesSpecOutPoint->Delete(0);
				}
				//tableSpecOutList[ListBoxSpecOutList->Count] = SeriesVibRMS->Count() - 1;
				strData.sprintf(L"Node: %d, Offset: %d(mm), 진동: %.02f(G)", nTag,nOffset,vibRms);
				nPreOffset = nOffset;
				//nPreTag = nTag;
				preVibRms = vibRms;
				ListBoxSpecOutList->Items->Add(strData);
				SeriesSpecOutPoint->AddXY(dVibTime_sec,vibRms + 0.05,"",clBlack);
				ListBoxSpecOutList->ItemIndex = ListBoxSpecOutList->Items->Count - 1;
			}
		}

		dVibTime_sec += BOTTOMAXIS_RESOLUTION_SEC_VIB;
		if(dVibTime_sec > BOTTOMAXIS_SEC_MAX)
		{
			chartDrivingMotorSpeed->BottomAxis->Minimum = 0;
			chartDrivingMotorSpeed->BottomAxis->Maximum = dVibTime_sec;
			chartDrivingMotorSpeed->BottomAxis->Minimum = dVibTime_sec-BOTTOMAXIS_SEC_MAX;

			chartVibRms->BottomAxis->Minimum = 0;
			chartVibRms->BottomAxis->Maximum = chartDrivingMotorSpeed->BottomAxis->Maximum;
			chartVibRms->BottomAxis->Minimum = chartDrivingMotorSpeed->BottomAxis->Minimum;

		}
		if(bIsFileOpen)
		{
			fprintf(fp, "%d,%06d,%d,%.02f,%c,%c,%.02f,%.02f,%.02f,%.02f\n", gDataCnt, nTag, nOffset, VelFrontMotor, chSteerStatus[nSteerF], chSteerStatus[nSteerR], vibRms, vibX, vibY, vibZ);
			gDataCnt++;
		}
	}

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

	//
	if(SeriesFrontMotor->Count()+1 >= MAX_VELOCITY_CNT) {
		SeriesFrontMotor->Delete(0);
		SteerFront.pop_front();
		SteerRear.pop_front();
		CurNodeIdList.pop_front();
		CurNodeOffset.pop_front();
	}

	//VelFrontMotor = VibrationInfo.VelFrontMotor;
	SeriesFrontMotor->AddXY(dVelTime_sec,VelFrontMotor,String(nTag),clBlack);
	CurNodeIdList.push_back(nTag);
	CurNodeOffset.push_back(nOffset);
	SteerFront.push_back(nSteerF);
	SteerRear.push_back(nSteerR);

	dMinVibTime_sec = SeriesVibRMS->XValue[0];

	if(SeriesSpecOutPoint->Count() > 0){
		while(dMinVibTime_sec > SeriesSpecOutPoint->XValue[0])
		{
			SeriesSpecOutPoint->Delete(0);
			ListBoxSpecOutList->Items->Delete(0);
			if(SeriesSpecOutPoint->Count() == 0) break;
		}
	}

	dVelTime_sec += BOTTOMAXIS_RESOLUTION_SEC_VEL;


	int cntVibCart = SeriesFrontMotor->Count();
	float fN = BOTTOMAXIS_SEC_MAX/BOTTOMAXIS_RESOLUTION_SEC_VEL;
	int nN = (int)(BOTTOMAXIS_SEC_MAX/BOTTOMAXIS_RESOLUTION_SEC_VEL);
	if(dVibTime_sec >= BOTTOMAXIS_SEC_MAX+BOTTOMAXIS_RESOLUTION_SEC_VIB)
	{
		ScrollBarVibTime->Max = SeriesFrontMotor->Count() - (int)(BOTTOMAXIS_SEC_MAX/BOTTOMAXIS_RESOLUTION_SEC_VEL) + 1;
	}
	ScrollBarVibTime->Position = ScrollBarVibTime->Max;

	int dwTime = timeGetTime();
//	memoDebug->Lines->Add(FloatToStr(VibrationInfo.VelTransMotor) + "/" + IntToStr(dwTimeStart)+"/" + IntToStr(dwTime));
}

//---------------------------------------------------------------------------


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
	// Position= 0 ~ SeriesFrontMotor->Count()[150] - 10s/0.1s[100] + 1
	pos = (bottom_axis_min - dMinVibTime_sec)/BOTTOMAXIS_RESOLUTION_SEC_VEL;
	if(pos > ScrollBarVibTime->Max) pos = ScrollBarVibTime->Max;

	ScrollBarVibTime->Position = pos;

	x = SeriesSpecOutPoint->XValue[indexSelectedItem];
	for(int i = 0; i < SeriesVibRMS->Count(); i++)
	{
		if(x == SeriesVibRMS->XValue[i]){
			ValueIndex = i;
			break;
		}
	}

	if((ValueIndex < 0) || (ValueIndex >= SeriesVibRMS->Count())) return;
	DrawCheckLineDisplayInfo(ValueIndex/NUM_OF_SAMPLE, SeriesVibRMS->XValue[ValueIndex]);
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


void __fastcall TfrmVibrationMeasurement::FormClose(TObject *Sender, TCloseAction &Action)

{
	if(nSensorConnectionStep != VIB_SENSOR_DISCONNECTED)
	{
		bool bRealTimeLogSave;
		bRealTimeLogSave = cbRealTimeLog->Checked;
		btnChangeSaveFilePath->Enabled = bRealTimeLogSave;
		edtSaveFilePath->Enabled = bRealTimeLogSave;
		cbRealTimeLog->Enabled = true;
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
}
//---------------------------------------------------------------------------



void __fastcall TfrmVibrationMeasurement::cbRealTimeLogClick(TObject *Sender)
{
	bool bRealTimeLogSave;
	bRealTimeLogSave = cbRealTimeLog->Checked;
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
	if(nSensorConnectionStep == VIB_SENSOR_DISCONNECTED)
	{
		btnVibMeasureStart->Enabled = false;
		btnVibMeasureStop->Enabled = false;
	}
	else if(nSensorConnectionStep == VIB_SENSOR_CONNECTED)
	{
		btnVibMeasureStart->Enabled = true;
		btnVibMeasureStop->Enabled = true;
	}
}
//---------------------------------------------------------------------------

#include <fstream>

void __fastcall TfrmVibrationMeasurement::btFileOpenClick(TObject *Sender)
{
	MeasurementInitialization();
	cgGauge->MaxValue = 0;
	cgGauge->Progress = 0;
	m_bStopDrawChart = false;

	if(OpenDialog->Execute())
	{
		AVS_VIBRATION_DATA_INFO drawData;
		const int COL_COUNT = 10;
		char buf[256] = {0,};
		float dRMS = 0.0f, dVel = 0.0f, dX = 0.0f, dY = 0.0f, dZ = 0.0f;
		int nTimeOffset, nNodeID, nOffset = 0;
		int retValue = 0;
		char steerFL = 0, steerRL = 0;
		FILE* fs;

		//파일 열기
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

			//두번째 줄부터 읽음 %d,%06d,%d,%.02f,%c,%c,%.02f,%.02f,%.02f,%.02f
			while(retValue != EOF)
			{
				//Stop 버튼 눌렀을 때 탈출
				if(m_bStopDrawChart == true) break;

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
					int n = sscanf(buf, "%d %d %d %f %c %c %f %f %f %f",
						&nTimeOffset,
						&nNodeID,
						&nOffset,
						&dVel,
						&steerFL,
						&steerRL,
						&dRMS,
						&dX,
						&dY,
						&dZ);


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
					strLoadCounter.sprintf("%d/%d", cgGauge->Progress, line);
					lbLoadCounter->Caption = strLoadCounter;
				}

				//차트 그리기
				DisplayVibrationData(drawData);
				Application->ProcessMessages();
			}

			//버튼 활성화 변경
			btStopDraw->Enabled = false;
			btFileOpen->Enabled = true;
			btInitData->Enabled = true;
		}

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
}
//---------------------------------------------------------------------------

void __fastcall TfrmVibrationMeasurement::btExecuteFFTClick(TObject *Sender)
{
	FFTViewerForm->Show();
}
//---------------------------------------------------------------------------

