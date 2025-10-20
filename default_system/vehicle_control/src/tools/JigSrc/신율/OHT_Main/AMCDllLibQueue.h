// ---------------------------------------------------------------------------
/**
@file    AMCDllLibQueue.h
@version OHT 7.0
@brief   AMCDllLibQueue Class Header File
*/

#ifndef AMCDllLibQueueH
#define AMCDllLibQueueH

#include <windows.h>
#include "ErrorCode.h"
#include "Def_MotionControler.h"
#include "AMC_Protocol.h"
#include "List.h"

#include "LogHandler.h"	// Log 작성 목적

// Additional Protocol for AMCDllLib.cpp
#if(USE_CHANGENODE == 1)
typedef struct _CMD_DATA_UNION
{
	int	Output[4];		///< Output 정보		(SetOutBit)
	CMD_AXIS	CmdAxis;		///< 축 명령 정보		(축 이동 명령)
	DETECT_INFO	DetectInfo;	///< 전방 감지 Type 설정을 위한 정보
	PAUSE_INFO PauseInfo;
	OPTIONAL_NODE_INFO OptNodeInfo;
//	int	DecelLimitOfDriving;	///< 주행 감속 Limit 설정가속도(Cts/s2)
}CMD_DATA_UNION;
#else
typedef union _CMD_DATA_UNION
{
	int	Output[4];		///< Output 정보		(SetOutBit)
	CMD_AXIS	CmdAxis;		///< 축 명령 정보		(축 이동 명령)
	DETECT_INFO	DetectInfo;	///< 전방 감지 Type 설정을 위한 정보
	PAUSE_INFO PauseInfo;
//	int	DecelLimitOfDriving;	///< 주행 감속 Limit 설정가속도(Cts/s2)
}CMD_DATA_UNION;
#endif


// Additional Protocol for AMCDllLib.cpp
typedef struct _QUEUEING_AMC_CMD
{
	CMD_ANALYSIS_UNION	CmdAnalysisUnion;	///< Union의 Type를 확인하는 함수
	CMD_DATA_UNION		DataUnion;			///< 명령의 공용체 --> AMCDllLib 내부에서 Buffer의 메모리 절약을 목적으로 제작됨

#if((OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V86))  //timy v8.2 8.6용  8.1버전 컴파일할때는 이부분은 주석처리
	int m_nSound;
	int m_nReserved_cmd1;
	int m_nReserved_cmd2;
#else
#endif

}QUEUEING_AMC_CMD;


//-----------------------------------------------------------
/**
 @class   AMCDllLibQueue
 @date    2012.10.25
 @author  ehoto
 @brief   AMCDllLibQueue 관련 Class
 @note    AMC 주기 통신 정보를 저장하는 Queue
 */
class AMCDllLibQueue
{
public:
	AMCDllLibQueue();	///< 생성자
	~AMCDllLibQueue();	///< 소멸자

	bool init(void);	///<List를 비우고 Count를 초기화
	bool putData(QUEUEING_AMC_CMD QueuingAmcCmd, int *nError);		///< User가 Buffer에 정보를 입력할 때 사용
	bool makeData(AMC_CMD* pAmcCmd, int *nError);	///< Buffer 정보를 기준으로 AMC로 보낼 명령을 만들 때 사용
	int GetReportError();
	
private:
	CRITICAL_SECTION	m_CS;		///< Buffer의 Count를 읽고 쓸 때
	LList	m_LList;	///< 명령을 저장하는 List
//	int 	m_nCount;	///< List에 들어가 있는 명령의 개수
	int	m_nError;	///< 에러: 내부 에러 상황
	int	m_nReportError;	///< 에러: 내부 에러 상황

};
// ---------------------------------------------------------------------------
#endif
