//---------------------------------------------------------------------------

#ifndef FtpClassH
#define FtpClassH
//---------------------------------------------------------------------------
#include <vcl.h>
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
//---------------------------------------------------------------------------

// Connect Defines
#define CONN_SUCCESS          (0)
#define CONN_ALREADY          (-1)
#define CONN_UNABLE           (-2)
#define CONN_UNKNOWN_FAIL     (-3)
#define CONN_LOGIN_INFO_FAIL  (-4)
#define CONN_SOCKET_ERROR     (-5)

// FileType Defines
#define IS_FILE       (0)
#define IS_DIRECTORY	(1)

// DisConnect Defines
#define DISCONN_OK		(0)
#define DISCONN_FAIL	(-1)

// DisConnect Defines
#define NO_ERR          (0)
#define ERR_DIRECTORY	  (-1)
#define ERR_FILES     	(-2)
#define ERR_UPLOAD      (-3)
#define ERR_DOWNLOAD    (-4)

//---------------------------------------------------------------------------
class FtpClass
{
	private:
		TIdFTP *IndyFtpClient;
		UnicodeString SapName(UnicodeString strName, int& iSeprator);
		
	public:
		FtpClass();
		~FtpClass();
		int  Connect(AnsiString strHost, int iPort, AnsiString UnserName,
					AnsiString strPassword, bool bPassive);
		void DisConnect();
		int MoveDirectory(AnsiString strDir);
		void GetFileList(TStrings* TSfileList, TStrings* TSdirList);
		int Download(AnsiString strServerFileName, AnsiString strLocalFileName);
		int Upload(AnsiString strLocalFileName, AnsiString strServerFileName);
		AnsiString GetCurrentDir();
		int DirectCommand(AnsiString strCommand);
};

#endif
