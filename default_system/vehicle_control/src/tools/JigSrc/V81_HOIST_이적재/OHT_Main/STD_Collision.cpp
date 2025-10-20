/**
@file    STD_Collision.cpp
@version OHT 7.5
@brief   STD_Collision과 관련된 Class 멤버 함수를 구현한 cpp 파일
*/

#pragma hdrstop

#include <iostream>
#include <fstream>
#include <locale.h> // 경로명에 한글이 있는 경우를 위해 추가
#include "STD_Collision.h"
//#include "OHTMain.h"
//#include "ErrorCode.h"
#include "PUtily.h"
#include "crc16.h" 


#include <windows.h>
#include <stdio.h>
#include <string.h>



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
STD_Collision::STD_Collision()
{

    m_NodeNum = 0;

    m_NodeArray = NULL;

    // CollisionMap 초기화
    m_CollisionMap.clear();

}
//---------------------------------------------------------------------------

/**
@brief   MAP Class 소멸자.

         동적 할당된 배열, 포인터 변수를 delete한다.
@author  KJD
@date    2016.03.30
@note
*/
STD_Collision::~STD_Collision()
{
    if(m_NodeArray != NULL)
        delete [] m_NodeArray;

    m_CollisionMap.clear();


}

/**
@brief   MAP Class 생성자
		 Node 개수를 이용하여 동적 생성한다.
@author  KJD
@date    2016.03.30
@param   NodeNum : Node 개수
*/
void STD_Collision::InitMap(int NodeNum)
{
    m_NodeNum = NodeNum;

    // 동적 생성하기 전 Null인지를 확인 후 생성
    if(m_NodeArray != NULL)
    {
		// NULL이 아닌 경우는 delete 후에 동적 생성
		// 맵 데이터 갱신시 사용
        delete [] m_NodeArray;
		m_NodeArray = new NODE_INFO_STD_COLLISION[NodeNum];
    }
    else
    {
    	//NodeNum Max값으로 테스트 필요 ??
		m_NodeArray = new NODE_INFO_STD_COLLISION[NodeNum];
    }

    m_CollisionMap.clear();

}

//---------------------------------------------------------------------------
/**
@brief   MapDatamanager 클래스 디폴트 생성자
         변수 초기화
@author  KJD
@date    2016.03.30

*/
CollisionManager_STD::CollisionManager_STD()
{
//	pLogCRC = getLogUnit("D:\\log\\CRC1.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_MapFileLoaded = false;

	memset(&m_CollisionFileHeader, 0x00, sizeof(FILE_HEADER_STD));
}
//---------------------------------------------------------------------------

/**
@brief   MapDatamanager 클래스 소멸자
         FileStream 변수 메모리 해제
@author  KJD
@date    2016.03.30

*/
CollisionManager_STD::~CollisionManager_STD()
{
    m_MapFileLoaded = false;
}
//---------------------------------------------------------------------------

/**
@brief   Map Data를 File에서 불러오는 함수
@author  KJD
@date    2016.03.30
@param   FileName : Map Data가 저장된 파일이름(MAPDATA.DAT)
@param   pMapdata : Map Data가 저장될 구조체 포인터
@return  성공일 경우 0를 리턴, 실패할 경우 에러 코드 리턴
@todo    에러 코드 정의 필요
*/
int CollisionManager_STD::LoadMapFile(TCHAR *FileName, STD_Collision *pMapdata)
//int CollisionManager_STD::LoadMapFile(TCHAR *FileName)
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
        memset(&m_CollisionFileHeader, 0x00, sizeof(m_CollisionFileHeader));

        // Step 2. Read File Header Information
		FILE_HEADER_STD FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));

		fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));
		

        // Step 3. Header 파싱  --> 현재 파싱 필요없음.
		//m_CollisionFileHeader = ParsingMapFileHeader_STD(&FileHeader);
		memcpy(&m_CollisionFileHeader,&FileHeader,sizeof(FILE_HEADER_STD));
		ConvertEndian(&m_CollisionFileHeader.ModelID);
		ConvertEndian(&m_CollisionFileHeader.Version);
		ConvertEndian(&m_CollisionFileHeader.DataCount);
		ConvertEndian(&m_CollisionFileHeader.DataType);
		
        if( (m_CollisionFileHeader.DataCount > 0) && (m_CollisionFileHeader.DataCount < NODE_COUNT_MAX) )
        {
        	int NodeSize = 0; // Node Data 총 size
            // Step 4. 파일에서 Load한 내용에 따라 Init
			pMapdata->InitMap(m_CollisionFileHeader.DataCount);

			// Step 5. Node Size만큼 Read
			NodeSize = m_CollisionFileHeader.DataCount * sizeof(NODE_INFO_STD_COLLISION);

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
//					ADD_LOG("[CRC16_ERR] Collision File: cCRC16_header: 0x%x, cCRC16_file: 0x%x",cCRC16_header, cCRC16_file);
					Error = ERR_MAP_FILEOPENFAIL;
					return Error;
				}
			}
			else
			{
//				ADD_LOG("[EOF_ERR] Collision File");
				//맵 구조 수정 후 주석 부분 적용
				//Error = ERR_MAP_FILEOPENFAIL;
				//fs.close();
				//return Error;
			}
			
            for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].NodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeID);
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
int CollisionManager_STD::SaveMapFile(TCHAR *FileName, STD_Collision *pMapdata)
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
		if( (m_CollisionFileHeader.DataCount == 0) || (m_CollisionFileHeader.DataCount > NODE_COUNT_MAX))
		{
			Error = ERR_MAP_FILESAVEFAIL;
		}
		// Step 3. Make Header
        //String strTime;
        //strTime = Now().FormatString("yyyymmddhhnnss");
		//m_CollisionFileHeader.UpdateTime = StrToInt64("0x" + strTime);
		//m_CollisionFileHeader.VersionString = StrToInt64("0x" + strTime);

		//m_CollisionFileHeader.DataCount = STD_HEADER.DataCount;    // Node 개수

        // step4. Header, Node 정보 write
		if(m_CollisionFileHeader.DataCount > 0)
        {
			//fs.write((char *)&FileHeader, sizeof(HEADER_INFO_STD_Collision));
			//fs.write((char *)&m_CollisionFileHeader, sizeof(HEADER_INFO_STD_Collision));
			ConvertEndian(&m_CollisionFileHeader.ModelID);
			ConvertEndian(&m_CollisionFileHeader.Version);
			ConvertEndian(&m_CollisionFileHeader.DataCount);
			ConvertEndian(&m_CollisionFileHeader.DataType);
			// Header 정보 wirte
			fs.write((char *)&m_CollisionFileHeader, sizeof(FILE_HEADER_STD));
			cCRC16_header = crc16_ccitt((const char *)&m_CollisionFileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&cCRC16_header);
			ConvertEndian(&m_CollisionFileHeader.ModelID);
			ConvertEndian(&m_CollisionFileHeader.Version);
			ConvertEndian(&m_CollisionFileHeader.DataCount);
			ConvertEndian(&m_CollisionFileHeader.DataType);

			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].NodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeID);

			}
            // Node 정보 write
			int NodeSize = m_CollisionFileHeader.DataCount * sizeof(NODE_INFO_STD_COLLISION);
			fs.write((char *) pMapdata->m_NodeArray, NodeSize);

			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].NodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeID);

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
//---------------------------------------------------------------------------
/**
@brief   Node 구조체를 map 컨테이너로 변환시키는 함수.
@author  KJD
@date    2016.04.29

*/
#if 0
void STD_Collision::MakeCollisionMap_STD()
{
    if((m_NodeNum == 0) || (m_NodeArray == NULL))
		return;

    for(int i = 0; i < m_NodeNum; i++)
    {
        // Key는 Path ID, Value는 배열 Index
		m_CollisionMap[m_NodeArray[i].NodeID] = i;
	}
}
#endif 
#pragma package(smart_init)
