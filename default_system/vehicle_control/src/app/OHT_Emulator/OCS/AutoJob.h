//---------------------------------------------------------------------------

#ifndef AutoJobH
#define AutoJobH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>

#include "Protocol_Define_Modify.h"


#include <ComCtrls.hpp>

/* 주의 : cboMajorCmd의 ItemIndex와, 이 커맨드 정의순서는 일치하여야 한다 */
namespace Sim_Major_Cmd
{
	enum Reserved
	{
	// 작업지시 관련 명령등
		MOVE_ON						= 0, // 기동
		MOVE_OFF					= 1, // 정지
		ORD_MOVE_NORMAL				= 2, // 작업지시(이동-정상)
		ORD_UP_NORMAL				= 3, // 작업지시(올림-정상)
		ORD_DN_NORMAL				= 4, // 작업지시(내림-정상)

		// 주의 : 이 부분에 01234 식으로 써 넣으면 컴파일러는 8진수로 해석해 버림!
	};
};


namespace Sim_Run_Cmd
{
	enum Reserved
	{
		RUN	 		= 0,
		PAUSE  		= 1,
		CONTINUE	= 2,
		STOP		= 3,
		RESPONSE	= 4,	// OHT에서 응답이 날아왔음
		RETRY		= 5,
		// 주의 : 이 부분에 01234 식으로 써 넣으면 컴파일러는 8진수로 해석해 버림!
	};
};
//---------------------------------------------------------------------------
class TfrmAutoJob : public TForm
{
__published:	// IDE-managed Components
	TStringGrid *FeederGrid;
	TPanel *Panel1;
	TButton *btnNew;
	TButton *btnSave;
	TButton *btnLoad;
	TOpenDialog *JobOpenDialog;
	TSaveDialog *JobSaveAsDialog;
	TPanel *Panel2;
	TButton *btnRun;
	TButton *btbPause;
	TButton *btnStop;
	TCheckBox *chkAutoOcsStatus;
	TButton *btnClose;
	TRadioGroup *rdoGrpPass;
	TRadioGroup *rdoGrpOpenReq;
	TComboBox *FWidthComboBox;
	TComboBox *cboMajorCmd;
	TButton *btnAdd;
	TButton *btnDelete;
	TRadioGroup *rdoTrunDefault;
	TButton *btnResume;
	TButton *Button1;
	TTimer *tmrCmdSend;
	TTimer *tmrOcsStatus;
	TEdit *edtCount;
	TLabel *Label1;
	TProgressBar *prgAutoSend;
	TButton *btnPassConfirmSet;
	TButton *btnOpenResponseSet;
	TPanel *Panel3;
	TEdit *edtFileName;
	TButton *btnViewStatus;
	TButton *Button2;
	TTimer *tmrRetry;
	TEdit *Edit1;
	TEdit *edtRunCount;
	TLabel *Label2;
	void __fastcall FeederGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall FWidthComboBoxChange(TObject *Sender);
	void __fastcall btnNewClick(TObject *Sender);
	void __fastcall btnAddClick(TObject *Sender);
	void __fastcall btnDeleteClick(TObject *Sender);
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall btbPauseClick(TObject *Sender);
	void __fastcall btnResumeClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall tmrCmdSendTimer(TObject *Sender);
	void __fastcall chkAutoOcsStatusClick(TObject *Sender);
	void __fastcall tmrOcsStatusTimer(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnSaveClick(TObject *Sender);
	void __fastcall btnLoadClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnPassConfirmSetClick(TObject *Sender);
	void __fastcall rdoGrpPassClick(TObject *Sender);
	void __fastcall rdoGrpOpenReqClick(TObject *Sender);
	void __fastcall btnOpenResponseSetClick(TObject *Sender);
	void __fastcall btnViewStatusClick(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall tmrRetryTimer(TObject *Sender);

private:	// User declarations
	void GridClear();
	void Minor_Cmd_AutoSet(int iMajorCmd);
	void GetRunningCmd(int iGrdIndex, int& iMajor, int& iMinor);
	int Send2OHT(int iGrdIndex);
	bool IsSendCmd(int iGrdIndex);
	void DisplayBlank(int iGrdIndex);

private:	// 이 모듈에서만 사용할 변수들
	int i_Cmd_Count;	// 현재 보유한 명령어 갯수
	PKT_OCS_STATUS_ID_01	PKT_OCS_STATUS;	// OCS 상태보고를 일관성있게 하기 위함

	AnsiString CurrentFileName;

	bool bOHT_NOW_RUNNING;


	//int i_Run_Position;
	//bool b_Pause;

public:		// User declarations
	__fastcall TfrmAutoJob(TComponent* Owner);
	void StatusMachine(int iCmd, PKT_OHT2OCS Pkt, int iTagID, int iPacketSize);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmAutoJob *frmAutoJob;
//---------------------------------------------------------------------------
#endif
