/*
 * SEMES CONFIDENTIAL
 *
 * Copyright (c) 2017 SEMES Co.,Ltd.
 * All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains the property
 * of SEMES and its suppliers, if any. The intellectual and technical concepts
 * contained herein are proprietary to SEMES and its suppliers and may be
 * covered by S.Korea and Foreign Patents, patents in process, and are
 * protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from SEMES.
 */

//
// Created by sem on 18. 4. 26.
//

#include "ecmaster_app.h"
#include "acontis/at_xml_parser.h"
#include "acontis/select_link_layer.h"

using namespace sephi;

/*-DEFINES-------------------------------------------------------------------*/
#define ETHERCAT_STATE_CHANGE_TIMEOUT   15000   /* master state change timeout in ms */
#define ETHERCAT_SCANBUS_TIMEOUT        5000    /* scanbus timeout in ms */

#define ETHERCAT_DC_TIMEOUT             12000   /* DC initialization timeout in ms */
#define ETHERCAT_DC_ARMW_BURSTCYCLES    10000   /* DC burst cycles (static drift compensation) */
#define ETHERCAT_DC_ARMW_BURSTSPP       12      /* DC burst bulk (static drift compensation) */
#define ETHERCAT_DC_DEV_LIMIT           13      /* DC deviation limit (highest bit tolerate by the broadcast read) */
#define ETHERCAT_DC_SETTLE_TIME         1500    /* DC settle time in ms */

#define APP_ERROR         -1
#define SYNTAX_ERROR      -2
#define APP_OUT_OF_MEMORY -3
#define APP_NOERROR        0

/* tag names for DemoConfig.xml file */
#define DEMO_CFG_DEFAULT_FILENAME       (EcChar*)"DemoConfig.xml"
#define DEMO_CFG_TAG_ENI_FILENAME       (EcChar*)"Config\\Common\\ENIFileName"
#define DEMO_CFG_TAG_LOG_FILEPREFIX     (EcChar*)"Config\\Common\\LogFilePrefix"
#define DEMO_CFG_TAG_LINK_LAYER         (EcChar*)"Config\\Common\\LinkLayer"
#define DEMO_CFG_TAG_LINK_LAYER2        (EcChar*)"Config\\Common\\LinkLayer2"
#define DEMO_CFG_TAG_DURATION           (EcChar*)"Config\\Common\\DemoDuration"
#define DEMO_CFG_TAG_CPU_AFFINITY       (EcChar*)"Config\\Common\\CpuAffinity"
#define DEMO_CFG_TAG_VERBOSITY_LVL      (EcChar*)"Config\\Common\\VerbosityLevel"
#define DEMO_CFG_TAG_PERF_MEASURE       (EcChar*)"Config\\Common\\PerfMeasurement"
#define DEMO_CFG_TAG_RAS_ENABLED        (EcChar*)"Config\\Common\\RASEnabled"
#define DEMO_CFG_TAG_RAS_PORT           (EcChar*)"Config\\Common\\RASPort"
#define DEMO_CFG_TAG_AUXCLK             (EcChar*)"Config\\Common\\AuxClk"
#define DEMO_CFG_TAG_BUSCYCLETIME       (EcChar*)"Config\\Common\\BusCycleTime"

/* tag names for DemoConfig.xml file */
#define MOTIONDEMO_CFG_TAG_NO_BUS_SHIFT      (EcChar*)"Config\\MotionDemo\\NoDCMBusShift"        /* Disable DCM controller */
#define MOTIONDEMO_CFG_TAG_CTL_SETVAL        (EcChar*)"Config\\MotionDemo\\DCMCtlSetVal"         /* DCM controller set value in nanosec */
#define MOTIONDEMO_CFG_TAG_CMD_MODE          (EcChar*)"Config\\MotionDemo\\CmdMode"              /* Command Mode */

#define MOTIONDEMO_CFG_TAG_DRIVE_ADDR       (EcChar*)"Config\\MotionDemo\\Drive%d\\Address"          /* EtherCAT Address */
#define MOTIONDEMO_CFG_TAG_DRIVE_MODE       (EcChar*)"Config\\MotionDemo\\Drive%d\\OperationMode"    /* Mode of operation */
#define MOTIONDEMO_CFG_TAG_DRIVE_NO         (EcChar*)"Config\\MotionDemo\\Drive%d\\SercosDriveNo"    /* Adressed drive number within SERCOS servo controller */
#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_OPMODE (EcChar*)"Config\\MotionDemo\\Drive%d\\CoeIdxOpMode"     /* Index of CoE object 'Operation Mode'. Default 0x6060 */
#define MOTIONDEMO_CFG_TAG_DRIVE_PROFILE    (EcChar*)"Config\\MotionDemo\\Drive%d\\DriveProfile"     /* "DS402" or "SERCOS" */
#define MOTIONDEMO_CFG_TAG_DRIVE_INCPERMM   (EcChar*)"Config\\MotionDemo\\Drive%d\\IncPerMM"         /* Increments per mm */
#define MOTIONDEMO_CFG_TAG_DRIVE_INCFACTOR  (EcChar*)"Config\\MotionDemo\\Drive%d\\IncFactor"        /* Increments are scaled with this value */
#define MOTIONDEMO_CFG_TAG_DRIVE_ACC        (EcChar*)"Config\\MotionDemo\\Drive%d\\Acc"              /* Move acc */
#define MOTIONDEMO_CFG_TAG_DRIVE_DEC        (EcChar*)"Config\\MotionDemo\\Drive%d\\Dec"              /* Move dec */
#define MOTIONDEMO_CFG_TAG_DRIVE_VEL        (EcChar*)"Config\\MotionDemo\\Drive%d\\Vel"              /* Move vel */
#define MOTIONDEMO_CFG_TAG_DRIVE_JERK       (EcChar*)"Config\\MotionDemo\\Drive%d\\Jerk"             /* Move jerk */
#define MOTIONDEMO_CFG_TAG_DRIVE_DIST       (EcChar*)"Config\\MotionDemo\\Drive%d\\Distance"         /* Move distance */
#define MOTIONDEMO_CFG_TAG_DRIVE_POS_WIND   (EcChar*)"Config\\MotionDemo\\Drive%d\\PositionWindow"   /* Position Window */
#define MOTIONDEMO_CFG_TAG_DRIVE_VEL_GAIN   (EcChar*)"Config\\MotionDemo\\Drive%d\\VelocityGain"     /* Velocity gain */
#define MOTIONDEMO_CFG_TAG_DRIVE_TOR_GAIN   (EcChar*)"Config\\MotionDemo\\Drive%d\\TorqueGain"       /* Torque gain */

#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_STATUS    (EcChar*)"Config\\MotionDemo\\Drive%d\\IdxStatus"     /* Object Index Statusword */
#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_CONTROL   (EcChar*)"Config\\MotionDemo\\Drive%d\\IdxControl"    /* Object Index Controlword */
#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_POSACT    (EcChar*)"Config\\MotionDemo\\Drive%d\\IdxPosact"     /* Object Index Position actual */
#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_TARPOS    (EcChar*)"Config\\MotionDemo\\Drive%d\\IdxTargetpos"  /* Object Index Target Position */
#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_TARVEL    (EcChar*)"Config\\MotionDemo\\Drive%d\\IdxTargetvel"  /* Object Index Target Velocity */
#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_ACTTRQ    (EcChar*)"Config\\MotionDemo\\Drive%d\\IdxActualtrq"  /* Object Index Actual Torque */
#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_TARTRQ    (EcChar*)"Config\\MotionDemo\\Drive%d\\IdxTargettrq"  /* Object Index Target Torque */

#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_VELOFF    (EcChar*)"Config\\MotionDemo\\Drive%d\\IdxVelOffset"  /* Object Index Velocity Offset */
#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_TOROFF    (EcChar*)"Config\\MotionDemo\\Drive%d\\IdxTorOffset"  /* Object Index Torque Offset */
#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_MODE      (EcChar*)"Config\\MotionDemo\\Drive%d\\IdxMode"       /* Object Index Mode of Operation */
#define MOTIONDEMO_CFG_TAG_DRIVE_IDX_MODE_DISPLAY      (EcChar*)"Config\\MotionDemo\\Drive%d\\IdxModeDisplay"       /* Object Index Mode of Operation Dissplay */


#define COMMAND_LINE_BUFFER_LENGTH  2048


/*-TYPEDEFS------------------------------------------------------------------*/
struct EC_T_TIMING_DESC
{
    EcVoid*          pvTimingEvent;      /* event handle */
    EcDWord          dwBusCycleTimeUsec; /* cycle time in usec */
    EcBool           bShutdown;          /* EC_TRUE if aux thread shall shut down */
    EcBool           bIsRunning;         /* EC_TRUE if the aux thread is running */
    EcDWord          dwCpuIndex;         /* SMP systems: CPU index */
};



static EcVoid ShowSyntax(EcVoid)
{
    OsDbgMsg("Syntax variant 1:\n");
    OsDbgMsg("EcMasterDemoMotion [DemoConfigFileName]\n");
    OsDbgMsg("   DemoConfigFileName     Use given configuration file (default = %s)\n", DEMO_CFG_DEFAULT_FILENAME);

    OsDbgMsg("Syntax variant 2:\n");
    OsDbgMsg("EcMasterDemoMotion [-cfg FileName] [-f ENI-FileName] [-t time] [-b time] [-a affinity] [-v lvl] [-perf] [-log Prefix] [-dcmmode]");
    ShowLinkLayerSyntax1();
    OsDbgMsg("   -cfg              Use given file for additional parameters\n");
    OsDbgMsg("     FileName        file name\n");
    OsDbgMsg("   -f                Use given ENI file\n");
    OsDbgMsg("     FileName        file name .xml\n");
    OsDbgMsg("   -t                Demo duration\n");
    OsDbgMsg("     time            Time in msec, 0 = forever (default = 120000)\n");
    OsDbgMsg("   -b                Bus cycle time\n");
    OsDbgMsg("     cycle time      Cycle time in usec\n");
    OsDbgMsg("   -a                CPU affinity\n");
    OsDbgMsg("     affinity        0 = first CPU, 1 = second, ...\n");
    OsDbgMsg("   -v                Set verbosity level\n");
    OsDbgMsg("     lvl             Level: 0=off, 1(default) ...n=more messages\n");
    OsDbgMsg("   -perf             Enable job measurement\n");
    OsDbgMsg("   -log              Use given file name prefix for log files\n");
    OsDbgMsg("     Prefix          prefix\n");
    OsDbgMsg("   -dcmmode          Set DCM mode\n");
    OsDbgMsg("     off             Off\n");
    OsDbgMsg("     busshift        BusShift mode (default)\n");
    OsDbgMsg("     mastershift     MasterShift mode\n");
    OsDbgMsg("     masterrefclock  MasterRefClock mode\n");
    OsDbgMsg("     dcx             External synchronization mode\n");
    OsDbgMsg("   -mb               Enable mailbox communication only.\n");
    OsDbgMsg("   -emul             Run ecmaster as emulation mode.\n");
    OsDbgMsg("   -model=<filename> Model file for emulator.\n");
    ShowLinkLayerSyntax2();

    return;
}


EcBool sephi::OsDbgMsgHookWrapper(const EcChar* szFormat, EC_T_VALIST vaArgs)
{
    EcBool bPrint = EC_TRUE;

    if (bPrint) {
        OsVprintf(szFormat, vaArgs);
    }
    return EC_FALSE;
}


bool EmCommandLineOption::parse(int argc, char *argv[])
{
    int                     nRetVal             = APP_ERROR;
    EcDWord              dwRes               = EC_E_ERROR;
    EcBool               bLogInitialized     = EC_FALSE;
    EcChar               szCommandLine[COMMAND_LINE_BUFFER_LENGTH];
    EcChar               szFullCommandLine[COMMAND_LINE_BUFFER_LENGTH];
    EcBool               bGetNextWord        = EC_TRUE;
    EcChar*              ptcWord             = EC_NULL;
    EcChar               tcStorage           = '\0';

    EcChar               szLogFileprefix[256] = {'\0'};
    EcChar               szENIFilename[256]  = {'\0'};
    EC_T_CPUSET             CpuSet;
    EcChar               szCfgFilename[256]  = {'\0'};
    EcBool               bTimingCmdLineOverride = EC_FALSE;
    EcChar*              pszCfgFilePath      = DEMO_CFG_DEFAULT_FILENAME;
    EC_T_TIMING_DESC        TimingDesc;

    eCnfType            = eCnfType_Unknown;
    pbyCnfData          = 0;
    dwCnfDataLen        = 0;
    dwDuration          = 120000;
    dwNumLinkLayer     = 0;
    dwCpuIndex          = 0;
    bEnaPerfJobs        = EC_FALSE;  /* enable job measurements */
    bMailboxOnly        = EC_FALSE; // mailbox ?�신�?가??(PREOP?�태까�?�?.)
    nVerbose            = -1; /* command line overwrite config file setting */

    eDcmMode           = eDcmMode_BusShift; // eDcmMode_Off; //eDcmMode_BusShift;
    pvCfgFileHandle     = EC_NULL;

    OsMemset(apLinkParms, 0, sizeof(apLinkParms));
    OsMemset(&TimingDesc, 0, sizeof(TimingDesc));
    OsMemset(&oOsParms, 0, sizeof(EC_T_OS_PARMS));

    szCommandLine[0] = '\0';

    /* OS specific initialization */
    oOsParms.dwSize                         = sizeof(EC_T_OS_PARMS);
    oOsParms.dwSupportedFeatures            = 0xFFFFFFFF;


    /* add hook to log all EtherCAT messages. Without hook no messages will be generated! */
    OsAddDbgMsgHook(OsDbgMsgHookWrapper);

    /* Initialize Timing Event descriptor */
    TimingDesc.bShutdown          = EC_FALSE;
    TimingDesc.bIsRunning         = EC_FALSE;
    TimingDesc.dwBusCycleTimeUsec = CYCLE_TIME * 1000;

    /* build szCommandLine from argument list */
    EcChar* pcStrCur   = szCommandLine;
    EC_T_INT   nStrRemain = COMMAND_LINE_BUFFER_LENGTH;
    EcChar  szStrFormat[] = "%s";
    /* build szCommandLine from argument list, skipping executable name */
    for (argc--, argv++; argc > 0; argc--, argv++) {
        EcBool bIsFileName = EC_FALSE;

        /* insert next argument */
        OsSnprintf(pcStrCur, nStrRemain - 1, szStrFormat, *argv);

        /* check for file name */
        if (0 == OsStrcmp(pcStrCur, "-f")) {
            bIsFileName = EC_TRUE;
        }
        /* adjust string cursor */
        nStrRemain -= (EC_T_INT)OsStrlen(pcStrCur);
        pcStrCur = pcStrCur + OsStrlen(pcStrCur);

        /* insert space */
        OsStrncpy(pcStrCur, " ", nStrRemain - 1); nStrRemain--; pcStrCur++;

        if (bIsFileName && (1 < argc)) {
            /* move to next arg (ENI file name) */
            argc--; argv++;

            /* insert quotation mark */
            OsStrncpy(pcStrCur, "\"", nStrRemain - 1); nStrRemain--; pcStrCur++;

            /* insert ENI file name */
            OsSnprintf(pcStrCur, nStrRemain - 1, szStrFormat, *argv); nStrRemain -= (EC_T_INT)OsStrlen(pcStrCur);
            pcStrCur = pcStrCur + OsStrlen(pcStrCur);

            /* insert quotation mark */
            OsStrncpy(pcStrCur, "\" ", nStrRemain - 1); nStrRemain--; pcStrCur++;
        }
    }

    /* backup full command line */
    OsStrncpy(szFullCommandLine, szCommandLine, sizeof(szFullCommandLine) - 1);

    /* parse command line */
    for (ptcWord = OsStrtok(szCommandLine, " "); ptcWord != EC_NULL;) {
        if( 0 == OsStricmp( ptcWord, "-f") ) {
            EC_T_INT nPtcWordIndex = 3;

            /* Search for the start of the config file name. The config file
               name may start with quotation marks because of blanks in the filename */
            while(ptcWord[nPtcWordIndex] != '\0') {
                if(ptcWord[nPtcWordIndex] == '\"' || ptcWord[nPtcWordIndex] != ' ') {
                    break;
                }
                nPtcWordIndex++;
            }

            /* Depending of a config file name within quotation marks (or without
               quotation marks) extract the filename */
            if(ptcWord[nPtcWordIndex] == '\"') {
                /* Check if the strtok position is already correct */
                if(nPtcWordIndex > 3) {
                    /* More than 1 blank before -f. Correct strtok position. */
                    OsStrtok(EC_NULL,"\"");
                }
                /* Now extract the config file name */
                ptcWord = OsStrtok(EC_NULL,"\"");
            } else {
                /* Extract the config file name if it was not set within quotation marks */
                ptcWord = GetNextWord((EcChar**)&szCommandLine, &tcStorage );
            }

            if ((ptcWord == EC_NULL) || (OsStrncmp( ptcWord, "-", 1) == 0)) {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            OsSnprintf(szENIFilename, sizeof(szENIFilename) - 1, "%s", ptcWord);
        } else if( 0 == OsStricmp( ptcWord, "-log") ) {
            EC_T_INT nPtcWordIndex = 4;

            /* Search for the start of the config file name. The config file
               name may start with quotation marks because of blanks in the filename */
            while(ptcWord[nPtcWordIndex] != '\0') {
                if(ptcWord[nPtcWordIndex] == '\"' || ptcWord[nPtcWordIndex] != ' ') {
                    break;
                }
                nPtcWordIndex++;
            }

            /* Depending of a config file name within quotation marks (or without
               quotation marks) extract the filename */
            if(ptcWord[nPtcWordIndex] == '\"') {
                /* Check if the strtok position is already correct */
                if(nPtcWordIndex > 3) {
                    /* More than 1 blank before -f. Correct strtok position. */
                    OsStrtok(EC_NULL,"\"");
                }

                /* Now extract the config file name */
                ptcWord = OsStrtok(EC_NULL,"\"");
            } else {
                /* Extract the config file name if it was not set within quotation marks */
                ptcWord = GetNextWord((EcChar**)&szCommandLine, &tcStorage );
            }

            if ((ptcWord == EC_NULL) || (OsStrncmp( ptcWord, "-", 1) == 0)) {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            OsSnprintf(szLogFileprefix, sizeof(szLogFileprefix) - 1, "%s", ptcWord);
        } else if (OsStricmp( ptcWord, "-t") == 0) {
            ptcWord = GetNextWord((EcChar**)&szCommandLine, &tcStorage );
            if ( (ptcWord == EC_NULL) || (OsStrncmp( ptcWord, "-", 1) == 0) ) {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            dwDuration = OsStrtol(ptcWord, EC_NULL, 0);
        }
        else if( OsStricmp( ptcWord, "-auxclk") == 0) {
            OsDbgMsg( "Auxiliary clock not supported by this operating system!)\n" );
            goto Exit;
        }
        else if (OsStricmp( ptcWord, "-b") == 0) {
            ptcWord = GetNextWord((EcChar**)&szCommandLine, &tcStorage );
            if ( (ptcWord == EC_NULL) || (OsStrncmp( ptcWord, "-", 1) == 0) ) {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            TimingDesc.dwBusCycleTimeUsec = OsStrtol(ptcWord, EC_NULL, 0);
        }
        else if (OsStricmp( ptcWord, "-a") == 0) {
            ptcWord = GetNextWord((EcChar**)&szCommandLine, &tcStorage);
            if ( (ptcWord == EC_NULL) || (OsStrncmp( ptcWord, "-", 1) == 0) ){
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            dwCpuIndex = OsStrtol(ptcWord, EC_NULL, 0);
        } else if (OsStricmp( ptcWord, "-v") == 0) {
            ptcWord = GetNextWord((EcChar**)&szCommandLine, &tcStorage);
            if ( (ptcWord == EC_NULL) || (OsStrncmp( ptcWord, "-", 1) == 0) ) {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            nVerbose = OsStrtol(ptcWord, EC_NULL, 10);
        } else if (OsStricmp( ptcWord, "-perf") == 0) {
            bEnaPerfJobs = EC_TRUE;
        } else if (OsStricmp( ptcWord, "-mb") == 0) {
            bMailboxOnly = EC_TRUE;
        } else if (OsStricmp(ptcWord, "-dcmmode") == 0) {
            /* Extract the config file name if it was not set within quotation marks */
            ptcWord = GetNextWord((EcChar**)&szCommandLine, &tcStorage);

            if      ((EC_NULL != ptcWord) && (OsStricmp(ptcWord, "off") == 0))           eDcmMode = eDcmMode_Off;
            else if ((EC_NULL != ptcWord) && (OsStricmp(ptcWord, "busshift") == 0))       eDcmMode = eDcmMode_BusShift;
            else if ((EC_NULL != ptcWord) && (OsStricmp(ptcWord, "mastershift") == 0))    eDcmMode = eDcmMode_MasterShift;
            else if ((EC_NULL != ptcWord) && (OsStricmp(ptcWord, "masterrefclock") == 0)) eDcmMode = eDcmMode_MasterRefClock;
#if (defined INCLUDE_DCX)
                else if ((EC_NULL != ptcWord) && (OsStricmp(ptcWord, "dcx") == 0))            eDcmMode = eDcmMode_Dcx;
#endif
            else {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
        } else if (OsStricmp( ptcWord, "-cfg") == 0) {
            EC_T_INT nPtcWordIndex = 5;

            /* Search for the start of the config file name. The config file
               name may start with quotation marks because of blanks in the filename */
            while(ptcWord[nPtcWordIndex] != '\0') {
                if(ptcWord[nPtcWordIndex] == '\"' || ptcWord[nPtcWordIndex] != ' ') {
                    break;
                }
                nPtcWordIndex++;
            }

            /* Depending of a config file name within quotation marks (or without
               quotation marks) extract the filename */
            if(ptcWord[nPtcWordIndex] == '\"') {
                /* Check if the strtok position is already correct */
                if(nPtcWordIndex > 3) {
                    /* More than 1 blank before -f. Correct strtok position. */
                    OsStrtok(EC_NULL,"\"");
                }
                /* Now extract the config file name */
                ptcWord = OsStrtok(EC_NULL,"\"");
            } else {
                /* Extract the config file name if it was not set within quotation marks */
                ptcWord = GetNextWord((EcChar**)&szCommandLine, &tcStorage );
            }

            if ( (ptcWord == EC_NULL) || (OsStrncmp( ptcWord, "-", 1) == 0) ) {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            OsSnprintf(szCfgFilename, sizeof(szCfgFilename) - 1, "%s", ptcWord);
            pszCfgFilePath = szCfgFilename;
        } else if (OsStricmp( ptcWord, "-emul") == 0) {
            this->emulation_mode = true;
        } else if (OsStricmp( ptcWord, "-setmode") == 0) {
            this->set_mode = true;
        } else if (OsStrncmp( ptcWord, "-model=", 7) == 0) {
            this->modelfile = ptcWord+7;
        } else {
            dwRes = CreateLinkParmsFromCmdLine(&ptcWord, (EcChar**)&szCommandLine, &tcStorage, &bGetNextWord, &apLinkParms[dwNumLinkLayer]);
            if (EC_E_NOERROR != dwRes) {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            if (dwNumLinkLayer > 1) {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            } else {

                EC_CPUSET_ZERO(CpuSet);
                EC_CPUSET_SET(CpuSet, dwCpuIndex);
                apLinkParms[dwNumLinkLayer]->dwIstPriority = (CpuSet << 16) | RECV_THREAD_PRIO;
                dwNumLinkLayer++;
            }
        }
        /* get next word */
        if (bGetNextWord) {
            ptcWord = GetNextWord((EcChar**)&szCommandLine, &tcStorage );
        }
        bGetNextWord = EC_TRUE;
    }

    /* open config file */
    if (atOpenXmlFile(pszCfgFilePath, &pvCfgFileHandle) != EC_E_NOERROR) {
        OsDbgMsg( "ERROR: Can't open demo configuration file %s\n",  pszCfgFilePath);
        ShowSyntax();
        nRetVal = APP_ERROR;
        goto Exit;
    }

    /* get parameters from config file */

    /* ENI file name and path */
    if(szENIFilename[0] == '\0')  atParamRead(pvCfgFileHandle, eDataType_STRING, DEMO_CFG_TAG_ENI_FILENAME, (EC_T_BYTE*)szENIFilename, sizeof(szENIFilename));

    /* logging prefix */
    if(szLogFileprefix[0] == '\0') atParamRead(pvCfgFileHandle, eDataType_STRING, DEMO_CFG_TAG_LOG_FILEPREFIX, (EC_T_BYTE*)szLogFileprefix, sizeof(szLogFileprefix));

    /* Duration of demo application */
    if(dwDuration == 120000) {
        if(atParamRead(pvCfgFileHandle, eDataType_DWORD, DEMO_CFG_TAG_DURATION, (EC_T_BYTE*)&dwDuration, sizeof(dwDuration))
           == EC_E_NOERROR) {
            dwDuration = dwDuration * 1000;
        }
    }

    if( !bTimingCmdLineOverride ) {
        /* aux clk used to trigger the bus cycle */
        if (atParamRead(pvCfgFileHandle, eDataType_DWORD, DEMO_CFG_TAG_AUXCLK, (EC_T_BYTE*)&TimingDesc.dwBusCycleTimeUsec, sizeof(TimingDesc.dwBusCycleTimeUsec))
            == EC_E_NOERROR) {
            if (0 != TimingDesc.dwBusCycleTimeUsec) {
                OsDbgMsg( "Auxiliary clock not supported by this operating system!)\n" );
                nRetVal = APP_ERROR;
                goto Exit;
            }
        }
        /* bus cycle time */
        if (0 == TimingDesc.dwBusCycleTimeUsec) {
            if (atParamRead(pvCfgFileHandle, eDataType_DWORD, DEMO_CFG_TAG_BUSCYCLETIME, (EC_T_BYTE*)&TimingDesc.dwBusCycleTimeUsec, sizeof(TimingDesc.dwBusCycleTimeUsec))
                != EC_E_NOERROR) {
                TimingDesc.dwBusCycleTimeUsec = BASE_PERIOD;
            }
        }
    }

    /* Affinity for multi core cpus */
    if(dwCpuIndex == 0)  atParamRead(pvCfgFileHandle, eDataType_DWORD, DEMO_CFG_TAG_CPU_AFFINITY,  (EC_T_BYTE*)&dwCpuIndex, sizeof(dwCpuIndex));

    /* Verbosity level */
    if(nVerbose < 0)  atParamRead(pvCfgFileHandle, eDataType_DWORD, DEMO_CFG_TAG_VERBOSITY_LVL, (EC_T_BYTE*)&nVerbose, sizeof(nVerbose));
    if(nVerbose < 0) nVerbose = 1;

    /* Enable/Disable Performance measurement */
    if(!bEnaPerfJobs) atParamRead(pvCfgFileHandle, eDataType_BOOL,  DEMO_CFG_TAG_PERF_MEASURE, (EC_T_BYTE*)&bEnaPerfJobs, sizeof(bEnaPerfJobs));


    /* Disables /Enables the Master Sync Controller */
    if (DCM_ENABLED && eDcmMode_Off != eDcmMode) {
        EcBool bNoDcm = EC_TRUE;
        atParamRead(pvCfgFileHandle, eDataType_BOOL,  MOTIONDEMO_CFG_TAG_NO_BUS_SHIFT, (EC_T_BYTE*)&bNoDcm, sizeof(bNoDcm));
        if (bNoDcm) {
            eDcmMode = eDcmMode_Off;
        }
    }
    /* get parameters for the link layer */
    if(dwNumLinkLayer == 0) {
        EcChar szLinkLayerParameters[256];

        szLinkLayerParameters[0] = '\0';
        atParamRead(pvCfgFileHandle, eDataType_STRING, DEMO_CFG_TAG_LINK_LAYER, (EC_T_BYTE*)szLinkLayerParameters, sizeof(szLinkLayerParameters));
        ptcWord = OsStrtok(szLinkLayerParameters, " ");

        dwRes = CreateLinkParmsFromCmdLine(&ptcWord, (EcChar**)&szLinkLayerParameters, &tcStorage, &bGetNextWord, &apLinkParms[dwNumLinkLayer]);
        if (EC_E_NOERROR != dwRes)
        {
            nRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        if (dwNumLinkLayer > 1)
        {
            nRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        else
        {
            EC_CPUSET_ZERO(CpuSet);
            EC_CPUSET_SET(CpuSet, dwCpuIndex);
            apLinkParms[dwNumLinkLayer]->dwIstPriority = (CpuSet << 16) | RECV_THREAD_PRIO;

            dwNumLinkLayer++;
        }
    }
    SEP_LOGT("Full command line: %s\n", szFullCommandLine);

    /* determine master configuration type */
    if ('\0' != szENIFilename[0]) {
        eCnfType     = eCnfType_Filename;
        pbyCnfData   = (EC_T_BYTE*)&szENIFilename[0];
        dwCnfDataLen = 256;
    } else {
        eCnfType     = eCnfType_GenPreopENI;
    }
    if (0 == dwNumLinkLayer) {
        OsDbgMsg("Syntax error: missing link layer command line parameter\n");
        nRetVal = SYNTAX_ERROR;
        goto Exit;
    }
    /* for multi core cpus: select cpu number for this thread */
    EC_CPUSET_ZERO( CpuSet );
    EC_CPUSET_SET( CpuSet, dwCpuIndex );
    if( ! OsSetThreadAffinity(EC_NULL, CpuSet) ) {
        OsDbgMsg("ERROR: Set Affinity Failed!\n");
    }
    TimingDesc.dwCpuIndex = dwCpuIndex;

    /* create timing event to trigger the job task */
    TimingDesc.pvTimingEvent = OsCreateEvent();
    if( EC_NULL == TimingDesc.pvTimingEvent ) {
        OsDbgMsg("ERROR: insufficient memory to create timing event!\n");
        goto Exit;
    }

    dwBusCycleTimeUsec = TimingDesc.dwBusCycleTimeUsec;
    return true;
    Exit:
    return false;
}
