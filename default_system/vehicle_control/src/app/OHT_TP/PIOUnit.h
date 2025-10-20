//---------------------------------------------------------------------------

#ifndef PIOUnitH
#define PIOUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TPIOForm : public TForm
{
__published:	// IDE-managed Components
   TPopupMenu *PopupMenu1;
   TMenuItem *OutReset1;
   TPanel *Panel1;
   TPanel *Panel4;
   TPopupMenu *PopupMenu2;
   TMenuItem *InputClear1;
   TButton *btnPIOEnable;
   TButton *Button2;
   TPanel *Panel3;
   TPanel *Panel10;
   TPanel *Panel11;
   TPanel *Panel12;
   TPanel *Panel13;
   TPanel *Panel14;
   TMemo *Memo1;
   TPanel *Panel16;
   TMemo *Memo2;
   TGroupBox *EQPIOGB;
   TPanel *EqPIOInPanel;
   TPanel *EqPioInCaption;
   TPanel *EqPIOOutCaption;
   TPanel *EqPIOOutPanel;
   TGroupBox *HandPIOGB;
   TPanel *HandPIOInPanel;
   TPanel *HandPIOOutPanel;
   TPanel *HandPIOOutCaption;
   TPanel *HandPIOInCaption;
        TShape *shpGoLeft;
   TLabel *Label3;
   TPanel *Panel5;
   TPanel *Panel6;
    TShape *shapLREQ;
   TLabel *Label1;
   TShape *shapUREQ;
   TShape *shapREADY;
   TShape *shapHOAVBL;
   TShape *shapES;
   TLabel *Label2;
   TLabel *Label4;
   TLabel *Label7;
   TLabel *Label8;
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
   TCheckBox *CheckBox1;
        TLabel *Label5;
        TShape *shpGoRight;
        TShape *shpEQPIO_Left_Sel;
        TLabel *Label6;
        TShape *shpEQPIO_Right_Sel;
        TLabel *Label9;
    TTimer *timerUpdateDisplay;
	TEdit *edtID;
	TEdit *edtCH;
	TLabel *Label10;
	TLabel *lbl1;
	TComboBox *cbbPIOType;
	TLabel *Label13;
   void __fastcall PIOOutput(TObject *Sender);
   void __fastcall OutReset1Click(TObject *Sender);
   void __fastcall InputClear1Click(TObject *Sender);
   void __fastcall btnPIOEnableClick(TObject *Sender);
   void __fastcall Button2Click(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall timerUpdateDisplayTimer(TObject *Sender);

private:	// User declarations
public:		// User declarations
   __fastcall TPIOForm(TComponent* Owner);

   void PIODisplay(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TPIOForm *PIOForm;
//---------------------------------------------------------------------------
#endif
