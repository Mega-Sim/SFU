//---------------------------------------------------------------------------

#ifndef DisUnitH
#define DisUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TDisForm : public TForm
{
__published:	// IDE-managed Components
   TMemo *Memo1;
   TSaveDialog *SaveDialog1;
   TMainMenu *MainMenu1;
   TMenuItem *Save1;
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
   void __fastcall Save1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TDisForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDisForm *DisForm;
//---------------------------------------------------------------------------
#endif
