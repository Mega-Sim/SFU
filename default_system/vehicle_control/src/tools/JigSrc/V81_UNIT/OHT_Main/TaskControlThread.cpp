/**
@file    TaskControlThread.cpp 
@version OHT 7.0
@brief   TaskControl 구동 Thread
*/

#pragma hdrstop

#include "TaskControlThread.h"
#include "OHTMain.h"
#include "LogHandler.h"	// Log 작성 목적

// ---------------------------------------------------------------------------

TaskControlThread::TaskControlThread() : ThreadCtrl()
{
    m_pControlTask = NULL;
	m_pControlTask = OHTMainForm->GetTaskControl();
}
//--------------------------------------------------------------------------

TaskControlThread::~TaskControlThread()
{
	m_pControlTask =NULL;
}
//--------------------------------------------------------------------------

void TaskControlThread::RealThreadProc(void)
{
	static DWORD dwOCSTrigerTime = timeGetTime();     //함수 진입 시간.

    // 20161216 sh17.jo : Commfail 체크 기능 추가
    static DWORD dwCommfailTrigerTime = timeGetTime();

	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker==true)  break;  // Thread 종료를 위해 필수로 추가

		//2014.04.15 doori.shin Thread Check를 위한 Tick값 증가
		if(m_Tick == MAX_TICK)
			m_Tick = 0;
		else
			m_Tick++;

		if(m_pControlTask !=NULL)
		{
			// 1. OCS Data Update

			m_pControlTask->StandardOCSReadDataUpdate();

            // 2. Handy TP Update
            // 2016-12-19, Add by Labeler
            // 현재 Handy TP의 경우, TaskControl Class의 ExcuteStandardTaskUpdate 에서 처리하나, 해당 위치에서 처리토록 변경 필요
            m_pControlTask->GetReadRemoconData();

			// 3. TP Update
			m_pControlTask->StandardPCReadDataUpdate();

			// 4.1 AST Update
			m_pControlTask->StandardASTReadDataUpdate();

			// 5. Monitoring Update
			m_pControlTask->StandardMonitoringReadDataUpdate();

			//6.  Map Maker File Update
			m_pControlTask->StandardMapMakerFileUpdate();

			// 6. Task 업데이트
			m_pControlTask->ExcuteStandardTaskUpdate();

			// 6.1 시퀀스 스탭 수행
			m_pControlTask->ExecuteSteps();

//			m_pControlTask->ExecuteResetSteps();

			// 7. OCS에 Data 보내기 위한 부분 (100msec)
			m_pControlTask->SuperviseSendCmdDataOHT2OCS();

			// 8. OCS서버와 연결 체크
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwOCSTrigerTime) >Time_10sec)
			{
				m_pControlTask->StandardAddress_Broadcast();
				dwOCSTrigerTime = timeGetTime();
            }
			
            // 20161216 sh17.jo : Commfail 체크 기능 추가
			//9. 무선 연결 상태 체크
			if(m_pControlTask->m_bSetCheckCommfail == true)
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwCommfailTrigerTime) > Time_1sec)
				{
					m_pControlTask->CheckCommfail();
					dwCommfailTrigerTime = timeGetTime();
				}
			}
			else
			{
				m_pControlTask->m_bflagARPClear = false; //AOHC-234 C3850 Switch Fail-Over 시 통신 끊김 개선
			}

			//10. IoT 기능 update 부분
			m_pControlTask->IOTHubSocketClientUpdate();
			//AOHC-204 Thread 별 수행 시간 감시 기능
			static TimeChk timeChk;
			static ThreadTimeMonitor tmon;
			timeChk.End(0);
			tmon.Execute("TaskCtrl", timeChk.Get(0));
			timeChk.Start(0);
		}

		Sleep(10);
	}

}
//----------------------------------------------------------------------------




#pragma package(smart_init)



