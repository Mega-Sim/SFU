//---------------------------------------------------------------------------

#ifndef VMSH
#define VMSH

#include "../Common/Protocol_Define_Modify.h"
#include "CommHandler.h"
#include "ErrorCode.h"

#include "Def_DefaultIniParam.h"
#include "OHTMainStatus.h"

#include "LogHandler.h"
#include "LogThread.h"

//---------------------------------------------------------------------------

/* Packet 구조
|STX  | Header | Length | Data | CRC16 | ETX |
|1Byte| 1Byte  | 1Byte  | 가변 | 2Byte |1Byte|
*/

#define	CR					0x0D
#define	LF					0x0A

// VMS Bluetooth 명령어
#define BT_CMD_IDCHECK          "AT+BTINFO?"
#define BT_CMD_BTMODESET        "AT+BTMODE,0"
#define BT_CMD_MULTIMODESET     "AT+MULTI,2"
#define BT_CMD_CONNECT          "ATD"
#define BT_CMD_ONLINE           "ATO"
#define BT_CMD_DISCONNECT       "ATH"
#define BT_CMD_WAIT 	 	    "+++"
#define BT_CMD_SWRESET 	 	    "ATZ"
#define BT_CMD_HWRESET 	 	    "AT&F"

/**
@class   VMS
@date    2017.12.12
@author  kyh
@brief   VMS 관련 Class
*/
class VMS
{
private:
	LogUnit *pLogTest;      ///< 로그 생성을 위한 변수
	Comm_Port *m_pCommPort;

	OHTMainStatus *m_pOHTMainStatus;
	DEFAULT_PARAM_SET *m_defualtparam;

	bool bConnect;

	// VMS에 대한정보 (OHT가 알고 있는)
    bool bVMSConnected;		// OHT가 사용. VMS가 연결되었다는 플래그

public:
	VMS();
	~VMS();

	bool IsConnect();
	int AnalysisData();

	int OpenCom();
	int CloseCom();
	int ReOpenCom();

	int SetMode();
	int SetMultiMode();
	int Connect(AnsiString strBTID);
	int SetOnline();
	int Disconnect();
	int SetWait();

	int GetBTID();
	int SetSWReset();
	int SetHWReset();

	int GetData();//PKT_OCS2OHT& OcsCmdPkt);
	int SendData2VMS(unsigned char* SendData, int length);

	bool VMSConnected();
	void SetVMSDisconnect();

	void SuperviseSendCmdDataOHT2VMS();
};

#endif
