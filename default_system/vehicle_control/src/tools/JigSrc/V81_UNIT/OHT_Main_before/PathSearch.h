//---------------------------------------------------------------------------
#ifndef PathSearchH
#define PathSearchH

//---------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "MAP.h"
#include "List.h"
#include "ExecuteInfo.h"
#include "LogHandler.h"	// Log 작성 목적
#include "Def_PathSearchIniParam.h" //파라미터
#include "LIniFile.h"
#include "DrivingControl.h"
#include "OHTMainStatus.h"

#define	MARGIN_OF_DIST_TO_STATION	100	// 단위: mm
#define	MARGIN_OF_DIST_TO_RESETNODE	250.0	// 100  단위: mm
#define	MARGIN_OF_DIST_TO_STOPNODE	350// 400이하 단위: mm
#define	MARGIN_OF_DIST_TO_STOPNODE_ROLLBACK	250// 400이하 단위: mm
#define	VHLACCEL				    2000//2000	// 단위: mm/s^2
#define	SPEED_LIMIT_OF_MAP_MAKE		0.2	// 단위: mm/msec
#define MAX_PATHINDEX 				9999

#define MAX_LIST_COUNT_FOR_PRESTEER				9999
#define FIRST_INDEX				1

// soming UBG 단계 Check Default Node 정의
#define UBG_CHECK_NODE_1 999999
#define UBG_CHECK_NODE_2 999999
#define UBG_CHECK_NODE_3 999999
#define UBG_CHECK_NODE_4 999999



#define MAXSPEED(a,b) (((a) >(b))?(a) :(b))     //puting

/***********************************************************************************/
// 주행 중 map 속도 최적화 관련
/***********************************************************************************/
#define TIME_OF_CONST_VEL	0.5 // 0.5s
#define TIME_OF_STEERING_CHANGE 0.7	// 0.7s
#define VELOCITY_INCREASE_MARGIN  1.1 // 10%
#define DEFAULT_VEL 3.3 // m/s
#define DEFAULT_ACCEL 2.0 // m/s^2
#define DEFAULT_DECEL 3.0 // m/s^2
#define LAST_INDEX_OF_PATH 0
#define LAST_INDEX_OF_NORMAL_LINK 1
#define MIN_CNT_OF_VEL_CHANGE 2
#define VELOCITY_MARGIN 0.7 //70%

#define HIGH_SPEED_LIMIT_VEL	3.3 // m/s
#define HIGH_SPEED_ACCEL	1.0 // m/s^2, #define HIGH_SPEED_LIMIT_VEL 을 넘으면 HIGH_SPEED_ACCEL 적용



#define CAL_SPEED(speed, distance, const_time, accel, decel)	abs((sqrt(((const_time)*(const_time))+	\
	((1.0/(decel)+1.0/(accel))*((speed)*(speed)/(accel)+2.0*(distance))))	\
	-(const_time)) / (1.0/(decel)+1.0/(accel)))

#define GET_ACCEL_DIST(inspeed, outspeed, accel)	abs((((outspeed)*(outspeed))-((inspeed)*(inspeed)))/(2.0*(accel)))
#define GET_DECEL_DIST(inspeed, outspeed, decel)	abs((((inspeed)*(inspeed))-((outspeed)*(outspeed)))/(2.0*(decel)))
#define GET_MOVE_TIME(inspeed, distance, decel)	    abs(((distance)/(inspeed))+((inspeed)/(2*(decel))))

/***********************************************************************************/
// Function Name : PathSearch
// Release Date: 2012.11.12, 임태웅
/***********************************************************************************/

//typedef	struct CHANGE_PATH_NODE_INFO_
//{
//	CYCLEDRIVING_CHANGE_INFO	  			ChangeInfo[MAX_LIST_COUNT_FOR_CHANGE];
//	int	ChangeNodeCount;
//}CHANGE_PATH_NODE_INFO;


/**
@brief   V8.0 OHT Manual Path Search를 위한 구조체, 이동 Path 정보가 담겨 있음
@author  Labeler
@date    2016.12.22
*/
typedef struct _EXCEPT_PATH_LIST_V80
{
	unsigned int nCnt;
	int ExceptNode[300];
}EXCEPT_PATH_LIST_V80;


class PathSearch
{
private:

	bool getNodeType(MAP *pMap, UINT uNID, NODE_TYPE* pNodeType);
	UINT GetIncreaseSeqenceNumber();
	UINT GetSeqenceNumber();
//	double GetSpeedDownDist(double dAccel, double dSpeed);

	LList		*m_PathList;    ///< List
	CRITICAL_SECTION	m_CS;
	LogUnit *pLogPathSearch;      ///< PathSearch 로그 생성을 위한 변수
	FDCUnit *pFDCUnit;

	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;

	UINT m_SquenceIndex;
	DrivingControl	*m_pDrivingControl;

	double calcSpeed(double velIn, double distance, double constTime, double accel, double decel);      // 등속 시간 보장  (constTime)
	double calcSpeed_2(double velIn, double distance, double spdDownTime, double accel, double decel);  // 감속 타이밍 보장. (spdDownTime)

	double GetNewVelocity(double dVelIn, double dVelOut, double dDefaultAccel, double dDefaultDecel, double dDistanceSum, bool bSteeringChange);
	int ChangePathVelocity(void);
//	CHANGE_PATH_NODE_INFO m_CurrentChangePathInfo;  ///< 현재 싸이클에서 작업변경이 가능한 위치를 넣어주는 곳. 새로 Path 탐색할떄 초기화 해줄 것.
//													///<
public:
	PATHSEARCH_PARAM_SET		*m_pParamSet;		///< 파라미터 집합: 현재 사용되는 값
	PATHSEARCH_PARAM_SET		*m_pTmpParamSet;	///< 파라미터 집합: 업데이트 할 때 사용되는 값
	PATHSEARCH_UBG_CHECK_PARAM_SET  *m_pUBGParamSet; ///<soming UBG Check 파라미터 집합 :현재 사용되는 값
	PATHSEARCH_NODE_LANCUT_PARAM_SET *m_pNodeLanCutSet; ///<Manual 이동시 이동불가 처리할 Node 정보


public:
	PathSearch();
	~PathSearch();

    void InitPathList();
    void AddPath(void *pData);
    void* ReferFirstPath();
    void* ReferPath(UINT index);
    void* ReferBackPath();
    void* PopFirstPath();
    void* PopBackPath();
    int GetPathCount();

	//파라미터 관련 자료
	bool    loadParameter(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet);    ///< 파라미터 불러오는 함수
	bool    saveParameter(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet);    ///< 파라미터 저장하는 함수
	bool    updateParameter();
	bool    loadUBGCheckParam(char* szFilePath, PATHSEARCH_UBG_CHECK_PARAM_SET* pUBGParamSet);  ///< 파라미터 불러오는 함수
	bool    loadNodeLanCutCheckParam(char* szFilePath, PATHSEARCH_NODE_LANCUT_PARAM_SET* pNodeLanCutSet);  ///< 파라미터 불러오는 함수

	bool    loadParameterRecovery(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet);    ///< 파라미터 불러오는 함수
    int  	m_ParamChehckError;
									   ///< 파라미터 업데이트 하는 함수

    //int GetSearchDivergence(MAP *pMap, int StartNode);
    double GetSpeedDownDist(double dAccel, double dSpeed);
    double GetMarginOfSpeedDist(double dSpeed, double Margin);
	int CheckSearchDifferntPoint(MAP *pMap, PARSING_GO_INFO* CmdGoData, UINT &ChangeSqNum, int &ChangeNodeID, double &ChangeNodeDistance, int &nExtraPath, BYTE &chNackToAckType, UINT nPreTargetStation);
    bool CheckSearchChangePoint(int ChangeNodeID, UINT &ChangeSqNum, double &ChangeNodeDistance);

    int GetPassPermitPriod();
    int GetPassPermitRollbackTime();
	int GetPassPermitRollbackFinshTime();
	double GetPassPermitRollbackMargin();

    int	GetNextNode(MAP *pMap, UINT nFromNode, UINT nPathDiv);  //puting
	bool GetIsNBrench(MAP *pMap, UINT nFromNode, UINT nNextNode);  //puting
	bool GetIsNBrenchMapMake(MAP *pMap, UINT nNextNode);  //puting
    int	GetPreSteering(MAP *pMap, UINT StopNode, UINT PreSteeringNode, PATH_STEERING_INFO *pPreSteeringInfo);
    int CheckPathOfStation(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo);	///< Station 이 있는 Path를 사전 검사
    int SearchDrivingPath(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List);	///< PathSearch 결과 Return
	int StandardSearchPassPermit(MAP *pMap,STD_FlowControl *pFlowMap, PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List, UINT &uCheckExtraNum, bool bIsFoup,double PathExtra =0.0, bool bIgnorOverrun = false);	///< PathSearch 결과 Return
    int StandardSearchOnlyOnePassPermit(MAP *pMap, STD_FlowControl *pFlowMap, PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List, bool bcheckTarget, PKT_CYCLECMDDATA *FinalCycleData, UINT &uCheckExtraNum,double &dTempDis, UINT FinalSeqNum, PASSPERMIT_COMMON TempPassPermitData,bool bIsFoup, bool bIgnorOverrun = false);	///<//AOHC-195 오버런 개선 작업(유형2) ( PathSearch 결과 Return)
    int CheckPassPermit(CYCLENODEINFO CycleNodeInfo, PASSPERMIT_NODE_TYPE StartType, PASSPERMIT_NODE_TYPE EndType, EXECUTE_CYCLEDRIVING_INFO *pPassPermitInfo,bool bIsTargetStationoPassPermitOpen=false);	///< PathSearch 결과 Return
    int CheckOnlyOnePassPermit(CYCLENODEINFO CycleNodeInfo, PASSPERMIT_NODE_TYPE StartType, PASSPERMIT_NODE_TYPE EndType, EXECUTE_CYCLEDRIVING_INFO *pPassPermitInfo,bool IsStartStation,bool IsEndStation,bool bStartStationPassPermit,bool bIsTargetStationoPassPermitOpen);	///< PathSearch 결과 Return

    int StandardSearchPassBack(MAP *pMap,PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List);	///< PathSearch 결과 Return

	PATH_DIVERGENCE_INFO GetStationDivInfo(MAP *pMap, UINT nFromNode, UINT nNextNode);  //puting

	bool CheckTurnOffOfPathInfo(MAP *pMap,UINT nCurNode);  //puting
	int CheckDeviceOfPathInfo(MAP *pMap,UINT nCurNode);  //puting

 	PATH_STEERING_INFO GetSteeringInfo(MAP *pMap, UINT nFromNode, UINT nNextNode); //puting

    // 2016-12-22, Add by Labeler
    int FrontSearchDrivingPath(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List);	///< PathSearch 결과 Return
    int FrontSearchDrivingPathMapMake(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List, int &nTargetNode);	///< PathSearch 결과 Return
    int FrontSearchDrivingPathAutoMoveTest(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List);	///< PathSearch 결과 Return
	int SearchExceptPathForV80(MAP * pMap, UINT FromNode, UINT ToNode, EXCEPT_PATH_LIST_V80 *pPath,int &TargetNode, BYTE MapMakeBrench=0x00);

	int GetIncreaseSQNumber();

	bool CheckNodeAndLinkIndex(MAP * pMap, UINT FromNode, UINT ToNode);
	bool CheckEndStationOffset(MAP * pMap, UINT ParentNode, UINT NextNode, int EndStationOffset);
	bool CheckSteering4Deccel(PARSING_GO_INFO *GetGOInfo, int ChangeNodeID);

};

//----------------속도 테이블에 따른 통과허가 거리값------------------------------/
//puting 통과허가를 위한 거리계산 부분 추가
inline double PathSearch::GetSpeedDownDist(double dAccel, double dSpeed)
{
	return (fabs((dSpeed*dSpeed)/(2*dAccel)));   // S=(V^2)/(2*a)
}


inline double PathSearch::GetMarginOfSpeedDist(double dSpeed, double Margin)
{
   //	return (fabs(sqrt(sqrt(sqrt(dSpeed))))*Margin);
   return (fabs(dSpeed*(Margin/1000))); // 통과허가 요청주기 * (속도 * 횟수)
}

inline UINT PathSearch::GetIncreaseSeqenceNumber()
{
	if(m_SquenceIndex ==65535) m_SquenceIndex = 0;
	return ++m_SquenceIndex;
}
inline UINT PathSearch::GetSeqenceNumber()
{
	return m_SquenceIndex;
}
//---------------------------------------------------------------------------
#endif
