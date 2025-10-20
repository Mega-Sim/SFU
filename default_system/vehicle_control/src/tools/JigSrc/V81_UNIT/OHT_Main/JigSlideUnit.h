//---------------------------------------------------------------------------

#ifndef JigSlideUnitH
#define JigSlideUnitH
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
class TSlideUnit : public TForm
{
__published:	// IDE-managed Components

//Slide 관련 UI

    TStaticText *SlideUnit;
	TEdit *edtSlide_moveDist;
	TProgressBar *progress_Slide;
	TStaticText *txt4;
	TButton *btnSlideAxis_Run;
	TEdit *edtSlide_Interval;
	TStaticText *txt5;
	TEdit *edtSlide_Count;
	TStaticText *txt6;
	TTimer *tmr1;
	TEdit *edt_SlideErrMsg;
	TStaticText *txt_ErrMsg1;
	TButton *btn_SlideReset;
    TButton *Slide_LoadParam;
    TButton *Slide_EStop;
	TButton *btn_Servo_On;
	TButton *btn_Servo_Off;
	TPanel *pnl_SlideServoStat;
	TPanel *pnl_SlideBusy;
	TButton *btn_SlideHome;
    TEdit *edt_SlideStepMsg;
    TStaticText *Slide_StepMsg;
	TStaticText *txt16;
	TPanel *pnl_SlideCurPos;
	TButton *btn_SlideSetHome;
	TPanel *pnl_SlideVel;
	TStaticText *txt17;
	TButton *btn_SlideJog_P;
	TButton *btn_SlideJogM;
	TEdit *edt_RelMoveVal;
    TCheckBox *chkSlidelongrun;
	TEdit *edt_Slide_RealCount;
    TStaticText *RealtimeCount;
    TStaticText *setting;
    TMemo *Slide_Manual;
	TEdit *edtStartDate;
	TEdit *edtEndDate;
	TPanel *pnl_SlideReady;
	TStaticText *txt1;
	TStaticText *txt2;
	TStaticText *txt3;
	TButton *btnSlide_Main_Reset;
	TStaticText *txt7;
	TButton *Slide_SaveParam;

//Slide 관련 함수

	void __fastcall btnSlideAxis_RunClick(TObject *Sender);
	void __fastcall tmr1Timer(TObject *Sender);
	void __fastcall btn_SlideResetClick(TObject *Sender);
	void __fastcall Slide_LoadParamClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Slide_EStopClick(TObject *Sender);
	void __fastcall btn_Servo_OnClick(TObject *Sender);
	void __fastcall btn_Servo_OffClick(TObject *Sender);
	void __fastcall btn_SlideHomeClick(TObject *Sender);
	void __fastcall btn_SlideSetHomeClick(TObject *Sender);
	void __fastcall btn_SlideJog_PClick(TObject *Sender);
	void __fastcall btn_SlideJogMClick(TObject *Sender);
    void __fastcall btn_LongrunClick(TObject *Sender);
	void __fastcall btnSlide_Main_ResetClick(TObject *Sender);
	void __fastcall Slide_SaveParamClick(TObject *Sender);

private:
	LogUnit* pLogJig; // Jig Log 생성 변수
	CmdManager *m_pCmdManager;       ///< Command Manager Class

public:
	void Init();
	void UpdateParam(bool bUI_toParam = true);
	void LoadParam();
	void SaveParam();

	SYSTEMTIME CurTime;
	double     dValue;


public:		// User declarations
	__fastcall TSlideUnit(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSlideUnit *SlideUnit;
//---------------------------------------------------------------------------
#endif
