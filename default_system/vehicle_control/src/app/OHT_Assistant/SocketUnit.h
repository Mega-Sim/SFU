//---------------------------------------------------------------------------

#ifndef SocketUnitH
#define SocketUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>

#include "OHT_TLV.h"
#include "../Common/Def_Protocol.h"
#include "Utility.h"
#include "../Common/Def_TPCommand.h"


//---------------------------------------------------------------------------
class TSocketForm : public TForm
{
__published:	// IDE-managed Components
	TServerSocket *m_ServerSocketDM;
	TServerSocket *m_ServerSocketTP;

	void __fastcall TPSocketClientRead(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall TPSocketClientConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall TPSocketClientDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall TPSocketClientError(TObject *Sender, TCustomWinSocket *Socket,
		TErrorEvent ErrorEvent, int &ErrorCode);

	void __fastcall DMSocketClientConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall DMSocketClientDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall DMSocketClientError(TObject *Sender, TCustomWinSocket *Socket,
		  TErrorEvent ErrorEvent, int &ErrorCode);
	void __fastcall DMSocketClientRead(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall FormDestroy(TObject *Sender);

private:	// User declarations
	// by zzang9un 2013.10.10
	OHTLogFile *m_Log;     ///< Log 저장을 위한 클래스

    // by zzang9un 2013.10.12 : 수신한 파일 사이즈
	int m_FileSize;

	bool g_bIsTPServerConnected;
	bool m_bDMConnected;    // Data Manager 연결 확인 변수
	// by zzang9un 2013.04.11 : TP Command 관련
	bool m_TPConnected;    ///< TP와 연결 여부를 판단하는 변수

	// by zzang9un 2013.10.14 : 소켓 버퍼보다 큰 TLV를 위한 Flag
	bool bBigTLV_DM;               ///< 소켓 버퍼보다 큰 TLV를 위한 Flag
	CMD_HEADER m_PreCmdHeader_DM;  ///< 이전 CmdHeader
	char *pBigTLV_DM;              ///< TLV 내용이 저장될 버퍼 포인터
	int m_BigTLVTotalLen_DM;       ///< TLV 패킷의 총 SIZE
	int m_BigTLVReadCnt_DM;        ///< Big TLV에 Read한 Byte 수
	void InitBigTLV_DM();      ///< Big TLV 관련 변수 초기화 함수

	// by zzang9un 2013.11.07 : TP와 통신을 위해 추가함
	///@todo Socket별로 아래 변수들이 있어야 함(구조화 필요)
	bool bBigTLV_TP;               ///< 소켓 버퍼보다 큰 TLV를 위한 Flag
	CMD_HEADER m_PreCmdHeader_TP;  ///< 이전 CmdHeader
	char *pBigTLV_TP;              ///< TLV 내용이 저장될 버퍼 포인터
	int m_BigTLVTotalLen_TP;       ///< TLV 패킷의 총 SIZE
	int m_BigTLVReadCnt_TP;        ///< Big TLV에 Read한 Byte 수
	void InitBigTLV_TP();      ///< Big TLV 관련 변수 초기화 함수

	// TP Command 관련 함수
	int MakeACK2TP(ACK_DATA SndACKData, char *pSndBuf);

	// by zzang9un 2013.10.10 : Command 관련 함수
	int HandleDMCmd(char *pRecvBuf, TCustomWinSocket *ClientSocket);
	int HandleTPCmd(char *pRecvBuf, TCustomWinSocket *ClientSocket);

	// by zzang9un 2013.10.11 : 통신 Log 저장 함수
	void AddTLVCommLog(AnsiString strTitle, char *pBuf, int size);

	// by zzang9un 2013.10.12 : Data Manager용 소켓 Read 함수
	//int ReadDMSocket(TCustomWinSocket *pSocket, int *Bytes, bool IsConvertEndian /* = flase */)

	// by zzang9un 2013.10.10 : Data 관련 함수
	int SendDataVerionInfo(char *pRecvBuf, TCustomWinSocket *ClientSocket);
	int SendDataFile(char *pRecvBuf, TCustomWinSocket *ClientSocket);
	int SendBackupDataFile(char *pRecvBuf, TCustomWinSocket *ClientSocket);
	int MakeFile2TLV(char *pBuf, String strFileName, int FileSize);
	int SendACKDataUpload(char *pRecvBuf, TCustomWinSocket *ClientSocket);

	int ReceiveDataFile(char *pRecvBuf);
	int MakeTLV2File(char *pBuf, BYTE DataType);

	void SendFBWFStatus(String cmd);
	void SendSetDisableFBWFStatus();
	void SendSetEnableFBWFStatus();
	void SendFileExists(String strFileName);
	void SendOHTAdditionalStatus();
	void SendCurrentTime(uTP_CMD& data);
	void SendCloseOHTProgram();

	//2015.05.12 doori.shin : TLV Class
    TLV* m_TLV_DM;

public:		// User declarations
	__fastcall TSocketForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSocketForm *SocketForm;
//---------------------------------------------------------------------------
#endif
