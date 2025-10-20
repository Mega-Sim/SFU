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
};

#endif  // HID_VII_ZR
