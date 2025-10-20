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
// Created by sem1 on 17. 7. 25.
//
#include "signal.h"
#include "amc_oht.h"

using namespace amc;

static bool shutdown_service = false;

static void SignalHandler( int nSignal )
{
    SEP_LOGI("Signal %d arrived. AMC Emulator stops...\n", nSignal);
    shutdown_service = true;
    fclose(stdin);
}

int checkUserInterrupt(int us)
{
    fd_set rdset;
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = us;
    FD_ZERO(&rdset);
    FD_SET(fileno(stdin), &rdset);
    auto ret = select(fileno(stdin)+1, &rdset, nullptr, nullptr, &tv);
    if (ret) {
        char buffer[1024];
        fgets(buffer, sizeof(buffer), stdin); // flush stdin
        if (buffer[0] == 'q' || buffer[0] =='x') {
            printf("Emergent Stop!\n");
            return 2;
        }
        if(buffer[0] == 'p') {
            return 3;
        }
        return 1;
    }
    return 0;
}

int main(int argc, const char* argv[])
{
    Amc4OHT oht_emu;
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    oht_emu.VERSION_DSP = 10233;
    oht_emu.VER1 = oht_emu.VERSION_DSP / 10000;
    oht_emu.VER2 = (oht_emu.VERSION_DSP - (oht_emu.VER1*10000)) / 100;
    oht_emu.VER3 = (oht_emu.VERSION_DSP - (oht_emu.VER1*10000) - (oht_emu.VER2*100));
    SEP_LOGT("=================================\n");
    SEP_LOGT("AMC Version : %d.%02d.%02d\n", oht_emu.VER1, oht_emu.VER2, oht_emu.VER3);
    SEP_LOGT("=================================\n");
    oht_emu.start();
    oht_emu.printState();


    printf("Press any key if read state, or q or x for emergent stop.\n");
    while(!shutdown_service) {
        sleep(1);   // for nohup command, 231117 yjchoe
        switch(checkUserInterrupt(100000)) {
            case 1:
                printf("key in!\n");
                oht_emu.printState();
                break;
            case 2:
                shutdown_service = true;
                break;
            default:
                break;
        }
    }

    oht_emu.stop();

    return 0;
}
