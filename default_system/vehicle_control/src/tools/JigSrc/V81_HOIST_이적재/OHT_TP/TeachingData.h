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
#include "Def_DataType.h"
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
    int  Hoist;                 ///< Hoist Teching 값(mm)
    int  Shift;                 ///< Shift Teching 값(mm)
    int  Rotate;                ///< Rotate Teching 값(radian)
    BYTE PIODirection;          ///< PIO 방향(Left, Right)
    BYTE PIOTimeLevel;          ///< PIO Time Level
    BYTE LookDownLevel;         ///< LookDownLevel
	uTRANS_OPTION TransOption;   ///< 기타 이적재 옵션
	UINT PIOID;             	///< RF PIO ID
	BYTE PIOCS;					///< RF PIO CS 신호 선택
	UINT Reserved1;             ///< Reserved 4Byte
	BYTE Reserved2;				///< Reserved 1Byte
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
    int LoadDataFromFile(String FileName);
	int SaveDataToFile(String FileName);
	FILE_HEADER* GetFileHeader();
	void SetFileHeader(FILE_HEADER *pHeader);

    // Map 자료구조 관련 함수
    void Init();
    int GetCount();

    // Map 자료구조 추가, 업데이트, 삭제, 검색  함수
    int Insert(TEACHING_DATA *addStationData);
    int Update(TEACHING_DATA *updateSTD);
    int Delete(UINT StationID, BYTE PortType);
    TEACHING_DATA* Find(UINT StationID, BYTE PortType);
};

//---------------------------------------------------------------------------

#endif
