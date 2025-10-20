//---------------------------------------------------------------------------

#ifndef M2AUnitH
#define M2AUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TM2AForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
   TStringGrid *CheckONOFFGrid;
   TBitBtn *btnAutoOK;
   TBitBtn *BitBtn2;
   TPanel *Panel2;
   void __fastcall FormShow(TObject *Sender);
   void __fastcall CheckONOFFGridDrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
   void __fastcall btnAutoOKClick(TObject *Sender);
   void __fastcall BitBtn2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TM2AForm(TComponent* Owner);
   void IO_Display_Init();
   void IO_Display();
};
//---------------------------------------------------------------------------
extern PACKAGE TM2AForm *M2AForm;
//---------------------------------------------------------------------------
#endif
