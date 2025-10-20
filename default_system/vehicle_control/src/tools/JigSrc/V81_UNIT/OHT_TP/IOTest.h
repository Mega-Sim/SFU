//---------------------------------------------------------------------------

#ifndef IOTestH
#define IOTestH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>

//---------------------------------------------------------------------------
class TIOTestForm : public TForm
{
__published:	// IDE-managed Components
	TTimer *IOTestTimer;
   TGroupBox *GroupBox5;
   TRadioGroup *SoundRadioGroup;
   TButton *Button6;
   TButton *Button7;
   TGroupBox *GroupBox6;
   TPanel *Panel5;
   TShape *shpLkdnTrouble;
   TShape *shpLkdnDetect;
   TLabel *Label12;
   TLabel *Label13;
   TButton *Button1;
   TButton *Button2;
   TGroupBox *GroupBox7;
   TButton *ReadBCRBtn;
   TPanel *Panel9;
   TPanel *BCRPanel;
   TButton *Button8;
   TButton *Button9;
   TEdit *edtCurNode;
   TGroupBox *GroupBox2;
   TButton *btnDetectPwrOn;
   TButton *btnDetectPwrOff;
   TButton *btnDetectEnable;
   TPanel *Panel2;
   TShape *shpDetectShort;
   TShape *shpDetectMiddle;
   TShape *shpDetectTrouble;
   TShape *shpDetectLong;
   TLabel *Label4;
   TLabel *Label5;
   TLabel *Label6;
   TLabel *Label20;
   TPanel *panDetect;
   TLabel *lblDetect;
   TGroupBox *GroupBox1;
   TButton *btnObsPwrOn;
   TButton *btnObsPwrOff;
   TButton *btnObsEnable;
   TPanel *Panel1;
   TShape *shpObsShort;
   TShape *shpObsMiddle;
   TShape *shpObsTrouble;
   TShape *shpObsLong;
   TLabel *Label1;
   TLabel *Label2;
   TLabel *Label3;
   TLabel *Label21;
   TPanel *panObs;
   TLabel *lblObs;
   TGroupBox *GroupBox3;
   TButton *btnAlarmReset;
   TPanel *Panel3;
   TShape *shpAlarm2;
   TShape *shpAlarm1_R;
   TShape *shpAlarm4;
   TShape *shpAlarm1_F;
   TLabel *Label7;
   TLabel *Label8;
   TLabel *Label9;
   TLabel *Label10;
   TShape *shpAlarm3;
   TLabel *Label11;
   TRadioGroup *rgServoAlarm;
   TButton *btnServoON;
   TButton *btnServoOFF;
   TComboBox *comboObs;
   TComboBox *comboDetect;
   TGroupBox *GroupBox4;
   TPanel *Panel4;
   TShape *shpDetect;
   TLabel *Label14;
	void __fastcall SoundOnBtnClick(TObject *Sender);
	void __fastcall SoundOffBtnClick(TObject *Sender);
	void __fastcall IOTestTimerTimer(TObject *Sender);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
   void __fastcall ReadBCRBtnClick(TObject *Sender);
   void __fastcall Button8Click(TObject *Sender);
   void __fastcall Button9Click(TObject *Sender);
   void __fastcall btnAlarmResetClick(TObject *Sender);
   void __fastcall btnObsPwrOnClick(TObject *Sender);
   void __fastcall btnObsPwrOffClick(TObject *Sender);
   void __fastcall btnObsEnableClick(TObject *Sender);
   void __fastcall btnDetectPwrOnClick(TObject *Sender);
   void __fastcall btnDetectPwrOffClick(TObject *Sender);
   void __fastcall btnDetectEnableClick(TObject *Sender);
   void __fastcall Button1Click(TObject *Sender);
   void __fastcall Button2Click(TObject *Sender);
   void __fastcall btnEQPIOEnableClick(TObject *Sender);
   void __fastcall btnEQPIODisableClick(TObject *Sender);
   void __fastcall btnHPIOEnableClick(TObject *Sender);
   void __fastcall btnHPIODisableClick(TObject *Sender);
   void __fastcall btnServoONClick(TObject *Sender);
   void __fastcall btnServoOFFClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TIOTestForm(TComponent* Owner);
   int i;
   void Warnning_Display(TColor, String);
};
//---------------------------------------------------------------------------
extern PACKAGE TIOTestForm *IOTestForm;
//---------------------------------------------------------------------------
#endif
