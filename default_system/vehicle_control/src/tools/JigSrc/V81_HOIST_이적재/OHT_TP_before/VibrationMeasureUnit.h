//---------------------------------------------------------------------------

#ifndef VibrationMeasureUnitH
#define VibrationMeasureUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "math.h"
#include "Chart.hpp"
#include "Series.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include "List.h"
#include <CheckLst.hpp>
#include "Def_DataType.h"
#include "CMDControlUnit.h"
#include "LList.h"
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include "cgauges.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

typedef enum VIB_SONSOR_CONNECTION_STEP_
{
	VIB_SENSOR_DISCONNECTED	= 0,
	VIB_SENSOR_CONNECTING	= 1,
	VIB_SENSOR_CONNECTED	=  2,
}VIB_SONSOR_CONNECTION_STEP;

//---------------------------------------------------------------------------
#define MAX_SAMPLE 1024
#define NUM_OF_SAMPLE NUM_OF_VIB_SAMPLE
#define MAX_VIBRATION_CNT 100000 // 30s // NUM_OF_SAMPLE의 배수로 설정
#define MAX_VELOCITY_CNT (MAX_VIBRATION_CNT/NUM_OF_SAMPLE) // 30s // 200의 배수로 설정

//#define MAX_VIBRATION_CNT 150
#define BOTTOMAXIS_SEC_MAX 10
#define BOTTOMAXIS_RESOLUTION_SEC_VIB 0.001	// s
#define BOTTOMAXIS_RESOLUTION_SEC_VEL (BOTTOMAXIS_RESOLUTION_SEC_VIB*NUM_OF_SAMPLE)

#define SPEC_OUT_LIST_MAX_COUNT 100


//---------------------------------------------------------------------------
class TfrmVibrationMeasurement : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnInformation;
	TPanel *pnVibMeasureControl;
	TButton *btnVibMeasureStart;
	TButton *btnVibMeasureStop;
	TPanel *PanelGraph;
	TScrollBar *ScrollBarVibTime;
	TChart *chartDrivingMotorSpeed;
	TFastLineSeries *SeriesFrontMotor;
	TFastLineSeries *SeriesRearMotor;
	TFastLineSeries *SeriesTransMotor;
	TFastLineSeries *SeriesMotorPos;
	TChart *chartVibRms;
	TFastLineSeries *SeriesVibRMS;
	TFastLineSeries *SeriesVibX;
	TFastLineSeries *SeriesVibY;
	TFastLineSeries *SeriesVibZ;
	TFastLineSeries *SeriesVibPos;
	TGroupBox *gbVel;
	TShape *shVelRear;
	TShape *shVelFront;
	TCheckBox *cbVelFront;
	TCheckBox *cbVelRear;
	TShape *shVelTrans;
	TCheckBox *cbVelTrans;
	TGroupBox *GroupBox1;
	TShape *shVibY;
	TShape *shVibRMS;
	TShape *shVibX;
	TCheckBox *cbVibRMS;
	TCheckBox *cbVibX;
	TCheckBox *cbVibY;
	TShape *shVibZ;
	TCheckBox *cbVibZ;
	TButton *btnSensorConnection;
	TLabel *LabelStatus;
	TTimer *TimerCheckConnect;
	TPanel *pnSpecOutFilter;
	TListBox *ListBoxSpecOutList;
	TFastLineSeries *SeriesSpecOutPoint;
	TPanel *pnSpecOutFilterControl;
	TLabel *LabelThreshold;
	TEdit *edtThresholdInput;
	TLabel *LabelThreshold2;
	TButton *btnThreshold;
	TButton *btnSpecOutListReset;
	TLabel *LabelThreshold4;
	TEdit *edtThresholdOutput;
	TLabel *LabelThreshold3;
	TPanel *pnSelectSavePath;
	TPanel *pnVibInfo;
	TGroupBox *gbNodeInfo;
	TLabel *LabelOffsetDistance;
	TLabel *LabelCurrentNode;
	TEdit *edtCurrentNode;
	TEdit *edtOffsetDistance;
	TGroupBox *GroupBox2;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *edtSteerFront;
	TEdit *edtSteerRear;
	TGroupBox *gbMotorVelocity;
	TLabel *LabelVhlVel;
	TLabel *LabelTransVel;
	TEdit *edtVhlVel;
	TEdit *edtTransVel;
	TGroupBox *gbVibrationInfo;
	TLabel *LabelVibX;
	TLabel *LabelVibZ;
	TLabel *LabelVibY;
	TLabel *LabelVibRMS;
	TEdit *edtVibX;
	TEdit *edtVibZ;
	TEdit *edtVibY;
	TEdit *edtVibRMS;
	TButton *btnChangeSaveFilePath;
	TEdit *edtSaveFilePath;
	TCheckBox *cbRealTimeLog;
	TSaveDialog *SaveDialog;
	TButton *btnInitData;
	TEdit *edtVelTimeOffset;
	TLabel *Label1;
	TLabel *Label4;
	TPageControl *pgcAVSMenu;
	TTabSheet *tsConnection;
	TTabSheet *tsPlayback;
	TButton *btFileOpen;
	TOpenDialog *OpenDialog;
	TButton *btInitData;
	TCGauge *cgGauge;
	TButton *btStopDraw;
	TButton *btExecuteFFT;
	TLabel *lbLoadCounter;
	TFastLineSeries *SeriesVibPosRange;
	TFastLineSeries *SeriesMotorPosRange;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *lbTemperature;
	TLabel *lbBattery;
	TCGauge *cgTemperature;
	TCGauge *cgBattery;

	void __fastcall btnVibMeasureStartClick(TObject *Sender);
	void __fastcall btnVibMeasureStopClick(TObject *Sender);

	void __fastcall chartDrivingMotorSpeedClickSeries(TCustomChart *Sender, TChartSeries *Series,
		  int ValueIndex, TMouseButton Button, TShiftState Shift, int X,
		  int Y);
	void __fastcall ScrollBarVibTimeChange(TObject *Sender);
	void __fastcall cbVelFrontClick(TObject *Sender);
	void __fastcall cbVelRearClick(TObject *Sender);
	void __fastcall cbVelTransClick(TObject *Sender);
	void __fastcall cbVibRMSClick(TObject *Sender);
	void __fastcall cbVibXClick(TObject *Sender);
	void __fastcall cbVibYClick(TObject *Sender);
	void __fastcall cbVibZClick(TObject *Sender);
	void __fastcall btnSensorConnectionClick(TObject *Sender);
	void __fastcall TimerCheckConnectTimer(TObject *Sender);
	void __fastcall chartVibRmsClickSeries(TCustomChart *Sender, TChartSeries *Series,
          int ValueIndex, TMouseButton Button, TShiftState Shift, int X,
          int Y);
	void __fastcall btnThresholdClick(TObject *Sender);
	void __fastcall btnSpecOutListResetClick(TObject *Sender);
	void __fastcall ListBoxSpecOutListDblClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall cbRealTimeLogClick(TObject *Sender);
	void __fastcall btnChangeSaveFilePathClick(TObject *Sender);
	void __fastcall btnInitDataClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btFileOpenClick(TObject *Sender);
	void __fastcall btInitDataClick(TObject *Sender);
	void __fastcall btStopDrawClick(TObject *Sender);
	void __fastcall btExecuteFFTClick(TObject *Sender);

private:	// User declarations
	/// log 저장 관련
	AnsiString mStrLogFileName;
	String GetLogFileName(String strFileName);
	bool SaveLog();
	FILE *fp;
	bool bIsFileOpen;
	UINT gDataCnt;

	// private member veriable
	double dVibTime_sec;
	double dVelTime_sec;
	double dMinVibTime_sec;

	int nSampleInScreen;

	bool bVibMeasDrawLine;
	bool bVibMeasMouseDown;
	int nSensorConnectionStep;
	int nSensorConnectionCheckCnt;


	std::list<unsigned int> CurNodeIdList;
	std::list<int> CurNodeOffset;
	std::list<BYTE> SteerFront;
	std::list<BYTE> SteerRear;

	double dVibFilterThreshold;
	WORD tableSpecOutList[100];
	WORD mBackUpUdpListenerInterval;

	float m_fTimeOffset;

	// private method
	void MeasurementInitialization(void);
	void DrawCheckLineDisplayInfo(int ValueIndex, double X);

	bool m_bStopDrawChart;
public:		// User declarations
	__fastcall TfrmVibrationMeasurement(TComponent* Owner);
	void DisplayVibrationData(AVS_VIBRATION_DATA_INFO VibrationInfo);
	void Receive_AVS_Response(int nResponse);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmVibrationMeasurement *frmVibrationMeasurement;
//---------------------------------------------------------------------------
#endif
