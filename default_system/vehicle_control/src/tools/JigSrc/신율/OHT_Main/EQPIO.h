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
} sTIMEOVER_EQPIO;

typedef struct EQPIO_PARAM_
{
	//OHT Name
	char Version[8];

	int nMaxIndex;
	int m_nPIOCSONDelayTime;

	sTIMEOVER_EQPIO m_TIMEOVER_EQPIO[10];

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
//	sTIMEOVER_EQPIO m_TIMEOVER_EQPIO[10];

//	int m_dwPIOCSONDelayTime;
	
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V81S) || (OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V86))
    Comm_Port *m_pCommPort;
    int m_nPortNum;
    bool m_bRFPIO_Setting_Start;
	bool m_bSettingComplete;
    UINT m_nCmdPIOType;     //LimJY_20160122 : For IR+RF PIO (0:IR, 1:RF, 2:Hybrid 5.0GHz)
	UINT m_nCmdID;
	UINT m_nCmdChannel;
	UINT m_nCmdCS;
	char m_nCmdOHTNo[7];

	int m_nSetRFPIOStep;
#endif

public:

	EQPIO(HWCommon *pHWCommon, int nLeftRight);
	~EQPIO();


	EQPIO_PARAM  *m_pParamSet;
	bool LoadEQPIOParam(char* szPath, EQPIO_PARAM  *pParamSet);

	bool LoadEQPIOParamRecorvery(char* szFilePath, EQPIO_PARAM* pParamSet);  //진단 파라메터 읽기
	int m_ParamChehckError;
	bool saveParameter(char* szFilePath, EQPIO_PARAM* pParamSet);

	int	Init();
	int PIOEnable();
	int PIODisable(bool bOutClear);
	int AllPIOOff();
	int SequenceBeforeLoad(int PIOTimeoverLevel);
	int SequenceBeforeUnload(int PIOTimeoverLevel);
	int SequenceAfterLoad(int PIOTimeoverLevel);
	int SequenceAfterUnload(int PIOTimeoverLevel);
	int SequenceForMoving(int PIOTimeoverLevel);
	void Init_SequenceForLoading();
	void Init_SequenceForUnloading();
	void Init_SequenceForMoving();
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
	int TRREQOnOff(bool bOnOff);
	int BusyOnOff(bool bOnOff);
	int CompleteOnOff(bool bOnOff);
    int GetReadyOffTimeover(int PIOTimeoverLevel);
    int GetLREQOffTimeover(int PIOTimeoverLevel);
	int GetPIOStep();
	int GetLoadPIOStep();
	int GetUnloadPIOStep();
	int GetAfterLoadPIOStep();
	int GetAfterUnloadPIOStep();
	int Check_GoOffTime(int nPeriodTime, int nStep);
	void Reset_EQPIO();
	int Save_HybridPIO_Data();

	int Load_EQPIO24G(UINT CurStationID);
	
	bool IsSelectOff();
    void Init_SequenceForRFPIOConnection();
	int GetRFPIOConnectStep();
    int ReadData(char *RtnReadBuff, char cTarget);
	int RFPIO_Open();
	void RFPIO_Close();
	int Set_RFPIO_Data_RS232_Hybrid(UINT nPIOType, UINT nRFID, UINT nRFCHA, UINT nRFCS,char* sOHTName, bool bPreTrans = false);     //LimJY_20160122 : For IR+RF PIO UINT nPIOType 추가
    int Set_RFPIO_Data_RS232_RF(UINT nRFID, UINT nRFCHA, UINT nRFCS);					     //For RF PIO
    int Set_RFPIO_TYPE(void);         //LimJY_20160122 : For IR+RF PIO
    int Set_RFPIO_CHANNEL(void);      //LimJY_20160122 : For IR+RF PIO
    int Set_RFPIO_ID(void);           //LimJY_20160122 : For IR+RF PIO
    int Set_RFPIO_CS(void);           //LimJY_20160122 : For IR+RF PIO
    int Set_RFPIO_OHT_No(void);       //LimJY_20160122 : For IR+RF PIO
	int Set_RFPIO_IDCHANNEL(void);    //For RF PIO
    int Set_RFPIO_Reset(void);        //For RF PIO
	int Set_HybridPIO_Type(void);     //LimJY_20160425 : For IR+RF PIO
    int Set_HybridPIO_Confirm(void);  //LimJY_20160122 : For IR+RF PIO
	int Get_RFPIO_Response_Hybrid(AnsiString strSndData, AnsiString &strRcvData);	//For IR+RF PIO
    int Get_RFPIO_Response_RF(AnsiString strSndData, AnsiString &strRcvData);		//For RF PIO
    AnsiString CheckSum_StringToHex(AnsiString sData);      //LimJY_20160122 : For IR+RF PIO

};

//---------------------------------------------------------------------------
#endif  //EQPIOH

