/**
@file    VirtualIdReader.h
@version OHT 7.0
@brief   VirtualIdReader Class Header File
*/

#ifndef VirtualIdReaderH
#define VirtualIdReaderH

#include <stdarg.h>
#include <stdio.h>

typedef enum ID_READER_TYPE_
{
	ID_READER_TYPE_ID_ONLY			= 0,
	ID_READER_TYPE_ID_AND_OFFSET
}ID_READER_TYPE;


/**
@class   VirtualIdReader
@date    
@author  
@brief   VirtualIdReader 관련 Class
*/
class VirtualIdReader
{
private:

protected:
	ID_READER_TYPE m_Type;
	
public:
    VirtualIdReader(){m_Type = ID_READER_TYPE_ID_ONLY;}
    virtual ~VirtualIdReader(){;}		///< 가상 소멸자
    
//	virtual	int SetResultPointer(int* pnResultID) = 0;	///< Read Buffer 할 때 자동으로 값을 받는 변수 설정
	virtual int SetResultPointer(int* pnResultID, ...) = 0;	///< Read Buffer 할 때 자동으로 값을 받는 변수 설정
	
	virtual int SendTriggerOn() = 0;	///< Trigger On 신호를 보냄 : 대기 상태로 ID 발견하면 읽음
	virtual int SendTriggerOff() = 0;	///< Trigger Off 신호를 보냄: 대기 상태 종료
	virtual int ReadBuffer() = 0;		///< 수신 Data를 확인함
	virtual int	GetLatestID() = 0;		///< 수신된 ID를 읽음
	
	virtual int Enable() = 0;
	virtual void Disable() = 0;
	virtual int ReStart() = 0;

};

#endif  // VirtualIdReader

