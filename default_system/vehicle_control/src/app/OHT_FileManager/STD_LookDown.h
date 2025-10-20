//---------------------------------------------------------------------------
/**
@file    STD_LookDown.h
@version OHT 7.5
@brief   STANDARD MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef STD_LookDownH
#define STD_LookDownH

#include <map>
#include "Def_DataType.h"
#include "Def_OHT.h"
//#include "Def_PathInfo.h"
//#include "Def_DataFile.h"
#include "Def_DataFile_STD.h"


//typedef파일명 Def_Variables.h
using namespace std;

// -------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack



/**
@brief   LOOKDOWN.DAT의 data 관련 구조체
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_LookDown
{
    UINT StationID;                     ///< Station Id
	BYTE PortType;                      ///< 이적재 Port 정보 ex) EQ, STB-L, STB-R, UTB"
	BYTE LookDown;                      ///< Station 이적재 시 Lookdown Area 정보
	UINT Reserved1;                     ///< Reserved_1
} NODE_INFO_STD_LOOKDOWN;
//stuct size : 10Byte

#pragma pack(pop) // Pack 해제

// -------------------------------------------

/**
@class   STD_LookDown
@author  KJD
@date    2016.03.30
@brief   OHT에 사용되는 Standard Map에 관련된 Data, 함수를 정의한 class
*/
class STD_LookDown
{
private:


public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_LOOKDOWN *m_NodeArray; ///< Node 동적 배열 포인터

	std::map<int, int> m_LookDownMap;

	STD_LookDown();
	~STD_LookDown();

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
class LookDownManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_LookDownFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	LookDownManager_STD();
	~LookDownManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_LookDown *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_LookDown *pMapdata);

};
#endif //STD_LookDownH
