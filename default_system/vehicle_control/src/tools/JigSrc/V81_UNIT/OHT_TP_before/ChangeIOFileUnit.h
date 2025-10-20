//---------------------------------------------------------------------------

#ifndef ChangeIOFileUnitH
#define ChangeIOFileUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TChIOFileForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel2;
   TButton *btnOpen;
   TLabel *Label1;
   TOpenDialog *OpenDialog1;
   void __fastcall btnOpenClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TChIOFileForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TChIOFileForm *ChIOFileForm;
//---------------------------------------------------------------------------
#endif
