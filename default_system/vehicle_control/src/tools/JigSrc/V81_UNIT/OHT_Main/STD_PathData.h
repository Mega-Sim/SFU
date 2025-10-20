//---------------------------------------------------------------------------
/**
@file    STD_PATHDATA.h
@version OHT 7.5
@brief   STANDARD MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef STD_PATHDATAH
#define STD_PATHDATAH

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
@brief   PATHDATA.DAT의 data 관련 구조체
@author  KJD
@date    2016.05.08
*/
typedef struct Node_Info_STD_PathData
{
    BYTE LayoutType;                    ///< Layout Type(1: MTL, 2: Lifter)
    UINT LayoutID;                      ///< Layout ID
    UINT LeftNodeIDIn;                  ///< MTL(왼쪽), Lifter(아래) 진입 Node Id
    UINT LeftNodeIDMiddle;              ///< MTL(왼쪽), Lifter(아래) 중간 Node Id
    UINT LeftNodeIDOut;                 ///< MTL(왼쪽) 진출 Node Id
    BYTE LeftDriveDirection;            ///< 차량 진행 방향을 표시(0: 전진, 1: 후진)
    UINT RightNodeIDIn;                 ///< MTL(오른쪽), Lifter(위) 진입 Node Id
    UINT RightNodeIDMiddle;             ///< MTL(오른쪽), Lifter(위) 중간 Node Id
    UINT RightNodeIDOut;                ///< MTL(오른쪽) 진출 Node Id
    BYTE RightDriveDirection;           ///< 차량 진행 방향을 표시(0: 전진, 1: 후진)
    UINT Reserved1;                     ///< Reserved 1
    UINT Reserved2;                     ///< Reserved 2
} NODE_INFO_STD_PATHDATA;
//stuct size : 39Byte
#pragma pack(pop) // Pack 해제

// -------------------------------------------

/**
@class   STD_PATHDATA
@author  KJD
@date    2016.05.08
@brief   OHT에 사용되는 Standard Map에 관련된 Data, 함수를 정의한 class
*/
class STD_PathData
{
private:


public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_PATHDATA *m_NodeArray; ///< Node 동적 배열 포인터


	std::map<int, int> m_PathDataMap;

	STD_PathData();
	~STD_PathData();

	void InitMap(int NodeNum);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
@class   MapDataManager
@author  KJD
@date    2016.05.08
@brief   Map Data를 관리하는 Class
*/
class PathDataManager_STD
{
private:
//	LogUnit *pLogCRC;      ///< CRC 로그 생성을 위한 변수
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_PathDataFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	PathDataManager_STD();
	~PathDataManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_PathData *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_PathData *pMapdata);

	bool find(STD_PathData *pMapdata, int NodeID, NODE_INFO_STD_PATHDATA* pPathData);

	int GetPathData(STD_PathData *pMapdata, NODE_INFO_STD_PATHDATA *pPathdata,UINT NodeID);


};
#endif //STD_PATHDATAH

