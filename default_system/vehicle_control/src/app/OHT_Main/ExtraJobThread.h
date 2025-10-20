//---------------------------------------------------------------------------
/**
@file    ExtraJobThread.h
@version OHT 7.0
@brief   Data를 Update하는 Thread를 구현한 Header file
*/

#ifndef ExtraJobThreadH
#define ExtraJobThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "ThreadCtrl.h"
#include "LogManager.h"
#include "StationData.h"
#include "TeachingData.h"
#include "MAP.h"
#include "TaskControl.h"
#include "OHTMainStatus.h"
#include "HWSet.h"

#include "STD_MapMakerFile.h"


//Test용 매크로(함수수행시간 구하기)
#define		SET_FREQ(t1)			 QueryPerformanceFrequency(&t1)
#define		START_TIME(t2)			 QueryPerformanceCounter(&t2)
#define		END_TIME(t3)		 	 QueryPerformanceCounter(&t3)
#define		GET_TIME(t1, t2, t3) (double)(t3.QuadPart-t2.QuadPart)/(double)(t1.QuadPart)

//2013.08.08 doori.shin Update Status
#define		UPDATE_STATE_CHECK_FILE		1
#define		UPDATE_STATE_FILE_LOAD		2
#define		UPDATE_STATE_FILE_COPY		3
#define		UPDATE_STATE_DATA_SWAP		4
#define		UPDATE_STATE_END_UPDATE		5
#define		UPDATE_STATE_UPDATE_FAIL	99
#define		UPDATE_STATE_ERR			-1


#define		UPDATEPARAM_STATE_CHECK_FILE		1
#define		UPDATEPARAM_STATE_FILE_COPY			2
#define		UPDATEPARAM_STATE_FILE_LOAD			3
#define		UPDATEPARAM_STATE_END_UPDATE		4
#define		UPDATEPARAM_STATE_UPDATE_FAIL	99
#define		UPDATEPARAM_STATE_ERR			-1

/**
@brief   Update 폴더의 File 정보
@author  doori.shin
@date    2013.08.06
*/
typedef struct UPDATE_FILE_INFO_
{
	BYTE	DataType;  	//데이터 타입
	UINT	Size;		//데이터 크기
	UINT	nDataNum;	//데이터 갯수
	UINT	Revision;	//데이터 리비전
	UINT64	UpdateTime;	//데이터 수정날짜
}UPDATE_FILE_INFO;

/**
@class   ExtraJobThread
@author  zzang9un
@brief   Main에서 부수적인 기능을 위한 Thread(우선 순위 낮음)
*/
class ExtraJobThread : public ThreadCtrl
{
private:
	MAPMAKE_STATUS *m_pMapMake_Status;
	TSearchRec	sr;

	//2013.08.06 For Update File
	String strOHTPath;
	int UpdateDataStep;
	int UpdateDataStepParam;

	DATA_USE_FLAG *m_pDataUseFlag;
   	OHTMainStatus *m_pOHTMainStatus;

	//2013.08.06 Temp Data Structure
	MAP *m_pTempMap;
	MapDataManager *m_pTempMapData;   ///< MapDataManager
	TeachingDataManager *m_pTempTeachingData;
	StationDataManager *m_pTempStationData;


	// Data
	STD_CID *m_TempCIDData_STD;
	STD_Collision *m_TempCollisionData_STD;
	STD_FlowControl *m_TempFlowControlData_STD;
	STD_MAP *m_TempMapData_STD;
	STD_Station *m_TempStationData_STD;
	STD_Teaching *m_TempTeachingData_STD;
	STD_PathData *m_TempPathData_STD;

	// Data Manager
	CIDManager_STD *m_TempCIDManager_STD;
	CollisionManager_STD *m_TempCollisionManager_STD;
	FlowControlManager_STD *m_TempFlowControlManager_STD;
	MapDataManager_STD *m_TempMapManager_STD;
	StationManager_STD *m_TempStationManager_STD;
	TeachingManager_STD *m_TempTeachingManager_STD;
	PathDataManager_STD *m_TempPathDataManager_STD;

	MapMakerFileManager_STD *m_TempMapMakerFileManager_STD;


	TaskControl *m_pTaskControl;

	unsigned int gUpdateErrCnt;

    // by zzang9un 2014.01.22 : Trace를 위해 변수 선언
    HWSet *m_pHwSet; ///< Trace를 위해 사용

    LogUnit *pLogUnit;

    void RealThreadProc(void);	// Execute 와 같은 함수
    bool CheckFileCopyCompleted(void);
	SIZE_T CheckMemoryInfo();                         //메모리 사용률

public:
	ExtraJobThread();
	~ExtraJobThread();

	int AutoUpdate(TSearchRec* pSchRec);
	bool CheckFileCopyCompleted(String path);

	int AutoUpdateParam();
	bool CheckFileCopyCompletedParam();
	String m_strFileParam[6];
	String m_strFileParamDST[6];
	int FindParamsetFiles();
	void DeleteParamsetFile(int nType);
	int m_nParamNum;

	bool CheckFileCopyCompletedParamDST();

//	int BackUpMapData();                 //미사용
//	int BackUpStationData();             //미사용
	int SaveMapData();
	int SaveStationData();
	int SaveTeachingData();

	bool IsTeachingDataModified();
	bool IsStationDataModified();

	inline int SwapUpdateData();
//	inline int DeleteUpdateTempData();      //미사용

	int DeleteMapsetFile();
	int BackupMapsetFiles();
//	int SaveMapsetFiles();                  //미사용
	int FindMapsetFiles();

	bool DeletebackupFile();
    
};

#endif
