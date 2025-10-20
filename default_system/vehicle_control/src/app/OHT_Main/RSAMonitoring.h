//---------------------------------------------------------------------------

#ifndef RSAMonitoringH
#define RSAMonitoringH


#include "HWCommon.h"
#include "CommHandler.h"
#include "ErrorCode.h"


//parity
#define NOPARITY            0
#define ODDPARITY           1
#define EVENPARITY          2
#define MARKPARITY          3
#define SPACEPARITY         4


////stop bits
//#define ONESTOPBIT          0x00
//#define ONE5STOPBITS        0x01
//#define TWOSTOPBITS         0x02

#define DATASIZE 2

#define RSA_ALARM_COUNT 42

//FUNCTIONCODE
typedef enum _RCOM_FUNC_CODE_
{

	KEEP_REG_READ    = 0x03,
	INPUT_REG_READ   = 0x04,       //모니터링 파라미터
	SINGLE_REG_WRITE = 0x06,     //운전 파라미터
	MANY_REG_WRITE   = 0x10       //운전 파라미터

} RCOM_FUNC_CODE;

typedef enum _RCOM_DATA_LENGTH_DEF_
{
	RDATABIT_8	= 0x08,
	RDATABIT_7	= 0x07,
	RDATABIT_6	= 0x06,
	RDATABIT_5	= 0X05

} RCOM_DATA_LENGTH_DEF;

typedef enum _RCOM_PARITY_DEF_
{
	R_PARITYBIT_NONE	= 0x00,
	R_PARITYBIT_ODD	    = 0x01,
	R_PARITYBIT_EVEN	= 0x02
} RCOM_PARITY_DEF;

typedef enum _RCOM_STOPBIT_DEF_
{
	R_STOPBIT_1	= 0x00,
	R_STOPBIT_2	= 0x01
} RCOM_STOPBIT_DEF;

#ifndef _RCOM_FUNCIOTN_PARA_
#define _RCOM_FUNCIOTN_PARA_
typedef enum _RCOM_FUNCIOTN_PARA_
{
   PositiveTorque = 407,
   NegetiveTorque = 408


}RCOM_FUNCIOTN_PARA;
#endif

typedef enum _RCOM_OPERAION_PARA_
{
   Analog_Velocity_Command_Offset = 2003,
   Analog_Torque_Command_Offset   = 2004

} RCOM_OPERAION_PARA;

typedef enum _RCOM_FUNC_RESULT_
{
	RCOM_RT_SUCCESS							 = 0,
	RCOM_RT_COM_OPEN_FAIL					 = 1000,
	RCOM_RT_COM_ALREADY_OPENED				 = 1001,
	RCOM_RT_COM_NOT_OPEN			         = 1002,
	RCOM_RT_COM_RETRY_ABOVE_MAX_COUNT		 = 1003,
	RCOM_RT_COM_RECEIVE_FAIL				 = 1004,
	RCOM_RT_1ST_BELOW_MIN_VALUE              = 1005,
	RCOM_RT_RECEIVE_INVALID_DATA			 = 1006,
	RCOM_RT_WRITE_FAIL						 = 1007,
	RCOM_RT_INVALID_FUNCTION_CODE			 = 1008,
	RCOM_RT_INVALID_REGISTER				 = 1009,
	RCOM_RT_INVALID_NUMBER_OF_DATA			 = 1010,
	RCOM_RT_MISS_INPUT_NUMBER_OF_DATA		 = 1011,
	RCOM_RT_RCV_TIMEOUT_FAIL				 = 1012
} RCOM_FUNC_RESULT;

typedef enum _RCOM_MONITOR_PARA_
{
   velocity_feedback					 = 0,
   velocity_order						 = 1,
   velocity_error 						 = 2,
   torque_order							 = 3,
   position_feedback					 = 4, //4~5?
   position_order						 = 6, //6~7?
   position_error						 = 8, //8~9?
   pulse_order_frequency				 = 10,
   electrical_degree					 = 11,
   machine_degree						 = 12,
   regeneration_accumulation_load_ration = 13,
   DC_Link_voltage						 = 14,
   servo_alarm_code                      = 100,
   servo_warning_code                    = 101

} RCOM_MONITOR_PARA;

#ifndef _RCOM_ALARM_LIST_
#define _RCOM_ALARM_LIST_
typedef struct _RCOM_ALARM_LIST_
{
	int alarm_number;
	AnsiString alarm_name;
} RCOM_ALARM_LIST;

#endif

#ifndef _GRCOM_ALARM_LIST_
#define _GRCOM_ALARM_LIST_
const RCOM_ALARM_LIST gRSAErrorList[RSA_ALARM_COUNT] =
{
   {0x0,"No_alarm"},
   {0x10, "HWARE"},
   {0x11,"IPMFT"},
   {0x12,"SHTOC"},
   {0x13,"OFSET"},
   {0x14,"OVCUR"},
   {0x20,"CABLE"},
   {0x21,"INSOL"},
   {0x23,"SHTOL"},
   {0x24,"DRVOT"},
   {0x25,"CONOL"},
   {0x27,"DRVOL"},
   {0x28,"MATCH"},
   {0x31,"ENCOP"},
   {0x32,"ENCCE"},
   {0x33,"ABSBE"},
   {0x34,"ABSOS"},
   {0x35,"MTCER"},
   {0x36,"MTCOF"},
   {0x38,"ENCPE"},
   {0x39,"SCNER"},
   {0x40,"OVVTG"},
   {0x41,"UDVTG"},
   {0x42,"ACOFF"},
   {0x43,"FANOFF"},
   {0x50,"OVSPD"},
   {0x51,"POSER"},
   {0x52,"CDFRE"},
   {0x53,"CPUFT"},
   {0x54,"ESTOP"},
   {0x55,"ABSTO"},
   {0x56,"STOIN"},
   {0x60,"SERCE"},
   {0x61,"CHSUM"},
   {0x62,"RANGE"},
   {0x63,"PINIT"},
   {0x64,"IRANG"},
   {0x65,"FLCOV"},
   {0x70,"DINIT"},
   {0x71,"SETUP"},
   {0x73,"HFAIL"},
   {0x74,"NOTHM"}

};
#endif

typedef enum _RCOM_ERROR_CODE_
{
	PWM_HW_ERROR						= 0xE057,
	IPM_ERROR  							= 0xE005,
	RE_OVERCURRENT_PROTECTION			= 0xE079,
	CURRENT_FEEDBACK_OFFSET				= 0xE054,
	MOTOR_OVERCURRENT					= 0xE114,
	MOTOR_POWER_CABLE_OPEN				= 0xE101,
	MOTOR_REOVERCURRENT					= 0xE102,
	REGENERATIVE_OVERCURRENT_PROTECTION = 0XE075,
	DRIVIE_OVERHEAT						= 0XE036,
	MOTOR_REOVERCURRENT_2				= 0XE022,
	DRIVE_OVERLOAD						= 0XE023,
	MOTOR_MISMATCH						= 0XE103,
	ENCODER_CABLE_OPEN 					= 0XE030,
	ENCODER_COM_ERROR					= 0XE106,
	ABS_ENCODER_BATTERY_ERROR			= 0XE083,
	ABS_ENCORDER_OVERVELOCITY			= 0XE084


} RCOM_ERROR_CODE;

typedef enum _RSA_AVCO_STEP_
{
	RSA_AVCO_STEP_INIT          = 0,
	RSA_AVCO_STEP_1				,
	RSA_AVCO_STEP_2       		,
	RSA_AVCO_STEP_3      		,
	RSA_AVCO_STEP_4      	   	,
	RSA_AVCO_STEP_5	   			,
	RSA_AVCO_STEP_6				,
	RSA_AVCO_STEP_7       		,
	RSA_AVCO_STEP_8      		,
	RSA_AVCO_STEP_9      	   	,
	RSA_AVCO_STEP_10	   		,
	RSA_AVCO_STEP_11            ,
	RSA_AVCO_STEP_ERROR	   		,
	RSA_AVCO_STEP_FINAL

} RSA_AVCO_STEP;



class RSAMonitoring
{

private:
	HWCommon *m_pHWCommon;
	Comm_Port *m_cCOMPort; ///< Serial 연결을 위한 Comm Handle

	LogUnit *pLogRSAMonitoring;      ///< pLogServoMonitoring 로그 생성을 위한 변수


	DWORD	m_dwErrCode;      ///<에러코드
	long	m_lReadData;      ///<읽은 데이터 정보



	BYTE m_RSAIsSetTorqueFront;  //0,1,2
	BYTE m_RSAIsSetTorqueFront_2; //0,1,2
	BYTE m_RSAIsSetTorqueRear;   //0,1,2
	BYTE m_RSAIsSetTorqueRear_2; //0,1,2
	BYTE m_RSAIsSetTorqueHoist; //0,1,2
	BYTE m_RSAIsSetTorqueHoist_2; //0,1,2
	BYTE m_RSAIsSetTorqueShift; //0,1,2
	BYTE m_RSAIsSetTorqueShift_2; //0,1,2

	int m_nStepAVCO;
	bool m_AVCOFrontFlag;
	bool m_AVCORearFlag;

public:


	RSAMonitoring(HWCommon *pHWCommon);
	~RSAMonitoring();

	bool IsRSACommOpen();
	//DWORD ReadErrorCode(WORD wAddress);
	//DWORD RSACom(WORD wAddress, WORD wFuncCode, WORD wDataAddress, int DataSize, WORD wData);
	DWORD ReadErrorCode(WORD wAddress, DWORD* wMonitorData);
	DWORD RSACom(WORD wAddress, WORD wFuncCode, WORD wDataAddress, int DataSize, WORD wData, DWORD* wReadData);
	DWORD DisplayRSAErrorCode(DWORD readAlarm);

	DWORD RSAComWrite(WORD wAddress, WORD wFuncCode, WORD wDataAddress, int DataSize, WORD wData, int sleep_ms = 200);
	DWORD RSAComReadTest(WORD wAddress, WORD wFuncCode, WORD wDataAddress, int DataSize, WORD wData);
	int RSAWrite(char* chSndData, int len);
	int RSARead(char* chReadData, int len);
	WORD CRC16(const byte *nData, WORD wLength);

	bool SetInnerLimitTorque_0();
	bool SetInnerLimitTorque_90();
	bool SetInnerLimitTorque_300();
	bool SetInnerLimitTorque_300_Trans();
	bool IsSetInnerLimitTorque_90();
	bool IsSetInnerLimitTorque_300();
	bool IsSetInnerLimitTorque_0();
	void SetInnerLimitTorque_Front();
	void SetInnerLimitTorque_Rear();

	bool SetVelocityCommandOffset(Byte axis);
	bool AVCOing();
	int GetAVCOStep();
	void SetAVCOStep(int nStep);
	bool IsAVCOFlag();
	DWORD AutoVelocityCommandOffset();

};
#endif
