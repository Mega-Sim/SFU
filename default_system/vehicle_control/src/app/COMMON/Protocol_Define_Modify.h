#ifndef Protocol_Define_ModifyH
#define Protocol_Define_ModifyH

#include <vcl.h>
#include <windows.h>
#include <stdio.h>
//#include <math.h>
//#include "PUtily.h"
#include "Def_DataType.h"

//#include "Def_DataType.h"

// ---------------------------------------------------------------------------
//  이 부분에 각 프로그램이 사용하는 프로토콜 버전을 써 넣을 것
//  버전번호는 1부터 증가하는 숫자로 한다
#define OHT_REAL_VER		1	// Ver 1.0.0
#define OHT_EMUL_VER		1	// Ver 1.0.0
#define OCS_EMUL_VER		1	// Ver 1.0.0
#define NOTE_TP_VER			1	// Ver 1.0.0
#define ASSISTANCE_VER 		1	// Ver 1.0.0
#define FILEMANAGER_VER 	1	// Ver 1.0.0

// 통신모듈의 프로토콜 지원버전 (max) 명시.
#define SUPPORT_PROTOCOL_VER 1  // Ver 1.0.0 (2016.12.01)

// 적용하고 싶은 프로토콜 버전 명시
#define APPLY_PROTOCOL_VER 1  // Ver 1.0.0 (2016.12.01)


// 프로토콜 이력은 아래에 간단하게 기술
/*
	2016.12.01 (0) : 버전 zero. 통신모듈 버전확인 기능 테스트용
	2016.12.01 (1) : 버전 1.0.0 정의

*/
// ---------------------------------------------------------------------------

// LOG Save Defines
#define RAW_LOG_BYTE_1_LINE_SIZE	100	// RAW 로그1라인당 저장할 바이트 수
#define RAW_LOG_ASCII_LOW_NUM		32	// RAW 로그 저장시 아스키 범위 지정 (이것보다 커야 아스키로 생각)
#define RAW_LOG_ASCII_HIGH_NUM		126	// RAW 로그 저장시 아스키 범위 지정 (이것보다 작아야 아스키로 생각)

// 네트워크가 안되어 임의로 IP를 고정하는 설정
// 사용하고자 하면 이 부분을 수정하고, 프로젝트 설정에 _K_DEMO 를 넣어줄 것
// [OCS - OHT]
#define K_DEMO_VHL_ADDR 		"127.0.0.1"				// OCS에서 사용함. OHT의 주소를 의미
#define K_DEMO_VHL_MACHINE_ID	"OHT001"				// OCS에서 사용함. OHT의 이름을 의미

#define K_DEMO_OCS_ADDR 		"127.0.0.1"				// OHT에서 사용함. OCS의 주소를 의미
#define K_DEMO_OCS_MACHINE_ID	"OCS001"				// OHT에서 사용함. OCS의 이름을 의미

#define K_DEMO_OCS_ADDR_2 		"192.168.200.200"		// OHT에서 사용함. OCS의 주소를 의미
#define K_DEMO_OCS_MACHINE_ID_2	"OCS002"				// OHT에서 사용함. OCS의 이름을 의미

// [OHT - DIAG]
#define K_DEMO_DIAG_SVR_ADDR 	"192.168.100.100"		// OHT에서 사용함. 진단서버의 주소를 의미
#define K_DEMO_DIAG_SVR_MACHINE_ID	"SDG-01"		    // OHT에서 사용함. 진단서버의 주소를 의미

#define K_DEMO_DIAG_VHL_ADDR 		"192.168.100.100"	// 진단서버에서 사용함. OHT의 주소를 의미
#define K_DEMO_DIAG_VHL_MACHINE_ID	"OHT001"			// 진단서버에서 사용함. OHT의 이름을 의미

// [OCS - MTL]
#define K_DEMO_MTL_DOWN_ADDR 		"192.168.100.1"		// OCS에서 사용함. MTL(DOWN)의 주소를 의미
#define K_DEMO_MTL_DOWN_MACHINE_ID	"MTL-01"			// OCS에서 사용함. MTL(DOWN)의 이름을 의미

#define K_DEMO_MTL_UPPER_ADDR 		"192.168.100.1"		// OCS에서 사용함. MTL(UPPER)의 주소를 의미
#define K_DEMO_MTL_UPPER_MACHINE_ID	"MTL-02"			// OCS에서 사용함. MTL(UPPER)의 이름을 의미

#define K_DEMO_MTL_OCS_ADDR 		"192.168.100.100"	// MTL에서 사용함. OCS의 주소를 의미
#define K_DEMO_MTL_OCS_MACHINE_ID	"OCS001"			// MTL에서 사용함. OCS의 이름을 의미

// ---------------------------------------------------------------------------
// Default define 값 정의
#define MAX_PACKET_SIZE	1024		// 패킷 최대크기.
#define MAX_BIG_PACKET_SIZE	8192

#define MAX_PATH_SIZE    400        //puting Path Max
#define BROADCAST_IP   "192.167.255.255"//	"255.255.255.255"	// default       20161112 네트워크 브로드케이스 변경
//#define BROADCAST_IP	"192.168.100.8"	// ocs_test_ver

#define MACHINE_ID_LENGTH	6

// Ocs 보고 용 VHL 기종 구분
#define MACHINETYPE_VHL_COMMON   12851      // 0x3233, ASCII '23'
//#define MACHINETYPE_VHL_RATICLE  12852		// 0x3234, ASCII '24'
#define MACHINETYPE_VHL_PATROL   12853      // 0x3235, ASCII '25'

// [OCS - OHT]
#define MACHINETYPE_MCP			12336		// 0x3030, ASCII '00'


#define MACHINETYPE_VHL 		MACHINETYPE_VHL_COMMON


#define MACHINETYPE_TP			   02
#define MACHINETYPE_HANDYTP        03
#define MACHINETYPE_MAPVIEW		   04
#define MACHINETYPE_VMS   		   05   // VMS
#define MACHINETYPE_BLACKBOX   	   06

// [OHT - DIAG]
#define MACHINETYPE_DIAG_SVR		0x0000		// AGENT 진단서버 식별 ID (사양서 4.2참고)
#define MACHINETYPE_DIAG_NORMAL_OHT 0x0101		// SEMES 일반 OHT 식별 ID
#define MACHINETYPE_DIAG_CLEAN_OHT  0x0102		// SEMES 청소 OHT 식별 ID

// [OCS - MTL]
#define MACHINETYPE_MTL				0x3630		// MTL의 기종 ID (ASCII '60')

// [Assistance - FileManager]
#define MACHINETYPE_AS				0x5000
#define MACHINETYPE_FM				0x5001

#define ASCII_SPACE			0x20	// blank (DEC 32)

#define TRANSTYPE_FLOORMOVE		0x06	///< Trans Type : Floor movement
#define TRANSTYPE_MTLMOVE		0x07	///< Trans Type : MTL movement

// ########################## 주소 획득 관련 정의 ###########################

// [OCS - OHT]
#define ADDRESS_BROADCAST_DEF		48 // 0		// 주소통지 B/C일때 패킷의 identify에 들어갈 값
#define ADDRESS_UNICAST_REPLY		49 // 1
#define ADDRESS_EXPIRE_ORDER		50 // 2
#define ADDRESS_EXPIRE_ALL			51 // 4

// [OHT - DIAG]
#define ADDRESS_DIAG_BROADCAST_DEF		0x0000	// 주소통지 B/C일때 패킷의 identify에 들어갈 값
#define ADDRESS_DIAG_UNICAST_REPLY		0x0001  // 1


// [OCS - OHT]
// 2016-12-14, Add by Labeler
// ROHT의 경우, 임시로 7.5 Test Bed에서 돌리기 위해 변경
#define ADDRESS_GET_DEFAULT_PORT	9000// 9000      //Port 구분
#define DEFAULT_BROADCAST_TIMEOUT   1000	// ms단위.

#define CONTROL_DEFAULT_PORT		(ADDRESS_GET_DEFAULT_PORT + 1)
#define DEFAULT_CONTROL_TIMEOUT   	1000	// ms단위
#define DEFAULT_STATUS_AUTOSEND_TIME  10	// 초단위

#define MAX_OCS_NUMBER				2	// 이중화를 위해 OCS를 몇개까지 동시접속할 것인가를 결정

// [OHT - DIAG]
#define ADDRESS_DIAG_GET_DEFAULT_PORT	9620
#define CONTROL_DIAG_DEFAULT_PORT		9621 //(ADDRESS_DIAG_GET_DEFAULT_PORT + 1)



// ---------- 브로드캐스트 Send결과 ----------
#define BROADCAST_SEND_RESULT_SUCCESS	1
#define BROADCAST_SEND_RESULT_FAIL		0

// ---------- 브로드캐스트 Listen 결과 ----------
// 주의 : 패킷의 수신함수에서는 패킷의 수신이 성공하면, 수신한 패킷의 바이트 수를
//         리턴한다. 따라서, 실패의 원인은 항상 마이너스로 정의하여야 한다
#define BROADCAST_LISTEN_RESULT_SUCCESS					1
#define BROADCAST_LISTEN_RESULT_PACKET_SIZE_INVALID		-1
#define BROADCAST_LISTEN_RESULT_PACKET_CONTENT_INVALID 	-2
#define BROADCAST_LISTEN_RESULT_PACKET_NOT_FOUND		-3
#define BROADCAST_LISTEN_RESULT_OTHER_OHT_S_BROADCAST	-4
#define BROADCAST_LISTEN_RESULT_OTHER_OCS_S_BROADCAST   -5

// ---------- 유니캐스트 Send결과 ----------
#define UNICAST_SEND_RESULT_SUCCESS		1
#define UNICAST_SEND_RESULT_FAIL		0
#define UNICAST_SEND_ADDRESS_NOT_RESISTED	-1	// 주소획득 절차를 거치지 않아, 현재 알고있는 주소가 없음

// ---------- 유니캐스트 Listen 결과 ----------
// 주의 : 패킷의 수신함수에서는 패킷의 수신이 성공하면, 수신한 패킷의 바이트 수를
//        리턴한다. 따라서, 실패의 원인은 항상 마이너스로 정의하여야 한다
// #define UNICAST_LISTEN_RESULT_SUCCESS					1	// 유니케스트 Listen시, 성공하면 수신한 Byte수를 리턴하므로, 이 정의는 불필요 (삭제)
#define UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID		-1
#define UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID 	-2
#define UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND		-3
#define UNICAST_LISTEN_RESULT_OTHER_OHT_S_UNICAST	-4
#define UNICAST_LISTEN_RESULT_OTHER_OCS_S_UNICAST   -5
#define TP_ADDRESS_REGISTED							-6
#define TP_ADDRESS_DELETED							-7

// ---------- PC/TP define ------------
#define TP_FIX_PORT_NUM				9031
// #define TP_FIX_PORT_NUM				9031	// PC TP가 사용하는 포트번호 (고정), 진단서버와 겹쳐서 기존 9020에서 9031로 수정함
#define OHT_S_TP_LISTEN_PORT_NUM	9030	// OHT가 TP의 패킷을 받기위해 사용하는 포트번호 (고정)

// ---------- PC ~ ASSISTANCE define ----------
#define OHT_AS__OHT_PORT_NUM	9035	// OHT와 어시스턴스 간 접속에 사용하는 포트 (OHT의 수신포트)
#define OHT_AS__AS_PORT_NUM		9036	// OHT와 어시스턴스 간 접속에 사용하는 포트 (어시스턴스의 수신포트)
#define OHT_AS__LOCAL_IP		"127.0.0.1"

// 22.05.04 JUKim added
// ---------- PC ~ AGENT define ----------
#define OHT_AGENT__OHT_PORT_NUM	        9073	// OHT와 Agent 간 접속에 사용하는 포트 (OHT의 수신포트)
#define OHT_AGENT__AGENT_PORT_NUM		9074	// OHT와 Agent 간 접속에 사용하는 포트 (Agent의 수신포트)
#define OHT_AGENT__LOCAL_IP		"127.0.0.1"

//AS 포함시
#if 1
// ---------- ASSISTANCE ~ FILE Manager define ----------
#define AS_2_FM_PORT			9037	// 어시스턴스에서 보낸 메시지를 파일매니저가 수신하는 포트
//#define FM_2_AS_PORT			9038    // 파일매니저가 어시스턴스에 보낸 메시지를  수신하는 포트
#define FM_2_AS_PORT			9036    // 파일매니저가 어시스턴스에 보낸 메시지를  수신하는 포트
#else//AS 제외시
// ---------- ASSISTANCE ~ FILE Manager define ----------
#define AS_2_FM_PORT			9036	// OHT에서 보낸 메시지를 파일매니저가 수신하는 포트
//#define FM_2_AS_PORT			9038    // 파일매니저가 어시스턴스에 보낸 메시지를  수신하는 포트
#define FM_2_AS_PORT			9035    // 파일매니저가 OHT에 보낸 메시지를  수신하는 포트

#endif

#define TP_FIX_PORT_NUM_FOR_MAPVIEW	9040
// ########################## 기본 상수정의  ###########################
// 2진수 연산용 define
#define	bit_0 0x01
#define bit_1 0x02
#define bit_2 0x04
#define bit_3 0x08
#define bit_4 0x10
#define bit_5 0x20
#define bit_6 0x40
#define bit_7 0x80

// ------- 3바이트 연산을 위한 정의 (혹시 H-M-L순서가 뒤바뀐다면.. 이걸로 바로 변경가능....
//         즉, 3바이트가 있는데 H-M-L 인줄 알았는데 반대라면 이걸로 슥삭 변경가능
#define HIGH_BYTE 0
#define MID_BYTE  1
#define LOW_BYTE  2

// ------- 4Byte Order
#define FOUR_BYTE_HIGH  0
#define FOUR_BYTE_M1	1
#define FOUR_BYTE_M2	2
#define FOUR_BYTE_LOW	3

#define INIT  		0x00
#define FAIL  		0x01
#define SUCCESS  	0x02
#define REQ_INSERT 	0x02
#define COMPLETE  	0x03
#define REPORT      0x04

// ---------- FTP define ------------
#define FTP_MODE_OFF	(0)
#define FTP_MODE_ON		(1)
#define FTP_WIRELESS_UPDATE	(2)

// ---------- 데이터 전송 지시 응답 ------------
#define FTP_STATUS_NOP_DONE			(0x00) //전송처리 이전 or 정상 완료
#define FTP_STATUS_COMMUNICATION	(0x01) //FTP 서버와 통신 중
#define FTP_STATUS_FILE_CHECK		(0x02) //파일의 체크 처리 중
#define FTP_STATUS_FILE_BACKUP		(0x03) //파일 보존 처리 중

#define FTP_STEP_START			    (0x00) //전송처리 시작
#define FTP_STEP_FTP_PATH_CHECK	    (0x01) //FTP 디렉토리 경로 유무 검사 단계
#define FTP_STEP_FTP_FILE_CHECK	    (0x02) //FTP 다운로드시 파일 유무 검사 단계
#define FTP_STEP_COMMUNICATION	    (0x03) //FTP 서버와 통신 중
#define FTP_STEP_FILE_CHECK		    (0x04) //파일의 검사  처리 중
#define FTP_STEP_FILE_BACKUP	    (0x05) //파일 보존 처리 중
#define FTP_STEP_MAP_DATA_LOAD	    (0x06) //파일 적용  처리 중
#define FTP_STEP_END			    (0x07) //전송처리 완료
#define FTP_STEP_DEFAULT		    (0xFF) //전송처리 완료 후 대기 모드

#define FILE_TRANS_SUCCESS_ON		(0x01) //데이터 전송이 정상적으로 완료했을 경우에 ON한다.
#define FILE_TRANS_SUCCESS_OFF		(0x00) // MCP가「전송완료확인」의 ON한 것을 확인하면, OFF한다.

#define FILE_TRANS_FAIL_ON			(0x01) //데이터 전송이 비정상적으로 종료했을 경우에 ON한다. 
#define FILE_TRANS_FAIL_OFF			(0x00) // MCP가「전송완료확인」의 ON한 것을 확인하면, OFF한다.

// ------------ 무선모듈 펌웨어 업데이트 -------------
#define FTP_WIRELESS_DEFAULT		(0xFF) // Now work. wait state
#define FTP_WIRELESS_START			(0x00) // Connect
// #define FTP_WIRELESS_MODE_CHANGE	(0x01) // ftp mode change (normal -> bin)
#define FTP_WIRELESS_FILE_CONFIG	(0x02) // files put 
#define FTP_WIRELESS_FILE_MACFIL	(0x03) // files put 
#define FTP_WIRELESS_FILE_FIRMWARE	(0x04) // files put 
#define FTP_WIRELESS_RESET_CMD		(0x05) // Reset command send
#define FTP_WIRELESS_DISCONNECT     (0x06) // Disconnect & Ending. Step go default.

// ---------- 데이터 전송지시 응답(접수불가 구분) ----------
#define FTP_NOT_ACCEPT_NO_ERR          (0) // 접수불가 오류 없음
#define FTP_NOT_ACCEPT_NO_SUPPORT_DATA (1) // 데이터 전송을 서포트하고 있지 않다.
#define FTP_NOT_ACCEPT_IP_ADDR_ERR     (2) // IP Address가 바르지 않다.
#define FTP_NOT_ACCEPT_LOGIN_INFO_ERR  (3) // 로그인 정보가 바르지 않다.
#define FTP_NOT_ACCEPT_DIRECTORY_ERR   (4) // 전송처가 바르지 않다.
#define FTP_NOT_ACCEPT_FILE_ERR        (5) // 지정된 파일이 존재하지 않는다.
#define FTP_NOT_ACCEPT_ORDER_ERR       (6) // 데이터 전송할수 있는 상태가 아니다.
										  // 전송 지시의 내용이 전송 요구와 다르다.
										  // --> OHT main에서 자체 처리, Assistant는 전송요구 정보를 안가지고 있음
#define FTP_NO_ACCEPT_TRAN_ORDER      (7) // 이미 다른 전송지시를 실행중

// ---------- 데이터 전송지시 요구구분(FTP->OHT)  ----------
#define FTP_CMDTYPE_MAPSET_TEACHING          (0x01) // 맵 파일 세트 조정용
#define FTP_CMDTYPE_MAPSET_OPERATING         (0x02) // 맵 파일 세트 운용용
#define FTP_CMDTYPE_OS                       (0x04) // OS
#define FTP_CMDTYPE_OS_SETTING               (0x08) // OS설정파일(예약)
#define FTP_CMDTYPE_APPLICATION              (0x10) // 어플리케이션 프로그램
#define FTP_CMDTYPE_OPERATING_PARA           (0x20) // 운용 파라미터
#define FTP_CMDTYPE_AXIS_PARA                (0x40) // 축 파라미터(예약)
// ---------- 데이터 전송지시 요구구분(OHT->FTP)  ----------
#define FTP_CMDTYPE_LOGDATA                  (0x01) // 로그데이터
#define FTP_CMDTYPE_MAPSET                   (0x02) // 맵 파일 세트
// ---------- 데이터 전송구분  ----------
#define FTP_TESTMODE_FTP2OHT                  (0x00) // FTP -> OHT
#define FTP_TESTMODE_OHT2FTP                  (0x01) // OHT -> FTP

#define FTP_ERROR_ON                          (0x01) // 접수불가 비트 : 접수 불가의 경우 ON
#define FTP_ERROR_OFF                         (0x00) // 접수불가 비트 : 정상 접수의 경우 OFF


// ---------- 패킷 타입 정의 ----------
// 주소획득 패킷

#pragma pack(push, 1) //한 바이트씩 패킹..

// [OCS - OHT]
typedef struct //AddressBroadcastPacket
{
	unsigned char 		identify;		// 1Byte, 숫자가 들어감
	unsigned short 		MachineType;	// 2Byte
	char 				MachineID[6];	// 6Byte
} AddressBroadcastPacket;

void Conv_AddressBroadcastPacket(AddressBroadcastPacket& Pkt)
{
	ConvertEndian(&Pkt.MachineType);
}

// [OHT - DIAG]
typedef struct 
{
	
	unsigned short 		MachineType;	// 2Byte, MACHINETYPE_DIAG_SVR, MACHINETYPE_DIAG_NORMAL_OHT
	char 				MachineID[6];	// 6Byte, Name 
	unsigned short 		CommandID;		// 2Byte, ID주소의 통지(ADDRESS_DIAG_BROADCAST_DEF), 응답(ADDRESS_DIAG_UNICAST_REPLY) 
} AddressBroadcast_DIAG_Packet;

void Conv_AddressBroadcast_DIAG_Packet(AddressBroadcast_DIAG_Packet& Pkt)
{
	ConvertEndian(&Pkt.MachineType);
	ConvertEndian(&Pkt.CommandID);
}


// OCS용 정보저장 구조체
// [OCS - OHT]
typedef struct
{
	AnsiString strAddress;
	AnsiString strMachineID;
} VHL_INFO;

// [OHT - DIAG]
typedef struct
{
	AnsiString strAddress;
	AnsiString strMachineID;
} DIAG_SVR_INFO;



// ------------------ 여기는 패킷의 부품들임 --------------------
typedef struct
{
	// Header
	unsigned short	MachineType;	// 2Byte, 기종
	char 			MachineID[6];	// 6Byte , 머신ID
} PROTOCOL_HEADER;



typedef struct
{
	unsigned short	Value;			// 2Byte
} PROTOCOL_TAG;


/**
@brief   TAGCATEGORY 정보
@author  puting
@date    2015.11.11
*/
typedef enum STANDARDCATEGORY_
{
	CATEGORY_PRIORITY 	= 1,	//우선 처리 명령(Auto, Manual, Reset 등 _ Monitoring 정보 포함) : 자동/수동, 기동 On/off, 에러 상관없는 명령들
	CATEGORY_PRIMARY,           //첫째 명령 : Error가 없이, 자동/수동, 기동 On/off에는 상관없는 명령들 : 우선처리중 에러가 아닐때 처리가 가능한 명령들.
	CATEGORY_SECONDARY,			//둘쨰 명령 : Error가 없이 수동 상태에서, 기동 On/off에 상관없는 명령들. : 메뉴얼 동작
	CATEGORY_TERTIARY,          //셋째 명령 : Error가 없이, 기동 On, 자동 일떄만 처리되는 명령들.  : OCS 명령들
	CATEGORY_ETC
}STANDARDCATEGORY;


namespace TagID
{
	enum Reserved
	{
	// 작업지시 관련 명령등
		MCP_TO_CLW_STATUS						= 1001,
		CLW_TO_MCP_STATUS						= 1003, // CLW Status 보고 (CLW -> MCP)

		CMD_SIMPLE_VERSION_REQUEST				= 1081,	// 버전요청 (OCS -> OHT) 
		RESPONSE_SIMPLE_VERSION_OHT_OCS			= 1082, // 버전보고 (OHT -> OCS)

		CMD_STATION_INFO_REQUST_OHT2OCS		 	= 1123,
		STATION_INFO_RESPONSE_OCS2OHT			= 1124,
	
		JOB_ORDER								= 1121, // 작업지시 (MCP -> CLW)
		JOB_ORDER_RESPONSE						= 1012, // 작업지시응답 (CLW -> MCP)
		JOB_PROGRESS_REPORT						= 1013, // 진척보고 (CLW -> MCP)    //진척보고시 변경 1103 ->1013 //AOHC-297

		JOB_WARNING_REPORT					    = 1029, // 경고장 보고    //AOHC-296
		
		CMD_PASSPERMIT_OHT2OCS					= 1241, // 통과 허가 요구
		RESPONSE_PASSPERMIT_OCS2OHT				= 1242, // 통과 허가 요구 응답
		CMD_PASSPERMITOPEN_OHT2OCS				= 1243, // 통과 허가 개방 요구
		RESPONSE_PASSPERMITOPEN_OCS2OHT			= 1244, // 통과 허가 개방 요구 응답
		CMD_PASSPERMITREQ_OCS2OHT				= 1245, // 통과 허가 문의
		
		RESPONSE_PASSPERMITREQ_OHT2OCS  		= 1246, // 통과 허가 상태 보고
		CMD_PASSPERMITFORCEOPEN_OHT2OCS 		= 1247, // 통과 허가 강제 개방 요구
		RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT	= 1248, // 통과 허가 강제 개방 응답
		
		REQ_DATATRANSFER_OHT2OCS				= 1221, // 데이터 전송 요구
		CMD_DATATRANSFER_OCS2OHT				= 1223, // 데이터 전송 지시
		RESPONSE_DATATRANSFER_OHT2OCS			= 1224, // 데이터 전송 지시 응답
		REPORT_DATATRANSFERPROGRESS_OHT2OCS		= 1225, // 데이터 전송 진척 보고

		CID_ERROR_REPORT_OHT2OCS				= 1249, // CID 이상 보고
		MOVE_RESTART_OCS2OHT					= 1250, // 주행 재게

		PASSPERMIT_CANCEL_OCS2OHT			= 1270, // 통과 허가 취소      //soming
		RESPONSE_PASSPERMIT_CANCEL_OHT2OCS	        = 1271, // 통과 허가 취소 응답 //soming

		CMD_FLOORMOVE_OCS_OHT					= 1115, // 층간이동 지시
		RESPONSE_FLOORMOVE_OHT2OCS				= 1116, // 층간이동 지시 응답
		
		CMD_INSERTREQ_OHT_OCS					= 1015, // 투입 요구
		RESPONSE_INSERTREQ_OCS_OHT				= 1016, // 투입 요구 응답

//		CMD_TAKEOUTREQ_OHT_OCS					= 1017, // 빼냄 보고
		CMD_TAKEOUTREQ_OHT_OCS					= 17, // 빼냄 보고   // OCS와 맞추기 위해 변경

		CMD_DEF_REQ_OHT_OCS						= 1019, // 주회 주행 요구

		CMD_VERSIONREQ_OCS_OHT					= 1227, // 버전 상세 문의
		RESPONSE_VERSIONREQ_OHT_OCS				= 1228,	 // 버전 상세 보고               04228
	
		CMD_AUTOCHANGE_OCS_OHT					= 1251, // manual -> auto 지시
		RESPONSE_AUTOCHANGE_OCS_OHT 			= 1252, // manual -> auto 응답

		CMD_CARRIERINFO_OHT_OCS                 = 1301, // Carrier 정보 요청
		RESPONSE_CARRIERINFO_OCS_OHT            = 1302, // Carrier 정보 요청 응답

		CMD_TP_COMMON							= 44777,
		CMD_SOFTRESET_TP_OHT					= 14213,
		CMD_AUTOMODE_TP_OHT					   	= 14551,
		CMD_MANUALMODE_TP_OHT					= 14552,
		CMD_E_STOP_TP_OHT						= 14555,
		
		CMD_STATUSREQ_TP_OHT					= 14560, // OHT's Status request (from TP)
		CMD_STATUSREQ_TP_RESPONSE				= 14561, // OHT's Retponse (from OHT)

		CMD_INITREQ_TP_OHT						= 14562,
		CMD_INITREQ_TP_RESPONSE					= 14563,

		CMD_VIBDATA_REQ_TP_OHT					= 14564,
		CMD_VIBDATA_REQ_TP_RESPONSE				= 14565,

		CMD_FIRMWARE_VERSION_REQUEST_AGENT2OHT  = 15710, // 펌웨어 버전요청 (AGENT -> OHT)
		RESPONSE_FIRMWARE_VERSION_OHT_AGENT     = 15711, // 펌웨어 버전응답 (OHT -> AGENT)

		CMD_SIMPLE_VERSION_REQUEST_TP  			= 25081,	// 버전요청 (OCS -> OHT)
		RESPONSE_SIMPLE_VERSION_OHT_TP			= 25082, // 버전보고 (OHT -> OCS)

		JOB_ORDER_TP		   					= 22121, // 작업지시 (MCP -> CLW)
		JOB_ORDER_RESPONSE_TP  					= 22012, // 작업지시응답 (CLW -> MCP)

		CMD_PASSPERMITFORCEOPEN_OHT2TP 			= 23247, // 통과 허가 강제 개방 요구
		RESPONSE_PASSPERMITFORCEOPEN_TP2OHT		= 23248, // 통과 허가 강제 개방 응답

		REQ_DATATRANSFER_OHT_TP					= 25221, // 데이터 전송 요구
		CMD_DATATRANSFER_TP_OHT					= 25223, // 데이터 전송 지시
		RESPONSE_DATATRANSFER_OHT_TP			= 25224, // 데이터 전송 지시 응답
		REPORT_DATATRANSFERPROGRESS_OHT_TP		= 25225, // 데이터 전송 진척 보고

		CMD_VERSIONREQ_TP_OHT					= 25227, // 버전 상세 문의
		RESPONSE_VERSIONREQ_OHT_TP				= 25228, // 버전 상세 보고               04228

		CMD_IOMAP_REQ_TP2OHT					= 25230, // TP에서 OHT로 IO리스트를 요청한다
		CMD_IOMAP_RESPONSE_OHT2TP				= 25231, // OHT에서 TP로 IO리스트를 전송한다

		CMD_IOMAP_USE_ONOFF_TP2OHT				= 25232, // TP에서 IO의 ON, OFF를 행한다
		CMD_IOMAP_SET_RESET_TP2OHT				= 25233, // TP에서 IO의 1, 0 세팅을 행한다

		CMD_TEACHING_INFO_SAVE					= 25234, // TP에서 티칭후 OHT로 티칭값을 전송한다
		CMD_TEACHING_INFO_REQ					= 25235, // TP에서 OHT의 티칭 데이터를 요청한다
		CMD_TEACHING_INFO_LOAD					= 25236, // OHT에 저장되어 있는 티칭값을 TP로 전송한다

		CMD_TRANS_LOAD							= 25237, // 재하를 LOAD (장비로 내려놓음) 하도록 한다
		CMD_TRANS_UNLOAD						= 25238, // 재하를 UNLOAD (장비에서 꺼내 OHT로 이동) 하도록 한다
		CMD_TRANS_MOVE_TEACHING_POINT			= 25239, // TEACHING POINT로 이동하도록 한다

		CMD_STATIONINFOREQ_TP2OHT				= 25240, // TP에서 Station 정보 요구.
		CMD_DRIVINGMAP_INFO_SAVE				= 25253, // TP에서 변경하고자하는 주행Map을 OHT로 전송한다
		CMD_DRIVINGMAP_INFO_REQ                 = 25254,
		CMD_DRIVINGMAP_INFO_LOAD				= 25255, // OHT에 저장되어 있는 티칭값을 TP로 전송한다

		CMD_OACIS_INFO_SAVE						= 25256, // OACIS 결과를 OHT로 전송한다

		CMD_WINDOW_OS_SHUTDOWN					= 25257, // Window OS 종료 버튼을 전송한다.

		CMD_TP_DUAL_STAGE_STB_TRANS				= 25569, // TP에서 OHT로 이동명령 (2단STB 작업 명령)

		CMD_TP_MOVE_BY_MAP						= 25570, // TP에서 OHT로 이동명령 (목적지만 지정하고 MAP정보 사용)
		CMD_TP_MOVE_RESULT						= 25571, // TP에서 OHT로 이동명령 Result
        CMD_TP_MOVE_LOAD                        = 25572, // TP에서 OHT로 이동 후 Load 명령
		CMD_TP_MOVE_UNLOAD                      = 25574, // TP에서 OHT로 이동 후 Unload 명령

		CMD_MAP_MAKE_RESULT						= 25575, // Map Make후 진행결과를 OHT에서 TP로 전송

        CMD_TP_MOVE_ALL_HOME                    = 25576,  // TP에서 Hoist, Shift, Rotate, Shuttet, Hand Home 명령
		CMD_TP_MOVE_HOIST_HOME 					= 25577,
        CMD_TP_AUTOMODE_CHANGE                  = 25578,  // TP에서 자동 Auto 전환 명령
		CMD_TP_OHT_TAKEOUT                      = 25580,  // TP에서 OHT를 MTL로 Take Out 명령
		CMD_TP_AUTO_HOISTHOME                   = 25581,  // TP에서 Hoist  원점을 자동으로 보정해주는 명령

		CMD_CID_M_COMMAND_SEND				    = 25600,
		CMD_CID_M_COMMAND_RESULT_READ			= 25601,
		CMD_CID_M_COMMAND_RESULT_RETURN			= 25602,

		CMD_CID_SETTING_SEND					= 25603,
		CMD_CID_SETTING_SEND_RETURN				= 25604,

		CMD_TP_SOUND_ONOFF						= 25610,
		CMD_TP_POWER_ONOFF						= 25611,

		CMD_TP_AMC_UPDATE						= 25612,

		CMD_TP_ERRORIGNOL_ONOFF					= 25613,  //AOHT-0001_FOUPCOVER

		CMD_TP_OCS_PAUSE						= 25615,
		CMD_TP_OCS_RESUME						= 25616,

		CMD_TP_TP_PAUSE 						= 25617,
		CMD_TP_TP_RESUME						= 25618,

		CMD_TP_DEBUG_COMMAND				= 25620,
		CMD_TP_DEBUG_RESULT					= 25621,

		CMD_TP_HID_LOG_SAVE_ORDER				= 25622,
		CMD_TP_HID_LOG_SAVE_RESULT				= 25623,

		CMD_FIRMWARE_UPDATE_RESTART_REQ_OHT2AS	= 25700,
		CMD_FIRMWARE_UPDATE_CLOSE_ORDER_AS2OHT  = 25701,
		CMD_FIRMWARE_UPDATE_AUTOMODE_ORDER_AS2OHT= 25702,

		CMD_FIRMWARE_UPDATE_ORDER_WIRELESS_OHT2AS 	= 25703,
		CMD_FIRMWARE_UPDATE_RESULT_WIRELESS_AS2OHT 	= 25704,

		CMD_TEACHING_LOOKDOWN_LEVEL_SET         = 25801, // 2016-12-27, Add by Labeler, LookDown Level 설정용 Tag 추가

		CMD_TP_MAPMAKER_FILE                    = 25900,  // TP에서 MapMaker Trigger(file 유)
//		CMD_TP_MAPMAKER   			            = 25902,  // TP에서 MapMaker Trigger(file 유)
		CMD_TP_AUTOTEACHING_FILE                = 25902,  // TP에서 MapMaker Trigger(file 유)
		CMD_TP_AUTOTEACHING                   	= 25903,  // TP에서 MapMaker Trigger(file 무)
		CMD_TP_AUTOTEACHING_READ_DRIVING       	= 25904,  // TP에서 MapMaker Trigger(file 무)
		CMD_TP_AUTOTEACHING_LOAD_DRIVING	  	= 25905,  // OHT에 저장되어 있는 티칭값을 TP로 전송한다
		CMD_TP_AUTOTEACHING_NULL_TYPE          	= 25906,  // 오토티칭 영점조정
		CMD_TP_AUTOTEACHING_VISION              = 25907,  // Vision을 이용한 Auto Teaching

		CMD_TP_HOIST_TEACH_LOAD_VALUE			= 25910,  // TP -> OHT (hoist pameter)
		CMD_TP_HOIST_TEACH_SAVE_VALUE			= 25911,
		CMD_TP_HOIST_TEACH_MOVE_TO_PULSE		= 25912,
		CMD_TP_HOIST_TEACH_LOAD_RESULT			= 25913,  // OHT -> TP (hoist pameter)
		CMD_TP_CHECK_MATERIAL					= 25914,

		CMD_TP_VIB_DATA_LINK					= 25920,
		CMD_TP_VIB_DATA_REQ_START				= 25921,
		CMD_TP_VIB_DATA_REQ_STOP				= 25922,
		CMD_TP_VIB_DATA_CLOSE					= 25923,
		CMD_TP_VIB_DATA_ALIVE					= 25924,


		CMD_AUTO_DRIVING_TEST_ROUTE_REQ_TP2OHT              = 26931,
		RESPONSE_AUTO_DRIVING_TEST_ROUTE_OHT2TP  			= 26932,
		CMD_AUTO_DRIVING_TEST_MOVE_REQ_TP2OHT				= 26933,
		RESPONSE_AUTO_DRIVING_TEST_MOVE_OHT2TP  			= 26934,

		CMD_TP_TO_MAPVIEW_STATUS_REQ			= 30001,	//TP -> MAP VIEW 상태요청
		CMD_MAPVIEW_TO_TP_STATUS_RESPONSE		= 30002,    //MAP VIEW -> TP 상태요청응답
		CMD_TP_TO_MAPVIEW_MAP_VERSION_REQ		= 30003,    //TP -> MAP VIEW UI 요청
		CMD_MAPVIEW_TO_TP_MAP_VERSION_RESPONSE	= 30004,    //MAP VIEW -> TP UI 요청응답
		CMD_TP_TO_MAPVIEW_UI_UPDATE_REQ			= 30005,    //TP -> MAP VIEW UI 요청
		CMD_MAPVIEW_TO_TP_UI_UPDATE_RESPONSE	= 30006,    //MAP VIEW -> TP UI 요청응답

		CMD_CONNECT_VMS_OHT_RESPONSE   			= 40001,    // VMS -> OHT 연결 응답
		CMD_STATUS_OHT_VMS				   		= 40002,    // OHT -> VMS 상태 보고
		CMD_CONNECT_OHT_VMS   					= 40003,    // OHT -> VMS 연결
		CMD_DISCONNECT_OHT_VMS		   			= 40004,    // OHT -> VMS 연결해제

		CMD_STATUS_OHT_BLACKBOX   		     	= 41001,    // OHT -> BLACKBOX 정보 전달
		CMD_TIMESETTING_BLACKBOX   				= 41002,    // OHT -> BLACKBOX 시간 설정
		CMD_TIMESETTING_BLACKBOX_RESPONSE   	= 41003,    // BLACKBOX -> OHT 시간 설정 응답
		CMD_EVENTSET_BLACKBOX   				= 41004,    // OHT -> BLACKBOX 이벤트 발생 요청
		CMD_EVENTSET_BLACKBOX_RESPONSE   	    = 41005,    // BLACKBOX -> OHT 이벤트 발생 요청 응답
		CMD_ERROR_BLACKBOX   	    			= 41006,    // BLACKBOX -> OHT 이벤트 발생 요청 응답

		CMD_INERNAL_PING_REQ					= 50000, // 내부 프로그램들끼리 핑을 보내고 받아서 연결여부를 확인한다
		CMD_INERNAL_PING_RESPONSE				= 50001,
		// 주의 : 이 부분에 01234 식으로 써 넣으면 컴파일러는 8진수로 해석해 버림!
	};
};


// 기본 분기방향 정의
namespace DefaultDirection
{
	enum Reserved
	{
	// 작업지시 관련 명령등
		DIR_LEFT						= 0,
		DIR_RIGHT						= 1,
		Reserver_2					= 2,
		Reserver_3					= 3,
		Reserver_4					= 4,
		Reserver_5					= 5,
		Reserver_6					= 6,
		Reserver_7					= 7,
		// 주의 : 이 부분에 01234 식으로 써 넣으면 컴파일러는 8진수로 해석해 버림!
	};
};

namespace TAGCATEGORY	// 실행중 사이클
{
	enum Reserved
	{
		TAG_CUTLINE_OCSCMD_OUT	  =	0,
		TAG_CATEGORY_OCSWORKORDER   , 		///< Command Type : OCS 작업지시 Tag
		TAG_CATEGORY_OCSPASSPERMIT 	, 		///< Command Type : OCS 통과허가 Tag
		TAG_CATEGORY_OCSFLOORMOVE 	, 		///< Command Type : OCS 동간이동/ 층간이동 Tag
		TAG_CATEGORY_OCSCOMMON 		,    	///< Command Type : OCS Common Command 관련 Tag
		TAG_CUTLINE_OCSCMD_IN			,		//< 순수 자동명령 커트라인

		TAG_CATEGORY_STATUS 	= 11,		///< Command Type : Status/Report Tag
		TAG_CATEGORY_INIT			,		///< Command Type : 초기화 정보 관련 Tag
		TAG_CATEGORY_DATATRANSFER 	,    	///< Command Type : Data Transfer Command 관련 Tag(Teaching, Station, IO Map ... Etc)
		TAG_CATEGORY_QUICK ,     			///< Command Type : Quick Command 관련 Tag

		TAG_CUTLINE_PCCMD_OUT	  =	21,
		TAG_CATEGORY_TPWORKORDER 	, 		///< Command Type : PC(HandTP) 작업지시 Tag
		TAG_CATEGORY_TPPASSPERMIT	 , 		///< Command Type : PC(HandTP) 통과허가 Tag
		TAG_CATEGORY_TPFLOORMOVE 	, 		///< Command Type : PC(HandTP) 동간이동/ 층간이동 Tag
		TAG_CATEGORY_TPCOMMON 	    ,		///< Command Type : PC(HandTP) Common Command 관련 Tag
		TAG_CUTLINE_PCCMD_IN	   	,		//< 순수 자동명령 커트라인
		TAG_TP_ADDRESS_DELETE		,
		TAG_TP_ADDRESS_REGISTER		,
		};
};



// ########################## 기본포맷 구조체 정의  ###########################
// Header - Tag - Status까지 정의한다. Body는 가변이므로 별도 정의해야 함
// 표준화 사양에 의거, 현재까지 정의된 내용까지만 정리하여 정의함

// ############## OHT Status 값 정의 (비트값 설정을 편하게 해준다) #############

/**
@brief   WORKINGSTATUS 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct WORKINGSTATUS_
{
	BYTE NowRunning        		: 1; ///< 기동중
	BYTE LayoutChangeRun   		: 1; ///< 레이아웃 변경용 주행중
	BYTE AbnormalTermination   	: 1; ///< 이상
	BYTE Reserved_3  			: 1; ///< Reserved
	BYTE Auto  					: 1; ///< 자동
	BYTE ReturnPossible  		: 1; ///< 반송가능
	BYTE Reserved_6  			: 1; ///< Reserved
	BYTE Reserved_7  			: 1; ///< Reserved
} WORKINGSTATUS;

/**
@brief   RUNNINGSTATUS 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct RUNNINGSTATUS_
{
    BYTE CorrectPositionStop	: 1; ///< 정위치 정지
    BYTE Running		   		: 1; ///< 주행중
    BYTE BzWait				   	: 1; ///< BZ 대기중
    BYTE CrashAvoidWait  		: 1; ///< 추돌방지 대기중
    BYTE SlowDownStop			: 1; ///< 감속 정지중
    BYTE Traffic_Sapche  		: 1; ///< 삽체
    BYTE Traffic_Jungche  		: 1; ///< 정체
    BYTE CorrectPositionOvrRun	: 1; ///< 정위치 오버런
} RUNNINGSTATUS;

/**
@brief   Carriage 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct CARRIAGE
{
    BYTE Load1        			: 1; ///< 재하1
    BYTE Reserved_2   			: 1; ///< 재하2(Reserved)
    BYTE DoubleInput   			: 1; ///< 이중반입 
    BYTE BlankUnload  			: 1; ///< 공출하
    BYTE LoadUnloadHP  			: 1; ///< 이재기 HP
    BYTE Reserved_5  			: 1; ///< 이재기 HP2(Reserved)
    BYTE Reserved_6  			: 1; ///< Reserved
    BYTE E84Timeout  			: 1; ///< E84 타임아웃
} CARRIAGE;

/**
@brief   MAINTANANCESTATE 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct MAINTANANCESTATE_
{
	BYTE TakeOutOK        		: 1; ///< 뺴냄 OK
	BYTE TakeOutNG   			: 1; ///< 뺴냄 NG
	BYTE FloorMoveOk   			: 1; ///< 층간이동 OK
	BYTE FloorMoveNg  			: 1; ///< 층간이동 NG
	BYTE Rail_1_InputReq  		: 1; ///< Rail1 투입
	BYTE Rail_2_InputReq  		: 1; ///< Rail2 투입
	BYTE HtControlStopped  		: 1; ///< HT Control Stop
    BYTE BackMovingNow  		: 1; ///< 후진
} MAINTANANCESTATE;

/**
@brief   CARRYTYPE 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct CARRYTYPE_
{
    BYTE LoadTypeCheckingNow   	: 1; ///< 재하종류 체크중
    BYTE LoadType1   			: 1; ///< 재하 종류 1
    BYTE LoadType2   			: 1; ///< 재하 종류 2 
    BYTE LoadType3  			: 1; ///< 재하 종류 3
    BYTE LoadType4  			: 1; ///< 재하 종류 4
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6  			: 1; ///< Reserved
    BYTE Reserved_7  			: 1; ///< Reserved
} CARRYTYPE;

/**
@brief   TUNDIRECTION 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct TUNDIRECTION_
{
    BYTE TurnRight        		: 1; ///< 오른쪽 분기
    BYTE TurnLeft   			: 1; ///< 왼쪽 분기
    BYTE TurnImpossible 	  	: 1; ///< 분기 불가 
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6  			: 1; ///< Reserved
    BYTE PassConfirmGet			: 1; ///< Reserved
} TUNDIRECTION;

/**
@brief   WorkkingStatus 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct DATATRANSER_STATE_
{
    BYTE DataCmdID        		: 3; ///< 데이터 전송지시 ID
    BYTE DataType   			: 1; ///< 데이터 전송 구분
    BYTE DataStatus		 	  	: 2; ///< 데이터 전송상태 구분  
    BYTE DataSuccess			: 1; ///< Data 전송 성공
    BYTE DataFail  				: 1; ///< Data 전송 실패
} DATATRANSER_STATE;

/**
@brief   Patrol 정보를 위한 구조체
@author  puting
@date    2016.12.05
*/
typedef struct PATROL_STATE_
{
	BYTE LitzCableCheck   		: 1; ///< 데이터 전송지시 ID
	BYTE SloopCheck    			: 1; ///< 데이터 전송 구분
	BYTE oscillateCheck 	  	: 1; ///< 데이터 전송상태 구분
	BYTE ParticleCheck			: 1; ///< Data 전송 성공
	BYTE Mode					: 3; ///< Data 전송 성공
	BYTE ErrorBit  				: 1; ///< Data 전송 실패
} PATROL_STATE;

/**
@brief   WorkkingStatus 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct PATROL_STATE_2_
{
	BYTE Reserved        		: 1; ///< 데이터 전송지시 ID
	BYTE LeftRailMisMatch		: 1; ///< 데이터 전송 구분
	BYTE RightRailMisMatch	 	: 1; ///< 데이터 전송상태 구분
	BYTE RegulatorTemper		: 2; ///< Data 전송 성공
	BYTE CapBankTemper			: 2; ///< Data 전송 실패
	BYTE LitzCableTemper 		: 1;
} PATROL_STATE_2;


namespace TEXTID	// Text ID
{
	enum Reserved
	{
		OCSSTATUS							= 1,
		OHTSTATUS							= 3,
		// TPSTATUS							= 4,
		INSERTREQ_RESPONSE					= 16,
		VERSION_REQ							= 81,
		VERSION_REQ_RESPONSE				= 82,
		STATION_INFO_REQUST					= 123,
		STATION_INFO_RESPONSE				= 124,
		PROGRESS_REPORT						= 13,     //AOHC-297
		WARNING_REPORT						= 29,     //AOHC-296
		JOBORDER		   					= 121,
		JOBORDER_RESPONSE					= 12,
		FLOORORDER			    			= 115,
		FLOORORDER_RESPONSE					= 116,
		REQ_DATATRANSFER					= 221,	// 데이터 전송 요구
		DATATRANSFERORDER					= 223,	// 데이터 전송 지시
		DATATRANSFERORDER_RESPONSE	   		= 224,	// 데이터 전송 지시 응답
		DATATRANSFERORDER_PROGRESS          = 225,	// 데이터 전송 진척 보고
		VERSION_REQ_DETAIL 					= 227,	// 버전 상세 문의
		VERSION_REQ_RESPONSE_DETAIL			= 228,	// 버전 상세 보고
		IOMAP_REQ_TP2OHT					= 230,	// TP에서 OHT로 IO리스트를 요청한다
		IOMAP_RESPONSE_OHT2TP				= 231,	// OHT에서 TP로 IO리스트를 전송한다
		IOMAP_USE_ONOFF_TP2OHT				= 232,  // TP에서 IO의 ON, OFF를 행한다
		IOMAP_SET_RESET_TP2OHT				= 233,  // TP에서 IO의 1, 0 세팅을 행한다	
		TEACHING_INFO_SAVE					= 234,	// TP에서 티칭후 OHT로 티칭값을 전송한다
		TEACHING_INFO_REQ					= 235,	// TP에서 OHT의 티칭 데이터를 요청한다
		TEACHING_INFO_LOAD					= 236,	// OHT에 저장되어 있는 티칭값을 TP로 전송한다
		TRANS_LOAD							= 237,  // 재하를 LOAD (장비로 내려놓음) 하도록 한다
		TRANS_UNLOAD						= 238,  // 재하를 UNLOAD (장비에서 꺼내 OHT로 이동) 하도록 한다
		TRANS_MOVE_TEACHING_POINT			= 239,  // TEACHING POINT로 이동하도록 한다
		STATIONINFOREQ_TP2OHT	        	= 240,  // Station 정보 요구
		PASSPERMIT							= 241,
		PASSPERMIT_RESPONSE					= 242,
		PASSPERMIT_OPEN 					= 243,
		PASSPERMIT_OPEN_RESPONSE			= 244,
		PASSPERMIT_REQ	    				= 245,
		PASSPERMIT_REQ_RESPONSE				= 246,
		FORCE_PASSPERMIT_OPEN				= 247,
		FORCE_PASSPERMIT_OPEN_RESPONSE	    = 248,
		CID_ERROR_REPORT					= 249,  //CID 이상 보고
		MOVE_RESTART						= 250,  //주행 재게		
		AUTOCHANGE_REQ						= 251, // manual -> auto 지시
		AUTOCHANGE_RESPONSE					= 252, // manual -> auto 응답
		DRIVINGMAP_INFO_SAVE				= 253,
		DRIVINGMAP_INFO_REQ					= 254,
		DRIVINGMAP_INFO_LOAD				= 255,

		OACIS_INFO_SAVE                     = 256,
		WINDOW_OS_SHUTDOWN                  = 257,
		PASSPERMIT_CANCEL                   = 270, //soming
		PASSPERMIT_CANCEL_RESPONSE          = 271, //soming
        CARRIERINFO_OHT_OCS                 = 301, //(OHT->OCS)Carrier 정보 요청
		RESPONSE_CARRIERINFO_OCS_OHT        = 302, //(OCS->OHT)Carrier 정보 요청 응답

		TP_COMMON							= 777,
		SOFTRESET							= 213,
		AUTOMODE							= 551,
		MANUALMODE							= 552,
		E_STOP_PC							= 555,
		TPSTATUS							= 560,
		TP_INIT								= 562,

		TP_VIB_DATA							= 564,
		TP_VIB_DATA_RESPONSE				= 565,

		TP_DUAL_STAGE_STB_TRANS             = 569,

		TP_MOVE_BY_MAP                      = 570,  // TP에서 OHT로 이동명령 (목적지만 지정하고 MAP정보 사용)		
        TP_MOVE_LOAD                        = 572,  // TP에서 OHT로 이동 후 Load 명령
        TP_MOVE_UNLOAD                      = 574,  // TP에서 OHT로 이동 후 Unload 명령

		MAP_MAKE_RESULT						= 575,	// Map Make후 진행결과를 OHT에서 TP로 전송
		
		TP_MOVE_ALL_HOME                    = 576,  // TP에서 Hoist, Shift, Rotate, Shuttet, Hand Home 명령
		TP_MOVE_HOIST_HOME = 577,
		TP_AUTOMODE_CHANGE                  = 578,  // TP에서 자동 Auto Mode 전환 명령
		TP_OHT_TAKEOUT                      = 580,  // TP에서 OHT를 MTL로 Take Out 명령
		TP_AUTO_HOISTHOME                   = 581,  // TP에서 Hoist  원점을 자동으로 보정해주는 명령
		
		CID_M_COMMAND_SEND					= 600,	// Monitoring M Command Send
		CID_M_COMMAND_RESULT_READ			= 601,	// Monitoring M Result Read

		CID_SETTING_SEND					= 603,
		CID_SETTING_SEND_RETURN				= 604,

		TP_SOUND_ONOFF						= 610,
		TP_POWER_ONOFF						= 611,

		TP_AMC_UPDATE						= 612,
		TP_ERRORIGNOL_ONOFF             	= 613,   //AOHT-0001_FOUPCOVER

		TP_OCS_PAUSE						= 615,
		TP_OCS_RESUME						= 616,

		TP_TP_PAUSE							= 617,
		TP_TP_RESUME						= 618,

		TP_DEBUG_COMMAND				= 620,
		TP_DEBUG_RESULT 				= 621,

		TP_HID_LOG_SAVE_ORDER				= 622,
		TP_HID_LOG_SAVE_RESULT				= 623,

		FIRMWARE_UPDATE_RESTART_REQ_OHT2AS	= 700,
		FIRMWARE_UPDATE_CLOSE_ORDER_AS2OHT  = 701,
		FIRMWARE_UPDATE_AUTOMODE_ORDER_AS2OHT= 702,

		FIRMWARE_UPDATE_ORDER_WIRELESS_OHT2AS 	= 703,
		FIRMWARE_UPDATE_RESULT_WIRELESS_AS2OHT 	= 704,

		FIRMWARE_VERSION_REQUEST_AGENT2OHT = 710,

        TEACHING_LOOKDOWN_LEVEL_SET         = 801,

		TP_MAPMAKER_FILE                    = 900,  // TP에서 MapMaker Trigger(file 유)
		TP_AUTOTEACHING_FILE                = 902,  // TP에서 MapMaker Trigger(file 유)
		TP_AUTOTEACHING                   	= 903,  // TP에서 MapMaker Trigger(file 무)
		TP_AUTOTEACHING_READ_DRIVING        = 904,
		TP_AUTOTEACHING_LOAD_DRIVING        = 905,
		TP_AUTOTEACHING_NULL_TYPE			= 906,
		TP_AUTOTEACHING_VISION				= 907,

		TP_HOIST_TEACH_LOAD_VALUE			= 910,  // OHT -> TP (hoist pameter)
		TP_HOIST_TEACH_SAVE_VALUE			= 911,
		TP_HOIST_TEACH_MOVE_TO_PULSE		= 912,
		TP_CHECK_MATERIAL					= 914,

		TP_VIB_DATA_LINK					= 920,
		TP_VIB_DATA_REQ_START				= 921,
		TP_VIB_DATA_REQ_STOP				= 922,
		TP_VIB_DATA_CLOSE					= 923,
		TP_VIB_DATA_ALIVE					= 924,

		TP_AUTO_DRIVING_ROUTE_REQ			= 931,
		TP_AUTO_DRIVING_ROUTE_RESPONSE		= 932,
		TP_AUTO_DRIVING_MOVE_REQ			= 933,
		TP_AUTO_DRIVING_MOVE_RESPONSE		= 934,

		VMSSTATUS							= 940,

		BLACKBOX_STATUS                     = 950,
	};
};


namespace CYCLE	// 실행중 사이클
{
	enum Reserved
	{
		CycleNotExist			= 0x00,	// 사이클 없음
		PositionSearching		= 0x01,	// 위치확인 사이클 중
		MovingToStation			= 0x02, // 이동 사이클 중(Station)
		CraneUp					= 0x03, // 올림 사이클 중

		CraneDown				= 0x04, // 내림 사이클 중
		TakeOut			    	= 0x05, // 빼냄 사이클 중
		MovingToNode			= 0x08, // 이동 사이클 중(노드)
		MovingBetweenFloor		= 0x09,	// 층간 이동 사이클 중(ID 101의 경우만)
		MovingAutoSearch        = 0x10,

		Tour 					= 0x1A, // 투어 사이클 중
		Tour_Node 				= 0x1B, // 투어 이동 사이클 중(Node)
		Tour_Station 			= 0x1C, // 투어 이동 사이클 중(Station)
		Tour_Acquire			= 0x1D, // 투어 올림 사이클 중
		Tour_Deposit			= 0x1E, // 투어 내림 사이클 중

		Instrument				= 0x2C, // 계측 사이클(Yellow VHL 제어용)
		CleanerVhlMove    		= 0x2D, // 흡인구 이동 사이클 중(청소기 VHL 제어용)
		AreaMoving				= 0x2E, // 동간 이동 사이클(ID 101의 경우만)
		SideStepMoving			= 0x2F,	// 퇴피 이동 사이클(ID 101의 경우만)

		// ＝＝＝이하, 수동 모드시에 유효＝＝＝
		TP_RoundRunning			= 0x21, // 주회 주행중 사이클 중(미사용)
		TP_ManualOperation 		= 0x22, // 수동 조작 중
		TP_RunningLearning		= 0x23, // 주행 학습 사이클 중
		TP_PortTeaching	    	= 0x24, // 이재부 학습 중(티칭중)

		TP_CheckPortTeaching	= 0x25, // 테스트 사이클 중(MOVE T)
		TP_CraneHome			= 0x26,	// 테스트 사이클 중(패턴2)(미사용)
		TP_TestRunningLearning  = 0x27, // 주행 이동 테스트 싸이클

		TP_AutoMode				= 0x28,

		TP_AutoHoistHome		 = 0x29,
		TP_PortTeaching_NullType = 0x30,
		TP_SmalladdHoistHome 	 = 0x31,
        TP_MoveAllHome           = 0x32,
		TP_PortTeaching_Vision	 = 0x33, // 이재부 학습 중(티칭중)
		TP_CheckMaterial         = 0x34,

	};
};

// 대기 사이클은 define 머지작업시 오류가 나서 임시로 막았던 것이나 반드시 필요한 정의라 다시 살림
namespace WaitingCycle	// 대기 사이클
{
	enum Reserved
	{
		CycleNotExist			= 0x00,	// 사이클 없음
		PositionSearching		= 0x01,	// 위치확인 사이클 중
		MovingToStation			= 0x02, // 이동 사이클 중(Station)
		CraneUp					= 0x03, // 올림 사이클 중

		CraneDown				= 0x04, // 내림 사이클 중
		TakeOut			    	= 0x05, // 빼냄 사이클 중
		MovingToNode			= 0x08, // 이동 사이클 중(노드)
		MovingBetweenFloor		= 0x09,	// 층간 이동 사이클 중(ID 101의 경우만)

		Tour 					= 0x1A, // 투어 사이클 중
		Tour_Node 				= 0x1B, // 투어 이동 사이클 중(Node)
		Tour_Station 			= 0x1C, // 투어 이동 사이클 중(Station)
		Tour_Acquire			= 0x1D, // 투어 올림 사이클 중
		Tour_Deposit			= 0x1E, // 투어 내림 사이클 중

		Instrument				= 0x2C, // 계측 사이클(Yellow VHL 제어용)
		CleanerVhlMove    		= 0x2D, // 흡인구 이동 사이클 중(청소기 VHL 제어용)
		AreaMoving				= 0x2E, // 동간 이동 사이클(ID 101의 경우만)
		SideStepMoving			= 0x2F,	// 퇴피 이동 사이클(ID 101의 경우만)

		// ＝＝＝이하, 수동 모드시에 유효＝＝＝
		TP_RoundRunning			= 0x21, // 주회 주행중 사이클 중(미사용)
		TP_ManualOperation 		= 0x22, // 수동 조작 중
		TP_RunningLearning		= 0x23, // 주행 학습 사이클 중
		TP_PortTeaching	    	= 0x24, // 이재부 학습 중(티칭중)

		TP_CheckPortTeaching	= 0x25, // 테스트 사이클 중(패턴1)
		TP_CraneHome			= 0x26,	// 테스트 사이클 중(패턴2)(미사용)
		TP_TestRunningLearning  = 0x27, // 테스트 사이클 중(패턴3)(미사용)

		TP_AutoMode				= 0x28,

		TP_AutoHoistHome		= 0x29,
		TP_PortTeaching_NullType = 0x30,
		TP_SmalladdHoistHome = 0x31,
		TP_MoveAllHome           = 0x32,
		TP_PortTeaching_Vision	 = 0x33, // 이재부 학습 중(티칭중)
		TP_CheckMaterial         = 0x34,
	};
};

namespace PROGRESS	// 진척보고
{
	enum Reserved
	{
		NotProgress				= 0x00,	// 초기 상태
		DrivingBegin	 		= 0x01,	// 이동 개시
		Arrived					= 0x02, // 도착
		TransBegin				= 0x03, // 이재 개시
		TransCompleted			= 0x04, // 이재 완료
		PositionCheckCompleted 	= 0x05, // 위치 확인 완료

		InpositionOverRun  		= 0x08, // 정위치 오버런
		E84Timeout				= 0x09,	// E84 Timeout
		DoubleInsert			= 0x0A, // 이중반입
		EmptyShipment	     	= 0x0B, // 공출하.
		E84InterlockCompleted	= 0x0C, // E84 Interlock 완료    //PIO 완료시점에서추가.

		// ＝＝＝미사용＝＝＝
		HIG_Arrived				= 0x0D, // 흡인구 이동완료
		YellowVHLBegin	 		= 0x0E, // 계측 개시
		YellowVHLCompleted		= 0x0F, // 계측 종료
	};
};

namespace JobOderError	// 작업지시 상세에러 코드
{
	enum Reserved
	{
		VHL_Turn_Off			= 0x81,	// 기동 Off
		VHL_Manual		 		= 0x82,	// 메뉴얼
		VHL_Error			   	= 0x83, // 에러
		Path_Error				= 0x84, // (분기 포인트를 포인트를 지나버린 지나버린 뒤)지시받은 지시받은 경로 정보의 정보의 개시 번지에서는 번지에서는 현재 번지에 가도록 한다 .
		Taraget_Sam				= 0x85, // 이미 목적지에 정지해버렸으므로 받아 들일수 없다.
		PositionCheckCompleted 	= 0x05, // 위치 확인 완료

		InpositionOverRun  		= 0x08, // 정위치 오버런
		E84Timeout				= 0x09,	// E84 Timeout
		DoubleInsert			= 0x0A, // 이중반입
		EmptyShipment	     	= 0x0B, // 공출하.
		E84InterlockCompleted	= 0x0C, // E84 Interlock 완료

		// ＝＝＝미사용＝＝＝
		HIG_Arrived				= 0x0D, // 흡인구 이동완료
		YellowVHLBegin	 		= 0x0E, // 계측 개시
		YellowVHLCompleted		= 0x0F, // 계측 종료
	};
};

// 작업변경 Nack시 가능유무에 대한 분류 작업 puting
namespace JobChageNackType	// 작업변경 Nack 개선 내용 반영
{
	enum Reserved
	{
		CycleNotUse				= 0x00,	// 작업변경 불가능 //기타
		CycleChage_Type1		= 0x01,	// 작업변경 가능/불가능 : 3) 거리가 짧은 경우 변경된 경우 : O
		CycleChage_Type1_1 		= 0x03, // 작업변경 가능/불가능 : 3) 경로가 중간에 바뀐경우 : O
		CycleChage_Type2 		= 0x02, // 작업변경 가능 :  1) 동일한 위치로 변경된 경우  2) 멀리이동된 경우  Nack -> Ack
	};
};


namespace ORDERDIV	// 지시 구분
{
	enum Reserved
	{
		NotOrder				= 0,	// 구분 없음.
		JobOrder		 		= 1,	// 작업 지시
		JobCancel		  		= 2, 	// 작업 취소
		JobChange				= 3, 	// 작업 변경
		TargetChange			= 4,    // 목적지 변경
		PathChange 				= 5,    // 경로 변경
		JobClear 				= 9,    // 작업 취소
		JobCancel_2 	   		= 13,    // 작업 취소(신규추가)
	};
};
/**
@brief   STATUSORDER 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct STATUSORDER_
{
    BYTE MoveStart   			: 1; ///< 기동
    BYTE LayoutChangeRunOrder 	: 1; ///< 레이아웃 변경 주행 지시
    BYTE Reserved_2   			: 1; ///< Reserved
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE CraneDownAdmission 	: 1; ///< 내림 이재 허가
    BYTE CraneUpAdmission  		: 1; ///< 올림 이재 허가
} STATUSORDER;


/**
@brief   STOPORDER 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct STOPORDER_
{
    BYTE DrivingReduce			: 1; ///< 주행 감속 정지 
    BYTE DrivingEmergen			: 1; ///< 주행 긴급 정지 
    BYTE Reserved_2   			: 1; ///< Reserved
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE TrnasReduce  			: 1; ///< 이적재 감속 정지
    BYTE TransEmergen  			: 1; ///< 이적재 긴급 정지
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE CycleUnable	  		: 1; ///< 사이클 실행 불가
} STOPORDER;

/**
@brief   MAINTANANCEORDER 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct MAINTANANCEORDER_
{
    BYTE Insert_OK   			: 1; ///< Reserved
    BYTE Insert_NG 				: 1; ///< Reserved
    BYTE FloorTransfer_OK  		: 1; ///< Reserved
    BYTE FloorTransfer_NG		: 1; ///< Reserved
    BYTE Board_OK	  			: 1; ///< Reserved
    BYTE Board_NG	  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} MAINTANANCEORDER;

/**
@brief   MAINTANANCEORDER 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct EMORDER_
{
    BYTE EM_Cut		   			: 1; ///< Reserved
    BYTE Reserved_1 			: 1; ///< Reserved
    BYTE Reserved_2   			: 1; ///< Reserved
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} EMORDER;

/**
@brief   MAINTANANCEORDER 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct DATATRANSORDER_
{
    BYTE CheckDataTransfer		: 1; ///< Reserved
    BYTE Reserved_1 			: 1; ///< Reserved
    BYTE Reserved_2   			: 1; ///< Reserved
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} DATATRANSORDER;

/**
@brief   MAINTANANCEORDER 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct RUNNINGSTATUS2_
{
	BYTE CID_Interlock			: 1; ///< Reserved
	BYTE CID_Stop 				: 1; ///< Reserved
	BYTE Reserved_2   			: 1; ///< Reserved
	BYTE Reserved_3  			: 1; ///< Reserved
	BYTE Reserved_4  			: 1; ///< Reserved
	BYTE Reserved_5  			: 1; ///< Reserved
	BYTE OBSDetect	 			: 1; ///< Reserved
	BYTE OHTDetect		  		: 1; ///< Reserved
} RUNNINGSTATUS2;


namespace STOPReason	// Stop 정지 요인 상세내용 작성 필요
{
	enum Reserved
	{
		/*
		CycleNotExist			= 0x00,	// 사이클 없음
		PositionSearching		= 0x01,	// 위치확인 사이클 중
		MovingToStation			= 0x02, // 이동 사이클 중(Station)
		CraneUp					= 0x03, // 올림 사이클 중

		CraneDown				= 0x04, // 내림 사이클 중
		TakeOut			    	= 0x05, // 빼냄 사이클 중
		MovingToNode			= 0x08, // 이동 사이클 중(노드)
		MovingBetweenFloor		= 0x09,	// 층간 이동 사이클 중(ID 101의 경우만)

		Instrument				= 0x2C, // 계측 사이클(Yellow VHL 제어용)
		CleanerVhlMove    		= 0x2D, // 흡인구 이동 사이클 중(청소기 VHL 제어용)
		AreaMoving				= 0x2E, // 동간 이동 사이클(ID 101의 경우만)
		SideStepMoving			= 0x2F,	// 퇴피 이동 사이클(ID 101의 경우만)

		// ＝＝＝이하, 수동 모드시에 유효＝＝＝
		TP_RoundRunning			= 0x21, // 주회 주행중 사이클 중(미사용)
		TP_ManualOperation 		= 0x22, // 수동 조작 중
		TP_RunningLearning		= 0x23, // 주행 학습 사이클 중
		TP_PortTeaching	    	= 0x24, // 이재부 학습 중(티칭중)

		TP_TestPattern1			= 0x25, // 테스트 사이클 중(패턴1)
		TP_TestPattern2			= 0x26,	// 테스트 사이클 중(패턴2)(미사용)
		TP_TestPattern3			= 0x27, // 테스트 사이클 중(패턴3)(미사용)
		*/
		};
};

/**
@brief   OHT->OCS Status 정보를 위한 구조체
@date    2015.10.27
*/
typedef struct PROTOCOL_STATUS_OHT_
{
	WORKINGSTATUS		WorkingStatus;		// 가동상태
	RUNNINGSTATUS		RunningStatus;		// 주행상태
	CARRIAGE			Carriage;			// 이재기 상태
	MAINTANANCESTATE	MaintananceState;	// 유지보수 상태
	unsigned char		CarryType;			// 재하 및 수하상태
	unsigned char		CurrentAddr[3];		// 현재번지
	unsigned char		StopNowAddr[3];		// 정지중 station 번지
	unsigned char		DestinationAddr[3]; // 목적지 station 번지
	TUNDIRECTION		TunDirection;		// 분기방향
	unsigned char		RunningCycle;		// 실행중 사이클
	unsigned char		WaitingCycle;		// 대기 사이클
	short				ErrorCode;			// 에러코드
//	unsigned char		Error_Reserved[2];  // 에러코드
//	unsigned char		EmStatus;			// E/M상태
	DATATRANSER_STATE	DataTransStatus;	// 데이터 전송상태

	unsigned char		APSinalLevel;		// 전파강도
    short				RunningDistance;	// 주행거리 (mm)  - Offset값
	unsigned short		RunningSpeed;		// 주행속도 (mm/s)
	PATROL_STATE		Patrol_State;		//패트롤 정보
	PATROL_STATE_2		Patrol_State_2;		//패트롤 정보
	RUNNINGSTATUS2		RunningVHLStatus;		//CID정보
	unsigned char		Dummy[2];			// 30바이트에 맞춰야 하므로 더미 데이터 넣음
} PROTOCOL_STATUS_OHT;

typedef struct PROTOCOL_FW_VERSION_OHT_
{
	int RSADrivingRear;
	int RSADRivingFront;
	int RSAHoist;
	int RSASlide;
	int FineSlaveRear;
	int FineSlaveFront;
} PROTOCOL_FW_VERSION_OHT;

/**
@brief   OCS->OHT Status 정보를 위한 구조체
@date    2015.10.27
*/
typedef struct PROTOCOL_STATUS_OCS_
{
	STATUSORDER			StatusOrder;		// 상태지시
	STOPORDER			StopOrder;			// 정지지시
	unsigned char		StopReason;			// 정지요인
	MAINTANANCEORDER	MaintananceOrder;	// 유지보수지시
	EMORDER				EmOder;				// E/M지시
	DATATRANSORDER		DataTransOrder;		// 데이터 전송지시
	unsigned char		Dummy[24];			// 30바이트에 맞춰야 하므로 더미 데이터 넣음

} PROTOCOL_STATUS_OCS;


/**
@brief   STATUSORDER 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct INNER_ORDER_STATUS_
{
	BYTE LayoutChangeRunOrder	: 1; ///< 레이아웃 변경 주행 지시
	BYTE CycleUnable	   	 	: 1; ///< Cycle 실행 불가
	BYTE DataTransfer	   		: 1; ///< 데이터 전송 성공 유무
	BYTE CraneDownAdmission  	: 1; ///< 내림 이재 허가
	BYTE CraneUpAdmission  		: 1; ///< 올림 이재 허가
	BYTE Reserved1	  		 	: 1; ///< Reseverd
	BYTE Reserved2		 		: 1; ///< Reseverd
	BYTE Reserved3	  	   		: 1; ///< Reseverd
}INNER_ORDER_STATUS;

/**
@brief   Pause 조건들 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct INNER_PAUSE_STATUS_
{
	BYTE OHT_StartUp		   	: 1; ///< OHT 기동 On/Off
	BYTE OHT_Detect			 	: 1; ///< OHT 감지될 경우 퓨즈
	BYTE OBS_Detect	   			: 1; ///< OBS 감지될 경우
	BYTE CID		 			: 1; ///< CID 감지 될 경우
	BYTE LastOHTDetect			: 1; ///< 통과허가 미허용 위치.
	BYTE Reseverd5			 	: 1; ///< 내림 이재 허가
	BYTE Reseverd6		  		: 1; ///< 올림 이재 허가
	BYTE Reseverd7 				: 1; ///< 올림 이재 허가
} INNER_PAUSE_STATUS;

/**
@brief   OHT Status 정보를 위한 구조체
@date    2015.10.27
*/
typedef struct INNER_STATUS_OHT_
{
	PROTOCOL_STATUS_OHT 	StandardOHTStatus;
	INNER_ORDER_STATUS	 	StandardOrderStatus;
	BYTE                    VHL_Insert;
	BYTE 					VHL_FloorTransfer; ///< Reseverd
	BYTE 					VHL_Board; ///< Reseverd
	INNER_PAUSE_STATUS	 	PauseStatus;
	unsigned char			StopReason;			// 정지요인

} INNER_STATUS_OHT;

/**
@brief   OHT 기동 Off 정보를 위한 구조체
@date    2016.04.22
*/
typedef struct OHT_TURN_OFF_STATUS_
{
	bool                    bMarked; ///<
	UINT					nNodeID; ///< NodeID(주행)
	UINT					nStationID; ///< StaionID(이적재)
	double					dOffset; ///<마지막 주행Node로부터 떨어진 값.
	BYTE 					VHL_Div; ///< 분기정보
} OHT_TURN_OFF_STATUS;


// ########################## 여기는 완성된 패킷 선언들임 (ID도 적어둔다) (BODY부 정의 포함)  ##########################

/**
@brief   REASPONSEACK 정보를 위한 구조체
@author  puting
@date    2015.10.27
//AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현
*/
typedef struct AUTOCHANGEACK_
{
    BYTE PresionCode	   		: 6; ///< 상세코드
    BYTE VHL_Auto 				: 1; ///< 상태(풀오토1, 세미오토0) : 접수가능일때
    BYTE Accept_Error			: 1; ///< 접수불가
} AUTOCHANGEACK;


/**
@brief   CMDDIVIDE 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct CMDDIVIDE_
{
    BYTE CMDType   			: 7; ///< 지시구분
    BYTE TestMode 			: 1; ///< Test Mode
} CMDDIVIDE;

/**
@brief   WORKCARRYTYPE 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct WORKCARRYTYPE_
{
    BYTE Carriage_1   			: 1; ///< FOUP
    BYTE Carriage_2 			: 1; ///< POD
    BYTE Carriage_3   			: 1; ///< MAC
    BYTE Carriage_4  			: 1; ///< FOSB
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} WORKCARRYTYPE;


typedef struct BUFFERTYPE_
{
	BYTE Buffer_1	 			: 1; ///< 제1수하대	
	BYTE Buffer_2 				: 1; ///< 제2수하대
	BYTE Reserved_2 			: 1; ///< Reserved
	BYTE Reserved_3 			: 1; ///< Reserved
	BYTE Reserved_4 			: 1; ///< Reserved
	BYTE Reserved_5 			: 1; ///< Reserved
	BYTE Reserved_6 			: 1; ///< Reserved
	BYTE Reserved_7 			: 1; ///< Reserved
} BUFFERTYPE;

/**
@brief   RIV 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct RIV_
{
    BYTE Riv_Value 			: 7; ///< 우선도
    BYTE Hotlot 			: 1; ///< Hot lot
} RIV;

/**
@brief   CHANGENODE 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct CHANGENODE_
{
    BYTE Map_Num	   			: 4; ///< Map 번호
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} CHANGENODE;

/**
@brief   MAINTANANCEORDER 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct FINAL_PORT_TYPE_
{
    BYTE PortType_1   			: 1; ///< STOKER
    BYTE PortType_2 			: 1; ///< EQ
    BYTE PortType_3   			: 1; ///< STB
    BYTE PortType_4  			: 1; ///< UTB
    BYTE PortType_5  			: 1; ///< LOADER
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} FINAL_PORT_TYPE;

/**
@brief   STATION_MARK_TYPE 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct STATION_MARK_TYPE_
{
    BYTE StopTag   				: 1; ///< STOP TAG
    BYTE QR_Tag_Left 			: 1; ///< QR Tag(Left)
    BYTE QR_Tag_Right  			: 1; ///< QR Tag(Right)
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} STATION_MARK_TYPE;

/**
@brief   TEACHINGDATA_LOOKDOWN 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct TEACHINGDATA_LOOKDOWN_
{
    BYTE LKDown_1   			: 1; ///< LKDown_1
    BYTE LKDown_2 				: 1; ///< LKDown_2
    BYTE LKDown_3  				: 1; ///< LKDown_3
    BYTE LKDown_4  				: 1; ///< LKDown_4
    BYTE LKDown_5 	 			: 1; ///< LKDown_5
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} TEACHINGDATA_LOOKDOWN;


/**
@brief   TEACHINGDATA_PIOINFO 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct TEACHINGDATA_PIOINFO_
{
    BYTE EQPIO_ENABLE  			: 1; ///< EQPIO 사용유무   
    BYTE EQPIO_Left   			: 1; ///< EQPIO_Left
    BYTE EQPIO_Right			: 1; ///< EQPIO_Right
    BYTE EQPIO_IR 				: 1; ///< EQPIO_IR
    BYTE EQPIO_RF 				: 1; ///< EQPIO_RF
    BYTE Reserved_4 	 		: 1; ///< Reserved
    BYTE PIO_CS0 				: 1; ///< PIO_CS0
    BYTE PIO_CS1		  		: 1; ///< PIO_CS1
} TEACHINGDATA_PIOINFO;

/**
@brief   TEACHINGDATA_PIOTIME 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct TEACHINGDATA_PIOTIME_
{
    BYTE PIOTime_1   			: 1; ///< PIOTIME_1
    BYTE PIOTime_2				: 1; ///< PIOTIME_2
    BYTE PIOTime_3 				: 1; ///< PIOTIME_3
    BYTE PIOTime_4 				: 1; ///< PIOTIME_4
    BYTE PIOTime_5 	 			: 1; ///< PIOTIME_5
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} TEACHINGDATA_PIOTIME;


/**
@brief   TEACHINGDATA_OPTION 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct TEACHINGDATA_OPTION_
{
    BYTE AutoRecovery			: 1; ///< AUTORECOVERY
    BYTE RFReader				: 1; ///< RFREADER
    BYTE TransSound 			: 1; ///< TRANSSOUND
    BYTE Oscilation 			: 1; ///< OSCILATION
    BYTE HnadDecetEQ 	 		: 1; ///< HANDDECETEQ
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} TEACHINGDATA_OPTION;


/**
@brief   PATHDATA_DEF_DIRECTION 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct PATHDATA_DEF_DIRECTION_
{
    BYTE Path_Direction	   		: 1; ///< 분기 방향
    BYTE Driving_Flag			: 1; ///< 주행 플래그
    BYTE Reserved_2 			: 1; ///< Reserved
    BYTE Reserved_3 			: 1; ///< Reserved
    BYTE Reserved_4 	 		: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} PATHDATA_DEF_DIRECTION;


/**
@brief   REASPONSEACK 정보를 위한 구조체
@author  puting
@date    2015.10.27
*/
typedef struct REASPONSEACK_
{
    BYTE PresionCode	   		: 6; ///< 상세코드
    BYTE VHL_Error 				: 1; ///< VHL 이상
    BYTE Accept_Error			: 1; ///< 접수불가
} REASPONSEACK;


typedef struct OHTREQUEST_
{
    BYTE CMDType   			: 7; ///< 요구구분
    BYTE TestMode 			: 1; ///< 전송구분
    
} OHTREQUEST;


typedef struct TRANSORDERACK_
{
    BYTE TransOrderID	   		: 4; ///< 전송 지시 ID    
    BYTE Accept_Error_Class		: 3; ///< 접수 불가 구분    
    BYTE Accept_Error			: 1; ///< 접수불가
} TRANSORDERACK;


typedef struct TRANS_ORDER_RESULT_
{
    BYTE TransOrderID	   		: 4; ///< 전송 지시 ID    
    BYTE Not_Accept_Class		: 2; ///< 접수 불가 구분    
    BYTE Trans_OK				: 1; ///< 전송 성공
    BYTE Trans_Fail				: 1; ///< 전송 실패
} TRANS_ORDER_RESULT;



typedef struct
{
	unsigned short			CmdID;				// 지시 ID
	CMDDIVIDE				CmdDiv;				// 지시구분
	unsigned char			CmdCycle;			// 지시 사이클
	BUFFERTYPE				BufferType;			// 이적재 Buffer 위치   
	unsigned char			StopNodeID[3];		// 목적지노드
	unsigned char			StopStationID[3];	// 목적지 Station노드
		
	RIV						Riv;				// 우선도
	CHANGENODE				ChangeNode;			// 변환번지 정보
	unsigned char			AutoRecovery;    		// Auto Recovry
	//변경 후 Station 정보
	int						StationData_StationID;  //  추가
	unsigned char			StationData_MarkType;   // Target 이적재 Tag Type
	int     				StationData_ParentNodeID;   //추가
	int     				StationData_NextNodeID;      //추가
	short     				StationData_Offset;      //추가
	unsigned char           StationData_Safety_Level;
	int						StationData_PhysicalID;		// 2단 STB, 실제 물리적 Tag ID
	unsigned char			StationData_Reserved[3];
//	unsigned char			StationData_Reserved[7];    // Target 이적재 Tag Type

	int						TeachingData_StationID;  //  추가
	unsigned char			TeachingData_PortType;  //  추가
	int						TeachingData_Hoist;  //  추가
	int						TeachingData_Shift;  //  추가
	int						TeachingData_Rotate;  //  추가
	unsigned char			TeachingData_PIOType;  //  추가
	unsigned char			TeachingData_PIODirect;  //  추가
	int						TeachingData_PIOID;  //  추가
	int						TeachingData_PIOChannel;  //  추가
	unsigned char			TeachingData_PIOTimeLevel;  //  추가
	unsigned char			TeachingData_AutoRecovery;  //  추가
	unsigned char			TeachingData_HandDetectEQ;  //  추가
	int						TeachingData_STBOffset;  //  추가
	unsigned char			TeachingData_LookDownLevel;  //  추가
	unsigned char			TeachingData_SpecialEQLevel;  //  AOHC-294
	unsigned char			TeachingData_PIOCS;
	unsigned char			TeachingData_MaterialType;
	unsigned char			TeachingData_Reserved[5];    // Target 이적재 Tag Type
//	unsigned char			TeachingData_Reserved[6];    // Target 이적재 Tag Type
//	unsigned char			TeachingData_Reserved[7];    // Target 이적재 Tag Type
	
	//변경 전 Data
/*
    unsigned char			StationData_MarkType;   // Target 이적재 Tag Type
	unsigned char			StationData_PortID[12];    // Target 이적재 Tag Type
	int						StationData_RunDist;			// Target_RunDist
	int						TeachingData_Hoist;	// Node로부터 이적재 Tag까지 거리
	int						TeachingData_Shift;	// Node로부터 이적재 Tag까지 거리
	int						TeachingData_Rotate;	// Node로부터 이적재 Tag까지 거리

	unsigned char 	  		 TeachingData_FinalPortType;
	
	unsigned char			TeachingData_LookDownLevel;	// Node로부터 이적재 Tag까지 거리
	TEACHINGDATA_PIOINFO	TeachingData_PIOInfo;
	unsigned char			TeachingData_PIOTimeLevel;
	TEACHINGDATA_OPTION		TeachingData_Option;
	unsigned char			TeachingData_Reserved[8];
*/



		
	PATHDATA_DEF_DIRECTION	PathData_Def_Direction;		// 기본 분기방향
	unsigned char			PathData_StartNodeID[3];	// 개시번지
	unsigned char			PathData_EndNodeID[3];		// 종료번지
	unsigned short			PathData_ExtraNum;			//	기본외 분기 번지수
	
	// 이 항목 바로 앞까지 크기를 빼고, 정의된 크기에서 넣을 수 있는 최대의 수를 연산한다
	// 아래의 40은 앞 항목들의 Byte 크기. 변경되면 반드시 수정해 줄것
	unsigned char  	PathData_ExtraID[MAX_PACKET_SIZE - 130];
} PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121;

 




typedef struct
{
	unsigned short			CmdID;					// 지시ID
	CMDDIVIDE				ResponseDiv;			// 지시구분
	unsigned char			CmdCycle;				// 지시 사이클
	BUFFERTYPE				BufferType;				// 이적재 Buffer 위치	
	unsigned char			StopNodeID[3];			// 목적지노드
	unsigned char			StopStationID[3];		// 목적지 Station노드
	RIV						Riv;					// 우선도
	REASPONSEACK			ResponseAck;			// 응답/접수 불가 구분
} PROTOCOL_BODY_JOB_RESPONSE_CLW_TO_MCP_ID_122;


//==================================Station 정보 요구/응답 추가=======================================//

typedef struct
{
//	unsigned short			CmdID;					// 지시ID
	unsigned char			RequestDiv;				//  요구 구분
//	unsigned char			CmdCycle;				// 지시 사이클
//	BUFFERTYPE				BufferType;				// 이적재 Buffer 위치
//	unsigned char			StopNodeID[3];			// 목적지노드
	unsigned char			StopStationID[3];		// 목적지 Station노드
//	RIV						Riv;					// 우선도
//	REASPONSEACK			ResponseAck;			// 응답/접수 불가 구분
} PROTOCOL_BODY_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111;


typedef struct
{
	unsigned char			ResponseType;			// 응답구분 ---추가
	unsigned char		  	PathType;				// 루트응답 구분 구분
	unsigned char			StopStationID[3];	// 목적지 Station노드

	//변경 후 Station 정보
	int						StationData_StationID;  //  추가
	unsigned char			StationData_MarkType;   // Target 이적재 Tag Type
	int     				StationData_ParentNodeID;   //추가
	int     				StationData_NextNodeID;      //추가
	short     				StationData_Offset;      //추가
	unsigned char           StationData_Safety_Level;
	int						StationData_PhysicalID;		// 2단 STB, 실제 물리적 Tag ID
	unsigned char			StationData_Reserved[3];
//	unsigned char			StationData_Reserved[7];    // Target 이적재 Tag Type
//	unsigned char			StationData_Reserved[8];    // Target 이적재 Tag Type

	int						TeachingData_StationID;  //  추가
	unsigned char			TeachingData_PortType;  //  추가
	int						TeachingData_Hoist;  //  추가
	int						TeachingData_Shift;  //  추가
	int						TeachingData_Rotate;  //  추가
	unsigned char			TeachingData_PIOType;  //  추가
	unsigned char			TeachingData_PIODirect;  //  추가
	int						TeachingData_PIOID;  //  추가
	int						TeachingData_PIOChannel;  //  추가
	unsigned char			TeachingData_PIOTimeLevel;  //  추가
	unsigned char			TeachingData_AutoRecovery;  //  추가
	unsigned char			TeachingData_HandDetectEQ;  //  추가
	int						TeachingData_STBOffset;  //  추가
	unsigned char			TeachingData_LookDownLevel;  //  추가
	unsigned char			TeachingData_SpecialEQLevel;  //  AOHC-294
	unsigned char			TeachingData_Reserved[7];    // Target 이적재 Tag Type

	//변경 전 Data
/*
    unsigned char			StationData_MarkType;   // Target 이적재 Tag Type
	unsigned char			StationData_PortID[12];    // Target 이적재 Tag Type
	int						StationData_RunDist;			// Target_RunDist
	int						TeachingData_Hoist;	// Node로부터 이적재 Tag까지 거리
	int						TeachingData_Shift;	// Node로부터 이적재 Tag까지 거리
	int						TeachingData_Rotate;	// Node로부터 이적재 Tag까지 거리

	unsigned char 	  		 TeachingData_FinalPortType;

	unsigned char			TeachingData_LookDownLevel;	// Node로부터 이적재 Tag까지 거리
	TEACHINGDATA_PIOINFO	TeachingData_PIOInfo;
	unsigned char			TeachingData_PIOTimeLevel;
	TEACHINGDATA_OPTION		TeachingData_Option;
	unsigned char			TeachingData_Reserved[8];
*/


	PATHDATA_DEF_DIRECTION	PathData_Def_Direction;		// 기본 분기방향
	unsigned char			PathData_StartNodeID[3];	// 개시번지
	unsigned char			PathData_EndNodeID[3];		// 종료번지
	unsigned short			PathData_ExtraNum;			//	기본외 분기 번지수

	// 이 항목 바로 앞까지 크기를 빼고, 정의된 크기에서 넣을 수 있는 최대의 수를 연산한다
	// 아래의 40은 앞 항목들의 Byte 크기. 변경되면 반드시 수정해 줄것
	unsigned char  	PathData_ExtraID[MAX_PACKET_SIZE - 121];
} PROTOCOL_BODY_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112;


//================================================================================================//



namespace AckType
{
	enum Reserved
	{
		Ack				= 0x00,
		Busy			= 0x01,
		ErrorState		= 0x02,
		DataLogicError	= 0x03,
	};
};

typedef struct
{
	unsigned short	RcvCmdID;			// 수신한 Command ID
	unsigned char	ProgressType;		// 진척구분
	// unsigned int	MoveDist; 			// 주행거리
	// unsigned char	Reserved[32];		//예약
	
} PROTOCOL_BODY_JOB_PROGRESS_REPORT_CLW_TO_MCP_ID_13; 	// CLW -> MCP 진척보고 (ID:13) //AOHC-297

typedef struct
{
	unsigned int	WarningMessageID;	 // 고장 메세지
	unsigned char	MessageData[64];	 // MessageData


} PROTOCOL_BODY_JOB_WARNING_REPORT_CLW_TO_MCP_ID_29; 	// OHT -> OCS 경고장 보고 (ID:29)   //AOHC-296


typedef struct
{
	unsigned short	SQNum;					// 시퀀스 넘버
	unsigned char	StopNodeID[3];			// STOP 번지
	unsigned char	ResetNodeID_Main[3];	// RESET 번지(Main)
	unsigned char	ResetNodeID_Sub[3];		// RESET 번지(Sub)

}PASSPERMIT_COMMON;


typedef struct
{
	unsigned char	StopNodeID[3];			// STOP 번지
	unsigned char	ResetNodeID_Main[3];	// RESET 번지(Main)
	unsigned char	ResetNodeID_Sub[3];		// RESET 번지(Sub)
}FORCE_PASSPERMIT_COMMON;
typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;	
	unsigned char		PassDelayTime[3];		// 통과대기시간

} PROTOCOL_BODY_PASSPERMIT_OHT2OCS_ID_241;		// 통과 허가 요구 (ID:241)


typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;
	unsigned char	AckType;				// 응답 구분
	unsigned char	ResultType;				// 결과 구분

} PROTOCOL_BODY_RESPONSE_PASSPERMIT_OCS2OHT_ID_242;	// 통과 허가 요구 응답 (ID:242)


typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;

} PROTOCOL_BODY_CMD_PASSPERMITOPEN_OHT2OCS_ID_243;		// 통과 허가 개방 요구 (ID:243)


typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;

} PROTOCOL_BODY_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244;	// 통과 허가 개방 요구 응답 (ID:244)


typedef struct
{
	// protocol is blank (0 byte!)

} PROTOCOL_BODY_CMD_PASSPERMITREQ_OCS2OHT_ID_245;		// 통과 허가 문의 (ID:245)


typedef struct
{
	BYTE	Finish_PassPermitNum;	// 통과허가 취득이 끝난 건수
	PASSPERMIT_COMMON	PassPermit_CM[10];
} PROTOCOL_BODY_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246;	// 통과 허가 상태 보고 (ID:246)
					// OHT의 통과허가에 대한 상태를 OCS에 보고함(통과허가문의에 대한 응답) 							

typedef struct
{
	FORCE_PASSPERMIT_COMMON ForcePassPermit;

} PROTOCOL_BODY_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247;	// 통과 허가 강제 개방 요구 (ID:247)
														//OHT가 OCS에게 강제통과허가 요청
														
typedef struct
{
	FORCE_PASSPERMIT_COMMON ForcePassPermit;
	unsigned char   Reserved[6];				//미사용
	unsigned char	AckType;				// 응답 구분
	unsigned char	ResultType;				// 결과 구분
} PROTOCOL_BODY_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248;	// 통과 허가 강제 개방 응답 (ID:248)
											// OHT의 강제통과허가요청에 대한 OCS의 응답								

typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;
	unsigned char   Reserved[9];
	unsigned char	CIDResultType;				// 결과 구분
	unsigned char   CIDStatus[3];
} PROTOCOL_BODY_CID_ERROR_REPORT_OHT2OCS_ID_249;	// CID 이상보고 (ID:249)

typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;
	unsigned char   Reserved[9];
	unsigned char	AcOrder;				// 응답 구분
	unsigned char   Reserved2[43];
} PROTOCOL_BODY_MOVE_RESTART_OCS2OHT_ID_250;	// 주행 재개(ID:250)

//AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현
typedef struct
{
	AUTOCHANGEACK	AutoChangeACK;
} PROTOCOL_BODY_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252;	// 자동오토전환 (ID:252)

typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;
} PROTOCOL_BODY_PASSPERMIT_CANCEL_OCS2OHT_ID_270;	// 통과 허가 상태 보고 (ID:246)


typedef struct
{
   //	FORCE_PASSPERMIT_COMMON ForcePassPermit;
	PASSPERMIT_COMMON	PassPermit_CM;
	unsigned char	AckType;				// 응답 구분
	unsigned char	ResultType;				// 결과 구분
} PROTOCOL_BODY_RESPONSE_PASSPERMIT_CANCEL_OHT2OCS_ID_271;	// 통과 허가 강제 개방 응답 (ID:248)

typedef struct
{
	OHTREQUEST				Request;			
} PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS;
// 데이터 전송 요구 (OHT -> OCS)


typedef struct
{
	OHTREQUEST				Request;
	unsigned char			OrderID;
	unsigned char			IpAddress[4];
	char					UserName[17];		// 유저명
	char					PassWord[17];		// 패스워드
	char					Version[33];		// 버전
	char					DataSendTo[256];	// 데이터 전송처
} PROTOCOL_BODY_DATA_TRANSPER_ORDER_OCS2OHT;
// 데이터 전송 요구 (OCS -> OHT)


typedef struct
{
	OHTREQUEST				Request;	
	TRANSORDERACK			Ack;
} PROTOCOL_BODY_DATA_TRANSPER_ORDER_RESPONSE_OHT2OCS;
// 데이터 전송 지시 응답								


typedef struct
{
	OHTREQUEST				Request;	
	TRANS_ORDER_RESULT		Result;
	short int				FailClass;	// 데이터 전송실패구분 						
} PROTOCOL_BODY_DATA_TRANSPER_PROGRESS_OHT2OCS;
// 데이터 전송 진척 보고								

typedef struct
{
	char					Version[512];	
} PROTOCOL_BODY_OHT_VERSION_OHT2OCS;


typedef struct
{
	char					RailNumber;			
} PROTOCOL_BODY_OHT_INPUT_REQUEST;

typedef struct
{
	char					Ack;
} PROTOCOL_BODY_OHT_INPUT_REQUEST_RESPONSE;


typedef struct
{
	unsigned short			OrderID;			
}
PROTOCOL_BODY_FLOOR_MOVE_ORDER;


typedef struct
{
	unsigned short			OrderID;
	REASPONSEACK			AckClass;		    
}
PROTOCOL_BODY_FLOOR_MOVE_ORDER_RESPONSE;


typedef struct
{
	char AppMonitorVersion[32];
	char ApplicationVersion[32];
	char ParameterVersion[32];
	char MapVersion[32];
}
PROTOCOL_BODY_VERSION_RESPONSE;


typedef struct
{
	unsigned short		CmdID;		    	// 지시 ID
	CMDDIVIDE		    CmdDiv;		    	// 지시구분
	unsigned char		CmdCycle;	    	// 지시 사이클
	unsigned char		StopStationID[3];	// 목적지 Station

}
PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301;


typedef struct
{
	unsigned short		CmdID;		    	// 지시 ID
	CMDDIVIDE		    CmdDiv;		    	// 지시구분
	unsigned char		CmdCycle;	    	// 지시 사이클
	unsigned char		StopStationID[3];	// 목적지 Station

    unsigned char       Response;           // Carrier 요청 응답 확인
    unsigned char       CarrierID[64];      // 요청 Carrier의 ID

}
PROTOCOL_BODY_RESPONSE_CARRIERINFO_OCS_OHT_1302;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 프로토콜 기본 포멧 PACKET 내용  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////




typedef enum TAGTYPE_
{
	TAGTYPE_PASSPERMIT_RESPONSE      =1,
	TAGTYPE_PASSPERMIT_OPEN_RESPONSE,
	TAGTYPE_FORCE_PASSPERMIT_OPEN_RESPONSE,
	TAGTYPE_PASSPERMIT_REQ,
    TAGTYPE_JOBORDER_TARGETINIT,  //작업변경에 의해 타겟에 대한 싸이클 초기화->이동싸이클로만 변경시
	TAGTYPE_PASSPERMIT_CANCEL, //soming

	TAGTYPE_TOUR_MODE = 11,  //투어 모드 변경
	TAGTYPE_TOUR_TARGETINIT,  //작업변경에 의해 타겟에 대한 싸이클 초기화->투어싸이클로만 변경시
    TAGTYPE_TOUR_STATIONMOVEINFO,

	TAGTYPE_AUTOMODE = 21,  //우선순위 처리에서 밀림. 통과허가 뒤로.
	TAGTYPE_RESET,
	TAGTYPE_DATATRANSFER,
	TAGTYPE_INFOREQ,
	TAGTYPE_JOBORDER_JOBCHAGE,
	TAGTYPE_JOBORDER_TARGETCHAGE,
	TAGTYPE_JOBORDER_PATHCHAGE,
	TAGTYPE_JOBORDER_JOBCLEAR,
	TAGTYPE_JOBORDER_JOBCANCEL,
	TAGTYPE_MANUALMODE,

	TAGTYPE_JOBORDER = 31,
    TAGTYPE_STATIONMOVEINFO,
	TAGTYPE_FLOORMOVEMENT,

	TAGTYPE_MANUAL_COMMON = 41,

	TAGTYPE_AUTOCHANGE = 51 // jhoun.lim

	

}TAGTYPE;



/**
@brief   CYCLETYPE 정보를 위한 구조체
@author  puting
@date    2015.10.31
*/

typedef enum CYCLETYPE_
{
	CYCLETYPE_DRIVING 	= 1,		//통과허가 요청이 필요한 주행
	//CYCLETYPE_DRIVING_FINAL,			//Mark or Target 으로가는 주행
	CYCLETYPE_TRANS,
	CYCLETYPE_MANUAL
}CYCLETYPE;

/**
@brief   CYCLETYPE 정보를 위한 구조체
@author  puting
@date    2015.10.31
*/

typedef enum CYCLENODEINFO_
{
	CYCLENODETYPE_START2STOP 	= 1,		//Start Node에서 Stop Node 
	CYCLENODETYPE_STOP2STOP,			    //Stop Node에서 Next Stop Node	
	CYCLENODETYPE_STOP2TARGET,				//Stop Node에서 Target Node	
	CYCLENODETYPE_START2TARGET				//Start Node에서 Target Node	
	
}CYCLENODEINFO;


/**
@brief   CYCLESTEP 정보를 위한 구조체
@author  puting
@date    2015.10.31
*/

typedef enum CYCLESTEP_
{

	CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE = 1,
	CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE,
	CYCLESTEP_DRIVING_MEANINGLESS
/*
	CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE =1,
	CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE,
	CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE,
	CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE,
	CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_OPEN_RESPONSE,
	CYCLESTEP_DRIVING_MEANINGLESS
 */
 //	CYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE,   //통과허가 응답을 받은 후
 //	CYCLESTEP_DRIVING_AFTER_NEXTLINK_PASSPERMIT_RESPONSE     //통과허가 개방까지 응답을 받은 후

	//CYCLESTEP_TRANS_BEFOER_PERMIT,
	//CYCLESTEP_TRANS_AFTER_PERMIT,
	//CYCLESTEP_MANUAL
}CYCLESTEP;

/**
@brief   NextCYCLESTEP 정보를 위한 구조체
@author  puting
@date    2015.10.31
*/

typedef enum NEXTCYCLESTEP_
{
	//NEXTCYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE =0,
	NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE = 1,
	NEXTCYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_OPEN_RESPONSE,
	NEXTCYCLESTEP_DRIVING_MEANINGLESS

//	NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE,   //통과허가 응답을 받은 후
//	NEXTCYCLESTEP_DRIVING_AFTER_NEXTLINK_PASSPERMIT_RESPONSE,     //통과허가 개방까지 응답을 받은 후
//	NEXTCYCLESTEP_DRIVING_MEANINGLESS

	//CYCLESTEP_TRANS_BEFOER_PERMIT,
	//CYCLESTEP_TRANS_AFTER_PERMIT,
	//CYCLESTEP_MANUAL
}NEXTCYCLESTEP;
// COMMON Protocol Packet -----------------------------------------------


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
} PKT_CHECK_TAG;								// 프로토콜 첫 해석에서 TAG 변수의 위치를 찾기 위해 정의한 것임.

void Conv_PKT_CHECK_TAG(PKT_CHECK_TAG& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_STATUS_OCS							STATUS;
	unsigned char								Value[MAX_PACKET_SIZE-sizeof(PROTOCOL_HEADER)-sizeof(PROTOCOL_TAG)-sizeof(PROTOCOL_STATUS_OCS)];
} PKT_OCS2OHT;

void Conv_PKT_OCS2OHT(PKT_OCS2OHT& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_STATUS_OHT							STATUS;
	unsigned char								Value[MAX_PACKET_SIZE-sizeof(PROTOCOL_HEADER)-sizeof(PROTOCOL_TAG)-sizeof(PROTOCOL_STATUS_OHT)];
} PKT_OHT2OCS;

void Conv_PKT_OHT2OCS(PKT_OHT2OCS& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
	// OHT Status
	ConvertEndian(&Pkt.STATUS.ErrorCode);
	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
}


/**
@brief   Status(OHT->TP) 구조체
@author  puting
@date    2012.11.19
@note    Input, Output의 경우 Light-On, Dark-On을 고려할 필요 없음
*/
#if 0
typedef struct DEF_STATUS_OHT2TP_
{
    OHT_COMMON_STATUS Data; ///< OHT 기본 정보

    // HID 정보
    WORD HID_Erec;          ///< HID 2차측 입력전압
    WORD HID_Edc;           ///< HID 2차측 출력전압
    WORD HID_Idc;           ///< HID 2차측 전류
    WORD HID_Ecap;          ///< HID 캡 뱅크 전압
    WORD HID_Kw;            ///< HID 출력 파워
    WORD HID_ErrorCode;     ///< HID Error Code
    WORD HID_PickupTemper;  ///< HID Pick up 온도
    WORD HID_CPUTemper;     ///< ???

    // IO 정보
    UINT Input[3];          ///< OHT Input Data #1~3
    UINT Output[3];         ///< OHT Output Data #1~3  
    
    // 추가 정보 
    // OHT Use Info
    uCMD_USE_DATA UseInfo;  ///< Use OHT Data(유니온)

    // Motor 관련 정보
    MOTOR_INFO Driving_F;   ///< 주행축 Front
    MOTOR_INFO Driving_R;   ///< 주행축 Rear
    MOTOR_INFO Hoist;       ///< Hoist 축
    MOTOR_INFO Shift;       ///< Shift 축
    MOTOR_INFO Rotate;      ///< Rotate 축

    // UBG 관련
    BYTE IsUBGSen;          ///< UBG Sensor 유무
    BYTE UBGDetectLevel;    ///< UBG Sensor Level

    // PBS 관련
    BYTE IsPBSSen;          ///< PBS Sensor 유무
    BYTE PBSDetectlevel;    ///< PBS Sensor Level

    BYTE IsFCPause;         ///< FC Pause
    UINT TPCommandType;     ///< TP CommandType 4자리
    BYTE Cmd_Count;         ///< Cmd 개수

    BYTE AMCResponse;       ///< AMC Response
//    UINT MapMakeData;      ///< Map Make Data(단위 : mm)
	short MapMakedLinkCount;      			  ///< Map Make 진행된 갯수
    short MapMakedLinkDrivingCheckCount;      ///< Map Make 주행검증된 진행된 갯수
	BYTE DownPercent;       ///< Down Percent
	double CPUTemperature;  ///< IPC 온도
    UINT APSignalLevel;     ///< 와이파이 신호

    // Log 관련 정보
    BYTE isLogDrive;        ///< Log Drive 유무

    BYTE STBDirection;      ///< STB Direction
    BYTE IsRotateExist;     ///< Rotate Exist
    MAPDATALOADINFO AdjustMapLoadInfo;       ///< 조정용 Map 정보
	MAPMAKE_STATE_INFO MapMakeState;     ///< MapMaker 상태 정보
	BYTE IsErrorIgnolState;     ///< FBWF 유무  ->ErrorIgnorstate     //AOHT-0001_FOUPCOVER
	FLAG_STATE_INFO IsWarningState;       ///< RF PIO Type       //AOHC-285

    // by zzang9un 2013.03.29 : 버전 정보 관련 구조체
    uVERSION_DATA OHTVer;   ///< OHT Version
	uVERSION_DATA AMCVer;   ///< AMC Version
	uVERSION_MAP  MapVer;

    // by zzang9un 2013.03.29 : Mark 정보를 위한 구조체 추가
    uMARK_INFO MarkInfo;    ///< Mark 정보를 위한 구조체

    // by zzang9un 2013.07.22 : Data 로드 정보 추가
    uDATALOADED DataLoaded; ///<  Data 로드 정보
} DEF_STATUS_OHT2TP;
#endif

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	MSG_STATUS_OHT2TP							OHT_STATUS;	
	uTP_CMD										OHT_BODY;
} PKT_OHT2TP_RESPONSE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	MSG_STATUS_OHT2VMS						OHT_STATUS;
} PKT_OHT2VMS_RESPONSE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	MSG_STATUS_OHT2BLACKBOX						OHT_STATUS;
} PKT_OHT2BLACKBOX;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	SYSTEMTIME 									TIME;
} PKT_BLACKBOX_TIME;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	BYTE 										EventType;
} PKT_OHT2BLACKBOX_EVENT;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	MSG_EVENT_BLACKBOX2OHT 						EVENT;
} PKT_BLACKBOX2OHT_EVENT;

// -----------------------------------------------------------------------------
//  TP에서 OHT의 IO리스트를 질의하면 OHT는 이 패킷에 리스트를 기입하여 전송한다
//  2016.06.15 shkim. added
// -----------------------------------------------------------------------------
typedef struct
{
	int	 iOhtInputNum;		// IO리스트 갯수
	int  iOhtOutputNum;
	char cIONameList[MAX_BIG_PACKET_SIZE-sizeof(PROTOCOL_HEADER)-sizeof(PROTOCOL_TAG)-(sizeof(int) * 2)];	// IO리스트 이름. 인풋 이름이 차례대로 나열되고, 아웃풋 이름이 그다음으로 나열된다.
							// 구분자는 '\0' 이다.
}
PROTOCOL_BODY_IO_LIST_OHT_2_TP_RESPONSE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_IO_LIST_OHT_2_TP_RESPONSE		BODY;	
} PKT_OHT2TP_IOLIST;


// -------------------------------------------------------------
typedef struct
{
	unsigned char			OHTID[6];
	unsigned char			CIDO_FW_Ver[3];			
	unsigned char			CIDR_FirstLinkLevel[3];	// CID-R 첫 광 송신(Link) 신호 세기
	unsigned char			CIDO_FiberCommError[2];	// 통신 중 광 통신 오류(%)
	unsigned char			CIDO_RFCommError[2];	// 통신 중 무선 통신 오류(%)
	unsigned char			delimiter;				// CID-O와 CID-R 데이터 구분
	unsigned char			CID_UniqueID[6];		// CID-고유 ID
	unsigned char			CIDR_FW_Ver[3];		// CID-R F/W Version
	
	unsigned char			RF_ID[5];				// added (2016.10.30)
	unsigned char			RF_CH[4];				// added (2016.10.30)
	unsigned char			CIDR_MAXCurrent[3];		// 통신 중 CID-R 최대 전류
	unsigned char			CIDR_MAXVoltage[3];		// 통신 중 CID-R 최대 전압
	unsigned char			CIDR_MAXTemp[2];		// 통신 중 CID-R 최대 온도
	unsigned char			CIDR_StandbyFiberNoise[3];	// 대기 상태 CID-R 광 노이즈 수신값
	unsigned char			CIDO_FirstLinkLevel[3];	// CID-O의 첫 광 송신(Link) 신호 세기
	unsigned char			FiberError_Per[2];		// 통신 중 광 통신 오류(%)
	unsigned char			RFError_Per[2];			// 통신 중 무선 통신 오류(%)											// 구분자는 '\0' 이다.
}
PROTOCOL_BODY_CID_INFO_OHT_2_TP_RESPONSE;


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_CID_INFO_OHT_2_TP_RESPONSE	BODY;	
} PKT_OHT2TP_CID_INFO;

// IO의 값을 OnOff하거나 0,1을 세팅하는 패킷이다
// (아래 두 프로토콜이 동일한 패킷 정의를 사용함)
/* TagID
	CMD_IOMAP_USE_ONOFF_TP2OHT				= 25232, // TP에서 IO의 ON, OFF를 행한다
	CMD_IOMAP_SET_RESET_TP2OHT				= 25233, // TP에서 IO의 1, 0 세팅을 행한다

   TEXTID
	IOMAP_USE_ONOFF_TP2OHT					= 232,	// TP에서 IO의 ON, OFF를 행한다
	IOMAP_SET_RESET_TP2OHT	
*/
typedef struct
{
	int	 iIoNum;			// IO리스트 no
	int  iOnOff;			// 구분자는 '\0' 이다.
}
PROTOCOL_BODY_IO_ONOFF_OHT_2_TP;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_IO_ONOFF_OHT_2_TP				BODY;	
} PKT_OHT2TP_IO_SET;

// -------- 티칭값 SAVE 패킷정의 (티칭값 LOAD 패킷정의와 공용) ---------
// TagID  : CMD_TEACHING_INFO_SAVE = 25234, // TP에서 티칭후 OHT로 티칭값을 전송한다
// TextID : TEACHING_INFO_SAVE     = 234,
// -----------------------------------------------------------------------------
typedef struct SUB_BODY_TEACHING_INFO_ETC_
{
    UINT PIODirection   : 4; ///< PIO 방향(Left, Right)
    UINT PIOTimeLevel   : 8; ///< PIO Time Level
	UINT LookDownLevel  : 8; ///< LookDownLevel
	UINT SpecialEQLevel : 4; ///< SpecialEQLevel
    UINT Reserved2      : 1; ///< 
    UINT Reserved1      : 1; ///< 
    UINT EQPIODisable_TP: 1; ///< EQPIO Disable
    UINT HandDetectEQ   : 1; ///< Hand Detect EQ 여부
    UINT Oscillation    : 1; ///< Oscillation On/Off
    UINT TransSound     : 1; ///< 이적재 소리 On/Off
    UINT RFReader       : 1; ///< RF Read 여부
    UINT AutoRecovery   : 1; ///< Auto Recovery 여부
} SUB_BODY_TEACHING_INFO_ETC;


typedef union SUB_BODY_uTEACHING_INFO_ETC_
{
    SUB_BODY_TEACHING_INFO_ETC Info;
    SUB_BODY_TEACHING_INFO_ETC Data;
} SUB_BODY_uTEACHING_INFO_ETC;


typedef struct
{
	BYTE CmdID;                 			///< Command ID
	int  PortType;              			///< Port Type
	int  Hoist;
	int  Shift;
	int  Rotate;
	BYTE PIOType;
	UINT PIOChannel;
	UINT PIOID;
	SUB_BODY_uTEACHING_INFO_ETC EtcData;  	///< 기타 Teaching 정보
	//int  Reserved1;             			///< Reserved1
	BYTE  PIOCS;             			     //< PIO Carrier Stage
	BYTE MaterialType;
	BYTE  Reserved[6];        			///< Reserved
} PROTOCOL_BODY_TEACHING_SAVE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_TEACHING_SAVE					BODY;
} PKT_OHT2TP_TEACHING_INFO;


// -------- 오와시스값을  SAVE 패킷정의 (패킷정의와 공용) ---------
// TagID  : CMD_OACIS_INFO_SAVE = 25256, // TP에서 OHT로 티칭값을 전송한다
// TextID : OACIS_INFO_SAVE     = 256,
typedef struct
{
	short  Type;              			///< Type
	short  TypeNum;              		///< Type Num
	int  nData1;                        ///< 끝노드
	int  nData2;                        ///< 변경 거리값
	int  nData3;             			///< Reserved1
	int  nData4;             			///< Reserved2
} PROTOCOL_BODY_OACIS_SAVE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_OACIS_SAVE				BODY;
} PKT_OHT2TP_OACIS_INFO;



// -------- Window OS 종료 버튼패킷정의 (패킷정의와 공용) ---------
// TagID  : CMD_WINDOW_OS_SHUTDOWN		= 25257, // Window OS 종료 버튼을 전송한다.
// TextID : WINDOW_OS_SHUTDOWN     = 257,
typedef struct
{
	short  Type;              			///< Type
	short  TypeNum;              		///< Type Num
	int  nData1;                        ///< Reserved
	int  nData2;                        ///< Reserved
	int  nData3;             			///< Reserved
	int  nData4;             			///< Reserved
} PROTOCOL_BODY_WINDOW_OS_SHUTDOWN;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_WINDOW_OS_SHUTDOWN		  	BODY;
} PKT_OHT2TP_WINDOW_OS_SHUTDOWN_INFO;




// -------- 주행 Map을 SAVE 패킷정의 (패킷정의와 공용) ---------
// TagID  : CMD_DRIVINGMAP_INFO_SAVE = 25253, // TP에서 OHT로 티칭값을 전송한다
// TextID : DRIVINGMAP_INFO_SAVE     = 253,
typedef struct
{
	int  nStartNodeID;              		///< 시작노드
	int  nEndNodeID;                        ///< 끝노드
	int  nMapMakeDist;                      ///< 변경 거리값
	int  Reserved1;             			///< Reserved1
	int  Reserved2;             			///< Reserved2
} PROTOCOL_BODY_DIRIVINGMAP_SAVE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_DIRIVINGMAP_SAVE				BODY;
} PKT_OHT2TP_DRIVINGMAP_INFO;



// -----------------------------------------------------------------------------
// 재하 LOAD (장비로 내려놓음), 재하 UNLOAD (장비에서 꺼내 OHT로 이동),
// TEACHING POINT로 이동하도록 지시하는 명령패킷
// -----------------------------------------------------------------------------
typedef struct 
{
    BYTE Reserved2      : 1;    ///<
    BYTE Reserved1      : 1;    ///<
    BYTE EQPIODisable_TP: 1;    ///< TP에서 EQPIO를 끄는 옵션
    BYTE HandDetectEQ   : 1;    ///< Hand Detect EQ 여부
    BYTE Oscillation    : 1;    ///< Oscillation On/Off
    BYTE TransSound     : 1;    ///< 이적재 소리 On/Off
    BYTE RFReader       : 1;    ///< RF Read 여부
    BYTE AutoRecovery   : 1;    ///< Auto Recovery 여부
} TRANS_OPT;

typedef union 
{
    TRANS_OPT    Info;   ///< 기타 이적재 옵션
    BYTE         Data;   ///< Data
} uTRANS_OPT;

typedef struct 
{
    BYTE CmdID;                 ///< Command ID
    UINT CurNodeID;             ///< 이적재 Tag가 속해 있는 DrivingNodeID ID
    UINT CurStationID;          ///< 이적재 Node ID
    BYTE PortType;              ///< 작업 구분
    BYTE CarrierType;           ///< 반송물 구분
    BYTE PIODirection;          ///< PIO Direction
    int  Tposition_Hoist;       ///< Hoist Teaching 값
    int  Tposition_Shift;       ///< Shift Teaching 값
    int  Tposition_Rotate;      ///< Rotate Teaching 값
    BYTE HoistSpeedLevel;       ///< Hoist 속도 레벨(50~100%)
    BYTE ShiftSpeedLevel;       ///< Shift 속도 레벨(50~100%)
    BYTE PIOTimeLevel;          ///< 포트 별 EQ PIO Timeover 레벨(1~10)
	BYTE LookDownLevel;         ///< Lookdown Sensor Level(1~16)
	BYTE SpecialEQLevel;         ///< SpecialEQLevel(0~9)
	uTRANS_OPT TransOption;  ///< 이적재 옵션 셋팅값
	BYTE BufferType;			///< 이적재시 Buffer 위치
	UINT PIOID;             	///< RF PIO ID
	BYTE PIOCS;					///< RF PIO CS 신호 선택
	BYTE PIOChannel;		  	///< RF PIO PIOChannel (미사용시  0 )
	BYTE MaterialType;
	BYTE PIOType;               ///< PIO Type 정의 사용(신규로 정의된 항목만 사용)

} PROTOCOL_BODY_TRANS_INFO;


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_TRANS_INFO					BODY;	
} PKT_TP2OHT_TRANS_LOAD_UNLOAD;




typedef struct 
{
	int iVar1;
	int iVar2;
	int iVar3;
} PROTOCOL_BODY_SIMPLE_COMMAND;


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_SIMPLE_COMMAND				BODY;	
} PKT_TP2OHT_SIMPLE_COMMAND;

// -------- HOIST 티칭관련 기능을 위한 패킷정의 ----------
typedef struct 
{
	double	dPulse;		///< Pulse 거리 단위
	double	dUserUnit;	///< 사용자 거리 단위
} HOIST_TEACH_VALUE;

typedef struct 
{
	HOIST_TEACH_VALUE PositionValue[10];
	
	double dHomeOffsetOfRailToJigOnUserUnit;
	double dHomeSearchOffset;

	double dIdealGearRatio;
	double dIdeal2RealRatio;
	double dHomeOffsetOfMTLToJigOnUserUnit;
} PROTOCOL_BODY_HOIST_TEACH_COMMAND;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_HOIST_TEACH_COMMAND			BODY;	
} PKT_TP2OHT_HOIST_TEACH_COMMAND;

typedef struct
{
	BYTE MaterialType;
} PROTOCOL_BODY_CHECK_MATERIAL;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_CHECK_MATERIAL			    BODY;
} PKT_TP2OHT_CHECK_MATERIAL;

// -------------------------------------------------

typedef struct 
{
	char 	Log[512];
	
	int 	iResult;
	
	int 	iVar1;
	int 	iVar2;
	int 	iVar3;
	int 	iVar4;
} PROTOCOL_BODY_LOG_DATA;


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_LOG_DATA						BODY;	
} PKT_OHT2TP_LOG_DATA;


#define MAP_MAKE_RESULT_SUCCESS			0
#define MAP_MAKE_RESULT_FAIL   	   	   -1
#define MAP_MAKE_NOT_FOUND_NODE_ID 	   -2

// -------------------------------------------------

// TP에서 OHT로 이동명령 (목적지만 지정하고 MAP정보 사용)
typedef struct 
{
	bool IsGoStation;		    // Target => Node : 0, Station : 1
	int  StartNodeID;			// 개발중 테스트 시 값을 편하게 입력하는 용도임
	int  StopNodeID;			// Target Node No
    int  PortType;              // Port Type ;
                                // 0x01 : EQ, 0x02 : EQ_X, 0x03 : STK, 0x04 : Loader,
								// 0x05 : STB_L, 0x06 : STB_R, 0x07 : UTB

	BYTE IsMapMaker;            //Map Maker Flag , 0x00 일반 주행, 0x01: MapMaker, 0x02: MapMaker 주행 검증
	BYTE IsSteerCheck;          //Map Maker 시, Steer Check 기능 사용 유무
	BYTE nCycle; // Move 2, Load 3, Unload 4
} PROTOCOL_BODY_MOVE_BY_MAP;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_MOVE_BY_MAP					BODY;	
} PKT_TP2OHT_MOVE_BY_MAP;

// -----------------------------------------------------------------------------

// TP에서 OHT로 2단 STB 작업 명령
typedef struct
{
	BYTE RequestType;		    // Move: 0, Load: 1, Unload: 2
	int  StopStationID;			// Logical Tag ID
} PROTOCOL_BODY_DUAL_STAGE_STB_TRANS;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_DUAL_STAGE_STB_TRANS					BODY;
} PKT_TP2OHT_DUAL_STAGE_STB_TRANS;

// -----------------------------------------------------------------------------

typedef struct 
{
	int	 iResult;		    

} PROTOCOL_BODY_MOVE_BY_MAP_RESULT;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_MOVE_BY_MAP_RESULT			BODY;	
} PKT_OHT2TP_MOVE_BY_MAP_RESULT;

#define MAP_MOVE_NO_ERR					0
#define MAP_MOVE_ERR_NOT_MARKED			-1
#define MAP_MOVE_ERR_MAP_SEARCH_FAIL	-2
#define MAP_MOVE_ERR_NOT_STATION_INFO   -3

// ---------------------------------------
//	TP Command Debug Support
// ---------------------------------------
// Command (TP -> OHT)
#define TP_DEBUG_START			0
#define TP_DEBUG_STOP				1

// Result (OHT -> TP)
#define TP_DEBUG_PROGRESS			0
#define TP_DEBUG_ERR				1
#define TP_DEBUG_ENDED			2



typedef struct
{
	PKT_OHT2OCS                                 AckPkt;
	UINT 										Length;
} PKT_ACK_OHT2OCS;								// 프로토콜 첫 해석에서 TAG 변수의 위치를 찾기 위해 정의한 것임.

void Conv_PKT_ACK_OHT2OCS(PKT_ACK_OHT2OCS& Pkt)
{
	ConvertEndian(&Pkt.AckPkt.HEADER.MachineType);
	ConvertEndian(&Pkt.AckPkt.TAG.Value);
	// OHT Status
	ConvertEndian(&Pkt.AckPkt.STATUS.ErrorCode);
	ConvertEndian(&Pkt.AckPkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.AckPkt.STATUS.RunningSpeed);

	// Length
	ConvertEndian(&Pkt.Length);
}



/*
@brief   PKT_TASKCMDDATA 정보를 위한 구조체
@author  puting
@date    2015.10.31
*/

/*
	  - 작업지시 List는 일반 사이클은 명령과 경로/목적지 변경 사이클과 구분하여 저장
	  - 우선 순위는 하기와 같다.
		 우선처리 >> 작업지시(작업취소, 목적지, 경로 변경) >= 통과허가 명령 > 정보요구 or 데이터 전송 >> 작업지시(작업지시) = 층간 이동 지시 및 NextCMD > Manual 동작

		우선처리 명령 (1~10)
			//-------------대기가 필요하거나, 전처리 과정이 필요한 우선처리 명령---------//
			1 : 수동 전환( 메뉴얼로 동작)
			2 : 자동 전환(메뉴얼에서만, 메뉴얼로 동작)
			//3 : Soft Reset(메뉴얼동작에서만 동작가능) 기존대로 QUICK으로 처리

			3 : 작업지시 中 작업변경
			4 : 작업지시 中 목적지 변경
			5 : 작업지시 中 경로 변경
			6 : 작업지시 中 작업 취소/클리어 변경
			7 : 강제 통과허가 개방 응답(메뉴얼 동작에서만 동작 가능)
			8 : 통과허가 개방 응답
			9 : 통과허가 응답

		정보요구 및 Data 전송(11~40)
			11 : 통과허가 문의
			12 : Data 전송 지시
			13 : 정보 요구

		///--------------------CMD 대기가 필요한 명령들----------------------//
		Task(CMD_NextCmd 포함)(31~40)
			31 : 작업 지시
			32 : 층간이동 지시

		기타 메뉴얼_동작(41~)
			41 : 메뉴얼 작업지시
			42 : 메뉴얼 공통( 조그, IO 동작)
*/
typedef struct
{
	TAGTYPE									DetailTagSort; //세부 구분자 : 작업지시중 목적지 변경도 포함되어서....
	STANDARDCATEGORY 						CategoryDiv;
	PKT_OCS2OHT								PKTDATA;
} PKT_TASKCMDDATA;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 프로토콜 구조체 정의 내용 ID 별로 구분
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// OCS Using Protocol Packet ---------------------------------------------
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;	
} PKT_OCS_STATUS_ID_01; 						// MCP -> CLW 상태보고 (ID:01)

void Conv_PKT_OCS_STATUS_ID_01(PKT_OCS_STATUS_ID_01& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}


// OHT Station 정보 요구 (OCS -> OHT)  20161022
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;
	PROTOCOL_BODY_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111		BODY;
} PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111;

void Conv_PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111(PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121					BODY;
} PKT_JOB_ORDER_ID_121;							// MCP -> CLW 작업지시 (ID:121)

void Conv_PKT_JOB_ORDER_ID_121(PKT_JOB_ORDER_ID_121& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.BODY.CmdID);
	// ConvertEndian(&Pkt.BODY.StationData_Start_RunDist);
	ConvertEndian(&Pkt.BODY.StationData_StationID);
	ConvertEndian(&Pkt.BODY.StationData_ParentNodeID);
	ConvertEndian(&Pkt.BODY.StationData_NextNodeID);
	ConvertEndian(&Pkt.BODY.StationData_Offset);
	ConvertEndian(&Pkt.BODY.StationData_PhysicalID);

	ConvertEndian(&Pkt.BODY.TeachingData_StationID);
	ConvertEndian(&Pkt.BODY.TeachingData_Hoist);
	ConvertEndian(&Pkt.BODY.TeachingData_Shift);
	ConvertEndian(&Pkt.BODY.TeachingData_Rotate);
	ConvertEndian(&Pkt.BODY.TeachingData_PIOID);
	ConvertEndian(&Pkt.BODY.TeachingData_PIOChannel);
	ConvertEndian(&Pkt.BODY.TeachingData_STBOffset);

	ConvertEndian(&Pkt.BODY.PathData_ExtraNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_RESPONSE_PASSPERMIT_OCS2OHT_ID_242			BODY;
} PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242;		// 통과 허가 요구 응답 (ID:242)

void Conv_PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242(PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244		BODY;
} PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244;	// 통과 허가 개방 요구 응답 (ID:244)

void Conv_PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244(PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_CMD_PASSPERMITREQ_OCS2OHT_ID_245				BODY;
} PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245;			// 통과 허가 문의 (ID:245)

void Conv_PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245(PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248	BODY;
} PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248;	// 통과 허가 강제 개방 응답 (ID:248)

void Conv_PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248(PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

//	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_MOVE_RESTART_OCS2OHT_ID_250					BODY;
} PKT_MOVE_RESTART_OCS2OHT_ID_250;	// 주행 재개  (ID:250)

void Conv_PKT_MOVE_RESTART_OCS2OHT_ID_250(PKT_MOVE_RESTART_OCS2OHT_ID_250& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_DATA_TRANSPER_ORDER_OCS2OHT					BODY;
} PKT_DATA_TRANSPER_ORDER_OCS2OHT;
												// 데이터 전송 지시	
void Conv_PKT_DATA_TRANSPER_ORDER_OCS2OHT(PKT_DATA_TRANSPER_ORDER_OCS2OHT& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

  //	ConvertEndian(&Pkt.BODY.IpAddress);
}
												

typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;
} PKT_OHT_VERSION_REQUEST;

void Conv_PKT_OHT_VERSION_REQUEST(PKT_OHT_VERSION_REQUEST& Pkt)	// OCS가 OHT의 버전 문의
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}

//정전복구 OCS -> OHT 요청
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;
} PKT_REQUEST_AUTOCHANGE_OCS_OHT_ID_251;

void Conv_PKT_REQUEST_AUTOCHANGE_OCS_OHT_ID_251(PKT_REQUEST_AUTOCHANGE_OCS_OHT_ID_251& Pkt)	// OCS가 OHT의 버전 문의
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}

// OHT 투입요구 응답 (OCS -> OHT)																				
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;
	PROTOCOL_BODY_OHT_INPUT_REQUEST								BODY;
} PKT_OHT_INPUT_REQUEST_RESPONSE;

void Conv_PKT_OHT_INPUT_REQUEST_RESPONSE(PKT_OHT_INPUT_REQUEST_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}

// 층간 이동지시 (OCS -> OHT)
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;
	PROTOCOL_BODY_FLOOR_MOVE_ORDER								BODY;
} PKT_FLOOR_MOVE_ORDER;

void Conv_PKT_FLOOR_MOVE_ORDER(PKT_FLOOR_MOVE_ORDER& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.BODY.OrderID);
}

// 버전문의 (OCS -> OHT) 
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;	
} PKT_SIMPLE_VERSION_REQUEST;


void Conv_PKT_SIMPLE_VERSION_REQUEST(PKT_SIMPLE_VERSION_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	
}

// OHT Using Protocol Packet ---------------------------------------------
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
} PKT_STATUS_REPORT_ID_03;						// CLW Status 보고 (CLW→MCP) (ID:03)

void Conv_PKT_STATUS_REPORT_ID_03(PKT_STATUS_REPORT_ID_03& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
} PKT_TAKEOUT_REPORT_ID_17;						// CLW 뺴냄 보고 (CLW→MCP) (ID:17)

void Conv_PKT_TAKEOUT_REPORT_ID_17(PKT_TAKEOUT_REPORT_ID_17& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
} PKT_DEF_REQ_ID_19;						//  주회주행 요청 (CLW→MCP) (ID:19)

void Conv_PKT_DEF_REQ_ID_19(PKT_DEF_REQ_ID_19& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}

//============================Station 정보 응답  =========================//
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112		BODY;
} PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112;							// MCP -> OHT(ID :112

void Conv_PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112(PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.BODY.StationData_StationID);
	ConvertEndian(&Pkt.BODY.StationData_ParentNodeID);
	ConvertEndian(&Pkt.BODY.StationData_NextNodeID);
	ConvertEndian(&Pkt.BODY.StationData_Offset);
	ConvertEndian(&Pkt.BODY.StationData_PhysicalID);

	ConvertEndian(&Pkt.BODY.TeachingData_StationID);
	ConvertEndian(&Pkt.BODY.TeachingData_Hoist);
	ConvertEndian(&Pkt.BODY.TeachingData_Shift);
	ConvertEndian(&Pkt.BODY.TeachingData_Rotate);
	ConvertEndian(&Pkt.BODY.TeachingData_PIOID);
	ConvertEndian(&Pkt.BODY.TeachingData_PIOChannel);
	ConvertEndian(&Pkt.BODY.TeachingData_STBOffset);

	ConvertEndian(&Pkt.BODY.PathData_ExtraNum);
}
//============================Station 정보 응답  MCP -> OHT(ID :112)=========================//
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_JOB_RESPONSE_CLW_TO_MCP_ID_122				BODY;

} PKT_JOB_ORDER_RESPONSE_ID_122;				// CLW -> MCP 작업지시 응답 (ID:122)

void Conv_PKT_JOB_ORDER_RESPONSE_ID_122(PKT_JOB_ORDER_RESPONSE_ID_122& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.CmdID);
}



typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_JOB_PROGRESS_REPORT_CLW_TO_MCP_ID_13			BODY;
} PKT_JOB_PROGRESS_REPORT_ID_13;				// CLW -> MCP 진척보고 (ID:103)  AOHC-297


void Conv_PKT_JOB_PROGRESS_REPORT_ID_13(PKT_JOB_PROGRESS_REPORT_ID_13& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.RcvCmdID);
}

//AOHC-296
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_JOB_WARNING_REPORT_CLW_TO_MCP_ID_29			BODY;
} PKT_JOB_WARNING_REPORT_ID_29;				// CLW -> MCP 경고장 (ID:29)

void Conv_PKT_JOB_WARNING_REPORT_ID_29(PKT_JOB_WARNING_REPORT_ID_29& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.WarningMessageID);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_PASSPERMIT_OHT2OCS_ID_241						BODY;
} PKT_PASSPERMIT_OHT2OCS_ID_241;				// 통과 허가 요구 (ID:241)

void Conv_PKT_PASSPERMIT_OHT2OCS_ID_241(PKT_PASSPERMIT_OHT2OCS_ID_241& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_CMD_PASSPERMITOPEN_OHT2OCS_ID_243				BODY;
} PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243;		// 통과 허가 개방 요구 (ID:243)

void Conv_PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246			BODY;
} PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246;	// 통과 허가 상태 보고 (ID:246)

void Conv_PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246(PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

//	ConvertEndian(&Pkt.BODY.Finish_PassPermitNum);

	//OHT Main 종료되는 부분 20160421 -------해결. puting
	for(int i=0;  i<Pkt.BODY.Finish_PassPermitNum; i++)
	{
		ConvertEndian(&Pkt.BODY.PassPermit_CM[i].SQNum);
	}

}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247		BODY;
} PKT_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247;	// 통과 허가 강제 개방 요구 (ID:247)

void Conv_PKT_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247(PKT_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

//	ConvertEndian(&Pkt.BODY.Finish_PassPermitNum);

}
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_CID_ERROR_REPORT_OHT2OCS_ID_249				BODY;
} PKT_CMD_CID_ERROR_REPORT_OHT2OCS_ID_249;		// CID 이상 보고 (ID:249)

void Conv_PKT_CMD_CID_ERROR_REPORT_OHT2OCS_ID_249(PKT_CMD_CID_ERROR_REPORT_OHT2OCS_ID_249& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}

//soming
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_PASSPERMIT_CANCEL_OCS2OHT_ID_270		        BODY;
} PKT_PASSPERMIT_CANCEL_OCS2OHT_ID_270;	// 통과 허가 취소(ID:270)

void Conv_PKT_PASSPERMIT_CANCEL_OCS2OHT_ID_270(PKT_PASSPERMIT_CANCEL_OCS2OHT_ID_270& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_RESPONSE_PASSPERMIT_CANCEL_OHT2OCS_ID_271	    BODY;
} PKT_RESPONSE_PASSPERMIT_CANCEL_OHT2OCS_ID_271;	// 통과 허가 취소 응답(ID:271)

void Conv_PKT_RESPONSE_PASSPERMIT_CANCEL_OHT2OCS_ID_271(PKT_RESPONSE_PASSPERMIT_CANCEL_OHT2OCS_ID_271& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS					BODY;
} PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS;
												// 데이터 전송 요구
												
void Conv_PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}

												
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_DATA_TRANSPER_ORDER_RESPONSE_OHT2OCS			BODY;
} PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE;

void Conv_PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}												// 데이터 전송 지시 응답								


typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_DATA_TRANSPER_PROGRESS_OHT2OCS				BODY;
} PKT_DATA_TRANSPER_ORDER_PROGRESS;
												// 데이터 전송 진척 보고

void Conv_PKT_DATA_TRANSPER_ORDER_PROGRESS(PKT_DATA_TRANSPER_ORDER_PROGRESS& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.FailClass);
}
		
//AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252		 	BODY;
} PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252;	// 오토전환   (ID:252)

void Conv_PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252(PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

}

 typedef struct
{
	PROTOCOL_HEADER					            	HEADER;
	PROTOCOL_TAG						            TAG;
	PROTOCOL_STATUS_OHT					            STATUS;
	PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301		BODY;

}PKT_CMD_CARRIERINFO_OHT_OCS_ID_1301;

void Conv_PKT_CMD_CARRIERINFO_OHT_OCS_ID_1301(PKT_CMD_CARRIERINFO_OHT_OCS_ID_1301& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
    ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
	ConvertEndian(&Pkt.BODY.CmdID);

}

typedef struct
{
	PROTOCOL_HEADER					                	HEADER;
	PROTOCOL_TAG					                	TAG;
	PROTOCOL_STATUS_OCS					                STATUS;
	PROTOCOL_BODY_RESPONSE_CARRIERINFO_OCS_OHT_1302		BODY;

}PKT_CMD_RESPONSE_CARRIERINFO_OCS_OHT_ID_1302;

void Conv_PKT_CMD_RESPONSE_CARRIERINFO_OCS_OHT_ID_1302(PKT_CMD_RESPONSE_CARRIERINFO_OCS_OHT_ID_1302& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
    ConvertEndian(&Pkt.BODY.CmdID);

}

// ------------ OHT Main재시작 의뢰 (OHT -> AS로 요청) --------------
typedef struct
{
	int	iOhtMainCopyNeed;	// 0 : No,  1 : Need Copy
	int	iOhtParamCopyNeed;	// 0 : No,  1 : Need Copy
	DWORD OHTPID;
	int ShutDownDelay;
} PROTOCOL_BODY_RESTART_INFO_OHT2AS;

// OHT가 자신에게 종료명령을 주고, 재시작 시켜 달라고 AS로 요청하는 패킷
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_RESTART_INFO_OHT2AS							BODY;
} PKT_RESTART_REQUEST_OHT2AS;

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_RESTART_INFO_OHT2AS							BODY;
} PKT_RESTART_KEEPING_INFO_AS2OHT;

// -------------------------------------------------------------------

// AGENT로부터 펌웨어 정보 요청받는 패킷
typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_STATUS_OHT                         STATUS;
}PKT_FIRMWARE_VERSION_REQUEST_AGENT2OHT;


// AGENT 에게 펌웨어 정보를 전달하는 패킷
typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_STATUS_OHT                         STATUS;
	PROTOCOL_FW_VERSION_OHT						BODY;
}PKT_FIRMWARE_VERSION_RESPONSE_OHT2AGENT;

// ------------ 무선모듈 업데이트 또는 데이터읽기 의뢰 (OHT -> AS로 요청) --------------
#define WIRELESS_PUT	0
#define WIRELESS_GET	1
typedef struct
{
	int						iCmd;				// 0 = put, 1 = get
	
	unsigned char			IpAddress[4];
	char					UserName[17];		// 유저명
	char					PassWord[17];		// 패스워드
	char					FilePath[512];		// Local Path
	char					DataSendTo[256];	// 데이터 전송처 (Remote)

	int iWirelessConfig;	// TRUE, FALSE. 무선모듈 Config 업데이트 여부를 설정한다
	int iWirelessMacfil;	// TRUE, FALSE. 무선모듈 Macfil 업데이트 여부를 설정한다
	int iWirelessFirmware;	// TRUE, FALSE. 무선모듈 Firmware 업데이트 여부를 설정한다

} PROTOCOL_BODY_FTP_PUT_GET_INFO_OHT2AS;


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_FTP_PUT_GET_INFO_OHT2AS						BODY;
} PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS;

// -------------------------------------------------------------------


// ------------ 무선모듈 업데이트 또는 데이터읽기 결과 (AS -> OHT) --------------
typedef struct
{
	int						iResult;
	char					ResultText[512];
	
} PROTOCOL_BODY_FTP_PUT_GET_RESULT_AS2OHT;

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_FTP_PUT_GET_RESULT_AS2OHT						BODY;
} PKT_FTP_PUT_GET_RESULT_AS2OHT;

#define WIRELESS_UPDATE_SUCCESS	0x00
#define WIRELESS_UPDATE_FAIL	0x01

// -------------------------------------------------------------------

typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_OHT_VERSION_OHT2OCS							BODY;
} PKT_VERSION_RESPONSE;

												// OHT의 버전을 보고	
void Conv_PKT_VERSION_RESPONSE(PKT_VERSION_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}


typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_OHT_INPUT_REQUEST								BODY;
} PKT_OHT_INPUT_REQUEST;
												// OHT의 투입을 OCS에 요구								
void Conv_PKT_OHT_INPUT_REQUEST(PKT_OHT_INPUT_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}


typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_FLOOR_MOVE_ORDER_RESPONSE						BODY;
} PKT_FLOOR_MOVE_ORDER_RESPONSE;
												 // 층간이동지시응답								
void Conv_PKT_FLOOR_MOVE_ORDER_RESPONSE(PKT_FLOOR_MOVE_ORDER_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);  

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.OrderID);
}


// 버전보고 (OHT -> OCS) 
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_VERSION_RESPONSE								BODY;
} PKT_SIMPLE_VERSION_RESPONSE;

void Conv_PKT_SIMPLE_VERSION_RESPONSE(PKT_SIMPLE_VERSION_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);  

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}


typedef struct _STANDARD_NODE_INFO
{
	unsigned int Num1			: 4;		///<명령 인가 시점( 0:바로 시작, 1:이전 프로파일의 감속 시점에 명령 인가
	unsigned int Num2			: 4;		///<사용할 속도( 0: 명령 받은 속도, 1: 현재 속도, 2:Min(명령속도, 현재속도), 4:MAX(명령속도, 현재속도) )
	unsigned int Num3			: 4;		///<Small Add ( 0: NoUse, 1: Use )
}STANDARD_NODE_INFO;


// AMC_CMD.Cmd 를 쉽게 만들고 분석하기 위한 union
typedef union _STANDARD_NODE_INFO_UNION
{
	STANDARD_NODE_INFO NodeIDInfo;	///< AMC_CMD.Cmd의 bitfield
	unsigned int NodeID;
}STANDARD_NODE_INFO_UNION;



// ============================= DIAG Server - OHT Define ==============================
namespace DiagCmdID
{
	enum Reserved
	{
	// 작업지시 관련 명령등
		AGENT_TO_CLW_CONNECT_CHK_REQUEST		= 0x0001,	// 연결 확인 요청 (진단 Agent -> EQ)
		CLW_TO_AGENT_CONNECT_CHK_RESPONSE		= 0x0002,   // 연결 확인 응답 (EQ -> 진단 Agent)

		DIAG_INFO_DATA_REQUEST					= 0x0111,	// 통계정보보고 요청 (진단 Agent -> 일반 OHT) 
		DIAG_INFO_DATA_RESPONSE					= 0x0112, 	// 통계정보보고 (일반 OHT -> 진단 Agent) 
	
		DIAG_DATA_RESET_REQUEST					= 0x0113, 	// 통계정보 초기화 요청 (진단 Agent -> 일반 OHT)
		DIAG_DATA_RESET_RESPONSE				= 0x0114, 	// 통계정보 초기화 응답 (일반 OHT -> 진단 Agent)

		DIAG_INFO_LEVEL_CHANGE_REQUEST			= 0x0115, 	// DELETED - 통계정보 기준 변경 요청 (진단 Agent -> 일반 OHT)
		DIAG_INFO_LEVEL_CHANGE_RESPONSE			= 0x0116, 	// DELETED - 통계정보 기준 변경 응답 (일반 OHT -> 진단 Agent)

		//새로 추가된 부분 //
		DIAG_INFO_STATUS_REPORT					= 0x0118, 	// 상태정보 보고  (일반 OHT -> 진단 Agent)
		DIAG_INFO_ERROR_REPORT					= 0x0120, 	// 이상이력 보고 (일반 OHT -> 진단 Agent)
		DIAG_INFO_STATISTICS_REQUEST			= 0x0122, 	// 통계정보 요구 (일반 OHT -> 진단 Agent)
		DIAG_INFO_STATISTICS_RESPONSE			= 0x0123, 	// 통계정보 응답 (진단 Agent -> 일반 OHT)
		DIAG_INFO_CIDMONITORING_REPORT			= 0x0124, 	// CID 상태 Monitoring 정보 보고 (OHT -> 진단 Agent)
		//새로 추가된 부분 //

		CLEAN_CLW_TO_AGENT_LINK_INFO			= 0x0212, 	// 링크정보보고 (SEMES사 청소 OHT -> 진단 Agent)

		// 주의 : 이 부분에 01234 식으로 써 넣으면 컴파일러는 8진수로 해석해 버림!
	};
};


// 집계항목 ID
namespace DiagParamID
{
	enum Reserved
	{
	// 작업지시 관련 명령등
		RUNNING_DISTANCE				= 0x10,			// 주행 구동부의 누적 주행거리
		STEERING_COUNT					= 0x20,   		// Steering 동작 누적 횟수		

		LOAD_UNLOAD_COUNT				= 0x30,			// Shift 동작 누적 횟수
		LOAD_UNLOAD_ROT_COUNT			= 0x31, 		// Shift 모터 누적 회전수
	
		HOIST_COUNT						= 0x40, 		// Hoist 동작 누적 횟수
		HOIST_ROT_COUNT					= 0x41, 		// Hoist 모터 누적 회전수
		
		ROTATE_COUNT					= 0x50, 		// Rotate 동작 누적 횟수
		HAND_COUNT						= 0x60,			// Hand 동작 누적 횟수		
		SHUTTER_COUNT					= 0x70, 		// Shutter 동작 누적 횟수
		
		RUN_NODE_BARCODE_READ_COUNT		= 0x80, 		// 주행노드 바코드리더 동작 누적 횟수
		STATION_NODE_BARCODE_READ_COUNT	= 0x81, 		// 스테이션노드 바코드리더 동작 누적 횟수
		
		LOOK_DOWN_SENSOR_TIMES			= 0x82, 		// Look down 센서 가동 누적 시간		
		UBG_SENSOR_TIMES				= 0x84, 		// UBG 센서 가동 누적 시간
		PBS_SENSOR_TIMES				= 0x85, 		// PBS 센서 동작 누적 시간 (장애물 검출)		

		// 주의 : 이 부분에 01234 식으로 써 넣으면 컴파일러는 8진수로 해석해 버림!
	};
};

// 이상항목 ID
namespace ErrorParamID
{
	enum Reserved
	{
	// 작업지시 관련 명령등
		MEMORY_ERROR					= 0x10,			// Memory 사용량 이상
		HAND_MOVING_ERROR				= 0x20,   		// Hand 동작 시간 이상
		SHUTTER_MOVING_ERROR			= 0x21,			// Shutter 동작 시간 이상

		STEERING_MOVIING_ERROR			= 0x22,			// Steering 동작 시간 이상
		HAND_ALRAM_RESET_COUNT_ERROR	= 0x23, 		// Hand Alarm Reset 횟수 이상 (이상횟수 이상 발생시마다 보고)

		PBS_ERROR						= 0x30, 		// PSB 탈조 발생(위치정보 포함)
		UBG_ERROR						= 0x31, 		// UBG 탈조 발생(위치정보 포함)

		BCR_RETRY_COUNT_ERROR		 	= 0x32, 		// BCR Retry 횟수 이상
		FORNT_REAR_ENCODER_GAP_ERROR	= 0x40, 		// 주행 전/후륜 엔코더 값 차이 이상(엔코더 차이 이상 횟수를 일단위로 Reset하며 이상횟수 이상 발생시마다 보고함. )


		// 주의 : 이 부분에 01234 식으로 써 넣으면 컴파일러는 8진수로 해석해 버림!
	};
};


typedef struct DIAG_HEADER_ 
{
	
	unsigned short 		MachineType;	// 2Byte, MACHINETYPE_DIAG_SVR, MACHINETYPE_DIAG_NORMAL_OHT
	char 				MachineID[6];	// 6Byte, Name 
	unsigned short 		CommandID;		// 2Byte, ID주소의 통지(ADDRESS_DIAG_BROADCAST_DEF), 응답(ADDRESS_DIAG_UNICAST_REPLY) 
} DIAG_HEADER;


typedef struct DIAG_PKT_CHECK_TAG_
{
	DIAG_HEADER								HEADER;
} DIAG_PKT_CHECK_TAG;								// 프로토콜 첫 해석에서 TAG 변수의 위치를 찾기 위해 정의한 것임.

void Conv_DIAG_PKT_CHECK_TAG(DIAG_PKT_CHECK_TAG& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}

// OHT -> DIAG SVR standart packet
typedef struct PKT_OHT2DIAG_
{
	DIAG_HEADER		HEADER;
	unsigned char	Value[MAX_PACKET_SIZE-sizeof(DIAG_HEADER)];
} PKT_OHT2DIAG;

void Conv_PKT_OHT2DIAG(PKT_OHT2DIAG& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}


typedef struct
{
	PKT_OHT2DIAG                                AckPkt;
	UINT 										Length;
} PKT_ACK_OHT2DIAG;								// 프로토콜 첫 해석에서 TAG 변수의 위치를 찾기 위해 정의한 것임.


// 연결 확인 요청 (진단 Agent -> EQ)
typedef struct DIAG_PKT_CONNECT_CHK_REQUEST_
{
	DIAG_HEADER				HEADER;
} DIAG_PKT_CONNECT_CHK_REQUEST;

void Conv_DIAG_PKT_CONNECT_CHK_REQUEST(DIAG_PKT_CONNECT_CHK_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}
// 연결 확인 응답 (EQ -> 진단 Agent)
typedef struct DIAG_PKT_BODY_CONNECT_CHK_RESPONSE_
{
	char					result;
} DIAG_PKT_BODY_CONNECT_CHK_RESPONSE;

typedef struct DIAG_PKT_CONNECT_CHK_RESPONSE_
{
	DIAG_HEADER							HEADER;
    DIAG_PKT_BODY_CONNECT_CHK_RESPONSE BODY;
} DIAG_PKT_CONNECT_CHK_RESPONSE;

void Conv_DIAG_PKT_CONNECT_CHK_RESPONSE(DIAG_PKT_CONNECT_CHK_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}

// 통계정보보고 요청 (진단 Agent -> 일반 OHT)
typedef struct DIAG_PKT_BODY_INFO_DATA_REQUEST_
{
	char Parameter;		// 집계항목 ID
} DIAG_PKT_BODY_INFO_DATA_REQUEST;

typedef struct DIAG_PKT_INFO_DATA_REQUEST_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_INFO_DATA_REQUEST		BODY;
} DIAG_PKT_INFO_DATA_REQUEST;

void Conv_DIAG_PKT_INFO_DATA_REQUEST(DIAG_PKT_INFO_DATA_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}


// 통계정보보고 (일반 OHT -> 진단 Agent) 
typedef struct DIAG_PKT_BODY_INFO_DATA_RESPONSE_
{
	char Parameter;		// 집계항목 ID
	UINT64 Value;		// 누계값 1~8
} DIAG_PKT_BODY_INFO_DATA_RESPONSE;

typedef struct DIAG_PKT_INFO_DATA_RESPONSE_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_INFO_DATA_RESPONSE	BODY;
} DIAG_PKT_INFO_DATA_RESPONSE;

void Conv_DIAG_PKT_INFO_DATA_RESPONSE(DIAG_PKT_INFO_DATA_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	ConvertEndian(&Pkt.BODY.Value);
}


// 통계정보 초기화 요청 (진단 Agent -> 일반 OHT)
typedef struct DIAG_PKT_BODY_DATA_RESET_
{
	char Parameter;		// 집계항목 ID
} DIAG_PKT_BODY_DATA_RESET;

typedef struct DIAG_PKT_DATA_RESET_REQUEST_
{
	DIAG_HEADER					HEADER;
	DIAG_PKT_BODY_DATA_RESET	BODY;
} DIAG_PKT_DATA_RESET_REQUEST;

void Conv_DIAG_PKT_DATA_RESET_REQUEST(DIAG_PKT_DATA_RESET_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}

// 통계정보 초기화 응답 (일반 OHT -> 진단 Agent)
typedef struct DIAG_PKT_BODY_DATA_RESET_RESPONSE_
{
	char Parameter;		// 집계항목 ID
	char SuccessCode;	// 0x00 (성공) / 에러코드
} DIAG_PKT_BODY_DATA_RESET_RESPONSE;

typedef struct DIAG_PKT_DATA_RESET_RESPONSE_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_DATA_RESET_RESPONSE	BODY;
} DIAG_PKT_DATA_RESET_RESPONSE;

void Conv_DIAG_PKT_DATA_RESET_RESPONSE(DIAG_PKT_DATA_RESET_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}



// ---------------- DELETED SEPC ------------------------
// 통계정보 기준 변경 요청 (진단 Agent -> 일반 OHT)
typedef struct DIAG_PKT_BODY_LEVEL_CHANGE_REQUEST_
{
	char Parameter;		// 집계항목 ID
	UINT64 Value;		// 누계값 1~8
} DIAG_PKT_BODY_LEVEL_CHANGE_REQUEST;

typedef struct DIAG_PKT_LEVEL_CHANGE_REQUEST_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_LEVEL_CHANGE_REQUEST	BODY;
} DIAG_PKT_LEVEL_CHANGE_REQUEST;

void Conv_DIAG_PKT_LEVEL_CHANGE_REQUEST(DIAG_PKT_LEVEL_CHANGE_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	ConvertEndian(&Pkt.BODY.Value);
}


// 통계정보 기준 변경 응답 (일반 OHT -> 진단 Agent)
typedef struct DIAG_PKT_BODY_LEVEL_CHANGE_RESPONSE_
{
	char Parameter;		// 집계항목 ID
	char SuccessCode;	// 0x00 (성공) / 에러코드
} DIAG_PKT_BODY_LEVEL_CHANGE_RESPONSE;

typedef struct DIAG_PKT_LEVEL_CHANGE_RESPONSE_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_LEVEL_CHANGE_RESPONSE	BODY;
} DIAG_PKT_LEVEL_CHANGE_RESPONSE;

void Conv_DIAG_PKT_LEVEL_CHANGE_RESPONSE(DIAG_PKT_LEVEL_CHANGE_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}
// --------------------------------------------------------------



// 통계 상태정보 보고 (일반 OHT -> 진단 Agent)
typedef struct DIAG_PKT_BODY_INFO_STATUS_REPORT_
{
	char IsUSBWrite;		// USB 기록 유무
	char IsUSBCheck;		// USB 인식 유무
	char IsFANCheck;		// Fan 정상 유무
	char IsC_Check;			// C 드라이버 용량 정상 유무
	char IsD_Check;			// D 드라이버 용량 정상 유무
} DIAG_PKT_BODY_INFO_STATUS_REPORT;

typedef struct DIAG_PKT_INFO_STATUS_REPORT_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_INFO_STATUS_REPORT	BODY;
} DIAG_PKT_INFO_STATUS_REPORT;

void Conv_DIAG_PKT_INFO_STATUS_REPORT(DIAG_PKT_INFO_STATUS_REPORT& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}

// 이상이력 보고  (일반 OHT -> 진단 Agent)
typedef struct DIAG_PKT_BODY_ERROR_REPORT_
{
	char ErrorID;		// 이상항목 ID
	UINT  NodeID;		//  NODE

} DIAG_PKT_BODY_ERROR_REPORT;

typedef struct DIAG_PKT_ERROR_REPORT_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_ERROR_REPORT			BODY;
} DIAG_PKT_ERROR_REPORT;

void Conv_DIAG_PKT_ERROR_REPORT(DIAG_PKT_ERROR_REPORT& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	 ConvertEndian(&Pkt.BODY.NodeID);
}

// 통계정보 요청 (일반 OHT -> 진단 Agent)
typedef struct DIAG_PKT_DATA_INFO_REQUEST_
{
	DIAG_HEADER				HEADER;
} DIAG_PKT_DATA_INFO_REQUEST;

void Conv_DIAG_PKT_DATA_INFO_REQUEST(DIAG_PKT_DATA_INFO_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}

// 통계정보 응답 (진단 Agent -> OHT)
typedef struct DIAG_PKT_BODY_DATA_INFO_RESPONSE_
{
	UINT64 InfoData[14];		// 인포데이터

} DIAG_PKT_BODY_DATA_INFO_RESPONSE;

typedef struct DIAG_PKT_DATA_INFO_RESPONSE_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_DATA_INFO_RESPONSE	BODY;
} DIAG_PKT_DATA_INFO_RESPONSE;

void Conv_DIAG_PKT_DATA_INFO_RESPONSE(DIAG_PKT_DATA_INFO_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	for(int i=0; i<14; i++)
		ConvertEndian(&Pkt.BODY.InfoData[i]);
}

// CID 상태 Monitoring 정보 보고  (일반 OHT -> 진단 Agent)
// 2015-06-01 Spec changed
typedef struct DIAG_PKT_BODY_CID_REPORT_
{
	int	 StopNodeID;		// STOP NODE
	int  ResetNodeID;		//  RESET NODE
	char Reseult;            //취득 결과 (성공 : 0x00, 실패 : 0x01)
	// char CIDStatus[25];       //CID 상태 Monitoring 정보(25Byte ASCII)
	//char CIDStatus[47];       //CID 상태 Monitoring 정보(38Byte ASCII) => 47Byte Changed
	char CIDStatus[53]; 	  //CID 상태 Monitoring 정보(47Byte ASCII) => 53Byte Changed
	char CIDErrorInfo[3];		//장애 발생 정보 

} DIAG_PKT_BODY_CID_REPORT;

typedef struct DIAG_PKT_CID_REPORT_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_CID_REPORT			BODY;
} DIAG_PKT_CID_REPORT;

void Conv_DIAG_PKT_CID_REPORT(DIAG_PKT_CID_REPORT& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	ConvertEndian(&Pkt.BODY.StopNodeID);
	ConvertEndian(&Pkt.BODY.ResetNodeID);
}


// 링크정보보고 (SEMES사 청소 OHT -> 진단 Agent)
typedef struct DIAG_PKT_BODY_CLEAN_LINK_INFO_
{
	int FromNode;		// 링크 시작 node
	int ToNode;			// 링크 끝 node
} DIAG_PKT_BODY_CLEAN_LINK_INFO;

typedef struct DIAG_PKT_CLEAN_LINK_INFO_
{
	DIAG_HEADER						HEADER;
	DIAG_PKT_BODY_CLEAN_LINK_INFO	BODY;
} DIAG_PKT_CLEAN_LINK_INFO;

void Conv_DIAG_PKT_CLEAN_LINK_INFO(DIAG_PKT_CLEAN_LINK_INFO& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	ConvertEndian(&Pkt.BODY.FromNode);
	ConvertEndian(&Pkt.BODY.ToNode);
}
//진동 데이터 정보
typedef struct
{
	PROTOCOL_HEADER							HEADER;
	PROTOCOL_TAG							TAG;
	AVS_VIBRATION_DATA_INFO					BODY;
} PKT_OHT2TP_VIBRATION_INFO;


typedef struct
{
	BYTE Mode;            		//0x00 노드, 0x01: 스테이션, 0x02: 링크
	int  StartID;	    		//시작지점
	int  LinkCount;         	//링크 카운트 설정
	int  LaneCutList[20][2];    //레인컷 설정
}PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_REQ;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_REQ	BODY;
}PKT_TP2OHT_AUTO_DRIVING_TEST_ROUTE_REQ;


typedef struct
{
	bool  Success;			//성공유무
}PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_RESPONSE;

typedef struct
{
	PROTOCOL_HEADER									HEADER;
	PROTOCOL_TAG									TAG;
	PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_RESPONSE	BODY;
}PKT_OHT2TP_AUTO_DRIVING_TEST_ROUTE_RESPONSE;


typedef struct
{
	BYTE Mode;            		//0x00 노드, 0x01: 스테이션, 0x02: 링크
	int RouteList[50];    	//주행경로정보
	int RouteCount;				//주행경로개수
	int StartID;              //시작점
	int EndID;                //끝점
}PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE_REQ;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE_REQ	BODY;
}PKT_TP2OHT_AUTO_DRIVING_TEST_MOVE_REQ;



typedef struct
{
	bool Success;	    		//성공여부
}PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE;

typedef struct
{
	PROTOCOL_HEADER									HEADER;
	PROTOCOL_TAG									TAG;
	PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE			BODY;
}PKT_OHT2TP_AUTO_DRIVING_TEST_MOVE_RESPONSE;



#pragma pack(pop) //한 바이트씩 패킹..

#endif


