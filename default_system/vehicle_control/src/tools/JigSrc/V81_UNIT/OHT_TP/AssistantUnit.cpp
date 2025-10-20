// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AssistantUnit.h"
#include "SocketUnit.h"
#include "MainUnit.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "Define.h"
#include "FileUnit.h"
#include "OHTInfoUnit.h"
#include "Def_TP.h"
#include <shlwapi.h>
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGAUGES"
#pragma resource "*.dfm"
#pragma link "shlwapi.lib"
TAssistantForm *AssistantForm;

// ---------------------------------------------------------------------------
DWORD CALLBACK MainCopyProgressRoutine(
    LARGE_INTEGER TotalFileSize,          // total file size, in bytes
    LARGE_INTEGER TotalBytesTransferred,  // total number of bytes transferred
    LARGE_INTEGER StreamSize,             // total number of bytes for this stream
    LARGE_INTEGER StreamBytesTransferred, // total number of bytes transferred for
    DWORD dwStreamNumber,                 // the current stream
    DWORD dwCallbackReason,               // reason for callback
    HANDLE hSourceFile,                   // handle to the source file
    HANDLE hDestinationFile,              // handle to the destination file
    LPVOID lpData                         // passed by CopyFileEx
    )
{
    //int newpos;

    if((int)TotalFileSize.QuadPart == 0)
    {
        AssistantForm->Caption = "0";
        //newpos                 = 0;
    }
    else
    {
        AssistantForm->Caption = Format("[%d] TotalFileSize: %d, Transffered: %d",
            OPENARRAY(TVarRec, ((int)(TotalBytesTransferred.QuadPart * 100 / TotalFileSize.QuadPart)
            , (int)TotalFileSize.QuadPart, (int)TotalBytesTransferred.QuadPart)));

        //newpos = (int)(TotalBytesTransferred.QuadPart * 100 / TotalFileSize.QuadPart);
    }

    // AssistantForm->CGauge1->Progress = newpos;
    Application->ProcessMessages();
    return PROGRESS_CONTINUE;
}

// ---------------------------------------------------------------------------
__fastcall TAssistantForm::TAssistantForm(TComponent *Owner)
    :
    TForm(Owner)
{
}

// ---------------------------------------------------------------------------
void __fastcall TAssistantForm::FormClose(TObject *Sender,
    TCloseAction &Action)
{
    // SocketForm->Assistant_DisConnect();
    FileForm->FileControlClose();

    if(SocketForm->OHTSocket->Active == false)
    {
        SocketForm->AssistantSocket->Close();
    }
}

// ---------------------------------------------------------------------------
void __fastcall TAssistantForm::FormShow(TObject *Sender)
{
    String strTmpVer, g_strOHTNum;
    int nPos;

    AssistantForm->Top   = MainForm->Top + MainForm->Height + 1;	// MainForm->Top;
    AssistantForm->Left  = MainForm->Left;		// MainForm->Width;
    AssistantForm->Height = MainForm->Height;

    // strTmpVer = SocketForm->g_strOHTName;
    nPos = strTmpVer.Pos("#");
    if(nPos == 0)
        g_strOHTNum = "?";
    else
        g_strOHTNum = strTmpVer.SubString(nPos + 1, strTmpVer.Length() - nPos + 1);

    if(g_strOHTNum.Length() > 2)
        panVer->Font->Size = 32;
    else
        panVer->Font->Size = 48;

    panVer->Caption = g_strOHTNum;

    // StatusBar1->Panels->Items[0]->Text=SocketForm->g_strOHTHost;

}
// ---------------------------------------------------------------------------

/**
@brief   OHT Main 프로그램을 종료하는 함수
@author  zzang9un
@date    2013.11.08
*/
void __fastcall TAssistantForm::btnOHTMainCloseClick(TObject *Sender)
{
    uTP_CMD TPCmd;
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2AST(TPCMD_AST_OHT_MAINPROG_CLOSE, &TPCmd);
	MainForm->DataDisplay(COLOR_BLACK, "OHT Main Program Close");

	delete m_CMDControl;
}
// ---------------------------------------------------------------------------

/**
@brief   OHT Main 프로그램을 실행하는 함수
@author  zzang9un
@date    2013.11.08
*/
void __fastcall TAssistantForm::btnOHTMainExecClick(TObject *Sender)
{
    uTP_CMD TPCmd;
	memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2AST(TPCMD_AST_OHT_MAINPROG_EXEC, &TPCmd);
	MainForm->DataDisplay(COLOR_BLACK, "OHT Main Program Exec");

	delete m_CMDControl;
}
// ---------------------------------------------------------------------------
//
//void __fastcall TAssistantForm::btnScreenCaptureClick(TObject *Sender)
//{
//	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
//
//     m_CMDControl->Assistant_MakeCommand2OHT(OHT_CAPTURE);
//	MainForm->DataDisplay(COLOR_BLACK, "OHT Capture");
//
//    delete m_CMDControl;
//}
//// ---------------------------------------------------------------------------
//
//void __fastcall TAssistantForm::btnDownImgAndMainExecClick(TObject *Sender)
//{
//    String strTmpName, strTmpNewName, TmpFilter;
//    String strLocalName = gm_OHTStatus->strNetworkDrive;
//
//    // "Text files (*.txt)|*.TXT";
//	TmpFilter             = "Save Image File(*.bmp)|*.BMP";
//    dlgOpen->Filter   = TmpFilter;
//    dlgOpen->FileName = "OHTCapture.bmp";
//
//    if(dlgOpen->Execute())
//    {
//        strTmpName    = strLocalName + L"\\" + STR_PATH_LOG + L"\\" + L"OHTCapture.bmp";
//        strTmpNewName = dlgOpen->FileName;
//        if(CopyFile(strTmpName.w_str(), strTmpNewName.w_str(), FALSE) != 0)
//        {
//            MainForm->DataDisplay(COLOR_BLUE, ExtractFileName(strTmpName) + " Download ..... OK");
//            ShellExecute(Handle, L"open", strTmpNewName.w_str(), NULL, NULL, SW_SHOW);
//            DeleteFile(strTmpName.w_str());
//        }
//	}
//}
// ---------------------------------------------------------------------------

void __fastcall TAssistantForm::btnOHTMainUploadClick(TObject *Sender)
{
    String strFromName, strToName, TmpFilter;
	String strLocalName = gm_OHTStatus->strNetworkDrive;
    BOOL   bFileCancel  = false;

    TmpFilter             = "Upload OHT File(*.exe)|*.EXE";
    dlgOpen->Filter   = TmpFilter;
    dlgOpen->FileName = "OHT.exe";

    if(dlgOpen->Execute())
    {
        strFromName = dlgOpen->FileName;
		strToName   = strLocalName + "\\" + "OHT.exe";

        // CGauge1->Progress=0;
		if(CopyFileEx(strFromName.w_str(), strToName.w_str(), MainCopyProgressRoutine, NULL, &bFileCancel, 0))
            // COPY_FILE_FAIL_IF_EXISTS
            // if(CopyFile(strFromName.w_str(),strToName.w_str(),FALSE)!=0)
        {
            MainForm->DataDisplay(COLOR_BLUE, ExtractFileName(strFromName) + " Upload ..... OK");
            ShowMessage("OHT.exe Upload OK....");
            // CGauge1->Progress=0;
        }
        else
        {
            ShowMessage("OHT.exe Upload Fail....");
            // CGauge1->Progress=0;
        }
    }

}
// ---------------------------------------------------------------------------
/**
@brief      FBWF의 상태를 Assistant에 요청
@author     doori.shin
@date       2013.12.31
*/
void __fastcall TAssistantForm::btReqFBWFStatClick(TObject *Sender)
{
    uTP_CMD TPCmd;
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2AST(TPCMD_AST_OHT_REQ_FBWF_STATUS, &TPCmd);
    MainForm->DataDisplay(COLOR_BLACK, "Request FBWF Status");

    delete m_CMDControl;
}
//---------------------------------------------------------------------------
/**
@brief      OHT시스템의 FBWF상태를 출력
@author     doori.shin
@date       2013.12.31
@param      status : FBWF상태
*/
void TAssistantForm::SetFBWFStatus(int status)
{
    String strFBWFCaption = MainForm->panFBWF->Caption;
    TColor cFBWF = MainForm->panFBWF->Color;
    String strDisplay = L"";
    TColor cDisplay = COLOR_BLACK;

    switch(status)
    {
        case FBWF_ENABLE :
            strFBWFCaption = L"FBWF:Enable";
            cFBWF = COLOR_LIME;
            strDisplay = L"File-based write filter configuration for the current session: \nfilter state: enabled.";
            break;
        case FBWF_DISABLE :
            strFBWFCaption = L"FBWF:Disable";
			cFBWF = COLOR_RED;
            strDisplay = L"File-based write filter configuration for the current session: \nfilter state: disabled.";
            break;
        case FBWF_WILL_BE_ENABLED :
            strDisplay = L"File-based write filter configuration for the next session: \nfilter state: enabled.";
            cDisplay = COLOR_BLUE;
            break;
        case FBWF_WILL_BE_DISABLED :
            strDisplay = L"File-based write filter configuration for the next session: \nfilter state: disabled.";
            cDisplay = COLOR_BLUE;
            break;
		case FBWF_ERROR :
			strFBWFCaption = L"FBWF:Unknown";
			strDisplay = L"Fail to get information FBWF status in OHT system..";
			cDisplay = COLOR_RED;
			cFBWF = COLOR_YELLOW;
            break;
    }

    MainForm->panFBWF->Caption = strFBWFCaption;
    MainForm->panFBWF->Color = cFBWF;

    MainForm->DataDisplay(cDisplay, strDisplay);
}
/**
@brief      FBWF를 Enable로 Assistant에 요청
@author     doori.shin
@date       2013.12.31
*/
void __fastcall TAssistantForm::btReqFBWFEnableClick(TObject *Sender)
{
    uTP_CMD TPCmd;
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2AST(TPCMD_AST_OHT_REQ_FBWF_ENABLE, &TPCmd);
    MainForm->DataDisplay(COLOR_BLACK, "Request FBWF Status");

    delete m_CMDControl;
}
//---------------------------------------------------------------------------
/**
@brief      FBWF를 disable상태로 Assistant에 요청
@author     doori.shin
@date       2013.12.31
*/
void __fastcall TAssistantForm::btReqFBWFDisableClick(TObject *Sender)
{
    uTP_CMD TPCmd;
	memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2AST(TPCMD_AST_OHT_REQ_FBWF_DISABLE, &TPCmd);
    MainForm->DataDisplay(COLOR_BLACK, "Request FBWF Status");

	delete m_CMDControl;
}
//---------------------------------------------------------------------------

/**
@brief      OHT 시스템 정보를 표시함
@author     doori.shin
@date       2013.12.31
@param      info : OHT 시스템 정보를 갖는 구조
*/
void TAssistantForm::SetOHTAddidtionalInfo(OHT_ADDITIONAL_INFO* info)
{
    memcpy(&AdditionalInfo, info, sizeof(OHT_ADDITIONAL_INFO));

    //1) Set OHT Version
    String strVersion;
    if(AdditionalInfo.OHTVersion.Version.IsExists != 0)
    {
        strVersion.sprintf(L"%d.%d.%d", AdditionalInfo.OHTVersion.Version.Major, AdditionalInfo.OHTVersion.Version.Minor,
                            AdditionalInfo.OHTVersion.Version.Build);
    }
    else
    {
        strVersion = L"Main Program doesn't exist";
    }
    lbOHTVersion->Caption = strVersion;

    //2) Disk 정보
    // system disk
    TCHAR *Total = new TCHAR[10];
    TCHAR *Free = new TCHAR[10];
    Total = StrFormatByteSizeW(AdditionalInfo.SystemDiskInfo.TotalSpace, Total, 32);
    Free = StrFormatByteSizeW(AdditionalInfo.SystemDiskInfo.FreeSpace, Free, 32);
    String strDisk;
    strDisk.sprintf(L"%s / %s", Free, Total);
    lbSpace->Caption = strDisk;

    //3) D:\ 존재여부
    if(AdditionalInfo.LogDiskInfo.TotalSpace == -1)
    {
        lbDriveConnection->Caption = "Disconnection";
	}
    else
    {
        lbDriveConnection->Caption = "Connection";

        // Log disk
        Total = StrFormatByteSizeW(AdditionalInfo.LogDiskInfo.TotalSpace, Total, 32);
        Free = StrFormatByteSizeW(AdditionalInfo.LogDiskInfo.FreeSpace, Free, 32);
        strDisk.sprintf(L"%s / %s", Free, Total);
        lbLogSpace->Caption = strDisk;
    }

	//5) FBWF 표시
	SetFBWFStatus(AdditionalInfo.FBWFStatus);

	//6) 마무으리
    delete Total;
    delete Free;
}

/**
@brief   복사중인 파일의 완료여부를 알아냄
@author  doori.shin
@date    2013.08.08
@return  true : 완료, false : 복사중
*/
bool TAssistantForm::CheckFileCopyCompleted(String strFileName)
{

	HANDLE hFile = CreateFile(strFileName.w_str(),
							  GENERIC_READ,
							  0,    /// no share
							  NULL,
							  OPEN_EXISTING,
							  FILE_ATTRIBUTE_NORMAL,
							  NULL);


	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return true;

	}

	return false;

}
