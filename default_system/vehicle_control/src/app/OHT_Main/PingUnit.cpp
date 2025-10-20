/**
@file    PingUnit.cpp
$Author:: soo583
$Date:: 2013/07/17
@version OHT 6.5
@brief   Ping Unit CPP File
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PingUnit.h"
#include "Ping.h"
#include "Utility.h"

TOHT_PING *gm_Ping;

//---------------------------------------------------------------------------
/**
@brief   TOHT_PING 생성자
@author  soo583
@date    2013/07/17
*/
TOHT_PING::TOHT_PING()
{
   int nError=0;
   AnsiString strTmpLog;
   status = 0;


   nError = Socket_Create();
   if(nError==1)
   {
      strTmpLog = "ERROR : Error initializing WinSock";
//      if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);
   }
   else if(nError==2)
   {
	  strTmpLog = "ERROR : WinSock version not supported";
//      if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);
   }
   else if(nError==3)
   {
	  strTmpLog = "ERROR : Make Socket Error";
//      if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);
   }
   /*
   else
   {
      strTmpLog = "Ping Make Socket ..OK";
      if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);
   }
   */

}
//---------------------------------------------------------------------------
/**
@brief   TOHT_PING 소멸자
@author  soo583
@date    2013/07/17
*/
TOHT_PING::~TOHT_PING()
{
   Socket_Destroy();
}
//---------------------------------------------------------------------------
/**
@brief   소켓 생성하는 함수
@author  soo583
@date    2013/07/17
*/
int TOHT_PING::Socket_Create(void)
{
   int nError=0;
   WSADATA wsaData;
   WORD wVersionRequested = MAKEWORD(1,1);
   int nRet;

	// Init WinSock
   nRet = WSAStartup(wVersionRequested, &wsaData);
   if (nRet)   return 1;

   // Check version
   if (wsaData.wVersion != wVersionRequested) return 2;

	rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(rawSocket == INVALID_SOCKET ) return 3;

   return nError;
}
//---------------------------------------------------------------------------
/**
@brief   소켓 파괴하는 함수
@author  soo583
@date    2013/07/17
@return  성공하면 0, 실패하면 1
*/
int TOHT_PING::Socket_Destroy()
{
   int nError=0;

   if(rawSocket != NULL)
   {
	   nError = closesocket(rawSocket);
	   if(nError == SOCKET_ERROR)
      {
         nError=1;
      }
   }
	// Free WinSock
   WSACleanup();
   return nError;
}
//---------------------------------------------------------------------------
/**
@brief   핑을 실행하는 함수
@author  soo583
@date    2013/07/17
@param   strHost : 호스트 주소
*/
// return time : -1(timeout or error)
int TOHT_PING::Ping(AnsiString strHost)
{
	LPHOSTENT lpHost;
	struct    sockaddr_in saDest;
	struct    sockaddr_in saSrc;
	DWORD	  dwTimeSent;
	DWORD	  dwElapsed;
	u_char    cTTL;
	int       nRet;
   AnsiString strTmpLog;
   AnsiString strSrc, strDest;
   bool bFind=false;
   DWORD  dwStart, dwEnd;

	// Lookup host
	lpHost = gethostbyname(strHost.c_str());
	if (lpHost == NULL)
	{
      strTmpLog.sprintf("Host not found: %s", strHost);
//      if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);
		return -1;
	}

	// Setup destination socket address
	saDest.sin_addr.s_addr = *((u_long FAR *) (lpHost->h_addr));
	saDest.sin_family = AF_INET;
	saDest.sin_port = 0;

	// Tell the user what we're doing
	strTmpLog.sprintf("Pinging %s [%s] with %d bytes of data:",
				strHost,inet_ntoa(saDest.sin_addr),REQ_DATASIZE);
//   if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);
	// Send ICMP echo request
	SendEchoRequest(rawSocket, &saDest);

   // Use select() to wait for data to be received
   dwStart=GetTickCount();
   while(bFind==false)
   {
	   nRet = WaitForEchoReply(rawSocket);
	   if (nRet == SOCKET_ERROR)
	   {
         strTmpLog="Socket Error";
//         if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);

		   return -1;
	   }
	   if (!nRet)
	   {
		 strTmpLog.sprintf("TimeOut:%d[sec]", TIME_OUT);
//         if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);

		   return -1;
	   }
      // Receive reply
	   dwTimeSent = RecvEchoReply(rawSocket, &saSrc, &cTTL);
      strSrc.sprintf("%s",inet_ntoa(saSrc.sin_addr));
      strDest.sprintf("%s",inet_ntoa(saDest.sin_addr));
      if(strSrc==strDest)
      {
         bFind=true;
      }
      dwEnd = GetTickCount() - dwStart;
      if(dwEnd >1000)
      {
         strTmpLog.sprintf("TimeOut:%d[sec]", TIME_OUT);
//         if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);

		   return -1;
      }
   }
	// Calculate elapsed time
	dwElapsed = GetTickCount() - dwTimeSent;

	strTmpLog.sprintf("%ld",dwElapsed);
   if(strTmpLog.Pos("-")!=0)
   {
      strTmpLog.sprintf("TimeOut:%d[sec]", TIME_OUT);
//      if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);

		return -1;
   }
	strTmpLog.sprintf("Reply from: %s: bytes=%d time=%ldms TTL=%d",
                  inet_ntoa(saSrc.sin_addr),
			         REQ_DATASIZE,
                  dwElapsed,
                  cTTL);
//   if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);
   return dwElapsed;
}
//---------------------------------------------------------------------------
// SendEchoRequest()
// Fill in echo request header
// and send to destination
/**
@brief   Eho Request 보내는 함수
@author  soo583
@date    2013/07/17
@param   s : 소켓
         lpstToAddr : IP 주소
*/
int TOHT_PING::SendEchoRequest(SOCKET s,LPSOCKADDR_IN lpstToAddr)
{
	static ECHOREQUEST echoReq;
	static int nId = 1;
	static int nSeq = 1;
	int nRet;

	// Fill in echo request
	echoReq.icmpHdr.Type		= ICMP_ECHOREQ;
	echoReq.icmpHdr.Code		= 0;
	echoReq.icmpHdr.Checksum	= 0;
	echoReq.icmpHdr.ID			= nId++;
	echoReq.icmpHdr.Seq			= nSeq++;

	// Fill in some data to send
	for (nRet = 0; nRet < REQ_DATASIZE; nRet++)
		echoReq.cData[nRet] = ' '+nRet;

	// Save tick count when sent
	echoReq.dwTime				= GetTickCount();

	// Put data in packet and compute checksum
	echoReq.icmpHdr.Checksum = in_cksum((u_short *)&echoReq, sizeof(ECHOREQUEST));

	// Send the echo request  								  
	nRet = sendto(s,						/* socket */
				 (LPSTR)&echoReq,			/* buffer */
				 sizeof(ECHOREQUEST),
				 0,							/* flags */
				 (LPSOCKADDR)lpstToAddr, /* destination */
				 sizeof(SOCKADDR_IN));   /* address length */


	return (nRet);
}
//---------------------------------------------------------------------------
// WaitForEchoReply()
// Use select() to determine when
// data is waiting to be read
/**
@brief   Eho EchoReply를 기다리는 함수
@author  soo583
@date    2013/07/17
@param   s : 소켓
*/
int TOHT_PING::WaitForEchoReply(SOCKET s)
{
   struct timeval Timeout;
	fd_set readfds;

	readfds.fd_count = 1;
	readfds.fd_array[0] = s;
	Timeout.tv_sec = (int)TIME_OUT;
   Timeout.tv_usec = 0;

	return(select(1, &readfds, NULL, NULL, &Timeout));
}
//---------------------------------------------------------------------------
// RecvEchoReply()
// Receive incoming data
// and parse out fields
/**
@brief   Eho EchoReply를 받는 함수
@author  soo583
@date    2013/07/17
@param   s : 소켓
         lpsaFrom : IP 주소
         pTTL : Time To Live
*/
DWORD TOHT_PING::RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, u_char *pTTL)
{
	ECHOREPLY echoReply;
	int nRet;
	int nAddrLen = sizeof(struct sockaddr_in);

   	memset(&echoReply,0x00, sizeof(echoReply));

    AnsiString strTmpLog;
	// Receive the echo reply
	nRet = recvfrom(s,					// socket
					(LPSTR)&echoReply,	// buffer
					sizeof(ECHOREPLY),	// size of buffer
					0,					// flags
					(LPSOCKADDR)lpsaFrom,	// From address
					&nAddrLen);			// pointer to address len

	if(nRet == SOCKET_ERROR)
	{
      strTmpLog="Socket Error";
//      if(m_LogFile[LOG_PING]->g_bIsUseFunction==false) m_LogFile[LOG_PING]->WriteLogFile( strTmpLog);
   }
	// return time sent and IP TTL
	*pTTL = echoReply.ipHdr.TTL;
	return(echoReply.echoRequest.dwTime);
}
//---------------------------------------------------------------------------
/**
@brief   체크섬을 실행하는 함수
@author  soo583
@date    2013/07/17
@param   addr : 주소
         len : 길이
*/
u_short TOHT_PING::in_cksum(u_short *addr, int len)
{
	register int nleft = len;
	register u_short *w = addr;
	register u_short answer;
	register int sum = 0;

	/*
	 *  Our algorithm is simple, using a 32 bit accumulator (sum),
	 *  we add sequential 16 bit words to it, and at the end, fold
	 *  back all the carry bits from the top 16 bits into the lower
	 *  16 bits.
	 */
	while( nleft > 1 )  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if( nleft == 1 ) {
		u_short	u = 0;

		*(u_char *)(&u) = *(u_char *)w ;
		sum += u;
	}

	/*
	 * add back carry outs from top 16 bits to low 16 bits
	 */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return (answer);
}


void TOHT_PING::Init()
{
	strResult = "";
	status = 0;
}

int TOHT_PING::GetStatus()
{
	return status;
}

/**
@brief   FBWF로 명령을 보낸 결과를 저장한 문자열을 받음
@author  doori.shin
@date    2013.12.31
@return	 strResult : FBWF명령을 보낸 결과를 갖고있음

String TOHT_PING::GetResultString()
{
    return strResult;
}
*/

/**
@brief   	도스 커맨드로 명령어를 보내어 실행하게 하는 함수
			이 프로그램에서는 FBWF에 맞추어 설계되었다
@author  	doori.shin
@date    	2013.12.31
@param		strDosCmd : 도스 명령어
@return	 	strResult : FBWF명령을 보낸 결과를 갖고있음

int TOHT_PING::ExecuteDosCmd(String strDosCmd)
{
	bool                    bflag;
	HANDLE                  hIwrite, hIread;
	HANDLE                  hOwrite, hOread;
	HANDLE                  hEwrite, hEread;
	SECURITY_ATTRIBUTES     sa;
	STARTUPINFO             siParam;
	PROCESS_INFORMATION     piParam;
	DWORD                   dwExitCode;
	int error = 0x07;
	Init();

	DWORD BytesRead;
	AnsiString strData;

	// Pipe Parameter Set
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = true;

	// 어노니머스 파이프 생성  // Input File
	bflag = CreatePipe(&hIread, &hIwrite, &sa, 0);
	if (!bflag)
	{
//		error = ERR_PIPE_OPEN_ERROR;
		CloseHandle(hIread);
		CloseHandle(hIwrite);

		return error;
	}

	// 어노니머스 파이프 생성  // output File
	bflag = CreatePipe(&hOread, &hOwrite, &sa, 0);
	if (!bflag)
	{
//		error = ERR_PIPE_OPEN_ERROR;
		CloseHandle(hIread);
		CloseHandle(hIwrite);
		CloseHandle(hOread);
		CloseHandle(hOwrite);

		return error;
	}

	// 어노니머스 파이프 생성 // Error File
	bflag = CreatePipe(&hEread, &hEwrite, &sa, 0);
	if (!bflag)
	{
//		error = ERR_PIPE_OPEN_ERROR;
		CloseHandle(hIread);
		CloseHandle(hIwrite);
		CloseHandle(hOread);
		CloseHandle(hOwrite);
		CloseHandle(hEread);
		CloseHandle(hEwrite);

		return error;
	}

	// 초기화
	FillMemory(&siParam, sizeof(STARTUPINFO), 0);
	FillMemory(&piParam, sizeof(PROCESS_INFORMATION), 0);

	// Param 섯팅
	siParam.cb          = sizeof(STARTUPINFO);
	siParam.dwFlags     = STARTF_USESTDHANDLES;
	siParam.wShowWindow = SW_HIDE;
	siParam.hStdInput = hIread;   // 표준입력(stdin)
	siParam.hStdOutput = hOwrite;  // 표준출력(stdout) 리다이렉션
	siParam.hStdError = hEwrite;   // 표준에러(stderr) 리다이렉션

	hIread = CreateFile(L"input.d",            //표준입력용 input.d 파일 오픈
					   GENERIC_READ,
					   FILE_SHARE_READ,
					   &sa,
					   OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL);

	bflag = CreateProcess(NULL, strDosCmd.c_str(), NULL, NULL, true, DETACHED_PROCESS, NULL, NULL, &siParam, &piParam);
	WaitForInputIdle(GetCurrentProcess(), INFINITE);

	if(!bflag)
	{
		CloseHandle(hIread);
		CloseHandle(hIwrite);
		CloseHandle(hOread);
		CloseHandle(hOwrite);
		CloseHandle(hEread);
		CloseHandle(hEwrite);
//		error = ERR_CREATE_PROCESS;

		return error;
	}

	// 프로세스가 종료될때까지 대기함
	if(piParam.hProcess)
	{
	  dwExitCode = STILL_ACTIVE;

		while(dwExitCode == STILL_ACTIVE)
		{
			WaitForSingleObject(piParam.hProcess, INFINITE);
			GetExitCodeProcess(piParam.hProcess, &dwExitCode);
			CloseHandle(piParam.hProcess);
			CloseHandle(piParam.hThread);
//			UpdateWindow(Handle);
		}
	}

	CloseHandle(hIread);
	CloseHandle(hOwrite);
	CloseHandle(hEwrite);

	char *readBuffer = new char[512];

	while(ReadFile(hOread, readBuffer, sizeof(readBuffer), &BytesRead, NULL) && BytesRead)
	{
		readBuffer[BytesRead] = '\0';
		strResult += readBuffer;
	}

	if(strResult.Length() != 0)
	{
		strResult += "\0";
	}

	CloseHandle(hOread);
	CloseHandle(hEread);
	CloseHandle(hIwrite);

	delete[] readBuffer;



//	if(strResult.Length() != 0)
//	{
//		int ePos = strResult.Pos("filter state: enabled");
//		int dPos = strResult.Pos("filter state: disabled");
//
//		if(ePos == 0)
//			status = FBWF_DISABLE;
//		else if(dPos == 0)
//			status = FBWF_ENABLE;
//		else if(ePos < dPos)
//			status = FBWF_ENABLE;
//		else if(dPos < ePos)
//			status = FBWF_DISABLE;
//
//		return status;
//	}
//	else
//	{
//		return error;
//	}



	return error;
}
*/
#pragma package(smart_init)
