/**
@file    RailSupport.h
@version OHT 7.0
@brief   RailSupport Class Header File
*/

#ifndef RAILSUPPORTH
#define RAILSUPPORTH

#include "HWCommon.h"
#include "Utility.h"

/**
@class   RailSupport
@date    2012.10.22
@author  LSJ
@brief   Rail Support Unit 관련 Class
@note    Rail Support Unit을 구동하기 위한 함수 구현
*/
class RailSupport
{
private:
	HWCommon *m_pHWCommon;

    bool bReadyOperation;
	bool bLongRun;
	int nMovingCount;
	int nDelayTime;
	DWORD dwMaxMovingTime;
	int nSendCommand;
	DWORD dwStartTime;
    DWORD dwOriginLimitTime;
    DWORD dwMotionLimitTime;
	int m_nCheckOuttiggerCount;

public:

	RailSupport(HWCommon *pHWCommon);
	~RailSupport();

	void Init();
	int MoveWorkPos();
	int MoveHomePos();
	int SendMoveWorkPosCmd();
	int SendMoveHomePosCmd();
	int CheckComplete();
	bool IsMovingTorque();
	int Free();
	int Enable();
	int Stop();
	bool IsWorkPos(int bCheck =0);
	bool IsHomePos();
	bool IsLongRun();
	void StartLongRun();
	void StopLongRun();
	int GetMovingCount();
	DWORD GetMaxMovingTime();
    int ReadyToOperation();
	int AutoInitialize();
	int Adjust();

};

#endif  //RAILSUPPORTH


