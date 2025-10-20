/**
@file    ManualControl.h
@version OHT 7.0
@brief   ManualControl Class Header File
*/
//---------------------------------------------------------------------------

#ifndef ManualControlH
#define ManualControlH

#include <math.h>
#include <stdio.h>
#include "VirtualModuleControl.h"
#include "VirtualDrivingControl.h"
#include "VirtualTransControl.h"
#include "HWSet.h"
//#include "HWCommon.h"
#include "ExecuteInfo.h"
#include "Def_TPCommand.h"
#include "LogHandler.h"
#include "DiagManager.h"

//2014.02.10 ji7412.lee : AMC.out Update 등록
#define	WM_DSP_MSGID	    (WM_USER+100)
//---------------------------------------------------------------------------

/**
@class   ManualControl
@date    2012.11.14
@author  LSJ
@brief   ManualControl 관련 Class
*/
class ManualControl:public VirtualModuleControl
{
private:
    HWSet 					*m_pHWSet;
    HWCommon 				*m_pHWCommon;
    VirtualDrivingControl	*m_pVirtualDrivingControl;
    VirtualTransControl	    *m_pTransControl;

	DEFAULT_PARAM_SET *m_defualtparam;
	DEFAULT_LINE_PARAM_SET *m_Lineparam;   //AOHC-285
    LogUnit *pLogUnit;      ///< 로그 생성을 위한 변수
    
	FDCUnit *pFDCUnit;
	DiagManager*	m_DiagManager;
    
   	EXECUTE_INFO *m_pExecuteInfo;    
	OHTMainStatus *m_pOHTMainStatus;

    EXECUTE_MANUALCONTROL_INFO m_ManualCmd; ///< 수행해야 하는 ManualCmd
    
	int		m_nIOControlOutIndex;		///< I/O 출력시 I/O 번호 : iData1

	double	m_dJogDistance_mm;			///< 조그 이동 시 거리값 : iData1
	double	m_dAcceleration_mmpermsec2;	///< 조그 주행 시 가속도 : iData2
	double	m_dDeceleration_mmpermsec2;	///< 조그 주행 시 감속도 : iData3
	double	m_dVel_mmpermsec;			///< 조그 주행 시 속도   : iData4

	bool	m_bNew;						// 명령이 처음 하달 입력 될 때 true로 설정 

//	double	m_dJogStartPos;				// 명령 받는 위치는 아님 : 실질적으로 사용되지 않음

    // by zzang9un 2013.08.17 : Use 설정을 위한 변수
    int m_UseIndex;     ///< Use 설정을 위한 Index
    bool m_bUseOnOff;    ///< Use 설정을 위한 OnOff 변수

	void	Init();
	bool	m_bFanFailFlag;
	BYTE    m_bIgnorFoupcoveropenFalg;   //AOHT-0001 : FOUP COVER

	double m_dShift_PosLimit;  //라인별 Shift Limit 값 구분으로 추가
	double m_dShift_NegLimit;  //라인별 Shift Limit 값 구분으로 추가

protected:
	virtual EXECUTE_ACK	Stop(void);
	virtual EXECUTE_ACK	Reset(void);
	virtual	EXECUTE_ACK	TurnOff(void);	///< 기동 Off
	virtual	EXECUTE_ACK	TurnON(void);	///< 기동 ON

private:
	int 	ControlDriving();
//	int		controlTrans();		//@todo 나중에 꼭 할것	: 
//	int		controlFrame();		//@todo 나중에 꼭 할 것	: E_Stop, MC, Bumper, Lamp, etc
//	int		controlHWCommon();	//@todo 나중에 꼭 할 것	: I/O, Axis, Serial, etc

public:
    ManualControl();
    ~ManualControl();
    
private:
    int		setOutput();
    int		SetOriginAndLimit();
    int		JogCommand();
	int		ControlHW();
	int		ControlRFPIOSet();
#if(OHT_NAME == OHT_NAME_P4_MASK)
	int 	JogMove_Hoist_SmallAdd(double, double, double, double, int, double);
	void 	CheckCarrierType();
#endif


public:

	EXECUTE_ACK	InsertGeneralCmd(void *sManualControlInfo);		///<논의 필요: EXECUTE_MANUALCONTROL_INFO
	UINT 		ExecuteJob(void);
	UINT		CheckAbnormalOnWait(void);						///< 대기 시 이상 현상 확인 함수 (Step 형태로 구현 - Thread가 돌림)
	UINT		CheckAlways(void);		///< 항시 이상 현상 확인 함수 (Step 형태로 구현 - Thread가 돌림)
	bool		DeleteCmd(void *pCmd);							///< 할당 된 명령을 소멸함

	void*		LoadParam(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory){return NULL;}
	bool		RequestParamUpdate(void* pParameter){return false;}
	bool		IsParamUpdated(){return false;}

	//2014.02.10 ji7412.lee : AMC out파일 업데이트 함수, OHT->Assistant 완료보고 함수
	int         UpdateAMCDownloadFile();
	void        InformAssistantCommand();

	int 		SetOnOff(int m_UseIndex, int m_bUseOnOff);
	int			SetOutBit(int m_UseIndex, int m_bUseTrueFalse);

	void 		SendCidMonitor(void);
	bool		SendCidSetting(void);
	
	void SoundOn(int iSoundNum);
	void SoundOff();

	void PowerOnOff(int iAxis, int iOnOff);

//	void MarkTry(void);          //미사용
	void MoveByPulse(double dblPulse, double dblMoveSpeed);
	void setFanFailFlag(bool Flag);
	void setFoupCoverFlag(bool Flag);  //AOHT-0001 : FOUP COVER
	int GetFoupCoverFlag();      //AOHT-0001 : FOUP COVER
};
//---------------------------------------------------------------------------
#endif
