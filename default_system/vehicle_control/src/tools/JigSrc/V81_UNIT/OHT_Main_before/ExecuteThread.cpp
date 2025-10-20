//---------------------------------------------------------------------------
#include "ExecuteThread.h"
//---------------------------------------------------------------------------

#include "OHTMain.h"
#include "TestLog.h"
#include "LogHandler.h"	// Log 작성 목적
#include "MyTools.h"
#define PAUSED_BY_TP       		1
#define PAUSED_BY_OCS      		2

#define DRIVING_CONTROL_MODULE	0
#define TRANS_CONTROL_MODULE	1
#define MANUAL_CONTROL_MODULE	2

#define MAX_BYTE_SIZE_OF_EXECUTE_THREAD_LOG	1024*10

//#define ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\Execute.log", __VA_ARGS__)
//수정
#define ADD_LOG(...)   WRITE_LOG_UNIT(pLogExecuteThread, __VA_ARGS__)

/**
@brief	 Motion 동작을 실행하는 함수
@author  ehoto
@date	 2012.11.09
@param	pExecuteInfo: Main Thread와 Execute Thread 간의 공유 변수
@param	pDrivingControl: Driving Module
@param	pTransControl: Trans Module
@param	pManualControl: Manual Module
*/
ExecuteThread::ExecuteThread():ThreadCtrl()
{
	pLogExecuteThread = getLogUnit("D:\\log\\Execute.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_bReady									= false;
	m_pExecuteInfo 								= OHTMainForm->GetExecuteInfo();
	m_pModuleControl[DRIVING_CONTROL_MODULE]	= OHTMainForm->GetDrivingControl();
	m_pModuleControl[TRANS_CONTROL_MODULE]		= OHTMainForm->GetTransControl();
	m_pModuleControl[MANUAL_CONTROL_MODULE]		= OHTMainForm->GetManualControl();
	m_pHWCommon									= OHTMainForm->GetHWCommon();
	
	if(		m_pExecuteInfo	!= NULL
		&& 	m_pModuleControl[DRIVING_CONTROL_MODULE]	!= NULL
		&& 	m_pModuleControl[TRANS_CONTROL_MODULE]		!= NULL
		&& 	m_pModuleControl[MANUAL_CONTROL_MODULE]		!= NULL		)
	{
		m_bReady=true;
	}
	m_nError = 0;

	for(int n=0; n<3; n++)
	{
		m_Status[n]			= STS_INIT;
		m_ModuleError[n]	= NO_ERR;
	}
}

ExecuteThread::~ExecuteThread(void)
{

}
//---------------------------------------------------------------------------
/**
@brief	 RealThreadProc Thread 함수

	1. 각 Unit의 Status 확인
	2. Quick CMD 처리 (긴급 명령 등)
	3. 명령 타입에 따라 해당되는 Unit의 작업 실시 (D/T/M 확인)
		1) Driving 처리
		2) Trans 처리
		3) Manual 처리
	4. 동작 이상 시 정지 (Quick CMD 에서 Stop 처리)
	5. 각 Unit의 Status 및 Error를 공유 변수에 업데이트
@author  ehoto
@date	 2012.11.09
*/
void ExecuteThread::RealThreadProc()
{
	int		nTmpError	= NO_ERR;
	int		nIndex		= m_pExecuteInfo->ExecuteUnit;
	int		nCount		= 0;
	TaskControl* m_pControlTask = OHTMainForm->GetTaskControl();
    HWSet * pHWSet = OHTMainForm->GetHWSet();
    bool bNeedResume = false;

	//---- Place thread code here ----
	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker==true)	break;	// Thread 종료를 위해 필수로 추가
		
		//2014.04.15 doori.shin Thread Check를 위한 Tick값 증가
		if(m_Tick == MAX_TICK)
			m_Tick = 0;
		else
			m_Tick++;


		if(m_bReady==true)
		{
			nTmpError = NO_ERR;

			//1 1. Status 확인
			nTmpError=updateStatus();

            // 1.2. Pause 상태 Check 및 해제 처리
            // 2017-01-09, Add by Labeler
            if((m_pExecuteInfo->bTpPause == false)
            && (m_pExecuteInfo->bOcsPause == false)
            && (pHWSet->m_pCID->GetCIDPause() == false))
            {
                if(bNeedResume == true)
                {
                    m_pHWCommon->SetPauseControl(false, PAUSED_BY_OCS);
                    bNeedResume = false;
                }
            }
            else
            {
                bNeedResume = true;
            }

			//1 3. Quick CMD 처리
			if(m_pExecuteInfo->QuickCmd!=EXECUTE_QUICK_CMD_NONE)
			{

				ADD_LOG("QuickCmd:%d Info(Status:%d, Error:%d, Mode:%d, DTM:%d, Count:%d)"
					, m_pExecuteInfo->QuickCmd
					, m_pExecuteInfo->StatusCmdId.Info.Status
					, m_pExecuteInfo->ErrCode_ExecuteInfo
					, m_pExecuteInfo->ExecuteMode
					, m_pExecuteInfo->ExecuteUnit
					, m_pExecuteInfo->ListOfCmd.getCount());
				CS_CmdList->Acquire();
				// E-Stop의 경우는 에러 없을 경우에만, 그외믄 무조건 실행(Reset 등)
				if(m_nError==NO_ERR || m_pExecuteInfo->QuickCmd!=EXECUTE_QUICK_CMD_STOP)
				{
					handleQuickCommand(m_pExecuteInfo->QuickCmd);
					
					if(m_pExecuteInfo->QuickCmd==EXECUTE_QUICK_CMD_STOP)
					{
						if(m_pExecuteInfo->QuickCmdEStopCommander == ESTOP_BY_TP)
						{
							m_nError = ERR_TP_ESTOP;
						}
						else
						{
							if(m_pExecuteInfo->QuickCmdEStopType == EXIT_ROUTE_OCS_PATH)		    	m_nError = ERR_OCS_ESTOP1;
							else if(m_pExecuteInfo->QuickCmdEStopType == OHT_EXIST_IN_COLLISON_AREA)	m_nError = ERR_OCS_ESTOP2;
							else if(m_pExecuteInfo->QuickCmdEStopType == CARRIER_EXIST_4_UNLOADING)	    m_nError = ERR_OCS_ESTOP3;
							else if(m_pExecuteInfo->QuickCmdEStopType == NOT_CARRIER_4_LOADING)		    m_nError = ERR_OCS_ESTOP4;
							else if(m_pExecuteInfo->QuickCmdEStopType == TRUNOFF_STOP)		 			m_nError = NO_ERR;
							else if(m_pExecuteInfo->QuickCmdEStopType == TRUNON)						m_nError = NO_ERR;
							else																		m_nError = ERR_OCS_ESTOP5;
						}

                        // STOP 명령 발생 시, OCS, TP Pause 상태일 경우, Pasue Flag 해제
                        // 2017-01-09, Add by Labeler
                        if((m_pExecuteInfo->bTpPause != false) || (m_pExecuteInfo->bOcsPause != false) || (pHWSet->m_pCID->GetCIDPause() != false))
                        {
                            m_pExecuteInfo->bTpPause = false;
                            m_pExecuteInfo->bOcsPause = false;
                            pHWSet->m_pCID->SetCIDPause(false);
                        }
					}
				}

				m_pExecuteInfo->QuickCmd=EXECUTE_QUICK_CMD_NONE;
				CS_CmdList->Release();
			}

			//1 4. D/T/M 확인
//			ADD_LOG("handleModuleControl m_pExecuteInfo->ExecuteUnit=%d, FlagMainThreadUsing=%d, Count=%d", 
//    			m_pExecuteInfo->ExecuteUnit, m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing, m_pExecuteInfo->ListOfCmd.getCount() );
			if(	(m_pExecuteInfo->ExecuteUnit!=EXECUTE_UNIT_NONE)	&&
				(m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing==false) 		&& 
				(m_pExecuteInfo->ListOfCmd.getCount()>0)		)	// 명령이 입력 중이 아니고 명령이 없으면 SKIP
			{
				switch(m_pExecuteInfo->ExecuteUnit)
				{
				case EXECUTE_UNIT_DRIVING:
					nIndex = DRIVING_CONTROL_MODULE;
					nCount = 10;	// Buffer 의 값을 최대 10개 받아온다 ~ 무한히 받아올 경우 구동에 딜레이 발생 가능
					break;
				case EXECUTE_UNIT_TRANS:
					nIndex = TRANS_CONTROL_MODULE;
					nCount = 1;	// Buffer 의 값을 1개만 받아온다 ~ 이적재에는 연속 명령 동작 없음: 에러 처리 검토 필요
					break;
				case EXECUTE_UNIT_MANUAL:
					nIndex = MANUAL_CONTROL_MODULE;
					nCount = 1;	// Buffer 의 값을 1개만 받아온다 ~ Manual 명령에는 연속 명령 없음 가정
					break;
				default:
					break;
				}

				nTmpError = checkModules(nIndex, m_pExecuteInfo);
				
				if(nTmpError==NO_ERR)
				{
					nTmpError=handleModuleControl(nIndex, m_pExecuteInfo, nCount);
				}
			}
			else
			{
				switch(m_pExecuteInfo->ExecuteUnit)
				{
				case EXECUTE_UNIT_DRIVING:
					nIndex = DRIVING_CONTROL_MODULE;
					break;
				case EXECUTE_UNIT_TRANS:
					nIndex = TRANS_CONTROL_MODULE;
					break;
				case EXECUTE_UNIT_MANUAL:
				default:
					nIndex = MANUAL_CONTROL_MODULE;
					break;
				}
				
				nTmpError=checkModules(nIndex, m_pExecuteInfo);
				
				if(nTmpError==NO_ERR && m_pModuleControl[nIndex]->IsOnProcess()==true)
				{
					try{
					nTmpError = m_pModuleControl[nIndex]->ExecuteJob();	// 추가 명령 없이 현재 명령 실행
					}catch(...)
                       	ADD_LOG("Excute 22 Exception!!!");
				}				

			}

			//1 5. 동작 이상 시 정지
			if(nTmpError==NO_ERR)
			{
				nTmpError=updateStatus();
			}
			
			if(		(m_nError==NO_ERR)
				&&	(nTmpError!=NO_ERR)
				&&	(isOnAutoRecovery(DRIVING_CONTROL_MODULE, nTmpError)==false)
				&&	(isOnAutoRecovery(TRANS_CONTROL_MODULE, nTmpError)==false)
				&&	(isOnAutoRecovery(MANUAL_CONTROL_MODULE, nTmpError)==false)
			)
			{
				ADD_LOG("isOnAutoRecovery:TempE:%d",nTmpError);
				m_nError	= nTmpError;

				handleQuickCommand(EXECUTE_QUICK_CMD_STOP);
                
//                if(m_pExecuteInfo->bJogInterlockIgnore == true)
//                {
//	                EnterCriticalSection(&OHTMainForm->m_CS);
//                    m_pExecuteInfo->bJogInterlockIgnore = false;
//                    LeaveCriticalSection(&OHTMainForm->m_CS);
//                }
    		}			

			//1 6. Status 를 공유 변수에 업데이트
			transferDataToMainThread(m_pExecuteInfo);
		}
       //AOHC-204 Thread 별 수행 시간 감시 기능
		static TimeChk timeChk;
		static ThreadTimeMonitor tmon;
		timeChk.End(0);

		tmon.Execute("ExecT", timeChk.Get(0));

		timeChk.Start(0);

		Sleep(5);

	}
}

//---------------------------------------------------------------------------
/**
@brief	 RealThreadProc Thread 함수
@author  ehoto
@date	 2012.11.09
*/
UINT ExecuteThread::updateStatus(void)
{
	for(int n=0; n<3; n++)
	{
		m_ModuleError[n]=m_pModuleControl[n]->GetError();	
		m_Status[n]=m_pModuleControl[n]->GetStatus();		
	}

	for(int n=0; n<3; n++)
	{
		if(m_ModuleError[n]!=NO_ERR)
		{
			return m_ModuleError[n];
		}
	}

	//@todo:위치 ID 업데이트 필요
	
	
	return NO_ERR;
}


//---------------------------------------------------------------------------
/**
@brief	 ExecuteThread 우선순위 높은 명령 실행 함수

	아래의 명령을 전 모듈로 전달함
	EXECUTE_QUICK_CMD_NONE = 0, 		///< 초기화 상태		A/M
	EXECUTE_QUICK_CMD_STOP = 1, 		///< 긴급 정지			A/M 	~ OCS_Estop / Status Check 오는 Internal Cmd 가 E-stop / 
	EXECUTE_QUICK_CMD_PAUSE = 2,		  ///< 일시 정지			A/M 	~ OCS_Pause  
	EXECUTE_QUICK_CMD_RESUME = 3,		  ///< 재 실행			A/M 	~ OCS_Resume 
	EXECUTE_QUICK_CMD_CANCEL = 4,		 ///< 주행 명령 Clear		 A/M	 ~ OCS 에 경로 변경할 경우
	EXECUTE_QUICK_CMD_RESET = 5   

@author  ehoto
@date	 2012.11.09
@param	CmdType : 명령 타입 
@return	에러 응답 ( 0:정상 완료 / 그외:에러코드 )

*/
UINT ExecuteThread::handleQuickCommand(EXECUTE_QUICK_CMD CmdType)
{
	if(CmdType==EXECUTE_QUICK_CMD_RESET)
	{
		m_nError = NO_ERR;
	}

	switch(CmdType)
	{
		case EXECUTE_QUICK_CMD_PAUSE_OCS :
			m_pHWCommon->SetPauseControl(true, PAUSED_BY_OCS);
            m_pExecuteInfo->bOcsPause = true;
			return NO_ERR;
		case EXECUTE_QUICK_CMD_RESUME_OCS :
// 2016-12-29, Add by Labeler
// OCS, TP, CID Pause가 모두 해제 되어야지만 Resume 명령 발생시키기 위해 주석 처리
//			m_pHWCommon->SetPauseControl(false, PAUSED_BY_OCS);
            m_pExecuteInfo->bOcsPause = false;
			return NO_ERR;
		case EXECUTE_QUICK_CMD_PAUSE_TP :
			m_pHWCommon->SetPauseControl(true, PAUSED_BY_TP);
            m_pExecuteInfo->bTpPause = true;
			return NO_ERR;
		case EXECUTE_QUICK_CMD_RESUME_TP :
// 2016-12-29, Add by Labeler
// OCS, TP, CID Pause가 모두 해제 되어야지만 Resume 명령 발생시키기 위해 주석 처리
//			m_pHWCommon->SetPauseControl(false, PAUSED_BY_TP);
            m_pExecuteInfo->bTpPause = false;
			return NO_ERR;
		default :
			break;
	}
	
	for(int n=0; n<3; n++)
	{
		m_ModuleError[n]=m_pModuleControl[n]->SetQuickCmd(CmdType);
	}

	for(int n=0; n<3; n++)
	{
		if(m_ModuleError[n]!=EXECUTE_ACK_NO_ERR)
		{
			return m_ModuleError[n];
		}
	}
	
	return NO_ERR;	
}

//---------------------------------------------------------------------------
/**
@brief	 RealThreadProc Thread 함수

	1. 에러 및 상태 확인
	2. ExecuteInfo의 명령 리스트 확인하여 명령존재 시 ModuleControl로 명령 전달
	3. ExecuteJob 함수 호출
@author  ehoto
@date	 2012.11.09
@param	nIndex: Index of Module
@param	pExecuteInfo: 참조할 공유 변수의 포인터
@param	nMaxCountGetCmd: 공유변수에서 명령을 추출할 최대 개수
@return	에러 응답 ( 0:정상 완료 / 그외:에러코드 )

*/
UINT ExecuteThread::handleModuleControl(int nIndex, EXECUTE_INFO *pExecuteInfo, UINT nMaxCountGetCmd)
{
	UINT nTmpError=NO_ERR;
	BYTE Status = 0;
	UINT nCount = 0;
	UINT nTmpCountGetCmd=nMaxCountGetCmd;
	void* 		pCmd = NULL;
	EXECUTE_ACK TmpExecuteAck = 0;
	VirtualModuleControl *pModuleControl = m_pModuleControl[nIndex];

	// 1. 에러 및 상태 확인
	Status		= pModuleControl->GetStatus();
	nTmpError	= pModuleControl->GetError();

	if((m_pExecuteInfo->ExecuteMode != EXECUTE_MODE_MANUAL) || (!m_pExecuteInfo->bJogInterlockIgnore))	
	{
		if(nTmpError!=NO_ERR || Status == STS_ERROR) //|| Status == STS_TURN_OFF)
		{
			// 잔여 명령 제거
			deleteCommand(nIndex, pExecuteInfo);
			return nTmpError;
		}
	}	

	// 2. ExecuteInfo의 명령 리스트 확인하여 명령존재 시 ModuleControl로 명령 전달
	nCount = pExecuteInfo->ListOfCmd.getCount();

	if((pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing==false) && (nCount>0))
	{
		CS_CmdList->Acquire();

		if(nCount>nTmpCountGetCmd)
		{
			nCount = nTmpCountGetCmd;	// 10개 이상 노드를 투입하지 않는다 -> 다를 동작에 지연 초래 방지
		}

		ADD_LOG("Excute Count %d",nCount);

		for(int n=0; n<nCount; n++)
		{
			pCmd = pExecuteInfo->ListOfCmd.referFront();
//			ADD_LOG("Excute address %X",pCmd);
			if(pCmd ==NULL)
			{
 				pCmd = pExecuteInfo->ListOfCmd.referFront();     //절대 지우지 말것.
			}

			TmpExecuteAck = pModuleControl->InsertGeneralCmd(pCmd);
			switch(TmpExecuteAck)
			{
			case EXECUTE_ACK_NO_ERR:	// 정상 처리 완료
				pExecuteInfo->ListOfCmd.popFront();
				break;
			case EXECUTE_ACK_BUSY:		// 구동부 busy 일 경우 다음 번에 명령 하달
				n=nCount;				// for 문 벗어남
				break;
			case EXECUTE_ACK_NOT_AVAILABLE_CMD:	// 불가한 명령 일 경우 명령 삭제
			case EXECUTE_ACK_ON_ERR:
				deleteCommand(nIndex, pExecuteInfo);
				n=nCount;				// for 문 벗어남
				break;
			}
		}

		CS_CmdList->Release();
	}

	updateStatus();
	transferDataToMainThread(pExecuteInfo); 	// 임시: 동작 전 상태 변경 먼저 진행 하기 위해 

	// 3. ExecuteJob 함수 호출
	if(pModuleControl->IsOnProcess()==true)
	{
		nTmpError = pModuleControl->ExecuteJob();
	}

	return nTmpError;
		
}


//---------------------------------------------------------------------------
/**
@brief	 CheckAbnormal 함수

	1. D/T/M 명령에 따른 다른 모듈에서의 비정상 동장 상태 확인
@author  임태웅
@date	 2013.02.08
@param	nIndex: Index of Module
@return 에러 응답 ( 0:정상 / 그외:에러코드 )
*/										
UINT ExecuteThread::checkModules(int nIndex, EXECUTE_INFO * pExecuteInfo)
{

	UINT nTMPError	=NO_ERR;
	int nMouduleIndex[3]	= {0,};
	int nCountOfModule	= 0;
	EXECUTE_MODE Mode = pExecuteInfo->ExecuteMode;

	if(	Mode==EXECUTE_MODE_AUTO &&
		m_pModuleControl[TRANS_CONTROL_MODULE]->IsOnProcess()==false &&
		m_pModuleControl[DRIVING_CONTROL_MODULE]->IsOnProcess()==false
		)
	{
		nMouduleIndex[0]	= MANUAL_CONTROL_MODULE;
		nMouduleIndex[1]	= TRANS_CONTROL_MODULE;
		nMouduleIndex[2]	= DRIVING_CONTROL_MODULE;
		nCountOfModule	= 3;
	}
	else
	{
		switch(nIndex)
		{
		case DRIVING_CONTROL_MODULE:
			nMouduleIndex[0]	= MANUAL_CONTROL_MODULE;    //기존에 메뉴얼때 하는게 없음
			nMouduleIndex[1]	= TRANS_CONTROL_MODULE;
			nCountOfModule	= 2;
			break;
		case TRANS_CONTROL_MODULE:
			nMouduleIndex[0]	= MANUAL_CONTROL_MODULE;    //기존에 메뉴얼때 하는게 없음
			nMouduleIndex[1]	= DRIVING_CONTROL_MODULE;
			nCountOfModule	= 2;
			break;
		case MANUAL_CONTROL_MODULE:
			nMouduleIndex[0]	= MANUAL_CONTROL_MODULE;   
			nCountOfModule	= 1;
			break;
		}
	}

	//2	상시 확인 내용
	try{
	if(nTMPError==NO_ERR)	nTMPError = m_pModuleControl[MANUAL_CONTROL_MODULE]->CheckAlways();
	if(nTMPError==NO_ERR)	nTMPError = m_pModuleControl[DRIVING_CONTROL_MODULE]->CheckAlways();
	if(nTMPError==NO_ERR)	nTMPError = m_pModuleControl[TRANS_CONTROL_MODULE]->CheckAlways();
	}catch(...)
    	ADD_LOG("Excute Exception!!!");
	//2	대기 중 다른 Unit 이 동작 중일 때 확인 하는 내용
	if(nTMPError==NO_ERR)
	{
		for(int nTmpIndex=0; nTmpIndex<nCountOfModule; nTmpIndex++)
		{
			nTMPError = m_pModuleControl[nMouduleIndex[nTmpIndex]]->CheckAbnormalOnWait();
			if(nTMPError!=NO_ERR)	break;
		}
	}
	
	if((nTMPError == NO_ERR) && (m_pExecuteInfo->ExecuteMode == EXECUTE_MODE_MANUAL) && (m_pExecuteInfo->bJogInterlockIgnore))
	{
		nTMPError = NO_ERR;
	}
	else
	{
//		if(m_pExecuteInfo->bJogInterlockIgnore == true)
//		{
//			EnterCriticalSection(&OHTMainForm->m_CS);
//			m_pExecuteInfo->bJogInterlockIgnore = false;
//			LeaveCriticalSection(&OHTMainForm->m_CS);
//		}
		
		//2	에러 발생 시 명령 삭제
		if(nTMPError!=NO_ERR)
		{
			deleteCommand(nIndex, pExecuteInfo);
		}
	}
	
	return nTMPError;
		
}

/**
@brief	 CheckAbnormal 함수

	1. D/T/M 명령에 따른 다른 모듈에서의 비정상 동장 상태 확인
@author  임태웅
@date	 2013.02.08
@param	nIndex: Index of Module
@return 에러 응답 ( 0:정상 / 그외:에러코드 )
*/										
UINT ExecuteThread::deleteCommand(int nIndex, EXECUTE_INFO *pExecuteInfo)
{
	int		nTMPError 	= NO_ERR;
	UINT	nCount 		= 0;
	void	*pCmd 		= NULL;
	VirtualModuleControl *pModuleControl = m_pModuleControl[nIndex];

	nCount = pExecuteInfo->ListOfCmd.getCount();
	CS_CmdList->Acquire();


	for(int n=0; n<nCount; n++)
	{
		pCmd = pExecuteInfo->ListOfCmd.popFront();
		if(pCmd!=NULL)
		{
			if(pModuleControl->DeleteCmd(pCmd)==false) break;	//@todo: 잘못된 메모리 처리에 대한 처리 필요 가능성 있음
		}
	}
	CS_CmdList->Release();
	return nTMPError;

}

//---------------------------------------------------------------------------
/**
@brief	 RealThreadProc Thread 함수

	1. Status						// I, A, G, L, O, U, N ...
	2. nErrorCode
	3. CurrentDrivingNode			// 주행 노드 ID
	4. CurNodeOffset		// 주행 노드에서의 Offset 거리 : 소수점 절사(mm)
	5. CurrentTransNode			//< 이적재 노드 : 마크 후에만 업데이트
@author  ehoto
@date	 2012.11.09
@param	pExecuteInfo: 참조할 공유 변수의 포인터
@return	에러 응답 ( 0:정상 완료 / 그외:에러코드 )
*/
UINT ExecuteThread::transferDataToMainThread(EXECUTE_INFO *pExecuteInfo)
{
	UINT nTMPError=NO_ERR;
	BYTE TmpStatus=STS_INIT;
	EXECUTE_INFO *pTmpExecuteInfo = pExecuteInfo;
	STATUS_CMD_ID_UNION	CurrStatusCmdID;
	STATUS_CMD_ID_UNION	TmpStatusCmdID;
	CMD_ON_ADDING_UNION CurrOnAddingCmd;
	EXECUTE_UNIT		CurrExecuteUnit = pTmpExecuteInfo->ExecuteUnit;
	int nCmdCount = pTmpExecuteInfo->ListOfCmd.getCount();
	VirtualDrivingControl* pDrivingControl = ((VirtualDrivingControl*)m_pModuleControl[DRIVING_CONTROL_MODULE]);
	VirtualTransControl* pTransControl = ((VirtualTransControl*)m_pModuleControl[TRANS_CONTROL_MODULE]);

	//1	0. 기본 정보 업데이트
	pExecuteInfo->ExecutePositionInfo = pDrivingControl->GetPositionInfo();
	pDrivingControl->GetMapData(&(pExecuteInfo->ListOfMapInfo), &(pExecuteInfo->OnAddingMapInfo));
	pExecuteInfo->TransDifference = pTransControl->getDifference();

	//1	1. Status
	//1	2. nErrorCode
	CurrStatusCmdID.Data = pTmpExecuteInfo->StatusCmdId.Data;
	TmpStatusCmdID.Data	= CurrStatusCmdID.Data;
	CurrOnAddingCmd.Data = pTmpExecuteInfo->OnAddingCmd.Data;
	if(m_nError==NO_ERR)
	{
		for(int n=0; n<3; n++)
		{
//			if(m_ModuleError[n]!=NO_ERR &&m_nError==NO_ERR )
//			{
//				m_nError = m_ModuleError[n];
//			}
			if(m_Status[n]==STS_ERROR && TmpStatus!=STS_ERROR)
			{
				if(m_ModuleError[n]!=NO_ERR &&m_nError==NO_ERR )
				{
					m_nError = m_ModuleError[n];
				}
				TmpStatus = m_Status[n];
			}
		}
	}

	if(m_nError!=NO_ERR||TmpStatus==STS_ERROR)
	{
		TmpStatusCmdID.Info.Status 			= STS_ERROR;
		pTmpExecuteInfo->StatusCmdId.Data	= TmpStatusCmdID.Data;	// int 단위 변경 //Info.Status = STS_ERROR;
		pTmpExecuteInfo->ErrCode_ExecuteInfo= m_nError;
	}
	else
	{
		TmpStatus = STS_INIT;
		nTMPError = NO_ERR;

		switch(pTmpExecuteInfo->ExecuteUnit)
		{
		case EXECUTE_UNIT_DRIVING:
			{
				TmpStatus=m_Status[DRIVING_CONTROL_MODULE];
				nTMPError=m_ModuleError[DRIVING_CONTROL_MODULE];
			}
			break;
		case EXECUTE_UNIT_TRANS:
			{
				TmpStatus=m_Status[TRANS_CONTROL_MODULE];
				nTMPError=m_ModuleError[TRANS_CONTROL_MODULE];
			}
			break;
		case EXECUTE_UNIT_MANUAL:
			{
				TmpStatus=m_Status[MANUAL_CONTROL_MODULE];
				nTMPError=m_ModuleError[MANUAL_CONTROL_MODULE];
			}
			break;
		default:
			break;
		}

		//2	Status 정보 만들기(Cmd ID 포함)
		if(	(TmpStatusCmdID.Info.Status == STS_ARRIVED		&& CurrExecuteUnit!=EXECUTE_UNIT_DRIVING)	||
			(TmpStatusCmdID.Info.Status == STS_UNLOADED		&& CurrExecuteUnit!=EXECUTE_UNIT_TRANS)		||
			(TmpStatusCmdID.Info.Status == STS_LOADED		&& CurrExecuteUnit!=EXECUTE_UNIT_TRANS)	  ||
			(TmpStatusCmdID.Info.Status == STS_AUTO_TAUGHT		&& CurrExecuteUnit!=EXECUTE_UNIT_TRANS)	)
		{
			// 완료 상태는 그전 상태 유지: D/T/M 변경에 따른 변경 방지
			if(	TmpStatus!=STS_ARRIVED	&& TmpStatus!=STS_UNLOADED	&& TmpStatus!=STS_LOADED  &&  TmpStatus!=STS_AUTO_TAUGHT &&
				TmpStatus!=STS_INIT		&& TmpStatus!=STS_COMPLETE	)
			{
				TmpStatusCmdID.Info.Status = TmpStatus;
			}
			else if(TmpStatus==STS_INIT	
				&&	m_Status[DRIVING_CONTROL_MODULE]==STS_INIT
				&&	m_Status[TRANS_CONTROL_MODULE]==STS_INIT
				&&	m_Status[MANUAL_CONTROL_MODULE]==STS_INIT
				)
			{
				TmpStatusCmdID.Info.Status = TmpStatus;
			}
		}
		else
		{
			TmpStatusCmdID.Info.Status = TmpStatus;
		}
		
		switch(TmpStatus)	//2	 현재 상황에 따른 Cmd ID 변경 
		{
		case STS_INIT:
			TmpStatusCmdID.Info.PreCmd 	= 0;
			TmpStatusCmdID.Info.CurrCmd	= 0;
			TmpStatusCmdID.Info.NextCmd	= 0;
			break;

		case STS_ARRIVED:
		case STS_UNLOADED:
		case STS_LOADED:
		case STS_COMPLETE:
		case STS_AUTO_TAUGHT:
			if(	(CurrOnAddingCmd.Info.FlagMainThreadUsing==false)	// 입력중이면 넘어감
			&&	(CurrStatusCmdID.Info.CurrCmd	!= 0)
			&&	(nCmdCount==0)
			)
			{
				TmpStatusCmdID.Info.PreCmd 	= CurrStatusCmdID.Info.CurrCmd;
				TmpStatusCmdID.Info.CurrCmd	= 0;
				TmpStatusCmdID.Info.NextCmd	= 0;
			}
			break;

		case STS_GOING:
		case STS_UNLOADING:
		case STS_LOADING:
		case STS_AUTOPOSITION:
		case STS_TURN_OFF: // 기도우오프
		case STS_RUNNING:
		case STS_AUTO_TEACHING:
		case STS_AUTO_TEACHING_NULLTYPE:
		case STS_AUTO_TRANS_HOMING:
			if( (CurrOnAddingCmd.Info.FlagMainThreadUsing==false)	// 입력이 완료된 명령에만 대응
			&&	(CurrOnAddingCmd.Info.CmdID!=CurrStatusCmdID.Info.CurrCmd)
            &&	(CurrOnAddingCmd.Info.CmdID!=0))
			{
                if(CurrStatusCmdID.Info.CurrCmd!=0)
                {
				    TmpStatusCmdID.Info.PreCmd = CurrStatusCmdID.Info.CurrCmd;
                }
				TmpStatusCmdID.Info.CurrCmd	= CurrOnAddingCmd.Info.CmdID;
				TmpStatusCmdID.Info.NextCmd	= 0;

				ADD_LOG("Status:%d CmdID(%d:%d:%d) AddingCmd(%d,%d) --> CmdID(%d:%d:%d)"
					, CurrStatusCmdID.Info.Status
					, CurrStatusCmdID.Info.PreCmd
					, CurrStatusCmdID.Info.CurrCmd
					, CurrStatusCmdID.Info.NextCmd
					, CurrOnAddingCmd.Info.FlagMainThreadUsing
					, CurrOnAddingCmd.Info.CmdID
					, TmpStatusCmdID.Info.PreCmd
					, TmpStatusCmdID.Info.CurrCmd
					, TmpStatusCmdID.Info.NextCmd
					);
			}

			break;
			
		case STS_ERROR:
		default:
			break;
		}
		
		pTmpExecuteInfo->StatusCmdId.Data		= TmpStatusCmdID.Data;
		pTmpExecuteInfo->ErrCode_ExecuteInfo	= nTMPError;
	}

	nTMPError = pTmpExecuteInfo->ErrCode_ExecuteInfo;
	return nTMPError;

	//@todo:위치 ID 업데이트 필요
	// 3. CurrentDrivingNode			// 주행 노드 ID
	// 4. CurNodeOffset		// 주행 노드에서의 Offset 거리 : 소수점 절사(mm)
	// 5. CurrentTransNode			//< 이적재 노드 : 마크 후에만 업데이트


}
//---------------------------------------------------------------------------


bool ExecuteThread::isOnAutoRecovery(int nIndex, int nError)
{
	bool bOnRetry = false;
	
	VirtualModuleControl *pModuleControl = m_pModuleControl[nIndex];

	if(((pModuleControl->GetStatus()==STS_AUTORECOVERY && pModuleControl->GetError()==nError)) ||
		((pModuleControl->GetStatus()==STS_AUTORECOVERY_OVERRUN && pModuleControl->GetError()==nError)) ||
		((pModuleControl->GetStatus()==STS_AUTORECOVERY_BLANKTRANS && pModuleControl->GetError()==nError)) ||
		((pModuleControl->GetStatus()==STS_AUTORECOVERY_DOUBLETRANS && pModuleControl->GetError()==nError)))
	{
		bOnRetry = true;
	}
	
	return bOnRetry;

}



