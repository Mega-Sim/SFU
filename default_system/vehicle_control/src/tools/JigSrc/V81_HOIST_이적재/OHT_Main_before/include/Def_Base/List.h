//---------------------------------------------------------------------------
/**
@file    List.h
@version OHT 7.0
@brief   MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

//#include "stdafx.h"

#ifndef ListH
#define ListH

#include <list>
#include <algorithm>

typedef unsigned int        UINT;   ///< 4byte 자료형(unsigned int)

/**
@class   List
@date    20      12.11.12
@author  ehoto
@brief   OHT에 사용되는 Custom화된 List class
*/
template <typename T>
class List
{
private:
	std::list<T> m_StlList;		///< 내부에서 사용되는 STL List
	UINT m_nCount;				///< List 내부에 들어가 있는 인자 개수

public:
	List();
	~List();

	void insertBack(T Value);			///< List 뒤에 추가
	void insertFront(T Value);			///< List 앞에 추가
	int insert(T Value, UINT nIndex);	///< List 임의의 위치에 추가 (처음이 0, 마지막이 N-1)

	T referBack(void);		///< List 뒤에 추가
	T referFront(void);		///< List 뒤에 추가
	T refer(UINT nIndex);	///< List 뒤에 추가

	T popFront(void);		///< List 뒤에 추가
	T popBack(void);		///< List 뒤에 추가
	T pop(UINT nIndex);		///< List 뒤에 추가

	UINT find(T Value);

	void clear(void);		///< List Clear하는 함수 ~ [주의] 내부 인자가 자동 소멸시키지 않음

	UINT getCount();		///< List 인자 개수 받아오는 함수
	UINT getCountsize();
};

/**
@brief List Class default 생성자

         변수, 포인터 변수 초기화
@author  ehoto
@date    2012.11.12
*/
template <typename T>
List<T>::List()
{
	m_nCount=0;
}

/**
@brief List Class default 소멸자

@author  ehoto
@date    2012.11.12
*/
template <typename T>
List<T>::~List()
{

}
// Insert : 삽입 시 사용 ---------------------------------------------------------------------------

/**
@brief   List 뒤에 삽입하는 함수

         임의 타입의 변수를 리스트의 뒤에 삽입한다.
@author  ehoto
@date    2012.11.12
@param   Value : 추가하는 변수의 포인터 ( T )
*/
template <typename T>
void List<T>::insertBack(T Value)
{
	m_StlList.push_back(Value);
	m_nCount++;
}

/**
@brief   List 앞에 삽입하는 함수

         임의 타입의 변수를 리스트의 앞에 삽입한다.
@author  ehoto
@date    2012.11.12
@param   Value : 추가하는 변수의 포인터 ( T )
*/
template <typename T>
void List<T>::insertFront(T Value)
{
	m_StlList.push_front(Value);
	m_nCount++;
}

/**
@brief   List 임의 위치에 삽입하는 함수

         임의 타입의 변수를 리스트의 임의의 위치에 삽입한다.
@author  ehoto
@date    2012.11.12
@param   Value : 추가하는 변수의 포인터 ( T )
@param   nIndex : 추가하는 위치 (0: 맨처음, N-1: 맨 마지막)
@return  에러 여부 Return (0: 정상, 그외: 비정상)
*/
template <typename T>
int List<T>::insert(T Value, UINT nIndex)
{
	if(m_nCount==0) return -1;		///<에러처리
	if(nIndex>=m_nCount) return -1;	///<에러처리

	std::list<T>::iterator iterPos = m_StlList.begin();
	for(UINT i=0; i<nIndex; i++)
	{
		++iterPos;
	}

	m_StlList.insert(iterPos, Value);
	m_nCount++;

	return 0;
}

// Refer : 참조시 사용 (인자를 꺼내지 않음) ---------------------------------------------------------------------------
/**
@brief   List 맨 뒤의 인자를 참조하는 함수

         리스트 마지막에 있는 임의 타입의 변수를 참조한다.
@author  ehoto
@date    2012.11.12
@return  입력된 임의 변수의 포인터 ( T )
*/
template <typename T>
T List<T>::referBack()
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;	///<에러처리

	iterPos = m_StlList.end();
	--iterPos;
	pTemp = (*iterPos);

	return pTemp;
}

/**
@brief   List 맨 앞의 인자를 참조하는 함수

         리스트 맨 압에 있는 임의 타입의 변수를 참조한다.
@author  ehoto
@date    2012.11.12
@return  입력된 임의 변수의 포인터 ( T )
*/
template <typename T>
T List<T>::referFront(void)
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;	///<에러처리

	iterPos = m_StlList.begin();
	pTemp = (*iterPos);

	return pTemp;
}

/**
@brief   List 임의 위치의 인자를 참조하는 함수

         리스트의 임의의 위치에 있는 임의 타입의 변수를 참조한다.
@author  ehoto
@date    2012.11.12
@param   nIndex : 추가하는 위치 (0: 맨처음, N-1: 맨 마지막)
@return  입력된 임의 변수의 포인터 ( T )
*/
template <typename T>
T List<T>::refer(UINT nIndex)
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;		///<에러처리
	if(nIndex>=m_nCount) return NULL;	///<에러처리

	iterPos = m_StlList.begin();
	for(UINT i=0; i<nIndex; i++)
	{
		++iterPos;
	}

	pTemp = (*iterPos);

	return pTemp;
}

// Pop : 인자를 List에서 꺼냄 ---------------------------------------------------------------------------
/**
@brief  List 맨 뒤에서 인자를 꺼내는 함수

         리스트 마지막에 있는 임의 타입의 변수를 반환하고 List에서 제거한다.
@author  ehoto
@date    2012.11.12
@return  입력된 임의 변수의 포인터 ( T )
*/
template <typename T>
T List<T>::popBack()
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;	///<에러처리

	iterPos = m_StlList.end();
	--iterPos;
	pTemp = (*iterPos);

	m_nCount--;
	m_StlList.pop_back();

	return pTemp;
}
/**
@brief   List 맨 앞에서 인자를 꺼내는 함수

         리스트 맨 압에 있는 임의 타입의 변수를 반환하고 List에서 제거한다.
@author  ehoto
@date    2012.11.12
@return  입력된 임의 변수의 포인터 ( T )
*/
template <typename T>
T List<T>::popFront(void)
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;	///<에러처리

	iterPos = m_StlList.begin();
	pTemp = (*iterPos);

	m_nCount--;
	m_StlList.pop_front();

	return pTemp;
}

/**
@brief   List 임의 위치에서 인자를 꺼내는 함수

         리스트의 임의의 위치에 있는 임의 타입의 변수를 반환하고 List에서 제거한다.
@author  ehoto
@date    2012.11.12
@param   nIndex : 추가하는 위치 (0: 맨처음, N-1: 맨 마지막)
@return  입력된 임의 변수의 포인터 ( T )
*/
template <typename T>
T List<T>::pop(UINT nIndex)
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;		///<에러처리
	if(nIndex>=m_nCount) return NULL;	///<에러처리

	iterPos = m_StlList.begin();
	for(UINT i=0; i<nIndex; i++)
	{
		++iterPos;
	}

	pTemp = (*iterPos);

	m_nCount--;
	m_StlList.erase(iterPos);

	return pTemp;
}

//---------------------------------------------------------------------------

/**
@brief   List 전체를 삭제하는 함수

         리스트의 임의의 위치에 있는 임의 타입의 변수를 반환하고 List에서 제거한다.
		 [주의] 내부 인자가 자동 소멸시키지 않음
		 clear 호출하기 전에 내부 인자가 소멸 필요 시 꼭 소멸할 것
		 그렇지 않을 경우 메모리 누수 발생 가능성 높음
@author  ehoto
@date    2012.11.12
*/
template <typename T>
void List<T>::clear(void)
{
	m_StlList.clear();
	m_nCount = 0;
}
//---------------------------------------------------------------------------

/**
@brief   LList에 있는 임의 변수의 개수를 return하는 함수

         리스트의 있는 임의 변수의 개수를 return하는 함수.
@author  ehoto
@date    2012.11.12
@return  입력된 임의 변수의 개수 (0: 없음, 1~: 들어가 있는 인자의 개수)
*/
template <typename T>
UINT List<T>::getCount()
{
	return m_nCount;
}

template <typename T>
UINT List<T>::getCountsize()
{
	return m_StlList.size();
}

template <typename T>
UINT List<T>::find(T Value)
{

   int index=0;

   std::list<T>::iterator iterPos;

   iterPos = m_StlList.begin();

	for(UINT i=0; i<m_nCount; i++)
	{
		if((*iterPos) == Value) break;
		++iterPos;
		index++;
	}

	if(m_nCount== index)
	{
		index =-1;
	}

	return index;
}


typedef List<void*> LList;


#endif
