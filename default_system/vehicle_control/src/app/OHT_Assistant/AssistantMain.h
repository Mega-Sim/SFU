// ----------------------------------------------------------------------------
// Borland C++Builder
// Copyright (c) 1987, 1998 Borland International Inc. All Rights Reserved.
// ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#ifndef AssistantMainH
#define AssistantMainH
// ---------------------------------------------------------------------------
#include <Forms.hpp>
#include <StdCtrls.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Graphics.hpp>
#include <Dialogs.hpp>
#include <ImgList.hpp>

#include "Utility.h"
#include "VersionInfo.h"
#include "MonitoringUtil.h"
#include "SocketUnit.h"


#define MAX_BYTE_SIZE_OF_LOG    1024*1024

#define MYWM_NOTIFY         (WM_APP+100)
#define IDC_MYICON                     1006

#define FILE_PATH_AST_PARAM		"./DataFiles\\AssistantParam.ini"
#define AS_VER_FILE_PATH		"C:\\User\\OHT\\DataFiles\\AsInfo.dat"

extern HINSTANCE g_hinst;
LRESULT IconDrawItem(LPDRAWITEMSTRUCT lpdi);

// ---------------------------------------------------------------------------
// 통신할 자료형 선언
// ---------------------------------------------------------------------------
typedef struct DataInfo
{
	int nIsOHT;
	TCHAR chData[100];
} DINFO, *PINFO;

struct ASSISTANT_PARAM
{
	int nFileMonitoringInterval;
	int nMaxWaitTimeToDelete;
	int nLogSaveMode;
	AnsiString strLogPath;
};

// ---------------------------------------------------------------------------

class TAssistantMainForm : public TForm
{
__published:
    TPopupMenu *popupTrayIcon;
    TMenuItem *mniAbout;
    TMenuItem *mniExit;
	TMenuItem *mniOHTExec;
    TMenuItem *mniOHTClose;
    TTimer *ExceptCheckTimer;
    TTimer *LogDriveTimer;
    TImageList *ImageListTrayIcon;
    TTrayIcon *TrayIconAssistant;
    TMenuItem *mniN2;
	TMenuItem *mniFBWF;
	TMenuItem *mniN1;
	TMenuItem *N1;
	TMenuItem *mnuComLogView;
	TMenuItem *mnuComLogHide;
	TTimer *OHTMoniteringtimer;

    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall mniAboutClick(TObject *Sender);
    void __fastcall mniExitClick(TObject *Sender);
    void __fastcall mniOHTExecClick(TObject *Sender);
	void __fastcall mniOHTCloseClick(TObject *Sender);

	void __fastcall ExceptCheckTimerTimer(TObject *Sender);
	void __fastcall LogDriveTimerTimer(TObject *Sender);
	void __fastcall mniFBWFClick(TObject *Sender);
	void __fastcall mnuComLogViewClick(TObject *Sender);
	void __fastcall mnuComLogHideClick(TObject *Sender);
	void __fastcall MonitoringOHT(TObject *Sender);

private: // private user declarations
	int m_FBWFStatus;

	TIniFile *m_IniFile;
	#ifdef IS_REAL_OHT
	LogUnit *pLogUnit;
	#endif

public: // public user declarations
	String g_strOHTVer;
	String g_strLogPath;

	int g_nIsOHTBoot;
	bool g_bIsEnableLogSave;
	void LoadAssistantParam();

	// by zzang9un 2013.10.10
	OHTLogFile *m_Log;     ///< Log 저장을 위한 클래스

	virtual __fastcall TAssistantMainForm(TComponent *Owner);

	PROCESS_INFORMATION pinfo;
	HWND hhFrame;

	ASSISTANT_PARAM m_AstParam;

	//2013.12.16 doori.shin Monitoring관련 함수들
	//주의! 자주 호출하게되면 리소스 과부하 문제가 될 수 있음
	int GetFBWFStatus();
	int SetFBWFStatus(int status);

	// shkim. 2016.12.12 버전읽기 기능을 위해 추가
	TVersionInfo VerInfo;

protected:
	virtual void __fastcall DeviceChangeProc(Messages::TMessage &Message);
};

// ---------------------------------------------------------------------------
extern TAssistantMainForm *AssistantMainForm;
// ---------------------------------------------------------------------------
#endif
