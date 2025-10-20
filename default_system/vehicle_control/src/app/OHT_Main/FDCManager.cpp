//---------------------------------------------------------------------------


#include "FDCManager.h"
#include "Def_OHT.h"
#include "crc16.h"
#include "OHTMain.h"

#define	INITIALIZE_CRITICAL_SECTION		InitializeCriticalSection(&m_CS)
#define	ENTER_CRITICAL_SECTION			EnterCriticalSection(&m_CS)
#define	LEAVE_CRITICAL_SECTION			LeaveCriticalSection(&m_CS)
#define	DELETE_CRITICAL_SECTION			DeleteCriticalSection(&m_CS)

#define	MIN_TOTAL_FDC_BUFFER_BYTE_SIZE	4096

#define FDC_PORT_NO 					9000
#define CRC_LENGTH 						5

FDCManager::FDCManager(unsigned int nMaxByteSizeOfFDCSum, bool checkFDCFolder)
{
    static FDCManager *pInstance;

    INITIALIZE_CRITICAL_SECTION;
	m_nByteSizeOfFDCSum		= 0;
	m_nMaxByteSizeOfFDCSum	= 0;
	m_bFDCUse = false;
	memset(m_bFDCConnectIP, 0x00, sizeof(m_bFDCConnectIP));
	m_bFDCServerConnect = false;

	m_dwServerConnectTryTime = timeGetTime();

	m_checkFDCFolder = checkFDCFolder;

	m_FDCUnitMap.clear();

	// Buffer MAX 사이즈 설정
	if( nMaxByteSizeOfFDCSum > MIN_TOTAL_FDC_BUFFER_BYTE_SIZE )
	{
		m_nMaxByteSizeOfFDCSum = nMaxByteSizeOfFDCSum;
	}
	else
	{
		m_nMaxByteSizeOfFDCSum = MIN_TOTAL_FDC_BUFFER_BYTE_SIZE;
	}

	m_pParamSet = new FDC_PARAM_SET();
	LoadParameter("DataFiles\\FDC.ini", m_pParamSet); // m_ParamSet

	//Server 접속
	m_UdpClient = new UdpClient(m_pParamSet->FDC_Param.sIPAddress, m_pParamSet->FDC_Param.nPortNo);
	sprintf(m_bFDCConnectIP,"%s",m_pParamSet->FDC_Param.sIPAddress);
	m_UdpClient->SocketStatus();
}


FDCManager::~FDCManager()
{
	// 생성된 로그 유닛 소멸
	FDCUnit* pFDCUnit = NULL;
	int nCountOfFDCUnit = m_FDC_List.getCount();
	m_FDCUnitMap.clear();

	Sleep(3000);

	for(int n=0; n<nCountOfFDCUnit; n++)
	{
		pFDCUnit = (FDCUnit*)m_FDC_List.popFront();
		delete pFDCUnit;
		pFDCUnit = NULL;
	}

	delete m_UdpClient;
	delete m_pParamSet;

		// 크리티컬 섹션 소멸
	DELETE_CRITICAL_SECTION;
}

bool	FDCManager::connectFDCUnit(FDCUnit *pFDCUnit)
{
	bool	bReturn = false;

	ENTER_CRITICAL_SECTION;

	if((m_nByteSizeOfFDCSum )>m_nMaxByteSizeOfFDCSum)	// 로그 용량이 관리 총용량을 초과할 경우 미생성
	{
		bReturn = false;
	}
	else
	{
		bReturn = true;
		m_FDC_List.insertBack((void *) pFDCUnit);
		m_nByteSizeOfFDCSum+=pFDCUnit->m_nMaxSizeOfFDC;
	}
	LEAVE_CRITICAL_SECTION;

	return bReturn;
}


bool  FDCManager::FDC_Socket_Connection()
{
	if(m_UdpClient != NULL)
	{
		delete m_UdpClient;
	}

	m_UdpClient = new UdpClient(m_pParamSet->FDC_Param.sIPAddress, m_pParamSet->FDC_Param.nPortNo);
	sprintf(m_bFDCConnectIP,"%s",m_pParamSet->FDC_Param.sIPAddress);

	if( m_UdpClient->SocketStatus())
	{
		return true;
	}

	return false;
}


bool  FDCManager::handleFDC_Transmit_Fun()
{
	FILE			*fp;

	int 			nTempCount;	// 현재까지의 Count 를 받아온다.

	FDCUnit 		*pFDCUnit = NULL;

	char			szTemp[256]		= {0,};
	char 			strTmpFullName[40] ={0,};
	int 			strLen = 1;

	SYSTEMTIME CurTime;

	// by zzang9un 2013.03.20 : 현재 시간 얻어오기
	GetLocalTime(&CurTime);

	nTempCount = m_FDC_List.getCount();	// 현재까지의 Count 를 받아온다.

	if(nTempCount < 1)
		return false;
//1	Socket 연결 확인 -> 재 연결
	if(!m_UdpClient->SocketStatus())
	{
		FDC_Socket_Connection();
	}

//3 Data 전송 -> Ack 확인 -> Retry;

	pFDCUnit = (FDCUnit*)m_FDC_List.refer(0);

	while(strLen > -1)
	{
		if(m_UdpClient->SocketStatus())
		{
			strLen = m_UdpClient->Receive(szTemp, sizeof(szTemp));
		}
		else
		{
        	strLen = -1;
        }

		if(strLen == -1)
			break;

		try
		{
			sprintf(strTmpFullName, "D:\\log\\FDC%04d%02d%02d%02d.log", CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour);
			// 파일을 연다
			fp = fopen(strTmpFullName, "a");
			Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할

			fprintf(fp, "[%02d:%02d:%02d.%03d]%s\n",
							CurTime.wHour,
							CurTime.wMinute,
							CurTime.wSecond,
							CurTime.wMilliseconds,
							szTemp);

			// 파일을 닫는다
			fflush(fp);
			Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
			fclose(fp);
			Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
			fp = NULL;

			FDC_Transmit_Rcv(pFDCUnit,szTemp);
		}
		catch (...)
		{
			fflush(fp);
			Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
			fclose(fp);
			Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
			fp = NULL;

		}
	}

	if(m_bFDCServerConnect)
		FDC_Transmit_Snd(pFDCUnit);

	return true;
}



bool  FDCManager::FDC_Check_Transmit(char *Msg,char *szTemp, int *msgLength)
{
	int 	Cnt;
	char    szTemp2[256]		= {0,};
	unsigned short 	cCRC16_header;
	int 	TempSize;
	memset(szTemp, 0, sizeof(char)*256);

	if(FDC_Server_Connection_Rcv(Msg))
		return false;

	for(Cnt=0;Cnt<120;Cnt++)
	{
		szTemp[Cnt] = Msg[Cnt];
    	if(Msg[Cnt]==':')
			break;
	}

	for(TempSize=0 ; TempSize < 256 ; TempSize++)
	{
		if(szTemp[TempSize] == '\0')
			break;
	}
	cCRC16_header = crc16_ccitt((const char *)szTemp,TempSize);
	sprintf(szTemp2, "%s%04x", szTemp, cCRC16_header);

	memcpy(szTemp, szTemp2, TempSize + 5);

	for(int i=0;i<5;i++)
	{
		if(szTemp2[Cnt] != Msg[Cnt++])
			return false;
	}

	for(Cnt=0;Cnt<120;Cnt++)
	{
		if(szTemp2[Cnt+12]==':')
		{
			Msg[Cnt] = '\0';
			break;
		}
		Msg[Cnt] = szTemp2[Cnt+12];
	}

	*msgLength = Cnt -1;

	return true;
}

FDCUnit*  FDCManager::getFDCUnit(char *szSavePath, unsigned int nMaxSizeOfLog)
{
	char szFullSavePath[_MAX_PATH]={0,};
	char* pFullPathResult = NULL;
	pFullPathResult = _fullpath(szFullSavePath, szSavePath, _MAX_PATH);

	FDCUnit* pFDCUnit = NULL;
	std::string strSavePath = std::string(szFullSavePath);
	std::map<std::string, FDCUnit*>::iterator IterPos;

	if(pFullPathResult!=NULL)
	{
		IterPos = m_FDCUnitMap.find(strSavePath);
		if(IterPos == m_FDCUnitMap.end())	// 정보 없으면
		{
			pFDCUnit = new FDCUnit(szFullSavePath, nMaxSizeOfLog);
			connectFDCUnit(pFDCUnit);
			m_FDCUnitMap[strSavePath] = pFDCUnit;		//m_LogUnitMap.insert(std::map<std::string, LogUnit*>::value_type(strSavePath, pLogUnit));
		}
		else
		{
			pFDCUnit = (*IterPos).second;
		}
	}

	return pFDCUnit;
}


bool  FDCManager::handleFDCSum()
{
	if(m_bFDCUse == true)
	{
	handleFDC_Transmit_Fun();

		if((m_bFDCServerConnect == false) && (OHTUTILITY::PreventOverflowGetTimeMs(m_dwServerConnectTryTime) > m_pParamSet->FDC_Param.nFDC_Server_Connect_Try_Time * 60 * 1000))
		{
			FDC_Server_Connection_Snd(ConnectionRequest);
			m_dwServerConnectTryTime = timeGetTime();
		}
	}
	return true;
}

bool FDCManager::LoadParameter(char* szFilePath, FDC_PARAM_SET* pParamSet)
{
	bool bSuccess = false;
	int nTmpValue = 0;
	int nResult = 0;
	double dTmpValue = 0.0;
	bool bTmpValue = false;
	double dResult = 0.0;
	int nErrorCode = 0;

	char szTmpValue[512];
	memset(szTmpValue, 0x00, sizeof(szTmpValue));

	LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile(szFilePath);

    if(!pIniFile->GetIniFileExist())
	{
        delete pIniFile;
        pIniFile = NULL;
    }

	if(pIniFile != NULL)
	{
		pIniFile->ReadBool("PARAM", "bUseFDC", false, &bTmpValue);
		pParamSet->FDC_Param.bUseFDC = bTmpValue;

		pIniFile->ReadString("PARAM", "sIPAddress", "", szTmpValue);
		sprintf(pParamSet->FDC_Param.sIPAddress,"%s",szTmpValue);
		memset(szTmpValue, 0x00, sizeof(szTmpValue));

		pIniFile->ReadString("PARAM", "sOHTIPAddress", "", szTmpValue);
		sprintf(pParamSet->FDC_Param.sOHTIPAddress,"%s",szTmpValue);
		memset(szTmpValue, 0x00, sizeof(szTmpValue));

		pIniFile->ReadInteger("PARAM", "nPortNo", FDC_PORT_NO, &nTmpValue);
		pParamSet->FDC_Param.nPortNo = nTmpValue;

		pIniFile->ReadString("PARAM", "sFDCOHTType", "", szTmpValue);
		sprintf(pParamSet->FDC_Param.sFDC_OHT_Type,"%s",szTmpValue);
		memset(szTmpValue, 0x00, sizeof(szTmpValue));

		pIniFile->ReadString("PARAM", "sFDCOHTNo", "", szTmpValue);
		sprintf(pParamSet->FDC_Param.sFDC_OHT_No,"%s",szTmpValue);
		memset(szTmpValue, 0x00, sizeof(szTmpValue));

		pIniFile->ReadString("PARAM", "sFDCHWVersion", "", szTmpValue);
		sprintf(pParamSet->FDC_Param.sFDC_HW_Version,"%s",szTmpValue);
		memset(szTmpValue, 0x00, sizeof(szTmpValue));

		pIniFile->ReadInteger("PARAM", "nFDCRetryCountMax", 3, &nTmpValue);
		pParamSet->FDC_Param.nFDC_Retry_Count_Max = nTmpValue;

		pIniFile->ReadInteger("PARAM", "nFDCRetryTime", 5000, &nTmpValue);
		pParamSet->FDC_Param.nFDC_Retry_Time = nTmpValue;

		pIniFile->ReadInteger("PARAM", "nFDCWARNSendDelay", 5000, &nTmpValue);
		pParamSet->FDC_Param.nFDC_WARN_Send_Delay = nTmpValue;

		pIniFile->ReadInteger("PARAM", "nFDCServerConnectTryTime", 60, &nTmpValue);
		pParamSet->FDC_Param.nFDC_Server_Connect_Try_Time = nTmpValue;

		bSuccess = true;

		//데이터 유효성 검사
		if(strcmp(pParamSet->FDC_Param.sIPAddress,"") == 0)
		{
			nErrorCode = ERR_DIAGPARAM_INVALID_IP;
		}
		else if(pParamSet->FDC_Param.nPortNo == 0)
		{
			nErrorCode = ERR_DIAGPARAM_INVALID_PORT;
		}
		else if(strcmp(m_bFDCConnectIP, pParamSet->FDC_Param.sIPAddress) != 0)
		{
			FDC_Socket_Connection();
        }
	}
	else
	{
		//File Open Error
		nErrorCode = ERR_DIAGPARAM_READ_FAIL;

		pParamSet->FDC_Param.bUseFDC = false;
	}

	if(nErrorCode != 0)
	{
		delete pIniFile;
		pIniFile = NULL;
	}

	if(pParamSet->FDC_Param.bUseFDC == true)
	{
	FDCUnit *pFDCUnit;
        pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
		if(pFDCUnit != NULL)
		{
		m_bFDCUse = true;
	}
	}
	else if(pParamSet->FDC_Param.bUseFDC == false)
	{
		FDCUnit *pFDCUnit;
        pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
		if(pFDCUnit != NULL)
		{
				m_bFDCUse = false;
    }
    }


	return bSuccess;

}



bool FDCManager::FDC_Transmit_Snd(FDCUnit *pFDCUnit)
{
	FILE			*fp;
	LList       	TmpList;
	int 			nTmpError;
	int 			nCountOfFDC;
	int				TempSize;
    int 			strLen;
	unsigned short 	cCRC16_header;

	char 			strTmpFullName[40] ={0,};
	char			szTemp[256]		= {0,};
	char			szTemp2[256]		= {0,};

	int nCountOfLostLog	= 0;

	FDC_PACKET 		*pFDCPacket = NULL;

	SYSTEMTIME CurTime;
	// by zzang9un 2013.03.20 : 현재 시간 얻어오기
	GetLocalTime(&CurTime);

	static DWORD dwWARNUpdateTime = timeGetTime();

	pFDCUnit->FDC_get(&TmpList, &nTmpError, &nCountOfLostLog);

	nCountOfFDC = TmpList.getCount();       //List 안에 들어있는 FDC의 수

	if(nCountOfFDC > 0)
	{
        LoadParameter("DataFiles\\FDC.ini", m_pParamSet); // m_ParamSet

		sprintf(strTmpFullName, "D:\\log\\FDC%04d%02d%02d%02d.log", CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour);
		// 파일을 연다
		fp = fopen(strTmpFullName, "a");
		Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
		while(nCountOfFDC > 0)
		{
			try
			{
			if(TmpList.getCountsize() > 0)
			{
				pFDCPacket = (FDC_PACKET *)TmpList.popFront();

				if(((pFDCPacket->pcValue[0] == 'E') && (pFDCPacket->pcValue[1] == 'R') && (pFDCPacket->pcValue[2] == 'R'))
					&& (pFDCUnit->getOCSErrorcode() != pFDCPacket->nErrorcode))
				{
					delete[] pFDCPacket->pcValue;	//@todo: 문제없는지 확인 필요 delete[]
					delete pFDCPacket;
				}
				else
				{
					if(pFDCPacket->pcValue != NULL)
					{
						if((pFDCPacket->nTransmit_Count == 0) || (OHTUTILITY::PreventOverflowGetTimeMs(pFDCPacket->dwTransmitTime) > m_pParamSet->FDC_Param.nFDC_Retry_Time))
						{
							memset(szTemp, 0, sizeof(char)*256);
							memset(szTemp2, 0, sizeof(char)*256);
							if(pFDCPacket->nLength < 256)
							{
								memcpy(szTemp2, pFDCPacket->pcValue, pFDCPacket->nLength);
								sprintf(szTemp,"VHL/%s/%s/%s/%03u_%03u_%03u_%03u/%04d%02d%02d%02d%02d%02d/%s/%d/%s",
												m_pParamSet->FDC_Param.sFDC_OHT_Type,
												m_pParamSet->FDC_Param.sFDC_OHT_No,
												m_pParamSet->FDC_Param.sFDC_HW_Version,
												(BYTE)OHTMainForm->VerInfo.Major,
												(BYTE)OHTMainForm->VerInfo.Minor,
												(BYTE)OHTMainForm->VerInfo.Release,
												(BYTE)OHTMainForm->VerInfo.Build,
												pFDCPacket->LocalTime.wYear,
												pFDCPacket->LocalTime.wMonth,
												pFDCPacket->LocalTime.wDay,
												pFDCPacket->LocalTime.wHour,
												pFDCPacket->LocalTime.wMinute,
												pFDCPacket->LocalTime.wSecond,
												m_pParamSet->FDC_Param.sOHTIPAddress,
												pFDCPacket->nTransmit_Count,
												szTemp2);

								for(TempSize=0 ; TempSize < 256 ; TempSize++)
								{
									if(szTemp[TempSize] == '\0')
										break;
								}
								cCRC16_header = crc16_ccitt((const char *)szTemp,TempSize);
								memset(szTemp2, 0, sizeof(char)*256);
								sprintf(szTemp2, "%s:%04x",szTemp,cCRC16_header);
								TempSize += CRC_LENGTH;

								//Server로 Message 전송
								if(m_UdpClient->SocketStatus())
								{
									strLen = m_UdpClient->Send(szTemp2, TempSize);
								}
								else
								{
									fprintf(fp, "[%04d%02d%02d%02d%02d%02d] UDP Connect Error",
												pFDCPacket->LocalTime.wYear,
												pFDCPacket->LocalTime.wMonth,
												pFDCPacket->LocalTime.wDay,
												pFDCPacket->LocalTime.wHour,
												pFDCPacket->LocalTime.wMinute,
												pFDCPacket->LocalTime.wSecond);
								}
								fprintf(fp, "[%02d:%02d:%02d.%03d]%s\n",
								CurTime.wHour,
								CurTime.wMinute,
								CurTime.wSecond,
								CurTime.wMilliseconds,
								szTemp2);

								pFDCUnit->Transmitted(pFDCPacket);
								pFDCPacket->dwTransmitTime = timeGetTime();
							}
						}

						if(pFDCUnit->Transmit_Count(pFDCPacket) < m_pParamSet->FDC_Param.nFDC_Retry_Count_Max)
							pFDCUnit->Transmit_Insert(pFDCPacket);
						else
						{
							delete[] pFDCPacket->pcValue;	//@todo: 문제없는지 확인 필요 delete[]
							delete pFDCPacket;

							pFDCUnit->changeNoUseMode();
							m_bFDCServerConnect = true;
						}
					}
					else
					{
                       	delete[] pFDCPacket->pcValue;	//@todo: 문제없는지 확인 필요 delete[]
						delete pFDCPacket;
                    }
				}
			}
			}
			catch (...)
			{
				fflush(fp);
				Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
				fclose(fp);
				Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
				fp = NULL;

			}
			nCountOfFDC--;

			Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할


		}
		// 손실된 로그가 있다면 개수를 기록한다
		if(nCountOfLostLog > 0)
		{
			fprintf(fp, "[%02d:%02d:%02d.%03d] CountOfLostLog:%d\n",
						CurTime.wHour,
						CurTime.wMinute,
						CurTime.wSecond,
						CurTime.wMilliseconds,
						nCountOfLostLog);
			nCountOfLostLog = 0;
			//Sleep(0);	// Context Switching
		}

		// 파일을 닫는다
		fflush(fp);
		Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
		fclose(fp);
		Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
		fp = NULL;

	}

	if(OHTUTILITY::PreventOverflowGetTimeMs(dwWARNUpdateTime) > m_pParamSet->FDC_Param.nFDC_WARN_Send_Delay * 60 * 1000)
	{
		pFDCUnit->WARN_Insert();
		dwWARNUpdateTime = timeGetTime();
	}

	return true;

}


bool FDCManager::FDC_Transmit_Rcv(FDCUnit *pFDCUnit, char *MSG)
{
	LList       	TmpList;
	int 			nCountOfFDC;
    int 			nTmpError;
	int 			nCount = 0;
	int 			msgLength;

	char			szTemp[256]		= {0,};
	char			szTemp2[20]		= {0,};
	char			szTemp3[256]		= {0,};

	int nCountOfLostLog	= 0;

	FDC_PACKET 		*pFDCPacket = NULL;

	if(FDC_Check_Transmit(MSG, szTemp, &msgLength))
	{
		if(szTemp[0] == '0')
		{
			pFDCUnit->FDC_get(&TmpList,&nTmpError, &nCountOfLostLog);

			nCountOfFDC = TmpList.getCount();

			for(int nIndex=0 ; nIndex<nCountOfFDC ; nIndex++)
			{
                try
				{
				if(TmpList.getCountsize() > 0)
				{

					pFDCPacket = (FDC_PACKET *)TmpList.popFront();

					for(int nIndex2=0; nIndex2<20 ; nIndex2++)
					{
						  if( pFDCPacket->pcValue[nIndex2] == '/')
							nCount++;
						  if(nCount == 3)
						  {
							nCount = 0;
							break;
						  }
						  szTemp2[nIndex2] = pFDCPacket->pcValue[nIndex2];
					}


					memset(szTemp3, 0, sizeof(char)*256);
					sprintf(szTemp3,"VHL/%s/%s/%s/%03u_%03u_%03u_%03u/%04d%02d%02d%02d%02d%02d/%s/%d/%s",
									m_pParamSet->FDC_Param.sFDC_OHT_Type,
									m_pParamSet->FDC_Param.sFDC_OHT_No,
									m_pParamSet->FDC_Param.sFDC_HW_Version,
									(BYTE)OHTMainForm->VerInfo.Major,
									(BYTE)OHTMainForm->VerInfo.Minor,
									(BYTE)OHTMainForm->VerInfo.Release,
									(BYTE)OHTMainForm->VerInfo.Build,
									pFDCPacket->LocalTime.wYear,
									pFDCPacket->LocalTime.wMonth,
									pFDCPacket->LocalTime.wDay,
									pFDCPacket->LocalTime.wHour,
									pFDCPacket->LocalTime.wMinute,
									pFDCPacket->LocalTime.wSecond,
									m_pParamSet->FDC_Param.sOHTIPAddress,
									pFDCPacket->nTransmit_Count-1,szTemp2);

					if(strncmp(szTemp3, MSG, msgLength) == 0)
					{
						delete[] pFDCPacket->pcValue;	//@todo: 문제없는지 확인 필요 delete[]
						delete pFDCPacket;
					}
					else
					{
						pFDCUnit->Transmit_Insert(pFDCPacket);
					}
				}
                }
				catch (...)
				{
                	TmpList.getCountsize();
				}
			}
		}
	}

	return true;

}

void FDCManager::FDC_Manager_Off()
{
	m_bFDCUse = false;
	FDCUnit *pFDCUnit;
	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	if(pFDCUnit != NULL)
	{
	pFDCUnit->changeNoUseMode();
}
}

void FDCManager::FDC_Server_Connection_Snd(short cmd)
{
	LoadParameter("DataFiles\\FDC.ini", m_pParamSet); // m_ParamSet
	FDC_Socket_Connection();

	PKT_OHT2FDC_COMM SEND_PACKET;

	char	szTemp[64]		= {0,};
	AnsiString strTmp = "";
	AnsiString strMsg = "";
	int		strLen = 0;

	memset(szTemp, 0, sizeof(char)*64);

	FILE			*fp;
	SYSTEMTIME CurTime;
	char 			strTmpFullName[40] ={0,};
	GetLocalTime(&CurTime);

	int iPacketSize = sizeof(PKT_OHT2FDC_COMM);

	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER = 101;
	memset(SEND_PACKET.MachineID, 0x20, sizeof(SEND_PACKET.MachineID));
	memcpy(SEND_PACKET.MachineID,(char*)m_pParamSet->FDC_Param.sFDC_OHT_No,6);
	SEND_PACKET.CommandID = cmd;
	ConvertEndian(&SEND_PACKET.HEADER);
	ConvertEndian(&SEND_PACKET.CommandID);

	memcpy(szTemp,(char*)&SEND_PACKET,38);

	for(int TempSize=0 ; TempSize < 38 ; TempSize++)
	{
		strTmp.sprintf("%02X", szTemp[TempSize]);

		strMsg += strTmp;
	}

	if(m_UdpClient->SocketStatus())
	{
		m_UdpClient->Send((char*)&SEND_PACKET, 38);

		// 파일을 연다
		sprintf(strTmpFullName, "D:\\log\\FDCSndTest%04d%02d%02d%02d.log", CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour);

		fp = fopen(strTmpFullName, "a");
		Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할

		fprintf(fp, "[%02d:%02d:%02d.%03d]%s\n",
						CurTime.wHour,
						CurTime.wMinute,
						CurTime.wSecond,
						CurTime.wMilliseconds,
						strMsg);

		// 파일을 닫는다
		fflush(fp);
		Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
		fclose(fp);
		Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
		fp = NULL;
	}
}

bool FDCManager::FDC_Server_Connection_Rcv(char *Msg)
{
 	//통신개시 메세지 확인
	PKT_OHT2FDC_COMM RCV_PACKET;
	int iPacketSize = sizeof(PKT_OHT2FDC_COMM);
	char	szTemp[64]		= {0,};
	char	szTemp2[64]		= {0,};

	memset(&RCV_PACKET, 0, iPacketSize);

	RCV_PACKET.HEADER = 9999;
	memset(RCV_PACKET.MachineID, 0x20, sizeof(RCV_PACKET.MachineID));
	sprintf(szTemp2,"FDS1");
	memcpy(RCV_PACKET.MachineID,szTemp2,4);
	RCV_PACKET.CommandID = ConnectionReply;
	ConvertEndian(&RCV_PACKET.HEADER);
	ConvertEndian(&RCV_PACKET.CommandID);

	memcpy(szTemp,(char*)&RCV_PACKET,38);

	// 파일을 연다

	int		TempSize = 0;
	AnsiString strTmp = "";
	AnsiString strMsg = "";
	AnsiString strMsg2 = "";
	int		strLen = 0;

	FILE			*fp;
	SYSTEMTIME CurTime;
	char 			strTmpFullName[40] ={0,};
	GetLocalTime(&CurTime);

	memcpy(szTemp2,(char*)&RCV_PACKET,iPacketSize);

	sprintf(strTmpFullName, "D:\\log\\FDCRcvTest%04d%02d%02d%02d.log", CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour);

	fp = fopen(strTmpFullName, "a");
	Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할

	for(TempSize=0 ; TempSize < 38 ; TempSize++)
	{
		if(Msg[TempSize] == '\0')
		{
			Msg[TempSize] = 0x20;
			szTemp[TempSize] = 0x20;
			szTemp2[TempSize] = 0x20;
		}

		strTmp.sprintf("%02X", szTemp[TempSize]);

		strMsg += strTmp;
	}

	fprintf(fp, "[%02d:%02d:%02d.%03d]rcv!%s\n",
					CurTime.wHour,
					CurTime.wMinute,
					CurTime.wSecond,
					CurTime.wMilliseconds,
					strMsg);

	// 파일을 닫는다
	fflush(fp);
	Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
	fclose(fp);
	Sleep(0);	// Context Switching 가능하도록 공간을 비워두는 역할
	fp = NULL;

	if(strncmp(Msg, szTemp, 38) == 0)
	{
		FDCUnit *pFDCUnit;
		pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
		if(pFDCUnit != NULL)
		{
			pFDCUnit->changeUseMode();
			m_bFDCServerConnect = true;
		}
		return false;
	}

	RCV_PACKET.CommandID = ConnectionRequest;
	ConvertEndian(&RCV_PACKET.CommandID);

	memcpy(szTemp,(char*)&RCV_PACKET,38);

	if(strncmp(Msg, szTemp, 38) == 0)
	{
		FDCUnit *pFDCUnit;
		pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
		if(pFDCUnit != NULL)
		{
			pFDCUnit->changeUseMode();
			m_bFDCServerConnect = true;
			FDC_Server_Connection_Snd(ConnectionReply);
		}
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
