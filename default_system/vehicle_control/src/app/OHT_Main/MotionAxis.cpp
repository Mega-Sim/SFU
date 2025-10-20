/**
@file    MotionAxis.cpp
@version OHT 7.0
@brief   MotionAxis Class CPP File
*/

#include <math.h>
#include "MotionAxis.h"
#include "LogHandler.h"

#define ERR_NO_HWCOMMON	-1

/**
@brief   MotionAxis Class 디폴트 생성자
@author
@date    2012-11-01
*/
MotionAxis::MotionAxis()
{
	m_nAxisNum = 0;
	m_pHWCommon = NULL;
	m_pUnitConverter = NULL;
	m_bAbsoluteEncoder = false;
	m_dDistOfMoveDistance = 0.0;
}

/**
@brief   MotionAxis Class 생성자
@author
@date    2012-11-01
@param   pHWCommon : HWCommon 클래스 포인터
@param   nAxisNum : Motor에 사용될 축 번호
*/
MotionAxis::MotionAxis(HWCommon *pHWCommon, int nAxisNum, bool bAbsoluteEncoder)
{
	m_nAxisNum = 0;
	m_pHWCommon = NULL;
	m_pUnitConverter = NULL;
	m_bAbsoluteEncoder = bAbsoluteEncoder;
	m_dDistOfMoveDistance = 0.0;

	if(nAxisNum>=0)
	{
		m_nAxisNum = nAxisNum;
	}
	if(m_pHWCommon==NULL)
	{
		m_pHWCommon = pHWCommon;
	}

}

/**
@brief   MotionAxis Class 소멸자
@author
@date    2012-11-01
*/
MotionAxis::~MotionAxis()
{
	m_pUnitConverter = NULL;
	m_pHWCommon = NULL;
	m_nAxisNum = 0;
}

/**
@brief   MotionAxis 초기화 함수
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
int MotionAxis::Init()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	return m_pHWCommon->Init(m_nAxisNum);
}

/**
@brief   UnitConverter 미사용 설정 함수
@author  ehoto
@date    2012-11-01
*/
void MotionAxis::DisableUnitConverter(void)
{
	m_pUnitConverter = NULL;
}

/**
@brief   UnitConverter
@author  ehoto
@date    2012-11-01
@param	dUserUnitDistance 사용자 단위 거리
@return	제어기 사용 거리
*/
double MotionAxis::ConvertDistanceUnit(double dInput, bool bUserUnit)
{
	if(m_pUnitConverter!=NULL)
	{
		if(bUserUnit==true)
		{
			return m_pUnitConverter->calcUser2Cts(dInput);	// User --> Cts
		}
		else
		{
			return m_pUnitConverter->calcCts2User(dInput);
		}
	}
	else
	{
		return dInput;
	}
}

/**
@brief   UnitConverter
@author  ehoto
@date    2012-11-01
@param	dUserUnitSpeed 사용자 단위 속도
@return	제어기 사용 속도
*/
double MotionAxis::ConvertSpeedUnit(double dInput, bool bUserUnit)
{
	if(m_pUnitConverter!=NULL)
	{
		if(bUserUnit==true)
		{
			return m_pUnitConverter->calcUser2Cts(dInput);	// User/msec --> Cts/msec
		}
		else
		{
			return m_pUnitConverter->calcCts2User(dInput);	// Cts/msec --> User/msec
		}
	}
	else
	{
		return dInput;
	}
}

/**
@brief   UnitConverter
@author  ehoto
@date    2012-11-01
@param	dUserUnitSpeed 사용자 단위 속도
@return	제어기 사용 속도
*/
double MotionAxis::ConvertAccelerationUnit(double dInput, bool bUserUnit)
{
	if(m_pUnitConverter!=NULL)
	{
		if(bUserUnit==true)
		{
			return m_pUnitConverter->calcUser2Cts(dInput);	// User/msec2 --> Cts/msec2
		}
		else
		{
			return m_pUnitConverter->calcCts2User(dInput);	// Cts/msec2 --> User/msec2
		}
	}
	else
	{
		return dInput;
	}
}

/// 상태 확인
/**
@brief   MotionAxis 동작 확인 하는 함수
@author  ehoto
@date    2012-11-01
@return  동작중이면 true / 비동작이면 false
*/
bool MotionAxis::IsRunning()
{
	if(m_pHWCommon==NULL) return false;
	return m_pHWCommon->IsRunning(m_nAxisNum);
}

/**
@brief   MotionAxis 정위치 확인 함수
@author  ehoto
@date    2012-11-01
@return  정위치이면 true / 정위치 아니면 false
*/
bool MotionAxis::IsInPosition()
{
	if(m_pHWCommon==NULL) return false;
	return m_pHWCommon->IsInPosition(m_nAxisNum);
}

/**
@brief   MotionAxis 명령 실행 완료 확인 함수
@author  ehoto
@date    2012-11-01
@return  완료이면 true / 완료 아니면 false
*/
bool MotionAxis::IsAxisDone()
{
	if(m_pHWCommon==NULL) return false;
	if(IsInPosition()==true && IsRunning()==false) return true;	//1	나중에 AMC 에서 값 받아오도록 변경
	else return false;
}


/**
@brief   MotionAxis AMP On/Off 상태 확인 함수
@author  ehoto
@date    2012-11-01
@return  동작 대기 상태면 true / 정지 상태면 false ( ex: Servi On / Off )
*/
bool MotionAxis::IsAMPEnable()
{
	if(m_pHWCommon==NULL) return false;

	return m_pHWCommon->IsAMPEnable(m_nAxisNum);
}

/**
@brief   MotionAxis 현재 위치 확인 함수
@author  ehoto
@date    2012-11-01
@return  현재 위치를 실수로 돌려줌
*/
double MotionAxis::GetCurrentPosition()
{
	if(m_pHWCommon==NULL) return 0.0;
	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetCurrentPosition(m_nAxisNum);
	}
	else
	{
		return ConvertDistanceUnit(m_pHWCommon->GetCurrentPosition(m_nAxisNum), false);	// cts --> unit
	}
}

/**
@brief   MotionAxis 현재 이전 위치 확인 함수
@author  doori.shin
@date    2016.04.26
@return  현재 이전 위치를 실수로 돌려줌
*/
double MotionAxis::GetPrePosition()
{
	if(m_pHWCommon==NULL) return 0.0;
	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetPrePosition(m_nAxisNum);
	}
	else
	{
		return ConvertDistanceUnit(m_pHWCommon->GetPrePosition(m_nAxisNum), false);	// cts --> unit
	}
}

/**
@brief   MotionAxis 현재 누적 거리를 가져옴
@author  doori.shin
@date    2016.04.26
@param	 bInit : 누적거리를 가져온 뒤 초기화 여부
@return  현재 이동 누적거리를 실수로 돌려줌
*/
double MotionAxis::GetRunningPosition(bool bInit, bool bConvert)
{
	double dReturnValue = 0.0;

	if(m_pHWCommon == NULL)
	{
		dReturnValue = 0.0;
	}
	else
	{
		//값 변형하는지 확인
		if((bConvert == ON) && (m_pUnitConverter != NULL))
		{
			dReturnValue = ConvertDistanceUnit(m_pHWCommon->GetTotalPosition(m_nAxisNum), false);
		}
		else
		{
			dReturnValue = m_pHWCommon->GetTotalPosition(m_nAxisNum);
		}

		//누적거리 초기화
		if(bInit == true)
		{
			m_pHWCommon->ResetTotalPosition(m_nAxisNum);
        }
	}

	return dReturnValue;
}

/**
@brief   MotionAxis 현재 누적 움직인 횟수를 가져옴
@author  doori.shin
@date    2016.04.26
@param	 bInit : 값을 가져온 뒤 초기화 여부
@return  현재 누적 움직인 횟수를 돌려줌
*/
int MotionAxis::GetTotalMoveCount(bool bInit)
{
	int nReturnValue = 0;

	if(m_pHWCommon == NULL)
	{
        nReturnValue = 0;
	}
	else
	{
		nReturnValue = m_pHWCommon->GetTotalMoveCount(m_nAxisNum);

		if(bInit == true)
		{
			m_pHWCommon->ResetTotalMoveCount(m_nAxisNum);
        }
	}

	return nReturnValue;
}


/**
@brief   MotionAxis Small Add Move 시 센서가 감지된 위치 확인 함수
@author  ehoto
@date    2012-11-01
@return  Small Add Move 시 센서가 감지된 위치를 표시함
*/
double MotionAxis::GetSmallAddCheckPosition()
{
	if(m_pHWCommon==NULL) return 0.0;
	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetSmallAddCheckPosition(m_nAxisNum);
	}
	else
	{
		return ConvertDistanceUnit(m_pHWCommon->GetSmallAddCheckPosition(m_nAxisNum), false);	// cts --> unit
	}
}


/**
@brief	 MotionAxis 현재 위치 확인 함수
@author  ehoto
@date	 2012-11-01
@return  현재 위치를 실수로 돌려줌
*/
double MotionAxis::GetFinalPosition()
{
	if(m_pHWCommon==NULL) return 0.0;
	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetFinalPosition(m_nAxisNum);
	}
	else
	{
		return ConvertDistanceUnit(m_pHWCommon->GetFinalPosition(m_nAxisNum), false); // cts --> unit
	}
}


/**
@brief   MotionAxis 현재 속도 확인 함수
@author  ehoto
@date    2012-11-01
@return  현재 속도를 실수로 돌려줌
*/
double MotionAxis::GetCurrentVelocity()
{
	if(m_pHWCommon==NULL) return 0.0;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetCurrentVelocity(m_nAxisNum);
	}
	else
	{
		return ConvertSpeedUnit(m_pHWCommon->GetCurrentVelocity(m_nAxisNum), false);	// cts/msec --> unit/msec
	}
}

/**
@brief   MotionAxis Following Error 확인 함수
@author  ehoto
@date    2012-11-01
@return  Following Error를 실수로 보내줌
*/
double MotionAxis::GetFollowingError()
{
	if(m_pHWCommon==NULL) return 0.0;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetFollowingError(m_nAxisNum);
	}
	else
	{
		return ConvertDistanceUnit(m_pHWCommon->GetFollowingError(m_nAxisNum), false);	// cts --> unit
	}
}

double MotionAxis::GetFollowingError_cts()
{
	if(m_pHWCommon==NULL) return 0.0;

	return m_pHWCommon->GetFollowingError(m_nAxisNum);
}


/**
@brief   MoveDistance 명령의 남은 거리를 Return 하는 함수
@author  ehoto
@date    2013-02-03
@return  MoveDistance 명령의 남은 거리를 실수로 보내줌
*/
double MotionAxis::GetDistOfRemain()
{
	double dDistOfRemain = GetFinalPosition()-GetCurrentPosition();	//m_dDistOfMoveDistance-
//	double dDistOfRemain = m_dDistOfMoveDistance-GetCurrentPosition();
	return dDistOfRemain;
}

/// 에러 관련
/**
@brief   MotionAxis Error 확인 함수
@author  ehoto
@date    2012-11-01
@return  축별 Error Code
*/
int MotionAxis::GetError()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	return m_pHWCommon->GetError(m_nAxisNum);
}

/**
@brief   MotionAxis reset 함수
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
int MotionAxis::Reset()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	return m_pHWCommon->Reset(m_nAxisNum);
}

/// Offset 관련
/**
@brief   MotionAxis 현재 위치 Home 설정 함수
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
int MotionAxis::SetHome()
{
	double dTmpOffset = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;


	if(m_bAbsoluteEncoder==false)
	{
		//WRITE_LOG_ON_THIS_FILE("D:\\log\\OriginTest.log", "SetHome m_bAbsoluteEncoder==false");
		m_dDistOfMoveDistance = 0.0;
		return m_pHWCommon->SetHome(m_nAxisNum);
	}
	else
	{
		//WRITE_LOG_ON_THIS_FILE("D:\\log\\OriginTest.log", "SetHome m_bAbsoluteEncoder==true");
		m_pHWCommon->GetOffset(m_nAxisNum, &dTmpOffset);
		//WRITE_LOG_ON_THIS_FILE("D:\\log\\OriginTest.log", "SetHome_GetOffset Axis=%d, Offset=%7.1lf", m_nAxisNum, dTmpOffset);
		dTmpOffset += m_pHWCommon->GetCurrentPosition(m_nAxisNum); 	// 현재 위치를 Home으로 만듬
		//WRITE_LOG_ON_THIS_FILE("D:\\log\\OriginTest.log", "SetHome_SetOffset Axis=%d, Offset=%7.1lf", m_nAxisNum, dTmpOffset);
		return m_pHWCommon->SetOffset(m_nAxisNum, dTmpOffset);
	}
}

/// Limit 관련
/**
@brief   MotionAxis Postive S/W Limit 설정 함수
@author  ehoto
@date    2012-11-01
@param   dLimit : Limit 거리
@return  Error Code
*/
int MotionAxis::SetPositiveSWLimit(double dLimit)
{
	if(m_pHWCommon==NULL) return -1;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->SetPositiveSWLimit(m_nAxisNum, dLimit);
	}
	else
	{
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->SetPositiveSWLimit(m_nAxisNum, ConvertDistanceUnit(dLimit, true));
		}
		else
		{
			return m_pHWCommon->SetNegativeSWLimit(m_nAxisNum, ConvertDistanceUnit(dLimit, true));
		}
	}
}

/**
@brief   MotionAxis Negative S/W Limit 설정 함수
@author  ehoto
@date    2012-11-01
@param   dLimit : Limit 거리
@return  Error Code
*/
int MotionAxis::SetNegativeSWLimit(double dLimit)
{
	if(m_pHWCommon==NULL) return -1;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->SetNegativeSWLimit(m_nAxisNum, dLimit);
	}
	else
	{
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->SetNegativeSWLimit(m_nAxisNum, ConvertDistanceUnit(dLimit, true));
		}
		else
		{
			return m_pHWCommon->SetPositiveSWLimit(m_nAxisNum, ConvertDistanceUnit(dLimit, true));
		}
	}
}

/**
@brief   MotionAxis S/W Limit 확인 함수
@author  ehoto
@date    2012-11-01
@param   dPosLimit : Positive Limit 거리
@param   dNegLimit : Negative Limit 거리
@return  Error Code
*/
int MotionAxis::GetSWLimit(double& dPosLimit, double& dNegLimit)
{
	double dTmpPosLimit = 0.0, dTmpNegLimit = 0.0;
	int nReturnValue = 0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetSWLimit(m_nAxisNum, dPosLimit, dNegLimit);
	}
	else
	{
		nReturnValue = m_pHWCommon->GetSWLimit(m_nAxisNum, dTmpPosLimit, dTmpNegLimit);
		if(m_pUnitConverter->isSameDirection()==true)
		{
			 dPosLimit= ConvertDistanceUnit(dTmpPosLimit, false);
			 dNegLimit= ConvertDistanceUnit(dTmpNegLimit, false);
		}
		else
		{
			dNegLimit= ConvertDistanceUnit(dTmpPosLimit, false);
			dPosLimit= ConvertDistanceUnit(dTmpNegLimit, false);
		}
		return nReturnValue;
	}
}

/**
@brief   MotionAxis S/W Limit 해제 함수
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
int MotionAxis::ResetSWLimit()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	return m_pHWCommon->ResetSWLimit(m_nAxisNum);
}

// 구동 관련 - 단위: 거리(mm), 속도(mm/msec), 가속도(mm/msec2), 시간(msec)
/**
@brief   MotionAxis Amp On/Off 설정 함수
@author  ehoto
@date    2012-11-01
@param   bValue : On/Off 값
@return  Error Code
*/
int MotionAxis::SetAMPEnable(bool bValue)
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	return m_pHWCommon->SetAMPEnable(m_nAxisNum, bValue);
}

// 가속도 기반
/**
@brief   MotionAxis 양의 방향으로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   dAccel : 가속도
@param   dVel : 속도
@return  Error Code
*/
int MotionAxis::MovePositive(double dAccel, double dVel)
{
	double dTmpAccel = 0.0, dTmpVel = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MovePositive(m_nAxisNum, dAccel, dVel);
	}
	else
	{
//		 dTmpAccel = fabs(ConvertAccelerationUnit(dAccel, true));	// User/msec2 --> Cts/msec2
		 dTmpAccel = ConvertAccelerationUnit(dAccel, true);	// User/msec2 --> Cts/msec2
		 dTmpAccel = fabs(dTmpAccel);
		 dTmpVel = fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->MovePositive(m_nAxisNum, dTmpAccel, dTmpVel);
		}
		else
		{
			return m_pHWCommon->MoveNegative(m_nAxisNum, dTmpAccel, dTmpVel);
		}
	}

}

/**
@brief   MotionAxis 음의 방향으로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   dAccel : 가속도
@param   dVel : 속도
@return  Error Code
*/
int MotionAxis::MoveNegative(double dAccel, double dVel)
{
	double dTmpAccel = 0.0, dTmpVel = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MoveNegative(m_nAxisNum, dAccel, dVel);
	}
	else
	{
		 dTmpAccel = fabs(ConvertAccelerationUnit(dAccel, true));	// User/msec2 --> Cts/msec2
		 dTmpVel = fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->MoveNegative(m_nAxisNum, dTmpAccel, dTmpVel);
		}
		else
		{
			return m_pHWCommon->MovePositive(m_nAxisNum, dTmpAccel, dTmpVel);
		}
	}

}

/**
@brief   MotionAxis 정지하는 함수
@author  ehoto
@date    2012-11-01
@param   dAccel : 감속도
@return  Error Code
*/
int MotionAxis::MoveStop(double dAccel)
{
	double dTmpAccel = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MoveStop(m_nAxisNum, dAccel);
	}
	else
	{
		dTmpAccel = fabs(ConvertAccelerationUnit(dAccel, true));	// User/msec2 --> Cts/msec2
		return m_pHWCommon->MoveStop(m_nAxisNum, dTmpAccel);
	}

}

/**
@brief   MotionAxis 거리를 가속도/감속도/속도 프로파일로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   dAccel_Up : 가속도
@param   dAccel_Down : 감속도
@param   dVel : 속도
@param   dDistance : 이동 거리
@return  Error Code
*/
int MotionAxis::MoveDistance(double dAccel_Up, double dAccel_Down, double dVel, double dDistance)
{
	USER_MOVE_DS_ADDITIONAL_INFO AdditionalInfo;
	memset(&AdditionalInfo, 0, sizeof(USER_MOVE_DS_ADDITIONAL_INFO));
	return MoveDistance(dAccel_Up, dAccel_Down, dVel, dDistance, AdditionalInfo);
}


/**
@brief   MotionAxis 거리를 가속도/감속도/속도 프로파일로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   dAccel_Up : 가속도
@param   dAccel_Down : 감속도
@param   dVel : 속도
@param   dDistance : 이동 거리
@return  Error Code
*/
int MotionAxis::MoveDistance(double dAccel_Up, double dAccel_Down, double dVel, double dDistance, USER_MOVE_DS_ADDITIONAL_INFO AdditionalInfo)
//int MotionAxis::MoveDistance(double dAccel_Up, double dAccel_Down, double dVel, double dDistance, TRIGGER_CTRL_INFO TriggerCtrlInfo)
{
	double dTmpAccel_Up = 0.0, dTmpAccel_Down = 0.0, dTmpVel = 0.0, dTmpDistance = 0.0;
	int	nTmpSmallDistance = 0;
	TRIGGER_CTRL_INFO TriggerCtrlInfo;
	memset(&TriggerCtrlInfo, 0, sizeof(TRIGGER_CTRL_INFO));

	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	m_dDistOfMoveDistance = dDistance;	// 명령 거리를 백업함

	if(m_pUnitConverter==NULL)
	{
		TriggerCtrlInfo.StartOnSpeedDown		= AdditionalInfo.bStartOnSpeedDown;
		TriggerCtrlInfo.UseCurrSpeed			= AdditionalInfo.UseCurrSpeed;	

		TriggerCtrlInfo.UseSmallAdd				= AdditionalInfo.bUseSmallAdd;	
		TriggerCtrlInfo.TriggerIONumber			= AdditionalInfo.byTriggerIONumber;
		TriggerCtrlInfo.IsTriggerOnRisingEdge	= AdditionalInfo.bIsTriggerOnRisingEdge;
		nTmpSmallDistance 						= (int)(AdditionalInfo.dSmallDistance);
		TriggerCtrlInfo.AdditionalDist			= ((nTmpSmallDistance>524287)?524287:nTmpSmallDistance);	// 524287 = 2^19-1

		return m_pHWCommon->MoveDistance(m_nAxisNum, dAccel_Up, dAccel_Down, dVel, dDistance, TriggerCtrlInfo);
	}
	else
	{
		dTmpAccel_Up	= fabs(ConvertAccelerationUnit(dAccel_Up, true));	// User/msec2 --> Cts/msec2
		dTmpAccel_Down	= fabs(ConvertAccelerationUnit(dAccel_Down, true));	// User/msec2 --> Cts/msec2
		dTmpVel			= fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		dTmpDistance	= ConvertDistanceUnit(dDistance, true);	// User --> Cts

		TriggerCtrlInfo.StartOnSpeedDown		= AdditionalInfo.bStartOnSpeedDown;
		TriggerCtrlInfo.UseCurrSpeed			= AdditionalInfo.UseCurrSpeed;	

		TriggerCtrlInfo.UseSmallAdd				= AdditionalInfo.bUseSmallAdd;	
		TriggerCtrlInfo.TriggerIONumber			= AdditionalInfo.byTriggerIONumber;
		TriggerCtrlInfo.IsTriggerOnRisingEdge	= AdditionalInfo.bIsTriggerOnRisingEdge;
		nTmpSmallDistance 						= (int)(ConvertDistanceUnit(AdditionalInfo.dSmallDistance, true));	// User --> Cts
		TriggerCtrlInfo.AdditionalDist			= ((nTmpSmallDistance>524287)?524287:nTmpSmallDistance);	// 524287 = 2^19-1
		
		return m_pHWCommon->MoveDistance(m_nAxisNum, dTmpAccel_Up, dTmpAccel_Down, dTmpVel, dTmpDistance, TriggerCtrlInfo);
	}

}

// 가속 시간 기반
/**
@brief   MotionAxis 일정 시간 가속하여 양의 방향으로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   dwDrivingTime : 가속 시간
@param   dVel : 속도
@return  Error Code
*/
int MotionAxis::MovePositive(DWORD dwTime, double dVel)
{
	double dTmpVel = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MovePositive(m_nAxisNum, dwTime, dVel);
	}
	else
	{
		 dTmpVel = fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->MovePositive(m_nAxisNum, dwTime, dTmpVel);	// msec -->msec
		}
		else
		{
			return m_pHWCommon->MoveNegative(m_nAxisNum, dwTime, dTmpVel);	// msec -->msec
		}
	}

}

/**
@brief   MotionAxis 일정 시간 가속하여 음의 방향으로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   dwDrivingTime : 가속 시간
@param   dVel : 속도
@return  Error Code
*/
int MotionAxis::MoveNegative(DWORD dwTime, double dVel)
{
	double dTmpVel = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MoveNegative(m_nAxisNum, dwTime, dVel);
	}
	else
	{
		 dTmpVel = fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->MoveNegative(m_nAxisNum, dwTime, dTmpVel);	// msec -->sec
		}
		else
		{
			return m_pHWCommon->MovePositive(m_nAxisNum, dwTime, dTmpVel);	// msec -->sec
		}
	}

}

/**
@brief   MotionAxis 일정 시간 감속하여 정지하는 함수
@author  ehoto
@date    2012-11-01
@param   dwDrivingTime : 감속 시간
@param   dVel : 속도
@return  Error Code
*/
int MotionAxis::MoveStop(DWORD dwTime)
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	return m_pHWCommon->MoveStop(m_nAxisNum, dwTime);
}

/**
@brief   MotionAxis 거리를 가속시간/감속시간/속도 프로파일로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   dwTime_Up : 가속 시간
@param   dwTime_Down : 감속 시간
@param   dVel : 속도
@param   dDistance : 이동 거리
@return  Error Code
*/
int MotionAxis::MoveDistance(DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance)
{
	double	dTmpVel = 0.0, dTmpDistance = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	m_dDistOfMoveDistance = dDistance;	// 명령 거리를 백업함

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MoveDistance(m_nAxisNum, dwTime_Up, dwTime_Down, dVel, dDistance);
	}
	else
	{
		 dTmpVel = fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		 dTmpDistance = ConvertDistanceUnit(dDistance, true);	// User --> Cts
		 return m_pHWCommon->MoveDistance(m_nAxisNum, dwTime_Up, dwTime_Down, dTmpVel, dTmpDistance);	// User --> Cts, msec -->sec
	}

}

// 통신으로 제어시에서 구현 시 사용 함수
/**
@brief   MotionAxis 에 프로토콜로 명령 전달
@author  ehoto
@date    2012-11-01
@param   pSendData : 보내는 Data 가 들어있는 Char 배열
@param   nSendLen : 보내는 Data 길이
@param   pRecvData : 받는 Data 가 들어있는 Char 배열
@param   nRecvLen : 받는 Data 길이
@return  Error Code
*/
int MotionAxis::SendData(char* pSendData, int nSendLen, char* pRecvData, int* nRecvLen)
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	return m_pHWCommon->SendData(m_nAxisNum, pSendData, nSendLen, pRecvData, nRecvLen);
}



// mm단위로 이동하는 것이 아니라 pulse로 이동하도록 하는 함수 (Hoist 티칭을 위함)
int MotionAxis::MoveDistance_Pulse(DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance)
{	
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	m_dDistOfMoveDistance = dDistance;	// 명령 거리를 백업함
	return m_pHWCommon->MoveDistance(m_nAxisNum, dwTime_Up, dwTime_Down, dVel, dDistance);
}

SPEED_MODE MotionAxis::GetSpeedMode()
{
	SPEED_MODE returnMode = SPEED_MODE_NORMAL;
	static double dStartTime = clock();
	static double dPreSpeed = GetCurrentVelocity() * 1000;
	double dDelta = (fabs(GetCurrentVelocity() * 1000) - dPreSpeed)/100;
	double dCheckTime = clock() - dStartTime;
	if(dCheckTime > 50)
	{
		if(fabs(dDelta) < 0.5)
		{
			if(fabs(GetCurrentVelocity()) < 0.05)
				returnMode = SPEED_MODE_NORMAL;
			else
				returnMode = SPEED_MODE_CRUISE;
		}else if(dDelta >= 0.5)
		{
			returnMode = SPEED_MODE_ACCEL;
		}else
		{
		   returnMode = SPEED_MODE_DECEL;
		}

		dPreSpeed  =fabs(GetCurrentVelocity() * 1000);

		dStartTime = clock();
	}

	return returnMode;
}
