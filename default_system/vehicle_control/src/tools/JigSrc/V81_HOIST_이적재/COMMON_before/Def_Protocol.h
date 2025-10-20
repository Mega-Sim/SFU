/**
@file    Def_Protocol.h
@version OHT 7.0
@brief   Protocol에 사용되는 공통 상수, 변수 정의된 Header File
@note    Tag 분류 체계는 프로토콜 문서를 참조.
*/

#ifndef Def_ProtocolH
#define Def_ProtocolH

#include "Def_OHT.h"

#pragma pack(push, 1) // 1byte로 Pack

// ----------------------------------------------------------------------------------------
// Communication Define
// ----------------------------------------------------------------------------------------
// -------------------------------------------
// TCP/IP Port 관련 정보
// -------------------------------------------
#define PORT_OHT_OCS            5001    ///< Port 정보 : OHT - OCS
#define PORT_OHT_OCSSUB         5011    ///< Port 정보 : OHT - OCS Sub
#define PORT_OHT_TP             5002    ///< Port 정보 : OHT - TP
#define PORT_OHT_MONITOR        5004    ///< Port 정보 : OHT - monitor(자가진단)
#define PORT_TP_ASSISTANT       5100    ///< Port 정보 : TP - Assistant
#define PORT_TP_AUTOTECHING     1234	///< Port 정보 : TP - Auto Teaching
#define PORT_ASSISTANT_DM       9000    ///< Port 정보 : Assistant - Data Manager

// -------------------------------------------
// Tag 관련 Macro 함수
// -------------------------------------------
/// Sender를 추출하는 매크로 함수
#define GET_SENDER(i)           ((i & MASK_TAG_SENDER) >> 28)
/// Sender를 추출하는 매크로 함수
#define GET_RECEIVER(i)         ((i & MASK_TAG_RECEIVER) >> 24)
/// Tag Type을 추출하는 매크로 함수
#define GET_TAGTYPE(i)          ((i & MASK_TAG_TYPE) >> 16)
/// Command Type을 추출하는 매크로 함수
#define GET_CMDTYPE(i)          ((i & MASK_TAG_CMDTYPE) >> 12)

#define NORMAL_TYPE 	0x01
#define FORCE_TYPE 		0x02
#define JOBCHAGE_RETURN_TYPE 	0x03

// -------------------------------------------
// Tag 검색을 위한 Bit Mask
// -------------------------------------------
#define MASK_TAG_SENDER             0xF0000000 ///< Sender 추출을 위한 Bit Mask
#define MASK_TAG_RECEIVER           0x0F000000 ///< Receiver 추출을 위한 Bit Mask
#define MASK_TAG_TYPE               0x00FF0000 ///< Tag Type 추출을 위한 Bit Mask
#define MASK_TAG_CMDTYPE            0x0000F000 ///< Command Type 추출을 위한 Bit Mask

// -------------------------------------------
// Tag 분류 정의
// -------------------------------------------
// Sender 구분
#define TAG_SENDER_OCS              0x3 ///< Tag Sender : OCS
#define TAG_SENDER_ASSISTANT		0x4	///< Tag Sender : Assistant
#define TAG_SENDER_TP               0x5 ///< Tag Sender : TP
#define TAG_SENDER_DM				0x6	///< Tag Sender : Data Manager
#define TAG_SENDER_MONITOR          0x7 ///< Tag Sender : 자가진단
#define TAG_SENDER_OHT              0x9 ///< Tag Sender : OHT

// Receiver 구분
#define TAG_RECEIVER_OCS            0x3 ///< Tag Receiver : OCS
#define TAG_RECEIVER_ASSISTANT      0x4 ///< Tag Receiver : Assistant
#define TAG_RECEIVER_TP             0x5 ///< Tag Receiver : TP
#define TAG_RECEIVER_DM				0x6	///< Tag Sender : Data Manager
#define TAG_RECEIVER_MONITOR        0x7 ///< Tag Receiver : 자가진단
#define TAG_RECEIVER_OHT            0x9 ///< Tag Receiver : OHT

// Protocol type 구분
#define TAG_PROTOTYPE_REQ           0x1 ///< Protocol Type : 명령, 요청 Tag
#define TAG_PROTOTYPE_ACK           0xA ///< Protocol Type : 응답 Tag

// Cmd Type 구분
#define TAG_CMDTYPE_DRVING          0x1 ///< Command Type : 주행 관련 Tag
#define TAG_CMDTYPE_DRVING_ETC      0x2 ///< Command Type : 주행 기타 관련 Tag
#define TAG_CMDTYPE_TRANS           0x3 ///< Command Type : 이적재 관련 Tag
#define TAG_CMDTYPE_OCSCMD          0x4 ///< Command Type : OCS Common Command 관련 Tag
#define TAG_CMDTYPE_TPCMD           0x5 ///< Command Type : TP Common Command 관련 Tag
#define TAG_CMDTYPE_STATUS          0xA ///< Command Type : Status Tag
#define TAG_CMDTYPE_DATA			0xB	///< Command Type : Transfer Data(Teaching, Station, IO Map ... Etc)
#define TAG_CMDTYPE_INIT            0xC ///< Command Type : 초기화 정보 관련 Tag
#define TAG_CMDTYPE_DM_CMD			0xD	///< Command Type : OCS Remote 관련 Tag
#define TAG_CMDTYPE_QUICKCMD        0xE ///< Command Type : Quick Command 관련 Tag

// -------------------------------------------
// 실제 사용 Command Data
// -------------------------------------------
// 주행 Command
#define TAG_CMDDATA_GO              0x1000 ///< Command Data : Go 명령

// 주행 Etc Command
#define TAG_CMDDATA_GO_MTL          0x2000 ///< Command Data : Go 명령(MTL)
#define TAG_CMDDATA_GO_MAPMAKE      0x2010 ///< Command Data : Map Maker 명령
#define TAG_CMDDATA_GO_CLEAN        0x2020 ///< Command Data : Clean 명령(Service OHT)
#define TAG_CMDDATA_GO_CLEANCANCEL  0x2021 ///< Command Data : Clean 명령 취소(Service OHT)

// 이적재 관련 Command
#define TAG_CMDDATA_TR_LOAD         0x3000 ///< Command Data : Load 명령
#define TAG_CMDDATA_TR_UNLOAD       0x3010 ///< Command Data : Unload 명령

#define TAG_CMDDATA_TR_SCAN         0x3020 ///< Command Data : Scan 명
#define TAG_CMDDATA_TR_HOME_AUTOSET 0x3030 ///< Command Data : Hoist hoem auto set 명령

// 초기화 작업 Command
#define TAG_CMDDATA_INIT_INFO_OCS   0xC000 ///< Command Data : OHT 초기 정보 요청(For OCS)
#define TAG_CMDDATA_INIT_INFO_TP    0xC104 ///< Command Data : OHT 초기 정보 요청(For TP)

//2013.08.20 doori.shin IO Map Tag
#define TAG_CMDDATA_DATA_IOMAP_TP   0xB110 ///< Command Data : Send IO Map Field to TP
#define TAG_CMDDATA_DATA_NOT_READY	0xB120 ///< OHT doesn't ready for sending

// Quick Command 관련
#define TAG_CMDDATA_CTRL_CANCEL     0x4020 ///< Command Data : Cancel 명령
#define TAG_CMDDATA_CTRL_IDRESET    0x4030 ///< Command Data : ID Reset 명령
#define TAG_CMDDATA_CTRL_PAUSE      0x4040 ///< Command Data : Pause 명령
#define TAG_CMDDATA_CTRL_RESUME     0x4050 ///< Command Data : Resume 명령
#define TAG_CMDDATA_CTRL_ESTOP      0x4060 ///< Command Data : E-Stop 명령
#define TAG_CMDDATA_CTRL_TURN_OFF   0x4070 ///< Command Data : 기동 Off 명령

// Status 관련
#define TAG_CMDDATA_STATUS_OCS      0xA000 ///< Status : OHT->OCS
#define TAG_CMDDATA_STATUS_TP       0xA104 ///< Status : OHT->TP

//2013.08.20 doori.shin IO Map Tag
#define TAG_CMDDATA_DATA_IOMAP_TP   0xB110 ///< Command Data : Send IO Map Field to TP
#define TAG_CMDDATA_DATA_NOT_READY	0xB120 ///< OHT doesn't ready for sending
//2013.12.19 doori.shin Additional Status
#define TAG_CMDDATA_DATA_OHT_STATUS		0xB701
//---------------------------------------------------------------------------

// Data 관련 
#define TAG_CMDDATA_VERSIONINFO             0xD000 ///< Command Data : Version 정보
#define TAG_CMDDATA_DATA_DOWNLOAD           0xD100 ///< Command Data : Data Download 명령
#define TAG_CMDDATA_DATA_DOWNLOAD_MAP       0xD101 ///< Command Data : Map Data 전송
#define TAG_CMDDATA_DATA_DOWNLOAD_STATION   0xD102 ///< Command Data : Station Data 전송
#define TAG_CMDDATA_DATA_DOWNLOAD_TEACHING  0xD103 ///< Command Data : Teaching Data 전송
#define TAG_CMDDATA_DATA_BAKCUP             0xD200 ///< Command Data : Data Backup 명령
#define TAG_CMDDATA_DATA_BAKCUP_MAP         0xD201 ///< Command Data : Map Data Backup
#define TAG_CMDDATA_DATA_BAKCUP_STATION     0xD202 ///< Command Data : Station Backup
#define TAG_CMDDATA_DATA_BAKCUP_TEACHING    0xD203 ///< Command Data : Teaching Backup
#define TAG_CMDDATA_DATA_UPLOAD             0xD300 ///< Command Data : Data Upload 명령
#define TAG_CMDDATA_DATA_UPLOAD_MAP         0xD301 ///< Command Data : Map Data 전송
#define TAG_CMDDATA_DATA_UPLOAD_STATION     0xD302 ///< Command Data : Station Data 전송
#define TAG_CMDDATA_DATA_UPLOAD_TEACHING    0xD303 ///< Command Data : Teaching Data 전송

//----------------------------------------------------------------------------
//2013.07.15 doori.shin v7.0 헤더 추가
#define REQ_DATA_VERSIONINFO_DM_RMT     0xD000
#define ACK_DATA_VERSIONINFO_RMT_DM     0xD000
#define REQ_DATA_DOWNLOADINFO_DM_RMT    0xD100
#define DOWNLOAD_STATIONDATA_RMT_DM     0xD110
//#define RESULT_STATIONDATA_DM_RMT       0xF022
#define DOWNLOAD_TEACHINGDATA_RMT_DM    0xD120
//#define RESULT_REACHINGDATA_DM_RMT      0xF024
//#define REQ_DATA_UPLOADINFO_DM_RMT      0xF030
#define UPLOAD_STATIONDATA_DM_RMT       0xD210
#define RESULT_STATIONDATA_RMT_DM       0xD210
#define UPLOAD_TEACHINGDATA_DM_RMT      0xD220
#define RESULT_TEACHINGDATA_RMT_DM      0xD220

//2014.06.12 doori.shin 진단모니터링 통신용 태그 추가
#define REQ_OHT_OPERATION_STATUS		0xB000
#define SND_OHT_OPERATION_STATUS		0xB000
#define REQ_OHT_MOTION_DATA				0xB010
#define SND_OHT_MOTION_DATA				0xB010
#define CMD_RESET_MOVEMENT_VALUE		0xB020
#define ACK_RESET_MOVEMENT_VALUE		0xB020
#define REQ_OHT_HID_DATA				0xB030
#define SND_OHT_HID_DATA				0xB030

// -------------------------------------------
// Tag 관련 구조체
// -------------------------------------------
/**
@brief   Tag 내부 분리를 위한 구조체.

         비트 필드를 이용하여 변수 분리함.
@author  zzang9un
@date    2012.11.14
@warning Little Endian이므로 변수 선언이 하위비트(0)부터 시작함.
*/
typedef struct TAG_INFO_
{
    UINT CmdData    : 16;   ///< Command Data
    UINT CmdType    : 4;    ///< Command Type
    UINT ProtoType  : 4;    ///< Protocol Type
    UINT Receiver   : 4;    ///< Receiver
    UINT Sender     : 4;    ///< Sender
} TAG_INFO;

/**
@brief   Tag 내부 분리를 위한 구조체.

         Union형태로 Tag 변수를 바로 대입할 수 있도록 구현함.
@author  zzang9un
@date    2012.11.14
*/
typedef union uTAG_DATA_
{
    TAG_INFO    Info; ///< Tag Info 구조체
    UINT        Data; ///< Tag를 대입할 변수
} uTAG_DATA;
//---------------------------------------------------------------------------

// -------------------------------------------
// Ack Error 정의
// -------------------------------------------
#define ACK_ERR_BUSY            0x01 ///< Busy : 다른 작업 진행중
#define ACK_ERR_STATE           0x02 ///< Error State : 에러 상태
#define ACK_ERR_PROTOCOL        0x03 ///< Protocol Error : 데이터 이상
#define ACK_ERR_DATALOGIC       0x04 ///< Data Logic Error : 불가한 작업, 의미없는 작업

/**
@brief   Parsing 후 ACK할 TYPE에 대한 enum 변수
@author  zzang9un
@date    2012.11.21
*/
typedef enum ACK_TYPE_
{
    ACK_TYPE_CMD_ERR_OHT2OCS    = -2,   ///< Command에 에러가 발생한 경우
    ACK_TYPE_CMD_ERR_OHT2TP     = -1,   ///< Command에 에러가 발생한 경우
    ACK_TYPE_NONE               = 0,    ///< None
    ACK_TYPE_STATUS_OHT2OCS     = 1,    ///< OCS에게 Status를 보내야 하는 경우
    ACK_TYPE_STATUS_OHT2TP,             ///< TP에게 Status를 보내야 하는 경우
    ACK_TYPE_CMD_OHT2OCS,               ///< OCS에게 Command에 대한 ACK를 보내야 하는 경우
    ACK_TYPE_CMD_OHT2TP,                ///< TP에게 Command에 대한 ACK를 보내야 하는 경우
	ACK_TYPE_OHTINIT_OHT2TP,            ///< TP에게 OHT Info를 보내야 하는 경우
	//2013.07.16 doori.shin 추가
	ACK_TYPE_UPLOAD_DM2RMT,             ///< OCS Remote에게 데이터요청 하는 경우
	ACK_TYPE_DOWNLOAD_RMT2DM,           ///< DataManager에게 ACK를 보내야 하는 경우

	//2013.08.22 doori.shin 추가
	ACK_TYPE_DATA_OHT2TP
} ACK_TYPE;


/**
@brief   송신자에게 응답하기 위한 Ack 정보를 가진 구조체
@author  zzang9un
@date    2013.01.24
*/
typedef struct ACK_DATA_
{
    uTAG_DATA AckTagData;   ///< 응답 Tag
    BYTE RecvCmdID;         ///< 수신한 Command ID
    ACK_TYPE ACKType;       ///< ACK 할 Data의 Type
    BYTE ACKResult;         ///< ACK 결과
    UINT ACKErrorCode;      ///< ACK Error Code
} ACK_DATA;
//---------------------------------------------------------------------------

#pragma pack(pop) // Pack 해제


#endif // Def_ProtocolH
