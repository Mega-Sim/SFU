//---------------------------------------------------------------------------

#ifndef ClwDiagAnswerH
#define ClwDiagAnswerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmDiagAnswer : public TForm
{
__published:	// IDE-managed Components
	TButton *btnClose;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TGroupBox *GroupBox1;
	TButton *btnMaintInfoRequest;
	TButton *btnMaintInfoReset;
	TButton *btnMaintLevelRequest;
	TComboBox *cbMaint;
	TTabSheet *TabSheet2;
	TEdit *edtSummaryInfo;
	TEdit *edtResponse;
	TLabel *Label1;
	TButton *btnAbnormalReport;
	TEdit *edtList;
	TEdit *edtErrorNode;
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnMaintInfoRequestClick(TObject *Sender);
	void __fastcall btnMaintInfoResetClick(TObject *Sender);
	void __fastcall btnMaintLevelRequestClick(TObject *Sender);
	void __fastcall btnAbnormalReportClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmDiagAnswer(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmDiagAnswer *frmDiagAnswer;
//---------------------------------------------------------------------------
#endif
