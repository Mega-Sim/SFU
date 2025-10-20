//---------------------------------------------------------------------------

#ifndef DebugUnitH
#define DebugUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <CheckLst.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TDebugForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
   TPanel *Panel2;
   TEdit *DebugEdit;
   TCheckListBox *DebugListB;
   TPopupMenu *PopupMenu1;
   TMenuItem *Clear1;
   void __fastcall FormShow(TObject *Sender);
   void __fastcall DebugEditKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
   void __fastcall DebugListBDblClick(TObject *Sender);
   void __fastcall Clear1Click(TObject *Sender);
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
   void __fastcall DebugListBClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TDebugForm(TComponent* Owner);
   void DataDisplay(String);
   int nClearLineNum;
};
//---------------------------------------------------------------------------
extern PACKAGE TDebugForm *DebugForm;
//---------------------------------------------------------------------------
#endif
