//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RebootUnit.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRebootForm *RebootForm;
//---------------------------------------------------------------------------
__fastcall TRebootForm::TRebootForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TRebootForm::btnOKClick(TObject *Sender)
{
   String strPass, strUserPass;
   strPass="rmobile0";
   strUserPass = edtPassword->Text;
   if(strUserPass == strPass)
   {
      edtPassword->Clear();
      TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
      m_CMDControl->MakeTPCmd2OHT(TPCMD_OHT_REBOOT);
      delete m_CMDControl;
      RebootForm->Close();
   }
   else
   {
      edtPassword->Clear();
      RebootForm->Caption = "Invalid Password";
   }
}
//---------------------------------------------------------------------------
void __fastcall TRebootForm::btnCancelClick(TObject *Sender)
{
   Close();   
}
//---------------------------------------------------------------------------
