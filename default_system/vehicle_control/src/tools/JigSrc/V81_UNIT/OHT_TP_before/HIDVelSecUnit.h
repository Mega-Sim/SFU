//---------------------------------------------------------------------------

#ifndef HIDVelSecUnitH
#define HIDVelSecUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class THIDVelSecForm : public TForm
{
__published:	// IDE-managed Components
   TChart *Chart1;
   TFastLineSeries *Series1;
   TPageControl *PageControl1;
   TTabSheet *TabSheet1;
   TTabSheet *TabSheet2;
   TTabSheet *TabSheet3;
   TMemo *memHIDLog1;
   TMemo *memHIDLog2;
   TMemo *memHIDLog3;
   TMainMenu *MainMenu1;
   TMenuItem *Draw1;
   TMenuItem *Clear1;
   TMenuItem *Capture1;
   TFastLineSeries *Series2;
   TFastLineSeries *Series3;
   TOpenDialog *OpenDialog1;
   TMenuItem *Drawsec1;
   TMenuItem *Drawmm1;
   void __fastcall Clear1Click(TObject *Sender);
   void __fastcall Capture1Click(TObject *Sender);
   void __fastcall Drawsec1Click(TObject *Sender);
   void __fastcall Drawmm1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall THIDVelSecForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE THIDVelSecForm *HIDVelSecForm;
//---------------------------------------------------------------------------
#endif
