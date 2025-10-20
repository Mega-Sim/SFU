/**
@file    OHT_TLV.h
$Rev:: 2173          $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2014-03-12 1#$:  Date of last commit
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

#include "../Common/PUtily.h"

// -------------------------------------------
// Common  Definition
// -------------------------------------------
#define CPLUS_BUILDER_XE ///< IDE Tool 종류
#define LITTLE_ENDIAN    ///< Little-Endian System인 경우 

#define LEN_TLV_BUFFER  8192 ///< TLV 최대 버퍼


// -------------------------------------------
// Common Error Code
// -------------------------------------------
#define ERR_TAG_NAME        -1000
#define ERR_TAG_CMDTYPE     -1001
#define ERR_TAG_PROTOTYPE   -1002
#define ERR_BODY_LENGTH  	-1010
#define ERR_ENCODE_LENGTH  	-1020
#define ERR_DECODE_LENGTH  	-1021
#define ERR_UNKNOWNDATALEN  -1030

#define ERR_PROTOCOL_DECODEERR -1030

// Command 처리 함수(parsing->Make CmdList)
CMD_HEADER GetHeader(char *pRecvBuf, bool IsConvertEndian = false);
CMD_HEADER MakeHeader(int Tag, int Length);


/**
@brief   Big-Endian을 Little-Endian으로 변환해주는 함수
@author  zzang9un
@date    2012.11.01
@param   objp : Convert할 Data의 주소
@tparam  Type : template parameter
*/
/*
template <class Type>
void ConvertEndian(Type *objp)
{
    unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
    std::reverse(memp, memp + sizeof(Type));
}
*/


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
		if(pHeader->uTag.Info.CmdData == TAG_CMDDATA_INIT_INFO_TP)
		{
			// by zzang9un 2013.03.22 : OHT_INFO 정보를 Decode한다.
			RtnBytes = DecodeTPCmd(pBuf, pHeader, (uTP_CMD *)pRecvData, ConvertEndian);
		}
        break;     
	case TAG_CMDTYPE_DATA:
		switch(pHeader->uTag.Info.CmdData)
		{
			case TAG_CMDDATA_DATA_IOMAP_TP :
				RtnBytes = DecodeIOMapField_OHT2TP(pBuf, pHeader, (IO_MAP_FIELD *)pRecvData, ConvertEndian);
				break;
			case TAG_CMDDATA_DATA_NOT_READY :
				break;
        }
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

// -------------------------------------------
// Assistant Command Encode 함수(For Assistant)
// -------------------------------------------
int EncodeASTCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uTP_CMD *pSndData,
                       bool IsConvertEndian = false);

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
int DecodeIOMapField_OHT2TP(char *pBuf,
					   CMD_HEADER *pHeader,
					   IO_MAP_FIELD *pRcvData,
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
int DecodeOHTStatus_AST2TP(char *pBuf,
						CMD_HEADER *pHeader,
						OHT_ADDITIONAL_INFO *pRcvData,
						bool IsConvertEndian /* = flase */);
                       
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

#endif
