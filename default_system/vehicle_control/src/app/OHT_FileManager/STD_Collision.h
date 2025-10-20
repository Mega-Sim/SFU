//---------------------------------------------------------------------------
/**
@file    STD_Collision.h
@version OHT 7.5
@brief   STANDARD MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef STD_CollisionH
#define STD_CollisionH

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
@brief   COLLISION.DAT의 data 관련 구조체
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_Collision
{
	UINT NodeID;                        ///< 주행 Node Id
	UINT LeftNodeID;                    ///< (왼쪽) 연결된 Node Id
	BYTE LeftUBGRegion;                 ///< (왼쪽) 연결된 Point까지의 대차감지센서 영역
	BYTE LeftObstacleRegion;            ///< (왼쪽) 연결된 Point까지의 장애물감지센서 영역
	BYTE LeftObstacleStatus;            ///< (왼쪽) 장애물감지센서 사용 유무 (0: 미사용, 1: 사용)
	UINT RightNodeID;                   ///< (오른쪽) 연결된 Node Id
	BYTE RightUBGRegion;                ///< (오른쪽) 연결된 Point까지의 대차감지센서 영역
	BYTE RightObstacleRegion;           ///< (오른쪽) 연결된 Point까지의 장애물감지센서 영역
	BYTE RightObstacleStatus;           ///< (오른쪽) 장애물감지센서 사용 유무 (0: 미사용, 1: 사용)
} NODE_INFO_STD_COLLISION;

#pragma pack(pop) // Pack 해제

// -------------------------------------------

/**
@class   STD_Collision
@author  KJD
@date    2016.03.30
@brief   OHT에 사용되는 Standard Map에 관련된 Data, 함수를 정의한 class
*/
class STD_Collision
{
private:


public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_COLLISION *m_NodeArray; ///< Node 동적 배열 포인터

	std::map<int, int> m_CollisionMap;
	void MakeCollisionMap_STD();

	STD_Collision();
	~STD_Collision();

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
class CollisionManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_CollisionFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	CollisionManager_STD();
	~CollisionManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_Collision *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_Collision *pMapdata);


};
#endif //STD_CollisionH


