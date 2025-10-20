//---------------------------------------------------------------------------

#ifndef MonitoringUnitH
#define MonitoringUnitH

#include <windows.h>
#include <stdio.h>
#include <dos.h>
#include <vcl.h>
#include <TlHelp32.h>
#include <shlwapi.h>

#include "Def_DataType.h"
#include "LIniFile.h"
#include "OHTMainStatus.h"


#define MAX_LENGTH_OF_FULL_PATH		1024

/*********************************************
 *  Content : Data 관리  모듈               *
 *********************************************/
#define  NAME_LOCATIONFILE          "Location.loc"
#define  NAME_TMPLOCATIONFILE       "Location.tmp"

#define  NAME_STB_L_LOCATIONFILE    "STBLocationL.stbl"
#define  NAME_TMPSTB_L_LOCATIONFILE "STBLocationL.tmp"

#define  NAME_STB_R_LOCATIONFILE    "STBLocationR.stbr"
#define  NAME_TMPSTB_R_LOCATIONFILE "STBLocationR.tmp"

#define  NAME_STBLOCATIONFILE       "STBLocation.stb"
#define  NAME_TMPSTBLOCATIONFILE    "STBLocation.tmp"

#define  NAME_UTBLOCATIONFILE       "UTBLocation.utb"
#define  NAME_TMPUTBLOCATIONFILE    "UTBLocation.tmp"

#define  NAME_EQ_X_LOCATIONFILE     "LocationX.locx"
#define  NAME_TMPEQ_X_LOCATIONFILE  "LocationX.tmp"

#define  NAME_MAPFILE               "Map.map"
#define  NAME_TMPMAPFILE            "Map.tmp"

#define  NAME_PARAFILE              "OHTPARAM.ini"
#define  NAME_NODECHAGNE            "NodeChange.nch"
#define  NAME_NODEDISABLE           "NodeDisable.nod"
#define  NAME_HANDDETECTEQ          "HandDetectEQ.hnd"
#define  NAME_CROSSRAILINT          "CrossRailInterlock.cri"
#define  NAME_ERRCODEFILE           "ErrorCode.txt"
#define  NAME_NODEDIST              "NodeDist.txt"
#define  NAME_PINGADDR              "PingAddr.txt"
#define  NAME_HOISTPOS              "HoistPos.txt"
#define  NAME_SYNCFILE              "SyncGain.ini"


#define  NAME_AMCPARA            "DefaultParam.ini"

#define  NAME_SERIAL_CFG            "SerialConfig.cfg"
#define  NAME_AMCOUTOBJ             "AMCOut.out"
#define  NAME_LOADERIP              "LoaderIP.txt"

/*********************************************
 *  Content : Communication Definition       *
 *********************************************/
#define  WM_OCS_SEND                (WM_USER+2)

#define  chST                       '$'
#define  chTP                       's'
#define  chMOTOR                    'm'
#define  chLOG                      'f'
#define	chCR  	                  0x0D
#define	chLF  	                  0x0A
#define  chSTX                      0x02
#define  chETX                      0x03
#define  chRST                      0x1F
#define  chESC                      0x1B


//---------------------------------------------------------------------------
class MONITORING
{

 private:
	MONITOR_DATA_INFO* gm_Monitoringinfo;

	int Kill_VirusCheckProgram(void);
	BOOL SetPrivilege(HANDLE, LPCTSTR, BOOL);
	int Is_USBAlive(void);
	int GetNum_LogFile(String Path);
	int Get_IsLogDriveAlive(String strDrvName);

	BYTE Find_IsVirusCheckProgramExec(void);
	bool File_Find(String pTmpPath, String pPosData);
	char* Get_FileAccessTime(String strFileName);
	BYTE Get_Dos_Cmd_Result(TCHAR* strDosCmd);

	bool loadParameter();
	bool SaveParameter();

	bool m_bCheckWinSetup;

 public :
   MONITORING();
   ~MONITORING();

	//  int Init_for_SumValue();
   void MonitorUpdate(MONITOR_DATA_INFO *pMonitoringinfo);
	int MonitorInitData(MONITOR_OPERATION_STATUS *pOperationStatus);
   bool IsWinSetupLoaded(); //2014.07.02 doori.shin 추가

   MONITOR_DATA_INFO  Get_MonitorInfo();
};
//---------------------------------------------------------------------------
#endif
