/**
@file    Rotate.h
@version OHT 7.0
@brief   Rotate Class Header File
*/

#ifndef RotateH
#define RotateH

#include <System.hpp>
#include "HWCommon.h"
#include "Utility.h"
#include "ErrorCode.h"
#include "LogHandler.h"
#include "LogThread.h"

#define	NEXT_CHECK				   -1


/**
@class   Rotate
@date
@author
@brief   Rotate 관련 Class
*/
class Rotate
{
private:
	HWCommon *m_pHWCommon;
	//CommPort hCom; ///< Serial 연결을 위한 Comm Handle

	//2014.06.17 ji7412.lee v6.5 Rotate 변수 추가
	bool bReadyOperation;
	int  nSendCommand;
    DWORD dwOriginLimitTime;
    DWORD dwMotionLimitTime;
	DWORD dwStartTime;
	LogUnit* pLogJig; // Jig Log 생성 변수

public:
	Rotate(HWCommon *pHWCommon);
	~Rotate();

	void Init();

	//2014.06.17 ji7412.lee v6.5 Rotate 함수 추가
	int Rotate_0_Manual(void);
	int Rotate_0_Auto(void);
	int Rotate_90_Manual(void);
	int Rotate_90_Auto(void);
    int Rotate_180_Manual(void);
	int Rotate_180_Auto(void);
	int Rotate_270_Manual(void);
	int Rotate_270_Auto(void);
	int Origin(void);
	int Origin_forRotateAuto(void);
	int CheckMotionComplete(void);
	int ReadyToOperation(void);
	int Rotate_IO_Init(void);
	int Alarm_Check_Reset(void);
	int Free(void);
	int Enable(void);
	bool Is0Pos(void);
	bool Is18Pos(void);
	bool Is90Pos(void);
	bool Is180Pos(void);
	bool Is270Pos(void);
	bool IsInposition(void);
	bool IsAlarm(void);
	bool IsFree(void);
	int Send_Rotate_0_AutoCmd(void);
	int Send_Rotate_18_AutoCmd(void);
	int Send_Rotate_90_AutoCmd(void);
	int Send_Rotate_180_AutoCmd(void);
	int Send_Rotate_270_AutoCmd(void);
};

#endif  //RotateH



