//---------------------------------------------------------------------------

#ifndef DiagSvrCmdH
#define DiagSvrCmdH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmDiagSvrCmd : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TButton *btnClose;
	TButton *btnConnectCheckRequest;
	TGroupBox *GroupBox1;
	TButton *btnMaintInfoRequest;
	TButton *btnMaintInfoReset;
	TButton *btnMaintInfoResponse;
	TComboBox *cbMaint;
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnConnectCheckRequestClick(TObject *Sender);
	void __fastcall btnMaintInfoRequestClick(TObject *Sender);
	void __fastcall btnMaintInfoResetClick(TObject *Sender);
	void __fastcall btnMaintInfoResponseClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmDiagSvrCmd(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmDiagSvrCmd *frmDiagSvrCmd;
//---------------------------------------------------------------------------
#endif
