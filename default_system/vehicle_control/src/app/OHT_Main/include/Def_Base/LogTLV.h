//---------------------------------------------------------------------------

#ifndef LogTLVH
#define LogTLVH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
//#include <ScktComp.hpp>
#include <iostream>
#include <algorithm>

#include "../Common/Def_DataType.h"

#define LEN_LOGTLV_BUFFER  8192 ///< TLV 최대 버퍼

#define MAX_WAIT        60 ///< Socket Read, Write 최대 대기 횟수


using namespace std;

#pragma pack(push, 1)
typedef enum LOG_DATA_TAG_
{
	LOG_CLEAR	= 0,
	LOG_WRITE	= 1
}LOG_DATA_TAG;
/**
@brief   Log Header
@author  박태욱
@date
*/
typedef struct LOG_HEADER_
{
	LOG_DATA_TAG	Tag;    ///<Tag : Clear/Write
	UINT		nLength;    ///<길이가 1000단위를 넘어가므로 UINT(4바이트로 사용)
} LOG_HEADER;
/**
@brief   Log Packet
@author  박태욱
@date
*/
typedef struct LOG_DATA_PACKET_
{
	char	pPath[256];    ///<Path 정보
	char	pValue[1024];  ///<Log Data 정보
}LOG_DATA_PACKET;
#pragma pack(pop) // Pack 해제

/**
@brief   GetLogHeader 함수
@author  박태욱
@date    2013.07.30
@note    소켓을 통해 받은 데이터를 통해 Header를 추출하기 위한 함수.
*/
LOG_HEADER GetLogHeader(char *pRecvBuf)
{
	LOG_HEADER RtnHeader; // Return할 Header 구조체

	memcpy(&RtnHeader, pRecvBuf, LEN_HEADER);

	return RtnHeader;
}

/**
@brief   MakeLogHeader 함수
@author  박태욱
@date    2013.07.30
@note   Tag와 Length를 통해 Header를 만들기 위한 함수.
*/
LOG_HEADER MakeLogHeader(int Tag, int Length)
{
	LOG_HEADER RtnHeader;

	RtnHeader.Tag = Tag;
	RtnHeader.nLength = Length;

	return RtnHeader;
}

//-------------------------------------------------------------------------
//  Common Socket I/O
//-------------------------------------------------------------------------
/**
@brief   Socket의 버퍼를 Read하는 함수
@author  puting
@date    2013.07.30
@param   pSocket : Read할 Socket
@param   pTLV : 수신한 내용을 저장할 버퍼(tag와 length를 포함한 버퍼)
@param   Bytes : 수신한 전체 bytes 값
@return  Read 성공 시 True 리턴, 실패 시 에러 코드 리턴
@note    Length 추출 시 Endian 변환 여부 설정에 주의 해야 함
*/
//int ReadLogSocket(TCustomWinSocket *pSocket,
//			   char *pTLV,
//			   int *Bytes)
//			  // bool IsConvertEndian /* = flase */)
//{
//	int WaitCount; // 대기 횟수
//    int RecvBytes; // 수신한 Bytes
//    int BodyLen; // Body Length
//
//    // Step 1. 수신 버퍼 size 확인
//    WaitCount = 0;
//    while(1)
//    {
//		RecvBytes = pSocket->ReceiveLength();
//
//        if(RecvBytes >= LEN_HEADER)
//            break;
//
//        // 수신한 size가 Header보다 작은 경우 대기
//        if(++WaitCount > MAX_WAIT)
//            break;
//
//        Sleep(WAITTIME);
//    }
//
//    // 에러 처리
//    if(RecvBytes < LEN_HEADER)
//    {
//        ///@todo 에러 처리 필요
//        return (FALSE);
//    }
//
//    // Step 2. Header Read
//    RecvBytes = pSocket->ReceiveBuf(pTLV, LEN_HEADER);
//
//    if (RecvBytes < LEN_HEADER)
//    {
//        ///@todo 에러 처리 필요
//		return (FALSE);
//	}
//
//    // Step 3. Body의 Length 추출
//	BodyLen = ((BYTE)pTLV[4]) + (BYTE)pTLV[5] * 256 + ((BYTE)pTLV[6]) * 256 * 256 + + ((BYTE)pTLV[7]) * 256 * 256 * 256;
//
//	if(BodyLen <0)
//	{
//	     return (FALSE);
//    }
//	// by zzang9un 2013.03.15 : Body에 Tail까지 추가로 읽어옴
//	int ReadByteLen = BodyLen + LEN_TAIL;
//
//	// Step 4. Length만큼 수신 버퍼에 data가 있는 지를 확인
//	WaitCount = 0;
//	while(1)
//	{
//		// by zzang9un 2013.03.15 : Tail까지 한꺼번에 읽어옴
//		RecvBytes = pSocket->ReceiveLength();
//
//        if(RecvBytes >= ReadByteLen)
//            break;
//
//        if(++WaitCount > MAX_WAIT)
//            break;
//
//		Sleep(WAITTIME);
//    }
//
//    // 에러 처리
//    if(RecvBytes < ReadByteLen)
//    {
//        ///@todo 에러 처리 필요
//        return (FALSE);
//    }
//
//    // Step 5. Body Read
//    // by zzang9un 2013.03.15 : Tail까지 한꺼번에 읽어옴
//	RecvBytes = pSocket->ReceiveBuf(pTLV + LEN_HEADER, ReadByteLen);
//
//    // 에러 처리
//    if(RecvBytes < ReadByteLen)
//    {
//        ///@todo 에러 처리 필요
//        return (FALSE);
//    }
//
//    // by zzang9un 2013.03.15 : Tail Lenth 추가
//	*Bytes = BodyLen + LEN_HEADER + LEN_TAIL;
//
//    return (TRUE);
//}
/**
@brief   OHT에서 Monitoring으로 보낼때 사용하는 함수
@author  puting
@date    2013.07.30
@param   pBuf : Send할 buffer
@param   SndTag : Send할 Tag값
@param   LOG_HEADER : 보낼 Header값
@param   LOG_PACKET : 보낼 구조체 정보
@return  성공 시 길이 리턴, 실패 시 -1 리턴
*/
int EncodeLogCmd(char *pBuf,
				 LOG_HEADER *pSndHeader,
				 LOG_DATA_PACKET *pSndData)
{
	int RtnBytes = 0; // Encode한 총 bytes

	LOG_HEADER SndHeader = *pSndHeader;
	LOG_DATA_PACKET SndPacket = *pSndData;

	UINT BodySize = sizeof(LOG_DATA_PACKET);

	// Step 0. Error 처리
	// Body의 Length가 틀린 경우 에러 처리
	if(SndHeader.nLength != BodySize)
		return -1;

	// -------------------------------------------
	// Step 1. Send Buf 초기화
	memset(pBuf, 0x00, LEN_LOGTLV_BUFFER);  //8192

	// -------------------------------------------
	// Step 2. Header Encode
	memcpy(pBuf, &SndHeader, LEN_HEADER);

	// -------------------------------------------
	// Step 3. Body Encode
	memcpy(pBuf + LEN_HEADER, &SndPacket, SndHeader.nLength);  //Path :1024, SYSTETIME : 16, HEADER :6

	// by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
	memcpy(pBuf + LEN_HEADER + BodySize, &SndHeader.nLength, LEN_TAIL);

	RtnBytes = LEN_HEADER + SndHeader.nLength + LEN_TAIL; // Encode한 총 bytes 리턴


	return RtnBytes;
}


/**
@brief   Monitring이 받은 데이터를 구분할때 사용.
@author  puting
@date    2013.07.30
@param   pBuf : Decode할 버퍼의 주소
@param   pHeader : 수신한 Command의 Header
@param   pRcvData : Decode할 Data(Template)
@return  정상인 경우 Decode한 총 bytes 수, 실패 시 에러코드 리턴
*/
int DecodeLogCmd(char *pBuf,
				 LOG_HEADER *pHeader,
				 LOG_DATA_PACKET *pRcvData)
{
	int RtnBytes = 0; // Decode한 총 bytes
	WORD BodySize = sizeof(LOG_DATA_PACKET);

	// -------------------------------------------
	// Step 0. Error 처리
	// Body의 Length가 틀린 경우 에러 처리
	if(pHeader->nLength != BodySize)
		return -1;

	// -------------------------------------------
	// Step 1. Body Decode
	int Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

	memcpy(pRcvData, pBuf + Offset, pHeader->nLength);
	// Step 2. Length 정보 입력
	RtnBytes = LEN_HEADER + pHeader->nLength;

	return RtnBytes;

}
/**
@brief   Monitoring 프로그램에서 파싱할때 사용될 함수.
@author  puting
@date    2013.07.30
@param   pRecvBuf : 받은 데이터를 이용하여 Decoding
@return  LOG_DATA_PACKET return
@note    OHT Assistant에만 사용되는 함수.
*/
int ParsingLogCmd(char *pRecvBuf, LOG_DATA_PACKET* LogData)
{
	int Error = 0;

	int DecodeSize = 0; // Decode한 전체 사이즈
	LOG_HEADER Header;  // Header를 parsing한 구조체

	memset(LogData, 0x00, sizeof(LOG_DATA_PACKET));

	// step1. Header parsing
	Header = GetLogHeader(pRecvBuf);

	// step2. Tag의 Type에 따라 Decode 함수 호출

	DecodeSize = DecodeLogCmd(pRecvBuf, &Header, LogData);

	if(DecodeSize ==-1)
	{

		return -1;
	}
	////////////////////////
//	실제 적용할 코드 작성.
   ///헤드와.LogData에 값이 들어있음.
///
	return Error;

}
//---------------------------------------------------------------------------
/**
@brief   OHT에서 Monitornig으로 보낼 데이터를 만든 함수.
@author  puting
@date    2013.07.30
@param   SndBuf : 보낼 Data를 버퍼에 넣기 위한 버퍼
@param   pLogHeader : LogHeader 정보
@param   pLogPacket : LogPacket 정보
@return  성공 시 길이 리턴, 실패 시 -1 리턴
@note    OHT에만 사용되는 함수.
*/
int MakeLogOHT2Monitor(char *SndBuf, LOG_HEADER *pLogHeader, LOG_DATA_PACKET *pLogPacket)
{
	int SndLen = 0;

	memset(SndBuf, 0x0,LEN_LOGTLV_BUFFER);

	SndLen = EncodeLogCmd(SndBuf, pLogHeader, pLogPacket);

	// 에러 처리
	if(SndLen < 0)
	{
      return -1;
	}

	return SndLen;
}

#endif
