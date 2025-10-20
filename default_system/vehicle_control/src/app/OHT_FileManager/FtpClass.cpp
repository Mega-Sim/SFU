// ---------------------------------------------------------------------------

#pragma hdrstop

#include "FtpClass.h"
#ifdef _IN_UDP_UPPER_FORM
	#include "Udp.h"
#endif
// ---------------------------------------------------------------------------

#pragma package(smart_init)

FtpClass::FtpClass()
{
	// Indy Ftp 생성
	IndyFtpClient = new TIdFTP(NULL);
}

FtpClass::~FtpClass()
{
	delete IndyFtpClient;
}

int FtpClass::Connect(AnsiString strHost, int iPort, AnsiString UnserName,
	AnsiString strPassword, bool bPassive)
{
	if (IndyFtpClient->Connected())
	{
		return CONN_ALREADY;
	}

	try
	{
		IndyFtpClient->Host = strHost;
		IndyFtpClient->Port = iPort;
		IndyFtpClient->Username = UnserName;
		IndyFtpClient->Password = strPassword;
		IndyFtpClient->Passive = bPassive;
		IndyFtpClient->DataPortProtection = 0; //기본값 0   1설정해도  sFTP 접속 안됨.
		IndyFtpClient->UseTLS =0;              //1설정해도 sFTP 접속 안됨.

		//IndyFtpClient->UsingSFTP
//		IndyFtpClient->p
		IndyFtpClient->ConnectTimeout = 2000;   //기본값 0
		IndyFtpClient->ListenTimeout = 2000;    //기본값 10000
		IndyFtpClient->ReadTimeout  = 2000;     //기본값 60000 IP주소 틀릴시 응답이 느린경우 시간제한 설정


		IndyFtpClient->Connect();
		frmUDP->DisplayLog("FtpClass::Connect");

		IndyFtpClient->IOHandler->DefStringEncoding = TIdTextEncoding_UTF8;
	}
	#if 1
	catch (Exception &exception)
	{
		// IP Address exception이 바로 걸릴때가 있고 시간이 지나야 걸리는 IP가 있음.
		// 바로 걸리는 IP > 231.23.23.23
		// 오래 걸리는 IP > 127.1.0.1
		// AnsiString tmp;
		//UnicodeString tmp;
		char tmp[100],strWarning[100]={0};
		int i,j;
		char* strLoginWarning;

		strLoginWarning = "User cannot log in.";
		frmUDP->DisplayLog("[FtpClass] User cannot log in.");
		//tmp.sprintf(tmp1, exception.Message);
		memcpy(tmp,exception.Message.c_str(), 100);
		IndyFtpClient->ConnectTimeout = 0;   //기본값 0
		IndyFtpClient->ListenTimeout = 10000;    //기본값 10000
		IndyFtpClient->ReadTimeout  = 60000;	//기본값 60000

		for( i=0, j=0 ; i<100 ; i++ )
		{
			if(tmp[i]==0x0)
			{
				//NOP
			}
			else
			{
				strWarning[j]=tmp[i];
				j++;
			}
		}

		if (memcmp(strLoginWarning,strWarning,19)==0)
		{
			frmUDP->DisplayLog("[FtpClass] CONN_LOGIN_INFO_FAIL");
			return CONN_LOGIN_INFO_FAIL;
		}
		else
		{
			frmUDP->DisplayLog("[FtpClass] CONN_SOCKET_ERROR");
			return CONN_SOCKET_ERROR;
		}
	}
	#endif
	IndyFtpClient->ConnectTimeout = 0;   //기본값 0
	IndyFtpClient->ListenTimeout = 10000;    //기본값 10000
	IndyFtpClient->ReadTimeout  = 60000;	//기본값 60000
	if (IndyFtpClient->Connected())
	{
		return CONN_SUCCESS;
	}
	else
	{
		return CONN_UNKNOWN_FAIL;
	}
}

void FtpClass::DisConnect()
{
	IndyFtpClient->Quit();
	IndyFtpClient->Disconnect();

	return;
}

int FtpClass::MoveDirectory(AnsiString strDir)
{
	int iResult = NO_ERR;

	try
	{
		IndyFtpClient->ChangeDir(strDir);
		iResult = NO_ERR;
	}
	catch (...)
	{
		iResult = ERR_DIRECTORY;
	}

	return iResult;
}

void FtpClass::GetFileList(TStrings* TSfileList, TStrings* TSdirList)
{
	int iSeprator;
	UnicodeString strName;

	TStrings* ListBox1;
	ListBox1 = new TStringList();

	IndyFtpClient->TransferType = ftASCII;
	IndyFtpClient->List(ListBox1, "", false);

	TSfileList->Clear();
	TSdirList->Clear();

	int iSize = 0;
	// IndyFtpClient->Size()
	for (int Count = 0; Count < ListBox1->Count; Count++)
	{
		strName = ListBox1->Strings[Count];
		iSize = IndyFtpClient->Size(strName);
		// IndyFtpClient->DirectoryListing->Items[Count]->
		if (iSize != -1)
			TSfileList->Add(strName);
		else
			TSdirList->Add(strName);
	}

#if 0
	for (int Count = 0; Count < ListBox1->Count; Count++)
	{
		strName = SapName(IndyFtpClient->DirectoryListing->Items[Count]->FileName, iSeprator);
		 // IndyFtpClient->DirectoryListing->Items[Count]->
		if(iSeprator == IS_FILE)
			TSfileList->Add(strName);
		else
			TSdirList->Add(strName);
	}
#endif

#if 0
	for (int i = 0; i < IndyFtpClient->DirectoryListing->Count; i++)
		if (IndyFtpClient->DirectoryListing->Items[i]->ItemType == ditDirectory)
			TSdirList->Add(IndyFtpClient->DirectoryListing->Items[i]->FileName);


	// files
	for (int i = 0; i < IndyFtpClient->DirectoryListing->Count; i++)
		if (IndyFtpClient->DirectoryListing->Items[i]->ItemType == ditFile)
			TSfileList->Add(IndyFtpClient->DirectoryListing->Items[i]->FileName);

	delete ListBox1;
#endif
}

UnicodeString FtpClass::SapName(UnicodeString strName, int& iSeprator)
{
	UnicodeString strTmp;
	UnicodeString strNameChk;
	int iPos;

	strTmp = strName[1];
	strTmp = strTmp.UpperCase();

	strName = strName.Trim();
	iPos = strName.LastDelimiter(L" ");
	strNameChk = strName.Delete(1, iPos);

	if (strTmp == "D")
		iSeprator = IS_DIRECTORY;
	else
		iSeprator = IS_FILE;

	return strNameChk;
}

// 인수의 예 : Download("setup.log", "D:\\setup.log");
int FtpClass::Download(AnsiString strServerFileName,
	AnsiString strLocalFileName)
{
	int iResult = NO_ERR;
	IndyFtpClient->TransferType = ftBinary;
	IndyFtpClient->ConnectTimeout = 2000;   //기본값 0
	IndyFtpClient->ListenTimeout = 2000;    //기본값 10000
	IndyFtpClient->ReadTimeout  = 2000;     //기본값 60000 IP주소 틀릴시 응답이 느린경우 시간제한 설정
    IndyFtpClient->TransferTimeout = 2000;
    IndyFtpClient->ReadTimeout = 2000;


	try
	{
		frmUDP->DisplayLog("[Download] Start");
		IndyFtpClient->Get(strServerFileName, strLocalFileName);
		iResult = NO_ERR;
		frmUDP->DisplayLog("[Download] End");
	}
	catch (...)
	{
		iResult = ERR_DOWNLOAD;
		frmUDP->DisplayLog("[Download] Error");
	}
	return iResult;
}

// 인수의 예 : Upload"C:\\test.txt", "mytest.txt");
int FtpClass::Upload(AnsiString strLocalFileName, AnsiString strServerFileName)
{
	int iResult = NO_ERR;
	IndyFtpClient->TransferType = ftBinary;

	// IndyFtpClient->Put()
	// 옵션 true 설정시 업로드 파일 용량 증가,
	// 옵션 해제(false) 후 업로드 시 이동식 디스크 저장 보안 발생하나 업로드는 완료됨.
	// IndyFtpClient->Put(strLocalFileName, strServerFileName, true);
	try
	{
		IndyFtpClient->Put(strLocalFileName, strServerFileName);
		iResult = NO_ERR;
	}
	catch(...)
	{
		iResult = ERR_UPLOAD;
	}
	return iResult;
}

AnsiString FtpClass::GetCurrentDir()
{
	return IndyFtpClient->RetrieveCurrentDir();
}


int FtpClass::DirectCommand(AnsiString strCommand)
{
	int iResult;

	iResult = IndyFtpClient->SendCmd(strCommand);
	return iResult;
}

