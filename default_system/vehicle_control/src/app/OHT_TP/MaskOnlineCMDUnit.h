// ---------------------------------------------------------------------------

#ifndef MaskOnlineCMDUnitH
#define MaskOnlineCMDUnitH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include <Grids.hpp>

#include "include/Def_OHT.h"
#include "include/Def_TP.h"
#include "../Common/PUtily.h"
#include "../Common/Protocol_Define_Modify.h"
// ---------------------------------------------------------------------------
class TMaskOnlineCMDForm : public TForm
{
__published: // IDE-managed Components
    TPanel *panBottom;
    TPopupMenu *pmOriginSet;
	TMenuItem *mniMemoClear;
    TPanel *panFunc;
    TTimer *timerUpdateDisplay;
    TPanel *panTop;
    TPanel *panMid;
    TPanel *panName;
    TPageControl *pgcMainControl;
    TTabSheet *TabSheet2;
    TPanel *Panel7;
    TMenuItem *mniN1;
    TMenuItem *mniHoistOriginSet;
    TMenuItem *mniShiftOriginSet;
    TMenuItem *mniN2;
    TPanel *Panel13;
    TRichEdit *PMACMemo;
    TPanel *Panel14;
    TPanel *panManual;
    TGroupBox *grpSteering;
    TPanel *Panel17;
    TSpeedButton *btnSteeringRight;
    TSpeedButton *btnSteeringLeft;
    TPanel *Panel18;
    TShape *RStrRightLt;
    TShape *FStrRightLt;
    TLabel *Label17;
    TLabel *Label18;
    TPanel *Panel19;
    TShape *RStrLeftLt;
    TShape *FStrLeftLt;
    TLabel *Label19;
    TLabel *Label20;
    TGroupBox *grpHand;
    TPanel *Panel20;
    TSpeedButton *btnHandOrigin;
	TSpeedButton *btnHandHome;
	TSpeedButton *btnHandWork;
    TSpeedButton *btnHandReset;
    TPanel *panHandSensor;
    TShape *HandInPosLt;
    TShape *HandAlarmLt;
	TShape *HandHomeLt;
	TShape *HandWorkLt;
    TLabel *Label21;
    TLabel *Label22;
    TLabel *Label23;
    TLabel *Label24;
    TGroupBox *grpShutter;
    TPanel *panShutter;
    TSpeedButton *btnShutterClose;
    TSpeedButton *btnShutterOpen;
    TShape *shpShutterClose_Front;
    TPanel *panShutterFront;
    TShape *shpShutterOpen_Front;
    TTabSheet *TabSheet5;
    TStringGrid *PWGrid;
    TGroupBox *GroupBox7;
    TBitBtn *btnHaON;
    TBitBtn *btnHaOFF;
    TBitBtn *btnHaRst;
    TShape *GLLt;
    TShape *GRLt;
    TLabel *GLLa;
    TLabel *GRLa;
    TTimer *timerTransLongRun;
    TPanel *Panel4;
    TPanel *panVer;
    TPanel *panEdc;
    TPanel *panErec;
    TPanel *panHIDError;
    TLabel *lblErrString;
    TPanel *Panel25;
    TPanel *Panel27;
    TSpeedButton *SpeedButton4;
    TMenuItem *mniAbsEncoderReading;
    TPageControl *pgcPosVel;
    TTabSheet *TabSheet6;
    TTabSheet *TabSheet7;
    TLabel *lblM4Caption;
    TLabel *Label10;
    TLabel *Label5;
    TLabel *Label3;
    TPanel *panPosition_Driving_F;
    TLabel *lblMotor1_pos_F;
    TPanel *panPosition_Hoist;
    TLabel *lblMotor2_pos;
    TPanel *panPosition_Shift;
    TLabel *lblMotor3_pos;
    TPanel *panPosition_Rotate;
    TLabel *lblMotor4_pos;
    TLabel *Label29;
    TPanel *Panel29;
    TLabel *lblVel1_F_rpm;
    TLabel *Label31;
    TPanel *Panel30;
    TLabel *lblVel2_rpm;
    TLabel *Label33;
    TPanel *Panel31;
    TLabel *lblVel3_rpm;
    TLabel *Label35;
    TPanel *Panel32;
    TLabel *lblVel4_rpm;
    TSpeedButton *btnShutterAlarmReset;
    TShape *shpShutterAlarm_Front;
    TLabel *Label38;
    TLabel *Label39;
    TLabel *Label40;
    TLabel *Label41;
    TPanel *panPDownPercent;
    TLabel *lblPMACDown;
    TPanel *panTeachingSave;
    TPanel *panTrans;
    TButton *btnLoad;
    TButton *btnUnload;
    TCheckBox *chkLongRun;
    TPanel *Panel36;
    TLabel *lblVel1_F_mpersec;
    TLabel *Label12;
    TPanel *panPosition_Driving_R;
    TLabel *lblMotor1_pos_R;
    TPanel *Panel39;
    TLabel *lblVel1_R_rpm;
    TPanel *Panel42;
    TLabel *lblVel1_R_mpersec;
    TPanel *panAx1_F_ServoON;
    TPanel *panAx1_R_ServoON;
    TPanel *panAx2_ServoON;
    TPanel *panAx3_ServoON;
    TPanel *panAx4_ServoON;
    TPopupMenu *pmDrivingFServoOnOff;
    TPopupMenu *pmDrivingRServoOnOff;
    TPopupMenu *pmHoistServoOnOff;
    TPopupMenu *pmShiftServoOnOff;
    TPopupMenu *pmRotate;
    TMenuItem *mniDrvFServoOn;
    TMenuItem *mniDrvFServoOff;
    TMenuItem *mniDrivingRServoOn;
    TMenuItem *mniDrivingRServoOff;
    TMenuItem *mniHoistServoOn;
    TMenuItem *mniHoistServoOff;
    TMenuItem *mniShiftServoOn;
    TMenuItem *mniShiftServoOff;
    TMenuItem *mniRotateServoOn;
    TMenuItem *mniRotateServoOff;
    TMenuItem *mniRotateOriginSet;
    TPanel *HandGoLt;
    TSpeedButton *btnHandFree;
    TPanel *panFStrTorque;
    TLabel *Label30;
    TLabel *Label32;
    TPanel *panRStrTorque;
    TSpeedButton *btnShutterOrigin;
    TSpeedButton *btnShutterFree;
    TPanel *HandOriginLt;
    TPanel *panShtrOrigin;
    TShape *shpShutterInposition_Front;
    TGroupBox *GroupBox1;
    TBitBtn *btnFSteerON;
    TBitBtn *btnFSteerOFF;
    TBitBtn *btnRSteerON;
    TBitBtn *btnRSteerOFF;
    TLabel *Label46;
    TLabel *Label47;
    TPanel *Panel44;
    TPanel *panState_F;
    TPanel *panState_R;
    TPanel *panState_H;
    TPanel *panState_S;
    TPanel *panSource_F;
    TPanel *panSource_R;
    TPanel *panSource_H;
    TPanel *panSource_S;
    TPopupMenu *pmUnload;
    TPanel *panKw;
    TLabel *lblM4Origin;
    TPanel *panAx4_Origin;
    TLabel *Label55;
    TLabel *Label56;
    TPopupMenu *pmLoad;
    TMenuItem *mniLoadEQ;
	TMenuItem *mniLoadEQ_X;
    TMenuItem *N15;
    TMenuItem *mniLoadSTB_L;
    TMenuItem *mniLoadSTB_R;
    TPanel *panPickupTemper;
    TLabel *lblP1;
    TLabel *lblP2;
    TLabel *Label59;
    TMenuItem *N32;
    TMenuItem *mniLoadUTB;
    TLabel *lblEdc;
    TLabel *lblErec;
    TLabel *lblKw;
    TSpeedButton *btnTractionLeft;
    TSpeedButton *btnTractionRight;
    TLabel *lblIPCTemp;
    TLabel *lbLongrunCount;
    TLabel *lbTransTT;
    TTabSheet *ts1;
    TLabel *Label6;
    TEdit *edtAcceleration;
    TEdit *edtDeceleration;
    TLabel *Label7;
    TEdit *edtSpeed;
    TUpDown *UpDown2;
    TPanel *panMPerMin;
    TLabel *Label9;
    TLabel *Label42;
    TPanel *panCenter;
    TLabel *Label4;
    TPanel *panMPerSec;
    TCheckBox *chkDist;
    TPageControl *pgcJog;
    TTabSheet *tabJog;
    TPanel *pan1;
    TLabel *lbJog;
    TPanel *pan2;
    TMemo *memoJogKeyControl;
    TTabSheet *tabMoveDist;
    TPanel *pan3;
    TLabel *Label48;
    TLabel *Label49;
    TEdit *edtDistmm;
    TEdit *edtDistcts;
    TButton *btnMove;
    TPanel *panDist_M;
    TGroupBox *GroupBox6;
    TPanel *panMC_Status;
    TBitBtn *btnMCControlOff;
    TBitBtn *btnMCControlOn;
    TCheckBox *chkTractionMode;
    TBitBtn *btnMotor2Home;
    TBitBtn *btnMotor34Home;
    TMenuItem *N3;
    TMenuItem *mniLoadLOADER;
    TMenuItem *mniUnloadEQ;
    TMenuItem *mniUnloadEQ_X;
    TMenuItem *N5;
    TMenuItem *mniUnloadSTB_L;
    TMenuItem *mniUnloadSTB_R;
    TMenuItem *N8;
    TMenuItem *mniUnloadUTB;
    TMenuItem *N9;
    TMenuItem *mniUnloadLOADER;
    TEdit *edtLongRunInterval;
    TLabel *lbLongrunInterval;
    TTimer *timerDrivingJog;
    TPanel *panMark;
    TGroupBox *grpDrivingMark;
    TPanel *pan5;
    TSpeedButton *btnDrivingMarkForward;
    TSpeedButton *btnDrivingBCR;
    TPanel *panDrivingMarkSensor;
    TLabel *Label64;
    TShape *shpF_DrivingSen;
    TLabel *Label63;
    TShape *shpBCRDriving;
    TLabel *Label67;
    TGroupBox *grpTransMark;
    TPanel *pan7;
    TSpeedButton *btnStopTagBCR;
    TSpeedButton *btnTransMarkForward;
    TPanel *panStopTagMarkSensor;
    TShape *shpR_TransSen;
    TShape *shpF_TransSen;
    TLabel *Label14;
    TLabel *Label16;
    TShape *shpBCRTransLt;
    TLabel *Label43;
    TGroupBox *grpQRMark;
    TPanel *panQRMark;
    TShape *shpRun_Driving_F;
    TShape *shpRun_Driving_R;
    TShape *shpRun_Hoist;
    TShape *shpRun_Shift;
    TShape *shpRun_Rotate;
    TPanel *panSensor;
    TPanel *panHoist_Home;
    TPanel *panShift_Home;
    TPanel *panOscillation;
    TPanel *panUTB;
    TPanel *panLookdown;
	TPanel *panFFUFailHoist;
    TPanel *panUBG;
    TPanel *panUBG_OUT4;
    TPanel *panUBG_OUT3;
    TPanel *panUBG_OUT2;
    TPanel *panUBG_OUT1;
    TPanel *panUBG_OUT5;
    TPanel *panUBG_OUT6;
    TPanel *panUBG_FAIL;
    TMenuItem *mniUnloadSTOCKER;
    TMenuItem *mniLoadSTOCKER;
    TLabel *lbShutterOpen_Front;
    TLabel *lbShutterClose_Front;
    TLabel *lbShutterAlarm_Front;
    TLabel *lbShutterInposition_Front;
    TBitBtn *btnMoveTeachingPoint;
    TPopupMenu *pmMoveTP;
    TMenuItem *mniMoveTPEQ;
    TMenuItem *mniMoveTPEQ_X;
    TMenuItem *mni3;
    TMenuItem *mniMoveTPSTOCKER;
    TMenuItem *mniMoveTPLOADER;
    TMenuItem *mni6;
    TMenuItem *mniMoveTPSTB_L;
    TMenuItem *mniMoveTPSTB_R;
    TMenuItem *mni9;
    TMenuItem *mniMoveTPUTB;
	TBitBtn *btnHoistHomeAutoSet;
	TTabSheet *TabSheet3;
	TMemo *mmCom;
	TButton *btnForceOpen;
	TTimer *tmrTransRun;
    TTimer *tmrCidInfoRead;
	TEdit *edtMoveEnd;
    TCheckBox *cbCheckGoStation;
	TButton *btnMapBaseMove;
	TEdit *edtMoveResult;
    TLabel *Label25;
    TPanel *panTpPause;
    TPanel *panOcsPause;
    TPanel *panPassPermit;
    TBitBtn *btnMoveAllHome;
    TBitBtn *btnAutoChange;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox3;
	TCheckBox *chbBackWard;
	TButton *Button1;
	TButton *btnOCSMapBaseMove;
	TPanel *pnlJogMoveAlarm;
	TPanel *InputVolt1;
	TLabel *lbl1;
	TPanel *InputVolt2;
	TPanel *InputVolt2_1;
	TPanel *InputVolt3;
	TPanel *InputVolt4;
	TLabel *Label87;
	TShape *shpCycliing;
	TPanel *pnlFanFailAlarm;
	TCheckBox *chkEmergencyJog;
	TSpeedButton *btnHandILHome;
	TSpeedButton *btnHandILWork;
	TPanel *panDetect;
	TPanel *panUBGFail;
	TPanel *panUBGShort;
	TPanel *panUBGMiddle;
	TPanel *panUBGLong;
	TPanel *panFObspan;
	TPanel *panOBSFail;
	TPanel *panOBSShort;
	TPanel *panOBSMiddle;
	TPanel *panOBSLong;
	TLabel *lblFObslbl;
	TLabel *Label37;
	TPanel *panM1_F;
	TPanel *panM1_R;
	TPanel *panM3;
	TPanel *panM2;
	TPanel *panM4;
	TPanel *panHLoadPosition;
	TPanel *panHUnloadPosition;
	TPanel *panEUVPodHome;
	TPanel *panRSP150Home;
	TPanel *panRSP200Home;
	TPanel *panAntidropPodDetect1;
	TPanel *panAntidropPodDetect2;
	TGroupBox *GroupBox2;
	TPanel *panEqPioInCaption;
	TPanel *panEQPIOIn;
	TPanel *panEqPIOOutCaption;
	TPanel *panEQPIOOut;
	TPanel *panAntidropPodDetect3;
	TPanel *panAntidropPodDetect4;
	TPanel *panEUVPodDetect;
	TPanel *panRSP150Detect;
	TPanel *panRSP200Detect;
	TPanel *panPODDetect;
	TPanel *panHand_TP_Con;
	TPanel *panFFUFailFrame;
	TTabSheet *TabSheet4;
	TBitBtn *BitBtn1;
	TButton *btnDownUnloadPosition;
	TButton *btnDownLoadPosition;
	TButton *btnUpLoadPosition;
	TButton *btnDownAntidropNone;
	TButton *btnDownAntidropEUV;
	TButton *btnDownAntidropRSP200;
	TButton *btnDownAntidropRSP150;
	TTabSheet *TabSheet1;
	TLabel *lbStopNodeID;
	TLabel *lbStopStationID;
	TLabel *lbStopStationOffset;
	TLabel *lbNextNodeID;
	TLabel *lbStopStationType;
	TLabel *lbPreSteeringNodeID;
	TBitBtn *btnGo;
	TCheckBox *chkGoStation;
	TEdit *edtStopNodeID;
	TEdit *edtStopStationID;
	TEdit *edtStopStationsOffset;
	TEdit *edtNextNodeID;
	TComboBox *cbbStopStationType;
	TEdit *edtPreSteeringNodeID;
	TCheckBox *chkFromStation;
	TLabel *lblDetect;
	// TTimer *Timer1;

    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall mniMemoClearClick(TObject *Sender);
	void __fastcall btnHandHomeClick(TObject *Sender);
    void __fastcall btnHandWorkClick(TObject *Sender);
	void __fastcall btnGoClick(TObject *Sender);
    void __fastcall btnStopClick(TObject *Sender);
    void __fastcall btnTransMarkForwardClick(TObject *Sender);
    void __fastcall btnTransMarkBackwardClick(TObject *Sender);
    void __fastcall memoJogKeyControlKeyDown(TObject *Sender, WORD &Key,
        TShiftState Shift);
    void __fastcall memoJogKeyControlKeyUp(TObject *Sender, WORD &Key,
        TShiftState Shift);
    void __fastcall btnHandOriginClick(TObject *Sender);
    void __fastcall btnHandResetClick(TObject *Sender);
    void __fastcall btnSteeringRightClick(TObject *Sender);
    void __fastcall btnShutterOpenClick(TObject *Sender);
    void __fastcall btnShutterCloseClick(TObject *Sender);
    void __fastcall timerUpdateDisplayTimer(TObject *Sender);
    void __fastcall btnStopTagBCRClick(TObject *Sender);
    void __fastcall btnMotor2HomeClick(TObject *Sender);
    void __fastcall mniHoistOriginSetClick(TObject *Sender);
    void __fastcall mniShiftOriginSetClick(TObject *Sender);
    void __fastcall edtSpeedChange(TObject *Sender);
    void __fastcall btnMoveStopClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall pgcMainControlDrawTab(TCustomTabControl *Control,
        int TabIndex, const TRect &Rect, bool Active);
    void __fastcall btnSteeringLeftClick(TObject *Sender);
    void __fastcall PWGridDrawCell(TObject *Sender, int ACol, int ARow,
        TRect &Rect, TGridDrawState State);
    void __fastcall btnHaONClick(TObject *Sender);
    void __fastcall btnHaOFFClick(TObject *Sender);
    void __fastcall btnHaRstClick(TObject *Sender);
    void __fastcall timerTransLongRunTimer(TObject *Sender);
    void __fastcall chkLongRunClick(TObject *Sender);
    void __fastcall btnShutterAlarmResetClick(TObject *Sender);
    void __fastcall btnMotor34HomeClick(TObject *Sender);
	void __fastcall btnLoadMouseDown(TObject *Sender, TMouseButton Button,
        TShiftState Shift, int X, int Y);
    void __fastcall btnUnloadMouseDown(TObject *Sender, TMouseButton Button,
        TShiftState Shift, int X, int Y);
    void __fastcall pmDrivingFServoOnOffPopup(TObject *Sender);
    void __fastcall pmDrivingRServoOnOffPopup(TObject *Sender);
    void __fastcall pmHoistServoOnOffPopup(TObject *Sender);
    void __fastcall pmShiftServoOnOffPopup(TObject *Sender);
    void __fastcall mniDrvFServoOnClick(TObject *Sender);
    void __fastcall mniDrvFServoOffClick(TObject *Sender);
    void __fastcall mniDrivingRServoOnClick(TObject *Sender);
    void __fastcall mniDrivingRServoOffClick(TObject *Sender);
    void __fastcall mniHoistServoOnClick(TObject *Sender);
    void __fastcall mniHoistServoOffClick(TObject *Sender);
    void __fastcall mniShiftServoOnClick(TObject *Sender);
	void __fastcall mniShiftServoOffClick(TObject *Sender);
    void __fastcall btnHandFreeClick(TObject *Sender);
    void __fastcall btnShutterOriginClick(TObject *Sender);
    void __fastcall btnShutterFreeClick(TObject *Sender);
    void __fastcall btnFSteerONClick(TObject *Sender);
    void __fastcall btnFSteerOFFClick(TObject *Sender);
    void __fastcall btnRSteerONClick(TObject *Sender);
    void __fastcall btnRSteerOFFClick(TObject *Sender);
    void __fastcall chkDistClick(TObject *Sender);
    void __fastcall edtDistmmChange(TObject *Sender);
    void __fastcall edtDistctsChange(TObject *Sender);
    void __fastcall btnMoveClick(TObject *Sender);
    void __fastcall btnScanUTBClick(TObject *Sender);
    void __fastcall mniLoadEQClick(TObject *Sender);
	void __fastcall mniLoadEQ_XClick(TObject *Sender);
    void __fastcall mniLoadUTBClick(TObject *Sender);
    void __fastcall btnTractionLeftClick(TObject *Sender);
    void __fastcall btnTractionRightClick(TObject *Sender);
	void __fastcall chkTractionModeClick(TObject *Sender);
    void __fastcall memoJogKeyControlExit(TObject *Sender);
    void __fastcall btnMCControlOnClick(TObject *Sender);
	void __fastcall btnMCControlOffClick(TObject *Sender);
    void __fastcall btnDrivingBCRClick(TObject *Sender);
    void __fastcall btnDrivingMarkBackwardClick(TObject *Sender);
	void __fastcall btnDrivingMarkForwardClick(TObject *Sender);
    void __fastcall chkGoStationClick(TObject *Sender);
    void __fastcall mniLoadLOADERClick(TObject *Sender);
    void __fastcall mniUnloadLOADERClick(TObject *Sender);
    void __fastcall mniUnloadEQClick(TObject *Sender);
	void __fastcall mniUnloadEQ_XClick(TObject *Sender);
    void __fastcall mniUnloadUTBClick(TObject *Sender);
    void __fastcall timerDrivingJogTimer(TObject *Sender);
    void __fastcall panTeachingSaveClick(TObject *Sender);
    void __fastcall mniUnloadSTOCKERClick(TObject *Sender);
    void __fastcall mniLoadSTOCKERClick(TObject *Sender);
    void __fastcall memoJogKeyControlClick(TObject *Sender);
	void __fastcall btnMoveTeachingPointMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall mniMoveTPEQClick(TObject *Sender);
	void __fastcall mniMoveTPEQ_XClick(TObject *Sender);
    void __fastcall mniMoveTPSTOCKERClick(TObject *Sender);
	void __fastcall mniMoveTPLOADERClick(TObject *Sender);
    void __fastcall mniMoveTPUTBClick(TObject *Sender);
	void __fastcall btnHoistHomeAutoSetClick(TObject *Sender);
	void __fastcall btnForceOpenClick(TObject *Sender);
	void __fastcall btnMapBaseMoveClick(TObject *Sender);
    void __fastcall btnMoveAllHomeClick(TObject *Sender);
    void __fastcall btnAutoChangeClick(TObject *Sender);
	void __fastcall btnOCSMapBaseMoveClick(TObject *Sender);
    void __fastcall chbBackWardClick(TObject *Sender);
	void __fastcall chkEmergencyJogClick(TObject *Sender);
	void __fastcall btnHandILHomeClick(TObject *Sender);
	void __fastcall btnHandILWorkClick(TObject *Sender);
	void __fastcall btnDownUnloadPositionClick(TObject *Sender);
	void __fastcall btnDownLoadPositionClick(TObject *Sender);
	void __fastcall btnUpLoadPositionClick(TObject *Sender);
	void __fastcall btnDownAntidropNoneClick(TObject *Sender);
	void __fastcall btnDownAntidropEUVClick(TObject *Sender);
	void __fastcall btnDownAntidropRSP150Click(TObject *Sender);
	void __fastcall btnDownAntidropRSP200Click(TObject *Sender);
 //	void __fastcall pgcMainControlChange(TObject *Sender);



private: // User declarations
    void __fastcall WMEXITSIZEMOVE(TMessage &msg);
    int DistChang_mm_2(void);
    int DistChang_cts_2(void);

	int TransLoad(void);
	int TransUnLoad(void);

	int iCidInfo_Direct;
	void PowerOnOff(int iAxis, int iOnOff);

	DWORD dwTimeChange;

protected:
    BEGIN_MESSAGE_MAP
        VCL_MESSAGE_HANDLER(WM_EXITSIZEMOVE, TMessage, WMEXITSIZEMOVE)
    END_MESSAGE_MAP(TForm)

public : // User declarations
    TTextAttributes *__fastcall CurrText(void);

    int nCount;
    String strCMDBuffer[10];
    int nKeyIndex;
    int nCMDCount;

    // by zzang9un 2013.05.28 : 롱런 시간 계산을 위한 변수
    TTime LongrunStartTime;
    TTime LongrunEndTime;

    void PMACDataDisplay(TColor, String);

    // by zzang9un 2013.11.20 : Jog Key 관련 변수
    WORD CurJogKey;

    void Warnning_Display(TColor, String);
    int Check_Invalid_Command(String);
    int Set_UBGColor(int);
    __fastcall TMaskOnlineCMDForm(TComponent *Owner);

    // by zzang9un 2013.01.22 : Trans Node Type Combobox 변환 함수 추가
    BYTE GetTransNodeType(int index);

    // by zzang9un 2013.11.20
    void timerAllDisable();
	void Display_MapMove_Result(PKT_OHT2TP_MOVE_BY_MAP_RESULT PKT);
	void Display_MapMake_Result(PKT_OHT2TP_LOG_DATA PKT);
	void Display_Com(PKT_OHT2OCS PKT);


	char m_VHLNAME[10];   //VHL 실제 호기명 받아오기

};

// ---------------------------------------------------------------------------
extern PACKAGE TMaskOnlineCMDForm *MaskOnlineCMDForm;
// ---------------------------------------------------------------------------
#endif
