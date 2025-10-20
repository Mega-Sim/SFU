//---------------------------------------------------------------------------


#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <locale.h> // 경로명에 한글이 있는 경우를 위해 추가
#include "OHTMain.h"

#include "StationData.h"

using namespace std;
#pragma hdrstop
//#define STATION_LOAD_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\StationLoad.log", __VA_ARGS__);
//수정
//#define STATION_LOAD_LOG(...)    WRITE_LOG_UNIT(pLogStationLoad, __VA_ARGS__)
//---------------------------------------------------------------------------

/**
@brief   StationDataManager 클래스 생성자
@author  zzang9un
@date    2013.06.08
*/
StationDataManager::StationDataManager()
{
	Init();
//	pLogStationLoad = getLogUnit("D:\\log\\StationLoad.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
}

/**
@brief   StationDataManager 클래스 소멸자
@author  zzang9un
@date    2013.06.08
*/
StationDataManager::~StationDataManager()
{
    // 메모리 해제
    for(STATION_DATA_MAP_ITER iter = m_StationData.begin(); iter != m_StationData.end(); iter++)
    {
        STATION_DATA *pSTD = (STATION_DATA *)iter->second;
        delete pSTD;
	}

    m_StationData.clear();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
@brief   Data를 File에 save하는 함수
@author  zzang9un
@date    2013.06.09
@param   FileName : File Name(경로명 전체 포함)
@return  정상인 경우 0, 그렇지 않은 경우 에러 코드 리턴
*/
int StationDataManager::SaveDataToFile(String FileName, int FileType)
{
    int Error = NO_ERR;

    setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

    // 저장할 data가 있는 지 확인
    if(m_StationData.empty())
    {
        // Data가 없는 경우는 에러
        Error = ERR_STDATA_NODATA;
		return Error;
    }


    // Step 1. File open
    //fs.open(FileName.w_str(), ios::out | ios::binary);

   
    int DataNum = GetCount();

    if( DataNum>0 )
    {

		//Step 2. Write File Header Information
        
        // 버전 정보 변경은 안함.
        // 헤더의 Data 종류 및 옵션비트 변경 1 bit : 파라미터 변경유bit
		if(FileType == DATATYPE_STATION )
		{
			OHTMainForm->GetStationDataManager_STD()->m_StationFileHeader.DataType |= bit_1; // 파라미터 변경유bit 설정
			OHTMainForm->GetStationDataManager_STD()->m_StationFileHeader.DataCount = DataNum;
			OHTMainForm->GetStation_STD()->m_NodeNum = DataNum;
		}
		else
		{
			//NOP
		}
        
		
        //Step 3. Teaching 정보 STD 포맷으로 변환 및 저장
        //노드 변경에 의한 추가 여부를 판단하지는 않음.
        if(Error == NO_ERR)
        {
            STATION_DATA *saveData;
            STATION_DATA_MAP_ITER iter;
            int SaveCnt = 0;
            
			if(FileType == DATATYPE_STATION )
			{
				//STD Station 구조체 변경 후 파일로 저장.
				STD_Station *pMapdata;
				pMapdata = OHTMainForm->GetStation_STD();

				for(iter = m_StationData.begin(); iter != m_StationData.end(); iter++)
	            {
					saveData = iter->second;

					pMapdata->m_NodeArray[SaveCnt].StationID = saveData->StationID;
					pMapdata->m_NodeArray[SaveCnt].NextNodeID = saveData->NextNodeID;
					pMapdata->m_NodeArray[SaveCnt].Offset = saveData->StationOffset;
					pMapdata->m_NodeArray[SaveCnt].ParentNodeID = saveData->ParentNodeID;
					pMapdata->m_NodeArray[SaveCnt].StationType = saveData->StationType;
					pMapdata->m_NodeArray[SaveCnt].Safety_Level = saveData->Safety_Level;
					pMapdata->m_NodeArray[SaveCnt].PhysicalID = saveData->PhysicalID;
					pMapdata->m_NodeArray[SaveCnt].Reserved1 = 0x00;
					pMapdata->m_NodeArray[SaveCnt].Reserved2 = 0x00;
//					pMapdata->m_NodeArray[SaveCnt].Reserved3 = 0x00;
//					pMapdata->m_NodeArray[SaveCnt].Reserved4 = 0x00;
//					pMapdata->m_NodeArray[SaveCnt].Reserved5 = 0x00;
//					pMapdata->m_NodeArray[SaveCnt].Reserved6 = 0x00;
					pMapdata->m_NodeArray[SaveCnt].IsDefaultMap.LOW_DefaultMapInfo = saveData->IsDefaultMap.LOW_DefaultMapInfo;  //수정 필요.
					
					SaveCnt++;
				}
				OHTMainForm->GetStationDataManager_STD()->SaveMapFile(FileName.w_str(), OHTMainForm->GetStation_STD());
			}
			else
			{
			}

            if(DataNum != SaveCnt)
            {
                Error = ERR_TDATA_SAVEFAIL;
            }
            else
            {
                // 저장이 성공한 경우
                m_bDataModified = false;
            }
        }
    }
    else
    {
        Error = ERR_TDATA_NODATA;
    }

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   StationDataManager 초기화
@author  zzang9un
@date    2013.06.08
*/
void StationDataManager::Init()
{
    m_bDataLoaded    = false;
    m_bDataModified  = false;

	memset(&m_FileHeader, 0, sizeof(FILE_HEADER));

    for(STATION_DATA_MAP_ITER iter = m_StationData.begin(); iter != m_StationData.end(); iter++)
    {
        STATION_DATA *pSTD = (STATION_DATA *)iter->second;
        delete pSTD;
	}

	m_StationData.clear();
}
//---------------------------------------------------------------------------

/**
@brief   Station Data의 개수를 얻어오는 함수
@author  zzang9un
@date    2013.06.13
@return  Map에 저장된 Data의 개수를 리턴
*/
int StationDataManager::GetCount()
{
    return m_StationData.size();
}
//---------------------------------------------------------------------------

/**
@brief   Station Data 구조체를 Map에 insert하는 함수
@author  zzang9un
@date    2013.06.13
@param   addStationData : 추가할 Station Data 구조체
@return  insert가 성공한 경우 0, 그렇지 않은 경우 Error Code 리턴
*/
int StationDataManager::Insert(STATION_DATA *addStationData)
{
    int Error = NO_ERR;

    STATION_DATA_MAP_RESULT Result;

    STATION_DATA *findSTD;

    findSTD = Find(addStationData->StationID);

    if(findSTD == NULL)
    {
        // 없는 경우는 추가
        Result = m_StationData.insert(STATION_DATA_PAIR(addStationData->StationID, addStationData));

        if(Result.second == false)
        {
            Error = ERR_STDATA_INSERTFAIL;            
//			STATION_LOAD_LOG("STATION ERR_STDATA_INSERTFAIL : %d", addStationData->StationID);
        }
        else
        {
            // 추가 성공한 경우
            m_bDataModified = true;
        }
    }
    else
    {
        // 있는 경우 중복 에러 처리
        Error = ERR_STDATA_DUPLICATION;
//		STATION_LOAD_LOG("STATION ERR_STDATA_DUPLICATION : %d", addStationData->StationID);
    }

    return Error;           
}
//---------------------------------------------------------------------------

/**
@brief   Station Data 구조체를 Map에 Update하는 함수
@author  zzang9un
@date    2013.06.13
@param   updateSTD : Update할 Station Data 구조체
@return  Update가 성공한 경우 0, 그렇지 않은 경우 Error Code 리턴
*/
int StationDataManager::Update(STATION_DATA *updateSTD)
{
    int Error = NO_ERR;

    STATION_DATA *findSTD;

    findSTD = Find(updateSTD->StationID);

    if(findSTD == NULL)
    {
        // 없는 경우는 에러 처리
        Error = ERR_STDATA_UPDATEFAIL;
    }
    else
    {
        // 있는 경우 업데이트 수행
        memcpy(findSTD, updateSTD, sizeof(STATION_DATA));
        m_bDataModified = true;
    }

    return Error;           
}
//---------------------------------------------------------------------------

/**
@brief   Station Data를 자료구조 Map에서 delete하는 함수
@date    2013.06.13
@param   StationID : Map 검색을 위한 Key
@return  성공한 경우 0, 실패한 경우 에러 코드 리턴
*/
int StationDataManager::Delete(UINT StationID)
{
    int Error = NO_ERR;
    
    int DelCnt = 0;
    
	STATION_DATA_MAP_ITER FindIter;

	FindIter = m_StationData.find(StationID);

	//Error 체크, 없다면 에러
	if(FindIter == m_StationData.end())
		Error = ERR_STDATA_DELETEFAIL;
    else
    {
        // 있는 경우 메모리 해제를 위해 Find
        STATION_DATA *tmpSTD;

        tmpSTD = Find(StationID);

        delete tmpSTD;  // 메모리 해제

        DelCnt = m_StationData.erase(StationID);

        if(DelCnt == 1)
            m_bDataModified = true;
        else
            Error = ERR_STDATA_DELETEFAIL;   
    }

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Station Data를 검색하는 함수
@author  zzang9un
@date    2013.06.09
@param   StationID : Map 검색을 위한 Key
@return  찾은 경우 Station Data의 포인터, 못 찾은 경우 NULL을 리턴
*/
STATION_DATA* StationDataManager::Find(UINT StationID)
{
    STATION_DATA_MAP_ITER FindIter;

    FindIter = m_StationData.find(StationID);

    // Error 체크
    if(FindIter == m_StationData.end())
        return NULL;    // 찾지 못한 경우 Null을 리턴

    return FindIter->second;
}
//---------------------------------------------------------------------------

/**
@brief   STD Map으로부터 7.0 Station Data를 Load하는 함수
@author  KJD
@date    2016.04.22
@param   TBD
@return  정상인 경우 0, 그렇지 않은 경우 에러 코드 리턴
*/
int StationDataManager::LoadDataFromSTDMap(MAP *pMapdata, NODE_INFO_STD_STATION *m_NodeArray, UINT DataNum)
{
	int Error = NO_ERR;

	Init();

	STATION_DATA *loadSTD;

	// Step 1. Station Data를 Map에 Insert함
	for(int i = 0; i < DataNum; i++)
	{
		loadSTD = new STATION_DATA;

		loadSTD->StationID = m_NodeArray[i].StationID;
		loadSTD->StationType = m_NodeArray[i].StationType;
		loadSTD->ParentNodeID = m_NodeArray[i].ParentNodeID;
		loadSTD->NextNodeID = m_NodeArray[i].NextNodeID;
		loadSTD->StationOffset = m_NodeArray[i].Offset;
		loadSTD->Safety_Level = m_NodeArray[i].Safety_Level;
		loadSTD->PhysicalID = m_NodeArray[i].PhysicalID;
		loadSTD->IsDefaultMap.LOW_DefaultMapInfo = m_NodeArray[i].IsDefaultMap.LOW_DefaultMapInfo;
		if(m_NodeArray[i].Offset > 5000)
		{
			Error = ERR_STDATA_OFFSET;
//			STATION_LOAD_LOG("STATION ERR_STDATA_OFFSET : %d", loadSTD->StationID);
		}
		int tmp = pMapdata->FindNodeIndex(loadSTD->ParentNodeID);
		if(tmp != INDEX_NONE)
		{
			if( !((pMapdata->m_NodeArray[tmp].NID == loadSTD->ParentNodeID)))
	//			  (pMapdata->m_PathArray[pMapdata->m_NodeArray[tmp].NextPathIndex[0]].EndNID == loadSTD->NextNodeID ||
	//			   pMapdata->m_PathArray[pMapdata->m_NodeArray[tmp].NextPathIndex[1]].EndNID == loadSTD->NextNodeID)))
			{
				Error = ERR_STDATA_LINK;
	//			STATION_LOAD_LOG("STATION ERR_STDATA_LINK : %d", loadSTD->StationID);
			}
			Error = Insert(loadSTD);
		}
//개발자 의도 확인 필요
//		else
//		{
//			Error = Error;
//		}
	}

	if(DataNum != GetCount())
	{
		Error = ERR_STDATA_FORMAT;
	}
	else
	{
		m_bDataLoaded = true;
		m_bDataModified = false;
	}

	return Error;
}

int StationDataManager::MemoryInsert(STATION_DATA *addStationData)
{
	int nError = NO_ERR;

	STATION_DATA *loadSTD;
	loadSTD = new STATION_DATA;

	loadSTD->StationID = addStationData->StationID;
	loadSTD->StationType = addStationData->StationType;
	loadSTD->ParentNodeID = addStationData->ParentNodeID;
	loadSTD->NextNodeID = addStationData->NextNodeID;
	loadSTD->StationOffset = addStationData->StationOffset;
	loadSTD->Safety_Level = addStationData->Safety_Level;

	Insert(loadSTD);

	return nError;
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
