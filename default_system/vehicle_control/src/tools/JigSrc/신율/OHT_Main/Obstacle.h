//---------------------------------------------------------------------------
/**
@file    Obstacle.h
@version OHT 7.0
@brief   Obstacle Class Header File
*/

#ifndef ObstacleH
#define ObstacleH

#include "HWCommon.h"
#include "VirtualDetect.h"
#include "CommHandler.h"

#define OBS_ComPort     9 ///< 대차감지센서
#define OBS_Baudrate    38400
#define OBS_Command_Packet_Size 4
#define	OBS_Final_Flag (0x0A)
#define	OBS_Target      4
#define OBS_ReceiveDataSize 1547
#define OBS_Start_Flag_1 (0x24)
#define OBS_Start_Flag_2 (0x47)
#define OBS_Sensor_Data_Num 513
#define OBS_Sensor_Data_StartPacketSize 6
#define OBS_Sensor_Data 3
#define OBS_Sensor_Request_Data_Interval 800
#define ASCII_ZERO 0x30

#define OBSLOGTIME 1000 //OBS LOG 기록하는 초 지정

/**
@class   Obstacle
@date    2012.10.22
@author  LSJ
@brief   Obstacle Sensor 관련 Class
@note    Obstacle Sensor의 Area를 선택하고 감지하기 위한 함수 구현
*/
class Obstacle:VirtualDetect
{
private:
	HWCommon *m_pHWCommon;

	//신규진단모니터링 항목(센서 누적 가동시간)
	long m_nStartEnableTime;
    Comm_Port *m_pCommPort;
    bool bConnect;
    int m_nComPortNum;
	int m_nBaudRate;
    LogUnit *m_pLogObstacle;

public:

	Obstacle(HWCommon *pHWCommon);
	~Obstacle();

	int				Enable();						// 활성화(전원 ON)
	int				Disable();						// 비활성화(전원 OFF)

	DETECT_STATUS	GetStatus();					// 상태를 받아오는 함수
	void			SetArea(DETECT_AREA_TYPE Type);	// 영역을 설정하는 함수
	double			GetMovableDist();			// 이동 가능한 거리를 받아오는 함수
	int				GetObsAreaStatus();				// 상태를 받아오는 함수	

	int	PowerControl(int nMode);
    int ErrorReset();
	int AreaSelect(int nArea);
    bool IsDetect(int nLevel);
	bool CheckError();
    int OpenCom(void);
    int Decode(int start, char *code, int byte);
    int CheckObsSensorData(void);
    int CheckObsSensorData(int *Buff);
    int ReceiveData(char *ReadBuff);
    void RequestData(void);
    int CloseCom(void);
    int UpdataData( int *Buff, bool bIsStart = false);
    void DataReadStart(int *Buff);

	//신규 진단모니터링 함수(센서 누적 가동시간 전송)
	int GetEnableTime(bool bInit);
	int GetSensorLevel(bool bLong, bool bMiddle, bool bShort, bool bFail); //soming OBS 패턴 USER_LOG 추가
   //	int GetOutputSensorLevel(bool bSel1, bool bSel2, bool bSel3, bool bSel4, bool bPower);
	AnsiString GetOutputSensorLevel(bool bSel1, bool bSel2, bool bSel3, bool bSel4, bool bPower);

};

#endif  //ObstacleH
