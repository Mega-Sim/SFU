//---------------------------------------------------------------------------

#ifndef HIDVelUnitH
#define HIDVelUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Chart.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
//---------------------------------------------------------------------------
class THIDVelForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
   TPanel *Panel2;
   TMemo *memHIDLog;
   TMainMenu *MainMenu1;
   TMenuItem *Draw1;
   TChart *Chart1;
   TFastLineSeries *Series1;
   TMenuItem *Clear1;
   void __fastcall Draw1Click(TObject *Sender);
   void __fastcall Clear1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall THIDVelForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE THIDVelForm *HIDVelForm;
//---------------------------------------------------------------------------
#endif
