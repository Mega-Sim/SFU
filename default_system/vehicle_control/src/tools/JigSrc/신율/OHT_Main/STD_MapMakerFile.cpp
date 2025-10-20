/**
@file    STD_MapMakerFileH.cpp
@version OHT 7.5
@brief   MAP과 관련된 Class 멤버 함수를 구현한 cpp 파일
*/

#pragma hdrstop

#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <locale.h> // 경로명에 한글이 있는 경우를 위해 추가
#include "STD_MapMakerFile.h"
#include "PUtily.h"

//#define ADD_LOG(...)        WRITE_LOG_UNIT(pLogMapMakefile, __VA_ARGS__)

using namespace std;
#pragma hdrstop


//---------------------------------------------------------------------------

/**
@brief   STD_Teaching Class default 생성자

         변수, 포인터 변수 초기화
@author  puting
@date    2016.12.25
*/
MapMakerFileManager_STD::MapMakerFileManager_STD()
{
	Init();

//	pLogMapMakefile = getLogUnit("D:\\log\\MapMakefile.log", 1024*2);

}
//---------------------------------------------------------------------------

/**
@brief   MAP Class 소멸자.

         동적 할당된 배열, 포인터 변수를 delete한다.
@author  puting
@date    2016.12.25
@note
*/
MapMakerFileManager_STD::~MapMakerFileManager_STD()
{
    // 메모리 해제
	for(MAPMAKERFILEDATA_MAP_ITER iter = m_MapMakerFileData.begin(); iter != m_MapMakerFileData.end(); iter++)
    {
		NODE_INFO_STD_MAPMAEKRFILE *pSTD = (NODE_INFO_STD_MAPMAEKRFILE *)iter->second;
        delete pSTD;
	}

	m_MapMakerFileData.clear();


}

/**
@brief   FileHeader를 리턴
@author  doori.shin
@date    2013.07.26
*/
MAPMAKERFILE_HEADER_STD* MapMakerFileManager_STD::GetFileHeader()
{
	return &m_MapMakerFileHeader;
}
/**
@brief   FileHeader 셋팅
@author  doori.shin
@date    2013.07.26
*/
void MapMakerFileManager_STD::SetFileHeader(MAPMAKERFILE_HEADER_STD* pHeader)
{
	memcpy(&m_MapMakerFileHeader, pHeader, sizeof(MAPMAKERFILE_HEADER_STD));
}


/**
@brief   File로부터 Data를 Load하는 함수
@author  zzang9un
@date    2013.06.09
@param   FileName : File Name(경로명 전체 포함)
@return  정상인 경우 0, 그렇지 않은 경우 에러 코드 리턴
*/
int MapMakerFileManager_STD::LoadDataFromFile(String FileName)
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

        // Step 2. Read File Header Information
		MAPMAKERFILE_HEADER_STD FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));

		fs.read((char *)&FileHeader, sizeof(MAPMAKERFILE_HEADER_STD));

//		// Step 3. Check header
//		m_FileHeader = ParsingFileHeader(&FileHeader);

        // Step 3. Header 파싱  --> 현재 파싱 필요없음.
		//m_FlowControlFileHeader = ParsingMapFileHeader_STD(&FileHeader);
		memcpy(&m_MapMakerFileHeader,&FileHeader,sizeof(MAPMAKERFILE_HEADER_STD));
		ConvertEndian(&m_MapMakerFileHeader.ModelID);
		ConvertEndian(&m_MapMakerFileHeader.Version);
		ConvertEndian(&m_MapMakerFileHeader.SDataCount);
		ConvertEndian(&m_MapMakerFileHeader.TDataCount);
		ConvertEndian(&m_MapMakerFileHeader.DataType);

		if( (m_MapMakerFileHeader.SDataCount > 0) &&
			(m_MapMakerFileHeader.TDataCount > 0) &&
			(m_MapMakerFileHeader.SDataCount < NODE_COUNT_MAX) &&
			(m_MapMakerFileHeader.TDataCount < NODE_COUNT_MAX) )
        {
			DataNum = m_MapMakerFileHeader.SDataCount+m_MapMakerFileHeader.TDataCount;

			NODE_INFO_STD_MAPMAEKRFILE *loadSTD;

            // Step 4. Station Data를 Map에 Insert함
    		for(int i = 0; i < DataNum; i++)
            {
				loadSTD = new NODE_INFO_STD_MAPMAEKRFILE;

				fs.read((char *)loadSTD, sizeof(NODE_INFO_STD_MAPMAEKRFILE));

//				if(loadSTD->IsCheck ==0x01)
//				   loadSTD->IsCheck = 0x00;

				ConvertEndian(&loadSTD->StationID);

    			Insert(loadSTD);
            }

    		if(DataNum != GetCount())
                Error = -1;
            else
			{
				m_bDataLoaded = true;
                m_bDataModified = false;
            }
        }
	}
    else
	{
		Error = -1;
	}

    fs.close();

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Data를 File에 save하는 함수
@author  zzang9un
@date    2013.06.09
@param   FileName : File Name(경로명 전체 포함)
@return  정상인 경우 0, 그렇지 않은 경우 에러 코드 리턴
*/
int MapMakerFileManager_STD::SaveDataToFile(String FileName)
{
    int Error = NO_ERR;

    setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

    // 저장할 data가 있는 지 확인
	if(m_MapMakerFileData.empty())
	{
        // Data가 없는 경우는 에러
		Error = -1;
    }

    ofstream fs;

    // Step 1. File open
    fs.open(FileName.w_str(), ios::out | ios::binary);

    if(fs.is_open())
    {
        int DataNum = 0;

		// Step 2. Write File Header Information

        // Data 개수
		DataNum = GetCount();

		if((m_MapMakerFileHeader.SDataCount == 0) && (m_MapMakerFileHeader.TDataCount == 0))
			Error = -1;
        else
			DataNum = m_MapMakerFileHeader.TDataCount + m_MapMakerFileHeader.SDataCount;

		// Step 3. Check Size and Make Header
		MAPMAKERFILE_HEADER_STD FileHeader;
		memset(&FileHeader, 0x00, sizeof(FileHeader));

//            // Step 4. File Header 정보 write
		if(Error == NO_ERR)
		{
			ConvertEndian(&m_MapMakerFileHeader.ModelID);
			ConvertEndian(&m_MapMakerFileHeader.Version);
			ConvertEndian(&m_MapMakerFileHeader.SDataCount);
			ConvertEndian(&m_MapMakerFileHeader.TDataCount);
			ConvertEndian(&m_MapMakerFileHeader.DataType);
			// Header 정보 wirte
			fs.write((char *)&m_MapMakerFileHeader, sizeof(MAPMAKERFILE_HEADER_STD));
			ConvertEndian(&m_MapMakerFileHeader.ModelID);
			ConvertEndian(&m_MapMakerFileHeader.Version);
			ConvertEndian(&m_MapMakerFileHeader.SDataCount);
			ConvertEndian(&m_MapMakerFileHeader.TDataCount);
			ConvertEndian(&m_MapMakerFileHeader.DataType);
        }
//            FileHeader = MakeFileHeader(&m_FileHeader);

		if(DataNum > 0)
        {

//    		fs.write((char *)&FileHeader, sizeof(MAPMAKERFILE_HEADER_STD));

            // Step 4. Station Data 정보 write
			NODE_INFO_STD_MAPMAEKRFILE *saveData;
			MAPMAKERFILEDATA_MAP_ITER iter;
			int SaveCnt = 0;

			for(iter = m_MapMakerFileData.begin(); iter != m_MapMakerFileData.end(); ++iter)
            {
                saveData = iter->second;
                fs.write((char *)saveData, sizeof(NODE_INFO_STD_MAPMAEKRFILE));
                SaveCnt++;
            }

            // by zzang9un 2013.07.31 : Make된 Header의 값은 필터가 있어서 비교하면 안됨
    		if(DataNum != SaveCnt)
				Error = -1;
            else
            {
                // 저장이 성공한 경우
                m_bDataModified = false;
            }
        }
    }
    else
    {
		Error = -1;
    }

    fs.close();

    return Error;

}
//---------------------------------------------------------------------------

/**
@brief   StationDataManager 초기화
@author  zzang9un
@date    2013.06.08
*/
void MapMakerFileManager_STD::Init()
{
	m_bDataLoaded    = false;
	m_bDataModified  = false;

	memset(&m_MapMakerFileHeader, 0, sizeof(MAPMAKERFILE_HEADER_STD));

	for(MAPMAKERFILEDATA_MAP_ITER iter = m_MapMakerFileData.begin(); iter != m_MapMakerFileData.end(); iter++)
    {
		NODE_INFO_STD_MAPMAEKRFILE *pSTD = (NODE_INFO_STD_MAPMAEKRFILE *)iter->second;
        delete pSTD;
	}

	m_MapMakerFileData.clear();
}
//---------------------------------------------------------------------------

/**
@brief   Station Data의 개수를 얻어오는 함수
@author  zzang9un
@date    2013.06.13
@return  Map에 저장된 Data의 개수를 리턴
*/
int MapMakerFileManager_STD::GetCount()
{
	return m_MapMakerFileData.size();
}
//---------------------------------------------------------------------------

/**
@brief   Station Data 구조체를 Map에 insert하는 함수
@author  zzang9un
@date    2013.06.13
@param   addStationData : 추가할 Station Data 구조체
@return  insert가 성공한 경우 0, 그렇지 않은 경우 Error Code 리턴
*/
int MapMakerFileManager_STD::Insert(NODE_INFO_STD_MAPMAEKRFILE *addStationData)
{
    int Error = NO_ERR;

	MAPMAKERFIL_DATA_MAP_RESULT Result;

	NODE_INFO_STD_MAPMAEKRFILE *findSTD;

 	//식별자로 등록됨.
	static UINT tmepIndex = 0; //((addStationData->MapMakerType << 24) & 0xFF000000) | addStationData->StationID;

	tmepIndex++;
//	findSTD = Find(tmepIndex);

//	if(findSTD == NULL)
//    {
		// 없는 경우는 추가

//		ADD_LOG("Insert STID %c %d", addStationData->MapMakerType, addStationData->StationID);

		Result = m_MapMakerFileData.insert(MAPMAKERFILEDATA_MAP_PAIR(tmepIndex, addStationData));

        if(Result.second == false)
		{
            Error = -1;
        }
		else
        {
            // 추가 성공한 경우
            m_bDataModified = true;
        }
//    }
//	else
//	{
//		// 있는 경우 중복 에러 처리
//		Error = -1;
//    }

    return Error;
}
//---------------------------------------------------------------------------

///**
//@brief   Station Data 구조체를 Map에 Update하는 함수
//@author  zzang9un
//@date    2013.06.13
//@param   updateSTD : Update할 Station Data 구조체
//@return  Update가 성공한 경우 0, 그렇지 않은 경우 Error Code 리턴
//*/
//int MapMakerFileManager_STD::Update(NODE_INFO_STD_MAPMAEKRFILE *updateSTD)
//{
//    int Error = NO_ERR;
//
//    NODE_INFO_STD_MAPMAEKRFILE *findSTD;
//
//	UINT tmepIndex = ((updateSTD->MapMakerType << 24) & 0xFF000000) | updateSTD->StationID;
//	findSTD = Find(tmepIndex);
//
//    if(findSTD == NULL)
//    {
//        // 없는 경우는 에러 처리
//        Error = -1;
//    }
//    else
//    {
//        // 있는 경우 업데이트 수행
//		memcpy(findSTD, updateSTD, sizeof(NODE_INFO_STD_MAPMAEKRFILE));
//        m_bDataModified = true;
//    }
//
//    return Error;
//}

//---------------------------------------------------------------------------

/**
@brief   Station Data를 자료구조 Map에서 delete하는 함수
@date    2013.06.13
@param   StationID : Map 검색을 위한 Key
@return  성공한 경우 0, 실패한 경우 에러 코드 리턴
*/
//int MapMakerFileManager_STD::Delete(BYTE MapMakerType, UINT StationID)
//{
//    int Error = NO_ERR;
//
//    int DelCnt = 0;
//
//	MAPMAKERFILEDATA_MAP_ITER FindIter;
//
//	UINT tmepIndex = ((MapMakerType << 24) & 0xFF000000) | StationID;
//
//	FindIter = m_MapMakerFileData.find(tmepIndex);
//
//	//Error 체크, 없다면 에러
//	if(FindIter == m_MapMakerFileData.end())
//		Error = -1;
//    else
//    {
//        // 있는 경우 메모리 해제를 위해 Find
//        NODE_INFO_STD_MAPMAEKRFILE *tmpSTD;
//
//        tmpSTD = Find(tmepIndex);
//
//        delete tmpSTD;  // 메모리 해제
//
//        DelCnt = m_MapMakerFileData.erase(tmepIndex);
//
//        if(DelCnt == 1)
//            m_bDataModified = true;
//        else
//            Error = -1;
//    }
//
//    return Error;
//}

//---------------------------------------------------------------------------

///**
//@brief   Station Data를 검색하는 함수
//@author  puting
//@date    2013.06.09
//@param   StationID : Map 검색을 위한 Key
//@return  찾은 경우 Station Data의 포인터, 못 찾은 경우 NULL을 리턴
//*/
//NODE_INFO_STD_MAPMAEKRFILE* MapMakerFileManager_STD::Find(UINT nTypeAndStation)
//{
//    MAPMAKERFILEDATA_MAP_ITER FindIter;
//
//	FindIter = m_MapMakerFileData.find(nTypeAndStation);
//
//    // Error 체크
//	if(FindIter == m_MapMakerFileData.end())
//        return NULL;    // 찾지 못한 경우 Null을 리턴
//
//    return FindIter->second;
//}
//
///**
//@brief   Station Data를 검색하는 함수
//@author  puting
//@date    2013.06.09
//@param   StationID : Map 검색을 위한 Key
//@return  찾은 경우 Station Data의 포인터, 못 찾은 경우 NULL을 리턴
//*/
//NODE_INFO_STD_MAPMAEKRFILE* MapMakerFileManager_STD::Find(BYTE MapMakerType, UINT StationID)
//{
//    MAPMAKERFILEDATA_MAP_ITER FindIter;
//
//	UINT tmepIndex = ((MapMakerType << 24) & 0xFF000000) | StationID;
//
//	FindIter = m_MapMakerFileData.find(tmepIndex);
//
//    // Error 체크
//	if(FindIter == m_MapMakerFileData.end())
//        return NULL;    // 찾지 못한 경우 Null을 리턴
//
//    return FindIter->second;
//}


/**
@brief   Station Data를 검색하는 함수
@author  puting
@date    2013.06.09
@param   StationID : Map 검색을 위한 Key
@return  찾은 경우 Station Data의 포인터, 못 찾은 경우 NULL을 리턴
*/
NODE_INFO_STD_MAPMAEKRFILE* MapMakerFileManager_STD::FindUpdate(BYTE MapMakerType)
{
	NODE_INFO_STD_MAPMAEKRFILE *saveData;
	MAPMAKERFILEDATA_MAP_ITER iter;

	for(iter = m_MapMakerFileData.begin(); iter != m_MapMakerFileData.end(); ++iter)
	{
			saveData = iter->second;

			if(saveData->MapMakerType == MapMakerType)
			{
				if(saveData->IsCheck == 0x00)
					return iter->second;
            }
	}
	return NULL;
}

//bool MapMakerFileManager_STD::ProcessingFindUpdate(BYTE MapMakerType, UINT StationID)
//{
//	bool breturn= false;
//	NODE_INFO_STD_MAPMAEKRFILE *saveData;
//	MAPMAKERFILEDATA_MAP_ITER iter;
//
//	for(iter = m_MapMakerFileData.begin(); iter != m_MapMakerFileData.end(); ++iter)
//	{
//			saveData = iter->second;
//
//			if((saveData->MapMakerType == MapMakerType) && (saveData->StationID) && (saveData->IsCheck == 0x00))
//			{
//			   saveData->IsCheck = 0x01;
//			   return true;
//            }
//	}
//	return breturn;
//
//}


bool MapMakerFileManager_STD::SucessesFindUpdate(BYTE MapMakerType, UINT StationID)
{
	bool breturn= false;
	NODE_INFO_STD_MAPMAEKRFILE *saveData;
	MAPMAKERFILEDATA_MAP_ITER iter;


	for(iter = m_MapMakerFileData.begin(); iter != m_MapMakerFileData.end(); ++iter)
	{
			saveData = iter->second;

			if((saveData->MapMakerType == MapMakerType) && (saveData->StationID ==StationID))
			{
//				ADD_LOG("SucessesFindUpdate succese! %c SID :%d",saveData->MapMakerType, StationID);
			   saveData->IsCheck = 0x02;
			   return true;
            }
	}
	return breturn;

}

bool MapMakerFileManager_STD::MapMakerFileFindUpdate(BYTE MapMakerType, UINT StationID)
{
	bool breturn= false;
	NODE_INFO_STD_MAPMAEKRFILE *saveData;
	MAPMAKERFILEDATA_MAP_ITER iter;

	for(iter = m_MapMakerFileData.begin(); iter != m_MapMakerFileData.end(); ++iter)
	{
			saveData = iter->second;

			if((saveData->MapMakerType == MapMakerType) && (saveData->StationID ==StationID))
			{
//				ADD_LOG("MapMakerFileFindUpdate succese! %c SID :%d",saveData->MapMakerType, StationID);
			    return true;
            }
	}
	return breturn;

}

//bool MapMakerFileManager_STD::FailFindUpdate(BYTE MapMakerType, UINT StationID)
//{
//	bool breturn= false;
//	NODE_INFO_STD_MAPMAEKRFILE *saveData;
//	MAPMAKERFILEDATA_MAP_ITER iter;
//
//	for(iter = m_MapMakerFileData.begin(); iter != m_MapMakerFileData.end(); ++iter)
//	{
//			saveData = iter->second;
//
//			if((saveData->MapMakerType == MapMakerType) && (saveData->StationID) && (saveData->IsCheck == 0x01))
//			{
//			   saveData->IsCheck = 0x03;
//			   return true;
//            }
//	}
//	return breturn;
//
//}

bool MapMakerFileManager_STD::ProcessingFindTeachingFileUpdate(UINT CurStation, UINT &TargetStationID, bool &bIsMove, BYTE &PortType)
{
	bool breturn = false; //없음

   	NODE_INFO_STD_MAPMAEKRFILE *saveData;
	MAPMAKERFILEDATA_MAP_ITER iter;

//	ADD_LOG("ProcessingFindTeachingFileUpdate Step1");

	for(iter = m_MapMakerFileData.begin(); iter != m_MapMakerFileData.end(); ++iter)
	{
			saveData = iter->second;

			if(saveData->MapMakerType == 'T')
			{
               if(saveData->IsCheck == 0x00)
				{

					if(saveData->StationID == CurStation)
						bIsMove = false;
					else
						bIsMove = true;

					PortType =  saveData->PortType;
					TargetStationID = saveData->StationID;
					breturn = true;
                	break;
                }
			}
	}


	return breturn;
}

bool MapMakerFileManager_STD::ProcessingFindMapMakerFileUpdate(UINT CurNode, StationDataManager *TempStationManager, UINT &EndNodeID, bool &bMapMaker)
{
	bool breturn = false; //없음

	int nTempStartNode = 0;
	int nTempEndNode = 0;
	STATION_DATA *pSData = NULL;
//	memset(pSData, 0x00, sizeof(STATION_DATA));

	NODE_INFO_STD_MAPMAEKRFILE *saveData;
	MAPMAKERFILEDATA_MAP_ITER iter;

	for(int i =0; i>10; i++)
	{
        Sleep(30);
    }

//	ADD_LOG("ProcessingFindMapMakerFileUpdate Step1");
    Sleep(30);
	for(iter = m_MapMakerFileData.begin(); iter != m_MapMakerFileData.end(); ++iter)
	{
			saveData = iter->second;
//			ADD_LOG("ProcessingFindMapMakerFileUpdate Step2");
			Sleep(30);

			if(saveData->MapMakerType == 'S')
			{
				if(saveData->IsCheck == 0x00)
				{
					pSData = TempStationManager->Find(saveData->StationID);

					if (pSData == NULL) break;
					else
					{
						//초기 위치 확인
						if((nTempStartNode ==0) && (nTempEndNode==0))
						{
							//초기위치가 현재위치랑 다른경우 _일반 이동
							if(pSData->ParentNodeID !=CurNode)
							{
								bMapMaker = false;
								EndNodeID =  pSData->ParentNodeID;
								breturn = true;
//								ADD_LOG("ProcessingFindMapMakerFileUpdate First STID %d End:%d", saveData->StationID, EndNodeID);
								Sleep(30);
								break;
							}
							else //초기위치가 현재위치와 같은 경우  _ MapMaker 이동
							{
								nTempStartNode =  pSData->ParentNodeID;
								nTempEndNode = pSData->NextNodeID;
								EndNodeID = pSData->NextNodeID;
								saveData->IsCheck = 0x01;
								bMapMaker = true;
								breturn = true;
//								ADD_LOG("ProcessingFindMapMakerFileUpdate First_1 STID %d End:%d", saveData->StationID, EndNodeID);
								Sleep(30);
							}

						}
						else //추가 값 확인
						{
							if((pSData->ParentNodeID ==nTempStartNode) ||
							(pSData->ParentNodeID ==nTempEndNode))
							{
                             	nTempStartNode =  pSData->ParentNodeID;
								nTempEndNode = pSData->NextNodeID;
								EndNodeID = pSData->NextNodeID;
								saveData->IsCheck = 0x01;
								bMapMaker = true;
								breturn = true;

//								ADD_LOG("ProcessingFindMapMakerFileUpdate 2 S:%d E:%d STID %d End:%d", nTempStartNode,nTempEndNode, saveData->StationID, EndNodeID);
								Sleep(30);
							}
							else
								break;
						}

					}
				}

			}
			else
				break;
	}

	return breturn;

}


void MapMakerFileManager_STD::InitFindUpdate()
{

	NODE_INFO_STD_MAPMAEKRFILE *saveData;
	MAPMAKERFILEDATA_MAP_ITER iter;

	for(iter = m_MapMakerFileData.begin(); iter != m_MapMakerFileData.end(); ++iter)
	{
			saveData = iter->second;
//			 ADD_LOG("InitFindUpdate STID %c %d : %d",saveData->MapMakerType, saveData->StationID, saveData->IsCheck);
			if(saveData->IsCheck == 0x01)
			{
			   saveData->IsCheck = 0x00;
            }
	}

}

bool MapMakerFileManager_STD::IsMapMakeFileLoading()
{
    return m_bDataLoaded;
}

void MapMakerFileManager_STD::InitMapMakeFileMemory()
{
 	Init();
}
#pragma package(smart_init)
