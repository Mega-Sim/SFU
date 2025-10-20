//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SetCurnodeNUnit.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RebootUnit"
#pragma resource "*.dfm"
TSetCurForm *SetCurForm;
//---------------------------------------------------------------------------
__fastcall TSetCurForm::TSetCurForm(TComponent* Owner)
   : TRebootForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSetCurForm::btnOKClick(TObject *Sender)
{
   String strPass, strUserPass, strTmpCMD;
   strPass="rmobile0";
   strUserPass = edtPassword->Text;
    //puting 2012.12.03 공통cmd 구조체 초기화
   uTP_CMD TPCmd;     // TP Command
   memset(&TPCmd, 0x00, sizeof(uTP_CMD));

   TPCmd.TPCmd.Data1 = nCmdCurnode;

   if(strUserPass == strPass)
   {
      edtPassword->Clear();

      TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
      //strTmpCMD.sprintf(L"%06d",nCmdCurnode);
      m_CMDControl->MakeTPCmd2OHT(TPCMD_GO_SETCURNODE_NUM, &TPCmd);
      delete m_CMDControl;

      Close();

   }
   else
   {
      edtPassword->Clear();
      Caption = "Invalid Password";
   }
}
//---------------------------------------------------------------------------
void __fastcall TSetCurForm::btnCancelClick(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------
