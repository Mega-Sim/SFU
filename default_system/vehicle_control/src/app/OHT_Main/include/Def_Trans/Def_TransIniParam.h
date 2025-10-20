#ifndef Def_TransIniParamH
#define Def_TransIniParamH

#include <stdio.h>
#include "AxisParam.h"		// PROPOTINAL_AXIS_PARAM, THREE_LINEAR_EQUATION_AXIS_PARAM
#include "Def_Variables.h"

#define CARRIER_OFFSET_MAX_COUNT	4
#define DEFAULT_SPECIAL_EQ_PARAM 30 //P1L default 반영

/**
@brief	이종 반송물에 따른 Ofsset 처리를 위한 변수
@author  ehoto
@date	 2013.08.05
*/
typedef struct CARRIER_OFFSET_
{
	double dHosit;
	double dShift;
}CARRIER_OFFSET;

/**
@brief	 Reticle 개별 Port에 대한 Ofsset 처리를 위한 변수
@author  ehoto
@date	 2013.08.05
*/
typedef struct
{
	double dPortOffset_Shift;
	double dPortOffset_Hoist;
	int    nPortOffset_Rotate;
}RETICLE_TEACH_OFFSET;

typedef struct TRANS_PARAM_
{
   double dOffset_MoveUp_SpdnOffset;  ///< Hoist Up 시 저속구간 Offset
   double dOffset_MoveDn_up_SpdnOffset;  ///< Hoist Down 시 시작 저속구간 Offset
   double dOffset_MoveDn_dn_SpdnOffset;  ///< Hoist Down 시 도착 저속구간 Offset
   
   int nTrans_Time;  ///< Trans 시 Timeover 시간
   int nLookdownSen_Detect_Time;  ///< Look Down 센서 Detect Timeover 시간
   int nOscillationCheckDist;   ///< 
   int nTransSound;  ///< 이적재 Sound On, Off

   int nTransStep;  ///< 이적재 단계
   int nTransType;  ///< 이적재 타입(Load 명령인지, Unload 명령인지)
   int nEqType;  ///< 각 설비의 포트 타입
   int nSTBDirection; //STB 방향 (0:EQ,STK 1:RIGHT 2:LEFT)
   int nPIOTimeoverLevel;  ///< 포트 별 EQ PIO Timeover 레벨

   DWORD dwLREQ_STTime;  ///< Load REQ 시작 시간
   DWORD dwLookdn_STTime;  ///< LookDown 센서 감지 시작 시간
   DWORD dwTotalTrans_STTime; ///< 총 이적재 시간
   
   double dHoist_TeachingPos;  ///< Hoist 티칭 값
   double dShift_TeachingPos;  ///< Shift 티칭 값
   double dRotate_TeachingPos;  ///< Rotate 티칭 값
   int nRotate_Pos; //(사용 X)
   
   double dCmdHoistPos;  ///< 명령 수행 시 Hoist 명령 위치(Hoist에 실제 명령을 준 값)
   double dCmdHoistStartPos;  ///< 명령 수행 시 Hoist 시작 위치
   double dCmdHoistSpeed;  ///< 명령 수행 시 Hoist 명령 속도
   double dCmdFinal_HoistPos; ///< 실제 Hoist의 티칭 명령 값
   double dCmdShiftPos;  ///< 명령 수행 시 Shift 명령 위치
   double dCmdRotatePos;  ///< 명령 수행 시 Rotate 명령 위치

   double dHoistLevel;  ///< Hoist 축 속도 레벨 설정
   double dShiftLevel;  ///< Shift 축 속도 레벨 설정

   int nErrCount_GripAbNormalON;  ///< Gripper Sensor 가 비정상적으로 On 되어있을 때 확인 횟수
   int nErrCount_GripAbNormalOFF;  ///< Gripper Sensor 가 비정상적으로 Off 되어있을 때 확인 횟수

   bool bIsSTBTypeTrans;  ///< STB 로의 이적재
   bool bIsSTKTypeTrans;  ///< STK 로의 이적재
}TRANS_PARAM;


typedef struct TRANS_USE_PART_
{
    bool EQPIO;            ///< EQ PIO 사용 유무
    bool GripperSen;       ///< Gripper Sensor 사용 유무
	bool OscillationSen;   ///< Ocillation Sensor 사용 유무
    bool HandPushSen;      ///< HandPush Sensor 사용 유무
    bool LookdownSen;      ///< Look Down Sensor 감지 확인 유무
    bool ShutterControl;   ///< Shutter 제어 가능 여부
    bool HandSensor;       ///< Hand Sensor 사용 유무
    bool HoistHomeSen;       ///< Hoist Align Sensor 사용 유무
    bool STBSen;           ///< STB Sensor 사용 유무
    bool UTBSen;           ///< UTB Sensor 사용 유무
    bool UTBFoupCheck;     ///< UTB Unload시에 티칭Point까지 이동해서 Foup을 체크 여부
    bool IgnorePIOError;   ///< PIO Error 무시 여부
    bool TransSound;       ///< 이적재 Sound 사용 유무
    bool ObsDetectSound;   ///< 장애물 감지 Sound 사용 유무
	bool OhtDetectSound;   ///< 대차 감지 Sound 사용 유무
//	bool bLookdown_Detected;//??
	bool DrivingSound;       ///< 주행 Sound 사용 유무
	int  EQPIOType;
	bool OscillationSenPause;
	bool TransEQSound;         ///< 이적재 포트별 Sound 사용 유무
	bool TransSTKSound;        ///< 이적재 포트별 Sound 사용 유무
	bool TransSTBSound;        ///< 이적재 포트별 Sound 사용 유무
#if(OHT_NAME == OHT_NAME_P4_MASK)	//mgchoi. 2022.08.23. P4 MaskOHT용
	bool HandLoadUnlPositionSen;
#endif
	bool bDualStageStbPIO; // 2단 STB를 위한 PIO 사용 유무
	bool bPstbPIO;			///<PSTB용 PIO 사용 유무
}TRANS_USE_PART;



typedef struct TRANS_DEFINE_
{
#if(OHT_NAME == OHT_NAME_P4_MASK)
	int nTracker_Delay_Time;  ///< Tracker 지연 시간

	double dHoist_Positive_Limit_OnWait;  ///< 대기 중 Hoist 축의 Positive Limit
	double dHoist_Negative_Limit_OnWait;  ///< 대기 중 Hoist 축의 Negative Limit
	double dShift_Positive_Limit_OnWait;  ///< 대기 중 Shift 축의 Positive Limit
	double dShift_Negative_Limit_OnWait;  ///< 대기 중 Shift 축의 Negative Limit
	double dHoist_Abnormal_Move_Speed_Limit;  ///< Hoist 축의 이동 속도 Limit
	double dShift_Abnormal_Move_Speed_Limit;  ///< Shift 축의 이동 속도 Limit

	double dHoist_MoveLimit_P;  ///< Hoist 축의 Positive 이동 Limit
	double dHoist_MoveLimit_N;  ///< Hoist 축의 Negative 이동 Limit
	double dShift_MoveLimit_P;  ///< Shift 축의 Positive 이동 Limit
	double dShift_MoveLimit_N;  ///< Shift 축의 Negative 이동 Limit
	double dDist_Hoist_Home_Margin;  ///< Hoist 축의 원점 여유 거리
	double dDist_Shift_Home_Margin;  ///< Shift 축의 원점 여유 거리

	int nUpstate_SpeedDown_Offset;  ///< Hoist Up 시 저속구간 Offset
	int nDnState_UP_SpeedDown_Offset;  ///< Hoist Down 시 시작 저속구간 Offset
	int nDnState_DN_SpeedDown_Offset;  ///< Hoist Down 시 도착 저속구간 Offset

	int nUpstate_SpeedDown_Offset_Load;  ///< Hoist Up 시 저속구간 Offset
	int nDnState_UP_SpeedDown_Offset_Load;  ///< Hoist Down 시 시작 저속구간 Offset
	int nDnState_DN_SpeedDown_Offset_Load;  ///< Hoist Down 시 도착 저속구간 Offset

	float dHoist_SmallAdd_Dist;
	float dHoist_SmallAdd_Dist_UTB; 	//2025-01-20. mgchoi [AMOHTV80F-1644]
	float dHoist_SmallAdd_Dist_For_UnloadPosition;
	float dHoist_SmallAdd_Antidrop_DownDist;

	float dHoist_SmallAdd_Antidrop_DownDist_RSP150;
	float dHoist_SmallAdd_Antidrop_DownDist_RSP200;
	float dHoist_SmallAdd_Antidrop_DownDist_EUV;

#endif
   int nDnState_UP_SpeedDown_Offset_SpecialEQ;  ///< Hoist Up 시 시작 저속구간 Offset
   int nDnState_UP_SpeedDown_Percent_SpecialEQ;  ///< Hoist Up 시 시작 저속구간 Offset  0~100

   int nLookdn_Detect_Timeover;  ///< Look Down 센서 Detect Timeover 시간
   double dLookdn_Detect_Check_Dist;  ///< Look Down 센서 Detect Check 거리
   int nTrans_Timeover;  ///< Trans 시 Timeover 시간
   double dOscillation_Check_Dist;  ///<
//   double dHoist_MovInpos_Limit;  ///< ??

   double dShift_Fast_Move_Dist;  ///< Shift 축 고속 이동 거리
   double dOscillation_Ignore_Dist;  ///< Oscillation 센서 체크 무시 거리
   double dFast_Move_Limit;  ///< Hoist 축의 고속 이동 구간 Limit
   double dFast_Move_Limit_Ver85h;

   int nSTBUTBSensorOFFTime;  ///< STB, UTB Sensor 가 Off 되는데 걸리는 시간
   int nSTBUTBSensorONTime;  ///< STB, UTB Sensor 가 Off 되는데 걸리는 시간
   int nRotateManualMoveTimeOver;
//   int nRailsupport_Homing_TimeOut;  ///< Rail support 가 Homing 시 중지시간?

//   double dShift_Speed_Ratio;  ///< Shift 의 속도 비

//   double dShift_AccTime;  ///< Shift 축 가속 시간

//   double dHoist_Max_Speed;  ///< Hoist 축 최고 속도
//
//   double dHoist_Pos_Limit;  ///< Hoist 축 Positive Software Limit
//   double dHoist_Neg_Limit;  ///< Hoist 축 Negative Software Limit
//   double dShift_Pos_Limit;  ///< Shift 축 Positive Software Limit
//   double dShift_Neg_Limit;  ///< Shift 축 Negative Software Limit

//   double dInpos_Limit;
//   double dShift_Pos_Limit_ForWait;  ///< 대기 중 Shift 축의 Positive Limit
//   double dShift_Neg_Limit_ForWait;  ///< 대기 중 Shift 축의 Negative Limit
//   double dHoist_Pos_Limit_TransOffset;  ///< 이적재 시 Hoist Limit 설정 Offset
//
//   double dHoist_Origin_Limit;  ///< Hoist 원점 Limit

// 2016-11-28, Add by Labeler, Rotate 동작 Timeout 변수 추가
//   int nRotate_Exec_Timeout;
//   int nDnState_DN_SpeedDown_Speed_Load;  ///< Hoist Down 시 도착 저속구간 속도 조절(0 ~ 100%)

//   double dShift_Speed_Ratio_50Down_Load;
//   double dShift_Speed_Ratio_50Up_Load;
//   double dShift_Speed_Ratio_50Down_UnLoad;
//   double dShift_Speed_Ratio_50Up_UnLoad;

//   double dHandPushRetryPos;


//	int nHoistMoveUp_HomeOffset_Load;  ///< Hoist Up 시 도착 Offset
//	int nHoistMoveUp_TeachingOffset_Load;  ///< Hoist Up 시 시작 저속구간 Offset
//	int nHoistMoveDown_TeachingOffset_Load;  ///< Hoist Down 시 도착 저속구간 Offset

//	int nHoistMoveUp_HomeOffset_Unload;  ///< Hoist Up 시 도착 Offset
//	int nHoistMoveUp_TeachingOffset_Unload;  ///< Hoist Up 시 시작 저속구간 Offset
//	int nHoistMoveDown_TeachingOffset_Unload;  ///< Hoist Down 시 도착 저속구간 Offset

//	int nHoistMoveUp_HomeOffset_STB_Load;  ///< Hoist Up 시 도착 Offset_STB
//	int nHoistMoveUp_TeachingOffset_STB_Load;  ///< Hoist Up 시 시작 저속구간 Offset_STB
//	int nHoistMoveDown_TeachingOffset_STB_Load;  ///< Hoist Down 시 도착 저속구간 Offset_STB

//	int nHoistMoveUp_HomeOffset_STB_Unload;  ///< Hoist Up 시 도착 Offset_STB
//	int nHoistMoveUp_TeachingOffset_STB_Unload;  ///< Hoist Up 시 시작 저속구간 Offset_STB
//	int nHoistMoveDown_TeachingOffset_STB_Unload;  ///< Hoist Down 시 도착 저속구간 Offset_STB

}TRANS_DEFINE;

//AOHC-294
typedef struct TRANS_PROFILEDETAIL_
{
	//기존 프로파일
	int nProfile1_DN_SpeedDown_Offset_Load;  	 	// Hoist Down 시 도착    		저속구간 Offset
	int nProfile1_DN_TeachingSpeedRate_Load;  	 	// Hoist Down 시 도착    		저속구간 Velocity
	int nProfile1_DN_SpeedDown_Offset_Unload; 	 	// Hoist Down 시 도착    		저속구간 Offset
	int nProfile1_DN_TeachingSpeedRate_Unload;	 	// Hoist Down 시 도착    		저속구간 Velocity
	int nProfile1_UP_SpeedDown_Offset_Unload;	 	// Hoist Up   시 티칭위치에서   저속구간 Offset
	int nProfile1_UP_TeachingSpeedRate_Unload;      // Hoist Up   시 티칭위치에서   저속구간 Velocity
	int nProfile1_UP_SpeedDown_Home_Offset_Unload;  // Hoist Up   시 홈위치에서     저속구간 Offset

	//개선 프로파일
	int nProfile2_HMD_TeachingOffset_Load;         // Hoist Down 시 도착    		저속구간 Offset
	int nProfile2_HMD_TeachingSpeedRate_Load;      // Hoist Down 시 도착    		저속구간 Velocity
	int nProfile2_HMD_TeachingOffset_Unload;       // Hoist Down 시 도착    		저속구간 Offset
	int nProfile2_HMD_TeachingSpeedRate_Unload;    // Hoist Down 시 도착    		저속구간 Velocity
	int nProfile2_HMU_TeachingOffset_Unload;       // Hoist Up   시 티칭 위치에서   저속구간 Offset
	int nProfile2_HMU_TeachingSpeedRate_Unload;    // Hoist Up   시 티칭 위치에서   저속구간 Velocity
	int nProfile2_HMU_TeachingOffset_Home_Unload;  // Hoist Up   시 홈 위치에서     저속구간 Offset

	//STB 프로파일
	int nProfile2_HMD_TeachingOffset_STB_Load;
	int nProfile2_HMD_TeachingOffset_STB_Unload;


}TRANS_PROFILEDETAIL;

/**
@brief	 주행 전방감지 파라미터
@author  ehoto
@date	 2013.07.08
*/
struct TRANS_PARAM_SET
{
	THREE_LINEAR_EQUATION_AXIS_PARAM	HoistAxis;			///< 호이스트축 기준 정보
	PROPOTINAL_AXIS_PARAM				ShiftAxis;			///< 시프트축 기준 정보
	
	TRANS_PARAM		TransParam;
	TRANS_USE_PART	TransUsePart;
	TRANS_DEFINE	TransDefine;

//	CARRIER_OFFSET	CarrierOffset[CARRIER_OFFSET_MAX_COUNT];	///< 반송물에 따른 Offset; 우선 FOUP이 0, MAC이 1로 사용 --> 불편사항 추후 보완

//	int		CarrierType;
	char	Version[8];
};



#endif
