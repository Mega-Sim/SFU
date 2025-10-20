/**
@file    Hand.h
@version OHT 7.0
@brief   Hand Class Header File
*/

#ifndef HandH
#define HandH

#include <System.hpp>
#include "HWCommon.h"
#include "Utility.h"
#include "ErrorCode.h"
#include "LogHandler.h"
#include "ExecuteInfo.h"
#include "OHTMainStatus.h"

/**
@class   Hand
@date    2012.10.30
@author  LSJ
@brief   Hand ฐüทร Class
*/
class Hand
{
private:
	HWCommon *m_pHWCommon;

	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;

	bool bLongRun;
	int m_nMovingCount;
	int nMovingCount;
	DWORD dwMaxMovingTime;
	unsigned int nCount_HandReset;
	bool bReadyOperation;
	bool bHomingState;
	int nSendCommand;
	DWORD dwStartTime;
    DWORD dwOriginLimitTime;
	DWORD dwMotionLimitTime;

	CRITICAL_SECTION m_MovCntCS;

    LogUnit *pLogUnit;
	FDCUnit *pFDCUnit;

public:
	Hand(HWCommon *pHWCommon);
	~Hand();

	int CheckComplete();

	void Init();
    int IO_Init();
	int ManualOpen();
	int AutoOpen();
	int ManualClose();
	int AutoClose();
	int Origin();
	int AlarmReset();
	int Enable();
	int Free();
	bool IsOpen(int nLeftRight = -1);
	bool IsClose(int nLeftRight = -1);
	bool IsAlarm();
	bool IsFree();
	bool IsReady();
	bool IsInposition();
	bool IsGoSigOn();
	bool IsLeftGripOn();
	bool IsRightGripOn();
	bool IsPushOn(int nLeftRight = -1);
	bool IsCloseOutputOn();
	bool IsOpenOutputOn();
	bool IsHomingState();
	bool IsLongRun();
	bool IsHandPowerOn();
	void StartLongRun();
	void StopLongRun();
	int GetMovingCount(bool bInit);
	DWORD GetMaxMovingTime();
	unsigned int GetHandResetCount();
    int HandPowerControl(int nMode);
    int ReadyToOperation();
	int AutoInitialize();
	void ResetMovementData();

};

#endif  // HandH
