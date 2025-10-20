//---------------------------------------------------------------------------
/**
@file    PassPermit.cpp
@version OHT 7.0
@brief   PassPermit Class CPP File
*/
#include <windows.h>
#include <stdlib.h>
#include <math.h>


#include "PassPermit.h"
#include "ErrorCode.h"
#include "OHTMain.h"

#include "LogHandler.h"	// Log 작성 목적

//#define PASSPERMIT_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\PASSPERMIT.log", __VA_ARGS__)
//수정
#define PASSPERMIT_LOG(...)    WRITE_LOG_UNIT(pLogPasspermit, __VA_ARGS__)

//---------------------------------------------------------------------------

PassPermit::PassPermit()
{
	pLogPasspermit = getLogUnit("D:\\log\\PASSPERMIT.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
 	m_pPassOpenList = OHTMainForm->GetPassOpenList();
	m_pPassPermitList = OHTMainForm->GetPassPermitList();

	InitPassPermit();

	InitPassOpenList();
	InitPassPermitList();

}

/**
@brief   CID 소멸자
@author	 tg117
@date    2015.12.01
*/
PassPermit::~PassPermit()
{

	InitPassOpenList();
	InitPassPermitList();

}

/**
@brief   CID 입출력 및 각종 변수 초기화
@author	 tg117
@date    2015.12.01
@return	 정상인 경우 0, 비정상인 경우 ErrorCode Return
*/
int PassPermit::InitPassPermit()
{

	m_PPIndex = 0;
	m_PPOIndex = 0;
	m_PPMaxIndex = 0;
	m_SumResetPosition = 0.0;
	m_SumCyclePosition = 0.0;
	m_PrePasOpenSeq = INDEX_NONE;

	m_PPCount = 0;

	m_bCheckRollbackFinish = false;

	m_dwPassPermitTime =  timeGetTime();
	m_dwPassPeriodTime = PASSTIME_NONE;

	m_bISForcePPO = false;


}

DWORD PassPermit::GetPassPermitTime()
{
	return m_dwPassPermitTime;
}
DWORD PassPermit::GetPassPeriodTime()
{
	return m_dwPassPeriodTime;
}

void PassPermit::SetPassPermitTime(DWORD dwTime)
{
   	m_dwPassPermitTime = dwTime;
}
void PassPermit::SetPassPeriodTime(DWORD dwTime)
{
   	m_dwPassPeriodTime =  dwTime;
}

int PassPermit::SetPPIndex(int nIndex)
{
    m_PPIndex = nIndex;
	return m_PPIndex;
}
int PassPermit::SetPPOIndex(int nIndex)
{
	m_PPOIndex = nIndex;
	return m_PPOIndex;
}
//
/**
 @class   GetPPIndex 함수.
 @date    2015.10.15
 @author  puting
 @brief
 */
int PassPermit::GetPPIndex()
{
	if (m_PPIndex < 0)
		return -1;

	return m_PPIndex;
//
}
/**
 @class   GetPPOIndex 함수.
 @date    2015.10.15
 @author  puting
 @brief
 */
int PassPermit::GetPPOIndex()
{
	if (m_PPOIndex < 0)
		return -1;

	return m_PPOIndex;
}
/**
 @class   GetPPMaxIndex 함수.
 @date    2015.10.15
 @author  puting
 @brief

int PassPermit::GetPPMaxIndex()
{
	if (m_PPMaxIndex < 0)
		return -1;

	return m_PPMaxIndex;
}
 */
/**
 @class   GetPPMaxIndex 함수.
 @date    2015.10.15
 @author  puting
 @brief
 */
bool PassPermit::GetRollBackFinish()
{
	return m_bCheckRollbackFinish;
}

/**
 @class   SetIncreasePPIndex 함수.
 @date    2015.10.15
 @author  puting
 @brief
 */
int PassPermit::SetIncreasePPIndex(int nMax) //-1 일 경우 최대값 위치임
{
	if(nMax < (m_PPIndex +1))
		return -1;

   m_PPIndex = m_PPIndex +1;

   if(m_PPMaxIndex < m_PPIndex)
		m_PPMaxIndex = m_PPIndex;

   PASSPERMIT_LOG("SetIncreasePPIndex PPIndex:%d, MaxIndex:%d",m_PPIndex, m_PPMaxIndex);
   return m_PPIndex;


}
/**
 @class   SetIncreasePPOIndex 함수.
 @date    2015.10.15
 @author  puting
 @brief   포지션 얻어옴
 */
int PassPermit::SetIncreasePPOIndex(int nMax) //-1 일 경우 최대값 위치임
{
	if(nMax < (m_PPOIndex +1))
		return -1;

   m_PPOIndex = m_PPOIndex +1;

//   m_PPMaxIndex = m_PPIndex;
   PASSPERMIT_LOG("SetIncreasePPOIndex %d",m_PPOIndex);
   return m_PPOIndex;


}

/**
 @class   SetDecreasePPOIndex 함수.
 @date    2018.03.10
 @author  puting
 @brief   포지션 얻어옴
 */
int PassPermit::SetDecreasePPOIndex()
{
	PASSPERMIT_LOG("SetDecreasePPOIndex PP:%d PPO:%d",m_PPIndex, m_PPOIndex);
	if(m_PPOIndex < 1)
		return -1;

   m_PPOIndex = m_PPOIndex -1;

   return m_PPOIndex;

}
/**
 @class   SetDecreasePPIndex 함수.
 @date    2015.10.15
 @author  puting
 @brief   포지션 얻어옴
 */
int PassPermit::SetDecreasePPIndex() //-1 일 경우 최소값 위치임.
{
	if(m_PPOIndex > (m_PPIndex -1))
		return -1;

	if(m_PPIndex < 1)
		return -1;

   m_PPIndex = m_PPIndex -1;

   PASSPERMIT_LOG("SetDecreasePPIndex PP:%d PPO:%d",m_PPIndex, m_PPOIndex);
   return m_PPIndex;

}

/**
 @class   SetDecreasePPIndex 함수.
 @date    2015.10.15
 @author  puting
 @brief   포지션 얻어옴
 */
void PassPermit::SetRollBackFinish(bool bCheck) //-1 일 경우 최소값 위치임.
{

   if(m_bCheckRollbackFinish != bCheck)
	   PASSPERMIT_LOG("SetRollBackFinish %d",m_bCheckRollbackFinish);

   m_bCheckRollbackFinish = bCheck;

}

/**
 @class   SetSumResetPositon 함수.
 @date    2015.10.15
 @author  puting
 @brief   포지션 얻어옴
 */
void PassPermit::SetSumResetPositon(double dResetPostion)
{
	m_SumResetPosition =  (double)m_SumResetPosition  +  (double)dResetPostion;
//	m_SumResetPosition =  (double)dResetPostion;
}
/**
 @class   GetAbsolutePositon 함수.
 @date    2015.10.15
 @author  puting
 @brief   포지션 얻어옴
 */
double PassPermit::GetAbsolutePositon()
{
	return (double)m_SumResetPosition;

}
/**
 @class   SetSumCyclePositon 함수.
 @date    2015.10.15
 @author  puting
 @brief   포지션 얻어옴

void PassPermit::SetSumCyclePositon(double dCyclePostion)    //Map상의 End시점.
{
//	m_SumCyclePosition =  (double)m_SumCyclePosition  +  (double)dCyclePostion;
	m_SumCyclePosition =  (double)dCyclePostion;
}
 */
/**
 @class   GetSumCyclePositon 함수.
 @date    2015.10.15
 @author  puting
 @brief   포지션 얻어옴

double PassPermit::GetSumCyclePositon()
{
	return (double)m_SumCyclePosition;
}
 */

void PassPermit::SetPassOpenSeq(int PasOpenSeq)
{
	m_PrePasOpenSeq = PasOpenSeq;
}

int PassPermit::GetPassOpenSeq()
{
	return (int)m_PrePasOpenSeq;
}

//---------------------------------------------------------------------------
/**
@brief   List를 초기화하는 함수
@author  puting
@date    2016.03.15
@note    List는 RESPONSE_DATA 구조체의 포인터로 구성되어 있음.
*/
void PassPermit::InitPassOpenList()
{
	// List를 꺼내 해당 포인터 delete 후
	int nCount = m_pPassOpenList->getCount();
	for(int i = 0; i < nCount; i++)
    {
		CS_PassOpenList->Acquire();
        try
        {
			delete (DATA_PASSOPEN *)m_pPassOpenList->popFront();
        }
        __finally
        {
			CS_PassOpenList->Release();
        }
    }

    // 리스트를 Clear한다.
	CS_PassOpenList->Acquire();
    try
    {
		m_pPassOpenList->clear();
    }
    __finally
    {
		CS_PassOpenList->Release();
    }

}
//---------------------------------------------------------------------------

/**
@brief   List의 첫번째 Command를 delete하는 함수
@author  zzang9un
@date    2012.11.16
@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우(count가 0인 경우) 0(false)를 리턴

int PassPermit::DeleteFirstPassOpen()
{
	if(m_pPassOpenList->getCount() == 0)
        return FALSE;

	CS_PassOpenList->Acquire();
    try
    {
		delete (DATA_PASSOPEN *)m_pPassOpenList->popFront();
    }
    __finally
    {
		CS_PassOpenList->Release();
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
int PassPermit::DeletePassOpen(UINT Index)
{
	if(m_pPassOpenList->getCount() == 0)
        return FALSE;

	if(Index > m_pPassOpenList->getCount() - 1)
    {
        // Index가 List의 개수보다 클 경우
        return FALSE;
    }

	CS_PassOpenList->Acquire();
    try
    {
		delete (DATA_PASSOPEN *)m_pPassOpenList->pop(Index);
    }
    __finally
    {
		CS_PassOpenList->Release();
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
void PassPermit::AddPassOpen(void *pData)
{
	CS_PassOpenList->Acquire();
    try
    {
		m_pPassOpenList->insertBack(pData);
    }
    __finally
    {
		CS_PassOpenList->Release();
    }
}
//---------------------------------------------------------------------------

/**
@brief   List의 Index Item을 참조하는 포인트를 리턴하는 함수
@author  puting
@date    2013.10.28
@return  List의 index Item의 포인터를 리턴한다.
*/
void* PassPermit::ReferPassOpen(UINT index)
{
    void *pListItem = NULL;

	pListItem = m_pPassOpenList->refer(index);

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List의 첫번째 Item을 참조하는 포인트를 리턴하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.
*/
void* PassPermit::ReferFirstPassOpen()
{
    void *pListItem = NULL;

	pListItem = m_pPassOpenList->referFront();

    return pListItem;
}
//---------------------------------------------------------------------------



/**
@brief   List의 첫번째 Item을 Pop하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.
@note    Pop한 Item은 리스트에서 삭제된다.

void* PassPermit::PopFirstPassOpen()
{
    void *pListItem = NULL;

	CS_PassOpenList->Acquire();
    try
    {
		pListItem = m_pPassOpenList->popFront();
    }
    __finally
    {
        CS_PassOpenList->Release();
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
int PassPermit::GetPassOpenCount()
{
    return m_pPassOpenList->getCount();
}
//---------------------------------------------------------------------------
/**
@brief   List를 초기화하는 함수
@author  puting
@date    2016.03.15
@note    List는 RESPONSE_DATA 구조체의 포인터로 구성되어 있음.
*/
void PassPermit::InitPassPermitList()
{
	// List를 꺼내 해당 포인터 delete 후
	int nCount = m_pPassPermitList->getCount();
	for(int i = 0; i < nCount; i++)
    {
		delete (DATA_PASS *)m_pPassPermitList->popFront();

	}

	// 리스트를 Clear한다.

   	m_pPassPermitList->clear();

}
//---------------------------------------------------------------------------

/**
@brief   List의 첫번째 Command를 delete하는 함수
@author  zzang9un
@date    2012.11.16
@return  Delete가 성공한 경우 1(true), 그렇지 않은 경우(count가 0인 경우) 0(false)를 리턴

int PassPermit::DeleteFirstPassPermit()
{
	if(m_pPassPermitList->getCount() == 0)
        return FALSE;

	delete (DATA_PASS *)m_pPassPermitList->popFront();


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
int PassPermit::DeletePassPermit(UINT Index)
{
	if(m_pPassPermitList->getCount() == 0)
        return FALSE;

	if(Index > m_pPassPermitList->getCount() - 1)
    {
        // Index가 List의 개수보다 클 경우
        return FALSE;
    }

	delete (DATA_PASS *)m_pPassPermitList->pop(Index);


	return TRUE;
}
//---------------------------------------------------------------------------

/**
@brief   List에 Command를 추가하는 함수
@author  zzang9un
@date    2012.11.16
@param   pData : Add하려는 void 포인터
*/
void PassPermit::AddPassPermit(void *pData)
{

	m_pPassPermitList->insertBack(pData);

}
//---------------------------------------------------------------------------

/**
@brief   List의 Index Item을 참조하는 포인트를 리턴하는 함수
@author  puting
@date    2013.10.28
@return  List의 index Item의 포인터를 리턴한다.
*/
void* PassPermit::ReferPassPermit(UINT index)
{
    void *pListItem = NULL;

	pListItem = m_pPassPermitList->refer(index);

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List의 첫번째 Item을 참조하는 포인트를 리턴하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.
*/
void* PassPermit::ReferFirstPassPermit()
{
    void *pListItem = NULL;

	pListItem = m_pPassPermitList->referFront();

    return pListItem;
}
//---------------------------------------------------------------------------
/**
@brief   List의 마지막 Item을 참조하는 포인트를 리턴하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.
*/
void* PassPermit::ReferBackPassPermit()
{
	void *pListItem = NULL;

	pListItem = m_pPassPermitList->referBack();

    return pListItem;
}


/**
@brief   List의 첫번째 Item을 Pop하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.
@note    Pop한 Item은 리스트에서 삭제된다.
*/
void* PassPermit::PopFirstPassPermit()
{
    void *pListItem = NULL;


	pListItem = m_pPassPermitList->popFront();


    return pListItem;
}
//---------------------------------------------------------------------------
/**
@brief   List의 마지막 Item을 Pop하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 첫번째 Item의 포인터를 리턴한다.
@note    Pop한 Item은 리스트에서 삭제된다.
*/
void* PassPermit::PopBackPassPermit()
{
	void *pListItem = NULL;


	pListItem = m_pPassPermitList->popBack();


	return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List의 count를 리턴하는 함수
@author  zzang9un
@date    2013.04.10
@return  List의 count를 리턴한다.
*/
int PassPermit::GetPassPermitCount()
{
    return m_pPassPermitList->getCount();
}
//---------------------------------------------------------------------------

//
///**
// @class   SetIncreasePPCount 함수.
// @date    2015.10.15
// @author  puting
// @brief   포지션 얻어옴
// */
//int PassPermit::SetIncreasePPCount() //-1 일 경우 최대값 위치임
//{
//
//   m_PPCount++;
//
//   PASSPERMIT_LOG("SetIncreasePPCount %d",m_PPCount);
//   return m_PPCount;
//
//
//}
///**
// @class   SetDecreasePPCount 함수.
// @date    2015.10.15
// @author  puting
// @brief   포지션 얻어옴
// */
//int PassPermit::SetDecreasePPCount() //-1 일 경우 최소값 위치임.
//{
//	if((m_PPCount -1) < 0 )
//		return -1;
//
//   m_PPCount = m_PPCount -1;
//
//   PASSPERMIT_LOG("SetDecreasePPCount :%d",m_PPCount);
//   return m_PPCount;
//
//}
///**
// @class   SetIncreasePPCount 함수.
// @date    2015.10.15
// @author  puting
// @brief   포지션 얻어옴
// */
//int PassPermit::GetPPCount() //-1 일 경우 최대값 위치임
//{
//
//   return m_PPCount;
//
//
//}



