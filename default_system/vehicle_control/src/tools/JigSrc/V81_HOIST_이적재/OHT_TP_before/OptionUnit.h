//---------------------------------------------------------------------------

#ifndef OptionUnitH
#define OptionUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
//---------------------------------------------------------------------------
class TOptionForm : public TForm
{
__published:	// IDE-managed Components
    TTreeView *tvOption;
    TSplitter *splOption;
	TPageControl *PageControl1;
	TTabSheet *LogSheet;
	TTabSheet *EtcSheet;
	TValueListEditor *LogValueListEditor1;
	void __fastcall tvOptionClick(TObject *Sender);
	void __fastcall LogValueListEditor1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

private:	// User declarations
public:		// User declarations
    __fastcall TOptionForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TOptionForm *OptionForm;
//---------------------------------------------------------------------------
#endif
