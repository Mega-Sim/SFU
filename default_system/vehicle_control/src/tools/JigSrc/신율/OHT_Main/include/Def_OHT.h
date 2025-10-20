/**
@file    Def_OHT.h
@version OHT 7.0
@brief   OHT에 공통으로 사용될 상수, 변수를 정의한 Header File
*/

#ifndef Def_OHTH
#define Def_OHTH

#include "Def_Variables.h"

//wchar_t *strOHTMODEL[] = {L"FOUP", L"MAC", L"PROBECARD", L"PROBECARD_JIG", L"FOUP_REV01"};

// -------------------------------------------
// 기본 Data typedef
//typedef unsigned char       BYTE;   ///< 1byte 자료형
//typedef unsigned short      WORD;   ///< 2byte 자료형
//typedef unsigned int        UINT;   ///< 4byte 자료형(unsigned int)
//typedef unsigned __int64    UINT64; ///< 8byte 자료형(unsigned __int64)
// -------------------------------------------
#define OHT_NAME_STD_V80_RETICLE    29
#define OHT_NAME_STD_V80	        30   //V8.1 정의 내용 확인 할 것..
#define OHT_NAME_STD_V81	        31   //V8.1
#define OHT_NAME_STD_V81S    		32    //V8.1S
#define OHT_NAME_STD_V85    		34
#define OHT_NAME_STD_V86    		37
#define OHT_NAME_STD_PATROL        	40


//모델 정보(정합성 확인)
#define OHTTYPE_NONE         	"UNKNOWN"
#define OHTTYPE_FOUP80      	"FOUP_V80"
#define OHTTYPE_EUV80        	"EUV_V80"
#define OHTTYPE_FOUP81         	"FOUP_V81E"   		   //timy v8.1
//#define OHTTYPE_FOUP81         	"FOUP_V82"        ///timy v8.2용  추후 8.1버전 사용시 이부분은 주석처리
#define OHTTYPE_EUV85         	"EUV_V85"
#define OHTTYPE_FOUP81S         "FOUP_V81S"
#define OHTTYPE_EUV86         	"EUV_V86"

#define LINETYPE_P1       "P1_LINE"
#define LINETYPE_E         "E_LINE"
#define LINETYPE_P2        "P2_LINE"


//#define OHT_NAME            	OHT_NAME_STD_V80_RETICLE // Setting Vehicle Type
#define OHT_NAME            	OHT_NAME_STD_V86 // Setting Vehicle Type

#define USE_SOM6869				0    // timy  6869 유무 !!!!


#define INTERNAL_VER			3
// ------------------------------------
#define RETICLE_NOMAL				0
#define RETICLE_EUV_1				1


#define OHT_RETICLE_EXTEND_MODEL	RETICLE_EUV_1
// ------------------------------------

//#define OHT_STD_V80_EXTEND_MODEL	STD_V80_NOMAL
// ------------------------------------

#define USE_CHANGENODE    		1    //26mm SHift 개선 전/후에 따른 분류 (shift limit값) 라인투입전 반드시 확인할것 0 : 개조전 1 : 개조후

//21.12.08
#define USE_TESTSTATION			0    //★★★TESTStation 사용 시 1 → 배포시 꼭 0이어야 합니다★★★




#define HOKUYO_PBS 				0
#define SICK_PBS 				1

// 20161216 sh17.jo : Commfail 체크 기능 추가
// 무선 Client Type
#define FXE3000_WIRELESS_CLIENT 0
#define XR5_WIRELESS_CLIENT 1
#define FXE2000_WIRELESS_CLIENT 2



// 장애물 센서 type
#if ((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
#define PBS_TYPE				HOKUYO_PBS
#define	UBG_SENSOR_ENABLE		ON				// UBG 센서 사용 여부
#define FOLLOW_CONTROL			1             // 근접 제어 사용 여부
#define NUMBER_OF_OUTPUT        96
#define SAMSUNG_COMMON_SPEC 	1
#define WirelessClient_TYPE     FXE3000_WIRELESS_CLIENT
#elif((OHT_NAME == OHT_NAME_STD_V81) ||(OHT_NAME == OHT_NAME_STD_V81S))
#define PBS_TYPE				HOKUYO_PBS
#define	UBG_SENSOR_ENABLE		ON				// UBG 센서 사용 여부
#define FOLLOW_CONTROL			1             // 근접 제어 사용 여부
#define NUMBER_OF_OUTPUT        96
#define SAMSUNG_COMMON_SPEC 	1
#define WirelessClient_TYPE     FXE3000_WIRELESS_CLIENT

#define USE_TRANS_PROFILE_VER2   //이적재 최적화 코드 우선 테스트용으로 사용할것 20190818~

#elif(OHT_NAME == OHT_NAME_STD_V85)
#define PBS_TYPE				HOKUYO_PBS
#define	UBG_SENSOR_ENABLE		ON				// UBG 센서 사용 여부
#define FOLLOW_CONTROL			1             // 근접 제어 사용 여부
#define NUMBER_OF_OUTPUT        96
#define SAMSUNG_COMMON_SPEC 	1
#define WirelessClient_TYPE     FXE3000_WIRELESS_CLIENT

#define OHT_HIGH_RAIL_HOIST_PRAM
#define OHT_HIGH_RAIL_CHECK_HOME
#define USE_TRANS_PROFILE_VER2

#elif(OHT_NAME == OHT_NAME_STD_V86)
#define PBS_TYPE				SOSLAB_PBS
#define	UBG_SENSOR_ENABLE		ON				// UBG 센서 사용 여부
#define FOLLOW_CONTROL			1             // 근접 제어 사용 여부
#define NUMBER_OF_OUTPUT        96
#define SAMSUNG_COMMON_SPEC 	1
#define WirelessClient_TYPE     FXE3000_WIRELESS_CLIENT

#define OHT_HIGH_RAIL_HOIST_PRAM
#define OHT_HIGH_RAIL_CHECK_HOME
#define USE_TRANS_PROFILE_VER2

#else
#define PBS_TYPE				SICK_PBS
#define FOLLOW_CONTROL			1             // 근접 제어 사용 여부
#define	UBG_SENSOR_ENABLE		ON				// UBG 센서 사용 여부
#define FOLLOW_CONTROL			1				// 근접 제어 사용 여부#else
#define NUMBER_OF_OUTPUT        72
#define SAMSUNG_COMMON_SPEC 	0
#define WirelessClient_TYPE     XR5_WIRELESS_CLIENT
#endif

// -------------------------------------------
// Common Define
// -------------------------------------------
#define NULL                0

#define OFF                 0 ///< OFF
#define ON                  1 ///< ON

#define NO_ERR              0 ///< No Error

#define FALSE               0 ///< Flase
#define TRUE                1 ///< True

#define RTN_OK              0 //
#define RTN_FAIL            1 //

#define CMD_ACKNONE			0
#define CMD_ACK 	        1 //

#define RCV_ST				0


#define HIGH_BYTE			0
#define MID_BYTE			1
#define LOW_BYTE			2

#define LISTCOUNT_EMPTY		0


// ----------------------------------------------------------------------------------------
// OHT Information
// ----------------------------------------------------------------------------------------

// OHT Version
#define OHT_VERSION_70      0x70 ///< v7.0
#define OHT_VERSION_71      0x71 ///< v7.1
#define OHT_VERSION_72      0x72 ///< v7.2
#define OHT_VERSION_80      0x80 ///< v8.0
#define OHT_VERSION_81      0x81 ///< v8.1
#define OHT_VERSION_85      0x85 ///< v8.5
#define OHT_VERSION_86      0x86 ///< v8.6

// OHT Type
#define OHT_TYPE_FOUP       0x01 ///< Foup OHT
#define OHT_TYPE_RETICLE    0x02 ///< Raticle OHT
#define OHT_TYPE_FOSB       0x03 ///< FOSB OHT
#define OHT_TYPE_SERVICE    0x04 ///< Service OHT
#define OHT_TYPE_MZ0        0x05 ///< Magazine 0도 OHT
#define OHT_TYPE_CS0        0x06 ///< Cassette 0도 OHT
#define OHT_TYPE_MZ90       0x07 ///< Magazine 90도 OHT
#define OHT_TYPE_CS90       0x08 ///< Cassette 90도 OHT
#define OHT_TYPE_MZRT       0x09 ///< Magazine Rotate OHT
#define OHT_TYPE_CSRT       0x0A ///< Cassette Rotate OHT
#define OHT_TYPE_PROBE      0x0B ///< Probe Card OHT
#define OHT_TYPE_FOUP_S  	0x0C ///< V8.1S
#define OHT_TYPE_FOUP_REV01 0x0D ///< Foup IconBoard II
#define OHT_TYPE_EUV		0x0E ///< EUV OHT


// ----------------------------------------------------------------------------------------
// Socket 정보
// ----------------------------------------------------------------------------------------
// Port Info
#define PORT_OCS            5001 ///< OCS Port 정보
#define PORT_OCS_SUB        5011 ///< OCS Sub Port 정보
#define PORT_TP             5002 ///< TP Port 정보
#define PORT_MONITOR        7000 ///< 자가진단 Port 정보
#define PORT_IOTHUB			7007 ///< 물류 IoT Hub Port 정보
//#define PORT_MONITORING     7000 ///< HID/IPC/Network 등 정보

// IP Address 정보
#define IPADDR_MONITOR      L"10.240.21.196" ///< 자가진단 IP 주소
#define IPADDR_LOOPING		L"127.0.0.1" ///< 자신 IP 주소

// ----------------------------------------------------------------------------------------
// OHT Common Define
// ----------------------------------------------------------------------------------------

// OHT Mode
#define MODE_MANUAL         0x01 ///< Manual Mode
#define MODE_AUTO           0x10 ///< Auto Mode

// OHT Status
#define STS_INIT            0x00 ///< Initialized
#define STS_GOING           0x01 ///< Going
#define STS_ARRIVED         0x02 ///< Arrived
#define STS_UNLOADING       0x03 ///< Unloading
#define STS_UNLOADED        0x04 ///< Unloaded
#define STS_LOADING         0x05 ///< Loading
#define STS_LOADED          0x06 ///< Loaded
#define STS_AUTOPOSITION    0x07 ///< Auto Position
#define STS_RUNNING         0x08 ///< Manual 수행 중
#define STS_COMPLETE        0x09 ///< Manual 수행 완료
#define STS_AUTORECOVERY    0x0A ///< Auto Recovery
#define STS_AUTORECOVERY_BLANKTRANS    0x0B ///< 공출하
#define STS_AUTORECOVERY_DOUBLETRANS   0x0C ///< 이중반입
#define STS_AUTORECOVERY_OVERRUN   		0x0D ///< 오버런
#define STS_ERROR           			0x0E ///< Error
#define STS_TURN_OFF			   		0x0F ///< 자동 주행 중 기동 off 상태
#define STS_AUTO_TEACHING		   		0x21 ///< AUTO_TEACHING
#define STS_AUTO_TAUGHT		   		0x22 ///< AUTO_TAUGHT
#define STS_AUTO_COMPLETE				0x23
#define STS_AUTO_TEACHING_NULLTYPE		0x24 ///< AUTO_TEACHING
#define STS_AUTO_TRANS_HOMING           0x25


// FinalPortType
#define GO_FINAL_PORT_TYPE_NONE			0x00
#define GO_FINAL_PORT_TYPE_STOCKER		0x01
#define GO_FINAL_PORT_TYPE_EQ			0x02
#define GO_FINAL_PORT_TYPE_STB			0x03
#define GO_FINAL_PORT_TYPE_UTB			0x04
#define GO_FINAL_PORT_TYPE_Loader		0x05
#define GO_FINAL_PORT_TYPE_PassBoxIn	0x06
#define GO_FINAL_PORT_TYPE_PassBoxOut	0x07

//---------------------------------------------------------------------------
// 이적재 Command 관련 Define
//---------------------------------------------------------------------------
// 이적재 Tag Type
#define TAG_TYPE_NONE       0x00 ///< Stop Tag
#define TAG_TYPE_STOP       0x01 ///< Stop Tag
#define TAG_TYPE_QR_LEFT    0x02 ///< QR Tag(Left)
#define TAG_TYPE_QR_RIGHT   0x03 ///< QR Tag(Right)
#define TAG_TYPE_MODIFY   	0x04 ///< 주행 Tag(강제수정)

// 이적재 Port Type
#define PORT_TYPE_EQ        0x01 ///< 일반 설비
#define PORT_TYPE_EQ_X      0x02 ///< Multi Port EQ
#define PORT_TYPE_STOCKER   0x03 ///< Stocker
#define PORT_TYPE_LOADER    0x04 ///< Loader Buffer
#define PORT_TYPE_STB_L     0x05 ///< STB Left
#define PORT_TYPE_STB_R     0x06 ///< STB Right
#define PORT_TYPE_UTB       0x07 ///< UTB

// Trans Type
#define TRANSTYPE_NONE      0x00    ///< Trans Type : None
#define TRANSTYPE_LOAD      0x01    ///< Trans Type : Load
#define TRANSTYPE_UNLOAD    0x02    ///< Trans Type : Unload
#define TRANSTYPE_SCAN      0x03    ///< Trans Type : Scan
#define TRANSTYPE_MOVET     0x04    ///< Trans Type : Move Teaching Point
#define TRANSTYPE_AUTOSET_HOME  0x05    ///< Trans Type : Set Hoist Home (auto)
#define TRANSTYPE_AUTOTEACHING  0x06    ///< Trans Type : Set Hoist Home (auto)
#define TRANSTYPE_MOVE_ALL_HOME 0x07    ///< Trans Type : All Axis(Hoisr, Shift, Hand Home 이동) (Shutter, Rotate 제외)
#define TRANSTYPE_AUTO_CHANGE  0x08
#define TRANSTYPE_AUTOTEACHING_NULLTYPE  0x09    ///< Trans Type : Auto Teaching Nulltype
#define TRANSTYPE_SMALLADD_HOIST_HOME  0x10    ///< Trans Type : Auto Teaching Nulltype


// PIO 방향
#define PIO_DIR_LEFT        0x01 ///< Left STB
#define PIO_DIR_RIGHT       0x02 ///< Right STB

// STB 방향
#define STB_DIR_NONE        0x00 ///< None
#define STB_DIR_LEFT        0x01 ///< Left STB
#define STB_DIR_RIGHT       0x02 ///< Right STB

// PIO Error 발생 여부
#define PIO_ERR_DISABLE     0x00 ///< PIO Error 발생 안함(Auto Retry 함)
#define PIO_ERR_ENABLE      0x01 ///< PIO Error 발생(Auto Retry 안함)

// by zzang9un 2013.09.06 : 반송물 종류 추가(MCS, OCS 기준으로 값 정의)
#define CARRIER_NOTDEFINED  0x64 ///< Not Defined
#define CARRIER_FOUP        0x00 ///< Foup
#define CARRIER_POD         0x01 ///< POD
#define CARRIER_EUV			0x02 ///< EUV
#define CARRIER_MAC         0x03 ///< MAC
#define CARRIER_FOSB        0x04 ///< FOSB

// ----------------------------------------------------------------------------------------
// Etc Define
// ----------------------------------------------------------------------------------------

// Virus check
#define  STR_VIRUS_PATH		"\\ime\\"
#define  STR_LOG_PATH		"D:\\Log"

// Power Control
#define PWR_ON              1
#define PWR_OFF             2
#define PWR_RST             3
#define PWR_ENABLE          4

#define AXIS_HAND			1
#define AXIS_FRONT			2
#define AXIS_REAR			3

// File 경로
#define STR_DATAFILE_PATH_MAPSET    L"\\DataFiles\\MAPSET\\"
#define STR_DATAFILE_PATH           L"\\DataFiles\\"
#define STR_DATAFILE_BACKUP_PATH    L"\\DataFiles\\Backup\\"
#define STR_WAVE_PATH               L"\\Wave\\"
#define STR_DATAFILE_UPDATE_PATH    L"\\DataFiles\\Update\\"

#define STR_FILENAME_CID            L"CID.dat"
#define STR_FILENAME_COLLISION      L"COLLISION.dat"
#define STR_FILENAME_FLOWCONTROL    L"FLOWCONTROL.dat"
#define STR_FILENAME_MAP            L"MAPDATA.dat"
#define STR_FILENAME_STATION        L"STATION.dat"
#define STR_FILENAME_TEACHING       L"TEACHING.dat"
#define STR_FILENAME_PATHDATA       L"PATHDATA.dat"

#define STR_FILENAME_JOBORDER		L"JOBORDER.dat"
#define STR_FILENAME_MAPAKERFILE	L"MAPMAKERFILE.dat"


#define STR_FILENAME_DIAG			L"DiagParam.ini"
#define STR_FILENAME_DRIVING		L"Driving.ini"
#define STR_FILENAME_EQPIO			L"EQPIO.ini"
#define STR_FILENAME_PATHSEARCH		L"PathSearch.ini"
#define STR_FILENAME_TRANS			L"Trans.ini"
#define STR_FILENAME_IOMAP			L"IOMap.ini"
#define STR_FILENAME_UBGCHECKNODES	L"UBGCheckNodes.dat"

#define STR_FILENAME_OHTASSISTANT	L"OHTAssistant.exe"

#define STR_FILENAME_MapMakerFile            L"MapMakerFile.dat"
#define STR_FILENAME_JobOrderFile            L"JobOrder.dat"


// wave 파일 이름
#define NAME_WAV_DRIVING            L"Driving.wav"
#define NAME_WAV_TRANS              L"Trans.wav"
#define NAME_WAV_ERROR              L"Error.wav"
#define NAME_WAV_OBS                L"Obstacle.wav"
#define NAME_WAV_DETECT             L"Detect.wav"
#define NAME_WAV_PAUSE              L"Pause.wav"
#define NAME_WAV_OBSWARNNING        L"ObsWarnning.wav"
#define NAME_WAV_LOOKDOWN        	L"Lookdown.wav"
#define NAME_WAV_OSCILLATION		L"Oscillation.wav"

//Data Type 2016.05.26 by doori.shin
#define DATATYPE_CID				0x00
#define DATATYPE_COLLISION			0x01
#define DATATYPE_FLOWCONTROL		0x02
#define DATATYPE_LOOKDOWN			0x03
#define	DATATYPE_MAP				0x04
#define DATATYPE_STATION			0x05
#define DATATYPE_TEACHING			0x06
#define DATATYPE_PATHDATA			0x07

//---------------------------------------------------------------------------
// Content : Log Buffer
//---------------------------------------------------------------------------
#define BASE_PORT                   5100

#define LOG_COMM                    0
#define LOG_COMMAND                 1
#define LOG_RUNNING                 2
#define LOG_TPEXEC                  3
#define LOG_TPCOMMAND               4
#define LOG_PARA                    5
#define LOG_PATHSEAR                6
#define LOG_EXCEPT                  7
#define LOG_ERR                     8
#define LOG_BCR                     9
#define LOG_EXEC                    10
#define LOG_RFPIO                   11
#define LOG_AUTOUP                  12
#define LOG_DRVEXEC                 13
#define LOG_RFID                    14
#define LOG_AV                      15
#define LOG_PING                    16
#define LOG_AMCCMD                  17
#define LOG_CMC                     18
#define LOG_LOOKDN                  19
#define LOG_AMCDEBUG                20
#define LOG_MAPOUT                  21
#define LOG_MAPIN                   22
#define LOG_TREXEC                  23
#define LOG_OBSCMD                  24
#define LOG_OBSEXEC                 25
#define LOG_OBSCOUNT                26
#define LOG_HID                     27
#define LOG_NODEDIST                28
#define LOG_AMCERR                  29
#define LOG_AMCIN1                  30
#define LOG_AMCIN2                  31
#define LOG_AMCOUT1                 32
#define LOG_AMCOUT2                 33
#define LOG_CMDLST                  34
#define LOG_AMCDATA                 35
#define LOG_AVNAME                  36
#define LOG_STEER                   37
#define LOG_TEMPER                  38
#define LOG_TRANSAUTOPOS            39
#define LOG_LOADER                  40
#define LOG_WLANINFO                41
#define LOG_AMCDBGLOG               42
#define LOG_MAXSIZE                 LOG_AMCDBGLOG+1    //50개가 넘을경우 OHTAssistant 프로그램 수정해야함.
//---------------------------------------------------------------------------
#define LOGMODE_NOTSAVE             0  //저장하지 않음
#define LOGMODE_LOCAL               1  //실행파일 아래 Log폴더안에 저장
#define LOGMODE_DRIVE               2  //다른 Drive 지정 가능

//2013.10.16 ji7412.lee HandTP InterLock 대응
#define EXCLUDE_AUTO               "1"
#define EXCLUDE_LOAD               "2"
#define EXCLUDE_UNLOAD             "3"
#define EXCLUDE_GOING              "4"
#define EXCLUDE_MOVETPOINT         "5"
#define EXCLUDE_DRIVING_RUNNING    "6"
#define EXCLUDE_HOIST_RUNNING      "7"
#define EXCLUDE_SHIFT_RUNNING      "8"
#define EXCLUDE_ROTATE_RUNNING     "9"

//doori.shin 2014.07.08 접속 TP 종류
#define CONNECTED_TP_NONE	   	0x00
#define CONNECTED_TP_WIRELESS	0x01
#define CONNECTED_TP_WIRED		0x02

#define EQPIO_CS_ON_TIME				50 // ms
#define EQPIO_ABNORMAL_ON_OFF_CHECK_TIME				500 // ms
#define EQPIO_GO_ABNORMAL_OFF_CHECK_TIME				3000 // ms

#define ESTOP_BY_TP				0
#define ESTOP_BY_OCS			1
#define ESTOP_BY_TURM_OFF		2
#define BY_TURM_ON				3

#define EXIT_ROUTE_OCS_PATH				1		
#define OHT_EXIST_IN_COLLISON_AREA		2	
#define CARRIER_EXIST_4_UNLOADING		3
#define NOT_CARRIER_4_LOADING			4	
#define ABNORMAL_CARRIER_IN_STB			5
#define TRUNOFF_STOP					6
#define TRUNON					7


// 데이터 전송 진척 보고 관련 OHT 내부 상태 정의 
//g_DataTransProgressStatus 설정
#define DATATRANSFERPROGRESS_ACK_INIT	(0)//데이터 전송 진척 보고 내용을 Assistant에서 받아서 보내는 상태
#define DATATRANSFERPROGRESS_ACK_TRIGGER_ON	(1)//데이터 전송 진척 보고 최종 결과를 보내는 상태

#define DATA_UPDATE_INIT	(0)  //Map update 결과 보고 관련 초기값
#define DATA_UPDATE_SUCCESS	(1)  //Map update 결과 보고 성공으로 전달
#define DATA_UPDATE_FAIL	(2)  //Map update 결과 보고 실패로 전달


#define Time_1sec             1000
#define Time_10sec           10000
#define Time_20sec           20000
#define Time_25sec           25000
#define Time_40sec           40000
#define Time_60sec           60000
#define Time_100sec         100000
#define Time_160sec         160000




#endif // Def_OHTH

