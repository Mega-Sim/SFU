//---------------------------------------------------------------------------
/**
@file    MapMakeProcess.h
@version OHT 8.i
@brief   STANDARD MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef MapMakeProcessH
#define MapMakeProcessH


#include <utility>
#include <map>
#include "../Common/Def_DataType.h"
#include "Def_OHT.h"
#include "Def_DataFile_STD.h"

//#include "LogHandler.h" // Log 작성 목적


using namespace std;
// -------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack
/**
@brief   MapMakeProcess Data 관련 구조체
@author  puting
@date    2019.09.02
*/
typedef struct MAMMAKEPROCESS_LINK_INFO
{
	UINT LinkNID;                     ///< 식별자 : 분기방향(왼쪽 0, 오른쪽 : 1000000) + 시작NodeID
	BYTE IsCheck;                     ///< 0x00 : 주행검증 미진행, 0x01 : 주행검증 진행 : 0x01
} MAMMAKEPROCESS_LINK_INFO;
//stuct size : 34Byte

#pragma pack(pop) // Pack 해제

// -------------------------------------------
typedef std::map<UINT, MAMMAKEPROCESS_LINK_INFO *> MAMMAKEPROCESS_MAP;
typedef MAMMAKEPROCESS_MAP::iterator MAMMAKEPROCESS_MAP_ITER;
typedef std::pair<UINT, MAMMAKEPROCESS_LINK_INFO *> MAMMAKEPROCESS_MAP_PAIR;
typedef std::pair<MAMMAKEPROCESS_MAP_ITER, bool> MAMMAKEPROCESS_DATA_MAP_RESULT;
/**
@class   STD_MapMakerFile
@author  puting
@date    2016.12.25
@brief   OHT에 사용되는 Standard Map에 관련된 Data, 함수를 정의한 class
*/
//---------------------------------------------------------------------------

class MapMakeProcess
{
private:

//	bool m_bDataModified;   ///< Data 수정 flag
//	LogUnit *pLogMapMakefile; ///< Map Make STD관련 로그 변수
	MAMMAKEPROCESS_MAP m_MapMakeProcessData;    ///< Station Data 자료구조(Map)
   	void Init();

	MAMMAKEPROCESS_LINK_INFO* Find(UINT LinkNID);
	int m_MapMakeDrivingCount;

public:

	//HEADER_INFO_STD_MAP
	
	MapMakeProcess();
	~MapMakeProcess();


	// Map 자료구조 관련 함수
	int GetCount();
	int GetCountMapMakeDriving();

	// Map 자료구조 추가, 갱신/ 추가 모두 포함.
	int Update(MAMMAKEPROCESS_LINK_INFO *updateSTD);
	int Insert(MAMMAKEPROCESS_LINK_INFO *addStationData);

	//추가.
//	int GetCountMapMakeDrivingReflash();   //Map Maker file Loading 유무                  //미사용
	void InitMapMakeProcessMemory(); //Loading된 Map maker File 초기화. 파일 전송완료할때

};
#endif //STD_StationH
