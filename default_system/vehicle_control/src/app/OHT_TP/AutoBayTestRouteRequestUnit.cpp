//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OHTInfoUnit.h"
#include "WarnningUnit.h"
#include "MainUnit.h"
#include "AutoBayTestRouteRequestUnit.h"
#include "AutoBayTestUnit.h"
#include "chkExecUnit.h"
#include "UDP.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tfrm_RouteRequest *frm_RouteRequest;
TCHKOHT CmdFoupOHTCheck;
//---------------------------------------------------------------------------
__fastcall Tfrm_RouteRequest::Tfrm_RouteRequest(TComponent* Owner)
	: TForm(Owner)
{
	strngrdLaneCutList->Cells[0][0] = "FromNode";
	strngrdLaneCutList->Cells[1][0] = "ToNode";

	for(int i = 1; i<20; i++)
	{
		strngrdLaneCutList->Cells[0][i] = "";
		strngrdLaneCutList->Cells[1][i] = "";
	}

	m_nMode = NONE_MODE;

}
//---------------------------------------------------------------------------
void __fastcall Tfrm_RouteRequest::btnRouteRequestClick(TObject *Sender)
{
	int iPacketSize;
	int iReturn;

	AnsiString strMachineID = "TP001";
//
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

	//마크상태인지 점검
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
		break;

		case LINK_MODE:
			if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag != 1)
			{
				Warnning_Display(COLOR_RED, "ERROR : You have to Mark at Driving Tag");
				return;
			}
		break;

		default:
			Warnning_Display(COLOR_RED, "ERROR : You have to Select Mode");
			return;
		break;
	}

	if(m_nMode == NONE_MODE)
	{
		Warnning_Display(COLOR_RED, "Select Mode!");
		return;
    }

	frm_AutoBayTest->setMode(m_nMode);

	// 새로운 TP 이동명령 패킷을 이용한다.
	iPacketSize = sizeof(PKT_TP2OHT_AUTO_DRIVING_TEST_ROUTE_REQ);
	PKT_TP2OHT_AUTO_DRIVING_TEST_ROUTE_REQ	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::CMD_AUTO_DRIVING_TEST_ROUTE_REQ_TP2OHT;
	 // TP에서 OHT로 이동명령 (목적지만 지정하고 MAP정보 사용)

	 // ---- Body Set

	int nLinkCnt = edtLinkCount->Text.ToInt();
	if( nLinkCnt > 5)
	{
		Warnning_Display(COLOR_RED, "ERROR : Link Count > 5!");
		return;
	}
	else if (nLinkCnt == 0 )
	{
		Warnning_Display(COLOR_RED, "ERROR : Link Count = 0!");
		return;
	}

	SEND_PACKET.BODY.Mode =	m_nMode;
	SEND_PACKET.BODY.LinkCount = nLinkCnt;

	if(m_nMode == NODE_MODE || m_nMode == LINK_MODE)
		SEND_PACKET.BODY.StartID = gm_OHTStatus->m_OHTStatus.Data.CurNodeID;
	else if (m_nMode == STATION_MODE)
		SEND_PACKET.BODY.StartID = gm_OHTStatus->m_OHTStatus.Data.CurStationID;

	for(int i = 1; i<21; i++)
	{
		if(strngrdLaneCutList->Cells[0][i].data()== NULL && strngrdLaneCutList->Cells[1][i].data() == NULL)
			continue;

		else if(strngrdLaneCutList->Cells[0][i].data()== NULL)
		{
			Warnning_Display(COLOR_RED, "ERROR : Data Abnomral!");
			return;
		}
		else if(strngrdLaneCutList->Cells[1][i].data()== NULL)
		{
			Warnning_Display(COLOR_RED, "ERROR : Data Abnomral!");
			return;
		}

		AnsiString Cell1 = strngrdLaneCutList->Cells[0][i].c_str();
		AnsiString Cell2 = strngrdLaneCutList->Cells[1][i].c_str();

		char tmpStr[7] = {0,};
		strcpy(tmpStr, Cell1.c_str());

		for(int j = 0; j<6; j++)
		{
			if(tmpStr[j] == ' ')
			{
				if(j == 0)
				{
					Warnning_Display(COLOR_RED, "ERROR : Data Abnomral!");
					return;
				}
				Cell1 = Cell1.SubString(0,j);
				break;
			}
		}
		memset(tmpStr, 0, sizeof(tmpStr));
		strcpy(tmpStr, Cell2.c_str());
		for(int j = 0; j<6; j++)
		{
			if(tmpStr[j] == ' ')
			{
				if(j == 0)
				{
					Warnning_Display(COLOR_RED, "ERROR : Data Abnomral!");
					return;
				}
				Cell2 = Cell2.SubString(0,j);
				break;
			}
		}


		SEND_PACKET.BODY.LaneCutList[i-1][0] =  Cell1.ToInt();
		SEND_PACKET.BODY.LaneCutList[i-1][1] =  Cell2.ToInt();

	}

	// 데이터 전송
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
	else
		frm_AutoBayTest->setStep(ROUTE_STEP);

	frm_AutoBayTest->tmrAutoBayLongRunTimer->Enabled = true;
	this->Close();
}

void __fastcall Tfrm_RouteRequest::cbbModeSelectClick(TObject *Sender)
{
	switch(cbbModeSelect->ItemIndex)
	{
		case 0 :
			m_nMode = NODE_MODE;
			break;

		case 1 :
			m_nMode = STATION_MODE;
			break;

		case 2:
			m_nMode = LINK_MODE;
			break;

		default:
			m_nMode = NONE_MODE;
			break;
	}
}


//---------------------------------------------------------------------------
void __fastcall Tfrm_RouteRequest::btnInitializationClick(TObject *Sender)
{
	for(int i = 1; i<21; i++)
	{
		strngrdLaneCutList->Cells[0][i] = "";
		strngrdLaneCutList->Cells[1][i] = "";
	}
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_RouteRequest::strngrdLaneCutListKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift)
{
	if(Key < '0' || Key > '9'|| Key == ' ')
		Key == 0;
}
//---------------------------------------------------------------------------

void __fastcall Tfrm_RouteRequest::strngrdLaneCutListGetEditMask(TObject *Sender,
          int ACol, int ARow, UnicodeString &Value)
{
	Value = "000000";
}
//---------------------------------------------------------------------------

void __fastcall Tfrm_RouteRequest::btnFileLoadClick(TObject *Sender)
{
	bool nSuccess = 0;
	for(int i = 1; i<21; i++)
	{
		strngrdLaneCutList->Cells[0][i] = "";
		strngrdLaneCutList->Cells[1][i] = "";
	}

	if(dlgOpenTextFileDig_LaneCut->Execute())
	{
		String strFileName = dlgOpenTextFileDig_LaneCut->FileName;
		int nPos = strFileName.Pos(".txt");
		// File Check
		if(nPos!=0)
			nSuccess = LaneCutListFileLoad(strFileName);
	}
}


bool Tfrm_RouteRequest::LaneCutListFileLoad(String strTmpFileName)
{
	TCHAR chBuffer[256];
    String strTmpBuffer;
	int Pos = 0;
	int nCount = 0;

	if(strTmpFileName.Length() <= 0) return false;

	FILE* fp = _wfopen(strTmpFileName.w_str(), L"r");

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
            fgetws(chBuffer, 255, fp);
        }
		catch (Exception &e)
        {
            fclose(fp);
            return false;
		}
		strTmpBuffer = chBuffer;
        strTmpBuffer.TrimRight();
        while ((Pos = strTmpBuffer.Pos(L"\n")) > 0)
			strTmpBuffer.Delete(Pos, 1);

		if (strTmpBuffer.Length() == 0)  continue;


		Pos=strTmpBuffer.Pos("_");

		for(int j = 0; j<strTmpBuffer.Length(); j++)
		{
			if(j != Pos-1)
			{
				if(chBuffer[j]<'0' || chBuffer[j]>'9')
				{
					Warnning_Display(COLOR_RED, "Have To be numerical");
					return false;
                }


			}
		}

		if(Pos>0)
        {
			String tmpLeftStr  = strTmpBuffer.SubString(1,Pos-1);
			String tmpRightStr = strTmpBuffer.SubString(Pos+1, strTmpBuffer.Length());

           	nCount++;
			if(nCount>20)
			{
				for(int i = 1; i<21; i++)
				{
					strngrdLaneCutList->Cells[0][i] = "";
					strngrdLaneCutList->Cells[1][i] = "";

				}
				Warnning_Display(COLOR_RED, "LaneCut Count Limit Over");
				return false;
			}
			strngrdLaneCutList->Cells[0][nCount]= tmpLeftStr;
			strngrdLaneCutList->Cells[1][nCount]= tmpRightStr;

		}
	}
	fclose(fp);

}
//---------------------------------------------------------------------------

void Tfrm_RouteRequest::Warnning_Display(TColor tWColor, String strData)
{
    MainForm->DataDisplay(tWColor, strData);
    WarnningForm->DisplayMemo->Clear();
	WarnningForm->DisplayMemo->Lines->Add(strData);
    WarnningForm->Show();
}
