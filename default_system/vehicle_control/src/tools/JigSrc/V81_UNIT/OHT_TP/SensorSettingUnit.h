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

typedef enum SENSOR_INSTALL_RESULT_
{
	SENSOR_INSTALL_RESULT_OK = 1,
	SENSOR_INSTALL_RESULT_NG_SENSOR_LEVEL_LOW,
	SENSOR_INSTALL_RESULT_NG_MOVE_LEFT,
	SENSOR_INSTALL_RESULT_NG_MOVE_RIGHT,
	SENSOR_INSTALL_RESULT_NG_REFLECTOR_BAD,
	SENSOR_INSTALL_RESULT_NG_OBS_VERTICAL_LEVEL,
	SENSOR_INSTALL_RESULT_NG_OHT_POSITION,
	SENSOR_INSTALL_RESULT_NG_OHT_VERTICAL_LEVEL
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
    TLabel *Label3;
    TEdit *edt_SensorCenter;
    TLabel *Label5;
    TEdit *edt_SensorLeft;
    TLabel *Label6;
    TEdit *edt_SensorRight;
    TEdit *edt_SensorSettingResult;
    TButton *Btn_Check_Start;
    TButton *Btn_Check_Stop;
    TLabel *Label4;
    TMemo *Sensor_Debug;
    TEdit *Edit1;
    TEdit *edtInform;
    TTimer *SensorDataTimer;
    TLabel *Label7;
    TLabel *Label8;
    TEdit *edt_start_pos;
    TEdit *edt_end_pos;
    TLabel *Label9;
	TBitBtn *btnSave;
    void __fastcall SensorData_ComboBoxChange(TObject *Sender);
    void __fastcall SensorDataTimerTimer(TObject *Sender);
    void __fastcall Btn_Check_StartClick(TObject *Sender);
    void __fastcall Btn_Check_StopClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
	void __fastcall btnSaveClick(TObject *Sender);
private:	// User declarations
    int nSensorType;
    bool Check_CenterPos(int *data);
	int Cal_Dis(int cnt, int dis, char type);
	int m_nResult;

public:		// User declarations
    __fastcall TOBS_UBG_Form(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TOBS_UBG_Form *OBS_UBG_Form;
//---------------------------------------------------------------------------
#endif
