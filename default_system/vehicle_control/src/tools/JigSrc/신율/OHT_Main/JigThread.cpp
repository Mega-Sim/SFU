//---------------------------------------------------------------------------


#pragma hdrstop

#include "JigThread.h"
#include "OHTMain.h"
#include "Def_OHT.h"
#include "TaskControl.h"
#include "HWSet.h"
#include "JigHoistUnit.h"
#include "JigSlideUnit.h"
#include "JigRotateUnit.h"
#include "JigHandUnit.h"

//---------------------------------------------------------------------------
#define LOG_JIG(...)   WRITE_LOG_UNIT(pLogJig, __VA_ARGS__)

#if((OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V86) || (OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S) ||(OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
#define HOIST_SPEED_RATIO      0.8573       // OHT V7.0 감속비 12.86:1
#else
#define HOIST_SPEED_RATIO      1.0      // OHT V7.0 감속비 15:1
#endif

#if(OHT_NAME == OHT_NAME_STD_V85)
#define HOIST_MAX_SPEED	2.75
#define HOIST_ESTOP_DECEL       (HOIST_MAX_SPEED / 400.0)   // msec^2
#define SHIFT_ESTOP_DECEL       (SHIFT_MAX_SPEED / 280.0)   // msec^2
#else
#define HOIST_MAX_SPEED        2.14*HOIST_SPEED_RATIO ///< Hoist 이적재 최고 속도(mm/msec) : 02.14
#define HOIST_ESTOP_DECEL       (HOIST_MAX_SPEED / 280.0)   // msec^2
#define SHIFT_ESTOP_DECEL       (SHIFT_MAX_SPEED / 280.0)   // msec^2
#endif
#define HOIST_ACCTIME_500      500

// Slide 정의 필요
#define SHIFT_MAX_SPEED        0.475
#define SHIFT_ACCTIME          600   //msec
//기존 속도 (V8.1)
//#define SHIFT_LOAD_VEL		   0.300000
//#define SHIFT_UNLOAD_VEL	   0.700000
//변경 속도 - 김성규 프로 요청
#define SHIFT_LOAD_VEL		   1.100000
#define SHIFT_UNLOAD_VEL	   1.100000
#define Hoist_Home	300


TaskControl *m_pTaskControl = NULL;
HWSet *m_pHWSet = NULL; // HWSet Class 변수
HWCommon* m_pHWCommon = NULL;


JigThread::JigThread() : ThreadCtrl()
{
	InitializeCriticalSection(&m_CS);
	m_pHWCommon = OHTMainForm->GetHWCommon();
	m_pHWSet = OHTMainForm->GetHWSet();
	m_pTaskControl = OHTMainForm->GetTaskControl();
	m_pDrivingControl = OHTMainForm->GetDrivingControl();
	JigHwInfo = m_pDrivingControl->getHWInfo();
//	= getHWInfo();

	pLogJig = getLogUnit("D:\\log\\jig.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	m_nError = NO_ERR; // rotate, hand JIG용
	nError = NO_ERR; // hoist, slide JIG용
	m_nHoistError  = NO_ERR;      //임시로 사용 예정
	m_nSlideError  = NO_ERR;	  //임시로 사용 예정

	Err_cnt = 0;
	AlarmReset_cnt = 0;
	nJogVal = 0;

	LOG_JIG("ThreadCheck - Start");

	//Hoist Parameter Default Setting
//	m_JigParam.dHoist_HomePos = 0;
	m_JigParam.dHoist_TeachingPos = 0;
	m_JigParam.nHoist_Interval_ms = 0;
	m_JigParam.nHoist_CycleCnt = 0;
	m_JigParam.nHoist_CurrentCnt = 0;

	//Shutter Parameter Default Setting
	m_JigParam.nShutter_Interval_ms = 2000;
	m_JigParam.nShutter_CycleCnt = 100;
	m_JigParam.nShutter_CurrentCnt = 0;

	//Rotate Parameter Default Setting
	m_JigParam.nRotate_Interval_ms = 15000;
	m_JigParam.nRotate_CycleCnt = 100;
	m_JigParam.nRotate_CurrentCnt = 0;

	//Rotate Parameter Default Setting
	m_JigParam.nHand_Interval_ms = 3000;
	m_JigParam.nHand_CycleCnt = 100;
	m_JigParam.nHand_CurrentCnt = 0;


}


JigThread::~JigThread()
{
	LOG_JIG("ThreadCheck - End");
	DeleteCriticalSection(&m_CS);

}

void JigThread::RealThreadProc()
{
	int m_nError = NO_ERR;   // rotate, Hand JIG ERROR
	int nError = NO_ERR; // hoist , slide JIG ERROR     OHT에러가 정상적인지 확인 필요
	int m_nHoistError  = NO_ERR;      //임시로 사용 예정
	int m_nSlideError  = NO_ERR;	  //임시로 사용 예정
	int JigUnit = 0;
	int nJigCnt = 0;

    // ---- Place thread code here ----
	while(!m_bThreadBreaker)
    {


		if(m_bThreadBreaker == true) // Thread 종료를 위해 필수로 추가
            break;

		if(m_Tick == MAX_TICK)
			m_Tick = 0;
		else
			m_Tick++;
//
//        if(nError != NO_ERR) //다른 쓰레드에서 에러가 났을 때, Break;
//            break;
//        if(m_nError != NO_ERR) //지그 쓰레드에서 에러가 났을 때, Break;
//            break;

		try{

			JigUnit = 0;
			nJigCnt = 0;


//			nError = OHTMainForm->Jig_ErrReturn();

			//Hoist Jig 구동
			if(m_StepJig_Hoist.bSignal)
			{
				if(GetHoistError(m_nHoistError)!= NO_ERR)
				{
					m_StepJig_Hoist.Step = 200;
					m_StepJig_Hoist.nErrorCode = m_nHoistError;
					m_StepJig_Hoist.StepComment = "Error Step";
					m_StepJig_Hoist.ErrMsg = "Main Error";
					m_StepJig_Hoist.bSignal = false;
					LOG_JIG("*[HOIST] ERROR : %d",m_nHoistError);
				}
				else
				{
					m_StepJig_Hoist.nErrorCode = 0;
					m_StepJig_Hoist.StepComment = "";
					m_StepJig_Hoist.ErrMsg = "";
					if(m_StepJig_Hoist.Step != 200)
						ExecuteHoistSteps();
				}

			}
			else
			{
				m_StepJig_Hoist.StepComment = "Thread Stop, Please Reset";
			}



			//Rotate Jig 구동
			if(m_StepJig_Rotate.bSignal)
			{
				if(GetRotateError(m_nError)!= NO_ERR)
				{
					m_StepJig_Rotate.Step = 200;
					m_StepJig_Rotate.nErrorCode = m_nError;
					m_StepJig_Rotate.StepComment = " Error Step";
					m_StepJig_Rotate.ErrMsg = "Main Error";
					m_StepJig_Rotate.bSignal = false;
				}
				else
				{
					m_StepJig_Rotate.nErrorCode = 0;
					m_StepJig_Rotate.StepComment = "";
					m_StepJig_Rotate.ErrMsg ="";
					if(m_StepJig_Rotate.Step != 200)
						ExecuteRotateSteps();
				}

			}
			else
			{
				m_StepJig_Rotate.StepComment = "Thread Stop, Please Reset";
			}


			//Slide Jig 구동
			if(m_StepJig_Slide.bSignal)
			{
				if(GetSlideError(m_nSlideError)!= NO_ERR)
				{
					m_StepJig_Slide.Step = 200;
					m_StepJig_Slide.nErrorCode = m_nSlideError;
					m_StepJig_Slide.StepComment = "Error Step";
					m_StepJig_Slide.ErrMsg = "Main Error";
					m_StepJig_Slide.bSignal = false;
					LOG_JIG("*[SLIDE] ERROR : %d",m_nSlideError);
				}
				else
				{
					m_StepJig_Slide.nErrorCode = 0;
					m_StepJig_Slide.StepComment = "";
					m_StepJig_Slide.ErrMsg = "";
					if(m_StepJig_Slide.Step != 200)
						ExecuteSlideSteps();
				}
			}
			else
			{
				m_StepJig_Slide.StepComment = "Thread Stop, Please Reset";
			}

//			//Shutter Jig 구동
//			if(m_StepJig_Shutter.bSignal)
//			{
//				if(GetShutterError(m_nError)!= NO_ERR)
//				{
//					m_StepJig_Shutter.Step = 200;
//					m_StepJig_Shutter.nErrorCode = m_nError;
//					m_StepJig_Shutter.StepComment = " Error Step";
//					m_StepJig_Shutter.ErrMsg = "Main Error";
//					m_StepJig_Shutter.bSignal = false;
//				}
//				else
//				{
//					m_StepJig_Shutter.nErrorCode = 0;
//					m_StepJig_Shutter.StepComment = "";
//					m_StepJig_Shutter.ErrMsg ="";
//					if(m_StepJig_Shutter.Step != 200)
//						ExecuteShutterSteps();
//				}
//
//			}
//			else
//			{
//				m_StepJig_Shutter.StepComment = "Thread Stop, Please Reset";
//			}

			//Hand Jig 구동
			if(m_StepJig_Hand.bSignal)
			{
				if(GetHandError(m_nError)!= NO_ERR)
				{
					m_StepJig_Hand.Step = 200;
					m_StepJig_Hand.nErrorCode = m_nError;
					m_StepJig_Hand.StepComment = " Error Step";
					m_StepJig_Hand.ErrMsg = "Main Error";
					m_StepJig_Hand.bSignal = false;
				}
				else
				{
					m_StepJig_Hand.nErrorCode = 0;
					m_StepJig_Hand.StepComment = "";
					m_StepJig_Hand.ErrMsg ="";
					if(m_StepJig_Hand.Step != 200)
						ExecuteHandSteps();
				}

			}
			else
			{
				m_StepJig_Hand.StepComment = "Thread Stop, Please Reset";
			}


		}
		catch(...)
		  LOG_JIG("Exception!!!!");



        Sleep(10);
    }



}

//****************
// HJH 2019.07.05
// Shutter Step

void JigThread::ExecuteShutterSteps()
{

}


//****************
// HJH 2020.03.11
// Hoist Step
void JigThread::ExecuteHoistSteps()
{

//	int nPreCount = m_JigParam.nHoist_CurrentCnt;
//	int nCurrentCount = OHTMainForm->GetTransControl()->GetHoistCycleCount();
//
//	if(nPreCount != nCurrentCount)
//	{
//		nPreCount = nCurrentCount;
//		m_JigParam.nHoist_CurrentCnt = nCurrentCount;
//	}


	OHTMainStatus *m_pMainStatus = OHTMainForm->GetOHTStatus();
	BYTE MainStatus = m_pMainStatus->StatusCommon.MSCmdID.Info.Status;
	//Sequence
	int Status = (m_StepJig_Hoist.Step)/100;
	int Step = m_StepJig_Hoist.Step;

	//Interval Time
	int nIntervalTime = m_JigParam.nHoist_Interval_ms;
	double dVel_mmpermsec = 0;
	double dAccel_mmpermsec2 = 0;
	double dDecel_mmpermsec2 = 0;
	double dVal = 0;
	double dPos_mm = 0;

	int     nTmpID = JigHwInfo.TagStaion.nBcrId;
	bool    bDetectFirst                = JigHwInfo.IO.TagStationFrontOn;
	bool    bDetectSecond               = JigHwInfo.IO.TagStationRearOn;

	// 추후에 확인 필요.
//	if(MainStatus == STS_ERROR)
//	{
//		m_StepJig_Hoist.ErrMsg = "Main - Error Status";
//		m_StepJig_Hoist.StepComment = "Main - Error Status";
//
//		if(Step >= 100 && Step<200)
//		{
//			GetLocalTime(&CurTime);
//			HoistUnit->edtEndDate->Text = HoistUnit->edtEndDate->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
//		}
//		m_StepJig_Hoist.Step = 200;
//		return;
//	}

	switch(Status)
	{
	// Status 0 : Init, 초기상태 Lonrun, Manual 실행 전
	// Status 100: Longrun, Count 만큼 구동 or 롱런모드
	// Status 200: Abnormal, Error발생

	case JIGSTEP::INITIALIZATION:
		{
			switch(Step)
			{

			// Sequence 0: waiting
			// Sequence 1: Servo On
			// Sequence 2: Servo Off
			// Sequence 3: Go Home
			// Sequence 4: Set Home
			// Sequence 5: jog Move + Down
			// Sequence 6: jog Move - Up
				case 0:
					m_StepJig_Hoist.ErrMsg = "";
					m_StepJig_Hoist.StepComment = "Waiting";

				break;

				case 1:
					m_StepJig_Hoist.StepComment = "Amp ON";
					m_pHWSet->m_pHoistAxis->SetAMPEnable(true);
					m_StepJig_Hoist.Step = 0;
				break;

				case 2:
					m_StepJig_Hoist.StepComment = "Amp OFF";
					m_pHWSet->m_pHoistAxis->SetAMPEnable(false);
					m_StepJig_Hoist.Step = 0;
				break;

				case 3:
					m_StepJig_Hoist.StepComment = "Go Home";

					dVel_mmpermsec = HOIST_MAX_SPEED * (double)0.1;
																			  //Hoist Home 300mm
					m_nHoistError = m_pHWSet->m_pHoistAxis->MoveDistance((DWORD)500, (DWORD)500, dVel_mmpermsec, Hoist_Home);

					m_pHWSet->m_pDrivingAxis->SetAMPEnable(true);
					m_pHWSet->m_pDrivingAxis->SetSlaveAMPEnable(false);

					LOG_JIG("신률개선버전 21.11.25");

					if(m_nHoistError != 0)
					{
						m_StepJig_Hoist.ErrMsg = "JigError - Go Home";
						m_StepJig_Hoist.Step = 200;
						LOG_JIG("GO HOME HOIST STEP 200  : %d",m_nHoistError);
					}
					else
					{
						m_StepJig_Hoist.Step = 0;
						LOG_JIG("GO HOME HOIST STEP 200  : %d",m_nHoistError);
					}

				break;
				case 4:

					if((m_pHWSet->m_pHoistAxis->GetCurrentPosition() < 10) &&
					(m_pHWSet->m_pHoistAxis->GetCurrentPosition() > -10))
					{
						m_StepJig_Hoist.StepComment = "Set Home";
						m_pHWSet->m_pHoistAxis->SetHome();
					}
					else
						m_StepJig_Hoist.StepComment = "10mm Limit Over";

					m_StepJig_Hoist.Step = 0;

				break;
				case 5:

					dVel_mmpermsec = 0.2075;
					dAccel_mmpermsec2 /= HOIST_ACCTIME_500;
					dDecel_mmpermsec2 /= HOIST_ACCTIME_500;
					dPos_mm = m_pHWSet->m_pHoistAxis->GetCurrentPosition() +  nJogVal;

					m_nHoistError = m_pHWSet->m_pHoistAxis->MoveDistance((DWORD)500, (DWORD)500, dVel_mmpermsec, dPos_mm);
					m_StepJig_Hoist.StepComment.sprintf( L"Jog Move(+) to %f(v:%f,a:%f,d:%f)",dPos_mm, dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2);

					if(m_nHoistError != 0)
					{
						m_StepJig_Hoist.ErrMsg = "JigError - Jog Move(+)";
						m_StepJig_Hoist.Step = 200;
						LOG_JIG("JOG이동+ HOIST STEP 200  : %d",m_nHoistError);
					}
					else
					{
						m_StepJig_Hoist.ErrMsg = "Success, Step->0";
						m_StepJig_Hoist.Step = 0;
						LOG_JIG("JOG이동+ HOIST STEP 0  : %d ",m_nHoistError);
					}


				break;
				case 6:

					dVel_mmpermsec = 0.2075;
					dAccel_mmpermsec2 /= HOIST_ACCTIME_500;
					dDecel_mmpermsec2 /= HOIST_ACCTIME_500;

					dPos_mm = m_pHWSet->m_pHoistAxis->GetCurrentPosition() - nJogVal;

					m_StepJig_Hoist.StepComment.sprintf( L"Jog Move(-) to %f(v:%f,a:%f,d:%f)",dPos_mm, dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2);
					m_nHoistError = m_pHWSet->m_pHoistAxis->MoveDistance((DWORD)500, (DWORD)500, dVel_mmpermsec, dPos_mm);

					if(m_nHoistError != 0)
					{
						m_StepJig_Hoist.ErrMsg = "JigError - Jog Move(-)";
						m_StepJig_Hoist.Step = 200;
						LOG_JIG("JOG이동- HOIST STEP 200  : %d",m_nHoistError);
					}
					else
					{

						m_StepJig_Hoist.ErrMsg = "Success, Step->0";
						m_StepJig_Hoist.Step = 0;
						LOG_JIG("JOG이동- HOIST STEP 0  : %d ",m_nHoistError);
					}


				break;
			}
		}

    break;

	case JIGSTEP::LONGRUN:
		{
			//Interval 대기

			if(!(Step == 100))
			{
				//Interval 대기
				if(m_StepJig_Hoist.StepDelay.OnDelay(true, nIntervalTime))
				{
					m_StepJig_Hoist.StepDelay.OnDelay(false);
				}
				else
					break;
			}
			switch(Step)
			{
				// Sequence 100: Longrun Init
                // Sequence 101: Longrun Unload
				// Sequence 102: Longrun Load
				// Sequence 103: Longrun End
				case 100:

					m_StepJig_Hoist.StepComment = "Longrun - Check";


					m_pHWSet->m_pDrivingAxis->SetAMPEnable(true);
					m_pHWSet->m_pDrivingAxis->SetSlaveAMPEnable(false);

					if(bDetectFirst == true &&  bDetectSecond == true)
					{
//						LOG_JIG("[Longrun] [Hoist] Start : OK,  First Detect : %d, Second Detect : %d",bDetectFirst ,bDetectSecond);
					}
					else
					{
						Application->MessageBox(L"Not Mark...", L"Error", MB_OK + MB_ICONSTOP);
						LOG_JIG("[Longrun] [Hoist] Start : FAIL,  First Detect : %d, Second Detect : %d",bDetectFirst ,bDetectSecond);
						m_StepJig_Hoist.Step = 200;
						m_StepJig_Hoist.ErrMsg = "Not Mark!";
					}

					if((m_pHWSet->m_pHoistAxis->GetCurrentPosition() < Hoist_Home+2) &&
							 (m_pHWSet->m_pHoistAxis->GetCurrentPosition() > Hoist_Home-2))
//					if(m_pHWCommon->m_Input.HOIST_Home == ON)
					{
						if(MainStatus == STS_INIT)
						{
							m_StepJig_Hoist.Step++;
							m_StepJig_Hoist.StepComment = "Longrun - Start";
							LOG_JIG("[Longrun] [Hoist] Start : Teaching Data : %f",m_JigParam.dHoist_TeachingPos);
						}
						else
							m_StepJig_Hoist.ErrMsg = "OHT is Not Initialized!";
					}
					else
					{
						m_StepJig_Hoist.Step = 0;
						m_StepJig_Hoist.ErrMsg = "Hoist Not home!";
					}


//					m_StepJig_Hoist.Step++;

				break;

				case 101:

					//HW 동작
					if(bDetectFirst == true &&  bDetectSecond == true)
					{
//						LOG_JIG("[Longrun] [Hoist] step1 : OK,  First Detect : %d, Second Detect : %d",bDetectFirst ,bDetectSecond);
					}
					else
					{
						Application->MessageBox(L"Mark Fail...", L"Error", MB_OK + MB_ICONSTOP);
						LOG_JIG("[Longrun] [Hoist] step1 : Fail,  First Detect : %d, Second Detect : %d",bDetectFirst ,bDetectSecond);
						m_StepJig_Hoist.Step = 200;
						m_StepJig_Hoist.ErrMsg = "Station Sensor Error!";
					}


					if(MainStatus == STS_LOADED || MainStatus == STS_INIT)
					{
						m_StepJig_Hoist.StepComment = "Longrun - Step";
						m_StepJig_Hoist.ErrMsg = "";

						if((m_pHWSet->m_pHoistAxis->GetCurrentPosition() < Hoist_Home+2) &&
							 (m_pHWSet->m_pHoistAxis->GetCurrentPosition() > Hoist_Home-2))
						{
//							m_pTaskControl->HoistLongRun(1, m_JigParam.dHoist_TeachingPos);
							m_pTaskControl->HoistLongRun(1, m_JigParam.dHoist_TeachingPos, Hoist_Home);     //Load 명령
							m_StepJig_Hoist.Step++;
						}
//						else
//						{
//							dHomePos = (double)(((double)(m_pTaskControl->m_pHWSet->m_pHoistAxis->GetCurrentPosition()*1000))/1000.0);
//
//							if(m_StepJig_Hoist.bHomeChange == true && fabs(dHomePos-m_JigParam.dHoist_HomePos)<0.5)
//
//							m_pTaskControl->HoistLongRun(1, m_JigParam.dHoist_TeachingPos, m_JigParam.dHoist_HomePos);
//							m_StepJig_Hoist.Step++;
//						}
					}

				break;

				case 102:
					//HW 동작

					if(bDetectFirst == true &&  bDetectSecond == true)
					{
//						LOG_JIG("[Longrun] [Hoist] step2 : OK,  First Detect : %d, Second Detect : %d",bDetectFirst ,bDetectSecond);
					}
					else
					{
						Application->MessageBox(L"Mark Fail...", L"Error", MB_OK + MB_ICONSTOP);
						LOG_JIG("[Longrun] [Hoist] step2 : Fail,  First Detect : %d, Second Detect : %d",bDetectFirst ,bDetectSecond);
						m_StepJig_Hoist.Step = 200;
						m_StepJig_Hoist.ErrMsg = "Station Sensor Error!";
					}


					if(MainStatus == STS_LOADED)
					{
						m_StepJig_Hoist.StepComment = "Longrun - Step2";
						m_StepJig_Hoist.ErrMsg = "";

						if((m_pHWSet->m_pHoistAxis->GetCurrentPosition() < Hoist_Home+2) &&
							 (m_pHWSet->m_pHoistAxis->GetCurrentPosition() > Hoist_Home-2))
						{
							m_JigParam.nHoist_CurrentCnt++;
							LOG_JIG("[Longrun] [Hoist] Count: %d",m_JigParam.nHoist_CurrentCnt);

							if(m_StepJig_Hoist.bLongrun == true)
								m_StepJig_Hoist.Step--;
							else
							{
								//카운트 완료 -> END
								if(m_JigParam.nHoist_CurrentCnt >= m_JigParam.nHoist_CycleCnt)
									m_StepJig_Hoist.Step++;
								//카운트중
								else
									m_StepJig_Hoist.Step--;
							}
						}
					}
					else
					{
                    	m_StepJig_Hoist.StepComment = "Longrun - ing";
                    }
				break;

                case 103:
					//롱런 종료
					m_StepJig_Hoist.StepComment = "Longrun - End";

					GetLocalTime(&CurTime);
					HoistUnit->edtEndDate->Text = HoistUnit->edtEndDate->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);


                break;
			}

		}
	break;

	case JIGSTEP::ABNORMAL:
		{
			switch(Step)
			{
				// Sequence 200: Abnormal Alarm
				case 200:
				m_StepJig_Hoist.StepComment = "Error Step";
				break;
			}
		}
	break;

	default:
		m_StepJig_Hoist.StepComment = "Abnormal";

	break;
	}

}


//****************
// timy 2020.04.08
// Slide Step

void JigThread::ExecuteSlideSteps()
{

	OHTMainStatus *m_pMainStatus = OHTMainForm->GetOHTStatus();
	BYTE MainStatus = m_pMainStatus->StatusCommon.MSCmdID.Info.Status;
	//Sequence
	int Status = (m_StepJig_Slide.Step)/100;
	int Step = m_StepJig_Slide.Step;

	//Interval Time
	int nIntervalTime = m_JigParam.nSlide_Interval_ms;
	double dVel_mmpermsec = 0;
	double dAccel_mmpermsec2 = 0;
	double dDecel_mmpermsec2 = 0;
	double dVal = 0;
	double dPos_mm = 0;
	double dCurrent_Pos_mm = m_pHWSet->m_pShiftAxis->GetCurrentPosition(); //현재 위치
	double dTeaching_mm = m_JigParam.dSlide_SlideTeachingPos; //티칭 위치

	// 추후에 확인 필요.
//	if(MainStatus == STS_ERROR)
//	{
//		m_StepJig_Hoist.ErrMsg = "Main - Error Status";
//		m_StepJig_Hoist.StepComment = "Main - Error Status";
//
//		if(Step >= 100 && Step<200)
//		{
//			GetLocalTime(&CurTime);
//			HoistUnit->edtEndDate->Text = HoistUnit->edtEndDate->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
//		}
//		m_StepJig_Hoist.Step = 200;
//		return;
//	}

	switch(Status)
	{
	// Status 0 : Init, 초기상태 Lonrun, Manual 실행 전
	// Status 100: Longrun, Count 만큼 구동 or 롱런모드
	// Status 200: Abnormal, Error발생

	case JIGSTEP::INITIALIZATION:
		{
			switch(Step)
			{

			// Sequence 0: waiting
			// Sequence 1: Servo On
			// Sequence 2: Servo Off
			// Sequence 3: Go Home
			// Sequence 4: Set Home
			// Sequence 5: jog Move + Down
			// Sequence 6: jog Move - Up
				case 0:
					m_StepJig_Slide.ErrMsg = "";
					m_StepJig_Slide.StepComment = "Waiting";

				break;

				case 1:
					m_StepJig_Slide.StepComment = "Amp ON";
					m_pHWSet->m_pShiftAxis->SetAMPEnable(true);
					m_StepJig_Slide.Step = 0;
				break;

				case 2:
					m_StepJig_Slide.StepComment = "Amp OFF";
					m_pHWSet->m_pShiftAxis->SetAMPEnable(false);
					m_StepJig_Slide.Step = 0;
				break;

				case 3:
					m_StepJig_Slide.StepComment = "Go Home";

					dVel_mmpermsec = SHIFT_MAX_SPEED * (double)0.2;

					m_nSlideError = m_pHWSet->m_pShiftAxis->MoveDistance((DWORD)600, (DWORD)600, dVel_mmpermsec, 0);

					if(m_nSlideError == NO_ERR)
					{
						m_StepJig_Slide.Step = 0;
						LOG_JIG("GO HOME SLIDE STEP 3 : %d", m_nSlideError);
					}
					else
					{
						m_StepJig_Slide.ErrMsg = "JigError - Go Home";
						m_StepJig_Slide.Step = 200;
						LOG_JIG("GO HOME SLIDE STEP 200  : %d", m_nSlideError);
					}

				break;
				case 4:

					if((m_pHWSet->m_pShiftAxis->GetCurrentPosition() < 10) &&
					(m_pHWSet->m_pShiftAxis->GetCurrentPosition() > -10))
					{
						m_StepJig_Slide.StepComment = "Set Home";
						m_pHWSet->m_pShiftAxis->SetHome();
					}
					else
						m_StepJig_Slide.StepComment = "10mm Limit Over";

					m_StepJig_Slide.Step = 0;

				break;
				case 5:

					dVel_mmpermsec = SHIFT_MAX_SPEED * (double)0.2;
					dAccel_mmpermsec2 = SHIFT_ACCTIME;
					dDecel_mmpermsec2 = SHIFT_ACCTIME;
					dPos_mm = m_pHWSet->m_pShiftAxis->GetCurrentPosition() +  nJogVal;

					m_nSlideError = m_pHWSet->m_pShiftAxis->MoveDistance((DWORD)600, (DWORD)600, dVel_mmpermsec, dPos_mm);
					m_StepJig_Slide.StepComment.sprintf( L"Jog Move(+) to %f(v:%f,a:%f,d:%f)",dPos_mm, dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2);

					if(m_nSlideError == NO_ERR)
					{
						m_StepJig_Slide.ErrMsg = "Success, Jog Move";
						m_StepJig_Slide.Step = 0;
						LOG_JIG("JOG이동+ SLIDE STEP 0  : %d ",m_nSlideError);
					}
					else
					{
						m_StepJig_Slide.ErrMsg = "JigError - Jog Move(+)";
						m_StepJig_Slide.Step = 200;
						LOG_JIG("JOG이동+ SLIDE STEP 200  : %d",m_nSlideError);
					}


				break;
				case 6:

					dVel_mmpermsec = SHIFT_MAX_SPEED * (double)0.2;
					dAccel_mmpermsec2 = SHIFT_ACCTIME;
					dDecel_mmpermsec2 = SHIFT_ACCTIME;

					dPos_mm = m_pHWSet->m_pShiftAxis->GetCurrentPosition() - nJogVal;

					m_nSlideError = m_pHWSet->m_pShiftAxis->MoveDistance((DWORD)600, (DWORD)600, dVel_mmpermsec, dPos_mm);
					m_StepJig_Slide.StepComment.sprintf( L"Jog Move(-) to %f(v:%f,a:%f,d:%f)",dPos_mm, dVel_mmpermsec, dAccel_mmpermsec2, dDecel_mmpermsec2);

					if(m_nSlideError == NO_ERR)
					{
						m_StepJig_Slide.ErrMsg = "Success, Jog Move";
						m_StepJig_Slide.Step = 0;
						LOG_JIG("JOG이동- SLIDE STEP 0  : %d ",m_nSlideError);
					}
					else
					{
						m_StepJig_Slide.ErrMsg = "JigError - Jog Move(-)";
						m_StepJig_Slide.Step = 200;
						LOG_JIG("JOG이동- SLIDE STEP 200  : %d",m_nSlideError);
					}


				break;
			}
		}

	break;

	case JIGSTEP::LONGRUN:
		{
			//Interval 대기

			if(!(Step == 100))
			{
				//Interval 대기
				if(m_StepJig_Slide.StepDelay.OnDelay(true, nIntervalTime))
				{
					m_StepJig_Slide.StepDelay.OnDelay(false);
				}
				else
					break;
			}
			switch(Step)
			{
				// Sequence 100: Longrun Init
                // Sequence 101: Longrun Unload
				// Sequence 102: Longrun Load
				// Sequence 103: Longrun End
				case 100:

					m_StepJig_Slide.StepComment = "Longrun - Check";

					// 원점센서가 없어서 +-2mm 기준으로 분류
					if((m_pHWSet->m_pShiftAxis->GetCurrentPosition() < 2) && (m_pHWSet->m_pShiftAxis->GetCurrentPosition() > -2))
					{
						if(m_nSlideError == NO_ERR) // NO_ERR 로 변경
						{
							m_StepJig_Slide.Step++;
							m_StepJig_Slide.StepComment = "Longrun - Start";
							LOG_JIG("[Longrun] [Slide] Start : Teaching Data : %f"	,dTeaching_mm);
						}
						else
							m_StepJig_Slide.ErrMsg = "OHT is Not Initialized!";
					}
					else
					{
						m_StepJig_Slide.Step = 0;
						m_StepJig_Slide.ErrMsg = "Slide Not home!";
					}

				break;

				case 101:
					//HW 동작
					dVel_mmpermsec = SHIFT_MAX_SPEED * (double)SHIFT_LOAD_VEL; // + 방향 MOVE LOAD 속도  [Teaching]

					m_nSlideError = m_pHWSet->m_pShiftAxis->MoveDistance((DWORD)600, (DWORD)600, dVel_mmpermsec, dTeaching_mm);

					if(m_nSlideError == NO_ERR)
					{
						if(dCurrent_Pos_mm >= (dTeaching_mm-1))
						{
							m_StepJig_Slide.Step++;
							m_StepJig_Slide.StepComment = "Longrun - Slide Move +";
							LOG_JIG("[Longrun] [Slide] Move + Teaching, Current : %f"	,dCurrent_Pos_mm );
						}
					}
					else
					{
						m_StepJig_Slide.ErrMsg = "Longrun - Slide Move Error";
						m_StepJig_Slide.Step = 200;
						LOG_JIG("Slide STEP 101  Error : %d", m_nSlideError);
					}


				break;

				case 102:
					//HW 동작
					dVel_mmpermsec = SHIFT_MAX_SPEED * (double)SHIFT_UNLOAD_VEL ; // - 방향 MOVE UNLOAD 속도 [Home]

					m_nSlideError = m_pHWSet->m_pShiftAxis->MoveDistance((DWORD)600, (DWORD)600, dVel_mmpermsec, 0);

					if(m_nSlideError == NO_ERR)
					{
						if(dCurrent_Pos_mm <= 1)
						{
							m_StepJig_Slide.Step++;
							m_StepJig_Slide.StepComment = "Longrun - Slide Move - [Home]";
							LOG_JIG("[Longrun] [Slide] Move - Home, Current : %f" ,dCurrent_Pos_mm );
						}
					}
					else
					{
						m_StepJig_Slide.ErrMsg = "Longrun - Slide Move Error";
						m_StepJig_Slide.Step = 200;
						LOG_JIG("Slide STEP 102  Error : %d", m_nSlideError);
					}

				break;

				case 103:
					//HW 동작
					dVel_mmpermsec = SHIFT_MAX_SPEED * (double)SHIFT_LOAD_VEL; // - 방향 MOVE LOAD 속도  [Teaching]

					m_nSlideError = m_pHWSet->m_pShiftAxis->MoveDistance((DWORD)600, (DWORD)600, dVel_mmpermsec, -(dTeaching_mm));

					if(m_nSlideError == NO_ERR)
					{
						if(dCurrent_Pos_mm <= -(dTeaching_mm-1))
						{
							m_StepJig_Slide.Step++;
							m_StepJig_Slide.StepComment = "Longrun - Slide Move - ";
							LOG_JIG("[Longrun] [Slide] Move - Teaching, Current : %f "	,dCurrent_Pos_mm);
						}
					}
					else
					{
						m_StepJig_Slide.ErrMsg = "Longrun - Slide Move Error";
						m_StepJig_Slide.Step = 200;
						LOG_JIG("Slide STEP 103  Error : %d", m_nSlideError);
					}


				break;

				case 104:
					//HW 동작
					dVel_mmpermsec = SHIFT_MAX_SPEED * (double)SHIFT_UNLOAD_VEL; // + 방향 MOVE LOAD 속도

					m_nSlideError = m_pHWSet->m_pShiftAxis->MoveDistance((DWORD)600, (DWORD)600, dVel_mmpermsec, 0);


					//다음 스텝
					if(m_nSlideError == NO_ERR)
					{
						if(dCurrent_Pos_mm >= -1)
						{
							m_StepJig_Slide.StepComment = "Longrun - Slide Move + [Home] ";
							LOG_JIG("[Longrun] [Slide] Move + Home, Current : %f",dCurrent_Pos_mm );

							//1cycle 완료, Count 증가
							m_JigParam.nSlide_CurrentCnt++;
							LOG_JIG("[Longrun] [Slide] Count: %d", m_JigParam.nSlide_CurrentCnt);

							//롱런 체크
							if(m_StepJig_Slide.bLongrun == true)
								m_StepJig_Slide.Step = 101;
							//롱런 미체크
							else
							{
							//카운트 완료 -> END
							if(m_JigParam.nSlide_CurrentCnt >= m_JigParam.nSlide_CycleCnt)
								m_StepJig_Slide.Step++;
							//카운트중
							else
								m_StepJig_Slide.Step = 101;
							}
						}
					}
					//에러 처리
					else
					{
						m_StepJig_Slide.ErrMsg = "Longrun - Slide Move Error";
						m_StepJig_Slide.Step = 200;
						LOG_JIG("Slide STEP 104  Error : %d", m_nSlideError);
					}


				break;

				case 105:
					//롱런 종료
					m_StepJig_Slide.StepComment = "Longrun - End";

					GetLocalTime(&CurTime);
					SlideUnit->edtEndDate->Text = SlideUnit->edtEndDate->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);


                break;
			}

		}
	break;

	case JIGSTEP::ABNORMAL:
		{
			switch(Step)
			{
				// Sequence 200: Abnormal Alarm
				case 200:
				m_StepJig_Slide.StepComment = "Error Step";
				m_StepJig_Slide.ErrMsg = "Error";
				m_StepJig_Slide.bSignal = false; // Abnormal 시 동작 플래그 끔

				break;
			}
		}
	break;

	default:
		m_StepJig_Slide.StepComment = "Abnormal";

	break;
	}

}
//****************
// timy 2020.04.08
// Rotate Step
void JigThread::ExecuteRotateSteps()
{

	//Sequence
	int Step = (m_StepJig_Rotate.Step)/100;
	int Sequence = m_StepJig_Rotate.Step;

	//Interval Time
	int nIntervalTime = m_JigParam.nRotate_Interval_ms;


	switch(Step)
	{
	// step 0 : Init, 초기상태 Lonrun, Manual 실행 전
	// step 100: Longrun, Count 만큼 구동 or 롱런모드
	// step 200: Abnormal, Error발생

	case JIGSTEP::INITIALIZATION:


			switch(Sequence)
			{
			// Sequence 0: waiting
			// Sequence 1: Origin setting
			// Sequence 2: Reset/Stop
			// Sequence 3: Start
			// Sequence 4: 0'
			// Sequence 5: 90'
			// Sequence 6: 180'
			// Sequence 7: 270'
			// Sequence 8: Alarm Reset

				case 0:
						m_StepJig_Rotate.ErrMsg = "";
						m_StepJig_Rotate.StepComment = "Waiting";

				break;

				case 1:       // Origin

					m_StepJig_Rotate.StepComment = "Origin Setting";
//					m_nError = m_pHWSet->m_pRotate->Origin();
					LOG_JIG("Rotate Origin 명령 실시");
					m_StepJig_Rotate.Step = 0;

				break;

				case 2:      // Reset

					m_StepJig_Rotate.StepComment = "Reset";
					m_StepJig_Rotate.ErrMsg = "Reset";
					m_StepJig_Rotate.Step = 0;
//					m_nError = NO_ERR;  // 조금 더 생각해보자.

				break;


				case 3:

					m_StepJig_Rotate.StepComment = "Run";
					m_StepJig_Rotate.Step = 100; //Longrun Start

				break;


				case 4:       // 0'

					m_StepJig_Rotate.StepComment = "Move 0'";
//					m_nError = m_pHWSet->m_pRotate->Send_Rotate_0_AutoCmd();
					LOG_JIG("Rotate 0도 명령 실시");
					m_StepJig_Rotate.Step = 0;

				break;


				case 5:       // 90'

					m_StepJig_Rotate.StepComment = "Move 90'";
//					m_nError = m_pHWSet->m_pRotate->Send_Rotate_90_AutoCmd();
					LOG_JIG("Rotate 90도 명령 실시");
					m_StepJig_Rotate.Step = 0;

				break;


				case 6:       // 180'

					m_StepJig_Rotate.StepComment = "Move 180'";
//					m_nError = m_pHWSet->m_pRotate->Send_Rotate_180_AutoCmd();
					LOG_JIG("Rotate 180도 명령 실시");
					m_StepJig_Rotate.Step = 0;

				break;


				case 7:       // 270'

					m_StepJig_Rotate.StepComment = "Move 270'";
//					m_nError = m_pHWSet->m_pRotate->Send_Rotate_270_AutoCmd();
					LOG_JIG("Rotate 270도 명령 실시");
					m_StepJig_Rotate.Step = 0;

				break;


				case 8:       // Alarm Reset

					m_StepJig_Rotate.StepComment = "Alarm Reset";
//					m_nError = m_pHWSet->m_pRotate->Alarm_Check_Reset();
					m_StepJig_Rotate.Step = 0;

				break;

			}


	break;

	case JIGSTEP::LONGRUN:

			if(!(Sequence == 100))
			{
				//Interval 대기
				if(m_StepJig_Rotate.StepDelay.OnDelay(true, nIntervalTime))
				{
					m_StepJig_Rotate.StepDelay.OnDelay(false);
				}
				else
					break;
			}

			switch(Sequence)
			{
                // Sequence 100: Longrun Init
				// Sequence 101: Longrun 270'
				// Sequence 102: Longrun   0'
				// Sequence 103: Longrun End


				case 100:

					m_StepJig_Rotate.StepComment = "Longrun - Check";

					//Error check
					if(m_nError != NO_ERR)
					{
						m_StepJig_Rotate.Step = 200;
						break;
					}

					//HW 동작
					LOG_JIG("[Longrun] [Rotate] Start, Error : %d",m_nError);
//

					//Count 초기화
					m_StepJig_Rotate.Step++;


				break;

				case 101:

					m_StepJig_Rotate.StepComment = "[Longrun] [Rotate] - 270'";
					//HW 동작
//					m_nError = m_pHWSet->m_pRotate->Send_Rotate_270_AutoCmd();
					LOG_JIG("[Longrun] [Rotate] 270'");

					//다음 스텝
					if(m_nError == NO_ERR)
						m_StepJig_Rotate.Step++;
					//에러 처리
					else
						m_StepJig_Rotate.Step = 200;

				 break;


				case 102:


					m_StepJig_Rotate.StepComment = "[Longrun] [Rotate] - 0'";

					//HW 동작
//					m_nError = m_pHWSet->m_pRotate->Send_Rotate_0_AutoCmd();
					LOG_JIG("[Longrun] [Rotate] 0'");

					//다음 스텝
					if(m_nError == NO_ERR)
					{
						//1cycle 완료, Count 증가
						m_JigParam.nRotate_CurrentCnt++;
						LOG_JIG("[Longrun] [Rotate] Count: %d", m_JigParam.nRotate_CurrentCnt);

						//롱런 체크
						if(m_StepJig_Rotate.bLongrun == true)
							m_StepJig_Rotate.Step--;
						//롱런 미체크
						else
						{
							//카운트 완료 -> END
							if(m_JigParam.nRotate_CurrentCnt >= m_JigParam.nRotate_CycleCnt)
								m_StepJig_Rotate.Step++;
							//카운트중
							else
								m_StepJig_Rotate.Step--;
						}
					}
					//에러 처리
					else
					{
						LOG_JIG("[Longrun [Rotate] - STOP(ERROR)] ERROR : %d",m_nError);
						m_StepJig_Rotate.Step = 200;
					}

				break;

				case 103:
                    //롱런 종료
					m_StepJig_Rotate.StepComment = "Longrun - End";
					LOG_JIG("[longrun - End]");
					RotateUnit->edt_RotateErrTime->Text = RotateUnit->edt_RotateErrTime->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
				break;
			}


    break;

	case JIGSTEP::ABNORMAL:

			switch(Sequence)
			{
				// Sequence 200: Abnormal Alarm
				case 200:
					//다시 개선 필요.
					m_StepJig_Rotate.StepComment = "Abnormal";
					m_StepJig_Rotate.ErrMsg = "Error";
					m_StepJig_Rotate.bSignal = false; // Abnormal 시 동작 플래그 끔

				break;
			}

	break;

    default:
	break;
	}


}
//****************
// timy 2020.04.08
// Hand Step
void JigThread::ExecuteHandSteps()
{

	//Sequence
	int Step = (m_StepJig_Hand.Step)/100;
	int Sequence = m_StepJig_Hand.Step;

	//Interval Time
	int nIntervalTime = m_JigParam.nHand_Interval_ms;


	switch(Step)
	{
	// step 0 : Init, 초기상태 Lonrun, Manual 실행 전
	// step 100: Longrun, Count 만큼 구동 or 롱런모드
	// step 200: Abnormal, Error발생

	case JIGSTEP::INITIALIZATION:


			switch(Sequence)
			{
			// Sequence 0: waiting
			// Sequence 1: Origin
			// Sequence 2: Reset/Stop
			// Sequence 3: Start
			// Sequence 4: Open
			// Sequence 5: Close
			// Sequence 6: Alarm Reset

				case 0:
						m_StepJig_Hand.ErrMsg = "";
						m_StepJig_Hand.StepComment = "Waiting";

				break;

				case 1:       // Origin

					m_StepJig_Hand.StepComment = "Origin Setting";
					m_nError = m_pHWSet->m_pHand->Origin();
					LOG_JIG("Hand Origin 명령 실시");
					m_StepJig_Hand.Step = 0;

				break;

				case 2:      // Reset

					m_StepJig_Hand.StepComment = "Reset";
					m_StepJig_Hand.ErrMsg = "Reset";
					m_StepJig_Hand.Step = 0;

                break;


				case 3:

					m_StepJig_Hand.StepComment = "Run";
					m_StepJig_Hand.Step = 100; //Longrun Start


				break;


				case 4:       // Hand Open

					m_StepJig_Hand.StepComment = "Hand Open";
					m_nError = m_pHWSet->m_pHand->ManualOpen();
					LOG_JIG("Hand Open 실시");
					m_StepJig_Hand.Step = 0;

				break;


				case 5:       // Hand Close

					m_StepJig_Hand.StepComment = "Hand Close";
					m_nError = m_pHWSet->m_pHand->ManualClose();
					LOG_JIG("Hand Close 실시");
					m_StepJig_Hand.Step = 0;

				break;


				case 6:       // Alarm Reset

					m_StepJig_Hand.StepComment = "Alarm Reset";
					m_nError = m_pHWSet->m_pHand->AlarmReset();
					LOG_JIG("Hand Alarm Reset 명령 실시");
					m_StepJig_Hand.Step = 0;

				break;

			}


	break;

	case JIGSTEP::LONGRUN:

			if(!(Sequence == 100))
			{
				//Interval 대기
				if(m_StepJig_Hand.StepDelay.OnDelay(true, nIntervalTime))
				{
					m_StepJig_Hand.StepDelay.OnDelay(false);
				}
				else
					break;
			}

			switch(Sequence)
            {
                // Sequence 100: Longrun Init
				// Sequence 101: Longrun Open
				// Sequence 102: Longrun Close
				// Sequence 103: Longrun End


				case 100:

					m_StepJig_Hand.StepComment = "Longrun - Check";

					//Error check
					if(m_nError != NO_ERR)
					{
						m_StepJig_Hand.Step = 200;
                        break;
					}

					//HW 동작
					LOG_JIG("[Longrun] [Hand] Start, Error : %d",m_nError);

					//Count 초기화
					m_StepJig_Hand.Step++;


				break;

				case 101:

					m_StepJig_Hand.StepComment = "Longrun - OPEN";
					//HW 동작
					m_nError = m_pHWSet->m_pHand->ManualOpen();
					LOG_JIG("[Longrun] [Hand] OPEN");

					//다음 스텝
					if(m_nError == NO_ERR)
						m_StepJig_Hand.Step++;
					//에러 처리
					else
						m_StepJig_Hand.Step = 200;

				 break;


				case 102:


					m_StepJig_Hand.StepComment = "Longrun - CLOSE'";

					//HW 동작
					m_nError = m_pHWSet->m_pHand->ManualClose();
					LOG_JIG("[Longrun] [Hand] CLOSE");

					//다음 스텝
					if(m_nError == NO_ERR)
					{
						//1cycle 완료, Count 증가
						m_JigParam.nHand_CurrentCnt++;
						LOG_JIG("[Longrun] [Hand] Count: %d", m_JigParam.nHand_CurrentCnt);

						//롱런 체크
						if(m_StepJig_Hand.bLongrun == true)
							m_StepJig_Hand.Step--;
						//롱런 미체크
						else
						{
							//카운트 완료 -> END
							if(m_JigParam.nHand_CurrentCnt >= m_JigParam.nHand_CycleCnt)
								m_StepJig_Hand.Step++;
							//카운트중
							else
								m_StepJig_Hand.Step--;
						}
                    }
                    //에러 처리
					else
					{
						LOG_JIG("[longrun - STOP(ERROR)] ERROR : %d",m_nError);
						m_StepJig_Hand.Step = 200;
					}

				break;

				case 103:
                    //롱런 종료
					m_StepJig_Hand.StepComment = "Longrun - End";
					LOG_JIG("[longrun - End]");
					HandUnit->edt_HandErrTime->Text = HandUnit->edt_HandErrTime->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
				break;
			}


    break;

	case JIGSTEP::ABNORMAL:

			switch(Sequence)
			{
				// Sequence 200: Abnormal Alarm
				case 200:
					//다시 개선 필요.
					m_StepJig_Hand.StepComment = "Abnormal";
					m_StepJig_Hand.ErrMsg = "Error";
					m_StepJig_Hand.bSignal = false;

				break;
			}

	break;

    default:
	break;
	}



}


int JigThread::GetJigError()
{
    return m_nError;
}

int JigThread::GetShutterError(int nError)   // 지그별 알맞는 Error Code 기입
{

	if( (nError == 161)||
		(nError>=698 && nError<=701)||
		(nError>=719 && nError<=733)||
		(nError>=209 && nError<=242)||
		(nError == ERR_TP_ESTOP))
	{
		return nError;
	}
	else
		return NO_ERR;
	return nError;
}

int JigThread::GetHoistError(int nError)
{
	return nError;
}

int JigThread::GetSlideError(int nError)
{
	return nError;
}

int JigThread::GetRotateError(int nError)
{
	if(
		(nError == 158)||
		(nError>=875 && nError<=885)
	  )
	{
		return nError;
	}
	else
		return NO_ERR;

}

int JigThread::GetHandError(int nError)
{
	if(
//		(nError == 929)||
		(nError>=929 && nError<=939)
	  )
	{
		LOG_JIG("[Hand ERROR]  %d",nError);
		return nError;
	}
	else
		return NO_ERR;

}
