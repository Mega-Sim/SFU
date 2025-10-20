//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "About.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAboutBox *AboutBox;
//---------------------------------------------------------------------------
__fastcall TAboutBox::TAboutBox(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAboutBox::FormShow(TObject *Sender)
{
	UnicodeString ExeFileName;
	ExeFileName = Application->ExeName;

	/*
	DWORD size, dwHandle;
	LPBYTE pVerInfo = NULL;
	VS_FIXEDFILEINFO* pFixedFileInfo = NULL;
	UINT uiLength;
	size = GetFileVersionInfoSize(ExeFileName.c_str(), &dwHandle);
	pVerInfo = new BYTE[size];
	if (::GetFileVersionInfo(ExeFileName.c_str(), 0, size, pVerInfo) &&
		::VerQueryValue(pVerInfo, TEXT("\\"), (LPVOID*)&pFixedFileInfo, &uiLength))
	{
		//
	}

	delete pVerInfo;
	int main = pFixedFileInfo->dwFileVersionMS / (16*16*16*16);
	int sub1 = pFixedFileInfo->dwFileVersionMS % (16*16*16*16);
	int sub2 = pFixedFileInfo->dwFileVersionLS / (16*16*16*16);
	int sub3 = pFixedFileInfo->dwFileVersionLS % (16*16*16*16);
	*/

	int main = 1;
	int sub1 = 0;
	int sub2 = 0;
	int sub3 = 0;

	AnsiString VersionText;
	VersionText = "Version :   " + IntToStr(main);
	VersionText += ".";
	VersionText += IntToStr(sub1);
	VersionText += ".";
	VersionText += IntToStr(sub2);
	VersionText += ".";
	VersionText += IntToStr(sub3);

	VersionLabel->Caption = VersionText;
}
//---------------------------------------------------------------------------
void __fastcall TAboutBox::OKButtonClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
