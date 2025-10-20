//---------------------------------------------------------------------------

#ifndef UdpClientH
#define UdpClientH
//---------------------------------------------------------------------------
typedef	enum	SNMP_GET_RESULT_
{
	SNMP_GET_OK			= 0,
	SNMP_GET_WSA_ERR,
	SNMP_GET_INVALID_SOCKET,
	SNMP_GET_NONE_BLOCK_ERR,
	SNMP_GET_CONNECT_ERR,
    SNMP_GET_ERROR,
    SNMP_GET_SOCKET_NOT_OPEN
}SNMP_GET_RESULT;

class UdpClient
{
private:
	SOCKET sock;
    SOCKADDR_IN servAdr;
    bool bUdpSocketOpenSuccess;

public:
	UdpClient(char *ServerIP, int Port);
    UdpClient::~UdpClient();
    int Send(char *packet, int length);
    int Receive(char *packet, int length);
    int SocketOpen(char *ServerIP, int Port);
    void SocketClose(void);
    bool SocketStatus(void);
};
#endif
