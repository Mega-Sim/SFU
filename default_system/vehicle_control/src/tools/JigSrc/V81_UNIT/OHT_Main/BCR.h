/**
@file    BCR.h
@version OHT 7.0
@brief   BCR Class Header File
*/

#ifndef BCRH
#define BCRH

#include "VirtualIdReader.h"
#include "HWCommon.h"
#include "CommHandler.h"
#include "ErrorCode.h"

#include "LogHandler.h" // Log 작성 목적

/**
@class   BCR
@date    
@author  
@brief   BCR 관련 Class
*/
class BCR: public VirtualIdReader
{
private:
	LogUnit *pLogBCRRaw;      ///< BCR Raw 로그 생성을 위한 변수

    Comm_Port *m_pCommPort;
    HWCommon *m_pHWCommon;

    int m_nPortNum;
    int m_nIndex;
	
	int m_nLatestID;
	int* m_pnResultID;

	//진단
	int m_nRetryCount;
	int m_nMovingCount;
	CRITICAL_SECTION m_MovCntCS;

	int nPreID;
	bool ReadCheck;


public:
    BCR(HWCommon *pHWCommon, int nLeftRight);
    ~BCR();
    
	bool IsBCROK();
    bool IsBCRNG();
    
	int GetID(int *nID);
	int GetID(int *pnID, bool *bCheck);  //AOHC-249 BCR Read Sequence 개선

	int SetResultPointer(int* pnResultID, ...);
	
	int SendTriggerOn();
	int SendTriggerOff();
	int ReadBuffer();
	int	GetLatestID();
	
	
	int Enable();
	void Disable();
	int ReStart();
    bool isEnable();		//AOHC-249 BCR Read Sequence 개선
    //진단
	int GetMovingCount(bool bInit);
	int GetRetryCount();
	void ResetRetryCount();
	void IncreaseRetryCount();

};

#endif  //BCRH
