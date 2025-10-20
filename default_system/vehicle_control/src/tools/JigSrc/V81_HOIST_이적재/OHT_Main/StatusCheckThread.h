/**
@file    StatusCheckThread.h
@version OHT 7.0
@brief   StatusCheckThread Header File
*/

#ifndef StatusCheckThreadH
#define StatusCheckThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "ThreadCtrl.h"
#include "HWSet.h"
#include "TransControl.h"
#include "OHTMainStatus.h"
#include "ExecuteInfo.h"
#include "CPUandMemInfo.h"
#include "DiagManager.h"

#include "HID_VII_ZR.h"
#include "IPCLoadLib.h"
#include "CPUandMemInfo.h"
#include "NetworkSignallib.h"
#include "OHTSysInfo.h"
#include "UdpClient.h"
#include "Def_DefaultIniParam.h"
#include "Def_MCCParam.h"
#include "SNMP.h"
#include "LCD.h"
#include "Obstacle.h"
#include "Lookdown.h"
//---------------------------------------------------------------------------

typedef enum    SNMP_PARSING_RESULT_
{
    SNMP_NO_ERR = 0,
    SNMP_SEND_ERR,
    SNMP_RCV_ERR,
    SNMP_RSSI_NULL_ERR,
    SNMP_MAC_NULL_ERR,
    SNMP_SSID_NULL_ERR,
    SNMP_TYPE_ERR
}SNMP_PARSING_RESULT;

#define WirelessClientStatusPort 161

class TaskControl;

/**
@class   StatusCheckThread
@date    2012.11.15
@brief   StatusCheckThread 클래스
*/
class StatusCheckThread : public ThreadCtrl 
{
private:

	LogUnit *pLogExtraJobThread;      ///< ExtraJobThread 로그 생성을 위한 변수
	LogUnit *pLogUser; ///< User 저장을 위한 변수 선언
	LogUnit *pLogErrHID;  ///< HID Error 저장을 위한 변수 선언
	LogUnit *pLogCPUandMemInfo; ///< CPUandMemInfo 관련 로그 변수
	LogUnit *pLogWirelessNet; ///< WirelessNet 통신 로그 변수
	LogUnit *pLogMCC;

	FDCUnit *pFDCUnit;

	HWSet *m_pHWSet; ///< HWSet Class 변수

    // by zzang9un 2013.08.18 : Use 변수를 위해 추가
	TransControl *m_pTransControl; ///< TransConrol Class 변수
	TaskControl *m_pTaskControl;

    // by zzang9un 2013.09.02 : Sound 재생 여부를 판단하기 위한 변수
    bool m_bPlayingSound;
	int m_PlaySoundNum;

	UINT nCommfailCount;


	HID_VII_ZR *m_HID;
	IPCLibrary *m_IPCLoadLib;
	NetworkSinalLib *m_NetworkDll;
	CPUandMemInfo *m_CPUandMemInfo;

    PassPermit * m_PassPermit;

	bool m_CheckLampYF;
	bool m_TPESTOP;
	String GetStatusToString(const BYTE status);

    /// OHT Main Status 변수(MainThread와 StatusThread 공유)
    OHTMainStatus *m_OHTMainStatus;

    /// Execute Info 변수(StatusThread와 ExecuteThread 공유)
    EXECUTE_INFO *m_ExecuteInfo;

	DiagManager* m_DiagManager;

	//MONITOR_DATA_INFO *m_MonitorDataArray;  ///< HID 동적 배열 포인터
	//HID_NODE_INFO *m_NODEArray; ///< NOD 동적 배열 포인터

	//void InitSTRCUTInfo();
	//void CloseSTRCUTinfo();
	DEFAULT_PARAM_SET *m_defualtparam;
	MCC_STATUS_PARAM_SET	m_mccparam;
    // Wireless Client IP 주소 (단말별 파라미터 다름)
    char szWirelessClientIP[20];
    UdpClient *m_UdpClient;
	int Get_WLanInfo_FXE_MODEL(char* chAddr, APSIGNAL_DATA* APSignalData);

	//LCD 관련 내용

	LCD *m_pLCD;
	bool bOpenLCD;
	int ComOpenForLCD();

    int m_DistanceSensorType;

	// Cycle Over Check 2018.12.04 SHCHO
	// StatusCheck Thread에서 혹시라도, 행걸리는 부분이 있는지 체크하기 위함
    void CycleOverCheck(int nEventNo, DWORD nChkTime);
	int m_MoniVANode;
	int m_MoniVDAUNode;
	int m_MoniVBNode;
	int m_MoniVDBUNode;
	int m_MoniVCNode;
	int m_MoniVDCUNode;
	int m_MoniAPNode;

	LCD_DATA_FLAG m_lcd_flag;

protected:
	void RealThreadProc(void);	// Execute 와 같은 함수

public:
	StatusCheckThread();
	~StatusCheckThread();

	void UpdateStandardStatus(); //puting

	// Status를 Update하는 함수들


	void UpdateCommonStatus();
	void UpdateStatusForTP();
	void UpdateStatusForMonitoring();
    void UpdateDiagData();

	void ControlLamp();
	void ControlYellowLamp();
	void ControlSound();

	int GetReadLCDData(bool OHTMainOff = false); //OHTMainOff 경우 처리하기 위해 추가

	int CheckHID();

	// by zzang9un 2013.09.13 : User를 위한 로그 생성 함수
	void AddUserLog();

	void CheckTraceLog();

	int SaveHID_Log();

	void SetDistanceSensorType(int cmd);
	int  GetDistanceSensorType();
	void MCCStatusData();

	void SaveThreadTime(DWORD max, char* name);
	void AMCCommfail();
};
//---------------------------------------------------------------------------
#endif
