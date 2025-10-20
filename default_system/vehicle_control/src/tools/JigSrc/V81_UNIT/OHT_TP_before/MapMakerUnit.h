// ---------------------------------------------------------------------------

#ifndef MapMakerUnitH
#define MapMakerUnitH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>

#include "../Common/Def_DataType.h"
#include "../Common/Protocol_Define_Modify.h"

// ---------------------------------------------------------------------------
class TMapMakerForm : public TForm
{
__published: // IDE-managed Components

    TMemo *memoMapMaker;
    TPopupMenu *pmMemo;
    TMenuItem *mniMemoClear;
	TTimer *tmrUI;
	TPageControl *pgcMapMake;
	TTabSheet *MapMake;
	TTabSheet *주행검증;
	TTabSheet *E305_Link거리수정;
	TShape *shpCycle;
	TLabel *Label11;
	TShape *ShpIsTargetNode;
	TLabel *Label13;
	TShape *ShaAdjustMap;
	TLabel *Label9;
	TShape *shpDrivingMarked;
	TLabel *Label15;
	TShape *shpCIDPause;
	TLabel *Label17;
	TEdit *edtMoveEnd;
	TLabel *Label87;
	TButton *btnMapMake;
	TLabel *Label12;
	TLabel *LabMapMakeMoveCount;
	TLabel *LabMapMakeCount;
	TLabel *Label3;
	TLabel *Label8;
	TProgressBar *ProgressBar1;
	TButton *BtnMapMakeMoveCheck;
	TButton *Button1;
	TShape *shpCIDPause2;
	TLabel *Label18;
	TShape *shpDrivingMarked2;
	TLabel *Label16;
	TShape *ShaAdjustMapModify;
	TLabel *Label10;
	TShape *ShaAdjustMap2;
	TLabel *Label6;
	TShape *shpCycle2;
	TLabel *Label5;
	TLabel *Label2;
	TEdit *edtStartNodeID;
	TLabel *Label4;
	TEdit *edtEndNodeID;
	TLabel *Label1;
	TEdit *edtMapMakerDist;
	TButton *btnReq;
	TButton *btnAutoMapStart;
	TButton *btnSolution;
	TLabel *lblpercent;

    void __fastcall FormShow(TObject *Sender);
    void __fastcall btnAutoMapStartClick(TObject *Sender);
    void __fastcall mniMemoClearClick(TObject *Sender);
	void __fastcall btnReqClick(TObject *Sender);
	void __fastcall tmrUITimer(TObject *Sender);
	void __fastcall btnMapMakeClick(TObject *Sender);
	void __fastcall BtnMapMakeMoveCheckClick(TObject *Sender);
	void __fastcall btnSolutionClick(TObject *Sender);

private: // User declarations

public : // User declarations
    __fastcall TMapMakerForm(TComponent* Owner);

    // by zzang9un 2013.07.23 : UI 관련 함수
	void DisplayMapMakerInfo(uTP_CMD MapInfo);
	void DisplayDrivingMapInfo(PKT_OHT2TP_DRIVINGMAP_INFO* PACKET);
	void Warnning_Display(TColor tWColor, String strData);
	void Display_MapMake_Result(PKT_OHT2TP_LOG_DATA PKT);
};

// ---------------------------------------------------------------------------
extern PACKAGE TMapMakerForm *MapMakerForm;
// ---------------------------------------------------------------------------
#endif
