//---------------------------------------------------------------------------

#ifndef DiagManagerH
#define DiagManagerH

#include <vcl.h>
#include <map>
#include <IniFiles.hpp>
//#include "Def_UDPProtocol.h"
#include "Protocol_Define_Modify.h"
#include "List.h"
//#include "OHT_TLV.h"


#define DIAG_PARAM_FILE_PATH		"\\Datafiles\\DiagParam.ini"

//OHT 통계정보 항목 데이터 정의
#define DIAG_DATA_ID_MOVEDISTANCE				0x10
#define DIAG_DATA_ID_MOVESTEERING				0x20
#define DIAG_DATA_ID_MOVESHIFT					0x30
#define DIAG_DATA_ID_REVSHIFT					0x31
#define DIAG_DATA_ID_MOVEHOIST					0x40
#define DIAG_DATA_ID_REVHOIST					0x41
#define DIAG_DATA_ID_MOVEROTATE					0x50
#define DIAG_DATA_ID_MOVEHAND					0x60
#define DIAG_DATA_ID_MOVESHUTTER				0x70
#define DIAG_DATA_ID_COUNTDRVBCR				0x80
#define DIAG_DATA_ID_COUNTSTABCR				0x81
#define DIAG_DATA_ID_TIMELOOKDOWN				0x82
#define DIAG_DATA_ID_TIMEUBG					0x84
#define DIAG_DATA_ID_TIMEPBS					0x85

//OHT 통계정보 데이터 보고하는 기준값
#define	DIAG_DEFAULT_REF_VALUE_MOVEDISTANCE		100			//km
#define DIAG_DEFAULT_REF_VALUE_MOVESTEERING		5000       	//Count
#define DIAG_DEFAULT_REF_VALUE_MOVESHIFT		2000       	//Count
#define DIAG_DEFAULT_REF_VALUE_REVSHIFT			2 	     	//1만
#define DIAG_DEFAULT_REF_VALUE_MOVEHOIST		2000		//Count
#define DIAG_DEFAULT_REF_VALUE_REVHOIST			10      	//1만
#define DIAG_DEFAULT_REF_VALUE_MOVEROTATE		2000        //Count
#define DIAG_DEFAULT_REF_VALUE_MOVEHAND			2000        //Count
#define DIAG_DEFAULT_REF_VALUE_MOVESHUTTER		2000        //Count
#define DIAG_DEFAULT_REF_VALUE_COUNTDRVBCR		100        //Count
#define DIAG_DEFAULT_REF_VALUE_COUNTSTABCR		100        //Count
#define DIAG_DEFAULT_REF_VALUE_TIMELOOKDOWN		24      	//hours
#define DIAG_DEFAULT_REF_VALUE_TIMEUBG			24      	//hours
#define DIAG_DEFAULT_REF_VALUE_TIMEPBS			24      	//hours

//OHT 통계정보 데이터 단위
#define DIAG_REF_VALUE_UNIT_MOVEDISTANCE		1000000		// 1,000,000 mm = 1 km
#define DIAG_REF_VALUE_UNIT_MOVESTEERING		1
#define DIAG_REF_VALUE_UNIT_MOVESHIFT			1
#define DIAG_REF_VALUE_UNIT_REVSHIFT			10000		// 10,000 Revolution
#define DIAG_REF_VALUE_UNIT_MOVEHOIST			1
#define DIAG_REF_VALUE_UNIT_REVHOIST			10000		// 10,000 Revolution
#define DIAG_REF_VALUE_UNIT_MOVEROTATE		   	1
#define DIAG_REF_VALUE_UNIT_MOVEHAND			1
#define DIAG_REF_VALUE_UNIT_MOVESHUTTER			1
#define DIAG_REF_VALUE_UNIT_COUNTDRVBCR			10000       // 10,000 Count
#define DIAG_REF_VALUE_UNIT_COUNTSTABCR			10000		// 10,000 Count
#define DIAG_REF_VALUE_UNIT_TIMELOOKDOWN		3600000		//3,600,000 msec = 1 hour
#define DIAG_REF_VALUE_UNIT_TIMEUBG				3600000     //3,600,000 msec = 1 hour
#define DIAG_REF_VALUE_UNIT_TIMEPBS				3600000		//3,600,000 msec = 1 hour

//OHT 이상정보 데이터 정의
#define DIAG_ABNORMAL_DATA_ID_USING_MEMORY			0x10
#define DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_HAND	0x20
#define DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_SHUTTER	0x21
#define DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_STEER	0x22
#define DIAG_ABNORMAL_DATA_ID_ALARM_RESET_HAND		0x23
#define DIAG_ABNORMAL_DATA_ID_OUT_OF_PHASE_PBS		0x30
#define DIAG_ABNORMAL_DATA_ID_OUT_OF_PHASE_UBG		0x31
#define DIAG_ABNORMAL_DATA_ID_RETRY_BCR				0x32
#define DIAG_ABNORMAL_DATA_ID_DIFFERENCE_ENCODER	0x40

//OHT 이상정보 데이터 보고 기준값
#define DIAG_ABNORMAL_REF_VALUE_MEMORY_USING_RATE			80		//Percent
#define DIAG_ABNORMAL_REF_VALUE_MEMORY_COUNT				5		//Count
#define DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_HAND			1500	//msec
#define DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_SHUTTER		2000	//msec
#define DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_STEERING		5000	//msec
#define DIAG_ABNORMAL_REF_VALUE_ALARM_RESET_HAND			1		//Count
#define DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_PBS			1		//Count
#define DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_UBG			1       //Count
#define DIAG_ABNORMAL_REF_VALUE_RETRY_BCR					1       //Count
#define DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_CNT		3       //Count
#define DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_LIMIT	5		//Percent

//OHT Status 기준 값
#define DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_C_DRIVE		75      //Percent
#define DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_D_DRIVE		75      //Percent

//기타 파라메터
#define PBS_OUT_OF_RANGE_TIMEOUT	1500	//PBS탈조발생시 Abnormal로 처리하는 Timeout 시간

//OHT Status 관련
#define DIAG_OHT_STATUS_NG	0
#define DIAG_OHT_STATUS_OK	1
#define DIAG_SEND_INTERVAL	24

//기타
#define DIAG_MAX_SIZE_ABNORMAL_DATA_LIST	300
#define DIAG_MAX_SIZE_PATROL_DATA_LIST		300
#define DIAG_CHECK_INIT_STAT_DATA_INTERVAL	10*1000			//10 sec
#define DIAG_CHECK_TIMER_RETRY_COUNT		3600*1000		//1 hour

//Error Code
#define	DIAG_ACK_OK					0x00
#define	DIAG_ACK_ERR_NO_DATA	    0x01
#define DIAG_ACK_DATA_LOGIC			0x02
#define	DIAG_ACK_ERR_UNKNOWN        0x09
#define DIAG_NO_ACK					0xFF

/**
@brief   진단 Agent 전송용 Patrol 데이터
@author  by doori.shin
@date    2015.11.30
*/
typedef struct PATROL_DATA_
{
	WORD wLitzDetect	: 2;
	WORD wLitzTemp		: 2;
	WORD wRailMisMatchR	: 2;
	WORD wRailMisMatchL	: 2;

	WORD wReserved02	: 2;
	WORD wParticleDetect: 1;
	WORD wRailSlope		: 1;
	WORD wReserved01	: 1;
	WORD wSlopeMode		: 1;
	WORD wVibrationMode	: 1;
	WORD wCleanMode		: 1;
}PATROL_DATA;

/**
@brief   PatrolVHL이 진단 Agent로 데이터 전송 Protocol
@author  by doori.shin
@date    2015.11.30
*/
typedef struct SERVICE_DATA_DIAG_
{
	UINT nStartNode;
	UINT nEndNode;

	PATROL_DATA PatrolData;

	float fVibrationRMS;

}SERVICE_DATA_DIAG;

/**
@brief   진단 Agent에서 데이터 요청하는 프로토콜
@author  by doori.shin
@date    2015.11.30
*/
typedef struct DIAG_REQ_DATA_
{
	BYTE bDataID;
}DIAG_REQ_DATA;

/**
@brief   진단 Agent 송수신용 프로토콜
@author  by doori.shin
@date    2015.11.30
*/
typedef struct DIAG_OHT_STATISTICS_
{
	BYTE 	bInfoID;
	UINT64	nData;
}DIAG_OHT_STATISTICS;

/**
@brief   진단 Agent 에게 상태보고용 프로토콜
@author  by doori.shin
@date    2015.11.30
*/
typedef struct RET_DATA_DIAG_
{
	BYTE bInfoID;
	BYTE bResult;
}RET_DATA_DIAG;

/**
@brief   진단 Agent 에게 OHT상태정보 구조체(1일 1회)
@author  by doori.shin
@date    2015.11.30
*/
typedef struct DIAG_OHT_STATUS_DATA_
{
	BYTE bIsLogSave;			//USB에 로그 저장하는지 여부
	BYTE bIsEnabledUSB;			//USB가 정상으로 인식되는지 여부
	BYTE bIsEnabledFAN;			//FAN이 정상동작 하는지 여부
	BYTE bIsAvailableCDrive;	//C Drive에 여유공간이 있는지 확인
	BYTE bIsAvailableDDrive;	//D Drive에 여유공간이 있는지 확인
}DIAG_OHT_STATUS_DATA;

/**
@brief   진단 Agent 에게 OHT이상 발생시 보고하는 프로토콜
@author  by doori.shin
@date    2015.11.30
*/
typedef struct DIAG_OHT_ABNORMAL_
{
	BYTE bDataType;	//이상이 발생한 데이터 타입
	UINT nNodeID;	//이상이 발생한 노드
}DIAG_OHT_ABNORMAL;

/**
@brief   진단 Agent로부터 DB에 저장된 통계값 받는 구조체
@author  by doori.shin
@date    2015.11.30
*/
typedef struct OHT_STATISTICS_DATA_FROM_DIAG_
{
	UINT64 nMovingDistance;
	UINT64 nMoveCountSteering;
	UINT64 nMoveCountShift;
	UINT64 nMoveRevShift;
	UINT64 nMoveCountHoist;
	UINT64 nMoveRevHoist;
	UINT64 nMoveCountRotate;
	UINT64 nMoveCountHand;
	UINT64 nMoveCountShutter;
	UINT64 nMoveCountDrvBCR;
	UINT64 nMoveCountStaBCR;
	UINT64 nMoveTimeLookdown;
	UINT64 nMoveTimeUBG;
	UINT64 nMoveTimePBS;
}OHT_STATISTICS_DATA_FROM_DIAG;

/**
@brief   진단보고용 통계정보 구조체
@author  by doori.shin
@date    2015.11.30
*/
class DIAG_OHT_STATISTICS_DATA
{
private:
    CRITICAL_SECTION	m_DataCS;

public:
	BYTE bID;						//통계 항목 ID
	UINT nReferenceValue;			//보고되는 기준값
	UINT nUnit;						//보고되는 단위
	double dTotalData;				//누적 데이터
	double dPreSendData;			//누적데이터 보고시기를 알기 위한 다음 누적데이터

public:
	DIAG_OHT_STATISTICS_DATA(BYTE bID, UINT nUnit, UINT nRefValue)
	{
		InitializeCriticalSection(&m_DataCS);
		this->bID = bID;
		this->nUnit = nUnit;
		this->nReferenceValue = nRefValue;
		this->dTotalData = 0.0f;
		this->dPreSendData = 0.0f;
	}

	~DIAG_OHT_STATISTICS_DATA()
	{
		DeleteCriticalSection(&m_DataCS);
	}

	//TotalData 초기화
	void ClearData()
	{
		EnterCriticalSection(&m_DataCS);
		this->dTotalData = 0.0f;
		this->dPreSendData = 0.0f;
		LeaveCriticalSection(&m_DataCS);
	}

	//진단서버로 보낼 데이터 생성
	DIAG_OHT_STATISTICS* MakeSendData()
	{
		DIAG_OHT_STATISTICS* pStatData = new DIAG_OHT_STATISTICS;
		double dInteger, dFraction = 0.f;

		memset(pStatData, 0, sizeof(DIAG_OHT_STATISTICS));

		pStatData->bInfoID = this->bID;	//ID입력
		dFraction = modf(this->dTotalData, &dInteger); //정수부, 소수부 구분

		pStatData->nData = dInteger;

		return pStatData;
	}

	//TotalData에 값을 누적함
	void UpdateData(double nValue)
	{
		if(this->nUnit != 0 && nValue > 0)
		{
			EnterCriticalSection(&m_DataCS);
			this->dTotalData += (double)nValue / (double)(this->nUnit);
			LeaveCriticalSection(&m_DataCS);
		}
	}

	//누적값이 보고기준을 초과했는지 확인
	bool IsReadyToSend()
	{
		bool bResult = false;

		if(this->nReferenceValue != 0)
		{
			bResult = (this->dTotalData >= ((double)this->dPreSendData + (double)this->nReferenceValue));
		}

		return bResult;
	}
};

//통계정보 보고용 Map 자료구조 type definition
typedef std::map<BYTE, DIAG_OHT_STATISTICS_DATA*> 	DIAG_STAT_DATA_MAP;
typedef DIAG_STAT_DATA_MAP::iterator		DIAG_STAT_DATA_MAP_ITER;
typedef std::pair<BYTE, DIAG_OHT_STATISTICS_DATA*> DIAG_STAT_DATA_PAIR;

/**
@brief   DiagManager용 Parameter 저장 구조체
@author  by doori.shin
@date    2015.11.30
*/
typedef struct DIAG_PARAM_
{
	//OHT Name
	char Version[8];

	//VHL 통계 기준값
	int nRefStatMoveDistance;
	int nRefStatMoveSteering;
	int nRefStatMoveShift;
	int nRefStatRevShift;
	int nRefStatMoveHoist;
	int nRefStatRevHoist;
	int nRefStatMoveRotate;
	int nRefStatMoveHand;
	int nRefStatMoveShutter;
	int nRefStatCountDrvBCR;
	int nRefStatCountStaBCR;
	int nRefStatTimeLookdown;
	int nRefStatTimeUBG;
	int nRefStatTimePBS;

	//VHL 통계 기준값 단위
	int nStatUnitMoveDistance;
	int nStatUnitMoveSteering;
	int nStatUnitMoveShift;
	int nStatUnitRevShift;
	int nStatUnitMoveHoist;
	int nStatUnitRevHoist;
	int nStatUnitMoveRotate;
	int nStatUnitMoveHand;
	int nStatUnitMoveShutter;
	int nStatUnitCountDrvBCR;
	int nStatUnitCountStaBCR;
	int nStatUnitTimeLookdown;
	int nStatUnitTimeUBG;
	int nStatUnitTimePBS;


	//VHL Abnormal 기준값
	int nRefAbnormUsingMemoryRate;
	int nRefAbnormUsingMemoryCount;
	int nRefAbnormTimeoverMoveHand;
	int nRefAbnormTimeoverMoveShutter;
	int nRefAbnormTimeoverMoveSteering;
	int nRefAbnormAlarmResetHand;
	int nRefAbnormOutofPhasePBS;
	int nRefAbnormOutofPhaseUBG;
	int nRefAbnormRetryBCR;
	int nRefAbnormDifferenceEncoderCount;
	int nRefAbnormDifferenceEncoderLimit;

	//VHL Status 기준값
	int nRefStatusAvailableSpaceC;
	int nRefStatusAvailableSpaceD;

	//PBS탈조 chattering
	int nPBSOutOfRangeTimeout;

}DIAG_PARAM;


//AOHC-196 사전 로그 추가
// 2019.03 SHCHO * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// 사전로그, 추가할때 변경해야할 부분
// 아래 enum PREVIOS_DIAG_PARAM_NUM_ 에서 인덱스 추가하고
// 그 아래 GetPrevIdxName(..) 함수에서 해당 인덱스 이름 지어주면 됩니다.
// 다른 부분은 수정할 필요 없습니다. 적절한 시점에 Increase 함수만 호출하면 됩니다.
#define PREVIOUS_LOG_WRITE_INTERVAL_MIN		   	60        // 사전로그 남기는 시간 간격(단위 : 분)
typedef enum PREVIOS_DIAG_PARAM_NUM_
{
	PREVIOS_DIAG_HO					= 0,		//AG일떄, Off -> On될때, Count++
	PREVIOS_DIAG_IF					   ,		//AG일떄  Foup이 없을떄, Off -> On될때, Count++      AG일떄  Foup이 있을때, On -> Off될때, Count++
	PREVIOS_DIAG_IF_B				   ,        //AG일떄  Foup이 없을떄, Off -> On될때, Count++      AG일떄  Foup이 있을때, On -> Off될때, Count++
	PREVIOS_DIAG_FC					   ,		//AG일떄  Foup이 있을때, On -> Off될때, Count++
	PREVIOS_DIAG_SH					   ,		//AG일떄, ON-> Off될때, Count++
	PREVIOS_DIAG_OC					   ,		//이적재 중 Ocillation Pause 걸린 횟수 + 채터링 감지 이내에 걸린 건 횟수(Pause 걸리기전)
	PREVIOS_DIAG_HS					   ,		 //1) Hand Close/Open 동시에  On   //3) Hand Open  상황에서  Gripper On일때    4) Hand Go 시그널 off일때
	PREVIOS_DIAG_HS_B				   ,	       //1) Hand Close/Open 동시에  On   //3) Hand Open  상황에서  Gripper On일때    4) Hand Go 시그널 off일때
	PREVIOS_DIAG_HS_C				   ,           //1) Hand Close/Open 동시에  On   //3) Hand Open  상황에서  Gripper On일때    4) Hand Go 시그널 off일때
	PREVIOS_DIAG_TR					   ,		//최초 Front 만나고나서 이동 중 front 센서가 Off -> On이되는 경우에 Count++
	PREVIOS_DIAG_VD					   ,		//5V : 4.9이상에서 4.9 이하로 떨어지는 시점에  Count++ (5.0→ 4.8 O, 4.8→ 4.8 X)       12V : 11.64기준 300V : 240V 기준
	PREVIOS_DIAG_VD_B				   ,          //5V : 4.9이상에서 4.9 이하로 떨어지는 시점에  Count++ (5.0→ 4.8 O, 4.8→ 4.8 X)
	PREVIOS_DIAG_VD_C				   ,          //5V : 4.9이상에서 4.9 이하로 떨어지는 시점에  Count++ (5.0→ 4.8 O, 4.8→ 4.8 X)
	PREVIOS_DIAG_PS					   ,		//직선 구간 주행 中 대차 센서는 감지 되지 않았지만 장애물 센서만 감지되는 경우
	PREVIOS_DIAG_CF     			   ,		 //CID 시작 offset 거리 + 맵 속도 기반 200ms 기준 거리 + Tag margin 100(common normal 동일값 적용)
	PREVIOS_DIAG_COMM   			   ,					//OCS 통신 연결 이후, 통신 딜레이가 발생한 최대 시간 기록
	PREVIOS_DIAG_OT     			   ,                    //OCS 통신 연결 이후, 10초 단위로 OCS 통신 응답이 없을 경우에 카운트 증가
	PREVIOS_DIAG_DBT     			   ,                    //DrivingBCR Time out(오버런) 발생 횟수에 대한 횟수 추가
	PREVIOS_DIAG_TBT                   ,                     //TransBCR Time out(오버런) 발생 횟수에 대한 횟수 추가
	PREVIOS_DIAG_UC     			   ,    //soming         //UBG 직선 명령에 따른 순차적 레벨 동작하지 않는 경우에 카운트 증가 soming 2레벨 이상 튈떄
	PREVIOS_DIAG_UD     			   ,    //soming        //UBG Direction UBG가 틀어져서 옆레일을 보고 있을떄. 직선구간에서 OBS감지 없이 UBG만 3~6레벨 감
	PREVIOS_DIAG_UF     			   ,    //soming        //UBG Fail. OBS는 들어오고 UBG 안들어옴. 직진구간에서

	//P2 항목  2019 02 18 puting
	PREVIOS_DIAG_CPU 	   				,					//일정 CPU 점유율 이상(40%) 올라갈 경우 카운트 증가
	PREVIOS_DIAG_MEM 	   				,               	//일정 메모리 점유율 이상(50%) 올라갈 경우 카운트 증가
	PREVIOS_DIAG_HANDT_MAX 	   			,               	 //Hand Close/Open의 최대 동작 시간
	PREVIOS_DIAG_SHUT_MAX 	  			,               	  //Shutter Close/open의 최대 동작 시간
	PREVIOS_DIAG_STEERLT_MAX 			  	,           	 //조향 Front Left/Right의 최대 동작 시간
	PREVIOS_DIAG_STEERRT_MAX 			   	,           	 //조향 Rear Left/Right의 최대 동작 시간
	PREVIOS_DIAG_MO_FRGAP 	   			,               	 //전륜과 후륜의 차이의 최대 엔코더값
	PREVIOS_DIAG_FOLOWF_MAX 			,               	 //전륜 최대 Following Error 최대 차이값
	PREVIOS_DIAG_MAP_DGAP 	   			,                 // 주행 Link간 실제와 Map상의 차이값 100mm(??) 이상일 경우 카운트 증가
	PREVIOS_DIAG_E84TM 	   				,               //  E84Timeout 발생시 카운트 증가
	PREVIOS_DIAG_EMPTY	   				,                // 공출하 발생시 카운트 증가
	PREVIOS_DIAG_DOUBLE 	   			,               // 이중반입 발생시 카운트 증가
	PREVIOS_DIAG_HAND_ALAM 	   			,               //Hand Alaram 시 Reset시 카운트 증가
	PREVIOS_DIAG_HAND_ORIGIN			,                //Hand Origin 수행 시 카운트 증가
	PREVIOS_DIAG_PPO_RETYE 	   			,  // 호출되는 곳 없음   	//Shutter Alaram시 리트리이시 카운트 증가
	PREVIOS_DIAG_SHU_ALAM 	   			,               	//통과개방 Retry 요청시 일정 횟수를 넘어서 요청할 경우 카운트 증가
	PREVIOS_DIAG_CID_RETYE 	   			,               	//CID 주행 재개리트리이시 카운트 증가
	PREVIOS_DIAG_CID_ERROR_ST 			,               	//CID 이상 보고시 카운트 증가
	PREVIOS_DIAG_JOBO_NAK 	   			,               	//  작업 지시시 Nack일 경우 카운트 증가
	PREVIOS_DIAG_JOBC_NAK 	   			,               //  작업 변경시 Nack일 경우 카운트 증가
	PREVIOS_DIAG_JOBD_NAK       		,               	//   작업 취소시 Nack일 경우 카운트 증가
	PREVIOS_DIAG_VD_U   				,                //5V : 5.2이상으로 떨어지는 시점에  Count++ (5.0→ 4.8 O, 4.8→ 4.8 X)       12V : 12.36기준 300V : 355V 기준
	PREVIOS_DIAG_VD_B_U   				,                //5V : 4.9이상에서 4.9 이하로 떨어지는 시점에  Count++ (5.0→ 4.8 O, 4.8→ 4.8 X)
	PREVIOS_DIAG_VD_C_U         		,                //5V : 4.9이상에서 4.9 이하로 떨어지는 시점에  Count++ (5.0→ 4.8 O, 4.8→ 4.8 X)
	PREVIOS_DIAG_UF_2					,				//UBG Fail. UBG가 사양에 없는 IO가 출력될경우 처리.On On off
	PREVIOS_DIAG_OF_2					,				//OBG Fail. OBG가 사양에 없는 IO가 출력될경우 처리.On On off
	PREVIOS_DIAG_UBG_PAT                ,               //선정된 Node에서 정지 패턴인 경우 UBG 감지 영역 로깅 확인 위해 카운트
	PREVIOS_DIAG_AP                     ,               //AP 감도<4 모니터링 로깅



	//HJH : Teaching Auto Adjustment
	PREVIOS_DIAG_LOAD_TEACHING_ABNORMAL_CNT		,
	PREVIOS_DIAG_UNLOAD_TEACHING_ABNORMAL_CNT 	,

	PREVIOS_DIAG_HP      ,                                   // Hoist Pause Count
	PREVIOS_DIAG_LP     ,                                    // LookDown Pause Count
	PREVIOS_DIAG_HANDPIOP,                                   //Hand Pio Pause Count
	// 반드시 마지막 인덱스
	COUNTOF_PREVIOUS


}PREVIOS_DIAG_PARAM_NUM;

// * * * * 위에 enum 과 쌍이 반드시 맞아야 합니다.
bool GetPrevIdxName(PREVIOS_DIAG_PARAM_NUM idx, char *buf)
{
	if(buf == NULL)		return false;
	
	switch(idx)
	{
		case PREVIOS_DIAG_HO			:		strcpy(buf, "HO");	break;
		case PREVIOS_DIAG_IF			:		strcpy(buf, "IA");	break;
		case PREVIOS_DIAG_IF_B			:		strcpy(buf, "IB");	break;
		case PREVIOS_DIAG_FC			:		strcpy(buf, "FC");	break;
		case PREVIOS_DIAG_SH			:		strcpy(buf, "SH");	break;
		case PREVIOS_DIAG_OC			:		strcpy(buf, "OC");	break;
		case PREVIOS_DIAG_HS			:		strcpy(buf, "HA");	break;
		case PREVIOS_DIAG_HS_B			:		strcpy(buf, "HB");	break;
		case PREVIOS_DIAG_HS_C			:		strcpy(buf, "HC");	break;
		case PREVIOS_DIAG_TR			:		strcpy(buf, "TR");	break;
		case PREVIOS_DIAG_VD			:		strcpy(buf, "VA");	break;
		case PREVIOS_DIAG_VD_B			:		strcpy(buf, "VB");	break;
		case PREVIOS_DIAG_VD_C			:		strcpy(buf, "VC");	break;
		case PREVIOS_DIAG_PS			:		strcpy(buf, "PS");	break;
		case PREVIOS_DIAG_CF     		:		strcpy(buf, "CF");	break;
		case PREVIOS_DIAG_COMM   		:		strcpy(buf, "CM");	break;
		case PREVIOS_DIAG_OT     		:		strcpy(buf, "OT");	break;
		case PREVIOS_DIAG_DBT     		:		strcpy(buf, "DBT");	break;
		case PREVIOS_DIAG_TBT           :       strcpy(buf, "TBT");	break;
		case PREVIOS_DIAG_UC     		:		strcpy(buf, "UC");	break;
		case PREVIOS_DIAG_UD     		:		strcpy(buf, "UD");	break;
		case PREVIOS_DIAG_UF     		:		strcpy(buf, "UF");	break;
		case PREVIOS_DIAG_CPU 	   		:		strcpy(buf, "CPU");	break;
		case PREVIOS_DIAG_MEM 	   		:		strcpy(buf, "MEM");	break;
		case PREVIOS_DIAG_HANDT_MAX 	:		strcpy(buf, "HAN_MAX");	break;
		case PREVIOS_DIAG_SHUT_MAX 	  	:		strcpy(buf, "SHU_MAX");	break;
		case PREVIOS_DIAG_STEERLT_MAX 	:		strcpy(buf, "STL_MAX");	break;
		case PREVIOS_DIAG_STEERRT_MAX 	:		strcpy(buf, "STR_MAX");	break;
		case PREVIOS_DIAG_MO_FRGAP 	   	:		strcpy(buf, "MO_FRGAP");	break;
		case PREVIOS_DIAG_FOLOWF_MAX 	:		strcpy(buf, "FOLOW_MAX");	break;
		case PREVIOS_DIAG_MAP_DGAP 	   	:		strcpy(buf, "MAP_DGAP");	break;
		case PREVIOS_DIAG_E84TM 	   	:		strcpy(buf, "E84TM");	break;
		case PREVIOS_DIAG_EMPTY	   		:		strcpy(buf, "EMPTY");	break;
		case PREVIOS_DIAG_DOUBLE 	   	:		strcpy(buf, "DOUBLE");	break;
		case PREVIOS_DIAG_HAND_ALAM 	:		strcpy(buf, "HAN_ALAM");	break;
		case PREVIOS_DIAG_HAND_ORIGIN   :		strcpy(buf, "HAN_ORI");	break;
		case PREVIOS_DIAG_PPO_RETYE 	:		strcpy(buf, "PPO_RET");	break;		// 기존에 빠져있었음
		case PREVIOS_DIAG_SHU_ALAM 	   	:		strcpy(buf, "SHU_ALAM");	break;
		case PREVIOS_DIAG_CID_RETYE 	:		strcpy(buf, "CID_RETYE");	break;
		case PREVIOS_DIAG_CID_ERROR_ST 	:		strcpy(buf, "CID_ERROR_ST");	break;
		case PREVIOS_DIAG_JOBO_NAK 	   	:		strcpy(buf, "JOBO_NAK");	break;
		case PREVIOS_DIAG_JOBC_NAK 	   	:		strcpy(buf, "JOBC_NAK");	break;
		case PREVIOS_DIAG_JOBD_NAK      :		strcpy(buf, "JOBD_NAK");	break;
		case PREVIOS_DIAG_VD_U   		:		strcpy(buf, "VD_A_U");	break;
		case PREVIOS_DIAG_VD_B_U   		:		strcpy(buf, "VD_B_U");	break;
		case PREVIOS_DIAG_VD_C_U        :		strcpy(buf, "VD_C_U");	break;
		case PREVIOS_DIAG_UF_2			:		strcpy(buf, "UF_2");	break;
		case PREVIOS_DIAG_OF_2			:		strcpy(buf, "OF_2");	break;
		case PREVIOS_DIAG_UBG_PAT       :       strcpy(buf, "UBG_PAT");	break;
		case PREVIOS_DIAG_AP            :       strcpy(buf, "AP");	break;
		case PREVIOS_DIAG_LOAD_TEACHING_ABNORMAL_CNT		: 	strcpy(buf, "LN");		break;
		case PREVIOS_DIAG_UNLOAD_TEACHING_ABNORMAL_CNT		:   strcpy(buf, "UN");      break;
		case PREVIOS_DIAG_HP            :       strcpy(buf, "HP");	break;
		case PREVIOS_DIAG_LP            :       strcpy(buf, "LP");	break;
		case PREVIOS_DIAG_HANDPIOP      :       strcpy(buf, "HPIO");	break;


		default: return false;
	}
	return true;
	
}


/**
@brief   진단모니터링 항목 수집 및 전송을 담당하는 클래스
@author  by doori.shin
@date    2015.11.30
*/
class DiagManager
{
private:
	static DiagManager* pInstance;
	DiagManager();
	~DiagManager();

	LogUnit *pLogPreviosDiag; ///< IOT 통신 로그 변수
	CRITICAL_SECTION	m_CSPreDiag;		///< Buffer의 Count를 읽고 쓸 때

	DIAG_STAT_DATA_MAP* m_DiagDataMap;		//VHL 공통 진단데이터를 담고있는 자료구조
	LList* m_DiagAbnormalDataList;			//진단항목 중 Abnormal 감지된 데이터들 저장되는 자료구조
	LList* m_DiagPatrolInfoList; 			//Patrol진단 데이터 저장하는 리스트
	SERVICE_DATA_DIAG m_DiagServiceData;	//Patrol이벤트 기록하는 변수
	DIAG_OHT_STATUS_DATA m_DiagStatusData;	//OHT Status 정보 기록하는 변수

	int m_nDiagAbnormalDrvDiffCnt;			//Abnormal 항목중 Driving Encoder 차이발생 횟수 저장하는 변수
	int m_nDiagAbnormalHandResetCount;		//
	int m_nDiagCheckCount;					//24시간 구동 체크 변수

	DIAG_PARAM m_DiagParam;

	// 구조체에서 배열로 변경. 사전로그 개수 늘어남에 따라
	int m_nArrDiagVal[COUNTOF_PREVIOUS];	//PREVIOS_DIAG_PARAM m_PreviosDiagParam; //사전 진단 로그 puting 20180510
	DWORD m_dwPrevLogStartTick;

	int m_nVHLWorkingTime;	//VHL 가동시간, 단위 : Hour (UBG, PBS, Lookdown 센서보고용)

	bool m_bIsLoadedDiagData;	//최초 실행시 진단서버로 부터 통계 데이터를 받았는지 여부
	UINT m_nDiagCheckTimerID;	//Timer ID
	UINT m_nTimerRetryCount;	//Timer Retry 횟수

	//Status 관련 함수들
	bool CheckDiskStatus(DISK_INFO* pInfo);
	void ClearDiagStatusData();

	void MakeStatDataMap();
public:
	static DiagManager* getInstance();
	static void DestroyInstance();	//shcho 2018.08.06
	bool IsDiagStatLoaded();



	//Socket 관련
//	AnsiString GetDiagIpAddr();

	//패킷생성

	//Param관련
	bool LoadDiagParam(char* szFilePath, DIAG_PARAM* pParamSet);  //진단 파라메터 읽기
	DIAG_PARAM GetDiagParam();

	bool LoadDiagParamRecorvery(char* szFilePath, DIAG_PARAM* pParamSet);  //진단 파라메터 읽기
	int m_ParamChehckError;
    bool saveParameter(char* szFilePath, DIAG_PARAM* pParamSet);

	//통계정보 관련 Map 제어 함수들
	int UpdateStatDiagData(BYTE bID, double nValue);
	int UpdateStatReferenceValue(BYTE bID, int nRefValue);
	int ClearStatDiagData(BYTE bID);
	bool CheckStatDataToSend(BYTE bID);
	double GetStatDiagData(BYTE bID);
	DIAG_STAT_DATA_MAP* GetDiagStatMap();

	bool UpdateStatDiagAllData(DIAG_PKT_BODY_DATA_INFO_RESPONSE RcvData);

	//Abnormal 정보 관련 Map 제어 함수들
	void InsertAbnormalDiagData(BYTE bDataType);
	LList* GetAbnormalDiagDataList();

	//Abnormal 정보중 Cnt값을 갖는 항목들
	int GetAbnormalDrvDiffCount();
	void IncreaseAbnormalDrvDiffCount();
	void ResetAbnormalDrvDiffCount();

	int GetAbnormalHandResetCount();
	void IncreaseAbnormalHandResetCount();
	void ResetAbnormalHandResetCount();

	int GetDiagCheckCount();
	void IncreaseDiagCheckCount();
	void ResetDiagCheckCount();

	//OHTStatus 관련 함수들
	void CheckOHTStatus();
    DIAG_OHT_STATUS_DATA* GetDiagOHTstatus();

	//OHT 가동시간 확인
	void IncreaseWorkingTime();
	int GetWorkingTime(bool bInit);

	//진단소켓관련 함수들
//	UDP_ACK_DATA ParsingDiagCmd(char* pRecvBuf);


	//사전 진단 로그 관련 함수
	void LogginPreviosDiagCount();
	void IncreasePreviosDiagCount(PREVIOS_DIAG_PARAM_NUM nType);	// 해당항목 개수 1증가 / AOHC-196 사전 로그 추가
	void IncreasePreviosDiagMax(PREVIOS_DIAG_PARAM_NUM nType, int dwTime);	// 해당항목 최대값 저장 / AOHC-196 사전 로그 추가
	void ResetPreviosDiagCount();  //Arrived 일때 호출할것.

	// 사전로그  값 리턴 / AOHC-196 사전 로그 추가
	int 	GetPreviosValue(PREVIOS_DIAG_PARAM_NUM type);	// 해당항목 값 리턴

	//Timer 관련 함수들
	void SetDiagStatCheckTimer();
	void IncreaseRetryCount();
	int GetRetryCount();
	int GetCheckTimerID();
	void Reset_DiagManager();
	bool PrevLogTimeChk();			// 로그 기록 시간 체크
};

DiagManager* DiagManager::pInstance = NULL;

//---------------------------------------------------------------------------
#endif
