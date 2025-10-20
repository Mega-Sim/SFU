//---------------------------------------------------------------------------

#ifndef AMCParaUnitH
#define AMCParaUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TAMCPARA
{
   public :

   String VPGAIN;
   String VIGAIN;
   String VDGAIN;
   String VFGAIN;
   String VILIMIT;
   String PGAIN;
   String IGAIN;
   String DGAIN;
   String FGAIN;
   String ILIMIT;
   String VEL_LIMIT;
   String ACC_LIMIT;
   String SWLOWER_LIMIT;
   String SWUPPER_LIMIT;
   String IN_POSITION;
   String ERROR_LIMIT;
   String SWUPPER_LIMIT_ST;
   String SWLOWER_LIMIT_ST;
   String POS_LEVEL;
   String NEG_LEVEL;
   String HOME_LEVEL;
   String AMP_LEVEL;
   String AMP_RESET_LEVEL;
   String ERROR_LIMIT_ST;
   String STOP_RATE;
   String ESTOP_RATE;
   String ENCODER_DIR;
   String MOTOR_PAUSE;
   String AMP_ON_LEVEL;
   String MOTOR_PAUSE_LEVEL;
   String MOTOR_PAUSE_CHECKBIT;
   String CONTROL_CFG;
   String LOOP_CFG;
   String VOLTAGE_CFG;
   String POS_IMODE;
   String VEL_IMODE;
   String PULSE_MODE;
   String VTRACKING_FACTOR;
   String ENCODER_OFFSET;
   String POS_NOTCH_FREQ;
   String POS_LPF_FREQ;
   String VEL_NOTCH_FREQ;
   String VEL_LPF_FREQ;
   String GEAR_RATIO;
   String HWLOWER_LIMIT;
   String HWUPPER_LIMIT;
   String MOTORLYPE;
   String ENCODER_CFG;
   String HOME_INDEX;
   String IO_INT_ENABLE;
   String INT_EVENT_ST;
   String AMP_FAULT_ST;
   String INPOS_LEVEL;
   String USERIO_BOOTMODE;
   String USERIO_BOOTVALUE;
   String POS_LEVEL_ST;
   String NEG_LEVEL_ST;
   String HOME_LEVEL_ST;

   public :
      TAMCPARA();
      ~TAMCPARA();
      int Init();
};
//----------------------------------------------------------------------------
class TAMCParaForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
   TPanel *Panel2;
   TGroupBox *GroupBox2;
   TComboBox *ComboBox2;
   TPanel *Panel3;
   TGroupBox *GroupBox1;
   TLabel *Label1;
   TLabel *Label2;
   TLabel *Label3;
   TPanel *Panel4;
   TEdit *edtVPGAIN;
   TPanel *Panel5;
   TEdit *edtVIGAIN;
   TPanel *Panel6;
   TEdit *edtVDGAIN;
   TPanel *Panel7;
   TEdit *edtVFGAIN;
   TPanel *Panel13;
   TEdit *edtVILIMIT;
   TGroupBox *GroupBox4;
   TLabel *Label4;
   TLabel *Label5;
   TLabel *Label11;
   TPanel *Panel14;
   TEdit *edtPGAIN;
   TPanel *Panel15;
   TEdit *edtIGAIN;
   TPanel *Panel16;
   TEdit *edtDGAIN;
   TPanel *Panel17;
   TEdit *edtFGAIN;
   TPanel *Panel18;
   TEdit *edtILIMIT;
   TGroupBox *GroupBox5;
   TPanel *Panel20;
   TLabel *Label13;
   TEdit *edtVEL_LIMIT;
   TPanel *Panel21;
   TLabel *Label14;
   TEdit *edtACC_LIMIT;
   TPanel *Panel22;
   TLabel *Label15;
   TEdit *edtSWLOWER_LIMIT;
   TPanel *Panel23;
   TLabel *Label16;
   TEdit *edtSWUPPER_LIMIT;
   TPanel *Panel24;
   TLabel *Label17;
   TEdit *edtIN_POSITION;
   TPanel *Panel25;
   TLabel *Label18;
   TEdit *edtERROR_LIMIT;
   TGroupBox *GroupBox3;
   TPanel *Panel8;
   TEdit *edtSWUPPER_LIMIT_ST;
   TPanel *Panel9;
   TEdit *edtSWLOWER_LIMIT_ST;
   TLabel *Label6;
   TLabel *Label7;
   TPanel *Panel10;
   TEdit *edtPOS_LEVEL;
   TPanel *Panel11;
   TEdit *edtNEG_LEVEL;
   TPanel *Panel12;
   TEdit *edtHOME_LEVEL;
   TLabel *Label8;
   TLabel *Label9;
   TPanel *Panel19;
   TEdit *edtAMP_LEVEL;
   TPanel *Panel26;
   TEdit *edtAMP_RESET_LEVEL;
   TPanel *Panel30;
   TEdit *edtERROR_LIMIT_ST;
   TPanel *Panel31;
   TEdit *edtSTOP_RATE;
   TPanel *Panel32;
   TEdit *edtESTOP_RATE;
   TGroupBox *GroupBox6;
   TPanel *Panel33;
   TEdit *edtENCODER_DIR;
   TPanel *Panel34;
   TEdit *edtMOTOR_PAUSE;
   TPanel *Panel35;
   TEdit *edtAMP_ON_LEVEL;
   TGroupBox *GroupBox7;
   TPanel *Panel50;
   TEdit *edtMOTOR_PAUSE_LEVEL;
   TPanel *Panel51;
   TEdit *edtMOTOR_PAUSE_CHECKBIT;
   TGroupBox *GroupBox8;
   TPanel *Panel36;
   TEdit *edtCONTROL_CFG;
   TPanel *Panel37;
   TEdit *edtLOOP_CFG;
   TPanel *Panel38;
   TEdit *edtVOLTAGE_CFG;
   TPanel *Panel39;
   TEdit *edtPOS_IMODE;
   TPanel *Panel40;
   TEdit *edtVEL_IMODE;
   TPanel *Panel46;
   TEdit *edtPULSE_MODE;
   TPanel *Panel52;
   TEdit *edtVTRACKING_FACTOR;
   TPanel *Panel53;
   TEdit *edtENCODER_OFFSET;
   TGroupBox *GroupBox9;
   TPanel *Panel45;
   TEdit *edtPOS_NOTCH_FREQ;
   TPanel *Panel47;
   TEdit *edtPOS_LPF_FREQ;
   TPanel *Panel48;
   TEdit *edtVEL_NOTCH_FREQ;
   TPanel *Panel49;
   TEdit *edtVEL_LPF_FREQ;
   TPanel *Panel41;
   TEdit *edtGEAR_RATIO;
   TPanel *Panel42;
   TEdit *edtHWLOWER_LIMIT;
   TPanel *Panel43;
   TEdit *edtHWUPPER_LIMIT;
   TPanel *Panel44;
   TEdit *edtMOTORLYPE;
   TPanel *Panel54;
   TEdit *edtENCODER_CFG;
   TPanel *Panel55;
   TEdit *edtHOME_INDEX;
   TPanel *Panel56;
   TEdit *edtIO_INT_ENABLE;
   TPanel *Panel57;
   TEdit *edtINT_EVENT_ST;
   TPanel *Panel58;
   TEdit *edtAMP_FAULT_ST;
   TPanel *Panel59;
   TEdit *edtINPOS_LEVEL;
   TPanel *Panel60;
   TEdit *edtUSERIO_BOOTMODE;
   TPanel *Panel61;
   TEdit *edtUSERIO_BOOTVALUE;
   TLabel *Label10;
   TLabel *Label12;
   TLabel *Label19;
   TLabel *Label20;
   TPanel *Panel27;
   TEdit *edtPOS_LEVEL_ST;
   TPanel *Panel28;
   TEdit *edtNEG_LEVEL_ST;
   TPanel *Panel29;
   TEdit *edtHOME_LEVEL_ST;
   TLabel *Label21;
   TLabel *Label22;
   TLabel *Label23;
   TLabel *Label24;
   TLabel *Label25;
   TLabel *Label26;
   TLabel *Label27;
   TLabel *Label28;
   TLabel *Label29;
   TLabel *Label30;
   TLabel *Label31;
   TLabel *Label32;
   TLabel *Label33;
   TLabel *Label34;
   TLabel *Label35;
   TLabel *Label36;
   TLabel *Label37;
   TLabel *Label38;
   TLabel *Label39;
   TLabel *Label40;
   TLabel *Label41;
   TLabel *Label42;
   TLabel *Label43;
   TLabel *Label44;
   TLabel *Label45;
   TLabel *Label46;
   TLabel *Label47;
   TSaveDialog *SaveDialog1;
   TButton *btnOpen;
   TButton *btnSave;
   TButton *btnSaveAs;
   TOpenDialog *OpenDialog2;
   void __fastcall FormCreate(TObject *Sender);
   void __fastcall FormDestroy(TObject *Sender);
   void __fastcall edtVEL_LIMITChange(TObject *Sender);
   void __fastcall ComboBox2Change(TObject *Sender);
   void __fastcall btnOpenClick(TObject *Sender);
   void __fastcall btnSaveClick(TObject *Sender);
   void __fastcall btnSaveAsClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TAMCParaForm(TComponent* Owner);
   String g_strIniFileName;
   TAMCPARA *t_AMCParameter[4];
   int Int_edtColor(void);
   int Load_IniData(String);
   int Save_IniData(String);
   bool IsNumber(String);
   int Display_IniData(int);
   int Set_Display_IniData(int);

};
//---------------------------------------------------------------------------
extern PACKAGE TAMCParaForm *AMCParaForm;
//---------------------------------------------------------------------------
#endif
