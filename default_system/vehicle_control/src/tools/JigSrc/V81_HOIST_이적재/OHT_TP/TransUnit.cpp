//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TransUnit.h"
#include "CMDControlUnit.h"
#include "OHTInfoUnit.h"
#include "Def_TPCommand.h"
#include "Define.h"
#include "ChkExecUnit.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTransForm *TransForm;

//---------------------------------------------------------------------------
__fastcall TTransForm::TTransForm(TComponent* Owner)
    : TForm(Owner)
{
    m_TransCmdData = 0x00;
    m_PortType     = 0x00;

    m_strTransCmdType = "None";
    m_strPortType     = "None";

    tmUpdateUI->Interval = 500;
    tmUpdateUI->Enabled  = false;

//    chkEQPIODisable->Checked = false;   // Default 값은 false
}
//---------------------------------------------------------------------------

void __fastcall TTransForm::FormShow(TObject *Sender)
{
    // Form이 Show되면 UI Update 시작
    tmUpdateUI->Enabled = true;

    // 초기 설정 값
    cbbPIODirection->ItemIndex = 0;
    cbbHoistSpeedLevel->ItemIndex = 9;
    cbbShiftSpeedLevel->ItemIndex = 9;

//    chkEQPIODisable->Checked = false;
    chkHandDetectEQ->Checked = false;

    // by zzang9un 2014.03.12 : STB의 경우 EQPIO, HandDetectEQ UI 숨김
    switch(m_PortType)
    {
    case PORT_TYPE_STB_L:
    case PORT_TYPE_STB_R:
    case PORT_TYPE_UTB:    
//        chkEQPIODisable->Enabled = false;
//        chkEQPIODisable->Checked = false;

        chkHandDetectEQ->Enabled = false;
        chkHandDetectEQ->Checked = false;
        break;  

    default:
//        chkEQPIODisable->Enabled = true;
        chkHandDetectEQ->Enabled = true;
    }

    UpdateUI();
}
//---------------------------------------------------------------------------

void __fastcall TTransForm::FormCreate(TObject *Sender)
{
    InitGridDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TTransForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    tmUpdateUI->Enabled = false;    
}
//---------------------------------------------------------------------------

/**
@brief   StringGrid를 초기화하는 함수
@author  zzang9un
@date    2012.12.01
*/
void TTransForm::InitGridDisplay()
{
    // Step 1. Caption UI를 Update한다.
    panTransType->Caption = "[" + m_strPortType + "] " + m_strTransCmdType;
    
    for(int i = 0; i <= 8; i++)
    {
        strGridTransSetting->Cells[0][i] = IntToStr(i+1);
    }

    //*** Check ON/OFF ***
    int nCol = 1;
    int nRow = 0;
    strGridTransSetting->Cells[nCol][nRow++] = "Use Shutter Control";
    strGridTransSetting->Cells[nCol][nRow++] = "Use HandLimitSensor";
    strGridTransSetting->Cells[nCol][nRow++] = "Use HandControl";
    strGridTransSetting->Cells[nCol][nRow++] = "Use LookDownSensor";
    strGridTransSetting->Cells[nCol][nRow++] = "Use GripperSensor";
    strGridTransSetting->Cells[nCol][nRow++] = "Use HoistAlignSensor";
    strGridTransSetting->Cells[nCol][nRow++] = "Use Oscillation Sen";
    strGridTransSetting->Cells[nCol][nRow++] = "Use EQ-PIO";
}
//---------------------------------------------------------------------------

/**
@brief   TransForm의 UI를 Update하는 함수
@author  zzang9un
@date    2012.12.01
*/
void TTransForm::UpdateUI()
{
    // Step 1. Caption UI를 Update한다.
    panTransType->Caption = "[" + m_strPortType + "] " + m_strTransCmdType;

    // Step 2. StringGrid를 Update한다.
    int nCol = 2;
    int nRow = 0;

    strGridTransSetting->Cells[nCol][nRow++] = (gm_OHTStatus->CheckOnOff->bUse_ShutterControl == true) ? "ON" : "OFF";
    strGridTransSetting->Cells[nCol][nRow++] = (gm_OHTStatus->CheckOnOff->bUse_HandLimitSensorCheck == true) ? "ON" : "OFF";
    strGridTransSetting->Cells[nCol][nRow++] = (gm_OHTStatus->CheckOnOff->bUse_HandControl == true) ? "ON" : "OFF";
    strGridTransSetting->Cells[nCol][nRow++] = (gm_OHTStatus->CheckOnOff->bUse_LookDownSensor == true) ? "ON" : "OFF";
    strGridTransSetting->Cells[nCol][nRow++] = (gm_OHTStatus->CheckOnOff->bUse_GripperSen == true) ? "ON" : "OFF";
    strGridTransSetting->Cells[nCol][nRow++] = (gm_OHTStatus->CheckOnOff->bUse_HoistAlignSen == true) ? "ON" : "OFF";
    strGridTransSetting->Cells[nCol][nRow++] = (gm_OHTStatus->CheckOnOff->bUse_OscillationSen == true) ? "ON" : "OFF";
    strGridTransSetting->Cells[nCol][nRow++] = (gm_OHTStatus->CheckOnOff->bUse_EQPIO == true) ? "ON" : "OFF";
}
//---------------------------------------------------------------------------

/**
@brief   StringGrid 이벤트 발생 시 호출되는 함
@author  zzang9un
@date    2012.12.01
*/
void __fastcall TTransForm::strGridTransSettingDrawCell(TObject *Sender,
      int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
    int i;
    String strText;
    if (State.Contains(gdFixed))
    {
        strGridTransSetting->Canvas->Brush->Color = clBtnFace;
        strGridTransSetting->Canvas->Font->Color = clWindowText;
        strGridTransSetting->Canvas->FillRect(Rect);
        Frame3D(strGridTransSetting->Canvas, Rect, clBtnHighlight, clBtnShadow, 1);
    }
    else if(ACol == 2)
    {
        if(strGridTransSetting->Cells[ACol][ARow] == "OFF")
        {
            strGridTransSetting->Canvas->Brush->Color = (TColor)0x00C080FF;
            strGridTransSetting->Canvas->FillRect(Rect);
        }
        else
        {
            strGridTransSetting->Canvas->Brush->Color = COLOR_WHITE;
            strGridTransSetting->Canvas->FillRect(Rect);
        }
    }
    else
    {
        strGridTransSetting->Canvas->Brush->Color = COLOR_WHITE;
        strGridTransSetting->Canvas->FillRect(Rect);
    }
    
    strText = strGridTransSetting->Cells[ACol][ARow];
    strGridTransSetting->Canvas->Font->Color = COLOR_BLACK;
    strGridTransSetting->Canvas->TextRect(Rect, Rect.Left, Rect.Top, strText);
}
//---------------------------------------------------------------------------

/**
@brief   이적재 정보를 설정하는 함수
@author  zzang9un
@date    2012.12.01
@param   TransCmdData : 이적재 명령 Type
@param   porttype : 이적재할 Port Type
*/
void TTransForm::SetTrans(BYTE TransCmdData, BYTE porttype)
{
    m_TransCmdData = TransCmdData;
    m_PortType = porttype;

    // 이적재 명령 판단
    switch(m_TransCmdData)
    {
    case CMD_LOAD:
        m_strTransCmdType = "Load";
        break;

    case CMD_UNLOAD:
        m_strTransCmdType = "Unload";
        break;

    // by zzang9un 2014.04.07 : Move Teaching Point 명령 추가
    case CMD_MOVETPOINT:
        m_strTransCmdType = "Move Teaching Point";

    default:
        break;
    }

    // Port Type 판단
    m_strPortType = PortTypeToStr(porttype);
    
}
//---------------------------------------------------------------------------
void __fastcall TTransForm::strGridTransSettingClick(TObject *Sender)
{
    /* // [BEGIN] by zzang9un 2013.08.18 : 설정 불가능하도록 임시 조치함
    // On, Off column을 선택한 경우에만 Popup이 뜨도록 설정
    if(strGridTransSetting->Col != 2)
        popMenuOnOff->AutoPopup=false;
    else
        popMenuOnOff->AutoPopup=true;
    */ // [END] by zzang9un 2013.08.18 : 설정 불가능하도록 임시 조치함
}
//---------------------------------------------------------------------------
void __fastcall TTransForm::menuItemONClick(TObject *Sender)
{
    if(TOHTCheck.Is_Executable() == NO_ERR)
        ChangeUseControl(strGridTransSetting->Row+1, ON);
}
//---------------------------------------------------------------------------
void __fastcall TTransForm::menuItemOFFClick(TObject *Sender)
{
    if(TOHTCheck.Is_Executable() == NO_ERR)
        ChangeUseControl(strGridTransSetting->Row+1, OFF);        
}
//---------------------------------------------------------------------------

/**
@brief   UseControl Data를 변경하도록 하는 함수
@author  zzang9un
@date    2012.12.02
@param   UseNum : 변경할 Index
@param   OnOff : On ro Off
*/
void TTransForm::ChangeUseControl(int UseNum, bool OnOff)
{
    TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

    uTP_CMD pSndCmd; // 전송해야 할 Command를 담을 구조체

    // 구조체 초기화
    memset(&pSndCmd, 0x00, sizeof(uTP_CMD));

    // On, Off 값만 구조체 iData1에 적용
    pSndCmd.TPCmd.Data1 = OnOff;
    
    /* // [BEGIN] by zzang9un 2013.08.18 : 기존 소스 임시 주석
    switch(UseNum)
    {
    case 1:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_SHUTTERCONTROL, &pSndCmd);
        break;

    case 2:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_HANDLIMITSENSORCHK, &pSndCmd);
        break;

    case 3:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_HANDCONTROL, &pSndCmd);
        break;

    case 4:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_LOOKDOWNSENSOR, &pSndCmd);
        break;

    case 5:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_GRIPPERSEN, &pSndCmd);
        break;

    case 6:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_HOISTALIGNSEN, &pSndCmd);
        break;

    case 7:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_OSCILLATIONSEN, &pSndCmd);
        break;

    case 8:
        m_CMDControl->SendTPCmd2OHT(TPCMD_MAP_USE_EQPIO, &pSndCmd);
        break;

    default:
        break;

    }
    */ // [END] by zzang9un 2013.08.18 : 기존 소스 임시 주석

    delete m_CMDControl;    
}
//---------------------------------------------------------------------------

void __fastcall TTransForm::tmUpdateUITimer(TObject *Sender)
{
    UpdateUI();    
}
//---------------------------------------------------------------------------

void __fastcall TTransForm::btnCancelTransClick(TObject *Sender)
{
    Close();    
}
//---------------------------------------------------------------------------

void __fastcall TTransForm::btnDoTransClick(TObject *Sender)
{

    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	
#if 0 	// 예전 코드
    // 이적재 명령 구조체
    TRANS_CMD SndTransCmd;

    // 구조체 초기화
    memset(&SndTransCmd, 0x00, sizeof(TRANS_CMD));

    // 이적재 명령 셋팅
    MakeTransCmd(&SndTransCmd);

    // Load, Unload 구분
    switch(m_TransCmdData)
    {
    case CMD_LOAD:
        // 이적재 명령 전송
        m_CMDControl->SendTransCmd2OHT(TPCMD_TRANS_LOAD, &SndTransCmd);
        break;

    case CMD_UNLOAD:
        // 이적재 명령 전송
        m_CMDControl->SendTransCmd2OHT(TPCMD_TRANS_UNLOAD, &SndTransCmd);
        break;

    // by zzang9un 2014.04.07 : Move Teaching Point 명령 추가
    case CMD_MOVETPOINT:
        m_CMDControl->SendTransCmd2OHT(TPCMD_MOVE_TEACHPOINT, &SndTransCmd);
        break;

    default:
        break;
    }
#else
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";

	// 이적재 명령 구조체
	iPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
	PKT_TP2OHT_TRANS_LOAD_UNLOAD	SEND_PACKET;

	// 구조체 초기화
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// 이적재 명령 셋팅
	MakeTransPkt(&SEND_PACKET);

	// ---- Tag
    // Load, Unload 구분
    switch(m_TransCmdData)
    {
	    case CMD_LOAD:
	        // 이적재 명령 전송
	        SEND_PACKET.TAG.Value = TagID::CMD_TRANS_LOAD;
	        m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	        break;

	    case CMD_UNLOAD:
	        // 이적재 명령 전송
	        SEND_PACKET.TAG.Value = TagID::CMD_TRANS_UNLOAD;
	        m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	        break;

	    // by zzang9un 2014.04.07 : Move Teaching Point 명령 추가
	    case CMD_MOVETPOINT:
	        SEND_PACKET.TAG.Value = TagID::CMD_TRANS_MOVE_TEACHING_POINT;
	        m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	        break;

	    default:
	        break;
    }
#endif

    // by zzang9un 2013.02.05 : 이적재 롱런을 위해 현재 보낸 명령을 저장해둠
    gm_OHTStatus->PreTransCmd = m_TransCmdData;
    gm_OHTStatus->PrePortType = m_PortType;

	// by zzang9un 2013.02.05 : 이적재 롱런을 위해 Timer를 true또는 false로 변경
	if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP) ||(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP_S))
	{
		if(FoupOnlineCMDForm->chkLongRun->Checked == true)
		{
			FoupOnlineCMDForm->timerTransLongRun->Enabled = true;
			FoupOnlineCMDForm->timerTransLongRun->Interval = FoupOnlineCMDForm->edtLongRunInterval->Text.ToInt();
		}
		else
			FoupOnlineCMDForm->timerTransLongRun->Enabled = false;
	}
    else if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
    {
		if(EUVOnlineCMDForm->chkLongRun->Checked == true)
		{
			EUVOnlineCMDForm->timerTransLongRun->Enabled = true;
			EUVOnlineCMDForm->timerTransLongRun->Interval = EUVOnlineCMDForm->edtLongRunInterval->Text.ToInt();
		}
		else
			EUVOnlineCMDForm->timerTransLongRun->Enabled = false;
    }


    delete m_CMDControl;
    Close();
}
//---------------------------------------------------------------------------

/**
@brief   Manual 이적재 Command를 생성하는 함수
@author  zzang9un
@date    2012.12.08
@param   pTransCmd : 이적재 명령 구조체
@return  정상인 경우 0, Teaching Data Data를 못 찾은 경우 1을 리턴
*/
void TTransForm::MakeTransCmd(TRANS_CMD *pTransCmd)
{
    pTransCmd->CurNodeID = 123;
	pTransCmd->CurStationID = 456;

    // by zzang9un 2013.09.02 : Hoist, Shift Level을 계산하여 전달
	pTransCmd->HoistSpeedLevel = 100;
	pTransCmd->ShiftSpeedLevel = 100;

    // by zzang9un 2013.08.26 : EQPIO Disable 여부를 추가
    pTransCmd->TransOption.Info.AutoRecovery = false;
    pTransCmd->TransOption.Info.HandDetectEQ = false;
//    pTransCmd->TransOption.Info.EQPIODisable_TP = chkEQPIODisable->Checked;
}
//---------------------------------------------------------------------------


void TTransForm::MakeTransPkt(PKT_TP2OHT_TRANS_LOAD_UNLOAD *pTrans)
{
	pTrans->BODY.CurNodeID = 123;
	pTrans->BODY.CurStationID = 456;


//	pTrans->BODY.CurNodeID = gm_OHTStatus->m_OHTStatus.Data.CurNodeID;
//	pTrans->BODY.CurStationID = gm_OHTStatus->m_OHTStatus.Data.CurStationID;
	pTrans->BODY.PortType = m_PortType;

	// by zzang9un 2013.02.22 : EQ PIO Direction 추가
	pTrans->BODY.PIODirection = cbbPIODirection->ItemIndex + 1;

	// by zzang9un 2013.09.02 : Hoist, Shift Level을 계산하여 전달
	pTrans->BODY.HoistSpeedLevel = (cbbHoistSpeedLevel->ItemIndex + 1) * 10;
	pTrans->BODY.ShiftSpeedLevel = (cbbShiftSpeedLevel->ItemIndex + 1) * 10;

	// by zzang9un 2013.08.26 : EQPIO Disable 여부를 추가
	pTrans->BODY.TransOption.Info.AutoRecovery = false;
	pTrans->BODY.TransOption.Info.HandDetectEQ = chkHandDetectEQ->Checked;
//	pTrans->BODY.TransOption.Info.EQPIODisable_TP = chkEQPIODisable->Checked;
}
//---------------------------------------------------------------------------


/**
@brief   이적재 롱런을 위한 함수

         OnlineCmdUnit에서 호출하는 함수
@author  zzang9un
@date    2013.02.05
@param   TransCmdData : 이적재 명령 Type
@param   porttype : 이적재할 Port Type
@return  정상인 경우 0, Teaching Data Data를 못 찾은 경우 1을 리턴
*/
void TTransForm::SendTransCmd(BYTE TransCmdData, BYTE PortType)
{
    // Step 1. 이적재 변수 Setting
    SetTrans(TransCmdData, PortType);

    // Step 2. 이적재 명령 전송
    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

#if 0
    // 이적재 명령 구조체
    TRANS_CMD SndTransCmd;

    // 구조체 초기화
    memset(&SndTransCmd, 0x00, sizeof(TRANS_CMD));

    // 이적재 명령 셋팅
    MakeTransCmd(&SndTransCmd);

    // Load, Unload 구분
    switch(m_TransCmdData)
    {
    case CMD_LOAD:
        // 이적재 명령 전송
        m_CMDControl->SendTransCmd2OHT(TPCMD_TRANS_LOAD, &SndTransCmd);
        break;

    case CMD_UNLOAD:
        // 이적재 명령 전송
        m_CMDControl->SendTransCmd2OHT(TPCMD_TRANS_UNLOAD, &SndTransCmd);
        break;

    default:
        break;
    }
#else
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";

	// 이적재 명령 구조체
	iPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
	PKT_TP2OHT_TRANS_LOAD_UNLOAD	SEND_PACKET;

	// 구조체 초기화
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// 이적재 명령 셋팅
	MakeTransPkt(&SEND_PACKET);

	// ---- Tag
	// Load, Unload 구분
	switch(m_TransCmdData)
	{
		case CMD_LOAD:
			// 이적재 명령 전송
			SEND_PACKET.TAG.Value = TagID::CMD_TRANS_LOAD;
			m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
			break;

		case CMD_UNLOAD:
			// 이적재 명령 전송
			SEND_PACKET.TAG.Value = TagID::CMD_TRANS_UNLOAD;
			m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
			break;
			
		default:
			break;
	}
#endif

    delete m_CMDControl;    
}

