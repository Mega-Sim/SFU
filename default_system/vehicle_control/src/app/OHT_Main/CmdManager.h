/**
@file    CmdManager.h
@version OHT 7.0
@brief   CmdManager Class를 정의한 Header File
*/

#ifndef CmdManagerH
#define CmdManagerH
//---------------------------------------------------------------------------

#include "../Common/Protocol_Define_Modify.h"

#include "../Common/Def_DataType.h"
#include "../Common/Def_Protocol.h"
#include "../Common/Def_TPCommand.h"
//#include "OHT_TLV.h"
#include "List.h"
#include "OHTMainStatus.h"
#include "LogHandler.h"
#include "Def_HW.h"
#include "ExecuteInfo.h"
#include "Def_DefaultIniParam.h"

// by zzang9un 2013.12.05 : 누적 주행 거리가 SW Limit을 넘지 않도록 하기 위한 변수
#define	DRIVING_DISTANCE_LIMIT          800000 // 주행 거리 제한 800 meter
//---------------------------------------------------------------------------
/**
@class   CmdManager
@date    2012.10.30
@author  zzang9un
@brief   OHT Command Manager
*/
class CmdManager
{
private:
//    LList       *m_CmdList; ///< Main Command List


	LList       *m_TaskList; ///< Main Command List
	LList       *m_ResponseList; ///< Reseponse Command List
	LList		*m_CycleList;    ///< Cycle Command List

//	LList		*m_pPassOpenList;    	 ///< Pass Open Command List    puting 2016 03 15
//	LList		*m_pPassPermitList;     ///< Pass Permit Command List    puting 2016 05 31


    // by zzang9un 2013.04.09 : Quick Cmd 처리를 위해 추가
	EXECUTE_INFO *m_pExecuteInfo;

    // by zzang9un 2013.08.16 : Log 변수 생성
	LogUnit *pLogUnit;      ///< TaskControl 로그 생성을 위한 변수

    // by zzang9un 2013.10.03 : OCS 재전송 에러 검출을 위해 추가

	// by zzang9un 2013.11.11 : 주행 명령 재전송 에러 검출을 위해 추가

//	DEFAULT_PARAM_SET *m_defualtparam;
public:
	CmdManager();  // constructor
	~CmdManager(); // destructor
    OHTMainStatus *m_MainStatus;  ///< Main Status Command
    // List 관련 함수
//    void InitCmdList();
//    int DeleteFirstCmd();
//    int DeleteCmd(UINT Index);
//    void AddCmd(void *pData);
//    void* ReferFirstCmd();
//    void* PopFirstCmd();
//	int GetCmdCount();


	void InitTaskList();
//	int DeleteFirstTask();                      //미사용
	int DeleteTask(UINT Index);
	void AddTask(void *pData);
	void AddTask(void *pData,int Index);
	void* ReferFirstTask();
	void* ReferTask(UINT index);
	void* PopFirstTask();
	int GetTaskCount();


	void InitOCSResponseList();
//    int DeleteFirstOCSResponse();             //미사용
	int DeleteOCSResponse(UINT Index);
	void AddOCSResponse(void *pData);
//    void* ReferFirstOCSResponse();            //미사용
	void* PopFirstOCSResponse();
    int GetOCSResponseCount();

	void InitCycleList();
//	int DeleteFirstCycle();                     //미사용
//	int DeleteCycle(UINT Index);                //미사용
	void AddCycle(void *pData);
	void* ReferCycle(UINT index);
//	void* ReferFirstCycle();                    //미사용
//	void* PopFirstCycle();                      //미사용
	void* PopBackCycle();
	int GetCycleCount();

	int DeleteBackCycle(); //puting 작업취소에 의해 생긴부분

	//Pass Open 관련 추가 내용 puting 2016 03 15
//	void InitPassOpenList();
//	int DeleteFirstPassOpen();
//	int DeletePassOpen(UINT Index);
//	void AddPassOpen(void *pData);
//	void* ReferPassOpen(UINT index);
//	void* ReferFirstPassOpen();
//	void* PopFirstPassOpen();
//	int GetPassOpenCount();
//
//	//Pass Permit 관련 추가 내용 puting 2016 03 15
//	void InitPassPermitList();
//	int DeleteFirstPassPermit();
//	int DeletePassPermit(UINT Index);
//	void AddPassPermit(void *pData);
//	void* ReferPassPermit(UINT index);
//	void* ReferFirstPassPermit();
//	void* ReferBackPassPermit();
//	void* PopFirstPassPermit();
//	void* PopBackPassPermit();
//	int GetPassPermitCount();

	void MakeTaskList_Standard(PKT_TASKCMDDATA *pCmdPktData);


	void MakeTaskList_Standard_Del_JobOrder();

    // by zzang9un 2013.04.09 : Quick Command를 Main에게 전송하는 함수
	void SendQuickCmd2Main(QUICK_CMD QuickCmd);

	// 2013.10.16 ji7412.lee : 실행 가능한 상태인지 확인하는 함수
    int  Is_Executable(void);
    int  Is_Executable(char* ExMessage, ...);
};

#endif
