//---------------------------------------------------------------------------
/**
@file    MAP.h
@version OHT 7.0
@brief   MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef MAPH
#define MAPH

#include <map>
#include "../Common/Def_DataType.h"
#include "Def_OHT.h"
#include "Def_PathInfo.h"
#include "Def_DataFile.h"
#include "STD_MAP.h"
#include "STD_Collision.h"
#include "STD_FlowControl.h"
#include "STD_Teaching.h"
#include "STD_CID.h"
#include "MapMakeProcess.h"

#define INDEX_NONE  	 0xFFFFFFFF
#define PATHINDEX_OFFSET (1)


#define NAME_NODE_DISABLE 			"NodeDisable.nod"
#define NAME_PASS_BOX_NODE			"PassBoxNode.nod"

using namespace std;

// -------------------------------------------
// [BEGIN] by zzang9un 013.07.30 : 1Byte씩 Packing
#pragma pack(push, 1) // 1byte로 Pack

/**
@brief   Path 관련 구조체
@author  zzang9un
@date    2012.09.04
*/
typedef struct Path_Info_
{    
    UINT ID;                ///< Path ID
    UINT StartNID;          ///< Path 시작 Node ID
    UINT EndNID;            ///< Path 종료 Node ID
    UINT Speed;             ///< Path Max Speed(mm/s)
    BYTE Steering;          ///< Steering 방향 (N:0/L:1/R:2) -> 3, 4가 맵에서 제거되므로 PathBranch로 처리 추가 필요
	UINT Distance;          ///< 실제 Path 거리(mm)
#if(USE_CHANGENODE == 1)
    //BYTE PathDirection;     ///< Path 곡선의 방향 표시 (NoUse:0 / N:1 / L:2 / R:3 / S:4 / B:5 / V:6)   // 신규 추가 UBGRegion 값
    BYTE UBGRegion;
	BYTE ObstacleRegion;
	BYTE PathBranch;        ///< 일반 분기 또는 N분기 판단 (NoUse:0, 1: 일반분기, 2: N분기)
	BYTE RailBranch;         ///< 레일 기준 N, R, L data
    UINT OutSteeringOffset; ///< 선조향 명령 가능한 Offset
    UINT InSteeringOffset;  ///< 아직 미정
    BYTE ObstacleIgnore;     ///< OBS 미사용 구간  (사용:true/미사용:false)  -> Node 정보에서 변환해서 Path정보로 변경

	// for change node (jhoun.lim)
	UINT OptDistance;
	UINT OptDistanceAfter;
	BYTE OptUBGRegion;
	BYTE OptObstacleRegion;


	//---------------------------------------------------------------------------
	// for change node (jhoun.lim)
	static UINT RatioEncoding(const double ratio)
	{
		return (((UINT(ratio*0.1) << 4) | (UINT(ratio) % 10)) << 4 | UINT(ratio * 10) % 10) << 4 | UINT(ratio * 100) % 10;
	}
	static double RatioDecoding(const UINT ratio)
	{
		UINT a = ((ratio & 0xF000) >> 12);
		if(a>9) return -1.0;

		UINT b = ((ratio & 0x0F00) >> 8);
		if(b>9) return -1.0;

		UINT c = ((ratio & 0x00F0) >> 4);
		if(c>9) return -1.0;

		UINT d = (ratio & 0x000F);
		if(d>9)    return -1.0;

		UINT partInt = a * 10 + b;
		UINT partFloating = c * 10 + d;
		return partInt + partFloating*0.01;
	}

	static UINT ConvertRatio2Distance(const UINT length, const UINT ratio, const bool isFirst = true)
	{
		return length * RatioDecoding(isFirst ? (ratio & 0xFFFF0000) >> 16 : (ratio & 0x0000FFFF)) * 0.01;
	}
	void calcOptDistance(const UINT ratio) { this->OptDistance = ConvertRatio2Distance(this->Distance, ratio, true); }
	void calcOptDistanceAfter(const UINT ratio) { this->OptDistanceAfter = ConvertRatio2Distance(this->Distance, ratio, false); }
#else
	BYTE PathDirection;     ///< Path 곡선의 방향 표시 (NoUse:0 / N:1 / L:2 / R:3 / S:4 / B:5 / V:6)   // 신규 추가 UBGRegion 값
	BYTE PathBranch;        ///< 일반 분기 또는 N분기 판단 (NoUse:0, 1: 일반분기, 2: N분기)
	BYTE RailBranch;         ///< 레일 기준 N, R, L data
	UINT OutSteeringOffset; ///< 선조향 명령 가능한 Offset
	UINT InSteeringOffset;  ///< 아직 미정
	BYTE ObstacleIgnore;     ///< OBS 미사용 구간  (사용:true/미사용:false)  -> Node 정보에서 변환해서 Path정보로 변경
#endif
} PATH_INFO;

/**
@brief   Node 관련 구조체
@author  zzang9un
@date    2012.09.04
*/
typedef struct Node_Info_
{
    UINT NID;               ///< Node ID
//    UINT Point_X;           ///< X좌표
//	UINT Point_Y;           ///< Y좌표
	UINT TagType;           ///< Node Type N S R C    puting
	// TagType 생성 방법 확인 필요
	
//	UINT CID;               ///< Start/ 내부/ End / 공용 --> CID.DAT 구조체 사용 
//	UINT FrontOBS2;         ///< 전방 장애물 무시 Node 2    -> 변환해서 Path정보로 변경
//	UINT LowAccellation;    ///< 감속도 변경
//	UINT CollisionInfo;     ///< 근접 제어시 필요한 정보 (제거 예정)
	//CollisionInfo를 구조체로 변경 요구 by puting
	// CollisionInfo 구조체를 별도 생성하여 호출하는 것이
	// NODE_INFO를 통해 호출하는 것보다 좋을듯...파.
	// CollisionInfo NODE_INFO의 객체 정보가 같으므로 NODE_INFO에 CollisionInfo의 내용을 넣는게 제일 좋음.
	// 추후 맵데이터 변경시 동일위치에 추가여부를 
	UINT InOutInfo;         ///< 통과허가 Stop/Reset/공용                     		puting
							// 1: Lifter In
							// 2: Lifter Out
							// 3: MTL In
							// 4: MTL Middle
							// 5: MTL Out
	UINT PrePathIndex[2];   ///< 해당 Node가 EndNID인 Path 구조체 배열의 Index : 거리 가중치 작은 것이 PrePathIndex[0]  - PathSearch 시 사용됨
	UINT NextPathIndex[2];  ///< 해당 Node가 StartNID인 Path 구조체 배열의 Index : 거리 가중치 작은 것이 NextPathIndex[0]   - PathSearch 시 사용됨
													            //--------> 분기 정보 [0] 왼쪽 분기 Node , [1] 오른쪽 분기 Node        puting
} NODE_INFO;

#pragma pack(pop) // Pack 해제
// [END] by zzang9un 013.07.30 : 1Byte씩 Packing
// -------------------------------------------

/**
@class   MAP
@date    2012.11.09
@author  zzang9un
@brief   OHT에 사용되는 Map에 관련된 Data, 함수를 정의한 class
*/
class MAP
{
private:
    void    MakeNodeMap();
	void	MakeRightNodeMapFromPathArray();
	int 	MakePathIndex(MAP *pMapdata, NODE_INFO_STD_MAP * m_NodeArray_Map, UINT NodeNum_Map);
    void    ComparePathDistance(UINT *PathIndex);

public:
    int m_PathNum; ///< Path 개수
	int m_NodeNum; ///< Node 개수
	unsigned char m_FlowcontrolCheckResult;

    PATH_INFO *m_PathArray; ///< Path 동적 배열 포인터
    NODE_INFO *m_NodeArray; ///< Path 동적 배열 포인터

	std::map<int, int> m_MakeLeftNodeMap;
    std::map<int, int> m_MakeRightNodeMap;
    std::map<int, int> m_NodeMap;
	std::map<int, int> m_TeachingMap;
	std::map<int, int> m_LookDownMap;
	std::map<int, int> m_CIDResetMap;
	std::map<int, int> m_CIDStopMap_1;
	std::map<int, int> m_CIDStopMap_2;
	std::map<int, int> m_CIDStartMap_1;
	std::map<int, int> m_CIDStartMap_2;

	std::map<int, int> m_FlowControl;
	std::map<int, int> m_NextNode;
	std::map<unsigned long long, int> m_FlowControl_StopReset;	
	std::map<unsigned long long, int> m_RailMerge_StopReset;

    MAP();
    ~MAP();

    void InitMap(int PathNum, int NodeNum);
	int MakeMap(MAP *pMapdata, NODE_INFO_STD_MAP * m_NodeArray_Map, UINT NodeNum_Map);
	void MakeTeachingMap(NODE_INFO_STD_TEACHING *m_NodeArray_Teach, UINT DataNum);
	void MakeCIDMap(NODE_INFO_STD_CID * m_NodeArray_CID, UINT DataNum);
	void MakeFlowControlMap(NODE_INFO_STD_FLOWCONTROL * m_FlowArray, UINT DataNum);

    int FindNodeIndex(int NodeID);
	int FindNextIndex(int NodeID);
	int FindRightNodeIndex(int NodeID);
	int FindStationIndexFromTeachingMap(int NodeID);
	int FindStationIndexFromLookDownMap(int NodeID);
	int FindNodeIndexFromCIDReset(int NodeID);
	int FindNodeIndexFromCIDStop_1(int NodeID);
	int FindNodeIndexFromCIDStop_2(int NodeID);
	int FindNodeIndexFromCIDStart_1(int NodeID);
	int FindNodeIndexFromCIDStart_2(int NodeID);

	int FindNodeIndexFromFlowControl(int NodeID,int NextNodeID,STD_FlowControl *pFlowMap);
	int FindIndex_FlowControl_StopReset(unsigned long long NodeID);
	int FindIndex_RailMerge_StopReset(unsigned long long NodeID);


    // by zzang9un 2013.07.23 : PID를 찾는 함수 추가(for Map Make)
    int GetPathIndex(int StartNID, int EndNID);

	int ReadNodeDisableFile();
	int ReadPassBoxNodeFile();

	int CheckFlowControlData(unsigned long long StopResetNode);

	TStringList* tNodeDisalbeList;
	TStringList* tPassBoxNodeList;
};
//---------------------------------------------------------------------------

// -------------------------------------------
// Map 관련 에러 코드
// -------------------------------------------
//#define ERR_MAP_FILESAVEFAIL        -2000

/**
@class   MapDataManager
@date    2013.07.30
@author  zzang9un
@brief   Map Data를 관리하는 Class
*/
class MapDataManager
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag
    
    MAP_FILE_HEADER m_MapFileHeader; ///< Map File Header

public:
    MapDataManager();
    ~MapDataManager();

	int LoadMapFile(TCHAR *FileName, MAP *pMapdata);
	int SaveMapFile(TCHAR *FileName, MAP *pMapdata);
	int LoadDataFromSTDMap(MAP *pMapdata, 
                           NODE_INFO_STD_MAP * m_NodeArray_Map, 
                           NODE_INFO_STD_COLLISION * m_NodeArray_Collision,
                           NODE_INFO_STD_FLOWCONTROL * m_NodeArray_Flow,
						   NODE_INFO_STD_CID * m_NodeArray_CID,
						   MapMakeProcess* pMapMakeProcess,
                           UINT NodeNum_Map, UINT DataNum_Flow, UINT DataNum_CID, bool bAdjust = false);

	bool GetMapLoaded();
	unsigned int GetMapVersion();

};
//---------------------------------------------------------------------------

// by zzang9un 2012.11.30 : Map 정보의 String을 Int로 변경하는 함수 생성
// by zzang9un 2012.11.30 : 임태웅 선임 요청에 의해 생성함

/**
@brief   Steering 정보 문자를 내부 enum 변수로 convert하는 함수
@author  zzang9un
@date    2012.11.30
@param   strSteering : Port ID
@return  BYTE(PATH_STEERING_INFO) 변수를 리턴
*/
BYTE SteeringToInt(TCHAR cSteering)
{
    BYTE ReturnVal = PATH_STEERING_NONE;
    
    switch(cSteering)
    {
    case 'N':
        ReturnVal = PATH_STEERING_NONE;
        break;

    case 'L':
        ReturnVal = PATH_STEERING_LEFT;
        break;

    case 'R':
        ReturnVal = PATH_STEERING_RIGHT;
        break;

    case 'l':    
        ReturnVal = PATH_STEERING_N_BRANCH_L2R;
        break;

    case 'r':    
        ReturnVal = PATH_STEERING_N_BRANCH_R2L;
        break;    

    default:
        break;        
    }

    return ReturnVal;
}
//---------------------------------------------------------------------------

/**
@brief   Steering 정보 문자를 내부 enum 변수로 convert하는 함수
@author  zzang9un
@date    2012.11.30
@param   strSteering : Port ID
@return  PATH_STEERING_INFO 변수를 리턴
*/
BYTE PathDirToInt(TCHAR cPathDir)
{
    BYTE ReturnVal = PATH_DIRECTION_NO_USE;
    
    switch(cPathDir)
    {
    case 'N':
        ReturnVal = PATH_DIRECTION_NORMAL;
        break;

    case 'L':
        ReturnVal = PATH_DIRECTION_LEFT;
        break;

    case 'R':
        ReturnVal = PATH_DIRECTION_RIGHT;
        break;

    case 'S':
        ReturnVal = PATH_DIRECTION_SHORT;
        break;

    case 'B':
        ReturnVal = PATH_DIRECTION_BOTH;
        break;    

    case 'V':
        ReturnVal = PATH_DIRECTION_NARROW;
		break;

	case 'H':
		ReturnVal = PATH_DIRECTION_HILL;
        break;
        
    default:
        break;        
    }

    return ReturnVal;
}
//---------------------------------------------------------------------------
#endif //MAPH
