//---------------------------------------------------------------------------
/**
@file    STD_CID.h
@version OHT 7.5
@brief   STANDARD MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef STD_CIDH
#define STD_CIDH

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
@brief   CID.DAT의 data 관련 구조체
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_CID
{
    BYTE LayoutType;                    ///< 합류지점, 경사로, MTL, Lift, 중계(Interface), Pass Door, Fire Door
    //UINT LayoutID;                    // UI 표시용 ID
    BYTE LayoutID[6];                   ///< UI 표시용 ID : CID001, CID002...
    UINT ResetNode;                     ///< Layout 내 Reset Node
    UINT StopNode1;                     ///< Layout 내 Stop Node 1
    UINT StopNode2;                     ///< Layout 내 Stop Node 2
    BYTE ResetNodeType;                 ///< Layout 내 Reset Node Type
    BYTE CIDDirection1;                 ///< Stop Node 1 → Reset Node에서의 CID-O 방향
    UINT CommStartNode1_1;              ///< Stop Node 1 → Reset Node에서의 통신 시작(CID-O On) Node 1
    UINT CommStartNode1_2;              ///< Stop Node 1 → Reset Node에서의 통신 시작(CID-O On) Node 2
    UINT CommStartNode1_3;              ///< Stop Node 1 → Reset Node에서의 통신 시작(CID-O On) Node 3
    UINT CommStartNode1_4;              ///< Stop Node 1 → Reset Node에서의 통신 시작(CID-O On) Node 4
    UINT CommStartNode1_5;              ///< Stop Node 1 → Reset Node에서의 통신 시작(CID-O On) Node 5
    INT  CommStartNode1_Offset;         ///< CommStartNode1_1 에서 CID동작 시작지점 까지의 거리(mm, 기본값0, 맵작성시 설계값 기입)
    BYTE CIDDirection2;                 ///< Stop Node 2 → Reset Node에서의 CID-O 방향
    UINT CommStartNode2_1;              ///< Stop Node 2 → Reset Node에서의 통신 시작(CID-O On) Node 1
    UINT CommStartNode2_2;              ///< Stop Node 2 → Reset Node에서의 통신 시작(CID-O On) Node 2
    UINT CommStartNode2_3;              ///< Stop Node 2 → Reset Node에서의 통신 시작(CID-O On) Node 3
    UINT CommStartNode2_4;              ///< Stop Node 2 → Reset Node에서의 통신 시작(CID-O On) Node 4
    UINT CommStartNode2_5;              ///< Stop Node 2 → Reset Node에서의 통신 시작(CID-O On) Node 5
    INT  CommStartNode2_Offset;         ///< CommStartNode2_1 에서 CID동작 시작지점 까지의 거리(mm, 기본값0, 맵작성시 설계값 기입)
    UINT Reserved1;                     ///< Reserved_1
    UINT Reserved2;                     ///< Reserved_2
} NODE_INFO_STD_CID;
//stuct size : 79Byte
#pragma pack(pop) // Pack 해제

// -------------------------------------------

/**
@class   STD_CID
@author  KJD
@date    2016.03.30
@brief   OHT에 사용되는 Standard Map에 관련된 Data, 함수를 정의한 class
*/
class STD_CID
{

public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_CID *m_NodeArray; ///< Node 동적 배열 포인터

	std::map<int, int> m_CIDMap;

	STD_CID();
	~STD_CID();

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
class CIDManager_STD
{
private:
//	LogUnit *pLogCRC;      ///< CRC 로그 생성을 위한 변수
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_CIDFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	CIDManager_STD();
	~CIDManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_CID *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_CID *pMapdata);

};
#endif //STD_CIDH

