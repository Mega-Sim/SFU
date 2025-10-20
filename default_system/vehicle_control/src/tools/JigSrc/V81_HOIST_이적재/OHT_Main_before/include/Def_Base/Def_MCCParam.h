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

	double dHoist_HomeOffPos;
	double dHoist_HomeOnPos;
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
};

struct MCC_DRIVING_PARAM_SET
{
	//DrivingControl 사용
	int nFoupCoverDetectCount;
	int nInnerFOUPDetectCount;
	int nInnerFOUPNotDetectCount;
	int nShiftHomeSenOffCount;
	int nHoistHomeSenOffCount;

	DWORD  dwSteerFrontTime;
	DWORD  dwSteerRearTime;
	DWORD  dwShutterFrontTime;
	DWORD  dwShutterRearTime;

	bool 	bSteerFrontFlag;
	bool 	bSteerRearFlag;
	bool    bDetectStatus;
	bool	bMarkStartFlag;
	bool	bBCRReadFail;
	bool	bFrontShutterFlag;
	bool	bRearShutterFlag;
	bool	bAbnormalStopFlag;

	DWORD	dwOBSDetectTime;
	DWORD	dwUBGDetectTime;

	DWORD	dwCIDLinkOnTime;

	int 	nPreCount;
	int		nNodeCount;
	int		nBCRTriggerCount;

	UINT	uSourceNode;
	UINT	uTargetNode;

	double 	dMarkStartSpeed;
	double	dMarkStartDist;
	int		nFrontSensorHuntingCount;

	int 	nSteerFrontHuntingCount;
	int 	nSteerRearHuntingCount;
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
};

#endif
