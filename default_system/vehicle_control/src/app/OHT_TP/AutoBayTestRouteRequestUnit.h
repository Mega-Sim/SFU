//---------------------------------------------------------------------------

#ifndef AutoBayTestRouteRequestUnitH
#define AutoBayTestRouteRequestUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include "AutoBayTestUnit.h"
#include "include/Def_TP.h"
#include "../Common/Protocol_Define_Modify.h"
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
//---------------------------------------------------------------------------
class Tfrm_RouteRequest : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRouteRequest;
	TComboBox *cbbModeSelect;
	TEdit *edtLinkCount;
	TStringGrid *strngrdLaneCutList;
	TLabel *lblModeSelect;
	TLabel *lblLinkCount;
	TButton *btnInitialization;
	TButton *btnFileLoad;
	TOpenTextFileDialog *dlgOpenTextFileDig_LaneCut;
	void __fastcall btnRouteRequestClick(TObject *Sender);
	void __fastcall cbbModeSelectClick(TObject *Sender);
	void __fastcall btnInitializationClick(TObject *Sender);
	void __fastcall strngrdLaneCutListKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall strngrdLaneCutListGetEditMask(TObject *Sender, int ACol, int ARow,
		  UnicodeString &Value);
	void __fastcall btnFileLoadClick(TObject *Sender);


private:	// User declarations


	int m_nMode;
public:		// User declarations
	__fastcall Tfrm_RouteRequest(TComponent* Owner);
	void Warnning_Display(TColor, String);
	bool LaneCutListFileLoad(String strTmpFileName);
};
//---------------------------------------------------------------------------
extern PACKAGE Tfrm_RouteRequest *frm_RouteRequest;
//---------------------------------------------------------------------------
#endif
