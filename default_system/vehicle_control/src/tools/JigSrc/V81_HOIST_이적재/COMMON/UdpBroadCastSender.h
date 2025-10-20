//---------------------------------------------------------------------------

#ifndef UdpBroadCastSenderH
#define UdpBroadCastSenderH

class UdpBroadCastSender
{
private:
	AnsiString strBroadCastMask;

public:
	UdpBroadCastSender();
	int BroadcastSendMessage(int iPort, AnsiString strMessage);
	int BroadcastSendMessage(int iPort, char* buf, int len);
	String err_display(char *msg);
	void SetMask(AnsiString strMask);
};
//---------------------------------------------------------------------------
#endif
