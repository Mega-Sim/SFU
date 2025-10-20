/*-----------------------------------------------------------------------------
 * ATEMDemo.cpp
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master demo application
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include <AtEthercat.h>

#include "ATEMDemo.h"
#include "Logging.h"

#ifdef ATEMRAS_SERVER 
#include <AtEmRasSrv.h>
#endif

/*-DEFINES-------------------------------------------------------------------*/

#define LogMsg      S_poLog->LogMsg
#define LogError    S_poLog->LogError

/* master internal debug level */
#define ERR_MSG_TO_LINK_LAYER                   0   /* 0=off, 1=on */


/*-TYPEDEFS------------------------------------------------------------------*/

/*-GLOBAL VARIABLES-----------------------------------------------------------*/


/*-LOCAL VARIABLES-----------------------------------------------------------*/
static EC_T_DWORD          S_dwClntId        = 0;
static CAtEmLogging*       S_poLog    = EC_NULL;
static T_DEMO_THREAD_PARAM S_DemoThreadParam = {0};
static EC_T_PVOID          S_pvtJobThread    = EC_NULL;
static EC_T_BOOL           S_bEnaPerfJobs    = EC_FALSE;
static EC_T_TSC_MEAS_DESC  S_TscMeasDesc;
static EC_T_CHAR*          S_aszMeasInfo[MAX_JOB_NUM] =
{
    (EC_T_CHAR*)"JOB_ProcessAllRxFrames",
    (EC_T_CHAR*)"JOB_SendAllCycFrames  ",
    (EC_T_CHAR*)"JOB_MasterTimer       ",
    (EC_T_CHAR*)"JOB_SendAcycFrames    ",
    (EC_T_CHAR*)"Cycle Time            ",
    (EC_T_CHAR*)"myAppWorkPd           "
};

/*-FORWARD DECLARATIONS------------------------------------------------------*/
static EC_T_DWORD ecatNotifyCallback(EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
static EC_T_VOID  tEcJobTask(EC_T_VOID* pvThreadParamDesc);

/*-MYAPP---------------------------------------------------------------------*/
/* Demo code: Remove/change this in your application */

/* demo bus configuration */
#define EC_SLAVE_EK1100     0       /* 1. slave = bus coupler */
#define EC_SLAVE_EL2004_1   1       /* 2. slave = EL2004 (digital output) */
#define EC_SLAVE_EL2004_2   2       /* 3. slave = EL2004 (digital output) */
#define EC_SLAVE_EL1004_1   3       /* 4. slave = EL1004 (digital input) */
#define EC_SLAVE_EL1004_2   4       /* 5. slave = EL1004 (digital input) */
#define EC_NUM_SLAVES       5       /* number of slaves */

/* physical addresses: by default, ET9000 starts with physical address 1001 */
#define EC_ADDR_SLAVE_EK1100     1001
#define EC_ADDR_SLAVE_EL2004_1   1002
#define EC_ADDR_SLAVE_EL2004_2   1003
#define EC_ADDR_SLAVE_EL1004_1   1004
#define EC_ADDR_SLAVE_EL1004_2   1005


#if (defined INCLUDE_VARREAD)
#define MAX_NUM_OF_SLAVE_VARS       4       /* we have a maximum of x variables for each slave */
#endif

typedef struct _EC_T_SLAVE
{
    EC_T_BOOL           bSlaveFound;

    EC_T_WORD           wFixedAddress;
    EC_T_WORD           wReserved;

    EC_T_DWORD          dwVendorId;
    EC_T_DWORD          dwProductCode;

    EC_T_CFG_SLAVE_INFO oSlaveInfo;             /* slave information (properties) */

#if (defined INCLUDE_VARREAD)
    EC_T_PROCESS_VAR_INFO_EX   aVarInfo[MAX_NUM_OF_SLAVE_VARS];            /* */
#endif
} EC_T_SLAVE;

static EC_T_SLAVE       S_aSlaveList[EC_NUM_SLAVES];  /* list of slaves */

static EC_T_DWORD myAppInit     (CAtEmLogging*           poLog, EC_T_INT nVerbose);
static EC_T_DWORD myAppPrepare  (CAtEmLogging*           poLog, EC_T_INT nVerbose);
static EC_T_DWORD myAppWorkpd   (CAtEmLogging*           poLog, EC_T_INT nVerbose, EC_T_BYTE* pbyPDIn, EC_T_BYTE* pbyPDOut);

/* Demo code: End */

/*-FUNCTION DEFINITIONS------------------------------------------------------*/

/********************************************************************************/
/** \brief  EtherCAT Master demo Application.
*
* This is a EtherCAT Master demo application.
*
* \return  Status value.
*/
EC_T_DWORD ATEMDemo(
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

#ifdef ATEMRAS_SERVER
    EC_UNREFPARM(wServerPort);
#endif
    EC_UNREFPARM(poLinkParmsRed);

    /* store parameters */
    S_poLog = poLog;
    S_bEnaPerfJobs = bEnaPerfJobs;
    S_DemoThreadParam.pvTimingEvent = pvTimingEvent;

    /* check if interrupt mode is selected */
    if (poLinkParms->eLinkMode != EcLinkMode_POLLING)
    {
        dwRetVal = EC_E_INVALIDPARM;
        LogError("Error: Link layer in 'interrupt' mode is not supported by EcMasterDemo2. Please select 'polling' mode.");
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

    /******************************************************/
    /* Demo code: Remove/change this in your application  */
    /* Initialize application                             */
    /******************************************************/
    dwRes = myAppInit(poLog, nVerbose);
    if (EC_E_NOERROR != dwRes)
    {
        LogError( (EC_T_CHAR*)"myAppInit failed, error code: 0x%x", dwRes );
        dwRetVal = dwRes;
        goto Exit;
    }

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
    /*************************************/
    /* Create cyclic process data Thread */
    /*************************************/
    {
        S_DemoThreadParam.bJobThreadRunning  = EC_FALSE;
        S_DemoThreadParam.bJobThreadShutdown = EC_FALSE;
        S_DemoThreadParam.pLogInst           = S_poLog;
        S_DemoThreadParam.pNotInst           = pNotification;
        S_DemoThreadParam.dwCpuIndex         = dwCpuIndex;
        S_DemoThreadParam.dwBusCycleTimeUsec = dwBusCycleTimeUsec;
        S_pvtJobThread = OsCreateThread((EC_T_CHAR*)"tEcJobTask", tEcJobTask, JOBS_THREAD_PRIO, JOBS_THREAD_STACKSIZE, (EC_T_VOID*)&S_DemoThreadParam);
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
    }
    /********************/
    /* Configure master */
    /********************/
    dwRes = ecatConfigureMaster(eCnfType, pbyCnfData, dwCnfDataLen);
    if (dwRes != EC_E_NOERROR)
    {
        dwRetVal = dwRes;
        LogError("Cannot configure EtherCAT-Master! %s (Result = 0x%x)", ecatGetText(dwRes), dwRes);
        goto Exit;
    }
    /*******************/
    /* Register client */
    /*******************/
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
    /**********************/
    /* Print found slaves */
    /**********************/
    if (nVerbose >= 2)
    {
    EC_T_SB_STATUS_NTFY_DESC oBusScanStatus;

        OsMemset(&oBusScanStatus, 0, sizeof(EC_T_SB_STATUS_NTFY_DESC));
        DoBusScan(INSTANCE_MASTER_DEFAULT, poLog, &oBusScanStatus, EC_TRUE, EC_FALSE, ETHERCAT_SCANBUS_TIMEOUT);
        PrintSlaveInfos(INSTANCE_MASTER_DEFAULT, poLog);
    }
    /*********************/
    /* Print MAC address */
    /*********************/
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
    /*******************************************************************************/
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
    /* Search for some well known slaves                  */
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
    
    /* ----------------------------------------------------------------------------- */
    /* After reaching the PREOP state CANOpen SDO uploads and downloads are possible */
    /* ----------------------------------------------------------------------------- */

    /* set master and bus state to SAFEOP */
    dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_SAFEOP);
    pNotification->ProcessNotificationJobs();
    if (dwRes != EC_E_NOERROR)
    {
        LogError( "Cannot start set master state to SAFEOP (Result = %s (0x%lx))", ecatGetText(dwRes), dwRes );
            dwRetVal = dwRes;
        goto Exit;
    }
    /* set master and bus state to OP */
    dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_OP);
    pNotification->ProcessNotificationJobs();
    if (dwRes != EC_E_NOERROR)
    {
        LogError( "Cannot start set master state to OP (Result = %s (0x%lx))", ecatGetText(dwRes), dwRes );
        dwRetVal = dwRes;
        goto Exit;
    }

    if (S_bEnaPerfJobs)
    {
        LogMsg("");
        LogMsg("Job times during startup <INIT> to <%s>:", ecatStateToStr(ecatGetMasterState()));
        PERF_MEASURE_JOBS_SHOW();       /* show job times */
        LogMsg("");
        ecatPerfMeasReset(&S_TscMeasDesc, 0xFFFFFFFF);        /* clear job times of startup phase */
    }

    /****************/
    /* run the demo */
    /****************/
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
        
        /* process notification jobs */
        pNotification->ProcessNotificationJobs();

        OsSleep(5);
    }

    if (S_bEnaPerfJobs)
    {
        LogMsg("");
        LogMsg("Job times before shutdown");
        PERF_MEASURE_JOBS_SHOW();       /* show job times */
        LogMsg("");
    }

Exit:
    if (0 != nVerbose) LogMsg( "========================" );
    if (0 != nVerbose) LogMsg( "Shutdown EtherCAT Master" );
    if (0 != nVerbose) LogMsg( "========================" );

    /**************************************************/
    /* Stop EtherCAT bus --> Set Master state to INIT */
    /**************************************************/
    dwRes = ecatSetMasterState(ETHERCAT_STATE_CHANGE_TIMEOUT, eEcatState_INIT);
    if (EC_E_NOERROR != dwRes)
    {
        LogError("Cannot stop EtherCAT-Master! %s (0x%lx)", ecatGetText(dwRes), dwRes);
    }
    /*********************/
    /* Unregister client */
    /*********************/
    if (S_dwClntId != 0)
    {
        dwRes = ecatUnregisterClient(S_dwClntId); 
        if (EC_E_NOERROR != dwRes)
        {
            LogError("Cannot unregister client! %s (0x%lx)", ecatGetText(dwRes), dwRes);
        }
        S_dwClntId = 0;
    }
    /***********************/
    /* Shutdown tEcJobTask */
    /***********************/
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
    /***********************/
    /* Deinitialize master */
    /***********************/
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

    EC_CPUSET_ZERO(CpuSet);
    EC_CPUSET_SET(CpuSet, pDemoThreadParam->dwCpuIndex);
    bOk = OsSetThreadAffinity(EC_NULL, CpuSet);
    if (!bOk)
    {
        LogError("Error: Set job task affinitiy, invalid CPU index %d\n", pDemoThreadParam->dwCpuIndex);
        goto Exit;
    }

    /* demo loop */
    pDemoThreadParam->bJobThreadRunning = EC_TRUE;
    do
    {
        /* wait for next cycle (event from scheduler task) */
#if (defined RTAI)
    	OsSleepTillTick(); /* period is set after creating jobtask */
#else
        OsWaitForEvent(pDemoThreadParam->pvTimingEvent, EC_WAITINFINITE);
#endif /* !RTAI */

        PERF_JOB_END(PERF_CycleTime);
        PERF_JOB_START(PERF_CycleTime);

        /* process all received frames (read new input values) */
        PERF_JOB_START(JOB_ProcessAllRxFrames);
        dwRes = ecatExecJob( eUsrJob_ProcessAllRxFrames, &bPrevCycProcessed );
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes && EC_E_LINK_DISCONNECTED != dwRes)
        {
            LogError("ERROR: ecatExecJob( eUsrJob_ProcessAllRxFrames): %s (0x%lx)", ecatGetText(dwRes), dwRes);
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


        /*********************************************************/
        /* New input data are available. Generate output data    */
        /*                                                       */
        /* Insert your application here.                         */
        /*********************************************************/

        /* The input data are found in: EC_T_BYTE* pbyPDIn  = ecatGetProcessImageInputPtr();   */
        /* Write the output data to   : EC_T_BYTE* pbyPDOut = ecatGetProcessImageOutputPtr();  */
        /*
        if ((pbyPDIn != EC_NULL) && (pbyPDOut != EC_NULL))
        {
            if (pbyPDIn[0] & 1)
            {
                pbyPDOut[0] |= 1;
            }
            else
            {
                pbyPDOut[0] &= ~1;
            }
        }
        */


        /*****************************************************/
        /* Demo code: Remove/change this in your application */
        /* Working process data cyclic call                  */
        /*****************************************************/
        PERF_JOB_START(PERF_myAppWorkpd);
        myAppWorkpd(pDemoThreadParam->pLogInst, pDemoThreadParam->pNotInst->Verbose(), ecatGetProcessImageInputPtr(), ecatGetProcessImageOutputPtr());
        PERF_JOB_END(PERF_myAppWorkpd);

        /* write output values of current cycle, by sending all cyclic frames */
        PERF_JOB_START(JOB_SendAllCycFrames);
        dwRes = ecatExecJob( eUsrJob_SendAllCycFrames, EC_NULL );
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes && EC_E_LINK_DISCONNECTED != dwRes)
        {
            LogError("ecatExecJob( eUsrJob_SendAllCycFrames,    EC_NULL ): %s (0x%lx)", ecatGetText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_SendAllCycFrames);

        /* remove this code when using licensed version */
        if (EC_E_EVAL_EXPIRED == dwRes )
        {
            /* set shutdown flag */
            bRun = EC_FALSE;
        }

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

    pNotifyInstance = (CEmNotification*)pParms->pCallerData;

    /* call the default handler */
    dwRetVal = pNotifyInstance->ecatNotify(dwCode, pParms);

Exit:

    return dwRetVal;
}

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

    OsMemset(S_aSlaveList, 0, sizeof(S_aSlaveList));

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
    EC_T_DWORD dwSlaveCnt;
    EC_T_SLAVE* pSlave;

    EC_UNREFPARM(poLog);
    EC_UNREFPARM(nVerbose);

    /* insert physical addresses into slave list entries */
    S_aSlaveList[EC_SLAVE_EK1100].wFixedAddress         = EC_ADDR_SLAVE_EK1100;     
    S_aSlaveList[EC_SLAVE_EK1100].dwVendorId            = ecvendor_beckhoff; 
    S_aSlaveList[EC_SLAVE_EK1100].dwProductCode         = ecprodcode_beck_EK1100;

    S_aSlaveList[EC_SLAVE_EL2004_1].wFixedAddress       = EC_ADDR_SLAVE_EL2004_1;   
    S_aSlaveList[EC_SLAVE_EL2004_1].dwVendorId          = ecvendor_beckhoff; 
    S_aSlaveList[EC_SLAVE_EL2004_1].dwProductCode       = ecprodcode_beck_EL2004;

    S_aSlaveList[EC_SLAVE_EL2004_2].wFixedAddress       = EC_ADDR_SLAVE_EL2004_2;   
    S_aSlaveList[EC_SLAVE_EL2004_2].dwVendorId          = ecvendor_beckhoff; 
    S_aSlaveList[EC_SLAVE_EL2004_2].dwProductCode       = ecprodcode_beck_EL2004;

    S_aSlaveList[EC_SLAVE_EL1004_1].wFixedAddress       = EC_ADDR_SLAVE_EL1004_1;   
    S_aSlaveList[EC_SLAVE_EL1004_1].dwVendorId          = ecvendor_beckhoff; 
    S_aSlaveList[EC_SLAVE_EL1004_1].dwProductCode       = ecprodcode_beck_EL1004;

    S_aSlaveList[EC_SLAVE_EL1004_2].wFixedAddress       = EC_ADDR_SLAVE_EL1004_2;   
    S_aSlaveList[EC_SLAVE_EL1004_2].dwVendorId          = ecvendor_beckhoff; 
    S_aSlaveList[EC_SLAVE_EL1004_2].dwProductCode       = ecprodcode_beck_EL1004;

    /* determine slave offsets */
    for( dwSlaveCnt = 0; dwSlaveCnt < EC_NUM_SLAVES; dwSlaveCnt++ )
    {
        pSlave = &S_aSlaveList[dwSlaveCnt];

        if( (emGetCfgSlaveInfo(INSTANCE_MASTER_DEFAULT, EC_TRUE, pSlave->wFixedAddress, &pSlave->oSlaveInfo) == EC_E_NOERROR))
        {
            if( pSlave->oSlaveInfo.dwVendorId == pSlave->dwVendorId && pSlave->oSlaveInfo.dwProductCode == pSlave->dwProductCode )
            {
                pSlave->bSlaveFound = EC_TRUE;
            }
            else
            {
                LogMsg("Wrong Slave found at physical address: %d", (int)pSlave->wFixedAddress );
                pSlave->bSlaveFound = EC_FALSE;
            }
        }
        else
        {
            LogMsg("No Slave found at physical address: %d", (int)pSlave->wFixedAddress );
            pSlave->bSlaveFound = EC_FALSE;
        }

        /* read variable infos */
#if (defined INCLUDE_VARREAD)
        switch(dwSlaveCnt)
        {
        case EC_SLAVE_EK1100:
            break;

        case EC_SLAVE_EL2004_1:
            if( (emFindOutpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 2 (EL2004).Channel 1.Output", &pSlave->aVarInfo[0]) != EC_E_NOERROR))
            {
                LogMsg("EL2004 variable not found");
            }
            if( (emFindOutpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 2 (EL2004).Channel 2.Output", &pSlave->aVarInfo[1]) != EC_E_NOERROR))
            {
                LogMsg("EL2004 variable not found");
            }
            if( (emFindOutpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 2 (EL2004).Channel 3.Output", &pSlave->aVarInfo[2]) != EC_E_NOERROR))
            {
                LogMsg("EL2004 variable not found");
            }
            if( (emFindOutpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 2 (EL2004).Channel 4.Output", &pSlave->aVarInfo[3]) != EC_E_NOERROR))
            {
                LogMsg("EL2004 variable not found");
            }
            break;

        case EC_SLAVE_EL2004_2:
            if( (emFindOutpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 3 (EL2004).Channel 1.Output", &pSlave->aVarInfo[0]) != EC_E_NOERROR))
            {
                LogMsg("EL2004 variable not found");
            }
            if( (emFindOutpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 3 (EL2004).Channel 2.Output", &pSlave->aVarInfo[1]) != EC_E_NOERROR))
            {
                LogMsg("EL2004 variable not found");
            }
            if( (emFindOutpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 3 (EL2004).Channel 3.Output", &pSlave->aVarInfo[2]) != EC_E_NOERROR))
            {
                LogMsg("EL2004 variable not found");
            }
            if( (emFindOutpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 3 (EL2004).Channel 4.Output", &pSlave->aVarInfo[3]) != EC_E_NOERROR))
            {
                LogMsg("EL2004 variable not found");
            }
            break;

        case EC_SLAVE_EL1004_1:
            if( (emFindInpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 4 (EL1004).Channel 1.Input", &pSlave->aVarInfo[0]) != EC_E_NOERROR))
            {
                LogMsg("EL1004 variable not found");
            }
            if( (emFindInpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 4 (EL1004).Channel 2.Input", &pSlave->aVarInfo[1]) != EC_E_NOERROR))
            {
                LogMsg("EL1004 variable not found");
            }
            if( (emFindInpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 4 (EL1004).Channel 3.Input", &pSlave->aVarInfo[2]) != EC_E_NOERROR))
            {
                LogMsg("EL1004 variable not found");
            }
            if( (emFindInpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 4 (EL1004).Channel 4.Input", &pSlave->aVarInfo[3]) != EC_E_NOERROR))
            {
                LogMsg("EL1004 variable not found");
            }
            break;

        case EC_SLAVE_EL1004_2:
            if( (emFindInpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 5 (EL1004).Channel 1.Input", &pSlave->aVarInfo[0]) != EC_E_NOERROR))
            {
                LogMsg("EL1004 variable not found");
            }
            if( (emFindInpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 5 (EL1004).Channel 2.Input", &pSlave->aVarInfo[1]) != EC_E_NOERROR))
            {
                LogMsg("EL1004 variable not found");
            }
            if( (emFindInpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 5 (EL1004).Channel 3.Input", &pSlave->aVarInfo[2]) != EC_E_NOERROR))
            {
                LogMsg("EL1004 variable not found");
            }
            if( (emFindInpVarByNameEx(INSTANCE_MASTER_DEFAULT, (EC_T_CHAR*)"Term 5 (EL1004).Channel 4.Input", &pSlave->aVarInfo[3]) != EC_E_NOERROR))
            {
                LogMsg("EL1004 variable not found");
            }
            break;
        }
#endif
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
    EC_T_SLAVE*         pSlave;
    static EC_T_DWORD   s_dw2004Counter     = 0;
    EC_T_BYTE           byWorkData          = 0;
    static EC_T_BYTE    s_by1004PDInOld     = 0;
    EC_T_WORD           wShift = 0;
    EC_T_WORD           wMask  = 0;
    EC_T_BYTE           byVal  = 0;

    EC_UNREFPARM(poLog);
    EC_UNREFPARM(nVerbose);

    /* handle EL2004 slaves device (digital output device) */

    if( (s_dw2004Counter % 500) == 0 )      /* hint: do it not in every cycle here */
    {

        //#define COUNTING_EXAMPLE            
#ifdef COUNTING_EXAMPLE
        EC_T_DWORD          dwSlaveCnt;
        EC_T_BYTE           byPDOut         = 0;

        for( dwSlaveCnt = EC_SLAVE_EL2004_1; dwSlaveCnt <= EC_SLAVE_EL2004_2; dwSlaveCnt++ )
        {
            pSlave = &S_aSlaveList[dwSlaveCnt];

            if( pSlave->bSlaveFound && (pbyPDOut != EC_NULL) )
            {
                wShift = (EC_T_WORD)(pSlave->oSlaveInfo.dwPdOffsOut % 8);
                wMask  = (EC_T_WORD)((1<<pSlave->oSlaveInfo.dwPdSizeOut)-1);

                /* get data */
                byPDOut = pbyPDOut[pSlave->oSlaveInfo.dwPdOffsOut/8];
                byWorkData = byPDOut;

                /* clear own bits */
                byPDOut &= ~(wMask<<wShift);

                /* shift, mask and increment */
                byWorkData = (EC_T_BYTE)(byWorkData>>wShift);
                byWorkData = (EC_T_BYTE)(byWorkData & wMask);
                if(dwSlaveCnt == EC_SLAVE_EL2004_1)
                {
                    byWorkData++;   // count up on first EL2004
                }
                else
                {
                    byWorkData--;   // count down on second EL2004
                }
                byWorkData = (EC_T_BYTE)(byWorkData & wMask);

                /* back shift */
                byWorkData = (EC_T_BYTE)(byWorkData<<wShift);

                /* write data */
                pbyPDOut[pSlave->oSlaveInfo.dwPdOffsOut/8] = (EC_T_BYTE)(byPDOut | byWorkData);
            }
        }

#elif (defined INCLUDE_VARREAD)

        /* set digital output 1 on this slave */
        pSlave = &S_aSlaveList[EC_SLAVE_EL2004_1];

        if(pSlave->bSlaveFound)
        {
            EC_GETBITS(pbyPDOut, &byVal, pSlave->aVarInfo[0].nBitOffs, pSlave->aVarInfo[0].nBitSize);

            /* if set then clear else set it */            
            byVal = (byVal == 1) ? (EC_T_BYTE)0 : (EC_T_BYTE)1;

            EC_SETBITS(pbyPDOut, &byVal, pSlave->aVarInfo[0].nBitOffs, pSlave->aVarInfo[0].nBitSize);
        }
#endif

    }
    s_dw2004Counter++;

    /* handle first EL1004 slave device (digital input device) */
    if( S_aSlaveList[EC_SLAVE_EL1004_1].bSlaveFound && (pbyPDIn != EC_NULL) )
    {
        pSlave = &S_aSlaveList[EC_SLAVE_EL1004_1];

        /* read data from process data input area */
        byWorkData = pbyPDIn[pSlave->oSlaveInfo.dwPdOffsIn/8];

        /* shift and mask value */
        wShift = (EC_T_WORD)(pSlave->oSlaveInfo.dwPdOffsIn % 8);
        wMask  = (EC_T_WORD)((1<<pSlave->oSlaveInfo.dwPdSizeIn)-1);

        byWorkData = (EC_T_BYTE)(byWorkData>>wShift);
        byWorkData = (EC_T_BYTE)(byWorkData & wMask);

        if( byWorkData != s_by1004PDInOld )
        {
            LogMsg("Input Value updated : Old : 0x%x -> New : 0x%x", s_by1004PDInOld, byWorkData);
            s_by1004PDInOld = byWorkData;
        }
    }
    return EC_E_NOERROR;
}

/*-END OF SOURCE FILE--------------------------------------------------------*/
