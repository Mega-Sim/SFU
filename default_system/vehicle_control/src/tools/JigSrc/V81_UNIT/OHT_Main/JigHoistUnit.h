//---------------------------------------------------------------------------

#ifndef JigHoistUnitH
#define JigHoistUnitH
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
class THoistUnit : public TForm
{
__published:	// IDE-managed Components
	TStaticText *txtsetting;
	TStaticText *txtHoistUnit;
	TMemo *mmoHoist_Manual;
	TEdit *edt_HoistStepMsg;
	TStaticText *txtHoistPos;
	TStaticText *txt5;
	TStaticText *txt6;
	TEdit *edt_HoistErrMsg;
	TEdit *edt_HoistRealCount;
	TStaticText *txtHoistStepMsg;
	TStaticText *txtHoistErrMsg;
	TStaticText *RealtimeCount;
	TProgressBar *progress_Hoist;
	TButton *btnHoistAxis_Run;
	TButton *btn_HoistReset;
	TButton *btn_Servo_On;
	TButton *btn_Servo_Off;
	TPanel *pnl_HoistServoStat;
	TPanel *pnl_HoistBusy;
	TButton *btn_HoistHome;
	TStaticText *txt16;
	TPanel *pnl_HoistCurPos;
	TButton *btnHoistSetHome;
	TPanel *pnl_HoistVel;
	TStaticText *txt17;
	TButton *btn_HoistJog_P;
	TButton *btn_HoistJog_N;
	TEdit *edt_RelMoveVal;
	TCheckBox *chkHoistLongrun;
	TEdit *edt_HoistMoveDist;
	TEdit *edt_HoistInterval;
	TEdit *edt_HoistCount;
	TButton *btnHoist_LoadParam;
	TButton *btnHoist_EStop;
	TPanel *pnl_HoistHome;
	TTimer *tmr1;
	TStaticText *txtProfile;
	TComboBox *cbbHoistProfile;
	TStaticText *txtDate;
	TStaticText *txtEndDate;
	TEdit *edtStartDate;
	TEdit *edtEndDate;
	TButton *btnHoist_Main_Reset;
	TPanel *pnl_HoistReady;
	TStaticText *txt1;
	TStaticText *txt2;
	TButton *Save_Param;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnHoist_LoadParamClick(TObject *Sender);
	void __fastcall btnHoistAxis_RunClick(TObject *Sender);
	void __fastcall btn_Servo_OnClick(TObject *Sender);
	void __fastcall btn_Servo_OffClick(TObject *Sender);
	void __fastcall btn_HoistResetClick(TObject *Sender);
	void __fastcall tmr1Timer(TObject *Sender);
	void __fastcall chkHoistLongrunClick(TObject *Sender);
	void __fastcall btnHoist_EStopClick(TObject *Sender);
	void __fastcall btnHoistSetHomeClick(TObject *Sender);
	void __fastcall btn_HoistHomeClick(TObject *Sender);
	void __fastcall btn_HoistJog_PClick(TObject *Sender);
	void __fastcall btn_HoistJog_NClick(TObject *Sender);
	void __fastcall btnHoist_Main_ResetClick(TObject *Sender);
	void __fastcall Save_ParamClick(TObject *Sender);

private:	// User declarations
	LogUnit* pLogJig; // Jig Log 생성 변수
	CmdManager *m_pCmdManager;       ///< Command Manager Class

public:		// User declarations
	__fastcall THoistUnit(TComponent* Owner);

public:
	void Init();
	void UpdateParam(bool bUI_toParam = true);
	void LoadParam();
	void SaveParam();
	SYSTEMTIME CurTime;
	double     dValue;
	// by zzang9un 2013.03.20 : 현재 시간 얻어오기

};
//---------------------------------------------------------------------------
extern PACKAGE THoistUnit *HoistUnit;
//---------------------------------------------------------------------------
#endif
