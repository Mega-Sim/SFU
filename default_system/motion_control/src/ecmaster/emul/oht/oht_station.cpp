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
// Created by root on 17. 11. 23.
//

#include "oht_station.h"
using namespace sephi;
using namespace sephi::oht_sim;


OHTStation::OHTStation(OHTMap::StationData* st, OHTMap::TeachData* teach)
{
    st_data_ = st;
    teach_data_ = teach;
    is_loading_ = false;
}

void OHTStation::setFoupState(bool has)
{
    pio.setCarrierState(has);
}

bool OHTStation::hasFoup()
{
    return pio.hasCarrier();
}

bool OHTStation::inLoadSequence()
{
    return pio.inLoadingSeq();
}

bool OHTStation::inUnloadSequence()
{
    return !pio.inLoadingSeq();
}

bool OHTStation::isOhtInStation(double oht_offset)
{
    return (fabs(st_data_->offset - oht_offset) <= IN_POSITION_MARGIN_UM/1000.0);
}


bool OHTStation::isValidStation()
{
    return (st_data_->teach != nullptr);
}

bool OHTStation::isHandPushed(const OHTInfo& oht)
{
    auto& teach = *st_data_->teach;

    bool ret =  (fabs(teach.shift - oht.slide) <= IN_SLIDE_MARGIN_UM/1000.0
                && fabs(teach.hoist - oht.hoist) <= IN_HOIST_MARGIN_UM/1000.0);
    //SEP_LOGD("OHTStation::isHandPushed  teach(hoist,slide)=%.3f, %.3f, real=%.3f,%.3f; pushed=%d", teach.hoist, teach.shift, oht.hoist, oht.slide, ret);
    return ret;
}

bool OHTStation::checkCarrierTransfer(const OHTInfo& oht, std::list<uint32_t>& foup_list)
{
    //auto& teach = *st_data_->teach;
    if (isHandPushed(oht)) {
        if (pio.triggerCarrierXferState()) {
            if (pio.hasCarrier()) { // foup : oht -> eq
                foup_list.remove(0xFFFFFFFF);
                foup_list.push_back(st_data_->id);
            } else { // foup : eq --> oht
                foup_list.push_back(0xFFFFFFFF);
                foup_list.remove(st_data_->id);
            }
            return true;
        }
    }
    return false;
}
