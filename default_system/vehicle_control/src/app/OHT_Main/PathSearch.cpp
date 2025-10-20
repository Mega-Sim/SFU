//---------------------------------------------------------------------------
#include <map>
#include <iostream>
#include "PathSearch.h"
#include "ErrorCode.h"
#include "MAP.h"
#include "OHTMain.h"
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#define ADD_LOG(...)    WRITE_LOG_UNIT(pLogPathSearch, __VA_ARGS__)
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)
#define ADD_MD_LOG(...)    WRITE_LOG_UNIT(pLogMDetail, __VA_ARGS__)  //AOHC-253 Monitoring 항목 추가 2

using namespace std;
//---------------------------------------------------------------------------
/**
@brief   Main Thread와 Execute Thread 사이에 전달될 Driving용 Define
@author  ehoto
@date    2012.11.12
*/
int VisitLinkCheckArray[1000000][2];
int VisitNodeCheckArray[1000000];

typedef struct PATH_SEARCH_NODE_INFO_
{
	UINT IndexNum;				 ///< Path Index
	UINT SQID;					///< 시퀀스 ID : Passpermit 용도.
	UINT NID;              	 ///< Node ID
	UINT NextNID;           ///< Next Node ID		: 다음 노드 정보 찾을 때 사용
	UINT SubNID;			///나머지 방향의 Node ID : 통과허가를 위한 부분
	UINT PathIndex;		///< Next Path ID Index	: 다음 Path 정보 찾을 때 사용
	PATH_STEERING_INFO		PreSteering;         ///<선조향 정보를 위한 부분.
	PATH_DIVERGENCE_INFO    DivInfo;			///< 분기 정보

	double dTotalDistance;      //처음 시작부터 현재 Path까지 합한 거리
	double dPathDistance;       //현재 패스의 거리

} PATH_SEARCH_NODE_INFO;


PathSearch::PathSearch()
{
	pLogPathSearch = getLogUnit("D:\\log\\PathSearch.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogMDetail = getLogUnit("D:\\log\\MonitoringDetail.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	InitializeCriticalSection(&m_CS);
	m_PathList = OHTMainForm->GetPathList();

	m_pParamSet = new PATHSEARCH_PARAM_SET();
	m_pUBGParamSet = new PATHSEARCH_UBG_CHECK_PARAM_SET();
	m_pNodeLanCutSet = new PATHSEARCH_NODE_LANCUT_PARAM_SET();

	loadParameterRecovery("DataFiles\\PathSearch.ini", m_pParamSet);
	loadUBGCheckParam("DataFiles\\UBGCheckNodes.dat", m_pUBGParamSet);

	m_SquenceIndex = 1;
	m_pDrivingControl = NULL;
	m_pTmpParamSet = NULL;

	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
	m_defualtparam = OHTMainForm->GetDefaultParam();
	m_EndNodeCnt = 0;

	ZeroMemory(VisitLinkCheckArray, sizeof(VisitLinkCheckArray));
	m_bFindTheWay = false;
	m_ParamChehckError = NO_ERR;
	bCheckPathParam=false;
	bUseAVS = m_defualtparam->bUseAVS;
	dwPathSearchTime = timeGetTime();

}
PathSearch::~PathSearch()
{

	InitPathList();
//	delete m_PathList;
	DeleteCriticalSection(&m_CS);

//	saveParameter("DataFiles\\PathSearch.ini", m_pParamSet);


	delete m_pParamSet;
	delete m_pUBGParamSet;
	delete m_pNodeLanCutSet;
}
/**
@brief   List를 초기화하는 함수
@author  puting
@date    2015.11.16
@note    List는 CMD_DATA 구조체의 포인터로 구성되어 있음.
*/
void PathSearch::InitPathList()
{
	// List를 꺼내 해당 포인터 delete 후
	int nCount = m_PathList->getCount();
	for(int i = 0; i < nCount; i++)
	{
		EnterCriticalSection(&m_CS);

        try
        {
			delete (EXECUTE_DRIVING_INFO *)m_PathList->popFront();
        }
        __finally
		{
			LeaveCriticalSection(&m_CS);
        }
    }

    // 리스트를 Clear한다.
	EnterCriticalSection(&m_CS);
    try
    {
		m_PathList->clear();
    }
    __finally
    {
		LeaveCriticalSection(&m_CS);
    }

}
//---------------------------------------------------------------------------

/**
@brief   List에 Command를 추가하는 함수
@author  puting
@date    2015.11.16
@param   pData : Add하려는 void 포인터
*/
void PathSearch::AddPath(void *pData)
{
	EnterCriticalSection(&m_CS);
	try
	{
		m_PathList->insertBack(pData);
	}
	__finally
	{
		LeaveCriticalSection(&m_CS);
	}
}

/**
@brief   List의 첫번째 Item을 참조하는 포인트를 리턴하는 함수
@author  puting
@date    2015.11.16
@return  List의 첫번째 Item의 포인터를 리턴한다.

void* PathSearch::ReferFirstPath()
{
	void *pListItem = NULL;

	pListItem = m_PathList->referFront();

	return pListItem;
}
*/


/**
@brief   List의 Index Item을 참조하는 포인트를 리턴하는 함수
@author  puting
@date    2013.10.28
@return  List의 index Item의 포인터를 리턴한다.
*/
void* PathSearch::ReferPath(UINT index)
{
    void *pListItem = NULL;

	pListItem = m_PathList->refer(index);

    return pListItem;
}


/**
@brief   List의 마지막번째 Item을 참조하는 포인트를 리턴하는 함수
@author  puting
@date    2016.01.19
@return  List의 첫번째 Item의 포인터를 리턴한다.
*/
void* PathSearch::ReferBackPath()
{
	void *pListItem = NULL;

	pListItem = m_PathList->referBack();

	return pListItem;
}

//---------------------------------------------------------------------------

/**
@brief   List의 첫번째 Item을 Pop하는 함수
@author  puting
@date    2015.11.16
@return  List의 첫번째 Item의 포인터를 리턴한다.
@note    Pop한 Item은 리스트에서 삭제된다.
*/
void* PathSearch::PopBackPath()
{
    void *pListItem = NULL;

	CS_TaskList->Acquire();
    try
    {
		pListItem = m_PathList->popBack();
    }
    __finally
    {
		CS_TaskList->Release();
    }

    return pListItem;
}
/**
@brief   List의 첫번째 Item을 Pop하는 함수
@author  puting
@date    2015.11.16
@return  List의 첫번째 Item의 포인터를 리턴한다.
@note    Pop한 Item은 리스트에서 삭제된다.
*/
void* PathSearch::PopFirstPath()
{
    void *pListItem = NULL;

	CS_TaskList->Acquire();
    try
    {
		pListItem = m_PathList->popFront();
    }
    __finally
    {
		CS_TaskList->Release();
    }

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List의 count를 리턴하는 함수
@author  puting
@date    2015.11.16
@return  List의 count를 리턴한다.
*/
int PathSearch::GetPathCount()
{
	return m_PathList->getCount();
}

/**
@brief   통과허가 주기 리턴
@author  puting
@date    2016.11.26
@return  List의 count를 리턴한다.
*/
int PathSearch::GetPassPermitPriod()
{
	return m_pParamSet->nPassPermitPeriod;
}

/**
@brief   통과허가 주기 리턴
@author  puting
@date    2016.11.26
@return  List의 count를 리턴한다.
*/
int PathSearch::GetPassPermitRollbackTime()
{
	return m_pParamSet->nPassPermitRollbackTime;
}

/**
@brief   통과허가 주기 리턴
@author  puting
@date    2016.11.26
@return  List의 count를 리턴한다.
*/
int PathSearch::GetPassPermitRollbackFinshTime()
{
	return m_pParamSet->nPassPermitRollbackFinshTime;
}

/**
@brief   통과허가 반납 마진 리턴
@author  puting
@date    2018.4.9
@return  List의 count를 리턴한다.
*/
double PathSearch::GetPassPermitRollbackMargin()
{
	return m_pParamSet->dPassPermitRollbackMargin;
}

//---------------------------------------------------------------------------
/**
@brief   Map 의 Node ID를 입력하여 NODE TYPE을 받는 함수
@author  임태웅
@date    2013-04-02
@param   pMap : 사용하는 Map
@param   uNID :  Node ID
@return  NODE TYPE
*/
bool PathSearch::getNodeType(MAP *pMap, UINT uNID, NODE_TYPE* pNodeType)
{
	bool bSuccess = true;
	
	NODE_INFO	*pNODE_INFO = NULL; // Node 정보
	NODE_TYPE NodeType;
	int		nNodeIndex			= 0;		// Node의 Index

	nNodeIndex = pMap->FindNodeIndex(uNID);

	if(nNodeIndex==INDEX_NONE)
	{
		bSuccess=false;
		return bSuccess;
	}

	pNODE_INFO = &(pMap->m_NodeArray[nNodeIndex]);

	if(pNODE_INFO->PrePathIndex[0]!=INDEX_NONE && pNODE_INFO->PrePathIndex[1]!=INDEX_NONE &&
	   pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE )
	{
		NodeType = MERGE_BRANCH_POINT;
	}
	else if(pNODE_INFO->PrePathIndex[0]!=INDEX_NONE && pNODE_INFO->PrePathIndex[1]!=INDEX_NONE)
	{
		NodeType = MERGE_POINT;
	}
	else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)
	{
		NodeType = BRANCH_POINT;
	}
	else
	{
		NodeType = NORMAL_POINT;
	}

	*pNodeType = NodeType;
	
	return bSuccess;
	
}


/**
@brief   Station 있는 Path의 정합성을 확인하는 함수
@author  임태웅
@date    2013-04-02
@param   pMap : 사용하는 Map
@param   FromNode : 시작 Node
@param   ToNode : Target Node
@param   AdditionalInfo : 주행 명령 추가 정보
@return  Error Code

int	PathSearch::GetPreSteering(MAP *pMap, UINT StopNode, UINT PreSteeringNode, PATH_STEERING_INFO *pPreSteeringInfo)
{
	MAP	*pTmpMap			= pMap;				// 사용하는 Map
	int	nStopNode			= StopNode;			// 시작 노드
	int	nPreSteeringNode	= PreSteeringNode;	// 끝 노드
	int	nTmpError			= 0;
	int	nCount				= 0;
	int	nCountOfList		= 0;

	LList 						DRIVING_INFO_List;
	EXECUTE_DRIVING_INFO		*pEXECUTE_DRIVING_INFO	= NULL;	// List에 저장하는 구조체
	EXECUTE_DRIVING_ADD_INFO	AdditionalInfo;
	PATH_STEERING_INFO			PathSteeringInfo = PATH_STEERING_NONE;

	memset(&AdditionalInfo, 0, sizeof(EXECUTE_DRIVING_ADD_INFO));

	//1 1. Path Search 실시
	nTmpError		= SearchDrivingPath(pTmpMap, nStopNode, nPreSteeringNode, AdditionalInfo, &DRIVING_INFO_List);
	nCountOfList	= DRIVING_INFO_List.getCount();

	//1 2. 조향 확인
	if(nTmpError == NO_ERR)
	{
		nCount = 0;

		for(int n=0; n<nCountOfList; n++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)DRIVING_INFO_List.popFront();
			if(pEXECUTE_DRIVING_INFO->BasicInfo.Steering == PATH_STEERING_NONE)
			{
				nCount++;
				delete pEXECUTE_DRIVING_INFO;

				continue;
			}
			else
			{
				PathSteeringInfo = pEXECUTE_DRIVING_INFO->BasicInfo.Steering;
				delete pEXECUTE_DRIVING_INFO;

				break;
			}
		}

		//2	결과 입력
//		pPreSteeringInfo->NodeCountFromStopNode	= nCount;
//		pPreSteeringInfo->PathSteeringInfo		= PathSteeringInfo;
		*pPreSteeringInfo = PathSteeringInfo;
	}

	nCountOfList = DRIVING_INFO_List.getCount();

	//1 3. 정보 소멸
	for(int n=0; n<nCountOfList; n++)
	{
		pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)DRIVING_INFO_List.popFront();
		delete pEXECUTE_DRIVING_INFO;
	}

	return nTmpError;
}
*/
bool PathSearch::CheckTurnOffOfPathInfo(MAP *pMap,UINT nCurNode)
{
	bool bReturn = false;
	MAP		*pTmpMap			= pMap;				// 사용하는 Map

	int		nNodeIndex			= 0;		// Node의 Index
	int		nPathIndex			= 0;		// Path의 Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보

	nNodeIndex = pTmpMap->FindNodeIndex(nCurNode);   ///< 출발 노드 Index
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

	switch(pNODE_INFO->InOutInfo)
	{
		case SLOPE_NTYPE:
		case LIFTPRE_NTYPE_LEFT:
		case LIFTPRE_NTYPE_RIGHT:
		case LIFTIN_NTYPE_LEFT:
		case LIFTIN_NTYPE_RIGHT:
		case LIFTOUT_NTYPE:
		case MTL_SEL_ON_LEFT:
		case MTL_SEL_ON_RIGHT:
		case MTL_STOP_LEFT:
		case MTL_STOP_RIGTH:
		case AUTODOOR_LEFT:
		case AUTODOOR_RIGHT:
         	bReturn = true;
		break;

	}

	return  bReturn;
}
bool PathSearch::CheckEmergencyPathInfo(MAP *pMap,UINT nCurNode)
{
	bool nReturn = false;
	MAP		*pTmpMap			= pMap;				// 사용하는 Map

	int		nNodeIndex			= 0;		// Node의 Index
	int		nPathIndex			= 0;		// Path의 Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보

	nNodeIndex = pTmpMap->FindNodeIndex(nCurNode);   ///< 출발 노드 Index
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

	switch(pNODE_INFO->InOutInfo)
	{
		case EmergencyNodeMTL_NTYPE:
			nReturn = true;
		break;

	}

	return  nReturn;
}

int PathSearch::CheckDeviceOfPathInfo(MAP *pMap,UINT nCurNode)
{
	int nReturn = 0;
	MAP		*pTmpMap			= pMap;				// 사용하는 Map

	int		nNodeIndex			= 0;		// Node의 Index
	int		nPathIndex			= 0;		// Path의 Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보

	nNodeIndex = pTmpMap->FindNodeIndex(nCurNode);   ///< 출발 노드 Index
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

	switch(pNODE_INFO->InOutInfo)
	{
		case SLOPE_NTYPE:
			nReturn = 1;
		break;
		case LIFTPRE_NTYPE_LEFT:
		case LIFTPRE_NTYPE_RIGHT:
		case LIFTIN_NTYPE_LEFT:
		case LIFTIN_NTYPE_RIGHT:
		case LIFTOUT_NTYPE:
        	nReturn = 2;
		break;
		case MTL_SEL_ON_LEFT:
		case MTL_SEL_ON_RIGHT:
		case MTL_STOP_LEFT:
		case MTL_STOP_RIGTH:
		case AUTODOOR_LEFT:
		case AUTODOOR_RIGHT:
			nReturn = 3;
		break;

	}

	return  nReturn;
}
PATH_DIVERGENCE_INFO PathSearch::GetStationDivInfo(MAP *pMap, UINT nFromNode, UINT nNextNode)
{

  	PATH_DIVERGENCE_INFO nTmpreturn = PATH_DIVERGENCE_NONE;
//	UINT	CmdID				= CmdGoData->CmdID;  //지시 ID
	MAP		*pTmpMap			= pMap;				// 사용하는 Map

//
	int		nNodeIndex			= 0;		// Node의 Index
	int		nPathIndex			= 0;		// Path의 Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보

	///시작이 Station인경우에 대한 전처리 부분

		//이적재의 경우 부모노드가 들어온다.

		nNodeIndex = pTmpMap->FindNodeIndex(nFromNode);   ///< 출발 노드 Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보

					if(pPATH_INFO->EndNID ==nNextNode)
					{
                     	if(n == 0)
							nTmpreturn = PATH_DIVERGENCE_LEFT;    //왼쪽 분기
						else
							nTmpreturn = PATH_DIVERGENCE_RIGHT;    //오른쪽 분기

						break;
                    }
				}
			}
		}
		else
		{
			ADD_LOG("[GetStationDivInfo] %d:%d"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
		}

        ADD_LOG("[GetStationDivInfo:FromNode %d, EndNode %d  %d",nFromNode, nNextNode, nTmpreturn);

		return nTmpreturn;
}

bool PathSearch::GetIsNBrench(MAP *pMap, UINT nFromNode, UINT nNextNode)
{

	bool nTmpreturn = false;
	MAP		*pTmpMap			= pMap;				// 사용하는 Map
	//
	int		nNodeIndex			= 0;		// Node의 Index
	int		nPathIndex			= 0;		// Path의 Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보


	///시작이 Station인경우에 대한 전처리 부분

		//이적재의 경우 부모노드가 들어온다.
		ADD_LOG("[PATHSEARCH_Start_Station_FromNode] %d",nFromNode);
		nNodeIndex = pTmpMap->FindNodeIndex(nFromNode);   ///< 출발 노드 Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

		if(nNodeIndex != INDEX_NONE)
		{
			if((pNODE_INFO->NextPathIndex[0]!=INDEX_NONE) && (pNODE_INFO->NextPathIndex[1]!=INDEX_NONE))// 시작 노드에 Path 연결되지 않은 경우
			{
				for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보

					if(pPATH_INFO->EndNID ==nNextNode)
					{
						if(pPATH_INFO->PathBranch ==  0x02)
						{
							nTmpreturn =true;
							break;
						}
					}
				}
			}
			else
			{
				ADD_LOG("[ERR_PATHSEARCH_FAIL_55] %d:%d"
				, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			}
		}

		ADD_LOG("[CheckNextNode:FromNode %d, Return %d",nFromNode,nTmpreturn);

		return nTmpreturn;

}


bool PathSearch::GetIsNBrenchMapMake(MAP *pTmpMap, UINT nPreNode, UINT nNextNode)
{
	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	PATH_INFO	*pPATH_INFO		= NULL;	// Path 정보
	int	 nPathIndex;
	bool nTmpreturn = false;
	int  nPreNodeIndex	= pTmpMap->FindNodeIndex(nPreNode);
	int  nNodeIndex = pTmpMap->FindNodeIndex(nNextNode);
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

	//합류부 앞, 분기 앞, 일반곡선
	 ADD_LOG("[PATHSEARCH_Start_NextNode,nNodeIndex] %d %d %d %d",nNextNode,nNodeIndex,pNODE_INFO->NextPathIndex[0],pNODE_INFO->NextPathIndex[1]);
	if(nNodeIndex != INDEX_NONE)
	{
		//분기
		if((pNODE_INFO->NextPathIndex[0]!=INDEX_NONE) && (pNODE_INFO->NextPathIndex[1]!=INDEX_NONE))// 시작 노드에 Path 연결되지 않은 경우
		{

			return true;
		}
		//합류, 일반
		else if(pNODE_INFO->NextPathIndex[0] != INDEX_NONE)
		{

			int NewNextNodeIndex = pNODE_INFO->NextPathIndex[0];
#if(USE_CHANGENODE == 1)
			BYTE UBGRegion = pTmpMap->m_PathArray[NewNextNodeIndex].UBGRegion;
#else
			BYTE UBGRegion = (pTmpMap->m_PathArray[NewNextNodeIndex].PathDirection & 0x04);
#endif
			BYTE Steering = pTmpMap->m_PathArray[NewNextNodeIndex].Steering;

			//분기직선
			if(Steering !=0x00 && UBGRegion == 0x01)
				return true;

			//곡선
			if(UBGRegion !=0x01)
				return true;


        }
	}
	//합류부 뒤, 분기 뒤, 곡선 뒤
	if(nPreNodeIndex!= INDEX_NONE)
	{

		nPathIndex  = pTmpMap->GetPathIndex(nPreNode, nNextNode);
#if(USE_CHANGENODE == 1)
		BYTE UBGRegion = pTmpMap->m_PathArray[nPathIndex].UBGRegion;
#else
        BYTE UBGRegion = (pTmpMap->m_PathArray[nPathIndex].PathDirection & 0x04);
#endif
		BYTE Steering = pTmpMap->m_PathArray[nPathIndex].Steering;

		//분기직선
		if(Steering != 0x00 && UBGRegion == 0x01)
			return true;

		//곡선
		if(UBGRegion != 0x01)
			return true;


	}



	ADD_LOG("[CheckNextNode:nNextNode %d, Return %d", nNextNode, nTmpreturn);

	return nTmpreturn;

}

PATH_STEERING_INFO PathSearch::GetSteeringInfo(MAP *pMap, UINT nFromNode, UINT nNextNode)
{

	PATH_STEERING_INFO nTmpreturn = PATH_STEERING_NONE;
	MAP		*pTmpMap			= pMap;				// 사용하는 Map
	//
	int		nNodeIndex			= 0;		// Node의 Index
	int		nPathIndex			= 0;		// Path의 Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보


	///시작이 Station인경우에 대한 전처리 부분

		//이적재의 경우 부모노드가 들어온다.
		ADD_LOG("[PATHSEARCH_Start_Station_FromNode] %d",nFromNode);
		nNodeIndex = pTmpMap->FindNodeIndex(nFromNode);   ///< 출발 노드 Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

		if((pNODE_INFO->NextPathIndex[0]!=INDEX_NONE) || (pNODE_INFO->NextPathIndex[1]!=INDEX_NONE))// 시작 노드에 Path 연결되지 않은 경우
		{
			for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보

					if(pPATH_INFO->EndNID ==nNextNode)
					{
						nTmpreturn = pPATH_INFO->Steering;
						break;
					}
				}
			}
		}

		ADD_LOG("[CheckNextNode] FromNode %d, Return %d",nFromNode,nTmpreturn);

		return nTmpreturn;

}

int	PathSearch::GetNextNode(MAP *pMap, UINT nFromNode, UINT nPathDiv)
{

   	int nTmpreturn = -1;
//	UINT	CmdID				= CmdGoData->CmdID;  //지시 ID
	MAP		*pTmpMap			= pMap;				// 사용하는 Map

//
	int		nNodeIndex			= 0;		// Node의 Index
	int		nPathIndex			= 0;		// Path의 Index

//
	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보

	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보

	///시작이 Station인경우에 대한 전처리 부분

	//이적재의 경우 부모노드가 들어온다.
	ADD_LOG("[PATHSEARCH_Start_Station_FromNode] %d",nFromNode);
	nNodeIndex = pTmpMap->FindNodeIndex(nFromNode);   ///< 출발 노드 Index
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

	if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
	{
		for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
		{
			if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
			{
				nPathIndex = pNODE_INFO->NextPathIndex[n];
				pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보

				if(nPathDiv !=0x00)
				{
					if(((nPathDiv ==0x01) && (n==0)) ||
					   ((nPathDiv ==0x02) && (n==1)))
					   {
							nTmpreturn  =  pPATH_INFO->EndNID;

					   }
					else if(nPathDiv==0x03)
					{

							nTmpreturn  =  pPATH_INFO->EndNID;
					}

				}
			}
		}
	}
	else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
	{
		for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
		{
			if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
			{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
					nTmpreturn  =  pPATH_INFO->EndNID;

			}
		}
	}
	else
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_55] %d:%d"
		, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
	}

	ADD_LOG("[CheckNextNode:FromNode %d, EndNode %d",nFromNode,nTmpreturn);

	return nTmpreturn;

}

/**
@brief   Station 있는 Path의 정합성을 확인하는 함수
@author  임태웅
@date    2013-03-20
@param   pMap : 사용하는 Map
@param   FromNode : 시작 Node
@param   ToNode : Target Node
@param   AdditionalInfo : 주행 명령 추가 정보
@return  Error Code
*/
int PathSearch::CheckPathOfStation(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo)
{
	int	nTmpError	= NO_ERR;
	
	MAP		*pTmpMap			= pMap;				// 사용하는 Map
	UINT	uFromNode			= FromNode;			// 시작 노드
	UINT	uToNode				= ToNode;			// 끝 노드

	int		nFromNodeIndex		= pTmpMap->FindNodeIndex(uFromNode);	// FromNode의 Index
	int		nToNodeIndex		= pTmpMap->FindNodeIndex(uToNode);		// ToNode의 Index
	UINT	nPathIndex			= 0;		// Path의 Index

//	double	dTmpPathDistance	= 0.0;	// Path의 Link 가중치(거리/속도)
	
	NODE_INFO	*pNODE_INFO	= NULL;	// Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보

	//1 0. Station 정보가 있을 경우만 에러 처리
	if(AdditionalInfo.StationType==MARK_TYPE_NODE)
	{	
		nTmpError	= NO_ERR;
		return nTmpError;
	}

	//1 1. 도착노드 다음노드 정보가 없을 경우 / 두 노드가 같을 경우
	if( nFromNodeIndex	==	-1	|| 
		nToNodeIndex	==	-1	||
		uFromNode		== uToNode 	)
	{
		nTmpError	= ERR_PATHSEARCH_FAIL;     //에러 발생 필요
		ADD_LOG("[ERR_PATHSEARCH_FAIL_1] %d:%d:%d:%d", nFromNodeIndex, nToNodeIndex, uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return nTmpError;
	}

	//1 2. Link가 1개가 아닐 경우(FromNode 다음 노드에 ToNode 존재 여부 확인)
	pNODE_INFO	= &(pMap->m_NodeArray[nFromNodeIndex]);
	nTmpError	= ERR_PATHSEARCH_FAIL;	// 문제 없을 경우 NO_ERR로 변경
	for(int i=0; i<2;i++)
	{
		nPathIndex	= pNODE_INFO->NextPathIndex[i];
		if(nPathIndex!=INDEX_NONE)
		{
			pPATH_INFO	= &(pMap->m_PathArray[nPathIndex]);
			if(pPATH_INFO->EndNID==uToNode)
			{
				nTmpError	= NO_ERR;
				break;
			}
		}
	}
	if(nTmpError!=NO_ERR)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_2] %d", pNODE_INFO->NID);
		ADD_FDC_LOG("ERR/%d/4/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
	}
	
	if(nTmpError	!= NO_ERR)
	{
		return nTmpError;
	}

	//1 3. Station으로의 거리가 Link 사이의 거리보다 클경우
	if(	AdditionalInfo.DistToStation > (pPATH_INFO->Distance+MARGIN_OF_DIST_TO_STATION) ||
		AdditionalInfo.DistToStation < 0)
	{
		nTmpError	= ERR_PATHSEARCH_FAIL;   //에러 발생 필요
		ADD_LOG("[ERR_PATHSEARCH_FAIL_3] %3.1lf:%3.1lf"
			, AdditionalInfo.DistToStation
			, pPATH_INFO->Distance);
		ADD_FDC_LOG("ERR/%d/5/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
	}

    return nTmpError;

}

/**
@brief   Driving Tag PathSearch Map Search 함수
@author  ehoto
@date    2012-11-01
@param   pMap : 사용하는 Map
@param   FromNode : 시작 Node
@param   ToNode : Target Node
@param   AdditionalInfo : 주행 명령 추가 정보
@param   DRIVING_INFO_List : 주행 명령 추가 List
@return  Error Code

int PathSearch::SearchDrivingPath(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	// 0. 값 초기화 ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// 현재 에러 초기화

	MAP		*pTmpMap			= pMap;				// 사용하는 Map
	UINT	uFromNode			= FromNode;			// 시작 노드
	UINT	uToNode				= ToNode;			// 끝 노드
	LList	*pResultAddList	= DRIVING_INFO_List;	// 주행을 위한 명령을 추가하기 위한 List

	bool	bArrived			= false;	// Map Seach에 성공한 경우
	UINT	uPathSearchIndex	= 0;		// PathSearch List의 Index
	UINT	uListInsertIndex	= 0;		// PathSearch List 삽입시 사용되는 Index
	UINT	uTmpListCount		= 0;		// PathSearch List 내의 인자 수

	int		nNodeIndex			= 0;		// Node의 Index
	int		nPathIndex			= 0;		// Path의 Index
	UINT	uTmpToNode			= 0;		// Path 단위로 정보 확인 시
	UINT	uTmpFromNode		= 0;		// Path 단위로 정보 확인 시
	double	dTmpTotalDistance	= 0.0;	// 현재 Node까지의 총 가중치(거리/속도)
	double	dTmpPathDistance	= 0.0;	// Path의 Link 가중치(거리/속도)

	int		dMaxPathSearchListCnt = 0;
	int		dtmpPathSearchListCnt = 0;
	
	NODE_INFO	*pNODE_INFO	= NULL;	// Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보
	
	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search를 위한 Node 구조체
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search를 위한 Node 구조체
	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread에 전달하는 구조체

	LList PathSearchList;									// PathSearch를 위한 List
	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	// Node 정보를 찾기 위한 Map
	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

//	NODE_TYPE DrivingEndNodeType = NORMAL_POINT;
	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";

	OHTMainStatus *pOHTMainStatusTmp = OHTMainForm->GetOHTStatus();

	// 1. 초기 설정 : ----------------------------------------------------------
	//   0) 시작 Node의 Path 확인
	//   1)끝 Node List에 추가
	//   ※ 역순으로 찾으면 마지막 처리가 순방향으로 결과가 나옴
	ADD_LOG("[PATHSEARCH_Step 1]");
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
//	||	(uFromNode == uToNode)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_PATHSEARCH_FAIL;     //에러발생필요
		
	}

	if(AdditionalInfo.StationType!=MARK_TYPE_NODE)
	{
		nTmpError = CheckPathOfStation(pTmpMap, uFromNode, uToNode, AdditionalInfo);
		if(nTmpError!=NO_ERR)
		{
			return nTmpError;
		}
	}
	ADD_LOG("[PATHSEARCH_Step 1-1]");
	nNodeIndex = pTmpMap->FindNodeIndex(uFromNode);
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);
	if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
	{
		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
		ADD_LOG("[ERR_PATHSEARCH_FAIL_51] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[0]);
		ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return nTmpError;
	}

	uTmpToNode = uToNode;
	pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;	// PathSearch Node 정보 생성 : 없을 경우
	pPATH_SEARCH_NODE_INFO->NID = uTmpToNode;
	pPATH_SEARCH_NODE_INFO->NextNID = 0;
	pPATH_SEARCH_NODE_INFO->PathIndex = INDEX_NONE;
	pPATH_SEARCH_NODE_INFO->dTotalDistance = 0.0;

	PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);	// List 에 삽입
	mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;		// map 에 삽입
	uPathSearchIndex = 0;
	dMaxPathSearchListCnt = 0;

	// 2. Index 순차적으로 증가시키면서 Node 추가 ------------------------------
	//   1) 다음 위치로 Index를 증가시켜가면서 다음 노드들을 PathSarchList에 추가함
	//     . PathSearchList에 추가 할 때는 거리 가중치 반영하여 가까운 순서로 삽입
	//     . 중복되는 Node 가 있을 경우 거리 가중치가 가까운 노드로 삽입, 먼 노드는 삭제
	//   2) 시작 노드에 연결 Path의 개수 만큼 시작노드가 발견되면 정지
	ADD_LOG("[PATHSEARCH_Step 1-2]");

	while(bArrived==false)	// 시작 Node 나올 경우 bArrived 가 true로 바꿔줌
	{
		pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex);
		uPathSearchIndex++;

		dtmpPathSearchListCnt = PathSearchList.getCount();
		if(dtmpPathSearchListCnt > dMaxPathSearchListCnt) 
		{
			dMaxPathSearchListCnt = dtmpPathSearchListCnt;
			
			if((dMaxPathSearchListCnt > m_pParamSet->nMapSearchMax) && (AdditionalInfo.FinalNode == 0))
			{
				nTmpError = ERR_PATHSEARCH_FAIL;
				ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dMaxPathSearchListCnt, m_pParamSet->nMapSearchMax);
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				break;
			}
		}
		
		// 시작노드 도달 확인
		if(pPATH_SEARCH_NODE_INFO != NULL)
		{
			if(pPATH_SEARCH_NODE_INFO->NID == uFromNode)
			{
				bArrived = true;
				break;
			}
		}

		if(pPATH_SEARCH_NODE_INFO != NULL)	// mapPathSearchNode 에  uTmpToNode 의 PathSearch Node정보가 없으면
		{
			dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;
		}
		else
		{
			nTmpError = -1; // PathSearch 실패
			break;
		}

		uTmpToNode = pPATH_SEARCH_NODE_INFO->NID;
		nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index 획득
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

		for(int n=0; n<2; n++)// 이전 Path 개수 만큼 확인
		{
			nPathIndex = pNODE_INFO->PrePathIndex[n];		// Path Index 획득
			if(nPathIndex!=-1)
			{
				pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// Path 정보
				dTmpPathDistance = (double)pPATH_INFO->Distance / (double)pPATH_INFO->Speed;	// Link 길이 획득 : 이동 시간 (거리/속도)
				uTmpFromNode = pPATH_INFO->StartNID;            // 이전 Node ID 획득

				IterPos = mapPathSearchNode.find(uTmpFromNode);
				if(IterPos == mapPathSearchNode.end())	// mapPathSearchNode 에  uTmpFromNode 의 PathSearch Node정보가 없으면
				{
					// PathSearch Node 정보 생성 : 없을 경우
					pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
					pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
					pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
					pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
					pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

					//추가하는 루틴
					pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.referBack();
					if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance <= pPATH_SEARCH_NODE_INFO->dTotalDistance)	// 거리가 제일 먼 경우
					{
						PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
						mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;
					}
					else	// 중간에 삽입 되는 경우
					{
						uListInsertIndex = 0;
						uTmpListCount = PathSearchList.getCount();
						while(uListInsertIndex<uTmpListCount)
						{
							pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
							if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
							{
								PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
								mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;
								uListInsertIndex = uTmpListCount;	// while 문 끝냄
							}
							else
							{
								uListInsertIndex++;
							}
						}	// while
					}
				}
				else	// mapPathSearchNode 에  uTmpFromNode 의 PathSearch Node정보가 있으면
				{
					pTempPATH_SEARCH_NODE_INFO = (*IterPos).second;
					if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > dTmpTotalDistance + dTmpPathDistance) // 값이 같거나 클 경우 먼길-->skip
					{
						// PathSearch Node 정보 생성 : 추가할 정보
						pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
						pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
						pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
						pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
						pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

						// 리스트에서 추가 및 기존 값 제거 루틴
						uListInsertIndex = 0;
						uTmpListCount = PathSearchList.getCount();
						while(uListInsertIndex<uTmpListCount)
						{
							pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
							if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
							{
								// 새로운 값 추가
								PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
								// 기존 거리 먼 인자 것 제거
								uTmpListCount = PathSearchList.getCount();
								uListInsertIndex++;
								while(uListInsertIndex<uTmpListCount)
								{
									pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
									if(pTempPATH_SEARCH_NODE_INFO==(*IterPos).second)
									{
										PathSearchList.pop(uListInsertIndex); 	// List 에서 제거
										mapPathSearchNode.erase(IterPos);
										mapPathSearchNode[uTmpFromNode]=pPATH_SEARCH_NODE_INFO;
										delete pTempPATH_SEARCH_NODE_INFO;		// 인자 소멸
									}
									uListInsertIndex = uTmpListCount;	// while 문 끝냄
								}
							}
							else
							{
								uListInsertIndex++;
							}
						}	// while
					}
				}
			}	// if(nPathIndex!=-1)
		}	// for(int n=0; n<2; n++)
	}

	ADD_LOG("[PATHSEARCH_Step 1-3]");
	// 3. Start Node 에서 다음 Node 찾아가면서 Driving List 만듬 ---------------
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) 에 추가
	bArrived		= false;
	uTmpListCount	= PathSearchList.getCount();
	uTmpFromNode	= uFromNode;

	while(bArrived==false && nTmpError == NO_ERR)
	{
        IterPos = mapPathSearchNode.find(uTmpFromNode);
		
        if( IterPos != mapPathSearchNode.end() )
        {
            pPATH_SEARCH_NODE_INFO = IterPos->second;
        }
        else
        {
        	// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드 
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_6]");
			ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
        }

		if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) || 
			(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false)	)
		{
        	// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드 
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
			ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		}

		if(pTmpMap->tNodeDisalbeList != NULL)
		{
			for(int j=0 ; j < pTmpMap->tNodeDisalbeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tNodeDisalbeList->Strings[j];

				if((uTmpFromNode == StrToIntDef(StrTmp, 0)) || (pPATH_SEARCH_NODE_INFO->NextNID == StrToIntDef(StrTmp, 0)))
				{
					ADD_LOG("[ERR_NODE_DISABLE] %d:%d:%d", StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}
			}
		}

		if((pTmpMap->tPassBoxNodeList != NULL) && (pOHTMainStatusTmp->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL))
		{
			for(int j=0 ; j < pTmpMap->tPassBoxNodeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tPassBoxNodeList->Strings[j];

				if((uTmpFromNode == StrToIntDef(StrTmp, 0)) && ((uTmpFromNode != FromNode) || (AdditionalInfo.FinalPortType != GO_FINAL_PORT_TYPE_PassBoxOut)))
				{
					ADD_LOG("[ERR_NODE_DISABLE_BY_PASSBOX_#1] %d:%d:%d:%d:%d:%d", FromNode, ToNode, StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID,AdditionalInfo.FinalPortType);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}

				if((pPATH_SEARCH_NODE_INFO->NextNID == StrToIntDef(StrTmp, 0)) && ((StrToIntDef(StrTmp, 0) != ToNode) || (AdditionalInfo.FinalPortType != GO_FINAL_PORT_TYPE_PassBoxIn)))
				{
					ADD_LOG("[ERR_NODE_DISABLE_BY_PASSBOX_#2] %d:%d:%d:%d:%d:%d", FromNode, ToNode, StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID,AdditionalInfo.FinalPortType);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}
			}
		}

		nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
		pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= uTmpFromNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= pPATH_SEARCH_NODE_INFO->NextNID;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
		pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
		pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
		pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
		pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, 분기 정보 추가
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;

		// for change node (jhoun.lim)
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;

#else
		pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, 분기 정보 추가
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;
#endif
	   //	pEXECUTE_DRIVING_INFO->AddInfo					= AdditionalInfo;

		memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
		// MapMake 일 경우 Map Make 속도 반영 : 위치 및 처리 방법에 대해 추가 고민 필요
		if(	(AdditionalInfo.CmdType==EXECUTE_DRIVING_CMD_MAPMAKE) &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
		{
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
		}

//		Route_ADD_LOG("[PathSearch] Node:%X~%X",pEXECUTE_DRIVING_INFO->StartNID, pEXECUTE_DRIVING_INFO->EndNID);
		pResultAddList->insertBack((void*)pEXECUTE_DRIVING_INFO);

		uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
		if(uTmpFromNode == uToNode)
		{
			bArrived = true;
		}
	}

	if(nTmpError != NO_ERR)
	{
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)pResultAddList->popFront();
			delete pEXECUTE_DRIVING_INFO;
		}	
	}
	
	// 4. Path Search List 소멸 ------------------------------------------------
	//   1) 인자들 delete
	//   2) List 소멸
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}

    ADD_LOG("[PATHSEARCH_Step 1-4]");
	return nTmpError;
}
*/
/**
@brief   ChangPoint 정보를 Search 함수
@author  puting
@date    2016-07-02
@param   StartNode : 시작 Node
@param   DefaultDiv : 분기 정보 방향
@return  0 성공, 나머지는 에러.
*/
bool PathSearch::CheckSearchChangePoint(int ChangeNodeID, UINT &ChangeSqNum, double &ChangeNodeDistance)
{
	bool bArrived = false;
	int Index = 0;
	bool bCheckFinish = false;
	EXECUTE_DRIVING_INFO 	*pPathListInfo = NULL;

	// 1) Path List 내용 확인
	for(Index =0; Index< GetPathCount(); Index++)
	{
		pPathListInfo = (EXECUTE_DRIVING_INFO*) ReferPath(Index);

		if(pPathListInfo !=NULL)
		{
			if(pPathListInfo->BasicInfo.StartNID ==  ChangeNodeID)
			{
				bArrived = true;
				break;
			}
		}
	}

	if(Index ==(GetPathCount() -1))  bCheckFinish = true;

	if(bArrived == true)
	{

		ChangeSqNum = pPathListInfo->SQNum;
		ChangeNodeDistance = pPathListInfo->ChangeEndNodeDistance - pPathListInfo->BasicInfo.dLength;
		ADD_LOG("[CheckSearchChangePoint] %d %f:%f--------",ChangeNodeID, pPathListInfo->ChangeEndNodeDistance,pPathListInfo->BasicInfo.dLength);

		if(bCheckFinish ==true)
		{
			if(pPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == true)
			{
				ChangeNodeDistance = pPathListInfo->ChangeEndNodeDistance - pPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist;
				ADD_LOG("[CheckSearchChangePoint %d:%d]",Index,ChangeNodeID);
			}
		}
	}
	else
	{
		//마지막 위치인지 확인하는 부분
		if(pPathListInfo !=NULL)
		{
			if(pPathListInfo->BasicInfo.EndNID ==  ChangeNodeID)
			{
				bArrived = true;
				ChangeSqNum = pPathListInfo->SQNum;
				ChangeNodeDistance = pPathListInfo->ChangeEndNodeDistance;
				ADD_LOG("[CheckSearchChangePoint22] %d:%f--------",ChangeNodeID,pPathListInfo->ChangeEndNodeDistance);
			}
		}
    }


	return bArrived;
}
/**
@brief   다음 분기 Search 함수
@author  puting
@date    2016-06-19  , 2018.03.10 우선 너부터 변경함.
@param   pMap : 사용하는 Map
@param   StartNode : 시작 Node
@param   DefaultDiv : 분기 정보 방향
@return  0 성공, 나머지는 에러.
*/
int PathSearch::CheckSearchDifferntPoint(MAP *pMap, PARSING_GO_INFO* CmdGoData, UINT &ChangeSqNum, int &ChangeNodeID, double &ChangeNodeDistance, int &nExtraPath, BYTE &chNackToAckType, UINT nPreTargetStation)
{
	int nTmpError				= NO_ERR;					// 현재 에러 초기화

	UINT	CmdID				= CmdGoData->CmdID;  //지시 ID
	MAP		*pTmpMap			= pMap;				// 사용하는 Map

	//경로탐색을 해야될 시작과 끝 위치
	UINT	uFromNode			= CmdGoData->PathData_StartNodeID;			// 시작 노드
	UINT	uToNode				= CmdGoData->PathData_EndNodeID;		    // 끝 노드


	bool	bArrived			= false;	// Map Seach에 성공한 경우

	int		nPathListIndex	   	= 0;		// 비교하고자하는 경로 위치.

	UINT	uPathSearchIndex	= 0;		// PathSearch List의 Index
	UINT	uTmpListCount		= 0;		// PathSearch List 내의 인자 수

	int		nNodeIndex			= 0;		// Node의 Index
	int		nPathIndex			= 0;		// Path의 Index


	PATH_DIVERGENCE_INFO     nTempDivInfo        = PATH_DIVERGENCE_NONE;         //분기정보

	UINT	uTmpToNode			= 0;		// Path 단위로 정보 확인 시
	UINT	uTmpFromNode		= 0;		// Path 단위로 정보 확인 시

	double	dTmpTotalDistance	= 0.0;	// 현재 Node까지의 총 가중치(거리/속도)
	double	dTmpPathDistance	= 0.0;	// Path의 Link 가중치(거리/속도)


	//기본 분기 방향 // 중요한 부분. 0 : 왼쪽 1: 오른쪽
	int	bDefaultPathDirection = (int)CmdGoData->PathData_Def_Direction;
	bool	bCheckDefaultPathDirection = false;

	int		dtmpPathSearchListCnt = 0; //Pass Permit Limit 체크용
	int     uCheckExtraNum =0;
	int     nLastExtraNID = 0;

	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	NODE_INFO	*pNextNODE_INFO	= NULL;	// NextNode 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보


	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search를 위한 Node 구조체
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search를 위한 Node 구조체

	EXECUTE_DRIVING_INFO 	*pPathListInfo              = NULL;
	EXECUTE_DRIVING_INFO 	*pTempPathListInfo          = NULL;
	LList PathSearchList;									// PathSearch를 위한 List
//	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	// Node 정보를 찾기 위한 Map
//	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;



	//------------- 1. 전처리 부분------------//
	// 1. 초기 설정 : ----------------------------------------------------------
	//   0) 시작 Node의 Path 확인
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
  //	||	(uFromNode == uToNode)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_PATHSEARCH_FAIL;

	}

	ADD_LOG("[PATHSEARCH_Step 1 from%d To %d : Dir :%d]",uFromNode,uToNode,bDefaultPathDirection);



//	//------------- 2. 주요 처리부분------------//
//	// 1) 시작 위치인지 확인
	for(int i=0; i< GetPathCount(); i++)
	{
		pPathListInfo = (EXECUTE_DRIVING_INFO*) ReferPath(i);
		ADD_LOG("[PATHSEARCH Start ID]%d ExtraID:%d %d",pPathListInfo->BasicInfo.StartNID, CmdGoData->PathData_ExtraID[uCheckExtraNum],CmdGoData->PathData_ExtraNum);

		ChangeSqNum = pPathListInfo->SQNum;  //초기값 갱신

		/*
		if((CmdGoData->PathData_ExtraID[uCheckExtraNum] == pPathListInfo->BasicInfo.StartNID) &&
			(pPathListInfo->BasicInfo.StartNID !=  uFromNode))
		{

			for(int j=0; j<CmdGoData->PathData_ExtraNum; j++)
			{
				ADD_LOG("[Error_ExtraID:%d %d",CmdGoData->PathData_ExtraID[j],j);
			}

			return -1;         //빠질리 없음 빠지면 안되용.

		}
         */

		if(pPathListInfo->BasicInfo.StartNID ==  uFromNode)
		{
			nPathListIndex = i;
			bArrived = true;
			break;
		}
	}

	//시작위치를 못찾으면 에러 처리. (작업변경 Nack)

	if(bArrived == false)
	{
		return -1;
	}

//	if((uFromNode == uToNode) &&
//	  (bArrived == true))
//	{
//		ChangeSqNum = pPathListInfo->SQNum;
//		ChangeNodeID = pPathListInfo->BasicInfo.StartNID;
//		ChangeNodeDistance = pPathListInfo->ChangeEndNodeDistance - pPathListInfo->BasicInfo.dLength;
//		return nTmpError;
//	}

//	if(bArrived ==false)
//		return ERR_STD_INNER_PATH_FAIL; // PathSearch 실패

	// 3) 시작 Node위치 List에 저장
	uTmpFromNode = uFromNode;
	uPathSearchIndex = 0;
	bArrived = false;

	while(bArrived==false)	// 시작 Node 나올 경우 bArrived 가 true로 바꿔줌
	{
		nTempDivInfo        = PATH_DIVERGENCE_NONE;         //분기정보
		////< (1) first Node Check!!
		if(uPathSearchIndex !=0)
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex-1);
			////< (1)-1 현재 Node의 정보 확인
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			}
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보


			///< (1)-2 Node Info Check!!
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{

				///(3)-2 거리 정보 확인 필요
				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;

			}
			else
			{
				nTmpError = ERR_STD_INNER_PATH_FAIL; // PathSearch 실패
			}

		}
		else
		{
			//uTmpFromNode = uTmpFromNode;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

		}

		////< (2) List의 Max Count Check!!     puting -----------------------------------------------------------추후삭제나 수정유무 판단.
		dtmpPathSearchListCnt = PathSearchList.getCount();
		if(dtmpPathSearchListCnt > m_pParamSet->nMapSearchMax)
		{
			nTmpError = ERR_PATHSEARCH_FAIL;
			ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dtmpPathSearchListCnt , m_pParamSet->nMapSearchMax);
            ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		}

		////< (4) 현재 노드가 아무런 PathIndex가 없을 경우 에러 처리 추가
		if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			nTmpError = ERR_PATHSEARCH_FAIL;
			ADD_LOG("[ERR_PATHSEARCH_FAIL_54] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;

		}

		///<(6) Next Node(Path) 정보가 어떤 건지 확인하는 부분  puting 20151107
 		////< (6)-1 분기인 경우,
		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			  ////< (6-1) 기본외 번지인지 혹인하는 부분  puting 20151107
			if(CmdGoData->PathData_ExtraID[uCheckExtraNum]==pNODE_INFO->NID)
			{
				ADD_LOG("[PathData_ExtraID %d",CmdGoData->PathData_ExtraID[uCheckExtraNum]);
				bCheckDefaultPathDirection = true;  //기본외 분기 일 경우
				uCheckExtraNum++;
				nExtraPath++;

				nLastExtraNID = pNODE_INFO->NID;

				if(uCheckExtraNum > CmdGoData->PathData_ExtraNum)
				{
					nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
					ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				}
			};

			/////< (6-3) 분기에서 가야될 Path 정보 확인

			////Map 부분 수정 필요.     확인 필요 조건이 반대일 경우 들어옴.
			for(int nTempIndex=0; nTempIndex<2; nTempIndex++)// 다음 Path 개수 만큼 확인
			{

				if(bCheckDefaultPathDirection == true)
				{
					if(bDefaultPathDirection != nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //왼쪽 분기
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //왼쪽 분기

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
					}

				}
				else
				{
					if(bDefaultPathDirection == nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //왼쪽 분기
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //왼쪽 분기

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
					}

				}
			}

                  //오른쪽이 0, 왼쪽이 1

			bCheckDefaultPathDirection = false;
		}

		////< (6-3) 분기가 아닌경우,
		else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
			{
				//nTempDivInfo =  PATH_DIVERGENCE_NONE;
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
					nPathIndex = pNODE_INFO->NextPathIndex[n];
			}

		}

		///< (7) 연결된 Next Path(Node) 정보 확인 및 생성
		////<(7)-1 연결된 Path가 존재 하는 경우
		if(nPathIndex!=INDEX_NONE)
		{
					////<(7)-1 Next Path 정보
			uPathSearchIndex++;

			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보


			dTmpPathDistance = (double)pPATH_INFO->Distance;
			uTmpToNode = pPATH_INFO->EndNID; //연결된 다음 노드


			if(uPathSearchIndex >MAX_PATHINDEX)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
				ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

			//가중치는 필요없음.


			// PathSearch Node 정보 생성 : 없을 경우
			pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
			pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
			pPATH_SEARCH_NODE_INFO->IndexNum = uPathSearchIndex;

		   //	pPATH_SEARCH_NODE_INFO->PassPermit_NType =
//				pPATH_SEARCH_NODE_INFO->SQID =  uSequenceIndex;
			pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;

			pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;

			pPATH_SEARCH_NODE_INFO->DivInfo = nTempDivInfo;

			pPATH_SEARCH_NODE_INFO->PreSteering = pPATH_INFO->Steering;     //선조향 관련된 정보 추가. puting 20160119

			///puting 추가해야되는 사항/ 초기거리와 Target에 따라 거리값 변경.
			//  Step 1.Target이 station일 경우 거리값 추가할것.
			pPATH_SEARCH_NODE_INFO->dTotalDistance = (double)dTmpPathDistance + (double)dTmpTotalDistance;
			pPATH_SEARCH_NODE_INFO->dPathDistance  = (double)dTmpPathDistance;
//				ADD_LOG("[PATHSEARCH_Stepll %06f]",pPATH_SEARCH_NODE_INFO->dTotalDistance);

			//추가하는 루틴
			PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
//				mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;

			pTempPathListInfo = (EXECUTE_DRIVING_INFO*) ReferPath(nPathListIndex);
			nPathListIndex++;



			///< (3)-1 마지막 노드 도달 확인  //이적재에 스도록 코드 변경 필요 puting  20180311
			//짧은 경우
		  if(nPathListIndex != GetPathCount())
		   {
				 //짧은 경우
				 if(pPATH_SEARCH_NODE_INFO->NID == uToNode)
				 {

					  ADD_LOG("[PATHSEARCH_Diff_Short %d:%d]",pPATH_SEARCH_NODE_INFO->NextNID,pTempPathListInfo->BasicInfo.EndNID);
					  ChangeSqNum = pTempPathListInfo->SQNum;
					  ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
					  ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - pTempPathListInfo->BasicInfo.dLength;
					  chNackToAckType = JobChageNackType::CycleChage_Type1;

					  if(pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == true)
					  {
							ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist;
					  }

					  if (nLastExtraNID == pPATH_SEARCH_NODE_INFO->NID)
					  {
							nExtraPath--;
					  }

					  bArrived = true;
					  break;

				  }
				  //경로가 변경되는 경우
				  else if(pPATH_SEARCH_NODE_INFO->NextNID != pTempPathListInfo->BasicInfo.EndNID)
				 {
						ADD_LOG("[PATHSEARCH_Diff_Mid %d:%d %06f %06f]"
						,pPATH_SEARCH_NODE_INFO->NextNID,pTempPathListInfo->BasicInfo.EndNID,pTempPathListInfo->ChangeEndNodeDistance,pTempPathListInfo->BasicInfo.dLength);
			//					ChangeSqNum = pTempPathListInfo->SQNum;
						ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
						ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - pTempPathListInfo->BasicInfo.dLength;
						chNackToAckType = JobChageNackType::CycleChage_Type1_1;

						if (nLastExtraNID == pPATH_SEARCH_NODE_INFO->NID)
						{
							nExtraPath--;
						}

						bArrived = true;
						break;
				}
		   }
		   else
		   {

				//짧은 경우(이적재 Tag는 동일하나 다른분기로 가는경우.)
				if((pPATH_SEARCH_NODE_INFO->NextNID != pTempPathListInfo->BasicInfo.EndNID)
				  || ((CmdGoData->StopStationID !=0)&&
					 (CmdGoData->StationData_ParentNodeID == pTempPathListInfo->BasicInfo.StartNID) &&
					 (CmdGoData->StationData_NextNodeID != pTempPathListInfo->BasicInfo.EndNID)))
				{
					ChangeSqNum = pTempPathListInfo->SQNum;
					ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
					ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - pTempPathListInfo->BasicInfo.dLength;
					chNackToAckType = JobChageNackType::CycleChage_Type1;

					ADD_LOG("[PATHSEARCH_Diff_Short_2 Ne%d:Cha%d:End %d IsStat:%d %f %d]",
						pPATH_SEARCH_NODE_INFO->NextNID,
						ChangeNodeID,
						pTempPathListInfo->BasicInfo.EndNID,
						pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd,
						pTempPathListInfo->ChangeEndNodeDistance,
						pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist);


					if(pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == true)
					{
						ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - (double)pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist;
					}

					if (nLastExtraNID == pPATH_SEARCH_NODE_INFO->NID)
					{
					   nExtraPath--;
					}

				}
				//동일하거나 먼 경우
				else
				{


					 ADD_LOG("[PATHSEARCH_Diff_End Ne%d:Cha%d:End %d IsStat:%d %f %d]",
					 pPATH_SEARCH_NODE_INFO->NextNID,
					 ChangeNodeID,
					 pTempPathListInfo->BasicInfo.EndNID,
					 pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd,
						pTempPathListInfo->ChangeEndNodeDistance,
					 pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist);


					 ChangeSqNum = pTempPathListInfo->SQNum;
					 ChangeNodeID = pTempPathListInfo->BasicInfo.EndNID;
					 ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance;

					 //동일하거나 짧은 경우
					 // (pPATH_SEARCH_NODE_INFO->NextNID == pTempPathListInfo->BasicInfo.EndNID)
					  if(pPATH_SEARCH_NODE_INFO->NID == uToNode)
					  {
					  		ChangeSqNum = pTempPathListInfo->SQNum;
							ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
							ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - pTempPathListInfo->BasicInfo.dLength;

							chNackToAckType = JobChageNackType::CycleChage_Type1;

							if(pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == true)
							{
							//Start와 End가 동일할때만 한바퀴 돌리기.
								if(CmdGoData->PathData_StartNodeID == CmdGoData->PathData_EndNodeID)
								{
										chNackToAckType = JobChageNackType::CycleChage_Type2;//이적재에 스도록 코드 변경 필요 puting
								}

								//전방에 바로 이동할 경우만 nack에서 ack로 처리.
								if((CmdGoData->StopStationID !=0) &&
								   ((CmdGoData->StationData_RunDist -pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist) >200.0))
								  {
										chNackToAckType = JobChageNackType::CycleChage_Type2;//이적재에 스도록 코드 변경 필요 puting
								  }

								 if(nPreTargetStation == CmdGoData->StopStationID)
								 {
										chNackToAckType = JobChageNackType::CycleChage_Type2;//이적재에 스도록 코드 변경 필요 puting
								 }

								  //동일하지만, 이적재 Tag Next Node위치에 따라서 차이점 확인 할것.(동일한 이적재 Tag, 다른 이적재 Tag에 따라서 분류할것)
								  ADD_LOG("[PATHSEARCH_Diff_End_1 %d:%d]",pPATH_SEARCH_NODE_INFO->NextNID,pTempPathListInfo->BasicInfo.EndNID);
								  ChangeSqNum = pTempPathListInfo->SQNum;
								  ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
								  ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - (double)pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist;

							 }

							 if (nLastExtraNID == pPATH_SEARCH_NODE_INFO->NID)
							  {
							   nExtraPath--;
							  }

						}

					//동일한 경우, 주행의 경우는 작업변경 우선 작업변경 Nack 처리할것.
					// (pPATH_SEARCH_NODE_INFO->NextNID == pTempPathListInfo->BasicInfo.EndNID)
						 else if((pPATH_SEARCH_NODE_INFO->NextNID == uToNode) &&
							 (pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == false))
					   {
							 //해당케이스는 일반적인경우는 없음.
							ADD_LOG("[PATHSEARCH_Diff_End_2 %d:%d]",pPATH_SEARCH_NODE_INFO->NextNID,pTempPathListInfo->BasicInfo.EndNID);
							ChangeSqNum = pTempPathListInfo->SQNum;
							ChangeNodeID = pTempPathListInfo->BasicInfo.EndNID;
							ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance;
							chNackToAckType = JobChageNackType::CycleChage_Type1_1;

					   }
					   else //먼경우
					   {
							 //이적재인경우 찾는노드를 주행노드로 변경할것 부모노드로 거기까지 짜르고 다시 이어줘야됨.
							chNackToAckType = JobChageNackType::CycleChage_Type2;//이적재에 스도록 코드 변경 필요 puting
							if(pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == true)
							{
								ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
								ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - (double)pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist;
								ADD_LOG("[PATHSEARCH_Diff_End_1_3 Ne%d:Cha%d:End %d IsStat:%d %f %d]",
										pPATH_SEARCH_NODE_INFO->NextNID,
										ChangeNodeID,
										pTempPathListInfo->BasicInfo.EndNID,
										pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd,
										pTempPathListInfo->ChangeEndNodeDistance,
										pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist);

								if (nLastExtraNID == pPATH_SEARCH_NODE_INFO->NID)
								{
									nExtraPath--;
								}

							}

						   ADD_LOG("[PATHSEARCH_Diff_End_1-4 Ne%d:Cha%d:End %d IsStat:%d %f %f]",
							pPATH_SEARCH_NODE_INFO->NextNID,
							ChangeNodeID,
							pTempPathListInfo->BasicInfo.EndNID,
							pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd,
							pTempPathListInfo->ChangeEndNodeDistance,
							(double)pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist);
						}
				}

				bArrived = true;
				break;
		   }

			ChangeSqNum = pTempPathListInfo->SQNum;  //시퀀스 넘버 갱신
		}

		if(nTmpError!=NO_ERR) break;


		nPathIndex = INDEX_NONE;

	 }  //while()

	 //3. PathSerchlist 삭제
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{

		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();

		ADD_LOG("[PATHSEARCH_Diff_ %d:%d:%d:%d",uTmpListCount,uIndex, pTempPATH_SEARCH_NODE_INFO->SQID,pTempPATH_SEARCH_NODE_INFO->NID);
		delete pTempPATH_SEARCH_NODE_INFO;
	}

      return nTmpError;
}

/**
@brief   Driving Tag Map Search 함수
@author  puting
@date    2015-10-28
@param   pMap : 사용하는 Map
@param   FromNode : 시작 Node
@param   ToNode : Target Node
@param   AdditionalInfo : 주행 명령 추가 정보
@param   DRIVING_INFO_List : 주행 명령 추가 List
@return  Error Code
*/

int PathSearch::StandardSearchPassPermit(MAP *pMap, STD_FlowControl *pFlowMap, PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List, UINT &uCheckExtraNum,bool bIsFoup,double PathExtra,bool bIgnorOverrun,bool bNoonechek)
{
	// 0. 값 초기화 ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// 현재 에러 초기화

	UINT	CmdID				= CmdGoData->CmdID;  //지시 ID
	MAP		*pTmpMap			= pMap;				// 사용하는 Map

	//경로탐색을 해야될 시작과 끝 위치
	UINT	uFromNode			= AdditionalInfo.ProcessingGoInfo.StartNodeID;			// 시작 노드
	UINT	uToNode				= AdditionalInfo.ProcessingGoInfo.EndNodeID;		    // 끝 노드

	LList	*pResultAddList		= DRIVING_INFO_List;	// 주행을 위한 명령을 추가하기 위한 Cycle List

	bool	bArrived			= false;	// Map Seach에 성공한 경우

	UINT	uPathSearchIndex	= 0;		// PathSearch List의 Index
//	UINT	uListInsertIndex	= 0;		// PathSearch List 삽입시 사용되는 Index
	UINT	uTmpListCount		= 0;		// PathSearch List 내의 인자 수

	UINT 	uSequenceIndex      = 0;      // 시퀀스 Number
	UINT 	uCompSequenceIndex  = 0;      // 임시 시퀀스 Number
	UINT 	uFirstSequenceIndex  = 0;      // 임시 시퀀스 Number

  //	UINT	uCheckExtraNum		 = 0;

	int		nNodeIndex			= 0;		// Node의 Index
	int     nNextNodeIndex      = 0;        //NextNode의 Index
	int		nPathIndex			= 0;		// Path의 Index
	int		nSubPathIndex	 	= 0;		// Path의 Index

	PATH_DIVERGENCE_INFO     nTempDivInfo        = PATH_DIVERGENCE_NONE;         //분기정보


	UINT	uTmpToNode			= 0;		// Path 단위로 정보 확인 시
	UINT	uTmpFromNode		= 0;		// Path 단위로 정보 확인 시
//	UINT	uTmpNode			= 0;		// 임시 정보 확인 시

	double	dTmpTotalDistance	= 0.0;	// 현재 Node까지의 총 가중치(거리/속도)
	double	dTmpPathDistance	= 0.0;	// Path의 Link 가중치(거리/속도)
	double  dStopNodeMarGin     = 0.0;
	double  dTempMaxSpeed       = 0.0;  //통과허가를 위한 Max 속도

	//기본 분기 방향 // 중요한 부분. 0 : 왼쪽 1: 오른쪽
	int	bDefaultPathDirection = (int)CmdGoData->PathData_Def_Direction;
	bool	bCheckDefaultPathDirection = false;

	int		dtmpPathSearchListCnt = 0; //Pass Permit Limit 체크용

	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	NODE_INFO	*pNextNODE_INFO	= NULL;	// NextNode 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보
	PATH_INFO	*pSubPATH_INFO	= NULL;	// Path 정보를 이용해 통과허가 Sub정보를 획득

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search를 위한 Node 구조체
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search를 위한 Node 구조체


	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread에 전달하는 구조체
	EXECUTE_DRIVING_INFO	*pTempEXECUTE_DRIVING_INFO	= NULL;	// 선조향 관련된 내용
	PKT_CYCLECMDDATA  		*pCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst에 전달하는 구조체
//	PKT_CYCLECMDDATA		*pNextCYCLE_DRIVING_INFO= NULL; // CycleLIst에 전달하는 구조체
	PKT_CYCLECMDDATA  		*pPreCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst에 전달하는 구조체


	LList PathSearchList;									// PathSearch를 위한 List
//	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	// Node 정보를 찾기 위한 Map
//	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

	EXECUTE_DRIVING_ADD_INFO TempAdditionalInfo;

	memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
//	NODE_TYPE DrivingEndNodeType = NORMAL_POINT;
	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};
  //	PATH_STEERING_INFO		TempPreSteering = STEERING_NONE;		///< 선조향 정보 Default puting 20160119

	String StrTmp = "";

//    UINT									DivCount = 0;
//	UINT									TempDivCount = 0; 			 							//현재싸이클 내에서의 분기 갯수
//	CYCLEDRIVING_CHANGE_INFO	  			TempDivInfo[MAX_LIST_COUNT_FOR_CHANGE] ={0,};	 	   //현재싸이클 내에서의 분기 정보 (Max 10개 설정, 레일 기획측에 문의 필요.)
	bool bIsTargetStationoPassPermitOpen = false; //마지막 위치에 통과허가를 안받고 이동하는 경우


	int EndCount = 0;
	//------------- 1. 전처리 부분------------//
	// 1. 초기 설정 : ----------------------------------------------------------
	//   0) 시작 Node의 Path 확인
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
  //	||	(uFromNode == uToNode)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		m_bFindTheWay = false;
		return ERR_PATHSEARCH_FAIL;

	}

	ADD_LOG("[PATHSEARCH_Step 1 from%d To %d]",uFromNode,uToNode);

	////<(7)-1 Next Path 정보
	///Target이 Station인경우에 대한 전처리 부분
	if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
	{
		//이적재의 경우 부모노드가 들어온다.

		bIsTargetStationoPassPermitOpen =  CmdGoData->bIsTargetStationoPassPermitOpen;  //puting

		nNodeIndex = pTmpMap->FindNodeIndex(CmdGoData->PathData_EndNodeID);   ///< 출발 노드 Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

		ADD_LOG("[PATHSEARCH_End_Station_5] %d %d %d",CmdGoData->PathData_EndNodeID, CmdGoData->bIsTargetStationoPassPermitOpen,bIgnorOverrun);
		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
					if(pPATH_INFO->EndNID == AdditionalInfo.ProcessingGoInfo.EndNodeID)
					{
						AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode =  pPATH_INFO->Distance - AdditionalInfo.ProcessingGoInfo.StationData_EndRunDist;
                    }
				}
			}

			if(AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode <0)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
				ADD_LOG("[PATHSEARCH_End_Station_Fail] %d",AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
				ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
            }

		}
		else
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_53] %d:%d"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			m_bFindTheWay = false;
			return nTmpError;
		}
	}

	ADD_LOG("[PATHSEARCH_Step 2]");


	//------------- 2. 주요 처리부분------------//

	// 3) 시작 Node위치 List에 저장
	uTmpFromNode = uFromNode;
	uPathSearchIndex = 0;
	uSequenceIndex = 0;

	// 2. Index 순차적으로 증가시키면서 Node 추가 ------------------------------
	//   1) 다음 위치로 Index를 증가시켜가면서 다음 노드들을 PathSarchList에 추가함
	//     . PathSearchList에 추가 할 때는 거리 가중치 반영하여 가까운 순서로 삽입
	//     . 중복되는 Node 가 있을 경우 거리 가중치가 가까운 노드로 삽입, 먼 노드는 삭제
	//   2) 시작 노드에 연결 Path의 개수 만큼 시작노드가 발견되면 정지

	while(bArrived==false)	// 시작 Node 나올 경우 bArrived 가 true로 바꿔줌
	{
		nTempDivInfo        = PATH_DIVERGENCE_NONE;         //분기정보
		////< (1) first Node Check!!
		if(uPathSearchIndex !=0)
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex-1);
			////< (1)-1 현재 Node의 정보 확인
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			}
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보


			///< (1)-2 Node Info Check!!
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{

				///(3)-2 거리 정보 확인 필요
				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;

			}
			else
			{
				nTmpError = ERR_STD_INNER_PATH_FAIL; // PathSearch 실패
				//break;
			}

		}
		else
		{
			//uTmpFromNode = uTmpFromNode;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

			 dTmpTotalDistance = PathExtra;  //----------------------------초기 값 정보 입력해줄것.

		}

		////< (2) List의 Max Count Check!!     puting -----------------------------------------------------------추후삭제나 수정유무 판단.
		dtmpPathSearchListCnt = PathSearchList.getCount();
		if((dtmpPathSearchListCnt > m_pParamSet->nMapSearchMax) && (AdditionalInfo.FinalNode == 0))
		{
			nTmpError = ERR_PATHSEARCH_FAIL;
			ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dtmpPathSearchListCnt , m_pParamSet->nMapSearchMax);
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		}

		////< (4) 현재 노드가 아무런 PathIndex가 없을 경우 에러 처리 추가
		if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_54] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		   //	return nTmpError;
		}


		///<(6) Next Node(Path) 정보가 어떤 건지 확인하는 부분  puting 20151107
 		////< (6)-1 분기인 경우,
		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			  ////< (6-1) 기본외 번지인지 혹인하는 부분  puting 20151107
			if(CmdGoData->PathData_ExtraID[uCheckExtraNum]==pNODE_INFO->NID)
			{
				bCheckDefaultPathDirection = true;  //기본외 분기 일 경우
				uCheckExtraNum++;

				if(uCheckExtraNum > CmdGoData->PathData_ExtraNum)
				{
					nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
					ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				}
			}

			ADD_LOG("[ERR_PATHSEARCH_FAIL_54] %d %X:%X  :%d %d  NodeIndex:%d"  ,pNODE_INFO->NID
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1],bCheckDefaultPathDirection,bDefaultPathDirection , nNodeIndex);
			////< ( 6-2) 분기에 대한 정보를 저장함. 작업변경

//			TempDivInfo[DivCount].ChangeNodePoint = pNODE_INFO->NID;
//            TempDivInfo[DivCount].ChangeNodeType = 0x01;
//			TempDivInfo[DivCount].ChangeNodeDistance = dTmpTotalDistance;
//			DivCount++;

//			if(DivCount >m_pParamSet->nMapSearchMax)
//			{
//				ADD_LOG("[TempDivInfo] Max Over!!");
//				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
//            }
			/////< (6-3) 분기에서 가야될 Path 정보 확인

			////Map 부분 수정 필요.     확인 필요 조건이 반대일 경우 들어옴.
			for(int nTempIndex=0; nTempIndex<2; nTempIndex++)// 다음 Path 개수 만큼 확인
			{

				if(bCheckDefaultPathDirection ==true)
				{
					if(bDefaultPathDirection != nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //왼쪽 분기
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //오른쪽 분기

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
					}
					else
						nSubPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];  //통과허가 Sub부분을 위해 Check

				}
				else
				{
					if(bDefaultPathDirection == nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //왼쪽 분기
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //왼쪽 분기

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
//
//						pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
//
//						ADD_LOG("[bDefaultPathDirection %d][n : %d, Direction %d]", bDefaultPathDirection , nTempIndex, pPATH_INFO->PathDirection);
					}
					else
						nSubPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];  //통과허가 Sub부분을 위해 Check
				}


			}
             	 //이적재 이동시 분기일 경우, 상위에서 분기정보를 안줌. 비클에서 처리 20160422
			if((CmdGoData->PathData_EndNodeID == pNODE_INFO->NID) &&
				(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)&& ((uPathSearchIndex !=0) || (bNoonechek ==true)))     //AOHC-301
			{
				pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
				if(pPATH_INFO->EndNID != AdditionalInfo.ProcessingGoInfo.EndNodeID)
				{
					int Temp =0;
					Temp = nPathIndex;
					nPathIndex = nSubPathIndex;
					nSubPathIndex = Temp;

					if(nTempDivInfo == PATH_DIVERGENCE_LEFT)
						nTempDivInfo = PATH_DIVERGENCE_RIGHT;
					else
						nTempDivInfo = PATH_DIVERGENCE_LEFT;    //왼쪽 분기


					ADD_LOG("Station Target End %d %d %d",nPathIndex, nSubPathIndex,nTempDivInfo);
				}
			}
			else
			{
				ADD_LOG("StopNode Normal");
			}


			bCheckDefaultPathDirection = false;
		}

		////< (6-3) 분기가 아닌경우,
		else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
//			nTempDivInfo =  PATH_DIVERGENCE_NONE;
			for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
					nPathIndex = pNODE_INFO->NextPathIndex[n];
			}

		}

		///< (7) 연결된 Next Path(Node) 정보 확인 및 생성
		////<(7)-1 연결된 Path가 존재 하는 경우
		if(nPathIndex!=INDEX_NONE)
		{
			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
			if(nSubPathIndex !=0)
				pSubPATH_INFO = &(pTmpMap->m_PathArray[nSubPathIndex]);		// 연결된 Path 정보

			dTmpPathDistance = (double)pPATH_INFO->Distance;
			uTmpToNode = pPATH_INFO->EndNID; //연결된 다음 노드



		   //통과허가 정보 확인하는 부분
			int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_INFO->EndNID,pFlowMap);


			/// (5) 정지노드일 경우 sequenceIndex 증가하는 부분  puting------------------------------------------------------------------------------------------ MapData 수정 필요.7
			if((uPathSearchIndex==0)||
				(FlowControlIndex != INDEX_NONE))
//				(pNODE_INFO->TagType ==STOP_NTYPE)||(pNODE_INFO->TagType ==COMMON_NTYPE))
			 {
    			 if(uPathSearchIndex==0)
                 {
                	uSequenceIndex = GetIncreaseSeqenceNumber();
                 }
                 else
                 {
                    if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_INFO->EndNID) ||
					(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_INFO->EndNID))
                    {
                        uSequenceIndex = GetIncreaseSeqenceNumber();
                    }
                    else
                    {
        			 	uSequenceIndex = GetSeqenceNumber();
                    }
                 }

			   //	ADD_LOG("[STOP/COMMONSequenceIndexUpCheck][SequenceIndex:%d/NodeType:%d]", uSequenceIndex , pNODE_INFO->TagType);

			 }else
			 {
			 	uSequenceIndex = GetSeqenceNumber();
			   //	ADD_LOG("[SequenceIndexUpCheck][SequenceIndex:%d/NodeType:%d]", uSequenceIndex , pNODE_INFO->TagType);
			 }
			 if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)
			 {
				ADD_LOG("[PathSearch_Check][uTmpFromNode:%d/uTmpToNode:%d/uSequenceIndex%d : %d]", uTmpFromNode, uTmpToNode, uSequenceIndex, nPathIndex);
			 }

			 if(uPathSearchIndex==0)
				uFirstSequenceIndex  = uSequenceIndex;

			////<(7)-1 Next Path 정보
			uPathSearchIndex++;     //uPathSearchIndex 에 의하여 선조향 영향 주므로 순서 변경 불가


			if(uPathSearchIndex >MAX_PATHINDEX)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

			//가중치는 필요없음.

//			IterPos = mapPathSearchNode.find(uTmpToNode);
//			// mapPathSearchNode 에  uTmpFromNode 의 PathSearch Node정보가 없으면 정상처리.
//			if(IterPos == mapPathSearchNode.end())
//			{
				// PathSearch Node 정보 생성 : 없을 경우
				pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
				pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
				pPATH_SEARCH_NODE_INFO->IndexNum = uPathSearchIndex;

			   //	pPATH_SEARCH_NODE_INFO->PassPermit_NType = 
				pPATH_SEARCH_NODE_INFO->SQID =  uSequenceIndex;
				pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
				if(nSubPathIndex !=0)
					pPATH_SEARCH_NODE_INFO->SubNID = pSubPATH_INFO->EndNID;
				else
					pPATH_SEARCH_NODE_INFO->SubNID = 0;
				pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;

				pPATH_SEARCH_NODE_INFO->DivInfo = nTempDivInfo;

				pPATH_SEARCH_NODE_INFO->PreSteering = pPATH_INFO->Steering;     //선조향 관련된 정보 추가. puting 20160119

				if((uPathSearchIndex ==1)&& (GetPathCount()!=0))
				{
					pTempEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO *) ReferBackPath();
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreSteering  = pPATH_INFO->Steering;
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo =  nTempDivInfo;
				}

				///puting 추가해야되는 사항/ 초기거리와 Target에 따라 거리값 변경.
				//  Step 1.Target이 station일 경우 거리값 추가할것.
				pPATH_SEARCH_NODE_INFO->dTotalDistance = (double)dTmpPathDistance + (double)dTmpTotalDistance;
				pPATH_SEARCH_NODE_INFO->dPathDistance  = (double)dTmpPathDistance;
//				ADD_LOG("[PATHSEARCH_Stepll %06f]",pPATH_SEARCH_NODE_INFO->dTotalDistance);

				//추가하는 루틴
				PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
//				mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;

				///< (3)-1 마지막 노드 도달 확인

				//경로탐색일 경우만 카운트++
				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode && m_bFindTheWay)
				{
					EndCount++;
					ADD_LOG("FindTheWay: End Count - %d", EndCount);
				}

				//경로탐색일경우 m_EndNodeCnt 만큼 EndCount++ 일때 break
				//맵기반이동경우 m_EndNodeCnt =0, EndCount = 0, 도착노드 만나자마자 break
				//CmdGoData->PathData_EndNodeID ==pPATH_SEARCH_NODE_INFO->NID 의미 -> 타겟 부모노드 == 마지막 노드가 같은지 판단 (항상 고려 필요)
				//uPathSearchIndex !=1 는 경로탐색 초기 제외 위하여 추가   AOHC-301
				if((pPATH_SEARCH_NODE_INFO->NextNID == uToNode && m_EndNodeCnt == EndCount) &&
				   ((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==false) ||

				   ((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true) &&(((uPathSearchIndex !=1) || (bNoonechek ==true)) &&(CmdGoData->PathData_EndNodeID ==pPATH_SEARCH_NODE_INFO->NID)))))   //AOHC-301
				{
					m_EndNodeCnt = 0;
					EndCount = 0;

					ADD_LOG("[PATHSEARCH_Stepll %06f %d]",pPATH_SEARCH_NODE_INFO->dTotalDistance, AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
					if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
						pPATH_SEARCH_NODE_INFO->dTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance - AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode;


					bArrived = true;
					break;
				}

		}

		if(nTmpError!=NO_ERR) break;


		nPathIndex = 0;
		nSubPathIndex =0;


	 }  //while()


    ADD_LOG("[PATHSEARCH_Step 3]");
	///----------------3. 후처리 부분 ---------------------//
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) 에 추가
	try{
	bArrived		= false;
	uPathSearchIndex= 0;

	uTmpListCount	= PathSearchList.getCount();
	uTmpFromNode	= uFromNode;
	uCompSequenceIndex = 0;

	memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
	}catch(...)
	{
		m_EndNodeCnt = 0;
		m_bFindTheWay = false;
		ADD_LOG("[ERR_PATHSEARCH_Exception111111111]");
    }



	///<(2) 들어가있는 Path를 하나씩 빼서 후처리 작업
	if( nTmpError == NO_ERR)
	{

		for(int PathIndex =0; PathIndex < uTmpListCount; PathIndex++)
		{
			uPathSearchIndex++; //들어가있는 Path의 Pop Conut용!!

			if(nTmpError!=NO_ERR) break;

			try
			{
//			///< (2)-1 후처리 전 에러 확인 내용
			pPATH_SEARCH_NODE_INFO =(PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();

			if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
				(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false))
			{
				// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
				ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				delete pPATH_SEARCH_NODE_INFO;
				break;
			}
			}catch(...)
			   ADD_LOG("[ERR_PATHSEARCH_Exception22222222222]");

			//투입/빼냄싸이클이 아닐때 확인하는 부분
			if((CmdGoData->CmdCycle != CYCLE::CycleNotExist) && (CmdGoData->CmdCycle != CYCLE::TakeOut))
			{
				if((CheckEmergencyPathInfo(pTmpMap,pPATH_SEARCH_NODE_INFO->NID) ==true) ||
				   (CheckEmergencyPathInfo(pTmpMap,pPATH_SEARCH_NODE_INFO->NextNID) ==true))
				   {
						nTmpError = ERR_LAYOUT_PATH_LIMIT;
						break;
				   }
			}

			//(4)  EXECUTE_CYCLEDRIVING_INFO를  List 만듬 ---------------
			//    (시퀀스 ID가 변경이 일어나는 지점에서 확인 할 것. )
		 
			if((uCompSequenceIndex != pPATH_SEARCH_NODE_INFO->SQID) ||
				(uTmpListCount == uPathSearchIndex))  //Target Node 일 경우, 마지막 Ndoe
			{
				 //시퀀스 ID 정보 등록
				 uCompSequenceIndex = pPATH_SEARCH_NODE_INFO->SQID;

				 nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
				 pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

				 dStopNodeMarGin = (double)GetSpeedDownDist((double)VHLACCEL,(double)MAXSPEED(1500,pPATH_INFO->Speed)) + (double)GetMarginOfSpeedDist((double)MAXSPEED(1500,pPATH_INFO->Speed), (m_pParamSet->nPassPermitPeriod*m_pParamSet->nPassPermitCount));// + PathExtra;  //puting path Extra 제거 20160727

				 if(dStopNodeMarGin > m_pParamSet->dPassPermitMaxMargin) dStopNodeMarGin = m_pParamSet->dPassPermitMaxMargin;

				//(4)-1시작 위치일 경우
				if(PathExtra != 0.0)
				{
					 PathExtra = 0.0;
				}

				ADD_LOG("[dStopNodeMarGin] dStopNodeMarGin : %f, %f %f, VHLACCEL:%f,  dTempMaxSpeed:%f Speed %f, %d %d",
						dStopNodeMarGin,
						(double)GetSpeedDownDist((double)VHLACCEL,(double)MAXSPEED(1500,pPATH_INFO->Speed)),
						 (double)GetMarginOfSpeedDist((double)pPATH_INFO->Speed,(m_pParamSet->nPassPermitPeriod*m_pParamSet->nPassPermitCount)),
						 (double)VHLACCEL, (double)dTempMaxSpeed,(double)pPATH_INFO->Speed, m_pParamSet->nPassPermitPeriod, m_pParamSet->nPassPermitCount);

				if((uPathSearchIndex==FIRST_INDEX)&&(uTmpListCount != uPathSearchIndex))
				{


					try{
					TempAdditionalInfo.DistToStation = 0.0;
					TempAdditionalInfo.StationType = MARK_TYPE_NODE;
					TempAdditionalInfo.FinalPortType = MARK_TYPE_NODE;
					TempAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;


					uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

					pCYCLE_DRIVING_INFO    	= 	new PKT_CYCLECMDDATA;

					memset(pCYCLE_DRIVING_INFO, 0x00, sizeof(PKT_CYCLECMDDATA));

					// 1-1기본정보 등록
					pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
					pCYCLE_DRIVING_INFO->CmdID = CmdID;
					pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
					pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
					pCYCLE_DRIVING_INFO->IsListF2B  = false;
					pCYCLE_DRIVING_INFO->Isreturn = false;

					//출발 LINK에 대해 통과허가 요청 유무
					//정지 Node일 경우, 앞으로 이동할 통과허가 구간에 대해 통과허가 요청 할것.
					//excute info에 이동 정보를 넣어주는 기준으로 사용.



					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint					=  pPATH_SEARCH_NODE_INFO->NID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType           	 	=  pNODE_INFO->TagType;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	 	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance            		=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)dStopNodeMarGin;      //Stop 포인트 위치
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 				= CYCLENODETYPE_START2STOP; //Default로 설정
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum 	= (unsigned short) uCompSequenceIndex;

					if(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance < 0) pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = 0;

                	//분기정보 추가.  puting 20170312
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.Division = pPATH_SEARCH_NODE_INFO->DivInfo;

					// 2-3 작업변경을 위한 분기 정보
//					if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//					{
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodePPoint = TempDivInfo[TempDivCount].DivNodePPoint;
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodeDistance = TempDivInfo[TempDivCount].DivNodeDistance;
//
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivCount++;
////						ADD_LOG("[PassPermit_DIV_Point] SQnum %d StartND:%d,DivNodeDistance %f",uCompSequenceIndex,TempDivInfo[TempDivCount].caDivNodePoint,TempDivInfo[TempDivCount].DivNodeDistance);
//					}

					ADD_LOG("[PassPermit_Start_NotMatch_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);

					int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_SEARCH_NODE_INFO->NextNID,pFlowMap);

					if(FlowControlIndex != INDEX_NONE)
					{


						 if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID) ||
							(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_SEARCH_NODE_INFO->NextNID))
							{
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - m_pParamSet->dPassPermitOpenMargin;

								if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMainDefaultValue;
								else
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSubDefaultValue;

								ADD_LOG("[통과허가 정보] 시퀀스 Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d Start Distance%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);
								for(int i=0; i< 3; i++)
								{
									if (HIGH_BYTE == 0)
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
                                        }
									 }
									 else
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
                                        }

									 }
								}

							}
						 else
						 {
							if(pNODE_INFO->TagType == COMMON_NTYPE)
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
							else if(pNODE_INFO->TagType == STOP_NTYPE)
                                pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;
                         }

					}
					else
					{
                      	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = 0;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;

						if(pNODE_INFO->TagType == COMMON_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
						else if(pNODE_INFO->TagType == STOP_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;

                    }

					/*
					if((pNODE_INFO->TagType == STOP_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
					{
                      pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - pParamSet->dPassPermitOpenMargin;

					}
					else
					{
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = 0;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;
                    }

                    */
//					if((pNODE_INFO->TagType == STOP_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
//					{
//                    	ADD_LOG("[통과허가 정보] 시퀀스 Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d StartDistance:%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);

//						for(int i=0; i< 3; i++)
//						{
//							if (HIGH_BYTE == 0)
//							 {
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
//							 }
//							 else
//							 {
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
//							 }
//
//						}
//					}
					 /*
					if((pNODE_INFO->TagType == RESET_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
					{
						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }

						}
					}
                     */

					pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);
					}catch(...)
						ADD_LOG("[ERR_PATHSEARCH_Exception333333333]");

				}

				// (4)-1시작 위치일 경우, 패스가 하나밖에 없는 경우.
				else if((uPathSearchIndex==FIRST_INDEX)&&(uTmpListCount == uPathSearchIndex))
				{
//					if(dStopNodeMarGin < 0) dStopNodeMarGin = 0;
					memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));

					uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

					pCYCLE_DRIVING_INFO    					= new PKT_CYCLECMDDATA;

					memset(pCYCLE_DRIVING_INFO, 0x00,sizeof(PKT_CYCLECMDDATA));
					// 1-1기본정보 등록
					pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
					pCYCLE_DRIVING_INFO->CmdID = CmdID;
					pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
					pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
					pCYCLE_DRIVING_INFO->IsListF2B  = false;
					pCYCLE_DRIVING_INFO->Isreturn = false;

					//출발 LINK에 대해 통과허가 요청 유무
					//정지 Node일 경우, 앞으로 이동할 통과허가 구간에 대해 통과허가 요청 할것.
					//excute info에 이동 정보를 넣어주는 기준으로 사용.

					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint				=  pPATH_SEARCH_NODE_INFO->NID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType            =  pNODE_INFO->TagType;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode) - (double)dStopNodeMarGin;

					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 			= CYCLENODETYPE_START2TARGET; //Default로 설정
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum =  (unsigned short)uCompSequenceIndex;

   					//분기정보 추가.  puting 20170312
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.Division = pPATH_SEARCH_NODE_INFO->DivInfo;

					if(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance < 0) pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = 0;

				   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.SQNum =  0;

				   					// 2-3 작업변경을 위한 분기 정보
//					if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//					{
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodePPoint = TempDivInfo[TempDivCount].DivNodePPoint;
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodeDistance = TempDivInfo[TempDivCount].DivNodeDistance;
////
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivCount++;
////						ADD_LOG("[PassPermit_DIV_Point] SQnum %d StartND:%d,DivNodeDistance %f",uCompSequenceIndex,TempDivInfo[TempDivCount].DivNodePPoint,TempDivInfo[TempDivCount].DivNodeDistance);
//                    }
					ADD_LOG("[PassPermit_Start_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);

					//Reset 거리값 넣기. puting
					int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_SEARCH_NODE_INFO->NextNID,pFlowMap);

					if(FlowControlIndex != INDEX_NONE)
					{
						 if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID) ||
							(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_SEARCH_NODE_INFO->NextNID))
							{
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - m_pParamSet->dPassPermitOpenMargin;

								if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMainDefaultValue;
								else
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSubDefaultValue;

								ADD_LOG("[통과허가 정보] 시퀀스 Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d Start Distance%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);
								for(int i=0; i< 3; i++)
								{
									if (HIGH_BYTE == 0)
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										}
									 }
									 else
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										}

									 }
								}

							}
						 else
						 {
							if(pNODE_INFO->TagType == COMMON_NTYPE)
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
							else if(pNODE_INFO->TagType == STOP_NTYPE)
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;
                         }

					}
					else
					{
                      	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = 0;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;

						if(pNODE_INFO->TagType == COMMON_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
						else if(pNODE_INFO->TagType == STOP_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;

					}





				   /*
					if((pNODE_INFO->TagType == STOP_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
					{
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - pParamSet->dPassPermitOpenMargin;
					}
					else
					{
                       	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = 0;
                    	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;
                    }


					if((pNODE_INFO->TagType == STOP_NTYPE) ||(pNODE_INFO->TagType == COMMON_NTYPE))
					{
						ADD_LOG("[통과허가 정보] 시퀀스 Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d StartDistance:%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);

						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }

						}
					}
					*/
				   /*
					if((pNODE_INFO->TagType == RESET_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
					{
						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }

						}
					}
                    */
					uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index 획득
					pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint = pPATH_SEARCH_NODE_INFO->NextNID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;


					ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

					nTmpError = CheckPassPermit(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
												&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
												bIsTargetStationoPassPermitOpen);


					//이적재일 경우 예외처리.  puting 20160113
					if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
					{
						if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
							{
								if(bIgnorOverrun == true)
								{
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;

									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

									TempAdditionalInfo.bIgnolOverRun = true;
                                }
								else if(bIsTargetStationoPassPermitOpen ==true)
								{
									//safety Level 일 경우
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = true;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.TargetStationID = AdditionalInfo.StationID;
								}
								else
								{
									//일반적인 경우
                                	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

								}
							}

							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
					}

					pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);

					//마지막 위치가 확인되면 나가기
					if((pPATH_SEARCH_NODE_INFO->NextNID == uToNode) &&
						((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==false) ||
						((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true) &&(CmdGoData->PathData_EndNodeID ==pPATH_SEARCH_NODE_INFO->NID))))

					{
						bArrived = true;

					}
					else if(!m_bFindTheWay)
					{
						nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
						ADD_LOG("[ERR_PATHSEARCH_FAIL_8]");
						ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
									ERR_PATHSEARCH_FAIL,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
					}


				}
				//(4)-2 마지막 위치 일 경우.(패스가 하나 이상인 경우)
				else if((uPathSearchIndex!=FIRST_INDEX)&&(uTmpListCount == uPathSearchIndex))
				{
//					if(dStopNodeMarGin < 0) dStopNodeMarGin = 10;
					memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));

					//정보 획득
					pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();

                   	uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

					uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index 획득
					pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보


					int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_SEARCH_NODE_INFO->NextNID,pFlowMap);

					if(FlowControlIndex != INDEX_NONE)
					{
                      	///<Step1. 이전 통과허가 구간에 대한 정보 추가=================================================================================
//						pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();
                                               //다음노드가 Reset인 경우 통과허가 처리.
						if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID) ||
							(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_SEARCH_NODE_INFO->NextNID))
							{
                             	pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint   	=  pPATH_SEARCH_NODE_INFO->NID;
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType 	=  pNODE_INFO->TagType;
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance 	= (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;

											/// 선조향 관련하여 마지막 정보 변경
								pTempEXECUTE_DRIVING_INFO =(EXECUTE_DRIVING_INFO*)ReferBackPath();
								pTempEXECUTE_DRIVING_INFO->BasicInfo.PreSteering =  pPATH_SEARCH_NODE_INFO->PreSteering;
								pTempEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo =  pPATH_SEARCH_NODE_INFO->DivInfo;

								ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

								nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
															pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
															pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
															 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
															 bIsTargetStationoPassPermitOpen);

                                //다음 사이클 정보 등록
								pCYCLE_DRIVING_INFO    			   = new PKT_CYCLECMDDATA;

								memset(pCYCLE_DRIVING_INFO, 0x00,sizeof(PKT_CYCLECMDDATA));
								// 1-1기본정보 등록
								pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
								pCYCLE_DRIVING_INFO->CmdID = CmdID;
								pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
								pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
								pCYCLE_DRIVING_INFO->IsListF2B  = false;
								pCYCLE_DRIVING_INFO->Isreturn = false;

								//출발 LINK에 대해 통과허가 요청 유무
								//정지 Node일 경우, 앞으로 이동할 통과허가 구간에 대해 통과허가 요청 할것.
								//excute info에 이동 정보를 넣어주는 기준으로 사용.

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint				=  pPATH_SEARCH_NODE_INFO->NID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType            =  pNODE_INFO->TagType;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode) - (double)dStopNodeMarGin;

								//분기정보 추가.  puting 20170312
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.Division = pPATH_SEARCH_NODE_INFO->DivInfo;
									   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 			= CYCLENODETYPE_STOP2STOP; //Default로 설정
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum =  (unsigned short)uCompSequenceIndex;
							   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.SQNum =  0;

								ADD_LOG("[PassPermit_Start_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);

								//Reset 거리값 넣기. puting

								if(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance < 0) pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = 10;
								//통과허가 거리값 보정
								if((pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance > pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance)
								  && (pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance > pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance))
								  {
										double TempPreDist  = pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance -  pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance;
										double TempDist  = pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance -  pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance;

										if( TempDist > TempPreDist)
										{
											ADD_LOG("[PassPermit MOD] Pre %6.11f/%6.11f/%6.11f/%6.11f", pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance, pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance;
										}
								  }

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint		 =  pPATH_SEARCH_NODE_INFO->NextNID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - m_pParamSet->dPassPermitOpenMargin;
								if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMainDefaultValue;
								else
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSubDefaultValue;
								ADD_LOG("[통과허가 정보] 시퀀스 Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d StartDist:%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);

								for(int i=0; i< 3; i++)
								{
									if (HIGH_BYTE == 0)
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										}
									 }
									 else
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
                                        }

									 }
								}

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint=  pPATH_SEARCH_NODE_INFO->NextNID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;


								if(uCompSequenceIndex == uFirstSequenceIndex)
								{
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
								}
								else
								{
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
								}

								ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

								nTmpError = CheckPassPermit(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
															pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
															pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
															&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
															bIsTargetStationoPassPermitOpen);


								//이적재일 경우 예외처리.  puting 20160113
								if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
								{
									if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
										{
											if(bIgnorOverrun == true)
											{
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;

												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

                                            	TempAdditionalInfo.bIgnolOverRun = true;
											}
											else if(bIsTargetStationoPassPermitOpen ==true)
											{
												//safety Level 일 경우
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = true;
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.TargetStationID = AdditionalInfo.StationID;

											}else
											{
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

											}
										}

										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
								}




								pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);

							}else
							{
                                	pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;


									if(uCompSequenceIndex == uFirstSequenceIndex)
									{
										pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
									}
									else
									{
										pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
									}

									ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

									nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
															pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
															pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
															 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
															 bIsTargetStationoPassPermitOpen);

									//이적재일 경우 예외처리.  puting 20160113
									if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
									{
										if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
											{
												if(bIgnorOverrun == true)
												{
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;

													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

                                                	TempAdditionalInfo.bIgnolOverRun = true;
												}
												else if(bIsTargetStationoPassPermitOpen ==true)
												{
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = true;
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.TargetStationID = AdditionalInfo.StationID;


												}else
												{
                                                    pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;
												}
											}

											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
									}
                            }




					}else
					{
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;

		//					if(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance ==0.0)
		//						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - pParamSet->dPassPermitOpenMargin;
					 //	ADD_LOG("[PassPermit_End] EndND:%d, NType:%d", pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint, pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);


						if(uCompSequenceIndex == uFirstSequenceIndex)
						{
							pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
						}
						else
						{
							pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
						}

						ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

						nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
												 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
												 bIsTargetStationoPassPermitOpen);

						//이적재일 경우 예외처리.  puting 20160113
						if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
						{
							if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
								{
									if(bIgnorOverrun == true)
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;

										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

                                    	TempAdditionalInfo.bIgnolOverRun = true;

									}
									else if(bIsTargetStationoPassPermitOpen ==true)
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = true;
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.TargetStationID = AdditionalInfo.StationID;

									}else
									{
                                        pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;
									}
								}

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
						}
                    }


					/*
					if((pNODE_INFO->TagType == STOP_NTYPE) ||(pNODE_INFO->TagType == COMMON_NTYPE))
					{

						pCYCLE_DRIVING_INFO    			   = new PKT_CYCLECMDDATA;

						memset(pCYCLE_DRIVING_INFO, 0x00,sizeof(PKT_CYCLECMDDATA));
						// 1-1기본정보 등록
						pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
						pCYCLE_DRIVING_INFO->CmdID = CmdID;
						pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
						pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
						pCYCLE_DRIVING_INFO->IsListF2B  = false;
						pCYCLE_DRIVING_INFO->Isreturn = false;

						//출발 LINK에 대해 통과허가 요청 유무
						//정지 Node일 경우, 앞으로 이동할 통과허가 구간에 대해 통과허가 요청 할것.
						//excute info에 이동 정보를 넣어주는 기준으로 사용.

						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint				=  pPATH_SEARCH_NODE_INFO->NID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType            =  pNODE_INFO->TagType;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode) - (double)dStopNodeMarGin;

                       					// 2-3 작업변경을 위한 분기 정보
//						if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//						{
//
////							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodePPoint = TempDivInfo[TempDivCount].DivNodePPoint;
////							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodeDistance = TempDivInfo[TempDivCount].DivNodeDistance;
////
////							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivCount++;
////							ADD_LOG("[PassPermit_DIV_Point] SQnum %d StartND:%d,DivNodeDistance %f",uCompSequenceIndex,TempDivInfo[TempDivCount].DivNodePPoint,TempDivInfo[TempDivCount].DivNodeDistance);
//						}
					   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 			= CYCLENODETYPE_STOP2STOP; //Default로 설정
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum =  (unsigned short)uCompSequenceIndex;
					   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.SQNum =  0;

						ADD_LOG("[PassPermit_Start_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);

						//Reset 거리값 넣기. puting

						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint		 =  pPATH_SEARCH_NODE_INFO->NextNID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - pParamSet->dPassPermitOpenMargin;

						ADD_LOG("[통과허가 정보] 시퀀스 Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d StartDist:%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);

						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }

						}


						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint=  pPATH_SEARCH_NODE_INFO->NextNID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;


						if(uCompSequenceIndex == uFirstSequenceIndex)
						{
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
						}
						else
						{
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
						}

						ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

						nTmpError = CheckPassPermit(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
													&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info);


						//이적재일 경우 예외처리.  puting 20160113
						if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
						{
							if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
								{
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;
								}

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
						}


						pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);


					}
					else
					{

						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;

		//					if(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance ==0.0)
		//						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - pParamSet->dPassPermitOpenMargin;
					 //	ADD_LOG("[PassPermit_End] EndND:%d, NType:%d", pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint, pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);


						if(uCompSequenceIndex == uFirstSequenceIndex)
						{
							pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
						}
						else
						{
							pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
						}

						ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

						nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
												 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info);

						//이적재일 경우 예외처리.  puting 20160113
						if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
						{
							if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
								{
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;
								}

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
						}

					}



                     */
					//마지막지점에서  통과허가에 대하 개방을 해야될지 판단하는 부분 추가
					//상위쪽에서 마지막 통과허가 요청한 내용을 기억후, 통과허가 개방이 필요할 경우, 전송함.


					//마지막 위치가 확인되면 나가기


					if((pPATH_SEARCH_NODE_INFO->NextNID == uToNode) &&
						((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==false) ||
						((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true) &&(CmdGoData->PathData_EndNodeID ==pPATH_SEARCH_NODE_INFO->NID))))

					{
						bArrived = true;
					
					}
					else if (!m_bFindTheWay)
					{
						nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
						ADD_LOG("[ERR_PATHSEARCH_FAIL_8]");
						ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
									ERR_PATHSEARCH_FAIL,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
					}
				
				}
				//(4)-3중간 지점 통과할 경우
				else
				{
//					if(dStopNodeMarGin < 0) dStopNodeMarGin = 10;

					TempAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;

					uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

					///<Step1. 이전 통과허가 구간에 대한 정보 추가=================================================================================
					pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();

					/// 선조향 관련하여 마지막 정보 변경
              		pTempEXECUTE_DRIVING_INFO =(EXECUTE_DRIVING_INFO*)ReferBackPath();
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreSteering =  pPATH_SEARCH_NODE_INFO->PreSteering;
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo =  pPATH_SEARCH_NODE_INFO->DivInfo;


					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType 	=  pNODE_INFO->TagType;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance 	= (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;


					ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

					nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
												 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info);

				

					///<Step2. 다음 통과허가 구간에 대한 정보 생성=================================================================================
					pCYCLE_DRIVING_INFO    						= new PKT_CYCLECMDDATA;
					memset(pCYCLE_DRIVING_INFO, 0x00,sizeof(PKT_CYCLECMDDATA));
					// 2-1기본정보 등록
					pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
					pCYCLE_DRIVING_INFO->CmdID = CmdID;
					pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
					pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
					pCYCLE_DRIVING_INFO->IsListF2B  = false;
					pCYCLE_DRIVING_INFO->Isreturn = false;

					// 2-2 주행관련 정보 등록
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint				=  pPATH_SEARCH_NODE_INFO->NID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType 		  	=  pNODE_INFO->TagType;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance 		  	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance 		  	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)dStopNodeMarGin;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 			=  CYCLENODETYPE_STOP2STOP; //Default로 설정

					//분기정보 추가.  puting 20170312
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.Division = pPATH_SEARCH_NODE_INFO->DivInfo;

					if(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance < 0) pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = 10;
					// 2-3 작업변경을 위한 분기 정보
//					if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//					{
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodePPoint = TempDivInfo[TempDivCount].DivNodePPoint;
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodeDistance = TempDivInfo[TempDivCount].DivNodeDistance;
////
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivCount++;
////						ADD_LOG("[PassPermit_DIV_Point] SQnum %d StartND:%d,DivNodeDistance %f",uCompSequenceIndex,TempDivInfo[TempDivCount].DivNodePPoint,TempDivInfo[TempDivCount].DivNodeDistance);
//                    }
				 //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
                    //선조향을 위한 부분 추가.


					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum =  (unsigned short)uCompSequenceIndex;
				   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.SQNum =  0;

					ADD_LOG("[PassPermit_Mid] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

					//Reset NTYpe은 없음 puting
	//				if((pNODE_INFO->TagType == RESET_NTYPE) ||(pNODE_INFO->TagType == COMMON_NTYPE))
	//				{
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - m_pParamSet->dPassPermitOpenMargin;

	//				}
	//				else if(pNODE_INFO->TagType == STOP_NTYPE)
	//				{
	//					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;
	//				}


					//통과허가 거리값 보정
					if((pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance > pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance)
					  && (pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance > pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance))
					  {
							double TempPreDist  = pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance -  pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance;
							double TempDist  = pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance -  pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance;

							if( TempDist > TempPreDist)
							{
								ADD_LOG("[PassPermit MOD] Pre %6.11f/%6.11f/%6.11f/%6.11f", pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance, pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance;
							}
					  }


					int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_SEARCH_NODE_INFO->NextNID,pFlowMap);

					if(FlowControlIndex != INDEX_NONE)
					{
						 if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID) ||
							(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_SEARCH_NODE_INFO->NextNID))
							{
								if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMainDefaultValue;
								else
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSubDefaultValue;

								ADD_LOG("[통과허가 정보] 시퀀스 Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d Start Distance%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);
								for(int i=0; i< 3; i++)
								{
									if (HIGH_BYTE == 0)
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										}
									 }
									 else
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										}

									 }
								}

							}
						 else
						 {
							ADD_LOG("예외상황발생 Path 통과허가");
						 }

					}




					 /*
					if((pNODE_INFO->TagType == STOP_NTYPE) ||(pNODE_INFO->TagType == COMMON_NTYPE))
					{

						ADD_LOG("[통과허가 정보] 시퀀스 Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d, StartDist:%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);

						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }
						}
					}
                     */
					pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);
					
				}


                dTempMaxSpeed = 0.0;  //Max Speed 초기화
			}else
			{
				try{

					// 2-3 작업변경을 위한 분기 정보
					TempAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;

					pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();
					nNodeIndex = pTmpMap->FindNodeIndex(pPATH_SEARCH_NODE_INFO->NID);	// Node Index 획득
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

//					if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//					{
////						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodePPoint = TempDivInfo[TempDivCount].DivNodePPoint;
////						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodeDistance = TempDivInfo[TempDivCount].DivNodeDistance;
////						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivCount++;
////						ADD_LOG("[PassPermit_DIV_Point] SQnum %d StartND:%d,DivNodeDistance %f",uCompSequenceIndex,TempDivInfo[TempDivCount].DivNodePPoint,TempDivInfo[TempDivCount].DivNodeDistance);
//					}

					/// 선조향 관련하여 마지막 정보 변경
              		pTempEXECUTE_DRIVING_INFO =(EXECUTE_DRIVING_INFO*)ReferBackPath();
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreSteering =  pPATH_SEARCH_NODE_INFO->PreSteering;
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo =  pPATH_SEARCH_NODE_INFO->DivInfo;

					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType 	=  pNODE_INFO->TagType;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance 	= (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;
                    	}catch(...)
						 ADD_LOG("[ERR_PATHSEARCH_Exception77777777]");

            }

			//MAP MAKER 부분 추후에 생각하기 PUTING
			/*
			// MapMake 일 경우 Map Make 속도 반영 : 위치 및 처리 방법에 대해 추가 고민 필요
			if(	(AdditionalInfo.CmdType==EXECUTE_DRIVING_CMD_MAPMAKE) &&
				(pCYCLECMDDATA_INFO->EXE_Driving_Info.BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
			{
				pCYCLECMDDATA_INFO->EXE_Driving_Info.BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
			}

			Route_ADD_LOG("[PathSearch] Node:%X~%X",pEXECUTE_DRIVING_INFO->StartNID, pEXECUTE_DRIVING_INFO->EndNID);
			///< 결과값을 넣어줌
			pResultAddList->insertBack((void*)pCYCLECMDDATA_INFO);
			*/

			//처음부터 하나씩 Path 정보를 뺌.
			nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		   //	ADD_LOG("[ExcuteDrivingInfo: %d", nPathIndex);
			// (3) EXECUTE_DRIVING_INFO를 PathSearcing List 만듬 ---------------
			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

			uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index 획득
			pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보


			//Excute Info에 저장될 Data
			pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
			memset(pEXECUTE_DRIVING_INFO, 0x00,sizeof(EXECUTE_DRIVING_INFO));

			pEXECUTE_DRIVING_INFO->PathIndexNum				= pPATH_SEARCH_NODE_INFO->IndexNum;
			pEXECUTE_DRIVING_INFO->CmdID					= CmdID;
			pEXECUTE_DRIVING_INFO->SQNum					= pPATH_SEARCH_NODE_INFO->SQID;
			pEXECUTE_DRIVING_INFO->ChangeEndNodeDistance    = pPATH_SEARCH_NODE_INFO->dTotalDistance;
            pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= uTmpFromNode;
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= pPATH_SEARCH_NODE_INFO->NextNID;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartType		= pNODE_INFO->TagType;
//			pEXECUTE_DRIVING_INFO->BasicInfo.DeviceType     = pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NodeType      	= pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NextNodeType   = pNextNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo        = pPATH_SEARCH_NODE_INFO->DivInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
			pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
			pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
			pEXECUTE_DRIVING_INFO->BasicInfo.bChargeEnable	= pPATH_INFO->ChargeEnable;   //eco test 
			//임시
//			if(pPATH_INFO->PathDirection == PATH_DIRECTION_SHORT)
//				 pPATH_INFO->PathDirection  = PATH_DIRECTION_SPECIAL_R;
#if(USE_CHANGENODE == 1)
			pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;

			// for change node (jhoun.lim)
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;

#else
			pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
#endif
            		//Map Maker 관련 속도
			if((AdditionalInfo.CmdType == EXECUTE_DRIVING_CMD_MAPMAKE)  &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
			{
				pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
				ADD_LOG("[Map Maker PathSearch...]");

			}else if((bIsFoup ==false) && (m_pParamSet->nIsCurveSpeedUp == true) &&( bUseAVS == false))
			{
#if(USE_CHANGENODE == 1)
				int nTempFoupCoverDectect = pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion;
#else
				int nTempFoupCoverDectect = pEXECUTE_DRIVING_INFO->BasicInfo.Direction & 0x0F;
#endif
				//N분기, 직선이 아닐경우에 주행속도 변경 : 기존대비 -300감소
				if((nTempFoupCoverDectect ==PATH_DIRECTION_LEFT) || (nTempFoupCoverDectect ==PATH_DIRECTION_RIGHT))
				{

					if((pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed	== 0.75) && (pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed < (double)(m_pParamSet->nIsShoutCurveSpeedUpValue/1000.0)))//Short Curve
						pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(m_pParamSet->nIsShoutCurveSpeedUpValue/1000.0);			// mm/s -> mm/msec
					else if((pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed	== 1.0)&& (pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed < (double)(m_pParamSet->nIsCurveSpeedUpValue/1000.0))) // Curve
						pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(m_pParamSet->nIsCurveSpeedUpValue/1000.0);			// mm/s -> mm/msec

					ADD_LOG("[IsFoup :Speed Down %f]",pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed);
				}

			}

			memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &TempAdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));

			AddPath(pEXECUTE_DRIVING_INFO);


			if((pEXECUTE_DRIVING_INFO->BasicInfo.NodeType == LIFTIN_NTYPE_LEFT) || (pEXECUTE_DRIVING_INFO->BasicInfo.NodeType == LIFTIN_NTYPE_RIGHT) || (pEXECUTE_DRIVING_INFO->BasicInfo.NodeType == LIFTOUT_NTYPE))
				dTempMaxSpeed = pPATH_INFO->Speed;
			else
				dTempMaxSpeed = MAXSPEED(pPATH_INFO->Speed,dTempMaxSpeed); // mm/s

			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;

			//작업변경으로 인한 확인 부분
//			if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//            	TempDivCount++;

			delete pPATH_SEARCH_NODE_INFO;
		}
	}
	ADD_LOG("[PATHSEARCH_Step 4]");
	m_bFindTheWay = false;
	m_EndNodeCnt = 0;
   //	if(uTmpFromNode !=  uTmpFromNode)  nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
	///< Error 발생시 모든 List 삭제 하는 부분
	if(nTmpError != NO_ERR)
	{
		//1. Path List 정보 삭제
		InitPathList();

		//2. Cycle Lis 정보 삭제
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->popFront();
			delete pCYCLE_DRIVING_INFO;
		}
	}

	// 4. 임시 Path Search List 소멸 ------------------------------------------------
	//   1) 인자들 delete
	//   2) List 소멸
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}

	ChangePathVelocity();

	return nTmpError;
}


/**
@brief   동일한 부모노드에서 이적재 노드에서 또는 이적재 노드로 이동할 경우 Tag Map Search 함수
@author  puting
@date    2015-12-04
@param   pMap : 사용하는 Map
@param   FromNode : 시작 Node
@param   ToNode : Target Node
@param   AdditionalInfo : 주행 명령 추가 정보
@param   DRIVING_INFO_List : 주행 명령 추가 List
@return  Error Code
*/
//AOHC-195 오버런 개선 작업(유형2)  ->  bIgnorOverrun 추
int PathSearch::StandardSearchOnlyOnePassPermit(MAP *pMap, STD_FlowControl *pFlowMap, PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List, bool bcheckTarget, PKT_CYCLECMDDATA *FinalCycleData, UINT &uCheckExtraNum, double &dTempDis, UINT FinalSeqNum,PASSPERMIT_COMMON TempPassPermitData,bool bIsFoup, bool bIgnorOverrun,bool bturnoff)
{
	// 0. 값 초기화 ------------------------------------------------------------
	int nTmpError				= NO_ERR;				// 현재 에러 초기화

	UINT	CmdID				= CmdGoData->CmdID; 	//지시 ID
	MAP		*pTmpMap			= pMap;					// 사용하는 Map

	//경로탐색을 해야될 시작과 끝 위치
	UINT	uFromNode			= AdditionalInfo.ProcessingGoInfo.StartNodeID;			// 시작 노드
	UINT	uToNode				= AdditionalInfo.ProcessingGoInfo.EndNodeID;			// 끝 노드
	bool	bPassPermit			= false;	// Map Seach에 성공한 경우

	LList	*pResultAddList		= DRIVING_INFO_List;	// 주행을 위한 명령을 추가하기 위한 Cycle List

	bool	bArrived			= false;	// Map Seach에 성공한 경우

	UINT	uPathSearchIndex	= 0;		// PathSearch List의 Index
//	UINT	uListInsertIndex	= 0;		// PathSearch List 삽입시 사용되는 Index
	UINT	uTmpListCount		= 0;		// PathSearch List 내의 인자 수

	UINT 	uSequenceIndex      = 0;      // 시퀀스 Number
	UINT 	uCompSequenceIndex  = 0;      // 임시 시퀀스 Number

 //	UINT	uCheckExtraNum		 = 0;

	int		nNodeIndex			= 0;		// Node의 Index
	int		nPathIndex			= 0;		// Path의 Index
	int		nSubPathIndex	  	= 0;		// Path의 통과허가를 위한 Index

	PATH_DIVERGENCE_INFO     nTempDivInfo         = PATH_DIVERGENCE_NONE;

	UINT	uTmpToNode			= 0;		// Path 단위로 정보 확인 시
	UINT	uTmpFromNode		= 0;		// Path 단위로 정보 확인 시
//	UINT	uTmpNode			= 0;		// 임시 정보 확인 시

	double	dTmpPathDistance	= 0.0;	// Path의 Link 가중치(거리/속도)
	double  dStopNodeMarGin     = 0.0;
	double  dTmpBetweenDistance = 0.0;

	//기본 분기 방향 // 중요한 부분. 0 : 왼쪽 1: 오른쪽
	int	bDefaultPathDirection = (int)CmdGoData->PathData_Def_Direction;
	bool	bCheckDefaultPathDirection = false;

//	int		dtmpPathSearchListCnt = 0; //Pass Permit Limit 체크용

	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	NODE_INFO	*pNextNODE_INFO	= NULL;	// NextNode 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보
	PATH_INFO	*pSubPATH_INFO	= NULL;	// Path 정보를 이용해 통과허가 Sub정보를 획득

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search를 위한 Node 구조체
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search를 위한 Node 구조체


	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread에 전달하는 구조체
	PKT_CYCLECMDDATA  		*pCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst에 전달하는 구조체
//	PKT_CYCLECMDDATA		*pNextCYCLE_DRIVING_INFO= NULL; // CycleLIst에 전달하는 구조체
//	PKT_CYCLECMDDATA  		*pPreCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst에 전달하는 구조체


	LList PathSearchList;									// PathSearch를 위한 List
//	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	// Node 정보를 찾기 위한 Map
//	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

//	NODE_TYPE DrivingEndNodeType = NORMAL_POINT;
	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";

	bool bIsStartStationPassPermit = false;     //출발 위치에 통과허가를 받고 이동하는 경우
	bool bIsTargetStationoPassPermitOpen = false; //마지막 위치에 통과허가를 안받고 이동하는 경우


//	OHTMainStatus *pOHTMainStatusTmp = OHTMainForm->GetOHTStatus();

	//------------- 1. 전처리 부분------------//

	// 1. 초기 설정 : ----------------------------------------------------------
	//   0) 시작 Node의 Path 확인
//	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
//	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
//	||	(uFromNode == uToNode)
//	)
//	{
//
//		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
//			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
//		return ERR_PATHSEARCH_FAIL;
//	}

	//  1) Station의 Path 확인   //용도 ?  puting
	/*
	if(AdditionalInfo.StationType!=MARK_TYPE_NODE)
	{
		nTmpError = CheckPathOfStation(pTmpMap, uFromNode, uToNode, AdditionalInfo);
		if(nTmpError!=NO_ERR)
		{
			return nTmpError;
		}
	}
	*/

	///시작이 Station인경우에 대한 전처리 부분
	if(AdditionalInfo.ProcessingGoInfo.IsStationStart ==true)
	{
		//이적재의 경우 부모노드가 들어온다.
		ADD_LOG("[PATHSEARCH_Start_Station_5] %d",CmdGoData->PathData_StartNodeID);
		nNodeIndex = pTmpMap->FindNodeIndex(CmdGoData->PathData_StartNodeID);   ///< 출발 노드 Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
					if(pPATH_INFO->EndNID == AdditionalInfo.ProcessingGoInfo.EndNodeID)
					{
					   // bDefaultPathDirection = n;
						//AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode =  pPATH_INFO->Distance - AdditionalInfo.ProcessingGoInfo.StationData_StartRunDist;
					}

					if(AdditionalInfo.ProcessingGoInfo.PathDiv !=0x00)
					{
						if(((AdditionalInfo.ProcessingGoInfo.PathDiv ==0x01) && (n==0)) ||
						   ((AdditionalInfo.ProcessingGoInfo.PathDiv ==0x02) && (n==1)))
						   {
						   	   //	bDefaultPathDirection = n;
								if(AdditionalInfo.ProcessingGoInfo.EndNodeID==0)
									AdditionalInfo.ProcessingGoInfo.EndNodeID = pPATH_INFO->EndNID;
								  //	TempEndID  =  pPATH_INFO->EndNID;
									uToNode =  pPATH_INFO->EndNID;
						   }
						else if(AdditionalInfo.ProcessingGoInfo.PathDiv ==0x03)
						{
							if(AdditionalInfo.ProcessingGoInfo.EndNodeID==0)
									AdditionalInfo.ProcessingGoInfo.EndNodeID = pPATH_INFO->EndNID;
                           //	TempEndID  =  pPATH_INFO->EndNID;
							uToNode =  pPATH_INFO->EndNID;
                        }

                    }
				}
			}

			if(AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode < 0)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
				ADD_LOG("[PATHSEARCH_Start_Station_Fail] %d",AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode);
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

		}
		else
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_55] %d:%d"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			return nTmpError;
		}
	}

	////<(7)-1 Next Path 정보
	///Target이 Station인경우에 대한 전처리 부분
	if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
	{
		//이적재의 경우 부모노드가 들어온다.

		nNodeIndex = pTmpMap->FindNodeIndex(CmdGoData->PathData_EndNodeID);   ///< 출발 노드 Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

		ADD_LOG("[PATHSEARCH_End_Station_5] %d",CmdGoData->PathData_EndNodeID);
		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
					if(pPATH_INFO->EndNID == AdditionalInfo.ProcessingGoInfo.EndNodeID)
					{
						AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode =  pPATH_INFO->Distance - AdditionalInfo.ProcessingGoInfo.StationData_EndRunDist;
					}
				}
			}

			if(AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode <0)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
				ADD_LOG("[PATHSEARCH_End_Station_Fail] %d",AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

		}
		else
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_56] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			return nTmpError;
		}
	}



	if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==false) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true) ||
	   (AdditionalInfo.ProcessingGoInfo.IsStationStart ==true) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==false))
	 {
		bPassPermit = true;     //통과허가관련 처리 필요.

	 }
	 else if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==true) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true))
	 {
     	dTmpBetweenDistance =  AdditionalInfo.ProcessingGoInfo.StationData_EndRunDist - AdditionalInfo.ProcessingGoInfo.StationData_StartRunDist;
	 }


     //puting 정지노드 이적재 Tag에 대한 추가부분 20170803
      if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==false) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true) ||
	   (AdditionalInfo.ProcessingGoInfo.IsStationStart ==true) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true))
	   {
			bIsTargetStationoPassPermitOpen =  CmdGoData->bIsTargetStationoPassPermitOpen;
       }
	//------------- 2. 주요 처리부분------------//

	// 3) 시작 Node위치 List에 저장
	uPathSearchIndex = 0;
    uSequenceIndex = 0;

	// 2. Index 순차적으로 증가시키면서 Node 추가 ------------------------------
	//   1) 다음 위치로 Index를 증가시켜가면서 다음 노드들을 PathSarchList에 추가함
	//     . PathSearchList에 추가 할 때는 거리 가중치 반영하여 가까운 순서로 삽입
	//     . 중복되는 Node 가 있을 경우 거리 가중치가 가까운 노드로 삽입, 먼 노드는 삭제
	//   2) 시작 노드에 연결 Path의 개수 만큼 시작노드가 발견되면 정지


	////< (1) first Node Check!!
 	uTmpFromNode = uFromNode;
	nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보


	////< (4) 현재 노드가 아무런 PathIndex가 없을 경우 에러 처리 추가
	if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
	{
		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
		ADD_LOG("[ERR_PATHSEARCH_FAIL_57] %X:%X"
		, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
		ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return nTmpError;
	}

//	if((pNODE_INFO->TagType ==STOP_NTYPE)||(pNODE_INFO->TagType ==COMMON_NTYPE))


	///<(6) Next Node(Path) 정보가 어떤 건지 확인하는 부분  puting 20151107
	////< (6)-1 분기인 경우,
	if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
	{
		  ////< (6-1) 기본외 번지인지 혹인하는 부분  puting 20151107
		if(CmdGoData->PathData_ExtraID[uCheckExtraNum]==pNODE_INFO->NID)
		{
			bCheckDefaultPathDirection = true;  //기본외 분기 일 경우
			uCheckExtraNum++;

			ADD_LOG("[분기외번지 %d %d DefDir %d]", CmdGoData->PathData_ExtraID[uCheckExtraNum] ,pNODE_INFO->NID,bDefaultPathDirection);

			if(uCheckExtraNum > CmdGoData->PathData_ExtraNum)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}
		}
		/////< (6-2) 분기에서 가야될 Path 정보 확인

		////Map 부분 수정 필요.
		for(int nTempIndex=0; nTempIndex<2; nTempIndex++)// 다음 Path 개수 만큼 확인
			{

				if(bCheckDefaultPathDirection ==true)
				{
					if(bDefaultPathDirection != nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //왼쪽 분기
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //오른쪽 분기

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
					}
					else
						nSubPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];  //통과허가 Sub부분을 위해 Check

				}
				else
				{
					if(bDefaultPathDirection == nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //왼쪽 분기
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //왼쪽 분기

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
//
//						pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
//
//						ADD_LOG("[bDefaultPathDirection %d][n : %d, Direction %d]", bDefaultPathDirection , nTempIndex, pPATH_INFO->PathDirection);
					}
					else
						nSubPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];  //통과허가 Sub부분을 위해 Check
				}


			}
             	 //이적재 이동시 분기일 경우, 상위에서 분기정보를 안줌. 비클에서 처리 20160422
			if((CmdGoData->PathData_EndNodeID == pNODE_INFO->NID) &&
				(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true))
			{
				pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
				if(pPATH_INFO->EndNID != AdditionalInfo.ProcessingGoInfo.EndNodeID)
				{
					int Temp =0;
					Temp = nPathIndex;
					nPathIndex = nSubPathIndex;
					nSubPathIndex = Temp;

					if(nTempDivInfo == PATH_DIVERGENCE_LEFT)
						nTempDivInfo = PATH_DIVERGENCE_RIGHT;
					else
						nTempDivInfo = PATH_DIVERGENCE_LEFT;    //왼쪽 분기

					ADD_LOG("Station Target End %d %d %d ",nPathIndex, nSubPathIndex,nTempDivInfo);
				}
			}
			//이적재 이동시 분기일 경우, 상위에서 분기정보를 안줌. 상위에서 처리해야됨
//			else if((CmdGoData->PathData_StartNodeID == pNODE_INFO->NID) &&
//				(AdditionalInfo.ProcessingGoInfo.IsStationStart ==true))
//			{
//				pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
//				if(pPATH_INFO->EndNID != AdditionalInfo.ProcessingGoInfo.StartNodeID)
//				{
//					int Temp =0;
//					Temp = nPathIndex;
//					nPathIndex = nSubPathIndex;
//					nSubPathIndex = Temp;
//
//					ADD_LOG("Station Target Start");
//				}
//			}

//		bCheckDefaultPathDirection = false;
	}

	////< (6-3) 분기가 아닌경우,
	else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
	{
//		nTempDivInfo =  PATH_DIVERGENCE_NONE;
		for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
		{
			if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				nPathIndex = pNODE_INFO->NextPathIndex[n];
		}

	}

	///< (7) 연결된 Next Path(Node) 정보 확인 및 생성
	////<(7)-1 연결된 Path가 존재 하는 경우
	if(nPathIndex!=-1)
	{

		////<(7)-1 Next Path 정보
		pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
		if(nSubPathIndex !=0)
			pSubPATH_INFO = &(pTmpMap->m_PathArray[nSubPathIndex]);		// 연결된 Path 정보

		//가중치는 필요없음.
		dTmpPathDistance = (double)pPATH_INFO->Distance;
		uTmpToNode = pPATH_INFO->EndNID; //연결된 다음 노드

		int FlowControlIndex =pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_INFO->EndNID,pFlowMap);

		if(FinalSeqNum !=0)
		   uSequenceIndex =  FinalSeqNum;
		else
		{
			if(FlowControlIndex != INDEX_NONE)
			{
				if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_INFO->EndNID) ||
					(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_INFO->EndNID))
					{
                    	uSequenceIndex = GetSeqenceNumber();//7102;
					}
					else
						uSequenceIndex = GetIncreaseSeqenceNumber();//7102;

			}else
				uSequenceIndex = GetIncreaseSeqenceNumber();//7102;

		}

		// PathSearch Node 정보 생성 : 없을 경우
		pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
		pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
		pPATH_SEARCH_NODE_INFO->IndexNum = uPathSearchIndex;

	   //	pPATH_SEARCH_NODE_INFO->PassPermit_NType =
		pPATH_SEARCH_NODE_INFO->SQID =  uSequenceIndex;
		pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
		if(nSubPathIndex !=0)
			pPATH_SEARCH_NODE_INFO->SubNID = pSubPATH_INFO->EndNID;
		else
			pPATH_SEARCH_NODE_INFO->SubNID = 0;
		pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;

		pPATH_SEARCH_NODE_INFO->DivInfo = nTempDivInfo;

		ADD_LOG("[PathSearch_Check][uTmpFromNode:%d/uTmpToNode:%d/uSequenceIndex%d]", uTmpFromNode, uTmpToNode, uSequenceIndex);
		///puting 추가해야되는 사항/ 초기거리와 Target에 따라 거리값 변경.
		//  Step 1.Target이 station일 경우 거리값 추가할것.
		if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==true) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true))
		{
			 pPATH_SEARCH_NODE_INFO->dPathDistance = (double)dTmpBetweenDistance;
			 pPATH_SEARCH_NODE_INFO->dTotalDistance =  pPATH_SEARCH_NODE_INFO->dPathDistance  + dTempDis;
		}
		else if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==false) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true))
		{
			if(bcheckTarget ==false)
			{
				pPATH_SEARCH_NODE_INFO->dPathDistance = (double)dTmpPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode;
				pPATH_SEARCH_NODE_INFO->dTotalDistance =  pPATH_SEARCH_NODE_INFO->dPathDistance  + dTempDis;
			}
			else
			{
				ADD_LOG("[PATHSEARCH_Stepll %06f %06f %d %d]",pPATH_SEARCH_NODE_INFO->dTotalDistance, pPATH_SEARCH_NODE_INFO->dPathDistance,FinalCycleData->EXE_CycleDriving_Info.EndDistance, dTempDis);
				pPATH_SEARCH_NODE_INFO->dPathDistance = (double)dTmpPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode;
				pPATH_SEARCH_NODE_INFO->dTotalDistance =  (double)pPATH_SEARCH_NODE_INFO->dPathDistance  + (double)FinalCycleData->EXE_CycleDriving_Info.EndDistance  + dTempDis;
			}

		}
		else if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==true) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==false))
		{
			pPATH_SEARCH_NODE_INFO->dPathDistance = (double)dTmpPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_StartRunDist;
			pPATH_SEARCH_NODE_INFO->dTotalDistance =  pPATH_SEARCH_NODE_INFO->dPathDistance  + dTempDis;
		}

		ADD_LOG("[PathSearch_Check][dTotalDistance:%f/IsStationStart:%d/IsStationEnd%d]", pPATH_SEARCH_NODE_INFO->dTotalDistance, AdditionalInfo.ProcessingGoInfo.IsStationStart, AdditionalInfo.ProcessingGoInfo.IsStationEnd);
		//추가하는 루틴
		PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
		//mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;

		if(pPATH_SEARCH_NODE_INFO->NextNID != uToNode)
		{
			ADD_LOG("[PathSearch_Check][Same Node]");
			nTmpError = ERR_PATHSEARCH_FAIL;
			ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		}


	}	// if(nPathIndex!=-1)





	///----------------3. 후처리 부분 ---------------------//
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) 에 추가
	bArrived		= false;
	uPathSearchIndex= 0;
	uTmpListCount	= PathSearchList.getCount();
	uTmpFromNode	= uFromNode;
    uCompSequenceIndex = 0;

	///<(1) 기존에 잔여한 Path 정보가 있으면 에러 처리. (상위에서 처리?) 추후에 예외처리 해줄 것.
//	if(GetPathCount() !=0)
//	{
//		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
//    }

	///<(2) 들어가있는 Path를 하나씩 빼서 후처리 작업
	if( nTmpError == NO_ERR)
    {


			if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
				(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false))
			{
				// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
				ADD_LOG("[ERR_PATHSEARCH_FAIL_OnlyOne_7]");
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);

			}

			//투입/빼냄싸이클이 아닐때 확인하는 부분
			if((CmdGoData->CmdCycle != CYCLE::CycleNotExist) && (CmdGoData->CmdCycle != CYCLE::TakeOut))
			{
				if((CheckEmergencyPathInfo(pTmpMap,pPATH_SEARCH_NODE_INFO->NID) ==true) ||
				   (CheckEmergencyPathInfo(pTmpMap,pPATH_SEARCH_NODE_INFO->NextNID) ==true))
				   {
						nTmpError = ERR_LAYOUT_PATH_LIMIT;
				   }
			}
			//처음부터 하나씩 Path 정보를 뺌.
			nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

			// (3) EXECUTE_DRIVING_INFO를 PathSearcing List 만듬 ---------------
			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

			uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index 획득
			pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

			//Excute Info에 저장될 Data
			pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
			memset(pEXECUTE_DRIVING_INFO, 0x00,sizeof(EXECUTE_DRIVING_INFO));

			pEXECUTE_DRIVING_INFO->PathIndexNum				= pPATH_SEARCH_NODE_INFO->IndexNum;
			pEXECUTE_DRIVING_INFO->CmdID					= CmdID;
			pEXECUTE_DRIVING_INFO->SQNum					= pPATH_SEARCH_NODE_INFO->SQID;
			pEXECUTE_DRIVING_INFO->ChangeEndNodeDistance    = pPATH_SEARCH_NODE_INFO->dTotalDistance;
            pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= uTmpFromNode;
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= pPATH_SEARCH_NODE_INFO->NextNID;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartType		= pNODE_INFO->TagType;
//			pEXECUTE_DRIVING_INFO->BasicInfo.DeviceType     = pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NodeType     	= pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NextNodeType   = pNextNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo        = pPATH_SEARCH_NODE_INFO->DivInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
			pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
			pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
			pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;

			// for change node (jhoun.lim)
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;
#else
			pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
#endif
			//AOHC-195 오버런 개선 작업(유형2)
			if(bIgnorOverrun == true)
			{
				AdditionalInfo.bIgnolOverRun = true;
			}

			pEXECUTE_DRIVING_INFO->AddInfo					= AdditionalInfo;


			//Map Maker 관련 속도
			if((AdditionalInfo.CmdType == EXECUTE_DRIVING_CMD_MAPMAKE)  &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
			{
				pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
				ADD_LOG("[Map Maker PathSearch...]");

			}else if((bIsFoup ==false) && (m_pParamSet->nIsCurveSpeedUp == true) && (bUseAVS == false))
			{
#if(USE_CHANGENODE == 1)
				int nTempFoupCoverDectect = pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion;
#else
				int nTempFoupCoverDectect = pEXECUTE_DRIVING_INFO->BasicInfo.Direction & 0x0F;
#endif
				//N분기, 직선이 아닐경우에 주행속도 변경 : 기존대비 -300감소
				if((nTempFoupCoverDectect ==PATH_DIRECTION_LEFT) || (nTempFoupCoverDectect ==PATH_DIRECTION_RIGHT))
				{

					if((pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed	== 0.75) && (pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed < (double)(m_pParamSet->nIsShoutCurveSpeedUpValue/1000.0)))//Short Curve
						pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(m_pParamSet->nIsShoutCurveSpeedUpValue/1000.0);			// mm/s -> mm/msec
					else if((pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed	== 1.0)&& (pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed < (double)(m_pParamSet->nIsCurveSpeedUpValue/1000.0))) // Curve
						pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(m_pParamSet->nIsCurveSpeedUpValue/1000.0);			// mm/s -> mm/msec

					ADD_LOG("[IsFoup :Speed Down %f]",pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed);
				}

			}

			AddPath(pEXECUTE_DRIVING_INFO);

			dStopNodeMarGin = (double)GetSpeedDownDist((double)VHLACCEL,(double)MAXSPEED(1500,pPATH_INFO->Speed)) +  (double)GetMarginOfSpeedDist((double)MAXSPEED(1500,pPATH_INFO->Speed),(m_pParamSet->nPassPermitPeriod*m_pParamSet->nPassPermitCount));

			ADD_LOG("[Only dStopNodeMarGin] dStopNodeMarGin %f ,VHLACCEL:%f, dTempMaxSpeed:%f  %d %d",dStopNodeMarGin, (double)VHLACCEL, (double)pPATH_INFO->Speed, m_pParamSet->nPassPermitPeriod, m_pParamSet->nPassPermitCount);
//			double k = sqrt(sqrt((double)GetSpeedDownDist((double)VHLACCEL,(double)pPATH_INFO->Speed)));

			//(4)  EXECUTE_CYCLEDRIVING_INFO를  List 만듬 ---------------
			//    (시퀀스 ID가 변경이 일어나는 지점에서 확인 할 것. )

			//(4)-1시작 위치일 경우


			// (4)-1시작 위치일 경우, 패스가 하나밖에 없는 경우.

				uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
				nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
				pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

				uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
				nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index 획득
				pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보


				pCYCLE_DRIVING_INFO    					= new PKT_CYCLECMDDATA;

				memset(pCYCLE_DRIVING_INFO, 0x00,sizeof(PKT_CYCLECMDDATA));
				// 1-1기본정보 등록
				pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
				pCYCLE_DRIVING_INFO->CmdID = CmdID;
				pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
				pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
				pCYCLE_DRIVING_INFO->IsListF2B  = false;
				pCYCLE_DRIVING_INFO->Isreturn = false;

				//출발 LINK에 대해 통과허가 요청 유무
				//정지 Node일 경우, 앞으로 이동할 통과허가 구간에 대해 통과허가 요청 할것.
				//excute info에 이동 정보를 넣어주는 기준으로 사용.

				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint				=  pPATH_SEARCH_NODE_INFO->NID;
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType            =  pNODE_INFO->TagType;
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)dStopNodeMarGin;

				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 			= CYCLENODETYPE_START2TARGET; //Default로 설정
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum =  (unsigned short)pPATH_SEARCH_NODE_INFO->SQID;
				//YCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.SQNum =  0;

				if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
				{
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = pPATH_SEARCH_NODE_INFO->NID; // pPATH_SEARCH_NODE_INFO->NextNID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.TargetStationID = AdditionalInfo.StationID;
				}
				else
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = pPATH_SEARCH_NODE_INFO->NextNID; // pPATH_SEARCH_NODE_INFO->NextNID;

				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint = pPATH_SEARCH_NODE_INFO->NextNID; // pPATH_SEARCH_NODE_INFO->NextNID;
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;

              	dTempDis += (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;


				ADD_LOG("[PassPermit_Start_OnlyOne_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);




				ADD_LOG("[통과허가 정보] 시퀀스 Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d StartDist:%06f", pPATH_SEARCH_NODE_INFO->SQID, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);


				int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_SEARCH_NODE_INFO->NextNID,pFlowMap);

				if(FlowControlIndex != INDEX_NONE)
				{
					 if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID) ||
						(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_SEARCH_NODE_INFO->NextNID))
						{
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - m_pParamSet->dPassPermitOpenMargin;


							if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMainDefaultValue;
							else
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSubDefaultValue;

							ADD_LOG("[통과허가 정보] 시퀀스 Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d Start Distance%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);
							for(int i=0; i< 3; i++)
							{
								if (HIGH_BYTE == 0)
								 {
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
									if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
									}else
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
									}
								 }
								 else
								 {
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
									if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
									}else
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
									}

								 }
							}

							bPassPermit = true;
							bIsStartStationPassPermit =true;
							/*
							//정지노드내 이적재 태그에서 이동하는 경우
							if(TempPassPermitData.SQNum ==0)
							{
								bPassPermit = true;
								bIsStartStationPassPermit =true;
							}
							else
							{
								if(memcmp(&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData, &TempPassPermitData, sizeof(PASSPERMIT_COMMON)) != 0)
								{
									nTmpError = ERR_STD_ST_PASSPERMIT_FAIL;
								}
							}
							*/

						}
					 else
					 {
						if(pNODE_INFO->TagType == COMMON_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
						else if(pNODE_INFO->TagType == STOP_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;

						if(bcheckTarget ==false)
						{
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint =  0;
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;
						}
						else
						{
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint =  pPATH_SEARCH_NODE_INFO->NID;
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;
						}

					 }

				}else
				{
					if(pNODE_INFO->TagType == COMMON_NTYPE)
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
					else if(pNODE_INFO->TagType == STOP_NTYPE)
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;

					if(bcheckTarget ==false)
					{
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint =  0;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;
					}
					else
					{
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint =  pPATH_SEARCH_NODE_INFO->NID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;
					}
				}

					/*
					if((pNODE_INFO->TagType == RESET_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
					{
					   //memmove(&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData,&FinalCycleData->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON));
					   //emmove(&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData,&FinalCycleData->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON));

						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }
								//	pCYCLE_DRIVING_INFO->PassPermitData.ResetNodeID_Main = pPATH_SEARCH_NODE_INFO->NextNID;
								///puting Reset Sub부분 어떻게 넣을지 생각...
							//	pCYCLE_DRIVING_INFO->PassPermitData.ResetNodeID_Sub =0;

						}

					}
					*/

					ADD_LOG("[OnlyPassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d %d",
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
						bPassPermit);

                     //
					if((bPassPermit) && (nTmpError == NO_ERR))
					{
						nTmpError = CheckOnlyOnePassPermit(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
														pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
														pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
														&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
														AdditionalInfo.ProcessingGoInfo.IsStationStart,
														AdditionalInfo.ProcessingGoInfo.IsStationEnd,
														bIsStartStationPassPermit,
														bIsTargetStationoPassPermitOpen);

							//AOHC-195 오버런 개선 작업(유형2)
							if(bIgnorOverrun == true)
							{
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

//								AdditionalInfo.bIgnolOverRun = true;
							}
							else if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance < 0) && (bturnoff==true)) // 기동 Off이후 시작위치가 통과 개방 반납 위치의 합류부위치일 경우
							{
								ADD_LOG("기동Off && ResetNodeDistance < 0 조건 In");
                             	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = true;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
                            }
					}
					else
					{
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					  //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsPreLinkPPOpenPoint = false;
					}


					pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);
	}


	if(nTmpError != NO_ERR)
	{
		//1. Path List 정보 삭제
		InitPathList();

		//2. Cycle Lis 정보 삭제
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->popFront();
			delete pCYCLE_DRIVING_INFO;
		}
	}

	// 4. 임시 Path Search List 소멸 ------------------------------------------------
	//   1) 인자들 delete
	//   2) List 소멸
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}


	return nTmpError;
}

int PathSearch::CheckOnlyOnePassPermit(CYCLENODEINFO CycleNodeInfo, PASSPERMIT_NODE_TYPE StartType, PASSPERMIT_NODE_TYPE EndType, EXECUTE_CYCLEDRIVING_INFO *pPassPermitInfo,bool IsStartStation,bool IsEndStation,bool bStartStationPassPermit,bool bIsTargetStationoPassPermitOpen)	///< PathSearch 결과 Return
{
	int nError = NO_ERR;

	if(bStartStationPassPermit ==false)  //1. 통과허가 요청하지 않는 로직  (Safety Level 적용전 기존로직)
	{
		if((IsStartStation ==  true) && (IsEndStation ==false))   //1) 이적재 -> 주행으로 이동
		{
		  if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
			}
			else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
			}
			else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
			}
			else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
			}
			else
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

			}
		}
		else if((IsStartStation ==  false) && (IsEndStation ==true)) //2) 주행 -> 이적재으로 이동
		{
		   if(bIsTargetStationoPassPermitOpen == true)  //반납이 필요한 경우.
		   {
               if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

				}
		   }
		   else        //반납이 필요하지 않는 경우
		   {
		        if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

				}
           }

		}
		else  //3) 이적재 ->이적재로 이동하는 경우 ※ 주행->주행(케이스 없음),
		{
			if((IsStartStation ==  true) && (IsEndStation ==true) && (bIsTargetStationoPassPermitOpen == true))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
			}
			else
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
			}
		}
	}
	else //2. 통과허가를 받아야되는 케이스
	{
		if((IsStartStation ==  true) && (IsEndStation ==false))    //1) 이적재 -> 주행으로 이동
		{
		  if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
			}
			else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
			}
			else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
			}
			else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
			}
			else
			{
				nError = ERR_STD_ST_PASSPERMIT_FAIL;

			}
		}
		//2) 주행 -> 이적재, 이적재 -> 이적재로 이동
		else if(((IsStartStation ==  false) && (IsEndStation ==true))
				||((IsStartStation ==  true) && (IsEndStation ==true)))
		{
			 if(bIsTargetStationoPassPermitOpen == true)    // 타겟이 반납인경우
			 {
				if((StartType == STOP_NTYPE) && (EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else
				{
					nError = ERR_STD_ST_PASSPERMIT_FAIL;
				}
			 }
			 else
			 {
			 	if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else
				{
					nError = ERR_STD_ST_PASSPERMIT_FAIL;
				}
			 }

		}
	}

	 return nError;
}

int PathSearch::CheckPassPermit(CYCLENODEINFO CycleNodeInfo, PASSPERMIT_NODE_TYPE StartType, PASSPERMIT_NODE_TYPE EndType, EXECUTE_CYCLEDRIVING_INFO *pPassPermitInfo,bool bIsTargetStationoPassPermitOpen)	///< PathSearch 결과 Return
{
			int nError =NO_ERR;
			if(m_pDrivingControl == NULL) m_pDrivingControl = OHTMainForm->GetDrivingControl();
			int nMTLTakeInPIOStep = m_pDrivingControl->m_nMTLTakeInPIOStep; //MTL Auto TakeIn 로직 실행시 예외처리용
			/*
			  예외처리 내용 추가----puting
			  1. Reset 이후에 Reset Node가 오는 경우
			  2. Normal Node 이후에 Reset Node가 오는 경우
			  3. Stop Node 이후에 Normal Node가 오는 경우
			*/
				/*
				Normal -> Stop							Normal -> Normal
				Stop -> Stop		Stop -> Reset		Stop -> Noraml			Stop -> Comon
				Reset -> Stop							Reset -> Noraml
				Common -> Stop		Common -> Reset		Common -> Normal		Common -> Comon
				*/
			///////----------------------------puting 명일여기서 부터 시작~~~ 완료

			//bIsTargetStationoPassPermitOpen //타겟 반납 유무 마지막일때 이적재 위치에 정지할 경우만 true로 처리할것.  IsCurLinkPPOpenPoint = false ->   IsCurLinkPPOpenPoint = true
			if(CycleNodeInfo ==  CYCLENODETYPE_START2STOP)
			{
				//시작과 중간에서 경우,
				   if((StartType == NORMAL_NTYPE) &&(EndType==STOP_NTYPE))
					{

						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

					}else if((StartType == RESET_NTYPE) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;//NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

					}else if((StartType == COMMON_NTYPE) &&((EndType==STOP_NTYPE)||(EndType==COMMON_NTYPE)))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

					}else if((StartType == STOP_NTYPE) &&((EndType==STOP_NTYPE)||(EndType==COMMON_NTYPE)))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else
						nError = ERR_STD_ST_PASSPERMIT_FAIL;
			}
				//중간에서 중간일 경우,
			else if(CycleNodeInfo ==  CYCLENODETYPE_STOP2STOP)
			{
				if(((StartType == STOP_NTYPE) ||(StartType == COMMON_NTYPE)) &&(EndType==STOP_NTYPE)||(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//					pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//					pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
//					pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = true;    	//다음 Link의 통과허가 개방 요청 유무
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
				}
				else
					nError = ERR_STD_MD_PASSPERMIT_FAIL;
			}
			else if(CycleNodeInfo ==  CYCLENODETYPE_STOP2TARGET)
			{
					//중간에서 마지막일 경우,
					if(((StartType == STOP_NTYPE) ||(StartType == COMMON_NTYPE)) &&(EndType==NORMAL_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
//						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else if(((StartType == STOP_NTYPE) ||(StartType == COMMON_NTYPE)) &&(EndType==RESET_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;
						//pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;        puting 20160113

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
//						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else if(((StartType == STOP_NTYPE) ||(StartType == COMMON_NTYPE)) &&(EndType==COMMON_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;
						//pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;        puting 20160113
//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
//						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else if(((StartType == STOP_NTYPE) ||(StartType == COMMON_NTYPE)) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
//						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else
						nError = ERR_STD_ED_PASSPERMIT_FAIL;
			}
			else if(CycleNodeInfo ==  CYCLENODETYPE_START2TARGET)
			{
				//시작에서 마지막일 경우,

					if((StartType == NORMAL_NTYPE) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_MEANINGLESS;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

					}
					else if((StartType == RESET_NTYPE) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;//true;    //현재 Link의 통과허가 개방 요청 유무
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

					}
					else if((StartType == COMMON_NTYPE) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

					}else if((StartType == STOP_NTYPE) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
//						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else if((StartType == STOP_NTYPE) &&(EndType==NORMAL_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
					  	pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else if((StartType == NORMAL_NTYPE) &&(EndType==NORMAL_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_MEANINGLESS;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
  //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else if((StartType == RESET_NTYPE) &&(EndType==NORMAL_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;//NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;//true;    //현재 Link의 통과허가 개방 요청 유무
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
					 	pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else if((StartType == COMMON_NTYPE) &&(EndType==NORMAL_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
					  	pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
                    	pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}
					else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //현재 Link의 통과허가 요청유무
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //현재 Link의 통과허가 개방 요청 유무
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//다음 Link의 통과허가 개방 요청 유무
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)
					}

					else
						nError = ERR_STD_ETC_PASSPERMIT_FAIL;
			}
			else
						nError = ERR_STD_ETC_PASSPERMIT_FAIL;

			if(MTL_TakeIn_CTRL_STEP_INIT <= nMTLTakeInPIOStep && nMTLTakeInPIOStep < MTL_TakeIn_CTRL_STEP_COMPLETE)
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

				ADD_LOG("MTL Auto TakeIn 중에는 MTL 합류부 통과허가 미사용, nMTLTakeInPIOStep = %d", nMTLTakeInPIOStep);
			}
			return nError;
}


/**
@brief   Driving Tag Map Back Search 함수
@author  puting
@date    2016-10-22
@param   pMap : 사용하는 Map
@param   FromNode : 시작 Node
@param   ToNode : Target Node
@param   AdditionalInfo : 주행 명령 추가 정보
@param   DRIVING_INFO_List : 주행 명령 추가 List
@return  Error Code
*/
int PathSearch::StandardSearchPassBack(MAP *pMap, PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	// 0. 값 초기화 ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// 현재 에러 초기화

	UINT	CmdID				= CmdGoData->CmdID;  //지시 ID
	MAP		*pTmpMap			= pMap;				// 사용하는 Map

	//경로탐색을 해야될 시작과 끝 위치
	UINT	uFromNode			= AdditionalInfo.ProcessingGoInfo.StartNodeID;			// 보모 노드
	UINT	uToNode				= AdditionalInfo.ProcessingGoInfo.EndNodeID;		    //  Next Node

	LList	*pResultAddList		= DRIVING_INFO_List;	// 주행을 위한 명령을 추가하기 위한 Cycle List

	bool	bArrived			= false;	// Map Seach에 성공한 경우

	UINT	uPathSearchIndex	= 0;		// PathSearch List의 Index
	UINT	uTmpListCount		= 0;		// PathSearch List 내의 인자 수

	UINT 	uSequenceIndex      = 0;      // 시퀀스 Number
	UINT 	uCompSequenceIndex  = 0;      // 임시 시퀀스 Number
	UINT 	uFirstSequenceIndex  = 0;      // 임시 시퀀스 Number

  //	UINT	uCheckExtraNum		 = 0;

	int		nNodeIndex			= 0;		// Node의 Index
	int     nNextNodeIndex      = 0;        //NextNode의 Index
	int		nPathIndex			= 0;		// Path의 Index
	int		nSubPathIndex	 	= 0;		// Path의 Index

	PATH_DIVERGENCE_INFO     nTempDivInfo        = PATH_DIVERGENCE_NONE;         //분기정보


	UINT	uTmpToNode			= 0;		// Path 단위로 정보 확인 시
	UINT	uTmpFromNode		= 0;		// Path 단위로 정보 확인 시
//	UINT	uTmpNode			= 0;		// 임시 정보 확인 시

	double	dTmpTotalDistance	= 0.0;	// 현재 Node까지의 총 가중치(거리/속도)
	double	dTmpPathDistance	= 0.0;	// Path의 Link 가중치(거리/속도)
	double  dStopNodeMarGin     = 0.0;
	double  dTempMaxSpeed       = 0.0;  //통과허가를 위한 Max 속도

	//기본 분기 방향 // 중요한 부분. 0 : 왼쪽 1: 오른쪽
	int	bDefaultPathDirection = (int)CmdGoData->PathData_Def_Direction;
	bool	bCheckDefaultPathDirection = false;

	int		dtmpPathSearchListCnt = 0; //Pass Permit Limit 체크용

	NODE_INFO	*pNODE_INFO		= NULL;	// Node 정보
	NODE_INFO	*pNextNODE_INFO	= NULL;	// NextNode 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보
	PATH_INFO	*pSubPATH_INFO	= NULL;	// Path 정보를 이용해 통과허가 Sub정보를 획득

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search를 위한 Node 구조체
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search를 위한 Node 구조체


	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread에 전달하는 구조체
	EXECUTE_DRIVING_INFO	*pTempEXECUTE_DRIVING_INFO	= NULL;	// 선조향 관련된 내용
	PKT_CYCLECMDDATA  		*pCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst에 전달하는 구조체
//	PKT_CYCLECMDDATA		*pNextCYCLE_DRIVING_INFO= NULL; // CycleLIst에 전달하는 구조체
	PKT_CYCLECMDDATA  		*pPreCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst에 전달하는 구조체

	LList PathSearchList;									// PathSearch를 위한 List


	EXECUTE_DRIVING_ADD_INFO TempAdditionalInfo;

	memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
//	NODE_TYPE DrivingEndNodeType = NORMAL_POINT;
	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";


	//------------- 1. 전처리 부분------------//
	// 1. 초기 설정 : ----------------------------------------------------------
	//   0) 시작 Node의 Path 확인
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
  //	||	(uFromNode == uToNode)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_PATHSEARCH_FAIL;

	}

	//  1) Station의 Path 확인   //용도 ?  puting


	// 2) 시작 Node에 Path 연결이 없는 경우

	///시작이 Station인경우에 대한 전처리 부분
	if(AdditionalInfo.ProcessingGoInfo.IsStationStart ==true)
	{
		//이적재의 경우 부모노드가 들어온다.
		ADD_LOG("[PATHSEARCH_Start_Station_5] %d",CmdGoData->PathData_StartNodeID);
		nNodeIndex = pTmpMap->FindNodeIndex(CmdGoData->PathData_StartNodeID);   ///< 출발 노드 Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< 출발 노드 정보

		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보
					if(pPATH_INFO->EndNID == AdditionalInfo.ProcessingGoInfo.StartNodeID)
					{
						AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode =  pPATH_INFO->Distance - AdditionalInfo.ProcessingGoInfo.StationData_StartRunDist;
                    }
				}
			}

			if(AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode <0)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
				ADD_LOG("[PATHSEARCH_Start_Station_Fail] %d",AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode);
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
            }

		}
		else
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_52] %d:%d"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			return nTmpError;
		}
	}



	ADD_LOG("[PATHSEARCH_Step 1 from%d To %d]",uFromNode,uToNode);



	//------------- 2. 주요 처리부분------------//

	// 3) 시작 Node위치 List에 저장
	uTmpFromNode = uFromNode;
	uPathSearchIndex = 0;
	uSequenceIndex = 0;

	// 2. Index 순차적으로 증가시키면서 Node 추가 ------------------------------
	//   1) 다음 위치로 Index를 증가시켜가면서 다음 노드들을 PathSarchList에 추가함
	//     . PathSearchList에 추가 할 때는 거리 가중치 반영하여 가까운 순서로 삽입
	//     . 중복되는 Node 가 있을 경우 거리 가중치가 가까운 노드로 삽입, 먼 노드는 삭제
	//   2) 시작 노드에 연결 Path의 개수 만큼 시작노드가 발견되면 정지

	while(bArrived==false)	// 시작 Node 나올 경우 bArrived 가 true로 바꿔줌
	{
		nTempDivInfo        = PATH_DIVERGENCE_NONE;         //분기정보
		////< (1) first Node Check!!
		if(uPathSearchIndex !=0)
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex-1);
			////< (1)-1 현재 Node의 정보 확인
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			}
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보


			///< (1)-2 Node Info Check!!
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{

				///(3)-2 거리 정보 확인 필요
//				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;

			}
			else
			{
				nTmpError = ERR_STD_INNER_PATH_FAIL; // PathSearch 실패
				//break;
			}

		}
		else
		{
			//uTmpFromNode = uTmpFromNode;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보
		}

		////< (2) List의 Max Count Check!!     puting -----------------------------------------------------------추후삭제나 수정유무 판단.
		dtmpPathSearchListCnt = PathSearchList.getCount();
		if((dtmpPathSearchListCnt > m_pParamSet->nMapSearchMax) && (AdditionalInfo.FinalNode == 0))
			{
				nTmpError = ERR_PATHSEARCH_FAIL;
				ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dtmpPathSearchListCnt , m_pParamSet->nMapSearchMax);
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				break;
			}

		////< (4) 현재 노드가 아무런 PathIndex가 없을 경우 에러 처리 추가
		if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_54] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		   //	return nTmpError;
		}


		///<(6) Next Node(Path) 정보가 어떤 건지 확인하는 부분  puting 20151107
 		////< (6)-1 분기인 경우,
		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			////< (6-1) 기본외 번지인지 혹인하는 부분  puting 20151107
			for(int nTempIndex=0; nTempIndex<2; nTempIndex++)// 다음 Path 개수 만큼 확인
			{

				if(bDefaultPathDirection == nTempIndex)
				{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //왼쪽 분기
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //왼쪽 분기

					nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
				}

			}
		}

		////< (6-3) 분기가 아닌경우,
		else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
//			nTempDivInfo =  PATH_DIVERGENCE_NONE;
			for(int n=0; n<2; n++)// 다음 Path 개수 만큼 확인
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
					nPathIndex = pNODE_INFO->NextPathIndex[n];
			}

		}

		///< (7) 연결된 Next Path(Node) 정보 확인 및 생성
		////<(7)-1 연결된 Path가 존재 하는 경우
		if(nPathIndex!=INDEX_NONE)
		{
			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// 연결된 Path 정보

			dTmpPathDistance = (double)pPATH_INFO->Distance;
			uTmpToNode = pPATH_INFO->EndNID; //연결된 다음 노드

			ADD_LOG("[PathSearch_Check][uTmpFromNode:%d/uTmpToNode:%d/uSequenceIndex%d]", uTmpFromNode, uTmpToNode, uSequenceIndex);

		   //통과허가 정보 확인하는 부분  //삭제
			/// (5) 정지노드일 경우 sequenceIndex 증가하는 부분  puting------------------------------------------------------------------------------------------ MapData 수정 필요.7
			if(uPathSearchIndex==0)
			 {
                	uSequenceIndex = GetIncreaseSeqenceNumber();
				//ADD_LOG("[BACKSequenceIndexUpCheck][SequenceIndex:%d/NodeType:%d]", uSequenceIndex , pNODE_INFO->TagType);

			 }
			 else
			 {
			 	uSequenceIndex = GetSeqenceNumber();
			   //	ADD_LOG("[SequenceIndexUpCheck][SequenceIndex:%d/NodeType:%d]", uSequenceIndex , pNODE_INFO->TagType);
			 }

			 if(uPathSearchIndex==0)
				uFirstSequenceIndex  = uSequenceIndex;

			////<(7)-1 Next Path 정보
			uPathSearchIndex++;


			if(uPathSearchIndex >MAX_PATHINDEX)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

			//가중치는 필요없음.

//			IterPos = mapPathSearchNode.find(uTmpToNode);
//			// mapPathSearchNode 에  uTmpFromNode 의 PathSearch Node정보가 없으면 정상처리.
//			if(IterPos == mapPathSearchNode.end())
//			{
				// PathSearch Node 정보 생성 : 없을 경우
				pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
				pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
				pPATH_SEARCH_NODE_INFO->IndexNum = uPathSearchIndex;

			   //	pPATH_SEARCH_NODE_INFO->PassPermit_NType =
				pPATH_SEARCH_NODE_INFO->SQID =  uSequenceIndex;
				pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
				pPATH_SEARCH_NODE_INFO->SubNID = 0;
				pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
				pPATH_SEARCH_NODE_INFO->DivInfo = nTempDivInfo;
				pPATH_SEARCH_NODE_INFO->PreSteering = pPATH_INFO->Steering;     //선조향 관련된 정보 추가. puting 20160119

				///puting 추가해야되는 사항/ 초기거리와 Target에 따라 거리값 변경.
				//  Step 1.Target이 station일 경우 거리값 추가할것.
				pPATH_SEARCH_NODE_INFO->dTotalDistance = 0;  //후처리에서 진행할것. 뒤로 넣는거 때문에.
				pPATH_SEARCH_NODE_INFO->dPathDistance  = - (double)dTmpPathDistance;
//				ADD_LOG("[PATHSEARCH_Stepll %06f]",pPATH_SEARCH_NODE_INFO->dTotalDistance);

				//추가하는 루틴
				PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
//				mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;

				///< (3)-1 마지막 노드 도달 확인
				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode)
				{
//					ADD_LOG("[PATHSEARCH_Stepll %06f]",pPATH_SEARCH_NODE_INFO->dTotalDistance);

					bArrived = true;
					break;
				}
//			}
//			else
//			{
//				nTmpError = ERR_PATHSEARCH_FAIL;
//			}

		}	// if(nPathIndex!=-1)

		if(nTmpError!=NO_ERR) break;


		nPathIndex = 0;
		nSubPathIndex =0;

	 }  //while()


    ADD_LOG("[PATHSEARCH_Step 3]");
	///----------------3. 후처리 부분 ---------------------//
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) 에 추가
	try{
	bArrived		= false;
	uPathSearchIndex= 0;

	uTmpListCount	= PathSearchList.getCount();
	uTmpFromNode	= uFromNode;
	uCompSequenceIndex = 0;
	dTmpTotalDistance = 0.0;

	memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
	}catch(...)
	  ADD_LOG("[ERR_PATHSEARCH_Exception111111111]");


	///<(2) 들어가있는 Path를 하나씩 빼서 후처리 작업
	if( nTmpError == NO_ERR)
	{

		for(int PathIndex =0; PathIndex < uTmpListCount; PathIndex++)
		{
			uPathSearchIndex++; //들어가있는 Path의 Pop Conut용!!

			if(nTmpError!=NO_ERR) break;

			try
			{
//			///< (2)-1 후처리 전 에러 확인 내용
			pPATH_SEARCH_NODE_INFO =(PATH_SEARCH_NODE_INFO*)PathSearchList.popBack();


			dTmpTotalDistance += pPATH_SEARCH_NODE_INFO->dPathDistance;


			if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
				(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false))
			{
				// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
				ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
                delete pPATH_SEARCH_NODE_INFO;
				break;
			}
			}catch(...)
			   ADD_LOG("[ERR_PATHSEARCH_Exception22222222222]");



			//(4)  EXECUTE_CYCLEDRIVING_INFO를  List 만듬 ---------------
			//    (시퀀스 ID가 변경이 일어나는 지점에서 확인 할 것. )

			if((uCompSequenceIndex != pPATH_SEARCH_NODE_INFO->SQID) ||
				(uTmpListCount == uPathSearchIndex))  //Target Node 일 경우, 마지막 Ndoe
			{
				 //시퀀스 ID 정보 등록
				 uCompSequenceIndex = pPATH_SEARCH_NODE_INFO->SQID;

				 dStopNodeMarGin = 0;  // 후진은 통과허가 요청이 없음.

				if((uPathSearchIndex==FIRST_INDEX)&&(uTmpListCount != uPathSearchIndex))
				{
					try{
					TempAdditionalInfo.DistToStation = 0.0;
					TempAdditionalInfo.StationType = MARK_TYPE_TAG_STATION_BACK;
					TempAdditionalInfo.FinalPortType = MARK_TYPE_TAG_STATION_BACK;


					uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

					uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index 획득
					pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보


					pCYCLE_DRIVING_INFO    	= 	new PKT_CYCLECMDDATA;

					memset(pCYCLE_DRIVING_INFO, 0x00, sizeof(PKT_CYCLECMDDATA));

					// 1-1기본정보 등록
					pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
					pCYCLE_DRIVING_INFO->CmdID = CmdID;
					pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
					pCYCLE_DRIVING_INFO->IsNoExcuteCycle = true;
					pCYCLE_DRIVING_INFO->IsListF2B  = false;
					pCYCLE_DRIVING_INFO->Isreturn = false;

					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint					=  pPATH_SEARCH_NODE_INFO->NextNID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType           	 	=  pNextNODE_INFO->TagType;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	 	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance            		=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;      //Stop 포인트 위치
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 				= CYCLENODETYPE_START2STOP; //Default로 설정
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum 	= (unsigned short) uCompSequenceIndex;


					ADD_LOG("[PassPermit_Start_NotMatch_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);

				

					pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);
					}catch(...)
						ADD_LOG("[ERR_PATHSEARCH_Exception33]");

				}

				// (4)-1시작 위치일 경우, 패스가 하나밖에 없는 경우.
				else if((uPathSearchIndex==FIRST_INDEX)&&(uTmpListCount == uPathSearchIndex))
				{
				   // 후진으니 패스는 무조건 하나 이상임.


				}
				//(4)-2 마지막 위치 일 경우.(패스가 하나 이상인 경우)
				else if((uPathSearchIndex!=FIRST_INDEX)&&(uTmpListCount == uPathSearchIndex))
				{
					TempAdditionalInfo.DistToStation = AdditionalInfo.DistToStation;
					TempAdditionalInfo.StationType = AdditionalInfo.StationType;
					TempAdditionalInfo.FinalPortType = AdditionalInfo.FinalPortType;
					//정보 획득
					pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();

                   	uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

					uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index 획득
					pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보



					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint =  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNODE_INFO->TagType;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance + dTmpTotalDistance;


					if(uCompSequenceIndex == uFirstSequenceIndex)
					{
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
					}
					else
					{
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
					}

					ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);


					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;//NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;       //현재 Link의 통과허가 요청유무
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;    //현재 Link의 통과허가 개방 요청 유무
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)

//					nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
//											pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
//											pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
//											 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info);
				}
				//(4)-3중간 지점 통과할 경우
				else
				{

				    //후진은 하나의 싸이클만 생성됨.

				}


                dTempMaxSpeed = 0.0;  //Max Speed 초기화
			}else
			{

													// 2-3 작업변경을 위한 분기 정보
					pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();
					nNodeIndex = pTmpMap->FindNodeIndex(pPATH_SEARCH_NODE_INFO->NID);	// Node Index 획득
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

				try{
					// 2-3 작업변경을 위한 분기 정보
					TempAdditionalInfo.DistToStation = AdditionalInfo.DistToStation;
					TempAdditionalInfo.StationType = AdditionalInfo.StationType;
					TempAdditionalInfo.FinalPortType = AdditionalInfo.FinalPortType;

					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType 	=  pNODE_INFO->TagType;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance 	= (double)pPATH_SEARCH_NODE_INFO->dTotalDistance + dTmpTotalDistance;
				}catch(...)
						 ADD_LOG("[ERR_PATHSEARCH_Exception77777777]");

            }

			//처음부터 하나씩 Path 정보를 뺌.
			nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		  //	ADD_LOG("[ExcuteDrivingInfo: %d", nPathIndex);
			// (3) EXECUTE_DRIVING_INFO를 PathSearcing List 만듬 ---------------
			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

			uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index 획득
			pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node 정보

			//Excute Info에 저장될 Data
			pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
			memset(pEXECUTE_DRIVING_INFO, 0x00,sizeof(EXECUTE_DRIVING_INFO));

			pEXECUTE_DRIVING_INFO->PathIndexNum				= pPATH_SEARCH_NODE_INFO->IndexNum;
			pEXECUTE_DRIVING_INFO->CmdID					= CmdID;
			pEXECUTE_DRIVING_INFO->SQNum					= pPATH_SEARCH_NODE_INFO->SQID;
			pEXECUTE_DRIVING_INFO->ChangeEndNodeDistance    = pPATH_SEARCH_NODE_INFO->dTotalDistance;
            pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= pPATH_SEARCH_NODE_INFO->NID;
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= pPATH_SEARCH_NODE_INFO->NextNID;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartType		= pNODE_INFO->TagType;
//			pEXECUTE_DRIVING_INFO->BasicInfo.DeviceType     = pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NodeType      	= pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NextNodeType   = pNextNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo        = pPATH_SEARCH_NODE_INFO->DivInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)0.4;			// mm/s -> mm/msec AOHC-367
			pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
			pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
			pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;

			// for change node (jhoun.lim)
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;

#else
			pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
#endif
			//Map Maker 관련 속도

			memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &TempAdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));

			AddPath(pEXECUTE_DRIVING_INFO);



			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
			delete pPATH_SEARCH_NODE_INFO;

		}
	}
     ADD_LOG("[PATHSEARCH_Step 4]");
   //	if(uTmpFromNode !=  uTmpFromNode)  nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
	///< Error 발생시 모든 List 삭제 하는 부분
	if(nTmpError != NO_ERR)
	{
		//1. Path List 정보 삭제
		InitPathList();

		//2. Cycle Lis 정보 삭제
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->popFront();
			delete pCYCLE_DRIVING_INFO;
		}
	}

	// 4. 임시 Path Search List 소멸 ------------------------------------------------
	//   1) 인자들 delete
	//   2) List 소멸
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}


	return nTmpError;
}


/**
 @brief   명령 삭제
 @author  soo583
 @date    2013.07.11
 @param   szFilePath(파일 경로)
 @return  성공 시 true / 실패 시 false
 */
bool PathSearch::loadParameterRecovery(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet)
{
    bool bSuccess = true;
    int nTmpValue = 0;
    int nResult = 0;
    double dTmpValue = 0.0;
	bool bTmpValue = false;
	double dResult = 0.0;
	char strTempValue[8]= {0,};
	bool bCheckParam = false;

    LIniFile *pIniFile;
    pIniFile = NULL;
    pIniFile = new LIniFile(szFilePath);

    if(!pIniFile->GetIniFileExist())
    {
        delete pIniFile;
		pIniFile = NULL;
		bSuccess = false;
    }

	if(pIniFile != NULL)
	{

       	pIniFile->ReadString("Version","IniVersion", "00", strTempValue);
		if(StrToIntA(strTempValue) <= StrToIntA(pParamSet->Version))
			bSuccess = false;
		else
		{
			memmove(&pParamSet->Version, strTempValue, strlen(strTempValue)); //Return Value
			memset(strTempValue, 0, sizeof(strTempValue));
		}

		// 1.
		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PATHSEARCH", "MapSearchMax", MAX_LIST_COUNT_FOR_PRESTEER, 5000, 15000,&nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nMapSearchMax = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PASSPERMIT","PassPermitPeriod", 90, 0, 200, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nPassPermitPeriod = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PASSPERMIT","PassPermit_Count ", 7, 1, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nPassPermitCount = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("PASSPERMIT", "PassPermitOpen_Margin", MARGIN_OF_DIST_TO_RESETNODE, 100.0, 500.0,&dTmpValue);
			if(bCheckParam)
			{
				pParamSet->dPassPermitOpenMargin = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PASSPERMIT","PassPermitRollbackTime", 200, 100, 300, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nPassPermitRollbackTime = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PASSPERMIT","PassPermitRollbackFinshTime", 400, 350, 800, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nPassPermitRollbackFinshTime = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PATHSEARCH", "IsCurveSpeedUp", 1,0,3, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nIsCurveSpeedUp = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PATHSEARCH","IsShoutCurveSpeedUpValue", 1000, 750, 1000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nIsShoutCurveSpeedUpValue = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PATHSEARCH","IsCurveSpeedUpValue", 1000, 800, 1200, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nIsCurveSpeedUpValue = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("PASSPERMIT", "PassPermitMax_Margin", 10000.0, 5000.0, 20000.0,&dTmpValue);
			if(bCheckParam)
			{
				pParamSet->dPassPermitMaxMargin = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("PASSPERMIT", "PassPermitRollbacking_Margin", 200.0, 0.0, 300.0,  &dTmpValue);
			if(bCheckParam)
			{
				pParamSet->dPassPermitRollbackMargin = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		delete pIniFile;


	}
	else
	{
		bSuccess = false;
	}

	//AOHC-358 파라미터 적용 오류 시, 에러 발생
	if(bSuccess ==false)
	{
	  //	SOM_LOG ("loadparameterRecovery");
		bCheckPathParam = true;
		bCheckParam = loadParameter("DataFiles\\PathSearch.ini", m_pParamSet);
		if(bCheckParam == false)

			OHTMainForm->m_CheckParamResult = ERR_PARADATA_LOAD_FAIL;
		else
			OHTMainForm->m_CheckParamResult = NO_ERR;

	}



	return bSuccess;
}
/**
 @brief   명령 삭제
 @author  soo583
 @date    2013.07.11
 @param   szFilePath(파일 경로)
 @return  성공 시 true / 실패 시 false
 */
bool PathSearch::loadParameter(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet)
{
    bool bSuccess = false;
    int nTmpValue = 0;
    int nResult = 0;
    double dTmpValue = 0.0;
	bool bTmpValue = false;
	double dResult = 0.0;
	char strTempValue[8]= {0,};


    LIniFile *pIniFile;
    pIniFile = NULL;
	pIniFile = new LIniFile(szFilePath);

    if(!pIniFile->GetIniFileExist())
    {
        delete pIniFile;
        pIniFile = NULL;
	}
	//AOHC-358 파라미터 적용 오류 시, 에러 발생
	 if(pIniFile == NULL|| bCheckPathParam == true)
	{
		strcpy(pParamSet->Version, "00");

		pParamSet->nMapSearchMax  = MAX_LIST_COUNT_FOR_PRESTEER;
		pParamSet->nIsCurveSpeedUp = 1;
		pParamSet->nIsShoutCurveSpeedUpValue = 1000;
		pParamSet->nIsCurveSpeedUpValue = 1000;

		pParamSet->nPassPermitPeriod = 90;
		pParamSet->nPassPermitCount  = 8;
		pParamSet->dPassPermitOpenMargin = MARGIN_OF_DIST_TO_RESETNODE;
		pParamSet->dPassPermitRollbackMargin = 250.0;
		pParamSet->nPassPermitRollbackFinshTime  = 350.0;
		pParamSet->dPassPermitMaxMargin  = 10000.0;
		pParamSet->dPassPermitRollbackMargin = 200.0;

		bSuccess = false;
		bCheckPathParam = false ;
		ADD_MD_LOG("Pathsearch Param Loading Fail");
		 return bSuccess;
	}
	else if(pIniFile != NULL)
	{

		memset(&pParamSet->Version,0x00, sizeof(pParamSet->Version));
		pIniFile->ReadString("Version","IniVersion", "00", strTempValue);
		memmove(&pParamSet->Version, strTempValue, strlen(strTempValue)); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		// 1. HOIST PARAM
		pIniFile->ReadInteger("PATHSEARCH", "MapSearchMax", MAX_LIST_COUNT_FOR_PRESTEER, &nTmpValue);
		pParamSet->nMapSearchMax = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("PASSPERMIT","PassPermitPeriod", 90, 0, 200, &nTmpValue);
		pParamSet->nPassPermitPeriod = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("PASSPERMIT","PassPermit_Count ", 8, 1, 10, &nTmpValue);
		pParamSet->nPassPermitCount = nTmpValue;

		pIniFile->ReadDouble("PASSPERMIT", "PassPermitOpen_Margin", MARGIN_OF_DIST_TO_RESETNODE, &dTmpValue);
		pParamSet->dPassPermitOpenMargin = dTmpValue;


		pIniFile->ReadIntegerMinMaxCheck("PASSPERMIT","PassPermitRollbackTime", 200, 100, 300, &nTmpValue);
		pParamSet->nPassPermitRollbackTime = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("PASSPERMIT","PassPermitRollbackFinshTime", 350, 340, 800, &nTmpValue);
		pParamSet->nPassPermitRollbackFinshTime = nTmpValue;

		pIniFile->ReadInteger("PATHSEARCH", "IsCurveSpeedUp", 1, &nTmpValue);
		pParamSet->nIsCurveSpeedUp = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("PATHSEARCH","IsShoutCurveSpeedUpValue", 1000, 750, 1000, &nTmpValue);
		pParamSet->nIsShoutCurveSpeedUpValue = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("PATHSEARCH","IsCurveSpeedUpValue", 1000, 800, 1200, &nTmpValue);
		pParamSet->nIsCurveSpeedUpValue = nTmpValue;

		pIniFile->ReadDouble("PASSPERMIT", "PassPermitMax_Margin", 10000.0, &dTmpValue);
		pParamSet->dPassPermitMaxMargin = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("PASSPERMIT", "PassPermitRollbacking_Margin", 200.0, 0.0, 300.0,  &dTmpValue);
		pParamSet->dPassPermitRollbackMargin = dTmpValue;

		delete pIniFile;
		bSuccess = true;

	}


	return bSuccess;
}

bool PathSearch::saveParameter(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet)
{
    bool bResult = false;

    LIniFile *m_lWriteIniFile;
    m_lWriteIniFile = NULL;
    m_lWriteIniFile = new LIniFile(szFilePath);

    if(!m_lWriteIniFile->GetIniFileExist())
    {
		delete m_lWriteIniFile;
		m_lWriteIniFile = NULL;
    }

    if(m_lWriteIniFile != NULL)
    {
		m_lWriteIniFile->DeleteIniFile();

		m_lWriteIniFile->WriteString("Version","IniVersion", pParamSet->Version);
		// 1. Hoist Param
		m_lWriteIniFile->WriteInteger("PATHSEARCH", "MapSearchMax", pParamSet->nMapSearchMax);

		m_lWriteIniFile->WriteInteger("PASSPERMIT", "PassPermitPeriod", pParamSet->nPassPermitPeriod);
		m_lWriteIniFile->WriteInteger("PASSPERMIT", "PassPermit_Count", pParamSet->nPassPermitCount);
		m_lWriteIniFile->WriteDouble("PASSPERMIT", "PassPermitOpen_Margin", pParamSet->dPassPermitOpenMargin);
		m_lWriteIniFile->WriteInteger("PASSPERMIT", "PassPermitRollbackTime", pParamSet->nPassPermitRollbackTime);
		m_lWriteIniFile->WriteInteger("PASSPERMIT", "PassPermitRollbackFinshTime", pParamSet->nPassPermitRollbackFinshTime);

		m_lWriteIniFile->WriteInteger("PATHSEARCH", "IsCurveSpeedUp", pParamSet->nIsCurveSpeedUp);
		m_lWriteIniFile->WriteInteger("PATHSEARCH", "IsShoutCurveSpeedUpValue", pParamSet->nIsShoutCurveSpeedUpValue);
		m_lWriteIniFile->WriteInteger("PATHSEARCH", "IsCurveSpeedUpValue", pParamSet->nIsCurveSpeedUpValue);

		m_lWriteIniFile->WriteDouble("PASSPERMIT", "PassPermitMax_Margin", pParamSet->dPassPermitMaxMargin);

		m_lWriteIniFile->WriteDouble("PASSPERMIT", "PassPermitRollbacking_Margin", pParamSet->dPassPermitRollbackMargin);


		delete m_lWriteIniFile;
		bResult = true;


	}

    return bResult;
}
/**
@brief   파라미터를 업데이트 하는 함수
@author  임태웅
@date    2013.7.19
@return 성공하면 true 반환, List에 명령이 없어 추가 못하면 false 반환
*/
bool PathSearch::updateParameter()
{
	PATHSEARCH_PARAM_SET* pTmpParamSet;

    if(m_pTmpParamSet!=NULL)
    {
        pTmpParamSet = m_pParamSet;
        m_pParamSet = m_pTmpParamSet;
        m_pTmpParamSet = NULL;
        delete pTmpParamSet;
        return true;
    }
    else
    {
        return false;
    }
}
/**
@brief    UBG Monitoring.ini 파라미터 Load
@author  soming
@date    2018.10.15
@ 직진노드만 가진 파일을 읽어서  m_pUBGParamSet 에 집어 넣는다
@return 성공하면 true 반환, List에 명령이 없어 추가 못하면 false 반환
*/
bool PathSearch::loadUBGCheckParam(char* szFilePath, PATHSEARCH_UBG_CHECK_PARAM_SET* pUBGParamSet)
{

	std::string tmp;
	std::ifstream tmpFile(szFilePath);

	// 초기화
	memset(pUBGParamSet->ArrNode, 0 ,sizeof(pUBGParamSet->ArrNode));
	pUBGParamSet->nNodeCnt = 0;

	if(tmpFile.is_open() == false)
		return false;

	int nCurNodeIdx = 0;

	// 한줄한줄 읽는다.
	// 한줄에 한개 노드가 있다
	while (std::getline(tmpFile, tmp))
	{
		char szLine[62];
		memset(szLine, 0, sizeof(szLine));
		sprintf(szLine, "%s", tmp.c_str());

		int nNode = atoi(szLine);

		if(nCurNodeIdx >= MAX_JIKJIN_NODE_CHECK_CNT-1)
			break;
		if((nNode <= 0) || (nNode > 99999999))
			continue;

		pUBGParamSet->ArrNode[nCurNodeIdx++] = nNode;

	}

	 pUBGParamSet->nNodeCnt = nCurNodeIdx;
    tmpFile.close();

	return true;
}

/**
@brief    UBG Monitoring.ini 파라미터 Load
@author  soming
@date    2018.10.15
@ 직진노드만 가진 파일을 읽어서  m_pUBGParamSet 에 집어 넣는다
@return 성공하면 true 반환, List에 명령이 없어 추가 못하면 false 반환
*/
bool PathSearch::loadNodeLanCutCheckParam(char* szFilePath, PATHSEARCH_NODE_LANCUT_PARAM_SET* pNodeLanCutSet)
{
	std::string tmp;
	std::ifstream tmpFile(szFilePath);

	// 초기화
	memset(pNodeLanCutSet->ArrNode, 0 ,sizeof(pNodeLanCutSet->ArrNode));
	pNodeLanCutSet->nNodeCnt = 0;

	if(tmpFile.is_open() == false)
		return false;

	int nCurNodeIdx = 0;

	// 한줄한줄 읽는다.
	// 한줄에 한개 노드가 있다
	while (std::getline(tmpFile, tmp))
	{
		char szLine[62];
		memset(szLine, 0, sizeof(szLine));
		sprintf(szLine, "%s", tmp.c_str());

		int nNode = atoi(szLine);

		if(nCurNodeIdx >= MAX_JIKJIN_NODE_CHECK_CNT-1)
			break;
		if((nNode <= 0) || (nNode > 99999999))
			continue;

		pNodeLanCutSet->ArrNode[nCurNodeIdx++] = nNode;

	}

	pNodeLanCutSet->nNodeCnt = nCurNodeIdx;
    tmpFile.close();

	return true;
}

/**
@brief   Driving PathSearch 함수, 기존 PathSearch 함수의 경우, Target Node 부터 From Node를 Search 하나, 해당 함수의 경우, From Node 에서 Target Node 탐색
@author  Labeler
@date    2016-12-22
@param   pMap : 사용하는 Map
@param   FromNode : 시작 Node
@param   ToNode : Target Node
@param   AdditionalInfo : 주행 명령 추가 정보
@param   DRIVING_INFO_List : 주행 명령 추가 List
@return  Error Code
*/
int PathSearch::FrontSearchDrivingPath(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	// 0. 값 초기화 ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// 현재 에러 초기화

	MAP		*pTmpMap			= pMap;				        // 사용하는 Map
	UINT	uFromNode			= FromNode;			        // 시작 노드
	UINT	uToNode				= ToNode;			        // 끝 노드
	LList	*pResultAddList	= DRIVING_INFO_List;	        // 주행을 위한 명령을 추가하기 위한 List

	bool	bArrived			= false;	                // Map Seach에 성공한 경우
	UINT	uPathSearchIndex	= 0;		                // PathSearch List의 Index
	UINT	uListInsertIndex	= 0;		                // PathSearch List 삽입시 사용되는 Index
	UINT	uTmpListCount		= 0;		                // PathSearch List 내의 인자 수

	int		nNodeIndex			= 0;		                // Node의 Index
	int		nPathIndex			= 0;		                // Path의 Index
	UINT	uTmpToNode			= 0;		                // Path 단위로 정보 확인 시
	UINT	uTmpFromNode		= 0;		                // Path 단위로 정보 확인 시
	double	dTmpTotalDistance	= 0.0;	                    // 현재 Node까지의 총 가중치(거리/속도)
	double	dTmpPathDistance	= 0.0;	                    // Path의 Link 가중치(거리/속도)

	int		dMaxPathSearchListCnt = 0;
	int		dtmpPathSearchListCnt = 0;

	NODE_INFO	*pNODE_INFO	= NULL;	                        // Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	                        // Path 정보

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search를 위한 Node 구조체
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search를 위한 Node 구조체
	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread에 전달하는 구조체

	LList PathSearchList;									// PathSearch를 위한 List
	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	    // Node 정보를 찾기 위한 Map
	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";

	OHTMainStatus *pOHTMainStatusTmp = OHTMainForm->GetOHTStatus();

	// 1. Path Search 전 정보 Check
	ADD_LOG("[PATHSEARCH_Step 1]");
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_PATHSEARCH_FAIL;
	}

	if(AdditionalInfo.StationType!=MARK_TYPE_NODE)
	{
		nTmpError = CheckPathOfStation(pTmpMap, uFromNode, uToNode, AdditionalInfo);
		if(nTmpError!=NO_ERR)
		{
			return nTmpError;
		}
	}

	ADD_LOG("[PATHSEARCH_Step 1-1]");
	nNodeIndex = pTmpMap->FindNodeIndex(uFromNode);
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);
	if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
	{
		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
		ADD_LOG("[ERR_PATHSEARCH_FAIL_51] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[0]);
		ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return nTmpError;
	}

	uTmpFromNode = uFromNode;
	pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;	// PathSearch Node 정보 생성 : 없을 경우
	pPATH_SEARCH_NODE_INFO->NID = 0;
	pPATH_SEARCH_NODE_INFO->NextNID = uTmpFromNode;
	pPATH_SEARCH_NODE_INFO->PathIndex = INDEX_NONE;
	pPATH_SEARCH_NODE_INFO->dTotalDistance = 0.0;

	PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);	// List 에 삽입
	mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;		// map 에 삽입
	uPathSearchIndex = 0;
	dMaxPathSearchListCnt = 0;

	// 2. Index 순차적으로 증가시키면서 Node 추가 ------------------------------
	//   1) 다음 위치로 Index를 증가시켜가면서 다음 노드들을 PathSarchList에 추가함
	//     . PathSearchList에 추가 할 때는 거리 가중치 반영하여 가까운 순서로 삽입
	//     . 중복되는 Node 가 있을 경우 거리 가중치가 가까운 노드로 삽입, 먼 노드는 삭제
	//   2) 시작 노드에 연결 Path의 개수 만큼 타겟노드가 발견되면 정지
	ADD_LOG("[PATHSEARCH_Step 1-2]");


	try{
		while(bArrived==false)	                // Target Node 나올 경우 bArrived 가 true로 바꿔줌
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex);
			uPathSearchIndex++;

			dtmpPathSearchListCnt = PathSearchList.getCount();
			if(dtmpPathSearchListCnt > dMaxPathSearchListCnt)
			{
				dMaxPathSearchListCnt = dtmpPathSearchListCnt;

				if(dMaxPathSearchListCnt > m_pParamSet->nMapSearchMax)
				{
					nTmpError = ERR_PATHSEARCH_FAIL;
					ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dMaxPathSearchListCnt, m_pParamSet->nMapSearchMax);
					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}
			}

			// 타겟노드 도달 확인
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode)
				{
					bArrived = true;
					break;
				}
			}

			if(pPATH_SEARCH_NODE_INFO != NULL)	                // mapPathSearchNode 에  uTmpToNode 의 PathSearch Node정보가 없으면
			{
				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;
			}
			else
			{
				nTmpError = -1;                                 // PathSearch 실패
				break;
			}

			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			if(nNodeIndex == -1) continue;
			if(m_pNodeLanCutSet != NULL)
			{
				if(m_pNodeLanCutSet->ExistsCheck(uTmpFromNode))
				{
                 	continue;
				}
			}

			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);	// Node 정보

			for(int n=0; n<2; n++)                              // Next Path 개수 만큼 확인
			{
				nPathIndex = pNODE_INFO->NextPathIndex[n];		// Path Index 획득
			   //	if((nPathIndex!=0) && (nPathIndex!=-1))
			   if(nPathIndex!=-1)
				{
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// Path 정보
					dTmpPathDistance = (double)pPATH_INFO->Distance / (double)pPATH_INFO->Speed;	// Link 길이 획득 : 이동 시간 (거리/속도)
					uTmpToNode = pPATH_INFO->EndNID;            // Next Node ID 획득

					IterPos = mapPathSearchNode.find(uTmpToNode);
					if(IterPos == mapPathSearchNode.end())	// mapPathSearchNode 에  uTmpFromNode 의 PathSearch Node정보가 없으면
					{
						// PathSearch Node 정보 생성 : 없을 경우
						pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
						pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
						pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
						pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
						pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

						//추가하는 루틴
						pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.referBack();
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance <= pPATH_SEARCH_NODE_INFO->dTotalDistance)	// 거리가 제일 먼 경우
						{
							PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
							mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
						}
						else	// 중간에 삽입 되는 경우
						{
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
									uListInsertIndex = uTmpListCount;	// while 문 끝냄
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
					else	// mapPathSearchNode 에  uTmpFromNode 의 PathSearch Node정보가 있으면
					{
						pTempPATH_SEARCH_NODE_INFO = (*IterPos).second;
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > dTmpTotalDistance + dTmpPathDistance) // 값이 같거나 클 경우 먼길-->skip
						{
							// PathSearch Node 정보 생성 : 추가할 정보
							pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
							pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
							pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
							pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
							pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

							// 리스트에서 추가 및 기존 값 제거 루틴
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									// 새로운 값 추가
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									// 기존 거리 먼 인자 것 제거
									uTmpListCount = PathSearchList.getCount();
									uListInsertIndex++;
									while(uListInsertIndex<uTmpListCount)
									{
										pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
										if(pTempPATH_SEARCH_NODE_INFO==(*IterPos).second)
										{
											PathSearchList.pop(uListInsertIndex); 	// List 에서 제거
											mapPathSearchNode.erase(IterPos);
											mapPathSearchNode[uTmpToNode]=pPATH_SEARCH_NODE_INFO;
											delete pTempPATH_SEARCH_NODE_INFO;		// 인자 소멸
										}
										uListInsertIndex = uTmpListCount;	// while 문 끝냄
									}
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
				}
			}
		}
	}catch(...)
	{
        return 9999;
    }

	ADD_LOG("[PATHSEARCH_Step 1-3]");
	// 3. Target Node 에서 이전 Node 찾아가면서 Driving List 만듬 ---------------
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) 에 추가
	bArrived		= false;
	uTmpListCount	= PathSearchList.getCount();
	uTmpToNode		= uToNode;

	while(bArrived==false && nTmpError == NO_ERR)
	{
        IterPos = mapPathSearchNode.find(uTmpToNode);

        if( IterPos != mapPathSearchNode.end() )
        {
            pPATH_SEARCH_NODE_INFO = IterPos->second;
        }
        else
        {
        	// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_6]");
			ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
        }

		if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
			(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false)	)
		{
        	// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
			ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		}

		if(pTmpMap->tNodeDisalbeList != NULL)
		{
			for(int j=0 ; j < pTmpMap->tNodeDisalbeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tNodeDisalbeList->Strings[j];

				if((uTmpToNode == StrToIntDef(StrTmp, 0)) || (pPATH_SEARCH_NODE_INFO->NID == StrToIntDef(StrTmp, 0)))
				{
					ADD_LOG("[ERR_NODE_DISABLE] %d:%d:%d", StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}
			}
		}

		if((pTmpMap->tPassBoxNodeList != NULL) && (pOHTMainStatusTmp->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL))
		{
			for(int j=0 ; j < pTmpMap->tPassBoxNodeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tPassBoxNodeList->Strings[j];

				if((uTmpToNode == StrToIntDef(StrTmp, 0)) && ((uTmpToNode != ToNode) || (AdditionalInfo.FinalPortType != GO_FINAL_PORT_TYPE_PassBoxOut)))
				{
					ADD_LOG("[ERR_NODE_DISABLE_BY_PASSBOX_#1] %d:%d:%d:%d:%d:%d", FromNode, ToNode, StrToIntDef(StrTmp, 0), uTmpToNode, pPATH_SEARCH_NODE_INFO->NID,AdditionalInfo.FinalPortType);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/4/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}

				if((pPATH_SEARCH_NODE_INFO->NID == StrToIntDef(StrTmp, 0)) && ((StrToIntDef(StrTmp, 0) != ToNode) || (AdditionalInfo.FinalPortType != GO_FINAL_PORT_TYPE_PassBoxIn)))
				{
					ADD_LOG("[ERR_NODE_DISABLE_BY_PASSBOX_#2] %d:%d:%d:%d:%d:%d", FromNode, ToNode, StrToIntDef(StrTmp, 0), uTmpToNode, pPATH_SEARCH_NODE_INFO->NID,AdditionalInfo.FinalPortType);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/4/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}
			}
		}

		nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
		pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= pPATH_SEARCH_NODE_INFO->NID;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= uTmpToNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
		pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
		pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
		pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
		pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, 분기 정보 추가
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;

		// for change node (jhoun.lim)
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;
#else
		pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, 분기 정보 추가
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;
#endif
		memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
		// MapMake 일 경우 Map Make 속도 반영 : 위치 및 처리 방법에 대해 추가 고민 필요
		if(	(AdditionalInfo.CmdType==EXECUTE_DRIVING_CMD_MAPMAKE) &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
		{
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
		}

		pResultAddList->insertFront((void*)pEXECUTE_DRIVING_INFO);

		uTmpToNode = pPATH_SEARCH_NODE_INFO->NID;

		if(uTmpToNode == uFromNode)
		{
			bArrived = true;
		}
	}

	if(nTmpError != NO_ERR)
	{
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)pResultAddList->popFront();
			delete pEXECUTE_DRIVING_INFO;
		}
	}

	// 4. Path Search List 소멸 ------------------------------------------------
	//   1) 인자들 delete
	//   2) List 소멸
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}

    ADD_LOG("[PATHSEARCH_Step 1-4]");
	return nTmpError;
}




/**
@brief   Driving PathSearch 함수, 기존 PathSearch 함수의 경우, From Node 에서 Target Node 탐색, 분기에서는 Stop
@author  puitng
@date    2019-09-02
@param   pMap : 사용하는 Map
@param   FromNode : 시작 Node
@param   ToNode : Target Node
@param   AdditionalInfo : 주행 명령 추가 정보
@param   DRIVING_INFO_List : 주행 명령 추가 List
@return  Error Code
*/
int PathSearch::FrontSearchDrivingPathMapMake(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List,int &nTargetNode)
{
	// 0. 값 초기화 ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// 현재 에러 초기화

	MAP		*pTmpMap			= pMap;				        // 사용하는 Map
	UINT	uFromNode			= FromNode;			        // 시작 노드
	UINT	uToNode				= ToNode;			        // 끝 노드
	LList	*pResultAddList	= DRIVING_INFO_List;	        // 주행을 위한 명령을 추가하기 위한 List

	bool	bArrived			= false;	                // Map Seach에 성공한 경우
	UINT	uPathSearchIndex	= 0;		                // PathSearch List의 Index
	UINT	uListInsertIndex	= 0;		                // PathSearch List 삽입시 사용되는 Index
	UINT	uTmpListCount		= 0;		                // PathSearch List 내의 인자 수

	int		nNodeIndex			= 0;		                // Node의 Index
	int		nPathIndex			= 0;		                // Path의 Index
	UINT	uTmpToNode			= 0;		                // Path 단위로 정보 확인 시
	UINT	uTmpFromNode		= 0;		                // Path 단위로 정보 확인 시
	double	dTmpTotalDistance	= 0.0;	                    // 현재 Node까지의 총 가중치(거리/속도)
	double	dTmpPathDistance	= 0.0;	                    // Path의 Link 가중치(거리/속도)

	int		dMaxPathSearchListCnt = 0;
	int		dtmpPathSearchListCnt = 0;

	NODE_INFO	*pNODE_INFO	= NULL;	                        // Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	                        // Path 정보

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search를 위한 Node 구조체
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search를 위한 Node 구조체
	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread에 전달하는 구조체

	LList PathSearchList;									// PathSearch를 위한 List
	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	    // Node 정보를 찾기 위한 Map
	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";

	OHTMainStatus *pOHTMainStatusTmp = OHTMainForm->GetOHTStatus();

	nTargetNode = ToNode;

	// 1. Path Search 전 정보 Check
	ADD_LOG("[PATHSEARCH_Step 1]");
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		return ERR_PATHSEARCH_FAIL;
	}


	ADD_LOG("[PATHSEARCH_Step 1-1]");
	nNodeIndex = pTmpMap->FindNodeIndex(uFromNode);
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);
	if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
	{
		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
		ADD_LOG("[ERR_PATHSEARCH_FAIL_51] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[0]);
		return nTmpError;
	}

	uTmpFromNode = uFromNode;
	pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;	// PathSearch Node 정보 생성 : 없을 경우
	pPATH_SEARCH_NODE_INFO->NID = 0;
	pPATH_SEARCH_NODE_INFO->NextNID = uTmpFromNode;
	pPATH_SEARCH_NODE_INFO->PathIndex = INDEX_NONE;
	pPATH_SEARCH_NODE_INFO->dTotalDistance = 0.0;

	PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);	// List 에 삽입
	mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;		// map 에 삽입
	uPathSearchIndex = 0;
	dMaxPathSearchListCnt = 0;

	// 2. Index 순차적으로 증가시키면서 Node 추가 ------------------------------
	//   1) 다음 위치로 Index를 증가시켜가면서 다음 노드들을 PathSarchList에 추가함
	//     . PathSearchList에 추가 할 때는 거리 가중치 반영하여 가까운 순서로 삽입
	//     . 중복되는 Node 가 있을 경우 거리 가중치가 가까운 노드로 삽입, 먼 노드는 삭제
	//   2) 시작 노드에 연결 Path의 개수 만큼 타겟노드가 발견되면 정지
	ADD_LOG("[PATHSEARCH_Step 1-2]");


	try{
		while(bArrived==false)	                // Target Node 나올 경우 bArrived 가 true로 바꿔줌
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex);
			uPathSearchIndex++;

			dtmpPathSearchListCnt = PathSearchList.getCount();
			if(dtmpPathSearchListCnt > dMaxPathSearchListCnt)
			{
				dMaxPathSearchListCnt = dtmpPathSearchListCnt;

				if(dMaxPathSearchListCnt > m_pParamSet->nMapSearchMax)
				{
					nTmpError = ERR_PATHSEARCH_FAIL;
					ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dMaxPathSearchListCnt, m_pParamSet->nMapSearchMax);
					break;
				}
			}

			// 타겟노드 도달 확인
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode)
				{
					bArrived = true;
					break;
				}
			}

//			// 분기노드 도달 확인
//			if(pPATH_SEARCH_NODE_INFO != NULL)
//			{
//				if(uPathSearchIndex >1)
//				{
//					ADD_LOG("[PATHSEARCH_Step 1-2-3: %d %d]", uPathSearchIndex, pPATH_SEARCH_NODE_INFO->NextNID);
//				  if(GetIsNBrenchMapMake(pMap, pPATH_SEARCH_NODE_INFO->NextNID))
//				  {
//					bArrived = true;
//					uToNode = pPATH_SEARCH_NODE_INFO->NextNID;
//					nTargetNode = pPATH_SEARCH_NODE_INFO->NextNID;
//					break;
//                  }
//                }
//
//				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode)
//				{
//					bArrived = true;
//					break;
//				}
//			}

			if(pPATH_SEARCH_NODE_INFO != NULL)	                // mapPathSearchNode 에  uTmpToNode 의 PathSearch Node정보가 없으면
			{
				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;
			}
			else
			{
				nTmpError = -1;                                 // PathSearch 실패
				break;
			}

			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			if(nNodeIndex == -1) continue;
			if(m_pNodeLanCutSet != NULL)
			{
				if(m_pNodeLanCutSet->ExistsCheck(uTmpFromNode))
				{
                 	continue;
				}
			}

			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);	// Node 정보

			for(int n=0; n<2; n++)                              // Next Path 개수 만큼 확인
			{
				nPathIndex = pNODE_INFO->NextPathIndex[n];		// Path Index 획득
			   //	if((nPathIndex!=0) && (nPathIndex!=-1))
			   if(nPathIndex!=-1)
				{
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// Path 정보
					dTmpPathDistance = (double)pPATH_INFO->Distance / (double)pPATH_INFO->Speed;	// Link 길이 획득 : 이동 시간 (거리/속도)
					uTmpToNode = pPATH_INFO->EndNID;            // Next Node ID 획득

					IterPos = mapPathSearchNode.find(uTmpToNode);
					if(IterPos == mapPathSearchNode.end())	// mapPathSearchNode 에  uTmpFromNode 의 PathSearch Node정보가 없으면
					{
						// PathSearch Node 정보 생성 : 없을 경우
						pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
						pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
						pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
						pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
						pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

						//추가하는 루틴
						pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.referBack();
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance <= pPATH_SEARCH_NODE_INFO->dTotalDistance)	// 거리가 제일 먼 경우
						{
							PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
							mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
						}
						else	// 중간에 삽입 되는 경우
						{
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
									uListInsertIndex = uTmpListCount;	// while 문 끝냄
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
					else	// mapPathSearchNode 에  uTmpFromNode 의 PathSearch Node정보가 있으면
					{
						pTempPATH_SEARCH_NODE_INFO = (*IterPos).second;
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > dTmpTotalDistance + dTmpPathDistance) // 값이 같거나 클 경우 먼길-->skip
						{
							// PathSearch Node 정보 생성 : 추가할 정보
							pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
							pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
							pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
							pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
							pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

							// 리스트에서 추가 및 기존 값 제거 루틴
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									// 새로운 값 추가
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									// 기존 거리 먼 인자 것 제거
									uTmpListCount = PathSearchList.getCount();
									uListInsertIndex++;
									while(uListInsertIndex<uTmpListCount)
									{
										pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
										if(pTempPATH_SEARCH_NODE_INFO==(*IterPos).second)
										{
											PathSearchList.pop(uListInsertIndex); 	// List 에서 제거
											mapPathSearchNode.erase(IterPos);
											mapPathSearchNode[uTmpToNode]=pPATH_SEARCH_NODE_INFO;
											delete pTempPATH_SEARCH_NODE_INFO;		// 인자 소멸
										}
										uListInsertIndex = uTmpListCount;	// while 문 끝냄
									}
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
				}
			}
		}
	}catch(...)
	{
        return 9999;
    }

	ADD_LOG("[PATHSEARCH_Step 1-3]");
	// 3. Target Node 에서 이전 Node 찾아가면서 Driving List 만듬 ---------------
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) 에 추가
	bArrived		= false;
	uTmpListCount	= PathSearchList.getCount();
	uTmpToNode		= uToNode;

	while(bArrived==false && nTmpError == NO_ERR)
	{
		IterPos = mapPathSearchNode.find(uTmpToNode);

		if( IterPos != mapPathSearchNode.end() )
		{
			pPATH_SEARCH_NODE_INFO = IterPos->second;
		}
		else
        {
        	// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_6]");
			break;
        }

		if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
			(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false)	)
		{
        	// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
			break;
		}

		if(pTmpMap->tNodeDisalbeList != NULL)
		{
			for(int j=0 ; j < pTmpMap->tNodeDisalbeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tNodeDisalbeList->Strings[j];

				if((uTmpToNode == StrToIntDef(StrTmp, 0)) || (pPATH_SEARCH_NODE_INFO->NID == StrToIntDef(StrTmp, 0)))
				{
					ADD_LOG("[ERR_NODE_DISABLE] %d:%d:%d", StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID);
					nTmpError = ERR_NODE_DISABLE;
					break;
				}
			}
		}

		nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
		pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= pPATH_SEARCH_NODE_INFO->NID;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= uTmpToNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
		pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
		pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
		pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
		pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, 분기 정보 추가
		pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;

		// for change node (jhoun.lim)
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;
#else
		pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, 분기 정보 추가
		pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;
#endif
		memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
		// MapMake 일 경우 Map Make 속도 반영 : 위치 및 처리 방법에 대해 추가 고민 필요
		if(	(AdditionalInfo.CmdType==EXECUTE_DRIVING_CMD_MAPMAKE) &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
		{
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
		}

		pResultAddList->insertFront((void*)pEXECUTE_DRIVING_INFO);

		//다음으로
		uTmpToNode = pPATH_SEARCH_NODE_INFO->NID;

		IterPos = mapPathSearchNode.find(uTmpToNode);
		if( IterPos != mapPathSearchNode.end() )
		{
			pPATH_SEARCH_NODE_INFO = IterPos->second;
		}
		else
        {
        	// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_6]");
			break;
		}

		UINT uTmpFromNode 	= pPATH_SEARCH_NODE_INFO->NID;

		//분기구간 확인 뒤에서부터 확인
		if((uTmpToNode !=uToNode) && (uTmpToNode != uFromNode))
		{
			  if(GetIsNBrenchMapMake(pMap, uTmpFromNode, uTmpToNode))
			  {

				   ADD_LOG("[PATHSEARCH_Step 1-2-3: %d]", uTmpToNode);
				   nTargetNode = uTmpToNode;

			  }
		}


		if(uTmpToNode == uFromNode)
		{
			bArrived = true;
		}
	}

	if(nTmpError != NO_ERR)
	{
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)pResultAddList->popFront();
			delete pEXECUTE_DRIVING_INFO;
		}
	}else
	{
		if(nTargetNode !=uToNode)
		{
		 	uTmpListCount = pResultAddList->getCount();
			for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
			{
				pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)pResultAddList->popBack();
				delete pEXECUTE_DRIVING_INFO;

				if(pEXECUTE_DRIVING_INFO->BasicInfo.StartNID  ==nTargetNode)
                  	break;
			}
		}
	}

	// 4. Path Search List 소멸 ------------------------------------------------
	//   1) 인자들 delete
	//   2) List 소멸
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}

    ADD_LOG("[PATHSEARCH_Step 1-4]");
	return nTmpError;
}


/**
@brief   Driving PathSearch 함수, 기존 PathSearch 함수의 경우, Target Node 부터 From Node를 Search 하나, 해당 함수의 경우, From Node 에서 Target Node 탐색
@author  puting
@date    2016-12-22
@param   pMap : 사용하는 Map
@param   FromNode : 시작 Node
@param   ToNode : Target Node
@param   AdditionalInfo : 주행 명령 추가 정보
@param   DRIVING_INFO_List : 주행 명령 추가 List
@return  Error Code
*/
int PathSearch::FrontSearchDrivingPathAutoMoveTest(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	// 0. 값 초기화 ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// 현재 에러 초기화

	MAP		*pTmpMap			= pMap;				        // 사용하는 Map
	UINT	uFromNode			= FromNode;			        // 시작 노드
	UINT	uToNode				= ToNode;			        // 끝 노드
	LList	*pResultAddList	= DRIVING_INFO_List;	        // 주행을 위한 명령을 추가하기 위한 List

	bool	bArrived			= false;	                // Map Seach에 성공한 경우
	UINT	uPathSearchIndex	= 0;		                // PathSearch List의 Index
	UINT	uListInsertIndex	= 0;		                // PathSearch List 삽입시 사용되는 Index
	UINT	uTmpListCount		= 0;		                // PathSearch List 내의 인자 수

	int		nNodeIndex			= 0;		                // Node의 Index
	int		nPathIndex			= 0;		                // Path의 Index
	UINT	uTmpToNode			= 0;		                // Path 단위로 정보 확인 시
	UINT	uTmpFromNode		= 0;		                // Path 단위로 정보 확인 시
	double	dTmpTotalDistance	= 0.0;	                    // 현재 Node까지의 총 가중치(거리/속도)
	double	dTmpPathDistance	= 0.0;	                    // Path의 Link 가중치(거리/속도)

	int		dMaxPathSearchListCnt = 0;
	int		dtmpPathSearchListCnt = 0;

	NODE_INFO	*pNODE_INFO	= NULL;	                        // Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	                        // Path 정보

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search를 위한 Node 구조체
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search를 위한 Node 구조체
	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread에 전달하는 구조체

	LList PathSearchList;									// PathSearch를 위한 List
	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	    // Node 정보를 찾기 위한 Map
	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";

	OHTMainStatus *pOHTMainStatusTmp = OHTMainForm->GetOHTStatus();

	// 1. Path Search 전 정보 Check
	ADD_LOG("[PATHSEARCH_Step 1]");
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		return ERR_PATHSEARCH_FAIL;
	}

	if(AdditionalInfo.StationType!=MARK_TYPE_NODE)
	{
		nTmpError = CheckPathOfStation(pTmpMap, uFromNode, uToNode, AdditionalInfo);
		if(nTmpError!=NO_ERR)
		{
			return nTmpError;
		}
	}

	ADD_LOG("[PATHSEARCH_Step 1-1]");
	nNodeIndex = pTmpMap->FindNodeIndex(uFromNode);
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);
	if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
	{
		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
		ADD_LOG("[ERR_PATHSEARCH_FAIL_51] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[0]);
		return nTmpError;
	}

	uTmpFromNode = uFromNode;
	pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;	// PathSearch Node 정보 생성 : 없을 경우
	pPATH_SEARCH_NODE_INFO->NID = 0;
	pPATH_SEARCH_NODE_INFO->NextNID = uTmpFromNode;
	pPATH_SEARCH_NODE_INFO->PathIndex = INDEX_NONE;
	pPATH_SEARCH_NODE_INFO->dTotalDistance = 0.0;

	PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);	// List 에 삽입
	mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;		// map 에 삽입
	uPathSearchIndex = 0;
	dMaxPathSearchListCnt = 0;

	// 2. Index 순차적으로 증가시키면서 Node 추가 ------------------------------
	//   1) 다음 위치로 Index를 증가시켜가면서 다음 노드들을 PathSarchList에 추가함
	//     . PathSearchList에 추가 할 때는 거리 가중치 반영하여 가까운 순서로 삽입
	//     . 중복되는 Node 가 있을 경우 거리 가중치가 가까운 노드로 삽입, 먼 노드는 삭제
	//   2) 시작 노드에 연결 Path의 개수 만큼 타겟노드가 발견되면 정지
	ADD_LOG("[PATHSEARCH_Step 1-2]");


	try{
		while(bArrived==false)	                // Target Node 나올 경우 bArrived 가 true로 바꿔줌
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex);
			uPathSearchIndex++;

			dtmpPathSearchListCnt = PathSearchList.getCount();
			if(dtmpPathSearchListCnt > dMaxPathSearchListCnt)
			{
				dMaxPathSearchListCnt = dtmpPathSearchListCnt;

				if(dMaxPathSearchListCnt > m_pParamSet->nMapSearchMax)
				{
					nTmpError = ERR_PATHSEARCH_FAIL;
					ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dMaxPathSearchListCnt, m_pParamSet->nMapSearchMax);
					break;
				}
			}

			// 타겟노드 도달 확인
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode)
				{
					bArrived = true;
					break;
				}
			}

			if(pPATH_SEARCH_NODE_INFO != NULL)	                // mapPathSearchNode 에  uTmpToNode 의 PathSearch Node정보가 없으면
			{
				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;
			}
			else
			{
				nTmpError = -1;                                 // PathSearch 실패
				break;
			}

			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index 획득
			if(nNodeIndex == -1) continue;
			if(m_pNodeLanCutSet != NULL)
			{
				if(m_pNodeLanCutSet->ExistsCheck(uTmpFromNode))
				{
                 	continue;
				}
			}

			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);	// Node 정보

			for(int n=0; n<2; n++)                              // Next Path 개수 만큼 확인
			{
				nPathIndex = pNODE_INFO->NextPathIndex[n];		// Path Index 획득
			   //	if((nPathIndex!=0) && (nPathIndex!=-1))
			   if(nPathIndex!=-1)
				{
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// Path 정보
					dTmpPathDistance = (double)pPATH_INFO->Distance / (double)pPATH_INFO->Speed;	// Link 길이 획득 : 이동 시간 (거리/속도)
					uTmpToNode = pPATH_INFO->EndNID;            // Next Node ID 획득

					IterPos = mapPathSearchNode.find(uTmpToNode);
					if(IterPos == mapPathSearchNode.end())	// mapPathSearchNode 에  uTmpFromNode 의 PathSearch Node정보가 없으면
					{
						// PathSearch Node 정보 생성 : 없을 경우
						pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
						pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
						pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
						pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
						pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

						//추가하는 루틴
						pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.referBack();
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance <= pPATH_SEARCH_NODE_INFO->dTotalDistance)	// 거리가 제일 먼 경우
						{
							PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
							mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
						}
						else	// 중간에 삽입 되는 경우
						{
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
									uListInsertIndex = uTmpListCount;	// while 문 끝냄
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
					else	// mapPathSearchNode 에  uTmpFromNode 의 PathSearch Node정보가 있으면
					{
						pTempPATH_SEARCH_NODE_INFO = (*IterPos).second;
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > dTmpTotalDistance + dTmpPathDistance) // 값이 같거나 클 경우 먼길-->skip
						{
							// PathSearch Node 정보 생성 : 추가할 정보
							pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
							pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
							pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
							pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
							pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

							// 리스트에서 추가 및 기존 값 제거 루틴
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									// 새로운 값 추가
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									// 기존 거리 먼 인자 것 제거
									uTmpListCount = PathSearchList.getCount();
									uListInsertIndex++;
									while(uListInsertIndex<uTmpListCount)
									{
										pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
										if(pTempPATH_SEARCH_NODE_INFO==(*IterPos).second)
										{
											PathSearchList.pop(uListInsertIndex); 	// List 에서 제거
											mapPathSearchNode.erase(IterPos);
											mapPathSearchNode[uTmpToNode]=pPATH_SEARCH_NODE_INFO;
											delete pTempPATH_SEARCH_NODE_INFO;		// 인자 소멸
										}
										uListInsertIndex = uTmpListCount;	// while 문 끝냄
									}
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
				}
			}
		}
	}catch(...)
	{
        return 9999;
    }

	ADD_LOG("[PATHSEARCH_Step 1-3]");
	// 3. Target Node 에서 이전 Node 찾아가면서 Driving List 만듬 ---------------
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) 에 추가
	bArrived		= false;
	uTmpListCount	= PathSearchList.getCount();
	uTmpToNode		= uToNode;

	while(bArrived==false && nTmpError == NO_ERR)
	{
        IterPos = mapPathSearchNode.find(uTmpToNode);

        if( IterPos != mapPathSearchNode.end() )
        {
            pPATH_SEARCH_NODE_INFO = IterPos->second;
        }
        else
        {
        	// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_6]");
			break;
        }

		if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
			(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false)	)
		{
        	// 아래가 실행 될 확률은 0% 임 : Path Search가 완료 되면 아래 에러 발생 확률 없음. 논리적으로 공백을 매우기 위한 코드
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch 실패
			ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
			break;
		}

		if(pTmpMap->tNodeDisalbeList != NULL)
		{
			for(int j=0 ; j < pTmpMap->tNodeDisalbeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tNodeDisalbeList->Strings[j];

				if((uTmpToNode == StrToIntDef(StrTmp, 0)) || (pPATH_SEARCH_NODE_INFO->NID == StrToIntDef(StrTmp, 0)))
				{
					ADD_LOG("[ERR_NODE_DISABLE] %d:%d:%d", StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID);
					nTmpError = ERR_NODE_DISABLE;
					break;
				}
			}
		}


		nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
		pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= pPATH_SEARCH_NODE_INFO->NID;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= uTmpToNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
		pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
		pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
		pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
		pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, 분기 정보 추가
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;

		// for change node (jhoun.lim)
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;
#else
		pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, 분기 정보 추가
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;
#endif
		memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
		// MapMake 일 경우 Map Make 속도 반영 : 위치 및 처리 방법에 대해 추가 고민 필요
		if(	(AdditionalInfo.CmdType==EXECUTE_DRIVING_CMD_MAPMAKE) &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
		{
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
		}

		pResultAddList->insertFront((void*)pEXECUTE_DRIVING_INFO);

		uTmpToNode = pPATH_SEARCH_NODE_INFO->NID;

		if(uTmpToNode == uFromNode)
		{
			bArrived = true;
		}
	}

	if(nTmpError != NO_ERR)
	{
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)pResultAddList->popFront();
			delete pEXECUTE_DRIVING_INFO;
		}
	}

	// 4. Path Search List 소멸 ------------------------------------------------
	//   1) 인자들 delete
	//   2) List 소멸
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}

    ADD_LOG("[PATHSEARCH_Step 1-4]");
	return nTmpError;
}

/**
@brief   주행 경로를 기본 분기(좌측) 외 우측으로 이동해야 하는 경우 탐색 함수
@author  Labeler
@date    2016-12-22
@return 성공하면 NO_ERR 반환, 실패 시 Error 반환
*/
int PathSearch::SearchExceptPathForV80(MAP * pMap, UINT FromNode, UINT ToNode, EXCEPT_PATH_LIST_V80 *pPath,int &TargetNode, BYTE MapMakeBrench)
{
	int nError = NO_ERR;
	int nCount = 0;

	int nTempTargetNode = 0;
    LList DRIVING_INFO_List;
	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO	= NULL;	                    // List에 저장하는 구조체
    EXECUTE_DRIVING_ADD_INFO DrivingAdditionalInfo;                         // 주행 추가 정보

    memset(&DrivingAdditionalInfo, 0, sizeof(DrivingAdditionalInfo));       // 추가 주행 정보 초기화

    DrivingAdditionalInfo.CmdType = EXECUTE_DRIVING_CMD_GO; 				// 명령 타입
    DrivingAdditionalInfo.FinalNode = ToNode; 								// 현재 받은 타겟 노드
    DrivingAdditionalInfo.StationType = TAG_TYPE_NONE; 						// 노드 Type
    DrivingAdditionalInfo.FinalPortType = GO_FINAL_PORT_TYPE_NONE;

	loadNodeLanCutCheckParam("DataFiles\\LanCutNodes.dat", m_pNodeLanCutSet);

	if(MapMakeBrench == 0x00)
	{
		nError = FrontSearchDrivingPath(pMap, FromNode, ToNode, DrivingAdditionalInfo ,&DRIVING_INFO_List);
    }
	else if(MapMakeBrench == 0x01)
	{
		nError = FrontSearchDrivingPathMapMake(pMap, FromNode, ToNode, DrivingAdditionalInfo ,&DRIVING_INFO_List,nTempTargetNode);
		TargetNode = nTempTargetNode;
	}
	else if(MapMakeBrench == 0x02)
	{
		nError = FrontSearchDrivingPathAutoMoveTest(pMap, FromNode, ToNode, DrivingAdditionalInfo ,&DRIVING_INFO_List);
    	//이동테스트
	}
	else if(MapMakeBrench == 0x10)      //HJH Test Code
	{
		if(!FindTheWay(pMap, FromNode, ToNode, DrivingAdditionalInfo ,&DRIVING_INFO_List))
		{
			nError = ERR_PATHSEARCH_FAIL;
		}

	}

    if(nError == NO_ERR)
    {
        nCount = DRIVING_INFO_List.getCount();

		for(int i  = 0 ; i < nCount ; i++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)DRIVING_INFO_List.popFront();
			if(pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo == PATH_DIVERGENCE_RIGHT)
			{
				ADD_LOG("ExceptNode :%d",pEXECUTE_DRIVING_INFO->BasicInfo.StartNID);
				pPath->ExceptNode[pPath->nCnt] = pEXECUTE_DRIVING_INFO->BasicInfo.StartNID;
				pPath->nCnt++;
			}
			delete pEXECUTE_DRIVING_INFO;
		}
    }
    return nError;
}
int PathSearch::SearchExceptPathForV80_AutoSearch(MAP * pMap,  PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE_REQ PACKET_BODY, EXCEPT_PATH_LIST_V80 *pPath)
{
	int nError = NO_ERR;
	int nCount = 0;

	int nTempTargetNode = 0;
    LList DRIVING_INFO_List;
	EXECUTE_DRIVING_INFO 	*pEXECUTE_DRIVING_INFO	= NULL;	                // List에 저장하는 구조체
	EXECUTE_DRIVING_ADD_INFO 	DrivingAdditionalInfo;                      // 주행 추가 정보

	memset(&DrivingAdditionalInfo, 0, sizeof(DrivingAdditionalInfo));       // 추가 주행 정보 초기화

	DrivingAdditionalInfo.CmdType 		= EXECUTE_DRIVING_CMD_GO; 				// 명령 타입
	DrivingAdditionalInfo.FinalNode 	= PACKET_BODY.EndID; 					// 현재 받은 타겟 노드
	DrivingAdditionalInfo.StationType	= TAG_TYPE_NONE; 						// 노드 Type
    DrivingAdditionalInfo.FinalPortType = GO_FINAL_PORT_TYPE_NONE;

	if(!RouteCheckNode(pMap, PACKET_BODY, DrivingAdditionalInfo, &DRIVING_INFO_List))
	{
    	nError = ERR_PATHSEARCH_FAIL;
	}


    if(nError == NO_ERR)
    {
        nCount = DRIVING_INFO_List.getCount();

		for(int i  = 0 ; i < nCount ; i++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)DRIVING_INFO_List.popFront();
			if(pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo == PATH_DIVERGENCE_RIGHT)
			{
				ADD_LOG("ExceptNode :%d",pEXECUTE_DRIVING_INFO->BasicInfo.StartNID);
				pPath->ExceptNode[pPath->nCnt] = pEXECUTE_DRIVING_INFO->BasicInfo.StartNID;
				pPath->nCnt++;
			}
			delete pEXECUTE_DRIVING_INFO;
		}
    }
    return nError;
}

int PathSearch::GetIncreaseSQNumber()
{
	return GetIncreaseSeqenceNumber();
}

/**
@brief   Node와 Link Index 정보 확인하는 함수
@author  puting
@date    2017.08.16
@return  이상유무를 리턴함. 이상무 false, 이상 true
*/
bool PathSearch::CheckNodeAndLinkIndex(MAP * pMap, UINT FromNode, UINT ToNode)
{
	bool breturn = false;

	MAP		*pTmpMap			= pMap;				// 사용하는 Map

	int		nFromNodeIndex	  = 0;		// FromNode의 Index
	int     nToNodeIndex      = 0;      // ToNode의 Index

	NODE_INFO	*pNODE_INFO			= NULL;	// Node 정보
	NODE_INFO	*pEndNODE_INFO		= NULL;	// Node 정보
	//1) Node Index Check 부분
	if(	(pTmpMap->FindNodeIndex(FromNode)	==	INDEX_NONE)
	||	(pTmpMap->FindNodeIndex(ToNode)	==	INDEX_NONE)
	)
	{
		ADD_LOG("[Node:CheckNodeAndLinkIndex] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(FromNode), pTmpMap->FindNodeIndex(ToNode), FromNode, ToNode);

		return true;

	}

	try
	{
	//2) Link Index Check 부분
		nFromNodeIndex = pTmpMap->FindNodeIndex(FromNode);   ///< 출발 노드 Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nFromNodeIndex]);  ///< 출발 노드 정보

		nToNodeIndex = pTmpMap->FindNodeIndex(ToNode);   ///< 출발 노드 Index
		pEndNODE_INFO = &(pTmpMap->m_NodeArray[nToNodeIndex]);  ///< 출발 노드 정보

		if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			ADD_LOG("[Link From:CheckNodeAndLinkIndex] %d:%d:%d:%d"
			,pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1],FromNode,nFromNodeIndex);

			breturn = true;
		}
		else if(pEndNODE_INFO->NextPathIndex[0]==INDEX_NONE && pEndNODE_INFO->NextPathIndex[1]==INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			ADD_LOG("[Link From:CheckNodeAndLinkIndex] %d:%d:%d:%d"
			,pEndNODE_INFO->NextPathIndex[0], pEndNODE_INFO->NextPathIndex[1],ToNode,nToNodeIndex);

			breturn = true;
		}
	}catch(...)
       	ADD_LOG("[CheckNodeAndLinkIndex] Exception");

    return breturn;
}


/**
@brief   End Staion의 정보 확인하는 함수
@author  puting
@date    2017.08.16
@return  이상유무를 리턴함. 이상무 false, 이상 true
*/
bool PathSearch::CheckEndStationOffset(MAP * pMap, UINT ParentNode, UINT NextNode, int EndStationOffset)
{

	bool breturn = false;

	MAP		*pTmpMap			= pMap;				// 사용하는 Map

	int		nFromNodeIndex	  = 0;		// FromNode의 Index
	int     nToNodeIndex      = 0;      // ToNode의 Index

	UINT	nPathIndex			= 0;		// Path의 Index

	NODE_INFO	*pNODE_INFO			= NULL;	// Node 정보
	PATH_INFO	*pPATH_INFO	= NULL;	// Path 정보

	//1) Node Index Check 부분
	if(	(pTmpMap->FindNodeIndex(ParentNode)	==	INDEX_NONE)
	||	(pTmpMap->FindNodeIndex(NextNode)	==	INDEX_NONE)
	)
	{
		ADD_LOG("[Node:CheckEndStationOffset] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(ParentNode), pTmpMap->FindNodeIndex(NextNode), ParentNode, NextNode);

		return true;

	}
	try
	{
		//2) Link Index Check 부분
		nFromNodeIndex = pTmpMap->FindNodeIndex(ParentNode);   ///< 출발 노드 Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nFromNodeIndex]);  ///< 출발 노드 정보


		if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// 시작 노드에 Path 연결되지 않은 경우
		{
			ADD_LOG("[Link From:CheckEndStationOffset] %d:%d:%d:%d"
			,pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1],ParentNode,nFromNodeIndex);

			return true;
		}

		//3) NexeNode 확인하는 부분
		breturn = true; //찾을 경우 false로 변환
		for(int i=0; i<2;i++)
		{
			nPathIndex	= pNODE_INFO->NextPathIndex[i];
			if(nPathIndex!=INDEX_NONE)
			{
				pPATH_INFO	= &(pMap->m_PathArray[nPathIndex]);
				if(pPATH_INFO->EndNID==NextNode)
				{
					breturn	= false;
					break;
				}
			}
		}


		//3) Station Offset 값 확인하는 부분
		if(breturn ==false)
		{
				//1 3. Station으로의 거리가 Link 사이의 거리보다 클경우
			if(	EndStationOffset > pPATH_INFO->Distance ||
				EndStationOffset < 0)
			{
				breturn = true;
				ADD_LOG("[StationOffset:CheckEndStationOffset] %d:%d"
					, EndStationOffset
					, pPATH_INFO->Distance);
			}
		}
    }catch(...)
		ADD_LOG("[CheckNodeAndLinkIndex] Exception");

	return breturn;
}

int PathSearch::ChangePathVelocity(void)
{
	/*
		기본적으로 연속된 N구간은 한개의 속도로 MAP이 구성되야 하며
		예외로 CID에 의해 3.3 으로 MAP 속도가 제한되어 있는 경우가 있으므로
		해당 구간을 고려 해줘야 함.
		CID의 의한 속도 변화 구간 전에 조향 완료 된 것으로 속도 계산
	*/
	int nError = NO_ERR;
	//LList *m_PathList = OHTMainForm->GetPathList();

	int cntPath = GetPathCount();
	int pathIndex;
	int tempIndex;

	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO = NULL;
	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO_TEMP = NULL;

	PATH_STEERING_INFO StartSteering;	// 첫번째조향
	PATH_STEERING_INFO EndSteering;   // 두번째조향

	double dDefaultDecel;
	double dDefaultAccel;
	double dVelNew;
	double dVelOri;
	double dVelTemp;

	double dVelBefore;
	double dVelAfter;

	double dVelIn;
	double dVelOut;
	double dDisSum; // 조향에서 다음 조향까지 누적 거리
	int nNodeCnt;

	bool bVelChangeSkipFlag;

	if(cntPath == 1)
	{
		//ADD_LOG("[Test] cntPath = 1");
		return nError; // Pathserch 결과 link 하나의 경우 skip
	}
	if(m_pDrivingControl == NULL) m_pDrivingControl = OHTMainForm->GetDrivingControl();

	if(m_pDrivingControl != NULL){

		dDefaultAccel = m_pDrivingControl->GetDefaultAccel()*1000;
		dDefaultDecel = m_pDrivingControl->GetDefaultDecel()*1000;
	}
	else{
		dDefaultAccel = DEFAULT_ACCEL;
		dDefaultDecel = DEFAULT_DECEL;
	}

	dDisSum = 0;
	nNodeCnt = 0;
	dVelBefore = 0;

	// 1. 조향이 N - N - R - N - N - N - L - N - N 일 때 R과 L 과 같은 조향 사이에서 N 정보만 빼는 작업
	// 2. N 구간에서 속도가 연속되지 않은 경우 skip (CID에 의한 속도 제한 등)

	for(pathIndex=0; pathIndex < cntPath - 2; pathIndex++) // // AOHC-230 Map 속도 차이 E23xx 개선
	{
		pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO *)m_PathList->refer(pathIndex);
		dVelAfter  = pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed;
		if(pathIndex == 0) StartSteering = pEXECUTE_DRIVING_INFO->BasicInfo.Steering;

		if(pEXECUTE_DRIVING_INFO->BasicInfo.Steering == STEERING_NONE)
		{
			if(nNodeCnt == 0) // N구간 새로 시작
			{
				dVelIn = dVelBefore;
			}

			if(pathIndex == 0) // pathsearch 처음
			{
				dDisSum = pEXECUTE_DRIVING_INFO->ChangeEndNodeDistance;
				dVelIn = dVelBefore = 0;  // 1번째 node의 경우 멈춰 있으므로 속도 0
				//dVelIn = dVelBefore = pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed;  // 1번째 node를 속도 0으로 안하려면 이코드 사용
			}
			else
			{
				dDisSum += pEXECUTE_DRIVING_INFO->BasicInfo.dLength;
				dVelBefore = dVelAfter;
			}

			if(pathIndex != cntPath - 3) { // AOHC-230 Map 속도 차이 E23xx 개선
				nNodeCnt++; // N구간 count 증가
				continue; // 마지막 node가 아닌 경우
			}
		}

		EndSteering = pEXECUTE_DRIVING_INFO->BasicInfo.Steering;
		dVelOut = dVelAfter; // dVelBefore <  dVelAfter ? dVelBefore :	dVelAfter;
         		// add by jhoun.lim
		if(pathIndex == cntPath - 3 && pEXECUTE_DRIVING_INFO->BasicInfo.Steering == STEERING_NONE)
		{
			// get next path
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath - 2);
			EndSteering = pEXECUTE_DRIVING_INFO->BasicInfo.Steering;
			dVelOut  = pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed;
			if(EndSteering != STEERING_NONE)
			{
				pathIndex++;
				nNodeCnt++;
			}
		}

		//==========================================================================================
		// CID, SLOPE, LIFT 에 의해 의도적인 속도 제한 등에 의해
		// 연속된 N구간에서 속도가 변하는 부분이 있는지 확인
		// 바뀌는 구간은 수정 없이 Skip
		// N구간이 2개 이상인 경우 만 수행
		bVelChangeSkipFlag = false;
		if(nNodeCnt >= MIN_CNT_OF_VEL_CHANGE)
		{
			if(EndSteering == STEERING_NONE) tempIndex = LAST_INDEX_OF_PATH; // pathsearch의 마지막 node의 경우
			else tempIndex = LAST_INDEX_OF_NORMAL_LINK;

			if((0 <= (pathIndex - tempIndex)) && (cntPath > (pathIndex - tempIndex)) && (cntPath > (pathIndex - nNodeCnt))){ // 방어코드
				pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(pathIndex - tempIndex++);
				dVelTemp = pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed;

				for(; tempIndex<=nNodeCnt; tempIndex++){
					pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(pathIndex - tempIndex);
					dVelOri  = pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed;
					if(dVelOri != dVelTemp) {
						//ADD_LOG("[Test] VelChange = true");
						bVelChangeSkipFlag = true;
						break;
					}
				}
			}
		}
		//==========================================================================================


		//==========================================================================================
		// IN, OUT 속도, 연속된 N구간 거리, 조향 변경 여부를 통해 속도 계산 및 변경 속도 적용
		if((nNodeCnt != 0 || EndSteering ==STEERING_NONE) && (bVelChangeSkipFlag == false))// if(연속 조향 아닌 경우 || 마지막 link 인 경우 && 연속된 N구간에서 속도 변화 없는 경우)
		{
			if(((StartSteering == STEERING_LEFT) && (EndSteering == STEERING_RIGHT)) ||
				((StartSteering == STEERING_LEFT) && (EndSteering == STEERING_N_BRANCH_R2L)) ||
				((StartSteering == STEERING_N_BRANCH_L2R) && (EndSteering == STEERING_LEFT)) ||
				((StartSteering == STEERING_N_BRANCH_L2R) && (EndSteering == STEERING_N_BRANCH_L2R)) ||
				((StartSteering == STEERING_RIGHT) && (EndSteering == STEERING_LEFT)) ||
				((StartSteering == STEERING_RIGHT) && (EndSteering == STEERING_N_BRANCH_L2R)) ||
				((StartSteering == STEERING_N_BRANCH_R2L) && (EndSteering == STEERING_RIGHT)) ||
				((StartSteering == STEERING_N_BRANCH_R2L) && (EndSteering == STEERING_N_BRANCH_R2L)))
			{
				// 변경될 속도 계산, 조향 다른 경우
				dVelNew = GetNewVelocity(dVelIn, dVelOut, dDefaultAccel, dDefaultDecel, dDisSum, true);
			}
			else 
			{
				// 변경될 속도 계산, 조향 같은 경우
				dVelNew = GetNewVelocity(dVelIn, dVelOut, dDefaultAccel, dDefaultDecel, dDisSum, false);
			}

			if(EndSteering == STEERING_NONE) tempIndex = LAST_INDEX_OF_PATH; // pathsearch의 마지막 node의 경우 속도 변경에 포함
			else tempIndex = LAST_INDEX_OF_NORMAL_LINK;
			for(; tempIndex<=nNodeCnt; tempIndex++){
				pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(pathIndex - tempIndex);
				dVelOri  = pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed;
				//pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed = dVelNew;

				if(dVelNew < dVelOri) {
					//ADD_LOG("[MapVelOptim] Map speed change, NodeID:%d, AsIs:%.2f, ToBe:%.2f", pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.StartNID,
					//	pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed, dVelNew);
					pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed = dVelNew; // 변경된 속도 적용
				}
				else{
					ADD_LOG("[MVO]Map speed change x/N:%d,MapS:%.2f,CalS:%.2f", pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.StartNID,
						pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed, dVelNew);
					break;
				}
			}
			if(dVelNew < dVelOri) {
				ADD_LOG("[MVO] Map speed change/N:%d,AsIs:%.2f,ToBe:%.2f,nNC:%d", pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.StartNID,
					dVelOri, dVelNew, nNodeCnt);
			}
		}

		// 거리, Node count, 초기화
		dDisSum = 0;
		nNodeCnt = 0;
		dVelBefore = dVelAfter;
		StartSteering = pEXECUTE_DRIVING_INFO->BasicInfo.Steering;

		// N분기를 지나간 후에는 조향은 반대로 바뀌므로 StartSteering를 변경
		if(StartSteering == STEERING_N_BRANCH_L2R) StartSteering = STEERING_RIGHT;
		else if(StartSteering == STEERING_N_BRANCH_R2L) StartSteering = STEERING_LEFT;
	}

	// AOHC-230 Map 속도 차이 E23xx 개선
	// cntPath - 1, cntPath 비교하여 작은 값 사용
	if ((EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath-1) != NULL) //방어 코드
	{
		double dVelBeforLast = 0;
		double dVelLast = 0;

		pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath - 2);
		dVelBeforLast = pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed;

		pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath-1);
		dVelLast = pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed;

		if (dVelBeforLast != 0 && dVelLast != 0) //방어 코드
		{
			if (dVelBeforLast > dVelLast)
			{
				pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath - 2);
				pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed = dVelLast;
			}
			else
			{
				pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath-1);
				pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed = dVelBeforLast;
			}
		}
	}
	return nError;
}


double PathSearch::GetNewVelocity(double dVelIn, double dVelOut, double dDefaultAccel, double dDefaultDecel, double dDistanceSum, bool bSteeringChange)
{
	double dDistanceDecel = 0.0;
	
	double dVelCalculated;
	double distance = dDistanceSum / 1000;

	double dStopTime;
	double dSpeedDownTime;

	double dDistanceAccel = 0.0;

	double defaultAccelDistance;
	double defaultAccelTime;

	// steering 변화가 있는 경우

	/******************************************************************************
	1. 조향 변경 시간 >= 감속 시작 시간 (증속 불가, 등속 시간 = 조향 변경 시간) 
	  → 구간 속도 = 이전 속도 이하로 계산 (Case1)
	2. 감속 시작 시간 <= 조향 변경 시간 (증속 가능, 증속 시간 + 등속 시간 = 조향 변경 시간)
	 1) (이전 속도 < 3.3m/s) && (구간 속도 < 3.3m/s) → 조향 변경 시간을 감속 시작 시간으로 하는 최대 속도로 계산 (Case2-1)
	 2) (이전 속도 < 3.3m/s) && (구간 속도 > 3.3m/s) → Case2와 같음, 3.3m/s 이상부터 가속도 1.0 사용 (Case2-2) 
	 3) (이전 속도 > 3.3m/s) → Case2와 같음, 가속도 1.0 사용 (Case2-3) 
	3. 감속 시작 시간 > 조향 변경 시간 (증속 가능, 등속 시간 = 등속 시간) 
	 1) (이전 속도 < 3.3m/s) && (구간 속도 < 3.3m/s) → 구간 속도 사용 (Case3-1)
	 2) (이전 속도 < 3.3m/s) && (구간 속도 > 3.3m/s) → Case5와 같음, 3.3m/s 이상부터 가속도 1.0 사용 (Case3-2)
	 3) (이전 속도 > 3.3m/s) → Case5와 같음, 가속도 1.0 사용 (Case3-3)
	******************************************************************************/
	if(bSteeringChange == true){

		dStopTime = dVelIn/dDefaultDecel;
		///dDistanceDecel = GET_DECEL_DIST(dVelIn, 0, dDefaultDecel);
		double dMinStrChangeDis = ((dStopTime+TIME_OF_STEERING_CHANGE+TIME_OF_STEERING_CHANGE)* dVelIn)/2;  // 등속시간이 보장 되는지 확인

		if(dMinStrChangeDis > distance) 
		{
			// Steering change 이 보장 되지 않는 경우 Vin을 조정 (Vin == Vmax, steering change time == 등속시간)
			// 가속 없이 진입속도 정지거리 사다리꼴 계산
			ADD_LOG("[속도변경] 조향변경1");
			dVelCalculated = abs((-1.0*dDefaultDecel*(TIME_OF_STEERING_CHANGE))+(dDefaultDecel*(
			sqrt(((TIME_OF_STEERING_CHANGE)*(TIME_OF_STEERING_CHANGE))+(2.0*distance/dDefaultDecel)))));
		}
		else
		{
			if(dVelIn >= HIGH_SPEED_LIMIT_VEL) // 진입 속도가 3.3 이상인 경우
			{	
				// Vmax를 구하는 식
				dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, HIGH_SPEED_ACCEL, dDefaultDecel);
				dSpeedDownTime = TIME_OF_CONST_VEL + ((dVelCalculated - dVelIn) /HIGH_SPEED_ACCEL);

				// Vmax를 Steering change time에 맞춰 준다
				if(dSpeedDownTime <= TIME_OF_STEERING_CHANGE)
				{
					ADD_LOG("[속도변경] 조향변경2-3");
					dVelCalculated = calcSpeed_2(dVelIn, distance, TIME_OF_STEERING_CHANGE, HIGH_SPEED_ACCEL, dDefaultDecel);
				}
				else{
					ADD_LOG("[속도변경] 조향변경3-3");
				}
			}
			else
			{
				// Vmax를 구하는 식
				dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, dDefaultAccel, dDefaultDecel);
				dSpeedDownTime = TIME_OF_CONST_VEL + ((dVelCalculated - dVelIn) /dDefaultAccel);

				if(dSpeedDownTime <= TIME_OF_STEERING_CHANGE)
				{
					dVelCalculated = calcSpeed_2(dVelIn, distance, TIME_OF_STEERING_CHANGE, dDefaultAccel, dDefaultDecel);

					if(dVelCalculated > HIGH_SPEED_LIMIT_VEL)
					{
						ADD_LOG("[속도변경] 조향변경2-2");
						//defaultAccelDistance = (HIGH_SPEED_LIMIT_VEL*HIGH_SPEED_LIMIT_VEL - dVelIn*dVelIn)/(2.0*dDefaultAccel);
						defaultAccelTime = (HIGH_SPEED_LIMIT_VEL-dVelIn)/dDefaultAccel;
						defaultAccelDistance = ((dVelIn + HIGH_SPEED_LIMIT_VEL) * defaultAccelTime) / 2;

						defaultAccelTime = TIME_OF_STEERING_CHANGE - defaultAccelTime; // Tstr(streering 변경 시간) - T3.3(3.3도달 시간)
						dVelCalculated = calcSpeed_2(HIGH_SPEED_LIMIT_VEL, distance-defaultAccelDistance, defaultAccelTime, HIGH_SPEED_ACCEL, dDefaultDecel);
						
					}
					else if(dVelCalculated <= HIGH_SPEED_LIMIT_VEL)
					{
						// Vmax 이후 감속되는 시점이 Sterring change time보다 짧은 시간인 경우 등속을 Sterring change time 만큼 하게끔 Vmax를 낮춤
						ADD_LOG("[속도변경] 조향변경2-1");
						//dVelCalculated = (sqrt((TIME_OF_STEERING_CHANGE+dVelIn/dDefaultAccel)*(TIME_OF_STEERING_CHANGE+dVelIn/dDefaultAccel)+
						//(1/dDefaultDecel-1/dDefaultAccel)*(dVelIn*dVelIn/dDefaultAccel+2*distance))
						//-TIME_OF_STEERING_CHANGE-dVelIn/dDefaultAccel)/(1/dDefaultDecel-1/dDefaultAccel);
					}

				}

				else if(dVelCalculated > HIGH_SPEED_LIMIT_VEL && dSpeedDownTime > TIME_OF_STEERING_CHANGE)
				{	
					// 3.3m/s 이후는 가속도 1.0을 사용하므로 3.3m/s에 도달하는 거리만큼 뺀 후 가속도 1.0으로 다시 계산
					ADD_LOG("[속도변경] 조향변경3-2");
					defaultAccelDistance = (HIGH_SPEED_LIMIT_VEL*HIGH_SPEED_LIMIT_VEL - dVelIn*dVelIn)/(2.0*dDefaultAccel); // default accel (2.0)에서 3.3m/s 까지 증속 하는데까지의 거리
					dVelCalculated = calcSpeed(HIGH_SPEED_LIMIT_VEL, distance-defaultAccelDistance, TIME_OF_CONST_VEL, HIGH_SPEED_ACCEL, dDefaultDecel);

				}

				else{
					//아래 조건의 해당 -> 계산된 값 그대로 사용
					//if(dVelCalculated <= HIGH_SPEED_LIMIT_VEL && dSpeedDownTime > TIME_OF_STEERING_CHANGE)
					ADD_LOG("[속도변경] 조향변경3-1");
				}
			}
			
		}

	}
	// steering 변화가 없는 경우
	else{
		/******************************************************************************
		유형
		1) 이전 속도 < 다음 속도 && 가속 거리 >= 구간 거리
		  → 구간 속도 = 다음 속도
		2) 이전 속도 < 다음 속도 && 가속 거리 < 구간 거리
		  → 구간 속도 = 거리에 따라 산출(다음 속도 or 다음 속도 +a)
		3) 이전 속도 > 다음 속도 && 감속 거리 >= 구간 거리
		  → 구간 속도 = 이전 속도
		4) 이전 속도 > 다음 속도 && 감속 거리 < 구간 거리
		  → 구간 속도 =  거리에 따라 산출(이전 속도 or 이전 속도 + a)
		5) 이전 속도 = 다음 속도
		  → 구간 속도 = 거리에 따라 산출
		******************************************************************************/

		if(dVelIn < dVelOut) dDistanceAccel = GET_ACCEL_DIST(dVelIn, dVelOut, dDefaultAccel);
		if(dVelIn > dVelOut) dDistanceDecel = GET_DECEL_DIST(dVelIn, dVelOut, dDefaultDecel);

		if((dVelIn < dVelOut) && (dDistanceAccel >= distance)){ // case 1
			dVelCalculated = dVelOut;
			ADD_LOG("[속도변경] 조향미변경 1");
		}
		else if((dVelIn < dVelOut) && (dDistanceAccel < distance)){ // case 2
			dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, dDefaultAccel, dDefaultDecel);
			if(dVelCalculated < dVelOut* VELOCITY_INCREASE_MARGIN) dVelCalculated = dVelOut; // 새로 산출 된 속도가 out 속도와 비슷(10%)하면 out 속도 적용
			ADD_LOG("[속도변경] 조향미변경 2");
		}
		else if((dVelIn > dVelOut) && (dDistanceDecel >= distance)){ // case 3
			dVelCalculated = dVelIn;
			ADD_LOG("[속도변경] 조향미변경 3");
		}
		else if((dVelIn > dVelOut) && (dDistanceDecel < distance)){ // case 4
			dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, dDefaultAccel, dDefaultDecel);
			if(dVelCalculated < dVelIn* VELOCITY_INCREASE_MARGIN) dVelCalculated = dVelIn; // 새로 산출 된 속도가 in 속도와 비슷(10%)하면 out 속도 적용
			ADD_LOG("[속도변경] 조향미변경 4");
		}
		else if(dVelIn == dVelOut){ // case 5
			dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, dDefaultAccel, dDefaultDecel);
			if(dVelCalculated < dVelOut* VELOCITY_INCREASE_MARGIN) dVelCalculated = dVelOut;
			ADD_LOG("[속도변경] 조향미변경 5");
		}
		else{
			//error;
			dVelCalculated = DEFAULT_VEL;
		}
		
		if(dVelCalculated > HIGH_SPEED_LIMIT_VEL)
		{	
			if(dVelIn > HIGH_SPEED_LIMIT_VEL){
				dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, HIGH_SPEED_ACCEL, dDefaultDecel);
				ADD_LOG("[속도변경] 조향미변경 6");
			}
			else{
				defaultAccelDistance = (HIGH_SPEED_LIMIT_VEL*HIGH_SPEED_LIMIT_VEL - dVelIn*dVelIn)/(2.0*dDefaultAccel); // default accel (2.0)에서 3.3m/s 까지 증속 하는데까지의 거리
				dVelCalculated = calcSpeed(HIGH_SPEED_LIMIT_VEL, distance-defaultAccelDistance, TIME_OF_CONST_VEL, HIGH_SPEED_ACCEL, dDefaultDecel);
                ADD_LOG("[속도변경] 조향미변경 7");
			}
		}
	}
	

	if(dVelCalculated <= 0.7) dVelCalculated = 0.7; // 최소 속도

	//ADD_LOG("[Test] 속도변경 함수");
	return dVelCalculated;
}


// 등속 시간 보장  (constTime)
double PathSearch::calcSpeed(double velIn, double distance, double constTime, double accel, double decel)
{
	// 사전 조건 판단
	// 1. 감속 거리 보장 안되면 velOut = velIn
	double decelDist = (velIn*velIn) / (2.0*decel);
	if(distance < decelDist)
	{
		return velIn;
	}

	// 2. 등속시간을 보장할 수 없으면 velOut = velIn
    double constDist = velIn * constTime;
	if (distance < decelDist + constDist)
	{
		return velIn;
	}

	// A(X^2) + 2B(X) - C = 0
	// X = (-B + sqrt(B^2 + AC)) / A
	double A = 1.0/decel+1.0/accel;
	double B = constTime;
	double C = velIn*velIn/accel + 2*distance;

	double velOut = (sqrt(B*B+A*C)- B) / A;
	return velOut;
}


// 감속 타이밍 보장. (spdDownTime)
double PathSearch::calcSpeed_2(double velIn, double distance, double spdDownTime, double accel, double decel)
{
	// 사전 조건 판단
	// 1. 감속 거리 보장 안되면 velOut = velIn
	double decelDist = (velIn*velIn) / (2.0*decel);
	if(distance < decelDist)
	{
		return velIn;
	}

	// 2. 감속 시점을 보장할 수 없으면 velOut = velIn
    double constDist = velIn * spdDownTime;
	if (distance < decelDist + constDist)
	{
		return velIn;
	}

	// A(X^2) + 2B(X) - C = 0
	// X = (-B + sqrt(B^2 + AC)) / A
	double A = 1.0/decel - 1.0/accel;
	double B = spdDownTime + velIn/accel;
	double C = velIn*velIn/accel + 2*distance;

	double velOut;
	if(A == 0) // 2BX = C (2차식 성립 안함)
		velOut =  C / (2.0 * B);
	else
		velOut = (sqrt(B*B+A*C)- B) / A;

	return velOut;
}
bool PathSearch::CheckSteering4Deccel(PARSING_GO_INFO *GetGOInfo,int ChangeNodeID)
{
	int cntPath = GetPathCount();
	int cntSteer = 0;
	int pathIndex =0;
	int CheChangeNode =ChangeNodeID;
	UINT	uFromNode			= GetGOInfo->PathData_StartNodeID;			// 시작 노드
	UINT	uToNode				= GetGOInfo->PathData_EndNodeID;		    // 끝 노드
	int		nPathListIndex	   	= 0;		// 비교하고자하는 경로 위치.

	EXECUTE_DRIVING_INFO 	*pPathListInfo  = NULL;
   //	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO = NULL;
  //	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO_TEMP = NULL;

	for(int i=0; i< cntPath; i++)
	{
		pPathListInfo = (EXECUTE_DRIVING_INFO*) ReferPath(i);
	 //	ADD_LOG("[PATHSEARCH Start ID]%d ExtraID:%d %d",pPathListInfo->BasicInfo.StartNID, CmdGoData->PathData_ExtraID[uCheckExtraNum],CmdGoData->PathData_ExtraNum);

	 //	ChangeSqNum = pPathListInfo->SQNum;  //초기값 갱신

		if(pPathListInfo->BasicInfo.StartNID ==  uFromNode)
		{
			nPathListIndex = i;
//				 ADD_SOM_LOG("StartNID %d, uFromNode = %d ,nPathListIndex:%d ",pPathListInfo->BasicInfo.StartNID, uFromNode,i ) ;
		  //	bArrived = true;
			break;
		}
	}

//	ADD_SOM_LOG("cntPath = %d",cntPath );
	for(pathIndex= nPathListIndex; pathIndex < cntPath ; pathIndex++)
	{
		pPathListInfo = (EXECUTE_DRIVING_INFO *)m_PathList->refer(pathIndex);

		if(pPathListInfo->BasicInfo.Steering == STEERING_NONE)
		{
			// 조향 NONE 인 경우 ++;
//			 ADD_SOM_LOG("Steering = %d",pPathListInfo->BasicInfo.Steering ) ;
//			 ADD_SOM_LOG("node %d", pPathListInfo->BasicInfo.StartNID);
			 cntSteer ++;



		}
		else
		{  //조향 NONE이 아닌게 하나라도 있으면 return 하여 감속도 fix
		 //	cntSteer = 0;
//			ADD_SOM_LOG("diff status node %d ,cntPath:%d, cntSteer:%d", pPathListInfo->BasicInfo.StartNID,cntPath,cntSteer);
			return false;
		}

		if(pPathListInfo->BasicInfo.EndNID == CheChangeNode)        //ChangeNode 만나면 조향 체크 끝
		{
//			ADD_SOM_LOG("End %d, change node %d", pPathListInfo->BasicInfo.EndNID,CheChangeNode);
			break;
		 }

	}
	//Path에 NONE 만 있는 경우 확인
//	if(cntPath != 0)
//	{
//		  ADD_SOM_LOG("same status cntPath:%d, cntSteer:%d,pathIndex:%d",cntPath,cntSteer,pathIndex);
		  return true;
//	}

}

//AOHC-302 맵기반 주행경로 자동탐색
bool PathSearch::FindTheWay(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	//맵기반 이동, 공용처리를 위한 변수
	m_bFindTheWay = true;
	m_EndNodeCnt = 0;

	//성공/실패여부 리턴
	bool bSuccess = true;

	//while문 카운트
	int nCnt = 0;

	//시작, 끝노드
	int nStartNode = FromNode;
	int nEndNode = ToNode;

	//맵정보 확인용 변수, 리스트
	int nTempPreNode  = 0;
	int nTempNextNode[2] = {0,};
	int VisitCnt = 0;
	int nCrossRoad = 0;

	List <int> idx;
	NODE_INFO	*pNodeInfo	= NULL;	  // Node 정보
	PATH_INFO	*pPathInfo	= NULL;	  // Path 정보
	LaneCutSt	*TempLanCut = NULL;   // LaneCut 정보

	//결과 추출에 사용되는 리스트
	LList	*pResultAddList	= DRIVING_INFO_List;
	EXECUTE_DRIVING_INFO  *pEXECUTE_DRIVING_INFO = NULL;

	//알고리즘에 사용되는 변수

	int nIndex = 0;                 //현재 지나온 노드 개수 Cnt(중복 포함)
	int nVisitCnt = 0;              //현재 Visit한 링크 개수 Cnt(중복 제외)
	int nMaxVisitCnt = 0;      	 	//최대 Visit한 링크 개수 Cnt
	int nRouteListSize = 0;     	//리스트에 쌓인 데이터 수

	int Node;						//현재 노드
	int NodeArray[250];        		//지나온 노드 정보 저장

	std::list <RouteSt> RouteList;	//알고리즘에 사용되는 리스트
	RouteSt RouteTemp;				//리스트에 사용되는 구조체

	//Initailization
	ZeroMemory(VisitLinkCheckArray, sizeof(VisitLinkCheckArray));

	//LaneCutLoad
	if(!LaneCutLoad())
	{
		return false;
	}

	//1. Route Search Ready : Information Check, Interlock

	//시작노드, 끝노드 맵정보가 실제로 있는지 확인, 정보가 없을 때 Fail
	if(	(pMap->FindNodeIndex(nStartNode) ==	-1)
	||	(pMap->FindNodeIndex(nEndNode)	==	-1)	)
	{
		ADD_LOG("No Map Info, Start: %d, End: %d", nStartNode, nEndNode);
		return false;
	}

	//현재 마크가 된 상태인지 확인, 마크가 아닐 때 Fail
	if(AdditionalInfo.StationType != MARK_TYPE_NODE)
	{
		if(CheckPathOfStation(pMap, nStartNode, nEndNode, AdditionalInfo)!= NO_ERR)
		{
			ADD_LOG("No Node Mark");
			return false;
		}
	}

	//	현재 노드 기준 다음 노드 정보가 있는지 확인, 정보가 없을 때 Fail
	int nNodeIndex = pMap->FindNodeIndex(nStartNode);
	pNodeInfo = &(pMap->m_NodeArray[nNodeIndex]);
	if(pNodeInfo->NextPathIndex[0] == INDEX_NONE && pNodeInfo->NextPathIndex[1] == INDEX_NONE)
	{
		ADD_LOG("Next Node Index None");
		return false;
	}


	//2. Route Teaching : Closed loop가 형성된 전 경로 Teaching
	idx.clear();
	idx.insertBack(nStartNode);
	while(idx.getCount() != 0)
	{
		//카운트 이상 시 종료
		if(nCnt > 50000)
		{
			break;
		}
		int Node = idx.popFront();
		int NodeIndex = pMap->FindNodeIndex(Node);

		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i<2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				nTempNextNode[i] = pPathInfo->EndNID;

			}
			else
			{
				nTempNextNode[i] = -1;
            }
		}

		int nCount = LaneCutList.getCount();
		for(int i = 0; i < nCount; i++)
		{
			TempLanCut =(LaneCutSt*) LaneCutList.refer(i);
			if(TempLanCut->PreNode == Node)
			{
				if(TempLanCut->NextNode ==  nTempNextNode[0])
				{
					nTempNextNode[0] = nTempNextNode[1];
					nTempNextNode[1] = -1;
				}
				else if(TempLanCut->NextNode  ==  nTempNextNode[1])
				{
					nTempNextNode[1] = -1;
				}
			}

		}

		int nCntChk = nTempNextNode[1] < 0 ? 1:2;

		for(int i = 0; i<nCntChk;i++)
		{
			if(VisitLinkCheckArray[Node][i]==0)
			{
				ADD_LOG("Visit:%d-%d ", Node,nTempNextNode[i]);
				VisitLinkCheckArray[Node][i] = 1;
				VisitCnt++;
			}

			if(nTempNextNode[i]!=-1)
				idx.insertBack(nTempNextNode[i]);
		}
		nCnt++;
	}
	idx.clear();


	//3. Algorithm

	//리스트 초기화
	RouteList.clear();

	//처음 리스트에 넣을 데이터 세팅
	RouteTemp.index = 0;
	RouteTemp.VisitCnt = 0;
	RouteTemp.indexNode = nStartNode;

	ZeroMemory(&RouteTemp.NodeArray, sizeof(RouteTemp.NodeArray));
	RouteTemp.NodeArray[RouteTemp.index++] = nStartNode;


	//리스트 iteratior, 최초 리스트 삽입
	nCnt = 0;
	list<RouteSt>::iterator iterRouteSt;
	RouteList.push_back(RouteTemp);


	//알고리즘  시작
	while(!RouteList.empty())
	{
		//리스트의 맨 앞데이터를 POP
		nRouteListSize = RouteList.size();
		iterRouteSt = RouteList.begin();
		nIndex = iterRouteSt->index;
		nVisitCnt = iterRouteSt->VisitCnt;
		Node = iterRouteSt->indexNode;
		memmove(&NodeArray,  &iterRouteSt->NodeArray, sizeof(iterRouteSt->NodeArray));
		RouteList.pop_front();

		//연산 회수 최소화
		if(nMaxVisitCnt <= nVisitCnt)
		{
			nMaxVisitCnt = nVisitCnt;
		}
		else
		{
			continue;
		}

		//성공조건, 경로탐색완료
		if(nVisitCnt == VisitCnt && Node == nEndNode)
		{
			int NodeSum = nIndex;
			int FromNode;
			int ToNode;
			int nPathIndex;
			ADD_LOG("index: %d", NodeSum);

			for(int i=0; i<NodeSum-1; i++)
			{
				FromNode   = NodeArray[i];
				ToNode 	   = NodeArray[i+1];

				if(ToNode == nEndNode)
					m_EndNodeCnt++;

				nPathIndex	= pMap->GetPathIndex(FromNode, ToNode);
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);

				ADD_LOG("Link: %d-%d, Steer: %d", FromNode, ToNode,	pPathInfo->RailBranch);

				pEXECUTE_DRIVING_INFO	 = new EXECUTE_DRIVING_INFO;
				pEXECUTE_DRIVING_INFO->BasicInfo.StartNID			= FromNode;
				pEXECUTE_DRIVING_INFO->BasicInfo.EndNID				= ToNode;
				pEXECUTE_DRIVING_INFO->BasicInfo.StartNType			= 0;
				pEXECUTE_DRIVING_INFO->BasicInfo.EndNType			= 0;
				pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed				= (double)(pPathInfo->Speed/1000.0);			// mm/s -> mm/msec
				pEXECUTE_DRIVING_INFO->BasicInfo.dLength			= (double)(pPathInfo->Distance);		// UINT --> double
				pEXECUTE_DRIVING_INFO->BasicInfo.Steering			= (PATH_STEERING_INFO)(pPathInfo->Steering);
		#if(USE_CHANGENODE == 1)
				pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion			= (PATH_DIRECTION_INFO)(pPathInfo->UBGRegion);
				pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion 	= (PATH_OBS_DIRECTION_INFO)(pPathInfo->ObstacleRegion);
				pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering		= PATH_STEERING_NONE;
				pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag 	= pPathInfo->ObstacleIgnore;

				pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo 			= pPathInfo->RailBranch;

				pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance		= pPathInfo->OptDistance;
				pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter	= pPathInfo->OptDistanceAfter;
				pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion 		= (PATH_DIRECTION_INFO)pPathInfo->OptUBGRegion;
				pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion	= (PATH_OBS_DIRECTION_INFO)pPathInfo->OptObstacleRegion;
		#else
				pEXECUTE_DRIVING_INFO->BasicInfo.Direction			= (PATH_DIRECTION_INFO)(pPathInfo->PathDirection);
				pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering		= PATH_STEERING_NONE;
				pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag		= pPathInfo->ObstacleIgnore;
				pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo 			= pPathInfo->RailBranch;
		#endif
				memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));


				pResultAddList->insertBack((void*)pEXECUTE_DRIVING_INFO);
			}
			break;
		}
		//실패조건, Index가 250 이상일 때, 중지
		else if(nIndex >= 250)
		{
			bSuccess = false;
			break;

		}
		//그 외 조건, 탐색 계속 진행


		//레인컷 체크
		int NodeIndex = pMap->FindNodeIndex(Node);
		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i<2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				nTempNextNode[i] = pPathInfo->EndNID;
			}
			else
			{
				nTempNextNode[i] = -1;
			}
		}
		int nCount = LaneCutList.getCount();
		for(int i = 0; i < nCount; i++)
		{
			LaneCutSt* TempLanCut = (LaneCutSt*)LaneCutList.refer(i);
			if(TempLanCut->PreNode == Node)
			{
				if(TempLanCut->NextNode ==  nTempNextNode[0])
				{
					nTempNextNode[0] = nTempNextNode[1];
					nTempNextNode[1] = -1;
				}
				else if(TempLanCut->NextNode ==  nTempNextNode[1])
				{
					nTempNextNode[1] = -1;
				}
				else
				{
					break;
				}
			}
		}

		//경로 수 체크
		int nCntChk = nTempNextNode[1] < 0 ? 1 : 2;

		//경로 수만큼 리스트에 쌓는다.
		for(int i = 0; i < nCntChk; i++)
		{
			//데이터 초기화, 덮어치기
			ZeroMemory(&RouteTemp, sizeof(RouteTemp));
			RouteTemp.index = nIndex;
			RouteTemp.VisitCnt = nVisitCnt;
			RouteTemp.indexNode = nTempNextNode[i];
			memmove(&RouteTemp.NodeArray, &NodeArray, sizeof(NodeArray));
			RouteTemp.NodeArray[RouteTemp.index++] = nTempNextNode[i];

			//왔던 경로인지 확인하는 함수
			Fn_VisitCheck(&RouteTemp);

			//리스트에 추가
			RouteList.push_back(RouteTemp);
		}

		nCnt++;
	}
	ADD_LOG("End Count: %d", m_EndNodeCnt);

	//리스트 초기화
	while(!RouteList.empty())
	{
		RouteList.pop_front();
	}
	RouteList.clear();
	LaneCutList.clear();

	return bSuccess;
}

bool PathSearch::RouteCheckNode(MAP *pMap, PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE_REQ PACKET_BODY, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	//맵기반 이동, 공용처리를 위한 변수
	m_bFindTheWay = true;
	m_EndNodeCnt = 0;

	//성공/실패여부 리턴
	bool bSuccess = true;

	//시작노드,  끝노드
	int nStartNode  		=   PACKET_BODY.StartID;
	int nEndNode 			=	PACKET_BODY.EndID;
	int nNodeCnt 			= 	PACKET_BODY.RouteCount;
	int RouteList[50] 		= 	{0,};
	memset(RouteList,  	0,						sizeof(RouteList));
	memcpy(RouteList, 	PACKET_BODY.RouteList, 	sizeof(RouteList));

	NODE_INFO	*pNodeInfo	= NULL;	  // Node 정보
	PATH_INFO	*pPathInfo	= NULL;	  // Path 정보
	LaneCutSt	*TempLanCut = NULL;   // LaneCut 정보

	//결과 추출에 사용되는 리스트
	LList	*pResultAddList	= DRIVING_INFO_List;
	EXECUTE_DRIVING_INFO  *pEXECUTE_DRIVING_INFO = NULL;


	//시작노드, 끝노드 맵정보가 실제로 있는지 확인, 정보가 없을 때 Fail
	if((pMap->FindNodeIndex(nStartNode) ==	-1)
		||	(pMap->FindNodeIndex(nEndNode)	== -1) )
	{
		ADD_LOG("[RouteCheckNode]No Map Info, Start: %d, End: %d", nStartNode, nEndNode);
		return false;
	}


	//현재 마크가 된 상태인지 확인, 마크가 아닐 때 Fail
	if(AdditionalInfo.StationType != MARK_TYPE_NODE)
	{
		if(CheckPathOfStation(pMap, nStartNode, nEndNode, AdditionalInfo)!= NO_ERR)
		{
			ADD_LOG("[RouteCheckNode]No Node Mark");
			return false;
		}
	}

	//현재 노드 기준 다음 노드 정보가 있는지 확인, 정보가 없을 때 Fail
	int nNodeIndex = pMap->FindNodeIndex(nStartNode);
	pNodeInfo = &(pMap->m_NodeArray[nNodeIndex]);

	if(pNodeInfo->NextPathIndex[0] == INDEX_NONE && pNodeInfo->NextPathIndex[1] == INDEX_NONE)
	{
		ADD_LOG("[RouteCheckNode]Next Node Index None");
		return false;
	}

	ADD_LOG("[RouteCheckNode]index: %d", nNodeCnt);

	for(int i=0; i<nNodeCnt-1; i++)
	{
		int FromNode   = RouteList[i];
		int ToNode 	   = RouteList[i+1];

		if(ToNode == nEndNode)
			m_EndNodeCnt++;

		int nPathIndex = pMap->GetPathIndex(FromNode, ToNode);
		if(nPathIndex == INDEX_NONE)
		{
			ADD_LOG("[RouteCheckNode]Path Abnormal");
			return false;
		}
		pPathInfo = &(pMap->m_PathArray[nPathIndex]);

		ADD_LOG("[RouteCheckNode]Link: %d-%d, Steer: %d", FromNode, ToNode,	pPathInfo->RailBranch);

		pEXECUTE_DRIVING_INFO	 							= new EXECUTE_DRIVING_INFO;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNID			= FromNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNID				= ToNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNType			= 0;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNType			= 0;
		pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed				= (double)(pPathInfo->Speed/1000.0);			// mm/s -> mm/msec
		pEXECUTE_DRIVING_INFO->BasicInfo.dLength			= (double)(pPathInfo->Distance);		// UINT --> double
		pEXECUTE_DRIVING_INFO->BasicInfo.Steering			= (PATH_STEERING_INFO)(pPathInfo->Steering);
#if(USE_CHANGENODE == 1)
		pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion			= (PATH_DIRECTION_INFO)(pPathInfo->UBGRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion 	= (PATH_OBS_DIRECTION_INFO)(pPathInfo->ObstacleRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering		= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag 	= pPathInfo->ObstacleIgnore;

		pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo 			= pPathInfo->RailBranch;

		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance		= pPathInfo->OptDistance;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter	= pPathInfo->OptDistanceAfter;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion 		= (PATH_DIRECTION_INFO)pPathInfo->OptUBGRegion;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion	= (PATH_OBS_DIRECTION_INFO)pPathInfo->OptObstacleRegion;
#else
		pEXECUTE_DRIVING_INFO->BasicInfo.Direction			= (PATH_DIRECTION_INFO)(pPathInfo->PathDirection);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering		= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag		= pPathInfo->ObstacleIgnore;
		pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo 			= pPathInfo->RailBranch;
#endif
		memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));


		pResultAddList->insertBack((void*)pEXECUTE_DRIVING_INFO);
	}


	ADD_LOG("End Count: %d", m_EndNodeCnt);

	return bSuccess;
}
//AOHC-302 맵기반 주행경로 자동탐색
void PathSearch::Fn_VisitCheck(RouteSt* pRouteSt)
{
	ZeroMemory(VisitLinkCheckArray, sizeof(VisitLinkCheckArray));

	int cnt = pRouteSt->index;
	int VisitCnt = 0;
	int pre_node, next_node;

	for(int i = 0; i<cnt-1; i++)
	{
		pre_node = pRouteSt->NodeArray[i];
		next_node = pRouteSt->NodeArray[i+1];

		if(VisitLinkCheckArray[pre_node][0] == 0)
		{
			 VisitCnt++;
			 VisitLinkCheckArray[pre_node][0] = next_node;
		}
		else
		{
			if(VisitLinkCheckArray[pre_node][0] != next_node)
			{
				if(VisitLinkCheckArray[pre_node][1] == 0)
				{
					VisitCnt++;
					VisitLinkCheckArray[pre_node][1] = next_node;
				}
			}
		}
	}
	pRouteSt->VisitCnt = VisitCnt;
}

//AOHC-302 맵기반 주행경로 자동탐색
bool PathSearch::LaneCutLoad()
{
	std::string tmp;
	std::ifstream tmpFile("DataFiles\\LaneCutList.ini");

	int PreNodeID = 0;
	int PreNode = 0;
	int NextNode = 0;

	LaneCutSt* pLanCutSt;
	LaneCutList.clear();

	while(LaneCutList.getCount()>0)
	{
		pLanCutSt = (LaneCutSt*)LaneCutList.popFront();
		delete pLanCutSt;
	}

	if(tmpFile.is_open() == false)
		return false;

	//한줄씩 읽는 부분
	while (std::getline(tmpFile, tmp))
	{
		char szLine[32];
		memset(szLine, 0, sizeof(szLine));
		sprintf(szLine, "%s", tmp.c_str());
		for(int i=0; szLine[i]!=0; i++)
		{
			if(szLine[i] == '_') szLine[i] = ' ';
		}
			int n = sscanf(szLine, "%d %d",
					&PreNode,
					&NextNode
					);

		pLanCutSt = new LaneCutSt;
		pLanCutSt->PreNode = PreNode;
		pLanCutSt->NextNode = NextNode;

		LaneCutList.insertBack((void*)pLanCutSt);

	}
	tmpFile.close();

	return true;
}

bool PathSearch::LaneCutLoad(MAP *pMap, PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_REQ PACKET_BODY)
{
	std::string tmp;

	int PreNodeID = 0;
	int PreNode = 0;
	int NextNode = 0;
	int LaneCutArray[20][2];

	LaneCutSt* pLanCutSt;
	LaneCutList.clear();

	while(LaneCutList.getCount()>0)
	{
		pLanCutSt = (LaneCutSt*)LaneCutList.popFront();
		delete pLanCutSt;
	}

	memcpy(&LaneCutArray, &PACKET_BODY.LaneCutList, sizeof(PACKET_BODY.LaneCutList));

	//한줄씩 읽는 부분
	for(int i = 0; i<20; i++)
	{
		NODE_INFO	*pNodeInfo	= NULL;
		PATH_INFO	*pPathInfo	= NULL;	  // Path 정보

		bool bResult = false;
		PreNode = LaneCutArray[i][0];
		NextNode = LaneCutArray[i][1];

		if(PreNode == 0 && NextNode == 0)
			continue;
		else if(PreNode == 0 && NextNode != 0)
			return false;
		else if(PreNode!=0 && NextNode == 0)
			return false;

		if(	(pMap->FindNodeIndex(PreNode) == -1)
		||	(pMap->FindNodeIndex(NextNode)	==	-1)	)
		{
			ADD_LOG("No Map Info, PreNode: %d, Nextnode: %d", PreNode, NextNode);
			return false;
		}

		int NodeIndex = pMap->FindNodeIndex(PreNode);
		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i < 2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				if(NextNode == pPathInfo->EndNID)
                	bResult = true;
			}
		}

		if(!bResult)
		{
			ADD_LOG("LaneCutList Information Abnormal, preNode: %d, NextNode: %d", PreNode, NextNode);
			return false;
		}

		pLanCutSt = new LaneCutSt;
		pLanCutSt->PreNode = PreNode;
		pLanCutSt->NextNode = NextNode;

		LaneCutList.insertBack((void*)pLanCutSt);
	}

	return true;

}
bool PathSearch::NodeDrivingTest(MAP *pMap, UINT FromNode, int nLinkSetting)
{
	//성공/실패여부 리턴
	bool bSuccess = false;

	//while문 카운트
	int nCnt = 0;

	//시작노드
	int nStartNode = FromNode;

	//맵정보 확인용 변수, 리스트
	int nTempPreNode  = 0;
	int nTempNextNode[2] = {0,};
	int VisitLinkCnt = 0;
	int VisitNodeCnt = 0;
	int nCrossRoad = 0;

	List <int> idx;
	NODE_INFO	*pNodeInfo	= NULL;	  // Node 정보
	PATH_INFO	*pPathInfo	= NULL;	  // Path 정보
	LaneCutSt	*TempLanCut = NULL;   // LaneCut 정보


	//알고리즘에 사용되는 변수
	int nLinkCnt = 0;               //인덱스 하나 기준 돌은 링크 개수
	int nIndex = 0;                 //현재 인덱스값
	int nIndexNode = 0;				//현재 노드
	int nVisitLinkCnt = 0;          //현재 Visit한 링크 개수 Cnt(중복 제외)
	int nVisitLinkMaxCnt = 0;      	//최대 Visit한 링크 개수 Cnt
	int nVisitNodeCnt = 0;          //현재 Visit한 노드 개수 Cnt(중복 제외)
	int nVisitNodeMaxCnt = 0;      	//최대 Visit한 노드 개수 Cnt
	int NodeArray[500];
	int StopOverArray[100];

	int nListSize = 0;     			//리스트에 쌓인 데이터 수
	std::list <DrivingTestSt> DrivingTestList;	//알고리즘에 사용되는 리스트
	DrivingTestSt DrivingTestTemp;				//리스트에 사용되는 구조체

	//Initailization
	ZeroMemory(&VisitLinkCheckArray, sizeof(VisitLinkCheckArray));
	ZeroMemory(&VisitNodeCheckArray, sizeof(VisitNodeCheckArray));

	//LaneCutLoad
	if(!LaneCutLoad())
	{
		return false;
	}

	//1. Route Search Ready : Information Check, Interlock

	//시작노드 맵정보가 실제로 있는지 확인, 정보가 없을 때 Fail
	if(	(pMap->FindNodeIndex(nStartNode) ==	-1)  )
	{
		ADD_LOG("No Map Info, Start: %d", nStartNode);
		return false;
	}

	//	현재 노드 기준 다음 노드 정보가 있는지 확인, 정보가 없을 때 Fail
	int nNodeIndex = pMap->FindNodeIndex(nStartNode);
	pNodeInfo = &(pMap->m_NodeArray[nNodeIndex]);
	if(pNodeInfo->NextPathIndex[0] == INDEX_NONE && pNodeInfo->NextPathIndex[1] == INDEX_NONE)
	{
		ADD_LOG("Next Node Index None");
		return false;
	}


	//2. Route Teaching : Closed loop가 형성된 전 경로 Teaching
	idx.clear();
	idx.insertBack(nStartNode);
	VisitNodeCnt = 1;
	VisitNodeCheckArray[nStartNode] = 1;

	while(idx.getCount() != 0)
	{
		//카운트 이상 시 종료
		if(nCnt > 500)
		{
			break;
		}
		int Node = idx.popFront();
		int NodeIndex = pMap->FindNodeIndex(Node);

		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i < 2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				nTempNextNode[i] = pPathInfo->EndNID;

			}
			else
			{
				nTempNextNode[i] = -1;
            }
		}
		int nCount = LaneCutList.getCount();
		for(int i = 0; i < nCount; i++)
		{
			TempLanCut =(LaneCutSt*) LaneCutList.refer(i);
			if(TempLanCut->PreNode == Node)
			{
				if(TempLanCut->NextNode ==  nTempNextNode[0])
				{
					nTempNextNode[0] = nTempNextNode[1];
					nTempNextNode[1] = -1;
				}
				else if(TempLanCut->NextNode == nTempNextNode[1])
				{
					nTempNextNode[1] = -1;
				}
			}

		}

		int nCntChk = nTempNextNode[1] < 0 ? 1:2;
		int preVisitLinkCnt = VisitLinkCnt;

		for(int i = 0; i<nCntChk; i++)
		{
			if(VisitLinkCheckArray[Node][i] == 0)
			{
				ADD_LOG("Visit:%d-%d ", Node,nTempNextNode[i]);
				VisitLinkCheckArray[Node][i] = 1;
				VisitLinkCnt++;
			}

			if(nTempNextNode[i]!=-1)
			{
				idx.insertBack(nTempNextNode[i]);
				if(VisitNodeCheckArray[nTempNextNode[i]] == 0)
				{
					VisitNodeCheckArray[nTempNextNode[i]] = 1;
					VisitNodeCnt++;
				}
			}

		}
		if(preVisitLinkCnt == VisitLinkCnt)
			nCnt++;
	}
	idx.clear();


	//3. Algorithm

	//주행성공여부(node)
	bool bNodeCheck = false;
	bool bAllCheck = false;

	//리스트 초기화
	DrivingTestList.clear();

	//처음 리스트에 넣을 데이터 세팅
	DrivingTestTemp.index = 1;
	DrivingTestTemp.indexNode = nStartNode;
	DrivingTestTemp.VisitLinkCnt = 0;
	DrivingTestTemp.VisitNodeCnt = 0;
	ZeroMemory(&DrivingTestTemp.NodeArray, sizeof(DrivingTestTemp.NodeArray));
	ZeroMemory(&DrivingTestTemp.StopOverArray, sizeof(DrivingTestTemp.StopOverArray));

	DrivingTestTemp.NodeArray[0] = nStartNode;
	DrivingTestTemp.StopOverArray[0] = 0;

	//리스트 iteratior, 최초 리스트 삽입
	nCnt = 0;
	list<DrivingTestSt>::iterator iterDrivingTestSt;
	DrivingTestList.push_back(DrivingTestTemp);


	//알고리즘  시작
	while(!DrivingTestList.empty())
	{
		//리스트의 맨 앞데이터를 POP..
		nListSize = DrivingTestList.size();
		iterDrivingTestSt = DrivingTestList.begin();
		nIndex = iterDrivingTestSt->index;
		nIndexNode = iterDrivingTestSt->indexNode;
		nVisitLinkCnt = iterDrivingTestSt->VisitLinkCnt;
		nVisitNodeCnt = iterDrivingTestSt->VisitNodeCnt;

		memset(&NodeArray, 0, sizeof(NodeArray));
		memset(&StopOverArray, 0, sizeof(StopOverArray));
		memmove(&NodeArray,  iterDrivingTestSt->NodeArray, sizeof(iterDrivingTestSt->NodeArray));
		memmove(&StopOverArray,  iterDrivingTestSt->StopOverArray, sizeof(iterDrivingTestSt->StopOverArray));

		DrivingTestList.pop_front();


		//노드체크중.. 연산회수 최소화
		if(!bNodeCheck)
		{
			if(nVisitNodeMaxCnt <= nVisitNodeCnt)
			{
				nVisitNodeMaxCnt = nVisitNodeCnt;
			}
			else
			{
				continue;
			}
		}

		//성공조건1 - 경로탐색(노드주행) 완료
		if(VisitNodeCnt == nVisitNodeCnt && !bNodeCheck)
		{
			bNodeCheck = true;
			DrivingTestList.clear();
		}
		//성공조건2 - 경로탐색 완료 후, 시작지점으로 복귀
		else if(bNodeCheck && !bAllCheck)
		{
			//성공조건2 - 시작지점으로 복귀완료
			if(nIndexNode == nStartNode)
				bAllCheck = true;

		}
		//실패조건, Index가 250 이상일 때, 중지
		else if(nIndex >= 500 || nVisitNodeCnt >= 100)
		{
			bSuccess = false;
			break;

		}

		//탐색이 모두 진행완료상태인지  확인.
		if(bAllCheck)
		{
			FILE* fp;
			fp = fopen("DataFiles\\NavigationRoute.txt", "w");

			if(fp == NULL) return false;
			int S_Index = 0;

			int n = 0;
			for(int i = 0; i<nIndex; i++)
			{
                n++;
				if(i!= 0 && NodeArray[i] == StopOverArray[S_Index] && n>nLinkSetting-1)
				{
					fprintf(fp, "%d\n", NodeArray[i]);
					S_Index++;
					n = 0;
					if(i == nIndex-1)
						break;
				}

				fprintf(fp, "%d ", NodeArray[i]);
			}
			fclose(fp);
			bSuccess = true;
			break;
		}


		// 그 외인 경우 아래 탐색로직 진행.


		//레인컷 체크
		int NodeIndex = pMap->FindNodeIndex(nIndexNode);
		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i<2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				nTempNextNode[i] = pPathInfo->EndNID;
			}
			else
			{
				nTempNextNode[i] = -1;
			}
		}
		int nCount = LaneCutList.getCount();
		for(int i = 0; i < nCount; i++)
		{
			LaneCutSt* TempLanCut = (LaneCutSt*)LaneCutList.refer(i);
			if(TempLanCut->PreNode == nIndexNode)
			{
				if(TempLanCut->NextNode ==  nTempNextNode[0])
				{
					nTempNextNode[0] = nTempNextNode[1];
					nTempNextNode[1] = -1;
				}
				else if(TempLanCut->NextNode ==  nTempNextNode[1])
				{
					nTempNextNode[1] = -1;
				}
				else
				{
					break;
				}
			}
		}

		//경로 수 체크
		int nCntChk = nTempNextNode[1] < 0 ? 1 : 2;


		//경로 수만큼 리스트에 쌓는다 -> 핵심 로직
		for(int i = 0; i < nCntChk; i++)
		{
			int nextNode = nTempNextNode[i];
			memset(&DrivingTestTemp, 0, sizeof(DrivingTestTemp));

			//데이터 쓰기
			DrivingTestTemp.index = nIndex;
			DrivingTestTemp.VisitLinkCnt = nVisitLinkCnt;
			DrivingTestTemp.VisitNodeCnt = nVisitNodeCnt;
			memcpy(&DrivingTestTemp.NodeArray, &NodeArray ,sizeof(DrivingTestTemp.NodeArray));
			memcpy(&DrivingTestTemp.StopOverArray, &StopOverArray ,sizeof(DrivingTestTemp.StopOverArray));

			DrivingTestTemp.indexNode = nTempNextNode[i];
			DrivingTestTemp.NodeArray[DrivingTestTemp.index++] = nextNode;


			if(!bNodeCheck)
			{
				//위 로직에서 링크개수가 기준 이상일 때, 노드체크 시작
				VisitNodeCheck(&DrivingTestTemp,nLinkSetting);

			}


			//리스트에 추가
			DrivingTestList.push_back(DrivingTestTemp);
		}

		nCnt++;
	}

	//리스트 초기화
	while(!DrivingTestList.empty())
	{
		DrivingTestList.pop_front();
	}
	DrivingTestList.clear();
	LaneCutList.clear();

	return bSuccess;
}
bool PathSearch::NodeDrivingTest(MAP *pMap, PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_REQ PACKET_BODY)
{

//성공/실패여부 리턴
	bool bSuccess = false;

	//while문 카운트
	int nCnt = 0;

	//필요한 주요정보 파싱
	int nStartNode = PACKET_BODY.StartID;
	int nLinkSetting = PACKET_BODY.LinkCount;

	//맵정보 확인용 변수, 리스트
	int nTempPreNode  = 0;
	int nTempNextNode[2] = {0,};
	int VisitLinkCnt = 0;
	int VisitNodeCnt = 0;
	int nCrossRoad = 0;

	List <int> idx;
	NODE_INFO	*pNodeInfo	= NULL;	  // Node 정보
	PATH_INFO	*pPathInfo	= NULL;	  // Path 정보
	LaneCutSt	*TempLanCut = NULL;   // LaneCut 정보

	//알고리즘에 사용되는 변수
	int nLinkCnt = 0;               //인덱스 하나 기준 돌은 링크 개수
	int nIndex = 0;                 //현재 인덱스값
	int nIndexNode = 0;				//현재 노드
	int nVisitLinkCnt = 0;          //현재 Visit한 링크 개수 Cnt(중복 제외)
	int nVisitLinkMaxCnt = 0;      	//최대 Visit한 링크 개수 Cnt
	int nVisitNodeCnt = 0;          //현재 Visit한 노드 개수 Cnt(중복 제외)
	int nVisitNodeMaxCnt = 0;      	//최대 Visit한 노드 개수 Cnt
	int NodeArray[500];
	int StopOverArray[100];

	int nListSize = 0;     						//리스트에 쌓인 데이터 수
	std::list <DrivingTestSt> DrivingTestList;	//알고리즘에 사용되는 리스트
	DrivingTestSt DrivingTestTemp;				//리스트에 사용되는 구조체

	//Initailization
	ZeroMemory(&VisitLinkCheckArray, sizeof(VisitLinkCheckArray));
	ZeroMemory(&VisitNodeCheckArray, sizeof(VisitNodeCheckArray));

	//LaneCutLoad
	if(!LaneCutLoad(pMap, PACKET_BODY))
	{
		ADD_LOG("LaneCutList Abnormal");
		return false;
	}

	//1. Route Search Ready : Information Check, Interlock

	//시작노드 맵정보가 실제로 있는지 확인, 정보가 없을 때 Fail
	if(	(pMap->FindNodeIndex(nStartNode) ==	-1)  )
	{
		ADD_LOG("No Map Info, Start: %d", nStartNode);
		return false;
	}

	//	현재 노드 기준 다음 노드 정보가 있는지 확인, 정보가 없을 때 Fail
	int nNodeIndex = pMap->FindNodeIndex(nStartNode);
	pNodeInfo = &(pMap->m_NodeArray[nNodeIndex]);
	if(pNodeInfo->NextPathIndex[0] == INDEX_NONE && pNodeInfo->NextPathIndex[1] == INDEX_NONE)
	{
		ADD_LOG("Next Node Index None");
		return false;
	}


	//2. Route Teaching : Closed loop가 형성된 전 경로 Teaching
	idx.clear();
	idx.insertBack(nStartNode);
	VisitNodeCnt = 1;
	VisitNodeCheckArray[nStartNode] = 1;

	while(idx.getCount() != 0)
	{
		//카운트 이상 시 종료
		if(nCnt > 500)
		{
			break;
		}
		int Node = idx.popFront();
		int NodeIndex = pMap->FindNodeIndex(Node);

		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i < 2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				nTempNextNode[i] = pPathInfo->EndNID;

			}
			else
			{
				nTempNextNode[i] = -1;
            }
		}

		int nCount = LaneCutList.getCount();
		for(int i = 0; i < nCount; i++)
		{
			TempLanCut =(LaneCutSt*) LaneCutList.refer(i);
			if(TempLanCut->PreNode == Node)
			{
				if(TempLanCut->NextNode ==  nTempNextNode[0])
				{
					nTempNextNode[0] = nTempNextNode[1];
					nTempNextNode[1] = -1;
				}
				else if(TempLanCut->NextNode == nTempNextNode[1])
				{
					nTempNextNode[1] = -1;
				}
			}

		}

		int nCntChk = nTempNextNode[1] < 0 ? 1:2;
		int preVisitLinkCnt = VisitLinkCnt;

		for(int i = 0; i<nCntChk; i++)
		{
			if(VisitLinkCheckArray[Node][i] == 0)
			{
				ADD_LOG("Visit:%d-%d ", Node,nTempNextNode[i]);
				VisitLinkCheckArray[Node][i] = 1;
				VisitLinkCnt++;
			}

			if(nTempNextNode[i]!=-1)
			{
				idx.insertBack(nTempNextNode[i]);
				if(VisitNodeCheckArray[nTempNextNode[i]] == 0)
				{
					VisitNodeCheckArray[nTempNextNode[i]] = 1;
					VisitNodeCnt++;
				}
			}

		}
		if(preVisitLinkCnt == VisitLinkCnt)
			nCnt++;
	}
	idx.clear();


	//3. Algorithm

	//주행성공여부(node)
	bool bNodeCheck = false;
	bool bAllCheck = false;

	//리스트 초기화
	DrivingTestList.clear();

	//처음 리스트에 넣을 데이터 세팅
	DrivingTestTemp.index = 1;
	DrivingTestTemp.indexNode = nStartNode;
	DrivingTestTemp.VisitLinkCnt = 0;
	DrivingTestTemp.VisitNodeCnt = 0;
	ZeroMemory(&DrivingTestTemp.NodeArray, sizeof(DrivingTestTemp.NodeArray));
	ZeroMemory(&DrivingTestTemp.StopOverArray, sizeof(DrivingTestTemp.StopOverArray));

	DrivingTestTemp.NodeArray[0] = nStartNode;
	DrivingTestTemp.StopOverArray[0] = 0;

	//리스트 iteratior, 최초 리스트 삽입
	nCnt = 0;
	list<DrivingTestSt>::iterator iterDrivingTestSt;
	DrivingTestList.push_back(DrivingTestTemp);


	try
	{
		dwPathSearchTime = timeGetTime();
		//알고리즘  시작
		while(!DrivingTestList.empty())
		{
			//시간초과 확인 2.5초 이상시 Fail 처리
			if (OHTUTILITY::PreventOverflowGetTimeMs(dwPathSearchTime) > 2500)
					break;

			//리스트의 사이즈가 20만 이상일 경우 Fail 처리 60만 이상일 경우 SIZE FULL
			nListSize = DrivingTestList.size();
			if(nListSize > 200000)
				break;

			//리스트의 맨 앞데이터를 POP..
			iterDrivingTestSt = DrivingTestList.begin();
			nIndex = iterDrivingTestSt->index;
			nIndexNode = iterDrivingTestSt->indexNode;
			nVisitLinkCnt = iterDrivingTestSt->VisitLinkCnt;
			nVisitNodeCnt = iterDrivingTestSt->VisitNodeCnt;

			memset(&NodeArray, 0, sizeof(NodeArray));
			memset(&StopOverArray, 0, sizeof(StopOverArray));
			memmove(&NodeArray,  iterDrivingTestSt->NodeArray, sizeof(iterDrivingTestSt->NodeArray));
			memmove(&StopOverArray,  iterDrivingTestSt->StopOverArray, sizeof(iterDrivingTestSt->StopOverArray));

			DrivingTestList.pop_front();



			//노드체크중.. 연산회수 최소화
			if(!bNodeCheck)
			{
				if(nVisitNodeMaxCnt <= nVisitNodeCnt)
				{
					nVisitNodeMaxCnt = nVisitNodeCnt;
				}
				else
				{
					continue;
				}
			}

			//성공조건1 - 경로탐색(노드주행) 완료
			if(VisitNodeCnt == nVisitNodeCnt && !bNodeCheck)
			{
				bNodeCheck = true;
				if(nIndexNode == nStartNode)
					bAllCheck = true;
				DrivingTestList.clear();
			}
			//성공조건2 - 경로탐색 완료 후, 시작지점으로 복귀
			else if(bNodeCheck && !bAllCheck)
			{
				//성공조건2 - 시작지점으로 복귀완료
				if(nIndexNode == nStartNode)
					bAllCheck = true;

			}
			//실패조건, Index가 250 이상일 때, 중지
			else if(nIndex >= 500 || nVisitNodeCnt >= 100)
			{
				bSuccess = false;
				break;

			}

			//탐색이 모두 진행완료상태인지  확인.
			if(bAllCheck)
			{
				FILE* fp;
				fp = fopen("DataFiles\\NavigationRoute.txt", "w");

				if(fp == NULL) return false;
				int S_Index = 0;

				int n = 0;
				int PreNode = 0;
				int NodeIndex = 0;
				double DistSum = 0;
				for(int i = 0; i<nIndex; i++)
				{
					// Link 거리 계산
					if(PreNode!=0)
					{
						NodeIndex = pMap->FindNodeIndex(PreNode);
						pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

						//거리계산
						for(int j = 0; j<2; j++)
						{
							int nPathIndex = pNodeInfo->NextPathIndex[j];
							if(nPathIndex != -1)
							{
								pPathInfo = &(pMap->m_PathArray[nPathIndex]);
								if(NodeArray[i] == pPathInfo->EndNID)
								{
									DistSum += (double)pPathInfo->Distance;
								}
							}
						}
					}
					n++;
					//경유지 1개씩 자르는 부분
					if(i!= 0 && NodeArray[i] == StopOverArray[S_Index] && n>nLinkSetting-1)
					{
						fprintf(fp, "%d %.0f\n", NodeArray[i], DistSum);
						ADD_LOG("EndID:%d, DistSum:%.0f ", NodeArray[i],DistSum);
						S_Index++;
						PreNode = 0;
						DistSum = 0;
						n = 0;
					}

					// 노드 1개씩 찍는 부분
					if(i < nIndex-1)
					{
					   fprintf(fp, "%d ", NodeArray[i]);
					   PreNode = NodeArray[i];
					}
					else
					{
						fprintf(fp, "%d %.0f\n", NodeArray[i], DistSum);
						ADD_LOG("EndID:%d, DistSum:%.0f ", NodeArray[i],DistSum);
					}


				}
				fclose(fp);
				bSuccess = true;
				break;
			}


			// 그 외인 경우 아래 탐색로직 진행.


			//레인컷 체크
			int NodeIndex = pMap->FindNodeIndex(nIndexNode);
			int BlockCheck = 0;
			pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

			for(int i = 0; i<2; i++)
			{
				int nPathIndex = pNodeInfo->NextPathIndex[i];

				if(nPathIndex != -1)
				{
					pPathInfo = &(pMap->m_PathArray[nPathIndex]);
					nTempNextNode[i] = pPathInfo->EndNID;
				}
				else
				{
					BlockCheck++;
					nTempNextNode[i] = -1;
				}
			}

			//막혀있는 경우
			if(BlockCheck >=2)
				break;

			int nCount = LaneCutList.getCount();
			for(int i = 0; i < nCount; i++)
			{
				LaneCutSt* TempLanCut = (LaneCutSt*)LaneCutList.refer(i);
				if(TempLanCut->PreNode == nIndexNode)
				{
					if(TempLanCut->NextNode ==  nTempNextNode[0])
					{
						nTempNextNode[0] = nTempNextNode[1];
						nTempNextNode[1] = -1;
					}
					else if(TempLanCut->NextNode ==  nTempNextNode[1])
					{
						nTempNextNode[1] = -1;
					}
					else
					{
						break;
					}
				}
			}

			//경로 수 체크
			int nCntChk = nTempNextNode[1] < 0 ? 1 : 2;


			//경로 수만큼 리스트에 쌓는다 -> 핵심 로직
			for(int i = 0; i < nCntChk; i++)
			{
				int nextNode = nTempNextNode[i];
				memset(&DrivingTestTemp, 0, sizeof(DrivingTestTemp));


				//데이터 쓰기
					DrivingTestTemp.index = nIndex;
					DrivingTestTemp.VisitLinkCnt = nVisitLinkCnt;
					DrivingTestTemp.VisitNodeCnt = nVisitNodeCnt;
					memcpy(&DrivingTestTemp.NodeArray, &NodeArray ,sizeof(DrivingTestTemp.NodeArray));
					memcpy(&DrivingTestTemp.StopOverArray, &StopOverArray ,sizeof(DrivingTestTemp.StopOverArray));

					DrivingTestTemp.indexNode = nTempNextNode[i];
					DrivingTestTemp.NodeArray[DrivingTestTemp.index++] = nextNode;




				if(!bNodeCheck)
				{
					//위 로직에서 링크개수가 기준 이상일 때, 노드체크 시작
					VisitNodeCheck(&DrivingTestTemp,nLinkSetting);

				}

				//리스트에 추가
				DrivingTestList.push_back(DrivingTestTemp);
			}

			nCnt++;
		}
	}
	catch(...)
	{
		ADD_LOG("Abnormal Case");
	}

	//리스트 초기화
	while(!DrivingTestList.empty())
	{
		DrivingTestList.pop_front();
	}
	DrivingTestList.clear();
	LaneCutList.clear();

	return bSuccess;
}

bool PathSearch::VisitLinkCheck(DrivingTestSt* pDrivingTestSt, int VisitLinkCnt)
{
	ZeroMemory(VisitLinkCheckArray, sizeof(VisitLinkCheckArray));

	int cnt = pDrivingTestSt->index;
	int VisitCnt = 0;
	int pre_node, next_node;
	bool bSuccess = false;

	for(int i = 0; i<cnt-1; i++)
	{
		pre_node = pDrivingTestSt->NodeArray[i];
		next_node = pDrivingTestSt->NodeArray[i+1];

		if(VisitLinkCheckArray[pre_node][0] == 0)
		{
			 VisitCnt++;
			 VisitLinkCheckArray[pre_node][0] = next_node;
		}
		else
		{
			if(VisitLinkCheckArray[pre_node][0] != next_node)
			{
				if(VisitLinkCheckArray[pre_node][1] == 0)
				{
					VisitCnt++;
					VisitLinkCheckArray[pre_node][1] = next_node;
				}
			}
		}
	}

	pDrivingTestSt->VisitLinkCnt = VisitCnt;

	if(VisitCnt == VisitLinkCnt)
		bSuccess = true;

	return	bSuccess;
}


bool PathSearch::VisitNodeCheck(DrivingTestSt* pDrivingTestSt, int nLinkSetting)
{
	bool bSuccess = true;
	int VisitCnt = 0;
	int CurrentNode;
	int nextNode = pDrivingTestSt->indexNode;
	int cnt = pDrivingTestSt->VisitNodeCnt;
	int index = pDrivingTestSt->index - 1;
	int StopIndex =  pDrivingTestSt->VisitNodeCnt-1;
	int CntCheck = 0;

	for(int i = 0; i<cnt; i++)
	{
		CurrentNode = pDrivingTestSt->StopOverArray[i];

		if(CurrentNode == nextNode)
		{
			bSuccess = false;

			break;
		}
	}

	if(bSuccess)
	{
    	if(StopIndex >= 0)
		{
			for(int i = index; i>=0; i--)
			{
				CntCheck++;
				if(pDrivingTestSt->NodeArray[i] == 	pDrivingTestSt->StopOverArray[StopIndex])
				{
					break;
				}
			}
		}
		else
		{
			CntCheck = index + 1;
		}
	}

	if(bSuccess && CntCheck > nLinkSetting)
	{
		pDrivingTestSt->StopOverArray[pDrivingTestSt->VisitNodeCnt++] = nextNode;
	}


	return bSuccess;
}



