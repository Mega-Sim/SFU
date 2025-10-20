//---------------------------------------------------------------------------
/**
@file    LList.h
$Rev:: 467           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-01-03 1#$:  Date of last commit
@version OHT 7.0
@brief   MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef LListH
#define LListH

#include <list>

typedef unsigned int        UINT;   ///< 4byte 자료형(unsigned int)

/**
@class   LList
@date    20      12.11.12
@author  ehoto
@brief   OHT에 사용되는 Custom화된 List class
*/
class LList
{
private:
	std::list<void*> m_LList;	///< 내부에서 사용되는 STL List
	UINT m_nCount;				///< List 내부에 들어가 있는 인자 개수

public:
	LList();
	~LList();

	void insertBack(void* pCmd);		///< List 뒤에 추가
	void insertFront(void* pCmd);		///< List 앞에 추가
	int insert(void* pCmd, UINT nIndex);	///< List 임의의 위치에 추가 (처음이 0, 마지막이 N-1)

	void* referBack(void);		///< List 뒤에 추가
	void* referFront(void);		///< List 뒤에 추가
	void* refer(UINT nIndex);	///< List 뒤에 추가

	void* popFront(void);		///< List 뒤에 추가
	void* popBack(void);		///< List 뒤에 추가
	void* pop(UINT nIndex);		///< List 뒤에 추가

	void clear(void);			///< List Clear하는 함수 ~ [주의] 내부 인자가 자동 소멸시키지 않음

	int getCount();				///< List 인자 개수 받아오는 함수
};

#endif
