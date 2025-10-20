// ---------------------------------------------------------------------------

#ifndef TeachingUnitH
#define TeachingUnitH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>

#include "../Common/Def_DataType.h"
#include "../Common/Protocol_Define_Modify.h"

// by zzang9un 2013.08.30 : Data 범위 체크 값
#define MIN_PIOTIMELEVEL            1
#define MAX_PIOTIMELEVEL            10
#define MIN_LOOKDOWNLEVEL           1
#define MAX_LOOKDOWNLEVEL           16

#define HOIST_JOG_NONE				0
#define HOIST_JOG_DOWN				1
#define HOIST_JOG_UP				2
#define SHIFT_JOG_NONE				0
#define SHIFT_JOG_LEFT				1
#define SHIFT_JOG_RIGHT				2
#define ROTATE_JOG_NONE             0
#define ROTATE_JOG_CW               1
#define ROTATE_JOG_CCW              2

#define HoistNegativeLimit4PulseMoving -5.0

typedef enum EQPIO_TYPE_INFO_
{
	EQ_PIO_IR   = 0,
	EQ_PIO_HYBRID = 1,
	EQ_PIO_RF   = 2,
	EQ_PIO_HYBRID_SN   = 3
}EQPIO_TYPE_INFO;

// ---------------------------------------------------------------------------
class TTeachingForm : public TForm
{
__published: // IDE-managed Components

    TPanel *panTop;
    TGroupBox *grpHoistTeaching;
    TLabel *lblHoistPos;
    TPanel *Panel10;
    TLabel *lbHoistCurCts;
    TShape *shpHoistRunning;
    TBitBtn *btnHoistMoveHome;
    TBitBtn *btnHoistMoveUp;
    TBitBtn *btnHoistMoveDown;
    TGroupBox *grpRotateTeaching;
    TLabel *lblRotatePos;
    TPanel *Panel3;
    TLabel *lblMotor4;
    TShape *shp4Run;
    TBitBtn *btnMotor4Home;
    TBitBtn *btn4CCWCmd;
    TBitBtn *btn4CWCmd;
    TGroupBox *grpShiftTeaching;
    TLabel *lblShiftPos;
    TPanel *Panel4;
    TLabel *lbShiftCurCts;
    TShape *shpShiftRunning;
    TBitBtn *btnShiftMoveHome;
    TBitBtn *btnShiftMoveRight;
    TBitBtn *btnShiftMoveLeft;
    TTimer *timerUpdateDisplay;
    TPanel *panRotateMotor;
    TLabel *lblDMotor4;
    TButton *btnRotateOrigin;
    TEdit *edtHoistNegLimit;
    TButton *btnSetHoistNegLimit;
    TEdit *edtHoistPosLimit;
    TButton *btnSetHoistPosLimit;
    TEdit *edtShiftNegLimit;
    TEdit *edtShiftPosLimit;
    TButton *btnSetShiftNegLimit;
    TButton *btnSetShiftPosLimit;
    TTimer *ATTimer;
    TPanel *panHoistMotor;
    TLabel *lbHoistCurPos;
    TPanel *panShiftMotor;
    TLabel *lbShiftCurPos;
    TGroupBox *grpHoistJog;
    TCheckBox *chkHoistJogABS;
    TCheckBox *chkHoistJogDist;
    TEdit *edtHoistJogABS;
    TEdit *edtHoistJogDist;
    TUpDown *udHoistDist;
    TBitBtn *btnHoistMoveUpDist;
    TBitBtn *btnHoistMoveDownDist;
    TBitBtn *btnHoistMoveABS;
    TGroupBox *grpShiftJog;
    TCheckBox *chkShiftJogABS;
    TCheckBox *chkShiftJogDist;
    TEdit *edtShiftJogABS;
    TEdit *edtShiftJogDist;
    TUpDown *udShiftDist;
    TGroupBox *grpRotateJog;
    TCheckBox *chkRotateJogABS;
    TCheckBox *chkRotateJogDist;
    TEdit *edtRotateJogABS;
    TEdit *edtRotateJogDist;
    TUpDown *udRotateDist;
    TBitBtn *btnShiftMoveLeftDist;
    TBitBtn *btnShiftMoveABS;
    TBitBtn *btnShiftMoveRightDist;
    TBitBtn *btnRotateMoveABS;
    TBitBtn *btnRotateMoveRight;
    TBitBtn *btnRotateMoveLeft;
    TPageControl *pgcTeaching;
    TTabSheet *tabJog;
    TTabSheet *tabTeachingData;
    TGroupBox *grpTeachingData;
    TLabel *lblPortType;
    TLabel *lblHoist;
    TLabel *lblShift;
    TLabel *lblRotate;
    TLabel *lblPIODirection;
    TLabel *lblPIOTimeLevel;
    TLabel *lblLookDownLevel;
    TEdit *edtHoist;
    TComboBox *cbbPortType;
    TEdit *edtShift;
    TEdit *edtRotate;
    TEdit *edtPIOTimeLevel;
    TComboBox *cbbPIODirection;
    TEdit *edtLookDownLevel;
    TGroupBox *grpTransOption;
    TCheckBox *chkRFReader;
    TCheckBox *chkTransSound;
    TCheckBox *chkOscillation;
    TCheckBox *chkHandDetectEQ;
    TButton *btnTeachingSave;
    TButton *btnTeachingDataReq;
    TButton *btnDefault;
    TGroupBox *grpPreTeachingData;
    TLabel *lbl1;
    TLabel *lbl2;
    TLabel *lbl3;
    TLabel *lbl4;
    TLabel *lbl5;
    TLabel *lbl6;
    TLabel *lbl7;
    TEdit *edtPre_Hoist;
    TComboBox *cbbPre_PortType;
    TEdit *edtPre_Shift;
    TEdit *edtPre_Rotate;
    TEdit *edtPre_PIOTimeLevel;
    TComboBox *cbbPre_PIODirection;
    TEdit *edtPre_LookdownLevel;
    TGroupBox *grpPre_TransOption;
    TCheckBox *chkPre_RFReader;
    TCheckBox *chkPre_TransSound;
    TCheckBox *chkPre_Oscillation;
    TCheckBox *chkPre_HandDetectEQ;
	TTimer *tmrJogMove;
	TLabel *Label1;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TEdit *edtPre_PIOID;
	TComboBox *cbbPre_PIOType;
	TEdit *edtPre_PIOChannel;
	TComboBox *cbbPIOType;
	TEdit *edtPIOID;
	TEdit *edtPIOChannel;
	TTabSheet *AutoTeahcingData;
	TGroupBox *GroupBox1;
	TLabel *Label15;
	TComboBox *ComboBox1;
	TMemo *memoAT;
	TLabel *Label16;
	TEdit *edtStationIDAT;
	TButton *Button3;
	TButton *Button4;
	TButton *Button5;
	TButton *Button2;
	TLabel *Label17;
	TComboBox *ComboBox2;
	TButton *btnNullType;
	TLabel *Label2;
	TComboBox *cbbPre_SpecailEQLevel;
	TLabel *Label14;
	TComboBox *cbbSpecailEQLevel;
	TTabSheet *tabVisionAutoTeaching;
	TLabel *Label18;
	TComboBox *cbb1;
	TButton *btnVisionAutoTeaching;
	TGroupBox *grphoistpulse;
	TLabel *hoistpulse;
	TEdit *hoistpulsevalue;
	TButton *btnPULSE_MOVE;
	TEdit *hoistpulSpeed;
	TLabel *Label19;
	TLabel *lbl8;
	TEdit *edtPre_PIOCS;
	TLabel *lbl9;
	TEdit *edtPIOCS;
	TButton *btnCheckPodEuv;
	TComboBox *cbbMaterialType;
	TLabel *MaterialType;
	TLabel *preMaterialType;
	TComboBox *cbbPre_MaterialType;

    void __fastcall FormShow(TObject *Sender);
    void __fastcall udHoistDistChanging(TObject *Sender, bool &AllowChange);
    void __fastcall edtHoistJogABSClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall timerUpdateDisplayTimer(TObject *Sender);
    void __fastcall btnHoistMoveHomeClick(TObject *Sender);
    void __fastcall btnShiftMoveHomeClick(TObject *Sender);
    void __fastcall btnHoistMoveUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnHoistMoveUpMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnHoistMoveDownMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnShiftMoveLeftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnShiftMoveLeftMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnShiftMoveRightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btn4CWCmdMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btn4CWCmdMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btn4CCWCmdMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnHoistMoveUpDistClick(TObject *Sender);
    void __fastcall btnHoistMoveABSClick(TObject *Sender);
    void __fastcall btnHoistMoveDownDistClick(TObject *Sender);
    void __fastcall btnShiftMoveLeftDistClick(TObject *Sender);
    void __fastcall btnShiftMoveRightDistClick(TObject *Sender);
    void __fastcall btnShiftMoveABSClick(TObject *Sender);
    void __fastcall btnRotateMoveLeftClick(TObject *Sender);
    void __fastcall btnRotateMoveRightClick(TObject *Sender);
    void __fastcall btnRotateMoveABSClick(TObject *Sender);
    void __fastcall btnRotateOriginClick(TObject *Sender);
    void __fastcall btnSetHoistNegLimitClick(TObject *Sender);
    void __fastcall btnSetHoistPosLimitClick(TObject *Sender);
    void __fastcall btnSetShiftNegLimitClick(TObject *Sender);
    void __fastcall btnSetShiftPosLimitClick(TObject *Sender);
    void __fastcall ATTimerTimer(TObject *Sender);
    void __fastcall udShiftDistChanging(TObject *Sender, bool &AllowChange);
    void __fastcall udRotateDistChanging(TObject *Sender, bool &AllowChange);
    void __fastcall edtShiftJogABSClick(TObject *Sender);
    void __fastcall edtHoistJogDistClick(TObject *Sender);
    void __fastcall edtShiftJogDistClick(TObject *Sender);
    void __fastcall edtRotateJogABSClick(TObject *Sender);
    void __fastcall chkHoistJogABSMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall chkHoistJogDistMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall chkShiftJogABSMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
    void __fastcall chkShiftJogDistMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall chkRotateJogABSMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall chkRotateJogDistMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall btnTeachingSaveClick(TObject *Sender);
    void __fastcall btnTeachingDataReqClick(TObject *Sender);
    void __fastcall btnDefaultClick(TObject *Sender);
	void __fastcall tmrJogMoveTimer(TObject *Sender);
	void __fastcall btnHoistMoveUpMouseLeave(TObject *Sender);
	void __fastcall btnHoistMoveDownMouseLeave(TObject *Sender);
	void __fastcall btnShiftMoveRightMouseLeave(TObject *Sender);
	void __fastcall btnShiftMoveLeftMouseLeave(TObject *Sender);
    void __fastcall btnMotor4HomeClick(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall btnNullTypeClick(TObject *Sender);
	void __fastcall btnVisionAutoTeachingClick(TObject *Sender);
	void __fastcall btnPULSE_MOVEClick(TObject *Sender);
	void __fastcall btnCheckPodEuvClick(TObject *Sender);
private: // User declarations
	double m_dHoistPos;
	double m_dShiftPos;
	double m_dRotatePos;

    bool bHandCheck;
public : // User declarations
    __fastcall TTeachingForm(TComponent* Owner);

    void SetHoistJogUI(BYTE JogSetting);
    void SetShiftJogUI(BYTE JogSetting);
    void SetRotateJogUI(BYTE JogSetting);

    int Set_Axis4_EnDisPosEdt(void);

    double dAT2Data;
    double dAT3Data;
    double dAT4Data;
    double dCts2mm;

	//Hoist, Shift move용 변수들
	bool bHoistMoveCommand;
	bool bShiftMoveCommand;
    bool bRotateMoveCommand;
	bool bCheckMoveHoist;
	bool bCheckMoveShift;
    bool bCheckMoveRotate;
	int nHoistMoveDir;
	int nShiftMoveDir;
    int nRotateMoveDir;
	double m_dHoistNegLimit;


    int Delete_Comma(String);
    double Delete_dComma(String);

	void SetDefaultTeachingData();
    //void DisplayTeachingData(uTP_CMD *pTData);
	void DisplayTeachingData(PKT_OHT2TP_TEACHING_INFO *pTData);
	void Display_Teaching_Result(PKT_TP2OHT_SIMPLE_COMMAND PKT);
	void DisplayAutoTeachingData(PKT_OHT2TP_TEACHING_INFO *pTData);

};

// ---------------------------------------------------------------------------
extern PACKAGE TTeachingForm *TeachingForm;
// ---------------------------------------------------------------------------
#endif
