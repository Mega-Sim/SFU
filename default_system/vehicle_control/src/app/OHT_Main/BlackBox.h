#ifndef BlackBoxH
#define BlackBoxH

#include "../Common/Protocol_Define_Modify.h"
//#include "../Common/UdpCom.h"
//#include "../Common/UdpDiag.h"
//#include "PassPermit.h"

#include "Def_OHT.h"
#include "../Common/Def_Protocol.h"
#include "Def_DefaultIniParam.h"

#include "TCPClient.h"
#include "OHTMainStatus.h"
//#include "StatusCheckThread.h"
#include "Utility.h"
#include "LogHandler.h"
#include "LogThread.h"

#include "MyTools.h"

//---------------------------------------------------------------------------

#define BLACKBOX_NONE			-1
#define BLACKBOX_STATUS			0
#define BLACKBOX_TIMESET		1
#define BLACKBOX_EVENT			2

#define BLACKBOX_VIDEOSAVE		1
#define BLACKBOX_STREAMING		2

#define LEN_HEADER_BLACKBOX 	10
#define LEN_BLACKBOX_TIMESET 	16
#define LEN_BLACKBOX_EVENT 		2
// =====================================================================================

class BlackBox
{
private:
	LogUnit *pLogBlackBoxComm; ///< Black Box 통신 로그 변수
		
	//EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;
	DEFAULT_PARAM_SET *m_defualtparam;
		
	TCPCLIENT*	pBlackBoxClientSocket;
		
	SYSTEMTIME m_BlackBox_SetTime_Recv;
	SYSTEMTIME m_BlackBox_SetTime_Send;
		
	BYTE BlackBoxEventType;
	BYTE BlackBoxEventResult;
	BYTE BlackBoxError;

	bool m_bBlackBoxTimeSet;
	bool m_bBlackBoxEventOK;

	DWORD m_dwRetryCommBlackBox;
				
	int BlackBoxSocketParsing(int *nDataType);
		
public:
	BlackBox();
	~BlackBox();
		
	bool m_ConnectedBlackBox;
	int m_nBlackBoxCommand;
	int m_nBlackBoxEventType;
		
	void ExcuteTimeCheck();
	void SuperviseSendDataOHT2BLACKBOX();
	void BlackBoxSocketUpdate();
		
	int GetBlackBoxData(char *pBuffer, int BuffLen, char *pTLV, int *Bytes, bool IsConvertEndian = false);
		
	long DifHour(SYSTEMTIME sTm1, SYSTEMTIME sTm2);
};
#endif
