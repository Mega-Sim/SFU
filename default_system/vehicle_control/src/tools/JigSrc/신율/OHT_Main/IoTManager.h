//---------------------------------------------------------------------------

#ifndef IoTManagerH
#define IoTManagerH
//---------------------------------------------------------------------------
#include "Def_OHT.h"
//#include "OHT_TLV.h"
#include "TaskControl.h"

#define IOT_PARAM_FILE_PATH		"\\Datafiles\\IotParam.ini"

#define IOT_LEN_HEADER 	12    // 4: IOT ID, 4: Machine ID, 2: 장비 분류, 2: Command ID
#define IOT_H_LEN_IOTID 	4
#define IOT_H_LEN_MACHINEID	4
#define IOT_H_LEN_MACHINET 	2
#define IOT_H_LEN_COMMAND	2
#define IOT_H_LEN_HUBSTATE	2
//AOHC-203 IoT정보 데이터 취득 및 활용위해 추
#define IOT_H_LEN_X_AXIS_ACC 4
#define IOT_H_LEN_Y_AXIS_ACC 4
#define IOT_H_LEN_Z_AXIS_ACC 4
#define IOT_H_LEN_GYRO_ROLL  4
#define IOT_H_LEN_GYRO_PITCH 4
#define IOT_H_LEN_GYRO_YAW   4
#define IOT_H_LEN_X_AXIS_DEGREE 4
#define IOT_H_LEN_Y_AXIS_DEGREE 4
#define IOT_H_LEN_1_AXIS_TORQUE 4
#define IOT_H_LEN_2_AXIS_TORQUE 4
#define IOT_H_LEN_3_AXIS_TORQUE 4
#define IOT_H_LEN_4_AXIS_TORQUE 4
#define IOT_SH_LEN_HEADER 	58    // Command ID, Hub State 제외한 길이

#define IOT_LEN_NODE		4
#define IOT_LEN_COMMSTATE	2

#define IOT_LEN_DEFECT_INFO  6

#define IOT_MAX_PACKET_SIZE		1024

// Command ID

#define COMMAND_COMMCHECK_S   	1
#define COMMAND_COMMCHECK_R   	2
#define COMMAND_REQINFO_R		3
#define COMMAND_REQINFO_S   	4
#define COMMAND_WARNDEFECT_R 	5
#define COMMAND_WARNDEFECT_S   	6

// Sensor Type
#define ACCELERATION_SENSOR		1
#define PARTICLE_SENSOR			2
#define GYRO_SENSOR  			3

//  Defect Level
#define DEFECT_SHOCK_LEVEL      1800  // 1.8g
#define DEFECT_PARTICLE_LEVEL	3      //0.3um이상
#define DEFECT_SLOPE_LEVEL	   6      //0.3um이상

// Detect Time Period
#define DEFECT_CHECK_PERIOD   	600000



#pragma pack(push, 1) // 1byte로 Pack
//---------------------------------------------------------------------------
/*
@brief  IoT 보드와 통신할 때의 헤더
@data   2016.08.09
@author
*/
typedef struct IOT_HEADER_UP_
{
	int nIotID;        // IOT Hub Number
	int nMachineID;    // Machine Number
	char cMachineType1;  // Machine Type, <First> 1: OHT      <Second> 1: Semes Mask OHT
	char cMachineType2;
	short nCommandID;       //AOHC-203 IoT정보 데이터 취득 및 활용
	short nIotHubState;    //AOHC-203 IoT정보 데이터 취득 및 활용
}IOT_HEADER_UP;
/**
@brief  IoT 보드와 통신할 때의 헤더
@data   2016.08.09
@author
*/
typedef struct IOT_HEADER_
{
	IOT_HEADER_UP IotHeaderUp;

//	char Header10B[10];   // Command id를 제외한 모든 것 저장
//AOHC-203 IoT정보 데이터 취득 및 활용위해 추
	float fXAxisACC;
	float fYAxisACC;
	float fZAxisACC;
	float fGyroRoll;
	float fGyroPitch;
	float fGyroYaw;
	float fXAxisDegree;
	float fYAxisDegree;
	float f1AxisTorque;
	float f2AxisTorque;
	float f3AxisTorque;
	float f4AxisTorque;


}IOT_HEADER;

/**
@brief	IoT 보드에서 Defect Warning 발생 시 데이터
@data   2016.08.09
@author
*/
typedef struct IOT_DEFECT_SIGNAL_COUNT_
{
	int nCountShockDefect;
	int nCountParticleDefect;
	int nDegreeOfSlope; // 각도
	bool bDetectSlope;

	DWORD dwLastTimeShockDef;
	DWORD dwLastTimeParticleDef;
	DWORD dwLastTimeSlopeDef;
}IOT_DEFECT_SIGNAL_COUNT;

/**
@brief	IoT 보드에서 Defect Warning 발생 시 데이터
@data   2016.08.09
@author
*/
typedef struct IOT_DEFECT_SIGNAL_
{
	int nSensorType;         // Sensor Type, 0: vib, 1: particle
	int nSensingData;   	 // vib: mG, particle: ea
	int nSensingAxis;  		 // only vib,
}IOT_DEFECT_SIGNAL;

#pragma pack(pop) // Pack 해제

/**
@brief	IoT 보드와의 통신 응답과 데이터 송부
@data   2016.08.09
@author
*/
class IotManager
{
private:
	IOT_HEADER *IotHeader;
	IOT_DEFECT_SIGNAL *IotLastDefWarnData;
	IOT_DEFECT_SIGNAL_COUNT *LastDefSignalCount;

	DWORD dwDetectPeriod;
	int nDefectShockLevel;
	int nDefectParticleLevel;

    LogUnit *pLogIOT;      ///< pLogServoMonitoring 로그 생성을 위한 변수

public:
	IotManager();
	~IotManager();

	void InitializeDefectSigCnt(void);

//	int IoTCommand(char *cRBuf, char *SendBuf, int *nLen, short *nIoTErr);

 	IOT_HEADER GetIotHeader(char *pBuffer, int *nBuffOffset);
/* int GetIotHeader(char *pBuffer,  //데이터
				int BuffLen,    //총길이
				char *pTLV,    // 1개의 TLV 데이터
				int *Bytes,    //1개의 TLV 총 길이
				bool IsConvertEndian );   */
	IOT_DEFECT_SIGNAL_COUNT *GetIotDefectSignal(char *pBuffer, int nStart);

//	int MakeCommStateCheck(char *SndBuf, IOT_HEADER header, short cid);
	int MakeOHTInformation(char *SndBuf, IOT_HEADER header, short cid);
/*
	void GetOhtCurNode(char cNode[6]);
	char GetOhtState(void);
	char GetOhtCarrierState(void);
*/

	unsigned long PreventOverflowGetTimeMs(unsigned long precalc);
};

//---------------------------------------------------------------------------
#endif
