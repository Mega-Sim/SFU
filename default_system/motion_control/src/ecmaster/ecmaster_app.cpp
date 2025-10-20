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
// Created by master on 17. 3. 29.
//

#include <boost/algorithm/string/replace.hpp>
#include "ecmaster_app.h"
#include "ecmaster_service.h"
#include "emul/ecat_emulator.h"


using namespace sephi;

EmCommandLineOption::EmCommandLineOption(int argc, char* argv[]): error_code_(0), emulation_mode(false), set_mode(false)
{
    parse(argc, argv);
    // set yaml eni file
    enifile = (char *) pbyCnfData;
    boost::replace_last(enifile, ".xml", ".yml");
    // set yaml model if not specified
    if (modelfile.size() == 0) {
        modelfile = (char *) pbyCnfData;
        boost::replace_last(modelfile, ".xml", "_model.yml");
    }
}


int EmCommandLineOption::error()
{
    return error_code_;
}

void EmCommandLineOption::print()
{
#define PRINT_ITEM(name) printf(#name " = %d\n", name)
#define PRINT_PITEM(name) printf(#name " = %p\n", name)
    PRINT_ITEM(eCnfType);
    PRINT_ITEM(dwBusCycleTimeUsec);
    PRINT_ITEM(dwCnfDataLen);
    PRINT_ITEM(nVerbose);
    PRINT_ITEM(dwDuration);
    //EC_T_LINK_PARMS*        apLinkParms[MAX_LINKLAYER];
    PRINT_ITEM(dwCpuIndex);
    PRINT_ITEM(bEnaPerfJobs);
    PRINT_ITEM(dwNumLinkLayer);
    //EC_T_OS_PARMS           oOsParms;
    //EC_T_DCM_MODE           eDcmMode;
    PRINT_PITEM(pvCfgFileHandle);

    PRINT_ITEM(error_code_);
    PRINT_ITEM(emulation_mode);
    //PRINT_PITEM(pbyCnfData);
    printf("config file = %s\n", (char*)pbyCnfData);
    printf("model file = %s\n", modelfile.c_str());
}

static atomic_bool shutdown_service(false);
static void SignalHandler( int nSignal )
{
    SEP_LOGI("Signal %d arrived. EC Master stops...\n", nSignal);
    shutdown_service = true;
}

int main(int argc, char* argv[])
{
    printf("hello? SERVO = %d\n", ECatResource::Type::SERVO);

    DebugState::initServer();

    std::unique_ptr<EmCommandLineOption> options = std::make_unique<EmCommandLineOption>(argc, argv);
    options->print();

    // signal handler 등록
    sigset_t SigSet;
    int nSigNum = SIGALRM;
    sigemptyset(&SigSet);
    sigaddset(&SigSet, nSigNum);
    sigprocmask(SIG_BLOCK, &SigSet, NULL);
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    EcMasterSvc svc;

    svc.VERSION_ECM = 10016;
    svc.VER1 = svc.VERSION_ECM / 10000;
    svc.VER2 = (svc.VERSION_ECM - (svc.VER1*10000)) / 100;
    svc.VER3 = (svc.VERSION_ECM - (svc.VER1*10000) - (svc.VER2*100));
    SEP_LOGD("=================================\n");
    SEP_LOGD("EC-Master Version : %d.%02d.%02d\n", svc.VER1, svc.VER2, svc.VER3);
    SEP_LOGD("=================================\n");

    svc.writeEcmVersion();

    if ( svc.initService(std::move(options))) {

        if (the_ec_emulator.get()->emulationMode()) {
            the_ec_emulator.get()->runMenu();
            //svc.runTest();
        } else {
#if 1 // testmode
            //the_ec_emulator.get()->runMenu();
            svc.writeSlaveVersion();
            svc.runTest();
#else
            SEP_TRACE("Ready to serve Ecat Master.\n");
        while(!shutdown_service){
            // wait for stop service
            sleep(1);
        }
        SEP_TRACE("Shutdown Ecat Master service.\n");
#endif
        }
    } else {
        SEP_TRACE("Fail to init ecat master service! See the logs.\n");
    }
    svc.shutdownService();
    return 0;
}
