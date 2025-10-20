#ifndef Def_MCCParamH
#define Def_MCCParamH

#include <stdio.h>

/**
@brief	 MCC 파라미터
@author  moon
@date	 2019.11.27
*/
struct MCC_TRANS_PARAM_SET
{
	//TransControl 사용
	double dHoist_TeachingPos;
	double dShift_TeachingPos;
	int    nEqType;
	double dHoist_Acc;
	double dHoist_Dec;
	double dHoist_Vel;
	double dShift_Acc;
	double dShift_Dec;
	double dShift_Vel;

	double dHoist_StartPos;
	double dHoist_EndPos;
	double dHoist_PushOnPos;
	int    nHandPush_RetryCount;
	int    nOscillation_Count;

	DWORD  dwHoistDownTime;
	DWORD  dwHoistUpTime;

	DWORD  dwShutterFrontTime;
	DWORD  dwShutterRearTime;

	int  nShutterFrontTime;
	int  nShutterRearTime;
	bool  bShutterFrontAlarm;
	bool  bShutterRearAlarm;

	double dHoist_HomeOffPos;
	double dHoist_HomeOnPos;
	double dHoist_HomeOnPulse;
    int    nHoist_Home_Haunt;

	double dShift_HomeOffPos;
	double dShift_HomeOnPos;
	double dFOUPCover_OffPos;
	double dFOUPCover_OnPos;
	double dFOUPDetect_OffPos;
	double dFOUPDetect_OnPos;
	double dLeftSTBDetect_OnPos;
	double dLeftSTBDetect_OffPos;
	double dRightSTBDetect_OnPos;
	double dRightSTBDetect_OffPos;

	double dShift_HomeOnSpeed;
	double dShift_HomeOffSpeed;
	double dHoist_HomeOnSpeed;
	double dHoist_HomeOffSpeed;

	bool   bRotate_Moving;

	bool 	bpreRotate0;
	bool 	bpreRotate180;
	int		nRotate0OnDegree;
	int		nRotate0OffDegree;
	int		nRotate180OnDegree;
	int		nRotate180OffDegree;
	int		nRotateMovingTime;

	bool	bpreShiftHomeSensor;
	bool	bpreFOUPCoverSensor;
	bool	bpreFOUPDetectSensor;
	bool	bpreSTBLeftDetectSensor;
	bool	bpreSTBRightDetectSensor;

	bool 	bpreHoistHomeSensor;

	int		nTransStep;

	DWORD	dwBeforePIOTime;
	DWORD	dwAfterPIOTime;

	int    nShift_Off_Haunt;
	int    nShift_On_Haunt;

	int 	nHandAlaramCount;

	int 	nHandInpositionOffCount;
	int 	nHandAlarmOffCount;
	int 	nHandRightGripOffCount;
	int 	nHandLeftGripOffCount;
	int 	nHandCloseOffCount;
	int 	nHandOpenOffCount;
	int 	nHandPIOOffCount;
	bool 	bpreHandInposition;
	bool 	bpreHandAlarm;
	bool 	bpreHandRightGrip;
	bool 	bpreHandLeftGrip;
	bool 	bpreHandClose;
	bool 	bpreHandOpen;
	bool 	bpreHandPIO;

	char	cCarrierType[10];
};

struct MCC_TASK_PARAM_SET
{
	//TaskControl 사용
	DWORD	dwHandOpenOutOnTime;
	DWORD	dwHandOpenOutOffTime;
	DWORD	dwHandCloseOutOnTime;
	DWORD	dwHandCloseOutOffTime;
	DWORD	dwHandInpoInOnTime;
	DWORD	dwHandInpoInOffTime;
	DWORD	dwHandOpenInOnTime;
	DWORD	dwHandOpenInOffTime;
	DWORD	dwHandCloseInOnTime;
	DWORD	dwHandCloseInOffTime;
	DWORD	dwHandLeftGripInOnTime;
	DWORD	dwHandLeftGripInOffTime;
	DWORD	dwHandRightGripInOnTime;
	DWORD	dwHandRightGripInOffTime;
	DWORD	dwHandPushInOnTime;
	DWORD	dwHandPushInOffTime;

	bool bpreHandCloseOut;
	bool bpreHandOpenOut;
	bool bpreHandCloseIn;
	bool bpreHandOpenIn;
	bool bpreHandInposition;
	bool bpreHandLeftGrip;
	bool bpreHandRightGrip;
	bool bpreHandPush;
	bool bTransLoaded;

	bool 	bHandSequenceFlag;
	bool 	bTransMessageFlag;

	int 	nHandPushCount;
	int     nJobO_NAK_Count;
	int     nJobC_NAK_Count;
	int     nJobD_NAK_Count;
	int 	nOCS_Comm;

	int    nEqType;

	char	cCarrierType[10];
};

struct MCC_STATUS_PARAM_SET
{
	//StatusCheckThread 사용
	int		nMem;
	int		nCPU;

	int 	nRSSI;
	unsigned char cClientMAC[18];

	int 	nLogTtime;
	int     nTaskTtime;
	int     nStatusTtime;
	int     nExcuteTtime;
	int     nExtraTtime;
	int     nAMCTtime;

	bool	bAMCcomm;

	int 	nFanFail;

	bool	bpreHandPIO;
	int		nHandPIOCount;

	int 	nUseFDCCount;

	char	cCarrierType[10];
	char	cOHTType[10];

	float  	fAxis1Torque;
	float  	fAxis2Torque;
	float  	fAxis3Torque;
	float  	fAxis4Torque;

	double  dDrivingFollowingError;
	double 	dHoistFollowingError;
	double 	dShiftFollowingError;

    double dDrivingMaxPos;

	int nPathSteering;

	int	nUBGDetectTime_N[9];
	int	nUBGDetectTime_L[9];
	int	nUBGDetectTime_R[9];
	int	nUBGDetectTime_S[9];
	int	nUBGDetectTime_B[9];
	int	nUBGDetectTime_V[9];
	int	nUBGDetectTime_P[9];
};

struct MCC_DRIVING_PARAM_SET
{
	//DrivingControl 사용
	int nFoupCoverDetectCount;
	int nInnerFOUPDetectCount;
	int nInnerFOUPNotDetectCount;
	int nShiftHomeSenOffCount;
	int nHoistHomeSenOffCount;
	int nShutterFrontOpenCount;
	int nShutterFrontCloseCount;
	int nShutterRearOpenCount;
	int nShutterRearCloseCount;

	DWORD  dwSteerFrontTime;
	DWORD  dwSteerRearTime;
	DWORD  dwShutterFrontTime;
	DWORD  dwShutterRearTime;

	int  nSteerFrontTime;
	int  nSteerRearTime;
	int  nShutterFrontTime;
	int  nShutterRearTime;
	bool  bShutterFrontAlarm;
	bool  bShutterRearAlarm;

	bool 	bSteerFrontLeftFlag;
	bool 	bSteerRearLeftFlag;
	bool 	bSteerFrontRightFlag;
	bool 	bSteerRearRightFlag;
	bool    bDetectStatus;
	bool	bMarkStartFlag;
	bool	bBCRReadFail;
	bool	bFrontShutterFlag;
	bool	bRearShutterFlag;
	bool	bAbnormalStopFlag;

	DWORD	dwOBSDetectTime;
	DWORD	dwUBGDetectTime;

	double 	dUBGDetectStopDistance;
	double	dUBGDetectStopPoint;

	DWORD	dwCIDLinkOnTime;
	int 	nCIDLinkOnTime;

	int 	nCIDMapOffset;
	int		nCIDLinkOffset;
	int 	nCIDLinkStartPosition;

	UINT	nCIDLinkNode;
	UINT	nCIDLinkStartNode;
	int		nCIDLinkNodeOffset;

	int 	nPreCount;
	int		nNodeCount;
	int		nBCRTriggerCount;

	UINT	uSourceNode;
	UINT	uTargetNode;
	UINT	uPreNode;
	UINT	uFromStation;

	double 	dMarkStartSpeed;
	double	dMarkStartDist;
	int		nPreCheckSensorHauntingCount;
	bool 	bPrePreCheckSensorDetect;
	int 	nStopSensorHauntingCount;
	bool 	bPreStopSensorDetect;
	int		nFrontSensorHauntingCount;
	int 	nRearSensorHauntingCount;
	bool 	bPreRearSensorDetect;

	int 	nSteerFrontHauntingCount;
	int 	nSteerRearHauntingCount;
	bool	bSteerTimeCheckFlag;
	bool	bPreSteerFTRQ0;
	bool	bPreSteerFTRQ1;
	bool	bPreSteerRTRQ0;
	bool	bPreSteerRTRQ1;
	bool	bPreSteerFDIR;
	bool	bPreSteerRDIR;
	bool	bPreSteerFrontLeft;
	bool	bPreSteerRearLeft;
	bool	bPreSteerFrontRight;
	bool	bPreSteerRearRight;

	DWORD	dwSteerFTRQ0OnTime;
	DWORD	dwSteerFTRQ0OffTime;
	DWORD	dwSteerRTRQ0OnTime;
	DWORD	dwSteerRTRQ0OffTime;
	DWORD	dwSteerFDIROnTime;
	DWORD	dwSteerFTDIROffTime;
	DWORD	dwSteerRDIROnTime;
	DWORD	dwSteerRDIROffTime;
	DWORD	dwSteerFLeftOnTime;
	DWORD	dwSteerFLeftOffTime;
	DWORD	dwSteerFRightOnTime;
	DWORD	dwSteerFRightOffTime;
	DWORD	dwSteerRLeftOnTime;
	DWORD	dwSteerRLeftOffTime;
	DWORD	dwSteerRRightOnTime;
	DWORD	dwSteerRRightOffTime;

	int		nOBSMaxOnSteering;
	int		nOBSMinOnSteering;
	int		nUBGMaxOnSteering;
	int		nUBGMinOnSteering;

	bool	bNodeDetectFlag;
	DWORD	dwNodeDetectDelay;

	char	cSteerFrontStatus;
	char	cSteerRearStatus;

	double	dMarkFirstDetectSpeed;
	double	dMarkSecondDetectSpeed;

	char	cCarrierType[10];

	bool 	bMarkingOHTDetectStop;

	int		nOHTDetectArea;
	int		nOHTDetectLevel;
	int		nOBSDetectArea;
	int		nOBSDetectLevel;
	DWORD	dwOHTDetectAreaTime;
	DWORD	dwOHTDetectLevelTime;
	DWORD	dwOBSDetectAreaTime;
	DWORD	dwOBSDetectLevelTime;
	DWORD	dwOHTDetectLevelFailTime;

	int 	nPathSteering;
	bool	bFStopOnStartPos;

	bool 	bOHTDetectFail2Flag;
	bool 	bLookDownDetect;
	bool	bLookDownError;

	int 	nHandInpositionOffCount;
	int 	nHandAlarmOffCount;
	int 	nHandRightGripOffCount;
	int 	nHandLeftGripOffCount;
	int 	nHandCloseOffCount;
	int 	nHandOpenOffCount;
	int 	nHandPIOOffCount;
	bool 	bpreHandInposition;
	bool 	bpreHandAlarm;
	bool 	bpreHandRightGrip;
	bool 	bpreHandLeftGrip;
	bool 	bpreHandClose;
	bool 	bpreHandOpen;
	bool 	bpreHandPIO;
};


struct MCC_PIO_PARAM_SET
{

	int    nEqType;

	DWORD	dwPIOSelectOnTime;
	DWORD	dwPIOInitTime;
	DWORD	dwHOAVBLOnTime;
	DWORD	dwESOnTime;
	DWORD	dwCS0OnTime;
	DWORD	dwValidOnTime;
	DWORD	dwLReqOnTime;
	DWORD	dwUReqOnTime;
	DWORD	dwTRReqOnTime;
	DWORD	dwReadyOnTime;
	DWORD	dwBusyOnTime;

	DWORD	dwBusyOffTime;
	DWORD	dwTRReqOffTime;
	DWORD	dwCompleteOnTime;
	DWORD	dwReadyOffTime;
	DWORD	dwOutputOffTime;

	UINT	uPIOInput;
	UINT	uPIOOutput;

	int		nPIOStep;

	char	cCarrierType[10];
};

#endif
