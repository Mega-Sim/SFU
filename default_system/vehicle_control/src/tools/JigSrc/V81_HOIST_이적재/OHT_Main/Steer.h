/**
@file    Steer.h
@version OHT 7.0
@brief   Steer Class Header File
*/

#ifndef SteerH
#define SteerH

#include <System.hpp>
#include "HWCommon.h"
#include "Utility.h"

#include "OHTMainStatus.h"
#include "ExecuteInfo.h"

/**
@brief	 Steer 방향 정보
@author  ehoto
@date	 2013.07.01
*/
typedef enum HW_STEER_DIRECTION_
{
	HW_STEER_DIRECTION_NONE		= 0,
	HW_STEER_DIRECTION_LEFT		,
	HW_STEER_DIRECTION_RIGHT	,	
	HW_STEER_DIRECTION_ERROR
}HW_STEER_DIRECTION;

/**
@brief	 Steer 방향 정보
@author  ehoto
@date	 2013.07.01
*/
typedef enum HW_STEER_DIRECTION_CMD_
{
	HW_STEER_DIRECTION_CMD_LEFT		= HW_STEER_DIRECTION_LEFT,
	HW_STEER_DIRECTION_CMD_RIGHT	= HW_STEER_DIRECTION_RIGHT	
}HW_STEER_DIRECTION_CMD;

/**
@brief	 Steer 방향 정보
@author  ehoto
@date	 2013.07.01
*/
typedef enum HW_STEER_TORQUE_
{
	HW_STEER_TORQUE_FREE		= 0,
	HW_STEER_TORQUE_STOP		,
	HW_STEER_TORQUE_MOVING		,
	HW_STEER_TORQUE_TRACTION	
}HW_STEER_TORQUE;


/**
@class   Steer
@date    LSJ
@author  2012.11.01
@brief   Steer 관련 Class
*/
class Steer
{
private:
	HWCommon *m_pHWCommon;
	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;

	FDCUnit *pFDCUnit;

    bool bReadyOperation;
	bool bLongRun;
//	int nMovingCount;
//	DWORD dwMaxFrontMovingTime;
//	DWORD dwMaxRearMovingTime;
	int nSendCommand;
	DWORD dwStartTime;
    DWORD dwOriginLimitTime;
	DWORD dwMotionLimitTime;

	//신규 진단데이터
	int m_nMovingCount;
	CRITICAL_SECTION m_MovCntCS;
    
public:
    Steer(HWCommon *pHWCommon);
    ~Steer();
        
	void Init();
    int Adjust();
    int Left(bool bTraction);
    int Right(bool bTraction);

    //Front Rear 조향 독립제어
    int FrontLeft();
    int RearLeft();
    int FrontRight();
    int RearRight();
    int SendFrontCmd(bool bSteer); //0: left, 1: right
    int SendRearCmd(bool bSteer);


    int CheckComplete();
    int CheckSensor(int nSteerPos);
    int SendLeftCmd();
    int SendRightCmd();
    int SendLeftCmdWeakly();
    int SendRightCmdWeakly();
    int Stop();
    int Resume();
    int SendTractionLeftCmd();
    int SendTractionRightCmd();
    int FrontLeftStop();
    int RearLeftStop();
    int FrontRightStop();
    int RearRightStop();
    int FrontFree();
    int RearFree();
    int FrontEnable();
    int RearEnable();
    bool IsLongRun();
	void StartLongRun();
	void StopLongRun();
//	int GetMovingCount();
//	DWORD GetMaxFrontMovingTime();
//	DWORD GetMaxRearMovingTime();
	bool IsLeftPos(int nFrontRear);
	bool IsRightPos(int nFrontRear);
	bool IsLeftCmd(int nFrontRear);
	bool IsRightCmd(int nFrontRear);
    int Check_Steering_Torque(bool bResetCount);
    int ReadyToOperation();
	int AutoInitialize();
	void ResetMovementData();



    //1	New Way
    bool IsFrontLeftOn();
    bool IsFrontRightOn();
    bool IsRearLeftOn();
    bool IsRearRightOn();
	bool IsFTorque0On();
	bool IsFTorque1On();
	bool IsRTorque0On();
	bool IsRTorque1On();
	bool IsFDirOn();
	bool IsRDirOn();

	HW_STEER_DIRECTION GetFrontPos();
	HW_STEER_DIRECTION GetRearPos();

	HW_STEER_DIRECTION GetFrontCmd();
	HW_STEER_DIRECTION GetRearCmd();
	HW_STEER_TORQUE GetFrontTorque();
	HW_STEER_TORQUE GetRearTorque();

	int SetFrontDirection(HW_STEER_DIRECTION_CMD Direction);
	int SetRearDirection(HW_STEER_DIRECTION_CMD Direction);
	int SetFrontTorque(HW_STEER_TORQUE Torque);
	int SetRearTorque(HW_STEER_TORQUE Torque);

	//신규 진단데이터 : 데이터 리턴 후 초기화
	int GetMovingCount(bool bInit);
	void IncreaseMovingCount();
	void ResetMovingCount();
    
};
#endif  // SteerH

