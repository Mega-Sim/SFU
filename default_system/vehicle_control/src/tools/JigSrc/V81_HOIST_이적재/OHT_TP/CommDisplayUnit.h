//---------------------------------------------------------------------------

#ifndef CommDisplayUnitH
#define CommDisplayUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TCommDisplayForm : public TForm
{
__published:	// IDE-managed Components
   TListBox *mmoDisplay;
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall mmoDisplayDblClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TCommDisplayForm(TComponent* Owner);
   bool bDisplayFlag;
   void Communicaion_Display(String strCommData);
   void DataAdd(String strTmpData);
   bool bTrace;
};
//---------------------------------------------------------------------------
extern PACKAGE TCommDisplayForm *CommDisplayForm;
//---------------------------------------------------------------------------
#endif
