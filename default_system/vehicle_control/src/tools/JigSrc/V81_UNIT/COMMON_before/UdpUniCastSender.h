//---------------------------------------------------------------------------

#ifndef UdpUniCastSenderH
#define UdpUniCastSenderH

class UdpUniCastSender
{
private:


public:
	UdpUniCastSender();
	int UnicastSendMessage(AnsiString strIpAddr, int iPort, AnsiString strMessage);
	int UnicastSendMessage(AnsiString strIpAddr, int iPort, char* buf, int len);
	String err_display(char *msg);
};
//---------------------------------------------------------------------------
#endif
