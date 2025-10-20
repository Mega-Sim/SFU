// ---------------------------------------------------------------------------
/**
@file    AMCDllLib.cpp
$Rev:: 2909          $:  Revision of last commit
$Author:: ks4423     $:  Author of last commit
$Date:: 2015-04-30 0#$:  Date of last commit
@version OHT 7.0
@brief   AMCDllLib Class 소스 File
*/
// ---------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "AMCDllLib.h"

#include "Def_OHT.h"
#include "OHTMain.h"
#include "NumericSystem.h" // 90진수 변환

#define REG_READ_MODE	0x80	// bit7=1
#define REG_READ_DONE	0xFF

#define EXP_FPGA_VER					0x00 // RO
#define READ_RETRY_CNT					100

#define EXP_BASE_ADDR	((BYTE *)DeviceAMCGetPhysicalAddr(0x03F1))
#define EXP_LSB_ADDR	((BYTE *)DeviceAMCGetPhysicalAddr(0x03F2))
#define EXP_MSB_ADDR	((BYTE *)DeviceAMCGetPhysicalAddr(0x03F3))


// ---------------------------------------------------------------------------
#define	NO_ERR						0
#define	ERR_AMC_NOTOPEN				-1

#define MAX_IO_INT_COUNT			3

#define AXIS_DRIVING_F             	0
#define AXIS_DRIVING_R             	1
#define AXIS_HOIST                 	2
#define AXIS_SHIFT                 	3

#define AMC_DPRAM_ADDRESS			0xD0000
#define AMC_INTERRUPT				11 //PCM9377 == 9 , 신규 SOM4463 == 11

#define NAME_AMCPARA				"DefaultParam.ini"	// 미사용
#define PATH_AMCPARA				"DataFiles\\"
#define PATH_NAME_AMCPARA			"DataFiles\\DefaultParam.ini"   // 미사용	AMCPARAM.INI" //
#define MAX_LENGTH_OF_FULL_PATH		1024


#define MAX_BYTE_SIZE_OF_AMC_LOG	1024*100

// ---------------------------------------------------------------------------


/************************************************************
 *  Content : AMC Errorcode ==> axis_source에서 받는것  *
 ************************************************************/
#define ST_NONE               0x0000 // 정상동작
#define ST_HOME_SWITCH        0x0001 // 원점(Home) 센서 감지
#define ST_POS_LIMIT          0x0002 // + 방향 Limit 센서 감지
#define ST_NEG_LIMIT          0x0004 // - 방향 Limit 센서 감지
#define ST_AMP_FAULT          0x0008 // AMP Fault 상태 발생
#define ST_A_LIMIT            0x0010 // 가, 감속시간이 Limit 보다 클때 발생
#define ST_V_LIMIT            0x0020 // 현재속도가 속도 limit보다 클때 발생
#define ST_REVESE_MOVE        0x00C0 // 진행방향과 반대의 프로파일 생성시
#define ST_X_NEG_LIMIT        0x0040 // - Position limit를 벗어날때 발생
#define ST_X_POS_LIMIT        0x0080 // + Position limit를 벗어날때 발생
#define ST_ERROR_LIMIT        0x0100 // 위치오차가 Limit 값보다 클때 발생
#define ST_PC_COMMAND         0x0200 // EVENT가 발생된 상태일때 발생
                                    // set_stop, set_e_stop, set_linear_all_stop
#define ST_OUT_OF_FRAMES      0x0400 // Frame buffer가 Full 상태일때 발생
#define ST_AMP_POWER_ONOFF    0x0800 // AMP disable 상태일때 발생
#define ST_ABS_COMM_ERROR     0x1000 // ABS encoder 통신 Error시 발생
#define ST_INPOSITION_STATUS  0x2000 // In position signal 입력 여부
#define ST_RUN_STOP_COMMAND   0x4000 // Run stop error
#define ST_COLLISION_STATE    0x8000 // 충돌방지 Error
#define MAX_NODE_COUNT 		10000
#define AMC_ADD_LOG(...)    WRITE_LOG_UNIT(m_pLogAMCSend, __VA_ARGS__)
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)
//#define AMC_ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\AMC_Send.log", __VA_ARGS__)
//#define AMC_PERIODIC_SEND_LOG_OLD(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\AMC_Send_PeriodicOLD.log", __VA_ARGS__)
//#define AMC_PERIODIC_SEND_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\AMC_Send_Periodic.log", __VA_ARGS__)
//#define AMC_PERIODIC_LOG(...)	WRITE_LOG_UNIT(m_pLogUnit, __VA_ARGS__)
//#define AMC_ENC_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\AMC_Recv.log", __VA_ARGS__)



int ConvertAMCError(int nAmcError);
wchar_t *strOHTMODEL[] = {L"FOUP", L"MAC", L"PROBECARD", L"PROBECARD_JIG", L"FOUP_REV01",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW", L"UNKNOW",
						  L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",
						  L"UNKNOW", L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"OHT_STD_V80_EUV",
						  L"OHT_STD_V80",L"OHT_STD_V81",L"OHT_STD_V81S",L"UNKNOW",L"OHT_STD_V85"};

enum
{
	WDT_MAINLOOP = 1,
	WDT_EXTRA = 2,
	WDT_SUBCONTROL = 3,
	WDT_CONTROL = 4
};


/**
@brief   AMC Library를 Load하여 함수 초기화
@author  LSJ
@date    2012.10.27
@return  Load에 문제 없으면 Instance, 문제 발생시 NULL 리턴.
*/
HINSTANCE AMCLibrary::RunTimeLink()
{
	EnterCriticalSection(&m_CS);
	HINSTANCE hAMCLib = NULL;
	TCHAR strTmpFullPathFileName[MAX_LENGTH_OF_FULL_PATH]={0,};

#ifdef  UNICODE
	LPCTSTR strTmpFileName = L"DataFiles\\amc.dll";       //TCHAR
	_wfullpath(strTmpFullPathFileName, strTmpFileName, MAX_LENGTH_OF_FULL_PATH);
#else
	LPCTSTR strTmpFileName = "DataFiles\\amc.dll";       //TCHAR
	_fullpath(strTmpFullPathFileName, strTmpFileName, MAX_LENGTH_OF_FULL_PATH);
#endif

	if(m_bDllLoaded==true)
	{
		LeaveCriticalSection(&m_CS);
		return NULL;
	}

	hAMCLib = LoadLibrary(strTmpFullPathFileName);


	if (hAMCLib == NULL)
	{

		LeaveCriticalSection(&m_CS);
		return NULL;
	}
	else
	{
		// -- 초기화 --
		DeviceAMCOpen	= (OPENAMCDEVICE)	GetProcAddress(hAMCLib, "amc_open");
		DeviceAMCClose	= (CLOSEAMCDEVICE)	GetProcAddress(hAMCLib, "amc_close");
		DeviceAMCInit	= (AMCINIT)			GetProcAddress(hAMCLib, "mmc_init");
		DeviceAMCReboot	= (AMC_DSP_REBOOT)	GetProcAddress(hAMCLib,"dsp_reboot");
		// -- 버전 확인 -- 리턴된 값이 20100이라면 2.01.00
		DeviceAMCVerChk	= (AMC_VERSION_CHK)	GetProcAddress(hAMCLib, "version_chk");

	//==========================================================================
		// -- AMC 정보 받기 -- : OHTv7.0 하면서 추가된 함수
		DeviceAMCGetDatas				= (AMC_GETDATAS)				GetProcAddress(hAMCLib, "ReturnAMCData");
		DeviceAMCGatheringTraceData		= (AMC_GATHERING_TRACE_DATA)	GetProcAddress(hAMCLib, "GatheringTraceData");
		DeviceAMCGatheringTraceData_1	= (AMC_GATHERING_TRACE_DATA_1)	GetProcAddress(hAMCLib, "GatheringTraceData_1");

		// AMC Emulator 시리얼 통신 목적
		DeviceAMCCreateSerial = (AMC_CREATE_SERIAL) GetProcAddress(hAMCLib, "CreateSerial");
		DeviceAMCReadSerial = (AMC_READ_SERIAL) GetProcAddress(hAMCLib, "ReadSeiral");
		DeviceAMCWriteSerial = (AMC_WRITE_SERIAL) GetProcAddress(hAMCLib, "WriteSeiral");
		DeviceAMCCloseSerial = (AMC_CLOSE_SERIAL) GetProcAddress(hAMCLib, "CloseSeiral");

	//==========================================================================
		// -- Init 값 --
		DeviceAMCSetPosition = (AMC_SETPOSITION) GetProcAddress(hAMCLib, "set_position");
		DeviceAMCGetPosition = (AMC_GETPOSITION) GetProcAddress(hAMCLib, "get_position");
		DeviceAMCSetAmpEnable = (AMC_SETAMPENABLE) GetProcAddress(hAMCLib, "set_amp_enable");
		DeviceAMCGetAmpEnable = (AMC_GETAMPENABLE) GetProcAddress(hAMCLib, "get_amp_enable");
		DeviceAMCFaultSet = (AMC_AMPFAULTSET) GetProcAddress(hAMCLib, "amp_fault_set");
		DeviceAMCFaultReset = (AMC_AMPFAULTRESET) GetProcAddress(hAMCLib, "amp_fault_reset");

		// -- Motion --
		DeviceAMCInSequence = (AMC_IN_SEQUENCE) GetProcAddress(hAMCLib, "in_sequence");
		DeviceAMCInMotion = (AMC_IN_MOTION) GetProcAddress(hAMCLib, "in_motion");
		DeviceAMCInPosition = (AMC_IN_POSITION) GetProcAddress(hAMCLib, "in_position");
		DeviceAMCFramesLeft = (AMC_FRAMES_LEFT) GetProcAddress(hAMCLib, "frames_left");
		DeviceAMCMotionDone = (AMC_MOTION_DONE) GetProcAddress(hAMCLib, "motion_done");

		DeviceAMCAxisDone = (AMC_AXIS_DONE) GetProcAddress(hAMCLib, "axis_done");
		DeviceAMCAxisState = (AMC_AXIS_STATE) GetProcAddress(hAMCLib, "axis_state");
		DeviceAMCAxisSource = (AMC_AXIS_SOURCE) GetProcAddress(hAMCLib, "axis_source");
		DeviceAMCClearStatus = (AMC_CLEAR_STATUS) GetProcAddress(hAMCLib, "clear_status");
		DeviceAMCClearStop = (AMC_CLEAR_STOP) GetProcAddress(hAMCLib, "clear_stop");

		DeviceAMCFramesClear = (AMC_FRAMES_CLEAR) GetProcAddress(hAMCLib, "frames_clear");
		DeviceAMCSetClosedLoop = (AMC_SET_CLOSED_LOOP) GetProcAddress(hAMCLib, "set_closed_loop");
		DeviceAMCSetUnipolar = (AMC_SET_UNIPOLAR) GetProcAddress(hAMCLib, "set_unipolar");
		DeviceAMCSetFeedback = (AMC_SET_FEEDBACK) GetProcAddress(hAMCLib, "set_feedback");
		DeviceAMCSetControl = (AMC_SET_CONTROL) GetProcAddress(hAMCLib, "set_control");

		DeviceAMCSetServo = (AMC_SET_SERVO) GetProcAddress(hAMCLib, "set_servo");
		DeviceAMCVMove = (AMC_VMOVE) GetProcAddress(hAMCLib, "v_move");
		DeviceAMCMove = (AMC_MOVE) GetProcAddress(hAMCLib, "move");
		DeviceAMCStartMove = (AMC_STARTMOVE) GetProcAddress(hAMCLib, "start_move");

		DeviceAMCPMove = (AMC_PMOVE) GetProcAddress(hAMCLib, "move_p");
		DeviceAMCNMove = (AMC_NMOVE) GetProcAddress(hAMCLib, "move_n");
		DeviceAMCSMove = (AMC_SMOVE) GetProcAddress(hAMCLib, "move_s");
		DeviceAMCDSMove = (AMC_DSMOVE) GetProcAddress(hAMCLib, "move_ds");

		DeviceAMCPTMove = (AMC_PTMOVE) GetProcAddress(hAMCLib, "move_pt");
		DeviceAMCNTMove = (AMC_NTMOVE) GetProcAddress(hAMCLib, "move_nt");
		DeviceAMCSTMove = (AMC_STMOVE) GetProcAddress(hAMCLib, "move_st");
		DeviceAMCDSTMove = (AMC_DSTMOVE) GetProcAddress(hAMCLib, "move_dst");

		DeviceAMCSetEncoderOffset = (AMC_SET_ENCODEROFFSET) GetProcAddress(hAMCLib, "set_encoder_offset");
		DeviceAMCGetEncoderOffset = (AMC_GET_ENCODEROFFSET) GetProcAddress(hAMCLib, "get_encoder_offset");
		// -- I/O 제어 --
		DeviceAMCSetIO_64 = (AMC_SET_IO64) GetProcAddress(hAMCLib, "set_io64");
		DeviceAMCGetIO_64 = (AMC_GET_IO64) GetProcAddress(hAMCLib, "get_io64");
		DeviceAMCGetOutIO = (AMC_GET_OUT64) GetProcAddress(hAMCLib, "get_out64");
		DeviceAMCSetBit = (AMC_SET_BIT) GetProcAddress(hAMCLib, "set_bit");
		DeviceAMCResetBit = (AMC_RESET_BIT) GetProcAddress(hAMCLib, "reset_bit");

		DeviceAMCSetIO_Count    = (AMC_SET_IO_COUNT)        GetProcAddress(hAMCLib, "set_io_count");
		DeviceAMCGetIO_Count    = (AMC_GET_IO_COUNT)        GetProcAddress(hAMCLib, "get_io_count");
		DeviceAMCSetIO_Output    = (AMC_SET_IO_OUTPUT)        GetProcAddress(hAMCLib, "set_io_output");
		DeviceAMCGetIO_Output    = (AMC_GET_IO_OUTPUT)        GetProcAddress(hAMCLib, "get_io_output");
		DeviceAMCGetIO_Input    = (AMC_GET_IO_INPUT)        GetProcAddress(hAMCLib, "get_io_input");

		// -- 사용자 Command 정의 및 File Download --
		DeviceAMCDownloadFile = (AMC_DOWNLOAD_FILE) GetProcAddress(hAMCLib, "DownloadLongBlock");
		DeviceAMCCmdGenerate = (AMC_CMD_GEN) GetProcAddress(hAMCLib, "amc_exchange_string");

		// -- File 관련 함수 --
		DeviceAMCOutUpdate = (AMC_FS_DOWNLOAD) GetProcAddress(hAMCLib, "fs_download");
		DeviceAMCSetBootFile = (AMC_FS_SETBOOTFILE) GetProcAddress(hAMCLib, "fs_setbootfile");
		DeviceAMCInstall = (AMC_FS_INSTALL) GetProcAddress(hAMCLib, "fs_install");

		DeviceAMCUploadSysParam = (AMC_UPLOAD_SYS_PARAM) GetProcAddress(hAMCLib, "UploadSysParam");
		DeviceAMCDnloadSysParam = (AMC_DNLOAD_SYS_PARAM) GetProcAddress(hAMCLib, "DnloadSysParam");
		DeviceAMCUploadParam = (AMC_UPLOAD_PARAM) GetProcAddress(hAMCLib, "UploadParam");
		DeviceAMCDnloadParam = (AMC_DNLOAD_PARAM) GetProcAddress(hAMCLib, "DnloadParam");

		DeviceAMCParamSaveToDSP = (AMC_SAVE_PARAM) GetProcAddress(hAMCLib, "amc_save_local_sysparam_to_dsp");
		DeviceAMCParamSaveToFile = (AMC_LOAD_PARAM) GetProcAddress(hAMCLib, "amc_load_dsp_sysparam_with_localfile");
		DeviceAMCParamSaveToEEPROM = (AMC_SAVE_PARAM_TO_EEPROM) GetProcAddress(hAMCLib, "amc_flush_sysparam_to_eeprom");

		DeviceAMCGetActVelocity = (AMC_GETACTVELOCITY) GetProcAddress(hAMCLib, "get_act_velocity");
		DeviceAMCGetComVelocity = (AMC_GETCOMVELOCITY) GetProcAddress(hAMCLib, "get_com_velocity");

		DeviceAMCGetActVelocity_RPM = (AMC_GETACTVELOCITY_RPM) GetProcAddress(hAMCLib, "get_act_velocity_rpm");
		DeviceAMCGetComVelocity_RPM = (AMC_GETCOMVELOCITY_RPM) GetProcAddress(hAMCLib, "get_com_velocity_rpm");

		DeviceAMCSetPositiveSWLimit = (AMC_SET_POSITIVE_SW_LIMIT) GetProcAddress(hAMCLib, "set_positive_sw_limit");
		DeviceAMCSetNegativeSWLimit = (AMC_SET_NEGATIVE_SW_LIMIT) GetProcAddress(hAMCLib, "set_negative_sw_limit");

		DeviceAMCGetPositiveSWLimit = (AMC_GET_POSITIVE_SW_LIMIT) GetProcAddress(hAMCLib, "get_positive_sw_limit");
		DeviceAMCGetNegativeSWLimit = (AMC_GET_NEGATIVE_SW_LIMIT) GetProcAddress(hAMCLib, "get_negative_sw_limit");

		DeviceAMCSetErrorLimit = (AMC_SET_ERROR_LIMIT) GetProcAddress(hAMCLib, "set_error_limit");
		DeviceAMCGetErrorLimit = (AMC_GET_ERROR_LIMIT) GetProcAddress(hAMCLib, "get_error_limit");
		DeviceAMCGetError = (AMC_GET_ERROR) GetProcAddress(hAMCLib, "get_error");

		DeviceAMCSetSyncMapAxes = (AMC_SET_SYNC_MAP_AXES) GetProcAddress(hAMCLib, "set_sync_map_axes");
		DeviceAMCSetSyncControl = (AMC_SET_SYNC_CONTROL) GetProcAddress(hAMCLib, "set_sync_control");
		DeviceAMCSetSyncGain = (AMC_SET_SYNC_GAIN) GetProcAddress(hAMCLib, "set_sync_gain");
		DeviceAMCGetSyncControl = (AMC_GET_SYNC_CONTROL) GetProcAddress(hAMCLib, "get_sync_control");
		DeviceAMCGetSyncGain = (AMC_GET_SYNC_GAIN) GetProcAddress(hAMCLib, "get_sync_gain");
		DeviceAMCGetSyncPosition = (AMC_GET_SYNC_POSITION) GetProcAddress(hAMCLib, "get_sync_position");

		DeviceAMCFormat = (AMC_FORMAT) GetProcAddress(hAMCLib, "fs_format");

		// 필터 관련..
		DeviceAMCSetPositionLPF = (AMC_SET_POSITION_LPF) GetProcAddress(hAMCLib, "set_position_lpf");
		DeviceAMCSetPositionNotchFilter = (AMC_SET_POSITION_NOTCH_FILTER) GetProcAddress(hAMCLib, "set_position_notch_filter");

		// ** watchdog 관련
		DeviceAMCEnableWatchdogReason = (AMC_ENABLE_WDT_REASON) GetProcAddress(hAMCLib, "enable_wdt_reason");
		DeviceAMCDisableWatchdogReason = (AMC_DISABLE_WDT_REASON) GetProcAddress(hAMCLib, "disable_wdt_reason");
		DeviceAMCGetWatchdogStatus = (AMC_GET_WDT_STATUS) GetProcAddress(hAMCLib, "get_wdt_status");
		DeviceAMCSetWatchdogStatus = (AMC_SET_WDT_STATUS) GetProcAddress(hAMCLib, "set_wdt_status");
		DeviceAMCClearWatchdogReason = (AMC_CLR_WDT_REASON) GetProcAddress(hAMCLib, "clr_wdt_reason");
        DeviceAMCGetPhysicalAddr = (AMC_GET_PHYSICAL_ADDR) GetProcAddress(hAMCLib, "GetPhysicalAddr");

		// ** debug status
		DeviceAMCGetDebugStatus = (AMC_GET_DBG_STATUS) GetProcAddress(hAMCLib, "get_dbg_status");


		// ** velocity limit	3.0
		DeviceAMCSetVelCurve = (AMC_SET_VEL_CURVE) GetProcAddress(hAMCLib, "set_vel_curve");
		DeviceAMCSetActVelMargin = (AMC_SET_ACTVEL_MARGIN) GetProcAddress(hAMCLib, "set_actvel_margin");

		DeviceAMCSetOhtModelId = (AMC_SET_OHT_MODEL_ID) GetProcAddress(hAMCLib, "set_oht_model_id");
		DeviceAMCGetOhtModelId = (AMC_GET_OHT_MODEL_ID) GetProcAddress(hAMCLib, "get_oht_model_id");

		if (!DeviceAMCOpen || !DeviceAMCClose || !DeviceAMCInit || !DeviceAMCSetPosition || !DeviceAMCGetPosition ||
			!DeviceAMCSetAmpEnable || !DeviceAMCGetAmpEnable || !DeviceAMCFaultSet || !DeviceAMCFaultReset ||
			!DeviceAMCInSequence || !DeviceAMCInMotion || !DeviceAMCInPosition || !DeviceAMCFramesLeft ||
			!DeviceAMCMotionDone || !DeviceAMCAxisDone || !DeviceAMCAxisState || !DeviceAMCAxisSource || !DeviceAMCClearStatus ||
			!DeviceAMCClearStop || !DeviceAMCFramesClear || !DeviceAMCSetClosedLoop || !DeviceAMCSetUnipolar ||
			!DeviceAMCSetFeedback || !DeviceAMCSetControl || !DeviceAMCSetServo || !DeviceAMCVMove || !DeviceAMCMove ||
			!DeviceAMCStartMove || !DeviceAMCSetIO_64 || !DeviceAMCGetIO_64 || !DeviceAMCGetOutIO || !DeviceAMCSetBit || !DeviceAMCResetBit ||
			!DeviceAMCDownloadFile || !DeviceAMCCmdGenerate || !DeviceAMCUploadSysParam || !DeviceAMCDnloadSysParam ||
			!DeviceAMCUploadParam || !DeviceAMCDnloadParam || !DeviceAMCParamSaveToDSP || !DeviceAMCParamSaveToFile ||
			!DeviceAMCParamSaveToEEPROM || !DeviceAMCGetActVelocity || !DeviceAMCGetComVelocity || !DeviceAMCSetPositiveSWLimit ||
			!DeviceAMCSetNegativeSWLimit || !DeviceAMCGetPositiveSWLimit || !DeviceAMCGetNegativeSWLimit || !DeviceAMCSetErrorLimit ||
			!DeviceAMCVerChk || !DeviceAMCPMove || !DeviceAMCNMove || !DeviceAMCSMove || !DeviceAMCDSMove || !DeviceAMCSetEncoderOffset ||
			!DeviceAMCGetEncoderOffset || !DeviceAMCPTMove || !DeviceAMCNTMove || !DeviceAMCSTMove || !DeviceAMCDSTMove ||
			!DeviceAMCGetActVelocity_RPM || !DeviceAMCGetComVelocity_RPM || !DeviceAMCSetSyncMapAxes || !DeviceAMCSetSyncControl ||
			!DeviceAMCSetSyncGain || !DeviceAMCGetSyncControl || !DeviceAMCGetSyncGain || !DeviceAMCGetSyncPosition ||
			!DeviceAMCOutUpdate || !DeviceAMCReboot || !DeviceAMCSetBootFile || !DeviceAMCSetPositionLPF || !DeviceAMCSetPositionNotchFilter ||
			!DeviceAMCInstall || !DeviceAMCFormat || !DeviceAMCGetErrorLimit || !DeviceAMCGetError || !DeviceAMCEnableWatchdogReason ||
			!DeviceAMCDisableWatchdogReason || !DeviceAMCGetWatchdogStatus || !DeviceAMCSetWatchdogStatus || !DeviceAMCClearWatchdogReason ||
			!DeviceAMCGetDebugStatus || !DeviceAMCSetVelCurve || !DeviceAMCSetActVelMargin ||
	//========================================================================== 7.0 추가 내용
			!DeviceAMCSetOhtModelId || !DeviceAMCGetOhtModelId ||		
			!DeviceAMCSetIO_Count || !DeviceAMCGetIO_Count || !DeviceAMCSetIO_Output || !DeviceAMCGetIO_Output || !DeviceAMCGetIO_Input ||
			!DeviceAMCGetDatas	|| !DeviceAMCGatheringTraceData|| !DeviceAMCGatheringTraceData_1 || !DeviceAMCGetPhysicalAddr)
		{
			FreeLibrary(hAMCLib);
			LeaveCriticalSection(&m_CS);
			return NULL;
		}
		else
		{
			LeaveCriticalSection(&m_CS);
			return hAMCLib;
		}
	}
}

/**
@brief   AMC Library 생성자

	1. 변수 초기화
	2. DLL Loading
	3. 에러 확인
@author  ehoto
@date    2012.10.27
*/
AMCLibrary::AMCLibrary()
{
	bSetting_Jig = false;
	bAxis_Ignore = false;


	for(int i = 0; i<4; i++)
		bUseAxis_Jig[i] = false;

	// 90진수 변환
	m_pLogAmcRcv = getLogUnit("D:\\log\\AMC_Recv.log", MAX_BYTE_SIZE_OF_AMC_LOG);
	m_pLogAmcSendPr =  getLogUnit("D:\\log\\AMC_Send_Periodic.log", MAX_BYTE_SIZE_OF_AMC_LOG);
	m_pLogAMCSend = getLogUnit("D:\\log\\AMC_Send.log", MAX_BYTE_SIZE_OF_AMC_LOG);
	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", 1024*10);


	InitializeCriticalSection(&m_CS);

	int nError = NO_ERR;
	AbnormalNodeCount = 0;
	nAMCCommError = NO_ERR;
	nAMCBufferError = NO_ERR;

	// 1. 변수 초기화
	m_fSyncGain = 1.0;
	m_hAMCLib = NULL; // handle

	m_bDllLoaded			= false;
	m_bDriverOpen			= false;
	m_bAMCFileDownloaded	= false;
	m_bAMCReady				= false;
	g_uLogDpramBase = 0x0;
	wFPGAVersion= 0x0;
	m_nError = 0;

	nAMCPauseStatus= 0;
	nAMCTorqueStatus = 0;
	nAMCVHLDetectStatus = 0;
	nAMCVHLStopStatus =0;
	nAMCDecelStatus=0;
	nAMCDetectStopDecelDownStatus = 0;
	nAMCDetectForceIOStatus = 0;
	m_VolatageMonitor[0] = 0;
	m_VolatageMonitor[1] = 0;

	m_FollowigAxis1 = 0; //AOHC-196 사전 로그 추가
	for(int nAxis=0; nAxis<4; nAxis++)
	{
		m_abIsRunning[nAxis]				=	false;					///< 주행 중인지
		m_abInPosition[nAxis]				=	false;				///< 도착 완료 상태인지
		m_abAmpEnabled[nAxis]				= 	false;					///< AMP Enable 상태인지    : [임시]역으로 변환해둠
		m_adCurrentPosition[nAxis]			= 	0.0;				///< 현재 위치:	Cts
		m_adFinalPosition[nAxis]			= 	0.0;				///< 최종 위치:	Cts
		m_adCurrentVelocity[nAxis]			= 	0.0;			///< 현재 속도: Cts/sec
		m_adFollowingError[nAxis]			=	0.0;			///< 현재 Following Error :  Cts
		m_adPositionOfSmallAddCheck[nAxis]	= 	0.0;	///< 현재 위치:	Cts

		m_anAxisError[nAxis] 				=	0;	///< 축별 에러

		m_adPrePosition[nAxis] = 0.0;
		m_adTotalPosition[nAxis] = 0.0;
		m_abIsCounted[nAxis] = false;
		m_anTotalMoveCount[nAxis] = 0;

	}

	for(int nIndex=0; nIndex<COUNT_OF_AMC_AXIS; nIndex++)
	{
		m_adPositiveLimit[nIndex]=(0x01<<(sizeof(UINT)*8-1))-1;	// 엔코더 Max 값 설정 : AMC에서 실질적으로 int로 사용됨
		m_adNegativeLimit[nIndex]=((-1)*pow(0x01,(sizeof(UINT)*8-1)));	// 엔코더 Min 값 설정 : AMC에서 실질적으로 int로 사용됨
	}

	m_dwSndTime = timeGetTime();

	m_bUseWatchDog = true;		// WatchDog 사용 여부 파라미터

	// AMC 주기 통신 명령
	memset( &m_SndCmd,0,sizeof(AMC_CMD));
	memset( &m_QueuingAmcCmd,0,sizeof(QUEUEING_AMC_CMD));
	memset(&m_NodeCheckData, 0, sizeof(m_NodeCheckData));

	// 2. DLL Loading
	nError = AMCInit();

	// 3. 에러 확인
	if (!nError)
	{
		nError = DeviceAMCVerChk(0, nAMCVer);
		FPGARegRead(EXP_FPGA_VER, READ_RETRY_CNT, wFPGAVersion);

	}
	else
	{
		AMCClose();
	}

	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
	/// @todo	AMC 프로그램 버전을 확인한다   // gm_OHTStatus->strFWVer=AMC_Get_FirmwareVision();
}

/**
@brief   AMC Library 소멸자
@author  ehoto
@date    2012.10.27
*/
AMCLibrary::~AMCLibrary()
{
	AMCClose();
	DeleteCriticalSection(&m_CS);
}


int AMCLibrary::AMCInit(void)
{
	EnterCriticalSection(&m_CS);

	int	nError = 0;
	int	nRetValue = 0;
	char strTmpFullPathFileName[MAX_LENGTH_OF_FULL_PATH]={0,};
	AMC_DATA RcvData;

	if(m_bAMCReady==true)
	{
		LeaveCriticalSection(&m_CS);
		return NO_ERR;
	}

	// 0. 변수 초기화
	memset( m_anInput, 0, MAX_COUNT_IO_INPUT * sizeof(int) );
	memset( m_anOutput, 0, MAX_COUNT_IO_OUTPUT * sizeof(int) );
	memset( m_anOutput_CMD, 0, MAX_COUNT_IO_OUTPUT * sizeof(int));

	memset(&m_Input1Mask_R01, 0x00, sizeof(m_Input1Mask_R01));
	memset(&m_Input2Mask_R01, 0x00, sizeof(m_Input2Mask_R01));
	memset(&m_Input3Mask_R01, 0x00, sizeof(m_Input3Mask_R01));

	InputMaskInit();

//	Sleep(5000);

	// 1. DLL Load ---------- ---------- ---------- ----------
	LeaveCriticalSection(&m_CS);
	if(!m_hAMCLib) m_hAMCLib = RunTimeLink();
	EnterCriticalSection(&m_CS);
	if(m_hAMCLib!=NULL)
	{
		m_bDllLoaded = true;
	}
	else
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_AMC_INIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_AMC_INIT;
	}

//    Sleep(5000);

	// 2. AMC Open ---------- ---------- ---------- ----------
	// @todo : 경로 값의 위치 확인 필요
	memset(strTmpFullPathFileName, 0, MAX_LENGTH_OF_FULL_PATH * sizeof(char) );
	_fullpath(strTmpFullPathFileName, PATH_AMCPARA, MAX_LENGTH_OF_FULL_PATH);
	nRetValue = DeviceAMCOpen(AMC_INTERRUPT, AMC_DPRAM_ADDRESS, strTmpFullPathFileName);

	AMC_ADD_LOG("Directory: %s", strTmpFullPathFileName);

	m_bDriverOpen = (nRetValue) ? true : false; // 요기 return int를 bool로 변경...

	if(m_bDriverOpen)
	{
		nRetValue = DeviceAMCInit();
		nError = ConvertAMCError(nRetValue);
	}
	else
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		return ERR_AMC_NOTOPEN;
	}

//	EnterCriticalSection(&m_CS);
//	SetBit(OUT_BRAKE_DRIVING_RELEASE, false, false);
//	DeviceAMCGetDatas(&m_SndCmd, &RcvData);
//	LeaveCriticalSection(&m_CS);

//    Sleep(5000);     //DeviceAMCInit 함수 호출때 뒤로 밀리는 현상 발생.

	if(nError!=NO_ERR)
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		return nError; //Convert_Error(nError);;
	}

	// 3. AMCParam 다운로드 ---------- ---------- ---------- ----------
	// 이상: AMC 내부의 값을 Defaul Parameter로 저장 후 그 파일을 다시 AMC에 투입함
	// @todo : Return 값이 어떤 값인지 확인 필요
	if(nError==NO_ERR)
	{
		memset( strTmpFullPathFileName, 0, MAX_LENGTH_OF_FULL_PATH * sizeof(char) );
		_fullpath(strTmpFullPathFileName, PATH_NAME_AMCPARA, MAX_LENGTH_OF_FULL_PATH);
		nRetValue = DeviceAMCParamSaveToFile(true, PATH_NAME_AMCPARA);	//"C:\\USER\\OHT\\DataFiles\\DefaultParam.ini"
	}
	if(nError==NO_ERR)
	{
		nRetValue = DeviceAMCParamSaveToDSP();
		ConvertAMCError(nRetValue);
	}
	if(nError==NO_ERR)
	{
		nRetValue = DeviceAMCParamSaveToEEPROM();
		ConvertAMCError(nRetValue);
	}
	if(nError==NO_ERR)
	{
		m_bAMCFileDownloaded = true;
	}
	else
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		return ConvertAMCError(nRetValue); //Convert_Error(nError);;
    }

	// 4. AMC 사용 가능한 상태 ---------- ---------- ---------- ----------
	if(m_bAMCFileDownloaded==true && m_bDriverOpen==true && m_bDllLoaded==true)
	{
		m_bAMCReady = true;
		AMC_ADD_LOG("m_bAMCReady = %d [%d:%d:%d]", m_bAMCReady, m_bAMCFileDownloaded, m_bDriverOpen, m_bDllLoaded);

		g_uLogDpramBase = DeviceAMCGetPhysicalAddr(0x1D0);
        AMC_ADD_LOG("Log Dpram Base = %x", g_uLogDpramBase);
	}
	else
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		return ERR_AMC_NOTOPEN;
	}

	// 5. 축 초기화 ---------- ---------- ---------- ----------
	for(int nIndex=0; nIndex<COUNT_OF_AMC_AXIS; nIndex++)
	{

//		DeviceAMCSetAmpEnable(nIndex, false);
        DeviceAMCSetAmpEnable(nIndex, true);
		AMC_ADD_LOG("DeviceAMCSetAmp Enable 축_초기화");
	}


#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
   if(nError == NO_ERR)
		nError = DeviceAMCSetIO_Count(96, 96);
#endif

	// 7. Watch Dog 설정 ---------- ---------- ---------- ----------
   if(m_bUseWatchDog==true)
   {
	   nError = EnableWatchDog();
	   if(nError==NO_ERR) nRetValue = ClearWatchDogStatus();
	   if(nError)
	   {
			LeaveCriticalSection(&m_CS);
			AMCClose();
		  return nError;
	   }
   }

	int OHT_Model = -2;

    // 2016-12-03, Add by Labeler
    // Reticle OHT용 AMC Firmware 받을 경우 변경필요
	if(OHT_NAME >= OHT_NAME_STD_V80_RETICLE)
	{
		for(int j=0 ; j<5 ; j++)
		{
			if((OHT_NAME == OHT_NAME_STD_V80_RETICLE))
				DeviceAMCSetOhtModelId(OHT_NAME_STD_V80);
			else
				DeviceAMCSetOhtModelId(OHT_NAME);
			OHTMainForm->memoMain->Lines->Add("OHT Model");
			OHTMainForm->memoMain->Lines->Add(strOHTMODEL[OHT_NAME]);

			DeviceAMCGetOhtModelId(&OHT_Model);
			OHTMainForm->memoMain->Lines->Add("AMC Model");
			OHTMainForm->memoMain->Lines->Add(strOHTMODEL[OHT_Model]);

			if((OHT_NAME == OHT_NAME_STD_V80_RETICLE))
			{
              	if(OHT_NAME_STD_V80 == OHT_Model)
				{
					nError = NO_ERR;
					break;
				}
				else
				{
					nError = ERR_AMC_INIT;
				ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
						ERR_AMC_INIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				}
            }
			else
			{
				if(OHT_NAME == OHT_Model)
				{
					nError = NO_ERR;
					break;
				}
				else
				{
					nError = ERR_AMC_INIT;
			ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_AMC_INIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				}
            }

		}

	}



	if(nError != NO_ERR)
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		return nError;
	}

	if(nError == NO_ERR)
		nError = DeviceAMCSetVelCurve(AXIS_DRIVING_F, 56715*5, 1);	// 0.5m/s	// 토크
	if(nError == NO_ERR)
		nError = DeviceAMCSetVelCurve(AXIS_DRIVING_R, 56715*5, 1);	// 0.4m/s: 52151
	if(nError == NO_ERR)
		nError = DeviceAMCSetVelCurve(AXIS_HOIST, 150000, 1);	 // 0.5m/s
	if(nError == NO_ERR)
		nError = DeviceAMCSetVelCurve(AXIS_SHIFT, 150000, 1);	 // 0.5m/s


	if(nError == NO_ERR)
		nError = DeviceAMCSetActVelMargin(AXIS_DRIVING_F, 56715*5, ABORT_EVENT, 250);	// margin: 0.3m/s, Time:  250(250*0.2ms=50ms)	// 속도
	if(nError == NO_ERR)
		nError = DeviceAMCSetActVelMargin(AXIS_DRIVING_R, 56715*5, ABORT_EVENT, 250);	// margin: 0.4m/s, Time:  250(250*0.2ms=50ms)



   LeaveCriticalSection(&m_CS);
   return nError;
}

//AOHC-232 정전 복구 시, 자동 Auto 전환 기능 구현
bool AMCLibrary::IsAMCDllOpen()
{
	return  m_bAMCReady;
}

int AMCLibrary::AMCClose(void)
{
	EnterCriticalSection(&m_CS);

   if(m_bUseWatchDog==true)
   {
	   ClearWatchDogStatus();
	   DisableWatchDog();
   }

   if(m_bDriverOpen==true)
   {
	  DeviceAMCClose();
	  m_bDriverOpen = false;
   }
   if(m_bDllLoaded==true || m_hAMCLib!=NULL)
   {
	  FreeLibrary(m_hAMCLib);
	  m_hAMCLib=NULL;
	  m_bDllLoaded = false;
   }

   m_bAMCFileDownloaded = false;
   m_bAMCReady = false;

   LeaveCriticalSection(&m_CS);
   return NO_ERR;
}

/**
@brief   AMCError를 일반화 에러로 변경 시킬 때 사용하는 함수

	sync 하기 전 싱크 하려는 축 Amp disable 시킴
@author  ehoto
@date    2012.10.29
@param   nMasterAxisNum : Master축 번호
@param   nSlaveAxisNum : Slave 축 번호
@param   dSyncGain : SyncGain
@return	 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int AMCLibrary::SetSync(int nMasterAxisNum, int nSlaveAxisNum, double dSyncGain)
{
	int nTmpError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	AMC_ADD_LOG("DeviceAMCSetAmp Disable_SetSync");

	EnterCriticalSection(&m_CS);
	nTmpError = DeviceAMCSetAmpEnable(nMasterAxisNum, false);
	LeaveCriticalSection(&m_CS);
	if(nTmpError==NO_ERR)
	{
		EnterCriticalSection(&m_CS);
		nTmpError = DeviceAMCSetAmpEnable(nSlaveAxisNum, false);
		LeaveCriticalSection(&m_CS);
	}
	if(nTmpError==NO_ERR)
	{
		EnterCriticalSection(&m_CS);
		nTmpError = DeviceAMCSetSyncMapAxes(nMasterAxisNum, nSlaveAxisNum);
		LeaveCriticalSection(&m_CS);
	}
	if(nTmpError==NO_ERR)
	{
		EnterCriticalSection(&m_CS);
		nTmpError = DeviceAMCSetSyncControl(true); //sync mode enable
		LeaveCriticalSection(&m_CS);
	}
	if(nTmpError==NO_ERR)
	{
		m_fSyncGain=dSyncGain;
		EnterCriticalSection(&m_CS);
		nTmpError = DeviceAMCSetSyncGain(m_fSyncGain);	///< 1.0
		LeaveCriticalSection(&m_CS);
	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary Max S/W Limit 저장하는 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dPosLimit : Positive Limit 거리
@param   dNegLimit : Negative Limit 거리
@return  Error Code
*/
int AMCLibrary::SetMaxSWLimitForReset(int nAxisNum, double dPosLimit, double dNegLimit)
{
	if(m_bAMCReady == false)
	{
		return ERR_AMC_NOTOPEN;
	}

	m_adPositiveLimit[nAxisNum]=dPosLimit;
	m_adNegativeLimit[nAxisNum]=dNegLimit;

	return NO_ERR;
}


/**
@brief   AMCError를 일반화 에러로 변경 시킬 때 사용하는 함수
@author  ehoto
@date    2012.10.29
@param   nAmcError : AMC Error
@return	 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int ConvertAMCError(int nAmcError)
{
	int nError = 0;

	switch (nAmcError)
    {
    case 0:
		nError = NO_ERR;
        break;
    case 1:
		nError = ERR_AMC_NOT_INITIALIZED;
        break;
    case 2:
		nError = ERR_AMC_TIMEOUT_ERR;
		break;
	case 3:
		nError = ERR_AMC_INVALID_AXIS;
		break;
	case 4:
		nError =  ERR_AMC_ILLEGAL_ANALOG;
        break;
    case 5:
		nError = ERR_AMC_ILLEGAL_IO;
        break;
    case 6:
		nError = ERR_AMC_ILLEGAL_PARAMETER;
		break;
    case 7:
		nError = ERR_AMC_NO_MAP;
        break;
    case 8:
		nError = ERR_AMC_AMP_FAULT;
        break;
    case 9:
		nError = ERR_AMC_ON_MOTION;
        break;
    case 10:
		nError = ERR_AMC_NON_EXIST;
        break;
    case 11:
		nError = ERR_AMC_BOOT_OPEN_ERROR;
        break;
    case 12:
		nError = ERR_AMC_CHKSUM_OPEN_ERROR;
        break;
    case 13:
		nError = ERR_AMC_WINNT_DRIVER_OPEN_ERROR;
        break;
    case 14:
		nError = ERR_AMC_ILLEGAL_PARAM_MOVE_DS;
        break;
    case 15:
		nError = ERR_AMC_BUSY_PROFILE_GEN_MOVEX;
        break;
	case 17:
//		nError = AMC_VERSION_ERROR;
        break;
    case 30:
		nError = ERR_MMC_MOVE_LASTDS;
        break;
    case 31:
		nError = ERR_MMC_MUTEXLOCK_ERROR;
        break;
    case 32:
		nError = ERR_MMC_FILTERING_ERROR;
        break;
    case -1:
		nError = ERR_AMC_FUNC_ERR;
		break;
	// 아래 확인 필요
	case 901 :
		nError = ERR_AMC_RCVFORMAT1;
		break;
//	default:
//		nError = ERR_AMC_NOTDEFINED;
//        break;
    }

//	if(nError != NO_ERR)
//	{
//		ADD_FDC_LOG("ERR/%d/1/0",nError)
//	}

	return nError;
}

/**
@brief   IO의 해당 Output을 On/Off 시킬 때 사용하는 함수
@author  ehoto
@date    2012.10.29
@param   index : DEF_HW.h에 구현된 ENUM 변수 OUTPUT 사용(On/Off 하고자 하는 번호 기재)
@param   OnOff : 해당 출력은 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
int AMCLibrary::SetBit(int nIndex, bool bOnOff, bool bOBSControl)
{
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	if(m_bAMCReady == false)
	{
		return ERR_AMC_NOTOPEN;
	}

	int nTmpError = 0;
	int nArrayNum = 0, nMaskingNum = 0;

	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetOutBit = true;


	nArrayNum = nIndex/32;
	nMaskingNum = (0x01<<(nIndex%32));



	if(bOnOff==true)
	{
		m_anOutput_CMD[nArrayNum] = m_anOutput_CMD[nArrayNum]|nMaskingNum;		//|(0000...010...0000)
	}
	else
	{
		m_anOutput_CMD[nArrayNum] = m_anOutput_CMD[nArrayNum]&(~nMaskingNum);	//&(1111...101...1111)
	}

	if(bOBSControl == true)
	{
		m_anOutput_CMD[MAX_COUNT_IO_OUTPUT-1] = m_anOutput_CMD[MAX_COUNT_IO_OUTPUT-1]|0x8000;
	}
	
	memcpy(QueuingAmcCmd.DataUnion.Output, m_anOutput_CMD, sizeof(int)*4);

	if(bOBSControl == true)
	{
		m_anOutput_CMD[MAX_COUNT_IO_OUTPUT-1] = m_anOutput_CMD[MAX_COUNT_IO_OUTPUT-1]&(~0x8000);
	}

	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

//	AMC_ADD_LOG("AMC SetBit Error:%d %d %d", bOnOff,nIndex,bOBSControl);
//	if(nTmpError!=NO_ERR)  nAMCBufferError = ERR_INVALID_RESPONS;

	return ConvertAMCError(nTmpError);
}

/**
@brief   IO의 해당 Output을 On/Off 시킬 때 사용하는 함수
@author  ehoto
@date    2012.10.29
@param   index : DEF_HW.h에 구현된 ENUM 변수 OUTPUT 사용(On/Off 하고자 하는 번호 기재)
@param   OnOff : 해당 출력은 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
bool AMCLibrary::GetInputBit(int nIndex)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	int nArrayNum = 0, nMaskingNum = 0;
	nArrayNum = nIndex/32;
	nMaskingNum = (0x01<<(nIndex%32));

	return m_anInput[nArrayNum]&nMaskingNum;
}

/**
@brief   IO의 해당 Output을 On/Off 시킬 때 사용하는 함수
@author  ehoto
@date    2012.10.29
@param   index : DEF_HW.h에 구현된 ENUM 변수 OUTPUT 사용(On/Off 하고자 하는 번호 기재)
@param   OnOff : 해당 출력은 On 하려면 True, Off 하려면 false.
@return	 동작에 문제 없으면 0 리턴, 문제 발생 시 해당 ErrorCode 리턴.
*/
bool AMCLibrary::GetOutputBit(int nIndex)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	int nArrayNum = 0, nMaskingNum = 0;
	nArrayNum = nIndex/32;
	nMaskingNum = (0x01<<(nIndex%32));

	return m_anOutput[nArrayNum]&nMaskingNum;
}



/**
@brief   AMCLibrary 초기화 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호
@return  Error Code
*/
int AMCLibrary::Init(int nAxisNum)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	return Reset(nAxisNum);
}

/// 상태 확인
/**
@brief   AMCLibrary 동작 확인 하는 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호
@return  Error Code
*/
bool AMCLibrary::IsRunning(int nAxisNum)
{
	if(m_bAMCReady == false) return false;
	return m_abIsRunning[nAxisNum];
}

/**
@brief   AMCLibrary 정위치 확인 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호
@return  Error Code
*/
bool AMCLibrary::IsInPosition(int nAxisNum)
{
	if(m_bAMCReady == false) return false;
	return m_abInPosition[nAxisNum];
}

/**
@brief   AMCLibrary AMP On/Off 상태 확인 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@return  Error Code
*/
bool AMCLibrary::IsAMPEnable(int nAxisNum)
{
	if(m_bAMCReady == false) return false;

	return m_abAmpEnabled[nAxisNum];
}

/**
@brief   AMCLibrary 현재 위치 확인 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@return  현재 위치
*/
double AMCLibrary::GetCurrentPosition(int nAxisNum)
{
	if(m_bAMCReady == false) return 0.0;
	return m_adCurrentPosition[nAxisNum];
}

/**
@brief   AMCLibrary Small Add Move 시 센서가 감지된 위치 확인 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@return  Small Add Move 시 센서가 감지된 위치을 표시함
*/
double AMCLibrary::GetSmallAddCheckPosition(int nAxisNum)
{
	if(m_bAMCReady == false) return 0.0;
	return m_adPositionOfSmallAddCheck[nAxisNum];
}

/**
@brief   AMCLibrary 현재 이전 위치 확인 함수
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : 축 번호 (0~3)
@return  현재 이전 위치
*/
double AMCLibrary::GetPrePosition(int nAxisNum)
{
	if(m_bAMCReady == false)
		return 0.0;
	else
		return m_adPrePosition[nAxisNum];
}

/**
@brief   Reset하기전까지 누적한 값
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : 축 번호 (0~3)
@return  누적 position값
*/
double AMCLibrary::GetTotalPosition(int nAxisNum)
{
	if(m_bAMCReady == false)
		return 0.0;
	else
		return m_adTotalPosition[nAxisNum];
}

/**
@brief   TotalPosition을 Reset함
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : 축 번호 (0~3)
*/
void AMCLibrary::ResetTotalPosition(int nAxisNum)
{
	if(m_bAMCReady == true)
		m_adTotalPosition[nAxisNum] = 0.0;
}

/**
@brief   TotalMoveCount를 가져옴
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : 축 번호 (0~3)
*/
int AMCLibrary::GetTotalMoveCount(int nAxisNum)
{
	if(m_bAMCReady == false)
		return 0;
	else
		return m_anTotalMoveCount[nAxisNum];
}

/**
@brief   TotalMoveCount를 Reset함
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : 축 번호 (0~3)
*/
void AMCLibrary::ResetTotalMoveCount(int nAxisNum)
{
	if(m_bAMCReady == true)
		m_anTotalMoveCount[nAxisNum] = 0;
}

/**
@brief	 AMCLibrary 현재 위치 확인 함수
@author  임태웅
@date	 2013-04-03
@param	 nAxisNum : 축 번호 (0~3)
@return  명령 받음 위치
*/
double AMCLibrary::GetFinalPosition(int nAxisNum)
{
	if(m_bAMCReady == false) return 0.0;
	return m_adFinalPosition[nAxisNum];
}

/**
@brief   AMCLibrary 현재 속도 확인 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@return  Error Code
*/
double AMCLibrary::GetCurrentVelocity(int nAxisNum)
{
	if(m_bAMCReady == false) return 0.0;
	return m_adCurrentVelocity[nAxisNum];
}

/**
@brief   AMCLibrary Following Error 확인 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@return  Error Code
*/
double AMCLibrary::GetFollowingError(int nAxisNum)
{
	if(m_bAMCReady == false) return 0.0;

	//JIG 축 이외 에러무시

	// AXIS 무시  (지그 세팅)
	if(bAxis_Ignore == true)
		return 0;

	//조건1. 지그세팅일 때
	if(bSetting_Jig == true)
	{
		if(bUseAxis_Jig[nAxisNum] == true)
			return m_adFollowingError[nAxisNum];
		else
			return 0;
	}
	//조건2. 지그세팅이 아닐 때
	else
	{
    	return m_adFollowingError[nAxisNum];
	}

}

/// 에러 관련
/**
@brief   AMCLibrary Error 확인 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@return  Error Code
*/
int AMCLibrary::GetError(int nAxisNum)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	if(nAMCCommError !=NO_ERR) return nAMCCommError;

	if(nAMCBufferError !=NO_ERR) return nAMCBufferError;

	if(m_AMCDllLibQueue.GetReportError() !=NO_ERR)   return m_AMCDllLibQueue.GetReportError();


	// AXIS 무시  (지그 세팅)
	if(bAxis_Ignore == true)
		return 0;

	//조건1. 지그세팅일 때
	if(bSetting_Jig == true)
	{
		if(bUseAxis_Jig[nAxisNum] == true)
			return m_anAxisError[nAxisNum];
		else
			return 0;
	}
	//조건2. 지그세팅이 아닐 때
	else
	{
		return m_anAxisError[nAxisNum];
	}
}

/**
@brief   AMCLibrary reset 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@return  Error Code
*/
int AMCLibrary::Reset(int nAxisNum)
{
	int nTmpError=NO_ERR;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	EnterCriticalSection(&m_CS);

	/// 1. Clear Event
   nTmpError = DeviceAMCFramesClear(nAxisNum);
   if(nTmpError==NO_ERR) nTmpError = DeviceAMCClearStatus(nAxisNum);

	/// 2. Clear AMP FALUT
	if(nTmpError==NO_ERR)
	{
		nTmpError = DeviceAMCSetAmpEnable(nAxisNum, false);
		if(nTmpError==NO_ERR)
		{
			nTmpError = DeviceAMCFaultReset(nAxisNum);
			Sleep(10);
			nTmpError = DeviceAMCFaultSet(nAxisNum);
		}

		nAMCBufferError =NO_ERR;
	}

	LeaveCriticalSection(&m_CS);
	return ConvertAMCError(nTmpError);
}

/// Offset 관련
/**
@brief   AMCLibrary 현재 위치 Home 설정 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@return  Error Code
*/
int AMCLibrary::SetHome(int nAxisNum)
{
	int nTmpError=NO_ERR;
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	switch(nAxisNum)
	{
	case 0:
		QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
		break;
	case 1:
		QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
		break;
	case 2:
		QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
		break;
	case 3:
		QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
		break;
	}
	QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_SET_HOME;

	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

	AMC_ADD_LOG("AMC SetHome 1st:Error:%d,Axis~[%d]", nTmpError,nAxisNum);

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary Offset 설정 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dOffset : Offset 거리
@return  Error Code
*/
int AMCLibrary::SetOffset(int nAxisNum, double dOffset)
{
	int nTmpError = 0;
	int nOffset = (int)dOffset;
	int nSaveOffset=0;

	AMC_ADD_LOG("SetOffset Axis=%d, Offset=%8.1lf", nAxisNum, dOffset);


	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	EnterCriticalSection(&m_CS);
    AMC_ADD_LOG("DeviceAMCSetAmp Disable_SetOffset");
	if(nTmpError==NO_ERR) nTmpError = DeviceAMCSetAmpEnable(nAxisNum, false);
	if(nTmpError==NO_ERR) nTmpError = DeviceAMCSetEncoderOffset(nAxisNum, nOffset);
	if(nTmpError==NO_ERR) nTmpError = DeviceAMCParamSaveToEEPROM();

	LeaveCriticalSection(&m_CS);
	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary Offset 받아오는 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dOffset : Offset 거리
@return  Error Code
*/
int AMCLibrary::GetOffset(int nAxisNum, double* dOffset)
{
	int nTmpError = 0;
	int nOffset = 0;
	int nSaveOffset=0;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	AMC_ADD_LOG("GetOffset Axis=%d, Offset=%8.1lf", nAxisNum, dOffset);

	EnterCriticalSection(&m_CS);

	nTmpError = DeviceAMCGetEncoderOffset(nAxisNum, &nOffset);

	*dOffset = (double)nOffset;

	LeaveCriticalSection(&m_CS);
	return ConvertAMCError(nTmpError);
}

/// Limit 관련
/**
@brief   AMCLibrary Postive S/W Limit 설정 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dLimit : Limit 거리
@return  Error Code
*/
int AMCLibrary::SetPositiveSWLimit(int nAxisNum, double dLimit)
{
	int nTmpError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	EnterCriticalSection(&m_CS);

	nTmpError = DeviceAMCSetPositiveSWLimit(nAxisNum,dLimit,STOP_EVENT);
	AMC_ADD_LOG("[Axis%d] SW_PosLimit: %8.1lf",nAxisNum,dLimit);

	LeaveCriticalSection(&m_CS);
	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary Negative S/W Limit 설정 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dLimit : Limit 거리
@return  Error Code
*/
int AMCLibrary::SetNegativeSWLimit(int nAxisNum, double dLimit)
{
	int nTmpError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	EnterCriticalSection(&m_CS);

	nTmpError = DeviceAMCSetNegativeSWLimit(nAxisNum,dLimit,STOP_EVENT);
	AMC_ADD_LOG("[Axis%d] SW_NegLimit: %8.1lf",nAxisNum,dLimit);

	LeaveCriticalSection(&m_CS);
	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary S/W Limit 확인 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dPosLimit : Positive Limit 거리
@param   dNegLimit : Negative Limit 거리
@return  Error Code
*/
int AMCLibrary::GetSWLimit(int nAxisNum, double& dPosLimit, double& dNegLimit)
{
	int nTmpError = 0;
	double dTmpPosLimit=0.0, dTmpNegLimit=0.0;
	int nPosAct=0, nNegAct=0;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	EnterCriticalSection(&m_CS);
	nTmpError = DeviceAMCGetPositiveSWLimit(nAxisNum,&dTmpPosLimit,&nPosAct);
	if(nTmpError==NO_ERR)
	{
		nTmpError = DeviceAMCGetNegativeSWLimit(nAxisNum,&dTmpNegLimit,&nNegAct);
    }
	LeaveCriticalSection(&m_CS);

	dPosLimit = dTmpPosLimit;
	dNegLimit = dTmpNegLimit;

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary S/W Limit 해제 함수

	설정되어 있는 S/W Limit의 Max 값 대입
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@return  Error Code
*/
int AMCLibrary::ResetSWLimit(int nAxisNum)
{
	int nTmpError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	AMC_ADD_LOG("DeviceAMCSetAmpDisable_Reset");
	EnterCriticalSection(&m_CS);
	nTmpError = DeviceAMCSetPositiveSWLimit(nAxisNum,m_adPositiveLimit[nAxisNum],STOP_EVENT);
	if(nTmpError==NO_ERR)
		nTmpError = DeviceAMCSetNegativeSWLimit(nAxisNum,m_adNegativeLimit[nAxisNum],STOP_EVENT);
	LeaveCriticalSection(&m_CS);

	return ConvertAMCError(nTmpError);
}

/// 구동 관련 - 단위: 거리(mm), 속도(mm/msec), 가속도(mm/msec2), 시간(msec)
/**
@brief   AMCLibrary Amp On/Off 설정 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   bValue : On/Off 값
@return  Error Code
*/
int AMCLibrary::SetAMPEnable(int nAxisNum, bool bValue)
{
	int nTmpError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	EnterCriticalSection(&m_CS);
	nTmpError = ConvertAMCError(DeviceAMCSetAmpEnable(nAxisNum, bValue));
	LeaveCriticalSection(&m_CS);

//	AMC_ADD_LOG("DeviceAMCSetAmpEnable_Check : %d",bValue);   ////timy 불필요한 LOG 삭제

	return nTmpError;
}

// 가속도 기반
/**
@brief   AMCLibrary 양의 방향으로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dAccel : 가속도
@param   dVel : 속도
@return  Error Code
*/
int AMCLibrary::MovePositive(int nAxisNum, double dAccel, double dVel)
{
	int nTmpError = 0;
	double dAccel_CtsPerSec2 = 0.0, dVel_CtsPerSec = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	/// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);


	/// 2. DeviceAMCPMove
	if(nTmpError==NO_ERR)
	{
		dAccel_CtsPerSec2 = dAccel*1000000.0;	///< Cts/sec2
		dVel_CtsPerSec = dVel*1000.0;			///< Cts/sec

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_P;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel = dAccel_CtsPerSec2;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed = dVel_CtsPerSec;

		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveP 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf]", nTmpError,nAxisNum,dAccel_CtsPerSec2,dVel_CtsPerSec);
	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary 음의 방향으로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dAccel : 가속도
@param   dVel : 속도
@return  Error Code
*/
int AMCLibrary::MoveNegative(int nAxisNum, double dAccel, double dVel)
{
	int nTmpError = 0;
	double dAccel_CtsPerSec2 = 0.0, dVel_CtsPerSec = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);

	// 2. DeviceAMCNMove
	if(nTmpError==NO_ERR)
	{
		dAccel_CtsPerSec2 = dAccel*1000000.0;	///< Cts/msec2 --> Cts/sec2
		dVel_CtsPerSec =dVel*1000.0;			///< Cts/msec  --> Cts/sec

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_N;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel = dAccel_CtsPerSec2;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed = dVel_CtsPerSec;

		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveN 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf]", nTmpError,nAxisNum,dAccel_CtsPerSec2,dVel_CtsPerSec);

	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary 정지하는 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dAccel : 감속도
@return  Error Code
*/
int AMCLibrary::MoveStop(int nAxisNum, double dAccel)
{
	int nTmpError = 0;
	double dAccel_CtsPerSec2 = 0;	///< Cts/sec2, Cts/sec
	const double dDefault_ACCEL=340119.0; //3msec2
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	/// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);

	/// 2. DeviceAMCNMove
	if(nTmpError==NO_ERR)
	{
		if(dAccel!=0.0)
		{
			dAccel_CtsPerSec2 = dAccel*1000000.0;	// UserUnit/msec2 --> Cts/sec2
		}
		else
		{
			///@todo: 하드 코딩 값 변경 필요
			dAccel_CtsPerSec2 = 340119.0; 			//3msec2;
		}

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_S;
		QueuingAmcCmd.DataUnion.CmdAxis.Decel= dAccel_CtsPerSec2;

		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveS:Err:%d[%d:%8.1lf]", nTmpError,nAxisNum,dAccel_CtsPerSec2);

	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary 거리를 가속도/감속도/속도 프로파일로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dAccel_Up : 가속도
@param   dAccel_Down : 감속도
@param   dVel : 속도
@param   dDistance : 이동 거리
@return  Error Code
*/
int AMCLibrary::MoveDistance(int nAxisNum, double dAccel_Up, double dAccel_Down, double dVel_CtsPerMsec, double dDistance_Cts, TRIGGER_CTRL_INFO TriggerCtrlInfo)
{
	int nTmpError=NO_ERR;
	double dAccel_CtsPerSec2 = 0.0, dDecel_CtsPerSec2 = 0.0, dVel_CtsPerSec = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;
	TRIGGER_CTRL_INFO_UNION TrigerCtrlInfoUnion;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	TrigerCtrlInfoUnion.TriggerInfo = TriggerCtrlInfo;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	static bool bCheckPulse = false;

	/// 1. DeviceAMCDSMove
	if(nTmpError==NO_ERR)
	{
		dAccel_CtsPerSec2 = dAccel_Up*1000000.0;		///< Cts/msec2 --> Cts/sec2
		dDecel_CtsPerSec2 = dAccel_Down*1000000.0;		///< Cts/msec2 --> Cts/sec2
		dVel_CtsPerSec = dVel_CtsPerMsec*1000.0;		///< Cts/msec  --> Cts/sec

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}

		//E2314에러 개선 대응
		if((nAxisNum == 0) &&(TrigerCtrlInfoUnion.TriggerInfo.UseSmallAdd ==1))
		{
			if(bCheckPulse == true)
			{
				TrigerCtrlInfoUnion.TriggerInfo.AdditionalDist = TrigerCtrlInfoUnion.TriggerInfo.AdditionalDist+1;

				bCheckPulse = false;
			}
			else
				bCheckPulse = true;
		}


		QueuingAmcCmd.DataUnion.CmdAxis.CmdType			= MMC_CMD_AXIS_MOVE_DS;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel			= dAccel_CtsPerSec2;
		QueuingAmcCmd.DataUnion.CmdAxis.Decel			= dDecel_CtsPerSec2;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed			= dVel_CtsPerSec;
		QueuingAmcCmd.DataUnion.CmdAxis.Distance		= dDistance_Cts;
		QueuingAmcCmd.DataUnion.CmdAxis.TriggerCtrlInfo = TrigerCtrlInfoUnion.Cmd;



		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveDS 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf:%8.1lf:%8.1lf] EX[%d:%d/%d:%d:%d:%d]",
            nTmpError,
			nAxisNum,
			dAccel_CtsPerSec2,
			dDecel_CtsPerSec2,
			dVel_CtsPerSec,
            dDistance_Cts,
			TrigerCtrlInfoUnion.TriggerInfo.StartOnSpeedDown,
			TrigerCtrlInfoUnion.TriggerInfo.UseCurrSpeed,
			TrigerCtrlInfoUnion.TriggerInfo.UseSmallAdd, TrigerCtrlInfoUnion.TriggerInfo.IsTriggerOnRisingEdge,
			TrigerCtrlInfoUnion.TriggerInfo.TriggerIONumber,TrigerCtrlInfoUnion.TriggerInfo.AdditionalDist);

	}

	return ConvertAMCError(nTmpError);
}

// 가속 시간 기반
/**
@brief   AMCLibrary 일정 시간 가속하여 양의 방향으로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dwTime : 가속 시간
@param   dVel : 속도
@return  Error Code
*/
int AMCLibrary::MovePositive(int nAxisNum, DWORD dwTime, double dVel)
{
	int nTmpError=NO_ERR;
	double dTime_sec = 0.0, dVel_CtsPerSec = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	/// 1. DeviceAMCPMove
	if(nTmpError==NO_ERR)
	{
		dTime_sec = dwTime;	///< msec
		dVel_CtsPerSec = dVel*1000.0;		///< msec  --> sec

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_TIME_P;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel = dTime_sec;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed = dVel_CtsPerSec;

		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MovePT 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf]", nTmpError,nAxisNum,dTime_sec,dVel_CtsPerSec);
	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary 일정 시간 가속하여 음의 방향으로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dwTime : 가속 시간
@param   dVel : 속도
@return  Error Code
*/
int AMCLibrary::MoveNegative(int nAxisNum, DWORD dwTime, double dVel)
{
	int nTmpError = 0;
	double dTime_sec = 0.0, dVel_CtsPerSec = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	/// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);

	/// 2. DeviceAMCPMove
	if(nTmpError==NO_ERR)
	{
		dTime_sec = dwTime;	///< msec
		dVel_CtsPerSec = dVel*1000.0;			///< Cts/msec  --> Cts/sec

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_TIME_N;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel = dTime_sec;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed = dVel_CtsPerSec;


		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveNT 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf]", nTmpError,nAxisNum,dTime_sec,dVel_CtsPerSec);
	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary 일정 시간 감속하여 정지하는 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dwTime : 감속 시간
@param   dVel : 속도
@return  Error Code
*/
int AMCLibrary::MoveStop(int nAxisNum, DWORD dwTime)
{
	int nTmpError = 0;
	double dTime_sec = 0.0;	///< Cts/sec2, Cts/sec
	const double dDefault_ACCEL=340119.0; //3msec2
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	/// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);


	/// 2. DeviceAMCNMove
	if(nTmpError==NO_ERR)
	{
		if(dwTime!=0.0)
		{
			dTime_sec = dwTime;		///< msec
		}
		else
		{
			dTime_sec = 3000.0;	///< UserUnit/msec2 --> Cts/sec2 ( 3m/s )
		}

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_TIME_S;
		QueuingAmcCmd.DataUnion.CmdAxis.Decel = dTime_sec;

		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveST:Err:%d[%d:%8.1lf]", nTmpError,nAxisNum,dTime_sec);
	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary 거리를 가속시간/감속시간/속도 프로파일로 이동하는 함수
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   dwTime_Up : 가속 시간
@param   dwTime_Down : 감속 시간
@param   dVel : 속도
@param   dDistance : 이동 거리
@return  Error Code
*/
int AMCLibrary::MoveDistance(int nAxisNum, DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance)
{
	int nTmpError = 0;
	double dAccel_Time_sec = 0.0, dDecel_Time_sec = 0.0, dVel_CtsPerSec = 0.0, dDistance_Cts = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	/// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);


	/// 2. DeviceAMCDSMove
	if(nTmpError==NO_ERR)
	{
		dAccel_Time_sec = (double)dwTime_Up;		///< msec
		dDecel_Time_sec = (double)dwTime_Down;		///< msec
		dVel_CtsPerSec = dVel*1000.0;		///< UserUnit/msec  --> Cts/sec
		dDistance_Cts = dDistance;

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_TIME_DS;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel = dAccel_Time_sec;
		QueuingAmcCmd.DataUnion.CmdAxis.Decel = dDecel_Time_sec;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed = dVel_CtsPerSec;
		QueuingAmcCmd.DataUnion.CmdAxis.Distance = dDistance_Cts;


		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		 ////timy 불필요한 LOG 삭제
//		AMC_ADD_LOG("AMC MoveDST 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf:%8.1lf:%8.1lf]", nTmpError,nAxisNum,dAccel_Time_sec,dDecel_Time_sec,dVel_CtsPerSec,dDistance_Cts);

	}

	return ConvertAMCError(nTmpError);
}

// 통신으로 제어시에서 구현 시 사용 함수
/**
@brief   AMCLibrary 에 프로토콜로 명령 전달
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   pSendData : 보내는 Data 가 들어있는 Char 배열
@param   nSendLen : 보내는 Data 길이
@param   pRecvData : 받는 Data 가 들어있는 Char 배열
@param   nRecvLen : 받는 Data 길이
@return  Error Code
*/
int AMCLibrary::SendData(int nAxisNum, char* pSendData, int nSendLen, char* pRecvData, int* nRecvLen)
{
	int nTmpError = 0;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	EnterCriticalSection(&m_CS);
	nTmpError = DeviceAMCCmdGenerate(pSendData, nSendLen, pRecvData, nRecvLen);
	LeaveCriticalSection(&m_CS);
	m_dwSndTime = timeGetTime();
	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary 에 프로토콜로 명령 전달
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   pSendData : 보내는 Data 가 들어있는 Char 배열
@param   nSendLen : 보내는 Data 길이
@param   pRecvData : 받는 Data 가 들어있는 Char 배열
@param   nRecvLen : 받는 Data 길이
@return  Error Code
*/
//AOHC-216 AMC CommFail, CRC 에러명 수정
bool AMCLibrary::UpdateData(int* pInputBuff, int& nInputCount, int* pOutputBuff, int& nOutCount, int *_out_pErrCode)
{
    //AOHC-204 Thread 별 수행 시간 감시 기능
	static TimeChk timeChk;
	timeChk.End(0);

	// Thread Cyele Time 기록
	static ThreadTimeMonitor tmon;
	tmon.Execute("AmcLibT", timeChk.Get(0));

	timeChk.Start(0);

	int nTmpError = NO_ERR;
	int nTmpFrontError = NO_ERR;
    int nSuccess = 0;
	static AMC_DATA PreRcvData; // 이전 수신한 AMC Data
	static DWORD dwAMCTrigerTime = timeGetTime();     //함수 진입 시간.
	static DWORD dwAMCTrigerTimeSend = timeGetTime();     //함수 진입 시간.
	static int  nAMCIgnorSendCount = 0;     //함수 진입 시간.
	static int	PreInput[4];
	static int	PreOutput[4];
	static int	PreNodeCount = 0;
	bool bCheckAMCLog = false;

	AMC_DATA RcvData;
	AMC_DATA TempData;
	memset(&TempData, 0x00, sizeof(AMC_DATA));
//  VOLTAGE_MONITOR_INFO_UNION VoltageUnion;

try{
    // by zzang9un 2013.07.09 : Mask변수 대입
	//UINT Mask[4] = {0xF0000080, 0x00759FBF,0x00, 0x00};   // UBG B접
    UINT Mask[4] = {0x00, 0x00, 0x00, 0x00};

	// 변수 초기화
    nInputCount = MAX_IO_INT_COUNT;
	nOutCount = MAX_IO_INT_COUNT;

	memcpy(&Mask[0], &m_Input1Mask_R01, sizeof(m_Input1Mask_R01));
	memcpy(&Mask[1], &m_Input2Mask_R01, sizeof(m_Input2Mask_R01));
	memcpy(&Mask[2], &m_Input3Mask_R01, sizeof(m_Input3Mask_R01));

	if(m_bAMCReady == false)
	{
		*_out_pErrCode = ERR_AMC_NOTOPEN;
		return false;
	}

	if(m_AMCDllLibQueue.makeData(&m_SndCmd ,&nTmpError)==0)
	{
		*_out_pErrCode = nTmpError;
		return false;
	}

    if((m_SndCmd.Cmd&(0x3C))!=0)
    {
		// 축 정보 명령 로그 작성
		for(int n=0; n<4;n++)
        {
			if(m_SndCmd.Cmd&(0x01<<(2+n)))
			{
			////timy 불필요한 LOG 삭제
//				AMC_ADD_LOG("AMC m_SndCmd:%o AXIS %d(%d::%d:%d:%d:%d)", m_SndCmd.Cmd, n, m_SndCmd.CmdAxis[n].CmdType,
//				m_SndCmd.CmdAxis[n].Accel, m_SndCmd.CmdAxis[n].Decel, m_SndCmd.CmdAxis[n].Speed, m_SndCmd.CmdAxis[n].Distance );
			}
		}
	}
    // Node Count 명령 로그 작성
	if(m_SndCmd.Cmd&(1<<CMD_TYPE_CLEAR_NODE_COUNT))
	{
        AMC_ADD_LOG("AMC Clear Node Count:%o ", m_SndCmd.Cmd );
	}

	if(m_SndCmd.Cmd&(1<<CMD_TYPE_SET_TORQUE_LIMIT_RESPONSE))
	{
		AMC_ADD_LOG("AMC Set_Torque_Limit:%o ", m_SndCmd.Cmd );
	}

	if(m_SndCmd.Cmd&(1<<CMD_TYPE_SET_TORQUE_LIMIT_REALEASE_RESPONSE))
	{
		AMC_ADD_LOG("AMC Set_Torque_Limit_REALEASE:%o ", m_SndCmd.Cmd );
	}


    if(m_SndCmd.Cmd&(1<<CMD_TYPE_SET_DETECT_INFO))
	{
#if(USE_CHANGENODE == 1)
		AMC_ADD_LOG("AMC Detect Info:%o [%d:%d:%d:%d:%d]"
            , m_SndCmd.Cmd
            , (int)m_SndCmd.DetectInfo.UseFollowControlFlag
            , (int)m_SndCmd.DetectInfo.OHTDetectType
			, (int)m_SndCmd.DetectInfo.OBSDetectType
            , m_SndCmd.DetectInfo.NodeCount
			, m_SndCmd.DetectInfo.DistSumOfStraightPath);

		if(m_SndCmd.OptNodeInfo.DistON > 0)
		{
			AMC_ADD_LOG("Optional Node Info: [%d] [%d:%d] [%d:%d]"

				, m_SndCmd.OptNodeInfo.NodeType
				, (int)m_SndCmd.OptNodeInfo.OHTDetectTypeOpt
				, (int)m_SndCmd.OptNodeInfo.OBSDetectTypeOpt
				, m_SndCmd.OptNodeInfo.DistON
				, m_SndCmd.OptNodeInfo.DistAfterON);
		}
#else
		AMC_ADD_LOG("AMC Detect Info:%o [%d:%d:%d:%d]"
			, m_SndCmd.Cmd
			, (int)m_SndCmd.DetectInfo.UseFollowControlFlag
			, (int)m_SndCmd.DetectInfo.Type
			, m_SndCmd.DetectInfo.NodeCount
			, m_SndCmd.DetectInfo.DistSumOfStraightPath);
#endif
	}

	#ifdef USE_CRC_CHECK      // 사업부에서 CRC 대응 될때 ifdef 해제 예정
	m_SndCmd.MakeCRC16();	// Send Packet CRC 값 생성
	#endif


	// AMC CALL ********************************************
	EnterCriticalSection(&m_CS);
	nSuccess = DeviceAMCGetDatas(&m_SndCmd, &RcvData);
	LeaveCriticalSection(&m_CS);

	#ifdef USE_CRC_CHECK      // 사업부에서 CRC 대응 될때 ifdef 해제 예정
	bool bCrcOK = RcvData.CrcCheck();	// Recv Packet CRC 체크
	#endif

	//AOHC-216 AMC CommFail, CRC 에러명 수정
	if(!nSuccess)
	{
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwAMCTrigerTime)> 3000)
		{
			Get_LogDpram();
			m_bAMCReady = false;
			nAMCCommError = ERR_AMC_COMM;
			*_out_pErrCode = ERR_AMC_COMM;
		}
		AMC_ADD_LOG("[Fail]DeviceAMCGetDatas:%x ,%d", m_SndCmd.Cmd,nSuccess);
		OHTMainForm->GetStatusCheckThread()->AMCCommfail();
		return false;
	}

	// 2019.02.14 Null Data가 오더라도 CRC Check에서 걸리기 때문에 주석처리함
	//
	//if(memcmp(&RcvData, &TempData, sizeof(AMC_DATA))==0)  // Null Check!
	//{
	//	nAMCCommError = ERR_AMC_RCVFORMAT;
	//	*_out_pErrCode = ERR_AMC_RCVFORMAT;
	//	return false;
	//}

	#ifdef USE_CRC_CHECK      // 사업부에서 CRC 대응 될때 ifdef 해제 예정
	// CRC 실패 Case
	static int nCrcRetryCnt = 0;
	if(bCrcOK == false)
	{
		nCrcRetryCnt++;
		AMC_ADD_LOG("[CRC_ERROR] Retry :%d", nCrcRetryCnt);
		if(nCrcRetryCnt > 5)
		{
			nCrcRetryCnt = 0;
			AMC_ADD_LOG("[CRC_ERROR] Retry Count Over!!!");
			nAMCCommError = ERR_AMC_INVALID_RESPONSE;
			*_out_pErrCode = ERR_AMC_INVALID_RESPONSE;
			m_bAMCReady = false;
        }
		return false;
	}
	nCrcRetryCnt = 0;
	#endif

	// 입출력이 모두 0인경우
	if(((RcvData.Output[0] ==0) &&(RcvData.Output[1] ==0)&& (RcvData.Output[2]==0) && (RcvData.Input[0]==0) &&(RcvData.Input[1]==0)&& (RcvData.Input[2]==0))
	|| ((RcvData.Output[0] ==0xFFFFFFFF) &&(RcvData.Output[1] ==0xFFFFFFFF)&& (RcvData.Output[2]==0xFFFFFFFF) && (RcvData.Input[0]==0xFFFFFFFF) &&(RcvData.Input[1]==0xFFFFFFFF)&& (RcvData.Input[2]==0xFFFFFFFF)))
   {
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwAMCTrigerTime)> 50)
		{
			Get_LogDpram();
			nAMCCommError = ERR_AMC_COMM;
			*_out_pErrCode = ERR_AMC_COMM;
		}
		return false;
   }

	//AMC에서 받은 NodeCount의 갯수가 비정상적인 경우 2018.07.23 JRESPECT.IM
	if(RcvData.NodeCheckData.NodeCountOfPreCheck < 0 || RcvData.NodeCheckData.NodeCount < 0 || RcvData.NodeCheckData.NodeCount > MAX_NODE_COUNT || RcvData.NodeCheckData.NodeCountOfPreCheck > MAX_NODE_COUNT)
	{
		if(AbnormalNodeCount > 1)
		{
			AbnormalNodeCount = 0;
			AMC_ADD_LOG("AbnormalNodeNodeCount : %d %d", RcvData.NodeCheckData.NodeCountOfPreCheck , RcvData.NodeCheckData.NodeCount);
			*_out_pErrCode = ERR_AMC_RCVFORMAT1; //ErrorCode어떤걸로 해야할지 물어보기
		}
		else
		{
			AbnormalNodeCount++;
		}

		return false;
	}

    // -------------------------------------------
	// [BEGIN] by zzang9un 2014.01.18 : AMC 송신 명령 Logging
	if(m_SndCmd.Cmd)
	{
		// Log 축소 (100msec)

		if(OHTUTILITY::PreventOverflowGetTimeMs(dwAMCTrigerTimeSend)> 100)
		{
#if(USE_CHANGENODE == 1)
				// LogVersion 2.0 SHCHO 2019.03 ************
			// AMC SEND 로그 작성시 주의사항
			// 진법 계산때문에 반드시 10진수만 int만 사용해야함. 16진수, 소수점, 안됨!! 음수는 됨.
			char orgLog[2048], encLog[2048];
			memset(orgLog, 0, sizeof(orgLog));
			memset(encLog, 0, sizeof(encLog));
			sprintf(orgLog, "/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",
				m_SndCmd.Cmd,
				m_SndCmd.Output[0], m_SndCmd.Output[1], m_SndCmd.Output[2], m_SndCmd.Output[3],
				m_SndCmd.CmdAxis[0].CmdType, m_SndCmd.CmdAxis[0].Accel, m_SndCmd.CmdAxis[0].Decel, m_SndCmd.CmdAxis[0].Speed, m_SndCmd.CmdAxis[0].Distance, m_SndCmd.CmdAxis[0].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[1].CmdType, m_SndCmd.CmdAxis[1].Accel, m_SndCmd.CmdAxis[1].Decel, m_SndCmd.CmdAxis[1].Speed, m_SndCmd.CmdAxis[1].Distance, m_SndCmd.CmdAxis[1].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[2].CmdType, m_SndCmd.CmdAxis[2].Accel, m_SndCmd.CmdAxis[2].Decel, m_SndCmd.CmdAxis[2].Speed, m_SndCmd.CmdAxis[2].Distance, m_SndCmd.CmdAxis[2].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[3].CmdType, m_SndCmd.CmdAxis[3].Accel, m_SndCmd.CmdAxis[3].Decel, m_SndCmd.CmdAxis[3].Speed, m_SndCmd.CmdAxis[3].Distance, m_SndCmd.CmdAxis[3].TriggerCtrlInfo,
				m_SndCmd.DetectInfo.UseFollowControlFlag,
				m_SndCmd.DetectInfo.OHTDetectType,
				m_SndCmd.DetectInfo.OBSDetectType,
				m_SndCmd.DetectInfo.NodeCount,
				m_SndCmd.DetectInfo.DistSumOfStraightPath,
				nAMCIgnorSendCount);
#else
				// LogVersion 2.0 SHCHO 2019.03 ************
			// AMC SEND 로그 작성시 주의사항
			// 진법 계산때문에 반드시 10진수만 int만 사용해야함. 16진수, 소수점, 안됨!! 음수는 됨.
			char orgLog[2048], encLog[2048];
			memset(orgLog, 0, sizeof(orgLog));
			memset(encLog, 0, sizeof(encLog));
			sprintf(orgLog, "/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",
				m_SndCmd.Cmd,
				m_SndCmd.Output[0], m_SndCmd.Output[1], m_SndCmd.Output[2], m_SndCmd.Output[3],
				m_SndCmd.CmdAxis[0].CmdType, m_SndCmd.CmdAxis[0].Accel, m_SndCmd.CmdAxis[0].Decel, m_SndCmd.CmdAxis[0].Speed, m_SndCmd.CmdAxis[0].Distance, m_SndCmd.CmdAxis[0].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[1].CmdType, m_SndCmd.CmdAxis[1].Accel, m_SndCmd.CmdAxis[1].Decel, m_SndCmd.CmdAxis[1].Speed, m_SndCmd.CmdAxis[1].Distance, m_SndCmd.CmdAxis[1].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[2].CmdType, m_SndCmd.CmdAxis[2].Accel, m_SndCmd.CmdAxis[2].Decel, m_SndCmd.CmdAxis[2].Speed, m_SndCmd.CmdAxis[2].Distance, m_SndCmd.CmdAxis[2].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[3].CmdType, m_SndCmd.CmdAxis[3].Accel, m_SndCmd.CmdAxis[3].Decel, m_SndCmd.CmdAxis[3].Speed, m_SndCmd.CmdAxis[3].Distance, m_SndCmd.CmdAxis[3].TriggerCtrlInfo,
				m_SndCmd.DetectInfo.UseFollowControlFlag,
				m_SndCmd.DetectInfo.Type,
				m_SndCmd.DetectInfo.NodeCount,
				m_SndCmd.DetectInfo.DistSumOfStraightPath,
				nAMCIgnorSendCount);
#endif


			NumericSystem::EncodingLog(orgLog, encLog, sizeof(encLog));

            			//Null 접근 부분 보완 처리. 20191204
			if(gm_LogManager!=NULL && m_pLogAmcSendPr!=NULL && gm_LogManager->Get_LogFolder())
			{
				m_pLogAmcSendPr->put(strlen(encLog), encLog);
			}

			//m_pLogAmcSendPr_Old->put(strlen(orgLog), orgLog);

                nAMCIgnorSendCount = 0;
				dwAMCTrigerTimeSend = timeGetTime();
		}else
			nAMCIgnorSendCount++;
	}
	// [END] by zzang9un 2014.01.18 : AMC 송신 명령 Logging
	// -------------------------------------------

	// 응답이 성공일 경우 명령 초기화
	memset(&m_SndCmd,0,sizeof(AMC_CMD));

	for(int nAxis=0; nAxis<4; nAxis++)
	{

		if((bUseAxis_Jig[nAxis] == false && bSetting_Jig == true)) // 해당축이 아니면 나머지 Aixs 무시 (지그세팅)
		{
			continue;
		}

		m_abIsRunning[nAxis]                =   (bool)RcvData.Running[nAxis];                   ///< 주행 중인지
		m_abInPosition[nAxis]               =   (bool)RcvData.InPosition[nAxis];                ///< 도착 완료 상태인지
		m_abAmpEnabled[nAxis]               =   (bool)RcvData.AmpEnable[nAxis];                 ///< AMP Enable 상태인지    : [임시]역으로 변환해둠
//      m_abAmpEnabled[nAxis]               =   (bool)RcvData.Source[nAxis]&0x0800; 			///< AMP Enable 상태인지
		m_adPrePosition[nAxis]				= 	m_adCurrentPosition[nAxis];
		m_adCurrentPosition[nAxis]          =   (double)RcvData.Position[nAxis];                ///< 현재 위치: Cts
		m_adFinalPosition[nAxis]            =   (double)RcvData.FinalPos[nAxis];                ///< 최종 위치: Cts
		m_adCurrentVelocity[nAxis]          =   (double)RcvData.Velocity[nAxis]/1000.0;         ///< 현재 속도: Cts/sec
		m_adFollowingError[nAxis]           =   (double)RcvData.FollowingError[nAxis];          ///< 현재 Following Error :  Cts
		m_adPositionOfSmallAddCheck[nAxis]  =   (double)RcvData.PositionOfSmallAddCheck[nAxis]; ///< 현재 위치: Cts

		if (nAxis ==0)
		{
			m_FollowigAxis1 = RcvData.FollowingError[nAxis]; //AOHC-196 사전 로그 추가
		}

		m_anAxisError[nAxis] = CheckError(RcvData.Source[nAxis], RcvData.State[nAxis]); ///< 축별 에러

		if(m_anAxisError[nAxis] != NO_ERR)
		{
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						m_anAxisError[nAxis],
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		}

		//이동 누적값을 구함
		if(m_abIsRunning[nAxis] == true)
		{
			double dDelta = m_adCurrentPosition[nAxis] - m_adPrePosition[nAxis];

			//주행부일 때는 차이가 음수인 경우 누적하지 않는다
			if(nAxis == AXIS_DRIVING_F || nAxis == AXIS_DRIVING_R)
			{
				if(dDelta > 0)
				{
					m_adTotalPosition[nAxis] += dDelta;
				}
			}
			else
			{
				m_adTotalPosition[nAxis] += fabs(dDelta);
			}
		}

		//명령수행 횟수 계산
		if(m_abIsRunning[nAxis] == true)
		{
			//집계가 안되었다면 카운트 후 플래그 설정
			if(m_abIsCounted[nAxis] == false)
			{
				m_abIsCounted[nAxis] = true;
				m_anTotalMoveCount[nAxis] = m_anTotalMoveCount[nAxis]+1;
			}
		}
		else
		{
			//집계가 된상태고 모터제어가 마무리 되면 플래그 해제
			if(m_abIsCounted[nAxis] == true)
			{
				m_abIsCounted[nAxis] = false;
			}
		}
		/*
		if(nAxis==1)    // Rear 축 에러 제거 -- 승준형 검토
		{
			if(m_anAxisError[nAxis]==ERR_AXIS1_F_POS_LIMIT || m_anAxisError[nAxis]==ERR_AXIS1_F_NEG_LIMIT)
			{
				m_anAxisError[nAxis]=NO_ERR;
			}
		}
		//ERR_AXIS1_R_AMP_FAULT발생한 경우 ERR_AXIS1_F_ABORT_EVENT로 Error올라오는 경우 방지 2018.07.23 JRESPECT.IM
		if(nAxis == 0)  //1축 Error 발생한 경우 nTmpFrontError에 저장
		{
		  if(nTmpError == NO_ERR && m_anAxisError[nAxis] != NO_ERR)
		  {
            m_anAxisError[nAxis] += nAxis;
			nTmpFrontError = m_anAxisError[nAxis];
          }
        }
		*/
		if(nAxis > 0)   //2축 Error까지 보고나서 Error 결정
		{
			if(nTmpError == NO_ERR && m_anAxisError[nAxis] != NO_ERR)
			{
				//1축 Error X, 나머지 Error O
				if(nTmpFrontError == NO_ERR)
				{
					m_anAxisError[nAxis] += nAxis;
					nTmpError = m_anAxisError[nAxis];
				}
				//1축 Error O
				else //if(nTmpFrontError != NO_ERR)
				{
                    //1축 Error O, 나머지 Error O중 E614인 경우
					if(nTmpFrontError == ERR_AXIS1_F_ABORT_EVENT && m_anAxisError[nAxis] == ERR_AXIS1_F_AMP_FAULT)
					{
						m_anAxisError[nAxis] += nAxis;
						nTmpError = m_anAxisError[nAxis];
					}
					/*//1축 Error O, 나머지축 Error O중 E614가 아닌 경우
					else if(m_anAxisError[nAxis] != ERR_AXIS1_F_AMP_FAULT)
					{
						nTmpError = nTmpFrontErrors;
					}
					*/
					//1축 Error O, 나머지축 Error X
					else
					{
						nTmpError = nTmpFrontError;
                    }
                }
			}
		}

		if(nTmpError != NO_ERR)
		{
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						nTmpError,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		}
	}

	for(int n=0; n<MAX_IO_INT_COUNT; n++)
	{
		m_anInput[n] = (RcvData.Input[n]^Mask[n]);
		m_anOutput[n] = RcvData.Output[n];
		pInputBuff[n] = (RcvData.Input[n]^Mask[n]);
		pOutputBuff[n] = RcvData.Output[n];

		if((PreInput[n] != RcvData.Input[n])
		 ||(PreOutput[n] != RcvData.Output[n]))
		 {
			bCheckAMCLog = true;
		 }

		PreInput[n] = RcvData.Input[n];
		PreOutput[n] = RcvData.Output[n];
	}

	nAMCPauseStatus = RcvData.PauseState;
	nAMCVHLDetectStatus = RcvData.AxisFlag2[2]; //0; 대차 무시 설정, 1; 대차 무시 해제(정상적으로 센서 감지후 정지함)
	nAMCTorqueStatus = RcvData.AxisFlag2[3];   //1 : 토크제한 리밋 설정/ 2 : 토크제한 리밋 해제   0: 초기값

	nAMCVHLStopStatus = RcvData.TargetPos[0];   //
	nAMCDecelStatus = RcvData.TargetPos[1];

	nAMCDetectStopDecelDownStatus = RcvData.AxisFlag1[2];
	nAMCDetectForceIOStatus = RcvData.AxisFlag1[3];

	if(PreNodeCount !=  RcvData.NodeCheckData.NodeCount)
	{
		bCheckAMCLog = true;
		PreNodeCount  = RcvData.NodeCheckData.NodeCount;
	}

	//AMC에서 받은 NodeCount정보가 정상인 경우에는 받은 Data를 사용

	m_NodeCheckData.NodeCountOfPreCheck= RcvData.NodeCheckData.NodeCountOfPreCheck;
	m_NodeCheckData.NodeCount = RcvData.NodeCheckData.NodeCount;


    m_NodeCheckData.PositionOfDrivingTagPreCheck= (double)RcvData.NodeCheckData.PositionOfDrivingTagPreCheck;
    m_NodeCheckData.PositionOfDrivingTag= (double)RcvData.NodeCheckData.PositionOfDrivingTag;
    m_NodeCheckData.PositionOfTransTagFirst= (double)RcvData.NodeCheckData.PositionOfTransTagFirst;
    m_NodeCheckData.PositionOfTransTagFinal = (double)RcvData.NodeCheckData.PositionOfTransTagFinal;

#if 0
    VoltageUnion.Data = RcvData.VolatageMonitor;
#endif

	m_VolatageMonitor[0] =RcvData.VolatageMonitor[0];
	m_VolatageMonitor[1] =RcvData.VolatageMonitor[1];
#if 1

	// AMC 주기 통신 로그
	if(memcmp(&PreRcvData, &RcvData, sizeof(AMC_DATA))!=0)  // 이전 값과 다른 경우만 로그를 남긴다.
	{
		// Log 축소 (100msec)
		if((OHTUTILITY::PreventOverflowGetTimeMs(dwAMCTrigerTime) > 100) || (bCheckAMCLog ==true))
		{
			int AmpEnable = (RcvData.AmpEnable[0] << 0) + (RcvData.AmpEnable[1] << 1) + (RcvData.AmpEnable[2] << 2) + (RcvData.AmpEnable[3] << 3);
			int AmpRunning = (RcvData.Running[0] << 0) + (RcvData.Running[1] << 1) + (RcvData.Running[2] << 2) + (RcvData.Running[3] << 3);

			// LogVersion 2.0 SHCHO 2019.03 ************
			// AMC RECV 로그 작성시 주의사항
			// 진법 계산때문에 반드시 10진수만 int만 사용해야함. 16진수, 소수점 안됨!! 음수 가능.
			char orgLog[4096], encLog[4096];
			memset(orgLog, 0, sizeof(orgLog));
			memset(encLog, 0, sizeof(encLog));

			sprintf(orgLog, "/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d"
				, AmpEnable
				, RcvData.FollowingError[0],    RcvData.FollowingError[1],  RcvData.FollowingError[2],  RcvData.FollowingError[3]
				, RcvData.Position[0],          RcvData.Position[1],        RcvData.Position[2],        RcvData.Position[3]
				, AmpRunning
				, RcvData.Velocity[0],          RcvData.Velocity[1],        RcvData.Velocity[2],        RcvData.Velocity[3]
				, RcvData.Output[0],            RcvData.Output[1],          RcvData.Output[2]
				, RcvData.Input[0],				RcvData.Input[1],           RcvData.Input[2]
				, RcvData.NodeCheckData.NodeCount
				, RcvData.NodeCheckData.NodeCountOfPreCheck
				, RcvData.NodeCheckData.PositionOfDrivingTag
				, RcvData.NodeCheckData.PositionOfTransTagFirst
				, RcvData.NodeCheckData.PositionOfTransTagFinal
				, RcvData.NodeCheckData.PositionOfDrivingTagPreCheck
				, RcvData.VolatageMonitor[0]&0xFFFF, (RcvData.VolatageMonitor[0]>>16)&0xFFFF, RcvData.VolatageMonitor[1]&0xFFFF, (RcvData.VolatageMonitor[1]>>16)&0xFFFF
				, RcvData.PauseState
				, RcvData.Source[0], RcvData.Source[1], RcvData.Source[2], RcvData.Source[3], RcvData.State[0],RcvData.State[1]
			);
			// 로그 91진법으로 변환
			NumericSystem::EncodingLog(orgLog, encLog, sizeof(encLog));

			if(gm_LogManager!=NULL && m_pLogAmcRcv!=NULL && gm_LogManager->Get_LogFolder())
			{
				m_pLogAmcRcv->put(strlen(encLog), encLog);
			}

			// AMC RECV OLD 로그. 로그 압축 정합성 확인되면 삭제할것
			dwAMCTrigerTime = timeGetTime();
		}
		PreRcvData = RcvData;
	}

#endif

	}catch(...)
	{
		AMC_ADD_LOG("AMC Exception!!!!");
		return false;
	}

	return true;

}

/**
@brief   AMCLibrary 의 I/O Set을 받는 함수
@author  ehoto
@date    2012-11-01
@param   pSendData : 보내는 Data 가 들어있는 int 배열
@param   nSendLen : 보내는 Data 길이
@param   pRecvData : 받는 Data 가 들어있는 int 배열
@param   nRecvLen : 받는 Data 길이
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int AMCLibrary::GetIOSet(int* pInputBuff, int& nInputCount, int* pOutputBuff, int& nOutCount)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	for(int n=0; n<MAX_IO_INT_COUNT; n++)
	{
		pInputBuff[n] = m_anInput[n];
		pOutputBuff[n] = m_anOutput[n];
	}

	nInputCount = MAX_IO_INT_COUNT;
	nOutCount = MAX_IO_INT_COUNT;

	return NO_ERR;
}

/**
@brief   AMC로 Node Count를 Reset 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::ClearNodeCountInfo(void)
{
    int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.ClearNodeCount = true;
	AMC_ADD_LOG("ClearNodeCountInfo");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}



/**
@brief   AMC로 내부 토크 설정  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetTorqueLimit(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetToreqLimit = true;
	AMC_ADD_LOG("SetTorqueLimit");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   AMC로 내부토크 해지 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetTorqueLimitRelease(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetToreqLimitRelease = true;
	AMC_ADD_LOG("SetTorqueLimitRelease");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}

/**
@brief   AMC로 내부 토크 설정  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetSlopeTime(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetSlopeTime = true;
	AMC_ADD_LOG("SetSlopeTime");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   AMC로 내부토크 해지 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetSlopeTimeRelease(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetSlopeTimeRelease = true;
	AMC_ADD_LOG("SetSlopeTimeRelease");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}


/**
@brief   AMC로 내부 토크 설정  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetTargetPos(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetTargetPos = true;

	AMC_ADD_LOG("SetTargetPos!!!!!!!!!!!!!!!!!!!");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   AMC로 내부토크 해지 요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetTargetPosRelease(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetTargetPosRelease = true;

	AMC_ADD_LOG("SetTargetPosRelease!!!!");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}

/**
@brief   AMC로 내부 토크 설정  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetFirstNodeCountAdd(void)
{

	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetFirstNodeCountAdd = true;
	AMC_ADD_LOG("SetFirstNodeCountAdd");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   AMC로 내부 토크 설정  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetVHLDetectIgnor(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetVHLDetectIgnor = true;
	AMC_ADD_LOG("SetVHLDetectIgnor");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   AMC로 내부 토크 설정  요청하는 함수
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetVHLDetectIgnorRelease(void)
{

	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetVHLDetectIgnorRelease = true;
	AMC_ADD_LOG("SetVHLDetectIgnorRelease");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
//AOHC-196 사전 로그 추가
int	  AMCLibrary::GetFollowing_Axis1()
{
	return m_FollowigAxis1;
}
/**
@brief   대차 감지시 감속도 0.5m/s^2 적용
@author  puting
@date    2019-04-21
*/
void AMCLibrary::SetDetectStopDecelDown(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectStopDecelDown = true;
	AMC_ADD_LOG("SetDetectStopDecelDown");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   대차 감지시 감속도 0.5m/s^2 적용 해제
@author  puting
@date    2019-04-21
*/
void AMCLibrary::SetDetectStopDecelDownRelease(void)
{

	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectStopDecelDownRelease = true;
	AMC_ADD_LOG("SetDetectStopDecelDownRelease");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   대차감지시 패턴 강제 설정
@author  puting
@date    2019-07-24
*/
void AMCLibrary::SetVHLDetectIOSet(bool bIoSet)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetVHLDetectIOSet = bIoSet;
	AMC_ADD_LOG("SetVHLDetectIOSet");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}

/**
@brief   Torque data 정보를 받는 함수
@author  kks
@date    2014-10-24
@param
@param
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int AMCLibrary::GetVHLDetectStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCVHLDetectStatus;
}

/**
@brief   대차 감지에 따른 감속도 변경 상태값
@author  kks
@date    2019-04-21
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int AMCLibrary::GetDetectStopDecelDownStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCDetectStopDecelDownStatus;
}
/**
@brief   대차 감지 센서 IO 강제 설정 상태값
@author  puting
@date    2019-07-25
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int AMCLibrary::GetVHLDetectForceIO()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCDetectForceIOStatus;

}

/**
@brief   NodeCount 의 정보를 받는 함수
@author  ehoto
@date    2012-11-28
@param   nNodeCount : Node Count를 돌려 받는 변수
@param   dNodeDetectPosition : Node 감지된 위치를 돌려 받는 변수
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int AMCLibrary::GetNodeCountInfo(NODE_CHECK_DATA &NodeCheckData)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	NodeCheckData = m_NodeCheckData;

	return NO_ERR;
}

/**
@brief   Pause data 정보를 받는 함수
@author  kks
@date    2014-10-24
@param   
@param
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int AMCLibrary::GetPauseStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCPauseStatus;
}

/**
@brief   전압값 출력 함수
@author  KDS
@date    2017-11-29
@param
@param
@return  전압값 리턴
*/
int AMCLibrary::GetVolatageMonitor(int Index)
{
	  return 	m_VolatageMonitor[Index];
}

/**
@brief   Torque data 정보를 받는 함수
@author  kks
@date    2014-10-24
@param
@param
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int AMCLibrary::GetTorqueStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCTorqueStatus;
}

/**
@brief   Stop State정보를 받는 함수
@author  put
@date    2018-09-10
@param
@param
@return  State
*/
int AMCLibrary::GetStopStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCVHLStopStatus;
}

/**
@brief   Decel State정보를 받는 함수
@author  put
@date    2019-10-22
@param
@param
@return  State
*/
int AMCLibrary::GetDecelStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCDecelStatus;
}


#if(USE_CHANGENODE == 1)
/**
@brief   DetectSensor의 정보를 설정하는 함수
@author  ehoto
@date    2013-01-22
@param   DetectInfo : DetectSensor 의 값을 설정하기 휘한 정보를 가지고 있는 구조체
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int AMCLibrary::SetDetectInfo(DETECT_INFO DetectInfo, OPTIONAL_NODE_INFO OptNodeInfo)
{
	int nTmpError = NO_ERR;
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectInfo=true;
	QueuingAmcCmd.DataUnion.DetectInfo = DetectInfo;

	QueuingAmcCmd.DataUnion.OptNodeInfo = OptNodeInfo; // for change node (jhoun.lim)

//	QueuingAmcCmd.DataUnion.DetectInfo.UseFollowControlFlag = DetectInfo.UseFollowControlFlag;
//	QueuingAmcCmd.DataUnion.DetectInfo.Type = DetectInfo.Type;
//	QueuingAmcCmd.DataUnion.DetectInfo.NodeCount = DetectInfo.NodeCount;
//	QueuingAmcCmd.DataUnion.DetectInfo.DistSumOfStraightPath = DetectInfo.DistSumOfStraightPath;

	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

	AMC_ADD_LOG("AMC Detect Info:%o [%d:%d:%d:%d:%d]", QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectInfo,
		(int)QueuingAmcCmd.DataUnion.DetectInfo.OHTDetectType,
		(int)QueuingAmcCmd.DataUnion.DetectInfo.OBSDetectType,
		QueuingAmcCmd.DataUnion.DetectInfo.NodeCount,
		QueuingAmcCmd.DataUnion.DetectInfo.FollowCommandType,
		QueuingAmcCmd.DataUnion.DetectInfo.PBSIgnoreFlag );

	// for change node (jhoun.lim)
	if(QueuingAmcCmd.DataUnion.OptNodeInfo.DistON > 0)
	{
		AMC_ADD_LOG("AMC Change Node Info: %d [%d:%d] [%d:%d]",
			QueuingAmcCmd.DataUnion.OptNodeInfo.NodeType,
			(int)QueuingAmcCmd.DataUnion.OptNodeInfo.OHTDetectTypeOpt,
			(int)QueuingAmcCmd.DataUnion.OptNodeInfo.OBSDetectTypeOpt,
			QueuingAmcCmd.DataUnion.OptNodeInfo.DistON,
			QueuingAmcCmd.DataUnion.OptNodeInfo.DistAfterON);
	}

	return ConvertAMCError(nTmpError);
}
#else
/**
@brief   DetectSensor의 정보를 설정하는 함수
@author  ehoto
@date    2013-01-22
@param   DetectInfo : DetectSensor 의 값을 설정하기 휘한 정보를 가지고 있는 구조체
@return  Error Code 정상의 경우 NO_ERR(0)
*/
int AMCLibrary::SetDetectInfo(DETECT_INFO DetectInfo)
{
	int nTmpError = NO_ERR;
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectInfo=true;
	QueuingAmcCmd.DataUnion.DetectInfo = DetectInfo;
//	QueuingAmcCmd.DataUnion.DetectInfo.UseFollowControlFlag = DetectInfo.UseFollowControlFlag;
//	QueuingAmcCmd.DataUnion.DetectInfo.Type = DetectInfo.Type;
//	QueuingAmcCmd.DataUnion.DetectInfo.NodeCount = DetectInfo.NodeCount;
//	QueuingAmcCmd.DataUnion.DetectInfo.DistSumOfStraightPath = DetectInfo.DistSumOfStraightPath;

	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

	AMC_ADD_LOG("AMC Detect Info:%o [%d:%d:%d:%d]", QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectInfo,
		(int)QueuingAmcCmd.DataUnion.DetectInfo.Type,
		QueuingAmcCmd.DataUnion.DetectInfo.NodeCount,
		QueuingAmcCmd.DataUnion.DetectInfo.FollowCommandType,
		QueuingAmcCmd.DataUnion.DetectInfo.PBSIgnoreFlag );

	return ConvertAMCError(nTmpError);
}

#endif

/**
@brief   CompensateSlavePos

	뒷바퀴를 앞바퀴 엔코더 값으로 강제로 맞추는 함수
	전후륜 싸움 방지 목적 (특히 소량 이동 시)
@author  임태웅
@date    2013-03-12
*/
int AMCLibrary::CompensateSlavePos()
{
	int nTmpError = NO_ERR;
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.CompensateRearEncoder=true;

	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

	AMC_ADD_LOG("CompensateSlavePos:%X", QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.CompensateRearEncoder);

	return ConvertAMCError(nTmpError);
}

int AMCLibrary::SetPauseControl(int bIsPause, int nCommander)
{
	int nTmpError = NO_ERR;
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.PauseControl = true;
	QueuingAmcCmd.DataUnion.PauseInfo.Commander = nCommander;
	QueuingAmcCmd.DataUnion.PauseInfo.Pause = bIsPause;
	
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

	AMC_ADD_LOG("AMC Pause Info:[%d:%d]", QueuingAmcCmd.DataUnion.PauseInfo.Commander,
		QueuingAmcCmd.DataUnion.PauseInfo.Pause	);

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary 에 프로토콜로 명령 전달
@author  ehoto
@date    2012-11-01
@param   nAxisNum : 축 번호 (0~3)
@param   pSendData : 보내는 Data 가 들어있는 Char 배열
@param   nSendLen : 보내는 Data 길이
@param   pRecvData : 받는 Data 가 들어있는 Char 배열
@param   nRecvLen : 받는 Data 길이
@return  Error Code
*/
int AMCLibrary::CheckError(int Source, int State)
{
	int TmpError=0;

	// source 확인
	if(Source & ST_HOME_SWITCH)				TmpError = int(ERR_AXIS1_F_HOME_SWITCH);
	else if(Source & ST_POS_LIMIT)			TmpError = int(ERR_AXIS1_F_POS_LIMIT);
	else if(Source & ST_NEG_LIMIT)			TmpError = int(ERR_AXIS1_F_NEG_LIMIT);
	else if(Source & ST_AMP_FAULT)			TmpError = int(ERR_AXIS1_F_AMP_FAULT);
//	else if(Source & ST_A_LIMIT) TmpError = int(ERR_AXIS1_F_A_LIMIT);
	else if(Source & ST_V_LIMIT)			TmpError = int(ERR_AXIS1_F_V_LIMIT);
//	else if((Source & ST_REVESE_MOVE) == ST_REVESE_MOVE) TmpError = int(ERR_AXIS1_F_REVERSE_MOVE);
	else if(Source & ST_X_NEG_LIMIT)		TmpError = int(ERR_AXIS1_F_X_NEG_LIMIT);
	else if(Source & ST_X_POS_LIMIT)		TmpError = int(ERR_AXIS1_F_X_POS_LIMIT);
	else if(Source & ST_ERROR_LIMIT)		TmpError = int(ERR_AXIS1_F_ERROR_LIMIT);
	else if(Source & ST_PC_COMMAND)			TmpError = int(ERR_AXIS1_F_PC_COMMAND);
	else if(Source & ST_OUT_OF_FRAMES)		TmpError = int(ERR_AXIS1_F_OUT_OF_FRAMES);
	else if(Source & ST_ABS_COMM_ERROR)		TmpError = int(ERR_AXIS1_F_ABS_COMM_ERROR);
	else if(Source & ST_INPOSITION_STATUS)	TmpError = int(ERR_AXIS1_F_INPOSITION_STATUS);
//	else if(Source & ST_RUN_STOP_COMMAND) TmpError = int(ERR_AXIS1_F_RUN_STOP_COMMAND);
	else if(Source & ST_COLLISION_STATE)	TmpError = int(ERR_AXIS1_F_COLLISION_STATE);

	// state 확인
	else if(State==1)	TmpError = int(ERR_AXIS1_F_STOP_EVENT);
	else if(State==2)	TmpError = int(ERR_AXIS1_F_ESTOP_EVENT);
	else if(State==3)	TmpError = int(ERR_AXIS1_F_ABORT_EVENT);

	return TmpError;
}



//watchdog관련
//---------------------------------------------------------------------------
int AMCLibrary::EnableWatchDog(void)
{
   int nTmpError=0;
   if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
   EnterCriticalSection(&m_CS);

	//DeviceAMCEnableWatchdogReason(WDT_MAINLOOP); //나중에 살려라   // main loop
	//DeviceAMCEnableWatchdogReason(WDT_EXTRA);    //나중에 살려라   // 5ms loop
	//DeviceAMCEnableWatchdogReason(WDT_SUBCONTROL);                 // 1ms loop
	if(nTmpError==NO_ERR) nTmpError = DeviceAMCEnableWatchdogReason(WDT_CONTROL);   // 200us loop

	LeaveCriticalSection(&m_CS);
   return ConvertAMCError(nTmpError);
}

//---------------------------------------------------------------------------
int AMCLibrary::DisableWatchDog(void)
{
    int nTmpError=0;
    if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
    EnterCriticalSection(&m_CS);
      //DeviceAMCDisableWatchdogReason(WDT_MAINLOOP); //나중에 살려라     // main loop
      //DeviceAMCDisableWatchdogReason(WDT_EXTRA);    //나중에 살려라     // 5ms loop
      //DeviceAMCDisableWatchdogReason(WDT_SUBCONTROL);                   // 1ms loop
	if(nTmpError==NO_ERR) nTmpError = DeviceAMCDisableWatchdogReason(WDT_CONTROL);     // 200us loop

	LeaveCriticalSection(&m_CS);
   return ConvertAMCError(nTmpError);
}
//---------------------------------------------------------------------------
// 에러값 (nVal)
//0x80 : 모터 회전 방향이 INI 파일과 사용변수가 다른 경우
//0x40 : 모터 타입(절대치, 인크리멘탈)이 바뀐 경우
//0x20 : 현재 위치가 작업 영역을 벗어난 경우
//0x08 : control wdt. Event, 200us INT => 사용 가능
//0x04 : sub-control wdt. Event, 1ms INT => 사용 가능
//0x02 : extra wdt. Event, 10ms INT => DPRAM INT, 초 단위로 변경 후 사용
//0x01 :  main-loop wdt. Event, Main Loop => 초 단위로 변경 후 사용

//---------------------------------------------------------------------------
int AMCLibrary::GetWatchDogStatus(void)
{
   int nTmpError=0, nAMCError=0;

   unsigned int nVal=0;

   if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
   EnterCriticalSection(&m_CS);

   nAMCError = DeviceAMCGetWatchdogStatus(&nVal);          // nVal=> bit0:main loop, 1:5ms, 2:1ms, 3:200us
   nTmpError = ConvertAMCError(nAMCError);

   if((nTmpError==NO_ERR) && (nVal==8)) //모든 AMC동작 중지..
   {
      nTmpError=ERR_AMC_WATCHDOG_STOP;
	  ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_AMC_WATCHDOG_STOP,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
   }
   else if(nVal!=0)
   {
      nAMCError = DeviceAMCSetWatchdogStatus(0);
   }

   LeaveCriticalSection(&m_CS);
   return nTmpError;
}

//---------------------------------------------------------------------------
int AMCLibrary::ClearWatchDogStatus(void)
{
   int nTmpError=0, nAMCError=0;
   unsigned int nVal=0;

   if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
   EnterCriticalSection(&m_CS);

   nAMCError = DeviceAMCGetWatchdogStatus(&nVal);          // nVal=> bit0:main loop, 1:5ms, 2:1ms, 3:200us
   if((nAMCError==NO_ERR) && (nVal!=0))
   {
      nAMCError = DeviceAMCSetWatchdogStatus(0);         // bit0:main loop, 1:5ms, 2:1ms, 3:200us
      nTmpError = ConvertAMCError(nAMCError);
   }

   LeaveCriticalSection(&m_CS);
   return nTmpError;
}
//---------------------------------------------------------------------------
void AMCLibrary::makeTraceLog()
{
	AMC_ADD_LOG("Trace Start");

	DeviceAMCGatheringTraceData();
	DeviceAMCGatheringTraceData_1();

	AMC_ADD_LOG("Trace End");
}
//---------------------------------------------------------------------------

/**
@brief   Input Data의 접점을 판단하기 위한 Mask 변수를 설정하는 함수
@author  zzang9un
@date    2013.07.09
@note    센서의 A(Normal Open), B(Normal Close) 접점에 따라 설정한다(A접점 0, B접점은 1)
*/
void AMCLibrary::InputMaskInit(void)
{
#if ((OHT_NAME == OHT_NAME_STD_V80))
	// Input 1 설정
	m_Input1Mask_R01.HAND_Inposition         = 0;
	m_Input1Mask_R01.HAND_Alarm              = 0;
	m_Input1Mask_R01.HAND_Right_Grip         = 0;
	m_Input1Mask_R01.HAND_Left_Grip          = 0;
	m_Input1Mask_R01.HAND_Close              = 0;
	m_Input1Mask_R01.HAND_Open               = 0;
	m_Input1Mask_R01.HAND_Push               = 0;

	m_Input1Mask_R01.HOIST_Home              = 1;

	m_Input1Mask_R01.HAND_PIO_Go             = 0;
	m_Input1Mask_R01.EQPIO_Load_REQ          = 0;
	m_Input1Mask_R01.EQPIO_Unload_REQ        = 0;
	m_Input1Mask_R01.EQPIO_Ready             = 0;
	m_Input1Mask_R01.EQPIO_HO_AVBL           = 0;
	m_Input1Mask_R01.EQPIO_ES                = 0;
	m_Input1Mask_R01.EQPIO_Left_Go           = 0;
	m_Input1Mask_R01.EQPIO_Right_Go          = 0;

	m_Input1Mask_R01.STEER_F_Left            = 0;
	m_Input1Mask_R01.STEER_R_Left            = 0;
	m_Input1Mask_R01.STEER_F_Right           = 0;
	m_Input1Mask_R01.STEER_R_Right           = 0;
	m_Input1Mask_R01.SHUTTER_F_Close         = 0;
	m_Input1Mask_R01.SHUTTER_R_Close         = 0;
	m_Input1Mask_R01.SHUTTER_F_Open          = 0;
	m_Input1Mask_R01.SHUTTER_R_Open          = 0;

	m_Input1Mask_R01.SHUTTER_F_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_R_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_F_Inposition    = 0;
	m_Input1Mask_R01.SHUTTER_R_Inposition    = 0;
	m_Input1Mask_R01.OBSTACLE_Long           = 1;
	m_Input1Mask_R01.OBSTACLE_Middle         = 1;
	m_Input1Mask_R01.OBSTACLE_Short          = 1;
	m_Input1Mask_R01.OBSTACLE_Fail           = 1;

    // Input 2 설정
	m_Input2Mask_R01.OHTDETECT_Long          = 1;
	m_Input2Mask_R01.OHTDETECT_Middle        = 1;
	m_Input2Mask_R01.OHTDETECT_Short         = 1;
	m_Input2Mask_R01.OHTDETECT_Fail          = 1;
	m_Input2Mask_R01.LOOKDOWN_Detect         = 1;
	m_Input2Mask_R01.LOOKDOWN_Error          = 1;

	m_Input2Mask_R01.SHIFT_Home              = 0;


	m_Input2Mask_R01.STB_Detect_Left         = 1;

	m_Input2Mask_R01.STB_Detect_Right        = 1;
	m_Input2Mask_R01.UTB_Detect              = 1;
	m_Input2Mask_R01.FOUP_Detect             = 1;
	m_Input2Mask_R01.OSCILLATION             = 0;
	m_Input2Mask_R01.EMO_Press               = 1;
	m_Input2Mask_R01.BUMPER_Press            = 0;
	m_Input2Mask_R01.FAN_Fail                = 1;
	m_Input2Mask_R01.Reserved_48     		 = 1;

	m_Input2Mask_R01.MC_Status    			 = 0;
	m_Input2Mask_R01.HANDTP_Connect          = 0;
	m_Input2Mask_R01.FOUP_Cover_Open_Detect  = 1;
	m_Input2Mask_R01.STOP_Driving_Front      = 1;
	m_Input2Mask_R01.STOP_Driving_PreCheck   = 1;
	m_Input2Mask_R01.STOP_Trans_Front        = 1;
	m_Input2Mask_R01.STOP_Trans_Rear         = 1;
	m_Input2Mask_R01.BCR_Driving_OK          = 0;

	m_Input2Mask_R01.BCR_Driving_NG          = 0;
	m_Input2Mask_R01.BCR_Trans_OK            = 0;
	m_Input2Mask_R01.BCR_Trans_NG            = 0;
	m_Input2Mask_R01.HOIST_Belt_Detect       = 1;
	m_Input2Mask_R01.RailSupport_Home        = 0;    //
	m_Input2Mask_R01.Railsupport_Work        = 0;
	m_Input2Mask_R01.MAC_Detect              = 1;
	m_Input2Mask_R01.SMPS_Short 			 = 1;
	//tg 추가
	m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
	m_Input3Mask_R01.CID_Interlock_IN 	     = 0;
	m_Input3Mask_R01.CID_Completion_ACK	     = 0;
	m_Input3Mask_R01.CID_Status_IN           = 0;
	m_Input3Mask_R01.CID_Reserved_IN1        = 0;
	m_Input3Mask_R01.CID_Reserved_IN2        = 0;
	m_Input3Mask_R01.Outtrigger_F_Alarm_IN   = 0;
	m_Input3Mask_R01.Outtrigger_F_Home_IN    = 0;
	m_Input3Mask_R01.Outtrigger_F_Work_IN    = 0;
	m_Input3Mask_R01.Outtrigger_R_Alarm_IN   = 0;

	m_Input3Mask_R01.Outtrigger_R_Home_IN    = 0;

	m_Input3Mask_R01.Outtrigger_R_Work_IN    = 0;
	m_Input3Mask_R01.Rotate_L_Limit          = 0;
	m_Input3Mask_R01.Rotate_R_Limit          = 0;
	m_Input3Mask_R01.Rotate_Inposition       = 0;
	m_Input3Mask_R01.Rotate_Alarm            = 0;

#elif ((OHT_NAME == OHT_NAME_STD_V81))     // JIG INPUT IO Timy
	// Input 1 설정
	m_Input1Mask_R01.HAND_Inposition         = 0;
	m_Input1Mask_R01.HAND_Alarm              = 0;
	m_Input1Mask_R01.HAND_Right_Grip         = 0;
	m_Input1Mask_R01.HAND_Left_Grip          = 0;
	m_Input1Mask_R01.HAND_Close              = 0;
	m_Input1Mask_R01.HAND_Open               = 0;
	m_Input1Mask_R01.HAND_Push               = 0;
	m_Input1Mask_R01.Reserved_7   			 = 0;	//m_Input1Mask_R01.HOIST_Home              = 1;

	m_Input1Mask_R01.HAND_PIO_Go             = 0;
	m_Input1Mask_R01.ROTATE_Inposition		 = 0;  	//m_Input1Mask_R01.EQPIO_Load_REQ          = 0;
	m_Input1Mask_R01.ROTATE_Alarm			 = 0;   //m_Input1Mask_R01.EQPIO_Unload_REQ        = 0;
	m_Input1Mask_R01.ROTATE_90				 = 0;   //m_Input1Mask_R01.EQPIO_Ready             = 0;
	m_Input1Mask_R01.EQPIO_HO_AVBL           = 0;
	m_Input1Mask_R01.EQPIO_ES                = 0;
	m_Input1Mask_R01.EQPIO_Left_Go           = 0;
	m_Input1Mask_R01.EQPIO_Right_Go          = 0;

	m_Input1Mask_R01.STEER_F_Left            = 0;
	m_Input1Mask_R01.STEER_R_Left            = 0;
	m_Input1Mask_R01.STEER_F_Right           = 0;
	m_Input1Mask_R01.STEER_R_Right           = 0;
	m_Input1Mask_R01.SHUTTER_F_Close         = 0;
	m_Input1Mask_R01.SHUTTER_R_Close         = 0;
	m_Input1Mask_R01.SHUTTER_F_Open          = 0;
	m_Input1Mask_R01.SHUTTER_R_Open          = 0;

	m_Input1Mask_R01.SHUTTER_F_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_R_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_F_Inposition    = 0;
	m_Input1Mask_R01.SHUTTER_R_Inposition    = 0;
	m_Input1Mask_R01.OBSTACLE_Long           = 1;
	m_Input1Mask_R01.OBSTACLE_Middle         = 1;
	m_Input1Mask_R01.OBSTACLE_Short          = 1;
	m_Input1Mask_R01.OBSTACLE_Fail           = 1;

    // Input 2 설정
	m_Input2Mask_R01.OHTDETECT_Long          = 1;
	m_Input2Mask_R01.OHTDETECT_Middle        = 1;
	m_Input2Mask_R01.OHTDETECT_Short         = 1;
	m_Input2Mask_R01.OHTDETECT_Fail          = 1;
	m_Input2Mask_R01.LOOKDOWN_Detect         = 1;
	m_Input2Mask_R01.LOOKDOWN_Error          = 1;

	m_Input2Mask_R01.SHIFT_Home              = 0;


	m_Input2Mask_R01.STB_Detect_Left         = 1;

	m_Input2Mask_R01.STB_Detect_Right        = 1;
	m_Input2Mask_R01.UTB_Detect              = 1;
	m_Input2Mask_R01.FOUP_Detect             = 1;
	m_Input2Mask_R01.OSCILLATION             = 0;
	m_Input2Mask_R01.EMO_Press               = 1;
	m_Input2Mask_R01.BUMPER_Press            = 0;
	m_Input2Mask_R01.FAN_Fail                = 1;
	m_Input2Mask_R01.Reserved_48     		 = 1;

	m_Input2Mask_R01.MC_Status    			 = 0;
	m_Input2Mask_R01.HANDTP_Connect          = 0;
	m_Input2Mask_R01.FOUP_Cover_Open_Detect  = 1;
	m_Input2Mask_R01.STOP_Driving_Front      = 1;
	m_Input2Mask_R01.STOP_Driving_PreCheck   = 1;
	m_Input2Mask_R01.STOP_Trans_Front        = 1;
	m_Input2Mask_R01.STOP_Trans_Rear         = 1;
	m_Input2Mask_R01.BCR_Driving_OK          = 0;

	m_Input2Mask_R01.BCR_Driving_NG          = 0;
	m_Input2Mask_R01.BCR_Trans_OK            = 0;
	m_Input2Mask_R01.BCR_Trans_NG            = 0;
	m_Input2Mask_R01.HOIST_Belt_Detect       = 1;
	m_Input2Mask_R01.ROTATE_0				 = 0; //m_Input2Mask_R01.RailSupport_Home        = 0;
	m_Input2Mask_R01.ROTATE_180				 = 0; //m_Input2Mask_R01.Railsupport_Work        = 0;
	m_Input2Mask_R01.ROTATE_270				 = 0; //m_Input2Mask_R01.MAC_Detect              = 1;
	m_Input2Mask_R01.SMPS_Short 			 = 1;

	//tg 추가
	m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
	m_Input3Mask_R01.CID_Interlock_IN 	     = 0;
	m_Input3Mask_R01.CID_Completion_ACK	     = 0;
	m_Input3Mask_R01.CID_Status_IN           = 0;
	m_Input3Mask_R01.CID_Reserved_IN1        = 0;
	m_Input3Mask_R01.CID_Reserved_IN2        = 0;
	m_Input3Mask_R01.Reserved_70   		= 0;
	m_Input3Mask_R01.Reserved_71    	= 0;
	m_Input3Mask_R01.Reserved_72   		= 0;
	m_Input3Mask_R01.Reserved_73    	= 0;
	m_Input3Mask_R01.HOIST_Home              = 1; //m_Input3Mask_R01.Reserved_74    	= 0;
	m_Input3Mask_R01.Reserved_75    	= 0;
	m_Input3Mask_R01.Reserved_76          = 0;
	m_Input3Mask_R01.EQPIO_Load_REQ          = 0;        //임시 IO 차용 컴파일문제로
	m_Input3Mask_R01.EQPIO_Unload_REQ       = 0;
	m_Input3Mask_R01.EQPIO_Ready            = 0;
#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
	// Input 1 설정
	m_Input1Mask_R01.HAND_Inposition         = 0;
	m_Input1Mask_R01.HAND_Alarm              = 0;
	m_Input1Mask_R01.HAND_Right_Grip         = 0;
	m_Input1Mask_R01.HAND_Left_Grip          = 0;
	m_Input1Mask_R01.HAND_Close              = 0;
	m_Input1Mask_R01.HAND_Open               = 0;
	m_Input1Mask_R01.HAND_Push               = 0;
	m_Input1Mask_R01.Reserved_7              = 1;

	m_Input1Mask_R01.HAND_PIO_Go             = 0;
	m_Input1Mask_R01.EQPIO_Load_REQ          = 0;
	m_Input1Mask_R01.EQPIO_Unload_REQ        = 0;
	m_Input1Mask_R01.EQPIO_Ready             = 0;
	m_Input1Mask_R01.EQPIO_HO_AVBL           = 0;
	m_Input1Mask_R01.EQPIO_ES                = 0;
	m_Input1Mask_R01.EQPIO_Left_Go           = 0;
	m_Input1Mask_R01.EQPIO_Right_Go          = 0;

	m_Input1Mask_R01.STEER_F_Left            = 0;
	m_Input1Mask_R01.STEER_R_Left            = 0;
	m_Input1Mask_R01.STEER_F_Right           = 0;
	m_Input1Mask_R01.STEER_R_Right           = 0;
	m_Input1Mask_R01.SHUTTER_F_Close         = 0;
	m_Input1Mask_R01.SHUTTER_R_Close         = 0;
	m_Input1Mask_R01.SHUTTER_F_Open          = 0;
	m_Input1Mask_R01.SHUTTER_R_Open          = 0;

	m_Input1Mask_R01.SHUTTER_F_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_R_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_F_Inposition    = 0;
	m_Input1Mask_R01.SHUTTER_R_Inposition    = 0;
	m_Input1Mask_R01.OBSTACLE_Long           = 1;
	m_Input1Mask_R01.OBSTACLE_Middle         = 1;
	m_Input1Mask_R01.OBSTACLE_Short          = 1;
	m_Input1Mask_R01.OBSTACLE_Fail           = 1;

    // Input 2 설정
	m_Input2Mask_R01.OHTDETECT_Long          = 1;
	m_Input2Mask_R01.OHTDETECT_Middle        = 1;
	m_Input2Mask_R01.OHTDETECT_Short         = 1;
	m_Input2Mask_R01.OHTDETECT_Fail          = 1;
	m_Input2Mask_R01.LOOKDOWN_Detect         = 1;
	m_Input2Mask_R01.LOOKDOWN_Error          = 1;

	m_Input2Mask_R01.SHIFT_Home              = 0;

	m_Input2Mask_R01.STB_Detect_Left         = 1;

	m_Input2Mask_R01.STB_Detect_Right        = 1;
	m_Input2Mask_R01.UTB_Detect              = 1;
	m_Input2Mask_R01.FOUP_Detect             = 1;
	m_Input2Mask_R01.OSCILLATION             = 0;
	m_Input2Mask_R01.EMO_Press               = 1;
	m_Input2Mask_R01.BUMPER_Press            = 0;
	m_Input2Mask_R01.FAN_Fail                = 1;
	m_Input2Mask_R01.Reserved_48     		 = 1;

	m_Input2Mask_R01.MC_Status    			 = 0;
	m_Input2Mask_R01.HANDTP_Connect          = 0;
	m_Input2Mask_R01.FOUP_Cover_Open_Detect  = 1;
	m_Input2Mask_R01.STOP_Driving_Front      = 1;
	m_Input2Mask_R01.STOP_Driving_PreCheck   = 1;
	m_Input2Mask_R01.STOP_Trans_Front        = 1;
	m_Input2Mask_R01.STOP_Trans_Rear         = 1;
	m_Input2Mask_R01.BCR_Driving_OK          = 0;

	m_Input2Mask_R01.BCR_Driving_NG          = 0;
	m_Input2Mask_R01.BCR_Trans_OK            = 0;
	m_Input2Mask_R01.BCR_Trans_NG            = 0;
	m_Input2Mask_R01.HOIST_Belt_Detect       = 1;
	m_Input2Mask_R01.RailSupport_Home        = 0;    //
	m_Input2Mask_R01.Railsupport_Work        = 0;
	m_Input2Mask_R01.MAC_Detect              = 1;
	m_Input2Mask_R01.SMPS_Short 			 = 1;
	//tg 추가
	m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
	m_Input3Mask_R01.CID_Interlock_IN 	     = 0;
	m_Input3Mask_R01.CID_Completion_ACK	     = 0;
	m_Input3Mask_R01.CID_Status_IN           = 0;
	m_Input3Mask_R01.CID_Reserved_IN1        = 0;
	m_Input3Mask_R01.CID_Reserved_IN2        = 0;
	m_Input3Mask_R01.Rotate_0_Sensor		    = 0;
	m_Input3Mask_R01.Rotate_90_Sensor	    = 0;
	m_Input3Mask_R01.Rotate_180_Sensor	    = 0;
	m_Input3Mask_R01.Rotate_270_Sensor	    = 0;
	m_Input3Mask_R01.HOIST_Home              = 1;
	m_Input3Mask_R01.Reserved_75             = 0;
	m_Input3Mask_R01.Rotate_L_Limit          = 0;
	m_Input3Mask_R01.Rotate_R_Limit          = 0;
	m_Input3Mask_R01.Rotate_Inposition       = 0;
	m_Input3Mask_R01.Rotate_Alarm            = 0;


#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	// ========== Input 1 설정 ==========

	// 0 - 15
	m_Input1Mask_R01.HAND_Inposition 		= 0;
	m_Input1Mask_R01.HAND_Alarm				= 0;
	m_Input1Mask_R01.HAND_Right_Grip 		= 0;
	m_Input1Mask_R01.HAND_Left_Grip			= 0;
	
	m_Input1Mask_R01.HAND_Close				= 0;
	m_Input1Mask_R01.HAND_Open				= 0;
	m_Input1Mask_R01.HAND_Push				= 0;
	m_Input1Mask_R01.HOIST_Align  			= 1;

	m_Input1Mask_R01.HAND_PIO_Go 			= 0;
	m_Input1Mask_R01.EQPIO_Load_REQ			= 0;
	m_Input1Mask_R01.EQPIO_Unload_REQ		= 0;
	m_Input1Mask_R01.EQPIO_Ready 			= 0;
	
	m_Input1Mask_R01.EQPIO_HO_AVBL			= 0;
	m_Input1Mask_R01.EQPIO_ES				= 0;
	m_Input1Mask_R01.EQPIO_Right_Go			= 0;
	m_Input1Mask_R01.EQPIO_Left_Go			= 0;

	// 16 - 31
	m_Input1Mask_R01.STEER_F_Left			= 0;
	m_Input1Mask_R01.STEER_R_Left			= 0;
	m_Input1Mask_R01.STEER_F_Right			= 0;
	m_Input1Mask_R01.STEER_R_Right			= 0;
	
	m_Input1Mask_R01.SHUTTER_F_Close 		= 0;
	m_Input1Mask_R01.SHUTTER_R_Close 		= 0;
	m_Input1Mask_R01.SHUTTER_F_Open			= 0;
	m_Input1Mask_R01.SHUTTER_R_Open			= 0;
	
	m_Input1Mask_R01.SHUTTER_F_Alarm 		= 0;
	m_Input1Mask_R01.SHUTTER_R_Alarm 		= 0;
	m_Input1Mask_R01.SHUTTER_F_Inposition	= 0;
	m_Input1Mask_R01.SHUTTER_R_Inposition	= 0;
	
	m_Input1Mask_R01.OBSTACLE_Long			= 1;
	m_Input1Mask_R01.OBSTACLE_Middle 		= 1;
	m_Input1Mask_R01.OBSTACLE_Short			= 1;
	m_Input1Mask_R01.OBSTACLE_Fail			= 1;

	
	// ========== Input 2 설정 ==========
	// 0 - 15

	m_Input2Mask_R01.OHTDETECT_Long			= 1;
	m_Input2Mask_R01.OHTDETECT_Middle		= 1;
	m_Input2Mask_R01.OHTDETECT_Short 		= 1;
	m_Input2Mask_R01.OHTDETECT_Fail			= 1;
	
	m_Input2Mask_R01.LOOKDOWN_Detect 		= 1;
	m_Input2Mask_R01.LOOKDOWN_Error			= 1;

	m_Input2Mask_R01.SHIFT_Home              = 0;

	m_Input2Mask_R01.STB_Detect_Left 		= 1;
	
	m_Input2Mask_R01.STB_Detect_Right		= 1;
	m_Input2Mask_R01.UTB_Detect				= 1;
	m_Input2Mask_R01.FOUP_Detect 			= 1;
	m_Input2Mask_R01.OSCILLATION 			= 0;
	
	m_Input2Mask_R01.EMO_Press				= 1;
	m_Input2Mask_R01.BUMPER_Press			= 0;
	m_Input2Mask_R01.FAN_Fail				= 1;
	m_Input2Mask_R01.HOIST_Belt 			 	= 1;

	// 16 - 31
	m_Input2Mask_R01.MC_Status				= 0;
	m_Input2Mask_R01.HANDTP_Connect			= 0;
	m_Input2Mask_R01.FOUP_Cover_Open_Detect	= 1;
	m_Input2Mask_R01.STOP_Driving_Front		= 1;
	
	m_Input2Mask_R01.STOP_Driving_PreCheck	= 1;
	m_Input2Mask_R01.STOP_Trans_Front		= 1;
	m_Input2Mask_R01.STOP_Trans_Rear 		= 1;
	m_Input2Mask_R01.BCR_Driving_OK			= 0;
	
	m_Input2Mask_R01.BCR_Driving_NG			= 0;
	m_Input2Mask_R01.BCR_Trans_OK			= 0;
	m_Input2Mask_R01.BCR_Trans_NG			= 0;
	m_Input2Mask_R01.HOIST_Belt_Detect		= 1;

	m_Input2Mask_R01.PIO_Input3				= 0;
	m_Input2Mask_R01.PIO_Input5				= 0;
	m_Input2Mask_R01.PIO_Input6				= 0;
	m_Input2Mask_R01.SMPS_Short				= 1;

	// ========== tg 추가 ==========
	m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
	m_Input3Mask_R01.CID_Interlock_IN		 = 0;
	m_Input3Mask_R01.CID_Completion_ACK 	 = 0;
	m_Input3Mask_R01.CID_Status_IN			 = 0;
	m_Input3Mask_R01.CID_Reserved_IN1		 = 0;
	m_Input3Mask_R01.CID_Reserved_IN2		 = 0;

	// ========= Input 3 설정 ==========
	// 0 - 15
	m_Input3Mask_R01.CID_Link_Compt_IN		= 0;
	m_Input3Mask_R01.CID_Interlock_IN		= 0;
	m_Input3Mask_R01.CID_Completion_ACK		= 0;
	m_Input3Mask_R01.CID_Status_IN			= 0;
	
	m_Input3Mask_R01.CID_Reserved_IN1		= 0;
	m_Input3Mask_R01.CID_Reserved_IN2		= 0;
	m_Input3Mask_R01.Rotate_0_Sensor			= 0;
	m_Input3Mask_R01.Rotate_90_Sensor		= 0;

	m_Input3Mask_R01.Rotate_180_Sensor		= 0;
	m_Input3Mask_R01.Rotate_270_Sensor		= 0;
	m_Input3Mask_R01.HOIST_Home				= 1;
	m_Input3Mask_R01.SHUTTER_F_Middle_Close 	= 0;
	
	m_Input3Mask_R01.SHUTTER_R_Middle_Close 	= 0;
	m_Input3Mask_R01.POD_EUV_1				= 1;
	m_Input3Mask_R01.POD_EUV_2				= 0;
	m_Input3Mask_R01.SPARE_5					= 0;

	// 16 - 31
	m_Input3Mask_R01.SPARE_6					= 0;
	m_Input3Mask_R01.SPARE_7					= 0;
	m_Input3Mask_R01.SPARE_8					= 0;
	m_Input3Mask_R01.SPARE_9					= 0;
	
	m_Input3Mask_R01.SPARE_10				= 0;
	m_Input3Mask_R01.SPARE_11				= 0;
	m_Input3Mask_R01.SPARE_12				= 0;
	m_Input3Mask_R01.SPARE_13				= 0;
	
	m_Input3Mask_R01.SPARE_14 				= 0;
	m_Input3Mask_R01.SPARE_15 				= 0;
	m_Input3Mask_R01.SPARE_16 				= 0;
	m_Input3Mask_R01.SPARE_17 				= 0;
	m_Input3Mask_R01.SPARE_18 				= 0;
	m_Input3Mask_R01.SPARE_19 				= 0;


#elif(OHT_NAME == OHT_NAME_STD_V85)
// ========== Input 1 설정 ==========

// 0 - 15
m_Input1Mask_R01.HAND_Inposition 		= 0;
m_Input1Mask_R01.HAND_Alarm				= 0;
m_Input1Mask_R01.HAND_Right_Grip 		= 0;
m_Input1Mask_R01.HAND_Left_Grip			= 0;

m_Input1Mask_R01.HAND_Close				= 0;
m_Input1Mask_R01.HAND_Open				= 0;
m_Input1Mask_R01.HAND_Push				= 0;
m_Input1Mask_R01.HNAD_Reserved_1  		= 1;
m_Input1Mask_R01.HNAD_ES  				= 1;

m_Input1Mask_R01.HAND_PIO_Go 			= 0;
m_Input1Mask_R01.EQPIO_Load_REQ			= 0;
m_Input1Mask_R01.EQPIO_Unload_REQ		= 0;
m_Input1Mask_R01.EQPIO_Ready 			= 0;

m_Input1Mask_R01.EQPIO_HO_AVBL			= 0;
m_Input1Mask_R01.EQPIO_ES				= 0;
m_Input1Mask_R01.EQPIO_Left_Go			= 0;

// 16 - 31
m_Input1Mask_R01.STEER_F_Left			= 0;
m_Input1Mask_R01.STEER_R_Left			= 0;
m_Input1Mask_R01.STEER_F_Right			= 0;
m_Input1Mask_R01.STEER_R_Right			= 0;

m_Input1Mask_R01.SHUTTER_F_Close 		= 0;
m_Input1Mask_R01.SHUTTER_R_Close 		= 0;
m_Input1Mask_R01.SHUTTER_F_Open			= 0;
m_Input1Mask_R01.SHUTTER_R_Open			= 0;

m_Input1Mask_R01.SHUTTER_F_Alarm 		= 0;
m_Input1Mask_R01.SHUTTER_R_Alarm 		= 0;
m_Input1Mask_R01.SHUTTER_F_Inposition	= 0;
m_Input1Mask_R01.SHUTTER_R_Inposition	= 0;

m_Input1Mask_R01.OBSTACLE_Input_Extend	= 1;
m_Input1Mask_R01.OBSTACLE_Long			= 1;
m_Input1Mask_R01.OBSTACLE_Middle 		= 1;
m_Input1Mask_R01.OBSTACLE_Short			= 1;
m_Input2Mask_R01.OBSTACLE_Fail			= 1;


// ========== Input 2 설정 ==========
// 0 - 15
m_Input2Mask_R01.OHTDETECT_Input_Extend	= 1;
m_Input2Mask_R01.OHTDETECT_Long			= 1;
m_Input2Mask_R01.OHTDETECT_Middle		= 1;
m_Input2Mask_R01.OHTDETECT_Short 		= 1;
m_Input2Mask_R01.OHTDETECT_Fail			= 1;

m_Input2Mask_R01.LOOKDOWN_Detect 		= 1;
m_Input2Mask_R01.LOOKDOWN_Error			= 1;
m_Input2Mask_R01.SHIFT_Home             = 0;
m_Input2Mask_R01.STB_Detect_Left 		= 1;
m_Input2Mask_R01.STB_Detect_Right		= 1;
m_Input2Mask_R01.UTB_Detect				= 1;
m_Input2Mask_R01.FOUP_Detect 			= 1;
m_Input2Mask_R01.OSCILLATION 			= 0;

m_Input2Mask_R01.EMO_Press				= 1;
m_Input2Mask_R01.BUMPER_Press			= 0;
m_Input2Mask_R01.FAN_Fail				= 1;
m_Input2Mask_R01.HOIST_Belt_Detect1 	= 1;

// 16 - 31
m_Input2Mask_R01.HOIST_Belt_Detect2 	= 1;
m_Input2Mask_R01.TP_Enable				= 0;
m_Input2Mask_R01.HANDTP_Connect			= 0;
m_Input2Mask_R01.FOUP_Cover_Open_Detect	= 1;
m_Input2Mask_R01.STOP_Driving_Front		= 1;

m_Input2Mask_R01.STOP_Driving_PreCheck	= 1;
m_Input2Mask_R01.STOP_Trans_Front		= 1;
m_Input2Mask_R01.STOP_Trans_Rear 		= 1;
m_Input2Mask_R01.BCR_Driving_OK			= 0;

m_Input2Mask_R01.BCR_Driving_NG			= 0;
m_Input2Mask_R01.BCR_Trans_OK			= 0;
m_Input2Mask_R01.BCR_Trans_NG			= 0;
m_Input2Mask_R01.Reserved_1				= 0;

m_Input2Mask_R01.Reserved_2				= 0;

m_Input3Mask_R01.Reserved_3				= 0;
m_Input3Mask_R01.SMPS_Short				= 1;

// ========== tg 추가 ==========
m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
m_Input3Mask_R01.CID_Interlock_IN		 = 0;
m_Input3Mask_R01.CID_Completion_ACK 	 = 0;
m_Input3Mask_R01.CID_Status_IN			 = 0;
m_Input3Mask_R01.CID_Reserved_IN1		 = 0;
m_Input3Mask_R01.CID_Reserved_IN2		 = 0;

// ========= Input 3 설정 ==========
// 0 - 15
m_Input3Mask_R01.CID_Link_Compt_IN		= 0;
m_Input3Mask_R01.CID_Interlock_IN		= 0;
m_Input3Mask_R01.CID_Completion_ACK		= 0;
m_Input3Mask_R01.CID_Status_IN			= 0;

m_Input3Mask_R01.CID_Reserved_IN1		= 0;
m_Input3Mask_R01.CID_Reserved_IN2		= 0;
m_Input3Mask_R01.Rotate_0_Sensor		= 0;
m_Input3Mask_R01.Rotate_90_Sensor		= 0;

m_Input3Mask_R01.Rotate_180_Sensor		= 0;
m_Input3Mask_R01.Rotate_270_Sensor		= 0;
m_Input3Mask_R01.HOIST_Home				= 1;
m_Input3Mask_R01.HOIST_Home_2			= 1;
m_Input3Mask_R01.SHUTTER_F_Middle_Close = 0;

m_Input3Mask_R01.SHUTTER_R_Middle_Close = 0;
m_Input3Mask_R01.POD_EUV_1				= 1;
m_Input3Mask_R01.POD_EUV_2				= 0;
m_Input3Mask_R01.SPARE_5				= 0;

// 16 - 31
m_Input3Mask_R01.SPARE_6					= 0;
m_Input3Mask_R01.SPARE_7					= 0;
m_Input3Mask_R01.SPARE_8					= 0;
m_Input3Mask_R01.SPARE_9					= 0;

m_Input3Mask_R01.SPARE_10				= 0;
m_Input3Mask_R01.SPARE_11				= 0;
m_Input3Mask_R01.SPARE_12				= 0;
m_Input3Mask_R01.SPARE_13				= 0;

m_Input3Mask_R01.SPARE_14 				= 0;
m_Input3Mask_R01.SPARE_15 				= 0;
m_Input3Mask_R01.SPARE_16 				= 0;
m_Input3Mask_R01.SPARE_17 				= 0;
m_Input3Mask_R01.SPARE_18 				= 0;

#elif(OHT_NAME == OHT_NAME_STD_V81S)
// ========== Input 1 설정 ==========

// 0 - 15
m_Input1Mask_R01.HAND_Inposition 		= 0;
m_Input1Mask_R01.HAND_Alarm				= 0;
m_Input1Mask_R01.HAND_Right_Grip 		= 0;
m_Input1Mask_R01.HAND_Left_Grip			= 0;

m_Input1Mask_R01.HAND_Close				= 0;
m_Input1Mask_R01.HAND_Open				= 0;
m_Input1Mask_R01.HAND_Push				= 0;
m_Input1Mask_R01.HOIST_Home	  			= 1;
m_Input1Mask_R01.HNAD_ES  				= 1;

m_Input1Mask_R01.HAND_PIO_Go 			= 0;
m_Input1Mask_R01.EQPIO_Load_REQ			= 0;
m_Input1Mask_R01.EQPIO_Unload_REQ		= 0;
m_Input1Mask_R01.EQPIO_Ready 			= 0;

m_Input1Mask_R01.EQPIO_HO_AVBL			= 0;
m_Input1Mask_R01.EQPIO_ES				= 0;
m_Input1Mask_R01.EQPIO_Left_Go			= 0;

// 16 - 31
m_Input1Mask_R01.STEER_F_Left			= 0;
m_Input1Mask_R01.STEER_R_Left			= 0;
m_Input1Mask_R01.STEER_F_Right			= 0;
m_Input1Mask_R01.STEER_R_Right			= 0;

m_Input1Mask_R01.SHUTTER_F_Close 		= 0;
m_Input1Mask_R01.SHUTTER_R_Close 		= 0;
m_Input1Mask_R01.SHUTTER_F_Open			= 0;
m_Input1Mask_R01.SHUTTER_R_Open			= 0;

m_Input1Mask_R01.SHUTTER_F_Alarm 		= 0;
m_Input1Mask_R01.SHUTTER_R_Alarm 		= 0;
m_Input1Mask_R01.SHUTTER_F_Inposition	= 0;
m_Input1Mask_R01.SHUTTER_R_Inposition	= 0;

m_Input1Mask_R01.OBSTACLE_Input_Extend	= 1;
m_Input1Mask_R01.OBSTACLE_Long			= 1;
m_Input1Mask_R01.OBSTACLE_Middle 		= 1;
m_Input1Mask_R01.OBSTACLE_Short			= 1;
m_Input2Mask_R01.OBSTACLE_Fail			= 1;


// ========== Input 2 설정 ==========
// 0 - 15
m_Input2Mask_R01.OHTDETECT_Input_Extend	= 1;
m_Input2Mask_R01.OHTDETECT_Long			= 1;
m_Input2Mask_R01.OHTDETECT_Middle		= 1;
m_Input2Mask_R01.OHTDETECT_Short 		= 1;
m_Input2Mask_R01.OHTDETECT_Fail			= 1;

m_Input2Mask_R01.LOOKDOWN_Detect 		= 1;
m_Input2Mask_R01.LOOKDOWN_Error			= 1;
m_Input2Mask_R01.SHIFT_Home             = 0;
m_Input2Mask_R01.STB_Detect_Left 		= 1;
m_Input2Mask_R01.STB_Detect_Right		= 1;
m_Input2Mask_R01.UTB_Detect				= 1;
m_Input2Mask_R01.FOUP_Detect 			= 1;
m_Input2Mask_R01.OSCILLATION 			= 0;

m_Input2Mask_R01.EMO_Press				= 1;
m_Input2Mask_R01.BUMPER_Press			= 0;
m_Input2Mask_R01.FAN_Fail				= 1;
m_Input2Mask_R01.HOIST_Belt_Detect1 	= 1;

// 16 - 31
m_Input2Mask_R01.HOIST_Belt_Detect2 	= 1;
m_Input2Mask_R01.TP_Enable				= 0;
m_Input2Mask_R01.HANDTP_Connect			= 0;
m_Input2Mask_R01.FOUP_Cover_Open_Detect	= 1;
m_Input2Mask_R01.STOP_Driving_Front		= 1;

m_Input2Mask_R01.STOP_Driving_PreCheck	= 1;
m_Input2Mask_R01.STOP_Trans_Front		= 1;
m_Input2Mask_R01.STOP_Trans_Rear 		= 1;
m_Input2Mask_R01.BCR_Driving_OK			= 0;

m_Input2Mask_R01.BCR_Driving_NG			= 0;
m_Input2Mask_R01.BCR_Trans_OK			= 0;
m_Input2Mask_R01.BCR_Trans_NG			= 0;
m_Input2Mask_R01.Reserved_1				= 0;

m_Input2Mask_R01.Reserved_2				= 0;

m_Input3Mask_R01.Reserved_3				= 0;
m_Input3Mask_R01.SMPS_Short				= 1;

// ========== tg 추가 ==========
m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
m_Input3Mask_R01.CID_Interlock_IN		 = 0;
m_Input3Mask_R01.CID_Completion_ACK 	 = 0;
m_Input3Mask_R01.CID_Status_IN			 = 0;
m_Input3Mask_R01.CID_Reserved_IN1		 = 0;
m_Input3Mask_R01.CID_Reserved_IN2		 = 0;

// ========= Input 3 설정 ==========
// 0 - 15
m_Input3Mask_R01.CID_Link_Compt_IN		= 0;
m_Input3Mask_R01.CID_Interlock_IN		= 0;
m_Input3Mask_R01.CID_Completion_ACK		= 0;
m_Input3Mask_R01.CID_Status_IN			= 0;

m_Input3Mask_R01.CID_Reserved_IN1		= 0;
m_Input3Mask_R01.CID_Reserved_IN2		= 0;
m_Input3Mask_R01.Rotate_0_Sensor		= 0;
m_Input3Mask_R01.Rotate_90_Sensor		= 0;

m_Input3Mask_R01.Rotate_180_Sensor		= 0;
m_Input3Mask_R01.Rotate_270_Sensor		= 0;
m_Input3Mask_R01.HNAD_Reserved_1	  	= 1;
m_Input3Mask_R01.HOIST_Home_2			= 1;
m_Input3Mask_R01.SHUTTER_F_Middle_Close = 0;

m_Input3Mask_R01.SHUTTER_R_Middle_Close = 0;
m_Input3Mask_R01.POD_EUV_1				= 1;
m_Input3Mask_R01.POD_EUV_2				= 0;
m_Input3Mask_R01.SPARE_5				= 0;

// 16 - 31
m_Input3Mask_R01.SPARE_6					= 0;
m_Input3Mask_R01.SPARE_7					= 0;
m_Input3Mask_R01.SPARE_8					= 0;
m_Input3Mask_R01.SPARE_9					= 0;

m_Input3Mask_R01.SPARE_10				= 0;
m_Input3Mask_R01.SPARE_11				= 0;
m_Input3Mask_R01.SPARE_12				= 0;
m_Input3Mask_R01.SPARE_13				= 0;

m_Input3Mask_R01.SPARE_14 				= 0;
m_Input3Mask_R01.SPARE_15 				= 0;
m_Input3Mask_R01.SPARE_16 				= 0;
m_Input3Mask_R01.SPARE_17 				= 0;
m_Input3Mask_R01.SPARE_18 				= 0;
#endif
}

//---------------------------------------------------------------------------

/**
@brief   AMC의 버전을 얻어오는 함수
@author  zzang9un
@date    2013.10.07
@return  AMC의 버전 정보(int)를 리턴
*/
int AMCLibrary::GetAMCVersion()
{
	return nAMCVer[0];
}
//---------------------------------------------------------------------------
/**
@brief   FPGA의 버전을 얻어오는 함수
@author  zzang9un
@date    2013.10.07
@return  AMC의 버전 정보(int)를 리턴
*/
int AMCLibrary::GetFPGAVersion()
{
	return (int)wFPGAVersion;
}
/**
@brief   AMC의 FileSystem 전체를 포맷하기 위한 함수
@author  ji7412.lee
@date    2014.02.10
@return
*/
int AMCLibrary::AMCFileSystemForamt()
{
	int nTmpError = 0, nAMCError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	//EnterCriticalSection(&m_CS);

	nAMCError = DeviceAMCFormat();
	nTmpError = ConvertAMCError(nAMCError);

	//LeaveCriticalSection(&m_CS);
	//AMC_PERIODIC_SEND_LOG("AMC FileSystemFormat Error:%d", nTmpError);

	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 Drive변수를 강제로 Set/Reset 시키는 함수
@author  ji7412.lee
@date    2014.02.10
@note    v6.5에서 AMCUpdate진행할때 DriverOpen Flag를 Reset시킴.
*/
int AMCLibrary::SetAMCDriverOpenByCommon(bool isOnOff)
{
	int nTmpError = 0;

	if(isOnOff)
		m_bDriverOpen = true;
	else
		m_bDriverOpen = false;

	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 DonwLoadFile Flag를 Set 시키는 함수
@author  ji7412.lee
@date    2014.02.10
@note    v6.5에서 AMCUpdate진행될때 FileDownloaded Flag를 Reset시킴.
*/
int AMCLibrary::SetAMCFileDownloaded(bool isOnOff)
{
	int nTmpError = 0;

	if(isOnOff)
		m_bAMCFileDownloaded = true;
	else
		m_bAMCFileDownloaded = false;

	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 ClearWatchDogStatus 함수
@author  ji7412.lee
@date    2014.02.10
@note    Common에서 요청할수 있도록 Public 추가
*/
int AMCLibrary::ClearWatchDogStatusPublic(void)
{
	int nTmpError = 0;
	nTmpError = ClearWatchDogStatus();
	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 DisableWatchDog 함수
@author  ji7412.lee
@date    2014.02.10
@note    Common에서 요청할수 있도록 Public 추가
*/
int AMCLibrary::DisableWatchDogPublic(void)
{
	int nTmpError = 0;
	nTmpError = DisableWatchDog();
	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 File System Install 함수
@author  ji7412.lee
@date    2014.02.10
*/
int AMCLibrary::AMCFileSystemInstall(char *pszBootLoader, char *pszBootFile, int *prtn, HWND hWnd, UINT uiMsg)
{
	int nTmpError = 0, nAMCError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	//EnterCriticalSection(&m_CS);

	nAMCError = DeviceAMCInstall(pszBootLoader, pszBootFile, prtn, hWnd, uiMsg);
	nTmpError = ConvertAMCError(nAMCError);

	//LeaveCriticalSection(&m_CS);
	//AMC_PERIODIC_SEND_LOG("AMC FileSystemInstall Error:%d", nTmpError);

	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC의 DSP_Reboot 함수
@author  ji7412.lee
@date    2014.02.10
*/
int AMCLibrary::DSP_Reboot(void)
{
	int nTmpError = 0, nAMCError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	//EnterCriticalSection(&m_CS);

	m_bDriverOpen = false;

	Sleep(500);
	DeviceAMCReboot();
	Sleep(3000);

	//LeaveCriticalSection(&m_CS);
	//AMC_PERIODIC_SEND_LOG("AMC DSP_Reboot Error:%d", nTmpError);
	return nTmpError;
}
//---------------------------------------------------------------------------
/**
@brief   AMC의 DPRAM 로그 저장
@author  sh17.jo
@date    2018.05.08
*/
void AMCLibrary ::Get_LogDpram(void)
{
    if(g_uLogDpramBase == 0x0)
    {
        AMC_ADD_LOG("[AMC Comm] DPram Bass Address is Zero");
        return;
    }

	LOG_DATA *pLD = new LOG_DATA;
	memset(pLD, 0, sizeof(LOG_DATA));
	memcpy(pLD, (LOG_DATA *)(g_uLogDpramBase), sizeof(LOG_DATA));
	if(pLD)
    {
		AMC_ADD_LOG("[AMC Comm] DPram Log = %x, DPram Base Address = %x [%d/%d/%d] %d/%d/%d/%d/%d", pLD->nSignature,g_uLogDpramBase, pLD->n1msCnt, pLD->n200usCnt, pLD->nDpramCnt
		,pLD->cmd_index,pLD->nMainLoopCnt,pLD->nDprIntClrCnt,pLD->status_DprIntClr,pLD->nAutoClear);

		delete pLD;
		pLD = NULL;
    }
    else
    {
        AMC_ADD_LOG("[AMC Comm] DPram Bass Address is Zero");
    }
}

//---------------------------------------------------------------------------
/**
@brief   AMC의 DPRAM 로그 저장
@author  sh17.jo
@date    2018.05.08
*/
bool AMCLibrary ::FPGARegRead(BYTE byOfsAddr, BYTE byRetryCnt, WORD &wReadData)
{
	BYTE byReady=0x0, byHigh=0x0, byLow=0x0;

	wReadData = 0x0;
	*EXP_BASE_ADDR = REG_READ_MODE | byOfsAddr;
	do
	{
		if(*EXP_BASE_ADDR == REG_READ_DONE)
		{
			byLow = *EXP_LSB_ADDR;
			byHigh = *EXP_MSB_ADDR;
			wReadData = ((byHigh << 8) & 0xff00) | byLow;
			return true;
		}
		Sleep(1);
	}
	while((--byRetryCnt) > 0);
	return false;
}

void AMCLibrary::Set_Jig(int n, bool bUse)
{
	bUseAxis_Jig[n] = bUse;

	int cnt = 0;
	for(int i= 0; i<4; i++)
	{
		if(bUseAxis_Jig[i] == true)
		{
			cnt++;
			bSetting_Jig = true;
		}
	}
	if(cnt == 0)
		bSetting_Jig = false;
}

void AMCLibrary::Axis_Ignore(bool bUse)
{
	if(bUse==true)
		  bAxis_Ignore = true;
}
