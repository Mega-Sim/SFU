/**
@file    MapMakeProcess.cpp
@version OHT 8.i
@brief   MAPMake과 관련된 Class 멤버 함수를 구현한 cpp 파일
*/

#pragma hdrstop

#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <locale.h> // 경로명에 한글이 있는 경우를 위해 추가
#include "MapMakeProcess.h"
#include "PUtily.h"

//#define ADD_LOG(...)        WRITE_LOG_UNIT(pLogMapMakefile, __VA_ARGS__)

using namespace std;
#pragma hdrstop


//---------------------------------------------------------------------------

/**
@brief   MapMakeProcess Class default 생성자

         변수, 포인터 변수 초기화
@author  puting
@date    2019.09.13
*/
MapMakeProcess::MapMakeProcess()
{
	Init();

//	pLogMapMakefile = getLogUnit("D:\\log\\MapMakefile.log", 1024*2);

}
//---------------------------------------------------------------------------

/**
@brief   MapMakeProcess Class 소멸자.

         동적 할당된 배열, 포인터 변수를 delete한다.
@author  puting
@date    2019.09.13
@note
*/
MapMakeProcess::~MapMakeProcess()
{
    // 메모리 해제
	for(MAMMAKEPROCESS_MAP_ITER iter = m_MapMakeProcessData.begin(); iter != m_MapMakeProcessData.end(); iter++)
    {
		MAMMAKEPROCESS_LINK_INFO *pSTD = (MAMMAKEPROCESS_LINK_INFO *)iter->second;
        delete pSTD;
	}

	m_MapMakeProcessData.clear();


}
/**
@brief   MapMakeProcess 초기화
@author  puting
@date    2019.09.13
*/
void MapMakeProcess::Init()
{


	for(MAMMAKEPROCESS_MAP_ITER iter = m_MapMakeProcessData.begin(); iter != m_MapMakeProcessData.end(); iter++)
    {
		MAMMAKEPROCESS_LINK_INFO *pSTD = (MAMMAKEPROCESS_LINK_INFO *)iter->second;
        delete pSTD;
	}

	m_MapMakeProcessData.clear();

	m_MapMakeDrivingCount = 0;
}
//---------------------------------------------------------------------------

/**
@brief   MapMakeProcess Data의 개수를 얻어오는 함수
@author  puting
@date    2019.09.13
@return  Map에 저장된 Data의 개수를 리턴
*/
int MapMakeProcess::GetCount()
{
	return m_MapMakeProcessData.size();
}
//---------------------------------------------------------------------------
 /**
@brief   MapMakeProcess Data의 개수를 얻어오는 함수
@author  puting
@date    2019.09.13
@return  Map에 저장된 Data의 개수를 리턴
*/
int MapMakeProcess::GetCountMapMakeDriving()
{
	return m_MapMakeDrivingCount;
}
//---------------------------------------------------------------------------

/**
@brief   MapMakeProcess Data 구조체를 Map에 insert하는 함수(MapLoading시만 사용할것)
@author  puting
@date    2019.09.13
@param   addStationData : 추가할 Station Data 구조체
@return  insert가 성공한 경우 0, 그렇지 않은 경우 Error Code 리턴
*/
int MapMakeProcess::Insert(MAMMAKEPROCESS_LINK_INFO *addStationData)
{
    int Error = NO_ERR;

	MAMMAKEPROCESS_DATA_MAP_RESULT Result;

	MAMMAKEPROCESS_LINK_INFO *findSTD;


	findSTD = Find(addStationData->LinkNID);

    if(findSTD == NULL)
    {
        // 없는 경우는 추가
		Result = m_MapMakeProcessData.insert(MAMMAKEPROCESS_MAP_PAIR(addStationData->LinkNID, addStationData));

        if(Result.second == false)
        {
            Error = -1;
//			STATION_LOAD_LOG("STATION ERR_STDATA_INSERTFAIL : %d", addStationData->StationID);
        }
        else
        {
            // 추가 성공한 경우
//            m_bDataModified = true;
        }
	}
    else
    {
        // 있는 경우 중복 에러 처리
        Error = -2;
//		STATION_LOAD_LOG("STATION ERR_STDATA_DUPLICATION : %d", addStationData->StationID);
    }

	return Error;

}
//---------------------------------------------------------------------------

/**
@brief   MapMakeProcess Data 구조체를 Map에 Update하는 함수
@author  puting
@date    2019.09.13
@param   updateSTD : Update할 Station Data 구조체
//@return  Update가 성공한 경우 0, 그렇지 않은 경우 Error Code 리턴
*/
int MapMakeProcess::Update(MAMMAKEPROCESS_LINK_INFO *updateSTD)
{
    int Error = NO_ERR;

	MAMMAKEPROCESS_LINK_INFO *findSTD;

	findSTD = Find(updateSTD->LinkNID);

    if(findSTD == NULL)
	{
		MAMMAKEPROCESS_LINK_INFO *newData;
		newData = new MAMMAKEPROCESS_LINK_INFO;
		memmove(newData,updateSTD, sizeof(MAMMAKEPROCESS_LINK_INFO));
		Insert(newData);
        // 없는 경우는 에러 처리
    }
    else
    {
        // 있는 경우 업데이트 수행
		memcpy(findSTD, updateSTD, sizeof(MAMMAKEPROCESS_LINK_INFO));
    }

    return Error;
}

//---------------------------------------------------------------------------


/**
@brief   MapMakeProcess Data를 검색하는 함수
@author  puting
@date    2019.09.13
@param   StationID : Map 검색을 위한 Key
@return  찾은 경우 Station Data의 포인터, 못 찾은 경우 NULL을 리턴
*/
MAMMAKEPROCESS_LINK_INFO* MapMakeProcess::Find(UINT LinkNID)
{
	MAMMAKEPROCESS_MAP_ITER FindIter;

	FindIter = m_MapMakeProcessData.find(LinkNID);

    // Error 체크
	if(FindIter == m_MapMakeProcessData.end())
        return NULL;    // 찾지 못한 경우 Null을 리턴

    return FindIter->second;
}

/*
int MapMakeProcess::GetCountMapMakeDrivingReflash()
{

	MAMMAKEPROCESS_LINK_INFO *saveData;
	MAMMAKEPROCESS_MAP_ITER iter;
	m_MapMakeDrivingCount = 0;


	for(iter = m_MapMakeProcessData.begin(); iter != m_MapMakeProcessData.end(); ++iter)
	{
		saveData = iter->second;
		if(saveData->IsCheck == 0x01)
		   m_MapMakeDrivingCount++;
	}

	return m_MapMakeDrivingCount;
}
*/


void MapMakeProcess::InitMapMakeProcessMemory()
{
 	Init();
}
#pragma package(smart_init)
