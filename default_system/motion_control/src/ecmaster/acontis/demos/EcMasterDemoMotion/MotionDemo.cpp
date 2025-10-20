/*-----------------------------------------------------------------------------
 * MotionDemo.cpp            
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Christoph Widmann
 * Description              EtherCAT Master demo application
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include <AtEthercat.h>

#include "MotionDemo.h"
#include "Logging.h"
#undef DEBUGTRACE
#include "EcFiFo.h"
#include "AtXmlParser.h"
#include "ECMotionControl.h"

#ifdef ATEMRAS_SERVER 
#include <AtEmRasSrv.h>
#endif

/*-DEFINES-------------------------------------------------------------------*/
#define DCM_ENABLE_LOGFILE
#define MOTION_ENABLE_LOGFILE
#define MOTION_LOG_NUM_MSGS 1000
#undef MOTION_INPOSITION_CHECK
#undef MOTION_ENABLE_TORQUE_LIMIT
#define MOTION_YASK_STAT_TORQUE_LIMIT_ACT  (1 << 14) /* Specific to Yaskawa Sigma V drive */
#define MOTION_YASK_CTRL_TORQUE_LIMIT_EN   (1 << 11 | 1 << 12) /* Specific to Yaskawa Sigma V drive */

#define LogMsg      S_poLog->LogMsg
#define LogError    S_poLog->LogError
#define LogDcm      S_poLog->LogDcm
#define TRC(msg) if (pDemoAxis->pFb->ReadAxisInfo.Verbose > 0) { LogMsg(msg); }
#define LOG_BUFSIZE 200

/* definitions for motion log file */
#ifdef FILESYS_8_3
#define MOTION_DATALOGGERFILE             (EC_T_CHAR*)"mot"
#else
#define MOTION_DATALOGGERFILE             (EC_T_CHAR*)"motionlog"
#endif
#define MOTION_DATALOGGERFILEEXTENSION    (EC_T_CHAR*)"csv"
#define MOTION_DATALOGGERROLLOVERLIMIT    31000

#define AT_XMLBUF_SIZE 64

// Uncomment to active move sequence test
//#define MOVE_TEST_SEQUENCE
// Uncomment to use ELMO specific functions
//#define ELMO_DRIVE

#ifdef ELMO_DRIVE
#define DRV_OBJ_GAIN_SCHEDULING     0x2E00      /**< Gain scheduling manual index */
#define DRV_OBJ_SMOOTH_FACTOR       0x31D9      /**< Smooth factor index */
#define DRV_OBJ_USER_INT            0x2F00      /**< General purpose user integer array  */
#define DRV_OBJ_USER_FLOAT          0x2F01      /**< General purpose user float array  */
#endif

#ifdef EC_MOTION_SUPPORT_PP_MODE
// Homing on index pulse, negative direction
#define HOMING_MODE                 33
// Homing on index pulse, positive direction
//#define HOMING_MODE                 34
// Speed during search for switch, object 0x6099:1
#define HOMING_SPEED_SEARCH_SWITCH  5000.0
// Speed during search for zero, object 0x6099:2
#define HOMING_SPEED_SEARCH_ZERO    2500.0
// Homing acceleration, object 0x609a:1
#define HOMING_ACCELERATION         10000.0
// Home offset, object 0x607c
#define HOMING_OFFSET               0.0
// Homing "zero" position for FB MC_Home
#define HOMING_POSITION             10.0
#endif

/*-TYPEDEFS------------------------------------------------------------------*/
typedef struct
{
    /* trace file support */
    CAtEmLogging*               poLogging;
    struct _MSG_BUFFER_DESC*    pvLogBufDesc;
} MCLOG_DESC;

/*-LOCAL VARIABLES-----------------------------------------------------------*/
static EC_T_DWORD          S_dwClntId        = 0;
static CAtEmLogging*       S_poLog           = EC_NULL;
static T_DEMO_THREAD_PARAM S_DemoThreadParam = {0};
static EC_T_PVOID          S_pvtJobThread    = EC_NULL;
static volatile EC_T_DWORD S_dwJobTaskCycleCnt = 0; // Must be volatile because variable is accessed from different threads.
static EC_T_DWORD          S_dwMotionStartCycle;
static int                 S_numAxes;
#ifdef MOTION_ENABLE_LOGFILE
static bool                S_motionLogEnabled;
static MC_T_REAL           S_cycleTimeS;
#endif
#ifdef ATEMRAS_SERVER 
static EC_T_BOOL           S_bRasSrvStarted  = EC_FALSE;
static EC_T_PVOID          S_pvRemoteApiSrvH = EC_NULL;
#endif
static EC_T_BOOL           S_bEnaPerfJobs    = EC_FALSE;
static EC_T_TSC_MEAS_DESC  S_TscMeasDesc;
static EC_T_CHAR*          S_aszMeasInfo[MAX_JOB_NUM] =
{
    (EC_T_CHAR*)"JOB_ProcessAllRxFrames",
    (EC_T_CHAR*)"JOB_SendAllCycFrames  ",
    (EC_T_CHAR*)"JOB_MasterTimer       ",
    (EC_T_CHAR*)"JOB_SendAcycFrames    ",
    (EC_T_CHAR*)"Cycle Time            ",
    (EC_T_CHAR*)"myAppWorkPd           ",
    (EC_T_CHAR*)"Write DCM logfile     ",
    (EC_T_CHAR*)"MC Calc Val 1         ",
    (EC_T_CHAR*)"MC Calc Val 2         ",
    (EC_T_CHAR*)"MC Calc Val 3         "

};

static MCLOG_DESC  S_McLogDesc[MAX_NUMOF_MASTER_INSTANCES];

/*-FORWARD DECLARATIONS------------------------------------------------------*/
static EC_T_DWORD ecatNotifyCallback(EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
#if (defined ATEMRAS_SERVER)
static EC_T_DWORD RasNotifyWrapper(EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
#endif
static EC_T_VOID  tEcJobTask(EC_T_VOID* pvThreadParamDesc);

#ifdef ELMO_DRIVE
MC_T_DWORD SetGainScheduling(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_WORD       wValue              /**< [in] Gain value */
    );

MC_T_DWORD SetSmoothFactor(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_DWORD      dwValue             /**< [in] Smooth factor value */
    );

MC_T_DWORD SetUserInteger(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_BYTE       bySubIndex,         /**< [in] Subindex 0..24 */
    MC_T_DWORD      dwValue             /**< [in] Smooth factor value */
    );

MC_T_DWORD GetUserInteger(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_BYTE       bySubIndex,         /**< [in] Subindex 0..24 */
    MC_T_DWORD*     pdwValue            /**< [in] Pointer to buffer */
    );

MC_T_DWORD SetUserFloat(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_BYTE       bySubIndex,         /**< [in] Subindex 0..24 */
    MC_T_REAL       fValue              /**< [in] User float value */
    );

MC_T_DWORD GetUserFloat(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_BYTE       bySubIndex,         /**< [in] Subindex 0..24 */
    MC_T_REAL*      pfValue             /**< [in] Pointer to buffer */
    );
#endif


/*-MYAPP---------------------------------------------------------------------*/
/* Demo code: Remove/change this in your application */

/* -MYAPP-DEFINES----------------------------------------------------------- */
#define DEMO_MAX_NUM_OF_AXIS    4                       /* number of axis */

/* MotionDemo Command Codes */
#define MC_CMD_CODE_POWER_ON        1           
#define MC_CMD_CODE_POWER_OFF       2
#define MC_CMD_CODE_STOP            3
#define MC_CMD_CODE_MOVE_RELATIVE   4
#define MC_CMD_CODE_MOVE_ABSOULTE   5
#define MC_CMD_CODE_READ_POS        6
#define MC_CMD_CODE_MOVE_VELOCITY   7
#define MC_CMD_CODE_RESET           8
#define MC_CMD_CODE_HALT            9
#define MC_CMD_READ_PARAMETER       10
#define MC_CMD_READ_BOOL_PARAMETER  11
#define MC_CMD_WRITE_PARAMETER      12
#define MC_CMD_WRITE_BOOL_PARAMETER 13
#define MC_CMD_HOME                 14

/*-PACK SETTINGS-------------------------------------------------------------*/

/* Align/pack structures to 8 byte for proper access to double variables.
 * This is for avoiding misaligned accesses (faults on ARM / PPC) if the
 * object are placed on stack.
 * The cmd structures are manually padded to 8 bytes, so overall structure
 * size is not changed (same as pack 1).
 */
#include EC_PACKED_INCLUDESTART(8)

#if defined  __GNUC__
#  undef EC_PACKED /* GCC has problems with the pack attribute */
#  define EC_PACKED(alignment)  __attribute__(( aligned(alignment) ))
#endif

/* Input command structure from EC-STA */
typedef struct
{
    EC_T_DWORD      dwCmdCode;
    EC_T_DWORD      dwSlaveAddress; // Low word station address, High word axis index
    EC_T_DWORD      dwIncPerMM;
    EC_T_DWORD      dwDirection;
    MC_T_REAL       fVel;
    MC_T_REAL       fAcc;
    MC_T_REAL       fDec;
    MC_T_REAL       fJerk;
    MC_T_REAL       fDistance;
} EC_PACKED(8) MC_T_CMD_INP_PARA;

/* extended version */
typedef struct
{
    EC_T_DWORD      dwCmdCode;
    EC_T_DWORD      dwStationAddress;
    EC_T_DWORD      dwIncPerMM;
    EC_T_DWORD      dwDirection;
    MC_T_REAL       fVel;
    MC_T_REAL       fAcc;
    MC_T_REAL       fDec;
    MC_T_REAL       fJerk;
    MC_T_REAL       fDistance;
    EC_T_DWORD      dwContUpdate;
    EC_T_DWORD      dwParaNumber;       /* Write Parameter: Number */
    EC_T_BOOL       bParaValue;         /* Write Bool Parameter: Value */
    EC_T_DWORD      dwRes4;
    MC_T_REAL       fParaValue;         /* Write Parameter: Value */
    EC_T_DWORD      dwRes5;
    EC_T_DWORD      dwRes6;
    EC_T_DWORD      dwRes7;
    EC_T_DWORD      dwRes8;
    EC_T_DWORD      dwRes9;
    EC_T_DWORD      dwRes10;
} EC_PACKED(8) MC_T_CMD_INP_PARA_V2;

/* Output command structure to EC-STA */
typedef struct
{
    MC_T_REAL           fPosition;
} EC_PACKED(8) MC_T_CMD_OUT_PARA_V1;

typedef struct
{
    MC_T_REAL           fPosition;
    EC_T_WORD           wProfileState;          /* DS 402 or SERCOS drive state   */
    MC_T_WORD           ePLCOpenState;          /* PLCOpen state        */
    MC_T_WORD           wStatusWord;            /* Status Word  0x6041 */
    MC_T_WORD           wControlWord;           /* Control Word 0x6040 */
    EC_T_SDWORD         lActualPosition;        /* Object 0x6064 */
    EC_T_SDWORD         lTargetPosition;        /* Object 0x607A */
} EC_PACKED(8) MC_T_CMD_READ_POS;                            /* Size = 24 bytes */

typedef struct
{
    EC_T_DWORD      dwParaNumber;       /* Read Parameter: Number */
    EC_T_BOOL       bParaValue;         /* Read Bool Parameter: Value */
    MC_T_REAL       fParaValue;         /* Read Parameter: Value */
    MC_T_BOOL       bValid;             /* Value available */
    MC_T_BOOL       bError;             /* Indicates if an error has occurred */
    MC_T_WORD       wErrorID;           /* Error identification */
    MC_T_WORD       wRes;
    EC_T_DWORD      dwRes2;             /* Size = 32 bytes */
} EC_PACKED(8) MC_T_CMD_READ_PARA;

typedef union
{
    MC_T_CMD_READ_POS   oReadPos;
    MC_T_CMD_READ_PARA  oReadPara;
} EC_PACKED(8) MC_T_CMD_OUT_PARA_V2;

/*-PACK SETTINGS-------------------------------------------------------------*/
#include EC_PACKED_INCLUDESTOP

static EC_T_BOOL        S_bCmdMode = EC_FALSE;        /* In command mode the MotionDemo gets commands from EC-STA */

static  CFiFoListDyn<MC_T_CMD_INP_PARA_V2>*    S_pPendMotionCmdFifo = EC_NULL;    /* FIFO containing pending motion commands */

/* Motion Control Function Blocks (MCFB's) 
 * This structure contains C++ objects. Don't initialize to zero!
 */
class EC_T_FUNCTION_BLOCKS
{
public:

    MC_T_AXIS_INIT                    AxisInitData;            // MCFB axis initialization data
    MC_T_AXIS_REF                     Axis;                    // MCFB axis reference

    MC_POWER_T                        Power;
#if defined(EC_MOTION_TRAJECTORY_GEN)
    MC_STOP_T                         Stop;
    MC_HALT_T                         Halt;
#endif /* defined(EC_MOTION_TRAJECTORY_GEN) */
    MC_RESET_T                        Reset;
#if defined(EC_MOTION_TRAJECTORY_GEN)
    MC_MOVE_RELATIVE_T                MoveRelative;
    MC_MOVE_ABSOLUTE_T                MoveAbsolute;
    MC_MOVE_VELOCITY_T                MoveVelocity;
#endif /* defined(EC_MOTION_TRAJECTORY_GEN) */
    MC_READ_ACTUAL_POSITION_T         ReadActualPosition;
    MC_READ_PARAMETER_T               ReadParameter;
    MC_READ_BOOL_PARAMETER_T          ReadBoolParameter;
    MC_WRITE_PARAMETER_T              WriteParameter;
    MC_WRITE_BOOL_PARAMETER_T         WriteBoolParameter;
#if defined(EC_MOTION_TRAJECTORY_GEN)
    MC_READ_MOTION_STATE_T            ReadMotionState;
#endif /* defined(EC_MOTION_TRAJECTORY_GEN) */
    MC_READ_AXIS_INFO_T               ReadAxisInfo;
    AMC_CHECK_TARGETPOS_REACHED_T     CheckTargetposReached;
    AMC_HALT_RECOVERY_T               HaltRecovery;
#ifdef EC_MOTION_SUPPORT_PP_MODE
    MC_HOME_T                         Home;
#endif
    MC_READ_DIGITAL_INPUT_T           ReadDigitalInput;
    MC_READ_DIGITAL_OUTPUT_T          ReadDigitalOutput;
    MC_WRITE_DIGITAL_OUTPUT_T         WriteDigitalOutput;
private:

    EC_T_FUNCTION_BLOCKS & operator=(const EC_T_FUNCTION_BLOCKS &) { return *this; }
};

/* Index for process data variable */
struct EC_T_VARIDX
{
   EC_T_DWORD dwPdoIdx;
   EC_T_DWORD dwVarIdx;
};

enum eMoveState
{
   EMoveSetup,
   EMoving,
   EPause
};

struct EC_T_MOTION_PARAMETER
{
   MC_T_REAL fCommandPos, fCommandVel, fCommandAcc, fCommandJerk;
   MC_T_REAL fFollowingError;
};

struct EC_T_LOGINFO
{
   EC_T_MOTION_PARAMETER curPos;
};


/* data for one axis */
typedef struct  
{
    /* read from DemoConfig.xml */
    EC_T_WORD                    wDriveFixAddress;       
    EC_T_BYTE                    byDriveModesOfOperation;
    EC_T_BYTE                    bySercosDriveNo;
    EC_T_DWORD                   dwCoeIdxOpMode;
    EC_T_DWORD                   dwDriveJerk;
    EC_T_DWORD                   dwDriveAcc;
    EC_T_DWORD                   dwDriveDec;
    EC_T_DWORD                   dwDriveVel;
    EC_T_DWORD                   dwDriveDistance;
    EC_T_DWORD                   dwDriveIncPerMM;
    EC_T_DWORD                   dwDriveIncFactor;
    EC_T_DWORD                   dwPosWindow;
    EC_T_DWORD                   dwVelocityGain;        /* CSV-Mode: Velocity Gain */
                                                        /* CSP-Mode: Velocity Gain for Feed Forward Object 0x60B1 */  
    MC_T_DWORD                   dwTorqueGain;          /* CSP and CSV-Mode: Torque Gain for Feed Forward Object 0x60B2 */  
    EC_T_VARIDX                  IdxStatusword;         /* default for DS402: DRV_OBJ_STATUS_WORD                 0x6041 */
    EC_T_VARIDX                  IdxControlword;        /* default for DS402: DRV_OBJ_CONTROL_WORD                0x6040 */
    EC_T_VARIDX                  IdxActualPos;          /* default for DS402: DRV_OBJ_POSITION_ACTUAL_VALUE       0x6064 */
    EC_T_VARIDX                  IdxTargetPos;          /* default for DS402: DRV_OBJ_TARGET_POSITION             0x607A */
    EC_T_VARIDX                  IdxTargetVel;          /* default for DS402: DRV_OBJ_TARGET_VELOCITY             0x60FF */
    EC_T_VARIDX                  IdxTargetTrq;          /* default for DS402: DRV_OBJ_TARGET_TORQUE               0x6071 */
    EC_T_VARIDX                  IdxActualTrq;          /* default for DS402: DRV_OBJ_ACTUAL_TORQUE               0x6077 */
    EC_T_VARIDX                  IdxVelOffset;          /* default for DS402: DRV_OBJ_VELOCITY_OFFSET             0x60B1 */
    EC_T_VARIDX                  IdxTorOffset;          /* default for DS402: DRV_OBJ_TORQUE_OFFSET               0x60B2 */
    EC_T_VARIDX                  IdxModeOfOperation;    /* default for DS402: DRV_OBJ_MODES_OF_OPERATION          0x6060 */
    EC_T_VARIDX                  IdxModeOfOperationDisplay;    /* default for DS402: DRV_OBJ_MODES_OF_OPERATION_DISPLAY          0x6061 */

    MC_T_AXIS_PROFILE            eDriveProfile;

    /* EtherCAT info */
    EC_T_DWORD                   dwSlaveId;
    EC_T_WORD                   *pwPdStatusWord;         /* Drive status word */
    EC_T_WORD                   *pwPdControlWord;        /* Drive control word */

    EC_T_SDWORD                 *plPdActualPosition;     /* ptr to actual position in process data */
                                                         /* Data type DINT (INTEGER32 �2147483648 to +2147483627) */
    EC_T_SDWORD                 *plPdTargetPosition;     /* ptr to target position: Data type DINT (INTEGER32 �2147483648 to +2147483627)
                                                            In Cyclic Synchronous Position mode, it is always interpreted as an absolute value */

    EC_T_SDWORD                 *plPdTargetVelocity;     /* ptr to target velocity: Data type DINT (INTEGER32 �2147483648 to +2147483627) */

    EC_T_SDWORD                 *plPdVelocityOffset;     /* ptr to velocity offset for feed forward: Data type DINT (INTEGER32 �2147483648 to +2147483627) */
    EC_T_SWORD                  *psPdTorqueOffset;       /* ptr to torque offset for feed forward: Data type INT (INTEGER16 �32768 to +32767) */

    EC_T_SWORD                  *psPdActualTorque;       /* ptr to actual torque: Data type INT (INTEGER16 �32768 to +32767) */
    EC_T_SWORD                  *psPdTargetTorque;       /* ptr to target torque: Data type INT (INTEGER16 �32768 to +32767) */
    EC_T_BYTE                   *pbyPdModeOfOperation;   /* ptr to mode of operation (DS402 0x6060) */
    EC_T_BYTE                   *pbyPdModeOfOperationDisplay;   /* ptr to mode of operation display (DS402 0x6061) */

    EC_T_DWORD                  *pdwDigInputs;
    EC_T_DWORD                  *pdwDigOutputs;

#ifdef EC_MOTION_SUPPORT_PP_MODE
    EC_T_DWORD                  *plPdProfileVelocity;    /* ptr to profile velocity for PP mode and homing mode */
    EC_T_DWORD                  *plPdProfileAcc;         /* ptr to profile acc for PP mode and homing mode */
    EC_T_DWORD                  *plPdProfileDec;         /* ptr to profile dec for PP mode and homing mode */
#endif

    /* MCFB data */
    EC_T_FUNCTION_BLOCKS        *pFb;
    EC_T_BOOL                    bTriggerMoveRel;
    EC_T_BOOL                    bTriggerMoveAbs;
    EC_T_BOOL                    bTriggerMoveVelo;
    EC_T_BOOL                    bTriggerCheckPos;
    EC_T_BOOL                    bTorqueLimitActive;

    /* helpers for autonomous mode */
    EC_T_INT                     eDistanceState;
    eMoveState                   eMotionState;
    EC_T_INT                     nWaitCounter1;

    /* Logging */
    EC_T_LOGINFO                 log;
} EC_T_DEMO_AXIS;


static EC_T_DEMO_AXIS       S_aAxisList[DEMO_MAX_NUM_OF_AXIS];  /* Demo code: List of axis */

static EC_T_DWORD myAppInit     (CAtEmLogging* poLog, EC_T_INT nVerbose);
static EC_T_DWORD myAppPrepare  (CAtEmLogging* poLog, EC_T_INT nVerbose, EC_T_VOID* pvCfgFileHandle, EC_T_DWORD dwBusCycleTimeUsec);
static EC_T_DWORD myAppSetup    (CAtEmLogging* poLog, EC_T_INT nVerbose, EC_T_DWORD dwClntId, EC_T_BYTE* pbyPDIn, EC_T_BYTE* pbyPDOut);
static EC_T_DWORD myAppWorkpd   (CAtEmLogging* poLog, EC_T_INT nVerbose, EC_T_BYTE* pbyPDIn, EC_T_BYTE* pbyPDOut, EC_T_DWORD dwInstanceID, EC_T_STATE eMasterState, EC_T_DWORD dwBusCycleTimeUsec);
static EC_T_DWORD myAppDiagnosis(CAtEmLogging* poLog, EC_T_INT nVerbose);
static EC_T_DWORD myAppNotify   (EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
#ifdef MOVE_TEST_SEQUENCE
static EC_T_DWORD myAppMoveTest (CAtEmLogging* poLog, EC_T_INT nVerbose);
#endif

static EC_T_DEMO_AXIS* myAppSearchAxis(
    EC_T_DWORD              dwSlaveAddress);

#ifdef MOTION_ENABLE_LOGFILE
static EC_T_DWORD motionLogEnable
    (CAtEmLogging*				poLogging
	,EC_T_DWORD                 dwInstanceID					/* Master Instance ID */
    ,EC_T_BYTE*                 pbyLogBuffer 
	,EC_T_DWORD                 dwNumMsgs
	,EC_T_DWORD                 dwMotionLogBufSize
    ,EC_T_CHAR*                 szFilenamePrefix);
static void motionLogPos(EC_T_DWORD dwInstanceID, EC_T_DWORD dwBusCycleTimeUsec);
static void motionLogWriteRowHeader(EC_T_DWORD dwInstanceID);
#endif

/* Demo code: End */

/*-FUNCTION DEFINITIONS------------------------------------------------------*/

/*
 * Reset pending drive errors
 */
static void ResetDriveErrors()
{
   int i;
   for (i = 0; i < S_numAxes; ++i)
   {
      EC_T_DEMO_AXIS *pDemoAxis = &S_aAxisList[i];

      if (pDemoAxis->pFb->AxisInitData.AxisType == MC_AXIS_TYPE_VIRTUAL) continue;

      pDemoAxis->pFb->ReadAxisInfo.DriveErrorAck =  MC_FALSE;

      // DriveErrorAckReq is set to 1 if FB MC_Reset() want's to acknowledge axis errors.
      if (! pDemoAxis->pFb->ReadAxisInfo.DriveErrorAckReq) continue;

      switch (pDemoAxis->eDriveProfile)
      {
      case MC_T_AXIS_PROFILE_SERCOS:
         {
            // S-0-0099: A C1D error is reset if this procedure command is called.
            EC_T_BYTE flags = SOE_BM_ELEMENTFLAG_VALUE;

            EC_T_WORD wCmd = 0x3; // Set & enable cmd
            ecatSoeWrite(pDemoAxis->dwSlaveId, pDemoAxis->bySercosDriveNo, &flags, DRV_IDN_CLEAR_FAULT,
               (EC_T_BYTE *)&wCmd, sizeof(EC_T_WORD), 100);

            wCmd = 0; // Cancel cmd
            ecatSoeWrite(pDemoAxis->dwSlaveId, pDemoAxis->bySercosDriveNo, &flags, DRV_IDN_CLEAR_FAULT,
               (EC_T_BYTE *)&wCmd, sizeof(EC_T_WORD), 100);
            
            pDemoAxis->pFb->ReadAxisInfo.DriveErrorAck =  MC_TRUE;
            break;
         }
      case MC_T_AXIS_PROFILE_DS402:
         {
            // Not implemented
            break;
         }
      default:
         break;
      } // end switch
   }
}

/********************************************************************************/
/** \brief  Motion demo Application.
*
* This is a Motion demo application.
* 
* \return  Status value.
*/
EC_T_DWORD MotionDemo(
    CAtEmLogging*       poLog
   ,EC_T_CNF_TYPE       eCnfType            /* [in] Enum type of configuration data provided */
   ,EC_T_PBYTE          pbyCnfData          /* [in] Configuration data */                      
   ,EC_T_DWORD          dwCnfDataLen        /* [in] Length of configuration data in byte */    
   ,EC_T_DWORD          dwBusCycleTimeUsec  /* [in]  bus cycle time in usec */
   ,EC_T_INT            nVerbose            /* [in]  verbosity level */
   ,EC_T_DWORD          dwDuration          /* [in]  test duration in msec (0 = forever) */
   ,EC_T_LINK_PARMS*    poLinkParms         /* [in]  pointer to link parameter */
   ,EC_T_VOID*          pvTimingEvent       /* [in]  Timing event handle */
   ,EC_T_DWORD          dwCpuIndex          /* [in]  SMP only: CPU index */
   ,EC_T_BOOL           bEnaPerfJobs        /* [in]  Performance measurement */
#ifdef ATEMRAS_SERVER 
   ,EC_T_WORD           wServerPort         /* [in]   Remote API Server Port */
#endif
   ,EC_T_LINK_PARMS*    poLinkParmsRed      /* [in]  Redundancy Link Layer Parameter */
   ,EC_T_OS_PARMS*      poOsParms           /* [in]  pointer to OS parameters  */
   ,EC_T_DCM_MODE       eDcmMode            /* [in]  DCM mode */
   ,EC_T_VOID*          pvCfgFileHandle     /* [in]  handle to DemoConfig.xml file */
   ,EC_T_BYTE*          pbyMotionLogBuffer  /* [in]  pointer to motion logging buffer (EC_NULL, only file system) */
   ,EC_T_DWORD          dwMotionLogBufSize  /* [in]  length of motion logging buffer */
   ,EC_T_CHAR*          szLogPrefix
)
{
    EC_T_DWORD       dwRetVal = EC_E_NOERROR;
    EC_T_DWORD       dwRes    = EC_E_NOERROR;
    EC_T_BOOL        bRes     = EC_FALSE;
    CEcTimer         oTimeout;
    CEmNotification* pNotification = EC_NULL;
    CEcTimer         oDcmStatusTimer;
    EC_T_BOOL        bFirstDcmStatus = EC_TRUE;
    EC_T_INT         nDcmCtlSetVal = ((dwBusCycleTimeUsec*2)/3)*1000; /* set value in nanosec, 66% of the bus cycle */
    EC_T_BOOL        bDcmCtlOff = EC_FALSE;

    /* store parameters */
    S_poLog = poLog;
    S_bEnaPerfJobs = bEnaPerfJobs;
    S_DemoThreadParam.pvTimingEvent = pvTimingEvent;

    /* check if interrupt mode is selected */
    if (poLinkParms->eLinkMode != EcLinkMode_POLLING)
    {
        dwRetVal = EC_E_INVALIDPARM;
        LogError("Error: Link layer in 'interrupt' mode is not supported by EcMasterDemoMotion. Please select 'polling' mode.");
        goto Exit;
    }
    /* set thread affinity */
    {
        EC_T_CPUSET CpuSet;

        EC_CPUSET_ZERO(CpuSet);
        EC_CPUSET_SET(CpuSet, dwCpuIndex);
        bRes = OsSetThreadAffinity(EC_NULL, CpuSet);
        if (!bRes)
        {
            dwRetVal = EC_E_INVALIDPARM;
            LogError("Error: Set thread affinitiy, invalid CPU index %d\n", dwCpuIndex);
            goto Exit;
        }
    }
    /* create notification context */
    pNotification = EC_NEW(CEmNotification(INSTANCE_MASTER_DEFAULT, poLog));
    if (EC_NULL == pNotification)
    {
        dwRetVal = EC_E_NOMEMORY;
        goto Exit;
    }
    pNotification->Verbose(nVerbose);

    if (S_bEnaPerfJobs)
    {
        PERF_MEASURE_JOBS_INIT(EC_NULL);
    }

    if (S_bCmdMode)
    {
        if (0 != nVerbose) LogMsg( "==========================================================" );
        if (0 != nVerbose) LogMsg( "Command mode enabled! Motion operation controlled remotely" );
        if (0 != nVerbose) LogMsg( "==========================================================" );
    }
    /* create command fifo list */
    S_pPendMotionCmdFifo = EC_NEW(CFiFoListDyn<MC_T_CMD_INP_PARA_V2>(32, EC_NULL, (EC_T_CHAR*)"PendMotionCmd"));
    if (S_pPendMotionCmdFifo == EC_NULL)
    {
        LogError((EC_T_CHAR*)"ERROR: Can not allocate motion command queue!\n");
        goto Exit;
    }
    /* Get DCM control setval */
    atParamRead(pvCfgFileHandle, eDataType_DWORD,  MOTIONDEMO_CFG_TAG_CTL_SETVAL, (EC_T_BYTE*)&nDcmCtlSetVal, sizeof(nDcmCtlSetVal));

    /* Enables the Command Mode */
    S_bCmdMode = EC_FALSE;
    atParamRead(pvCfgFileHandle, eDataType_BOOL,  MOTIONDEMO_CFG_TAG_CMD_MODE, (EC_T_BYTE*)&S_bCmdMode, sizeof(S_bCmdMode));

#if (defined MOTION_ENABLE_LOGFILE)
    if (nVerbose >= 3)
    {
        motionLogEnable(poLog, 0, pbyMotionLogBuffer, MOTION_LOG_NUM_MSGS, dwMotionLogBufSize, szLogPrefix);
        S_cycleTimeS = dwBusCycleTimeUsec / 1000000.0;
    }
#endif
    /* Demo code: Remove/change this in your application: Initialize application */
    /*****************************************************************************/
    dwRes = myAppInit(poLog, nVerbose);
    if (EC_E_NOERROR != dwRes)
    {
        LogError( (EC_T_CHAR*)"myAppInit failed, error code: 0x%x", dwRes );
        dwRetVal = dwRes;
        goto Exit;
    }
#ifdef ATEMRAS_SERVER
    /*******************************/
    /* Start RAS server if enabled */
    /*******************************/
    if (0xFFFF != wServerPort)
    {
        ATEMRAS_T_SRVPARMS oRemoteApiConfig;

        OsMemset(&oRemoteApiConfig, 0, sizeof(ATEMRAS_T_SRVPARMS));
        oRemoteApiConfig.oAddr.dwAddr       = 0;                    /* INADDR_ANY */
        oRemoteApiConfig.wPort              = wServerPort;
        oRemoteApiConfig.dwCycleTime        = REMOTE_CYCLE_TIME;    /* 2 msec */
        oRemoteApiConfig.dwWDTOLimit        = (REMOTE_WD_TO_LIMIT/REMOTE_CYCLE_TIME); /* WD Timeout after 10secs */
        oRemoteApiConfig.dwReConTOLimit     = 6000;                 /* Reconnect Timeout after 6000*2msec + 10secs */
        oRemoteApiConfig.dwMasterPrio       = MAIN_THREAD_PRIO;
        oRemoteApiConfig.dwClientPrio       = MAIN_THREAD_PRIO;
        oRemoteApiConfig.pvNotifCtxt        = pNotification;        /* Notification context */
        oRemoteApiConfig.pfNotification     = RasNotifyWrapper;     /* Notification function for emras Layer */
        oRemoteApiConfig.dwConcNotifyAmount = 100;                  /* for the first pre-allocate 100 Notification spaces */
        oRemoteApiConfig.dwMbxNotifyAmount  = 50;                   /* for the first pre-allocate 50 Notification spaces */
        oRemoteApiConfig.dwMbxUsrNotifySize = 3000;                 /* 3K user space for Mailbox Notifications */
        oRemoteApiConfig.dwCycErrInterval   = 500;                  /* span between to consecutive cyclic notifications of same type */
        if (0 != nVerbose) LogMsg("Start Remote API Server now\n");
        dwRes = emRasSrvStart(oRemoteApiConfig, &S_pvRemoteApiSrvH);
        if (EC_E_NOERROR != dwRes)
        {
            LogError("ERROR: Cannot spawn Remote API Server\n");
        }
        S_bRasSrvStarted = EC_TRUE;
    }
#endif
    /******************************/
    /* Initialize EtherCAT master */
    /******************************/
    if (0 != nVerbose) LogMsg( "==========================" );
    if (0 != nVerbose) LogMsg( "Initialize EtherCAT Master" );
    if (0 != nVerbose) LogMsg( "==========================" );
    {
        EC_T_INIT_MASTER_PARMS oInitParms;

        OsMemset(&oInitParms, 0, sizeof(EC_T_INIT_MASTER_PARMS));
        oInitParms.dwSignature                   = ATECAT_SIGNATURE;
        oInitParms.dwSize                        = sizeof(EC_T_INIT_MASTER_PARMS);
        oInitParms.pLinkParms                    = poLinkParms;
        oInitParms.pLinkParmsRed                 = poLinkParmsRed;
        oInitParms.dwBusCycleTimeUsec            = dwBusCycleTimeUsec;
        oInitParms.dwMaxBusSlaves                = MASTER_CFG_ECAT_MAX_BUS_SLAVES;
        oInitParms.dwMaxQueuedEthFrames          = MASTER_CFG_MAX_QUEUED_ETH_FRAMES;
        oInitParms.dwMaxSlaveCmdPerFrame         = MASTER_CFG_MAX_SLAVECMD_PER_FRAME;
        if (dwBusCycleTimeUsec < 1000)
        {
            oInitParms.dwMaxSentQueuedFramesPerCycle = 1;
        }
        else
        {
            oInitParms.dwMaxSentQueuedFramesPerCycle = MASTER_CFG_MAX_SENT_QUFRM_PER_CYC;
        }
        oInitParms.dwEcatCmdMaxRetries           = MASTER_CFG_ECAT_CMD_MAX_RETRIES;
        oInitParms.dwEoETimeout                  = MASTER_CFG_EOE_TIMEOUT;
        oInitParms.dwFoEBusyTimeout              = MASTER_CFG_FOE_BUSY_TIMEOUT;
        oInitParms.dwLogLevel                    = nVerbose;
        oInitParms.pfLogMsgCallBack              = CAtEmLogging::OsDbgMsgHookWrapper;
        oInitParms.pOsParms                      = poOsParms;
        dwRes = ecatInitMaster(&oInitParms);
        if (dwRes != EC_E_NOERROR)
        {
            dwRetVal = dwRes;
            LogError("Cannot initialize EtherCAT-Master! (Result = %s 0x%x)", ecatGetText(dwRes), dwRes);
            goto Exit;
        }
    }
    
    /* Create cyclic task to trigger master jobs */
    /*********************************************/
    S_DemoThreadParam.bJobThreadRunning  = EC_FALSE;
    S_DemoThreadParam.bJobThreadShutdown = EC_FALSE;
    S_DemoThreadParam.pLogInst           = S_poLog;
    S_DemoThreadParam.pNotInst           = pNotification;
    S_DemoThreadParam.dwCpuIndex         = dwCpuIndex;
    S_DemoThreadParam.dwBusCycleTimeUsec = dwBusCycleTimeUsec;
    S_pvtJobThread = OsCreateThread((EC_T_CHAR*)"tEcJobTask", tEcJobTask,
#if !(defined EC_VERSION_GO32)
                                    JOBS_THREAD_PRIO,
#else
                                    dwBusCycleTimeUsec,
#endif
                                    JOBS_THREAD_STACKSIZE, (EC_T_VOID*)&S_DemoThreadParam);
#ifdef RTAI
    OsMakeThreadPeriodic(S_pvtJobThread, dwBusCycleTimeUsec);
#endif
    /* wait until thread is running */
    oTimeout.Start(2000);
    while (!oTimeout.IsElapsed() && !S_DemoThreadParam.bJobThreadRunning)
    {
        OsSleep(10);
    }
    if (!S_DemoThreadParam.bJobThreadRunning)
    {
        dwRetVal = EC_E_TIMEOUT;
        LogError("Timeout starting JobTask");
        goto Exit;
    }
    oTimeout.Stop();

    /* Configure master */
    dwRes = ecatConfigureMaster(eCnfType, pbyCnfData, dwCnfDataLen);
    if (dwRes != EC_E_NOERROR)
    {
        dwRetVal = dwRes;
        LogError("Cannot configure EtherCAT-Master! %s (Result = 0x%x)", ecatGetText(dwRes), dwRes);
        goto Exit;
    }
    /* configure DC/DCM if master is started with ENI */
    if (pbyCnfData != EC_NULL)
    {
        /* configure DC */
        {
            EC_T_DC_CONFIGURE oDcConfigure;

            OsMemset(&oDcConfigure, 0, sizeof(EC_T_DC_CONFIGURE));
            oDcConfigure.dwTimeout          = ETHERCAT_DC_TIMEOUT;
            oDcConfigure.dwDevLimit         = ETHERCAT_DC_DEV_LIMIT;
            oDcConfigure.dwSettleTime       = ETHERCAT_DC_SETTLE_TIME;
            if (eDcmMode_MasterRefClock == eDcmMode)
            {
                oDcConfigure.dwTotalBurstLength = 10000; 
                oDcConfigure.dwBurstBulk        = 1;
            }
            else
            {
                oDcConfigure.dwTotalBurstLength = ETHERCAT_DC_ARMW_BURSTCYCLES; 
                if (dwBusCycleTimeUsec < 1000)
                {
                    /* if the cycle time is below 1000 usec, we have to reduce the number of frames sent within one cycle */
                    oDcConfigure.dwBurstBulk = ETHERCAT_DC_ARMW_BURSTSPP / 2;
                }
                else
                {
                    oDcConfigure.dwBurstBulk = ETHERCAT_DC_ARMW_BURSTSPP;
                }
            }
            if (eDcmMode_Dcx == eDcmMode)
            {
                oDcConfigure.bAcycDistributionDisabled = EC_FALSE; /* Enable acyclic distribution if cycle time is above 1000 usec to get DCX in sync */
            }
            else
            {
                oDcConfigure.bAcycDistributionDisabled = EC_TRUE;
            }
            dwRes = ecatDcConfigure(&oDcConfigure);
            if (dwRes != EC_E_NOERROR )
            {
                dwRetVal = dwRes;
                LogError("Cannot configure DC! (Result = 0x%x)", dwRes);
                goto Exit;
            }
        }
        /* configure DCM */
        {
            EC_T_DCM_CONFIG oDcmConfig;
            EC_T_BOOL       bLogEnabled = EC_FALSE;
#ifdef DCM_ENABLE_LOGFILE
            if (pNotification->Verbose() >= 3)
            {
                bLogEnabled = EC_TRUE;
            }
#endif
            if (bDcmCtlOff)
            {
                if (0 != nVerbose) LogMsg("DCM control loop disabled for diagnosis!\n");
            }
            OsMemset(&oDcmConfig, 0, sizeof(EC_T_DCM_CONFIG));
            switch (eDcmMode)
            {
            case eDcmMode_Off:
                oDcmConfig.eMode = eDcmMode_Off;
                break;
            case eDcmMode_BusShift:
                oDcmConfig.eMode = eDcmMode_BusShift;
                oDcmConfig.u.BusShift.nCtlSetVal    = nDcmCtlSetVal;
                oDcmConfig.u.BusShift.dwInSyncLimit = (dwBusCycleTimeUsec*1000)/5;    /* 20 % limit in nsec for InSync monitoring */
                oDcmConfig.u.BusShift.bLogEnabled = bLogEnabled;
                oDcmConfig.u.BusShift.bCtlOff = bDcmCtlOff;
                break;
             case eDcmMode_MasterShift:
                oDcmConfig.eMode = eDcmMode_MasterShift;
                oDcmConfig.u.MasterShift.nCtlSetVal    = nDcmCtlSetVal;
                oDcmConfig.u.MasterShift.dwInSyncLimit = (dwBusCycleTimeUsec*1000)/5;    /* 20 % limit in nsec for InSync monitoring */
                oDcmConfig.u.MasterShift.bLogEnabled = bLogEnabled;
                oDcmConfig.u.MasterShift.bCtlOff = bDcmCtlOff;
                break;
            case eDcmMode_MasterRefClock:
                oDcmConfig.eMode = eDcmMode_MasterRefClock;
                oDcmConfig.u.MasterRefClock.nCtlSetVal  = nDcmCtlSetVal;
                oDcmConfig.u.MasterRefClock.bLogEnabled = bLogEnabled;
                break;
            case eDcmMode_Dcx:
                oDcmConfig.eMode = eDcmMode_Dcx;
                /* Mastershift */
                oDcmConfig.u.Dcx.MasterShift.nCtlSetVal = nDcmCtlSetVal;
                oDcmConfig.u.Dcx.MasterShift.dwInSyncLimit = (dwBusCycleTimeUsec * 1000) / 5;    /* 20 % limit in nsec for InSync monitoring */
                oDcmConfig.u.Dcx.MasterShift.bLogEnabled = bLogEnabled;
                oDcmConfig.u.Dcx.MasterShift.bCtlOff = bDcmCtlOff;
                /* Dcx Busshift */
                oDcmConfig.u.Dcx.nCtlSetVal = nDcmCtlSetVal;
                oDcmConfig.u.Dcx.dwInSyncLimit = (dwBusCycleTimeUsec * 1000) / 5;    /* 20 % limit in nsec for InSync monitoring */
                oDcmConfig.u.Dcx.bLogEnabled = bLogEnabled;
                oDcmConfig.u.Dcx.bCtlOff = bDcmCtlOff;
                oDcmConfig.u.Dcx.dwExtClockTimeout = 1000;
                oDcmConfig.u.Dcx.wExtClockFixedAddr = 0; /* 0 only when clock adjustment in external mode configured by EcEngineer */
                break;
            default:
                dwRetVal = EC_E_NOTSUPPORTED;
                LogError("DCM mode is not supported!");
                goto Exit;

            }
            dwRes = ecatDcmConfigure(&oDcmConfig, 0);
            switch (dwRes)
            {
            case EC_E_NOERROR:
                break;
            case EC_E_FEATURE_DISABLED:
                LogError("Cannot configure DCM mode!");
                LogError("Start with -dcmmode off to run the DC demo without DCM, or prepare the ENI file to support the requested DCM mode");
                LogError("In ET9000 for example, select under ""Advanced settings\\Distributed clocks"" ""DC in use"" and ""Slave Mode""");
                LogError("to support BusShift and MasterRefClock modes.");
                LogError("Please refer to the class A manual for further information");
                dwRetVal = dwRes;
                goto Exit;
            default:
                LogError("Cannot configure DCM mode! %s (Result = 0x%x)", ecatGetText(dwRes), dwRes);
                dwRetVal = dwRes;
                goto Exit;
            }
        }
    }
    /* Register client */
    {
        EC_T_REGISTERRESULTS oRegisterResults;

        OsMemset(&oRegisterResults, 0, sizeof(EC_T_REGISTERRESULTS));
        dwRes = ecatRegisterClient(ecatNotifyCallback, pNotification, &oRegisterResults);
        if (dwRes != EC_E_NOERROR)
        {
            dwRetVal = dwRes;
            LogError("Cannot register client! (Result = 0x%x)", dwRes);
            goto Exit;
        }
        S_dwClntId = oRegisterResults.dwClntId;
        pNotification->SetClientID(S_dwClntId);
    }
    
    /* Print found slaves */
    if (nVerbose >= 2)
    {
        dwRes = ecatScanBus(ETHERCAT_SCANBUS_TIMEOUT);
        switch (dwRes)
        {
        case EC_E_NOERROR:
        case EC_E_BUSCONFIG_MISMATCH:
        case EC_E_LINE_CROSSED:
            PrintSlaveInfos(INSTANCE_MASTER_DEFAULT, poLog);
            break;
        default:
            LogError("Cannot scan bus: %s (0x%lx)", ecatGetText(dwRes), dwRes);
            break;
        }
    }

    /* Print MAC address */
    if (nVerbose > 0)
    {
        ETHERNET_ADDRESS oSrcMacAddress;

        dwRes = ecatGetSrcMacAddress(&oSrcMacAddress);
        if (dwRes != EC_E_NOERROR)
        {
            LogError("Cannot get MAC address! (Result = 0x%x)", dwRes);
        }
        LogMsg("EtherCAT network adapter MAC: %02X-%02X-%02X-%02X-%02X-%02X\n",
            oSrcMacAddress.b[0], oSrcMacAddress.b[1], oSrcMacAddress.b[2], oSrcMacAddress.b[3], oSrcMacAddress.b[4], oSrcMacAddress.b[5]);
    }
    
    /* Start EtherCAT bus --> set Master state to OPERATIONAL if ENI file provided */
    /*******************************************************************************/
    if (0 != nVerbose) LogMsg( "=====================" );
    if (0 != nVerbose) LogMsg( "Start EtherCAT Master" );
    if (0 != nVerbose) LogMsg( "=====================" );

    /* set master and bus state to INIT */
    dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_INIT);
    pNotification->ProcessNotificationJobs();
    if (dwRes != EC_E_NOERROR)
    {
        LogError("Cannot start set master state to INIT (Result = %s (0x%lx))", ecatGetText(dwRes), dwRes);
        dwRetVal = dwRes;
        goto Exit;
    }

    /******************************************************/
    /* Demo code: Remove/change this in your application  */
    /******************************************************/
    dwRes = myAppPrepare(poLog, nVerbose, pvCfgFileHandle, dwBusCycleTimeUsec);
    if (EC_E_NOERROR != dwRes)
    {
        LogError((EC_T_CHAR*)"myAppPrepare failed, error code: 0x%x", dwRes);
        dwRetVal = dwRes;
        goto Exit;
    }
    /* set master and bus state to PREOP */
    dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_PREOP);
    pNotification->ProcessNotificationJobs();
    if (dwRes != EC_E_NOERROR)
    {
        LogError("Cannot start set master state to PREOP (Result = %s (0x%lx))", ecatGetText(dwRes), dwRes);
        dwRetVal = dwRes;
        goto Exit;
    }
    /* skip this step if demo started without ENI */
    if (pbyCnfData != EC_NULL)
    {
        /******************************************************/
        /* Demo code: Remove/change this in your application  */
        /******************************************************/
        dwRes = myAppSetup(poLog, nVerbose, S_dwClntId, ecatGetProcessImageInputPtr(), ecatGetProcessImageOutputPtr());
        if (EC_E_NOERROR != dwRes)
        {
            LogError((EC_T_CHAR*)"myAppSetup failed, error code: 0x%x", dwRes);
            dwRetVal = dwRes;
            goto Exit;
        }
        /* set master and bus state to SAFEOP */
        dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_SAFEOP);
        pNotification->ProcessNotificationJobs();
        if (dwRes != EC_E_NOERROR)
        {
            LogError("Cannot start set master state to SAFEOP (Result = %s (0x%lx))", ecatGetText(dwRes), dwRes);

            /* most of the time SAFEOP is not reachable due to DCM */
            if (eDcmMode_Off != eDcmMode)
            {
            EC_T_DWORD dwDcmStatus = 0;
            EC_T_INT   nDiffCur = 0, nDiffAvg = 0, nDiffMax = 0;

                dwRes = ecatDcmGetStatus(&dwDcmStatus, &nDiffCur, &nDiffAvg, &nDiffMax);
                if (dwRes == EC_E_NOERROR)
                {
                    if (dwDcmStatus != EC_E_NOERROR)
                    {
                        LogError("DCM Status: %s (0x%08X)\n", ecatGetText(dwDcmStatus), dwDcmStatus);
                    }
                }
                else
                {
                    LogError("Cannot get DCM status! %s (0x%08X)\n", ecatGetText(dwRes), dwRes);
                }
            }
            dwRetVal = dwRes;
            goto Exit;
        }
        /* set master and bus state to OP */
        dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_OP);
        pNotification->ProcessNotificationJobs();
        if (dwRes != EC_E_NOERROR)
        {
            LogError("Cannot start set master state to OP (Result = %s (0x%lx))", ecatGetText(dwRes), dwRes);
            dwRetVal = dwRes;
            goto Exit;
        }
    }
    else
    {
        if (0 != nVerbose) LogMsg("No ENI file provided. EC-Master started with generated ENI file.");
    }

    if (S_bEnaPerfJobs)
    {
        LogMsg("");
        LogMsg("Job times during startup <INIT> to <%s>:", ecatStateToStr(ecatGetMasterState()));
        PERF_MEASURE_JOBS_SHOW();       /* show job times */
        LogMsg("");
        ecatPerfMeasReset(&S_TscMeasDesc, 0xFFFFFFFF);        /* clear job times of startup phase */
    }

#if (defined DEBUG) && (defined XENOMAI)
    /* Enabling mode switch warnings for shadowed task */
    dwRes = rt_task_set_mode(0, T_WARNSW, NULL);
    if (0 != dwRes)
    {
        OsDbgMsg("EnableRealtimeEnvironment: rt_task_set_mode returned an error\n");
        OsDbgAssert(EC_FALSE);
    }
#endif

    /* run the demo */
    if (dwDuration != 0)
    {
        oTimeout.Start(dwDuration);
    }
    while (bRun && (!oTimeout.IsStarted() || !oTimeout.IsElapsed()))
    {
        if (nVerbose >= 2)
        {
            PERF_MEASURE_JOBS_SHOW();       /* show job times */
        }
        bRun = !OsTerminateAppRequest();/* check if demo shall terminate */

#ifdef MOVE_TEST_SEQUENCE
        /*****************************************************************************************************/
        /* Demo code: Remove/change this in your application:  Do movements according to application purposes*/
        /*****************************************************************************************************/
        myAppMoveTest(poLog, nVerbose);
#endif
        /*****************************************************************************************/
        /* Demo code: Remove/change this in your application: Do some diagnosis outside job task */
        /*****************************************************************************************/
        myAppDiagnosis(poLog, nVerbose);

        /* check DCM status if master is started with ENI */
        if (pbyCnfData != EC_NULL)
        {
            if (eDcmMode_Off != eDcmMode)
            {
            EC_T_DWORD dwDcmStatus = 0;
            EC_T_INT   nDiffCur = 0, nDiffAvg = 0, nDiffMax = 0;
            EC_T_BOOL  bLogDcmStatus = EC_FALSE;
            EC_T_BOOL  bResetDcmStatus = bFirstDcmStatus;

                if (!oDcmStatusTimer.IsStarted() || oDcmStatusTimer.IsElapsed())
                {
                    oDcmStatusTimer.Start(5000);
                    if (nVerbose >= 3)
                    {
                        bLogDcmStatus = EC_TRUE;
                    }
                }
                dwRes = ecatDcmGetStatus(&dwDcmStatus, &nDiffCur, &nDiffAvg, &nDiffMax);
                if (dwRes == EC_E_NOERROR)
                {
                    if (bFirstDcmStatus)
                    {
                        LogMsg("DCM during startup (<INIT> to <%s>)", ecatStateToStr(ecatGetMasterState()));
                    }
                    if ((dwDcmStatus != EC_E_NOTREADY) && (dwDcmStatus != EC_E_BUSY) && (dwDcmStatus != EC_E_NOERROR))
                    {
                        LogMsg("DCM Status: %s (0x%08X)\n", ecatGetText(dwDcmStatus), dwDcmStatus);
                    }
                    if (bLogDcmStatus)
                    {
                        LogMsg("DCM Diff(ns): Cur=%7d, Avg=%7d, Max=%7d", nDiffCur, nDiffAvg, nDiffMax);
                    }
                }
                else
                {
                    if ((eEcatState_OP == ecatGetMasterState()) || (eEcatState_SAFEOP == ecatGetMasterState()))
                    {
                        LogError("Cannot get DCM status! %s (0x%08X)\n", ecatGetText(dwRes), dwRes);
                    }
                    bResetDcmStatus = EC_FALSE;
                }
                if (eDcmMode_Dcx == eDcmMode)
                {
                EC_T_INT64 qwTimeStampDiff = 0;

                    dwRes = ecatDcxGetStatus(&dwDcmStatus, &nDiffCur, &nDiffAvg, &nDiffMax, &qwTimeStampDiff);
                    if (EC_E_NOERROR == dwRes)
                    {
                        if (bFirstDcmStatus)
                        {
                            LogMsg("DCX during startup (<INIT> to <%s>)", ecatStateToStr(ecatGetMasterState()));
                        }
                        if ((dwDcmStatus != EC_E_NOTREADY) && (dwDcmStatus != EC_E_BUSY) && (dwDcmStatus != EC_E_NOERROR))
                        {
                            LogMsg("DCX Status: %s (0x%08X)\n", ecatGetText(dwDcmStatus), dwDcmStatus);
                        }
                        if (bLogDcmStatus)
                        {
                            LogMsg("DCX Diff(ns): Cur=%7d, Avg=%7d, Max=%7d, TimeStamp=%7d", nDiffCur, nDiffAvg, nDiffMax, qwTimeStampDiff);
                        }
                    }
                    else
                    {
                        if ((eEcatState_OP == ecatGetMasterState()) || (eEcatState_SAFEOP == ecatGetMasterState()))
                        {
                            LogError("Cannot get DCX status! %s (0x%08X)\n", ecatGetText(dwRes), dwRes);
                        }
                        bResetDcmStatus = EC_FALSE;
                    }
                }
                if (bResetDcmStatus)
                {
#if (defined ATECAT_VERSION) && (ATECAT_VERSION >= 0x02040106)
                    ecatDcmResetStatus();
#endif
                    bFirstDcmStatus = EC_FALSE;
                }
            }
        }
        /* reset pending drive errors */
        ResetDriveErrors();

        /* process notification jobs */
        pNotification->ProcessNotificationJobs();

        OsSleep(5);
    }

    if (S_bEnaPerfJobs)
    {
        LogMsg("");
        LogMsg("Job times before shutdown");
        PERF_MEASURE_JOBS_SHOW();       /* show job times */
    }

Exit:
    if (0 != nVerbose) LogMsg( "========================" );
    if (0 != nVerbose) LogMsg( "Shutdown EtherCAT Master" );
    if (0 != nVerbose) LogMsg( "========================" );

    /* Stop EtherCAT bus --> Set Master state to INIT */
    dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_INIT);
    if (EC_E_NOERROR != dwRes)
    {
        LogError("Cannot stop EtherCAT-Master! %s (0x%lx)", ecatGetText(dwRes), dwRes);
    }
    /* Unregister client */
    if (S_dwClntId != 0)
    {
        dwRes = ecatUnregisterClient(S_dwClntId); 
        if (EC_E_NOERROR != dwRes)
        {
            LogError("Cannot unregister client! %s (0x%lx)", ecatGetText(dwRes), dwRes);
        }
        S_dwClntId = 0;
    }

#if (defined DEBUG) && (defined XENOMAI)
    /* Disable PRIMARY to SECONDARY MODE switch warning */
    dwRes = rt_task_set_mode(T_WARNSW, 0, NULL);
    if (dwRes != 0)
    {
        OsDbgMsg("OsCreateThread: rt_task_set_mode returned error %d\n", dwRes);
        OsDbgAssert(EC_FALSE);
    }
#endif /* XENOMAI */

    /* Shutdown tEcJobTask */
    S_DemoThreadParam.bJobThreadShutdown = EC_TRUE;
    oTimeout.Start(2000);
    while (S_DemoThreadParam.bJobThreadRunning && !oTimeout.IsElapsed())
    {
        OsSleep(10);
    }
    if (S_pvtJobThread != EC_NULL)
    {
        OsDeleteThreadHandle(S_pvtJobThread);
        S_pvtJobThread = EC_NULL;
    }

#ifdef ATEMRAS_SERVER
    /* Stop RAS server */
    if (S_bRasSrvStarted)
    {
        LogMsg("Stop Remote Api Server\n");
        
        if (EC_E_NOERROR != emRasSrvStop(S_pvRemoteApiSrvH, 2000))
        {
            LogError("Remote API Server shutdown failed\n");
        }
    }
#endif

    /* Deinitialize master */
    dwRes = ecatDeinitMaster();
    if (EC_E_NOERROR != dwRes)
    {
        LogError("Cannot de-initialize EtherCAT-Master! %s (0x%lx)", ecatGetText(dwRes), dwRes);
    }

    if (S_bEnaPerfJobs)
    {
        PERF_MEASURE_JOBS_DEINIT();
    }
    /* delete notification context */
    SafeDelete(pNotification);

    SafeDelete(S_pPendMotionCmdFifo);

    // Free MCFB's
    int i;
    for (i = 0; i < S_numAxes; ++i) 
    {
       EC_T_DEMO_AXIS *pDemoAxis = &S_aAxisList[i];
       SafeDelete(pDemoAxis->pFb);
    }
    return dwRetVal;
}


/********************************************************************************/
/** \brief  Trigger jobs to drive master, and update process data.
*
* \return N/A
*/
static EC_T_VOID tEcJobTask(EC_T_VOID* pvThreadParamDesc)
{
    EC_T_DWORD           dwRes             = EC_E_ERROR;
    T_DEMO_THREAD_PARAM* pDemoThreadParam  = (T_DEMO_THREAD_PARAM*)pvThreadParamDesc;
    EC_T_CPUSET          CpuSet;
    EC_T_BOOL            bPrevCycProcessed = EC_FALSE;
    EC_T_INT             nOverloadCounter  = 0;               /* counter to check if cycle time is to short */
    EC_T_BOOL            bOk;
    EC_T_DWORD           dwInstanceID = pDemoThreadParam->dwMasterID;

    EC_CPUSET_ZERO(CpuSet);
    EC_CPUSET_SET(CpuSet, pDemoThreadParam->dwCpuIndex);
    bOk = OsSetThreadAffinity(EC_NULL, CpuSet);
    if (!bOk)
    {
        LogError("Error: Set job task affinitiy, invalid CPU index %d\n", pDemoThreadParam->dwCpuIndex);
        goto Exit;
    }

    /* Start periodic task right before where it is used,
     * otherwise, OsSleepTillTick() might fail if this thread is suspended before being able to call OsMakeThreadPeriodic() */
#if (defined XENOMAI)
    OsMakeThreadPeriodic(rt_task_self(), pDemoThreadParam->dwBusCycleTimeUsec);
#endif

    /* demo loop */
    pDemoThreadParam->bJobThreadRunning = EC_TRUE;
    do
    {
        /* wait for next cycle (event from scheduler task) */
#if (defined RTAI) || (defined XENOMAI)
        OsSleepTillTick(); /* period is set after creating jobtask */
#else
        OsWaitForEvent(pDemoThreadParam->pvTimingEvent, EC_WAITINFINITE);
#endif /* !RTAI && !XENOMAI*/

        PERF_JOB_END(PERF_CycleTime);
        PERF_JOB_START(PERF_CycleTime);

        /* process all received frames (read new input values) */
        PERF_JOB_START(JOB_ProcessAllRxFrames);
        dwRes = emExecJob(dwInstanceID, eUsrJob_ProcessAllRxFrames, &bPrevCycProcessed);
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes && EC_E_LINK_DISCONNECTED != dwRes)
        {
            LogError("ERROR: emExecJob( eUsrJob_ProcessAllRxFrames): %s (0x%lx)", ecatGetText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_ProcessAllRxFrames);

        if (EC_E_NOERROR == dwRes)
        {
            if (!bPrevCycProcessed)
            {
                /* it is not reasonable, that more than 5 continuous frames are lost */
                nOverloadCounter += 10;
                if (nOverloadCounter >= 50)
                {
                    LogError( "Error: System overload: Cycle time too short or huge jitter!" );
                }
                else
                {
                    LogError( "eUsrJob_ProcessAllRxFrames - not all previously sent frames are received/processed (frame loss)!" );
                }
            }
            else
            {
                /* everything o.k.? If yes, decrement overload counter */
                if (nOverloadCounter > 0)    nOverloadCounter--;
            }
        }
        /* handle DCM logging */
#ifdef DCM_ENABLE_LOGFILE
        PERF_JOB_START(PERF_DCM_Logfile);
        {
            EC_T_CHAR* pszLog = EC_NULL;

            emDcmGetLog(dwInstanceID, &pszLog);
            if ((EC_NULL != pszLog))
            {
                LogDcm(pszLog);
            }
        }
        PERF_JOB_END(PERF_DCM_Logfile);
#endif

        /*****************************************************/
        /* Demo code: Remove/change this in your application: Working process data cyclic call */
        /*****************************************************/
        PERF_JOB_START(PERF_myAppWorkpd);
        myAppWorkpd(pDemoThreadParam->pLogInst, pDemoThreadParam->pNotInst->Verbose(),
            emGetProcessImageInputPtr(dwInstanceID), emGetProcessImageOutputPtr(dwInstanceID), 
            dwInstanceID, emGetMasterState(dwInstanceID), pDemoThreadParam->dwBusCycleTimeUsec );
        PERF_JOB_END(PERF_myAppWorkpd);

        /* write output values of current cycle, by sending all cyclic frames */
        PERF_JOB_START(JOB_SendAllCycFrames);
        dwRes = emExecJob( dwInstanceID, eUsrJob_SendAllCycFrames, EC_NULL );
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes && EC_E_LINK_DISCONNECTED != dwRes)
        {
            LogError("emExecJob( eUsrJob_SendAllCycFrames,    EC_NULL ): %s (0x%lx)", ecatGetText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_SendAllCycFrames);

        /* remove this code when using licensed version */
        if (EC_E_EVAL_EXPIRED == dwRes )
        {
            bRun = EC_FALSE;        /* set shutdown flag */
        }

        /* Execute some administrative jobs. No bus traffic is performed by this function */
        PERF_JOB_START(JOB_MasterTimer);
        dwRes = emExecJob( dwInstanceID, eUsrJob_MasterTimer,           EC_NULL );
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes)
        {
            LogError("emExecJob( eUsrJob_MasterTimer,    EC_NULL ): %s (0x%lx)", ecatGetText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_MasterTimer);

        /* send queued acyclic EtherCAT frames */
        PERF_JOB_START(JOB_SendAcycFrames);
        dwRes = emExecJob( dwInstanceID, eUsrJob_SendAcycFrames,        EC_NULL );
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes && EC_E_LINK_DISCONNECTED != dwRes)
        {
            LogError("emExecJob( eUsrJob_SendAcycFrames,    EC_NULL ): %s (0x%lx)", ecatGetText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_SendAcycFrames);

        S_dwJobTaskCycleCnt++;

#if !(defined NO_OS)
    } while (!pDemoThreadParam->bJobThreadShutdown);

    PERF_MEASURE_JOBS_SHOW();

    pDemoThreadParam->bJobThreadRunning = EC_FALSE;
#else
    /* in case of NO_OS the job task function is called cyclically within the timer ISR */
    } while (EC_FALSE);
    pDemoThreadParam->bJobThreadRunning = !pDemoThreadParam->bJobThreadShutdown;
#endif

Exit:
#if (defined EC_VERSION_RTEMS)
    rtems_task_delete(RTEMS_SELF);
#endif
    return;
}

/********************************************************************************/
/** \brief  Handler for master notifications
*
* \return  Status value.
*/
static EC_T_DWORD ecatNotifyCallback(
    EC_T_DWORD         dwCode,  /**< [in]   Notification code */
    EC_T_NOTIFYPARMS*  pParms   /**< [in]   Notification parameters */
                                         )
{
    EC_T_DWORD         dwRetVal                = EC_E_NOERROR;
    CEmNotification*   pNotifyInstance;

    if ((EC_NULL == pParms)||(EC_NULL==pParms->pCallerData))
    {
        dwRetVal = EC_E_INVALIDPARM;
        goto Exit;
    }

    /* notification for application ? */
    if ((dwCode >= EC_NOTIFY_APP) && (dwCode <= EC_NOTIFY_APP+EC_NOTIFY_APP_MAX_CODE))
    {
        /*****************************************************/
        /* Demo code: Remove/change this in your application */
        /* to get here the API ecatNotifyApp(dwCode, pParms) has to be called */
        /*****************************************************/
        dwRetVal = myAppNotify(dwCode-EC_NOTIFY_APP, pParms);
    }
    else
    {
        pNotifyInstance = (CEmNotification*)pParms->pCallerData;

        /* call the default handler */
        dwRetVal = pNotifyInstance->ecatNotify(dwCode, pParms);
    }

Exit:
    return dwRetVal;
}


/********************************************************************************/
/** \brief  Handler for master RAS notifications
*
*
* \return  Status value.
*/
#ifdef ATEMRAS_SERVER 
static EC_T_DWORD RasNotifyWrapper(
                            EC_T_DWORD         dwCode, 
                            EC_T_NOTIFYPARMS*  pParms
                            )
{
    EC_T_DWORD                      dwRetVal                = EC_E_NOERROR;
    CEmNotification*                pNotInst                = EC_NULL;
    
    if ((EC_NULL == pParms)||(EC_NULL==pParms->pCallerData))
    {
        dwRetVal = EC_E_INVALIDPARM;
        goto Exit;
    }
    
    pNotInst = (CEmNotification*)(pParms->pCallerData);
    dwRetVal = pNotInst->emRasNotify(dwCode, pParms);
Exit:
    
    return dwRetVal;
}
#endif

/*-MYAPP---------------------------------------------------------------------*/

/***************************************************************************************************/
/**
\brief  Initialize Application

\return EC_E_NOERROR on success, error code otherwise.
*/
static EC_T_DWORD myAppInit(
    CAtEmLogging*       poLog,          /* [in]  Logging instance */ 
    EC_T_INT            nVerbose        /* [in]  Verbosity level */
    )
{
    EC_UNREFPARM(poLog);
    EC_UNREFPARM(nVerbose);

    OsMemset(S_aAxisList, 0, DEMO_MAX_NUM_OF_AXIS*sizeof(EC_T_DEMO_AXIS));

    return EC_E_NOERROR;
}

static void ParseIdx(EC_T_VOID *cfgFileHandle, EC_T_CHAR *paraString, EC_T_DWORD &pdoIdx, EC_T_DWORD &varIdx)
{
   EC_T_BYTE buffy[AT_XMLBUF_SIZE];
   if (atParamRead(cfgFileHandle, eDataType_STRING, paraString, buffy, AT_XMLBUF_SIZE) != EC_E_NOERROR) return;
   buffy[AT_XMLBUF_SIZE - 1] = '\0';

   char *idx1Str = OsStrtok(buffy, ":");
   char *idx2Str = OsStrtok(EC_NULL, ":");

   EC_T_DWORD idx1 = 0, idx2 = 0;
   if (idx1Str != EC_NULL) idx1 = (EC_T_DWORD) OsStrtoul(idx1Str, EC_NULL, 0);
   if (idx2Str != EC_NULL) idx2 = (EC_T_DWORD) OsStrtoul(idx2Str, EC_NULL, 0);

   // For compatibility. If there is no seperator, the first idx is assumed as var idx.
   if (!idx2) varIdx = idx1;
   else pdoIdx = idx1, varIdx = idx2;
}

/***************************************************************************************************/
/**
\brief  Initialize Slave Instance.

Find slave parameters.
\return EC_E_NOERROR on success, error code otherwise.
*/
static EC_T_DWORD myAppPrepare( 
    CAtEmLogging*       poLog,          /* [in]  Logging instance */ 
    EC_T_INT            nVerbose,       /* [in]  Verbosity level */
    EC_T_VOID*          pvCfgFileHandle, /* [in] handle to DemoConfig.xml file */
    EC_T_DWORD dwBusCycleTimeUsec
    )
{
    EC_T_DWORD          dwValue    = 0;

    EC_T_CHAR           szParaString[160];

    EC_UNREFPARM(poLog);
    EC_UNREFPARM(nVerbose);

    S_numAxes = 0;
    int i;
    for (i = 0; i < DEMO_MAX_NUM_OF_AXIS; i++) 
    {
        /* read fixed address */
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_ADDR, i+1);
        if (atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&dwValue, sizeof(dwValue)) != EC_E_NOERROR 
           || !dwValue)
        {
            continue;            /* axis not available */
        }

        EC_T_DEMO_AXIS *pDemoAxis = &S_aAxisList[S_numAxes++];

        pDemoAxis->wDriveFixAddress = (EC_T_WORD)dwValue;

        /* Drive profile */
        char buffy[AT_XMLBUF_SIZE] = { 0 };
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_PROFILE, i+1);
        atParamRead(pvCfgFileHandle, eDataType_STRING, szParaString, (EC_T_BYTE *)buffy, sizeof(buffy));
        buffy[AT_XMLBUF_SIZE - 1] = '\0';

        pDemoAxis->eDriveProfile = MC_T_AXIS_PROFILE_DS402; // Default
        if (OsStrncmp(buffy, "DS402", AT_XMLBUF_SIZE) == 0) pDemoAxis->eDriveProfile = MC_T_AXIS_PROFILE_DS402;
        else if (OsStrncmp(buffy, "SERCOS", AT_XMLBUF_SIZE) == 0) pDemoAxis->eDriveProfile = MC_T_AXIS_PROFILE_SERCOS;

        /* Modes of operation */
        dwValue = (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402) ? DRV_MODE_OP_CSP : SER_OPMODE_POS_FB1;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_MODE, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&dwValue, sizeof(dwValue));
        pDemoAxis->byDriveModesOfOperation = (EC_T_BYTE) dwValue;

        /* Adressed drive number within SERCOS servo controller */
        dwValue = 0;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_NO, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&dwValue, sizeof(dwValue));
        pDemoAxis->bySercosDriveNo = (EC_T_BYTE) dwValue;

        /* CoE/DS402 operation mode object idx */
        pDemoAxis->dwCoeIdxOpMode = DRV_OBJ_MODES_OF_OPERATION;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_OPMODE, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&pDemoAxis->dwCoeIdxOpMode, sizeof(pDemoAxis->dwCoeIdxOpMode));

        /* get values for move: jerk, acc, dec, vel and distance */
        pDemoAxis->dwDriveJerk = DEFAULT_JERK;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_JERK, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&pDemoAxis->dwDriveJerk, sizeof(pDemoAxis->dwDriveJerk));

        pDemoAxis->dwDriveAcc = DEFAULT_ACC;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_ACC, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&pDemoAxis->dwDriveAcc, sizeof(pDemoAxis->dwDriveAcc));

        pDemoAxis->dwDriveDec = DEFAULT_DEC;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_DEC, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&pDemoAxis->dwDriveDec, sizeof(pDemoAxis->dwDriveDec));

        pDemoAxis->dwDriveVel = DEFAULT_MAX_VELOCITY;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_VEL, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&pDemoAxis->dwDriveVel, sizeof(pDemoAxis->dwDriveVel));

        pDemoAxis->dwDriveDistance = DEFAULT_DISTANCE;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_DIST, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&pDemoAxis->dwDriveDistance, sizeof(pDemoAxis->dwDriveDistance));

        pDemoAxis->dwPosWindow = DEFAULT_POS_WINDOW;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_POS_WIND, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&pDemoAxis->dwPosWindow, sizeof(pDemoAxis->dwPosWindow));

        pDemoAxis->dwDriveIncPerMM = DEFAULT_INCPERMM;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_INCPERMM, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&pDemoAxis->dwDriveIncPerMM, sizeof(pDemoAxis->dwDriveIncPerMM));

        pDemoAxis->dwDriveIncFactor = 0; // x << 0 -> Not scaled
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_INCFACTOR, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&pDemoAxis->dwDriveIncFactor, sizeof(pDemoAxis->dwDriveIncFactor));

        pDemoAxis->dwVelocityGain = DEFAULT_VEL_GAIN;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_VEL_GAIN, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&pDemoAxis->dwVelocityGain, sizeof(pDemoAxis->dwVelocityGain));

        pDemoAxis->dwTorqueGain = DEFAULT_TORQUE_GAIN;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_TOR_GAIN, i+1);
        atParamRead(pvCfgFileHandle, eDataType_DWORD, szParaString, (EC_T_BYTE*)&pDemoAxis->dwTorqueGain, sizeof(pDemoAxis->dwTorqueGain));

        pDemoAxis->IdxStatusword.dwVarIdx = (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402) ? DRV_OBJ_STATUS_WORD : DRV_IDN_STATUS_WORD;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_STATUS, i+1);
        ParseIdx(pvCfgFileHandle, szParaString, pDemoAxis->IdxStatusword.dwPdoIdx, pDemoAxis->IdxStatusword.dwVarIdx);

        pDemoAxis->IdxControlword.dwVarIdx = (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402) ? DRV_OBJ_CONTROL_WORD : DRV_IDN_CONTROL_WORD;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_CONTROL, i+1);
        ParseIdx(pvCfgFileHandle, szParaString, pDemoAxis->IdxControlword.dwPdoIdx, pDemoAxis->IdxControlword.dwVarIdx);

        pDemoAxis->IdxActualPos.dwVarIdx = (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402) ? DRV_OBJ_POSITION_ACTUAL_VALUE : DRV_IDN_POSITION_ACTUAL_VALUE;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_POSACT, i+1);
        ParseIdx(pvCfgFileHandle, szParaString, pDemoAxis->IdxActualPos.dwPdoIdx, pDemoAxis->IdxActualPos.dwVarIdx);

        pDemoAxis->IdxModeOfOperation.dwVarIdx = (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402) ? DRV_OBJ_MODES_OF_OPERATION : DRV_IDN_OPMODE;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_MODE, i+1);
        ParseIdx(pvCfgFileHandle, szParaString, pDemoAxis->IdxModeOfOperation.dwPdoIdx, pDemoAxis->IdxModeOfOperation.dwVarIdx);

        pDemoAxis->IdxModeOfOperationDisplay.dwVarIdx = DRV_OBJ_MODES_OF_OPERATION_DISPLAY;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_MODE_DISPLAY, i + 1);
        ParseIdx(pvCfgFileHandle, szParaString, pDemoAxis->IdxModeOfOperationDisplay.dwPdoIdx, pDemoAxis->IdxModeOfOperationDisplay.dwVarIdx);

        /* CSP */
        pDemoAxis->IdxTargetPos.dwVarIdx = (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402) ? DRV_OBJ_TARGET_POSITION : DRV_IDN_TARGET_POSITION;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_TARPOS, i+1);
        ParseIdx(pvCfgFileHandle, szParaString, pDemoAxis->IdxTargetPos.dwPdoIdx, pDemoAxis->IdxTargetPos.dwVarIdx);

        /* Velocity Offset Feed Forward (only DS402) */
        pDemoAxis->IdxVelOffset.dwVarIdx = DRV_OBJ_VELOCITY_OFFSET;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_VELOFF, i+1);
        ParseIdx(pvCfgFileHandle, szParaString, pDemoAxis->IdxVelOffset.dwPdoIdx, pDemoAxis->IdxVelOffset.dwVarIdx);

        /* Torque Offset Feed Forward (only DS402) */
        pDemoAxis->IdxTorOffset.dwVarIdx = DRV_OBJ_TORQUE_OFFSET;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_TOROFF, i+1);
        ParseIdx(pvCfgFileHandle, szParaString, pDemoAxis->IdxTorOffset.dwPdoIdx, pDemoAxis->IdxTorOffset.dwVarIdx);

        /* CSV */
        pDemoAxis->IdxTargetVel.dwVarIdx = (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402) ? DRV_OBJ_TARGET_VELOCITY : DRV_IDN_TARGET_VELOCITY;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_TARVEL, i+1);
        ParseIdx(pvCfgFileHandle, szParaString, pDemoAxis->IdxTargetVel.dwPdoIdx, pDemoAxis->IdxTargetVel.dwVarIdx);

        /* CST */
        pDemoAxis->IdxTargetTrq.dwVarIdx = (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402) ? DRV_OBJ_TARGET_TORQUE : DRV_IDN_TARGET_TORQUE;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_TARTRQ, i+1);
        ParseIdx(pvCfgFileHandle, szParaString, pDemoAxis->IdxTargetTrq.dwPdoIdx, pDemoAxis->IdxTargetTrq.dwVarIdx);

        pDemoAxis->IdxActualTrq.dwVarIdx = (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402) ? DRV_OBJ_ACTUAL_TORQUE : DRV_IDN_ACTUAL_TORQUE;
        EcSnprintf(szParaString, sizeof(szParaString), MOTIONDEMO_CFG_TAG_DRIVE_IDX_ACTTRQ, i+1);
        ParseIdx(pvCfgFileHandle, szParaString, pDemoAxis->IdxActualTrq.dwPdoIdx, pDemoAxis->IdxActualTrq.dwVarIdx);

        /*
         * Initialize EC-Motion API
         */

        // Create Motion Control Function Blocks
        pDemoAxis->pFb = new EC_T_FUNCTION_BLOCKS(); // Allocate on heap, because the demo struct is memset to zero! 

        MC_T_AXIS_INIT &axisInitData = pDemoAxis->pFb->AxisInitData;
        axisInitData.AxisType = MC_AXIS_TYPE_REAL_ALL;
        // axisInitData.eAxisType = MC_AXIS_TYPE_VIRTUAL;
        axisInitData.CycleTime = dwBusCycleTimeUsec;
        axisInitData.IncPerMM =  pDemoAxis->dwDriveIncPerMM;
        axisInitData.IncFactor = pDemoAxis->dwDriveIncFactor;
        axisInitData.Verbose = nVerbose;
        axisInitData.VelocityGain = pDemoAxis->dwVelocityGain;
        axisInitData.TorqueGain   = pDemoAxis->dwTorqueGain;
        
        MC_T_AXIS_INIT_PERFMEAS axisPerfInitData = { 0 };
        axisPerfInitData.pPerfMeasStart = ecatPerfMeasStart;
        axisPerfInitData.pPerfMeasEnd = ecatPerfMeasEnd;
        axisPerfInitData.pTscMeasDesc = &S_TscMeasDesc;

        // Initialize axis
        pDemoAxis->pFb->Axis.Init(axisInitData);
        pDemoAxis->pFb->Axis.InitPerfMeas(axisPerfInitData);

        // Set mandatory IN/OUT variable "Axis" (reference to the axis).
        MC_T_AXIS_REF *pAxis = &pDemoAxis->pFb->Axis;
        pDemoAxis->pFb->Power.Axis = pAxis;
#if defined(EC_MOTION_TRAJECTORY_GEN)
        pDemoAxis->pFb->Stop.Axis = pAxis;
        pDemoAxis->pFb->Halt.Axis = pAxis;
#endif /* defined(EC_MOTION_TRAJECTORY_GEN) */
        pDemoAxis->pFb->Reset.Axis = pAxis;
#if defined(EC_MOTION_TRAJECTORY_GEN)
        pDemoAxis->pFb->MoveRelative.Axis = pAxis;
        pDemoAxis->pFb->MoveAbsolute.Axis = pAxis;
        pDemoAxis->pFb->MoveVelocity.Axis = pAxis;
#endif /* defined(EC_MOTION_TRAJECTORY_GEN) */
        pDemoAxis->pFb->ReadActualPosition.Axis = pAxis;
        pDemoAxis->pFb->ReadParameter.Axis = pAxis;
        pDemoAxis->pFb->ReadBoolParameter.Axis = pAxis;
        pDemoAxis->pFb->WriteParameter.Axis = pAxis;
        pDemoAxis->pFb->WriteBoolParameter.Axis = pAxis;
#if defined(EC_MOTION_TRAJECTORY_GEN)
        pDemoAxis->pFb->ReadMotionState.Axis = pAxis;
#endif /* defined(EC_MOTION_TRAJECTORY_GEN) */
        pDemoAxis->pFb->ReadAxisInfo.Axis = pAxis;
        pDemoAxis->pFb->CheckTargetposReached.Axis = pAxis;
        pDemoAxis->pFb->HaltRecovery.Axis = pAxis;
#ifdef EC_MOTION_SUPPORT_PP_MODE
        pDemoAxis->pFb->Home.Axis = pAxis;
#endif
        pDemoAxis->pFb->ReadDigitalInput.Axis = pAxis;
        pDemoAxis->pFb->ReadDigitalOutput.Axis = pAxis;
        pDemoAxis->pFb->WriteDigitalOutput.Axis = pAxis;
    }

    return EC_E_NOERROR;
}

/***************************************************************************************************/
/**
\brief  Setup slave parameters (normally done in PREOP state

  - SDO up- and Downloads
  - Read Object Dictionary

\return EC_E_NOERROR on success, error code otherwise.
*/
static EC_T_DWORD myAppSetup(
    CAtEmLogging*      poLog,           /* [in]  Logging instance */     
    EC_T_INT           nVerbose,        /* [in]  verbosity level */
    EC_T_DWORD         dwClntId,        /* [in]  EtherCAT master client id */
    EC_T_BYTE*         pbyPDIn,         /* [in]  pointer to process data input buffer */
    EC_T_BYTE*         pbyPDOut         /* [in]  pointer to process data output buffer */
    )
{
   EC_T_DWORD           dwRetVal = EC_E_NOERROR;
   EC_T_DWORD           dwInstanceID = 0;
   EC_T_CFG_SLAVE_INFO  oSlaveInfo;

   EC_T_WORD               wNumOfVarsToReadInp          = 0;
   EC_T_WORD               wNumOfVarsToReadOutp         = 0;
   EC_T_WORD               wReadEntries                 = 0;
   EC_T_DWORD              dwProcessVarIdx              = 0;
   EC_T_PROCESS_VAR_INFO_EX* pProcessVarInfo  = EC_NULL;

   EC_UNREFPARM(poLog);
   EC_UNREFPARM(dwClntId);
   EC_UNREFPARM(nVerbose);

   int i;
   for (i = 0; i < S_numAxes; ++i) 
   {
      EC_T_DEMO_AXIS *pDemoAxis = &S_aAxisList[i];

      /* get slave */
      dwRetVal = emGetCfgSlaveInfo(dwInstanceID, EC_TRUE, pDemoAxis->wDriveFixAddress, &oSlaveInfo);
      if (EC_E_NOERROR != dwRetVal)
      {
         LogError("ERROR: emGetCfgSlaveInfo() (Result = %s 0x%x)", ecatGetText(dwRetVal), dwRetVal );
         continue;
      }

      pDemoAxis->dwSlaveId = oSlaveInfo.dwSlaveId;

      /* Get the number of INPUT process variable information entries of the current slave */
      dwRetVal = emGetSlaveInpVarInfoNumOf(dwInstanceID, EC_TRUE, pDemoAxis->wDriveFixAddress, &wNumOfVarsToReadInp );
      if (wNumOfVarsToReadInp > 0)
      {
         /* Allocate  memory for the INPUT process variable information entries of the current slave */
         pProcessVarInfo = (EC_T_PROCESS_VAR_INFO_EX*)OsMalloc(sizeof(EC_T_PROCESS_VAR_INFO_EX)* wNumOfVarsToReadInp);
         if (pProcessVarInfo == EC_NULL)
         {
            dwRetVal = EC_E_NOMEMORY;
         }
         else
         {
            /* Get all INPUT process variable information entries of the current slave */
            dwRetVal = emGetSlaveInpVarInfoEx(dwInstanceID, EC_TRUE, pDemoAxis->wDriveFixAddress, wNumOfVarsToReadInp, pProcessVarInfo, &wReadEntries);
            if (EC_E_NOERROR != dwRetVal)
            {
               LogError( (EC_T_CHAR*)"ERROR: emGetSlaveInpVarInfoEx() (Result = %s 0x%x)", ecatGetText(dwRetVal), dwRetVal );
            }

            /* search input variables */
            for (dwProcessVarIdx = 0; (dwProcessVarIdx < wReadEntries) && (dwRetVal == EC_E_NOERROR) ; dwProcessVarIdx++)
            {   
               if ((pDemoAxis->IdxStatusword.dwPdoIdx == 0 || pProcessVarInfo[dwProcessVarIdx].wPdoIndex == pDemoAxis->IdxStatusword.dwPdoIdx)
                  && pProcessVarInfo[dwProcessVarIdx].wIndex == pDemoAxis->IdxStatusword.dwVarIdx) 
               {
                  pDemoAxis->pwPdStatusWord = (EC_T_WORD*)&(pbyPDIn[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }
               else if ((pDemoAxis->IdxActualPos.dwPdoIdx == 0 || pProcessVarInfo[dwProcessVarIdx].wPdoIndex == pDemoAxis->IdxActualPos.dwPdoIdx)
                  && pProcessVarInfo[dwProcessVarIdx].wIndex == pDemoAxis->IdxActualPos.dwVarIdx) 
               {
                  pDemoAxis->plPdActualPosition = (EC_T_SDWORD*)&(pbyPDIn[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }
               else if ((pDemoAxis->IdxActualTrq.dwPdoIdx == 0 || pProcessVarInfo[dwProcessVarIdx].wPdoIndex == pDemoAxis->IdxActualTrq.dwPdoIdx)
                  && pProcessVarInfo[dwProcessVarIdx].wIndex == pDemoAxis->IdxActualTrq.dwVarIdx) 
               {
                  pDemoAxis->psPdActualTorque = (EC_T_SWORD*)&(pbyPDIn[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }
               else if ((pDemoAxis->IdxModeOfOperationDisplay.dwPdoIdx == 0 || pProcessVarInfo[dwProcessVarIdx].wPdoIndex == pDemoAxis->IdxModeOfOperationDisplay.dwPdoIdx)
                   && pProcessVarInfo[dwProcessVarIdx].wIndex == pDemoAxis->IdxModeOfOperationDisplay.dwVarIdx)
               {
                   pDemoAxis->pbyPdModeOfOperationDisplay = (EC_T_BYTE*)&(pbyPDIn[pProcessVarInfo[dwProcessVarIdx].nBitOffs / 8]);
               }
               else if (pProcessVarInfo[dwProcessVarIdx].wIndex == DRV_OBJ_DIGITAL_INPUTS)
               {
                   pDemoAxis->pdwDigInputs = (EC_T_DWORD*)&(pbyPDIn[pProcessVarInfo[dwProcessVarIdx].nBitOffs / 8]);
               }
            }

            OsFree(pProcessVarInfo);
            pProcessVarInfo = EC_NULL;
         }
      }

      if (pDemoAxis->pwPdStatusWord == EC_NULL)
      {
          LogError((EC_T_CHAR*)"ERROR: Invalid PDO mapping Axis %d: Status Word Object=0x%x not found", pDemoAxis->wDriveFixAddress, pDemoAxis->IdxStatusword.dwVarIdx);
          dwRetVal = EC_E_INVALIDPARM;
      }

      if (pDemoAxis->plPdActualPosition == EC_NULL)
      {
          LogError((EC_T_CHAR*)"ERROR: Invalid PDO mapping Axis %d: Actual Position Object=0x%x not found", pDemoAxis->wDriveFixAddress, pDemoAxis->IdxActualPos.dwVarIdx);
          dwRetVal = EC_E_INVALIDPARM;
      }

#ifdef EC_MOTION_SUPPORT_PP_MODE
      // Only in PP mode, will be used in homing
      if (pDemoAxis->byDriveModesOfOperation == DRV_MODE_OP_PROF_POS)
      {
          if (pDemoAxis->pbyPdModeOfOperationDisplay == EC_NULL)
          {
              LogError((EC_T_CHAR*)"ERROR: Invalid PDO mapping Axis %d: Operation Mode Display Object=0x%x not found", pDemoAxis->wDriveFixAddress, pDemoAxis->IdxModeOfOperationDisplay.dwVarIdx);
              dwRetVal = EC_E_INVALIDPARM;
          }
      }
#endif
      /* Get the number of OUTPUT process variable information entries of the current slave */
      dwRetVal = emGetSlaveOutpVarInfoNumOf(dwInstanceID, EC_TRUE, pDemoAxis->wDriveFixAddress, &wNumOfVarsToReadOutp );
      if(wNumOfVarsToReadOutp > 0)
      {
         /* Allocate  memory for the OUTPUT process variable information entries of the current slave */
         pProcessVarInfo = (EC_T_PROCESS_VAR_INFO_EX*)OsMalloc(sizeof(EC_T_PROCESS_VAR_INFO_EX)* wNumOfVarsToReadOutp);
         if(pProcessVarInfo == EC_NULL)
         {
            dwRetVal = EC_E_NOMEMORY;
         }
         else
         {
            /* Get all OUTPUT process variable information entries of the current slave */
            dwRetVal = emGetSlaveOutpVarInfoEx(dwInstanceID, EC_TRUE, pDemoAxis->wDriveFixAddress, wNumOfVarsToReadOutp, pProcessVarInfo, &wReadEntries);
            if (EC_E_NOERROR != dwRetVal)
            {
               LogError( (EC_T_CHAR*)"ERROR: emGetSlaveInpVarInfoEx() (Result = %s 0x%x)", ecatGetText(dwRetVal), dwRetVal );
            }

            /* search output variables */
            for (dwProcessVarIdx = 0; dwProcessVarIdx < wReadEntries; dwProcessVarIdx++)
            {        
               /* control word */
               if ((pDemoAxis->IdxControlword.dwPdoIdx == 0 || pProcessVarInfo[dwProcessVarIdx].wPdoIndex == pDemoAxis->IdxControlword.dwPdoIdx)
                  && pProcessVarInfo[dwProcessVarIdx].wIndex == pDemoAxis->IdxControlword.dwVarIdx)   /* status word ? */
               {

                  pDemoAxis->pwPdControlWord = (EC_T_WORD*)&(pbyPDOut[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }

               /* target position */
               if ((pDemoAxis->IdxTargetPos.dwPdoIdx == 0 || pProcessVarInfo[dwProcessVarIdx].wPdoIndex == pDemoAxis->IdxTargetPos.dwPdoIdx)
                  && pProcessVarInfo[dwProcessVarIdx].wIndex == pDemoAxis->IdxTargetPos.dwVarIdx) 
               {
                  pDemoAxis->plPdTargetPosition  = (EC_T_SDWORD*)&(pbyPDOut[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }

               /* target velocity */
               if ((pDemoAxis->IdxTargetVel.dwPdoIdx == 0 || pProcessVarInfo[dwProcessVarIdx].wPdoIndex == pDemoAxis->IdxTargetVel.dwPdoIdx)
                  && pProcessVarInfo[dwProcessVarIdx].wIndex == pDemoAxis->IdxTargetVel.dwVarIdx) 
               {
                  pDemoAxis->plPdTargetVelocity  = (EC_T_SDWORD*)&(pbyPDOut[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }
               
               /* target torque */
               if ((pDemoAxis->IdxTargetTrq.dwPdoIdx == 0 || pProcessVarInfo[dwProcessVarIdx].wPdoIndex == pDemoAxis->IdxTargetTrq.dwPdoIdx)
                  && pProcessVarInfo[dwProcessVarIdx].wIndex == pDemoAxis->IdxTargetTrq.dwVarIdx) 
               {
                  pDemoAxis->psPdTargetTorque  = (EC_T_SWORD*)&(pbyPDOut[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }

               /* velocity offset for feed forward */
               if ((pDemoAxis->IdxVelOffset.dwPdoIdx == 0 || pProcessVarInfo[dwProcessVarIdx].wPdoIndex == pDemoAxis->IdxVelOffset.dwPdoIdx)
                  && pProcessVarInfo[dwProcessVarIdx].wIndex == pDemoAxis->IdxVelOffset.dwVarIdx) 
               {
                  pDemoAxis->plPdVelocityOffset  = (EC_T_SDWORD*)&(pbyPDOut[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }

               /* torque offset for feed forward */
               if ((pDemoAxis->IdxTorOffset.dwPdoIdx == 0 || pProcessVarInfo[dwProcessVarIdx].wPdoIndex == pDemoAxis->IdxTorOffset.dwPdoIdx)
                  && pProcessVarInfo[dwProcessVarIdx].wIndex == pDemoAxis->IdxTorOffset.dwVarIdx) 
               {
                  pDemoAxis->psPdTorqueOffset  = (EC_T_SWORD*)&(pbyPDOut[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }
               /* mode of operation */
               if ((pDemoAxis->IdxModeOfOperation.dwPdoIdx == 0 || pProcessVarInfo[dwProcessVarIdx].wPdoIndex == pDemoAxis->IdxModeOfOperation.dwPdoIdx)
                  && pProcessVarInfo[dwProcessVarIdx].wIndex == pDemoAxis->IdxModeOfOperation.dwVarIdx) 
               {
                  pDemoAxis->pbyPdModeOfOperation  = (EC_T_BYTE*)&(pbyPDOut[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }
#ifdef EC_MOTION_SUPPORT_PP_MODE
               /* profile velocity */
               if (pProcessVarInfo[dwProcessVarIdx].wIndex == DRV_OBJ_PROFILE_VELOCITY) 
               {
                  pDemoAxis->plPdProfileVelocity  = (EC_T_DWORD*)&(pbyPDOut[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }
               /* profile acc */
               if (pProcessVarInfo[dwProcessVarIdx].wIndex == DRV_OBJ_PROFILE_ACC) 
               {
                  pDemoAxis->plPdProfileAcc  = (EC_T_DWORD*)&(pbyPDOut[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }
               /* profile dec */
               if (pProcessVarInfo[dwProcessVarIdx].wIndex == DRV_OBJ_PROFILE_DEC) 
               {
                  pDemoAxis->plPdProfileDec  = (EC_T_DWORD*)&(pbyPDOut[pProcessVarInfo[dwProcessVarIdx].nBitOffs/8]);
               }
#endif
               if (pProcessVarInfo[dwProcessVarIdx].wIndex == DRV_OBJ_DIGITAL_OUTPUTS)
               {
                   pDemoAxis->pdwDigOutputs = (EC_T_DWORD*)&(pbyPDOut[pProcessVarInfo[dwProcessVarIdx].nBitOffs / 8]);
               }
            }

            OsFree(pProcessVarInfo);
            pProcessVarInfo = EC_NULL;
         }
      }

#ifdef EC_MOTION_SUPPORT_PP_MODE
      // Only in PP mode
      if (pDemoAxis->byDriveModesOfOperation == DRV_MODE_OP_PROF_POS)
      {
          if (pDemoAxis->plPdProfileVelocity == EC_NULL)
          {
              LogError((EC_T_CHAR*)"ERROR: Invalid PDO mapping Axis %d: Profile Velocity Object=0x%x not found", pDemoAxis->wDriveFixAddress, DRV_OBJ_PROFILE_VELOCITY);
              dwRetVal = EC_E_INVALIDPARM;
          }
          if (pDemoAxis->plPdProfileAcc == EC_NULL)
          {
              LogError((EC_T_CHAR*)"ERROR: Invalid PDO mapping Axis %d: Profile Acceleration Object=0x%x not found", pDemoAxis->wDriveFixAddress, DRV_OBJ_PROFILE_ACC);
              dwRetVal = EC_E_INVALIDPARM;
          }
          if (pDemoAxis->plPdProfileDec == EC_NULL)
          {
              LogError((EC_T_CHAR*)"ERROR: Invalid PDO mapping Axis %d: Profile Deceleration Object=0x%x not found", pDemoAxis->wDriveFixAddress, DRV_OBJ_PROFILE_DEC);
              dwRetVal = EC_E_INVALIDPARM;
          }
          if (pDemoAxis->pbyPdModeOfOperation == EC_NULL)
          {
              LogError((EC_T_CHAR*)"ERROR: Invalid PDO mapping Axis %d: Operation Mode Object=0x%x not found", pDemoAxis->wDriveFixAddress, DRV_OBJ_MODES_OF_OPERATION);
              dwRetVal = EC_E_INVALIDPARM;
          }
      }
#endif
      if (pDemoAxis->pwPdControlWord == EC_NULL)
      {
          LogError((EC_T_CHAR*)"ERROR: Invalid PDO mapping Axis %d: Control Word Object=0x%x not found", pDemoAxis->wDriveFixAddress, pDemoAxis->IdxControlword.dwVarIdx);
          dwRetVal = EC_E_INVALIDPARM;
      }

      EC_T_BYTE op = pDemoAxis->byDriveModesOfOperation;
      if(   ((pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402 && (op == DRV_MODE_OP_CSP || op == DRV_MODE_OP_INTER_POS))
         || (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_SERCOS && (op >= SER_OPMODE_POS_FB1 && op <= SER_OPMODE_POS_FB1FB2_LAGLESS)))
         && pDemoAxis->plPdTargetPosition == EC_NULL)
      {
         LogError( (EC_T_CHAR*)"ERROR: Invalid PDO mapping Axis %d: Target Position Object=0x%x not found", pDemoAxis->wDriveFixAddress, pDemoAxis->IdxTargetPos.dwVarIdx);
         dwRetVal = EC_E_INVALIDPARM;
      }

      if(   ((pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402 && op == DRV_MODE_OP_CSV)
         || (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_SERCOS && op == SER_OPMODE_VEL))
         && pDemoAxis->plPdTargetVelocity == EC_NULL)
      {
         LogError( (EC_T_CHAR*)"ERROR: Invalid PDO mapping Axis %d: Target Velocity Object=0x%x not found", pDemoAxis->wDriveFixAddress, pDemoAxis->IdxTargetVel.dwVarIdx);
         dwRetVal = EC_E_INVALIDPARM;
      }

      if(   ((pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_DS402 && op == DRV_MODE_OP_CST)
         || (pDemoAxis->eDriveProfile == MC_T_AXIS_PROFILE_SERCOS && op == SER_OPMODE_TORQUE))
         && pDemoAxis->psPdTargetTorque == EC_NULL)
      {
         LogError( (EC_T_CHAR*)"ERROR: Invalid PDO mapping Axis %d: Target Torque Object=0x%x not found", pDemoAxis->wDriveFixAddress, pDemoAxis->IdxTargetTrq.dwVarIdx);
         dwRetVal = EC_E_INVALIDPARM;
      }

      if (dwRetVal == EC_E_NOERROR)
      {
         MC_T_AXIS_INIT_INPUTS inp;
         OsMemset(&inp, 0, sizeof(MC_T_AXIS_INIT_INPUTS));
         inp.pActualTorque = pDemoAxis->psPdActualTorque;
         inp.pActualPosition = pDemoAxis->plPdActualPosition;
         inp.pDigitalInputs = pDemoAxis->pdwDigInputs;

         pDemoAxis->pFb->Axis.InitInputs(inp);

         MC_T_AXIS_INIT_OUTPUTS outp;
         OsMemset(&outp, 0, sizeof(MC_T_AXIS_INIT_OUTPUTS));
         outp.pTargetTorque = pDemoAxis->psPdTargetTorque;
         outp.pTargetVelocity = pDemoAxis->plPdTargetVelocity;
         outp.pTargetPosition = pDemoAxis->plPdTargetPosition;
         outp.pVelocityOffset = pDemoAxis->plPdVelocityOffset;
         outp.pTorqueOffset   = pDemoAxis->psPdTorqueOffset;
         outp.pModeOfOperation = pDemoAxis->pbyPdModeOfOperation;
         outp.pModeOfOperationDisplay = pDemoAxis->pbyPdModeOfOperationDisplay;
         outp.pDigitalOutputs = pDemoAxis->pdwDigOutputs;

#ifdef EC_MOTION_SUPPORT_PP_MODE
         outp.pProfileVelocity = pDemoAxis->plPdProfileVelocity;
         outp.pProfileAcc = pDemoAxis->plPdProfileAcc;
         outp.pProfileDec = pDemoAxis->plPdProfileDec;
#endif
         pDemoAxis->pFb->Axis.InitOutputs(outp);

         if (pDemoAxis->pFb->AxisInitData.AxisType == MC_AXIS_TYPE_REAL_ALL)
         {
            /* store information about EtherCAT slave into AXIS ref */
            MC_T_AXIS_INIT_ECAT axisEcInitData = { 0 };
            axisEcInitData.ProductCode        = oSlaveInfo.dwProductCode;
            axisEcInitData.VendorId           = oSlaveInfo.dwVendorId;
            axisEcInitData.SlaveID            = oSlaveInfo.dwSlaveId;
            axisEcInitData.StationAddress     = oSlaveInfo.wStationAddress;
            axisEcInitData.Profile            = pDemoAxis->eDriveProfile;
            axisEcInitData.SercosDriveNo      = pDemoAxis->bySercosDriveNo;
            axisEcInitData.CoeIdxOpMode       = (EC_T_WORD) pDemoAxis->dwCoeIdxOpMode;
            axisEcInitData.pEcatGetSlaveState = ecatGetSlaveState;
            axisEcInitData.pStatusWord        = pDemoAxis->pwPdStatusWord;
            axisEcInitData.pControlWord       = pDemoAxis->pwPdControlWord;

            switch (axisEcInitData.Profile)
            {
            case MC_T_AXIS_PROFILE_DS402:
               axisEcInitData.pEcatCoeSdoDownload = ecatCoeSdoDownload;
               axisEcInitData.pEcatCoeSdoUpload = ecatCoeSdoUpload;
               break;
            case MC_T_AXIS_PROFILE_SERCOS:
               axisEcInitData.pEcatSoeWrite = ecatSoeWrite;
               axisEcInitData.pEcatSoeRead = ecatSoeRead;
               break;
            }

            pDemoAxis->pFb->Axis.InitEcat(axisEcInitData);
         }


         /* set modes of operation */
         dwRetVal = pDemoAxis->pFb->Axis.SetModeOfOperation(pDemoAxis->byDriveModesOfOperation);
         if (EC_E_NOERROR != dwRetVal)
         {
            LogError( (EC_T_CHAR*)"Axis %d: Cannot set mode of operation for the drive (Result = %s 0x%x)!", pDemoAxis->wDriveFixAddress, ecatGetText(dwRetVal), dwRetVal );
         }
      }
   }

   return dwRetVal;
}

/* 
 * Pop motion command from the cmdqueue and process them if any are pending.
 *
 * The cmdqueue is filled by the EC-Master notification mechanism. The
 * cmd's are send over RAS from the EC-SlaveTestApplication tool.
 */
static EC_T_DEMO_AXIS* MotionQueuePopCmd(EC_T_DWORD dwBusCycleTimeUsec)
{
   MC_T_CMD_INP_PARA_V2 oMcInpCmd = {0};

   // get new command from EC-STA
   if (!S_pPendMotionCmdFifo->RemoveNoLock(oMcInpCmd)) return EC_NULL;

   EC_T_DEMO_AXIS *pDemoAxis = myAppSearchAxis(oMcInpCmd.dwStationAddress);
   if (pDemoAxis == EC_NULL) return EC_NULL;

   switch (oMcInpCmd.dwCmdCode)
   {
   case MC_CMD_CODE_POWER_ON:     /* Power-On */
      TRC("Motion Command: Power-On received");

      pDemoAxis->dwDriveIncPerMM = oMcInpCmd.dwIncPerMM;

      pDemoAxis->pFb->AxisInitData.IncPerMM = pDemoAxis->dwDriveIncPerMM;
      pDemoAxis->pFb->AxisInitData.CycleTime = dwBusCycleTimeUsec;
      pDemoAxis->pFb->Axis.Init(pDemoAxis->pFb->AxisInitData);

      pDemoAxis->pFb->Power.Enable = MC_TRUE;
      pDemoAxis->pFb->Power.EnablePositive = MC_TRUE;
      pDemoAxis->pFb->Power.EnableNegative = MC_TRUE;
      break;

   case MC_CMD_CODE_POWER_OFF:     /* Power-Off */
      TRC("Motion Command: Power-Off received");
      pDemoAxis->pFb->Power.Enable = EC_FALSE;
      pDemoAxis->pFb->Power.EnablePositive = EC_FALSE;
      pDemoAxis->pFb->Power.EnableNegative = EC_FALSE;
      break;

#if defined(EC_MOTION_TRAJECTORY_GEN)
   case MC_CMD_CODE_STOP:     /* Stop */
      if(!pDemoAxis->pFb->Stop.Execute)
      {
         TRC("Motion Command: Stop received");
         pDemoAxis->pFb->Stop.Execute =  EC_TRUE;
         pDemoAxis->pFb->Stop.Deceleration =  oMcInpCmd.fDec;         /* mm/sec^2 */
         pDemoAxis->pFb->Stop.Jerk         =  oMcInpCmd.fJerk;        /* mm/sec^3   */
      }
      break;

   case MC_CMD_CODE_HALT:     /* Halt */
      if(!pDemoAxis->pFb->Halt.Execute)
      {
         TRC("Motion Command: Halt received");
         pDemoAxis->pFb->Halt.Execute =  EC_TRUE;
         pDemoAxis->pFb->Halt.Deceleration =  oMcInpCmd.fDec;         /* mm/sec^2 */
         pDemoAxis->pFb->Halt.Jerk         =  oMcInpCmd.fJerk;        /* mm/sec^3   */
      }
      break;

   case MC_CMD_CODE_MOVE_RELATIVE:     /* Move Relative */
      if(!pDemoAxis->pFb->MoveRelative.Execute || pDemoAxis->pFb->MoveRelative.ContinuousUpdate)
      {
         TRC("Motion Command: Move Relative received and accepted");
         pDemoAxis->pFb->MoveRelative.Distance     =  oMcInpCmd.fDistance;
         pDemoAxis->pFb->MoveRelative.Acceleration =  oMcInpCmd.fAcc;         /* mm/sec^2 */
         pDemoAxis->pFb->MoveRelative.Deceleration =  oMcInpCmd.fDec;         /* mm/sec^2 */
         pDemoAxis->pFb->MoveRelative.Velocity     =  oMcInpCmd.fVel;         /* mm/sec   */
         pDemoAxis->pFb->MoveRelative.Jerk         =  oMcInpCmd.fJerk;        /* mm/sec^3   */
         pDemoAxis->pFb->MoveRelative.ContinuousUpdate   =  oMcInpCmd.dwContUpdate;
         if(!pDemoAxis->pFb->MoveRelative.Execute) 
            pDemoAxis->bTriggerMoveRel      = EC_TRUE;
      }
      else
      {
         LogError("Motion Command: Move Relative received, but not accepted due to ongoing move");
      }
      break;

   case MC_CMD_CODE_MOVE_ABSOULTE:     /* Move Absolute */
      if(!pDemoAxis->pFb->MoveAbsolute.Execute || pDemoAxis->pFb->MoveAbsolute.ContinuousUpdate)
      {
         TRC("Motion Command: Move Absolute received and accepted");
         pDemoAxis->pFb->MoveAbsolute.Position     =  oMcInpCmd.fDistance;
         pDemoAxis->pFb->MoveAbsolute.Acceleration =  oMcInpCmd.fAcc;         /* mm/sec^2 */
         pDemoAxis->pFb->MoveAbsolute.Deceleration =  oMcInpCmd.fDec;         /* mm/sec^2 */
         pDemoAxis->pFb->MoveAbsolute.Velocity     =  oMcInpCmd.fVel;         /* mm/sec   */
         pDemoAxis->pFb->MoveAbsolute.Jerk         =  oMcInpCmd.fJerk;        /* mm/sec^3   */
         pDemoAxis->pFb->MoveAbsolute.Direction   =  (MC_T_DIRECTION) oMcInpCmd.dwDirection;
         pDemoAxis->pFb->MoveAbsolute.ContinuousUpdate   =  oMcInpCmd.dwContUpdate;
         if(!pDemoAxis->pFb->MoveAbsolute.Execute) 
            pDemoAxis->bTriggerMoveAbs      = EC_TRUE;
      }
      else
      {
         LogError("Motion Command: Move Absolute received, but not accepted due to ongoing move");
      }
      break;

   case MC_CMD_CODE_MOVE_VELOCITY:    /* Move Velocity */
       if (!pDemoAxis->pFb->MoveVelocity.Execute || pDemoAxis->pFb->MoveVelocity.ContinuousUpdate)
       {
           pDemoAxis->pFb->MoveVelocity.Execute = EC_FALSE;
           TRC("Motion Command: Move Velocity received and accepted");
           pDemoAxis->pFb->MoveVelocity.Acceleration = oMcInpCmd.fAcc;         /* mm/sec^2 */
           pDemoAxis->pFb->MoveVelocity.Deceleration = oMcInpCmd.fDec;         /* mm/sec^2 */
           pDemoAxis->pFb->MoveVelocity.Velocity = oMcInpCmd.fVel;         /* mm/sec   */
           pDemoAxis->pFb->MoveVelocity.Jerk = oMcInpCmd.fJerk;        /* mm/sec^3   */
           pDemoAxis->pFb->MoveVelocity.Direction = (MC_T_DIRECTION)oMcInpCmd.dwDirection;
           pDemoAxis->pFb->MoveVelocity.ContinuousUpdate = oMcInpCmd.dwContUpdate;
           pDemoAxis->bTriggerMoveVelo = EC_TRUE;
       }

       break;
#endif /* defined(EC_MOTION_TRAJECTORY_GEN) */

   case MC_CMD_CODE_RESET:            /* Reset */
      if(!pDemoAxis->pFb->Reset.Execute)
      {
         TRC("Motion Command: Reset received and accepted");
         pDemoAxis->pFb->Reset.Execute =  EC_TRUE;
      }
      break;

   case MC_CMD_READ_PARAMETER:       /* Read Parameter */
      pDemoAxis->pFb->ReadParameter.Enable =  EC_TRUE;
      pDemoAxis->pFb->ReadParameter.ParameterNumber =  oMcInpCmd.dwParaNumber; 
      break;

   case MC_CMD_READ_BOOL_PARAMETER:  /* Read Bool Parameter */
      pDemoAxis->pFb->ReadBoolParameter.Enable =  EC_TRUE;
      pDemoAxis->pFb->ReadBoolParameter.ParameterNumber =  oMcInpCmd.dwParaNumber; 
      break;

   case MC_CMD_WRITE_PARAMETER:       /* Write Parameter */
      if(!pDemoAxis->pFb->WriteParameter.Execute)
      {
         pDemoAxis->pFb->WriteParameter.Execute =  EC_TRUE;
         pDemoAxis->pFb->WriteParameter.ParameterNumber =  oMcInpCmd.dwParaNumber; 
         pDemoAxis->pFb->WriteParameter.Value           =  oMcInpCmd.fParaValue; 
      }
      break;

   case MC_CMD_WRITE_BOOL_PARAMETER:   /* Write Bool Parameter */
      if(!pDemoAxis->pFb->WriteBoolParameter.Execute)
      {
         pDemoAxis->pFb->WriteBoolParameter.Execute =  EC_TRUE;
         pDemoAxis->pFb->WriteBoolParameter.ParameterNumber =  oMcInpCmd.dwParaNumber; 
         pDemoAxis->pFb->WriteBoolParameter.Value           =  oMcInpCmd.bParaValue; 
      }
      break;
#ifdef EC_MOTION_SUPPORT_PP_MODE
   case MC_CMD_HOME:   /* Home */
       TRC("Motion Command: Home received");
       if (!pDemoAxis->pFb->Home.Execute)
       {
           // Absolute home position, set after homing done
           pDemoAxis->pFb->Home.Position = HOMING_POSITION;
           // Start homing
           pDemoAxis->pFb->Home.Execute = EC_TRUE;
       }
       break;
#endif
   default:
      LogError("ERROR: Invalid motion command code received: Code=%d", oMcInpCmd.dwCmdCode);
      break;
   }

   return pDemoAxis;
}

/*
 * Run cyclic part of MCFB's
 */
static void MotionOnCycle(EC_T_DEMO_AXIS *pDemoAxis)
{
   /* power on drive: we have to call this in each cycle */
   pDemoAxis->pFb->Power.OnCycle();
   if (pDemoAxis->pFb->Power.Enable && pDemoAxis->pFb->Power.Error)
   {
      LogError("ERROR MC_Power Axis %d: %s Code=%d", pDemoAxis->wDriveFixAddress, 
         MC_GetErrorText(pDemoAxis->pFb->Power.ErrorID), pDemoAxis->pFb->Power.ErrorID);
   }

   if (pDemoAxis->pFb->Power.Status)
   {
      if (!S_dwMotionStartCycle) S_dwMotionStartCycle = S_dwJobTaskCycleCnt;
   }

#if defined(EC_MOTION_TRAJECTORY_GEN)
   /* Stop ? */
   pDemoAxis->pFb->Stop.OnCycle();
   if (pDemoAxis->pFb->Stop.Done || pDemoAxis->pFb->Stop.CommandAborted)
   {
      pDemoAxis->pFb->Stop.Execute = EC_FALSE;
   }
   else if(pDemoAxis->pFb->Stop.Error)     /* error ? */
   {
      LogError("ERROR MC_Stop Axis %d: %s Code=%d", pDemoAxis->wDriveFixAddress, 
         MC_GetErrorText(pDemoAxis->pFb->Stop.ErrorID), pDemoAxis->pFb->Stop.ErrorID);
   }

   /* Halt ? */
   pDemoAxis->pFb->Halt.OnCycle();
   if(pDemoAxis->pFb->Halt.Done || pDemoAxis->pFb->Halt.CommandAborted)
   {
      pDemoAxis->pFb->Halt.Execute = EC_FALSE;
   }
   else if(pDemoAxis->pFb->Halt.Error)     /* error ? */
   {
      LogError("ERROR MC_Halt Axis %d: %s Code=%d", pDemoAxis->wDriveFixAddress, 
         MC_GetErrorText(pDemoAxis->pFb->Halt.ErrorID), pDemoAxis->pFb->Halt.ErrorID);
      pDemoAxis->pFb->Halt.Execute =  EC_FALSE;                // Reset error
   }
#endif /* defined(EC_MOTION_TRAJECTORY_GEN) */

   /* Reset */
   pDemoAxis->pFb->Reset.OnCycle();
   if(pDemoAxis->pFb->Reset.Done)
   {
      pDemoAxis->pFb->Reset.Execute = EC_FALSE;
   }
   else if(pDemoAxis->pFb->Reset.Error)     /* error ? */
   {
       LogError("ERROR MC_Reset Axis %d: %s Code=%d", pDemoAxis->wDriveFixAddress,
         MC_GetErrorText(pDemoAxis->pFb->Reset.ErrorID), pDemoAxis->pFb->Reset.ErrorID);
       pDemoAxis->pFb->Reset.Execute = EC_FALSE;
   }

   /* Read axis info */
   pDemoAxis->pFb->ReadAxisInfo.Enable = EC_TRUE;
   pDemoAxis->pFb->ReadAxisInfo.OnCycle();

   /* Read Parameter */
   pDemoAxis->pFb->ReadParameter.OnCycle();

   /* Read Bool Parameter */
   pDemoAxis->pFb->ReadBoolParameter.OnCycle();

   /* Write Parameter */
   pDemoAxis->pFb->WriteParameter.OnCycle();
   if(pDemoAxis->pFb->WriteParameter.Execute)
   {
      pDemoAxis->pFb->WriteParameter.Execute = EC_FALSE;
   }

   /* Write Bool Parameter */
   pDemoAxis->pFb->WriteBoolParameter.OnCycle();
   if(pDemoAxis->pFb->WriteBoolParameter.Execute)
   {
      pDemoAxis->pFb->WriteBoolParameter.Execute = EC_FALSE;
   }

#if defined(EC_MOTION_TRAJECTORY_GEN)
   /* read Motion State */
   pDemoAxis->pFb->ReadMotionState.Enable = EC_TRUE;
   pDemoAxis->pFb->ReadMotionState.OnCycle();
#endif /* defined(EC_MOTION_TRAJECTORY_GEN) */

   /* read actual position */
   pDemoAxis->pFb->ReadActualPosition.Enable = EC_TRUE;
   pDemoAxis->pFb->ReadActualPosition.OnCycle();

#if defined(EC_MOTION_TRAJECTORY_GEN)
   /* Motion function blocks */ 
   pDemoAxis->pFb->MoveRelative.OnCycle();
   pDemoAxis->pFb->MoveAbsolute.OnCycle();
   pDemoAxis->pFb->MoveVelocity.OnCycle();

   /* relative move */
#ifndef MOVE_TEST_SEQUENCE
   if (pDemoAxis->pFb->MoveRelative.Execute)
   {
      if (pDemoAxis->pFb->MoveRelative.Error)
      {
         LogError("ERROR MC_MoveRelative Axis %d: %s Code=%d", pDemoAxis->wDriveFixAddress,
            MC_GetErrorText(pDemoAxis->pFb->MoveRelative.ErrorID), pDemoAxis->pFb->MoveRelative.ErrorID);
      }

      if (pDemoAxis->pFb->MoveRelative.Done 
         || pDemoAxis->pFb->MoveRelative.Error 
         || pDemoAxis->pFb->MoveRelative.CommandAborted)      /* move done or aborted ? */
      {
         pDemoAxis->pFb->MoveRelative.Execute = EC_FALSE;
      }
   }
   else if (pDemoAxis->bTriggerMoveRel)
   {
      /* start new movement */
      pDemoAxis->bTriggerMoveRel = EC_FALSE;
      pDemoAxis->pFb->MoveRelative.Execute = EC_TRUE;
      pDemoAxis->bTorqueLimitActive = EC_FALSE;
   }
#endif /* MOVE_TEST_SEQUENCE */
   /* absolute move */
   if (pDemoAxis->pFb->MoveAbsolute.Execute)
   {
      if (pDemoAxis->pFb->MoveAbsolute.Error)
      {
         LogError("ERROR MC_MoveAbsolute Axis %d: %s Code=%d", pDemoAxis->wDriveFixAddress, 
            MC_GetErrorText(pDemoAxis->pFb->MoveAbsolute.ErrorID), pDemoAxis->pFb->MoveAbsolute.ErrorID);
      }

      if (pDemoAxis->pFb->MoveAbsolute.Done 
         || pDemoAxis->pFb->MoveAbsolute.Error 
         || pDemoAxis->pFb->MoveAbsolute.CommandAborted)     /* move done ? */
      {
         pDemoAxis->pFb->MoveAbsolute.Execute = EC_FALSE;
      }
   }
   else if (pDemoAxis->bTriggerMoveAbs)
   {
      /* start new movement */
      pDemoAxis->bTriggerMoveAbs = EC_FALSE;
      pDemoAxis->pFb->MoveAbsolute.Execute = EC_TRUE;
      pDemoAxis->bTorqueLimitActive = EC_FALSE;
   }

   /* absolute velocity */
   if (pDemoAxis->pFb->MoveVelocity.Execute)
   {
      if (pDemoAxis->pFb->MoveVelocity.Error)     /* error ? */
      {
         LogError("ERROR MC_MoveVelocity Axis %d: %s Code=%d", pDemoAxis->wDriveFixAddress,
            MC_GetErrorText(pDemoAxis->pFb->MoveVelocity.ErrorID), pDemoAxis->pFb->MoveVelocity.ErrorID);
         pDemoAxis->pFb->MoveVelocity.Execute = EC_FALSE;
      }
      if (pDemoAxis->pFb->MoveVelocity.CommandAborted) /* This is the regular way to stop this motion */
      {
         LogError("ABORTED MC_MoveVelocity");
         pDemoAxis->pFb->MoveVelocity.Execute = EC_FALSE;
      }
   }
   else if (pDemoAxis->bTriggerMoveVelo)
   {
      /* start new movement */
      pDemoAxis->bTriggerMoveVelo = EC_FALSE;
      pDemoAxis->pFb->MoveVelocity.Execute = EC_TRUE;
      pDemoAxis->bTorqueLimitActive = EC_FALSE;
   }
#endif /* defined(EC_MOTION_TRAJECTORY_GEN) */

   /* check target pos reached */
   pDemoAxis->pFb->CheckTargetposReached.InPositionWindow = pDemoAxis->dwPosWindow;
   pDemoAxis->pFb->CheckTargetposReached.OnCycle();
   if (pDemoAxis->bTriggerCheckPos)
   {
      pDemoAxis->bTriggerCheckPos = EC_FALSE;
      pDemoAxis->pFb->CheckTargetposReached.Execute = EC_TRUE;
   }

#ifdef MOTION_ENABLE_TORQUE_LIMIT

   /* Enable drive's torque limit. The torque limit must be set
    * prior in CoE object 0x2404 and/or 0x2405 (put CoE init command in ENI).
    */
   EC_T_WORD wControlWord = EC_GET_FRM_WORD(pDemoAxis->pwPdControlWord);
   wControlWord |= MOTION_YASK_CTRL_TORQUE_LIMIT_EN;
   EC_SET_FRM_WORD(pDemoAxis->pwPdControlWord, wControlWord);

   // Torque limit active?
   EC_T_WORD wStatusWord = EC_GET_FRM_WORD(pDemoAxis->pwPdStatusWord);
   if (!pDemoAxis->bTorqueLimitActive 
      && wStatusWord & MOTION_YASK_STAT_TORQUE_LIMIT_ACT)
   {
      pDemoAxis->bTorqueLimitActive = EC_TRUE;

      /* Sync target position with the actual position. The drive halts immediately
       * if the force is released that hold the drive in the torque limitation.
       */
      pDemoAxis->pFb->HaltRecovery.RecoveryMode = MC_RECOVERY_ABORT_MOVEMENT;
      pDemoAxis->pFb->HaltRecovery.Execute = EC_TRUE;
   }

   pDemoAxis->pFb->HaltRecovery.OnCycle();
   pDemoAxis->pFb->HaltRecovery.Execute = EC_FALSE;

#endif

#ifdef EC_MOTION_SUPPORT_PP_MODE
   // Homing is supported only in PP mode
   // Homing demo
   pDemoAxis->pFb->Home.OnCycle();
   if (pDemoAxis->pFb->Home.Execute)
   {
       if (pDemoAxis->pFb->Home.Error)
       {
           LogError("ERROR MC_Home Axis %d: %s Code=%d", pDemoAxis->wDriveFixAddress,
               MC_GetErrorText(pDemoAxis->pFb->Home.ErrorID), pDemoAxis->pFb->Home.ErrorID);
       }

       if (pDemoAxis->pFb->Home.Done
           || pDemoAxis->pFb->Home.Error
           || pDemoAxis->pFb->Home.CommandAborted)     /* move done ? */
       {
           pDemoAxis->pFb->Home.Execute = EC_FALSE;
       }
   }
#endif

   // Digital inputs (object 0x60FD) have to be mapped in order to run inputs demo
   // Digital outputs (object 0x60FE) have to be mapped in order to run outputs demo
   if ((pDemoAxis->pdwDigInputs != EC_NULL) || (pDemoAxis->pdwDigOutputs != EC_NULL))
   {
       // Digital I/O Demo
       static MC_T_DWORD    dwCnt = 0;
       static MC_T_INT      iInput = 0;
       static MC_T_INT      iOutput = 0;

       if (dwCnt++ % 100 == 0)
       {
           if (pDemoAxis->pdwDigInputs != EC_NULL)
           {
               // InputNumber must be between 0 and 31
               // InputNumber is equal bit number from object 0x60FD
               pDemoAxis->pFb->ReadDigitalInput.InputNumber = iInput % 32;
               pDemoAxis->pFb->ReadDigitalInput.Enable = EC_TRUE;;
               iInput++;
           }
           if (pDemoAxis->pdwDigOutputs != EC_NULL)
           {
               // OutputNumber must be between 0 and 31
               // OutputNumber is equal bit number from object 0x60FE
               pDemoAxis->pFb->ReadDigitalOutput.OutputNumber = iOutput % 32;
               pDemoAxis->pFb->ReadDigitalOutput.Enable = EC_TRUE;
               iOutput++;
           }
       }

       if (dwCnt % 1000 == 0)
       {
           if (pDemoAxis->pdwDigOutputs != EC_NULL)
           {
               if (!pDemoAxis->pFb->WriteDigitalOutput.Execute)
               {
                   // OutputNumber must be between 0 and 31
                   // OutputNumber is equal bit number from object 0x60FE
                   pDemoAxis->pFb->WriteDigitalOutput.OutputNumber = 24;
                   pDemoAxis->pFb->WriteDigitalOutput.Value = !pDemoAxis->pFb->WriteDigitalOutput.Value;
                   pDemoAxis->pFb->WriteDigitalOutput.Execute = EC_TRUE;
               }
           }
       }

       // Only if digital inputs are mapped
       if (pDemoAxis->pdwDigInputs != EC_NULL)
       {
           pDemoAxis->pFb->ReadDigitalInput.OnCycle();
           if (pDemoAxis->pFb->ReadDigitalInput.Enable)
           {
               if (pDemoAxis->pFb->ReadDigitalInput.Error)
               {
                   LogError("ERROR ReadDigitalInput Axis %d: %s Code=%d Input=%d", pDemoAxis->wDriveFixAddress,
                       MC_GetErrorText(pDemoAxis->pFb->ReadDigitalInput.ErrorID), pDemoAxis->pFb->ReadDigitalInput.ErrorID,
                       pDemoAxis->pFb->ReadDigitalInput.InputNumber);
               }

               if (pDemoAxis->pFb->ReadDigitalInput.Valid)
               {
                   LogMsg("ReadDigitalInput(%u) = %u", pDemoAxis->pFb->ReadDigitalInput.InputNumber, pDemoAxis->pFb->ReadDigitalInput.Value);
                   pDemoAxis->pFb->ReadDigitalInput.Enable = EC_FALSE;
               }
           }
       }

        // Only if digital outputs are mapped
      if (pDemoAxis->pdwDigOutputs != EC_NULL)
       {
           pDemoAxis->pFb->ReadDigitalOutput.OnCycle();
           if (pDemoAxis->pFb->ReadDigitalOutput.Enable)
           {
               if (pDemoAxis->pFb->ReadDigitalOutput.Error)
               {
                   LogError("ERROR ReadDigitalOutput Axis %d: %s Code=%d Input=%d", pDemoAxis->wDriveFixAddress,
                       MC_GetErrorText(pDemoAxis->pFb->ReadDigitalOutput.ErrorID), pDemoAxis->pFb->ReadDigitalOutput.ErrorID,
                       pDemoAxis->pFb->ReadDigitalOutput.OutputNumber);
               }

               if (pDemoAxis->pFb->ReadDigitalOutput.Valid)
               {
                   LogMsg("ReadDigitalOutput(%u) = %u", pDemoAxis->pFb->ReadDigitalOutput.OutputNumber, pDemoAxis->pFb->ReadDigitalOutput.Value);
                   pDemoAxis->pFb->ReadDigitalOutput.Enable = EC_FALSE;
               }
           }

           pDemoAxis->pFb->WriteDigitalOutput.OnCycle();
           if (pDemoAxis->pFb->WriteDigitalOutput.Execute)
           {
               if (pDemoAxis->pFb->WriteDigitalOutput.Error)
               {
                   LogError("ERROR WriteDigitalOutput Axis %d: %s Code=%d Input=%d", pDemoAxis->wDriveFixAddress,
                       MC_GetErrorText(pDemoAxis->pFb->WriteDigitalOutput.ErrorID), pDemoAxis->pFb->WriteDigitalOutput.ErrorID,
                       pDemoAxis->pFb->WriteDigitalOutput.OutputNumber);
               }

               if (pDemoAxis->pFb->WriteDigitalOutput.Done)
               {
                   LogMsg("WriteDigitalOutput(%u) = %u", pDemoAxis->pFb->WriteDigitalOutput.OutputNumber, pDemoAxis->pFb->WriteDigitalOutput.Value);
                   pDemoAxis->pFb->WriteDigitalOutput.Execute = EC_FALSE;
               }
           }
       }
   }
}

/*
 * Check if all configured axes are powered on and ready for move
 */
static bool AreAxesReady()
{
   int axCnt = 0;
   int i;
   
   for (i = 0; i < S_numAxes; ++i) 
   {
      EC_T_DEMO_AXIS *pDemoAxis = &S_aAxisList[i];
      if (pDemoAxis->pFb->Power.Status) ++axCnt;
   }

   return axCnt == i;
}

/*
 * Runs a state machine that moves's the axes forward and backward.
 * 
 * This code is run if the Motion Demo runs in standalone / unintended mode.
 * That means the config's XML-tag MotionDemo/CmdMode is set to false.
 */
static void MotionStandaloneModeOnCycle()
{
#if defined(EC_MOTION_TRAJECTORY_GEN)
   int i;
   for (i = 0; i < S_numAxes; ++i) 
   {
      EC_T_DEMO_AXIS *pDemoAxis = &S_aAxisList[i];

      pDemoAxis->pFb->Power.Enable = MC_TRUE;
      pDemoAxis->pFb->Power.EnablePositive = MC_TRUE;
      pDemoAxis->pFb->Power.EnableNegative = MC_TRUE;

      if (AreAxesReady())
      {
         /* we are either in state "move setup", "moving" or "pause" */
         switch (pDemoAxis->eMotionState)
         {
         case EMoveSetup:
            {
               pDemoAxis->pFb->MoveRelative.Acceleration =  pDemoAxis->dwDriveAcc;         /* u/sec^2 */
               pDemoAxis->pFb->MoveRelative.Deceleration =  pDemoAxis->dwDriveDec;         /* u/sec^2 */
               pDemoAxis->pFb->MoveRelative.Velocity     =  pDemoAxis->dwDriveVel;         /* u/sec   */
               pDemoAxis->pFb->MoveRelative.Jerk         =  pDemoAxis->dwDriveJerk;        /* u/sec^3   */

               double sign = /*i&1 ? -1.0 :*/ 1.0;

               switch (pDemoAxis->eDistanceState)    /* do different distances */
               {
               default:    
               case 0:
                  pDemoAxis->eDistanceState = 0;
                  pDemoAxis->pFb->MoveRelative.Distance =  sign * 2.0 * pDemoAxis->dwDriveDistance;       /* + 2*Distance */
                  break;

               case 1:
                  pDemoAxis->pFb->MoveRelative.Distance =  -sign * pDemoAxis->dwDriveDistance;      /* - 1*Distance */
                  break;

               case 2:
                  pDemoAxis->pFb->MoveRelative.Distance =  -sign * (pDemoAxis->dwDriveDistance/2);  /* - 1/2*Distance */
                  break;

               case 3:
                  pDemoAxis->pFb->MoveRelative.Distance =  -sign * (pDemoAxis->dwDriveDistance/2);  /* - 1/2*Distance */
                  break;
               }

               pDemoAxis->eDistanceState++;
               pDemoAxis->bTriggerMoveRel = EC_TRUE;
               pDemoAxis->bTriggerCheckPos = EC_TRUE;
               pDemoAxis->eMotionState = EMoving;
               break;
            }
         case EMoving:
            {
               if (pDemoAxis->pFb->MoveRelative.Error)
               {
                  LogError("ERROR Axis %d: Error Code=%d", i+1, pDemoAxis->pFb->MoveRelative.ErrorID);
                  pDemoAxis->eMotionState = EPause;
               }

               MC_T_BOOL isDone = 
#ifdef MOTION_INPOSITION_CHECK
                  pDemoAxis->pFb->CheckTargetposReached.InPosition
#else
                  pDemoAxis->pFb->MoveRelative.Done
#endif
                  ;

               if (isDone
                  || pDemoAxis->pFb->MoveRelative.Error 
                  || pDemoAxis->pFb->MoveRelative.CommandAborted)
               {
                  pDemoAxis->pFb->MoveRelative.Execute = EC_FALSE;
                  pDemoAxis->pFb->CheckTargetposReached.Execute = EC_FALSE;
                  pDemoAxis->eMotionState = EPause;
               }
            }
            break;

         case EPause:
            pDemoAxis->nWaitCounter1++;
            if (pDemoAxis->nWaitCounter1 == 200)
            {
               pDemoAxis->nWaitCounter1 = 0;
               pDemoAxis->eMotionState = EMoveSetup;
            }
            break;
         default:
            {
               OsDbgAssert(0);
            }
         }
      }
   } /* loop through axis list */
#endif /* defined(EC_MOTION_TRAJECTORY_GEN) */
}

/***************************************************************************************************/
/**
\brief  demo application working process data function.

  This function is called in every cycle after the the master stack is started.
  
*/
static EC_T_DWORD myAppWorkpd(
    CAtEmLogging*       poLog,          /* [in]  Logging instance */ 
    EC_T_INT            nVerbose,       /* [in]  Verbosity level */
    EC_T_BYTE*          pbyPDIn,        /* [in]  pointer to process data input buffer */
    EC_T_BYTE*          pbyPDOut,       /* [in]  pointer to process data output buffer */
    EC_T_DWORD          dwInstanceID,      /* [in]  Master Instance ID */
    EC_T_STATE          eMasterState,
    EC_T_DWORD          dwBusCycleTimeUsec /* [in]  bus cycle time in usec */
    )
{
    EC_UNREFPARM(poLog);
    EC_UNREFPARM(nVerbose);
    EC_UNREFPARM(pbyPDIn);
    EC_UNREFPARM(pbyPDOut);

    /* drive available ? */
    if (eMasterState < eEcatState_SAFEOP)
    {
        goto Exit;
    }

    if (S_bCmdMode)
    {
       MotionQueuePopCmd(dwBusCycleTimeUsec);
    }
    else
    {
       MotionStandaloneModeOnCycle();
    }

    int i;
    for (i = 0; i < S_numAxes; ++i) 
    {
       EC_T_DEMO_AXIS *pDemoAxis = &S_aAxisList[i];
       MotionOnCycle(pDemoAxis);
    }

#ifdef MOTION_ENABLE_LOGFILE
    motionLogPos(dwInstanceID, dwBusCycleTimeUsec);
#endif

Exit:
    return EC_E_NOERROR;
 }

/***************************************************************************************************/
/**
\brief  demo application doing some diagnostic tasks

  This function is called in sometimes from the main demo task
*/
static EC_T_DWORD myAppDiagnosis(
    CAtEmLogging*       poLog,          /* [in]  Logging instance */ 
    EC_T_INT            nVerbose        /* [in]  Verbosity level */
    )
{
    EC_UNREFPARM(poLog);
    EC_UNREFPARM(nVerbose);

    return EC_E_NOERROR;
}

#ifdef MOVE_TEST_SEQUENCE
/***************************************************************************************************/
/**
\brief  Does some axis movements

\detail This function is called from the main demo task

*/
static EC_T_DWORD myAppMoveTest(
    CAtEmLogging*       poLog,          /* [in]  Logging instance */
    EC_T_INT            nVerbose        /* [in]  Verbosity level */
    )
{
    EC_UNREFPARM(poLog);
    EC_UNREFPARM(nVerbose);

    static  EC_T_INT        s_eDistanceState = 0;
    static  EC_T_INT        s_eMotionState = 0;
    static  EC_T_DWORD      s_dwSmooth = 10;
    static  EC_T_WORD       s_wGain = 0xFF00;

#ifdef ELMO_DRIVE
    EC_T_DWORD  dwInt = 0;
    EC_T_DWORD  dwRes = 0;
    EC_T_REAL   fFloat = 0.0;
#endif

    // We use the axis 1 (index 0) only
    EC_T_DEMO_AXIS* pDemoAxis = &S_aAxisList[0];

    /* drive available ? */
    if (pDemoAxis->pFb->Power.Status)
    {
        /* we are either in state "move setup", "moving", "pause" */
        switch (s_eMotionState)
        {
        default:
        case 0:         /* move setup */
            pDemoAxis->pFb->MoveRelative.Execute = EC_FALSE;
            pDemoAxis->pFb->MoveRelative.Distance = 20;
            pDemoAxis->pFb->MoveRelative.Velocity = 1000;            /* mm/sec   */
            pDemoAxis->pFb->MoveRelative.Acceleration = 100000;       /* mm/sec^2 */
            pDemoAxis->pFb->MoveRelative.Deceleration = 100000;       /* mm/sec^2 */
            pDemoAxis->pFb->MoveRelative.Jerk = 0;                  /* mm/sec^3 */

            s_eMotionState = 1; /* go to state "moving" */

            break;

        case 1:         /* moving */
#ifdef ELMO_DRIVE
            if (pDemoAxis->pFb->MoveRelative.Distance < 0)
            {
                // Example of using SetSmoothFactor()
                dwRes = SetSmoothFactor(pDemoAxis->wDriveFixAddress, s_dwSmooth);
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: SetSmoothFactor() error dwRes=%d", dwRes);
                // Example of using SetGainScheduling()
                dwRes = SetGainScheduling(pDemoAxis->wDriveFixAddress, s_wGain);
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: SetGainScheduling() error dwRes=%d", dwRes);
                // Example of using SetUserInteger():
                // store in slot 1 gain value for moving in negative direction
                dwRes = SetUserInteger(pDemoAxis->wDriveFixAddress, 1, s_wGain);
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: SetUserInteger() error dwRes=%d", dwRes);
                // Example of using SetUserFloat()
                // store in slot 1 actual position
                dwRes = SetUserFloat(pDemoAxis->wDriveFixAddress, 1, (MC_T_REAL)(*pDemoAxis->plPdActualPosition));
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: SetUserFloat() error dwRes=%d", dwRes);
                
                // Example of using GetUserInteger():
                // recall from slot 2 gain value for moving in positive direction
                dwRes = GetUserInteger(pDemoAxis->wDriveFixAddress, 2, &dwInt);
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: GetUserInteger() error dwRes=%d", dwRes);
                // Example of using GetUserFloat()
                // recall stored position from slot 2
                dwRes = GetUserFloat(pDemoAxis->wDriveFixAddress, 2, &fFloat);
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: GetUserFloat() error dwRes=%d", dwRes);
            }
            else
            {
                dwRes = SetSmoothFactor(pDemoAxis->wDriveFixAddress, s_dwSmooth + 20);
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: SetSmoothFactor() error dwRes=%d", dwRes);
                dwRes = SetGainScheduling(pDemoAxis->wDriveFixAddress, s_wGain ^ 0xffff);
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: SetGainScheduling() error dwRes=%d", dwRes);
                // store in slot 2 gain value for moving in positive direction
                dwRes = SetUserInteger(pDemoAxis->wDriveFixAddress, 2, s_wGain ^ 0xffff);
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: SetUserInteger() error dwRes=%d", dwRes);
                // store in slot 2 actual position
                dwRes = SetUserFloat(pDemoAxis->wDriveFixAddress, 2, (MC_T_REAL)(*pDemoAxis->plPdActualPosition));
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: SetUserFloat() error dwRes=%d", dwRes);

                // recall from slot 1 gain value for moving in negative direction
                dwRes = GetUserInteger(pDemoAxis->wDriveFixAddress, 1, &dwInt);
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: GetUserInteger() dwRes=%d", dwRes);
                // recall stored position from slot 1
                dwRes = GetUserFloat(pDemoAxis->wDriveFixAddress, 1, &fFloat);
                if (dwRes != EC_E_NOERROR)
                    LogMsg("Axis 1: GetUserFloat() dwRes=%d", dwRes);
            }
#endif

            // Start axis relative
            pDemoAxis->pFb->MoveRelative.Execute = EC_TRUE;

            s_eMotionState = 2; /* go to state "pause" */

            break;

        case 2:         /* pause */
            if (pDemoAxis->pFb->MoveRelative.Execute)
            {
                if (pDemoAxis->pFb->MoveRelative.Error)
                {
                    LogError("ERROR Axis 1: %s Code=%d", pDemoAxis->wDriveFixAddress,
                        MC_GetErrorText(pDemoAxis->pFb->MoveRelative.ErrorID), pDemoAxis->pFb->MoveRelative.ErrorID);
                    s_eMotionState = 0;                     /* go to state "pause" */
                }

                if (pDemoAxis->pFb->MoveRelative.Done
                    || pDemoAxis->pFb->MoveRelative.Error
                    || pDemoAxis->pFb->MoveRelative.CommandAborted)      /* move done or aborted ? */
                {
                    pDemoAxis->pFb->MoveRelative.Execute = EC_FALSE;
                }


                if (pDemoAxis->pFb->MoveRelative.Done)
                {
                    pDemoAxis->pFb->MoveRelative.Distance = -pDemoAxis->pFb->MoveRelative.Distance;
                    s_eMotionState = 1; /* go to state "move" */
                }

                if ((pDemoAxis->pFb->MoveRelative.Error)
                    || (pDemoAxis->pFb->MoveRelative.CommandAborted))      /* move done or aborted ? */
                {
                    s_eMotionState = 0;                     /* go to state "pause" */
                }
            }

            break;
        } /* switch(s_eMotionState) */
    }
    else
    {
        pDemoAxis->pFb->MoveRelative.Execute = EC_FALSE;
        s_eMotionState = 0; /* go to state "setup" */
    }

    //OsSleep(1);

    return EC_E_NOERROR;
}
#endif

static void ConvertInPara(MC_T_CMD_INP_PARA *p)
{
   MC_SET_DWORD(&p->dwCmdCode, p->dwCmdCode);
   MC_SET_DWORD(&p->dwSlaveAddress, p->dwSlaveAddress);
   MC_SET_DWORD(&p->dwIncPerMM, p->dwIncPerMM);
   MC_SET_DWORD(&p->dwDirection, p->dwDirection);
   MC_SET_REAL(&p->fVel, p->fVel);
   MC_SET_REAL(&p->fAcc, p->fAcc);
   MC_SET_REAL(&p->fDec, p->fDec);
   MC_SET_REAL(&p->fJerk, p->fJerk);
   MC_SET_REAL(&p->fDistance, p->fDistance);
}

static void ConvertInParaV2(MC_T_CMD_INP_PARA_V2 *p)
{
   ConvertInPara((MC_T_CMD_INP_PARA *) p);
   MC_SET_DWORD(&p->dwContUpdate, p->dwContUpdate);
   MC_SET_DWORD(&p->dwParaNumber, p->dwParaNumber);
   MC_SET_REAL(&p->fParaValue, p->fParaValue);
   MC_SET_DWORD((EC_T_DWORD *) &p->bParaValue, p->bParaValue);
}

/********************************************************************************/
/** \brief  Handler for application notifications
*
*  !!! No blocking API shall be called within this function!!! 
*  !!! Function is called by cylic task                    !!! 
*
* \return  Status value.
*/
static EC_T_DWORD myAppNotify(
    EC_T_DWORD              dwCode,     /* [in]  Application notification code */
    EC_T_NOTIFYPARMS*       pParms      /* [in]  Notification parameters */
    )
{
   EC_T_DWORD             dwStartCycleCnt = S_dwJobTaskCycleCnt;      /* for synchronisation with job task */

   MC_T_CMD_INP_PARA_V2   oMcInpParaV2; // Aligned to 8 byte address (for double access)
   MC_T_CMD_OUT_PARA_V2   oMcOutParaV2; // Aligned to 8 byte address (for double access)

   MC_T_CMD_OUT_PARA_V1*  pMcOutParaV1 = (MC_T_CMD_OUT_PARA_V1 *) &oMcOutParaV2;
   MC_T_CMD_OUT_PARA_V2*  pMcOutParaV2 = &oMcOutParaV2;

   OsMemset(&oMcInpParaV2, 0, sizeof(oMcInpParaV2));
   OsMemset(&oMcOutParaV2, 0, sizeof(oMcOutParaV2));

   /* dispatch notification code */
   switch(dwCode)
   {
   case MC_CMD_CODE_POWER_ON:          /* Power-On */
   case MC_CMD_CODE_POWER_OFF:         /* Power-Off */
   case MC_CMD_CODE_STOP:              /* Stop */
   case MC_CMD_CODE_RESET:             /* Reset */
   case MC_CMD_CODE_HALT:              /* Halt */
   case MC_CMD_CODE_MOVE_RELATIVE:     /* Move Relative */
   case MC_CMD_CODE_MOVE_ABSOULTE:     /* Move Absolute */
   case MC_CMD_CODE_MOVE_VELOCITY:     /* Move Velocity */
   case MC_CMD_WRITE_PARAMETER:    
   case MC_CMD_WRITE_BOOL_PARAMETER:
// Homing supported only in PP mode
#ifdef EC_MOTION_SUPPORT_PP_MODE
   case MC_CMD_HOME:                    /* Home */
#endif
   {
       if ((pParms->pbyInBuf != EC_NULL) && (pParms->dwInBufSize == sizeof(MC_T_CMD_INP_PARA)))
       {
           OsMemcpy(&oMcInpParaV2, pParms->pbyInBuf, sizeof(MC_T_CMD_INP_PARA)); // Make an aligned copy
           ConvertInPara((MC_T_CMD_INP_PARA *)&oMcInpParaV2);
       }
       else if ((pParms->pbyInBuf != EC_NULL) && (pParms->dwInBufSize == sizeof(MC_T_CMD_INP_PARA_V2)))
       {
           OsMemcpy(&oMcInpParaV2, pParms->pbyInBuf, sizeof(MC_T_CMD_INP_PARA_V2)); // Make an aligned copy
           ConvertInParaV2(&oMcInpParaV2);
       }
       else
       {
           LogError("ERROR in myAppNotify: Size of input parameters are not matching: Excepted=%d != dwInBufSize=%d",
               sizeof(MC_T_CMD_INP_PARA), pParms->dwInBufSize);
           LogError("ERROR in myAppNotify: Size of input parameters are not matching: Excepted=%d != dwInBufSize=%d",
               sizeof(MC_T_CMD_INP_PARA_V2), pParms->dwInBufSize);
           break;
       }
// Homing supported only in PP mode
#ifdef EC_MOTION_SUPPORT_PP_MODE
       if (dwCode == MC_CMD_HOME)
       {
           EC_T_DEMO_AXIS *pDemoAxis = myAppSearchAxis(oMcInpParaV2.dwStationAddress);

           /* set homing parameters */
           EC_T_DWORD dwRetVal = pDemoAxis->pFb->Axis.SetHomingParameters(HOMING_MODE,
               HOMING_SPEED_SEARCH_SWITCH,
               HOMING_SPEED_SEARCH_ZERO,
               HOMING_ACCELERATION,
               HOMING_OFFSET);
           if (EC_E_NOERROR != dwRetVal)
           {
               LogError((EC_T_CHAR*)"Axis 1: Cannot set homing parameters for the drive (Result = %s 0x%x)!", ecatGetText(dwRetVal), dwRetVal);
           }
       }
#endif
       S_pPendMotionCmdFifo->Add(oMcInpParaV2);

       break;
   }
   case MC_CMD_CODE_READ_POS:     /* read actual position */
      {
         if ((pParms->pbyInBuf == EC_NULL)
            || (pParms->dwInBufSize < sizeof(MC_T_CMD_INP_PARA)) 
            || (pParms->pbyOutBuf == EC_NULL))
         {
            LogError("ERROR in myAppNotify: Size of parameters are not matching");
            break;
         }

         OsMemcpy(&oMcInpParaV2, pParms->pbyInBuf, sizeof(MC_T_CMD_INP_PARA)); // Make an aligned copy
         ConvertInPara((MC_T_CMD_INP_PARA *) &oMcInpParaV2);

         EC_T_DEMO_AXIS *pDemoAxis = myAppSearchAxis(oMcInpParaV2.dwStationAddress);

         while (S_dwJobTaskCycleCnt - dwStartCycleCnt < 1); // Wait at least one cycle

         if ((pDemoAxis != EC_NULL) && (pParms->dwOutBufSize == sizeof(MC_T_CMD_OUT_PARA_V1)))
         {
            MC_SET_REAL(&pMcOutParaV1->fPosition, pDemoAxis->pFb->ReadActualPosition.Position);
            *pParms->pdwNumOutData = sizeof(MC_T_CMD_OUT_PARA_V1);
         }
         else if ((pDemoAxis != EC_NULL) && (pParms->dwOutBufSize == sizeof(MC_T_CMD_READ_POS)))
         {
            MC_SET_REAL(&pMcOutParaV2->oReadPos.fPosition, pDemoAxis->pFb->ReadActualPosition.Position);
            MC_SET_WORD(&pMcOutParaV2->oReadPos.wProfileState, (EC_T_WORD) pDemoAxis->pFb->ReadAxisInfo.DriveState);
            MC_SET_WORD(&pMcOutParaV2->oReadPos.ePLCOpenState, (EC_T_WORD) pDemoAxis->pFb->ReadAxisInfo.PLCOpenState);
            pMcOutParaV2->oReadPos.wStatusWord = EC_GETWORD(pDemoAxis->pwPdStatusWord); // Already little endian
            pMcOutParaV2->oReadPos.wControlWord = EC_GETWORD(pDemoAxis->pwPdControlWord); // Already little endian
            MC_SET_SDWORD(&pMcOutParaV2->oReadPos.lActualPosition, (EC_T_INT) (pDemoAxis->pFb->ReadActualPosition.Position * pDemoAxis->dwDriveIncPerMM));
            MC_SET_SDWORD(&pMcOutParaV2->oReadPos.lTargetPosition, (EC_T_INT) (pDemoAxis->pFb->ReadAxisInfo.CommandedPosition * pDemoAxis->dwDriveIncPerMM));
            *pParms->pdwNumOutData = sizeof(MC_T_CMD_READ_POS);
         }
         else
         {
            LogError("ERROR in myAppNotify: Size of output parameters are not matching");
         }

         break;
      }
   case MC_CMD_READ_PARAMETER:
   case MC_CMD_READ_BOOL_PARAMETER:
      {
         if ((pParms->pbyInBuf == EC_NULL) 
            || (pParms->dwInBufSize != sizeof(MC_T_CMD_INP_PARA_V2)) 
            || (pParms->pbyOutBuf == EC_NULL) 
            || (pParms->dwOutBufSize != sizeof(MC_T_CMD_READ_PARA)))
         {
            LogError("ERROR in myAppNotify: Size of parameters are not matching: Excepted=%d != dwOutBufSize=%d",
               sizeof(MC_T_CMD_READ_PARA), pParms->dwOutBufSize);
            break;
         }

         OsMemcpy(&oMcInpParaV2, pParms->pbyInBuf, sizeof(MC_T_CMD_INP_PARA_V2)); // Make an aligned copy
         ConvertInParaV2(&oMcInpParaV2);

         S_pPendMotionCmdFifo->Add(oMcInpParaV2);

         EC_T_DEMO_AXIS *pDemoAxis = myAppSearchAxis(oMcInpParaV2.dwStationAddress);
         if (pDemoAxis == EC_NULL) break;

         while (S_dwJobTaskCycleCnt - dwStartCycleCnt < 1); // Wait at least one cycle

         if (dwCode == MC_CMD_READ_PARAMETER)
         {
            MC_SET_DWORD(&pMcOutParaV2->oReadPara.dwParaNumber, (EC_T_DWORD) pDemoAxis->pFb->ReadParameter.ParameterNumber);
            MC_SET_REAL(&pMcOutParaV2->oReadPara.fParaValue, pDemoAxis->pFb->ReadParameter.Value);
            MC_SET_DWORD((EC_T_DWORD *) &pMcOutParaV2->oReadPara.bValid, pDemoAxis->pFb->ReadParameter.Valid);
            MC_SET_DWORD((EC_T_DWORD *) &pMcOutParaV2->oReadPara.bError, pDemoAxis->pFb->ReadParameter.Error);
            MC_SET_WORD(&pMcOutParaV2->oReadPara.wErrorID, pDemoAxis->pFb->ReadParameter.ErrorID);
         }
         else
         {
            MC_SET_DWORD(&pMcOutParaV2->oReadPara.dwParaNumber, (EC_T_DWORD) pDemoAxis->pFb->ReadBoolParameter.ParameterNumber);
            MC_SET_DWORD((EC_T_DWORD *) &pMcOutParaV2->oReadPara.bParaValue, pDemoAxis->pFb->ReadBoolParameter.Value);
            MC_SET_DWORD((EC_T_DWORD *) &pMcOutParaV2->oReadPara.bValid, pDemoAxis->pFb->ReadBoolParameter.Valid);
            MC_SET_DWORD((EC_T_DWORD *) &pMcOutParaV2->oReadPara.bError, pDemoAxis->pFb->ReadBoolParameter.Error);
            MC_SET_WORD(&pMcOutParaV2->oReadPara.wErrorID, pDemoAxis->pFb->ReadBoolParameter.ErrorID);
         }
         *pParms->pdwNumOutData = sizeof(MC_T_CMD_READ_PARA);

         break;
      }
   } // switch

   if (pParms->pbyOutBuf != EC_NULL)
   {
      OsMemcpy(pParms->pbyOutBuf, &oMcOutParaV2, *pParms->pdwNumOutData); // Copy aligned data to unaligned buffer
   }

   return EC_E_NOERROR;
}



/********************************************************************************/
/** \brief  Search axis in list
*
*
* \return  Pointer to axis or EC_NULL if not found
*/
static EC_T_DEMO_AXIS* myAppSearchAxis(
    EC_T_DWORD              dwSlaveAddress
    )
{
   EC_T_WORD wStationAddress = (EC_T_WORD) (dwSlaveAddress & 0xFFFF);
   EC_T_WORD wAxIdx = (EC_T_WORD) (dwSlaveAddress >> 16);

   int axCnt = 0;
   int i;
   for (i = 0; i < S_numAxes; ++i) 
   {
      EC_T_DEMO_AXIS *pDemoAxis = &S_aAxisList[i];
      if (pDemoAxis->wDriveFixAddress == wStationAddress) axCnt++;
      if (axCnt == wAxIdx + 1) return pDemoAxis;
   }

   return EC_NULL;
}

#ifdef MOTION_ENABLE_LOGFILE
/********************************************************************************************/
/**
\brief  Initialization motion log file

\return EC_E_NOERROR on success, error code otherwise.
*/
static EC_T_DWORD motionLogEnable
    (CAtEmLogging*				poLogging,				/* logging object */
	 EC_T_DWORD                 dwInstanceID,           /* Master Instance ID */
     EC_T_BYTE*                 pbyLogBuffer,
	 EC_T_DWORD                 dwLogNumMsgs,
	 EC_T_DWORD                 dwMotionLogBufSize,
     EC_T_CHAR*                 szFilenamePrefix
     )
{
   MCLOG_DESC* pLogDesc;
   EC_T_DWORD  dwRes = EC_E_NOERROR;
   EC_T_CHAR   szLogFilename[256];

   if ((EC_NULL != szFilenamePrefix) && ('\0' != szFilenamePrefix[0]))
   {
      OsSnprintf(szLogFilename, sizeof(szLogFilename) - 1, "%s_%s", szFilenamePrefix, MOTION_DATALOGGERFILE);
   }
   else
   {
      OsSnprintf(szLogFilename, sizeof(szLogFilename) - 1, "%s", MOTION_DATALOGGERFILE);
   }

   pLogDesc = &S_McLogDesc[dwInstanceID];
   pLogDesc->poLogging = poLogging;
   pLogDesc->pvLogBufDesc = poLogging->AddLogBuffer(
      dwInstanceID,
      MOTION_DATALOGGERROLLOVERLIMIT,
      dwLogNumMsgs,
      EC_FALSE,                           /* do not skip duplicates */
      (EC_T_CHAR*)"Motion",               /* name of the logging (identification) */
      szLogFilename,
      MOTION_DATALOGGERFILEEXTENSION,     /* log file extension */
      EC_FALSE,                           /* print message on console? */
      EC_FALSE );                         /* logging with time stamp? */

   if( pLogDesc->pvLogBufDesc == EC_NULL )
   {
      OsDbgMsg( "motionLogEnable: ERROR adding new log buffer\n" ); 
      dwRes = EC_E_NOMEMORY;
      goto Exit;
   }
   if( pbyLogBuffer != EC_NULL && dwMotionLogBufSize != 0 )
   {
      poLogging->SetMsgBuf( (MSG_BUFFER_DESC*)pLogDesc->pvLogBufDesc, pbyLogBuffer, dwMotionLogBufSize );
   }

   motionLogWriteRowHeader(dwInstanceID);

   S_motionLogEnabled = true;

Exit:
   return dwRes;
}

/********************************************************************************/
/** \brief Insert a new message into message buffer
*
* \return N/A
*/
static EC_T_VOID motionLogMsg
    (EC_T_DWORD                 dwInstanceID                   /* Master Instance ID */
    ,EC_T_CHAR*                 szFormat, ...)
{
   MCLOG_DESC* pLogDesc;
   EC_T_VALIST vaArgs;


   EC_VASTART(vaArgs, szFormat);
   pLogDesc = &S_McLogDesc[dwInstanceID];
   if( pLogDesc->pvLogBufDesc != EC_NULL )
   {
      pLogDesc->poLogging->InsertNewMsg( pLogDesc->pvLogBufDesc, szFormat, vaArgs, EC_TRUE, EC_FALSE );
   }

   /* important note: on VxWorks 6.1 PowerPC: re-using vaArgs in the next function call may fail without calling EC_VAEND/EC_VASTART!! */
   EC_VAEND(vaArgs);
   EC_VASTART(vaArgs, szFormat);

   return;
}

/* Calculate actual vel, acc, jerk, lag
 */
static void motionLogCalcMotionDerivatives(EC_T_DEMO_AXIS *pDemoAxis)
{
   MC_T_AXIS_REF *pAxis = &pDemoAxis->pFb->Axis;

   MC_T_REAL pos = pAxis->lCommandedPosition / (MC_T_REAL)pAxis->dwCalcIncPerMM;
   MC_T_REAL vel = 0.0, acc = 0.0, jerk = 0.0;

   vel = pAxis->oMove.lCommandVel / pAxis->fVelToInc;
   if (pAxis->dwDirection == MC_DIR_NEGATIVE) vel = -vel;

   acc = pAxis->oMove.lCommandAcc / pAxis->fAccToInc;
   jerk = pAxis->oMove.lCommandJerk / pAxis->fJerkToInc;

   pDemoAxis->log.curPos.fCommandPos = pos;
   pDemoAxis->log.curPos.fCommandVel = vel;
   pDemoAxis->log.curPos.fCommandAcc = acc;
   pDemoAxis->log.curPos.fCommandJerk = jerk;
   pDemoAxis->log.curPos.fFollowingError = pos - (pAxis->lActualPosition / (MC_T_REAL)pAxis->dwCalcIncPerMM);
}

static char* motionLogFormatFp(double val, char *buf, int buflen)
{
   int a = (int) (val * 1000.0 + 0.5);
   OsSnprintf(buf, buflen, "%c%d,%03d",
      (a < 0 ? '-' : ' '),
      abs(a / 1000), 
      abs(a % 1000));
   buf[buflen - 1] = '\0';
   return buf;
}

static void motionLogWriteRowHeader(EC_T_DWORD dwInstanceID)
{
   char buffy[LOG_BUFSIZE];
   int cnt = OsSnprintf(buffy, LOG_BUFSIZE, "Time [ms]; ");
   
   for (int i = 0; i < S_numAxes; ++i) 
   {
      int ax = i + 1;
      cnt += OsSnprintf(buffy + cnt, LOG_BUFSIZE - cnt, 
         "Ax%d Pos; Ax%d Vel; Ax%d Acc; Ax%d Jer; Ax%d FollowErr; ", ax, ax, ax, ax, ax);
   }

   buffy[LOG_BUFSIZE - 1] = '\0';
   motionLogMsg(dwInstanceID, buffy);
}

static void motionLogPos(EC_T_DWORD dwInstanceID, EC_T_DWORD dwBusCycleTimeUsec)
{
   if (!S_motionLogEnabled || !S_dwMotionStartCycle) return;

   EC_T_DWORD motionCycle = S_dwJobTaskCycleCnt - S_dwMotionStartCycle;
   char buffy[LOG_BUFSIZE];
   int cnt = OsSnprintf(buffy, LOG_BUFSIZE, "%d;", motionCycle * dwBusCycleTimeUsec / 1000);

   int i;
   for (i = 0; i < S_numAxes; ++i) 
   {
      EC_T_DEMO_AXIS *pDemoAxis = &S_aAxisList[i];

      if (pDemoAxis->pFb->Power.Status)
      {
         motionLogCalcMotionDerivatives(pDemoAxis);

         char fmtBuf[32];

         /* actual position */
         cnt += OsSnprintf(buffy + cnt, LOG_BUFSIZE - cnt, "%s;", motionLogFormatFp(pDemoAxis->log.curPos.fCommandPos, fmtBuf, sizeof(fmtBuf)));
         
         /* commanded velocity */
         cnt += OsSnprintf(buffy + cnt, LOG_BUFSIZE - cnt, "%s;", motionLogFormatFp(pDemoAxis->log.curPos.fCommandVel, fmtBuf, sizeof(fmtBuf)));
         
         /* commanded acc */
         cnt += OsSnprintf(buffy + cnt, LOG_BUFSIZE - cnt, "%s;", motionLogFormatFp(pDemoAxis->log.curPos.fCommandAcc, fmtBuf, sizeof(fmtBuf)));

         /* commanded jerk */
         cnt += OsSnprintf(buffy + cnt, LOG_BUFSIZE - cnt, "%s;", motionLogFormatFp(pDemoAxis->log.curPos.fCommandJerk, fmtBuf, sizeof(fmtBuf)));
         
         /* following error */
         cnt += OsSnprintf(buffy + cnt, LOG_BUFSIZE - cnt, "%s;", motionLogFormatFp(pDemoAxis->log.curPos.fFollowingError, fmtBuf, sizeof(fmtBuf)));
      }
      else
      {
         cnt += OsSnprintf(buffy + cnt, LOG_BUFSIZE - cnt, ";;;;;");
      }
   }

   buffy[LOG_BUFSIZE - 1] = '\0';
   motionLogMsg(dwInstanceID, buffy);
}
#endif

#ifdef ELMO_DRIVE
/***************************************************************************************************/
/**
* \brief    Sets gain scheduling
*
* \detail   An object 0x2E00:00 will be written. May not be called within tEcJobTask.
*
* \param    wStationAddress     EtherCAT station address
* \param    wGainValue          Gain value
*
* \return   EC_E_NOERROR or an error code, i.e. EC_E_SLAVE_NOT_ADDRESSABLE if a slave
*           with station address not found
*/
MC_T_DWORD SetGainScheduling(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_WORD       wValue              /**< [in] Gain value */
    )
{
    EC_T_DWORD  dwRet = EC_E_NOERROR;
    EC_T_DWORD  dwSlaveId = ecatGetSlaveId(wStationAddress);
    EC_T_WORD   wTmp = wValue;

    if (dwSlaveId != INVALID_SLAVE_ID)
        dwRet = ecatCoeSdoDownload(dwSlaveId, DRV_OBJ_GAIN_SCHEDULING, 0,
                    (EC_T_BYTE *)&wTmp, sizeof(wTmp), EC_SDO_UP_DOWN_TIMEOUT, 0);
    else
        dwRet = EC_E_SLAVE_NOT_ADDRESSABLE;

    return dwRet;
}

/***************************************************************************************************/
/**
* \brief    Sets smooth factor
*
* \detail   An object 0x31D9:01 will be written. May not be called within tEcJobTask.
*
* \param    wStationAddress     EtherCAT station address
* \param    wValue              Smooth factor
*
* \return   EC_E_NOERROR or an error code, i.e. EC_E_SLAVE_NOT_ADDRESSABLE if a slave
*           with station address not found
*/
MC_T_DWORD SetSmoothFactor(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_DWORD      dwValue             /**< [in] Smooth factor value */
    )
{
    EC_T_DWORD  dwRet = EC_E_NOERROR;
    EC_T_DWORD  dwSlaveId = ecatGetSlaveId(wStationAddress);
    EC_T_DWORD  dwTmp = dwValue;

    if (dwSlaveId != INVALID_SLAVE_ID)
        dwRet = ecatCoeSdoDownload(dwSlaveId, DRV_OBJ_SMOOTH_FACTOR, 1,
                    (EC_T_BYTE *)&dwTmp, sizeof(dwTmp), EC_SDO_UP_DOWN_TIMEOUT, 0);
    else
        dwRet = EC_E_SLAVE_NOT_ADDRESSABLE;

    return dwRet;
}

/***************************************************************************************************/
/**
* \brief    Sets an integer value in general pupose array
*
* \detail   An object 0x2F00 will be written. May not be called within tEcJobTask.
*
* \param    wStationAddress     EtherCAT station address
* \param    bySubIndex          Subindex 1..24
* \param    dwValue             INT32 value
*
* \return   EC_E_NOERROR or an error code, i.e. EC_E_SLAVE_NOT_ADDRESSABLE if a slave
*           with station address not found
*/
MC_T_DWORD SetUserInteger(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_BYTE       bySubIndex,         /**< [in] Subindex 1..24 */
    MC_T_DWORD      dwValue             /**< [in] Smooth factor value */
    )
{
    EC_T_DWORD  dwRet = EC_E_NOERROR;
    EC_T_DWORD  dwSlaveId = ecatGetSlaveId(wStationAddress);
    EC_T_DWORD  dwTmp = dwValue;

    if ((bySubIndex < 1) || (bySubIndex > 24))
        return EC_E_INVALIDPARM;

    if (dwSlaveId != INVALID_SLAVE_ID)
        dwRet = ecatCoeSdoDownload(dwSlaveId, DRV_OBJ_USER_INT, bySubIndex,
                    (EC_T_BYTE *)&dwTmp, sizeof(dwTmp), EC_SDO_UP_DOWN_TIMEOUT, 0);
    else
        dwRet = EC_E_SLAVE_NOT_ADDRESSABLE;

    return dwRet;
}

/***************************************************************************************************/
/**
* \brief    Gets an integer value from general pupose array
*
* \detail   An object 0x2F00 will be used. This function may not be called within EC main cyclic task.
*
* \param    wStationAddress     EtherCAT station address
* \param    bySubIndex          Subindex 1..24
* \param    pdwValue            Pointer to buffer
*
* \return   EC_E_NOERROR or an error code, i.e. EC_E_SLAVE_NOT_ADDRESSABLE if a slave
*           with station address not found
*/
MC_T_DWORD GetUserInteger(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_BYTE       bySubIndex,         /**< [in] Subindex 1..24 */
    MC_T_DWORD*     pdwValue            /**< [in] Pointer to buffer */
    )
{
    EC_T_DWORD  dwRet = EC_E_NOERROR;
    EC_T_DWORD  dwSlaveId = ecatGetSlaveId(wStationAddress);
    EC_T_DWORD  dwBytesLoaded = 0;
    EC_T_DWORD  dwTmp = 0;

    if (pdwValue == EC_NULL)
        return EC_E_INVALIDPARM;

    if ((bySubIndex < 1) || (bySubIndex > 24))
        return EC_E_INVALIDPARM;

    if (dwSlaveId != INVALID_SLAVE_ID)
        dwRet = ecatCoeSdoUpload(dwSlaveId, DRV_OBJ_USER_INT, bySubIndex,
                    (EC_T_BYTE *)&dwTmp, sizeof(dwTmp), &dwBytesLoaded, EC_SDO_UP_DOWN_TIMEOUT, 0);
    else
        dwRet = EC_E_SLAVE_NOT_ADDRESSABLE;

    *pdwValue = dwTmp;

    return dwRet;
}

/***************************************************************************************************/
/**
* \brief    Sets an float value in general pupose array
*
* \detail   An object 0x2F01 will be written. May not be called within tEcJobTask.
*
* \param    wStationAddress     EtherCAT station address
* \param    bySubIndex          Subindex 1..24
* \param    fValue              FLOAT value
*
* \return   EC_E_NOERROR or an error code, i.e. EC_E_SLAVE_NOT_ADDRESSABLE if a slave
*           with station address not found
*/
MC_T_DWORD SetUserFloat(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_BYTE       bySubIndex,         /**< [in] Subindex 1..24 */
    MC_T_REAL       fValue              /**< [in] User float value */
    )
{
    EC_T_DWORD  dwRet = EC_E_NOERROR;
    EC_T_DWORD  dwSlaveId = ecatGetSlaveId(wStationAddress);
    float       fTmp = (float)fValue;

    if ((bySubIndex < 1) || (bySubIndex > 24))
        return EC_E_INVALIDPARM;

    if (dwSlaveId != INVALID_SLAVE_ID)
        dwRet = ecatCoeSdoDownload(dwSlaveId, DRV_OBJ_USER_FLOAT, bySubIndex,
                    (EC_T_BYTE *)&fTmp, sizeof(fTmp), EC_SDO_UP_DOWN_TIMEOUT, 0);
    else
        dwRet = EC_E_SLAVE_NOT_ADDRESSABLE;

    return dwRet;
}

/***************************************************************************************************/
/**
* \brief    Gets an float value from general pupose array
*
* \detail   An object 0x2F01 will be used. This function may not be called within EC main cyclic task.
*
* \param    wStationAddress     EtherCAT station address
* \param    bySubIndex          Subindex 1..24
* \param    pfValue             Pointer to buffer
*
* \return   EC_E_NOERROR or an error code, i.e. EC_E_SLAVE_NOT_ADDRESSABLE if a slave
*           with station address not found
*/
MC_T_DWORD GetUserFloat(
    MC_T_WORD       wStationAddress,    /**< [in] EtherCAT station address */
    MC_T_BYTE       bySubIndex,         /**< [in] Subindex 1..24 */
    MC_T_REAL*      pfValue             /**< [in] Pointer to buffer */
    )
{
    EC_T_DWORD  dwRet = EC_E_NOERROR;
    EC_T_DWORD  dwSlaveId = ecatGetSlaveId(wStationAddress);
    EC_T_DWORD  dwBytesLoaded = 0;
    float       fTmp = 0.0;

    if (pfValue == EC_NULL)
        return EC_E_INVALIDPARM;

    if ((bySubIndex < 1) || (bySubIndex > 24))
        return EC_E_INVALIDPARM;

    if (dwSlaveId != INVALID_SLAVE_ID)
        dwRet = ecatCoeSdoUpload(dwSlaveId, DRV_OBJ_USER_FLOAT, bySubIndex,
                    (EC_T_BYTE *)&fTmp, sizeof(fTmp), &dwBytesLoaded, EC_SDO_UP_DOWN_TIMEOUT, 0);
    else
        dwRet = EC_E_SLAVE_NOT_ADDRESSABLE;

    *pfValue = fTmp;

    return dwRet;
}
#endif
/*-END OF SOURCE FILE--------------------------------------------------------*/
