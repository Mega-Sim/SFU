/**
@file    OHTDetect.h
@version OHT 7.0
@brief   OHTDetect Class Header File
*/

#ifndef OHTDetectH
#define OHTDetectH

#include "VirtualDetect.h"
#include "HWCommon.h"
#include "CommHandler.h"

#define UBG_ComPort     8 ///< 대차감지센서
#define UBG_Baudrate    38400
#define	UBG_Final_Flag (0x0A)
#define	UBG_Target      3

#define UBG_Command_Packet_Size 4
#define	UBG_Area_Target      4
#define UBG_ReceiveDataSize 1547
#define UBG_Start_Flag_1 (0x24)
#define UBG_Start_Flag_2 (0x47)
#define UBG_Sensor_Data_Num 513
#define UBG_Sensor_Data_StartPacketSize 6
#define UBG_Sensor_Data 3
#define UBG_Sensor_Request_Data_Interval 500
#define UBGLOGTIME 800			//msec
#define UBGCHECKTIME 3000 //msec


/**
@class   OHTDetect
@date    
@author  
@brief   OHTDetect 관련 Class
*/
class OHTDetect:VirtualDetect
{
private:
	HWCommon *m_pHWCommon;
    Comm_Port *m_pCommPort;
	bool bConnect;
	int m_nComPortNum;
	int m_nBaudRate;
	long m_nStartEnableTime;
    LogUnit *pLogDetect;

	int             OpenCom();                 //UBZ PORT를 opne하는 함수
	int             CloseCom();                 //UBZ PORT를 close하는 함수
	bool IsConnect(); //UBG ComOpen 상태에서  오토 체인지 필요한 경우에 UBG Comclose 위해

public:
    OHTDetect(HWCommon *pHWCommon);
	~OHTDetect();


	int				Enable();						// 활성화(전원 ON)
	int				Disable();						// 비활성화(전원 OFF)

//	int				ReOpenCom();				   //Comport open 실패시 재시도 하는 함수



	DETECT_STATUS	GetStatus();					// 상태를 받아오는 함수
	void			SetArea(DETECT_AREA_TYPE Type);	// 영역을 설정하는 함수
	double			GetMovableDist();			// 이동 가능한 거리를 받아오는 함수
	bool 			GetSensorStatus();
	bool 			GetCheckStatus();
	int				GetStatusLevel();				   // 상태를 받아오는 함수

    
	int PowerControl(int nMode);
    int ErrorReset();
	int AreaSelect(int nArea);
	int GetSensorLevel(bool bLong, bool bMiddle, bool bShort, bool bFail);
	static int GetSensorLevel2(bool bLong, bool bMiddle, bool bShort, bool bFail);
   //	int GetOutputSensorLevel(bool bSel1, bool bSel2, bool bSel3, bool bSel4, bool bPower);
//	AnsiString GetOutputSensorLevel(bool bSel1, bool bSel2, bool bSel3, bool bSel4, bool bPower);
	bool IsDetect(int nLevel);
	bool CheckError();

	int GetEnableTime(bool bInit);

	bool CheckSensorOff(); //대차 센서 Off 유무 확인 하는 함수

	//UBG 정합성 Code
	int 			Send_UBGData(unsigned char* ackData, int Length);				   //UBZ DATA를 받기 위해 신호를 보내는 함수
	int				Recieve_UBGData(unsigned char* pReadBuff);			   //UBZ DATA를 받아오는 함수


	//UBG Logging Code
	void DataReadStart(int *Buff); //요청
	int UpdataData(int *Buff,bool bIsStart = false);  //false RequestData(Loging 요청), true CheckUBG(Logging 수행)
	void RequestData(void);
	int CheckUbgSensorData(int *Buff);
	int ReceiveData(char *ReadBuff);
	int GetUbgAreaStatus();  // Check


	int Decode(int start, char *code, int byte);
 	int CheckUBGConnect(bool bDisConnect); //AOHC-256 AutoMode 시 UBG Fiemware 실행 불가
};

#endif  // OHTDetectH


