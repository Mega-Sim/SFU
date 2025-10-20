/**
@file    Rotate.cpp
@version OHT 8.0
@brief   Rotate Class CPP File
*/

#include "Rotate.h"
#include "OHTMain.h"

//#define ROTATE_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\Rotate.log", __VA_ARGS__)
//수정
#define ROTATE_LOG(...)    WRITE_LOG_UNIT(pLogRotate, __VA_ARGS__)


/**
@brief   Rotate Class 생성자
@author  Labeler
@date    2016-11-25
@param   pHWCommon : HWCommon 클래스 포인터
*/
Rotate::Rotate(HWCommon *pHWCommon)
{
	pLogRotate = getLogUnit("D:\\log\\Rotate.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
    m_pHWCommon = pHWCommon;
    m_pCommPort = NULL;

	m_bCmdExec = false;
	InitializeCriticalSection(&m_CS_CmdExec);
    bInit = false;
	m_nError = Init();
}

/**
@brief   Rotate Class 소멸자
@author
@date
*/
Rotate::~Rotate()
{
	Close();
	m_bCmdExec = true;
	DeleteCriticalSection(&m_CS_CmdExec);
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Init()
{
    int nError = NO_ERR;
    m_nPortNum = COM_ROTATE;

	m_nIdxCurCmd = 0;
	m_nRecvFailCnt = 0;
	torque = 0;

    position_cts = 0;
    position_degree = 0;
    Velocity_ctsPsec = 0;

    bOrigin = false;
    //bCmdExec = false;
    bRunning = false;
    bMotorOn = false;

    bReqPause = false;
    bReqPosStatus = false;
    bReqVelStatus = false;
    bReqMotorStatus = false;
    //bReqABSMoveStatus = false;

    dwStartReqPosTime = 0;
    dwStartReqVelTime = 0;
    dwStartReqStatusTime = 0;

    nCntIncorrectResponse = 0;
	nAbnormalCheckMotorStatus = 0;
    nError = Open();

    if(NO_ERR == nError)
    {
        bInit = true;
    }
    else
    {
        m_nError = nError;
    }
    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Open()
{
    int nError = NO_ERR;

	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);	// Critical Section Start ***

    if(NULL == m_pCommPort)
    {
        m_pCommPort = new Comm_Port(m_nPortNum, ROTATE_BAUDRATE);

        if(NULL == m_pCommPort)
        {
            ROTATE_LOG("[Open] Fail");
            nError = ERR_ROTATE_OPEN;
        }
        else if (INVALID_HANDLE_VALUE == m_pCommPort->m_hCOM)
        {
            delete m_pCommPort;
            m_pCommPort = NULL;
            ROTATE_LOG("[Open] Fail - INVALID_HANDLE_VALUE");
            nError = ERR_ROTATE_OPEN;
        }
        else
        {
            ROTATE_LOG("[Open] Success");
        }
    }
	LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ***
	m_bCmdExec = false;

    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }

    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
void Rotate::Close()
{
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);

	if(m_pCommPort!=NULL)
    {
		delete m_pCommPort;
		m_pCommPort = NULL;
        ROTATE_LOG("[Close] Success");
	}

	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;
}

/**
@brief   Rotate Motor Enable Func
@author  Labeler
@date    2016-11-25
@return  Enable 동작 중 Error 발생 ErrorCode Return
         미발생 할경우 NO_ERR Return
*/
int Rotate::Enable()
{
	if (NULL == m_pCommPort)
	{
		bOrigin = false;
		m_nError = ERR_ROTATE_NOT_CONNECTED;
		ROTATE_LOG("Enable Failed. No Connection");
		return m_nError;
	}
	if (bMotorOn)
	{
		ROTATE_LOG("Alread Enabled");
		return NO_ERR;
	}

    // Rotate Motor가 Disable 상태일 경우에만, Enable 수행
    char ReadBuff[1024] = {0, };
    char strMsg[1024] = {0,};
    AnsiString strReceiveData = "";

    // Enable 작업 후에는, 반드시 Origin 수행 필요
    bOrigin = false;

	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);	// ** Critical Section Started ***
	ROTATE_LOG("Enable Start");
    Sleep(20);

    sprintf(strMsg, "%s%c%c", ROTATE_ENABLE, CR, LF);
	PurgeComm_s();
	if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
    {

		LeaveCriticalSection(&m_CS_CmdExec);	// ** Critical Section Leave **
		m_bCmdExec = false;
		ROTATE_LOG("[Enable] ERR_ROTATE_WRITE");
		m_nError = ERR_ROTATE_WRITE;
		return m_nError;
    }

	ROTATE_LOG("[Enable] Data Sent: %s", strMsg);

    Sleep(COMM_WAIT_TIME);
	if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
    {
		LeaveCriticalSection(&m_CS_CmdExec);	// ** Critical Section Leave **
		m_bCmdExec = false;
		ROTATE_LOG("[Enable] Recv Failed");
		bOrigin = false;
		m_nError = ERR_ROTATE_NO_RESPONSE;
		return m_nError;
    }

    strReceiveData = ReadBuff;
    if(0 == strReceiveData.Pos(strMsg))
    {
		LeaveCriticalSection(&m_CS_CmdExec);	// ** Critical Section Leave **
		m_bCmdExec = false;
		ROTATE_LOG("[Enable] ERR_ROTATE_NO_RESPONSE2");
		m_nError = ERR_ROTATE_INCORRECT_RESPONSE;
		return m_nError;
    }

	LeaveCriticalSection(&m_CS_CmdExec);	// ** Critical Section Leave **
	m_bCmdExec = false;
	ROTATE_LOG("[Enable] Rotate Motor Enable Success");
    bMotorOn = true;

    Sleep(20);
	return NO_ERR;
}

/**
@brief   Rotate Motor Disable Function
@author  Labeler
@date    2016-11-25
@return  Disable 동작 중 Error 발생 ErrorCode Return
         미발생 할경우 NO_ERR Return
*/
int Rotate::Disable()
{
    int nError = NO_ERR;

    // Rotate Motor가 Enable 상태일 경우와 에러가 있을 경우에만, Disable 수행
	if((true == bMotorOn) || NO_ERR != GetError());
    {
        char ReadBuff[1024] = {0, };
		char strMsg[1024] = {0,};
        AnsiString strReceiveData = "";

        // Enable 작업 후에는, 반드시 Origin 수행 필요
        bOrigin = false;

        Sleep(20);
        //nError = IsCmdExec();
         m_bCmdExec = true;
		 EnterCriticalSection(&m_CS_CmdExec);

        if(NO_ERR == nError)
        {


            if(NULL == m_pCommPort)
            {
                nError = ERR_ROTATE_NOT_CONNECTED;
                ROTATE_LOG("[Disable] ERR_ROTATE_NOT_CONNECTED");
            }
            else
            {
                sprintf(strMsg, "%s%c%c", ROTATE_DISABLE, CR, LF);
                PurgeComm_s();
                if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
                {
                    nError = ERR_ROTATE_WRITE;
                    ROTATE_LOG("[Disable] ERR_ROTATE_WRITE");
                }
                else
                {
                    ROTATE_LOG("[Disable] Send Data");
                }
            }
        }

        if(NO_ERR == nError)
        {
            Sleep(COMM_WAIT_TIME);
            if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
            {
                nError = ERR_ROTATE_NO_RESPONSE;
                ROTATE_LOG("[Disable] Send ERR_ROTATE_NO_RESPONSE1");
            }
        }

        if(NO_ERR == nError)
        {
            strReceiveData = ReadBuff;
            if(0 == strReceiveData.Pos(strMsg))
            {
                nError = ERR_ROTATE_INCORRECT_RESPONSE;
                ROTATE_LOG("[Disable] Send ERR_ROTATE_INCORRECT_RESPONSE");
            }
            else
            {
                ROTATE_LOG("[Disable] Rotate Motor Disable Success");
            }
        }
        Sleep(20);
		LeaveCriticalSection(&m_CS_CmdExec);
		m_bCmdExec = false;
        //bCmdExec = false;
        bMotorOn = false;
    }

    if(NO_ERR != nError)
    {
        m_nError = nError;
    }
    else
    {
        if(NO_ERR != GetError())
        {
            m_nError = NO_ERR;
        }
    }

    return nError;
}

/**
@brief   Rotate Motor의 현재 위치를 원점으로 설정//
@author  Labeler//
@date    2016-11-28//
@return  정상인 경우 NO_ERR, 비정상의 경우 에러코드 반환//
*/
int Rotate::SetOrigin()
{
    int nError = NO_ERR;
	char ReadBuff[1024] = {0, };
   	char strMsg[1024] = {0,};
    AnsiString strReceiveData = "";

	Sleep(100);
	ROTATE_LOG("[SetOrigin] Start");
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);
    //nError = IsCmdExec();


    // 현재 Position Read
	if(NO_ERR == nError)
    {
		//nError = ReadPosition();     // 포지션 상시 Read *****
		if(NO_ERR == nError)
        {
            Sleep(COMM_WAIT_TIME);
			sprintf(strMsg, "%s%d%c%c", ROTATE_SETORIGIN, position_cts, CR, LF);
			//nError = IsCmdExec();
        }
	}

    if(NO_ERR == nError)
    {
        //bCmdExec = true;
		if(NULL == m_pCommPort)
        {
            nError = ERR_ROTATE_NOT_CONNECTED;
            ROTATE_LOG("[SetOrigin] ERR_ROTATE_NOT_CONNECTED");
        }
    }

    if(NO_ERR == nError)
	{
		ROTATE_LOG("[SetOrigin] send: %s", strMsg);
        PurgeComm_s();
		if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
        {
            nError = ERR_ROTATE_WRITE;
            ROTATE_LOG("[SetOrigin] ERR_ROTATE_WRITE");
        }
        else
        {
            ROTATE_LOG("[SetOrigin] Send Data");
        }
    }

    if(NO_ERR == nError)
    {
        Sleep(COMM_WAIT_TIME);
		if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
		{
            nError = ERR_ROTATE_NO_RESPONSE;
            ROTATE_LOG("[SetOrigin] ERR_ROTATE_NO_RESPONSE");
        }
    }
	ROTATE_LOG("[SetOrigin] recv: %s", ReadBuff);
    if(NO_ERR == nError)
    {
        strReceiveData = ReadBuff;
        if(0 == strReceiveData.Pos(strMsg))
        {
            nError = ERR_ROTATE_INCORRECT_RESPONSE;
            ROTATE_LOG("[SetOrigin] ERR_ROTATE_INCORRECT_RESPONSE");
        }
        else
        {
            ROTATE_LOG("[SetOrigin] Origin Setting Success");
        }
    }

	Sleep(20);

	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;
    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }

    return nError;
}

/**
@brief   Rotate Motor를 원점으로 이동//
@author  Labeler//
@date    2016-11-28//
@return  정상인 경우 NO_ERR, 비정상의 경우 에러코드 반환//
*/
int Rotate::Zeroing()
{
    int nError = NO_ERR;
    char ReadBuff[1024] = {0, };
   	char strMsg[1024] = {0,};
    AnsiString strReceiveData = "";

    Sleep(20);
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);

    if(NO_ERR == nError)
    {
        //bCmdExec = true;
        if(NULL == m_pCommPort)
        {
            nError = ERR_ROTATE_NOT_CONNECTED;
            ROTATE_LOG("[Zeroing] ERR_ROTATE_NOT_CONNECTED");
        }
    }

    if(NO_ERR == nError)
    {
        ROTATE_LOG("[Zeroing] Start");

        sprintf(strMsg, "%s%c%c", ROTATE_ZEROING, CR, LF);
		PurgeComm_s();
        if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		{
            nError = ERR_ROTATE_WRITE;
            ROTATE_LOG("[Zeroing] ERR_ROTATE_WRITE");
        }
        else
        {
            ROTATE_LOG("[Zeroing] Send Data");
        }
    }

    if(NO_ERR == nError)
    {
        Sleep(COMM_WAIT_TIME);
        if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
        {
            nError = ERR_ROTATE_NO_RESPONSE;
            ROTATE_LOG("[Zeroing] ERR_ROTATE_NO_RESPONSE");
        }
    }

    if(NO_ERR == nError)
    {
        strReceiveData = ReadBuff;
        if(0 == strReceiveData.Pos(strMsg))
        {
            nError = ERR_ROTATE_INCORRECT_RESPONSE;
            ROTATE_LOG("[Zeroing] ERR_ROTATE_INCORRECT_RESPONSE");
        }
        else
        {
            ROTATE_LOG("[Zeroing] Zeroing Success");
        }
    }
    //Sleep(20);
	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;

    if(NO_ERR == nError)
    {
        bOrigin = true;
    }
    else
    {
        bOrigin = false;
        m_nError = nError;
    }

    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::GoHome()
{
    int nError = MoveAbsolute(0);

    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
//int Rotate::IsCmdExec()
//{
//    int nError = NO_ERR;
//    DWORD dwSTTime;
//    DWORD dwCheckTime;
//
//	if(true == bCmdExec)
//	{
//        bReqPause = true;
//        dwSTTime = timeGetTime();
//        dwCheckTime = (DWORD)OHTMainForm->GetTransControl()->m_pParamSet->TransDefine.nRotate_Exec_Timeout;
//
//		while(true == bCmdExec)
//        {
//			if(OHTUTILITY::PreventOverflowGetTimeMs(dwSTTime) > dwCheckTime)
//			{
//				nError = ERR_ROTATE_EXEC_TIMEOVER;
//                bCmdExec = false;
//                break;
//            }
//            Sleep(10);
//        }
//    }
//
//    bReqPause = false;
//
//    if(NO_ERR != nError)
//    {
//        m_nError = nError;
//    }
//
//    return nError;
//}

// SHCHO 2019.02
// Rotate 모터 상태 갱신 스텝 함수
void Rotate::ExecuteStep()
{
	static const int nReqCmdCnt = 2; //ArrReqCmd 커맨드 개수. 아래 배열 개수와 맞추기
	static char *ArrReqCmd[nReqCmdCnt] =
	{
		READ_POS_COMMAND,	// 현재 위치 요청.  Px.1=50000  형태로 응답. cts 단위
		//READ_VEL_COMMAND,   // 현재 속도 요청.  Sn.1=200 	형태로 응답.  단위/K37 파라미터 설정값
		//READ_TORQUE_COMMAND,    // 현재 토크 요청.  lx.1=15    형태로 응답.
		READ_STATUS_COMMAND	  // 모터 상태 요청. Ux.1=Int 형태로 응답. Int 값이 모터 상태
	};

	char szBuf[1024];
	memset(szBuf, 0, sizeof(szBuf));

	// 에러 없고 할일 없을떄 스탭 진입
	bool bDoStep = ((m_bCmdExec == false) && (m_nError == NO_ERR) && (m_stepRotate.Step == 0));

	static DTimer idleTimer;
	// 20ms 쉬엇다가 스텝진입. 너무 빨리 갱신되서 다른 스레드가 Critical Section 진입 못하는 것 방지용도
	if (idleTimer.Enable(bDoStep, 20))
	{
		m_bCmdExec = true;
		idleTimer.Init();
		m_stepRotate.Step = 100;
	}

	// SHCHO 2019.02
	// Rotate 상태 갱신 비동기 스텝 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Step 100~102: Rotate 모터의 위치, 토크 상태를 Request 하고 일정 시간 지난 후 결과를 Recv하여 멤버변수 갱신한다
	//  주의 : step 시작할때 Ciritcal Section 시작함으로, Step=0 만드는 부분에서 반드시 Leave 해줘야 함.
	// 보내고, 보낸 데이터 읽는게 한 쌍이라서 Critical section 함
	//
	switch(m_stepRotate.Step)
	{
		case 100:		// Purge 명령으로 버퍼 클리어
		{
			m_bCmdExec = true;
			EnterCriticalSection(&m_CS_CmdExec);	// Criitical Section Start ***
			PurgeComm_s();     	// purge
			m_stepRotate.Timer.Init();
			m_stepRotate.Step++;
			break;
		}
		case 101:		// Request 커맨드 하나를 Rotate로 보냄
		{
			if(!m_stepRotate.Timer.Enable(true, 10))	// non-block delay 10ms
				break;

			// 위치, 토크, 상태 값 한번씩 요청
			sprintf(szBuf, "%s%c%c", ArrReqCmd[m_nIdxCurCmd], CR, LF);
			m_nIdxCurCmd++;
			m_nIdxCurCmd %= nReqCmdCnt; // 다음번에 다음 커맨드.

			// Send Command [PC ---> Rotate]
			int nLen = strlen(szBuf);
			if(m_pCommPort->WriteCommBlock(szBuf, nLen) != nLen)
			{
				// send 실패 경우
				if(m_stepRotate.RetryCnt > 10)
				{
					m_stepRotate.RetryCnt = 0;
					m_nError = ERR_ROTATE_WRITE;
					ROTATE_LOG("Request Failed. Retry Count Over");

					m_stepRotate.Step = 0;
					LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ****
					m_bCmdExec = false;
					break;
				}

				m_stepRotate.RetryCnt++;
				ROTATE_LOG("Request Failed, Retry: %d", m_stepRotate.RetryCnt);

				m_stepRotate.Step = 0;
				LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ****
				m_bCmdExec = false;
				break;
			}

			m_stepRotate.Timer.Init();
			m_stepRotate.Step++;
			break;
		}
		case 102:		// Rotate에서 결과를 받음
		{
			if(!m_stepRotate.Timer.Enable(true, 50))	// non-block wait 50ms
				break;

			// Raw Data Recv [Rotate --> PC]
			m_pCommPort->ReadUntilCount(szBuf, '\n', 2);

			// Parse Data ********
			if( ParseCmd(szBuf, strlen(szBuf)) == false)
			{
				// 파싱 실패 경우
				if(m_nRecvFailCnt > 10)	//10번 이유는 이전 코드 내용 유지
				{
					m_nRecvFailCnt = 0;
					m_nError = ERR_ROTATE_INCORRECT_RESPONSE;
					ROTATE_LOG("ERR_ROTATE_INCORRECT_RESPONSE");
					m_stepRotate.Step = 0;
					LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ****
					m_bCmdExec = false;
					break;

				}
				m_nRecvFailCnt++;
				m_stepRotate.Step = 0;
				LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ****
				m_bCmdExec = false;
				break;
			}

			m_nRecvFailCnt = 0;
			m_stepRotate.Step++;
			break;
		}
		case 103:		// 마지막  스텝 마무리
		{
			m_stepRotate.Timer.Init();
			m_stepRotate.Step = 0;
			LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ****
			m_bCmdExec = false;
			break;
		}
	}
}

// SHCHO 2019.02
// Rotate 에서 받은 데이터 파싱
// 기존에 데이터 파싱부분에서 AnsiString 쓰이던 부분 모두 제거
// 위치, 속도, 토크, 모터상태를 파싱할수있음
bool Rotate::ParseCmd(char *buf, int nSize)
{
	int nVal = 0;

	if(GetThisValue(buf, "Px.1=", &nVal))  // 위치
	{
		position_cts = nVal;
		//position_degree = position_cts / 1421.1167;
		position_degree = position_cts / ROTATE_PULSE_PER_DEGREE;

		ROTATE_LOG("[Recv] Position cts=%d, deg=%d", nVal, position_degree);
	}
	else if(GetThisValue(buf, "Ix.1=", &nVal)) // 토크
	{
		torque = nVal;
		ROTATE_LOG("[Recv] Torque=%d", nVal);
	}
	else if(GetThisValue(buf, "Ux.1=", &nVal)) // 상태
	{
		SetStatusCmd(nVal);	//상태는 종류가 몇개있어서 따로 뺌
		ROTATE_LOG("[Recv] Status=%d", nVal);
	}
	else if(GetThisValue(buf, "Sx.1=", &nVal)) //속도
	{
		Velocity_ctsPsec = nVal;
		ROTATE_LOG("[Recv] Velocity=%d", nVal);
	}
	else
	{
		// unkown command
		//ROTATE_LOG("Recv Unkown Cmd: %s", buf == NULL ? "[NULL]" : buf);
		//return false;
    }

	return true;
}

// Ux.1 : 모터 상태 Recv 받은 데이터에 따라서 멤버변수 갱신
void Rotate::SetStatusCmd(int nRcvData)
{
	switch(nRcvData)
	{
		case 0 : //모터 동작중(running)
			m_nError = NO_ERR;
			bMotorOn = true;
			bRunning = true;
		break;

		case 1 : //편차카운터 overload
			m_nError = ERR_ROTATE_COUNTOVERLOAD;
			bMotorOn = false;
			bRunning = false;
			bInit = false;
			bOrigin = false;
		break;

		case 2 : //과속도/회생전압
			m_nError = ERR_ROTATE_OVERSPEED;
			bMotorOn = false;
			bRunning = false;
			bInit = false;
			bOrigin = false;
		break;

		case 4 : //과부하
			m_nError = ERR_ROTATE_OVERLOAD;
			bMotorOn = false;
			bRunning = false;
			bInit = false;
			bOrigin = false;
		break;

		case 8 : //동작/위치완료(inposition)
			m_nError = NO_ERR;
			bMotorOn = true;
			bRunning = false;
		break;

		case 16 : //모터 free
			m_nError = NO_ERR;
			bMotorOn = false;
			bRunning = false;
		break;

		case 32 : //완충모드 running
			m_nError = NO_ERR;
			bMotorOn = true;
			bRunning = true;
		break;

		case 40 : //완충모드 완료
			m_nError = NO_ERR;
			bMotorOn = false;
			bRunning = false;
		break;

		default:
		break;
	}
}
/**
@brief      Motor의 현재 위치 요청 함수
@author     Labeler
@date       2016-12-03
@return     다른 명령 진행 중   : NO_ERR
            정상 요청 완료      : NO_ERR
            정상 요청 실패      : 해당 ErrorCode 반환
*/
//int Rotate::ReqPosition()
//{
//	int nError = NO_ERR;
//
//	if((true == bCmdExec) || (true == bReqPause))
//	{
//		return nError;
//	}
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReqPosition] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//	else
//	{
//       	char strMsg[1024] = {0,};
//		bCmdExec = true;
//
//        sprintf(strMsg, "%s%c%c", READ_POS_COMMAND, CR, LF);
//		PurgeComm_s();
//        Sleep(10);
//
//        if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
//		{
//			ROTATE_LOG("[ReqPosition] ERR_ROTATE_WRITE");
//			bCmdExec = false;
//            m_nError = ERR_ROTATE_WRITE;
//			return ERR_ROTATE_WRITE;
//		}
//        else
//        {
////            ROTATE_LOG("[ReqPosition] Send Data");
//			bReqPosStatus = true;
//			dwStartReqPosTime = timeGetTime();
//        }
//	}
//	return nError;
//}
//
///**
//@brief      Motor의 현재 위치 정보 수신 함수
//@author     Labeler
//@date       2016-12-03
//@return     다른 명령 진행 중   : NO_ERR
//            위치 요청 無        : NO_ERR
//            정상 수신 완료      : NO_ERR
//            정상 수신 실패      : 해당 ErrorCode 반환
//*/
//int Rotate::ReadPosition()
//{
//	int nError = NO_ERR;
//	char ReadBuff[1024] = {0, };
//
//	if((true != bCmdExec) && (true != bReqPosStatus))
//	{
//		return nError;
//	}
//	else if((true != bCmdExec) && (true == bReqPosStatus))
//	{
//		bReqPosStatus = false;
//		return nError;
//	}
//    else if((true == bCmdExec) && (true != bReqPosStatus))
//    {
//        return nError;
//    }
//    else if((true == bReqPosStatus) && ((true == bReqMotorStatus) || (true == bReqVelStatus)))
//    {
//        bReqPosStatus = false;
//        bReqVelStatus = false;
//        bReqMotorStatus = false;
//        bCmdExec = false;
//        return nError;
//    }
//    else if((true == bReqMotorStatus) || (true == bReqVelStatus))
//    {
//        return nError;
//    }
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReadPosition] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//
//	if(0 == m_pCommPort->ReadUntilCount(ReadBuff, '\n', 2))
//	{
//		if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartReqPosTime) > COMM_RECIVIE_TIME_LIMIT)
//		{
//			bReqPosStatus = false;
//			bCmdExec = false;
//			nError = ERR_ROTATE_NO_RESPONSE;
//			ROTATE_LOG("[ReadPosition] ERR_ROTATE_NO_RESPONSE");
//		}
//	}
//	else
//	{
//		int nPxPos = 0;
//        int nEndPos = 0;
//		int nRcvData = 0;
//       	AnsiString strReceiveData = "";
//
//		strReceiveData = ReadBuff;
//		nPxPos = strReceiveData.Pos("Px.1=");
//
//		if(0 == nPxPos)
//		{
//            if(INCORRECT_RESPONSE_LIMIT_COUNT > nCntIncorrectResponse)
//            {
//                nCntIncorrectResponse++;
//            }
//            else
//            {
//    			nError = ERR_ROTATE_INCORRECT_RESPONSE;
//    			ROTATE_LOG("[ReadPosition] ERR_ROTATE_INCORRECT_RESPONSE1");
//            }
//		}
//		else
//		{
//            nCntIncorrectResponse = 0;
//			position_cts = StrToIntDef(strReceiveData.SubString(nPxPos+5, strReceiveData.Length()-nPxPos-6), position_cts);
////            position_degree = position_cts / 1421.1167;
//			position_degree = position_cts / ROTATE_PULSE_PER_DEGREE;
//		}
//		bReqPosStatus = false;
//		bCmdExec = false;
//	}
//
//    if(NO_ERR != nError)
//    {
//        m_nError = nError;
//    }
//	return nError;
//}
//
///**
//@brief      Motor의 현재 속도 요청 함수
//@author     Labeler
//@date       2016-12-03
//@return     다른 명령 진행 중   : NO_ERR
//            정상 요청 완료      : NO_ERR
//            정상 요청 실패      : 해당 ErrorCode 반환
//*/
//int Rotate::ReqVelocity()
//{
//	int nError = NO_ERR;
//
//	if((true == bCmdExec) || (true == bReqPause))
//	{
//		return nError;
//	}
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReqVelocity] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//	else
//	{
//       	char strMsg[1024] = {0,};
//		bCmdExec = true;
//
//        sprintf(strMsg, "%s%c%c", READ_VEL_COMMAND, CR, LF);
//		PurgeComm_s();
//        Sleep(10);
//
//        if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
//		{
//			ROTATE_LOG("[ReqVelocity] ERR_ROTATE_WRITE");
//			bCmdExec = false;
//            m_nError = ERR_ROTATE_WRITE;
//			return ERR_ROTATE_WRITE;
//		}
//        else
//        {
////            ROTATE_LOG("[ReqVelocity] Send Data");
//			bReqVelStatus = true;
//			dwStartReqVelTime = timeGetTime();
//        }
//	}
//	return nError;
//}
//
///**
//@brief      Motor의 현재 속도 정보 수신 함수
//@author     Labeler
//@date       2016-12-03
//@return     다른 명령 진행 중   : NO_ERR
//            속도 요청 無        : NO_ERR
//            정상 수신 완료      : NO_ERR
//            정상 수신 실패      : 해당 ErrorCode 반환
//*/
//int Rotate::ReadVelocity()
//{
//	int nError = NO_ERR;
//	char ReadBuff[1024] = {0, };
//
//	if((true != bCmdExec) && (true != bReqVelStatus))
//	{
//		return nError;
//	}
//	else if((true != bCmdExec) && (true == bReqVelStatus))
//	{
//		bReqVelStatus = false;
//		return nError;
//	}
//    else if((true == bCmdExec) && (true != bReqVelStatus))
//    {
//        return nError;
//    }
//    else if((true == bReqVelStatus) && ((true == bReqMotorStatus) || (true == bReqPosStatus)))
//    {
//        bReqPosStatus = false;
//        bReqVelStatus = false;
//        bReqMotorStatus = false;
//        bCmdExec = false;
//        return nError;
//    }
//    else if((true == bReqMotorStatus) || (true == bReqPosStatus))
//    {
//        return nError;
//    }
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReadVelocity] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//
//	if(0 == m_pCommPort->ReadUntilCount(ReadBuff, '\n', 2))
//	{
//		if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartReqVelTime) > COMM_RECIVIE_TIME_LIMIT)
//		{
//			bReqVelStatus = false;
//			bCmdExec = false;
//			nError = ERR_ROTATE_NO_RESPONSE;
//			ROTATE_LOG("[ReadVelocity] ERR_ROTATE_NO_RESPONSE");
//		}
//	}
//	else
//	{
//		int nSxPos = 0;
//		int nRcvData = 0;
//       	AnsiString strReceiveData = "";
//
//		strReceiveData = ReadBuff;
//		nSxPos = strReceiveData.Pos("Sx.1=");
//		if(0 == nSxPos)
//		{
//            if(INCORRECT_RESPONSE_LIMIT_COUNT > nCntIncorrectResponse)
//            {
//                nCntIncorrectResponse++;
//            }
//            else
//            {
//			    nError = ERR_ROTATE_INCORRECT_RESPONSE;
//    			ROTATE_LOG("[ReadVelocity] ERR_ROTATE_INCORRECT_RESPONSE1");
//            }
//		}
//		else
//		{
//			nCntIncorrectResponse = 0;
//			Velocity_ctsPsec = StrToIntDef(strReceiveData.SubString(nSxPos+5, strReceiveData.Length()-nSxPos-6), (int)Velocity_ctsPsec);
//		}
//		bReqVelStatus = false;
//		bCmdExec = false;
//	}
//
//    if(NO_ERR != nError)
//    {
//        m_nError = nError;
//    }
//	return nError;
//}
//
///**
//@brief      Motor의 현재 상태 요청 함수
//@author     Labeler
//@date       2016-12-03
//@return     다른 명령 진행 중   : NO_ERR
//            정상 요청 완료      : NO_ERR
//            정상 요청 실패      : 해당 ErrorCode 반환
//*/
//int Rotate::ReqMotorStatus()
//{
//	int nError = NO_ERR;
//
//	if((true == bCmdExec) || (true == bReqPause))
//	{
//		return nError;
//	}
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReqMotorStatus] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//	else
//	{
//       	char strMsg[1024] = {0,};
//		bCmdExec = true;
//
//		sprintf(strMsg, "%s%c%c", READ_STATUS_COMMAND, CR, LF); //#define  READ_STATUS_COMMAND  "?99"
//		PurgeComm_s();
//        Sleep(10);
//
//        if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
//		{
//			ROTATE_LOG("[ReqMotorStatus] ERR_ROTATE_WRITE");
//			bCmdExec = false;
//            m_nError = ERR_ROTATE_WRITE;
//			return ERR_ROTATE_WRITE;
//		}
//        else
//        {
//			bReqMotorStatus = true;
//			dwStartReqStatusTime = timeGetTime();
//        }
//	}
//	return nError;
//}
//
///**
//@brief      Motor의 현재 상태 정보 수신 함수
//@author     Labeler
//@date       2016-12-03
//@return     다른 명령 진행 중   : NO_ERR
//            상태 요청 無        : NO_ERR
//            정상 수신 완료      : NO_ERR
//            정상 수신 실패      : 해당 ErrorCode 반환
//*/
//int Rotate::ReadMotorStatus()
//{
//	int nError = NO_ERR;
//	char ReadBuff[1024] = {0, };
//
//	if((true != bCmdExec) && (true != bReqMotorStatus))
//	{
//		return nError;
//	}
//	else if((true != bCmdExec) && (true == bReqMotorStatus))
//	{
//		bReqVelStatus = false;
//		return nError;
//	}
//    else if((true == bCmdExec) && (true != bReqMotorStatus))
//    {
//        return nError;
//    }
//    else if((true == bReqMotorStatus) && ((true == bReqVelStatus) || (true == bReqPosStatus)))
//    {
//        bReqPosStatus = false;
//        bReqVelStatus = false;
//        bReqMotorStatus = false;
//        bCmdExec = false;
//        return nError;
//    }
//    else if((true == bReqPosStatus) || (true == bReqVelStatus))
//    {
//        return nError;
//    }
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReadMotorStatus1] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//
//	if(0 == m_pCommPort->ReadUntilCount(ReadBuff, '\n', 2))
//	{
//		if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartReqStatusTime) > COMM_RECIVIE_TIME_LIMIT)
//		{
//			bReqVelStatus = false;
//			bCmdExec = false;
//			nError = ERR_ROTATE_NO_RESPONSE;
//			ROTATE_LOG("[ReadMotorStatus1] ERR_ROTATE_NO_RESPONSE");
//		}
//	}
//	else
//	{
//		int nUxPos = 0;
//		int nRcvData = 0;
//		AnsiString strReceiveData = "";
//		strReceiveData = ReadBuff;
//		ROTATE_LOG("[ReceiveData] strReceiveData : %s, strReceiveData.Length : %d ", strReceiveData, strReceiveData.Length()); //Rotate Motor Status 값 추가 2018.10.15 JRESPECT.IM
//		nUxPos = strReceiveData.Pos("Ux.1=");
//		ROTATE_LOG("[ReceiveData] nUxPos : %d", nUxPos);
//		//받은 Rotate Status Data의 길이가 다를 경우 길이 아직 미정
////		if(strReceiveData != 10)
////		{
////		  nError = ERR_ROTATE_INCORRECT_RESPONSE;
////		  ROTATE_LOG("[ReadMotorStatus] MotorStatus Length NG");
////		}
//		if(0 >= nUxPos)
//		{
//			if(INCORRECT_RESPONSE_LIMIT_COUNT > nCntIncorrectResponse)
//			{
//				nCntIncorrectResponse++;
//			}
//			else
//			{
//                nCntIncorrectResponse = 0;
//			    nError = ERR_ROTATE_INCORRECT_RESPONSE;
//				ROTATE_LOG("[ReadMotorStatus] ERR_ROTATE_INCORRECT_RESPONSE1");
//            }
//		}
//		else
//		{
//			nRcvData = StrToIntDef(strReceiveData.SubString(nUxPos+5, strReceiveData.Length()-nUxPos-6), -1);
//			if(-1 == nRcvData)
//			{
//				if (nAbnormalCheckMotorStatus >= 2) //Rotate Motor Status 값이 비정상인 경우 Retry 3회 추가 2018.07.10 JRESPECT.IM
//				{
//					nAbnormalCheckMotorStatus = 0;
//					ROTATE_LOG("[ReadMotorStatus] ERR_ROTATE_INCORRECT_RESPONSE2");
//					nError = ERR_ROTATE_INCORRECT_RESPONSE;
//				}
//				else
//				{
//					nAbnormalCheckMotorStatus++;
//					ROTATE_LOG("[ReadMotorStatus] Abnormal MotorStatus");
//                }
//			}
//			else
//			{
//				ROTATE_LOG("[Read MotorStatus Data] %d", nRcvData);  //Rotate Motor Status 값 추가 2018.10.15 JRESPECT.IM
//				nAbnormalCheckMotorStatus = 0;
//				switch(nRcvData)
//				{
//					case 0 : //모터 동작중(running)
//						nError = NO_ERR;
//						bMotorOn = true;
//						bRunning = true;
//					break;
//
//					case 1 : //편차카운터 overload
//						nError = ERR_ROTATE_COUNTOVERLOAD;
//						bMotorOn = false;
//						bRunning = false;
//						bInit = false;
//                        bOrigin = false;
//					break;
//
//					case 2 : //과속도/회생전압
//						nError = ERR_ROTATE_OVERSPEED;
//						bMotorOn = false;
//						bRunning = false;
//						bInit = false;
//                        bOrigin = false;
//					break;
//
//					case 4 : //과부하
//						nError = ERR_ROTATE_OVERLOAD;
//						bMotorOn = false;
//						bRunning = false;
//						bInit = false;
//                        bOrigin = false;
//					break;
//
//					case 8 : //동작/위치완료(inposition)
//						nError = NO_ERR;
//						bMotorOn = true;
//						bRunning = false;
//					break;
//
//					case 16 : //모터 free
//						nError = NO_ERR;
//						bMotorOn = false;
//						bRunning = false;
//					break;
//
//					case 32 : //완충모드 running
//						nError = NO_ERR;
//						bMotorOn = true;
//						bRunning = true;
//					break;
//
//					case 40 : //완충모드 완료
//						nError = NO_ERR;
//						bMotorOn = false;
//						bRunning = false;
//					break;
//
//					default:
//					break;
//				}
//			}
//		}
//
//		bReqMotorStatus = false;
//		bCmdExec = false;
//	}
//
//    if(NO_ERR != nError)
//    {
//        m_nError = nError;
//    }
//	return nError;
//}

/**
@brief   //
@author  //
@date    //
@return  //
*/

// Stop 명령의 경우, 기존 명령 유/무 관계 없이 바로 전송
int Rotate::Stop()
{
    int nError = NO_ERR;
    AnsiString strRData;
    int nStopCnt = 0;

    Sleep(20);
    if((true == bReqPosStatus) || (true == bReqVelStatus) || (true == bReqMotorStatus))
    {
        //IsCmdExec();
        ROTATE_LOG("[Stop] IsCmdExec, Error : %d", nError);
        //bCmdExec = true;
	}
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);

    if(NO_ERR == m_nError)
    {
        ROTATE_LOG("[Stop] Before SendCommand1_1");
        nError = SendCommand1(ROTATE_STOP, strRData);
        nStopCnt++;
        if(NO_ERR == nError)
        {
            ROTATE_LOG("[Stop] Before SendCommand1_2");
            nError = SendCommand1(ROTATE_STOP, strRData);
            nStopCnt++;
        }
    }

    if((NO_ERR != m_nError) && (2 > nStopCnt))
    {
        ROTATE_LOG("[Stop] Before SendCommand1_3");
        nError = SendCommand1(ROTATE_STOP, strRData);
        nStopCnt++;
        if((NO_ERR != m_nError) && (2 > nStopCnt))
        {
            ROTATE_LOG("[Stop] Before SendCommand1_4");
            nError = SendCommand1(ROTATE_STOP, strRData);
        }
    }

	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;
    //if(true == bCmdExec)
    //{
    //    bCmdExec = false;
    //}

    if( (NO_ERR != nError) && (ERR_ROTATE_INCORRECT_RESPONSE != nError) )
    {
        bOrigin = false;
        m_nError = nError;
    }
    else
    {
        ROTATE_LOG("[Stop] Stop Success");
    }
    Sleep(20);
    return nError;
}
//
///**
//@brief   //
//@author  //
//@date    //
//@return  //
//*/
//int Rotate::Move()
//{
//    int nError = NO_ERR;
//    return nError;
//}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Move_Jog_CW()
{
    int nError = NO_ERR;
    AnsiString strRData;

	Sleep(20);
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);
    //nError = IsCmdExec();

    if(NO_ERR == nError)
    {
        //bCmdExec = true;

        if(NULL == m_pCommPort)
        {
            nError = ERR_ROTATE_NOT_CONNECTED;
        }

        if(false == bOrigin)
        {
            nError = ERR_ROTATE_NOTORIGIN;
        }
    }

    if(NO_ERR == nError)
    {
        nError = SendCommand1(ROTATE_JOGMOVE_CW, strRData);
    }

    Sleep(20);
	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;


    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }
    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Move_Jog_CCW()
{
    int nError = NO_ERR;
    AnsiString strRData;

    Sleep(20);
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);

    if(NO_ERR == nError)
    {
        //bCmdExec = true;

        if(NULL == m_pCommPort)
        {
            nError = ERR_ROTATE_NOT_CONNECTED;
        }

        if(false == bOrigin)
        {
            nError = ERR_ROTATE_NOTORIGIN;
        }
    }

    if(NO_ERR == nError)
    {
        nError = SendCommand1(ROTATE_JOGMOVE_CCW, strRData);
    }

    Sleep(20);
	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;

    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }
    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
//int Rotate::MoveIncremental()
//{
//    int nError = NO_ERR;
//    return nError;
//}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::MoveIncremental_CW(int nLoc)
{
    int nError = NO_ERR;
    //AnsiString strRData;
    //AnsiString strSndData;

    //Sleep(20);
    //nError = IsCmdExec();

    if(NO_ERR == nError)
    {
        // 이동
        nError = MoveAbsolute(position_degree+nLoc);
    }

    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }
    else
    {
        ROTATE_LOG("[MoveIncremental_CW] Move Success");
    }

    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::MoveIncremental_CCW(int nLoc)
{
    int nError = NO_ERR;
    //AnsiString strRData;
    //AnsiString strSndData;

    //Sleep(20);
    //nError = IsCmdExec();

    if(NO_ERR == nError)
    {
        // 이동
        nError = MoveAbsolute(position_degree-nLoc);
    }

    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }
    else
    {
        ROTATE_LOG("[MoveIncremental_CCW] Move Success");
    }

    return nError;
}


/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::MoveAbsolute(int nLoc)
{
	//nError = IsCmdExec();
     //bCmdExec = true;
    if(NULL == m_pCommPort)
    {
		ROTATE_LOG("[MoveAbsolute] ERR_ROTATE_NOT_CONNECTED");
        return  ERR_ROTATE_NOT_CONNECTED;
	}
	if(false == bOrigin)
    {
		ROTATE_LOG("[MoveAbsolute] ERR_ROTATE_NOTORIGIN");
        return ERR_ROTATE_NOTORIGIN;
    }

	Sleep(20);
	AnsiString strRData;
	AnsiString strSndData;
	int nBeforeLoc = 0;

    // 2016-12-19, Add by Labeler
    // Rotate 진동 발생 억제 위해, 2단 가감속 동작하도록 이동 명령 변경
    if(nLoc > GetCurrentPositionDegree())
    {
        nBeforeLoc = (int)((nLoc - GetCurrentPositionDegree()) * 0.85) + GetCurrentPositionDegree();
    }
    else if(nLoc < GetCurrentPositionDegree())
    {
        nBeforeLoc = (int)((GetCurrentPositionDegree() - nLoc) * 0.15) + nLoc;
    }
    else
    {
        nBeforeLoc = nLoc;
    }

	nLoc = nLoc*ROTATE_PULSE_PER_DEGREE;
    nBeforeLoc = nBeforeLoc*ROTATE_PULSE_PER_DEGREE;
	strSndData.sprintf("%s%d,%s%d", ROTATE_MANUAL_LOC, nBeforeLoc, ROTATE_MANUAL_LOC2, nLoc);

	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);	// Critical Section Start *********
	int nError = SendCommand2(strSndData, strRData);

    // 2016-12-27, Add by Labeler
    // 간혹, 응답이 오지 않는 경우, Motor에 Stop 명령 발생 후, 재 명령 발생 처리 기능 추가
	if(NO_ERR != nError)
    {
        //bCmdExec = false;
		LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section Leave *********
		m_bCmdExec = false;

		Stop();

		m_bCmdExec = true;
		EnterCriticalSection(&m_CS_CmdExec);	// Critical Section Start *********

        Sleep(100);
        PurgeComm_s();
        //bCmdExec = true;
        // 2016-12-12, Add by Labeler
        // 간혹 Error 발생 경우 막기 위해 Sleep 추가
        Sleep(100);
		nError = SendCommand2(strSndData, strRData);

    }
    Sleep(20);

    if((NO_ERR == nError) || (ERR_ROTATE_NO_RESPONSE == nError))
    {
        nError = SendCommand2(ROTATE_MANUAL_MOVE, strRData);

        if(NO_ERR != nError)
        {
            // 2016-12-12, Add by Labeler
            // 간혹 Error 발생 경우 막기 위해 Sleep 추가
            Sleep(100);
            nError = SendCommand2(ROTATE_MANUAL_MOVE, strRData);
        }
    }
//    Sleep(100);

    PurgeComm_s();
	LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section Leave *********
	m_bCmdExec = false;

    if((NO_ERR != nError) && (ERR_ROTATE_NO_RESPONSE != nError))
    {
        bOrigin = false;
        m_nError = nError;
        ROTATE_LOG("Error[%d] MOVE ABSOLUTE", nError);
    }
    else
    {
        if(ERR_ROTATE_NO_RESPONSE == nError)
        {
            nError = NO_ERR;
            ROTATE_LOG("[MoveAbsolute] ERR_ROTATE_NO_RESPONSE for Skip");
        }
        else
        {
            ROTATE_LOG("[MoveAbsolute] Move Success");
        }
    }

    return nError;
}

/**
@brief   Motor에 구동 Command를 보내는 함수//
@author  Labeler//
@date    2016-11-28//
@return  정상인 경우 NO_ERR, 에러 발생시 에러코드 반환//
*/
// 이 함수를 호출하는 쪽에서 이미 m_CS_CmdExec 크리티컬섹션으로 보호된 상태에서 호출한다
int Rotate::SendCommand1(AnsiString strSendCommand, AnsiString &strRcvData)
{
    int nError = NO_ERR;
    char ReadBuff[1024] = {0, };
   	char strMsg[1024] = {0,};
    AnsiString strReceiveData = "";

    if(NULL == m_pCommPort)
    {
        nError = ERR_ROTATE_NOT_CONNECTED;
    }

    try
    {
        if(NO_ERR == nError)
        {
            sprintf(strMsg, "%s%c%c", strSendCommand, CR, LF);
            PurgeComm_s();
            if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
            {
                nError = ERR_ROTATE_WRITE;
            }
        }
        if(NO_ERR == nError)
        {
            Sleep(COMM_WAIT_TIME);
            if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
            {
                nError = ERR_ROTATE_NO_RESPONSE;
            }
        }
    }
    __finally
    {

    }

    if(NO_ERR == nError)
    {
        strReceiveData = ReadBuff;
        if(0 == strReceiveData.Pos(strSendCommand))
        {
            nError = ERR_ROTATE_INCORRECT_RESPONSE;
        }
        if(NO_ERR == nError)
        {
            strRcvData = strReceiveData;
        }
    }

    if((strSendCommand == ROTATE_STOP) && (ERR_ROTATE_NO_RESPONSE == nError))
    {
        return NO_ERR;
    }
    else if(NO_ERR != nError)
    {
        ROTATE_LOG("chk1[E%d] Snd=%s, Rcv=%s", nError, strSendCommand, strReceiveData);
    }

    return nError;
}

/**
@brief   Motor에 ABS 관련 구동 Command를 보내는 함수//
@author  Labeler//
@date    2016-11-28//
@return  정상인 경우 NO_ERR, 에러 발생시 에러코드 반환//
*/
// 최소 250ms 이상 Deley가 생기는 block 함수
// 이 함수를 호출하는 쪽에서 이미 m_CS_CmdExec 크리티컬섹션으로 보호된 상태에서 호출한다
int Rotate::SendCommand2(AnsiString strSendCommand, AnsiString &strRcvData)
{
	int nError = NO_ERR;
    char ReadBuff[1024] = {0, };
   	char strMsg[1024] = {0,};
    AnsiString strReceiveData = "";
    AnsiString strData1, strData2;
    DWORD dwStartTime;

    if(NULL == m_pCommPort)
    {
        nError = ERR_ROTATE_NOT_CONNECTED;
    }

    try
    {
        if(NO_ERR == nError)
        {
			PurgeComm_s();
            Sleep(50);

            sprintf(strMsg, "%s%c%c", strSendCommand, CR, LF);
			if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)))
            {
                nError = ERR_ROTATE_WRITE;
                ROTATE_LOG("[SendCommand2] ERR_ROTATE_WRITE");
            }
        }
        if(NO_ERR == nError)
        {
            Sleep(200/*COMM_WAIT_TIME*/);
			if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
            {
				strReceiveData = ReadBuff;
				nError = ERR_ROTATE_NO_RESPONSE;
                ROTATE_LOG("[SendCommand2] ERR_ROTATE_NO_RESPONSE1");
            }
        }

        if(NO_ERR == nError)
        {
            strReceiveData = ReadBuff;
			if(0 == strReceiveData.Pos(strMsg))
            {
                nError = ERR_ROTATE_INCORRECT_RESPONSE;
				ROTATE_LOG("[SendCommand2] ERR_ROTATE_INCORRECT_RESPONSE2");
            }
        }
    }
    __finally
    {

    }

    if(NO_ERR != nError)
    {
        strData1 = strMsg;
        strData2 = strRcvData;
        if(strData1.Trim() == strData2.Trim())
        {
            nError = NO_ERR;
        }
    }

    if(NO_ERR != nError)
    {
        ROTATE_LOG("chk2[E%d] Snd=%s, Rcv=%s", nError, strSendCommand, strReceiveData);
    }

    return nError;
}

int Rotate::GetCurrentPosition()
{
    return position_cts;
}

int Rotate::GetCurrentPositionDegree()
{
	MASK_AXIS	*pMaskOfAxis;

	pMaskOfAxis = m_pHWCommon->GetMask()->getMaskOfAxis(ROTATE_AXIS_NUM);

    if(NULL != pMaskOfAxis)
    {
        if(true == pMaskOfAxis->Position.bUse)
        {
            return pMaskOfAxis->Position.Value;
        }
    }

    return position_degree;
}

double Rotate::GetCurrentVelocity()
{
    return Velocity_ctsPsec;
}

int Rotate::GetError()
{
	MASK_AXIS	*pMaskOfAxis;

	pMaskOfAxis = m_pHWCommon->GetMask()->getMaskOfAxis(ROTATE_AXIS_NUM);

	if(NULL != pMaskOfAxis)
    {
        if(true == pMaskOfAxis->ErrorCode.bUse)
        {
            return pMaskOfAxis->ErrorCode.Value;
        }
    }

    return m_nError;
}

bool Rotate::IsAMPEnable()
{
	MASK_AXIS	*pMaskOfAxis;

	pMaskOfAxis = m_pHWCommon->GetMask()->getMaskOfAxis(ROTATE_AXIS_NUM);

	if(NULL != pMaskOfAxis)
    {
		if(true == pMaskOfAxis->AmpEnable.bUse)
        {
            return pMaskOfAxis->AmpEnable.Value;
        }
	}

	return bMotorOn;
}
//bool Rotate::IsRotate0Sensor()
//{
//	return m_pHWCommon->m_Input.Rotate_0_Sensor;
//}
//bool Rotate::IsRotate90Sensor()
//{
//	 return m_pHWCommon->m_Input.Rotate_90_Sensor;
//}
//bool Rotate::IsRotate180Sensor()
//{
//	 return m_pHWCommon->m_Input.Rotate_180_Sensor;
//}
//bool Rotate::IsRotate270Sensor()
//{
//	return m_pHWCommon->m_Input.Rotate_270_Sensor;
//}

bool Rotate::IsRunning()
{
	MASK_AXIS	*pMaskOfAxis;

	pMaskOfAxis = m_pHWCommon->GetMask()->getMaskOfAxis(ROTATE_AXIS_NUM);

    if(NULL != pMaskOfAxis)
    {
        if(true == pMaskOfAxis->Running.bUse)
        {
            return pMaskOfAxis->Running.Value;
		}
    }

    return bRunning;
}

bool Rotate::IsOrigin()
{
	MASK_AXIS	*pMaskOfAxis;

	pMaskOfAxis = m_pHWCommon->GetMask()->getMaskOfAxis(ROTATE_AXIS_NUM);

    if(NULL != pMaskOfAxis)
    {
		if(true == pMaskOfAxis->Origin.bUse)
        {
			return pMaskOfAxis->Origin.Value;
        }
	}

	return bOrigin;
}

int Rotate::ResetRotate()
{
// Motor Diable 후 Enable 할 경우, Error Clear 됨
	Disable();
    Sleep(20);
    Enable();
}


// RS232 Serial 통신 Thread Safe 함수 **************
//


void Rotate::PurgeComm_s()
{
	PurgeComm(m_pCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
}
