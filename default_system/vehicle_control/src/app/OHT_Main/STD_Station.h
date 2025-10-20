//---------------------------------------------------------------------------
/**
@file    STD_StationH.h
@version OHT 7.5
@brief   STANDARD MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef STD_StationH
#define STD_StationH

#include <map>
#include "../Common/Def_DataType.h"
#include "Def_OHT.h"
//#include "Def_PathInfo.h"
//#include "Def_DataFile.h"
#include "Def_DataFile_STD.h"
//#include "LogHandler.h"	// Log 작성 목적


//typedef파일명 Def_Variables.h
using namespace std;

// -------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack



/**
@brief   STATION.DAT의 data 관련 구조체
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_Station
{
	UINT StationID;                     ///< Station Id
	BYTE StationType;                   ///< Station Type
	UINT ParentNodeID;                  ///< Station이 속해 있는 Link의 from Node Id
	UINT NextNodeID;                    ///< Station이 속해 있는 Link의 to Node Id
	WORD Offset;                        ///< Parent Node로부터의 거리(mm)
	BYTE Safety_Level;                  ///< 정지노드내 이적재 반납 유무 판단 플래그
	UINT PhysicalID;					///< 2단 STB, Physical Tag ID
	BYTE Reserved1;         			///< Reserved 3
	BYTE Reserved2;                     ///< Reserved 4
	DEFAULT_MAPINFO IsDefaultMap;       ///< Low사용할것.
} NODE_INFO_STD_STATION;
//stuct size : 23Byte

#pragma pack(pop) // Pack 해제

// -------------------------------------------

/**
@class   STD_Station
@author  KJD
@date    2016.03.30
@brief   OHT에 사용되는 Standard Map에 관련된 Data, 함수를 정의한 class
*/
class STD_Station
{
private:


public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_STATION *m_NodeArray; ///< Node 동적 배열 포인터

	std::map<int, int> m_StationMap;

	STD_Station();
	~STD_Station();

	void InitMap(int NodeNum);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
@class   MapDataManager
@author  KJD
@date    2016.03.30
@brief   Map Data를 관리하는 Class
*/
class StationManager_STD
{
private:
//	LogUnit *pLogCRC;      ///< CRC 로그 생성을 위한 변수
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_StationFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	StationManager_STD();
	~StationManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_Station *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_Station *pMapdata);
	FILE_HEADER_STD* GetFileHeader();
//	void SetFileHeader(FILE_HEADER_STD *pFileHeader);       //미사용

};
#endif //STD_StationH
