/**
@file    About.h
$Rev:: 771           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-03-28 1#$:  Date of last commit
@version OHT 7.0
@brief   프로그램 정보 표시를 위한 Form Header 파일
*/

//----------------------------------------------------------------------------
#ifndef AboutH
#define AboutH
//----------------------------------------------------------------------------
#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <Forms.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <jpeg.hpp>
//----------------------------------------------------------------------------
#define FILENAME_UPDATE	L"\\Assistant_Update.txt"
//----------------------------------------------------------------------------

/**
@class   TAboutBox
@date    2013.04.03
@author  zzang9un
@brief   Program 정보 표시를 위한 AboutBox
@note    프로그램 정보는 버전, 업데이트 정보는 update.txt 파일을 활용
@warning 업데이트 정보 파일은 실행파일과 동일한 위치에 위치하도록 한다.
*/
class TAboutBox : public TForm
{
__published:
	TPanel *panProgramInfo;
	TImage *ProgramIcon;
	TLabel *ProgramName;
	TLabel *Version;
	TLabel *Copyright;
	TLabel *CompanyName;
	TButton *OKButton;
	TPageControl *pgcAbout;
	TTabSheet *tabInfo;
	TTabSheet *tabUpdate;
	TRichEdit *redtUpdate;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);

private:
	String __fastcall VersionInfo(const String &sQuery); // 파일 정보 가져오는 함수

public:
	virtual __fastcall TAboutBox(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TAboutBox *AboutBox;
//----------------------------------------------------------------------------
#endif    
