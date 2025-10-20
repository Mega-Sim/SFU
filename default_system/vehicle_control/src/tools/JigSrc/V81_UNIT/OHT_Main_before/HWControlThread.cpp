/**
@file    HWControlThread.cpp
@version OHT 7.0
@brief   HWControlThread CPP File
*/

//---------------------------------------------------------------------------
#include <windows.h>

#include "OHTMain.h"
#include "HWControlThread.h"


//---------------------------------------------------------------------------

/**
@brief   HWControlThread 생성자
@author  LSJ
@date    2012.
@param   pHWCommon : HWCommon 클래스 포인터
@param   pHWSet : HWSet 클래스 포인터
*/
HWControlThread::HWControlThread(): ThreadCtrl()
{
	m_pHWCommon = OHTMainForm->GetHWCommon();
	m_pHWSet = OHTMainForm->GetHWSet();
}

/**
@brief   HWControlThread Class 소멸자
@author  LSJ
@date    2012.11.04
*/
HWControlThread::~HWControlThread()
{

}

/**
@brief   RealThreadProc Thread 함수(IO 업데이트, 축 정보 업데이트, HID Data 업데이트)
@author  LSJ
@date    2012.11.09
*/
void HWControlThread::RealThreadProc()
{
	//---- Place thread code here ----

	SYSTEM_INFO systemInfo;
	bool bCheckCore = false;
	DWORD nProA,nSysA = 0;

 /*
	//Core Affinity Set 1- Core1, 2-Core2, 3-Core1+core2// 2018 01 25
	GetSystemInfo(&systemInfo);
	if(systemInfo.dwNumberOfProcessors > 1)
	{
//		HANDLE hProc = GetCurrentProcess();
//		bCheckCore = SetProcessAffinityMask(hProc,2);
//		GetProcessAffinityMask(hProc,&nProA,&nSysA);
		SetThreadAffinityMask((HANDLE)GetTreadHandle(), 2);
//		ADD_LOG("SetThreadAffinityMask : 2");
	}
    */
	///////////////////////////////////////////////////////

	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker==true)	
            break;  // Thread 종료를 위해 필수로 추가
			
		//2014.04.15 doori.shin Thread Check를 위한 Tick값 증가
		if(m_Tick == MAX_TICK)
			m_Tick = 0;
		else
			m_Tick++;
    
		if(m_pHWCommon!=NULL)
		{
			m_pHWCommon->UpdateData();
		}

		Sleep(5);
	}
}
//---------------------------------------------------------------------------
