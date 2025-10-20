//---------------------------------------------------------------------------

#ifndef FirmwareUpdateUnitH
#define FirmwareUpdateUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "Protocol_Define_Modify.h"
#include <ExtCtrls.hpp>

// ----- 체크박스 확인을 위한 정의 ----
#define CHK_PROTO		1
#define CHK_RUN			2
#define CHK_EXPORT		3
#define CHK_FIRMWARE	4
// ------------------------------------

//---------------------------------------------------------------------------
class TFirmwareUpdateForm : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TCheckBox *chkMapSet_Proto;
	TCheckBox *chkMapSet_Run;
	TCheckBox *chkMapSet_Export;
	TCheckBox *chkFirmwareUpdate;
	TGroupBox *GroupBox2;
	TLabel *Label23;
	TEdit *IpAddr_H;
	TEdit *IpAddr_M1;
	TEdit *IpAddr_M2;
	TEdit *IpAddr_L;
	TLabel *Label24;
	TEdit *edtUserName;
	TLabel *Label29;
	TEdit *edtPassword;
	TLabel *Label9;
	TEdit *edtDataSendTo;
	TGroupBox *GroupBox3;
	TMemo *mmLog;
	TButton *btnUpdate;
	TButton *btnVersionInfoGet;
	TButton *btnSettingSave;
	TButton *btnClear;
	TLabel *Label8;
	TEdit *edtVersion;
	TButton *btnSettingLoad;
	TGroupBox *GroupBox4;
	TPanel *panCmd_0;
	TPanel *panCmd_1;
	TPanel *panCmd_2;
	TPanel *panCmd_3;
	TPanel *panCmd_Start;
	void __fastcall btnClearClick(TObject *Sender);
	void __fastcall chkMapSet_ProtoClick(TObject *Sender);
	void __fastcall chkMapSet_RunClick(TObject *Sender);
	void __fastcall chkMapSet_ExportClick(TObject *Sender);
	void __fastcall btnUpdateClick(TObject *Sender);
	void __fastcall btnVersionInfoGetClick(TObject *Sender);
	void __fastcall chkFirmwareUpdateClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnSettingLoadClick(TObject *Sender);
	void __fastcall btnSettingSaveClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
	void CheckBoxVerify(int iCheck);
	void SaveSetting();
	void LoadSetting();

	void Reset_Display();
	void Parse_Result(AnsiString strMsg);

public:		// User declarations
	__fastcall TFirmwareUpdateForm(TComponent* Owner);
	void FWU_Display(AnsiString strMsg);
};
//---------------------------------------------------------------------------
extern PACKAGE TFirmwareUpdateForm *FirmwareUpdateForm;
//---------------------------------------------------------------------------
#endif
