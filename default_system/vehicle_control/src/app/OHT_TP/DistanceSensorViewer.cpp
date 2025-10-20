//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DistanceSensorViewer.h"
#include "OHTInfoUnit.h"
#include "MainUnit.h"
#include "CMDControlUnit.h"
#include "Def_DataType.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDistanceSensorViewrForm *DistanceSensorViewrForm;
//---------------------------------------------------------------------------

struct point{
	int X;
	int Y;
};

point gLookdownSensorArea[NUM_PBS_SELECT_AREA][NUM_AREA_POINT] = {
	{{655,250}, {655,1999}, {103,1999}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{655,250}, {625,1999}, {-75,1999}, {-75,1025}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{450,250}, {450,1999}, {-75,1999}, {-75,1025}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{450,500}, {450,2000}, {0,2000}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{350,500}, {350,2000}, {0,2000}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{350,500}, {350,1700}, {0,1700}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{350,500}, {350,1400}, {0,1400}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{350,500}, {350,900}, {0,900}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
};

point gOhtDetectSensorArea[NUM_UBG_SELECT_AREA][NUM_AREA_POINT] = {
	{{100, 0}, {310, 350}, {310, 5000}, {0, 5000}, {-310, 5000}, {-310, 350}, {-100, 0}, {0,0}},
	{{870, 0}, {870, 1000}, {320, 1500}, {320, 3000}, {0, 3000}, {-320, 3000}, {-320, 350}, {0,0}},
	{{320, 350}, {320, 3000}, {0, 3000}, {-320, 3000}, {-320, 1500}, {-870, 1000}, {-870, 0}, {0,0}},
	{{770, 0}, {800, 800}, {200, 1370}, {-200, 1370}, {-800, 800}, {-770, 0}, {0,0}},
	{{550, 0}, {600, 1000}, {300, 1500}, {13, 3000}, {-300, 1500}, {-600, 1000}, {-550, 0}, {0,0}},
	{{475, 0}, {550, 1000}, {300, 1500}, {13, 3000}, {-300, 1500}, {-550, 1000}, {-475, 0}, {0,0}},
	{{450, 0}, {450, 735}, {450, 3000}, {0, 3000}, {-450, 3000}, {-450, 735}, {-450, 0}, {0,0}},
	{{450, 0}, {450, 735}, {450, 3000}, {0, 3000}, {-450, 3000}, {-450, 735}, {-450, 0}, {0,0}},
	{{450, 0}, {450, 735}, {450, 3000}, {0, 3000}, {-450, 3000}, {-450, 735}, {-450, 0}, {0,0}},
	{{450, 0}, {450, 735}, {450, 3000}, {0, 3000}, {-450, 3000}, {-450, 735}, {-450, 0}, {0,0}},
	{{350, 0}, {350, 1000}, {200, 600}, {-100, 300}, {-100, 0}, {0,0}, {0,0}, {0,0}},
	{{80, 0}, {80, 300}, {-220, 600}, {-370, 1000}, {-370, 0}, {0,0}, {0,0}, {0,0}},
	{{850, 0}, {850, 800}, {-80, 800}, {-80, 0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{80, 0}, {80, 800}, {-850, 800}, {-850, 0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{450, 0}, {450, 735}, {450, 3000}, {0, 3000}, {-450, 3000}, {-450, 735}, {-450, 0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
};

point gObstacleSensorArea[NUM_UBG_SELECT_AREA][NUM_AREA_POINT] = {
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{125, 150}, {125, 5000}, {125, 5000}, {0, 5000}, {-125, 5000}, {-125, 5000}, {-125, 150}, {0,0}},
	{{545, 78}, {445, 328}, {234, 510}, {0, 550}, {-125, 390}, {-125, 50}, {0,0}, {0,0}},
	{{125, 50}, {125, 390}, {0, 550}, {-234, 510}, {-445, 328}, {-545, 78}, {0,0}, {0,0}},
	{{280, 330}, {280, 450}, {92, 536}, {0, 550}, {-92, 536}, {-280, 450}, {-280, 330}, {0,0}},
	{{280, 330}, {280, 450}, {92, 536}, {0, 550}, {-92, 536}, {-280, 450}, {-280, 330}, {0,0}},
	{{280, 330}, {280, 450}, {92, 536}, {0, 550}, {-92, 536}, {-280, 450}, {-280, 330}, {0,0}},
	{{0,0}, {545, 78}, {445, 328}, {285, 465}, {-125, 130}, {-125, 50}, {0,0}, {0,0}},
	{{0,0}, {125, 50}, {125, 130}, {-285, 465}, {-445, 328}, {-545, 78}, {0,0}, {0,0}},
	{{280, 330}, {280, 450}, {200, 486}, {-100, 60}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{100, 60}, {-200, 486}, {-280, 450}, {-280, 330}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {545, 78}, {445, 328}, {395, 370}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {-395, 370}, {-445, 328}, {-545, 78}, {0,0}, {0,0}, {0,0}, {0,0}},
	{{0,0}, {220, 31}, {220, 515}, {0, 550}, {-220, 515}, {-220, 31}, {0,0}, {0,0}},
	{{350, 70}, {440, 240}, {440, 500}, {-440, 500}, {-440, 240}, {-350, 70}, {0, 0}, {0,0}},
	{{100, 150}, {100, 3000}, {100, 3000}, {0, 3000}, {-150, 3000}, {-150, 3000}, {-150, 150}, {0,0}},
};
//---------------------------------------------------------------------------

__fastcall TDistanceSensorViewrForm::TDistanceSensorViewrForm(TComponent* Owner)
	: TForm(Owner)
{
    //int tmpMaxSize;
	m_distanceSensorData.nSensorType = DISTANCE_SENSOR_DISABLE;
	//tmpMaxSize = (UBG_DISTANCE_DATA_SIZE > PBS_DISTANCE_DATA_SIZE) ? UBG_DISTANCE_DATA_SIZE:PBS_DISTANCE_DATA_SIZE;
	m_distanceSensorData.pDistanceData = gm_OHTStatus->m_OHTStatus.DistanceSensorData.nDistance;
	m_distanceSensorData.pDistancePoint = new TPoint[SOSLABUBG_DISTANCE_DATA_SIZE+1];
	timerDistanceDataUpdate->Enabled = false;
	memoDebug->Lines->Clear();
}


//extern int distanceUbgSample[513];
//extern int distancePbsSample[121];
//extern void test(int *src, int size);
//---------------------------------------------------------------------------

/*
      if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

		m_CMDControl->MakeTPCmd2OHT(TPCMD_TRACTION_STEER_LEFT);
        MainForm->DataDisplay(COLOR_BLACK, "Traction Mode Steering Left");

        delete m_CMDControl;
    }

*/
void __fastcall TDistanceSensorViewrForm::gbSelectDistanceSensorClick(TObject *Sender)
{
	int nPatternNum;
	TCMDCONTROL *m_CMDControl;

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL)
	{
		MainForm->Warnning_Display(clRed, "OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		return;
	}

    if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE))
    {
        MainForm->Warnning_Display(clRed, "OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		return;
    }

	m_distanceSensorData.nSensorType = gbSelectDistanceSensor->ItemIndex;

	switch (m_distanceSensorData.nSensorType) {

	case DISTANCE_SENSOR_DISABLE:
		timerDistanceDataUpdate->Enabled = false;
		break;
	case PBS_LOOKDOWN_SENSOR:
		m_distanceSensorData.nDistanceDataSize = PBS_DISTANCE_DATA_SIZE;
		m_distanceSensorData.nDistanceLimit = PBS_DISTANCE_LIMIT;
		m_distanceSensorData.dDegreeResolution = 180.0/(PBS_DISTANCE_DATA_SIZE-1);
		break;

	case UBG_OHTDETECT_SENSOR:
	case UBG_OBSTACLE_SENSOR:
		m_distanceSensorData.nDistanceDataSize = UBG_DISTANCE_DATA_SIZE;
		m_distanceSensorData.nDistanceLimit = UBG_DISTANCE_LIMIT;
		m_distanceSensorData.dDegreeResolution = 180.0/(UBG_DISTANCE_DATA_SIZE-1);
		break;
	case UBG_OHTDETECT_SOSLAB_SENSOR: //puting
	case UBG_OBSTACLE_SOSLAB_SENSOR:
		m_distanceSensorData.nDistanceDataSize = SOSLABUBG_DISTANCE_DATA_SIZE;
		m_distanceSensorData.nDistanceLimit = UBG_DISTANCE_LIMIT;
		m_distanceSensorData.dDegreeResolution = 180.0/(SOSLABUBG_DISTANCE_DATA_SIZE-1);
		break;
	default:
		;
	}
	m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2OHT(TPCMD_DISTANCE_SENSOR_REQUEST_STOP+m_distanceSensorData.nSensorType);
	delete m_CMDControl;
	String strTemp = "";
	strTemp = IntToStr(TPCMD_DISTANCE_SENSOR_REQUEST_STOP+m_distanceSensorData.nSensorType);
	memoDebug->Lines->Add(strTemp);

	if(m_distanceSensorData.nSensorType == DISTANCE_SENSOR_DISABLE)
	{
		timerDistanceDataUpdate->Enabled = false;
		return;
	}

	m_distanceSensorData.nPatternNum = GetSensorArea(m_distanceSensorData.nSensorType);
	AreaSelect(m_distanceSensorData.nSensorType);
	//test(m_distanceSensorData.pDistanceData, m_distanceSensorData.nDistanceDataSize);
	timerDistanceDataUpdate->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TDistanceSensorViewrForm::paintBoxDistanceSensorPaint(TObject *Sender)

{
	// paint
	if(m_distanceSensorData.nSensorType == DISTANCE_SENSOR_DISABLE) return;

	//
}
//---------------------------------------------------------------------------
void TDistanceSensorViewrForm::ConvertDistanceToXY(double radius, double scale)
{
	// paint를 위한 x, y 좌표 계산
	int startIndex = 0;
	double degree = 0.0;
	double distance = 0.0;
	int ndistance = 0.0;
	int tempCloasestDis = m_distanceSensorData.nDistanceLimit;
	int indexClosestDis;
	int width = paintBoxDistanceSensor->Width;
	int height = paintBoxDistanceSensor->Height;

	if(m_distanceSensorData.nSensorType == DISTANCE_SENSOR_DISABLE) return;
	if(m_distanceSensorData.nSensorType == PBS_LOOKDOWN_SENSOR)
	{
		// PBS
		startIndex = PBS_DISTANCE_DATA_TRIM_SIZE;
	}

	if(m_distanceSensorData.nSensorType != UBG_OHTDETECT_SOSLAB_SENSOR && m_distanceSensorData.nSensorType !=UBG_OBSTACLE_SOSLAB_SENSOR)
	{
		for(int i = 0; i < m_distanceSensorData.nDistanceDataSize; i++)
		{
			distance = (double)m_distanceSensorData.pDistanceData[i+startIndex];
			if(distance < 0.0) distance = 0.0;
			else if(distance > m_distanceSensorData.nDistanceLimit) distance = (double)m_distanceSensorData.nDistanceLimit;

			if((m_distanceSensorData.nSensorType == UBG_OHTDETECT_SENSOR || m_distanceSensorData.nSensorType == UBG_OBSTACLE_SENSOR) && distance < 20) distance = 0.0;

			if(distance == 0.0)
			{
				m_distanceSensorData.pDistancePoint[i].X = radius;
				m_distanceSensorData.pDistancePoint[i].Y = 0;
				continue;
			}


			degree = (double)i*m_distanceSensorData.dDegreeResolution*MATH_PI/180.0;

			if(tempCloasestDis > distance) // 최소 거리 data update
			{
				tempCloasestDis = distance;
				m_distanceSensorData.indexClosestDistance = i;
			}

			if(degree == 90.0)
			{
				m_distanceSensorData.pDistancePoint[i].X = 0;
				m_distanceSensorData.pDistancePoint[i].Y = CONVERT2INT(distance*scale);
			}
			else if(degree < 90.0)
			{
				m_distanceSensorData.pDistancePoint[i].X = CONVERT2INT(-1.0*distance*scale*Cos(degree)+radius);
				m_distanceSensorData.pDistancePoint[i].Y = CONVERT2INT(distance*scale*Sin(degree));
			}
			else
			{
				m_distanceSensorData.pDistancePoint[i].X = CONVERT2INT(distance*scale*Cos(degree));
				m_distanceSensorData.pDistancePoint[i].Y = CONVERT2INT(distance*scale*Sin(degree)+radius);
			}

		}
	}else //SOSLAB
	{
		for(int i = 0; i < m_distanceSensorData.nDistanceDataSize; i++)
		{

			ndistance = (m_distanceSensorData.pDistanceData[i+startIndex] & 0xffff0000) >> 16;

			if(ndistance < 0) ndistance = m_distanceSensorData.nDistanceLimit;
			else if(ndistance > m_distanceSensorData.nDistanceLimit) ndistance = m_distanceSensorData.nDistanceLimit;

			if(ndistance < 20 && ndistance >= 0) ndistance = 0;

			if(ndistance == 0)
			{
				m_distanceSensorData.pDistancePoint[i].X = radius;
				m_distanceSensorData.pDistancePoint[i].Y = 0;
				continue;
			}

            if(tempCloasestDis > ndistance) // 최소 거리 data update
			{
				tempCloasestDis = ndistance;
				m_distanceSensorData.indexClosestDistance = i;
			}

			degree = (double)i*m_distanceSensorData.dDegreeResolution*MATH_PI/180.0;

			if(degree == 90.0)
			{
				m_distanceSensorData.pDistancePoint[i].X = 0;
				m_distanceSensorData.pDistancePoint[i].Y = CONVERT2INT(ndistance*scale);
			}
			else if(degree < 90.0)
			{
				m_distanceSensorData.pDistancePoint[i].X = CONVERT2INT(-1.0*ndistance*scale*Cos(degree)+radius);
				m_distanceSensorData.pDistancePoint[i].Y = CONVERT2INT(ndistance*scale*Sin(degree));
			}
			else
			{
				m_distanceSensorData.pDistancePoint[i].X = CONVERT2INT(ndistance*scale*Cos(degree));
				m_distanceSensorData.pDistancePoint[i].Y = CONVERT2INT(ndistance*scale*Sin(degree)+radius);
			}

		}
    }


	// 도형 그리기 위해 원점 추가
	m_distanceSensorData.pDistancePoint[m_distanceSensorData.nDistanceDataSize].X = radius;
	m_distanceSensorData.pDistancePoint[m_distanceSensorData.nDistanceDataSize].Y = 0;

	for(int i = 0; i <= m_distanceSensorData.nDistanceDataSize; i++)
	{
    	// lookdown sensor는 그대로
		// OHT Detect sensor는 상하 반전
		// Obstacle sensor는 상화좌우 반전
		if((m_distanceSensorData.nSensorType == UBG_OHTDETECT_SENSOR) || (m_distanceSensorData.nSensorType == UBG_OHTDETECT_SOSLAB_SENSOR))
		{
			// 상하 반전
			m_distanceSensorData.pDistancePoint[i].Y = height - m_distanceSensorData.pDistancePoint[i].Y;		}
		else if((m_distanceSensorData.nSensorType == UBG_OBSTACLE_SENSOR) || (m_distanceSensorData.nSensorType = UBG_OBSTACLE_SOSLAB_SENSOR))
		{
			// 상하 좌우 반전
			m_distanceSensorData.pDistancePoint[i].X = width - m_distanceSensorData.pDistancePoint[i].X;
			m_distanceSensorData.pDistancePoint[i].Y = height - m_distanceSensorData.pDistancePoint[i].Y;
		}
	}

}
//---------------------------------------------------------------------------
void TDistanceSensorViewrForm::DrawSensorData()
{
	int width = paintBoxDistanceSensor->Width;
	int height = paintBoxDistanceSensor->Height;
	double radius, scale;

	radius = (double)width/2;
	scale = radius / (double)m_distanceSensorData.nDistanceLimit;

	// 흰 배경
	SetCanvas(DISTANCE_SENSOR_DRAWING_PAPER);
	paintBoxDistanceSensor->Canvas->Rectangle(-1,-1,width+1,height+1);

	// 센서에 검출된 거리 데이터
	DrawDetectArea(radius, scale);

	// 센서 패턴
	DrawPatternArea(radius, scale);

	// 거리 및 각도 표시 background
	DrawBackGround(radius, scale);
}
//---------------------------------------------------------------------------
void TDistanceSensorViewrForm::DrawDetectArea(double radius, double scale)
{
	SetCanvas(DISTANCE_SENSOR_DETECT_AREA);
	/*
	//////////////////////////////////////////////////////////////
	String strTemp = "";
	int startIndex = 0;
	int distance;
	if(m_distanceSensorData.nSensorType == PBS_LOOKDOWN_SENSOR)
	{
		// PBS
		startIndex = PBS_DISTANCE_DATA_TRIM_SIZE;
	}
	for(int i = 0; i < m_distanceSensorData.nDistanceDataSize; i++)
	{
		distance = m_distanceSensorData.pDistanceData[i+startIndex];
		strTemp += IntToStr(distance) + "/";
	}

	memoDebug->Lines->Add(strTemp);
	////////////////////////////////////////////////////////////
    */

	ConvertDistanceToXY(radius,scale);

	paintBoxDistanceSensor->Canvas->Polygon(m_distanceSensorData.pDistancePoint,m_distanceSensorData.nDistanceDataSize);

	double degree;
	int  ndistance = 0 ;
	if(m_distanceSensorData.nSensorType != UBG_OHTDETECT_SOSLAB_SENSOR && m_distanceSensorData.nSensorType !=UBG_OBSTACLE_SOSLAB_SENSOR)
	{
		ndistance = m_distanceSensorData.pDistanceData[m_distanceSensorData.indexClosestDistance];
	}
	else
	{
		ndistance = (m_distanceSensorData.pDistanceData[m_distanceSensorData.indexClosestDistance] & 0xffff0000) >> 16;
	}

	txtMinDist->Caption = "거리: " + IntToStr(ndistance) + "mm";
	degree = m_distanceSensorData.indexClosestDistance * m_distanceSensorData.dDegreeResolution;
	if(m_distanceSensorData.nSensorType == UBG_OBSTACLE_SENSOR) degree = 180.0 - degree;
	else if(m_distanceSensorData.nSensorType == PBS_LOOKDOWN_SENSOR) degree += PBS_DISTANCE_DATA_TRIM_SIZE * m_distanceSensorData.dDegreeResolution;
	txtMinDegree->Caption =  "각도: " + FloatToStr(m_distanceSensorData.indexClosestDistance * m_distanceSensorData.dDegreeResolution) + "º";
	m_distanceSensorData.nPatternNum = GetSensorArea(m_distanceSensorData.nSensorType);
	txtPaternInfo->Caption = "패턴:" +  IntToStr(m_distanceSensorData.nPatternNum);



	SetCanvas(DISTANCE_SENSOR_CLOSEST_POINT);
	paintBoxDistanceSensor->Canvas->Ellipse(m_distanceSensorData.pDistancePoint[m_distanceSensorData.indexClosestDistance].X-3,
	m_distanceSensorData.pDistancePoint[m_distanceSensorData.indexClosestDistance].Y-3,
	m_distanceSensorData.pDistancePoint[m_distanceSensorData.indexClosestDistance].X+3,
	m_distanceSensorData.pDistancePoint[m_distanceSensorData.indexClosestDistance].Y+3);
	// lookdown sensor는 그대로
	// OHT Detect sensor는 상하 반전
	// Obstacle sensor는 상화좌우 반전

}
//---------------------------------------------------------------------------
void TDistanceSensorViewrForm::DrawPatternArea(double radius, double scale)
{
	int width = paintBoxDistanceSensor->Width;
	int height = paintBoxDistanceSensor->Height;
	static int nPrePatternNum  = 0;
	static double dPreScale  = 1.0;
	static TPoint pPatternAreaReScale[NUM_UBG_SELECT_AREA][NUM_AREA_POINT];
    if(m_distanceSensorData.nPatternNum == 0) return;

	SetCanvas(DISTANCE_SENSOR_PATTERN_AREA);
	if(nPrePatternNum != m_distanceSensorData.nPatternNum || dPreScale != scale)
	{
		for(int i=0; i< NUM_AREA_POINT; i++){
			pPatternAreaReScale[m_distanceSensorData.nPatternNum-1][i].X = CONVERT2INT((double)(m_distanceSensorData.pPatternArea[m_distanceSensorData.nPatternNum-1][i].X) * scale + radius);
			pPatternAreaReScale[m_distanceSensorData.nPatternNum-1][i].Y = CONVERT2INT((double)(m_distanceSensorData.pPatternArea[m_distanceSensorData.nPatternNum-1][i].Y) * scale);
			if((m_distanceSensorData.nSensorType == UBG_OHTDETECT_SENSOR) || (m_distanceSensorData.nSensorType == UBG_OHTDETECT_SOSLAB_SENSOR)){
				pPatternAreaReScale[m_distanceSensorData.nPatternNum-1][i].Y = height - pPatternAreaReScale[m_distanceSensorData.nPatternNum-1][i].Y;
			}
			else if((m_distanceSensorData.nSensorType == UBG_OBSTACLE_SENSOR)|| (m_distanceSensorData.nSensorType = UBG_OBSTACLE_SOSLAB_SENSOR)){
				pPatternAreaReScale[m_distanceSensorData.nPatternNum-1][i].X = width - pPatternAreaReScale[m_distanceSensorData.nPatternNum-1][i].X;
				pPatternAreaReScale[m_distanceSensorData.nPatternNum-1][i].Y = height - pPatternAreaReScale[m_distanceSensorData.nPatternNum-1][i].Y;
			}
		}
		nPrePatternNum = m_distanceSensorData.nPatternNum;
		dPreScale = scale;
	}
	paintBoxDistanceSensor->Canvas->Polygon(&pPatternAreaReScale[m_distanceSensorData.nPatternNum-1][0],NUM_AREA_POINT-1);

}
//---------------------------------------------------------------------------
void TDistanceSensorViewrForm::DrawBackGround(double radius, double scale)
{
	int nCircle;
	String strTemp;
	TPoint pTempArea1;
	TPoint pTempArea2;

	int height = paintBoxDistanceSensor->Height;

	int tempY = 0;
	int tempX = 0;
	double tempTheta;

	// background circle 개수
	nCircle = m_distanceSensorData.nDistanceLimit / 1000 + 3;

	double lengthDegreeLine, xDegreeLine, yDegreeLine, degree;
	double xDegreeText, yDegreeText;
	int nxDegreeText;

	SetCanvas(DISTANCE_SENSOR_BACK_GROUND);
	paintBoxDistanceSensor->Canvas->Font->Size = 8;


	lengthDegreeLine = nCircle * 1000.0;

	for(int i=1, n=9; i<n; i++)
	{
		degree = ((double)i) * (90.0/(double)n) * MATH_PI /180.0;
		xDegreeLine = CONVERT2INT(scale * lengthDegreeLine * Cos(degree));
		yDegreeLine = CONVERT2INT(scale * lengthDegreeLine * Sin(degree));
		xDegreeText = CONVERT2INT(scale * m_distanceSensorData.nDistanceLimit *0.8* Cos(degree));
		yDegreeText = CONVERT2INT(scale * m_distanceSensorData.nDistanceLimit *0.8* Sin(degree));

		tempX = -xDegreeLine + radius;
		tempY = yDegreeLine;
		nxDegreeText = -xDegreeText + radius;
		if(m_distanceSensorData.nSensorType == UBG_OHTDETECT_SENSOR  || m_distanceSensorData.nSensorType == UBG_OHTDETECT_SOSLAB_SENSOR || m_distanceSensorData.nSensorType == UBG_OBSTACLE_SOSLAB_SENSOR || m_distanceSensorData.nSensorType == UBG_OBSTACLE_SENSOR)
		{
			// 상하 반전
			tempY = height-tempY;
			yDegreeText = height - yDegreeText;
			paintBoxDistanceSensor->Canvas->MoveTo(radius,height);
		}
		else
		{
			paintBoxDistanceSensor->Canvas->MoveTo(radius,0);
		}
		paintBoxDistanceSensor->Canvas->LineTo(tempX,tempY);
		strTemp = IntToStr(i*10) + "º";
		paintBoxDistanceSensor->Canvas->TextOutW(nxDegreeText, yDegreeText, strTemp);

		if(m_distanceSensorData.nSensorType == PBS_LOOKDOWN_SENSOR)
		{
			paintBoxDistanceSensor->Canvas->MoveTo(radius,0);
		}
		else
		{
			paintBoxDistanceSensor->Canvas->MoveTo(radius,height);
		}

		tempX = xDegreeLine + radius;
		nxDegreeText = xDegreeText + radius;
		paintBoxDistanceSensor->Canvas->LineTo(tempX,tempY);

		strTemp = IntToStr(90-i*10 + 90) + "º";
		paintBoxDistanceSensor->Canvas->TextOutW(nxDegreeText, yDegreeText, strTemp);

	}
	if(m_distanceSensorData.nSensorType == PBS_LOOKDOWN_SENSOR)
	{
		paintBoxDistanceSensor->Canvas->TextOutW(radius, scale *m_distanceSensorData.nDistanceLimit*0.8, "90º");
	}
	else
	{
		paintBoxDistanceSensor->Canvas->TextOutW(radius, height-scale *m_distanceSensorData.nDistanceLimit*0.8, "90º");
	}
	paintBoxDistanceSensor->Canvas->MoveTo(radius,0);
	paintBoxDistanceSensor->Canvas->LineTo(radius,height);


	for(int i = 1; i < nCircle+1; i++)
	{
		pTempArea1.X = -i*1000.0*scale+radius;
		pTempArea1.Y = -i*1000.0*scale;
		pTempArea2.X = i*1000.0*scale+radius;
		pTempArea2.Y = i*1000.0*scale;
		if(m_distanceSensorData.nSensorType == UBG_OHTDETECT_SENSOR ||m_distanceSensorData.nSensorType == UBG_OHTDETECT_SOSLAB_SENSOR || m_distanceSensorData.nSensorType == UBG_OBSTACLE_SOSLAB_SENSOR||m_distanceSensorData.nSensorType == UBG_OBSTACLE_SENSOR)
		{
			pTempArea1.Y = height + pTempArea1.Y;
			pTempArea2.Y = height + pTempArea2.Y;
			tempY = height-paintBoxDistanceSensor->Canvas->Font->Size*2;
		}
		else
		{
			tempY = 0;
		}

		strTemp = i;
		strTemp += "000mm";
		paintBoxDistanceSensor->Canvas->Ellipse(pTempArea1.X,pTempArea1.Y, pTempArea2.X, pTempArea2.Y);
		paintBoxDistanceSensor->Canvas->TextOutW(pTempArea2.X - paintBoxDistanceSensor->Canvas->Font->Size*5, tempY, strTemp);
	}
}
//---------------------------------------------------------------------------
void TDistanceSensorViewrForm::SetCanvas(int CanvasType)
{
	//DistanceSensorViewrForm->Canvas->Brush->Style = ;
	//DistanceSensorViewrForm->Canvas->Brush->Color = ;
	//DistanceSensorViewrForm->Canvas->Pen->Color = ;
	//DistanceSensorViewrForm->Canvas->Pen->Width = ;
	switch (CanvasType){
	case DISTANCE_SENSOR_DRAWING_PAPER:
		paintBoxDistanceSensor->Canvas->Brush->Style = bsClear;
		paintBoxDistanceSensor->Canvas->Brush->Color = clWhite;
		paintBoxDistanceSensor->Canvas->Pen->Color = clBlack;
		paintBoxDistanceSensor->Canvas->Pen->Width = 1;
		break;

	case DISTANCE_SENSOR_DETECT_AREA:   //감지 영역
		paintBoxDistanceSensor->Canvas->Brush->Style = bsClear;
		paintBoxDistanceSensor->Canvas->Brush->Color = clMedGray;
		paintBoxDistanceSensor->Canvas->Pen->Color = clDkGray;
		paintBoxDistanceSensor->Canvas->Pen->Width = 2;
		break;

	case DISTANCE_SENSOR_PATTERN_AREA:   //패턴 모양
		paintBoxDistanceSensor->Canvas->Brush->Color =  clMaroon;
		paintBoxDistanceSensor->Canvas->Brush->Style = bsDiagCross;
		paintBoxDistanceSensor->Canvas->Pen->Color = clMaroon;
		paintBoxDistanceSensor->Canvas->Pen->Width = 1;
		break;

	case DISTANCE_SENSOR_BACK_GROUND:
		paintBoxDistanceSensor->Canvas->Brush->Color =  clBlack;
		paintBoxDistanceSensor->Canvas->Brush->Style = bsClear;
		paintBoxDistanceSensor->Canvas->Pen->Color = clBlack;
		paintBoxDistanceSensor->Canvas->Pen->Width = 1;
		break;

	case DISTANCE_SENSOR_CLOSEST_POINT:
		paintBoxDistanceSensor->Canvas->Brush->Color =  clRed;
		paintBoxDistanceSensor->Canvas->Brush->Style = bsClear;
		paintBoxDistanceSensor->Canvas->Pen->Color = clRed;
		paintBoxDistanceSensor->Canvas->Pen->Width = 2;
		break;

	default:
        ;
	}
}

//---------------------------------------------------------------------------
void __fastcall TDistanceSensorViewrForm::timerDistanceDataUpdateTimer(TObject *Sender)

{
	if(m_distanceSensorData.nSensorType == DISTANCE_SENSOR_DISABLE) return;
	DrawSensorData();
	//Invalidate();
}
//---------------------------------------------------------------------------

void TDistanceSensorViewrForm::UpdatePattern()
{
	static int nPrePattern = 0;
	if(nPrePattern != m_distanceSensorData.nPatternNum)
	{

		nPrePattern = m_distanceSensorData.nPatternNum;
	}
}

//---------------------------------------------------------------------------

int TDistanceSensorViewrForm::GetSensorArea(int SensorType)
{
	int nValue = 0;

	if(SensorType == PBS_LOOKDOWN_SENSOR){
		nValue	= 	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel << 1)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel << 2)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel << 3);
		//nValue = 3; // test
	}
	else if(SensorType == UBG_OHTDETECT_SENSOR){
		nValue	= 	(gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel << 1)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel << 2)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel << 3);
		//nValue = 4; // test
	}
	else if(SensorType == UBG_OBSTACLE_SENSOR){
		nValue	= 	(gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel << 1)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel << 2)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel << 3);
		//nValue = 17; // test
	}else if(SensorType == UBG_OHTDETECT_SOSLAB_SENSOR){
		nValue	= 	(gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area1_Sel)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area2_Sel << 1)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area3_Sel << 2)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OHTDETECT_Area4_Sel << 3);
		//nValue = 4; // test
	}else if(SensorType == UBG_OBSTACLE_SOSLAB_SENSOR){
		nValue	= 	(gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area1_Sel)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area2_Sel << 1)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area3_Sel << 2)
				+	(gm_OHTStatus->IOOutput->uOutputCommon.OBSTACLE_Area4_Sel << 3);
		//nValue = 17; // test
	}
	
	nValue = ~nValue & 0xF;
	if(SensorType == UBG_OBSTACLE_SENSOR) nValue += 0x10;
	return nValue;

}
//---------------------------------------------------------------------------
void TDistanceSensorViewrForm::AreaSelect(int nSensorType)
{
	if(nSensorType == PBS_LOOKDOWN_SENSOR)
	{
		for(int i=0; i<NUM_PBS_SELECT_AREA; i++){
			for(int j=0;j<NUM_AREA_POINT; j++){
				m_distanceSensorData.pPatternArea[i][j].X = -1*gLookdownSensorArea[i][j].X;
				m_distanceSensorData.pPatternArea[i][j].Y = gLookdownSensorArea[i][j].Y;
			}
		}
	}
	else if(nSensorType == UBG_OHTDETECT_SENSOR)
	{
		for(int i=0; i<NUM_UBG_SELECT_AREA; i++){
			for(int j=0;j<NUM_AREA_POINT; j++){
				m_distanceSensorData.pPatternArea[i][j].X = -1*gOhtDetectSensorArea[i][j].X;
				m_distanceSensorData.pPatternArea[i][j].Y = gOhtDetectSensorArea[i][j].Y;
			}
		}
	}
	else if(nSensorType == UBG_OHTDETECT_SOSLAB_SENSOR)
	{
		for(int i=0; i<NUM_UBG_SELECT_AREA; i++){
			for(int j=0;j<NUM_AREA_POINT; j++){
				m_distanceSensorData.pPatternArea[i][j].X = -1*gOhtDetectSensorArea[i][j].X;
				m_distanceSensorData.pPatternArea[i][j].Y = gOhtDetectSensorArea[i][j].Y;
			}
		}
	}
	else
	{
		for(int i=0; i<NUM_UBG_SELECT_AREA; i++){
			for(int j=0;j<NUM_AREA_POINT; j++){
				m_distanceSensorData.pPatternArea[i][j].X = -1*gObstacleSensorArea[i][j].X;
				m_distanceSensorData.pPatternArea[i][j].Y = gObstacleSensorArea[i][j].Y;
			}
		}
    }
}

//---------------------------------------------------------------------------
/*
int distancePbsSample[121] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-01,1749,1672,1898,1469,1613,1250,1250,1218,1171,1384,979,927,1072,1042,733,713,646,647,831,860,834,857,834,844,781,844,772,818,772,781,474,781,471,786,467,500,468,500,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int distanceUbgSample[513] = {19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45008,45008,45008,45014,45014,45014,45014,45014,45014,45014,45014,45008,45008,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45000,45000,45014,45000,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45008,1303,1301,1298,1301,1303,1311,1314,1314,1318,1321,1325,1326,1329,1335,1340,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45007,45014,1627,1627,1619,1619,1624,1624,1624,1627,1664,45000,45000,45006,45006,45006,45006,45006,45006,45006,45006,45006,45006,45006,45006,45006,45008,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45014,45007,45014,45014,45014,45014,45014,45014,45014,45014,45000,45000,45000,45000,45007,45014,45014,45014,45014,45014,45014,45014,45014,45000,45000,45000,45000,45006,45006,45006,45006,45006,45006,45006,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,45000,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19};


void test(int *src, int size)
{
	if(size == 101){
		for(int i = 0; i < 121; i++)
		{
			src[i] = distancePbsSample[i];
		}
	}
	else
	{
		for(int i = 0; i < size; i++)
		{
			src[i] = distanceUbgSample[i];
		}
	}
}

  */

//---------------------------------------------------------------------------

void __fastcall TDistanceSensorViewrForm::mnReadObsFileClick(TObject *Sender)
{
	String strFileName;
	if(OpenDialogDistSenAreaFile->Execute())
	{
    	UpdateSensorArea(OpenDialogDistSenAreaFile->FileName, UBG_OHTDETECT_SENSOR);
    }
}
//---------------------------------------------------------------------------

void __fastcall TDistanceSensorViewrForm::mnReadUbgFileClick(TObject *Sender)
{
	String strFileName;
	if(OpenDialogDistSenAreaFile->Execute())
	{
		UpdateSensorArea(OpenDialogDistSenAreaFile->FileName, UBG_OBSTACLE_SENSOR);
	}
}
//---------------------------------------------------------------------------

void __fastcall TDistanceSensorViewrForm::mnReadPbsFileClick(TObject *Sender)
{
	String strFileName;
	if(OpenDialogDistSenAreaFile->Execute())
	{
    	UpdateSensorArea(OpenDialogDistSenAreaFile->FileName, PBS_LOOKDOWN_SENSOR);
    }
}
//---------------------------------------------------------------------------

void TDistanceSensorViewrForm::UpdateSensorArea(String strFileName, int sensorType)
{
	int cntAreaFile, findPos1, findPos2;
	int indexArea, indexPoint;
	const int lenOfAreaText = 5;
	const int lenOfPointText = 6;
	String strNumArea;
	String strTemp;

	TStringList *areaInfo = new TStringList();
	areaInfo->LoadFromFile(strFileName);
	cntAreaFile = areaInfo->Count;

	int x;
	int y;


	for(int i = 0; i < cntAreaFile; i++)
	{
		findPos1 = areaInfo->Strings[i].Pos("[Area");
		if(findPos1)
		{
			findPos2 = areaInfo->Strings[i].Pos("]");
			findPos2 = findPos2 - findPos1 - lenOfAreaText;
			strTemp = areaInfo->Strings[i].SubString(findPos1+lenOfAreaText, findPos2);
			indexArea = strTemp.ToInt() - 1;
		}
		findPos1 = areaInfo->Strings[i].Pos("[Point");
		if(findPos1)
		{
			findPos2 = areaInfo->Strings[i].Pos("]");
			findPos2 = findPos2 - findPos1 - lenOfPointText;
			strTemp = areaInfo->Strings[i].SubString(findPos1+lenOfPointText, findPos2);
			indexPoint = strTemp.ToInt();
			if(i+1 <  cntAreaFile)
			{
				findPos1 = areaInfo->Strings[i+1].Pos(" ");
				x = areaInfo->Strings[i+1].SubString(0, findPos1-1).ToInt();
				findPos2 =  areaInfo->Strings[i+1].Length() - findPos1;
				y = areaInfo->Strings[i+1].SubString(findPos1+1, findPos2).ToInt();
				if(sensorType == PBS_LOOKDOWN_SENSOR){
					gLookdownSensorArea[indexArea][indexPoint].X =  x;
					gLookdownSensorArea[indexArea][indexPoint].Y =  y;
				}
				else if(sensorType == UBG_OHTDETECT_SENSOR){
					gOhtDetectSensorArea[indexArea][indexPoint].X =  x;
					gOhtDetectSensorArea[indexArea][indexPoint].Y =  y;
				}
				else if(sensorType == UBG_OBSTACLE_SENSOR){
					gObstacleSensorArea[indexArea][indexPoint].X =  x;
					gObstacleSensorArea[indexArea][indexPoint].Y =  y;
				}
				else if(sensorType == UBG_OHTDETECT_SOSLAB_SENSOR){
					gOhtDetectSensorArea[indexArea][indexPoint].X =  x;
					gOhtDetectSensorArea[indexArea][indexPoint].Y =  y;
				}
				else if(sensorType == UBG_OBSTACLE_SOSLAB_SENSOR){
					gObstacleSensorArea[indexArea][indexPoint].X =  x;
					gObstacleSensorArea[indexArea][indexPoint].Y =  y;
				}
            }
		}
    }
}


//---------------------------------------------------------------------------

void __fastcall TDistanceSensorViewrForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	gbSelectDistanceSensor->ItemIndex = 0;
}
//---------------------------------------------------------------------------

