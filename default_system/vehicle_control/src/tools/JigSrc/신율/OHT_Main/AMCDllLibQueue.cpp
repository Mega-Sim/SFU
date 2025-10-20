// ---------------------------------------------------------------------------
/**
@file    AMCDllLibQueue.cpp
@version OHT 7.0
@brief   AMCDllLibQueue Class 소스 File
*/
// ---------------------------------------------------------------------------

#include "AMCDllLibQueue.h"

#define	INITIALIZE_CRITICAL_SECTION		InitializeCriticalSection(&m_CS)
#define	ENTER_CRITICAL_SECTION			EnterCriticalSection(&m_CS)
#define	LEAVE_CRITICAL_SECTION			LeaveCriticalSection(&m_CS)
#define	DELETE_CRITICAL_SECTION			DeleteCriticalSection(&m_CS)

#define	AMC_CMD_BUFFER_IS_NOT_ENOUGH	901
#define	MAX_BUFFER_COUNT	128

#define MAX_BYTE_SIZE_OF_AMC_LOG	1024*100

#define AMC_ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\AMC_Send.log", __VA_ARGS__)

/**
@brief   AMCDllLibQueue 초기화
@author  임태웅
@date    2013.01.21
*/
AMCDllLibQueue::AMCDllLibQueue()
{
	INITIALIZE_CRITICAL_SECTION;

	init();
}

/**
@brief   AMCDllLibQueue 소멸자
@author  임태웅
@date    2013.01.21
*/
AMCDllLibQueue::~AMCDllLibQueue()
{
	DELETE_CRITICAL_SECTION;
}

bool AMCDllLibQueue::init(void)
{
	bool bReturn = true;	
	QUEUEING_AMC_CMD *pQueuingAmcCmd = NULL;

	ENTER_CRITICAL_SECTION;
	while(m_LList.getCount()>0)
	{
		pQueuingAmcCmd = (QUEUEING_AMC_CMD*)m_LList.popFront() ;
		delete pQueuingAmcCmd;
	}
	m_nError = 0;
	m_nReportError= 0;
	LEAVE_CRITICAL_SECTION;

	return bReturn;
}

/**
@brief   User가 Buffer에 정보를 입력할 때 사용
@author  임태웅
@date    2013.01.21
@param   QueuingAmcCmd : 입력되는 명령 정보
@param   nError : 실패 시 반환하는 에러값
@return	 정상 처리되면 true 반환(Buffer에 아무 값이 없어 명령을 보내지 않았다면 이것도 정상처리이다)
*/
bool AMCDllLibQueue::putData(QUEUEING_AMC_CMD QueuingAmcCmd, int *nError)
{
	bool bReturn = true;
	int nTempError = 0;
	QUEUEING_AMC_CMD *pQueuingAmcCmd = NULL;
	int nTmpListCount;

	ENTER_CRITICAL_SECTION;
	nTmpListCount = m_LList.getCount();
	LEAVE_CRITICAL_SECTION;

	if(m_nError!=0)
	{
		for(int nloop = 0 ; nloop < nTmpListCount ; nloop++)
		{
			ENTER_CRITICAL_SECTION;
			pQueuingAmcCmd = (QUEUEING_AMC_CMD*)m_LList.popFront() ;
			LEAVE_CRITICAL_SECTION;

			delete pQueuingAmcCmd;
		}

        AMC_ADD_LOG("[putData]Error:%d", m_nError);
		return m_nError;	// 에러 발생 시 명령 추가 하지 않음
	}
	else if(nTmpListCount >= MAX_BUFFER_COUNT)
	{
		if((nTmpListCount % MAX_BUFFER_COUNT) ==0)
			AMC_ADD_LOG("[putData]AMC_CMD_BUFFER_IS_NOT_ENOUGH, Buffer:%d", nTmpListCount);
	}
	else if(nTmpListCount >= (MAX_BUFFER_COUNT*2))
	{
		for(int nloop = 0 ; nloop < nTmpListCount ; nloop++)
		{
			ENTER_CRITICAL_SECTION;
			pQueuingAmcCmd = (QUEUEING_AMC_CMD*)m_LList.popFront();
			LEAVE_CRITICAL_SECTION;

			delete pQueuingAmcCmd;
		}

//		m_nError = AMC_CMD_BUFFER_IS_NOT_ENOUGH;
		m_nReportError = ERR_WINDOWEXCEPTION;
		nTempError = AMC_CMD_BUFFER_IS_NOT_ENOUGH;
        AMC_ADD_LOG("[putData]AMC_CMD_BUFFER_IS_NOT_ENOUGH:%d, Buffer full:%d", m_nError, nTmpListCount);
	}

	pQueuingAmcCmd = new QUEUEING_AMC_CMD();
	memcpy(pQueuingAmcCmd, &QueuingAmcCmd, sizeof(QUEUEING_AMC_CMD));

	///@todo: m_nCount가 특정 값보다 클 경우 전부 제거하고 에러 발생: AMC Buffer 용량 초과
	ENTER_CRITICAL_SECTION;
	m_LList.insertBack((void*)pQueuingAmcCmd);
	LEAVE_CRITICAL_SECTION;


	*nError = nTempError;
	return bReturn;
}

int AMCDllLibQueue::GetReportError()
{
	return m_nReportError;
}

/**
@brief   AMC로 주기적으로 보낼 명령 구조체를 만들 때 사용
@author  임태웅
@date    2013.01.21
@param   pAmcCmd : AMC로 보내는 명령 구조체
@param   nError : 실패 시 반환하는 에러값
@return	 정상 처리되면 true 반환(Buffer에 아무 값이 없어 명령을 보내지 않았다면 이것도 정상처리이다)
*/
bool  AMCDllLibQueue::makeData(AMC_CMD* pAmcCmd, int *nError)
{
	bool bReturn = true;	
	int nTempError = 0;
	int nCmd = 0;
	int nTempCount = 0;
	QUEUEING_AMC_CMD *pQueuingAmcCmd = NULL;

	ENTER_CRITICAL_SECTION;
	nTempCount = m_LList.getCount();	// 현재까지의 Count 를 받아온다.
	LEAVE_CRITICAL_SECTION;

	for(int n=0; n<nTempCount; n++)
	{
		ENTER_CRITICAL_SECTION;
		pQueuingAmcCmd = (QUEUEING_AMC_CMD*)m_LList.popFront();
		LEAVE_CRITICAL_SECTION;

		nCmd = pQueuingAmcCmd->CmdAnalysisUnion.Cmd;
		pAmcCmd->Cmd = pAmcCmd->Cmd|nCmd;
		switch(nCmd)
		{
		case 0x01<<CMD_TYPE_SET_OUTBIT:
            for(int i=0; i<4; i++)
            {
                pAmcCmd->Output[i] = pQueuingAmcCmd->DataUnion.Output[i];
            }
//				memcpy(pAmcCmd->Output, pQueuingAmcCmd->DataUnion.Output, sizeof(int)*4);
			break;
		case 0x01<<CMD_TYPE_CLEAR_NODE_COUNT:
			break;
		case 0x01<<CMD_TYPE_MOVE_AXIS1:
			memcpy(&(pAmcCmd->CmdAxis[0]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;
		case 0x01<<CMD_TYPE_MOVE_AXIS2:
			memcpy(&(pAmcCmd->CmdAxis[1]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;
		case 0x01<<CMD_TYPE_MOVE_AXIS3:
			memcpy(&(pAmcCmd->CmdAxis[2]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;
		case 0x01<<CMD_TYPE_MOVE_AXIS4:
			memcpy(&(pAmcCmd->CmdAxis[3]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;
		case 0x01<<CMD_TYPE_SET_DETECT_INFO:
			memcpy(&(pAmcCmd->DetectInfo), &(pQueuingAmcCmd->DataUnion.DetectInfo), sizeof(DETECT_INFO));
#if(USE_CHANGENODE == 1)
			memcpy(&(pAmcCmd->OptNodeInfo), &(pQueuingAmcCmd->DataUnion.OptNodeInfo), sizeof(OPTIONAL_NODE_INFO)); // for change node (jhoun.lim)
#endif
			break;
//			case 0x01<<CMD_TYPE_SET_GEAR_RATIO_OF_DRIVING:
//				pAmcCmd->GearRatioOfDriving = pQueuingAmcCmd->DataUnion.GearRatioOfDriving;
//				break;
//			case 0x01<<AMC_CMD_TYPE_SET_DECEL_LIMIT_OF_DRIVING:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
//				break;

		case 0x01<<CMD_TYPE_SET_TORQUE_LIMIT_RESPONSE:
//			memcpy(&(pAmcCmd->CmdAxis[3]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;

		case 0x01<<CMD_TYPE_SET_TORQUE_LIMIT_REALEASE_RESPONSE:
//			memcpy(&(pAmcCmd->CmdAxis[3]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;

		case 0x01<<CMD_TYPE_SET_SLOPE_TIME:
//				pAmcCmd->GearRatioOfDriving = pQueuingAmcCmd->DataUnion.GearRatioOfDriving;
				break;
		case 0x01<<CMD_TYPE_SET_SLOPE_TIME_RELEASE:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;

		case 0x01<<CMD_TYPE_SET_TARGET_POS:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;
		case 0x01<<CMD_TYPE_SET_TARGET_POS_RELEASE:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;
		case 0x01<<CMD_TYPE_SET_FIRSTNODE_ADD:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;
		case 0x01<<CMD_TYPE_SET_VHLDETECTIGNOR:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;
		case 0x01<<CMD_TYPE_SET_VHLDETECTIGNOR_RELEASE:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;
		case 0x01<<CMD_TYPE_PAUSE_CONTROL:
			memcpy(&(pAmcCmd->PauseInfo), &(pQueuingAmcCmd->DataUnion.PauseInfo), sizeof(PAUSE_INFO));
			break;
		}
		delete pQueuingAmcCmd;
	}

	*nError = nTempError;
	return bReturn;
}

//---------------------------------------------------------------------------
