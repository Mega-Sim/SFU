//---------------------------------------------------------------------------
/**
@file    StationData.h
$Rev:: 477           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-01-07 1#$:  Date of last commit
@version OHT 7.0
@brief   Station Data 관리를 위한 Header 파일
*/

#ifndef StationDataH
#define StationDataH

#include <map>
#include <utility>

#include "Def_OHT.h"
#include "Def_DataFile.h"
#include "STD_Station.h"
#include "MAP.h"

using namespace std;
//---------------------------------------------------------------------------

// by zzang9un 2013.07.19 : 통신 프로토콜 전송을 위해 1바이트로 Pack
#pragma pack(push, 1) // 1byte로 Pack

// Error Code
#define ERR_STDATA_MAKEMAP      -1
#define ERR_STDATA_FILEOPEN     -1001
#define ERR_STDATA_FORMAT       -2000   ///< Data Format Error
#define ERR_STDATA_DUPLICATION  -2001   ///< Data 중복
#define ERR_STDATA_INSERTFAIL   -2002   ///< Map Insert Fail
#define ERR_STDATA_DELETEFAIL   -2003   ///< Map Delete Fail
#define ERR_STDATA_UPDATEFAIL   -2004   ///< Map Update Fail
#define ERR_STDATA_NODATA       -2005   ///< Map Data 없음
#define ERR_STDATA_SAVEFAIL     -2006   ///< Save 에러
#define ERR_STDATA_OFFSET	  	-2007   ///< Save 에러
#define ERR_STDATA_LINK		  	-2008   ///< Save 에러

#define NUM_STATION_DATA_ELEMENT    5   ///< Station Data 구조체 변수 개수
/**
@brief   Station Data 구조체
@author  zzang9un
@date    2013.06.08
*/
typedef struct STATION_DATA_
{
    UINT    StationID;      ///< Station ID
    BYTE    StationType;    ///< Station Type
    UINT    ParentNodeID;   ///< Parent Node ID
    UINT    NextNodeID;     ///< Next Node ID
	int     StationOffset;  ///< Node로부터의 거리(mm)
	BYTE    Safety_Level;  ///< Node로부터의 거리(mm)
	UINT	PhysicalID;		//< 2단 STB, Physical Tag ID
	DEFAULT_MAPINFO    IsDefaultMap;  ///< Node로부터의 거리(mm)
} STATION_DATA;



#pragma pack(pop) // Pack 해제

// Station Data를 위한 map 설정
typedef std::map<UINT, STATION_DATA *> STATION_DATA_MAP;
typedef STATION_DATA_MAP::iterator STATION_DATA_MAP_ITER;
typedef std::pair<UINT, STATION_DATA *> STATION_DATA_PAIR;
typedef std::pair<STATION_DATA_MAP_ITER, bool> STATION_DATA_MAP_RESULT;

/**
@class   StationDataManager
@date    2013.06.08
@author  zzang9un
@brief   Station Data를 관리하는 Class

         1. Station Data는 map(자료구조)로 구성되어 있음
         2. binary 형태의 파일만 읽어올 수 있음
*/
class StationDataManager
{
private:
//	LogUnit *pLogStationLoad;      ///< StationLoad 로그 생성을 위한 변수

public:
    bool m_bDataLoaded;     ///< Data 로드 flag
	bool m_bDataModified;   ///< Data 수정 flag
	FILE_HEADER m_FileHeader;
    
public:
    STATION_DATA_MAP m_StationData;    ///< Station Data 자료구조(Map)
    
    StationDataManager();
    ~StationDataManager();

    // 파일 처리 함수
//	int LoadDataFromFile(String FileName);
	int SaveDataToFile(String FileName, int FileType);


	// Map 자료구조 관련 함수
	void Init();
	int GetCount();

	// Map 자료구조 추가, 업데이트, 삭제, 검색  함수
	int Insert(STATION_DATA *addStationData);
	int Update(STATION_DATA *updateSTD);
	int Delete(UINT StationID);
	STATION_DATA* Find(UINT StationID);

	int MemoryInsert(STATION_DATA *addStationData);
	int LoadDataFromSTDMap(MAP *pMapdata, NODE_INFO_STD_STATION *m_NodeArray, UINT DataNum);

};

//---------------------------------------------------------------------------

#endif

