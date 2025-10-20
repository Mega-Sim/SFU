// ---------------------------------------------------------------------------

#ifndef VelGraphUnitH
#define VelGraphUnitH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Dialogs.hpp>

// ---------------------------------------------------------------------------
class TVelGraphForm : public TForm
{
__published: // IDE-managed Components

    TPanel *Panel1;
    TPanel *panDrivingSpeed;
    TSplitter *Splitter1;
    TPanel *Panel3;
    TGroupBox *GroupBox1;
    TStatusBar *StatusBar1;
    TPanel *Panel29;
    TLabel *lblMotor4_1;
    TLabel *Label38;
    TPanel *Panel4;
    TLabel *lblMotor4_2;
    TLabel *Label2;
    TPanel *Panel5;
    TLabel *lblMotor4_3;
    TLabel *Label4;
    TGroupBox *GroupBox3;
    TLabel *Label15;
    TLabel *Label16;
    TLabel *Label17;
    TPanel *Panel11;
    TLabel *lblMotor2_1;
    TPanel *Panel12;
    TLabel *lblMotor2_2;
    TPanel *Panel13;
    TLabel *lblMotor2_3;
    TGroupBox *GroupBox4;
    TLabel *Label23;
    TLabel *Label24;
    TLabel *Label25;
    TPanel *Panel15;
    TLabel *lblMotor1_1;
    TPanel *Panel17;
    TLabel *lblMotor1_3;
    TMainMenu *MainMenu1;
    TMenuItem *Start1;
    TMenuItem *Stop1;
    TMenuItem *Save1;
    TMenuItem *N1Start1;
    TMenuItem *N24Start1;
    TMenuItem *N1Stop1;
    TMenuItem *N24Stop1;
    TTimer *VelTimer;
    TTimer *Motor1Timer;
    TTimer *Motor24Timer;
    TOpenDialog *OpenDialog1;
    TOpenDialog *OpenDialog2;
    TMenuItem *Motor1Graph1;
    TMenuItem *Motor24Graph1;
    TPanel *Panel6;
    TLabel *lblMotor1_1_msec;
    TLabel *Label3;
    TMenuItem *File1;
    TMenuItem *Open1;
    TOpenDialog *OpenDialog3;
    TPanel *Panel16;
    TLabel *lblMotor1_2;
	TChart *chartDrivingSpeed;
	TChart *chartHoistSpeed;
    TCheckBox *chkDisplayBothDrv;
    TCheckBox *CheckBox2;
    TCheckBox *chkLogSave1;
    TCheckBox *chkTime;
    TFastLineSeries *fastlineDriving_F;
    TFastLineSeries *fastlineDriving_R;
	TCheckBox *CheckBox1;
	TFastLineSeries *fastlineHoist;
	TGroupBox *GroupBox2;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TPanel *Panel7;
	TLabel *lblMotor3_1;
	TPanel *Panel8;
	TLabel *lblMotor3_2;
	TPanel *Panel9;
	TLabel *lblMotor3_3;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall VelTimerTimer(TObject *Sender);
    void __fastcall Motor1TimerTimer(TObject *Sender);
    void __fastcall N1Stop1Click(TObject *Sender);
    void __fastcall N1Start1Click(TObject *Sender);
    void __fastcall N24Start1Click(TObject *Sender);
    void __fastcall N24Stop1Click(TObject *Sender);
    void __fastcall Motor24TimerTimer(TObject *Sender);
    void __fastcall Motor1Graph1Click(TObject *Sender);
    void __fastcall Motor24Graph1Click(TObject *Sender);
    void __fastcall Open1Click(TObject *Sender);


private: // User declarations
        public : // User declarations
    __fastcall TVelGraphForm(TComponent* Owner);

    // *** Motor 1 Graph ***
    int nTmpCount1;
	float fTmpCount1;

    void Graph_Start1(int);

	int nxTime1;
	// *** Motor 1 Graph ***
    int nTmpCount24;
    float fTmpCount24;

    void Graph_Start24(int);

	int nxTime24;

};

// ---------------------------------------------------------------------------
extern PACKAGE TVelGraphForm *VelGraphForm;
// ---------------------------------------------------------------------------
#endif
