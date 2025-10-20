//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
                               
#include "FileUploadAlarm.h"
#include "Utility.h"

#include "FileUnit.h"
#include "MainUnit.h"
#include "SocketUnit.h"
#include "ProcessUnit.h"
#include "Define.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "WaitUnit.h"
#include "LogDownUnit.h"
#include "ConnectUnit.h"
#include "FoupOnlineCMDUnit.h"
//#include "ReticleOnlineCMDUnit.h"
#include "EUVOnlineCMDUnit.h"
//#include "ProbeOnlineCMDUnit.h"
//#include "ProbeJigOnlineCMDUnit.h"
#include "ChkExecUnit.h"
#include "LogFileUnit.h"
#include "FileListUnit.h"
#include "TeachingData.h"
#include "Def_TP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFileUploadAlarmForm *FileUploadAlarmForm;
//---------------------------------------------------------------------------
__fastcall TFileUploadAlarmForm::TFileUploadAlarmForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFileUploadAlarmForm::FormShow(TObject *Sender)
{
      FileUploadAlarmForm->Caption = "Version of The File Alarm";
      btnForce->Caption = "Continue to Force";
      btnCancel->Caption = "Cancel";
      Label1->Caption = "** The Operator is responsible for the Error After Compulsory process!!!";
      Label2->Caption = "Operator Name :";
      Label3->Caption = "* If you wanna cancel Click the Cancel (or Close) button.";
      lbAlarm->Caption = "*** If you wish to proceed Compulsory Please enter a name.";
}
//---------------------------------------------------------------------------
void __fastcall TFileUploadAlarmForm::btnForceClick(TObject *Sender)
{
	if(editName->Text == "")
	{
      Application->MessageBox(L"Please enter your name.", L"Error", MB_OK);
      return;
   }
   else
   {
      TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
      String strLog;
         strLog.sprintf(L"====> File Upload in Compulsory process : Operator[%s]", editName->Text);
      MainForm->DataDisplay(COLOR_BLUE, strLog);
      m_LogFile->WriteLogFile("FileUpload",strLog);
      delete m_LogFile;

      FileForm->bFlag_ForceFileUpload = true;

      Close();
   }
}
//---------------------------------------------------------------------------

void __fastcall TFileUploadAlarmForm::btnCancelClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

