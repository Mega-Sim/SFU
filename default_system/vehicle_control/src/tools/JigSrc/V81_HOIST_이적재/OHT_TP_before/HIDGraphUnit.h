//---------------------------------------------------------------------------

#ifndef HIDGraphUnitH
#define HIDGraphUnitH
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
#include <ExtDlgs.hpp>
//---------------------------------------------------------------------------
class THIDGraphForm : public TForm
{
__published:	// IDE-managed Components
   TStatusBar *StatusBar1;
   TTimer *Timer1;
   TMainMenu *MainMenu1;
   TMenuItem *Online1;
   TMenuItem *Offline1;
   TMenuItem *popStart;
   TMenuItem *popEnd;
   TMenuItem *FileOpen1;
   TOpenDialog *OpenDialog1;
   TMenuItem *SaveImage1;
   TSavePictureDialog *SPicture;
   TSplitter *Splitter1;
   void __fastcall Timer1Timer(TObject *Sender);
   void __fastcall popEndClick(TObject *Sender);
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
   void __fastcall FileOpen1Click(TObject *Sender);
   void __fastcall SaveImage1Click(TObject *Sender);
   void __fastcall Chart1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
   void __fastcall popStartClick(TObject *Sender);
   void __fastcall Chart1MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
   void __fastcall Chart1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
   void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall THIDGraphForm(TComponent* Owner);
   int nTmpCount;
   float fTmpCount;
   int nEdc_MaxValue;
   int nEdc_MinValue;
   int nErec_MaxValue;
   int nErec_MinValue;
   int LogParse(String);
   void Graph_Start(int nTime_min);
   int nxTime;
};
//---------------------------------------------------------------------------
extern PACKAGE THIDGraphForm *HIDGraphForm;
//---------------------------------------------------------------------------
#endif
