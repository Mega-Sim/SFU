/**
@file    Shutter.h
@version OHT 7.0
@brief   Shutter Class Header File
*/

#ifndef ShutterH
#define ShutterH

#include <System.hpp>
#include "HWCommon.h"
#include "Utility.h"

#include "LogHandler.h"
#include "OHTMainStatus.h"
#include "ExecuteInfo.h"
/**
@class   Shutter
@date    2012.11.01
@author  LSJ
@brief   Shutter 관련 Class
*/
class Shutter
{
private:
    HWCommon *m_pHWCommon;

	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;

    bool bLongRun;
    int m_nMovingCount;
    DWORD dwMaxMovingTime;
    bool bReadyOperation;
    int nSendCommand;
    DWORD dwStartTime;
    DWORD dwOriginLimitTime;
	DWORD dwMotionLimitTime;

	CRITICAL_SECTION m_MovCntCS;

	LogUnit *pLogShutter;
	FDCUnit *pFDCUnit;

public:
    Shutter(HWCommon *pHWCommon);
    ~Shutter();

    int CheckComplete();

    void Init();
    int IO_Init();
    int Open();
	int Close();
	int Mid_Close();
	
	int Open_Cmd();
	int Close_Cmd();
    int SendOpenCmd();
    int SendCloseCmd();
    int Origin();
    int AlarmReset();
    int Enable();
    int Free();
    bool IsOpen(int nFrontRear);
    bool IsClose(int nFrontRear);
    bool IsCmdOpen(int nFrontRear);
    bool IsCmdClose(int nFrontRear);
    bool IsInposition(int nFrontRear);
    bool IsAlarm(int nFrontRear);
    bool IsFree(int nFrontRear);
    int CheckSensor(int nShutterPos);
    bool IsLongRun();
    void StartLongRun();
    void StopLongRun();
    int GetMovingCount(bool bInit);
    DWORD GetMaxMovingTime();
    int ReadyToOperation();
    /// @todo 상위 단에서 해당 내용의 함수 구현 필요
	int AutoInitialize();

	void ResetMovementData();

	int CheckEUVSensor();
public:
	bool bHoistJig ;
};

#endif  // ShutterH
