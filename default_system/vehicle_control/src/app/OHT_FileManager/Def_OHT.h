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
#define OHT_NAME_V70        	0
#define OHT_NAME_MAC        	1
#define OHT_NAME_PROBECARD  	2
#define OHT_NAME_PROBECARD_JIG  3
#define OHT_NAME_V70_REV_01		4
#define OHT_NAME_STD		   30



#define OHT_NAME            	OHT_NAME_V70_REV_01

#define HOKUYO_PBS 				0
#define SICK_PBS 				1

// 장애물 센서 type
#if ((OHT_NAME == OHT_NAME_V70) || (OHT_NAME == OHT_NAME_MAC) || (OHT_NAME == OHT_NAME_PROBECARD) || (OHT_NAME == OHT_NAME_PROBECARD_JIG))
#define PBS_TYPE				HOKUYO_PBS
#define	UBG_SENSOR_ENABLE		ON				// UBG 센서 사용 여부
#define FOLLOW_CONTROL			1             // 근접 제어 사용 여부
#define NUMBER_OF_OUTPUT        72
#define SAMSUNG_COMMON_SPEC 	0
#elif (OHT_NAME == OHT_NAME_V70_REV_01)
#define PBS_TYPE				HOKUYO_PBS
#define	UBG_SENSOR_ENABLE		ON				// UBG 센서 사용 여부
#define FOLLOW_CONTROL			1             // 근접 제어 사용 여부
#define NUMBER_OF_OUTPUT        96
#define SAMSUNG_COMMON_SPEC 	1
#elif((OHT_NAME == OHT_NAME_MASK) || (OHT_NAME == OHT_NAME_MASK_JIG))
#define PBS_TYPE				HOKUYO_PBS	 	// PBS 센서 타입 여부
#define	UBG_SENSOR_ENABLE		OFF				// UBG 센서 사용 여부
#define FOLLOW_CONTROL			0				// 근접 제어 사용 여부#else
#define NUMBER_OF_OUTPUT        72
#define SAMSUNG_COMMON_SPEC 	1
#else
#define PBS_TYPE				SICK_PBS
#define FOLLOW_CONTROL			1             // 근접 제어 사용 여부
#define	UBG_SENSOR_ENABLE		ON				// UBG 센서 사용 여부
#define FOLLOW_CONTROL			1				// 근접 제어 사용 여부#else
#define NUMBER_OF_OUTPUT        72
#define SAMSUNG_COMMON_SPEC 	0
#endif

// -------------------------------------------
// Common Define
// -------------------------------------------
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
#define OHT_VERSION_47      0x47 ///< v4.7
#define OHT_VERSION_60      0x60 ///< v6.0
#define OHT_VERSION_65      0x65 ///< v6.5
#define OHT_VERSION_70      0x70 ///< v7.0		// Foup with AZ01
#define OHT_VERSION_10      0x10 ///< v1.0
#define OHT_VERSION_71      0x71 ///< v7.1		// Foup with AH01
#define OHT_VERSION_72      0x72 ///< v7.2		// MAC

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
#define OHT_TYPE_PROBE_JIG  0x0C ///< Probe Card OHT Jig
#define OHT_TYPE_FOUP_REV01 0x0D ///< Foup IconBoard II

// ----------------------------------------------------------------------------------------
// Socket 정보
// ----------------------------------------------------------------------------------------
// Port Info
#define PORT_OCS            5001 ///< OCS Port 정보
#define PORT_OCS_SUB        5011 ///< OCS Sub Port 정보
#define PORT_TP             5002 ///< TP Port 정보
#define PORT_MONITOR        7000 ///< 자가진단 Port 정보
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
#define STS_ERROR           0x0E ///< Error

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

// File 경로
#if 0
#define STR_DATAFILE_PATH           L"\\DataFiles\\"
#define STR_DATAFILE_BACKUP_PATH    L"\\DataFiles\\Backup\\"
#define STR_WAVE_PATH               L"\\Wave\\"
#define STR_DATAFILE_UPDATE_PATH    L"\\DataFiles\\Update\\"

#define STR_FILENAME_MAP            L"map.dat"
#define STR_FILENAME_STATION        L"station.std"
#define STR_FILENAME_TEACHING       L"teaching.tcd"
#define STR_FILENAME_IOMAP			L"IOMap.ini"
#define STR_FILENAME_OHTASSISTANT	L"OHTAssistant.exe"

#else
//#define STR_OHT_FOLDER_PATH			L"C:\\User\\OHT\\"
#define STR_DATAFILE_PATH_MAPSET    L"\\DataFiles\\MAPSET\\"
#define STR_DATAFILE_PATH           L"\\DataFiles\\"
#define STR_DATAFILE_BACKUP_PATH    L"\\DataFiles\\Backup\\"
#define STR_WAVE_PATH               L"\\Wave\\"
#define STR_DATAFILE_UPDATE_PATH    L"\\DataFiles\\Update\\"
#define STR_DATAFILE_Down_PATH      L"\\DataFiles\\Down\\"


#define STR_FILENAME_CID            L"CID.DAT"
#define STR_FILENAME_COLLISION      L"COLLISION.DAT"
#define STR_FILENAME_FLOWCONTROL    L"FLOWCONTROL.DAT"
#define STR_FILENAME_LOOKDOWN       L"LOOKDOWN.DAT"
#define STR_FILENAME_MAP            L"MAPDATA.DAT"
#define STR_FILENAME_STATION        L"STATION.DAT"
#define STR_FILENAME_TEACHING       L"TEACHING.DAT"
#define STR_FILENAME_PATHDATA       L"PATHDATA.DAT"
#define STR_FILENAME_JOBORDER		L"JOBORDER.DAT"
#define STR_FILENAME_MAPMAKERFILE	L"MAPMAKERFILE.DAT"
#define STR_FILENAME_IOMAP			L"IOMap.ini"
#define STR_FILENAME_OHTASSISTANT	L"OHTAssistant.exe"
#endif

// wave 파일 이름
#define NAME_WAV_DRIVING            L"Driving.wav"
#define NAME_WAV_TRANS              L"Trans.wav"
#define NAME_WAV_ERROR              L"Error.wav"
#define NAME_WAV_OBS                L"Obstacle.wav"
#define NAME_WAV_DETECT             L"Detect.wav"
#define NAME_WAV_PAUSE              L"Pause.wav"
#define NAME_WAV_OBSWARNNING        L"ObsWarnning.wav"

//Data Type 2013.08.06 by doori.shin
#define DATATYPE_CID				0x00
#define DATATYPE_COLLISION			0x01
#define DATATYPE_FLOWCONTROL		0x02
#define DATATYPE_LOOKDOWN			0x03
#define	DATATYPE_MAP				0x04
#define DATATYPE_STATION			0x05
#define DATATYPE_TEACHING			0x06

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

#define EQPIO_ABNORMAL_ON_OFF_CHECK_TIME				500 // ms
#define EQPIO_GO_ABNORMAL_OFF_CHECK_TIME				3000 // ms

#define ESTOP_BY_TP				0
#define ESTOP_BY_OCS			1
#define ESTOP_BY_TURM_OFF		2

#define EXIT_ROUTE_OCS_PATH				1		
#define OHT_EXIST_IN_COLLISON_AREA		2	
#define CARRIER_EXIST_4_UNLOADING		3
#define NOT_CARRIER_4_LOADING			4	
#define ABNORMAL_CARRIER_IN_STB			5
#define TRUNOFF_STOP					6


#endif // Def_OHTH

