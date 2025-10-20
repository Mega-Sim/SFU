/**
@file    Def_OHT.h
@version OHT 7.0
@brief   OHT에 공통으로 사용될 상수, 변수를 정의한 Header File
*/

#ifndef Def_OHTH
#define Def_OHTH

// -------------------------------------------
// 기본 Data typedef
typedef unsigned char       BYTE;   ///< 1byte 자료형
typedef unsigned short      WORD;   ///< 2byte 자료형
typedef unsigned int        UINT;   ///< 4byte 자료형(unsigned int)
typedef unsigned __int64    UINT64; ///< 8byte 자료형(unsigned __int64)
// -------------------------------------------

// -------------------------------------------
// Debugging용 Define
// -------------------------------------------
#define USE_LOG_MEMO

#define ADD_MEMO_T(STR)   OHTMainForm->memoMain->Text = OHTMainForm->memoMain->Text + _T(STR)
#define ADD_MEMO_S(STR)   OHTMainForm->memoMain->Lines->Add(STR)


// -------------------------------------------
// Common Define
// -------------------------------------------
#define OFF                 0 ///< OFF
#define ON                  1 ///< ON

#define NO_ERR              0 ///< No Error

#define FALSE               0 ///< Flase
#define TRUE                1 ///< True

// ----------------------------------------------------------------------------------------
// OHT Information
// ----------------------------------------------------------------------------------------

// OHT Version
#define OHT_VERSION_47      0x47 ///< v4.7
#define OHT_VERSION_60      0x60 ///< v6.0
#define OHT_VERSION_65      0x65 ///< v6.5
#define OHT_VERSION_70      0x70 ///< v7.0
#define OHT_VERSION_10      0x10 ///< v1.0
#define OHT_VERSION_71      0x71 ///< v7.1
#define OHT_VERSION_72      0x72 ///< v7.2

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


// IP Address 정보
#define IPADDR_MONITOR      L"127.0.0.1" ///< 자가진단 IP 주소

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
#define STS_ERROR           0x0E ///< Error

// CMD Type
#define CMD_NONE            0
#define CMD_GO              1      //G
#define CMD_GOMORE          2      //M
#define CMD_NEXTGOMORE      3      //내부
#define CMD_LOAD            4      //L
#define CMD_UNLOAD          5      //U
#define CMD_SCAN            6      //C
#define CMD_CANCEL          7      //X
#define CMD_ESTOP           8      //P
#define CMD_IDRESET         9      //I
#define CMD_GO_MTLIN        10
#define CMD_MOVETPOINT      11

#define CMD_STOP            12   // 장애물 감지시..
#define CMD_MOVE_DIST       13   // 거리로 이동시
#define CMD_MOVE_FORWARD    14   // 마지막 저속 이동시
#define CMD_MOVE_BACKWARD   15

#define CMD_SCAN_SIMPLE     16  //Foup을 가진상태에서는 Reading만하고 끝냄..
// ----------------------------------------------------------------------------------------
// Driving 관련 Define
// ----------------------------------------------------------------------------------------

// Steering 방향
//#define STEERING_NONE       	0x00 ///< Steering 방향 없음
//#define STEERING_LEFT       	0x01 ///< Left Steering
//#define STEERING_RIGHT      	0x02 ///< Right Steering
//#define STEERING_N_BRANCH_L2R   0x03 ///< N분기 Left to Right
//#define STEERING_N_BRANCH_R2L   0x04 ///< N분기 Right to Left
//#define STEERING_MOVING			0x05 ///< Steering 이동 중

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

// Power Control
#define PWR_ON              1
#define PWR_OFF             2
#define PWR_RST             3
#define PWR_ENABLE          4

// File 경로
#define STR_PATH_LOG                L"\\Log"    ///< 로그 경로
#define STR_PATH_DATAFILES          L"\\DataFiles\\"
#define STR_PATH_BACKUP             L"\\DataFiles\\Backup\\"
#define STR_PATH_UPDATE             L"\\DataFiles\\Update\\"
#define STR_PATH_WAVE               L"\\Wave\\"

// by zzang9un 2013.09.13 : File Name 정의
#define FILENAME_MAP                L"map.dat"
#define FILENAME_STATION            L"station.std"
#define FILENAME_TEACHING           L"teaching.tcd"
#define FILENAME_PARAM_DRIVING      L"Driving.ini"
#define FILENAME_PARAM_TRANS        L"Trans.ini"
#define FILENAME_AMCOUT             L"amc.out"
#define FILENAME_ERRORCODE          L"ErrorCode.ini"
#define FILENAME_ASSISTANT          L"OHTAssistant.exe"
#define FILENAME_LOGDELETER			L"LogDeleter.bat"


// 2016.12.31   파일 업 다운로드 (TP) 기능 관련하여 정의 추가함. 위 정의는 불필요 검토후 삭제예정
/* Map */
#define STR_FILENAME_CID            L"CID.dat"
#define STR_FILENAME_COLLISION      L"COLLISION.dat"
#define STR_FILENAME_FLOWCONTROL    L"FLOWCONTROL.dat"
#define STR_FILENAME_MAP            L"MAPDATA.dat"
#define STR_FILENAME_PATHDATA       L"PATHDATA.dat"

/* Station */
#define STR_FILENAME_STATION        L"STATION.dat"

/* Teaching */
#define STR_FILENAME_TEACHING       L"TEACHING.dat"


#define STR_FILENAME_JOBORDER		L"JOBORDER.dat"
#define STR_FILENAME_MAPMAKERFILE	L"MAPMAKERFILE.dat"


/* map location */
#define STR_PATH_MAPFILES          L"\\DataFiles\\MAPSET\\"




// by zzang9un 2013.09.13 : File 확장자 정의(File Extention)
#define FILE_EXT_MAP                L"*.dat"
#define FILE_EXT_STATION            L"*.std"
#define FILE_EXT_TEACHING           L"*.tcd"
#define FILE_EXT_PARAMETER          L"*.ini"
#define FILE_EXT_AMCOUT             L"*.out"


// 2016.12.31   파일 업 다운로드 (TP) 기능 관련하여 정의 추가함. 위 정의는 불필요 검토후 삭제예정
#define STR_FILE_EXT_STATION            L"*.dat"
#define STR_FILE_EXT_TEACHING           L"*.dat"



// wave 파일 이름
#define NAME_WAV_DRIVING            L"Driving.wav"
#define NAME_WAV_TRANS              L"Trans.wav"
#define NAME_WAV_ERROR              L"Error.wav"
#define NAME_WAV_OBS                L"Obstacle.wav"
#define NAME_WAV_DETECT             L"Detect.wav"
#define NAME_WAV_PAUSE              L"Pause.wav"
#define NAME_WAV_OBSWARNNING        L"ObsWarnning.wav"

//Data Type 2013.08.06 by doori.shin
#define DATATYPE_STATION			0x00
#define DATATYPE_TEACHING			0x01
#define	DATATYPE_MAP				0x02

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

#define ERR_AXIS1_F_AMP_FAULT               613     ///<  AMP Fault 상태 발생
#define ERR_AXIS1_R_AMP_FAULT               614

#define ERR_AXIS1_F_ERROR_LIMIT             633     ///<  위치오차가 Limit 값보다 클때 발생
#define ERR_AXIS1_R_ERROR_LIMIT             634     ///<  위치오차가 Limit 값보다 클때 발생

#endif // Def_OHTH

