#ifndef CarrierIDH
#define CarrierIDH


//#include "OHTMain.h"
#include "LIniFile.h"
#include "LogHandler.h"
#include "LogThread.h"

#include "../Common/Protocol_Define_Modify.h"
#include "../Common/Def_Protocol.h"

namespace CARRIERID_RESPONSE
{
    enum Reserved
	{
        SUCCEED = 1,
        IMPOSSIBLE,
        INVALID_RESPONSE
    };

};

class CarrierID
{

private:
    LogUnit *pCarrierLog;
    std::vector<std::string> arrFOSB;

public:

    CarrierID();
    ~CarrierID();

    int m_nCarrierIDReq;
    char CarrierIDBuf[64];
	PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301 ReqBuff;

	void GetCarrierIDReqBuffer(PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301 TempBuff);
	void CarrierIDReqResponse(PKT_OCS2OHT* pRcvData);
	void ClearCarrierID();
	void ClearReqBuff();

	bool LoadCarrierID();
    bool IsFOSB();

};

#endif



