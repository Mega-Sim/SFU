//---------------------------------------------------------------------------
#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpClient.h"
#include "LogHandler.h"	// Log 작성 목적
#define UDP_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\UDP.log", __VA_ARGS__)
//---------------------------------------------------------------------------

#pragma package(smart_init)
UdpClient::UdpClient(char *ServerIP, int Port)
{
    bUdpSocketOpenSuccess = false;
    if(SocketOpen(ServerIP, Port) == SNMP_GET_OK)
        bUdpSocketOpenSuccess = true;
}
UdpClient::~UdpClient()
{
    SocketClose();
}

int UdpClient::SocketOpen(char *ServerIP, int Port)
{
	WSADATA wsaData;
    int strLen;
    int nError = 0;
    int iResult = SNMP_GET_OK;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		UDP_LOG("[UdpClient] WSAStartup() error");
		iResult = SNMP_GET_WSA_ERR;
	}

    if(iResult == SNMP_GET_OK)
    {
        sock = socket(PF_INET, SOCK_DGRAM, 0);
        if (sock == INVALID_SOCKET)
        {
            UDP_LOG("[UdpClient] socket() error");
            iResult = SNMP_GET_INVALID_SOCKET;
        }
    }

    if(iResult == SNMP_GET_OK)
    {
        // [recv] non Blocking option
        u_long on = 1;
        strLen = ioctlsocket(sock, FIONBIO, &on);
        if (strLen == SOCKET_ERROR)
        {
            iResult = SNMP_GET_NONE_BLOCK_ERR;
        }
    }

    if(iResult == SNMP_GET_OK)
    {
        memset(&servAdr, 0, sizeof(servAdr));
        servAdr.sin_family = AF_INET;
        servAdr.sin_addr.s_addr = inet_addr(ServerIP);
        servAdr.sin_port = htons(Port);
        strLen = connect(sock, (SOCKADDR *)&servAdr, sizeof(servAdr));
        if (strLen == -1)
        {
            UDP_LOG("[UdpClient] connect error!");
            iResult = SNMP_GET_CONNECT_ERR;
        }
    }

    if(iResult != SNMP_GET_OK)
    {
         UDP_LOG("[UdpClient] Error : %d", iResult);
    }
    return iResult;
}

int UdpClient::Send(char *packet, int length)
{
    int strLen = 0;
    strLen = send(sock, packet, length, 0);
    if (strLen == -1)
	{
        return -1;
    }
    return strLen;
}

int UdpClient::Receive(char *packet, int length)
{
    int strLen = 0;
    strLen = recv(sock, packet, length, 0);
    if (strLen == -1)
	{
        return -1;
    }
    return strLen;
}

void UdpClient::SocketClose(void)
{
    closesocket(sock);
    // WSACleanup();
}

bool UdpClient::SocketStatus(void)
{
    return bUdpSocketOpenSuccess;
}
