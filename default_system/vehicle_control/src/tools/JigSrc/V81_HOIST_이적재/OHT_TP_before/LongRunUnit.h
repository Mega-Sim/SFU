// ---------------------------------------------------------------------------

#ifndef LongRunUnitH
#define LongRunUnitH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

// ---------------------------------------------------------------------------
class TLongRunForm : public TForm
{
__published: // IDE-managed Components

    TPanel *Panel1;
    TPanel *panHand;
    TPanel *panShutter;
    TPanel *panSteering;
    TButton *btnStart_Hand;
    TButton *btnStop_Hand;
    TButton *btnStart_Shutter;
    TButton *btnStop_Shutter;
    TButton *btnStart_Steering;
    TButton *btnStop_Steering;
    TTimer *HandTimer;
    TTimer *ShutterTimer;
    TTimer *SteeringTimer;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label7;
    TLabel *Label8;
    TLabel *Label9;
    TLabel *Label13;
    TLabel *Label14;
    TLabel *Label15;
    TMemo *Hand_Count_Memo;
    TMemo *Hand_Start_Time_Memo;
    TMemo *Hand_Stop_Time_Memo;
    TMemo *Shutter_Count_Memo;
    TMemo *Shutter_Start_Time_Memo;
    TMemo *Shutter_Stop_Time_Memo;
    TMemo *Steering_Start_Time_Memo;
    TMemo *Steering_Stop_Time_Memo;
    TMemo *SteeringL_Count_Memo;
    TMemo *SteeringR_Count_Memo;
    TLabel *Label16;
    TLabel *Label17;
    TGroupBox *grpTransAxisLongrun;
    TLabel *lbSpeed;
    TLabel *lbPosition;
    TLabel *lbDelay;
    TEdit *edtVelocity;
    TEdit *edtStartPosition;
    TEdit *edtDelay;
    TEdit *edtEndPosition;
    TLabel *lbEndPosition;
    TButton *btnStartShift;
    TButton *btnStopShift;
    TTimer *timerShift;
	TTimer *tmrRotate;
	TGroupBox *grpTransAxisLongrunRotate;
	TLabel *lbl2;
	TLabel *lbl3;
	TLabel *lbl4;
	TEdit *edtStartPositionRotate;
	TEdit *edtDelayRotate;
	TEdit *edtEndPositionRotate;
	TButton *btnStartRoate;
	TButton *btnStopRotate;
	TMemo *Rotate_Count_Memo;
	TLabel *lbl1;

	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

	void __fastcall btnStart_HandClick(TObject *Sender);
	void __fastcall HandTimerTimer(TObject *Sender);
	void __fastcall btnStop_HandClick(TObject *Sender);

	void __fastcall btnStart_ShutterClick(TObject *Sender);
	void __fastcall btnStop_ShutterClick(TObject *Sender);
	void __fastcall ShutterTimerTimer(TObject *Sender);

	void __fastcall btnStart_SteeringClick(TObject *Sender);
	void __fastcall btnStop_SteeringClick(TObject *Sender);
	void __fastcall SteeringTimerTimer(TObject *Sender);

	void __fastcall btnStartShiftClick(TObject *Sender);
	void __fastcall timerShiftTimer(TObject *Sender);
	void __fastcall btnStopShiftClick(TObject *Sender);
	void __fastcall btnStartRoateClick(TObject *Sender);
	void __fastcall btnStopRotateClick(TObject *Sender);
	void __fastcall tmrRotateTimer(TObject *Sender);


//private:         // User declarations
public : // User declarations
	__fastcall TLongRunForm(TComponent *Owner);

	int nHandCount, nRSteeringCount, nLSteeringCount, nShutterCount, nRotateCount;

	double dStartPosition;
	double dEndPosition;

	double dStartPositionRotate;
	double dEndPositionRotate;
	    // by zzang9un 2013.11.19 : Timer 관련 함수 추가
	void timerAllDisable();
};

// ---------------------------------------------------------------------------
extern PACKAGE TLongRunForm *LongRunForm;
// ---------------------------------------------------------------------------
#endif
