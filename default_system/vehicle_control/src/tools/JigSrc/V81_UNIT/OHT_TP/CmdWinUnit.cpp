//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CmdWinUnit.h"
#include "FoupOnlineCMDUnit.h"
//#include "ReticleOnlineCMDUnit.h"
#include "EUVOnlineCMDUnit.h"
//#include "ProbeOnlineCMDUnit.h"
//#include "ProbeJigOnlineCMDUnit.h"
#include "OHTInfoUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCmdWinForm *CmdWinForm;
//---------------------------------------------------------------------------
__fastcall TCmdWinForm::TCmdWinForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TCmdWinForm::ListBox1DblClick(TObject *Sender)
{
   String strdd;
   strdd = ListBox1->Items->Strings[ListBox1->ItemIndex];
//   CmdWinForm->Caption = strdd;
}
//---------------------------------------------------------------------------
void __fastcall TCmdWinForm::FormShow(TObject *Sender)
{
	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP) ||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
	{
		CmdWinForm->Top = FoupOnlineCMDForm->Top;
		CmdWinForm->Left = FoupOnlineCMDForm->Left + FoupOnlineCMDForm->Width;
		CmdWinForm->Height = FoupOnlineCMDForm->Height;
	}
	else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
	{
		CmdWinForm->Top = EUVOnlineCMDForm->Top;
		CmdWinForm->Left = EUVOnlineCMDForm->Left + EUVOnlineCMDForm->Width;
		CmdWinForm->Height = EUVOnlineCMDForm->Height;
	}

}
//---------------------------------------------------------------------------
