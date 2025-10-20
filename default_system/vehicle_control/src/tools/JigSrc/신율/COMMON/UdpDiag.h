//---------------------------------------------------------------------------

#ifndef UdpDiagH
#define UdpDiagH
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
class UdpDiag
{
private:	// User declarations

#ifdef IS_REAL_OHT
	LogUnit *pLogUnit;
#endif
	void DisplayLog(String strLog);
	int DIAG_BroadcastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort);
	// -------------- OHT 통신부가 사용하는 정보 -------------------
	// OHT가 사용할 정보 
	// (2) PORT 9020 (OHT-DIAG Server)
	AnsiString strDIAG_SVR_Address;
	AnsiString strDIAG_SVR_MachineID;	// OHT가 기억해놓은 진단서버 이름
										// "나"가 진단서버라면, 내 이름을 저장하고 있다
										// 진단서버가 기억하는 OHT의 정보는 DIAG_LIST[] 에 저장되어 있다
	bool bDIAG_SVR_Connected; 	// OHT가 사용. DIAG Server와 연결되었다는 플래그
	// -------------------------------------------------------------
	// --------------- 양쪽이 다 사용할 정보 -----------------------
	AnsiString strMyMachineId;		// 내 이름 - OHT면 "OHT-01"등, OCS면 "OCS-01"
	// --------------- DIAG 양쪽이 다 사용할 정보 -----------------------
	int iDIAG_MachineType;
	// AnsiString strDIAG_MachineId;
	int iDIAG_BroadCastPort;
	int iDIAG_UnicastPort;

	// -------------------------------------------------------------
	int UnicastSend(AnsiString strIpAddr, int iPort, AnsiString strMessage);
	int UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len);
	int BroadcastSend(int iPort, AnsiString strMessage);
	int BroadcastSend(int iPort, char* cMessage, int iLen);
	int UnicastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort);


// ====== 주소획득  관련 인터페이스 (진단검증 완료후 삭제예정)=======
	int DIAG_BroadcastBind();
	int DIAG_BroadcastBind(int iBroadCastListenPort);
	void DIAG_BroadcastClose();
	int DIAG_BroadCast_ThreadCall(AnsiString strMachineID, int iPort);
	int DIAG_BroadCast_ThreadCall();

	int Address_Broadcast();														// 3.3.1 MCP 또는 CLW가 주소획득 (디폴트 값으로 진행한다)
	int Address_Broadcast(int iMachineType, AnsiString strMachineID, int iPort);	// 3.3.1 MCP가 CLW 주소를 획득한다 & 3.3.2 CLW가 MCP주소를 획득한다
// ===================================================================

public:		// User declarations
	UdpDiag();
	~UdpDiag();

// ==================== 디폴트 설정 인터페이스 ====================
	// 자동 지정된 디폴트값을 한번에 변경.
	// (이것을 호출하지 않으면 기본 지정값이 사용됨. 반드시 사용할 필요는 없다)
	//  => 이 함수는 Bind() 호출전에 실행해줘야 인수없는 Bind실행시 값이 적용된다.
	// 1PC 모드라 해도 여기서 입력하면 그대로 적용된다.
	// void DiagInfoUpdate(int iMachineType, AnsiString strMachineId, int iBroadCastPort, int iUnicastPort);

// ==================== 주소획득  관련 인터페이스 ====================
	//int DIAG_BroadcastBind();
	//int DIAG_BroadcastBind(int iBroadCastListenPort);
	//void DIAG_BroadcastClose();
	//int DIAG_BroadCast_ThreadCall(AnsiString strMachineID, int iPort);
	//int DIAG_BroadCast_ThreadCall();

	//int Address_Broadcast();														// 3.3.1 MCP 또는 CLW가 주소획득 (디폴트 값으로 진행한다)
	//int Address_Broadcast(int iMachineType, AnsiString strMachineID, int iPort);	// 3.3.1 MCP가 CLW 주소를 획득한다 & 3.3.2 CLW가 MCP주소를 획득한다
// ===================================================================
	int OHT_AS_Bind();
	void OHT_AS_Close();
	
#ifdef IS_OHT
	bool DIAG_Svr_Connected();
	int UnicastSend_OHT_2_AS(char* buf, int len);
	int OHT_AS_ThreadCall(PKT_OHT2OCS& AS_Pkt);
#endif

#ifdef IS_AS
	int OHT_AS_ThreadCall(PKT_OCS2OHT& AS_Pkt);
	int UnicastSend_AS_2_OHT(char* buf, int len);
#endif
// ==================== 커맨드 통신 관련 인터페이스 ====================
	int UnicastBind();							// 컨트롤 포트를 디폴트 값으로 열고 싶으면 이 함수를 호출함
	int UnicastBind(int iUnicastListenPort);	// 컨트롤 포트를 포트번호 지정하여 열고 싶으면 이 함수를 호출
	void UnicastClose();
	int UnicastSend(char* buf, int len);

#ifdef IS_OHT
	int SetDiagInfo(AnsiString strSvrAddr, int iListenPortNum, AnsiString strMyName,int iOhtMachineType);
	int DIAG_UniCast_ThreadCall(PKT_OHT2DIAG&	OcsCmdPkt);
#endif

#ifdef IS_MCP
	int SetDiagInfo(AnsiString strOhtAddr, int iListenPortNum, AnsiString strMyName, int iDiagMachineType);
	int DIAG_UniCast_ThreadCall();
#endif

};

#endif
