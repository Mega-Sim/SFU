//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RFTPUnit.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "OHTInfoUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRFTPForm *RFTPForm;
//---------------------------------------------------------------------------
__fastcall TRFTPForm::TRFTPForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TRFTPForm::Button1Click(TObject *Sender)
{
   String strTmpID;
   int nTmpID;

   //puting 2012.12.03 공통cmd 구조체 초기화
   uTP_CMD TPCmd;     // TP Command
   memset(&TPCmd, 0x00, sizeof(uTP_CMD));

   strTmpID = edtRFTPID->Text;
   nTmpID = StrToIntDef(strTmpID,0);

   TPCmd.TPCmd.Data1 = StrToIntDef(strTmpID,0);
   if((nTmpID !=0)&&(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_MANUAL))
   {
      TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
      m_CMDControl->MakeTPCmd2OHT(TPCMD_SET_RFTP_ID, &TPCmd);
      //m_CMDControl->MakeCommand2OHT(TPCMD_SET_RFTP_ID, strTmpID);
      delete m_CMDControl;
   }
   Close();
}
//---------------------------------------------------------------------------

void __fastcall TRFTPForm::Button2Click(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------

