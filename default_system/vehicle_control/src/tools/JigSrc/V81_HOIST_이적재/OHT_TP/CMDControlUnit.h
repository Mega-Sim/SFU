//---------------------------------------------------------------------------

#ifndef CMDControlUnitH
#define CMDControlUnitH

#include "../Common/Def_DataType.h"
#include "../Common/Def_Protocol.h"
#include "MainUnit.h"
#include "Define.h"
#include "../Common/Def_TPCommand.h"
#include "ConnectUnit.h"
#include "FoupOnlineCMDUnit.h"
//#include "ReticleOnlineCMDUnit.h"
#include "EUVOnlineCMDUnit.h"
//#include "ProbeOnlineCMDUnit.h"
//#include "ProbeJigOnlineCMDUnit.h"
#include "OHT_TLV.h"
//---------------------------------------------------------------------------

namespace IO_TYPE	// 실행중 사이클
{
	enum Reserved
	{
		ON_OFF_TYPE	  =	0,
		ZERO_OR_ONE_TYPE	= 1,
	};
};


class TCMDCONTROL
{
private:
    CMD_HEADER  sSndHeader;  // Header
    
public :
    TCMDCONTROL();
    ~TCMDCONTROL();

    // For Assistant
    void MakeTPCmd2AST(int CmdType, uTP_CMD *pTPCmd);
    
    void Init_MSG_HEADER();

    // For OHT
    void MakeTP_IO_SET_Cmd2OHT(int iIoType, int iIndex, int iValue);
	void MakeTP_RAW_SEND_Cmd2OHT(char* buf, int len);
    void MakeTPCmd2OHT(int CmdType);
    void MakeTPCmd2OHT(int CmdType, uTP_CMD *pTPCmd);

	void MakeTP_Quick_Cmd2OHT(int CmdType);

    // 주행 Command 전송 함수
    void SendGoCmd2OHT(int CmdType, GO_CMD *pGoCmd);
    void SendGoCmdEtc2OHT(int CmdType, uGO_ETC_CMD *pGoCmdEtc);

    // 이적재 Command 전송
    void SendTransCmd2OHT(int CmdType, TRANS_CMD *pTransCmd);
};

#endif
