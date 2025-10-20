/**
@file    VirtualIdAndOffsetReader.h
@version OHT 7.0
@brief   VirtualIdAndOffsetReader Class Header File
*/

#ifndef VirtualIdAndOffsetReaderH
#define VirtualIdAndOffsetReaderH

#include "VirtualIdReader.h"

/**
@class   VirtualIdAndOffsetReader
@date    
@author  
@brief   VirtualIdAndOffsetReader 관련 Class
*/
class VirtualIdAndOffsetReader: public VirtualIdReader
{
private:
	
public:
    VirtualIdAndOffsetReader(){m_Type = ID_READER_TYPE_ID_AND_OFFSET;}
    virtual ~VirtualIdAndOffsetReader(){;}		///< 가상 소멸자
    
	//virtual	int SetResultPointer(int* pnReadedID, double* pdReadedOffset) = 0;	///< Read Buffer 할 때 자동으로 값을 받는 변수 설정
	virtual double	GetLatestOffset() = 0;

};

#endif  // VirtualIdAndOffsetReader

