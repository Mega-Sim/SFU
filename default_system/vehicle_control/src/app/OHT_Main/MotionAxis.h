/**
@file    MotionAxis.h
@version OHT 7.0
@brief   MotionAxis Class Header File
*/

#ifndef MotionAxisH
#define MotionAxisH

#include "HWCommon.h"			// HW 정보 사용
#include "VirtualUnitConverter.h"	// 담위 표준화 추상화 클래스

typedef enum SPEED_TYPE_
{
	SPEED_OF_COMMAND	= 0,		///< 명령 속도
	SPEED_OF_CURRENT	= 1,		///< 현재 속도
	SPEED_OF_MIN		= 2,		///< Min (명령 속도, 현재 속도)
	SPEED_OF_MAX		= 3		///< Max (명령 속도, 현재 속도)
}SPEED_TYPE;

typedef	struct USER_MOVE_DS_ADDITIONAL_INFO_
{
	bool		bStartOnSpeedDown;
	SPEED_TYPE	UseCurrSpeed;

	bool		bUseSmallAdd;
	BYTE		byTriggerIONumber;
	bool		bIsTriggerOnRisingEdge;
	double 		dSmallDistance;
}USER_MOVE_DS_ADDITIONAL_INFO;

typedef enum SPEED_MODE_
{
	SPEED_MODE_NORMAL	= 0,
	SPEED_MODE_ACCEL	= 1,
	SPEED_MODE_DECEL	= 2,
	SPEED_MODE_CRUISE	= 3
}SPEED_MODE;

/**
@class   MotionAxis
@date    2012.10.17
@author  ehoto
@brief   Axis 관련 인터페이스 클래스
*/
class MotionAxis
{
private:
	int m_nAxisNum; ///< 축 번호
	HWCommon *m_pHWCommon;
	bool m_bAbsoluteEncoder;

	double m_dDistOfMoveDistance;	///< MoveDS 명령 시 남은 거리 

protected:
	VirtualUnitConverter *m_pUnitConverter;

	virtual double ConvertDistanceUnit(double dInput, bool bUserUnit);		///< InputType Flag = bUserInput ( true:UserUnit / false:Cts )
	virtual double ConvertSpeedUnit(double dInput, bool bUserUnit);				///< InputType Flag = bUserInput ( true:UserUnit / false:Cts )
	virtual double ConvertAccelerationUnit(double dInput, bool bUserUnit);	///< InputType Flag = bUserInput ( true:UserUnit / false:Cts )

public:
	MotionAxis();
	MotionAxis(HWCommon *pHWCommon, int nAxisNum, bool bAbsoluteEncoder);
	virtual ~MotionAxis();

	/// 초기화
	virtual int Init();
	virtual bool EnableUnitConverter(void) = 0;
	virtual void DisableUnitConverter(void);

	/// 상태 확인
	virtual bool IsRunning();
	virtual bool IsInPosition();
	virtual bool IsAxisDone();
	virtual bool IsAMPEnable();
	virtual double GetCurrentPosition();
	virtual double GetPrePosition();
	virtual double GetRunningPosition(bool bInit, bool bConvert);
	virtual int GetTotalMoveCount(bool bInit);
	virtual double GetFinalPosition();	
	virtual double GetCurrentVelocity();
	virtual double GetFollowingError();
	virtual double GetFollowingError_cts();
	virtual double GetSmallAddCheckPosition();	
	virtual double GetDistOfRemain();
	SPEED_MODE GetSpeedMode();

	/// 에러 관련
	virtual int GetError();
	virtual int Reset();

	/// Offset 관련
	virtual int SetHome();

	/// Limit 관련
	virtual int SetPositiveSWLimit(double dLimit);
	virtual int SetNegativeSWLimit(double dLimit);
	virtual int GetSWLimit(double& dPosLimit, double& dNegLimit);
	virtual int ResetSWLimit();

	/// 구동 관련 - 단위: 거리(mm), 속도(mm/msec), 가속도(mm/msec2), 시간(msec)
	virtual int SetAMPEnable(bool bValue);
	// 가속도 기반
	virtual int MovePositive(double dAccel, double dVel);
	virtual int MoveNegative(double dAccel, double dVel);
	virtual int MoveStop(double dAccel);
	virtual int MoveDistance(double dAccel_Up, double dAccel_Down, double dVel, double dDistance);
	virtual int MoveDistance(double dAccel_Up, double dAccel_Down, double dVel, double dDistance, USER_MOVE_DS_ADDITIONAL_INFO AdditionalInfo);
	// 가속 시간 기반
	virtual int MovePositive(DWORD dwTime, double dVel);
	virtual int MoveNegative(DWORD dwTime, double dVel);
	virtual int MoveStop(DWORD dwDrivingTime);
	virtual int MoveDistance(DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance);

	// 통신으로 제어시에서 구현 시 사용 함수
	virtual int SendData(char* pSendData, int nSendLen, char* pRecvData, int* nRecvLen);	///< Data 전송

	// mm단위로 이동하는 것이 아니라 pulse로 이동하도록 하는 함수 (Hoist 티칭을 위함)
	virtual int MoveDistance_Pulse(DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance);
};
 
#endif  // MotionAxisH


