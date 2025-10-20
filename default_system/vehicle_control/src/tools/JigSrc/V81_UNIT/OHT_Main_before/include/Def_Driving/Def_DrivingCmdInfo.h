#ifndef Def_DrivingCmdInfoH
#define Def_DrivingCmdInfoH

#include <stdio.h>
#include "Def_Variables.h"	// BYTE, WORD, UINT, UINT64
#include "Def_PathInfo.h"	// NODE_TYPE, PATH_STEERING_INFO, PATH_DIRECTION_INFO, MARK_TYPE

/**
@brief	 주행 명령 정보 (Node)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_NODE_
{
	UINT		ID;					///< NodeID
	NODE_TYPE	Type;				///< Path 종료 Node Type: 일반(0), 분기(1), 합류(2), 합류분기(3)
	double		dDistSum;	///< 노드까지의 거리
	int			nCount;				///< 시작부터 현재까지의 Node 개수( 0부터 시작 )
//	PATH_DIVERGENCE_INFO    DivInfo;			///< 분기 정보
//	SLOPE_NODE_INFO			SlopeInfo;
    DEVICE_NODE_TYPE		NodeType;
}DRIVING_CMD_INFO_OF_NODE;

/**
@brief	 주행 명령 정보 (Steer)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_STEER_
{
	PATH_STEERING_INFO	Type;				///< 조향 정보
	PATH_STEERING_INFO  NextType;			///< 선조향 정보:끝 부분에 노드가 사전 감지되면 조향 변경
	double				dDistSumToEndNode;	///< 동일 조향으로 이동하는 거리
	int					nCount;				///< 시작부터 현재까지의 Node 개수
}DRIVING_CMD_INFO_OF_STEER;

/**
@brief	 주행 명령 정보 (Steer)
@author  puting
@date	 2016.11.09
*/
typedef struct DRIVING_CMD_INFO_OF_DIV_
{
	PATH_DIVERGENCE_INFO	Type;				///< 조향 정보
	PATH_DIVERGENCE_INFO  NextType;			///< 선조향 정보:끝 부분에 노드가 사전 감지되면 조향 변경
	double				dDistSumToEndNode;	///< 동일 조향으로 이동하는 거리
	int					nCount;				///< 시작부터 현재까지의 Node 개수
}DRIVING_CMD_INFO_OF_DIV;


/**
@brief	 주행 명령 정보 (Direction)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_DIRECTION_
{
#if(USE_CHANGENODE == 1)
	PATH_DIRECTION_INFO		UBGRegion; 		///< 방향 정보 패턴 정보
	PATH_OBS_DIRECTION_INFO ObstacleRegion;
#else
	PATH_DIRECTION_INFO	Type;				///< 방향 정보 패턴 정보
#endif
	int					nPBSIgnoreFlag;
	int					nFollowsCommandType;
	double				dDistSumToEndNode;	///< 동일 방향으로 이동하는 거리
	int					nCount;				///< 시작부터 현재까지의 Node 개수
#if(USE_CHANGENODE == 1)
	// for change node (jhoun.lim)
	DEVICE_NODE_TYPE		NodeType;			// node type 	(ex : OPT_TAG, OPT_DISTANCE..)
	int						OptDistance;		// Change node 1까지의 거리
	int						OptDistanceAfter;		//
	PATH_DIRECTION_INFO		OptUBGRegion;		// Optional node 1에서 변경할 UBG type
	PATH_OBS_DIRECTION_INFO OptObstacleRegion;	// Optional node 1에서 변경할 OBS type
#endif
}DRIVING_CMD_INFO_OF_DIRECTION;

/**
@brief	 주행 명령 정보 (Speed)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_SPEED_
{
	double		dSpeed;					///< 속도
	double		dDistSumToFinalNode;	///< 동일 속도 최종 Node까지 거리 ( Node로의 주행: Target Node / Station으로의 주행: Next Node )
	int			nCount;					///< 시작부터 최종 Node까지의 개수
	
	MARK_TYPE	MarkType;				///< 최종 도착지의 MarkType ( 0:Node / 1:TagStation / 2:QRLeft / 3:QRRight )
	double		dDistSumToTarget;		///< 최종 도착지까지의 거리
	double		dDecelDist;				///< 감속 거리
}DRIVING_CMD_INFO_OF_SPEED;

/**
@brief	 주행 명령 정보 (Path)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_PATH_
{
	UINT					StartNID;			///< 시작 노드
	UINT        			EndNID;				///< 끝 노드
	NODE_TYPE				StartNType;			///< Path 종료 Node Type: 일반(0), 분기(1), 합류(2), 합류분기(3)	: TCU 대용으로 사용 가능
	NODE_TYPE				EndNType;			///< Path 종료 Node Type: 일반(0), 분기(1), 합류(2), 합류분기(3)	: TCU 대용으로 사용 가능
	PASSPERMIT_NODE_TYPE	StartType;			///< 시작 통과허가에 따른 Type   정지, 리셋, 공용, 일반
//	DEVICE_NODE_TYPE        DeviceType;         ///< Lifter In : 1, Lifter Out :2, MTL In : 3, MTL Out :4
	DEVICE_NODE_TYPE        NodeType;           ///< Lifter In : 1, Lifter Out :2, MTL In : 3, MTL Out :4-> (일반 : 0, 슬로프 : 1  (임시로 슬로프는 6)
	DEVICE_NODE_TYPE        NextNodeType;           ///< Lifter In : 1, Lifter Out :2, MTL In : 3, MTL Out :4-> (일반 : 0, 슬로프 : 1  (임시로 슬로프는 6)
	double      			dSpeed;				///< 속도
	double      			dLength;			///< Path의 길이
	double      			dDistSumToEndNode;	///< EndNode 까지의 거리 합
	int						nPBSIgnoreFlag;
	int         			nCount;				///< 시작부터 현재(End Node)까지의 Node 개수
	PATH_STEERING_INFO		Steering;			///< 조향 정보
#if(USE_CHANGENODE == 1)
	PATH_DIRECTION_INFO		UBGRegion;			///< 방향 정보
	PATH_OBS_DIRECTION_INFO ObstacleRegion;
#else
	PATH_DIRECTION_INFO		Direction;			///< 방향 정보
#endif
	PATH_STEERING_INFO		PreSteering;		///< 선조향 정보
	PATH_DIVERGENCE_INFO    DivInfo;			///< 분기 정보
	PATH_DIVERGENCE_INFO	PreDivInfo;         ///< 선분기 정보
#if(USE_CHANGENODE == 1)
	// for change node (jhoun.lim)
	int						OptDistance;		// Change node 1까지의 거리
	int						OptDistanceAfter;	//
	PATH_DIRECTION_INFO		OptUBGRegion;		// Optional node 1에서 변경할 UBG type
	PATH_OBS_DIRECTION_INFO OptObstacleRegion;	// Optional node 1에서 변경할 OBS type
#endif
}DRIVING_CMD_INFO_OF_PATH;

/**
@brief	 주행 명령 정보 (Brief)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_TARGET_
{
	MARK_TYPE				Type;			///< 최종 도착지의 Type
	
	UINT					Node;			///< Node ID ( Node로의 주행: Target Node, Station으로의 주행: Parent Node)
	double					dDistSumToNode;	///< 최종 도착지까지의 거리 ( Node로의 주행: Target Node 까지의 거리 / Station으로의 주행: Parent Node 까지의 거리 )
	int						nCount;			///< 동일 속도 최종 Node까지 nCount ( Node로의 주행: Target Node, Station으로의 주행: Next Node)

	UINT					Station;		///< Staion ID
	double					dOffset;		///< Node부터 Station까지의 길이
	double					dSTBOffset;		///< STB의 Station까지의 길이

	UINT					NextNode;		///< Next Node ID ( Node로의 주행: N/A, Station으로의 주행: Next Node )
	double					dLength;		///< Node부터 NextNode까지의 길이
	bool					PassBoxIn;
	PATH_STEERING_INFO		Steering;		///< 조향 정보: Station 일 때 사용
	unsigned short			SQNum;			///< 시퀀스 넘버
	unsigned char StationSafetyLevel;		///< SafetyLevel
	bool 		  bIgnolOverRun;			///< OverrunLevel

}DRIVING_CMD_INFO_OF_TARGET;

#endif
