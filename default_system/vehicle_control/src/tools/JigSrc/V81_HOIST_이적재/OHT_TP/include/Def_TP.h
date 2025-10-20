/**
@file    Def_TP.h
$Rev:: 735           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-03-22 1#$:  Date of last commit
@version OHT 7.0
@brief   TP 프로그램에 사용되는 상수, 함수 정의
*/

#ifndef Def_TPH
#define Def_TPH
//---------------------------------------------------------------------------
#include "Def_OHT.h"
#include "Def_Protocol.h"
#include "Def_TPCommand.h"
#include "Def_DataType.h"

#define MODE_KOREAN         1
#define LANGUAGE_MODE       MODE_KOREAN

#define LOG_DELETE_DAY		10			// 오래된 로그 파일 기준 날짜

// -------------------------------------------
// 초기화 관련 정보 : Splash 표시용
// -------------------------------------------
typedef enum INIT_STATE_
{
    STATE_NONE       = 0,    ///< State Node
    STATE_CONNECTCHECK,      ///< Data Server Connect 체크
    STATE_DATAVERREQUEST,    ///< Data 버전 요청
    STATE_DATAVERCHECK,      ///< Data 버전 체크
    STATE_STATIONSYNC,       ///< Station Map Data 다운로딩
    STATE_STATIONSYNCDONE,   ///< Station Map Data 다운로딩 완료
    STATE_STATIONSYNCFAIL,   ///< Station Map Data 다운로딩 Fail
    STATE_TEACHINGSYNC,      ///< Teaching Data 다운로딩
    STATE_TEACHINGSYNCDONE,  ///< Teaching Data 다운로딩 완료
    STATE_TEACHINGSYNCFAIL,  ///< Teaching Data 다운로딩 Fail
    STATE_DATASYNCCOMPLETE,  ///< Data Sync 완료된 상태
    STATE_DATASYNCFAIL,      ///< Data Sync Fail된 상태  
    STATE_CONNECTFAIL,       ///< Data Sever Connect 실패
    STATE_END,               ///< 초기화 작업 종료
    STATE_MAX                ///< State Max
} INIT_STATE;

// -------------------------------------------
// TColor 색상 변수 define
// -------------------------------------------
#define COLOR_AQUA          clAqua
#define COLOR_BLACK         clBlack
#define COLOR_BLUE          clBlue
#define COLOR_CREAM         clCream
#define COLOR_DARKGRAY      clDkGray
#define COLOR_FUCHSIA       clFuchsia
#define COLOR_GRAY          clGray
#define COLOR_GREEN         clGreen
#define COLOR_LIME          clLime
#define COLOR_LIGHTGRAY     clLtGray
#define COLOR_MAROON        clMaroon
#define COLOR_MEDIUMGRAY    clMedGray
#define COLOR_MINTGREEN     clMoneyGreen
#define COLOR_NAVYBLUE      clNavy
#define COLOR_OLIVE         clOlive
#define COLOR_PURPLE        clPurple
#define COLOR_RED           clRed
#define COLOR_SILVER        clSilver
#define COLOR_SKYBLUE       clSkyBlue
#define COLOR_WHITE         clWhite
#define COLOR_YELLOW        clYellow

// -------------------------------------------
// 파일 관련 Define
// -------------------------------------------
#define STR_NETWORKDRIVE            L"Z:"       ///< 네트워크 드라이브 이름
#define STR_LOGDRIVE            	L"Y:"


// by zzang9un 2013.09.13 : Data File 종류
#define DATAFILE_MAP                0 ///< Map Data 
#define DATAFILE_STATION            1 ///< Station Data
#define DATAFILE_TEACHING           2 ///< Teaching Data
#define DATAFILE_AMCOUTFILE         3 ///< AMC Out File
#define DATAFILE_PARAM_DRIVING      4 ///< Parameter(Driving) Data
#define DATAFILE_PARAM_TRANS        5 ///< Parameter(Trans) Data
#define DATAFILE_NODEDISABLE        6 ///< Node Disable Data
#define DATAFILE_USERSELECT         7 ///< User Select File
#define DATAFILE_ASSISTANT          8 ///< Assistant Program
#define DATAFILE_LOG                9 ///< Log

#define FILENAME_IOMAP L"IOMap.ini"

// -------------------------------------------
// 선언 변수 문자열 변환 함수
// -------------------------------------------
String GetStringMode(BYTE Mode);
String GetStringStatus(BYTE Status);
String GetStringOHTVer(BYTE OHTVer);
String GetStringOHTType(BYTE OHTType);
BYTE StrToPortType(String strPortType);
String PortTypeToStr(BYTE PortType);
String TagTypeToStr(BYTE TagType);

//---------------------------------------------------------------------------
#endif
