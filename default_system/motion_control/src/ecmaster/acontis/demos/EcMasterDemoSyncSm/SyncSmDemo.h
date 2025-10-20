/*-----------------------------------------------------------------------------
 * SyncSmDemo.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master demo header
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include "SyncSmDemoConfig.h"
#include "ecatNotification.h"
#include "ecatDemoCommon.h"
#ifdef VXWORKS
#include "wvLib.h"
#endif

/*-MACROS--------------------------------------------------------------------*/

/*-DEFINES-------------------------------------------------------------------*/

#define MAX_LINKLAYER 5

#if (defined EC_SOCKET_SUPPORTED)
/* the RAS server is necessary to support the EC-Engineer or other remote applications */
  #define ATEMRAS_SERVER
#endif
#define REMOTE_WD_TO_LIMIT          10000
#define REMOTE_CYCLE_TIME           2
#define ATDS_CFG_DEFAULT_PORT       ((EC_T_WORD)6000)


/*-TYPEDEFS------------------------------------------------------------------*/

/*-GLOBAL VARIABLES----------------------------------------------------------*/


/*-FORWARD DECLARATIONS------------------------------------------------------*/
EC_T_DWORD SYNCSMDemo(
     CAtEmLogging*       poLog
    ,EC_T_CNF_TYPE       eCnfType
    ,EC_T_PBYTE          pbyCnfData
    ,EC_T_DWORD          dwCnfDataLen
    ,EC_T_DWORD          dwBusCycleTimeUsec
    ,EC_T_INT            nVerbose
    ,EC_T_DWORD          dwDuration
    ,EC_T_LINK_PARMS* poLinkParms
    ,EC_T_VOID*          pvTimingEvent
    ,EC_T_DWORD          dwCpuIndex
    ,EC_T_BOOL           bEnaPerfJobs
#ifdef ATEMRAS_SERVER 
    ,EC_T_WORD           wServerPort
#endif
    ,EC_T_LINK_PARMS* poLinkParmsRed
    );

/*--------------------------------------------------------------------------*/
/* Performance measurements of jobs                                         */
/* This is only available on CPUs with TSC support                          */
/*--------------------------------------------------------------------------*/

#define PERF_RoundTrip          0
#define JOB_SendAllCycFrames    1
#define JOB_MasterTimer         2
#define JOB_SendAcycFrames      3
#define PERF_CycleTime          4
#define PERF_myAppWorkpd        5
#define MAX_JOB_NUM             6

#define PERF_MEASURE_JOBS_INIT(msgcb)   ecatPerfMeasInit(&S_TscMeasDesc,0,MAX_JOB_NUM,msgcb);ecatPerfMeasEnable(&S_TscMeasDesc)
#define PERF_MEASURE_JOBS_DEINIT()      ecatPerfMeasDeinit(&S_TscMeasDesc)
#define PERF_MEASURE_JOBS_SHOW()        ecatPerfMeasShow(&S_TscMeasDesc,0xFFFFFFFF,S_aszMeasInfo)
#define PERF_JOB_START(nJobIndex)       ecatPerfMeasStart(&S_TscMeasDesc,(EC_T_DWORD)(nJobIndex))
#define PERF_JOB_END(nJobIndex)         ecatPerfMeasEnd(&S_TscMeasDesc,(EC_T_DWORD)(nJobIndex))

/*-END OF SOURCE FILE--------------------------------------------------------*/
