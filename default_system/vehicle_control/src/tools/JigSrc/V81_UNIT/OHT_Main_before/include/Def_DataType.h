/**
@file    Def_DataType.h
@version OHT 7.0
@brief   OHT에 사용될 Data Type(자료형)에 대한 정의
*/

#ifndef Def_DataTypeH
#define Def_DataTypeH

#include "Def_OHT.h"
#include "Def_Protocol.h"
//#include "Protocol_Define_Modify.h"

#include <Classes.hpp>
#include <dos.h>

#pragma pack(push, 1) // 1byte로 Pack
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// OCS Command 관련 구조체 정의
//---------------------------------------------------------------------------

// -------------------------------------------
// 주행 관련 Command
// -------------------------------------------
/**
@brief   Go 관련 Command(OCS->OHT) 구조체.
@author  zzang9un
@date    2013.04.01
@note    프로토콜 버전에 따라 확인할 것.
*/
typedef struct GO_CMD_
{
    // 공통
    BYTE CmdID;             ///< Command ID
    UINT StopNodeID;        ///< Target 주행 Node ID
    
    // 이적재로 tag로 Go 명령 시
    UINT StopStationID;     ///< Target 이적재 Tag ID
    int  StopStationOffset; ///< Node로부터 이적재 Tag까지 거리(mm)
    BYTE StopStationType;   ///< Target 이적재 Tag Type
    UINT NextNodeID;        ///< Next Target 주행 Node ID(Trans Node 주행 시 필요한 정보)

    // Speed, Accel Level
    BYTE LineSpeedLevel;    ///< Line Speed Level(50~100%)
    BYTE CurveSpeedLevel;   ///< Curve Speed Level(50~100%)
    BYTE AccelationLevel;   ///< Accelation Level(50~100%)

    // Pre Trans
    BYTE FinalPortType;     ///< 작업 구분(EQ, STB, UTB, Loader 등)
    int  TPosition_Shift;   ///< Shift Teaching값(mm)

	int Tposition_Rotate;

    // by zzang9un 2013.04.01 : 선조향을 위한 정보 추가(v1.5)    
    UINT PreSteeringNodeID; ///< 선조향을 위한 Node ID

	BYTE Buffer_Type;
} GO_CMD;

/**
@brief   MTL Insert 관련 Command(OCS->OHT) 구조체.
@author  zzang9un
@date    2013.03.12
*/
typedef struct CMD_MTL_INSERT_
{
    BYTE CmdID;             ///< Command ID
    UINT NodeID;            ///< Node ID
    int  Reserved1;         ///< Reserved 1
} CMD_MTL_INSERT;

/**
@brief   Map Make 관련 Command(OCS->OHT) 구조체.
@author  zzang9un
@date    2013.03.12
*/
typedef struct CMD_MAP_MAKE_
{
    BYTE CmdID;             ///< Command ID
    UINT EndNodeID;         ///< Map Make 완료 위치
    int  Speed;             ///< Map Make Speed(mm/s)
} CMD_MAP_MAKE;

/**
@brief   Clean 관련 Command(OCS->OHT) 구조체.
@author  zzang9un
@date    2013.03.12
@note    Clean 명령 취소시 CmdID만 유효하며 나머지 값은 0으로 설정해야 함.
*/
typedef struct CMD_CLEAN_
{
    BYTE CmdID;             ///< Command ID
    UINT EndNodeID;         ///< Clean 완료 위치
    int  Speed;             ///< Clean Speed(mm/s)
} CMD_CLEAN;

/**
@brief   기타 Go 관련 구조체
@author  zzang9un
@date    2013.03.18
*/
typedef struct GO_CMD_ETC_
{
    BYTE CmdID;             ///< Command ID
    UINT Data1;             ///< Data1
    int  Data2;             ///< Data2
} GO_ETC_CMD;

/**
@brief   기타 Go 관련 Command(OCS->OHT) 구조체.
@author  zzang9un
@date    2013.03.18
@note    MTL, Map Make, Clean 명령에 사용한다.
*/
typedef union uGO_ETC_CMD_
{
    CMD_MTL_INSERT  MTLCmd;         ///< MTL Insert
    CMD_MAP_MAKE    MapMakeCmd;     ///< Map Make
    CMD_CLEAN       CleanCmd;       ///< Clean

    GO_ETC_CMD      GoEtcCmd;       ///< Go Etc Cmd  Data
} uGO_ETC_CMD;

/**
@brief   Node Info 관련 Command(OCS->OHT) 구조체.
@author  zzang9un
@date    2013.03.12
*/
typedef struct CMD_NODEINFO_
{
    BYTE CmdID;             ///< Command ID
    UINT TargetNodeID;      ///< 최종목적지 Node ID
    int  NodeCount;         ///< Node 개수
    UINT *NodeID;           ///< 경유 Node ID #N
} CMD_NODEINFO;

// -------------------------------------------
// 이적재 관련 Command
// -------------------------------------------

/**
@brief   이적재 Option 구조체
@author  zzang9un
@date    2013.07.18
*/
typedef struct TRANS_OPTION_
{
    BYTE Reserved2      : 1;    ///<    (미사용)
	BYTE Reserved1      : 1;    ///<   (미사용)
    BYTE EQPIODisable_TP: 1;    ///< TP에서 EQPIO를 끄는 옵션
    BYTE HandDetectEQ   : 1;    ///< Hand Detect EQ 여부
    BYTE Oscillation    : 1;    ///< Oscillation On/Off  (미사용)
	BYTE TransSound     : 1;    ///< 이적재 소리 On/Off
	BYTE RFReader       : 1;    ///< RF Read 여부      (미사용)
    BYTE AutoRecovery   : 1;    ///< Auto Recovery 여부
} TRANS_OPTION;

/**
@brief   이적재 Option 유니온
@author  zzang9un
@date    2013.07.18
*/
typedef union uTRANS_OPTION_
{
    TRANS_OPTION Info;   ///< 기타 이적재 옵션
    BYTE         Data;   ///< Data
} uTRANS_OPTION;

/**
@brief   Trans 관련 Command(OCS->OHT) 구조체.

         다음 명령이 이 구조체를 사용함.
          - Load, Unload 명령
          - 명령에 따라 값이 유효하거나 유효하지 않을 수 있음.
@author  zzang9un
@date    2013.09.06
*/
typedef struct TRANS_CMD_
{
    BYTE CmdID;                 ///< Command ID
    UINT CurNodeID;             ///< 이적재 Tag가 속해 있는 DrivingNodeID ID
    UINT CurStationID;          ///< 이적재 Node ID
	BYTE PortType;              ///< 작업 구분
    BYTE CarrierType;           ///< 반송물 구분
    BYTE PIODirection;          ///< PIO Direction
    int  Tposition_Hoist;       ///< Hoist Teaching 값
    int  Tposition_Shift;       ///< Shift Teaching 값
    int  Tposition_Rotate;      ///< Rotate Teaching 값
    BYTE HoistSpeedLevel;       ///< Hoist 속도 레벨(50~100%)
	BYTE ShiftSpeedLevel;       ///< Shift 속도 레벨(50~100%)
	BYTE PIOTimeLevel;          ///< 포트 별 EQ PIO Timeover 레벨(1~10)
    BYTE LookDownLevel;         ///< Lookdown Sensor Level(1~16)
	uTRANS_OPTION TransOption;  ///< 이적재 옵션 셋팅값
	BYTE BufferType;			///< 이적재시 Buffer 위치 (미사용)
	UINT PIOID;             	///< RF PIO ID
	BYTE PIOCS;					///< RF PIO CS 신호 선택
	BYTE PIOChannel;		  	///< RF PIO PIOChannel (미사용시  0 )
} TRANS_CMD;

// -------------------------------------------
// 일반 Command
// -------------------------------------------
/**
@brief   OCS 공통 명령(OCS Common Command)를 Decode하기 위한 구조체
@author  zzang9un
@date    2013.03.13
*/
typedef struct OCS_CMD_COMMON_
{
    BYTE cData; ///< cData
    BYTE Data1; ///< Data1
    BYTE Data2; ///< Data2
} OCS_CMD_COMMON;

/**
@brief   Cancel Command(OCS->OHT) 구조체
@author  zzang9un
@date    2013.03.13
*/
typedef struct CMD_CTRL_CANCEL_OCS_OHT_
{
    BYTE CmdID;         ///< Command ID
    BYTE Cancel_CmdID;  ///< Cancel 할 Command ID
    BYTE CancelType;    ///< Cancel Type
} CMD_CTRL_CANCEL_OCS_OHT;

/**
@brief   ID Reset Command(OCS->OHT) 구조체
@author  zzang9un
@date    2013.03.13
*/
typedef struct CMD_CTRL_IDRESET_OCS_OHT_
{
    BYTE CmdID;     ///< Command ID
    BYTE Reserved1; ///< 미사용
    BYTE Reserved2; ///< 미사용
} CMD_CTRL_IDRESET_OCS_OHT;

/**
@brief   Pause Command(OCS->OHT) 구조체
@author  zzang9un
@date    2013.03.13
*/
typedef struct CMD_CTRL_PAUSE_OCS_OHT_
{
    BYTE CmdID;     ///< Command ID
    BYTE Reserved1; ///< 미사용
    BYTE Reserved2; ///< 미사용
} CMD_CTRL_PAUSE_OCS_OHT;

/**
@brief   Resume Command(OCS->OHT) 구조체
@author  zzang9un
@date    2012.11.07
*/
typedef struct CMD_CTRL_RESUME_OCS_OHT_
{
    BYTE CmdID;     ///< Command ID
    BYTE Reserved1; ///< 미사용
    BYTE Reserved2; ///< 미사용
} CMD_CTRL_RESUME_OCS_OHT;

/**
@brief   Resume Command(OCS->OHT) 구조체
@author  zzang9un
@date    2013.03.13
*/
typedef struct CMD_CTRL_ESTOP_OCS_OHT_
{
    BYTE EStopType; ///< OCS E-Stop 명령 Type
    BYTE Reserved1; ///< 미사용
    BYTE Reserved2; ///< 미사용
} CMD_CTRL_ESTOP_OCS_OHT;

/**
@brief   OCS Command(OCS->OHT) 구조체.

         이 구조체는 공용으로 사용하도록 Union으로 정의함.
         다음 명령이 이 구조체를 사용함.
          - E-Stop, Cancel, ID Reset, Pause, Resume 명령
@author  zzang9un
@date    2013.03.13
*/
typedef union uOCS_CMD_
{
    CMD_CTRL_CANCEL_OCS_OHT     Cancel;     ///< Cancel Command
    CMD_CTRL_IDRESET_OCS_OHT    IDReset;    ///< ID Reset Command
    CMD_CTRL_PAUSE_OCS_OHT      Pause;      ///< Pause Command
    CMD_CTRL_RESUME_OCS_OHT     Resume;     ///< Resume Command
    CMD_CTRL_ESTOP_OCS_OHT      EStop;      ///< E-Stop Command

    OCS_CMD_COMMON              OCSCmd;     ///< Decode를 위해 선언된 구조체
} uOCS_CMD;

// -------------------------------------------
// OHT 초기 정보 관련 
// -------------------------------------------
/**
@brief   OHT 초기 정보 응답 Command(OHT->OCS) 구조체
@author  zzang9un
@date    2013.03.13
*/
typedef struct CMD_INIT_INFO_OCS_OHT_
{
    BYTE CmdID;     ///< Command ID
    BYTE Reserved1; ///< 미사용
    BYTE Reserved2; ///< 미사용    
} CMD_INIT_INFO_OCS_OHT;

// -------------------------------------------
// OHT Data Update 요청 관련
// -------------------------------------------
/**
@brief   Map 정보 Update Request(OHT->OCS) 구조체
@author  zzang9un
@date    2013.03.13
*/
typedef struct REQ_UPDATE_MAPINFO_OHT_OCS_
{
    BYTE CmdID;         ///< Command ID
    UINT NodeID;        ///< Update할 주행 Node ID
    UINT StationID;     ///< Update할 이적재 Station ID
    int  StationOffset; ///< NodeID로부터의 Offset
    BYTE StationType;   ///< Station Type
    UINT NextNodeID;    ///< Station 다음 주행 NodeID
} REQ_UPDATE_MAPINFO_OHT_OCS;

/**
@brief   Teaching 정보 Update Request(OHT->OCS) 구조체
@author  zzang9un
@date    2013.03.13
*/
typedef struct REQ_UPDATE_TEACHINGINFO_OHT_OCS_
{
    BYTE CmdID;             ///< Command ID
    UINT NodeID;            ///< Update할 주행 Node ID
    UINT CurStationID;      ///< Update할 이적재 Station ID
    BYTE PortType;          ///< 작업 구분
    BYTE PIODirection;      ///< PIO 방향
    int  TPosition_Hoist;   ///< Hoist Teaching값
    int  TPosition_Shift;   ///< Hoist Teaching값
    int  TPosition_Rotate;  ///< Hoist Teaching값
} REQ_UPDATE_TEACHINGINFO_OHT_OCS;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// TP Command 관련 구조체 정의
//---------------------------------------------------------------------------
/**
@brief   일반적인 TP 명령(Common Command)를 Decode하기 위한 구조체
@author  zzang9un
@date    2012.11.19
*/
typedef struct TP_CMD_COMMON_
{
    BYTE CmdID;     ///< Command ID
    int  Data1;     ///< Data 1
    int  Data2;     ///< Data 2
    int  Data3;     ///< Data 3 
    int  Data4;     ///< Data 4
} TP_CMD_COMMON;

/**
@brief   TP Jog(Driving) 명령을 Decode하기 위한 구조체
@author  zzang9un
@date    2013.02.19
*/
typedef struct CMD_TP_JOG_DIST_
{
    BYTE CmdID;     ///< Command ID
    int  Accel;     ///< 가속도(mm/s^2)
    int  Decel;     ///< 감속도(mm/s^2)
    int  Velocity;  ///< 속도(mm/s)
    int  Distance;  ///< 거리(mm)
} CMD_TP_JOG_DIST;

/**
@brief   TP Jog(Hoist, Shift) 명령을 Decode하기 위한 구조체
@author  zzang9un
@date    2013.02.19
*/
typedef struct CMD_TP_JOG_TR_
{
    BYTE CmdID;     ///< Command ID
    int  AccelTime; ///< 가속시간(ms)
    int  DecelTime; ///< 감속시간(ms)
    int  Velocity;  ///< 속도(mm/s)
    int  Distance;  ///< 거리(mm)
} CMD_TP_JOG_TR;

typedef struct CMD_TP_JOG_DRV_
{
    BYTE CmdID;     ///< Command ID
    int  Accel;     ///< 가속도(mm/s^2)
    int  Decel;     ///< 감속도(mm/s^2)
    int  Velocity;  ///< 속도(mm/s)
    int  bJogInterlockIgnore;  ///< 조그 무시
} CMD_TP_JOG_DRV;

/**
@brief   OHT Init 정보를 주기 위한 구조체
@author  zzang9un
@date    2012.11.27
*/
typedef struct OHT_INFO_
{
    BYTE cData;     ///< Default : 0
    int  OHT_ID;    ///< Data 1
    int  LineInfo;  ///< Data 2
    int  OHT_Ver;   ///< Data 3 
    int  OHT_Type;  ///< Data 4
} OHT_INFO;

/**
@brief   Teaching 정보 중 일부를 Packing한 구조체
@author  zzang9un
@date    2013.07.19
*/
typedef struct TEACHING_INFO_ETC_
{
    UINT PIODirection   : 8; ///< PIO 방향(Left, Right)
    UINT PIOTimeLevel   : 8; ///< PIO Time Level
    UINT LookDownLevel  : 8; ///< LookDownLevel
    UINT Reserved2      : 1; ///< 
    UINT Reserved1      : 1; ///< 
    UINT EQPIODisable_TP: 1; ///< EQPIO Disable
    UINT HandDetectEQ   : 1; ///< Hand Detect EQ 여부
    UINT Oscillation    : 1; ///< Oscillation On/Off
    UINT TransSound     : 1; ///< 이적재 소리 On/Off
    UINT RFReader       : 1; ///< RF Read 여부
    UINT AutoRecovery   : 1; ///< Auto Recovery 여부
} TEACHING_INFO_ETC;

/**
@brief   TEACHING_INFO_ETC 구조체를 위한 Dummy 구조체
@author  zzang9un
@date    2013.07.19
*/
typedef struct TEACHING_INFO_ETC_DUMMY_
{
    UINT PIODirection   : 8; ///< PIO 방향(Left, Right)
    UINT PIOTimeLevel   : 8; ///< PIO Time Level
    UINT LookDownLevel  : 8; ///< LookDownLevel
    UINT TransOption    : 8; ///< Trans Option
} TEACHING_INFO_ETC_DUMMY;

/**
@brief   Teaching 정보 중 일부를 Packing한 union 구조체
@author  zzang9un
@date    2013.07.19
*/
typedef union uTEACHING_INFO_ETC_
{
    TEACHING_INFO_ETC Info;
    TEACHING_INFO_ETC_DUMMY Data;
} uTEACHING_INFO_ETC;

/**
@brief   OHT Teaching 정보를 TP로 보내기 위한 구조체
@author  zzang9un
@date    2013.07.19
*/
typedef struct TEACHING_INFO_
{
    BYTE PortType;              ///< PortType
    int  TPosition_Hoist;       ///< Teaching 값 : Hoist   
    int  TPosition_Shift;       ///< Teaching 값 : Shift
    int  TPosition_Rotate;      ///< Teaching 값 : Rotate
    uTEACHING_INFO_ETC EtcData;  ///< 기타 Teaching 정보
} TEACHING_INFO;

/**
@brief   Teaching 정보 저장 요청을 위한 구조체
@author  zzang9un
@date    2013.07.19
*/
typedef struct TEACHING_SAVE_
{
    BYTE CmdID;                 ///< Command ID
    int  PortType;              ///< Port Type
    int  Hoist;
	int  Shift;
	int  Rotate;
	BYTE PIOType;
	UINT PIOChannel;
	UINT PIOID;
    uTEACHING_INFO_ETC EtcData;  ///< 기타 Teaching 정보
    int  Reserved1;             ///< Reserved1
    int  Reserved2;             ///< Reserved2    
} TEACHING_SAVE;

/**
@brief   Mark 정보를 위한 구조체
@author  zzang9un
@date    2013.03.29
*/
typedef struct MAPMAKE_DATAINFO_
{
    BYTE DataType   : 1; ///< Data Type(0:Node, 1:Station)
    BYTE TagType    : 2; ///< Tag Type(0:주행, 1:Stop Tag, 2:QR_L, 3:QR_R)
    BYTE Reserved1  : 2; ///< Reserved
    BYTE Reserved2  : 2; ///< Reserved
} MAPMAKE_DATAINFO;

/**
@brief   Map Make 정보를 전송을 위한 구조체
@author  zzang9un
@date    2013.07.22
@note    Station ID와 StopTagType이 0인 경우 Offset은 Node와 Node 사이의 거리를 의미한다.
*/
typedef struct MAPMAKE_INFO_
{
    MAPMAKE_DATAINFO DataInfo;      ///< Map Make 수행 여부 flag
    int StartNodeID;    ///< Station Data Load Flag 
    int EndNodeID;      ///< Station Data Load Flag 
    int StationID;      ///< Station ID
    int Offset;         ///< Offset
} MAPMAKE_INFO;

/**
@brief   TP 공통 Command(TP->OHT) 구조체.

         이 구조체는 공용으로 사용하도록 Union으로 정의함.
         다음 명령이 이 구조체를 사용함.
          - TP 일반 Cmd, Jog관련 Cmd, OHT Info
@author  zzang9un
@date    2013.07.19
*/
typedef union uTP_CMD_
{
    CMD_TP_JOG_DRV  CmdTP_Jog_DRV;  ///< 주행 Jog를 위한 TP Command에 사용
    CMD_TP_JOG_DIST	CmdTP_Jog_DIST; ///< 거리 주행 Jog를 위한 TP Command에 사용
    CMD_TP_JOG_TR   CmdTP_Jog_TR;   ///< 이적재 Jog를 위한 TP Command에 사용
    OHT_INFO        OHTInfo;        ///< OHT 정보를 위한 구조체
    TEACHING_INFO   TeachingInfo;   ///< OHT Teaching 값 전송을 위한 구조체
    TEACHING_SAVE   TeachingSave;   ///< Teaching 정보 저장 요청을 위한 구조체
    MAPMAKE_INFO    MapMakeInfo;    ///< Map Make 정보 전송을 위한 구조체
    
    TP_CMD_COMMON   TPCmd;          ///< 일반적인 TP Command에 사용
} uTP_CMD;

/**
@brief   OHT Main 내부에서 명령을 처리하는 Command Data 구조체.

         OCS, TP로부터 수신한 모든 명령은 이 구조체 형태로 저장된다.
@author  zzang9un
@date    2012.11.14
*/
typedef struct CMD_DATA_
{
    uTAG_DATA   uCmdTag;    ///< Command Tag 정보

	GO_CMD      GoCmd;      ///< 주행 관련 Command
	uGO_ETC_CMD uGoCmdEtc;  ///< 기타 주행 관련 Command
	TRANS_CMD   TransCmd;   ///< 이적재 관련 Command
	uOCS_CMD    uOCS_CMD;   ///< OCS 공통 Command
	uTP_CMD     uTP_CMD;    ///< TP 공통 Command 
} CMD_DATA;

//---------------------------------------------------------------------------
// Status 관련 구조체
//---------------------------------------------------------------------------
/**
@brief   2~3축, Hand, Shutter 등의 Origin 정보를 가진 구조체
@author  zzang9un
@date    2013.03.12
*/
typedef struct ORIGIN_INFO_
{
    WORD Hoist      : 1; ///< Hoist 원점 여부
    WORD Shift      : 1; ///< Shift 원점 여부
    WORD Rotate     : 1; ///< Rotate 원점 여부
    WORD Hand       : 1; ///< Hand 원점 여부
    WORD Shutter    : 1; ///< Shutter 원점 여부
    WORD Clamp      : 1; ///< Clamp 원점 여부
    WORD Lift       : 1; ///< Lift 원점 여부
    WORD Sliding    : 1; ///< Sliding 원점 여부
    WORD Reserved   : 8; ///< Reserved
} ORIGIN_INFO;

/**
@brief   Mode, Status, CmdID를 관리하는 구조체
@author  zzang9un
@date    2013.03.21
*/
typedef struct MSCMDID_
{
    UINT CurCmdID   : 8; ///< Current Command ID
    UINT PreCmdID   : 8; ///< Previous Command ID
    UINT Status     : 8; ///< OHT 상태
    UINT Mode       : 8; ///< OHT 모드
} MSCMDID;

/**
@brief   Mode, Status, CmdID를 관리하는 구조체(Union)
@author  zzang9un
@date    2013.03.21
*/
typedef union uMSCMDID_
{
    MSCMDID Info; ///< Status와 Cmd ID를 관리하는 구조체
    UINT    Data; ///< Data
} uMSCMDID;


/**
@brief   OHT 각 요소별 사용 여부 정보를 가지고 있는 구조체
@author  puting
@date    2012.11.23
*/
typedef struct USE_INFO_
{
    UINT BCR                    : 1; ///< BCR 사용 여부
    UINT PBSSensor              : 1; ///< PBS 사용 여부
    UINT SoundAlarm             : 1; ///< Sound 사용 여부
    UINT Lamp                   : 1; ///< Lamp 사용 여부
    UINT ShutterControl         : 1; ///< Shutter 제어 사용 여부
    UINT DirectLogSave          : 1; ///< 로그 바로 저장 여부
    UINT EQ_PIO                 : 1; ///< EQ PIO 사용 여부
    UINT STBSensor              : 1; ///< STB 센서 사용 여부

    UINT HandLimitSensorCheck   : 1; ///< Hand Limit 센서 체크 여부
    UINT HandControl            : 1; ///< Hand 제어 사용 여부
    UINT SteeringControl        : 1; ///< 조향 제어 사용 여부
    UINT UTBSensor              : 1; ///< UTB 센서 사용 여부
    UINT LookDownSensor         : 1; ///< Lookdown 센서 사용 여부
    UINT HandPushSensorCheck    : 1; ///< Hand Push 센서 체크 여부
    UINT GripperSen             : 1; ///< Gripper 센서 사용 여부
    UINT HoistAlignSensor       : 1; ///< Hoist 원점 센서 사용 여부
    
    UINT OscillationSensor      : 1; ///< Oscillation 센서 사용여부
    UINT ErrSound               : 1; ///< 에러 경고음 사용 여부
    UINT DrivingSound           : 1; ///< 주행 Sound 사용 여부
    UINT TransSound             : 1; ///< 이적재 Sound 사용 여부
    UINT ObsDetectSound         : 1; ///< OBS 센서 감지 Sound 사용 여부
    UINT IgnorePIOError         : 1; ///< PIO 에러 무시 여부
    UINT HoistHomePos_PIOError  : 1; ///< Hoist 원점 위치에서 PIO 에러 발생 여부
	UINT ObsIgnoreSound         : 1; ///< OBS 무시 Sound 사용 여부

    UINT LocSave_Trans_ing      : 1; ///< 이적재중 Log 저장 여부
    UINT PushAbnormalOnCheck    : 1; ///< PushAbnormalOnCheck
    UINT CrossRail_Interlock    : 1; ///< Crossrail 인터락
    UINT UTBFoupCheck           : 1; ///< UTB Foup 체크 여부
    UINT InnerFoupDetectSen     : 1; ///< Inner Foup 감지 센서 사용 여부
    UINT Reserved1              : 1; ///<미사용
    UINT Reserved2              : 1; ///<미사용
    UINT Reserved3              : 1; ///<미사용
} USE_INFO;

/**
@brief   USE_INFO를 위한 Union
@author  puting
@date    2012.11.19
*/
typedef union uCMD_USE_DATA_
{
    // Decode를 위해 선언된 구조체
    USE_INFO Info;
    UINT Data;
    
} uCMD_USE_DATA;


/**
@brief   Motor 정보 구조체
@author  zzang9un
@date    2013.09.10
*/
typedef struct MOTOR_INFO_
{
    // Motor 정보
    int  Position;  ///< 이동 거리(mm), 단 Hoist, Shift의 경우 (1/10000m)
    int  Speed;     ///< 이동 속도(mm/s)
    UINT ErrorCode; ///< 에러 코드
    BYTE AMPEnable; ///< AMP Enable
    BYTE IsRunning; ///< Running 중인지 판단하는 변수

    // 2016-12-03, Add by Labeler
    BYTE IsOrigin; ///< Rotate Motor의 Origin 유/무 체크 위해 추가
} MOTOR_INFO;

/**
@brief   Version 정보 전달을 위한 구조체
@author  zzang9un
@date    2013.03.26
*/
typedef struct VERSION_DATA_
{
    UINT Major      : 8; ///< Major
    UINT Minor      : 8; ///< Minor
    UINT Release    : 8; ///< Release
    UINT Build      : 8; ///< Build
} VERSION_DATA;

/**
@brief   Version 정보 전달을 위한 Union
@author  zzang9un
@date    2013.03.26
*/
typedef union uVERSION_DATA_
{
    VERSION_DATA Info;
    UINT         Data;
} uVERSION_DATA;

/**
@brief   Mark 정보를 위한 구조체
@author  zzang9un
@date    2013.03.29
*/
typedef struct MARK_INFO_
{
    BYTE DrivingTag : 2; ///< 주행 Tag Marked
    BYTE StopTag    : 2; ///< Stop Tag Marked
    BYTE QRLTag_L   : 2; ///< QR Left Tag Marked
    BYTE QRLTag_R   : 2; ///< QR Right Tag Marked
} MARK_INFO;

/**
@brief   Mark 정보를 위한 Union
@author  zzang9un
@date    2013.03.29
*/
typedef union uMARK_INFO_
{
    MARK_INFO Info;
    BYTE Data;
} uMARK_INFO;

/**
@brief   Data Load 정보를 위한 구조체
@author  zzang9un
@date    2013.07.22
*/
typedef struct DATALOADED_
{
    BYTE Map        : 1; ///< Map Data Load Flag
    BYTE Station    : 1; ///< Station Data Load Flag 
    BYTE Teaching   : 1; ///< Teaching Data Load Flag 
    BYTE Reserved1  : 1; ///< Reserved
    BYTE Reserved2  : 1; ///< Reserved
    BYTE Reserved3  : 1; ///< Reserved
    BYTE Reserved4  : 1; ///< Reserved
    BYTE Reserved5  : 1; ///< Reserved
} DATALOADED;

/**
@brief   Data Load 정보를 위한 union
@author  zzang9un
@date    2013.07.22
*/
typedef union uDATALOADED_
{
    DATALOADED Info;
    BYTE Data;
} uDATALOADED;

/**
@brief   OHT Status 기본 Data
@author  zzang9un
@date    2013.03.20
@note    OCS와 TP, 자가진단 등 OHT Status를 위한 기본 Data를 모아 생성한 구조체
*/
typedef struct OHT_COMMON_STATUS_
{
    // 기본 정보
    // by zzang9un 2013.03.21 : 아래 4개의 변수를 구조체 하나로 통합함
    // BYTE Mode;              ///< OHT 모드
    // BYTE Status;            ///< OHT 상태
    // BYTE PreCmdID;          ///< Previous Command ID
    // BYTE CurCmdID;          ///< Current Command ID
    uMSCMDID MSCmdID;       ///< Mode, Status, CmdID 구조체

    BYTE NextCmdID;         ///< Next Command ID

    UINT CurNodeID;         ///< 현재 주행 Node ID
    int  CurNodeOffset;     ///< 현재 주행 Node로부터 이동한 거리(mm)
    UINT CurStationID;      ///< 현재 이적재 Node ID
    UINT StopNodeID;        ///< Target 주행 Node ID
    UINT StopStationID;     ///< Target 이적재 Node ID
    int  StopStationOffset; ///< 목적 Node로부터 Target Trans Node의 거리(mm)
    BYTE ExistFoup;         ///< 재하 존재 여부
    BYTE CarrierType;       ///< 반송물 종류
    BYTE PauseStatus;       ///< OHT Pause 상태(장애물, 대차 감지 여부)
    BYTE SteerPosition;     ///< 조향 정보
    UINT ErrorCode;         ///< OHT 에러 코드

	ORIGIN_INFO OriginInfo; ///< 2~4축, Hand, Shutter 원점 여부


} OHT_COMMON_STATUS;
/**
@brief   Status(OHT->OCS) 구조체
@author  zzang9un
@date    2012.11.04
*/
typedef struct MSG_STATUS_OHT2OCS_
{
    OHT_COMMON_STATUS Data; ///< OHT 기본 정보

    // RF Tag Data
    char RFTagData[10];         ///< RF Tag Data

    // AP 정보
    UINT APSignalLevel;         ///< OHT 무선 신호 레벨
    BYTE APMacAddr[6];          ///< AP Mac Address

    // Motor 정보
    int  Motor_Drv_F_Position;  ///< OHT 주행 축 Encoder 값(mm)
    int  Motor_Drv_F_Speed;     ///< OHT 주행 속도(mm/s)
    int  Motor_Hoist_Position;  ///< Hoist축 이동 거리(mm)
    int  Motor_Shift_Position;  ///< Shift축 이동 거리(mm)
    int  Motor_Rotate;          ///< Rotate축 이동 거리(radian)

    BYTE OHTType;               ///< OHT Type

    // by zzang9un 2013.09.06 : Data 버전 정보
    int MapData_CurRevision;
    int StationData_CurRevision;
    int TeachingData_CurRevision;

    // by zzang9un 2013.05.14 : 7.0 특별 Status(HOHT와 구별되는 부분)
    // HID 정보
    WORD HID_Erec;          ///< HID 2차측 입력전압
    WORD HID_Edc;           ///< HID 2차측 출력전압
    WORD HID_Idc;           ///< HID 2차측 전류
    WORD HID_Ecap;          ///< HID 캡 뱅크 전압
    WORD HID_Kw;            ///< HID 출력 파워
    WORD HID_ErrorCode;     ///< HID Error Code
    WORD HID_PickupTemper;  ///< HID Pick up 온도
    WORD HID_CPUTemper;     ///< ???

    // IO 정보
    UINT Input[3];          ///< OHT Input Data #1~3
    UINT Output[3];         ///< OHT Output Data #1~3
} MSG_STATUS_OHT2OCS;

/**
@brief   Status(OHT->TP) 구조체
@author  puting
@date    2012.11.19
@note    Input, Output의 경우 Light-On, Dark-On을 고려할 필요 없음
*/
typedef struct MSG_STATUS_OHT2TP_
{
    OHT_COMMON_STATUS Data; ///< OHT 기본 정보

    // HID 정보
    WORD HID_Erec;          ///< HID 2차측 입력전압
    WORD HID_Edc;           ///< HID 2차측 출력전압
    WORD HID_Idc;           ///< HID 2차측 전류
    WORD HID_Ecap;          ///< HID 캡 뱅크 전압
    WORD HID_Kw;            ///< HID 출력 파워
    WORD HID_ErrorCode;     ///< HID Error Code
    WORD HID_PickupTemper;  ///< HID Pick up 온도
    WORD HID_CPUTemper;     ///< ???

    // IO 정보
    UINT Input[3];          ///< OHT Input Data #1~3
    UINT Output[3];         ///< OHT Output Data #1~3  
    
    // 추가 정보 
    // OHT Use Info
    uCMD_USE_DATA UseInfo;  ///< Use OHT Data(유니온)

    // Motor 관련 정보
    MOTOR_INFO Driving_F;   ///< 주행축 Front
    MOTOR_INFO Driving_R;   ///< 주행축 Rear
    MOTOR_INFO Hoist;       ///< Hoist 축
    MOTOR_INFO Shift;       ///< Shift 축
    MOTOR_INFO Rotate;      ///< Rotate 축

    // UBG 관련
    BYTE IsUBGSen;          ///< UBG Sensor 유무
    BYTE UBGDetectLevel;    ///< UBG Sensor Level

    // PBS 관련
    BYTE IsPBSSen;          ///< PBS Sensor 유무
    BYTE PBSDetectlevel;    ///< PBS Sensor Level

    BYTE IsFCPause;         ///< FC Pause
    UINT TPCommandType;     ///< TP CommandType 4자리
    BYTE Cmd_Count;         ///< Cmd 개수

    BYTE AMCResponse;       ///< AMC Response
    UINT MapMakeData;      ///< Map Make Data(단위 : mm)
    BYTE DownPercent;       ///< Down Percent
    double CPUTemperature;  ///< IPC 온도
    UINT APSignalLevel;     ///< 와이파이 신호

    // Log 관련 정보
    BYTE isLogDrive;       ///< Log Save Mode

    BYTE STBDirection;      ///< STB Direction
    BYTE IsRotateExist;     ///< Rotate Exist
    BYTE IsRFPIORS23;       ///< RF PIO RS23 유무
    BYTE IsRFPIO6Digit;     ///< FR PIO 6Digit 유무
    BYTE IsFBWF;            ///< FBWF 유무
    BYTE IsRFPIOType;       ///< RF PIO Type  

    // by zzang9un 2013.03.29 : 버전 정보 관련 구조체
    uVERSION_DATA OHTVer;   ///< OHT Version
    uVERSION_DATA AMCVer;   ///< AMC Version

    // by zzang9un 2013.03.29 : Mark 정보를 위한 구조체 추가
    uMARK_INFO MarkInfo;    ///< Mark 정보를 위한 구조체

    // by zzang9un 2013.07.22 : Data 로드 정보 추가
    uDATALOADED DataLoaded; ///<  Data 로드 정보
} MSG_STATUS_OHT2TP;

///**
//@brief   HID 정보를 담은 구조체
//@author  zzang9un
//@date    2012.11.07
//*/
typedef struct HID_DATA_
{
	int Edc;                ///<레귤레이터 출력전압: 300 -> 300V
	int Ecap;               ///< 캡 뱅크 전압 : 3자리
	int Eout;               ///< 출력 전압
	char Iout_sign;         ///< 출력전류방향(+,-) : -방향이면 회생모드
	int Iout;               ///< 레큘레이터 입력전류
	int Iac;                ///< Pickup 입력 전류

	int ErrorCode;          ///< 에러코드 : 000~999 까지
//	int WarningCode;        ///< warning 코드

	int PickupTemperature;  ///< 132 -> 132도

	int IGBTTemperature;
	int ECap1Temperature;
	int ECap2Temperature;
	int ECapRoundTemperature;
	int Reserve1;
	int Reserve2;
	int Reserve3;
} HID_DATA;

/**
@brief   APSIGNAL 정보를 담은 구조체
@author  puting
@date    2013.03.26
*/
typedef struct APSIGNAL_DATA_
{
	int 	nAPSignal;
	int 	nAPSignalLevel;
	unsigned int     nChannel;
	unsigned char 	szAPMacAddress[6];
	unsigned char 	szSSID[32];
} APSIGNAL_DATA;

/**
@brief   SIGMA 정보를 담은 구조체
@author  puting
@date    2013.07.10
*/
typedef struct SIGMA_DATA_
{
	DWORD dwTempErrcode;
	long lTempReadData;
} SIGMA_DATA;

// ---------------------------------------------------------------------------
/**
@brief   IPC_DATA_INFO를 담은 구조체
@author  puting
@date    2013.03.28
*/
typedef struct _IPC_DATA_INFO_ {
	double dCPUTemperature; ;
	///< IPC CPU 온도
	double dVoltage_Input;
	///< IPC 입력 전압
} IPC_DATA_INFO;


/**
@brief   IPC_SYS_INFO를 담은 구조체
@author  puting
@date    2013.03.28
*/
typedef struct _IPC_SYS_INFO_
{
	char szOHTSysTime[16];

	BYTE DrvType;
	char szFileSysName[16];
	UINT64 nfreeSpace;
	UINT64 nTotalSpace;
} IPC_SYS_INFO;


/**
@brief   CPU_INFO를 담은 구조체
@author  puting
@date    2013.03.28
*/
typedef struct _CPU_INFO_ {
	int nCPU;
	long lMem;
} CPU_INFO;

/**
@brief   OHT_MONITORING_SYS_INFO를 담은 구조체
@author  puting
@date    2013.07.16
*/
typedef struct _OHT_DATA_INFO_
{
	 //Data info
	double dCurr_DrivingVel;
	double dCurr_HoistVel;
	double dCurr_ShiftVel;
	double dCurr_RotateVel;

	int IOInput[4];
	int IOOutput[4];
	char nFan_Alive1; //IOInput
	int nAMCVer;

} OHT_DATA_INFO;

/**
@brief   Operation Status 정보들을 담은 모니터링 구조체
@author  doori.shin
@date    2014.06.11
*/
typedef struct _MONITOR_OPERATION_STATUS
{
	BYTE bIsFBWF;
	BYTE LogSaveMode;
	BYTE bIs_LogSave;
	BYTE bIs_USBAlive;
	UINT nLogCount;

	BYTE bIs_OHTInk;
	BYTE bIs_AssistantInk;
	BYTE bIs_VNCInk;

	IPC_SYS_INFO IPCSysInfo;  //시스템정보(디스크, 시간)

	char szDBVersion[16];   //V3 DB Version
	char szVirusVersion[16];  //V3 Version
	UINT nOHTVer;
	UINT nAMCVer;
	UINT nAssistantVer;
	UINT nMapVer;         //Map Revision
	UINT nStationVer;     //Station Revision
	UINT nTeachingVer;    //Teaching Revision
	UINT nCIDVer;         //CID Revision
	UINT nCollisionVer;   //Collision Revision
	UINT nLooKDownVer;    //Lookdown Revision
	UINT nFlowControlVer; //Flowcontrol Revision

	CPU_INFO CPUandMemInfo;   //CPU and Memory 점유율
	BYTE Is_VirusCheckProgram;  //실행중인 프로그램
	BYTE bIsVirusCheckProgramExec; //실행갯수

	int Reserve1;
	int Reserve2;
	int Reserve3;
	int Reserve4;
	int Reserve5;
}MONITOR_OPERATION_STATUS;



/**
@brief   Movement 정보들을 담은 모니터링 구조체
@author  doori.shin
@date    2014.06.11
*/
typedef struct _MONITOR_MOVEMENT_DATA
{
	double dSum_DrivingPosition;
	double dSum_HoistPosition;
	double dSum_ShiftPosition;
	double dSum_RotatePosition;
	UINT nSum_SteerMoveCount;
	UINT nSum_HandMoveCount;
	UINT nSum_ShutterMoveCount;

	UINT nMaxHandMovingTime;
	UINT nMaxShutterMovingTime;
	UINT nMaxFrontSteeringMovingTime;
	UINT nMaxRearSteeringMovingTime;

	double fMaxVelgap;
	UINT nVelgapEventCount;
	UINT nCountHandReset;
	UINT nFollowingLimitOverCountDrivingAxis;
	UINT nFollowingLimitOverCountHoistAxis;
	UINT nFollowingLimitOverCountShiftAxis;

	int Reserve1;
	int Reserve2;
	int Reserve3;
	int Reserve4;
	int Reserve5;
	int Reserve6;
}MONITOR_MOVEMENT_DATA;


/**
@brief   OHT Disk 정보 구조체
@author  doori.shin
@date    2013.12.18
*/
typedef struct DISK_INFO_
{
	char  Drive;
	INT64 TotalSpace;
	INT64 FreeSpace;
	INT64 UsedSpace;
}DISK_INFO;

/**
@brief    Monitor 구조체
@author  bDAyTA  Jeong
@date    2013.06.03
*/
typedef struct _MONITOR_DATA_INFO_ {
	//IPC//
	IPC_DATA_INFO	IPCData;

	//HID//
	HID_DATA HIDData;

	//Network//
	APSIGNAL_DATA APSignalData;

	//CPU 정보 //
	CPU_INFO pValue;

	//Servo Pack//
	SIGMA_DATA SigmaData;

	 //Monitoring Info//
	 MONITOR_OPERATION_STATUS    MonitorOperStatus;         //<--이부분 추가
	 MONITOR_MOVEMENT_DATA       MonitorMovementData;    //<--이부분 추가

} MONITOR_DATA_INFO;

/**
@brief   OHT 및 Monitor Info 관리 구조체
@author  by Jeong
@date    2013.06.03
*/
typedef struct _TOTAL_DATA_INFO_
{
	MONITOR_DATA_INFO MonitorInfo;
	OHT_DATA_INFO OHTInfo;
}TOTAL_DATA_INFO;


/**
@brief   IO Map Field 구조체
@author  by doori.shin
@date    2013.08.14
*/
typedef struct IO_MAP_FIELD_
{
	bool fIsLoaded;

    int InputNum;
    int OutputNum;

	AnsiString* InputField;
    AnsiString* OutputField;
}IO_MAP_FIELD;
//---------------------------------------------------------------------------

/*
@brief	 Motion 제어기로 부터 받는 User 단위로 변환된 Node 정보
@author  임태웅
@date	 2013.06.30
*/
typedef struct STATUS_NODE_CHECK_DATA_
{
	int NodeCountOfPreCheck;	///< 노드 개수
	int NodeCount;				///< 노드 개수

	// SetHome, setOffset 시 하면 현재 위치를 뺀 값으로 재저장
	double	PositionOfDrivingTagPreCheck;	///< 주행 노드 사전 감지된 위치
	double	PositionOfDrivingTag;			///< 주행 노드 감지된 위치
	double	PositionOfTransTagFirst;		///< 이적재 노드 감지된 위치(Front)
	double	PositionOfTransTagFinal;		///< 이적재 노드 감지된 위치(Rear)

}STATUS_NODE_CHECK_DATA;

#pragma pack(pop) // Pack 해제


#endif // Def_DataTypeH
