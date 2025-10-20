//---------------------------------------------------------------------------

#ifndef SetCurnodeNUnitH
#define SetCurnodeNUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RebootUnit.h"
//---------------------------------------------------------------------------
class TSetCurForm : public TRebootForm
{
__published:	// IDE-managed Components
   void __fastcall btnOKClick(TObject *Sender);
   void __fastcall btnCancelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TSetCurForm(TComponent* Owner);
   int nCmdCurnode;
};
//---------------------------------------------------------------------------
extern PACKAGE TSetCurForm *SetCurForm;
//---------------------------------------------------------------------------
#endif
