//---------------------------------------------------------------------------

#ifndef MultiJogDistUnitH
#define MultiJogDistUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TMultiJogDistForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
   TPanel *Panel2;
   TPanel *Panel3;
   TPanel *Panel4;
   TPanel *Panel5;
   TPanel *Panel6;
   TPanel *Panel7;
   TPanel *Panel8;
   TPanel *Panel9;
   TPanel *Panel10;
   TPanel *Panel11;
   TPanel *Panel12;
   TPanel *Panel13;
   TEdit *edtmpersec8_ACC;
   TEdit *edtmpersec8_DEC;
   TEdit *edtVel8;
   TEdit *edtDist8;
   TButton *Button1;
   TPanel *Panel14;
   TPanel *Panel15;
   TButton *Button2;
   TPanel *Panel16;
   TEdit *edtDist7;
   TPanel *Panel17;
   TEdit *edtVel7;
   TPanel *Panel18;
   TEdit *edtmpersec7_DEC;
   TPanel *Panel19;
   TEdit *edtmpersec7_ACC;
   TPanel *Panel20;
   TPanel *Panel21;
   TButton *Button3;
   TPanel *Panel22;
   TEdit *edtDist6;
   TPanel *Panel23;
   TEdit *edtVel6;
   TPanel *Panel24;
   TEdit *edtmpersec6_DEC;
   TPanel *Panel25;
   TEdit *edtmpersec6_ACC;
   TPanel *Panel26;
   TPanel *Panel27;
   TButton *Button4;
   TPanel *Panel28;
   TEdit *edtDist5;
   TPanel *Panel29;
   TEdit *edtVel5;
   TPanel *Panel30;
   TEdit *edtmpersec5_DEC;
   TPanel *Panel31;
   TEdit *edtmpersec5_ACC;
   TPanel *Panel32;
   TPanel *Panel33;
   TButton *Button5;
   TPanel *Panel34;
   TEdit *edtDist4;
   TPanel *Panel35;
   TEdit *edtVel4;
   TPanel *Panel36;
   TEdit *edtmpersec4_DEC;
   TPanel *Panel37;
   TEdit *edtmpersec4_ACC;
   TPanel *Panel38;
   TPanel *Panel39;
   TButton *Button6;
   TPanel *Panel40;
   TEdit *edtDist3;
   TPanel *Panel41;
   TEdit *edtVel3;
   TPanel *Panel42;
   TEdit *edtmpersec3_DEC;
   TPanel *Panel43;
   TEdit *edtmpersec3_ACC;
   TPanel *Panel44;
   TPanel *Panel45;
   TButton *Button7;
   TPanel *Panel46;
   TEdit *edtDist2;
   TPanel *Panel47;
   TEdit *edtVel2;
   TPanel *Panel48;
   TEdit *edtmpersec2_DEC;
   TPanel *Panel49;
   TEdit *edtmpersec2_ACC;
   TPanel *Panel50;
   TPanel *Panel51;
   TButton *Button8;
   TPanel *Panel52;
   TEdit *edtDist1;
   TPanel *Panel53;
   TEdit *edtVel1;
   TPanel *Panel54;
   TEdit *edtmpersec1_DEC;
   TPanel *Panel55;
   TEdit *edtmpersec1_ACC;
   TLabel *Label1;
   TLabel *Label2;
   TButton *Button9;
   TButton *Button10;
   TTimer *Timer1;
   void __fastcall Timer1Timer(TObject *Sender);
   void __fastcall Button9Click(TObject *Sender);
   void __fastcall Button10Click(TObject *Sender);
   void __fastcall Button8Click(TObject *Sender);
   void __fastcall Button7Click(TObject *Sender);
   void __fastcall Button6Click(TObject *Sender);
   void __fastcall Button5Click(TObject *Sender);
   void __fastcall Button4Click(TObject *Sender);
   void __fastcall Button3Click(TObject *Sender);
   void __fastcall Button2Click(TObject *Sender);
   void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TMultiJogDistForm(TComponent* Owner);
   int gm_nStep;
   int MoveDist(int);
};
//---------------------------------------------------------------------------
extern PACKAGE TMultiJogDistForm *MultiJogDistForm;
//---------------------------------------------------------------------------
#endif
