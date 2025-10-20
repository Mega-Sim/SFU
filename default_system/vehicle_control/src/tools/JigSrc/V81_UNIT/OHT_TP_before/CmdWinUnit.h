//---------------------------------------------------------------------------

#ifndef CmdWinUnitH
#define CmdWinUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TCmdWinForm : public TForm
{
__published:	// IDE-managed Components
   TListBox *ListBox1;
   void __fastcall ListBox1DblClick(TObject *Sender);
   void __fastcall FormShow(TObject *Sender);
private:	// User declarations

public:		// User declarations
   __fastcall TCmdWinForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TCmdWinForm *CmdWinForm;
//---------------------------------------------------------------------------
#endif
