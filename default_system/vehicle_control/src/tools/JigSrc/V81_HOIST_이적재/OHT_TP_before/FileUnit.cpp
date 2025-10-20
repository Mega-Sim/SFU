// ---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <dos.h>
#include <Filectrl.hpp>
#include <ShellApi.h>
#pragma hdrstop

#include "FileUnit.h"
#include "MainUnit.h"
#include "SocketUnit.h"
#include "ProcessUnit.h"
#include "Define.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "WaitUnit.h"
#include "LogDownUnit.h"
#include "OHTInfoUnit.h"
#include "FoupOnlineCMDUnit.h"
//#include "ReticleOnlineCMDUnit.h"
#include "EUVOnlineCMDUnit.h"
//#include "ProbeOnlineCMDUnit.h"
//#include "ProbeJigOnlineCMDUnit.h"
#include "ChkExecUnit.h"
#include "LogFileUnit.h"
#include "FileListUnit.h"
#include "TeachingData.h"

// by JYH 111027 : log를 위해 header 파일 추가
#include "Utility.h"
#include "FileUploadAlarm.h"
#include "Def_TP.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFileForm *FileForm;
TCHKOHT FileOHTCheck;

// ---------------------------------------------------------------------------
DWORD CALLBACK CopyProgressRoutine(LARGE_INTEGER TotalFileSize, // total file size, in bytes
    LARGE_INTEGER TotalBytesTransferred, // total number of bytes transferred
    LARGE_INTEGER StreamSize, // total number of bytes for this stream
    LARGE_INTEGER StreamBytesTransferred, // total number of bytes transferred for
    DWORD dwStreamNumber, // the current stream
    DWORD dwCallbackReason, // reason for callback
    HANDLE hSourceFile, // handle to the source file
    HANDLE hDestinationFile, // handle to the destination file
    LPVOID lpData // passed by CopyFileEx
    )
{
    int newpos;
    if((int)TotalFileSize.QuadPart == 0)
    {
        ProcessForm->Label1->Caption = "0";
        newpos = 0;

    }
    else
    {
        ProcessForm->Label1->Caption = Format("[%d] TotalFileSize: %d, Transffered: %d",
            OPENARRAY(TVarRec, ((int)(TotalBytesTransferred.QuadPart * 100 / TotalFileSize.QuadPart), (int)TotalFileSize.QuadPart,
            (int)TotalBytesTransferred.QuadPart)));

        newpos = (int)(TotalBytesTransferred.QuadPart * 100 / TotalFileSize.QuadPart);
    }

    ProcessForm->CGauge1->Progress = newpos;
    Application->ProcessMessages();
    return PROGRESS_CONTINUE;
}

// ---------------------------------------------------------------------------
__fastcall TFileForm::TFileForm(TComponent *Owner) : TForm(Owner)
{
    bFileCancel = false;

    // by zzang9un 2013.09.13 : 아직 미구현된 UI를 Disable 처리
    chkNodeDisableFile->Enabled = false;
    chkNodeDisableFile->Visible = false;
    tabAMCUpdate->Enabled = false;
    tabAMCUpdate->TabVisible = false;
}

// ---------------------------------------------------------------------------
void __fastcall TFileForm::comboFileSelectChange(TObject *Sender)
{
	String strLocalName = gm_OHTStatus->strNetworkDrive;

	switch(comboFileSelect->ItemIndex)
    {
    case 0: // upload
		pgcFileControl->Enabled = true;
		pgcFileControl->ActivePage = tabUpload;
		tabUpload->TabVisible = true;
		tabDownload->TabVisible = false;
		tabDelete->TabVisible = false;
		tabFileList->TabVisible = false;

		// 2016.12.29. 조작하는 패널이 보이도록 함
		pgcFileControl->ActivePageIndex = 1;

		break;

    case 1: // download
		pgcFileControl->Enabled = true;
        tabUpload->TabVisible = false;
		tabDownload->TabVisible = true;
		tabDelete->TabVisible = false;
		tabFileList->TabVisible = false;

		// 2016.12.29. 조작하는 패널이 보이도록 함
		pgcFileControl->ActivePageIndex = 0;
        break;
        
    case 2: // explore
        ShellExecute(0, L"explore", strLocalName.w_str(), NULL, NULL, SW_SHOWNORMAL);
        break;
        
    case 3: // delete
        pgcFileControl->Enabled = true;
        tabDownload->TabVisible = false;
        tabUpload->TabVisible = false;
        tabFileList->TabVisible = false;
		tabDelete->TabVisible = true;

		// 2016.12.29. 조작하는 패널이 보이도록 함
		pgcFileControl->ActivePageIndex = 2;
        break;
        
    case 4: // file list
        if(gm_OHTStatus->g_nIsOHTBoot == FROMCFC)
            return;
        pgcFileControl->Enabled = true;
        tabDownload->TabVisible = false;
        tabUpload->TabVisible = false;
        tabDelete->TabVisible = false;
		tabFileList->TabVisible = true;

		// 2016.12.29. 조작하는 패널이 보이도록 함
		pgcFileControl->ActivePageIndex = 4;
        break;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TFileForm::btnDownCancelClick(TObject *Sender)
{
    bFileCancel = true;
}
// ---------------------------------------------------------------------------

/**
@brief   Upload 버튼 클릭 시 호출되는 이벤트 함수
         - 선택한 Data의 파일을 업로드 한다.
@author  zzang9un
@date    2013.09.13
@note    Map Data               : 0
         Station Data           : 1
         Teaching Data          : 2
         Parameter(Driving) Data: 3
         Parameter(Trans) Data  : 4
         AMC Program            : 5
@warning Parameter, AMC 파일은 추후 적용함         
*/
void __fastcall TFileForm::btnUploadClick(TObject *Sender)
{
    if(FileOHTCheck.Is_Executable() == NO_ERR)
    {
        switch(rgSelectUploadFile->ItemIndex)
        {
        case 0: // Map Data
            UploadFile(DATAFILE_MAP, FILE_EXT_MAP);
            break;
            
        case 1: // Station Data
            // UploadFile(DATAFILE_STATION, FILE_EXT_STATION);
            UploadFile(DATAFILE_STATION, STR_FILENAME_STATION);
            break;
            
        case 2: // Teaching Data
            // UploadFile(DATAFILE_TEACHING, FILE_EXT_TEACHING);
            UploadFile(DATAFILE_TEACHING, STR_FILENAME_TEACHING);
            break;

		case 3: // AMC Program
			// UploadFile(DATAFILE_AMCOUTFILE, FILE_EXT_AMCOUT);
			UploadFile(DATAFILE_AMCOUTFILE, FILENAME_AMCOUT);
			break;
			
		/* // [BEGIN] by zzang9un 2013.09.13 : 아직 미적용
		case 3: // Parameter(Driving) Data
			UploadFile(DATAFILE_PARAM_DRIVING, FILE_EXT_PARAMETER);
			break;

		case 4: // Parameter(Trans) Data
			UploadFile(DATAFILE_PARAM_TRANS, FILE_EXT_PARAMETER);
			break;


        */ // [END] by zzang9un 2013.09.13 : 아직 미적용
        }
    }
}
// ---------------------------------------------------------------------------

/**
@brief   TP로 OHT에 파일을 업로드 하는 함수
@author  zzang9un
@date    2013.09.13
@param   DataType : Data 종류
@param   FileName : Upload할 파일 이름
*/
void TFileForm::UploadFile(int DataType, String FileName)
{
    String TmpFilter, TmpName, TmpNewName, LocalName, TmpNewBackup;
    String strPMACDown_FileName[10];
    static String strTmpDir;
    String strNetworkDrive = gm_OHTStatus->strNetworkDrive;
    String strLineString;
	// int nPos = 0;

    // by JYH 111027
    int nMsgboxID; // file upload 시 messagebox return value

    TmpFilter = "Upload File(" + FileName + ")|" + FileName;
    dlgOpenFile->Filter = TmpFilter;
    dlgOpenFile->FileName = "Select Upload File";
    dlgOpenFile->InitialDir = strTmpDir;
    bFileCancel = false;
    
    if(dlgOpenFile->Execute())
    {
		MainForm->DataDisplay(COLOR_BLACK, "======= Upload File List =======");
        ProcessForm->Visible = true;

        for(int i = 0; i < dlgOpenFile->Files->Count; i++)
        {
            TmpName = dlgOpenFile->Files->Strings[i]; // Multi Select...set the ofAllowMultiSelect flag in Options
            strTmpDir = ExtractFilePath(TmpName);

            switch(DataType)
            {
            case DATAFILE_MAP:
                // TmpNewName = strNetworkDrive + STR_PATH_UPDATE + FILENAME_MAP;
                TmpNewName = strNetworkDrive + STR_PATH_UPDATE + ExtractFileName(TmpName);
                break;

            case DATAFILE_STATION:
                // TmpNewName = strNetworkDrive + STR_PATH_UPDATE + FILENAME_STATION;
                TmpNewName = strNetworkDrive + STR_PATH_UPDATE + STR_FILENAME_STATION;
                break;

            case DATAFILE_TEACHING:
                // TmpNewName = strNetworkDrive + STR_PATH_UPDATE + FILENAME_TEACHING;
                TmpNewName = strNetworkDrive + STR_PATH_UPDATE + STR_FILENAME_TEACHING;
				break;

			case DATAFILE_AMCOUTFILE:
				TmpNewName = strNetworkDrive + STR_PATH_DATAFILES + FILENAME_AMCOUT;
				break;

            /* // [BEGIN] by zzang9un 2013.09.13 : 아래 Data는 아직 미적용            
            case DATAFILE_PARAM_DRIVING:
                TmpNewName = strNetworkDrive + STR_PATH_DATAUPDATE + FILENAME_PARAM_DRIVING;
                break;

            case DATAFILE_PARAM_TRANS:
                TmpNewName = strNetworkDrive + STR_PATH_DATAUPDATE + FILENAME_PARAM_TRANS;
                break;  


            */ // [END] by zzang9un 2013.09.13 : 아래 Data는 아직 미적용

            case DATAFILE_USERSELECT:
                TmpNewName = strNetworkDrive + STR_PATH_UPDATE + FileName;
                break;

            case DATAFILE_ASSISTANT:
                TmpNewName = strNetworkDrive + L"\\" + FileName;
                break;    
            }
            
            ProcessForm->lblFrom->Caption = TmpName;
            ProcessForm->lblTo->Caption = TmpNewName;

			if(CopyFileEx(TmpName.w_str(), TmpNewName.w_str(), CopyProgressRoutine, NULL, &bFileCancel, 0))
                // COPY_FILE_FAIL_IF_EXISTS))
                // if(CopyFile(TmpName.w_str(),TmpNewName.w_str(),FALSE)!=0)
                MainForm->DataDisplay(COLOR_BLUE, ExtractFileName(TmpName) + " UPLoad ..... OK");
            else
				MainForm->DataDisplay(COLOR_RED, ExtractFileName(TmpName) + " UPLoad ..... Fail");
            Application->ProcessMessages();
        }
        
        ProcessForm->Visible = false;
        MainForm->DataDisplay(COLOR_BLACK, "============================");

		 // [BEGIN] by zzang9un 2013.09.13 : AMC는 Upload 안되도록 협의함(임태웅 선임)
		if(DataType == DATAFILE_AMCOUTFILE)
		{

			MainForm->DataDisplay(COLOR_BLACK, "--> Wait....AMC Uploading.......");
			pgcFileControl->ActivePage = tabAMCUpdate;
			WaitForm->DisplayMemo->Lines->Add("Wait....AMC Uploading.......");
#if(LANGUAGE_MODE==MODE_KOREAN)
			WaitForm->DisplayMemo->Lines->Add("건들지 말것!!");
#else
			WaitForm->DisplayMemo->Lines->Add("Prohibit the Operation!!");
#endif


#if 0
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_DOWN_AMCFILE);
			delete m_CMDControl;
#else
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

			char buf[MAX_PACKET_SIZE+1];
			int iPacketSize;
			int iReturn, iTemp;
			AnsiString strMachineID = "TP001";

			// 명령 구조체
			iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
			PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;

			// 구조체 초기화
			memset(&SEND_PACKET, 0, iPacketSize);
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
			memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());
			SEND_PACKET.TAG.Value = TagID::CMD_TP_AMC_UPDATE;

			m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

			delete m_CMDControl;
#endif


			MainForm->DataDisplay(COLOR_BLACK, "-------------------------------------------------");
			WaitForm->ShowModal();
		}
		 // [END] by zzang9un 2013.09.13 : AMC는 Upload 안되도록 협의함(임태웅 선임)

    }
}

// ---------------------------------------------------------------------------
void __fastcall TFileForm::btnDeleteClick(TObject *Sender)
{
    String TmpFromDir, TmpTxt, strTmpMessage;
	String strLocalName;	// = gm_OHTStatus->strNetworkDrive;

    bFileCancel = false;

	if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
	{
		strLocalName = gm_OHTStatus->strNetworkDrive;
	    TmpTxt = "*.log";
	    TmpFromDir = strLocalName + STR_PATH_LOG + L"\\" + TmpTxt;
	}
	else
	{
		strLocalName = gm_OHTStatus->strLogworkDrive;
	    TmpTxt = "*.log";
	    TmpFromDir = strLocalName + L"\\" + TmpTxt;
	}
	
    strTmpMessage = "Do you want to (" + TmpTxt + ") Delete?";
	
    if(Application->MessageBox(strTmpMessage.w_str(), L"Confirm", MB_OKCANCEL) == IDOK)
    {
        MainForm->DataDisplay(COLOR_BLACK, "======= Delete File List =======");
        Delete_OHTFile(TmpFromDir);
        MainForm->DataDisplay(COLOR_BLACK, "==============================");
    }
}
// ---------------------------------------------------------------------------

void TFileForm::FileControlInit()
{
    bFileCancel = false;
}

// ---------------------------------------------------------------------------
int TFileForm::FileControlStart(String strRName)
{
    String TmpLocalName, TmpRemoteName, TmpPassWord, TmpID;
    int nError = 0;
    String strLocalName = gm_OHTStatus->strNetworkDrive;

    FileControlInit();
	GetnetworkDrive_NotConnected(strRName);

	// 2016.12.29 add (log drive)
	GetOhtLogDrive_NotConnected(strRName);

	// --------------------------------------------
	//	Step 1. make oht drive 
	// --------------------------------------------
	strLocalName = gm_OHTStatus->strNetworkDrive;
    TmpLocalName = strLocalName;
    TmpRemoteName = "\\\\" + strRName + "\\OHT";

    TmpPassWord = "OHT";
    TmpID = "OHT";

	NetWork_DisConnect(strLocalName);

	nError = NetWork_Connect(TmpLocalName, TmpRemoteName, TmpPassWord, TmpID);


#if 0
	if(nError == 85) 	// 85는 네트워크 드라이브로 이미 연결되어 있는 경우
    {
        nError = 0;
        NetWork_DisConnect(strLocalName);
		// nError = NetWork_Connect(TmpLocalName, TmpRemoteName, TmpPassWord, TmpID);
    }
#endif

	
	if(nError != NO_ERR)
    {
        TmpPassWord = "NO";
        TmpID = "NO";
        nError = NetWork_Connect(TmpLocalName, TmpRemoteName, TmpPassWord, TmpID);
    }


	// --------------------------------------------
	//	Step 2. make log drive 
	// --------------------------------------------
	strLocalName = gm_OHTStatus->strLogworkDrive;
    TmpLocalName = strLocalName;
    TmpRemoteName = "\\\\" + strRName + "\\LOG";

    TmpPassWord = "OHT";
    TmpID = "OHT";

	NetWork_DisConnect(strLocalName);

	nError = NetWork_Connect(TmpLocalName, TmpRemoteName, TmpPassWord, TmpID);

#if 0
	if(nError == 85)
    {
        nError = 0;
		NetWork_DisConnect(strLocalName);
        // nError = NetWork_Connect(TmpLocalName, TmpRemoteName, TmpPassWord, TmpID);
    }
#endif

	if(nError != NO_ERR)
    {
        TmpPassWord = "NO";
        TmpID = "NO";
        nError = NetWork_Connect(TmpLocalName, TmpRemoteName, TmpPassWord, TmpID);
    }


	
    return nError;
}

// ---------------------------------------------------------------------------
void TFileForm::FileControlClose()
{
    NetWork_DisConnect(gm_OHTStatus->strNetworkDrive);
	NetWork_DisConnect(gm_OHTStatus->strLogworkDrive);
}

// ---------------------------------------------------------------------------
int TFileForm::NetWork_Connect(String TmpLocalName, String TmpRemoteName, String TmpPassWord, String TmpID)
{
    /***
     DWORD Result;
     NETRESOURCE NetRes;
     NetRes.dwType = RESOURCETYPE_DISK;
     NetRes.lpLocalName   = TmpLocalName.w_str();
     NetRes.lpRemoteName  = TmpRemoteName.w_str();
     NetRes.lpProvider    = NULL;

     if((TmpPassWord =="NO") || ( TmpID =="NO"))
     Result = WNetAddConnection2(&NetRes,NULL,NULL,CONNECT_UPDATE_PROFILE);
     else
     Result = WNetAddConnection2(&NetRes, TmpPassWord.w_str(), TmpID.w_str(),  CONNECT_UPDATE_PROFILE);
     *** */
    // ** test2 **//
    DWORD Result;
    NETRESOURCE NetRes;
    NetRes.dwScope = RESOURCE_GLOBALNET; // RESOURCE_CONNECTED
    NetRes.dwType = RESOURCETYPE_ANY; // RESOURCETYPE_DISK
    NetRes.dwDisplayType = RESOURCEDISPLAYTYPE_SHARE;
    NetRes.dwUsage = RESOURCEUSAGE_CONNECTABLE; ;
    NetRes.lpLocalName = TmpLocalName.w_str();
    NetRes.lpRemoteName = TmpRemoteName.w_str();
    NetRes.lpComment = NULL;
    NetRes.lpProvider = NULL;

    if((TmpPassWord == "NO") || (TmpID == "NO"))
        Result = WNetAddConnection2(&NetRes, NULL, NULL, CONNECT_UPDATE_PROFILE); // CONNECT_INTERACTIVE
    else
		Result = WNetAddConnection2(&NetRes, TmpPassWord.w_str(), TmpID.w_str(), CONNECT_UPDATE_PROFILE);
    // CONNECT_INTERACTIVE

	switch(Result)
	{
    case ERROR_ACCESS_DENIED:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "Access denied.");
        break;
    case ERROR_ALREADY_ASSIGNED:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "Drive is already connected.");
        break;
    case ERROR_BAD_DEV_TYPE:
		MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "Uncompatible dispositive.");
		break;
    case ERROR_BAD_DEVICE:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "Invalid Letter.");
        break;
    case ERROR_BAD_NET_NAME:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "Server name is not valid or can''t befound");
        break;
    case ERROR_BAD_PROFILE:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "Incorrect Parameters.");
        break;
    case ERROR_CANNOT_OPEN_PROFILE:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "Permanent conection is not allowed");
        break;
    case ERROR_DEVICE_ALREADY_REMEMBERED:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "User entry is already on the user profile.");
        break;
    case ERROR_EXTENDED_ERROR:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "Network error.");
        break;
    case ERROR_INVALID_PASSWORD:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "Wrong Password.");
        break;
    case ERROR_NO_NET_OR_BAD_PATH:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "The operation couldn''t be finished because the network or folder is unvailable.");
        break;
    case ERROR_NO_NETWORK:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Connect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "Network is unvailable.");
        break;
    case NO_ERROR:
        MainForm->DataDisplay(COLOR_BLUE, "Assistant Network Connect : OK");
        // MainForm->DataDisplay(COLOR_BLUE, "Network Drive connect..OK");
        break;
    }
    return Result;
}

// ---------------------------------------------------------------------------
// NetWork 연결 해제
// ---------------------------------------------------------------------------
void TFileForm::NetWork_DisConnect(String TmpLocalName)
{
    DWORD Result = WNetCancelConnection2(TmpLocalName.w_str(), CONNECT_UPDATE_PROFILE, TRUE);
    switch(Result)
    {
    case ERROR_BAD_PROFILE:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Disconnect : ERROR_BAD_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "Assistant Disconnect : ErrThe user profile is in an incorrect format.");
        break;

    case ERROR_CANNOT_OPEN_PROFILE:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Disconnect : ERROR_CANNOT_OPEN_PROFILE");
        // MainForm->DataDisplay(COLOR_RED, "The system is unable to open the user profile to process persistent connections.");
        break;

    case ERROR_DEVICE_IN_USE:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Disconnect : ERROR_DEVICE_IN_USE");
        // MainForm->DataDisplay(COLOR_RED, "The device is in use by an active process and cannot be disconnected.");
        break;

    case ERROR_EXTENDED_ERROR:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Disconnect : ERROR_EXTENDED_ERROR");
        // MainForm->DataDisplay(COLOR_RED, "A network-specific error occurred. To get a description of the error, use the WNetGetLastError function.");
        break;

    case ERROR_NOT_CONNECTED:
        // MainForm->DataDisplay(COLOR_RED, "Assistant Network Disconnect : ERROR_NOT_CONNECTED");
		MainForm->DataDisplay(COLOR_BLUE, "Assistant Network Disconnect : NOT_CONNECTED");
        // MainForm->DataDisplay(COLOR_RED, "The name specified by the lpName parameter is not a redirected device, or the system is not currently connected to the device specified by the parameter.");
        break;

    case ERROR_OPEN_FILES:
        MainForm->DataDisplay(COLOR_RED, "Assistant Network Disconnect : ERROR_OPEN_FILES");
        // MainForm->DataDisplay(COLOR_RED, "There are open files, and the fForce parameter is FALSE.");
        break;

    case NO_ERROR:
        MainForm->DataDisplay(COLOR_BLUE, "Assistant Network Disconnect : OK");
        break;
    }
}
// ---------------------------------------------------------------------------

/**
@brief   OHT로부터 파일을 다운로드 하기 위한 함수
@author  zzang9un
@date    2013.09.13
@param   DataType : Data Type
@param   FileName : 다운로드할 파일 이름
*/
void TFileForm::FileDownStart(int DataType, String FileName)
{
    String TmpFromDir, TmpToDir;
    int TmpIndex;
    String TmpFilter;
    String strNetworkDrive = gm_OHTStatus->strNetworkDrive;

    bFileCancel = false;

    // 파일 이름 설정    
    if ((DataType == DATAFILE_MAP) || (DataType == DATAFILE_STATION) || (DataType == DATAFILE_TEACHING))
		TmpFromDir = strNetworkDrive + STR_PATH_MAPFILES;
	else
    	TmpFromDir = strNetworkDrive + STR_PATH_DATAFILES;
	
    dlgSaveFile->FileName = FileName;
    dlgSaveFile->Title = "Select Directory and File Name";

    if(dlgSaveFile->Execute())
    {
        MainForm->DataDisplay(COLOR_BLACK, "======= Download File List =======");
        
        switch(DataType)
        {
	        case DATAFILE_MAP:
			{
				TmpToDir = ExtractFilePath(dlgSaveFile->FileName);
				
				DownLoadFile(STR_FILENAME_CID, 			TmpFromDir, TmpToDir);
				DownLoadFile(STR_FILENAME_COLLISION, 	TmpFromDir, TmpToDir);
				DownLoadFile(STR_FILENAME_FLOWCONTROL, 	TmpFromDir, TmpToDir);
				DownLoadFile(STR_FILENAME_MAP, 			TmpFromDir, TmpToDir);
				DownLoadFile(STR_FILENAME_PATHDATA, 	TmpFromDir, TmpToDir);
	        }
			break;
			
	        case DATAFILE_STATION:
	        case DATAFILE_TEACHING:
	        case DATAFILE_AMCOUTFILE:
	        case DATAFILE_PARAM_DRIVING:
	        case DATAFILE_PARAM_TRANS:
	        case DATAFILE_NODEDISABLE:    
	            TmpToDir = ExtractFilePath(dlgSaveFile->FileName);
	            DownLoadFile(FileName, TmpFromDir, TmpToDir);
	            break;

	        case DATAFILE_USERSELECT:
	            TmpToDir = ExtractFilePath(dlgSaveFile->FileName);
	            DownLoadFile(FileName, TmpFromDir, TmpToDir);
	            break;
	        }
	        
	        MainForm->DataDisplay(COLOR_BLACK, "=============================");
    }
}

// ---------------------------------------------------------------------------

/**
@brief   OHT의 파일을 다운로드 하는 함수
@author  zzang9un
@date    2013.09.13
@param   FileName : 파일 이름
@param   FromPath : 원본 파일 경로
@param   ToPath : 저장될 파일 경로
*/
void TFileForm::DownLoadFile(String FileName, String FromPath, String ToPath)
{
    TSearchRec sr;
    String TmpName, TmpNewName;
    String strLocalName = gm_OHTStatus->strNetworkDrive;

    TmpName = FromPath + FileName;
    TmpNewName = ToPath + FileName;
                
    ProcessForm->lblFrom->Caption = TmpName;
    ProcessForm->lblTo->Caption = TmpNewName;
    
    if(CopyFileEx(TmpName.w_str(), TmpNewName.w_str(), CopyProgressRoutine, NULL, &bFileCancel, 0))
        // COPY_FILE_FAIL_IF_EXISTS))
        // if(CopyFile(TmpName.w_str(),TmpNewName.w_str(),FALSE)!=0)
        MainForm->DataDisplay(COLOR_BLUE, sr.Name + " DownLoad ..... OK");
    else
        MainForm->DataDisplay(COLOR_RED, sr.Name + " DownLoad ..... Fail");

    Application->ProcessMessages();

    ProcessForm->Visible = false;
}
// ---------------------------------------------------------------------------

void TFileForm::Delete_OHTFile(String FromPath)
{
    TSearchRec sr;
	String TmpName, TmpNewName;
	String strLocalName;	// = gm_OHTStatus->strNetworkDrive;

	if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
		strLocalName = gm_OHTStatus->strNetworkDrive;
	else
		strLocalName = gm_OHTStatus->strLogworkDrive;


    if(FindFirst(FromPath, faArchive | faReadOnly | faSysFile | faHidden, sr) == 0)
    {
        // 디렉토리를 제외하고 파일만을 검색한다
        if(sr.Attr == faArchive || faReadOnly || faSysFile || faHidden)
        {
            // 검색한 내용이 파일이면
            if(!((sr.Name == ".") || (sr.Name == "..")))
            {
            	if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
					TmpName = strLocalName + STR_PATH_LOG + L"\\" + sr.Name;
				else
					TmpName = strLocalName + L"\\" + sr.Name;

				
                if(DeleteFile(TmpName.w_str()) != 0)
                    MainForm->DataDisplay(COLOR_BLUE, sr.Name + " Delete ..... OK");
                else
                    MainForm->DataDisplay(COLOR_BLUE, sr.Name + " Delete ..... Fail");
            }
        }
		while(FindNext(sr) == 0)
        {
            if(sr.Attr == faArchive || faReadOnly || faSysFile || faHidden)
            {
                if((sr.Name == ".") || (sr.Name == ".."))
                    continue;

				if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
                	TmpName = strLocalName + STR_PATH_LOG + L"\\" + sr.Name;
				else
					TmpName = strLocalName + L"\\" + sr.Name;
				
                if(DeleteFile(TmpName.w_str()) != 0)
                    MainForm->DataDisplay(COLOR_BLUE, sr.Name + " Delete ..... OK");
                else
                    MainForm->DataDisplay(COLOR_BLUE, sr.Name + " Delete ..... Fail");
            }
            Application->ProcessMessages();
            if(bFileCancel == true)
            {
                MainForm->DataDisplay(COLOR_BLUE, "DownLoad Canceled...");
                break;
            }
        }
    }
    FindClose(sr);
}


// ---------------------------------------------------------------------------
void TFileForm::GetLST_OHTFile(String strTmpName)
{

    // puting 2012.12.04 network driver enable

    TSearchRec sr;
    String FromPath;
    String strLocalName;   //  = gm_OHTStatus->strNetworkDrive;
    
    if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
    {
		strLocalName = gm_OHTStatus->strNetworkDrive;
		FromPath.sprintf(L"%s\\Log\\%s", strLocalName, strTmpName);
	}
	else
    {
		strLocalName = gm_OHTStatus->strLogworkDrive;
		FromPath.sprintf(L"%s\\%s", strLocalName, strTmpName);
    }
    
    FileListForm->DataClear();
    
	if(FindFirst(FromPath, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
    {
       //디렉토리를 제외하고 파일만을 검색한다
       if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
       {
         //검색한 내용이 파일이면
         if(!((sr.Name == ".") || (sr.Name == "..")))
         {
    		 // FileListForm->DataAdd(sr.Name);
    		 // 2016.12.29 fix
			 TDateTime dt = FileDateToDateTime(sr.Time);
			 FileListForm->DataAdd(sr.Name, FormatDateTime("yyyy-mm-dd hh:mm",dt));
    	 }
       }
	   
    	while(FindNext(sr) == 0)
    	{
    		if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
    		{
    			if((sr.Name == ".") || (sr.Name == "..")) continue;
    			// FileListForm->DataAdd(sr.Name);
				TDateTime dt = FileDateToDateTime(sr.Time);
				FileListForm->DataAdd(sr.Name, FormatDateTime("yyyy-mm-dd hh:mm",dt));
    		}
			
    		Application->ProcessMessages();
    	}
    }

	FindClose(sr);
}

// ---------------------------------------------------------------------------
String TFileForm::ExtractComboString(TComboBox *tmpCombo)
{
    String strTmpString;
    int nFrom, nTo;

    strTmpString = tmpCombo->Items->Strings[tmpCombo->ItemIndex];
    nFrom = strTmpString.Pos("[");
    nTo = strTmpString.Pos("]");
    return strTmpString.SubString(nFrom + 1, nTo - nFrom - 1);
}

// ---------------------------------------------------------------------------
void __fastcall TFileForm::FormShow(TObject *Sender)
{
	FileForm->Top = MainForm->Top + MainForm->Height;
    FileForm->Left = MainForm->Left;
    FileForm->Width = MainForm->Width;
    // SpeedButton1->Left = Panel2->Left+Panel2->Width-SpeedButton1->Width;
    // SpeedButton1->Top = Panel2->Top;

    pgcFileControl->ActivePageIndex = 0;
    pgcFileControl->Enabled = false;
    tabDownload->TabVisible = true;
    tabUpload->TabVisible = true;
    tabDelete->TabVisible = true;
    tabFileList->TabVisible = true;
    comboFileSelect->ItemIndex = -1;
    comboFileSelect->Text = "Select Upload / Download";

    // if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
    if(gm_OHTStatus->strLogSaveMode == "2")
    {
        chkUserSelectFile->Checked = false;
    }
    else
    {
		chkUserSelectFile->Visible = true;
	}

	// 2016.12.29 UI 일관성을 위해 추가
	comboFileSelect->ItemIndex = 0;

	pgcFileControl->Enabled = true;
	pgcFileControl->ActivePage = tabUpload;
	tabUpload->TabVisible = true;
	tabDownload->TabVisible = false;
	tabDelete->TabVisible = false;
	tabFileList->TabVisible = false;

	// 2016.12.29. 조작하는 패널이 보이도록 함
	pgcFileControl->ActivePageIndex = 1;


}
// ---------------------------------------------------------------------------

/**
@brief   DownLoad 버튼 클릭 시 호출되는 이벤트 함수

         - 체크된 Data를 다운로드 한다.
@author  zzang9un
@date    2013.09.13
*/
void __fastcall TFileForm::btnDownloadClick(TObject *Sender)
{
	// Map
    if(chkMapDataFile->Checked)
    {
        FileDownStart(DATAFILE_MAP, FILENAME_MAP);
    }

    // Station Data
    if(chkStationDataFile->Checked)
    {
        // FileDownStart(DATAFILE_STATION, FILENAME_STATION);
        FileDownStart(DATAFILE_STATION, STR_FILENAME_STATION);
    }

    // Teaching Data
    if(chkTeachingDataFile->Checked)
    {
        // FileDownStart(DATAFILE_TEACHING, FILENAME_TEACHING);
        FileDownStart(DATAFILE_TEACHING, STR_FILENAME_TEACHING);
    }

    // Parameter(Driving) Data
    if(chkParamDrivingFile->Checked)
    {
        FileDownStart(DATAFILE_PARAM_DRIVING, FILENAME_PARAM_DRIVING);
    }

    // Parameter(Trans) Data
    if(chkParamTransFile->Checked)
    {
        FileDownStart(DATAFILE_PARAM_TRANS, FILENAME_PARAM_TRANS);
    }
    
    // AMC Out File
    if(chkAMCOutFile->Checked == true)
    {
        FileDownStart(DATAFILE_AMCOUTFILE, FILENAME_AMCOUT);
    }

    // User Select File
    if(chkUserSelectFile->Checked == true)
    {
        FileDownStart(DATAFILE_USERSELECT, edtUserSelectFileName->Text);
    }
}
// ---------------------------------------------------------------------------


int TFileForm::GetnetworkDrive_NotConnected(String strNetName)
{
    // puting 2012.12.04 network driver enable

   char i;
   AnsiString strLoc="", strdd="", strNName="";
   char chThePath[255]={0,};
   DWORD MaxNetPathLen;
   int nError=0;
   bool bFindDrive;

   // shkim. 2015.11.19 안 쓰므로 막는다  bFindDrive=false;
   MaxNetPathLen = MAX_PATH;
   strNName = "\\\\"+strNetName+"\\OHT";

   for(i='Z'; i>='B'; i--)
   {
      strLoc = i;
	  strLoc += ":";
	  nError = WNetGetConnectionA(strLoc.c_str(),chThePath, &MaxNetPathLen);
	  if(nError==NO_ERR)
	  {
		 strdd=chThePath;
		 if(strdd==strNName)
		 {
			// shkim. 2015.11.19 안 쓰므로 막는다   bFindDrive=true;
			gm_OHTStatus->strNetworkDrive=strLoc;
			break;
		 }
	  }
   }

	return 0;
}

// --------------------------------------------------------------------------
int TFileForm::GetOhtLogDrive_NotConnected(String strNetName)
{
    // puting 2012.12.04 network driver enable

   char i;
   AnsiString strLoc="", strdd="", strNName="";
   char chThePath[255]={0,};
   DWORD MaxNetPathLen;
   int nError=0;
   bool bFindDrive;

   // shkim. 2015.11.19 안 쓰므로 막는다  bFindDrive=false;
   MaxNetPathLen = MAX_PATH;
   strNName = "\\\\"+strNetName+"\\LOG";

   for(i='Z'; i>='B'; i--)
   {
      strLoc = i;
	  strLoc += ":";
	  nError = WNetGetConnectionA(strLoc.c_str(),chThePath, &MaxNetPathLen);
	  if(nError==NO_ERR)
	  {
		 strdd=chThePath;
		 if(strdd==strNName)
		 {
			// shkim. 2015.11.19 안 쓰므로 막는다   bFindDrive=true;
			gm_OHTStatus->strLogworkDrive=strLoc;
			break;
		 }
	  }
   }

	return 0;
}

// ---------------------------------------------------------------------------
void __fastcall TFileForm::Button1Click(TObject *Sender)
{
    String strdd;

    switch(RadioGroup1->ItemIndex)
    {
    case 0: // error
		// GetLST_OHTFile("ErrorLog*.*");
		// 2016.12.29  에러에 해당하는 파일명을 몽땅 디스플레이 하도록 변경
		GetLST_OHTFile("*Error*.*");
        break;
    case 1: // autorecovery
        GetLST_OHTFile("AutoErrRecovery*.*");
        break;
    case 2: // etc
        // strdd.sprintf(L"%s*.*", Edit1->Text);
		// 2016.12.29  검색식 조건 변경. 제대로 된 와일드 카드 먹도록 변경
		strdd.sprintf(L"*%s*.*", Edit1->Text);
        GetLST_OHTFile(strdd);
        break;
    }
    FileListForm->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TFileForm::btnGetFileInfoClick(TObject *Sender)
{
    FileListForm->DataClear();
    GetLST_OHTFileInfo("*.exe");
    GetLST_OHTFileInfo(String(STR_PATH_DATAFILES) + "*.*");
    FileListForm->Show();
}

// ---------------------------------------------------------------------------
void TFileForm::GetLST_OHTFileInfo(String strTmpName)
{


	TSearchRec sr;
	String FromPath;
	String strLocalName = gm_OHTStatus->strNetworkDrive;

	FromPath.sprintf(L"%s\\%s", strLocalName, strTmpName);

	if(FindFirst(FromPath, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
	{
		//디렉토리를 제외하고 파일만을 검색한다
		if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
		{
			//검색한 내용이 파일이면
			if(!((sr.Name == ".") || (sr.Name == "..")))
			{
				TDateTime dt = FileDateToDateTime(sr.Time);
				FileListForm->DataAdd(sr.Name, FormatDateTime("yyyy-mm-dd hh:mm",dt));
			}
		}

		while(FindNext(sr) == 0)
		{
			if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
			{
				if((sr.Name == ".") || (sr.Name == "..")) continue;
				TDateTime dt = FileDateToDateTime(sr.Time);
				FileListForm->DataAdd(sr.Name, FormatDateTime("yyyy-mm-dd hh:mm",dt));
			}

			Application->ProcessMessages();
		}
	}
	FindClose(sr);
}
// ---------------------------------------------------------------------------

void __fastcall TFileForm::btnAssitantCloseClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

    m_CMDControl->MakeTPCmd2OHT(TPCMD_AST_ASSISTANTPROG_CLOSE);
    MainForm->DataDisplay(COLOR_BLACK, "Assistant Close");

    delete m_CMDControl;

}
// ---------------------------------------------------------------------------

void __fastcall TFileForm::btnAssistantUploadClick(TObject *Sender)
{
    UploadFile(DATAFILE_ASSISTANT, FILENAME_ASSISTANT);
}
// ---------------------------------------------------------------------------

void __fastcall TFileForm::btnAssistantExecClick(TObject *Sender)
{
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

    m_CMDControl->MakeTPCmd2OHT(TPCMD_AST_ASSISTANTPROG_EXEC);
    MainForm->DataDisplay(COLOR_BLACK, "Assistant Exec");

    delete m_CMDControl;
}
// ---------------------------------------------------------------------------

// by JYH 111027 : 파일 수정 시간 비교를 위한 함수
BOOL TFileForm::IsNewFile_Older(String oldFileName, String newFileName)
{
    TSearchRec sr; // TsarchRec
    FILETIME tmpTime;
    SYSTEMTIME SystemTime;
    TDateTime tmpOldFileTime, tmpNewFileTime; // 최종적으로 사용할 파일 시간

    String strMemo;

    // ---------------------------------------------------------------
    // by JYH 111027 : 파일 업로드 시 파일 수정 시간 비교 기능 추가
    //
    // 조건
    // 1. 업로드할 파일이 OHT에 저장된 파일보다 수정시간이 old인 경우을 체크
    // A. old인 경우 에러메세지 출력
    // a. 그래도 진행하고자 하는 경우 운영근무자 정보를 log로 기록하고 진행
    // B. old가 아닌 경우 기존과 같이 진행

    // 1. OHT 호기 내 파일 탐색
    if(!FindFirst(oldFileName, faAnyFile, sr))
    {
        // 파일 탐색 성공
        // 파일 수정 시간 취득
        FileTimeToLocalFileTime(&sr.FindData.ftLastWriteTime, &tmpTime);
        FileTimeToSystemTime(&tmpTime, &SystemTime); // File Time을 System Time으로 변환
        tmpOldFileTime = SystemTimeToDateTime(SystemTime); // System time을 TDateTime형으로 변환

        strMemo.sprintf(L"Current File = %s, Time = %s", oldFileName, tmpOldFileTime.FormatString("yyyy-mm-dd hh:mm:ss"));
        MainForm->DataDisplay(COLOR_BLACK, strMemo);

        FindClose(sr);
    }
    else
    {
        strMemo.sprintf(L"File Not Found");
        MainForm->DataDisplay(COLOR_BLACK, strMemo);

        FindClose(sr);
    }

    // 2. Upload할 파일 탐색
    if(!FindFirst(newFileName, faAnyFile, sr))
    {
        // 파일 탐색 성공
        // 파일 수정 시간 취득
        FileTimeToLocalFileTime(&sr.FindData.ftLastWriteTime, &tmpTime);
        FileTimeToSystemTime(&tmpTime, &SystemTime); // File Time을 System Time으로 변환
        tmpNewFileTime = SystemTimeToDateTime(SystemTime); // System time을 TDateTime형으로 변환

        strMemo.sprintf(L"Upload File = %s, Time = %s", newFileName, tmpOldFileTime.FormatString("yyyy-mm-dd hh:mm:ss"));
        MainForm->DataDisplay(COLOR_BLACK, strMemo);

        FindClose(sr);
    }
    else
    {
        strMemo.sprintf(L"File Not Found");
        MainForm->DataDisplay(COLOR_BLACK, strMemo);

        FindClose(sr);
    }

    // 3. 파일 시간 비교
    if(tmpOldFileTime >= tmpNewFileTime)
        return true;
    else
        return false;

}


void __fastcall TFileForm::Edit1Change(TObject *Sender)
{
	if(RadioGroup1->ItemIndex != 2)
		RadioGroup1->ItemIndex = 2;
}
//---------------------------------------------------------------------------


