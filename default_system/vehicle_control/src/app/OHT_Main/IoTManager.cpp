//---------------------------------------------------------------------------


#pragma hdrstop

#include <cstdlib>
//#include <mmsystem.h>

#include "OHTMain.h"
#include "IoTManager.h"

#define ADD_LOG_IOTHUB(...)    WRITE_LOG_UNIT(pLogIOT, __VA_ARGS__)


//---------------------------------------------------------------------------

IotManager::IotManager()
{

	IotHeader = new IOT_HEADER;
	LastDefSignalCount = new IOT_DEFECT_SIGNAL_COUNT;
	IotLastDefWarnData = new IOT_DEFECT_SIGNAL;

	// Initialize Defect Data
	IotLastDefWarnData->nSensorType = 0;
	IotLastDefWarnData->nSensingData = 0;
	IotLastDefWarnData->nSensingAxis = 0;

	// Initialize Defect Data Count/time
	InitializeDefectSigCnt();

	dwDetectPeriod = DEFECT_CHECK_PERIOD;
	nDefectShockLevel = DEFECT_SHOCK_LEVEL;
	nDefectParticleLevel = DEFECT_PARTICLE_LEVEL;
    pLogIOT = getLogUnit("D:\\log\\IOTHUB.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	OHTModelType = 0x8101;
}

IotManager::~IotManager()
{
	delete IotHeader;
	delete LastDefSignalCount;
	delete IotLastDefWarnData;
}

void IotManager::InitializeDefectSigCnt(void)
{
	LastDefSignalCount->nCountShockDefect =  0;
	LastDefSignalCount->nCountParticleDefect = 0;

	LastDefSignalCount->dwLastTimeShockDef = timeGetTime();
	LastDefSignalCount->dwLastTimeParticleDef = timeGetTime();
}

/*
int IotManager::IoTCommand(char *cRBuf, char *SendBuf, int *nLen, short *nIoTErr)
{
   int nError = 0;
   char *pTmpBuffer;
   int TotalLen=0, UsedLen=0, SndLen=0;

  	IOT_HEADER IotHeader;
	IOT_DEFECT_SIGNAL_COUNT *DefSignalCount;

	AnsiString strIotLog = "";

   memset(SendBuf, 0x00, LEN_TLV_BUFFER);

   int strTmpType;

   pTmpBuffer = cRBuf;

   //  Read Header
	IotHeader = GetIotHeader(pTmpBuffer, &UsedLen);


		if(IotHeader.IotHeaderUp.nCommandID == COMMAND_REQINFO_R)    //AOHC-203 IoT정보 데이터 취득 및 활용
		{
			SndLen = MakeOHTInformation(SendBuf, IotHeader, COMMAND_REQINFO_S);
		}
		else if(IotHeader.IotHeaderUp.nCommandID == COMMAND_WARNDEFECT_R)    //AOHC-203 IoT정보 데이터 취득 및 활용
		{
			DefSignalCount = GetIotDefectSignal(pTmpBuffer, UsedLen);

			if(DefSignalCount->nCountShockDefect > 3)
			{
				; // action
				InitializeDefectSigCnt();
			}
			else if(DefSignalCount->nCountParticleDefect > 3)
			{
				; // action
				InitializeDefectSigCnt();
			}

			// Make and Send a Message
			SndLen = MakeOHTInformation(SendBuf, IotHeader, COMMAND_WARNDEFECT_S);

		}
	 *nLen = SndLen;

	if(IotHeader.IotHeaderUp.nIotHubState != 0)      // 2: Particle sensor 에러
	{
	  *nIoTErr = IotHeader.IotHeaderUp.nIotHubState;
      return 3;
	}

	return 0;
}
*/
/*
IOT_HEADER IotManager::GetIotHeader(char *pBuffer, int *nBuffOffset)
{
	IOT_HEADER header = new IOT_HEADER;
	int nOffset = 0;



	// IoT ID
//	header.IotHeaderUp.nIotID = 0;
//	memcpy(&header.IotHeaderUp.nIotID, pBuffer, (IOT_H_LEN_IOTID));
//	nOffset += IOT_H_LEN_IOTID;
//
//	//  Machine ID
//	header.IotHeaderUp.nMachineID = 0;
//	memcpy(&header.IotHeaderUp.nMachineID, pBuffer + nOffset, (IOT_H_LEN_MACHINEID));
//	nOffset += IOT_H_LEN_MACHINEID;
//
//	// Machine Type
//	header.IotHeaderUp.cMachineType1=0;
//    header.IotHeaderUp.cMachineType2=0;
//	memcpy(&header.IotHeaderUp.cMachineType1, pBuffer + nOffset, (1));
//	nOffset++;
//	memcpy(&header.IotHeaderUp.cMachineType2, pBuffer + nOffset, (1));
//	nOffset++;
//
//	// Command ID
//	header.IotHeaderUp.nCommandID=0;
//	memcpy(&header.IotHeaderUp.nCommandID, pBuffer + nOffset, (IOT_H_LEN_COMMAND));
//	nOffset += IOT_H_LEN_COMMAND;
//
//	// IoT Hub State
//	header.IotHeaderUp.nIotHubState=0;
//	memcpy(&header.IotHeaderUp.nIotHubState, pBuffer + nOffset, (IOT_H_LEN_HUBSTATE));
//	nOffset += IOT_H_LEN_HUBSTATE;
//
//    // IOT X axis ACC
//    header.fXAxisACC=0;
//    memcpy(&header.fXAxisACC, pBuffer + nOffset, (IOT_H_LEN_X_AXIS_ACC));
//	nOffset += IOT_H_LEN_X_AXIS_ACC ;
//
//    // IOT Y axis ACC
//    header.fYAxisACC=0;
//    memcpy(&header.fYAxisACC, pBuffer + nOffset, (IOT_H_LEN_Y_AXIS_ACC));
//	nOffset += IOT_H_LEN_Y_AXIS_ACC ;
//
//    // IOT Z axis ACC
//    header.fZAxisACC=0;
//    memcpy(&header.fZAxisACC, pBuffer + nOffset, (IOT_H_LEN_Z_AXIS_ACC));
//	nOffset += IOT_H_LEN_Z_AXIS_ACC ;
//
//    // IOT Gyro Roll
//    header.fGyroRoll=0;
//    memcpy(&header.fGyroRoll, pBuffer + nOffset, (IOT_H_LEN_GYRO_ROLL));
//	nOffset += IOT_H_LEN_GYRO_ROLL ;
//
//    // IOT Gyro Pitch
//    header.fGyroPitch=0;
//    memcpy(&header.fGyroPitch, pBuffer + nOffset, (IOT_H_LEN_GYRO_PITCH));
//	nOffset += IOT_H_LEN_GYRO_PITCH ;
//
//    // IOT Gyro Yaw
//    header.fGyroYaw=0;
//    memcpy(&header.fGyroYaw, pBuffer + nOffset, (IOT_H_LEN_GYRO_YAW));
//	nOffset += IOT_H_LEN_GYRO_YAW ;
//
//    // IOT X axis Degree
//    header.fXAxisDegree=0;
//    memcpy(&header.fXAxisDegree, pBuffer + nOffset, (IOT_H_LEN_X_AXIS_DEGREE));
//	nOffset += IOT_H_LEN_X_AXIS_DEGREE ;
//
//    // IOT Y axis Degree
//    header.fYAxisDegree=0;
//    memcpy(&header.fYAxisDegree, pBuffer + nOffset, (IOT_H_LEN_Y_AXIS_DEGREE));
//	nOffset += IOT_H_LEN_Y_AXIS_DEGREE ;
//
//    // IOT 1 Axis Torque
//    header.f1AxisTorque=0;
//    memcpy(&header.f1AxisTorque, pBuffer + nOffset, (IOT_H_LEN_1_AXIS_TORQUE));
//	nOffset += IOT_H_LEN_1_AXIS_TORQUE ;
//
//    // IOT 2 Axis Torque
//    header.f2AxisTorque=0;
//    memcpy(&header.f2AxisTorque, pBuffer + nOffset, (IOT_H_LEN_2_AXIS_TORQUE));
//	nOffset += IOT_H_LEN_2_AXIS_TORQUE ;
//
//    // IOT 3 Axis Torque
//    header.f3AxisTorque=0;
//    memcpy(&header.f3AxisTorque, pBuffer + nOffset, (IOT_H_LEN_3_AXIS_TORQUE));
//	nOffset += IOT_H_LEN_3_AXIS_TORQUE ;
//
//    // IOT 4 Axis Torque
//	header.f4AxisTorque=0;
//	memcpy(&header.f4AxisTorque, pBuffer + nOffset, (IOT_H_LEN_4_AXIS_TORQUE));
//	nOffset += IOT_H_LEN_4_AXIS_TORQUE ;

	*nBuffOffset = nOffset;         // Header 끝 부분 위치 저장, 정보 시작 위치

	return header;
}
*/

IOT_DEFECT_SIGNAL_COUNT *IotManager::GetIotDefectSignal(char *pBuffer, int nStart)
{
	IOT_DEFECT_SIGNAL defect_sig;

	int nOffset = nStart;
	DWORD dwStartTime;

	// Sensor Type, 1B
	defect_sig.nSensorType=0;
	memcpy(&defect_sig.nSensorType, pBuffer + nOffset, sizeof(short));
	nOffset += sizeof(short);

	// axis, 1B
	defect_sig.nSensingAxis=0;
	memcpy(&defect_sig.nSensingAxis, pBuffer + nOffset, sizeof(short));
	nOffset += sizeof(short);

	// Sensor Data, 4Byte
	defect_sig.nSensingData=0;
	memcpy(&defect_sig.nSensingData, pBuffer + nOffset, sizeof(int));
	nOffset += sizeof(int);


	// axis, 1B
	if((defect_sig.nSensorType == ACCELERATION_SENSOR) && (defect_sig.nSensingData > nDefectShockLevel))      // shock > 1.8g
	{
		dwStartTime = LastDefSignalCount->dwLastTimeShockDef;
		if(PreventOverflowGetTimeMs(dwStartTime) > dwDetectPeriod)  //10min
		{
			LastDefSignalCount->nCountShockDefect =  1;
		}
		else
		{
			LastDefSignalCount->nCountShockDefect++;
		}
		LastDefSignalCount->dwLastTimeShockDef = timeGetTime();

	}
	else if((defect_sig.nSensorType == PARTICLE_SENSOR) && (defect_sig.nSensingData > nDefectParticleLevel))      // shock > 1.8g
	{
		dwStartTime = LastDefSignalCount->dwLastTimeParticleDef;
		if(PreventOverflowGetTimeMs(dwStartTime) > dwDetectPeriod)  //10min
		{
			LastDefSignalCount->nCountParticleDefect =  1;
		}
		else
		{
			LastDefSignalCount->nCountParticleDefect++;
		}
		LastDefSignalCount->dwLastTimeParticleDef = timeGetTime();
	}
	else if(defect_sig.nSensorType == GYRO_SENSOR)// && (defect_sig.nSensingData > nDefectSlopeLevel))              // Slope
	{
			LastDefSignalCount->nDegreeOfSlope = defect_sig.nSensingData;

			if(defect_sig.nSensingAxis == 11 && LastDefSignalCount->bDetectSlope == false)                // Slope
				 LastDefSignalCount->bDetectSlope = true;
			else if(defect_sig.nSensingAxis == 10 && LastDefSignalCount->bDetectSlope == true)            // Non slope
				 LastDefSignalCount->bDetectSlope = false;

		dwStartTime = LastDefSignalCount->dwLastTimeSlopeDef;
	}
	else
	{
		;
	}

	// save defect data
	memcpy(IotLastDefWarnData, &defect_sig, sizeof(IOT_DEFECT_SIGNAL));

	AnsiString strIotLog = "";
	strIotLog.sprintf("Defect Inf -> Sensor: %d, axis: %d, data: %d, cnt: P %d, V %d, slope : %d",
				defect_sig.nSensorType, defect_sig.nSensingAxis, defect_sig.nSensingData,
				LastDefSignalCount->nCountShockDefect, LastDefSignalCount->nCountParticleDefect,
                LastDefSignalCount->bDetectSlope);
	ADD_LOG_IOTHUB(strIotLog.c_str());

	return LastDefSignalCount;
}
   /*
int IotManager::MakeCommStateCheck(char *SndBuf, IOT_HEADER header, short cid)
{
//	char cTempBuffer[LEN_TLV_BUFFER];
	char cTemp[IOT_LEN_COMMSTATE];
	int nOffset = 0;

	memset(SndBuf, 0x00, sizeof(LEN_TLV_BUFFER));

	// Header, IOT ID, Machine Type 10byte
	memcpy(SndBuf, &header.IotHeaderUp, IOT_SH_LEN_HEADER);
	nOffset = IOT_SH_LEN_HEADER;

	// Command ID, 2byte
	memcpy(SndBuf + nOffset, &cid, sizeof(cid));
	nOffset += IOT_H_LEN_COMMAND;

	return nOffset;
}
*/


int IotManager::MakeOHTInformation(char *SndBuf, IOT_HEADER header, short cid)
{
//	char cTempBuffer[LEN_TLV_BUFFER];
	int nOffset = 0;
	unsigned char tempC;

	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// IoT ID
	memcpy(SndBuf, &header.IotHeaderUp.nIotID, IOT_H_LEN_IOTID);
	nOffset += IOT_H_LEN_IOTID;

	//  Machine ID
	memcpy(SndBuf + nOffset, &header.IotHeaderUp.nMachineID, IOT_H_LEN_MACHINEID);
	nOffset += IOT_H_LEN_MACHINEID;

	// Machine Type
	memcpy(SndBuf + nOffset, &header.IotHeaderUp.cMachineType1, 1);
	nOffset++;
	memcpy(SndBuf + nOffset, &header.IotHeaderUp.cMachineType2, 1);
	nOffset++;

	// Command ID, 2byte
	memcpy(SndBuf + nOffset, &cid, IOT_H_LEN_COMMAND);
	nOffset += IOT_H_LEN_COMMAND;

	// OHT Mode, 1B
	tempC = OHTMainForm->GetOHTStatus()->StatusCommon.MSCmdID.Info.Mode;
	memcpy(SndBuf + nOffset, &tempC, 1);
	nOffset++;

	// OHT Status, 1B
	//tempC = GetOhtState();
	//memmove(SndBuf + nOffset, &tempC, 1);
	tempC = OHTMainForm->GetOHTStatus()->StatusCommon.MSCmdID.Info.Status;
	memcpy(SndBuf + nOffset, &tempC, 1);
	nOffset++;

	// Carrier State, 1B
	memcpy(SndBuf + nOffset, &OHTMainForm->GetOHTStatus()->StatusCommon.ExistFoup, 1);
	nOffset++;

	//  Node, 4B
    int nNdT = OHTMainForm->GetOHTStatus()->StatusCommon.CurNodeID;
    int nNode = ((nNdT/100000)%10)*(1<<20) + ((nNdT/10000)%10)*(1<<16) + ((nNdT/1000)%10)*(1<<12) + ((nNdT/100)%10)*(1<<8) + ((nNdT/10)%10)*16 + nNdT%10;
    memcpy(SndBuf + nOffset, &nNode, IOT_LEN_NODE);
    nOffset += IOT_LEN_NODE;

	memcpy(SndBuf + nOffset, &OHTModelType, IOT_H_LEN_MODEL_TYPE);
	nOffset += IOT_H_LEN_MODEL_TYPE;

	memcpy(SndBuf + nOffset, &OHTMainForm->GetDefaultParam()->VHLName, IOT_H_LEN_OHT_NO);
	nOffset += IOT_H_LEN_OHT_NO;

	memcpy(SndBuf + nOffset, &OHTMainForm->GetOHTStatus()->m_cpMonitorData.APSignalData.szSSID, IOT_H_LEN_LINE_INFO);
	nOffset += IOT_H_LEN_LINE_INFO;

	memcpy(SndBuf + nOffset, &OHTMainForm->GetOHTStatus()->ErrCode_StatusChk, IOT_H_LEN_OHT_ERRORCODE);
	nOffset += IOT_H_LEN_OHT_ERRORCODE;

	int nReserved = 0;
	memcpy(SndBuf + nOffset, &nReserved, IOT_H_LEN_OHT_RESERVED);
	nOffset += IOT_H_LEN_OHT_RESERVED;

	// send packet
//	memmove(SndBuf, cTempBuffer, sizeof(nOffset));

	return nOffset;
}

/*
char IotManager::GetOhtCarrierState(void)
{
	BYTE CarrierState;
	char rtn;

	CarrierState = OHTMainForm->GetOHTStatus()->StatusCommon.ExistFoup;

	rtn = (char)CarrierState + '0';

	return rtn;
}

void IotManager::GetOhtCurNode(char cNode[6])
{
	UINT nCNode = 0;

	nCNode = OHTMainForm->GetOHTStatus()->StatusCommon.CurNodeID;

	memset(cNode, '0', sizeof(cNode));

	if(nCNode < 0x000010)		itoa(nCNode, cNode+5, 16);
	else if(nCNode < 0x000100) 	itoa(nCNode, cNode+4, 16);
	else if(nCNode < 0x001000) 	itoa(nCNode, cNode+3, 16);
	else if(nCNode < 0x010000) 	itoa(nCNode, cNode+2, 16);
	else if(nCNode < 0x100000) 	itoa(nCNode, cNode+1, 16);
	else 						itoa(nCNode, cNode,   16);

}

char IotManager::GetOhtState(void)
{
	char state;

	if(OHTMainForm->GetOHTStatus()->StatusCommon.MSCmdID.Info.Mode == 0x01)     // manual mode
	{
		state = 'M';
	}
	else	// auto mode   0x02
	{
		if(OHTMainForm->GetOHTStatus()->StatusCommon.MSCmdID.Info.Status == 0x01)    // Going
		{
			state = 'G';
		}
		else if(OHTMainForm->GetOHTStatus()->StatusCommon.MSCmdID.Info.Status == 0x03)      // Unloading
		{
			state = 'U';
		}
		else if(OHTMainForm->GetOHTStatus()->StatusCommon.MSCmdID.Info.Status == 0x05)    // Loading
		{
			state = 'L';
		}
		else if(OHTMainForm->GetOHTStatus()->StatusCommon.MSCmdID.Info.Status == 0x07)    // Auto Position
		{
			state = 'P';
		}
		else if(OHTMainForm->GetOHTStatus()->StatusCommon.MSCmdID.Info.Status == 0x0E)    // stop
		{
			state = 'E';
		}
		else
		{
			state = 'A';
		}
	}

	return state;
}
*/
unsigned long IotManager::PreventOverflowGetTimeMs(unsigned long precalc)
{
	double tmpPrecalc = (double)precalc;
	double postcalc = (double)timeGetTime();

	if(tmpPrecalc > postcalc) // 증가시간이므로, 이전에 얻은 시간값보다 향후 얻은 시간값이 작으면 오버플로우된 상태다.
	{
		tmpPrecalc = tmpPrecalc - (double)0x100000000;
	}

	return (unsigned long)(postcalc - tmpPrecalc);
}

void IotManager::SetOHTModelTypeInfo(BYTE cModel, BYTE cType)
{
	OHTModelType = cModel*(1<<8) + cType;
	ConvertEndian(&OHTModelType);
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
