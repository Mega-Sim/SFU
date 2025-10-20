/**
@file    MAP.cpp
@version OHT 7.0
@brief   MAP과 관련된 Class 멤버 함수를 구현한 cpp 파일
*/

#pragma hdrstop

#include <iostream>
#include <fstream>
#include <locale.h> // 경로명에 한글이 있는 경우를 위해 추가
#include "MAP.h"
#include "OHTMain.h"
#include "ErrorCode.h"

#define ADD_LOG_MAPMAKE_STD(...)        WRITE_LOG_UNIT(pLogMapMakeSTD, __VA_ARGS__)
LogUnit *pLogMapMakeSTD; ///< Map Make STD관련 로그 변수

using namespace std;
//---------------------------------------------------------------------------

/**
@brief   MAP Class default 생성자 

         변수, 포인터 변수 초기화
@author  zzang9un
@date    2012.11.09
*/
MAP::MAP()
{
    m_PathNum = 0;
    m_NodeNum = 0;

	m_FlowcontrolCheckResult = NO_ERR;
	
    m_PathArray = NULL;
    m_NodeArray = NULL;
	m_FlowCheckArray = NULL;
    
    // PathMap, NodeMap 초기화
    m_MakeRightNodeMap.clear();
    m_NodeMap.clear();
	m_TeachingMap.clear();
	m_LookDownMap.clear();
	m_CIDResetMap.clear();
	m_CIDStopMap_1.clear();
	m_CIDStopMap_2.clear();
	m_CIDStartMap_1.clear();
	m_CIDStartMap_2.clear();

	m_FlowControl.clear();

	tNodeDisalbeList = new TStringList;
	tPassBoxNodeList = new TStringList;

	ReadNodeDisableFile();
	ReadPassBoxNodeFile();

	pLogMapMakeSTD = getLogUnit("D:\\log\\MapMakeSTD.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
}
//---------------------------------------------------------------------------

/**
@brief   MAP Class 소멸자.

         동적 할당된 배열, 포인터 변수를 delete한다.
@author  
@date    2012.11.09
@note    
*/
MAP::~MAP()
{
    if(m_PathArray != NULL)
        delete [] m_PathArray;

    if(m_NodeArray != NULL)
        delete [] m_NodeArray;

	if(m_FlowCheckArray != NULL)
		delete [] m_FlowCheckArray;

    m_MakeRightNodeMap.clear();
    m_NodeMap.clear(); 
	m_TeachingMap.clear();
	m_LookDownMap.clear();
	m_CIDResetMap.clear();
	m_CIDStopMap_1.clear();
	m_CIDStopMap_2.clear();
	
	if(tNodeDisalbeList != NULL)
		delete tNodeDisalbeList;

	if(tPassBoxNodeList != NULL)
		delete tPassBoxNodeList;
	
}
//---------------------------------------------------------------------------

/**
@brief   Node 구조체의 Node ID를 map 컨테이너로 변환시키는 함수.
@author  zzang9un
@date    2012.11.09
*/
void MAP::MakeNodeMap()
{
    if((m_NodeNum == 0) || (m_NodeArray == NULL))
        return;

	m_NodeMap.clear(); // 신규 파일에 노드가 더 많은경우 갱신이 안됨. update시 m_NodeMap.clear 필요. 
    for(int i = 0; i < m_NodeNum; i++)
    {
        // Key는 Node ID, Value는 배열 Index
        m_NodeMap[m_NodeArray[i].NID] = i;
    }   
}
//---------------------------------------------------------------------------

/**
@brief   Node 구조체의 Right Node ID를 map 컨테이너로 변환시키는 함수.
         Collision 정보에서 m_PathArray의 Offset 값을 찾기위함.
@author  KJD
@date    2016.06.01
*/
void MAP::MakeRightNodeMapFromPathArray()
{
    if((m_NodeNum == 0) || (m_NodeArray == NULL))
        return;

    for(int i = 0; i < (m_PathNum-m_NodeNum); i++)
    {
        // Key는 Right Node ID, Value는 배열 Index
		m_MakeRightNodeMap[m_PathArray[m_NodeNum+i].EndNID] = i;
    }   
}

//---------------------------------------------------------------------------
/**
@brief   Teaching 구조체의 Station ID를 map 컨테이너로 변환시키는 함수.
@author  KJD
@date    2016.06.08
*/
void MAP::MakeTeachingMap(NODE_INFO_STD_TEACHING *m_NodeArray_Teach, UINT DataNum)
{
    if(DataNum == 0)
        return;

    for(int i = 0; i < DataNum; i++)
    {
        // Key는 Station ID, Value는 배열 Index
        m_TeachingMap[m_NodeArray_Teach[i].StationID] = i;
    }   
}
//---------------------------------------------------------------------------
/**
@brief   CID 구조체의 Reset, Stop node ID를 map 컨테이너로 변환시키는 함수.
@author  KJD
@date    2016.07.28
*/
#if(OHT_NAME != OHT_NAME_P4_MASK)
void MAP::MakeCIDMap(NODE_INFO_STD_CID * m_NodeArray_CID, UINT DataNum)
{
    if(DataNum == 0)
        return;

    for(int i = 0; i < DataNum; i++)
    {
        // Key는 CID Reset ID, Value는 배열 Index
        m_CIDResetMap[m_NodeArray_CID[i].ResetNode] = i;
        m_CIDStopMap_1[m_NodeArray_CID[i].StopNode1] = i;
		m_CIDStopMap_2[m_NodeArray_CID[i].StopNode2] = i;

		m_CIDStartMap_1[m_NodeArray_CID[i].CommStartNode1_1] = i;
		m_CIDStartMap_1[m_NodeArray_CID[i].CommStartNode1_2] = i;
		m_CIDStartMap_1[m_NodeArray_CID[i].CommStartNode1_3] = i;
		m_CIDStartMap_1[m_NodeArray_CID[i].CommStartNode1_4] = i;
		m_CIDStartMap_1[m_NodeArray_CID[i].CommStartNode1_5] = i;

		m_CIDStartMap_2[m_NodeArray_CID[i].CommStartNode2_1] = i;
		m_CIDStartMap_2[m_NodeArray_CID[i].CommStartNode2_2] = i;
		m_CIDStartMap_2[m_NodeArray_CID[i].CommStartNode2_3] = i;
		m_CIDStartMap_2[m_NodeArray_CID[i].CommStartNode2_4] = i;
		m_CIDStartMap_2[m_NodeArray_CID[i].CommStartNode2_5] = i;

	}
}
#endif
/**
@brief   Flowcontrol 구조체의 Reset, Stop node ID를 map 컨테이너로 변환시키는 함수.
@author   puting
@date    2016.07.28
*/
void MAP::MakeFlowControlMap(NODE_INFO_STD_FLOWCONTROL * m_FlowArray, UINT DataNum)
{
	map<int, int>::iterator FindIter;
    bool nbreak = false;
	int nTempStopNode = 0;
	int StopResetCNT = 0;
	unsigned long long StopResetNode = 0;

	if(DataNum == 0)
		return;

	m_FlowControl_StopReset.clear();

	for(int i = 0; i < DataNum; i++)
	{
		// Key는 CID Reset ID, Value는 배열 Index
        nTempStopNode =  m_FlowArray[i].StopNodeID;
		//4개 Stop Node 중복된거에 대한 처리 부분.
		for(int j = 0; j <4; j++)
		{
			nTempStopNode = nTempStopNode + 1000000;

			FindIter = m_FlowControl.find(nTempStopNode);

            if(FindIter == m_FlowControl.end())
			{
				// Node를 못 찾은 경우
				m_FlowControl[nTempStopNode] = i;
				break;
			}
		}
		//Flow Control 모든 합류부 검사를 위한 Map 컨테이너 생성
		StopResetNode = (unsigned long long)m_FlowArray[i].StopNodeID << 32 | (unsigned long long)m_FlowArray[i].ResetNodeMain;
		m_FlowControl_StopReset[StopResetNode]=StopResetCNT++;
//		if(m_FlowArray[i].StopNodeID == 1010)
//		ADD_LOG_MAPMAKE_STD("m_FlowControl_StopReset %6d, %6d, %6d",m_FlowArray[i].StopNodeID, m_FlowArray[i].ResetNodeMain, StopResetCNT);
		
		if(m_FlowArray[i].ResetNodeSub != 0)
		{
			StopResetNode = (unsigned long long)m_FlowArray[i].StopNodeID << 32 | (unsigned long long)m_FlowArray[i].ResetNodeSub;
			m_FlowControl_StopReset[StopResetNode]=StopResetCNT++;
//			if(m_FlowArray[i].StopNodeID == 1010)
//			ADD_LOG_MAPMAKE_STD("m_FlowControl_StopReset %6d, %6d, %6d",m_FlowArray[i].StopNodeID, m_FlowArray[i].ResetNodeSub, StopResetCNT);
		}
		//sleep(10);
	}
	ADD_LOG_MAPMAKE_STD("m_FlowControl_StopReset StopResetCNT : %d",StopResetCNT);
}
//---------------------------------------------------------------------------
/**
@brief   합류부 통과허가 유무 검사
@author  KJD
@date    2016.09.01
*/
int MAP::CheckFlowControlData(unsigned long long StopResetNode)
{
    int ret = 0;
	ret = FindIndex_FlowControl_StopReset(StopResetNode);
	if(ret == INDEX_NONE)
	{
		return -1;
	}
	else
	{
		return NO_ERR;
	}
}
//---------------------------------------------------------------------------


/**
@brief   MAP Class 생성자 

         Path와 Node 개수를 이용하여 동적 생성한다.
@author  zzang9un
@date    2012.11.09
@param   PathNum : Path 개수
@param   NodeNum : Node 개수
*/
void MAP::InitMap(int PathNum, int NodeNum)
{
    m_PathNum = PathNum;
    m_NodeNum = NodeNum;
    
    // 동적 생성하기 전 Null인지를 확인 후 생성
    if(m_PathArray != NULL)
    {
        // NULL이 아닌 경우는 delete 후에 동적 생성
		delete [] m_PathArray;
		m_PathArray = new PATH_INFO[PathNum];
    }
    else
		m_PathArray = new PATH_INFO[PathNum];
    
    if(m_NodeArray != NULL)
    {
        // NULL이 아닌 경우는 delete 후에 동적 생성
        delete [] m_NodeArray;
        m_NodeArray = new NODE_INFO[NodeNum];
    }
    else
        m_NodeArray = new NODE_INFO[NodeNum];

	if(m_FlowCheckArray != NULL)
	{
		// NULL이 아닌 경우는 delete 후에 동적 생성
		delete [] m_FlowCheckArray;
		m_FlowCheckArray = new FLOW_INFO[NodeNum];
	}
	else
		m_FlowCheckArray = new FLOW_INFO[NodeNum];


    m_MakeRightNodeMap.clear();
    m_NodeMap.clear();
	m_TeachingMap.clear();
	m_LookDownMap.clear();
	m_CIDResetMap.clear();
	m_CIDStopMap_1.clear();
	m_CIDStopMap_2.clear();
	m_CIDStartMap_1.clear();
	m_CIDStartMap_2.clear();

	m_FlowControl.clear();
   
}
//---------------------------------------------------------------------------

/**
@brief   map 컨테이너로 변환시키는 함수.
@author  zzang9un
@date    2012.11.09
*/
int MAP::MakeMap(MAP *pMapdata,
				NODE_INFO_STD_MAP * m_NodeArray_Map,
				UINT NodeNum_Map)
{
int ret = 0;
ADD_LOG_MAPMAKE_STD("MakeMap Step1");
	MakeNodeMap();
ADD_LOG_MAPMAKE_STD("MakeMap Step2");
	MakeRightNodeMapFromPathArray();
ADD_LOG_MAPMAKE_STD("MakeMap Step3");
	// Next Path의 Index를 연결 시켜 주는 함수
	ret = MakePathIndex(pMapdata,
						m_NodeArray_Map,
						NodeNum_Map);
ADD_LOG_MAPMAKE_STD("MakeMap Step4");
	return ret;
}
//---------------------------------------------------------------------------

/**
@brief   Node ID에 매칭되는 Path 배열의 index를 리턴하는 함수.
@author  zzang9un
@date    2012.11.09
@param   NodeID : 찾고자 하는 Node ID
@return  Node ID에 매칭되는 배열 index
@todo    에러 코드 재정의
*/
int MAP::FindNodeIndex(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_NodeMap.find(NodeID);

    if(FindIter != m_NodeMap.end())
    {
        // Node를 찾은 경우
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}

/**
@brief	 NextNode ID에 매칭되는 Path 배열의 index를 리턴하는 함수.
@author  KJD
@date	 2017.9.01
@param	 NodeID : 찾고자 하는 NextNode ID
@return  NextNode ID에 매칭되는 배열 index
@todo	 
*/
int MAP::FindNextIndex(int NodeID)
{
	map<int, int>::iterator FindIter;

	FindIter = m_NextNode.find(NodeID);

	if(FindIter != m_NextNode.end())
	{
		// Node를 찾은 경우
		return FindIter->second;
	}	 
	else
	{
		return INDEX_NONE;
	}		 
}
/**
@brief	 NextNode ID에 매칭되는 Path 배열의 index를 리턴하는 함수.
@author  KJD
@date	 2017.9.01
@param	 NodeID : 찾고자 하는 NextNode ID
@return  NextNode ID에 매칭되는 배열 index
@todo	 
*/
int MAP::FindIndex_FlowControl_StopReset(unsigned long long NodeID)
{
	map<unsigned long long, int>::iterator FindIter;

	FindIter = m_FlowControl_StopReset.find(NodeID);

	if(FindIter != m_FlowControl_StopReset.end())
	{
		// Node를 찾은 경우
		return FindIter->second;
	}	 
	else
	{
		return INDEX_NONE;
	}		 
}
/**
@brief	 NextNode ID에 매칭되는 Path 배열의 index를 리턴하는 함수.
@author  KJD
@date	 2017.9.01
@param	 NodeID : 찾고자 하는 NextNode ID
@return  NextNode ID에 매칭되는 배열 index
@todo	 

int MAP::FindIndex_RailMerge_StopReset(unsigned long long NodeID)
{
	map<unsigned long long, int>::iterator FindIter;

	FindIter = m_RailMerge_StopReset.find(NodeID);

	if(FindIter != m_RailMerge_StopReset.end())
	{
		// Node를 찾은 경우
		return FindIter->second;
	}	 
	else
	{
		return INDEX_NONE;
	}		 
}
*/



//---------------------------------------------------------------------------
/**
@brief   Right Node ID에 매칭되는 Path 배열의 index를 리턴하는 함수.
@author  KJD
@date    2016.06.01
@param   NodeID : 찾고자 하는 Right Node ID
@return  Node ID에 매칭되는 배열 index
@todo    에러 코드 재정의
*/
int MAP::FindRightNodeIndex(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_MakeRightNodeMap.find(NodeID);

    if(FindIter != m_MakeRightNodeMap.end())
    {
        // Node를 찾은 경우
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}

//---------------------------------------------------------------------------
/**
@brief   Station ID에 매칭되는 Path 배열의 index를 리턴하는 함수.
@author  KJD
@date    2016.06.08
@param   NodeID : 찾고자 하는 Station ID
@return  Station ID에 매칭되는 배열 index

int MAP::FindStationIndexFromTeachingMap(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_TeachingMap.find(NodeID);

    if(FindIter != m_TeachingMap.end())
    {
        // Node를 찾은 경우
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}
*/
//---------------------------------------------------------------------------
/**
@brief   Station ID에 매칭되는 Path 배열의 index를 리턴하는 함수.
@author  KJD
@date    2016.06.08
@param   NodeID : 찾고자 하는 Station ID
@return  Station ID에 매칭되는 배열 index

int MAP::FindStationIndexFromLookDownMap(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_LookDownMap.find(NodeID);

    if(FindIter != m_LookDownMap.end())
    {
        // Node를 찾은 경우
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}
*/


//---------------------------------------------------------------------------

/**
@brief   Reset Node ID에 매칭되는 CID 배열의 index를 리턴하는 함수.
@author  KJD
@date    2016.07.28
@param   NodeID : 찾고자 하는 Node ID
@return  Node ID에 매칭되는 배열 index
@todo    에러 코드 재정의
*/
int MAP::FindNodeIndexFromCIDReset(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_CIDResetMap.find(NodeID);

    if(FindIter != m_CIDResetMap.end())
    {
        // Node를 찾은 경우
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}
//---------------------------------------------------------------------------
/**
@brief   StopNode1 ID에 매칭되는 CID 배열의 index를 리턴하는 함수.
@author  KJD
@date    2016.07.28
@param   NodeID : 찾고자 하는 Node ID
@return  Node ID에 매칭되는 배열 index
@todo    에러 코드 재정의
*/
int MAP::FindNodeIndexFromCIDStop_1(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_CIDStopMap_1.find(NodeID);

    if(FindIter != m_CIDStopMap_1.end())
    {
        // Node를 찾은 경우
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}
//---------------------------------------------------------------------------
/**
@brief   StopNode2 ID에 매칭되는 CID 배열의 index를 리턴하는 함수.
@author  KJD
@date    2016.07.28
@param   NodeID : 찾고자 하는 Node ID
@return  Node ID에 매칭되는 배열 index
@todo    에러 코드 재정의
*/
int MAP::FindNodeIndexFromCIDStop_2(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_CIDStopMap_2.find(NodeID);

    if(FindIter != m_CIDStopMap_2.end())
    {
        // Node를 찾은 경우
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}
//---------------------------------------------------------------------------



/**
@brief   Node의 Next Path 인덱스를 Make하는 함수
         MAPDATA에 Next만 저장하여 로딩만 하는 것으로 수정
         MAPDATA에 부모 노드 추가시 관리에 어려움이 있어 추가하기 힘듦.
@author  KJD
@date    2016.05.31
@return  정상인 경우 0, 에러가 발생한 경우 에러 코드 리턴
*/
#if 1
int MAP::MakePathIndex(MAP *pMapdata, NODE_INFO_STD_MAP * m_NodeArray_Map, UINT NodeNum_Map)
{
	int tmpResetLH;
	int tmpResetRH;
	int RailMergeCNT=0;
	int ret = 0;
	int cnt = 0;
	int FlowCnt;
	unsigned long long StopResetNode = 0; 

	m_NextNode.clear();
	m_FlowcontrolCheckResult = NO_ERR;

	UINT tmpArray[1024] = {0,};

	// MTL STOP 의 다음노드는 별도의 배열로 저장
	// MTL 다음노드 배열 중 Node의 다음노드와 동일하다면 Flage true
	for(int i=0 ; i < NodeNum_Map ; i++ )
	{
		m_FlowCheckArray[i].Flowflag = false;     //초기화

		if((m_NodeArray_Map[i].NodeType == MTL_STOP_LEFT) || (m_NodeArray_Map[i].NodeType == MTL_STOP_RIGTH))
		{
			tmpArray[cnt++] = m_NodeArray_Map[i].LeftNodeID;
		}
	}

	for(int j=0 ; j < cnt ; j++ )
	{
		FlowCnt = 0;

		for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			if(FlowCnt >= 2)
			{
				break;
			}

			if((m_NodeArray_Map[i].LeftNodeID == tmpArray[j]) && (m_FlowCheckArray[i].Flowflag == false))
			{
				m_FlowCheckArray[i].Flowflag = true;
				FlowCnt++;
			}
		}
	}

	for(int i=0 ; i < cnt ; i++ )
	{
		ADD_LOG_MAPMAKE_STD("MTL STOP LeftNode List : %d",tmpArray[i]);
	}


	for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			//if( m_NodeArray_Map[i]->LeftNodeID != 0 ) // 노드가 존재시. 의미상 있어야 하나 항상 참이므로 조건비교 안함.
			//m_NodeArray_Map[i]->LeftNodeID의 경로는
			//NodeIndex와 PathIndex는 동일하므로 NodeIndex에서 배열순서를 알아옴.

			int tmp = pMapdata->FindNodeIndex(m_NodeArray_Map[i].NodeID);

           	if( m_NodeArray_Map[i].LeftNodeID != 0 ) // 노드가 존재시.
				m_NodeArray[i].NextPathIndex[0] = tmp; // 좌측 Path Index
			else
                m_NodeArray[i].NextPathIndex[0] = INDEX_NONE;

			if( m_NodeArray_Map[i].RightNodeID != 0 ) // 노드가 존재시.
			{
				tmp = pMapdata->FindRightNodeIndex(m_NodeArray_Map[i].RightNodeID);
				if( pMapdata->m_PathArray[NodeNum_Map+tmp].EndNID == m_NodeArray_Map[i].RightNodeID )
				{
					m_NodeArray[i].NextPathIndex[1] = NodeNum_Map+tmp; //우측 Path Index
				}
				else
				{
					m_NodeArray[i].NextPathIndex[1] = INDEX_NONE;
				}

			}
			else
			{
				m_NodeArray[i].NextPathIndex[1] = INDEX_NONE;
			}

			
			//통과허가 파일 검증을 위한 데이터 조합
			//합류소 찾기 - NextNode가 중복인 경우 
			//리프터 제외
			if(m_NodeArray_Map[i].LeftNodeID != 0 && m_NodeArray_Map[i].NodeType != LIFTIN_NTYPE_LEFT && m_NodeArray_Map[i].NodeType !=LIFTIN_NTYPE_RIGHT && m_FlowCheckArray[i].Flowflag == false)
			{
				tmpResetLH = FindNextIndex(m_NodeArray_Map[i].LeftNodeID);
				if(tmpResetLH == INDEX_NONE)// Path가 없는 경우
				{
					m_NextNode[m_NodeArray_Map[i].LeftNodeID] = i;
				}
				else// NextNode Map에 이미 존재하는 경우 합류점으로 등록.
				{
					//합류점 찾은 후 Flowcontrol에서 검사.
					StopResetNode = (unsigned long long)m_NodeArray_Map[i].NodeID<<32 | (unsigned long long)m_NodeArray_Map[i].LeftNodeID;//stop, reset 조합으로 8byte key 등록
					//m_RailMerge_StopReset[StopResetNode++] = RailMergeCNT++;
					ret = CheckFlowControlData(StopResetNode);
					if(ret == -1)
					{
						ADD_LOG_MAPMAKE_STD("CheckFlowControlData1 err: %d, %d, %d",m_NodeArray_Map[i].NodeID,m_NodeArray_Map[i].LeftNodeID, ret);
						m_FlowcontrolCheckResult = 0xFA;//fail
					}
				}
			}
			if(m_NodeArray_Map[i].RightNodeID != 0 && m_NodeArray_Map[i].NodeType != LIFTIN_NTYPE_LEFT && m_NodeArray_Map[i].NodeType !=LIFTIN_NTYPE_RIGHT && m_FlowCheckArray[i].Flowflag == false)
			{
				tmpResetRH = FindNextIndex(m_NodeArray_Map[i].RightNodeID);
				if(tmpResetRH == INDEX_NONE)
				{
					m_NextNode[m_NodeArray_Map[i].RightNodeID]=NodeNum_Map+i;
				}
				else// NextNode Map에 이미 존재하는 경우 합류점으로 등록.
				{
					//합류점 찾은 후 Flowcontrol에서 검사.
					StopResetNode = (unsigned long long)m_NodeArray_Map[i].NodeID<<32 | (unsigned long long)m_NodeArray_Map[i].RightNodeID;//stop, reset 조합으로 8byte key 등록
					//m_RailMerge_StopReset[StopResetNode] = RailMergeCNT++;
					ret = CheckFlowControlData(StopResetNode);
					if(ret == -1)
					{
						ADD_LOG_MAPMAKE_STD("CheckFlowControlData2 err: %d, %d, %d",m_NodeArray_Map[i].NodeID,m_NodeArray_Map[i].RightNodeID, ret);
						m_FlowcontrolCheckResult = 0xFA;//fail
					}
				}
			}
		}
	m_NextNode.clear();
	for(int i=NodeNum_Map-1 ; i > 0 ; i-- )
		{			
			//통과허가 파일 검증을 위한 데이터 조합
			//합류소 찾기 - NextNode가 중복인 경우 
			//리프터 제외
			if(m_NodeArray_Map[i].LeftNodeID != 0 && m_NodeArray_Map[i].NodeType != LIFTIN_NTYPE_LEFT && m_NodeArray_Map[i].NodeType !=LIFTIN_NTYPE_RIGHT && m_FlowCheckArray[i].Flowflag == false)
			{
				tmpResetLH = FindNextIndex(m_NodeArray_Map[i].LeftNodeID);
				if(tmpResetLH == INDEX_NONE)// 
				{
					m_NextNode[m_NodeArray_Map[i].LeftNodeID] = i;
				}
				else// NextNode Map에 이미 존재하는 경우 합류점으로 등록.
				{
					//합류점 찾은 후 Flowcontrol에서 검사.
					StopResetNode = (unsigned long long)m_NodeArray_Map[i].NodeID<<32 | (unsigned long long)m_NodeArray_Map[i].LeftNodeID;//stop, reset 조합으로 8byte key 등록
					//m_RailMerge_StopReset[StopResetNode++] = RailMergeCNT++;
					ret = CheckFlowControlData(StopResetNode);
					if(ret == -1)
					{
						ADD_LOG_MAPMAKE_STD("CheckFlowControlData1 err: %d, %d, %d",m_NodeArray_Map[i].NodeID,m_NodeArray_Map[i].LeftNodeID, ret);
						m_FlowcontrolCheckResult = 0xFA;//fail
					}
				}
			}
			if(m_NodeArray_Map[i].RightNodeID != 0 && m_NodeArray_Map[i].NodeType != LIFTIN_NTYPE_LEFT && m_NodeArray_Map[i].NodeType !=LIFTIN_NTYPE_RIGHT && m_FlowCheckArray[i].Flowflag == false)
			{
				tmpResetRH = FindNextIndex(m_NodeArray_Map[i].RightNodeID);
				if(tmpResetRH == INDEX_NONE)
				{
					m_NextNode[m_NodeArray_Map[i].RightNodeID]=NodeNum_Map+i;
				}
				else// NextNode Map에 이미 존재하는 경우 합류점으로 등록.
				{
					//합류점 찾은 후 Flowcontrol에서 검사.
					StopResetNode = (unsigned long long)m_NodeArray_Map[i].NodeID<<32 | (unsigned long long)m_NodeArray_Map[i].RightNodeID;//stop, reset 조합으로 8byte key 등록
					//m_RailMerge_StopReset[StopResetNode] = RailMergeCNT++;
					ret = CheckFlowControlData(StopResetNode);
					if(ret == -1)
					{
						ADD_LOG_MAPMAKE_STD("CheckFlowControlData2 err: %d, %d, %d",m_NodeArray_Map[i].NodeID,m_NodeArray_Map[i].RightNodeID, ret);
						m_FlowcontrolCheckResult = 0xFA;//fail
					}
				}
			}
		}

	return m_FlowcontrolCheckResult;
}


#else
/**
@brief   Node의 Pre, Next Path 인덱스를 Make하는 함수
@author  zzang9un
@date    2012.11.30
@return  정상인 경우 0, 에러가 발생한 경우 에러 코드 리턴
*/

int MAP::MakePathIndex(NODE_INFO_STD_MAP * m_NodeArray_Map)
{
    bool bPreFind = false;
    bool bNextFind = false;
    int PreFindCnt = 0;
    int NextFindCnt = 0;
    
    for(int i = 0; i < m_NodeNum; i++)
    {
        for(int j = 0; j < m_PathNum; j++)
        {
			// Pre Path 검색
			/*
            if(m_NodeArray[i].NID == m_PathArray[j].EndNID)
            {
                if(bPreFind)
                {
                    // 에러 처리를 위해 추가
                    if(PreFindCnt >= 3)
                        return -1;
                    
                    // by zzang9un 2012.11.30 : 한번 더 검색이 된 경우
                    m_NodeArray[i].PrePathIndex[PreFindCnt] = j;
                    PreFindCnt++;
                }
                else
                {
                    // by zzang9un 2012.11.30 : 최초 검색된 경우
                    m_NodeArray[i].PrePathIndex[PreFindCnt] = j;
                    PreFindCnt++;
                    bPreFind = true;
                }
            }
             */
            // Next Path 검색
            if(m_NodeArray[i].NID == m_PathArray[j].StartNID)
            {
                if(bNextFind)
                {
                    // 에러 처리를 위해 추가
                    if(NextFindCnt >= 3)
                        return -1;

                    // by zzang9un 2012.11.30 : 한번 더 검색이 된 경우
                    m_NodeArray[i].NextPathIndex[NextFindCnt] = j;
                    NextFindCnt++;  // by zzang9un 2013.08.02 : 빼먹어서 버그 발견
                }
                else
                {
                    // by zzang9un 2012.11.30 : 최초 검색된 경우
                    m_NodeArray[i].NextPathIndex[NextFindCnt] = j;
                    NextFindCnt++;
                    bNextFind= true;
                }
            }
        }

        // 결과 값 저장 후 처리해야 하는 부분

		// Pre Path 체크
		/*
        if(bPreFind == true)
        {
            // by zzang9un 2013.08.02 : 연결된 Path가 없는 경우도 고려해야 함
            switch(PreFindCnt)
            {
            case 0: // 연결된 Pre Path가 없는 경우
                m_NodeArray[i].PrePathIndex[0] = INDEX_NONE;
                m_NodeArray[i].PrePathIndex[1] = INDEX_NONE;
                break;

            case 1: // 연결된 Pre Path가 1개만 있는 경우
                // 1개만 찾은 경우 나머지 값은 0xFFFFFFFF로 초기화    
                m_NodeArray[i].PrePathIndex[1] = INDEX_NONE;
                break;

            case 2: // 연결된 Pre Path가 2개 있는 경우
                // 2개의 값이 있는 경우는 dist가 적은 값이 먼저 오도록 수정    
                ComparePathDistance(m_NodeArray[i].PrePathIndex);
                break;    
            }
        }
        else
        {
            // by zzang9un 2013.08.02 : 버그 수정함(0, 1 모두 변경)
            // 찾지 못한 경우에는 모두 0xFFFFFFFF로 초기화
            m_NodeArray[i].PrePathIndex[0] = INDEX_NONE;
            m_NodeArray[i].PrePathIndex[1] = INDEX_NONE;
        }
         */
        // Next Path 체크
        if(bNextFind == true)
        {
            // by zzang9un 2013.08.02 : 연결된 Path가 없는 경우도 고려해야 함
            switch(NextFindCnt)
            {
            case 0: // 연결된 Next Path가 없는 경우
                m_NodeArray[i].NextPathIndex[0] = INDEX_NONE;
                m_NodeArray[i].NextPathIndex[1] = INDEX_NONE;
                break;

            case 1: // 연결된 Next Path가 1개만 있는 경우
                // 1개만 찾은 경우 나머지 값은 0xFFFFFFFF로 초기화
                m_NodeArray[i].NextPathIndex[1] = INDEX_NONE;
                break;

            case 2: // 연결된 Next Path가 2개 있는 경우
                // 2개의 값이 있는 경우는 dist가 적은 값이 먼저 오도록 수정    
                ComparePathDistance(m_NodeArray[i].NextPathIndex);
                break;    
            }            
        }
        else
        {
            // by zzang9un 2013.08.02 : 버그 수정함(0, 1 모두 변경)
            // 찾지 못한 경우에는 모두 0xFFFFFFFF로 초기화
            m_NodeArray[i].NextPathIndex[0] = INDEX_NONE;
            m_NodeArray[i].NextPathIndex[1] = INDEX_NONE;
        }

        // for문 새로 진행하기 전 변수 초기화
		PreFindCnt = 0;
        bPreFind = false;

        NextFindCnt = 0;
        bNextFind = false;
    }
}
#endif
//---------------------------------------------------------------------------

/**
@brief   서로 다른 두 Path의 거리를 비교하여 swap하는 함수
@author  zzang9un
@date    2012.11.30
*/
void MAP::ComparePathDistance(UINT *PathIndex)
{
    // Dist가 짧은 Path의 Index를 0 index에 넣도록 체크
	// 수정내용 :
#if(USE_CHANGENODE == 1)
	if((m_PathArray[PathIndex[1]].UBGRegion == PATH_DIRECTION_LEFT) ||
	   (m_PathArray[PathIndex[0]].UBGRegion == PATH_DIRECTION_RIGHT) ||
	   (m_PathArray[PathIndex[0]].UBGRegion == PATH_DIRECTION_NARROW) ||
//		((m_PathArray[PathIndex[0]].StartNID == 217) && (m_PathArray[PathIndex[0]].Steering == PATH_STEERING_RIGHT)) ||
	   ((m_PathArray[PathIndex[0]].UBGRegion == PATH_DIRECTION_SHORT) && (m_PathArray[PathIndex[0]].Steering == PATH_STEERING_N_BRANCH_R2L)) ||
	   ((m_PathArray[PathIndex[1]].UBGRegion == PATH_DIRECTION_SHORT) && (m_PathArray[PathIndex[1]].Steering == PATH_STEERING_N_BRANCH_L2R)))

#else
	if((m_PathArray[PathIndex[1]].PathDirection == PATH_DIRECTION_LEFT) ||
	   (m_PathArray[PathIndex[0]].PathDirection == PATH_DIRECTION_RIGHT) ||
	   (m_PathArray[PathIndex[0]].PathDirection == PATH_DIRECTION_NARROW) ||
//		((m_PathArray[PathIndex[0]].StartNID == 217) && (m_PathArray[PathIndex[0]].Steering == PATH_STEERING_RIGHT)) ||
	   ((m_PathArray[PathIndex[0]].PathDirection == PATH_DIRECTION_SHORT) && (m_PathArray[PathIndex[0]].Steering == PATH_STEERING_N_BRANCH_R2L)) ||
	   ((m_PathArray[PathIndex[1]].PathDirection == PATH_DIRECTION_SHORT) && (m_PathArray[PathIndex[1]].Steering == PATH_STEERING_N_BRANCH_L2R)))
#endif
	{
		swap(PathIndex[0], PathIndex[1]);
		ADD_LOG_MAPMAKE_STD("ComparePathDistance");

	}

}
//---------------------------------------------------------------------------

/**
@brief   시작 Node와 끝 Node를 이용하여 해당 Path구조체의 Index를 추출한다.
@author  zzang9un
@date    2012.11.30
@param   StartNID : 시작 Node ID
@param   EndNID : 끝 Node ID
@return  찾은 경우 해당 Path 배열의 Index, 찾지 못한 경우 INDEX_NONE을 리턴
*/
int MAP::GetPathIndex(int StartNID, int EndNID)
{
    int RtnPathIndex = INDEX_NONE;

    UINT StartNodeIndex = 0; // 시작 Node의 index
    UINT EndNodeIndex = 0; // 끝 Node의 Index

    // 각 Node의 배열 Index를 가져온다.
	StartNodeIndex = FindNodeIndex(StartNID);
	//EndNodeIndex = FindNodeIndex(EndNID);

	PATH_INFO  pPATH_INFO;

	if(StartNodeIndex == INDEX_NONE)
	{
    	return -1;
    }

    // 시작과 끝이 연결된 Path를 찾아 Index를 리턴한다.
    for(int i = 0; i < 2; i++)
    {

		if(m_NodeArray[StartNodeIndex].NextPathIndex[i] != INDEX_NONE)
		{
			// 찾은 경우 index를 저장하고 for문은 break
			RtnPathIndex = m_NodeArray[StartNodeIndex].NextPathIndex[i];
			pPATH_INFO = m_PathArray[RtnPathIndex];		// 연결된 Path 정보

			if(pPATH_INFO.EndNID ==EndNID)
				break;
			else
             	RtnPathIndex = INDEX_NONE;
		}

    }

    return RtnPathIndex;
}

/**
@brief   진입 안되는 node 읽어온다.
@author  ks4423.kim
@date    2014.12.16
@param   
@return  찾은 경우 해당 Path 배열의 Index, 찾지 못한 경우 INDEX_NONE을 리턴
*/
int MAP::ReadPassBoxNodeFile(void)
{
    int nError = 0;
	String strOHTPath = "", strFileName = "";

	strOHTPath = ExtractFileDir(Application->ExeName);
    strFileName = String(strOHTPath + STR_DATAFILE_PATH) + NAME_PASS_BOX_NODE;

	if ((strFileName.Length() == 0) || (!FileExists(strFileName)))
    {
		nError = ERR_READ_PASS_BOX_NODE_FILE;
	}

    if (nError == NO_ERR)
    {
	   tPassBoxNodeList->LoadFromFile(strFileName);
	}
	
	return nError;
}

/**
@brief   진입 안되는 node 읽어온다.
@author  ks4423.kim
@date    2014.12.16
@param   
@return  찾은 경우 해당 Path 배열의 Index, 찾지 못한 경우 INDEX_NONE을 리턴
*/
int MAP::ReadNodeDisableFile(void)
{
    int nError = 0;
	String strOHTPath = "", strFileName = "";

	strOHTPath = ExtractFileDir(Application->ExeName);
    strFileName = String(strOHTPath + STR_DATAFILE_PATH) + NAME_NODE_DISABLE;

	if ((strFileName.Length() == 0) || (!FileExists(strFileName)))
    {
		nError = ERR_READ_NODE_DISABLE_FILE;
	}

    if (nError == NO_ERR)
    {
	   tNodeDisalbeList->LoadFromFile(strFileName);
	}
	
	return nError;
}
//---------------------------------------------------------------------------

/**
@brief   MapDatamanager 클래스 디폴트 생성자

         변수 초기화
@author  zzang9un
@date    2013.07.30
*/
MapDataManager::MapDataManager()
{
    m_MapFileLoaded = false;

    memset(&m_MapFileHeader, 0x00, sizeof(MAP_FILE_HEADER));
}
//---------------------------------------------------------------------------

/**
@brief   MapDatamanager 클래스 소멸자

         FileStream 변수 메모리 해제
@author  zzang9un
@date    2012.11.11
*/
MapDataManager::~MapDataManager()
{
    m_MapFileLoaded = false;
}
//---------------------------------------------------------------------------

/**
@brief   Map Data를 File에서 불러오는 함수
@author  zzang9un
@date    2013.07.30
@param   FileName : Map Data가 저장된 파일이름
@param   pMapdata : Map Data가 저장될 구조체 포인터
@return  성공일 경우 0를 리턴, 실패할 경우 에러 코드 리턴
@todo    에러 코드 정의 필요
*/
int MapDataManager::LoadMapFile(TCHAR *FileName, MAP *pMapdata)
{
    setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

    int Error = NO_ERR;

    ifstream fs;

    // Step 1. File Open
    fs.open(FileName, ios::in | ios::binary);

    memset(&m_MapFileHeader, 0x00, sizeof(m_MapFileHeader));

    if(fs.is_open())
    {
        int PathSize = 0; // Path Data 총 size
        int NodeSize = 0; // Node Data 총 size

        // Step 2. Read File Header Information
        MAP_FILE_HEADER FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));
        
        fs.read((char *)&FileHeader, sizeof(MAP_FILE_HEADER));

        // Step 3. Header 파싱
        m_MapFileHeader = ParsingMapFileHeader(&FileHeader);

        if(m_MapFileHeader.PathNum > 0 && m_MapFileHeader.NodeNum > 0)
        {
            // Step 4. 파일에서 Load한 내용에 따라 Init
            pMapdata->InitMap(m_MapFileHeader.PathNum, m_MapFileHeader.NodeNum);

            // Step 5. Path, Node Size만큼 Read
            PathSize = m_MapFileHeader.PathNum * sizeof(PATH_INFO);
            NodeSize = m_MapFileHeader.NodeNum * sizeof(NODE_INFO);

			fs.read((char *)pMapdata->m_PathArray, PathSize);
			fs.read((char *)pMapdata->m_NodeArray, NodeSize);

            // Step 5. Map Data를 map 컨테이너로 make
			//pMapdata->MakeMap(m_NodeArray_Map);

            m_MapFileLoaded = true; // Map File Loaded flag 변경
		}
    }
    else
    {
        Error = ERR_MAP_FILEOPENFAIL;
	}

    fs.close();
    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Map Data를 File에 저장하는 함수
@author  zzang9un
@date    2013.07.30
@param   FileName : Map Data을 저장할 파일 이름
@param   pMapdata : 저장할 Map Data 구조체 포인터
@return  성공일 경우 0를 리턴, 실패할 경우 에러 코드 리턴
@todo    에러 코드 정의 필요
*/
int MapDataManager::SaveMapFile(TCHAR *FileName, MAP *pMapdata)
{
    setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

    int Error = NO_ERR;

    ofstream fs;

    // Step 1. File Open
    fs.open(FileName, ios::out | ios::binary);

    if(fs.is_open())
    {
        // Step 2. Write File Header Information

        if(pMapdata->m_PathNum == 0 || pMapdata->m_NodeNum == 0)
            Error = ERR_MAP_FILESAVEFAIL;

        // Step 3. Make Header
        String strTime;
        strTime = Now().FormatString("yyyymmddhhnnss");

        m_MapFileHeader.Revision = 0; // Revision은 0으로 고정함
        m_MapFileHeader.UpdateTime = StrToInt64("0x" + strTime);
        m_MapFileHeader.PathNum = pMapdata->m_PathNum;    // Path 개수
        m_MapFileHeader.NodeNum = pMapdata->m_NodeNum;    // Node 개수

        MAP_FILE_HEADER FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));
        
        if(Error == NO_ERR)
            FileHeader = MakeMapFileHeader(&m_MapFileHeader);

        if(FileHeader.PathNum > 0 && FileHeader.NodeNum > 0)
        {
            fs.write((char *)&FileHeader, sizeof(FileHeader));

            // step3. Path 정보 write
            int PathSize = pMapdata->m_PathNum * sizeof(PATH_INFO);
            fs.write((char *)pMapdata->m_PathArray, PathSize);

            // step4. Node 정보 write
            int NodeSize = pMapdata->m_NodeNum * sizeof(NODE_INFO);
            fs.write((char *)pMapdata->m_NodeArray, NodeSize);
        }
    }
    else
    {
        Error = ERR_MAP_FILEOPENFAIL;
    }

    fs.close();

    return Error;    
}
//---------------------------------------------------------------------------

/**
@brief   Map File Load된 상태를 리턴하는 함수
@author  zzang9un
@date    2012.11.16
@return  Map File이 Load된 경우 true, 그렇지 않은 경우 false를 리턴

bool MapDataManager::GetMapLoaded()
{
	return m_MapFileLoaded;
}
*/
//---------------------------------------------------------------------------

/**
@brief   Map File Version 리턴하는 함수
@author  puting
@date    2014.06.15
@return  Map File이 Version 리턴

unsigned int MapDataManager::GetMapVersion()
{
	return m_MapFileHeader.Revision;
}
*/
//---------------------------------------------------------------------------

/**
@brief   STD Map으로부터 7.0 Map Path 총 개수를 구하는 함수
@author  KJD
@date    2016.04.22
@param   TBD
@return  정상인 경우 Path 개수, 그렇지 않은 경우 0
*/
UINT FindPathTotalCount(NODE_INFO_STD_MAP *m_NodeArray_Map,UINT DataNum_Map)
{
	UINT PathNum = 0;
	UINT RightNodeNum = 0;

	for( int i=0 ; i< DataNum_Map ; i++ )
	{
	  if(m_NodeArray_Map[i].RightNodeID != 0 )
	  {
	  	RightNodeNum += 1;
	  }
	}
	PathNum = DataNum_Map + RightNodeNum;
	if(PathNum>(NODE_COUNT_MAX*2)) //path 최대 크기는 노드의 2배수로 할당
	{
		PathNum = 0;
	}

	return PathNum;
}
//---------------------------------------------------------------------------
/**
@brief   STD Map으로부터 7.0 Map Data를 Load하는 함수
@author  KJD
@date    2016.04.22
@param   TBD
@return  정상인 경우 0, 그렇지 않은 경우 에러 코드 리턴
*/
#if(OHT_NAME == OHT_NAME_P4_MASK)
int MapDataManager::LoadDataFromSTDMap(MAP *pMapdata,
						   NODE_INFO_STD_MAP * m_NodeArray_Map,
						   NODE_INFO_STD_COLLISION * m_NodeArray_Collision,
						   NODE_INFO_STD_FLOWCONTROL * m_NodeArray_Flow,
						   /*NODE_INFO_STD_CID * m_NodeArray_CID,*/
						   MapMakeProcess* pMapMakeProcess,
						   bool bUseoptionalNode,
						   UINT NodeNum_Map, UINT DataNum_Flow, /*UINT DataNum_CID,*/ bool bAdjust)
#else
int MapDataManager::LoadDataFromSTDMap(MAP *pMapdata,
                           NODE_INFO_STD_MAP * m_NodeArray_Map,
                           NODE_INFO_STD_COLLISION * m_NodeArray_Collision,
                           NODE_INFO_STD_FLOWCONTROL * m_NodeArray_Flow,
						   NODE_INFO_STD_CID * m_NodeArray_CID,
						   MapMakeProcess* pMapMakeProcess,
						   bool bUseoptionalNode,
						   UINT NodeNum_Map, UINT DataNum_Flow, UINT DataNum_CID, bool bAdjust)
#endif
{
	int Error = NO_ERR;
	int ret = 0;
//	DWORD	TimeOfTest;
	UINT PathNum = 0; // 경로의 총 개수를 가지고 있는 변수

	int PathSize = 0; // Path Data 총 size
	int NodeSize = 0; // Node Data 총 size
	ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Start");
	PathNum = FindPathTotalCount(m_NodeArray_Map,NodeNum_Map);

	if(pMapMakeProcess->GetCount() !=0)
		pMapMakeProcess->InitMapMakeProcessMemory();

	//STD Map 자료를 이용하여 Path 정보 생성
	if( (PathNum >= NodeNum_Map) && (NodeNum_Map > 0) )
	{
		// Step 1. 파일에서 Load한 내용에 따라 Init
		pMapdata->InitMap(PathNum, NodeNum_Map);
		ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step1");
		// Step 2. 노드 정보 복사
		// copy NODE_INFO
		for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			//pMapdata->m_cpNode[0]
			pMapdata->m_NodeArray[i].NID =  m_NodeArray_Map[i].NodeID;
			pMapdata->m_NodeArray[i].InOutInfo = m_NodeArray_Map[i].NodeType;
			pMapdata->m_NodeArray[i].TagType = NORMAL_NTYPE;   // TagType 1차 : Normal Node로 입력
//			pMapdata->m_NodeArray[i].CollisionInfo = 0;
			if(pMapdata->m_NodeArray[i].InOutInfo != NONE_TYPE &&
			   pMapdata->m_NodeArray[i].InOutInfo != SLOPE_NTYPE &&
			   pMapdata->m_NodeArray[i].InOutInfo != LIFTPRE_NTYPE_LEFT &&
			   pMapdata->m_NodeArray[i].InOutInfo != LIFTPRE_NTYPE_RIGHT &&
			   pMapdata->m_NodeArray[i].InOutInfo != LIFTIN_NTYPE_LEFT &&
			   pMapdata->m_NodeArray[i].InOutInfo != LIFTIN_NTYPE_RIGHT &&
			   pMapdata->m_NodeArray[i].InOutInfo != LIFTOUT_NTYPE &&
			   pMapdata->m_NodeArray[i].InOutInfo != MTL_SEL_ON_LEFT &&
			   pMapdata->m_NodeArray[i].InOutInfo != MTL_SEL_ON_RIGHT &&
			   pMapdata->m_NodeArray[i].InOutInfo != MTL_STOP_LEFT &&
			   pMapdata->m_NodeArray[i].InOutInfo != MTL_STOP_RIGTH &&
			   pMapdata->m_NodeArray[i].InOutInfo != MTL_AREA&&
			   pMapdata->m_NodeArray[i].InOutInfo != AUTODOOR_LEFT&&
			   pMapdata->m_NodeArray[i].InOutInfo != AUTODOOR_RIGHT&&
			   pMapdata->m_NodeArray[i].InOutInfo != FDConnectedType &&
			   pMapdata->m_NodeArray[i].InOutInfo != FDDisconnectedType&&
			   pMapdata->m_NodeArray[i].InOutInfo != EmergencyNodeMTL_NTYPE &&
			   pMapdata->m_NodeArray[i].InOutInfo != OPT_TAG &&
			   pMapdata->m_NodeArray[i].InOutInfo != OPT_DISTANCE&&
			   pMapdata->m_NodeArray[i].InOutInfo != OPT_COMBO)
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step1 Error NodeType : %d, %d",pMapdata->m_NodeArray[i].NID, pMapdata->m_NodeArray[i].InOutInfo);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}

             //AOHC-285
			if(bUseoptionalNode==false)
			{
				if((pMapdata->m_NodeArray[i].InOutInfo == OPT_TAG) ||
				   (pMapdata->m_NodeArray[i].InOutInfo == OPT_DISTANCE) ||
				   (pMapdata->m_NodeArray[i].InOutInfo == OPT_COMBO))
				   {
						ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap OptionalNode : %d, %d",pMapdata->m_NodeArray[i].NID, pMapdata->m_NodeArray[i].InOutInfo);
						Error = ERR_MAPDATA_LOAD_FAIL;
						break;
				   }
			}
			// AOHC-409 FDConnectedType,  FDDisconnectedType 의 경우 플로우컨트롤 없으면 에러 발생
			if( pMapdata->m_NodeArray[i].InOutInfo == FDConnectedType || pMapdata->m_NodeArray[i].InOutInfo == FDDisconnectedType )
			{
			   int tmp;
			   tmp = pMapdata->FireDoorCheck(pMapdata->m_NodeArray[i].NID, m_NodeArray_Flow, DataNum_Flow);
			   if(tmp!=NO_ERR)
			   {
					ADD_LOG_MAPMAKE_STD("FireDoorTypeMismatch : %d, %d",pMapdata->m_NodeArray[i].NID, pMapdata->m_NodeArray[i].InOutInfo);
					Error = ERR_MAPDATA_LOAD_FAIL;
					break;

			   }
			}

			Sleep(0);
		}
 ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2");
		// Step 3. 경로 정보 복사
		// Step 3.1 왼쪽 분기 또는 직진(오른쪽 분기가 없는 구간)경로 복사
		for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			pMapdata->m_PathArray[i].ID = i+PATHINDEX_OFFSET;       //10
			pMapdata->m_PathArray[i].StartNID = m_NodeArray_Map[i].NodeID;
			pMapdata->m_PathArray[i].EndNID = m_NodeArray_Map[i].LeftNodeID;
			pMapdata->m_PathArray[i].Speed = m_NodeArray_Map[i].LeftNodeSpeed;
			pMapdata->m_PathArray[i].Steering = m_NodeArray_Map[i].LeftSteering;
			pMapdata->m_PathArray[i].Distance = m_NodeArray_Map[i].LeftNodeDistance;
#if(USE_CHANGENODE == 1)
			pMapdata->m_PathArray[i].UBGRegion= 0;  //하위스텝에서 진행 : COLLISION.DAT 사용
			pMapdata->m_PathArray[i].ObstacleRegion = 0;  //하위스텝에서 진행 : COLLISION.DAT 사용
#else
			pMapdata->m_PathArray[i].PathDirection = 0;  //하위스텝에서 진행 : COLLISION.DAT 사용
#endif
			pMapdata->m_PathArray[i].PathBranch = m_NodeArray_Map[i].LeftBranch;
			if(pMapdata->m_PathArray[i].Steering > 2)//N분기 처리 전 수행
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Steering : %d, %d", pMapdata->m_PathArray[i].StartNID,pMapdata->m_PathArray[i].Steering);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}
			if( pMapdata->m_PathArray[i].PathBranch == 2 )// N분기
			{
            	pMapdata->m_PathArray[i].Steering += 2;//STEERING_N_BRANCH_L2R or STEERING_N_BRANCH_R2L
            	if(pMapdata->m_PathArray[i].Steering != STEERING_N_BRANCH_L2R )
            	{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Steering : %d", pMapdata->m_PathArray[i].StartNID, pMapdata->m_PathArray[i].Steering);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
			}
			pMapdata->m_PathArray[i].ObstacleIgnore = 0;  //하위스텝에서 진행 : COLLISION.DAT 사용
			//COLLISION.DAT에 있는 노드 순서와 일치 여부를 확실 할 수 없으므로, 노드 검색하여 데이터 입력이 되야함.

			if( m_NodeArray_Map[i].RightNodeID != 0 ) // 오른쪽 분기 노드 존재 유무 확인
			{
				pMapdata->m_PathArray[i].RailBranch = PATH_DIVERGENCE_LEFT; // 오른쪽 노드가 있으면 현재 노드는 왼쪽분기를 가짐.
			}
			else
			{
				pMapdata->m_PathArray[i].RailBranch = PATH_DIVERGENCE_NONE; // 오른쪽 노드가 없으면 직진.
			}
			if(pMapdata->m_PathArray[i].Speed < 500 || pMapdata->m_PathArray[i].Speed > 8000)			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Speed : %d, %d", pMapdata->m_PathArray[i].StartNID, pMapdata->m_PathArray[i].Speed);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}			
			if(pMapdata->m_PathArray[i].PathBranch > 2)
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error PathBranch : %d, %d", pMapdata->m_PathArray[i].StartNID, pMapdata->m_PathArray[i].PathBranch);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}

			pMapdata->m_PathArray[i].ChargeEnable = m_NodeArray_Map[i].ChargeEnable;

			if(m_NodeArray_Map[i].ChargeEnable != 0)
			{ADD_LOG_MAPMAKE_STD("%d Left Node Charge:%d, Path Charge:%d", pMapdata->m_PathArray[i].StartNID, m_NodeArray_Map[i].ChargeEnable, pMapdata->m_PathArray[i].ChargeEnable);}

			if(bAdjust == true)
			{
				if(m_NodeArray_Map[i].IsDefaultMap.LOW_DefaultMapInfo == 1)
				{
					MAMMAKEPROCESS_LINK_INFO *updateSTD;
					updateSTD= new MAMMAKEPROCESS_LINK_INFO;
					updateSTD->LinkNID = m_NodeArray_Map[i].NodeID;
					updateSTD->IsCheck = 0x00;
					pMapMakeProcess->Update(updateSTD);
				}
            }

#if(USE_CHANGENODE == 1)
			// for change node (jhoun.lim)
			pMapdata->m_PathArray[i].OptDistance = 0;
			pMapdata->m_PathArray[i].OptDistanceAfter = 0;
			pMapdata->m_PathArray[i].OptObstacleRegion = 0;
			pMapdata->m_PathArray[i].OptUBGRegion = 0;
#endif
			Sleep(0);

		}
		// Step 3.2 오른쪽 분기가 있는 경우 경로 복사
		int tmp = 0;//오른쪽 노드가 있는 경우에만 증가시키기 위한 변수
		for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			if( m_NodeArray_Map[i].RightNodeID != 0 ) // 오른쪽 분기 노드 존재 유무 확인
			{
				pMapdata->m_PathArray[NodeNum_Map+tmp].ID = NodeNum_Map+tmp+1;
				pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID = m_NodeArray_Map[i].NodeID;
				pMapdata->m_PathArray[NodeNum_Map+tmp].EndNID = m_NodeArray_Map[i].RightNodeID;
				pMapdata->m_PathArray[NodeNum_Map+tmp].Speed = m_NodeArray_Map[i].RightNodeSpeed;
				pMapdata->m_PathArray[NodeNum_Map+tmp].Steering = m_NodeArray_Map[i].RightSteering;
				pMapdata->m_PathArray[NodeNum_Map+tmp].Distance = m_NodeArray_Map[i].RightNodeDistance;
#if(USE_CHANGENODE == 1)
				pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion= 0;	//하위스텝에서 진행 : COLLISION.DAT 사용
				pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleRegion = 0;  //하위스텝에서 진행 : COLLISION.DAT 사용
#else
				pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection = 0;  //하위스텝에서 진행 : COLLISION.DAT 사용
#endif
				pMapdata->m_PathArray[NodeNum_Map+tmp].PathBranch = m_NodeArray_Map[i].RightBranch;
				if(pMapdata->m_PathArray[NodeNum_Map+tmp].Steering > 2)//N분기 처리 전 수행
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Steering : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].Steering);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
				if( pMapdata->m_PathArray[NodeNum_Map+tmp].PathBranch == 2 )// N분기
				{
					pMapdata->m_PathArray[NodeNum_Map+tmp].Steering += 2;//STEERING_N_BRANCH_L2R or STEERING_N_BRANCH_R2L
					if(pMapdata->m_PathArray[NodeNum_Map+tmp].Steering != STEERING_N_BRANCH_R2L )
	            	{
						ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Steering : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].Steering);
						Error = ERR_MAPDATA_LOAD_FAIL;
					}
				}
				pMapdata->m_PathArray[NodeNum_Map+tmp].RailBranch = PATH_DIVERGENCE_RIGHT;  // 오른쪽 노드이므로 오른쪽분기를 가짐.
				pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleIgnore = 0;  //하위스텝에서 진행 : COLLISION.DAT 사용
				//COLLISION.DAT에 있는 노드 순서와 일치 여부를 확신 할 수 없으므로, 노드 검색하여 데이터 입력이 되야함.
				if(pMapdata->m_PathArray[NodeNum_Map+tmp].Speed<500 || pMapdata->m_PathArray[NodeNum_Map+tmp].Speed > 8000)
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Speed : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].Speed);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
				if(pMapdata->m_PathArray[NodeNum_Map+tmp].PathBranch > 2)
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error PathBranch : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].PathBranch);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}

				pMapdata->m_PathArray[i].ChargeEnable = m_NodeArray_Map[i].ChargeEnable;

				if(m_NodeArray_Map[i].ChargeEnable != 0)
				{ADD_LOG_MAPMAKE_STD("%d Right Node Charge : %d", pMapdata->m_PathArray[i].StartNID, m_NodeArray_Map[i].ChargeEnable);}

				if(bAdjust == true)
				{
					if(m_NodeArray_Map[i].IsDefaultMap.HIGH_DefaultMapInfo == 1)
					{
						MAMMAKEPROCESS_LINK_INFO *updateSTD;
						updateSTD= new MAMMAKEPROCESS_LINK_INFO;
						updateSTD->LinkNID = 1000000+ m_NodeArray_Map[i].NodeID;
						updateSTD->IsCheck = 0x00;
						pMapMakeProcess->Update(updateSTD);
					}
				}
#if(USE_CHANGENODE == 1)
				// for change node (jhoun.lim)
				pMapdata->m_PathArray[NodeNum_Map+tmp].OptDistance = 0;
				pMapdata->m_PathArray[NodeNum_Map+tmp].OptDistanceAfter = 0;
				pMapdata->m_PathArray[NodeNum_Map+tmp].OptObstacleRegion = 0;
				pMapdata->m_PathArray[NodeNum_Map+tmp].OptUBGRegion = 0;
#endif
				tmp += 1;
			}
			Sleep(0);
		}
ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step3");
		// Step 4. Map Data를 map 컨테이너로 make
		pMapdata->MakeFlowControlMap(m_NodeArray_Flow, DataNum_Flow);
ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step4");
		ret = pMapdata->MakeMap(pMapdata,m_NodeArray_Map,NodeNum_Map);
		if(ret != NO_ERR)
		{
			ADD_LOG_MAPMAKE_STD("m_FlowcontrolCheckResult : fail..");
			Error = ERR_MAPDATA_LOAD_FAIL;
		}
#if(OHT_NAME != OHT_NAME_P4_MASK)
		pMapdata->MakeCIDMap(m_NodeArray_CID, DataNum_CID);
#endif
		

		// Step 5, 6은 FindNodeIndex, FindRightNodeIndex를 사용하고자 MakeMap 다음에 구현함.

		// Step 5. TagType make(분기 또는 합류노드의 Tag정보 N:1, S:2, R:3, C:4)
		// Step 5.1 TagType 2차 : Stop Node로 입력
		for(int i=0 ; i < DataNum_Flow ; i++ )
		{
			int tmp;
			tmp = pMapdata->FindNodeIndex(m_NodeArray_Flow[i].StopNodeID);
			if(tmp != INDEX_NONE)
			{
			if( pMapdata->m_NodeArray[tmp].TagType == NORMAL_NTYPE )
			{
				pMapdata->m_NodeArray[tmp].TagType = STOP_NTYPE;   // TagType 2차 : Stop Node로 입력
			}
			else
			{
				//NOP
				}
			}
			Sleep(0);
		}

		// Step 5.2 TagType 3차 : Reset Node, Common Node 입력
		for(int i=0 ; i < DataNum_Flow ; i++ )
		{
			//if( m_NodeArray_Flow->ResetNodeMain != 0 ) // 노드가 존재시. 의미상 있어야 하나 항상 참이므로 조건비교 안함.
			int tmp = pMapdata->FindNodeIndex(m_NodeArray_Flow[i].ResetNodeMain);
			if(tmp != INDEX_NONE)
			{
			if( pMapdata->m_NodeArray[tmp].TagType == NORMAL_NTYPE )
			{
				pMapdata->m_NodeArray[tmp].TagType = RESET_NTYPE;   // TagType 3차 : Reset Node로 입력
			}
			else if( pMapdata->m_NodeArray[tmp].TagType == STOP_NTYPE )
			{
				pMapdata->m_NodeArray[tmp].TagType = COMMON_NTYPE;   // TagType 3차 : Common Node로 입력
			}
			}
			if( m_NodeArray_Flow[i].ResetNodeSub != 0 ) // 노드가 존재시.
			{
				int tmp = pMapdata->FindNodeIndex(m_NodeArray_Flow[i].ResetNodeSub);
				if(tmp != INDEX_NONE)
				{
				if( pMapdata->m_NodeArray[tmp].TagType == NORMAL_NTYPE )
				{
					pMapdata->m_NodeArray[tmp].TagType = RESET_NTYPE;   // TagType 3차 : Reset Node로 입력
				}
				else if( pMapdata->m_NodeArray[tmp].TagType == STOP_NTYPE )
				{
					pMapdata->m_NodeArray[tmp].TagType = COMMON_NTYPE;   // TagType 3차 : Common Node로 입력
					}
				}
			}
			Sleep(0);
		}
 ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5");
		// Step 6. m_PathArray에 COLLISION.DAT 정보 mapping
		for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			//if( m_NodeArray_Collision[i]->LeftNodeID != 0 ) // 노드가 존재시. 의미상 있어야 하나 항상 참이므로 조건비교 안함.
			//m_NodeArray_Collision[i]->LeftNodeID의 경로는
			//NodeIndex와 PathIndex는 동일하므로 NodeIndex에서 배열순서를 알아옴.
#if(USE_CHANGENODE == 1)
			int tmp = pMapdata->FindNodeIndex(m_NodeArray_Collision[i].NodeID);
			if( pMapdata->m_PathArray[tmp].UBGRegion != 0 || pMapdata->m_PathArray[tmp].ObstacleRegion != 0)//UBG 값이 0이 아니면 노드 중복
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error Node ID Not Unique : %d", pMapdata->m_PathArray[tmp].StartNID);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}

			pMapdata->m_PathArray[tmp].UBGRegion = m_NodeArray_Collision[i].LeftUBGRegion;
			pMapdata->m_PathArray[tmp].ObstacleRegion = m_NodeArray_Collision[i].LeftObstacleRegion;
			pMapdata->m_PathArray[tmp].ObstacleIgnore = m_NodeArray_Collision[i].LeftObstacleStatus;

            //puting optionalNode 기능 보완
			pMapdata->m_PathArray[tmp].OptUBGRegion = m_NodeArray_Collision[i].LeftUBGRegion;
			pMapdata->m_PathArray[tmp].OptObstacleRegion = m_NodeArray_Collision[i].LeftObstacleRegion;


			if(pMapdata->m_PathArray[tmp].UBGRegion == 0 && pMapdata->m_PathArray[tmp].ObstacleRegion == 0)
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection : %d, %d", pMapdata->m_PathArray[tmp].StartNID,pMapdata->m_PathArray[tmp].UBGRegion );
				Error = ERR_MAPDATA_LOAD_FAIL;
			}
			//이종 패턴 추가로 하위 4비트만으로 비교 처리
			if((pMapdata->m_PathArray[tmp].UBGRegion != PATH_DIRECTION_NORMAL && pMapdata->m_PathArray[tmp].UBGRegion != PATH_DIRECTION_HILL) && pMapdata->m_PathArray[tmp].Speed > 1300 )// 패턴이 N, Slope가 아닌데 속도가 1.3m/s 이상인 경우
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection Speed : %d, 0x%x, %d", pMapdata->m_PathArray[tmp].StartNID, pMapdata->m_PathArray[tmp].UBGRegion, pMapdata->m_PathArray[tmp].Speed );
				Error = ERR_MAPDATA_LOAD_FAIL;
			}

			// for change node (jhoun.lim)
			pMapdata->m_PathArray[tmp].calcOptDistance(m_NodeArray_Map[tmp].LeftNodeOptRatio);
			if(pMapdata->m_PathArray[tmp].OptDistance > 0)
			{
				pMapdata->m_PathArray[tmp].OptUBGRegion = m_NodeArray_Collision[i].LeftOptUBGRegion;
				pMapdata->m_PathArray[tmp].OptObstacleRegion = m_NodeArray_Collision[i].LeftOptObstacleRegion;
				pMapdata->m_PathArray[tmp].calcOptDistanceAfter(m_NodeArray_Map[tmp].LeftNodeOptRatio);
                if(pMapdata->m_PathArray[tmp].OptDistanceAfter < 0)
                {
                    Error = ERR_MAPDATA_LOAD_FAIL;
                }

			}else if(pMapdata->m_PathArray[tmp].OptDistance < 0)
            {
                Error = ERR_MAPDATA_LOAD_FAIL;
            }

			if( m_NodeArray_Collision[i].RightNodeID != 0 ) // 노드가 존재시.
			{
				int tmp2 = tmp;
				tmp = pMapdata->FindRightNodeIndex(m_NodeArray_Collision[i].RightNodeID);

				if( pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion != 0 || pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleRegion != 0)//UBG 값이 0이 아니면 노드 중복
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error Node ID Not Unique : %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}

				if( pMapdata->m_PathArray[NodeNum_Map+tmp].EndNID == m_NodeArray_Collision[i].RightNodeID )
				{
					pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion = m_NodeArray_Collision[i].RightUBGRegion;
					pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleRegion = m_NodeArray_Collision[i].RightObstacleRegion;
					pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleIgnore = m_NodeArray_Collision[i].RightObstacleStatus;

					//puting optionalNode 기능 보완
					pMapdata->m_PathArray[NodeNum_Map+tmp].OptUBGRegion = m_NodeArray_Collision[i].RightUBGRegion;
					pMapdata->m_PathArray[NodeNum_Map+tmp].OptObstacleRegion = m_NodeArray_Collision[i].RightObstacleRegion;
				}

				if(pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion == 0 && pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleRegion == 0)
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion );
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
				//이종 패턴 추가로 하위 4비트만으로 비교 처리
				if((pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion != PATH_DIRECTION_NORMAL && pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion != PATH_DIRECTION_HILL) && pMapdata->m_PathArray[NodeNum_Map+tmp].Speed > 1300 )// 패턴이 N, Slope가 아닌데 속도가 1.3m/s 이상인 경우
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection Speed : %d, %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion, pMapdata->m_PathArray[NodeNum_Map+tmp].Speed);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}

				// for change node (jhoun.lim)
				pMapdata->m_PathArray[NodeNum_Map+tmp].calcOptDistance(m_NodeArray_Map[tmp2].RightNodeOptRatio);
				if(pMapdata->m_PathArray[NodeNum_Map+tmp].OptDistance > 0)
				{

					pMapdata->m_PathArray[NodeNum_Map+tmp].OptUBGRegion = m_NodeArray_Collision[i].RightOptUBGRegion;
					pMapdata->m_PathArray[NodeNum_Map+tmp].OptObstacleRegion = m_NodeArray_Collision[i].RightOptObstacleRegion;
					pMapdata->m_PathArray[NodeNum_Map+tmp].calcOptDistanceAfter(m_NodeArray_Map[tmp2].RightNodeOptRatio);
					if(pMapdata->m_PathArray[NodeNum_Map+tmp].OptDistanceAfter< 0)
                    {
                        Error = ERR_MAPDATA_LOAD_FAIL;
                    }

				}
				else if(pMapdata->m_PathArray[NodeNum_Map+tmp].OptDistance < 0)
                {
                    Error = ERR_MAPDATA_LOAD_FAIL;
                }
			}
#else
			int tmp = pMapdata->FindNodeIndex(m_NodeArray_Collision[i].NodeID);
			if( pMapdata->m_PathArray[tmp].PathDirection != 0)//UBG 값이 0이 아니면 노드 중복
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error Node ID Not Unique : %d", pMapdata->m_PathArray[tmp].StartNID);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}
			

			pMapdata->m_PathArray[tmp].PathDirection = m_NodeArray_Collision[i].LeftUBGRegion | (m_NodeArray_Collision[i].LeftObstacleRegion << 4);

			pMapdata->m_PathArray[tmp].ObstacleIgnore = m_NodeArray_Collision[i].LeftObstacleStatus;

			if(pMapdata->m_PathArray[tmp].PathDirection == 0)
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection : %d, %d", pMapdata->m_PathArray[tmp].StartNID,pMapdata->m_PathArray[tmp].PathDirection );
				Error = ERR_MAPDATA_LOAD_FAIL;
			}
			//이종 패턴 사용으로 제거
			//if(pMapdata->m_PathArray[tmp].PathBranch == 2 && pMapdata->m_PathArray[tmp].PathDirection != PATH_DIRECTION_SHORT) // N분기인데 패턴이 S가 아닌 경우
			//{
			//	ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error N Branch Pattern : %d, %d", pMapdata->m_PathArray[tmp].StartNID, pMapdata->m_PathArray[tmp].PathDirection);
			//	Error = ERR_MAPDATA_LOAD_FAIL;
			//}
			//이종 패턴 추가로 하위 4비트만으로 비교 처리
			if(((pMapdata->m_PathArray[tmp].PathDirection & 0x0F) != PATH_DIRECTION_NORMAL && (pMapdata->m_PathArray[tmp].PathDirection & 0x0F) != PATH_DIRECTION_HILL) && pMapdata->m_PathArray[tmp].Speed > 1300 )// 패턴이 N, Slope가 아닌데 속도가 1.3m/s 이상인 경우
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection Speed : %d, 0x%x, %d", pMapdata->m_PathArray[tmp].StartNID, pMapdata->m_PathArray[tmp].PathDirection, pMapdata->m_PathArray[tmp].Speed );
				Error = ERR_MAPDATA_LOAD_FAIL;
			}

			if( m_NodeArray_Collision[i].RightNodeID != 0 ) // 노드가 존재시.
			{
				tmp = pMapdata->FindRightNodeIndex(m_NodeArray_Collision[i].RightNodeID);
				
				if( pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection != 0)//UBG 값이 0이 아니면 노드 중복
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error Node ID Not Unique : %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
				
				if( pMapdata->m_PathArray[NodeNum_Map+tmp].EndNID == m_NodeArray_Collision[i].RightNodeID )
				{

					pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection = m_NodeArray_Collision[i].RightUBGRegion | (m_NodeArray_Collision[i].RightObstacleRegion << 4);
					pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleIgnore = m_NodeArray_Collision[i].RightObstacleStatus;
				}
				
				if(pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection == 0)
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection );
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
				//이종 패턴 사용으로 제거
				//if(pMapdata->m_PathArray[NodeNum_Map+tmp].PathBranch == 2 && pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection != PATH_DIRECTION_SHORT) // N분기인데 패턴이 S가 아닌 경우
				//{
				//	ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error N Branch Pattern : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection);
				//	Error = ERR_MAPDATA_LOAD_FAIL;
				//}
				//이종 패턴 추가로 하위 4비트만으로 비교 처리
				if(((pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection & 0x0F) != PATH_DIRECTION_NORMAL && (pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection & 0x0F) != PATH_DIRECTION_HILL) && pMapdata->m_PathArray[NodeNum_Map+tmp].Speed > 1300 ) // 패턴이 N, Slope가 아닌데 속도가 1.3m/s 이상인 경우
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection Speed : %d, %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection, pMapdata->m_PathArray[NodeNum_Map+tmp].Speed);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}


			}
#endif
			
			Sleep(0);
		}
		

		m_MapFileLoaded = true; // Map File Loaded flag 변경
	}
	else
	{
		m_MapFileLoaded = false; // Map File Loaded flag 변경
	}
ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step6 END");

	return Error;
}


//---------------------------------------------------------------------------

/**
@brief   StartNode1_1~5 ID에 매칭되는 CID 배열의 index를 리턴하는 함수.
@author  KJD
@date    2016.08.18
@param   NodeID : 찾고자 하는 Node ID
@return  Node ID에 매칭되는 배열 index
@todo    에러 코드 재정의
*/
int MAP::FindNodeIndexFromCIDStart_1(int NodeID)
{
    map<int, int>::iterator FindIter;

	FindIter = m_CIDStartMap_1.find(NodeID);

    if(FindIter != m_CIDStartMap_1.end())
    {
        // Node를 찾은 경우
        return FindIter->second;
    }
    else
    {
        return INDEX_NONE;
    }
}

//---------------------------------------------------------------------------

/**
@brief   StartNode2_1~5 ID에 매칭되는 CID 배열의 index를 리턴하는 함수.
@author  KJD
@date    2016.08.18
@param   NodeID : 찾고자 하는 Node ID
@return  Node ID에 매칭되는 배열 index
@todo    에러 코드 재정의
*/
int MAP::FindNodeIndexFromCIDStart_2(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_CIDStartMap_2.find(NodeID);

    if(FindIter != m_CIDStartMap_2.end())
    {
        // Node를 찾은 경우
        return FindIter->second;
	}
	else
	{
		return INDEX_NONE;
	}
}

/**
@brief   FlowControl에 매칭되는 Flowcontrol 배열의 index를 리턴하는 함수.
@author  puting
@date    2016.08.18
@param   NodeID : 찾고자 하는 Node ID
@return  Node ID에 매칭되는 배열 index
@todo    에러 코드 재정의
*/
int MAP::FindNodeIndexFromFlowControl(int NodeID, int NextNodeID, STD_FlowControl *pFlowMap)
{
	map<int, int>::iterator FindIter;

	int nInedex = INDEX_NONE;
	int nTempStopNode = NodeID;

	for(int j = 0; j <4; j++)
	{
		nTempStopNode = nTempStopNode + 1000000;

		FindIter = m_FlowControl.find(nTempStopNode);

		if(FindIter != m_FlowControl.end())
		{
			// Node를 찾은 경우   혼용해서사용하는 경우때문에 적용
			if((pFlowMap->m_NodeArray[FindIter->second].ResetNodeMain ==NextNodeID) ||
				(pFlowMap->m_NodeArray[FindIter->second].ResetNodeSub ==NextNodeID))
				{
					nInedex = FindIter->second;
					break;
				}
		}
	}

	return nInedex;
}

UINT MAP::calReOptRatio(double ChangeOptRatio)         // STB 확장형
{
   double ChangeOptRatioCal;
   ChangeOptRatioCal=((int)(ChangeOptRatio*pow(10.0,4)))/pow(10.0,4);   // 소수점 넷쨰까지 자르기
   UINT ChangeOptCal, OptCalResult;
   int RatioDigit[10];

   ChangeOptCal	= UINT(ChangeOptRatioCal*10000);
   ADD_LOG_MAPMAKE_STD("ChangeOptRatio : %f, ChangeOptRatioCal :%f, ChangeOptCal:%x %d",ChangeOptRatio,ChangeOptRatioCal,ChangeOptCal, ChangeOptCal );
	for(int i = 0; i<4 ;i++)         // 4자리씩 보내기 때문에 (우선 Optional 1종류만 사용 가능)
	{
	   RatioDigit[i] = (ChangeOptCal % 10) & 0x000F;
	   ChangeOptCal /= 10;
	}
   ADD_LOG_MAPMAKE_STD("calReOptRatio : %x", (( RatioDigit[0]  | (RatioDigit[1]<<4)  |(RatioDigit[2]<<8) |(RatioDigit[3]<<12) ) & 0x0000FFFF ) << 16);

   return  (( RatioDigit[0]  | (RatioDigit[1]<<4)  |(RatioDigit[2]<<8) |(RatioDigit[3]<<12) ) & 0x0000FFFF ) << 16 ;


}

int MAP::FireDoorCheck(UINT NodeID, NODE_INFO_STD_FLOWCONTROL * m_FlowArray, UINT DataNum)
{
	int Error = NO_ERR;
	int nTempStopNode= 0;
	int nCnt = 0;

	for(int i=0;i<DataNum;i++)
	{
		nTempStopNode =  m_FlowArray[i].StopNodeID;
		if(NodeID == nTempStopNode)
		{
			ADD_LOG_MAPMAKE_STD("FireDoorCheck : %d %d", NodeID, nTempStopNode);
			return NO_ERR;
		}
		else
		{
			nCnt++; // Flowcontol의 StopNodeID와 비교 원하는 NodeID 다른 경우 Cnt ++해서 DataNum-1이랑 같으면 Flowconrol없다고 판단

			if(nCnt == (DataNum - 1))
			{
				ADD_LOG_MAPMAKE_STD(" nCnt: %d, DataNum : %d", nCnt, DataNum);
				  return -1;
			}

		}
	}
}
#pragma package(smart_init)
