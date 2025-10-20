//---------------------------------------------------------------------------

#ifndef ErrorUnitH
#define ErrorUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TErrorForm : public TForm
{
__published:	// IDE-managed Components
   TMemo *memErrHelp;
   void __fastcall FormShow(TObject *Sender);
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
   __fastcall TErrorForm(TComponent* Owner);
   int g_nIsShow;
   int Get_ErrorHelp(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TErrorForm *ErrorForm;
//---------------------------------------------------------------------------
#endif
