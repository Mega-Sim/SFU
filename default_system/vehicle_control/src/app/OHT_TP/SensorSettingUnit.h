//---------------------------------------------------------------------------

#ifndef SensorSettingUnitH
#define SensorSettingUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include "perfgrap.h"
#include "Chart.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include "Series.hpp"
#include "math.h"
#include <ComCtrls.hpp>
#include <jpeg.hpp>
#include <Buttons.hpp>

#define SensorUnitDegree            0.3515625
#define  MATH_PI                    3.1415926535897932384626433832795
#define Sensor_Data_Center          257
#define Sensor_Data_Num             513
#define LookDown_Sensor_Data_Center 61
#define MaxLimit                    918
#define MinLimit                    718
#define SensorUbgOhtMin             54
#define SensorUbgObsMin             24
#define OBS_Sensor_Offset           10
#define OHT_Sensor_Offset           50
#define OBS_Sensor_Margin_Pos       1
#define OBS_Sensor_Margin_Neg       -1
#define OHT_Sensor_Margin_Pos       1
#define OHT_Sensor_Margin_Neg       -1
#define LookDown_Data_Num			121
#define LookDown_SensorUnitDegree   1.5

#define SOSLAB_Sensor_Data_Num             1000
#define SOSLAB_SensorUnitDegree            0.18
#define SOSLAB_Sensor_Data_Center          	500
#define SOSLAB_OHT_Sensor_Offset            100
#define SOSLAB_OBS_Sensor_Offset            20
#define SOSLABSensorUbgOhtMin             106
#define SOSLABSensorOBSOhtMin             46
//#define SOSLABMaxLimit                    918
//#define SOSLABMinLimit                    718
#define SOSLABOHT_Sensor_Margin_Pos       2
#define SOSLABOHT_Sensor_Margin_Neg       -2


typedef enum SENSOR_INSTALL_RESULT_
{
	SENSOR_INSTALL_RESULT_OK = 1,
	SENSOR_INSTALL_RESULT_NG_SENSOR_LEVEL_LOW,
	SENSOR_INSTALL_RESULT_NG_MOVE_LEFT,
	SENSOR_INSTALL_RESULT_NG_MOVE_RIGHT,
	SENSOR_INSTALL_RESULT_NG_REFLECTOR_BAD,
	SENSOR_INSTALL_RESULT_NG_OBS_VERTICAL_LEVEL,
	SENSOR_INSTALL_RESULT_NG_OHT_POSITION,
	SENSOR_INSTALL_RESULT_NG_OHT_VERTICAL_LEVEL,
	SENSOR_INSTALL_RESULT_NG_FINE_DETECT_SENSOR,
}SENSOR_INSTALL_RESULT;


//---------------------------------------------------------------------------
class TOBS_UBG_Form : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TStringGrid *strSensorData;
    TComboBox *SensorData_ComboBox;
    TLabel *Label2;
    TChart *Chart1;
    TFastLineSeries *Series1;
    TImage *Image1;
    TEdit *edt_SensorSettingResult;
    TButton *Btn_Check_Start;
    TButton *Btn_Check_Stop;
    TLabel *Label4;
    TMemo *Sensor_Debug;
    TEdit *Edit1;
    TTimer *SensorDataTimer;
    TLabel *Label9;
	TBitBtn *btnSave;
	TButton *Btn_repair_Stop;
	TButton *Btn_repair_Start;
	TLabel *Label10;
	TLabel *Label11;
	TEdit *edt_SensorCheckResult;
	TTimer *SensorDataTimer2;
	TButton *Btn_repair_Clear;
	TLabel *Label12;
	TEdit *Edt_repair_Count;
	TEdit *edt_TEST;
	TPanel *result_pnl;
	TLabel *lbl1;
	TLabel *lbl2;
	TLabel *lbl3;
	TLabel *lbl4;
	TLabel *lbl5;
	TEdit *edt_end_pos;
	TEdit *edt_SensorCenter;
	TEdit *edt_SensorLeft;
	TEdit *edt_SensorRight;
	TEdit *edt_start_pos;
	TEdit *edtInform;
	TPanel *result_pnl_Fine;
	TLabel *lbl_Center_X;
	TLabel *lbl_Center_Y;
	TLabel *lbl_defalt_X;
	TLabel *lbl_default_Y;
	TLabel *lbl_Center;
	TLabel *lbl_default;
	TEdit *edt_Center_X;
	TEdit *edt_Center_Y;
	TEdit *edt_default_X;
	TEdit *edt_default_Y;
	TEdit *edt_Inform_Fine;
    void __fastcall SensorData_ComboBoxChange(TObject *Sender);
    void __fastcall SensorDataTimerTimer(TObject *Sender);
    void __fastcall Btn_Check_StartClick(TObject *Sender);
    void __fastcall Btn_Check_StopClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
	void __fastcall btnSaveClick(TObject *Sender);
	void __fastcall Btn_repair_StartClick(TObject *Sender);
	void __fastcall SensorDataTimer2Timer(TObject *Sender);
	void __fastcall Btn_repair_StopClick(TObject *Sender);
	void __fastcall Btn_repair_ClearClick(TObject *Sender);
private:	// User declarations
    int nSensorType;
    bool Check_CenterPos(int *data);
	bool Check_CenterPos_SOSLAB(int *data);
	bool Check_CenterPos_LookDown(int *data);
	int Cal_Dis(int cnt, int dis, int type);
	int m_nResult;
	bool Check_CenterPos2(int data);
	int m_nResultCheck;
	int m_nResultCheckCount;

public:		// User declarations
    __fastcall TOBS_UBG_Form(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TOBS_UBG_Form *OBS_UBG_Form;
//---------------------------------------------------------------------------
#endif
