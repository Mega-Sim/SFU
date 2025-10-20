//---------------------------------------------------------------------------

#ifndef E23PIOUnitH
#define E23PIOUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TE23PIOForm : public TForm
{
__published:	// IDE-managed Components
   TPopupMenu *PopupMenu1;
   TMenuItem *OutReset1;
   TPanel *Panel2;
   TShape *shapGo;
   TLabel *Label3;
   TPanel *Panel1;
   TPanel *Panel3;
   TPanel *Panel10;
   TPanel *Panel11;
   TPanel *Panel12;
   TPanel *Panel13;
   TPanel *Panel14;
   TMemo *Memo2;
   TCheckBox *CheckBox1;
   TPopupMenu *PopupMenu2;
   TMenuItem *InputClear1;
   TShape *Shape1;
   TLabel *Label5;
   TPanel *Panel4;
   TPanel *Panel5;
   TPanel *Panel6;
   TShape *shapLREQ;
   TLabel *Label1;
   TShape *shapUREQ;
   TShape *shapREADY;
   TShape *shapABORT;
   TLabel *Label2;
   TLabel *Label4;
   TLabel *Label7;
   TPanel *Panel7;
   TPanel *Panel8;
   TPanel *Panel9;
   TShape *shapVALID;
   TLabel *Label11;
   TShape *shapCS0;
   TShape *shapTRREQ;
   TShape *shapBUSY;
   TShape *shapCOMPT;
   TLabel *Label12;
   TLabel *Label15;
   TLabel *Label16;
   TLabel *Label17;
   TPanel *Panel15;
   TButton *btnPIOEnable;
   TButton *Button2;
   TMemo *Memo1;
        TLabel *Label6;
        TShape *shapCS1;
   void __fastcall PIOOutput(TObject *Sender);
   void __fastcall OutReset1Click(TObject *Sender);
   void __fastcall InputClear1Click(TObject *Sender);
   void __fastcall btnPIOEnableClick(TObject *Sender);
   void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TE23PIOForm(TComponent* Owner);

   void PIODisplay(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TE23PIOForm *E23PIOForm;
//---------------------------------------------------------------------------
#endif
