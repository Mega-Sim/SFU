//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OptionUnit.h"
#include "MainUnit.h"
#include "Def_TP.h"

#include "IniFiles.hpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TOptionForm *OptionForm;
//---------------------------------------------------------------------------
__fastcall TOptionForm::TOptionForm(TComponent* Owner)
	: TForm(Owner)
{
	TIniFile *ini;

	AnsiString sExePath = ExtractFilePath(Application->ExeName);
	AnsiString sIniFilePath = sExePath + "\\DataFiles\\Config.ini";

	ini = new TIniFile(sIniFilePath);
	LogValueListEditor1->Values["Delete_Day"] = ini->ReadInteger("Log", "Delete_Day", LOG_DELETE_DAY);

	delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TOptionForm::tvOptionClick(TObject *Sender)
{
	TTreeNode *pNode = tvOption->Selected;

	PageControl1->ActivePageIndex = pNode->Index;
}
//---------------------------------------------------------------------------

void __fastcall TOptionForm::LogValueListEditor1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if(Key==VK_RETURN)
	{
		TIniFile *ini;

		AnsiString sExePath = ExtractFilePath(Application->ExeName);
		AnsiString sIniFilePath = sExePath + "\\DataFiles\\Config.ini";

		ini = new TIniFile(sIniFilePath);
		ini->WriteString("Log", "Delete_Day", LogValueListEditor1->Values["Delete_Day"]);

		delete ini;
	}
}
//---------------------------------------------------------------------------

