//---------------------------------------------------------------------------

#ifndef WarnningUnitH
#define WarnningUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TWarnningForm : public TForm
{
__published:	// IDE-managed Components
   TTimer *ControlTimer;
   TMemo *DisplayMemo;
   void __fastcall FormShow(TObject *Sender);
   void __fastcall ControlTimerTimer(TObject *Sender);
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
   __fastcall TWarnningForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TWarnningForm *WarnningForm;
//---------------------------------------------------------------------------
#endif
