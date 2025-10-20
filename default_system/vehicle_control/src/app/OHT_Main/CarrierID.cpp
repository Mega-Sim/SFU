//---------------------------------------------------------------------------

#include "CarrierID.h"

//---------------------------------------------------------------------------


#define ADD_LOG(...)    WRITE_LOG_UNIT(pCarrierLog, __VA_ARGS__)

CarrierID::CarrierID()
{
	m_nCarrierIDReq = 0;
	memset(&ReqBuff, 0, sizeof(PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301));
	memset(CarrierIDBuf, 0, sizeof(char)*64);
	pCarrierLog = getLogUnit("D:\\log\\CarrierID.log", 1024*10);
	arrFOSB.clear();

	LoadCarrierID();
}
CarrierID::~CarrierID()
{

}

void CarrierID::GetCarrierIDReqBuffer(PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301 TempBuff)
{

	ADD_LOG("[GetCarrierIDReqBuffer]Temp StopStation: %d",TempBuff.StopStationID[2]);
	memset(&ReqBuff, 0, sizeof(PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301));
	memmove(&ReqBuff, &TempBuff, sizeof(PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301));
	ADD_LOG("[GetCarrierIDReqBuffer]Buff StopStation: %d",ReqBuff.StopStationID[2]);

}

void CarrierID::CarrierIDReqResponse(PKT_OCS2OHT* pRcvData)
{
	PROTOCOL_BODY_RESPONSE_CARRIERINFO_OCS_OHT_1302  CmdTempID1302;
	memset(&CmdTempID1302, 0x00, sizeof(PROTOCOL_BODY_RESPONSE_CARRIERINFO_OCS_OHT_1302));
	memcpy(&CmdTempID1302, &pRcvData->Value, sizeof(PROTOCOL_BODY_RESPONSE_CARRIERINFO_OCS_OHT_1302));
	int Response_CState = CmdTempID1302.Response;
	ADD_LOG("[CarrierIDReqResponse]Response_State: %d(1:Succeed, 2:Impossible, 3:Invalid)", Response_CState);
	ADD_LOG("[CarrierIDReqResponse]CarrierID: %s",CmdTempID1302.CarrierID);

	switch(Response_CState)
	{
		case CARRIERID_RESPONSE::SUCCEED:
			memmove(CarrierIDBuf, CmdTempID1302.CarrierID, sizeof(char)*64);
			ADD_LOG("[CarrierIDReqResponse]CARRIERID_RESPONSE:SUCCEED, %s",CarrierIDBuf);
			memset(&ReqBuff, 0, sizeof(PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301));
			m_nCarrierIDReq = 0;

			if(IsFOSB())
			{
				ADD_LOG("CarrierID: %s -> Fosb", CarrierIDBuf);
			}
			else
			{
            	ADD_LOG("CarrierID: %s -> Foup", CarrierIDBuf);
            }
			break;

		case CARRIERID_RESPONSE::IMPOSSIBLE:       //재요청
			ADD_LOG("[CarrierIDReqResponse]CARRIERID_RESPONSE:IMPOSSIBLE, %s",CarrierIDBuf);
			memset(CarrierIDBuf, 0, sizeof(char)*64);
			m_nCarrierIDReq = 1;
			break;

		case CARRIERID_RESPONSE::INVALID_RESPONSE:  //재요청
			ADD_LOG("[CarrierIDReqResponse]CARRIERID_RESPONSE:INVALID_RESPONSE, %s",CarrierIDBuf);
			memset(CarrierIDBuf, 0, sizeof(char)*64);
			m_nCarrierIDReq = 1;
            break;

	}

}

bool CarrierID::LoadCarrierID()
{
	bool bResult = false;
	bool bTmpValue = 0;

	LIniFile *pIniFile;
    pIniFile = NULL;
	pIniFile = new LIniFile("DataFiles\\CarrierID.ini");

    if(!pIniFile->GetIniFileExist())
	{
        delete pIniFile;
		pIniFile = NULL;
    }

	if(pIniFile != NULL)
	{
		std::vector<std::string> keys = pIniFile->GetKeysInSection("FOSB");
		while (!keys.empty())
		{
			// 해당 FOSB ID 값 확인하여, 사용한다면 목록에 추가
			std::string key = keys.back();
			pIniFile->ReadBool("FOSB", (char*)key.c_str(), false, &bTmpValue);
			if (bTmpValue)
				arrFOSB.push_back(key); // FOSB ID 추가
			keys.pop_back();
		}
		bResult = true;

		delete pIniFile;
	}

	return  bResult;
}

bool CarrierID::IsFOSB()
{
	std::string id(CarrierIDBuf);
	bool bFOSB = (std::find(
			arrFOSB.begin(), arrFOSB.end(), id
		) != arrFOSB.end()
	);

	ADD_LOG("[IsFOSB]: %s, %d(0:FOUP, 1:FOSB)", CarrierIDBuf, bFOSB);

	return  bFOSB;
}

void CarrierID::ClearCarrierID()
{
	ADD_LOG("ClearCarrierID!");
	memset(CarrierIDBuf, 0, sizeof(char)*64);
}
void CarrierID::ClearReqBuff()
{
	ADD_LOG("ClearReqBuffer!");
	memset(&ReqBuff, 0, sizeof(PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301));
}


