/**
@file    LogThread.cpp
@version OHT 7.0
@brief   LogThread CPP File
*/

//---------------------------------------------------------------------------
#include "LogThread.h"
#include "LogHandler.h"
#include "OHTMain.h"

#define ADD_MEMO_T(STR)   OHTMainForm->memoMain->Lines->Add(_T(STR))
#define ADD_MEMO_S(STR)   OHTMainForm->memoMain->Lines->Add(STR)

// 로그 작성 함수
//---------------------------------------------------------------------------

/**
@brief   LogThread 생성자
@author  임태웅, 신두현
@date    2013.01.28 / 2013.07.11
@param   pTeachingDataManager : TeachingData 포인터구조체
@param   pStationDataManager : StationData 포인터 구조체
@param   pFunctionFlag : 실행하려는 함수들의 플래그 포인터 구조체
*/
LogThread::LogThread(): ThreadCtrl()
{
	InitializeCriticalSection(&m_CS);//2013.07.11 doori.shin CriticalSection 추가

}
//---------------------------------------------------------------------------

/**
@brief   LogThread Class 소멸자
@author  임태웅
@date    2013.01.28
*/
LogThread::~LogThread()
{
    DeleteCriticalSection(&m_CS);
}
//---------------------------------------------------------------------------

/**
@brief   RealThreadProc Thread 함수(IO 업데이트, 축 정보 업데이트, HID Data 업데이트)
@author  임태웅
@date    2013.01.28
*/
void LogThread::RealThreadProc()
{
	//---- Place thread code here ----
	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker)
			break;  // Thread 종료를 위해 필수로 추가
		
		//2014.04.15 doori.shin Thread Check를 위한 Tick값 증가
		if(m_Tick == MAX_TICK)
			m_Tick = 0;
		else
			m_Tick++;

		// Manager Log
		manageLog();
		Sleep(1000);
		//AOHC-204 Thread 별 수행 시간 감시 기능
		static TimeChk timeChk;

		timeChk.End(0);
		static ThreadTimeMonitor tmon;
		tmon.Execute("LogT", timeChk.Get(0));

		timeChk.Start(0);
	}

}

