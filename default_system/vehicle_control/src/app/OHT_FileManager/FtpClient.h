//---------------------------------------------------------------------------

#ifndef FtpClientH
#define FtpClientH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdExplicitTLSClientServerBase.hpp>
#include <IdFTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <Dialogs.hpp>
#include <Sockets.hpp>
#include <Keyboard.hpp>
#include <ExtCtrls.hpp>
#include "Protocol_Define_Modify.h"

//---------------------------------------------------------------------------
class FtpClient
{
private:	// User declarations
public:		// User declarations
	FtpClient();
	~FtpClient();
	int FTPConnect(PKT_DATA_TRANSPER_ORDER_OCS2OHT UDP_PKT_Order);
	int FTPConnect(AnsiString strHost, AnsiString UnserName, AnsiString strPassword);
	void FTPDisConnect(void);
	UINT FTPDownload(char* pcFTPPath, unsigned char CMDType);
	UINT FTPUpload(char* pcFTPPath, unsigned char CMDType);
	UINT FTPPathCheck(char* pcFTPPath);
	UINT FTPFileCheck(char* pcFTPPath,unsigned char CMDType);
	void FTPClientWorkEnd(void);
	UINT Compression(void);
	UINT Decompression(void);
	UINT FirmwareUpdateDecompress(void);
	UINT EraseMapsetFiles(void);
	UINT FindMapsetFiles(void);
	UINT CopyMapsetFiles(void);
	UINT CheckFileSizeMapset(void);
	int CheckFileSize(TCHAR *FileName, UINT structsize);
	int DirectCommand(AnsiString strCommand);
	int DirectUpload(AnsiString strLocalFileName, AnsiString strServerFileName);

	AnsiString strAnsiOriginalName[8];
};
//---------------------------------------------------------------------------
extern FtpClient *fFtpClient;
//---------------------------------------------------------------------------
#endif
