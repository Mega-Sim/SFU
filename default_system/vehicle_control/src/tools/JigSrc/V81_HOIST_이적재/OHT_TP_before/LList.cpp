// #include "stdafx.h"
#include "LList.h"

using namespace std;

/**
 @brief LList Class default 생성자

 변수, 포인터 변수 초기화
 @author  ehoto
 @date    2012.11.12
 */
LList::LList()
{
    m_nCount = 0;
}

/**
 @brief LList Class default 소멸자

 @author  ehoto
 @date    2012.11.12
 */
LList::~LList()
{

}
// Insert : 삽입 시 사용 ---------------------------------------------------------------------------

/**
 @brief   LList 뒤에 삽입하는 함수

 임의 타입의 변수를 리스트의 뒤에 삽입한다.
 @author  ehoto
 @date    2012.11.12
 @param   pCmd : 추가하는 변수의 포인터 ( void* )
 */
void LList::insertBack(void* pCmd)
{
    m_LList.push_back(pCmd);
    m_nCount++;
}

/**
 @brief   LList 앞에 삽입하는 함수

 임의 타입의 변수를 리스트의 앞에 삽입한다.
 @author  ehoto
 @date    2012.11.12
 @param   pCmd : 추가하는 변수의 포인터 ( void* )
 */
void LList::insertFront(void* pCmd)
{
    m_LList.push_front(pCmd);
    m_nCount++;
}

/**
 @brief   LList 임의 위치에 삽입하는 함수

 임의 타입의 변수를 리스트의 임의의 위치에 삽입한다.
 @author  ehoto
 @date    2012.11.12
 @param   pCmd : 추가하는 변수의 포인터 ( void* )
 @param   nIndex : 추가하는 위치 (0: 맨처음, N-1: 맨 마지막)
 @return  에러 여부 Return (0: 정상, 그외: 비정상)
 */
int LList::insert(void* pCmd, UINT nIndex)
{
    if(m_nCount <= 0)
        return -1;
    ///<에러처리
    if(nIndex >= m_nCount)
        return -1;
    ///<에러처리

    list<void*>::iterator iterPos = m_LList.begin();
    for(UINT i = 0; i < nIndex; i++)
    {
        iterPos++;
    }

    m_LList.insert(iterPos, pCmd);
    m_nCount++;

    return 0;
}

// Refer : 참조시 사용 (인자를 꺼내지 않음) ---------------------------------------------------------------------------
/**
 @brief   LList 맨 뒤의 인자를 참조하는 함수

 리스트 마지막에 있는 임의 타입의 변수를 참조한다.
 @author  ehoto
 @date    2012.11.12
 @return  입력된 임의 변수의 포인터 ( void* )
 */
void* LList::referBack()
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<에러처리

    iterPos = m_LList.end();
    iterPos--;
    pTemp = (void*)(*iterPos);

    return pTemp;
}

/**
 @brief   LList 맨 앞의 인자를 참조하는 함수

 리스트 맨 압에 있는 임의 타입의 변수를 참조한다.
 @author  ehoto
 @date    2012.11.12
 @return  입력된 임의 변수의 포인터 ( void* )
 */
void* LList::referFront(void)
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<에러처리

    iterPos = m_LList.begin();
    pTemp = (void*)(*iterPos);

    return pTemp;
}

/**
 @brief   LList 임의 위치의 인자를 참조하는 함수

 리스트의 임의의 위치에 있는 임의 타입의 변수를 참조한다.
 @author  ehoto
 @date    2012.11.12
 @param   nIndex : 추가하는 위치 (0: 맨처음, N-1: 맨 마지막)
 @return  입력된 임의 변수의 포인터 ( void* )
 */
void* LList::refer(UINT nIndex)
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<에러처리
    if(nIndex >= m_nCount)
        return NULL;
    ///<에러처리

    iterPos = m_LList.begin();
    for(UINT i = 0; i < nIndex; i++)
    {
        iterPos++;
    }

    pTemp = (void*)(*iterPos);

    return pTemp;
}

// Pop : 인자를 List에서 꺼냄 ---------------------------------------------------------------------------
/**
 @brief  LList 맨 뒤에서 인자를 꺼내는 함수

 리스트 마지막에 있는 임의 타입의 변수를 반환하고 List에서 제거한다.
 @author  ehoto
 @date    2012.11.12
 @return  입력된 임의 변수의 포인터 ( void* )
 */
void* LList::popBack()
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<에러처리

    iterPos = m_LList.end();
    iterPos--;
    pTemp = (void*)(*iterPos);

    m_nCount--;
    m_LList.pop_back();

    return pTemp;
}

/**
 @brief   LList 맨 앞에서 인자를 꺼내는 함수

 리스트 맨 압에 있는 임의 타입의 변수를 반환하고 List에서 제거한다.
 @author  ehoto
 @date    2012.11.12
 @return  입력된 임의 변수의 포인터 ( void* )
 */
void* LList::popFront(void)
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<에러처리

    iterPos = m_LList.begin();
    pTemp = (void*)(*iterPos);

    m_nCount--;
    m_LList.pop_front();

    return pTemp;
}

/**
 @brief   LList 임의 위치에서 인자를 꺼내는 함수

 리스트의 임의의 위치에 있는 임의 타입의 변수를 반환하고 List에서 제거한다.
 @author  ehoto
 @date    2012.11.12
 @param   nIndex : 추가하는 위치 (0: 맨처음, N-1: 맨 마지막)
 @return  입력된 임의 변수의 포인터 ( void* )
 */
void* LList::pop(UINT nIndex)
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<에러처리
    if(nIndex >= m_nCount)
        return NULL;
    ///<에러처리

    iterPos = m_LList.begin();
    for(UINT i = 0; i < nIndex; i++)
    {
        iterPos++;
    }

    pTemp = (void*)(*iterPos);

    m_nCount--;
    m_LList.erase(iterPos);

    return pTemp;
}

// ---------------------------------------------------------------------------

/**
 @brief   LList 전체를 삭제하는 함수

 리스트의 임의의 위치에 있는 임의 타입의 변수를 반환하고 List에서 제거한다.
 [주의] 내부 인자가 자동 소멸시키지 않음
 clear 호출하기 전에 내부 인자가 소멸 필요 시 꼭 소멸할 것
 그렇지 않을 경우 메모리 누수 발생 가능성 높음
 @author  ehoto
 @date    2012.11.12
 */
void LList::clear(void)
{
    m_LList.clear();
    m_nCount = 0;
}
// ---------------------------------------------------------------------------

/**
 @brief   LList에 있는 임의 변수의 개수를 return하는 함수

 리스트의 있는 임의 변수의 개수를 return하는 함수.
 @author  ehoto
 @date    2012.11.12
 @return  입력된 임의 변수의 개수 (0: 없음, 1~: 들어가 있는 인자의 개수)
 */
int LList::getCount()
{
    return m_nCount;
}
