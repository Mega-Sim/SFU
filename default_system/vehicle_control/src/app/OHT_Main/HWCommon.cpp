/**
@file    HWCommon.cpp
@version OHT 7.0
@brief   HWCommon Class CPP File
*/

#include "HWCommon.h"
#include "LogHandler.h"
#include "OHTDetect.h"
#include "DiagManager.h"
#include "OHTMain.h"

#define ERR_NO_REAL_HW	-1
#define ADD_MD_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\MonitoringDetail.log", __VA_ARGS__) //AOHC-253 Monitoring 항목 추가 2

/**
 @brief   HWCommon Class 생성자
 @author  //
 @date    //
 @return  //
 */
HWCommon::HWCommon()
{
	m_nAmcErrCode = 0;
#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	m_defualtparam = OHTMainForm->GetDefaultParam();
#endif
	Open();
	UpdateData();	// 2013.10.16, 임태웅, 생성 시 H/W 정보를 받아오는데 목적이 있음.
	IsPauseControl = false;
}

/**
 @brief   HWCommon Class 소멸자
 @author  //
 @date    //
 @return  //
 */
HWCommon::~HWCommon() 
{
	Close();
}


/**
 @brief   HWCommon Open 하는 함수
 @author	임태웅
 @date	2013.02.17
 @return	H/W Mask class의 포인터 
 */
HwMask* HWCommon::GetMask(void)
{
	HwMask *pMask = NULL;
	if(m_pMask!=NULL)
	{
		pMask = m_pMask;
	}
	return pMask;
}

//AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현
bool HWCommon::IsSerialOpen()
{
#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))

	if(m_defualtparam->bSOM6869 ==true)
		return m_pMotionController->g_bSerialDriverOpen;
	else
		return m_pMultiSerialLib->g_bSerialDriverOpen;
#else
 	return m_pMultiSerialLib->g_bSerialDriverOpen;
#endif

}

/**
 @brief   HWCommon Open 하는 함수
 @author  //
 @date    //
 @return  //
 */
int HWCommon::Open()
{
	//Memory Leak때문에 추가
    m_pMotionController = NULL;
	if (m_pMotionController != NULL) {
		delete m_pMotionController;
		// m_pMotionController == NULL 인경우 InitializeCriticalSection(&m_CS);이 안된 상태에서
		// CriticalSection을 사용하면서 문제됨.
	}
	m_pMotionController = new AMCLibrary();
#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	if(m_defualtparam->bSOM6869 ==false)
		m_pMultiSerialLib = new MultiSerialLibrary();
#else
		m_pMultiSerialLib = new MultiSerialLibrary();
#endif
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V86))
	m_pMask = new HwMask(96, 96, 5);
#else
	m_pMask = new HwMask(96, 96, 4);
#endif
	char szCOM[16] = "";

	return NO_ERR;
}

/**
 @brief   HWCommon 닫는 함수
 @author  //
 @date    //
 @return  에러 //
 */
int HWCommon::Close() 
{

	if (m_pMask != NULL) {
		delete m_pMask;
		m_pMask = NULL;
	}
#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	if ((m_defualtparam->bSOM6869 ==false) && (m_pMultiSerialLib != NULL)) {
		delete m_pMultiSerialLib;
		m_pMultiSerialLib = NULL;
	}
#else
	if (m_pMultiSerialLib != NULL) {
		delete m_pMultiSerialLib;
		m_pMultiSerialLib = NULL;
	}
#endif

	if (m_pMotionController != NULL) {
		m_pMotionController->AMCClose();
		delete m_pMotionController;
		m_pMotionController = NULL;
	}

	return NO_ERR;
}

/**
 @brief   IO의 해당 Output을 On/Off 시킬 때 사용하는 함수
 @author  ehoto
 @date    2012.10.29
 @param   index : DEF_HW.h에 구현된 ENUM 변수 OUTPUT 사용(On/Off 하고자 하는 번호 기재)
 @param   OnOff : 해당 출력은 On 하려면 True, Off 하려면 false.
 @return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
 */
int HWCommon::SetBit(int nIndex, bool bOnOff, bool bOBSControl, bool bUBGControl) {
	int nError = 0;

	if(nIndex > NUMBER_OF_OUTPUT) return nError;  // ignore over NUMBER_OF_OUTPUT Outputs   OHT_NAME_MASK:72

	nError = m_pMotionController->SetBit(nIndex, bOnOff, bOBSControl, bUBGControl);

	return nError;
}

/**
 @brief   HWCommon의 MotionController 초기화 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
int HWCommon::Init(int nAxisNum) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;
	return m_pMotionController->Init(nAxisNum);
}

/**
 @brief   HWCommon의 MotionController 초기화 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
int HWCommon::SetSync(int nMasterAxisNum, int nSlaveAxisNum, double dSyncGain) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;
	return m_pMotionController->SetSync(nMasterAxisNum, nSlaveAxisNum,
		dSyncGain);
}


/**
 @brief   HWCommon의 MotionController 초기화 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
int HWCommon::SetMaxSWLimitForReset(int nAxisNum, double dPosLimit,
	double dNegLimit) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;
	return m_pMotionController->SetMaxSWLimitForReset(nAxisNum, dPosLimit,
		dNegLimit);
}

/// 상태 확인
/**
 @brief   HWCommon의 MotionController 동작 확인 하는 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
bool HWCommon::IsRunning(int nAxisNum) 
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return false;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);
	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->Running.bUse==true)
		{
			return pMaskOfAxis->Running.Value;
		}
	}

	return m_pMotionController->IsRunning(nAxisNum);
}

/**
 @brief   HWCommon의 MotionController 정위치 확인 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
bool HWCommon::IsInPosition(int nAxisNum) {
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->IsInPosition(nAxisNum);
}

/**
 @brief   HWCommon의 MotionController AMP On/Off 상태 확인 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호(0~)
 @return  Error Code
 */
bool HWCommon::IsAMPEnable(int nAxisNum) 
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return false;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);
	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->AmpEnable.bUse==true)
		{
			return pMaskOfAxis->AmpEnable.Value;
		}
	}

	return m_pMotionController->IsAMPEnable(nAxisNum);
}

/**
 @brief   HWCommon의 MotionController 현재 위치 확인 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
double HWCommon::GetCurrentPosition(int nAxisNum) 
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);
	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->Position.bUse==true)
		{
			return pMaskOfAxis->Position.Value;
		}
	}

	return m_pMotionController->GetCurrentPosition(nAxisNum);
}

/**
 @brief   HWCommon의 MotionController 현재 이전 위치 확인 함수
 @author  doori.shin
 @date    2016.04.26
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
double HWCommon::GetPrePosition(int nAxisNum)
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);
	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->Position.bUse==true)
		{
			return pMaskOfAxis->Position.Value;
		}
	}

	return m_pMotionController->GetPrePosition(nAxisNum);
}

/**
@brief   HWCommon의 MotionController 현재까지 주행한 거리를 리턴
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : 축 번호
@return  Error Code
*/
double HWCommon::GetTotalPosition(int nAxisNum)
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);

	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->Position.bUse==true)
		{
			return pMaskOfAxis->Position.Value;
		}
	}

	return m_pMotionController->GetTotalPosition(nAxisNum);
}

/**
@brief   HWCommon의 MotionController 현재까지 주행한 거리를 초기화
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : 축 번호
*/
void HWCommon::ResetTotalPosition(int nAxisNum)
{
	if(m_pMotionController != NULL)
		m_pMotionController->ResetTotalPosition(nAxisNum);
}

/**
@brief   HWCommon의 MotionController 현재까지 수행한 명령 횟수를 리턴
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : 축 번호
@return  Error Code
*/
int HWCommon::GetTotalMoveCount(int nAxisNum)
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);

	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->Position.bUse==true)
		{
			return pMaskOfAxis->Position.Value;
		}
	}

	return m_pMotionController->GetTotalMoveCount(nAxisNum);
}

/**
@brief   HWCommon의 MotionController 현재까지 수행한 명령횟수 초기화
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : 축 번호
*/
void HWCommon::ResetTotalMoveCount(int nAxisNum)
{
	if(m_pMotionController != NULL)
		m_pMotionController->ResetTotalMoveCount(nAxisNum);
}


/**
 @brief   HWCommon의 MotionController Small Add Move 시 센서가 감지된 위치 확인 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Small Add Move 시 센서가 감지된 위치
 */
double HWCommon::GetSmallAddCheckPosition(int nAxisNum)
{
//	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	return m_pMotionController->GetSmallAddCheckPosition(nAxisNum);
}


/**
 @brief   HWCommon의 MotionController 현재 위치 확인 함수
 @author  ehoto
 @date	  2012-11-01
 @param   nAxisNum : 축 번호
 @return  Small Add Move 시 센서가 감지된 영역을 표시함
 */
double HWCommon::GetFinalPosition(int nAxisNum)
{
//	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	return m_pMotionController->GetFinalPosition(nAxisNum);
}


/**
 @brief   HWCommon의 MotionController 현재 속도 확인 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
double HWCommon::GetCurrentVelocity(int nAxisNum) {
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);

	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->Speed.bUse==true)
		{
			return pMaskOfAxis->Speed.Value;
		}
	}


	return m_pMotionController->GetCurrentVelocity(nAxisNum);
}

/**
 @brief   HWCommon의 MotionController Following Error 확인 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
double HWCommon::GetFollowingError(int nAxisNum) {
	if (m_pMotionController == NULL)
		return 0.0;

	return m_pMotionController->GetFollowingError(nAxisNum);
}

/// 에러 관련
/**
 @brief   HWCommon의 MotionController Error 확인 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
int HWCommon::GetError(int nAxisNum) 
{
	if(m_nAmcErrCode != 0)	// AMC 관련 에러 먼저 리턴
		return m_nAmcErrCode;

	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);
	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->ErrorCode.bUse==true)
		{
			return pMaskOfAxis->ErrorCode.Value;
		}
	}

	return m_pMotionController->GetError(nAxisNum);
}
//AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현
bool HWCommon::IsAMCDllOpen()
{
	return m_pMotionController->IsAMCDllOpen();
}
/**
 @brief   HWCommon의 MotionController reset 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
int HWCommon::Reset(int nAxisNum)
{
	m_nAmcErrCode = 0;

	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;
	return m_pMotionController->Reset(nAxisNum);
}

/// Offset 관련
/**
 @brief   HWCommon의 MotionController 현재 위치 Home 설정 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
int HWCommon::SetHome(int nAxisNum) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->SetHome(nAxisNum);
}

/**
 @brief   HWCommon의 MotionController Offset 설정 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dDistance : Offset 거리
 @return  Error Code
 */
int HWCommon::SetOffset(int nAxisNum, double dDistance) {
	if (m_pMotionController == NULL)
		return -1;

	return m_pMotionController->SetOffset(nAxisNum, dDistance);
}

/**
 @brief   HWCommon의 MotionController Offset 받아오는 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dDistance : Offset 거리
 @return  Error Code
 */
int HWCommon::GetOffset(int nAxisNum, double *dDistance) {
	if (m_pMotionController == NULL)
		return -1;

	return m_pMotionController->GetOffset(nAxisNum, dDistance);
}

/// Limit 관련
/**
 @brief   HWCommon의 MotionController Postive S/W Limit 설정 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dLimit : Limit 거리
 @return  Error Code
 */
int HWCommon::SetPositiveSWLimit(int nAxisNum, double dLimit) {
	if (m_pMotionController == NULL)
		return -1;

	return m_pMotionController->SetPositiveSWLimit(nAxisNum, dLimit);
}

/**
 @brief   HWCommon의 MotionController Negative S/W Limit 설정 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dLimit : Limit 거리
 @return  Error Code
 */
int HWCommon::SetNegativeSWLimit(int nAxisNum, double dLimit) {
	if (m_pMotionController == NULL)
		return -1;

	return m_pMotionController->SetNegativeSWLimit(nAxisNum, dLimit);
}

/**
 @brief   HWCommon의 MotionController S/W Limit 확인 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dPosLimit : Positive Limit 거리
 @param   dNegLimit : Negative Limit 거리
 @return  Error Code
 */
int HWCommon::GetSWLimit(int nAxisNum, double& dPosLimit, double& dNegLimit) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->GetSWLimit(nAxisNum, dPosLimit, dNegLimit);
}

/**
 @brief   HWCommon의 MotionController S/W Limit 해제 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @return  Error Code
 */
int HWCommon::ResetSWLimit(int nAxisNum) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->ResetSWLimit(nAxisNum);
}

/// 구동 관련 - 단위: 거리(mm), 속도(mm/msec), 가속도(mm/msec2), 시간(msec)
/**
 @brief   HWCommon의 MotionController Amp On/Off 설정 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   bValue : On/Off 값
 @return  Error Code
 */
int HWCommon::SetAMPEnable(int nAxisNum, bool bValue) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->SetAMPEnable(nAxisNum, bValue);
}

/// 구동 관련 - 단위: 거리(mm), 속도(mm/msec), 가속도(mm/msec2), 시간(msec)
/**
 @brief   HWCommon의 MotionController Amp On/Off 설정 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   bValue : On/Off 값
 @return  Error Code
 */
int HWCommon::GetMoveDSAxisNum() {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->GetMoveDSAxisNum();
}

/// 구동 관련 - 단위: 거리(mm), 속도(mm/msec), 가속도(mm/msec2), 시간(msec)
/**
 @brief   HWCommon의 MotionController Amp On/Off 설정 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   bValue : On/Off 값
 @return  Error Code
 */
int HWCommon::SetForceAMPEnable(int nAxisNum, bool bValue) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->SetForceAMPEnable(nAxisNum, bValue);
}

/**
 @brief   HWCommon의 MotionController Interrupt Check 함수
 @author  ehoto
 @date    2012-11-01
 @return  true/false
 */
bool HWCommon::InterruptCntCheck() {
	if (m_pMotionController == NULL)
		return false;

	return m_pMotionController->InterruptCntCheck();
}


// 가속도 기반
/**
 @brief   HWCommon의 MotionController 양의 방향으로 이동하는 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dAccel : 가속도
 @param   dVel : 속도
 @return  Error Code
 */
int HWCommon::MovePositive(int nAxisNum, double dAccel, double dVel) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MovePositive(nAxisNum, dAccel, dVel);
}

/**
 @brief   HWCommon의 MotionController 음의 방향으로 이동하는 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dAccel : 가속도
 @param   dVel : 속도
 @return  Error Code
 */
int HWCommon::MoveNegative(int nAxisNum, double dAccel, double dVel) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveNegative(nAxisNum, dAccel, dVel);
}

/**
 @brief   HWCommon의 MotionController 정지하는 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dAccel : 감속도
 @return  Error Code
 */
int HWCommon::MoveStop(int nAxisNum, double dAccel) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveStop(nAxisNum, dAccel);
}

/**
 @brief   HWCommon의 MotionController 거리를 가속도/감속도/속도 프로파일로 이동하는 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dAccel_Up : 가속도
 @param   dAccel_Down : 감속도
 @param   dVel : 속도
 @param   dDistance : 이동 거리
 @return  Error Code
 */
int HWCommon::MoveDistance(int nAxisNum, double dAccel_Up, double dAccel_Down,
	double dVel, double dDistance, TRIGGER_CTRL_INFO TriggerCtrlInfo) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveDistance(nAxisNum, dAccel_Up, dAccel_Down,
		dVel, dDistance, TriggerCtrlInfo);
}

// 가속 시간 기반
/**
 @brief   HWCommon의 MotionController 일정 시간 가속하여 양의 방향으로 이동하는 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dwTime : 가속 시간
 @param   dVel : 속도
 @return  Error Code
 */
int HWCommon::MovePositive(int nAxisNum, DWORD dwTime, double dVel) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MovePositive(nAxisNum, dwTime, dVel);
}

/**
 @brief   HWCommon의 MotionController 일정 시간 가속하여 음의 방향으로 이동하는 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dwTime : 가속 시간
 @param   dVel : 속도
 @return  Error Code
 */
int HWCommon::MoveNegative(int nAxisNum, DWORD dwTime, double dVel) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveNegative(nAxisNum, dwTime, dVel);
}

/**
 @brief   HWCommon의 MotionController 일정 시간 감속하여 정지하는 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dwTime : 감속 시간
 @param   dVel : 속도
 @return  Error Code
 */
int HWCommon::MoveStop(int nAxisNum, DWORD dwTime) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveStop(nAxisNum, dwTime);
}

/**
 @brief   HWCommon의 MotionController 거리를 가속시간/감속시간/속도 프로파일로 이동하는 함수
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   dwTime_Up : 가속 시간
 @param   dwTime_Down : 감속 시간
 @param   dVel : 속도
 @param   dDistance : 이동 거리
 @return  Error Code
 */
int HWCommon::MoveDistance(int nAxisNum, DWORD dwTime_Up, DWORD dwTime_Down,
	double dVel, double dDistance) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveDistance(nAxisNum, dwTime_Up, dwTime_Down,
		dVel, dDistance);
}

// 통신으로 제어시에서 구현 시 사용 함수
/**
 @brief   HWCommon의 MotionController 에 프로토콜로 명령 전달
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : 축 번호
 @param   pSendData : 보내는 Data 가 들어있는 Char 배열
 @param   nSendLen : 보내는 Data 길이
 @param   pRecvData : 받는 Data 가 들어있는 Char 배열
 @param   nRecvLen : 받는 Data 길이
 @return  Error Code
 */
int HWCommon::SendData(int nAxisNum, char* pSendData, int nSendLen,
	char* pRecvData, int* nRecvLen) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;
	return m_pMotionController->SendData(nAxisNum, pSendData, nSendLen,
		pRecvData, nRecvLen);
}


// 통신으로 제어시에서 구현 시 사용 함수
/**
 @brief   HWCommon의 MotionController 의 정보 Update
 @author  ehoto
 @date    2012-11-07
 @return  Error Code
 */
void HWCommon::UpdateData(void)
{
	int nInput[4] = {0, }, nOutput[4] = {0, };
	int nInputCount = 0, nOutCount = 0;
	unsigned int nInData[4][32] = {0, };
	unsigned int nOutData[4][32] = {0, };

	MASK_INPUT	*pMaskOfInput = NULL;
    
	if (m_pMotionController == NULL)
		return;

	int nErrCode = 0;
	bool bResult = m_pMotionController->UpdateData(nInput, nInputCount, nOutput, nOutCount, &nErrCode);

	// AMC의 UpdateData 실패하는 경우는 AMC에서 Invalidate 패킷을 받은 경우이다.
	// 이럴 경우 받은 패킷은 버려야 하기 때문에 IO 갱신하지 않고 return 한다.
	if(bResult == false)
	{
		if(nErrCode != 0)
			m_nAmcErrCode = nErrCode;
		return;
	}



	// Test
//	static nIndex = 0;
//	bool bOnOff = false;
//	bOnOff = (rand() % 2);
//	m_pMotionController->SetBit(nIndex, bOnOff);
//	nIndex++;
//	if (nIndex == 64) {
//		nIndex = 0;
//	}

	for (int i = 0; i < nInputCount; i++)
	{
		for (int j = 0; j < 32; j++)
		{

			nInData[i][j] = (nInput[i] >> j) & 0x01;

			// Value Masking
			pMaskOfInput = m_pMask->getMaskOfInput( (i*32+j) );
			if(pMaskOfInput!=NULL)
			{
				if(pMaskOfInput->bUse==true)
				{
					nInData[i][j] = (pMaskOfInput->Value==true)?ON:OFF;
				}
			}
//			pMaskOfInput = NULL;
		}
	}

#if(OHT_NAME == OHT_NAME_STD_V80)
	m_Input.HAND_Inposition = nInData[0][0];
	m_Input.HAND_Alarm = nInData[0][1];
	m_Input.HAND_Right_Grip = nInData[0][2];
	m_Input.HAND_Left_Grip = nInData[0][3];
	m_Input.HAND_Close = nInData[0][4];
	m_Input.HAND_Open = nInData[0][5];
	m_Input.HAND_Push = nInData[0][6];

	m_Input.HOIST_Home = nInData[0][7];

	m_Input.HAND_PIO_Go = nInData[0][8];
	m_Input.EQPIO_Load_REQ = nInData[0][9];
	m_Input.EQPIO_Unload_REQ = nInData[0][10];
	m_Input.EQPIO_Ready = nInData[0][11];
	m_Input.EQPIO_HO_AVBL = nInData[0][12];
	m_Input.EQPIO_ES = nInData[0][13];
	m_Input.EQPIO_Right_Go = nInData[0][14];
	m_Input.EQPIO_Left_Go = nInData[0][15];
	m_Input.STEER_F_Left = nInData[0][16];
	m_Input.STEER_R_Left = nInData[0][17];
	m_Input.STEER_F_Right = nInData[0][18];
	m_Input.STEER_R_Right = nInData[0][19];
	m_Input.SHUTTER_F_Close = nInData[0][20];
	m_Input.SHUTTER_R_Close = nInData[0][21];
	m_Input.SHUTTER_F_Open = nInData[0][22];
	m_Input.SHUTTER_R_Open = nInData[0][23];
	m_Input.SHUTTER_F_Alarm = nInData[0][24];
	m_Input.SHUTTER_R_Alarm = nInData[0][25];
	m_Input.SHUTTER_F_Inposition = nInData[0][26];
	m_Input.SHUTTER_R_Inposition = nInData[0][27];
	m_Input.OBSTACLE_Long = nInData[0][28];
	m_Input.OBSTACLE_Middle = nInData[0][29];
	m_Input.OBSTACLE_Short = nInData[0][30];
	m_Input.OBSTACLE_Fail = nInData[0][31];

	m_Input.OHTDETECT_Long = nInData[1][0];
	m_Input.OHTDETECT_Middle = nInData[1][1];
	m_Input.OHTDETECT_Short = nInData[1][2];
	m_Input.OHTDETECT_Fail = nInData[1][3];
	m_Input.LOOKDOWN_Detect = nInData[1][4];
	m_Input.LOOKDOWN_Error = nInData[1][5];
	m_Input.SHIFT_Home = nInData[1][6];
	m_Input.STB_Detect_Left = nInData[1][7];
	m_Input.STB_Detect_Right = nInData[1][8];
	m_Input.UTB_Detect = nInData[1][9];
	m_Input.FOUP_Detect = nInData[1][10];
	m_Input.OSCILLATION = nInData[1][11];
	m_Input.EMO_Press = nInData[1][12];
	m_Input.BUMPER_Press = nInData[1][13];
	m_Input.FAN_Fail = nInData[1][14];
	m_Input.HOIST_Belt_Detect2 = nInData[1][15];
	m_Input.TP_Enable = nInData[1][16];
	m_Input.HANDTP_Connect = nInData[1][17];
	m_Input.FOUP_Cover_Open_Detect = nInData[1][18];
	m_Input.STOP_Driving_Front = nInData[1][19];
	m_Input.STOP_Driving_PreCheck = nInData[1][20];
	m_Input.STOP_Trans_Front = nInData[1][21];
	m_Input.STOP_Trans_Rear = nInData[1][22];
	m_Input.BCR_Driving_OK = nInData[1][23];
	m_Input.BCR_Driving_NG = nInData[1][24];
	m_Input.BCR_Trans_OK = nInData[1][25];
	m_Input.BCR_Trans_NG = nInData[1][26];
	m_Input.HOIST_Belt_Detect1 = nInData[1][27];
	m_Input.PIO_Input3 = nInData[1][28];
	m_Input.PIO_Input5 = nInData[1][29];
	m_Input.PIO_Input6 = nInData[1][30];
	m_Input.SMPS_Short = nInData[1][31];
	//TG 추가
	m_Input.CID_Link_Compt_IN = nInData[2][0];
	m_Input.CID_Interlock_IN = nInData[2][1];
//	m_Input.CID_Pass_OK_IN = nInData[2][2];
	m_Input.CID_Completion_ACK = nInData[2][2];
	m_Input.CID_Status_IN = nInData[2][3];
	m_Input.CID_NoRegistration = nInData[2][4];
	m_Input.CID_Reserved_IN2 = nInData[2][5];
	m_Input.Rotate_0_Sensor = nInData[2][6];
	m_Input.Rotate_90_Sensor = nInData[2][7];
	m_Input.Rotate_180_Sensor = nInData[2][8];
	m_Input.Rotate_270_Sensor = nInData[2][9];

	m_Input.Reverved_74 = nInData[2][10];

	m_Input.Reverved_75 = nInData[2][11];
	m_Input.Reverved_76 = nInData[2][12];
	m_Input.Reverved_77 = nInData[2][13];
	m_Input.Reverved_78 = nInData[2][14];
	m_Input.Reverved_79 = nInData[2][15];

#elif(OHT_NAME == OHT_NAME_STD_V81)
	m_Input.HAND_Inposition = nInData[0][0];
	m_Input.HAND_Alarm = nInData[0][1];
	m_Input.HAND_Right_Grip = nInData[0][2];
	m_Input.HAND_Left_Grip = nInData[0][3];
	m_Input.HAND_Close = nInData[0][4];
	m_Input.HAND_Open = nInData[0][5];
	m_Input.HAND_Push = nInData[0][6];

	m_Input.HOIST_Home = nInData[0][7];

	m_Input.HAND_PIO_Go = nInData[0][8];
	m_Input.EQPIO_Load_REQ = nInData[0][9];
	m_Input.EQPIO_Unload_REQ = nInData[0][10];
	m_Input.EQPIO_Ready = nInData[0][11];
	m_Input.EQPIO_HO_AVBL = nInData[0][12];
	m_Input.EQPIO_ES = nInData[0][13];
	m_Input.EQPIO_Right_Go = nInData[0][14];
	m_Input.EQPIO_Left_Go = nInData[0][15];
	m_Input.STEER_F_Left = nInData[0][16];
	m_Input.STEER_R_Left = nInData[0][17];
	m_Input.STEER_F_Right = nInData[0][18];
	m_Input.STEER_R_Right = nInData[0][19];
	m_Input.SHUTTER_F_Close = nInData[0][20];
	m_Input.SHUTTER_R_Close = nInData[0][21];
	m_Input.SHUTTER_F_Open = nInData[0][22];
	m_Input.SHUTTER_R_Open = nInData[0][23];
	m_Input.SHUTTER_F_Alarm = nInData[0][24];
	m_Input.SHUTTER_R_Alarm = nInData[0][25];
	m_Input.SHUTTER_F_Inposition = nInData[0][26];
	m_Input.SHUTTER_R_Inposition = nInData[0][27];
	m_Input.OBSTACLE_Long = nInData[0][28];
	m_Input.OBSTACLE_Middle = nInData[0][29];
	m_Input.OBSTACLE_Short = nInData[0][30];
	m_Input.OBSTACLE_Fail = nInData[0][31];

	m_Input.OHTDETECT_Long = nInData[1][0];
	m_Input.OHTDETECT_Middle = nInData[1][1];
	m_Input.OHTDETECT_Short = nInData[1][2];
	m_Input.OHTDETECT_Fail = nInData[1][3];
	m_Input.LOOKDOWN_Detect = nInData[1][4];
	m_Input.LOOKDOWN_Error = nInData[1][5];
	m_Input.SHIFT_Home = nInData[1][6];
	m_Input.STB_Detect_Left = nInData[1][7];
	m_Input.STB_Detect_Right = nInData[1][8];
	m_Input.UTB_Detect = nInData[1][9];
	m_Input.FOUP_Detect = nInData[1][10];
	m_Input.OSCILLATION = nInData[1][11];
	m_Input.EMO_Press = nInData[1][12];
	m_Input.BUMPER_Press = nInData[1][13];
	m_Input.FAN_Fail = nInData[1][14];
	m_Input.HOIST_Belt_Detect2 = nInData[1][15];
	m_Input.TP_Enable = nInData[1][16];
	m_Input.HANDTP_Connect = nInData[1][17];
	m_Input.FOUP_Cover_Open_Detect = nInData[1][18];
	m_Input.STOP_Driving_Front = nInData[1][19];
	m_Input.STOP_Driving_PreCheck = nInData[1][20];
	m_Input.STOP_Trans_Front = nInData[1][21];
	m_Input.STOP_Trans_Rear = nInData[1][22];
	m_Input.BCR_Driving_OK = nInData[1][23];
	m_Input.BCR_Driving_NG = nInData[1][24];
	m_Input.BCR_Trans_OK = nInData[1][25];
	m_Input.BCR_Trans_NG = nInData[1][26];
	m_Input.HOIST_Belt_Detect1 = nInData[1][27];
	m_Input.PIO_Input3 = nInData[1][28];
	m_Input.PIO_Input5 = nInData[1][29];
	m_Input.PIO_Input6 = nInData[1][30];
	m_Input.SMPS_Short = nInData[1][31];
	//TG 추가
	m_Input.CID_Link_Compt_IN = nInData[2][0];
	m_Input.CID_Interlock_IN = nInData[2][1];
//	m_Input.CID_Pass_OK_IN = nInData[2][2];
	m_Input.CID_Completion_ACK = nInData[2][2];
	m_Input.CID_Status_IN = nInData[2][3];
	m_Input.CID_NoRegistration = nInData[2][4];
	m_Input.CID_Reserved_IN2 = nInData[2][5];
	m_Input.Rotate_0_Sensor = nInData[2][6];
	m_Input.Rotate_90_Sensor = nInData[2][7];
	m_Input.Rotate_180_Sensor = nInData[2][8];
	m_Input.Rotate_270_Sensor = nInData[2][9];

	m_Input.BlackBox_Error = nInData[2][10];

	m_Input.Reverved_75 = nInData[2][11];
	m_Input.Reverved_76 = nInData[2][12];
	m_Input.Reverved_77 = nInData[2][13];
	m_Input.Reverved_78 = nInData[2][14];
	m_Input.Reverved_79 = nInData[2][15];

#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))	
	{
		m_Input.HAND_Inposition = nInData[0][0];
		m_Input.HAND_Alarm = nInData[0][1];
		m_Input.HAND_Right_Grip = nInData[0][2];
		m_Input.HAND_Left_Grip = nInData[0][3];
		m_Input.HAND_Close = nInData[0][4];
		m_Input.HAND_Open = nInData[0][5];
		m_Input.HAND_Push = nInData[0][6];
	    m_Input.Hand_Reserved7 = nInData[0][7];
		m_Input.HAND_PIO_Go = nInData[0][8];
		m_Input.EQPIO_Load_REQ = nInData[0][9];
		m_Input.EQPIO_Unload_REQ = nInData[0][10];
		m_Input.EQPIO_Ready = nInData[0][11];
		m_Input.EQPIO_HO_AVBL = nInData[0][12];
		m_Input.EQPIO_ES = nInData[0][13];
		m_Input.EQPIO_Right_Go = nInData[0][14];
		m_Input.EQPIO_Left_Go = nInData[0][15];
		m_Input.STEER_F_Left = nInData[0][16];
		m_Input.STEER_R_Left = nInData[0][17];
		m_Input.STEER_F_Right = nInData[0][18];
		m_Input.STEER_R_Right = nInData[0][19];
		m_Input.SHUTTER_F_Close = nInData[0][20];
		m_Input.SHUTTER_R_Close = nInData[0][21];
		m_Input.SHUTTER_F_Open = nInData[0][22];
		m_Input.SHUTTER_R_Open = nInData[0][23];
		m_Input.SHUTTER_F_Alarm = nInData[0][24];
		m_Input.SHUTTER_R_Alarm = nInData[0][25];
		m_Input.SHUTTER_F_Inposition = nInData[0][26];
		m_Input.SHUTTER_R_Inposition = nInData[0][27];
		m_Input.OBSTACLE_Long = nInData[0][28];
		m_Input.OBSTACLE_Middle = nInData[0][29];
		m_Input.OBSTACLE_Short = nInData[0][30];
		m_Input.OBSTACLE_Fail = nInData[0][31];

		m_Input.OHTDETECT_Long = nInData[1][0];
		m_Input.OHTDETECT_Middle = nInData[1][1];
		m_Input.OHTDETECT_Short = nInData[1][2];
		m_Input.OHTDETECT_Fail = nInData[1][3];
		m_Input.LOOKDOWN_Detect = nInData[1][4];
		m_Input.LOOKDOWN_Error = nInData[1][5];
		m_Input.SHIFT_Home = nInData[1][6];
		m_Input.Reserved_40 = nInData[1][7];         //AOHC-286
		m_Input.Reserved_41 = nInData[1][8];          //AOHC-286
		m_Input.Reserved_42 = nInData[1][9];          //AOHC-286
		m_Input.FOUP_Detect = nInData[1][10];
		m_Input.OSCILLATION = nInData[1][11];
		m_Input.EMO_Press = nInData[1][12];
		m_Input.BUMPER_Press = nInData[1][13];
		m_Input.FAN_Fail = nInData[1][14];
		m_Input.Reserved_48 = nInData[1][15];       //AOHC-286
		m_Input.Reserved_49 = nInData[1][16];       //AOHC-286
		m_Input.HANDTP_Connect = nInData[1][17];
		m_Input.Reserved_51 = nInData[1][18];       //AOHC-286
		m_Input.STOP_Driving_Front = nInData[1][19];
		m_Input.STOP_Driving_PreCheck = nInData[1][20];
		m_Input.STOP_Trans_Front = nInData[1][21];
		m_Input.STOP_Trans_Rear = nInData[1][22];
		m_Input.BCR_Driving_OK = nInData[1][23];
		m_Input.BCR_Driving_NG = nInData[1][24];
		m_Input.BCR_Trans_OK = nInData[1][25];
		m_Input.BCR_Trans_NG = nInData[1][26];
		m_Input.HOIST_Belt_Detect1 = nInData[1][27];
		m_Input.PIO_Input3 = nInData[1][28];
		m_Input.PIO_Input5 = nInData[1][29];
		m_Input.PIO_Input6 = nInData[1][30];
		m_Input.SMPS_Short = nInData[1][31];
		//TG 추가
		m_Input.CID_Link_Compt_IN = nInData[2][0];
		m_Input.CID_Interlock_IN = nInData[2][1];
		m_Input.CID_Completion_ACK = nInData[2][2];
		m_Input.CID_Status_IN = nInData[2][3];
		m_Input.CID_NoRegistration = nInData[2][4];
		m_Input.CID_Reserved_IN2 = nInData[2][5];
		m_Input.Rotate_0_Sensor		= nInData[2][6];
		m_Input.Rotate_90_Sensor	= nInData[2][7];
		m_Input.Rotate_180_Sensor 	= nInData[2][8];
		m_Input.Rotate_270_Sensor 	= nInData[2][9];
		m_Input.HOIST_Home = nInData[2][10];
		m_Input.Reverved_75 = nInData[2][11];    //AOHC-286
		m_Input.Reverved_76 = nInData[2][12];    //AOHC-286
		m_Input.Reverved_77 = nInData[2][13];    //AOHC-286
		m_Input.Reverved_78 = nInData[2][14];    //AOHC-286
		m_Input.Reverved_79 = nInData[2][15];    //AOHC-286
	}

#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	{
		// 0 - 15
		m_Input.HAND_Inposition = nInData[0][0];
		m_Input.HAND_Alarm 		= nInData[0][1];
		m_Input.HAND_Right_Grip = nInData[0][2];
		m_Input.HAND_Left_Grip 	= nInData[0][3];
		
		m_Input.HAND_Close 		= nInData[0][4];		
		m_Input.HAND_Open 		= nInData[0][5];		// 5
		m_Input.HAND_Push 		= nInData[0][6];
	    m_Input.Hand_Reserved7 	= nInData[0][7];      //AOHC-286
		
		m_Input.HAND_PIO_Go 	= nInData[0][8];
		m_Input.EQPIO_Load_REQ 	= nInData[0][9];		
		m_Input.EQPIO_Unload_REQ= nInData[0][10];		// 10
		m_Input.EQPIO_Ready 	= nInData[0][11];
		
		m_Input.EQPIO_HO_AVBL 	= nInData[0][12];
		m_Input.EQPIO_ES 		= nInData[0][13];
		m_Input.EQPIO_Right_Go 	= nInData[0][14];
		m_Input.EQPIO_Left_Go 	= nInData[0][15];		// 15

		// 16 - 31
		m_Input.STEER_F_Left 	= nInData[0][16];
		m_Input.STEER_R_Left 	= nInData[0][17];
		m_Input.STEER_F_Right 	= nInData[0][18];
		m_Input.STEER_R_Right 	= nInData[0][19];
		
		m_Input.SHUTTER_F_Close = nInData[0][20];		// 20
		m_Input.SHUTTER_R_Close = nInData[0][21];		
		m_Input.SHUTTER_F_Open 	= nInData[0][22];
		m_Input.SHUTTER_R_Open 	= nInData[0][23];		// 25
		
		m_Input.SHUTTER_F_Alarm = nInData[0][24];		
		m_Input.SHUTTER_R_Alarm = nInData[0][25];		
		m_Input.SHUTTER_F_Inposition 	= nInData[0][26];
		m_Input.SHUTTER_R_Inposition 	= nInData[0][27];	
		
		m_Input.OBSTACLE_Long 	= nInData[0][28];		// 30
		m_Input.OBSTACLE_Middle = nInData[0][29];
		m_Input.OBSTACLE_Short 	= nInData[0][30];
		m_Input.OBSTACLE_Fail 	= nInData[0][31];	

		// 32 - 47	
		m_Input.OHTDETECT_Long 	= nInData[1][0];
		m_Input.OHTDETECT_Middle= nInData[1][1];		
		m_Input.OHTDETECT_Short = nInData[1][2];		
		m_Input.OHTDETECT_Fail 	= nInData[1][3];
		
		m_Input.LOOKDOWN_Detect = nInData[1][4];
		m_Input.LOOKDOWN_Error 	= nInData[1][5];		
		m_Input.SHIFT_Home 		= nInData[1][6];		
		m_Input.Reserved_40 		= nInData[1][7];    //AOHC-286
		
		m_Input.Reserved_41 		= nInData[1][8];    //AOHC-286
		m_Input.Reserved_42 		= nInData[1][9];	//AOHC-286
		m_Input.FOUP_Detect 	= nInData[1][10];		
		m_Input.OSCILLATION 	= nInData[1][11];	
		
		m_Input.EMO_Press 		= nInData[1][12];		
		m_Input.BUMPER_Press 	= nInData[1][13];
		m_Input.FAN_Fail 		= nInData[1][14];
		m_Input.Reserved_48 		= nInData[1][15];     //AOHC-286

		// 48 - 63		
		m_Input.Reserved_49 		= nInData[1][16];	  //AOHC-286
		m_Input.HANDTP_Connect 	= nInData[1][17];
		m_Input.Reserved_51 		= nInData[1][18];     //AOHC-286
		m_Input.STOP_Driving_Front 		= nInData[1][19];
		
		m_Input.STOP_Driving_PreCheck 	= nInData[1][20];		
		m_Input.STOP_Trans_Front= nInData[1][21];		
		m_Input.STOP_Trans_Rear = nInData[1][22];
		m_Input.BCR_Driving_OK 	= nInData[1][23];
		
		m_Input.BCR_Driving_NG 	= nInData[1][24];
		m_Input.BCR_Trans_OK 	= nInData[1][25];		
		m_Input.BCR_Trans_NG 		= nInData[1][26];	// 60
		m_Input.HOIST_Belt_Detect1 	= nInData[1][27];
		
		m_Input.PIO_Input3 			= nInData[1][28];
		m_Input.PIO_Input5 			= nInData[1][29];
		m_Input.PIO_Input6 			= nInData[1][30];
		m_Input.SMPS_Short 			= nInData[1][31];

		// 64 - 79	
		m_Input.CID_Link_Compt_IN 	= nInData[2][0];
		m_Input.CID_Interlock_IN 	= nInData[2][1];		
		m_Input.CID_Completion_ACK 	= nInData[2][2];
		m_Input.CID_Status_IN 		= nInData[2][3];
		
		m_Input.CID_NoRegistration 	= nInData[2][4];	// 70
		m_Input.CID_Reserved_IN2 	= nInData[2][5];
		m_Input.Rotate_0_Sensor		= nInData[2][6];
		m_Input.Rotate_90_Sensor	= nInData[2][7];

		m_Input.Rotate_180_Sensor 	= nInData[2][8];
		m_Input.Rotate_270_Sensor 	= nInData[2][9];
		m_Input.HOIST_Home 			= nInData[2][10];	// 75
		m_Input.SHUTTER_F_Middle_Close = nInData[2][11];
		
		m_Input.SHUTTER_R_Middle_Close = nInData[2][12];	
		m_Input.POD_EUV_1 			= nInData[2][13];
		m_Input.POD_EUV_2 			= nInData[2][14];
		m_Input.Reverved_79			 = nInData[2][15];     //AOHC-286
	}


#elif(OHT_NAME == OHT_NAME_STD_V86)
	{
		//10개
		m_Input.HAND_Inposition 			= nInData[0][0];
		m_Input.HAND_Alarm 					= nInData[0][1];
		m_Input.HAND_Right_Grip 			= nInData[0][2];
		m_Input.HAND_Left_Grip 				= nInData[0][3];
		m_Input.HAND_Close 					= nInData[0][4];
		m_Input.HAND_Open 					= nInData[0][5];
		m_Input.HAND_Push 					= nInData[0][6];
		m_Input.HNAD_Reserved_1 			= nInData[0][7];
		m_Input.HNAD_ES 					= nInData[0][8];
		m_Input.HAND_PIO_Go 				= nInData[0][9];

		//10개
		m_Input.EQPIO_Load_REQ 				= nInData[0][10];
		m_Input.EQPIO_Unload_REQ		 	= nInData[0][11];
		m_Input.EQPIO_Ready 				= nInData[0][12];
		m_Input.EQPIO_HO_AVBL 				= nInData[0][13];
		m_Input.EQPIO_ES 					= nInData[0][14];
		m_Input.EQPIO_Right_Go 				= nInData[0][15];
		m_Input.EQPIO_Left_Go 				= nInData[0][16];
		m_Input.STEER_F_Left 				= nInData[0][17];
		m_Input.STEER_R_Left 				= nInData[0][18];
		m_Input.STEER_F_Right 				= nInData[0][19];

		//10개
		m_Input.STEER_R_Right 				= nInData[0][20];
		m_Input.SHUTTER_F_Close 			= nInData[0][21];
		m_Input.SHUTTER_R_Close 			= nInData[0][22];
		m_Input.SHUTTER_F_Open 				= nInData[0][23];
		m_Input.SHUTTER_R_Open 				= nInData[0][24];
		m_Input.SHUTTER_F_Alarm 			= nInData[0][25];
		m_Input.SHUTTER_R_Alarm 			= nInData[0][26];
		m_Input.SHUTTER_F_Inposition 		= nInData[0][27];
		m_Input.SHUTTER_R_Inposition 		= nInData[0][28];
		m_Input.OBSTACLE_Input_Extend 		= nInData[0][29];

		//10개
		m_Input.OBSTACLE_Long 				= nInData[0][30];
		m_Input.OBSTACLE_Middle 			= nInData[0][31];
		m_Input.OBSTACLE_Short 				= nInData[1][0];
		m_Input.OBSTACLE_Fail 				= nInData[1][1];
		m_Input.OHTDETECT_Input_Extend 		= nInData[1][2];
		m_Input.OHTDETECT_Long 				= nInData[1][3];
		m_Input.OHTDETECT_Middle		 	= nInData[1][4];
		m_Input.OHTDETECT_Short 			= nInData[1][5];
		m_Input.OHTDETECT_Fail 				= nInData[1][6];
		m_Input.LOOKDOWN_Detect 			= nInData[1][7];

		//10개
		m_Input.LOOKDOWN_Error 				= nInData[1][8];
		m_Input.SHIFT_Home 					= nInData[1][9];
		m_Input.STB_Detect_Left 			= nInData[1][10];
		m_Input.STB_Detect_Right 			= nInData[1][11];
		m_Input.UTB_Detect 					= nInData[1][12];
		m_Input.FOUP_Detect 				= nInData[1][13];
		m_Input.OSCILLATION 				= nInData[1][14];
		m_Input.EMO_Press 					= nInData[1][15];
		m_Input.BUMPER_Press 				= nInData[1][16];
		m_Input.FAN_Fail 					= nInData[1][17];

		//10개
		m_Input.HOIST_Belt_Detect2 			= nInData[1][18];
		m_Input.Reserved_51 				= nInData[1][19];
		m_Input.HANDTP_Connect 				= nInData[1][20];
		m_Input.POD_EUV_1		 			= nInData[1][21];
		m_Input.STOP_Driving_Front 			= nInData[1][22];
		m_Input.STOP_Driving_PreCheck 		= nInData[1][23];
		m_Input.STOP_Trans_Front 			= nInData[1][24];
		m_Input.STOP_Trans_Rear			 	= nInData[1][25];
		m_Input.BCR_Driving_OK 				= nInData[1][26];
		m_Input.BCR_Driving_NG 				= nInData[1][27];

		//10개
		m_Input.BCR_Trans_OK 				= nInData[1][28];
		m_Input.BCR_Trans_NG 				= nInData[1][29];
		m_Input.HOIST_Belt_Detect1 			= nInData[1][30];
		m_Input.PIO_Input3 					= nInData[1][31];
		m_Input.PIO_Input5 					= nInData[2][0];
		m_Input.PIO_Input6 					= nInData[2][1];
		m_Input.SMPS_Short 					= nInData[2][2];
		m_Input.CID_Link_Compt_IN 			= nInData[2][3];
		m_Input.CID_Interlock_IN 			= nInData[2][4];
		m_Input.CID_Completion_ACK 			= nInData[2][5];

		//10개
		m_Input.CID_Status_IN 				= nInData[2][6];
		m_Input.CID_NoRegistration 			= nInData[2][7];
		m_Input.CID_Reserved_IN2 			= nInData[2][8];
		m_Input.HOIST_Home					= nInData[2][9];
		m_Input.Blackbox_DI2			 	= nInData[2][10];
		m_Input.Blackbox_DI1 				= nInData[2][11];
		m_Input.Blackbox_DI0 				= nInData[2][12];
		m_Input.Rotate_0_Sensor				= nInData[2][13];
		m_Input.Rotate_90_Sensor			= nInData[2][14];
		m_Input.Rotate_180_Sensor 			= nInData[2][15];

		//10개
		m_Input.Rotate_270_Sensor 			= nInData[2][16];
		m_Input.SHUTTER_F_Middle_Close 		= nInData[2][17];
		m_Input.SHUTTER_R_Middle_Close 		= nInData[2][18];
		m_Input.SPARE_7					 	= nInData[2][19];
		m_Input.POD_EUV_2 				 	= nInData[2][20];
		m_Input.SPARE_9					 	= nInData[2][21];
		m_Input.SPARE_10				 	= nInData[2][22];
		m_Input.SPARE_11				 	= nInData[2][23];
		m_Input.SPARE_12				 	= nInData[2][24];
		m_Input.SPARE_13				 	= nInData[2][25];

		//10개
		m_Input.SPARE_14				 	= nInData[2][26];
		m_Input.SPARE_15				 	= nInData[2][27];
		m_Input.SPARE_16				 	= nInData[2][28];
		m_Input.SPARE_17				 	= nInData[2][29];
		m_Input.SPARE_18				 	= nInData[2][30];
		m_Input.SPARE_19				 	= nInData[2][31];

	}
#elif(OHT_NAME == OHT_NAME_STD_V85)
	{
		// 0 - 15
		m_Input.HAND_Inposition = nInData[0][0];
		m_Input.HAND_Alarm 		= nInData[0][1];
		m_Input.HAND_Right_Grip = nInData[0][2];
		m_Input.HAND_Left_Grip 	= nInData[0][3];

		m_Input.HAND_Close 		= nInData[0][4];
		m_Input.HAND_Open 		= nInData[0][5];		// 5
		m_Input.HAND_Push 		= nInData[0][6];
		m_Input.HNAD_Reserved_1 	= nInData[0][7];

		m_Input.HNAD_ES 	= nInData[0][8];

		m_Input.HAND_PIO_Go 	= nInData[0][9];
		m_Input.EQPIO_Load_REQ 	= nInData[0][10];
		m_Input.EQPIO_Unload_REQ= nInData[0][11];		// 10
		m_Input.EQPIO_Ready 	= nInData[0][12];

		m_Input.EQPIO_HO_AVBL 	= nInData[0][13];
		m_Input.EQPIO_ES 		= nInData[0][14];
		m_Input.EQPIO_Left_Go 	= nInData[0][15];		// 15

		// 16 - 31
		m_Input.STEER_F_Left 	= nInData[0][16];
		m_Input.STEER_R_Left 	= nInData[0][17];
		m_Input.STEER_F_Right 	= nInData[0][18];
		m_Input.STEER_R_Right 	= nInData[0][19];

		m_Input.SHUTTER_F_Close = nInData[0][20];		// 20
		m_Input.SHUTTER_R_Close = nInData[0][21];
		m_Input.SHUTTER_F_Open 	= nInData[0][22];
		m_Input.SHUTTER_R_Open 	= nInData[0][23];		// 25

		m_Input.SHUTTER_F_Alarm = nInData[0][24];
		m_Input.SHUTTER_R_Alarm = nInData[0][25];
		m_Input.SHUTTER_F_Inposition 	= nInData[0][26];
		m_Input.SHUTTER_R_Inposition 	= nInData[0][27];


		m_Input.OBSTACLE_Input_Extend 	= nInData[0][28];		// 30
		m_Input.OBSTACLE_Long 	= nInData[0][29];		// 30
		m_Input.OBSTACLE_Middle = nInData[0][30];
		m_Input.OBSTACLE_Short 	= nInData[0][31];
		// 32 - 47
		m_Input.OBSTACLE_Fail 	= nInData[1][0];
		m_Input.OHTDETECT_Input_Extend 	= nInData[1][1];
		m_Input.OHTDETECT_Long 	= nInData[1][2];
		m_Input.OHTDETECT_Middle= nInData[1][3];
		m_Input.OHTDETECT_Short = nInData[1][4];
		m_Input.OHTDETECT_Fail 	= nInData[1][5];

		m_Input.LOOKDOWN_Detect = nInData[1][6];
		m_Input.LOOKDOWN_Error 	= nInData[1][7];
		m_Input.SHIFT_Home 		= nInData[1][8];
		m_Input.STB_Detect_Left 		= nInData[1][9];

		m_Input.STB_Detect_Right 		= nInData[1][10];
		m_Input.UTB_Detect 		= nInData[1][11];
		m_Input.FOUP_Detect 	= nInData[1][12];
		m_Input.OSCILLATION 	= nInData[1][13];

		m_Input.EMO_Press 		= nInData[1][14];
		m_Input.BUMPER_Press 	= nInData[1][15];
		m_Input.FAN_Fail 		= nInData[1][16];
		m_Input.HOIST_Belt_Detect1 		= nInData[1][17];
		m_Input.HOIST_Belt_Detect2 		= nInData[1][18];
		// 48 - 63
		m_Input.TP_Enable 		= nInData[1][19];		// 50
		m_Input.HANDTP_Connect 	= nInData[1][20];
		m_Input.FOUB_Cover_Detect 		= nInData[1][21];
		m_Input.STOP_Driving_Front 		= nInData[1][22];

		m_Input.STOP_Driving_PreCheck 	= nInData[1][23];
		m_Input.STOP_Trans_Front= nInData[1][24];
		m_Input.STOP_Trans_Rear = nInData[1][25];
		m_Input.BCR_Driving_OK 	= nInData[1][26];

		m_Input.BCR_Driving_NG 	= nInData[1][27];
		m_Input.BCR_Trans_OK 	= nInData[1][28];
		m_Input.BCR_Trans_NG 		= nInData[1][29];	// 60

		m_Input.Reserved_1 			= nInData[1][30];
		m_Input.Reserved_2 			= nInData[1][31];

		// 64 - 79
		m_Input.Reserved_3 			= nInData[2][0];
		m_Input.SMPS_Short 			= nInData[2][1];


		m_Input.CID_Link_Compt_IN 	= nInData[2][2];
		m_Input.CID_Interlock_IN 	= nInData[2][3];
		m_Input.CID_Completion_ACK 	= nInData[2][4];
		m_Input.CID_Status_IN 		= nInData[2][5];

		m_Input.CID_NoRegistration 	= nInData[2][6];	// 70
		m_Input.CID_Reserved_IN2 	= nInData[2][7];
		m_Input.Rotate_0_Sensor		= nInData[2][8];
		m_Input.Rotate_90_Sensor	= nInData[2][9];

		m_Input.Rotate_180_Sensor 	= nInData[2][10];
		m_Input.Rotate_270_Sensor 	= nInData[2][11];
		m_Input.HOIST_Home 			= nInData[2][12];	// 75
		m_Input.HOIST_Home_2 			= nInData[2][13];	// 75
		m_Input.SHUTTER_F_Middle_Close = nInData[2][14];

		m_Input.SHUTTER_R_Middle_Close = nInData[2][15];
		m_Input.POD_EUV_1 			= nInData[2][16];
		m_Input.POD_EUV_2 			= nInData[2][17];
		m_Input.SPARE_5				= nInData[2][18];

		// 80 - 89
		m_Input.SPARE_6 			= nInData[2][19];
		m_Input.SPARE_7 			= nInData[2][20];
		m_Input.SPARE_8				= nInData[2][21];
		m_Input.SPARE_9				= nInData[2][22];

		m_Input.SPARE_10			= nInData[2][23];
		m_Input.SPARE_11			= nInData[2][24];	// 85
		m_Input.SPARE_12			= nInData[2][25];
		m_Input.SPARE_13			= nInData[2][26];

		m_Input.SPARE_14			= nInData[2][27];
		m_Input.SPARE_15			= nInData[2][28];
		m_Input.SPARE_16			= nInData[2][29];	// 90
		m_Input.SPARE_17			= nInData[2][30];
		m_Input.SPARE_18			= nInData[2][31];

	}
#elif(OHT_NAME == OHT_NAME_STD_V81S)
	{
		// 0 - 15
		m_Input.HAND_Inposition = nInData[0][0];
		m_Input.HAND_Alarm 		= nInData[0][1];
		m_Input.HAND_Right_Grip = nInData[0][2];
		m_Input.HAND_Left_Grip 	= nInData[0][3];

		m_Input.HAND_Close 		= nInData[0][4];
		m_Input.HAND_Open 		= nInData[0][5];		// 5
		m_Input.HAND_Push 		= nInData[0][6];
		m_Input.HOIST_Home 	= nInData[0][7];

		m_Input.HNAD_ES 	= nInData[0][8];

		m_Input.HAND_PIO_Go 	= nInData[0][9];
		m_Input.EQPIO_Load_REQ 	= nInData[0][10];
		m_Input.EQPIO_Unload_REQ= nInData[0][11];		// 10
		m_Input.EQPIO_Ready 	= nInData[0][12];

		m_Input.EQPIO_HO_AVBL 	= nInData[0][13];
		m_Input.EQPIO_ES 		= nInData[0][14];
		m_Input.EQPIO_Left_Go 	= nInData[0][15];		// 15

		// 16 - 31
		m_Input.STEER_F_Left 	= nInData[0][16];
		m_Input.STEER_R_Left 	= nInData[0][17];
		m_Input.STEER_F_Right 	= nInData[0][18];
		m_Input.STEER_R_Right 	= nInData[0][19];

		m_Input.SHUTTER_F_Close = nInData[0][20];		// 20
		m_Input.SHUTTER_R_Close = nInData[0][21];
		m_Input.SHUTTER_F_Open 	= nInData[0][22];
		m_Input.SHUTTER_R_Open 	= nInData[0][23];		// 25

		m_Input.SHUTTER_F_Alarm = nInData[0][24];
		m_Input.SHUTTER_R_Alarm = nInData[0][25];
		m_Input.SHUTTER_F_Inposition 	= nInData[0][26];
		m_Input.SHUTTER_R_Inposition 	= nInData[0][27];


		m_Input.OBSTACLE_Input_Extend 	= nInData[0][28];		// 30
		m_Input.OBSTACLE_Long 	= nInData[0][29];		// 30
		m_Input.OBSTACLE_Middle = nInData[0][30];
		m_Input.OBSTACLE_Short 	= nInData[0][31];
        // 32 - 47
		m_Input.OBSTACLE_Fail 	= nInData[1][0];
		m_Input.OHTDETECT_Input_Extend 	= nInData[1][1];
		m_Input.OHTDETECT_Long 	= nInData[1][2];
		m_Input.OHTDETECT_Middle= nInData[1][3];
		m_Input.OHTDETECT_Short = nInData[1][4];
		m_Input.OHTDETECT_Fail 	= nInData[1][5];

		m_Input.LOOKDOWN_Detect = nInData[1][6];
		m_Input.LOOKDOWN_Error 	= nInData[1][7];
		m_Input.SHIFT_Home 		= nInData[1][8];
		m_Input.STB_Detect_Left 		= nInData[1][9];

		m_Input.STB_Detect_Right 		= nInData[1][10];
		m_Input.UTB_Detect 		= nInData[1][11];
		m_Input.FOUP_Detect 	= nInData[1][12];
		m_Input.OSCILLATION 	= nInData[1][13];

		m_Input.EMO_Press 		= nInData[1][14];
		m_Input.BUMPER_Press 	= nInData[1][15];
		m_Input.FAN_Fail 		= nInData[1][16];
		m_Input.HOIST_Belt_Detect1 		= nInData[1][17];
        m_Input.HOIST_Belt_Detect2 		= nInData[1][18];
		// 48 - 63
		m_Input.TP_Enable 		= nInData[1][19];		// 50
		m_Input.HANDTP_Connect 	= nInData[1][20];
		m_Input.FOUP_Cover_Open_Detect 		= nInData[1][21];
		m_Input.STOP_Driving_Front 		= nInData[1][22];

		m_Input.STOP_Driving_PreCheck 	= nInData[1][23];
		m_Input.STOP_Trans_Front= nInData[1][24];
		m_Input.STOP_Trans_Rear = nInData[1][25];
		m_Input.BCR_Driving_OK 	= nInData[1][26];

		m_Input.BCR_Driving_NG 	= nInData[1][27];
		m_Input.BCR_Trans_OK 	= nInData[1][28];
		m_Input.BCR_Trans_NG 		= nInData[1][29];	// 60

		m_Input.Reserved_1 			= nInData[1][30];
		m_Input.Reserved_2 			= nInData[1][31];

		// 64 - 79
		m_Input.Reserved_3 			= nInData[2][0];
		m_Input.SMPS_Short 			= nInData[2][1];


		m_Input.CID_Link_Compt_IN 	= nInData[2][2];
		m_Input.CID_Interlock_IN 	= nInData[2][3];
		m_Input.CID_Completion_ACK 	= nInData[2][4];
		m_Input.CID_Status_IN 		= nInData[2][5];

		m_Input.CID_NoRegistration 	= nInData[2][6];	// 70
		m_Input.CID_Reserved_IN2 	= nInData[2][7];
		m_Input.Rotate_0_Sensor		= nInData[2][8];
		m_Input.Rotate_90_Sensor	= nInData[2][9];

		m_Input.Rotate_180_Sensor 	= nInData[2][10];
		m_Input.Rotate_270_Sensor 	= nInData[2][11];
		m_Input.HNAD_Reserved_1 			= nInData[2][12];	// 75
		m_Input.HOIST_Home_2 			= nInData[2][13];	// 75
		m_Input.SHUTTER_F_Middle_Close = nInData[2][14];

		m_Input.SHUTTER_R_Middle_Close = nInData[2][15];
		m_Input.POD_EUV_1 			= nInData[2][16];
		m_Input.POD_EUV_2 			= nInData[2][17];
		m_Input.SPARE_5				= nInData[2][18];

		// 80 - 89
		m_Input.SPARE_6 			= nInData[2][19];
		m_Input.SPARE_7 			= nInData[2][20];
		m_Input.SPARE_8				= nInData[2][21];
		m_Input.SPARE_9				= nInData[2][22];

		m_Input.SPARE_10			= nInData[2][23];
		m_Input.SPARE_11			= nInData[2][24];	// 85
		m_Input.SPARE_12			= nInData[2][25];
		m_Input.SPARE_13			= nInData[2][26];

		m_Input.SPARE_14			= nInData[2][27];
		m_Input.SPARE_15			= nInData[2][28];
		m_Input.SPARE_16			= nInData[2][29];	// 90
		m_Input.SPARE_17			= nInData[2][30];

		m_Input.SPARE_18			= nInData[2][31];

	}
#elif(OHT_NAME == OHT_NAME_STD_V85S)
	{
		// 0 - 15
		m_Input.HAND_Inposition 		= nInData[0][0];
		m_Input.HAND_Alarm 				= nInData[0][1];
		m_Input.HAND_Right_Grip 		= nInData[0][2];
		m_Input.HAND_Left_Grip 			= nInData[0][3];

		m_Input.HAND_Close 				= nInData[0][4];
		m_Input.HAND_Open 				= nInData[0][5];		// 5
		m_Input.HAND_Push 				= nInData[0][6];
		m_Input.HNAD_Reserved_1 		= nInData[0][7];

		m_Input.HNAD_ES 				= nInData[0][8];

		m_Input.HAND_PIO_Go 			= nInData[0][9];
		m_Input.EQPIO_Load_REQ 			= nInData[0][10];
		m_Input.EQPIO_Unload_REQ		= nInData[0][11];		// 10
		m_Input.EQPIO_Ready 			= nInData[0][12];

		m_Input.EQPIO_HO_AVBL 			= nInData[0][13];
		m_Input.EQPIO_ES 				= nInData[0][14];
		m_Input.EQPIO_Right_Go 			= nInData[0][15];		// 15
		m_Input.EQPIO_Left_Go 			= nInData[0][16];		// 15

		// 16 - 31
		m_Input.STEER_F_Left 			= nInData[0][17];
		m_Input.STEER_R_Left 			= nInData[0][18];
		m_Input.STEER_F_Right 			= nInData[0][19];
		m_Input.STEER_R_Right 			= nInData[0][20];

		m_Input.SHUTTER_F_Close 		= nInData[0][21];		// 20
		m_Input.SHUTTER_R_Close 		= nInData[0][22];
		m_Input.SHUTTER_F_Open 			= nInData[0][23];
		m_Input.SHUTTER_R_Open 			= nInData[0][24];		// 25

		m_Input.SHUTTER_F_Alarm 		= nInData[0][25];
		m_Input.SHUTTER_R_Alarm 		= nInData[0][26];
		m_Input.SHUTTER_F_Inposition 	= nInData[0][27];
		m_Input.SHUTTER_R_Inposition 	= nInData[0][28];


		m_Input.OBSTACLE_Input_Extend 	= nInData[0][29];	// 30
		m_Input.OBSTACLE_Long 			= nInData[0][30];		// 30
		m_Input.OBSTACLE_Middle 		= nInData[0][31];

		// 32 - 47
		m_Input.OBSTACLE_Short 			= nInData[1][0];
		m_Input.OBSTACLE_Fail 			= nInData[1][1];        //33
		m_Input.OHTDETECT_Input_Extend 	= nInData[1][2];    //34
		m_Input.OHTDETECT_Long 			= nInData[1][3];                //35
		m_Input.OHTDETECT_Middle		= nInData[1][4];                 //36
		m_Input.OHTDETECT_Short 		= nInData[1][5];                 //37
		m_Input.OHTDETECT_Fail 			= nInData[1][6];                 //38

		m_Input.LOOKDOWN_Detect 		= nInData[1][7];                 //39
		m_Input.LOOKDOWN_Error 			= nInData[1][8];                 //40
		m_Input.SHIFT_Home 				= nInData[1][9];                 //41
		m_Input.STB_Detect_Left 		= nInData[1][10];         		//42

		m_Input.STB_Detect_Right 		= nInData[1][11];        		//43
		m_Input.UTB_Detect 				= nInData[1][12];                //44
		m_Input.FOUP_Detect 			= nInData[1][13];                //45
		m_Input.OSCILLATION 			= nInData[1][14];                //46

		m_Input.EMO_Press 				= nInData[1][15];                //47
		m_Input.BUMPER_Press 			= nInData[1][16];                //48
		m_Input.FAN_Fail 				= nInData[1][17];                //49
		m_Input.HOIST_Belt_Detect2 		= nInData[1][18];        	//50
		m_Input.Reserved_51 			= nInData[1][19];        	 	//51
		// 48 - 63
		m_Input.HANDTP_Connect 			= nInData[1][20];		// 52
		m_Input.FOUP_Cover_Open_Detect 	= nInData[1][21];       //53
		m_Input.STOP_Driving_Front 		= nInData[1][22];   //54
		m_Input.STOP_Driving_PreCheck 	= nInData[1][23];       //55

		m_Input.STOP_Trans_Front 		= nInData[1][24];       //56
		m_Input.STOP_Trans_Rear			= nInData[1][25];               //57
		m_Input.BCR_Driving_OK 			= nInData[1][26];               //58
		m_Input.BCR_Driving_NG 			= nInData[1][27];               //59

		m_Input.BCR_Trans_OK 			= nInData[1][28];               //60
		m_Input.BCR_Trans_NG 			= nInData[1][29];               //61
		m_Input.HOIST_Belt_Detect1 		= nInData[1][30];			// 62

		m_Input.PIO_Input3 				= nInData[1][31];             //63


		// 64 - 79
		m_Input.PIO_Input5 				= nInData[2][0];             //64
		m_Input.PIO_Input6 				= nInData[2][1];             //65
		m_Input.SMPS_Short 				= nInData[2][2];             //66
		m_Input.CID_Link_Compt_IN 		= nInData[2][3];            //67
		m_Input.CID_Interlock_IN 		= nInData[2][4];            //68
		m_Input.CID_Completion_ACK 		= nInData[2][5];            //69
		m_Input.CID_Status_IN 			= nInData[2][6];            //70

		m_Input.CID_NoRegistration 		= nInData[2][7];	        //71
		m_Input.CID_Reserved_IN2 		= nInData[2][8];            //72
		m_Input.HOIST_Home				= nInData[2][9];            //73
		m_Input.Blackbox_DI2			= nInData[2][10];            //74

		m_Input.Blackbox_DI1 			= nInData[2][11];           //75
		m_Input.Blackbox_DI0 			= nInData[2][12];           //76

		m_Input.Rotate_0_Sensor				   	= nInData[2][13];            //73
		m_Input.Rotate_90_Sensor					= nInData[2][14];            //74
		m_Input.Rotate_180_Sensor 				= nInData[2][15];           //75
		m_Input.Rotate_270_Sensor 				= nInData[2][16];           //76
		m_Input.SPARE_5					= nInData[2][17];
		m_Input.SPARE_6 				= nInData[2][18];
		m_Input.SPARE_7					= nInData[2][19];
		m_Input.SPARE_8					= nInData[2][20];
		m_Input.SPARE_9					= nInData[2][21];
		m_Input.SPARE_10				= nInData[2][22];
		m_Input.SPARE_11				= nInData[2][23];
		m_Input.SPARE_12				= nInData[2][24];
		m_Input.SPARE_13				= nInData[2][25];
		m_Input.SPARE_14				= nInData[2][26];
		m_Input.SPARE_15				= nInData[2][27];
		m_Input.SPARE_16				= nInData[2][28];
		m_Input.SPARE_17				= nInData[2][29];
		m_Input.SPARE_18				= nInData[2][30];
		m_Input.SPARE_19				= nInData[2][31];


	}
#elif(OHT_NAME == OHT_NAME_STD_V82)
	{
		// 0 - 15
		m_Input.HAND_Inposition 		= nInData[0][0];
		m_Input.HAND_Alarm 				= nInData[0][1];
		m_Input.HAND_Right_Grip 		= nInData[0][2];
		m_Input.HAND_Left_Grip 			= nInData[0][3];

		m_Input.HAND_Close 				= nInData[0][4];
		m_Input.HAND_Open 				= nInData[0][5];		// 5
		m_Input.HAND_Push 				= nInData[0][6];
		m_Input.HNAD_Reserved_1 		= nInData[0][7];

		m_Input.HNAD_ES 				= nInData[0][8];

		m_Input.HAND_PIO_Go 			= nInData[0][9];
		m_Input.EQPIO_Load_REQ 			= nInData[0][10];
		m_Input.EQPIO_Unload_REQ		= nInData[0][11];		// 10
		m_Input.EQPIO_Ready 			= nInData[0][12];

		m_Input.EQPIO_HO_AVBL 			= nInData[0][13];
		m_Input.EQPIO_ES 				= nInData[0][14];
		m_Input.EQPIO_Right_Go 			= nInData[0][15];		// 15
		m_Input.EQPIO_Left_Go 			= nInData[0][16];		// 15

		// 16 - 31
		m_Input.STEER_F_Left 			= nInData[0][17];
		m_Input.STEER_R_Left 			= nInData[0][18];
		m_Input.STEER_F_Right 			= nInData[0][19];
		m_Input.STEER_R_Right 			= nInData[0][20];

		m_Input.SHUTTER_F_Close 		= nInData[0][21];		// 20
		m_Input.SHUTTER_R_Close 		= nInData[0][22];
		m_Input.SHUTTER_F_Open 			= nInData[0][23];
		m_Input.SHUTTER_R_Open 			= nInData[0][24];		// 25

		m_Input.SHUTTER_F_Alarm 		= nInData[0][25];
		m_Input.SHUTTER_R_Alarm 		= nInData[0][26];
		m_Input.SHUTTER_F_Inposition 	= nInData[0][27];
		m_Input.SHUTTER_R_Inposition 	= nInData[0][28];


		m_Input.OBSTACLE_Input_Extend 	= nInData[0][29];	// 30
		m_Input.OBSTACLE_Long 			= nInData[0][30];		// 30
		m_Input.OBSTACLE_Middle 		= nInData[0][31];

		// 32 - 47
		m_Input.OBSTACLE_Short 			= nInData[1][0];
		m_Input.OBSTACLE_Fail 			= nInData[1][1];        //33
		m_Input.OHTDETECT_Input_Extend 	= nInData[1][2];    //34
		m_Input.OHTDETECT_Long 			= nInData[1][3];                //35
		m_Input.OHTDETECT_Middle		= nInData[1][4];                 //36
		m_Input.OHTDETECT_Short 		= nInData[1][5];                 //37
		m_Input.OHTDETECT_Fail 			= nInData[1][6];                 //38

		m_Input.LOOKDOWN_Detect 		= nInData[1][7];                 //39
		m_Input.LOOKDOWN_Error 			= nInData[1][8];                 //40
		m_Input.SHIFT_Home 				= nInData[1][9];                 //41
		m_Input.STB_Detect_Left 		= nInData[1][10];         		//42

		m_Input.STB_Detect_Right 		= nInData[1][11];        		//43
		m_Input.UTB_Detect 				= nInData[1][12];                //44
		m_Input.FOUP_Detect 			= nInData[1][13];                //45
		m_Input.OSCILLATION 			= nInData[1][14];                //46

		m_Input.EMO_Press 				= nInData[1][15];                //47
		m_Input.BUMPER_Press 			= nInData[1][16];                //48
		m_Input.FAN_Fail 				= nInData[1][17];                //49
		m_Input.HOIST_Belt_Detect2 		= nInData[1][18];        	//50
		m_Input.Reserved_51 			= nInData[1][19];        	 	//51
		// 48 - 63
		m_Input.HANDTP_Connect 			= nInData[1][20];		// 52
		m_Input.FOUP_Cover_Open_Detect 	= nInData[1][21];       //53
		m_Input.STOP_Driving_Front 		= nInData[1][22];   //54
		m_Input.STOP_Driving_PreCheck 	= nInData[1][23];       //55

		m_Input.STOP_Trans_Front 		= nInData[1][24];       //56
		m_Input.STOP_Trans_Rear			= nInData[1][25];               //57
		m_Input.BCR_Driving_OK 			= nInData[1][26];               //58
		m_Input.BCR_Driving_NG 			= nInData[1][27];               //59

		m_Input.BCR_Trans_OK 			= nInData[1][28];               //60
		m_Input.BCR_Trans_NG 			= nInData[1][29];               //61
		m_Input.HOIST_Belt_Detect1 		= nInData[1][30];			// 62

		m_Input.PIO_Input3 				= nInData[1][31];             //63


		// 64 - 79
		m_Input.PIO_Input5 				= nInData[2][0];             //64
		m_Input.PIO_Input6 				= nInData[2][1];             //65
		m_Input.SMPS_Short 				= nInData[2][2];             //66
		m_Input.CID_Link_Compt_IN 		= nInData[2][3];            //67
		m_Input.CID_Interlock_IN 		= nInData[2][4];            //68
		m_Input.CID_Completion_ACK 		= nInData[2][5];            //69
		m_Input.CID_Status_IN 			= nInData[2][6];            //70

		m_Input.CID_NoRegistration 		= nInData[2][7];	        //71
		m_Input.CID_Reserved_IN2 		= nInData[2][8];            //72
		m_Input.HOIST_Home				= nInData[2][9];            //73
		m_Input.Blackbox_DI2			= nInData[2][10];            //74

		m_Input.Blackbox_DI1 			= nInData[2][11];           //75
		m_Input.Blackbox_DI0 			= nInData[2][12];           //76

		m_Input.Rotate_0_Sensor				   	= nInData[2][13];            //73
		m_Input.Rotate_90_Sensor					= nInData[2][14];            //74
		m_Input.Rotate_180_Sensor 				= nInData[2][15];           //75
		m_Input.Rotate_270_Sensor 				= nInData[2][16];           //76
		m_Input.SPARE_5					= nInData[2][17];
		m_Input.SPARE_6 				= nInData[2][18];
		m_Input.SPARE_7					= nInData[2][19];
		m_Input.SPARE_8					= nInData[2][20];
		m_Input.SPARE_9					= nInData[2][21];
		m_Input.SPARE_10				= nInData[2][22];
		m_Input.SPARE_11				= nInData[2][23];
		m_Input.SPARE_12				= nInData[2][24];
		m_Input.SPARE_13				= nInData[2][25];
		m_Input.SPARE_14				= nInData[2][26];
		m_Input.SPARE_15				= nInData[2][27];
		m_Input.SPARE_16				= nInData[2][28];
		m_Input.SPARE_17				= nInData[2][29];
		m_Input.SPARE_18				= nInData[2][30];
		m_Input.SPARE_19				= nInData[2][31];


	}
#elif(OHT_NAME == OHT_NAME_P4_MASK)  	//mgchoi. P4 MaskOHT 용 InputData Update
	{
		m_Input.HAND_Flange_Check = nInData[0][0];
		m_Input.HAND_Alarm = nInData[0][1];
		m_Input.HAND_Right_Grip = nInData[0][2];
		m_Input.HAND_Left_Grip = nInData[0][3];
		m_Input.HAND_Work = nInData[0][4];
		m_Input.HAND_Home = nInData[0][5];
		m_Input.HAND_LoadPosition = nInData[0][6];
		m_Input.HAND_UnloadPosition = nInData[0][7];

		m_Input.HAND_ES = nInData[0][8];
		m_Input.HAND_PIO_Go = nInData[0][9];
		m_Input.EQPIO_Load_REQ = nInData[0][10];
		m_Input.EQPIO_Unload_REQ = nInData[0][11];
		m_Input.EQPIO_Ready = nInData[0][12];
		m_Input.EQPIO_HO_AVBL = nInData[0][13];
		m_Input.EQPIO_ES = nInData[0][14];
		m_Input.EQPIO_Right_Go = nInData[0][15];

		m_Input.EQPIO_Left_Go = nInData[0][16];
		m_Input.STEER_F_Left = nInData[0][17];
		m_Input.STEER_R_Left = nInData[0][18];
		m_Input.STEER_F_Right = nInData[0][19];
		m_Input.STEER_R_Right = nInData[0][20];
		m_Input.SHUTTER_F_Close = nInData[0][21];
		m_Input.ANTIDROP_POD_Placement1 = nInData[0][22];
		m_Input.SHUTTER_F_Open = nInData[0][23];

		m_Input.ANTIDROP_POD_Placement2 = nInData[0][24];
		m_Input.SHUTTER_F_Alarm = nInData[0][25];
		m_Input.Reserved_27 = nInData[0][26];
		m_Input.SHUTTER_F_Inposition = nInData[0][27];
		m_Input.Reserved_29 = nInData[0][28];
		m_Input.Reserved_30 = nInData[0][29];
		m_Input.OBSTACLE_Long = nInData[0][30];
		m_Input.OBSTACLE_Middle = nInData[0][31];

		m_Input.OBSTACLE_Short = nInData[1][0];
		m_Input.OBSTACLE_Fail = nInData[1][1];
		m_Input.Reserved_35 = nInData[1][2];
		m_Input.OHTDETECT_Long = nInData[1][3];
		m_Input.OHTDETECT_Middle = nInData[1][4];
		m_Input.OHTDETECT_Short = nInData[1][5];
		m_Input.OHTDETECT_Fail = nInData[1][6];
		m_Input.LOOKDOWN_Detect = nInData[1][7];

		m_Input.LOOKDOWN_Error = nInData[1][8];
		m_Input.FFU_Fail_Hoist = nInData[1][9];
		m_Input.Reserved_43 = nInData[1][10];
		m_Input.Reserved_44 = nInData[1][11];
		m_Input.UTB_Detect = nInData[1][12];
		m_Input.EUVPod_Detect = nInData[1][13];
		m_Input.OSCILLATION = nInData[1][14];
		m_Input.EMO_Press = nInData[1][15];

		m_Input.BUMPER_Press = nInData[1][16];
		m_Input.FFU_Fail_Frame = nInData[1][17];
		m_Input.Slide_Neg_Limit = nInData[1][18];
		m_Input.Reserved_52 = nInData[1][19];
		m_Input.Reserved_53 = nInData[1][20];
		m_Input.RSP200_Detect = nInData[1][21];
		m_Input.STOP_Driving_Front = nInData[1][22];
		m_Input.STOP_Driving_PreCheck = nInData[1][23];

		m_Input.STOP_Trans_Front = nInData[1][24];
		m_Input.STOP_Trans_Rear = nInData[1][25];
		m_Input.BCR_Driving_OK = nInData[1][26];
		m_Input.BCR_Driving_NG = nInData[1][27];
		m_Input.BCR_Trans_OK = nInData[1][28];
		m_Input.BCR_Trans_NG = nInData[1][29];
		m_Input.Slide_Pos_Limit = nInData[1][30];
		m_Input.Reserved_64  = nInData[1][31];

		m_Input.Reserved_65 = nInData[2][0];
		m_Input.Reserved_66 = nInData[2][1];
		m_Input.SMPS_Short = nInData[2][2];
		m_Input.HOIST_Home = nInData[2][3];
		m_Input.EUVPod_Home = nInData[2][4];
		m_Input.RSP200_Home = nInData[2][5];
		m_Input.RSP150_Home = nInData[2][6];
		m_Input.Reserved_72 = nInData[2][7];

		m_Input.Reserved_73 = nInData[2][8];
		m_Input.SHIFT_Home = nInData[2][9];
		m_Input.Reserved_75 = nInData[2][10];
		m_Input.Reserved_76 = nInData[2][11];
		m_Input.Reserved_77 = nInData[2][12];
		m_Input.FOUP_Detect = nInData[2][13];
		m_Input.RSP150_Detect = nInData[2][14];
		m_Input.ANTIDROP_POD_Placement3 = nInData[2][15];
		m_Input.ANTIDROP_POD_Placement4 = nInData[2][16];
	}
#else
	m_Input.HAND_Inposition = nInData[0][0];
	m_Input.HAND_Alarm = nInData[0][1];
	m_Input.HAND_Right_Grip = nInData[0][2];
	m_Input.HAND_Left_Grip = nInData[0][3];
	m_Input.HAND_Close = nInData[0][4];
	m_Input.HAND_Open = nInData[0][5];
	m_Input.HAND_Push = nInData[0][6];
	m_Input.HOIST_Home = nInData[0][7];
	m_Input.HAND_PIO_Go = nInData[0][8];
	m_Input.EQPIO_Load_REQ = nInData[0][9];
	m_Input.EQPIO_Unload_REQ = nInData[0][10];
	m_Input.EQPIO_Ready = nInData[0][11];
	m_Input.EQPIO_HO_AVBL = nInData[0][12];
	m_Input.EQPIO_ES = nInData[0][13];
	m_Input.EQPIO_Right_Go = nInData[0][14];
	m_Input.EQPIO_Left_Go = nInData[0][15];
	m_Input.STEER_F_Left = nInData[0][16];
	m_Input.STEER_R_Left = nInData[0][17];
	m_Input.STEER_F_Right = nInData[0][18];
	m_Input.STEER_R_Right = nInData[0][19];
	m_Input.SHUTTER_F_Close = nInData[0][20];
	m_Input.SHUTTER_R_Close = nInData[0][21];
	m_Input.SHUTTER_F_Open = nInData[0][22];
	m_Input.SHUTTER_R_Open = nInData[0][23];
	m_Input.SHUTTER_F_Alarm = nInData[0][24];
	m_Input.SHUTTER_R_Alarm = nInData[0][25];
	m_Input.SHUTTER_F_Inposition = nInData[0][26];
	m_Input.SHUTTER_R_Inposition = nInData[0][27];
	m_Input.OBSTACLE_Long = nInData[0][28];
	m_Input.OBSTACLE_Middle = nInData[0][29];
	m_Input.OBSTACLE_Short = nInData[0][30];
	m_Input.OBSTACLE_Fail = nInData[0][31];

	m_Input.OHTDETECT_Long = nInData[1][0];
	m_Input.OHTDETECT_Middle = nInData[1][1];
	m_Input.OHTDETECT_Short = nInData[1][2];
	m_Input.OHTDETECT_Fail = nInData[1][3];
	m_Input.LOOKDOWN_Detect = nInData[1][4];
	m_Input.LOOKDOWN_Error = nInData[1][5];
	m_Input.SHIFT_Home = nInData[1][6];
	m_Input.STB_Detect_Left = nInData[1][7];
	m_Input.STB_Detect_Right = nInData[1][8];
	m_Input.UTB_Detect = nInData[1][9];
	m_Input.FOUP_Detect = nInData[1][10];
	m_Input.OSCILLATION = nInData[1][11];
	m_Input.EMO_Press = nInData[1][12];
	m_Input.BUMPER_Press = nInData[1][13];
	m_Input.FAN_Fail = nInData[1][14];
	m_Input.STB_OHT_Detect_Left = nInData[1][15];
	m_Input.STB_OHT_Detect_Right = nInData[1][16];
	m_Input.HANDTP_Connect = nInData[1][17];
	m_Input.STOP_Driving_PreCheck = nInData[1][18];
	m_Input.MC_Status = nInData[1][19];
	m_Input.STOP_Driving_Front = nInData[1][20];
	m_Input.STOP_Trans_Front = nInData[1][21];
	m_Input.STOP_Trans_Rear = nInData[1][22];
	m_Input.BCR_Driving_OK = nInData[1][23];
	m_Input.BCR_Driving_NG = nInData[1][24];
	m_Input.BCR_Trans_OK = nInData[1][25];
	m_Input.BCR_Trans_NG = nInData[1][26];
	m_Input.OHT_COMM_CD = nInData[1][27];
	m_Input.RailSupport_Home = nInData[1][28];
	m_Input.Railsupport_Work = nInData[1][29];
	m_Input.MAC_Detect = nInData[1][30];
	m_Input.SMPS_Short = nInData[1][31];
#endif

	for (int i = 0; i < nOutCount; i++) {
		for (int j = 0; j < 32; j++) {
			nOutData[i][j] = (nOutput[i] >> j) & 0x01;
		}
	}

#if(OHT_NAME == OHT_NAME_STD_V80)
	m_Output.HAND_Close = nOutData[0][0];
	m_Output.HAND_Open = nOutData[0][1];
	m_Output.HAND_Origin = nOutData[0][2];
	m_Output.HAND_Reset = nOutData[0][3];
	m_Output.HAND_Free = nOutData[0][4];
	m_Output.HAND_Reserved6 = nOutData[0][5];
	m_Output.HAND_Reserved7 = nOutData[0][6];
	m_Output.HAND_Reserved8 = nOutData[0][7];
	m_Output.HAND_PIO_Select = nOutData[0][8];
	m_Output.EQPIO_Valid = nOutData[0][9];
	m_Output.EQPIO_CS_0 = nOutData[0][10];
	m_Output.EQPIO_CS_1 = nOutData[0][11];
	m_Output.EQPIO_CS_2 = nOutData[0][12];
	m_Output.EQPIO_CS_3 = nOutData[0][13];
	m_Output.EQPIO_TR_Req = nOutData[0][14];
	m_Output.EQPIO_Busy = nOutData[0][15];
	m_Output.EQPIO_Complete = nOutData[0][16];
	m_Output.EQPIO_Right_Select = nOutData[0][17];
	m_Output.EQPIO_Left_Select = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 = nOutData[0][19];
	m_Output.STEER_F_TRQ_1 = nOutData[0][20];
	m_Output.STEER_R_TRQ_0 = nOutData[0][21];
	m_Output.STEER_R_TRQ_1 = nOutData[0][22];
	m_Output.STEER_F_Dir = nOutData[0][23];
	m_Output.STEER_R_Dir = nOutData[0][24];
	m_Output.SHUTTER_F_Reset = nOutData[0][25];
	m_Output.SHUTTER_R_Reset = nOutData[0][26];
	m_Output.SHUTTER_F_Close = nOutData[0][27];
	m_Output.SHUTTER_R_Close = nOutData[0][28];
	m_Output.SHUTTER_F_Open = nOutData[0][29];
	m_Output.SHUTTER_R_Open = nOutData[0][30];
	m_Output.SHUTTER_F_Origin = nOutData[0][31];	

	m_Output.SHUTTER_R_Origin = nOutData[1][0];
	m_Output.SHUTTER_F_Free = nOutData[1][1];
	m_Output.SHUTTER_R_Free = nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];
	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][7];
	m_Output.OHTDETECT_Area1_Sel = nOutData[1][8];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][11];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][12];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][13];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][14];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][15];
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][17];
	m_Output.LAMP_Red = nOutData[1][18];

	m_Output.LAMP_Yellow_Right = nOutData[1][19];
	m_Output.LAMP_Green = nOutData[1][20];
	m_Output.HAND_Power_Reset = nOutData[1][21];
	m_Output.BRAKE_Driving_Release = nOutData[1][22];
	m_Output.Bumper_Clear = nOutData[1][23];
	m_Output.SAFETY_Reset = nOutData[1][24];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][25];
	m_Output.QRR_Power = nOutData[1][26];
	m_Output.LAMP_Yellow_Left = nOutData[1][27];
	m_Output.Reserved_60 = nOutData[1][28];
	m_Output.Reserved_61 = nOutData[1][29];
	m_Output.Reserved_62 = nOutData[1][30];
	m_Output.Reserved_63 = nOutData[1][31];

	//CID output 추가
	m_Output.CID_Left_Select_ON = nOutData[2][0];
	m_Output.CID_Right_Select_ON = nOutData[2][1];
	m_Output.CID_Occupancy = nOutData[2][2];
//	m_Output.CID_PASS_Compt_OUT = nOutData[2][3];
	m_Output.CID_Pass_Completion = nOutData[2][3];
	m_Output.CID_Abnormal = nOutData[2][4];
	m_Output.CID_Operation_Mode = nOutData[2][5];
	m_Output.CID_Common_Tag = nOutData[2][6];
	m_Output.CID_LinkRequest = nOutData[2][7];
	m_Output.Reserved_72 = nOutData[2][8];      	//AOHC-286
	m_Output.Reserved_73 = nOutData[2][9];         	//AOHC-286
	m_Output.Reserved_74 = nOutData[2][10];         	//AOHC-286
	m_Output.Reserved_75 = nOutData[2][11];         	//AOHC-286
	m_Output.Reserved_76 = nOutData[2][12];          	//AOHC-286
	m_Output.Reserved_77 = nOutData[2][13];          	//AOHC-286
	m_Output.Reserved_78 = nOutData[2][14];          	//AOHC-286
	m_Output.Reserved_79 = nOutData[2][15];             	//AOHC-286

#elif(OHT_NAME == OHT_NAME_STD_V81)
	m_Output.HAND_Close = nOutData[0][0];
	m_Output.HAND_Open = nOutData[0][1];
	m_Output.HAND_Origin = nOutData[0][2];
	m_Output.HAND_Reset = nOutData[0][3];
	m_Output.HAND_Free = nOutData[0][4];
	m_Output.HAND_Reserved6 = nOutData[0][5];
	m_Output.HAND_Reserved7 = nOutData[0][6];
	m_Output.HAND_Reserved8 = nOutData[0][7];
	m_Output.HAND_PIO_Select = nOutData[0][8];
	m_Output.EQPIO_Valid = nOutData[0][9];
	m_Output.EQPIO_CS_0 = nOutData[0][10];
	m_Output.EQPIO_CS_1 = nOutData[0][11];
	m_Output.EQPIO_CS_2 = nOutData[0][12];
	m_Output.EQPIO_CS_3 = nOutData[0][13];
	m_Output.EQPIO_TR_Req = nOutData[0][14];
	m_Output.EQPIO_Busy = nOutData[0][15];
	m_Output.EQPIO_Complete = nOutData[0][16];
	m_Output.BlackBox_Output = nOutData[0][17];
	m_Output.EQPIO_Left_Select = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 = nOutData[0][19];
	m_Output.STEER_F_TRQ_1 = nOutData[0][20];
	m_Output.STEER_R_TRQ_0 = nOutData[0][21];
	m_Output.STEER_R_TRQ_1 = nOutData[0][22];
	m_Output.STEER_F_Dir = nOutData[0][23];
	m_Output.STEER_R_Dir = nOutData[0][24];
	m_Output.SHUTTER_F_Reset = nOutData[0][25];
	m_Output.SHUTTER_R_Reset = nOutData[0][26];
	m_Output.SHUTTER_F_Close = nOutData[0][27];
	m_Output.SHUTTER_R_Close = nOutData[0][28];
	m_Output.SHUTTER_F_Open = nOutData[0][29];
	m_Output.SHUTTER_R_Open = nOutData[0][30];
	m_Output.SHUTTER_F_Origin = nOutData[0][31];

	m_Output.SHUTTER_R_Origin = nOutData[1][0];
	m_Output.SHUTTER_F_Free = nOutData[1][1];
	m_Output.SHUTTER_R_Free = nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];
	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][7];
	m_Output.OHTDETECT_Area1_Sel = nOutData[1][8];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][11];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][12];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][13];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][14];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][15];
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][17];
	m_Output.LAMP_Red = nOutData[1][18];

	m_Output.LAMP_Yellow_Right = nOutData[1][19];
	m_Output.LAMP_Green = nOutData[1][20];
	m_Output.HAND_Power_Reset = nOutData[1][21];
	m_Output.BRAKE_Driving_Release = nOutData[1][22];
	m_Output.Bumper_Clear = nOutData[1][23];
	m_Output.SAFETY_Reset = nOutData[1][24];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][25];
	m_Output.QRR_Power = nOutData[1][26];             //AOHTC-286
	m_Output.LAMP_Yellow_Left = nOutData[1][27];
	m_Output.Reserved_60 = nOutData[1][28];
	m_Output.Reserved_61 = nOutData[1][29];
	m_Output.Reserved_62 = nOutData[1][30];
	m_Output.Reserved_63 = nOutData[1][31];

	//CID output 추가
	m_Output.CID_Left_Select_ON = nOutData[2][0];
	m_Output.CID_Right_Select_ON = nOutData[2][1];
	m_Output.CID_Occupancy = nOutData[2][2];
//	m_Output.CID_PASS_Compt_OUT = nOutData[2][3];
	m_Output.CID_Pass_Completion = nOutData[2][3];
	m_Output.CID_Abnormal = nOutData[2][4];
	m_Output.CID_Operation_Mode = nOutData[2][5];
	m_Output.CID_Common_Tag = nOutData[2][6];
	m_Output.CID_LinkRequest = nOutData[2][7];   //AOHC-279
	m_Output.Reserved_72 = nOutData[2][8];
	m_Output.Reserved_73 = nOutData[2][9];
	m_Output.Reserved_74 = nOutData[2][10];
	m_Output.Reserved_75 = nOutData[2][11];
	m_Output.Reserved_76 = nOutData[2][12];
	m_Output.Reserved_77 = nOutData[2][13];
	m_Output.Reserved_78 = nOutData[2][14];
	m_Output.Reserved_79 = nOutData[2][15];
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
	m_Output.HAND_Close = nOutData[0][0];
	m_Output.HAND_Open = nOutData[0][1];
	m_Output.HAND_Origin = nOutData[0][2];
	m_Output.HAND_Reset = nOutData[0][3];
	m_Output.HAND_Free = nOutData[0][4];
	m_Output.HAND_Reserved6 = nOutData[0][5];
	m_Output.HAND_Reserved7 = nOutData[0][6];
	m_Output.HAND_Reserved8 = nOutData[0][7];
	m_Output.HAND_PIO_Select = nOutData[0][8];
	m_Output.EQPIO_Valid = nOutData[0][9];
	m_Output.EQPIO_CS_0 = nOutData[0][10];
	m_Output.EQPIO_CS_1 = nOutData[0][11];
	m_Output.EQPIO_CS_2 = nOutData[0][12];
	m_Output.EQPIO_CS_3 = nOutData[0][13];
	m_Output.EQPIO_TR_Req = nOutData[0][14];
	m_Output.EQPIO_Busy = nOutData[0][15];
	m_Output.EQPIO_Complete = nOutData[0][16];
	m_Output.EQPIO_Right_Select = nOutData[0][17];
	m_Output.EQPIO_Left_Select = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 = nOutData[0][19];
	m_Output.STEER_F_TRQ_1 = nOutData[0][20];
	m_Output.STEER_R_TRQ_0 = nOutData[0][21];
	m_Output.STEER_R_TRQ_1 = nOutData[0][22];
	m_Output.STEER_F_Dir = nOutData[0][23];
	m_Output.STEER_R_Dir = nOutData[0][24];
	m_Output.SHUTTER_F_Reset = nOutData[0][25];
	m_Output.SHUTTER_R_Reset = nOutData[0][26];
	m_Output.SHUTTER_F_Close = nOutData[0][27];
	m_Output.SHUTTER_R_Close = nOutData[0][28];
	m_Output.SHUTTER_F_Open = nOutData[0][29];
	m_Output.SHUTTER_R_Open = nOutData[0][30];
	m_Output.SHUTTER_F_Origin = nOutData[0][31];	

	m_Output.SHUTTER_R_Origin = nOutData[1][0];
	m_Output.SHUTTER_F_Free = nOutData[1][1];
	m_Output.SHUTTER_R_Free = nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];
	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][7];
	m_Output.OHTDETECT_Area1_Sel = nOutData[1][8];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][11];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][12];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][13];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][14];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][15];
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][17];
	m_Output.LAMP_Red = nOutData[1][18];
	m_Output.LAMP_Yellow_Right 		= nOutData[1][19];
	m_Output.LAMP_Green 		= nOutData[1][20];
	m_Output.HAND_Power_Reset 	= nOutData[1][21];
	m_Output.BRAKE_Driving_Release = nOutData[1][22];
	m_Output.Bumper_Clear 		= nOutData[1][23];
	m_Output.SAFETY_Reset 		= nOutData[1][24];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][25];
	m_Output.QRR_Power 		= nOutData[1][26];
	m_Output.LAMP_Yellow_Left	= nOutData[1][27];
	m_Output.Reserved_60 		= nOutData[1][28];    //AOHC-286
	m_Output.Reserved_61 		= nOutData[1][29];    //AOHC-286
	m_Output.Reserved_62 		= nOutData[1][30];    //AOHC-286
	m_Output.Reserved_63 		= nOutData[1][31];    //AOHC-286

	//CID output 추가
	m_Output.CID_Left_Select_ON = nOutData[2][0];
	m_Output.CID_Right_Select_ON = nOutData[2][1];
	m_Output.CID_Occupancy = nOutData[2][2];
//	m_Output.CID_PASS_Compt_OUT = nOutData[2][3];
	m_Output.CID_Pass_Completion = nOutData[2][3];
	m_Output.CID_Abnormal = nOutData[2][4];
	m_Output.CID_Operation_Mode = nOutData[2][5];
	m_Output.CID_Common_Tag = nOutData[2][6];
	m_Output.CID_LinkRequest = nOutData[2][7];
	m_Output.Reserved_72 = nOutData[2][8];      	//AOHC-286
	m_Output.Reserved_73 = nOutData[2][9];         	//AOHC-286
	m_Output.Reserved_74 = nOutData[2][10];         	//AOHC-286
	m_Output.Reserved_75 = nOutData[2][11];         	//AOHC-286
	m_Output.Reserved_76 = nOutData[2][12];          	//AOHC-286
	m_Output.Reserved_77 = nOutData[2][13];          	//AOHC-286
	m_Output.Reserved_78 = nOutData[2][14];          	//AOHC-286
	m_Output.Reserved_79 = nOutData[2][15];             	//AOHC-286

#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
    // 0 - 15
	m_Output.HAND_Close 		= nOutData[0][0];
	m_Output.HAND_Open 			= nOutData[0][1];
	m_Output.HAND_Origin 		= nOutData[0][2];
	m_Output.HAND_Reset 		= nOutData[0][3];
	
	m_Output.HAND_Free 			= nOutData[0][4];
	m_Output.HAND_Reserved6 	= nOutData[0][5];
	m_Output.HAND_Reserved7 	= nOutData[0][6];
	m_Output.HAND_Reserved8 	= nOutData[0][7];
	
	m_Output.HAND_PIO_Select 	= nOutData[0][8];
	m_Output.EQPIO_Valid 		= nOutData[0][9];
	m_Output.EQPIO_CS_0 		= nOutData[0][10];
	m_Output.EQPIO_CS_1 		= nOutData[0][11];
	
	m_Output.EQPIO_CS_2 		= nOutData[0][12];
	m_Output.EQPIO_CS_3 		= nOutData[0][13];
	m_Output.EQPIO_TR_Req 		= nOutData[0][14];
	m_Output.EQPIO_Busy 		= nOutData[0][15];

	// 16 - 31
	m_Output.EQPIO_Complete 	= nOutData[0][16];
	m_Output.EQPIO_Right_Select = nOutData[0][17];
	m_Output.EQPIO_Left_Select  = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 		= nOutData[0][19];
	
	m_Output.STEER_F_TRQ_1 		= nOutData[0][20];
	m_Output.STEER_R_TRQ_0 		= nOutData[0][21];
	m_Output.STEER_R_TRQ_1 		= nOutData[0][22];
	m_Output.STEER_F_Dir 		= nOutData[0][23];
	
	m_Output.STEER_R_Dir 		= nOutData[0][24];
	m_Output.SHUTTER_F_Reset 	= nOutData[0][25];
	m_Output.SHUTTER_R_Reset 	= nOutData[0][26];
	m_Output.SHUTTER_F_Close 	= nOutData[0][27];
	
	m_Output.SHUTTER_R_Close 	= nOutData[0][28];
	m_Output.SHUTTER_F_Open 	= nOutData[0][29];
	m_Output.SHUTTER_R_Open 	= nOutData[0][30];
	m_Output.SHUTTER_F_Origin 	= nOutData[0][31];

	// 32 - 47
	m_Output.SHUTTER_R_Origin 	= nOutData[1][0];
	m_Output.SHUTTER_F_Free 	= nOutData[1][1];	
	m_Output.SHUTTER_R_Free 	= nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];
	
	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];	
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][7];
	
	m_Output.OHTDETECT_Area1_Sel = nOutData[1][8];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][9];	
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][11];
	
	m_Output.OHTDETECT_Power_Reset = nOutData[1][12];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][13];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][14];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][15];

	// 48 - 63
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][17];	
	m_Output.LAMP_Red 			= nOutData[1][18];
	m_Output.LAMP_Yellow_Right 		= nOutData[1][19];
	
	m_Output.LAMP_Green 		= nOutData[1][20];
	m_Output.HAND_Power_Reset 	= nOutData[1][21];	
	m_Output.BRAKE_Driving_Release = nOutData[1][22];
	m_Output.Bumper_Clear 		= nOutData[1][23];
	
	m_Output.SAFETY_Reset 		= nOutData[1][24];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][25];		
	m_Output.QRR_Power 		= nOutData[1][26];
	m_Output.LAMP_Yellow_Left	= nOutData[1][27];
	
	m_Output.Reserved60 		= nOutData[1][28];
	m_Output.Reserved61 		= nOutData[1][29];
	m_Output.Reserved62 		= nOutData[1][30];
	m_Output.Reserved63 		= nOutData[1][31];

	// 64 - 79
	m_Output.CID_Left_Select_ON = nOutData[2][0];
	m_Output.CID_Right_Select_ON = nOutData[2][1];	
	m_Output.CID_Occupancy 		= nOutData[2][2];
	m_Output.CID_Pass_Completion = nOutData[2][3];	
	
	m_Output.CID_Abnormal 		= nOutData[2][4];
	m_Output.CID_Operation_Mode = nOutData[2][5];	
	m_Output.CID_Common_Tag 	= nOutData[2][6];
	m_Output.CID_LinkRequest 	= nOutData[2][7];
	
	m_Output.Reserved_72 = nOutData[2][8];     //AOHC-286
	m_Output.Reserved_73 = nOutData[2][9];     //AOHC-286
	m_Output.Reserved_74 = nOutData[2][10];        //AOHC-286
	m_Output.SHUTTER_F_Middle_Close = nOutData[0][11];

	m_Output.SHUTTER_R_Middle_Close = nOutData[0][12];
	m_Output.Reserved_77 				= nOutData[2][13];  //AOHC-286
	m_Output.Reserved_78 				= nOutData[2][14];  //AOHC-286
	m_Output.Reserved_79 				= nOutData[2][15];  //AOHC-286

#elif(OHT_NAME == OHT_NAME_STD_V86)
   //10개
	m_Output.HAND_Close 			= nOutData[0][0];
	m_Output.HAND_Open 				= nOutData[0][1];
	m_Output.HAND_Origin 			= nOutData[0][2];
	m_Output.HAND_Reset 			= nOutData[0][3];
	m_Output.HAND_Free 				= nOutData[0][4];
	m_Output.HAND_Reserved6 		= nOutData[0][5];
	m_Output.HAND_Reserved7 		= nOutData[0][6];
	m_Output.HAND_Reserved8 		= nOutData[0][7];
	m_Output.HAND_HandTrigger 		= nOutData[0][8];
	m_Output.HAND_PIO_Select 		= nOutData[0][9];

	//10개
	m_Output.EQPIO_Valid 			= nOutData[0][10];
	m_Output.EQPIO_CS_0 			= nOutData[0][11];
	m_Output.EQPIO_CS_1 			= nOutData[0][12];
	m_Output.EQPIO_CS_2 			= nOutData[0][13];
	m_Output.EQPIO_CS_3	 			= nOutData[0][14];
	m_Output.EQPIO_TR_Req 			= nOutData[0][15];
	m_Output.EQPIO_Busy 			= nOutData[0][16];
	m_Output.EQPIO_Complete 		= nOutData[0][17];
	m_Output.EQPIO_Right_Select 	= nOutData[0][18];
	m_Output.EQPIO_Left_Select		= nOutData[0][19];

	//10개
	m_Output.STEER_F_TRQ_0 			= nOutData[0][20];
	m_Output.STEER_F_TRQ_1 			= nOutData[0][21];
	m_Output.STEER_R_TRQ_0 			= nOutData[0][22];
	m_Output.STEER_R_TRQ_1 			= nOutData[0][23];
	m_Output.STEER_F_Dir 			= nOutData[0][24];
	m_Output.STEER_R_Dir 			= nOutData[0][25];
	m_Output.SHUTTER_F_Reset 		= nOutData[0][26];
	m_Output.SHUTTER_R_Reset 		= nOutData[0][27];
	m_Output.SHUTTER_F_Close 		= nOutData[0][28];
	m_Output.SHUTTER_R_Close 		= nOutData[0][29];

	//10개
	m_Output.SHUTTER_F_Open 		= nOutData[0][30];
	m_Output.SHUTTER_R_Open 		= nOutData[0][31];
	m_Output.SHUTTER_F_Origin 		= nOutData[1][0];
	m_Output.SHUTTER_R_Origin 		= nOutData[1][1];
	m_Output.SHUTTER_F_Free 		= nOutData[1][2];
	m_Output.SHUTTER_R_Free 		= nOutData[1][3];
	m_Output.OBSTACLE_Area1_Sel 	= nOutData[1][4];
	m_Output.OBSTACLE_Area2_Sel 	= nOutData[1][5];
	m_Output.OBSTACLE_Area3_Sel 	= nOutData[1][6];
	m_Output.OBSTACLE_Area4_Sel 	= nOutData[1][7];

	//10개
	m_Output.Reserved40 			= nOutData[1][8];
	m_Output.OBSTACLE_Power_Reset 	= nOutData[1][9];
	m_Output.OHTDETECT_Area1_Sel 	= nOutData[1][10];
	m_Output.OHTDETECT_Area2_Sel 	= nOutData[1][11];
	m_Output.OHTDETECT_Area3_Sel 	= nOutData[1][12];
	m_Output.OHTDETECT_Area4_Sel 	= nOutData[1][13];
	m_Output.OHTDETECT_Area5_Sel 	= nOutData[1][14];
	m_Output.OHTDETECT_Power_Reset 	= nOutData[1][15];
	m_Output.LOOKDOWN_Area1_Sel 	= nOutData[1][16];
	m_Output.LOOKDOWN_Area2_Sel 	= nOutData[1][17];

	//10개
	m_Output.LOOKDOWN_Area3_Sel 	= nOutData[1][18];
	m_Output.LOOKDOWN_Area4_Sel 	= nOutData[1][19];
	m_Output.LOOKDOWN_Power_Reset 	= nOutData[1][20];
	m_Output.LAMP_Red 				= nOutData[1][21];
	m_Output.LAMP_Yellow_Right 		= nOutData[1][22];
	m_Output.LAMP_Green 			= nOutData[1][23];
	m_Output.Reserved56 			= nOutData[1][24];
	m_Output.LAMP_Yellow_Left		= nOutData[1][25];
	m_Output.Reserved58 			= nOutData[1][26];
	m_Output.HAND_Power_Reset 		= nOutData[1][27];

	//10개
	m_Output.BRAKE_Driving_Release 	= nOutData[1][28];
	m_Output.Bumper_Clear 			= nOutData[1][29];
	m_Output.SAFETY_Reset 			= nOutData[1][30];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][31];
	m_Output.Reserved64 			= nOutData[2][0];
	m_Output.Reserved65 			= nOutData[2][1];
	m_Output.OBSTACLE_Area5_Sel 	= nOutData[2][2];
	m_Output.Hoist_Steer_PWR_Enable = nOutData[2][3];
	m_Output.IO_PWR_Enable		 	= nOutData[2][4];
	m_Output.EQ_PWR_Enable 			= nOutData[2][5];

	//10개
	m_Output.CID_Left_Select_ON 	= nOutData[2][6];
	m_Output.CID_Right_Select_ON 	= nOutData[2][7];
	m_Output.CID_Occupancy 			= nOutData[2][8];
	m_Output.CID_Pass_Completion	= nOutData[2][9];
	m_Output.CID_Abnormal 			= nOutData[2][10];
	m_Output.CID_Operation_Mode 	= nOutData[2][11];
	m_Output.CID_Common_Tag 		= nOutData[2][12];
	m_Output.CID_LinkRequest 		= nOutData[2][13];
	m_Output.Blackbox_DO2			= nOutData[2][14];
	m_Output.Blackbox_DO0			= nOutData[2][16];

	//10개
	m_Output.SHUTTER_F_Middle_Close = nOutData[2][17];
	m_Output.SHUTTER_R_Middle_Close = nOutData[2][18];
	m_Output.SPARE_78 				= nOutData[2][19];
	m_Output.SPARE_79				= nOutData[2][20];

#elif(OHT_NAME == OHT_NAME_STD_V85)

	m_Output.HAND_Close 		= nOutData[0][0];
	m_Output.HAND_Open 			= nOutData[0][1];
	m_Output.HAND_Origin 		= nOutData[0][2];
	m_Output.HAND_Reset 		= nOutData[0][3];

	m_Output.HAND_Free 			= nOutData[0][4];
	m_Output.HAND_Reserved6 	= nOutData[0][5];
	m_Output.HAND_Reserved7 	= nOutData[0][6];
	m_Output.HAND_Reserved8 	= nOutData[0][7];
	m_Output.HAND_HandTrigger 	= nOutData[0][8];
	m_Output.HAND_PIO_Select 	= nOutData[0][9];
	m_Output.EQPIO_Valid 		= nOutData[0][10];
	m_Output.EQPIO_CS_0 		= nOutData[0][11];
	m_Output.EQPIO_CS_1 		= nOutData[0][12];

	m_Output.EQPIO_CS_2 		= nOutData[0][13];
	m_Output.EQPIO_CS_3 		= nOutData[0][14];
	m_Output.EQPIO_TR_Req 		= nOutData[0][15];
	m_Output.EQPIO_Busy 		= nOutData[0][16];

	// 16 - 31
	m_Output.EQPIO_Complete 	= nOutData[0][17];
	m_Output.EQPIO_Left_Select  = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 		= nOutData[0][19];

	m_Output.STEER_F_TRQ_1 		= nOutData[0][20];
	m_Output.STEER_R_TRQ_0 		= nOutData[0][21];
	m_Output.STEER_R_TRQ_1 		= nOutData[0][22];
	m_Output.STEER_F_Dir 		= nOutData[0][23];

	m_Output.STEER_R_Dir 		= nOutData[0][24];
	m_Output.SHUTTER_F_Reset 	= nOutData[0][25];
	m_Output.SHUTTER_R_Reset 	= nOutData[0][26];
	m_Output.SHUTTER_F_Close 	= nOutData[0][27];

	m_Output.SHUTTER_R_Close 	= nOutData[0][28];
	m_Output.SHUTTER_F_Open 	= nOutData[0][29];
	m_Output.SHUTTER_R_Open 	= nOutData[0][30];
	m_Output.SHUTTER_F_Origin 	= nOutData[0][31];

	// 32 - 47
	m_Output.SHUTTER_R_Origin 	= nOutData[1][0];
	m_Output.SHUTTER_F_Free 	= nOutData[1][1];
	m_Output.SHUTTER_R_Free 	= nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];

	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Area5_Sel = nOutData[1][7];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][8];

	m_Output.OHTDETECT_Area1_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][11];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][12];
	m_Output.OHTDETECT_Area5_Sel = nOutData[1][13];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][14];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][15];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][17];

	// 48 - 63
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][18];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][19];
	m_Output.LAMP_Red 			= nOutData[1][20];
	m_Output.LAMP_Yellow_Right 		= nOutData[1][21];
	m_Output.LAMP_Yellow_Left	= nOutData[1][22];
	m_Output.LAMP_Green 		= nOutData[1][23];
	m_Output.HAND_Power_Reset 	= nOutData[1][24];
	m_Output.BRAKE_Driving_Release = nOutData[1][25];
	m_Output.Bumper_Clear 		= nOutData[1][26];

	m_Output.SAFETY_Reset 		= nOutData[1][27];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][28];
	m_Output.QRR_Power 		= nOutData[1][29];
	m_Output.Reserved62 		= nOutData[1][30];        //AOHTC-286
	m_Output.Reserved63 		= nOutData[1][31];        //AOHTC-286

		// 64 - 79
	m_Output.Reserved64 		= nOutData[2][0];          //AOHTC-286
	m_Output.Reserved65 		= nOutData[2][1];          //AOHTC-286

	m_Output.CID_Left_Select_ON = nOutData[2][2];
	m_Output.CID_Right_Select_ON = nOutData[2][3];
	m_Output.CID_Occupancy 		= nOutData[2][4];
	m_Output.CID_Pass_Completion = nOutData[2][5];

	m_Output.CID_Abnormal 		= nOutData[2][6];
	m_Output.CID_Operation_Mode = nOutData[2][7];
	m_Output.CID_Common_Tag 	= nOutData[2][8];
	m_Output.CID_LinkRequest 	= nOutData[2][9];

	m_Output.SPARE_74			= nOutData[2][10];          //AOHTC-286
	m_Output.SPARE_75 			= nOutData[2][11];          //AOHTC-286
	m_Output.SHUTTER_F_Middle_Close = nOutData[2][12];

	m_Output.SHUTTER_R_Middle_Close = nOutData[2][13];
	m_Output.SPARE_78 				= nOutData[2][14];      //AOHTC-286
	m_Output.SPARE_79 				= nOutData[2][15];      //AOHTC-286

#elif(OHT_NAME == OHT_NAME_STD_V81S)
	m_Output.HAND_Close 		= nOutData[0][0];
	m_Output.HAND_Open 			= nOutData[0][1];
	m_Output.HAND_Origin 		= nOutData[0][2];
	m_Output.HAND_Reset 		= nOutData[0][3];

	m_Output.HAND_Free 			= nOutData[0][4];
	m_Output.HAND_Reserved6 	= nOutData[0][5];
	m_Output.HAND_Reserved7 	= nOutData[0][6];
	m_Output.HAND_Reserved8 	= nOutData[0][7];
	m_Output.HAND_HandTrigger 	= nOutData[0][8];
	m_Output.HAND_PIO_Select 	= nOutData[0][9];
	m_Output.EQPIO_Valid 		= nOutData[0][10];
	m_Output.EQPIO_CS_0 		= nOutData[0][11];
	m_Output.EQPIO_CS_1 		= nOutData[0][12];

	m_Output.EQPIO_CS_2 		= nOutData[0][13];
	m_Output.EQPIO_CS_3 		= nOutData[0][14];
	m_Output.EQPIO_TR_Req 		= nOutData[0][15];
	m_Output.EQPIO_Busy 		= nOutData[0][16];

	// 16 - 31
	m_Output.EQPIO_Complete 	= nOutData[0][17];
	m_Output.EQPIO_Left_Select  = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 		= nOutData[0][19];

	m_Output.STEER_F_TRQ_1 		= nOutData[0][20];
	m_Output.STEER_R_TRQ_0 		= nOutData[0][21];
	m_Output.STEER_R_TRQ_1 		= nOutData[0][22];
	m_Output.STEER_F_Dir 		= nOutData[0][23];

	m_Output.STEER_R_Dir 		= nOutData[0][24];
	m_Output.SHUTTER_F_Reset 	= nOutData[0][25];
	m_Output.SHUTTER_R_Reset 	= nOutData[0][26];
	m_Output.SHUTTER_F_Close 	= nOutData[0][27];

	m_Output.SHUTTER_R_Close 	= nOutData[0][28];
	m_Output.SHUTTER_F_Open 	= nOutData[0][29];
	m_Output.SHUTTER_R_Open 	= nOutData[0][30];
	m_Output.SHUTTER_F_Origin 	= nOutData[0][31];

	// 32 - 47
	m_Output.SHUTTER_R_Origin 	= nOutData[1][0];
	m_Output.SHUTTER_F_Free 	= nOutData[1][1];
	m_Output.SHUTTER_R_Free 	= nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];

	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Area5_Sel = nOutData[1][7];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][8];

	m_Output.OHTDETECT_Area1_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][11];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][12];
	m_Output.OHTDETECT_Area5_Sel = nOutData[1][13];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][14];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][15];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][17];

	// 48 - 63
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][18];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][19];
	m_Output.LAMP_Red 			= nOutData[1][20];
	m_Output.LAMP_Yellow_Right 		= nOutData[1][21];
    m_Output.LAMP_Yellow_Left	= nOutData[1][22];
	m_Output.LAMP_Green 		= nOutData[1][23];
	m_Output.HAND_Power_Reset 	= nOutData[1][24];
	m_Output.BRAKE_Driving_Release = nOutData[1][25];
	m_Output.Bumper_Clear 		= nOutData[1][26];

	m_Output.SAFETY_Reset 		= nOutData[1][27];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][28];
	m_Output.QRR_Power 		= nOutData[1][29];
	m_Output.Reserved62 		= nOutData[1][30];      //AOHTC-286
	m_Output.Reserved63 		= nOutData[1][31];      //AOHTC-286

		// 64 - 79
	m_Output.Reserved64 		= nOutData[2][0];       //AOHTC-286
	m_Output.Reserved65 		= nOutData[2][1];       //AOHTC-286

	m_Output.CID_Left_Select_ON = nOutData[2][2];
	m_Output.CID_Right_Select_ON = nOutData[2][3];
	m_Output.CID_Occupancy 		= nOutData[2][4];
	m_Output.CID_Pass_Completion = nOutData[2][5];

	m_Output.CID_Abnormal 		= nOutData[2][6];
	m_Output.CID_Operation_Mode = nOutData[2][7];
	m_Output.CID_Common_Tag 	= nOutData[2][8];
	m_Output.CID_LinkRequest 	= nOutData[2][9];

	m_Output.SPARE_74			= nOutData[2][10];          //AOHTC-286
	m_Output.SPARE_75			= nOutData[2][11];         //AOHTC-286
	m_Output.SHUTTER_F_Middle_Close = nOutData[2][12];

	m_Output.SHUTTER_R_Middle_Close = nOutData[2][13];
	m_Output.SPARE_78 				= nOutData[2][14];      //AOHTC-286
	m_Output.SPARE_79 				= nOutData[2][15];       //AOHTC-286

#elif(OHT_NAME == OHT_NAME_STD_V85S)
	m_Output.HAND_Close 			= nOutData[0][0];     //0
	m_Output.HAND_Open 				= nOutData[0][1];     //1
	m_Output.HAND_Origin 			= nOutData[0][2];     //2
	m_Output.HAND_Reset 			= nOutData[0][3];     //3

	m_Output.HAND_Free 				= nOutData[0][4];
	m_Output.HAND_Reserved6 		= nOutData[0][5];
	m_Output.HAND_Reserved7 		= nOutData[0][6];
	m_Output.HAND_Reserved8 		= nOutData[0][7];
	m_Output.HAND_HandTrigger 		= nOutData[0][8];
	m_Output.HAND_PIO_Select 		= nOutData[0][9];
	m_Output.EQPIO_Valid 			= nOutData[0][10];
	m_Output.EQPIO_CS_0 			= nOutData[0][11];
	m_Output.EQPIO_CS_1 			= nOutData[0][12];

	m_Output.EQPIO_CS_2 			= nOutData[0][13];
	m_Output.EQPIO_CS_3 			= nOutData[0][14];
	m_Output.EQPIO_TR_Req 			= nOutData[0][15];
	m_Output.EQPIO_Busy 			= nOutData[0][16];

	// 16 - 31
	m_Output.EQPIO_Complete 		= nOutData[0][17];
	m_Output.EQPIO_Right_Select 	= nOutData[0][18];
	m_Output.EQPIO_Left_Select	 	= nOutData[0][19];
	m_Output.STEER_F_TRQ_0 			= nOutData[0][20];

	m_Output.STEER_F_TRQ_1 			= nOutData[0][21];
	m_Output.STEER_R_TRQ_0 			= nOutData[0][22];
	m_Output.STEER_R_TRQ_1 			= nOutData[0][23];
	m_Output.STEER_F_Dir 			= nOutData[0][24];

	m_Output.STEER_R_Dir 			= nOutData[0][25];
	m_Output.SHUTTER_F_Reset 		= nOutData[0][26];
	m_Output.SHUTTER_R_Reset 		= nOutData[0][27];
	m_Output.SHUTTER_F_Close 		= nOutData[0][28];

	m_Output.SHUTTER_R_Close 		= nOutData[0][29];
	m_Output.SHUTTER_F_Open 		= nOutData[0][30];
	m_Output.SHUTTER_R_Open 		= nOutData[0][31];

	// 32 - 47
	m_Output.SHUTTER_F_Origin 		= nOutData[1][0];
	m_Output.SHUTTER_R_Origin 		= nOutData[1][1];          //33
	m_Output.SHUTTER_F_Free 		= nOutData[1][2];          //34
	m_Output.SHUTTER_R_Free 		= nOutData[1][3];          //35
	m_Output.OBSTACLE_Area1_Sel 	= nOutData[1][4];          //36

	m_Output.OBSTACLE_Area2_Sel 	= nOutData[1][5];           //37
	m_Output.OBSTACLE_Area3_Sel 	= nOutData[1][6];           //38
	m_Output.OBSTACLE_Area4_Sel 	= nOutData[1][7];           //39
	m_Output.Reserved40 			= nOutData[1][8];           //40
	m_Output.OBSTACLE_Power_Reset 	= nOutData[1][9];         //41

	m_Output.OHTDETECT_Area1_Sel 	= nOutData[1][10];         //42
	m_Output.OHTDETECT_Area2_Sel 	= nOutData[1][11];         //43
	m_Output.OHTDETECT_Area3_Sel 	= nOutData[1][12];         //44
	m_Output.OHTDETECT_Area4_Sel 	= nOutData[1][13];         //45
	m_Output.OHTDETECT_Area5_Sel 	= nOutData[1][14];         //46
	m_Output.OHTDETECT_Power_Reset 	= nOutData[1][15];       	//47
	m_Output.LOOKDOWN_Area1_Sel 	= nOutData[1][16];          //48
	m_Output.LOOKDOWN_Area2_Sel 	= nOutData[1][17];          //49
	m_Output.LOOKDOWN_Area3_Sel 	= nOutData[1][18];          //50

	// 48 - 63
	m_Output.LOOKDOWN_Area4_Sel 	= nOutData[1][19];        //51
	m_Output.LOOKDOWN_Power_Reset 	= nOutData[1][20];        //52
	m_Output.LAMP_Red 				= nOutData[1][21];        //53
	m_Output.LAMP_Yellow_Right 		= nOutData[1][22];        //54
	m_Output.LAMP_Green    			= nOutData[1][23];        //55
	m_Output.Reserved56 			= nOutData[1][24];        //56
	m_Output.LAMP_Yellow_Left		= nOutData[1][25];        //57
	m_Output.Reserved58   		 	= nOutData[1][26];        //58


	m_Output.HAND_Power_Reset 		= nOutData[1][27];        //59
	m_Output.BRAKE_Driving_Release 	= nOutData[1][28];       //60
	m_Output.Bumper_Clear 			= nOutData[1][29];       //61

	m_Output.SAFETY_Reset 			= nOutData[1][30];      //62
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][31];     //63


	m_Output.Reserved64 			= nOutData[2][0];      //64
	m_Output.Reserved65 			= nOutData[2][1];      //65
	m_Output.OBSTACLE_Area5_Sel 	= nOutData[2][2];      //66

		// 64 - 79
	m_Output.Hoist_Steer_PWR_Enable = nOutData[2][3];      //67
	m_Output.IO_PWR_Enable 			= nOutData[2][4];      //68
	m_Output.EQ_PWR_Enable 			= nOutData[2][5];      //69
	m_Output.CID_Left_Select_ON 	= nOutData[2][6];      //70
	m_Output.CID_Right_Select_ON 	= nOutData[2][7];      //71
	m_Output.CID_Occupancy 			= nOutData[2][8];      //72
	m_Output.CID_Pass_Completion 	= nOutData[2][9];      //73

	m_Output.CID_Abnormal 			= nOutData[2][10];      //74
	m_Output.CID_Operation_Mode 	= nOutData[2][11];      //75
	m_Output.CID_Common_Tag 		= nOutData[2][12];      //76
	m_Output.CID_LinkRequest 		= nOutData[2][13];     //77

	m_Output.Blackbox_DO2			= nOutData[2][14];     //78
	m_Output.Blackbox_DO1			= nOutData[2][15];     //79
	m_Output.Blackbox_DO0			= nOutData[2][16];     //80
	m_Output.SPARE_76				= nOutData[2][17];     //81
	m_Output.SPARE_77 				= nOutData[2][18];     //82
	m_Output.SPARE_78 				= nOutData[2][19];     //83
	m_Output.SPARE_79 				= nOutData[2][20];     //83

#elif(OHT_NAME == OHT_NAME_STD_V82)
	m_Output.HAND_Close 			= nOutData[0][0];     //0
	m_Output.HAND_Open 				= nOutData[0][1];     //1
	m_Output.HAND_Origin 			= nOutData[0][2];     //2
	m_Output.HAND_Reset 			= nOutData[0][3];     //3

	m_Output.HAND_Free 				= nOutData[0][4];
	m_Output.HAND_Reserved6 		= nOutData[0][5];
	m_Output.HAND_Reserved7 		= nOutData[0][6];
	m_Output.HAND_Reserved8 		= nOutData[0][7];
	m_Output.HAND_HandTrigger 		= nOutData[0][8];
	m_Output.HAND_PIO_Select 		= nOutData[0][9];
	m_Output.EQPIO_Valid 			= nOutData[0][10];
	m_Output.EQPIO_CS_0 			= nOutData[0][11];
	m_Output.EQPIO_CS_1 			= nOutData[0][12];

	m_Output.EQPIO_CS_2 			= nOutData[0][13];
	m_Output.EQPIO_CS_3 			= nOutData[0][14];
	m_Output.EQPIO_TR_Req 			= nOutData[0][15];
	m_Output.EQPIO_Busy 			= nOutData[0][16];

	// 16 - 31
	m_Output.EQPIO_Complete 		= nOutData[0][17];
	m_Output.EQPIO_Right_Select 	= nOutData[0][18];
	m_Output.EQPIO_Left_Select	 	= nOutData[0][19];
	m_Output.STEER_F_TRQ_0 			= nOutData[0][20];

	m_Output.STEER_F_TRQ_1 			= nOutData[0][21];
	m_Output.STEER_R_TRQ_0 			= nOutData[0][22];
	m_Output.STEER_R_TRQ_1 			= nOutData[0][23];
	m_Output.STEER_F_Dir 			= nOutData[0][24];

	m_Output.STEER_R_Dir 			= nOutData[0][25];
	m_Output.SHUTTER_F_Reset 		= nOutData[0][26];
	m_Output.SHUTTER_R_Reset 		= nOutData[0][27];
	m_Output.SHUTTER_F_Close 		= nOutData[0][28];

	m_Output.SHUTTER_R_Close 		= nOutData[0][29];
	m_Output.SHUTTER_F_Open 		= nOutData[0][30];
	m_Output.SHUTTER_R_Open 		= nOutData[0][31];

	// 32 - 47
	m_Output.SHUTTER_F_Origin 		= nOutData[1][0];
	m_Output.SHUTTER_R_Origin 		= nOutData[1][1];          //33
	m_Output.SHUTTER_F_Free 		= nOutData[1][2];          //34
	m_Output.SHUTTER_R_Free 		= nOutData[1][3];          //35
	m_Output.OBSTACLE_Area1_Sel 	= nOutData[1][4];          //36

	m_Output.OBSTACLE_Area2_Sel 	= nOutData[1][5];           //37
	m_Output.OBSTACLE_Area3_Sel 	= nOutData[1][6];           //38
	m_Output.OBSTACLE_Area4_Sel 	= nOutData[1][7];           //39
	m_Output.Reserved40 			= nOutData[1][8];           //40
	m_Output.OBSTACLE_Power_Reset 	= nOutData[1][9];         //41

	m_Output.OHTDETECT_Area1_Sel 	= nOutData[1][10];         //42
	m_Output.OHTDETECT_Area2_Sel 	= nOutData[1][11];         //43
	m_Output.OHTDETECT_Area3_Sel 	= nOutData[1][12];         //44
	m_Output.OHTDETECT_Area4_Sel 	= nOutData[1][13];         //45
	m_Output.OHTDETECT_Area5_Sel 	= nOutData[1][14];         //46
	m_Output.OHTDETECT_Power_Reset 	= nOutData[1][15];       	//47
	m_Output.LOOKDOWN_Area1_Sel 	= nOutData[1][16];          //48
	m_Output.LOOKDOWN_Area2_Sel 	= nOutData[1][17];          //49
	m_Output.LOOKDOWN_Area3_Sel 	= nOutData[1][18];          //50

	// 48 - 63
	m_Output.LOOKDOWN_Area4_Sel 	= nOutData[1][19];        //51
	m_Output.LOOKDOWN_Power_Reset 	= nOutData[1][20];        //52
	m_Output.LAMP_Red 				= nOutData[1][21];        //53
	m_Output.LAMP_Yellow_Right 		= nOutData[1][22];        //54
	m_Output.LAMP_Green    			= nOutData[1][23];        //55
	m_Output.Reserved56 			= nOutData[1][24];        //56
	m_Output.LAMP_Yellow_Left		= nOutData[1][25];        //57
	m_Output.Reserved58   		 	= nOutData[1][26];        //58


	m_Output.HAND_Power_Reset 		= nOutData[1][27];        //59
	m_Output.BRAKE_Driving_Release 	= nOutData[1][28];       //60
	m_Output.Bumper_Clear 			= nOutData[1][29];       //61

	m_Output.SAFETY_Reset 			= nOutData[1][30];      //62
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][31];     //63


	m_Output.Reserved64 			= nOutData[2][0];      //64
	m_Output.Reserved65 			= nOutData[2][1];      //65
	m_Output.OBSTACLE_Area5_Sel 	= nOutData[2][2];      //66

		// 64 - 79
	m_Output.Hoist_Steer_PWR_Enable = nOutData[2][3];      //67
	m_Output.IO_PWR_Enable 			= nOutData[2][4];      //68
	m_Output.EQ_PWR_Enable 			= nOutData[2][5];      //69
	m_Output.CID_Left_Select_ON 	= nOutData[2][6];      //70
	m_Output.CID_Right_Select_ON 	= nOutData[2][7];      //71
	m_Output.CID_Occupancy 			= nOutData[2][8];      //72
	m_Output.CID_Pass_Completion 	= nOutData[2][9];      //73

	m_Output.CID_Abnormal 			= nOutData[2][10];      //74
	m_Output.CID_Operation_Mode 	= nOutData[2][11];      //75
	m_Output.CID_Common_Tag 		= nOutData[2][12];      //76
	m_Output.CID_LinkRequest 		= nOutData[2][13];     //77

	m_Output.Blackbox_DO2			= nOutData[2][14];     //78
	m_Output.Blackbox_DO1			= nOutData[2][15];     //79
	m_Output.Blackbox_DO0			= nOutData[2][16];     //80
	m_Output.SPARE_76				= nOutData[2][17];     //81
	m_Output.SPARE_77 				= nOutData[2][18];     //82
	m_Output.SPARE_78 				= nOutData[2][19];     //83
	m_Output.SPARE_79 				= nOutData[2][20];     //83

#elif(OHT_NAME == OHT_NAME_P4_MASK)	//mgchoi. P4 MaskOHT 용 InputData Update
	m_Output.HAND_Work = nOutData[0][0];
	m_Output.HAND_Work_Interlock = nOutData[0][1];
	m_Output.HAND_Home = nOutData[0][2];
	m_Output.HAND_Home_Interlock = nOutData[0][3];
	m_Output.HAND_Origin = nOutData[0][4];
	m_Output.HAND_Reset = nOutData[0][5];
	m_Output.HAND_Free = nOutData[0][6];
	m_Output.HAND_Reserved8 = nOutData[0][7];

	m_Output.HAND_Reserved9 = nOutData[0][8];
	m_Output.HAND_PIO_Select = nOutData[0][9];
	m_Output.EQPIO_Valid = nOutData[0][10];
	m_Output.EQPIO_CS_0 = nOutData[0][11];
	m_Output.EQPIO_CS_1 = nOutData[0][12];
	m_Output.EQPIO_CS_2 = nOutData[0][13];
	m_Output.EQPIO_CS_3 = nOutData[0][14];
	m_Output.EQPIO_TR_Req = nOutData[0][15];

	m_Output.EQPIO_Busy = nOutData[0][16];
	m_Output.EQPIO_Complete = nOutData[0][17];
	m_Output.EQPIO_Right_Select = nOutData[0][18];
	m_Output.EQPIO_Left_Select = nOutData[0][19];
	m_Output.STEER_F_TRQ_0 = nOutData[0][20];
	m_Output.STEER_F_TRQ_1 = nOutData[0][21];
	m_Output.STEER_R_TRQ_0 = nOutData[0][22];
	m_Output.STEER_R_TRQ_1 = nOutData[0][23];

	m_Output.STEER_F_Dir = nOutData[0][24];
	m_Output.STEER_R_Dir = nOutData[0][25];
	m_Output.SHUTTER_F_Reset = nOutData[0][26];
	m_Output.SHUTTER_R_Reset = nOutData[0][27];
	m_Output.SHUTTER_F_Close = nOutData[0][28];
	m_Output.SHUTTER_R_Close = nOutData[0][29];
	m_Output.SHUTTER_F_Open = nOutData[0][30];
	m_Output.SHUTTER_R_Open = nOutData[0][31];

	m_Output.SHUTTER_F_Origin = nOutData[1][0];
	m_Output.SHUTTER_R_Origin = nOutData[1][1];
	m_Output.SHUTTER_F_Free = nOutData[1][2];
	m_Output.SHUTTER_R_Free = nOutData[1][3];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area2_Sel = nOutData[1][5];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][7];

	m_Output.Reserved_41 = nOutData[1][8];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][9];
	m_Output.OHTDETECT_Area1_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][11];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][12];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][13];
	m_Output.Reserved_47 = nOutData[1][14];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][15];

	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][17];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][18];
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][19];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][20];
	m_Output.LAMP_Red_Right = nOutData[1][21];
	m_Output.LAMP_Yellow_Right = nOutData[1][22];
	m_Output.LAMP_Green_Right = nOutData[1][23];

	m_Output.LAMP_Red_Left = nOutData[1][24];
	m_Output.LAMP_Yellow_Left = nOutData[1][25];
	m_Output.LAMP_Green_Left = nOutData[1][26];
	m_Output.HAND_Power_Reset = nOutData[1][27];
	m_Output.BRAKE_Driving_Release = nOutData[1][28];
	m_Output.Bumper_Clear = nOutData[1][29];
	m_Output.SAFETY_Reset = nOutData[1][30];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][31];

	m_Output.Reserved_65 = nOutData[2][0];
	m_Output.Reserved_66 = nOutData[2][1];
	m_Output.OBSTACLE_Area5_Sel = nOutData[2][2];
	m_Output.Hoist_Steer_PWR_Enable = nOutData[2][3];
	m_Output.IO_PWR_Enable = nOutData[2][4];
	m_Output.EQ_PIO_PWR_Enable = nOutData[2][5];
	m_Output.Reserved_71 = nOutData[2][6];
	m_Output.Reserved_72 = nOutData[2][7];

#else
	m_Output.HAND_Close = nOutData[0][0];
	m_Output.HAND_Open = nOutData[0][1];
	m_Output.HAND_Origin = nOutData[0][2];
	m_Output.HAND_Reset = nOutData[0][3];
	m_Output.HAND_Free = nOutData[0][4];
	m_Output.HAND_Reserved6 = nOutData[0][5];
	m_Output.HAND_Reserved7 = nOutData[0][6];
	m_Output.HAND_Reserved8 = nOutData[0][7];
	m_Output.HAND_PIO_Select = nOutData[0][8];
	m_Output.EQPIO_Valid = nOutData[0][9];
	m_Output.EQPIO_CS_0 = nOutData[0][10];
	m_Output.EQPIO_CS_1 = nOutData[0][11];
	m_Output.EQPIO_TR_Req = nOutData[0][12];
	m_Output.EQPIO_Busy = nOutData[0][13];
	m_Output.EQPIO_Complete = nOutData[0][14];
	m_Output.EQPIO_Right_Select = nOutData[0][15];
	m_Output.EQPIO_Left_Select = nOutData[0][16];
	m_Output.STEER_F_TRQ_0 = nOutData[0][17];
	m_Output.STEER_F_TRQ_1 = nOutData[0][18];
	m_Output.STEER_R_TRQ_0 = nOutData[0][19];
	m_Output.STEER_R_TRQ_1 = nOutData[0][20];
	m_Output.STEER_F_Dir = nOutData[0][21];
	m_Output.STEER_R_Dir = nOutData[0][22];
	m_Output.SHUTTER_F_Reset = nOutData[0][23];
	m_Output.SHUTTER_R_Reset = nOutData[0][24];
	m_Output.SHUTTER_F_Close = nOutData[0][25];
	m_Output.SHUTTER_R_Close = nOutData[0][26];
	m_Output.SHUTTER_F_Open = nOutData[0][27];
	m_Output.SHUTTER_R_Open = nOutData[0][28];
	m_Output.SHUTTER_F_Origin = nOutData[0][29];
	m_Output.SHUTTER_R_Origin = nOutData[0][30];
	m_Output.SHUTTER_F_Free = nOutData[0][31];

	m_Output.SHUTTER_R_Free = nOutData[1][0];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][1];
	m_Output.OBSTACLE_Area2_Sel = nOutData[1][2];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][3];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][5];
	m_Output.OHTDETECT_Area1_Sel = nOutData[1][6];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][7];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][8];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][10];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][11];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][12];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][13];
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][14];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][15];
	m_Output.LAMP_Red = nOutData[1][16];
	m_Output.LAMP_Yellow = nOutData[1][17];
	m_Output.LAMP_Green = nOutData[1][18];
	m_Output.STB_Right_Work = nOutData[1][19];
	m_Output.STB_Left_Work = nOutData[1][20];
	m_Output.HAND_Power_Reset = nOutData[1][21];
	m_Output.BRAKE_Driving_Release = nOutData[1][22];
	m_Output.Bumper_Clear = nOutData[1][23];
	m_Output.MC_Control = nOutData[1][24];
	m_Output.LCD_Display_Reset = nOutData[1][25];
	m_Output.SERIAL_Select_1 = nOutData[1][26];
	m_Output.SERIAL_Select_2 = nOutData[1][27];
	m_Output.RailSupport_TRQ_0 = nOutData[1][28];
	m_Output.RailSupport_TRQ_1 = nOutData[1][29];
	m_Output.RailSupport_Dir = nOutData[1][30];
	m_Output.Reserved_1 = nOutData[1][31];
#endif

	//AOHC-196 사전 로그 추가
	// SHCHO 2019.03 : UBG, OBS 사양서에 없는 값이 들어올 경우 사전로그 증가
	int nUBGLevel = OHTDetect::GetSensorLevel2(m_Input.OHTDETECT_Long,m_Input.OHTDETECT_Middle, m_Input.OHTDETECT_Short , m_Input.OHTDETECT_Fail);
	int nOBSLevel = OHTDetect::GetSensorLevel2(m_Input.OBSTACLE_Long, m_Input.OBSTACLE_Middle, m_Input.OBSTACLE_Short , m_Input.OBSTACLE_Fail);

	static OHTMainStatus *pMainStat = NULL;
	if(pMainStat == NULL)
		pMainStat = OHTMainForm->GetOHTStatus();

	static DiagManager *pDiag = NULL;
	if(pDiag == NULL)
		pDiag = DiagManager::getInstance();
	if(pDiag != NULL && pMainStat != NULL)
	{
		bool bAutoMode = pMainStat->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO;

		if(bAutoMode)
		{	if(nUBGLevel < 0)
			{
				pDiag->IncreasePreviosDiagCount(PREVIOS_DIAG_UF_2);
				ADD_MD_LOG("UF_2 UBG Lv : %d%d%d%d",m_Input.OHTDETECT_Long,m_Input.OHTDETECT_Middle,m_Input.OHTDETECT_Short,m_Input.OHTDETECT_Fail);

			}

			if(nOBSLevel < 0)
			{
				pDiag->IncreasePreviosDiagCount(PREVIOS_DIAG_OF_2);
				ADD_MD_LOG("OF_2 OBS Lv : %d%d%d%d", m_Input.OBSTACLE_Long,m_Input.OBSTACLE_Middle,	m_Input.OBSTACLE_Short,m_Input.OBSTACLE_Fail);
			}
		}
	}


}

/**
 @brief   HWCommon의 I/O의 정보 획득
 @author  ehoto
 @date    2012-11-19
 @param	pInputBuff: Input 받는 버퍼
 @param	nInputCount: Input 받는 버퍼 개수(int 단위)
 @param	pOutputBuff: Output 받는 버퍼
 @param	nOutCount: Output 받는 버퍼 개수(int 단위)
 @return  Error Code 정상의 경우 NO_ERR(0)
 */
int HWCommon::GetIOSet(int* pInputBuff, int& nInputCount, int* pOutputBuff,
	int& nOutCount)
{
	return m_pMotionController->GetIOSet(pInputBuff, nInputCount, pOutputBuff,
		nOutCount);
}

/**
@brief   주행부 내부토크 설정 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetTorqueLimit(void)
{
	m_pMotionController->SetTorqueLimit();
}
/**
@brief   주행부 내부토크 해지  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetTorqueLimitRelease(void)
{
	m_pMotionController->SetTorqueLimitRelease();
}

/**
@brief   주행부 내부토크 설정 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetSlopeTime(void)
{
	m_pMotionController->SetSlopeTime();
}
/**
@brief   주행부 내부토크 해지  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetSlopeTimeRelease(void)
{
	m_pMotionController->SetSlopeTimeRelease();
}

/**
@brief   주행부 목적지 위치 설정 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetTargetPos(void)
{
	m_pMotionController->SetTargetPos();
}
/**
@brief   주행부 목적지 위치  해지  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetTargetPosRelease(void)
{
	m_pMotionController->SetTargetPosRelease();
}

/**
@brief   주행부 목적지 위치  해지  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetFirstNodeCountAdd(void)
{
	m_pMotionController->SetFirstNodeCountAdd();
}
/**
@brief   주행부 목적지 위치  해지  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetVHLDetectIgnor(void)
{
	m_pMotionController->SetVHLDetectIgnor();
}
/**
@brief   주행부 목적지 위치  해지  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetVHLDetectIgnorRelease(void)
{
	m_pMotionController->SetVHLDetectIgnorRelease();
}

//AOHC-196 사전 로그 추가
int HWCommon::GetFollowing_Axis1(void)
{
	return m_pMotionController->GetFollowing_Axis1();
}

//AOHC-285
void HWCommon::SetLowVibration(void)
{
	m_pMotionController->SetLowVibration();
}

//AOHC-285
void HWCommon::SetLowVibrationRelease(void)
{
    m_pMotionController->SetLowVibrationRelease();
}

/**
@brief   대차감지시 감속도 변경
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetDetectStopDecelDown(void)
{
	m_pMotionController->SetDetectStopDecelDown();
}
/**
@brief   대차감지시 감속도 변경 해제
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetDetectStopDecelDownRelease(void)
{
	m_pMotionController->SetDetectStopDecelDownRelease();
}
/**
@brief   대차감지시 패턴 강제 설정
@author  puting
@date    2019-07-24
*/
void HWCommon::SetVHLDetectIOSet(bool bIoSet)
{
   m_pMotionController->SetVHLDetectIOSet(bIoSet);
}

/**
@brief   주행부 Node Count를 Reset 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void HWCommon::ClearNodeCountInfo(void)
{
	m_pMotionController->ClearNodeCountInfo();
}

/**
@brief   주행부 NodeCount 의 정보를 받는 함수
@author  ehoto
@date    2012-11-28
@param   nNodeCount : Node Count를 돌려 받는 변수
@param   dNodeDetectPosition : Node 감지된 위치를 돌려 받는 변수
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int HWCommon::GetNodeCountInfo(NODE_CHECK_DATA &NodeCheckData)
{
	return m_pMotionController->GetNodeCountInfo(NodeCheckData);
}

/**
@brief   DetectSensor의 정보를 설정하는 함수
@author  ehoto
@date    2013-01-22
@param   DetectInfo : DetectSensor 의 값을 설정하기 휘한 정보를 가지고 있는 구조체
@return  Error Code 정상의 경우 NO_ERR(0)
*/

#if(USE_CHANGENODE == 1)
int HWCommon::SetDetectInfo(DETECT_INFO DetectInfo, OPTIONAL_NODE_INFO OptNodeInfo)
{
	return m_pMotionController->SetDetectInfo(DetectInfo, OptNodeInfo);
}
#else
int HWCommon::SetDetectInfo(DETECT_INFO DetectInfo)
{
	return m_pMotionController->SetDetectInfo(DetectInfo);
}
#endif
#if((OHT_NAME == OHT_NAME_STD_V82)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
void HWCommon::SetClearFault(bool bClear)
{
	return m_pMotionController->SetClearFault(bClear);
}
#endif
/**
@brief   CompensateSlavePos 

	뒷바퀴를 앞바퀴 엔코더 값으로 강제로 맞추는 함수
	전후륜 싸움 방지 목적 (특히 소량 이동 시)
@author  임태웅
@date    2013-03-12
*/
int HWCommon::CompensateSlavePos()
{
	return m_pMotionController->CompensateSlavePos();
}

int HWCommon::SetPauseControl(int bIsPause, int nCommander)
{
    IsPauseControl = bIsPause;
	return m_pMotionController->SetPauseControl(bIsPause, nCommander);
}

bool HWCommon::GetPauseControl()
{
	return IsPauseControl;
}

void	HWCommon::makeTraceLog()
{
	m_pMotionController->makeTraceLog();
}
//---------------------------------------------------------------------------

/**
@brief   전압값 출력  함수
@author  KDS
@date    2017.11.29
@return  전압값 리턴
*/
int HWCommon::GetVolatageMonitor(int Index)
{
	return	m_pMotionController->GetVolatageMonitor(Index);
}

/**
@brief   AMC의 버전을 얻어오는 함수
@author  zzang9un
@date    2013.10.07
@return  AMC의 버전 정보(int)를 리턴
*/
int HWCommon::GetAMCVersion()
{
    return m_pMotionController->GetAMCVersion();
}
int HWCommon::GetAMCDLLVersion()
{
	return m_pMotionController->GetAMCDLLVersion();
}
//---------------------------------------------------------------------------

int HWCommon::GetFPGAVersion()
{
    return m_pMotionController->GetFPGAVersion();
}
// AMOHTV80F-1703
int HWCommon::GetRSAnSlaveVersion(int num)
{
	return m_pMotionController->GetRSAnSlaveVersion(num);
}

int HWCommon::GetAMCTorqueStatus()
{
	return m_pMotionController->GetTorqueStatusByAMC();
}

int HWCommon::GetAMCPauseStatus()
{
    return m_pMotionController->GetPauseStatusByAMC();
}

int HWCommon::GetAMCVHLDetectStatus()
{
    return m_pMotionController->GetVHLDetectStatusByAMC();
}

int HWCommon::GetAMCStopStatus()
{
    return m_pMotionController->GetStopStatusByAMC();
}
int HWCommon::GetAMCDecelStatus()
{
    return m_pMotionController->GetDecelStatusByAMC();
}
int HWCommon::GetAMCDetectStopDecelDownStatus()
{
	return m_pMotionController->GetDetectStopDecelDownStatusByAMC();
}
int HWCommon::GetVHLDetectForceIOStatus()
{
    return m_pMotionController->GetVHLDetectForceIO();
}

//AOHC-285
int HWCommon::GetLowVabraionStatus()
{
    return m_pMotionController->GetLowVibraionStatusByAMC();
}
/**
@brief   AMC의 FileSystem 전체를 포맷하기 위한 함수
@author  ji7412.lee
@date    2014-02-10
@param   nAxisNum : 축 번호
@return  Error Code
 */
int HWCommon::AMCFileSystemForamt()
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->AMCFileSystemForamt();
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 Drive변수를 강제로 Set/Reset 시키는 함수
@author  ji7412.lee
@date    2014-02-10
 */
int HWCommon::SetAMCDriverOpenByCommon(bool isOnOff)
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->SetAMCDriverOpenByCommon(isOnOff);
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 DonwLoadFile Flag를 Set 시키는 함수
@author  ji7412.lee
@date    2014-02-10
 */
int HWCommon::SetAMCFileDownloaded(bool isOnOff)
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->SetAMCFileDownloaded(isOnOff);
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 ClearWatchDogStatus 함수
@author  ji7412.lee
@date    2014-02-10
@note    Common에서 요청할수 있도록 Public 추가
 */
int HWCommon::ClearWatchDogStatus()
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->ClearWatchDogStatusPublic();
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 DisableWatchDog 함수
@author  ji7412.lee
@date    2014.02.10
@note    Common에서 요청할수 있도록 Public 추가
 */
int HWCommon::DisableWatchDog()
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->DisableWatchDogPublic();
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 File System Install 함수
@author  ji7412.lee
@date    2014.02.10
 */
int HWCommon::AMCFileSystemInstall(char *pszBootLoader, char *pszBootFile, int *prtn, HWND hWnd, UINT uiMsg)
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->AMCFileSystemInstall(pszBootLoader, pszBootFile, prtn, hWnd, uiMsg);
}
//---------------------------------------------------------------------------
/**
//AOHC-154
@brief  AMC Auto Update Test
@author  kds
@date    2020.07.03
 */
int HWCommon::AMCFileAutoUpdate(char *pszBootLoader, HWND hWnd)
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->AMCFileAutoUpdate(pszBootLoader,  hWnd);
}
//---------------------------------------------------------------------------
int HWCommon::AMCClose()
{
    	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->AMCClose();

}
/**
@brief   AMC의 DSP_Reboot 함수
@author  ji7412.lee
@date    2014.02.10
 */
int HWCommon::DSP_Reboot(void)
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->DSP_Reboot();
}


int HWCommon::SetSoundCommand(int nSound)
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->SetSoundCommand(nSound);
}
//---------------------------------------------------------------------------


