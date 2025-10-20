/**
@file    EXECUTE_INFO.h
@version OHT 7.0
@brief   공유 구조체 선언 Header File
*/

#ifndef ExecuteInfoH
#define ExecuteInfoH

#include "List.h"
#include "MAP.h"
#include "../Common/Def_Protocol.h"
#include "../Common/Def_DataType.h"
#include "../Common/Protocol_Define_Modify.h"

#include "Def_PathInfo.h"
#include "Def_Variables.h"
#include "Def_DrivingCmdInfo.h"




/**
@brief   Main Thread와 Excute Thread 사이에 전달될 Driving용 Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_MODE_
{
	EXECUTE_MODE_MANUAL	= 0,		///< Manual Mode (초기화 상태도 Manual)
	EXECUTE_MODE_AUTO	= 1		///< Auto Mode
}EXECUTE_MODE;


/**
@brief   Main Thread와 Excute Thread 사이에 전달될 Driving용 Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_UNIT_
{
	EXECUTE_UNIT_NONE		= 0,		///< 초기화 상태
	EXECUTE_UNIT_DRIVING	= 1,		///< 주행 명령 -> Driving Ctrl		A/M
	EXECUTE_UNIT_TRANS		= 2,		///< 이적재 명령 -> Trans Ctrl		A/M
	EXECUTE_UNIT_MANUAL		= 3		///< Manual 명령 -> Manual Ctrl	_/M
}EXECUTE_UNIT;

/**
@brief   Main Thread와 Excute Thread 사이에 전달될 Driving용 Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_DRIVING_CMD_TYPE_
{
	EXECUTE_DRIVING_CMD_NONE= 0,		///< 초기화 상태
	EXECUTE_DRIVING_CMD_INIT,		///< 주행 명령 -> Driving Ctrl		A/M
	EXECUTE_DRIVING_CMD_GO,			///< 주행 명령 -> Driving Ctrl		A/M
	EXECUTE_DRIVING_CMD_GOBACK,	   	///< 후진 주행 명령 -> Driving Ctrl		A/M
	EXECUTE_DRIVING_CMD_MAPMAKE,	///< Map Make 명령 -> Driving Ctrl	_/M
	EXECUTE_DRIVING_CMD_CLEAN,		///< Manual 명령 -> Manual Ctrl	A/M
	EXECUTE_DRIVING_CMD_SET_STATION_OFFSET,	///< Station Offset 설정 -> Driving Ctrl	_/M
	EXECUTE_DRIVING_CMD_MARK,            ///< Mark 명령 -> Driving Ctrl			_/M
	EXECUTE_DRIVING_CMD_FLOOR,            ///< FLOOR 이동 명령 -> Driving Ctrl			_/M
	EXECUTE_DRIVING_CMD_MOVETEST  ///< FLOOR 이동 명령 -> Driving Ctrl			_/M
//	EXECUTE_DRIVING_CMD_MARK = 5            ///< Mark 명령 -> Driving Ctrl			_/M
}EXECUTE_DRIVING_CMD_TYPE;


/**
@brief   Main Thread와 Excute Thread 사이에 전달될 Driving용 Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_QUICK_CMD_
{
	EXECUTE_QUICK_CMD_NONE			= 0,		///< 초기화 상태		A/M
	EXECUTE_QUICK_CMD_STOP			= 1,		///< 긴급 정지		A/M     ~ OCS_Estop / Status Check 오는 Internal Cmd 가 E-stop / 
	EXECUTE_QUICK_CMD_PAUSE_OCS		= 2,		///< 일시 정지		A/M     ~ OCS_Pause  
	EXECUTE_QUICK_CMD_RESUME_OCS	= 3,		///< 재 실행		A/M     ~ OCS_Resume 	
	EXECUTE_QUICK_CMD_CANCEL		= 4,		///< 주행 명령 Clear	A/M     ~ OCS 에 경로 변경할 경우
	EXECUTE_QUICK_CMD_RESET			= 5,         ///< 주행 명령 Clear	A/M     ~ OCS 에 경로 변경할 경우
	EXECUTE_QUICK_CMD_PAUSE_TP		= 6,		///< 일시 정지		A/M
	EXECUTE_QUICK_CMD_RESUME_TP		= 7,			///< 재 실행		A/M
	EXECUTE_QUICK_CMD_TURN_OFF 		= 8,        ///< 기동 Off
	EXECUTE_QUICK_CMD_TURN_ON       = 9
}EXECUTE_QUICK_CMD;

/**
@brief   Excute Thread 와 D,T,M 사이 명령에 대한 응답
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_ACK_
{
	EXECUTE_ACK_NO_ERR				= 0,		///< 정상 응답 			A/M
	EXECUTE_ACK_BUSY				= 1,		///< 명령 받을 수 없는 상태	A/M
	EXECUTE_ACK_NOT_AVAILABLE_CMD	= 2,		///< 처리할 수 없는 명령		A/M
	EXECUTE_ACK_ON_ERR				= 3
}EXECUTE_ACK;

/**
@brief   Main Thread와 Excute Thread 사이에 전달될 Driving용 Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_RESPONSE_TYPE_
{
	EXECUTE_RESPONCE_NONE		= 0,		///< 초기화 상태					A/M
	EXECUTE_RESPONCE_MAPMAKING	= 1,		///< MapMaking 응답 -> Driving Ctrl		A/M
	EXECUTE_RESPONCE_TRANS		= 2		///< 티칭 응답 -> Trans Ctrl			_/M
}EXECUTE_RESPONCE_TYPE;

/**
@brief   Main Thread와 Excute Thread 사이에 전달될 Driving용 Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_HIDPATH_TYPE_
{
	EXECUTE_HIDPATH_NONE		= 0,		///< 메뉴얼
	EXECUTE_HIDPATH_LINE		= 1,		///< 직선
	EXECUTE_HIDPATH_CURVE		= 2,		///< 곡선
	EXECUTE_HIDPATH_SLOOP		= 3,		///< 경사로 Auto 상태일때만 표시함.
}EXECUTE_HIDPATH_TYPE;

/**
@brief   Main Thread와 Execute Thread 사이에 Return 하는 Driving용 정보 for TCU
@author  ehoto
@date    2012.11.12
*/
typedef struct _FLAG_MERGE_POINT_BIT_
{
	char isNear			: 1;
	char isSpeedDown	: 1;
	char isStoped		: 1;
	char researved_4	: 1;
	char researved_5	: 1;
	char researved_6	: 1;
	char researved_7	: 1;
	char researved_8	: 1;
}FLAG_MERGE_POINT_BIT;

typedef union _FLAG_MERGE_POINT_
{
	BYTE ByteValue;
	FLAG_MERGE_POINT_BIT BitValue;
}FLAG_MERGE_POINT;

/**
@brief   CYCLELIST에 파싱하기전에 필요한 주행 정보를 위한 구조체
@author  puting
@date    2015.11.06
*/
typedef struct
{
	unsigned short			CmdID;				// 지시 ID

   	unsigned int 			CmdCycle;           //작업 사이클

    MARK_TYPE				MarkType;    // Target 이적재 Tag Type
	unsigned int			StopNodeID;		// 목적지노드
	unsigned int			StopStationID;	// 목적지 Station노드

	unsigned int 			PhysicalID;

//	int						StationData_StartRunDist;	// Node로부터 이적재 Tag까지 거리 : 주행이면 0으로 표시
//	unsigned int			StationDataStart_NextNodeID;		// 다음 노드

	unsigned int			StationData_ParentNodeID;		// 다음 노드
	unsigned int			StationData_NextNodeID;		// 다음 노드
	int						StationData_RunDist;	    // Node로부터 이적재 Tag까지 거리 : 주행이면 0으로 표시
    int						TeachingData_STBOffset;  //  추가

	unsigned int			NextNodeID;		// 다음 노드


	BYTE					PathData_Def_Direction;	// 기본 분기방향
	unsigned int			PathData_StartNodeID;	// 개시번지
	unsigned int			PathData_EndNodeID;	// 종료번지
	unsigned short			PathData_ExtraNum;	//	기본외 분기 번지수

	unsigned int  			PathData_ExtraID[MAX_PATH_SIZE];

	bool					bIsTargetStationoPassPermitOpen;  //정지노드에 속한 이적재 태그일 경우 반납 유무 20170730


} PARSING_GO_INFO;


/**
@brief   CYCLELIST에 가공한 필요한 주행 정보를 위한 구조체
@author  puting
@date    2015.11.06
*/
typedef struct
{
	unsigned int			OriginStartNodeID;	// 부모노드위치
	unsigned int			OriginEndNodeID;	// 부모노드위치

	unsigned int			StartNodeID;	// 출발 노드가 일반 주행일 경우 OriginNID하고 동일 : Station은 Next Node
	unsigned int			EndNodeID;		// 목적지 노드가 일반 주행일 경우 OriginNID하고 동일 : Station은 Next Node

	bool   	         		IsStationStart;
	bool            		IsStationEnd;

	int			StationData_StartRunDist;	// 부모 Node로부터 이적재 Tag까지 거리 :
	int			StationData_EndRunDist;	    // 부모 Node로부터 이적재 Tag까지 거리 :

	int			StationData_StartRunDistOfNextNode;		// Next로부터 이적재 Tag까지 거리
	int			StationData_EndRunDistOfNextNode;	    // Next로부터 이적재 Tag까지 거리

	BYTE					PathDiv;					//분기정보 (0 : 해당없음, 1 : 왼쪽 2: 오른쪽 3: 직선.

  /*
	PATHDATA_DEF_DIRECTION	PathData_Def_Direction;	// 기본 분기방향
	unsigned int			PathData_StartNodeID;	// 개시번지
	unsigned int			PathData_EndNodeID;	// 종료번지
	unsigned short			PathData_ExtraNum;	//	기본외 분기 번지수

	unsigned int  			PathData_ExtraID[MAX_PATH_SIZE];
  */

} PROCESSING_GO_INFO;


typedef struct MAP_MAKER_TYPE_
{
	BYTE MapMake_DrivingTag			:1;               ///< 주행 MapMaer
	BYTE MapMake_StopTag	  		:1;               ///< 주행 MapMaer
	BYTE MapMake_QR_Left_Tag 		:1;               ///< 주행 MapMaer
	BYTE MapMake_QR_Right_Tag 		:1;               ///< 주행 MapMaer
	BYTE MapMake_Reserved			:4;               ///< 주행 MapMaer
} MAP_MAKER_TYPE;

/**
@brief	 Main Thread와 Execute Thread 사이에 전달될 Driving용 Cmd List
@author  ehoto
@date	 2012.11.12
*/
typedef struct EXECUTE_DRIVING_ADD_INFO_
{	 
	// 명령 타입
	EXECUTE_DRIVING_CMD_TYPE	CmdType;	///< Go, MapMake, Clean, Mark(?)

	// 이적재 시 추가되는 정보
	UINT		StationID;						///< Path 종료 Node ID
	UINT 		PhysicalID;
	MARK_TYPE	StationType;						///< 이적재(NONE:0/TAG:1/QRL:2/QRR:3)
	double		DistToStation;					///< 이적재 노드까지의 거리
	double		STBOffset;					///< 이적재 노드까지의 거리
	unsigned char StationSafetyLevel;		///< SafetyLevel
	bool 		  bIgnolOverRun;			///< OverrunLevel

	double		DistOffsetForStartFromStation;	///< 이적재 노드에서 처음 초기화 되면 상위로부터 받는값
	UINT		FinalNode;						///< 명령 받은 최종 노드 : 주행 TAG 만 해당됨.. (Station이 목적지 일 경우 Next Node)

	// Map Make를 위해 추가되는 정보
	UINT		SpeedLimitOfMapMake;			///< Map Make Speed Limit
	MAP_MAKER_TYPE	MapMakeType;

	UINT		FinalPortType;

	PROCESSING_GO_INFO 	ProcessingGoInfo; 		///< 주행에 필요한 가공된 Data(이적재일 경우에 대한 거리값과 노드 정보)


} EXECUTE_DRIVING_ADD_INFO;

/**
@brief	 주행 명령 정보 (Path)
@author  puting
@date	 2013.11.09
*/
typedef struct DRIVING_CYCLE_ADD_INFO
{
	EXECUTE_DRIVING_CMD_TYPE	CmdType;	///< Go, MapMake, Clean, Mark(?)

	UINT					StartNID;			///< 시작 노드
	UINT        			EndNID;				///< 끝 노드

	NODE_TYPE				StartNType;			///< Path 종료 Node Type: 일반(0), 분기(1), 합류(2), 합류분기(3)	: TCU 대용으로 사용 가능
	NODE_TYPE				EndNType;			///< Path 종료 Node Type: 일반(0), 분기(1), 합류(2), 합류분기(3)	: TCU 대용으로 사용 가능

	double      			dSpeed;				///< 속도
	double      			dDistSumToEndNode;	///< EndNode 까지의 거리 합

	PATH_STEERING_INFO		Steering;			///< 조향 정보
	PATH_DIRECTION_INFO		Direction;			///< 방향 정보
	PATH_DIVERGENCE_INFO  	Division;			///< 분기 정보 왼쪽/오른쪽(디폴트왼쪽)

	UINT					TargetStationID;

}DRIVING_CYCLE_ADD_INFO;

/**
@brief	 Main Thread와 Execute Thread 사이에 전달될 Driving용 Cmd List
@author  ehoto
@date	 2012.11.12
*/
typedef struct EXECUTE_DRIVING_INFO_
{	 
	//2	정보 입력 시에 사용되는 정보
	UINT 						PathIndexNum;    ///<Path의 Index Num
	UINT						CmdID;	 	  ///< 지시 ID
	unsigned short				SQNum;					// 시퀀스 넘버

	//경로 변경을 하기 위한 정보.
//	BYTE						StartNodeType;    //0: 일반, 1 ; 분기, ///2: 사이클의 시작 or 끝  3: 사이클의 시작 or 끝 && 분기
//	BYTE						EndNodeType;      //0: 일반, 1 ; 분기, ////2: 사이클의 시작 or 끝  3: 사이클의 시작 or 끝 && 분기
	double						ChangeEndNodeDistance;     //출발지점부터 마지막 위치까지 거리값.

	DRIVING_CMD_INFO_OF_PATH	BasicInfo;
	EXECUTE_DRIVING_ADD_INFO	AddInfo; ///< 주행 Map Search 이외에 필요한 정보

	// 정지 했을 때 정위치 확인 용 
	double				RealPositionOfStop[2];	///< 최종 정지 했을 때의 엔코더 값(보정되지 않은 위치 값)
 //	bool 				bStartNode;						//시작 노드인지 통과중에 정지한건지 확인.

} EXECUTE_DRIVING_INFO;

/**
@brief	 Main Thread와 Execute Thread 사이에 전달될 Driving용 Cmd List
@author  puting
@date	 2015.11.09
*/
typedef struct CYCLEDRIVING_PASSPERMIT_INFO_
{
	//2	정보 입력 시에 사용되는 정보
//	bool									CheckCurLinkPPStartPoint;	//통과허가 개방 요청 유무
//	bool									CheckPreLinkPPOpenStartPoint;
//	bool									CheckCurLinkPPEndPoint;		//통과허가 개방 요청 유무
//	bool									CheckCurLinkPPOpenEndPoint;		//통과허가 개방 요청 유무
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///다른개념 적용
	bool									IsCurLinkPPPoint;			//현재 Link의 통과허가 요청유무
	bool									IsCurLinkPPOpenPoint;       //현재 Link의 통과허가 개방 요청 유무
//	bool									IsNextLinkPPPoint;			//현재 Link의 통과허가 개방 요청 유무

//	bool									IsPreLinkPPOpenPoint;		//이전 Link의 통과허가 개방 유무( 시작시와 마지막에 확인함. 나머지는 모두 true)
	bool									IsTargetStationPPOpenPoint;		//마지막 위치가 Station인경우 통과허가 반납 유무에 대한 플래그 (디폴트는 false)


}CYCLEDRIVING_PASSPERMIT_INFO;

/**
@brief	 작업변경을 위한 분기정보에 대한 정보들
@author  puting
@date	 2015.11.09
*/
typedef struct CYCLEDRIVING_CHANGE_INFO_
{
	UINT									ChangeNodePoint;				// 변경 Point Node
	BYTE									ChangeNodeType;					// 분기 :1 , 사이클 시작과 끝 :2, 분기 &&사이클 시작과 끝 : 3
	double									ChangeNodeDistance;       		//변경 Point까지 거리값
}CYCLEDRIVING_CHANGE_INFO;


/**
@brief	 Main Thread와 Execute Thread 사이에 전달될 Driving용 Cmd List
@author  ehoto
@date	 2012.11.12
*/
typedef struct EXECUTE_CYCLEDRIVING_INFO_
{
	//2	정보 입력 시에 사용되는 정보
	CYCLESTEP								D_PPStep;		   	 	///< 통과허가 관련된 Step
	NEXTCYCLESTEP							D_PPOStep;		   	 	///< 통과허가 관련된 Next Stop Step
	CYCLENODEINFO							CycleNodeInfo; 		///<시작~ 정지노드, 정지~ 정지, 정지~ 목적지, 시작~ 목적지

	CYCLEDRIVING_PASSPERMIT_INFO		 	PassPermitInfo;    	///<시작시/끝점에서  통과허가 개방 요청 유무 확인하는 부분

	UINT								    StartPoint;   		///< 시작노드 위치
	double								    StartDistance;   	///< 출발부터 시작Node까지 거리
	double								    StopDistance;   	///< 출발부터 시작Node까지 거리
	PASSPERMIT_NODE_TYPE				    StartNodeType;   	///< 시작 점의 정지/리셋/공용노드 정보 확인

	UINT									EndPoint;     		///< 끝 노드 위치(주행 노드)
	UINT									FinalPoint;     		///< 끝 노드 위치(주행 노드)
	double									EndDistance;      	///< 출발부터 끝 Node까지 거리
	PASSPERMIT_NODE_TYPE					EndNodeType;     	///< 끝 점의 일반/정지/리셋/공용노드 정보 확인

	UINT								    ResetPoint;   		///< Reset노드 위치
    double									ResetNodeDistance;  ///< 출발부터 해당 통과허가구간의 Reset Node까지 거리(없을 경우 EndDistance로 설정할 것.

	PASSPERMIT_COMMON						CurLinkPassPermitData;	    ///< 통과허가 정보. 시작지점에 대한

	UINT									DivCount; 			 	//현재싸이클 내에서의 분기 갯수
//	CYCLEDRIVING_CHANGE_INFO		  		DivInfo[10];	 	   //현재싸이클 내에서의 분기 정보 (Max 10개 설정, 레일 기획측에 문의 필요.)


	DRIVING_CYCLE_ADD_INFO		AddInfo; ///< 주행 Map Search 이외에 필요한 정보

	// 정지 했을 때 정위치 확인 용


} EXECUTE_CYCLEDRIVING_INFO;


 /**
@brief   CYCLELIST에 파싱하기전에 필요한 이적재 정보를 위한 구조체
@author  puting
@date    2015.11.06
*/
typedef struct
{
	unsigned short			CmdID;				// 지시 ID

	unsigned int 			CmdCycle;
	unsigned int			TrasType;			//작업 구분

    unsigned char			AutoRecovery;	    // E84 Time Out 발생 유무.
//	WORKCARRYTYPE			WorkCarryType;		//반송물 Type
	unsigned char			StationData_PortID[12];    // Target 이적재 Tag Type

	TRANS_CMD               TransCmd;

} PARSING_TRANS_INFO;

/**
@brief   Main Thread와 Execute Thread 사이에 전달될 Trans용 Cmd List
@author  zzang9un
@date    2013.08.15
*/
typedef struct EXECUTE_TRANS_INFO_
{
    uTAG_DATA   CmdTag;         ///< Command Tag
    BYTE        TransType;      ///< Trans Type (L, U...)

    TRANS_CMD   TransCmd;       ///< 이적재 Command 구조체

    /* // [BEGIN] by zzang9un 2013.08.15 : 중복된 변수를 하나의 구조체로 통일함
    // by zzang9un 2012.11.19 : TransType은 CmdTag에서 알 수 있으므로 아래 변수 중복됨
    BYTE        TransType;      ///< Trans Type (L, U...)
    UINT        TransTagID;     ///< Trans Tag ID
    UINT        PortType;       ///< EQ Type(EQ, STB, UTB, FOUP LOADER...)
    UINT        STBDir;         ///< STB Direction(None, Left, Right)
    int         Hoist;          ///< Hoist Teaching 값(mm)
    int         Shift;          ///< Shift Teaching 값(mm)
    int         Rotate;         ///< Rotate Teaching 값(radian)
    BYTE        LookDown;       ///< LookDown Sensor On/Off
    BYTE        TransSound;     ///< 이적재 사운드 On/Off
    BYTE        Oscillation;        ///< Oscillation Sensor On/Off
    UINT        HoistLevel;     ///< Hoist 축 속도 레벨 설정
    UINT        SlideLevel;     ///< Shift 축 속도 레벨 설정
    UINT        PIODirection;       ///< 사용하는 EQ PIO(Left, Right)
    UINT        PIOTimeLevel;   ///< 포트 별 EQ PIO Timeover 레벨
    BYTE        HandDetectEQ;   ///< 컨베이어나 Stoker처럼 이적재 시 OHT 핸드를 감시하는 설비 포트
    BYTE        PIOAutoRecovery;    ///< PIO 관련 에러가 발생했을 때 Auto Recovery를 할지 말지 결정
    */ // [END] by zzang9un 2013.08.15 : 중복된 변수를 하나의 구조체로 통일함
} EXECUTE_TRANS_INFO;

/**
@brief   Main Thread와 Execute Thread 사이에 전달될 Manual Control용 Cmd List
@author  zzang9un
@date    2013.08.17
*/
typedef struct EXECUTE_MANUALCONTROL_INFO_
{
	// Command Info
	uTAG_DATA uCmdTag; ///< Command Tag 정보

	uTP_CMD	uTPCmd;
} EXECUTE_MANUALCONTROL_INFO;

typedef struct NODE_OFFSET_INFO_
{
	// 종속 주행 노드 정보 
	UINT			StartNodeID;		///< Path 시작 Node ID
	UINT			EndNodeID;		///< Path 종료 Node ID
	UINT			Dist;			///< 시작 Node로 부터의 실제 Path 거리(mm)
}NODE_OFFSET_INFO;

typedef struct NODE_DATA_INFO_
{
	UINT NID;               ///< Node ID
	UINT Point_X;           ///< X좌표
	UINT Point_Y;           ///< Y좌표
} NODE_DATA_INFO;


/**
@brief	 Main Thread와 Execute Thread 사이에 Return 하는 Driving용 MapMaking Result List
@author  ehoto
@date	 2012.11.12
*/
typedef struct EXECUTE_RESPONSE_MAPMAKING_INFO_
{
	// Command ID
	UINT				CmdID;				///< 응답에 붙이는 ID

	// 이적재 정보
	UINT				StationID;			///< Station ID
	MARK_TYPE		StationType;			///< Station Type (NONE:0/TAG:1/QRL:2/QRR:3)
	NODE_OFFSET_INFO	NodeOffsetInfo[2];	///< 전후 Node 및 Offset 정보
	NODE_DATA_INFO		NodeDataInfo;		///< Node 좌표

	//AOHC-369 STB 확장형 적용을 위한 Map Make 기능 개
	//Virtual Node 정보
	bool			bUseVirtualNode;	//Virtual Node 사용 여부
	UINT			nOptionalDistance;	//Optional Distance 수정이 필요한 경우
} EXECUTE_RESPONSE_MAPMAKING_INFO;


/**
@brief	 Main Thread와 Execute Thread 사이에 Return 하는 Driving용 MapMaking Result List
@author  ehoto
@date	 2012.11.12
*/
typedef struct EXECUTE_POSITION_INFO_
{
	// 현재 위치 정보
	UINT	CurrNode;				///< 주행 노드 ID
	UINT	CurrPreCheckNode;	 	///< 주행 노드 Precheck Sensor ID
	UINT	NextNode;				///< 주행 노드 ID
	int		CurrNodeOffset;			///< 소수점 절사(mm)
	UINT	CurrStation;				///< 이적재 노드 마크 후에만 업데이트

	// 명령 위치 정보
	UINT	CurrCommandedNode;		///< 제어기로 보낸 최종 주행 노드 ID (논의 필요) - OCS Reroute 시 찹고 자료 제공 목적
	UINT	CurrCommandedStation;	///< Target 이적재 Node ID

//	double  dCompensatedPosition;	///< 오차를 보상한 기준 위치( 엔코더값 - 누적 거리 오차값 + 출발 Station Offset )

	// 정위치 정보
	MARK_TYPE	StationType;		///< Station Type (NONE:0/TAG:1/QRL:2/QRR:3)
	bool			Marked;			///< 마킹 완료 여부 정보

} EXECUTE_POSITION_INFO;        ///상태 보고 정보값  ------- puting



/**
@brief   PKT_CYCLECMDDATA 정보를 위한 구조체
@author  puting
@date    2015.10.31
*/
typedef struct
{


	/*
			공통 인자
			 1. Cycle Type  : D/T/M
			 2. 지시 ID (Manaul은 업성)
			 3. 시퀀스 Number(메뉴얼하고 이적재는 없어됨)
			 4. DTM Excuteinfo에 넘겨준 유무
			 5. DTM Step check
			   >>주행관련 명령
			   1) 통과허가 응답 전
			   2) 통과허가 개방 응답 전(통과허가 반납 시, 1)로 복귀)
			   3) 통과허가 개방 응답 후(Isexcuteinfo와 같이 확인되면 주행 CMD POP)  -----> 다른버퍼에 임시 저장
			   >>이적재 관련 명령(Isexcuteinfo와 같이 확인되면 이적재 CMD POP)
			   //11) 이적재 허가 전
			   //12) 이적재 허가 후(Isexcuteinfo와 같이 확인되면 이적재 CMD POP)
			   >>메뉴얼 관련 명령(Isexcuteinfo와 같이 확인되면  메뉴얼 CMD POP)
			   //21) Reserved


			 //별개로 필요한 Data(Excute Info에 바로 넘겨줄 정도)
			 1. 주행관련 Data
			   - Driving 관련하여 Pass Search를하여 바로 전달 가능 할 정도 의 수준으로 정리 할 것.
			 2. Trans 관련 Data
			   - MTL/층간이동/ 올림/ 내림 정의에 의해 분류하여 저장 할 것.
			 3. Manual 관련 Data
			   -Manual 기본 Data

		*/
	//EXECUTE_MODE							Mode; //


	///> Common <///
	CYCLETYPE								CycleType; 	  ///< D/T/M
	UINT									CmdID;	 	  ///< 지시 ID
	bool									IsExcuteInfo; ///< ExcuteInfo로 전송 유무 확인
	bool									IsNoExcuteCycle; ///< 싸이클 실행유무 (빼냄싸이클일 경우 컷)
	bool									IsListF2B; 	  ///< List Front ->back 유무 확인
	bool									Isreturn;      ///< 작업변경에 의한 통과허가 반납 유무



	///> Driving <///
	EXECUTE_CYCLEDRIVING_INFO			   	EXE_CycleDriving_Info;		///< CycleList의 정보 확인후 Execute Thread에 실제로 전달되는 부분

	///> Trans <///
	EXECUTE_TRANS_INFO						EXE_Trans_Info;			///< Execute Thread에 실제로 전달되는 부분

	///> Manaul <///
	EXECUTE_MANUALCONTROL_INFO				EXE_Maual_Info;			///< Execute Thread에 실제로 전달되는 부분

} PKT_CYCLECMDDATA;


typedef struct _DATA_PASSOPEN
{
	UINT													PassOpenIndex;	 	  ///< 지시 ID
    PASSPERMIT_COMMON										PassOpneInfo;		  ///<
	BYTE													bPassOpenType;		  /// 0x01 일반 반납, 0x02 강제통과 반납. 0x03 : 작업변경에 의한 반납

	PROTOCOL_BODY_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247	CmdID247Info;   ///강제통과허가 요구 정보
} DATA_PASSOPEN;

typedef struct _DATA_PASS
{

	PASSPERMIT_COMMON										PassInfo;		  ///<
	BYTE													bCIDType;		  /// 0x00 CId 일반 점유, 0x01 CID 무시

} DATA_PASS;




/**
@brief	 Main Thread로 Excute Thread의 상태 및 Cmd Id를 전달하기 위해 사용하는 구조체( sink를 맞추기 위해 Union의 Data 단위로 대입하여 사용 )
@author  임태웅
@date	 2013.04.03
*/
typedef struct STATUS_CMD_ID_
{
	UINT	Status	:8;								///< I, A, G, L, O, U, N ...
	UINT	PreCmd	:8;
	UINT	CurrCmd :8;
	UINT	NextCmd :8;
} STATUS_CMD_ID;

/**
@brief	 Main Thread로 Excute Thread의 상태 및 Cmd Id를 전달하기 위해 사용하는 Union( sink를 맞추기 위해 Union의 Data 단위로 대입하여 사용 )
@author  임태웅
@date	 2013.04.03
*/
typedef union STATUS_CMD_ID_UNION_
{
	STATUS_CMD_ID	Info;
	UINT			Data;

} STATUS_CMD_ID_UNION;

/**
@brief	 Main Thread가 Excute Thread에 명령을 인가할 때 사용하는 구조체 ( sink를 맞추기 위해 Union의 Data 단위로 대입하여 사용 )

	1) FlagMainThreadUsing 을 true로 변경
	2) 명령 리스트에 전달 후 이상 없으면 CmdID 를 대입
	3) FlagMainThreadUsing 을 false로 변경	
@author  임태웅
@date	 2013.04.03
*/
typedef struct CMD_ON_ADDING_
{
	UINT	FlagMainThreadUsing	:1;	///< MainThread가 Cmd 추가 중이 아닐 경우만  Excute Thread 가 Cmd를 가져가 사용한다
	UINT	CmdID				:4;	///< OCS로 부터 받는 명령 ID
	UINT	Reserved			:27;
} CMD_ON_ADDING;

/**
@brief	 Main Thread가 Excute Thread에 명령을 인가할 때 사용하는 Union ( sink를 맞추기 위해 Union의 Data 단위로 대입하여 사용 )
@author  임태웅
@date	 2013.04.03
*/
typedef union CMD_ON_ADDING_UNION_
{
	CMD_ON_ADDING	Info;
	UINT			Data;
} CMD_ON_ADDING_UNION;

/**
@brief	 Excute Thread가 Main Thread에 Map Make 결과를 Return할 때 사용하는 구조체 ( sink를 맞추기 위해 Union의 Data 단위로 대입하여 사용 )

	1) FlagMainThreadUsing 을 true로 변경
	2) 명령 리스트에 전달 후 이상 없으면 CmdID 를 대입
	3) FlagMainThreadUsing 을 false로 변경	
@author  임태웅
@date	 2013.04.03
*/
typedef struct MAP_INFO_ON_ADDING_
{
	UINT	FlagOnMapMaking			:1;	///< MApMaking 중임을 알려주는 Flag
	UINT	FlagExecuteThreadUsing	:1;	///< ExecuteThread가 Result 추가 중이 아닐 경우만  Main Thread 가 Result를 가져가 사용한다
	UINT	CountOfResult			:30;	///< 축척된 결과(MapMake, Teaching) : 0~2,147,483,647(2^31) 개
} MAP_INFO_ON_ADDING;

/**
@brief	 Main Thread가 Excute Thread에 명령을 인가할 때 사용하는 Union ( sink를 맞추기 위해 Union의 Data 단위로 대입하여 사용 )
@author  임태웅
@date	 2013.04.03
*/
typedef union MAP_INFO_ON_ADDING_UNION_
{
	MAP_INFO_ON_ADDING	Info;
	UINT				Data;
} MAP_INFO_ON_ADDING_UNION;

/**
@brief	호기당 이적재 위치를 맞추기 위한 TransUnit의 호기의 기준점과의 차이
@author  임태웅
@date	 2013.04.03
*/
typedef struct TRANSE_DIFFERENCE_
{
	volatile double dHoist;	///< Rail 부터 Jig 바닥면까지의 차이
	volatile double dShift;	///< MTL 부터 Shift 원점까지의 차이
	volatile int dRotate; ///< Ratate 원점으로 부터까지의 차이
	volatile int dRotate_90; ///< Ratate 원점으로 부터까지의 차이
	volatile int dRotate_180; ///< Ratate 원점으로 부터까지의 차이
} TRANSE_DIFFERENCE;

/**
@brief	누적 거리 값
@author  박태욱
@date	 2014.06.20
*/
typedef struct ACCUMULATION_DATA_
{
	volatile double dDrivingEncoder;	///< 주행부 누적 거리값
	volatile double dHoistEncoder;	    ///< 호이스트부 누적 거리값
	volatile double dShiftEncoder;	    ///< 쉬프트부 누적 거리값
} ACCUMULATION_DATA;

/**
@brief	초기화 위치 값
@author  박태욱
@date	 2015.11.16
*/
typedef struct FLAG_INIT_POSITION_
{
	volatile bool   bInitFlag;
	volatile double dDrivingInitPosition;	///< 주행부 누적 거리값
} FLAG_INIT_POSITION;

/**
@brief	Overrun 정보
@author  박태욱 puting
@date	 2016.05.27
*/
typedef struct FLAG_OVERRUN_INFO_
{
	volatile bool   bCheck; //오버런 발생 유무.
	volatile BYTE   OverrunType; //오버런 종류 : 주행 1, 이적재 2
	volatile unsigned int nFinalNode;	///< 오버런 발생 노드
	volatile double dDrivingInitPosition;	///< 오버런 발생노드로부터 떨어진 거리.

} FLAG_OVERRUN_INFO;


/**
@brief	CID 정보
@author
@date	 2015.11.16
*/
typedef struct FLAG_CID_INFO_
{
	volatile bool   bCIDErrorFlag;//	OCS에 이상보고 하기 위한 Flag
	volatile char   cCIDStateResult;
	volatile char   sCIDStatus[3];

	volatile bool   bMoveRestart;
	volatile char   cMoveResult;
} FLAG_CID_INFO;

/**
@brief	CID 정보
@author
@date	 2015.11.16
*/
typedef struct FLAG_CID_REPORT_
{
	volatile bool   bCIDInterlock;	//	OCS에 CID 상태 보고  하기 위한 Flag
	volatile bool   bCIDStop;		//	OCS에 CID 상태 보고  하기 위한 Flag
} FLAG_CID_REPORT;


/**
@brief	CID 정보
@author
@date	 2015.11.16
*/
typedef struct FLAG_CID_MONITORING_
{
	volatile bool   bCIDFlag;
	volatile int    nStopNodeID;
	volatile int    nResetNodeID;
	volatile char   cResult;
	volatile char   cMonitoring[53];	// CID_Monitoring_PKT, CID_Monitoring_BODY size check
} FLAG_CID_MONITORING;

/**
@brief	CID 정보
@author
@date	 2015.11.16
*/
typedef struct FLAG_CID_NOT_USE_
{
	volatile bool   bCIDUse;
	volatile int    nStopNodeID;
	volatile int    nResetNodeID;
} FLAG_CID_NOT_USE;

/**
@brief	사전동작 관련 정보
@author  박태욱
@date	 201612.28
*/
typedef struct FLAG_PRETRANS_
{
	volatile bool  bPreShutter; 		///< 싸이클 명령
	volatile bool  bPrePIO; 		 	///< 싸이클 명령
	volatile int   nTargetID;      	 	///< 최종 위치
	volatile int   nPIOID;      	 	///< PIO ID
	volatile int   nPIOChannel;         ///< PIO 채널
//	volatile char  cPIOType;		    ///< IR, RF, H-PIO
	volatile char  cPortType;		    ///< EQ, EQ_X, STOR_ STB, UTB 등
	volatile int   nPIOCS;
	volatile int   nPIOType;
} FLAG_PRETRANS;




/**
@brief	초기화 위치 값
@author  박태욱
@date	 2015.11.16
*/
typedef struct FLAG_MTL_INFO_
{
	volatile int   nTakeOut; // 뺴냄
	volatile int   nTakeIn;	 ///< 투입
	volatile int   nRailNum; //레일 번호
	volatile int   nTakeInOrder; //투입
	volatile bool  bTakeoutCycle; //빼냄싸이클 유무
	volatile bool  bTakeInCycle; //투입중 유무
	volatile bool  bTakeoutReport; //빼냄보고 플래그
} FLAG_MTL_INFO;

/**
@brief	초기화 위치 값
@author  박태욱
@date	 2015.11.16
*/
typedef struct FLAG_PATHCHANGE_
{
	volatile bool  bPathChange; //경로 변경 유무 확인 플래그
	volatile int   nChageNodeID; // 경로 변경 노드 위치
	volatile char  cResult;		 ///< 경로 변경 결과. //0 초기화, 1 실패  2 성공


} FLAG_PATHCHANGE;

/**
@brief	주행 이적재 명령 전달 유무 Flag
@author  puting
@date	 2018.09.06
*/
typedef struct FLAG_CMD_EXCUTE_
{
	volatile bool  bCmdTransCMDExcute; ///<이적재 명령 전달 유무 Flag
	volatile bool  bCmdDrivingCMDExcute; ///<주행 명령 전달 유무 Flag
  //	volatile bool  bCmdManualCMDExcute; ///<메뉴얼 명령 전달 유무 Flag

} FLAG_CMD_EXCUTE;


/**
@brief	 Main Thread와 Excute Thread가 사용할 공유 구조체
@author  zzang9un
@date	 2012.10.18
*/
typedef struct EXECUTE_INFO_
{
	//1 1. MainControlThread to Execute Thread -------------------------------------
	volatile	EXECUTE_MODE		ExecuteMode;	///< M,A
	volatile	EXECUTE_UNIT		ExecuteUnit;	///< D,T,M,...+ MapMake, Mark

	LList							ListOfCmd;		///< void* type CMD List
	volatile	CMD_ON_ADDING_UNION	OnAddingCmd;	///< [Flag+Info] MainThread가 Cmd 추가 중임을 나타내는 지표

	volatile	EXECUTE_QUICK_CMD	QuickCmd;		///< 동작 정지 명령 등의 긴급 명령

    volatile    int QuickCmdEStopCommander;
    volatile    int QuickCmdEStopType;	
	    
	//1 2. ExecuteThread to MainControlThread --------------------------------------
    volatile	STATUS_CMD_ID_UNION 		StatusCmdId;			///< 현재 상태 정보(I, A, G, L, O, U, N ...) 및 Cmd ID
	volatile	int							ErrCode_ExecuteInfo;	///< ExecuteInfo의 에러 정보
 	
	EXECUTE_POSITION_INFO					ExecutePositionInfo;	///< 현재 위치 정보
	volatile	bool				   		bPopListOfCmd;          ///< List에 정보를 뺴갔는지 확인하는 변수
	
	List<EXECUTE_RESPONSE_MAPMAKING_INFO*>	ListOfMapInfo;			///< void* type 응답 List : MapMake 시 결과 / (논의 필요)
	volatile	MAP_INFO_ON_ADDING_UNION	OnAddingMapInfo;		///< [Flag+Info] Execute Thread가 Return Value 추가 중임을 나타내는 지표
	volatile	EXECUTE_RESPONCE_TYPE		ResponceType;			///< Execute에서 MainThread로의 응답 Type (논의 필요)

	volatile	TRANSE_DIFFERENCE			TransDifference;		///< 호기당 이적재 위치를 맞추기 위한 TransUnit의 호기의 기준점과의 차이

	volatile	ACCUMULATION_DATA	   		AccumulationData; 	    ///< 누적 거리값.
	//미사용	FLAG_MERGE_POINT		 	FlagMergePoint; 	///< 합류 접근 상태 표시 Flag : Bit 단위 처리 (논의 필요)
	volatile	FLAG_OVERRUN_INFO			FlagOverRunInfo;	///< 오버런 발생유무 확인
	volatile	FLAG_INIT_POSITION	  		FlagInitPositon;
	volatile    FLAG_CID_INFO				FlagCIDInfo;
	volatile    FLAG_CID_NOT_USE			FlagCIDNotuse;
	volatile    UINT						nNextNodeIDInfo;    ///< CID Link 연결 확인을 위해 사용하는 다음정보 1) 초기화 조건, 작업변경 되었을 경우, 새로운 작업지시를 받았을 경우
																											   //2) 갱신 조건  새로운 명령이 주행측에 할당되었을 경우(다음정보가 없을우는 0으로 처리), 대차 or 반납시 위치 갱신(경로삭제한마지막 노드의 다음노드), 작업변경에의해 명령생성시 첫번재 노드의 속한 다음노드
	volatile    FLAG_MTL_INFO				FlagMTLInfo;
	volatile    FLAG_PATHCHANGE				FlagPathChange;
	volatile    bool						bChangeResult;
	volatile    FLAG_CID_MONITORING			FlagCIDMonitoring;
	volatile    FLAG_PRETRANS				FlagPreTrans;
	volatile    double  					dCompensatedPosition;	///< 오차를 보상한 기준 위치( 엔코더값 - 누적 거리 오차값 + 출발 Station Offset )
	volatile	bool						FlagRunningPositon;
//	volatile	bool						FlagPassPermit;
	volatile	UINT						ExecuteDrivingStep;
//	volatile	bool	  	            	FlagCIDOOccupReturnUBG;
	volatile    PATH_DIVERGENCE_INFO 		nCheckDivInfo;			//분기정보  0, 1
	volatile	bool				   		bSpeedDownFlag;

	volatile    EXECUTE_HIDPATH_TYPE		nHIDPathInfo;			//분기정보 , 0 메뉴얼 1 직선 2 곡선

	volatile    int  						nFinishOfNode;	///< 오차를 보상한 기준 위치( 엔코더값 - 누적 거리 오차값 + 출발 Station Offset )
	volatile    int  						nFinishOfStation;	///< 오차를 보상한 기준 위치( 엔코더값 - 누적 거리 오차값 + 출발 Station Offset )

    volatile    bool                        bOcsPause;              // OCS Pause 정보
    volatile    bool                        bTpPause;               // TP Pause 정보
	volatile	bool				   		bJogInterlockIgnore;
	volatile    FLAG_CID_REPORT             FlagCIDReport;
    volatile    FLAG_CMD_EXCUTE				FlagCmdExcute;

	//1 00. 초기화
	EXECUTE_INFO_()
	{
		//2	1. MainControlThread to Execute Thread -------------------------------------
		ExecuteMode			= EXECUTE_MODE_MANUAL;
		ExecuteUnit			= EXECUTE_UNIT_NONE;
		QuickCmd				= EXECUTE_QUICK_CMD_NONE;
		OnAddingCmd.Data		= 0;
		bPopListOfCmd = false;

		//2	2. ExecuteThread to MainControlThread --------------------------------------
		StatusCmdId.Data		= 0;
		StatusCmdId.Info.Status	= STS_INIT;
		ErrCode_ExecuteInfo		= 0x00;

		memset(&ExecutePositionInfo, 0, sizeof(EXECUTE_POSITION_INFO));
		
		OnAddingMapInfo.Data	= 0;
		ResponceType			= EXECUTE_RESPONCE_NONE;

		TransDifference.dHoist	= 320; // Rail 
		TransDifference.dShift	= 0; // Rail

		AccumulationData.dDrivingEncoder = 0.0;
		AccumulationData.dHoistEncoder = 0.0;
		AccumulationData.dShiftEncoder = 0.0;

		FlagInitPositon.bInitFlag    = false;
		FlagInitPositon.dDrivingInitPosition = 0.0;

		FlagOverRunInfo.bCheck = false;
		FlagOverRunInfo.OverrunType = 0x00;
		FlagOverRunInfo.nFinalNode = 0;
		FlagOverRunInfo.dDrivingInitPosition = 0.0;


		FlagCIDInfo.bCIDErrorFlag = false;
		FlagCIDInfo.bMoveRestart = false;

		FlagCIDMonitoring.bCIDFlag =false;
		FlagCIDMonitoring.nStopNodeID =0;
		FlagCIDMonitoring.nResetNodeID =0;
		FlagCIDMonitoring.cResult =0x00;

		FlagPathChange.bPathChange =false; //경로 변경 유무 확인 플래그
		FlagPathChange.nChageNodeID =0; // 경로 변경 노드 위치
		FlagPathChange.cResult= 0x00;		 ///< 경로 변경 결과. //0 초기화, 1 성공, 2 실패
		bChangeResult = false;

		FlagCIDNotuse.bCIDUse =false;
		FlagCIDNotuse.nStopNodeID =0;
		FlagCIDNotuse.nResetNodeID =0;

		FlagPreTrans.bPreShutter = false;
		FlagPreTrans.bPrePIO = false;
		FlagPreTrans.nPIOID = 0;
		FlagPreTrans.nPIOChannel = 0;
		FlagPreTrans.nTargetID = 0;
//		FlagPreTrans.cPIOType = 0x02;   //H-PIO
		FlagPreTrans.cPortType = 0x00;


		FlagMTLInfo.nTakeOut =INIT;
		FlagMTLInfo.nTakeIn =INIT;
		FlagMTLInfo.nRailNum =INIT;
		FlagMTLInfo.nTakeInOrder=INIT;
		FlagMTLInfo.bTakeoutCycle = false;
		FlagMTLInfo.bTakeoutReport = false;
		FlagMTLInfo.bTakeInCycle = false;

		dCompensatedPosition = 0.0;
		nCheckDivInfo = PATH_DIVERGENCE_NONE;

		nFinishOfNode = 0;
        nFinishOfStation = 0;

		nHIDPathInfo = EXECUTE_HIDPATH_NONE;

		FlagRunningPositon = false;
		bSpeedDownFlag = false;

		bJogInterlockIgnore		= true;
        ExecuteDrivingStep = 0; //DRIVING_CTRL_STEP_NONE
//		FlagPassPermit = true;
		nNextNodeIDInfo = 0;

        bOcsPause = false;
		bTpPause = false;

		FlagCIDReport.bCIDInterlock = false;
		FlagCIDReport.bCIDStop = false;

		FlagCmdExcute.bCmdTransCMDExcute = false;
		FlagCmdExcute.bCmdDrivingCMDExcute = false;
//		FlagCIDOOccupReturnUBG = false; //CID 점유반납 By UBG
		//미사용		memset(&FlagMeargePoint, 0, sizeof(FLAG_MERGE_POINT));
	}
		
} EXECUTE_INFO;


#endif	// ExecuteInfoH

