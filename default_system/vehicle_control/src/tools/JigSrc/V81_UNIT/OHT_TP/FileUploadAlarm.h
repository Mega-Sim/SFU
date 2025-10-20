//---------------------------------------------------------------------------

#ifndef FileUploadAlarmH
#define FileUploadAlarmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFileUploadAlarmForm : public TForm
{
__published:	// IDE-managed Components
    TEdit *editName;
    TLabel *lbAlarm;
    TLabel *Label2;
    TButton *btnForce;
    TLabel *Label1;
    TLabel *Label3;
    TButton *btnCancel;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btnForceClick(TObject *Sender);
    void __fastcall btnCancelClick(TObject *Sender);

private:	// User declarations
    int flag;   // 강제진행 여부

public:		// User declarations
    __fastcall TFileUploadAlarmForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFileUploadAlarmForm *FileUploadAlarmForm;
//---------------------------------------------------------------------------
#endif
