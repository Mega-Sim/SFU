// ---------------------------------------------------------------------------
/**
@file    EQPIO.cpp
@version OHT 7.0
@brief   EQPIO Class CPP File
*/

#include <IniFiles.hpp>
#include <Filectrl.hpp>
#include "EQPIO.h"
#include "Utility.h"

#include "OHTMain.h" //test

// -------------------------------------------
// Debugging용 Define
// -------------------------------------------
#define USE_LOG_MEMO

#define ADD_MEMO_T(STR)   OHTMainForm->memoMain->Lines->Add(_T(STR))
#define ADD_MEMO_S(STR)   OHTMainForm->memoMain->Lines->Add(STR)


#define PIO_LOG(...)	        WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)
//#define ADD_MCC_LOG(usemode, ...)	if(usemode)   WRITE_LOG_UNIT(pLogMCC, __VA_ARGS__)
#define ADD_MCC_LOG(usemode, ...)							\
	if(usemode)												\
	{										        		\
		try{									    		\
			WRITE_LOG_UNIT(pLogMCC, __VA_ARGS__)    		\
		}                                           		\
		catch(...)                                  		\
		{                                           		\
			WRITE_LOG_UNIT(pLogMCC, "EQ Exception")		\
		}                                           		\
	}
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)
#define SEND_WARN(...)				writeFDCUnit_WARN(pFDCUnit, __VA_ARGS__)
#define ADD_MD_LOG(...)    WRITE_LOG_UNIT(pLogMDetail, __VA_ARGS__)  //AOHC-253 Monitoring 항목 추가 2

/*******************************************************************************
 *  Content : PIO/Sensor Check Limit Time                 *
*******************************************************************************/
#define  L_REQ_OFF_TIMEOVER         10000  //[msec]=10sec
#define  L_REQ_WAITTIME             30000  //30sec
#define  U_REQ_WAITTIME             30000  //30sec
#define  HO_AVBL_ONTIME             60000  //8->60sec
#define  ES_ONTIME	               3000
#define  READY_ONTIME               60000  //60sec
#define  READY_OFFTIME              60000  //60sec
#define  U_REQ_OFFTIME              10000  //10sec

#define  PSTB_GO_OFF_DEFAULT_TIME              30000  //30sec

#define  NAME_EQPIOFILE              "EQPIO.ini"

#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S) ||  (OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE)||(OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
#define  EQPIO_BAUDRATE             38400   //LimJY_20160119 : For IR+RF PIO 57600 -> 38400
#define  EQPIO_FREQ_MAX             5814    //LimJY_20160119 : For IR+RF PIO 최대 주파수 5814 MHz
#define  EQPIO_FREQ_BAND            5775    //LimJY_20160119 : For IR+RF PIO 시작 주파수 5775 MHz
#define  EQPIO_BANDWIDTH            40      //LimJY_20160119 : For IR+RF PIO 40MHz(40개 채널사용)
#define  EQPIO_BASE_CHANNEL         178     //LimJY_20160119 : For IR+RF PIO 시작 채널 178 (178 ~ 217 사용가능)
#endif

/**
@brief   EQPIO 생성자
@author  LSJ
@date    2012.10.13
@param   pHWCommon : HWCommon Class의 포인터
@param   nLeftRight : EQ PIO 2개 중 어떤 PIO인지를 넘겨주는 인자
@note    Input, Ouput을 사용하기 위해 HWCommon의 Pointer를 가져옴, PIO Timeover 관련 데이터 초기화
*/
EQPIO::EQPIO(HWCommon *pHWCommon, int nLeftRight)
{
	static bool bPIOINIFileReadFlag = false;

	pLogUnit = getLogUnit("D:\\log\\EQPIOError.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogMCC = getLogUnit("D:\\log\\MCC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogMDetail = getLogUnit("D:\\log\\MonitoringDetail.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	m_pHWCommon = pHWCommon;
	m_nSide = nLeftRight;
	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
	m_defualtparam = OHTMainForm->GetDefaultParam();
	m_ParamChehckError = NO_ERR;
#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME== OHT_NAME_P4_MASK))
	m_bRFPIO_Setting_Start = false;
	m_bSettingComplete = false;
	m_nCmdPIOType = EQ_PIO_IR;     //LimJY_20160122 : For IR+RF PIO (0:IR, 1:Hybrid 5.0GHz 2:RF, 3:Hybrid 5.0GHz_SN )
	m_nCmdID = 0;
	m_nCmdChannel = 0;
	m_nCmdCS = 0;
	m_nCmdHexType = 0;

//    m_nCmdOHTNo = 0;
	memset(m_nCmdOHTNo, 0x00, sizeof(m_nCmdOHTNo));
	m_nSetRFPIOStep = 0;
#endif

	m_pParamSet = new EQPIO_PARAM();
	memset(m_pParamSet, 0x00, sizeof(EQPIO_PARAM));

	if(bPIOINIFileReadFlag == false)
	{
		LoadEQPIOParamRecorvery("DataFiles\\EQPIO.ini",m_pParamSet);
		bPIOINIFileReadFlag = true;
	}

	bCheckEQParam = false;

	Init();

	m_nRespSleep = 100;
	bIsPSTB = false;

}

/**
@brief   EQPIO 소멸자
@author  LSJ
@date    2012.10.13
*/
EQPIO::~EQPIO()
{
	delete m_pParamSet;

	if(m_pCommPort!=NULL)
		delete m_pCommPort;
}


/**
@brief   ini 파일에서 EQ PIO Timeover 관련된 Data를 Load 함.
@author  LSJ
@date    2012.11.07
@return  Unload 명령을 문제 없이 수행하면 0, 문제가 발생하면 해당 에러코드를 리턴함.
*/
bool EQPIO::LoadEQPIOParamRecorvery(char* szFilePath, EQPIO_PARAM  *pParamSet)
{

	bool bSuccess = true;
	char strIndex[255] = {0, };
	char strTempValue[8]= {0,};
	bool bCheckParam = false;
	int nTmpValue = 0;

	LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile(szFilePath);

	if(!pIniFile->GetIniFileExist())
    {
		delete pIniFile;
		pIniFile = NULL;
		bSuccess = false;
	}

	if(pIniFile != NULL)
	{
		//Read Diag Agent informations
		pIniFile->ReadString("Version","IniVersion", "00", strTempValue);
		if(StrToIntA(strTempValue) <= StrToIntA(pParamSet->Version))
			bSuccess = false;
		else
		{
			memmove(&pParamSet->Version, strTempValue, strlen(strTempValue)); //Return Value
			memset(strTempValue, 0, sizeof(strTempValue));
		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("PIO", "nMaxIndex", 10, 0, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nMaxIndex = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("PIO", "nChatteringCount", 0, 0, 1000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nChatteringCount = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("PSTB", "nPSTB_GO_OFF_Time", 30000, 30000, 60000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nPSTB_GO_OFF_Time = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
           	for (int j = 0; j < pParamSet->nMaxIndex; j++)
			{
				sprintf(strIndex, "PIO%d", j+1);

				bCheckParam =  pIniFile->ReadIntegerMinMaxVerification(strIndex, "nLREQ_ON_Time", L_REQ_WAITTIME, 5000, 60000, &nTmpValue);
				if(bCheckParam)
				{
					pParamSet->m_TIMEOVER_EQPIO[j].nLREQ_ON_Time = nTmpValue;
				}
				else
				{
					bSuccess = false;
					break;
				}

				bCheckParam =  pIniFile->ReadIntegerMinMaxVerification(strIndex, "nLREQ_OFF_Time", L_REQ_OFF_TIMEOVER, 5000, 60000, &nTmpValue);
				if(bCheckParam)
				{
					pParamSet->m_TIMEOVER_EQPIO[j].nLREQ_OFF_Time = nTmpValue;
				}
				else
				{
					bSuccess = false;
					break;
				}

				bCheckParam =  pIniFile->ReadIntegerMinMaxVerification(strIndex, "nUREQ_ON_Time", U_REQ_WAITTIME, 5000, 60000, &nTmpValue);
				if(bCheckParam)
				{
					pParamSet->m_TIMEOVER_EQPIO[j].nUREQ_ON_Time = nTmpValue;
				}
				else
				{
					bSuccess = false;
					break;
				}

				bCheckParam =  pIniFile->ReadIntegerMinMaxVerification(strIndex, "nUREQ_OFF_Time", U_REQ_OFFTIME, 5000, 60000, &nTmpValue);
				if(bCheckParam)
				{
					pParamSet->m_TIMEOVER_EQPIO[j].nUREQ_OFF_Time = nTmpValue;
				}
				else
				{
					bSuccess = false;
					break;
				}

				bCheckParam =  pIniFile->ReadIntegerMinMaxVerification(strIndex,"nReady_ON_Time", READY_ONTIME, 5000, 60000, &nTmpValue);
				if(bCheckParam)
				{
					pParamSet->m_TIMEOVER_EQPIO[j].nReady_ON_Time = nTmpValue;
				}
				else
				{
					bSuccess = false;
					break;
				}

				bCheckParam =  pIniFile->ReadIntegerMinMaxVerification(strIndex,"nReady_OFF_Time", READY_OFFTIME, 5000, 60000, &nTmpValue);
				if(bCheckParam)
				{
					pParamSet->m_TIMEOVER_EQPIO[j].nReady_OFF_Time = nTmpValue;
				}
				else
				{
					bSuccess = false;
					break;
				}

				bCheckParam =  pIniFile->ReadIntegerMinMaxVerification(strIndex,"nHOAVBL_ON_Time", HO_AVBL_ONTIME, 5000, 100000, &nTmpValue);
				if(bCheckParam)
				{
					pParamSet->m_TIMEOVER_EQPIO[j].nHOAVBL_ON_Time = nTmpValue;
				}
				else
				{
					bSuccess = false;
					break;
				}

				bCheckParam =  pIniFile->ReadIntegerMinMaxVerification(strIndex,"nES_ON_Time", ES_ONTIME, 1000, 100000, &nTmpValue);
				if(bCheckParam)
				{
					pParamSet->m_TIMEOVER_EQPIO[j].nES_ON_Time = nTmpValue;
				}
				else
				{
					bSuccess = false;
					break;
				}

				bCheckParam =  pIniFile->ReadIntegerMinMaxVerification(strIndex,"nCS_ON_Delay_Time", EQPIO_CS_ON_TIME, 0, 500, &nTmpValue);
				if(bCheckParam)
				{
					pParamSet->m_TIMEOVER_EQPIO[j].nCS_ON_Delay_Time = nTmpValue;
				}
				else
				{
					bSuccess = false;
					break;
				}


			}

			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("STBPIO", "nPort_P_ON_Time", 30000, 5000, 60000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nPort_P_ON_Time = nTmpValue;
			}
			else
			{
				//bSuccess = false;
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nPort_P_ON_Time = 30000;
			}

			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("STBPIO", "nPort_P_OFF_Time", 30000, 5000, 60000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nPort_P_OFF_Time = nTmpValue;
			}
			else
			{
				//bSuccess = false;
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nPort_P_OFF_Time = 30000;
			}

			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("STBPIO", "nPort_N_ON_Time", 30000, 5000, 60000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nPort_N_ON_Time = nTmpValue;
			}
			else
			{
				//bSuccess = false;
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nPort_N_ON_Time = 30000;
			}

			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("STBPIO", "nPort_N_OFF_Time", 30000, 5000, 60000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nPort_N_OFF_Time = nTmpValue;
			}
			else
			{
				//bSuccess = false;
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nPort_N_OFF_Time = 30000;
			}

			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("STBPIO","nReady_ON_Time", READY_ONTIME, 5000, 60000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nReady_ON_Time = nTmpValue;
			}
			else
			{
				//bSuccess = false;
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nReady_ON_Time = 10000;
			}

			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("STBPIO","nReady_OFF_Time", READY_OFFTIME, 5000, 60000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nReady_OFF_Time = nTmpValue;
			}
			else
			{
				//bSuccess = false;
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nReady_OFF_Time = 10000;
			}

			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("STBPIO","nHOAVBL_ON_Time", HO_AVBL_ONTIME, 5000, 100000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nHOAVBL_ON_Time = nTmpValue;
			}
			else
			{
				//bSuccess = false;
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nHOAVBL_ON_Time = 10000;
			}

			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("STBPIO","nES_ON_Time", ES_ONTIME, 1000, 100000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nES_ON_Time = nTmpValue;
			}
			else
			{
				//bSuccess = false;
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nES_ON_Time = 10000;
			}

			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("STBPIO","nCS_ON_Delay_Time", EQPIO_CS_ON_TIME, 0, 500, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nCS_ON_Delay_Time = nTmpValue;
			}
			else
			{
				//bSuccess = false;
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nCS_ON_Delay_Time = 200;
			}

			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("STBPIO","nCS_Resp_Delay_Time", 20000, 0, 60000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nCS_Resp_Delay_Time = nTmpValue;
			}
			else
			{
				//bSuccess = false;
				pParamSet->m_TIMEOVER_DUALSTAGEPIO.nCS_Resp_Delay_Time = 10000;
			}
		}
//
//		if(bSuccess)
//		{
//			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("PIO", "CSONDelayTime", EQPIO_CS_ON_TIME, 0, 200, &nTmpValue);
//			if(bCheckParam)
//			{
//				pParamSet->m_nPIOCSONDelayTime = nTmpValue;
//			}
//			else
//			{
//				bSuccess = false;
//			}
//
//		}

//		PIO_LOG("[EQ_PIO_INI Load]%d",pParamSet->m_nPIOCSONDelayTime);

		delete pIniFile;

	}
	else
	{
		bSuccess = false;

	}

	//AOHC-358 파라미터 적용 오류 시, 에러 발생

	if(bSuccess ==false)
	{
	  //	SOM_LOG ("loadparameterRecovery");
		bCheckEQParam = true;
		bCheckParam = LoadEQPIOParam("DataFiles\\EQPIO.ini", pParamSet);
		if(bCheckParam == false)
			OHTMainForm->m_CheckParamResult = ERR_PARADATA_LOAD_FAIL;
		else
			OHTMainForm->m_CheckParamResult = NO_ERR;

	}


	return bSuccess;
}




/**
@brief   ini 파일에서 EQ PIO Timeover 관련된 Data를 Load 함.
@author  LSJ
@date    2012.11.07
@return  Unload 명령을 문제 없이 수행하면 0, 문제가 발생하면 해당 에러코드를 리턴함.
*/
bool EQPIO::LoadEQPIOParam(char* szPath, EQPIO_PARAM  *pParamSet)
{
//    int nError = 0;
	bool bSuccess = false;
    int MaxIndex = 0;
	String strOHTPath = "", strIniFileName = "";
	char strIndex[255] = {0, };
	char strTempValue[8]= {0,};

 //	OHTMainForm->m_CheckParamResult =  NO_ERR;

	LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile(szPath);

	if(!pIniFile->GetIniFileExist())
	{
		delete pIniFile;
		pIniFile = NULL;
		bSuccess = false;
	}

	if(pIniFile == NULL || bCheckEQParam == true)
	{
		strcpy(pParamSet->Version, "00");
		pParamSet->nMaxIndex = 10;
//		pParamSet->m_nPIOCSONDelayTime = EQPIO_CS_ON_TIME;
        pParamSet->nChatteringCount = 0;
		pParamSet->nPSTB_GO_OFF_Time = 30000;

		for (int i = 0; i < 10; i++)
		{
			pParamSet->m_TIMEOVER_EQPIO[i].nLREQ_ON_Time = L_REQ_WAITTIME;
			pParamSet->m_TIMEOVER_EQPIO[i].nLREQ_OFF_Time = L_REQ_OFF_TIMEOVER;
			pParamSet->m_TIMEOVER_EQPIO[i].nUREQ_ON_Time = U_REQ_WAITTIME;
			pParamSet->m_TIMEOVER_EQPIO[i].nUREQ_OFF_Time = U_REQ_OFFTIME;
			pParamSet->m_TIMEOVER_EQPIO[i].nReady_ON_Time = READY_ONTIME;
			pParamSet->m_TIMEOVER_EQPIO[i].nReady_OFF_Time = READY_OFFTIME;
			pParamSet->m_TIMEOVER_EQPIO[i].nHOAVBL_ON_Time = HO_AVBL_ONTIME;
			pParamSet->m_TIMEOVER_EQPIO[i].nES_ON_Time = ES_ONTIME;
			pParamSet->m_TIMEOVER_EQPIO[i].nCS_ON_Delay_Time = EQPIO_CS_ON_TIME;
		}

		bSuccess = false;
		bCheckEQParam = false;
		PIO_LOG("[EQ_PIO_TIMEOVER_INI_FILE");
		ADD_MD_LOG("EQ PIO Param Loading Fail");
	  return bSuccess;

	}

	else if(pIniFile != NULL)
	{

		// PIO time
        pIniFile->ReadString("Version","IniVersion", "00", pParamSet->Version);

		pIniFile->ReadInteger("PIO", "nMaxIndex", 10, &pParamSet->nMaxIndex);

		pIniFile->ReadInteger("PIO", "nChatteringCount", 0, &pParamSet->nChatteringCount);

		pIniFile->ReadInteger("PSTB", "nPSTB_GO_OFF_Time", PSTB_GO_OFF_DEFAULT_TIME, &pParamSet->nPSTB_GO_OFF_Time);

		for (int j = 0; j < pParamSet->nMaxIndex; j++)
		{
			sprintf(strIndex, "PIO%d", j+1);

			pIniFile->ReadInteger(strIndex, "nLREQ_ON_Time", L_REQ_WAITTIME, &pParamSet->m_TIMEOVER_EQPIO[j].nLREQ_ON_Time);
			pIniFile->ReadInteger(strIndex, "nLREQ_OFF_Time", L_REQ_OFF_TIMEOVER, &pParamSet->m_TIMEOVER_EQPIO[j].nLREQ_OFF_Time);
			pIniFile->ReadInteger(strIndex, "nUREQ_ON_Time", U_REQ_WAITTIME, &pParamSet->m_TIMEOVER_EQPIO[j].nUREQ_ON_Time);
			pIniFile->ReadInteger(strIndex, "nUREQ_OFF_Time", U_REQ_OFFTIME, &pParamSet->m_TIMEOVER_EQPIO[j].nUREQ_OFF_Time);
			pIniFile->ReadInteger(strIndex, "nReady_ON_Time", READY_ONTIME, &pParamSet->m_TIMEOVER_EQPIO[j].nReady_ON_Time);
			pIniFile->ReadInteger(strIndex, "nReady_OFF_Time", READY_OFFTIME, &pParamSet->m_TIMEOVER_EQPIO[j].nReady_OFF_Time);
			pIniFile->ReadInteger(strIndex, "nHOAVBL_ON_Time", HO_AVBL_ONTIME, &pParamSet->m_TIMEOVER_EQPIO[j].nHOAVBL_ON_Time);
			pIniFile->ReadInteger(strIndex, "nES_ON_Time", ES_ONTIME, &pParamSet->m_TIMEOVER_EQPIO[j].nES_ON_Time);
			pIniFile->ReadInteger(strIndex, "nCS_ON_Delay_Time", EQPIO_CS_ON_TIME, &pParamSet->m_TIMEOVER_EQPIO[j].nCS_ON_Delay_Time);

			//				PIO_LOG("[nLREQ_ON_Time %d : %d",j,m_TIMEOVER_EQPIO[j].nLREQ_ON_Time);

		}

//		pIniFile->ReadInteger("PIO", "CSONDelayTime", EQPIO_CS_ON_TIME, &pParamSet->m_nPIOCSONDelayTime);
//
//		PIO_LOG("[EQ_PIO_INI Load]%d",pParamSet->m_nPIOCSONDelayTime);

		delete pIniFile;
		bSuccess = true;
	}

	return bSuccess;
}

/**
@brief   EQPIO의 출력 초기화 및 연결 초기화
@author  LSJ
@date    2012.10.29
@return	 정상인 경우 0, 비정상인 경우 ErrorCode Return
*/
int EQPIO::Init()
{
    int nError = NO_ERR;

    nError = AllPIOOff();

#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S)|| (OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME== OHT_NAME_P4_MASK))
    if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_EQPIO_LEFT_SEL, ON);

#if((OHT_NAME != OHT_NAME_STD_V85) && (OHT_NAME != OHT_NAME_STD_V81S)&& (OHT_NAME != OHT_NAME_STD_V81)&& (OHT_NAME != OHT_NAME_STD_V85S)&&(OHT_NAME != OHT_NAME_STD_V82)&&(OHT_NAME != OHT_NAME_STD_V86)&&(OHT_NAME != OHT_NAME_P4_MASK))
    if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_EQPIO_RIGHT_SEL, ON);
#endif
    Init_SequenceForLoading();
	Init_SequenceForUnloading();
	Init_SequenceForMoving();
    Init_SequenceForRFPIOConnection();

	// Commport 입력
	m_nPortNum = COM_RF_PIO;
	m_pCommPort = NULL;

	// RF PIO Commport Open
    // 생성자 호출 시 Comport Open으로 EQ PIO Type 구분 없이 Open 수행 필요.
	nError = RFPIO_Open();

#else
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_EQPIO_LEFT_SEL, ON);
    if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_EQPIO_RIGHT_SEL, ON);

	Init_SequenceForLoading();
	Init_SequenceForUnloading();
	Init_SequenceForMoving();
#endif
	m_mccparam.nEqType = 0;
	m_mccparam.dwPIOSelectOnTime = 0;
	m_mccparam.dwPIOInitTime = 0;
	m_mccparam.dwHOAVBLOnTime = 0;
	m_mccparam.dwESOnTime = 0;
	m_mccparam.dwCS0OnTime = 0;
	m_mccparam.dwValidOnTime = 0;
	m_mccparam.dwLReqOnTime = 0;
	m_mccparam.dwUReqOnTime = 0;
	m_mccparam.dwTRReqOnTime = 0;
	m_mccparam.dwReadyOnTime = 0;
	m_mccparam.dwBusyOnTime = 0;
	m_mccparam.dwBusyOffTime = 0;
	m_mccparam.dwTRReqOffTime = 0;
	m_mccparam.dwCompleteOnTime = 0;
	m_mccparam.dwReadyOffTime = 0;
	m_mccparam.dwOutputOffTime = 0;

    return nError;
}

/**
@brief   EQPIO의 출력 초기화
@author  LSJ
@date    2012.10.29
@return	 정상인 경우 0, 비정상인 경우 ErrorCode Return
*/
int EQPIO::AllPIOOff()
{
    int nError = NO_ERR;

    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_EQPIO_VALID, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_1, OFF);
#if ((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V81S) ||(OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME== OHT_NAME_P4_MASK))
    if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_2, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_3, OFF);
#endif
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_EQPIO_TR_REQ, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_EQPIO_BUSY, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_EQPIO_COMPLETE, OFF);

    return nError;
}

/**
@brief	 설비 PIO와의 통신을 위해 Enable 시키는 함수
@author	 LSJ
@date	 2012.10.29
@param   EQ PIO 선택(LEFT or RIGHT)
@return	 정상인 경우 0, 비정상인 경우 ErrorCode Return
*/
int EQPIO::PIOEnable()
{
    int nError = NO_ERR;

	if (m_nSide == LEFT_SIDE)
    {
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_EQPIO_LEFT_SEL, OFF);

#if((OHT_NAME != OHT_NAME_STD_V85) && (OHT_NAME != OHT_NAME_STD_V81S)&& (OHT_NAME != OHT_NAME_STD_V81)&& (OHT_NAME != OHT_NAME_STD_V85S)&&(OHT_NAME != OHT_NAME_STD_V82)&&(OHT_NAME != OHT_NAME_STD_V86)&&(OHT_NAME != OHT_NAME_P4_MASK))
        if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_EQPIO_RIGHT_SEL, ON);
#endif
    }
    else
	{
#if((OHT_NAME != OHT_NAME_STD_V85) && (OHT_NAME != OHT_NAME_STD_V81S)&& (OHT_NAME != OHT_NAME_STD_V81)&& (OHT_NAME != OHT_NAME_STD_V85S)&&(OHT_NAME != OHT_NAME_STD_V82)&&(OHT_NAME != OHT_NAME_STD_V86)&&(OHT_NAME != OHT_NAME_P4_MASK))
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_EQPIO_RIGHT_SEL, OFF);
#endif
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_LEFT_SEL, ON);
    }

    return nError;
}

/**
@brief	 설비 PIO와의 통신을 끊기 위해 Disable 시키는 함수
@author	 LSJ
@date	 2012.10.29
@param   EQ PIO 선택(LEFT or RIGHT)
@param	 출력 모두 Off 시킬지 선택(1 : 클리어, 0 : 상태 유지)
@return	 정상인 경우 0, 비정상인 경우 ErrorCode Return
*/
int EQPIO::PIODisable(bool bOutClear)
{
    int nError = NO_ERR;

    if (bOutClear == true)
    {
		if (m_nSide == LEFT_SIDE)
        {
            if (m_pHWCommon->m_Output.EQPIO_Left_Select == ON)
                nError = m_pHWCommon->SetBit(OUT_EQPIO_LEFT_SEL, OFF);
        }
        else
		{
#if((OHT_NAME != OHT_NAME_STD_V85) && (OHT_NAME != OHT_NAME_STD_V81S)&& (OHT_NAME != OHT_NAME_STD_V81)&& (OHT_NAME != OHT_NAME_STD_V85S)&&(OHT_NAME != OHT_NAME_STD_V82)&&(OHT_NAME != OHT_NAME_STD_V86)&&(OHT_NAME != OHT_NAME_P4_MASK))
			if (m_pHWCommon->m_Output.EQPIO_Right_Select == ON)
				nError = m_pHWCommon->SetBit(OUT_EQPIO_RIGHT_SEL, OFF);
#endif
        }
		if (nError == NO_ERR)
			nError = AllPIOOff();
    }
    else
    {
        if ((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Valid == ON))
            nError = m_pHWCommon->SetBit(OUT_EQPIO_VALID, OFF);
        if ((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_CS_0 == ON))
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, OFF);
        if ((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_CS_1 == ON))
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_1, OFF);

        if ((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_TR_Req == ON))
            nError = m_pHWCommon->SetBit(OUT_EQPIO_TR_REQ, OFF);
        if ((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Busy == ON))
            nError = m_pHWCommon->SetBit(OUT_EQPIO_BUSY, OFF);
        if ((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Complete == ON))
            nError = m_pHWCommon->SetBit(OUT_EQPIO_COMPLETE, OFF);
        Sleep(50);
	}

	Init_SequenceForLoading();
	Init_SequenceForUnloading();
	Init_SequenceForMoving();
#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81)||  (OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE)||(OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
    Init_SequenceForRFPIOConnection();
#endif

	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_EQPIO_LEFT_SEL, ON);
#if((OHT_NAME != OHT_NAME_STD_V85) && (OHT_NAME != OHT_NAME_STD_V81S) && (OHT_NAME != OHT_NAME_STD_V81)&& (OHT_NAME != OHT_NAME_STD_V85S)&&(OHT_NAME != OHT_NAME_STD_V82)&&(OHT_NAME != OHT_NAME_STD_V86)&&(OHT_NAME != OHT_NAME_P4_MASK))
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_EQPIO_RIGHT_SEL, ON);
#endif

    return nError;
}
//---------------------------------------------------------------------------

/**
@brief   Go Off time check
@author  KKS
@date    2014.08.28
@param   time out
@return  Go signal 이상 없으면 0, 문제 발생 시 ErrorCode 리턴
*/
int EQPIO::Check_GoOffTime(int nPeriodTime, int nStep)
{
	int nError = RTN_OK;

	static bool WaitTimeCheckFlag = false;
	static DWORD m_dwGOAbnormalOffWaitTime = timeGetTime();

	if(IsGoSignalOn() == false&&(IsValidOn() == false)&&(IsCS0On() == false))
	{
		if(WaitTimeCheckFlag == false)
		{
			m_dwGOAbnormalOffWaitTime = timeGetTime();
			WaitTimeCheckFlag = true;
		}

		if(WaitTimeCheckFlag == true && OHTUTILITY::PreventOverflowGetTimeMs(m_dwGOAbnormalOffWaitTime) > nPeriodTime)
		{
			WaitTimeCheckFlag = false;
			PIO_LOG("ERR_PIO_GO_OFF, Step(%d), E(%d), Time(%d)", nStep,	nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwGOAbnormalOffWaitTime));
			return RTN_FAIL;
		}
	}
	else
	{

		WaitTimeCheckFlag = false;
	}

	return nError;
}


/**
@brief   Loading 작업 시 Hoist와 Shift 축을 움직이기 전에 PIO 시퀀스 구현
@author  LSJ
@date    2012.11.08
@param   PIOTimeoverLevel : PIO 통신 시 TimeOver 값을 얼마로 설정할지 결정하는 변수
@return  PIO 통신에 이상 없으면 0, 문제 발생 시 ErrorCode 리턴
*/
int EQPIO::SequenceBeforeLoad(int PIOTimeoverLevel,int nEQType)
{
	int nError = 0;
    DWORD dwPIOWaitTime = 0;
	Update_MCC();
	m_mccparam.nEqType = nEQType;

	switch (m_nBeforeLoadStep)
	{
	case 1: // Go Signal On Check
		if(m_mccparam.nPIOStep < 1)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK GO ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if(nError == NO_ERR)
		{
			if(m_nSide == LEFT_SIDE)
			{
				if(m_pHWCommon->m_Output.EQPIO_Left_Select == OFF)
				{
					if(Check_GoOffTime(EQPIO_GO_ABNORMAL_OFF_CHECK_TIME, m_nBeforeLoadStep)==RTN_FAIL)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1;
			            PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1_1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
						ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
					}
				}
				else
				{
					nError = PIOEnable();
					ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,PIO BEFORE LOAD,Start,%s,%06d,%d,%06d,%d",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset);
					m_mccparam.dwPIOSelectOnTime = timeGetTime();
					Sleep(100);
					if(nError == NO_ERR)
					{
						if(Check_GoOffTime(EQPIO_GO_ABNORMAL_OFF_CHECK_TIME, m_nBeforeLoadStep)==RTN_FAIL)
						{
							nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1;
				            PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1_2, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
							ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
						}
					}
				}
			}
			else
			{
#if((OHT_NAME != OHT_NAME_STD_V85) && (OHT_NAME != OHT_NAME_STD_V81S) && (OHT_NAME != OHT_NAME_STD_V81)&& (OHT_NAME != OHT_NAME_STD_V85S)&&(OHT_NAME != OHT_NAME_STD_V82)&&(OHT_NAME != OHT_NAME_STD_V86)&&(OHT_NAME != OHT_NAME_P4_MASK))
				if(m_pHWCommon->m_Output.EQPIO_Right_Select == OFF)
				{
					if(Check_GoOffTime(EQPIO_GO_ABNORMAL_OFF_CHECK_TIME, m_nBeforeLoadStep)==RTN_FAIL)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1;
						PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1_3, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
						ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
					}
				}
				else
				{
					nError = PIOEnable();
					ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,PIO BEFORE LOAD,Start,%s,%06d,%d,%06d,%d",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset);
					m_mccparam.dwPIOSelectOnTime = timeGetTime();
					Sleep(100);
					if(Check_GoOffTime(EQPIO_GO_ABNORMAL_OFF_CHECK_TIME, m_nBeforeLoadStep)==RTN_FAIL)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1;
			            PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1_4, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
                    	ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
					}
				}
#else
				nError = PIOEnable();
				Sleep(100);
				if(Check_GoOffTime(EQPIO_GO_ABNORMAL_OFF_CHECK_TIME, m_nBeforeLoadStep)==RTN_FAIL)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1;
					PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1_4, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
				}
#endif
			}
		}
		if((nError == NO_ERR) && (IsGoSignalOn() == true))
		{
			m_nBeforeLoadStep = 2;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
			if(m_mccparam.nPIOStep < 2)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK GO ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

	case 2: // Signal Off
		if(m_mccparam.nPIOStep < 3)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO INIT,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
        nError = ValidOnOff(OFF);
		if(nError == NO_ERR)	nError = CS0OnOff(OFF);
        if(nError == NO_ERR)	nError = CS1OnOff(OFF);
		if(nError == NO_ERR)	nError = CS2OnOff(OFF);
		if(nError == NO_ERR)	nError = TRREQOnOff(OFF);
        if(nError == NO_ERR)    nError = BusyOnOff(OFF);
		if(nError == NO_ERR)	nError = CompleteOnOff(OFF);
		if(nError == NO_ERR)
		{
			m_dwLoadPIOWaitTime = timeGetTime();
			m_nBeforeLoadStep = 3;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
			m_mccparam.dwPIOInitTime = timeGetTime();
			if(m_mccparam.nPIOStep < 4)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO INIT,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

	case 3:
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 50)
		{
			m_dwLoadPIOWaitTime = 0;
			m_nBeforeLoadStep = 4;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
		}
		break;

	case 4:
        if((nError==NO_ERR) && (IsLoadREQOn() == true))
       	{
       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
	        if(IsLoadREQOn() == true)
        	{
				nError = ERR_PIO_LREQ_ABNORMAL_ON_TD1;
	            PIO_LOG("ERR_PIO_LREQ_ABNORMAL_ON_TD1_1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
							ERR_PIO_LREQ_ABNORMAL_ON_TD1,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nBeforeLoadStep);
        	}
       	}

		if((nError==NO_ERR) && (IsUnloadREQOn() == true))
       	{
       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
	        if(IsUnloadREQOn() == true)
        	{
				nError = ERR_PIO_UREQ_ABNORMAL_ON_TD1;
	            PIO_LOG("ERR_PIO_UREQ_ABNORMAL_ON_TD1_1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
							ERR_PIO_UREQ_ABNORMAL_ON_TD1,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nBeforeLoadStep);
			}
       	}

		if((nError==NO_ERR) && (IsReadyOn() == true))
		{
       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
			if(IsReadyOn() == true)
			{
				nError = ERR_PIO_READY_ABNORMAL_ON_TD1;
	            PIO_LOG("ERR_PIO_READY_ABNORMAL_ON_TD1_1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
							ERR_PIO_READY_ABNORMAL_ON_TD1,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nBeforeLoadStep);
			}
		}

		if(nError == NO_ERR)
		{
			m_dwLoadPIOWaitTime = timeGetTime();
			m_nBeforeLoadStep = 5;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
		}
		break;

	case 5:
		if(m_mccparam.nPIOStep < 5)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK HOAVBL ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if((nError==NO_ERR) && (IsHOAVBLOn() == true))
		{
			m_dwLoadPIOWaitTime = timeGetTime();
			m_nBeforeLoadStep = 6;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
			m_mccparam.dwHOAVBLOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 6)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK HOAVBL ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		else
		{
			if (OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nHOAVBL_ON_Time)
			{
				nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1;
	            PIO_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1_1, SequenceBeforeLoad, Step(%d), E(%d), Time(%d)", m_nBeforeLoadStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
				m_dwLoadPIOWaitTime = 0;
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
			}
		}
		break;

	case 6:
		if(m_mccparam.nPIOStep < 7)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK ES ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if((nError==NO_ERR) && (IsESOn() == true))
		{
			m_nBeforeLoadStep = 7;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
			m_mccparam.dwESOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 8)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK ES ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nES_ON_Time)
			{
				nError = ERR_PIO_ES_ABNORMAL_OFF_TD1;
	            PIO_LOG("ERR_PIO_ES_ABNORMAL_OFF_TD1_1, SequenceBeforeLoad, Step(%d), E(%d), Time(%d)", m_nBeforeLoadStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
				m_dwLoadPIOWaitTime = 0;
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
			}

			if((nError==NO_ERR) && (IsHOAVBLOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsHOAVBLOn() == false)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1;
		            PIO_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1_2, SequenceBeforeLoad, Step(%d), E(%d), Time(%d)", m_nBeforeLoadStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
				}
			}
		}
		break;

	case 7:
		if(m_mccparam.nPIOStep < 9)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CS ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if((nEQType == PORT_TYPE_STB_L_B_S) || (nEQType == PORT_TYPE_STB_R_B_S))
		{
			nError = CSBitOnOff(ON);
		}
		else
		{
			nError = CS0OnOff(ON);
		}
		if(nError == NO_ERR)
		{
			m_dwLoadPIOWaitTime = timeGetTime();
			m_nBeforeLoadStep = 8;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
			m_mccparam.dwCS0OnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 10)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CS ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

	case 8:
		if (OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nCS_ON_Delay_Time)
		{
			m_dwLoadPIOWaitTime = timeGetTime();
			m_nBeforeLoadStep = 9;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
		}
		break;
	case 9:
		if(m_mccparam.nPIOStep < 11)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO VALID ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		nError = ValidOnOff(ON);
		if(nError == NO_ERR)
		{
			m_dwLoadPIOWaitTime = timeGetTime();
			m_nBeforeLoadStep = 10;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
			m_mccparam.dwValidOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 12)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO VALID ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

	case 10:
		if(m_mccparam.nPIOStep < 13)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK LD REQ ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if((nError == NO_ERR) && (IsLoadREQOn() == true))
		{
			m_nBeforeLoadStep = 11;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
			m_mccparam.dwLReqOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 14)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK LD REQ ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nLREQ_ON_Time)
			{
				nError = ERR_PIO_LREQ_ABNORMAL_OFF_TA1;
	            PIO_LOG("ERR_PIO_LREQ_ABNORMAL_OFF_TA1, SequenceBeforeLoad, Step(%d), E(%d), Time(%d)", m_nBeforeLoadStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
				m_dwLoadPIOWaitTime = 0;
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_LREQ_ABNORMAL_OFF_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
			}

			if((nError == NO_ERR) && (IsUnloadREQOn() == true))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsUnloadREQOn() == true)
				{
					nError = ERR_PIO_UREQ_ABNORMAL_ON_TA1;
		            PIO_LOG("ERR_PIO_UREQ_ABNORMAL_ON_TA1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_UREQ_ABNORMAL_ON_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
				}
			}

			if((nError == NO_ERR) && (IsReadyOn() == true))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsReadyOn() == true)
				{
					nError = ERR_PIO_READY_ABNORMAL_ON_TA1;
		            PIO_LOG("ERR_PIO_READY_ABNORMAL_ON_TA1_1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_READY_ABNORMAL_ON_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
				}
			}

			if((nError == NO_ERR) && (IsGoSignalOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsGoSignalOn() == false)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA1;
		            PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA1_1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
                }
			}

			if((nError == NO_ERR) && (IsESOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsESOn() == false)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_TA1;
		            PIO_LOG("ERR_PIO_ES_ABNORMAL_OFF_TA1_1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
                }
			}

			if((nError == NO_ERR) && (IsHOAVBLOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsHOAVBLOn() == false)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TA1;
		            PIO_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TA1_1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
                }
			}
		}
		break;

	case 11:
		if(m_mccparam.nPIOStep < 15)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO TR REQ ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		nError = TRREQOnOff(ON);
		if(nError == NO_ERR)
		{
			m_dwLoadPIOWaitTime = timeGetTime();
			m_nBeforeLoadStep = 12;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
			m_mccparam.dwTRReqOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 16)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO TR REQ ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

	case 12:
		if(m_mccparam.nPIOStep < 17)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK READY ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if((nError == NO_ERR) && (IsReadyOn() == true))
		{
			m_nBeforeLoadStep = 13;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
			m_mccparam.dwReadyOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 18)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK READY ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nReady_ON_Time)
			{
				nError = ERR_PIO_READY_ABNORMAL_OFF_TA2;
	            PIO_LOG("ERR_PIO_READY_ABNORMAL_OFF_TA2_1, SequenceBeforeLoad, Step(%d), E(%d), Time(%d)", m_nBeforeLoadStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
				m_dwLoadPIOWaitTime = 0;
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_READY_ABNORMAL_OFF_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
			}

			if((nError == NO_ERR) && (IsUnloadREQOn() == true))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsUnloadREQOn() == true)
				{
					nError = ERR_PIO_UREQ_ABNORMAL_ON_TA2;
		            PIO_LOG("ERR_PIO_UREQ_ABNORMAL_ON_TA2, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_UREQ_ABNORMAL_ON_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
				}
			}

			if((nError == NO_ERR) && (IsLoadREQOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsLoadREQOn() == false)
				{
					nError = ERR_PIO_LREQ_ABNORMAL_OFF_TA2;
		            PIO_LOG("ERR_PIO_LREQ_ABNORMAL_OFF_TA2, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_LREQ_ABNORMAL_OFF_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
				}
			}

			if((nError == NO_ERR) && (IsGoSignalOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsGoSignalOn() == false)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA2;
		            PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA2_1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
                }
			}

			if((nError == NO_ERR) && (IsESOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsESOn() == false)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_TA2;
		            PIO_LOG("ERR_PIO_ES_ABNORMAL_OFF_TA2_1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
                }
			}

			if((nError == NO_ERR) && (IsHOAVBLOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsHOAVBLOn() == false)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TA2;
		            PIO_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TA2_1, SequenceBeforeLoad, Step(%d), E(%d)", m_nBeforeLoadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeLoadStep);
                }
			}
		}
		break;

	case 13:
		if(m_mccparam.nPIOStep < 19)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO BUSY ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		nError = BusyOnOff(ON);
		if(nError == NO_ERR)
		{
			m_nBeforeLoadStep = 14;
			PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", m_nBeforeLoadStep);
			m_mccparam.dwBusyOnTime = timeGetTime();
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,PIO BEFORE LOAD,End,%s,%06d,%d,%06d,%d,PIO Select On Time,%d,PIO Init Time,%d,HOAVBL On Time,%d,ES On Time,%d,CS0 On Time,%d,Valid On Time,%d,Load Req On Time,%d,TR Req On Time,%d,Ready On Time,%d,Busy On Time,%d",
						m_defualtparam->VHLName,m_mccparam.cCarrierType,
						m_pOHTMainStatus->StatusCommon.CurStationID,
						m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_mccparam.dwPIOSelectOnTime,
						m_mccparam.dwPIOInitTime,
						m_mccparam.dwHOAVBLOnTime,
						m_mccparam.dwESOnTime,
						m_mccparam.dwCS0OnTime,
						m_mccparam.dwValidOnTime,
						m_mccparam.dwLReqOnTime,
						m_mccparam.dwTRReqOnTime,
						m_mccparam.dwReadyOnTime,
						m_mccparam.dwBusyOnTime);
			if(m_mccparam.nPIOStep < 20)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO BUSY ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
			m_mccparam.dwPIOSelectOnTime = 0;
			m_mccparam.dwPIOInitTime = 0;
			m_mccparam.dwHOAVBLOnTime = 0;
			m_mccparam.dwESOnTime = 0;
			m_mccparam.dwCS0OnTime = 0;
			m_mccparam.dwValidOnTime = 0;
			m_mccparam.dwLReqOnTime = 0;
			m_mccparam.dwTRReqOnTime = 0;
			m_mccparam.dwReadyOnTime = 0;
			m_mccparam.dwBusyOnTime = 0;
			m_mccparam.nPIOStep = 0;
		}
		break;
	}

	if(nError)
	{
		m_nBeforeLoadStep = 0;
		m_dwLoadPIOWaitTime = 0;
	}

	return nError;
}

int EQPIO::SequenceBeforeTransForDualStageStb(bool bLoadUnload) //bLoadUnload = 0: Load, 1: Unload
{
	int nError = 0;
	DWORD dwPIOWaitTime = 0;
	int *pTransStep;
	BYTE EQPIO_Select;
	if(bLoadUnload == PIO_LOAD) pTransStep = &m_nBeforeLoadStep;
	else if(bLoadUnload == PIO_UNLOAD) pTransStep = &m_nBeforeUnloadStep;
	switch(*pTransStep)
	{
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_GO_SIG:
		{
			if(nError == NO_ERR)
			{
				if(m_nSide == LEFT_SIDE) EQPIO_Select = m_pHWCommon->m_Output.EQPIO_Left_Select;
#if((OHT_NAME != OHT_NAME_STD_V85) && (OHT_NAME != OHT_NAME_STD_V81S) && (OHT_NAME != OHT_NAME_STD_V81))
				else EQPIO_Select = m_pHWCommon->m_Output.EQPIO_Right_Select;
#endif

				if(EQPIO_Select == OFF)
				{
					if(Check_GoOffTime(EQPIO_GO_ABNORMAL_OFF_CHECK_TIME, *pTransStep)==RTN_FAIL)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1;
			            		PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1_1, SequenceBeforeLoadForDualStageStb, Step(%d), E(%d)", *pTransStep, nError);
					}
				}
				else
				{
					nError = PIOEnable();
					Sleep(100);
					if(nError == NO_ERR)
					{
						if(Check_GoOffTime(EQPIO_GO_ABNORMAL_OFF_CHECK_TIME, *pTransStep)==RTN_FAIL)
						{
							nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1;
				            		PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1_2, SequenceBeforeLoadForDualStageStb, Step(%d), E(%d)", *pTransStep, nError);
						}
					}
				}
			}

			if((nError == NO_ERR) && (IsGoSignalOn() == true))
			{
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_SIG_OFF;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_SIG_OFF");

			}

		}
		break;
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_SIG_OFF:
		{
			nError = ValidOnOff(OFF);
			if(nError == NO_ERR)	nError = CS0OnOff(OFF);
			if(nError == NO_ERR)	nError = CS1OnOff(OFF);
			if(nError == NO_ERR)	nError = CS2OnOff(OFF);
			if(nError == NO_ERR)	nError = TRREQOnOff(OFF);
			if(nError == NO_ERR)	nError = BusyOnOff(OFF);
			if(nError == NO_ERR)	nError = CompleteOnOff(OFF);
			if(nError == NO_ERR)
			{
				m_dwLoadPIOWaitTime = timeGetTime();
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_SIG_OFF_DELAY;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_SIG_OFF_DELAY");
			}
			//PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_SIG_OFF, errer:", nError);
		}
		break;
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_SIG_OFF_DELAY:
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 50)
			{
				m_dwLoadPIOWaitTime = 0;
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_HO_AVBL;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_HO_AVBL");
			}

		}
		break;
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_HO_AVBL:
		{
			//if((nError==NO_ERR) && (IsHOAVBLOn() == true))
			if((nError==NO_ERR) && (IsCS0On() == true))    // PIO Input3
			{
				m_dwLoadPIOWaitTime = timeGetTime();
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_HO_ES;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_HO_ES");
			}
			else
			{
				if (OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_DUALSTAGEPIO.nHOAVBL_ON_Time)
				{
					nError = ERR_DUAL_STAGE_STB_HOAVBL_OFF_TIMEOUT;
					PIO_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1_1, SequenceBeforeLoadForDualStageStb, Step(%d), E(%d), Time(%d)", *pTransStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
					m_dwLoadPIOWaitTime = 0;
				}
			}

		}
		break;
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_HO_ES:
		{
			//if((nError==NO_ERR) && (IsESOn() == true))
			if((nError==NO_ERR) && (IsHOAVBLOn() == true))  // PIO Input7
			{
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_CS_ON;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_CS_ON");
			}
			else
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_DUALSTAGEPIO.nES_ON_Time)
				{
					nError = ERR_DUAL_STAGE_STB_ES_ABNORMAL_OFF;
					PIO_LOG("ERR_PIO_ES_ABNORMAL_OFF_TD1_1, SequenceBeforeLoadForDualStageStb, Step(%d), E(%d), Time(%d)", *pTransStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
					m_dwLoadPIOWaitTime = 0;
				}

				//if((nError==NO_ERR) && (IsHOAVBLOn() == false))
				if((nError==NO_ERR) && (IsCS0On() == false))
				{
		       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
					//if(IsHOAVBLOn() == false)
					if(IsCS0On() == false)
					{
						nError = ERR_DUAL_STAGE_STB_HOAVBL_ABNORMAL_OFF;
						PIO_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1_2, SequenceBeforeLoadForDualStageStb, Step(%d), E(%d), Time(%d)", *pTransStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
					}
				}
			}

		}
		break;
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_CS_ON:
		{
		//	if(m_nCmdCS == 0) error
//			if(m_nCmdCS == 1){nError = CS0OnOff(ON);}
//			else if(m_nCmdCS == 2){nError = CS1OnOff(ON);}
//			else if(m_nCmdCS == 3){nError = CS1OnOff(ON); nError = CS0OnOff(ON);}
//			else if(m_nCmdCS == 4){nError = CS2OnOff(ON);}
//			else if(m_nCmdCS == 5){nError = CS2OnOff(ON); nError = CS0OnOff(ON);}
//			else if(m_nCmdCS == 6){nError = CS2OnOff(ON); nError = CS1OnOff(ON);}
//			else if(m_nCmdCS == 7){nError = CS2OnOff(ON); nError = CS1OnOff(ON); nError = CS0OnOff(ON);}
			if(m_nCmdCS == 1){nError = CS2OnOff(ON);}
			else if(m_nCmdCS == 2){nError = ValidOnOff(ON);}
			else if(m_nCmdCS == 3){nError = ValidOnOff(ON); nError = CS2OnOff(ON);}
			else if(m_nCmdCS == 4){nError = CS0OnOff(ON);}
			else if(m_nCmdCS == 5){nError = CS0OnOff(ON); nError = CS2OnOff(ON);}
			else if(m_nCmdCS == 6){nError = CS0OnOff(ON); nError = ValidOnOff(ON);}
			else if(m_nCmdCS == 7){nError = CS0OnOff(ON); nError = ValidOnOff(ON); nError = CS2OnOff(ON);}

			if(nError == NO_ERR)
			{
				m_dwLoadPIOWaitTime = timeGetTime();
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_CS_ON_DELAY;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_CS_ON, Port num:%d, CS:%d", m_nCmdCS, m_nCmdCS);
			}
		}
		break;
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_CS_ON_DELAY:
		{
			if (OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_DUALSTAGEPIO.nCS_ON_Delay_Time)
			{
				m_dwLoadPIOWaitTime = timeGetTime();
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_VALID_ON;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_CS_ON_DELAY");
			}
		}
		break;
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_VALID_ON:
		{
			//nError = ValidOnOff(ON);
			nError = CS1OnOff(ON);
			if(nError == NO_ERR)
			{
				m_dwLoadPIOWaitTime = timeGetTime();
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_CS;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_CS");
			}
		}
		break;
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_CS:
		{
			bool bOnOff = false;
			UINT nCSEcho = 0;
			//nCSEcho = (IsCS0On()) | (IsCS1On() << 1) | (IsCS2On() << 2);
			nCSEcho = (IsESOn()) | (IsReadyOn() << 1) | (IsCS1On() << 2);
			if( nCSEcho ==  m_nCmdCS) bOnOff = true;

			if((nError == NO_ERR) && (bOnOff == true))
			{
				PIO_LOG("CS Echo received: %d", nCSEcho);
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_PORT_NP;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_PORT_NP");
				m_dwLoadPIOWaitTime = timeGetTime();
			}
			else
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_DUALSTAGEPIO.nCS_Resp_Delay_Time)
				{
					nError = ERR_DUAL_STAGE_STB_CS_MISMATCH;
					PIO_LOG("ERR_DUAL_STAGE_STB_CS_MISMATCH, Time(%d), nCS_Resp_Delay_Time(%d)", OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime), m_pParamSet->m_TIMEOVER_DUALSTAGEPIO.nCS_Resp_Delay_Time);
                    		PIO_LOG("CS Echo received: %d", nCSEcho);
					m_dwLoadPIOWaitTime = 0;
				}
			}
		}
		break;
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_PORT_NP:
		{
			if((nError == NO_ERR) && (IsUnloadREQOn() == true) && (bLoadUnload == PIO_LOAD)) // LoadReq = PortN
			{
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_TR_REQ_ON;//DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_READY;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_TR_REQ_ON");
			}
			else if((nError == NO_ERR) && (IsLoadREQOn() == true) && (bLoadUnload == PIO_UNLOAD)) // UnloadReq = PortP
			{
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_TR_REQ_ON;//DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_READY;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_TR_REQ_ON");
			}
			else
			{
				m_pParamSet->m_TIMEOVER_DUALSTAGEPIO.nPort_N_ON_Time = 10000;
				if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_DUALSTAGEPIO.nPort_N_ON_Time)
				{
					nError = ERR_PIO_LREQ_ABNORMAL_OFF_TA1;
					PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_PORT_NP, Step(%d), E(%d), Time(%d)", *pTransStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
					m_dwLoadPIOWaitTime = 0;
					if(bLoadUnload == PIO_LOAD)
					{
						PIO_LOG("PORT_N ON TIMEOUT, TR_REQ: 0, READY: 0");
						nError = ERR_DUAL_STAGE_STB_PORT_N_ON_TIMEOUT;
					}
					else
					{
						PIO_LOG("PORT_P ON TIMEOUT, TR_REQ: 0, READY: 0");
						nError = ERR_DUAL_STAGE_STB_PORT_P_ON_TIMEOUT;
					}
				}
			}
		}
		break;
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_TR_REQ_ON:
		{
			//nError = TRREQOnOff(ON);
			nError = BusyOnOff(ON);
			if(nError == NO_ERR)
			{
				m_dwLoadPIOWaitTime = timeGetTime();
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_READY;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_READY");
			}
		}
		break;

		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_CHECK_READY:
		{
			//if((nError == NO_ERR) && (IsReadyOn() == true))
			if((nError == NO_ERR) && (IsCS2On() == true))
			{
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_BUSY_ON;
				PIO_LOG("DUAL_STAGE_STB_STEP_BEFORE_LOAD_BUSY_ON");
			}
			else
			{
				m_pParamSet->m_TIMEOVER_DUALSTAGEPIO.nReady_ON_Time = 30000;
				if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_DUALSTAGEPIO.nReady_ON_Time)
				{
					nError = ERR_DUAL_STAGE_STB_READY_ON_TIMEOUT;
					PIO_LOG("EERR_DUAL_STAGE_STB_READY_ON_TIMEOUT, Step(%d), E(%d), Time(%d)", *pTransStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
					m_dwLoadPIOWaitTime = 0;
				}
			}

		}
		break;
		case DUAL_STAGE_STB_STEP_BEFORE_LOAD_BUSY_ON:
		{
			//nError = BusyOnOff(ON);
			nError = CompleteOnOff(ON);
			if(nError == NO_ERR)
			{
				*pTransStep = DUAL_STAGE_STB_STEP_BEFORE_LOAD_START;
				PIO_LOG("[EQ_PIO : SequenceBeforeLoad] m_nBeforeLoadStep:%d", *pTransStep);
			}
		}
		break;
	}
	return nError;
}

/**
@brief   Unloading 작업 시 Hoist와 Shift 축을 움직이기 전에 PIO 시퀀스 구현
@author	 LSJ
@date	 2012.11.08
@param   PIOTimeoverLevel : PIO 통신 시 TimeOver 값을 얼마로 설정할지 결정하는 변수
@return  PIO 통신에 이상 없으면 0, 문제 발생 시 ErrorCode 리턴
*/
int EQPIO::SequenceBeforeUnload(int PIOTimeoverLevel,int nEQType)
{
	int nError = 0;

	DWORD dwPIOWaitTime = 0;
	int nHOAVCount = 0;

	Update_MCC();
	m_mccparam.nEqType = nEQType;


	switch (m_nBeforeUnloadStep)
	{
	case 1: // Go Signal On Check
        if(m_mccparam.nPIOStep < 1)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK GO ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if(nError == NO_ERR)
		{
			if(m_nSide == LEFT_SIDE)
			{
				if(m_pHWCommon->m_Output.EQPIO_Left_Select == OFF)
				{
					if(Check_GoOffTime(EQPIO_GO_ABNORMAL_OFF_CHECK_TIME, m_nBeforeLoadStep)==RTN_FAIL)
					{
						nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1;
			            PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1_5, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
						ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
					}
				}
				else
				{
					nError = PIOEnable();
					ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,PIO BEFORE UNLOAD,Start,%s,%06d,%d,%06d,%d",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset);
					m_mccparam.dwPIOSelectOnTime = timeGetTime();
					if(nError == NO_ERR)
					{
						if(Check_GoOffTime(EQPIO_GO_ABNORMAL_OFF_CHECK_TIME, m_nBeforeLoadStep)==RTN_FAIL)
						{
							nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1;
				            PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1_6, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
							ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
						}
					}
				}
   		}
			else
			{
				if(Check_GoOffTime(EQPIO_GO_ABNORMAL_OFF_CHECK_TIME, m_nBeforeLoadStep)==RTN_FAIL)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1;
		            PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1_7, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
					ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
				}
				else
				{
					nError = PIOEnable();
					ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,PIO BEFORE UNLOAD,Start,%s,%06d,%d,%06d,%d",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset);
					m_mccparam.dwPIOSelectOnTime = timeGetTime();
					if(nError == NO_ERR)
					{
						Sleep(100);
						if(Check_GoOffTime(EQPIO_GO_ABNORMAL_OFF_CHECK_TIME, m_nBeforeLoadStep)==RTN_FAIL)
						{
							nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1;
				            PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1_8, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
							ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
						}
					}
				}
			}
		}
		if((nError == NO_ERR) && (IsGoSignalOn() == true))
		{
			m_nBeforeUnloadStep = 2;
			if(m_mccparam.nPIOStep < 2)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK GO ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

	case 2: // Signal Off
		if(m_mccparam.nPIOStep < 3)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO INIT,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
        nError = ValidOnOff(OFF);
		if(nError == NO_ERR)	nError = CS0OnOff(OFF);
        if(nError == NO_ERR)	nError = CS1OnOff(OFF);
		if(nError == NO_ERR)	nError = CS2OnOff(OFF);
		if(nError == NO_ERR)	nError = TRREQOnOff(OFF);
        if(nError == NO_ERR)	nError = BusyOnOff(OFF);
		if(nError == NO_ERR)	nError = CompleteOnOff(OFF);
		if(nError == NO_ERR)
		{
			m_dwUnloadPIOWaitTime = timeGetTime();
			m_nBeforeUnloadStep = 3;
			m_mccparam.dwPIOInitTime = timeGetTime();
			if(m_mccparam.nPIOStep < 4)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO INIT,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

	case 3:
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > 50)
		{
			m_dwUnloadPIOWaitTime = 0;
			m_nBeforeUnloadStep = 4;
		}
		break;

	case 4:
        if((nError == NO_ERR) && IsLoadREQOn() == true)
		{
			Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
			if(IsLoadREQOn() == true)
			{
				nError = ERR_PIO_LREQ_ABNORMAL_ON_TD1;
				PIO_LOG("ERR_PIO_LREQ_ABNORMAL_ON_TD1_2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
				ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
							ERR_PIO_LREQ_ABNORMAL_ON_TD1,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nBeforeUnloadStep);
			}
		}

		if((nError == NO_ERR) && (IsUnloadREQOn() == true))
		{
			Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
			if(IsUnloadREQOn() == true)
			{
				nError = ERR_PIO_UREQ_ABNORMAL_ON_TD1;
				PIO_LOG("ERR_PIO_UREQ_ABNORMAL_ON_TD1_2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
				ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
							ERR_PIO_UREQ_ABNORMAL_ON_TD1,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nBeforeUnloadStep);
			}
		}
		if((nError == NO_ERR) && (IsReadyOn() == true))
		{
			Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
			if(IsReadyOn() == true)
			{
				nError = ERR_PIO_READY_ABNORMAL_ON_TD1;
				PIO_LOG("ERR_PIO_READY_ABNORMAL_ON_TD1_2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
				ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
							ERR_PIO_READY_ABNORMAL_ON_TD1,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							m_nBeforeUnloadStep);
			}
		}
		if(nError == NO_ERR)
		{
			m_dwUnloadPIOWaitTime = timeGetTime();
			m_nBeforeUnloadStep = 5;
		}
		break;

	case 5:
		if(m_mccparam.nPIOStep < 5)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK HOAVBL ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if((nError==NO_ERR) && (IsHOAVBLOn() == true))
		{
			m_dwUnloadPIOWaitTime = timeGetTime();
			m_nBeforeUnloadStep = 6;
			m_mccparam.dwHOAVBLOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 6)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK HOAVBL ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nHOAVBL_ON_Time)
			{
				nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1;
	            PIO_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1_3, SequenceBeforeUnload, Step(%d), E(%d), Time(%d)", m_nBeforeUnloadStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
				m_dwUnloadPIOWaitTime = 0;
				ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
			}
		}
		break;

	case 6:
		if(m_mccparam.nPIOStep < 7)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK ES ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if((nError==NO_ERR) && (IsESOn() == true))
		{
			m_nBeforeUnloadStep = 7;
			m_mccparam.dwESOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 8)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK ES ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nES_ON_Time)
			{
				nError = ERR_PIO_ES_ABNORMAL_OFF_TD1;
	            PIO_LOG("ERR_PIO_ES_ABNORMAL_OFF_TD1_2, SequenceBeforeUnload, Step(%d), E(%d), Time(%d)", m_nBeforeUnloadStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
				m_dwUnloadPIOWaitTime = 0;
				ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
			}

			if((nError==NO_ERR) && (IsHOAVBLOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsHOAVBLOn() == false)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1;
		            PIO_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1_4, SequenceBeforeUnload, Step(%d), E(%d), Time(%d)", m_nBeforeUnloadStep, nError, OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime));
                	ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
				}
			}
		}
		break;

	case 7:
		if(m_mccparam.nPIOStep < 9)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CS ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if((nEQType == PORT_TYPE_STB_L_B_S) || (nEQType == PORT_TYPE_STB_R_B_S))
		{
			nError = CSBitOnOff(ON);
		}
		else
		{
			nError = CS0OnOff(ON);
		}
		if(nError == NO_ERR)
		{
			m_dwUnloadPIOWaitTime = timeGetTime();
			m_nBeforeUnloadStep = 8;
			m_mccparam.dwCS0OnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 10)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CS ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

	case 8:
		if (OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nCS_ON_Delay_Time)
		{
			m_dwUnloadPIOWaitTime = timeGetTime();
			m_nBeforeUnloadStep = 9;
		}
		break;

	case 9:
		if(m_mccparam.nPIOStep < 11)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO VALID ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		nError = ValidOnOff(ON);
		if(nError == NO_ERR)
		{
			m_dwUnloadPIOWaitTime = timeGetTime();
			nHOAVCount = 0;
			m_nBeforeUnloadStep = 10;
			m_mccparam.dwValidOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 12)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO VALID ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

	case 10:
		if(m_mccparam.nPIOStep < 13)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK ULD REQ ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if((nError == NO_ERR) && (IsUnloadREQOn() == true))
		{
			m_nBeforeUnloadStep = 11;
			m_mccparam.dwUReqOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 14)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK ULD REQ ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nUREQ_ON_Time)
			{
				nError = ERR_PIO_UREQ_ABNORMAL_OFF_TA1;
	            PIO_LOG("ERR_PIO_UREQ_ABNORMAL_OFF_TA1, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
				m_dwUnloadPIOWaitTime = 0;
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_UREQ_ABNORMAL_OFF_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
			}

			if((nError == NO_ERR) && (IsLoadREQOn() == true))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsLoadREQOn() == true)
				{
					nError = ERR_PIO_LREQ_ABNORMAL_ON_TA1;
		            PIO_LOG("ERR_PIO_LREQ_ABNORMAL_ON_TA1, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_LREQ_ABNORMAL_ON_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
				}
			}

			if((nError == NO_ERR) && (IsReadyOn() == true))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsReadyOn() == true)
				{
					nError = ERR_PIO_READY_ABNORMAL_ON_TA1;
		            PIO_LOG("ERR_PIO_READY_ABNORMAL_ON_TA1_2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_READY_ABNORMAL_ON_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
				}
			}

			if((nError == NO_ERR) && (IsGoSignalOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsGoSignalOn() == false)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA1;
		            PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA1_2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
                }
			}

			if((nError == NO_ERR) && (IsESOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsESOn() == false)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_TA1;
		            PIO_LOG("ERR_PIO_ES_ABNORMAL_OFF_TA1_2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
					PIO_LOG("[PARAM UPDATE] m_TIMEOVER_EQPIO[0].nLREQ_ON_Time = %d", m_pParamSet->m_TIMEOVER_EQPIO[0].nLREQ_ON_Time);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
                }
			}

			if((nError == NO_ERR) && (IsHOAVBLOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsHOAVBLOn() == false)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TA1;
		            PIO_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TA1_2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TA1,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
                }
			}
		}
		break;

	case 11:
		if(m_mccparam.nPIOStep < 15)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO TR REQ ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		nError = TRREQOnOff(ON);
		if(nError == NO_ERR)
		{
			m_dwUnloadPIOWaitTime = timeGetTime();
			m_nBeforeUnloadStep = 12;
			m_mccparam.dwTRReqOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 16)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO TR REQ ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

	case 12:
		if(m_mccparam.nPIOStep < 17)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK READY ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if((nError == NO_ERR) && (IsReadyOn() == true))
		{
			m_nBeforeUnloadStep = 13;
			m_mccparam.dwReadyOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 18)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK READY ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		else
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nReady_ON_Time)
			{
				nError = ERR_PIO_READY_ABNORMAL_OFF_TA2;
	            PIO_LOG("ERR_PIO_READY_ABNORMAL_OFF_TA2_2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
				m_dwUnloadPIOWaitTime = 0;
				ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_READY_ABNORMAL_OFF_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
			}

			if((nError == NO_ERR) && (IsUnloadREQOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsUnloadREQOn() == false)
				{
					nError = ERR_PIO_UREQ_ABNORMAL_OFF_TA2;
		            PIO_LOG("ERR_PIO_UREQ_ABNORMAL_OFF_TA2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_UREQ_ABNORMAL_OFF_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
				}
			}

			if((nError == NO_ERR) && (IsLoadREQOn() == true))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsLoadREQOn() == true)
				{
					nError = ERR_PIO_LREQ_ABNORMAL_ON_TA2;
		            PIO_LOG("ERR_PIO_LREQ_ABNORMAL_ON_TA2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_LREQ_ABNORMAL_ON_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
				}
			}

			if((nError == NO_ERR) && (IsGoSignalOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsGoSignalOn() == false)
				{
					nError = ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA2;
		            PIO_LOG("ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA2_2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
                }
			}

			if((nError == NO_ERR) && (IsESOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsESOn() == false)
				{
					nError = ERR_PIO_ES_ABNORMAL_OFF_TA2;
		            PIO_LOG("ERR_PIO_ES_ABNORMAL_OFF_TA2_2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_ES_ABNORMAL_OFF_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
                }
			}

			if((nError == NO_ERR) && (IsHOAVBLOn() == false))
			{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsHOAVBLOn() == false)
				{
					nError = ERR_PIO_HOAVBL_ABNORMAL_OFF_TA2;
		            PIO_LOG("ERR_PIO_HOAVBL_ABNORMAL_OFF_TA2_2, SequenceBeforeUnload, Step(%d), E(%d)", m_nBeforeUnloadStep, nError);
					m_dwLoadPIOWaitTime = 0;
					ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/1/%d",
									ERR_PIO_HOAVBL_ABNORMAL_OFF_TA2,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nBeforeUnloadStep);
                }
			}
		}
		break;

 	case 13:
		if(m_mccparam.nPIOStep < 19)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO BUSY ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		nError = BusyOnOff(ON);
		if(nError == NO_ERR)
		{
			m_nBeforeUnloadStep = 14;
			m_mccparam.dwBusyOnTime = timeGetTime();
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,PIO BEFORE UNLOAD,End,%s,%06d,%d,%06d,%d,PIO Select On Time,%d,PIO Init Time,%d,HOAVBL On Time,%d,ES On Time,%d,CS0 On Time,%d,Valid On Time,%d,Unload Req On Time,%d,TR Req On Time,%d,Ready On Time,%d,Busy On Time,%d",
						m_defualtparam->VHLName,m_mccparam.cCarrierType,
						m_pOHTMainStatus->StatusCommon.CurStationID,
						m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_mccparam.dwPIOSelectOnTime,
						m_mccparam.dwPIOInitTime,
						m_mccparam.dwHOAVBLOnTime,
						m_mccparam.dwESOnTime,
						m_mccparam.dwCS0OnTime,
						m_mccparam.dwValidOnTime,
						m_mccparam.dwUReqOnTime,
						m_mccparam.dwTRReqOnTime,
						m_mccparam.dwReadyOnTime,
						m_mccparam.dwBusyOnTime);
			if(m_mccparam.nPIOStep < 20)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO BUSY ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
			m_mccparam.dwPIOSelectOnTime = 0;
			m_mccparam.dwPIOInitTime = 0;
			m_mccparam.dwHOAVBLOnTime = 0;
			m_mccparam.dwESOnTime = 0;
			m_mccparam.dwCS0OnTime = 0;
			m_mccparam.dwValidOnTime = 0;
			m_mccparam.dwUReqOnTime = 0;
			m_mccparam.dwTRReqOnTime = 0;
			m_mccparam.dwReadyOnTime = 0;
			m_mccparam.dwBusyOnTime = 0;
			m_mccparam.nPIOStep = 0;
		}
		break;
	}

	if(nError)
	{
		m_nBeforeUnloadStep = 0;
		m_dwUnloadPIOWaitTime = 0;
	}

	return nError;
}

int EQPIO::SequenceAfterTransForDualStageStb(bool bLoadUnload)
{
	int nError = 0;
	int *pTransStep;
	if(bLoadUnload == PIO_LOAD) pTransStep = &m_nAfterLoadStep;
	else if(bLoadUnload == PIO_UNLOAD) pTransStep = &m_nAfterUnloadStep;

	switch (*pTransStep)
	{
		case DUAL_STAGE_STB_STEP_AFTER_LOAD_BUSY_OFF:
		{
			PIO_LOG("DUAL_STAGE_STB_STEP_AFTER_LOAD_BUSY_OFF");
			if((nError == NO_ERR) && (IsUnloadREQOn() == true) && (bLoadUnload == PIO_LOAD))
	       	{
	       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsUnloadREQOn() == true || IsLoadREQOn() == false)
				{
					nError = ERR_PIO_UREQ_ABNORMAL_ON_TA3;
				}
	       	}

			if((nError == NO_ERR) && (IsLoadREQOn() == true) && (bLoadUnload == PIO_UNLOAD))
			{
				Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
				if(IsLoadREQOn() == true || IsUnloadREQOn() == false)
				{
					nError = ERR_PIO_LREQ_ABNORMAL_ON_TA3;
				}
			}

			if(nError == NO_ERR)
			{
				//nError = BusyOnOff(OFF);
				nError = CompleteOnOff(OFF);
			}
			if(nError == NO_ERR)
			{
				//nError = TRREQOnOff(OFF);
				nError = BusyOnOff(OFF);
			}
			if(nError == NO_ERR)
			{
				m_dwLoadPIOWaitTime = timeGetTime();
				*pTransStep = DUAL_STAGE_STB_STEP_AFTER_LOAD_BUSY_OFF_DELAY;
				PIO_LOG("DUAL_STAGE_STB_STEP_AFTER_LOAD_BUSY_OFF_DELAY");
			}
			break;
		}


		case DUAL_STAGE_STB_STEP_AFTER_LOAD_BUSY_OFF_DELAY:
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 100)
			{
				m_dwLoadPIOWaitTime = 0;
				*pTransStep = DUAL_STAGE_STB_STEP_AFTER_LOAD_COMPT_ON;
				PIO_LOG("DUAL_STAGE_STB_STEP_AFTER_LOAD_COMPT_ON");
			}
			break;
		}

		case DUAL_STAGE_STB_STEP_AFTER_LOAD_COMPT_ON:
		{
			//if(nError == NO_ERR) nError = CompleteOnOff(ON);
			if(nError == NO_ERR) nError = TRREQOnOff(ON);

			if(nError == NO_ERR)
			{
				m_dwLoadPIOWaitTime = timeGetTime();
				PIO_LOG("DUAL_STAGE_STB_STEP_AFTER_LOAD_CHECK_READY_OFF");
				*pTransStep = DUAL_STAGE_STB_STEP_AFTER_LOAD_CHECK_READY_OFF;
			}
			break;
		}

		case DUAL_STAGE_STB_STEP_AFTER_LOAD_CHECK_READY_OFF:
		{
			//if((IsReadyOn() == true) || (IsESOn() == false) || (IsHOAVBLOn() == false))
			if((IsCS2On() == true) || (IsHOAVBLOn() == false) || (IsCS0On() == false))
			{
				m_pParamSet->m_TIMEOVER_DUALSTAGEPIO.nReady_OFF_Time = 30000;
				if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_DUALSTAGEPIO.nReady_OFF_Time)
				{
					nError = ERR_PIO_READY_ABNORMAL_ON_TA3;
					PIO_LOG("ERR_PIO_READY_ABNORMAL_ON_TA3_1");
				}
			}
			else
			{
				//if(nError == NO_ERR)	nError = CompleteOnOff(OFF);
				if(nError == NO_ERR)	nError = TRREQOnOff(OFF);

				if(nError == NO_ERR)
				{
					m_dwLoadPIOWaitTime = timeGetTime();
					*pTransStep = DUAL_STAGE_STB_STEP_AFTER_LOAD_SIG_OFF;
					PIO_LOG("DUAL_STAGE_STB_STEP_AFTER_LOAD_SIG_OFF");
				}
			}
			break;
		}

		case DUAL_STAGE_STB_STEP_AFTER_LOAD_SIG_OFF:
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 50)
			{
//				if(nError == NO_ERR)
//					nError = ValidOnOff(OFF);
//				if(nError == NO_ERR)
//					nError = CS0OnOff(OFF);
//				if(nError == NO_ERR)
//					nError = CS1OnOff(OFF);
//				if(nError == NO_ERR)
//					nError = CS2OnOff(OFF);
//				if(nError == NO_ERR)
//					nError = BusyOnOff(OFF);
				if(nError == NO_ERR)
					nError = CS1OnOff(OFF);
				if(nError == NO_ERR)
					nError = CS2OnOff(OFF);
				if(nError == NO_ERR)
					nError = ValidOnOff(OFF);
				if(nError == NO_ERR)
					nError = CS0OnOff(OFF);
				if(nError == NO_ERR)
					nError = CompleteOnOff(OFF);
				if(nError == NO_ERR)
				{
					m_dwLoadPIOWaitTime = timeGetTime();
					*pTransStep = DUAL_STAGE_STB_STEP_AFTER_LOAD_SIG_OFF_DELAY;
					PIO_LOG("DUAL_STAGE_STB_STEP_AFTER_LOAD_SIG_OFF_DELAY");
				}
			}
			break;
		}

		case DUAL_STAGE_STB_STEP_AFTER_LOAD_SIG_OFF_DELAY:
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 100)
			{
				m_dwLoadPIOWaitTime = 0;
				*pTransStep = DUAL_STAGE_STB_STEP_AFTER_LOAD_FINISH;
			}
			break;
		}
	}

	if(nError)
	{
		*pTransStep = 0;
		m_dwLoadPIOWaitTime = 0;
	}

	return nError;
}
 #if(OHT_NAME == OHT_NAME_P4_MASK)
/**
@brief   Loading 작업 시 반송물을 설비에 내려 놓은 후 Hoist와 Shift 축 이동이 완료 된 후 PIO 시퀀스 구현(Loading 시 사용)
@author	 LSJ
@date	 2013.09.02
@param   PIOTimeoverLevel : PIO 통신 시 TimeOver 값을 얼마로 설정할지 결정하는 변수
@return  PIO 통신에 이상 없으면 0, 문제 발생 시 ErrorCode 리턴*/

int EQPIO::SequenceAfterLoad(int PIOTimeoverLevel)
{
	int nError = 0;

	switch (m_nAfterLoadStep)
	{
	case 1: // Busy Off
		if((nError == NO_ERR) && (IsLoadREQOn() == true))
		{
			Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
			if(IsLoadREQOn() == true)
			{
				nError = ERR_PIO_LREQ_ABNORMAL_ON_TA3;
				PIO_LOG("ERR_PIO_LREQ_ABNORMAL_ON_TA3_1");
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/2/%d",
									ERR_PIO_LREQ_ABNORMAL_ON_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nAfterLoadStep);
			}
		}

		if((nError == NO_ERR) && (IsUnloadREQOn() == true))
		{
       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
			if(IsUnloadREQOn() == true)
			{
				nError = ERR_PIO_UREQ_ABNORMAL_ON_TA3;
	            PIO_LOG("ERR_PIO_UREQ_ABNORMAL_ON_TA3_1");
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/2/%d",
									ERR_PIO_UREQ_ABNORMAL_ON_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nAfterLoadStep);
			}
       	}

        if(nError == NO_ERR)
		{
			nError = BusyOnOff(OFF);
		}
		if(nError == NO_ERR)
		{
			m_dwLoadPIOWaitTime = timeGetTime();
			m_nAfterLoadStep = 2;
		}
		break;
	case 2: // Sleep 100
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 100)
		{
			m_dwLoadPIOWaitTime = 0;
			m_nAfterLoadStep = 3;
		}
		break;
	case 3: // Complete ON
	if(nError == NO_ERR)
	   nError = TRREQOnOff(OFF);
		if(nError == NO_ERR)
		{
			m_dwLoadPIOWaitTime = timeGetTime();
			m_nAfterLoadStep = 4;
		}
		break;
	case 4: // Sleep 10
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 200)
		{
			m_dwLoadPIOWaitTime = 0;
			m_nAfterLoadStep = 5;
		}
		break;
	case 5: // TR_REQ OFF
	if(nError == NO_ERR)
			nError = CompleteOnOff(ON);
		if(nError == NO_ERR)
		{
			m_dwLoadPIOWaitTime = timeGetTime();
			m_nAfterLoadStep = 6;
		}
		break;
	case 6: // READY OFF Check
		if((IsReadyOn() == true) || (IsESOn() == false) || (IsHOAVBLOn() == false))
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nReady_OFF_Time)
			{
				nError = ERR_PIO_READY_ABNORMAL_ON_TA3;
				PIO_LOG("ERR_PIO_READY_ABNORMAL_ON_TA3_1");
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/2/%d",
									ERR_PIO_READY_ABNORMAL_ON_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nAfterLoadStep);
			}
		}
		else
		{
			if(nError == NO_ERR)
				nError = CompleteOnOff(OFF);

			if(nError == NO_ERR)
			{
				m_dwLoadPIOWaitTime = timeGetTime();
				m_nAfterLoadStep = 7;
			}
		}
		break;
	case 7: // Sleep 10
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 50)
		{
			if(nError == NO_ERR)
				nError = ValidOnOff(OFF);
			if(nError == NO_ERR)
				nError = CS0OnOff(OFF);
			if(nError == NO_ERR)
				nError = TRREQOnOff(OFF);
			if(nError == NO_ERR)
				nError = BusyOnOff(OFF);
			if(nError == NO_ERR)
			{
				m_dwLoadPIOWaitTime = timeGetTime();
				m_nAfterLoadStep = 8;
			}
		}
		break;
	case 8: // Sleep 10
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 100)
		{
			m_dwLoadPIOWaitTime = 0;
			m_nAfterLoadStep = 9;
		}
		break;
	}

	if(nError)
	{
		m_nAfterLoadStep = 0;
		m_dwLoadPIOWaitTime = 0;
	}

	return nError;
}
#endif
/**
@brief   Loading 작업 시 반송물을 설비에 내려 놓은 후 Hoist와 Shift 축 이동이 완료 된 후 PIO 시퀀스 구현(Loading 시 사용)
@author	 LSJ
@date	 2013.09.02
@param   PIOTimeoverLevel : PIO 통신 시 TimeOver 값을 얼마로 설정할지 결정하는 변수
@return  PIO 통신에 이상 없으면 0, 문제 발생 시 ErrorCode 리턴
*/
//int EQPIO::SequenceAfterLoad(int PIOTimeoverLevel)
//{
//	int nError = 0;
//
//	switch (m_nAfterLoadStep)
//	{
//	case 1: // Busy Off
//		if((nError == NO_ERR) && (IsLoadREQOn() == true))
//		{
//			Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
//			if(IsLoadREQOn() == true)
//			{
//				nError = ERR_PIO_LREQ_ABNORMAL_ON_TA3;
//				PIO_LOG("ERR_PIO_LREQ_ABNORMAL_ON_TA3_1");
//				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/2/%d",
//									ERR_PIO_LREQ_ABNORMAL_ON_TA3,
//									m_pOHTMainStatus->StatusCommon.CurNodeID,
//									m_pExecuteInfo->ExecutePositionInfo.NextNode,
//									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//									m_pOHTMainStatus->StatusCommon.ExistFoup,
//									m_pOHTMainStatus->StatusCommon.StopStationID,
//									m_nAfterLoadStep);
//			}
//		}
//
//		if((nError == NO_ERR) && (IsUnloadREQOn() == true))
//		{
//       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
//			if(IsUnloadREQOn() == true)
//			{
//				nError = ERR_PIO_UREQ_ABNORMAL_ON_TA3;
//	            PIO_LOG("ERR_PIO_UREQ_ABNORMAL_ON_TA3_1");
//				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/2/%d",
//									ERR_PIO_UREQ_ABNORMAL_ON_TA3,
//									m_pOHTMainStatus->StatusCommon.CurNodeID,
//									m_pExecuteInfo->ExecutePositionInfo.NextNode,
//									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//									m_pOHTMainStatus->StatusCommon.ExistFoup,
//									m_pOHTMainStatus->StatusCommon.StopStationID,
//									m_nAfterLoadStep);
//			}
//       	}
//
//        if(nError == NO_ERR)
//		{
//			nError = BusyOnOff(OFF);
//		}
//		if(nError == NO_ERR)
//		{
//			m_dwLoadPIOWaitTime = timeGetTime();
//			m_nAfterLoadStep = 2;
//		}
//		break;
//	case 2: // Sleep 100
//		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 100)
//		{
//			m_dwLoadPIOWaitTime = 0;
//			m_nAfterLoadStep = 3;
//		}
//		break;
//	case 3: // Complete ON
//    if(nError == NO_ERR)
//	   nError = TRREQOnOff(OFF);
//		if(nError == NO_ERR)
//		{
//			m_dwLoadPIOWaitTime = timeGetTime();
//			m_nAfterLoadStep = 4;
//		}
//		break;
//	case 4: // Sleep 10
//		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 200)
//		{
//			m_dwLoadPIOWaitTime = 0;
//			m_nAfterLoadStep = 5;
//		}
//		break;
//	case 5: // TR_REQ OFF
//    if(nError == NO_ERR)
//			nError = CompleteOnOff(ON);
//
//		if(nError == NO_ERR)
//		{
//			m_dwLoadPIOWaitTime = timeGetTime();
//			m_nAfterLoadStep = 6;
//		}
//		break;
//	case 6: // READY OFF Check
//        if((IsReadyOn() == true) || (IsESOn() == false) || (IsHOAVBLOn() == false))
//		{
//			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nReady_OFF_Time)
//			{
//				nError = ERR_PIO_READY_ABNORMAL_ON_TA3;
//				PIO_LOG("ERR_PIO_READY_ABNORMAL_ON_TA3_1");
//				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/2/%d",
//									ERR_PIO_READY_ABNORMAL_ON_TA3,
//									m_pOHTMainStatus->StatusCommon.CurNodeID,
//									m_pExecuteInfo->ExecutePositionInfo.NextNode,
//									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//									m_pOHTMainStatus->StatusCommon.ExistFoup,
//									m_pOHTMainStatus->StatusCommon.StopStationID,
//									m_nAfterLoadStep);
//			}
//		}
//        else
//		{
//            if(nError == NO_ERR)
//				nError = CompleteOnOff(OFF);
//
//			if(nError == NO_ERR)
//			{
//				m_dwLoadPIOWaitTime = timeGetTime();
//				m_nAfterLoadStep = 7;
//			}
//        }
//		break;
//	case 7: // Sleep 10
//		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 50)
//		{
//			if(nError == NO_ERR)
//				nError = ValidOnOff(OFF);
//			if(nError == NO_ERR)
//				nError = CS0OnOff(OFF);
//			if(nError == NO_ERR)
//				nError = TRREQOnOff(OFF);
//			if(nError == NO_ERR)
//				nError = BusyOnOff(OFF);
//			if(nError == NO_ERR)
//			{
//				m_dwLoadPIOWaitTime = timeGetTime();
//				m_nAfterLoadStep = 8;
//			}
//		}
//		break;
//	case 8: // Sleep 10
//		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwLoadPIOWaitTime) > 100)
//		{
//			m_dwLoadPIOWaitTime = 0;
//			m_nAfterLoadStep = 9;
//		}
//		break;
//	}
//
//	if(nError)
//	{
//		m_nAfterLoadStep = 0;
//		m_dwLoadPIOWaitTime = 0;
//	}
//
//	return nError;
//}


/**
@brief   Unloading 작업 시 반송물을 설비에 내려 놓은 후 Hoist와 Shift 축 이동이 완료 된 후 PIO 시퀀스 구현(Unloading 시 사용)
@author	 LSJ
@date	 2013.09.02
@param   PIOTimeoverLevel : PIO 통신 시 TimeOver 값을 얼마로 설정할지 결정하는 변수
@return  PIO 통신에 이상 없으면 0, 문제 발생 시 ErrorCode 리턴
*/
int EQPIO::SequenceAfterUnload(int PIOTimeoverLevel,int nEQType)
{
	int nError = 0;
	Update_MCC();
	m_mccparam.nEqType = nEQType;

	switch (m_nAfterUnloadStep)
	{
	case 1: // HO, ES 감시
		 if(nError == NO_ERR)
		 {
			m_dwUnloadPIOWaitTime = timeGetTime();
			m_nAfterUnloadStep = 2;
		 }

	case 2: // Busy Off
		if(m_mccparam.nPIOStep < 1)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO BUSY OFF,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
        if((nError == NO_ERR) && (IsLoadREQOn() == true))
       	{
       		Sleep(EQPIO_ABNORMAL_ON_OFF_CHECK_TIME);
			if(IsLoadREQOn() == true)
			{
				nError = ERR_PIO_LREQ_ABNORMAL_ON_TA3;
	            PIO_LOG("ERR_PIO_LREQ_ABNORMAL_ON_TA3_2");
				ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/2/%d",
									ERR_PIO_LREQ_ABNORMAL_ON_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nAfterUnloadStep);
			}
       	}

		if((nError == NO_ERR) && (IsUnloadREQOn() == true))
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nUREQ_OFF_Time)
			{
				nError = ERR_PIO_UREQ_ABNORMAL_ON_TA3;
	            PIO_LOG("ERR_PIO_UREQ_ABNORMAL_ON_TA3_2");
				m_dwUnloadPIOWaitTime = 0;
				ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/2/%d",
									ERR_PIO_UREQ_ABNORMAL_ON_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nAfterUnloadStep);
			}
        }
        else
		{
			if(nError == NO_ERR)
			{
				if(nError == NO_ERR)
				{
					nError = BusyOnOff(OFF);
				}
				if(nError == NO_ERR)
				{
					m_dwUnloadPIOWaitTime = timeGetTime();
					m_nAfterUnloadStep = 3;
					ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,PIO AFTER UNLOAD,Start,%s,%06d,%d,%06d,%d",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset);
					m_mccparam.dwBusyOffTime = timeGetTime();
					if(m_mccparam.nPIOStep < 2)
					{
						ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO BUSY OFF,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
											m_defualtparam->VHLName,m_mccparam.cCarrierType,
											m_pOHTMainStatus->StatusCommon.CurStationID,
											m_mccparam.nEqType,
											m_pOHTMainStatus->StatusCommon.CurNodeID,
											m_pOHTMainStatus->StatusCommon.CurNodeOffset,
											m_mccparam.uPIOInput,
											m_mccparam.uPIOOutput);
						m_mccparam.nPIOStep++;
					}
				}
			}
		}
		break;

	case 3: // Sleep 50
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > 50)
		{
			m_dwUnloadPIOWaitTime = 0;
			m_nAfterUnloadStep = 4;
		}
		break;

	case 4: // Complete ON
		if(m_mccparam.nPIOStep < 3)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO TR REQ OFF,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		nError = TRREQOnOff(OFF);
		if(nError == NO_ERR)
		{
			m_dwUnloadPIOWaitTime = timeGetTime();
			m_nAfterUnloadStep = 5;
			m_mccparam.dwTRReqOffTime = timeGetTime();
			if(m_mccparam.nPIOStep < 4)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO TR REQ OFF,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

	case 5: // Sleep 10
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > 200)
		{
			m_dwUnloadPIOWaitTime = 0;
			m_nAfterUnloadStep = 6;
		}
		break;

	case 6: // TR_REQ OFF
		if(m_mccparam.nPIOStep < 5)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO COMPLETE ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
        if(nError == NO_ERR)
       	{
       	    nError = CompleteOnOff(ON);

       	}
		if(nError == NO_ERR)
		{
			m_dwUnloadPIOWaitTime = timeGetTime();
			m_nAfterUnloadStep = 7;
			m_mccparam.dwCompleteOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 6)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO COMPLETE ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
        }
		break;

	case 7: // READY OFF Check
		if(m_mccparam.nPIOStep < 7)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK READY OFF,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if((IsReadyOn() == true) || (IsESOn() == false) || (IsHOAVBLOn() == false))
        {
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nReady_OFF_Time)
			{
				nError = ERR_PIO_READY_ABNORMAL_ON_TA3;
	            PIO_LOG("ERR_PIO_READY_ABNORMAL_ON_TA3_2");
				ADD_FDC_LOG("ERR/%d/2/8/%06d/%06d/%d/%d/%06d/2/2/%d",
									ERR_PIO_READY_ABNORMAL_ON_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nAfterUnloadStep);
			}
		}
        else
		{
			if(m_mccparam.nPIOStep < 8)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO CHECK READY OFF,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
			if(m_mccparam.nPIOStep < 9)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO COMPLETE OFF,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
            if(nError == NO_ERR)
           	{
				nError = CompleteOnOff(OFF);
           	}
			if(nError == NO_ERR)
			{
				m_dwUnloadPIOWaitTime = timeGetTime();
				m_nAfterUnloadStep = 8;
				m_mccparam.dwReadyOffTime = timeGetTime();
				if(m_mccparam.nPIOStep < 10)
				{
					ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO COMPLETE OFF,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
										m_defualtparam->VHLName,m_mccparam.cCarrierType,
										m_pOHTMainStatus->StatusCommon.CurStationID,
										m_mccparam.nEqType,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_mccparam.uPIOInput,
										m_mccparam.uPIOOutput);
					m_mccparam.nPIOStep++;
				}
			}
        }
        break;

	case 8: // Sleep 10
		if(m_mccparam.nPIOStep < 11)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO DONE,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > 50)
		{
			if(nError == NO_ERR)	nError = ValidOnOff(OFF);
			if((nEQType == PORT_TYPE_STB_L_B_S) || (nEQType == PORT_TYPE_STB_R_B_S))
			{
				if(nError == NO_ERR)	nError = CSBitOnOff(OFF);
			}
			else
			{
				if(nError == NO_ERR)	nError = CS0OnOff(OFF);
            }
			if(nError == NO_ERR)	nError = TRREQOnOff(OFF);
			if(nError == NO_ERR)	nError = BusyOnOff(OFF);
			if(nError == NO_ERR)
			{
				m_dwUnloadPIOWaitTime = timeGetTime();
				m_nAfterUnloadStep = 9;
				m_mccparam.dwOutputOffTime = timeGetTime();
			}
		}
		break;

	case 9: // Sleep 10
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwUnloadPIOWaitTime) > 100)
		{
			m_dwUnloadPIOWaitTime = 0;
			m_nAfterUnloadStep = 10;
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,PIO AFTER UNLOAD,End,%s,%06d,%d,%06d,%d,Busy Off Time,%d,TR Req Off Time,%d,Complete On Time,%d,Ready Off Time,%d,Output Off Time,%d",
						m_defualtparam->VHLName,m_mccparam.cCarrierType,
						m_pOHTMainStatus->StatusCommon.CurStationID,
						m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_mccparam.dwBusyOffTime,
						m_mccparam.dwTRReqOffTime,
						m_mccparam.dwCompleteOnTime,
						m_mccparam.dwReadyOffTime,
						m_mccparam.dwOutputOffTime);
			if(m_mccparam.nPIOStep < 12)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,UPIO DONE,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
			m_mccparam.dwBusyOffTime = 0;
			m_mccparam.dwTRReqOffTime = 0;
			m_mccparam.dwCompleteOnTime = 0;
			m_mccparam.dwReadyOffTime = 0;
			m_mccparam.dwOutputOffTime = 0;
			m_mccparam.nPIOStep = 0;
		}
		break;
	}

	if(nError)
	{
		m_nAfterUnloadStep = 0;
		m_dwUnloadPIOWaitTime = 0;
	}

	return nError;
}

/**
@brief   Loading 작업 시 반송물을 설비에 내려 놓은 후 Hoist와 Shift 축 이동 시의 PIO 시퀀스 구현(Loading 시 사용)
@author	 LSJ
@date	 2012.11.08
@param   PIOTimeoverLevel : PIO 통신 시 TimeOver 값을 얼마로 설정할지 결정하는 변수
@return  PIO 통신에 이상 없으면 0, 문제 발생 시 ErrorCode 리턴
*/
int EQPIO::SequenceForMoving(int PIOTimeoverLevel,int nEQType)
{
    int nError = 0;
	Update_MCC();
	m_mccparam.nEqType = nEQType;

	switch (m_nStep)
	{
	case 1: // Busy Off
		if(m_mccparam.nPIOStep < 1)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO BUSY OFF,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
        if(nError == NO_ERR)
       	{
			nError = BusyOnOff(OFF);
       	}
		if(nError == NO_ERR)
		{
			m_nStep = 2;
			m_dwPIOWaitTime = timeGetTime();
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,PIO AFTER LOAD,Start,%s,%06d,%d,%06d,%d",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset);
			m_mccparam.dwBusyOffTime = timeGetTime();
			if(m_mccparam.nPIOStep < 2)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO BUSY OFF,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
		}
		break;

    case 2: // Sleep 100
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwPIOWaitTime) > 100)
		{
			m_nStep = 3;
			m_dwPIOWaitTime = 0;
		}
		break;

    case 3: // Complete ON
		if(m_mccparam.nPIOStep < 3)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO TR REQ OFF,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
        nError = TRREQOnOff(OFF);
        if(nError == NO_ERR)
        {
            m_nStep = 4;
            m_dwPIOWaitTime = timeGetTime();
			m_mccparam.dwTRReqOffTime = timeGetTime();
			if(m_mccparam.nPIOStep < 4)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO TR REQ OFF,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
        }
        break;

    case 4: // Sleep 10
        if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwPIOWaitTime) > 200)
        {
            m_nStep = 5;
            m_dwPIOWaitTime = 0;
        }
        break;

    case 5: // TR_REQ OFF
		if(m_mccparam.nPIOStep < 5)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO COMPLETE ON,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
        if(nError == NO_ERR)
       	{
       	    nError = CompleteOnOff(ON);
        }
        if(nError == NO_ERR)
        {
            m_nStep = 6;
            m_dwPIOWaitTime = timeGetTime();
			m_mccparam.dwCompleteOnTime = timeGetTime();
			if(m_mccparam.nPIOStep < 6)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO COMPLETE ON,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
        }
        break;

    case 6: // READY OFF Check
		if(m_mccparam.nPIOStep < 7)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK READY OFF,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
        if((IsReadyOn() == true) || (IsESOn() == false) || (IsHOAVBLOn() == false))
        {
            if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwPIOWaitTime) > m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nReady_OFF_Time)
           	{
                nError = ERR_PIO_READY_ABNORMAL_ON_TA3;
	            PIO_LOG("ERR_PIO_READY_ABNORMAL_ON_TA3_3");
				ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/1/2/%d",
									ERR_PIO_READY_ABNORMAL_ON_TA3,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID,
									m_nStep);
           	}
		}
        else
        {
			if(m_mccparam.nPIOStep < 8)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO CHECK READY OFF,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
			if(m_mccparam.nPIOStep < 9)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO COMPLETE OFF,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
            if(nError == NO_ERR)
           	{
                nError = CompleteOnOff(OFF);
           	}
            if(nError == NO_ERR)
            {
                m_nStep = 7;
                m_dwPIOWaitTime = timeGetTime();
				m_mccparam.dwReadyOffTime = timeGetTime();
				if(m_mccparam.nPIOStep < 10)
				{
					ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO COMPLETE OFF,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
										m_defualtparam->VHLName,m_mccparam.cCarrierType,
										m_pOHTMainStatus->StatusCommon.CurStationID,
										m_mccparam.nEqType,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_mccparam.uPIOInput,
										m_mccparam.uPIOOutput);
					m_mccparam.nPIOStep++;
				}
            }
        }
        break;

    case 7: // Sleep 10
		if(m_mccparam.nPIOStep < 11)
		{
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO DONE,Start,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
								m_defualtparam->VHLName,m_mccparam.cCarrierType,
								m_pOHTMainStatus->StatusCommon.CurStationID,
								m_mccparam.nEqType,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_mccparam.uPIOInput,
								m_mccparam.uPIOOutput);
			m_mccparam.nPIOStep++;
		}
		if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwPIOWaitTime) > 50)
        {
            if(nError == NO_ERR)	nError = ValidOnOff(OFF);
			if((nEQType == PORT_TYPE_STB_R_T) || (nEQType == PORT_TYPE_STB_R_B))
			{
				if(nError == NO_ERR) 	nError = CSBitOnOff(OFF);
			}
			else
			{
				if(nError == NO_ERR)    nError = CS0OnOff(OFF);
			}
            if(nError == NO_ERR)    nError = TRREQOnOff(OFF);
            if(nError == NO_ERR)    nError = BusyOnOff(OFF);

            m_nStep = 8;
            m_dwPIOWaitTime = timeGetTime();
			m_mccparam.dwOutputOffTime = timeGetTime();
        }
        break;

    case 8: // Sleep 10
        if(OHTUTILITY::PreventOverflowGetTimeMs(m_dwPIOWaitTime) > 500)
        {
            m_nStep = 9;
			m_dwPIOWaitTime = 0;
			ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,PIO AFTER LOAD,End,%s,%06d,%d,%06d,%d,Busy Off Time,%d,TR Req Off Time,%d,Complete On Time,%d,Ready Off Time,%d,Output Off Time,%d",
						m_defualtparam->VHLName,m_mccparam.cCarrierType,
						m_pOHTMainStatus->StatusCommon.CurStationID,
						m_mccparam.nEqType,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_mccparam.dwBusyOffTime,
						m_mccparam.dwTRReqOffTime,
						m_mccparam.dwCompleteOnTime,
						m_mccparam.dwReadyOffTime,
						m_mccparam.dwOutputOffTime);
			if(m_mccparam.nPIOStep < 12)
			{
				ADD_MCC_LOG(m_pExecuteInfo->ExecuteMode,",%s,TRANS,LPIO DONE,End,%s,%06d,%d,%06d,%d,PIO Input,%X,PIO Output,%X",
									m_defualtparam->VHLName,m_mccparam.cCarrierType,
									m_pOHTMainStatus->StatusCommon.CurStationID,
									m_mccparam.nEqType,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_mccparam.uPIOInput,
									m_mccparam.uPIOOutput);
				m_mccparam.nPIOStep++;
			}
			m_mccparam.dwBusyOffTime = 0;
			m_mccparam.dwTRReqOffTime = 0;
			m_mccparam.dwCompleteOnTime = 0;
			m_mccparam.dwReadyOffTime = 0;
			m_mccparam.dwOutputOffTime = 0;
			m_mccparam.nPIOStep = 0;
        }
        break;
    }

	if(nError)
	{
		m_nStep = 0;
        m_dwPIOWaitTime = 0;
	}

	return nError;
}

/**
@brief	 SequenceForMoving 함수 수행 전 내부 Step과 PIOWaitTime을 초기화 함.
@author	 LSJ
@date	 2012.11.08
*/
void EQPIO::Init_SequenceForMoving()
{
    m_nStep = 1;
	m_dwPIOWaitTime = 0;
	nDuplicated_CheckCount= 0;
}

/**
@brief	 SequenceForLoading 함수 수행 전 내부 Step과 PIOWaitTime을 초기화 함.
@author	 LSJ
@date	 2013.08.18
*/
void EQPIO::Init_SequenceForLoading()
{
	m_nBeforeLoadStep = 1;
	m_nAfterLoadStep = 1;
	m_dwLoadPIOWaitTime = 0;
	nDuplicated_CheckCount= 0;
}

/**
@brief	 SequenceForUnloading 함수 수행 전 내부 Step과 PIOWaitTime을 초기화 함.
@author	 LSJ
@date	 2013.08.18
*/
void EQPIO::Init_SequenceForUnloading()
{
	m_nBeforeUnloadStep = 1;
	m_nAfterUnloadStep = 1;
	m_dwUnloadPIOWaitTime = 0;
	nDuplicated_CheckCount= 0;
}

bool EQPIO::IsPortPOn()
{
    bool bResult = false;

    if (m_pHWCommon->m_Input.EQPIO_Load_REQ == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

bool EQPIO::IsPortNOn()
{
    bool bResult = false;

    if (m_pHWCommon->m_Input.EQPIO_Unload_REQ == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

bool EQPIO::IsCS0On()
{
    bool bResult = false;
#if(OHT_NAME != OHT_NAME_STD_V85)
    if (m_pHWCommon->m_Input.PIO_Input3 == ON)
        bResult = true;
    else
        bResult = false;
#endif
    return bResult;
}

bool EQPIO::IsCS1On()
{
    bool bResult = false;
#if(OHT_NAME != OHT_NAME_STD_V85)
    if (m_pHWCommon->m_Input.PIO_Input5 == ON)
        bResult = true;
    else
        bResult = false;
#endif
    return bResult;
}


bool EQPIO::IsCS2On()
{
    bool bResult = false;
#if(OHT_NAME != OHT_NAME_STD_V85)
    if (m_pHWCommon->m_Input.PIO_Input6 == ON)
        bResult = true;
    else
        bResult = false;
#endif
    return bResult;
}

/**
@brief	 설비 PIO 신호인 Load Request 신호가 On인지 Off 인지 확인하는 함수
@author	 LSJ
@date	 2012.10.27
@return	 Load_Request 신호가 On 이면 true, Off 이면 false 리턴.
*/
bool EQPIO::IsLoadREQOn()
{
    bool bResult = false;

    if (m_pHWCommon->m_Input.EQPIO_Load_REQ == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief	 설비 PIO 신호인 Unload Request 신호가 On인지 Off 인지 확인하는 함수
@author	 LSJ
@date	 2012.10.27
@return	 Unload_Request 신호가 On 이면 true, Off 이면 false 리턴.
*/
bool EQPIO::IsUnloadREQOn()
{
    bool bResult = false;

    if (m_pHWCommon->m_Input.EQPIO_Unload_REQ == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief	 설비 PIO 신호인 Ready 신호가 On인지 Off 인지 확인하는 함수
@author	 LSJ
@date	 2012.10.27
@return	 Ready 신호가 On 이면 true, Off 이면 false 리턴.
*/
bool EQPIO::IsReadyOn()
{
    bool bResult = false;

    if (m_pHWCommon->m_Input.EQPIO_Ready == ON)
        bResult = true;
    else
        bResult = false;

	return bResult;
}

/**
@brief	 설비 PIO 신호인 HO available 신호가 On인지 Off 인지 확인하는 함수
@author	 LSJ
@date	 2012.10.27
@return	 HO available 신호가 On 이면 true, Off 이면 false 리턴.
*/
bool EQPIO::IsHOAVBLOn()
{
    bool bResult = false;

	if (m_pHWCommon->m_Input.EQPIO_HO_AVBL == ON)
        bResult = true;
    else
		bResult = false;

    return bResult;
}

/**
@brief	 설비 PIO 신호인 ES 신호가 On인지 Off 인지 확인하는 함수
@author	 LSJ
@date	 2012.10.27
@return	 ES 신호가 On 이면 true, Off 이면 false 리턴.
*/
bool EQPIO::IsESOn()
{
    bool bResult = false;

    if (m_pHWCommon->m_Input.EQPIO_ES == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

//#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V81S) || (OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86))
///**
//@brief	 Select 신호가 On인지 Off 인지 확인하는 함수
//@author	 JSH
//@date	 2014.08.05
//@return	 Select 신호가 Off 이면 true, Off 이면 false 리턴.
//*/
//bool EQPIO::IsSelectOff()
//{
//	bool bResult = false;
//
//#if((OHT_NAME != OHT_NAME_STD_V85) && (OHT_NAME != OHT_NAME_STD_V81S))
//	if((m_pHWCommon->m_Output.EQPIO_Left_Select == OFF)||(m_pHWCommon->m_Output.EQPIO_Right_Select == OFF))
//		bResult = true;
//	else
//		bResult = false;
// #else
//	if(m_pHWCommon->m_Output.EQPIO_Left_Select == OFF)
//		bResult = true;
//	else
//		bResult = false;
// #endif
//
//	return bResult;
//}
//#endif

/**
@brief	 설비 PIO 신호인 Go 신호가 On인지 Off 인지 확인하는 함수
@author	 LSJ
@date	 2012.10.27
@param   EQ PIO 선택(LEFT or RIGHT)
@return	 ES 신호가 On 이면 true, Off 이면 false 리턴.
*/
bool EQPIO::IsGoSignalOn()
{
    bool bResult = false;

    if (m_nSide == LEFT_SIDE)
	{
		if (m_pHWCommon->m_Input.EQPIO_Left_Go == ON)
            bResult = true;
        else
            bResult = false;
    }
    else
	{
#if((OHT_NAME != OHT_NAME_STD_V85) && (OHT_NAME != OHT_NAME_STD_V81S)&& (OHT_NAME != OHT_NAME_STD_V85S)&&(OHT_NAME != OHT_NAME_STD_V82)&&(OHT_NAME != OHT_NAME_STD_V86)&&(OHT_NAME != OHT_NAME_P4_MASK))
		if (m_pHWCommon->m_Input.EQPIO_Right_Go == ON)
			bResult = true;
        else
			bResult = false;
#endif
    }

	// PIO_LOG("Side:%d/Sel:[%d,%d]/L:%d/R:%d", m_nSide, m_pHWCommon->m_Output.EQPIO_Left_Select, m_pHWCommon->m_Output.EQPIO_Right_Select, m_pHWCommon->m_Input.EQPIO_Left_Go, m_pHWCommon->m_Input.EQPIO_Right_Go);

    return bResult;
}
bool EQPIO::IsCompleteOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Output.EQPIO_Complete == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}
/**
@brief	 OHT PIO 출력신호인 Valid 신호가 On인지 Off 인지 확인하는 함수
@author	 LSJ
@date	 2012.10.27
@return	 ES 신호가 On 이면 true, Off 이면 false 리턴.
*/
bool EQPIO::IsValidOn()
{
    bool bResult = false;

    if (m_pHWCommon->m_Output.EQPIO_Valid == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief	 PIO 출력 신호인 Valid 신호를 켜고 끄는 함수
@author	 LSJ
@date	 2012.10.29
@param   OnOff : 해당 출력을 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int EQPIO::ValidOnOff(bool bOnOff)
{
    int nError = NO_ERR;

    if (bOnOff == ON)
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_VALID, ON);
    }
    else
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_VALID, OFF);
    }

    return nError;
}

/**
@brief	 PIO 출력 신호인 CS0 신호를 켜고 끄는 함수
@author	 LSJ
@date	 2012.10.29
@param   OnOff : 해당 출력을 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int EQPIO::CS0OnOff(bool bOnOff)
{
    int nError = NO_ERR;

    if (bOnOff == ON)
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, ON);
    }
    else
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, OFF);
    }

    return nError;
}

/**
@brief	 PIO 출력 신호인 CS1 신호를 켜고 끄는 함수
@author	 LSJ
@date	 2012.10.29
@param   OnOff : 해당 출력을 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int EQPIO::CS1OnOff(bool bOnOff)
{
    int nError = NO_ERR;

    if (bOnOff == ON)
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_1, ON);
    }
    else
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_1, OFF);
    }

    return nError;
}

/**
@brief	 PIO 출력 신호인 CS2 신호를 켜고 끄는 함수
@author	 LSJ
@date	 2012.10.29
@param   OnOff : 해당 출력을 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int EQPIO::CS2OnOff(bool bOnOff)
{
	int nError = NO_ERR;

    if (bOnOff == ON)
    {
        if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_2, ON);
    }
    else
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_2, OFF);
    }

    return nError;
}

/**
@brief	 PIO 출력 신호인 CS2 신호를 켜고 끄는 함수
@author	 LSJ
@date	 2012.10.29
@param   OnOff : 해당 출력을 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int EQPIO::CS3OnOff(bool bOnOff)
{
	int nError = NO_ERR;

    if (bOnOff == ON)
    {
        if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_3, ON);
	}
	else
	{
		if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_3, OFF);
    }

    return nError;
}

/**
@brief	 PIO 출력 신호인 TR Request 신호를 켜고 끄는 함수
@author	 LSJ
@date	 2012.10.29
@param   OnOff : 해당 출력을 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int EQPIO::TRREQOnOff(bool bOnOff)
{
    int nError = NO_ERR;

    if (bOnOff == ON)
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_TR_REQ, ON);
    }
    else
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_TR_REQ, OFF);
    }

    return nError;
}

/**
@brief	 PIO 출력 신호인 Busy 신호를 켜고 끄는 함수
@author	 LSJ
@date	 2012.10.29
@param   OnOff : 해당 출력을 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int EQPIO::BusyOnOff(bool bOnOff)
{
    int nError = NO_ERR;

    if (bOnOff == ON)
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_BUSY, ON);
    }
    else
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_BUSY, OFF);
    }

    return nError;
}

/**
@brief	 PIO 출력 신호인 Complete 신호를 켜고 끄는 함수
@author	 LSJ
@date	 2012.10.29
@param   OnOff : 해당 출력을 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int EQPIO::CompleteOnOff(bool bOnOff)
{
    int nError = NO_ERR;

    if (bOnOff == ON)
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_COMPLETE, ON);
    }
    else
    {
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_EQPIO_COMPLETE, OFF);
    }

    return nError;
}

/**
@brief	 PIO 출력 신호인 CS Bit 조합 신호를 켜고 끄는 함수
@author	 LSJ
@date	 2012.10.29
@param   OnOff : 해당 출력을 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int EQPIO::CSBitOnOff(bool bOnOff)
{
	int nError = NO_ERR;

	switch(m_nCmdCS)
	{
		case 1 :
			if(bOnOff == true)
			{
				nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, ON);
			}
			else
			{
				nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, OFF);
            }
			break;

		case 2 :
			if(bOnOff == true)
			{
				nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_1, ON);
			}
			else
			{
				nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_1, OFF);
			}
			break;

		case 3 :
			if(bOnOff == true)
			{
				nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, ON);
				if (nError == NO_ERR)
					nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_1, ON);
			}
			else
			{
				nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, OFF);
				if (nError == NO_ERR)
					nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_1, OFF);
			}
			break;

		case 4 :
			if(bOnOff == true)
			{
				nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_2, ON);
			}
			else
			{
				nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_2, OFF);
			}
			break;
    }

    return nError;
}

/**
@brief	 PIO 신호 중 Ready 신호가 Off 될때까지 기다리는 Timeover 셋팅 값이 얼마인지 확인하는 함수
@author	 LSJ
@date	 2012.10.29
@param   PIOTimeoverLevel : PIO 통신 시 TimeOver 값을 얼마로 설정할지 결정하는 변수
@return	 Ready Off Timeover 값 리턴.
*/
int EQPIO::GetReadyOffTimeover(int PIOTimeoverLevel)
{
    return m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nReady_OFF_Time;
}

/**
@brief	 PIO 신호 중 Load Request 신호가 Off 될때까지 기다리는 Timeover 셋팅 값이 얼마인지 확인하는 함수
@author	 LSJ
@date	 2012.10.29
@param   PIOTimeoverLevel : PIO 통신 시 TimeOver 값을 얼마로 설정할지 결정하는 변수
@return	 Load Request Timeover 값 리턴.
*/
int EQPIO::GetLREQOffTimeover(int PIOTimeoverLevel)
{
	return m_pParamSet->m_TIMEOVER_EQPIO[PIOTimeoverLevel].nLREQ_OFF_Time;
}

/**
@brief	 SequenceForMoving 함수에서 사용하는 m_nStep 값이 얼마인지 확인하는 함수
@author	 LSJ
@date	 2013.02.28
@return	 SequenceForMoving 함수에서 사용하는 m_nStep 값 리턴.
*/
int EQPIO::GetPIOStep()
{
	return m_nStep;
}

/**
@brief	 SequenceBeforeLoad 함수에서 사용하는 m_nBeforeLoadStep 값이 얼마인지 확인하는 함수
@author	 LSJ
@date	 2013.08.18
@return	 SequenceBeforeLoad 함수에서 사용하는 m_nBeforeLoadStep 값 리턴.
*/
int EQPIO::GetLoadPIOStep()
{
	return m_nBeforeLoadStep;
}

/**
@brief	 SequenceBeforeUnload 함수에서 사용하는 m_nBeforeUnloadStep 값이 얼마인지 확인하는 함수
@author	 LSJ
@date	 2013.08.18
@return	 SequenceBeforeUnload 함수에서 사용하는 m_nBeforeUnloadStep 값 리턴.
*/
int EQPIO::GetUnloadPIOStep()
{
	return m_nBeforeUnloadStep;
}

/**
@brief	 SequenceAfterLoad 함수에서 사용하는 m_nAfterLoadStep 값이 얼마인지 확인하는 함수
@author	 LSJ
@date	 2013.08.18
@return	 SequenceAfterLoad 함수에서 사용하는 m_nAfterLoadStep 값 리턴.
*/
int EQPIO::GetAfterLoadPIOStep()
{
	return m_nAfterLoadStep;
}


/**
@brief	 SequenceAfterUnload 함수에서 사용하는 m_nAfterUnloadStep 값이 얼마인지 확인하는 함수
@author	 LSJ
@date	 2013.08.18
@return	 SequenceAfterUnload 함수에서 사용하는 m_nAfterUnloadStep 값 리턴.
*/
int EQPIO::GetAfterUnloadPIOStep()
{
	return m_nAfterUnloadStep;
}

#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V81S) ||(OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE)||(OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86) || (OHT_NAME == OHT_NAME_P4_MASK))

void EQPIO::Init_SequenceForRFPIOConnection()
{
	m_nSetRFPIOStep = 1;
	nDuplicated_CheckCount= 0;
}

int EQPIO::GetRFPIOConnectStep()
{
	return m_nSetRFPIOStep;
}

/**
@brief	 RFPIO Data의 IO를 설정하는 함수
@brief	 LimJY_20160122 : For IR+RF PIO UINT nPIOType 추가
@author	 JSH
@date	 2014.07.21
@return	 에러값 리턴
*/
int EQPIO::Set_RFPIO_Data_RS232_Hybrid(UINT nPIOType, UINT nRFID, UINT nRFCHA, UINT nRFCS, char* sOHTName, bool bPreTrans)
{
	int nReturnError = NO_ERR;
    int nError = NO_ERR;
    static DWORD dwStartTime=0;
	static int nRetry=0;
	static int nDelay=0;
    static int nErrNum=NO_ERR;
	static int nReturnRFPIOStep=0;

	m_nCmdPIOType = nPIOType;     //LimJY_20160122 : For IR+RF PIO
	m_nCmdID = nRFID;
	m_nCmdChannel = nRFCHA;
	m_nCmdCS = nRFCS;

//	PIO_LOG("[Set_RFPIO_Data_RS232_Hybrid] m_nCmdID: %d, nRFID: %d", m_nCmdID, nRFID);

//	int nTempChannel = Load_EQPIO24G(m_nCmdID);

	//m_nCmdCS = 0;    //평택은 CS 0만 사용함.(류건진 책임 확인된 내용)

//	m_nCmdOHTNo = 1;       //LimJY_20160216 : 고정 OHT No 1.
	memmove(m_nCmdOHTNo,sOHTName, sizeof(m_nCmdOHTNo));

	switch (m_nSetRFPIOStep)
    {
	case 1: // Go Signal On Check & PIO Type Setting
		//test log
//		PIO_LOG("1. PIO Info = M:%01d, A:%06d, N:%01d, O:%06d",
//				m_nCmdPIOType, m_nCmdID, m_nCmdCS,m_nCmdOHTNo);


		PIO_LOG("1. PIO Info = M:%01d, A:%06d, N:%01d, O:%s %s",
				m_nCmdPIOType, m_nCmdID, m_nCmdCS,m_nCmdOHTNo,sOHTName);


		nError = Set_HybridPIO_Type();
		if(nError != NO_ERR) { nErrNum = nError; }

		if(nError == NO_ERR)
		{
			nDelay=0;
			m_nSetRFPIOStep = 2;
		}
		else
		{
			nReturnRFPIOStep = m_nSetRFPIOStep;
			m_nSetRFPIOStep = 3;
		}
		break;

    case 2: // PIO Setting Confirm
        //test log
		PIO_LOG("2. Confirm");
		nError = Set_HybridPIO_Confirm();
        if(nError != NO_ERR) { nErrNum = nError; }

        if(nError == NO_ERR)
        {
            nDelay = 0;
            m_nSetRFPIOStep = 4;
        }
        else
        {
            nReturnRFPIOStep = m_nSetRFPIOStep;
            m_nSetRFPIOStep = 3;
        }
        break;
    case 3: // PIO Setting Retry or NG
        if(nRetry > 0)
        {
            //test log
            PIO_LOG("3. PIO Setting NG, ReturnRFPIOStep = %d", nReturnRFPIOStep);

            m_nSetRFPIOStep = 6;
        }
        else
        {
            //test log
            PIO_LOG("3. PIO Setting Retry, ReturnRFPIOStep = %d", nReturnRFPIOStep);

            nRetry++;
            m_nSetRFPIOStep = nReturnRFPIOStep;
        }
        break;
    case 4:
		if(IsGoSignalOn() == true)
		{
            //test log
            PIO_LOG("4. Go Signal : ON");

			dwStartTime = 0;
			m_nSetRFPIOStep = 7;
		}
		else
		{
			if(nDelay > 60)
			{
				// nDelay Parameter화 필요
				int PSTB_DelayCnt = m_pParamSet->nPSTB_GO_OFF_Time / 9;

				if(bIsPSTB == true && nDelay < PSTB_DelayCnt) //PSTB PIO에 연결시 30초가 되지 않은 경우
				{
					nDelay++;

					if(bPreTrans == true)
						nDelay = 0;

					break;
				}

				dwStartTime = timeGetTime();

				if(nRetry > 0)
                {
                    //test log
		            PIO_LOG("4. Go Signal : NG");

					nErrNum = ERR_PIO_GO_OFF;
					nReturnError = nErrNum;
					nDelay=0;
					nRetry = 0;
					dwStartTime=0;
                    m_nSetRFPIOStep = 6;
				}
                else
				{
                    //test log
		            PIO_LOG("4. Retry = %d, Delay = %d", nRetry, nDelay);

					nRetry++;
					nErrNum = ERR_PIO_GO_OFF;
					nDelay=0;
                    dwStartTime = timeGetTime();
					m_nSetRFPIOStep = 5;
				}
            }
            else
            {
				nDelay++;

				if(bIsPSTB == true && bPreTrans == true)
					nDelay = 0;
            }
		}
        break;
    case 5:
        if(IsGoSignalOn() == true)
        {
            //test log
            PIO_LOG("5. Go Signal : ON");

			dwStartTime = 0;
			m_nSetRFPIOStep = 7;
		}
		else if(bPreTrans ==true)
		{
			PIO_LOG("5. Go Signal : PreTransing");
			dwStartTime = timeGetTime();
        }
		else if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 1000)
		{
				//test log
				PIO_LOG("5. Retry = %d", nRetry);

				dwStartTime = 0;
				m_nSetRFPIOStep = 6;
		}

        break;
    case 6:
		if((nError == NO_ERR) && (nRetry == 0))
        {
            //test log
			PIO_LOG("6. Retry = %d", nRetry);

			nRetry++;
			nErrNum = ERR_PIO_GO_OFF;
            nDelay=0;
            m_nSetRFPIOStep = 1;
		}
		else
		{
			//test log
//			PIO_LOG("6-E. Retry = %d", nRetry);

			PIODisable(false);

			// PIO Error 발생
			nDelay=0;
			nRetry=0;
			m_nCmdID=0;                 //임준영책임님한테 내용 확인 필요
			m_nCmdChannel=0;            //임준영책임님한테 내용 확인 필요
			m_nCmdCS=0;                 //임준영책임님한테 내용 확인 필요
			m_bRFPIO_Setting_Start=false;
			m_bSettingComplete=false;
			nReturnError = nErrNum;    //임준영책임님한테 내용 확인 필요
			dwStartTime = 0;
			m_nSetRFPIOStep = 0;       //임준영책임님한테 내용 확인 필요
			nErrNum=NO_ERR;
			PIO_LOG("6-E. Retry = %d, ReturnError : %d", nRetry, nReturnError);
		}
		break;
	case 7:
        //test log
		PIO_LOG("7. Success");

		PIO_LOG("[PARAM UPDATE] m_TIMEOVER_EQPIO[0].nLREQ_ON_Time = %d", m_pParamSet->m_TIMEOVER_EQPIO[0].nLREQ_ON_Time);
        nDelay=0;
        nRetry=0;
		nErrNum=NO_ERR;
		m_bRFPIO_Setting_Start=false;
		m_bSettingComplete=true;
		m_nSetRFPIOStep = 8;
        break;
    }

    return nReturnError;
}

/**
@brief	 RFPIO Data의 IO를 설정하는 함수
@author	 JSH
@date	 2014.07.21
@return	 에러값 리턴
*/
int EQPIO::Set_RFPIO_Data_RS232_RF(UINT nRFID, UINT nRFCHA, UINT nRFCS)
{
    int nReturnError = NO_ERR;
    int nError = NO_ERR;
    static DWORD dwStartTime=0;

    /*
   if((g_nPreTransNode==gm_OHTStatus->RFPIO->nCmdID) &&
	  (gm_OHTStatus->IOInput->nEQPIO_GO_Signal==ON))
   {
	  gm_OHTStatus->RFPIO->bRFPIO_Setting_Start=false;
	  gm_OHTStatus->RFPIO->bSettingComplete=true;
	  return nError;
   }
*/
	m_nCmdID = nRFID;
	m_nCmdChannel = nRFCHA;
	m_nCmdCS = nRFCS;

	switch (m_nSetRFPIOStep)
	{
	case 1: // Go Signal On Check
		//test log
		PIO_LOG("1");
		nError = Set_RFPIO_IDCHANNEL();

		if((nError==ERR_RFPIO_ID_DATAFORMAT)||(nError==ERR_RFPIO_CHA_DATAFORMAT))
		{
			m_nSetRFPIOStep = 2;
		}
		else if(nError == NO_ERR)
			m_nSetRFPIOStep = 7;
		else
			m_nSetRFPIOStep = 3;
		break;
	case 2:
		//test log
		PIO_LOG("2");
		if(IsGoSignalOn() == true)
		{
			dwStartTime = 0;
			m_nSetRFPIOStep = 7;
		}
		else
		{
			PIODisable(false);
			Set_RFPIO_Reset();
			dwStartTime = timeGetTime();
			m_nSetRFPIOStep = 5;
		}
		break;
	case 3:

		//test log
		PIO_LOG("3");
		PIODisable(false);
		dwStartTime  = timeGetTime();
		m_nSetRFPIOStep = 4;
		break;
	case 4:
		//test log
		PIO_LOG("4");
		if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 100)
		{
			dwStartTime = 0;
			m_nSetRFPIOStep = 6;
		}
		break;
	case 5:
		//test log
		PIO_LOG("5");
		if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 1000)
		{
			nError = Set_RFPIO_IDCHANNEL();
			if((nError==ERR_RFPIO_ID_DATAFORMAT)||(nError==ERR_RFPIO_CHA_DATAFORMAT))
			{
				dwStartTime = timeGetTime();
				m_nSetRFPIOStep = 3;
			}
			else if(nError == NO_ERR)
				m_nSetRFPIOStep = 7;
		}
		break;
	case 6:
		//test log
		PIO_LOG("6");
		nError = Set_RFPIO_IDCHANNEL();
		if(nError == NO_ERR)
			m_nSetRFPIOStep = 7;
		else
		{
			//test log
			PIO_LOG("6-1");
			// PIO Error 발생
			m_nCmdID=0;
			m_nCmdChannel=0;
			m_nCmdCS=0;
			m_bRFPIO_Setting_Start=false;
			m_bSettingComplete=false;
			nReturnError = nError;
			m_nSetRFPIOStep = 0;
			dwStartTime = 0;
		}
		break;

	case 7:
		//test log
		PIO_LOG("7");
		m_bRFPIO_Setting_Start=false;
		m_bSettingComplete=true;
		m_nSetRFPIOStep = 8;
		break;
	}

    return nReturnError;
}

/**
@brief	 1. 혼용 PIO의 Type을 설정하는 함수 (0:IR, 1:RF, 2:Hybrid 5.0GHz)
@author	 LimJY_20160122 : For IR+RF PIO
@date	 2016.01.22
@return	 에러값 리턴
*/
//int EQPIO::Set_RFPIO_TYPE(void)
//{
//    int nError=0;
//    AnsiString strTmp="", strSum="";
//    AnsiString strTmpCmd, strsndCmd;
//	AnsiString strSnd_Compare, strRcv_Compare;
//
//    if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Left_Select == OFF))
//    {
//        nError = PIODisable(false);
//    }
//
//    if(nError==NO_ERR)
//    {
//        // 대문자 %X, 소문자 %x
//        strTmp.sprintf("M=%01d", m_nCmdPIOType);
//        strSum = CheckSum_StringToHex(strTmp);
//		strTmpCmd.sprintf("<%s%s>",strTmp, strSum);
//		strSnd_Compare.sprintf("[%s%s]",strTmp, strSum);
//
//        //test log
//		PIO_LOG("<M=%01d%s>",m_nCmdPIOType, strSum);
//
//		nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);
//
//		//test log
//		PIO_LOG("%s", strRcv_Compare);
//    }
//
//    if(nError==NO_ERR)
//    {
//		if(strSnd_Compare != strRcv_Compare)
//        {
//            //test log
//			PIO_LOG("<%s%s> : %s",strTmp, strSum, strSnd_Compare);
//
//			nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);
//
//            if(nError==NO_ERR)
//            {
//                if(strSnd_Compare != strRcv_Compare)
//                {
//                    nError = ERR_RFPIO_WRITE;
//                    PIO_LOG("[E%d] Snd : %s,  Rcv : %s", nError, strSnd_Compare, strRcv_Compare);
//					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
//									ERR_RFPIO_WRITE,
//									m_pOHTMainStatus->StatusCommon.CurNodeID,
//									m_pExecuteInfo->ExecutePositionInfo.NextNode,
//									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//									m_pOHTMainStatus->StatusCommon.ExistFoup,
//									m_pOHTMainStatus->StatusCommon.StopStationID);
//                }
//            }
//        }
//    }
//
//    return nError;
//}


/**
@brief	 1. 혼용 PIO의 Channel을 설정하는 함수
@author	 LimJY_20160122 : For IR+RF PIO
@date	 2016.01.22
@return	 에러값 리턴
*/
//int EQPIO::Set_RFPIO_CHANNEL(void)
//{
//    int nError=0;
//    int nFreq=0, nChannel=0;
//    AnsiString strTmp="", strSum="";
//    AnsiString strTmpCmd, strsndCmd;
//	AnsiString strSnd_Compare, strRcv_Compare;
//
//    if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Left_Select == OFF))
//    {
//        nError = PIODisable(false);
//    }
//
//    if(nError==NO_ERR)
//    {
//        nChannel = EQPIO_BASE_CHANNEL + m_nCmdChannel;
//
//        // 대문자 %X, 소문자 %x
//        strTmp.sprintf("C=%03d", nChannel);
//        strSum = CheckSum_StringToHex(strTmp);
//        strTmpCmd.sprintf("<%s%s>",strTmp, strSum);
//		strSnd_Compare.sprintf("[%s%s]",strTmp, strSum);
//
//        //test log
//		PIO_LOG("<C=%d%s>, nFreq=%d, m_nCmdChannel=%d, m_nCmdID=%X",
//            nChannel, strSum, nFreq, m_nCmdChannel, m_nCmdID);
//
//		nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);
//
//		//test log
//		PIO_LOG("%s", strRcv_Compare);
//    }
//
//    if(nError==NO_ERR)
//    {
//		if(strSnd_Compare != strRcv_Compare)
//        {
//            //test log
//			PIO_LOG("<%s%s> : %s",strTmp, strSum, strSnd_Compare);
//
//			nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);
//
//            if(nError==NO_ERR)
//			{
//				if(strSnd_Compare != strRcv_Compare)
//				{
//					nError = ERR_RFPIO_CHA_DATAFORMAT;
//					PIO_LOG("[E%d] Snd : %s,  Rcv : %s", nError, strSnd_Compare, strRcv_Compare);
//					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
//									ERR_RFPIO_CHA_DATAFORMAT,
//									m_pOHTMainStatus->StatusCommon.CurNodeID,
//									m_pExecuteInfo->ExecutePositionInfo.NextNode,
//									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//									m_pOHTMainStatus->StatusCommon.ExistFoup,
//									m_pOHTMainStatus->StatusCommon.StopStationID);
//				}
//			}
//        }
//    }
//
//    return nError;
//}


/**
@brief	 1. 혼용 PIO의 ID를 설정하는 함수
@author	 LimJY_20160122 : For IR+RF PIO
@date	 2016.01.22
@return	 에러값 리턴
*/
//int EQPIO::Set_RFPIO_ID(void)
//{
//    int nError=0;
//    AnsiString strTmp="", strSum="";
//    AnsiString strTmp_Compare="", strSum_Compare="";
//    AnsiString strTmpCmd, strsndCmd;
//	AnsiString strSnd_Compare, strRcv_Compare;
//
//    if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Left_Select == OFF))
//    {
//        nError = PIODisable(false);
//    }
//
//    if(nError==NO_ERR)
//    {
//        // 대문자 %X, 소문자 %x
//        strTmp.sprintf("A=%06X", m_nCmdID);
//        strTmp_Compare.sprintf("A=569A-%06X", m_nCmdID);
//        strSum = CheckSum_StringToHex(strTmp);
//        strSum_Compare = CheckSum_StringToHex(strTmp_Compare);
//		strTmpCmd.sprintf("<%s%s>",strTmp, strSum);
//		strSnd_Compare.sprintf("[%s%s]",strTmp_Compare, strSum_Compare);
//
//        //test log
//		PIO_LOG("<%s%s>",strTmp, strSum);
//
//		nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);
//
//		//test log
//		PIO_LOG("%s", strRcv_Compare);
//    }
//
//    if(nError==NO_ERR)
//    {
//		if(strSnd_Compare != strRcv_Compare)
//        {
//            //test log
//			PIO_LOG("<%s%s> : %s",strTmp_Compare, strSum_Compare, strSnd_Compare);
//
//			nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);
//
//            if(nError==NO_ERR)
//			{
//				if(strSnd_Compare != strRcv_Compare)
//				{
//					nError = ERR_RFPIO_ID_DATAFORMAT;
//					PIO_LOG("[E%d] Snd : %s,  Rcv : %s", nError, strSnd_Compare, strRcv_Compare);
//					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
//									ERR_RFPIO_ID_DATAFORMAT,
//									m_pOHTMainStatus->StatusCommon.CurNodeID,
//									m_pExecuteInfo->ExecutePositionInfo.NextNode,
//									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//									m_pOHTMainStatus->StatusCommon.ExistFoup,
//									m_pOHTMainStatus->StatusCommon.StopStationID);
//				}
//			}
//        }
//    }
//
//    return nError;
//}

/**
@brief	 1. 혼용 PIO의 CS를 설정하는 함수
@author	 LimJY_20160122 : For IR+RF PIO
@date	 2016.01.22
@return	 에러값 리턴
*/
//int EQPIO::Set_RFPIO_CS(void)
//{
//    int nError=0;
//    AnsiString strTmp="", strSum="";
//    AnsiString strTmpCmd, strsndCmd;
//	AnsiString strSnd_Compare, strRcv_Compare;
//
//    if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Left_Select == OFF))
//    {
//        nError = PIODisable(false);
//    }
//
//    if(nError==NO_ERR)
//    {
//        // 대문자 %X, 소문자 %x
//        strTmp.sprintf("N=%d", m_nCmdCS);
//        strSum = CheckSum_StringToHex(strTmp);
//		strTmpCmd.sprintf("<%s%s>",strTmp, strSum);
//		strSnd_Compare.sprintf("[%s%s]",strTmp, strSum);
//
//        //test log
//		PIO_LOG("<N=%d%s>",m_nCmdCS, strSum);
//
//		nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);
//
//		//test log
//		PIO_LOG("%s", strRcv_Compare);
//    }
//
//    if(nError==NO_ERR)
//    {
//		if(strSnd_Compare != strRcv_Compare)
//        {
//            //test log
//			PIO_LOG("<%s%s> : %s",strTmp, strSum, strSnd_Compare);
//
//			nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);
//
//			if(nError==NO_ERR)
//            {
//                if(strSnd_Compare != strRcv_Compare)
//                {
//                    nError = ERR_RFPIO_WRITE;
//                    PIO_LOG("[E%d] Snd : %s,  Rcv : %s", nError, strSnd_Compare, strRcv_Compare);
//					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
//									ERR_RFPIO_WRITE,
//									m_pOHTMainStatus->StatusCommon.CurNodeID,
//									m_pExecuteInfo->ExecutePositionInfo.NextNode,
//									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//									m_pOHTMainStatus->StatusCommon.ExistFoup,
//									m_pOHTMainStatus->StatusCommon.StopStationID);
//                }
//            }
//        }
//    }
//
//    return nError;
//}


/**
@brief	 1. 혼용 PIO의 OHT호기 번호 설정하는 함수
@author	 LimJY_20160122 : For IR+RF PIO
@date	 2016.01.22
@return	 에러값 리턴
*/
//int EQPIO::Set_RFPIO_OHT_No(void)
//{
//	int nError=0;
//	AnsiString strTmp="", strSum="";
//	AnsiString strTmpCmd, strsndCmd;
//	AnsiString strSnd_Compare, strRcv_Compare;
//
//	if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Left_Select == OFF))
//	{
//		nError = PIODisable(false);
//	}
//
//    if(nError==NO_ERR)
//	{
//		// 대문자 %X, 소문자 %x
////        strTmp.sprintf("O=%06d", m_nCmdOHTNo);
//		strTmp.sprintf("O=%s", m_nCmdOHTNo);
//		strSum = CheckSum_StringToHex(strTmp);
//		strTmpCmd.sprintf("<%s%s>",strTmp, strSum);
//		strSnd_Compare.sprintf("[%s%s]",strTmp, strSum);
//
//		//test log
////		PIO_LOG("<O=%d%s>",m_nCmdOHTNo, strSum);
//		PIO_LOG("<O=%s%s>",m_nCmdOHTNo, strSum);
//
//		nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);
//
//		//test log
//		PIO_LOG("%s", strRcv_Compare);
//	}
//
//	if(nError==NO_ERR)
//	{
//		if(strSnd_Compare != strRcv_Compare)
//		{
//			//test log
//			PIO_LOG("<%s%s> : %s",strTmp, strSum, strSnd_Compare);
//
//			nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);
//
//			if(nError==NO_ERR)
//            {
//				if(strSnd_Compare != strRcv_Compare)
//                {
//                    nError = ERR_RFPIO_WRITE;
//					PIO_LOG("[E%d] Snd : %s,  Rcv : %s", nError, strSnd_Compare, strRcv_Compare);
//					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
//									ERR_RFPIO_WRITE,
//									m_pOHTMainStatus->StatusCommon.CurNodeID,
//									m_pExecuteInfo->ExecutePositionInfo.NextNode,
//									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
//									m_pOHTMainStatus->StatusCommon.ExistFoup,
//									m_pOHTMainStatus->StatusCommon.StopStationID);
//                }
//			}
//        }
//    }
//
//	return nError;
//}


/**
@brief	 1. 혼용 PIO의 설정확인하는 함수
@author	 LimJY_20160122 : For IR+RF PIO
@date	 2016.01.22
@return	 에러값 리턴
*/
int EQPIO::Set_HybridPIO_Confirm(void)
{
//    int nError = PIOEnable();
//    return nError;

	int nError=NO_ERR;

	AnsiString strTmpCmd, strRcv_Compare;

	if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Left_Select == OFF))
	{
		nError = PIODisable(false);
	}

	if(nError==NO_ERR)
	{
		strRcv_Compare = "";
		strTmpCmd.sprintf("<Y59>");

		//test log
		PIO_LOG("<%s>",strTmpCmd);

		nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);

		//test log
		PIO_LOG("%s", strRcv_Compare);
	}

	if(nError==NO_ERR)
	{
		nError = PIOEnable();
	}

	return nError;

}

/**
@brief	 RFPIO의 채널을 설정하는 함수
@author	 JSH
@date	 2014.07.21
@return	 에러값 리턴
*/
 int EQPIO::Set_RFPIO_IDCHANNEL(void)
{
	int nError=0;
	AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare, strRcv_Compare;

	//if( m_pCommPort != NULL)
	//	PurgeComm(m_pCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);

	nError = PIODisable(false);
	if(nError==NO_ERR)
	{
		// 대문자 %X, 소문자 %x
		strTmpCmd.sprintf("<B=569A-%06X:%d>",m_nCmdID, m_nCmdChannel);
		strSnd_Compare.sprintf("%06X:%d]",m_nCmdID, m_nCmdChannel);

		//test log
		PIO_LOG("<B=569A-%06X:%d>",m_nCmdID, m_nCmdChannel);

		nError = Get_RFPIO_Response_RF(strTmpCmd, strRcv_Compare);

		//test log
		PIO_LOG("%s", strRcv_Compare);
	}

	if(nError==NO_ERR)
	{
		if(strSnd_Compare != strRcv_Compare)
		{
			//test log
			PIO_LOG("<B=569A-%06X:%d>",m_nCmdID, m_nCmdChannel);

			nError = Get_RFPIO_Response_RF(strTmpCmd, strRcv_Compare);

			//test log
			PIO_LOG("%s", strRcv_Compare);

			if(nError==NO_ERR)
			{
				if(strSnd_Compare != strRcv_Compare)
				{
					nError = ERR_RFPIO_ID_DATAFORMAT;
					PIO_LOG("[E%d] Snd : %s,  Rcv : %s", nError, strSnd_Compare,strRcv_Compare);
					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
									ERR_RFPIO_ID_DATAFORMAT,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
				}
			}
		}
	}

	//ID, Channel 셋팅전에 enable시키면 [M]이라는게 한번 올라옴 (이게 문제될수 있음)
	//ID, Channel 셋팅후 3초 이내에 enable시켜야 함.
	if(nError==NO_ERR)
	{
		nError = PIOEnable();
	}

	return nError;
}

int EQPIO::Set_RFPIO_Reset()
{
	int nError=0;
	AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare, strRcv_Compare;


	nError = PIODisable(false);
	if(nError==NO_ERR)
	{
		// 대문자 %X, 소문자 %x
		strTmpCmd.sprintf("<RESET>");
		strSnd_Compare.sprintf("[RESET]");

		//test log
		PIO_LOG("<Reset>");

		nError = Get_RFPIO_Response_RF(strTmpCmd, strRcv_Compare);

		//test log
		PIO_LOG("%s", strRcv_Compare);
	}

	if(nError==NO_ERR)
	{
		if(strSnd_Compare != strRcv_Compare)
		{
			//test log
			nError = Get_RFPIO_Response_RF(strTmpCmd, strRcv_Compare);

			//test log
			PIO_LOG("%s", strRcv_Compare);
		}
	}
	return nError;
}


//int EQPIO::Load_EQPIO24G(UINT CurStationID)
//{
//	int nResult = 0;
//	int nTmpValue = 0;
//
//	char strStationID[20] = {0,};
//
//	sprintf(strStationID, "%u",CurStationID);
////	sprintf(strPortType, "%d",PortType);
//
//
//	LIniFile *pIniFile;
//	pIniFile = NULL;
//	pIniFile = new LIniFile("DataFiles\\EQPIO24.txt");
//
//	if(!pIniFile->GetIniFileExist())
//	{
//        delete pIniFile;
//		pIniFile = NULL;
//	}
//
//	if(pIniFile != NULL)
//	{
//
//		pIniFile->ReadInteger("EQPIO24G", strStationID, 0,&nTmpValue);
//		nResult = nTmpValue;
//		delete pIniFile;
//	}
//
// //	PIO_LOG("[Load_EQPIO24G] CurStationID :%d, nResult : %d", CurStationID, nResult);
//
//	return  nResult;
// }

int EQPIO::Set_HybridPIO_Type()
{
    int nError=0;
    int nChannel=0;
    AnsiString strTmp="", strSum="";
    AnsiString strTmp_Compare="", strSum_Compare="";
    AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare, strRcv_Compare;

    if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Left_Select == OFF))
    {
        nError = PIODisable(false);
    }

    if(nError==NO_ERR)
    {
		if(m_nCmdPIOType == EQ_PIO_IR)
		{
			//대문자 %X, 소문자 %x
			strTmp.sprintf("BC=%01d:000000:000:0:%s", TYPE_PIO_IR,m_nCmdOHTNo);
			strTmp_Compare.sprintf("BC=%01d:569A-000000:000:0:%s", TYPE_PIO_IR,m_nCmdOHTNo);
        }
		else
		{
			nChannel = m_nCmdChannel;
			if(m_nCmdPIOType != EQ_PIO_HYBRID_SN)
			{
				strTmp.sprintf("BC=%01d:%06d:%03d:%01d:%s",
					TYPE_PIO_HYBRID, m_nCmdID, nChannel, 0, m_nCmdOHTNo);
				strTmp_Compare.sprintf("BC=%01d:569A-%06d:%03d:%01d:%s",
					TYPE_PIO_HYBRID, m_nCmdID, nChannel, 0, m_nCmdOHTNo);
			}else
			{
				// 시리얼 설정하여 사용할 부분
 				strTmp.sprintf("BC=%01d:%06x:%03d:%01d:%s",
					TYPE_PIO_HYBRID, m_nCmdID, nChannel, 0, m_nCmdOHTNo);
				strTmp_Compare.sprintf("BC=%01d:569A-%06x:%03d:%01d:%s",
					TYPE_PIO_HYBRID, m_nCmdID, nChannel, 0, m_nCmdOHTNo);
				strupr(strTmp_Compare.c_str());
			}

		}

        strSum = CheckSum_StringToHex(strTmp);
        strSum_Compare = CheckSum_StringToHex(strTmp_Compare);
		strTmpCmd.sprintf("<%s%s>",strTmp, strSum);
		strSnd_Compare.sprintf("[%s%s]",strTmp_Compare, strSum_Compare);

        //test log
		PIO_LOG("<%s%s>",strTmp, strSum);

		nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);

		//test log
		PIO_LOG("%s", strRcv_Compare);
    }

    if(nError==NO_ERR)
	{
		if(strSnd_Compare != strRcv_Compare)
        {
            //test log
			PIO_LOG("<%s%s> : %s",strTmp_Compare, strSum_Compare, strSnd_Compare);

			nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);

			if(nError==NO_ERR)
			{
				if(strSnd_Compare != strRcv_Compare)
				{
					nError = ERR_RFPIO_ID_DATAFORMAT;
					PIO_LOG("[E%d] Snd : %s,  Rcv : %s", nError, strSnd_Compare, strRcv_Compare);
					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
									ERR_RFPIO_ID_DATAFORMAT,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
				}
			}
        }
    }

    return nError;
}


/**
@brief	 RFPIO 통신 중복 감지 함수
@author	 puting
@date	 2025.02.7
@return	 에러값 리턴
*/
int EQPIO::Check_RFPIO_Response_Hybrid_Duplicated(bool bCheckCount)
{
    int nError=0;
    int nChannel=0;
    AnsiString strTmp="", strSum="";
    AnsiString strTmp_Compare="", strSum_Compare="";
    AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare="", strRcv_Compare="",strRecv_Cheeck ="", strRecv_Data ="";
	BYTE CHeckDupli = 0x00;

//    if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Left_Select == OFF))
//    {
//        nError = PIODisable(false);
//    }

	if(bCheckCount ==true)
	{
		if(nDuplicated_CheckCount < m_pParamSet->nChatteringCount)
		{
			//일정 이상 헌팅 발생 시, 조건 확인 기능 보완(디폴트 0)
			// ex) 카운트 0, 기준 0 일경우 만족하지 않음(검사함)
		   // ex) 카운트 3, 기준 0일 경우, 만족하지 않음(검사함)
		   //ex) 카운트 2, 기준 5일 경우, 만족함.(검사하지 않음)
		   //ex) 카운트 7, 기준 5일 경우, 만족하지않음.(검사함)

			PIO_LOG("Duplicated_No Check %d, %d",nDuplicated_CheckCount, m_pParamSet->nChatteringCount);
			return nError;
		}
	}

	if(nError==NO_ERR)
	{

            // 대문자 %X, 소문자 %x
		strTmp.sprintf("G");
		strTmp_Compare.sprintf("[G=");



        strSum = CheckSum_StringToHex(strTmp);
		strSum_Compare = CheckSum_StringToHex(strTmp_Compare);
		strTmpCmd.sprintf("<%s%s>",strTmp, strSum);
		//strSnd_Compare.sprintf("[%s%s]",strTmp_Compare, strSum_Compare);

		//test log
		PIO_LOG("<%s%s>%d/%d/%d",strTmp, strSum, nDuplicated_CheckCount,m_pParamSet->nChatteringCount,bCheckCount);

		nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);

	}

    if(nError==NO_ERR)
	{
		strRecv_Cheeck = strRcv_Compare.SubString(0,3);
		strRecv_Data = strRcv_Compare.SubString(4,2);

		int TmepHexData =  (int)strtol(strRecv_Data.c_str(), NULL, 16); // hex 처리

		CHeckDupli = 0x00;
		CHeckDupli =  TmepHexData & 0x04;

		PIO_LOG("Step1_%s, %s<%d, %x>",strRecv_Cheeck, strRecv_Data, TmepHexData, TmepHexData);

		if(strTmp_Compare != strRecv_Cheeck)    //실패
        {
			//test log
			PIO_LOG("<%s, %s>",strTmp_Compare, strRecv_Cheeck);

			nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);

			if(nError==NO_ERR)
			{
				strRecv_Cheeck = strRcv_Compare.SubString(0,3);
				strRecv_Data = strRcv_Compare.SubString(4,2);

				int TmepHexData =  (int)strtol(strRecv_Data.c_str(), NULL, 16); // hex 처리

				CHeckDupli = 0x00;
				CHeckDupli =  TmepHexData & 0x04;

				PIO_LOG("Step2_%s, %s<%d, %x>",strRecv_Cheeck, strRecv_Data, TmepHexData, TmepHexData);

				if(strTmp_Compare != strRecv_Cheeck)
				{
					nError = ERR_RFPIO_ID_DATAFORMAT;
					PIO_LOG("[ERROR_Duplicated%d] Snd : %s,  Rcv : %s", nError, strSnd_Compare, strRcv_Compare);
					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
									ERR_RFPIO_ID_DATAFORMAT,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
				}else
				{
					//다중 접속 플래그 확인
					  if(CHeckDupli == 0x04)
					  {
							nError = ERR_ABORT_ON_ERROR;
							PIO_LOG("[ERROR_Duplicated%d] Dupli.Rcv : %d/%d", nError, nDuplicated_CheckCount, CHeckDupli);

					  }else
					  {
						  PIO_LOG("[OK_Duplicated] Dupli.Rcv : %d/%d", nDuplicated_CheckCount,CHeckDupli);
					  }
				}
			}
		}else
		{
		   //strRecv_Data  아스키 문자를 변환 Hex로 변환
		   //다중 접속 플래그 확인
		   if(CHeckDupli == 0x04)
		   {
				nError = ERR_ABORT_ON_ERROR;
				PIO_LOG("[ERROR_Duplicated%d]_2 Dupli.Rcv : %d/%d", nError, nDuplicated_CheckCount,CHeckDupli);
		   }else
		   {
               PIO_LOG("[OK_Duplicated]_2 Dupli.Rcv : %d/%d", nDuplicated_CheckCount,CHeckDupli);
           }
		}
	}

	return nError;
}
/**
@brief	 RFPIO 통신 음답 얻는 함수
@author	 JSH
@date	 2014.07.21
@return	 에러값 리턴
*/
int EQPIO::Get_RFPIO_Response_Hybrid(AnsiString strSndData, AnsiString &strRcvData)
{
	int nError=0;
	char ReadBuff[1024] = {0, };
	char TempBuff[1024] = {0, };

	strRcvData="";

	for(int i=0; i<2; i++)
	{
		if( m_pCommPort != NULL)
			PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		if(strSndData.Length() != (int)(m_pCommPort->WriteCommBlock(strSndData.c_str(),strSndData.Length())))
		{
			nError = ERR_RFPIO_WRITE;
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
									ERR_RFPIO_WRITE,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);

			PIO_LOG("[E%d] Snd : %s", nError, strSndData);
		}

//		Sleep(50);
//		Sleep(100);
//		Sleep(500); //PIO Read Time Out 에러 방지
		Sleep(m_nRespSleep);

		if(nError==NO_ERR)
		{
			//if(ReadData(ReadBuff, ']') == 0)
			if(m_pCommPort->ReadUntil(ReadBuff, ']',true) == 0)
			{
				PIO_LOG("[E%d] SendBuff %s ReadBuff: %s", nError, strSndData, ReadBuff);
				Sleep(100);
				m_pCommPort->ReadCommBlock(TempBuff,1024);    //쓰레기 데이터 삭제
				memset(&ReadBuff,0x00,1024);
				if(i==0)
				{
					m_nRespSleep = 500;
					continue;
				}
				nError = ERR_RFPIO_READTIMEOUT;
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
									ERR_RFPIO_READTIMEOUT,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
			}
			else
			{
				PIO_LOG("Snd: %s ReadBuff: %s",strSndData, ReadBuff);
				strRcvData = ReadBuff;
				m_nRespSleep = 100;
				break;
			}
		}
	}

	return nError;
}

int EQPIO::Get_RFPIO_Response_RF(AnsiString strSndData, AnsiString &strRcvData)
{
	int nError=0;
	char ReadBuff[1024] = {0, };

	strRcvData="";

	if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	if(strSndData.Length() != (int)(m_pCommPort->WriteCommBlock(strSndData.c_str(),strSndData.Length())))
	{
		nError = ERR_RFPIO_WRITE;
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
									ERR_RFPIO_WRITE,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);

		PIO_LOG("[E%d] Snd : %s", nError, strSndData);
	}

	Sleep(100);

	if(nError==NO_ERR)
	{
		//if(ReadData(ReadBuff, ']') == 0)
		if(m_pCommPort->ReadUntil(ReadBuff, ']',true) == 0)
		{
			nError = ERR_RFPIO_READTIMEOUT;
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
									ERR_RFPIO_READTIMEOUT,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);

			PIO_LOG("[E%d] ReadBuff : %s", nError, ReadBuff);
		}
		else
		{
			int nPos;

			PIO_LOG("Snd:%s Rcv:%s",strSndData, ReadBuff);

			//명령 <B=123456:X38>  비교data 123456:X38]
			//응답 [B=890-123456:X38] 비교data 123456:X38]
			strRcvData = ReadBuff;
			strRcvData=strRcvData.Trim();
			nPos = strRcvData.Pos("-");
			strRcvData.Delete(1, nPos);
		}
	}

	return nError;
}

int EQPIO::ReadData(char *RtnReadBuff, char cTarget)
{
    int i, index = 0;
    char chBuff;


	for(i=0; i<255; i++)
    {
		if(m_pCommPort == NULL)
		{
			index=0;
			break;
		}
		if(m_pCommPort->ReadCommBlock(&chBuff,1)==0)  // One Byte Reading
		{
			if(m_pCommPort != NULL) PurgeComm(m_pCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
				index=0;
			break;
		}
		*(RtnReadBuff + index ) = chBuff;
		index++;
		if(chBuff == cTarget) break;
	}
	return index;
}

/**
@brief   EQPIO ComPort Open 수행
@author  JSH
@date    2014.07.21
@return  Open 하는데 문제 없으면 0, 문제가 발생하면 에러 코드 리턴
*/
int EQPIO::RFPIO_Open()
{
	int nError=NO_ERR;
	if(m_pCommPort==NULL)
	{
		m_pCommPort = new Comm_Port(m_nPortNum, EQPIO_BAUDRATE);

		if(m_pCommPort==NULL)
		{
			nError=ERR_RFPIO_OPEN;
		}
		//test log
//		PIO_LOG("PIO OPEN nError = %d", nError);
	}
	return nError;
}

/**
@brief   EQPIO ComPort Close 수행
@author  JSH
@date    2014.07.21
@return  Open 하는데 문제 없으면 0, 문제가 발생하면 에러 코드 리턴
*/
//void EQPIO::RFPIO_Close()
//{
//	if(m_pCommPort!=NULL)
//    {
//		delete m_pCommPort;
//		m_pCommPort = NULL;
//	}
//}


/**
@brief	 혼용 PIO Send Command Checksum 계산
@author	 LimJY_20160122 : For IR+RF PIO
@date	 2016.01.22
@param   CheckSum 계산 String
@return	 Hexa String
*/
AnsiString EQPIO::CheckSum_StringToHex(AnsiString sData)
{
    unsigned char cVal, checksum = 0;
    AnsiString sRv = "";

    for(int i=0; i<sData.Length(); i++)
    {
        cVal = (int)sData[i+1];
        checksum += cVal;
        //checksum += (int)(sData.SubString(i,1).c_str());
    }

    sRv.sprintf("%02X", checksum);
    return sRv;
}

#endif

//void EQPIO::Reset_EQPIO(void)
//{
//
//	// EQPIO의 출력 초기화 및 연결 초기화
//	// Init()
//}

// ---------------------------------------------------------------------------
#define HPIO_LOG_NUM		100
#define HPIO_LOG_MAX_LEN	1024

int EQPIO::Save_HybridPIO_Data()
{
	int nError=0;
	TDateTime tdt;
	String strFileName = "D:\\log\\HPIOLogDump"+Now().FormatString("yyyymmddhh")+".log";

	ofstream fs(strFileName.c_str(),ios::out);
	char ReadBuff[HPIO_LOG_NUM][HPIO_LOG_MAX_LEN] = {0, };
	int bufSize[HPIO_LOG_NUM] = {0,};
	AnsiString strSnd, strRcv;

	String strTemp;
	if(fs.is_open())
	{
		if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Left_Select == OFF))
		{
		    nError = PIODisable(false);
		}

		if(nError==NO_ERR)
		{
			strSnd = "<L4C>";

			strRcv="";

			if( m_pCommPort != NULL)
				PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

			OHTMainForm->memoMain->Lines->Add("HPIO log request");

			if(strSnd.Length() != (int)(m_pCommPort->WriteCommBlock(strSnd.c_str(),strSnd.Length())))
			{
				OHTMainForm->memoMain->Lines->Add("PIO Write error!");
				nError = ERR_RFPIO_WRITE;
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
									ERR_RFPIO_WRITE,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
			}

			Sleep(2000);      // 사업부 백승혁 선임님 요청으로 500ms -> 2000ms로 변경함

			if(nError==NO_ERR)
			{
				for(int i=0; i<HPIO_LOG_NUM; i++){
					bufSize[i] = m_pCommPort->ReadCommBlock(ReadBuff[i], HPIO_LOG_MAX_LEN);
					fs.write(ReadBuff[i], bufSize[i]);
					if (bufSize[i] < HPIO_LOG_MAX_LEN) {
						OHTMainForm->memoMain->Lines->Add("PIO log save success");
						break;
					}
				}

			}
		}
		fs.close();
	}
	else
	{
		nError = ERR_MAP_FILEOPENFAIL;
	}
	return nError;
}

void EQPIO::Update_MCC()
{
	m_mccparam.uPIOOutput = 1 * m_pHWCommon->m_Output.EQPIO_Valid
						 + 2 * m_pHWCommon->m_Output.EQPIO_CS_0
						 + 4 * m_pHWCommon->m_Output.EQPIO_CS_1
						 + 8 * m_pHWCommon->m_Output.EQPIO_CS_2
						 + 16 * m_pHWCommon->m_Output.EQPIO_CS_3
						 + 32 * m_pHWCommon->m_Output.EQPIO_TR_Req
						 + 64 * m_pHWCommon->m_Output.EQPIO_Busy
						 + 128 * m_pHWCommon->m_Output.EQPIO_Complete;

#if(OHT_NAME == OHT_NAME_STD_V85)
	m_mccparam.uPIOInput = 1 * m_pHWCommon->m_Input.EQPIO_Load_REQ
						 + 2 * m_pHWCommon->m_Input.EQPIO_Unload_REQ
						 + 4 * m_pHWCommon->m_Input.EQPIO_Ready
						 + 8 * m_pHWCommon->m_Input.EQPIO_HO_AVBL
						 + 16 * m_pHWCommon->m_Input.EQPIO_ES
//						 + 32 * m_pHWCommon->m_Input.EQPIO_Right_Go
						 + 64 * m_pHWCommon->m_Input.EQPIO_Left_Go;
#elif(OHT_NAME == OHT_NAME_STD_V81S)
	m_mccparam.uPIOInput = 1 * m_pHWCommon->m_Input.EQPIO_Load_REQ
						 + 2 * m_pHWCommon->m_Input.EQPIO_Unload_REQ
						 + 4 * m_pHWCommon->m_Input.EQPIO_Ready
						 + 8 * m_pHWCommon->m_Input.EQPIO_HO_AVBL
						 + 16 * m_pHWCommon->m_Input.EQPIO_ES
//						 + 32 * m_pHWCommon->m_Input.EQPIO_Right_Go
						 + 64 * m_pHWCommon->m_Input.EQPIO_Left_Go;
#else
	m_mccparam.uPIOInput = 1 * m_pHWCommon->m_Input.EQPIO_Load_REQ
						 + 2 * m_pHWCommon->m_Input.EQPIO_Unload_REQ
						 + 4 * m_pHWCommon->m_Input.EQPIO_Ready
						 + 8 * m_pHWCommon->m_Input.EQPIO_HO_AVBL
						 + 16 * m_pHWCommon->m_Input.EQPIO_ES
						 + 32 * m_pHWCommon->m_Input.EQPIO_Right_Go
						 + 64 * m_pHWCommon->m_Input.EQPIO_Left_Go;
#endif
	switch(m_pOHTMainStatus->StatusCommon.CarrierType)
	{
		case CARRIER_NOTDEFINED:
			sprintf(m_mccparam.cCarrierType,"None");
			break;
		case CARRIER_FOUP:
			sprintf(m_mccparam.cCarrierType,"FOUP");
			break;
		case CARRIER_POD:
			sprintf(m_mccparam.cCarrierType,"POD");
			break;
		case CARRIER_EUV:
			sprintf(m_mccparam.cCarrierType,"EUV");
			break;
		case CARRIER_MAC:
			sprintf(m_mccparam.cCarrierType,"MAC");
			break;
		case CARRIER_FOSB:
			sprintf(m_mccparam.cCarrierType,"FOSB");
			break;
	}
}

// ---------------------------------------------------------------------------
//2023.01.03 sseung 버전관리 시스템 EQPIO 버전 요청 및 저장
int EQPIO::Get_EQPIO_Ver()
{
	int nError=NO_ERR;

	AnsiString strTmpCmd, strRcv_Compare;
	unsigned char c_Rtnarr[10] = {0,};

	if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Left_Select == OFF))
	{
		nError = PIODisable(false);
	}

	if(nError==NO_ERR)
	{
		strRcv_Compare = "";
		strTmpCmd.sprintf("<V56>");

		nError = Get_RFPIO_Response_Hybrid(strTmpCmd, strRcv_Compare);
	}

	if(nError==NO_ERR)
	{
		nError = PIOEnable();
		strcpy(c_Rtnarr, strRcv_Compare.c_str());

		OHTMainForm->Version_Info.EQPIO_ver.sprintf(strRcv_Compare.SubString(4, 4).c_str());
		OHTMainForm->Version_Info.EQPIO_Date.sprintf(OHTMainForm->strVersion_Date.c_str());
		PIO_LOG("Final ver = %s", OHTMainForm->Version_Info.EQPIO_ver);
	}

	return nError;
}
