//---------------------------------------------------------------------------
/**
@file    STD_MAP.h
@version OHT 7.5
@brief   STANDARD MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef STD_MAPH
#define STD_MAPH

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
@brief   MAPDATA.DAT의 data 관련 구조체
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_Map
{
	UINT NodeID;                        ///< 주행 Node Id
	BYTE NodeType;                      ///< Node의 종류
										//   0x1: Lift In, 0x2: Lift out 0x3: MTL In, 0x4 : MTL Middle, 0x5:MTL out
	UINT LeftNodeID;                    ///< (왼쪽) 연결된 Node Id
	UINT LeftNodeDistance;              ///< (왼쪽) 연결된 Node까지의 거리(mm)
	WORD LeftNodeSpeed;                 ///< (왼쪽) 연결된 Node까지의 이동 속도(mm/s)
	BYTE LeftSteering;                  ///< (왼쪽) 연결된 Point까지의 Steer 방향
	BYTE LeftBranch;                    ///< (왼쪽) 연결된 Point까지의 분기정보(일반분기,N분기)
	UINT RightNodeID;                   ///< (오른쪽) 연결된 Node Id
	UINT RightNodeDistance;             ///< (오른쪽) 연결된 Node까지의 거리(mm)
	WORD RightNodeSpeed;                ///< (오른쪽) 연결된 Node까지의 이동 속도(mm/s)
	BYTE RightSteering;                 ///< (오른쪽) 연결된 Point까지의 Steer 방향
	BYTE RightBranch;                   ///< (오른쪽) 연결된 Point까지의 분기정보(일반분기,N분기)
//	UINT ResetNodeMain;                 ///< 통과_허가_Reset Node ID(Main)
//	UINT ResetNodeSub;                  ///< 통과_허가_Reset Node ID(Sub.)
//	UINT ResetNodeRedundance1;          ///< 통과_허가_Reset Node ID(Redundance 1)
//	UINT ResetNodeRedundance2;          ///< 통과_허가_Reset Node ID(Redundance 2)
#if(USE_CHANGENODE == 1)
	// for change node (jhoun.lim)
	UINT LeftNodeOptRatio;				///< distance ratio to left opt node (to node : 33.33%, after node : 66.67% -> 0x33336667)
	UINT RightNodeOptRatio;				///< distance ratio to right opt node
	BYTE ChargeEnable;		

	BYTE Reserved_1;                    ///< Reserved_1
	BYTE Reserved_2;                    ///< Reserved_2
	// BYTE Reserved_3;                    ///< Reserved_3
#else
	UINT Reserved_1;                    ///< Reserved_1
	UINT Reserved_2;                    ///< Reserved_2
	BYTE ChargeEnable;					///< (???) ECO?? ????? ???? ???

	BYTE Reserved_3;                    ///< Reserved_3
	BYTE Reserved_4;                    ///< Reserved_4
	// BYTE Reserved_5;                    ///< Reserved_5
#endif
	DEFAULT_MAPINFO IsDefaultMap;       ///< IsDefaultMap
} NODE_INFO_STD_MAP;
//stuct size : 57Byte

#pragma pack(pop) // Pack 해제

// -------------------------------------------

/**
@class   STD_MAP
@author  KJD
@date    2016.03.30
@brief   OHT에 사용되는 Standard Map에 관련된 Data, 함수를 정의한 class
*/

class STD_MAP
{
private:


public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_MAP *m_NodeArray; ///< Node 동적 배열 포인터

	STD_MAP();
	~STD_MAP();

	void InitMap(int NodeNum);

};
//---------------------------------------------------------------------------


/**
@class   MapDataManager
@author  KJD
@date    2016.03.30
@brief   Map Data를 관리하는 Class
*/
class MapDataManager_STD
{
private:
//	LogUnit *pLogCRC;      ///< CRC 로그 생성을 위한 변수
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_MapFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	MapDataManager_STD();
	~MapDataManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_MAP *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_MAP *pMapdata);

};
//---------------------------------------------------------------------------

#endif //STD_MAPH

