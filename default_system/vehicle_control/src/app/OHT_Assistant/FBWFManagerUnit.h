//---------------------------------------------------------------------------

#ifndef FBWFManagerUnitH
#define FBWFManagerUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

#include "MonitoringUtil.h"
//---------------------------------------------------------------------------
class TFBWFManagerForm : public TForm
{
__published:	// IDE-managed Components
	TMemo *mmoLog;
	TButton *btGetStatus;
	TRadioGroup *rgFBWF;
	TButton *btSetStatus;
	TStatusBar *statFBWF;
	void __fastcall btSetStatusClick(TObject *Sender);
	void __fastcall btGetStatusClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFBWFManagerForm(TComponent* Owner);

private:

public:
	void Init();
	void DisplayStatus(int status);
};
//---------------------------------------------------------------------------
extern PACKAGE TFBWFManagerForm *FBWFManagerForm;
//---------------------------------------------------------------------------
#endif
