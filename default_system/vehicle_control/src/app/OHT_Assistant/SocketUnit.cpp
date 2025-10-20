//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SocketUnit.h"
#include "MonitoringUtil.h"
#include "FileControl.h"
#include "ProcessTerminator.h"
#include "AssistantMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define ADD_COMM_LOG(STR)   m_Log->WriteLogFile("Comm", STR)
#define ADD_SOCKET_LOG(STR)	m_Log->WriteLogFile("Socket", STR)
#define STR_SEND            "[Snd]"
#define STR_RECV            "[Rcv]"

#define ADD_TEST_LOG(STR)	m_Log->WriteLogFile("Test", STR)

TSocketForm *SocketForm;
//---------------------------------------------------------------------------
__fastcall TSocketForm::TSocketForm(TComponent* Owner)
	: TForm(Owner)
{
	m_Log = new OHTLogFile;

	// by zzang9un 2013.10.10 : Socket 설정
	m_ServerSocketDM->Port = PORT_ASSISTANT_DM;
//	m_ServerSocketDM->Active = true;

	m_ServerSocketTP->Port = PORT_TP_ASSISTANT;
    m_ServerSocketTP->Active = true;

    m_bDMConnected = false;

    // TLV 관련 변수 초기화
	InitBigTLV_DM();
	InitBigTLV_TP();
}
//---------------------------------------------------------------------------

/**
@brief   TP Sockek의 Read 이벤트 발생 시 호출되는 함수
@author  zzang9un
@date    2013.04.11
*/
void __fastcall TSocketForm::TPSocketClientRead(TObject *Sender, TCustomWinSocket *Socket)
{
	int nError = 0;
	int nRecvBytes;
	int nTotalLenRead = 0;  // Read한 총 Bytes 길이
	int nLenBytesToRead;    // 읽어야할 Bytes 길이
	int WaitCount;          // 대기 횟수

	int BodyLen = 0; // Body 크기

	char RecvBuf[LEN_TLV_BUFFER]; // Header 내용이 저장될 버퍼

	String strLog;  // Log를 위한 변수

	// TLV를 처음 Read하는 경우인지 연결된 TLV인지를 체크
	if((bBigTLV_TP == false) && (m_PreCmdHeader_TP.Length == 0))
    {
		// -------------------------------------------
        // [BEGIN] by zzang9un 2013.10.15 : TLV를 처음 읽는 경우

		// Step 1. TLV의 Header를 읽어온다.
		// 버퍼의 길이를 읽어온다.
        WaitCount = 0;
		while(1)
        {
			nRecvBytes = Socket->ReceiveLength();

			if(nRecvBytes >= LEN_HEADER)
                break;

			if(++WaitCount > MAX_WAIT)
				break;

			Sleep(1);
		}

		// 에러 처리
        if(nRecvBytes < LEN_HEADER)
		{
			// 에러 로그 생성
            strLog.sprintf(L"[Error] nRecvByte(%d) Less than LEN_HEADER(%d)", nRecvBytes, LEN_HEADER);
			ADD_COMM_LOG(strLog);
			return;
        }

		// 버퍼에서 Header를 Read
        nRecvBytes = Socket->ReceiveBuf(RecvBuf, LEN_HEADER);

		// 에러 처리
        if(nRecvBytes < LEN_HEADER)
        {
			// 에러 로그 생성
            strLog.sprintf(L"[Error] nRecvByte(%d) Less than LEN_HEADER(%d)", nRecvBytes, LEN_HEADER);
			ADD_COMM_LOG(strLog);
            return;
		}

		// Header를 구조체에 저장
        CMD_HEADER RcvHeader;
		RcvHeader = GetHeader(RecvBuf);

		// Step 2. TLV의 Body를 읽어온다.
        // Body의 길이를 추출
        // Endian 방식에 따라 Body Length 추출 방법이 달라짐
		BodyLen = (BYTE)RecvBuf[4] | (RecvBuf[5] << 8) ;

        // Step 3. Socket의 버퍼 크기와 TLV의 크기를 비교하여 BigTLV인지 아닌지를 구분한다.
		// (Body + LEN_HEADER + LEN_TAIL)의 크기가 Socket 버퍼의 크기보다 큰 지 비교
        if((BodyLen + LEN_HEADER + LEN_TAIL) > LEN_TLV_BUFFER)
        {
			// -------------------------------------------
            // [BEGIN] by zzang9un 2013.10.15 : Socket 버퍼(8192)보다 큰 TLV의 경우

			// Socket 버퍼보다 큰 Body가 올 경우를 대비해 flag와 현재 Cmd header를 저장
            bBigTLV_TP = true;
            m_PreCmdHeader_TP = RcvHeader;

            // 필요한 크기만큼 동적 버퍼를 생성
            m_BigTLVTotalLen_TP = LEN_HEADER + BodyLen + LEN_TAIL;
			pBigTLV_TP = new char[m_BigTLVTotalLen_TP];

            // 이미 읽은 Header를 pBigTLV_TP 버퍼로 옮긴다.
			memcpy(pBigTLV_TP, RecvBuf, LEN_HEADER);
            m_BigTLVReadCnt_TP = LEN_HEADER; // 읽은 버퍼 수 반영

			// Body의 나머지를 읽어온다.
            WaitCount = 0;
            while(1)
			{
				nLenBytesToRead = Socket->ReceiveLength();

				// 첫 TLV는 Header를 제외한 크기와 비교
                if(nLenBytesToRead >= (LEN_TLV_BUFFER - LEN_HEADER))
                    break;

                if(++WaitCount > MAX_WAIT)
                    break;

                Sleep(1);
            }

            // 에러 처리
            if(nLenBytesToRead < (LEN_TLV_BUFFER - LEN_HEADER))
			{
                InitBigTLV_TP();

				// 에러 로그 생성
                strLog.sprintf(L"[Error][Body:%d] nLenBytesToRead(%d) Less than (LEN_TLV_BUFFER - LEN_HEADER)(%d)", nLenBytesToRead, BodyLen, (LEN_TLV_BUFFER - LEN_HEADER));
                ADD_COMM_LOG(strLog);
				return;
            }

			// 첫 TLV에서 Body를 버퍼에 Read함
            nRecvBytes = Socket->ReceiveBuf(pBigTLV_TP + LEN_HEADER, nLenBytesToRead);

			// 에러 처리
            if(nRecvBytes < nLenBytesToRead)
            {
				InitBigTLV_TP();

                // 에러 로그 생성
				strLog.sprintf(L"[Error] nRecvBytes(%d) Less than nLenBytesToRead(%d)", nRecvBytes, nLenBytesToRead);
                ADD_COMM_LOG(strLog);
                return;
			}

            m_BigTLVReadCnt_TP += nRecvBytes; // Read한 Byte를 반영
			// [END] by zzang9un 2013.10.15 : Socket 버퍼(8192)보다 큰 TLV의 경우
            // -------------------------------------------
        }
		else
		{
            // -------------------------------------------
			// [BEGIN] by zzang9un 2013.10.15 : Socket 버퍼(8192)보다 작은 TLV의 경우
			bBigTLV_TP = false;
            memset(&m_PreCmdHeader_TP, 0x00, sizeof(CMD_HEADER));

            // TLV Body를 읽어온다
            nLenBytesToRead = BodyLen + LEN_TAIL;

            WaitCount = 0;
            while(1)
			{
                nRecvBytes = Socket->ReceiveLength();

				if(nRecvBytes >= nLenBytesToRead)
                    break;

				if(++WaitCount > MAX_WAIT)
                    break;

				Sleep(1);
            }

			if(nRecvBytes < nLenBytesToRead)
            {
                // 에러 로그 생성
				strLog.sprintf(L"[Error] nRecvBytes(%d) Less than nLenBytesToRead(%d)", nRecvBytes, nLenBytesToRead);
                ADD_COMM_LOG(strLog);
                return;
			}

            nRecvBytes = Socket->ReceiveBuf(RecvBuf + LEN_HEADER, nLenBytesToRead);

            if(nRecvBytes < nLenBytesToRead)
            {
				// 에러 로그 생성
                strLog.sprintf(L"[Error] nRecvBytes(%d) Less than nLenBytesToRead(%d)", nRecvBytes, nLenBytesToRead);
                ADD_COMM_LOG(strLog);
				return;
            }

			// Body+Tail을 읽은 길이와 이미 읽은 header의 길이를 더함
			nTotalLenRead = LEN_HEADER + nRecvBytes;

			// 완료된지 체크 후 TLV 처리
            if((LEN_HEADER + nLenBytesToRead) != nTotalLenRead)
            {
				// 에러 로그 생성
				strLog.sprintf(L"[Error] Different (LEN_HEADER + nLenBytesToRead)(%d) and nTotalLenRead(%d)", (LEN_HEADER + nLenBytesToRead), nTotalLenRead);
                ADD_COMM_LOG(strLog);
				return;
            }
            else
			{
                // Tail 추출
                int Tail = (BYTE)RecvBuf[nTotalLenRead-4] |
							(RecvBuf[nTotalLenRead-3] << 8) |
                            (RecvBuf[nTotalLenRead-2] << 16) |
                            (RecvBuf[nTotalLenRead-1] << 24);

				// TLV의 Length와 Tail 비교
                if(Tail != BodyLen)
				{
                    // 에러 로그 생성
                    strLog.sprintf(L"[Error] Different Tail(%d) and BodyLen(%d)", (LEN_HEADER + nLenBytesToRead), nTotalLenRead);
					ADD_COMM_LOG(strLog);
                    return;
                }

                // by zzang9un 2013.10.10 : Data Manager 통신 로그
                AddTLVCommLog(STR_RECV, RecvBuf, nTotalLenRead);
			}

            // Step 4. 추출이 완료된 TLV는 처리하도록 함수 호출
			nError = HandleTPCmd(RecvBuf, Socket);

            // [END] by zzang9un 2013.10.15 : Socket 버퍼(8192)보다 작은 TLV의 경우
			// -------------------------------------------
        }
        // [END] by zzang9un 2013.10.15 : TLV를 처음 읽는 경우
		// -------------------------------------------
    }
    else
	{
		// -------------------------------------------
        // [BEGIN] by zzang9un 2013.10.15 : Big TLV를 계속 Read하는 경우

        // 수신해야 할 남은 Bytes를 계산한다.
        if((m_BigTLVTotalLen_TP - m_BigTLVReadCnt_TP) < LEN_TLV_BUFFER)
			nLenBytesToRead = m_BigTLVTotalLen_TP - m_BigTLVReadCnt_TP;
        else
            nLenBytesToRead = LEN_TLV_BUFFER;

		WaitCount = 0;
        while(1)
		{
            nRecvBytes = Socket->ReceiveLength();

			if(nRecvBytes >= nLenBytesToRead)
                break;

			if(++WaitCount > MAX_WAIT)
                break;

			Sleep(1);
		}

		// 에러 처리
		if(nRecvBytes < nLenBytesToRead)
		{
            InitBigTLV_TP();

			// 에러 로그 생성
            strLog.sprintf(L"[Error] nRecvBytes(%d) Less than nLenBytesToRead(%d)", nRecvBytes, nLenBytesToRead);
            ADD_COMM_LOG(strLog);

            return;
        }

        nRecvBytes = Socket->ReceiveBuf(pBigTLV_TP + m_BigTLVReadCnt_TP, nLenBytesToRead);

		// 에러 처리
        if(nRecvBytes < nLenBytesToRead)
        {
			InitBigTLV_TP();

			// 에러 로그 생성
			strLog.sprintf(L"[Error] nRecvBytes(%d) Less than nLenBytesToRead(%d)", nRecvBytes, nLenBytesToRead);
            ADD_COMM_LOG(strLog);

			return;
        }

		// Read한 bytes 수 반영
        m_BigTLVReadCnt_TP += nRecvBytes;

		// 완료된지 체크 후 TLV 처리
        if(m_BigTLVTotalLen_TP == m_BigTLVReadCnt_TP)
		{
			// Tail 추출
            int Tail = (BYTE)pBigTLV_TP[m_BigTLVTotalLen_TP-4] |
                        (pBigTLV_TP[m_BigTLVTotalLen_TP-3] << 8) |
						(pBigTLV_TP[m_BigTLVTotalLen_TP-2] << 16) |
                        (pBigTLV_TP[m_BigTLVTotalLen_TP-1] << 24);

			// TLV의 Length와 Tail을 비교한다.
            if(Tail != m_PreCmdHeader_TP.Length)
            {
				delete [] pBigTLV_TP;
				memset(&m_PreCmdHeader_TP, 0x00, sizeof(CMD_HEADER));
				bBigTLV_TP = false;
                return;
            }

            // by zzang9un 2013.10.10 : Data Manager 통신 로그
            AddTLVCommLog(STR_RECV, RecvBuf, nTotalLenRead);

            //ACKData = m_CmdManager->ParsingOCSCmd(RecvBuf, true);

			// Header에 따라 처리
            nError = HandleTPCmd(pBigTLV_TP, Socket);

			// Cmd 처리 후에 동적 할당한 Buffer는 delete
            delete [] pBigTLV_TP;
            memset(&m_PreCmdHeader_TP, 0x00, sizeof(CMD_HEADER));
			bBigTLV_TP = false;
		// [END] by zzang9un 2013.10.15 : Big TLV를 계속 Read하는 경우
		// -------------------------------------------
		}
	}
}

void __fastcall TSocketForm::TPSocketClientConnect(TObject *Sender, TCustomWinSocket *Socket)
{
	g_bIsTPServerConnected = true;
}

// ---------------------------------------------------------------------------
void __fastcall TSocketForm::TPSocketClientDisconnect(TObject *Sender, TCustomWinSocket *Socket)
{
	g_bIsTPServerConnected = false;
}

// ---------------------------------------------------------------------------
void __fastcall TSocketForm::TPSocketClientError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
    ErrorCode = 0;

}

// ---------------------------------------------------------------------------

/**
 @brief   Ack Data를 버퍼에 encoding 하는 함수
 @author  zzang9un
 @date    2013.04.11
 @param   SndACKData : 전송할 Ack Data
 @param   pSndBuf : 보낼 버퍼
 @return  전송할 버퍼의 bytes 수, 실패 시 에러 코드 리턴
 */
int TSocketForm::MakeACK2TP(ACK_DATA SndACKData, char *pSndBuf)
{
    int Error = NO_ERR;
	int SndTag = SndACKData.AckTagData.Data;
    int SndLen = 0;

	memset(pSndBuf, 0x0, sizeof(LEN_TLV_BUFFER));

	// Header 생성
	CMD_HEADER SndHeader;

	// by zzang9un 2013.01.25 : Length 6bytes로 고정
	SndHeader = MakeHeader(SndTag, 6); // Tag는 포함하지 않고 Body를 계산한다.

	SndLen = EncodeCmdACK(pSndBuf, &SndHeader, &SndACKData);

	return SndLen;
}
// ---------------------------------------------------------------------------

void __fastcall TSocketForm::DMSocketClientConnect(TObject *Sender, TCustomWinSocket *Socket)
{
    m_bDMConnected = true;

	ADD_COMM_LOG("Connect Data Manager...");
}
//---------------------------------------------------------------------------

void __fastcall TSocketForm::DMSocketClientDisconnect(TObject *Sender, TCustomWinSocket *Socket)
{
	m_bDMConnected = true;

	ADD_COMM_LOG("DisConnect Data Manager...");
}
//---------------------------------------------------------------------------

void __fastcall TSocketForm::DMSocketClientError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
	String strErrTxt;

	/*
	eeGeneral   : The socket received an error message that does not fit into any of
				   the following categories.
	eeSend      : An error occurred when trying to write to the socket connection.
	eeReceive   : An error occurred when trying to read from the socket connection.
	eeConnect   : A client connection request that has already been accepted could
				   not be completed.
    eeDisconnect: An error occurred when trying to close a connection.
	eeAccept    : A problem occurred when trying to accept a client connection request.
	*/

	switch(ErrorCode)
	{
	case eeGeneral:
		strErrTxt="The socket received an error message that does not fit into any of the following categories.";
        break;

	case eeSend:
        strErrTxt="An error occurred when trying to write to the socket connection.";
        break;

    case eeReceive:
        strErrTxt="An error occurred when trying to read from the socket connection.";
		break;

    case eeConnect:
		strErrTxt="A client connection request that has already been accepted could not be completed.";
        break;

	case eeDisconnect:
        strErrTxt="An error occurred when trying to close a connection.";
        break;

	case eeAccept:
        strErrTxt="A problem occurred when trying to accept a client connection request.";
		break;

    default:
		break;
	}

	ADD_COMM_LOG(strErrTxt);

	// Reset error code
	ErrorCode = 0;
}
//---------------------------------------------------------------------------

/**
@brief	소켓 Read 이벤트 개선
@remark	한번에 읽을 수 있는 바이트가 8192bytes 라서 이 이상의 TLV가 수신되면
		데이터를 여러번 받아야 함. 이를 처리하기 위한 알고리즘으로 구현
@author	doori.shin
@date	2015.05.12
*/
void __fastcall TSocketForm::DMSocketClientRead(TObject *Sender, TCustomWinSocket *Socket)
{
	int nError = 0;
	int nRecvLen = 0;							//Socket에서 실제로 읽은 데이터 길이
	char szRecvBuf[LEN_TLV_BUFFER] = {0,};      //Socket에서 데이터 가져오는 버퍼
	int nRecvBytes = Socket->ReceiveLength();	//Socket 버퍼에 있는 데이터 길이
	int WaitCount = 0;
	AnsiString strLog = "";
	int nBreakPoint = 0;                        //while문 탈출하기 위한 변수

	// Step 1. TVL받기 전 처리 작업
	//받은 데이터가 없는경우 버퍼를 비움
	if(nRecvBytes == 0)
	{
		Socket->ReceiveBuf(szRecvBuf, nRecvBytes);

		if(m_TLV_DM != NULL)
		{
			m_TLV_DM->InitTLV();
			delete m_TLV_DM;
			m_TLV_DM = NULL;
        }

		return;
	}

	//Socket 버퍼안에 여러개의 TLV가 있을 수 있으므로 루프로 처리
	//nBreakPoint는 혹시모를 무한루프발생 예방으로 넣음, 최대 8192번 반복함
	while(nBreakPoint < LEN_TLV_BUFFER)
	{
		// Step 2. TLV수신
		//처음 읽는 데이터인 경우
		if(m_TLV_DM == NULL)
		{
			//수신 버퍼의 크기가 적어도 헤더의 길이보다 큰지 확인
			while(1)
			{
				nRecvBytes = Socket->ReceiveLength();

				if(nRecvBytes >= LEN_HEADER)
					break;

				// 수신한 size가 Header보다 작은 경우 대기
				if(++WaitCount > MAX_WAIT)
					break;

				Sleep(WAITTIME);
			}

			//헤더 크기가 작으면 다음 Read이벤트때 처리함
			if(nRecvBytes < LEN_HEADER)
			{
				strLog.sprintf("[Error] nRecvByte(%d) Less than LEN_HEADER(%d)", nRecvBytes, LEN_HEADER);
				ADD_COMM_LOG(strLog);

				if(m_TLV_DM != NULL)
				{
					m_TLV_DM->InitTLV();
					delete m_TLV_DM;
					m_TLV_DM = NULL;
				}

				return;
			}

			nRecvLen = Socket->ReceiveBuf(szRecvBuf, LEN_HEADER);

			//헤더추출
			CMD_HEADER header = GetHeader(szRecvBuf, true);
			bool bIsBig = (header.Length > LEN_TLV_BUFFER) ? true : false;

			//TLV생성
			m_TLV_DM = new TLV(header.Length + LEN_HEADER + LEN_TAIL, bIsBig);
			memcpy(&m_TLV_DM->m_Header, &header, sizeof(CMD_HEADER));
		}
		//계속 받고있는 데이터인 경우
		else
		{
			//마지막 받을 데이터 외에 추가로 데이터가 있는 경우
			if(nRecvBytes > m_TLV_DM->GetRemainingData())
			{
				//남은 길이 만큼만 받는다.
				nRecvLen = Socket->ReceiveBuf(szRecvBuf, m_TLV_DM->GetRemainingData());
			}
			//아니면 다 받는다
			else
			{
				nRecvLen = Socket->ReceiveBuf(szRecvBuf, nRecvBytes);
            }
		}

		//데이터를 수신하고 현재까지 수신한 데이터를 저장
		int nReadBytes = m_TLV_DM->AppendTLVData(szRecvBuf, nRecvLen);

//		strLog.sprintf("Length : %d", nReadBytes);
//		ADD_COMM_LOG(strLog);

		// Step 3. TLV처리
		//마지막 데이터인지 확인
		if(m_TLV_DM->GetRemainingData() <= 0)
		{
			//TLVL 형식 중 마지막 Tail을 받아옴
			int nTail = GetBodyLength(m_TLV_DM->m_TLVBuffer + LEN_HEADER + m_TLV_DM->m_Header.Length, true);

			//Length와 Tail이 같은지 확인
			if(nTail == m_TLV_DM->m_Header.Length)
			{
				//실제 수신된 데이터길이와 Length와 비교
				if((nReadBytes - LEN_HEADER - LEN_TAIL) == m_TLV_DM->m_Header.Length)
				{
					//데이터 처리
					nError = HandleDMCmd(m_TLV_DM->m_TLVBuffer, Socket);
				}
				else
				{
					strLog = "";
					strLog.sprintf("[Error] Packet length is different to Data Length..[nReadByte:%d, TLV_Length:%d",
									nReadBytes, m_TLV_DM->m_Header.Length);
					ADD_TEST_LOG(strLog);
				}

				//초기화
				if(m_TLV_DM != NULL)
				{
					m_TLV_DM->InitTLV();
					delete m_TLV_DM;
					m_TLV_DM = NULL;
                }
			}
		}

		//Step 4. 종료 조건(Socket 버퍼에 더이상 데이터가 없는경우),  데이터가 있으면 while문 반복
		if(Socket->ReceiveLength() <= 0)
		{
			if(m_TLV_DM != NULL)
			{
				//초기화
				m_TLV_DM->InitTLV();
				delete m_TLV_DM;
				m_TLV_DM = NULL;
			}

			break; //End of while()
		}

		nBreakPoint++;

		//nBreakPoint가 8192면 문제가 있는 것이다. 코드 확인 필요
		if(nBreakPoint == LEN_TLV_BUFFER)
		{
			ADD_TEST_LOG("[Error] Fall in infinity loop.");
        }
	}
}


//---------------------------------------------------------------------------

int TSocketForm::HandleDMCmd(char *pRecvBuf, TCustomWinSocket *ClientSocket)
{
	int Error = NO_ERR;

	// 받은 Command에서 Header를 얻어옴
	CMD_HEADER RecvHeader;
	memset(&RecvHeader, 0x00, sizeof(CMD_HEADER));

    // Data Manager는 Endian 변환이 필요함
	RecvHeader = GetHeader(pRecvBuf, true);

	// 에러 처리
	if(RecvHeader.uTag.Info.Sender != TAG_SENDER_DM)
	{
		ADD_COMM_LOG("[Error][HandleDMCmd]Sender is not DataManager!");
		return -1;
	}

	switch(RecvHeader.uTag.Info.CmdData)
	{
	case TAG_CMDDATA_VERSIONINFO:
		ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_VERSIONINFO");

		// 버전 정보 전송
		Error = SendDataVerionInfo(pRecvBuf, ClientSocket);
        break;

	case TAG_CMDDATA_DATA_DOWNLOAD:
		ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_DOWNLOAD]");

		// Data File 전송
		Error = SendDataFile(pRecvBuf, ClientSocket);
		break;

    case TAG_CMDDATA_DATA_BAKCUP:
        ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_BAKCUP]");

		// Data File 전송
        Error = SendBackupDataFile(pRecvBuf, ClientSocket);
		break;

	case TAG_CMDDATA_DATA_UPLOAD:
		ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_UPLOAD]");

		// Data 파일 수신 요청
		Error = ReceiveDataFile(pRecvBuf);

        if(Error == NO_ERR)
			Error = SendACKDataUpload(pRecvBuf, ClientSocket);
		break;

	case TAG_CMDDATA_DATA_UPLOAD_MAP:
        ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_UPLOAD_MAP]");

		// Map 파일 수신
		Error = MakeTLV2File(pRecvBuf, DATA_TYPE_MAP);
        break;

    case TAG_CMDDATA_DATA_UPLOAD_STATION:
        ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_UPLOAD_STATION]");

        // Station 파일 수신
		Error = MakeTLV2File(pRecvBuf, DATA_TYPE_STATION);
		break;

    case TAG_CMDDATA_DATA_UPLOAD_TEACHING:
		ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_UPLOAD_TEACHING]");

        // Teaching 파일 수신
		Error = MakeTLV2File(pRecvBuf, DATA_TYPE_TEACHING);
        break;

	default:
        ADD_SOCKET_LOG("[Error][HandleDMCmd]Unknown TAG_CMDDATA!!");
    }

    return Error;
}
//---------------------------------------------------------------------------

int TSocketForm::SendDataVerionInfo(char *pRecvBuf, TCustomWinSocket *ClientSocket)
{
    int Error = NO_ERR;

	// Step 1. Data Type Decode
    int Offset = LEN_HEADER;

	BYTE DataType;

    DecodeBody(pRecvBuf, &Offset, &DataType);

    // Step 2. Data Type에 따라 Version을 가져옴
    DATA_VERSION_INFO DataVerInfo_Map;
	DATA_VERSION_INFO DataVerInfo_Station;
    DATA_VERSION_INFO DataVerInfo_Teaching;

	FILE_HEADER FileHeader;
	MAP_FILE_HEADER MapFileHeader;

	String strFileName;

    switch(DataType)
	{
	case DATA_TYPE_ALL:
		// 1. Map
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_MAP;
		Error = GetMapFileHeader(strFileName, &MapFileHeader);

		DataVerInfo_Map.DataType = DATA_TYPE_MAP;
		DataVerInfo_Map.ErrCode = Error;
		DataVerInfo_Map.UpdateTime = MapFileHeader.UpdateTime;
		DataVerInfo_Map.CurRevision = MapFileHeader.Revision;

        // 2. Station
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_STATION;
        Error = GetFileHeader(strFileName, &FileHeader);

		DataVerInfo_Station.DataType = DATA_TYPE_STATION;
        DataVerInfo_Station.ErrCode = Error;
        DataVerInfo_Station.UpdateTime = FileHeader.UpdateTime;
		DataVerInfo_Station.CurRevision = FileHeader.Revision;

        // 3. Teaching
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_TEACHING;
        Error = GetFileHeader(strFileName, &FileHeader);

		DataVerInfo_Teaching.DataType = DATA_TYPE_TEACHING;
        DataVerInfo_Teaching.ErrCode = Error;
        DataVerInfo_Teaching.UpdateTime = FileHeader.UpdateTime;
		DataVerInfo_Teaching.CurRevision = FileHeader.Revision;
        break;
    }

    // 보낼 버퍼 초기화
    char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

    // Tag 설정
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
    SndTag.Info.Receiver = TAG_RECEIVER_DM;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;
    SndTag.Info.CmdData = TAG_CMDDATA_VERSIONINFO;

    CMD_HEADER SndHeader;
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = LEN_ACK_DATA_VERSIONINFO;

    int BodySize = LEN_ACK_DATA_VERSIONINFO;

	ConvertEndian(&SndHeader.uTag.Data);
    ConvertEndian(&SndHeader.Length);
    memcpy(SndBuf, &SndHeader, LEN_HEADER);

    Offset = LEN_HEADER; // Offset 초기화

	// Body 인코딩
    switch(DataType)
    {
	case DATA_TYPE_ALL:
        // 1. Map
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Map.DataType);
		EncodeBody(SndBuf, &Offset, &DataVerInfo_Map.ErrCode);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Map.UpdateTime);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Map.CurRevision);

        // 2. Station
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Station.DataType);
		EncodeBody(SndBuf, &Offset, &DataVerInfo_Station.ErrCode);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Station.UpdateTime);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Station.CurRevision);

        // 3. Teaching
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Teaching.DataType);
		EncodeBody(SndBuf, &Offset, &DataVerInfo_Teaching.ErrCode);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Teaching.UpdateTime);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Teaching.CurRevision);
		break;
	}

	// Tail
    EncodeBody(SndBuf, &Offset, &BodySize);

	// 전송
    ClientSocket->SendBuf(SndBuf, Offset);

	// by zzang9un 2013.10.10 : Data Manager 통신 로그
    AddTLVCommLog(STR_SEND, SndBuf, Offset);

	return Error;
}
//---------------------------------------------------------------------------

int TSocketForm::SendDataFile(char *pRecvBuf, TCustomWinSocket *ClientSocket)
{
	int Error = NO_ERR;

    // Step 1. Data Type Decode
	int Offset = LEN_HEADER;

    BYTE DataType;

    DecodeBody(pRecvBuf, &Offset, &DataType);

	// Step 2. Data Type과 Size를 전송
    int FileSize = 0;

	String strFileName;

    switch(DataType)
	{
    case DATA_TYPE_MAP:
        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_MAP;
		FileSize = (int)GetLocalFileSize(strFileName);
        break;

	case DATA_TYPE_STATION:
        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_STATION;
        FileSize = (int)GetLocalFileSize(strFileName);
		break;

    case DATA_TYPE_TEACHING:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_TEACHING;
        FileSize = (int)GetLocalFileSize(strFileName);
        break;

    case DATA_TYPE_ALL:
        break;
	}

    // -------------------------------------------
	// [BEGIN] by zzang9un 2013.10.11 : 파일 정보 전송
    // 보낼 버퍼 초기화
    char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag 설정
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
    SndTag.Info.Receiver = TAG_RECEIVER_DM;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
    SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;
    SndTag.Info.CmdData = TAG_CMDDATA_DATA_DOWNLOAD;

    CMD_HEADER SndHeader;
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = LEN_ACK_DATAFILE_DOWNLOAD;

    int BodySize = LEN_ACK_DATAFILE_DOWNLOAD;
    int SndFileSize = FileSize;

    ConvertEndian(&SndHeader.uTag.Data);
    ConvertEndian(&SndHeader.Length);
	memcpy(SndBuf, &SndHeader, LEN_HEADER);

    Offset = LEN_HEADER;

    // Body 인코딩
    EncodeBody(SndBuf, &Offset, &DataType);
	EncodeBody(SndBuf, &Offset, &SndFileSize);

    // Tail 인코딩
	EncodeBody(SndBuf, &Offset, &BodySize);

    // ACK_DATAFILE_DOWNLOAD_AST_DM 명령 전송
	ClientSocket->SendBuf(SndBuf, Offset);

    // 통신 로그 생성
	AddTLVCommLog(STR_SEND, SndBuf, Offset);
    // [END] by zzang9un 2013.10.11 : 파일 정보 전송
    // -------------------------------------------

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.10.11 : 파일 전송

    // Header 변수  초기화
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

    // Tag 설정
    SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_DM;
    SndTag.Info.ProtoType = TAG_PROTOTYPE_REQ;
    SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;

    switch(DataType)
    {
	case DATA_TYPE_MAP:
        SndTag.Info.CmdData = TAG_CMDDATA_DATA_DOWNLOAD_MAP;
        break;

    case DATA_TYPE_STATION:
        SndTag.Info.CmdData = TAG_CMDDATA_DATA_DOWNLOAD_STATION;
		break;

    case DATA_TYPE_TEACHING:
		SndTag.Info.CmdData = TAG_CMDDATA_DATA_DOWNLOAD_TEACHING;
        break;
    }

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = FileSize;    // 전송하고자 하는 File Size
	BodySize = FileSize;

    // 파일 전송을 위한 버퍼 생성
	char *SndFileBuf;
    int SizeOfSndFileBuf = LEN_HEADER + FileSize + LEN_TAIL;
    SndFileBuf = new char[SizeOfSndFileBuf];

    // Header 인코딩
    ConvertEndian(&SndHeader.uTag.Data);
	ConvertEndian(&SndHeader.Length);
    memcpy(SndFileBuf, &SndHeader, LEN_HEADER);

	// 버퍼에 파일 내용 인코딩
    Error = MakeFile2TLV(SndFileBuf, strFileName, FileSize);

	// Tail 인코딩
    Offset = LEN_HEADER + BodySize;
    EncodeBody(SndFileBuf, &Offset, &BodySize);

    // 파일 전송
    ClientSocket->SendBuf(SndFileBuf, SizeOfSndFileBuf);

    // 통신 로그 생성
    String strLog;
	strLog.sprintf(L"[SendDataFile] File:%s, Size:%dBytes", strFileName.c_str(), FileSize);
    ADD_SOCKET_LOG(strLog);

	AddTLVCommLog(STR_SEND, SndFileBuf, SizeOfSndFileBuf);

	delete[] SndFileBuf;
	// [END] by zzang9un 2013.10.11 : 파일 전송
    // -------------------------------------------
}
//---------------------------------------------------------------------------

int TSocketForm::MakeFile2TLV(char *pBuf, String strFileName, int FileSize)
{
    setlocale(LC_ALL, "Korean");

	int Error = NO_ERR;

    ifstream fs;

    // File Open
    fs.open(strFileName.w_str(), ios::in | ios::binary);

    if(fs.is_open())
    {
		fs.read((char *)pBuf + LEN_HEADER, FileSize);
    }
	else
    {
        Error = ERR_FILE_OPEN;
	}

    fs.close();

	return Error;
}
//---------------------------------------------------------------------------

void TSocketForm::AddTLVCommLog(AnsiString strTitle, char *pBuf, int size)
{
	AnsiString strTmp;
	AnsiString strLog = strTitle;

	for(int i = 0; i < size; i++)
	{
		strTmp.sprintf("%02X", (BYTE)pBuf[i]);
		strLog += strTmp;
	}
	ADD_COMM_LOG(strLog);
}
//---------------------------------------------------------------------------

int TSocketForm::ReceiveDataFile(char *pRecvBuf)
{
	int Error = NO_ERR;

	// Step 1. Data Type Decode
	int Offset = LEN_HEADER;

	BYTE DataType;

	DecodeBody(pRecvBuf, &Offset, &DataType);
	DecodeBody(pRecvBuf, &Offset, &m_FileSize);

	String strLog;
	strLog.sprintf(L"[ReceiveDataFile] DataType:%d, FileSize:%dBytes", DataType, m_FileSize);
	ADD_SOCKET_LOG(strLog);

    return Error;
}
//---------------------------------------------------------------------------

int TSocketForm::MakeTLV2File(char *pBuf, BYTE DataType)
{
    setlocale(LC_ALL, "Korean");

	int Error = NO_ERR;

    ofstream fs;

    // 파일 이름 결정
    String strFileName;

	switch(DataType)
    {
	case DATA_TYPE_MAP:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_UPDATE + FILENAME_MAP;
		break;

    case DATA_TYPE_STATION:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_UPDATE + FILENAME_STATION;
		break;

    case DATA_TYPE_TEACHING:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_UPDATE + FILENAME_TEACHING;
        break;
    }

    // File Open
    fs.open(strFileName.w_str(), ios::out | ios::binary);

	if(fs.is_open())
	{
		fs.write((char *)pBuf + LEN_HEADER, m_FileSize);
	}
    else
	{
		Error = ERR_FILE_OPEN;
    }

	fs.close();

    return Error;
}
//---------------------------------------------------------------------------
int TSocketForm::SendACKDataUpload(char * pRecvBuf, TCustomWinSocket *ClientSocket)
{
	int Error = NO_ERR;

	// Step 1. Data Type Decode
	int Offset = LEN_HEADER;

	BYTE DataType;

	DecodeBody(pRecvBuf, &Offset, &DataType);

    // Step 2. Data Type과 Size를 전송
//	int FileSize = 0;
//
//	String strFileName;

//    switch(DataType)
//    {
//    case DATA_TYPE_MAP:
//        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_MAP;
//        FileSize = (int)GetLocalFileSize(strFileName);
//        break;
//
//    case DATA_TYPE_STATION:
//        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_STATION;
//        FileSize = (int)GetLocalFileSize(strFileName);
//        break;
//
//    case DATA_TYPE_TEACHING:
//        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_TEACHING;
//        FileSize = (int)GetLocalFileSize(strFileName);
//        break;
//
//    case DATA_TYPE_ALL:
//        break;
//    }

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.10.11 : 파일 정보 전송
    // 보낼 버퍼 초기화
    char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

    // Tag 설정
    uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
    SndTag.Info.Receiver = TAG_RECEIVER_DM;
    SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
    SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;
    SndTag.Info.CmdData = TAG_CMDDATA_DATA_UPLOAD;

	CMD_HEADER SndHeader;
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = LEN_ACK_DATAFILE_UPLOAD;

    int BodySize = LEN_ACK_DATAFILE_UPLOAD;
//    int SndFileSize = FileSize;

	ConvertEndian(&SndHeader.uTag.Data);
    ConvertEndian(&SndHeader.Length);
    memcpy(SndBuf, &SndHeader, LEN_HEADER);

    Offset = LEN_HEADER;

    // Body 인코딩
    EncodeBody(SndBuf, &Offset, &DataType);

    // Tail 인코딩
    EncodeBody(SndBuf, &Offset, &BodySize);

	// ACK_DATAFILE_DOWNLOAD_AST_DM 명령 전송
    ClientSocket->SendBuf(SndBuf, Offset);

    // 통신 로그 생성
    AddTLVCommLog(STR_SEND, SndBuf, Offset);
    // [END] by zzang9un 2013.10.11 : 파일 정보 전송
    // -------------------------------------------

    return Error;
}
//---------------------------------------------------------------------------

int TSocketForm::SendBackupDataFile(char *pRecvBuf, TCustomWinSocket *ClientSocket)
{
    int Error = NO_ERR;

    // Step 1. Data Type Decode
    int Offset = LEN_HEADER;

    BYTE DataType;

    DecodeBody(pRecvBuf, &Offset, &DataType);

    // Step 2. Data Type과 Size를 전송
	int FileSize = 0;

    String strFileName;

    switch(DataType)
    {
    case DATA_TYPE_MAP:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_MAP;
        FileSize = (int)GetLocalFileSize(strFileName);
        break;

    case DATA_TYPE_STATION:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_STATION;
        FileSize = (int)GetLocalFileSize(strFileName);
        break;

    case DATA_TYPE_TEACHING:
        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_TEACHING;
        FileSize = (int)GetLocalFileSize(strFileName);
        break;

    case DATA_TYPE_ALL:
        break;
    }

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.10.11 : 파일 정보 전송
    // 보낼 버퍼 초기화
    char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

    // Tag 설정
    uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
    SndTag.Info.Receiver = TAG_RECEIVER_DM;
    SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
    SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;
    SndTag.Info.CmdData = TAG_CMDDATA_DATA_BAKCUP;

    CMD_HEADER SndHeader;
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = LEN_ACK_DATAFILE_BACKUP;

	int BodySize = LEN_ACK_DATAFILE_BACKUP;
    int SndFileSize = FileSize;

    ConvertEndian(&SndHeader.uTag.Data);
    ConvertEndian(&SndHeader.Length);
    memcpy(SndBuf, &SndHeader, LEN_HEADER);

    Offset = LEN_HEADER;

    // Body 인코딩
    EncodeBody(SndBuf, &Offset, &DataType);
    EncodeBody(SndBuf, &Offset, &SndFileSize);

    // Tail 인코딩
    EncodeBody(SndBuf, &Offset, &BodySize);

    // ACK_DATAFILE_DOWNLOAD_AST_DM 명령 전송
    ClientSocket->SendBuf(SndBuf, Offset);

    // 통신 로그 생성
    AddTLVCommLog(STR_SEND, SndBuf, Offset);
    // [END] by zzang9un 2013.10.11 : 파일 정보 전송
    // -------------------------------------------

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.10.11 : 파일 전송

    // Header 변수  초기화
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

	// Tag 설정
    SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
    SndTag.Info.Receiver = TAG_RECEIVER_DM;
    SndTag.Info.ProtoType = TAG_PROTOTYPE_REQ;
    SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;

    switch(DataType)
    {
    case DATA_TYPE_MAP:
        SndTag.Info.CmdData = TAG_CMDDATA_DATA_BAKCUP_MAP;
        break;

    case DATA_TYPE_STATION:
        SndTag.Info.CmdData = TAG_CMDDATA_DATA_BAKCUP_STATION;
        break;

    case DATA_TYPE_TEACHING:
        SndTag.Info.CmdData = TAG_CMDDATA_DATA_BAKCUP_TEACHING;
        break;
    }

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = FileSize;    // 전송하고자 하는 File Size
    BodySize = FileSize;

    // 파일 전송을 위한 버퍼 생성
    char *SndFileBuf;
    int SizeOfSndFileBuf = LEN_HEADER + FileSize + LEN_TAIL;
    SndFileBuf = new char[SizeOfSndFileBuf];

    // Header 인코딩
    ConvertEndian(&SndHeader.uTag.Data);
    ConvertEndian(&SndHeader.Length);
    memcpy(SndFileBuf, &SndHeader, LEN_HEADER);

    // 버퍼에 파일 내용 인코딩
    Error = MakeFile2TLV(SndFileBuf, strFileName, FileSize);

    // Tail 인코딩
    Offset = LEN_HEADER + BodySize;
    EncodeBody(SndFileBuf, &Offset, &BodySize);

	// 파일 전송
    ClientSocket->SendBuf(SndFileBuf, SizeOfSndFileBuf);

    // 통신 로그 생성
    String strLog;
    strLog.sprintf(L"[SendDataFile] File:%s, Size:%dBytes", strFileName.c_str(), FileSize);
    ADD_SOCKET_LOG(strLog);

	AddTLVCommLog(STR_SEND, SndFileBuf, SizeOfSndFileBuf);

	delete[] SndFileBuf;
	// [END] by zzang9un 2013.10.11 : 파일 전송
    // -------------------------------------------
}
//---------------------------------------------------------------------------

/**
@brief   Bit TLV 관련 변수를 초기화 시키는 함수
@author  zzang9un
@date    2013.10.15
*/
//실행시 수행
void TSocketForm::InitBigTLV_DM()
{
	delete [] pBigTLV_DM;
	m_BigTLVTotalLen_DM = 0;
	m_BigTLVReadCnt_DM = 0;
	memset(&m_PreCmdHeader_DM, 0x00, sizeof(CMD_HEADER));
	bBigTLV_DM = false;
}
//---------------------------------------------------------------------------

/**
@brief   Bit TLV 관련 변수를 초기화 시키는 함수
@author  zzang9un
@date    2013.10.15
*/
//실행시 수행
void TSocketForm::InitBigTLV_TP()
{
    delete [] pBigTLV_TP;
    m_BigTLVTotalLen_TP = 0;
    m_BigTLVReadCnt_TP = 0;
    memset(&m_PreCmdHeader_TP, 0x00, sizeof(CMD_HEADER));
    bBigTLV_TP = false;
}
//---------------------------------------------------------------------------

/**
@brief   TP에서 온 Command를 Handle하는 함수
@author  zzang9un
@date    2013.11.07
*/
int TSocketForm::HandleTPCmd(char *pRecvBuf, TCustomWinSocket *ClientSocket)
{
    int Error = NO_ERR;

    // 받은 Command에서 Header를 얻어옴
    CMD_HEADER RecvHeader;
    memset(&RecvHeader, 0x00, sizeof(CMD_HEADER));

    // Data Manager는 Endian 변환이 필요함
	RecvHeader = GetHeader(pRecvBuf);

    // 에러 처리
	if(RecvHeader.uTag.Info.Sender != TAG_SENDER_TP)
    {
        ADD_SOCKET_LOG("[Error][HandleTPCmd]Sender is not TP!");
        return -1;
    }

    switch(RecvHeader.uTag.Info.CmdData)
    {
    case TPCMD_AST_OHT_MAINPROG_EXEC:
        ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_MAINPROG_EXEC");

		FileControl::getInstance().ExecOHTProgram();
        break;

    case TPCMD_AST_OHT_MAINPROG_CLOSE:
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_MAINPROG_CLOSE");
		SendCloseOHTProgram();
		break;

	case TPCMD_AST_OHT_REQ_FBWF_STATUS:
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_REQ_FBWF_STATUS");
		SendFBWFStatus(FBWF_CURRENT_STATUS);
		break;
	case TPCMD_AST_OHT_REQ_FBWF_ENABLE:
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_REQ_FBWF_ENABLE");
		SendFBWFStatus(FBWF_SET_ENABLE);
		break;
	case TPCMD_AST_OHT_REQ_FBWF_DISABLE:
		SendFBWFStatus(FBWF_SET_DISABLE);
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_REQ_FBWF_DISABLE");
		break;
	case TPCMD_AST_OHT_REQ_LOGDELETER_EXIST:
		SendFileExists(FILENAME_LOGDELETER);
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_REQ_LOGDELETER_EXIST");
		break;
	case TPCMD_OHT_TIMESETTING :
	{
		uTP_CMD data;
		Decode(pRecvBuf, &RecvHeader, &data, false);
		SendCurrentTime(data);
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_OHT_TIMESETTING");
	}
		break;
	case TPCMD_OHT_ADDITIONAL_STATUS :
		SendOHTAdditionalStatus();
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]REQ_TP_AST_ADDITIONAL_STATUS");
		break;
    default:
		ADD_SOCKET_LOG("[Error][HandleTPCmd]Unknown TAG_CMDDATA!!");
    }

	return Error;
}


/**
@brief      현재 설정된 FBWF상태를 TP로 보냄
@author  	doori.shin
@date    	2013.12.31
*/
void TSocketForm::SendFBWFStatus(String cmd)
{
	int error = NO_ERR;
	int status;

	// Step 1. Make header
	int Offset = LEN_HEADER;

	// 보낼 버퍼 초기화
	char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag 설정
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_TP;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_TPCMD;
	SndTag.Info.CmdData = TPRET_AST_OHT_ACK_FBWF_STATUS;

	CMD_HEADER SndHeader;
	memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

	SndHeader.uTag.Data = SndTag.Data;
	SndHeader.Length = sizeof(uTP_CMD);

	// Step 2. Make value
	uTP_CMD *sndData = new uTP_CMD;
	memset(sndData, 0, sizeof(uTP_CMD));

	sndData->TPCmd.CmdID = AssistantMainForm->GetFBWFStatus();

	// Step 3. Encoding
	Offset = EncodeTPCmd(SndBuf, &SndHeader, sndData, false);

	// Step 4. Transferring
	if(m_ServerSocketTP->Socket->ActiveConnections != 0)
		m_ServerSocketTP->Socket->Connections[0]->SendBuf(SndBuf, Offset);

	delete sndData;
}

//---------------------------------------------------------------------------
/**
@brief		파일이 존재하는지 여부를 TP로 보냄(LogDeleter.bat)
@author  	doori.shin
@date    	2013.12.31
*/
void TSocketForm::SendFileExists(String strFileName)
{
	int error = NO_ERR;
	int status;

	// Step 1. Make header
	int Offset = LEN_HEADER;

	// 보낼 버퍼 초기화
	char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag 설정
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_TP;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_TPCMD;
	SndTag.Info.CmdData = TPRET_AST_OHT_ACK_LOGDELETER;

	CMD_HEADER SndHeader;
	memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

	SndHeader.uTag.Data = SndTag.Data;
	SndHeader.Length = sizeof(uTP_CMD);

	// Step 2. Make value
	uTP_CMD *sndData = new uTP_CMD;
	memset(sndData, 0, sizeof(uTP_CMD));

	if(MonitoringUtil::getInstance().isFileExist(strFileName))
		sndData->TPCmd.CmdID = 0x01;
	else
		sndData->TPCmd.CmdID = 0x00;

	// Step 3. Encoding
	Offset = EncodeTPCmd(SndBuf, &SndHeader, sndData, false);

	// Step 4. Transferring
	if(m_ServerSocketTP->Socket->ActiveConnections != 0)
		m_ServerSocketTP->Socket->Connections[0]->SendBuf(SndBuf, Offset);

	delete sndData;
}

/**
@brief      TP에게 OHT 시스템의 부가적인 정보를 보냄
@author  	doori.shin
@date    	2013.12.31
@remark		부가정보 : 디스크 공간, 프로그램 및 파일 존재여부, 프로그램 버전 등
*/
void TSocketForm::SendOHTAdditionalStatus()
{
	int error = NO_ERR;

	// Step 1. Make header
	int Offset = LEN_HEADER;

	// 보낼 버퍼 초기화
	char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag 설정
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_TP;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_DATA;
	SndTag.Info.CmdData = TAG_CMDDATA_DATA_OHT_STATUS;

	//Step 2. Make Data
	OHT_ADDITIONAL_INFO* additionalInfo = new OHT_ADDITIONAL_INFO;
	memset(additionalInfo, 0x00, sizeof(OHT_ADDITIONAL_INFO));

	//1) FBWF Status
	additionalInfo->FBWFStatus = AssistantMainForm->GetFBWFStatus();

	//2) LogDeleter.bat 파일 존재여부
	additionalInfo->LogDeleterStatus = MonitoringUtil::getInstance().isFileExist(FILENAME_LOGDELETER) ? 0x01 : 0x00;

//	//3) Log Drive(D:\) 존재여부
//	additionalInfo->LogDriveStatus = monitoringUtil->GetDiskInfo(4)->TotalSpace == -1 ? 0x00 : 0x01;

	//4) C:\ 정보
	DISK_INFO* diskCInfo = MonitoringUtil::getInstance().GetDiskInfo(3);
	memcpy(&additionalInfo->SystemDiskInfo, diskCInfo, sizeof(DISK_INFO));
	delete diskCInfo;

	//5) D:\ 정보
	DISK_INFO* diskDInfo = MonitoringUtil::getInstance().GetDiskInfo(4);
	memcpy(&additionalInfo->LogDiskInfo, diskDInfo, sizeof(DISK_INFO));
	delete diskDInfo;

	//5) OHT 버전 및 파일존재여부
	OHT_VERSION* version = MonitoringUtil::getInstance().GetOHTMainProgramVersion();
	memcpy(&additionalInfo->OHTVersion, version, sizeof(OHT_VERSION));
	delete version;

	//Step 3. Encoding
	Offset = Encode(SndBuf, SndTag.Data, additionalInfo, false);
	delete additionalInfo;

	// Step 4. Transferring
	if(m_ServerSocketTP->Socket->ActiveConnections != 0)
		m_ServerSocketTP->Socket->Connections[0]->SendBuf(SndBuf, Offset);
}
//---------------------------------------------------------------------------
/**
@brief      OHT 메인프로그램을 종료시킨 결과를 보냄
@author  	doori.shin
@date    	2013.12.31
*/
void TSocketForm::SendCloseOHTProgram()
{
	int error = NO_ERR;

	// Step 1. Make header
	int Offset = LEN_HEADER;

	// 보낼 버퍼 초기화
	char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag 설정
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_TP;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_TPCMD;
	SndTag.Info.CmdData = TPRET_AST_OHT_MAINPROG_CLOSE;

	CMD_HEADER SndHeader;
	memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

	SndHeader.uTag.Data = SndTag.Data;
	SndHeader.Length = sizeof(uTP_CMD);

	// Step 2. Make value
	uTP_CMD *sndData = new uTP_CMD;
	memset(sndData, 0, sizeof(uTP_CMD));

	DWORD pid = ProcessTerminator::getInstance().FindPID(PROC_NAME);
	sndData->TPCmd.CmdID = ProcessTerminator::getInstance().TerminateApp(pid, 1000);

	// Step 3. Encoding
	Offset = EncodeTPCmd(SndBuf, &SndHeader, sndData, false);

	// Step 4. Transferring
	if(m_ServerSocketTP->Socket->ActiveConnections != 0)
		m_ServerSocketTP->Socket->Connections[0]->SendBuf(SndBuf, Offset);

	delete sndData;
}
//---------------------------------------------------------------------------

/**
@brief		현재 설정된 OHT시스템의 시간을 TP로 보냄
@author  	doori.shin
@date    	2013.12.31
*/
void TSocketForm::SendCurrentTime(uTP_CMD& data)
{
	//Set the time received TP
	if(data.TPCmd.Data1 != 0 && data.TPCmd.Data2 != 0)
	{
		String strDate;
		strDate.sprintf(L"%08d%06d", data.TPCmd.Data1, data.TPCmd.Data2);
		MonitoringUtil::getInstance().SetCurrentTime(strDate);
	}

	// Step 1. Make header
	int Offset = LEN_HEADER;

	// 보낼 버퍼 초기화
	char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag 설정
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_TP;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_TPCMD;
	SndTag.Info.CmdData = TPRET_OHT_TIMESETTING;

	CMD_HEADER SndHeader;
	memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

	SndHeader.uTag.Data = SndTag.Data;
	SndHeader.Length = sizeof(uTP_CMD);

	// Step 2. Make value
	struct time curTime;
	gettime(&curTime);
	struct date curDate;
	getdate(&curDate);

	data.TPCmd.Data1 = curDate.da_year*10000 + curDate.da_mon*100 + curDate.da_day;
	data.TPCmd.Data2 = curTime.ti_hour*10000 + curTime.ti_min*100 + curTime.ti_sec;


	// Step 3. Encoding
	Offset = EncodeTPCmd(SndBuf, &SndHeader, &data, false);

	// Step 4. Transferring
	if(m_ServerSocketTP->Socket->ActiveConnections != 0)
		m_ServerSocketTP->Socket->Connections[0]->SendBuf(SndBuf, Offset);
}
void __fastcall TSocketForm::FormDestroy(TObject *Sender)
{
	delete m_Log;
}

