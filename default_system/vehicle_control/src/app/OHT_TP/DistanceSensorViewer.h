//---------------------------------------------------------------------------

#ifndef DistanceSensorViewerH
#define DistanceSensorViewerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>

//---------------------------------------------------------------------------
#define  MATH_PI                    3.1415926535897932384626433832795
#define PBS_DISTANCE_DATA_TRIM_SIZE	10 // -18.0º ~ -1.8º 10개 거리 data 는 제거
#define UBG_DISTANCE_DATA_SIZE 513 // 0º ~ 180º까지 513개 data, 0.35º 간격
#define PBS_DISTANCE_DATA_SIZE (121-PBS_DISTANCE_DATA_TRIM_SIZE*2) // 0º ~ 180º까지 101개 data, 1.8º 간격


#define SOSLABUBG_DISTANCE_DATA_SIZE 1000 // 0º ~ 180º까지 513개 data, 0.35º 간격


#define PBS_DISTANCE_LIMIT 2000
#define UBG_DISTANCE_LIMIT 5300
#define NUM_PBS_SELECT_AREA	15
#define NUM_UBG_SELECT_AREA	31
#define NUM_AREA_POINT	8

#define CONVERT2INT(value) ((value)+0.5)

//---------------------------------------------------------------------------
typedef struct DISTANCE_SENSOR_DATA_
{
	int nSensorType;
	int nDistanceDataSize;
	int *pDistanceData;
	TPoint *pDistancePoint;  //test를 위해 사이즈 지정
//	TPoint pDistancePoint[UBG_DISTANCE_DATA_SIZE+1];   // 삭제 예정
	int nDistanceLimit;
	double dDegreeResolution;
	int indexClosestDistance;
	int nPatternNum;
	TPoint pPatternArea[NUM_UBG_SELECT_AREA][NUM_AREA_POINT];
}DISTANCE_SENSOR_DATA;

typedef enum DISTANCE_SENSOR_CANVAS_TYPE_
{
	DISTANCE_SENSOR_DRAWING_PAPER,
	DISTANCE_SENSOR_DETECT_AREA,
	DISTANCE_SENSOR_PATTERN_AREA,
	DISTANCE_SENSOR_BACK_GROUND,
	DISTANCE_SENSOR_CLOSEST_POINT,
}DISTANCE_SENSOR_CANVAS_TYPE;

//---------------------------------------------------------------------------

class TDistanceSensorViewrForm : public TForm
{
__published:	// IDE-managed Components
	TPaintBox *paintBoxDistanceSensor;
	TTimer *timerDistanceDataUpdate;
	TMainMenu *menuDistanceSensorViewer;
	TMenuItem *N1;
	TMenuItem *mnReadObsFile;
	TMenuItem *mnReadUbgFile;
	TMenuItem *mnReadPbsFile;
	TPanel *panelDistanceSensorViewer;
	TRadioGroup *gbSelectDistanceSensor;
	TGroupBox *gbDistanceDetectInfo;
	TLabel *txtMinDist;
	TLabel *txtMinDegree;
	TLabel *txtPaternInfo;
	TMemo *memoDebug;
	TOpenDialog *OpenDialogDistSenAreaFile;
	void __fastcall gbSelectDistanceSensorClick(TObject *Sender);
	void __fastcall paintBoxDistanceSensorPaint(TObject *Sender);
	void __fastcall timerDistanceDataUpdateTimer(TObject *Sender);
	void __fastcall mnReadObsFileClick(TObject *Sender);
	void __fastcall mnReadUbgFileClick(TObject *Sender);
	void __fastcall mnReadPbsFileClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
	// variables
	DISTANCE_SENSOR_DATA m_distanceSensorData;

	// functions
	void ConvertDistanceToXY(double radius, double scale);
	void DrawSensorData();
	void DrawDetectArea(double radius, double scale);
	void DrawPatternArea(double radius, double scale);
	void DrawBackGround(double radius, double scale);
	void SetCanvas(int CanvasType);
	int GetSensorArea(int SensorType);
	void UpdatePattern();
	void AreaSelect(int nSensorType);
	void UpdateSensorArea(String strFileName, int sensorType);
public:		// User declarations
	__fastcall TDistanceSensorViewrForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDistanceSensorViewrForm *DistanceSensorViewrForm;
//---------------------------------------------------------------------------
#endif
