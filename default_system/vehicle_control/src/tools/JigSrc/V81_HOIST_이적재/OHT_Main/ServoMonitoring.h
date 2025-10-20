//---------------------------------------------------------------------------
/**
@file    ServoMonitoring.h
@version OHT 7.0
@brief   ServoMonitoring Class Header File
*/

#ifndef ServoMonitoringH
#define ServoMonitoringH

#include "HWCommon.h"
#include "CommHandler.h"
#include "ErrorCode.h"

#define  chETX                      0x03

#define RECEIVE_TIME_OUT				50
#define SEND_PK_LENGTH					10
#define RECV_PK_LENGTH					10
#define RECV_PK_LENGTH2					12
#define RECV_ERROR_PK_LENGTH			5
#define CRC_SHIFT_COUNT					8
#define RETRY_MAX_COUNT					3

#define FUNC_CODE		0x40
#define REGISTER_READ	0x0003
#define REGISTER_WRITE	0x0006

#define SERVO_ALARM_COUNT	26
#define SERVO_STATUS_COUNT	27



#ifndef _SCOM_DATA_LENGTH_DEF_
#define _SCOM_DATA_LENGTH_DEF_
typedef enum _SCOM_DATA_LENGTH_DEF_
{
	DATABIT_8	= 0x08,
	DATABIT_7	= 0x07,
	DATABIT_6	= 0x06,
	DATABIT_5	= 0X05
} SCOM_DATA_LENGTH_DEF;
#endif

#ifndef _SCOM_PARITY_DEF_
#define _SCOM_PARITY_DEF_
typedef enum _SCOM_PARITY_DEF_
{
	PARITYBIT_NONE	= 0x00,
	PARITYBIT_ODD	= 0x01,
	PARITYBIT_EVEN	= 0x02
} SCOM_PARITY_DEF;
#endif

#ifndef _SCOM_STOPBIT_DEF_
#define _SCOM_STOPBIT_DEF_
typedef enum _SCOM_STOPBIT_DEF_
{
	STOPBIT_1	= 0x00,
	STOPBIT_2	= 0x01
} SCOM_STOPBIT_DEF;
#endif

#ifndef _SCOM_MONITOR_PARA_
#define _SCOM_MONITOR_PARA_
typedef enum _SCOM_MONITOR_PARA_
{
	Motor_Speed							= 0xE000,
	Speed_Reference						= 0xE001,
	Torque_Reference					= 0xE002,
	Rotate_Angle1						= 0xE003,
	Rotate_Angle2						= 0xE005,
	Input_Signal_Monitor				= 0xE006,
	Output_Signal_Monitor				= 0xE007,
	Input_Reference_Pulse_Speed			= 0xE008,
	Deviation_Counter					= 0xE009,
	Cumulative_Load						= 0xE00B,
	Regenerative_Load					= 0xE00C,
	DB_Resister_Consumption_Power		= 0xE00D,
	Reference_Pulse_Counter				= 0xE00E,
	Feedback_Pulse_Counter				= 0xE010,
	Fully_Closed_Feedback_Pulse_Counter	= 0xE012,
	Current_Alarm_status				= 0xE50A   //현재 경보상태를 알기위한 값 A.C80은 클리어 이상.
} SCOM_MONITOR_PARA;
#endif


#ifndef _SCOM_FUNCTION_PARA_
#define _SCOM_FUNCTION_PARA_
typedef enum _SCOM_FUNCTION_PARA_
{
	Operation_Mode_Settig						   	= 0x2000,
	Operation_Settig						   		= 0x2001,
	Excute											= 0x0001,
	UPKey											= 0x0002,
	DownKey											= 0x0003,
	Protect_Clear									= 0x0002,
	Normal_Mode										= 0x0000,
	Jogging_Mode									= 0x1002,
	Origin_Search									= 0x1003,
	Encoder_Reset_Mode								= 0x1008,
	Analog_Command_Automatic_Null_Adjustment_Mode	= 0x1009,
	Alarm_Reset										= 0x2001,
	SW_Reset										= 0x1030
} SCOM_FUNCTION_PARA;
#endif

#ifndef _SCOM_PARAMETER_PARA_
#define _SCOM_PARAMETER_PARA_
typedef enum _SCOM_PARAMETER_PARA_
{
	Axis_Select										= 0x0010,
	ForwardTorqe									= 0x0402,
	BackwardTorqe								 	= 0x0403
} SCOM_PARAMETER_PARA;
#endif



#ifndef _SCOM_FUNC_RESULT_
#define _SCOM_FUNC_RESULT_
typedef enum _SCOM_FUNC_RESULT_
{
	SCOM_RT_SUCCESS							 = 0,
    SCOM_RT_COM_OPEN_FAIL					 = 1000,
	SCOM_RT_COM_ALREADY_OPENED				 = 1001,
    SCOM_RT_COM_NOT_OPEN			         = 1002,
    SCOM_RT_COM_RETRY_ABOVE_MAX_COUNT		 = 1003,
    SCOM_RT_COM_RECEIVE_FAIL				 = 1004,
	SCOM_RT_1ST_BELOW_MIN_VALUE              = 1005,
	SCOM_RT_RECEIVE_INVALID_DATA			 = 1006,
	SCOM_RT_WRITE_FAIL						 = 1007,
	SCOM_RT_INVALID_FUNCTION_CODE			 = 1008,
	SCOM_RT_INVALID_REGISTER				 = 1009,
	SCOM_RT_INVALID_NUMBER_OF_DATA			 = 1010,
	SCOM_RT_MISS_INPUT_NUMBER_OF_DATA		 = 1011,
	SCOM_RT_RCV_TIMEOUT_FAIL				 = 1012
} SCOM_FUNC_RESULT;
#endif


#ifndef _SCOM_SERVOPACK_STATUS_
#define _SCOM_SERVOPACK_STATUS_
typedef enum _SCOM_SERVOPACK_STATUS_
{
	ERROR_TRACE_DATE0		= 0xE500,
	ERROR_TRACE_DATE1		= 0xE501,
	ERROR_TRACE_DATE2		= 0xE502,
	ERROR_TRACE_DATE3		= 0xE503,
	ERROR_TRACE_DATE4		= 0xE504,
	ERROR_TRACE_DATE5		= 0xE505,
	ERROR_TRACE_DATE6		= 0xE506,
	ERROR_TRACE_DATE7		= 0xE507,
	ERROR_TRACE_DATE8		= 0xE508,
	ERROR_TRACE_DATE9		= 0xE509,
	CURRUNT_ALARM_STATE	= 0xE50A,
	SERVO_STATUS_MONITOR = 0xE51B
} SCOM_SERVOPACK_STATUS;
#endif

#ifndef _SCOM_ALARM_LIST_
#define _SCOM_ALARM_LIST_
typedef struct _SCOM_ALARM_LIST_
{
	int alarm_number;
	AnsiString alarm_name;
} SCOM_ALARM_LIST;

typedef struct _SCOM_SERVO_STATUS_LIST_
{
	int status_number;
	AnsiString status_name;
} SCOM_SERVO_STATUS_LIST;
#endif


#ifndef _GSCOM_ALARM_LIST_
#define _GSCOM_ALARM_LIST_
const SCOM_ALARM_LIST gScomAlarmList[SERVO_ALARM_COUNT] =
{
	{0x0, "No alarm"},
	{0x20, "Parameter Checksum Error"},
	{0x21, "Parameter Format Error"},
	{0x22, "System Checksum Error"},
	{0x30, "Main Circuit Detector Error"},
	{0x40, "Parameter Setting Error"},
	{0x50, "Combination Error"},
	{0x51, "Unsupported Device Alarm"},
	{0x100, "Overcurrent"},
	{0x300, "Regeneration Error"},
	{0x400, "Overvoltage"},
	{0x510, "Overspeed"},
	{0x511, "Overspeed of Encoder"},
	{0x710, "Momentary Overload"},
	{0x730, "Continuously Overload"},
	{0x731, "DB Overload"},
	{0x720, "DB Overload"},
	{0x810, "Encoder Backup Err"},
	{0x820, "Encoder Checksum Error"},
	{0x830, "Encoder Battery Error"},
	{0x840, "Encoder Data Error"},
	{0x850, "Encoder Overspeed"},
	{0x860, "Encoder Overheated"},
	{0xC90, "Encoder Comm Error"},
	{0xC91, "Encoder Comm Position Data Error"},
	{0xC92, "Encoder Comm Timer Error"}
};

const SCOM_SERVO_STATUS_LIST gScomServoStatusList[SERVO_STATUS_COUNT] =
{
	{0x0000,"Initial"},
	{0x0001,"Alarm has occurred"},
	{0x0002,"Waiting for power recovery from the hard wire base block state (HBB)"},
	{0x0003,"Forward or reverse drive prohibited (PTNT)"},
	{0x0004,"Forward drive prohibited (P-OT)"},
	{0x0005,"Reverse drive prohibited (N-OT)"},
	{0x0006,"Power is not supplied to the motor (BB)"},
	{0x0007,"Power is supplied to the motor (RUN)"},
	{0x0008,"Polarity Detection Status (PDET)"},
	{0x0009,"Active Mode operation in progress (ACT)"},
	{0x000A,"Waiting for power recovery from forced stop state (FSTP)"},
	{0x0103,"Safety function active and forward/reverse drive prohibited (PTNT)"},
	{0x0104,"Safety function active and forward drive prohibited (P-OT)"},
	{0x0105,"Safety function active and reverse drive prohibited (N-OT)"},
	{0x0106,"Safety function active and power is not supplied to the motor (BB)"},
	{0x0107,"Safety function active and power is supplied to the motor (RUN)"},
	{0x0108,"Safety function active and polarity is detected (PDET)"},
	{0x0109,"Safety function active and in Active Mode (ACT)"},
	{0x010A,"Safety function active during a forced stop (FSTP)"},
	{0x0203,"Safe state (not HWBB state) and forward/reverse drive prohibited (PTNT)"},
	{0x0204,"Safe state (not HWBB state) and forward drive prohibited (P-OT)"},
	{0x0205,"Safe state (not HWBB state) and reverse drive prohibited (N-OT)"},
	{0x0206,"Safe state (not HWBB state) and power not supplied to motor (BB)"},
	{0x0207,"Safe state (not HWBB state) and power supplied to motor (RUN)"},
	{0x0208,"Safe state (not HWBB state) and polarity is detected (PDET)"},
	{0x0209,"Safe state (not HWBB state) in Active Mode (ACT)"},
	{0x020A,"Safe state (not HWBB state) during a forced stop (FSTP)"}
};
#endif

//-----------------------------------------------------------------------------

typedef enum SERVOPACK_FN009_STEP_
{


// 주행 초기위치, Shift 초기위치, Hoist 초기위치값은 가지고 있을 것.

	SERVOPACK_FN009_STEP_INIT                  = 0,      //CheckAutoTeachningReady() : 현재 Foup 들고있는지 확인, AutoTeaching Jig와 통신유무 확인 등
												   //각종 명령 초기화 단계.
	SERVOPACK_FN009_STEP_1				,
	SERVOPACK_FN009_STEP_2       		,     //시작 위치 이동 Check(기본 Map 파일 참조, 상위에서 명령 주기)
	SERVOPACK_FN009_STEP_3      		,
	SERVOPACK_FN009_STEP_4      	   	,
	SERVOPACK_FN009_STEP_5	   			,        // 티칭위치보다 20mm 이전에 이동.

	SERVOPACK_FN009_STEP_6				,
	SERVOPACK_FN009_STEP_7       		,     //시작 위치 이동 Check(기본 Map 파일 참조, 상위에서 명령 주기)
	SERVOPACK_FN009_STEP_8      		,
	SERVOPACK_FN009_STEP_9      	   	,
	SERVOPACK_FN009_STEP_10	   			,        // 티칭위치보다 20mm 이전에 이동.
	SERVOPACK_FN009_STEP_ERROR	   			,        // 티칭위치보다 20mm 이전에 이동.
	SERVOPACK_FN009_STEP_FINAL	   			,        // 티칭위치보다 20mm 이전에 이동.

} SERVOPACK_FN009_STEP;

/**
@class   ServoMonitoring
@date    2012.10.13
@author  zzang9un
@brief   ServoMonitoring 관련 Class
@note    Serial 통신으로 Servo Motor Data를 읽어들이거나 셋팅한다.
*/
class ServoMonitoring
{
//friend 하드웨어 쓰레드 클래스

private:
    HWCommon *m_pHWCommon;
	Comm_Port *m_cCOMPort; ///< Serial 연결을 위한 Comm Handle

	LogUnit *pLogServoMonitoring;      ///< pLogServoMonitoring 로그 생성을 위한 변수

	WORD	m_wMonitorPara;   ///<읽을 레지스터 값

	int 	m_nStepFn009;
	bool    m_Fn009FrontFlag;
	bool    m_Fn009RearFlag;

	WORD    m_wAddress;       ///<슬레이브 주소
	DWORD	m_dwErrCode;      ///<에러코드
	long	m_lReadData;      ///<읽은 데이터 정보

	BYTE    m_IsSetTorqueFront;   //0 초기화 1: 90, 2: 800
	BYTE    m_IsSetTorqueFront_2;   //0 초기화 1: 90, 2: 800
	BYTE    m_IsSetTorqueRear;   //0 초기화 1: 90, 2: 800
	BYTE    m_IsSetTorqueRear_2;   //0 초기화 1: 90, 2: 800

	BYTE    m_IsSetTorqueHoist;   //0 초기화 1: 90, 2: 800
	BYTE    m_IsSetTorqueShift;   //0 초기화 1: 90, 2: 800

	int Write(char *, int Len);
	int Read(char *, int Len);

	DWORD	SigmaCom(char byAddress, char byFuncCode, WORD wFuncSubCode, WORD wRegister, WORD wRegNum, DWORD* dwpReadData);
	WORD	GetMonitorParaValue(int index);
	WORD	CRCCheck(int nDataNum, char* szCheckBuff);       		//CRC Check


public:
	ServoMonitoring(HWCommon *pHWCommon);
	~ServoMonitoring();

	DWORD	SigmaComWrite(char byAddress, char byFuncCode, WORD wFuncSubCode, WORD wRegister, WORD wWriteData);
	DWORD	SigmaComWrite(char chAddress, char chFuncCode, WORD wFuncSubCode, WORD wRegister, WORD wWriteData, int sleep);
	DWORD   ReadMonitorData(WORD wAddress, WORD wMonitorPara, long* lpReadData);

//	bool	WriteFlag;
	bool 	ReUpdate;

	bool	SetParaValue(WORD wTempAddress, int nTempSelIndex);
	bool	GetRcvdData(DWORD* dwTempErrcode,long* lTempReadData);
	bool	IsCommOpen();

	bool	SetInnerLimitTorque_0();
	bool	SetInnerLimitTorque_90();
	bool	SetInnerLimitTorque_800();
	bool	SetInnerLimitTorque_800_Trans();

	void	SetInnerLimitTorque_Front(); // by jhoun.lim
	void	SetInnerLimitTorque_Rear(); // by jhoun.lim

	bool    IsSetInnerLimitTorque_0();
	bool	IsSetInnerLimitTorque_90();
	bool	IsSetInnerLimitTorque_800();
	void	DisplayServoMonitorResult(WORD RegisterAddress, int readValue);
	bool 	SetFn009(BYTE axis);
	DWORD 	SMCH_Update();
	int 	GetFn009Step();
	void 	SetFn009Step(int nStep);
	bool    IsFn009Flag();

	bool    Fn009ing();
};

#endif  //ServoMonitoringH
