//---------------------------------------------------------------------------

#ifndef JigThreadH
#define JigThreadH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include "ThreadCtrl.h"

#include "LogHandler.h"
#include "LogThread.h"
#include "MyTools.h"
#include "OHTMainStatus.h"
#include "DrivingControl.h"

// 2019.07.05 Jig Test 파라미터

typedef struct JigParam__
{
	// Hoist Jig Test param
	double dHoist_TeachingPos; // Hoist 아래로 뻗는 값 mm
//	double dHoist_HomePos;	   // Hoist 위에서 멈추는 값 mm

	int nHoist_Interval_ms;       // 1회 테스트 후 쉬는 interval time
	int nHoist_CycleCnt;		  // 몇회 테스트 할지
	int nHoist_CurrentCnt;		  // 현재 카운트.

	// Slide Jig Test Param
	double dSlide_SlideTeachingPos;	// Slide 좌우측으로 뻗는 값 mm
	double dSlide_Acc;
	double dSlide_Vel;

	int nSlide_Interval_ms;       // 1회 테스트 후 쉬는 interval time
	int nSlide_CycleCnt;		  // 몇회 테스트 할지
	int nSlide_CurrentCnt;		  // 현재 카운트.

	// Rotate Jig test Param
	int nRotate_Interval_ms;       // 1회 테스트 후 쉬는 interval time
	int nRotate_CycleCnt;          // 몇회 테스트 할지
	int nRotate_CurrentCnt;        // 현재 카운트.

	// Hand Jig Test Param
	int nHand_Interval_ms;
	int nHand_CycleCnt;
	int nHand_CurrentCnt;

	// Shutter Jig Test Param
	int nShutter_Interval_ms;           // 1회 테스트 후 쉬는 interval time
	int nShutter_CycleCnt;				// 몇회 테스트 할지
	int nShutter_CurrentCnt;			// 현재 카운트.


    JigParam()	{  ZeroMemory(this, sizeof(*this));	}

}JigParam;

enum JIGUNIT
{
    NONE = 0,
	HOIST,
	SLIDE,
	ROTATE,
	HAND,
	SHUTTER

};

enum JIGSTEP
{
	INITIALIZATION = 0,
	LONGRUN,
	ABNORMAL
};


class JigThread : public ThreadCtrl
{
private:

    CRITICAL_SECTION	m_CS;
    LogUnit* pLogJig; // Jig Log 생성 변수
	int m_nError;
	int nError;
	int m_nHoistError;
	int m_nSlideError;
	bool bRunning;
	DrivingControl	*m_pDrivingControl;
	DRIVING_HW_INFO_SET JigHwInfo;

protected:

	void RealThreadProc(void);


public:
	JigThread();
	~JigThread();

	void ExecuteHoistSteps();   //Hoist Excute
	void ExecuteSlideSteps();   //Slide Excute
	void ExecuteShutterSteps(); //Shutter Excute
	void ExecuteRotateSteps();  //Rotate Excute
	void ExecuteHandSteps();    //Hand Excute

	int GetJigError();  // JIG Thread에서 에러발생했을 때, Main thread에서 Error 처리를 하기위한 함수
	int GetMainError(int nError); 	 //Main Error 발생시 처리
	int GetShutterError(int nError); //Shutter Error 발생시 처리
	int GetHoistError(int nError);	 //Hoist Error 발생시 처리
	int GetSlideError(int nError);	 //Slide Error 발생시 처리
	int GetRotateError(int nError);	 //Rotate Error 발생시 처리
	int GetHandError(int nError);	 //Hand Error 발생시 처리


	// Unit 별 Class 정의
	CStepClass m_StepJig_Hoist;    //Hoist Step
	CStepClass m_StepJig_Slide;    //Slide Step
	CStepClass m_StepJig_Rotate;   //Shutter Step
	CStepClass m_StepJig_Hand;     //Rotate Step
	CStepClass m_StepJig_Shutter;  //Hand Step

	JigParam m_JigParam;


	int Err_cnt;
	int AlarmReset_cnt;

	int nJogVal;

	SYSTEMTIME CurTime;

	OHTMainStatus *m_pMainStatus;

};
#endif
