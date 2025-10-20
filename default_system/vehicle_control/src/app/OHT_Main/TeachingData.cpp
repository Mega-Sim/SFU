//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <locale.h> // 경로명에 한글이 있는 경우를 위해 추가
#include "OHTMain.h"

using namespace std;
#pragma hdrstop

#include "TeachingData.h"
#include "Def_OHT.h"
//---------------------------------------------------------------------------

/**
@brief   TeachingDataManager 클래스 생성자
@author  zzang9un
@date    2013.06.23
*/
TeachingDataManager::TeachingDataManager()
{
    Init();
}
//---------------------------------------------------------------------------

/**
@brief   TeachingDataManager 클래스 소멸자
@author  zzang9un
@date    2013.06.23
*/
TeachingDataManager::~TeachingDataManager()
{
    // 메모리 해제
    for(TEACHING_DATA_MMAP_ITER iter = m_TeachingData.begin(); iter != m_TeachingData.end(); iter++)
    {
        TEACHING_DATA *pTD = (TEACHING_DATA *)iter->second;
        delete pTD;
	}

    m_TeachingData.clear();
}
//---------------------------------------------------------------------------

/**
@brief   데이터 개수에 따라 Teaching Data를 초기화 한다.
@author  zzang9un
@date    2013.06.23
*/
void TeachingDataManager::Init()
{
    m_bDataLoaded    = false;
	m_bDataModified  = false;

	memset(&m_FileHeader, 0, sizeof(FILE_HEADER));

    for(TEACHING_DATA_MMAP_ITER iter = m_TeachingData.begin(); iter != m_TeachingData.end(); iter++)
    {
        TEACHING_DATA *pTD = (TEACHING_DATA *)iter->second;
        delete pTD;
    }
    m_TeachingData.clear();
}
//---------------------------------------------------------------------------

/**
@brief   Binary 파일에 Teaching Data 정보를 Save하는 함수
@author  zzang9un
@date    2013.04.25
@param   TeachingDataFileName : 경로를 포함한 파일명
@return  성공한 경우 0을 리턴, 에러 발생 시 에러 코드 리턴
@note    파일 저장한 경우 수정여부 플래그를 false로 변경한다.
*/
int TeachingDataManager::SaveDataToFile(String FileName, int FileType)
{
    int Error = NO_ERR;

    setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

    // 저장할 data가 있는 지 확인
    if(m_TeachingData.empty())
    {
        // Data가 없는 경우는 에러
        Error = ERR_TDATA_NODATA;
		return Error;
    }


    // Step 1. Teaching Data 수 획득.
    //fs.open(FileName.w_str(), ios::out | ios::binary);

    int DataNum = GetCount();

    if( DataNum>0 )
    {

		//Step 2. Write File Header Information
        
        // 버전 정보 변경은 안함.
        // 헤더의 Data 종류 및 옵션비트 변경 1 bit : 파라미터 변경유bit
		if(FileType == DATATYPE_TEACHING )
		{
			//OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.Version 
			//	= OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.Version;
			OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.DataType |= bit_1; // 파라미터 변경유bit 설정
			OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.DataCount 
				= DataNum;	
			OHTMainForm->GetTeaching_STD()->m_NodeNum = DataNum;
		}
		else
		{
			//NOP
		}
        
		if(DataNum == 0)
		{
			Error = ERR_TDATA_SAVEFAIL;
		}

        //Step 3. Teaching 정보 STD 포맷으로 변환 및 저장
        //노드 변경에 의한 추가 여부를 판단하지는 않음.
        if(Error == NO_ERR)
        {
			TEACHING_DATA *saveData;
            TEACHING_DATA_MMAP_ITER iter;
			int SaveCnt = 0;
            
			if(FileType == DATATYPE_TEACHING )
			{
				//STD Teaching 구조체 생성 후 파일로 저장.
				STD_Teaching *pMapdata;
				pMapdata = OHTMainForm->GetTeaching_STD();
				if(pMapdata->m_NodeArray != NULL)
			    {
					// NULL이 아닌 경우는 delete 후에 동적 생성
					// 맵 데이터 갱신시 사용
			        delete [] pMapdata->m_NodeArray;
					pMapdata->m_NodeArray = new NODE_INFO_STD_TEACHING[DataNum];
			    }
				for(iter = m_TeachingData.begin(); iter != m_TeachingData.end(); iter++)
	            {
					saveData = iter->second;

					pMapdata->m_NodeArray[SaveCnt].StationID = saveData->StationID;
					pMapdata->m_NodeArray[SaveCnt].PortType = saveData->PortType;
					pMapdata->m_NodeArray[SaveCnt].Hoist = saveData->Hoist;
					pMapdata->m_NodeArray[SaveCnt].Shift = saveData->Shift;
					pMapdata->m_NodeArray[SaveCnt].Rotate = saveData->Rotate;
					pMapdata->m_NodeArray[SaveCnt].PIOType = saveData->PIOType;
					pMapdata->m_NodeArray[SaveCnt].PIODirection = saveData->PIODirection;
					pMapdata->m_NodeArray[SaveCnt].PIOID = saveData->PIOID;
					pMapdata->m_NodeArray[SaveCnt].PIOChannel = saveData->PIOChannel;
					pMapdata->m_NodeArray[SaveCnt].PIOTimeLevel = saveData->PIOTimeLevel;
					pMapdata->m_NodeArray[SaveCnt].AutoRecovery = saveData->TransOption.Info.AutoRecovery;
					pMapdata->m_NodeArray[SaveCnt].HandDetectEQ = saveData->TransOption.Info.HandDetectEQ;
					pMapdata->m_NodeArray[SaveCnt].STBOffSet = saveData->STBOffSet;
					pMapdata->m_NodeArray[SaveCnt].LookDown = saveData->LookDownLevel;
					pMapdata->m_NodeArray[SaveCnt].SpecialEQLevel =	saveData->SpecialEQLevel;   //AOHC-294
					pMapdata->m_NodeArray[SaveCnt].CarrierStage = saveData->PIOCS;
					pMapdata->m_NodeArray[SaveCnt].MaterialType = saveData->MaterialType;
					pMapdata->m_NodeArray[SaveCnt].Reserved2 = 0x00;
					pMapdata->m_NodeArray[SaveCnt].Reserved3 = 0x00;
					pMapdata->m_NodeArray[SaveCnt].Reserved4 = 0x00;
					pMapdata->m_NodeArray[SaveCnt].STBL_Shift = saveData->STBL_Shift;     //soming
					pMapdata->m_NodeArray[SaveCnt].IsDefaultMap.LOW_DefaultMapInfo = saveData->IsDefaultMap.LOW_DefaultMapInfo;

	                SaveCnt++;
	            }
				OHTMainForm->GetTeachingDataManager_STD()->SaveMapFile(FileName.w_str(), OHTMainForm->GetTeaching_STD());
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
@brief   Teaching Data의 개수를 얻어오는 함수
@author  zzang9un
@date    2013.06.23
@return  Map에 저장된 Data의 개수를 리턴
*/
int TeachingDataManager::GetCount()
{
    return m_TeachingData.size();
}
//---------------------------------------------------------------------------

/**
@brief   Teaching Data 구조체를 Map에 insert하는 함수
@author  zzang9un
@date    2013.06.23
@param   addStationData : 추가할 Station Data 구조체
@return  insert가 성공한 경우 0, 그렇지 않은 경우 Error Code 리턴
*/
int TeachingDataManager::Insert(TEACHING_DATA *addTeachingData)
{
    int Error = NO_ERR;
	int count = 0;//Teaching node 확인용

//    TEACHING_DATA_MMAP_RESULT Result;
    TEACHING_DATA_MMAP_ITER Iter;

    TEACHING_DATA *findTD;

    // 동일한 Station ID와 PortType을 검색
    findTD = Find(addTeachingData->StationID, addTeachingData->PortType);

    if(findTD == NULL)
	{
		// 없는 경우는 추가

        Iter = m_TeachingData.insert(TEACHING_DATA_PAIR(addTeachingData->StationID, addTeachingData));
		count = GetCount();//Teaching node 확인용
        if(Iter == m_TeachingData.end())
        {
            Error = ERR_TDATA_INSERTFAIL;
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
        Error = ERR_TDATA_DUPLICATION;
    }

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Teaching Data 구조체를 Map에 Update하는 함수
@author  zzang9un
@date    2013.06.23
@param   updateSTD : Update할 Teaching Data 구조체
@return  Update가 성공한 경우 0, 그렇지 않은 경우 Error Code 리턴
*/
int TeachingDataManager::TeachingUpdate(TEACHING_DATA *updateTD)
{
    int Error = NO_ERR;

    TEACHING_DATA *findTD;

    findTD = Find(updateTD->StationID, updateTD->PortType);

    if(findTD == NULL)
	{
        // 없는 경우는 에러 처리
        Error = ERR_TDATA_UPDATEFAIL;
    }
    else
    {
        // 있는 경우 업데이트 수행
        memcpy(findTD, updateTD, sizeof(TEACHING_DATA));
        m_bDataModified = true;
    }

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Teaching Data를 자료구조 Map에서 delete하는 함수
@date    2013.06.23
@param   StationID : Map 검색을 위한 Key
@param   PortType : Port Type
@return  성공한 경우 0, 실패한 경우 에러 코드 리턴
*/
int TeachingDataManager::Delete(UINT StationID, BYTE PortType)
{
    int Error = NO_ERR;
    int EraseCnt = 0;

	TEACHING_DATA_MMAP_ITER Iter;
    TEACHING_DATA_MMAP_RANGE Range;

    // Station ID로 된 Data 모두 검색
    Range = m_TeachingData.equal_range(StationID);

	for(Iter = Range.first; Iter != Range.second; Iter++)
    {
        TEACHING_DATA *pTmpData = NULL;

        pTmpData = Iter->second;

        if(PortType == pTmpData->PortType)
        {
            // PortType이 동일한 경우
            delete pTmpData;    // 해당 메모리 delete

            TEACHING_DATA_MMAP_ITER ResultIter;
            ResultIter = m_TeachingData.erase(Iter);    // multimap에서 삭제

            if(ResultIter == m_TeachingData.end())
                Error = ERR_TDATA_DELETEFAIL;
            else
            {
                // 삭제가 성공한 경우
                m_bDataModified = true;
            }            
        }
    }

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Teaching Data를 검색하는 함수
@author  zzang9un
@date    2013.06.23
@param   StationID : Map 검색을 위한 Key
@param   PortType : Port Type
@return  찾은 경우 Teaching Data의 포인터, 못 찾은 경우 NULL을 리턴
*/
TEACHING_DATA* TeachingDataManager::Find(UINT StationID, BYTE PortType)
{
    TEACHING_DATA *pReturn = NULL;  // 리턴할 Data의 포인터

    TEACHING_DATA_MMAP_ITER FindIter;
    TEACHING_DATA_MMAP_RANGE Range;

    // Station ID로 된 Data 모두 검색
    Range = m_TeachingData.equal_range(StationID);

    bool bFind = false;
    for(FindIter = Range.first; FindIter != Range.second; FindIter++)
    {
        TEACHING_DATA *pTmpData = NULL;

        pTmpData = FindIter->second;

        if(PortType == pTmpData->PortType)
        {
            // PortType이 동일한 경우
            pReturn = FindIter->second;
            bFind = true;
            break;
        }
    }

    if(!bFind)
        pReturn = NULL; // 찾지 못한 경우 NULL

    return pReturn;
}
//---------------------------------------------------------------------------

/**
@brief   STD Map으로부터 7.0 Teaching Data를 Load하는 함수
@author  KJD
@date    2016.04.22
@param   TBD
@return  정상인 경우 0, 그렇지 않은 경우 에러 코드 리턴
*/
int TeachingDataManager::LoadDataFromSTDMap(NODE_INFO_STD_TEACHING *m_NodeArray_Teach, UINT DataNum)
{
	int Error = NO_ERR;
	//int count = 0;//Teaching node 확인용
	Init();

	TEACHING_DATA *loadTD;

	OHTMainForm->GetMap()->MakeTeachingMap(m_NodeArray_Teach, DataNum);

	// Step 4. Station Data를 Map에 Insert함
	for(int i = 0; i < DataNum; i++)
	{
		loadTD = new TEACHING_DATA;

		loadTD->StationID = m_NodeArray_Teach[i].StationID;
		loadTD->PortType = m_NodeArray_Teach[i].PortType;
		loadTD->Hoist = m_NodeArray_Teach[i].Hoist;
		loadTD->Shift = m_NodeArray_Teach[i].Shift;
		loadTD->Rotate = m_NodeArray_Teach[i].Rotate;
		loadTD->PIODirection = m_NodeArray_Teach[i].PIODirection;
		loadTD->PIOTimeLevel = m_NodeArray_Teach[i].PIOTimeLevel;
		loadTD->PIOID = m_NodeArray_Teach[i].PIOID;
		loadTD->PIOType = m_NodeArray_Teach[i].PIOType;
		loadTD->PIOChannel = m_NodeArray_Teach[i].PIOChannel;
		loadTD->TransOption.Info.AutoRecovery = m_NodeArray_Teach[i].AutoRecovery & 0x01;
		loadTD->TransOption.Info.HandDetectEQ = m_NodeArray_Teach[i].HandDetectEQ & 0x01;
		loadTD->TransOption.Info.EQPIODisable_TP = 0; //STD MAP에 없음.
		loadTD->TransOption.Info.Oscillation = 1;     //STD MAP에 없음.
		loadTD->TransOption.Info.TransSound = 0;      //STD MAP에 없음.
		loadTD->TransOption.Info.RFReader = 0;        //STD MAP에 없음.
		loadTD->STBOffSet = m_NodeArray_Teach[i].STBOffSet;
		loadTD->LookDownLevel = m_NodeArray_Teach[i].LookDown;
		loadTD->SpecialEQLevel = m_NodeArray_Teach[i].SpecialEQLevel;   //AOHC-294
		loadTD->IsDefaultMap.LOW_DefaultMapInfo = m_NodeArray_Teach[i].IsDefaultMap.LOW_DefaultMapInfo;
		loadTD->STBL_Shift = m_NodeArray_Teach[i].STBL_Shift; //soming
		loadTD->PIOCS = m_NodeArray_Teach[i].CarrierStage;
		loadTD->MaterialType = m_NodeArray_Teach[i].MaterialType;

		Insert(loadTD);
		//count = GetCount();//Teaching node 확인용
	}

	if(DataNum != GetCount())
	{
		Error = ERR_TDATA_FORMAT;
	}
	else
	{
		// Load가 성공한 경우
		m_bDataLoaded = true;
		m_bDataModified = false;
	}

	return Error;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
