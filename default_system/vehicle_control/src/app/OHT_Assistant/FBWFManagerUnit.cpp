//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FBWFManagerUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFBWFManagerForm *FBWFManagerForm;
//---------------------------------------------------------------------------
__fastcall TFBWFManagerForm::TFBWFManagerForm(TComponent* Owner)
	: TForm(Owner)
{
	Init();
}

void TFBWFManagerForm::Init()
{

}
//---------------------------------------------------------------------------
/**
@brief		FBWF 상태를 설정
@author  	doori.shin
@date    	2013.12.31
*/
void __fastcall TFBWFManagerForm::btSetStatusClick(TObject *Sender)
{
	int error = 0;

	switch(rgFBWF->ItemIndex)
	{
	case 0 :
		{
			error = MonitoringUtil::getInstance().ExecuteDosCmd(FBWF_SET_ENABLE);
			break;
		}
	case 1 :
		{
			error = MonitoringUtil::getInstance().ExecuteDosCmd(FBWF_SET_DISABLE);
			break;
		}
	}

	DisplayStatus(error);
}
//---------------------------------------------------------------------------
/**
@brief		현재 FBWF 상태를 얻음
@author  	doori.shin
@date    	2013.12.31
*/
void __fastcall TFBWFManagerForm::btGetStatusClick(TObject *Sender)
{
	int error = MonitoringUtil::getInstance().ExecuteDosCmd(FBWF_CURRENT_STATUS);

	DisplayStatus(error);
}
//---------------------------------------------------------------------------
/**
@brief      현재 FBWF 상태를 출력
@author  	doori.shin
@date    	2013.12.31
*/
void TFBWFManagerForm::DisplayStatus(int status)
{
	mmoLog->Lines->Add(MonitoringUtil::getInstance().GetResultString());

	switch(status)
	{
		case FBWF_ENABLE :
			statFBWF->SimpleText = "FBWF : Enable";
			break;
		case FBWF_DISABLE :
			statFBWF->SimpleText = "FBWF : Disable";
			break;
		case 0 :
			break;
		default :
		//error occured
		{
			mmoLog->Lines->Add(L"Error occured");
		}
	}
}

