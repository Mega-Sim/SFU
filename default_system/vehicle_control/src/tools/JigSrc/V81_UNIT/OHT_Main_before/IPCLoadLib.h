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

#define MAX_LENGTH_OF_FULL_PATH		1024

#define ERR_IPCLibrary_READY	  	901
#define MAX_SYSLEN    16



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
	INITIALIPCDLL DeviceSusiDLLInit;
	UNINITIALIPCDLL DeviceSusiDLLUninit;
	CHECKIPCMONITORDRIVER DeviceHWMAvailable;
	GETIPCTEMPERATURE DeviceHWMGetTemp;
	GETIPCVOLTAGE DeviceHWMGetVolt;


	bool m_bIPCReady;     ///< IPC »ç¿ë °¡´ÉÇÑ »óÅÂ

public:


	bool IsIPCReady();
	//int CheckIPCTemp();
	// ¿Âµµ, Àü¾Ð : IPC »ç¾ç¿¡ ´Ù¶ó ´Þ¶óÁü: SOM, (PCM9377, PCM9371)   : DLL
	int GetIPCData(IPC_DATA_INFO* m_IPCData);

	// 2018.10.11 SHCHO : IPC의 온도, 전압정보를 String으로 전달한다.
	bool GetInfo_String(char *szOut, int nSize);





	// Disk: Windows   : ÇÔ¼ö
   //	int Get_IsLogDriveAlive(char*);

};

#endif
