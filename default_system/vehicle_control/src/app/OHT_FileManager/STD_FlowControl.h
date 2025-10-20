//---------------------------------------------------------------------------
/**
@file    STD_FlowControl.h
@version OHT 7.5
@brief   STANDARD MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef STD_FlowControlH
#define STD_FlowControlH

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
@brief   FLOWCONTROL.DAT의 data 관련 구조체
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_FlowControl
{
	UINT StopNodeID;                    ///< Stop Node Id
	UINT ResetNodeMain;                 ///< Reset Node ID(Main) - 합류인 경우 Main에만 입력"
	BYTE BZStopMain;                    ///< BZ Stop 설정
	UINT ResetNodeMainDefaultValue;     ///< Reset Node ID(Main)에 대한 통과 Default 값
	UINT ResetNodeMainDefaultRatio;     ///< Reset Node ID(Main)에 대한 통과 Default 값 * (0 ~ 200%)
	UINT ResetNodeSub;                  ///< Reset Node ID(Sub.) - 분기인 경우 Main과 Sub.에 모두 입력"
	BYTE BZStopSub;                     ///< BZ Stop 설정
	UINT ResetNodeSubDefaultValue;      ///< Reset Node ID(Sub.)에 대한 통과 Default 값
	UINT ResetNodeSubDefaultRatio;      ///< Reset Node ID(Sub.)에 대한 통과 Default 값 * (0 ~ 200%)
	UINT ResetNodeRedundance1;          ///< Reset Node ID(Redundance 1)
	BYTE BZStopRedundance1;             ///< BZ Stop 설정
	UINT ResetNodeRedundance1DefaultValue;        ///< Reset Node ID(Redundance 1)에 대한 통과 Default 값
	UINT ResetNodeRedundance1DefaultRatio;        ///< Reset Node ID(Redundance 1)에 대한 통과 Default 값 * (0 ~ 200%)
	UINT ResetNodeRedundance2;          ///< Reset Node ID(Redundance 2)
	BYTE BZStopRedundance2;             ///< BZ Stop 설정
	UINT ResetNodeRedundance2DefaultValue;        ///< Reset Node ID(Redundance 2)에 대한 통과 Default 값
	UINT ResetNodeRedundance2DefaultRatio;        ///< Reset Node ID(Redundance 2)에 대한 통과 Default 값 * (0 ~ 200%)
	UINT Reserved1;                     ///< Reserved_1
	UINT Reserved2;                     ///< Reserved_2
	UINT Reserved3;                     ///< Reserved_3
	UINT Reserved4;                     ///< Reserved_4
} NODE_INFO_STD_FLOWCONTROL;
//stuct size : 72Byte

#pragma pack(pop) // Pack 해제

// -------------------------------------------

/**
@class   STD_FlowControl
@author  KJD
@date    2016.03.30
@brief   OHT에 사용되는 Standard Map에 관련된 Data, 함수를 정의한 class
*/
class STD_FlowControl
{
private:


public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_FLOWCONTROL *m_NodeArray; ///< Node 동적 배열 포인터


	std::map<int, int> m_FlowControlMap;
	void MakeFlowControlMap_STD();

	STD_FlowControl();
	~STD_FlowControl();

	void InitMap(int NodeNum);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
@class   MapDataManager
@author  KJD
@date    2016.03.30
@brief   FlowControl Data를 관리하는 Class
*/
class FlowControlManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_FlowControlFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	FlowControlManager_STD();
	~FlowControlManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_FlowControl *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_FlowControl *pMapdata);

};
#endif //STD_FlowControlH
