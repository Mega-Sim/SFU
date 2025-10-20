//---------------------------------------------------------------------------

#ifndef JigShutterUnitH
#define JigShutterUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TShutterUnit : public TForm
{
__published:	// IDE-managed Components
    TStaticText *txtShutterUnit;
    TCheckBox *chkShutterLongrun;
    TButton *btnShutterInit;
    TButton *btnShutter_Run;
    TProgressBar *pb_Shutter;
    TPanel *pnl_ShutterReadyStatus;
    TPanel *pnl_ShutterBusy;
    TButton *btnShutter_Reset;
    TPanel *pnlIn_ShutterFOpen;
    TPanel *pnlIn_ShutterFClose;
    TStaticText *txtShutter_ctrl;
    TPanel *pnlIn_ShutterFAlarm;
    TPanel *pnlIn_ShutterRAlarm;
    TButton *btnOut_ShutterOpen;
    TButton *btnOut_ShutterClose;
    TButton *btnOut_AlarmReset;
    TStaticText *txtShutterSetting;
    TStaticText *txtShutterInterval;
    TStaticText *txtShutterCount;
    TStaticText *txtShutterRealCnt;
    TEdit *edtShutter_RealCount;
    TStaticText *txt_ErrMsg2;
    TEdit *edt_ShutterErrMsg;
    TEdit *edt_ShutterStepMsg;
    TStaticText *txtShutter_StepMsg;
    TButton *btnShutter_LoadParam;
    TEdit *edtShutter_Count;
    TEdit *edtShutter_Interval;
    TMemo *mmoShutter_Manual;
    TPanel *pnlIn_ShutterROpen;
    TPanel *pnlIn_ShutterRClose;
    TStaticText *Txt_StartTime;
    TEdit *edt_ShutterDate;
    TButton *btn_ShutterEStop;
    TTimer *tmrShutterUI;
    TPanel *pnlIn_ShutterInpos;
    TStaticText *Txt_Errtime;
    TEdit *edt_ShutterErrTime;
    void __fastcall btnShutterInitClick(TObject *Sender);
    void __fastcall btnShutter_ResetClick(TObject *Sender);
    void __fastcall btn_ShutterEStopClick(TObject *Sender);
    void __fastcall btnOut_ShutterOpenClick(TObject *Sender);
    void __fastcall btnOut_ShutterCloseClick(TObject *Sender);
    void __fastcall btnOut_AlarmResetClick(TObject *Sender);
    void __fastcall btnShutter_LoadParamClick(TObject *Sender);
    void __fastcall btnShutter_RunClick(TObject *Sender);
    void __fastcall chkShutterLongrunClick(TObject *Sender);
    void __fastcall tmrShutterUITimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall FormDestroy(TObject *Sender);

private:	// User declarations
public:		// User declarations
    __fastcall TShutterUnit(TComponent* Owner);

    void Init();
	void UpdateParam(bool bUI_toParam = true);
	void LoadParam();
	void SaveParam();

    bool bCtrlFlag; // Lonrun: 0, Manual: 1
    bool bStartFlag; // Stop: 0, Start: 1


};
//---------------------------------------------------------------------------
extern PACKAGE TShutterUnit *ShutterUnit;
//---------------------------------------------------------------------------
#endif
