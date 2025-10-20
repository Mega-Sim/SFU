//---------------------------------------------------------------------------

#ifndef SocketUnitH
#define SocketUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>
#include <ExtCtrls.hpp>
#include "HoistDistanceConverter.h"

#include "OHT_TLV.h"
#include "Protocol_Define_Modify.h"
//---------------------------------------------------------------------------
class TSocketForm : public TForm
{
__published:	// IDE-managed Components
    TClientSocket *OHTSocket;           ///< OHT와 연결하는 Socket
    TClientSocket *AssistantSocket;     ///< Assistant와 연결하는 Socket
    TClientSocket *AutoTeachingSocket;
	TTimer *tmrOhtTypeCheck;    ///< OCS Remote(Data Server)와 연결하는 Socket
    
    void __fastcall OHTSocketConnect(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall OHTSocketConnecting(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall OHTSocketDisconnect(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall OHTSocketError(TObject *Sender,
          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall OHTSocketRead(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall AssistantSocketConnect(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall AssistantSocketConnecting(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall AssistantSocketDisconnect(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall AssistantSocketError(TObject *Sender,
          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
    void __fastcall AssistantSocketRead(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall AutoTeachingSocketConnect(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall AutoTeachingSocketConnecting(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall AutoTeachingSocketDisconnect(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall AutoTeachingSocketError(TObject *Sender,
          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
    void __fastcall AutoTeachingSocketRead(TObject *Sender,
          TCustomWinSocket *Socket);
	void __fastcall tmrOhtTypeCheckTimer(TObject *Sender);

private:	// User declarations
   void SocketInit();
   IO_MAP_FIELD *m_IOMapField;
   UnicodeString GetUnit(UnicodeString str);

public:		// User declarations
    String m_strOHTIPAddress;
    String m_strOHTName;
    String m_strOHTNum;

    bool bATReceive;

    // by zzang9un 2013.04.28 : 연결 여부 flag 추가
    bool bConnectedOHT;
    
    // shkim  void ConnectOHT(String strIPAddr, String strOHTName);
    void ConnectOHT(String strIPAddr, int iOhtPortNum, int iTpPortNum, String strOHTName);
    void DisConnect();

    // by zzang9un 2012.11.24 : 함수 원형 변경
    //void SendTPCmd2OHT(String strTmpCommand);
    void SendTPCmd2OHT(char *SndBuf, int nSndLen);
    void SendTPCmd2AST(char *SndBuf, int nSndLen);

    void AssistantConnect(String, String);
    void Assistant_DisConnect();
	void Assistant_Command2OHT(String strTmpCommand);
	void BayTestLongrun_Log(String strTmpCommand);
    String GetLocalIP();

    void AutoTeaching_Command2AT();

    __fastcall TSocketForm(TComponent* Owner);

    // Parsing 함수
	int ParsingOHTCmd(char *pRecvBuf, bool IsConvert);
	int ParsingAstCmd(char *pRecvBuf, bool IsConvert);
	bool fIsIOMapLoaded;

	// udp 수신함수
	void UdpSocketRead(PKT_OHT2OCS Pkt, int iMachineType);
	void UdpSocket_Status_Read(PKT_OHT2TP_RESPONSE Pkt, int iMachineType);
	void UdpSocket_IO_LIST_Read(PKT_OHT2TP_IOLIST Pkt);
	void UdpSocket_CID_INFO(PKT_OHT2TP_CID_INFO PKT);
	void UdpSocket_MapMove_Result(PKT_OHT2TP_MOVE_BY_MAP_RESULT PKT);
	void UdpSocket_MapMake_Result(PKT_OHT2TP_LOG_DATA PKT);
	void UdpSocket_AutoTeach_Result(PKT_TP2OHT_SIMPLE_COMMAND PKT);
	
	void UdpSocket_Standard_Read(PKT_OHT2OCS PKT);
	void DisplayTeachingData(PKT_OHT2TP_TEACHING_INFO SEND_PACKET);
	void DisplayAutoTeachingData(PKT_OHT2TP_TEACHING_INFO SEND_PACKET);
	void DisplayDrivingMapData(PKT_OHT2TP_DRIVINGMAP_INFO SEND_PACKET);

	// Firmware Update
	void UdpSocket_DataTransper_Response_Read(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE PKT);
	void UdpSocket_Transper_Progress_Read(PKT_DATA_TRANSPER_ORDER_PROGRESS PKT);
	void UdpSocket_VersionReq_Response_Read(PKT_VERSION_RESPONSE PKT);

	void UdpSocket_HID_LogSave_Result_Read(PKT_TP2OHT_SIMPLE_COMMAND PKT);
	void UdpSocket_Hoist_Param_Result_Read(PKT_TP2OHT_HOIST_TEACH_COMMAND PKT);

	void UdpSocket_AVS_Vibration_Data_Read(PKT_OHT2TP_VIBRATION_INFO PKT);
	void UdpSocket_AVS_Response(int nResponse);

	//AutoDriving
	void UdpSocket_AutoDriving_Route_Response(PKT_OHT2TP_AUTO_DRIVING_TEST_ROUTE_RESPONSE PKT);
	void UdpSocket_AutoDriving_Drive_Response(PKT_OHT2TP_AUTO_DRIVING_TEST_MOVE_RESPONSE PKT);

	//MapView-T/P 통신
	void UdpSocket_MapViewStatusRead(int nStatus);
	void UdpSocket_MapViewMapVersionRead(char* buf);
	void UdpSocket_MapViewUpdateResultRead(int nStatus,int nSuccess);

};
//---------------------------------------------------------------------------
extern PACKAGE TSocketForm *SocketForm;
//---------------------------------------------------------------------------
#endif
