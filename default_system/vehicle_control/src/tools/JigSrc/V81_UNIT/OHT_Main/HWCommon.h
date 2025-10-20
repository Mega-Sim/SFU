/**
@file    HWCommon.h
@version OHT 7.0
@brief   HWCommon Class Header File
*/

#ifndef HWCommonH
#define HWCommonH

#include "Def_HW.h"
#include "Def_OHT.h"
#include "AMCDllLib.h"
#include "MultiSerialLib.h"

#include "HwMask.h"		// HW 정보 Masking 을 위한 class


// -------------------------------------------
// [BEGIN] by zzang9un 2013.08.08 : HW에서 사용하는 상수
#define TRANS_SIDE			0 ///< Trans BCR, QR
#define DRIVING_SIDE		1 ///< Driving BCR, QR
#define LEFT_SIDE			0x01 ///< EQ_PIO, QR
#define RIGHT_SIDE			0x02 ///< EQ_PIO, QR
// [END] by zzang9un 2013.08.08 : HW에서 사용하는 상수
// -------------------------------------------

/**
@class   HWCommon
@date
@author  ehoto
@brief   HWSet에서 공통으로 사용될 Class
@brief   IO, Motion(모터), Serial(멀티시리얼)
@brief   AMC DLL Load
*/

class HWCommon
{
private:
	AMCLibrary *m_pMotionController;
    MultiSerialLibrary *m_pMultiSerialLib;
	HwMask	*m_pMask;


	int m_nAmcErrCode; //AOHC-216 AMC CommFail, CRC 에러명 수정
public:
//	Serial *m_pSerial[14];

	/// I/O 관련 변수 ----------------------------------
#if(OHT_NAME == OHT_NAME_STD_V80)
	sINPUTR01 m_Input;
	sOUTPUTR01 m_Output;
#elif(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
	sRETICLEINPUT m_Input;
	sRETICLEOUTPUT m_Output;
#elif(OHT_NAME == OHT_NAME_STD_V85)
	sSTANDARDINPUT_V85 m_Input;
	sSTANDARDOUTPUT_V85 m_Output;
#elif(OHT_NAME == OHT_NAME_STD_V81)
	sINPUTV81 m_Input;
	sOUTPUTV81 m_Output;
#elif(OHT_NAME == OHT_NAME_STD_V81S)
	sSTANDARDINPUT_V81S m_Input;
	sSTANDARDOUTPUT_V81S m_Output;
#else
	sINPUT	m_Input;
	sOUTPUT	m_Output;
#endif

	/// 생성자 & 소멸자 --------------------------------
	HWCommon();
	~HWCommon();
	
	// HW Open, Close
	int		Open();
	int		Close();

	/// I/O 관련 함수 ----------------------------------
	int		SetBit(int nIndex, bool bOnOff, bool bOBSControl = false);

	/// Motion 관려 함수 -------------------------------
	/// 초기화
	int		Init(int nAxisNum);
	int		SetSync(int nMasterAxisNum, int nSlaveAxisNum, double dSyncGain);
	int		SetMaxSWLimitForReset(int nAxisNum, double dPosLimit, double dNegLimit);

	bool 	IsAMCDllOpen();  //AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현

	/// 상태 확인
	bool	IsRunning(int nAxisNum);
	bool	IsInPosition(int nAxisNum);
	bool	IsAMPEnable(int nAxisNum);
	double	GetCurrentPosition(int nAxisNum);
	double	GetPrePosition(int nAxisNum);
	double	GetTotalPosition(int nAxisNum);
	void	ResetTotalPosition(int nAxisNum);
	double	GetFinalPosition(int nAxisNum);
	double	GetCurrentVelocity(int nAxisNum);
	double	GetFollowingError(int nAxisNum);
	double	GetSmallAddCheckPosition(int nAxisNum);
	int 	GetTotalMoveCount(int nAxisNum);
	void 	ResetTotalMoveCount(int nAxisNum);

	/// 에러 관련
	int		GetError(int nAxisNum);
	int		Reset(int nAxisNum);

	/// Offset 관련
	int		SetHome(int nAxisNum);
	int		SetOffset(int nAxisNum, double dDistance);
	int		GetOffset(int nAxisNum, double *dDistance);

	/// Limit 관련
	int		SetPositiveSWLimit(int nAxisNum, double dLimit);
	int		SetNegativeSWLimit(int nAxisNum, double dLimit);
	int		GetSWLimit(int nAxisNum, double& dPosLimit, double& dNegLimit);
	int		ResetSWLimit(int nAxisNum);

	/// 구동 관련 - 단위: 거리(mm), 속도(mm/msec), 가속도(mm/msec2)
	int		SetAMPEnable(int nAxisNum, bool bValue);
	// 가속도 기반
	int		MovePositive(int nAxisNum, double dAccel, double dVel);
	int		MoveNegative(int nAxisNum, double dAccel, double dVel);
	int		MoveStop(int nAxisNum, double dAccel);
	int		MoveDistance(int nAxisNum, double dAccel_Up, double dAccel_Down, double dVel, double dDistance, TRIGGER_CTRL_INFO TriggerCtrlInfo);
	// 가속 시간 기반
	int		MovePositive(int nAxisNum, DWORD dwTime, double dVel);
	int		MoveNegative(int nAxisNum, DWORD dwTime, double dVel);
	int		MoveStop(int nAxisNum, DWORD dwTime);
	int		MoveDistance(int nAxisNum, DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance);


	// 통신으로 제어시에서 구현 시 사용 함수
	int		SendData(int nAxisNum, char* pSendData, int nSendLen, char* pRecvData, int* nRecvLen);	///< Data 전송

	void	UpdateData(void);	///<HWCommon의 MotionController 의 정보 Update --> HWCtrl Thread 에서 사용
	int		GetIOSet(int* pInputBuff, int& nInputCount, int* pOutputBuff, int& nOutCount);

	void	ClearNodeCountInfo(void);
	int		GetNodeCountInfo(NODE_CHECK_DATA &NodeCheckData);

#if(USE_CHANGENODE == 1)
	int		SetDetectInfo(DETECT_INFO DetectInfo, OPTIONAL_NODE_INFO OptNodeInfo);
#else
	int		SetDetectInfo(DETECT_INFO DetectInfo);
#endif
	int		CompensateSlavePos();
	int		SetPauseControl(int bIsPause, int nCommander);


	void	SetTorqueLimit();
	void	SetTorqueLimitRelease();

	void	SetSlopeTime();
	void	SetSlopeTimeRelease();

	void	SetTargetPos();
	void	SetTargetPosRelease();

	void	SetFirstNodeCountAdd();
	void	SetVHLDetectIgnor();
	void	SetVHLDetectIgnorRelease();

	int		GetFollowing_Axis1();

	void	SetDetectStopDecelDown();
	void	SetDetectStopDecelDownRelease();
	int     GetAMCDetectStopDecelDownStatus();

	void	SetVHLDetectIOSet(bool bIoSet);
	int     GetVHLDetectForceIOStatus();

	int GetVolatageMonitor(int Index);
	void	makeTraceLog();

	// HW 정보를 Masking 할 때 사용하는 함수 
	HwMask* GetMask(void);

    // by zzang9un 2013.10.07 : AMC 버전 요청 함수
	int GetAMCVersion();
    int GetFPGAVersion();

	int GetAMCPauseStatus();
	int GetAMCTorqueStatus();
	int GetAMCVHLDetectStatus();

	int GetAMCStopStatus();

    int GetAMCDecelStatus();

	// by ji7412.lee 2014.02.10 : AMC Library 추가 함수(AMCupdate)
	int AMCFileSystemForamt(void);
	int ClearWatchDogStatus();
	int DisableWatchDog();
	int AMCFileSystemInstall(char *pszBootLoader, char *pszBootFile, int *prtn, HWND hWnd, UINT uiMsg);
	int SetAMCDriverOpenByCommon(bool isOnOff);
	int SetAMCFileDownloaded(bool isOnOff);
	int DSP_Reboot(void);

	bool IsSerialOpen();  //AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현
/*	int Init(int nAxis);
	bool IsRunning(int nAxis);
	bool IsServoOn(int nAxis);
	bool IsError(int nAxis);
	int ClearAMPFault(int nAxis);
	int ClearEvent(int nAxis);
	int SetEncoderOffset(int nAxis);
	int SetServoOnOff(int nAxis);
	bool IsAMPEnable(int nAxis);
	int SetAMPEnable(int nAxis);
	int MovePositive(int nAxis);
	int MoveNegative(int nAxis);
	int MoveStop(int nAxis);
	int MoveDistance(int nAxis);

	//int MoveTimePositive(int nAxis);
	//int MoveTimeNegative(int nAxis);
	//int MoveTimeStop(int nAxis);
	//int MoveTimeDistance(int nAxis);

	int SetSWLimit(int nAxis);
	int GetSWLimit(int nAxis);
	int SetPositiveSWLimit(int nAxis);
	int SetNegativeSWLimit(int nAxis);
	int ResetSWLimit(int nAxis);
	double GetFollowingError(int nAxis);
	double GetCurrentPosition(int nAxis);
	double GetCurrentVelocity(int nAxis);
*/
	// AMC DLL 관련 함수 다 있음(축 빼고)
	void Set_Jig(int n, bool bUse);

	// Axis 무시 함수
	void Axis_Ignore(bool bUse);

};

#endif  //HWCommonH
