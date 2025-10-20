//---------------------------------------------------------------------------

#ifndef AssistantUnitH
#define AssistantUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include "CGAUGES.h"
#include <ExtCtrls.hpp>
#include "../Common/DEF_DataType.h"

#define	FBWF_ENABLE				0x01
#define	FBWF_DISABLE			0x02
#define	FBWF_WILL_BE_ENABLED	0x03
#define	FBWF_WILL_BE_DISABLED	0x04
#define	FBWF_ERROR				0x07
//---------------------------------------------------------------------------
class TAssistantForm : public TForm
{
__published:	// IDE-managed Components
    TOpenDialog *dlgOpen;
    TGroupBox *grpProgramControl;
    TButton *btnOHTMainClose;
    TButton *btnOHTMainExec;
    TButton *btnOHTMainUpload;
   TPanel *panVer;
	TButton *btReqFBWFStat;
	TButton *btReqFBWFEnable;
	TButton *btReqFBWFDisable;
	TGroupBox *grpFBWFControl;
	TGroupBox *grpOHTInfo;
	TLabel *lbOHTVersion;
	TLabel *lbVersion;
	TLabel *Label1;
	TLabel *lblLog;
	TLabel *lbDriveConnection;
	TLabel *lbSpace;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *lbLogSpace;
	TLabel *lbl1;
    TStatusBar *stsbarAssistant;
    TPanel *panBottom;
    TPanel *panMiddle;
    TPanel *panTop;
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall btnOHTMainCloseClick(TObject *Sender);
   void __fastcall btnOHTMainExecClick(TObject *Sender);
//   void __fastcall btnScreenCaptureClick(TObject *Sender);
//   void __fastcall btnDownImgAndMainExecClick(TObject *Sender);
   void __fastcall btnOHTMainUploadClick(TObject *Sender);
	void __fastcall btReqFBWFStatClick(TObject *Sender);
	void __fastcall btReqFBWFEnableClick(TObject *Sender);
	void __fastcall btReqFBWFDisableClick(TObject *Sender);
private:	// User declarations
	int fbwfStatus;
	bool CheckFileCopyCompleted(String strFileName);
   	OHT_ADDITIONAL_INFO AdditionalInfo;
public:		// User declarations
	__fastcall TAssistantForm(TComponent* Owner);
	void SetFBWFStatus(int status);
	int GetFBWFStatus();

	void SetOHTAddidtionalInfo(OHT_ADDITIONAL_INFO* info);

};
//---------------------------------------------------------------------------
extern PACKAGE TAssistantForm *AssistantForm;
//---------------------------------------------------------------------------
#endif

