/*-----------------------------------------------------------------------------
 * SyncSmDemo.cpp
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master demo application
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include <AtEthercat.h>

#include "SyncSmDemo.h"
#include "Logging.h"

#ifdef ATEMRAS_SERVER 
#include <AtEmRasSrv.h>
#endif

/*-DEFINES-------------------------------------------------------------------*/
#define LogMsg      S_poLog->LogMsg
#define LogError    S_poLog->LogError

/*-LOCAL VARIABLES-----------------------------------------------------------*/
static EC_T_DWORD          S_dwClntId        = 0;
static CAtEmLogging*       S_poLog    = EC_NULL;
static T_DEMO_THREAD_PARAM S_DemoThreadParam = {0};
static EC_T_PVOID          S_pvtJobThread    = EC_NULL;
static EC_T_PVOID          S_pvCycFrameRxEvent = EC_NULL;
#ifdef ATEMRAS_SERVER 
static EC_T_BOOL           S_bRasSrvStarted  = EC_FALSE;
static EC_T_PVOID          S_pvRemoteApiSrvH = EC_NULL;
#endif
static EC_T_BOOL           S_bEnaPerfJobs    = EC_FALSE;
static EC_T_TSC_MEAS_DESC  S_TscMeasDesc;
static EC_T_CHAR*          S_aszMeasInfo[MAX_JOB_NUM] =
{
    (EC_T_CHAR*)"RoundTrip (TX+RX)     ",
    (EC_T_CHAR*)"JOB_SendAllCycFrames  ",
    (EC_T_CHAR*)"JOB_MasterTimer       ",
    (EC_T_CHAR*)"JOB_SendAcycFrames    ",
    (EC_T_CHAR*)"Cycle Time            ",
    (EC_T_CHAR*)"myAppWorkPd           "
};

/*-FORWARD DECLARATIONS------------------------------------------------------*/
static EC_T_VOID  CycFrameReceivedCallback(EC_T_VOID* pvContext);
static EC_T_DWORD ecatNotifyCallback(EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
#if (defined ATEMRAS_SERVER)
static EC_T_DWORD RasNotifyWrapper(EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
#endif
static EC_T_VOID  tEcJobTask(EC_T_VOID* pvThreadParamDesc);

/*-MYAPP---------------------------------------------------------------------*/
/* Demo code: Remove/change this in your application */
static EC_T_DWORD myAppInit     (CAtEmLogging*           poLog, EC_T_INT nVerbose);
static EC_T_DWORD myAppPrepare  (CAtEmLogging*           poLog, EC_T_INT nVerbose);
static EC_T_DWORD myAppSetup    (CAtEmLogging*           poLog, EC_T_INT nVerbose, EC_T_DWORD dwClntId);
static EC_T_DWORD myAppWorkpd   (CAtEmLogging*           poLog, EC_T_INT nVerbose, EC_T_BYTE* pbyPDIn, EC_T_BYTE* pbyPDOut);
static EC_T_DWORD myAppDiagnosis(CAtEmLogging*           poLog, EC_T_INT nVerbose);
static EC_T_DWORD myAppNotify   (EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
/* Demo code: End */

/*-FUNCTION DEFINITIONS------------------------------------------------------*/

/********************************************************************************/
/** \brief  EtherCAT Master demo Application.
*
* This is a EtherCAT Master demo application.
*
* \return  Status value.
*/
EC_T_DWORD SYNCSMDemo(
    CAtEmLogging*       poLog
   ,EC_T_CNF_TYPE       eCnfType                /* [in] Enum type of configuration data provided */
   ,EC_T_PBYTE          pbyCnfData              /* [in] Configuration data */                      
   ,EC_T_DWORD          dwCnfDataLen            /* [in] Length of configuration data in byte */    
   ,EC_T_DWORD          dwBusCycleTimeUsec      /* [in]  bus cycle time in usec */
   ,EC_T_INT            nVerbose                /* [in]  verbosity level */
   ,EC_T_DWORD          dwDuration              /* [in]  test duration in msec (0 = forever) */
   ,EC_T_LINK_PARMS* poLinkParms       /* [in]  pointer to link parameter */
   ,EC_T_VOID*          pvTimingEvent           /* [in]  Timing event handle */
   ,EC_T_DWORD          dwCpuIndex              /* [in]  SMP only: CPU index */
   ,EC_T_BOOL           bEnaPerfJobs            /* [in]  Performance measurement */
#ifdef ATEMRAS_SERVER 
   ,EC_T_WORD           wServerPort             /* [in]   Remote API Server Port */
#endif
   ,EC_T_LINK_PARMS* poLinkParmsRed    /* [in]  Redundancy Link Layer Parameter */
)
{
    EC_T_DWORD       dwRetVal = EC_E_NOERROR;
    EC_T_DWORD       dwRes    = EC_E_NOERROR;
    EC_T_BOOL        bRes     = EC_FALSE;
    CEcTimer         oTimeout;
    CEmNotification* pNotification    = EC_NULL;

    /* store parameters */
    S_poLog = poLog;
    S_bEnaPerfJobs = bEnaPerfJobs;
    S_DemoThreadParam.pvTimingEvent = pvTimingEvent;

    /* check if interrupt mode is selected */
    if (poLinkParms->eLinkMode != EcLinkMode_INTERRUPT)
    {
        dwRetVal = EC_E_INVALIDPARM;
        LogError("Error: Link layer in 'polling' mode is not supported by EcMasterDemoSyncSm. Please select 'interrupt' mode.");
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
        oRemoteApiConfig.dwCycleTime        = REMOTE_CYCLE_TIME;    /* 10 msec for the begin */
        oRemoteApiConfig.dwWDTOLimit        = (REMOTE_WD_TO_LIMIT/REMOTE_CYCLE_TIME); /* WD Timeout after 100*100msec = 10secs */
        oRemoteApiConfig.dwReConTOLimit     = 6000;                 /* Reconnect Timeout after 600*100msec = 60secs + 10secs */
        oRemoteApiConfig.dwMasterPrio       = MAIN_THREAD_PRIO;
        oRemoteApiConfig.dwClientPrio       = MAIN_THREAD_PRIO;
        oRemoteApiConfig.pvNotifCtxt        = pNotification;        /* Notification context */
        oRemoteApiConfig.pfNotification     = RasNotifyWrapper;     /* Notification function for emras Layer */
        oRemoteApiConfig.dwConcNotifyAmount = 100;                  /* for the first pre-allocate 50 Notification spaces */
        oRemoteApiConfig.dwMbxNotifyAmount  = 50;                   /* for the first pre-allocate 20 Notification spaces */
        oRemoteApiConfig.dwMbxUsrNotifySize = 3000;                 /* 1,5K user space for Mailbox Notifications */
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
        dwRes = ecatInitMaster(&oInitParms);
        if (dwRes != EC_E_NOERROR)
        {
            dwRetVal = dwRes;
            LogError("Cannot initialize EtherCAT-Master! (Result = %s 0x%x)", ecatGetText(dwRes), dwRes);
            goto Exit;
        }
    }
    /* create event for "cyclic frame is received" and register RX callback function */
    {
    EC_T_CYCFRAME_RX_CBDESC oCyRxDesc;
    EC_T_IOCTLPARMS         oIoCtlParms;

        S_pvCycFrameRxEvent = OsCreateEvent();

        /* setup callback function which is called after RX */
        OsMemset(&oCyRxDesc, 0, sizeof(EC_T_CYCFRAME_RX_CBDESC));
        oCyRxDesc.pfnCallback       = CycFrameReceivedCallback;
        oCyRxDesc.pCallbackParm     = EC_NULL;

        OsMemset(&oIoCtlParms, 0, sizeof(EC_T_IOCTLPARMS));
        oIoCtlParms.dwInBufSize     = sizeof(oCyRxDesc);
        oIoCtlParms.pbyInBuf        = (EC_T_PBYTE)&oCyRxDesc;
        oIoCtlParms.dwOutBufSize    = 0;
        oIoCtlParms.pbyOutBuf       = EC_NULL;
        oIoCtlParms.pdwNumOutData   = EC_NULL;
        dwRes = ecatIoControl(EC_IOCTL_REGISTER_CYCFRAME_RX_CB, &oIoCtlParms);
        if (EC_E_NOERROR != dwRes)
        {
            dwRetVal = dwRes;
            LogError("ERROR: cannot setup callback for Cyclic frame received %s (0x%lx)", ecatGetText(dwRes), dwRes);
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
    dwRes = myAppPrepare(poLog, nVerbose);
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
        dwRes = myAppSetup(poLog, nVerbose, S_dwClntId);
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

        /*****************************************************************************************/
        /* Demo code: Remove/change this in your application: Do some diagnosis outside job task */
        /*****************************************************************************************/
        myAppDiagnosis(poLog, nVerbose);

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

    OsDeleteEvent(S_pvCycFrameRxEvent);

    return dwRetVal;
}

/********************************************************************************/
/** \brief  This function is called after the cyclic frame is received.
*           (If more than one frame, after the last frame)
* \return N/A
*/
static EC_T_VOID CycFrameReceivedCallback(EC_T_VOID* pvContext)
{

    EC_UNREFPARM(pvContext);

    /* wake-up the "Job Task" to continue */
    OsSetEvent(S_pvCycFrameRxEvent);
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
    EC_T_BOOL            bOverload         = EC_FALSE;
    EC_T_BOOL            bFrameLoss        = EC_FALSE;
    EC_T_BOOL            bOk;

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

        /* Reset "Cyclic frame" received event */
        OsResetEvent(S_pvCycFrameRxEvent);

        /* start performance counter to measure round trip time */
        PERF_JOB_START(PERF_RoundTrip);

        /* write output values from last cycle, by sending all cyclic frames */
        PERF_JOB_START(JOB_SendAllCycFrames);
        dwRes = ecatExecJob(eUsrJob_SendAllCycFrames, EC_NULL);
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes && EC_E_LINK_DISCONNECTED != dwRes)
        {
            LogError("ERROR: ecatExecJob(eUsrJob_SendAllCycFrames,    EC_NULL): %s (0x%lx)", ecatGetText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_SendAllCycFrames);

        /* remove this code when using licensed version */
        if (EC_E_EVAL_EXPIRED == dwRes)
        {
            bRun = EC_FALSE;        /* set shutdown flag */
        }

        /* wait until cyclic frame is received */
        if (dwRes == EC_E_NOERROR)
        {
            /* Wait at most one cycle for frame to detect huge jitter (typically on non-real-time OS). */
            if (OsWaitForEvent(S_pvCycFrameRxEvent, EC_MAX(S_DemoThreadParam.dwBusCycleTimeUsec/1000, 1)) != EC_E_NOERROR)
            {
                /* Wait some more time to detect frame loss. */
                dwRes = OsWaitForEvent(S_pvCycFrameRxEvent, 100);
                switch (dwRes)
                {
                case EC_E_NOERROR:
                    if (!bOverload)
                    {
                        bOverload = EC_TRUE;
                        bFrameLoss = EC_FALSE;
                        PERF_MEASURE_JOBS_SHOW();
                        LogError("Error: System overload: Cycle time too short or huge jitter!");
                    }
                    break;
                case EC_E_TIMEOUT:
                    if (!bFrameLoss)
                    {
                        bFrameLoss = EC_TRUE;
                        bOverload = EC_FALSE;
                        LogError("Not all previously sent frames are received/processed (frame loss)!");
                    }
                    break;
                default:
                    LogError("Error 0x%x: %s when waiting for cyclic frame!", dwRes, ecatGetText(dwRes));
                    break;
                }
            }
            else
            {
                bOverload = EC_FALSE;
                bFrameLoss = EC_FALSE;
            }
        }

        /* stop performance counter to measure round trip time */
        PERF_JOB_END(PERF_RoundTrip);

        /*****************************************************/
        /* Demo code: Remove/change this in your application: Working process data cyclic call */
        /*****************************************************/
        PERF_JOB_START(PERF_myAppWorkpd);
        myAppWorkpd(pDemoThreadParam->pLogInst, pDemoThreadParam->pNotInst->Verbose(), ecatGetProcessImageInputPtr(), ecatGetProcessImageOutputPtr());
        PERF_JOB_END(PERF_myAppWorkpd);

        /* Execute some administrative jobs. No bus traffic is performed by this function */
        PERF_JOB_START(JOB_MasterTimer);
        dwRes = ecatExecJob(eUsrJob_MasterTimer, EC_NULL);
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes)
        {
            LogError("ecatExecJob(eUsrJob_MasterTimer, EC_NULL): %s (0x%lx)", ecatGetText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_MasterTimer);

        /* send queued acyclic EtherCAT frames */
        PERF_JOB_START(JOB_SendAcycFrames);
        dwRes = ecatExecJob(eUsrJob_SendAcycFrames, EC_NULL);
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes && EC_E_LINK_DISCONNECTED != dwRes)
        {
            LogError("ecatExecJob(eUsrJob_SendAcycFrames, EC_NULL): %s (0x%lx)", ecatGetText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_SendAcycFrames);

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
#define MBX_TIMEOUT                         5000

#define EL4132_INDEX_USER_SCALE             0x40A2
#define EL4132_SUBINDEX_USRSCL_NUMELEM         0
#define EL4132_SUBINDEX_USRSCL_OFFSET          1
#define EL4132_SUBINDEX_USRSCL_GAIN            2

#define DEMO_MAX_NUM_OF_SLAVES  5
#define SLAVE_NOT_FOUND         0xFFFF
static EC_T_DWORD               S_dwAppFoundSlaves = 0;
static EC_T_CFG_SLAVE_INFO      S_aSlaveList[DEMO_MAX_NUM_OF_SLAVES];
static EC_T_DWORD               S_dwSlaveIdx14        = SLAVE_NOT_FOUND;
static EC_T_DWORD               S_dwSlaveIdx24        = SLAVE_NOT_FOUND;
static EC_T_DWORD               S_dwSlaveIdx4132      = SLAVE_NOT_FOUND;
static EC_T_DWORD               S_dwSlaveIdxETCio100  = SLAVE_NOT_FOUND;

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

    S_dwAppFoundSlaves = 0;
    OsMemset(S_aSlaveList, 0, DEMO_MAX_NUM_OF_SLAVES*sizeof(EC_T_CFG_SLAVE_INFO));
    S_dwSlaveIdx14     = SLAVE_NOT_FOUND;
    S_dwSlaveIdx24     = SLAVE_NOT_FOUND;
    S_dwSlaveIdx4132   = SLAVE_NOT_FOUND;
    S_dwSlaveIdxETCio100 = SLAVE_NOT_FOUND;

    return EC_E_NOERROR;
}

/***************************************************************************************************/
/**
\brief  Initialize Slave Instance.

Find slave parameters.
\return EC_E_NOERROR on success, error code otherwise.
*/
static EC_T_DWORD myAppPrepare(
    CAtEmLogging*       poLog,          /* [in]  Logging instance */ 
    EC_T_INT            nVerbose        /* [in]  Verbosity level */
    )
{
EC_T_WORD wFixedAddress = 0;

    EC_UNREFPARM(nVerbose);

    /* Searching for: EL1004, EL1012, EL1014                                       */
    /* search for the first device at the bus and return its fixed (EtherCAT) address */
    if (FindSlaveGetFixedAddr(INSTANCE_MASTER_DEFAULT, poLog, 0, ecvendor_beckhoff, ecprodcode_beck_EL1004, &wFixedAddress))
    {
        S_dwSlaveIdx14 = S_dwAppFoundSlaves;
    }
    else if (FindSlaveGetFixedAddr(INSTANCE_MASTER_DEFAULT, poLog, 0, ecvendor_beckhoff, ecprodcode_beck_EL1012, &wFixedAddress))
    {
        S_dwSlaveIdx14 = S_dwAppFoundSlaves;
    }
    else if (FindSlaveGetFixedAddr(INSTANCE_MASTER_DEFAULT, poLog, 0, ecvendor_beckhoff, ecprodcode_beck_EL1014, &wFixedAddress))
    {
        S_dwSlaveIdx14 = S_dwAppFoundSlaves;
    }
    if (S_dwSlaveIdx14 != SLAVE_NOT_FOUND)
    {
        S_dwAppFoundSlaves++;

        /* now get the offset of this device in the process data buffer and some other infos */
        if (ecatGetCfgSlaveInfo(EC_TRUE, wFixedAddress, &S_aSlaveList[S_dwSlaveIdx14]) != EC_E_NOERROR)
        {
            LogError("ERROR: ecatGetCfgSlaveInfo() returns with error.");
        }
    }
    /* Searching for: EL2002, EL2004, EL2008                                       */
    /* search for the first device at the bus and return its fixed (EtherCAT) address */
    if (FindSlaveGetFixedAddr(INSTANCE_MASTER_DEFAULT, poLog, 0, ecvendor_beckhoff, ecprodcode_beck_EL2008, &wFixedAddress))
    {
        S_dwSlaveIdx24 = S_dwAppFoundSlaves;
    }
    else if (FindSlaveGetFixedAddr(INSTANCE_MASTER_DEFAULT, poLog, 0, ecvendor_beckhoff, ecprodcode_beck_EL2004, &wFixedAddress))
    {
        S_dwSlaveIdx24 = S_dwAppFoundSlaves;
    }
    else if (FindSlaveGetFixedAddr(INSTANCE_MASTER_DEFAULT, poLog, 0, ecvendor_beckhoff, ecprodcode_beck_EL2002, &wFixedAddress))
    {
        S_dwSlaveIdx24 = S_dwAppFoundSlaves;
    }
    if (S_dwSlaveIdx24 != SLAVE_NOT_FOUND)
    {
        S_dwAppFoundSlaves++;

        /* now get the offset of this device in the process data buffer and some other infos */
        if (ecatGetCfgSlaveInfo(EC_TRUE, wFixedAddress, &S_aSlaveList[S_dwSlaveIdx24]) != EC_E_NOERROR)
        {
            LogError("ERROR: ecatGetCfgSlaveInfo() returns with error.");
        }
    }

    /* Searching for: EL4132                                                       */
    /* search for the first device at the bus and return its fixed (EtherCAT) address */
    if (FindSlaveGetFixedAddr(INSTANCE_MASTER_DEFAULT, poLog, 0, ecvendor_beckhoff, ecprodcode_beck_EL4132, &wFixedAddress))
    {
        S_dwSlaveIdx4132 = S_dwAppFoundSlaves;
        S_dwAppFoundSlaves++;
        /* now get the offset of this device in the process data buffer and some other infos */
        if (ecatGetCfgSlaveInfo(EC_TRUE, wFixedAddress, &S_aSlaveList[S_dwSlaveIdx4132]) != EC_E_NOERROR)
        {
            LogError("ERROR: ecatGetCfgSlaveInfo() returns with error.");
        }
    }
    /* Searching for: IXXAT ETCio100                                                    */
    /* search for the first device at the bus and return its fixed (EtherCAT) address */
    if (FindSlaveGetFixedAddr(INSTANCE_MASTER_DEFAULT, poLog, 0, ecvendor_ixxat, ecprodcode_ixx_ETCio100, &wFixedAddress))
    {
        S_dwSlaveIdxETCio100 = S_dwAppFoundSlaves;
        S_dwAppFoundSlaves++;
        /* now get the offset of this device in the process data buffer and some other infos */
        if (ecatGetCfgSlaveInfo(EC_TRUE, wFixedAddress, &S_aSlaveList[S_dwSlaveIdxETCio100]) != EC_E_NOERROR)
        {
            LogError("ERROR: ecatGetCfgSlaveInfo() returns with error.");
        }
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
    EC_T_DWORD         dwClntId         /* [in]  EtherCAT master client id */
    )
{
    EC_T_DWORD           dwRes    = EC_E_ERROR;
    EC_T_CFG_SLAVE_INFO* pMySlave = EC_NULL;

    if (S_dwSlaveIdx4132 != SLAVE_NOT_FOUND)
    {
        EC_T_BOOL  bStopReading  = EC_FALSE;      /* Flag to stop object dictionary reading */
        EC_T_BYTE  byNumElements;
        EC_T_DWORD dwSize;
        EC_T_WORD  wOffsetTmp    = 0;
        EC_T_DWORD dwGainTmp     = 0;
        EC_T_WORD  wOffset;
        EC_T_DWORD dwGain;

        pMySlave = &S_aSlaveList[S_dwSlaveIdx4132];    

        /* demo: simple CoE SDO upload                              */
        /*       - synchronous: block until upload has finished     */
        dwRes = ecatCoeSdoUpload(pMySlave->dwSlaveId, EL4132_INDEX_USER_SCALE, EL4132_SUBINDEX_USRSCL_NUMELEM,
            &byNumElements, sizeof(EC_T_BYTE), &dwSize, MBX_TIMEOUT, 0);

        if (dwRes == EC_E_NOERROR)
        {
            if (nVerbose >= 2) LogMsg("tEl4132Mbx: EL4132 user scale: num elements = %d", (int)byNumElements);
        }
        else
        {
            LogError("tEl4132Mbx: error in COE SDO Upload! %s (0x%x)", ecatGetText(dwRes), dwRes);
        }

        dwRes = ecatCoeSdoUpload(pMySlave->dwSlaveId, EL4132_INDEX_USER_SCALE, EL4132_SUBINDEX_USRSCL_OFFSET,
            (EC_T_BYTE*)&wOffsetTmp, sizeof(EC_T_WORD), &dwSize, MBX_TIMEOUT, 0);
        wOffset = EC_NTOHS(wOffsetTmp);

        if (dwRes == EC_E_NOERROR)
        {
            if (nVerbose >= 2) LogMsg("tEl4132Mbx: EL4132 offset = 0x%x", (EC_T_DWORD)wOffset);
        }
        else
        {
            LogError("tEl4132Mbx: error in COE SDO Upload! %s (0x%x)", ecatGetText(dwRes), dwRes);
        }

        dwRes = ecatCoeSdoUpload(pMySlave->dwSlaveId, EL4132_INDEX_USER_SCALE, EL4132_SUBINDEX_USRSCL_GAIN,
            (EC_T_BYTE*)&dwGainTmp, sizeof(EC_T_DWORD), &dwSize, MBX_TIMEOUT, 0);
        dwGain = EC_NTOHL(dwGainTmp);

        if (dwRes == EC_E_NOERROR)
        {
            if (nVerbose >= 2) LogMsg("tEl4132Mbx: EL4132 gain = 0x%x", dwGain);
        }
        else
        {
            LogError("tEl4132Mbx: error in COE SDO Upload! %s (0x%x)", ecatGetText(dwRes), dwRes);
        }

        /* demo: simple CoE SDO download                            */
        /*       - synchronous: block until download has finished   */
        wOffset++;                  /* change user scale offset value */
        if (wOffset > 0x1000)
        {
            wOffset = 0;
        }
        wOffsetTmp = EC_HTONS(wOffset);
        dwRes = ecatCoeSdoDownload(pMySlave->dwSlaveId, EL4132_INDEX_USER_SCALE, EL4132_SUBINDEX_USRSCL_OFFSET, 
            (EC_T_BYTE*)&wOffsetTmp, sizeof(EC_T_WORD), MBX_TIMEOUT, 0);

        if (dwRes != EC_E_NOERROR)
        {
            LogError("tEl4132Mbx: error in COE SDO Download! %s (0x%x)", ecatGetText(dwRes), dwRes);
        }
        dwGain += 0x1000;
        if (dwGain > 0x10000000)
        {
            dwGain = 0;
        }
        
        dwGainTmp = EC_HTONL(dwGain);
        dwRes = ecatCoeSdoDownload(pMySlave->dwSlaveId, EL4132_INDEX_USER_SCALE, EL4132_SUBINDEX_USRSCL_GAIN,
            (EC_T_BYTE*)&dwGainTmp, sizeof(EC_T_DWORD), MBX_TIMEOUT, 0);
        
        if (dwRes != EC_E_NOERROR)
        {
            LogError("tEl4132Mbx: error in COE SDO Download! %s (0x%x)", ecatGetText(dwRes), dwRes);
        }

        /* now read object dict */
        /* In a real application this is typically not necessary */
        dwRes = CoeReadObjectDictionary(INSTANCE_MASTER_DEFAULT, poLog, nVerbose, &bStopReading, dwClntId, pMySlave->dwSlaveId, EC_TRUE, MBX_TIMEOUT);
    }
    return EC_E_NOERROR;
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
    EC_T_BYTE*          pbyPDOut        /* [in]  pointer to process data output buffer */
    )
{
    EC_T_CFG_SLAVE_INFO* pMySlave      = EC_NULL;

    static EC_T_DWORD    s_dwWorkpdCnt = 0;
    EC_T_BYTE            byVal         = 0;
    EC_T_BYTE*           pbyVal        = EC_NULL;
    EC_T_WORD*           pwVal         = EC_NULL;
    EC_T_WORD            wValue        = 0;

    EC_UNREFPARM(poLog);
    EC_UNREFPARM(nVerbose);

    /* process data are not modified not every cycle */
    s_dwWorkpdCnt++;
    if ((s_dwWorkpdCnt % 100) != 0)
    {
        goto Exit;
    }
    /* Digital input slave available ? */
    if ((S_dwSlaveIdx14 != SLAVE_NOT_FOUND) && (pbyPDIn != EC_NULL))
    {
    static EC_T_BYTE s_byDigInputLastVal = 0;

        /* get slave information */
        pMySlave = &S_aSlaveList[S_dwSlaveIdx14];

        /* monitor input change if ENI was NOT generated with GenPreopENI */
        if (((EC_T_DWORD)-1) != pMySlave->dwPdOffsIn)
        {
            EC_GETBITS(pbyPDIn, &byVal, pMySlave->dwPdOffsIn, pMySlave->dwPdSizeIn);
            if (byVal != s_byDigInputLastVal)
            {
                if (nVerbose >= EC_LOG_LEVEL_INFO)
                {
                    LogMsg("Input Value updated : Old : 0x%x -> New : 0x%x", s_byDigInputLastVal, byVal);
                    s_byDigInputLastVal = byVal;
                }
            }
        }
    }
    /* Digital output device */
    if ((S_dwSlaveIdx24 != SLAVE_NOT_FOUND) && (pbyPDOut != EC_NULL))
    {
        /* get slave information */
        pMySlave = &S_aSlaveList[S_dwSlaveIdx24];

        /* flash output if ENI was NOT generated with GenPreopENI */
        if (((EC_T_DWORD)-1) != pMySlave->dwPdOffsOut)
        {
            EC_GETBITS(pbyPDOut, &byVal, pMySlave->dwPdOffsOut, pMySlave->dwPdSizeOut);
            byVal++;
            EC_SETBITS(pbyPDOut, &byVal, pMySlave->dwPdOffsOut, pMySlave->dwPdSizeOut);
        }
    }

    /* EL4132 */
    if ((S_dwSlaveIdx4132 != SLAVE_NOT_FOUND) && (pbyPDOut != EC_NULL))
    {
        /* get slave information */
        pMySlave = &S_aSlaveList[S_dwSlaveIdx4132];    

        /* do some upcounting on channel 1 */
        if (((EC_T_DWORD)-1) != pMySlave->dwPdOffsOut)
        {
            pwVal = (EC_T_WORD*)&pbyPDOut[pMySlave->dwPdOffsOut/8];
            wValue = EC_GET_FRM_WORD(pwVal); /* for big endian systems we have to swap */
            wValue++;
            EC_SET_FRM_WORD(pwVal, wValue);

            /* do some downcounting on channel 2 */
            pwVal = (EC_T_WORD*)&pbyPDOut[(pMySlave->dwPdOffsOut+16)/8];
            wValue = EC_GET_FRM_WORD(pwVal); /* for big endian systems we have to swap */
            wValue--;
            EC_SET_FRM_WORD(pwVal, wValue);
        }
    }
    /* IXXAT ETCio100 */
    if ((S_dwSlaveIdxETCio100 != SLAVE_NOT_FOUND) && (pbyPDOut != EC_NULL))
    {
        /* get slave information */
        pMySlave = &S_aSlaveList[S_dwSlaveIdxETCio100];    

        /* flash digital output if ENI was NOT generated with GenPreopENI */
        if (((EC_T_DWORD)-1) != pMySlave->dwPdOffsOut)
        {
            pbyVal = (EC_T_BYTE*)&pbyPDOut[pMySlave->dwPdOffsOut/8];
            *pbyVal = (EC_T_BYTE)((*pbyVal) + 1);
        }

        /* increase analog output 1 if ENI was NOT generated with GenPreopENI */
        /* analog outputs are 12 Bit values at the ETCio 100 */
        if (((EC_T_DWORD)-1) != pMySlave->dwPdOffsOut)
        {
            pwVal = (EC_T_WORD*)&pbyPDOut[(pMySlave->dwPdOffsOut+8)/8];
            wValue = EC_GET_FRM_WORD(pwVal);  /* for big endian systems we have to swap */
            wValue = (EC_T_WORD)(wValue + 0x100);  /* to get a well measurable value difference modify last value by 0x100 */
            if(wValue & 0xF000)  /* 12 Bit value. The 4 highest bits are not relevant for the analog value */
            {
                wValue = 0;
            }
            EC_SET_FRM_WORD(pwVal, wValue);
        }

        /* decrease analog output 2 if ENI was NOT generated with GenPreopENI */
        /* analog outputs are 12 Bit values at the ETCio 100 */
        if (((EC_T_DWORD)-1) != pMySlave->dwPdOffsOut)
        {
            pwVal = (EC_T_WORD*)&pbyPDOut[(pMySlave->dwPdOffsOut+24)/8];
            wValue = EC_GET_FRM_WORD(pwVal); /* for big endian systems we have to swap */
            wValue = (EC_T_WORD)(wValue - 0x100);  /* to get a well measurable value difference modify last value by 0x100 */
            if(wValue & 0xF000)  /* 12 Bit value. The 4 highest bits are not relevant for the analog value */
            {
                wValue = 0x0F00;
            }
            EC_SET_FRM_WORD(pwVal, wValue);
        }
    }

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
    EC_T_DWORD           dwRes    = EC_E_ERROR;
    EC_T_CFG_SLAVE_INFO* pMySlave = EC_NULL;

    EC_UNREFPARM(poLog);
    EC_UNREFPARM(nVerbose);

    if (S_dwSlaveIdx4132 != SLAVE_NOT_FOUND)
    {
        EC_T_DWORD dwSize  = 0;
        EC_T_DWORD dwValue = 0;

        pMySlave = &S_aSlaveList[S_dwSlaveIdx4132];    

        dwRes = ecatCoeSdoUpload(pMySlave->dwSlaveId, 0x1018, 1,
            (EC_T_BYTE*)&dwValue, sizeof(EC_T_DWORD), &dwSize, MBX_TIMEOUT, 0);

        if (dwRes != EC_E_NOERROR)
        {
            LogError("myAppDiagnosis: error in COE SDO Upload of object 0x1018! %s (0x%x)", ecatGetText(dwRes), dwRes);
        }
    }

    return EC_E_NOERROR;
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
    EC_T_DWORD dwRetVal = EC_E_ERROR;

    EC_UNREFPARM(pParms);

    /* dispatch notification code */
    switch(dwCode)
    {
    case 1:
        LogMsg("Application notification code=%d received\n", dwCode);
        /* dwRetVal = EC_E_NOERROR; */
        break;
    case 2:
        break;
    default:
        break;
    }

    return dwRetVal;
}

/*-END OF SOURCE FILE--------------------------------------------------------*/
