#ifndef AMC_ProtocolH
#define AMC_ProtocolH

#include <stdio.h>
#include "Def_MotionControler.h"
#include "../crc16.h"

#define USE_CRC_CHECK

// -------------------------------------------
//1	Receive Data
// -------------------------------------------

// AMC Data To Get
typedef struct _AMC_DATA
{
	// Input/Output Data
	volatile int	Input[4];						///< InPut 정보
	volatile int	Output[4];					///< Output 정보

	// Axis Data
	volatile int	Source[4];					///< 축별 source
	volatile int	State[4];						///< 축별 state
	volatile char	Running[4];					///< 축별 움직이는 중 확인
	volatile char	InPosition[4];				///< 축별 도착 완료 확인
	volatile char	AmpEnable[4];					///< 축별 동작 대기 상태 확인(Servo On, ~~)
	volatile int	Position[4];					///< 축별 위치 (Cts)
	volatile int	Velocity[4];					///< 축별 속도 (Cts/sec)
	volatile int	FollowingError[4];			///< 축별 에러 (Cts)
	volatile int	PositionOfSmallAddCheck[4];	///< Small Add 실행 시 센서 감지 위치값 ( 미감지,미사용 시 0.0 / 감지 위치가 0일 경우 1로 받음 )  (Cts)

	// AMC Log
	volatile int	TargetVel[4];					///< Sample 당 속도 Reffernce
	volatile int	TargetPos[4];					///< Sample 당 위치
	volatile int	FinalPos[4];					///< 종착지 위치

	// 2018.07.26
	// AxisFlag1[1] 이 CRC플래그로 사업부에서 지정함
	volatile int    AxisFlag1[4];

	volatile int	AxisFlag2[4];
#if 1	// 전압 추가 시 AMC 이상 현상으로 우선 제거
	volatile int	VolatageMonitor[2];
#endif	
	volatile int 	PauseState;
	NODE_CHECK_DATA NodeCheckData;				///< OHTv7 을 위한 추가 정보

#ifdef USE_CRC_CHECK	// 사업부에서 CRC 대응 될때 ifdef 해제 예정 ********************


	// 현재 구조체의 CRC 값 구해서 Crc16 에 저장
	void MakeCRC16()
	{
		int *pnCrc16 = (int*)&AxisFlag1[1];    // CRC 선정값
		*pnCrc16 = 0;  // CRC 값 초기화

		// CRC값 초기화 된 상태에서 crc 값 구함
		*pnCrc16 = crc16_ccitt((const char*)this, sizeof(*this));
	}

	// CRC 체크
	// 현재 구조체의 Crc16 값과, 계산된 CRC값을 비교하여 Validation Check
	bool CrcCheck()
	{
		int *pnCrc16 = (int*)&AxisFlag1[1];   // CRC 선정값
		int OrgVal = *pnCrc16;	// CRC 받은 값 저장
		*pnCrc16 = 0;   // CRC 값 초기화

		// CRC값 초기화 상태의 crc값 구함
		unsigned short CalcVal = crc16_ccitt((const char*)this, sizeof(*this));
		*pnCrc16 = OrgVal;		// CRC 값 원복

		if(OrgVal != CalcVal)    // 값이 다르면 error
			return false;

		return true;
	}

	#endif
	
}AMC_DATA;
// -------------------------------------------


// -------------------------------------------
//1	Send Data
// -------------------------------------------

#if(USE_CHANGENODE == 1)
// AMC CMD Structure
typedef struct _AMC_CMD
{
	volatile int	Cmd;			///< bit 단위 명령 : CMD_TYPE bitfield 참조
	volatile int	Output[4];	///< Output 정보
	CMD_AXIS		CmdAxis[4];	///< 축 명령 정보
	DETECT_INFO		DetectInfo;	///< 전방 감지 Type 설정을 위한 정보
	PAUSE_INFO		PauseInfo;

	OPTIONAL_NODE_INFO OptNodeInfo;

	#ifdef USE_CRC_CHECK	// 사업부에서 CRC 대응 될때 ifdef 해제 예정 ********************

	volatile int 	Crc16;	///< CRC Check

	// 현재 구조체의 CRC 값 구해서 Crc16 에 저장
	void MakeCRC16()
	{
		Crc16 = 0;  // CRC 값 초기화

		// CRC값 초기화 된 상태에서 crc 값 구함
		Crc16 = crc16_ccitt((const char*)this, sizeof(*this));
	}

	// CRC 체크
	// 현재 구조체의 Crc16 값과, 계산된 CRC값을 비교하여 Validation Check
	bool CrcCheck()
	{
		int OrgVal = Crc16;	// CRC 값은 값은 저장
		Crc16 = 0;   // CRC 값 초기화

		// CRC값 초기화 상태의 crc값 구함
		int CalcVal = crc16_ccitt((const char*)this, sizeof(*this));
		Crc16 = OrgVal;		// CRC 값 원복
		if(OrgVal != CalcVal)    // 값이 다르면 error
			return false;

		return true;
	}
	#endif

#if((OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V86))  //timy v8.2 8.6용  8.1버전 컴파일할때는 이부분은 주석처리
	int m_nSound;
	int m_nReserved_cmd1;
	int m_nReserved_cmd2;
#else
#endif

}AMC_CMD;
#else
typedef struct _AMC_CMD
{
	volatile int	Cmd;			///< bit 단위 명령 : CMD_TYPE bitfield 참조
	volatile int	Output[4];	///< Output 정보
	CMD_AXIS		CmdAxis[4];	///< 축 명령 정보
	DETECT_INFO		DetectInfo;	///< 전방 감지 Type 설정을 위한 정보
	PAUSE_INFO		PauseInfo;

	#ifdef USE_CRC_CHECK	// 사업부에서 CRC 대응 될때 ifdef 해제 예정 ********************

	volatile int 	Crc16;	///< CRC Check

	// 현재 구조체의 CRC 값 구해서 Crc16 에 저장
	void MakeCRC16()
	{
		Crc16 = 0;  // CRC 값 초기화

		// CRC값 초기화 된 상태에서 crc 값 구함
		Crc16 = crc16_ccitt((const char*)this, sizeof(*this));
	}

	// CRC 체크
	// 현재 구조체의 Crc16 값과, 계산된 CRC값을 비교하여 Validation Check
	bool CrcCheck()
	{
		int OrgVal = Crc16;	// CRC 값은 값은 저장
		Crc16 = 0;   // CRC 값 초기화

		// CRC값 초기화 상태의 crc값 구함
		int CalcVal = crc16_ccitt((const char*)this, sizeof(*this));
		Crc16 = OrgVal;		// CRC 값 원복
		if(OrgVal != CalcVal)    // 값이 다르면 error
			return false;

		return true;
	}
	#endif

#if((OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V86))  //timy v8.2 8.6용  8.1버전 컴파일할때는 이부분은 주석처리
	int m_nSound;
	int m_nReserved_cmd1;
	int m_nReserved_cmd2;
#else
#endif

}AMC_CMD;
#endif

#endif
