//---------------------------------------------------------------------------

#ifndef UdpComH
#define UdpComH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

#include "Protocol_Define_Modify.h"
#ifdef IS_REAL_OHT
	#include "LogHandler.h"	// Log 작성 목적
#endif
//---------------------------------------------------------------------------
class UdpCom
{
private:	// User declarations

#ifdef IS_REAL_OHT
	LogUnit *pLogUnit;
#endif
	void DisplayLog(String strLog);
	int BroadcastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort);

	// -------------- OHT에 대한정보 (TP가 알고 있는) --------------
	AnsiString strOhtAddr;
	bool bOhtConnected;
	int iOhtPortNum;
	// -------------------------------------------------------------

	// -------------- OHT 통신부가 사용하는 정보 -------------------
	// TP에 대한정보 (OHT가 알고 있는)
	AnsiString strTpAddr;
	AnsiString strTpMachineID;
	bool bTpConnected;		// OHT가 사용. TP가 연결되었다는 플래그

	// OHT가 사용할 정보
	AnsiString strOcsAddress[MAX_OCS_NUMBER];
	AnsiString strOcsMachineID[MAX_OCS_NUMBER];
	int iOcsCount;	
	// bool bOcsConnected; 	// OHT가 사용. OCS와 연결되었다는 플래그
	// -------------------------------------------------------------
    // bool bOcsConnected;
	// --------------- MCP가 사용할 정보 ---------------------------
	//AnsiString strClwList[50];
	int iClwCount;
	// -------------------------------------------------------------

	// --------------- 양쪽이 다 사용할 정보 -----------------------
	int iMyMachineType;
	AnsiString strMyMachineId;
	int iMyBroadCastPort;
	int iMyUnicastPort;
	// -------------------------------------------------------------

	int UnicastSend(AnsiString strIpAddr, int iPort, AnsiString strMessage);
	int UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len);

	int Address_DropOrder_AllClw(AnsiString strMachineID, int iPort);
	int BroadcastSend(int iPort, AnsiString strMessage);
	int BroadcastSend(int iPort, char* cMessage, int iLen);

	int UnicastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort);
	int Address_DropOrder_ToClw(AnsiString strIpAddr, AnsiString strMachineID, int iPort);

public:		// User declarations
	UdpCom(int PortNum);
	~UdpCom();

// ==================== 디폴트 설정 인터페이스 ====================
	// 자동 지정된 디폴트값을 한번에 변경.
	// (이것을 호출하지 않으면 기본 지정값이 사용됨. 반드시 사용할 필요는 없다)
	//  => 이 함수는 Bind() 호출전에 실행해줘야 인수없는 Bind실행시 값이 적용된다.
	// 1PC 모드라 해도 여기서 입력하면 그대로 적용된다.
	void MyInfoUpdate(int iMachineType, AnsiString strMachineId, int iBroadCastPort, int iUnicastPort);

// ==================== 주소획득  관련 인터페이스 ====================
	int BroadcastBind();							// 주소획득 포트를 디폴트 값으로 열고 싶으면 이 함수를 호출함
	int BroadcastBind(int iBroadCastListenPort);	// 주소획득 포트에 포트번호를 지정하여 호출함
	void BroadcastClose();
	int BroadCast_ThreadCall(AnsiString strMachineID, int iPort);	// strMachineID : 자신의 MachineID, iPort : 상대방 포트번호 (주의!!)
	int BroadCast_ThreadCall();

	int Address_Broadcast();														// 3.3.1 MCP 또는 CLW가 주소획득 (디폴트 값으로 진행한다)
	int Address_Broadcast(int iMachineType, AnsiString strMachineID, int iPort);	// 3.3.1 MCP가 CLW 주소를 획득한다 & 3.3.2 CLW가 MCP주소를 획득한다

	// - 주소획득시 기존 OCS주소를 지우는데, 지우는 동작만 하지 않도록 하는 함수 (OHT)
	int Address_Broadcast_NotClear();														// 3.3.1 MCP 또는 CLW가 주소획득 (디폴트 값으로 진행한다)
	int Address_Broadcast_NotClear(int iMachineType, AnsiString strMachineID, int iPort);	// 3.3.1 MCP가 CLW 주소를 획득한다 & 3.3.2 CLW가 MCP주소를 획득한다

	int Address_DropOrder_ToClw();													// OCS가 CLW에게 주소를 삭제하도록 지시함
	int Address_DropOrder_AllClw(); 												// OCS가 모든 CLW에게 주소를 삭제하도록 지시함

#ifdef IS_OHT
	int OcsConnected();															// OHT가 사용. OCS와 연결 Count 
#endif

    // 20161216 sh17.jo : Commfail 체크 기능 추가
    void GetOcsAddress(char *OcsIP, int Ocs_Cnt);

// ==================== 커맨드 통신 관련 인터페이스 ====================
	int UnicastBind();							// 컨트롤 포트를 디폴트 값으로 열고 싶으면 이 함수를 호출함
	int UnicastBind(int iUnicastListenPort);	// 컨트롤 포트를 포트번호 지정하여 열고 싶으면 이 함수를 호출
	void UnicastClose();
	int UnicastSend(char* buf, int len);
	int UnicastSend_toMapView(AnsiString strMessage);
	void SetBroadCastMask(AnsiString strMask);

#ifdef IS_OHT
	int UniCast_ThreadCall(PKT_OCS2OHT&	OcsCmdPkt);
#endif

#ifdef IS_MCP
	int UniCast_ThreadCall();
#endif

#ifdef IS_TP
	int UniCast_ThreadCall();
#endif


// ==================== TP 관련 인터페이스 ====================
#ifdef IS_TP
	int TP_MAPVIEW_Bind();
#endif
	int OHT_TP_Bind(AnsiString sOhtAddr, int iOhtPort);
	int OHT_TP_Bind();									// OHT - TP의 패킷을 받기위해 사용하는 포트번호 Open (디폴트로)
	void OHT_TP_Close();
	int UnicastSend_OHT_2_TP(char* buf, int len);		// OHT -> TP Send func. (OHT Only Use)

#ifdef IS_OHT
	int OHT_TP_ThreadCall(PKT_OCS2OHT& OcsCmdPkt, char *szClientIP = NULL);		// OHT - TP간 통신 쓰레드 호출 (OHT 전용)
	bool TpConnected();									// OHT가 사용. TP가 연결되었다는 플래그
#endif

#ifdef IS_TP
	int MAPVIEW_TP_ThreadCall();
	int OHT_TP_ThreadCall();							// OHT - TP간 통신 쓰레드 호출 (TP 전용)
	bool OhtConnected();								// TP가 사용하는 정보. OHT가 연결되었다. (아직 미구현임!)
#endif
};
//---------------------------------------------------------------------------

#endif

