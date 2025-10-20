//---------------------------------------------------------------------------

#ifndef PASSPERMITH
#define PASSPERMITH
//---------------------------------------------------------------------------

#include "Protocol_Define_Modify.h"

#include "../Common/Def_DataType.h"
#include "../Common/Def_Protocol.h"
#include "../Common/Def_TPCommand.h"

//#include "OHT_TLV.h"
#include "List.h"
#include "OHTMainStatus.h"
#include "LogHandler.h"
#include "Def_HW.h"
#include "ExecuteInfo.h"


#define PASSTIME_NONE   0
#define PASSTIME_30   30
#define PASSTIME_50   50

#define MIN_ACCEL   2000
#define CHANGEDISTMARIN   400
class PassPermit
{
private:
	LogUnit *pLogPasspermit;      ///< Passpermit 로그 생성을 위한 변수
	unsigned int m_PrePasOpenSeq;          ///List 처리를 위한 부분
	DWORD m_dwPassPermitTime;                ///통과허가 요청을 위한 시간 설정
	DWORD m_dwPassPeriodTime;                ///통과허가 및 개방 요청 시간 주기 추가 설정 값.

	int m_PPIndex;               ///< 통과허가에 대한 Index
	int m_PPMaxIndex;            ///< 통과허가에 대한 Max Index(최고점)
	int m_PPOIndex;              ///< 통과허가개방에 대한 Index
	double m_SumResetPosition;   ///< 거리 초기화된 누적값.
	double m_SumCyclePosition;   ///< 거리 초기화된 누적값.

	int m_PPCount;               ///< 통과허가에 대한 Count Check

	bool m_bCheckRollbackFinish;			///< 롤백 단계가 종료된 지점인지 확인하는 부분.
	bool m_bISForcePPO;

	LList		*m_pPassOpenList;    	 ///< Pass Open Command List    puting 2016 03 15
	LList		*m_pPassPermitList;     ///< Pass Permit Command List    puting 2016 05 31



public:

	PassPermit();
	~PassPermit();


	int InitPassPermit();        ///< 초기화 : 작업지시를 최초로 받은 시점에서 해줄것. 현재 작업이 완료된 시점에서 처리해줄 것.
	int GetPPIndex();			//OK
	int GetPPOIndex();			//OK
//	int GetPPMaxIndex();		//OK        //미사용
	bool GetRollBackFinish();

	DWORD GetPassPermitTime();
	DWORD GetPassPeriodTime();

	void SetPassPermitTime(DWORD dwTime);
	void SetPassPeriodTime(DWORD dwTime);

	int SetIncreasePPIndex(int nMax); 			//-1 일 경우 최대값 위치임  //OK
	int SetDecreasePPIndex();		 			//-1 일 경우 최소값 위치임.  //OK
	int SetIncreasePPOIndex(int nMax);  //OK
	void SetRollBackFinish(bool bCheck);
   	int SetDecreasePPOIndex();		 			//puting 작업 취소에 의해 호출됨.

	//작업변경으로 인하여 추가됨.
	int SetPPIndex(int nIndex);
    int SetPPOIndex(int nIndex);

	void SetSumResetPositon(double dResetPostion); ///DrvingControl에서 리셋될때 그떄 위치값만 전송해주면됨.  그리고 플래그로 전달 할 것. //OK
//	void SetSumCyclePositon(double dCyclePostion); ///DrvingControl에서 리셋될때 그떄 위치값만 전송해주면됨.  그리고 플래그로 전달 할 것. //OK      //미사용
//	double GetSumCyclePositon();   //OK      //미사용
	double GetAbsolutePositon(); //OK
	
	void SetPassOpenSeq(int PasOpenSeq);
	int GetPassOpenSeq();


		//Pass Open 관련 추가 내용 puting 2016 03 15
	void InitPassOpenList();
//	int DeleteFirstPassOpen();             //미사용
	int DeletePassOpen(UINT Index);
	void AddPassOpen(void *pData);
	void* ReferPassOpen(UINT index);
	void* ReferFirstPassOpen();
//	void* PopFirstPassOpen();              //미사용
	int GetPassOpenCount();

	//Pass Permit 관련 추가 내용 puting 2016 03 15
	void InitPassPermitList();
//	int DeleteFirstPassPermit();           //미사용
	int DeletePassPermit(UINT Index);
	void AddPassPermit(void *pData);
	void* ReferPassPermit(UINT index);
	void* ReferFirstPassPermit();
	void* ReferBackPassPermit();
	void* PopFirstPassPermit();
	void* PopBackPassPermit();
	int GetPassPermitCount();


//	int SetDecreasePPCount();		 			//-1 일 경우 최소값 위치임.  //OK
//	int SetIncreasePPCount();  //OK
//	int GetPPCount();

};


#endif

