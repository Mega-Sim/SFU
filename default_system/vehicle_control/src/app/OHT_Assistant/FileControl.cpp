//---------------------------------------------------------------------------


#pragma hdrstop

#include <vcl.h>
#include <stdio.h>

#include "FileControl.h"
#include "ProcessTerminator.h"
//---------------------------------------------------------------------------

FileControl::FileControl()
{

}

FileControl& FileControl::getInstance()
{
	if(instance == NULL)
	{
		instance = new FileControl;
	}

	return *instance;
}


// ---------------------------------------------------------------------------
void FileControl::ExecProgram(String sFileName)
{
	HINSTANCE hResult;
    int nResult;
	String strFileName;
	String strRes;

	strFileName = ".\\" + sFileName;

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

// ---------------------------------------------------------------------------
void FileControl::CloseProgram(String sFileName)
{
    HWND hFindHandle;
	String strResult;

	hFindHandle = FindWindow(sFileName.c_str(), NULL);

	if(hFindHandle != NULL)
	{
		PostMessage(hFindHandle, WM_CLOSE, 0, 0);
	}
}

// ---------------------------------------------------------------------------
void FileControl::CloseExceptProgram(String sFileName)
{
	HWND hFindHandle;
	String strResult;
	String strFileName;

	strFileName = sFileName + " - 응용 프로그램 오류";

	hFindHandle = FindWindow(strFileName.c_str(), NULL);

    if(hFindHandle != NULL)
    {
        PostMessage(hFindHandle, WM_CLOSE, 0, 0);
    }
}

// ---------------------------------------------------------------------------
void FileControl::Delete_File(String sFileName)
{
    String strFileName;
    strFileName = ".\\" + sFileName;

    DeleteFile(strFileName);
}

// ---------------------------------------------------------------------------
void FileControl::Create_Directory(String sDirectoryName)
{
    String sTmpDirectoryName;
    sTmpDirectoryName = ".\\" + sDirectoryName;

    CreateDirectory(sTmpDirectoryName.c_str(), 0);
}

// ---------------------------------------------------------------------------
void FileControl::Delete_Directory(String sDirectoryName)
{
    String sTmpDirectoryName;
    sTmpDirectoryName = ".\\" + sDirectoryName;

    RemoveDirectory(sTmpDirectoryName.c_str());
}

// ---------------------------------------------------------------------------
void FileControl::ExecOHTProgram(void)
{
	ExecProgram(L".\\OHT.exe");
}

// ---------------------------------------------------------------------------
void FileControl::CloseOHTProgram(void)
{
	TForm *frm;
	DWORD pid = ProcessTerminator::getInstance().FindPID(PROC_NAME);
	if(pid == 0)
	{
		MessageDlg("OHT MAIN NOT OPEN", mtConfirmation, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	int result = ProcessTerminator::getInstance().TerminateApp(pid, 1000, false);

//	String msg;
//
//	switch(result)
//	{
//		case TA_FAILED:
//			msg = "Fail to terminate the [OHT.exe]";
//			break;
//		case TA_SUCCESS_CLEAN:
//			msg = "Terminate the process [OHT.exe]";
//			break;
//		case TA_SUCCESS_KILL:
//			msg = "Kill the process [OHT.exe] Force Closed.";
//			break;
//	}
//
//	frm = (TForm*) CreateMessageDialog(msg, mtCustom, TMsgDlgButtons()<<mbOK);
//	frm->ShowModal();
}
// ---------------------------------------------------------------------------
void FileControl::ExecFMProgram(void)
{
	ExecProgram(L".\\FM.exe");
}

// ---------------------------------------------------------------------------
void FileControl::CloseExceptProgram(void)
{
    HWND hFindHandle;
    String strResult;

    hFindHandle = FindWindow(NULL, L"OHT.exe - 응용 프로그램 오류");

    if(hFindHandle != NULL)
    {
		PostMessage(hFindHandle, WM_CLOSE, 0, 0);
	}

	
	hFindHandle = FindWindow(NULL, L"FM.exe - 응용 프로그램 오류");

    if(hFindHandle != NULL)
    {
		PostMessage(hFindHandle, WM_CLOSE, 0, 0);
	}
}

// ---------------------------------------------------------------------------

#pragma package(smart_init)
