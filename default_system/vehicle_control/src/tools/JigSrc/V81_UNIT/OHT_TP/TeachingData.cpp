//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <locale.h> // 경로명에 한글이 있는 경우를 위해 추가
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
@brief   FileHeader를 리턴
@author  doori.shin
@date    2013.07.26
*/
FILE_HEADER* TeachingDataManager::GetFileHeader()
{
	return &m_FileHeader;
}
//---------------------------------------------------------------------------

/**
@brief   FileHeader 셋팅
@author  doori.shin
@date    2013.07.26
*/
void TeachingDataManager::SetFileHeader(FILE_HEADER* pHeader)
{
	memcpy(&m_FileHeader, pHeader, sizeof(FILE_HEADER));
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
@brief   Binary 파일에서 Teaching Data 정보를 load하는 함수
@author  zzang9un
@date    2013.06.23
@param   TechingDataFileName : 경로를 포함한 파일명
@return  성공한 경우 0을 리턴, 에러 발생 시 에러 코드 리턴
*/
int TeachingDataManager::LoadDataFromFile(String FileName)
{
    setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

    int Error = NO_ERR;

	ifstream fs;

	// Step 1. File Open
	fs.open(FileName.w_str(), ios::in | ios::binary);

	if(fs.is_open())
	{
        int DataNum = 0; // Data 개수
 
        Init();

        // Step 2. Read File Header 정보
        FILE_HEADER FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));
        
		fs.read((char *)&FileHeader, sizeof(FileHeader));

        // Step 3. 파싱 Header
        m_FileHeader = ParsingFileHeader(&FileHeader);

        if(m_FileHeader.NumOfData > 0)
        {
            DataNum = m_FileHeader.NumOfData;

            TEACHING_DATA *loadTD;

            // Step 3. Station Data를 Map에 Insert함
            for(int i = 0; i < DataNum; i++)
            {
                loadTD = new TEACHING_DATA;

                fs.read((char *)loadTD, sizeof(TEACHING_DATA));

                Insert(loadTD);
            }

            if(DataNum != GetCount())
                Error = ERR_TDATA_FORMAT;
            else
            {
                // Load가 성공한 경우
                m_bDataLoaded = true;
                m_bDataModified = false;
            }
        }
	}
    else
	{
		Error = ERR_TDATA_FILEOPEN;
	}

    fs.close();

    return Error;
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
int TeachingDataManager::SaveDataToFile(String FileName)
{
    int Error = NO_ERR;

    setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

    // 저장할 data가 있는 지 확인
    if(m_TeachingData.empty())
    {
        // Data가 없는 경우는 에러
        Error = ERR_TDATA_NODATA;
    }

    ofstream fs;

    // Step 1. File open
    fs.open(FileName.w_str(), ios::out | ios::binary);

    if(fs.is_open())
    {
        int DataNum = GetCount();

		//Step 2. Write File Header Information

        // Data 개수
        m_FileHeader.NumOfData = GetCount();
        
		if(m_FileHeader.NumOfData == 0)
			Error = ERR_TDATA_SAVEFAIL;
        else
            DataNum = m_FileHeader.NumOfData;

        //Step 3. Check Size and Make Header
        FILE_HEADER FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));
        
		if(Error == NO_ERR)
            FileHeader = MakeFileHeader(&m_FileHeader);

        if(Error == NO_ERR)
        {
    		fs.write((char *)&FileHeader, sizeof(FILE_HEADER));

            // Step 3. Station Data 정보 write
            TEACHING_DATA *saveData;
            TEACHING_DATA_MMAP_ITER iter;
            int SaveCnt = 0;

            for(iter = m_TeachingData.begin(); iter != m_TeachingData.end(); ++iter)
            {
                saveData = iter->second;
                fs.write((char *)saveData, sizeof(TEACHING_DATA));
                SaveCnt++;
            }

            if(DataNum != SaveCnt)
                Error = ERR_TDATA_SAVEFAIL;
            else
            {
                // 저장이 성공한 경우
                m_bDataModified = false;
            }
        }
    }
    else
    {
        Error = ERR_TDATA_FILEOPEN;
    }

    fs.close();

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

    TEACHING_DATA_MMAP_RESULT Result;
    TEACHING_DATA_MMAP_ITER Iter;

    TEACHING_DATA *findTD;

    // 동일한 Station ID와 PortType을 검색
    findTD = Find(addTeachingData->StationID, addTeachingData->PortType);

    if(findTD == NULL)
    {
        // 없는 경우는 추가
        Iter = m_TeachingData.insert(TEACHING_DATA_PAIR(addTeachingData->StationID, addTeachingData));

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
int TeachingDataManager::Update(TEACHING_DATA *updateTD)
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
#pragma package(smart_init)
