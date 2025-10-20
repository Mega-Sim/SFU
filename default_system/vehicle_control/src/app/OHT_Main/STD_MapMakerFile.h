//---------------------------------------------------------------------------
/**
@file    STD_MapMakerFileH.h
@version OHT 7.5
@brief   STANDARD MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef STD_MapMakerFileH
#define STD_MapMakerFileH


#include <utility>
#include <map>
#include "../Common/Def_DataType.h"
#include "Def_OHT.h"
#include "Def_DataFile_STD.h"

#include "StationData.h"
//#include "LogHandler.h" // Log 작성 목적


//typedef파일명 Def_Variables.h
using namespace std;

// -------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack


/**
@brief   TEACHING.DAT의 data 관련 구조체
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_MapMakerFile
{
	BYTE MapMakerType;                  ///< MapMaker 정보 ex) Stationg, Teaching"
	UINT StationID;                     ///< Station Id
	BYTE PortType;                      ///< 이적재 Port 정보 ex) EQ, STB-L, STB-R, UTB"
	BYTE IsCheck;                      ///<  0x00 : MapMaker&티칭X, 0x01 : MapMaker&티칭 진행중 0x02 : MapMaker&티칭 완료  //0x03 : MapMaker&티칭 fail 처리(station 없음, 부모노드 다름, 등등)
} NODE_INFO_STD_MAPMAEKRFILE;
//stuct size : 34Byte

#pragma pack(pop) // Pack 해제

// -------------------------------------------
typedef std::map<UINT, NODE_INFO_STD_MAPMAEKRFILE *> MAPMAKERFILEDATA_MAP;
typedef MAPMAKERFILEDATA_MAP::iterator MAPMAKERFILEDATA_MAP_ITER;
typedef std::pair<UINT, NODE_INFO_STD_MAPMAEKRFILE *> MAPMAKERFILEDATA_MAP_PAIR;
typedef std::pair<MAPMAKERFILEDATA_MAP_ITER, bool> MAPMAKERFIL_DATA_MAP_RESULT;
/**
@class   STD_MapMakerFile
@author  puting
@date    2016.12.25
@brief   OHT에 사용되는 Standard Map에 관련된 Data, 함수를 정의한 class
*/
//---------------------------------------------------------------------------

class MapMakerFileManager_STD
{
private:
	bool m_bDataLoaded; ///< Map File Load Flag
	bool m_bDataModified;   ///< Data 수정 flag
	MAPMAKERFILE_HEADER_STD m_MapMakerFileHeader; ///< Map File Header
//	LogUnit *pLogMapMakefile; ///< Map Make STD관련 로그 변수

public:
	MAPMAKERFILEDATA_MAP m_MapMakerFileData;    ///< Station Data 자료구조(Map)
	//HEADER_INFO_STD_MAP
	
	MapMakerFileManager_STD();
	~MapMakerFileManager_STD();

	int LoadDataFromFile(String FileName);
	int SaveDataToFile(String FileName);

	MAPMAKERFILE_HEADER_STD* GetFileHeader();
//	void SetFileHeader(MAPMAKERFILE_HEADER_STD *pFileHeader);         //미사용


	// Map 자료구조 관련 함수
	void Init();
	int GetCount();

	// Map 자료구조 추가, 업데이트, 삭제, 검색  함수
	int Insert(NODE_INFO_STD_MAPMAEKRFILE *addStationData);
//	int Update(NODE_INFO_STD_MAPMAEKRFILE *updateSTD);
//	int Delete(BYTE MapMakerType, UINT StationID);
//	NODE_INFO_STD_MAPMAEKRFILE* Find(UINT nTypeAndStation);
//	NODE_INFO_STD_MAPMAEKRFILE* Find(BYTE MapMakerType, UINT StationID);

	//Map Maker및 티칭 필요한 부분 확인.
//	NODE_INFO_STD_MAPMAEKRFILE* FindUpdate(BYTE MapMakerType);               //미사용
	bool ProcessingFindMapMakerFileUpdate(UINT CurNode, StationDataManager *TempStationManager, UINT &EndNodeID, bool &bMapMaker);
	bool ProcessingFindTeachingFileUpdate(UINT CurStation, UINT &TargetStationID, bool &bIsMove,BYTE &PortType);

	bool SucessesFindUpdate(BYTE MapMakerType, UINT StationID);

//	bool MapMakerFileFindUpdate(BYTE MapMakerType, UINT StationID);           //미사용


	void InitFindUpdate();    //에러 발생 시 진행중인 부분 초기화(0x01 -> 0x00)

	//추가.
	bool IsMapMakeFileLoading();   //Map Maker file Loading 유무
//	void InitMapMakeFileMemory(); //Loading된 Map maker File 초기화. 파일 전송완료할때         //미사용

};
#endif //STD_StationH
