/**
@file    Def_DataFile_STD.h
@brief   Standard Map Data File에 공통으로 적용되는 Header 파일
*/

#ifndef Def_DataFile_STD
#define Def_DataFile_STD

#include <IniFiles.hpp>
#include <iostream>
#include <fstream>
#include <locale.h> // 경로명에 한글이 있는 경우를 위해 추가
#include "../Common/Def_DataType.h"

#define ERR_FILTER_FORMAT_REVISION      -100  //Define 값이 음수인 경우 괄호치면 중복선언 오류발생
#define ERR_FILTER_FORMAT_UPDATETIME    -101
#define ERR_FILTER_FORMAT_COUNT         -102
#define ERR_DATA_SIZE                   -110
#define ERR_DATA_LIMIT_REVISION         -200
#define ERR_DATA_LIMIT_UPDATETIME       -201
#define ERR_DATA_LIMIT_COUNT            -202

#if(OHT_NAME == OHT_NAME_P4_MASK)
#define TOTAL_MAPSET_FILES (6) //6->5 변경 맵세트 파일의 전체 숫자
#else
#define TOTAL_MAPSET_FILES (7) //맵세트 파일의 전체 숫자
#endif


#define NODE_COUNT_MAX        (999999) //최대 노드 수 999999

#define FILE_TYPE_CID               (1)
#define FILE_TYPE_COLLISION	    	(2)
#define FILE_TYPE_FLOWCONTROL       (3)
#define FILE_TYPE_LOOKDOWN		    (4)
#define FILE_TYPE_MAPDATA           (5)
#define FILE_TYPE_STATION           (6)
#define FILE_TYPE_TEACHING          (7)
#define FILE_TYPE_PATHDATA          (8)


// -------------------------------------------
// Map 관련 에러 코드
// -------------------------------------------
#define ERR_MAP_FILEOPENFAIL        (-1000)
#define ERR_MAP_FILESAVEFAIL        (-2000)
#define ERR_MAP_HEADERINVALID       (-3000)
#define ERR_MAP_FILEINVALID         (-4000)


#define VESION_STR_LENGTH			(80)

#define VERSION_MAJOR_FILTER		(0xFF000000)
#define VERSION_MINOR_FILTER		(0x00FFF000)
#define VERSION_REVISION_FILTER		(0x00000FFF)
#define VERSION_MAJOR_SHIFT			(24)
#define VERSION_MINOR_SHIFT			(12)
#define VERSION_REVISION_SHIFT		(0)



#if 0 // Def_OHT.h 에 정의
// -------------------------------------------
// Map 파일 경로 및 파일 이름
// -------------------------------------------
#define STR_DATAFILE_PATH           L"\\DataFiles\\MapSet\\"
#define STR_DATAFILE_BACKUP_PATH    L"\\DataFiles\\Backup\\"
#define STR_FILENAME_CID            L"CID.DAT"
#define STR_FILENAME_COLLISION      L"COLLISION.DAT"
#define STR_FILENAME_FLOWCONTROL    L"FLOWCONTROL.DAT"
#define STR_FILENAME_MAP            L"MAPDATA.DAT"
#define STR_FILENAME_STATION        L"STATION.DAT"
#define STR_FILENAME_TEACHING       L"TEACHING.DAT"
#define STR_FILENAME_PATHDATA       L"PATHDATA.DAT"
#endif
//---------------------------------------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack

/**
@brief   Data File Header 구조체
@author  KJD
@date    2016.3.30
@note    표준화 맵 해더는 동일 하므로 공용으로 사용
         개별 헤더 적용시 "HEADER_INFO_STD_파일이름" 구조체 사용
*/
typedef struct FILE_HEADER_STD
{
	UINT ModelID;                     ///< 적용 가능 OHT 모델 정보   ex) 적용 가능 기종 ID
	UINT Version;                     //< bit별로 Major(8bits), Minor(12bits), Revision(12bits)를  할당
                                      // ex) 버전정보의 값이 98.12.3456인 경우 
                                      //     98 → 2진수로 "110 0010",  Hex로 0x62
                                      //     12 → 2진수로 "1100", Hex로   0xC
                                      //     3456 → 2진수로 "1101 1000  0000", Hex로 0xD80
                                      //     버전정보의 값은 Hex로 0x6200CD80이 되며   10진수로는 1,644,219,776이 됨
	UINT DataType;                    ///< Data 종류 및 옵션				  
                                      //  0 bit : 조정/운용 확인 bit
                                      //  조정용 MAP이면 ON, 운용용 MAP이면 OFF.
                                      //  Project.tgz에 파일을 보존할 때  bit를 OFF(운용용)   조정 MAP에 보존할 경우에는 
                                      //  bit를 ON(조정용)로 한다.
                                      //  1 bit : 파라미터 변경유bit
                                      //  본체가 이재 티칭이나 주행 학습을 실시했을 경우 ON로 한다.
                                      //  StationEditor가 읽어냈을 경우, bit를 OFF로 한다.
	BYTE VersionString[VESION_STR_LENGTH];           ///< 버전정보의 문자열 - 예>일자, 사용자, 파일 명칭
	UINT DataCount;                   ///< Data 전체 수
}FILE_HEADER_STD;


/**
@brief   Data File Header 구조체
@author  KJD
@date    2016.3.30
@note    표준화 맵 해더는 동일 하므로 공용으로 사용
         개별 헤더 적용시 "HEADER_INFO_STD_파일이름" 구조체 사용
*/
typedef struct MAPMAKERFILE_HEADER_STD
{
	UINT ModelID;                     ///< 적용 가능 OHT 모델 정보   ex) 적용 가능 기종 ID
	UINT Version;                     //< bit별로 Major(8bits), Minor(12bits), Revision(12bits)를  할당
                                      // ex) 버전정보의 값이 98.12.3456인 경우
                                      //     98 → 2진수로 "110 0010",  Hex로 0x62
                                      //     12 → 2진수로 "1100", Hex로   0xC
                                      //     3456 → 2진수로 "1101 1000  0000", Hex로 0xD80
                                      //     버전정보의 값은 Hex로 0x6200CD80이 되며   10진수로는 1,644,219,776이 됨
	UINT DataType;                    ///< Data 종류 및 옵션
                                      //  0 bit : 조정/운용 확인 bit
                                      //  조정용 MAP이면 ON, 운용용 MAP이면 OFF.
                                      //  Project.tgz에 파일을 보존할 때  bit를 OFF(운용용)   조정 MAP에 보존할 경우에는
                                      //  bit를 ON(조정용)로 한다.
                                      //  1 bit : 파라미터 변경유bit
                                      //  본체가 이재 티칭이나 주행 학습을 실시했을 경우 ON로 한다.
                                      //  StationEditor가 읽어냈을 경우, bit를 OFF로 한다.
	BYTE VersionString[VESION_STR_LENGTH];           ///< 버전정보의 문자열 - 예>일자, 사용자, 파일 명칭
	short SDataCount;                   ///< Data 전체 수
	short TDataCount;                   ///< Data 전체 수
}MAPMAKERFILE_HEADER_STD;


#pragma pack(pop) // 1byte로 Pack
//---------------------------------------------------------------------------

/**
@brief   File에 저장할 STD File Header를 Make해주는 함수
@author  KJD
@date    2016.07.05
@param   pHeader : 헤더파일
@return  Make된 FILE_HEADER 구조체
*/
FILE_HEADER_STD MakeFileHeader_STD(FILE_HEADER_STD* pHeader)
{
    int Error = 0;
    
    FILE_HEADER_STD RtnFileHeader;
    memset(&RtnFileHeader, 0x00, sizeof(RtnFileHeader));

    if( pHeader->DataCount == 0) 
        Error = ERR_DATA_LIMIT_COUNT;
    
    if(Error == NO_ERR)
    {
        RtnFileHeader.ModelID = pHeader->ModelID;
        RtnFileHeader.Version = pHeader->Version;
        RtnFileHeader.DataType = pHeader->DataType;
		RtnFileHeader.DataCount = pHeader->DataCount;
		memcpy(RtnFileHeader.VersionString, pHeader->VersionString, VESION_STR_LENGTH);
    }

    return RtnFileHeader;
}

//---------------------------------------------------------------------------

/**
@brief   파일에서 읽은 Header를 파싱하여 리턴하는 함수
@author  KJD
@date    2016.3.30
@param   pHeader : 검사할 파일 Header 구조체
@return  Parsing한 File Header 구조체를 리턴한다. ???
*/
FILE_HEADER_STD ParsingFileHeader_STD(FILE_HEADER_STD* pHeader)
{
    FILE_HEADER_STD RtnHeader;
    memset(&RtnHeader, 0x00, sizeof(RtnHeader));

    //int Error = NO_ERR;

    // 헤더가 올바른지 확인
	if((pHeader->DataCount <= 0) || (pHeader->DataCount > 10000))
	{
		//Error = ERR_DATA_LIMIT_COUNT;
		RtnHeader.DataCount = NULL;
	}
	else
	{
		RtnHeader.ModelID = pHeader->ModelID;
		RtnHeader.Version = pHeader->Version;
		RtnHeader.DataCount = pHeader->DataCount;
		memcpy(RtnHeader.VersionString,pHeader->VersionString,sizeof(pHeader->VersionString));
	}

	//Todo : 추가적인 헤더 판단 조건 확인

    return RtnHeader;
}

//---------------------------------------------------------------------------
#endif

