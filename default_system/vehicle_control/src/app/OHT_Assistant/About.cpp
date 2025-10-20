/**
@file    About.cpp
$Rev:: 771           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-03-28 1#$:  Date of last commit
@version OHT 7.0
@brief   프로그램 정보 표시를 위한 Form CPP 파일
*/

// ---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "About.h"
// ---------------------------------------------------------------------
#pragma resource "*.dfm"
TAboutBox *AboutBox;

// ---------------------------------------------------------------------
__fastcall TAboutBox::TAboutBox(TComponent *AOwner)
	:
	TForm(AOwner)
{

}
// ---------------------------------------------------------------------

void __fastcall TAboutBox::FormShow(TObject *Sender)
{
	// 프로그램 정보가 먼저 보이도록 변경
	pgcAbout->ActivePageIndex = 0;

	// Form이 보여질 때 업데이트 정보를 파일로부터 읽어옴
	String FileName;
	FileName = ExtractFileDir(Application->ExeName) + FILENAME_UPDATE;

	redtUpdate->Lines->LoadFromFile(FileName);
}
// ---------------------------------------------------------------------------

/**
@brief   파일정보를 문자열로 변환해주는 함수

         Query는 다음 문자열 중 하나로 해야 한다.
         "CompanyName",
		"FileDescription",
		"FileVersion",
		"InternalName",
		"LegalCopyright",
		"LegalTradeMarks",
		"OriginalFileName",
		"ProductName",
		"ProductVersion",
		"Comments"
@param   sQuery : 얻어올 정보(query할 String)
@return  sQuery를 통해 얻어온 정보를 String으로 return
*/
String __fastcall TAboutBox::VersionInfo(const String &sQuery)
{
	DWORD dwHandle = 0, dwVersionInfoSize;
	UINT uLength;
	LPVOID pFileInfo, ptr;
	String sOut; // 리턴될 버전 정보.

	String filename = Application->ExeName;

	dwVersionInfoSize = GetFileVersionInfoSize(filename.w_str(), &dwHandle);

	pFileInfo = (LPVOID) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwVersionInfoSize);

	GetFileVersionInfo(filename.w_str(), dwHandle, dwVersionInfoSize, pFileInfo);
	VerQueryValue(pFileInfo, TEXT("\\VarFileInfo\\Translation"), &ptr, &uLength);

	WORD *id = (WORD*) ptr;
	String szString = "\\StringFileInfo\\" + IntToHex(id[0], 4) + IntToHex(id[1], 4) + "\\" + sQuery;

	VerQueryValue(pFileInfo, szString.w_str(), &ptr, &uLength);
	sOut = String((TCHAR *) ptr);
	HeapFree(GetProcessHeap(), 0, pFileInfo);
	return sOut;
}
// ---------------------------------------------------------------------------

void __fastcall TAboutBox::FormCreate(TObject *Sender)
{
    // by zzang9un 2013.04.03 : 프로그램 정보를 얻어온다.
	ProgramName->Caption = VersionInfo("FileDescription");
	Version->Caption = "Version : " + VersionInfo("FileVersion");
	Copyright->Caption = VersionInfo("LegalCopyright");
	CompanyName->Caption = VersionInfo("CompanyName");
}
//---------------------------------------------------------------------------

