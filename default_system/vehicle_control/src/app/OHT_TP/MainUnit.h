//---------------------------------------------------------------------------

#ifndef MainUnitH
#define MainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>
#include <FileCtrl.hpp>
#include <Dialogs.hpp>

#include "Def_Protocol.h"
#include "Def_TPCommand.h"
#include "VersionInfo.h"
#include "Chart.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include "WaitUnit.h"

#include "Protocol_Define_Modify.h"
//---------------------------------------------------------------------------
class TPLAYBACK
{
private:
public :
   TPLAYBACK();
   ~TPLAYBACK();
   int nCount_TrackBar;

   int nCount_Communication;
   int nCount_HIDData;
   int nCount_Input1;
   int nCount_Input2;
   int nCount_Output1;
   int nCount_Output2;
   int nCount_Max;

   TStringList *tList_Communication;
   TStringList *tList_HIDData;
   TStringList *tList_Input1;
   TStringList *tList_Input2;
   TStringList *tList_Output1;
   TStringList *tList_Output2;
   TStringList *tList_Sum;

   int Init();
};

class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *mmMain;
	TMenuItem *mniLogin;
	TMenuItem *mniFile;
	TMenuItem *mniControl;
	TMenuItem *mniInfomation;
	TMenuItem *mniConnect;
	TMenuItem *mniDisconnect;
    TMenuItem *N1;
	TMenuItem *mniExit;
	TPopupMenu *popupMemoMain;
	TMenuItem *pmnuMemoClear;
	TMenuItem *mniFileUpDownload;
	TMenuItem *mniYES;
    TTimer *timerStatusRequest;
	TMenuItem *mniCommunication;
    TMenuItem *N3;
	TMenuItem *mniIOMap;
    TTimer *timerCommCheck;
    TPanel *panQuickCmd;
    TBitBtn *btnEStop;
    TBitBtn *btnReset;
    TBitBtn *btnManual;
    TBitBtn *btnAuto;
	TMenuItem *mniOnlineControl;
	TMenuItem *mniDebug;
    TPanel *panMain;
	TRichEdit *memoMain;
    TPanel *ppanVer;
    TPanel *panLineName;
    TPanel *panVer;
    TPanel *panCommFail;
    TLabel *lblCommFail;
    TProgressBar *pbCommStatus;
	TMenuItem *mniPIODisplay;
	TMenuItem *mniETC;
	TMenuItem *mniOHTSysInfo;
    TMenuItem *N5;
	TMenuItem *mniIOControl;
	TMenuItem *mniVelGraph;
    TMenuItem *N6;
	TPopupMenu *popupLogSave;
    TMenuItem *Cancel1;
    TMenuItem *N9;
	TMenuItem *mniTeachingWindow;
    TMenuItem *N8;
	TMenuItem *mniLogDownload;
    TMenuItem *N10;
	TMenuItem *mniUnitConvert;
    TMenuItem *N14;
	TMenuItem *mniAMCParameter;
	TMenuItem *mniSterringCheck;
	TMenuItem *mniHoistParameter;
	TMenuItem *mniLogRun;
	TStatusBar *StatusBarMain;
	TMenuItem *mniAMCDebug;
    TStatusBar *StatusBarVersionInfo;
	TMenuItem *mniAPSignalDisplay;
    TMenuItem *N16;
	TMenuItem *mniErrorSearch;
	TMenuItem *mniN13;
	TMenuItem *mniAbout;
    TMenuItem *mniN14;
    TMenuItem *mniUserModeChange;
    TTimer *timerUpdateDisplay;
	TMenuItem *Optionconfig1;
	TPanel *panDataStatus;
	TPanel *panFBWF;
    TPanel *panLogDirve;
	TPanel *panMap;
	TMenuItem *mniOHTAssistant;
    TTimer *timerAssistant;
    TPanel *panTrackbar;
	TTrackBar *trackbar;
	TLabel *lbl1;
	TLabel *lbl2;
	TLabel *lbl3;
	TMenuItem *mnuUDPDebugVisibleYes;
	TMenuItem *mnuUDPDebugVisibleNo;
	TTimer *tmrUdpListen;
	TMenuItem *N4;
	TMenuItem *mnFirmwareUpdate;
	TMenuItem *N7;
	TMenuItem *mnuOCSPause;
	TMenuItem *mnuOCSResume;
	TMenuItem *mnuTpPause;
	TMenuItem *mnuTpResume;
	TMenuItem *N11;
    TMenuItem *BayMovingTransTeest1;
	TMenuItem *mnOHT_Obstacle_SensorSetting;
	TMenuItem *mnDistanceSensorViewer;
	TMenuItem *mniDataTransper1;
	TMenuItem *mniCIDChecker2;
	TMenuItem *mniHIDLOGSave;
	TMenuItem *mniHIDVelGraphcts1;
	TMenuItem *mniHIDVelGraphsec1;
	TMenuItem *mniDrivingMapMake1;
	TMenuItem *mniVibration;
	TMenuItem *AVSViewer;
	TPanel *pnalWarning;
	TMenuItem *N2;
	TMenuItem *ErrorIgnoalFoupcoveroepn1;
	TMenuItem *ErrorResumeFoupcoveroepn1;
	TPanel *pnlPause_SetReset;
	TBitBtn *btnPause_SetReset;
	TMenuItem *mniAutoMovingTest;
	TMenuItem *mnWINShutdown;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall mniConnectClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall mniDisconnectClick(TObject *Sender);
    void __fastcall mniFileUpDownloadClick(TObject *Sender);
    void __fastcall mniYESClick(TObject *Sender);
    void __fastcall timerStatusRequestTimer(TObject *Sender);
    void __fastcall mniCommunicationClick(TObject *Sender);
    void __fastcall pmnuMemoClearClick(TObject *Sender);
    void __fastcall mniIOMapClick(TObject *Sender);
    void __fastcall timerCommCheckTimer(TObject *Sender);
    void __fastcall btnEStopClick(TObject *Sender);
    void __fastcall btnManualClick(TObject *Sender);
    void __fastcall btnAutoClick(TObject *Sender);
    void __fastcall btnResetClick(TObject *Sender);
    void __fastcall mniOnlineControlClick(TObject *Sender);
    void __fastcall mniPIODisplayClick(TObject *Sender);
    void __fastcall mniETCClick(TObject *Sender);
    void __fastcall mniOHTSysInfoClick(TObject *Sender);
    void __fastcall mniHIDClick(TObject *Sender);
    void __fastcall mniIOControlClick(TObject *Sender);
    void __fastcall mniVelGraphClick(TObject *Sender);
    void __fastcall mniTeachingWindowClick(TObject *Sender);
    void __fastcall StatusBarMainDblClick(TObject *Sender);
    void __fastcall Cancel1Click(TObject *Sender);
    void __fastcall NodeDistOffsetclear1Click(TObject *Sender);
    void __fastcall mniUnitConvertClick(TObject *Sender);
    void __fastcall mniAMCParameterClick(TObject *Sender);
    void __fastcall mniLogDownloadClick(TObject *Sender);
    void __fastcall mniHoistParameterClick(TObject *Sender);
    void __fastcall mniLogRunClick(TObject *Sender);
	void __fastcall btnPause_SetResetClick(TObject *Sender);
    void __fastcall mniAMCDebugClick(TObject *Sender);
    void __fastcall mniAPSignalDisplayClick(TObject *Sender);
    void __fastcall mniErrorSearchClick(TObject *Sender);
	void __fastcall mniAboutClick(TObject *Sender);
    void __fastcall mniUserModeChangeClick(TObject *Sender);
    void __fastcall timerUpdateDisplayTimer(TObject *Sender);
	void __fastcall Optionconfig1Click(TObject *Sender);
	void __fastcall mniOHTAssistantClick(TObject *Sender);
	void __fastcall timerAssistantTimer(TObject *Sender);
	void __fastcall trackbarChange(TObject *Sender);
	void __fastcall mnuUDPDebugVisibleYesClick(TObject *Sender);
	void __fastcall mnuUDPDebugVisibleNoClick(TObject *Sender);
	void __fastcall tmrUdpListenTimer(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall mnFirmwareUpdateClick(TObject *Sender);
	void __fastcall mnuOCSPauseClick(TObject *Sender);
	void __fastcall mnuOCSResumeClick(TObject *Sender);
	void __fastcall mnuTpPauseClick(TObject *Sender);
	void __fastcall mnuTpResumeClick(TObject *Sender);
    void __fastcall BayMovingTransTeest1Click(TObject *Sender);
	void __fastcall mnOHT_Obstacle_SensorSettingClick(TObject *Sender);
	void __fastcall mnDistanceSensorViewerClick(TObject *Sender);
	void __fastcall mniDataTransper1Click(TObject *Sender);
	void __fastcall mniCIDChecker2Click(TObject *Sender);
	void __fastcall mniHIDLOGSaveClick(TObject *Sender);
	void __fastcall mniHIDVelGraphcts1Click(TObject *Sender);
	void __fastcall mniHIDVelGraphsec1Click(TObject *Sender);
	void __fastcall mniDrivingMapMake1Click(TObject *Sender);
	void __fastcall mniSterringCheckClick(TObject *Sender);
	void __fastcall mnuTpAVSViewerClick(TObject *Sender);
	void __fastcall AVSViewerClick(TObject *Sender);
	void __fastcall ErrorIgnoalFoupcoveroepn1Click(TObject *Sender);
	void __fastcall ErrorResumeFoupcoveroepn1Click(TObject *Sender);
	void __fastcall mniAutoMovingTestClick(TObject *Sender);
	void __fastcall mnWINShutdownClick(TObject *Sender);

private:	// User declarations
    bool bExitFlag;
    void __fastcall WMEXITSIZEMOVE(TMessage & msg);

    // by zzang9un 2013.02.18 : Version 관리를 위한 클래스 추가
    TVersionInfo VerInfo;

protected:
    BEGIN_MESSAGE_MAP
        VCL_MESSAGE_HANDLER(WM_EXITSIZEMOVE, TMessage, WMEXITSIZEMOVE)
    END_MESSAGE_MAP(TForm)

public:		// User declarations
    String strMyPath;
    String g_strTitle;
    String g_Caption_Name;

    // StatusBarMain에서 사용되는 패널
    TPanel *panCommStatus;  ///< 통신 상태 표시 패널
    TPanel *panUser;        ///< User 상태 표시 패널
    TPanel *panSend;        ///< 패킷 송신 상태 표시 패널
    TPanel *panReceive;     ///< 패킷 수신 상태 표시 패널

    // StatusBarVersionInfo에서 사용되는 패널
    TPanel *panOHTVer;      ///< OHT Version 표시 패널
    TPanel *panAMCVer;      ///< AMC Version 표시 패널
    TPanel *panOHTType;      ///< OHT Type 표시 패널

    TTextAttributes *__fastcall CurrText(void);
    __fastcall TMainForm(TComponent* Owner);

    // by zzang9un 2013.03.26 : Statusbar를 Setting하는 함수
    void SettingStatusBar();

    void CloseAllForm();
    void CloseSubForm();
    void ShowMainMenu();
    void HideMainMenu();
    void ClassInit();
    void ClassClose();
    void DataDisplay(TColor TmpFontColor, String strDisplayData);
    void CommOK();
    void CommFail();
    void Warnning_Display(TColor tWColor, String strData);
    void MainForm_Exist();
    String Get_FileAccessTime(String strFileName);
	int CommDisconnect();

    // by zzang9un 2013.11.19 : 타이머 관련 함수
    void timerInit();
    void timerAllDisable();
    void timerAllEnable();

	// by ks4423 2013.12.02 : TP 로그 삭제
	void DeleteOldLogFile();

	//2013.08.16 doori.shin Flag whether IO Map is loaded
	bool fLoadIOMap;

	void StandardRead(PKT_OHT2OCS Pkt, int iMachineType);
	bool m_bIgnoreJungBokConnect;     // 중복접속 무시 플래그
	bool IsManualWorking();

	void SetMapVersion(String MapVer);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
