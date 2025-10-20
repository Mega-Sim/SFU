//---------------------------------------------------------------------------
#ifndef CommHandlerH
#define CommHandlerH
//---------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <conio.h>

#define		MAX_COMM_PORT_STREAM		1024
#define		MAX_COMM_PORT_STREAM_LINKER	64


/***********************************************************************************/
// Class Name : Comm_Port
/***********************************************************************************/
class	Comm_Port
{
 private:
 protected:
 public:
   HANDLE		m_hCOM;

    Comm_Port(int port_no);	// Open Port
    Comm_Port(int port_no, int nBaudRate);
    Comm_Port(int port_no, int nBaudRate, BYTE byByte ,BYTE byParity, BYTE byStopBits); // NOPARITY, ODDPARITY, EVENPARITY  // ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS
	~Comm_Port();

   void	SetCommParam(int Baud, int Data, int Stop, int Parity);
	DWORD	ReadCommBlock(char *Buffer, int Length);
	DWORD	WriteCommBlock(char *Buffer, int Length);
	DWORD	ReadCommBlock(UCHAR *Buffer, int Length);
	DWORD	WriteCommBlock(UCHAR *Buffer, int Length);
   //#################################################
   int   ReadUntil(unsigned char *Buffer, char cTarget, bool bCheckCount = false);
   int   ReadUntilCount(unsigned char *Buffer, char cTarget, int nCnt);
   int   ReadUntilCountOHTDetect(unsigned char *Buffer, char cTarget, int nCnt);
   int   ReadUntilCountOHTDetect2(unsigned char *Buffer, char cTarget, int nCnt);
   int   ReadLinkerUart(unsigned char *Buffer, char cTarget);
   int	 ReadLCDUart(unsigned char *Buffer, char cTarget);
};

#endif
