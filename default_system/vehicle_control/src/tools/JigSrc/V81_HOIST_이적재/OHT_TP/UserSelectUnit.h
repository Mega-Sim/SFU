//---------------------------------------------------------------------------

#ifndef UserSelectUnitH
#define UserSelectUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TUserSelectForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
    TBitBtn *btnCancel;
   TEdit *edtPwd;
    TBitBtn *btnSelect;
    TRadioGroup *rgUserSelect;
    TLabel *lbPw;
   void __fastcall btnCancelClick(TObject *Sender);
   void __fastcall FormActivate(TObject *Sender);
   void __fastcall btnSelectClick(TObject *Sender);
   void __fastcall edtPwdKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
   void __fastcall rgUserSelectClick(TObject *Sender);
   void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TUserSelectForm(TComponent* Owner);
   int Get_PwKey(void);
   int Is_PwOK(int, String);   
   int g_nPWKey;
};
//---------------------------------------------------------------------------
extern PACKAGE TUserSelectForm *UserSelectForm;
//---------------------------------------------------------------------------
#endif
