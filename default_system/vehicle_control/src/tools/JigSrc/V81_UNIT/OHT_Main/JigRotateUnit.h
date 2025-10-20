//---------------------------------------------------------------------------

#ifndef JigRotateUnitH
#define JigRotateUnitH
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
class TRotateUnit : public TForm
{
__published:	// IDE-managed Components
    TStaticText *txtRotateUnit;
    TCheckBox *chkRotateLongrun;
    TButton *btnRotateInit;
    TButton *btnRotate_Run;
    TProgressBar *pb_Rotate;
    TPanel *pnl_RotateReadyStatus;
    TPanel *pnl_RotateBusy;
    TButton *btnRotate_Reset;
	TPanel *pnlIn_Rotate90;
	TPanel *pnlIn_Rotate0;
    TStaticText *txtRotate_ctrl;
	TPanel *pnlIn_RotateAlarm;
	TButton *btnOut_Rotate0;
	TButton *btnOut_Rotate90;
	TButton *btnOut_Rotate180;
	TButton *btnOut_Rotate270;
    TButton *btnOut_AlarmReset;
    TStaticText *txtRotateSetting;
    TStaticText *txtRotateInterval;
    TStaticText *txtRotateCount;
    TStaticText *txtRotateRealCnt;
	TEdit *edt_Rotate_RealCount;
    TStaticText *txt_ErrMsg2;
    TEdit *edt_RotateErrMsg;
    TEdit *edt_RotateStepMsg;
    TStaticText *txtRotate_StepMsg;
    TButton *btnRotate_LoadParam;
    TEdit *edtRotate_Count;
    TEdit *edtRotate_Interval;
    TMemo *mmoRotate_Manual;
	TPanel *pnlIn_Rotate270;
	TPanel *pnlIn_Rotate180;
    TStaticText *Txt_StartTime;
    TEdit *edt_RotateDate;
    TButton *btn_RotateEStop;
    TTimer *tmrRotateUI;
    TPanel *pnlIn_RotateInpos;
    TStaticText *Txt_Errtime;
    TEdit *edt_RotateErrTime;
	TStaticText *txt1;
	TEdit *edtRotate_Current_Count;
	TButton *btnRotate_Main_Reset;
	TButton *Save_Param;

	void __fastcall btnRotateInitClick(TObject *Sender);
	void __fastcall btnRotate_ResetClick(TObject *Sender);
	void __fastcall btn_RotateEStopClick(TObject *Sender);
	void __fastcall btnOut_Rotate0Click(TObject *Sender);
	void __fastcall btnOut_Rotate90Click(TObject *Sender);
	void __fastcall btnOut_Rotate180Click(TObject *Sender);
	void __fastcall btnOut_Rotate270Click(TObject *Sender);
	void __fastcall btnOut_AlarmResetClick(TObject *Sender);
    void __fastcall btnRotate_LoadParamClick(TObject *Sender);
	void __fastcall btnRotate_RunClick(TObject *Sender);
	void __fastcall chkRotateLongrunClick(TObject *Sender);
	void __fastcall tmrRotateUITimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnRotate_Main_ResetClick(TObject *Sender);
//	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Save_ParamClick(TObject *Sender);

private:	// User declarations
	LogUnit* pLogJig; // Jig Log 생성 변수
	CmdManager *m_pCmdManager;       ///< Command Manager Class
public:		// User declarations
    __fastcall TRotateUnit(TComponent* Owner);

    void Init();
	void UpdateParam(bool bUI_toParam = true);
	void LoadParam();
	void SaveParam();
	SYSTEMTIME CurTime;


//    bool bCtrlFlag; // Lonrun: 0, Manual: 1
//	bool bStartFlag; // Stop: 0, Start: 1



};
//---------------------------------------------------------------------------
extern PACKAGE TRotateUnit *RotateUnit;
//---------------------------------------------------------------------------
#endif
