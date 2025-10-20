//---------------------------------------------------------------------------

#ifndef JigHandUnitH
#define JigHandUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "LogHandler.h"
#include "LogThread.h"
#include "CmdManager.h"


//---------------------------------------------------------------------------
class THandUnit : public TForm
{
__published:	// IDE-managed Components
    TStaticText *txtHandUnit;
    TCheckBox *chkHandLongrun;
    TButton *btnHandInit;
    TButton *btnHand_Run;
    TProgressBar *pb_Hand;
    TPanel *pnl_HandReadyStatus;
    TPanel *pnl_HandBusy;
    TButton *btnHand_Reset;
	TPanel *pnlIn_HandClose;
	TPanel *pnlIn_HandOpen;
    TStaticText *txtHand_ctrl;
	TPanel *pnlIn_HandAlarm;
    TButton *btnOut_HandOpen;
    TButton *btnOut_HandClose;
    TButton *btnOut_AlarmReset;
    TStaticText *txtHandSetting;
    TStaticText *txtHandInterval;
    TStaticText *txtHandCount;
    TStaticText *txtHandRealCnt;
    TEdit *edtHand_RealCount;
    TStaticText *txt_ErrMsg2;
    TEdit *edt_HandErrMsg;
    TEdit *edt_HandStepMsg;
    TStaticText *txtHand_StepMsg;
    TButton *btnHand_LoadParam;
    TEdit *edtHand_Count;
    TEdit *edtHand_Interval;
    TMemo *mmoHand_Manual;
    TStaticText *Txt_StartTime;
    TEdit *edt_HandDate;
    TButton *btn_HandEStop;
    TTimer *tmrHandUI;
    TPanel *pnlIn_HandInpos;
    TStaticText *Txt_Errtime;
    TEdit *edt_HandErrTime;
	TEdit *edtHand_Current_Count;
	TStaticText *txtCurrentCnt;
	TButton *btnHand_Main_Reset;
	TButton *Save_Param;
    void __fastcall btnHandInitClick(TObject *Sender);
    void __fastcall btnHand_ResetClick(TObject *Sender);
    void __fastcall btn_HandEStopClick(TObject *Sender);
    void __fastcall btnOut_HandOpenClick(TObject *Sender);
    void __fastcall btnOut_HandCloseClick(TObject *Sender);
    void __fastcall btnOut_AlarmResetClick(TObject *Sender);
    void __fastcall btnHand_LoadParamClick(TObject *Sender);
    void __fastcall btnHand_RunClick(TObject *Sender);
    void __fastcall chkHandLongrunClick(TObject *Sender);
    void __fastcall tmrHandUITimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall btnHand_Main_ResetClick(TObject *Sender);
	void __fastcall Save_ParamClick(TObject *Sender);

private:	// User declarations
	LogUnit* pLogJig; // Jig Log 생성 변수
	CmdManager *m_pCmdManager;       ///< Command Manager Class
public:		// User declarations
	__fastcall THandUnit(TComponent* Owner);

    void Init();
	void UpdateParam(bool bUI_toParam = true);
	void LoadParam();
	void SaveParam();
	SYSTEMTIME CurTime;


};
//---------------------------------------------------------------------------
extern PACKAGE THandUnit *HandUnit;
//---------------------------------------------------------------------------
#endif
