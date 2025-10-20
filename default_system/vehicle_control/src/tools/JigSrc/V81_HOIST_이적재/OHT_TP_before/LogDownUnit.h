//---------------------------------------------------------------------------

#ifndef LogDownUnitH
#define LogDownUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TLogDownForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
    TButton *btnDownloadStart;
    TButton *btnDownloadCancel;
    TRadioGroup *rgLogSelect;
   TPanel *Panel2;
    TEdit *edtMonth;
   TLabel *Label1;
    TEdit *edtDay;
   TLabel *Label2;
    TEdit *edtHour;
   TLabel *Label3;
    TEdit *edtSpecifyMonth;
   TLabel *Label4;
   TLabel *Label5;
    TEdit *edtSpecifyDay;
    TCheckBox *chkHour0;
    TCheckBox *chkHour1;
    TCheckBox *chkHour2;
    TCheckBox *chkHour3;
    TCheckBox *chkHour4;
    TCheckBox *chkHour5;
    TCheckBox *chkHour6;
    TCheckBox *chkHour7;
    TCheckBox *chkHour8;
    TCheckBox *chkHour9;
    TCheckBox *chkHour10;
    TCheckBox *chkHour11;
    TCheckBox *chkHour12;
    TCheckBox *chkHour13;
    TCheckBox *chkHour14;
    TCheckBox *chkHour15;
    TCheckBox *chkHour16;
    TCheckBox *chkHour17;
    TCheckBox *chkHour18;
    TCheckBox *chkHour19;
    TCheckBox *chkHour20;
    TCheckBox *chkHour21;
    TCheckBox *chkHour22;
    TCheckBox *chkHour23;
   void __fastcall btnDownloadStartClick(TObject *Sender);
   void __fastcall btnDownloadCancelClick(TObject *Sender);
   void __fastcall edtMonthMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
   void __fastcall edtDayMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
   void __fastcall edtHourMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
public:		// User declarations
   __fastcall TLogDownForm(TComponent* Owner);
   bool bIsOHTExecLogSave;
   int LogDown_SelectTime(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TLogDownForm *LogDownForm;
//---------------------------------------------------------------------------
#endif
