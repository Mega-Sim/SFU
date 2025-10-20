//---------------------------------------------------------------------------

#ifndef AutoBayTestMapViewLoadH
#define AutoBayTestMapViewLoadH
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


#include "include/Def_TP.h"
#include "../Common/PUtily.h"
#include "../Common/Protocol_Define_Modify.h"
//---------------------------------------------------------------------------
class Tfrm_MapViewLoad : public TForm
{
__published:	// IDE-managed Components
	TButton *btnMapViewLoad;
	TButton *btnSimulation;
	TLabel *lblMapVersion;
	TLabel *lblStatus;
	TEdit *edtMapVersion;
	TEdit *edtStatus;
	TTimer *tmrUDPTimerMapview;
	void __fastcall tmrUDPTimerMapviewTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnMapViewLoadClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnSimulationClick(TObject *Sender);
private:	// User declarations

	char mapVersion[20];
	int m_nMapViewStatus; //

	int  m_nMode;
	int  m_nRowSize;
	int  m_nIndex;
	int  m_nCurrentStep;
	bool m_bSequenceMove;
	bool m_bSimulation;

	int  m_nUIUpdateStatus; 	//0.대기, 1.요청중, 2.응답완료
	int  m_nUIUpdateResult;

//문자열 자르기
	char* tok;
	bool bFirst;
	bool bEnd;
	AnsiString SplitStr;
	AnsiString strTmpBuffer;
	char chBuffer[1024];
	int nPrePos;
	int nPos;


public:		// User declarations
	__fastcall Tfrm_MapViewLoad(TComponent* Owner);
	void Warnning_Display(TColor, String);

	void setMapViewStatus(int nStatus);
	void setMapVersion(char* buf);
	void setUIUpdateResult(int nStatus, int nResult);

	void excuteSimulation();
};
//---------------------------------------------------------------------------
extern PACKAGE Tfrm_MapViewLoad *frm_MapViewLoad;
//---------------------------------------------------------------------------
#endif
