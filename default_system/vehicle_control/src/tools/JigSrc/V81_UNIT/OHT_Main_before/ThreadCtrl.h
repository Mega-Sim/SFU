#ifndef ThreadCtrlH
#define ThreadCtrlH

//#include <iostream>
//#include <tchar.h>
#include <windows.h>
#include <process.h>

/*
THREAD_PRIORITY_TIME_CRITICAL 
THREAD_PRIORITY_HIGHEST 
THREAD_PRIORITY_ABOVE_NORMAL 
THREAD_PRIORITY_NORMAL 
THREAD_PRIORITY_BELOW_NORMAL 
THREAD_PRIORITY_LOWEST 
THREAD_PRIORITY_IDLE 
*/
 

#pragma comment(lib, "Kernel32.lib")

//using namespace std;
//// static 함수에 대한 Critical section
//static int nThreadCtrlCount = 0;
//static CRITICAL_SECTION	ThreadCtrl_CS;

class ThreadCtrl
{
private:
	CRITICAL_SECTION	m_CS;
	HANDLE	m_hThread;
	bool	m_bOnThreading; ///< RealThreadProc 가 호출 종료 되었을 때 자동으로 true 호출, 완료 되었을 때 false로 변경 한다.

	static UINT WINAPI ThreadProc(LPVOID lpParam);
	void ThreadProcCtrl(void);
	void endThread(void);

protected:
//	virtual UINT WINAPI RealThreadProc(void)=0;
	bool	m_bThreadBreaker;	///< Thread를 브레이크 시킬 때 사용
	virtual void RealThreadProc(void)=0;
	
	//2014.04.15 doori.shin Thread check를 위한 변수
	int m_Tick;
	static const int MAX_TICK = 1023;
public:
	ThreadCtrl(void);
	virtual ~ThreadCtrl(void);
	
	void beginThread(void);
//	void suspendThread(void);	// Thread간 간섭으로 인태 불량 발생 가능성 있어 제외함
//	void resumeThread(void);	// Thread간 간섭으로 인태 불량 발생 가능성 있어 제외함
	void setPriority(int nPriority);
	void getPriority();
	bool isOnThreading();

	void qiutRealThreadProc();	/// m_bThreadBreaker 를 true로 만들어 주는 함수
	HANDLE GetTreadHandle();
	//2014.04.15 doori.shin Thread check를 위한 함수
	int getTick();
};

#endif
