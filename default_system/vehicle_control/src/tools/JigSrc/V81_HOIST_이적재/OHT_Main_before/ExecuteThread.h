//---------------------------------------------------------------------------
/**
@file    ExecuteThread.h
@version OHT 7.0
@brief   ExecuteThread Class Header File
*///---------------------------------------------------------------------------

#ifndef ExecuteThreadH
#define ExecuteThreadH
//---------------------------------------------------------------------------
#include <stdio.h>
#include "ThreadCtrl.h"
#include "ExecuteInfo.h"
#include "VirtualModuleControl.h"
#include "DrivingControl.h"
#include "TransControl.h"
#include "ManualControl.h"
#include "HWCommon.h"
#include "LogHandler.h"	// Log 작성 목적
//---------------------------------------------------------------------------

/**
@class   ExecuteThread
@date    2012.10.22
@author  ehoto
@brief   ExecuteThread 동작 관련 통제하는 Class
@note    ExecuteThread 동작 관련 통제를 위한 함수 구현
*/
class ExecuteThread:public ThreadCtrl
{
private:
	LogUnit *pLogExecuteThread;   ///< ExecuteThread 로그 생성을 위한 변수

protected:
	void RealThreadProc(void);	///< Execute 와 같은 함수
public:
	ExecuteThread();        ///< 생성자 : Tool Class 및 Data 공유 구조체
	~ExecuteThread(void);   ///< 소멸자 :

private:
	EXECUTE_INFO *m_pExecuteInfo;            ///< MainThread와 정보를 공유하는 구조체
	VirtualModuleControl *m_pModuleControl[3];    ///< Driving:1 / Trans:2 / Manual:3

    BYTE m_Status[3];
	UINT m_ModuleError[3];

	HWCommon	*m_pHWCommon;
    
private:
	UINT m_nError;
	bool m_bReady;
    
    private:
	UINT updateStatus(void);

	UINT handleQuickCommand(EXECUTE_QUICK_CMD CmdType);
	UINT stopCmd(void);

    UINT handleModuleControl(int nIndex, EXECUTE_INFO *pExecuteInfo, UINT nMaxCountGetCmd);
    UINT checkModules(int nIndex, EXECUTE_INFO * pExecuteInfo);
    UINT deleteCommand(int nIndex, EXECUTE_INFO *pExecuteInfo);

    UINT transferDataToMainThread(EXECUTE_INFO *pExecuteInfo);

    bool isOnAutoRecovery(int nIndex, int nError);

};
//---------------------------------------------------------------------------
#endif
