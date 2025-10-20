//---------------------------------------------------------------------------

#ifndef ProcessUnitH
#define ProcessUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "cgauges.h"

//---------------------------------------------------------------------------
class TProcessForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
   TCGauge *CGauge1;
   TLabel *Label1;
   TLabel *Label2;
   TLabel *Label3;
   TLabel *lblFrom;
   TLabel *lblTo;
   void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TProcessForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TProcessForm *ProcessForm;
//---------------------------------------------------------------------------
#endif
