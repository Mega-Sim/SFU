//---------------------------------------------------------------------------

#ifndef NetworkSignallibH
#define NetworkSignallibH
//---------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "NetworkAdapterControl.h"
#include "../Common/PUtily.h"
#include "../Common/Def_DataType.h"

#include "LogHandler.h"	// Log 작성 목적
#pragma comment (lib, "NetworkAdapterControl.lib")
 //기존은 OHT cpp에 USELIB("NetworkAdapterControl.lib")에 의해 라이브러리 사용 허가. puting

#define ERR_NETWORKAPDATER_INIT		777
#define ERR_NETWORKAPDATER_NOTOPEN	888

//#define MAX_PATH	1024


 /**
@brief   NetworkSinal 라이브러리
@author  puting
@date    2013.03.26
*/
class NetworkSinalLib
{
private:
	LogUnit *pLogWirelessNet;    				  ///< pLogWirelessNet 로그 생성을 위한 변수
	bool	m_bNETWORKReady;						   ///< NetWorkAdapter 사용 가능한 상태
	int     m_nNetworkAdapterIndex;    				   ///< NetWorkAdapter Index
	INetConnectInfo m_Info;
	bool	m_bDisconnectFlag;						   ///< Nerwork Adapter Disable 관련 Flag


public:
	int NETWORKInit(void);             				   ///< NetWork 초기화
	void NETWORKClose(void);             				   ///< NetWork 종료
	__property bool DisconnectFlag = {write = m_bDisconnectFlag, read = m_bDisconnectFlag};


public:
	NetworkSinalLib();
	~NetworkSinalLib();

	int Network_Adapter_Enable(void);       		  ///< Network Enable
	int NetWork_Adapter_Disable(void);                ///< Network Disable
	int Get_WLanInfo(APSIGNAL_DATA* APSignalData);    ///< Lan Info 얻기

};
#endif
