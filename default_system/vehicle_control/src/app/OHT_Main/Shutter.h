/**
@file    Shutter.h
@version OHT 7.0
@brief   Shutter Class Header File
*/

#ifndef ShutterH
#define ShutterH

#include <System.hpp>
#include "HWCommon.h"
#include "Hoist.h"
#include "Shift.h"
#include "Utility.h"

#include "LogHandler.h"
#include "OHTMainStatus.h"
#include "ExecuteInfo.h"

#define MATERIALTYPE_FOUP 0x00
#define MATERIALTYPE_RSP150 0x01
#define MATERIALTYPE_RSP200 0x02
#define MATERIALTYPE_PODEUV 0x03

/**
@class   Shutter
@date    2012.11.01
@author  LSJ
@brief   Shutter 관련 Class
*/
enum MaskPodType
{
	None	=	-1,
	EUVPod	=	1,
	RSP150	=	2,
	RSP200	=	3
};

class Shutter
{
private:
    HWCommon *m_pHWCommon;

	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;

    bool bLongRun;
    int m_nMovingCount;
    DWORD dwMaxMovingTime;
    bool bReadyOperation;
    int nSendCommand;
    DWORD dwStartTime;
    DWORD dwOriginLimitTime;
	DWORD dwMotionLimitTime;

	CRITICAL_SECTION m_MovCntCS;

	LogUnit *pLogShutter;
	FDCUnit *pFDCUnit;


	int nAutoRecoveryClose;
	DWORD dwAutoRecoveryTime;

#if(OHT_NAME == OHT_NAME_P4_MASK)
	Hoist *m_pHoistC;
	Shift *m_pShiftC;
#endif

public:
    Shutter(HWCommon *pHWCommon);
    ~Shutter();

    int CheckComplete();

    void Init();
    int IO_Init();
    int Open();
	int Close();
	int Close_Classifying_Material();
	int Mid_Close();
	
	int Open_Cmd();
//	int Close_Cmd();                    //미사용
	int SendOpenCmd();
	int SendCloseCmd();
	int SendCloseCmd_Classifying_Material();
	int Origin();
	int AlarmReset();
	int Enable();
	int Free();
	bool IsOpen(int nFrontRear);
	bool IsClose(int nFrontRear);
//	bool IsCmdOpen(int nFrontRear);                 //미사용
//	bool IsCmdClose(int nFrontRear);                //미사용
	bool IsInposition(int nFrontRear);
	bool IsAlarm(int nFrontRear);
	bool IsFree(int nFrontRear);
    int CheckSensor(int nShutterPos);
    bool IsLongRun();
//    void StartLongRun();                          //미사용
//	void StopLongRun();                             //미사용
	int GetMovingCount(bool bInit);
//	DWORD GetMaxMovingTime();                       //미사용
	int ReadyToOperation();
    /// @todo 상위 단에서 해당 내용의 함수 구현 필요
	int AutoInitialize();

	int Open_AutoRecovery();
	int Close_AutoRecovery();
//	void ResetMovementData();                      //미사용

//	int CheckEUVSensor();                          //미사용


#if(OHT_NAME == OHT_NAME_P4_MASK)
	/// mgchoi 2022-08-18. P4 MaskOHT용 추가함수 ///
	int CheckSensor(int nShutterPos, int mode);
	int CheckPodType();					// 3종류 POD 종류 체크
	bool IsPodPlacement1On();
	bool IsPodPlacement2On();
	bool IsPodPlacement3On();
	bool IsPodPlacement4On();
	////////////////////////////////////////////////
#endif

	void SetMaterialType(BYTE set);
	BYTE GetMaterialType();
	BYTE m_nSetMaterialType;
	void SetAutoRecoveryClose(int bFlag);
	int GetAutoRecoveryClose();
	void SetAutoRecoveryStartTime();
	DWORD GetAutoRecoveryStartTime();

};

#endif  // ShutterH
