/**
@file    OHT_TLV.h
@version OHT 7.0
@brief   OHT TLV 패킷을 Encode, Decode하는 함수를 정의한 Header 파일
*/

#ifndef OHT_TLVH
#define OHT_TLVH

#include <vcl.h>
#include <stdio.h>
#include <ScktComp.hpp>
#include <iostream>
#include <algorithm>
using namespace std;

#include "../Common/Def_Protocol.h"
#include "../Common/Def_DataType.h"
#include "ErrorCode.h"
#include "Def_DataFile.h"

// -------------------------------------------
// Common  Definition
// -------------------------------------------
// by zzang9un 2013.10.10 : 특정 Command 길이 설정
#define LEN_ACK_DATA_VERSIONINFO    (sizeof(DATA_VERSION_INFO) * 3)
#define LEN_ACK_DATAFILE_DOWNLOAD   5
#define LEN_ACK_DATAFILE_BACKUP     5
#define LEN_ACK_DATAFILE_UPLOAD     1

#define WAITTIME        10 ///< 버퍼 대기 시간 ms
#define MAX_WAIT        LEN_HEADER*WAITTIME ///< Socket Read, Write 최대 대기 횟수



// Command 처리 함수(parsing->Make CmdList)
CMD_HEADER GetHeader(char *pRecvBuf, bool IsConvertEndian = false);
CMD_HEADER MakeHeader(int Tag, int Length);
int GetBodyLength(char* pRecvBuf, bool IsConvertEndian = false);



/**
@brief	TLV데이터에 관한 정보를 저장하는 객체
@author	doori.shin
@date   2015.05.12
*/
class TLV
{
private:
	int m_nRecvLen;
public:
	bool bIsBigTLV;
	int m_TLVTotalLen;
	CMD_HEADER m_Header;  ///<  CmdHeader
	char *m_TLVBuffer;    ///< TLV 내용이 저장될 버퍼 포인터
	bool bIsMoreRcv;		// 다음 Read Event까지 받아야하는 경우

	TLV(int length, bool isBig)
	{
		m_nRecvLen = 0;
		m_TLVTotalLen = length;
		bIsBigTLV = isBig;
		m_TLVBuffer = new char[m_TLVTotalLen];
		bIsMoreRcv = false;

		memset(m_TLVBuffer, 0x00, sizeof(m_TLVTotalLen));
		memset(&m_Header, 0x00, sizeof(CMD_HEADER));
	}

	void InitTLV()
	{
		if(m_TLVBuffer != NULL)
		{
			delete[] m_TLVBuffer;
			m_TLVBuffer = NULL;
		}

		m_TLVTotalLen = 0;
		memset(&m_Header, 0x00, sizeof(CMD_HEADER));
		bIsBigTLV = false;
		m_nRecvLen = 0;
		bIsMoreRcv = false;
	}

	/**
	@brief	현재 버퍼에 data를 연결함
	@param 	data : 버퍼에 연결할 데이터
	@param	length : 저장할 데이터의 길이
	@return 버퍼에 저장된 데이터의 총 길이
	*/
	int AppendTLVData(char* data, const int& length)
	{
		memcpy(m_TLVBuffer + m_nRecvLen, data, length);
		m_nRecvLen += length;

		return m_nRecvLen;
	}

	/**
	@brief	더 받아야 할 Byte 길이를 측정
	*/
	int GetRemainingData()
	{
		return (this->m_TLVTotalLen - this->m_nRecvLen);
    }
};


// -------------------------------------------
// Template 함수
// -------------------------------------------
/**
@brief   Command 또는 Status를 Encode하는 Template 함수 
@author  zzang9un
@date    2013.03.13
@param   pBuf : Encode한 내용이 저장될 버퍼의 주소
@param   pHeader : 전송할 Data의 Header 구조체
@tparam  pSndData : Encode할 Data(Template)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  Encode한 전체 bytes, 실패 시 에러 코드 리턴
*/
template<typename Type>
int Encode(char *pBuf, int SndTag, Type *pSndData, bool ConvertEndian = false)
{
	int RtnBytes = 0; // Encode한 총 Bytes

	CMD_HEADER sSndHeader;

	sSndHeader = MakeHeader(SndTag, sizeof(Type));

    switch(sSndHeader.uTag.Info.CmdType)
    {
    case TAG_CMDTYPE_DRVING:
        RtnBytes = EncodeGoCmd(pBuf, &sSndHeader, (GO_CMD *)pSndData, ConvertEndian);
        break;

    case TAG_CMDTYPE_TRANS:
        RtnBytes = EncodeTransCmd(pBuf, &sSndHeader, (TRANS_CMD *)pSndData, ConvertEndian);
        break;

    case TAG_CMDTYPE_OCSCMD:
        RtnBytes = EncodeOCSCmd(pBuf, &sSndHeader, (uOCS_CMD *)pSndData, ConvertEndian);
        break;

	case TAG_CMDTYPE_TPCMD:
		RtnBytes = EncodeTPCmd(pBuf, &sSndHeader, (uTP_CMD *)pSndData, ConvertEndian);
        break;        

    case TAG_CMDTYPE_STATUS:
        if(sSndHeader.uTag.Info.Receiver == TAG_RECEIVER_OCS)
        {
            // OCS에게 보내는 Status인 경우
            RtnBytes = EncodeStatus_OHT2OCS(pBuf, &sSndHeader, (MSG_STATUS_OHT2OCS *)pSndData, ConvertEndian);
		}
		else if(sSndHeader.uTag.Info.Receiver == TAG_RECEIVER_TP)
		{
			// TP에게 보내는 Status인 경우
			RtnBytes = EncodeStatus_OHT2TP(pBuf, &sSndHeader, (MSG_STATUS_OHT2TP *)pSndData, ConvertEndian);
		}
		break;

	case TAG_CMDTYPE_INIT:
		if(sSndHeader.uTag.Info.CmdData == TAG_CMDDATA_INIT_INFO_TP)
		{
			// by zzang9un 2013.03.22 : TP로 OHT Info 정보를 Encode하는 함수
			// OHT_INFO는 uTP_CMD 구조체로 사용함
			RtnBytes = EncodeTPCmd(pBuf, &sSndHeader, (uTP_CMD *)pSndData, ConvertEndian);
		}
		break;

	case TAG_CMDTYPE_DATA:
		if(sSndHeader.uTag.Info.CmdData == TAG_CMDDATA_DATA_IOMAP_TP)
		{
			//TP에게 보내는 IO Map Field
			RtnBytes = EncodeIOMapField_OHT2TP(pBuf, &sSndHeader, (IO_MAP_FIELD *)pSndData, ConvertEndian);
		}
		else if(sSndHeader.uTag.Info.CmdData == TAG_CMDDATA_DATA_NOT_READY)
		{
			//TP에게 보내는 IO Map Field Not Ready
			RtnBytes = EncodeDataNotReady_OHT2TP(pBuf, &sSndHeader, (BYTE *)pSndData, ConvertEndian);
		}
		else if(sSndHeader.uTag.Info.CmdData == TAG_CMDDATA_DATA_OHT_STATUS)
		{
			RtnBytes = EncodeOHTStatus_AST2TP(pBuf, &sSndHeader, (OHT_ADDITIONAL_INFO *)pSndData, ConvertEndian);
        }
    default:
        ///@todo 에러 처리 필요
        break;
	}
    
    return RtnBytes;
}

/**
@brief   Command또는 Status를 Big-Endian으로 Decode하는 함수 
@author  zzang9un
@date    2012.11.05
@param   pBuf : Decode할 버퍼의 주소
@param   pHeader : Decode할 Data의 Header
@tparam  pRecvData : Decode한 결과가 저장되는 구조체 주소
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  Decode한 전체 bytes, 실패 시 에러 코드 리턴
*/
template<typename Type>
int Decode(char *pBuf, CMD_HEADER *pHeader, Type *pRecvData, bool ConvertEndian = false)
{
    int RtnBytes = 0; // Decode한 총 Bytes

	switch(pHeader->uTag.Info.CmdType)
    {
    case TAG_CMDTYPE_DRVING: // 주행 명령
        RtnBytes = DecodeGoCmd(pBuf, pHeader, (GO_CMD *)pRecvData, ConvertEndian);
        break;

    case TAG_CMDTYPE_DRVING_ETC: // 주행 기타 명령
        RtnBytes = DecodeGoEtcCmd(pBuf, pHeader, (uGO_ETC_CMD *)pRecvData, ConvertEndian);
        break;

    case TAG_CMDTYPE_TRANS: // 이적재 명령
        RtnBytes = DecodeTransCmd(pBuf, pHeader, (TRANS_CMD *)pRecvData, ConvertEndian);
        break;

    case TAG_CMDTYPE_OCSCMD: // OCS 일반 명령
        RtnBytes = DecodeOCSCmd(pBuf, pHeader, (uOCS_CMD *)pRecvData, ConvertEndian);
        break;

    case TAG_CMDTYPE_TPCMD: // TP 명령
        RtnBytes = DecodeTPCmd(pBuf, pHeader, (uTP_CMD *)pRecvData, ConvertEndian);
        break;

    case TAG_CMDTYPE_STATUS: // Status 요청 명령
        if(pHeader->uTag.Info.Receiver == TAG_RECEIVER_OCS)
        {
            RtnBytes = DecodeStatus_OHT2OCS(pBuf, pHeader, (MSG_STATUS_OHT2OCS *)pRecvData, ConvertEndian);
        }
        else if(pHeader->uTag.Info.Receiver == TAG_RECEIVER_TP)
        {
            RtnBytes = DecodeStatus_OHT2TP(pBuf, pHeader, (MSG_STATUS_OHT2TP *)pRecvData, ConvertEndian);
        }
        break; 

    case TAG_CMDTYPE_INIT: // 초기화 관련 명령
        if(pHeader->uTag.Info.Receiver == TAG_RECEIVER_TP)
        {
            // by zzang9un 2013.03.22 : OHT_INFO 정보를 Decode한다.
            RtnBytes = DecodeTPCmd(pBuf, pHeader, (uTP_CMD *)pRecvData, ConvertEndian);
        }
        break;     

    default:
        break;
    }    
    
    return RtnBytes;
}
//---------------------------------------------------------------------------

// -------------------------------------------
// Status 정보 Encode 함수
// -------------------------------------------
int EncodeStatus_OHT2OCS(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       MSG_STATUS_OHT2OCS *pSndData,
                       bool IsConvertEndian = false);
int EncodeStatus_OHT2TP(char *pBuf,
                       CMD_HEADER *pSndHeader,
					   MSG_STATUS_OHT2TP *pSndData,
					   bool IsConvertEndian = false);

int EncodeIOMapField_OHT2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
					   IO_MAP_FIELD *pSndData,
					   bool IsConvertEndian = false);
int EncodeDataNotReady_OHT2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
   					   BYTE *pSndData,
					   bool IsConvertEndian = false);

// -------------------------------------------
// 주행 및 이적재 Command Encode 함수(For OCSEmulator, TP)
// -------------------------------------------
int EncodeGoCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       GO_CMD *pSndData,
                       bool IsConvertEndian = false);
int EncodeGoEtcCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uGO_ETC_CMD *pSndData,
                       bool IsConvertEndian = false);
int EncodeTransCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       TRANS_CMD *pSndData,
                       bool IsConvertEndian = false);

// -------------------------------------------
// OCS Command Encode 함수(For OCSEmulator)
// -------------------------------------------
int EncodeOCSCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uOCS_CMD *pSndData,
                       bool IsConvertEndian = false);

// -------------------------------------------
// TP Command Encode 함수(For TP)
// -------------------------------------------
int EncodeTPCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uTP_CMD *pSndData,
                       bool IsConvertEndian = false);

// by zzang9un 2013.01.25 : ACK_DATA Encode 함수
int EncodeCmdACK(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       ACK_DATA *pSndData,
					   bool IsConvertEndian = false);

//2013.12.19 doori.shin OHT Additional status endcode
int EncodeOHTStatus_AST2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
					   OHT_ADDITIONAL_INFO *pSndData,
					   bool IsConvertEndian /* = flase */);

// -------------------------------------------
// Command Decode 함수 정의
// -------------------------------------------  
int DecodeStatus_OHT2OCS(char *pBuf,
                       CMD_HEADER *pHeader,
                       MSG_STATUS_OHT2OCS *pRcvData,
                       bool IsConvertEndian = false);
int DecodeStatus_OHT2TP(char *pBuf,
                       CMD_HEADER *pHeader,
                       MSG_STATUS_OHT2TP *pRcvData,
                       bool IsConvertEndian = false);
int DecodeGoCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       GO_CMD *pRcvData,
                       bool IsConvertEndian = false);
int DecodeGoEtcCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       uGO_ETC_CMD *pRcvData,
                       bool IsConvertEndian = false);
int DecodeTransCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       TRANS_CMD *pRcvData,
                       bool IsConvertEndian = false);
int DecodeOCSCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       uOCS_CMD *pRcvData,
                       bool IsConvertEndian = false);
int DecodeTPCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       uTP_CMD *pRcvData,
                       bool IsConvertEndian = false);
                       
/**
@brief   기본 자료형의 Data를 Encode하는 함수
@author  zzang9un
@date    2012.11.04
@param   pBuf : Encode할 버퍼의 주소
@param   pOffset : Encode할 버퍼의 offset
@tparam  pData : Encode할 Data(template)
@return  정상인 경우 Encode한 Bytes를 리턴, 비정상인 경우 에러 코드 리턴
*/
template <typename Type>
int EncodeBody(char *pBuf, int *pOffset, Type *pData)
{
    int RtnEncodeSize = sizeof(Type);

    if(RtnEncodeSize == 1)
    {
        // 1byte인 경우(char) Convert 필요 없음
        memcpy(pBuf + *pOffset, pData, RtnEncodeSize);
        *pOffset += RtnEncodeSize;
        return RtnEncodeSize;
    }
    else if(RtnEncodeSize >= 2)
    {
        // 2byte보다 큰 경우는 Covert 한 후 Encode
        ConvertEndian(pData);
        memcpy(pBuf + *pOffset, pData, RtnEncodeSize);
        *pOffset += RtnEncodeSize;
        return RtnEncodeSize;
    }
    else
    {
        // 에러 처리
        return ERR_UNKNOWNDATALEN;
    }    
}

/**
@brief   문자열을 Encode할 경우 사용하는 함수
@author  zzang9un
@date    2012.11.04
@param   pBuf : Encode할 버퍼의 주소
@param   pOffset : Encode할 버퍼의 offset
@param   pData : Encode할 Data(문자열)
@param   nSize : 문자열의 Size(bytes)
@note    문자열의 경우는 반드시 문자열의 크기를 함께 호출해야 한다.
*/
void EncodeBodyStr(char *pBuf, int *pOffset, char *pData, int Size)
{
    memcpy(pBuf + *pOffset, pData, Size);
    *pOffset += Size;
}

/**
@brief   기본 자료형의 Data를 Decode하는 함수
@author  zzang9un
@date    2012.11.05
@param   pBuf : Decode할 버퍼의 주소
@param   pOffset : Decode할 버퍼의 offset
@tparam  pData : Decode할 Data(template)
@return  정상인 경우 Decode한 Bytes를 리턴, 비정상인 경우 에러 코드 리턴
*/
template <typename Type>
int DecodeBody(char *pBuf, int *pOffset, Type *pData)
{
    int RtnDecodeSize = sizeof(Type);

    if(RtnDecodeSize == 1)
    {
        // 1byte인 경우(char) Convert 필요 없음
        memcpy(pData, pBuf + *pOffset, RtnDecodeSize);
        *pOffset += RtnDecodeSize;
        return RtnDecodeSize;
    }
    else if(RtnDecodeSize >= 2)
    {
        // 2byte보다 큰 경우는 Covert 한 후 Encode
        memcpy(pData, pBuf + *pOffset, RtnDecodeSize);
        ConvertEndian(pData);
        *pOffset += RtnDecodeSize;
        return RtnDecodeSize;
    }
    else
    {
        // 에러 처리
        return ERR_UNKNOWNDATALEN;
    }    
}

/**
@brief   문자열을 Decode할 경우 사용하는 함수
@author  zzang9un
@date    2012.11.04
@param   pBuf : Decode할 버퍼의 주소
@param   pOffset : Decode할 버퍼의 offset
@param   pData : Decode한 내용이 저장되는 문자열 주소
@param   nSize : 문자열의 Size(bytes)
@note    문자열의 경우는 반드시 문자열의 크기를 함께 호출해야 한다.
*/
void DecodeBodyStr(char *pBuf, int *pOffset, char *pData, int Size)
{
    memcpy(pData, pBuf + *pOffset, Size);
    *pOffset += Size;
}


// Socket I/O 함수
int ReadSocket(TCustomWinSocket *pSocket,
               char *pTLV,
               int *Bytes,
               bool IsConvertEndian = false);

/**
@brief   Buffer로부터 1개의 TLV를 얻기위한 함수
@author  puting
@date    2013.07.17
@param   pBuffer : 실제 Data가 있는 버퍼
@param   BuffLen : 실제 Data의 있는 버퍼 길이
@param   pTLV : 1개의 TLV가 저장될 버퍼
@param   Bytes : 1개의 TLV Size(bytes)
@note    문자열의 경우는 반드시 문자열의 크기를 함께 호출해야 한다.
*/
int ReadGetData(char *pBuffer,
				int BuffLen,
				char *pTLV,
				int *Bytes,
				bool IsConvertEndian = false);

#endif
