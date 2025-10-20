// ---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <dos.h>
#include <Filectrl.hpp>
#include <ShellApi.h>
#pragma hdrstop

#include "LogFileUnit.h"
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
#include "FileListUnit.h"
#include "FileUnit.h"
#include "Def_TP.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLogFileForm *LogFileForm;

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
__fastcall TLogFileForm::TLogFileForm(TComponent* Owner) : TForm(Owner)
{
    bFileCancel = false;
}

// ---------------------------------------------------------------------------
void __fastcall TLogFileForm::comboFileSelectChange(TObject *Sender)
{
    String strLocalName = gm_OHTStatus->strNetworkDrive;

    switch(comboFileSelect->ItemIndex)
    {
    case 0: // download
        PageControl3->Enabled = true;
        tabDownload->TabVisible = true;
        tabGetFileList->TabVisible = false;
        break;
    case 1: // explore
        ShellExecute(0, L"explore", strLocalName.w_str(), NULL, NULL, SW_SHOWNORMAL);
        break;
    case 2: // GetfileList
        PageControl3->Enabled = true;
        tabDownload->TabVisible = false;
        tabGetFileList->TabVisible = true;
        break;
    }
}

// ---------------------------------------------------------------------------
void TLogFileForm::FileControlInit()
{
    bFileCancel = false;
}

// ---------------------------------------------------------------------------
int TLogFileForm::FileControlStart(String strRName)
{
    String TmpLocalName, TmpRemoteName, TmpPassWord, TmpID;
    int nError = 0;
    String strLocalName = gm_OHTStatus->strNetworkDrive;
    String strLogPath = gm_OHTStatus->strLogSavePath;

    FileControlInit();
    GetnetworkDrive_NotConnected(strRName);
    strLocalName = gm_OHTStatus->strNetworkDrive;
    TmpLocalName = strLocalName;

    TmpRemoteName = "\\\\" + strRName + strLogPath;

    TmpPassWord = "ohtlog";
    TmpID = "ohtlog";

    nError = NetWork_Connect(TmpLocalName, TmpRemoteName, TmpPassWord, TmpID);
    if((nError == 85) || (nError == 1219))
    {
        nError = 0;
        NetWork_DisConnect(strLocalName);
        nError = NetWork_Connect(TmpLocalName, TmpRemoteName, TmpPassWord, TmpID);
    }

    if(nError != NO_ERR)
    {
        TmpPassWord = "NO";
        TmpID = "NO";
        nError = NetWork_Connect(TmpLocalName, TmpRemoteName, TmpPassWord, TmpID);
    }
    return nError;
}

// ---------------------------------------------------------------------------
void TLogFileForm::FileControlClose()
{
    NetWork_DisConnect(gm_OHTStatus->strNetworkDrive);
}

// ---------------------------------------------------------------------------
int TLogFileForm::NetWork_Connect(String TmpLocalName, String TmpRemoteName, String TmpPassWord, String TmpID)
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
     ****/
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
        Result = WNetAddConnection2(&NetRes, TmpPassWord.w_str(), TmpID.w_str(), CONNECT_UPDATE_PROFILE); // CONNECT_INTERACTIVE

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
void TLogFileForm::NetWork_DisConnect(String TmpLocalName)
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
void TLogFileForm::FileDownStart(int nTmpData, String nTmpDownFileName)
{
    String TmpFromDir, TmpToDir, strTmpN;
    int TmpIndex;
    String TmpFilter;
    String strLocalName = gm_OHTStatus->strNetworkDrive;

    bFileCancel = false;
    TmpFromDir = strLocalName + "\\" + nTmpDownFileName;
    SaveDialog1->FileName = "Log";
    SaveDialog1->Title = "Select Directory";

    if(SaveDialog1->Execute())
    {
        MainForm->DataDisplay(COLOR_BLACK, "======= Download File List =======");
        TmpToDir = ExtractFilePath(SaveDialog1->FileName);
        DownloadLogFile(nTmpData, TmpFromDir, TmpToDir, 0);
    }
    MainForm->DataDisplay(COLOR_BLACK, "=============================");
}

// ---------------------------------------------------------------------------
void TLogFileForm::DownloadLogFile(int nTmpData, String FromPath, String ToPath, int nNameFlag)
{
    TSearchRec sr;
    String TmpName, TmpNewName;
    String strLocalName = gm_OHTStatus->strNetworkDrive;

    if(FindFirst(FromPath, faArchive | faReadOnly | faSysFile | faHidden, sr) == 0)
    {
        ProcessForm->Visible = true;
        // 디렉토리를 제외하고 파일만을 검색한다
        if(sr.Attr == faArchive || faReadOnly || faSysFile || faHidden)
        {
            // 검색한 내용이 파일이면
            if(!((sr.Name == ".") || (sr.Name == "..")))
            {
                TmpName = strLocalName + "\\" + sr.Name;
                switch(nNameFlag)
                {
                case 0:
                    TmpNewName = ToPath + "\\" + sr.Name;
                    break;
                case 1:
                    TmpNewName = ToPath;
                    break;
                }
                ProcessForm->lblFrom->Caption = TmpName;
                ProcessForm->lblTo->Caption = TmpNewName;
                if(CopyFileEx(TmpName.w_str(), TmpNewName.w_str(), CopyProgressRoutine, NULL, &bFileCancel, 0))
                    // COPY_FILE_FAIL_IF_EXISTS))
                    MainForm->DataDisplay(COLOR_BLUE, sr.Name + " DownLoad ..... OK");
                else
                    MainForm->DataDisplay(COLOR_RED, sr.Name + " DownLoad ..... Fail");
            }
        }
        while(FindNext(sr) == 0)
        {
            if(bFileCancel == true)
                break;
            if(sr.Attr == faArchive || faReadOnly || faSysFile || faHidden)
            {
                if((sr.Name == ".") || (sr.Name == ".."))
                    continue;
                TmpName = strLocalName + "\\" + sr.Name;

                switch(nNameFlag)
                {
                case 0:
                    TmpNewName = ToPath + "\\" + sr.Name;
                    break;
                case 1:
                    TmpNewName = ToPath;
                    break;
                }
                ProcessForm->lblFrom->Caption = TmpName;
                ProcessForm->lblTo->Caption = TmpNewName;

                if(CopyFileEx(TmpName.w_str(), TmpNewName.w_str(), CopyProgressRoutine, NULL, &bFileCancel, 0))
                    // COPY_FILE_FAIL_IF_EXISTS))
                    MainForm->DataDisplay(COLOR_BLUE, sr.Name + " DownLoad ..... OK");
                else
                    MainForm->DataDisplay(COLOR_RED, sr.Name + " DownLoad ..... Fail");
            }
            Application->ProcessMessages();
        }
        ProcessForm->Visible = false;
    }
    FindClose(sr);
}

// ---------------------------------------------------------------------------
String TLogFileForm::ExtractComboString(TComboBox* tmpCombo)
{
    String strTmpString;
    int nFrom, nTo;

    strTmpString = tmpCombo->Items->Strings[tmpCombo->ItemIndex];
    nFrom = strTmpString.Pos("[");
    nTo = strTmpString.Pos("]");
    return strTmpString.SubString(nFrom + 1, nTo - nFrom - 1);
}

// ---------------------------------------------------------------------------
void __fastcall TLogFileForm::FormShow(TObject *Sender)
{
    LogFileForm->Top = MainForm->Top + MainForm->Height;
    LogFileForm->Left = MainForm->Left;
    LogFileForm->Width = MainForm->Width;

    PageControl3->ActivePageIndex = 0;
    PageControl3->Enabled = true;
    comboFileSelect->ItemIndex = -1;
    comboFileSelect->Text = "Select Upload / Download";
}

// ---------------------------------------------------------------------------
void __fastcall TLogFileForm::btnDownloadClick(TObject *Sender)
{
    struct date today;
    struct time currenttime;

    if(UserLckbox->Checked == true)
    {
        FileDownStart(DATAFILE_LOG, Logedt->Text.Trim());
    }
    else
    {
        getdate(&today);
        gettime(&currenttime);

        LogDownForm->edtMonth->Text = IntToStr(today.da_mon);
        LogDownForm->edtSpecifyMonth->Text = IntToStr(today.da_mon);
        LogDownForm->edtDay->Text = IntToStr(today.da_day);
        LogDownForm->edtSpecifyDay->Text = IntToStr(today.da_day);
        LogDownForm->edtHour->Text = IntToStr(currenttime.ti_hour);

        LogDownForm->chkHour0->Checked = false;
        LogDownForm->chkHour1->Checked = false;
        LogDownForm->chkHour2->Checked = false;
        LogDownForm->chkHour3->Checked = false;
        LogDownForm->chkHour4->Checked = false;
        LogDownForm->chkHour5->Checked = false;
        LogDownForm->chkHour6->Checked = false;
        LogDownForm->chkHour7->Checked = false;
        LogDownForm->chkHour8->Checked = false;
        LogDownForm->chkHour9->Checked = false;
        LogDownForm->chkHour10->Checked = false;
        LogDownForm->chkHour11->Checked = false;
        LogDownForm->chkHour12->Checked = false;
        LogDownForm->chkHour13->Checked = false;
        LogDownForm->chkHour14->Checked = false;
        LogDownForm->chkHour15->Checked = false;
        LogDownForm->chkHour16->Checked = false;
        LogDownForm->chkHour17->Checked = false;
        LogDownForm->chkHour18->Checked = false;
        LogDownForm->chkHour19->Checked = false;
        LogDownForm->chkHour20->Checked = false;
        LogDownForm->chkHour21->Checked = false;
        LogDownForm->chkHour22->Checked = false;
        LogDownForm->chkHour23->Checked = false;

        LogDownForm->bIsOHTExecLogSave = false;
        LogDownForm->ShowModal();
    }
}
// ---------------------------------------------------------------------------

int TLogFileForm::GetnetworkDrive_NotConnected(String strNetName)
{

    char i;
    String strLoc, strdd, strNName;
    TCHAR chThePath[255];
    DWORD MaxNetPathLen;
    int nError = 0;
    bool bFindDrive = false;

    MaxNetPathLen = MAX_PATH;

    strNName.sprintf(L"\\\\%s\\Log", strNetName);

    for(i = 'Z'; i >= 'H'; i--)
    {
        strLoc = i;
        strLoc += ":";
        nError = WNetGetConnection(strLoc.w_str(), chThePath, &MaxNetPathLen);
        if(nError == NO_ERR)
        {
            strdd = chThePath;
            if(strdd == strNName)
            {
                bFindDrive = true;
                gm_OHTStatus->strNetworkDrive = strLoc;
                break;
            }
        }
    }

    if(bFindDrive == false)
    {
        for(i = 'Z'; i >= 'H'; i--)
        {
            strLoc = i;
            strLoc += ":";
            nError = WNetGetConnection(strLoc.w_str(), chThePath, &MaxNetPathLen);

            strdd = chThePath;
            if(nError) // Not Connected
            {
                strdd = chThePath;
                gm_OHTStatus->strNetworkDrive = strLoc;
                break;
            }
        }
    }

    return 0;
}

void __fastcall TLogFileForm::Button1Click(TObject *Sender)
{
    String strdd;

    switch(RadioGroup1->ItemIndex)
    {
    case 0: // error
        GetLST_OHTFile("ErrorLog*.*");
        break;
    case 1: // autorecovery
        GetLST_OHTFile("AutoErrRecovery*.*");
        break;
    case 2: // etc
        strdd.sprintf(L"%s*.*", Edit1->Text);
        GetLST_OHTFile(strdd);
        break;
    }
    FileListForm->Show();

}

// ---------------------------------------------------------------------------
void TLogFileForm::GetLST_OHTFile(String strTmpName)
{
    // puting 2012.12.04 network driver enable

    // TSearchRec sr;
    // String FromPath;
    // String strLocalName = gm_OHTStatus->strNetworDirve;
    //
    // if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
    // FromPath.sprintf(L"%s\\%s", strLocalName, strTmpName);
    // else
    // FromPath.sprintf(L"%s\\Log\\%s", strLocalName, strTmpName);
    //
    // FileListForm->DataClear();
    //
    // if(FindFirst(FromPath, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
    // {
    // //디렉토리를 제외하고 파일만을 검색한다
    // if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
    // {
    // //검색한 내용이 파일이면
    // if(!((sr.Name == ".") || (sr.Name == "..")))
    // {
    // FileListForm->DataAdd(sr.Name);
    // }
    // }
    // while(FindNext(sr) == 0)
    // {
    // if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
    // {
    // if((sr.Name == ".") || (sr.Name == "..")) continue;
    // FileListForm->DataAdd(sr.Name);
    // }
    // Application->ProcessMessages();
    // }
    // }
    // FindClose(sr);
}
// ---------------------------------------------------------------------------
