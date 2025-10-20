/**
@file    CmdManager.cpp
@version OHT 7.0
@brief   CmdManager Class를 구현한 CPP File
*/
//---------------------------------------------------------------------------
#pragma hdrstop

#include "CmdManager.h"
//#include "OHT_TLV.h"
//---------------------------------------------------------------------------
#include "OHTMain.h"
#define ADD_MEMO_T(STR)   OHTMainForm->memoMain->Lines->Add(_T(STR))
#define ADD_MEMO_S(STR)   OHTMainForm->memoMain->Lines->Add(STR)

//---------------------------------------------------------------------------
#define MAX_BYTE_SIZE_OF_TASKCONTROL_LOG    1024*10

// 로그 작성 함수
#define ADD_LOG(...)	        WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)

#define ERR_LOG(...)            WRITE_LOG_ON_THIS_FILE("D:\\log\\Error.log", __VA_ARGS__);\
								ADD_LOG(__VA_ARGS__)
//---------------------------------------------------------------------------
/**
@brief   CmdManager 클래스 생성자
@author  zzang9un
@date    2013.04.09
@param   pCmdList : 관리해야 하는 List 클래스 포인터
@param   pOHTMainStatus : Quick Command를 위한 공유 변수
*/
CmdManager::CmdManager()
{
    // by zzang9un 2013.08.16 : Log 변수
    pLogUnit = getLogUnit("D:\\log\\CmdManager.log", MAX_BYTE_SIZE_OF_TASKCONTROL_LOG);

//	m_CmdList = OHTMainForm->GetCmdList();
	m_TaskList = OHTMainForm->GetTaskList();
    m_CycleList = OHTMainForm->GetCycleList();
	m_ResponseList = OHTMainForm->GetResponseList();
//	m_pPassOpenList = OHTMainForm->GetPassOpenList();
//	m_pPassPermitList = OHTMainForm->GetPassPermitList();        ///puting 리스트관리를 여기서하는 방안 생각해보기.

	m_MainStatus = OHTMainForm->GetOHTStatus();
    m_pExecuteInfo = OHTMainForm->GetExecuteInfo();

//	InitPreOCSCmdBuffer();
//	InitCmdList();
	InitTaskList();
	InitOCSResponseList();
	InitCycleList();
//	InitPassOpenList();
//	InitPassPermitList();
}
//---------------------------------------------------------------------------
/**
@brief   CmdManager 클래스 소멸자
@author  zzang9un
@date    2012.10.30
*/
CmdManager::~CmdManager()
{
//	InitPreOCSCmdBuffer();
//	InitCmdList();
	InitTaskList();
	InitOCSResponseList();
	InitCycleList();
//	InitPassOpenList();
//	InitPassPermitList();

}
//---------------------------------------------------------------------------

/**
@brief   List를 초기화하는 함수
@author  zzang9un
@date    2012.11.16
@note    List는 CMD_DATA 구조체의 포인터로 구성되어 있음.
*/
void CmdManager::InitTaskList()
{
	// List를 꺼내 해당 포인터 delete 후
	int nCount = m_TaskList->getCount();
	for(int i = 0; i < nCount; i++)
    {
		CS_TaskList->Acquire();
        try
        {
			delete (PKT_TASKCMDDATA *)m_TaskList->popFront();
        }
        __finally
        {
			CS_TaskList->Release();
        }
    }

    // 리스트를 Clear한다.
	CS_TaskList->Acquire();
    try
    {
		m_TaskList->clear();
    }
    __finally
    {
		CS_TaskList->Release();
    }

}
//---------------------------------------------------------------------------

/**
@brief   List의 첫번째 Command를 delete하는 함수
@author  zzang9un
@date    2012.11.16
@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우(count가 0인 경우) 0(false)를 리턴

int CmdManager::DeleteFirstTask()
{
	if(m_TaskList->getCount() == 0)
        return FALSE;

	CS_TaskList->Acquire();
    try
    {
		delete (PKT_TASKCMDDATA *)m_TaskList->popFront();
    }
	__finally
    {
		CS_TaskList->Release();
    }

    return TRUE;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List의 Index를 이용하여 특정 Command를 delete하는 함수
@author  zzang9un
@date    2012.11.16
@param   Index : delete하려는 List Index
@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우 0(false)를 리턴
*/
int CmdManager::DeleteTask(UINT Index)
{
	if(m_TaskList->getCount() == 0)
        return FALSE;

	if(Index > m_TaskList->getCount() - 1)
    {
        // Index가 List의 개수보다 클 경우
        return FALSE;
    }

	CS_TaskList->Acquire();
    try
    {
		delete (PKT_TASKCMDDATA *)m_TaskList->pop(Index);
    }
    __finally
    {
		CS_TaskList->Release();
    }

    return TRUE;
}
//---------------------------------------------------------------------------

/**
@brief   List에 Command를 추가하는 함수
@author  zzang9un
@date    2012.11.16
@param   pData : Add하려는 void 포인터
*/
void CmdManager::AddTask(void *pData)
{
	CS_TaskList->Acquire();
    try
    {
		m_TaskList->insertBack(pData);
    }
    __finally
    {
		CS_TaskList->Release();
    }
}
//---------------------------------------------------------------------------
/**
@brief   List에 Command를 추가하는 함수
@author  zzang9un
@date    2012.11.16
@param   pData : Add하려는 void 포인터
*/
void CmdManager::AddTask(void *pData, int Index)
{
	CS_TaskList->Acquire();
    try
	{
		m_TaskList->insert(pData,Index);
    }
    __finally
    {
		CS_TaskList->Release();
    }
}
//---------------------------------------------------------------------------


/**
@brief   List의 첫번째 Item을 참조하는 포인트를 리턴하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.
*/
void* CmdManager::ReferFirstTask()
{
    void *pListItem = NULL;

	pListItem = m_TaskList->referFront();

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List의 Index Item을 참조하는 포인트를 리턴하는 함수
@author  puting
@date    2013.10.28
@return  List의 index Item의 포인터를 리턴한다.
*/
void* CmdManager::ReferTask(UINT index)
{
    void *pListItem = NULL;

	pListItem = m_TaskList->refer(index);

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List의 첫번째 Item을 Pop하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.
@note    Pop한 Item은 리스트에서 삭제된다.
*/
void* CmdManager::PopFirstTask()
{
    void *pListItem = NULL;

	CS_TaskList->Acquire();
    try
    {
		pListItem = m_TaskList->popFront();
    }
    __finally
    {
		CS_TaskList->Release();
    }

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List의 count를 리턴하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 count를 리턴한다.
*/
int CmdManager::GetTaskCount()
{
	return m_TaskList->getCount();
}
//---------------------------------------------------------------------------
/**
@brief   List를 초기화하는 함수
@author  puting
@date    2012.11.16
@note    List는 RESPONSE_DATA 구조체의 포인터로 구성되어 있음.
*/
void CmdManager::InitOCSResponseList()
{
	// List를 꺼내 해당 포인터 delete 후
	int nCount = m_ResponseList->getCount();
    for(int i = 0; i < nCount; i++)
    {
		CS_ResponceList->Acquire();
        try
        {
			delete (PKT_ACK_OHT2OCS *)m_ResponseList->popFront();
        }
        __finally
        {
			CS_ResponceList->Release();
		}
    }

    // 리스트를 Clear한다.
	CS_ResponceList->Acquire();
    try
    {
        m_ResponseList->clear();
    }
    __finally
    {
		CS_ResponceList->Release();
    }

}
//---------------------------------------------------------------------------

/**
@brief   List의 첫번째 Command를 delete하는 함수
@author  zzang9un
@date    2012.11.16
@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우(count가 0인 경우) 0(false)를 리턴

int CmdManager::DeleteFirstOCSResponse()
{
    if(m_ResponseList->getCount() == 0)
        return FALSE;

	CS_ResponceList->Acquire();
    try
    {
		delete (PKT_ACK_OHT2OCS *)m_ResponseList->popFront();
    }
    __finally
    {
		CS_ResponceList->Release();
    }

    return TRUE;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List의 Index를 이용하여 특정 Command를 delete하는 함수
@author  zzang9un
@date    2012.11.16
@param   Index : delete하려는 List Index
@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우 0(false)를 리턴

int CmdManager::DeleteOCSResponse(UINT Index)
{
    if(m_ResponseList->getCount() == 0)
        return FALSE;

    if(Index > m_ResponseList->getCount() - 1)
    {
        // Index가 List의 개수보다 클 경우
        return FALSE;
    }

	CS_ResponceList->Acquire();
    try
    {
		delete (PKT_ACK_OHT2OCS *)m_ResponseList->pop(Index);
    }
    __finally
    {
		CS_ResponceList->Release();
    }

    return TRUE;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List에 Command를 추가하는 함수
@author  zzang9un
@date    2012.11.16
@param   pData : Add하려는 void 포인터
*/
void CmdManager::AddOCSResponse(void *pData)
{
	CS_ResponceList->Acquire();
    try
    {
        m_ResponseList->insertBack(pData);
    }
    __finally
    {
		CS_ResponceList->Release();
    }
}
//---------------------------------------------------------------------------

/**
@brief   List의 첫번째 Item을 참조하는 포인트를 리턴하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.

void* CmdManager::ReferFirstOCSResponse()
{
    void *pListItem = NULL;

    pListItem = m_ResponseList->referFront();

    return pListItem;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List의 첫번째 Item을 Pop하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.
@note    Pop한 Item은 리스트에서 삭제된다.
*/
void* CmdManager::PopFirstOCSResponse()
{
    void *pListItem = NULL;

	CS_ResponceList->Acquire();
    try
    {
        pListItem = m_ResponseList->popFront();
    }
    __finally
    {
		CS_ResponceList->Release();
    }

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List의 count를 리턴하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 count를 리턴한다.
*/
int CmdManager::GetOCSResponseCount()
{
    return m_ResponseList->getCount();
}
//---------------------------------------------------------------------------
/**
@brief   List를 초기화하는 함수
@author  puting
@date    2012.11.16
@note    List는 RESPONSE_DATA 구조체의 포인터로 구성되어 있음.
*/
void CmdManager::InitCycleList()
{
	// List를 꺼내 해당 포인터 delete 후
	int nCount = m_CycleList->getCount();
	for(int i = 0; i < nCount; i++)
    {
		CS_CycleList->Acquire();
        try
        {
            delete (PKT_CYCLECMDDATA *)m_CycleList->popFront();
        }
        __finally
        {
			CS_CycleList->Release();
        }
    }

    // 리스트를 Clear한다.
	CS_CycleList->Acquire();
    try
    {
        m_CycleList->clear();
    }
    __finally
    {
		CS_CycleList->Release();
    }

}
//---------------------------------------------------------------------------


  /**
@brief   List의 마지막  Command를 delete하는 함수
@author  zzang9un
@date    2012.11.16
@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우(count가 0인 경우) 0(false)를 리턴
*/
int CmdManager::DeleteBackCycle()
{
	if(m_CycleList->getCount() == 0)
		return FALSE;

	CS_CycleList->Acquire();
	try
	{
		delete (PKT_CYCLECMDDATA *)m_CycleList->popBack();
	}
	__finally
	{
		CS_CycleList->Release();
	}

	return TRUE;
}
//-----------

/**
@brief   List의 첫번째 Command를 delete하는 함수
@author  zzang9un
@date    2012.11.16
@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우(count가 0인 경우) 0(false)를 리턴

int CmdManager::DeleteFirstCycle()
{
	if(m_CycleList->getCount() == 0)
		return FALSE;

	CS_CycleList->Acquire();
	try
	{
		delete (PKT_CYCLECMDDATA *)m_CycleList->popFront();
	}
	__finally
	{
		CS_CycleList->Release();
	}

	return TRUE;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List의 Index를 이용하여 특정 Command를 delete하는 함수
@author  zzang9un
@date    2012.11.16
@param   Index : delete하려는 List Index
@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우 0(false)를 리턴

int CmdManager::DeleteCycle(UINT Index)
{
    if(m_CycleList->getCount() == 0)
        return FALSE;

    if(Index > m_CycleList->getCount() - 1)
    {
        // Index가 List의 개수보다 클 경우
        return FALSE;
    }

	CS_CycleList->Acquire();
    try
    {
		delete (PKT_CYCLECMDDATA *)m_CycleList->pop(Index);
    }
    __finally
    {
		CS_CycleList->Release();
    }

    return TRUE;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List에 Command를 추가하는 함수
@author  zzang9un
@date    2012.11.16
@param   pData : Add하려는 void 포인터
*/
void CmdManager::AddCycle(void *pData)
{
	CS_CycleList->Acquire();
    try
    {
        m_CycleList->insertBack(pData);
    }
    __finally
    {
		CS_CycleList->Release();
    }
}
//---------------------------------------------------------------------------

/**
@brief   List의 Index Item을 참조하는 포인트를 리턴하는 함수
@author  puting
@date    2013.10.28
@return  List의 index Item의 포인터를 리턴한다.
*/
void* CmdManager::ReferCycle(UINT index)
{
    void *pListItem = NULL;

    pListItem = m_CycleList->refer(index);

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List의 첫번째 Item을 참조하는 포인트를 리턴하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.

void* CmdManager::ReferFirstCycle()
{
    void *pListItem = NULL;

    pListItem = m_CycleList->referFront();

    return pListItem;
}
*/
//---------------------------------------------------------------------------
/**
@brief   List의 마지막 Item을 Pop하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.
@note    Pop한 Item은 리스트에서 삭제된다.
*/
void* CmdManager::PopBackCycle()
{
	void *pListItem = NULL;


	pListItem = m_CycleList->popBack();


	return pListItem;
}


/**
@brief   List의 첫번째 Item을 Pop하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.
@note    Pop한 Item은 리스트에서 삭제된다.

void* CmdManager::PopFirstCycle()
{
    void *pListItem = NULL;

	CS_CycleList->Acquire();
    try
    {
		pListItem = m_CycleList->popFront();
    }
    __finally
    {
        CS_CycleList->Release();
    }

    return pListItem;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List의 count를 리턴하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 count를 리턴한다.
*/
int CmdManager::GetCycleCount()
{
    return m_CycleList->getCount();
}
////---------------------------------------------------------------------------
///**
//@brief   List를 초기화하는 함수
//@author  puting
//@date    2016.03.15
//@note    List는 RESPONSE_DATA 구조체의 포인터로 구성되어 있음.
//*/
//void CmdManager::InitPassOpenList()
//{
//    // List를 꺼내 해당 포인터 delete 후
//	for(int i = 0; i < m_pPassOpenList->getCount(); i++)
//    {
//		CS_PassOpenList->Acquire();
//        try
//        {
//			delete (DATA_PASSOPEN *)m_pPassOpenList->popFront();
//        }
//        __finally
//        {
//			CS_PassOpenList->Release();
//        }
//    }
//
//    // 리스트를 Clear한다.
//	CS_PassOpenList->Acquire();
//    try
//    {
//		m_pPassOpenList->clear();
//    }
//    __finally
//    {
//		CS_PassOpenList->Release();
//    }
//
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List의 첫번째 Command를 delete하는 함수
//@author  zzang9un
//@date    2012.11.16
//@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우(count가 0인 경우) 0(false)를 리턴
//*/
//int CmdManager::DeleteFirstPassOpen()
//{
//	if(m_pPassOpenList->getCount() == 0)
//        return FALSE;
//
//	CS_PassOpenList->Acquire();
//    try
//    {
//		delete (DATA_PASSOPEN *)m_pPassOpenList->popFront();
//    }
//    __finally
//    {
//		CS_PassOpenList->Release();
//    }
//
//    return TRUE;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List의 Index를 이용하여 특정 Command를 delete하는 함수
//@author  zzang9un
//@date    2012.11.16
//@param   Index : delete하려는 List Index
//@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우 0(false)를 리턴
//*/
//int CmdManager::DeletePassOpen(UINT Index)
//{
//	if(m_pPassOpenList->getCount() == 0)
//        return FALSE;
//
//	if(Index > m_pPassOpenList->getCount() - 1)
//    {
//        // Index가 List의 개수보다 클 경우
//        return FALSE;
//    }
//
//	CS_PassOpenList->Acquire();
//    try
//    {
//		delete (DATA_PASSOPEN *)m_pPassOpenList->pop(Index);
//    }
//    __finally
//    {
//		CS_PassOpenList->Release();
//    }
//
//    return TRUE;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List에 Command를 추가하는 함수
//@author  zzang9un
//@date    2012.11.16
//@param   pData : Add하려는 void 포인터
//*/
//void CmdManager::AddPassOpen(void *pData)
//{
//	CS_PassOpenList->Acquire();
//    try
//    {
//		m_pPassOpenList->insertBack(pData);
//    }
//    __finally
//    {
//		CS_PassOpenList->Release();
//    }
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List의 Index Item을 참조하는 포인트를 리턴하는 함수
//@author  puting
//@date    2013.10.28
//@return  List의 index Item의 포인터를 리턴한다.
//*/
//void* CmdManager::ReferPassOpen(UINT index)
//{
//    void *pListItem = NULL;
//
//	pListItem = m_pPassOpenList->refer(index);
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List의 첫번째 Item을 참조하는 포인트를 리턴하는 함수
//@author  zzang9un
//@date    2013.04.10
//@return  List의 첫번째 Item의 포인터를 리턴한다.
//*/
//void* CmdManager::ReferFirstPassOpen()
//{
//    void *pListItem = NULL;
//
//	pListItem = m_pPassOpenList->referFront();
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
//
//
//
///**
//@brief   List의 첫번째 Item을 Pop하는 함수
//@author  zzang9un
//@date    2013.04.10
//@return  List의 첫번째 Item의 포인터를 리턴한다.
//@note    Pop한 Item은 리스트에서 삭제된다.
//*/
//void* CmdManager::PopFirstPassOpen()
//{
//    void *pListItem = NULL;
//
//	CS_PassOpenList->Acquire();
//    try
//    {
//		pListItem = m_pPassOpenList->popFront();
//    }
//    __finally
//    {
//        CS_PassOpenList->Release();
//    }
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List의 count를 리턴하는 함수
//@author  zzang9un
//@date    2013.04.10
//@return  List의 count를 리턴한다.
//*/
//int CmdManager::GetPassOpenCount()
//{
//    return m_pPassOpenList->getCount();
//}
////---------------------------------------------------------------------------
///**
//@brief   List를 초기화하는 함수
//@author  puting
//@date    2016.03.15
//@note    List는 RESPONSE_DATA 구조체의 포인터로 구성되어 있음.
//*/
//void CmdManager::InitPassPermitList()
//{
//    // List를 꺼내 해당 포인터 delete 후
//	for(int i = 0; i < m_pPassPermitList->getCount(); i++)
//    {
//		delete (PASSPERMIT_COMMON *)m_pPassPermitList->popFront();
//
//	}
//
//	// 리스트를 Clear한다.
//
//   	m_pPassPermitList->clear();
//
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List의 첫번째 Command를 delete하는 함수
//@author  zzang9un
//@date    2012.11.16
//@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우(count가 0인 경우) 0(false)를 리턴
//*/
//int CmdManager::DeleteFirstPassPermit()
//{
//	if(m_pPassPermitList->getCount() == 0)
//        return FALSE;
//
//	delete (PASSPERMIT_COMMON *)m_pPassPermitList->popFront();
//
//
//    return TRUE;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List의 Index를 이용하여 특정 Command를 delete하는 함수
//@author  zzang9un
//@date    2012.11.16
//@param   Index : delete하려는 List Index
//@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우 0(false)를 리턴
//*/
//int CmdManager::DeletePassPermit(UINT Index)
//{
//	if(m_pPassPermitList->getCount() == 0)
//        return FALSE;
//
//	if(Index > m_pPassPermitList->getCount() - 1)
//    {
//        // Index가 List의 개수보다 클 경우
//        return FALSE;
//    }
//
//	delete (PASSPERMIT_COMMON *)m_pPassPermitList->pop(Index);
//
//
//	return TRUE;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List에 Command를 추가하는 함수
//@author  zzang9un
//@date    2012.11.16
//@param   pData : Add하려는 void 포인터
//*/
//void CmdManager::AddPassPermit(void *pData)
//{
//
//	m_pPassPermitList->insertBack(pData);
//
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List의 Index Item을 참조하는 포인트를 리턴하는 함수
//@author  puting
//@date    2013.10.28
//@return  List의 index Item의 포인터를 리턴한다.
//*/
//void* CmdManager::ReferPassPermit(UINT index)
//{
//    void *pListItem = NULL;
//
//	pListItem = m_pPassPermitList->refer(index);
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List의 첫번째 Item을 참조하는 포인트를 리턴하는 함수
//@author  zzang9un
//@date    2013.04.10
//@return  List의 첫번째 Item의 포인터를 리턴한다.
//*/
//void* CmdManager::ReferFirstPassPermit()
//{
//    void *pListItem = NULL;
//
//	pListItem = m_pPassPermitList->referFront();
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
///**
//@brief   List의 마지막 Item을 참조하는 포인트를 리턴하는 함수
//@author  zzang9un
//@date    2013.04.10
//@return  List의 첫번째 Item의 포인터를 리턴한다.
//*/
//void* CmdManager::ReferBackPassPermit()
//{
//	void *pListItem = NULL;
//
//	pListItem = m_pPassPermitList->referBack();
//
//    return pListItem;
//}
//
//
///**
//@brief   List의 첫번째 Item을 Pop하는 함수
//@author  zzang9un
//@date    2013.04.10
//@return  List의 첫번째 Item의 포인터를 리턴한다.
//@note    Pop한 Item은 리스트에서 삭제된다.
//*/
//void* CmdManager::PopFirstPassPermit()
//{
//    void *pListItem = NULL;
//
//
//	pListItem = m_pPassPermitList->popFront();
//
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
///**
//@brief   List의 마지막 Item을 Pop하는 함수
//@author  zzang9un
//@date    2013.04.10
//@return  List의 첫번째 Item의 포인터를 리턴한다.
//@note    Pop한 Item은 리스트에서 삭제된다.
//*/
//void* CmdManager::PopBackPassPermit()
//{
//	void *pListItem = NULL;
//
//
//	pListItem = m_pPassPermitList->popBack();
//
//
//	return pListItem;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List의 count를 리턴하는 함수
//@author  zzang9un
//@date    2013.04.10
//@return  List의 count를 리턴한다.
//*/
//int CmdManager::GetPassPermitCount()
//{
//    return m_pPassPermitList->getCount();
//}
////---------------------------------------------------------------------------
/**
@brief   작업변경시 기존 대기싸이클 작업지시에 대해 삭제해주는 함수
@author  puting
@date    2018.03.13
*/
void CmdManager::MakeTaskList_Standard_Del_JobOrder()
{
	bool bAddTask =false;
	int nTaskCount = GetTaskCount();
	PKT_TASKCMDDATA* TempCmdTaskData;        ///< 비교하기위해 사용되는 임시 구조체

	if(nTaskCount !=0)
	{
		//puting 탐색 순서 확인 필요 2018 03 13
		for(int Index =0; Index < nTaskCount; Index++)
		{
			TempCmdTaskData = (PKT_TASKCMDDATA*) ReferTask(Index);
			if(TempCmdTaskData->DetailTagSort == TAGTYPE_JOBORDER)
				{
					DeleteTask(Index);
            		ADD_LOG("Insert_Index:%d", Index);
					break;
				}
		}
	}


}
/**
@brief   정합성 확인이 완료된 CMD에 대해서 Task(Cmd) List에 Add하는 함수
@author  puting
@date    2015.10.30
@param   pCmdHeader : Command의 Header
@param   pCmdGoEtc : Command 구조체
*/
void CmdManager::MakeTaskList_Standard(PKT_TASKCMDDATA *pCmdPktData)
{
	bool bAddTask =false;
	int nTaskCount = GetTaskCount();
	PKT_TASKCMDDATA* CmdTaskData = new PKT_TASKCMDDATA;
	PKT_TASKCMDDATA* TempCmdTaskData;        ///< 비교하기위해 사용되는 임시 구조체

	memmove(CmdTaskData, pCmdPktData, sizeof(PKT_TASKCMDDATA));

//    // 초기화
//    memset(pCmdTaskData, 0x00, sizeof(PKT_TASKCMDDATA));

	if(nTaskCount !=0)
		{
			//puting 탐색 순서 확인 필요 2015 10 31
			for(int Index =0; Index < nTaskCount; Index++)
			{
				TempCmdTaskData = (PKT_TASKCMDDATA*) ReferTask(Index);
				if(TempCmdTaskData->DetailTagSort > CmdTaskData->DetailTagSort)
					{
						AddTask(CmdTaskData, Index);
						bAddTask = true;
						ADD_LOG("Insert_Index:%d", Index);
						break;
					}
			}
		}

	if(!bAddTask)	AddTask(CmdTaskData);

    if(nTaskCount >1)
		ADD_LOG("MakeTaskList_Standard  Count %d CmdDiv:%d, Data1:%04X" , (nTaskCount+1),CmdTaskData->CategoryDiv, CmdTaskData->DetailTagSort);

}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
@brief   Main Thread로 Quick Command를 보내는 함수
@author  zzang9un
@date    2013.04.09
@param   QuickCmd : 전송할 Quick Command
*/
void CmdManager::SendQuickCmd2Main(QUICK_CMD QuickCmd)
{
    CS_OHTMainStatus->Acquire();
    try
    {
		m_MainStatus->QuickCmd = QuickCmd;
    }
    __finally
    {
        CS_OHTMainStatus->Release();
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
@brief   HandTP 명령이 실행 가능한지를 체크하는 함수
@date    2013.01.22
@param   nTmpPID : Port ID
@return
@note    CmdManager 특성에 맞지 않는 함수이지만, 방법이 없이 임시로 작성
*/
int CmdManager::Is_Executable(void)
{
    int nError = 0;

    if(m_MainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO) 
        nError = 1;
    else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_LOADING) 
        nError=2;
    else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_UNLOADING) 
		nError=3;
	else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_AUTO_TEACHING)
		nError=3;
	else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_AUTO_TEACHING_NULLTYPE)
		nError=3;
   	else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_AUTO_TRANS_HOMING)
		nError=3;
    else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_GOING) 
        nError=4;
    else if(m_MainStatus->StatusTP.Driving_F.IsRunning == ON) 
        nError=5;
    else if(m_MainStatus->StatusTP.Driving_R.IsRunning == ON) 
        nError=6;
    else if(m_MainStatus->StatusTP.Hoist.IsRunning == ON)
        nError=7;
    else if(m_MainStatus->StatusTP.Shift.IsRunning == ON)
        nError=8;
    // 2016-12-25, Add by Labeler, Rotate 구동 중 상태 Check 추가
    else if(m_MainStatus->StatusTP.Rotate.IsRunning == ON)
        nError=9;

    return nError;
}
//---------------------------------------------------------------------------

/**
@brief   HandTP 명령이 실행 가능한지를 체크하는 함수
@date    2013.01.22
@param   nTmpPID : Port ID
@return
@note    CmdManager 특성에 맞지 않는 함수이지만, 방법이 없이 임시로 작성
*/
int CmdManager::Is_Executable(char* ExMessage, ...)
{
   int nError = NO_ERR;

   char szDebugMsg[256] = "";
   int ncnt = 0, i = 0;
   String strExclude = "";

   va_list va;
   va_start(va, ExMessage);
   ncnt=vsprintf(szDebugMsg, ExMessage, (va_list)va);
   va_end(va);

   if(m_MainStatus->StatusCommon.MSCmdID.Info.Mode==MODE_AUTO)
   {
      nError = 1;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_AUTO)
         {
            nError =NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_LOADING))
   {
      nError = 2;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_LOAD)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_UNLOADING))
   {
      nError = 3;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_UNLOAD)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_AUTO_TEACHING))
   {
	  nError = 3;

	  for(i=0; i<ncnt; i++)
	  {
		 strExclude=szDebugMsg[i];

		 if(strExclude==STS_AUTO_TAUGHT)
		 {
			nError = NO_ERR;
			break;
		 }
	  }
   }

   if((nError==0)&&(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_AUTO_TEACHING_NULLTYPE))
   {
	  nError = 3;

	  for(i=0; i<ncnt; i++)
	  {
		 strExclude=szDebugMsg[i];

		 if(strExclude==STS_AUTO_TAUGHT)
		 {
			nError = NO_ERR;
			break;
		 }
	  }
   }

   if((nError==0)&&(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_GOING))
   {
      nError = 4;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_GOING)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&((m_MainStatus->StatusTP.Driving_F.IsRunning == ON)||(m_MainStatus->StatusTP.Driving_R.IsRunning == ON)))
   {
      nError = 5;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_DRIVING_RUNNING)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusTP.Hoist.IsRunning == ON))
   {
      nError = 7;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_HOIST_RUNNING)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusTP.Shift.IsRunning == ON))
   {
      nError = 8;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_SHIFT_RUNNING)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusTP.Rotate.IsRunning == ON))
   {
      nError = 99;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_ROTATE_RUNNING)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   return nError;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
