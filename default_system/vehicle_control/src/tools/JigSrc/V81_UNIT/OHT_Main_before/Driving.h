/**
@file    Driving.h
@version OHT 7.0
@brief   Driving Class Header File
*/

#ifndef DrivingH
#define DrivingH

#include "HWCommon.h"			// HW 정보 사용
#include "PropotionalMotionAxis.h"	// 비례 제어 축 상속 
#include "Def_Driving.h"			// 정보 전달 구조체


/**
@class   Driving
@date    2012.10.17
@author  ehoto
@brief   Driving Motor 관련 Class
@note    MotionAxis Class 상속
*/
class Driving : public PropotionalMotionAxis	//MotionAxis
{
//1	변수(Member)
private:
	HWCommon *m_pHWCommon;	///< HW의 정보를 획득
	
	int m_nMasterAxisNum, m_nSlaveAxisNum;	///< 각 축의 번호
	bool m_bSync;							///< 동기 제어 여부
	/*
	목적지 위치를 확인하여 AMC에 정보를 송/수신하는 부분이며 최종 목적지일경우만 설정함.
	false면 해제,   true면 설정.
    */
	bool 	m_bTargetPos2AMC;
	bool m_Curve_E_Stop;

public:
	PropotionalMotionAxis *m_pSlaveAxis;	///< 주행 2번째 휠

//1	생성자/소멸자 (Constructor/Destructor)
public:	
	Driving(HWCommon *pHWCommon, int nAxisNum);						///< 생성자: 단축제어 , 기어비 없음 
	Driving(HWCommon *pHWCommon, int nAxisNum, double dGearRatio);		///< 생성자: 단축제어 , 기어비 있음 
	
	Driving(HWCommon *pHWCommon, int nMasterAxisNum, int nSlaveAxisNum);						///< 생성자: 다축제어 , 기어비 없음 
	Driving(HWCommon *pHWCommon, int nMasterAxisNum, int nSlaveAxisNum, double dGearRatio);	///< 생성자: 다축제어 , 기어비 있음 
	
	~Driving();



//1	함수(Method)
public:
	bool	SetGearRatio(PROPOTINAL_GEAR_RATIO GearRatio);	///< Master, Slave 동시 처리를 위한 Overriding
	bool	EnableUnitConverter(void);			///< Master, Slave 동시 처리를 위한 Overriding


	bool	IsBrakeReleased();				///< IO정보 : Brake 상태 확인
	int		SetBrakeRelease(bool bValue);		///< IO정보 : Brake 상태 설정


	void	ClearNodeCountInfo(void);									///< Motion 제어기 에서 고속으로 Node/Station 정보를 처리하기 위한 함수: 정보 초기화
	int		GetNodeCountInfo(USER_NODE_CHECK_DATA &UserNodeCheckData);	///< Motion 제어기 에서 고속으로 Node/Station 정보를 처리하기 위한 함수: 정보 획득
	int		SetDetectInfo(USER_DETECT_INFO UserDetectInfo);		///< 근접제어를 위한 정보 전달 함수:  전방 감지 정보 설정

	void	SetTorqueLimit();
	void	SetTorqueLimitRelease();

	void	SetSlopeTime();
	void	SetSlopeTimeRelease();

	void	SetTargetPos();
	void	SetTargetPosRelease();
	bool	IsTargetPos();

	void	SetFirstNodeCountAdd();
	void	SetVHLDetectIgnor();
	void	SetVHLDetectIgnorRelease();

	int		GetFollowing_Axis1();          ///<AOHC-196 사전 로그 추가

	void	SetDetectStopDecelDown();
	void	SetDetectStopDecelDownRelease();

	void	SetVHLDetectIOSet(bool bIoSet);

	bool	IsSyncControl(void);			///< 동기 제어를 위한 함수: 동기 제어인지 확인 목적
	int		CompensateSlavePos();		///< 동기 제어를 위한 함수: 후륜 엔코더를 전륜에 맞춤 - 현재 미사용

	
	void SetCurveEstop(bool value);
	bool GetCurveEstop(void);

#if 1	//1	↓추후 제거 예정 함수들
	bool	IsSlaveAMPEnable();
	int		SetSlaveAMPEnable(bool bValue);
	int		SetSlaveHome(void);
	bool	IsSlaveRunning();
	double	GetSlaveCurrentPosition();
	double	GetSlaveCurrentVelocity();
	int		GetSlaveError();
	int		ResetSlave();
#endif

};

#endif  //DrivingH

