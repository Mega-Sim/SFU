//---------------------------------------------------------------------------

#ifndef JobOrderOCSH
#define JobOrderOCSH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TfrmJobOrderOCS : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TButton *btnClose;
	TButton *btnSend;
	TGroupBox *grpStatus;
	TLabel *Label1;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label2;
	TComboBox *cboStopReason;
	TComboBox *cboMaintananceOrder;
	TComboBox *cboEmOder;
	TComboBox *cboDataTransOrder;
	TGroupBox *GroupBox1;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TComboBox *cboCmdDiv;
	TComboBox *cboCmdCycle;
	TComboBox *cboWorkCarryType;
	TComboBox *cboRiv;
	TLabel *Label12;
	TEdit *edtCurAddr_H;
	TLabel *Label16;
	TEdit *edtStopAddr_H;
	TLabel *Label17;
	TEdit *edtDestAddr_H;
	TEdit *edtDestAddr_M;
	TEdit *edtDestAddr_L;
	TCheckBox *chkHotLot;
	TLabel *Label18;
	TComboBox *cboChangeNode;
	TLabel *Label19;
	TComboBox *cboFinalPortType;
	TLabel *Label21;
	TLabel *Label22;
	TLabel *Label23;
	TLabel *Label24;
	TEdit *edtBufferType;
	TLabel *Label25;
	TLabel *Label26;
	TLabel *Label27;
	TLabel *Label28;
	TLabel *Label29;
	TLabel *Label30;
	TLabel *Label31;
	TLabel *Label32;
	TEdit *edtTeachingData_Hoist;
	TEdit *edtTeachingData_Shift;
	TEdit *edtTeachingData_Rotate;
	TTabSheet *TabSheet3;
	TGroupBox *GroupBox2;
	TLabel *Label33;
	TComboBox *cboTeachingData_LookDownLevel;
	TLabel *Label34;
	TComboBox *cboTeachingData_PIOInfo;
	TLabel *Label35;
	TComboBox *cboTeachingData_PIOTimeLevel;
	TLabel *Label36;
	TComboBox *cboTeachingData_Option;
	TLabel *Label37;
	TLabel *Label38;
	TGroupBox *GroupBox3;
	TRadioButton *rdoPathData_Def_Direction_Left;
	TRadioButton *rdoPathData_Def_Direction_Right;
	TLabel *Label39;
	TLabel *Label40;
	TLabel *Label41;
	TEdit *edtPathData_StartNodeID;
	TEdit *edtPathData_EndNodeID;
	TEdit *edtPathData_ExtraNum;
	TLabel *Label42;
	TStringGrid *FeederGrid;
	TEdit *edtOrderID;
	TButton *btnGetOrderID;
	TGroupBox *GroupBox4;
	TCheckBox *chkStatus_0;
	TCheckBox *chkStatus_1;
	TCheckBox *chkStatus_2;
	TCheckBox *chkStatus_3;
	TCheckBox *chkStatus_4;
	TCheckBox *chkStatus_5;
	TCheckBox *chkStatus_6;
	TCheckBox *chkStatus_7;
	TGroupBox *GroupBox5;
	TCheckBox *chkStopOrder_0;
	TCheckBox *chkStopOrder_1;
	TCheckBox *chkStopOrder_2;
	TCheckBox *chkStopOrder_3;
	TCheckBox *chkStopOrder_4;
	TCheckBox *chkStopOrder_5;
	TCheckBox *chkStopOrder_6;
	TCheckBox *chkStopOrder_7;
	TCheckBox *chkDriving;
	void __fastcall btnSendClick(TObject *Sender);
	void __fastcall btnGetOrderIDClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnCloseClick(TObject *Sender);
private:	// User declarations
	AnsiString strCurrentFileName;
public:		// User declarations
	__fastcall TfrmJobOrderOCS(TComponent* Owner);
	void SaveFile(AnsiString strFileName);
	void LoadFile(AnsiString strFileName);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmJobOrderOCS *frmJobOrderOCS;
//---------------------------------------------------------------------------
#endif
