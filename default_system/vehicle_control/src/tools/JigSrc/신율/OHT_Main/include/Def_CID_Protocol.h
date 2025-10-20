
#ifndef Def_CID_ProtocolH
#define Def_CID_ProtocolH

// CID 모드 정의 (CID-O의 Operatin Mode (O6)
#define	CID_MODE		1	// 평상시 CID 우선권 제어를 위한 모드
#define MTL_LIFTER_MODE	0	// 엘리베이터와 리프터의 CID-O/R간 통신을 위한 모드

#define MTL_AREA_CID_INPUT_NUM	5

// ----- 시리얼로 제어하는 CID를 위한 패킷 정의 (초기화) -----
#define ELEVETOR_HIGH_BYTE	0
#define ELEVETOR_LOW_BYTE	1

typedef struct
{
	unsigned char			STX;		// 송신 Data의 시작. 0 x02
	unsigned char			CMD;		// CMD(Command)는 "i"(Ascii Code 69 H)
	unsigned char			Reserve[2]; // "00"(Ascii Code 30 H 30 H), 상위바이트를 먼저 전송
	unsigned char			SUM[2];		// CMD로부터 SUM의 직전까지를 모두 더한 값
	unsigned char			ETX;		// 송신 Data의 마지막. 0 x03
} OPC_INIT_PKT;

#define OPC_INIT_PKT_CHECKSUM_LENGTH		4	// 체크썸 4바이트

// 패킷에 기본적으로 써 넣을 정보들. 고정이므로 정의된 값을 사용하자
#define OPC_SERIAL_INIT_STX		0x02
#define OPC_SERIAL_INIT_CMD		0x69	// Ascii 'i'
#define OPC_SERIAL_INIT_RESERVE 0x30
#define OPC_SERIAL_INIT_ETX		0x03

#define OPC_SERIAL_INIT_TEST	0xff


// Monitoring OHT -> Agent ID:124
#define CID_MONITOR_INFO_GET_SUCCESS	0x00 // 모두 성공 
#define CID_MONITOR_INFO_GET_STATUS		0x01 //장애 정보만 취득
#define CID_MONITOR_INFO_GET_MONITORING	0x10 //monitoring 정보만 취득
#define CID_MONITOR_INFO_GET_FAIL		0x11 // 모두 실패

// Monitoring OHT -> Agent ID:249
#define CID_MONITOR_INFO_GET_SUCCESS_249	0x00 // 모두 성공 
#define CID_MONITOR_INFO_GET_FAIL_249		0x01 // 모두 실패


#define CID_SERIAL_CMD_DELAY_TURN		10	// 5 - error. 7 - success.  

// ----- 시리얼로 제어하는 CID를 위한 패킷 정의 (통상명령) -----
// ==> OHT의 CID-O가 CID-R 로 송신하는 패킷의 정의이다
typedef struct
{
	unsigned char			STX;		 // 송신 Data의 시작. 0 x02
	unsigned char			CMD;
	unsigned char			OHTNAME[6]; // 호기 No
	unsigned char			Priority[2]; // 반송물의 우선도. Ascii Code로 설정
	unsigned char			Turn;		 // 우/좌선택
	unsigned char			LightGuideID; // CID-R ID
	unsigned char			SUM[2];
	unsigned char			ETX;		// 송신 Data의 마지막. 0 x03
} OPC_TO_TRANSPER_PKT;

#define OPC_TO_TRANSPER_PKT_CHECKSUM_LENGTH		12	// 체크썸 12바이트

#define OPC_TO_TRANSPER_CMD	0x76		// VHL의 OPC 기판 -> 전송 장치 Command ('v')

#define PRIORITY_LEVEL_1	'1'
#define PRIORITY_LEVEL_2    '2'
#define PRIORITY_LEVEL_3    '3'
#define PRIORITY_LEVEL_4    '4'
#define PRIORITY_LEVEL_5    '5'
#define PRIORITY_LEVEL_6    '6'
#define PRIORITY_LEVEL_7    '7'
#define PRIORITY_LEVEL_8    '8'
#define PRIORITY_LEVEL_9    '9'

#define PRIORITY_LEVEL_NOT_COMMUNICATION	'0'
#define PRIORITY_LEVEL_ERR 					'F'
#define PRIORITY_LEVEL_NETWORK_ERR 			'E'
#define PRIORITY_LEVEL_INCHING              '@'

#define PASS_DIRECTION_REQUEST_RIGHT		'R'
#define PASS_DIRECTION_REQUEST_LEFT			'L'
#define PASS_DIRECTION_REQUEST_ALL			'N'
#define PASS_DIRECTION_REQUEST_INCHING		'@'
#define PASS_DIRECTION_REQUEST_PASS 		'P' // AOHC-217 VHLLifter 속도 개선 Lifter 통과하는 경로 by jhoun.lim 2018.06.04
#define PASS_DIRECTION_REQUEST_BEFORECHECK	'B' // AOHC-217 VHLLifter 속도 개선 Lifter 통과하는 경로 by jhoun.lim 2018.11.14

#define LIGHT_GUIDE_ID_INPUT_UP				'X'
#define LIGHT_GUIDE_ID_INPUT_DOWN			'Y'
#define LIGHT_GUIDE_ID_INPUT_UP_PRE			'x' // AOHC-217 VHLLifter 속도 개선 by jhoun.lim 2018.06.04
#define LIGHT_GUIDE_ID_INPUT_DOWN_PRE		'y' // AOHC-217 VHLLifter 속도 개선 by jhoun.lim 2018.06.04
#define LIGHT_GUIDE_ID_NONE					'N' // AOHC-217 VHLLifter 속도 개선 by jhoun.lim 2018.06.04


#define DELAY_CID_COMMAND					20	// 장비사양상 최소 10ms딜레이 줘야 함
#define DELAY_SERIAL_READ	    			70	// 사양에는 최소 50ms이나, 실험결과 이보다 약간 더 긴 시간이 필요하다
#define DELAY_SERIAL_RESET	    			90  // 사양에는 나와있지 않으나, 실험결과 이정도 시간이 필요하다 (50ms - 에러, 70ms - 간혹 에러발생)
#define DELAY_SERIAL_WAIT_N_REQUEST_RETRY	100

#define LIFTER_PASS_TIME_OUT_MS				240000 // 4min

/*
Priority : 반송물 우선도 Ascii Code로 설정
            VHL Lifter에서는 Priority 값과는 무관하게 Lifter Rail의 대기위치에 따라 통과시킬 VHL을 선택한다.
            “00”             ...VHL과 통신하고 있지 않는 상태
            ”01”에서”99” ...VHL이 정상 상태. ”01”이 가장 낮은 Priority. ”99”가 가장 높은 Priority
			“FF”             ... VHL이 이상
            “EE”             ... VHL이 이상(Network 전환 실패)
            “@@”           ... VHL이 Inching 조작 중

좌/우 선택 : VHL로부터 CID에 통과요구. Ascii Code로 설정
                “R”               ...VHL이 우방향을 통과 요구
                “L”               ...VHL이 좌방향을 통과 요구
                “N”               ...VHL이 방향 지정 없음 통과 요구. 분기가 없는 장소(예 : 180° Curve 등)에서의 통과 요구
                “@”              ... VHL이 Inching 조작 중


CID-R ID :  설정이 VHL과 CID가 같은경우는 통신가능, 불일치시 통신 불가능. Ascii Code로 설정
               CID-R ID에 대한 Ascii Code 값은 ”A”에서 ”Z” 및 ”a”에서 ”z”까지의 범위에서 임의 선정 가능

비고：VHL와 통신 불능인 CID-R에서는, 호기 No., Priority, 좌/우선택은 모두"0"(Ascii Code 30 H)이 된다.
*/


// ----- 시리얼로 제어하는 CID를 위한 패킷 정의 (통상명령) -----
// ==> CID-R이 OHT의 CID-O로 응답하는 패킷의 정의이다
typedef struct
{
	unsigned char			STX;		 	// 송신 Data의 시작. 0 x02
	unsigned char			CMD;
	unsigned char			OHTNAME[6]; 	// 호기 No
	unsigned char			PassInfo;	 	// 통과정보
	unsigned char			LightGuideID; 	//
	unsigned char			TransperStatus;	// 전송장치의 상태
	unsigned char			SUM[2];
	unsigned char			ETX;			// 송신 Data의 마지막. 0 x03
} TRANSPER_TO_OPC_PKT;



//CID Monitoring Packet
//[명령] [STX]M4D[ETX]
//[응답] [STX]M=OHT0011000800001,CID00110065535240212224040080000000156[ETX]

typedef struct
{
	// Head : 3Byte
	unsigned char			STX;               // 송신 Data의 시작. 0 x02
	unsigned char			CMD;
	unsigned char			Equal;
} CID_Monitoring_HEADER;


typedef struct
{
	// Here is Body : 38Byte + 15 = Body Size 53Byte
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
	unsigned char			RFError_Per[2];			// 통신 중 무선 통신 오류(%)
} CID_Monitoring_BODY;


typedef struct
{
	unsigned char			SUM[2];
	unsigned char			ETX;			// 송신 Data의 마지막. 0 x03
} CID_Monitoring_TAIL;


typedef struct	// All Byte : 53Byte
{
	CID_Monitoring_HEADER	HEADER;
	CID_Monitoring_BODY		BODY;
	CID_Monitoring_TAIL		TAIL;
	
} CID_Monitoring_PKT;


//CID Status Packet
typedef struct
{
	unsigned char			Setting[3];
} CID_Status_BODY;

typedef struct
{
	CID_Monitoring_HEADER	HEADER;
	CID_Status_BODY			BODY;
	CID_Monitoring_TAIL		TAIL;
} CID_Status_PKT;


// CID호기번호 설정
typedef struct
{
	unsigned char			STX;		 // 송신 Data의 시작. 0 x02
	unsigned char			CMD;
	unsigned char			SEP;
	unsigned char			OHTNAME[6]; // 호기 No
	unsigned char			SUM[2];
	unsigned char			ETX;		// 송신 Data의 마지막. 0 x03
} CID_NameSet_PKT;


// CID호기번호 확인요청
typedef struct
{
	unsigned char			STX;		 // 송신 Data의 시작. 0 x02
	unsigned char			CMD;
	unsigned char			SUM[2];
	unsigned char			ETX;		// 송신 Data의 마지막. 0 x03
} CID_Name_Request_PKT;


// CID호기번호 확인응답
typedef struct
{
	unsigned char			STX;		 // 송신 Data의 시작. 0 x02
	unsigned char			CMD;
	unsigned char			SEP;
	unsigned char			OHTNAME[6]; // 호기 No
	unsigned char			SUM[2];
	unsigned char			ETX;		// 송신 Data의 마지막. 0 x03
} CID_Name_Request_Response_PKT;


// shkim delete_reserve
#if 0
typedef struct	// All Byte : 53Byte
{
	// Head : 3Byte
	unsigned char			STX;               // 송신 Data의 시작. 0 x02
	unsigned char			CMD;
	unsigned char			Equal;
	// Here is Body : 38Byte + 9 = Body Size 47Byte
	unsigned char			OHTID[6];
	// unsigned char			CIDO_FW_Ver[3];			
	unsigned char			CIDR_FirstLinkLevel[3];	// CID-R 첫 광 송신(Link) 신호 세기
	unsigned char			CIDO_FiberCommError[2];	// 통신 중 광 통신 오류(%)
	unsigned char			CIDO_RFCommError[2];	// 통신 중 무선 통신 오류(%)
	unsigned char			delimiter;				// CID-O와 CID-R 데이터 구분
	unsigned char			CID_UniqueID[6];		// CID-고유 ID
	// unsigned char			CIDR_FW_Ver[3];
	unsigned char			RF_ID[5];				// added (2016.10.30)
	unsigned char			RF_CH[4];				// added (2016.10.30)
	unsigned char			CIDR_MAXCurrent[3];		// 통신 중 CID-R 최대 전류
	unsigned char			CIDR_MAXVoltage[3];		// 통신 중 CID-R 최대 전압
	unsigned char			CIDR_MAXTemp[2];		// 통신 중 CID-R 최대 온도
	unsigned char			CIDR_StandbyFiberNoise[3];	// 대기 상태 CID-R 광 노이즈 수신값
	unsigned char			CIDO_FirstLinkLevel[3];	// CID-O의 첫 광 송신(Link) 신호 세기
	unsigned char			FiberError_Per[2];		// 통신 중 광 통신 오류(%)
	unsigned char			RFError_Per[2];			// 통신 중 무선 통신 오류(%)
	// unsigned char			Monitoring6;
	// Tail : 3Byte
	unsigned char			SUM[2];
	unsigned char			ETX;			// 송신 Data의 마지막. 0 x03
} CID_Monitoring_PKT;
#endif
// ---------------------------------------------------

#define TRANSPER_TO_OPC_PKT_CHECKSUM_LENGTH		11	// 체크썸 11바이트

#define TRANSPER_TO_OPC_CMD 					0x74			// 전송 장치 -> VHL의 OPC 기판 Command ('t')

// ZCU측의 Light Guide Controller가 전원 OFF의 경우, 호기 No., 통과 정보, Light Guide ID 는 모두"0"(Ascii Code 30 H)
// 그런 상태에서도 전송장치의 Status는 “O”(정상)
#define TRANSPER_TO_OPC_POWER_OFF_STATUS		0x30

/*
	[TransperStatus]
	ASCII Code 의 종류 및 그에 따른 의미는 하기와 같다.

	　“O”（Ascii code 4FH） ：정상
	-> ZCU가 보낸 광 통신 DATA가 정상적으로 수신된 경우 “O”

	　“P”（Ascii code 50H） ：Parity Error
	-> ZCU가 보낸 광 통신 DATA 의 Parity값이 틀리게 수신된 경우 “P"
	?
	　“S”（Ascii code 53H） ：SUM Check Error
	-> ZCU가 보낸 광 통신 DATA 의 SUM Chech값이 틀리게 수신된 경우 “S”

	　“E”（Ascii code 45H） ：그 외 Error
	-> ZCU와 광 통신 중 그 외의 에러가 발생한 경우 “E”


*/

#define TRANSPER_STATUS_DATA_RECEIVED_OK		0x4f	// "O", 정상
#define TRANSPER_STATUS_PARITY_ERR				0x50	// "P", 패리티에러
#define TRANSPER_STATUS_SUM_ERR					0x53   // "S", SUM체크 에러
#define TRANSPER_STATUS_OTHER_ERR				0x45	// "E", 그외 에러

#define TRANSPER_STATUS_DATA_NOT_RECEIVED		0x10
#define TRANSPER_STATUS_DATA_LENGTH_ERR			0x20

// ----- VHL 리프터 => CID-R 로 보내는 패킷 및 정의 -------
typedef struct
{
	unsigned char			STX;		 	// 송신 Data의 시작. 0 x02
	unsigned char			CMD;

	unsigned char           OHTNAME_1[6]; 	// 호기 No 1
	unsigned char           PASSINFO_1;		// Fiber1 통과정보
	unsigned char			CID_R_ID_1;		// Fiber1 CID-R ID

	unsigned char           OHTNAME_2[6]; 	// 호기 No 2
	unsigned char           PASSINFO_2;		// Fiber2 통과정보
	unsigned char			CID_R_ID_2;		// Fiber2 CID-R ID

	unsigned char           OHTNAME_3[6]; 	// 호기 No 3
	unsigned char           PASSINFO_3;		// Fiber3 통과정보
	unsigned char			CID_R_ID_3;		// Fiber3 CID-R ID

	unsigned char           OHTNAME_4[6]; 	// 호기 No 4
	unsigned char           PASSINFO_4;		// Fiber4 통과정보
	unsigned char			CID_R_ID_4;		// Fiber4 CID-R ID

	unsigned char			SUM[2];
	unsigned char			ETX;			// 송신 Data의 마지막. 0 x03
} LIFTER_TO_OPC_PKT;

#define LIFTER_TO_OPC_PKT_CHECKSUM_LENGTH		34	// 체크썸 34바이트

#define VHL_LIFTER_TO_CID_R_CMD	0x74;	// "t" 에 해당. 명령어 예약어

#define VHL_LIFTER_TO_CID_R_RIGHT_PASS_OK		0x52	// "R" 우방향 통과 OK
#define VHL_LIFTER_TO_CID_R_LEFT_PASS_OK        0x4c	// "L" 좌방향 통과 OK
#define VHL_LIFTER_TO_CID_R_RIGHT_LOWSPEED_OK	0x4d	// "M" 우방향 저속 통과 OK
#define VHL_LIFTER_TO_CID_R_LEFT_LOWSPEED_OK 	0x48	// "H" 좌방향 저속 통과 OK

#define VHL_LIFTER_TO_CID_R_RIGHT_ONLY_OK 		0x72	// "r" 우방향 통과 가능
#define VHL_LIFTER_TO_CID_R_LEFT_ONLY_OK 		0x6c	// "i" 좌방향 통과 가능
#define VHL_LIFTER_TO_CID_R_ALL_PASS_OK   		0x41	// "A" 방향지정 없이 가능
#define VHL_LIFTER_TO_CID_R_ALL_LOWSPEED_OK 	0x44	// "D" 방향없이 저속통과 가능
#define VHL_LIFTER_TO_CID_R_ALL_PASS_OK_P  		0x50	// "P" Pass through lifter //AOHC-217 VHLLifter 속도 개선 add by jhoun.lim

#define VHL_LIFTER_TO_CID_R_PASS_NG 			0x53	// "S" 통과불가능
#define VHL_LIFTER_TO_CID_R_PASS_OK 			0x4e	// "N" 리프터 호출후 응답이 왔으나 무효한 응답임
#define VHL_LIFTER_TO_CID_R_LIFTER_OFF_STATE 	0x40	// "@" 리프트 기동 Off상태 (자동운전 아님)

/*  (참고 : 리프터 사양서 p17)
CMD : CMD(Command)는 “t”(Ascii Code 74 H)가 된다
.
호기 No. : VHL 호기명 Ascii Code로 설정
              호기명 표기 방법 : “000000”(Ascii Code 30H 30H 30H 30H 30H 30H)
              호기명은 CID-R로부터 받은 호기명을 입력하여 송신한다.

통과정보 : CID로 부터 VHL 통과여부 응답 Ascii Code로 설정
			   “R”（Ascii Code 52H） ... 우방향 통과 OK
			   “L”（Ascii Code 4CH） ... 좌방향 통과 OK
			   “M”（Ascii Code 4DH） ... 우방향 저속 통과 OK. VHL을 저속으로 통과시키고자 하는 경우에 송신한다.
			   “H”（Ascii Code 48H） ... 좌방향 저속 통과 OK. VHL을 저속으로 통과시키고자 하는 경우에 송신한다.
			   “r”（Ascii Code 72H）  ... 우방향 통과 가능(좌방향은 통과 NG이나, 우방향이라면 통과 가능)
													 VHL은 ”r”을 수신하면 VHL Lifter로 우방향 통과 요구한다.(”R”을 VHL Lifter에 송신한다.)
			   “l”（Ascii Code 6CH）  ... 좌방향 통과 가능(우방향은 통과 NG이나, 좌방향이라면 통과 가능)
													 VHL은 ”l”를 수신하면 VHL Lifter로 좌방향 통과 요구한다.(”L”을 VHL Lifter에 송신한다.)
			   “A”（Ascii Code 41H） ... 방향 지정 없음 통과 OK(방향 지정 없이 통과 OK)
			   “D”（Ascii Code 44H） ... 방향 지정 없음 저속 통과 OK(방향 지정 없이 저속 통과 OK) VHL을 저속으로 통과시
												 키고자 하는 경우에 송신한다.
			   “S”（Ascii Code 53H） ... 통과 NG(VHL에 정지명령 송신)
			   “N”（Ascii Code 4EH） ... 무효한 Data. VHL과 통신하고 있지 않는 경우에 ”N”을 송신
               “@” （Ascii Code 40H） ... VHL Lifter는 기동 OFF 중(자동 운전 상태가 아님). VHL은 통과 불능.
               ※ Up&Down Type의 경우 VHL → VHL Lifter로의 통신 中 좌/우 선택에 의해 동일 선상의 Rail로도 이동이 가능하다.
                   → 상부 Rail에서 VHL Lifter를 거쳐 상부 Rail로 이동 or 하부 Rail에서 VHL Lifter를 거쳐 하부 Rail로 이동

CID-R ID : 설정이 VHL과 CID가 같은경우는 통신가능, 불일치시 통신 불가능 Ascii Code로 설정
              CID-R ID에 대한 Ascii Code 값은 ”A”에서 ”Z” 및 ”a”에서 ”z”까지의 범위에서 임의 선정 가능

비고：VHL와 통신 불능인 CID-R에서는, 호기 No., Priority, 좌/우선택은 모두"0"(Ascii Code 30 H)이 된다.
*/
// ----------------------------------------------------------------


// ----- CID-R => VHL 리프터로 보내는 패킷 및 정의 -------
//	리프터 사양서 p.18, 19
typedef struct
{
	unsigned char           OHTNAME_2[6]; 	// 호기 No
	unsigned char			Priority[2]; 	// 반송물의 우선도. Ascii Code로 설정
	unsigned char			Turn;		 	// 우/좌선택
	unsigned char			LightGuideID; 	// CID-R ID
}VHL_INFOMATION;

typedef struct
{
	unsigned char			STX;		 	// 송신 Data의 시작. 0 x02
	unsigned char			CMD;

	VHL_INFOMATION				FIBER_INFO_1;
	VHL_INFOMATION				FIBER_INFO_2;
	VHL_INFOMATION				FIBER_INFO_3;
	VHL_INFOMATION				FIBER_INFO_4;

	unsigned char			CID_R_STATUS;	// CID-R 상태
	unsigned char			SUM[2];
	unsigned char			ETX;			// 송신 Data의 마지막. 0 x03
} OPC_TO_LIFTER_PKT;

#define OPC_TO_LIFTER_PKT_CHECKSUM_LENGTH		43	// 체크썸 43바이트

/*
#define CID_R_STATUS_OK				0x4f	// 'O', 정상
#define CID_R_STATUS_PARITY_ERR 	0x50	// 'P', 패리티 에러
#define CID_R_STATUS_SUM_ERR		0x53	// 'S'  Sum에러
#define CID_R_STATUS_OTHER_ERR		0x45	// 'E' 그외의 에러
	CID-R Status : CID-R 이상유무 전송(정상/이상) Ascii Code로 설정
                   ?ASCII Code 의 종류 및 그에 따른 의미는 하기와 같다.
                   　“O”（Ascii code 4FH） ：정상
                       ? ZCU가 보낸 광 통신 DATA가 정상적으로 수신된 경우 “O”
                   　“P”（Ascii code 50H） ：Parity Error
                       ? ZCU가 보낸 광 통신 DATA 의 Parity값이 틀리게 수신된 경우 “P”
                   　“S”（Ascii code 53H） ：SUM Check Error
                       ? ZCU가 보낸 광 통신 DATA 의 SUM Chech값이 틀리게 수신된 경우 “S”
                   　“E”（Ascii code 45H） ：그 외 Error
					  ? ZCU와 광 통신 중 그 외의 에러가 발생한 경우 “E”
*/
// ----------------------------------------------------------------

// 시리얼로 보냈을때 제대로 보냈나에 대한 정의 (CID와 직접 연관 없음)
#define OPC_SERIAL_SEND_OK		0
#define OPC_SERIAL_SEND_ERR		-1

#define OPC_SERIAL_RCV_OK		0
#define OPC_SERIAL_RCV_NO_DATA	-1
#define OPC_SERIAL_RCV_ERROR	-2

#endif