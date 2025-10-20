//---------------------------------------------------------------------------

#ifndef OcsStatusReportH
#define OcsStatusReportH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmOcsStatusReport : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TGroupBox *grpStatus;
	TLabel *Label1;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label2;
	TButton *btnClose;
	TButton *btnSend;
	TComboBox *cboStopReason;
	TComboBox *cboMaintananceOrder;
	TComboBox *cboEmOder;
	TComboBox *cboDataTransOrder;
	TGroupBox *GroupBox1;
	TCheckBox *chkStatus_0;
	TCheckBox *chkStatus_1;
	TCheckBox *chkStatus_2;
	TCheckBox *chkStatus_3;
	TCheckBox *chkStatus_4;
	TCheckBox *chkStatus_5;
	TCheckBox *chkStatus_6;
	TCheckBox *chkStatus_7;
	TGroupBox *GroupBox2;
	TCheckBox *chkStopOrder_0;
	TCheckBox *chkStopOrder_1;
	TCheckBox *chkStopOrder_2;
	TCheckBox *chkStopOrder_3;
	TCheckBox *chkStopOrder_4;
	TCheckBox *chkStopOrder_5;
	TCheckBox *chkStopOrder_6;
	TCheckBox *chkStopOrder_7;
	TCheckBox *chkStatusAuto;
	TTimer *tmrStatusAuto;
	TProgressBar *prgAutoSend;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnSendClick(TObject *Sender);
	void __fastcall chkStatusAutoClick(TObject *Sender);
	void __fastcall tmrStatusAutoTimer(TObject *Sender);

private:	// User declarations
	AnsiString strCurrentFileName;

public:		// User declarations
	__fastcall TfrmOcsStatusReport(TComponent* Owner);
	void SaveFile(AnsiString strFileName);
	void LoadFile(AnsiString strFileName);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmOcsStatusReport *frmOcsStatusReport;
//---------------------------------------------------------------------------
#endif
