/**
@file    Rotate.h
@version OHT 7.0
@brief   Rotate Class Header File
*/

#ifndef RotateH
#define RotateH

#include "HWCommon.h"
#include "Utility.h"
#include "ErrorCode.h"
#include "CommHandler.h"
#include "LogHandler.h"	// Log 작성 목적
#include "MyTools.h"
#define ROTATE_BAUDRATE     38400



/***********************************************************
 *  Content : Coolmuscle(Rotate) Command Define            *
/***********************************************************/
#define  ROTATE_END                         CR+LF
#define  READ_POS_COMMAND                   "?96"
#define  READ_VEL_COMMAND                   "?97"
#define  READ_TORQUE_COMMAND                   "?98"
#define  READ_STATUS_COMMAND                "?99"
#define  READ_POS_STATUS_COMMAND            "[8"
#define  ROTATE_ENABLE                      "(.1"
#define  ROTATE_DISABLE                     ").1"
#define  ROTATE_SETORIGIN                   "K48="
#define  ROTATE_ZEROING                     "|"
#define  ROTATE_STOP                        "].1"
#define  ROTATE_MANUAL_MOVE                 "[3"
#define  ROTATE_AUTO_MOVE                   "[4"
#define  ROTATE_MANUAL_LOC                  "P3.1="
#define  ROTATE_MANUAL_LOC2                 "P4.1="
#define  ROTATE_HOME_PARA                   "P3.1=0"
#define  ROTATE_JOGMOVE_CW                  "[1"
#define  ROTATE_JOGMOVE_CCW                 "[2"
#define  CR                                 0x0D
#define  LF                                 0x0A

#define COMM_WAIT_TIME                      100
#define COMM_RECIVIE_TIME_LIMIT             1000
#define	ROTATE_ABS_WAITTIME	                5000	//msec

#define INCORRECT_RESPONSE_LIMIT_COUNT      10

#define ROTATE_PULSE_PER_DEGREE             142.11167

#define ROTATE_INPOS_LIMIT                  20  // 2도degree
#define ROTATE_INPOS_CHECK_SENSOR           10  // 1도degree
#define ROTATE_POS_180	                  	1800
#define ROTATE_POS_90	                  	900
#define ROTATE_POS_270	                  	2700
#define ROTATE_POS_0	                  	0

#define ROTATE_AXIS_NUM                     4

/**
@class   Rotate
@date
@author
@brief   Rotate 관련 Class
*/
class Rotate
{
private:
	LogUnit *pLogRotate;      ///< Rotate 로그 생성을 위한 변수
    HWCommon *m_pHWCommon;

	Comm_Port *m_pCommPort;
	CRITICAL_SECTION m_CS_CmdExec;
	bool m_bCmdExec;

	int m_nPortNum;

	int position_cts;               // Current Pos (Pulse) 변수
	int position_degree;            // Current Pos (Degree) 변수
	int Velocity_ctsPsec;           // Current Vel 변수
	int torque;					// current 토크
	int m_nError;

	bool bMotorOn;                  // Motor On/Off
	bool bRunning;                  // Motor 구동 중
	bool bOrigin;                   // Origin 유/무
	bool bInit;                     // 초기화 유/무

	//bool bCmdExec;                  // 현재 수행 중 Command 유/무

    bool bReqPause;                 // Motor 상태(Pos, Vel, Status) 요청 중지 변수 (true : Pause 요청 상태)
    bool bReqPosStatus;             // Position 상태 요청 Check 변수 (true : 요청 상태)
    bool bReqVelStatus;             // Velocity 상태 요청 Check 변수 (true : 요청 상태)
    bool bReqMotorStatus;           // Motor 상태 요청 Check 변수 (true : 요청 상태)

    DWORD dwStartReqPosTime;
    DWORD dwStartReqVelTime;
	DWORD dwStartReqStatusTime;

    int nCntIncorrectResponse;      // Motor가 구동 중일 경우에는, 상태 정보가 비정상 적으로 Recive 되는 경우 발생
                                    // 몇 회 이상, 비정상 Recieve 될 경우에만, 에러 발생토록 처리 위한 변수
    int nAbnormalCheckMotorStatus;         // Motor Status가 비정상적으로 Reciece된 경우를 위한 Chattering Count 18.07.09추가

public:
	Rotate(HWCommon *pHWCommon);
	~Rotate();

	int Init();
	int Open();
	void Close();
	int Enable();
	int Disable();
	int SetOrigin();
	int Zeroing();
	int GoHome();
	//int IsCmdExec();
	int Stop();
	//int Move();
    int ResetRotate();
    int Move_Jog_CW();
    int Move_Jog_CCW();
    //int MoveIncremental();
    int MoveIncremental_CW(int nLoc);
    int MoveIncremental_CCW(int nLoc);
	int MoveAbsolute(int nLoc);

private:
	int SendCommand1(AnsiString strSendCommand, AnsiString &strRcvData);
	int SendCommand2(AnsiString strSendCommand, AnsiString &strRcvData);

public:
    int GetCurrentPosition();
    int GetCurrentPositionDegree();
    double GetCurrentVelocity();
    int GetError();
    bool IsAMPEnable();
    bool IsRunning();
	bool IsOrigin();

	bool IsRotate0Sensor();
	bool IsRotate90Sensor();
	bool IsRotate180Sensor();
	bool IsRotate270Sensor();

    //int ReqPosition();
    //int ReadPosition();
    //int RcvReqPosition();
	//
    //int ReqVelocity();
    //int ReadVelocity();
    //int RcvReqVelocity();
	//
    //int ReqMotorStatus();
    //int ReadMotorStatus();


	void ExecuteStep();

private:

	void PurgeComm_s();

	bool ParseCmd(char *buf, int nSize);
	void SetStatusCmd(int nRcvData);
	DStep m_stepRotate;
	int m_nIdxCurCmd;
	int m_nRecvFailCnt;
};

#endif  //RotateH

