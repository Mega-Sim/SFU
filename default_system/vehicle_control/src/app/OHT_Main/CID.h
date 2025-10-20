//---------------------------------------------------------------------------

#ifndef CIDH
#define CIDH
//---------------------------------------------------------------------------


#include "HWCommon.h"
#include "Utility.h"
#include "ErrorCode.h"
#include "CommHandler.h"             //TG
#include "OHTMainStatus.h"


class CID
{
private:
 	LogUnit *pLogCID;      ///< CID 로그 생성을 위한 변수
	FDCUnit *pFDCUnit;
	HWCommon *m_pHWCommon;
	Comm_Port *m_pCommPort;   //TG
	int m_nPortNum;            //TG
	OHTMainStatus *m_pOHTMainStatus;
	EXECUTE_INFO *m_pExecuteInfo;

	bool bReadyOperation;
    int nDelaycnt;
    DWORD dwMaxMovingTime;
    int nSendCommand;
    DWORD dwStartTime;
//    DWORD dwLinkLimitTime;
	bool bNBranchCheck;
	DWORD dwCIDTrigerTime;
	DWORD dwCIDStatusTrigerTime;
	bool bPuaseStatus;//미사용...
	bool bLRNBranch;
	bool bRLNBranch;

	bool bCid_State_Reset_Need;

	bool bCIDPause;

	bool m_bImmediateLinkReq; // [고속 OHT] 추가, 2019.06.04 AOHC-279
	bool m_bRegistrationDenied; // [고속 OHT] 추가, 2019.06.04  AOHC-279

	int m_nIndex;
	char m_TempBuffer[10240];
	int m_nBranchLength;
	bool m_bCIDNameSetting;

public:

	CID(HWCommon *pHWCommon);
	~CID();

	void Init();
	int CID_Open();
//	void CID_Close();                  //미사용
	int CID_Operation_On();
	bool Is_Operation_On();
	int Left_Link();
	int Left_Link_Lifter();
	int Right_Link();
	int Right_Link_Lifter();
//	int Check_CID_Complete();
	bool IsLinkOK();
	bool IS_Occup_On();
	int CID_Occup_OK();
	int CID_Occup_Pause(bool *bcheck);
	int CID_Occup_Resume(bool *bcheck);
//	int CID_Status_Pause(bool &bcheck);            //미사용
//	int CID_Status_Resume(bool &bcheck);           //미사용
	int CID_Occup_return(unsigned char num);
	int CID_Occup_compt(unsigned char num);
	int TerminateCID(unsigned char num);
	int CID_Output_Clear(unsigned char num);
    bool CID_Fiber_Monitoring;

#if(OHT_NAME == OHT_NAME_STD_V81S)
	bool IsVHLRegistrationFull(); // [고속 OHT] 등록 요청 여부 확인, 2019.06.04
	bool IsVHLRegistrationDenied(); // [고속 OHT] 등록 요청 여부 확인, 2019.06.04
	bool VHLRegistrationDeniedinit(); // [고속 OHT] VHL 6대 초과 확인 변수 초기화

	int Immediate_Link_Req_ON(); // [고속 OHT] 등록 요청 여부 확인, 2019.06.04
	int Immediate_Link_Req_OFF(); // [고속 OHT] 등록 요청 여부 확인, 2019.06.04
	bool IsImmediateLinkReqON(); // [고속 OHT] 등록 요청 여부 확인, 2019.06.04
	bool IsLinkRequest(); //[고속 OHT] 등록 요청 여부 확인
#endif

	bool IsOutputAllOff();		// shkim. 2016-11-03 류건진 책임 추가
	bool IsInputAllOff();		// shkim. 2016-11-03 류건진 책임 추가
	
	bool IsLinking();
	bool IsOccupying();
	bool IsInterlock_ON();
	bool ISCompletionACK_On();
	bool ISCompletion_On();
	bool ISCidManualMode();
	bool ISCidOperatingMode();
	bool ISCommonTagMode();
	bool IsLinking_Right();
	bool IsLinking_Left();
	bool ISCidStatusOn();
//	int CID_Common_Occup_compt();           //미사용
	int CID_Steerring_N_Branch();
	int CID_Steerring_N_Branch_Select_Off();
	int CID_Manual_Mode_On();
	int CID_Manual_Mode_Off();
	int ReadyToOperation(int CurrenrtNodeID,bool bStation);

	//메뉴얼 처리 동작
	int CID_Occup_compt_Manual(bool bCommonTag);
//	int CID_Common_Occup_compt_Manual();

	bool Check_Mode_Lifter(bool LightGuideDirectionIsLeft);

//	int Set_CID_IDSET_Cmd(void);                   //미사용
//	int Set_CID_RC_Cmd(void);                      //미사용
	int Set_CID_Status_Cmd(void);
	int Set_CID_Monitor_Cmd(int i);
	int Set_CID_Monitor_FiberLength_Cmd(int nFiberLength);
	void Set_CID_NBranch_Length(int nLength);
//	int SendMonitoringCmd(void);                    //미사용
	int SendMonitoringCmd_Clear(void);
	int SendReq_CID_O(void);
	bool Set_CID_Name_Setting_Cmd(char* cOHTName);
	void Set_CID_Name_Setting(bool bSet);
	int Get_CID_Name_Setting();

	int Clear_CID_Status_Cmd(void); // 정상 상태 보고

//	int Get_CID_Response(AnsiString strSndData);            //미사용
//	int SerialReadBufferClear();                            //미사용
	int SerialReadBufferClearManual();
	int ReadData(char *RtnReadBuff, char cTarget);
	int ReadData(char *RtnReadBuff, char cTarget, int iSize);
	int Save_CID_O_Data();

	int SendLGCmd(char* cProtocol, int iSize);
	int SendLGCmd_NotClear(char* cProtocol, int iSize);
//	int IsElevetorArea();                                   //미사용
	int ElevetorSelectOff();
	int ElevetorOperatingModeSet(int iMode);

	bool IsCommonTag();	
//
//		OUT_CID_Left_Select_OUT,    //64
//	OUT_CID_Right_Select_OUT,   //65
//	OUT_CID_PASS_Compt_OUT,
//	OUT_CID_Manual_OUT,
//	OUT_CID_Operation_OUT,
//	OUT_CID_Reserved_OUT1,      //70
//	OUT_CID_Reserved_OUT2,

   // I/O Control
   int CID_Out_Left_Select(bool bOnOff);
   int CID_Out_Right_Select(bool bOnOff);
   int CID_Out_Occup_Req(bool bOnOff);
   int CID_Out_PASS_Compt(bool bOnOff);
   int CID_Out_Manual(bool bOnOff);
   int CID_Out_Operation(bool bOnOff);
   int CID_Out_Common_Tag(unsigned char num, bool bOnOff);
   int CID_Out_Reserved_2(bool bOnOff);

   void LOG_WRITE_2_CID_LOG(AnsiString strLog);
   bool NeedTO_executeCID_Reset();
	/*
	bool IsMovingTorque();
	int Free();
	int Enable();
	int Stop();
	bool IsWorkPos();
	bool IsHomePos();
	bool IsLongRun();
	void StartLongRun();
	void StopLongRun();
	int GetMovingCount();
	DWORD GetMaxMovingTime();
	int ReadyToOperation();
	int AutoInitialize();
	int Adjust();
   */

   // 2016-12-29, Add by Labeler
   // CID Pause 상태 확인 용 함수 추가
   bool GetCIDPause();
   void SetCIDPause(bool bSet);
   void SetUseCIDMonitoring(bool bUse);

};
// ==================================================================
//		CID H/W Infterface 정의
// ==================================================================

#define Normal_Reset_tag	1
#define Common_Reset_tag	2
#define Slope_Front_Reset_tag	3
#define Slope_Rear_Reset_tag	4


#endif

