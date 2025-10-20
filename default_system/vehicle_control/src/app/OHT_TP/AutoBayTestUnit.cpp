//---------------------------------------------------------------------------

#include <vcl.h>
#include <tlhelp32.h>
#include <CString.h>


#pragma hdrstop
#include "AutoBayTestMapViewLoad.h"
#include "AutoBayTestRouteRequestUnit.h"
#include "AutoBayTestUnit.h"
#include "OHTInfoUnit.h"
#include "WarnningUnit.h"
#include "MainUnit.h"
#include "UDP.h"
#include "CMDControlUnit.h"
#include "ChkExecUnit.h"
#include "SocketUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tfrm_AutoBayTest *frm_AutoBayTest;
TCHKOHT CmdFoupOHTCheck;

//---------------------------------------------------------------------------
__fastcall Tfrm_AutoBayTest::Tfrm_AutoBayTest(TComponent* Owner)
	: TForm(Owner)
{
	strngrdRouteTable->Cells[NO][0] = "No";
	strngrdRouteTable->Cells[START_NODE][0] = "StartNode";
	strngrdRouteTable->Cells[END_NODE][0] = "EndNode";
	strngrdRouteTable->Cells[ROUTE][0] = "Route";
	strngrdRouteTable->Cells[OFFSET][0] = "Offset";
	strngrdRouteTable->Cells[STATUS][0] = "Status";
	strngrdRouteTable->Cells[RESULT][0] = "Result";

	tmrAutoBayLongRunTimer->Enabled = false;
	m_bRouteCheckSignal = false;
	m_bRouteCheckResult = false;
	m_bSequenceMove		= false;
	m_bSimulationReq	= false;
	m_nMode = 0;
	m_nCurrentStep = 0;
	m_nIndex = 0;
	m_nSelectedRowNum = -1;
	m_nStartIDFromIndex = 0;
	m_nLongrunCurrentCount = 1;
	m_nLongrunSetCount = 1;
	m_dPreOffset = 0;
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_AutoBayTest::tmrAutoBayLongRunTimerTimer(TObject *Sender)
{
    int nResult = 0;
	int nError = 0;
	int nProgressBarPercent = 0;
	static int  nPreStep = WAIT_STEP;
	static bool bRouteRequestIng = false;
	static bool bDriveRequestIng = false;
	static DWORD dwLastTimeOfDelay = timeGetTime();
	static DWORD dwLastTimeOfDelay_Simulation = timeGetTime();  //testCode;
	String sResult ="";
	String sErrorCode ="";

	switch(m_nMode)
	{
		case NONE_MODE:
			edtMode->Text = "None";
		break;
		case NODE_MODE:
			edtMode->Text = "Node";
		break;
		case STATION_MODE:
			edtMode->Text = "Station";
        break;
		case LINK_MODE:
			edtMode->Text = "Link";
		break;

	}

	switch(m_nCurrentStep)
	{
		case WAIT_STEP:
			lblTestStatus->Caption = "대기중";
		break;
		case ROUTE_STEP:
			lblTestStatus->Caption = "경로요청중";
		break;
		case REQ_MOVE_STEP:
			lblTestStatus->Caption = "주행요청";
        break;
		case CHECK_MOVE_STEP:
			lblTestStatus->Caption = "주행상태 체크";
		break;
		case CHECK_COMPLETE_STEP:
			lblTestStatus->Caption = "주행완료";
		break;

	}



	if(!m_bLongrun)
	{
      	if(m_nRowCount != 0 &&  m_nLongrunSetCount != 0)
			nProgressBarPercent =  (m_nIndex * 100) / m_nRowCount * (m_nLongrunCurrentCount/m_nLongrunSetCount);
		else if(m_nRowCount != 0 &&  m_nLongrunSetCount == 0)
			nProgressBarPercent =  (m_nIndex * 100) / m_nRowCount;
	}
	else
	{
    	nProgressBarPercent = 0;
    }


	if(nProgressBarPercent > 100)
		nProgressBarPercent = 100;

	edtRouteIndex->Text = IntToStr(m_nIndex);

	if(m_bSequenceMove)
	{
		edtCountSet->ReadOnly = true;
		pbLongrun_per->Position = nProgressBarPercent;
	}
	else
	{
    	edtCountSet->ReadOnly = false;
	}





	//에러상황일땐
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ERROR)
	{
		lblTestStatus->Caption = "에러발생 대기";
		if(nPreStep == CHECK_MOVE_STEP)
		{
			if(m_nCurrentStep != WAIT_STEP)
			{
				sErrorCode = gm_OHTStatus->m_OHTStatus.Data.ErrorCode;
				strngrdRouteTable->Cells[STATUS][m_nIndex] = "Error";
				if(strngrdRouteTable->Cells[RESULT][m_nIndex]!="")
				{
					sResult = strngrdRouteTable->Cells[RESULT][m_nIndex];
					sResult += "/" + sErrorCode;

					strngrdRouteTable->Cells[RESULT][m_nIndex] = sResult;
				}
				else
				{
					strngrdRouteTable->Cells[RESULT][m_nIndex] = sErrorCode;
				}
			}
		 }

		m_bRouteCheckSignal = false;
		m_bRouteCheckResult = false;
		m_bDriveCheckSignal = false;
		m_bDriveCheckResult = false;
		m_bSequenceMove = false;
		if(m_nMode != 0)
			m_nPreMode = m_nMode;
		m_nMode = 0;
		m_nCurrentStep = 0;
		m_nIndex = 0;
		m_nCount = 0;
		m_nStartIDFromIndex = 0;
		bRouteRequestIng = false;
		bDriveRequestIng = false;
		m_bLongrun = false;
		chkLongrun->Checked = false;
		shp_Status->Brush->Color = COLOR_RED;
		m_nCurrentStep = WAIT_STEP;
		mmoLongRunResult->Lines->Add("[ERR] OHT Error 발생");
		return;
	}

	if(m_nCurrentStep != ROUTE_STEP)
		bRouteRequestIng = false;
	if(m_nCurrentStep != CHECK_MOVE_STEP)
	{
		bDriveRequestIng = false;
		m_bMoveCheck = false;
		if(nPreStep == CHECK_MOVE_STEP)
		{
			m_bDriveCheckSignal = false;
			m_bDriveCheckResult = false;
		}
	}

	int nStep = m_nCurrentStep;
	switch(m_nCurrentStep)
	{
		case WAIT_STEP:
			shp_Status->Brush->Color = COLOR_WHITE;
			m_bSequenceMove = false;

			if(nPreStep != WAIT_STEP && m_nIndex != 0)
			{
				strngrdRouteTable->Cells[STATUS][m_nIndex] = "";
				m_nIndex = 0;
				m_nStartIDFromIndex = 0;
			}
			lblTestStatus->Caption = "대기중";
			break;
		//Main에 주행경로 추출 요청
		case ROUTE_STEP:
			shp_Status->Brush->Color = COLOR_YELLOW;
			lblTestStatus->Caption = "경로요청";
			if(!bRouteRequestIng)
			{
				bRouteRequestIng = true;
				dwLastTimeOfDelay = timeGetTime();
            }

			shp_Status->Brush->Color = COLOR_LIME;
			//응답이 3초이상 오지 않으면 대기 스텝으로..
			if(PreventOverflowGetTimeMs(dwLastTimeOfDelay) > 3000)
			{
				Warnning_Display(COLOR_RED, "There is no response..3sec");
				m_bRouteCheckSignal = false;
				m_bRouteCheckResult = false;
				m_nCurrentStep = WAIT_STEP;
				break;
			}
			//응답이 왔을 때, 경로탐색 성공한 경우  Route Read..
			if(m_bRouteCheckSignal)
			{
				if(m_bRouteCheckResult)
				{
					InitData();

					if(!ReadRouteList())
						InitData();
				}
				else
				{
                	Warnning_Display(COLOR_RED, "PathSearch Fail....");
				}
				m_bRouteCheckSignal = false;
				m_bRouteCheckResult = false;
				bRouteRequestIng = false;
				m_nCurrentStep = WAIT_STEP;
			}

			break;
		//현재 인덱스로 주행명령 T/P -> OHT
		case REQ_MOVE_STEP :
			lblTestStatus->Caption = "주행명령";
			shp_Status->Brush->Color = COLOR_LIME;
			if(MoveRequest())
				m_nCurrentStep = CHECK_MOVE_STEP; //성공
			else
				m_nCurrentStep = WAIT_STEP;       //실패
			break;

		//주행명령 후, 체크 스텝
		case CHECK_MOVE_STEP :
			lblTestStatus->Caption = "주행상태 체크";
			shp_Status->Brush->Color = COLOR_LIME;
			if(!bDriveRequestIng)
			{
				bDriveRequestIng = true;
				dwLastTimeOfDelay = timeGetTime();
			}

			//응답이 6초이상 오지 않으면 대기 스텝으로..
			//조향체크중 에러발생시 5~6초 소요
			if(m_bDriveCheckSignal == false && PreventOverflowGetTimeMs(dwLastTimeOfDelay) > 6000)
			{
				Warnning_Display(COLOR_RED, "There is no response..6sec");
				m_bDriveCheckSignal = false;
				m_bDriveCheckResult = false;
				m_nCurrentStep = WAIT_STEP;
				sErrorCode = gm_OHTStatus->m_OHTStatus.Data.ErrorCode;
				strngrdRouteTable->Cells[STATUS][m_nIndex] = "Error";
				if(strngrdRouteTable->Cells[RESULT][m_nIndex]!="")
				{
					sResult = strngrdRouteTable->Cells[RESULT][m_nIndex];
					sResult += "/" + sErrorCode;

					strngrdRouteTable->Cells[RESULT][m_nIndex] = sResult;
				}
				else
				{
					strngrdRouteTable->Cells[RESULT][m_nIndex] = sErrorCode;
				}
				break;
			}

			//응답이 왔을 때, 주행명령이 성공한 경우 Check  시작
			if(m_bDriveCheckSignal)
			{
				//주행명령에 실패한 경우
				if(!m_bDriveCheckResult)
				{
					Warnning_Display(COLOR_RED, "Move Request Fail!");
					m_bDriveCheckSignal = false;
					m_bDriveCheckResult = false;
					m_nCurrentStep = WAIT_STEP;

					sErrorCode = gm_OHTStatus->m_OHTStatus.Data.ErrorCode;
					strngrdRouteTable->Cells[STATUS][m_nIndex] = "Error";
					if(strngrdRouteTable->Cells[RESULT][m_nIndex]!="")
					{
						sResult = strngrdRouteTable->Cells[RESULT][m_nIndex];
						sResult += "/" + sErrorCode;

						strngrdRouteTable->Cells[RESULT][m_nIndex] = sResult;
					}
					else
					{
						strngrdRouteTable->Cells[RESULT][m_nIndex] = sErrorCode;
					}
                    break;
				}
			}


			//응답이 성공했을 때
			if(m_bDriveCheckResult)
			{
				nResult = MoveCheck();
				switch(nResult)
				{
					//주행중
					case DRIVING:
						lblTestStatus->Caption = "주행중";
						m_nCurrentStep = CHECK_MOVE_STEP;
					break;
					//다음 주행으로
					case ARRIVED_NEXTSTEP:
						lblTestStatus->Caption = "다음 주행으로";
						m_nIndex++;
						m_nCurrentStep = REQ_MOVE_STEP;
					break;
					//주행 완료
					case ARRIVED_END:
						lblTestStatus->Caption = "도착";
						if(m_bSequenceMove)
						{
                        	if(!m_bLongrun)
							{

								if(m_nLongrunCurrentCount >= m_nLongrunSetCount)
								{
									m_nIndex = 0;
									m_nLongrunCurrentCount = 1;
									m_bSequenceMove = false;
									m_nCurrentStep = CHECK_COMPLETE_STEP;
								}
								else
								{
									m_nLongrunCurrentCount++;
									m_nIndex = 1;
									m_nCurrentStep = REQ_MOVE_STEP;
								}

							}
							else
							{
								m_nIndex = 1;
								m_nCurrentStep = REQ_MOVE_STEP;
							}
						}
						else
						{
							m_nIndex = 0;
							m_nCurrentStep = CHECK_COMPLETE_STEP;
						}
					break;
					//주행 실패
					case DRIVING_FAIL:
						lblTestStatus->Caption = "주행실패";
						m_nIndex = 0;
						m_bSequenceMove = false;
						m_nCurrentStep = WAIT_STEP;
					break;
					//그 외 Abnormal Case
					default:
						m_nIndex = 0;
						m_bSequenceMove = false;
						m_nCurrentStep = WAIT_STEP;
					break;
				}
			}

			break;

		case CHECK_COMPLETE_STEP:
			shp_Status->Brush->Color = COLOR_LIME;
			lblTestStatus->Caption = "주행완료";
			strngrdRouteTable->Cells[STATUS][m_nIndex] = "Arrived";
			m_bSequenceMove = false;
			break;

		default:
            lblTestStatus->Caption = "Abnormal!!";
			break;


	}
    nPreStep = nStep;


}
//---------------------------------------------------------------------------
void Tfrm_AutoBayTest::InitData(void)
{
	m_nIndex = 0;
	m_nRowCount = 0;
	m_nCurrentStep = WAIT_STEP;
	m_nCount = 0;

	for(int i = 1; i<500; i++)
	{
		strngrdRouteTable->Cells[0][i] = "";
		strngrdRouteTable->Cells[1][i] = "";
		strngrdRouteTable->Cells[2][i] = "";
		strngrdRouteTable->Cells[3][i] = "";
		strngrdRouteTable->Cells[4][i] = "";
		strngrdRouteTable->Cells[5][i] = "";
		strngrdRouteTable->Cells[6][i] = "";
	}

}
void Tfrm_AutoBayTest::setMode(int nMode)
{
	m_nMode = nMode;
}

void Tfrm_AutoBayTest::setStep(int nStep)
{
	m_nCurrentStep = nStep;
}

void Tfrm_AutoBayTest::setRouteCheckSignalTrue()
{
    m_bRouteCheckSignal = true;
}

void Tfrm_AutoBayTest::setRouteCheckResult(bool bSuccess)
{
	m_bRouteCheckResult = bSuccess;
}

void Tfrm_AutoBayTest::setDriveCheckSignalTrue()
{
	m_bDriveCheckSignal = true;
}

void Tfrm_AutoBayTest::setDriveCheckResult(bool bResult)
{
	m_bDriveCheckResult = bResult;
}

void Tfrm_AutoBayTest::setSimulationReady(bool bResult)
{
    m_bSimulationReady = bResult;
}

bool Tfrm_AutoBayTest::ReadRouteList()
{
	String IpAddress = SocketForm->m_strOHTIPAddress;
	AnsiString strTmpFileName = "\\\\"+IpAddress + "\\oht\\DataFiles\\NavigationRoute.txt";
	char chTmpFilePath[1024];

	strcpy(chTmpFilePath, strTmpFileName.c_str());

	// File Check
	TCHAR chBuffer[1024];
	String strTmpBuffer;
	String strTmpRoute;
	int Pos = 0;
	int EndPos = 0;
	int nCount = 1;

	if(strTmpFileName.Length() <= 0) return false;

	FILE* fp = fopen(strTmpFileName.c_str(), "r");

	if (fp == NULL)
	{
		fclose(fp);
		return false;
	}

	while (feof(fp) == false)
	{
		swprintf(chBuffer, L"");
        try
        {
			fgetws(chBuffer, 1024, fp);
		}
		catch (Exception &e)
        {
            fclose(fp);
			return false;
		}
		strTmpBuffer = chBuffer;
        strTmpRoute = "";
		if (strTmpBuffer.Length() == 0)  continue;

		strngrdRouteTable->Cells[NO][nCount]= IntToStr(nCount);
		bool bStartFlag = false;
		bool bEndFlag = false;
		AnsiString tmpLeftStr ="";
		for(int j = 0; j < strTmpBuffer.Length(); j++)

		{

			Pos = strTmpBuffer.Pos(" ");

			if(Pos != 0)
			{
				chBuffer[Pos-1]='_';
				strTmpBuffer.Delete(Pos,1);
				strTmpBuffer.Insert('_',Pos);
			}
			else
			{
				strngrdRouteTable->Cells[END_NODE][nCount]= tmpLeftStr;
				tmpLeftStr  = strTmpBuffer.SubString(j, strTmpBuffer.Length()-j);
                strngrdRouteTable->Cells[OFFSET][nCount]= tmpLeftStr;
				Pos = strTmpBuffer.Length();
				bEndFlag = true;
			}

			//문자 중에 숫자가 아닌게 있는지 체크
			for(int k = j; k < Pos-1; k++)
			{
				if(chBuffer[k] < '0' || chBuffer[k] > '9')
				{
					Warnning_Display(COLOR_RED, "Have to be numerical");
					fclose(fp);
					remove(chTmpFilePath);
					return false;
				}
			}

			if(bEndFlag)
				break;

			if(j == 0)
			{
				j = 1;
			}

			tmpLeftStr = strTmpBuffer.SubString(j, Pos-j);
			if(!bStartFlag)
			{
				strngrdRouteTable->Cells[START_NODE][nCount]= tmpLeftStr;
				bStartFlag = true;
			}
			j = Pos;
			strTmpRoute += tmpLeftStr + "-";

		}
		strTmpRoute = strTmpRoute.SubString(1,strTmpRoute.Length()-1);
		strngrdRouteTable->Cells[ROUTE][nCount++] = strTmpRoute;

	}
	m_nRowCount = nCount-1;
	fclose(fp);
	remove(chTmpFilePath);
    return true;

}
bool Tfrm_AutoBayTest::MoveRequest()
{
	AnsiString strMachineID = "TP001";
	AnsiString strTmpBuffer = strngrdRouteTable->Cells[ROUTE][m_nIndex].c_str();
	String strStartID = strngrdRouteTable->Cells[START_NODE][m_nIndex];
	String strEndID = strngrdRouteTable->Cells[END_NODE][m_nIndex];
	strngrdRouteTable->Cells[STATUS][m_nIndex] = "Move Request";
	bool bEndFlag = false;
	int nStartID = 0;
	if(m_nStartIDFromIndex!=0)
		nStartID = m_nStartIDFromIndex;
	else
		nStartID = strStartID.ToInt();

	int nEndID = strEndID.ToInt();
	int nRouteList[50] = {0,};
	int nRouteCnt = 0;
	int Pos = 0;
	int iPacketSize;
	int iReturn;
	char chBuffer[1024] = {0,};

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		Warnning_Display(COLOR_RED, "OHT Mode is not Manual Mode.........");
		return false;
	}

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED)
   {
		Warnning_Display(COLOR_RED, "OHT Status have to be Initialized.......");
		 return false;
   }


	if((gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_AMP_FAULT)	)
    {
        Warnning_Display(COLOR_RED, "Driving Axis1 Error.........");
		return false;
	}

    if( (gm_OHTStatus->m_OHTStatus.Data.ErrorCode) &&
		((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right)))
	{
		Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
		return false;
	}

	if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
	{
		if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) > HOIST_ORIGIN_LIMIT)
        {
			Warnning_Display(COLOR_RED, "Hoist Not Origin...#1 can't Move...");
			return false;
        }
		else if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) > SHIFT_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Shift Not Origin...#1 can't Move...");
			return false;
        }
    }

    if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
    {
		if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
				return false;
            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
				return false;
            }
        }
        else
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
				return false;

            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == false)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
				return false;
            }
		}

	}


	//마크, 스타트지점 체크

	if(m_nMode == NONE_MODE)
	{
		Warnning_Display(COLOR_RED, "Select Mode!");
		return false;
	}


	switch (m_nMode)
	{
		case NODE_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Driving Tag");
				return false;
			}
			if(m_nStartIDFromIndex == 0)
			{
				if(gm_OHTStatus->m_OHTStatus.Data.CurNodeID != nStartID)
				{
					Warnning_Display(COLOR_RED, "ERROR : You have to Mark at StartID");
					return false;
				}
			}
		break;

		case STATION_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag != 1
			|| gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L != 1
			||	gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R != 1 )
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Station/QR Tag");
				return false;
			}
			if(m_nStartIDFromIndex == 0)
			{
				if(gm_OHTStatus->m_OHTStatus.Data.CurStationID !=nStartID)
				{
					Warnning_Display(COLOR_RED, "ERROR : You have to Mark at StartID");
					return false;
				}
			}
		break;

		case LINK_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Driving Tag");
				return false;
			}
			if(m_nStartIDFromIndex == 0)
			{
				if(gm_OHTStatus->m_OHTStatus.Data.CurNodeID != nStartID)
				{
					Warnning_Display(COLOR_RED, "ERROR : You have to Mark at StartID");
					return false;
				}
			}

		break;

		default:
			Warnning_Display(COLOR_RED, "ERROR : You have to Select Mode");
			 return false;
		break;
	}




	if (strTmpBuffer.Length() == 0 || strTmpBuffer.Length() > 1023) return false;
	strcpy(chBuffer, strTmpBuffer.c_str());

	bool bFromIndexID = m_nStartIDFromIndex!=0 ? true:false;
	bool bMetStartID = m_nStartIDFromIndex!=0 ? false:true;

	if(!bFromIndexID)
		m_dPreOffset = 0;

	for(int j = 0; j < strTmpBuffer.Length(); j++)
	{
		String tmpLeftStr ="";
		Pos = strTmpBuffer.Pos("-");

		if(Pos != 0)
		{
			chBuffer[Pos-1]='_';
			strTmpBuffer.Delete(Pos,1);
			strTmpBuffer.Insert('_',Pos);
		}
		else
		{
			tmpLeftStr  = strTmpBuffer.SubString(j, strTmpBuffer.Length()-j+1);

			Pos = strTmpBuffer.Length();
			bEndFlag = true;
		}

		//문자 중에 숫자가 아닌게 있는지 체크
		for(int k = j; k < Pos-1; k++)
		{
			if(chBuffer[k] < '0' || chBuffer[k] > '9')
			{
				Warnning_Display(COLOR_RED, "Route Have to be numerical");
				return false;
			}
		}

		if(bEndFlag)
		{
			nRouteList[nRouteCnt++] = tmpLeftStr.ToInt();
			break;
        }


		if(j == 0)
		{
			j = 1;
		}

		tmpLeftStr = strTmpBuffer.SubString(j, Pos-j);
		j = Pos;

		if(!bFromIndexID)
			nRouteList[nRouteCnt++]  = tmpLeftStr.ToInt();
		else
		{
			int tmpID = tmpLeftStr.ToInt();

			if( m_nStartIDFromIndex == tmpID && !bMetStartID)
				bMetStartID = true;

			if(bMetStartID)
				nRouteList[nRouteCnt++]  = tmpLeftStr.ToInt();
		}
	}
	m_nStartIDFromIndex = 0;

	if(!bMetStartID)
	{
		Warnning_Display(COLOR_RED, "There is No Currnet ID in Current Path.");
		return false;
    }

	// 새로운 TP 이동명령 패킷을 이용한다.
	iPacketSize = sizeof(PKT_TP2OHT_AUTO_DRIVING_TEST_MOVE_REQ);
	PKT_TP2OHT_AUTO_DRIVING_TEST_MOVE_REQ	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::CMD_AUTO_DRIVING_TEST_MOVE_REQ_TP2OHT;

	 // ---- Body Set
	SEND_PACKET.BODY.Mode =	m_nMode;
	SEND_PACKET.BODY.StartID = nStartID;
	SEND_PACKET.BODY.EndID = nEndID;
	SEND_PACKET.BODY.RouteCount = nRouteCnt;
	memcpy(SEND_PACKET.BODY.RouteList, nRouteList, sizeof(nRouteList));

	// 데이터 전송
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);
    m_nStartIDFromIndex = 0;
	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
	{
		ShowMessage(L"Address not registed!!, try first");
		return false;
	}
	else
	{
		return true;
	}
}

void Tfrm_AutoBayTest::excuteSimulation()
{
	int Pos;
	bool bEndFlag = false;
	bool bStartFlag = true;
	String PreMessage ="";
	String strTempMessage = "";
	AnsiString strTmpBuffer = strngrdRouteTable->Cells[ROUTE][++m_nSimulationIndex].c_str();
	AnsiString message = "";


	char chBuffer[1024] = {0,};
	if (strTmpBuffer.Length() == 0 || strTmpBuffer.Length() > 1023) return;
	strcpy(chBuffer, strTmpBuffer.c_str());

	String tmpLeftStr ="";
	for(int j = 0; j < strTmpBuffer.Length(); j++)
	{
		Sleep(300);
		Pos = strTmpBuffer.Pos("-");

		if(Pos != 0)
		{
			chBuffer[Pos-1]='_';
			strTmpBuffer.Delete(Pos,1);
			strTmpBuffer.Insert('_',Pos);
		}
		else
		{
			tmpLeftStr  = strTmpBuffer.SubString(j, strTmpBuffer.Length()-j+1);

			Pos = strTmpBuffer.Length();
			bEndFlag = true;
		}

		//문자 중에 숫자가 아닌게 있는지 체크
		for(int k = j; k < Pos-1; k++)
		{
			if(chBuffer[k] < '0' || chBuffer[k] > '9')
			{
				Warnning_Display(COLOR_RED, "Route Have to be numerical");
				return;
			}
		}

		if(bStartFlag)
		{
			j = 1;
			PreMessage = strTmpBuffer.SubString(j, Pos-j);
			j = Pos;
			bStartFlag =false;
			continue;
		}
		if(bEndFlag)
		{
			message = "ROUTE_LINKVIEW/" + PreMessage + "_" + tmpLeftStr;
			int iReturn = frmUDP->UnicastSend_toMapView(message);
			break;
		}


		tmpLeftStr = strTmpBuffer.SubString(j, Pos-j);

		j = Pos;

		message = "ROUTE_LINKVIEW/" + PreMessage +"_" + tmpLeftStr;
		int iReturn = frmUDP->UnicastSend_toMapView(message);
		PreMessage  =  tmpLeftStr;

	}
	String Num =  IntToStr(m_nSimulationIndex);
	message = "ROUTE_FLAGVIEW/" + tmpLeftStr +"_"+ Num;
	int iReturn = frmUDP->UnicastSend_toMapView(message);

	if(m_nSimulationIndex == m_nRowCount)
	{
		m_bSimulationFlag = false;
		m_bSimulationReady= false;
		m_bSimulationReq = false;
		m_nSimulationIndex = 0;
		AnsiString message = "ROUTE_END";
		int iReturn = frmUDP->UnicastSend_toMapView(message);
	}

}
//---------------------------------------------------------------------------
int Tfrm_AutoBayTest::MoveCheck()
{
	static DWORD dwLastTimeOfDelay = timeGetTime();
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ERROR)
	{
		String sErrorCode = gm_OHTStatus->m_OHTStatus.Data.ErrorCode;
		String strResult = strngrdRouteTable->Cells[RESULT][m_nIndex];
		strResult += "/";
		strResult += "E:";
		strResult += sErrorCode;
		strngrdRouteTable->Cells[RESULT][m_nIndex] = strResult;
		strngrdRouteTable->Cells[STATUS][m_nIndex] = "Error";

		m_bSequenceMove = false;
		m_nCurrentStep = WAIT_STEP;
		return	DRIVING_FAIL;
	}

	if(!m_bMoveCheck)
	{
    	dwLastTimeOfDelay = timeGetTime();
    }

	String strCurNodeOffset =  IntToStr((int)gm_OHTStatus->m_OHTStatus.Data.CurNodeID) +
								"(" + String(gm_OHTStatus->m_OHTStatus.Data.CurNodeOffset) + ")";


	//마크, 스타트지점 체크

	String strStartID = strngrdRouteTable->Cells[START_NODE][m_nIndex];
	String strEndID = strngrdRouteTable->Cells[END_NODE][m_nIndex];

	bool bArrived = false;
	int nStartID = strStartID.ToInt();
	int nEndID = strEndID.ToInt();
	double dResult = 0;
	double dMoveOffset = 0;
	double dMapOffset = strngrdRouteTable->Cells[OFFSET][m_nIndex].ToDouble();

	String sResultMessage ="";


	switch (m_nMode)
	{
		case NODE_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag == 1 &&
				gm_OHTStatus->m_OHTStatus.Data.CurNodeID == nEndID &&
				gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ARRIVED)
				bArrived = true;
		break;

		case STATION_MODE:
			if((gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag == 1 ||
				gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L == 1 ||
				gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R == 1 )&&
				gm_OHTStatus->m_OHTStatus.Data.CurStationID ==nEndID  &&
				gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ARRIVED)
				bArrived = true;
		break;

		case LINK_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag == 1 &&
				gm_OHTStatus->m_OHTStatus.Data.CurNodeID == nEndID &&
				gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ARRIVED)
				bArrived = true;
		break;
	}

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ARRIVED && bArrived == false)
	{
		if(PreventOverflowGetTimeMs(dwLastTimeOfDelay)>2000)
		{
			m_nPreMode = m_nMode;
			return DRIVING_FAIL;
        }

	}


	if(m_bSequenceMove)
	{
		if(!bArrived)
		{
			strngrdRouteTable->Cells[STATUS][m_nIndex] = "GOING "+ strCurNodeOffset;
			return DRIVING;

		}
		else
		{
			if(m_nIndex == m_nRowCount)
			{
				strngrdRouteTable->Cells[STATUS][m_nIndex] = "End LastIndex";
				dMoveOffset = gm_OHTStatus->Motor_Driving_F->dPosition_mm - m_dPreOffset;
				dResult  = dMapOffset - dMoveOffset;
				sResultMessage = strngrdRouteTable->Cells[RESULT][m_nIndex].c_str();
				if(sResultMessage =="")
					sResultMessage = "Diff:";
				else
					sResultMessage += "Diff:";
				sResultMessage += FloatToStr(dResult);
				strngrdRouteTable->Cells[RESULT][m_nIndex] = sResultMessage;
				m_dPreOffset = 0;
				return ARRIVED_END;
			}
			else
			{
				strngrdRouteTable->Cells[STATUS][m_nIndex] = "Next Step";
				dMoveOffset = gm_OHTStatus->Motor_Driving_F->dPosition_mm - m_dPreOffset;
				dResult  = dMapOffset - dMoveOffset;
				sResultMessage = strngrdRouteTable->Cells[RESULT][m_nIndex].c_str();
				if(sResultMessage =="")
					sResultMessage = "Diff:";
				else
					sResultMessage += "/Diff:";
				sResultMessage += FloatToStr(dResult);
				strngrdRouteTable->Cells[RESULT][m_nIndex] = sResultMessage;
				m_dPreOffset = 0;
				return ARRIVED_NEXTSTEP;
			}

		}
	}
	else
	{
		if(!bArrived)
		{
			strngrdRouteTable->Cells[STATUS][m_nIndex] = "GOING "+ strCurNodeOffset;
			return DRIVING;

		}
		else
		{
			strngrdRouteTable->Cells[STATUS][m_nIndex] = "End";
			dMoveOffset = gm_OHTStatus->Motor_Driving_F->dPosition_mm - m_dPreOffset;
			dResult  = dMapOffset - dMoveOffset;
			sResultMessage = strngrdRouteTable->Cells[RESULT][m_nIndex].c_str();
			if(sResultMessage =="")
				sResultMessage = "Diff:";
			else
				sResultMessage += "/Diff:";
			sResultMessage += FloatToStr(dResult);
			strngrdRouteTable->Cells[RESULT][m_nIndex] = sResultMessage;
			m_dPreOffset = 0;
			return ARRIVED_END;
		}

	}

}

void __fastcall Tfrm_AutoBayTest::btnRouteRequestClick(TObject *Sender)
{
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		Warnning_Display(COLOR_RED, "OHT Mode is not Manual Mode.........");
		return;
	}

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED)
   {
		Warnning_Display(COLOR_RED, "OHT Status have to be Initialized.......");
		 return;
   }


	if((gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_AMP_FAULT)	)
    {
        Warnning_Display(COLOR_RED, "Driving Axis1 Error.........");
        return;
	}

    if( (gm_OHTStatus->m_OHTStatus.Data.ErrorCode) &&
		((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right)))
	{
		Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
        return;
    }

    if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
		if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) > HOIST_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Hoist Not Origin...#1 can't Move...");
            return;
        }
		else if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) > SHIFT_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Shift Not Origin...#1 can't Move...");
            return;
        }
    }

    if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
    {
		if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                return;
            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                return;
            }
        }
        else
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
			{
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
                return;

            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == false)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
                return;
            }
		}
	}


	if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R != 1)
	{
		Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Tag");
		return;
	}

	InitData();
	frm_RouteRequest->Show();
	frm_RouteRequest->BringToFront();
}
void __fastcall Tfrm_AutoBayTest::btnInitializationClick(TObject *Sender)
{
	if(!m_bSequenceMove)
		InitData();
	else
		Warnning_Display(COLOR_RED, "sequence is in progress! Please stop Sequence.");
}
//---------------------------------------------------------------------------


void __fastcall Tfrm_AutoBayTest::btnDataSaveClick(TObject *Sender)
{

	TCHAR szFilename[1024];
	TStringList *sLst = new TStringList;
	String sExePath = ExtractFilePath(Application->ExeName);
	String sCSVFile = sExePath + "AutoDrivingTestResult.csv";

	String sFileName = "EXCEL.EXE";
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry32;

	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		exit(EXIT_FAILURE);
	}
	processEntry32.dwSize = sizeof(PROCESSENTRY32);

	if( !Process32First(hProcessSnap, &processEntry32) )
	{
		CloseHandle(hProcessSnap);
		exit(EXIT_FAILURE);

	}

	while(Process32Next(hProcessSnap, &processEntry32))
	{
		String s =  processEntry32.szExeFile;

		if(sFileName.Compare(s) == 0)
		{
			Warnning_Display(COLOR_RED, "EXCEL Have to be closed");
			return;
		}
	}

	try
	{
		for(int i=0;i< strngrdRouteTable->RowCount ; i++)
		{
			sLst->Add( strngrdRouteTable->Rows[i]->CommaText) ;
		}
		sLst->SaveToFile(sCSVFile);
    }
    __finally
    {
        delete sLst;
    }

}
//---------------------------------------------------------------------------



void __fastcall Tfrm_AutoBayTest::btnStartButtonClick(TObject *Sender)
{

	if(m_nCurrentStep != WAIT_STEP)
	{
		Warnning_Display(COLOR_RED, "Please Stop Sequence.");
			return;
    }

	if(m_bSequenceMove)
	{
		Warnning_Display(COLOR_RED, "Please Stop Sequence.");
		return;
	}

	if( m_nRowCount == 0 ||
	strngrdRouteTable->Cells[NO][1].data() == NULL ||
	strngrdRouteTable->Cells[START_NODE][1].data() == NULL ||
	strngrdRouteTable->Cells[END_NODE][1].data() == NULL ||
	strngrdRouteTable->Cells[ROUTE][1].data() == NULL ||
	strngrdRouteTable->Cells[OFFSET][1].data() == NULL )
	{
		Warnning_Display(COLOR_RED, "Please Route Request.");
		return;
	}

	if(m_nRowCount < m_nIndex)
	{
		Warnning_Display(COLOR_RED, "Please select cell included route");
		return;
    }

	String strStartID = strngrdRouteTable->Cells[START_NODE][1];
	int nStartID = strStartID.ToInt();

//에러상황일땐
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ERROR)
	{
		return;
	}

//로직을 수행할 수 없는 상황일 때
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		Warnning_Display(COLOR_RED, "OHT Mode is not Manual Mode.........");
		return;
	}

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED)
   {
		Warnning_Display(COLOR_RED, "OHT Status have to be Initialized.......");
		 return;
   }


	if((gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_AMP_FAULT)	)
    {
        Warnning_Display(COLOR_RED, "Driving Axis1 Error.........");
        return;
	}

    if( (gm_OHTStatus->m_OHTStatus.Data.ErrorCode) &&
		((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right)))
	{
		Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
        return;
    }

    if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
		if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) > HOIST_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Hoist Not Origin...#1 can't Move...");
            return;
        }
		else if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) > SHIFT_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Shift Not Origin...#1 can't Move...");
            return;
        }
    }

    if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
    {
		if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                return;
            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                return;
            }
        }
        else
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
			{
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
                return;

            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == false)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
                return;
            }
		}
	}


	if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R != 1)
	{
		Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Tag");
		return;
	}

//	모드에 맞는 마크상태인지
//	처음 스타트 지점이 현재위치와 맞는지 체크
	switch (m_nMode)
	{
		case NODE_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Driving Tag");
				return;
			}
			if(gm_OHTStatus->m_OHTStatus.Data.CurNodeID != nStartID)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at StartID");
				return;
			}
		break;

		case STATION_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag != 1
			|| gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L != 1
			||	gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R != 1 )
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Station/QR Tag");
				return;
			}
			if(gm_OHTStatus->m_OHTStatus.Data.CurStationID !=nStartID)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at StartID");
				return;
			}
		break;

		case LINK_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Driving Tag");
				return;
			}
			if(gm_OHTStatus->m_OHTStatus.Data.CurNodeID != nStartID)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at StartID");
				return;
			}
		break;

		default:
			Warnning_Display(COLOR_RED, "ERROR : You have to Select Mode");
			return;
		break;
	}
	m_nStartIDFromIndex = 0;
	m_dPreOffset = 0;
	m_nCurrentStep = REQ_MOVE_STEP;
	m_nIndex = 1;
	m_bSequenceMove = true;
	m_nLongrunCurrentCount = 1;
	m_nLongrunSetCount = edtCountSet->Text.ToInt() == 0? 1:edtCountSet->Text.ToInt() ;
	edtCountSet->Text =  IntToStr(m_nLongrunSetCount);
}
 //---------------------------------------------------------------------------

//---------------------------------------------------------------------------



void __fastcall Tfrm_AutoBayTest::btnStartFromIndexClick(TObject *Sender)
{
	if(m_nSelectedRowNum < 0)
	{
		Warnning_Display(COLOR_RED, "Please select cell included route.");
		return;
	}


	m_nMode = m_nPreMode;
	m_nIndex = m_nSelectedRowNum;

	switch(m_nMode)
	{
		case NONE_MODE:
			edtMode->Text = "None";
		break;
		case NODE_MODE:
			edtMode->Text = "Node";
		break;
		case STATION_MODE:
			edtMode->Text = "Station";
		break;
		case LINK_MODE:
			edtMode->Text = "Link";
		break;

	}

	if(m_nCurrentStep != WAIT_STEP)
	{
		Warnning_Display(COLOR_RED, "Please Stop Sequence.");
		return;
    }

	if(m_bSequenceMove)
	{
		Warnning_Display(COLOR_RED, "Please Stop Sequence.");
		return;
	}

	if( m_nRowCount == 0 ||
	strngrdRouteTable->Cells[NO][1].data() == NULL ||
	strngrdRouteTable->Cells[START_NODE][1].data() == NULL ||
	strngrdRouteTable->Cells[END_NODE][1].data() == NULL ||
	strngrdRouteTable->Cells[ROUTE][1].data() == NULL )
	{
		Warnning_Display(COLOR_RED, "Please Route Request.");
		return;
	}

	if(m_nRowCount < m_nIndex)
	{
		Warnning_Display(COLOR_RED, "Please select cell included route");
		return;
	}

	if(	strngrdRouteTable->Cells[NO][m_nIndex].data() == NULL ||
	strngrdRouteTable->Cells[START_NODE][m_nIndex].data() == NULL ||
	strngrdRouteTable->Cells[END_NODE][m_nIndex].data() == NULL ||
	strngrdRouteTable->Cells[ROUTE][m_nIndex].data() == NULL||
	strngrdRouteTable->Cells[OFFSET][m_nIndex].data() == NULL )
	{
		Warnning_Display(COLOR_RED, "Please Route Request.");
		return;
	}

	int nCurID = gm_OHTStatus->m_OHTStatus.Data.CurNodeID;

	m_nStartIDFromIndex = nCurID;
	//에러상황일땐
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ERROR)
	{
		return;
	}

	//로직을 수행할 수 없는 상황일 때
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		Warnning_Display(COLOR_RED, "OHT Mode is not Manual Mode.........");
		return;
	}

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED)
   {
		Warnning_Display(COLOR_RED, "OHT Status have to be Initialized.......");
		 return;
   }


	if((gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_AMP_FAULT)	)
    {
        Warnning_Display(COLOR_RED, "Driving Axis1 Error.........");
        return;
	}

    if( (gm_OHTStatus->m_OHTStatus.Data.ErrorCode) &&
		((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right)))
	{
		Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
        return;
    }

    if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
		if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) > HOIST_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Hoist Not Origin...#1 can't Move...");
            return;
        }
		else if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) > SHIFT_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Shift Not Origin...#1 can't Move...");
            return;
        }
    }

    if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
    {
		if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                return;
            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                return;
            }
        }
        else
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
			{
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
                return;

            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == false)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
                return;
            }
		}
	}


	if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R != 1)
	{
		Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Tag");
		return;
	}

	// 모드에 맞는 마크상태인지
	// 처음 스타트 지점이 현재위치와 맞는지 체크
	switch (m_nMode)
	{
		case NODE_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Driving Tag");
				return;
			}

		break;

		case STATION_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag != 1
			|| gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L != 1
			||	gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R != 1 )
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Station/QR Tag");
				return;
			}
//
		break;

		case LINK_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Driving Tag");
				return;
			}
//
		break;

		default:
			Warnning_Display(COLOR_RED, "ERROR : You have to Select Mode");
			return;
		break;
	}
	m_nCurrentStep = REQ_MOVE_STEP;
	m_bSequenceMove = true;
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_AutoBayTest::btnMoveStartOneRouteClick(TObject *Sender)
{

	if(m_nSelectedRowNum < 0)
	{
		Warnning_Display(COLOR_RED, "Please select cell included route.");
		return;
    }


	m_nIndex = m_nSelectedRowNum;

	if(m_nCurrentStep != WAIT_STEP)
	{
		Warnning_Display(COLOR_RED, "Please Stop Sequence.");
		return;
    }

	if(m_bSequenceMove)
	{
		Warnning_Display(COLOR_RED, "Please Stop Sequence.");
		return;
	}

	if( m_nRowCount == 0 ||
	strngrdRouteTable->Cells[NO][1].data() == NULL ||
	strngrdRouteTable->Cells[START_NODE][1].data() == NULL ||
	strngrdRouteTable->Cells[END_NODE][1].data() == NULL ||
	strngrdRouteTable->Cells[ROUTE][1].data() == NULL ||
	strngrdRouteTable->Cells[OFFSET][1].data() == NULL)
	{
		Warnning_Display(COLOR_RED, "Please Route Request.");
		return;
	}

	if(m_nRowCount < m_nIndex)
	{
		Warnning_Display(COLOR_RED, "Please select cell included route");
		return;
	}

	if(	strngrdRouteTable->Cells[NO][m_nIndex].data() == NULL ||
	strngrdRouteTable->Cells[START_NODE][m_nIndex].data() == NULL ||
	strngrdRouteTable->Cells[END_NODE][m_nIndex].data() == NULL ||
	strngrdRouteTable->Cells[ROUTE][m_nIndex].data() == NULL ||
	strngrdRouteTable->Cells[OFFSET][m_nIndex].data() == NULL)
	{
		Warnning_Display(COLOR_RED, "Please Route Request.");
		return;
	}
	String strStartID = strngrdRouteTable->Cells[START_NODE][m_nIndex];
	int nStartID = strStartID.ToInt();

	//에러상황일땐
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ERROR)
	{
		return;
	}

	//로직을 수행할 수 없는 상황일 때
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		Warnning_Display(COLOR_RED, "OHT Mode is not Manual Mode.........");
		return;
	}

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED)
   {
		Warnning_Display(COLOR_RED, "OHT Status have to be Initialized.......");
		 return;
   }


	if((gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
		|| (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_AMP_FAULT)	)
    {
        Warnning_Display(COLOR_RED, "Driving Axis1 Error.........");
        return;
	}

    if( (gm_OHTStatus->m_OHTStatus.Data.ErrorCode) &&
		((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right)))
	{
		Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
        return;
    }

    if(CmdFoupOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
		if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) > HOIST_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Hoist Not Origin...#1 can't Move...");
            return;
        }
		else if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) > SHIFT_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Shift Not Origin...#1 can't Move...");
            return;
        }
    }

    if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
    {
		if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                return;
            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                return;
            }
        }
        else
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
			{
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
                return;

            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == false)
			{
				Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
                return;
            }
		}
	}


	if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L != 1 &&
		gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R != 1)
	{
		Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Tag");
		return;
	}

//	 모드에 맞는 마크상태인지
//	 처음 스타트 지점이 현재위치와 맞는지 체크
	switch (m_nMode)
	{
		case NODE_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Driving Tag");
				return;
			}
			if(gm_OHTStatus->m_OHTStatus.Data.CurNodeID != nStartID)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at StartID");
				return;
			}
		break;

		case STATION_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag != 1
			|| gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L != 1
			||	gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R != 1 )
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Station/QR Tag");
				return;
			}
			if(gm_OHTStatus->m_OHTStatus.Data.CurStationID !=nStartID)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at StartID");
				return;
			}
		break;

		case LINK_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Driving Tag");
				return;
			}
			if(gm_OHTStatus->m_OHTStatus.Data.CurNodeID != nStartID)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at StartID");
				return;
			}
		break;

		default:
			Warnning_Display(COLOR_RED, "ERROR : You have to Select Mode");
			return;
		break;

	}
	m_nCurrentStep = REQ_MOVE_STEP;
	m_bSequenceMove = false;
	m_nStartIDFromIndex = 0;
	m_dPreOffset = 0;
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_AutoBayTest::btnSequenceStopClick(TObject *Sender)
{
	m_bSequenceMove = false;
	mmoLongRunResult->Lines->Add("시퀀스 종료중...");
	lblTestStatus->Caption = "시퀀스 종료대기";
}
//---------------------------------------------------------------------------


void __fastcall Tfrm_AutoBayTest::FormClose(TObject *Sender, TCloseAction &Action)
{
	tmrAutoBayLongRunTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void Tfrm_AutoBayTest::Warnning_Display(TColor tWColor, String strData)
{
	MainForm->DataDisplay(tWColor, strData);
	WarnningForm->DisplayMemo->Clear();
	WarnningForm->DisplayMemo->Lines->Add(strData);
	WarnningForm->Show();
}

void __fastcall Tfrm_AutoBayTest::strngrdRouteTableSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect)
{
	m_nSelectedRowNum = ARow;
	m_nSelectedRowNum = ARow;
}

//---------------------------------------------------------------------------

void __fastcall Tfrm_AutoBayTest::btnRouteResetClick(TObject *Sender)
{
	//에러상황일땐
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ERROR)
	{
		Warnning_Display(COLOR_RED, "Please Error reset");
		return;
	}

	if(m_nSelectedRowNum < 0)
	{
		Warnning_Display(COLOR_RED, "Please select cell included route.");
		return;
    }

	if(m_nRowCount == 0)
	{
		Warnning_Display(COLOR_RED, "Please Route Request, Row Count 0!!");
		return;
    }
	m_nIndex = m_nSelectedRowNum;

	if(m_nCurrentStep != WAIT_STEP)
	{
		Warnning_Display(COLOR_RED, "Please Stop Sequence.");
		return;
    }

	if(m_bSequenceMove)
	{
		Warnning_Display(COLOR_RED, "Please Stop Sequence.");
		return;
	}

	if( m_nRowCount == 0 ||
	strngrdRouteTable->Cells[NO][1].data() == NULL ||
	strngrdRouteTable->Cells[START_NODE][1].data() == NULL ||
	strngrdRouteTable->Cells[END_NODE][1].data() == NULL ||
	strngrdRouteTable->Cells[ROUTE][1].data() == NULL ||
	strngrdRouteTable->Cells[OFFSET][1].data() == NULL)
	{
		Warnning_Display(COLOR_RED, "Please Route Request.");
		return;
	}

	if(m_nRowCount < m_nIndex)
	{
		Warnning_Display(COLOR_RED, "Please select cell included route");
		return;
	}

	if(	strngrdRouteTable->Cells[NO][m_nIndex].data() == NULL ||
	strngrdRouteTable->Cells[START_NODE][m_nIndex].data() == NULL ||
	strngrdRouteTable->Cells[END_NODE][m_nIndex].data() == NULL ||
	strngrdRouteTable->Cells[ROUTE][m_nIndex].data() == NULL  ||
	strngrdRouteTable->Cells[OFFSET][m_nIndex].data() == NULL)
	{
		Warnning_Display(COLOR_RED, "Please Route Request.");
		return;
	}


	int n = m_nIndex;
	int preRowCount = m_nRowCount;
	String TmpStrTable[5][500];
	memset(TmpStrTable, 0, sizeof(TmpStrTable));
	for(int i = 1; i<= preRowCount; i++)
	{
		if(n>m_nRowCount)
			n = 1;


		String strStartID = strngrdRouteTable->Cells[START_NODE][n].c_str();
		String strEndID = strngrdRouteTable->Cells[END_NODE][n].c_str();
		String strRoute = strngrdRouteTable->Cells[ROUTE][n].c_str();
		String strOffset = strngrdRouteTable->Cells[OFFSET][n].c_str();
		TmpStrTable[START_NODE][i] = strStartID;
		TmpStrTable[END_NODE][i] = strEndID;
		TmpStrTable[ROUTE][i] = strRoute;
		TmpStrTable[OFFSET][i] = strOffset;
		n++;
	}

	InitData();
	m_nRowCount = preRowCount;
	for(int i = 1; i<=preRowCount; i++)
	{
		strngrdRouteTable->Cells[NO][i] = IntToStr(i);
		strngrdRouteTable->Cells[START_NODE][i] = TmpStrTable[START_NODE][i];
		strngrdRouteTable->Cells[END_NODE][i]=	TmpStrTable[END_NODE][i];
		strngrdRouteTable->Cells[ROUTE][i] = TmpStrTable[ROUTE][i];
		strngrdRouteTable->Cells[OFFSET][i] = TmpStrTable[OFFSET][i];
	}





}
//---------------------------------------------------------------------------

void __fastcall Tfrm_AutoBayTest::btnEStopClick(TObject *Sender)
{
	TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_OHT_ESTOP);

    delete m_CMDControl;
}
//---------------------------------------------------------------------------


void __fastcall Tfrm_AutoBayTest::btnSimulationClick(TObject *Sender)
{

	frm_MapViewLoad->Show();
	frm_MapViewLoad->BringToFront();

}
//---------------------------------------------------------------------------

void __fastcall Tfrm_AutoBayTest::chkLongrunClick(TObject *Sender)
{
	if(chkLongrun->Checked == true)
	{
    	m_bLongrun = true;
	}
	else
	{
		m_bLongrun = false;
    }
}
//---------------------------------------------------------------------------


int Tfrm_AutoBayTest::getMode()
{
	return  m_nMode;
}
int Tfrm_AutoBayTest::getCurrentStep()
{
	return 	m_nCurrentStep;
}
int Tfrm_AutoBayTest::getRowSize()
{
	return	m_nRowCount;
}

bool Tfrm_AutoBayTest::getSequenceMove()
{
    return m_bSequenceMove;
}
//---------------------------------------------------------------------------

void __fastcall Tfrm_AutoBayTest::FormShow(TObject *Sender)
{
	tmrAutoBayLongRunTimer->Enabled = true;
}
//---------------------------------------------------------------------------

