//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FtpClient.h"

#include "FtpClass.h"
#include "Def_OHT.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <locale.h> // 경로명에 한글이 있는 경우를 위해 추가

#include "zip.h"
#include "unzip.h"
#include "Def_DataFile_STD.h"
#include "Def_OHT.h"
#include "Protocol_Define_Modify.h"

#include <Shlwapi.h>

#include "STD_CID.h"
#include "STD_Collision.h"
#include "STD_FlowControl.h"
#include "STD_LookDown.h"
#include "STD_MAP.h"
#include "STD_PathData.h"
#include "STD_Station.h"
#include "STD_Teaching.h"

#include "FileManagerMain.h"

#pragma comment(lib, "Shlwapi.lib")

//---------------------------------------------------------------------------
#pragma package(smart_init)
FtpClient *fFtpClient;
FtpClass *FtpClientClass;

FtpClient::FtpClient()
{
	FtpClientClass = new FtpClass();
	//fFtpClient = new FtpClient();
}

FtpClient::~FtpClient()
{
	delete FtpClientClass;
}

int FtpClient::FTPConnect(PKT_DATA_TRANSPER_ORDER_OCS2OHT UDP_PKT_Order)
{
	// 접속 인수 예 : Connect("127.0.0.1", 21, "Anonymous", "Pass", true);
	char IPAddr[16]={0};
	int i,j,k;
	int iResult;
 
	memset(&IPAddr[0],'0',16);
	for(j=0, k=0 ; k<4; k++)
	{
		itoa(UDP_PKT_Order.BODY.IpAddress[k],&IPAddr[j],10);
		if(k<3)
		{
			for( i=0; i<16; i++)
			{
				if(IPAddr[i] == 0)
				{
					j = i+1;
					IPAddr[i]='.';
					i = 0xFF;
				}
			}
        }
	}
	for(i=0 ; i<15 ; i++)
	{
		if(UDP_PKT_Order.BODY.UserName[i]==0x20)
		{
			UDP_PKT_Order.BODY.UserName[i]=0;
		}
		if(UDP_PKT_Order.BODY.PassWord[i]==0x20)
		{
			UDP_PKT_Order.BODY.PassWord[i]=0;
		}
	}
	iResult = FtpClientClass->Connect(&IPAddr[0], 21, UDP_PKT_Order.BODY.UserName, UDP_PKT_Order.BODY.PassWord, 1); //FTP port 21
	//iResult = FtpClientClass->Connect(&IPAddr[0], 22, UDP_PKT_Order.BODY.UserName, UDP_PKT_Order.BODY.PassWord, 1); //sFTP port 22
	switch (iResult)
	{
		case CONN_SUCCESS:
			return FTP_NOT_ACCEPT_NO_ERR;
		break;
		case CONN_LOGIN_INFO_FAIL:
			return FTP_NOT_ACCEPT_LOGIN_INFO_ERR;
		break;
		case CONN_SOCKET_ERROR:
			return FTP_NOT_ACCEPT_IP_ADDR_ERR;
		break;
		default:
			return FTP_NOT_ACCEPT_LOGIN_INFO_ERR;
		break;
	}
#if 0
	if(iResult == CONN_SUCCESS)
	{
		ShowMessage(L"연결 OK");
		//Test1->MoveDirectory("/");	// 루트의 정보를 보기 위해서
		ShowContent();
	}
	else if(iResult == CONN_ALREADY)
		ShowMessage(L"이미 연결되어 있습니다");
	else if(iResult == CONN_UNABLE)
		ShowMessage(L"접속 불가");
	else
		ShowMessage(L"알수 없는 이유로 접속 실패");
#endif
}


int FtpClient::FTPConnect(AnsiString strHost, AnsiString UnserName, AnsiString strPassword)
{

	int iResult = FtpClientClass->Connect(strHost, 21, UnserName, strPassword, 1);

	switch (iResult)
	{
		case CONN_SUCCESS:
			return FTP_NOT_ACCEPT_NO_ERR;
		break;
		case CONN_LOGIN_INFO_FAIL:
			return FTP_NOT_ACCEPT_LOGIN_INFO_ERR;
		break;
		case CONN_SOCKET_ERROR:
			return FTP_NOT_ACCEPT_IP_ADDR_ERR;
		break;
		default:
			return FTP_NOT_ACCEPT_LOGIN_INFO_ERR;
		break;
	}

}

//---------------------------------------------------------------------------

void FtpClient::FTPDisConnect(void)
{
	FtpClientClass->DisConnect();
	//ShowMessage("Disconnected!");
}
//---------------------------------------------------------------------------
UINT FtpClient::FTPDownload(char* pcFTPPath, unsigned char CMDType)
{
	AnsiString strLocal;
	AnsiString strServer;
	AnsiString strLog;

	char LocalFileName[260];
	UINT iResult=FTP_NOT_ACCEPT_FILE_ERR;
	int iRet;
	bool bNeedDown = false;
	
	if(CMDType == 0)
	{
		iResult = FTP_NOT_ACCEPT_NO_SUPPORT_DATA;
		return iResult;
	}

	// -----------------------------------------
	AnsiString Dir = "C:\\User\\OHT\\DataFiles\\Down\\";
	if (!DirectoryExists(Dir))
	{
	  if (ForceDirectories(Dir))
	  	FileManagerForm->Memo1->Lines->Add("[데이터전송지시] 디렉토리가 없어 새로 생성함");		  
	}
    	// 전송지시 시작 전 Down folder clear
	system("del /q C:\\User\\OHT\\DataFiles\\Down\\*.*");
	// -----------------------------------------	
	
	for(int i=0 ; i<7; i++)
	{
		BYTE CMDtemp = 0;
		bNeedDown = false;
		
		CMDtemp = CMDType & (0x01<<i);
		//요구 구분에 해당하는 파일명 대입
		
		if(CMDtemp==0)
		{
			// Not Checked Download file Flag
		}
		else		
		{
			switch (CMDtemp)
			// switch (CMDType)
			{
				case FTP_CMDTYPE_MAPSET_TEACHING:
				case FTP_CMDTYPE_MAPSET_OPERATING:
					strLocal = "C:\\User\\OHT\\DataFiles\\Down\\MapSet.zip";
					strServer = strAnsiOriginalName[i];
					bNeedDown = true;
					break;
				/*
				case FTP_CMDTYPE_APPLICATION:
					strLocal = "C:\\User\\OHT\\DataFiles\\Down\\OHT.exe";
					strServer = "OHT.exe";
					break;
				*/
				case 0:
				case FTP_CMDTYPE_OS:
				case FTP_CMDTYPE_OS_SETTING:
				case FTP_CMDTYPE_OPERATING_PARA:
					break;

				case FTP_CMDTYPE_APPLICATION:
				case FTP_CMDTYPE_AXIS_PARA:		//  펌웨어 업데이트 관련 수정
					strLocal = "C:\\User\\OHT\\DataFiles\\Down\\FirmwareUpdateSet.zip";
					strServer = strAnsiOriginalName[i];
					bNeedDown = true;
					break;
					
				default:
					iResult = FTP_NOT_ACCEPT_NO_SUPPORT_DATA;
					return iResult;
			}

			if(bNeedDown == true)
			{
				strcpy(&LocalFileName[0],strLocal.c_str());   //PathFileExistsA가 AnsiString 지원 안해서 복사 후 사용.

				//다운받을 곳에 파일이 존재하면 오류발생
				//파일 삭제 후 다운로드
				if(PathFileExistsA(LocalFileName))
				{
					remove(LocalFileName);
				}

				iRet = FtpClientClass->Download(strServer, strLocal);
				if(iRet == NO_ERR )
				{
					iResult = FTP_NOT_ACCEPT_NO_ERR;
				}
				else
				{
					iResult = FTP_NOT_ACCEPT_FILE_ERR;
				}

				strLog.sprintf("다운로드하는 파일명(서버) : %s  받는 파일명 : %s 실행결과(0=success) : %d",
								strServer, strLocal, iResult);
				FileManagerForm->Memo1->Lines->Add(strLog);
			}
		}
	}
	
	return iResult;
}
//---------------------------------------------------------------------------

UINT FtpClient::FTPUpload(char* pcFTPPath, unsigned char CMDType)
{
	AnsiString strLocal;
	AnsiString strServer;
	UINT iResult=FTP_NOT_ACCEPT_DIRECTORY_ERR;
	int iRet;
	if(CMDType == 0)
	{
		iResult = FTP_NOT_ACCEPT_NO_SUPPORT_DATA;
		return iResult;
	}
	for(int i=0 ; i<2; i++)
	{
		BYTE CMDtemp = 0;
		CMDtemp = CMDType & (0x01<<i);
		//요구 구분에 해당하는 파일명 대입
		if(CMDtemp==0)
		{
			// Not Checked Download file Flag
		}
		else
		{
			switch (CMDtemp)
			{
				case FTP_CMDTYPE_LOGDATA:
					strLocal = "C:\\User\\OHT\\DataFiles\\MAPSET\\TaskControl.log";
					strServer = "TaskControl.log";
					break;
				case FTP_CMDTYPE_MAPSET:
					strLocal = "C:\\User\\OHT\\DataFiles\\MAPSET\\MapSet.zip";
					strServer = "MapSet.zip";
					FileManagerForm->Memo1->Lines->Add("MAPSET UPLOAD");
					break;
				case 0:
					break;
				default:
					iResult = FTP_NOT_ACCEPT_NO_SUPPORT_DATA;
					return iResult;
			}
			if( (CMDType&0x02) == FTP_CMDTYPE_MAPSET)
			{
				FileManagerForm->Memo1->Lines->Add("Compression...");
				Compression();
			}
	
			iRet = FtpClientClass->Upload(strLocal,strServer);
			if( iRet == NO_ERR )
			{
				iResult = FTP_NOT_ACCEPT_NO_ERR;
			}
			else
			{
				iResult = FTP_NOT_ACCEPT_DIRECTORY_ERR;
			}
		}
	}
	return iResult;
}
UINT FtpClient::FTPPathCheck(char* pcFTPPath)
{
	AnsiString strTmp;
	UINT iResult=0;
	int i, j;
	int iRet;

	//문자열을 수신받아 FTP 경로 변환
	char strFilePath[300]={0};
	strFilePath[0]='.';
	for(i=0, j=1 ; i<260 ; i++, j++)
	{
		if(pcFTPPath[i]==0x20)
		{
			pcFTPPath[i]=0;
			strFilePath[j]=pcFTPPath[i];
		}
		else if(pcFTPPath[i]==0x2F)// "\" 찾기
		{
			strFilePath[j]=pcFTPPath[i];
			j+=1;
			strFilePath[j]=pcFTPPath[i];
		}
		else
		{
			strFilePath[j]=pcFTPPath[i];
		}
	}
    iRet = FtpClientClass->MoveDirectory(strFilePath);

	if( iRet == NO_ERR )
	{
		iResult = FTP_NOT_ACCEPT_NO_ERR;
	}
	else
	{
        iResult = FTP_NOT_ACCEPT_DIRECTORY_ERR;
	}
	return iResult;
}


UINT FtpClient::FTPFileCheck(char* pcFTPPath, unsigned char CMDType)
{
	char FTPtext[260];
	TStrings *TSfileList;
	TStrings *TSDirList;
	UnicodeString strUnicode;
	UnicodeString strUnicodeFile;

	UnicodeString strServerFileNameBackup;

	// case check add	
	AnsiString strLog1, strLog2, strLog3;
	
	TSfileList = new TStringList();
	TSDirList = new TStringList();
	UINT iResult = FTP_NOT_ACCEPT_FILE_ERR;
	UINT uiNameLength = 0;

	bool bNeedFileChk = false;

	for(int i=0 ; i<7; i++)
	{
		strAnsiOriginalName[i] = "";
	}

	for(int i=0 ; i<7; i++)
	{
		BYTE CMDtemp = 0;
		CMDtemp = CMDType & (0x01<<i);     // <- 업데이트 비트가 동시에 온다면 이렇게 해야 할 것임

		bNeedFileChk = false;
		
		//요구 구분에 해당하는 파일명 대입
		switch (CMDtemp)		
		{
			case FTP_CMDTYPE_MAPSET_TEACHING:
			case FTP_CMDTYPE_MAPSET_OPERATING:
			FileManagerForm->Memo1->Lines->Add("MapSet.zip Check....");
				strUnicodeFile=L"MAPSET.ZIP";
				uiNameLength = 20;
				bNeedFileChk = true;
				break;
		/*
		case FTP_CMDTYPE_APPLICATION:
			strUnicodeFile=L"OHT.exe";
			break;
		*/
			case 0:
			case FTP_CMDTYPE_OS:
			case FTP_CMDTYPE_OS_SETTING:
			case FTP_CMDTYPE_OPERATING_PARA:
				break;

			case FTP_CMDTYPE_APPLICATION:
			case FTP_CMDTYPE_AXIS_PARA:		// shkim.  펌웨어 업데이트 관련 수정
				strUnicodeFile=L"FIRMWAREUPDATESET.ZIP";
				uiNameLength = 42;
				bNeedFileChk = true;				
				break;
			default:
				iResult = FTP_NOT_ACCEPT_NO_SUPPORT_DATA;
				return iResult;
		}


		// ------------- 파일 리스트에서 파일명 비교 --------------
		// shkim. 대소문자가 다르기만 해도 파일을 못 찾는다고 처리되는데 이를 수정
		// strUnicodeFile = strUnicodeFile.UpperCase();	// 대문자로 일괄 변환 (받아올 파일명)
		if(bNeedFileChk == true)
		{
			FtpClientClass->GetFileList(TSfileList, TSDirList);
			int iFileCheck = 0;
			for(int Count = 0; Count < TSfileList->Count; Count++)
			{
				strUnicode = TSfileList->Strings[Count];

				strServerFileNameBackup = strUnicode;
				// strAnsiOriginalName[i] = strUnicode;		// Original Name Backup
				strUnicode = strUnicode.UpperCase(); 	// 대문자로 일괄 변환 (ftp서버에서 받은 파일명)
				memset(&FTPtext[0],0,260);

				// ------------------------------------------------
				strLog2 = strUnicodeFile;
				strLog3 = strUnicode;

				/*
				strLog1.sprintf("OCS에서 다운 지시한 파일명(대문자) : %s  받으려는 파일(대문자) : %s  FTP서버 파일(실제) :%s",
								strLog2, strLog3, strAnsiOriginalName[i]);
				FileManagerForm->Memo1->Lines->Add(strLog1);
				*/

				memcpy(&FTPtext[0],(char*)strUnicode.t_str(),260);
				//int tmp1 = strlen(strUnicodeFile);
				int tmp2 = sizeof(strUnicodeFile);
    			int testi = memcmp((char*)strUnicode.t_str(),(char*)strUnicodeFile.t_str(),uiNameLength); // 비교

				if(!testi)	// if found...
				{
					iFileCheck = 1;
					Count = TSfileList->Count;
					iResult = FTP_NOT_ACCEPT_NO_ERR;

					FileManagerForm->Memo1->Lines->Add("다운로드할 파일 발견됨");

					strAnsiOriginalName[i] = strServerFileNameBackup;

					strLog1.sprintf("OCS에서 다운 지시한 파일명(대문자) : %s  받으려는 파일(대문자) : %s  FTP서버 파일(실제) :%s",
					strLog2, strLog3, strAnsiOriginalName[i]);
					FileManagerForm->Memo1->Lines->Add(strLog1);
				}
			}
		}
		
	}

	// for debug
	for(int i=0 ; i<7; i++)
	{
		strLog1.sprintf("FileList[%d] = %s",i,strAnsiOriginalName[i]);
		FileManagerForm->Memo1->Lines->Add(strLog1);
	}
	
	delete TSfileList;
	delete TSDirList;
    
	return iResult;
}

void FtpClient::FTPClientWorkEnd(void)
{
	// 여기에 전송 완료 등이 들어가야 한다
	//ShowMessage(L"Work Ok");
}
//---------------------------------------------------------------------------

UINT FtpClient::Compression(void)
{
	HZIP hz; 
	UINT iReturn=0;

	hz = CreateZip(_T("C:\\User\\OHT\\DataFiles\\MAPSET\\MapSet.zip"),0);
	
	ZipAdd(hz,_T("CID.dat"), _T("C:\\User\\OHT\\DataFiles\\MAPSET\\CID.DAT"));
	ZipAdd(hz,_T("COLLISION.dat"), _T("C:\\User\\OHT\\DataFiles\\MAPSET\\COLLISION.DAT"));
	ZipAdd(hz,_T("FLOWCONTROL.dat"), _T("C:\\User\\OHT\\DataFiles\\MAPSET\\FLOWCONTROL.DAT"));
	// deleted ZipAdd(hz,_T("LOOKDOWN.dat"), _T("C:\\User\\OHT\\DataFiles\\MAPSET\\LOOKDOWN.dat"));
	ZipAdd(hz,_T("MAPDATA.dat"), _T("C:\\User\\OHT\\DataFiles\\MAPSET\\MAPDATA.DAT"));
	ZipAdd(hz,_T("STATION.dat"), _T("C:\\User\\OHT\\DataFiles\\MAPSET\\STATION.DAT"));
	ZipAdd(hz,_T("TEACHING.dat"), _T("C:\\User\\OHT\\DataFiles\\MAPSET\\TEACHING.DAT"));
	ZipAdd(hz,_T("PATHDATA.dat"), _T("C:\\User\\OHT\\DataFiles\\MAPSET\\PATHDATA.DAT"));

	// 2017.01.05 added
	ZipAdd(hz,_T("MapMakerFile.dat"), _T("C:\\User\\OHT\\DataFiles\\MAPSET\\MAPMAKERFILE.DAT"));
	ZipAdd(hz,_T("JobOrder.dat"),     _T("C:\\User\\OHT\\DataFiles\\MAPSET\\JOBORDER.DAT"));

	CloseZip(hz);
	return iReturn;
}
//---------------------------------------------------------------------------

UINT FtpClient::Decompression(void)
{
	HZIP hz;
	UINT iReturn=0;	
	
	hz = OpenZip(_T("C:\\User\\OHT\\DataFiles\\Down\\MapSet.zip"),0);
	SetUnzipBaseDir(hz,_T("C:\\User\\OHT\\DataFiles\\Down\\"));
	ZIPENTRY ze; 
	GetZipItem(hz,-1,&ze); 
	int numitems=ze.index;
	for (int zi=0; zi<numitems; zi++)
	{ 
		GetZipItem(hz,zi,&ze);
		UnzipItem(hz,zi,ze.name);
	}
	CloseZip(hz);
	return iReturn;
}
//---------------------------------------------------------------------------

UINT FtpClient::FirmwareUpdateDecompress(void)
{
	HZIP hz;
	UINT iReturn=0;	
	
	hz = OpenZip(_T("C:\\User\\OHT\\DataFiles\\Down\\FirmwareUpdateSet.zip"),0);
	SetUnzipBaseDir(hz,_T("C:\\User\\OHT\\DataFiles\\Down\\"));
	ZIPENTRY ze; 
	GetZipItem(hz,-1,&ze); 
	int numitems=ze.index;
	for (int zi=0; zi<numitems; zi++)
	{ 
		GetZipItem(hz,zi,&ze);
		UnzipItem(hz,zi,ze.name);
	}
	CloseZip(hz);
	return iReturn;
}



//---------------------------------------------------------------------------


//다운로드 받은 폴더에 압축해제한 파일들을 삭제함.
UINT FtpClient::EraseMapsetFiles()
{
	AnsiString STR_OHT_FOLDER_PATH = "C:\\User\\OHT\\";
	AnsiString strFile[TOTAL_MAPSET_FILES];	
#if(OHT_NAME == OHT_NAME_P4_MASK)
	strFile[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_COLLISION;
	strFile[1] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile[2] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_MAP;
	strFile[3] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_STATION;
	strFile[4] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_TEACHING;
	strFile[5] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_PATHDATA;
#else
	strFile[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_CID;
	strFile[1] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_COLLISION;
	strFile[2] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile[3] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_MAP;
	strFile[4] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_STATION;
	strFile[5] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_TEACHING;
	strFile[6] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_PATHDATA;
#endif

	UINT iResult=FTP_NOT_ACCEPT_FILE_ERR;

	for(int i=0 ; i<TOTAL_MAPSET_FILES ; i++)
	{
		if(PathFileExistsA(strFile[i].c_str()))
		{
			remove(strFile[i].c_str());
		}
	}

	// add
	strFile[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_JOBORDER;
	strFile[1] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_MAPMAKERFILE;

	for(int i=0 ; i<2 ; i++)
	{
		if(PathFileExistsA(strFile[i].c_str()))
		{
			remove(strFile[i].c_str());
		}
	}

}

UINT FtpClient::FindMapsetFiles()
{
//	AnsiString strOHTPath = ExtractFileDir(Application->ExeName);  // 테스트시 ASSISTANT 실행파일 경로 절대경로로 표시해야함
	AnsiString STR_OHT_FOLDER_PATH = "C:\\User\\OHT\\";
	AnsiString strFile[TOTAL_MAPSET_FILES];
#if(OHT_NAME == OHT_NAME_P4_MASK)
	strFile[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_COLLISION;
	strFile[1] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile[2] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_MAP;
	strFile[3] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_STATION;
	strFile[4] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_TEACHING;
	strFile[5] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_PATHDATA;
#else
	strFile[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_CID;
	strFile[1] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_COLLISION;
	strFile[2] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile[3] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_MAP;
	strFile[4] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_STATION;
	strFile[5] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_TEACHING;
	strFile[6] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_PATHDATA;
#endif

	UINT iResult=0;

	for(int i=0 ; i<TOTAL_MAPSET_FILES ; i++)
	{
		if(PathFileExistsA(strFile[i].c_str()))
		{	iResult+=1;	}
	}
	
	//Mapset 중 찾은 파일 개수 반환
	return iResult;
}

UINT FtpClient::CopyMapsetFiles()
{
	int Error = 0;
	AnsiString STR_OHT_FOLDER_PATH = "C:\\User\\OHT\\";
	AnsiString strFile[TOTAL_MAPSET_FILES];
	AnsiString strFile_DST[TOTAL_MAPSET_FILES];
#if(OHT_NAME == OHT_NAME_P4_MASK)
	strFile[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_COLLISION;
	strFile[1] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile[2] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_MAP;
	strFile[3] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_STATION;
	strFile[4] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_TEACHING;
	strFile[5] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_PATHDATA;

	strFile_DST[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_COLLISION;
	strFile_DST[1] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile_DST[2] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MAP;
	strFile_DST[3] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_STATION;
	strFile_DST[4] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_TEACHING;
	strFile_DST[5] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_PATHDATA;
#else
	strFile[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_CID;
	strFile[1] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_COLLISION;
	strFile[2] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile[3] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_MAP;
	strFile[4] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_STATION;
	strFile[5] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_TEACHING;
	strFile[6] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_PATHDATA;

	strFile_DST[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_CID;
	strFile_DST[1] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_COLLISION;
	strFile_DST[2] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile_DST[3] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MAP;
	strFile_DST[4] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_STATION;
	strFile_DST[5] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_TEACHING;
	strFile_DST[6] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_PATHDATA;
#endif

	char LocalFileName[260];
	char LocalFileName2[260];
	UINT iResult=FTP_NOT_ACCEPT_FILE_ERR;
	UINT ErrorCount=0;

	for(int i=0 ; i<TOTAL_MAPSET_FILES ; i++)
	{
		if(PathFileExistsA(strFile[i].c_str()))
		{
			//Error = CopyFileA(LocalFileName, LocalFileName2, false);
			Error = CopyFileA(strFile[i].c_str(), strFile_DST[i].c_str(), false);
			if(Error == 1)
			{
				//복사 성공
			}
			else
			{
	        	ErrorCount +=1;
			}
		}
	}

	// file 2 add. temp
	strFile[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + "JOBORDER.DAT";
	strFile_DST[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + "JOBORDER.DAT";
	CopyFileA(strFile[0].c_str(), strFile_DST[0].c_str(), false);

	strFile[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + "MAPMAKERFILE.DAT";
	strFile_DST[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_UPDATE_PATH) + "MAPMAKERFILE.DAT";
	CopyFileA(strFile[0].c_str(), strFile_DST[0].c_str(), false);

	
	if( ErrorCount ==0 )
	{
		return 0;//정상
	}
	else
	{
		return 1;//오류
	}

}

UINT FtpClient::CheckFileSizeMapset()
{
	String STR_OHT_FOLDER_PATH = "C:\\User\\OHT\\";
	String strFile[TOTAL_MAPSET_FILES];
	char LocalFileName[260];
#if(OHT_NAME == OHT_NAME_P4_MASK)
	strFile[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_COLLISION;
	strFile[1] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile[2] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_MAP;
	strFile[3] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_STATION;
	strFile[4] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_TEACHING;
	strFile[5] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_PATHDATA;

	char StructureSize[TOTAL_MAPSET_FILES]={
						  /* sizeof(NODE_INFO_STD_CID),*/
						   sizeof(NODE_INFO_STD_COLLISION),
						   sizeof(NODE_INFO_STD_FLOWCONTROL),
						   sizeof(NODE_INFO_STD_MAP),
						   sizeof(NODE_INFO_STD_STATION),
						   sizeof(NODE_INFO_STD_TEACHING),
						   sizeof(NODE_INFO_STD_PATHDATA)};
#else
	strFile[0] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_CID;
	strFile[1] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_COLLISION;
	strFile[2] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile[3] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_MAP;
	strFile[4] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_STATION;
	strFile[5] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_TEACHING;
	strFile[6] = String(STR_OHT_FOLDER_PATH + STR_DATAFILE_Down_PATH) + STR_FILENAME_PATHDATA;
		char StructureSize[TOTAL_MAPSET_FILES]={
						   sizeof(NODE_INFO_STD_CID),
						   sizeof(NODE_INFO_STD_COLLISION),
						   sizeof(NODE_INFO_STD_FLOWCONTROL),
						   sizeof(NODE_INFO_STD_MAP),
						   sizeof(NODE_INFO_STD_STATION),
						   sizeof(NODE_INFO_STD_TEACHING),
						   sizeof(NODE_INFO_STD_PATHDATA)};
#endif

	UINT iResult=0;

    setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

    int ErrorCount = 0;
	int iRet = 0;

	// JOBORDER.dat 와 MAPMAKERFILE.dat 은 있을수도 없을수도 있다
	// 따라서 파일 체크에서는 빼자 (-2)
	for(int i ; i < TOTAL_MAPSET_FILES ; i++)
	{
		iRet=CheckFileSize(strFile[i].c_str(), StructureSize[i]);
		if(iRet != NO_ERR)
		{
			ErrorCount += 1; //Error count 증가
		}
	}

	//Error count 가 0 이상이면 오류 발생.
	if(ErrorCount>0)
	{
		iResult = 1;//파일 중 사이즈 이상이 있는 파일이 있음.
	}
	else
	{
		iResult = NO_ERR;//파일 사이즈 이상 없음.
	}

    
	return iResult;
}

int FtpClient::CheckFileSize(TCHAR *FileName, UINT structsize)
//int MapDataManager_STD::LoadMapFile(TCHAR *FileName)
{
    setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

    int Error = NO_ERR;

    ifstream fs;

    // Step 1. File Open
    fs.open(FileName, ios::in | ios::binary);

    if(fs.is_open())
    {
        int NodeSize = 0; // Node Data 총 size

        // Step 2. Read File Header Information
		FILE_HEADER_STD FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));

		fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));

        // Step 3. Header 파싱  --> 현재 파싱 필요없음.
		ConvertEndian(&FileHeader.ModelID);
		ConvertEndian(&FileHeader.Version);
		ConvertEndian(&FileHeader.DataCount);
		ConvertEndian(&FileHeader.DataType);
		
		fs.seekg(0,fstream::end);
		UINT file_size = fs.tellg();
		fs.seekg(0);

		if( (FileHeader.DataCount > 0) && (FileHeader.DataCount < NODE_COUNT_MAX) )
		{
			UINT tmpsize =(FileHeader.DataCount * structsize) + 96;
			if( tmpsize == file_size )
			{
				Error = NO_ERR;
			}
			else
			{
                Error = -1;
			}
		}
		else
		{
			Error = ERR_MAP_FILEOPENFAIL;
		}
	}
	else
	{
		Error = ERR_MAP_FILEOPENFAIL;
	}

    fs.close();
    return Error;
}

int FtpClient::DirectCommand(AnsiString strCommand)
{
	int iResult = FtpClientClass->DirectCommand(strCommand);

	return iResult;
}

// strLocal = "C:\\User\\OHT\\DataFiles\\MAPSET\\MAPSET.zip";
int FtpClient::DirectUpload(AnsiString strLocalFileName, AnsiString strServerFileName)
{
	int iRet;
	iRet = FtpClientClass->Upload(strLocalFileName,strServerFileName);

	return iRet;
}


