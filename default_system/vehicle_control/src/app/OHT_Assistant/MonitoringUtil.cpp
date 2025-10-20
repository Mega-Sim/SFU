//---------------------------------------------------------------------------


#pragma hdrstop
#include "ProcessTerminator.h"
#include "MonitoringUtil.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib");
#define ADD_Monitoring_LOG(STR)        m_Log->WriteLogFile("Monitoring", STR)
using namespace std;
//---------------------------------------------------------------------------
MonitoringUtil::MonitoringUtil()
{
	Init();
}

MonitoringUtil& MonitoringUtil::getInstance()
{
	if(instance == NULL)
	{
		instance = new MonitoringUtil;
	}

	return *instance;
}

void MonitoringUtil::Init()
{
	strResult = "";
	status = 0;
}

int MonitoringUtil::GetStatus()
{
	return status;
}

/**
@brief   FBWF로 명령을 보낸 결과를 저장한 문자열을 받음
@author  doori.shin
@date    2013.12.31
@return	 strResult : FBWF명령을 보낸 결과를 갖고있음
*/
String MonitoringUtil::GetResultString()
{
    return strResult;
}

/**
@brief   	도스 커맨드로 명령어를 보내어 실행하게 하는 함수
			이 프로그램에서는 FBWF에 맞추어 설계되었다
@author  	doori.shin
@date    	2013.12.31
@param		strDosCmd : 도스 명령어
@return	 	strResult : FBWF명령을 보낸 결과를 갖고있음
*/
int MonitoringUtil::ExecuteDosCmd(String strDosCmd)
{
	bool                    bflag;
	HANDLE                  hIwrite, hIread;
	HANDLE                  hOwrite, hOread;
	HANDLE                  hEwrite, hEread;
	SECURITY_ATTRIBUTES     sa;
	STARTUPINFO             siParam;
	PROCESS_INFORMATION     piParam;
	DWORD                   dwExitCode;
	int error = FBWF_ERROR;
	Init();

	DWORD BytesRead;
	AnsiString strData;

	// Pipe Parameter Set
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = true;

	// 어노니머스 파이프 생성  // Input File
	bflag = CreatePipe(&hIread, &hIwrite, &sa, 0);
	if (!bflag)
	{
//		error = ERR_PIPE_OPEN_ERROR;
		CloseHandle(hIread);
		CloseHandle(hIwrite);

		return error;
	}

	// 어노니머스 파이프 생성  // output File
	bflag = CreatePipe(&hOread, &hOwrite, &sa, 0);
	if (!bflag)
	{
//		error = ERR_PIPE_OPEN_ERROR;
		CloseHandle(hIread);
		CloseHandle(hIwrite);
		CloseHandle(hOread);
		CloseHandle(hOwrite);

		return error;
	}

	// 어노니머스 파이프 생성 // Error File
	bflag = CreatePipe(&hEread, &hEwrite, &sa, 0);
	if (!bflag)
	{
//		error = ERR_PIPE_OPEN_ERROR;
		CloseHandle(hIread);
		CloseHandle(hIwrite);
		CloseHandle(hOread);
		CloseHandle(hOwrite);
		CloseHandle(hEread);
		CloseHandle(hEwrite);

		return error;
	}

	// 초기화
	FillMemory(&siParam, sizeof(STARTUPINFO), 0);
	FillMemory(&piParam, sizeof(PROCESS_INFORMATION), 0);

	// Param 섯팅
	siParam.cb          = sizeof(STARTUPINFO);
	siParam.dwFlags     = STARTF_USESTDHANDLES;
	siParam.wShowWindow = SW_HIDE;
	siParam.hStdInput = hIread;   // 표준입력(stdin)
	siParam.hStdOutput = hOwrite;  // 표준출력(stdout) 리다이렉션
	siParam.hStdError = hEwrite;   // 표준에러(stderr) 리다이렉션

	hIread = CreateFile(L"input.d",            //표준입력용 input.d 파일 오픈
					   GENERIC_READ,
					   FILE_SHARE_READ,
					   &sa,
					   OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL);

	bflag = CreateProcess(NULL, strDosCmd.c_str(), NULL, NULL, true, DETACHED_PROCESS, NULL, NULL, &siParam, &piParam);
	WaitForInputIdle(GetCurrentProcess(), INFINITE);

	if(!bflag)
	{
		CloseHandle(hIread);
		CloseHandle(hIwrite);
		CloseHandle(hOread);
		CloseHandle(hOwrite);
		CloseHandle(hEread);
		CloseHandle(hEwrite);
//		error = ERR_CREATE_PROCESS;

		return error;
	}

	// 프로세스가 종료될때까지 대기함
	if(piParam.hProcess)
	{
	  dwExitCode = STILL_ACTIVE;

		while(dwExitCode == STILL_ACTIVE)
		{
			WaitForSingleObject(piParam.hProcess, INFINITE);
			GetExitCodeProcess(piParam.hProcess, &dwExitCode);
			CloseHandle(piParam.hProcess);
			CloseHandle(piParam.hThread);
//			UpdateWindow(Handle);
		}
	}

	CloseHandle(hIread);
	CloseHandle(hOwrite);
	CloseHandle(hEwrite);

	char *readBuffer = new char[512];

	while(ReadFile(hOread, readBuffer, sizeof(readBuffer), &BytesRead, NULL) && BytesRead)
	{
		readBuffer[BytesRead] = '\0';
		strResult += readBuffer;
	}

	if(strResult.Length() != 0)
	{
		strResult += "\0";
	}

	CloseHandle(hOread);
	CloseHandle(hEread);
	CloseHandle(hIwrite);

	delete[] readBuffer;

	if(strResult.Length() != 0)
	{
		int ePos = strResult.Pos("filter state: enabled");
		int dPos = strResult.Pos("filter state: disabled");

		if(ePos == 0)
			status = FBWF_DISABLE;
		else if(dPos == 0)
			status = FBWF_ENABLE;
		else if(ePos < dPos)
			status = FBWF_ENABLE;
		else if(dPos < ePos)
			status = FBWF_DISABLE;

		return status;
	}
	else
	{
		return error;
	}
}

bool MonitoringUtil::isFileExist(String strFilePath)
{
	if(FindFirst(strFilePath, faAnyFile-faDirectory, sr))
		return false;
	else
		return true;

}

/**
@brief   	디스크 정보를 받아옴
@author  	doori.shin
@date    	2013.12.31
@param		drive : 알아낼 드라이브 번호(ex. C:\ == 3, D:\ == 4,...)
@return	 	diskInfo : 드라이브정보를 담고있는 구조체
*/
DISK_INFO* MonitoringUtil::GetDiskInfo(int drive)
{
	DISK_INFO* diskInfo = new DISK_INFO();
	memset(diskInfo, 0, sizeof(DISK_INFO));

	diskInfo->Drive = drive + 0x40;
	diskInfo->TotalSpace = DiskSize(drive);

	//디스크 크기가 -1 일경우 드라이브 에러
	if(diskInfo->TotalSpace == -1)
		return diskInfo;

	diskInfo->FreeSpace = DiskFree(drive);
	diskInfo->UsedSpace = diskInfo->TotalSpace - diskInfo->FreeSpace;

	GetCurrentTime();

	return diskInfo;
}

///**
//@brief   	바이트로된 디스크 크기를 형식에 맞추어 변환
//@author  	doori.shin
//@date    	2013.12.31
//@param		size : 변환할 디스크 크기
//@return	 	변환된 디스크 크기(TCHAR*)
//*/
//void MonitoringUtil::ConvertDiskFormattedSize(UINT64 size, TCHAR* formattedSize)
//{
//	formattedSize = StrFormatByteSizeW(size, formattedSize, 32);
////	TCHAR* formattedSize = new TCHAR[10];
////	return formattedSize = StrFormatByteSizeW(size, formattedSize, 32);
//}

/**
@brief   	현재시간을 설정함(TP로부터 받은시간)
@author  	doori.shin
@date    	2013.12.31
@param		strTime : TP로부터 받은 시간
*/
void MonitoringUtil::SetCurrentTime(String strTime)
{
	int nY, nMon, nD, nH, nM, nS;

	nY = StrToIntDef(strTime.SubString(1, 4), 0);
	nMon = StrToIntDef(strTime.SubString(5, 2), 0);
	nD = StrToIntDef(strTime.SubString(7, 2), 0);
	nH = StrToIntDef(strTime.SubString(9, 2), 0);
	nM = StrToIntDef(strTime.SubString(11, 2), 0);
	nS = StrToIntDef(strTime.SubString(13, 2), 0);

	// 날짜 설정
	struct date stDate;
	stDate.da_year = nY;
	stDate.da_mon  = nMon;
	stDate.da_day  = nD;
	setdate(&stDate);

	// 시간 설정
	struct time stTime;
	stTime.ti_hour  = nH;
	stTime.ti_min   = nM;
	stTime.ti_sec   = nS;
	stTime.ti_hund  = 0;
	settime(&stTime);
}

/**
@brief   	OHT 메인프로그램의 버전을 얻음
@author  	doori.shin
@date    	2013.12.31
@return	 	version : OHT버전
@remark		OHT프로그램 존재여부, Major, minor, build 버전을 얻음
*/
OHT_VERSION* MonitoringUtil::GetOHTMainProgramVersion()
{
	OHT_VERSION* version = new OHT_VERSION();
	String strOHTPath = L"./OHT.exe";
	char *cVersion = NULL;
	DWORD infoSize = 0;

	infoSize = GetFileVersionInfoSize(strOHTPath.c_str(), 0);
	if(infoSize == 0)
		return version;

	cVersion = new char[infoSize];

	if(GetFileVersionInfo(strOHTPath.c_str(), 0, infoSize, cVersion) != 0)
	{
		VS_FIXEDFILEINFO* pFineInfo = NULL;
		UINT bufLen = 0;

		if(VerQueryValue(cVersion, L"\\", (LPVOID*)&pFineInfo, &bufLen) != 0)
		{
            version->Version.IsExists = 0x01;
			version->Version.Major = HIWORD(pFineInfo->dwFileVersionMS);
			version->Version.Minor = LOWORD(pFineInfo->dwFileVersionMS);
			version->Version.Build = HIWORD(pFineInfo->dwFileVersionLS);
        }
	}

	delete[] cVersion;

	return version;
}
/**
@brief   	OHT 메인프로그램의 실행여부 Monitoring
@author  	Jrespect
@date    	2018.03.10
@return	 	실행중이지 않으면 LOG 작성
@purpose    OHT Main 종료로 인한 통신불가 or CP Down에 의한 통신불가 판단
*/
void MonitoringUtil::MonitoringOHT()
{
  String strLog;  // Log를 위한 변수
  DWORD pid = ProcessTerminator::getInstance().FindPID(OHT_MAIN_FILE_NAME);
  if (pid == NULL) {
		strLog.sprintf(L"OHT Close, Warning Communication fail");
		ADD_Monitoring_LOG(strLog);
  }
  else{
		strLog.sprintf(L"OHT Running");
		ADD_Monitoring_LOG(strLog);
	 }
}








#pragma package(smart_init)
