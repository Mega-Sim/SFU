// MapExtract.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "MapExtract.h"
#include <iostream>
#include <fstream>
#include "MapFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 유일한 응용 프로그램 개체입니다.

CWinApp theApp;

using namespace std;

CString GetWorkDir()
{
	static CString strWorkingDirectory;

	if (strWorkingDirectory == "")
	{
		TCHAR szModuleFileName[_MAX_PATH];
		::GetModuleFileName(::AfxGetInstanceHandle(), szModuleFileName, _MAX_PATH);

		TCHAR szDrive[_MAX_DRIVE], szPath[_MAX_PATH];
		//_splitpath(szModuleFileName, szDrive, szPath, NULL, NULL);
		_splitpath_s(szModuleFileName, szDrive, _MAX_DRIVE, szPath, _MAX_PATH, NULL, 0, NULL, 0);

		strWorkingDirectory.Format("%s%s", szDrive, szPath);
	}

	return strWorkingDirectory;
}

int _FileAge(CString szFileName)
{
	HANDLE  Handle;
	WIN32_FIND_DATA FindData;
	FILETIME LocalFileTime;
	WORD wTime = 0, wDate = 0;


	Handle = FindFirstFile(LPCTSTR(szFileName), &FindData);
	if (Handle != INVALID_HANDLE_VALUE)
	{
		FindClose(Handle);
		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			FileTimeToLocalFileTime(&FindData.ftLastWriteTime, &LocalFileTime);
			if (FileTimeToDosDateTime(&LocalFileTime, &wDate, &wTime))
				return MAKELONG(wTime, wDate);
		}
	}

	return -1;
}

BOOL FileExists(CString szFileName)
{
	return (_FileAge(szFileName) != -1) ? TRUE : FALSE;
}

CString gStringFormat(LPCTSTR fmtstr, ...)
{
	TCHAR buffer[4096];
	ZeroMemory(buffer, sizeof(buffer));
	try
	{
		va_list argptr;
		va_start(argptr, fmtstr);
		_vstprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), fmtstr, argptr);
		va_end(argptr);
	}
	catch (...)
	{
		//...
	}

	CString str;
	str.SetString(buffer);
	return str;
}

// 특정 프로세스 아규먼트 실행 지원
// ShellExecute 보다 좋은 점은, 실행시킬 프로세스의 종료시점까지 대기할 수 있어, 동기식 처리 가능.
// 단 파일 경로같은 정보가 commandLine에 들어있을 경우, 경로에 공백이 존재하면 동작이 안될 수 가능성도 있음
bool RunProcess_Infinite(CString strProcess, CString commandLine, bool bShowHide, int *pRetVal)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { 0 };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = bShowHide ? SW_SHOW : SW_HIDE;

	CString strCmd = strProcess + " "+ commandLine;
	if (CreateProcess(NULL, strCmd.GetBuffer(0), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == false)
		return false;
	
	// Wait until child process exits.
	while (true)
	{
		Sleep(1);

		if (WaitForSingleObject(pi.hProcess, 1) != WAIT_TIMEOUT)
			break;
	}

	DWORD exitCode;
	GetExitCodeProcess(pi.hProcess, &exitCode);
	if(pRetVal)
		*pRetVal = exitCode;

	return true;
}

void GetCommandLineArg(CStringArray* _out_pStrArr)
{
	LPWSTR* pStr = NULL;
	int iCnt = 0;
	pStr = CommandLineToArgvW(GetCommandLineW(), &iCnt);

	try
	{
		for (int i = 1; i<iCnt; i++)
		{
			_out_pStrArr->Add(CString(pStr[i]));
		}
	}
	catch (...) {}
}
// Files & Directory
CString _ExtractFilePath(CString szFilePath)
{
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char buf[_MAX_PATH];
	CString szVal;


	ASSERT(szFilePath);

	strcpy_s(path_buffer, szFilePath);

	_splitpath_s(path_buffer, drive, dir, fname, ext);

	memset(buf, '\0', sizeof(buf));
	strcat_s(buf, drive);
	strcat_s(buf, dir);

	szVal = CString(buf);

	return szVal;
}

// SHCHO 2018.05.29
// 프로그램 설명
// 이 실행파일이 있는 폴더에서 extract.dat 파일이 있고 파일 안에 맵파일 zip 경로가 들어있어야 한다.
// 그럼 이 프로그램은 맵파일 압축을 7zip을 이용해서 풀고, 바이너리인 MapData.dat 파일을 열어서 구조체에 맞게 파싱한다
// 파싱한 정보를 OcsLayout.dat 파일에 텍스트로 붙여 넣는다.
// 그럼 최종적으로 이 프로그램을 호출한 프로세스는 텍스트 파일인 OcsLayout.dat 파일만 로딩하면
// 모든 맵정보를 알 수 있다
int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 오류 코드를 필요에 따라 수정합니다.
            wprintf(L"심각한 오류: MFC를 초기화하지 못했습니다.\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: 응용 프로그램의 동작은 여기에서 코딩합니다.
			// 아규먼트에서 zip 경로 알아오기
			CStringArray args;
			GetCommandLineArg(&args);
			//for (int n = 0; n < args.GetCount(); n++)
			//	AfxMessageBox(args.GetAt(n));
			bool DEBUGGING = false;

			if (args.GetCount() < 1 && !DEBUGGING)
				return -31;

			
			CString zipPath;
			
			if(!DEBUGGING)
				zipPath = args.GetAt(0);		// gStringFormat("%s\\Map.zip", gStrWorkDir);
			else
				zipPath = "C:\\.oht_easy_view_temporary\\22\\aa.zip";

			if (FileExists(zipPath) == false)
				return -30;

			
			CString strWorkDir = _ExtractFilePath(zipPath);
			// 임시 디렉토리 생성
			CreateDirectory(strWorkDir, NULL);
			SetFileAttributes(strWorkDir, FILE_ATTRIBUTE_HIDDEN);

			CString strSubDir = "vehicle_map\\1";	// 맵 그룹 1번 우선

			// Folder Clearing
			CString strLayout_in					= gStringFormat("%sOcsLayout.dat", strWorkDir);
			CString strMapDataPath_in			= gStringFormat("%s%s\\MAPDATA.DAT", strWorkDir, strSubDir);
			CString strMapDataPath_out		= gStringFormat("%sMAPDATA.TXT", strWorkDir);
			CString strCIDPath_in					= gStringFormat("%s%s\\CID.DAT", strWorkDir, strSubDir);
			CString strCIDPath_out				= gStringFormat("%sCID.TXT", strWorkDir);
			CString strCollisionPath_in			= gStringFormat("%s%s\\COLLISION.DAT", strWorkDir, strSubDir);
			CString strCollisionPath_out			= gStringFormat("%sCOLLISION.TXT", strWorkDir);
			CString strStationPath_in				= gStringFormat("%s%s\\STATION.DAT", strWorkDir, strSubDir);
			CString strStationPath_out			= gStringFormat("%sSTATION.TXT", strWorkDir);
			CString strFlowControlPath_in = gStringFormat("%s%s\\FLOWCONTROL.DAT", strWorkDir, strSubDir);
			CString strFlowControlPath_out = gStringFormat("%sFLOWCONTROL.TXT", strWorkDir);
			CString strTeachingPath_in = gStringFormat("%s%s\\TEACHING.DAT", strWorkDir, strSubDir);
			CString strTeachingPath_out = gStringFormat("%sTEACHING.TXT", strWorkDir);
			CString strPathData_in = gStringFormat("%s%s\\PATHDATA.DAT", strWorkDir, strSubDir);
			CString strPathData_out = gStringFormat("%sPATHDATA.TXT", strWorkDir);
			
			CPtrArray pathArr;
			pathArr.Add(&strLayout_in);
			pathArr.Add(&strMapDataPath_in);
			pathArr.Add(&strMapDataPath_out);
			pathArr.Add(&strCIDPath_in);
			pathArr.Add(&strCIDPath_out);
			pathArr.Add(&strCollisionPath_in);
			pathArr.Add(&strCollisionPath_out);
			pathArr.Add(&strCollisionPath_in);
			pathArr.Add(&strStationPath_in);
			pathArr.Add(&strStationPath_out);
			pathArr.Add(&strFlowControlPath_in);
			pathArr.Add(&strFlowControlPath_out);
			pathArr.Add(&strTeachingPath_in);
			pathArr.Add(&strTeachingPath_out);
			pathArr.Add(&strPathData_in);
			pathArr.Add(&strPathData_out);


			for (int i = 0; i < pathArr.GetCount(); i++)
			{
				CString *pStr = (CString*)pathArr.GetAt(i);
				DeleteFile(*pStr);
			}
			
			// 7zip을 이용해서 OcsLayout과 Mapdata 두개 파일만 현재 워킹 폴더에 풀겠다는
			CString cmd;
			//cmd.Format("e %s -o%s OcsLayout.dat MAPDATA.DAT CID.DAT COLLISION.DAT STATION.DAT FLOWCONTROL.DAT TEACHING.DAT-r -y", zipPath, gStrWorkDir);
			//cmd.Format("e %s -o%s -r -y", zipPath, strWorkDir);	
			cmd.Format("x %s -o%s -y", zipPath, strWorkDir);	//cmd.Format("e %s -o%s OcsLayout.dat MAPDATA.DAT CID.DAT COLLISION.DAT STATION.DAT FLOWCONTROL.DAT TEACHING.DAT-r -y", zipPath, gStrWorkDir);

			if (RunProcess_Infinite("7z.exe", cmd, false, NULL) == false)
				return -40;
			
			// dat 파일 압축 안풀린거 있는지 확인
			for (int i = 0; i < pathArr.GetCount(); i++)
			{
				CString *pStr = (CString*)pathArr.GetAt(i);
				if (pStr->Find(".DAT") >= 0)
				{
					if (!FileExists(*pStr))
						return -50 - i;
				}
			}

			// MapData.dat 파일을 로딩한 후 // txt로 save 한다
			STD_MAP mapData;
			if (mapData.LoadMapFile_MapData(strMapDataPath_in) == false)				return -70;
			if (mapData.Save_asText(strMapDataPath_out) == false)				return -71;

			// CID.dat 파일을 로딩한다 , 후 txt save
			STD_CID cidData;
			if (cidData.LoadMapFile_CID(strCIDPath_in) == false)				return -71;
			if (cidData.Save_asTxt(strCIDPath_out) == false)				return -71;

			// collision.dat 파일을 로딩한다 , 후 txt save
			CollisionManager_STD collision;
			if (collision.LoadMapFile(strCollisionPath_in) == false)				return -72;
			if (collision.Save_asTxt(strCollisionPath_out) == false)				return -72;

			//// staion.dat 파일을 로딩한다 , 후 txt save
			StationManager_STD station;
			if (station.LoadMapFile(strStationPath_in) == false)				return -73;
			if (station.Save_asTxt(strStationPath_out) == false)				return -73;

			//// flowcontrol.dat 파일을 로딩한다 , 후 txt save
			STD_FlowControl flowControl;
			if (flowControl.LoadMapFile(strFlowControlPath_in) == false)				return -73;
			if (flowControl.Save_asTxt(strFlowControlPath_out) == false)				return -73;

			//// teaching.dat 파일을 로딩한다 , 후 txt save
			STD_Teaching teaching;
			if (teaching.LoadMapFile(strTeachingPath_in) == false)				return -74;
			if (teaching.Save_asTxt(strTeachingPath_out) == false)				return -74;

			STD_PathData pathData;
			if (pathData.LoadMapFile(strPathData_in) == false)				return -75;
			if (pathData.Save_asTxt(strPathData_out) == false)				return -75;

        }
    }
    else
    {
        // TODO: 오류 코드를 필요에 따라 수정합니다.
        wprintf(L"심각한 오류: GetModuleHandle 실패\n");
        nRetCode = 1;
    }

    return nRetCode;
}
