//---------------------------------------------------------------------------

#include <vcl.h>
#include <dir.h>
#include <mmsystem.h>
#include <IniFiles.hpp>

#include "FMcontrolThreadUnit.h"
#include "UDP.h"
#include "UdpFM.h"
#include "ProcessTerminator.h"
#include "FileControl.h"
//#include "Trace.h"

#define FM_STATUS_READY			(0)
#define FM_STATUS_FTPCONNECTION	(1)
#define FM_STATUS_DOWNLOAD		(2)

#define FM_MONITORING_CNT_MAX	(2000)	//30ms*2000= 60000ms FM으로 부터 설정 시간 동안 응답이 없으면 FM 재시작
										//파일 송,수신이 60초 넘어가면 안됨.
#define FM_MONITORING_INTERVAL	(100)	//30ms*100 = 3000ms 3초 간격으로 Ping 전송


#define STATUS_MACHINE_INTERVAL	(17)	// 30ms * 17 = 510ms


#define FILE_MANAGER_RUN_CHECK  1	// 파일매니저 자동실행기능여부 (1=사용, 0=사용안함)

#pragma hdrstop

#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall FileThreadUnit::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------
FMcontrolThreadUnit *ptrFMcontrolThread = NULL;
//---------------------------------------------------------------------------

__fastcall FMcontrolThreadUnit::FMcontrolThreadUnit(bool CreateSuspended) : TThread(CreateSuspended)
{
   Priority = tpNormal;
   Resume();
   dwSTTime=timeGetTime();

#if FILE_MANAGER_RUN_CHECK
	;
#else
   ShowMessage("파일매니저 자동실행 기능 해제상태 (Debug전용). 주의 바랍니다");
#endif
}
//---------------------------------------------------------------------------
//OHT <-> Assistant <-> FM 간 UDP packet을 전달한다.
//Assistant는 FM을 모니터링 하여 FM을 재기동 시킨다.
void __fastcall FMcontrolThreadUnit::Execute()
{
	int iResult;
	int iMachineType;
	int i;
	int FM_Monitoring_cnt =0;
	int Ping_Interval_cnt =0;
	int FM_Status =0;

	int OHT_MAIN_RUN_Interval_cnt = 0;
	int iMainRun_Status = OHT_MAIN_STEP_NONE;

   	bool bOhtMain_CopyNeed = false;	 // amc인 경우 불필요하므로
	bool bOhtParam_CopyNeed = false;
	bool bAmcDll_CopyNeed = false;  //AOHC-154

	String strLog;  // Log를 위한 변수



	//AOHC-154     오토패치 필요 인자 받아오기
	memset(&m_UpdateIniChk, 0, sizeof(UPDATEINI_CHECK));

	AnsiString strFileName =  FIRMWARE_UPDATE_PATH;
	strFileName = strFileName + "UPDATE.INI";
//	strLog.sprintf(L"LoadUpdateIniFile");
//	ADD_LOG(strLog);
	LoadUpdateIniFile(strFileName);
	bAmcDll_CopyNeed = m_UpdateIniChk.bAmcDLLUpdate;
//	strLog.sprintf(L"bAmcDll_CopyNeed %d", bAmcDll_CopyNeed);
//	ADD_LOG(strLog);

	DWORD OHT_PID = 0;
	DWORD OHT_PID_Received = 0;
	int ShutDownDelay = 0;
	char buff[MAX_PACKET_SIZE+1];
	PKT_OHT2OCS SendPkt2OHT;
	PKT_OCS2OHT SendPkt2FM;
	int iPacketSize;
	
	FIRMWARE_INFO FIRMWARE_UPDATE_INFO_AS;
	AnsiString strResult;


	Sleep(5000);
	//---- Place thread code here ----
	while( !Terminated )
	{
		iResult = frmUDP->pUdpFM->AS_ThreadCall(buff, &iMachineType);
		
		if( iResult >= 10)
		{
			if(iMachineType == MACHINETYPE_VHL_COMMON)
			{
				iPacketSize = sizeof(PKT_CHECK_TAG);
				PKT_CHECK_TAG Pkt;
				memcpy(&Pkt, &buff, iPacketSize);

				if(Pkt.TAG.Value == TagID::CMD_FIRMWARE_UPDATE_RESTART_REQ_OHT2AS)
				{
					PKT_RESTART_REQUEST_OHT2AS CmdPkt;
					memcpy(&CmdPkt, &buff, sizeof(PKT_RESTART_REQUEST_OHT2AS));

					// Status Machine Trigger
					// ShowMessage("Hello");

					OHT_MAIN_RUN_Interval_cnt = 0;
					iMainRun_Status = OHT_MAIN_STEP_ORDER_RECEIVED;
					bOhtMain_CopyNeed = CmdPkt.BODY.iOhtMainCopyNeed;	// Main 복사 필요성 여부
					bOhtParam_CopyNeed = CmdPkt.BODY.iOhtParamCopyNeed;	// Param 복사 필요성 여부
					OHT_PID_Received = CmdPkt.BODY.OHTPID;
					ShutDownDelay = CmdPkt.BODY.ShutDownDelay;

					if(bOhtParam_CopyNeed == true )
					{
//						AnsiString strFileName =  FIRMWARE_UPDATE_PATH;
//						//bool bNeedMainRestart = false;	// AMC와 OHT.exe의 업데이트 시, Main을 재 시작해야 하므로, 이를 저장하는 변수임
//						int iResult = 0;
//
//						strFileName = strFileName + "UPDATE.INI";
//						memset(&FIRMWARE_UPDATE_INFO_AS, 0, sizeof(FIRMWARE_INFO));
//
//						LoadUpdateIniPARAMFile(FwIniFile, &FIRMWARE_UPDATE_INFO_AS);
					}
					else
					{
//						memset(&FIRMWARE_UPDATE_INFO_AS, 0, sizeof(FIRMWARE_INFO));
					}
					strResult.sprintf("[Firmware Update] OHT_MAIN_STEP_ORDER_RECEIVED %d, %d, %d, %d", iMainRun_Status, bOhtMain_CopyNeed, OHT_PID_Received, ShutDownDelay);
					frmUDP->DisplayLog(strResult);
//					strLog.sprintf(L"[Firmware Update] OHT_MAIN_STEP_ORDER_RECEIVED %d, %d, %d, %d", iMainRun_Status, bOhtMain_CopyNeed, OHT_PID_Received, ShutDownDelay);
//					ADD_LOG(strLog);
				}
				else
				{
					frmUDP->pUdpFM->UnicastSend_AS_2_FM(buff,iResult);
				}
			}
			else if(iMachineType == MACHINETYPE_FM)
			{
				iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG를 판독해야 하므로.

				PKT_CHECK_TAG Pkt;
				memcpy(&Pkt, &buff, iPacketSize);	// 버퍼로 넘어온 정보중 헤더크기만큼만 일단 복사한다
				if(Pkt.TAG.Value == TagID::CMD_INERNAL_PING_RESPONSE)
				{
					//FM으로부터 들어온 패킷이 CMD_INERNAL_PING_RESPONSE인 경우
					//OHT로 전달은 안하고, FM_Monitoring_cnt 초기화 시킴.
					FM_Monitoring_cnt = 0;
				}
				else
				{
					//FM으로부터 들어온 패킷이 CMD_INERNAL_PING_RESPONSE가 아닌경우 OHT로 전달
					frmUDP->pUdpFM->UnicastSend_AS_2_OHT(buff,iResult);
				}				
            }
		}

		// ------------- Status Machine Pos -------------------
		if(OHT_MAIN_RUN_Interval_cnt > STATUS_MACHINE_INTERVAL)
		{
			OHT_MAIN_RUN_Interval_cnt = 0;
			
			if(iMainRun_Status == OHT_MAIN_STEP_NONE)
			{
				;
			}
			else
			{
				switch(iMainRun_Status)
				{
					case OHT_MAIN_STEP_ORDER_RECEIVED:
					{
						frmUDP->DisplayLog("OHT_MAIN_STEP_ORDER_RECEIVED");

//						strLog.sprintf(L"[OHT_MAIN_STEP_ORDER_RECEIVED");
//						ADD_LOG(strLog);
						iMainRun_Status++;

					}
					break;

					case OHT_MAIN_STEP_CLOSE_ORDER:
					{
						// ------------------------------------------------
						int iSize = sizeof(PKT_RESTART_KEEPING_INFO_AS2OHT);
						
						PKT_RESTART_KEEPING_INFO_AS2OHT SEND_PKT;
						SEND_PKT.HEADER.MachineType = MACHINETYPE_AS;
						SEND_PKT.TAG.Value = TagID::CMD_FIRMWARE_UPDATE_CLOSE_ORDER_AS2OHT;

						frmUDP->pUdpFM->UnicastSend_AS_2_OHT((char*)&SEND_PKT, iSize);
						frmUDP->DisplayLog("[Firmware Update] OHT.exe close order sended");
//						strLog.sprintf(L"[Firmware Update] OHT.exe close order sended");
//						ADD_LOG(strLog);
					   //	Sleep(500);
					   Sleep(5000);      //amc dll 패치
						// ------------------------------------------------

						int result;
						OHT_PID = ProcessTerminator::getInstance().FindPID(OHT_MAIN_FILE_NAME);
						strResult.sprintf("[Firmware Update] OHT_PID %d, OHT_PID_Received : %d", OHT_PID, OHT_PID_Received);
//						strLog.sprintf(L"[Firmware Update] OHT_PID %d, OHT_PID_Received : %d", OHT_PID, OHT_PID_Received);
//						ADD_LOG(strLog);
						frmUDP->DisplayLog(strResult);

						if( OHT_PID == NULL )
						{
							OHT_PID = OHT_PID_Received;
						}

						if( OHT_PID != NULL )
						{
							frmUDP->DisplayLog("[Firmware Update] OHT.exe running found. try kill process ");
//							strLog.sprintf(L"[Firmware Update] OHT.exe running found. try kill process ");
//							ADD_LOG(strLog);
							result = ProcessTerminator::getInstance().TerminateApp(OHT_PID, 1000);
							Sleep(2000);//OHT.exe 종료 시간 확보
							Sleep(ShutDownDelay);//OHT.exe 종료 시간 추가 확보
							
							switch(result)
							{
								case TA_FAILED:
									result = ProcessTerminator::getInstance().TerminateApp(OHT_PID, 1000);	// retry
									frmUDP->DisplayLog("[Firmware Update] OHT.exe running found. try kill process2 ");
									Sleep(2000);//OHT.exe 종료 시간 확보
									break;
								case TA_SUCCESS_CLEAN:
								case TA_SUCCESS_KILL:
								{
									//FileControl::getInstance().ExecFMProgram();
									// Sleep(100);	//FileManager.exe 실행 시간 확보
								}
									break;
							}
						}

						strResult.sprintf("[Firmware Update] OHT.exe kill. result %d", result);
						frmUDP->DisplayLog(strResult);					

						iMainRun_Status++;
					}
					break;

					case OHT_MAIN_STEP_COPY:
					{
						AnsiString strSourceFile, strDestFile;
						int iErrorCode;
						
						if(bOhtMain_CopyNeed == true)
						{
							strSourceFile = FIRMWARE_UPDATE_PATH;
							strSourceFile = strSourceFile + OHT_MAIN_FILE_NAME;

							strDestFile = OHT_PROGRAM_PATH;
							strDestFile = strDestFile + OHT_MAIN_FILE_NAME;

							iErrorCode = CopyFileA(strSourceFile.c_str(), strDestFile.c_str(), false);
							Sleep(3000);//Copy 시간 확보

							strResult.sprintf("OHT_MAIN_STEP_COPY : COPYING...  Result %d", iErrorCode);
//								strLog.sprintf(L"OHT_MAIN_STEP_COPY : COPYING...  Result %d", iErrorCode);
//							ADD_LOG(strLog);
							frmUDP->DisplayLog(strResult);
						}
						else
						{
							frmUDP->DisplayLog("OHTMainCopy false.."); //AOHC-154
//							strLog.sprintf(L"OHTMainCopy false..");
//							ADD_LOG(strLog);

							//기존 메인 실행 되는지 메인 실행 안되는지 확인 필요.
							//Copy 안된경우 처리 방법 필요.
						}
						// copy parameter files
						if(bOhtParam_CopyNeed == true)
						{
							FirmwareUpdate_PARAM(FIRMWARE_UPDATE_INFO_AS);

							memset(&FIRMWARE_UPDATE_INFO_AS, 0, sizeof(FIRMWARE_INFO));
						}
						else
						{
							frmUDP->DisplayLog("OHTParamCopy false..");  //AOHC-154

							//기존 메인 실행 되는지 메인 실행 안되는지 확인 필요.
							//Copy 안된경우 처리 방법 필요.
						}
						//AOHC-154
						if(bAmcDll_CopyNeed == true)
						{
							strSourceFile = FIRMWARE_UPDATE_PATH;
							strSourceFile = strSourceFile + AMC_DLL_FILE_NAME;

							strDestFile = OHT_PARAM_FILE_PATH;
							strDestFile = strDestFile + AMC_DLL_FILE_NAME;

							iErrorCode = CopyFileA(strSourceFile.c_str(), strDestFile.c_str(), false);
							Sleep(3000);//Copy 시간 확보

							strResult.sprintf("AMC_DLL_COPY : COPYING...  Result %d", iErrorCode);
							frmUDP->DisplayLog(strResult);
//								strLog.sprintf(L"AMC_DLL_COPY : COPYING...  Result %d", iErrorCode);
//							ADD_LOG(strLog);


						}
						else
						{
							frmUDP->DisplayLog("AMCDLLCopy false..");
//							strLog.sprintf(L"AMCDLLCopy false..");
//							ADD_LOG(strLog);
							//Copy 안된경우 처리 방법 필요.

						}

					   	iMainRun_Status++;
					}
					break;

					case OHT_MAIN_STEP_MAIN_RUN:
					{
						AnsiString strSourceFile;

						strSourceFile = OHT_PROGRAM_PATH;
						strSourceFile = strSourceFile + OHT_MAIN_FILE_NAME;
						ExecuteProgram(strSourceFile);

						frmUDP->DisplayLog("OHT_MAIN_STEP_MAIN_RUN");
						iMainRun_Status++;
					}
					break;

					case OHT_MAIN_STEP_MODE_AUTO:
					{
						int iSize = sizeof(PKT_RESTART_KEEPING_INFO_AS2OHT);

						PKT_RESTART_KEEPING_INFO_AS2OHT SEND_PKT;
						SEND_PKT.HEADER.MachineType = MACHINETYPE_AS;
						SEND_PKT.TAG.Value = TagID::CMD_FIRMWARE_UPDATE_AUTOMODE_ORDER_AS2OHT;

						frmUDP->pUdpFM->UnicastSend_AS_2_OHT((char*)&SEND_PKT, iSize);
						frmUDP->DisplayLog("OHT_MAIN_STEP_MODE_AUTO");

						system("del /q C:\\User\\OHT\\DataFiles\\Down\\*.*"); // 패치 이후 Down 폴더에 있는 모든 파일 삭제
						system("del /q C:\\User\\OHT\\AMC\\Update\\*.*"); // 패치 이후 AMC Update 폴더에 있는 모든 파일 삭제

						iMainRun_Status = OHT_MAIN_STEP_NONE;
					}
					break;

					case OHT_MAIN_STEP_END:
					{
						frmUDP->DisplayLog("OHT_MAIN_STEP_END");
						iMainRun_Status = OHT_MAIN_STEP_NONE;		// State Machine end
					}
					break;

					default:
					{
						frmUDP->DisplayLog("DELAY 300ms");
						iMainRun_Status++;
					}
					break;
				}
			}
		}
		else
		{
			OHT_MAIN_RUN_Interval_cnt++;
		}

		// ----------------------------------------------------
		if(Ping_Interval_cnt >= FM_MONITORING_INTERVAL)
		{
			PKT_OCS2OHT SendPkt;
			int iPacketSize = sizeof(PKT_OCS2OHT);
		
			SendPkt.HEADER.MachineType = MACHINETYPE_AS;
			SendPkt.TAG.Value = TagID::CMD_INERNAL_PING_REQ;
		
			frmUDP->pUdpFM->UnicastSend_AS_2_FM((char*)&SendPkt, iPacketSize);
			Ping_Interval_cnt = 0;
		}
		if(FM_Monitoring_cnt >= FM_MONITORING_CNT_MAX)
		{
			//FM 응답 없음
			//FM process 찾아서 Kill
			//FM 재실행

			// shkim. 응답 없으면 kill 하는 기능 일시잠금
#if FILE_MANAGER_RUN_CHECK
			DWORD PID = ProcessTerminator::getInstance().FindPID(FM_NAME);
			if( PID != NULL )
            {
				int result = ProcessTerminator::getInstance().TerminateApp(PID, 1000);
             	switch(result)
				{
					case TA_FAILED:
						break;
					case TA_SUCCESS_CLEAN:
					case TA_SUCCESS_KILL:
					{
						FileControl::getInstance().ExecFMProgram();
						Sleep(5000);//FileManager.exe 실행 시간 확보
					}
						break;
				}
			}
			else
			{
				FileControl::getInstance().ExecFMProgram();
				Sleep(5000);//FileManager.exe 실행 시간 확보
			}
#endif

			FM_Monitoring_cnt = 0;
		}
		else
		{
			if(FM_Monitoring_cnt % 100 == 99)
			{
				strResult.sprintf("[FM_Monitoring] FM_Monitoring_cnt : %d", FM_Monitoring_cnt);
				frmUDP->DisplayLog(strResult);
			}
		}
		
		Ping_Interval_cnt += 1;
		FM_Monitoring_cnt += 1;
		WaitForSingleObject((void *)Handle, 30);//Sleep(30)보다 CPU 점유가 낮음.
	}
}

//----------------------------------------------------------------------------

int FMcontrolThreadUnit::ExecuteProgram(String strFileName)
{
	HINSTANCE hResult;
	int nResult;
	// String strFileName;
	String strRes;

	// strFileName = ".\\" + sFileName;

	hResult = ShellExecute(NULL, L"open", strFileName.c_str(), NULL, NULL, SW_SHOW);
	nResult = (int)hResult;
	switch(nResult)
	{
	case 0:
		strRes = "ERROR : The operating system is out of memory or resources.";
		break;
	case ERROR_FILE_NOT_FOUND:
		strRes = "ERROR : The specified file was not found.";
		break;
	case ERROR_PATH_NOT_FOUND:
		strRes = "ERROR : The specified path was not found.";
		break;
	case ERROR_BAD_FORMAT:
		strRes = "ERROR : The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).";
		break;
	case SE_ERR_ACCESSDENIED:
		strRes = "ERROR : The operating system denied access to the specified file.";
		break;
	case SE_ERR_ASSOCINCOMPLETE:
		strRes = "ERROR : The filename association is incomplete or invalid.";
		break;
	case SE_ERR_DDEBUSY:
		strRes = "ERROR : The DDE transaction could not be completed because other DDE transactions were being processed.";
		break;
	case SE_ERR_DDEFAIL:
		strRes = "ERROR : The DDE transaction failed.";
		break;
	case SE_ERR_DDETIMEOUT:
		strRes = "ERROR : The DDE transaction could not be completed because the request timed out.";
		break;
	case SE_ERR_DLLNOTFOUND:
		strRes = "ERROR : The specified dynamic-link library was not found.";
		break;
	case SE_ERR_NOASSOC:
		strRes = "ERROR : There is no application associated with the given filename extension.";
		break;
	case SE_ERR_OOM:
		strRes = "ERROR : There was not enough memory to complete the operation.";
		break;
	case SE_ERR_SHARE:
		strRes = "ERROR : A sharing violation occurred.";
		break;
	default:
		strRes = "OHT Execute ... complete..";
		break;
	}
}

void FMcontrolThreadUnit::UdpShow()
{
	frmUDP->Show();
}


void FMcontrolThreadUnit::UdpHide()
{
	frmUDP->Hide();
}

// Function copy from OHT Main
// Copy Parameter files 
int FMcontrolThreadUnit::FirmwareUpdate_PARAM(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO)
{
	AnsiString strSourceFile, strDestFile, strApplyFileName;

	int iErrorCount = 0;
	int iErrorCode = 0;
	AnsiString strResult;

	// 1) 파라미터 파일을 복사(반복적으로) 한다
	for(int i = 0; i < FIRMWARE_UPDATE_INFO.iParamFileCount; i++)
	{
		// 2) Param file copy
		strSourceFile = FIRMWARE_UPDATE_PATH;
		strSourceFile = strSourceFile + FIRMWARE_UPDATE_INFO.strParamFileList[i];

		strDestFile = OHT_PARAM_FILE_PATH;
		strDestFile = strDestFile + FIRMWARE_UPDATE_INFO.strParamFileList[i];

		strApplyFileName = FIRMWARE_UPDATE_INFO.strParamFileList[i];
		strApplyFileName = strApplyFileName.Trim();
		strApplyFileName = strApplyFileName.UpperCase();

		iErrorCode = CopyFileA(strSourceFile.c_str(), strDestFile.c_str(), false);

		if(iErrorCode != 1) // result 1 is copy success
		{
			iErrorCount++;
			strResult.sprintf("[Firmware Update] Param file copy error. Name = %s All Count : %d Err Count %d",
									strSourceFile.c_str(), FIRMWARE_UPDATE_INFO.iParamFileCount, iErrorCount);
			frmUDP->DisplayLog(strResult);
		}
		else
		{
			if(strApplyFileName == "DRIVING.INI")
			{
				strResult.sprintf("[Firmware Update] Driving.ini copy...");
				frmUDP->DisplayLog(strResult);
				//FirmwareUpdateMessageToTp("[Firmware Update] Driving.ini loading...");
				//ParamApply(UPDATE_DRIVING_INI);
			}
			else if(strApplyFileName == "TRANS.INI")
			{
				strResult.sprintf("[Firmware Update] Trans.ini copy...");
				frmUDP->DisplayLog(strResult);
				//FirmwareUpdateMessageToTp("[Firmware Update] Trans.ini loading...");
				//ParamApply(UPDATE_TRANS_INI);
			}
			else if(strApplyFileName == "EQPIO.INI")
			{
				strResult.sprintf("[Firmware Update] Eqpio.ini copy...");
				frmUDP->DisplayLog(strResult);
				//FirmwareUpdateMessageToTp("[Firmware Update] Eqpio.ini loading...");
				//ParamApply(UPDATE_EQPIO_INI);
			}
			else if(strApplyFileName == "DIAGPARAM.INI")
			{
				strResult.sprintf("[Firmware Update] DiagParam.ini copy...");
				frmUDP->DisplayLog(strResult);
				//FirmwareUpdateMessageToTp("[Firmware Update] DiagParam.ini loading...");
				//ParamApply(UPDATE_DIAG_PARAM_INI);
			}
			else if(strApplyFileName == "PATHSEARCH.INI")
			{
				strResult.sprintf("[Firmware Update] PathSearch.ini copy...");
				frmUDP->DisplayLog(strResult);
				//FirmwareUpdateMessageToTp("[Firmware Update] PathSearch.ini loading...");
				//ParamApply(UPDATE_PATHSEARCH_INI);
			}
			else if(strApplyFileName == "IOMAP.INI")
			{
				strResult.sprintf("[Firmware Update] IoMap.ini copy...");
				frmUDP->DisplayLog(strResult);
				//FirmwareUpdateMessageToTp("[Firmware Update] IoMap.ini loading...");
				//ParamApply(UPDATE_IOMAP_INI);
			}
		}
	}

	// Param Copy
	if(iErrorCount == 0)
	{
		strResult.sprintf("[Firmware Update] Parameter copy execute...");
		frmUDP->DisplayLog(strResult);
	}
	else
	{
		strResult.sprintf("[Firmware Update] Parameter copy Failed iErrorCount:%d ",iErrorCount);
		frmUDP->DisplayLog(strResult);
	}

	return iErrorCount;
}

int FMcontrolThreadUnit::LoadUpdateIniPARAMFile(TIniFile *FwIniFile, FIRMWARE_INFO* Info)
{
	int Error = NO_ERR;
	AnsiString strTempFileName;
	AnsiString strTempValue;
	AnsiString strResult;

	// 펌웨어 업데이트 관련한 변수의 초기화
	Info->iMainUpdate	= FALSE;	// OHT Main을 업데이트 할지 여부를 설정한다
	Info->iAmcUpdate 	= FALSE;	// AMC를 업데이트 할지 여부를 설정한다
	Info->iAsUpdate		= FALSE;	// 어시스턴스를 업데이트 할지 여부를 설정한다
	Info->iFmUpdate		= FALSE;	// File Manager를 업데이트 할지 여부를 설정한다
	Info->iParamUpdate	= FALSE;	// 파라미터를 업데이트 할지 여부를 설정한다

	Info->iWirelessConfig = FALSE;	// 무선모듈 업데이트 여부를 설정한다
	Info->iWirelessMacfil = FALSE;
	Info->iWirelessFirmware = FALSE;

	Info->iParamFileCount = 0;		// 업데이트할 파일의 갯수


	// 파라미터 로딩
	int iTmpCount = 0;
	for (int i = 1; i < 100; i++) 	// 파일은 01부터 99까지임
	{
		strTempFileName.sprintf("ParamFile%02d",i);
		strTempValue = FwIniFile->ReadString("PARAMETER", strTempFileName,"N");

		if(strTempValue.UpperCase() != "N")
		{
			Info->strParamFileList[iTmpCount] = strTempValue;

			strResult.sprintf("[Firmware Update] ParamFile = %s Checked", strTempValue);
			frmUDP->DisplayLog(strResult);
			iTmpCount++;
		}
	}

	Info->iParamFileCount = iTmpCount;
	strResult.sprintf("[Firmware Update] ParamFile Count = %d", iTmpCount);
	frmUDP->DisplayLog(strResult);

}
int FMcontrolThreadUnit::LoadUpdateIniFile(AnsiString strINIFilePath)
{
	int nTmpValue = 0;
	AnsiString strTempValue;
		// INI Load
	TIniFile *FwIniFile= new TIniFile(strINIFilePath);


	if(FwIniFile !=NULL)
	{
		strTempValue = FwIniFile->ReadString("TARGET","MainUpdate"	,"N");
		if(strTempValue.UpperCase() != "N")
		{
			m_UpdateIniChk.bMainUpdate = true;

		}

		strTempValue = FwIniFile->ReadString("TARGET","AmcUpdate"	,"N");
		if(strTempValue.UpperCase() != "N")
		{
			m_UpdateIniChk.bAmcUpdate = true;

		}
	//AOHC-154
		strTempValue = FwIniFile->ReadString("TARGET","AmcDLLUpdate"	,"N");     //신규
		if(strTempValue.UpperCase() != "N")
		{
			m_UpdateIniChk.bAmcDLLUpdate = true;
		}

		strTempValue = FwIniFile->ReadString("TARGET","AsUpdate"	,"N");
		if(strTempValue.UpperCase() != "N")
		{
			m_UpdateIniChk.bAsUpdate = true;
		}

		strTempValue = FwIniFile->ReadString("TARGET","FmUpdate"	,"N");
		if(strTempValue.UpperCase() != "N")
		{
			m_UpdateIniChk.bFmUpdate = true;
		}

		strTempValue = FwIniFile->ReadString("TARGET","ParamUpdate" ,"N");
		if(strTempValue.UpperCase() != "N")
		{
			m_UpdateIniChk.bPARAMUPATE = true;
		}

		strTempValue = FwIniFile->ReadString("TARGET","WirelessConfig" ,"N");
		if(strTempValue.UpperCase() != "N")
		{
			m_UpdateIniChk.bWirelessConfig  = true;
		}

		strTempValue = FwIniFile->ReadString("TARGET","WirelessMacfil" ,"N");
		if(strTempValue.UpperCase() != "N")
		{
			m_UpdateIniChk.bWirelessMacfil = true;
		}

		strTempValue = FwIniFile->ReadString("TARGET","WirelessFirmware" ,"N");
		if(strTempValue.UpperCase() != "N")
		{
			m_UpdateIniChk.bWirelessFirmware = true;
		}


		delete FwIniFile;
	}
	else
	{
		m_UpdateIniChk.bAsUpdate = 0;
		m_UpdateIniChk.bFmUpdate = 0;
		m_UpdateIniChk.bPARAMUPATE = 0;
		m_UpdateIniChk.bMainUpdate = 0;
		m_UpdateIniChk.bAmcUpdate = 0;
		m_UpdateIniChk.bAmcDLLUpdate = 0;
		m_UpdateIniChk.bWirelessConfig = 0;
		m_UpdateIniChk.bWirelessMacfil = 0;
		m_UpdateIniChk.bWirelessFirmware = 0;

	}
}




