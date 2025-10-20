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
// Created by root on 17. 11. 22.
//

#include "oht_map.h"
#include "oht_obstacle.h"
#include "oht_pio.h"

using namespace sephi;
using namespace sephi::oht_sim;

int main(void)
{
    OHTMap map;
    map.loadMapData("../test/map/data");
    OHTMap::Path  path;
    map.findPath(101, 170, path);
#if 1
    for(auto node: path) {
        printf("%d, ", node);
    }
    printf("\n");
    OHTMap::Travel travel;
    travel.initialize(map, path);
    printf("Path length = %f\n", travel.getLength());
    printf("length  node displacement speed isend\n");
    double travel_length = 0;
    while(!travel.isEnd()) {
        auto loc = travel.getLocation();
        printf("%7.1f %4d %4d %6.1f %6.1f %d\n", travel_length, loc.from, loc.to, loc.displacement, travel.maxSpeed(), travel.isEnd() );
        travel_length += 1000;
        travel.moveDistance(1000);
    }
#elif 0
    OtherOHT oht;
    oht.setMap(map);
    oht.setTarget(101, 170);
    double t = 0;
    printf("t node disp speed\n");
    while(!oht.travel().isEnd()) {
        printf("%.3f %d %.1f %.1f\n", t, oht.travel().getLocation().first, oht.travel().getLocation().second, oht.velocity());
        oht.update();
        t += 1e-3;
    }
#else
    PIOPassiveDevice eq;

    eq.runAsThread();

    printf("set cs0\n");
    eq.setCSn(0, true);
    usleep(10000);
    printf("set valid\n");
    eq.setValid(true);
    printf("wait load req\n");
    while(!eq.getLoadUnloadReq()) usleep(10000);
    printf("set tr req\n");
    eq.setTrReq(true);
    printf("wait ready\n");
    while(!eq.getReady()) usleep(10000);
    printf("set busy\n");
    eq.setBusy(true);
    // do load
    sleep(1);
    printf("foup moved to eq\n");
    eq.triggerCarrierXferState();
    printf("wait load req off\n");
    while(eq.getLoadUnloadReq()) usleep(10000);
    usleep(100000);
    printf("reset busy\n");
    eq.setBusy(false);
    usleep(100000);
    printf("set complete\n");
    eq.setComplete(true);
    usleep(100000);
    printf("reset tr req\n");
    eq.setTrReq(false);
    printf("wait ready off\n");
    while(eq.getReady()) usleep(10000);
    usleep(200000);
    printf("reset cs0, valid, complete\n");
    eq.setCSn(0, false);
    eq.setValid(false);
    eq.setComplete(false);
    usleep(100000);
    eq.stopThread();

#endif
    return 0;
}