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
//	unsigned char LineType[20];

	unsigned char YSCode[20];

//	int nPortNumAdd;
    int nCarrierType;
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
	unsigned int  nWlanCheckCount;

	// IOT 이더넷의 윈도우 네트워크 이름.
	// 만약 설정되어 있지 않다면 기본이 첫번째 이더넷 카드가 IOT 랜카드라고 간주한다
	char IotEternetName[50];
	bool bIgnoreIoTLanCheck;    // IOT 게이트웨이 검사를 무시할지 여부

	PROPOTINAL_GEAR_RATIO DrivingDefault;
	PROPOTINAL_GEAR_RATIO ShiftDefault;
	THREE_LINEAR_EQUATION_GEAR_RATIO HoistDefault;

	PROPOTINAL_GEAR_ROTATE RotateDefault;

	//AVS Use
	bool bUseAVS;

	//Regulator 확인필요 여부(통합 CP)
	bool bRegulatorCheck;

	bool bUseblackbox;
	bool bUseVisionTeaching;

	//BCR Choice
	bool bUseLeuze;
	bool bUseUBGMonitoring;

	//CID choice
	bool  bUse_CID_FIBER_MONITORING;
	int   nCID_SERIAL_CMD_DELAY_TURN;

	//OHT Detect Choice
	int OhtDetectSensorType;
	int IPCType;
//	bool bMacType;

	bool bUseRSA;
	bool bSOM6869;

	bool bUseVMS;
	int nBlueTooth1_StationID;
	AnsiString strBlueTooth1_DeviceID;
	int nBlueTooth2_StationID;
	AnsiString strBlueTooth2_DeviceID;

	bool bUse4ChBlackBox;
    unsigned char BlackBoxIP[20];
};

/**
@brief	 Line 파라미터 AOHT-285 라인 파라미터 추가
@author  puting
@date	 2020.01.16
*/
struct DEFAULT_LINE_PARAM_SET
{
//	unsigned char LineType[20];  //해당 라인에 가능한 모델인지 체크하는 기능 추가.

	bool bUseLowVibration;      //저진동 주행 관련 적용 유무
	bool bUseTransProfile;      //이적재 프로파일 개선 적용
	bool bUseCarrierID;         //Carrier ID 프로토콜 사용 및 Fosb/Foup 분류 처리 작업. 현재 미사용
	bool bUseOptionalNode;      //OptionalNode 사용유무
	double dLineHoistDistance_Foup; //라인 내 이적재 호이스트 범위
	double dLineHoistDistance_Euv;  //라인 내 이적재 호이스트 범위
	int nPortNumAdd;
	bool bUseFoupCoverOepnwaitingInterlock;
	bool bUseE84TimeoutErrorCode;

};

// AOHT-285
DEFAULT_LINE_PARAM_SET line_Table[] = {{0,0,0,0,3100.0,3100.0,0,0,0},       //천안 (defulat, 정합성 제외)
									   {0,0,0,0,3100.0,3100.0,0,0,1},       //P1
									   {0,0,0,0,3000.0,3000.0,0,0,1},       //P1_1F
									   {1,1,1,1,3100.0,3100.0,200,0,1},     //P1_4F
									   {1,1,1,1,3300.0,5300.0,0,1,0},       //V1_3F
									   {1,1,1,1,3300.0,3300.0,0,1,0},       //V1_10F
									   {1,1,1,1,3100.0,3100.0,0,0,1},       //V2
									   {1,1,1,1,3100.0,3100.0,0,0,1},       //P2
									   {1,1,1,1,3100.0,3100.0,0,0,1},       //P3
									   {1,1,1,1,3100.0,3100.0,0,0,1},      //P4
									   {1,1,1,1,3100.0,3100.0,0,0,1},       //C3
									   {1,1,1,1,3100.0,3100.0,0,0,1},       //C4
									   {1,1,1,1,3100.0,3100.0,0,0,1},
									   {1,1,1,1,3100.0,3100.0,0,0,1}};      //Unknown



enum LINE_INFO
{
	LINE_INFO_SEMES = 0,
	LINE_INFO_P1 = 1,
	LINE_INFO_P1_1F,
	LINE_INFO_P1_4F,
	LINE_INFO_V1_3F,
	LINE_INFO_V1_10F,
	LINE_INFO_V2,
	LINE_INFO_P2,
	LINE_INFO_P3,
	LINE_INFO_P4,
	LINE_INFO_C3,
	LINE_INFO_C4,
	LINE_INFO_NRDK,
	LINE_INFO_UNKNOWN,
};


//추후 라인 정보규칙성 추가 예정.(SSID기준)
#define LINETYPE_SEMES_1st  "Mech_TOHT_a4" 		    // AOHT-285
#define LINETYPE_SEMES_2nd  "Mech_TOHT_a5" 		    // AOHT-285
#define LINETYPE_SEMES_3rd  "Mech_TOHT_a6" 		    // AOHT-285
#define LINETYPE_P1_3F_TB   "SEMES_P1L_FAB_3F_TB"
#define LINETYPE_P2_7F_TB   "SEMES_P2L_TB_7F"
#define LINETYPE_V1_10F_TB  "SEMES_S3-DL_TB_10F"
#define LINETYPE_V1_10F_TB_2	"SEMES_S3-DL_FAB_10F_TEST"
#define LINETYPE_V2_3F_TB	"SEMES_V2L_TESTBED_3F"
#define LINETYPE_P1_1F      "SEMES_P1L_FAB_1F" 		// AOHT-285
#define LINETYPE_P1_3F      "SEMES_P1L_FAB_3F" 	 	// AOHT-285
#define LINETYPE_P1_7F      "SEMES_P1L_FAB_7F" 		// AOHT-285
#define LINETYPE_P1_4F      "SEMES_P1L_FAB_4F"    	// AOHT-285
#define LINETYPE_V1_3F      "SEMES_S3-EL_FAB_3F"    // AOHT-285
#define LINETYPE_V1_10F     "SEMES_S3-DL_FAB_10F"   // AOHT-285
#define LINETYPE_V2_7F      "SEMES_P2L_BIZ_7F"      // AOHT-285
#define LINETYPE_V2_7F_2    "SEMES_P2L_FIZ_7F"      // AOHT-285
#define LINETYPE_V2_8F      "SEMES_V2L_EUV_8F"      // AOHT-285
#define LINETYPE_V2_3F      "SEMES_P2L_FIZ_3F"      // AOHT-285
#define LINETYPE_V2_3F_2    "SEMES_P2L_BIZ_3F"      // AOHT-285
#define LINETYPE_V2_3F_3    "SEMES_S5_V2L_3F"       // AOHT-285
#define LINETYPE_P2_1F      "SEMES_P2L_FAB_1F"      // AOHT-285
#define LINETYPE_P2_3F_W    "SEMES_P2L_FAB_3F"      // AOHT-285  (하서)
#define LINETYPE_P2_3F_E    "SEMES_P2L_S5_3F"      // AOHT-285   (하동)
#define LINETYPE_P2_7F      "SEMES_P2L_FAB_7F"      // AOHT-285
#define LINETYPE_P3_1F      "SEMES_P3L_FAB_1F"
#define LINETYPE_P3_7F      "SEMES_P3L_FAB_7F"
#define LINETYPE_P3_3F_E    "SEMES_P3L_S5_3F"
#define LINETYPE_P3_5F_TB   "SEMES_P3L_TB_3F"
#define LINETYPE_P4_1F      "SEMES_P4L_FAB_1F"
#define LINETYPE_P4_3F      "SEMES_P4L_FAB_3F"
#define LINETYPE_P4_7F      "SEMES_P4L_FAB_7F"
#define LINETYPE_P4_3F_E    "SEMES_P4L_S5_3F"
#define LINETYPE_P4_MASK    "SEMES_P4L_MASK_3F"
#define LINETYPE_P4_MASK_TB "SEMES_P4L_MASK_TB_3F"
#define LINETYPE_C3_2F_TB   "SEMES_C3_FAB_2F_TESTBED"
#define LINETYPE_C3_2F		"SEMES_C3_FAB_2F"
#define LINETYPE_C4_2F		"SEMES_C4_FAB_2F"
#define LINETYPE_NRDK_1F	"SEMES_NK1_1F"
#define LINETYPE_NRDK_3F	"SEMES_NK1_3F"
#define LINETYPE_NRDK_7F	"SEMES_NK1_7F"
#define LINETYPE_NRDK_TB	"SEMES_NK1_TB"

//AOHC-154
struct  DEFAULT_AUTOPATCH_PARAM_SET
{
  bool bAutoPatchControl;
};
#endif
