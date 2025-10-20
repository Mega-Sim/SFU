/**
@file    STD_CID.cpp
@version OHT 7.5
@brief   MAP과 관련된 Class 멤버 함수를 구현한 cpp 파일
*/

#pragma hdrstop

#include <iostream>
#include <fstream>
#include <locale.h> // 경로명에 한글이 있는 경우를 위해 추가
#include "STD_CID.h"
//#include "OHTMain.h"
//#include "ErrorCode.h"
#include "PUtily.h"
#include "crc16.h" 


#include <windows.h>
#include <stdio.h>
#include <string.h>


//#define ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\CRC.log", __VA_ARGS__)
//수정
//#define ADD_LOG_CID_CRC(...)    WRITE_LOG_UNIT(pLogCRC, __VA_ARGS__)

using namespace std;
//---------------------------------------------------------------------------

/**
@brief   MAP Class default 생성자

         변수, 포인터 변수 초기화
@author  KDJ
@date    2016.03.30
*/
STD_CID::STD_CID()
{

	m_NodeNum = 0;

    m_NodeArray = NULL;


    // CIDMap 초기화
    m_CIDMap.clear();

}
//---------------------------------------------------------------------------

/**
@brief   MAP Class 소멸자.

         동적 할당된 배열, 포인터 변수를 delete한다.
@author  KJD
@date    2016.03.30
@note
*/
STD_CID::~STD_CID()
{
    if(m_NodeArray != NULL)
        delete [] m_NodeArray;


    m_CIDMap.clear();


}

/**
@brief   MAP Class 생성자
		 Node 개수를 이용하여 동적 생성한다.
@author  KJD
@date    2016.03.30
@param   NodeNum : Node 개수
*/
void STD_CID::InitMap(int NodeNum)
{
    m_NodeNum = NodeNum;

    // 동적 생성하기 전 Null인지를 확인 후 생성
    if(m_NodeArray != NULL)
    {
		// NULL이 아닌 경우는 delete 후에 동적 생성
		// 맵 데이터 갱신시 사용
        delete [] m_NodeArray;
		m_NodeArray = new NODE_INFO_STD_CID[NodeNum];
    }
    else
    {
    	//NodeNum Max값으로 테스트 필요 ??
		m_NodeArray = new NODE_INFO_STD_CID[NodeNum];
    }

    m_CIDMap.clear();

}

//---------------------------------------------------------------------------
/**
@brief   MapDatamanager 클래스 디폴트 생성자
         변수 초기화
@author  KJD
@date    2016.03.30

*/
CIDManager_STD::CIDManager_STD()
{
//	pLogCRC = getLogUnit("D:\\log\\CRC.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_MapFileLoaded = false;

	memset(&m_CIDFileHeader, 0x00, sizeof(FILE_HEADER_STD));
}
//---------------------------------------------------------------------------

/**
@brief   MapDatamanager 클래스 소멸자
         FileStream 변수 메모리 해제
@author  KJD
@date    2016.03.30

*/
CIDManager_STD::~CIDManager_STD()
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
int CIDManager_STD::LoadMapFile(TCHAR *FileName, STD_CID *pMapdata)
//int MapDataManager_STD::LoadMapFile(TCHAR *FileName)
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
        memset(&m_CIDFileHeader, 0x00, sizeof(m_CIDFileHeader));

        // Step 2. Read File Header Information
		FILE_HEADER_STD FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));

		fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));

        // Step 3. Header 파싱  --> 현재 파싱 필요없음.
		//m_CIDFileHeader = ParsingMapFileHeader_STD(&FileHeader);
		memcpy(&m_CIDFileHeader,&FileHeader,sizeof(FILE_HEADER_STD));
		ConvertEndian(&m_CIDFileHeader.ModelID);
		ConvertEndian(&m_CIDFileHeader.Version);
		ConvertEndian(&m_CIDFileHeader.DataCount);
		ConvertEndian(&m_CIDFileHeader.DataType);
		
        if( (m_CIDFileHeader.DataCount > 0) && (m_CIDFileHeader.DataCount < NODE_COUNT_MAX) )
        {
        	int NodeSize = 0; // Node Data 총 size
            // Step 4. 파일에서 Load한 내용에 따라 Init
			pMapdata->InitMap(m_CIDFileHeader.DataCount);

			// Step 5. Node Size만큼 Read
			NodeSize = m_CIDFileHeader.DataCount * sizeof(NODE_INFO_STD_CID);

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
//					ADD_LOG_CID_CRC("[CRC16_ERR] CID File: cCRC16_header: 0x%x, cCRC16_file: 0x%x",cCRC16_header, cCRC16_file);
					Error = ERR_MAP_FILEOPENFAIL;
					return Error;
				}
			}
			else
			{
//				ADD_LOG_CID_CRC("[EOF_ERR] CID File");
				//맵 구조 수정 후 주석 부분 적용
				//Error = ERR_MAP_FILEOPENFAIL;
				//fs.close();
				//return Error;
			}
			
			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				//ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
				ConvertEndian(&pMapdata->m_NodeArray[i].ResetNode);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode1);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_Offset);
				
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_Offset);

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
int CIDManager_STD::SaveMapFile(TCHAR *FileName, STD_CID *pMapdata)
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
		if( (m_CIDFileHeader.DataCount == 0) || (m_CIDFileHeader.DataCount > NODE_COUNT_MAX))
		{
			Error = ERR_MAP_FILESAVEFAIL;
		}
		// Step 3. Make Header
        //String strTime;
        //strTime = Now().FormatString("yyyymmddhhnnss");
		//m_CIDFileHeader.UpdateTime = StrToInt64("0x" + strTime);
		//m_CIDFileHeader.VersionString = StrToInt64("0x" + strTime);

		//m_CIDFileHeader.DataCount = STD_HEADER.DataCount;    // Node 개수

        // step4. Header, Node 정보 write
		if(m_CIDFileHeader.DataCount > 0)
        {
			//fs.write((char *)&FileHeader, sizeof(HEADER_INFO_STD_CID));
			//fs.write((char *)&m_CIDFileHeader, sizeof(HEADER_INFO_STD_CID));
			ConvertEndian(&m_CIDFileHeader.ModelID);
			ConvertEndian(&m_CIDFileHeader.Version);
			ConvertEndian(&m_CIDFileHeader.DataCount);
			ConvertEndian(&m_CIDFileHeader.DataType);
			// Header 정보 wirte
			fs.write((char *)&m_CIDFileHeader, sizeof(FILE_HEADER_STD));
			cCRC16_header = crc16_ccitt((const char *)&m_CIDFileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&cCRC16_header);
			ConvertEndian(&m_CIDFileHeader.ModelID);
			ConvertEndian(&m_CIDFileHeader.Version);
			ConvertEndian(&m_CIDFileHeader.DataCount);
			ConvertEndian(&m_CIDFileHeader.DataType);

			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				//ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
				ConvertEndian(&pMapdata->m_NodeArray[i].ResetNode);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode1);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_Offset);
				
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_Offset);

			}
            // Node 정보 write
			int NodeSize = m_CIDFileHeader.DataCount * sizeof(NODE_INFO_STD_CID);
			fs.write((char *) pMapdata->m_NodeArray, NodeSize);

			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				//ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
				ConvertEndian(&pMapdata->m_NodeArray[i].ResetNode);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode1);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_Offset);
				
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_Offset);

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
