/**
@file    Driving.cpp
@version OHT 7.0
@brief   Driving Class 소스 File
*/
#include <limits.h>	//for use: INT_MAX
#include "Driving.h"

#define	ERR_NO_HWCOMMON	-1

// by zzang9un 2013.06.21 : 기어비 변경
//#define	WHEEL_DIAMETER_RATIO	1.05//1.00	// Test 목적
#define	WHEEL_DIAMETER_RATIO	1.00	// 신뢰성 검증으로 인해 기어비 변경

//#define DRV_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\DrivingCtrl.log", __VA_ARGS__);\

/**
@brief   Driving Class 생성자
@date
@author
@param   pHWCommon : HWCommon 클래스 포인터
@param   nAxisNum : Motor에 사용될 축 번호
*/
Driving::Driving(HWCommon *pHWCommon, int nAxisNum):PropotionalMotionAxis(pHWCommon, nAxisNum, false, 1.0)
{
	m_pHWCommon = pHWCommon;
	m_pSlaveAxis = NULL;
	m_bSync = false;
	m_Curve_E_Stop = false;
	EnableUnitConverter();

	SetTargetPosRelease();
}

Driving::Driving(HWCommon *pHWCommon, int nAxisNum, double dGearRatio):PropotionalMotionAxis(pHWCommon, nAxisNum, false, dGearRatio)
{
	m_pHWCommon = pHWCommon;
	m_pSlaveAxis = NULL;
	m_bSync = false;
	EnableUnitConverter();

	SetTargetPosRelease();
}

Driving::Driving(HWCommon *pHWCommon, int nMasterAxisNum, int nSlaveAxisNum):PropotionalMotionAxis(pHWCommon, nMasterAxisNum, false, 1.0)//MotionAxis
{
	m_pHWCommon = pHWCommon;
	m_pSlaveAxis = new PropotionalMotionAxis(pHWCommon, nSlaveAxisNum, false, 1.0);

	m_nMasterAxisNum = nMasterAxisNum;
	m_nSlaveAxisNum = nSlaveAxisNum;

	SetBrakeRelease(false);
    Sleep(100);
	m_pHWCommon->SetSync(nMasterAxisNum, nSlaveAxisNum, 1.0);
	m_bSync = true;

	EnableUnitConverter();

	SetTargetPosRelease();
}

Driving::Driving(HWCommon *pHWCommon, int nMasterAxisNum, int nSlaveAxisNum, double dGearRatio):PropotionalMotionAxis(pHWCommon, nMasterAxisNum, false, dGearRatio)//MotionAxis
{
	//Info: double dGearRatio = /WHEEL_DIAMETER_RATIO;	// (8192*5)/(pi*115)  cts/mm	//113.273908			///test 시 사용 값: 112.86
	m_pHWCommon = pHWCommon;
	m_pSlaveAxis = new PropotionalMotionAxis(pHWCommon, nSlaveAxisNum, false, dGearRatio);

	m_nMasterAxisNum = nMasterAxisNum;
	m_nSlaveAxisNum = nSlaveAxisNum;

	SetBrakeRelease(false);
	Sleep(100);
	m_pHWCommon->SetSync(nMasterAxisNum, nSlaveAxisNum, 1.0);
	m_bSync = true;

	EnableUnitConverter();

	SetTargetPosRelease();
}

/**
@brief   Driving Class 소멸자
@date
@author
*/
Driving::~Driving()
{
	DisableUnitConverter();
	if(m_pSlaveAxis!=NULL)	delete m_pSlaveAxis;
}


bool Driving::SetGearRatio(PROPOTINAL_GEAR_RATIO GearRatio)
{
	double dGearRatio = GearRatio.dIdealGearRatio * GearRatio.dIdeal2RealRatio;
	if((m_pPropotionalConverter!=NULL)&&(dGearRatio>0))
	{
		m_GearRatio = GearRatio;
		if( (m_pPropotionalConverter->setCoefficient(dGearRatio)==true) &&
			(m_pSlaveAxis->SetGearRatio(m_GearRatio)==true)					)
		{
			return true;
		}
	}

	return false;
}



/**
@brief   // 입력
@date    // 입력
@author  // 입력
@param   // 입력(없을 경우 삭제)
@return  // 입력(없을 경우 삭제)
*/
bool Driving::IsBrakeReleased()
{

	bool bResult = false;

	if(m_pHWCommon==NULL) return false;

	if(m_pHWCommon->m_Output.BRAKE_Driving_Release == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   // 입력
@date    // 입력
@author  // 입력
@param   // 입력(없을 경우 삭제)
@return  // 입력(없을 경우 삭제)
*/
int Driving::SetBrakeRelease(bool bValue)
{
	int nError = NO_ERR;

	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;


	nError = m_pHWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, bValue);


	return nError;
}

/**
@brief   Driving의 단위 변환 기능 사용 결정 함수

	기어비 설정은 m_pPropotionalConverter를 이용한다.
@date    // 입력
@author  // 입력
@param   // 입력(없을 경우 삭제)
@return  Unit 사용 가능 하면 true Return
*/
bool Driving::EnableUnitConverter(void)
{
	bool bReturn = false;

	// Master 축 처리
	if(m_pPropotionalConverter==NULL)
	{
		bReturn = false;
	}
	else
	{
		m_pUnitConverter = m_pPropotionalConverter;
		bReturn = true;
	}

	// Slave 축 처리
	if(m_pSlaveAxis!=NULL && bReturn==true)
	{
		if(m_pSlaveAxis->EnableUnitConverter()!=true)
		{
			m_pSlaveAxis->DisableUnitConverter();
			bReturn = false;
		}
	}

	return bReturn;
}

/**
@brief   Driving Slave AMP On/Off 상태 확인 함수
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
bool Driving::IsSlaveAMPEnable()
{
	if(m_pHWCommon==NULL) return false;
	if(m_bSync==false) return false;

	return m_pSlaveAxis->IsAMPEnable();
//	return m_pHWCommon->IsAMPEnable(m_nSlaveAxisNum);
}

// 구동 관련 - 단위: 거리(mm), 속도(mm/msec), 가속도(mm/msec2), 시간(msec)
/**
@brief   Driving Slave Amp On/Off 설정 함수
@author  ehoto
@date    2012-11-01
@param   bValue : On/Off 값
@return  Error Code
*/
int Driving::SetSlaveAMPEnable(bool bValue)
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	if(m_bSync==false) return -1;

	return m_pSlaveAxis->SetAMPEnable(bValue);
}

// 구동 관련 - 단위: 거리(mm), 속도(mm/msec), 가속도(mm/msec2), 시간(msec)
/**
@brief   Driving Slave Amp On/Off 설정 함수
@author  ehoto
@date    2012-11-01
@param   bValue : On/Off 값
@return  Error Code
*/
bool Driving::IsSyncControl(void)
{
 	return m_bSync;
}

/**
@brief   Driving 현재 위치 Home 설정 함수
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
int Driving::SetSlaveHome(void)
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	if(m_bSync==false) return -1;

	return m_pSlaveAxis->SetHome();
}

/**
@brief   Driving 현재 위치 Home 설정 함수
@author  ehoto
@date    2012-11-01
@return  동작 중 여부 확인
*/
bool Driving::IsSlaveRunning()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	if(m_bSync==false) return -1;

	return m_pSlaveAxis->IsRunning();
}

/**
@brief   Driving 현재 위치 Home 설정 함수
@author  ehoto
@date    2012-11-01
@return  현재 위치(실수)
*/
double Driving::GetSlaveCurrentPosition()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	if(m_bSync==false) return -1;

	return m_pSlaveAxis->GetCurrentPosition();
}

/**
@brief   Driving 현재 위치 Home 설정 함수
@author  ehotom
@date    2012-11-01
@return  현재 속도(실수)
*/
double Driving::GetSlaveCurrentVelocity()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	if(m_bSync==false) return -1;

	return m_pSlaveAxis->GetCurrentVelocity();
}

/**
@brief   Driving 현재 위치 Home 설정 함수
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
int Driving::GetSlaveError()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	if(m_bSync==false) return -1;

	return m_pSlaveAxis->GetError();
}

/**
@brief   Driving 현재 위치 Home 설정 함수
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
int Driving::ResetSlave(void)
{
	return m_pSlaveAxis->Reset();
}


/**
@brief   주행부 Node Count를 Reset 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void Driving::ClearNodeCountInfo(void)
{
	m_pHWCommon->ClearNodeCountInfo();
}
/**
@brief   주행부 토크 설정 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void Driving::SetTorqueLimit()
{
	m_pHWCommon->SetTorqueLimit();
}
/**
@brief   주행부 주행부 토크 설정 해제 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void Driving::SetTorqueLimitRelease()
{
	m_pHWCommon->SetTorqueLimitRelease();
}
/**
@brief   경사로 설정 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void Driving::SetSlopeTime()
{
	m_pHWCommon->SetSlopeTime();
}
/**
@brief   경사로 설정 해제 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void Driving::SetSlopeTimeRelease()
{
	m_pHWCommon->SetSlopeTimeRelease();
}

/**
@brief   목적지  설정 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void Driving::SetTargetPos()
{
	m_bTargetPos2AMC = true;
	m_pHWCommon->SetTargetPos();
}
/**
@brief   목적지  설정 해제 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void Driving::SetTargetPosRelease()
{
	m_bTargetPos2AMC = false;
	m_pHWCommon->SetTargetPosRelease();
}

/**
@brief   목적지  설정 우무 하는 함수
@author  ehoto
@date    2012-11-28
*/
bool Driving::IsTargetPos()
{
	return m_bTargetPos2AMC;
}

/**
@brief   목적지  설정 해제 요청하는 함수
@author  puting
@date    2018-01-28
*/
void Driving::SetFirstNodeCountAdd()
{
	m_pHWCommon->SetFirstNodeCountAdd();
}

/**
@brief   목적지  설정 해제 요청하는 함수
@author  puting
@date    2018-01-28
*/
void Driving::SetVHLDetectIgnor()
{
	m_pHWCommon->SetVHLDetectIgnor();
}

/**
@brief   목적지  설정 해제 요청하는 함수
@author  puting
@date    2018-01-28
*/
void Driving::SetVHLDetectIgnorRelease()
{
	m_pHWCommon->SetVHLDetectIgnorRelease();
}

//AOHC-285
void Driving::SetLowVibration()
{
	m_pHWCommon->SetLowVibration();
}

//AOHC-285
void Driving::SetLowVibrationRelease()
{
	m_pHWCommon->SetLowVibrationRelease();
}

/**
@brief   대차감지시 감속도 변경
@author  puting
@date    2018-01-28
*/
void Driving::SetDetectStopDecelDown()
{
	m_pHWCommon->SetDetectStopDecelDown();
}

/**
@brief   대차감지시 감속도 변경 해제
@author  puting
@date    2018-01-28
*/
void Driving::SetDetectStopDecelDownRelease()
{
	m_pHWCommon->SetDetectStopDecelDownRelease();
}
/**
@brief   대차감지시 패턴 강제 설정
@author  puting
@date    2019-07-24
*/
void Driving::SetVHLDetectIOSet(bool bIoSet)
{
	m_pHWCommon->SetVHLDetectIOSet(bIoSet);
}
/**
@brief   AOHC-196 사전 로그 추가
@author  kds
@date    2019-03-22
*/
int Driving::GetFollowing_Axis1()
{
	m_pHWCommon->GetFollowing_Axis1();
}
/**
@brief   주행부 NodeCount 의 정보를 받는 함수
@author  ehoto
@date    2012-11-28
@param   nNodeCount : Node Count를 돌려 받는 변수
@param   dNodeDetectPosition : Node 감지된 위치를 돌려 받는 변수
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int Driving::GetNodeCountInfo(USER_NODE_CHECK_DATA &UserNodeCheckData)
{
	int nTmpReturn = 0;
	NODE_CHECK_DATA TmpNodeCheckData;
	memset(&TmpNodeCheckData, 0, sizeof(NODE_CHECK_DATA));
	nTmpReturn = m_pHWCommon->GetNodeCountInfo(TmpNodeCheckData);
	UserNodeCheckData.NodeCountOfPreCheck= TmpNodeCheckData.NodeCountOfPreCheck;
	UserNodeCheckData.NodeCount = TmpNodeCheckData.NodeCount;
	UserNodeCheckData.PositionOfDrivingTagPreCheck = m_pPropotionalConverter->calcCts2User( (double)TmpNodeCheckData.PositionOfDrivingTagPreCheck );	// Cts --> UserUnit
	UserNodeCheckData.PositionOfDrivingTag = m_pPropotionalConverter->calcCts2User( (double)TmpNodeCheckData.PositionOfDrivingTag );	// Cts --> UserUnit
	UserNodeCheckData.PositionOfTransTagFirst = m_pPropotionalConverter->calcCts2User( (double)TmpNodeCheckData.PositionOfTransTagFirst );	// Cts --> UserUnit
	UserNodeCheckData.PositionOfTransTagFinal = m_pPropotionalConverter->calcCts2User( (double)TmpNodeCheckData.PositionOfTransTagFinal );	// Cts --> UserUnit
	return nTmpReturn;
}

/**
@brief   DetectSensor의 정보를 설정하는 함수
@author  ehoto
@date    2013-01-22
@param   DetectInfo : DetectSensor 의 값을 설정하기 휘한 정보를 가지고 있는 구조체
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int Driving::SetDetectInfo(USER_DETECT_INFO UserDetectInfo)
{
	DETECT_INFO DetectInfo;
	double dTmpDistSumOfStraightPath	= m_pPropotionalConverter->calcUser2Cts(UserDetectInfo.DistSumOfStraightPath);
	DetectInfo.UseFollowControlFlag		= UserDetectInfo.UseFollowControlFlag;
#if(USE_CHANGENODE == 1)
	DetectInfo.OHTDetectType	   		= UserDetectInfo.OHTDetectType;
	DetectInfo.OBSDetectType	   		= UserDetectInfo.OBSDetectType;
	DetectInfo.NodeCount				= UserDetectInfo.NodeCount;
	DetectInfo.DistSumOfStraightPath	= (int)((dTmpDistSumOfStraightPath<INT_MAX)?dTmpDistSumOfStraightPath:INT_MAX);
	DetectInfo.FollowCommandType		= UserDetectInfo.FollowCommandType;
	DetectInfo.PBSIgnoreFlag			= UserDetectInfo.PBSIgnoreFlag;

	// for change node (jhoun.lim)
	OPTIONAL_NODE_INFO OptNodeInfo;
	OptNodeInfo.NodeType = UserDetectInfo.NodeType;
	OptNodeInfo.DistON = m_pPropotionalConverter->calcUser2Cts(UserDetectInfo.DistON);
	OptNodeInfo.DistAfterON = m_pPropotionalConverter->calcUser2Cts(UserDetectInfo.DistAfterON);
	OptNodeInfo.OHTDetectTypeOpt = UserDetectInfo.OHTDetectTypeOpt;
	OptNodeInfo.OBSDetectTypeOpt = UserDetectInfo.OBSDetectTypeOpt;

	return m_pHWCommon->SetDetectInfo(DetectInfo, OptNodeInfo);
#else
	DetectInfo.Type						= UserDetectInfo.Type;
	DetectInfo.NodeCount				= UserDetectInfo.NodeCount;
	DetectInfo.DistSumOfStraightPath	= (int)((dTmpDistSumOfStraightPath<INT_MAX)?dTmpDistSumOfStraightPath:INT_MAX);
	DetectInfo.FollowCommandType		= UserDetectInfo.FollowCommandType;
	DetectInfo.PBSIgnoreFlag			= UserDetectInfo.PBSIgnoreFlag;

	return m_pHWCommon->SetDetectInfo(DetectInfo);
#endif
}

/**
@brief   CompensateSlavePos 

	뒷바퀴를 앞바퀴 엔코더 값으로 강제로 맞추는 함수
	전후륜 싸움 방지 목적 (특히 소량 이동 시)
@author  임태웅
@date    2013-03-12
*/
int Driving::CompensateSlavePos()
{
	return m_pHWCommon->CompensateSlavePos();
}

void Driving::SetCurveEstop(bool value)
{
	m_Curve_E_Stop = value;
}

bool Driving::GetCurveEstop(void)
{
	return m_Curve_E_Stop;
}


