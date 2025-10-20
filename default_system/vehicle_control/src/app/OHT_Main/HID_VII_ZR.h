/**
@file    HID.h
$Rev:: 679           $:  Revision of last commit
$Author:: ltw070     $:  Author of last commit
$Date:: 2013-03-11 1#$:  Date of last commit
@version OHT 7.0
@brief   HID Class Header File
*/

#ifndef HID_VII_ZRH
#define HID_VII_ZRH

#include <Classes.hpp>
#include <dos.h>

#include "Virtual_HID.h"
#include "LogHandler.h" // Log 작성 목적
#include "ExecuteInfo.h"
#include "OHTMainStatus.h"

#include <time.h>

#include <fstream>
#include <iostream>

#include "PUtily.h"

#define SEND_PK_LENGTH70		   	18
#define RCV_PK_LENGTH70			 	44
#define RCV_PK_LENGTH85			 	80
#define RCV_PK_LENGTH_CP            85       //통합 CP ver.
#define RCV_PK_LENGTH_ECO           143      //ECO ver.
#define RCV_PK_LENGTH_BMS			179      //통합파워모듈 ver.


#define RCV_PK_PROTOCOL_LENSTD		50

//#include "atlstr.h"
//#pragma comment(lib, "GPProgrammer.lib")
//#include "GPProgrammer.h"

/**
@class   HID
@date    2012.10.13
@author  zzang9un
@brief   HID 관련 Class
@note    Serial 통신으로 HID Data를 읽어들인다.
*/

class HID_VII_ZR : public Virtual_HID
{
private:

	void GetSendData(char *Buff,int nMoveInfo, int nPathInfo);    					   //puting
	void GetSendData(char *SendData,int nMoveInfo, int nPathInfo, bool errorDataFlag);
	BYTE  CheckSUMData(char* SendData, int Len);   //puting
	bool  GetRcvDataCheckSum(char* RcvData, int Len);
	int ErrorConvert(int nErrorcode);
	void SendFirstData(char *SendData);


//	char  CheckSUMData_V85(char* SendData, int Len, int Num);   //puting
	bool  GetRcvDataCheckSum_V85(char* RcvData, int Len);

	char GetSendData_Type(int nType);
	char GetSendData_Path(int nType);

	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;

	FDCUnit *pFDCUnit;

protected:
	int AnalysisData(char* chRcvData);


public:
	HID_VII_ZR(int ComPortNum);
	~HID_VII_ZR();


	int UpdateData(int nMoveInfo, int nPathInfo);
	HID_DATA GetHIDData();

	int SaveErrorDataLog(void);

	void SetRegulatorCheck(bool bUse);
	bool RegulatorCheck;           //regulator 버전 체크가 필요한지 확인하는 변수
	bool BMS_Ver;                  //regulator가 ECO or 통합 파워 모듈 버전인지 확인하는 변수
	bool GetVer;                   //처음 한 번 버전을 가져왔는지 확인하는 변수.
	int Ver_Cnt;                   //통합 regulator일 때 버전 체크 횟수
	bool bChargeEnable;                  //노드 충전, 방전 여부
	int bError;                   //에러 발생 여부
};

#endif  // HID_VII_ZR
