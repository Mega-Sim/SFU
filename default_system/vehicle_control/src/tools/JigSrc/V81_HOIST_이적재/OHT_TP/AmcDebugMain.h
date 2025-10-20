//---------------------------------------------------------------------------

#ifndef AmcDebugMainH
#define AmcDebugMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TAmcDebugForm : public TForm
{
__published:	// IDE-managed Components
   TPageControl *PageControl1;
   TMainMenu *MainMenu1;
   TMenuItem *Analysis;
   TTabSheet *Log_RawData;
   TTabSheet *ResultData;
   TMemo *Memo1;
   TMemo *Memo2;
   TMenuItem *Clear1;
   TPopupMenu *LogClearPopup;
   TMenuItem *LogClear1;
   TPopupMenu *ResultClearPopup;
   TMenuItem *ResultClear1;
   TPanel *Panel1;
   TRadioGroup *LogTypeRadio;
   void __fastcall AnalysisClick(TObject *Sender);
   void __fastcall Clear1Click(TObject *Sender);
   void __fastcall LogClear1Click(TObject *Sender);
   void __fastcall ResultClear1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TAmcDebugForm(TComponent* Owner);
   void AnalysisData(void);
   void AnalysisData2(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TAmcDebugForm *AmcDebugForm;
//---------------------------------------------------------------------------
#endif
