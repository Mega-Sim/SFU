//---------------------------------------------------------------------------
/**
@file    STD_TeachingH.h
@version OHT 7.5
@brief   STANDARD MAP과 관련된 Class, 구조체를 정의한 Header 파일
*/

#ifndef STD_TeachingH
#define STD_TeachingH

#include <map>
#include "Def_DataType.h"
#include "Def_OHT.h"
//#include "Def_PathInfo.h"
//#include "Def_DataFile.h"
#include "Def_DataFile_STD.h"


//typedef파일명 Def_Variables.h
using namespace std;

// -------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack


/**
@brief   TEACHING.DAT의 data 관련 구조체
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_Teaching
{
	UINT StationID;                     ///< Station Id
	BYTE PortType;                      ///< 이적재 Port 정보 ex) EQ, STB-L, STB-R, UTB"
	int Hoist;                         ///< 이적재 Hoist 이동거리(1/10mm)
	int Shift;                         ///< 이적재 Shift 이동거리(1/10mm)
	int Rotate;                        ///< 이적재 Rotate 이동거리(radian)
	BYTE PIOType;                       ///< 0:IR, 1:RF, 2:Hybrid 5.0GHz
	BYTE PIODirection;                  ///< PIO 방향(좌, 우)
	UINT PIOID;                         ///< RF-PIO Setting ID 정보
	UINT PIOChannel;                    ///< RF-PIO Setting Channel 정보
	BYTE PIOTimeLevel;                  ///< PIO Time out 지연 시간 레벨
	BYTE AutoRecovery;                  ///< AutoRecovery
	BYTE HandDetectEQ;                  ///< HandDetectEQ
	UINT Reserved1;                     ///< Reserved 1
} NODE_INFO_STD_TEACHING;
//stuct size : 34Byte

#pragma pack(pop) // Pack 해제

// -------------------------------------------

/**
@class   STD_Station
@author  KJD
@date    2016.03.30
@brief   OHT에 사용되는 Standard Map에 관련된 Data, 함수를 정의한 class
*/
class STD_Teaching
{
private:


public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_TEACHING *m_NodeArray; ///< Node 동적 배열 포인터

	std::map<int, int> m_TeachingMap;

	STD_Teaching();
	~STD_Teaching();

	void InitMap(int NodeNum);

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
@class   MapDataManager
@author  KJD
@date    2016.03.30
@brief   Map Data를 관리하는 Class
*/
class TeachingManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_TeachingFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	TeachingManager_STD();
	~TeachingManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_Teaching *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_Teaching *pMapdata);

};
#endif //STD_StationH
