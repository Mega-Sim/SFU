#pragma once

#include "stdafx.h"
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region  MAPDATA.dat 파일 파싱 코드




#define NODE_COUNT_MAX        (999999) //최대 노드 수 999999
#define VESION_STR_LENGTH			(80)

// -------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack
typedef struct Node_Info_STD_Map
{
	UINT NodeID;                        ///< 주행 Node Id
	BYTE NodeType;                      ///< Node의 종류
										//   0x1: Lift In, 0x2: Lift out 0x3: MTL In, 0x4 : MTL Middle, 0x5:MTL out
	UINT LeftNodeID;                    ///< (왼쪽) 연결된 Node Id
	UINT LeftNodeDistance;              ///< (왼쪽) 연결된 Node까지의 거리(mm)
	WORD LeftNodeSpeed;                 ///< (왼쪽) 연결된 Node까지의 이동 속도(mm/s)
	BYTE LeftSteering;                  ///< (왼쪽) 연결된 Point까지의 Steer 방향
	BYTE LeftBranch;                    ///< (왼쪽) 연결된 Point까지의 분기정보(일반분기,N분기)
	UINT RightNodeID;                   ///< (오른쪽) 연결된 Node Id
	UINT RightNodeDistance;             ///< (오른쪽) 연결된 Node까지의 거리(mm)
	WORD RightNodeSpeed;                ///< (오른쪽) 연결된 Node까지의 이동 속도(mm/s)
	BYTE RightSteering;                 ///< (오른쪽) 연결된 Point까지의 Steer 방향
	BYTE RightBranch;                   ///< (오른쪽) 연결된 Point까지의 분기정보(일반분기,N분기)
										//	UINT ResetNodeMain;                 ///< 통과_허가_Reset Node ID(Main)
										//	UINT ResetNodeSub;                  ///< 통과_허가_Reset Node ID(Sub.)
										//	UINT ResetNodeRedundance1;          ///< 통과_허가_Reset Node ID(Redundance 1)
										//	UINT ResetNodeRedundance2;          ///< 통과_허가_Reset Node ID(Redundance 2)
	UINT Reserved_1;                    ///< Reserved_1
	UINT Reserved_2;                    ///< Reserved_2
	UINT Reserved_3;                    ///< Reserved_3

	Node_Info_STD_Map() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_MAP;
//stuct size : 57Byte
#pragma pack(pop) // Pack 해제

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

	FILE_HEADER_STD() { ZeroMemory(this, sizeof(*this)); }
}FILE_HEADER_STD;


static const unsigned short crc16tab[256] = {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

unsigned short crc16_ccitt(const char *buf, int len)
{
	register int counter;
	register unsigned short crc = 0;
	for (counter = 0; counter < len; counter++)
		crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ *(char *)buf++) & 0x00FF];
	return crc;
}


/**
@brief   Big-Endian을 Little-Endian으로 변환해주는 함수
@author  zzang9un
@date    2012.11.01
@param   objp : Convert할 Data의 주소
@tparam  Type : template parameter
*/
template <class Type>
inline void ConvertEndian(Type *objp)
{
	unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
	std::reverse(memp, memp + sizeof(Type));
}


class STD_MAP
{
private:


public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_MAP *m_NodeArray; ///< Node 동적 배열 포인터
	FILE_HEADER_STD MapFileHeader;

	STD_MAP()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;
	}
	~STD_MAP()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// 동적 생성하기 전 Null인지를 확인 후 생성
		if (m_NodeArray != NULL)
		{
			// NULL이 아닌 경우는 delete 후에 동적 생성
			// 맵 데이터 갱신시 사용
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_MAP[NodeNum];
		}
		else
		{
			//NodeNum Max값으로 테스트 필요 ??
			m_NodeArray = new NODE_INFO_STD_MAP[NodeNum];
		}
	}
	bool LoadMapFile_MapData(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

		
		bool MapFileLoaded = false;
		bool Error = false;
		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&MapFileHeader, 0x00, sizeof(MapFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header 파싱  --> 현재 파싱 필요없음.
			//m_MapFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&MapFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&MapFileHeader.ModelID);
			ConvertEndian(&MapFileHeader.Version);
			ConvertEndian(&MapFileHeader.DataCount);
			ConvertEndian(&MapFileHeader.DataType);

			if ((MapFileHeader.DataCount > 0) && (MapFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data 총 size
								  // Step 4. 파일에서 Load한 내용에 따라 Init
				InitMap(MapFileHeader.DataCount);

				// Step 5. Node Size만큼 Read
				NodeSize = MapFileHeader.DataCount * sizeof(NODE_INFO_STD_MAP);

				fs.read((char *)m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF 가 없으면 수정 전 맵으로 간주
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						Error = false;
						return Error;
					}
				}
				else
				{
					//ADD_LOG("[EOF_ERR] Map Data File");
					//맵 구조 수정 후 주석 부분 적용
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < m_NodeNum; i++)
				{
					ConvertEndian(&m_NodeArray[i].NodeID);
					ConvertEndian(&m_NodeArray[i].LeftNodeID);
					ConvertEndian(&m_NodeArray[i].LeftNodeDistance);
					ConvertEndian(&m_NodeArray[i].LeftNodeSpeed);
					ConvertEndian(&m_NodeArray[i].RightNodeID);
					ConvertEndian(&m_NodeArray[i].RightNodeDistance);
					ConvertEndian(&m_NodeArray[i].RightNodeSpeed);
					i = i; //debug break point 용도 추후 삭제  
				}
				MapFileLoaded = true; // Map File Loaded flag 변경
			}
		}
		else
		{
			Error = true;
		}

		fs.close();
		return true;
	}

	bool Save_asText(CString filename)
	{
		// OCSLayout파일에 뒤쪽에 붙여서 MapData 정보를 기록한다. [MAP_DATA] 로 시작
		CStdioFile layoutFile;
		if (layoutFile.Open(filename, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		CString strLine;
		// map data 기록
		for (int i = 0; i < m_NodeNum; i++)
		{
			NODE_INFO_STD_MAP *p = &m_NodeArray[i];

			strLine.Format("%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d\n",
				p->NodeID,
				p->NodeType,
				p->LeftNodeID,
				p->LeftNodeDistance,
				p->LeftNodeSpeed,
				p->LeftSteering,
				p->LeftBranch,
				p->RightNodeID,
				p->RightNodeDistance,
				p->RightNodeSpeed,
				p->RightSteering,
				p->RightBranch
			);

			layoutFile.WriteString(strLine);
		}
		layoutFile.Close();
		return true;
	}
};


#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region CID.dat 파일 파싱 코드



// -------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack


/**
@brief   CID.DAT의 data 관련 구조체
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_CID
{
	BYTE LayoutType;                    ///< 합류지점, 경사로, MTL, Lift, 중계(Interface), Pass Door, Fire Door
										//UINT LayoutID;                    // UI 표시용 ID
	BYTE LayoutID[6];                   ///< UI 표시용 ID : CID001, CID002...
	UINT ResetNode;                     ///< Layout 내 Reset Node
	UINT StopNode1;                     ///< Layout 내 Stop Node 1
	UINT StopNode2;                     ///< Layout 내 Stop Node 2
	BYTE ResetNodeType;                 ///< Layout 내 Reset Node Type
	BYTE CIDDirection1;                 ///< Stop Node 1 → Reset Node에서의 CID-O 방향
	UINT CommStartNode1_1;              ///< Stop Node 1 → Reset Node에서의 통신 시작(CID-O On) Node 1
	UINT CommStartNode1_2;              ///< Stop Node 1 → Reset Node에서의 통신 시작(CID-O On) Node 2
	UINT CommStartNode1_3;              ///< Stop Node 1 → Reset Node에서의 통신 시작(CID-O On) Node 3
	UINT CommStartNode1_4;              ///< Stop Node 1 → Reset Node에서의 통신 시작(CID-O On) Node 4
	UINT CommStartNode1_5;              ///< Stop Node 1 → Reset Node에서의 통신 시작(CID-O On) Node 5
	INT  CommStartNode1_Offset;         ///< CommStartNode1_1 에서 CID동작 시작지점 까지의 거리(mm, 기본값0, 맵작성시 설계값 기입)
	BYTE CIDDirection2;                 ///< Stop Node 2 → Reset Node에서의 CID-O 방향
	UINT CommStartNode2_1;              ///< Stop Node 2 → Reset Node에서의 통신 시작(CID-O On) Node 1
	UINT CommStartNode2_2;              ///< Stop Node 2 → Reset Node에서의 통신 시작(CID-O On) Node 2
	UINT CommStartNode2_3;              ///< Stop Node 2 → Reset Node에서의 통신 시작(CID-O On) Node 3
	UINT CommStartNode2_4;              ///< Stop Node 2 → Reset Node에서의 통신 시작(CID-O On) Node 4
	UINT CommStartNode2_5;              ///< Stop Node 2 → Reset Node에서의 통신 시작(CID-O On) Node 5
	INT  CommStartNode2_Offset;         ///< CommStartNode2_1 에서 CID동작 시작지점 까지의 거리(mm, 기본값0, 맵작성시 설계값 기입)
	UINT Reserved1;                     ///< Reserved_1
	UINT Reserved2;                     ///< Reserved_2

	Node_Info_STD_CID() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_CID;
//stuct size : 79Byte
#pragma pack(pop) // Pack 해제

class STD_CID
{
private:


public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_CID *m_NodeArray; ///< Node 동적 배열 포인터

	//std::map<int, int> m_CIDMap;

	STD_CID()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;


		// CIDMap 초기화
		//m_CIDMap.clear();
	}
	~STD_CID()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;


		//m_CIDMap.clear();
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// 동적 생성하기 전 Null인지를 확인 후 생성
		if (m_NodeArray != NULL)
		{
			// NULL이 아닌 경우는 delete 후에 동적 생성
			// 맵 데이터 갱신시 사용
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_CID[NodeNum];
		}
		else
		{
			//NodeNum Max값으로 테스트 필요 ??
			m_NodeArray = new NODE_INFO_STD_CID[NodeNum];
		}

		//m_CIDMap.clear();
	}
	bool LoadMapFile_CID(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

		FILE_HEADER_STD m_CIDFileHeader; ///< Map File Header

										 //bool Error = false;
		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&m_CIDFileHeader, 0x00, sizeof(m_CIDFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));

			// Step 3. Header 파싱  --> 현재 파싱 필요없음.
			//m_CIDFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&m_CIDFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&m_CIDFileHeader.ModelID);
			ConvertEndian(&m_CIDFileHeader.Version);
			ConvertEndian(&m_CIDFileHeader.DataCount);
			ConvertEndian(&m_CIDFileHeader.DataType);

			if ((m_CIDFileHeader.DataCount > 0) && (m_CIDFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data 총 size
								  // Step 4. 파일에서 Load한 내용에 따라 Init
				InitMap(m_CIDFileHeader.DataCount);

				// Step 5. Node Size만큼 Read
				NodeSize = m_CIDFileHeader.DataCount * sizeof(NODE_INFO_STD_CID);

				fs.read((char *)m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF 가 없으면 수정 전 맵으로 간주
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						return false;
					}
				}
				else
				{
					//ADD_LOG("[EOF_ERR] CID File");
					//맵 구조 수정 후 주석 부분 적용
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < m_NodeNum; i++)
				{
					//ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
					ConvertEndian(&m_NodeArray[i].ResetNode);
					ConvertEndian(&m_NodeArray[i].StopNode1);
					ConvertEndian(&m_NodeArray[i].StopNode2);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_1);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_2);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_3);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_4);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_5);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_Offset);

					ConvertEndian(&m_NodeArray[i].CommStartNode2_1);
					ConvertEndian(&m_NodeArray[i].CommStartNode2_2);
					ConvertEndian(&m_NodeArray[i].CommStartNode2_3);
					ConvertEndian(&m_NodeArray[i].CommStartNode2_4);
					ConvertEndian(&m_NodeArray[i].CommStartNode2_5);
					ConvertEndian(&m_NodeArray[i].CommStartNode2_Offset);
					i = i; //debug break point 용도 추후 삭제  
				}
				//m_MapFileLoaded = true; // Map File Loaded flag 변경
			}
		}
		else
		{
			return false;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString path)
	{
		CStdioFile layoutFile;
		if (layoutFile.Open(path, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		// cid data 기록
		CString strLine;
		for (int i = 0; i < m_NodeNum; i++)
		{
			Node_Info_STD_CID *p = &m_NodeArray[i];

			strLine.Format("%d/%c%c%c%c%c%c/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d\n",
				p->LayoutType,
				p->LayoutID[0], p->LayoutID[1], p->LayoutID[2], p->LayoutID[3], p->LayoutID[4], p->LayoutID[5],
				p->ResetNode,
				p->StopNode1,
				p->StopNode2,
				p->ResetNodeType,
				p->CIDDirection1,
				p->CommStartNode1_1
				, p->CommStartNode1_2
				, p->CommStartNode1_3
				, p->CommStartNode1_4
				, p->CommStartNode1_5
				, p->CommStartNode1_Offset
				, p->CIDDirection2
				, p->CommStartNode2_1
				, p->CommStartNode2_2
				, p->CommStartNode2_3
				, p->CommStartNode2_4
				, p->CommStartNode2_5
				, p->CommStartNode2_Offset
				, p->Reserved1
				, p->Reserved2
			);

			layoutFile.WriteString(strLine);
		}
		layoutFile.Close();
		return true;
	}

};


#pragma endregion




// -------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack
typedef struct Node_Info_STD_Collision
{
	UINT NodeID;                        ///< 주행 Node Id
	UINT LeftNodeID;                    ///< (왼쪽) 연결된 Node Id
	BYTE LeftUBGRegion;                 ///< (왼쪽) 연결된 Point까지의 대차감지센서 영역
	BYTE LeftObstacleRegion;            ///< (왼쪽) 연결된 Point까지의 장애물감지센서 영역
	BYTE LeftObstacleStatus;            ///< (왼쪽) 장애물감지센서 사용 유무 (0: 미사용, 1: 사용)
										//	UINT Reserved1;
	BYTE L_Reserved1;
	BYTE L_Reserved2;
	BYTE L_Reserved3;
	BYTE L_Reserved4;
	UINT RightNodeID;                   ///< (오른쪽) 연결된 Node Id
	BYTE RightUBGRegion;                ///< (오른쪽) 연결된 Point까지의 대차감지센서 영역
	BYTE RightObstacleRegion;           ///< (오른쪽) 연결된 Point까지의 장애물감지센서 영역
	BYTE RightObstacleStatus;           ///< (오른쪽) 장애물감지센서 사용 유무 (0: 미사용, 1: 사용)
										//	UINT Reserved2;
	BYTE R_Reserved1;
	BYTE R_Reserved2;
	BYTE R_Reserved3;
	BYTE R_Reserved4;
	Node_Info_STD_Collision() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_COLLISION;
#pragma pack(pop) // Pack 해제



class STD_Collision
{
private:


public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_COLLISION *m_NodeArray; ///< Node 동적 배열 포인터

	//std::map<int, int> m_CollisionMap;
	void MakeCollisionMap_STD()
	{
		if ((m_NodeNum == 0) || (m_NodeArray == NULL))
			return;

		//for (int i = 0; i < m_NodeNum; i++)
		//{
		//	// Key는 Path ID, Value는 배열 Index
		//	m_CollisionMap[m_NodeArray[i].NodeID] = i;
		//}
	}

	STD_Collision()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;

		// CollisionMap 초기화
		//m_CollisionMap.clear();
	}
	~STD_Collision()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;

		//m_CollisionMap.clear();
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// 동적 생성하기 전 Null인지를 확인 후 생성
		if (m_NodeArray != NULL)
		{
			// NULL이 아닌 경우는 delete 후에 동적 생성
			// 맵 데이터 갱신시 사용
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_COLLISION[NodeNum];
		}
		else
		{
			//NodeNum Max값으로 테스트 필요 ??
			m_NodeArray = new NODE_INFO_STD_COLLISION[NodeNum];
		}

		//m_CollisionMap.clear();
	}

};

class CollisionManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag
	STD_Collision Mapdata;
public:
	FILE_HEADER_STD m_CollisionFileHeader; ///< Map File Header
										   //HEADER_INFO_STD_MAP

	CollisionManager_STD()
	{
		m_MapFileLoaded = false;
		memset(&m_CollisionFileHeader, 0x00, sizeof(FILE_HEADER_STD));
	}
	~CollisionManager_STD()
	{
		m_MapFileLoaded = false;
	}

	bool LoadMapFile(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&m_CollisionFileHeader, 0x00, sizeof(m_CollisionFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header 파싱  --> 현재 파싱 필요없음.
			//m_CollisionFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&m_CollisionFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&m_CollisionFileHeader.ModelID);
			ConvertEndian(&m_CollisionFileHeader.Version);
			ConvertEndian(&m_CollisionFileHeader.DataCount);
			ConvertEndian(&m_CollisionFileHeader.DataType);

			if ((m_CollisionFileHeader.DataCount > 0) && (m_CollisionFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data 총 size
								  // Step 4. 파일에서 Load한 내용에 따라 Init
				Mapdata.InitMap(m_CollisionFileHeader.DataCount);

				// Step 5. Node Size만큼 Read
				NodeSize = m_CollisionFileHeader.DataCount * sizeof(NODE_INFO_STD_COLLISION);

				fs.read((char *)Mapdata.m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF 가 없으면 수정 전 맵으로 간주
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						//ADD_LOG("[CRC16_ERR] Collision File: cCRC16_header: 0x%x, cCRC16_file: 0x%x", cCRC16_header, cCRC16_file);
						//Error = ERR_MAP_FILEOPENFAIL;
						return false;
					}
				}
				else
				{
					//ADD_LOG("[EOF_ERR] Collision File");
					//맵 구조 수정 후 주석 부분 적용
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < Mapdata.m_NodeNum; i++)
				{
					ConvertEndian(&Mapdata.m_NodeArray[i].NodeID);
					ConvertEndian(&Mapdata.m_NodeArray[i].LeftNodeID);
					ConvertEndian(&Mapdata.m_NodeArray[i].RightNodeID);
					i = i; //debug break point 용도 추후 삭제  
				}
				m_MapFileLoaded = true; // Map File Loaded flag 변경
			}
		}
		else
		{
			return false;
			//Error = ERR_MAP_FILEOPENFAIL;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString FileName)
	{
		CStdioFile layoutFile;
		if (layoutFile.Open(FileName, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		// cid data 기록
		CString strLine;
		for (int i = 0; i <Mapdata.m_NodeNum; i++)
		{
			Node_Info_STD_Collision *p = &Mapdata.m_NodeArray[i];

			strLine.Format("%d/%d/%d/%d/%d/%d/%d/%d/%d\n",
				p->NodeID, p->LeftNodeID, p->LeftUBGRegion, p->LeftObstacleRegion, p->LeftObstacleStatus
				, p->RightNodeID, p->RightUBGRegion, p->RightObstacleRegion, p->RightObstacleStatus);
				

			layoutFile.WriteString(strLine);
		}
		layoutFile.Close();
		return true;
	}

};


// -------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack

typedef struct DEFAULT_MAPINFO_
{
	BYTE LOW_DefaultMapInfo : 4;    ///< 하위 비트(Left, Staion, Teaching 정보)
	BYTE HIGH_DefaultMapInfo : 4;    ///< 상위 비트(Right 정보)

	DEFAULT_MAPINFO_() { ZeroMemory(this, sizeof(*this)); }
} DEFAULT_MAPINFO;


typedef struct Node_Info_STD_Station
{
	UINT StationID;                     ///< Station Id
	BYTE StationType;                   ///< Station Type
	UINT ParentNodeID;                  ///< Station이 속해 있는 Link의 from Node Id
	UINT NextNodeID;                    ///< Station이 속해 있는 Link의 to Node Id
	WORD Offset;                        ///< Parent Node로부터의 거리(mm)
	BYTE Safety_Level;                  ///< 정지노드내 이적재 반납 유무 판단 플래그
	BYTE Reserved1;         			///< Reserved 1
	BYTE Reserved2;         			///< Reserved 2
	BYTE Reserved3;         			///< Reserved 3
	BYTE Reserved4;                     ///< Reserved 4
	BYTE Reserved5;                     ///< Reserved 5
	BYTE Reserved6;                     ///< Reserved 6
	DEFAULT_MAPINFO IsDefaultMap;       ///< Low사용할것.

	Node_Info_STD_Station() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_STATION;
//stuct size : 23Byte

#pragma pack(pop) // Pack 해제

class STD_Station
{
private:


public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_STATION *m_NodeArray; ///< Node 동적 배열 포인터

	//std::map<int, int> m_StationMap;

	STD_Station()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;

		// StationMap 초기화
		//m_StationMap.clear();
	}
	~STD_Station()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;
		//m_StationMap.clear();
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// 동적 생성하기 전 Null인지를 확인 후 생성
		if (m_NodeArray != NULL)
		{
			// NULL이 아닌 경우는 delete 후에 동적 생성
			// 맵 데이터 갱신시 사용
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_STATION[NodeNum];
		}
		else
		{
			//NodeNum Max값으로 테스트 필요 ??
			m_NodeArray = new NODE_INFO_STD_STATION[NodeNum];
		}

		//m_StationMap.clear();
	}

};

class StationManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_StationFileHeader; ///< Map File Header
										 //HEADER_INFO_STD_MAP
	STD_Station Mapdata;

	StationManager_STD()
	{
		m_MapFileLoaded = false;

		memset(&m_StationFileHeader, 0x00, sizeof(FILE_HEADER_STD));
	}
	~StationManager_STD()
	{
		m_MapFileLoaded = false;
	}

	bool LoadMapFile(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

		//int Error = NO_ERR;
		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&m_StationFileHeader, 0x00, sizeof(m_StationFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header 파싱  --> 현재 파싱 필요없음.
			//m_StationFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&m_StationFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&m_StationFileHeader.ModelID);
			ConvertEndian(&m_StationFileHeader.Version);
			ConvertEndian(&m_StationFileHeader.DataCount);
			ConvertEndian(&m_StationFileHeader.DataType);

			if ((m_StationFileHeader.DataCount > 0) && (m_StationFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data 총 size
								  // Step 4. 파일에서 Load한 내용에 따라 Init
				Mapdata.InitMap(m_StationFileHeader.DataCount);

				// Step 5. Node Size만큼 Read
				NodeSize = m_StationFileHeader.DataCount * sizeof(NODE_INFO_STD_STATION);

				fs.read((char *)Mapdata.m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF 가 없으면 수정 전 맵으로 간주
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						//ADD_LOG("[CRC16_ERR] Station File: cCRC16_header: 0x%x, cCRC16_file: 0x%x", cCRC16_header, cCRC16_file);
						//Error = ERR_MAP_FILEOPENFAIL;
						return false;
					}
				}
				else
				{
					//ADD_LOG("[EOF_ERR] Station File");
					//맵 구조 수정 후 주석 부분 적용
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < Mapdata.m_NodeNum; i++)
				{
					ConvertEndian(&Mapdata.m_NodeArray[i].StationID);
					ConvertEndian(&Mapdata.m_NodeArray[i].ParentNodeID);
					ConvertEndian(&Mapdata.m_NodeArray[i].NextNodeID);
					ConvertEndian(&Mapdata.m_NodeArray[i].Offset);
					i = i; //debug break point 용도 추후 삭제  
				}

				m_MapFileLoaded = true; // Map File Loaded flag 변경
			}
		}
		else
		{
			return false;
			//Error = ERR_MAP_FILEOPENFAIL;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString FileName)
	{
		CStdioFile layoutFile;
		if (layoutFile.Open(FileName, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		// cid data 기록
		CString strLine;
		for (int i = 0; i <Mapdata.m_NodeNum; i++)
		{
			NODE_INFO_STD_STATION *p = &Mapdata.m_NodeArray[i];
			strLine.Format("%d/%d/%d/%d/%d/%d\n", p->StationID, p->StationType, p->ParentNodeID, p->NextNodeID, p->Offset, p->Safety_Level);
			layoutFile.WriteString(strLine);
		}
		layoutFile.Close();
		return true;
	
	}
	//int SaveMapFile(TCHAR *FileName, STD_Station *pMapdata);
	FILE_HEADER_STD* GetFileHeader(){ return &m_StationFileHeader; }
	void SetFileHeader(FILE_HEADER_STD *pHeader){ memcpy(&m_StationFileHeader, pHeader, sizeof(FILE_HEADER_STD)); }

};


#pragma pack(push, 1) // 1byte로 Pack

typedef struct Node_Info_STD_FlowControl
{
	UINT StopNodeID;                    ///< Stop Node Id
	UINT ResetNodeMain;                 ///< Reset Node ID(Main) - 합류인 경우 Main에만 입력"
	BYTE BZStopMain;                    ///< BZ Stop 설정
	int ResetNodeMainDefaultValue;     ///< Reset Node ID(Main)에 대한 통과 Default 값
	UINT ResetNodeMainDefaultRatio;     ///< Reset Node ID(Main)에 대한 통과 Default 값 * (0 ~ 200%)
	UINT ResetNodeSub;                  ///< Reset Node ID(Sub.) - 분기인 경우 Main과 Sub.에 모두 입력"
	BYTE BZStopSub;                     ///< BZ Stop 설정
	int ResetNodeSubDefaultValue;      ///< Reset Node ID(Sub.)에 대한 통과 Default 값
	UINT ResetNodeSubDefaultRatio;      ///< Reset Node ID(Sub.)에 대한 통과 Default 값 * (0 ~ 200%)
	int ResetNodeRedundance1;          ///< Reset Node ID(Redundance 1)
	BYTE BZStopRedundance1;             ///< BZ Stop 설정
	int ResetNodeRedundance1DefaultValue;        ///< Reset Node ID(Redundance 1)에 대한 통과 Default 값
	UINT ResetNodeRedundance1DefaultRatio;        ///< Reset Node ID(Redundance 1)에 대한 통과 Default 값 * (0 ~ 200%)
												  //UINT ResetNodeRedundance2;          ///< Reset Node ID(Redundance 2)
												  //BYTE BZStopRedundance2;             ///< BZ Stop 설정
												  //UINT ResetNodeRedundance2DefaultValue;        ///< Reset Node ID(Redundance 2)에 대한 통과 Default 값
												  //UINT ResetNodeRedundance2DefaultRatio;        ///< Reset Node ID(Redundance 2)에 대한 통과 Default 값 * (0 ~ 200%)
	UINT Reserved1;                     ///< Reserved_1
	UINT Reserved2;                     ///< Reserved_2
	UINT Reserved3;                     ///< Reserved_3
	UINT Reserved4;                     ///< Reserved_4

	Node_Info_STD_FlowControl() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_FLOWCONTROL;
//stuct size : 72Byte

#pragma pack(pop) // Pack 해제


class STD_FlowControl
{
private:


public:
	int m_NodeNum; ///< Node 개수
	NODE_INFO_STD_FLOWCONTROL *m_NodeArray; ///< Node 동적 배열 포인터
	
	STD_FlowControl() 
	{
		m_NodeNum = 0;
		m_NodeArray = NULL;
	}

	~STD_FlowControl()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// 동적 생성하기 전 Null인지를 확인 후 생성
		if (m_NodeArray != NULL)
		{
			// NULL이 아닌 경우는 delete 후에 동적 생성
			// 맵 데이터 갱신시 사용
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_FLOWCONTROL[NodeNum];
		}
		else
		{
			//NodeNum Max값으로 테스트 필요 ??
			m_NodeArray = new NODE_INFO_STD_FLOWCONTROL[NodeNum];
		}
	}

	int LoadMapFile(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

		FILE_HEADER_STD FlowControlFileHeader; ///< Map File Header

		STD_FlowControl *pMapdata = this;
		
		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&FlowControlFileHeader, 0x00, sizeof(FlowControlFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header 파싱  --> 현재 파싱 필요없음.
			//m_FlowControlFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&FlowControlFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&FlowControlFileHeader.ModelID);
			ConvertEndian(&FlowControlFileHeader.Version);
			ConvertEndian(&FlowControlFileHeader.DataCount);
			ConvertEndian(&FlowControlFileHeader.DataType);


			if ((FlowControlFileHeader.DataCount > 0) && (FlowControlFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data 총 size
								  // Step 4. 파일에서 Load한 내용에 따라 Init
				pMapdata->InitMap(FlowControlFileHeader.DataCount);

				// Step 5. Node Size만큼 Read
				NodeSize = FlowControlFileHeader.DataCount * sizeof(NODE_INFO_STD_FLOWCONTROL);

				fs.read((char *)pMapdata->m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF 가 없으면 수정 전 맵으로 간주
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						return false;
					}
				}
				else
				{
					return false;
					//ADD_LOG("[EOF_ERR] FlowControl File");
					//맵 구조 수정 후 주석 부분 적용
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < pMapdata->m_NodeNum; i++)
				{
					ConvertEndian(&pMapdata->m_NodeArray[i].StopNodeID);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeMain);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeMainDefaultValue);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeMainDefaultRatio);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeSub);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeSubDefaultValue);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeSubDefaultRatio);
					ConvertEndian(&pMapdata->m_NodeArray[i].Reserved1);
					ConvertEndian(&pMapdata->m_NodeArray[i].Reserved2);
					ConvertEndian(&pMapdata->m_NodeArray[i].Reserved3);
					ConvertEndian(&pMapdata->m_NodeArray[i].Reserved4);
					i = i; //debug break point 용도 추후 삭제  
				}
				//m_MapFileLoaded = true; // Map File Loaded flag 변경
			}
		}
		else
		{
			//Error = ERR_MAP_FILEOPENFAIL;
			return false;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString FileName)
	{
		CStdioFile file;
		if (file.Open(FileName, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		CString strLine;
		for (int i = 0; i <m_NodeNum; i++)
		{
			NODE_INFO_STD_FLOWCONTROL *p = &m_NodeArray[i];
			strLine.Format("%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d\n", p->StopNodeID, p->ResetNodeMain, p->ResetNodeMainDefaultValue, p->ResetNodeMainDefaultRatio, p->ResetNodeSub, p->ResetNodeSubDefaultValue
				, p->ResetNodeSubDefaultRatio
				, p->ResetNodeRedundance1
				, p->BZStopRedundance1
				, p->ResetNodeRedundance1DefaultValue
				, p->ResetNodeRedundance1DefaultRatio);

			file.WriteString(strLine);
		}
		file.Close();
		return true;

	}
};


// -------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack


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
	int STBOffSet;                     ///< STB Off Set 이동거리(1/10mm, 기본값 0)
	BYTE LookDown;                      ///< Station 이적재 시 Lookdown Area 정보(0~16, 기본값 1)
	UINT Reserved1;                     ///< Reserved 1
	BYTE Reserved2;                     ///< Reserved 2
	BYTE Reserved3;                     ///< Reserved 4
	char STBL_Shift;                     ///< STBL_Shift 티칭 값에 3mm추가 soming
	DEFAULT_MAPINFO IsDefaultMap;       ///< Low사용할것.

	Node_Info_STD_Teaching() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_TEACHING;
//stuct size : 34Byte

#pragma pack(pop) // Pack 해제


class STD_Teaching
{
public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_TEACHING *m_NodeArray; ///< Node 동적 배열 포인터

	STD_Teaching()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;
	}
	~STD_Teaching()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// 동적 생성하기 전 Null인지를 확인 후 생성
		if (m_NodeArray != NULL)
		{
			// NULL이 아닌 경우는 delete 후에 동적 생성
			// 맵 데이터 갱신시 사용
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_TEACHING[NodeNum];
		}
		else
		{
			//NodeNum Max값으로 테스트 필요 ??
			m_NodeArray = new NODE_INFO_STD_TEACHING[NodeNum];
		}
	}

	int LoadMapFile(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

		FILE_HEADER_STD m_TeachingFileHeader; ///< Map File Header
		STD_Teaching *pMapdata = this;
		
		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&m_TeachingFileHeader, 0x00, sizeof(m_TeachingFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header 파싱  --> 현재 파싱 필요없음.
			//m_TeachingFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&m_TeachingFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&m_TeachingFileHeader.ModelID);
			ConvertEndian(&m_TeachingFileHeader.Version);
			ConvertEndian(&m_TeachingFileHeader.DataCount);
			ConvertEndian(&m_TeachingFileHeader.DataType);

			if ((m_TeachingFileHeader.DataCount > 0) && (m_TeachingFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data 총 size
								  // Step 4. 파일에서 Load한 내용에 따라 Init
				pMapdata->InitMap(m_TeachingFileHeader.DataCount);

				// Step 5. Node Size만큼 Read
				NodeSize = m_TeachingFileHeader.DataCount * sizeof(NODE_INFO_STD_TEACHING);

				fs.read((char *)pMapdata->m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF 가 없으면 수정 전 맵으로 간주
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						return false;
					}
				}
				else
				{
					return false;
				}

				for (int i = 0; i < pMapdata->m_NodeNum; i++)
				{
					ConvertEndian(&pMapdata->m_NodeArray[i].StationID);
					ConvertEndian(&pMapdata->m_NodeArray[i].PIOID);
					ConvertEndian(&pMapdata->m_NodeArray[i].Hoist);
					ConvertEndian(&pMapdata->m_NodeArray[i].Shift);
					ConvertEndian(&pMapdata->m_NodeArray[i].Rotate);
					ConvertEndian(&pMapdata->m_NodeArray[i].PIOChannel);
					ConvertEndian(&pMapdata->m_NodeArray[i].STBOffSet);

					//i = i; //debug break point 용도 추후 삭제  
				}
				
			}
		}
		else
		{
			return false;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString FileName)
	{
		CStdioFile file;
		if (file.Open(FileName, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		CString strLine;
		for (int i = 0; i <m_NodeNum; i++)
		{
			NODE_INFO_STD_TEACHING *p = &m_NodeArray[i];
			strLine.Format("%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d\n"
				, p->StationID
				, p->PortType
				, p->Hoist
				, p->Shift
				, p->Rotate
				, p->PIOType
				, p->PIODirection
				, p->PIOID
				, p->PIOChannel
				, p->PIOTimeLevel
				, p->AutoRecovery
				, p->HandDetectEQ
				, p->STBOffSet
				, p->LookDown
				, p->STBL_Shift
			);
		
			file.WriteString(strLine);
		}
		file.Close();
		return true;

	}
};
//---------------------------------------------------------------------------
#pragma pack(push, 1) // 1byte로 Pack
typedef struct Node_Info_STD_PathData
{
	BYTE LayoutType;                    ///< Layout Type(1: MTL, 2: Lifter)
	UINT LayoutID;                      ///< Layout ID
	UINT LeftNodeIDIn;                  ///< MTL(왼쪽), Lifter(아래) 진입 Node Id
	UINT LeftNodeIDMiddle;              ///< MTL(왼쪽), Lifter(아래) 중간 Node Id
	UINT LeftNodeIDOut;                 ///< MTL(왼쪽) 진출 Node Id
	BYTE LeftDriveDirection;            ///< 차량 진행 방향을 표시(0: 전진, 1: 후진)
	UINT RightNodeIDIn;                 ///< MTL(오른쪽), Lifter(위) 진입 Node Id
	UINT RightNodeIDMiddle;             ///< MTL(오른쪽), Lifter(위) 중간 Node Id
	UINT RightNodeIDOut;                ///< MTL(오른쪽) 진출 Node Id
	BYTE RightDriveDirection;           ///< 차량 진행 방향을 표시(0: 전진, 1: 후진)
	UINT Reserved1;                     ///< Reserved 1
	UINT Reserved2;                     ///< Reserved 2

	Node_Info_STD_PathData() { ZeroMemory(this, sizeof(*this)); }
	
} NODE_INFO_STD_PATHDATA;
#pragma pack(pop) // Pack 해제

class STD_PathData
{
public:
	int m_NodeNum; ///< Node 개수


	NODE_INFO_STD_PATHDATA *m_NodeArray; ///< Node 동적 배열 포인터

	STD_PathData()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;
	}
	~STD_PathData()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// 동적 생성하기 전 Null인지를 확인 후 생성
		if (m_NodeArray != NULL)
		{
			// NULL이 아닌 경우는 delete 후에 동적 생성
			// 맵 데이터 갱신시 사용
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_PATHDATA[NodeNum];
		}
		else
		{
			//NodeNum Max값으로 테스트 필요 ??
			m_NodeArray = new NODE_INFO_STD_PATHDATA[NodeNum];
		}
	}
	int LoadMapFile(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // 경로명에 한글이 있는 경우를 위해 추가

		FILE_HEADER_STD pathDataHeader; ///< Map File Header
		STD_PathData *pMapdata = this;

		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&pathDataHeader, 0x00, sizeof(pathDataHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header 파싱  --> 현재 파싱 필요없음.
			//m_CIDFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&pathDataHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&pathDataHeader.ModelID);
			ConvertEndian(&pathDataHeader.Version);
			ConvertEndian(&pathDataHeader.DataCount);
			ConvertEndian(&pathDataHeader.DataType);

			if ((pathDataHeader.DataCount > 0) && (pathDataHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data 총 size
								  // Step 4. 파일에서 Load한 내용에 따라 Init
				pMapdata->InitMap(pathDataHeader.DataCount);

				// Step 5. Node Size만큼 Read
				NodeSize = pathDataHeader.DataCount * sizeof(NODE_INFO_STD_PATHDATA);

				fs.read((char *)pMapdata->m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF 가 없으면 수정 전 맵으로 간주
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();					
						return false;
					}
				}
				else
				{
					//ADD_LOG("[EOF_ERR] Path Data File");
					//맵 구조 수정 후 주석 부분 적용
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < pMapdata->m_NodeNum; i++)
				{
					ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
					ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDIn);
					ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDMiddle);
					ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDOut);
					ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDIn);
					ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDMiddle);
					ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDOut);

					//i = i; //debug break point 용도 추후 삭제  
				}

			}
		}
		else
		{
			return false;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString FileName)
	{
		CStdioFile file;
		if (file.Open(FileName, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		CString strLine;
		for (int i = 0; i <m_NodeNum; i++)
		{
			// [PathData] 
			// LayoutType(1 or 2) / LayoutID / LeftNodeIDIn/ LeftNodeIDMiddle/ LeftNodeIDOut/ LeftDriveDir /
			// RightNodeIDIn/ RightNodeIDMiddle/ RightNodeIDOut/ RightDriveDir / Resered1 / Reserved2

			NODE_INFO_STD_PATHDATA *p = &m_NodeArray[i];
			strLine.Format("%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d\n"
				, p->LayoutType
				, p->LayoutID
				, p->LeftNodeIDIn
				, p->LeftNodeIDMiddle
				, p->LeftNodeIDOut
				, p->LeftDriveDirection
				, p->RightNodeIDIn
				, p->RightNodeIDMiddle
				, p->RightNodeIDOut
				, p->RightDriveDirection
				, p->Reserved1
				, p->Reserved2
			);
			file.WriteString(strLine);
		}
		file.Close();
		return true;

	}
};