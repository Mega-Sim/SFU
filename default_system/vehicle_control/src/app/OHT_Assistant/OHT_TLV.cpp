/**
@file    OHT_TLV.cpp
@version OHT 7.0
@brief   OHT TLV 패킷을 Encode, Decode하는 함수를 정의한 CPP 파일
*/

#pragma hdrstop

#include "OHT_TLV.h"


// -------------------------------------------
// Client Socket I/O
// -------------------------------------------


//-------------------------------------------------------------------------
//  Common Socket I/O
//-------------------------------------------------------------------------

/**
@brief   Socket의 버퍼를 Read하는 함수
@author  zzang9un
@date    2012.10.30
@param   pSocket : Read할 Socket
@param   pTLV : 수신한 내용을 저장할 버퍼(tag와 length를 포함한 버퍼)
@param   Bytes : 수신한 전체 bytes 값
@param   IsConvertEndian : Length를 추출할 때 Endian 변환이 필요한 경우(default:false)
@return  Read 성공 시 True 리턴, 실패 시 에러 코드 리턴
@note    Length 추출 시 Endian 변환 여부 설정에 주의 해야 함
@todo    추후 리턴할 에러 코드를 추가해야 함.
*/
int ReadSocket(TCustomWinSocket *pSocket,
			   char *pTLV,
               int *Bytes,
               bool IsConvertEndian /* = flase */)
{
    int WaitCount; // 대기 횟수
    int RecvBytes; // 수신한 Bytes
    int BodyLen = 0; // Body Length

    // Step 1. 수신 버퍼 size 확인
    WaitCount = 0;
    while(1)
    {
        RecvBytes = pSocket->ReceiveLength();

        if(RecvBytes >= LEN_HEADER)
            break;

        // 수신한 size가 Header보다 작은 경우 대기
        if(++WaitCount > MAX_WAIT)
            break;

        Sleep(WAITTIME);
    }

    // 에러 처리
    if(RecvBytes < LEN_HEADER)
    {
        ///@todo 에러 처리 필요
        return (FALSE);
    }

    // Step 2. Header Read
    RecvBytes = pSocket->ReceiveBuf(pTLV, LEN_HEADER);

    if (RecvBytes < LEN_HEADER)
    {
        ///@todo 에러 처리 필요
        return (FALSE);
    }

    // Step 3. Body의 Length 추출
// -------------------------------------------
// [BEGIN] by zzang9un 2013.10.10 : Length의 Bytes 크기에 따라 코드가 변경됨
#if(LEN_LENGTH == 2)
    if(IsConvertEndian)
        BodyLen = ((BYTE)pTLV[4]) * 256 + (BYTE)pTLV[5];
    else
        BodyLen = ((BYTE)pTLV[4]) + (BYTE)pTLV[5] * 256;
#elif(LEN_LENGTH == 4)
    if(IsConvertEndian)
        BodyLen = (pTLV[4] << 24) | (pTLV[5] << 16) | (pTLV[6] << 8) | (BYTE)pTLV[7];
    else
        BodyLen = (BYTE)pTLV[4] | (pTLV[5] << 8) | (pTLV[6] << 16) | (pTLV[7] << 24);
#endif
// [END] by zzang9un 2013.10.10 : Length의 Bytes 크기에 따라 코드가 변경됨
// -------------------------------------------

    // by zzang9un 2013.03.15 : Body에 Tail까지 추가로 읽어옴
    int ReadByteLen = BodyLen + LEN_TAIL;

	// Step 4. Length만큼 수신 버퍼에 data가 있는 지를 확인
    WaitCount = 0;
    while(1)
    {
        // by zzang9un 2013.03.15 : Tail까지 한꺼번에 읽어옴
        RecvBytes = pSocket->ReceiveLength();

        if(RecvBytes >= ReadByteLen)
            break;

        if(++WaitCount > MAX_WAIT)
            break;

        Sleep(WAITTIME);
    }

    // 에러 처리
    if(RecvBytes < ReadByteLen)
    {
        ///@todo 에러 처리 필요
        return (FALSE);
    }

    // Step 5. Body Read
    // by zzang9un 2013.03.15 : Tail까지 한꺼번에 읽어옴
    RecvBytes = pSocket->ReceiveBuf(pTLV + LEN_HEADER, ReadByteLen);

    // 에러 처리
    if(RecvBytes < ReadByteLen)
    {
        ///@todo 에러 처리 필요
        return (FALSE);
    }

    // by zzang9un 2013.03.15 : Tail Lenth 추가
    *Bytes = BodyLen + LEN_HEADER + LEN_TAIL;

    return (TRUE);
}
//---------------------------------------------------------------------------

// -------------------------------------------
// Command Parsing Function Prototype
// -------------------------------------------
/**
@brief   수신한 버퍼의 Header를 얻어오는 함수
@author  zzang9un
@date    2012.11.05
@param   pRecvBuf : 수신한 버퍼(tag와 length를 포함한 버퍼)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  수신한 Data의 Header 구조체를 리턴
*/
CMD_HEADER GetHeader(char *pRecvBuf, bool IsConvertEndian /* = flase */)
{
    CMD_HEADER RtnHeader; // Return할 Header 구조체

    if(IsConvertEndian)
    {
        memcpy(&RtnHeader, pRecvBuf, LEN_HEADER);
        ConvertEndian(&RtnHeader.uTag.Data);
        ConvertEndian(&RtnHeader.Length);
    }
    else
    {
        memcpy(&RtnHeader, pRecvBuf, LEN_HEADER);
    }

    return RtnHeader;
}

/**
@brief   수신한 버퍼의 BodyLength를 얻어오는 함수
@author  doori.shin
@date    2015.03.11
@param   pRecvBuf : Length가 시작되는 위치의 버퍼
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  Byte로 된 BodyLength를 정수로 변환한 값
*/
int GetBodyLength(char* pRecvBuf, bool IsConvertEndian)
{
	int length = 0;

	if(IsConvertEndian == true)
	{
		length = ((BYTE)pRecvBuf[0] << 24)|((BYTE)pRecvBuf[1] << 16)
				|((BYTE)pRecvBuf[2] << 8)|(BYTE)pRecvBuf[3];
	}
	else
	{
		length = ((BYTE)pRecvBuf[0])|((BYTE)pRecvBuf[1] << 8)
				|((BYTE)pRecvBuf[2] << 16)|((BYTE)pRecvBuf[3] << 24);
    }

	return length;
}
//---------------------------------------------------------------------------

/**
@brief   Tag, Length를 이용하여 Command의 Header를 Make하는 함수
@author  zzang9un
@date    2012.11.05
@param   Tag : Make 하는 Command의 Tag
@param   Length : Make 하는 Command의 Length
@return  Make 완료한 Header 구조체를 리턴
*/
CMD_HEADER MakeHeader(int Tag, int Length)
{
    CMD_HEADER RtnHeader;

    RtnHeader.uTag.Data = Tag;
    RtnHeader.Length = Length;

    return RtnHeader;
}
//---------------------------------------------------------------------------

/**
@brief   Status(OHT->TP)를 전송하기 전 Encode하는 함수
@author  zzang9un
@date    2012.11.14
@param   pBuf : Encode할 버퍼의 주소
@param   pSndHeader : 전송할 Command의 Header
@param   pSndData : Encode할 Data(Template)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Encode한 총 bytes 수, 실패 시 에러코드 리턴
@note    TP에 전송하는 경우 Little-Endian으로 Encode한다.
@warning Body 맨 뒤에 Tail 정보(Body Length)를 반드시 포함한다.
*/

int EncodeIOMapField_OHT2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
					   IO_MAP_FIELD *pSndData,
					   bool IsConvertEndian /* = flase */)
{
	int RtnBytes = 0; // Encode한 총 bytes
	WORD BodySize = sizeof(int)*2 + (sizeof(BYTE)+sizeof(String))*pSndData->InputNum*2+sizeof(bool);

	pSndHeader->Length = BodySize;

	CMD_HEADER SndHeader = *pSndHeader;

	int Offset = 0; // Buffer copy를 위한 Offset

	// -------------------------------------------
	// Step 0. Error 처리
	if(SndHeader.uTag.Info.CmdData != TAG_CMDDATA_DATA_IOMAP_TP)
		return ERR_TAG_NAME;

	// Body의 Length가 틀린 경우 에러 처리
	if(SndHeader.Length != BodySize)
		return ERR_BODY_LENGTH;

	// -------------------------------------------
	// Step 1. Send Buf 초기화
	memset(pBuf, 0x00, LEN_TLV_BUFFER);

	// -------------------------------------------
	// Step 2. Header Encode
	if(IsConvertEndian)
	{
		// by zzang9un 2013.01.31 : OHT와 TP는 Endian을 Covert할 필요가 없음
		// 향후 필요한 경우를 위해 주석 처리만 함
	}
	else
	{
		memcpy(pBuf, &SndHeader, LEN_HEADER);
	}

	// -------------------------------------------
	// Step 4. Body Encode
	Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

	if(IsConvertEndian)
	{
		// by zzang9un 2013.01.31 : OHT와 TP는 Endian을 Covert할 필요가 없음
		// 향후 필요한 경우를 위해 주석 처리만 함
	}
	else
	{
		memcpy(pBuf + Offset, &pSndData->fIsLoaded, sizeof(pSndData->fIsLoaded));//bool fIsLoaded, 필요없는 변수
		Offset += sizeof(pSndData->fIsLoaded);
		memcpy(pBuf + Offset, &pSndData->InputNum, sizeof(pSndData->InputNum));
		Offset += sizeof(pSndData->InputNum);
		memcpy(pBuf + Offset, &pSndData->OutputNum, sizeof(pSndData->OutputNum));
		Offset += sizeof(pSndData->OutputNum);

		BYTE strLen = 0;
		for(int i=0; i<pSndData->InputNum; i++)
		{
			//문자열 길이를 먼저 넣음
			strLen = strlen(pSndData->InputField[i].c_str())+1;
			memcpy(pBuf + Offset, &strLen, sizeof(BYTE));
			Offset += sizeof(BYTE);

			//맨뒤에 널문자 추가
			pSndData->InputField[i] += "\0";

			//문자열을 넣음
			memcpy(pBuf + Offset, pSndData->InputField[i].c_str(), strLen);
			Offset += strLen;
		}

		for(int i=0; i<pSndData->OutputNum; i++)
		{
			//문자열 길이를 넣음
			strLen = strlen(pSndData->OutputField[i].c_str())+1;
			memcpy(pBuf + Offset, &strLen, sizeof(BYTE));
			Offset += sizeof(BYTE);

			//맨뒤에 널문자 추가
			pSndData->OutputField[i] += "\0";

			//문자열 넣음
			memcpy(pBuf + Offset, pSndData->OutputField[i].c_str(), strLen);
			Offset += strLen;
		}

		// 2013.08.26 doori.shin Body의 길이가 가변적이기때문에 마지막에 Length 와 tail 다시 엔코딩
		BodySize = Offset - LEN_HEADER;
		memcpy(pBuf + LEN_TAG, &BodySize, LEN_LENGTH);
		memcpy(pBuf + Offset + BodySize, &BodySize, LEN_TAIL);

		RtnBytes = Offset + LEN_TAIL; // Encode한 총 bytes 리턴
	}

	return RtnBytes;
}

/**
@brief   Status(OHT->TP)를 전송하기 전 Encode하는 함수
@author  zzang9un
@date    2012.11.14
@param   pBuf : Encode할 버퍼의 주소
@param   pSndHeader : 전송할 Command의 Header
@param   pSndData : Encode할 Data(Template)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Encode한 총 bytes 수, 실패 시 에러코드 리턴
@note    TP에 전송하는 경우 Little-Endian으로 Encode한다.
@warning Body 맨 뒤에 Tail 정보(Body Length)를 반드시 포함한다.
*/
int EncodeDataNotReady_OHT2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
					   BYTE *pSndData,
                       bool IsConvertEndian /* = flase */)
{
	int RtnBytes = 0; // Encode한 총 bytes
	WORD BodySize = sizeof(BYTE);

	CMD_HEADER SndHeader = *pSndHeader;

	int Offset = 0; // Buffer copy를 위한 Offset

    // -------------------------------------------
    // Step 0. Error 처리
	if(SndHeader.uTag.Info.CmdData != TAG_CMDDATA_DATA_NOT_READY)
        return ERR_TAG_NAME;

    // Body의 Length가 틀린 경우 에러 처리
    if(SndHeader.Length != BodySize)
		return ERR_BODY_LENGTH;

	// -------------------------------------------
    // Step 1. Send Buf 초기화
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
	// Step 2. Header Encode
    if(IsConvertEndian)
	{
        // by zzang9un 2013.01.31 : OHT와 TP는 Endian을 Covert할 필요가 없음
        // 향후 필요한 경우를 위해 주석 처리만 함
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
	}

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

    if(IsConvertEndian)
    {
        // by zzang9un 2013.01.31 : OHT와 TP는 Endian을 Covert할 필요가 없음
		// 향후 필요한 경우를 위해 주석 처리만 함
    }
    else
    {
		memcpy(pBuf + Offset, pSndData, sizeof(BYTE));

		// by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
		memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

		RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode한 총 bytes 리턴
    }

    return RtnBytes;
}

int EncodeOHTStatus_AST2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
					   OHT_ADDITIONAL_INFO *pSndData,
					   bool IsConvertEndian /* = flase */)
{
	int RtnBytes = 0; // Encode한 총 bytes
	WORD BodySize = sizeof(OHT_ADDITIONAL_INFO);

	pSndHeader->Length = BodySize;

	CMD_HEADER SndHeader = *pSndHeader;

	int Offset = 0; // Buffer copy를 위한 Offset

	// -------------------------------------------
	// Step 0. Error 처리
	if(SndHeader.uTag.Info.CmdData != TAG_CMDDATA_DATA_OHT_STATUS)
		return ERR_TAG_NAME;

	// Body의 Length가 틀린 경우 에러 처리
	if(SndHeader.Length != BodySize)
		return ERR_BODY_LENGTH;

	// -------------------------------------------
	// Step 1. Send Buf 초기화
	memset(pBuf, 0x00, LEN_TLV_BUFFER);

	// -------------------------------------------
	// Step 2. Header Encode
	if(IsConvertEndian)
	{
		// by zzang9un 2013.01.31 : OHT와 TP는 Endian을 Covert할 필요가 없음
		// 향후 필요한 경우를 위해 주석 처리만 함
	}
	else
	{
		memcpy(pBuf, &SndHeader, LEN_HEADER);
	}

	// -------------------------------------------
	// Step 4. Body Encode
	Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

	if(IsConvertEndian)
	{
		// by zzang9un 2013.01.31 : OHT와 TP는 Endian을 Covert할 필요가 없음
		// 향후 필요한 경우를 위해 주석 처리만 함
	}
	else
	{
		memcpy(pBuf + Offset, pSndData, sizeof(OHT_ADDITIONAL_INFO));
		Offset += sizeof(OHT_ADDITIONAL_INFO);

		//Add tail
//		memcpy(pBuf + Offset, &BodySize, LEN_TAIL);
		memcpy(pBuf + Offset, &SndHeader.Length, LEN_TAIL);

		RtnBytes = Offset + LEN_TAIL; // Encode한 총 bytes 리턴
	}

	return RtnBytes;
}

/**
@brief   Status(OHT->OCS)를 전송하기 전 Encode하는 함수
@author  zzang9un
@date    2013.03.13
@param   pBuf : Encode할 버퍼의 주소
@param   pSndHeader : 전송할 Command의 Header
@param   pSndData : Encode할 Data(Template)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Encode한 총 bytes 수, 실패 시 에러코드 리턴
@note    OCS에 전송하는 경우 Big-Endian으로 Encode한다.
@warning Body 맨 뒤에 Tail 정보(Body Length)를 반드시 포함한다.
*/
int EncodeStatus_OHT2OCS(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       MSG_STATUS_OHT2OCS *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode한 총 bytes
    WORD BodySize = sizeof(MSG_STATUS_OHT2OCS);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy를 위한 Offset

    // -------------------------------------------
    // Step 0. Error 처리
    if(SndHeader.uTag.Info.CmdData != TAG_CMDDATA_STATUS_OCS)
        return ERR_TAG_NAME;

    // Body의 Length가 틀린 경우 에러 처리
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf 초기화
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset
    MSG_STATUS_OHT2OCS pTmpStatus = *pSndData;

    if(IsConvertEndian)
    {
		// 구조체 정의된 순서에 주의하여 Encode해야 함.

		// by zzang9un 2013.05.14 : OHT 기본 정보
		// by zzang9un 2013.03.21 : Mode, Status, Command ID는 한번에 Encoding
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.MSCmdID.Data);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.NextCmdID);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.CurNodeID);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.CurNodeOffset);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.CurStationID);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.StopNodeID);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.StopStationID);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.StopStationOffset);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.ExistFoup);
        EncodeBody(pBuf, &Offset, &pTmpStatus.Data.CarrierType);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.PauseStatus);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.SteerPosition);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.ErrorCode);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.OriginInfo);

		// 추가 정보
		EncodeBodyStr(pBuf, &Offset, pTmpStatus.RFTagData, sizeof(pTmpStatus.RFTagData));
		EncodeBody(pBuf, &Offset, &pTmpStatus.APSignalLevel);
		EncodeBodyStr(pBuf, &Offset, pTmpStatus.APMacAddr, sizeof(pTmpStatus.APMacAddr));
		EncodeBody(pBuf, &Offset, &pTmpStatus.Motor_Drv_F_Position);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Motor_Drv_F_Speed);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Motor_Hoist_Position);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Motor_Shift_Position);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Motor_Rotate);

        // OHT 타입
        EncodeBody(pBuf, &Offset, &pTmpStatus.OHTType);

        // by zzang9un 2013.09.06 : Data 버전 정보
        EncodeBody(pBuf, &Offset, &pTmpStatus.MapData_CurRevision);
        EncodeBody(pBuf, &Offset, &pTmpStatus.StationData_CurRevision);
        EncodeBody(pBuf, &Offset, &pTmpStatus.TeachingData_CurRevision);
        
		// by zzang9un 2013.05.14 : HOHT와 구별되는 7.0 특별 Status
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_Erec);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_Edc);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_Idc);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_Ecap);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_Kw);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_ErrorCode);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_PickupTemper);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_CPUTemper);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Input[0]);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Input[1]);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Input[2]);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Output[0]);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Output[1]);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Output[2]);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode한 bytes와 body가 다른 경우 에러 처리
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode한 총 bytes 리턴
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode한 총 bytes 리턴
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Status(OHT->TP)를 전송하기 전 Encode하는 함수
@author  zzang9un
@date    2012.11.14
@param   pBuf : Encode할 버퍼의 주소
@param   pSndHeader : 전송할 Command의 Header
@param   pSndData : Encode할 Data(Template)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Encode한 총 bytes 수, 실패 시 에러코드 리턴
@note    TP에 전송하는 경우 Little-Endian으로 Encode한다.
@warning Body 맨 뒤에 Tail 정보(Body Length)를 반드시 포함한다.
*/
int EncodeStatus_OHT2TP(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       MSG_STATUS_OHT2TP *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode한 총 bytes
    WORD BodySize = sizeof(MSG_STATUS_OHT2TP);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy를 위한 Offset

    // -------------------------------------------
    // Step 0. Error 처리
    if(SndHeader.uTag.Info.CmdData != TAG_CMDDATA_STATUS_TP)
        return ERR_TAG_NAME;

    // Body의 Length가 틀린 경우 에러 처리
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf 초기화
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        // by zzang9un 2013.01.31 : OHT와 TP는 Endian을 Covert할 필요가 없음
        // 향후 필요한 경우를 위해 주석 처리만 함
    }
    else
    {
		memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

    if(IsConvertEndian)
    {
        // by zzang9un 2013.01.31 : OHT와 TP는 Endian을 Covert할 필요가 없음
        // 향후 필요한 경우를 위해 주석 처리만 함
    }
    else
	{
		memcpy(pBuf + Offset, pSndData, SndHeader.Length);

		// by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
		memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

		RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode한 총 bytes 리턴
	}

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Go Cmd를 전송하기 전 Encode하는 함수
@author  zzang9un
@date    2013.03.13
@param   pBuf : Encode할 버퍼의 주소
@param   pSndHeader : 전송할 Command의 Header
@param   pSndData : Encode할 Data(GO_CMD)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Encode한 총 bytes 수, 실패 시 에러코드 리턴
@note    OCS Emulator를 위해 만든 함수.
@warning Body 맨 뒤에 Tail 정보(Body Length)를 반드시 포함한다.
*/
int EncodeGoCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       GO_CMD *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode한 총 bytes
    WORD BodySize = sizeof(GO_CMD);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy를 위한 Offset

    // -------------------------------------------
    // Step 0. Error 처리
    // Command Type이 주행 type이 아닌 경우 에러 처리
    if(SndHeader.uTag.Info.CmdType != TAG_CMDTYPE_DRVING)
        return ERR_TAG_CMDTYPE;

    // Body의 Length가 틀린 경우 에러 처리
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf 초기화
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset
    GO_CMD pTmpData = *pSndData;

    if(IsConvertEndian)
    {
        // 구조체를 실제로 Encode하는 부분
        // 구조체 정의된 순서에 주의하여 Encode해야 함.
        EncodeBody(pBuf, &Offset, &pTmpData.CmdID);
        EncodeBody(pBuf, &Offset, &pTmpData.StopNodeID);
        EncodeBody(pBuf, &Offset, &pTmpData.StopStationID);
        EncodeBody(pBuf, &Offset, &pTmpData.StopStationOffset);
        EncodeBody(pBuf, &Offset, &pTmpData.StopStationType);
        EncodeBody(pBuf, &Offset, &pTmpData.NextNodeID);
        EncodeBody(pBuf, &Offset, &pTmpData.LineSpeedLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.CurveSpeedLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.AccelationLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.FinalPortType);
        EncodeBody(pBuf, &Offset, &pTmpData.TPosition_Shift);
        EncodeBody(pBuf, &Offset, &pTmpData.PreSteeringNodeID);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode한 bytes와 body가 다른 경우 에러 처리
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode한 총 bytes 리턴
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode한 총 bytes 리턴
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Go Cmd(Etc)를 전송하기 전 Encode하는 함수
@author  zzang9un
@date    2013.03.13
@param   pBuf : Encode할 버퍼의 주소
@param   pSndHeader : 전송할 Command의 Header
@param   pSndData : Encode할 Data(GO_ETC_CMD)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Encode한 총 bytes 수, 실패 시 에러코드 리턴
@note    OCS Emulator를 위해 만든 함수.
@warning Body 맨 뒤에 Tail 정보(Body Length)를 반드시 포함한다.
*/
int EncodeGoEtcCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uGO_ETC_CMD *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode한 총 bytes
    WORD BodySize = sizeof(uGO_ETC_CMD);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy를 위한 Offset

    // -------------------------------------------
    // Step 0. Error 처리
    // Command Type이 주행 type이 아닌 경우 에러 처리
    if(SndHeader.uTag.Info.CmdType != TAG_CMDTYPE_DRVING_ETC)
        return ERR_TAG_CMDTYPE;

    // Body의 Length가 틀린 경우 에러 처리
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf 초기화
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset
    uGO_ETC_CMD pTmpData = *pSndData;

    if(IsConvertEndian)
    {
        // 구조체를 실제로 Encode하는 부분
        // 구조체 정의된 순서에 주의하여 Encode해야 함.
        EncodeBody(pBuf, &Offset, &pTmpData.GoEtcCmd.CmdID);
        EncodeBody(pBuf, &Offset, &pTmpData.GoEtcCmd.Data1);
        EncodeBody(pBuf, &Offset, &pTmpData.GoEtcCmd.Data2);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode한 bytes와 body가 다른 경우 에러 처리
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode한 총 bytes 리턴
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode한 총 bytes 리턴
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Trans Cmd를 전송하기 전 Encode하는 함수
@author  zzang9un
@date    2013.03.13
@param   pBuf : Encode할 버퍼의 주소
@param   pSndHeader : 전송할 Command의 Header
@param   pSndData : Encode할 Data(TRANS_CMD)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Encode한 총 bytes 수, 실패 시 에러코드 리턴
@note    OCS Emulator를 위해 만든 함수.
@warning Body 맨 뒤에 Tail 정보(Body Length)를 반드시 포함한다.
*/
int EncodeTransCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       TRANS_CMD *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode한 총 bytes
    WORD BodySize = sizeof(TRANS_CMD);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy를 위한 Offset

    // -------------------------------------------
    // Step 0. Error 처리
    // Command Type이 주행 type이 아닌 경우 에러 처리
    if(SndHeader.uTag.Info.CmdType != TAG_CMDTYPE_TRANS)
        return ERR_TAG_CMDTYPE;

    // Body의 Length가 틀린 경우 에러 처리
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf 초기화
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset
    TRANS_CMD pTmpData = *pSndData;

    if(IsConvertEndian)
    {
        // 구조체를 실제로 Encode하는 부분
        // 구조체 정의된 순서에 주의하여 Encode해야 함.
        EncodeBody(pBuf, &Offset, &pTmpData.CmdID);
        EncodeBody(pBuf, &Offset, &pTmpData.CurNodeID);
        EncodeBody(pBuf, &Offset, &pTmpData.CurStationID);
        EncodeBody(pBuf, &Offset, &pTmpData.PortType);
        EncodeBody(pBuf, &Offset, &pTmpData.CarrierType);
        EncodeBody(pBuf, &Offset, &pTmpData.PIODirection);
        EncodeBody(pBuf, &Offset, &pTmpData.Tposition_Hoist);
        EncodeBody(pBuf, &Offset, &pTmpData.Tposition_Shift);
        EncodeBody(pBuf, &Offset, &pTmpData.Tposition_Rotate);
        EncodeBody(pBuf, &Offset, &pTmpData.HoistSpeedLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.ShiftSpeedLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.PIOTimeLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.LookDownLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.TransOption.Data);

        // by zzang9un 2013.03.15 : Tail 정보 추가 Encode

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode한 bytes와 body가 다른 경우 에러 처리
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode한 총 bytes 리턴
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode한 총 bytes 리턴
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Common Command(OCS->OHT)를 전송하기 전 Encode하는 함수
@author  zzang9un
@date    2012.11.19
@param   pBuf : Encode할 버퍼의 주소
@param   pSndHeader : 전송할 Command의 Header
@param   pSndData : Encode할 Data
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Encode한 총 bytes 수, 실패 시 에러코드 리턴
@note    OCS에 전송하는 경우 Big-Endian으로 Encode한다.
@warning Body 맨 뒤에 Tail 정보(Body Length)를 반드시 포함한다.
*/
int EncodeOCSCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uOCS_CMD *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode한 총 bytes
    WORD BodySize = sizeof(uOCS_CMD);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy를 위한 Offset

    // -------------------------------------------
    // Step 0. Error 처리
    // Body의 Length가 틀린 경우 에러 처리
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf 초기화
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 3. Body Encode
    Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset
    uOCS_CMD pTmpStatus = *pSndData;

    if(IsConvertEndian)
    {
        // 구조체 정의된 순서에 주의하여 Encode해야 함.
		EncodeBody(pBuf, &Offset, &pTmpStatus.OCSCmd.cData);
        EncodeBody(pBuf, &Offset, &pTmpStatus.OCSCmd.Data1);
        EncodeBody(pBuf, &Offset, &pTmpStatus.OCSCmd.Data2);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode한 bytes와 body가 다른 경우 에러 처리
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode한 총 bytes 리턴
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode한 총 bytes 리턴
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Common Command(OCS->OHT)를 전송하기 전 Encode하는 함수
@author  zzang9un
@date    2012.11.19
@param   pBuf : Encode할 버퍼의 주소
@param   pSndHeader : 전송할 Command의 Header
@param   pSndData : Encode할 Data
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Encode한 총 bytes 수, 실패 시 에러코드 리턴
@note    OCS에 전송하는 경우 Big-Endian으로 Encode한다.
@warning Body 맨 뒤에 Tail 정보(Body Length)를 반드시 포함한다.
*/
int EncodeTPCmd(char *pBuf,
					   CMD_HEADER *pSndHeader,
                       uTP_CMD *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode한 총 bytes
    WORD BodySize = sizeof(uTP_CMD);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy를 위한 Offset

    // -------------------------------------------
    // Step 0. Error 처리
    // Body의 Length가 틀린 경우 에러 처리
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf 초기화
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 3. Body Encode
    Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset
    uTP_CMD pTmpStatus = *pSndData;

    if(IsConvertEndian)
    {
        // 구조체 정의된 순서에 주의하여 Encode해야 함.
		EncodeBody(pBuf, &Offset, &pTmpStatus.TPCmd.CmdID);
        EncodeBody(pBuf, &Offset, &pTmpStatus.TPCmd.Data1);
        EncodeBody(pBuf, &Offset, &pTmpStatus.TPCmd.Data2);
        EncodeBody(pBuf, &Offset, &pTmpStatus.TPCmd.Data3);
        EncodeBody(pBuf, &Offset, &pTmpStatus.TPCmd.Data4);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode한 bytes와 body가 다른 경우 에러 처리
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode한 총 bytes 리턴
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode한 총 bytes 리턴
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   ACK_DATA를 전송하기 전 Encode하는 함수
@author  zzang9un
@date    2013.01.25
@param   pBuf : Encode할 버퍼의 주소
@param   pSndHeader : 전송할 Command의 Header
@param   pSndData : Encode할 Data
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Encode한 총 bytes 수, 실패 시 에러코드 리턴
@note    OCS에 전송하는 경우 Big-Endian으로 Encode한다.
@warning Body 맨 뒤에 Tail 정보(Body Length)를 반드시 포함한다.
*/
int EncodeCmdACK(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       ACK_DATA *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode한 총 bytes

    // by zzang9un 2013.01.25 : length는 6바이트
    UINT BodySize = 8; // Tag는 Body에 포함하지 않으므로 4를 뺀다.

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy를 위한 Offset

    // -------------------------------------------
    // Step 0. Error 처리
    // Protocol Type이 주행 ACK이 아닌 경우 에러 처리
    if(SndHeader.uTag.Info.ProtoType != TAG_PROTOTYPE_ACK)
        return ERR_TAG_PROTOTYPE;

    // Body의 Length가 틀린 경우 에러 처리
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf 초기화
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

	// -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset
    ACK_DATA pTmpStatus = *pSndData;

    if(IsConvertEndian)
    {
        // 구조체 정의된 순서에 주의하여 Encode해야 함.
        // by zzang9un 2013.09.27 : ACKCode를 ACKType에 전송함(Protocol에 상이한 부분이 있음)
        EncodeBody(pBuf, &Offset, &pTmpStatus.RecvCmdID);
        EncodeBody(pBuf, &Offset, &pTmpStatus.ACKResult);
        EncodeBody(pBuf, &Offset, &pTmpStatus.ACKErrorCode);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode한 bytes와 body가 다른 경우 에러 처리
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode한 총 bytes 리턴
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : 마지막으로 Tail 정보 추가
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode한 총 bytes 리턴
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   수신한 Status(OHT->OCS)를 Decode하는 함수

         OCS Emulator를 위한 함수
@author  zzang9un
@date    2012.11.05
@param   pBuf : Decode할 버퍼의 주소
@param   pHeader : 수신한 Command의 Header
@param   pRcvData : Decode할 Data(Template)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Decode한 총 bytes 수, 실패 시 에러코드 리턴
*/
int DecodeStatus_OHT2OCS(char *pBuf,
                       CMD_HEADER *pHeader,
                       MSG_STATUS_OHT2OCS *pRcvData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode한 총 bytes
    WORD BodySize = sizeof(MSG_STATUS_OHT2OCS);

    // -------------------------------------------
    // Step 0. Error 처리
    if(pHeader->uTag.Info.CmdData != TAG_CMDDATA_STATUS_OCS)
        return ERR_TAG_NAME;

    // Body의 Length가 틀린 경우 에러 처리
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

    if(IsConvertEndian)
    {
        // 구조체 정의된 순서에 주의하여 Decode해야 함.

        // by zzang9un 2013.05.14 : OHT 기본 정보
        // by zzang9un 2013.03.21 : Mode, Status, Command ID는 한번에 Decoding
        DecodeBody(pBuf, &Offset, &pRcvData->Data.MSCmdID.Data);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.NextCmdID);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.CurNodeID);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.CurNodeOffset);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.CurStationID);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.StopNodeID);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.StopStationID);
		DecodeBody(pBuf, &Offset, &pRcvData->Data.StopStationOffset);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.ExistFoup);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.CarrierType);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.PauseStatus);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.SteerPosition);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.ErrorCode);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.OriginInfo);

        // 추가 정보
        DecodeBodyStr(pBuf, &Offset, pRcvData->RFTagData, sizeof(pRcvData->RFTagData));
        DecodeBody(pBuf, &Offset, &pRcvData->APSignalLevel);
        DecodeBodyStr(pBuf, &Offset, pRcvData->APMacAddr, sizeof(pRcvData->APMacAddr));
        DecodeBody(pBuf, &Offset, &pRcvData->Motor_Drv_F_Position);
        DecodeBody(pBuf, &Offset, &pRcvData->Motor_Drv_F_Speed);
        DecodeBody(pBuf, &Offset, &pRcvData->Motor_Hoist_Position);
        DecodeBody(pBuf, &Offset, &pRcvData->Motor_Shift_Position);
        DecodeBody(pBuf, &Offset, &pRcvData->Motor_Rotate);

        // OHT 타입
        DecodeBody(pBuf, &Offset, &pRcvData->OHTType);

        // by zzang9un 2013.09.06 : Data 버전 정보
        DecodeBody(pBuf, &Offset, &pRcvData->MapData_CurRevision);
        DecodeBody(pBuf, &Offset, &pRcvData->StationData_CurRevision);
        DecodeBody(pBuf, &Offset, &pRcvData->TeachingData_CurRevision);
        
        // by zzang9un 2013.05.14 : HOHT와 구별되는 7.0 특별 Status
        DecodeBody(pBuf, &Offset, &pRcvData->HID_Erec);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_Edc);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_Idc);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_Ecap);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_Kw);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_ErrorCode);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_PickupTemper);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_CPUTemper);
        DecodeBody(pBuf, &Offset, &pRcvData->Input[0]);
        DecodeBody(pBuf, &Offset, &pRcvData->Input[1]);
        DecodeBody(pBuf, &Offset, &pRcvData->Input[2]);
        DecodeBody(pBuf, &Offset, &pRcvData->Output[0]);
        DecodeBody(pBuf, &Offset, &pRcvData->Output[1]);
        DecodeBody(pBuf, &Offset, &pRcvData->Output[2]);
        
		if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode한 bytes와 body가 다른 경우 에러 처리
            return ERR_DECODE_LENGTH;
        }

        RtnBytes = Offset;
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;

}
//---------------------------------------------------------------------------

/**
@brief   수신한 Status(OHT->TP)를 Decode하는 함수
@author  zzang9un
@date    2012.11.05
@param   pBuf : Decode할 버퍼의 주소
@param   pHeader : 수신한 Command의 Header
@param   pRcvData : Decode할 Data(Template)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Decode한 총 bytes 수, 실패 시 에러코드 리턴
*/
int DecodeStatus_OHT2TP(char *pBuf,
                       CMD_HEADER *pHeader,
                       MSG_STATUS_OHT2TP *pRcvData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode한 총 bytes
    WORD BodySize = sizeof(MSG_STATUS_OHT2TP);

    // -------------------------------------------
    // Step 0. Error 처리
    if(pHeader->uTag.Info.CmdData != TAG_CMDDATA_STATUS_TP)
        return ERR_TAG_NAME;

	// Body의 Length가 틀린 경우 에러 처리
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

    if(IsConvertEndian)
    {
        // by zzang9un 2013.01.31 : OHT와 TP는 Endian을 Covert할 필요가 없음
        // 향후 필요한 경우를 위해 주석 처리만 함
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;

}
//---------------------------------------------------------------------------

/**
@brief   주행 관련 Commmand를 Decode하는 함수

         다음 Command가 이 함수를 이용하여 Decode한다.
          - Go, Go More 명령
@author  zzang9un
@date    2012.11.14
@param   pBuf : Decode할 버퍼의 주소
@param   pHeader : 수신한 Command의 Header
@param   pRcvData : Decode할 Data(GO_CMD)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Decode한 총 bytes 수, 실패 시 에러코드 리턴
*/
int DecodeGoCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       GO_CMD *pRcvData,
                       bool IsConvertEndian /* = flase */)
{
	int RtnBytes = 0; // Decode한 총 bytes
    WORD BodySize = sizeof(GO_CMD);

    // -------------------------------------------
    // Step 0. Error 처리
    // Command Type이 주행 Type이 아닌 경우 에러 처리
    if(pHeader->uTag.Info.CmdType != TAG_CMDTYPE_DRVING)
        return ERR_TAG_CMDTYPE;

    // Body의 Length가 틀린 경우 에러 처리
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

    if(IsConvertEndian)
    {
        // 구조체 정의된 순서에 주의하여 Decode해야 함.
        DecodeBody(pBuf, &Offset, &pRcvData->CmdID);
        DecodeBody(pBuf, &Offset, &pRcvData->StopNodeID);
        DecodeBody(pBuf, &Offset, &pRcvData->StopStationID);
        DecodeBody(pBuf, &Offset, &pRcvData->StopStationOffset);
        DecodeBody(pBuf, &Offset, &pRcvData->StopStationType);
        DecodeBody(pBuf, &Offset, &pRcvData->NextNodeID);
        DecodeBody(pBuf, &Offset, &pRcvData->LineSpeedLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->CurveSpeedLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->AccelationLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->FinalPortType);
        DecodeBody(pBuf, &Offset, &pRcvData->TPosition_Shift);
        DecodeBody(pBuf, &Offset, &pRcvData->PreSteeringNodeID);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode한 bytes와 body가 다른 경우 에러 처리
            return ERR_DECODE_LENGTH;
        }

        RtnBytes = Offset;
    }
    else
	{
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   주행 기타 관련 Commmand를 Decode하는 함수

         다음 Command가 이 함수를 이용하여 Decode한다.
          - Go MTL, Map Make, Clean 명령
@author  zzang9un
@date    2013.04.12
@param   pBuf : Decode할 버퍼의 주소
@param   pHeader : 수신한 Command의 Header
@param   pRcvData : Decode할 Data(uGO_ETC_CMD)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Decode한 총 bytes 수, 실패 시 에러코드 리턴
*/
int DecodeGoEtcCmd(char * pBuf,
                       CMD_HEADER *pHeader,
                       uGO_ETC_CMD *pRcvData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode한 총 bytes
    WORD BodySize = sizeof(uGO_ETC_CMD);

    // -------------------------------------------
    // Step 0. Error 처리
    // Command Type이 주행 Type이 아닌 경우 에러 처리
    if(pHeader->uTag.Info.CmdType != TAG_CMDTYPE_DRVING_ETC)
        return ERR_TAG_CMDTYPE;

    // Body의 Length가 틀린 경우 에러 처리
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
	int Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

    if(IsConvertEndian)
    {
        // 구조체 정의된 순서에 주의하여 Decode해야 함.
        DecodeBody(pBuf, &Offset, &pRcvData->GoEtcCmd.CmdID);
        DecodeBody(pBuf, &Offset, &pRcvData->GoEtcCmd.Data1);
        DecodeBody(pBuf, &Offset, &pRcvData->GoEtcCmd.Data2);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode한 bytes와 body가 다른 경우 에러 처리
            return ERR_DECODE_LENGTH;
        }

        RtnBytes = Offset;
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   이적재 관련 Command를 Decode하는 함수

         다음 Command가 이 함수를 이용하여 Decode한다.
          - Load, Unload 명령
@author  zzang9un
@date    2012.11.12
@param   pBuf : Decode할 버퍼의 주소
@param   pHeader : 수신한 Command의 Header
@param   pRcvData : Decode할 Data(TRANS_CMD)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Decode한 총 bytes 수, 실패 시 에러코드 리턴
*/
int DecodeTransCmd(char *pBuf,
                        CMD_HEADER *pHeader,
						TRANS_CMD *pRcvData,
                        bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode한 총 bytes
    WORD BodySize = sizeof(TRANS_CMD);

    // -------------------------------------------
    // Step 0. Error 처리
    // Command Type이 이적재 Type이 아닌 경우 에러 처리
    if(pHeader->uTag.Info.CmdType != TAG_CMDTYPE_TRANS)
        return ERR_TAG_CMDTYPE;

    // Body의 Length가 틀린 경우 에러 처리
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

    if(IsConvertEndian)
    {
        // 구조체 정의된 순서에 주의하여 Decode해야 함.
        DecodeBody(pBuf, &Offset, &pRcvData->CmdID);
        DecodeBody(pBuf, &Offset, &pRcvData->CurNodeID);
        DecodeBody(pBuf, &Offset, &pRcvData->CurStationID);
        DecodeBody(pBuf, &Offset, &pRcvData->PortType);
        DecodeBody(pBuf, &Offset, &pRcvData->CarrierType);
        DecodeBody(pBuf, &Offset, &pRcvData->PIODirection);
        DecodeBody(pBuf, &Offset, &pRcvData->Tposition_Hoist);
        DecodeBody(pBuf, &Offset, &pRcvData->Tposition_Shift);
        DecodeBody(pBuf, &Offset, &pRcvData->Tposition_Rotate);
        DecodeBody(pBuf, &Offset, &pRcvData->HoistSpeedLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->ShiftSpeedLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->PIOTimeLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->LookDownLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->TransOption.Data);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode한 bytes와 body가 다른 경우 에러 처리
            return ERR_DECODE_LENGTH;
		}

        RtnBytes = Offset;
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   OCS 공통 Command를 Decode하는 함수

         다음 Command가 이 함수를 이용하여 Decode한다.
         E-Stop, Cancel, ID Reset, Pause, Resume 명령
@author  zzang9un
@date    2012.11.14
@param   pBuf : Decode할 버퍼의 주소
@param   pHeader : 수신한 Command의 Header
@param   pRcvData : Decode할 Data(uTP_CMD)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Decode한 총 bytes 수, 실패 시 에러코드 리턴
@note    Decode할 때 union 구조체인 CMD_COMMON을 사용한다.
*/
int DecodeOCSCmd(char *pBuf,
                        CMD_HEADER *pHeader,
                        uOCS_CMD *pRcvData,
                        bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode한 총 bytes
    WORD BodySize = sizeof(uOCS_CMD);

    // -------------------------------------------
    // Step 0. Error 처리
    // Body의 Length가 틀린 경우 에러 처리
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

	// -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

    if(IsConvertEndian)
    {
        // 구조체 정의된 순서에 주의하여 Decode해야 함.
        DecodeBody(pBuf, &Offset, &pRcvData->OCSCmd.cData);
        DecodeBody(pBuf, &Offset, &pRcvData->OCSCmd.Data1);
        DecodeBody(pBuf, &Offset, &pRcvData->OCSCmd.Data2);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode한 bytes와 body가 다른 경우 에러 처리
            return ERR_DECODE_LENGTH;
        }

        RtnBytes = Offset;
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   TP 공통 Command를 Decode하는 함수
@author  zzang9un
@date    2012.11.14
@param   pBuf : Decode할 버퍼의 주소
@param   pHeader : 수신한 Command의 Header
@param   pRcvData : Decode할 Data(uTP_CMD)
@param   IsConvertEndian : Endian 변환이 필요한 경우(default:false)
@return  정상인 경우 Decode한 총 bytes 수, 실패 시 에러코드 리턴
@note    Decode할 때 union 구조체인 CMD_COMMON을 사용한다.
*/
int DecodeTPCmd(char *pBuf,
                        CMD_HEADER *pHeader,
						uTP_CMD *pRcvData,
                        bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode한 총 bytes
    WORD BodySize = sizeof(uTP_CMD);

    // -------------------------------------------
    // Step 0. Error 처리
    // Body의 Length가 틀린 경우 에러 처리
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy하기 위한 buf의 Offset

    if(IsConvertEndian)
    {
        // 구조체 정의된 순서에 주의하여 Decode해야 함.
        DecodeBody(pBuf, &Offset, &pRcvData->TPCmd.CmdID);
        DecodeBody(pBuf, &Offset, &pRcvData->TPCmd.Data1);
        DecodeBody(pBuf, &Offset, &pRcvData->TPCmd.Data2);
        DecodeBody(pBuf, &Offset, &pRcvData->TPCmd.Data3);
        DecodeBody(pBuf, &Offset, &pRcvData->TPCmd.Data4);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode한 bytes와 body가 다른 경우 에러 처리
            return ERR_DECODE_LENGTH;
        }

        RtnBytes = Offset;
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;
}


//---------------------------------------------------------------------------


#pragma package(smart_init)
