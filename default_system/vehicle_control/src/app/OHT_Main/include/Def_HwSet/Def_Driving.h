#ifndef Def_DrivingH
#define Def_DrivingH


#include <stdio.h>


/**
@brief	 Motion 제어기로 부터 받는 User 단위로 변환된 Node 정보
@author  임태웅
@date	 2013.06.30
*/
typedef struct USER_NODE_CHECK_DATA_
{
	int NodeCountOfPreCheck;	///< 노드 개수
	int NodeCount;				///< 노드 개수
	
	// SetHome, setOffset 시 하면 현재 위치를 뺀 값으로 재저장
	double	PositionOfDrivingTagPreCheck;	///< 주행 노드 사전 감지된 위치		
	double	PositionOfDrivingTag;			///< 주행 노드 감지된 위치			 
	double	PositionOfTransTagFirst;		///< 이적재 노드 감지된 위치(Front)
	double	PositionOfTransTagFinal;		///< 이적재 노드 감지된 위치(Rear)
	
}USER_NODE_CHECK_DATA;

/**
@brief	 Motion 제어기에 전달할 근접제어 참고 data 를 만둘기 위한 User 단위로 되어있는 정보
@author  임태웅
@date	 2013.06.30
*/
typedef struct USER_DETECT_INFO_
{
	char	UseFollowControlFlag;	///< 근접 제어 사용 여부: true 면 사용, false면 미사용
#if(USE_CHANGENODE == 1)
	char	OHTDetectType;				///< Link의 UBGPattern
	char	OBSDetectType;				///< Link의 OBSPattern
#else
	char	Type;					///< Link의 Type
#endif
	int		NodeCount;				///< 노드 개수
	double	DistSumOfStraightPath;	///< 직진 길이 이어지는 지점까지의 위치 (User)
	int		FollowCommandType;		
	int		PBSIgnoreFlag;
#if(USE_CHANGENODE == 1)
	// for change node (jhoun.lim)
	char	NodeType;
	int		DistON;					// Change node 까지의 거리
	int		DistAfterON;			// Change node 만난 이후 거리
	char	OHTDetectTypeOpt;		// Optional node 1에서 변경할 type UBG
	char	OBSDetectTypeOpt;		// Optional node 1에서 변경할 type OBS
#endif
}USER_DETECT_INFO;


#endif
