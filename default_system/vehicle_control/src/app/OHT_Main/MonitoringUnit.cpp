//---------------------------------------------------------------------------
/**
@file    MonitoringUnit.cpp
$Rev:: 602           $:  Revision of last commit
$Author:: puting     $:  Author of last commit
$Date:: 2013-07-28 0#$:  Date of last commit
@version Monitor Ver 1.0
@brief   MonitoringUnit Class CPP File
*/
#include "MonitoringUnit.h"
#include "PUtily.h"

#include "OHTMain.h"

//---------------------------------------------------------------------------
/**
@brief   MONITORING 생성자
@author  puting
@date    2013.07.22
*/
MONITORING::MONITORING()
{
   gm_Monitoringinfo = new MONITOR_DATA_INFO;
   memset(gm_Monitoringinfo, 0x00, sizeof(MONITOR_DATA_INFO));
   loadParameter();
   m_bCheckWinSetup = false;
}
//---------------------------------------------------------------------------
/**
@brief   MONITORING 소멸자
@author  puting
@date    2013.07.22
*/
MONITORING::~MONITORING()
{
	SaveParameter();
 	delete gm_Monitoringinfo;
}

//---------------------------------------------------------------------------
/**
@brief   주기적으로 업데이트 하는 항목은 이곳에서 구현된다
@author  puting
@date    2013.07.22
*/
void MONITORING::MonitorUpdate(MONITOR_DATA_INFO *pMonitoringinfo)   //쓰레드에서 돌려야될 것..
{
	static	DWORD	dwUpdateTrigerTime	= timeGetTime();     //함수 진입 시간.
	DWORD CurrTime = timeGetTime();
	static int nLogNotSaveCount=0;

	if(CurrTime - dwUpdateTrigerTime > 200)
	{
		//시작프로그램 항목 체크(모니터링 항목)
		pMonitoringinfo->MonitorOperStatus.bIs_OHTInk = File_Find(L"C:\\\\Documents and Settings\\Administrator\\시작 메뉴\\프로그램\\시작프로그램\\*.*",L"OHT.exe");
		pMonitoringinfo->MonitorOperStatus.bIs_AssistantInk=File_Find(L"C:\\\\Documents and Settings\\Administrator\\시작 메뉴\\프로그램\\시작프로그램\\*.*",L"OHTAssistant.exe");
		pMonitoringinfo->MonitorOperStatus.bIs_VNCInk=File_Find(L"C:\\\\Documents and Settings\\Administrator\\시작 메뉴\\프로그램\\시작프로그램\\*.*",L"Run WinVNC");

		//V3 체크
		if(FileExists(L"C:\\\\Program Files\\ESTsoft\\ALYac\\AYUpdate.exe"))
		{
			pMonitoringinfo->MonitorOperStatus.Is_VirusCheckProgram = 0x01;
			memmove(pMonitoringinfo->MonitorOperStatus.szVirusVersion, Get_FileAccessTime(L"C:\\\\Program Files\\ESTsoft\\ALYac\\AYUpdate.exe"), 16);
			memmove(pMonitoringinfo->MonitorOperStatus.szDBVersion, Get_FileAccessTime(L"C:\\\\Program Files\\ESTsoft\\ALYac\\VP\\PZPatch.dat"), 16);
		}
		else if (FileExists(L"C:\\\\Program Files\\AhnLab\\V3Lite\\V3Light.exe"))
		{
			pMonitoringinfo->MonitorOperStatus.Is_VirusCheckProgram = 0x02;
			memmove(pMonitoringinfo->MonitorOperStatus.szVirusVersion, Get_FileAccessTime(L"C:\\\\Program Files\\AhnLab\\V3Lite\\V3Light.exe"), 16);
			memmove(pMonitoringinfo->MonitorOperStatus.szDBVersion, Get_FileAccessTime(L"C:\\\\Program Files\\AhnLab\\V3Lite\\MUpdate2\\supdate.log"), 16);
		}
		else if (FileExists(L"C:\\\\Program Files\\AhnLab\\V3\\V3Pro32.exe"))
		{
			pMonitoringinfo->MonitorOperStatus.Is_VirusCheckProgram = 0x03;
			memmove(pMonitoringinfo->MonitorOperStatus.szVirusVersion, Get_FileAccessTime(L"C:\\\\Program Files\\AhnLab\\V3\\V3Pro32.exe"), 16);
			memmove(pMonitoringinfo->MonitorOperStatus.szDBVersion, Get_FileAccessTime(L"C:\\\\Program Files\\AhnLab\\Smart Update Utility\\supdate.log"), 16);
		}
		else
			pMonitoringinfo->MonitorOperStatus.Is_VirusCheckProgram = 0x00;

		//바이러스 체크프로그램 실행여부 체크
		pMonitoringinfo->MonitorOperStatus.bIsVirusCheckProgramExec = Find_IsVirusCheckProgramExec();

		//Check Log Alive, USB Alive(모니터링항목)
		if(pMonitoringinfo->MonitorOperStatus.LogSaveMode==LOGMODE_DRIVE)
		{
			pMonitoringinfo->MonitorOperStatus.bIs_LogSave = Get_IsLogDriveAlive("D:\\LOG");

			if(pMonitoringinfo->MonitorOperStatus.bIs_LogSave == 0x01) //dead
			{
			   nLogNotSaveCount++;
			   if(nLogNotSaveCount>50)        // about 10sec
			  {
					pMonitoringinfo->MonitorOperStatus.bIs_USBAlive = 0x02;  // USB Fault
					if(nLogNotSaveCount ==1000000) nLogNotSaveCount=60;   //오버플러우 방지
			  }
			}
			else
			{
				nLogNotSaveCount = 0;

				if(pMonitoringinfo->MonitorOperStatus.IPCSysInfo.nfreeSpace < 10000)  // If USB free space is under 10MB, then Error occure.
				{
					pMonitoringinfo->MonitorOperStatus.bIs_USBAlive = 0x01;  //  fully used
				}
				else
				{
					pMonitoringinfo->MonitorOperStatus.bIs_USBAlive = 0x00;  //  normal
				}
			}
		}

		//로그갯수(모니터링 항목)
		pMonitoringinfo->MonitorOperStatus.nLogCount = GetNum_LogFile(L"D:\\\\LOG\\*.*");

		//Assistant 프로그램 버전확인(모니터링 항목)
		OHTMainForm->VerInfo.GetFileVersionInformation(String(Application->ExeName + STR_FILENAME_OHTASSISTANT), "FileVersion");
		pMonitoringinfo->MonitorOperStatus.nAssistantVer = OHTMainForm->VerInfo.Major * 1000000 + OHTMainForm->VerInfo.Minor * 10000
														 + OHTMainForm->VerInfo.Release * 100 + OHTMainForm->VerInfo.Build;

		dwUpdateTrigerTime = CurrTime;
	}

}

bool MONITORING::loadParameter()
{
	bool bRetrun = false;
	double dTmpValue = 0.0;
	int    nTmpValue = 0;

	LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile("DataFiles\\MonitorSaveData.ini");

	if(!pIniFile->GetIniFileExist())
	{
		delete pIniFile;
		pIniFile = NULL;
	}

	if(pIniFile != NULL)
	{
		//2	(1)DRIVING_AXIS_PARAM
		//1.IniFile Read
		pIniFile->ReadDouble("Position","Driving", 0.0, &dTmpValue);
		gm_Monitoringinfo->MonitorMovementData.dSum_DrivingPosition = dTmpValue; //Return Value
		pIniFile->ReadDouble("Position","Hoist", 0.0, &dTmpValue);
		gm_Monitoringinfo->MonitorMovementData.dSum_HoistPosition = dTmpValue; //Return Value
		pIniFile->ReadDouble("Position","Shift", 0.0, &dTmpValue);
		gm_Monitoringinfo->MonitorMovementData.dSum_ShiftPosition = dTmpValue; //Return Value
		pIniFile->ReadDouble("Position","Rotate", 0.0, &dTmpValue);
		gm_Monitoringinfo->MonitorMovementData.dSum_RotatePosition = dTmpValue; //Return Value

		pIniFile->ReadInteger("MoveCount","Steer", 0, &nTmpValue);
		gm_Monitoringinfo->MonitorMovementData.nSum_SteerMoveCount = nTmpValue; //Return Value
		pIniFile->ReadInteger("MoveCount","Hand", 0, &nTmpValue);
		gm_Monitoringinfo->MonitorMovementData.nSum_HandMoveCount = nTmpValue; //Return Value
		pIniFile->ReadInteger("MoveCount","Shutter", 0, &nTmpValue);
		gm_Monitoringinfo->MonitorMovementData.nSum_ShutterMoveCount = nTmpValue; //Return Value


		delete pIniFile;
		bRetrun = true;
	}

	return bRetrun;

}

bool MONITORING::SaveParameter()
{
	bool bResult = false;

	LIniFile *m_lWriteIniFile;
	m_lWriteIniFile = NULL;
	m_lWriteIniFile = new LIniFile("DataFiles\\MonitorSaveData.ini");


	if(!m_lWriteIniFile->GetIniFileExist())
	{
		delete m_lWriteIniFile;
     	m_lWriteIniFile = NULL;
	}

	if(m_lWriteIniFile != NULL)
	{
		m_lWriteIniFile->DeleteIniFile();

		//1. [DRIVING_AXIS_PARAM]
		m_lWriteIniFile->WriteDouble("Position","Driving", gm_Monitoringinfo->MonitorMovementData.dSum_DrivingPosition);
		m_lWriteIniFile->WriteDouble("Position","Hoist", gm_Monitoringinfo->MonitorMovementData.dSum_HoistPosition);
		m_lWriteIniFile->WriteDouble("Position","Shift", gm_Monitoringinfo->MonitorMovementData.dSum_ShiftPosition);
		m_lWriteIniFile->WriteDouble("Position","Rotate", gm_Monitoringinfo->MonitorMovementData.dSum_RotatePosition);

		m_lWriteIniFile->WriteInteger("MoveCount","Steer", gm_Monitoringinfo->MonitorMovementData.nSum_SteerMoveCount);
		m_lWriteIniFile->WriteInteger("MoveCount","Hand", gm_Monitoringinfo->MonitorMovementData.nSum_HandMoveCount);
		m_lWriteIniFile->WriteInteger("MoveCount","Shutter", gm_Monitoringinfo->MonitorMovementData.nSum_ShutterMoveCount);

		delete m_lWriteIniFile;
		bResult = true;

	}

	return bResult;
}

//---------------------------------------------------------------------------
/**
@brief   MONITORING 생성자
@author  puting
@date    2013.07.22
@remark  실행시 한번만 값을 받아오는 항목은 이곳에 구현
*/
int MONITORING::MonitorInitData(MONITOR_OPERATION_STATUS *pOperationStatus)
{
	int nError=0;

	//FBWF 상태 체크
	pOperationStatus->bIsFBWF = Get_Dos_Cmd_Result(L"fbwfmgr");

	//로그 Save mode 체크
	/*
	AnsiString strOHTPath, strIniFileName;
	strOHTPath=ExtractFileDir(Application->ExeName);
	strIniFileName=AnsiString(strOHTPath+STR_DATAFILE_PATH)+"OHTPARAM.ini";
	TIniFile *TimeSetIni = new TIniFile (strIniFileName);
	pOperationStatus->LogSaveMode = TimeSetIni->ReadInteger("LOG","nIs_LogSaveMode",1);
	delete TimeSetIni;
	*/
    pOperationStatus->LogSaveMode = 2; //임시로 fixed

	//OHT, AMC 버전 체크
	pOperationStatus->nOHTVer = OHTMainForm->GetOHTProgramVersion().Info.Major * 1000000
												  + OHTMainForm->GetOHTProgramVersion().Info.Minor * 10000
												  + OHTMainForm->GetOHTProgramVersion().Info.Release * 100
												  + OHTMainForm->GetOHTProgramVersion().Info.Build;
	pOperationStatus->nAMCVer = OHTMainForm->GetHWSet()->GetAMCVersion();

    m_bCheckWinSetup = true;

   return nError;
}

//---------------------------------------------------------------------------
/**
@brief   MONITORING 생성자
@author  puting
@date    2013.07.22
*/
int MONITORING::Is_USBAlive(void)
{
   int nIsAlive=0;
    nIsAlive = Get_IsLogDriveAlive("D:\\LOG");

   return nIsAlive;
}
//---------------------------------------------------------------------------
/**
@brief   MONITORING 생성자
@author  puting
@date    2013.07.22
*/
int MONITORING::Get_IsLogDriveAlive(String strDrvName)
{
   TCHAR DrvRoot[4] = L"A:\\";
   int Drv;
   TCHAR VolName[MAX_PATH], FileSysName[10];
   DWORD VolSerial;
   bool bFind=false;
   AnsiString strTmpPath;
   AnsiString strFileFormat="";

   int nPos = strDrvName.Pos(":");
   if(nPos==0) return 0;

   strTmpPath = strDrvName.SubString(1, nPos-1);

   DWORD Drives = GetLogicalDrives();
   for(Drv=0; Drv<26; Drv++)
   {
      if((Drives >> Drv) & 0x00000001)
      {
         if((char)(Drv + 'A') == strTmpPath[1])
         {
            bFind=true;
            break;
         }
      }
   }

   if(bFind==false) return 0;

   DrvRoot[0] += Drv;
   GetVolumeInformation(DrvRoot, VolName, sizeof(VolName), &VolSerial, NULL, NULL, FileSysName, sizeof(FileSysName));

   strFileFormat=FileSysName;
   if((strFileFormat=="FAT32")||(strFileFormat=="NTFS")) return 0;
   else return 1;

}
//---------------------------------------------------------------------------
/**
@brief   MONITORING 생성자
@author  puting
@date    2013.07.22
//프로세스 실행파일을 찾아 종료하기

int MONITORING::Kill_VirusCheckProgram(void)
{
   int nError=0;

   HANDLE IdProcess, hToken;
   PROCESSENTRY32 Pren32 = {0} ;
   Pren32.dwSize = sizeof(PROCESSENTRY32);
   HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

   if(hSnapshot == INVALID_HANDLE_VALUE) return 1;
   if(!Process32First(hSnapshot, &Pren32)) return 1;

   AnsiString sProcessName;
   do
   {
      sProcessName=Pren32.szExeFile;
      if((sProcessName=="V3LTray.exe")||(sProcessName=="V3LSvc.exe")||(sProcessName=="AYAgent.aye")||(sProcessName=="AYServiceNT.aye"))
      {
         if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
         {
			SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);

            if((IdProcess=OpenProcess(PROCESS_ALL_ACCESS,false,Pren32.th32ProcessID))!=NULL)
            {
               TerminateProcess(IdProcess,-1);
               CloseHandle(IdProcess);
            }
            SetPrivilege(hToken, SE_DEBUG_NAME, FALSE);
            CloseHandle(hToken);
         }
	  }
   }while(Process32Next(hSnapshot, &Pren32));

   CloseHandle(hSnapshot);

   return nError;
}
*/
//---------------------------------------------------------------------------
/**
@brief   MONITORING 생성자
@author  puting
@date    2013.07.22
*/
BYTE MONITORING::Find_IsVirusCheckProgramExec(void)
{
   int bFindCount=0;

   PROCESSENTRY32 Pren32 = {0} ;
   Pren32.dwSize = sizeof(PROCESSENTRY32);
   HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

   if(hSnapshot == INVALID_HANDLE_VALUE) return bFindCount;
   if(!Process32First(hSnapshot, &Pren32)) return bFindCount;

   AnsiString sProcessName;
   do
   {
      sProcessName=Pren32.szExeFile;
      if((sProcessName=="V3LTray.exe")||(sProcessName=="V3LSvc.exe")||(sProcessName=="AYAgent.aye")||(sProcessName=="AYServiceNT.aye"))
      {
         bFindCount++;
      }
   }while(Process32Next(hSnapshot, &Pren32));

   CloseHandle(hSnapshot);

   return bFindCount;

}
//---------------------------------------------------------------------------
/**
@brief   MONITORING 생성자
@author  puting
@date    2013.07.22
*/
bool MONITORING::File_Find(String strTmpPath, String strPosData)
{
    bool bFind=false;
   TSearchRec sr;
   String strTmpFileName;

   if(FindFirst(strTmpPath, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
   {
    //디렉토리를 제외하고 파일만을 검색한다
      if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
      {
      //검색한 내용이 파일이면
         if(!((sr.Name == ".") || (sr.Name == "..")))
         {
            strTmpFileName=sr.Name;
            if(strTmpFileName.Pos(strPosData)!=0) bFind=true;
         }
      }
	  if(bFind==false)
      {
         while(FindNext(sr) == 0)
         {
            if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
            {
               if((sr.Name == ".") || (sr.Name == "..")) continue;
               strTmpFileName=sr.Name;
			   if(strTmpFileName.Pos(strPosData)!=0)
               {
                  bFind=true;
                  break;
               }
            }
         }
      }
   }
   FindClose(sr);

   return bFind;
}
//---------------------------------------------------------------------------
/**
@brief   MONITORING 생성자
@author  puting
@date    2013.07.22
*/
char* MONITORING::Get_FileAccessTime(String strFileName)
{
   TSearchRec sr;
   TDateTime fDate;
   String strDate="";
   strDate.sprintf(L"%04d%02d%02d%02d%02d",0,0,0,0,0);

   if(FindFirst(strFileName, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
   {
	//디렉토리를 제외하고 파일만을 검색한다
	  if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
	  {
	  //검색한 내용이 파일이면
		 if(!((sr.Name == ".") || (sr.Name == "..")))
		 {
			fDate = FileDateToDateTime(sr.Time);
			strDate = FormatDateTime("yyyymmddhhmm",fDate);
		 }
	  }
   }
   FindClose(sr);

   return WCharToChar(strDate.t_str());
}
//---------------------------------------------------------------------------
/**
@brief   MONITORING 생성자
@author  puting
@date    2013.07.22
//HANDLE hToken:토큰 핸들, LPCTSTR Privilege:활성/비활성화할 권한, BOOL bEnablePrivilege:권한 활성화 여부

BOOL MONITORING::SetPrivilege(HANDLE hToken, LPCTSTR Privilege, BOOL bEnablePrivilege)
{
   TOKEN_PRIVILEGES tp;
   LUID luid;
   TOKEN_PRIVILEGES tpPrevious;
   DWORD cbPrevious=sizeof(TOKEN_PRIVILEGES);

   if(!LookupPrivilegeValue( NULL, Privilege, &luid )) return FALSE;

   // 현재의 권한 설정 얻기
   tp.PrivilegeCount   = 1;
   tp.Privileges[0].Luid   = luid;
   tp.Privileges[0].Attributes = 0;

   AdjustTokenPrivileges(
                         hToken,
                         FALSE,
                         &tp,
                         sizeof(TOKEN_PRIVILEGES),
                         &tpPrevious,
                         &cbPrevious
                        );

   if(GetLastError() != ERROR_SUCCESS) return FALSE;

   // 이전의 권한 설정에 따라 권한 설정하기
   tpPrevious.PrivilegeCount       = 1;
   tpPrevious.Privileges[0].Luid   = luid;

   if(bEnablePrivilege)
   {
      tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
   }
   else
   {
      tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
      tpPrevious.Privileges[0].Attributes);
   }

   AdjustTokenPrivileges(
                         hToken,
                         FALSE,
                         &tpPrevious,
                         cbPrevious,
                         NULL,
                         NULL
                        );

   if(GetLastError() != ERROR_SUCCESS) return FALSE;

   return TRUE;
}
*/
//---------------------------------------------------------------------------
/**
@brief   MONITORING 생성자
@author  puting
@date    2013.07.22
*/
int MONITORING::GetNum_LogFile(String Path)
{
   TSearchRec sr;
   int nCount=0;

   if(FindFirst(Path, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
   {
    //디렉토리를 제외하고 파일만을 검색한다
	  if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
      {
      //검색한 내용이 파일이면

		 if(!((sr.Name == ".") || (sr.Name == "..")))
         {
            nCount++;
         }
      }
      while(FindNext(sr) == 0)
      {
         if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
         {
            if((sr.Name == ".") || (sr.Name == "..")) continue;
            nCount++;
         }
      }
   }
   FindClose(sr);

   return nCount;
}
//---------------------------------------------------------------------------
/**
@brief   MONITORING 생성자
@author  puting
@date    2013.07.22
*/
BYTE MONITORING::Get_Dos_Cmd_Result(TCHAR* strDosCmd)
{
   bool                    bflag;
   HANDLE                  hIwrite, hIread;
   HANDLE                  hOwrite, hOread;
   HANDLE                  hEwrite, hEread;
   SECURITY_ATTRIBUTES     sa;
   STARTUPINFO            siParam;
   PROCESS_INFORMATION     piParam;
   DWORD                   dwExitCode;

   char buffer[512];
   DWORD BytesRead;
   AnsiString strData;

   // Pipe Parameter Set
   sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor = NULL;
   sa.bInheritHandle = true;

  // memoDosCmd->Clear();            //puting


   // 어노니머스 파이프 생성  // Input File
   bflag = CreatePipe(&hIread, &hIwrite, &sa, 0);
   if (!bflag)
   {
	 // memoDosCmd->Lines->Add("ERROR:Fail to open pipe");  puting
	  CloseHandle(hIread);
	  CloseHandle(hIwrite);
	  return false;
   }

   // 어노니머스 파이프 생성  // output File
   bflag = CreatePipe(&hOread, &hOwrite, &sa, 0);
   if (!bflag)
   {
   //   memoDosCmd->Lines->Add("ERROR:Fail to open pipe");  puting
      CloseHandle(hIread);
      CloseHandle(hIwrite);
      CloseHandle(hOread);
      CloseHandle(hOwrite);
	  return false;
   }

   // 어노니머스 파이프 생성 // Error File
   bflag = CreatePipe(&hEread, &hEwrite, &sa, 0);
   if (!bflag)
   {
	 // memoDosCmd->Lines->Add("ERROR:Fail to open pipe");    //puting
      CloseHandle(hIread);
      CloseHandle(hIwrite);
      CloseHandle(hOread);
      CloseHandle(hOwrite);
      CloseHandle(hEread);
	  CloseHandle(hEwrite);
	  return false;
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

   bflag = CreateProcess(NULL, strDosCmd, NULL, NULL, true, DETACHED_PROCESS, NULL, NULL, &siParam, &piParam);
   WaitForInputIdle(GetCurrentProcess(), INFINITE);

   if(!bflag)
   {
     CloseHandle(hIread);
     CloseHandle(hIwrite);
     CloseHandle(hOread);
     CloseHandle(hOwrite);
     CloseHandle(hEread);
     CloseHandle(hEwrite);
	// memoDosCmd->Lines->Add("ERROR:Fail to create process");
	 return false;
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
		// UpdateWindow(Handle);
	  }
   }

   CloseHandle(hIread);
   CloseHandle(hOwrite);
   CloseHandle(hEwrite);


   while(ReadFile(hOread, buffer, sizeof(buffer)-1, &BytesRead, NULL) && BytesRead)
   {
	buffer[BytesRead] = '\0';
   }

   CloseHandle(hOread);
   CloseHandle(hEread);
   CloseHandle(hIwrite);

   ///디스플레이부분  및 IsFBWF설정

	if(wcscmp(strDosCmd, L"fbwfmgr")==0)
   {
	  int i;

	  TStringList* tmpResult = new TStringList;
	  tmpResult->Add(buffer);

	  for(i=0; i<tmpResult->Count; i++)
	  {
		 strData = tmpResult->Strings[i];

		 if(strData.Pos("filter state: enabled") != 0)
		 {
			delete tmpResult;
			OHTMainForm->memoMain->Lines->Add(L"FBWF Enable");
			return true;
		 }
		 else if(strData.Pos("filter state: disabled") != 0)
		 {
	        delete tmpResult;
			OHTMainForm->memoMain->Lines->Add(L"FBWF Disable");
			return false;
		 }
	  }

	  delete tmpResult;
   }

   return false;
}
//---------------------------------------------------------------------------

bool MONITORING::IsWinSetupLoaded()
{
    return m_bCheckWinSetup;
}
