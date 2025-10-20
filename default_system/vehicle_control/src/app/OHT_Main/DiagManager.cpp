//---------------------------------------------------------------------------


#pragma hdrstop

#include "OHTMain.h"
#include "Utility.h"
#include "DiagManager.h"
#include <dir.h>
#define ADD_LOG_DIAG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\DiagManager.log", __VA_ARGS__)
#define ADD_MD_LOG(...)    WRITE_LOG_UNIT(pLogMDetail, __VA_ARGS__)   //AOHC-253 Monitoring 항목 추가 2

#define ADD_PREVIOSDIAG_LOG(...)         WRITE_LOG_UNIT(pLogPreviosDiag, __VA_ARGS__)


DiagManager::DiagManager()
{

	m_DiagPatrolInfoList = new LList;
	m_DiagAbnormalDataList = new LList;
	m_nDiagAbnormalDrvDiffCnt = 0;
	m_nVHLWorkingTime = 0;
	m_nDiagCheckCount = 0;
	m_bIsLoadedDiagData = false;
	m_nDiagCheckTimerID = 0;
	m_nTimerRetryCount = 0;
	m_ParamChehckError = NO_ERR;

	pLogPreviosDiag = getLogUnit("D:\\log\\Monitor.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogMDetail = getLogUnit("D:\\log\\MonitoringDetail.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	memset(&m_DiagServiceData, 0, sizeof(SERVICE_DATA_DIAG));

	memset(&m_DiagStatusData, 0, sizeof(DIAG_OHT_STATUS_DATA));

	memset(&m_DiagParam, 0, sizeof(DIAG_PARAM));
	memset(&m_DiagEventFlag, 0, sizeof(DIAG_EVENT_REPORT));
	LoadDiagParamRecorvery("DataFiles\\DiagParam.ini",&m_DiagParam);

	mkdir("C:\\User\\OHT\\Diag");

	//Make Diag data
	m_DiagDataMap = new map<BYTE, DIAG_OHT_STATISTICS_DATA*>();
	m_DiagOverhaulDataMap = new map<BYTE, DIAG_OHT_STATISTICS_DATA*>();

	MakeStatDataMap();



	InitializeCriticalSection(&m_CSPreDiag);
	ResetPreviosDiagCount();

	//AOHC-196 사전 로그 추가
	memset(m_nArrDiagVal, 0, sizeof(m_nArrDiagVal));
	m_dwPrevLogStartTick = 0;
	// SHCHO 2019.03 : 사전로그 enum과 GetPrevIdxName() 인덱스가 맞지 않는 경우 미리 알려서 불쌍사를 막는다
	for(int i=0; i<COUNTOF_PREVIOUS ; i++)
	{
		char buf[32];
		memset(buf, 0, sizeof(buf));
		if(GetPrevIdxName((PREVIOS_DIAG_PARAM_NUM)i, buf) == false)
		{
			AnsiString msg;
			msg = msg.sprintf("[%s][Line:%d]\n모니터링 로그 PREVIOS_DIAG_PARAM_NUM 의 개수와 GetPrevIdxName(..) 함수의 인덱스가 맞지 않습니다.\n코드 수정을 해야합니다.\n프로그램은 종료됩니다.", __FILE__, __LINE__);
			
			wchar_t wmsg[1024];
			msg.WideChar(wmsg, sizeof(wmsg));
			Application->MessageBox(wmsg, L"Monitoring Log Error");
			exit(0);
		}
			
	}
	bCheckDiagParam = false;

}

DiagManager::~DiagManager()
{
	//타이머 가동중이었다면 종료
	if(m_nDiagCheckTimerID != NULL)
	{
		timeKillEvent(m_nDiagCheckTimerID);
	}

	//PatrolVHL 반납
	if(m_DiagPatrolInfoList != NULL)
	{
		int nDataCnt = m_DiagPatrolInfoList->getCount();

		for(int i=0; i < nDataCnt; i++)
		{
			SERVICE_DATA_DIAG* pDeleteData = (SERVICE_DATA_DIAG*)m_DiagPatrolInfoList->popFront();

			delete pDeleteData;
		}

		delete m_DiagPatrolInfoList;
	}

	//통계 데이터 반납
	if(m_DiagDataMap != NULL)
	{
		for(DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->begin(); iter != m_DiagDataMap->end(); iter++)
		{
			DIAG_OHT_STATISTICS_DATA* pDelData = (DIAG_OHT_STATISTICS_DATA*)iter->second;
			delete pDelData;
		}

		m_DiagDataMap->clear();

		delete m_DiagDataMap;
	}

	//Abnormal Data 반납
	if(m_DiagAbnormalDataList != NULL)
	{
		int nDataCnt = m_DiagAbnormalDataList->getCount();

		for(int i=0; i<nDataCnt; i++)
		{
			DIAG_OHT_ABNORMAL* pDeleteData = (DIAG_OHT_ABNORMAL*)m_DiagAbnormalDataList->popFront();

			delete pDeleteData;
		}

		delete m_DiagAbnormalDataList;
	}

	DeleteCriticalSection(&m_CSPreDiag);
}


void DiagManager::MakeStatDataMap()
{
	//각 VHL 통계데이터 생성
	// 진단 data
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVEDISTANCE,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVEDISTANCE, m_DiagParam.nStatUnitMoveDistance, m_DiagParam.nRefStatMoveDistance)));
	
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVESTEERING,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVESTEERING, m_DiagParam.nStatUnitMoveSteering, m_DiagParam.nRefStatMoveSteering)));
	
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVESHIFT,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVESHIFT, m_DiagParam.nStatUnitMoveShift, m_DiagParam.nRefStatMoveShift)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_REVSHIFT,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_REVSHIFT, m_DiagParam.nStatUnitRevShift, m_DiagParam.nRefStatRevShift)));
	
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVEHOIST,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVEHOIST, m_DiagParam.nStatUnitMoveHoist, m_DiagParam.nRefStatMoveHoist)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_REVHOIST,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_REVHOIST, m_DiagParam.nStatUnitRevHoist, m_DiagParam.nRefStatRevHoist)));
	
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVEROTATE,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVEROTATE, m_DiagParam.nStatUnitMoveRotate, m_DiagParam.nRefStatMoveRotate)));
	
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVEHAND,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVEHAND, m_DiagParam.nStatUnitMoveHand, m_DiagParam.nRefStatMoveHand)));
	
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVESHUTTER,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVESHUTTER, m_DiagParam.nStatUnitMoveShutter, m_DiagParam.nRefStatMoveShutter)));
	
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_COUNTDRVBCR,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_COUNTDRVBCR, m_DiagParam.nStatUnitCountDrvBCR, m_DiagParam.nRefStatCountDrvBCR)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_COUNTSTABCR,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_COUNTSTABCR, m_DiagParam.nStatUnitCountStaBCR, m_DiagParam.nRefStatCountStaBCR)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_TIMELOOKDOWN,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_TIMELOOKDOWN, m_DiagParam.nStatUnitTimeLookdown, m_DiagParam.nRefStatTimeLookdown)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_TIMEUBG,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_TIMEUBG, m_DiagParam.nStatUnitTimeUBG, m_DiagParam.nRefStatTimeUBG)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_TIMEPBS,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_TIMEPBS, m_DiagParam.nStatUnitTimePBS, m_DiagParam.nRefStatTimePBS)));

	//구동이력 data
	// Driving
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_MOVEDISTANCE_F,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_MOVEDISTANCE_F, m_DiagParam.nStatUnitMoveDistance, m_DiagParam.nRefStatMoveDistance)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_MOVEDISTANCE_R,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_MOVEDISTANCE_R, m_DiagParam.nStatUnitMoveDistance, m_DiagParam.nRefStatMoveDistance)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_TIMEDRIVING_F,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_TIMEDRIVING_F, m_DiagParam.nStatUnitTimeDriving, m_DiagParam.nRefStatTimeDriving)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_TIMEDRIVING_R,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_TIMEDRIVING_R, m_DiagParam.nStatUnitTimeDriving, m_DiagParam.nRefStatTimeDriving)));

	//Steering
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_MOVESTEERING_F,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_MOVESTEERING_F, m_DiagParam.nStatUnitMoveSteering, m_DiagParam.nRefStatMoveSteering)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_MOVESTEERING_R,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_MOVESTEERING_R, m_DiagParam.nStatUnitMoveSteering, m_DiagParam.nRefStatMoveSteering)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_TIMESTEERING_F,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_TIMESTEERING_F, m_DiagParam.nStatUnitTimeSteering, m_DiagParam.nRefStatTimeSteering)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_TIMESTEERING_R,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_TIMESTEERING_R, m_DiagParam.nStatUnitTimeSteering, m_DiagParam.nRefStatTimeSteering)));

	//Rotate
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ROTATECOUNT,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ROTATECOUNT, m_DiagParam.nStatUnitMoveRotate, m_DiagParam.nRefStatMoveRotate)));

	//Shift
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_SHIFTCOUNT,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_SHIFTCOUNT, m_DiagParam.nStatUnitMoveShift, m_DiagParam.nRefStatMoveShift)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_SHIFTTIME,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_SHIFTTIME, m_DiagParam.nStatUnitTimeShift, m_DiagParam.nRefStatTimeShift)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_SHIFTDIST,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_SHIFTDIST, m_DiagParam.nStatUnitShiftDistance, m_DiagParam.nRefStatShiftDistance)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_SHIFTREV,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_SHIFTREV, m_DiagParam.nStatUnitRevShift, m_DiagParam.nRefStatRevShift)));

	//Hoist
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_HOISTCOUNT,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_HOISTCOUNT, m_DiagParam.nStatUnitMoveHoist, m_DiagParam.nRefStatMoveHoist)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_HOISTTIME,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_HOISTTIME, m_DiagParam.nStatUnitTimeHoist, m_DiagParam.nRefStatTimeHoist)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_HOISTDIST,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_HOISTDIST, m_DiagParam.nStatUnitHoistDistance, m_DiagParam.nRefStatHoistDistance)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_HOISTREV,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_HOISTREV, m_DiagParam.nStatUnitRevHoist, m_DiagParam.nRefStatRevHoist)));

	//Hand
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_HANDCOUNT,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_HANDCOUNT, m_DiagParam.nStatUnitMoveHand, m_DiagParam.nRefStatMoveHand)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_HANDTIME,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_HANDTIME, m_DiagParam.nStatUnitTimeHand, m_DiagParam.nRefStatTimeHand)));

	//Shutter
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_SHUTTER_COUNT_F,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_SHUTTER_COUNT_F, m_DiagParam.nStatUnitMoveShutter, m_DiagParam.nRefStatMoveShutter)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_SHUTTER_COUNT_R,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_SHUTTER_COUNT_R, m_DiagParam.nStatUnitMoveShutter, m_DiagParam.nRefStatMoveShutter)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_SHUTTER_TIME_F,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_SHUTTER_TIME_F, m_DiagParam.nStatUnitTimeShutter, m_DiagParam.nRefStatTimeShutter)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_SHUTTER_TIME_R,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_SHUTTER_TIME_R, m_DiagParam.nStatUnitTimeShutter, m_DiagParam.nRefStatTimeShutter)));

	//BCR
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_DRVBCRCOUNT,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_DRVBCRCOUNT, m_DiagParam.nStatUnitCountDrvBCR, m_DiagParam.nRefStatCountDrvBCR)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_STABCRCOUNT,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_STABCRCOUNT, m_DiagParam.nStatUnitCountStaBCR, m_DiagParam.nRefStatCountStaBCR)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_DRVBCRTIME,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_DRVBCRTIME, m_DiagParam.nStatUnitTimeDrvBCR, m_DiagParam.nRefStatTimeDrvBCR)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_STABCRTIME,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_STABCRTIME, m_DiagParam.nStatUnitTimeStaBCR, m_DiagParam.nRefStatTimeStaBCR)));


	//Sensor
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_LOOKDOWNTIME,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_LOOKDOWNTIME, m_DiagParam.nStatUnitTimeLookdown, m_DiagParam.nRefStatTimeLookdown)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_UBGTIME,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_UBGTIME, m_DiagParam.nStatUnitTimeUBG, m_DiagParam.nRefStatTimeUBG)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_PBSTIME,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_PBSTIME, m_DiagParam.nStatUnitTimePBS, m_DiagParam.nRefStatTimePBS)));

	//전원
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_SMPSTIME,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_SMPSTIME, m_DiagParam.nStatUnitTimeSMPS, m_DiagParam.nRefStatTimeSMPS)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_REGULATORTIME,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_REGULATORTIME, m_DiagParam.nStatUnitTimeRegulator, m_DiagParam.nRefStatTimeRegulator)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_PICKUPTIME,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_PICKUPTIME, m_DiagParam.nStatUnitTimePickup, m_DiagParam.nRefStatTimePickup)));

	//QR
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_QR_COUNT_L,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_QR_COUNT_L, m_DiagParam.nStatUnitCountQR, m_DiagParam.nRefStatCountQR)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_QR_TIME_L,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_QR_TIME_L, m_DiagParam.nStatUnitTimeQR, m_DiagParam.nRefStatTimeQR)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_QR_COUNT_R,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_QR_COUNT_R, m_DiagParam.nStatUnitCountQR, m_DiagParam.nRefStatCountQR)));
	m_DiagOverhaulDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_QR_TIME_R,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_QR_TIME_R, m_DiagParam.nStatUnitTimeQR, m_DiagParam.nRefStatTimeQR)));
}

DiagManager* DiagManager::getInstance()
{
	int nError = NO_ERR;

	if(pInstance == NULL)
	{
		pInstance = new DiagManager;

		if(nError != NO_ERR)
		{
			ADD_LOG_DIAG("[DiagParam] Read File .. ErrorCode : %d", nError);
		}
	}

	return pInstance;
}

// 2018.08.06 SHCHO : To Call Kill Timer
void DiagManager::DestroyInstance()
{
	if(pInstance)
	{
		delete pInstance;   // calling ===> DiagManager::~DiagManager
		pInstance = NULL;
	}

}
/**
@brief	진단 관련 Parameter 불러옴
@data   2015.12.10
@author	doori.shin
*/
bool DiagManager::LoadDiagParamRecorvery(char* szFilePath, DIAG_PARAM* pParamSet)
{
	bool bSuccess = true;
	int nTmpValue = 0;
    int nResult = 0;
	bool bTmpValue = false;
	double dResult = 0.0;
	char strTempValue[8]= {0,};
	bool bCheckParam = false;

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
		//Read Reference values
		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveDistance", DIAG_DEFAULT_REF_VALUE_MOVEDISTANCE, 10, 300, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveDistance = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveSteering", DIAG_DEFAULT_REF_VALUE_MOVESTEERING, 2000, 10000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveSteering = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

 		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveShift", DIAG_DEFAULT_REF_VALUE_MOVESHIFT, 1000, 3000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveShift = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsRevShift", DIAG_DEFAULT_REF_VALUE_REVSHIFT, 1, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatRevShift = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsShiftDistance", DIAG_DEFAULT_REF_VALUE_SHIFTDISTANCE, 10, 300, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatShiftDistance = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

 		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveHoist", DIAG_DEFAULT_REF_VALUE_MOVEHOIST, 1000, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveHoist = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsRevHoist", DIAG_DEFAULT_REF_VALUE_REVHOIST, 5, 50, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatRevHoist = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsHoistDistance", DIAG_DEFAULT_REF_VALUE_HOISTDISTANCE, 10, 300, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatHoistDistance = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveRotate", DIAG_DEFAULT_REF_VALUE_MOVEROTATE, 1000, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveRotate = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveHand", DIAG_DEFAULT_REF_VALUE_MOVEHAND, 1000, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveHand = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveShutter", DIAG_DEFAULT_REF_VALUE_MOVESHUTTER, 1000, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveShutter = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsCountDrvBCR", DIAG_DEFAULT_REF_VALUE_COUNTDRVBCR, 50, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatCountDrvBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsCountStaBCR", DIAG_DEFAULT_REF_VALUE_COUNTSTABCR, 50, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatCountStaBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsCountQR", DIAG_DEFAULT_REF_VALUE_COUNTQR, 50, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatCountQR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeDriving", DIAG_DEFAULT_REF_VALUE_TIMEDRIVING, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeDriving = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeSteering", DIAG_DEFAULT_REF_VALUE_TIMESTEERING, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeSteering = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeShift", DIAG_DEFAULT_REF_VALUE_TIMESHIFT, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeShift = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeHoist", DIAG_DEFAULT_REF_VALUE_TIMEHOIST, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeHoist = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeHand", DIAG_DEFAULT_REF_VALUE_TIMEHAND, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeHand = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeShutter", DIAG_DEFAULT_REF_VALUE_TIMESHUTTER, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeShutter = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeDrvBCR", DIAG_DEFAULT_REF_VALUE_TIMEDRVBCR, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeDrvBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeStaBCR", DIAG_DEFAULT_REF_VALUE_TIMESTABCR, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeStaBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeQR", DIAG_DEFAULT_REF_VALUE_TIMEQR, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeQR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeLookdown", DIAG_DEFAULT_REF_VALUE_TIMELOOKDOWN, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeLookdown = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeUBG", DIAG_DEFAULT_REF_VALUE_TIMEUBG, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeUBG = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimePBS", DIAG_DEFAULT_REF_VALUE_TIMEPBS, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimePBS = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeSMPS", DIAG_DEFAULT_REF_VALUE_TIMESMPS, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeSMPS = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeRegulator", DIAG_DEFAULT_REF_VALUE_TIMEREGULATOR, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeRegulator = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimePickup", DIAG_DEFAULT_REF_VALUE_TIMEPICKUP, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimePickup = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveDistance", DIAG_REF_VALUE_UNIT_MOVEDISTANCE, 100000, 2000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveDistance = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveSteering", DIAG_REF_VALUE_UNIT_MOVESTEERING, 0, 5, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveSteering = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveShift", DIAG_REF_VALUE_UNIT_MOVESHIFT, 0, 5, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveShift = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitRevShift", DIAG_REF_VALUE_UNIT_REVSHIFT, 1000, 30000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitRevShift = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitShiftDistance", DIAG_REF_VALUE_UNIT_SHIFTDISTANCE, 100000, 2000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitShiftDistance = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveHoist", DIAG_REF_VALUE_UNIT_MOVEHOIST, 0, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveHoist = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitRevHoist", DIAG_REF_VALUE_UNIT_REVHOIST, 1000, 30000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitRevHoist = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitHoistDistance", DIAG_REF_VALUE_UNIT_HOISTDISTANCE, 100000, 2000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitHoistDistance = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveRotate", DIAG_REF_VALUE_UNIT_MOVEROTATE, 0, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveRotate = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveHand", DIAG_REF_VALUE_UNIT_MOVEHAND, 0, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveHand = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveShutter", DIAG_REF_VALUE_UNIT_MOVESHUTTER, 0, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveShutter = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitCountDrvBCR", DIAG_REF_VALUE_UNIT_COUNTDRVBCR, 1000, 30000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitCountDrvBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitCountStaBCR", DIAG_REF_VALUE_UNIT_COUNTSTABCR, 1000, 30000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitCountStaBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitCountQR", DIAG_REF_VALUE_UNIT_COUNTQR, 1000, 30000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitCountQR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeDriving", DIAG_REF_VALUE_UNIT_TIMEDRIVING, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeDriving = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeSteering", DIAG_REF_VALUE_UNIT_TIMESTEERING, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeSteering = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeShift", DIAG_REF_VALUE_UNIT_TIMESHIFT, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeShift = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeHoist", DIAG_REF_VALUE_UNIT_TIMEHOIST, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeHoist = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeHand", DIAG_REF_VALUE_UNIT_TIMEHAND, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeHand = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeShutter", DIAG_REF_VALUE_UNIT_TIMESHUTTER, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeShutter = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeDrvBCR", DIAG_REF_VALUE_UNIT_TIMEDRVBCR, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeDrvBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeStaBCR", DIAG_REF_VALUE_UNIT_TIMESTABCR, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeStaBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeQR", DIAG_REF_VALUE_UNIT_TIMEQR, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeQR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeLookdown", DIAG_REF_VALUE_UNIT_TIMELOOKDOWN, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeLookdown = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeUBG", DIAG_REF_VALUE_UNIT_TIMEUBG, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeUBG = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimePBS", DIAG_REF_VALUE_UNIT_TIMEPBS, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimePBS = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeSMPS", DIAG_REF_VALUE_UNIT_TIMESMPS, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeSMPS = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeRegulator", DIAG_REF_VALUE_UNIT_TIMEREGULATOR, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeRegulator = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimePickup", DIAG_REF_VALUE_UNIT_TIMEPICKUP, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimePickup = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalMemoryUsingCount", DIAG_ABNORMAL_REF_VALUE_MEMORY_COUNT, 1, 50, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormUsingMemoryCount = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalMemoryUsingRate", DIAG_ABNORMAL_REF_VALUE_MEMORY_USING_RATE, 5, 200, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormUsingMemoryRate = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalTimeoverMoveHand", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_HAND, 500, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormTimeoverMoveHand = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalTimeoverMoveShutter", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_SHUTTER, 500, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormTimeoverMoveShutter = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalTimeoverMoveSteering", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_STEERING, 500, 20000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormTimeoverMoveSteering = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalAlarmResetHandCount", DIAG_ABNORMAL_REF_VALUE_ALARM_RESET_HAND, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormAlarmResetHand = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalOutofPhasePBS", DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_PBS, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormOutofPhasePBS = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalOutofPhaseUBG", DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_UBG, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormOutofPhaseUBG = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalRetryBCR", DIAG_ABNORMAL_REF_VALUE_RETRY_BCR, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormRetryBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalDifferenceEncoderCount", DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_CNT, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormDifferenceEncoderCount = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalDifferenceEncoderLimit", DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_LIMIT, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormDifferenceEncoderLimit = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatusAvailableSpaceC", DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_C_DRIVE, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatusAvailableSpaceC = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatusAvailableSpaceD", DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_D_DRIVE, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatusAvailableSpaceD = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ETC", "PBSOutOfRangeTimeout", PBS_OUT_OF_RANGE_TIMEOUT, 100, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nPBSOutOfRangeTimeout = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ETC", "MemoryResetByte", 200000000, 20000000, 300000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefMemoryReset = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		delete pIniFile;
	}
	else
	{
		//File Open Error
		bSuccess = false;
	}
	//AOHC-358 파라미터 적용 오류 시, 에러 발생
	if(bSuccess ==false)
	{
		bCheckDiagParam = true;
		bCheckParam = LoadDiagParam("DataFiles\\DiagParam.ini", pParamSet);
		if(bCheckParam == false)

			OHTMainForm->m_CheckParamResult = ERR_PARADATA_LOAD_FAIL;
		else
			OHTMainForm->m_CheckParamResult = NO_ERR;

	}


	return bSuccess;
}


/**
@brief	진단 관련 Parameter 불러옴
@data   2015.12.10
@author	doori.shin
*/
bool DiagManager::LoadDiagParam(char* szFilePath, DIAG_PARAM* pParamSet)
{
	bool bSuccess = true;
	int nTmpValue = 0;
    int nResult = 0;
	bool bTmpValue = false;
	double dResult = 0.0;
	char strTempValue[8]= {0,};

    LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile(szFilePath);

	if(!pIniFile->GetIniFileExist())
    {
		delete pIniFile;
		pIniFile = NULL;
		bSuccess = false;
    }
	//AOHC-358 파라미터 적용 오류 시, 에러 발생
	if(pIniFile == NULL || bCheckDiagParam == true)
	{
		//File Open Error

		strcpy(pParamSet->Version, "00");

		pParamSet->nRefStatMoveDistance = 100;
		pParamSet->nRefStatMoveSteering = 5000;
		pParamSet->nRefStatMoveShift= 2000;
		pParamSet->nRefStatRevShift = 2;
		pParamSet->nRefStatShiftDistance = 100;
		pParamSet->nRefStatMoveHoist= 2000;
		pParamSet->nRefStatRevHoist=10;
		pParamSet->nRefStatHoistDistance = 100;
		pParamSet->nRefStatMoveRotate=2000;
		pParamSet->nRefStatMoveHand=2000;
		pParamSet->nRefStatMoveShutter=2000;
		pParamSet->nRefStatCountDrvBCR=5000;
		pParamSet->nRefStatCountStaBCR=2000;
		pParamSet->nRefStatCountQR=2000;
		pParamSet->nRefStatTimeDriving=24;
		pParamSet->nRefStatTimeSteering=24;
		pParamSet->nRefStatTimeShift=24;
		pParamSet->nRefStatTimeHoist=24;
		pParamSet->nRefStatTimeHand=24;
		pParamSet->nRefStatTimeShutter=24;
		pParamSet->nRefStatTimeDrvBCR=24;
		pParamSet->nRefStatTimeStaBCR=24;
		pParamSet->nRefStatTimeQR=24;
		pParamSet->nRefStatTimeLookdown=24;
		pParamSet->nRefStatTimeUBG=24;
		pParamSet->nRefStatTimePBS=24;
		pParamSet->nRefStatTimeSMPS=24;
		pParamSet->nRefStatTimeRegulator=24;
		pParamSet->nRefStatTimePickup=24;

		pParamSet->nStatUnitMoveDistance=1000000;
		pParamSet->nStatUnitMoveSteering=1;
		pParamSet->nStatUnitMoveShift=1;
		pParamSet->nStatUnitRevShift=10000;
		pParamSet->nStatUnitShiftDistance=1000000;
		pParamSet->nStatUnitMoveHoist=1;
		pParamSet->nStatUnitRevHoist=10000;
		pParamSet->nStatUnitHoistDistance=1000000;
		pParamSet->nStatUnitMoveRotate=1;
		pParamSet->nStatUnitMoveHand=1;
		pParamSet->nStatUnitMoveShutter=1;
		pParamSet->nStatUnitCountDrvBCR=10000;
		pParamSet->nStatUnitCountStaBCR=10000;
		pParamSet->nStatUnitCountQR=10000;
		pParamSet->nStatUnitTimeDriving=3600000;
		pParamSet->nStatUnitTimeSteering=3600000;
		pParamSet->nStatUnitTimeShift=3600000;
		pParamSet->nStatUnitTimeHoist=3600000;
		pParamSet->nStatUnitTimeHand=3600000;
		pParamSet->nStatUnitTimeShutter=3600000;
		pParamSet->nStatUnitTimeDrvBCR=3600000;
		pParamSet->nStatUnitTimeStaBCR=3600000;
		pParamSet->nStatUnitTimeQR=3600000;
		pParamSet->nStatUnitTimeLookdown=3600000;
		pParamSet->nStatUnitTimeUBG=3600000;
		pParamSet->nStatUnitTimePBS=3600000;
		pParamSet->nStatUnitTimeSMPS=3600000;
		pParamSet->nStatUnitTimeRegulator=3600000;
		pParamSet->nStatUnitTimePickup=3600000;

		pParamSet->nRefAbnormUsingMemoryCount=5;
		pParamSet->nRefAbnormUsingMemoryRate=80;
		pParamSet->nRefAbnormTimeoverMoveHand=1200;
		pParamSet->nRefAbnormTimeoverMoveShutter=1600;
		pParamSet->nRefAbnormTimeoverMoveSteering=4000;
		pParamSet->nRefAbnormAlarmResetHand=5;
		pParamSet->nRefAbnormOutofPhasePBS=100;
		pParamSet->nRefAbnormOutofPhaseUBG=100;
		pParamSet->nRefAbnormRetryBCR=1;
		pParamSet->nRefAbnormDifferenceEncoderCount=10;
		pParamSet->nRefAbnormDifferenceEncoderLimit=5;
		pParamSet->nRefStatusAvailableSpaceC=75;
		pParamSet->nRefStatusAvailableSpaceD=75;
		pParamSet->nPBSOutOfRangeTimeout=1500;
		pParamSet->nRefMemoryReset  = 200000000;

		bSuccess = false;
		bCheckDiagParam = false;
		ADD_MD_LOG("Diag Load Param Loading Fail");
		return   bSuccess;

	}
	else if(pIniFile != NULL)
	{
		//Read Diag Agent informations
		pIniFile->ReadString("Version","IniVersion", "00", pParamSet->Version);

		//Read Reference values

		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveDistance", DIAG_DEFAULT_REF_VALUE_MOVEDISTANCE, &pParamSet->nRefStatMoveDistance);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveSteering", DIAG_DEFAULT_REF_VALUE_MOVESTEERING, &pParamSet->nRefStatMoveSteering);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveShift", DIAG_DEFAULT_REF_VALUE_MOVESHIFT, &pParamSet->nRefStatMoveShift);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsRevShift", DIAG_DEFAULT_REF_VALUE_REVSHIFT, &pParamSet->nRefStatRevShift);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsShiftDistance", DIAG_DEFAULT_REF_VALUE_SHIFTDISTANCE, &pParamSet->nRefStatShiftDistance);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveHoist", DIAG_DEFAULT_REF_VALUE_MOVEHOIST, &pParamSet->nRefStatMoveHoist);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsRevHoist", DIAG_DEFAULT_REF_VALUE_REVHOIST, &pParamSet->nRefStatRevHoist);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsHoistDistance", DIAG_DEFAULT_REF_VALUE_HOISTDISTANCE, &pParamSet->nRefStatHoistDistance);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveRotate", DIAG_DEFAULT_REF_VALUE_MOVEROTATE, &pParamSet->nRefStatMoveRotate);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveHand", DIAG_DEFAULT_REF_VALUE_MOVEHAND, &pParamSet->nRefStatMoveHand);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveShutter", DIAG_DEFAULT_REF_VALUE_MOVESHUTTER, &pParamSet->nRefStatMoveShutter);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsCountDrvBCR", DIAG_DEFAULT_REF_VALUE_COUNTDRVBCR, &pParamSet->nRefStatCountDrvBCR);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsCountStaBCR", DIAG_DEFAULT_REF_VALUE_COUNTSTABCR, &pParamSet->nRefStatCountStaBCR);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsCountQR", DIAG_DEFAULT_REF_VALUE_COUNTQR, &pParamSet->nRefStatCountQR);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeDriving", DIAG_DEFAULT_REF_VALUE_TIMEDRIVING, &pParamSet->nRefStatTimeDriving);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeSteering", DIAG_DEFAULT_REF_VALUE_TIMESTEERING, &pParamSet->nRefStatTimeSteering);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeShift", DIAG_DEFAULT_REF_VALUE_TIMESHIFT, &pParamSet->nRefStatTimeShift);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeHoist", DIAG_DEFAULT_REF_VALUE_TIMEHOIST, &pParamSet->nRefStatTimeHoist);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeHand", DIAG_DEFAULT_REF_VALUE_TIMEHAND, &pParamSet->nRefStatTimeHand);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeShutter", DIAG_DEFAULT_REF_VALUE_TIMESHUTTER, &pParamSet->nRefStatTimeShutter);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeDrvBCR", DIAG_DEFAULT_REF_VALUE_TIMEDRVBCR, &pParamSet->nRefStatTimeDrvBCR);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeStaBCR", DIAG_DEFAULT_REF_VALUE_TIMESTABCR, &pParamSet->nRefStatTimeStaBCR);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeQR", DIAG_DEFAULT_REF_VALUE_TIMEQR, &pParamSet->nRefStatTimeQR);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeLookdown", DIAG_DEFAULT_REF_VALUE_TIMELOOKDOWN, &pParamSet->nRefStatTimeLookdown);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeUBG", DIAG_DEFAULT_REF_VALUE_TIMEUBG, &pParamSet->nRefStatTimeUBG);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimePBS", DIAG_DEFAULT_REF_VALUE_TIMEPBS, &pParamSet->nRefStatTimePBS);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeSMPS", DIAG_REF_VALUE_UNIT_TIMESMPS, &pParamSet->nRefStatTimeSMPS);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeRegulator", DIAG_REF_VALUE_UNIT_TIMEREGULATOR, &pParamSet->nRefStatTimeRegulator);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimePickup", DIAG_REF_VALUE_UNIT_TIMEPICKUP, &pParamSet->nRefStatTimePickup);

		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveDistance", DIAG_REF_VALUE_UNIT_MOVEDISTANCE, &pParamSet->nStatUnitMoveDistance);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveSteering", DIAG_REF_VALUE_UNIT_MOVESTEERING, &pParamSet->nStatUnitMoveSteering);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveShift", DIAG_REF_VALUE_UNIT_MOVESHIFT, &pParamSet->nStatUnitMoveShift);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitRevShift", DIAG_REF_VALUE_UNIT_REVSHIFT, &pParamSet->nStatUnitRevShift);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitShiftDistance", DIAG_REF_VALUE_UNIT_SHIFTDISTANCE, &pParamSet->nStatUnitShiftDistance);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveHoist", DIAG_REF_VALUE_UNIT_MOVEHOIST, &pParamSet->nStatUnitMoveHoist);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitRevHoist", DIAG_REF_VALUE_UNIT_REVHOIST, &pParamSet->nStatUnitRevHoist);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitHoistDistance", DIAG_REF_VALUE_UNIT_HOISTDISTANCE, &pParamSet->nStatUnitHoistDistance);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveRotate", DIAG_REF_VALUE_UNIT_MOVEROTATE, &pParamSet->nStatUnitMoveRotate);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveHand", DIAG_REF_VALUE_UNIT_MOVEHAND, &pParamSet->nStatUnitMoveHand);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveShutter", DIAG_REF_VALUE_UNIT_MOVESHUTTER, &pParamSet->nStatUnitMoveShutter);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitCountDrvBCR", DIAG_REF_VALUE_UNIT_COUNTDRVBCR, &pParamSet->nStatUnitCountDrvBCR);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitCountStaBCR", DIAG_REF_VALUE_UNIT_COUNTSTABCR, &pParamSet->nStatUnitCountStaBCR);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitCountQR", DIAG_REF_VALUE_UNIT_COUNTQR, &pParamSet->nStatUnitCountQR);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeDriving", DIAG_REF_VALUE_UNIT_TIMEDRIVING, &pParamSet->nStatUnitTimeDriving);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeSteering", DIAG_REF_VALUE_UNIT_TIMESTEERING, &pParamSet->nStatUnitTimeSteering);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeShift", DIAG_REF_VALUE_UNIT_TIMESHIFT, &pParamSet->nStatUnitTimeShift);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeHoist", DIAG_REF_VALUE_UNIT_TIMEHOIST, &pParamSet->nStatUnitTimeHoist);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeHand", DIAG_REF_VALUE_UNIT_TIMEHAND, &pParamSet->nStatUnitTimeHand);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeShutter", DIAG_REF_VALUE_UNIT_TIMESHUTTER, &pParamSet->nStatUnitTimeShutter);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeDrvBCR", DIAG_REF_VALUE_UNIT_TIMEDRVBCR, &pParamSet->nStatUnitTimeDrvBCR);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeStaBCR", DIAG_REF_VALUE_UNIT_TIMESTABCR, &pParamSet->nStatUnitTimeStaBCR);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeQR", DIAG_REF_VALUE_UNIT_TIMEQR, &pParamSet->nStatUnitTimeQR);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeLookdown", DIAG_REF_VALUE_UNIT_TIMELOOKDOWN, &pParamSet->nStatUnitTimeLookdown);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeUBG", DIAG_REF_VALUE_UNIT_TIMEUBG, &pParamSet->nStatUnitTimeUBG);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimePBS", DIAG_REF_VALUE_UNIT_TIMEPBS, &pParamSet->nStatUnitTimePBS);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeSMPS", DIAG_REF_VALUE_UNIT_TIMESMPS, &pParamSet->nStatUnitTimeSMPS);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeRagulator", DIAG_REF_VALUE_UNIT_TIMEREGULATOR, &pParamSet->nStatUnitTimeRegulator);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimePickup", DIAG_REF_VALUE_UNIT_TIMEPICKUP, &pParamSet->nStatUnitTimePickup);

		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalMemoryUsingCount", DIAG_ABNORMAL_REF_VALUE_MEMORY_COUNT, &pParamSet->nRefAbnormUsingMemoryCount);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalMemoryUsingRate", DIAG_ABNORMAL_REF_VALUE_MEMORY_USING_RATE, &pParamSet->nRefAbnormUsingMemoryRate);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveHand", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_HAND, &pParamSet->nRefAbnormTimeoverMoveHand);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveShutter", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_SHUTTER, &pParamSet->nRefAbnormTimeoverMoveShutter);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveSteering", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_STEERING, &pParamSet->nRefAbnormTimeoverMoveSteering);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalAlarmResetHandCount", DIAG_ABNORMAL_REF_VALUE_ALARM_RESET_HAND, &pParamSet->nRefAbnormAlarmResetHand);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalOutofPhasePBS", DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_PBS, &pParamSet->nRefAbnormOutofPhasePBS);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalOutofPhaseUBG", DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_UBG, &pParamSet->nRefAbnormOutofPhaseUBG);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalRetryBCR", DIAG_ABNORMAL_REF_VALUE_RETRY_BCR, &pParamSet->nRefAbnormRetryBCR);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalDifferenceEncoderCount", DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_CNT, &pParamSet->nRefAbnormDifferenceEncoderCount);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalDifferenceEncoderLimit", DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_LIMIT, &pParamSet->nRefAbnormDifferenceEncoderLimit);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatusAvailableSpaceC", DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_C_DRIVE, &pParamSet->nRefStatusAvailableSpaceC);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatusAvailableSpaceD", DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_D_DRIVE, &pParamSet->nRefStatusAvailableSpaceD);
		pIniFile->ReadInteger("ETC", "PBSOutOfRangeTimeout", PBS_OUT_OF_RANGE_TIMEOUT, &pParamSet->nPBSOutOfRangeTimeout);
        pIniFile->ReadInteger("ETC", "MemoryResetByte", 200000000, &pParamSet->nRefMemoryReset);


		bSuccess = true;

		delete pIniFile;
	}


	return bSuccess;
}


bool DiagManager::saveParameter(char* szFilePath, DIAG_PARAM* pParamSet)
{
    bool bResult = false;

    LIniFile *m_lWriteIniFile;
    m_lWriteIniFile = NULL;
    m_lWriteIniFile = new LIniFile(szFilePath);

    if(!m_lWriteIniFile->GetIniFileExist())
    {
		delete m_lWriteIniFile;
		m_lWriteIniFile = NULL;
    }

    if(m_lWriteIniFile != NULL)
    {
		m_lWriteIniFile->DeleteIniFile();

		m_lWriteIniFile->WriteString("Version","IniVersion", pParamSet->Version);
		// 1. Hoist Param
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveDistance",  pParamSet->nRefStatMoveDistance);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveSteering",  pParamSet->nRefStatMoveSteering);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveShift",  pParamSet->nRefStatMoveShift);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsRevShift",  pParamSet->nRefStatRevShift);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsShiftDistance",  pParamSet->nRefStatShiftDistance);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveHoist", pParamSet->nRefStatMoveHoist);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsRevHoist", pParamSet->nRefStatRevHoist);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsHoistDistance",  pParamSet->nRefStatHoistDistance);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveRotate", pParamSet->nRefStatMoveRotate);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveHand", pParamSet->nRefStatMoveHand);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveShutter", pParamSet->nRefStatMoveShutter);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsCountDrvBCR", pParamSet->nRefStatCountDrvBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsCountStaBCR", pParamSet->nRefStatCountStaBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsCountQR", pParamSet->nRefStatCountQR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeDriving", pParamSet->nRefStatTimeDriving);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeSteering", pParamSet->nRefStatTimeSteering);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeShift", pParamSet->nRefStatTimeShift);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeHoist", pParamSet->nRefStatTimeHoist);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeHand", pParamSet->nRefStatTimeHand);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeShutter", pParamSet->nRefStatTimeShutter);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeDrvBCR", pParamSet->nRefStatTimeDrvBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeStaBCR", pParamSet->nRefStatTimeStaBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeQR", pParamSet->nRefStatTimeQR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeLookdown", pParamSet->nRefStatTimeLookdown);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeUBG", pParamSet->nRefStatTimeUBG);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimePBS", pParamSet->nRefStatTimePBS);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeSMPS", pParamSet->nRefStatTimeSMPS);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeRegulator", pParamSet->nRefStatTimeRegulator);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimePickup", pParamSet->nRefStatTimePickup);

		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveDistance", pParamSet->nStatUnitMoveDistance);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveSteering", pParamSet->nStatUnitMoveSteering);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveShift", pParamSet->nStatUnitMoveShift);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitRevShift", pParamSet->nStatUnitRevShift);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitShiftDistance", pParamSet->nStatUnitShiftDistance);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveHoist", pParamSet->nStatUnitMoveHoist);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitRevHoist", pParamSet->nStatUnitRevHoist);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitHoistDistance", pParamSet->nStatUnitHoistDistance);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveRotate", pParamSet->nStatUnitMoveRotate);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveHand", pParamSet->nStatUnitMoveHand);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveShutter", pParamSet->nStatUnitMoveShutter);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitCountDrvBCR", pParamSet->nStatUnitCountDrvBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitCountStaBCR", pParamSet->nStatUnitCountStaBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitCountQR", pParamSet->nStatUnitCountQR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeDriving", pParamSet->nStatUnitTimeDriving);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeSteering", pParamSet->nStatUnitTimeSteering);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeShift", pParamSet->nStatUnitTimeShift);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeHoist", pParamSet->nStatUnitTimeHoist);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeHand", pParamSet->nStatUnitTimeHand);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeShutter", pParamSet->nStatUnitTimeShutter);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeDrvBCR", pParamSet->nStatUnitTimeDrvBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeStaBCR", pParamSet->nStatUnitTimeStaBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeRQ", pParamSet->nStatUnitTimeQR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeLookdown", pParamSet->nStatUnitTimeLookdown);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeUBG", pParamSet->nStatUnitTimeUBG);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimePBS", pParamSet->nStatUnitTimePBS);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeSMPS", pParamSet->nStatUnitTimeSMPS);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeRagulator", pParamSet->nStatUnitTimeRegulator);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimePickup", pParamSet->nStatUnitTimePickup);

		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalMemoryUsingCount", pParamSet->nRefAbnormUsingMemoryCount);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalMemoryUsingRate", pParamSet->nRefAbnormUsingMemoryRate);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveHand", pParamSet->nRefAbnormTimeoverMoveHand);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveShutter", pParamSet->nRefAbnormTimeoverMoveShutter);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveSteering", pParamSet->nRefAbnormTimeoverMoveSteering);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalAlarmResetHandCount", pParamSet->nRefAbnormAlarmResetHand);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalOutofPhasePBS", pParamSet->nRefAbnormOutofPhasePBS);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalOutofPhaseUBG", pParamSet->nRefAbnormOutofPhaseUBG);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalRetryBCR", pParamSet->nRefAbnormRetryBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalDifferenceEncoderCount", pParamSet->nRefAbnormDifferenceEncoderCount);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalDifferenceEncoderLimit", pParamSet->nRefAbnormDifferenceEncoderLimit);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatusAvailableSpaceC", pParamSet->nRefStatusAvailableSpaceC);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatusAvailableSpaceD", pParamSet->nRefStatusAvailableSpaceD);
		m_lWriteIniFile->WriteInteger("ETC", "PBSOutOfRangeTimeout", pParamSet->nPBSOutOfRangeTimeout);
        m_lWriteIniFile->WriteInteger("ETC", "MemoryResetByte", pParamSet->nRefMemoryReset);

		delete m_lWriteIniFile;
		bResult = true;


	}

    return bResult;
}

/**
@brief	진단에서 사용하는 파라메터를 리턴
@data   2016.04.06
@author	doori.shin
*/
DIAG_PARAM DiagManager::GetDiagParam()
{
	return this->m_DiagParam;
}

DIAG_EVENT_REPORT DiagManager::GetDiagEventFlag()
{
	return this->m_DiagEventFlag;
}


/**
@brief	진단 데이터를 업데이트 한다
@data   2016.04.06
@author	doori.shin
@param	bID : 업데이트 할 데이터 ID
@param	nValue : 추가로 더할 값
@return 업데이트 성공시 true, 실패시 false(Map 자료구조에 ID에 해당하는 데이터 없음)
*/
int DiagManager::UpdateStatDiagData(BYTE bID, double nValue)
{
	DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->find(bID);
	int nResult = DIAG_ACK_ERR_UNKNOWN;

	//0보다 큰 데이터인 경우만 업데이트
	if(nValue > 0.0f)
	{
		if(iter == m_DiagDataMap->end())
		{
			nResult = DIAG_ACK_ERR_NO_DATA;
		}
		else
		{
			iter->second->UpdateData(nValue);
			nResult = DIAG_ACK_OK;
		}
	}

	return nResult;
}

/**
@brief	구동이력 데이터를 업데이트 한다
@data   2024.11.25
@author	sg22.lee
@param	bID : 업데이트 할 데이터 ID
@param	nValue : 추가로 더할 값
@return 업데이트 성공시 true, 실패시 false(Map 자료구조에 ID에 해당하는 데이터 없음)
*/
int DiagManager::UpdateOverhaulData(BYTE bID, double nValue)
{
	DIAG_STAT_DATA_MAP_ITER iter = m_DiagOverhaulDataMap->find(bID);
	int nResult = DIAG_ACK_ERR_UNKNOWN;

	//0보다 큰 데이터인 경우만 업데이트
	if(nValue > 0.0f)
	{
		if(iter == m_DiagOverhaulDataMap->end())
		{
			nResult = DIAG_ACK_ERR_NO_DATA;
		}
		else
		{
			iter->second->UpdateData(nValue);
			nResult = DIAG_ACK_OK;
		}
	}

	return nResult;
}

/**
@brief	진단 데이터를 보고하는 기준 데이터를 갱신한다
@data   2016.04.06
@author	doori.shin
@param	bID : 업데이트 할 데이터 ID
@param	nValue : 갱신되는 값
@return 업데이트 성공시 true, 실패시 false(Map 자료구조에 ID에 해당하는 데이터 없음)

int DiagManager::UpdateStatReferenceValue(BYTE bID, int nValue)
{
	DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->find(bID);
	int nResult = DIAG_ACK_ERR_UNKNOWN;

	if(iter == m_DiagDataMap->end())
	{
		nResult = DIAG_ACK_ERR_NO_DATA;
	}
	else
	{
		iter->second->nReferenceValue = nValue;
		nResult = DIAG_ACK_OK;
	}

	return nResult;
}
*/

/**
@brief	수집한 진단데이터를 클리어 한다
@data   2016.04.06
@author	doori.shin
@param	bID : 클리어 할 데이터 ID
@return Clear성공 시 true, 실패시 false(Map 자료구조에 ID에 해당하는 데이터 없음)
*/
int DiagManager::ClearStatDiagData(BYTE bID)
{
	DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->find(bID);
	int nResult = DIAG_ACK_ERR_UNKNOWN;

	if(iter == m_DiagDataMap->end())
	{
		nResult = DIAG_ACK_ERR_NO_DATA;
	}
	else
	{
		iter->second->ClearData();
		nResult = DIAG_ACK_OK;
	}

	return nResult;
}

/**
@brief	수집한 진단데이터를 전송해도 되는지 확인
@data   2016.04.06
@author	doori.shin
@param	bID : 확인할 데이터 ID
@return 전송가능(기준값이상)이면 true, 그 외에는 false(Map 자료구조에 ID에 해당하는 데이터 없음)

bool DiagManager::CheckStatDataToSend(BYTE bID)
{
	DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->find(bID);
	bool bResult = false;

	if(iter == m_DiagDataMap->end())
	{
		bResult = false;
	}
	else
	{
		bResult = iter->second->IsReadyToSend();
	}

	return bResult;
}
*/
/**
@brief	진단데이터 전송 값 리턴
@data   2016.04.06
@author
@param	bID : 확인할 데이터 ID
@return
*/
double DiagManager::GetStatDiagData(BYTE bID)
{
 	DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->find(bID);
	double bResult = 0.0f;

	if(iter == m_DiagDataMap->end())
	{

	}
	else
	{
		bResult = iter->second->dTotalData;
	}

	return bResult;
}
/**
@brief	진단 데이터를 업데이트 한다
@data   2016.04.06
@author	doori.shin
@param	bID : 업데이트 할 데이터 ID
@param	nValue : 추가로 더할 값
@return 업데이트 성공시 true, 실패시 false(Map 자료구조에 ID에 해당하는 데이터 없음)
*/
bool DiagManager::UpdateStatDiagAllData(DIAG_PKT_BODY_DATA_INFO_RESPONSE nStatData)
{
//	    UINT64 nStatData = 0;
		int nLoadedCount = 0;
//		int nOffset = UDP_HEADER_LEN;

		//Parsing 통계 데이터
		for(DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->begin();
			iter != m_DiagDataMap->end(); iter++)
		{
			DIAG_OHT_STATISTICS_DATA* pData = (DIAG_OHT_STATISTICS_DATA*)iter->second;

			try
			{

				if(pData->dTotalData != 0)
				{
					nLoadedCount++;
					continue;
				}

				// DiagStat.txt (구동 이력)으로 대체하여 불필요

				double dUpdateData = nStatData.InfoData[nLoadedCount] * pData->nUnit;
				pData->UpdateData(dUpdateData);
				pData->dPreSendData = nStatData.InfoData[nLoadedCount];	//진단서버에 저장되어있던 데이터
				ADD_LOG_DIAG("Update Data. [ID:%02X, Value:%d]", pData->bID, nStatData.InfoData[nLoadedCount]);
				nLoadedCount++;
			}
			catch(Exception& e)
			{
				ADD_LOG_DIAG("[Exception] %s, ", e.StackTrace);
				break;
			}
		}

		//데이터가 정상 로딩되었으면 타이머 중지
		if(nLoadedCount == 14)
		{
			m_bIsLoadedDiagData = true;
			timeKillEvent(m_nDiagCheckTimerID);
			ADD_LOG_DIAG("Update Complete. Count : %d", nLoadedCount);
		}
		else
		{
			m_bIsLoadedDiagData = false;
		}

}

/**
@brief	Abnormal로 수집된 진단데이터를 넣음
@data   2016.04.19
@author	doori.shin
@param	pInsertData : 리스트에 삽입할 데이터
*/
void DiagManager::InsertAbnormalDiagData(BYTE bDataType)
{
	if(m_DiagAbnormalDataList != NULL)
	{
		DIAG_OHT_ABNORMAL* pInsertData = new DIAG_OHT_ABNORMAL;
		pInsertData->bDataType = bDataType;
		pInsertData->nNodeID = OHTMainForm->GetOHTStatus()->StatusCommon.CurNodeID;

		if(m_DiagAbnormalDataList->getCount() > DIAG_MAX_SIZE_ABNORMAL_DATA_LIST)
		{
			DIAG_OHT_ABNORMAL* pDeleteData = (DIAG_OHT_ABNORMAL*)m_DiagAbnormalDataList->popFront();

			delete pDeleteData;
		}

		m_DiagAbnormalDataList->insertBack(pInsertData);

		ADD_LOG_DIAG("[DiagManager] Inserted Abnormal data : [ID:%02X], [NodeID:%05d]",
					pInsertData->bDataType,
					pInsertData->nNodeID);
	}
}

/**
@brief	Abnormal 수집된 진단 데이터 리스트 가져옴
@data   2016.04.19
@author	doori.shin
*/
LList* DiagManager::GetAbnormalDiagDataList()
{
	return this->m_DiagAbnormalDataList;
}

/**
@brief	Disk의 사용량 정보를 확인
@data   2016.04.19
@author	doori.shin
@param	pInfo : Disk정보를 저장할 구조체
@return DIAG_OHT_STATUS_OK : Parameter로 정한 사용량 수치보다 낮음
		DIAG_OHT_STATUS_NG : Parameter로 정한 사용량 보다 초과사용
*/
bool DiagManager::CheckDiskStatus(DISK_INFO* pInfo)
{
	bool bResult = DIAG_OHT_STATUS_OK;
	int nUpperbound = 0;
	int nUsingRate = 0;

	if(pInfo == NULL)
	{
		ADD_LOG_DIAG("[Error] Parameter(pInfo) is NULL pointer.");
	}
	else if(OHTUTILITY::GetDiskInfo(pInfo, pInfo->Drive) == NO_ERR)
	{
		//Parameter로 가지고 있는 Drive 상한선값을 가져옴
		switch(pInfo->Drive)
		{
			case 'C':
			nUpperbound = m_DiagParam.nRefStatusAvailableSpaceC;
			break;
			case 'D':
			nUpperbound = m_DiagParam.nRefStatusAvailableSpaceD;
			break;
		}

		//사용비율 계산
		try
		{
			nUsingRate = ((double)pInfo->UsedSpace / (double)pInfo->TotalSpace) * 100;
		}
		catch(Exception& e)
		{
			ADD_LOG_DIAG("[Error] Divided by 0 in CheckDiskStatus");
		}

		//사용량이 상한선보다 큼
		if(nUpperbound < nUsingRate)
		{
			bResult = DIAG_OHT_STATUS_NG;
		}
	}

	return bResult;
}

/**
@brief	OHT Status 진단항목을 확인
@data   2016.04.19
@author	doori.shin
*/
void DiagManager::CheckOHTStatus()
{
	int nError = NO_ERR;
	DISK_INFO* pDiskInfo = NULL;	//디스크 사용량 확인할 때 사용

	//1. C Drive 사용량 확인
	pDiskInfo = new DISK_INFO;
	pDiskInfo->Drive = 'C';
	m_DiagStatusData.bIsAvailableCDrive = CheckDiskStatus(pDiskInfo);
	delete pDiskInfo;

	//2. D Drive 사용량 확인
	pDiskInfo = new DISK_INFO;
	pDiskInfo->Drive = 'D';
	m_DiagStatusData.bIsAvailableDDrive = CheckDiskStatus(pDiskInfo);
	delete pDiskInfo;

	//3. USB메모리에 데이터 기록여부 확인
    m_DiagStatusData.bIsLogSave = true;

	//4. USB메모리 인식확인
	m_DiagStatusData.bIsEnabledUSB = OHTUTILITY::Check_DDrive();

	//5. FAN동작여부 확인(확인필요)
	m_DiagStatusData.bIsEnabledFAN = OHTMainForm->GetHWSet()->m_pFan->IsFanFail() ? false : true;

	ADD_LOG_DIAG("Check OHT Status : LogSave(%d), EnabledUSB(%d), FanFail(%d), Disk C(%d), Disk D(%d)",
			m_DiagStatusData.bIsLogSave, m_DiagStatusData.bIsEnabledUSB,
			m_DiagStatusData.bIsEnabledFAN, m_DiagStatusData.bIsAvailableCDrive,
			m_DiagStatusData.bIsAvailableDDrive);

}

void DiagManager::CheckOHTStatusFail()
{
	int nError = NO_ERR;
	DISK_INFO* pDiskInfo = NULL;	//디스크 사용량 확인할 때 사용

	//1. C Drive 사용량 확인
	m_DiagStatusData.bIsAvailableCDrive = false;

	//2. D Drive 사용량 확인
	m_DiagStatusData.bIsAvailableDDrive = false;


	//3. USB메모리에 데이터 기록여부 확인
    m_DiagStatusData.bIsLogSave = false;

	//4. USB메모리 인식확인
	m_DiagStatusData.bIsEnabledUSB = false;

	//5. FAN동작여부 확인(확인필요)
	m_DiagStatusData.bIsEnabledFAN = OHTMainForm->GetHWSet()->m_pFan->IsFanFail() ? false : true;

	ADD_LOG_DIAG("Check OHT Status Fail : LogSave(%d), EnabledUSB(%d), FanFail(%d), Disk C(%d), Disk D(%d)",
			m_DiagStatusData.bIsLogSave, m_DiagStatusData.bIsEnabledUSB,
			m_DiagStatusData.bIsEnabledFAN, m_DiagStatusData.bIsAvailableCDrive,
			m_DiagStatusData.bIsAvailableDDrive);

}


/**
@brief	진단에 보낼 Status 변수를 초기화(보낼때 마다 초기화)
@data   2016.04.27
@author	doori.shin

void DiagManager::ClearDiagStatusData()
{
	memset(&m_DiagStatusData, 0x00, sizeof(DIAG_OHT_STATUS_DATA));
}
*/

/**
@brief	통계 항목에 대한 데이터가 있는 Map 자료구조를 가져옴
@data   2016.04.27
@author	doori.shin
*/
DIAG_STAT_DATA_MAP* DiagManager::GetDiagStatMap()
{
	return this->m_DiagDataMap;
}

/**
@brief	전/후방 엔코더값 이상감지 횟수를 가져옴
@data   2016.04.27
@author	doori.shin
*/
int DiagManager::GetAbnormalDrvDiffCount()
{
	return m_nDiagAbnormalDrvDiffCnt;
}

DIAG_OHT_STATUS_DATA* DiagManager::GetDiagOHTstatus()
{
    return &(this->m_DiagStatusData);
}

/**
@brief	전/후방 엔코더값 이상감지 횟수를 증가
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::IncreaseAbnormalDrvDiffCount()
{
	m_nDiagAbnormalDrvDiffCnt++;
}

/**
@brief	전/후방 엔코더값 이상감지 횟수를 초기화
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::ResetAbnormalDrvDiffCount()
{
	m_nDiagAbnormalDrvDiffCnt = 0;
}


/**
@brief	Hand Reset 이상감지 횟수를 가져옴
@data   2016.04.27
@author	doori.shin
*/
int DiagManager::GetAbnormalHandResetCount()
{
	return m_nDiagAbnormalHandResetCount;
}

/**
@brief	Hand Reset 이상감지 횟수를 증가
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::IncreaseAbnormalHandResetCount()
{
	m_nDiagAbnormalHandResetCount++;
}

/**
@brief	Hand Reset 이상감지 횟수를 초기화
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::ResetAbnormalHandResetCount()
{
	m_nDiagAbnormalHandResetCount = 0;
}

/**
@brief	24시간 체크하는 변수값 가져옴
@data   2016.04.27
@author	doori.shin
*/
int DiagManager::GetDiagCheckCount()
{
	return m_nDiagCheckCount;
}

/**
@brief	24시간 체크하는 변수값 증가
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::IncreaseDiagCheckCount()
{
	m_nDiagCheckCount++;
}

/**
@brief	전/24시간 체크하는 변수값 초기화
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::ResetDiagCheckCount()
{
	m_nDiagCheckCount = 0;
}

/**
@brief	OHT가 실행시 진단서버로 부터 통계 데이터를 받았는지 확인
@data   2016.04.27
@author	doori.shin
@return OK : 진단서버에서 통계 데이터를 받음
		NG : 진단서버에서 데이터 못받음
*/
bool DiagManager::IsDiagStatLoaded()
{
	return this->m_bIsLoadedDiagData;
}

/**
@brief	진단서버로 통계정보를 요청하는 콜백함수
@data   2016.05.16
@author	doori.shin
*/
void CALLBACK CheckInitStatDataProc(UINT _timerId, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if(DiagManager::getInstance()->IsDiagStatLoaded() == false)
	{
		OHTMainForm->GetTaskControl()->SendReqeustDiagStatData();
		DiagManager::getInstance()->IncreaseRetryCount();

		if(DiagManager::getInstance()->GetRetryCount() > DIAG_CHECK_TIMER_RETRY_COUNT)
		{
			//Timer 종료
			timeKillEvent(DiagManager::getInstance()->GetCheckTimerID());
			ADD_LOG_DIAG("[Inform] DiagStatCheckTimer is terminated..[RetryCount Over : %d]", DIAG_CHECK_TIMER_RETRY_COUNT);
		}
	}
}

/**
@brief	OHT가 실행시 진단서버에 통계정보 누적값 요청 타이머
@data   2016.05.16
@author	doori.shin
*/
void DiagManager::SetDiagStatCheckTimer()
{
	TIMECAPS timeCaps;

	timeGetDevCaps(&timeCaps, sizeof(TIMECAPS));

	if(m_nDiagCheckTimerID != NULL)
		timeKillEvent(m_nDiagCheckTimerID);

	m_nDiagCheckTimerID = timeSetEvent(DIAG_CHECK_INIT_STAT_DATA_INTERVAL,
										timeCaps.wPeriodMax,
										CheckInitStatDataProc,
										(DWORD)0,
										TIME_PERIODIC);

	if(m_nDiagCheckTimerID == NULL)
	{
		ADD_LOG_DIAG("[Error] Fail to Create DiagStatCheckTimer...");
	}
	else
	{
		ADD_LOG_DIAG("Success to Create DiagStatCheckTimer. [ID:%d]", m_nDiagCheckTimerID);
	}
}

/**
@brief	Retry Count 증가
@data   2016.05.16
@author	doori.shin
*/
void DiagManager::IncreaseRetryCount()
{
	this->m_nTimerRetryCount++;
}

/**
@brief	Retry Count 가져옴
@data   2016.05.16
@author	doori.shin
*/
int DiagManager::GetRetryCount()
{
	return this->m_nTimerRetryCount;
}

/**
@brief	Timer ID를 가져옴
@data   2016.05.16
@author	doori.shin
*/
int DiagManager::GetCheckTimerID()
{
	return this->m_nDiagCheckTimerID;
}

void DiagManager::Reset_DiagManager()
{
	ADD_LOG_DIAG("[PARAM APPLY] 통계 데이터 반납.....");
	
	//통계 데이터 반납 (from ~DiagManager())
	if(m_DiagDataMap != NULL)
	{
		for(DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->begin(); iter != m_DiagDataMap->end(); iter++)
		{
			ADD_LOG_DIAG("[PARAM APPLY] data set return..");
				
			DIAG_OHT_STATISTICS_DATA* pDelData = (DIAG_OHT_STATISTICS_DATA*)iter->second;
			delete pDelData;
		}

		m_DiagDataMap->clear();

		ADD_LOG_DIAG("[PARAM APPLY] Data Cleared");
		// delete m_DiagDataMap;

	}
	else
	{
		ADD_LOG_DIAG("[PARAM APPLY] data re alloc");
		m_DiagDataMap = new map<BYTE, DIAG_OHT_STATISTICS_DATA*>();
    }

	// data remake
	ADD_LOG_DIAG("[PARAM APPLY] data remake....");
	MakeStatDataMap();
}
//AOHC-196 사전 로그 추가
// 모니터링 로그 모든 항목 로깅하기 (한줄)
void DiagManager::LogginPreviosDiagCount()
{
	char buf[2048];
	memset(buf, 0, sizeof(buf));
	EnterCriticalSection(&m_CSPreDiag);
	
	for(int i=0; i<COUNTOF_PREVIOUS ; i++)
	{
		char name[32];	// 모니터링 로그 항목 이름 얻기
		memset(name, 0, sizeof(name));
		GetPrevIdxName((PREVIOS_DIAG_PARAM_NUM)i, name);

		char item[64];	// 이름/값 형태로 만들기
		memset(item, 0, sizeof(item));
		sprintf(item, "%s/%d/", name, m_nArrDiagVal[i]);

		strcat(buf, item);	// 버퍼에 쌓기
	}
	
	LeaveCriticalSection(&m_CSPreDiag);
	// 로깅
	ADD_PREVIOSDIAG_LOG(buf);

	m_dwPrevLogStartTick = timeGetTime();	// 마지막 로깅 시간 기억

}

// 해당 항목 카운트 증가
void DiagManager::IncreasePreviosDiagCount(PREVIOS_DIAG_PARAM_NUM nType)
{
	if(nType >= COUNTOF_PREVIOUS || nType < 0)
		return;
	
	EnterCriticalSection(&m_CSPreDiag);
	if(m_nArrDiagVal[nType] < INT_MAX)
		m_nArrDiagVal[nType]++;
	LeaveCriticalSection(&m_CSPreDiag);
}

// 해당 항목 최대값 저장
void DiagManager::IncreasePreviosDiagMax(PREVIOS_DIAG_PARAM_NUM nType, int dwTime)
{
	if(nType >= COUNTOF_PREVIOUS || nType < 0)
		return;
	
	EnterCriticalSection(&m_CSPreDiag);
	
	// 시간 최대값을 저장하도록 구현
	if(dwTime > m_nArrDiagVal[nType])
		m_nArrDiagVal[nType] = dwTime;
	
	LeaveCriticalSection(&m_CSPreDiag);
}

// 모니터링 로그 초기화
void DiagManager::ResetPreviosDiagCount()
{
	EnterCriticalSection(&m_CSPreDiag);
	memset(&m_nArrDiagVal, 0, sizeof(m_nArrDiagVal));
	LeaveCriticalSection(&m_CSPreDiag);
}

// 모니터링 로그 값 얻기
int DiagManager::GetPreviosValue(PREVIOS_DIAG_PARAM_NUM type)
{
	if(type >= COUNTOF_PREVIOUS || type < 0)
		return 0;
	
	int nRet = 0;
	EnterCriticalSection(&m_CSPreDiag);
	
	nRet = m_nArrDiagVal[type];
	
	LeaveCriticalSection(&m_CSPreDiag);
	
	return nRet;
}

// 모니터링 로그 기록 시간에 맞춰서 기록하도록 체크하는 함수
bool DiagManager::PrevLogTimeChk()
{
	if(m_dwPrevLogStartTick == 0)
		m_dwPrevLogStartTick = timeGetTime();  // 처음에 호출됐을때 초기화

	DWORD dwElapse = OHTUTILITY::PreventOverflowGetTimeMs(m_dwPrevLogStartTick);
	const static int nInterval_ms = PREVIOUS_LOG_WRITE_INTERVAL_MIN*60*1000;
	if(dwElapse > nInterval_ms)
		return true;

	return false;
}

void DiagManager::GetOverhaulLogStr(char* logStr)
{
	 for(DIAG_STAT_DATA_MAP_ITER iter = m_DiagOverhaulDataMap->begin(); iter != m_DiagOverhaulDataMap->end(); iter++)
	 {
		char name[32];
		memset(name,0,sizeof(name));
		GetStatisticsIdxName(iter->second->bID, name);

		if(strcmp(name, "") == 0) continue;

		char item[64];
		memset(item,0,sizeof(item));

		switch(iter->second->bID){
			case DIAG_DATA_MOVEDISTANCE_F:
			case DIAG_DATA_MOVEDISTANCE_R:
			case DIAG_DATA_SHIFTDIST:
			case DIAG_DATA_HOISTDIST:
			{
				double value = floor(iter->second->dTotalData * 1000000) / 1000000;
				sprintf(item,"%s,%0.6lf,", name, iter->second->dTotalData);
				break;
			}
			case DIAG_DATA_DRVBCRCOUNT:
			case DIAG_DATA_STABCRCOUNT:
			case DIAG_DATA_QR_COUNT_L:
			case DIAG_DATA_QR_COUNT_R:
			{
				sprintf(item,"%s,%.0f,", name, iter->second->dTotalData * iter->second->nUnit);
				break;
			}
			default:
			{
				sprintf(item,"%s,%.0f,", name, iter->second->dTotalData);
			}

		}

		strcat(logStr,item);
	 }
}

//최근 구동이력 정보 읽어오기
void DiagManager::SetOverhaulData()
{
	std::ifstream in("C:\\User\\OHT\\Diag\\DiagStat.txt");
	std::string recentlyData;

	if(!in.is_open())
	{
		return;
	}

	std::getline(in, recentlyData);

	std::istringstream stringStream(recentlyData);
	std::string strData;
	std::vector<std::string> DiagData;

	while(std::getline(stringStream, strData, ','))
	{
		DiagData.push_back(strData);
	}

	in.close();

	if(DiagData.size() == 0) return;

	DIAG_STAT_DATA_MAP_ITER iter = m_DiagOverhaulDataMap->begin();

	for(int i = 1; i < DiagData.size(); i+=2)
	{
		double dValue = std::atof(DiagData[i].c_str());

		switch(iter->second->bID){
			case DIAG_DATA_SHIFTREV:
			case DIAG_DATA_HOISTREV:
			case DIAG_DATA_ROTATECOUNT:
			{
				iter++;
				break;
			}
			case DIAG_DATA_DRVBCRCOUNT:
			case DIAG_DATA_STABCRCOUNT:
			case DIAG_DATA_QR_COUNT_L:
			case DIAG_DATA_QR_COUNT_R:
			{
				if(dValue > 0)
					UpdateOverhaulData(iter->second->bID, dValue);
				break;
            }
			default:
			{
                // 값이 음수, 문자가 아닐 때만 저장
				if(dValue > 0)
					iter->second->dTotalData = dValue;
            }
		}

		iter++;
	}

	return;
}


#pragma package(smart_init)

