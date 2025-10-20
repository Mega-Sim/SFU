/**
@file	 VirtualModuleControl.h
@version OHT 7.0
@brief	 ExecuteThread가 돌리는 Module의 추상 함수
*/

#ifndef VirtualModuleControlH
#define VirtualModuleControlH

#include <stdio.h>
#include "VirtualModuleMonitor.h"
#include "ExecuteInfo.h"

//---------------------------------------------------------------------------
typedef unsigned int		UINT;	///< 4byte 자료형(unsigned int)
//---------------------------------------------------------------------------
class VirtualModuleControl: public VirtualModuleMonitor
{
   public:
		VirtualModuleControl(void)	///< 생성자, 변수 초기화
		{
			m_QuickCmd			= EXECUTE_QUICK_CMD_NONE;
		}
		virtual ~VirtualModuleControl(void){;} ///< 가상 소멸자

   protected:
		EXECUTE_QUICK_CMD	m_QuickCmd;		///< Quick Command Status

		virtual EXECUTE_ACK	Stop(void)		{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< 필요에 따라 오버 라이딩 하여 사용 
		virtual EXECUTE_ACK	Pause(void)		{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< 필요에 따라 오버 라이딩 하여 사용 
		virtual EXECUTE_ACK	Resume(void)	{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< 필요에 따라 오버 라이딩 하여 사용 
		virtual EXECUTE_ACK	Cancel(void)	{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< 필요에 따라 오버 라이딩 하여 사용 
		virtual EXECUTE_ACK	Reset(void)		{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< 필요에 따라 오버 라이딩 하여 사용
		virtual EXECUTE_ACK	TurnOff(void)	{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< 필요에 따라 오버 라이딩 하여 사용
        virtual EXECUTE_ACK	TurnON(void)	{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< 필요에 따라 오버 라이딩 하여 사용

   public:
		virtual EXECUTE_ACK	InsertGeneralCmd(void *pCmdInfo)			= 0;	///< 명령을 삽입하는 함수 (받은 명령을 반드시 delete 해야 한다 / 내부 상태 동작 중 상태로 변결 필수)
		virtual EXECUTE_ACK	SetQuickCmd(EXECUTE_QUICK_CMD QuickCmd);		///< 긴급 명령을 실행하는 함수: 정지/일시정지/재실행/초기화/명령 클리어 등
		virtual UINT		ExecuteJob(void)							= 0;	///< 할당된 작업을 실행 시키는 함수 (Step 형태로 구현 - Thread가 돌림)

		virtual UINT		CheckAbnormalOnWait(void)					= 0;	///< 대기 시 이상 현상 확인 함수 (Step 형태로 구현 - Thread가 돌림)
		virtual UINT		CheckAlways(void)					= 0;	///< 항시 이상 현상 확인 함수 (Step 형태로 구현 - Thread가 돌림)
 		virtual bool		DeleteCmd(void *pCmd)						= 0;	///< 할당 된 명령을 소멸함
};
//---------------------------------------------------------------------------

EXECUTE_ACK VirtualModuleControl::SetQuickCmd(EXECUTE_QUICK_CMD QuickCmd)	  ///< 긴급 명령을 실행하는 함수: 정지/일시정지/재실행/초기화/명령 클리어 등
{
	m_QuickCmd=QuickCmd;
	EXECUTE_ACK Ack = EXECUTE_ACK_NOT_AVAILABLE_CMD;
	switch(QuickCmd)
	{
	case EXECUTE_QUICK_CMD_NONE:		///< 초기화 상태		A/M
		break;
	case EXECUTE_QUICK_CMD_STOP:		///< 긴급 정지		A/M	  ~ OCS_Estop / Status Check 오는 Internal Cmd 가 E-stop /
		Ack = Stop();
		break;
	case EXECUTE_QUICK_CMD_CANCEL:		///< 주행 명령 Clear	A/M	   ~ OCS 에 경로 변경할 경우
		Ack = Cancel();
		break;
	case EXECUTE_QUICK_CMD_RESET:		///< 주행 명령 Clear	A/M	  ~ OCS 에 경로 변경할 경우
		m_StatusSet.uStep	= 0;
		Ack = Reset();
		break;
	case EXECUTE_QUICK_CMD_TURN_OFF:
        Ack = TurnOff();
		break;
	case EXECUTE_QUICK_CMD_TURN_ON:
        Ack = TurnON();
		break;
	}
	return Ack;
}



#endif


