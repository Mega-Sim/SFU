// ---------------------------------------------------------------------------
#ifndef IPCLoadLibH
#define IPCLoadLibH
// ---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <mmsystem.h>
#include <string.h>
#include <dos.h>
//#include <algorithm>    // std::equal
#include <iostream>

//#include "IPCEXE.h"   //test

#include "ErrorCode.h"
#include "Def_HW.h"
#include "../Common/Def_DataType.h"

#include "Def_DefaultIniParam.h"
#include "LogHandler.h"

#define MAX_LENGTH_OF_FULL_PATH		1024

#define ERR_IPCLibrary_READY	  	901
#define MAX_SYSLEN    16

#define EZUNE_DAEMON_FILE_NAME L"SemesTempMonitor.exe"
#define SUSI_STATUS_NOT_INITIALIZED	0xFFFFFFFF
#define SUSI_STATUS_INITIALIZED		0xFFFFFFFE
#define SUSI_STATUS_SUCCESS			0


#define SUSI_ID_HWM_TEMP_CPU 		0x00020000
#define SUSI_ID_HWM_TEMP_CHIPSET	0x00020001
#define SUSI_ID_HWM_TEMP_SYSTEM	 	0x00020002
#define SUSI_ID_HWM_TEMP_CPU2 		0x00020003
#define SUSI_ID_HWM_TEMP_OEM0 		0x00020004
#define SUSI_ID_HWM_TEMP_OEM1 		0x00020005
#define SUSI_ID_HWM_TEMP_OEM2 		0x00020006
#define SUSI_ID_HWM_TEMP_OEM3 		0x00020007
#define SUSI_ID_HWM_TEMP_OEM4 		0x00020008
#define SUSI_ID_HWM_TEMP_OEM5 		0x00020009
#define SUSI_ID_HWM_VOLTAGE_VCORE 	0x00021000
#define SUSI_ID_HWM_VOLTAGE_VCORE2 	0x00021001
#define SUSI_ID_HWM_VOLTAGE_2V5 	0x00021002
#define SUSI_ID_HWM_VOLTAGE_3V3 	0x00021003
#define SUSI_ID_HWM_VOLTAGE_5V 		0x00021004
#define SUSI_ID_HWM_VOLTAGE_12V 	0x00021005
#define SUSI_ID_HWM_VOLTAGE_5VSB 	0x00021006
#define SUSI_ID_HWM_VOLTAGE_3VSB	0x00021007
#define SUSI_ID_HWM_VOLTAGE_VBAT	0x00021008
#define SUSI_ID_HWM_VOLTAGE_5NV		0x00021009
#define SUSI_ID_HWM_VOLTAGE_12NV	0x0002100A
#define SUSI_ID_HWM_VOLTAGE_VTT		0x0002100B
#define SUSI_ID_HWM_VOLTAGE_24V		0x0002100C
#define SUSI_ID_HWM_VOLTAGE_OEM0	0x00021010
#define SUSI_ID_HWM_VOLTAGE_OEM1	0x00021011
#define SUSI_ID_HWM_VOLTAGE_OEM2	0x00021012
#define SUSI_ID_HWM_VOLTAGE_OEM3	0x00021013

/**
@brief   IPCLibrary Å¬·¡½º
@author  puting
@date    2013.03.28
*/
class IPCLibrary {
private:
	typedef BOOL(_stdcall*INITIALIPCDLL)();
	typedef BOOL(_stdcall*UNINITIALIPCDLL)();
	typedef int (_stdcall*CHECKIPCMONITORDRIVER)();
	typedef BOOL(_stdcall*GETIPCTEMPERATURE)(WORD tempType, float *retval,WORD * typeSupport = NULL);
	typedef BOOL(_stdcall*GETIPCVOLTAGE)(DWORD voltType, float *retval, WORD * typeSupport = NULL);

	typedef uint32_t(_stdcall*INITALIPCDLL6869)();
	typedef uint32_t(_stdcall*UNINITIALIPCDLL6869)();
	typedef uint32_t (_stdcall*GETBOARDVALUE6869)(uint32_t Id, uint32_t *pValue);
     DEFAULT_PARAM_SET *m_defualtparam;
   //	CRITICAL_SECTION m_CS;

	HINSTANCE IPC_OpenDLL();

	HINSTANCE m_hIpcLib;

	IPC_DATA_INFO m_IPCData;
	bool m_bDllLoaded; ///< IPC DLL Load Á¤»ó ¿Ï·á

	void IPC_Init(void);

public:
	IPCLibrary();
	~IPCLibrary();

private:

	INITALIPCDLL6869 DeviceSusiDLL6869Init;
	UNINITIALIPCDLL6869 DeviceSusiDLL6869Uninit;
	GETBOARDVALUE6869 DeviceGetBoard6869Value;

	INITIALIPCDLL DeviceSusiDLLInit;
	UNINITIALIPCDLL DeviceSusiDLLUninit;
	CHECKIPCMONITORDRIVER DeviceHWMAvailable;
	GETIPCTEMPERATURE DeviceHWMGetTemp;
	GETIPCVOLTAGE DeviceHWMGetVolt;


	bool m_bIPCReady;     ///< IPC »ç¿ë °¡´ÉÇÑ »óÅÂ
    bool m_bEZUNEReady;
public:


	bool IsIPCReady();
	//int CheckIPCTemp();
	// ¿Âµµ, Àü¾Ð : IPC »ç¾ç¿¡ ´Ù¶ó ´Þ¶óÁü: SOM, (PCM9377, PCM9371)   : DLL
	int EZUNE_Run();
	int GetIPCData(IPC_DATA_INFO* m_IPCData);
	int GetEZUNEMonitoringData(IPC_DATA_INFO* IPCData);
	int GetEZUNEData(DWORD *temp);
	// 2018.10.11 SHCHO : IPC의 온도, 전압정보를 String으로 전달한다.
	bool GetInfo_String(char *szOut, int nSize);


	// Disk: Windows   : ÇÔ¼ö
   //	int Get_IsLogDriveAlive(char*);

};

#endif
