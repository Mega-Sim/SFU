//---------------------------------------------------------------------------

#ifndef BayTestUnitH
#define BayTestUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include "include/Def_OHT.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "../Common/Protocol_Define_Modify.h"

#define MAX_DATA 500

#define STATUS_STOP -1
#define STATUS_PASS 0
#define STATUS_WAIT 1
#define STATUS_COMPLETE 2

namespace BAYTEST_MODE
{
    enum MODE
    {
        NONE_MODE = 0,
        ONCE_MODE,
        REPEAT_MODE,
		SEQUENTIAL_MODE,
        RANDOM_MODE,
    };
};

namespace BAYTEST_STEP
{
    enum STATE
    {
		INIT_STEP = 0,
		MOVE_STEP,
		CHECK_MOVE_STEP,
		TRANS_LOAD_STEP,
		CHECK_TRANS_LOAD_STEP,
		TRANS_UNLOAD_STEP,
		CHECK_TRANS_UNLOAD_STEP,
		AUTO_TEACHING_STEP,
		CHECK_AUTO_TEACHING_STEP,
		CHECK_COMPLETE_STEP,
    };
};

//---------------------------------------------------------------------------
class Tfrm_Bay_Test : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
	TComboBox *TestModeBox;
    TLabel *Label2;
	TComboBox *ExcuteModeBox;
	TButton *btn_LongRunStart;
	TButton *btn_LongRunStop;
	TMemo *mmLongRunResult;
	TStringGrid *strGridLongrunInfor;
    TOpenTextFileDialog *OpenTextFileDig_Longrun;
    TButton *Btn_OpenLongrunFile;
    TTimer *BayLongRunTimer;
	TProgressBar *Longrun_per;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label_TestStatus;
	TShape *Shape_Status;
	void __fastcall Btn_OpenLongrunFileClick(TObject *Sender);
	void __fastcall btn_LongRunStartClick(TObject *Sender);
	void __fastcall btn_LongRunStopClick(TObject *Sender);
	void __fastcall BayLongRunTimerTimer(TObject *Sender);
	void __fastcall TestModeBoxClick(TObject *Sender);
	void __fastcall ExcuteModeBoxClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormDestroy(TObject *Sender);
  //	void __fastcall strGridLongrunInforClick(TObject *Sender);
  //	void __fastcall btn_OCSBase_LongrunStartClick(TObject *Sender);


private:	// User declarations
	bool    bTestFileOK;
	int     nUseTrans[MAX_DATA];
	bool	bUseAutoTeaching[MAX_DATA];
	bool    bTestStaus[MAX_DATA];
	int     nStationID[MAX_DATA];
	int     nTestCnt[MAX_DATA];
	int     nCount;
	int 	nCnt;
	int     Step;
	char 	bModeType;
	char 	bExecuteType;
	BYTE    PortType;
	int LongRunFileCheck(String strTmpFileName);
	void InitData(void);
	void InitTestResult(bool bAllInit);
	void MoveCMD(int Start, int End, bool bNode = false);
	void TransCMD(int Type);
	void MakeTransPkt(PKT_TP2OHT_TRANS_LOAD_UNLOAD *pTrans);
	void Tfrm_Bay_Test::Auto_Teaching_CMD(void);
	int Init_Step(void);
	int Move_Step(void);
	int Check_Move_Step(bool bNode = false);
	int Check_Complete_Step(void);
	int Trans_Load_Step(void);
	int Check_Trans_Load_Step(void);
	int Trans_UnLoad_Step(void);
	int Check_Trans_UnLoad_Step(void);
	int Auto_Teaching_Step(void);
	int Check_Auto_Teaching_Step(void);
	void StatusDisplay(void);

public:		// User declarations
    __fastcall Tfrm_Bay_Test(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tfrm_Bay_Test *frm_Bay_Test;
//---------------------------------------------------------------------------
#endif
