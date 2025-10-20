//---------------------------------------------------------------------------

#ifndef UdpFMH
#define UdpFMH

#include "Protocol_Define_Modify.h"

#include "SocketInitClose.h"
#include "UdpServer.h"
//#include "UdpBroadCastSender.h"
#include "UdpUniCastSender.h"
//---------------------------------------------------------------------------

class UdpFM
{
private:	// User declarations
	void DisplayLog(String strLog);
	int UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len);



	// 아래 두 함수는 어시스턴스와 파일매니저 공통 사용한다
	int  AS_FILE_MANAGER_BIND();
	void AS_FILE_MANAGER_CLOSE();

public:		// User declarations
	UdpFM();
	~UdpFM();

#ifdef IS_AS  	// 파일매니저 => 어시스턴스로 통신 (OHT -> OCS 표준 패킷 사용)
	int UnicastSend_AS_2_FM(char* buf, int len);
	int UnicastSend_AS_2_OHT(char* buf, int len);
	int FM_2_AS_ThreadCall(PKT_OHT2OCS& AS_Pkt);
//	int AS_ThreadCall(PKT_OHT2OCS& AS_Pkt);
	int AS_ThreadCall(char* AS_Pkt, int* piMachineType);
#endif

#ifdef IS_FM 	// 어시스턴스 => 파일매니저 (OCS -> OHT 표준 패킷 사용)
	int UnicastSend_FM_2_AS(char* buf, int len);
	int AS_2_FM_ThreadCall(PKT_OCS2OHT& AS_Pkt);	
#endif

};

#endif
