//---------------------------------------------------------------------------
#include "CommHandler.h"
#include <windows.h>
#include "LogHandler.h"	// Log 작성 목적

/////////////////////////////////////////////////////////////////////////////
// C_Comm_Port
// com5  : OHT Detect Sen
// com6  : BCR
// com7  : Front Steer(원점용)
// com8  : Rear Steer(원점용)
// com9  : Front Shutter
// com10 : Rear Shutter
// com11 : LookDown Sensor(Setting)
// com12 : Regulator
/////////////////////////////////////////////////////////////////////////////

#define	NONE_BLOCK_TYPE	0
#define HID_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\HIDRawData.log", __VA_ARGS__)
#define SOS_RECV_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\SOS_Rcv_RawData.log", __VA_ARGS__)

/////////////////////////////////////////////////////////////////////////////

Comm_Port::Comm_Port(int port_no)
{
	TCHAR 			port_name[30];
	DWORD	    	ErrCode = 0;
	OSVERSIONINFO	OSVer;

	OSVer.dwOSVersionInfoSize = sizeof(OSVer);
	GetVersionEx(&OSVer);
	if(OSVer.dwPlatformId  == VER_PLATFORM_WIN32_NT) // OS = "WINNT";
	{
#ifdef  UNICODE
		wsprintf(port_name, L"\\\\.\\COM%d", port_no);
#else	/* UNICODE */
		sprintf(port_name, "\\\\.\\COM%d", port_no);
#endif	/* UNICODE */
 	}
	else
	{
#ifdef  UNICODE
		wsprintf(port_name, L"COM%d", port_no);
#else	/* UNICODE */
		sprintf(port_name, "COM%d", port_no);
#endif	/* UNICODE */
	}

	SetLastError(NULL);

	m_hCOM = CreateFile( port_name,
	        GENERIC_READ | GENERIC_WRITE,	// access (read_write) mode
	  	     0,								// share mode
	        NULL,							// security descriptor
	        OPEN_EXISTING,					// how to create
#if NONE_BLOCK_TYPE	        
	        FILE_FLAG_OVERLAPPED,//,							// file attribute & flag
#else
			NULL,							// file attribute & flag
#endif
	        NULL);							// handle of file with attribute to copy

	ErrCode=(DWORD)GetLastError();
	if(ErrCode != NULL)
	{
      return;
	}

	SetCommMask(m_hCOM, EV_RXCHAR);

	SetupComm(m_hCOM,1024,1024);   // Set buffer size
	PurgeComm(m_hCOM, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ); // buffer clear

	//////   This mode sets NON_BLOCKING I/O
	COMMTIMEOUTS 	CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout		 =	  MAXDWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier  =	  0;
	CommTimeOuts.ReadTotalTimeoutConstant	 =	  0;//300;
	CommTimeOuts.WriteTotalTimeoutMultiplier =	  0;
	CommTimeOuts.WriteTotalTimeoutConstant	 =	  0;//300;
//	CommTimeOuts.ReadIntervalTimeout		 =	  1;
//	CommTimeOuts.ReadTotalTimeoutMultiplier  =	  1;
//	CommTimeOuts.ReadTotalTimeoutConstant	 =	  1;//300;
//	CommTimeOuts.WriteTotalTimeoutMultiplier =	  1;
//	CommTimeOuts.WriteTotalTimeoutConstant	 =	  1;//300;
	SetCommTimeouts(m_hCOM, &CommTimeOuts);

	DCB		dcb;
	GetCommState(m_hCOM, &dcb);
	dcb.BaudRate  =  38400;
	dcb.ByteSize  =  8;
	dcb.Parity    =  NOPARITY;      // NOPARITY, ODDPARITY, EVENPARITY
	dcb.StopBits  =  ONESTOPBIT;    // ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS
	SetCommState(m_hCOM, &dcb);

	SetLastError(0);
}

/////////////////////////////////////////////////////////////////////////////
Comm_Port::Comm_Port(int port_no, int nBaudRate)
{
	TCHAR 			port_name[30];
	DWORD	    	ErrCode = 0;
	OSVERSIONINFO	OSVer;
	//ADD_LOG("Comm_Port1 : %d, %d", port_no, nBaudRate);

	OSVer.dwOSVersionInfoSize = sizeof(OSVer);
	GetVersionEx(&OSVer);
	if(OSVer.dwPlatformId  == VER_PLATFORM_WIN32_NT) // OS = "WINNT";
	{
#ifdef  UNICODE
		wsprintf(port_name, L"\\\\.\\COM%d", port_no);
#else	/* UNICODE */
		sprintf(port_name, "\\\\.\\COM%d", port_no);
#endif	/* UNICODE */
 	}
	else
	{
#ifdef  UNICODE
		wsprintf(port_name, L"COM%d", port_no);
#else	/* UNICODE */
		sprintf(port_name, "COM%d", port_no);
#endif	/* UNICODE */
	}

	SetLastError(NULL);

	m_hCOM = CreateFile( port_name,
	        GENERIC_READ | GENERIC_WRITE,	// access (read_write) mode
	  	     0,								// share mode
	        NULL,							// security descriptor
	        OPEN_EXISTING,					// how to create
#if NONE_BLOCK_TYPE	        
			FILE_FLAG_OVERLAPPED,//,							// file attribute & flag
#else
			NULL,							// file attribute & flag
#endif
	        NULL);							// handle of file with attribute to copy

	ErrCode=(DWORD)GetLastError();
	if(ErrCode != NULL)
	{
	   //ADD_LOG("File Open Error (%d) : %s\n", ErrCode, port_name);
      return;
	}

	SetCommMask(m_hCOM, EV_RXCHAR);

	SetupComm(m_hCOM,1024,1024);   // Set buffer size
	PurgeComm(m_hCOM, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ); // buffer clear

	//////   This mode sets NON_BLOCKING I/O
	COMMTIMEOUTS 	CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout		 =	  MAXDWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier  =	  0;
	CommTimeOuts.ReadTotalTimeoutConstant	 =	  0;//300;
	CommTimeOuts.WriteTotalTimeoutMultiplier =	  0;
	CommTimeOuts.WriteTotalTimeoutConstant	 =	  0;//300;
//	CommTimeOuts.ReadIntervalTimeout		 =	  1;
//	CommTimeOuts.ReadTotalTimeoutMultiplier  =	  1;
//	CommTimeOuts.ReadTotalTimeoutConstant	 =	  1;//300;
//	CommTimeOuts.WriteTotalTimeoutMultiplier =	  1;
//	CommTimeOuts.WriteTotalTimeoutConstant	 =	  1;//300;
	SetCommTimeouts(m_hCOM, &CommTimeOuts);

	DCB		dcb;
	GetCommState(m_hCOM, &dcb);
	dcb.BaudRate  =  nBaudRate;
	dcb.ByteSize  =  8;
	dcb.Parity    =  NOPARITY;
	dcb.StopBits  =  ONESTOPBIT;
	SetCommState(m_hCOM, &dcb);

	SetLastError(0);
}

/////////////////////////////////////////////////////////////////////////////
Comm_Port::Comm_Port(int port_no, int nBaudRate, BYTE byByte ,BYTE byParity, BYTE byStopBits)
{
	TCHAR 			port_name[30];
	DWORD	    	ErrCode = 0;
	OSVERSIONINFO	OSVer;
	//ADD_LOG("Comm_Port2 : %d, %d, %d, %d, %d", port_no, nBaudRate, byByte ,byParity, byStopBits);

	OSVer.dwOSVersionInfoSize = sizeof(OSVer);
	GetVersionEx(&OSVer);
	if(OSVer.dwPlatformId  == VER_PLATFORM_WIN32_NT) // OS = "WINNT";
	{
#ifdef  UNICODE
		wsprintf(port_name, L"\\\\.\\COM%d", port_no);
#else	/* UNICODE */
		sprintf(port_name, "\\\\.\\COM%d", port_no);
#endif	/* UNICODE */
 	}
	else
	{
#ifdef  UNICODE
		wsprintf(port_name, L"COM%d", port_no);
#else	/* UNICODE */
		sprintf(port_name, "COM%d", port_no);
#endif	/* UNICODE */
	}

	SetLastError(NULL);

	m_hCOM = CreateFile( port_name,
	        GENERIC_READ | GENERIC_WRITE,	// access (read_write) mode
	  	     0,								// share mode
	        NULL,							// security descriptor
	        OPEN_EXISTING,					// how to create
#if NONE_BLOCK_TYPE	        
			FILE_FLAG_OVERLAPPED,//,							// file attribute & flag
#else
			NULL,							// file attribute & flag
#endif
	        NULL);							// handle of file with attribute to copy

	ErrCode=(DWORD)GetLastError();
	if(ErrCode != NULL)
	{
      return;
	}

	SetCommMask(m_hCOM, EV_RXCHAR);

	SetupComm(m_hCOM,1024,1024);   // Set buffer size
	PurgeComm(m_hCOM, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ); // buffer clear

	//////   This mode sets NON_BLOCKING I/O
	COMMTIMEOUTS 	CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout         =    MAXDWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier  =    0;
	CommTimeOuts.ReadTotalTimeoutConstant    =    0;//300;
	CommTimeOuts.WriteTotalTimeoutMultiplier =    0;
	CommTimeOuts.WriteTotalTimeoutConstant   =    0;//300;
//	CommTimeOuts.ReadIntervalTimeout         =    1;
//	CommTimeOuts.ReadTotalTimeoutMultiplier  =    1;
//	CommTimeOuts.ReadTotalTimeoutConstant    =    1;//300;
//	CommTimeOuts.WriteTotalTimeoutMultiplier =    1;
//	CommTimeOuts.WriteTotalTimeoutConstant   =    1;//300;
	SetCommTimeouts(m_hCOM, &CommTimeOuts);

	DCB		dcb;
	GetCommState(m_hCOM, &dcb);
	dcb.BaudRate  =  nBaudRate;
	dcb.ByteSize  =  byByte;
	dcb.Parity    =  byParity;
	dcb.StopBits  =  byStopBits;
	SetCommState(m_hCOM, &dcb);

	SetLastError(0);
}

/////////////////////////////////////////////////////////////////////////////
//
Comm_Port::~Comm_Port()
{
	SetCommMask(m_hCOM, 0);
	PurgeComm(m_hCOM, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
	CloseHandle(m_hCOM);
}

/////////////////////////////////////////////////////////////////////////////
//
//void	Comm_Port::SetCommParam(int Baud, int Data, int Stop, int Parity)
//{	// This Method is optional, Constructor Set Comm Param to Default Value
//	// Variable "Stop" and "Parity" can have Symbolic Constants defined in "WINBASE.H"
//	DCB		dcb;
//	GetCommState(m_hCOM, &dcb);
//	dcb.BaudRate  =  Baud;
//	dcb.ByteSize  =  (BYTE)Data;
//	dcb.Parity    =  (BYTE)Parity;
//	dcb.StopBits  =  (BYTE)Stop;
//	SetCommState(m_hCOM, &dcb);
//}


/////////////////////////////////////////////////////////////////////////////
//
DWORD	Comm_Port::ReadCommBlock(char *Buffer, int Length)
{
//	DWORD	nBytes=0;
	unsigned long    nBytes=0;
	char	tmpBuf[MAX_COMM_PORT_STREAM] = "";

	if ( ReadFile(m_hCOM, tmpBuf, Length, &nBytes, NULL) )
	{
		memcpy(Buffer, tmpBuf, nBytes);
	}
//	HID_LOG("Read Bytes = %d", (int)nBytes);

	return (DWORD)nBytes;
}

/////////////////////////////////////////////////////////////////////////////
//
DWORD	Comm_Port::WriteCommBlock(char *Buffer, int Length)
{
//	DWORD	nBytes=0;
	unsigned long    nBytes=0;

//	SOS_RECV_LOG("WriteCommBlock Len=%d, chSndData=%s", Length, Buffer);       //에코용

	if ( !WriteFile( m_hCOM, Buffer, Length, &nBytes, NULL) || Length!=(int)nBytes )
	{
//		HID_LOG("Wrong Bytes = %d", (int)nBytes);        //에코용
		nBytes=0;
	}
//	HID_LOG("Bytes = %d", (int)nBytes);        //에코용

	return (DWORD)nBytes;
}

/////////////////////////////////////////////////////////////////////////////
//
DWORD	Comm_Port::ReadCommBlock(UCHAR *Buffer, int Length)
{
//	DWORD	nBytes=0;
	UCHAR	tmpBuf[MAX_COMM_PORT_STREAM] = "";
	unsigned long    nBytes=0;

	if ( ReadFile(m_hCOM, tmpBuf, Length, &nBytes, NULL) )
	{
		memcpy(Buffer, tmpBuf, nBytes);
	}

	return (DWORD)nBytes;
}

/////////////////////////////////////////////////////////////////////////////
//
DWORD	Comm_Port::WriteCommBlock(UCHAR *Buffer, int Length)
{
//	DWORD	nBytes=0;
	unsigned long    nBytes=0;

	if ( !WriteFile( m_hCOM, Buffer, Length, &nBytes, NULL) || Length!=(int)nBytes )
	{
		nBytes=0;
	}

	return (DWORD)nBytes;
}
/////////////////////////////////////////////////////////////////////////////
#if NONE_BLOCK_TYPE	        
int Comm_Port::ReadUntil(char *Buffer, char cTarget)
{
	int i, nIndex = 0;
	unsigned char acBuff[MAX_COMM_PORT_STREAM] = "";
	unsigned long    nBytes=0;

	DWORD	dwRead = 0;
	DWORD	dwRes = 0;
	BOOL fWaitingOnRead = FALSE;
	OVERLAPPED osReader = {0};

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(osReader.hEvent==NULL)
	{
		return 0;
	}

	if(!ReadFile(m_hCOM, acBuff, MAX_COMM_PORT_STREAM, &nBytes, &osReader))
	{
		if(GetLastError()!=ERROR_IO_PENDING)
			return 0;
		else
		{
			dwRes = WaitForSingleObject(osReader.hEvent, 0);
	
			switch(dwRes)
			{
			case WAIT_OBJECT_0:
				if(!GetOverlappedResult(m_hCOM, &osReader, &dwRead, FALSE))
				{
					return 0;	// 통신 이상 처리
				}
				else
					break; //Read successful
			case WAIT_TIMEOUT:
				//Can do another job
			default:
				//Error occured in WaitForSingleObject function
				return 0;

			}
		}
	}
	else
	{
		//Read successful

	}


	//bool bSuccess =  ReadFile(m_hCOM, acBuff, MAX_COMM_PORT_STREAM, &nBytes, &osReader);
	if(nBytes==0)
	{
		PurgeComm(m_hCOM, PURGE_RXCLEAR);
		return 0;
	}

	for(i = 0; i < nBytes; i++) 
	{
		*(Buffer + nIndex ) = acBuff[i];
		nIndex++;

		if(acBuff[i] == cTarget)	return nIndex;
	}
	return 0;
}
#else
int Comm_Port::ReadUntil(unsigned char *Buffer, char cTarget, bool bCheckCount)//cTarget ==ETX
{

	int i = 0, nIndex = 0;
	unsigned char acBuff[MAX_COMM_PORT_STREAM] = "";
	unsigned long    nBytes=0;

	if(bCheckCount==false)
	{
		bool bSuccess =  ReadFile(m_hCOM, acBuff, MAX_COMM_PORT_STREAM, &nBytes, NULL);
		if(bSuccess==false || nBytes==0)
		{
			//PurgeComm(m_hCOM, PURGE_RXCLEAR);
			return 0;
		}

		for(i = 0; i < nBytes; i++)
		{
			*(Buffer + nIndex ) = acBuff[i];
			nIndex++;

			if(acBuff[i] == cTarget)	return nIndex;
		}
	}
	else //이어붙이기 해줄놈. E418 에러관련해서 문제되는 부분 3회까지 시도.
	{
		for(int k=0; k < 3; k++)       //두번째 통신 시도 시, 8회 시도   soming
		{
			bool bSuccess =  ReadFile(m_hCOM, acBuff, MAX_COMM_PORT_STREAM, &nBytes, NULL);
		   	if(bSuccess==false || nBytes==0)
			{
				//PurgeComm(m_hCOM, PURGE_RXCLEAR);
				return 0;
			}

			for(i = 0; i < nBytes; i++)
			{
				*(Buffer + nIndex ) = acBuff[i];
				nIndex++;

				if(acBuff[i] == cTarget)	return nIndex;
			}

			Sleep(10);

		}
	}

	return nIndex;
}
int Comm_Port::ReadUntilCount_SOS_AreaIndex(unsigned char *Buffer)
{
	int i = 0, nIndex = 0, nCheckCount = 0;
	unsigned char acBuff[30] = {0,};
	unsigned long nBytes=0;
 //	SOS_RECV_LOG("ReadUntilCount_SOS_AreaIndex START");
	bool bSuccess =  ReadFile(m_hCOM, acBuff, 30, &nBytes, NULL);
	SOS_RECV_LOG("ReadUntilCount_SOS_AreaIndex, bSuccess = %d, nBytes = %d , acBuff Size = %d", bSuccess, nBytes, sizeof(acBuff));

	if(bSuccess==false || nBytes==0)
	{
		PurgeComm(m_hCOM, PURGE_RXCLEAR);
		return 0;
	}
	memcpy(Buffer, acBuff, sizeof(acBuff));

//	SOS_RECV_LOG("ReadUntilCount_SOS_AreaIndex END");
	return nBytes;
}


int Comm_Port::ReadUntilCount_SOS_GetArea(unsigned char *Buffer)
{
	int i = 0, nIndex = 0, nCheckCount = 0;
	unsigned char acBuff[2200] = {0,};      //test용 Max    16384
	unsigned long nBytes=0;
	SOS_RECV_LOG("ReadUntilCount_SOS_GetArea START");
	bool bSuccess =  ReadFile(m_hCOM, acBuff,2200, &nBytes, NULL);
	SOS_RECV_LOG("ReadUntilCount_SOS_GetArea, bSuccess = %d, nBytes = %d , acBuff Size = %d", bSuccess, nBytes, sizeof(acBuff));

	if(bSuccess==false || nBytes==0)
	{
		PurgeComm(m_hCOM, PURGE_RXCLEAR);
		return 0;
	}
	memcpy(Buffer, acBuff, sizeof(acBuff));

	SOS_RECV_LOG("ReadUntilCount_SOS_GetArea END");
	return nBytes;
}

int Comm_Port::ReadUntilCount_SOS_GetFrame(unsigned char *Buffer, int nTargetSize)
{
	int i = 0, nIndex = 0, nCheckCount = 0;
	unsigned char acBuff[4036] = {0,};      //test용 Max    16384
	unsigned long nBytes=0;


	for(int k=0; k < 20; k++)
	{
			bool bSuccess =  ReadFile(m_hCOM, acBuff, 4036, &nBytes, NULL);
			SOS_RECV_LOG("ReadUntilCount_SOS_GetFrame, bSuccess = %d, nBytes = %d ,Index : %d, acBuff Size = %d", bSuccess, nBytes, nIndex, sizeof(acBuff));

			if(k==0)
			{
			 	if(bSuccess==false || nBytes==0)
				{
					//PurgeComm(m_hCOM, PURGE_RXCLEAR);
					return 0;
				}

			}

			for(i = 0; i < nBytes; i++)
			{
				*(Buffer + nIndex ) = acBuff[i];
				nIndex++;

				if(nIndex == nTargetSize)
				{
					if(!((Buffer[0] == 0xc3) && (Buffer[1] == 0x51) && (Buffer[2] == 0xa1) && (Buffer[3] == 0xf8)))
					{
						SOS_RECV_LOG("ReadUntilCount_SOS_GetFrame, Buff = %x %x %x %x", Buffer[0], Buffer[1], Buffer[2], Buffer[3]);
						return -1;
				 	}

					return nIndex;
				}
			}

			Sleep(20);

	}

	return nIndex;
}
// 2016-12-03, Add by Labeler, 수신 Data 내에 cTarget Data가 여러개 있는 경우 처리 위해, 함수 추가
int Comm_Port::ReadUntilCountOHTDetect(unsigned char *Buffer, char cTarget, int nCnt)
{
	int i = 0, nIndex = 0, nCheckCount = 0;
	unsigned char acBuff[MAX_COMM_PORT_STREAM] = "";
	unsigned long    nBytes=0;

	bool bSuccess =  ReadFile(m_hCOM, acBuff, MAX_COMM_PORT_STREAM, &nBytes, NULL);
	//ADD_LOG("ReadLinkerUart%d%s%d nBytes:%d",bSuccess,"/",m_hCOM, nBytes);
	if(bSuccess==false || nBytes==0)
	{
		PurgeComm(m_hCOM, PURGE_RXCLEAR);
		return 0;
	}

	for(i = 0; i < nBytes; i++)
	{
		*(Buffer + nIndex ) = acBuff[i];
		nIndex++;
		//ADD_LOG("%d %c",i, acBuff[i]);
		if(acBuff[i] == cTarget)    nCheckCount++;

        if(nCheckCount == nCnt)	return nIndex;
	}
	return nIndex;
}

// 2016-12-03, Add by Labeler, 수신 Data 내에 cTarget Data가 여러개 있는 경우 처리 위해, 함수 추가
int Comm_Port::ReadUntilCountOHTDetect2(unsigned char *Buffer, char cTarget, int nCnt)
{
	int i = 0, nIndex = 0, nCheckCount = 0;
	unsigned char acBuff[2048] = "";
	unsigned long    nBytes=0;

	bool bSuccess =  ReadFile(m_hCOM, acBuff, 2048, &nBytes, NULL);
	//ADD_LOG("ReadLinkerUart%d%s%d nBytes:%d",bSuccess,"/",m_hCOM, nBytes);
	if(bSuccess==false || nBytes==0)
	{
		PurgeComm(m_hCOM, PURGE_RXCLEAR);
		return 0;
	}

	for(i = 0; i < nBytes; i++)
	{
		*(Buffer + nIndex ) = acBuff[i];
		nIndex++;
		//ADD_LOG("%d %c",i, acBuff[i]);
		if(acBuff[i] == cTarget)    nCheckCount++;

        if(nCheckCount == nCnt)	return nIndex;
	}
	return nIndex;
}

// 2016-12-03, Add by Labeler, 수신 Data 내에 cTarget Data가 여러개 있는 경우 처리 위해, 함수 추가
int Comm_Port::ReadUntilCount(unsigned char *Buffer, char cTarget, int nCnt)
{
	int i = 0, nIndex = 0, nCheckCount = 0;
	unsigned char acBuff[MAX_COMM_PORT_STREAM] = "";
	unsigned long    nBytes=0;

	bool bSuccess =  ReadFile(m_hCOM, acBuff, MAX_COMM_PORT_STREAM, &nBytes, NULL);
//    ADD_LOG("ReadLinkerUart%d%s%d nBytes:%d",bSuccess,"/",m_hCOM, nBytes);
	if(bSuccess==false || nBytes==0)
	{
		//PurgeComm(m_hCOM, PURGE_RXCLEAR);
		return 0;
	}

	for(i = 0; i < nBytes; i++)
	{
		*(Buffer + nIndex ) = acBuff[i];
		nIndex++;
//		ADD_LOG("%d %c",i, acBuff[i]);
		if(acBuff[i] == cTarget)    nCheckCount++;

		if(nCheckCount == nCnt)	return nIndex;
	}

	Sleep(100);

    bSuccess =  ReadFile(m_hCOM, acBuff, MAX_COMM_PORT_STREAM, &nBytes, NULL);
//    ADD_LOG("ReadLinkerUart%d%s%d nBytes:%d",bSuccess,"/",m_hCOM, nBytes);
	if(bSuccess==false || nBytes==0)
	{
		//PurgeComm(m_hCOM, PURGE_RXCLEAR);
		return 0;
	}

	for(i = 0; i < nBytes; i++)
	{
		*(Buffer + nIndex ) = acBuff[i];
		nIndex++;
//		ADD_LOG("%d %c",i, acBuff[i]);
		if(acBuff[i] == cTarget)    nCheckCount++;

		if(nCheckCount == nCnt)	return nIndex;
	}

	return -1;
}

#endif
int Comm_Port::ReadLinkerUart(unsigned char *Buffer, char cTarget)
{
	int i = 0, nIndex = 0;
	unsigned char acBuff[1024] = "";
	unsigned long    nBytes=0;
	unsigned long    nBytes_2=0;

	memset(acBuff, 0, 1024);	// 이거 꼭 필요한데 없어서 넣음.

	// 1단계. 읽을것이 없으면 그냥 빠져나간다
	bool bSuccess =  ReadFile(m_hCOM, acBuff, 1024, &nBytes, NULL);
	if(bSuccess==false || nBytes==0)
	{
		PurgeComm(m_hCOM, PURGE_RXCLEAR);
		return 0;
	}

	// 2단계. retry 처리
	for(i = 0; i < 5; i++)
	{
		if (nBytes < 17)
		{
			Sleep(3);
			bool bSuccess =  ReadFile(m_hCOM, &acBuff[nBytes], (1024-nBytes), &nBytes_2, NULL);
			nBytes = nBytes + nBytes_2;
		}
		else
		{
			break;
		}
	}

#if 0
	// retry가 얼마나 나오는지 확인해 보기위한 곳. 테스트해보면, 최대 2회 까지 나온다)
	if(i > 1)
	{
		i++;
	}
#endif
	// 상위 함수로 결과값을 되돌린다
	for(i = 0; i < nBytes; i++)
	{
		*(Buffer + nIndex ) = acBuff[i];
		nIndex++;

		if((acBuff[i] == cTarget))
		{
			return nIndex;
		}
	}

	return nIndex;
}

int Comm_Port::ReadLCDUart(unsigned char *Buffer, char cTarget)
{
	int i = 0, nIndex = 0;
	unsigned char acBuff[1024] = "";
	unsigned long    nBytes=0;

	bool bSuccess =  ReadFile(m_hCOM, acBuff, 1024, &nBytes, NULL);
	if(bSuccess==false || nBytes==0)
	{
		PurgeComm(m_hCOM, PURGE_RXCLEAR);
		return 0;
	}

	for(i = 0; i < nBytes; i++)
	{
		*(Buffer + nIndex ) = acBuff[i];
		nIndex++;

		if((acBuff[i] == cTarget))
		{
			return nIndex;
		}
	}


	return nIndex;
}

int Comm_Port::ReadLCDUart(unsigned char *Buffer)
{
	int i = 0, nIndex = 0;
	unsigned char acBuff[1025] = "";
	unsigned long    nBytes=0;

	bool bSuccess =  ReadFile(m_hCOM, acBuff, 1024, &nBytes, NULL);
	if(bSuccess==false || nBytes==0)
	{
		PurgeComm(m_hCOM, PURGE_RXCLEAR);
		return 0;
	}

	for(i = 0; i < nBytes; i++)
	{
		*(Buffer + nIndex ) = acBuff[i];
		nIndex++;

		if(i == 1024)
		{
			return nIndex;
		}
	}


	return nIndex;
}


/////////////////////////////////////////////////////////////////////////////


