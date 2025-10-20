#ifndef Def_DefaultIniParamH
#define Def_DefaultIniParamH

#include <stdio.h>
#include "AxisParam.h"		// PROPOTINAL_AXIS_PARAM

/**
@brief	 디폴트 파라미터 
@author  puting
@date	 2016.11.26
*/
struct DEFAULT_PARAM_SET
{
	unsigned char VHLName[7];
	unsigned char BroadCasMask[20];


	unsigned char OHTType[20];  //0 : None (error 처리), 1: Foup 8.0, 2: Reticle 8.0(EUV) , 3: Foup 8.1, 4: Reticle 8.5(EUV) 5: Foup 8.1(고속화)
	unsigned char LineType[20];

	int nPortNumAdd;

	//Diag Agent 정보
	AnsiString strDiagIpAddr;
	int nDiagPort;

	bool IsLCDAndLED;    //add P1L VS P1_2L & P1_3L 구분 파라미터 #JRespect 2018.01.09
	bool IsRotateSensor;
	int  nHoistHomeSensorOriginConutAdd;
	bool  bHoistHomeSensorOriginRetry;
	bool  IsIOTHub;
	bool VHLLifterPassEnable;
	//Auto Teaching
	unsigned char AutoTeachingIP[20];

	//사전동작
	bool PreEQPIO;
	bool PreShutter;

    //2016.12.20 sh17.jo 무선 네트워크 상태 점검 관련 파라미터 추가
	unsigned char GatewayIP[20];
	unsigned char GatewayIP_OCS[20];	//2018.08.21 shcho : OCS의 gateway 주소. OCS 연결 불량시 Ping 시도하기 위함
    unsigned char WirelessClientIP[20];
    unsigned char UserID[10];
    unsigned char Password[10];
    unsigned int  nMaxOCSRcvTime;
    unsigned int  nMaxPingCheckTime_Step1;
    unsigned int  nMaxPingCheckTime_Step2;
    unsigned int  nMaxPingCheckTime_Step3;
    unsigned int  nMaxPingCheckTime_ETC;

	// IOT 이더넷의 윈도우 네트워크 이름.
	// 만약 설정되어 있지 않다면 기본이 첫번째 이더넷 카드가 IOT 랜카드라고 간주한다
	char IotEternetName[50];
	bool bIgnoreIoTLanCheck;    // IOT 게이트웨이 검사를 무시할지 여부

	PROPOTINAL_GEAR_RATIO DrivingDefault;
	PROPOTINAL_GEAR_RATIO ShiftDefault;
	THREE_LINEAR_EQUATION_GEAR_RATIO HoistDefault;

	PROPOTINAL_GEAR_ROTATE RotateDefault;
	
};

#endif
