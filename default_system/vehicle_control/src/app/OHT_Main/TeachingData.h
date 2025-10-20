//---------------------------------------------------------------------------
/**
@file    TeachingData.h
$Rev:: 477           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-01-07 1#$:  Date of last commit
@version OHT 7.0
@brief   TeachingData Class를 정의한 Header File
*/

#ifndef TeachingDataH
#define TeachingDataH

#include <map>
#include <utility>

#include "Def_OHT.h"
#include "Def_DataFile.h"
#include "../Common/Def_DataType.h"
#include "STD_Teaching.h"
//---------------------------------------------------------------------------

// by zzang9un 2013.07.19 : 통신 프로토콜 전송을 위해 1바이트로 Pack
#pragma pack(push, 1) // 1byte로 Pack

// Error Code
#define ERR_TDATA_MAKEMAP       -1
#define ERR_TDATA_FILEOPEN      -1001
#define ERR_TDATA_FORMAT        -2000   ///< Data Format Error
#define ERR_TDATA_DUPLICATION   -2001   ///< Data 중복
#define ERR_TDATA_INSERTFAIL    -2002   ///< Map Insert Fail
#define ERR_TDATA_DELETEFAIL    -2003   ///< Map Delete Fail
#define ERR_TDATA_UPDATEFAIL    -2004   ///< Map Update Fail
#define ERR_TDATA_NODATA        -2005   ///< Map Data 없음
#define ERR_TDATA_SAVEFAIL      -2006   ///< Save 에러
#define ERR_TDATA_MISSMATCH     -3000   ///< Data Format Error

// by zzang9un 2013.07.18 : 비트 필드가 포함된 총 변수 개수로 변경
#define NUM_TEACHING_DATA_ELEMENT    16   ///< Station Data 구조체 변수 개수

/**
@brief   Teaching Data 구조체
@author  zzang9un
@date    2013.07.18
*/
typedef struct TEACHING_DATA_
{
    UINT StationID;             ///< Station ID
    BYTE PortType;              ///< 작업 Port Type
    int  Hoist;                 ///< Hoist Teching 값(1/10mm)
    int  Shift;                 ///< Shift Teching 값(1/10mm)
    int  Rotate;                ///< Rotate Teching 값(1/10 degree)
    int  STBOffSet;             ///< STBOffSet(1/10mm)
    BYTE PIOType;				///< 0:IR, 1:RF, 2:Hybrid 5.0GHz
    BYTE PIOChannel;			///< PIOChannel
    BYTE PIODirection;          ///< PIO 방향(Left, Right)
    BYTE PIOTimeLevel;          ///< PIO Time Level
	BYTE LookDownLevel;         ///< LookDownLevel
	BYTE SpecialEQLevel;        ///< SpecialEQ (0~9)
	uTRANS_OPTION TransOption;   ///< 기타 이적재 옵션 구조체 생성 필요
	UINT PIOID;             	///< RF PIO ID
	BYTE PIOCS;					///< RF PIO CS 신호 선택
	BYTE MaterialType;             ///< Reserved 4Byte
	BYTE Reserved3;             ///< Reserved 4Byte
	char STBL_Shift;            ///< STBL_Shift 티칭 값에 3mm추가 soming
	//UINT Reserved1;             ///< Reserved 4Byte  위에 STBL_Shift 위해 Byte로 나눔.
	DEFAULT_MAPINFO IsDefaultMap; ///<Default값
} TEACHING_DATA;

#pragma pack(pop) // Pack 해제

// Teaching Data를 위한 multimap 설정
typedef std::multimap<UINT, TEACHING_DATA *> TEACHING_DATA_MMAP;
typedef TEACHING_DATA_MMAP::value_type TEACHING_DATA_VALUE;
typedef TEACHING_DATA_MMAP::iterator TEACHING_DATA_MMAP_ITER;
typedef std::pair<UINT, TEACHING_DATA *> TEACHING_DATA_PAIR;
typedef std::pair<TEACHING_DATA_MMAP_ITER, bool> TEACHING_DATA_MMAP_RESULT;
typedef std::pair<TEACHING_DATA_MMAP_ITER, TEACHING_DATA_MMAP_ITER> TEACHING_DATA_MMAP_RANGE;

/**
@class   TeachingDataManager
@date    2013.06.19
@author  zzang9un
@brief   Teaching Data를 관리하는 Class

         1. Teaching Data는 map(자료구조)로 구성되어 있음
         2. binary 형태의 파일만 읽어올 수 있음
*/
class TeachingDataManager
{
public:
    bool m_bDataLoaded;          ///< Data Load 여부 flag
	bool m_bDataModified;      ///< 임시 Data Load 여부 flag

	FILE_HEADER m_FileHeader;

public:
    TEACHING_DATA_MMAP m_TeachingData;   ///< Teaching Data 자료구조(map)

    TeachingDataManager();
    ~TeachingDataManager();

    // 파일 처리 함수
	int SaveDataToFile(String FileName, int FileType);


    // Map 자료구조 관련 함수
    void Init();
    int GetCount();

    // Map 자료구조 추가, 업데이트, 삭제, 검색  함수
    int Insert(TEACHING_DATA *addStationData);
    int TeachingUpdate(TEACHING_DATA *updateSTD);
    int Delete(UINT StationID, BYTE PortType);
	TEACHING_DATA* Find(UINT StationID, BYTE PortType);

	int LoadDataFromSTDMap(NODE_INFO_STD_TEACHING *m_NodeArray_Teach, UINT DataNum);
};

//---------------------------------------------------------------------------

#endif
