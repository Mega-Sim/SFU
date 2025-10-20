/**
@file    STD_Station.cpp
@version OHT 7.5
@brief   MAP과 관련된 Class 멤버 함수를 구현한 cpp 파일
*/

#pragma hdrstop

#include <iostream>
#include <fstream>
#include <locale.h> // 경로명에 한글이 있는 경우를 위해 추가
#include "STD_Station.h"
//#include "OHTMain.h"
//#include "ErrorCode.h"
#include "PUtily.h"
#include "crc16.h" 


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "Def_PathInfo.h"


//#define ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\CRC.log", __VA_ARGS__)
//수정
//#define ADD_LOG(...)    WRITE_LOG_UNIT(pLogCRC, __VA_ARGS__)
using namespace std;
//---------------------------------------------------------------------------

/**
@brief   MAP Class default 생성자

         변수, 포인터 변수 초기화
@author  KDJ
@date    2016.03.30
*/
STD_Station::STD_Station()
{
    m_NodeNum = 0;

    m_NodeArray = NULL;

    // StationMap 초기화
    m_StationMap.clear();

}
//---------------------------------------------------------------------------

/**
@brief   MAP Class 소멸자.

         동적 할당된 배열, 포인터 변수를 delete한다.
@author  KJD
@date    2016.03.30
@note
*/
STD_Station::~STD_Station()
{
    if(m_NodeArray != NULL)
        delete [] m_NodeArray;


    m_StationMap.clear();


}

/**
@brief   MAP Class 생성자
		 Node 개수를 이용하여 동적 생성한다.
@author  KJD
@date    2016.03.30
@param   NodeNum : Node 개수
*/
void STD_Station::InitMap(int NodeNum)
{
    m_NodeNum = NodeNum;

    // 동적 생성하기 전 Null인지를 확인 후 생성
    if(m_NodeArray != NULL)
    {
		// NULL이 아닌 경우는 delete 후에 동적 생성
		// 맵 데이터 갱신시 사용
        delete [] m_NodeArray;
		m_NodeArray = new NODE_INFO_STD_STATION[NodeNum];
    }
    else
    {
    	//NodeNum Max값으로 테스트 필요 ??
		m_NodeArray = new NODE_INFO_STD_STATION[NodeNum];
    }

    m_StationMap.clear();

}

//---------------------------------------------------------------------------
/**
@brief   MapDatamanager 클래스 디폴트 생성자
         변수 초기화
@author  KJD
@date    2016.03.30

*/
StationManager_STD::StationManager_STD()
{
//	pLogCRC = getLogUnit("D:\\log\\CRC.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_MapFileLoaded = false;

	memset(&m_StationFileHeader, 0x00, sizeof(FILE_HEADER_STD));
}
//---------------------------------------------------------------------------

/**
@brief   MapDatamanager 클래스 소멸자
         FileStream 변수 메모리 해제
@author  KJD
@date    2016.03.30

*/
StationManager_STD::~StationManager_STD()
{
    m_MapFileLoaded = false;
}
//---------------------------------------------------------------------------

/**
@brief   FileHeader를 리턴
@author  doori.shin
@date    2013.07.26
*/
FILE_HEADER_STD* StationManager_STD::GetFileHeader()
{
	return &m_StationFileHeader;
}
//---------------------------------------------------------------------------

/**
@brief   FileHeader 셋팅
@author  doori.shin
@date    2013.07.26

void StationManager_STD::SetFileHeader(FILE_HEADER_STD* pHeader)
{
	memcpy(&m_StationFileHeader, pHeader, sizeof(FILE_HEADER_STD));
}
*/
/**
@brief   Map Data를 File에서 불러오는 함수
@author  KJD
@date    2016.03.30
@param   FileName : Map Data가 저장된 파일이름(MAPDATA.DAT)
@param   pMapdata : Map Data가 저장될 구조체 포인터
@return  성공일 경우 0를 리턴, 실패할 경우 에러 코드 리턴
@todo    에러 코드 정의 필요
*/
int StationManager_STD::LoadMapFile(TCHAR *FileName, STD_Station *pMapdata)
//int StationManager_STD::LoadMapFile(TCHAR *FileName)
{
    setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

    int Error = NO_ERR;
	unsigned short cCRC16_file;	
	unsigned short cCRC16_header;
	unsigned char cEOF[3];

    ifstream fs;

    // Step 1. File Open
    fs.open(FileName, ios::in | ios::binary);

    if(fs.is_open())
    {
        memset(&m_StationFileHeader, 0x00, sizeof(m_StationFileHeader));

        // Step 2. Read File Header Information
		FILE_HEADER_STD FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));

		fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));
		

        // Step 3. Header 파싱  --> 현재 파싱 필요없음.
		//m_StationFileHeader = ParsingMapFileHeader_STD(&FileHeader);
		memcpy(&m_StationFileHeader,&FileHeader,sizeof(FILE_HEADER_STD));
		ConvertEndian(&m_StationFileHeader.ModelID);
		ConvertEndian(&m_StationFileHeader.Version);
		ConvertEndian(&m_StationFileHeader.DataCount);
		ConvertEndian(&m_StationFileHeader.DataType);
		
        if( (m_StationFileHeader.DataCount > 0) && (m_StationFileHeader.DataCount < NODE_COUNT_MAX) )
        {
        	int NodeSize = 0; // Node Data 총 size
            // Step 4. 파일에서 Load한 내용에 따라 Init
			pMapdata->InitMap(m_StationFileHeader.DataCount);

			// Step 5. Node Size만큼 Read
			NodeSize = m_StationFileHeader.DataCount * sizeof(NODE_INFO_STD_STATION);

			fs.read((char *)pMapdata->m_NodeArray, NodeSize);
			fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
			fs.read((char *)&cEOF, sizeof(cEOF));
			ConvertEndian(&cCRC16_file);
			cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
			if(cEOF[0]=='E' && cEOF[1]=='O' && cEOF[2]=='F') //EOF 가 없으면 수정 전 맵으로 간주
			{
				if(cCRC16_header != cCRC16_file)
				{
					fs.close();
//					ADD_LOG("[CRC16_ERR] Station File: cCRC16_header: 0x%x, cCRC16_file: 0x%x",cCRC16_header, cCRC16_file);
					Error = ERR_MAP_FILEOPENFAIL;
					return Error;
				}
			}
			else
			{
//				ADD_LOG("[EOF_ERR] Station File");
				//맵 구조 수정 후 주석 부분 적용
				//Error = ERR_MAP_FILEOPENFAIL;
				//fs.close();
				//return Error;
			}
			
			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].StationID);
				ConvertEndian(&pMapdata->m_NodeArray[i].ParentNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].NextNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].Offset);
				ConvertEndian(&pMapdata->m_NodeArray[i].PhysicalID);
            }

            m_MapFileLoaded = true; // Map File Loaded flag 변경
		}
    }
    else
    {
        Error = ERR_MAP_FILEOPENFAIL;
	}

    fs.close();
    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Map Data를 File에 저장하는 함수
@author  KJD
@date    2016.03.30
@param   FileName : Map Data을 저장할 파일 이름
@param   pMapdata : 저장할 Map Data 구조체 포인터
@return  성공일 경우 0를 리턴, 실패할 경우 에러 코드 리턴
@todo    에러 코드 정의 필요
*/
int StationManager_STD::SaveMapFile(TCHAR *FileName, STD_Station *pMapdata)
{

	//setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

	int Error = NO_ERR;
	unsigned short cCRC16_header;
	unsigned char cEOF[3]={'E','O','F'};

    ofstream fs;

    // Step 1. File Open
	fs.open(FileName, ios::out | ios::binary);

    if(fs.is_open())
    {
        // Step 2. Write File Header Information
		if( (m_StationFileHeader.DataCount == 0) || (m_StationFileHeader.DataCount > NODE_COUNT_MAX))
		{
			Error = ERR_MAP_FILESAVEFAIL;
		}
		// Step 3. Make Header
        //String strTime;
        //strTime = Now().FormatString("yyyymmddhhnnss");
		//m_StationFileHeader.UpdateTime = StrToInt64("0x" + strTime);
		//m_StationFileHeader.VersionString = StrToInt64("0x" + strTime);

		//m_StationFileHeader.DataCount = STD_HEADER.DataCount;    // Node 개수

        // step4. Header, Node 정보 write
		if(m_StationFileHeader.DataCount > 0)
        {
			//fs.write((char *)&FileHeader, sizeof(HEADER_INFO_STD_Station));
			//fs.write((char *)&m_StationFileHeader, sizeof(HEADER_INFO_STD_Station));
			ConvertEndian(&m_StationFileHeader.ModelID);
			ConvertEndian(&m_StationFileHeader.Version);
			ConvertEndian(&m_StationFileHeader.DataCount);
			ConvertEndian(&m_StationFileHeader.DataType);
			// Header 정보 wirte
			fs.write((char *)&m_StationFileHeader, sizeof(FILE_HEADER_STD));
			cCRC16_header = crc16_ccitt((const char *)&m_StationFileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&cCRC16_header);
			ConvertEndian(&m_StationFileHeader.ModelID);
			ConvertEndian(&m_StationFileHeader.Version);
			ConvertEndian(&m_StationFileHeader.DataCount);
			ConvertEndian(&m_StationFileHeader.DataType);

			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].StationID);
				ConvertEndian(&pMapdata->m_NodeArray[i].ParentNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].NextNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].Offset);
				ConvertEndian(&pMapdata->m_NodeArray[i].PhysicalID);

			}
            // Node 정보 write
			int NodeSize = m_StationFileHeader.DataCount * sizeof(NODE_INFO_STD_STATION);
			fs.write((char *) pMapdata->m_NodeArray, NodeSize);
			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].StationID);
				ConvertEndian(&pMapdata->m_NodeArray[i].ParentNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].NextNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].Offset);
				ConvertEndian(&pMapdata->m_NodeArray[i].PhysicalID);

            }
			fs.write((char *)&cCRC16_header, sizeof(cCRC16_header));
			fs.write((char *)&cEOF, sizeof(cEOF));
		}
		fs.close();
    }
    else
    {
        Error = ERR_MAP_FILEOPENFAIL;
    }

	return Error;

}


#pragma package(smart_init)
