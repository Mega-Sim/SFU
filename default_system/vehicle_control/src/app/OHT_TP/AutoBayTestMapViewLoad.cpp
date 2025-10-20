//---------------------------------------------------------------------------

#include <vcl.h>
#include <tlhelp32.h>
#include <CString.h>

#pragma hdrstop

#include "winbase.h"
#include "AutoBayTestMapViewLoad.h"
#include "AutoBayTestUnit.h"
#include "MainUnit.h"
#include "ConnectUnit.h"
#include "SocketUnit.h"
#include "UDP.h"
#include "DataTransperReq.h"
#include "WarnningUnit.h"
#include "MainUnit.h"
#include "OHTInfoUnit.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

Tfrm_MapViewLoad *frm_MapViewLoad;
String MapViewStatus[4] ={"No Response", "Running", "Loading..", "Completed"};

// 1. No Response : 맵뷰 응답없음
// 2. Running : 맵뷰 실행중
// 3. Loading : 맵 로딩중
// 4. Completed : 맵 로드 완료 -> 시뮬레이션 가능

//---------------------------------------------------------------------------
__fastcall Tfrm_MapViewLoad::Tfrm_MapViewLoad(TComponent* Owner)
	: TForm(Owner)
{
	m_nUIUpdateStatus = 0;
	m_nUIUpdateResult = 0;

	bFirst = false;
	strTmpBuffer =NULL;
	tok = NULL;
	//Simulation용 변수 초기화
	m_bSequenceMove = false;
	m_bSimulation = false;
	m_nIndex = 0;
	m_nMode = 0;
	m_nRowSize = 0;
	m_nCurrentStep = 0;

	//MapView Status, Version 초기화
	m_nMapViewStatus = 0;
	memset(mapVersion, 0, sizeof(mapVersion));

	//Timer Enable
	tmrUDPTimerMapview->Enabled = false;

	//Bind
	frmUDP->pUdpObject->TP_MAPVIEW_Bind();
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_MapViewLoad::tmrUDPTimerMapviewTimer(TObject *Sender)
{
	static int cnt = 0;
	//Listen
	int nResult = frmUDP->pUdpObject->MAPVIEW_TP_ThreadCall();
	if(nResult == -1)
	{
    	cnt++;
	}
	else
	{
        cnt = 0;
	}

	if(cnt >= 5)
	{
		m_nMapViewStatus = 0;
		cnt = 0;
	}

	//Status Request
	String Header = "TP";
	int TPPortNum = TP_FIX_PORT_NUM_FOR_MAPVIEW;
	AnsiString message =  Header + "/"	+ IntToStr(TagID::CMD_TP_TO_MAPVIEW_STATUS_REQ)+ "/" + IntToStr(TPPortNum);
	int iReturn = frmUDP->UnicastSend_toMapView(message);

	String tmpVersion = mapVersion;
	edtStatus->Text =  MapViewStatus[m_nMapViewStatus].c_str();
	edtMapVersion->Text = tmpVersion.c_str();

	static int nPreStatus =  m_nMapViewStatus;

	//상태 체크
	switch(m_nMapViewStatus)
	{
		//No response
		case 0:
			m_bSimulation = false;
			break;

		//Running
		case 1:
			break;

		//Map Loading..
		case 2:
			break;

		//Map Load complete... simulation Check;
		case 3:

			//최초 맵실행이 완료되면 최초 한 번만 맵 버전 요청
			if(nPreStatus !=  m_nMapViewStatus)
			{
				AnsiString message =
				 Header + "/" + IntToStr(TagID::CMD_TP_TO_MAPVIEW_MAP_VERSION_REQ)+ "/" + IntToStr(TPPortNum);
				iReturn = frmUDP->UnicastSend_toMapView(message);
			}


			break;

	}

	//시뮬레이션
	if(m_bSimulation)
	{
    	excuteSimulation();
	}
	else
	{
		m_nUIUpdateStatus = 0;
		bFirst = false;
		bEnd = false;
		m_nIndex = 1;
	}
	nPreStatus =  m_nMapViewStatus;
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_MapViewLoad::FormClose(TObject *Sender, TCloseAction &Action)

{
	tmrUDPTimerMapview->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_MapViewLoad::btnMapViewLoadClick(TObject *Sender)
{
	AnsiString sExePath = ExtractFilePath(Application->ExeName);
	AnsiString sMapView = sExePath + "MapView.exe";

	String sFileName = "MapView.exe";
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
			Warnning_Display(COLOR_RED, "MapView Have to be closed");
			return;
		}
	}

	if(!FileExists(sMapView.c_str()))
	{
		Warnning_Display(COLOR_RED, "MapView File is Not exist in the folder");
		return;
    }

	int nResult = WinExec(sMapView.c_str(), SW_SHOW);
	tmrUDPTimerMapview->Enabled = true;


}
//---------------------------------------------------------------------------
void Tfrm_MapViewLoad::Warnning_Display(TColor tWColor, String strData)
{
	MainForm->DataDisplay(tWColor, strData);
	WarnningForm->DisplayMemo->Clear();
	WarnningForm->DisplayMemo->Lines->Add(strData);
	WarnningForm->Show();
}

void __fastcall Tfrm_MapViewLoad::FormShow(TObject *Sender)
{
	tmrUDPTimerMapview->Enabled = true;
}
//---------------------------------------------------------------------------
void Tfrm_MapViewLoad::setMapViewStatus(int nStatus)
{
    m_nMapViewStatus = nStatus;
}

void Tfrm_MapViewLoad::setMapVersion(char* buf)
{
	int nSize = sizeof(mapVersion);

	try
	{
		memcpy(mapVersion, buf, sizeof(mapVersion));
	}
	catch(...)
	{
		tmrUDPTimerMapview->Enabled = false;
	}

}

void __fastcall Tfrm_MapViewLoad::btnSimulationClick(TObject *Sender)
{
	//시뮬레이션이 진행중인 경우 초기화 요청 및 중지
	if(m_bSimulation)
	{
		m_bSimulation = false;
		return;
	}

	m_nRowSize = frm_AutoBayTest->getRowSize();
	m_nCurrentStep = frm_AutoBayTest->getCurrentStep();
	m_bSequenceMove = frm_AutoBayTest->getSequenceMove();

	if(m_nMapViewStatus != 3)
	{
		Warnning_Display(COLOR_RED, "Please Load Map.");
		m_bSimulation = false;
		bFirst = false;
		bEnd = false;	
		return;
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

	if( m_nRowSize == 0 ||
	frm_AutoBayTest->strngrdRouteTable->Cells[NO][1].data() == NULL ||
	frm_AutoBayTest->strngrdRouteTable->Cells[START_NODE][1].data() == NULL ||
	frm_AutoBayTest->strngrdRouteTable->Cells[END_NODE][1].data() == NULL ||
	frm_AutoBayTest->strngrdRouteTable->Cells[ROUTE][1].data() == NULL ||
	frm_AutoBayTest->strngrdRouteTable->Cells[OFFSET][1].data() == NULL )
	{
		Warnning_Display(COLOR_RED, "Please Route Request.");
		return;
	}
	String Header = "TP";
	AnsiString message =  Header + "/" + IntToStr(TagID::CMD_TP_TO_MAPVIEW_UI_UPDATE_REQ)+ "/" + IntToStr(TP_FIX_PORT_NUM_FOR_MAPVIEW)+ "/0";
	frmUDP->UnicastSend_toMapView(message);

	m_bSimulation = true;
	m_nIndex = 1;
	bFirst = true;

}
//---------------------------------------------------------------------------
void Tfrm_MapViewLoad::setUIUpdateResult(int nStatus, int nResult)
{
	m_nUIUpdateStatus = nStatus;
	m_nUIUpdateResult = nResult;
}

void Tfrm_MapViewLoad::excuteSimulation()
{
	AnsiString tmpLeftStr ="";
	String Header = "TP";

//	if(m_nUIUpdateStatus == 1)
//	{
//		return;
//	}
//	else if(m_nUIUpdateStatus == 2)
//	{
//		m_nUIUpdateStatus  = 0;
//	}

	if(m_nMapViewStatus != 3)
	{
		Warnning_Display(COLOR_RED, "Please Load Map.");
		m_bSimulation = false;
		return;
	}

	if(m_nCurrentStep != WAIT_STEP)
	{
		m_nUIUpdateStatus  = 0;
		Warnning_Display(COLOR_RED, "Please Stop Sequence.");
		m_bSimulation = false;
		return;
	}

	if(m_bSequenceMove)
	{
		m_nUIUpdateStatus  = 0;
		Warnning_Display(COLOR_RED, "Please Stop Sequence.");
		m_bSimulation = false;
		return;
	}


	//에러상황일땐
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ERROR)
	{
		m_nUIUpdateStatus  = 0;
		Warnning_Display(COLOR_RED, "Please Reset Error Status");
		m_bSimulation = false;
		return;
	}

	//로직을 수행할 수 없는 상황일 때
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		m_nUIUpdateStatus  = 0;
		Warnning_Display(COLOR_RED, "OHT Mode is not Manual Mode.........");
		m_bSimulation = false;
		return;
	}

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE &&
	   gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED)
   {
		m_nUIUpdateStatus  = 0;
		Warnning_Display(COLOR_RED, "OHT Status have to be Initialized.......");
		return;
   }

	if( m_nRowSize == 0 ||
	frm_AutoBayTest->strngrdRouteTable->Cells[NO][1].data() == NULL ||
	frm_AutoBayTest->strngrdRouteTable->Cells[START_NODE][1].data() == NULL ||
	frm_AutoBayTest->strngrdRouteTable->Cells[END_NODE][1].data() == NULL ||
	frm_AutoBayTest->strngrdRouteTable->Cells[ROUTE][1].data() == NULL ||
	frm_AutoBayTest->strngrdRouteTable->Cells[OFFSET][1].data() == NULL )
	{
		Warnning_Display(COLOR_RED, "Please Route Request.");
		m_bSimulation = false;
		return;
	}

	if(m_nIndex > m_nRowSize)
	{
		Warnning_Display(COLOR_RED, "Simulation ends");
		m_bSimulation = false;
		bFirst = false;
		bEnd = false;
		return;
	}


	if(bFirst)
	{
		bFirst = false;
		strTmpBuffer = frm_AutoBayTest->strngrdRouteTable->Cells[ROUTE][m_nIndex].c_str();

		if (strTmpBuffer.Length() == 0 || strTmpBuffer.Length() > 1023)
		{
			Warnning_Display(COLOR_RED, "Route is Abnormal");
			m_bSimulation = false;
			return;
		}
		nPos = strTmpBuffer.Pos("-");
		SplitStr = strTmpBuffer.SubString(0,nPos-1);
		strTmpBuffer.Delete(1, nPos);
	}

	if(bEnd)
	{
		bEnd = false;      		
		AnsiString message = Header + "/" + IntToStr(TagID::CMD_TP_TO_MAPVIEW_UI_UPDATE_REQ)+ "/"
				 + IntToStr(TP_FIX_PORT_NUM_FOR_MAPVIEW)+ "/2_"+ SplitStr;    				 
		frmUDP->UnicastSend_toMapView(message);
		SplitStr ="";
		m_nUIUpdateStatus  = 1;
		m_nIndex++;
		bFirst = true;
        return;
	}

	nPos = strTmpBuffer.Pos("-");

	if(nPos != 0)
	{
		tmpLeftStr = strTmpBuffer.SubString(0, nPos-1);
		tmpLeftStr = SplitStr + "_" + tmpLeftStr;
		SplitStr = strTmpBuffer.SubString(0, nPos-1);
		strTmpBuffer.Delete(1, nPos);
		AnsiString message = Header + "/" + IntToStr(TagID::CMD_TP_TO_MAPVIEW_UI_UPDATE_REQ)+ "/"
				 + IntToStr(TP_FIX_PORT_NUM_FOR_MAPVIEW)+ "/1_"+ tmpLeftStr;
		frmUDP->UnicastSend_toMapView(message);
	}
	else
	{
		tmpLeftStr = strTmpBuffer.SubString(0, strTmpBuffer.Length());
		tmpLeftStr = SplitStr +"_"+tmpLeftStr;
		SplitStr = strTmpBuffer.SubString(0, strTmpBuffer.Length());
		AnsiString message = Header + "/" + IntToStr(TagID::CMD_TP_TO_MAPVIEW_UI_UPDATE_REQ)+ "/"
				 + IntToStr(TP_FIX_PORT_NUM_FOR_MAPVIEW)+ "/1_"+ tmpLeftStr;
		frmUDP->UnicastSend_toMapView(message);
		bEnd = true;	
	}
}
