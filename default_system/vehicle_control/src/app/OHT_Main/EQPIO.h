//---------------------------------------------------------------------------
/**
@file    EQPIO.h
@version OHT 7.0
@brief   EQPIO Class Header File
*/

#ifndef EQPIOH
#define EQPIOH

#include "HWCommon.h"
#include "LogHandler.h" // Log 작성 목적
#include "CommHandler.h"
#include "ExecuteInfo.h"
#include "OHTMainStatus.h"
#include "Def_DefaultIniParam.h"
#include "Def_MCCParam.h"

#define PIO_LOAD 0
#define PIO_UNLOAD 1

typedef enum DUAL_STAGE_STB_STEP_BEFORE_LOAD_
{
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_GO_SIG = 1,
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_SIG_OFF,
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_SIG_OFF_DELAY,
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_HO_AVBL,
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_HO_ES,
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_CS_ON,
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_CS_ON_DELAY,
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_VALID_ON,
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_CS,	//9
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_PORT_NP,   //10
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_TR_REQ_ON,           //11
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_READY,             //12
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_BUSY_ON,                     //13
	DUAL_STAGE_STB_STEP_BEFORE_LOAD_START // 작업 시작
} DUAL_STAGE_STB_STEP_BEFORE_LOAD;

typedef enum DUAL_STAGE_STB_STEP_AFTER_LOAD_
{
	DUAL_STAGE_STB_STEP_AFTER_LOAD_BUSY_OFF = 1,
	DUAL_STAGE_STB_STEP_AFTER_LOAD_BUSY_OFF_DELAY,
	DUAL_STAGE_STB_STEP_AFTER_LOAD_COMPT_ON,
	DUAL_STAGE_STB_STEP_AFTER_LOAD_CHECK_READY_OFF,
	DUAL_STAGE_STB_STEP_AFTER_LOAD_SIG_OFF,
	DUAL_STAGE_STB_STEP_AFTER_LOAD_SIG_OFF_DELAY,
	DUAL_STAGE_STB_STEP_AFTER_LOAD_FINISH // 작업 종료
} DUAL_STAGE_STB_STEP_AFTER_LOAD;

/**
@brief   EQ PIO 통신 시 입력 On/Off에 대한 Timeover Data의 구조체
@author  LSJ
@date    2012.11.07
*/
typedef struct TIMEOVER_EQPIO_
{
   int nLREQ_ON_Time;
   int nLREQ_OFF_Time;
   int nUREQ_ON_Time;
   int nUREQ_OFF_Time;
   int nReady_ON_Time;
   int nReady_OFF_Time;
   int nHOAVBL_ON_Time;
   int nES_ON_Time;
   int nCS_ON_Delay_Time;
} sTIMEOVER_EQPIO;

typedef struct TIMEOVER_DUAL_STAGE_STB_PIO_
{
   int nReady_ON_Time;
   int nReady_OFF_Time;
   int nHOAVBL_ON_Time;
   int nES_ON_Time;
   int nCS_ON_Delay_Time;
   int nCS_Resp_Delay_Time;
   int nPort_P_ON_Time;
   int nPort_P_OFF_Time;
   int nPort_N_ON_Time;
   int nPort_N_OFF_Time;
} sTIMEOVER_DUAL_STAGE_STB_PIO;

typedef struct EQPIO_PARAM_
{
	//OHT Name
	char Version[8];

	int nMaxIndex;
//	int m_nPIOCSONDelayTime;
	int nChatteringCount;
	int nPSTB_GO_OFF_Time;

	sTIMEOVER_EQPIO m_TIMEOVER_EQPIO[10];
	sTIMEOVER_DUAL_STAGE_STB_PIO m_TIMEOVER_DUALSTAGEPIO;

}EQPIO_PARAM;

/**
@class   EQPIO
@date    2012.10.22
@author  LSJ
@brief   EQPIO 통신 관련 Class
@note    EQPIO 통신을 위한 함수 구현
*/
class EQPIO
{
private:
	HWCommon *m_pHWCommon;

	LogUnit *pLogUnit;      ///< TaskControl 로그 생성을 위한 변수
	LogUnit *pLogMCC;
	LogUnit *pLogMDetail;
	FDCUnit *pFDCUnit;
	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;
	DEFAULT_PARAM_SET *m_defualtparam;
	MCC_PIO_PARAM_SET	m_mccparam;
    int m_nSide;
	int m_nStep;
	int m_nBeforeLoadStep;
	int m_nBeforeUnloadStep;
	int m_nAfterLoadStep;
	int m_nAfterUnloadStep;
	DWORD m_dwPIOWaitTime;
	DWORD m_dwLoadPIOWaitTime;
	DWORD m_dwUnloadPIOWaitTime;

	bool bCheckEQParam;

	int m_nRespSleep;
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V81S) || (OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
    Comm_Port *m_pCommPort;
    int m_nPortNum;
    bool m_bRFPIO_Setting_Start;
	bool m_bSettingComplete;
    UINT m_nCmdPIOType;     //LimJY_20160122 : For IR+RF PIO (0:IR, 1:RF, 2:Hybrid 5.0GHz)
	UINT m_nCmdID;
	UINT m_nCmdChannel;
	UINT m_nCmdHexType;  //시리얼 번호를 통해 ID 설정하는 기능

	char m_nCmdOHTNo[7];

	int m_nSetRFPIOStep;
#endif

public:

	EQPIO(HWCommon *pHWCommon, int nLeftRight);
	~EQPIO();

#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V81S) || (OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
	UINT m_nCmdCS;
#endif

	EQPIO_PARAM  *m_pParamSet;
	bool LoadEQPIOParam(char* szPath, EQPIO_PARAM  *pParamSet);

	bool LoadEQPIOParamRecorvery(char* szFilePath, EQPIO_PARAM* pParamSet);  //진단 파라메터 읽기
	int m_ParamChehckError;
	bool saveParameter(char* szFilePath, EQPIO_PARAM* pParamSet);

	int	Init();
	int PIOEnable();
	int PIODisable(bool bOutClear);
	int AllPIOOff();
	int SequenceBeforeLoad(int PIOTimeoverLevel,int nEQType);
	int SequenceBeforeUnload(int PIOTimeoverLevel,int nEQType);
#if(OHT_NAME==OHT_NAME_P4_MASK)	//mgchoi. 2022.08.23. P4 MaskOHT용 사용함수
	int SequenceAfterLoad(int PIOTimeoverLevel);
#endif

	int SequenceAfterUnload(int PIOTimeoverLevel,int nEQType);
	int SequenceForMoving(int PIOTimeoverLevel,int nEQType);
	int SequenceBeforeLoadForDualStageStb();
	//int SequenceBeforeUnloadForDualStageStb();
	int SequenceBeforeTransForDualStageStb(bool bLoadUnload);
	int SequenceAfterTransForDualStageStb(bool bLoadUnload);
	void Init_SequenceForLoading();
	void Init_SequenceForUnloading();
	void Init_SequenceForMoving();
	bool IsPortPOn();
	bool IsPortNOn();
	bool IsCS0On();
	bool IsCS1On();
	bool IsCS2On();
	bool IsLoadREQOn();
	bool IsUnloadREQOn();
	bool IsReadyOn();
	bool IsHOAVBLOn();
	bool IsESOn();
	bool IsGoSignalOn();
	bool IsValidOn();
	bool IsCompleteOn();
	int ValidOnOff(bool bOnOff);
	int CS0OnOff(bool bOnOff);
	int CS1OnOff(bool bOnOff);
	int CS2OnOff(bool bOnOff);
	int CS3OnOff(bool bOnOff);
	int CSBitOnOff(bool bOnOff);
	int TRREQOnOff(bool bOnOff);
	int BusyOnOff(bool bOnOff);
	int CompleteOnOff(bool bOnOff);
    int GetReadyOffTimeover(int PIOTimeoverLevel);
    int GetLREQOffTimeover(int PIOTimeoverLevel);
	int GetPIOStep();
	int GetLoadPIOStep();
	int GetUnloadPIOStep();
	int GetAfterLoadPIOStep();                  //미사용
	int GetAfterUnloadPIOStep();
	int Check_GoOffTime(int nPeriodTime, int nStep);
//	void Reset_EQPIO();                       //미사용
	int Save_HybridPIO_Data();

	int Check_RFPIO_Response_Hybrid_Duplicated(bool bCheckCount = true);
	int nDuplicated_CheckCount;

//	int Load_EQPIO24G(UINT CurStationID);    //미사용

//	bool IsSelectOff();                     //미사용
	void Init_SequenceForRFPIOConnection();
	int GetRFPIOConnectStep();
    int ReadData(char *RtnReadBuff, char cTarget);
	int RFPIO_Open();
//	void RFPIO_Close();               //미사용
	int Set_RFPIO_Data_RS232_Hybrid(UINT nPIOType, UINT nRFID, UINT nRFCHA, UINT nRFCS, char* sOHTName, bool bPreTrans = false);     //LimJY_20160122 : For IR+RF PIO UINT nPIOType 추가
	int Set_RFPIO_Data_RS232_RF(UINT nRFID, UINT nRFCHA, UINT nRFCS);					     //For RF PIO
//	int Set_RFPIO_TYPE(void);         //LimJY_20160122 : For IR+RF PIO          //미사용
//	int Set_RFPIO_CHANNEL(void);      //LimJY_20160122 : For IR+RF PIO          //미사용
//	int Set_RFPIO_ID(void);           //LimJY_20160122 : For IR+RF PIO          //미사용
//	int Set_RFPIO_CS(void);           //LimJY_20160122 : For IR+RF PIO          //미사용
//	int Set_RFPIO_OHT_No(void);       //LimJY_20160122 : For IR+RF PIO          //미사용
	int Set_RFPIO_IDCHANNEL(void);    //For RF PIO
    int Set_RFPIO_Reset(void);        //For RF PIO
	int Set_HybridPIO_Type(void);     //LimJY_20160425 : For IR+RF PIO
    int Set_HybridPIO_Confirm(void);  //LimJY_20160122 : For IR+RF PIO
	int Get_RFPIO_Response_Hybrid(AnsiString strSndData, AnsiString &strRcvData);	//For IR+RF PIO
    int Get_RFPIO_Response_RF(AnsiString strSndData, AnsiString &strRcvData);		//For RF PIO
    AnsiString CheckSum_StringToHex(AnsiString sData);      //LimJY_20160122 : For IR+RF PIO

	void Update_MCC();
	int Get_EQPIO_Ver();
	bool bIsPSTB;
};

//---------------------------------------------------------------------------
#endif  //EQPIOH

