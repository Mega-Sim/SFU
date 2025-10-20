//---------------------------------------------------------------------------

#ifndef AutoBayTestUnitH
#define AutoBayTestUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ExtDlgs.hpp>
#include <Grids.hpp>
#include <mmsystem.h>


#include <Buttons.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>

//---------------------------------------------------------------------------

#include "include/Def_TP.h"
#include "../Common/PUtily.h"
#include "../Common/Protocol_Define_Modify.h"

#define MAX_DATA 500

//Column 정의
#define			NO						0
#define			START_NODE          	1
#define			END_NODE            	2
#define			ROUTE               	3
#define			OFFSET					4
#define			STATUS              	5
#define			RESULT              	6


//주행 모드 정의
#define		 NONE_MODE 					0
#define		 NODE_MODE 					1
#define		 STATION_MODE	 			2
#define	 	 LINK_MODE					3


//스텝 정의
#define		WAIT_STEP  					0
#define		ROUTE_STEP 					1
#define		REQ_MOVE_STEP 				2
#define		CHECK_MOVE_STEP 			3
#define		CHECK_COMPLETE_STEP 		4


//주행중 체크
#define		DRIVING						0
#define		ARRIVED_NEXTSTEP			1
#define		ARRIVED_END					2
#define		DRIVING_FAIL				3



class Tfrm_AutoBayTest : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblMode;
	TLabel *lblStatus;
	TLabel *lblIndex;
	TLabel *lblProgress;
	TButton *btnRouteRequest;
	TButton *btnStartButton;
	TButton *btnStartFromIndex;
	TButton *btnSequenceStop;
	TButton *btnEStop;
	TButton *btnInitialization;
	TButton *btnRouteReset;
	TEdit *edtRouteIndex;
	TProgressBar *pbLongrun_per;
	TStringGrid *strngrdRouteTable;
	TTimer *tmrAutoBayLongRunTimer;
	TLabel *lblMessage;
	TMemo *mmoLongRunResult;
	TButton *btnDataSave;
	TLabel *lblTestStatus;
	TShape *shp_Status;
	TEdit *edtMode;
	TButton *btnMoveStartOneRoute;
	TButton *btnSimulation;
	TLabel *lblLongrun;
	TCheckBox *chkLongrun;
	TEdit *edtCountSet;
	void __fastcall tmrAutoBayLongRunTimerTimer(TObject *Sender);
	void __fastcall btnRouteRequestClick(TObject *Sender);
	void __fastcall btnInitializationClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnDataSaveClick(TObject *Sender);
	void __fastcall btnStartButtonClick(TObject *Sender);
	void __fastcall btnMoveStartOneRouteClick(TObject *Sender);
	void __fastcall strngrdRouteTableSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall btnSequenceStopClick(TObject *Sender);
	void __fastcall btnStartFromIndexClick(TObject *Sender);
	void __fastcall btnRouteResetClick(TObject *Sender);
	void __fastcall btnEStopClick(TObject *Sender);
	void __fastcall btnSimulationClick(TObject *Sender);
	void __fastcall chkLongrunClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations

	int 	m_nRowCount;
	int 	m_nMode;
	int     m_nPreMode;
	int 	m_nCurrentStep;
	int 	m_nIndex;
	int 	m_nCount;
	int 	m_nSelectedRowNum;
	int 	m_nStartIDFromIndex;
	int 	m_nLongrunCurrentCount;
	int 	m_nLongrunSetCount;

	double  m_dPreOffset;


	bool 	m_bRouteCheckSignal;
	bool 	m_bRouteCheckResult;
	bool 	m_bDriveCheckSignal;
	bool 	m_bDriveCheckResult;
	bool 	m_bMoveCheck;

	bool	m_bSequenceMove;
	bool 	m_bLongrun;

	bool 	m_bSimulationFlag;
	bool 	m_bSimulationReady;
	bool 	m_bSimulationReq;   // 시연용 추후 수정예정
	int		m_nSimulationIndex; // 시연용 추후 수정예정


public:		// User declarations
	__fastcall Tfrm_AutoBayTest(TComponent* Owner);


	void 	InitData();
	void    setMode(int nMode);
	void    setStep(int nCurrentStep);
	void 	setRouteCheckSignalTrue();
	void 	setRouteCheckResult(bool bResult);
	void 	setDriveCheckSignalTrue();
	void 	setDriveCheckResult(bool bResult);
	void 	setSimulationReady(bool bResult);
	void    excuteSimulation();
	bool 	ReadRouteList();
	bool 	MoveRequest();
	int 	MoveCheck();

	int 	getMode();
	int 	getCurrentStep();
	int 	getRowSize();
	bool 	getSequenceMove();

	void Warnning_Display(TColor, String);

};
//---------------------------------------------------------------------------
extern PACKAGE Tfrm_AutoBayTest *frm_AutoBayTest;
//---------------------------------------------------------------------------
#endif
