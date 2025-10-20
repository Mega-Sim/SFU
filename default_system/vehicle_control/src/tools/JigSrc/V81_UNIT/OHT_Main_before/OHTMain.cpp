//---------------------------------------------------------------------------
/**
@file    OHTMain.cpp
@version OHT 7.0
@brief   OHT Main Program Main Form CPP File
*/

#include <vcl.h>
#pragma hdrstop

#include "OHTMain.h"
#include "Utility.h"
#include "MyTools.h"
#include <fstream>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TOHTMainForm *OHTMainForm;


// by zzang9un 2013.04.10 : 공유변수를 위한 Critical Section
TCriticalSection *CS_CmdList; ///< CmdList를 위한 Critical Section 변수
TCriticalSection *CS_TaskList; ///< CmdList를 위한 Critical Section 변수
TCriticalSection *CS_CycleList; ///< CmdList를 위한 Critical Section 변수
TCriticalSection *CS_ResponceList; ///< CmdList를 위한 Critical Section 변수
TCriticalSection *CS_PassOpenList;

TCriticalSection *CS_OHTMainStatus; ///< OHTMainStatus를 위한 Critical Section 변수

TCriticalSection *CS_ThreadCycleLog;

//---------------------------------------------------------------------------
#define ADD_EXCEPT_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\Exception.log", __VA_ARGS__)
#define THREAD_STATUS_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\ThreadStatus.log", __VA_ARGS__)

#define AXIS_MASK_FUNC_POSITION     0
#define AXIS_MASK_FUNC_AMP_ENABLE   1
#define AXIS_MASK_FUNC_RUNNING      2
#define AXIS_MASK_FUNC_ERRORCODE    3
#define AXIS_MASK_FUNC_ORIGIN       4

//2014.04.16 doori.shin Thread 동작여부 확인 후 상태
#define THREAD_STATUS_UNKNOWN		0
#define THREAD_STATUS_ALIVE			1
#define THREAD_STATUS_STOP			2
#define THREAD_STATUS_DESTROY		3

/**
@brief   TOHTMainForm 클래스 생성자
@author  zzang9un
@date    2012.11.15
@param   Owner : TComponent
*/
__fastcall TOHTMainForm::TOHTMainForm(TComponent* Owner)
	: TForm(Owner)
{
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	Application->OnRestore = OnRestore;

	g_DataTransProgressStatus = 0;
	g_MAPUpdateStatus=0;

	m_StationData_STD = NULL;
	pLogTaskControl = NULL;
	pLogOCSComm = NULL;
	pLogTPComm = NULL;
	m_PathList = NULL;
	m_pServoControl = NULL;
	m_CIDData_STD = NULL;
	m_CollisionData_STD = NULL;
    m_FlowControlData_STD = NULL;
	m_MapData_STD = NULL;
	m_TeachingData_STD = NULL;
    m_PathData_STD = NULL;
	m_CIDManager_STD = NULL;
    m_CollisionManager_STD = NULL;
    m_FlowControlManager_STD = NULL;
	m_MapManager_STD = NULL;
	m_StationManager_STD = NULL;
	m_TeachingManager_STD = NULL;
	m_PathDataManager_STD = NULL;
	m_MapMakerFileManager_STD = NULL;
	m_pOHTMainStatus = NULL;
	m_pOHTDetect = NULL;
	m_nPreLogThTick = 0;

	memset(m_szUBGInfo_INI, 0x00, sizeof(m_szUBGInfo_INI));
	memset(m_szUBGInfo_Comm, 0x00, sizeof(m_szUBGInfo_Comm));

	// by zzang9un 2013.04.10 : Critical Section 변수 초기화
	CS_CmdList = new TCriticalSection;
	CS_TaskList= new TCriticalSection; ///< CmdList를 위한 Critical Section 변수
	CS_CycleList= new TCriticalSection; ///< CmdList를 위한 Critical Section 변수
	CS_ResponceList= new TCriticalSection; ///< CmdList를 위한 Critical Section 변수
	CS_PassOpenList = new TCriticalSection;
	CS_OHTMainStatus = new TCriticalSection;
	CS_ThreadCycleLog = new TCriticalSection;
//	m_CmdList = NULL;

	m_TaskList= NULL;                 ///< Main Command List
	m_ResponseList= NULL;                  ///< Main Command List
	m_CycleList= NULL;                 ///< Main Command List
	m_PassOpenList = NULL;
	m_PassPermitList = NULL;

    gm_ExecuteInfo = NULL;
    gm_OHTMainStatus = NULL;
	gm_HWSetStatus = NULL;

    m_MapData = NULL;
    m_TmpMapData = NULL;
	m_MapDataManager = NULL;

	m_UDPComm = NULL;
	m_UDPDiag = NULL;


	m_CmdManager = NULL;
	m_PathSearch = NULL;
	m_PassPermit = NULL;


//	m_socketThread = new SocketThread();

    m_HWCommon = NULL;
    m_HWSet = NULL;
    m_HWControlThread = NULL;
	m_MaskOfHW = NULL;

	m_DrivingControl = NULL;
	m_TransControl = NULL;
	m_ManualControl = NULL;
    m_ExecuteThread = NULL;

	m_StatusCheckThread = NULL;
	m_LogThread = NULL;
	m_ControlTask = NULL;
	m_TaskControlThread = NULL;
	m_ExtraJobThread = NULL;
//	m_AvsControlThread = NULL;
	m_MapMake_Status = NULL;
	m_StationDataManager = NULL;
	m_TeachingDataManager = NULL;

	m_MapMakeProcess = NULL;

//	m_MonitorControlThread = NULL;
	m_pParamSet = NULL;
    // by zzang9un 2013.02.19 : Form Title에 버전 정보 표시
	// String strFileDate = "", strTZitle = "", strCaption = ""; //룰 경고발생하여 선언과 동시에 데이터 할당
    // Title
    String strTitle = Application->Title;

    // 실행 파일 생성 시간
    TSearchRec rc;
    FindFirst(Application->ExeName, faArchive | faDirectory, rc);
	TDateTime dt = FileDateToDateTime(rc.Time);

    String strFileDate = L"[Date:" + FormatDateTime("yyyy.mm.dd", dt) + "]";

    String strCaption = strTitle + VerInfo.VersionString() + strFileDate;
    OHTMainForm->Caption = strCaption;

	checkLogDrive = false;
	checkLogFolder = false;	

	m_MapUpdateResult = NO_ERR;
	m_ParamUpdateResult = NO_ERR;
	
	//2014.04.15 doori.shin Thread Check변수 초기화
	m_nPreExecuteThTick = -1;
	m_nPreExtraJobThTick = -1;
	m_nPreHWCtrlThTick = -1;
	m_nPreTaskCtrlThTick = -1;
	m_nPreStatusChkThTick = -1;
    m_bUBGINILoaded = false;
	m_nEQPIO_Type = TYPE_PIO_HYBRID;
	m_bErrIotGateway = false;
	m_bErrOHTTypeMatching =false;
	InitializeCriticalSection(&m_CS);
}
//---------------------------------------------------------------------------
/**
@brief   Form Create 이벤트 발생 시 호출되는 함수

         OHT 초기화 작업 수행
@author  zzang9un
@date    2012.11.15
@param   Sender : TObject 클래스
*/
void __fastcall TOHTMainForm::FormCreate(TObject *Sender)
{
    // UI Init
	memoMain->Clear();
	tabIOTest->TabVisible = false;
	Tab81S->TabVisible = false;

    tabProgramInfo->Brush->Color = clWhite;

	Application->OnException = AppException;
}

/**
@brief   OnDestroy 이벤트 발생 시 호출되는 함수

		 동적 할당된 메모리 해제
         Thread 해제
@author  zzang9un
@date    2012.11.15
@param   Sender : TObject 클래스
*/
void __fastcall TOHTMainForm::FormDestroy(TObject *Sender)
{
//	tmOCSStatus->Enabled  = false;
//	tmMainTask->Enabled 	= false;
	tmUIupdate->Enabled = false;

	DeleteCriticalSection(&m_CS);

	DestroyClass();
}
//---------------------------------------------------------------------------

void TOHTMainForm::Update_UI()
{
    // by zzang9un 2013.04.10 : 모드 출력 추가
	if(gm_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO)
        panMode->Caption = "Auto";
    else
		panMode->Caption = "Manaul";

	panStatus->Caption = StatusToString(gm_OHTMainStatus->StatusCommon.MSCmdID.Info.Status);

    if((int)m_ControlTask->GetOHTError() != 0)
    {
    	panErrorCode->Caption = IntToStr((int)m_ControlTask->GetOHTError());
        panErrorCode->Color = clRed;
    }
    else
    {
        panErrorCode->Caption = "";
        panErrorCode->Color = clBlack;
    }

	// by zzang9un 2012.12.03 : 현재 Node ID 출력하도록 수정
	panCurNodeID->Caption = (int)gm_ExecuteInfo->ExecutePositionInfo.CurrNode;

	// by zzang9un 2013.03.20 : Station ID
	panCurStationID->Caption = (int)gm_OHTMainStatus->StatusCommon.CurStationID;

	// IoT Gateway 에러 일 경우 프로그램을 종료한다.
	if(m_bErrIotGateway ||m_bErrOHTTypeMatching)
	{
        this->Close();
    }
}
//---------------------------------------------------------------------------

/**
@brief   Status 변수를 String으로 변환하는 함수
@author  zzang9un
@date    2012.11.26
@param   status : Port ID
@return  Status를 변환한 문자열
*/
String TOHTMainForm::StatusToString(BYTE status)
{
    String strRtnStatus = "";

    switch(status)
	{
    case STS_INIT:
        strRtnStatus = "Initialized";
        break;
    case STS_GOING:
        strRtnStatus = "Going";
		break;
    case STS_ARRIVED:
        strRtnStatus = "Arrived";
		break;
    case STS_UNLOADING:
        strRtnStatus = "Unloading";
		break;
    case STS_UNLOADED:
        strRtnStatus = "Unloaded";
		break;
    case STS_LOADING:
        strRtnStatus = "Loading";
		break;
    case STS_LOADED:
        strRtnStatus = "Loaded";
		break;
    case STS_AUTOPOSITION:
        strRtnStatus = "Auto Position";
		break;
    case STS_RUNNING:
        strRtnStatus = "Running";
		break;
    case STS_COMPLETE:
        strRtnStatus = "Complete";
		break;
    case STS_ERROR:
        strRtnStatus = "Error";
		break;
	case STS_AUTORECOVERY:
		strRtnStatus = "Auto Recovery";
		break;
	case STS_AUTORECOVERY_BLANKTRANS:
		strRtnStatus = "Auto Recovery_B";
		break;
	case STS_AUTORECOVERY_DOUBLETRANS:
		strRtnStatus = "Auto Recovery_D";
		break;
	case STS_AUTORECOVERY_OVERRUN:
		strRtnStatus = "OverRun";
		break;
	case STS_TURN_OFF:
		strRtnStatus = "TurnOff";
		break;
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
		strRtnStatus = "Teaching";
		break;
	case STS_AUTO_TAUGHT:
		strRtnStatus = "Taught";
		break;
	case STS_AUTO_TRANS_HOMING:
		strRtnStatus = "Trans Homing";
		break;
	default:
		strRtnStatus = "Unknown";
        break;
    }

    return strRtnStatus;
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::SetInputMask(TCheckBox *CB_InputMask, TEdit *Edt_Mask_Num, TEdit *Edt_Mask_Value)
{
    int     nNum        = Edt_Mask_Num->Text.ToIntDef(1)-1;
    bool    bValue      = (bool)Edt_Mask_Value->Text.ToIntDef(0);
    bool    bChecked    = CB_InputMask->Checked;

    MASK_INPUT  *pMaskOfInput   = NULL;

    // 입력 값 feedback
    Edt_Mask_Num->Text     = nNum+1;
    Edt_Mask_Value->Text   = (int)bValue;


	// HW Mask가 없을 때 처리
    if(m_MaskOfHW==NULL)
    {
        if(bChecked==true)
        {
            CB_InputMask->Checked = false;
        }
        return;
    }

    // Input Mask가 없을 때 처리
    pMaskOfInput = m_MaskOfHW->getMaskOfInput(nNum);
    if(pMaskOfInput==NULL)
    {
        if(bChecked==true)
        {
            CB_InputMask->Checked = false;
        }
        return;
    }

    // 입력 값이 변경되지 않게 설정
    Edt_Mask_Num->Enabled       = !bChecked;
    Edt_Mask_Value->Enabled     = !bChecked;
    Edt_Mask_Num->ReadOnly      = bChecked;
    Edt_Mask_Value->ReadOnly    = bChecked;

    // 값 Masking
    if(bChecked==true)
    {
        pMaskOfInput->Value = bValue;
		pMaskOfInput->bUse  = true;
    }
    else
    {
        pMaskOfInput->bUse  = false;
    }

}

//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::tmInintTimer(TObject *Sender)
{
	tmInint->Enabled = false;
	OHTInit();

	Sleep(1000);
	tmUIupdate->Enabled = true;

}
//---------------------------------------------------------------------------

/**
@brief   OHT 초기화를 수행하는 함수
@author  zzang9un
@date    2012.11.15
@return  정상인 경우 0, 에러 발생 시 에러 코드 리턴
*/
void TOHTMainForm::OHTInit()
{
	uTP_CMD OHTInfo;      ///< OHT Info

	// Check D-Drive and Log folder by ks4423
	if(checkLogDrive = OHTUTILITY::Check_DDrive())
	{
		checkLogFolder = OHTUTILITY::Check_LogFolder();
	}
	else
	{
		checkLogFolder = false;
	}

	// Step 1. OHT에 사용할 Class 생성
	CreateClass();

    // Step 2. flag 변수 초기화
 //	ErrorCode_Main = NO_ERR;

	m_PathSearch->InitPathList();
//	m_CmdManager->InitCmdList();
	m_PassPermit->InitPassPermit();

    // Step 3. OHT 상태 변수 초기화 : 임시
    ///@todo by zzang9un 2012.11.26 : 향후 수정해야 함
    gm_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode = MODE_MANUAL;

	// by zzang9un 2012.11.28 : TP와 초기화 정보 테스트
	// OHT Init을 위한 OHT Info 정보 추가
	OHTInfo.OHTInfo.cData = 0;
	OHTInfo.OHTInfo.OHT_ID = 70;
	OHTInfo.OHTInfo.LineInfo = 100;
#if(OHT_NAME == OHT_NAME_STD_V80)
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_80;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_FOUP;
// 2016-12-03, Add by Labeler
#elif(OHT_NAME == OHT_NAME_STD_V81)
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_81;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_FOUP;
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_80;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_RETICLE;
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_80;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_EUV;
#elif(OHT_NAME == OHT_NAME_STD_V85)
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_85;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_EUV;
#elif(OHT_NAME == OHT_NAME_STD_V81S)
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_81;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_FOUP_S;
#else
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_70;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_FOUP;
#endif

	m_ControlTask->SetOHTInfo(OHTInfo);

	// by zzang9un 2012.12.03 : Main Task Timer를 활성화
  //	tmMainTask->Enabled = true;
	tmUIupdate->Enabled = true;

    // by zzang9un 2013.12.27 : OHT, AMC Version을 표시
	String strOHTVer = "", strAMCVer = "", strFPGAVer="";
	int nAMCVer = 0;
	int nFPGAVer = 0;

	if(OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)
	{
	    strOHTVer.sprintf(L"OHT_FOUP[Ver %u.%u.%u.%u]_%d",
	        (BYTE)OHTMainForm->VerInfo.Major,
	        (BYTE)OHTMainForm->VerInfo.Minor,
	        (BYTE)OHTMainForm->VerInfo.Release,
	        (BYTE)OHTMainForm->VerInfo.Build,
	        INTERNAL_VER);
	}
	else if(OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_RETICLE)
	{
	    strOHTVer.sprintf(L"OHT_Reticle[Ver %u.%u.%u.%u]_%d",
	        (BYTE)OHTMainForm->VerInfo.Major,
	        (BYTE)OHTMainForm->VerInfo.Minor,
	        (BYTE)OHTMainForm->VerInfo.Release,
	        (BYTE)OHTMainForm->VerInfo.Build,
	        INTERNAL_VER);
	}
	else if(OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
	{
	    strOHTVer.sprintf(L"OHT_EUV[Ver %u.%u.%u.%u]_%d",
	        (BYTE)OHTMainForm->VerInfo.Major,
	        (BYTE)OHTMainForm->VerInfo.Minor,
	        (BYTE)OHTMainForm->VerInfo.Release,
	        (BYTE)OHTMainForm->VerInfo.Build,
	        INTERNAL_VER);
	}
	else
	{
	    strOHTVer.sprintf(L"OHT[Ver %u.%u.%u.%u]_%d",
	        (BYTE)OHTMainForm->VerInfo.Major,
	        (BYTE)OHTMainForm->VerInfo.Minor,
	        (BYTE)OHTMainForm->VerInfo.Release,
	        (BYTE)OHTMainForm->VerInfo.Build,
	        INTERNAL_VER);
	}
	

    nAMCVer = m_HWSet->GetAMCVersion();

	int nAMCVerMajor = nAMCVer / 10000;
    int nAMCVerMinor = (nAMCVer - (nAMCVerMajor * 10000)) / 100;
    int nAMCVerRelease = (nAMCVer - (nAMCVerMajor * 10000)) % 100;

    strAMCVer.sprintf(L"AMC[Ver %d.%d.%02d]", nAMCVerMajor, nAMCVerMinor, nAMCVerRelease);


	nFPGAVer= m_HWSet->GetFPGAVersion();
	int nFPGAVerMajor = nFPGAVer / 10000;
	int nFPGAVerMinor = (nFPGAVer - (nFPGAVerMajor * 10000)) / 100;
	int nFPGAVerRelease = (nFPGAVer - (nFPGAVerMajor * 10000)) % 100;

	strFPGAVer.sprintf(L"FPGA[Ver %d.%d.%02d]", nFPGAVerMajor, nFPGAVerMinor, nFPGAVerRelease);


	lbOHTVer->Caption = strOHTVer;
	lbAMCVer->Caption = strAMCVer;
    lblFPGA->Caption =strFPGAVer;


}
//---------------------------------------------------------------------------

/**
@brief   OHT에 사용할 Class를 생성하는 함수
@author  zzang9un
@date    2012.10.22
@warning 반드시 생성 순서와 생성할 때 Parameter를 확인해야 함.
*/
void TOHTMainForm::CreateClass()
{
    // -------------------------------------------
    //1 Step 0. 기타 작업을 하는 Thread
	startLogManager(MAX_BYTE_SIZE_OF_LOG, this->checkLogFolder);

    // by zzang9un 2013.03.20 : Log Class 변수 추가
	pLogTaskControl = getLogUnit("D:\\log\\MainControl.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogOCSComm = getLogUnit("D:\\log\\OCSComm.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
    pLogTPComm = getLogUnit("D:\\log\\TPComm.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	//by puting 2016 1126 : default param load
	m_pParamSet =  new DEFAULT_PARAM_SET();
	memset(m_pParamSet, 0x00, sizeof(DEFAULT_PARAM_SET));
	LoadParameter("DataFiles\\OHTPARAM.ini",m_pParamSet);
	LoadPatternData("DataFiles\\OHTDetectAreaInfo.ini");
	LoadHighSpeedParameter("DataFiles\\HighSpeed.ini");
	WriteLog_NetworkInfo();	// 이더넷 카드 정보 로깅 [IPCONFIG]

	m_bErrIotGateway = IoTGatewayCheck() == false;	// IoT 게이트웨이 체크

	// SHCHO 2018.10.17
	// ※ 만약 OHT의 IOT 랜카드에 Gateway가 들어있을 경우, 프로그램을 종료한다
	// OCS Retry 로직에 영향을 미치기 때문에
	// 잘못된 경우, 최초 세팅과정에서 잘못 한 것이기 때문에
	// 에러코드 발생보다 프로그램 종료 후 재설정을 유도한다

	try
	{
#if(OHT_NAME == OHT_NAME_STD_V80)
		m_bErrOHTTypeMatching = (strcmp(m_pParamSet->OHTType, OHTTYPE_FOUP80) != 0);
#elif(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
		m_bErrOHTTypeMatching = (strcmp(m_pParamSet->OHTType, OHTTYPE_EUV80) != 0);
#elif(OHT_NAME == OHT_NAME_STD_V81S)
		m_bErrOHTTypeMatching = (strcmp(m_pParamSet->OHTType, OHTTYPE_FOUP81S) != 0);
#elif(OHT_NAME == OHT_NAME_STD_V81)
		m_bErrOHTTypeMatching = (strcmp(m_pParamSet->OHTType, OHTTYPE_FOUP81) != 0);
#elif(OHT_NAME == OHT_NAME_STD_V85)
		m_bErrOHTTypeMatching = (strcmp(m_pParamSet->OHTType, OHTTYPE_EUV85) != 0);
#endif

	}catch(...)
	{
    	m_bErrOHTTypeMatching = true;
    }

	if(m_bErrOHTTypeMatching == true)
	{
		AnsiString msg = "## OHT Type이 맞지 않는 파일을 실행하였습니다. ##\n\n";
		msg += "OHT.exe 실행파일이 OHTType에 맞늦지 확인하시기 바랍니다.\n";
		msg += "만약 OHT.exe 실행파일이 맞다면, OHTPARAM.ini의 [OHTInfo]의 OHTType 확인하시기 바랍니다.";
		msg += "(FOUP_V81E, EUV_V85,FOUP_V81S,FOUP_V80,EUV_V80)\n";
		msg += "\n확인 후 확인버튼을 누르면 프로그램이 종료됩니다.\n";

		wchar_t wmsg[1024];
		msg.WideChar(wmsg, sizeof(wmsg));
		Application->MessageBox(wmsg, L"OHT Type Error");
    }


	if(m_bErrIotGateway == true)
	{
		AnsiString msg = "## 윈도우 네트워크 어댑터 설정 오류 ##\n\n";
		msg += "IoT로 연결되는 이더넷 어댑터의 Gateway가 설정된것 같습니다.\n";
		msg += "이더넷 어댑터 설정을 다시 확인해 보세요.\n\n";
		msg += "만약 IoT Gateway Error를 검사하지 않기를 원한다면\n";
		msg += "OHTParam.INI의 [WIRELESS] IoT_Lan_Check_Ignore = 1\n위와 같이 설정하시기 바랍니다.\n";
		msg += "IoT 이더넷 카드 이름은 [WIRELESS] IOT_EternetName = 에서 설정할 수 있습니다.\n";
		msg += "\n확인을 누르면 프로그램이 종료됩니다.\n";

		wchar_t wmsg[1024];
		msg.WideChar(wmsg, sizeof(wmsg));
		Application->MessageBox(wmsg, L"IoT Gateway Error");
	}


    // -------------------------------------------
	//1 Step 1. 공유 변수 및 소켓 선언
//	m_CmdList = new LList;
	m_TaskList = new LList;                 ///< Main Command List
	m_ResponseList= new LList;                 ///< Main Command List
	m_CycleList = new LList;             ///< Main Command List
	m_PathList = new LList;             ///< Main Command List
	m_PassOpenList = new LList;        ///puting 통과개방을 위한 List
	m_PassPermitList = new LList;

	m_UDPComm = new UdpCom(m_pParamSet->nPortNumAdd);
	m_UDPDiag = new UdpDiag;


//	m_UDPComm->MyInfoUpdate()
//	m_UDPComm->Address_Broadcast();   //주소획득 연결 실패시 재시도 해야됨.

	gm_ExecuteInfo = new EXECUTE_INFO;
	gm_OHTMainStatus = new OHTMainStatus;
	gm_HWSetStatus = new HWSetStatus;

    //2013.07.11 doori.shin 플래스 구조체 메모리할당
	m_MapMake_Status = new MAPMAKE_STATUS;
	memset(m_MapMake_Status, 0x00, sizeof(MAPMAKE_STATUS));
	m_MapMakeProcess = new MapMakeProcess();

//	TEST_ADD_LOG("CreateClass 00 TOHTMainForm = %d", this);
//	TEST_ADD_LOG("CreateClass 00 gm_OHTMainStatus = %d", &gm_OHTMainStatus);

	// by zzang9un 2013.03.19 : Status 공용 구조체 초기화
	memset(gm_OHTMainStatus, 0x00, sizeof(OHTMainStatus));

	m_MapData = new MAP;
	m_TmpMapData = new MAP;
	m_MapDataManager = new MapDataManager;


	//2013.07.09 doori.shin 클래스 초기화

	m_StationDataManager = new StationDataManager();
	m_TeachingDataManager = new TeachingDataManager();

	// -------------------------------------------
	//2016.04.21 KJD 표준화 맵 파일 클래스 초기화
	m_CIDData_STD = new STD_CID;
	m_CollisionData_STD = new STD_Collision;
	m_FlowControlData_STD = new STD_FlowControl;
	m_MapData_STD = new STD_MAP;
	m_StationData_STD = new STD_Station;
	m_TeachingData_STD = new STD_Teaching;
	m_PathData_STD = new STD_PathData;

	m_CIDManager_STD = new CIDManager_STD;
	m_CollisionManager_STD = new CollisionManager_STD;
	m_FlowControlManager_STD = new FlowControlManager_STD;
	m_MapManager_STD = new MapDataManager_STD;
	m_StationManager_STD = new StationManager_STD;
	m_TeachingManager_STD = new TeachingManager_STD;
	m_PathDataManager_STD = new PathDataManager_STD;

	//File Map Teahcing puting
//	m_MapMakerFile_STD = new STD_MapMakerFile;
	m_MapMakerFileManager_STD = new MapMakerFileManager_STD;


	// -------------------------------------------
	//1 2. Command Manager 및 Pathsearch
	m_PathSearch    = new PathSearch();
	m_CmdManager    = new CmdManager();
	m_PassPermit    = new PassPermit();

    // -------------------------------------------
    // 3. HW Set
    m_HWCommon = new HWCommon;
	m_HWSet = new HWSet();

	if(m_HWSet!=NULL)
    {
        m_pOHTDetect = m_HWSet->m_pOHTDetect;
    }
//	m_AvsControl = new AvsControl();

	m_HWControlThread = new HWControlThread();

	m_MaskOfHW = m_HWCommon->GetMask();

    // -------------------------------------------
    //1 4. D, T, M, Execute& LogThread
    m_DrivingControl = new DrivingControl();
	m_TransControl = new TransControl();
	m_ManualControl = new ManualControl();
	m_ExecuteThread = new ExecuteThread();
	m_LogThread = new LogThread();//2013.07.11 추가
    // -------------------------------------------
    //1 5. Status Thread
    m_StatusCheckThread = new StatusCheckThread();
	m_ControlTask = new TaskControl();
	m_TaskControlThread = new TaskControlThread();

	m_ExtraJobThread = new ExtraJobThread();
//	m_AvsControlThread = new AvsControlThread();
  //	m_TaskControlThread->setMainPointer((void*)this);

 	//1 6. MonitorControl Thread
//	m_HID = new HID_VII_ZR(COM_HID);     //위치 옮기기
//	m_IPCLoadLib = new IPCLibrary();
//	m_NetworkDll = new NetworkSinalLib();
//	m_CPUandMemInfo = new CPUandMemInfo();
//	m_EtcMonitoring = new MONITORING;
	//m_SigmaMonitor = NULL;	//new SigmaMonitor();
//	m_MonitorControlThread = new MonitorControlThread(gm_OHTMainStatus, m_HID, m_NetworkDll, m_IPCLoadLib, m_CPUandMemInfo, m_EtcMonitoring);

	// -------------------------------------------
	//1 6-0. Thread 우선 순위 설정
	m_LogThread->setPriority(THREAD_PRIORITY_LOWEST);    // 우선 순위가 떨어짐: THREAD_PRIORITY_BELOW_NORMAL
	m_HWControlThread->setPriority(THREAD_PRIORITY_HIGHEST);
	m_ExecuteThread->setPriority(THREAD_PRIORITY_HIGHEST);
	m_ExtraJobThread->setPriority(THREAD_PRIORITY_NORMAL);
//	m_AvsControlThread->setPriority(THREAD_PRIORITY_NORMAL);
//	m_MonitorControlThread->setPriority(THREAD_PRIORITY_LOWEST);

	// -------------------------------------------
	//1 6. Thread 시작
	m_LogThread->beginThread();    // 제일 먼저 시작할 것 : 기반 정보 처리
	m_ExtraJobThread->beginThread();
	m_HWControlThread->beginThread();
	m_ExecuteThread->beginThread();
	m_StatusCheckThread->beginThread();
	m_TaskControlThread->beginThread();
//	m_MonitorControlThread->beginThread();
  //	TEST_ADD_LOG("CreateClass 01 m_socketThread = %d", m_socketThread);

	//2014.04.15 doori.shin Thread Start
	if(m_HWSet != NULL) m_pServoControl = m_HWSet->m_pServoMonitoring;

	tmChkThreadStatus->Enabled = true;

//	m_ControlTask->m_pUBGParamSet = OHTMainForm->m_PathSearch->m_pUBGParamSet;	// TaskControl의 UBG 패턴정보 포인터 값 세팅
//	m_StatusCheckThread->m_pHWCommon = GetHWCommon();
}
//---------------------------------------------------------------------------

/**
@brief   OHT에 사용할 Class를 소멸하는 함수
@author  zzang9un
@date    2012.10.22
@warning 반드시 생성 순서의 역순으로 delete 처리
*/
void TOHTMainForm::DestroyClass()
{
//	Test_RESET_LOG;

	//DWORD dwTime = timeGetTime();
    // -------------------------------------------
    // step 1. Thread 해제
    // by zzang9un 2012.11.15 (임태웅 선임 확인): Thread 종료 없이 delete만 해도 됨
//	TEST_ADD_LOG("Destroy 00");
//	ADD_EXCEPT_LOG("Destroy 00");
	//delete m_socketThread;
	m_HWSet->m_pDrivingAxis->MoveStop(0.0035);
	m_HWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, false);
	if (m_pParamSet->IsLCDAndLED)
	{
		m_StatusCheckThread->GetReadLCDData(true); // LCD에 종료 OHTOFF 처리
    }
//	//장애 호기 or Main Close 육안구분 by Jrespect 2018.03.10
//	m_HWSet->m_pLamp->LampOff();//Main 종료시 LED OFF
	Sleep(100);

	m_StatusCheckThread->qiutRealThreadProc();
	m_ExecuteThread->qiutRealThreadProc();
	m_HWControlThread->qiutRealThreadProc();
	m_LogThread->qiutRealThreadProc();
	m_TaskControlThread->qiutRealThreadProc();
	m_ExtraJobThread->qiutRealThreadProc();
//	m_AvsControlThread->qiutRealThreadProc();
//	m_MonitorControlThread->qiutRealThreadProc();

//	ADD_EXCEPT_LOG("Destroy 01");

	//dwTime = timeGetTime();

//	dwTime = timeGetTime();
//	while(timeGetTime()-dwTime<5000)	// 임시 Thread Timeout
	while(1)	// 임시 Thread Timeout
	{
		if(	m_StatusCheckThread->isOnThreading()==false &&
			m_ExecuteThread->isOnThreading()==false &&
			m_HWControlThread->isOnThreading()==false &&
			m_LogThread->isOnThreading()==false &&
			m_TaskControlThread->isOnThreading()==false &&
			m_ExtraJobThread->isOnThreading()==false)// &&
		   //	m_AvsControlThread->isOnThreading()==false)
		{
			break;
		}
		Sleep(1);
	}
//    ADD_EXCEPT_LOG("Destroy 02");
	//SHCHO 2018.08.06 : oht.exe 종료 에러 해소됨. DiagManager 소멸자 호출되어 timer 정상종료
	DiagManager::DestroyInstance();

//    TEST_ADD_LOG("Destroy 1 ~ Interval:%d", (timeGetTime()-dwTime));

	delete m_StatusCheckThread;
    delete m_ExecuteThread;
	delete m_HWControlThread;
	delete m_LogThread;
	delete m_TaskControlThread;
//	delete m_MonitorControlThread;

//	ADD_EXCEPT_LOG("Destroy 03");

	// -------------------------------------------

	delete m_ControlTask;
	/*
	// Socket Class 해제
	delete m_OCSSocket;
	delete m_TPSocket;
	*/

//	ADD_EXCEPT_LOG("Destroy 04");

    // -------------------------------------------
    // Step 2. Execute 내부에서 수행되는 Class 해제
    delete m_DrivingControl;
    delete m_TransControl;
    delete m_ManualControl;

//	ADD_EXCEPT_LOG("Destroy 05");


    // -------------------------------------------
    // Step 3. HW Set
    delete m_HWSet;
    delete m_HWCommon;

//	delete m_AvsControl;
//	ADD_EXCEPT_LOG("Destroy 06");

    // -------------------------------------------
    // Step 4. 공유 변수, 통신, 공유 List 해제
//  delete m_OCSSocket;
//    delete m_TPSocket;
	delete m_PassPermit;
	delete m_CmdManager;
	delete m_PathSearch;

//	ADD_EXCEPT_LOG("Destroy 07");
    delete m_MapMakerFileManager_STD;

	delete m_PathDataManager_STD;
	delete m_TeachingManager_STD;
	delete m_StationManager_STD;

	delete m_MapManager_STD;
	delete m_FlowControlManager_STD;
    delete m_CollisionManager_STD;
	delete m_CIDManager_STD;

	delete m_PathData_STD;
	delete m_TeachingData_STD;
    delete m_StationData_STD;
	delete m_MapData_STD;
	delete m_FlowControlData_STD;
	delete m_CollisionData_STD;
	delete m_CIDData_STD;

	delete m_TeachingDataManager;
	delete m_StationDataManager;

	delete m_MapDataManager;
	delete m_TmpMapData;

	if(m_MapData != NULL)
		delete m_MapData;	//<-- 문제 발생

	delete m_MapMakeProcess;
	delete m_MapMake_Status;



	delete gm_HWSetStatus;
	delete gm_OHTMainStatus;
	delete gm_ExecuteInfo;

	delete m_UDPDiag;
	delete m_UDPComm;

//	ADD_EXCEPT_LOG("Destroy 11");

//	delete m_CmdList;
	delete m_PassPermitList;
	delete m_PassOpenList;        ///puting 통과개방을 위한 List
	delete m_PathList;             ///< Main Command List
	delete m_CycleList;             ///< Main Command List
	delete m_ResponseList;                 ///< Main Command List
	delete m_TaskList;                ///< Main Command List

    delete m_pParamSet;

//	ADD_EXCEPT_LOG("Destroy 12");

    endLogManager();

//    TEST_ADD_LOG("Destroy 9");

	tmChkThreadStatus->Enabled = false;

//	Sleep(1000);

//	TEST_ADD_LOG("Destroy 10");

}

void __fastcall TOHTMainForm::tmUIupdateTimer(TObject *Sender)
{
	Update_UI();
}
//---------------------------------------------------------------------------

/**
@brief   Remocon 객체를 반환하기 위한 함수
@author  ji7412.lee
@date    2013.05.27
*/
Remocon* TOHTMainForm::GetHWSetRemocon()
{
	if(m_HWSet->m_pRemocon != NULL)
	{
		return m_HWSet->m_pRemocon;
	}
	return NULL;
}


//---------------------------------------------------------------------------

/**
@brief   LCD 객체를 반환하기 위한 함수
@author  kyh
@date    2017.12.12
*/
LCD* TOHTMainForm::GetHWSetLCD()
{
	if(m_HWSet->m_pLCD != NULL)
	{
		return m_HWSet->m_pLCD;
	}
	return NULL;
}


/**
@brief   Hand와 Shutter Origin 상태를 보기 위한 함수
         맵파일 종류 확인 추가 : 조정용 맵인경우 AUTO로 전환 안됨.
@author  elibeli2
@date    2014.01.23
*/

// Manual_CID()

int TOHTMainForm::GetReadyToOperation()
{
	int nError = NO_ERR;
	int tmpCurrenrtNodeID = 0;

	// 디버깅시 확인하기 위하여 값 대입하여 사용
	unsigned int CID_DataType = GetCIDDataManager_STD()->m_CIDFileHeader.DataType;
    unsigned int Collision_DataType = GetCollisionDataManager_STD()->m_CollisionFileHeader.DataType;
    unsigned int FlowControl_DataType = GetFlowControlDataManager_STD()->m_FlowControlFileHeader.DataType;
    unsigned int Map_DataType = GetMapDataManager_STD()->m_MapFileHeader.DataType;
    unsigned int Station_DataType = GetStationDataManager_STD()->m_StationFileHeader.DataType;
    unsigned int Teaching_DataType = GetTeachingDataManager_STD()->m_TeachingFileHeader.DataType;
    unsigned int Path_DataType = GetPathDataManager_STD()->m_PathDataFileHeader.DataType;
	// By elibeli2 2014.01.23 Origin Check
	nError = m_HWSet->m_pHand->ReadyToOperation();
	if( nError == NO_ERR)
	{
		 if(m_HWSet->m_pHand->IsInposition() ==false)
		 {
           	nError = ERR_HAND_NOTORIGIN;
         }
    }
	if( nError == NO_ERR)
	{
		nError = m_HWSet->m_pShutter->ReadyToOperation();
		if(nError == NO_ERR)
		{
			if(m_HWSet->m_pShutter->IsInposition(BOTH) ==false)
				nError = ERR_SHUTTER_NOTORIGIN;
		}
	}


	if( nError == NO_ERR)
	{
		if(((m_HWSet->m_pInnerFoup->IsFoupDetect()== false)  &&(m_HWSet->m_pHand->IsClose() ==true)) ||
			((m_HWSet->m_pInnerFoup->IsFoupDetect()== true)  &&(m_HWSet->m_pHand->IsOpen() ==true)))
				nError = ERR_HAND_NOTORIGIN;
	}

    if( nError == NO_ERR)
	{
		if(m_HWSet->m_pHand->IsPushOn()==true)
			nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
	}


	//이적재OK플래그 처리 Shift/Hoist Home 위치 확인_ 2017 06 04
	if( nError == NO_ERR)
	{
		if((fabs(m_HWSet->m_pShiftAxis->GetCurrentPosition()) >0.5)
			|| (m_HWSet->m_pShiftAxis->IsShiftHome() ==false))

		{
			nError = ERR_SHIFT_NOTORIGIN;
		}

	}

	if( nError == NO_ERR)
	{
#if(OHT_NAME != OHT_NAME_STD_V85)
		if((m_HWSet->m_pHoistAxis->IsHoistHome()==false)

//#ifdef OHT_HIGH_RAIL_CHECK_HOME      //puting 추후 확인 20190402
//		|| ((m_HWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladd() < (HOIST_SMALL_ADD_HOISTHOME_DISTANCE + 0.5))
//		&& (m_HWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladd() > (HOIST_SMALL_ADD_HOISTHOME_DISTANCE - 0.5))))
//#else
		|| (fabs(m_HWSet->m_pHoistAxis->GetCurrentPosition()) > HOIST_HOME_INPOS_LIMIT))
//#endif
		{
			nError = ERR_HOIST_NOTORIGIN;
		}
#else
		if((m_HWSet->m_pHoistAxis->IsHoistOriginFromSmalladd()==false)
		|| (m_HWSet->m_pHoistAxis->IsHoistHome()==false)
		|| (fabs(m_HWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) > HOIST_HOME_INPOS_LIMIT))
		{
			if(m_HWSet->m_pHoistAxis->IsHoistHome()==false)
				ADD_MEMO_T("IsHoistHome");
			else if(m_HWSet->m_pHoistAxis->IsHoistOriginFromSmalladd()==false)
				ADD_MEMO_T("IsHoistOriginFromSmalladd");

			//ADD_EXCEPT_LOG("Hoist Origin Error, %d %f",m_HWSet->m_pHoistAxis->IsHoistOriginFromSmalladd(),m_HWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition());
			nError = ERR_HOIST_NOTORIGIN;
		}
#endif



	}

// 조정용 맵 적용시 움직이지 않는 문제 임시조치
// 나중에 꼭 풀어줘야 한다
    if( nError == NO_ERR)
	{
		if( ((CID_DataType & bit_0) == bit_0)
	  ||((Collision_DataType & bit_0) == bit_0)
	  ||((FlowControl_DataType & bit_0) == bit_0)
	  ||((Map_DataType & bit_0) == bit_0)
	  ||((Station_DataType & bit_0) == bit_0)
	  ||((Teaching_DataType & bit_0) == bit_0)
	  ||((Path_DataType & bit_0) == bit_0) )
		{
			// 맵 데이터 중 조정용 맵이 포함됨.
			// 에러 종류 정의
			nError = ERR_AUTO_DATAUPLOAD;
		}
		else
		{


			nError = NO_ERR;
		}
	}


	
	// 김태균 2016 03 23
	if(nError == NO_ERR)
	{
		tmpCurrenrtNodeID = (int)gm_ExecuteInfo->ExecutePositionInfo.CurrNode;
		nError = m_HWSet->m_pCID->CID_Manual_Mode_Off();
		if (nError == NO_ERR)
		{
			if((gm_ExecuteInfo->ExecutePositionInfo.CurrStation == 0) &&(tmpCurrenrtNodeID !=0))
			{
				nError = m_HWSet->m_pCID->ReadyToOperation(tmpCurrenrtNodeID, false);

			}else if((gm_ExecuteInfo->ExecutePositionInfo.CurrStation != 0) &&(tmpCurrenrtNodeID !=0))
			{
				STATION_DATA *pSData;

				pSData = GetStationDataManager()->Find(gm_ExecuteInfo->ExecutePositionInfo.CurrStation);

				if (pSData == NULL)
					nError = ERR_DATA_STATION_NOEXIST;
				else
                	nError = m_HWSet->m_pCID->ReadyToOperation(pSData->NextNodeID, true);

			}
		}

	}
	//AOHC-125 Auto 전환 전, 자가 진단 기능 추가 
	// KYH 2019.01.21. FoupCoverDetect 센서가 감지된 경우 Error
	if(nError == NO_ERR)
	{
		if(m_HWSet->m_pFoupCoverOpenDetect->IsDetect() == true)
			nError = ERR_FOUP_COVER_OPEN_DETECT;
	}

	// KYH 2019.01.21. Regulator 전압이 낮을 경우 Error
	if(nError == NO_ERR)
	{
		if(GetOHTStatus()->m_cpMonitorData.HIDData.Ecap <= 240)
			nError = ERR_ECAP_OVERVOLTAGE_RUN; //사용하지 않는 에러명 사용..
	}

	// KYH 2019.01.21. Steering 변경 가능 확인 (분기 Node Station 제외)
	if(nError == NO_ERR)
	{
		if(gm_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_NODE)
		{
			if(m_HWSet->m_pSteer->IsLeftPos(BOTH) == true)
			{
				nError = m_HWSet->m_pSteer->Right(false);
			}
			else if(m_HWSet->m_pSteer->IsRightPos(BOTH) == true)
			{
				nError = m_HWSet->m_pSteer->Left(false);
			}
			else
			{
				nError = ERR_STEER_NOTSAMESTATUS;
			}
		}
		else if((gm_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_TAG_STATION) ||
		(gm_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_QR_STATION_LEFT) ||
		(gm_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_QR_STATION_RIGHT) ||
		(gm_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_TAG_STATION_BACK))
		{
			int nID = gm_ExecuteInfo->ExecutePositionInfo.CurrNode;
			NODE_INFO_STD_MAP *pNode = GetNodeObj(nID);

			if(pNode != NULL)
			{
				if(pNode->LeftSteering == 0) //0 = N, 1 = L, 2 = R 한쪽만 조향이 있을수 없으므로
				{
					if(m_HWSet->m_pSteer->IsLeftPos(BOTH) == true)
					{
						nError = m_HWSet->m_pSteer->Right(false);
					}
					else if(m_HWSet->m_pSteer->IsRightPos(BOTH) == true)
					{
						nError = m_HWSet->m_pSteer->Left(false);	
					}
					else
					{
						nError = ERR_STEER_NOTSAMESTATUS;
					}
				}
			}
		}
		else
		{
		//ADD_LOG_TASKCONTROL("마크 안되어 Steering NG");
			//Mark 안됨.
		}
	}

	if(nError == NO_ERR)
	{
		//AOHC-256  M -> A UBG Open Com
		//UBG 패턴 0 값 체크 시, Open com 상태 유지지만 안된 경우 재확인하기 위하여 추가
		nError =  m_pOHTDetect->CheckUBGConnect(true);

	}


	return nError;
}

/**
@brief   Application이 Exception 에러가 발생하면 로그를 찍고 화면을 Capture하기 위한 함수
@author  LSJ
@date    2013.10.17
*/
void __fastcall TOHTMainForm::AppException(TObject *Sender, Exception *E)
{
	ADD_EXCEPT_LOG("%s", E->Message);

	if(gm_LogManager->Get_LogFolder())
	{
		CaptureScreen();
	}
}

/**
@brief   현재의 Windows 화면을 Capture하기 위한 함수
@author  LSJ
@date    2013.10.17
*/
void TOHTMainForm::CaptureScreen()
{
   struct date today;
   struct time currenttime;
   AnsiString strFullName = "";
   AnsiString strOHTPath=".";
   AnsiString strLogPath=".";

   getdate(&today);
   gettime(&currenttime);

   HWND hwnd = GetDesktopWindow ();
   if(hwnd)
   {
      HDC dc = ::GetWindowDC(hwnd);
      Graphics::TCanvas *pCanvas = new Graphics::TCanvas;
      pCanvas->Handle = dc;

      TRect rect = pCanvas->ClipRect;
      TImage *pImg = new TImage(OHTMainForm);
      pImg->Picture->Bitmap->Width  = rect.Width();
      pImg->Picture->Bitmap->Height = rect.Height();
      pImg->Picture->Bitmap->Canvas->CopyRect(rect, pCanvas, rect);

      strOHTPath=ExtractFileDir(Application->ExeName);

	  strFullName.sprintf("D:\\log\\MainForm%04d%02d%02d%02d%02d.bmp",
						  today.da_year, today.da_mon, today.da_day,currenttime.ti_hour,currenttime.ti_sec);

	  pImg->Picture->SaveToFile(strFullName);
      delete pImg;
	  delete pCanvas;
      ::ReleaseDC(hwnd, dc);
   }
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkAxisClick(TObject *Sender)
{
    TCheckBox   *CB_InputMask   = chkAxis;

    TEdit       *Edt_Mask_Value = edtAxisMask_Value;

    int     nAxisNum    = cbbAxisNum->ItemIndex;
    int     nFuncNum    = cbbFnType->ItemIndex;
    int     nValue      = Edt_Mask_Value->Text.ToIntDef(0);
    bool    bChecked    = CB_InputMask->Checked;

    MASK_AXIS   *pMaskOfAxis    = NULL;

    // 입력 값 feedback
    if(nAxisNum==-1)
    {
        cbbAxisNum->ItemIndex = 0;
    }
    if(nFuncNum==-1)
    {
		cbbFnType->ItemIndex = 0;
    }
    if(nAxisNum==-1 || nFuncNum==-1)
    {
        if(bChecked==true)
        {
            CB_InputMask->Checked = false;
        }
        return;
    }
    Edt_Mask_Value->Text   = (int)nValue;

    // HW Mask가 없을 때 처리
    if(m_MaskOfHW==NULL)
    {
        if(bChecked==true)
        {
            CB_InputMask->Checked = false;
        }
        return;
    }

    // Axis Mask가 없을 때 처리
    pMaskOfAxis = m_MaskOfHW->getMaskOfAxis(nAxisNum);
    if(pMaskOfAxis==NULL)
    {
        if(bChecked==true)
        {
            CB_InputMask->Checked = false;
        }
        return;
	}

    // 입력 값이 변경되지 않게 설정
    cbbAxisNum->Enabled   = !bChecked;
    cbbFnType->Enabled    = !bChecked;
    Edt_Mask_Value->Enabled     = !bChecked;
    Edt_Mask_Value->ReadOnly    = bChecked;

    // 값 Masking
    switch(nFuncNum)
    {
    case AXIS_MASK_FUNC_POSITION:
        if(bChecked==true)
        {
            pMaskOfAxis->Position.Value = nValue;
            pMaskOfAxis->Position.bUse  = true;
        }
        else
        {
            pMaskOfAxis->Position.bUse = false;
        }
        break;

    case AXIS_MASK_FUNC_AMP_ENABLE:
        if(bChecked==true)
        {
            pMaskOfAxis->AmpEnable.Value    = nValue;
            pMaskOfAxis->AmpEnable.bUse     = true;
        }
        else
        {
			pMaskOfAxis->AmpEnable.bUse     = false;
        }
        break;

    case AXIS_MASK_FUNC_RUNNING:
        if(bChecked==true)
        {
            pMaskOfAxis->Running.Value    = nValue;
            pMaskOfAxis->Running.bUse     = true;
        }
        else
        {
            pMaskOfAxis->Running.bUse     = false;
        }
        break;

    case AXIS_MASK_FUNC_ERRORCODE:
        if(bChecked==true)
        {
            pMaskOfAxis->ErrorCode.Value    = nValue;
            pMaskOfAxis->ErrorCode.bUse     = true;
        }
        else
        {
            pMaskOfAxis->ErrorCode.bUse     = false;
        }
        break;

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
    case AXIS_MASK_FUNC_ORIGIN:
		if(bChecked==true)
        {
            pMaskOfAxis->Origin.Value    = nValue;
            pMaskOfAxis->Origin.bUse     = true;
		}
        else
        {
            pMaskOfAxis->Origin.bUse     = false;
        }
        break;
#endif
    default:
        break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask1Click(TObject *Sender)
{
    SetInputMask(chkInputMask1, edtInputMask_Num1, edtInputMask_Value1);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask2Click(TObject *Sender)
{
    SetInputMask(chkInputMask2, edtInputMask_Num2, edtInputMask_Value2);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask3Click(TObject *Sender)
{
    SetInputMask(chkInputMask3, edtInputMask_Num3, edtInputMask_Value3);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask4Click(TObject *Sender)
{
     SetInputMask(chkInputMask4, edtInputMask_Num4, edtInputMask_Value4);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask5Click(TObject *Sender)
{
     SetInputMask(chkInputMask5, edtInputMask_Num5, edtInputMask_Value5);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask6Click(TObject *Sender)
{
    SetInputMask(chkInputMask6, edtInputMask_Num6, edtInputMask_Value6);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask7Click(TObject *Sender)
{
    SetInputMask(chkInputMask7, edtInputMask_Num7, edtInputMask_Value7);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask8Click(TObject *Sender)
{
    SetInputMask(chkInputMask8, edtInputMask_Num8, edtInputMask_Value8);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::imgIOTestDblClick(TObject *Sender)
{
	if(tabIOTest->TabVisible)
        tabIOTest->TabVisible = false;
    else
		tabIOTest->TabVisible = true;

	if(Tab81S->TabVisible)
		Tab81S->TabVisible = false;
	else
		Tab81S->TabVisible = true;


}
//---------------------------------------------------------------------------


/**
@brief   OHTMain에서 생성한 UDPCOm 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  TaskControl 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
UdpCom* TOHTMainForm::GetUDPCom()
{
	if(this->m_UDPComm == NULL)
		return NULL;
	else
		return this->m_UDPComm;
}


/**
@brief   OHTMain에서 생성한 UDPDiag 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  TaskControl 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
UdpDiag* TOHTMainForm::GetUDPDiag()
{
	if(this->m_UDPDiag == NULL)
		return NULL;
	else
		return this->m_UDPDiag;
}


/**
@brief   OHTMain에서 생성한 TaskControl를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  TaskControl 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
TaskControl* TOHTMainForm::GetTaskControl()
{
    if(this->m_ControlTask == NULL)
        return NULL;
    else
        return this->m_ControlTask;
}
//---------------------------------------------------------------------------

///**
//@brief   OHTMain에서 생성한 LList를 리턴하는 함수
//@author  zzang9un
//@date    2014.02.06
//@return  LList 포인터, 생성이 되지 않은 경우 Null을 리턴
//*/
//LList* TOHTMainForm::GetCmdList()
//{
//    if(this->m_CmdList == NULL)
//        return NULL;
//    else
//        return this->m_CmdList;
//}

//---------------------------------------------------------------------------
/**
@brief   OHTMain에서 생성한 LList를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  LList 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
LList* TOHTMainForm::GetTaskList()
{
    if(this->m_TaskList == NULL)
        return NULL;
    else
		return this->m_TaskList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain에서 생성한 LList를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  LList 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
LList* TOHTMainForm::GetCycleList()
{
    if(this->m_CycleList == NULL)
        return NULL;
    else
        return this->m_CycleList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain에서 생성한 LList를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  LList 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
LList* TOHTMainForm::GetResponseList()
{
    if(this->m_ResponseList == NULL)
        return NULL;
    else
        return this->m_ResponseList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain에서 생성한 LList를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  LList 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
LList* TOHTMainForm::GetPathList()
{
    if(this->m_PathList == NULL)
        return NULL;
    else
		return this->m_PathList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain에서 생성한 LList를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  LList 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
LList* TOHTMainForm::GetPassOpenList()
{
	if(this->m_PassOpenList == NULL)
		return NULL;
	else
		return this->m_PassOpenList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain에서 생성한 LList를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  LList 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
LList* TOHTMainForm::GetPassPermitList()
{
	if(this->m_PassPermitList == NULL)
		return NULL;
	else
		return this->m_PassPermitList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain에서 생성한 EXECUTE_INFO를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  EXECUTE_INFO 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
EXECUTE_INFO* TOHTMainForm::GetExecuteInfo()
{
    if(this->gm_ExecuteInfo == NULL)
        return NULL;
    else
        return this->gm_ExecuteInfo;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 OHT_STATUS를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  OHT_STATUS 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
OHTMainStatus* TOHTMainForm::GetOHTStatus()
{
    if(this->gm_OHTMainStatus == NULL)
        return NULL;
    else
        return this->gm_OHTMainStatus;    
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 HWSET_STATUS를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  HWSET_STATUS 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
HWSetStatus* TOHTMainForm::GetHWSetStatus()
{
	if(this->gm_HWSetStatus == NULL)
		return NULL;
	else
		return this->gm_HWSetStatus;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 MAPMAKE_STATUS를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  MAPMAKE_STATUS 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
MAPMAKE_STATUS* TOHTMainForm::GetMapMakeStatus()
{
	if(this->m_MapMake_Status == NULL)
        return NULL;
    else
        return this->m_MapMake_Status;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 MAP을 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  MAP 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
MAP* TOHTMainForm::GetMap()
{
    if(this->m_MapData== NULL)
        return NULL;
    else
        return this->m_MapData;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 STD_CID MAP을 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  MAP 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
STD_CID* TOHTMainForm::GetCID_STD()
{
    if(this->m_CIDData_STD == NULL)
        return NULL;
    else
        return this->m_CIDData_STD;
}
//---------------------------------------------------------------------------


/**
@brief   OHTMain에서 생성한 STD_Collision MAP을 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  MAP 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
STD_Collision* TOHTMainForm::GetCollision_STD()
{
    if(this->m_CollisionData_STD == NULL)
        return NULL;
    else
        return this->m_CollisionData_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 STD_FlowControl MAP을 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  MAP 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
STD_FlowControl* TOHTMainForm::GetFlowControl_STD()
{
    if(this->m_FlowControlData_STD == NULL)
        return NULL;
    else
        return this->m_FlowControlData_STD;
}

//---------------------------------------------------------------------------
/**
@brief   OHTMain에서 생성한 STD_MAP MAP을 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  MAP 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
STD_MAP* TOHTMainForm::GetMap_STD()
{
    if(this->m_MapData_STD == NULL)
        return NULL;
    else
        return this->m_MapData_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 STD_Station MAP을 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  MAP 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
STD_Station* TOHTMainForm::GetStation_STD()
{
    if(this->m_StationData_STD == NULL)
        return NULL;
    else
        return this->m_StationData_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 STD_Teaching MAP을 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  MAP 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
STD_Teaching* TOHTMainForm::GetTeaching_STD()
{
    if(this->m_TeachingData_STD == NULL)
        return NULL;
    else
        return this->m_TeachingData_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 STD_PathData MAP을 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  MAP 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
STD_PathData* TOHTMainForm::GetPathData_STD()
{
	if(this->m_PathData_STD == NULL)
		return NULL;
	else
        return this->m_PathData_STD;
}
//---------------------------------------------------------------------------


/**
@brief   OHTMain에서 생성한 MapDataManager를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  MapDataManager 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
MapDataManager* TOHTMainForm::GetMapDataManager()
{
    if(this->m_MapDataManager == NULL)
        return NULL;
    else
        return this->m_MapDataManager;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 StationDataManager를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  StationDataManager 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
StationDataManager* TOHTMainForm::GetStationDataManager()
{
	if(this->m_StationDataManager == NULL)
		return NULL;
	else
		return this->m_StationDataManager;
}
//---------------------------------------------------------------------------
 /**
@brief   OHTMain에서 생성한 StationDataManager를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  StationDataManager 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
MapMakeProcess* TOHTMainForm::GetMapMakeProcess()
{
	if(this->m_MapMakeProcess == NULL)
		return NULL;
	else
		return this->m_MapMakeProcess;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain에서 생성한 TeachingDataManager를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  TeachingDataManager 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
TeachingDataManager* TOHTMainForm::GetTeachingDataManager()
{
    if(this->m_TeachingDataManager == NULL)
        return NULL;
    else
        return this->m_TeachingDataManager;
}

//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 CIDManager_STD DataManager를 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  m_CIDDataManager 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
CIDManager_STD* TOHTMainForm::GetCIDDataManager_STD()
{
    if(this->m_CIDManager_STD == NULL)
        return NULL;
    else
        return this->m_CIDManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 CollisionManager_STD DataManager를 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  m_CollisionManager_STD 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
CollisionManager_STD* TOHTMainForm::GetCollisionDataManager_STD()
{
    if(this->m_CollisionManager_STD == NULL)
        return NULL;
    else
        return this->m_CollisionManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 FlowControlManager_STD DataManager를 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  m_FlowControlManager_STD 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
FlowControlManager_STD* TOHTMainForm::GetFlowControlDataManager_STD()
{
    if(this->m_FlowControlManager_STD == NULL)
        return NULL;
    else
        return this->m_FlowControlManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 m_MapManager_STD를 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  m_MapManager_STD 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
MapDataManager_STD* TOHTMainForm::GetMapDataManager_STD()
{
    if(this->m_MapManager_STD == NULL)
        return NULL;
    else
        return this->m_MapManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 m_StationManager_STD를 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  m_StationManager_STD 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
StationManager_STD* TOHTMainForm::GetStationDataManager_STD()
{
    if(this->m_StationManager_STD == NULL)
        return NULL;
    else
        return this->m_StationManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 m_TeachingManager_STD를 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  m_TeachingManager_STD 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
TeachingManager_STD* TOHTMainForm::GetTeachingDataManager_STD()
{
    if(this->m_TeachingManager_STD == NULL)
        return NULL;
    else
        return this->m_TeachingManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 m_PathDataManager_STD를 리턴하는 함수
@author  KJD
@date    2016.04.20
@return  m_PathDataManager_STD 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
PathDataManager_STD* TOHTMainForm::GetPathDataManager_STD()
{
	if(this->m_PathDataManager_STD == NULL)
		return NULL;
	else
		return this->m_PathDataManager_STD;
}


/**
@brief   OHTMain에서 생성한 m_PathDataManager_STD를 리턴하는 함수
@author  puting
@date    2016.12.20
@return  m_PathDataManager_STD 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
MapMakerFileManager_STD* TOHTMainForm::GetMapMakerFileManager_STD()
{
	if(this->m_MapMakerFileManager_STD == NULL)
		return NULL;
	else
		return this->m_MapMakerFileManager_STD;
}
//---------------------------------------------------------------------------
DEFAULT_PARAM_SET* TOHTMainForm::GetDefaultParam()
{
	if(this->m_pParamSet == NULL)
		return NULL;
	else
		return this->m_pParamSet;
}
/**
@brief   OHTMain의 Map을 변경하는 함수
@author  doori.shin
@date    2015.03.08
@param  Map : 변경할 Map 데이터
*/

void TOHTMainForm::SetMap(MAP* mapData)
{
	this->m_MapData = mapData;
}

//---------------------------------------------------------------------------
void TOHTMainForm::SetCID_STD(STD_CID* CIDData)
{
	this->m_CIDData_STD = CIDData;
}
void TOHTMainForm::SetCollision_STD(STD_Collision* CollisionData)
{
	this->m_CollisionData_STD = CollisionData;
}
void TOHTMainForm::SetFlowControl_STD(STD_FlowControl* FlowControlData)
{
	this->m_FlowControlData_STD = FlowControlData;
}
void TOHTMainForm::SetMap_STD(STD_MAP* MapData)
{
	this->m_MapData_STD = MapData;
}
void TOHTMainForm::SetStation_STD(STD_Station* StationData)
{
 	this->m_StationData_STD = StationData;
}
void TOHTMainForm::SetTeaching_STD(STD_Teaching* TeachingData)
{
	this->m_TeachingData_STD = TeachingData;
}
void TOHTMainForm::SetPathData_STD(STD_PathData* PathData)
{
	this->m_PathData_STD = PathData;
}


void TOHTMainForm::SetCIDDataManager_STD(CIDManager_STD* CIDDataManager)
{
	this->m_CIDManager_STD = CIDDataManager;
}
void TOHTMainForm::SetCollisionDataManager_STD(CollisionManager_STD* CollisionDataManager)
{
	this->m_CollisionManager_STD = CollisionDataManager;
}
void TOHTMainForm::SetFlowControlDataManager_STD(FlowControlManager_STD* FlowControlDataManager)
{
	this->m_FlowControlManager_STD = FlowControlDataManager;
}
void TOHTMainForm::SetapDataManager_STD(MapDataManager_STD* MapDataManager)
{
	this->m_MapManager_STD = MapDataManager;
}
void TOHTMainForm::SetStationDataManager_STD(StationManager_STD* StationDataManager)
{
	this->m_StationManager_STD = StationDataManager;
}
void TOHTMainForm::SetTeachingDataManager_STD(TeachingManager_STD* TeachingDataManager)
{
	this->m_TeachingManager_STD = TeachingDataManager;
}
void TOHTMainForm::SetPathDataManager_STD(PathDataManager_STD* PathDataManager)
{
	this->m_PathDataManager_STD = PathDataManager;
}
void TOHTMainForm::SetMapMakerFileManager_STD(MapMakerFileManager_STD* MapMakerFileData)
{
	this->m_MapMakerFileManager_STD = MapMakerFileData;
}

/**
@brief   OHTMain의 MapDataManager를 변경하는 함수
@author  doori.shin
@date    2015.03.08
@param  MapDataManager : 변경할 데이터
*/

void TOHTMainForm::SetMapDataManager(MapDataManager* mapDataManager)
{
	this->m_MapDataManager = mapDataManager;
}

//---------------------------------------------------------------------------

/**
@brief   OHTMain의 StationDataManager을 변경하는 함수
@author  doori.shin
@date    2015.03.08
@param  StationDataManager : 변경할 데이터
*/

void TOHTMainForm::SetStationDataManager(StationDataManager* stationDataManager)
{
	this->m_StationDataManager = stationDataManager;
}

//---------------------------------------------------------------------------

/**
@brief   OHTMain의 TeachingDataManager를 변경하는 함수
@author  doori.shin
@date    2015.03.08
@param  TeachingDataManager : 변경할 데이터
*/

void TOHTMainForm::SetTeachingDataManager(TeachingDataManager* teachingDataManager)
{
	this->m_TeachingDataManager = teachingDataManager;
}


//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 CmdManager를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  CmdManager 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
CmdManager* TOHTMainForm::GetCmdManager()
{
	if(this->m_CmdManager == NULL)
		return NULL;
	else
		return this->m_CmdManager;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain에서 생성한 PassPermit를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  CmdManager 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
PassPermit* TOHTMainForm::GetPassPermit()
{
	if(this->m_PassPermit == NULL)
		return NULL;
	else
		return this->m_PassPermit;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 CmdManager를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  CmdManager 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
PathSearch* TOHTMainForm::GetPathSearch()
{
	if(this->m_PathSearch == NULL)
		return NULL;
	else
		return this->m_PathSearch;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain에서 생성한 HWCommon를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  HWCommon 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
HWCommon* TOHTMainForm::GetHWCommon()
{
    if(this->m_HWCommon == NULL)
        return NULL;
    else
        return this->m_HWCommon;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 HWSet를 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  HWSet 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
HWSet* TOHTMainForm::GetHWSet()
{
    if(this->m_HWSet == NULL)
        return NULL;
    else
        return this->m_HWSet;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 DrivingControl을 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  DrivingControl 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
DrivingControl* TOHTMainForm::GetDrivingControl()
{
    if(this->m_DrivingControl == NULL)
        return NULL;
    else
        return this->m_DrivingControl;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 TransControl을 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  TransControl 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
TransControl* TOHTMainForm::GetTransControl()
{
    if(this->m_TransControl == NULL)
        return NULL;
    else
        return this->m_TransControl;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain에서 생성한 ManualControl을 리턴하는 함수
@author  zzang9un
@date    2014.02.06
@return  ManualControl 포인터, 생성이 되지 않은 경우 Null을 리턴
*/
ManualControl* TOHTMainForm::GetManualControl()
{
    if(this->m_ManualControl == NULL)
        return NULL;
    else
        return this->m_ManualControl;
}
//---------------------------------------------------------------------------

/**
@brief   Main 프로그램의 버전 정보를 리턴해주는 함수
@author  zzang9un
@date    2014.02.06
@return  uVERSION_DATA 구조체
*/
uVERSION_DATA TOHTMainForm::GetOHTProgramVersion()
{
	uVERSION_DATA rtnVersion;

    rtnVersion.Info.Major = this->VerInfo.Major;
	rtnVersion.Info.Minor = this->VerInfo.Minor;
    rtnVersion.Info.Release = this->VerInfo.Release;
    rtnVersion.Info.Build = this->VerInfo.Build;

    return rtnVersion;
}
//---------------------------------------------------------------------------
void __fastcall TOHTMainForm::tmMinimizerTimer(TObject *Sender)
{
	Application->Minimize();
	tmMinimizer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TOHTMainForm::OnRestore(TObject *Sender)
{
	tmMinimizer->Enabled = true;
}


void TOHTMainForm::Set_LogFolder(bool bflag)
{
	if(gm_OHTMainStatus!=NULL)
	{
		gm_OHTMainStatus->StatusTP.isLogDrive = bflag;
	}			
}
//---------------------------------------------------------------------------
/**
@brief	Thread 상태를 확인하는 타이머
@author	doori.shin
@date	2014.04.15
*/
void __fastcall TOHTMainForm::tmChkThreadStatusTimer(TObject *Sender)
{
	AnsiString strLog = "";
	BYTE retValue = 0x00;

	//우선순위 높은 Thread부터 Check
	//Alive(A), Dead(D), Loop(L)
	//1. HWControlThread(H)
	retValue = CheckThreadStatus(m_HWControlThread, m_nPreHWCtrlThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog = "H:" + GetThreadStatusToString(retValue) + " ";
	}
	//2. ExecuteThread(E)
	retValue = CheckThreadStatus(m_ExecuteThread, m_nPreExecuteThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog += "E:" + GetThreadStatusToString(retValue) + " ";
	}
//	strLog += ",E:";
//	strLog += GetThreadStatusToString(CheckThreadStatus(m_ExecuteThread, m_nPreExecuteThTick));
	//3. TaskControlThread(T)
	retValue = CheckThreadStatus(m_TaskControlThread, m_nPreTaskCtrlThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog += "T:" + GetThreadStatusToString(retValue) + " ";
	}
//	strLog += ",T:";
//	strLog += GetThreadStatusToString(CheckThreadStatus(m_TaskControlThread, m_nPreTaskCtrlThTick));
	//4. ExtraJobThread(J)
	retValue = CheckThreadStatus(m_ExtraJobThread, m_nPreExtraJobThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog += "J:" + GetThreadStatusToString(retValue) + " ";
	}
//	strLog += ",J:";
//	strLog += GetThreadStatusToString(CheckThreadStatus(m_ExtraJobThread, m_nPreExtraJobThTick));
	//5. StatusCheckThread(S)
	retValue = CheckThreadStatus(m_StatusCheckThread, m_nPreStatusChkThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog += "S:" + GetThreadStatusToString(retValue) + " ";
	}
//	strLog += ",S:";
//	strLog += GetThreadStatusToString(CheckThreadStatus(m_StatusCheckThread, m_nPreStatusChkThTick));
	//6. LogThread(L)
	retValue = CheckThreadStatus(m_LogThread, m_nPreLogThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog += "L:" + GetThreadStatusToString(retValue) + " ";
	}
//	strLog += ",L:";
//	strLog += GetThreadStatusToString(CheckThreadStatus(m_LogThread, m_nPreLogThTick));

	//로그 기록
	if(strLog.Length() > 0)
		THREAD_STATUS_LOG(strLog.c_str());
}
//---------------------------------------------------------------------------

/**
@brief	Thread 실행여부 확인
@author	doori.shin
@date	2014.04.15
@param	thread : 확인하려는 Thread
@param	tick : 확인하려는 Thread의 이전 tick값
@return	Thread 상태값
		- THREAD_ALIVE : 동작중
		- THREAD_STOP :클래스는 살아있으나 Thread가 종료됨(다시 실행가능)
		- THREAD_DESTROY : 클래스가 종료됨(NULL값 또는 예외상황, 다시 실행불가능)
		- THREAD_UNKNOWN : 초기상태값
*/
inline BYTE TOHTMainForm::CheckThreadStatus(ThreadCtrl* thread, int& tick)
{
	BYTE result = THREAD_STATUS_UNKNOWN;

	try
	{
		if(tick == thread->getTick())//thread is stopped
		{
			result = THREAD_STATUS_STOP;
			//Thread를 다시 실행할 수 있다.
			//thread->beginThread();
		}
		else //thread is running
			result = THREAD_STATUS_ALIVE;

		tick = thread->getTick();
	}
	catch(Exception& e)
	{
		//thread is dead
		result = THREAD_STATUS_DESTROY;
	}

	return result;
}
//---------------------------------------------------------------------------
/**
@brief	Thread 상태값을 String으로 리턴
@author	doori.shin
@date	2014.04.15
@param	status : Thread상태값
@return	String으로 변환된 상태값
*/
inline String TOHTMainForm::GetThreadStatusToString(BYTE status)
{
	String result = L"";

	switch(status)
	{
	case THREAD_STATUS_ALIVE:
		result = L"A";
		break;
	case THREAD_STATUS_STOP:
		result = L"S";
		break;
	case THREAD_STATUS_DESTROY:
		result = L"D";
		break;
	case THREAD_STATUS_UNKNOWN:
		result = L"U";
		break;
	}

	return result;
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	static bool bCritical = true;

	if(bCritical == false)
		return;

	bCritical = false; 	// 임계영역 시작
	/// 주의!! bCiritical 이 false로 바꼈기 때문에 함수 중간에서 return 하는 일이 없도록해야함

	if(m_bErrIotGateway)
	{
		CanClose = true;
		RapidLog(MAIN_EXIT_LOG, "======== OHT MAIN PROGRAM TERMINATING  -  IOT Gateway Error =======");
        bCritical = true;
		return;
	}else if(m_bErrOHTTypeMatching)
	{
		CanClose = true;
		RapidLog(MAIN_EXIT_LOG, "======== OHT MAIN PROGRAM TERMINATING  -  OHT Type Error =======");
        bCritical = true;
		return;
    }

	RapidLog(MAIN_EXIT_LOG, "FormCloseQuery Enter");

	if(gm_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL)
	{
		String strTemp = "";
		strTemp.sprintf(L"정말 %c%c%c%c%c%c 종료 하시겠습니까?", m_pParamSet->VHLName[0],m_pParamSet->VHLName[1],m_pParamSet->VHLName[2],m_pParamSet->VHLName[3],m_pParamSet->VHLName[4],m_pParamSet->VHLName[5]);

		RapidLog(MAIN_EXIT_LOG, "Try Exit - manual mode");

		//if(ID_YES != MessageBox(this->Handle, strTemp.w_str(), L"경고",  MB_YESNO))
		if(MessageDlg(strTemp.c_str(), mtConfirmation, TMsgDlgButtons() << mbOK << mbCancel, 0) != mrOk)
		{
			RapidLog(MAIN_EXIT_LOG, "Close Cancled");
			CanClose = false;
		}
		else
		{
            RapidLog(MAIN_EXIT_LOG, "======== OHT MAIN PROGRAM TERMINATING  -  MANUAL MODE =======");
			CanClose = true;


		}
	}
	else
	{
		//MessageBox(NULL,L"Manual Mode로 전환 후 종료해주세요.",L"경고",NULL);
		RapidLog(MAIN_EXIT_LOG, "FormCloseQuery_Interlock");
		ADD_MEMO_T("Manual Mode로 전환 후 종료해주세요.");
		CanClose = false;
	}

	bCritical = true;   // 임계영역 해제
}
//---------------------------------------------------------------------------
//혼용 PIO//
void TOHTMainForm::Set_EQPIO_Type(int nEqPioType)
{
	m_nEQPIO_Type = nEqPioType;
	return;
}

int TOHTMainForm::Get_EQPIO_Type()
{
	return m_nEQPIO_Type;
}


//---------------------------------------------------------------------------
bool TOHTMainForm::LoadParameter(char* szFilePath, DEFAULT_PARAM_SET* pParamSet)
{
	bool bSuccess = false;
	int nTmpValue = 0;
    double dTmpValue = 0.0;
	bool bTmpValue = false;
	double dResult = 0.0;
	char strTempValue[30]= {0,};

    LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile(szFilePath);

	//기어비 초기화

	if(!pIniFile->GetIniFileExist())
    {
		delete pIniFile;
        pIniFile = NULL;
    }

	if(pIniFile != NULL)
	{

		pIniFile->ReadString("OHTInfo","MachineName", "V00001", strTempValue);
		memmove(pParamSet->VHLName, strTempValue, sizeof(pParamSet->VHLName)-1); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		pIniFile->ReadString("OHTInfo","BroadCastMask", "192.167.255.255", strTempValue);
		memmove(pParamSet->BroadCasMask, strTempValue, sizeof(pParamSet->BroadCasMask)-1); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		pIniFile->ReadInteger("OHTInfo", "PortNumAdd", 0, &nTmpValue);
		pParamSet->nPortNumAdd = nTmpValue;

		pIniFile->ReadString("OHTInfo","OHTType", "00000", strTempValue);
		memmove(pParamSet->OHTType, strTempValue, sizeof(pParamSet->OHTType)-1); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		pIniFile->ReadString("OHTInfo","LineType", "00000", strTempValue);
		memmove(pParamSet->LineType, strTempValue, sizeof(pParamSet->LineType)-1); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		pIniFile->ReadInteger("OHTInfo", "IsLCDAndLED", 0, &nTmpValue); //add P1L VS P1_2L & P1_3L 구분 파라미터 #JRespect 2018.01.09
		pParamSet->IsLCDAndLED = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("OHTInfo", "HoistHomeSensorOriginCountAdd", 0, 0, 4, &nTmpValue); //add P1L VS P1_2L & P1_3L 구분 파라미터 #JRespect 2018.01.09
		pParamSet->nHoistHomeSensorOriginConutAdd = nTmpValue;

		pIniFile->ReadInteger("OHTInfo", "bHoistHomeSensorOriginRetry", 0, &nTmpValue); //add P1L VS P1_2L & P1_3L 구분 파라미터 #JRespect 2018.01.09
		pParamSet->bHoistHomeSensorOriginRetry = nTmpValue;


		pIniFile->ReadInteger("OHTInfo", "IsIOTHub", 0, &nTmpValue); //add P1L VS P1_2L & P1_3L 구분 파라미터 #JRespect 2018.01.09
		pParamSet->IsIOTHub = nTmpValue;

		pIniFile->ReadInteger("OHTInfo", "VHLLifterPassEnable", 0, &nTmpValue); //AOHC-217 VHLLifter 속도 개선
		pParamSet->VHLLifterPassEnable = nTmpValue;

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
		pIniFile->ReadInteger("OHTInfo", "IsRotateSensor", 0, &nTmpValue); //0이 센서 인터락 미사용, 1이 센서인터락 사용
		pParamSet->IsRotateSensor = nTmpValue;
#endif
		//2017.01.04 puting 오토티칭 지그 IP 획득
		pIniFile->ReadString("AUTOTEACHING","IpAddress", "192.168.200.165", strTempValue);
		memmove(&(pParamSet->AutoTeachingIP), strTempValue, strlen(strTempValue)); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)||(OHT_NAME == OHT_NAME_STD_V85))
		pIniFile->ReadInteger("PRETRANS", "PreEQPIO", 0, &nTmpValue);
		pParamSet->PreEQPIO = nTmpValue;
        pParamSet->PreShutter =0;
#else
		pIniFile->ReadInteger("PRETRANS", "PreEQPIO", 0, &nTmpValue);
		pParamSet->PreEQPIO = nTmpValue;

		pIniFile->ReadInteger("PRETRANS", "PreShutter", 0, &nTmpValue);
		pParamSet->PreShutter = nTmpValue;
#endif
        //2016.12.20 sh17.jo 무선 네트워크 상태 점검 관련 파라미터 추가
 		pIniFile->ReadString("WIRELESS","GatewayIP", "192.168.0.1", strTempValue);
		memmove(&(pParamSet->GatewayIP), strTempValue, strlen(strTempValue)); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		//2018.08.21 shcho : OCS의 gateway 주소. OCS 연결 불량시 Ping 시도하기 위함
		memset(pParamSet->GatewayIP_OCS, 0, sizeof(pParamSet->GatewayIP_OCS));
		pIniFile->ReadString("WIRELESS","GatewayIP_OCS", "192.167.0.1", strTempValue);
		memmove(&(pParamSet->GatewayIP_OCS), strTempValue, strlen(strTempValue)); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));
		if(strlen(pParamSet->GatewayIP_OCS) < 7)	// 제대로 된 IP 값이 안들어 갔을 경우 default 지정
		{
			sprintf(pParamSet->GatewayIP_OCS, "192.167.0.1");
		}

        pIniFile->ReadString("WIRELESS","IpAddress", "192.168.110.1", strTempValue);
		memmove(&(pParamSet->WirelessClientIP), strTempValue, strlen(strTempValue)); //Return Value
        memset(strTempValue, 0, sizeof(strTempValue));

        pIniFile->ReadString("WIRELESS","UserID", "admin", strTempValue);
		memmove(&(pParamSet->UserID), strTempValue, strlen(strTempValue)); //Return Value
        memset(strTempValue, 0, sizeof(strTempValue));

        pIniFile->ReadString("WIRELESS","Password", "pass", strTempValue);
		memmove(&(pParamSet->Password), strTempValue, strlen(strTempValue)); //Return Value
        memset(strTempValue, 0, sizeof(strTempValue));

        pIniFile->ReadInteger("WIRELESS", "MaxOCSRcvTime", 25000, &nTmpValue);
        pParamSet->nMaxOCSRcvTime = nTmpValue;

        pIniFile->ReadInteger("WIRELESS", "MaxPingCheckTime_Step1", 40000, &nTmpValue);
        pParamSet->nMaxPingCheckTime_Step1= nTmpValue;

        pIniFile->ReadInteger("WIRELESS", "MaxPingCheckTime_Step2", 100000, &nTmpValue);
        pParamSet->nMaxPingCheckTime_Step2 = nTmpValue;

        pIniFile->ReadInteger("WIRELESS", "MaxPingCheckTime_Step3", 160000, &nTmpValue);
        pParamSet->nMaxPingCheckTime_Step3 = nTmpValue;

        pIniFile->ReadInteger("WIRELESS", "MaxPingCheckTime_ETC", 20000, &nTmpValue);
        pParamSet->nMaxPingCheckTime_ETC = nTmpValue;
		pParamSet->bIgnoreIoTLanCheck = false;
		pIniFile->ReadInteger("WIRELESS", "IOT_Lan_Check_Ignore", 0, &nTmpValue);
		pParamSet->bIgnoreIoTLanCheck = nTmpValue;

		pIniFile->ReadString("WIRELESS","IOT_EternetName", "", strTempValue);
		memmove(&(pParamSet->IotEternetName), strTempValue, strlen(strTempValue));
		memset(strTempValue, 0, sizeof(strTempValue));


		pIniFile->ReadString("DiagAgentInfo","IPAddress", "", strTempValue);
		pParamSet->strDiagIpAddr.sprintf("%s", strTempValue);


		pIniFile->ReadInteger("DiagAgentInfo", "Port", 0.0, &nTmpValue);
		pParamSet->nDiagPort = nTmpValue;



		// 1. driving PARAM
		//pIniFile->ReadDoubleMinCheck("DRIVING","IdealGearRatio", DRIVING_GEAR_RATIO, 0.1, &dTmpValue);
		//pParamSet->DrivingDefault.dIdealGearRatio = dTmpValue; //Return Value
		////기어비 5.5:1로 계산됨.(실제 5.555:1) = 114.7341619

		//pIniFile->ReadDoubleMinCheck("DRIVING","Ideal2RealRatio", 1.0, 0.8, &dTmpValue);
		//pParamSet->DrivingDefault.dIdeal2RealRatio = dTmpValue;

		pParamSet->DrivingDefault.dIdealGearRatio = DRIVING_GEAR_RATIO;
		pParamSet->DrivingDefault.dIdeal2RealRatio = 1.0;

		 // 1. HOIST PARAM
		pIniFile->ReadDouble("HOIST", "dPosition0_pulse", -500.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[0].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition0_User", -1.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[0].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition1_pulse", 20000.0, &dTmpValue);   	//STB
		pParamSet->HoistDefault.PositionValue[1].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition1_User", 106.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[1].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition2_pulse", 130000.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[2].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition2_User", 672.0, &dTmpValue);      	//STK
		pParamSet->HoistDefault.PositionValue[2].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition3_pulse", 350000.0, &dTmpValue);    //Centor
		pParamSet->HoistDefault.PositionValue[3].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition3_User", 1757.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[3].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition4_pulse", 450000.0, &dTmpValue);    //Centor ~ EQ Start 사이값
		pParamSet->HoistDefault.PositionValue[4].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition4_User", 2185.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[4].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition5_pulse", 620000.0, &dTmpValue);  //EQ Start
		pParamSet->HoistDefault.PositionValue[5].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition5_User", 3012.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[5].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition6_pulse", 800000.0, &dTmpValue);  //EQ End
		pParamSet->HoistDefault.PositionValue[6].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition6_User", 3797.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[6].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition7_pulse", 950000.0, &dTmpValue);  //EQ End
		pParamSet->HoistDefault.PositionValue[7].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition7_User", 4424.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[7].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition8_pulse", 1100000.0, &dTmpValue);  //EQ End
		pParamSet->HoistDefault.PositionValue[8].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition8_User", 5021.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[8].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition9_pulse", 1230000.0, &dTmpValue);  //EQ End
		pParamSet->HoistDefault.PositionValue[9].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition9_User", 5517.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[9].dUserUnit = dTmpValue;



		pIniFile->ReadDouble("HOIST", "dHomeOffsetOfRailToJigOnUserUnit", 0.0, &dTmpValue);
		pParamSet->HoistDefault.dDeviceDifference = dTmpValue;

		pIniFile->ReadInteger("HOIST", "dHomeSearchOffset", 0.0, &nTmpValue);
		pParamSet->HoistDefault.nHomeSearchCtsOffset = nTmpValue;



		// 2. SHIFT PARAM
		//pIniFile->ReadDoubleMinCheck("SHIFT", "dIdealGearRatio", SHIFT_IDEALGEARRATIO, 0.1, &dTmpValue);
		//pParamSet->ShiftDefault.dIdealGearRatio = dTmpValue;

		//pIniFile->ReadDoubleMinCheck("SHIFT", "dIdeal2RealRatio", 1.0, 0.8, &dTmpValue);
		//pParamSet->ShiftDefault.dIdeal2RealRatio = dTmpValue;

		//pIniFile->ReadDouble("SHIFT", "dHomeOffsetOfMTLToJigOnUserUnit", 0.0, &dTmpValue);
		//pParamSet->ShiftDefault.dDeviceDifference = dTmpValue;

		//pIniFile->ReadInteger("SHIFT", "dHomeSearchOffset", 0.0, &nTmpValue);
		//pParamSet->ShiftDefault.nHomeSearchCtsOffset = nTmpValue;

		pParamSet->ShiftDefault.dIdealGearRatio = SHIFT_IDEALGEARRATIO;
		pParamSet->ShiftDefault.dIdeal2RealRatio = 1.0;
		pParamSet->ShiftDefault.dDeviceDifference = 0.000000;
		pParamSet->ShiftDefault.nHomeSearchCtsOffset = 0;


		pIniFile->ReadIntegerMinMaxCheck("ROTATE","nHomeOffsetOfMTLToJigOnUserUnit", 0, -20, 20, &nTmpValue);
		pParamSet->RotateDefault.nDeviceDifference = nTmpValue;
		pIniFile->ReadIntegerMinMaxCheck("ROTATE","nHomeOffsetOfMTLToJigOnUserUnit_90", 0, -20, 20, &nTmpValue);
		pParamSet->RotateDefault.nDeviceDifference_90 = nTmpValue;
		pIniFile->ReadIntegerMinMaxCheck("ROTATE","nHomeOffsetOfMTLToJigOnUserUnit_180", 0, -20, 20, &nTmpValue);
		pParamSet->RotateDefault.nDeviceDifference_180 = nTmpValue;

		delete pIniFile;
		bSuccess = true;

	}
	else
	{
        //2016.12.20 sh17.jo 무선 네트워크 상태 점검 관련 파라미터 추가
        strcpy(pParamSet->VHLName, "V00001");
        strcpy(pParamSet->BroadCasMask, "192.167.255.255");
        strcpy(pParamSet->GatewayIP, "192.168.0.1");
		strcpy(pParamSet->WirelessClientIP, "192.168.110.1");
		strcpy(pParamSet->UserID, "admin");
		strcpy(pParamSet->Password, "pass");

		strcpy(pParamSet->OHTType, "00000");
		strcpy(pParamSet->LineType, "00000");
        pParamSet->nMaxOCSRcvTime = 25000;
		pParamSet->nMaxPingCheckTime_Step1 =  40000;
        pParamSet->nMaxPingCheckTime_Step2 = 100000;
        pParamSet->nMaxPingCheckTime_Step3 = 160000;
        pParamSet->nMaxPingCheckTime_ETC   =  20000;

		strcpy(pParamSet->AutoTeachingIP, "192.168.200.165");

		pParamSet->PreEQPIO = 0;
		pParamSet->PreShutter = 0;
		pParamSet->nPortNumAdd = 0;
		pParamSet->IsLCDAndLED = 0;
		pParamSet->nHoistHomeSensorOriginConutAdd = 0;
		pParamSet->bHoistHomeSensorOriginRetry = 0;
		pParamSet->IsIOTHub = 0;
		pParamSet->VHLLifterPassEnable=0;
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
		pParamSet->IsRotateSensor = 0;
#endif

		pParamSet->DrivingDefault.dIdealGearRatio = DRIVING_GEAR_RATIO; //Return Value
		pParamSet->DrivingDefault.dIdeal2RealRatio = 1.0;


		pParamSet->HoistDefault.PositionValue[0].dPulse = -500.0;
		pParamSet->HoistDefault.PositionValue[0].dUserUnit = -2.0;
		pParamSet->HoistDefault.PositionValue[1].dPulse = 20000.0;
		pParamSet->HoistDefault.PositionValue[1].dUserUnit = 100.0;
		pParamSet->HoistDefault.PositionValue[2].dPulse = 128000.0;
		pParamSet->HoistDefault.PositionValue[2].dUserUnit = 637.0;
		pParamSet->HoistDefault.PositionValue[3].dPulse = 300000.0;
		pParamSet->HoistDefault.PositionValue[3].dUserUnit = 1460.0;
		pParamSet->HoistDefault.PositionValue[4].dPulse = 450000.0;
		pParamSet->HoistDefault.PositionValue[4].dUserUnit = 2153.0;
		pParamSet->HoistDefault.PositionValue[5].dPulse = 620000.0;
		pParamSet->HoistDefault.PositionValue[5].dUserUnit = 2901.0;
		pParamSet->HoistDefault.PositionValue[6].dPulse = 780000.0;
		pParamSet->HoistDefault.PositionValue[6].dUserUnit = 3579.0;
		pParamSet->HoistDefault.PositionValue[7].dPulse = 940000.0;
		pParamSet->HoistDefault.PositionValue[7].dUserUnit = 4225.0;
		pParamSet->HoistDefault.PositionValue[8].dPulse = 1110000.0;
		pParamSet->HoistDefault.PositionValue[8].dUserUnit = 4881.0;
		pParamSet->HoistDefault.PositionValue[9].dPulse = 1290000.0;
		pParamSet->HoistDefault.PositionValue[9].dUserUnit = 5556.0;

		pParamSet->HoistDefault.dDeviceDifference = 0.0;
		pParamSet->HoistDefault.nHomeSearchCtsOffset = 0.0;

		pParamSet->ShiftDefault.dIdealGearRatio = SHIFT_IDEALGEARRATIO;
		pParamSet->ShiftDefault.dIdeal2RealRatio = 1.0;
		pParamSet->ShiftDefault.dDeviceDifference = 0.0;
		pParamSet->ShiftDefault.nHomeSearchCtsOffset = 0.0;

		pParamSet->RotateDefault.nDeviceDifference = 0;
		pParamSet->RotateDefault.nDeviceDifference_90 = 0;
		pParamSet->RotateDefault.nDeviceDifference_180 = 0;



	}

	return bSuccess;
}
void TOHTMainForm::HoistParam_Read(int iIndex, double &dbl_Pluse, double &dbl_User)
{
	dbl_Pluse = m_pParamSet->HoistDefault.PositionValue[iIndex].dPulse;
	dbl_User = m_pParamSet->HoistDefault.PositionValue[iIndex].dUserUnit;

	return;
}
void TOHTMainForm::DeviceDifference_Read(double &dbl_Pluse)
{
	dbl_Pluse = m_pParamSet->HoistDefault.dDeviceDifference;

	return;
}

bool TOHTMainForm::HoistParamSaveParameter(PKT_TP2OHT_HOIST_TEACH_COMMAND PKT)
{
	bool bSuccess = false;
	
	LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile("DataFiles\\OHTPARAM.ini");	

	if(!pIniFile->GetIniFileExist())
	{
		delete pIniFile;
		pIniFile = NULL;
	}

	if(pIniFile != NULL)		
	{
		m_pParamSet->HoistDefault.dDeviceDifference = PKT.BODY.dHomeOffsetOfRailToJigOnUserUnit;
#ifdef OHT_HIGH_RAIL_HOIST_PRAM
		for(int i = 0; i < 10; i++)
		{
			m_pParamSet->HoistDefault.PositionValue[i].dPulse = PKT.BODY.PositionValue[i].dPulse;
			m_pParamSet->HoistDefault.PositionValue[i].dUserUnit = PKT.BODY.PositionValue[i].dUserUnit;
		}
		 // 1. HOIST PARAM
		pIniFile->WriteDouble("HOIST", "dPosition0_pulse", m_pParamSet->HoistDefault.PositionValue[0].dPulse);
		pIniFile->WriteDouble("HOIST", "dPosition0_User", m_pParamSet->HoistDefault.PositionValue[0].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition1_pulse", m_pParamSet->HoistDefault.PositionValue[1].dPulse); 	//STB
		pIniFile->WriteDouble("HOIST", "dPosition1_User",  m_pParamSet->HoistDefault.PositionValue[1].dUserUnit);		

		pIniFile->WriteDouble("HOIST", "dPosition2_pulse", m_pParamSet->HoistDefault.PositionValue[2].dPulse);
		pIniFile->WriteDouble("HOIST", "dPosition2_User", m_pParamSet->HoistDefault.PositionValue[2].dUserUnit);		//STK		

		pIniFile->WriteDouble("HOIST", "dPosition3_pulse",m_pParamSet->HoistDefault.PositionValue[3].dPulse);	//Centor
		pIniFile->WriteDouble("HOIST", "dPosition3_User", m_pParamSet->HoistDefault.PositionValue[3].dUserUnit);		

		pIniFile->WriteDouble("HOIST", "dPosition4_pulse", m_pParamSet->HoistDefault.PositionValue[4].dPulse);	//Centor ~ EQ Start 사이값
		pIniFile->WriteDouble("HOIST", "dPosition4_User", m_pParamSet->HoistDefault.PositionValue[4].dUserUnit);		

		pIniFile->WriteDouble("HOIST", "dPosition5_pulse", m_pParamSet->HoistDefault.PositionValue[5].dPulse);  //EQ Start
		pIniFile->WriteDouble("HOIST", "dPosition5_User", m_pParamSet->HoistDefault.PositionValue[5].dUserUnit);		

		pIniFile->WriteDouble("HOIST", "dPosition6_pulse", m_pParamSet->HoistDefault.PositionValue[6].dPulse);  //EQ End
		pIniFile->WriteDouble("HOIST", "dPosition6_User",  m_pParamSet->HoistDefault.PositionValue[6].dUserUnit);	 

		pIniFile->WriteDouble("HOIST", "dPosition7_pulse", m_pParamSet->HoistDefault.PositionValue[7].dPulse);  //EQ End
		pIniFile->WriteDouble("HOIST", "dPosition7_User",  m_pParamSet->HoistDefault.PositionValue[7].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition8_pulse", m_pParamSet->HoistDefault.PositionValue[8].dPulse);  //EQ End
		pIniFile->WriteDouble("HOIST", "dPosition8_User",  m_pParamSet->HoistDefault.PositionValue[8].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition9_pulse", m_pParamSet->HoistDefault.PositionValue[9].dPulse);  //EQ End
		pIniFile->WriteDouble("HOIST", "dPosition9_User",  m_pParamSet->HoistDefault.PositionValue[9].dUserUnit);

#else
		for(int i = 0; i < 7; i++)
		{
			m_pParamSet->HoistDefault.PositionValue[i].dPulse = PKT.BODY.PositionValue[i].dPulse;
			m_pParamSet->HoistDefault.PositionValue[i].dUserUnit = PKT.BODY.PositionValue[i].dUserUnit;
		}
		 // 1. HOIST PARAM
		pIniFile->WriteDouble("HOIST", "dPosition0_pulse", m_pParamSet->HoistDefault.PositionValue[0].dPulse);
		pIniFile->WriteDouble("HOIST", "dPosition0_User", m_pParamSet->HoistDefault.PositionValue[0].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition1_pulse", m_pParamSet->HoistDefault.PositionValue[1].dPulse); 	//STB
		pIniFile->WriteDouble("HOIST", "dPosition1_User",  m_pParamSet->HoistDefault.PositionValue[1].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition2_pulse", m_pParamSet->HoistDefault.PositionValue[2].dPulse);
		pIniFile->WriteDouble("HOIST", "dPosition2_User", m_pParamSet->HoistDefault.PositionValue[2].dUserUnit);		//STK

		pIniFile->WriteDouble("HOIST", "dPosition3_pulse",m_pParamSet->HoistDefault.PositionValue[3].dPulse);	//Centor
		pIniFile->WriteDouble("HOIST", "dPosition3_User", m_pParamSet->HoistDefault.PositionValue[3].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition4_pulse", m_pParamSet->HoistDefault.PositionValue[4].dPulse);	//Centor ~ EQ Start 사이값
		pIniFile->WriteDouble("HOIST", "dPosition4_User", m_pParamSet->HoistDefault.PositionValue[4].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition5_pulse", m_pParamSet->HoistDefault.PositionValue[5].dPulse);  //EQ Start
		pIniFile->WriteDouble("HOIST", "dPosition5_User", m_pParamSet->HoistDefault.PositionValue[5].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition6_pulse", m_pParamSet->HoistDefault.PositionValue[6].dPulse);  //EQ End
		pIniFile->WriteDouble("HOIST", "dPosition6_User",  m_pParamSet->HoistDefault.PositionValue[6].dUserUnit);
#endif

		pIniFile->WriteDouble("HOIST", "dHomeOffsetOfRailToJigOnUserUnit",m_pParamSet->HoistDefault.dDeviceDifference);

		delete pIniFile;

		m_HWSet->m_pHoistAxis->SetHoistParam(m_pParamSet->HoistDefault);

		bSuccess = true;
	}
	
	return bSuccess;
}

void TOHTMainForm::ForceClose(void)
{
	RapidLog(MAIN_EXIT_LOG, "======== OHT MAIN PROGRAM TERMINATING  -  FORCE CLOSE =======");

	tmUIupdate->Enabled = false;

	DeleteCriticalSection(&m_CS);

	DestroyClass();

	Sleep(100);

	exit(0);
}




StatusCheckThread* TOHTMainForm::GetStatusCheckThread()
{
	if(this->m_StatusCheckThread == NULL)
        return NULL;
    else
		return this->m_StatusCheckThread;
}

// "DataFiles\\OHTDetectAreaInfo.ini" 에서 UBG 대차패턴 비교할 때  기준값이 될 값 로딩
bool TOHTMainForm::LoadPatternData(char* szFilePath)
{
	LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile(szFilePath);

	m_bUBGINILoaded = false;
	ZeroMemory(m_szUBGInfo_INI, sizeof(m_szUBGInfo_INI));

    char szLog[100];	ZeroMemory(szLog, sizeof(szLog));
	char szSection[20];
	ZeroMemory(szSection, sizeof(szSection));

	char szKey[20];
	ZeroMemory(szKey, sizeof(szKey));

	if(!pIniFile->GetIniFileExist())
	{
		RapidLog(RLOG_UBG_CHK, "INI File is not exist.");
		return false;
    }

	if(pIniFile == NULL)
	{
		RapidLog(RLOG_UBG_CHK, "INI File Load Fail");
		return false;
	}

	char szPattern[256];
	ZeroMemory(szPattern, sizeof(szPattern));

	pIniFile->ReadString("Setting", "CheckPattern", "", szPattern);
	std::string strPattern = szPattern;
	StrReplace(&strPattern, " ", ""); 	// 공백 제거

	// 꼭 순차적인 패턴일 필요가 없다 1,2,3,12,13 도 괜찮음
	m_nArrCheckPattern = ExtractNumber_inString(strPattern, ',');  // int 배열로 얻기

	bool bOldVersion = false;
	int nSize = m_nArrCheckPattern.size();
	if(nSize == 0)
	{
		bOldVersion = true;
		for(int i=0; i<7 ;i++) 	// 옛날버전은 0~6을 기본으로 봄
            m_nArrCheckPattern.push_back(i+1);
		nSize = m_nArrCheckPattern.size();
		RapidLog(RLOG_UBG_CHK, "Old INI Version. Default 0~6 Pattern setting.");
		//delete pIniFile;
		//return false;

    }
	for(int i=0 ; i<nSize ; i++)
	{
		int nPtnIdx = m_nArrCheckPattern[i];
		if(nPtnIdx > COUNTOF_PATTERN+1 || nPtnIdx < 1)
		{
			sprintf(szLog, "INI Load Failed. Wrong Pattern No : %d", nPtnIdx);
			RapidLog(RLOG_UBG_CHK, szLog);
			delete pIniFile;
			return false;

		}
		m_nArrCheckPattern[i] -= 1;	// INI에는 +1 된 값이기 때문에 배열 인덱스로 바꾸기 위해서 -1 한다
    }

	// INI에서 읽어서 m_szUBGInfo_INI[][] 변수에 저장
	for(int a=0; a<nSize ; a++)
	{
		int nPtnIdx = m_nArrCheckPattern[a];

		sprintf(szSection, "Pattern%d", bOldVersion ? nPtnIdx : nPtnIdx+1);
		for(int b=0; b<COUNTOF_PATTERN_ASCII ; b++)
		{
			sprintf(szKey, "VAL%d", b);
			int nTmpVal = INT_MAX;
			pIniFile->ReadInteger(szSection, szKey, nTmpVal, &nTmpVal);

			// INI 로딩 실패 경우. 한개라도 읽지 못하면 실패
			if(nTmpVal == INT_MAX)
			{
				sprintf(szLog, "INI Load Failed in [%s], [%s]", szSection, szKey);
				RapidLog(RLOG_UBG_CHK, szLog);
				delete pIniFile;
				return false;

            }
			m_szUBGInfo_INI[nPtnIdx][b] = (char)nTmpVal;
		}
	}

	m_bUBGINILoaded = true;
	delete pIniFile;

	return true;
}

//////////////////////////////////////////
// AddUBGPattern(...) 함수
// UBG로부턴 받은 패턴을 맴버변수에 저장한다
// 단 한번에 4개의 패턴에 들어오기 때문에 적절히 데이터를 잘라서 m_szUBGInfo_Comm[][] 배열에 복사한다
//
void TOHTMainForm::AddUBGPattern(int nCmdFRIdx1234, char *szRecv)
{
	// szRecv 의 사이즈가 201 이라는 것은 보장되었다는 가정하에 함수 실행
	// szRecv 사이즈 에러처리는 호출하는 부분에서 함

	if(nCmdFRIdx1234 < 1 || nCmdFRIdx1234 > 4)
	{
		RapidLog(RLOG_UBG_CHK, "[ERROR] Add Pattern Index Range Over.");
		return;
    }

	char tmpBuf[512];
	int nSize = COUNTOF_PATTERN_ASCII*4;

	memcpy(tmpBuf, szRecv+8, nSize);  // +8 이유는 시작 start index 건너뜀
	tmpBuf[nSize] = '\0';		//널문자 추가해줘야 로그 예쁘게 남아서

	// 받은 데이터 로깅
	//
	char szLogMsg[1024];
	ZeroMemory(szLogMsg, sizeof(szLogMsg));

	sprintf(szLogMsg,"UBG RecvData [FR1%d]: %s", nCmdFRIdx1234, tmpBuf);
	RapidLog(RLOG_UBG_CHK, szLogMsg);   // logging


	// data 잘라서 m_szUBGInfo_Comm[][] 변수 저장
	//
	int nStartIdx = (nCmdFRIdx1234-1)*4;	// 값이 1부터 들어오니까 -1 해줌

	int nCnt = 0;
	// 한개 명령당 4개의 패턴이 담겨있기땜에 4번 도는 반복문
	for(int i=nStartIdx; i<nStartIdx+4 ; i++)
	{
		memcpy(m_szUBGInfo_Comm[i], tmpBuf + nCnt*COUNTOF_PATTERN_ASCII, COUNTOF_PATTERN_ASCII);

		if(false)	// Only debugging
		{
			ZeroMemory(szLogMsg, sizeof(szLogMsg));

			sprintf(szLogMsg,"UBG memcpy m_szUBGInfo_Comm[%d]: %s", i, m_szUBGInfo_Comm[i]);
			RapidLog(RLOG_UBG_CHK, szLogMsg);   // logging

		}
		nCnt++;
	}

}

bool TOHTMainForm::DoCompUBGPattern(int nPatternNo0to15)
{
	if(nPatternNo0to15 < 0 || nPatternNo0to15 > COUNTOF_PATTERN)
	{
		RapidLog(RLOG_UBG_CHK, "[ERROR] Comp Pattern Index Range Over.");
		return false;
	}
	if(m_bUBGINILoaded == false)
	{
		RapidLog(RLOG_UBG_CHK, "[ERROR] Comp Failed. INI is not loaded.");
		return false;
    }

	char strHex_Comm[1024], strHex_INI[1024];
	char szComm[1024], szINI[1024];

	ZeroMemory(strHex_Comm, sizeof(strHex_Comm));
	ZeroMemory(strHex_INI, sizeof(strHex_INI));
	ZeroMemory(szComm, sizeof(szComm));
	ZeroMemory(szINI, sizeof(szINI));



	int nIdx = nPatternNo0to15;

	// 반드시 임시 변수에 복사하고 비교해야함
	memcpy(szComm, m_szUBGInfo_Comm[nIdx], COUNTOF_PATTERN_ASCII);
	memcpy(szINI, m_szUBGInfo_INI[nIdx], COUNTOF_PATTERN_ASCII);

	StringToHexStream(szComm, strHex_Comm, sizeof(strHex_Comm), ',', false); 	//
	StringToHexStream(szINI, strHex_INI, sizeof(strHex_INI), ',', false);


	if(false)	// only debugging
	{
		char szLogMsg[2048];
		ZeroMemory(szLogMsg, sizeof(szLogMsg));

		sprintf(szLogMsg,"### CompPattern [%d] szComm: %s", nIdx,szComm);

		RapidLog(RLOG_UBG_CHK, szLogMsg);   // logging

		sprintf(szLogMsg,"### CompPattern [%d] szINI: %s", nIdx,szINI);
		RapidLog(RLOG_UBG_CHK, szLogMsg);   // logging

	}


	bool bSame = (strcmp(strHex_Comm, strHex_INI) == 0);

	if(bSame == false)
	{
		char szTmpLog[4096];
		sprintf(szTmpLog, "[CompFail_%d]\nRCV: %s\nINI: %s", nPatternNo0to15+1, strHex_Comm, strHex_INI);
		RapidLog(RLOG_UBG_CHK, szTmpLog);
	}

	return bSame;
}

// OHTDetectAreaInfo_UBG_RECV.ini 파일에
// UBG Comm 에서 받은 데이터를 고스란히 저장한다
bool TOHTMainForm::WriteUBGRecv_INI()
{
	char filePath[200];	ZeroMemory(filePath, sizeof(filePath));
	sprintf(filePath, "C:\\User\\OHT\\DataFiles\\OHTDetectAreaInfo_UBG_RECV.ini");

	DeleteFileA(filePath);

	FILE *fp = fopen(filePath, "a");    // write+append

	if(fp == NULL)
	{
		RapidLog(RLOG_UBG_CHK, "[ERROR] Can not save [OHTDetectAreaInfo_UBG_RECV.ini]");
		return false;
	}

	fprintf(fp, "[Setting]");
	fprintf(fp, "\n");
	fprintf(fp, "CheckPattern=");


	// INI 가 로딩 되었다면 비교하고자 하는 패턴번호를 가지고 있다
	if(m_bUBGINILoaded)
	{
		//[Setting]  CheckPattern=1,2,3,4,5,6,12,13  <= 이런 패턴 기록
		int nSize = m_nArrCheckPattern.size();
		for(int r=0 ; r<nSize ; r++)
		{	fprintf(fp, "%d", m_nArrCheckPattern[r]+1);
			if(r != nSize-1)
				fprintf(fp, ",");
		}

		fprintf(fp, "\n");

		for(int i=0; i<nSize ; i++)
		{
			int nPtnIdx = m_nArrCheckPattern[i];
			fprintf(fp, "[Pattern%d]", nPtnIdx+1);
			fprintf(fp, "\n");

			for(int j=0; j<COUNTOF_PATTERN_ASCII ; j++)
			{
				fprintf(fp, "VAL%d=%d", j, m_szUBGInfo_Comm[nPtnIdx][j]);
				fprintf(fp, "\n");
			}
		}
	}
	else	// INI 로딩 안되었다면 모든 패턴 INI save
	{
		for(int r=0 ; r<COUNTOF_PATTERN ; r++)
		{	fprintf(fp, "%d", r+1);        	//[Setting]  CheckPattern=1,2,3,4,5,6,12,13  <= 이런 패턴 기록
			if(r != COUNTOF_PATTERN-1)
				fprintf(fp, ",");
		}

		fprintf(fp, "\n");

	   for(int i=0; i<COUNTOF_PATTERN ; i++)
	   {
		   fprintf(fp, "[Pattern%d]", i+1);
		   fprintf(fp, "\n");

		   for(int j=0; j<COUNTOF_PATTERN_ASCII ; j++)
		   {
				fprintf(fp, "VAL%d=%d", j, m_szUBGInfo_Comm[i][j]);
				fprintf(fp, "\n");
		   }
	   }
	}

	fclose(fp);
	RapidLog(RLOG_UBG_CHK, "[SUCCESS] Writting OK. OHTDetectAreaInfo_UBG_RECV.ini");

	return true;
}


// IP 정보를 로그로 기록함. 프로그램 실행시 한번 동작
void TOHTMainForm::WriteLog_NetworkInfo()
{


	FILE *fp = NULL;
	try
	{
		SYSTEMTIME CurTime;
		GetLocalTime(&CurTime);

		char szFilePath[512];
		ZeroMemory(szFilePath, sizeof(szFilePath));

		sprintf(szFilePath, "D:\\Log\\NetworkInfo%d%02d%02d%02d.log", CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour);

		fp = fopen(szFilePath, "a");    // write+append

		if(fp == NULL)
			return;


		fprintf(fp, "\n\n[%02d:%02d:%02d.%03d] IPCONFIG >>> ", CurTime.wHour, CurTime.wMinute, CurTime.wSecond, CurTime.wMilliseconds);
		fclose(fp);
		fp = NULL;

		char cmd1[256];
		ZeroMemory(cmd1, sizeof(cmd1));

		sprintf(cmd1, "ipconfig >> %s", szFilePath);
		system(cmd1); // 도스 커맨드 IPCONFIG 실행

	}
	catch(...)
	{
		if(fp != NULL)
			fclose(fp);
	}
}

void __fastcall TOHTMainForm::btnReadMonitorRegClick(TObject *Sender)
{
	short axis;
	short reg_add;
	long result;
	int cbbIndex = cbbMonitorRegister->ItemIndex;
	AnsiString strTemp;
	axis = cbbAxisNum4Monitor->ItemIndex;

	if(axis < 0 || axis > 3) return;
	else axis+=1;

	if(cbbIndex >= 0 && cbbIndex < 10) reg_add = ERROR_TRACE_DATE0 + cbbIndex;
	else if(cbbIndex == 10) reg_add = CURRUNT_ALARM_STATE;
	else if(cbbIndex == 11) reg_add = SERVO_STATUS_MONITOR;
	else return;



	m_pServoControl->ReadMonitorData(axis, reg_add, &result);
	m_pServoControl->DisplayServoMonitorResult(reg_add, result);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkAxisMonitorClick(TObject *Sender)
{
	if(chkAxisMonitor->Checked) btnReadMonitorReg->Enabled = true;
	else btnReadMonitorReg->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::servoClick(TObject *Sender)
{
	if((gm_ExecuteInfo->ExecuteMode != EXECUTE_MODE_AUTO)
	&& (gm_ExecuteInfo->ExecutePositionInfo.Marked ==true ))
	{
		//경사로유무 확인 GetNodeTypeSlop

		if(!m_ControlTask->GetNodeTypeSlop())
		{
			m_HWSet->m_pServoMonitoring->SetFn009(1);
			m_HWSet->m_pServoMonitoring->SetFn009(2);
		}
		else
		{
			memoMain->Lines->Add("Slope가 아닌 구간에 Mark해주세요");
		}
	}
	else
	{
		memoMain->Lines->Add("Slope가 아닌 구간에 Mark해주세요");
	}
}

// SHCHO 2018.10.17
// IoT Ethernet 카드 게트웨이 설정 됐는지 확인
bool TOHTMainForm::IoTGatewayCheck()
{
	if(m_pParamSet->bIgnoreIoTLanCheck)
		return true;

	sEthernetInfo info[5];

	// 이더넷 정보 얻어오기
	int nCnt = GetEternetInfo(info, 5);
	if(nCnt != 2)
		return true;   // 이더넷 카드 두개가 아니면 일반 8.0 VHL이 아니라고 간주

	// OHTParam.ini에 IOT 랜카드 이름을 명시했을 때, 해당 카드 이름만 검색한다.
	// 명시하지 않았다면 첫번째 랜카드가 IOT라고 간주한다
	if(strlen(m_pParamSet->IotEternetName) > 0)
	{
		for(int i=0; i < nCnt ; i++)
		{
			bool bSame = strcmp(m_pParamSet->IotEternetName, info[i].Name) == 0;
			if(bSame)
			{
				// gateway에 값이 있으면 return false
				if(strlen(info[i].Gateway) > 0)
					return false;
				else
					return true;

			}
		}

		// 랜카드 이름을 OHTParam.ini에 명시 했음에도 ipconfig에서 찾을 수 없다면
		// return false 한다.
		return false;
	}


	// 명시된 랜카드 이름이 없을 땐 첫번째를 IOT 랜카드라고 간주한다.
	// gateway에 값이 있으면 return false
	if(strlen(info[0].Gateway) > 0)
		return false;

	return true;
}

//---------------------------------------------------------------------------
// 노드 번호를 알때 노드 객체 포인터를 얻어 정보를 얻기위함
NODE_INFO_STD_MAP* TOHTMainForm::GetNodeObj(int nNodeID)
{
	STD_MAP *pData = GetMap_STD();

	if(pData == NULL)
		return NULL;

	try
	{
		for(int i=0; i<pData->m_NodeNum ; i++)
		{
			if(pData->m_NodeArray[i].NodeID == nNodeID)
			{
				NODE_INFO_STD_MAP *pNode = &pData->m_NodeArray[i];
				return pNode;
			}
		}
	}

	catch(...){return NULL;}

	return NULL;
}
//AOHC-204 Thread 별 수행 시간 감시 기능
void TOHTMainForm::ThreadCycleLog(char *msg)
{
	CS_ThreadCycleLog->Acquire();

	WRITE_LOG_ON_THIS_FILE("D:\\log\\ThreadCycle.log", msg);

	CS_ThreadCycleLog->Release();
}

//---------------------------------------------------------------------------
//AOHC-204 Thread 별 수행 시간 감시 기능
ThreadTimeMonitor::ThreadTimeMonitor()
{
	memset(this, 0, sizeof(*this));
}


void ThreadTimeMonitor::Execute(char *name, DWORD time_ms)
{
	DWORD dwCur = timeGetTime();
	static bool	bTimeMonitor_MCC = false;
	if(nIdx == 0)
	{
		dwStart = dwCur;
		min = max = time_ms;
	}

	sum += time_ms;

	if(min > time_ms)
		min = time_ms;
	if(max < time_ms)
		max = time_ms;

	nIdx++;

	DWORD dwSub = dwCur - dwStart;

	// 일정시간에 한번씩 로깅한다
	if(OHTUTILITY::PreventOverflowGetTimeMs(dwStart) > 3000)
	{
		char szLog[128];
		memset(szLog, 0, sizeof(szLog));
		int avr = -1;
		if(nIdx > 0)
			avr = (int)((double)sum/(double)nIdx);

		// min, max, avr, cnt, cycle elapse
		sprintf(szLog, "/%s/%lu/%lu/%d", name, min, max, avr);
		OHTMainForm->ThreadCycleLog(szLog);
		nIdx = 0;
		sum = 0;

		if(bTimeMonitor_MCC == false)
			bTimeMonitor_MCC = true;
	}
	if(bTimeMonitor_MCC)
	{
		OHTMainForm->GetStatusCheckThread()->SaveThreadTime(max, name);
	}
}



void __fastcall TOHTMainForm::btn1Click(TObject *Sender)
{
	if(gm_ExecuteInfo->ExecuteMode != EXECUTE_MODE_AUTO)
	{
		gm_OHTMainStatus->bTraceStart = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::btn_limit_frontClick(TObject *Sender)
{
	m_pServoControl->SetInnerLimitTorque_Front();
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::btn_limitClick(TObject *Sender)
{
	m_pServoControl->SetInnerLimitTorque_800();
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::btn_limit_rearClick(TObject *Sender)
{
	m_pServoControl->SetInnerLimitTorque_Rear();
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::btn_limit1Click(TObject *Sender)
{
	m_pServoControl->SetInnerLimitTorque_90();
}
//---------------------------------------------------------------------------

bool TOHTMainForm::LoadHighSpeedParameter(char* szFilePath)
{
    LIniFile iniFile(szFilePath);

    if(!iniFile.GetIniFileExist())
    {
    	return false;
    }

    int nTmpValue = 0;
	iniFile.ReadInteger("TorqueLimit", "Front", 800, &nTmpValue);
	OHTMainForm->Edit_front_torque->Text = nTmpValue;
	iniFile.ReadInteger("TorqueLimit", "Rear", 800, &nTmpValue);
	OHTMainForm->Edit_rear_torque->Text = nTmpValue;

	iniFile.ReadInteger("TorqueLimit", "FrontName", 1, &nTmpValue);
	OHTMainForm->Edit_frontName->Text = nTmpValue;
	iniFile.ReadInteger("TorqueLimit", "RearName", 2, &nTmpValue);
	OHTMainForm->Edit_rearName->Text = nTmpValue;

	return true;
}


void __fastcall TOHTMainForm::tmAvsControlTimer(TObject *Sender)
{
//	m_AvsControl->ReceiveVibDataTimer();
}
//---------------------------------------------------------------------------
//AvsControl* TOHTMainForm::GetAvsControl()
//{
//    if(this->m_AvsControl == NULL)
//        return NULL;
//    else
//        return this->m_AvsControl;
//}

