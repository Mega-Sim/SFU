//---------------------------------------------------------------------------
/**
@file    CID.cpp
@version OHT 7.0
@brief   CID Class CPP File
*/
#include <windows.h>
#include <stdlib.h>
#include <math.h>


#include "CID.h"
#include "Utility.h"
#include "ErrorCode.h"
#include "OHTMain.h"

#define	CR					0x0D
#define	LF					0x0A
#define	STX					0x02
#define	ETX					0x03
#define CMD_CID_RS      "RS95"

#include "LogHandler.h"	// Log

//#define CID_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\CID.log", __VA_ARGS__)
//수정
#define CID_LOG(...)	  WRITE_LOG_UNIT(pLogCID, __VA_ARGS__)
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)


#define CID_BAUDRATE     19200
#define CID_Left_Link_CMD    0
#define CID_Right_Link_CMD   1
#define CID_Occup_OK_CMD     2
#define CID_Occup_compt_CMD  3
#define CID_Occup_compt_Ack_CMD  4



#include "CID.h"

#define PAUSED_BY_TP       		1
#define PAUSED_BY_OCS      		2


/*
- CID-O Serial Comm. Command
. M - CID-O/R Status Monitor
. S - CID System Error Info.
. L - Log Info.
. O - OHT No. Set or Monitor
. W - Comm Activity Check Count Set or Monitor
. R - Fiber/RF Comm. Retry Count Set or Monitor
. V - Version Info.
. T - Local Time Set or Monitor
. PI - Auto Scan True or False

. XONCLK - ???

*/

//---------------------------------------------------------------------------


typedef struct _CID_SETTING_
{
	int nPortNum;
    unsigned int nIndexOfReadOK;
	unsigned int nIndexOfReadNG;
} CID_SETTING;

//@param   pHWCommon : HWCommon Class

CID::CID(HWCommon *pHWCommon)
{
	 pLogCID = getLogUnit("D:\\log\\CID_LOG.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	 pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	//m_nLatestID = 0;
	//m_pnResultData = NULL;
	m_pHWCommon = pHWCommon;

    Init();

	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
}

/**
@brief   CID ¼Ò¸ê?Ú
@author	 tg117
@date    2015.12.01
*/
CID::~CID()
{

}

/**
@brief   CID
@author	 tg117
@date    2015.12.01
@return	 ErrorCode Return
*/
void CID::Init()
{
	CID_Output_Clear(0);

	bReadyOperation = false;
	bNBranchCheck = false;
	bPuaseStatus = false;
	dwMaxMovingTime = 0;
	nSendCommand = 0;
	dwStartTime = 0;
	nDelaycnt = 0;
	dwCIDTrigerTime = timeGetTime();
	dwCIDStatusTrigerTime = timeGetTime();
	bLRNBranch = false;
	bRLNBranch = false;
   //	dwLinkLimitTime = CID_LINK_LIMIT_TIME;
	m_nPortNum = COM_CID;    // 14
	m_pCommPort = NULL;

    bCIDPause = false;
	CID_Open();


	bCid_State_Reset_Need = false;
}
//
int CID::CID_Open()
{
	int nError=NO_ERR;
	if(m_pCommPort==NULL)
	{
		m_pCommPort = new Comm_Port(m_nPortNum, CID_BAUDRATE, 8 ,ODDPARITY, ONESTOPBIT );

		if(m_pCommPort==NULL)
		{
			nError=ERR_CID_OPEN;
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_OPEN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
		//test log
		CID_LOG("CID OPEN");

	}
	return nError;
}
int CID::SendMonitoringCmd(void)
{
	int nError=0;
	char strMsg[1024] = {0,};
	AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare, strRcv_Compare;

	char ReadBuff[1024] = {0, };

	//SerialReadBufferClear();

	if(nError==NO_ERR && m_pCommPort!=NULL)
	{
		strsndCmd.sprintf("M4D");
		CID_LOG("[CID][SendMonitoringCmd]: %s" , strsndCmd);
		if(strsndCmd.Length() != (int)(m_pCommPort->WriteCommBlock(strsndCmd.c_str(),strsndCmd.Length())))
		{
			nError = ERR_CID_WRITE;
			CID_LOG("[CID][SendMonitoringCmd] E%d", nError);
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_WRITE,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
	}

	return nError;
}

int CID::SendMonitoringCmd_Clear(void)
{
	int nError=0;
	char strMsg[1024] = {0,};
	AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare, strRcv_Compare;

	char ReadBuff[1024] = {0, };

	SerialReadBufferClearManual();

	if(nError==NO_ERR && m_pCommPort!=NULL)
	{
		strsndCmd.sprintf("M4D");
		CID_LOG("[CID][SendMonitoringCmd_Clear]: %s" , strsndCmd);
		if(strsndCmd.Length() != (int)(m_pCommPort->WriteCommBlock(strsndCmd.c_str(),strsndCmd.Length())))
		{
			nError = ERR_CID_WRITE;
			CID_LOG("[CID][SendMonitoringCmd_Clear] E%d", nError);
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_WRITE,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
	}

	return nError;
}

int CID::SendReq_CID_O(void)
{
	int nError=0;
	char strMsg[1024] = {0,};
	AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare, strRcv_Compare;

	char ReadBuff[1024] = {0, };

	//SerialReadBufferClear();

	if((nError==NO_ERR || nError==ERR_CID_OCCUP_ON_INTERLOCK_ON) && m_pCommPort!=NULL)
	{
		strsndCmd.sprintf("L4C");

		CID_LOG("[CID][SendReq_CID_O]: %s" , strsndCmd);
		if(strsndCmd.Length() != (int)(m_pCommPort->WriteCommBlock(strsndCmd.c_str(),strsndCmd.Length())))
		{
			nError = ERR_CID_WRITE;
			CID_LOG("[CID][SendReq_CID_O] E%d", nError);
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_WRITE,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
	}

	return nError;
}

//
void CID::CID_Close()
{
	if(m_pCommPort!=NULL)
    {
		delete m_pCommPort;
		m_pCommPort = NULL;
	}
}


//
int CID::Left_Link()
{
	int nError = NO_ERR;

	if (nError == NO_ERR)
	{
		CID_LOG("[CID] L-operation On");
		nError = m_pHWCommon->SetBit(OUT_CID_Operation_Mode, ON);
		if (nError == NO_ERR)
		{
			nError = m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, ON);
			CID_LOG("[CID] L-CID_Select_ON");
		}
		if (nError == NO_ERR)
		{
			nError = m_pHWCommon->SetBit(OUT_CID_Common_Tag, OFF);
			CID_LOG("[CID] OUT_CID_Common_Tag_Off");
		}
	}

	if (nError)
	{
		nError = ERR_CID_Left_Link_CMD;
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_Left_Link_CMD,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}
	return nError;
}

int CID::Left_Link_Lifter()
{
	int nError = NO_ERR;

	if (nError == NO_ERR)
	{
		if (nError == NO_ERR)
		{
			nError = m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, ON);
			CID_LOG("[Lifter] L-CID_Select_ON");
		}
	}

	if (nError)
	{
		nError = ERR_CID_Left_Link_CMD;
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_Left_Link_CMD,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}
	return nError;
}

//
int CID::Right_Link()
{
	int nError = NO_ERR;

	if (nError == NO_ERR)
	{
		CID_LOG("[CID] R-operation On");
		nError = m_pHWCommon->SetBit(OUT_CID_Operation_Mode, ON);
		if (nError == NO_ERR)
		{
			nError = m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, ON);
			CID_LOG("[CID] R-CID_Select ON");
		}
		if (nError == NO_ERR)
		{
			nError = m_pHWCommon->SetBit(OUT_CID_Common_Tag, OFF);
			CID_LOG("[CID] OUT_CID_Common_Tag_Off");
		}
	}

	if (nError)
	{
		nError = ERR_CID_Right_Link_CMD;
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_Right_Link_CMD,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}
	return nError;
}

int CID::Right_Link_Lifter()
{
	int nError = NO_ERR;

	if (nError == NO_ERR)
	{
		if (nError == NO_ERR)
		{
			nError = m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, ON);
			CID_LOG("[Lifter] R-CID_Select ON");
		}
	}

	if (nError)
	{
		nError = ERR_CID_Right_Link_CMD;
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_Right_Link_CMD,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}
	return nError;
}

//

/*
int CID::Check_CID_Complete()
{
	int nError = 0;
	DWORD dwCIDLinkTime = 0;

   // *** Error Occur : Timeover ****
	switch (nSendCommand)
	{
	case CID_Left_Link_CMD:
		if (IsLinkOK() == true)
		{
			bReadyOperation = true;
			return NO_ERR;
		}
		break;
	case CID_Right_Link_CMD:
		if (IsLinkOK() == true)
		{
			bReadyOperation = true;
			return NO_ERR;
		}
		break;
	case CID_Occup_OK_CMD:
		if (IsOccupying() == true)
		{
			bReadyOperation = true;
			return NO_ERR;
		}
		break;
	case CID_Occup_compt_CMD:
		if ((ISCompletionACK_On() == true)||(nDelaycnt >= 50))
		{
			nError = m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, OFF);
			if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, OFF);
			if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(CID_Occupancy, OFF);
			if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_CID_Pass_Completion, OFF);
			if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(CID_Abnormal, OFF);
			if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_CID_Operation_Mode, OFF);
			CID_LOG("CID_Occup_compt_CMD : True");
			bReadyOperation = true;
			bNBranchCheck = false;
			return NO_ERR;
		}
		break;
	case CID_Occup_compt_Ack_CMD:
		if (ISCompletion_On() == true)
		{
			bReadyOperation = true;
			return NO_ERR;
		}
	}
	return NEXT_CHECK;
}
*/

// shkim. 2016-11-03
bool CID::IsOutputAllOff()
{
	bool bResult = false;
	if(m_pHWCommon->m_Output.CID_Left_Select_ON == OFF
		&& m_pHWCommon->m_Output.CID_Right_Select_ON == OFF
		&& m_pHWCommon->m_Output.CID_Occupancy == OFF
		&& m_pHWCommon->m_Output.CID_Pass_Completion == OFF
		&& m_pHWCommon->m_Output.CID_Abnormal == OFF
		&& m_pHWCommon->m_Output.CID_Operation_Mode == OFF
		&& m_pHWCommon->m_Output.CID_Common_Tag == OFF)
		bResult = true;
	else
		bResult = false;
	return bResult;
}

bool CID::IsCommonTag()
{
	bool bResult = false;
	if(m_pHWCommon->m_Output.CID_Left_Select_ON == OFF
		&& m_pHWCommon->m_Output.CID_Right_Select_ON == OFF
		&& m_pHWCommon->m_Output.CID_Common_Tag == ON)
		bResult = true;
	else
		bResult = false;
	return bResult;
}


bool CID::IsInputAllOff()
{
	bool bResult = false;


	if(m_pHWCommon->m_Input.CID_Link_Compt_IN == OFF
		&& m_pHWCommon->m_Input.CID_Interlock_IN == OFF
		&& m_pHWCommon->m_Input.CID_Completion_ACK == OFF
	&& m_pHWCommon->m_Input.CID_Status_IN == OFF)

//	if(m_pHWCommon->m_Input.CID_Interlock_IN == OFF	
//	&& m_pHWCommon->m_Input.CID_Status_IN == OFF)
		bResult = true;
	else
		bResult = false;
	return bResult;
}

bool CID::IsLinkOK()
{
	bool bResult = false;

	if ((m_pHWCommon->m_Input.CID_Link_Compt_IN == ON))
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}

bool CID::IsLinking()
{
	bool bResult = false;

	if ((m_pHWCommon->m_Output.CID_Left_Select_ON == ON)||(m_pHWCommon->m_Output.CID_Right_Select_ON == ON))
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}

bool CID::IsLinking_Left()
{
	bool bResult = false;

	if (m_pHWCommon->m_Output.CID_Left_Select_ON == ON)
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}

bool CID::IsLinking_Right()
{
	bool bResult = false;

	if (m_pHWCommon->m_Output.CID_Right_Select_ON == ON)
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}

//bool CID::IsOccupOK()
bool CID::IsOccupying()
{
	bool bResult = false;

	if  (m_pHWCommon->m_Output.CID_Occupancy == ON)
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}

//bool CID::IsOccup_InterLock()
bool CID::IsInterlock_ON()
{
	bool bResult = false;

	if  (m_pHWCommon->m_Input.CID_Interlock_IN == ON)
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}


//bool CID::ISOccupcomptOK()
bool CID::ISCompletionACK_On()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.CID_Completion_ACK == ON)
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}

//bool CID::ISOccupcomptAckOK()
bool CID::ISCompletion_On()
{

	bool bResult = false;

	if  (m_pHWCommon->m_Output.CID_Pass_Completion == ON)
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}

bool CID::ISCidManualMode()
{
	bool bResult = false;

	if  (m_pHWCommon->m_Output.CID_Abnormal == ON)
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}

bool CID::ISCidOperatingMode()
{
	bool bResult = false;

	if  (m_pHWCommon->m_Output.CID_Operation_Mode == ON)
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}

bool CID::ISCommonTagMode()
{
	bool bResult = false;

	if  (m_pHWCommon->m_Output.CID_Common_Tag == ON)
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}
bool CID::ISCidStatusOn()
{
	bool bResult = false;

	if  (m_pHWCommon->m_Input.CID_Status_IN == ON)
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}

bool CID::IS_Occup_On()
{

	bool bResult = false;

	if  (m_pHWCommon->m_Output.CID_Occupancy == ON)
		{
		bResult = true;
		}
	else
		bResult = false;
	return bResult;
}

// SetOutput_Occupancy
//
int CID::CID_Occup_OK()
{

	int nError = NO_ERR;

	m_pHWCommon->SetBit(OUT_CID_Occupancy, true);
	CID_LOG("[CID] Occup-OK");

#if 0
//	 if ((m_pHWCommon->m_Input.CID_Link_Compt_IN == ON)&&(m_pHWCommon->m_Input.CID_Interlock_IN == OFF)
//	   &&(m_pHWCommon->GetAMCPauseStatus() == 0) && (bPuaseStatus==false))
	if ((m_pHWCommon->GetAMCPauseStatus() == 0) && (bPuaseStatus==false))

	{
//		if(dwStartTime - dwCIDTrigerTime > 120)
		if (OHTUTILITY::PreventOverflowGetTimeMs(dwCIDTrigerTime)> 0)

		{
			m_pHWCommon->SetBit(OUT_CID_Occupancy, true);
			CID_LOG("Occup-OK");
			dwCIDTrigerTime = timeGetTime();
		}
	}
	else
	{
		CID_LOG("Occup- NOT OK");
	}
#endif

	return nError;
}


//

int CID::CID_Occup_Pause(bool *bcheck)
{
	int nError = NO_ERR;

	m_pHWCommon->SetPauseControl(true, PAUSED_BY_OCS);
    bCIDPause = true;
	*bcheck = true;
	CID_LOG("[CID] Occup-Pause");
	
/*
	  if ((m_pHWCommon->m_Input.CID_Link_Compt_IN == ON)&&(m_pHWCommon->m_Input.CID_Interlock_IN == ON)

		&&(m_pHWCommon->GetAMCPauseStatus() == 0))
	  {

//		if(dwStartTime - dwCIDTrigerTime > 100)
		if(dwStartTime - dwCIDTrigerTime > 0)
		{
			m_pHWCommon->SetPauseControl(true, PAUSED_BY_OCS);
			bPuaseStatus = true;
			bcheck = true;
			CID_LOG("[CID] Occup-Pause");
			dwCIDTrigerTime = timeGetTime();
		}
	  }
*/

	return nError;
}


int CID::CID_Occup_Resume(bool *bcheck)
{
	int nError = NO_ERR;
// 2016-12-29, Add by Labeler

//	m_pHWCommon->SetPauseControl(false, PAUSED_BY_OCS);
    bCIDPause = false;
	*bcheck = false;
	CID_LOG("[CID] Occup-Resume");

/*
	int nError = NO_ERR;
		if ((m_pHWCommon->m_Input.CID_Link_Compt_IN == ON)&&(m_pHWCommon->m_Input.CID_Interlock_IN == OFF)

			&&(m_pHWCommon->GetAMCPauseStatus() == 3))
		{
			DWORD dwStartTime = timeGetTime();
//			if(dwStartTime - dwCIDTrigerTime > 100)
			if(dwStartTime - dwCIDTrigerTime > 0)

			{
				m_pHWCommon->SetPauseControl(false, PAUSED_BY_OCS);
				bPuaseStatus = false;
				bcheck = false;
				CID_LOG("Occup-Resume");
				dwCIDTrigerTime = timeGetTime();
			}

		}
*/
	return nError;
}


//CID_Status_Pause
int CID::CID_Status_Pause(bool &bcheck)
{
	int nError = NO_ERR;

	m_pHWCommon->SetPauseControl(true, PAUSED_BY_OCS);
    bCIDPause = true;
	CID_LOG("[CID] Status-Pause");
	bcheck = true;
	
#if 0
	  if (m_pHWCommon->GetAMCPauseStatus() == 0)
	  {
//		if(dwStartTime - dwCIDStatusTrigerTime > 100)
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwCIDStatusTrigerTime) > 0)
		{
			m_pHWCommon->SetPauseControl(true, PAUSED_BY_OCS);
			bcheck = true;
			CID_LOG("Status-Pause");
			dwCIDStatusTrigerTime = timeGetTime();
		}
	  }
#endif

	return nError;
}


int CID::CID_Status_Resume(bool &bcheck)

{
	int nError = NO_ERR;
// 2016-12-29, Add by Labeler
// OCS, TP, CID Pause?? ??ǘf ?Ǿv?? Resume ?� ?߻??Å??⠀?ǘ V?? ó??
//	m_pHWCommon->SetPauseControl(false, PAUSED_BY_OCS);
    bCIDPause = false;
	bcheck = false;
	CID_LOG("[CID] Status-Resume");

#if 0
		if (m_pHWCommon->GetAMCPauseStatus() == 3)
		{
//			if(dwStartTime - dwCIDStatusTrigerTime > 100)
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwCIDStatusTrigerTime) > 0)

			{
				m_pHWCommon->SetPauseControl(false, PAUSED_BY_OCS);
				bcheck = false;
				CID_LOG("Status-Resume");
				dwCIDStatusTrigerTime = timeGetTime();
			}

		}
#endif

	return nError;
}


//

int CID::CID_Occup_return(unsigned char num)

{
	int nError = NO_ERR;

	m_pHWCommon->SetBit(OUT_CID_Occupancy, false);
	CID_LOG("[CID] CID_Occup_return(%d)",num);
	
#if 0
	  if ((m_pHWCommon->m_Input.CID_Link_Compt_IN == ON)&&(m_pHWCommon->m_Output.CID_Occupancy == ON))
	  {
//		if(dwStartTime - dwCIDTrigerTime > 100)
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwCIDTrigerTime) > 0)
		{
			m_pHWCommon->SetBit(OUT_CID_Occupancy, false);
			CID_LOG("Occup-Return");
			dwCIDTrigerTime = timeGetTime();
		}
	  }
#endif

	return nError;
}
int CID::CID_Occup_compt(unsigned char num)
{
	int nError = NO_ERR;
	nError = m_pHWCommon->SetBit(OUT_CID_Pass_Completion, true);
	CID_LOG("[CID] CID pass complete CID_Occup_compt(%d)",num);
	bNBranchCheck = false;

	if (nError)
	{
		nError = ERR_CID_Occup_compt_CMD;
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_Occup_compt_CMD,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}


	return nError;

}


int CID::CID_Occup_compt_Manual(bool bCommonTag)
{
	int nError = NO_ERR;
	bool bResult =false;

	//CID_Pass_Completion
	nError = m_pHWCommon->SetBit(OUT_CID_Pass_Completion, true);
	CID_LOG("CID_Occup_compt_On");
	bNBranchCheck = false;

	Sleep(100);
	//commontag
	if(bCommonTag ==true)
	{
		if (nError == NO_ERR)
		{
			nError = m_pHWCommon->SetBit(OUT_CID_Common_Tag, true);
			CID_LOG("OUT_CID_Common_Tag_ON");
		}
	}
	Sleep(100);
	CID_Output_Clear(41);

	return nError;


}


//int CID::CID_Occup_compt_Ack()
int CID::TerminateCID(unsigned char num)
{
int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_CID_Occupancy, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_CID_Pass_Completion, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_CID_Abnormal, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_CID_Operation_Mode, OFF);

	CID_LOG("[CID] TerminateCID(%d)",num);
	bReadyOperation = true;
	bNBranchCheck = false;

	if (nError)
	{
		nError = ERR_CID_Occup_compt_Ack;
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_Occup_compt_Ack,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}

	return nError;
}

int CID::CID_Common_Occup_compt()
{
int nError = NO_ERR;

   //	nError = m_pHWCommon->SetBit(OUT_CID_Occupancy, false);
	if (nError == NO_ERR)
	{
		nError = m_pHWCommon->SetBit(OUT_CID_Pass_Completion, true);
		CID_LOG("[CID] CID_Common_Occup_compt");
		bNBranchCheck = false;
	}
	if (nError == NO_ERR)
	{
		nError = m_pHWCommon->SetBit(OUT_CID_Common_Tag, true); //Common Node Tag Out put
		CID_LOG("[CID] OUT_CID_Common_Tag_ON");
	}
	if (nError)
	{
		nError = ERR_CID_Occup_compt_CMD;
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_Occup_compt_CMD,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}

	return nError;

}

/*
int CID::CID_Common_Occup_compt_Manual()
{
int nError = NO_ERR;

   //	nError = m_pHWCommon->SetBit(OUT_CID_Occupancy, false);
	if (nError == NO_ERR)
	{
		nError = m_pHWCommon->SetBit(OUT_CID_Pass_Completion, true);
		CID_LOG("CID_Common_Occup_compt");
		bNBranchCheck = false;
	}
	if (nError == NO_ERR)
	{
		nError = m_pHWCommon->SetBit(OUT_CID_Common_Tag, true);
		CID_LOG("OUT_CID_Common_Tag_ON");
	}
	if (nError)
		nError = ERR_CID_Occup_compt_CMD;
	else
	{
		nDelaycnt = 0;
		dwStartTime = timeGetTime();
		nSendCommand = CID_Occup_compt_CMD;
		while ((nError = Check_CID_Complete()) == NEXT_CHECK)
		{
			Sleep(1);
			nDelaycnt++;
		}
	}
	return nError;
}
*/



int CID::CID_Output_Clear(unsigned char num)
{
	int nError = NO_ERR;
	nError = m_pHWCommon->SetBit(OUT_CID_Occupancy, OFF);

	if (nError == NO_ERR)
	nError = m_pHWCommon->SetBit(OUT_CID_Pass_Completion, OFF);

	if (nError == NO_ERR)

	nError = m_pHWCommon->SetBit(OUT_CID_Operation_Mode, OFF);

	if (nError == NO_ERR)
	nError = m_pHWCommon->SetBit(OUT_CID_Common_Tag, OFF);
	Sleep(30);
	if (nError == NO_ERR)
	nError = m_pHWCommon->SetBit(OUT_CID_Abnormal, OFF);

	if (nError == NO_ERR)
	nError = m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, OFF);

	if (nError == NO_ERR)
	nError = m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, OFF);


	bNBranchCheck = false;
	CID_LOG("[CID] CID_Output_Clear(%d)",num);

	return nError;
}
//NºÐ±â Á¶Çâº¯°æ Select On
int CID::CID_Steerring_N_Branch()
{
	int nError = NO_ERR;
	if ((m_pHWCommon->m_Output.CID_Left_Select_ON == ON)&&(bNBranchCheck == false))
	{
		CID_LOG("[CID] N_Branch_L->R On");
		m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, ON);
		bLRNBranch = true;
		bNBranchCheck = true;

		return TRUE;
	}
	else if ((m_pHWCommon->m_Output.CID_Right_Select_ON == ON)&&(bNBranchCheck == false))
		{
		CID_LOG("[CID] N_Branch_R->L On");
			m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, ON);
			bRLNBranch = true;
			bNBranchCheck = true;

		return TRUE;
	}

	return FALSE;
}

int CID::CID_Steerring_N_Branch_Select_Off()
{
	int nError = NO_ERR;
	if ((m_pHWCommon->m_Output.CID_Left_Select_ON == ON)&&(m_pHWCommon->m_Output.CID_Right_Select_ON == ON)&&(bLRNBranch == true))
	{
		CID_LOG("[CID] N_Branch_L->R OFF");
		m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, OFF);
		bLRNBranch = false;
		//bNBranchCheck = true;

		return TRUE;
	}
	else if ((m_pHWCommon->m_Output.CID_Left_Select_ON == ON)&&(m_pHWCommon->m_Output.CID_Right_Select_ON == ON)&&(bRLNBranch == true))
	{
		CID_LOG("[CID] N_Branch_R->L OFF");
		m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, OFF);
		bRLNBranch = false;
		//bNBranchCheck = true;

		return TRUE;
	}

	return FALSE;
}


int CID::CID_Manual_Mode_On()
{
	int nError = NO_ERR;
		if (m_pHWCommon->m_Output.CID_Abnormal == OFF)
		{
			m_pHWCommon->SetBit(OUT_CID_Abnormal, ON);
			CID_LOG("[CID] Abnormal_ON");
		}

	return nError;
}


int CID::CID_Manual_Mode_Off()

{
	int nError = NO_ERR;
		if (m_pHWCommon->m_Output.CID_Abnormal == ON)
		{
			m_pHWCommon->SetBit(OUT_CID_Abnormal, OFF);
			CID_LOG("[CID] Abnormal_OFF");
		}	return nError;
}

// Manual_CID()
int CID::ReadyToOperation(int CurrenrtNodeID,bool bStation)

{
	int nError = NO_ERR;
	int CIDResetNode_arr = 0;
	int CIDResetNode = 0;
	int CIDResetNodeType = 0;
	CID_LOG("[ReadyToOperation] CurrenrtNodeID:%d, bStation :%d ", CurrenrtNodeID, bStation);

	CIDResetNode_arr = OHTMainForm->GetMap()->FindNodeIndexFromCIDReset(CurrenrtNodeID);
	if (CIDResetNode_arr != INDEX_NONE)
	{
	  CIDResetNode = OHTMainForm->GetCID_STD()->m_NodeArray[CIDResetNode_arr].ResetNode;
	  CIDResetNodeType = OHTMainForm->GetCID_STD()->m_NodeArray[CIDResetNode_arr].ResetNodeType;
	}
	CID_LOG("[ReadyToOperation] CIDResetNode_arr:%d, CIDResetNode :%d, CIDResetNodeType : %d", CIDResetNode_arr, CIDResetNode, CIDResetNodeType);
	// Manual -> auto. once run
	bCid_State_Reset_Need = true; //Stopnode ?͠ResetNode ?瀌 station?? ??ũȄ  Auto <ȯ?à false ó??.

	//AutoMode with Driving Tag 
	if(bStation == false)
	{
		if ((IsLinkOK() == true)&&(CIDResetNodeType == Common_Reset_tag))
		{
			CID_Occup_compt_Manual(true);
			CID_LOG("[ReadyToOperation] Common Node Manual Compt");
		}
		//
		else if(IsLinkOK() == true)
		{
			CID_Occup_compt_Manual(false);
			CID_LOG("[ReadyToOperation] Reset Node Manual Compt");
		}
		else if(CIDResetNodeType == Common_Reset_tag) //Common_Reset_tag ó?? 
		{
			m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, ON);
			CID_LOG("[ReadyToOperation] OUT_CID_Left_Select_ON");
			Sleep(200);
			CID_Occup_compt_Manual(true);
			CID_LOG("[ReadyToOperation] Common Node Manual Compt");
		}
		else
		{}
	}else//AutoMode with Station Tag(next node is Reset)
	{
		if ((IsLinkOK() == true) && (CIDResetNode_arr != INDEX_NONE))
		{
			if(CIDResetNode == CurrenrtNodeID)//CurrenrtNodeID ?? ?ǁ??δ Next node
			{
				bCid_State_Reset_Need = false; //Stopnode ?͠ResetNode ?瀌 station?? ??ũȄ  Auto <ȯ?à false ó??.
				
				OHTMainForm->GetDrivingControl()->m_nCidControlStep = CID_STEP_COMPLETION;
				OHTMainForm->GetDrivingControl()->m_Continue_CID_MODE = true;
				//OHTMainForm->GetDrivingControl()->m_Continue_CID_MODE_StopNode = gm_ExecuteInfo->ExecutePositionInfo.CurrNode;
				OHTMainForm->GetDrivingControl()->m_Continue_CID_MODE_ResetNode = CIDResetNode;
				OHTMainForm->GetDrivingControl()->m_Continue_CID_MODE_ResetNodeType = CIDResetNodeType;
				CID_LOG("[ReadyToOperation] AutoMode With Station in CID Collision Area");
			}
		}
		else if(IsLinkOK() == true)
		{
			CID_Occup_compt_Manual(false);
			CID_LOG("[ReadyToOperation] AutoMode With Station in CID Area");
		}
		else
		{}
    }
	

	return nError;
}

bool CID::NeedTO_executeCID_Reset()
{
	if(bCid_State_Reset_Need == true)
	{
		bCid_State_Reset_Need = false;
		return true;
	}
	else
	{
		return false;
	}
}


int CID::Set_CID_IDSET_Cmd(void)
{
	int nError=0;
	char strMsg[1024] = {0,};
	AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare, strRcv_Compare;


	char ReadBuff[1024] = {0, };

	if(nError==NO_ERR && m_pCommPort!=NULL)
	{
		strTmpCmd =
		strsndCmd.sprintf("RC95");
		CID_LOG("[CID][Set_CID_IDSET_Cmd]: %s" , strsndCmd);
		if(strsndCmd.Length() != (int)(m_pCommPort->WriteCommBlock(strsndCmd.c_str(),strsndCmd.Length())))
		{
			nError = ERR_CID_WRITE;

			CID_LOG("[[CID][Set_CID_IDSET_Cmd] E%d", nError);
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_WRITE,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
	}

	return nError;
}

 int CID::Set_CID_RC_Cmd(void)
{
	int nError=0;
	char strMsg[1024] = {0,};
	AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare, strRcv_Compare;


	char ReadBuff[1024] = {0, };

	if(nError==NO_ERR && m_pCommPort!=NULL)
	{
		strsndCmd.sprintf("XONCLK=1=5");
		CID_LOG("[CID][Set_CID_RC_Cmd]: %s" , strsndCmd);
		if(strsndCmd.Length() != (int)(m_pCommPort->WriteCommBlock(strsndCmd.c_str(),strsndCmd.Length())))
		{
			nError = ERR_CID_WRITE;

			CID_LOG("[CID][Set_CID_RC_Cmd] E%d", nError);
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_WRITE,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
	}

	return nError;
}

 int CID::Set_CID_Status_Cmd(void)
{
	int nError=0;
	char strMsg[1024] = {0,};
	AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare, strRcv_Compare;


	char ReadBuff[1024] = {0, };

	// SerialReadBufferClear();

	if(nError==NO_ERR && m_pCommPort!=NULL)
	{
		strsndCmd.sprintf("S53");
		CID_LOG("[CID][Set_CID_Status_Cmd]: %s" , strsndCmd);
		if(strsndCmd.Length() != (int)(m_pCommPort->WriteCommBlock(strsndCmd.c_str(),strsndCmd.Length())))
		{
			nError = ERR_CID_WRITE;

			CID_LOG("[CID][Set_CID_Status_Cmd] E%d", nError);
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_WRITE,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
	}

	return nError;
}

 int CID::Clear_CID_Status_Cmd(void)
{
	int nError=0;
	char strMsg[1024] = {0,};
	AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare, strRcv_Compare;


	char ReadBuff[1024] = {0, };

	// SerialReadBufferClear();

	if(nError==NO_ERR && m_pCommPort!=NULL)
	{
		strsndCmd.sprintf("S53");
		CID_LOG("[CID][Clear_CID_Status_Cmd]: %s" , strsndCmd);
		if(strsndCmd.Length() != (int)(m_pCommPort->WriteCommBlock(strsndCmd.c_str(),strsndCmd.Length())))
		{
			nError = ERR_CID_WRITE;

			CID_LOG("[CID][Clear_CID_Status_Cmd] E%d", nError);
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_WRITE,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
	}

	return nError;
}

 int CID::Set_CID_Monitor_Cmd(int i)
{
	int nError=0;
	char strMsg[1024] = {0,};
	AnsiString strTmpCmd, strsndCmd;
	AnsiString strSnd_Compare, strRcv_Compare;

	char ReadBuff[1024] = {0, };

	// SerialReadBufferClear();

	if(nError==NO_ERR && m_pCommPort!=NULL)
	{
		strsndCmd.sprintf("M4D");
		CID_LOG("[CID][Set_CID_Monitor_Cmd]: %s" , strsndCmd);
		if(strsndCmd.Length() != (int)(m_pCommPort->WriteCommBlock(strsndCmd.c_str(),strsndCmd.Length())))
		{
			nError = ERR_CID_WRITE;
			CID_LOG("[CID][Set_CID_Monitor_Cmd] E%d", nError);
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_WRITE,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
	}

	return nError;
}

int CID::Get_CID_Response(AnsiString strSndData)
{
	int nError=0;
	char ReadBuff[1024] = {0, };
	/*
	strRcvData="";

	if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	if(strSndData.Length() != (int)(m_pCommPort->WriteCommBlock(strSndData.c_str(),strSndData.Length())))
	{
		nError = ERR_CID_WRITE;

		CID_LOG("[E%d] Snd : %c", nError, strSndData);

	if(nTmpError==NO_ERR && m_pCommPort!=NULL)
	{
//		sprintf(strMsg, "%s%c%c",KEIENCE_QRR_TRIGGER, CR, LF);
		sprintf(strMsg, "%c%c%c",STX, COMMON_QRR_TRIGGER, ETX);
		if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		{
			nTmpError=ERR_BCR_WRITE;
		}
	}
	*/
	//Sleep(10);

	if(nError==NO_ERR)
	{
		//if(ReadData(ReadBuff, ']') == 0)
		if(m_pCommPort->ReadUntil(ReadBuff, ETX) == 0)
		{
			nError = ERR_CID_READTIMEOUT;

			CID_LOG("[E%d] ReadBuff : %c", nError, ReadBuff);
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_READTIMEOUT,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
		else
		{
			CID_LOG("Snd:%c Rcv:%c",strSndData, ReadBuff);

			//¸í·É <B=123456:X38>  ºñ±³data 123456:X38]
			//??´ä [B=890-123456:X38] ºñ±³data 123456:X38]
			//strRcvData = ReadBuff;
			//strRcvData=strRcvData.Trim();
			//nPos = strRcvData.Pos("-");
			//strRcvData.Delete(1, nPos);
		}
	}

	return nError;
}
int CID::SerialReadBufferClear()
{
   int i, index = 0;
   char chBuff;

	// for log display
	AnsiString strTmp1, strTmp2;
	char oneChar;
	// --------------------------

	for(i=0; i<255; i++)
   {
		if(m_pCommPort == NULL)
		{
			index=0;
			break;
		}
		if(m_pCommPort->ReadCommBlock(&chBuff,1)==0)  // One Byte Reading
		{
			if(m_pCommPort != NULL) PurgeComm(m_pCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
				index=0;
			break;
		}
	}
	// --------- Log Save --------
	CID_LOG("[LIFTER] Serial Read Buffer Cleared! Clear Byte : %d", i);

	return index;
}

int CID::SerialReadBufferClearManual()
{
   int i, index = 0;
   char chBuff;

	// for log display
	AnsiString strTmp1, strTmp2;
	char oneChar;
	// --------------------------

	for(i=0; i<4096; i++)
   {
		if(m_pCommPort == NULL)
		{
			index=0;
			break;
		}
		if(m_pCommPort->ReadCommBlock(&chBuff,1)==0)  // One Byte Reading
		{
			if(m_pCommPort != NULL) PurgeComm(m_pCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
				index=0;
			break;
		}
	}
	// --------- Log Save --------
	CID_LOG("[LIFTER] Serial Read Buffer Cleared! Clear Byte : %d", i);

	return index;
}

int CID::ReadData(char *RtnReadBuff, char cTarget)
{
  int i, index = 0;
   char chBuff;

	// for log display
	AnsiString strTmp1, strTmp2;
	char oneChar;
	// --------------------------

	for(i=0; i<10240; i++)
   {
		if(m_pCommPort == NULL)
		{
			index=0;
			break;
		}
		if(m_pCommPort->ReadCommBlock(&chBuff,1)==0)  // One Byte Reading
		{
			if(m_pCommPort != NULL) PurgeComm(m_pCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
				index=0;
			break;
		}
		*(RtnReadBuff + index ) = chBuff;
		 index++;
		 if(chBuff == cTarget) break;
	}
	// --------- Log Save --------
	if (index > 0 && index <= 1024)
	{
		strTmp1 = " ";

		for (int i = 0; i < index; i++)
		{
			oneChar = RtnReadBuff[i];

			if(oneChar == OPC_SERIAL_INIT_STX)
				strTmp2.sprintf("0\x%02X(STX) ", oneChar);	// HexÇ¥½Ã·Î ¹Ù²Þ
			else if(oneChar == OPC_SERIAL_INIT_ETX)
				strTmp2.sprintf("0\x%02X(ETX) ", oneChar);
			else
				strTmp2.sprintf("0\x%02X(%c) ", oneChar, oneChar);

		   	strTmp1 = strTmp1 + strTmp2;
		}

		CID_LOG("[CID / Lifter] Serial Received :%s", strTmp1);
	}
	else if(index > 1024)
	{
		CID_LOG("[CID / Lifter] Serial Received index over:%s", index);
	}

//#ifdef LIFTER_SCR_DEBUG
//	OHTMainForm->Memo1->Lines->Add(strTmp1);
//#endif
	// ----------------------------------------

	
	return index;
}

// shkim. 2016.09.13
int CID::ReadData(char *RtnReadBuff, char cTarget, int iSize)
{
  int i, index = 0;
   char chBuff;

	// for log display
	AnsiString strTmp1, strTmp2;
	char oneChar;
	// --------------------------

	for(i=0; i<iSize; i++)
   {
		if(m_pCommPort == NULL)
		{
			index=0;
			break;
		}

		if(m_pCommPort->ReadCommBlock(&chBuff,1)==0)  // One Byte Reading
		{
			if(m_pCommPort != NULL) PurgeComm(m_pCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
				index=0;
			break;
		}
		*(RtnReadBuff + index ) = chBuff;
		 index++;
		 if(chBuff == cTarget) break;
	}
	// --------- Log Save --------
	if (index > 0)
	{
		strTmp1 = " ";

		for (int i = 0; i < index; i++)
		{
			oneChar = RtnReadBuff[i];

			if(oneChar == OPC_SERIAL_INIT_STX)
				strTmp2.sprintf("0\x%02X(STX) ", oneChar);	// He
			else if(oneChar == OPC_SERIAL_INIT_ETX)
				strTmp2.sprintf("0\x%02X(ETX) ", oneChar);
			else
				strTmp2.sprintf("0\x%02X(%c) ", oneChar, oneChar);

			strTmp1 = strTmp1 + strTmp2;
		}

		CID_LOG("[CID / Lifter] Serial Received : %s", strTmp1);
	}
	// ----------------------------------------

	return index;
}

#define CIDO_LOG_NUM		100
#define CIDO_LOG_MAX_LEN	512

int CID::Save_CID_O_Data()
{
	int nError=0;
	TDateTime tdt;
	String strFileName = "D:\\log\\CID_O_LogDump"+Now().FormatString("yyyymmddhh")+".log";

	ofstream fs(strFileName.c_str(),ios::out);
	char ReadBuff[CIDO_LOG_NUM][CIDO_LOG_MAX_LEN] = {0, };
	int bufSize[CIDO_LOG_NUM] = {0,};
	AnsiString strSnd;

	String strTemp;
	if(fs.is_open())
	{
		//strSnd = "L=MD6";
		//strSnd = "L4C";

		//if( m_pCommPort != NULL)
		//	PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		OHTMainForm->memoMain->Lines->Add("Save_CID_O_Data");

		//if(strSnd.Length() != (int)(m_pCommPort->WriteCommBlock(strSnd.c_str(),strSnd.Length())))
		//{
		//	OHTMainForm->memoMain->Lines->Add("CID_O Write error!");
		//	nError = ERR_MAP_FILESAVEFAIL;
		//}
		Sleep(2000);

		if(nError==NO_ERR)
		{
			
			for(int i=0; i<CIDO_LOG_NUM; i++)
			{
				bufSize[i] = m_pCommPort->ReadCommBlock(ReadBuff[i], CIDO_LOG_MAX_LEN);
				fs.write(ReadBuff[i], bufSize[i]);
				if (bufSize[i] < CIDO_LOG_MAX_LEN) 
				{
					OHTMainForm->memoMain->Lines->Add("CID_O log save success");
					CID_LOG("CID_O log save success : %d, %d", i, bufSize[i]); // l 100ea i=6
					break;
				}
				Sleep(100);
			}

		}
		fs.close();
	}
	else
	{
		nError = ERR_MAP_FILEOPENFAIL;
	}
	return nError;
}


int CID::CID_Out_Left_Select(bool bOnOff)
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_CID_Left_Select_ON, bOnOff);

	return nError;
}
int CID::CID_Out_Right_Select(bool bOnOff)
{
	int nError = NO_ERR;
	nError = m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, bOnOff);
	return nError;
}
int CID::CID_Out_Occup_Req(bool bOnOff)
{
	int nError = NO_ERR;
	nError = m_pHWCommon->SetBit(OUT_CID_Occupancy, bOnOff);

	return nError;
}
int CID::CID_Out_PASS_Compt(bool bOnOff)
{
	int nError = NO_ERR;
	nError = m_pHWCommon->SetBit(OUT_CID_Pass_Completion, bOnOff);

	return nError;
}
int CID::CID_Out_Manual(bool bOnOff)
{
	int nError = NO_ERR;
	nError = m_pHWCommon->SetBit(OUT_CID_Abnormal, bOnOff);

	return nError;
}
int CID::CID_Out_Operation(bool bOnOff)
{
	int nError = NO_ERR;
	nError = m_pHWCommon->SetBit(OUT_CID_Operation_Mode, bOnOff);

	return nError;
}
int CID::CID_Out_Common_Tag(unsigned char num, bool bOnOff)
{
	int nError = NO_ERR;
	nError = m_pHWCommon->SetBit(OUT_CID_Common_Tag, bOnOff);
	CID_LOG("[CID] CID_Out_Common_Tag(%d) : %d",num,  bOnOff);
	return nError;
}
int CID::CID_Out_Reserved_2(bool bOnOff)
{
	int nError = NO_ERR;
	nError = m_pHWCommon->SetBit(OUT_CID_Reserved_OUT2, bOnOff);

	return nError;
}

int CID::SendLGCmd_NotClear(char* cProtocol, int iSize)
{
	AnsiString strTmp1, strTmp2;
	
	int nError=NO_ERR;
	int iSendByte = 0;
	char cBuffer[255];
	char oneChar;

	// --------- Log Save --------
	memset(&cBuffer, '\0', 255);
	memcpy(&cBuffer, cProtocol, iSize);	

	strTmp1 = "";
 
	// SerialReadBufferClear();

	for (int i = 0; i < iSize; i++)
	{
		oneChar = cProtocol[i];		

		if(oneChar == OPC_SERIAL_INIT_STX)
			strTmp2.sprintf("0\x%02X(STX) ", oneChar);
		else if(oneChar == OPC_SERIAL_INIT_ETX)
			strTmp2.sprintf("0\x%02X(ETX) ", oneChar);
		else
			strTmp2.sprintf("0\x%02X(%c) ", oneChar, oneChar);	
			
		//strTmp1 = strTmp1 + strTmp2;
	}

//	CID_LOG("[CID / LIFTER]  : %s\n", strTmp1);

	// ----------------------------------------

	// Real Send
	if(m_pCommPort!=NULL)
	{
		CID_LOG("[CID / LIFTER] m_pCommPort Not NULL");
		iSendByte = (int)(m_pCommPort->WriteCommBlock(cProtocol,iSize));
		
		if(iSize != iSendByte)
		{
			nError = ERR_CID_WRITE;
			CID_LOG("[CID / LIFTER] : Err : %d Send : %d", nError, iSendByte);
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_WRITE,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
		else
		{
			CID_LOG("[CID / LIFTER] : Send Success. Byte : %d", iSendByte);
		}
	}
	else
	{
		CID_LOG("[CID / LIFTER] : m_pCommPort is NULL Error!");
	}	

	return nError;
}

int CID::SendLGCmd(char* cProtocol, int iSize)
{
	AnsiString strTmp1, strTmp2;
	
	int nError=NO_ERR;
	int iSendByte = 0;
	char cBuffer[255];
	char oneChar;

	// --------- Log Save --------
	memset(&cBuffer, '\0', 255);
	memcpy(&cBuffer, cProtocol, iSize);	

	strTmp1 = "";
 
	//SerialReadBufferClear();

	for (int i = 0; i < iSize; i++)
	{
		oneChar = cProtocol[i];		

		if(oneChar == OPC_SERIAL_INIT_STX)
			strTmp2.sprintf("0\x%02X(STX) ", oneChar);
		else if(oneChar == OPC_SERIAL_INIT_ETX)
			strTmp2.sprintf("0\x%02X(ETX) ", oneChar);
		else
			strTmp2.sprintf("0\x%02X(%c) ", oneChar, oneChar);	
			
	   //	strTmp1 = strTmp1 + strTmp2;
	}

//	CID_LOG("[CID / LIFTER]  : %s", strTmp1);

	// ----------------------------------------

	// Real Send
	if(m_pCommPort!=NULL)
	{
		CID_LOG("[CID / LIFTER] m_pCommPort Not NULL");
		iSendByte = (int)(m_pCommPort->WriteCommBlock(cProtocol,iSize));
		
		if(iSize != iSendByte)
		{
			nError = ERR_CID_WRITE;
			CID_LOG("[CID / LIFTER] : Err : %d Send : %d", nError, iSendByte);
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_WRITE,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		}
		else
		{
			CID_LOG("[CID / LIFTER] : Send Success. Byte : %d", iSendByte);
		}
	}
	else
	{
		CID_LOG("[CID / LIFTER] : m_pCommPort is NULL Error!");
	}	

	return nError;
}
 

int CID::IsElevetorArea()
{

//	if ((m_pHWCommon->m_Input.CID_Reserved_IN1 == ON))
//	// if ((m_pHWCommon->m_Input.CID_Status_IN == ON))
//		return TRUE;
//	else
//		return FALSE;
}

int CID::ElevetorSelectOff()
{
	int nErrorLeft  = NO_ERR;
	int nErrorRight = NO_ERR;

	nErrorLeft  = m_pHWCommon->SetBit(OUT_CID_Left_Select_ON,  OFF);
	nErrorRight = m_pHWCommon->SetBit(OUT_CID_Right_Select_ON, OFF);

	if(nErrorLeft  != NO_ERR) 
	{
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_Left_Link_CMD,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_CID_Left_Link_CMD;
	}
	
	if(nErrorRight != NO_ERR)
	{
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					ERR_CID_Right_Link_CMD,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_CID_Right_Link_CMD;
	}

	return NO_ERR;
}


int CID::ElevetorOperatingModeSet(int iMode)
{
	int nError = m_pHWCommon->SetBit(OUT_CID_Operation_Mode, iMode);
}

void CID::LOG_WRITE_2_CID_LOG(AnsiString strLog)
{
	CID_LOG(strLog.c_str());
}

bool CID::Check_Mode_Lifter(bool LightGuideDirectionIsLeft)
{
	bool bResult = true;

	if(m_pHWCommon->m_Output.CID_Operation_Mode == ON)
		bResult = false;

	if(LightGuideDirectionIsLeft == true)
	{
		if(m_pHWCommon->m_Output.CID_Left_Select_ON == OFF)
         	bResult = false;

	}else
	{
	   if(m_pHWCommon->m_Output.CID_Right_Select_ON == OFF)
        	bResult = false;
    }

	return bResult;
}



/**
@brief   CID Pause
@author  Labeler
@date    2016-12-29
@return   CID Pause (true : CID Pause, false : CID Not Pause)
*/
bool CID::GetCIDPause()
{
    return bCIDPause;
}

void CID::SetCIDPause(bool bSet)
{
    bCIDPause = bSet;
	//m_CID_Pause = bSet;
}
