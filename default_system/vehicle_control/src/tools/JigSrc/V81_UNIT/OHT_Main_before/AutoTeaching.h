//---------------------------------------------------------------------------

#ifndef AUTOTEACHINGH
#define AUTOTEACHINGH
//---------------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include <winsock2.h>
#include "VirtualModuleControl.h"
#include "VirtualDrivingControl.h"

#include "HWSet.h"
#include "HWCommon.h"
#include "ExecuteInfo.h"
#include "Def_TPCommand.h"

#include "LogHandler.h" // Log 작성 목적

//#include "STD_MapMakerFile.h"


#define MAX_BIG_PACKET_SIZE  8192

#define OT		"Origin Teaching"		// zero position set. reset command. no response exist. ver 1.0 only

#define PT		"Free Teaching"			// Shift만 이동 (Step 추가후 정리할것.)

#define FT			"Focus Teaching"		// move1 -> FT Send
#define FT_EQ		"EFocus Teaching"		// move1 -> FT Send
#define FT_STB		"SFocus Teaching"		// move1 -> FT Send
#define FT_STK		"STKFocus Teaching"		// move1 -> FT Send

#define AT			"Auto Teaching"			// slide(Y), move(X). no (Z)
#define AT_EQ		"EAuto Teaching"			// slide(Y), move(X). no (Z)
#define AT_STB_L		"SLAuto Teaching"			// slide(Y), move(X). no (Z)
#define AT_STB_R		"SRAuto Teaching"			// slide(Y), move(X). no (Z)
#define AT_STK		"STKAuto Teaching"			// slide(Y), move(X). no (Z)

//#define STB_L_SHIFT_DIS 	-455.0
#define STB_L_HOIST_DIS 	108.5

//#define STB_R_SHIFT_DIS 	445.0
#define STB_R_HOIST_DIS 	108.5

#define ZT		"Z-Axis Teaching"
#define ST		"Status"
#define RST		"Reset"
#define BGE		"Begin"
#define END		"End"

#define HOIST_DOWN	0	// 아래 방향으로
#define HOIST_UP	1   // 위 방향으로

#define HOIST_DOWN_H2TP_AT         1 //ORIGIN -> TPOINT
#define HOIST_UP_TP2H_AT           2 //TPOINT -> ORIGIN
#define HOIST_MOVE_TP_AT           3 //TPOINT -> TPOINT_MODIFY
//#define HOIST_UP_TP2H_AT_STB       4 //TPOINT -> ORIGIN

#define SHIFTROTATE_H2TP_AT     	  1 //ORIGIN -> TPOINT
#define SHIFTROTATE_TP2H_AT           2 //TPOINT -> ORIGIN
#define SHIFTROTATE_MOVE_TP_AT        3 //TPOINT -> TPOINT_MODIFY

#define CMD_RETRY_ST        20000 //TPOINT -> TPOINT_MODIFY
#define CMD_RETRY_AT        15000 //TPOINT -> TPOINT_MODIFY
#define CMD_RETRY        	3000  //TPOINT -> TPOINT_MODIFY

#define DEFAULT_OFFSET        		30.0 //Default Offset 거리
#define DEFAULT_OFFSET_EQ        	20.0 //Default Offset 거리
#define TEACHING_OFFSET_HOIST  		10.0 //Default Offset 거리
#define TEACHING_OFFSET_STB_HOIST  	1.0 //Default Offset 거리
#define TEACHING_OFFSET_SHIFT_L  	0.0 //Default Offset 거리
#define TEACHING_OFFSET_SHIFT_R  	0.0 //Default Offset 거리
#define TEACHING_OFFSET_SHIFT_EQ  	0.0 //Default Offset 거리
#define VISION_OFFSET_SHIFT  		0.0 //Default Offset 거리
#define TEACHING_OFFSET_HOIST_AT  20.0 //soming : AUTOTEACHING Hoist Default Offset 거리



typedef enum SIM_RUN_CMD_
{
		CMD_INT 	 = 0,
		CMD_RESPONSE 	,	// 응답이 날아왔음 (다음으로 넘어갈지 판단은 State Machine이 해야 한다)   C
		CMD_READY		,	// Start from Step1
//		CMD_TIMEOUT		,
		// 주의 : 이 부분에 01234 식으로 써 넣으면 컴파일러는 8진수로 해석해 버림!
}SIM_RUN_CMD;

typedef enum RESPONCE_TYPE_
{
		RESPONCE_READY 	 	= 100,
		RESPONCE_BUSY 	 	= 101,
		RESPONCE_ERROR	 	= 109,
		RESPONCE_COMPLETE 	= 110,	// Start from Step1

		STATUS_M_READY  = 200,
		STATUS_M_BUSY   = 201,
		STATUS_V_ERROR  = 208,
		STATUS_O_ERROR  = 209,
}RESPONCE_TYPE;



// 테스트용 클라이언트 (OHT)
#define AUTO_TEACH_MY_SERVER_PORT  1201	// 내가 Listen 할 포트번호
#define AUTO_TEACH_SEND_PORT	   1200	// 상대방이 사용하는 포트번호

typedef struct
{
	char cCommand[256];
} AUTO_TEACH_PACKET;


typedef struct
{
	double dEQ_Shift;
	double dEQ_Hoist;
	double dSTB_Shift;
	double dSTB_Hoist;
	double dSTB_R_Shift_Offset;
	double dSTB_L_Shift_Offset;
	int nSTB_R_Driving_Offset;
	int nSTB_L_Driving_Offset;
	unsigned char AutoTeachingIP[20];
}AUTO_TEACH_OFFSET;

// Auto Teaching 작업 STEP 관련 Define
// --------------------------------------------------------------------------
typedef enum AUTO_TEACHING_STEP_
{


// 주행 초기위치, Shift 초기위치, Hoist 초기위치값은 가지고 있을 것.

	AUTO_TEACHING_READY                  = 0,      //CheckAutoTeachningReady() : 현재 Foup 들고있는지 확인, AutoTeaching Jig와 통신유무 확인 등
												   //각종 명령 초기화 단계.
	AUTO_TEACHING_CHECK_BEGIN				,

	AUTO_TEACHING_MOVE_INIT_SHIFTROTATE       			,     //시작 위치 이동 Check(기본 Map 파일 참조, 상위에서 명령 주기)
	AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_SHIFTROTATE      ,
	AUTO_TEACHING_MOVE_INIT_DOWN_HOIST      			,
	AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_DOWN_HOIST	    ,        // 티칭위치보다 20mm 이전에 이동.


	//1.---------사전 위치 준비 확인 및 이동
//	AUTO_TEACHING_P_TEACHING_CMD			,	// Free Teaching 1회 (FT) ---AT와 통신
//	AUTO_TEACHING_P_TEACHING_WAIT			,
//
//	AUTO_TEACHING_MOVE_FREE_CMD				,   // OHT 이동    --중심로 위치 저장. 다시 홈에서 이동 할경우, 해당 위치로 바로 이동.
//	AUTO_TEACHING_MOVE_FREE_WAIT			,

	//1.---------Focus 확인 및 이동
	AUTO_TEACHING_F_TEACHING_CMD			,	// Focus Teaching 1회 (FT) ---AT와 통신
	AUTO_TEACHING_F_TEACHING_WAIT			,

	AUTO_TEACHING_MOVE_FOCUS_CMD			,   // OHT 이동    --포커스 위치 저장. 다시 홈에서 이동 할경우, 해당 위치로 바로 이동.
	AUTO_TEACHING_MOVE_FOCUS_WAIT			,

///////////////////////////////////////////////////////////
	//2. --------------AT 반복 구간 ---시작-------------------//
///////////////////////////////////////////////////////////
    AUTO_TEACHING_TEACHING_CMD				,   // Auto Teaching 반복 (AT)   ---AT와 통신
	AUTO_TEACHING_TEACHING_WAIT				,	// 티칭 결과값이 넘어오면, 오차범위내에 들어갔는지 확인하고,
												// 범위안에 들어갔으면 AUTO_TEACHING_COMPLETE로 이동한다.

	//STB용
	AUTO_TEACHING_MOVE_UP_HOIST_STB      			,      //Home으로 이동
	AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB		    ,

	//------------주행축 이동 ----------//   Limit 거리 제한 둘것.. 초기 위치로 부터(+- 알파.)
	AUTO_TEACHING_MOVE_X_POS_CMD			,   // OHT 이동   ( 아주 천천히 가감속도 낮게 )
//	AUTO_TEACHING_MOVE_X_POS_WAIT			,

	//Shift 축 이동
	AUTO_TEACHING_MOVE_Y_POS_CMD			,
	AUTO_TEACHING_MOVE_Y_POS_WAIT			,   //이동 후 진동 감쇠기에 의해 수렴시간 동안 대기 이후 반복 수행 할 것.

	AUTO_TEACHING_MOVE_DOWN_HOIST_STB      			,      //Home으로 이동
	AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB		    ,

//	//------------Teaching위치 이동(주행축 이동 없을시, Teaching 위치로 바로 이동 ----//
//    AUTO_TEACHING_MOVE_T_POINT_CMD	    ,
//	AUTO_TEACHING_MOVE_T_POINT_WAIT	        ,
///////////////////////////////////////////////////////////
///---------------AT 반복 구간 ---끝------------------//
///////////////////////////////////////////////////////////

	//3. ---------Hoist 축 이동
    AUTO_TEACHING_Z_TEACHING_CMD			,	// Z축 티칭은 한번만 하고 넘어간다   ---AT와 통신
	AUTO_TEACHING_Z_TEACHING_WAIT			,

		//STB용
	AUTO_TEACHING_MOVE_UP_HOIST_STB_Z      			,      //Home으로 이동
	AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB_Z		    ,

		//Shift 축 이동
	AUTO_TEACHING_MOVE_Y_POS_CMD_Z			,
	AUTO_TEACHING_MOVE_Y_POS_WAIT_Z			,   //이동 후 진동 감쇠기에 의해 수렴시간 동안 대기 이후 반복 수행 할 것.

	AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z      			,      //Home으로 이동
//	AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB_Z		    ,

//	AUTO_TEACHING_HOIST_MOVE_CMD			,      // OHT 이동
//	AUTO_TEACHING_HOIST_MOVE_WAIT			,

	AUTO_TEACHING_CHECK_PUSH_HAND			,

	AUTO_TEACHING_SAVE_CURRENT_POS_CMD		,



	AUTO_TEACHING_CHECK_HAND_CLOSE_OPEN			   	, //soming :Hoist 위아래 seq 동작 전, Hand OPEN 동작

	AUTO_TEACHING_MOVE_COMP_UP_HOIST     			,      //Home으로 이동
	AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST	    ,
   /*
	AUTO_TEACHING_U_MOVE_DOWN_HOIST_FAST             ,     //soming 10단계
	AUTO_TEACHING_U_CHECK_MOVE_DOWN_HOIST_FAST     ,
	AUTO_TEACHING_U_MOVE_DOWN_HOIST_SLOW          ,
	AUTO_TEACHING_U_CHECK_MOVE_DOWN_HOIST_SLOW   ,
	AUTO_TEACHING_U_CHECK_ARRIVE_TEACHING_POINT  ,
	AUTO_TEACHING_U_GRIP_FOUP                    ,
	AUTO_TEACHING_U_MOVE_UP_HOIST_SLOW           ,
	AUTO_TEACHING_U_CHECK_MOVE_UP_HOIST_SLOW     ,
	AUTO_TEACHING_U_MOVE_UP_HOIST_FAST           ,
	AUTO_TEACHING_U_CHECK_MOVE_UP_HOIST_FAST     ,
    */
	AUTO_TEACHING_MOVE_COMP_HOME_SHIFTROTATE       			,
	AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_HOME_SHIFTROTATE      ,
	AUTO_TEACHING_CHECK_ANTIDROP			,

	AUTO_TEACHING_COMPLETE					,

} AUTO_TEACHING_STEP;


// Auto Teaching 작업 STEP 관련 Define
// --------------------------------------------------------------------------
typedef enum AUTO_TEACHING_NULL_TYPE_STEP_
{


// 주행 초기위치, Shift 초기위치, Hoist 초기위치값은 가지고 있을 것.

	N_AUTO_TEACHING_READY                  = 0,      //CheckAutoTeachningReady() : 현재 Foup 들고있는지 확인, AutoTeaching Jig와 통신유무 확인 등
												   //각종 명령 초기화 단계.
	N_AUTO_TEACHING_CHECK_BEGIN				,

	N_AUTO_TEACHING_MOVE_INIT_SHIFTROTATE       			,     //시작 위치 이동 Check(기본 Map 파일 참조, 상위에서 명령 주기)
	N_AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_SHIFTROTATE      ,
	N_AUTO_TEACHING_MOVE_INIT_DOWN_HOIST      			,
	N_AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_DOWN_HOIST	    ,        // 티칭위치보다 20mm 이전에 이동.


	//1.---------사전 위치 준비 확인 및 이동
//	AUTO_TEACHING_P_TEACHING_CMD			,	// Free Teaching 1회 (FT) ---AT와 통신
//	AUTO_TEACHING_P_TEACHING_WAIT			,
//
//	AUTO_TEACHING_MOVE_FREE_CMD				,   // OHT 이동    --중심로 위치 저장. 다시 홈에서 이동 할경우, 해당 위치로 바로 이동.
//	AUTO_TEACHING_MOVE_FREE_WAIT			,

	//1.---------Focus 확인 및 이동
	N_AUTO_TEACHING_F_TEACHING_CMD			,	// Focus Teaching 1회 (FT) ---AT와 통신
	N_AUTO_TEACHING_F_TEACHING_WAIT			,

	N_AUTO_TEACHING_MOVE_FOCUS_CMD			,   // OHT 이동    --포커스 위치 저장. 다시 홈에서 이동 할경우, 해당 위치로 바로 이동.
	N_AUTO_TEACHING_MOVE_FOCUS_WAIT			,

///////////////////////////////////////////////////////////
	//2. --------------AT 반복 구간 ---시작-------------------//
///////////////////////////////////////////////////////////
	N_AUTO_TEACHING_TEACHING_CMD				,   // Auto Teaching 반복 (AT)   ---AT와 통신
	N_AUTO_TEACHING_TEACHING_WAIT				,	// 티칭 결과값이 넘어오면, 오차범위내에 들어갔는지 확인하고,
												// 범위안에 들어갔으면 AUTO_TEACHING_COMPLETE로 이동한다.

	//STB용
	N_AUTO_TEACHING_MOVE_UP_HOIST_STB      			,      //Home으로 이동
	N_AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB		    ,

	//------------주행축 이동 ----------//   Limit 거리 제한 둘것.. 초기 위치로 부터(+- 알파.)
	N_AUTO_TEACHING_MOVE_X_POS_CMD			,   // OHT 이동   ( 아주 천천히 가감속도 낮게 )
//	AUTO_TEACHING_MOVE_X_POS_WAIT			,

	//Shift 축 이동
	N_AUTO_TEACHING_MOVE_Y_POS_CMD			,
	N_AUTO_TEACHING_MOVE_Y_POS_WAIT			,   //이동 후 진동 감쇠기에 의해 수렴시간 동안 대기 이후 반복 수행 할 것.

	N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB      			,      //Home으로 이동
	N_AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB		    ,

//	//------------Teaching위치 이동(주행축 이동 없을시, Teaching 위치로 바로 이동 ----//
//    AUTO_TEACHING_MOVE_T_POINT_CMD	    ,
//	AUTO_TEACHING_MOVE_T_POINT_WAIT	        ,
///////////////////////////////////////////////////////////
///---------------AT 반복 구간 ---끝------------------//
///////////////////////////////////////////////////////////

	//3. ---------Hoist 축 이동
	N_AUTO_TEACHING_Z_TEACHING_CMD			,	// Z축 티칭은 한번만 하고 넘어간다   ---AT와 통신
	N_AUTO_TEACHING_Z_TEACHING_WAIT			,

		//STB용
	N_AUTO_TEACHING_MOVE_UP_HOIST_STB_Z      			,      //Home으로 이동
	N_AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB_Z		    ,

		//Shift 축 이동
	N_AUTO_TEACHING_MOVE_Y_POS_CMD_Z			,
	N_AUTO_TEACHING_MOVE_Y_POS_WAIT_Z			,   //이동 후 진동 감쇠기에 의해 수렴시간 동안 대기 이후 반복 수행 할 것.

	N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z      			,      //Home으로 이동
//	AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB_Z		    ,

//	AUTO_TEACHING_HOIST_MOVE_CMD			,      // OHT 이동
//	AUTO_TEACHING_HOIST_MOVE_WAIT			,

	N_AUTO_TEACHING_CHECK_PUSH_HAND			,

	N_AUTO_TEACHING_SAVE_CURRENT_POS_CMD		,



	N_AUTO_TEACHING_CHECK_HAND_CLOSE_OPEN			   	, //soming :Hoist 위아래 seq 동작 전, Hand OPEN 동작

	N_AUTO_TEACHING_MOVE_COMP_UP_HOIST     			,      //Home으로 이동
	N_AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST	    ,
   /*
	AUTO_TEACHING_U_MOVE_DOWN_HOIST_FAST             ,     //soming 10단계
	AUTO_TEACHING_U_CHECK_MOVE_DOWN_HOIST_FAST     ,
	AUTO_TEACHING_U_MOVE_DOWN_HOIST_SLOW          ,
	AUTO_TEACHING_U_CHECK_MOVE_DOWN_HOIST_SLOW   ,
	AUTO_TEACHING_U_CHECK_ARRIVE_TEACHING_POINT  ,
	AUTO_TEACHING_U_GRIP_FOUP                    ,
	AUTO_TEACHING_U_MOVE_UP_HOIST_SLOW           ,
	AUTO_TEACHING_U_CHECK_MOVE_UP_HOIST_SLOW     ,
	AUTO_TEACHING_U_MOVE_UP_HOIST_FAST           ,
	AUTO_TEACHING_U_CHECK_MOVE_UP_HOIST_FAST     ,
    */
	N_AUTO_TEACHING_MOVE_COMP_HOME_SHIFTROTATE       			,
	N_AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_HOME_SHIFTROTATE      ,
	N_AUTO_TEACHING_CHECK_ANTIDROP			,

	N_AUTO_TEACHING_COMPLETE					,

} AUTO_TEACHING_NULL_TYPE_STEP;



/*
  : TCP or UDP 통신 부분 추가
  : 주행이동관련 부분 추가 .
  : 프로토콜 관련된 부분 확인.
*/


class AutoTeaching
{
private:
	HWSet 					*m_pHWSet;
    HWCommon 				*m_pHWCommon;
	VirtualDrivingControl	*m_pVirtualDrivingControl;

	EXECUTE_INFO *m_pExecuteInfo;


    LogUnit *pLogAutoTeaching; ///< Map Make STD관련 로그 변수


    AUTO_TEACH_OFFSET defualtOffset;
	DWORD dwAutoTeachingResponseTime;
	//--------Jig와 통신부분
	SOCKET ServerSock;
	bool bListenMode;
	char cATIpAddr[20];

	int SocketInit(void);
	void SocketClose();


	int UnicastSendMessage(char* cIpAddr, int iPort, char* buf, int len);
	int ServerListen(char* buf, int& iLen, int& iClientPort);

public:

	AutoTeaching();
	~AutoTeaching();


	//임시 정의
	int iCmd;

	double fHoist;
	double fSlide;
	double fDrive;


	double dHoist_Offset;
	double fSlide_Offset;
	double fDrive_Offset;
	double Total_fDrive_Offset;


	double dSTB_R_Offset;
	double dSTB_L_Offset;

	bool bSTBTeaching;
	int nResult;
	/////////
	///------ Jig와 통신 부분
	int ServerBind(int iPort, char* cIpAddr);
	int ServerClose();
	bool IsConnect(); //통신 연결햇는지 확인.

	bool IsCheckIPCompare(char *strIP); //통신 연결햇는지 확인.

	void SetAutoTeachingJigResponse();
	DWORD GetAutoTeachingJigResponse();


	//// ----Auto Teaching 수행 부분 --통신& Data 확인
	int AutoTeaching_SendCommand(char* Buf);  //FT, AT 등
	int UpdateATData();  //통신 연결시 쓰레드에서 감시할것.

	int CheckSTBShiftLimitError(int nPortType, double Shift);
	int CheckSTBHoistLimitError(int nPortType, double Hoist);

	int ATSave_Current_Pos(UINT CurStationID, BYTE PortType, double dDTeaching, double dHTeacing, double dSTeacing);

	int ATSave_NullType_Data(UINT CurStationID, BYTE PortType, double dDTeaching, double dHTeacing, double dSTeacing,char* cATAddr);

	int Save_AutoTeaching_DrvingData(UINT CurStationID, BYTE PortType, int dDriving);
	int Load_AutoTeaching_Param();

	int Save_AutoTeaching_Param_NullType(BYTE PortType);

	//// ----AUto Teaching 수행 부분 --이동명령
	int ATDrivingMove(BYTE PortType);
	int ATDrivingMoveCheck(bool &bCheck);

	void QRDrvingTrigger(BYTE PortType);
	bool QRDataCheck(BYTE PortType, int nTargetID);
	
};


#endif

