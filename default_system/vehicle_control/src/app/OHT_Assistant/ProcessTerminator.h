//---------------------------------------------------------------------------

#ifndef ProcessTerminatorH
#define ProcessTerminatorH
//---------------------------------------------------------------------------
#pragma once

#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <VdmDbg.h>

#define TA_FAILED 0
#define TA_SUCCESS_CLEAN 1
#define TA_SUCCESS_KILL 2

#define	FP_FIRST_PID	1
#define	FP_LAST_PID		2

#define	PROC_NAME	L"OHT.exe"
#define	FM_NAME	L"FM.exe"


class ProcessTerminator
{
private:
	ProcessTerminator();
	static ProcessTerminator* instance;
public:
	static ProcessTerminator& getInstance();

	DWORD WINAPI TerminateApp( DWORD dwPID, DWORD dwTimeout, bool bForceClose = true);
	DWORD FindPID(WCHAR* procName, int option = FP_FIRST_PID);
};

ProcessTerminator* ProcessTerminator::instance = NULL;
BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM lParam );

#endif
