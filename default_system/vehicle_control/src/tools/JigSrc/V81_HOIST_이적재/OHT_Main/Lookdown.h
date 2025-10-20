/**
@file    Lookdown.h
@version OHT 7.0
@brief   Lookdown Class Header File
*/

#ifndef LookdownH
#define LookdownH

#include "HWCommon.h"
#include "CommHandler.h"

//PBS용 일반 정의들
#define	  NUM_PBS_DATA		        121					   //PBS로부터 날라오는 거리 데이터의 갯수
#define	  PBS_DIFF_ANGLE		    0.0314159265358979	//PBS데이터 간의 각도 간격.  1.8 deg
#define	  PBS_START_ANGLE		    -0.314159265358979	//날라오는 데이터의 시작 각도
#define	PBS_BUFF_SIZE		        8192
#define	PBS_DISTOFFSET              100
#define	LINK_WAIT_TIME	            2400			//msec
#define LOOKDOWNLOGTIME  2000      //msec

/**
@class   Lookdown
@date    2012.10.30
@author  LSJ
@brief   Lookdown 센서 관련 Class
@note    Lookdown 센서의 전원이나 Area를 컨트롤 하는 함수 구현
*/
class Lookdown
{
private:
	HWCommon *m_pHWCommon;
	//신규진단모니터링 항목(Lookdown 센서 누적 가동시간)
	long m_nStartEnableTime;
	Comm_Port *m_pCommPort;
	bool bConnect;
	int m_nComPortNum;
	int m_nBaudRate;
	int m_nStatus;

	unsigned short	CertifCode;					      //16bit 인증코드. PBS에서 받은 code의 CRC 값에 해당한다.
	char	DecodedData[PBS_BUFF_SIZE];		   //디코딩된 데이터를 저장하는 공간.
	unsigned int				DistanceData[NUM_PBS_DATA];	//최종적으로 계산된 거리 데이터
	int            NumDecodedData;

	DWORD			dwStartTime;			// Start Time
	DWORD			dwLastLinkTime;			// Start Time

	int m_nArea;
    LogUnit *m_pLogLookdown;
public:
	Lookdown(HWCommon *pHWCommon);
	~Lookdown();

	int PowerControl(int nMode);
	int Enable();
	int Disable();
	int AreaSelect(int nArea);
	bool IsDetect();
	bool IsError();

	//신규진단모니터링 항목(Lookdown 센서 누적 가동시간)
	int GetEnableTime(bool bInit);

	void CheckSensorArea();
	void PBSDetect_ReadStart(void);
	void PBSDetect_ReadStop(void);
	int UpdataData(int *Buff);
private:
	void Init();
	int OpenCom();
	void CloseCom();
	void Obs_OnCommunication();
	void RequestCode(void);
	unsigned short LsbCRC(unsigned short , unsigned char *, unsigned short);
	void Encode(int , unsigned char *, unsigned char *);
	void Decode(int , unsigned char *, unsigned char *);
	int NumEncode(int);
	int NumDecode(int);
	bool OnGetCode(void);
	void RequestLink(void);
	bool OnGetLink(void);
	void RequestData(void);
	bool OnGetData(void);
	int ReadData(char *RtnReadBuff, char cTarget);
};

enum PBS_LOOKDOWN_STATUS
{
	STATUS_PBS_NOT_CONNTECTED = 1,
	STATUS_PBS_WAIT_CODE,
	STATUS_PBS_WAIT_FIRST_LINK_DATA1,
	STATUS_PBS_WAIT_FIRST_LINK_DATA2,
	STATUS_PBS_WAIT_LINK,
	STATUS_PBS_WAIT_DATA,
};


#define PBS_LINK_REQUEST_CYCLE_TIME 2400 // 3000ms 마다 link 끊김
#define PBS_WAIT_CODE_TIME 	200
#define PBS_WAIT_LINK_TIME     300
#define PBS_WAIT_DATA_TIME     300


#endif  // LookdownH


