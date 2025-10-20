//#include "stdafx.h"

#include "ThreadCtrl.h"

ThreadCtrl::ThreadCtrl(void)
{
	InitializeCriticalSection(&m_CS);
	m_hThread = NULL;
	m_bOnThreading	= false;
	m_bThreadBreaker	= false;
	m_Tick = 0;
}

ThreadCtrl::~ThreadCtrl(void)
{
	endThread();
	DeleteCriticalSection(&m_CS);
}
	
void ThreadCtrl::beginThread(void)
{
	EnterCriticalSection(&m_CS);
	if(m_hThread==NULL)
	{
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (LPVOID)this, 0, NULL);
	}
	LeaveCriticalSection(&m_CS);
}
/*
HANDLE ThreadCtrl::GetTreadHandle()
{
	return m_hThread;
}
*/
int ThreadCtrl::getTick()
{
    return m_Tick;
}
/*	
//void ThreadCtrl::suspendThread(void)
//{
//	EnterCriticalSection(&m_CS);
//
//	if(m_hThread!=NULL)
//	{
//		SuspendThread(m_hThread);	
//	}
//
//	LeaveCriticalSection(&m_CS);
//}
*/
/*
//void ThreadCtrl::resumeThread(void)
//{
//	EnterCriticalSection(&m_CS);
//
//	if(m_hThread!=NULL)
//	{
//		ResumeThread(m_hThread);
//	}
//
//	LeaveCriticalSection(&m_CS);
//}
*/
void ThreadCtrl::setPriority(int nPriority)
{
	EnterCriticalSection(&m_CS);

	if(m_hThread!=NULL)
	{
		SetThreadPriority(m_hThread, nPriority);
	}

	LeaveCriticalSection(&m_CS);
}
/*
void ThreadCtrl::getPriority()
{
	EnterCriticalSection(&m_CS);

	if(m_hThread!=NULL)
	{
		GetThreadPriority(m_hThread);
	}

	LeaveCriticalSection(&m_CS);
}
*/
UINT WINAPI ThreadCtrl::ThreadProc(LPVOID lpParam)
{
	ThreadCtrl* pThreadCtrl = (ThreadCtrl*)lpParam;
	pThreadCtrl->ThreadProcCtrl();
	return 0;
}

void ThreadCtrl::ThreadProcCtrl(void)
{
	m_bOnThreading = true;
	RealThreadProc();
	endThread();
	m_bOnThreading = false;
	m_bThreadBreaker = false;
}

void ThreadCtrl::endThread(void)
{
	m_bThreadBreaker	= true;

	EnterCriticalSection(&m_CS);
	if(m_hThread!=NULL)
	{
		CloseHandle(m_hThread);
		m_hThread=NULL;
	}
	LeaveCriticalSection(&m_CS);
}

bool ThreadCtrl::isOnThreading()
{
	return m_bOnThreading;
}

void ThreadCtrl::qiutRealThreadProc()
{
	m_bThreadBreaker = true;
}

