//---------------------------------------------------------------------------


#pragma hdrstop

#include "ProcessTerminator.h"

//---------------------------------------------------------------------------
ProcessTerminator::ProcessTerminator()
{

}

ProcessTerminator& ProcessTerminator::getInstance()
{
	if(instance == NULL)
	{
		instance = new ProcessTerminator;
	}

	return *instance;
}

/**
@brief		Process의 PID를 알아냄
@author  	doori.shin
@date    	2013.12.31
@param		procName : 얻고자 하는 프로세스 이름
@param		option : FP_FIRST_PID - 같은 이름의 프로세스가 있다면 제일 처음에 찾는것의 PID를 리턴(Basic)
					 FP_LAST_PID - 같은 이름의 프로세스가 있다면 제일 마지막에 찾는것의 PID를 리턴
@return		찾은 프로세스의 PID를 리턴, 없거나 에러이면 -1리턴
*/
DWORD ProcessTerminator::FindPID(WCHAR* procName, int option)
{
	//프로세스목록 가져오기
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD pid=0;

	PROCESSENTRY32 pe32 = {0,};
	pe32.dwSize = sizeof(PROCESSENTRY32);

	//첫 프로세스가 없다면 에러
	if(!Process32First(hProcessSnap, &pe32))
	{
		printf("Process32First Error!\n");
		CloseHandle(hProcessSnap);
		return -1;
	}

	//해당 이름의 process검색, PID얻기
	do{
		//printf("%25LS\t\t%d\n", pe32.szExeFile, pe32.th32ProcessID);
		//프로세스 이름 비교
		if(!wcscmp(pe32.szExeFile, procName))
		{
			pid = pe32.th32ProcessID;

			if(option == FP_FIRST_PID) break;
		}
	}while(Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return pid;
}

 /**
@brief		프로세스를 종료시킴
@author  	doori.shin
@date    	2013.12.31
@param		dwPID : 종료시킬 프로세스의 ID
@param		dwTimeout : 프로세스가 종료될 때까지 기다려줄 시간
@return		dwRet : 종료상태를 리턴
			TA_FAILED - 종료에 실패하거나 해당 프로세스가 없음
			TA_SUCCESS_CLEAN - 프로세스가 Timeout시간내에 종료됨
			TA_SUCCESS_KILL - Timeout되어 강제로 종료함
*/
DWORD WINAPI ProcessTerminator::TerminateApp( DWORD dwPID, DWORD dwTimeout)
{
    HANDLE   hProc ;
    DWORD   dwRet ;

    hProc = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE,
        dwPID);

    if(hProc == NULL)
    {
		return TA_FAILED ;
    }

    //PID에 맞는 프로세스에 WM_CLOSE메시지를 보낸다
    EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM) dwPID) ;

    //종료하려는 프로세스가 timeout 시간 내에 종료되었다면 CLEAN, 아니면 강제종료시킨다.
    if(WaitForSingleObject(hProc, dwTimeout)!=WAIT_OBJECT_0)
		dwRet=(TerminateProcess(hProc,0)?TA_SUCCESS_KILL:TA_FAILED);
    else
		dwRet = TA_SUCCESS_CLEAN ;

    CloseHandle(hProc) ;

    return dwRet ;
}

/**
@brief		프로세스에게 메시지를 보냄
@author  	doori.shin
@date    	2013.12.31
*/
BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM lParam )
{
    DWORD dwID ;

    GetWindowThreadProcessId(hwnd, &dwID) ;

    if(dwID == (DWORD)lParam)
    {
		PostMessage(hwnd, WM_CLOSE, 0, 0) ;
	}

	return TRUE ;
}

#pragma package(smart_init)
