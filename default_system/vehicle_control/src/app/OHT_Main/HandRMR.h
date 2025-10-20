//---------------------------------------------------------------------------

#ifndef HandRMRH
#define HandRMRH
//---------------------------------------------------------------------------
#include <System.hpp>
#include "HWCommon.h"
#include "Utility.h"
#include "ErrorCode.h"
#include "Hoist.h"
#include "Shutter.h"

/**
@class   HandRM
@date    2022.08.22
@author  mgchoi
@brief   P4 MaskOHT용 HandRMR 클래스 정의
*/
class HandRMR
{
private:
	HWCommon *m_pHWCommon;

	bool bLongRun;
	int m_nMovingCount;
	DWORD dwMaxMovingTime;
	unsigned int nCount_HandReset;
	bool bReadyOperation;
	bool bHomingState;
	int nSendCommand;
	DWORD dwStartTime;
	DWORD dwOriginLimitTime;
	DWORD dwMotionLimitTime;

	CRITICAL_SECTION m_MovCntCS;
	Shutter *m_pShutterC;
	Hoist *m_pHoistC;

public:
	HandRMR(HWCommon *pHWCommon);
	~HandRMR();

	int CheckComplete();

	void Init();
    int IO_Init();
	int ManualWork(int withIL);
	int AutoWork(int withIL);
	int ManualHome(int withIL);
	int AutoHome(int withIL);
	int Origin();
	int AlarmReset();
	int Enable();
	int Free();
	bool IsAlarm();
	bool IsFree();
	bool IsReady();
	bool IsWork();
	bool IsHome();
	bool IsInposition();
    // 2016-11-14, Add by Labeler
    // Flange Check 위해 함수 추가
    bool IsFlangeCheck();

	bool IsGoSigOn();
	bool IsLeftGripOn();
	bool IsRightGripOn();
	bool IsCloseOutputOn();
	bool IsHomingState();
	bool IsLongRun();
	bool IsHandPowerOn();
	bool IsLoadPosition();
	bool IsUnloadPosition();
	bool IsWorkOutputOn();
	void StartLongRun();
	void StopLongRun();
	int GetMovingCount(bool bInit);
	DWORD GetMaxMovingTime();
	unsigned int GetHandResetCount();
	int HandPowerControl(int nMode);
	int ReadyToOperation();
	int AutoInitialize();
	void ResetMovementData();
	int IsHandRMRMoveAvailableState(int);

	// for MASK OHT
	bool IsHomeOutputOn();

	// mgchoi. For Build...Delete or Modify Later
	bool IsOpen(int nLeftRight = -1);
	bool IsClose(int nLeftRight = -1);
	bool IsOpenOutputOn();
	bool IsPushOn();
};

#endif

