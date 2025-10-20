/**
@file	 VirtualModuleMonitor.h
@version OHT 7.0
@brief	 ExecuteThread가 돌리는 Module의 추상 함수
*/

#ifndef VirtualModuleMonitorH
#define VirtualModuleMonitorH

#include <stdio.h>
#include "Def_Variables.h"	//UINT
//---------------------------------------------------------------------------
typedef struct STATUS_SET_
{
	BYTE	byStatus;		///< 상태 변수
	UINT	uError;			///< 에러코드
	bool	bIsOnProcess; 	///< 동작중 여부 상태 변수
	UINT	uStep;			///< 순차적 명령 처리를 위한 Step
}STATUS_SET;
//---------------------------------------------------------------------------
class VirtualModuleMonitor
{
	public:
		VirtualModuleMonitor(void)	///< 생성자, 변수 초기화
		{
			memset(&m_StatusSet, 0, sizeof(STATUS_SET));
		}
		virtual ~VirtualModuleMonitor(void){;} ///< 가상 소멸자

	protected:
   		STATUS_SET	m_StatusSet;		///< 상태 정보 집합

	public:
		virtual UINT		GetError(void)			{return m_StatusSet.uError;}				///< 에러 상태를 받아 오는 함수
		virtual BYTE		GetStatus(void)			{return m_StatusSet.byStatus;}			///< 상태를 받아오는 함수
		virtual bool		IsOnProcess(void)		{return m_StatusSet.bIsOnProcess;}		///< 동작여부를 확인하는 함수 (동작중:true / 대기중:false)
		virtual UINT		GetStep(void)			{return m_StatusSet.uStep;}
		virtual void*		LoadParam(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory) = 0;	///< 파라미터를 백업하고, 새로운 정보를 Pointer로 받아오는 함수
		virtual bool		RequestParamUpdate(void* pParameter) = 0;									///< 받아온 Parameter 포인터 적용을 요청하는 함수
		virtual bool		IsParamUpdated() = 0;															///< 파라미터 적용 여부를 확인하는 함수
};
//---------------------------------------------------------------------------



#endif


