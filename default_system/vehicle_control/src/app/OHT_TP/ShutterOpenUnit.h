//---------------------------------------------------------------------------

#ifndef ShutterOpenUnitH
#define ShutterOpenUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TShutterOpenForm : public TForm
{
__published:	// IDE-managed Components
   TGroupBox *GroupBox1;
   TCheckBox *CheckBox1;
   TCheckBox *CheckBox2;
   TCheckBox *CheckBox3;
   TCheckBox *CheckBox4;
   TBitBtn *btnOK;
   TBitBtn *btnCancel;
   TLabel *Label1;
   void __fastcall FormShow(TObject *Sender);
   void __fastcall btnOKClick(TObject *Sender);
   void __fastcall btnCancelClick(TObject *Sender);
   void __fastcall CheckBox1Click(TObject *Sender);
private:	// User declarations
   int Check_OKBtn(void);
public:		// User declarations
   __fastcall TShutterOpenForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TShutterOpenForm *ShutterOpenForm;
//---------------------------------------------------------------------------
#endif
