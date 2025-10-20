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
// Created by root on 17. 12. 11.
//

#include "oht_localization.h"
#include "oht_controller.h"
using namespace sephi;
using namespace sephi::oht;

Localization::Localization(OHTController *controller)
    : parent (controller), trans_bcr_(controller->trans_bcr_.get()), driving_bcr_(controller->driving_bcr_.get()),
      node_counter_("node mark"), node_precounter_("node precheck"), trans_front_detector_("trans front"), trans_stop_dectector_("trans stop")
{
    name_ = "Localization";
    try{
        initialize();
    } catch (std::exception & e) {
        SEP_LOGE("Fail to initialize localization behavior! %s\n", e.what());
        throw e;
    }
}

bool Localization::initialize()
{
    mark_state_ = MarkState::NONE;
    mark_type_ = INVALID_MARK;
    // initialize location from location
    auto& loc = config_.initial_location;
    setLocation(loc.from, loc.to, loc.offset);
}

void Localization::setLocation(uint32_t fr, uint32_t to, double displacement)
{
    location_.set(fr, to, displacement);
    SEP_LOGT("Set Location : from %d to %d offset %f\n", fr, to, displacement);
}

void Localization::updateLocation()
{
    auto trip = getPosmm(OHT_WHEEL_F);
    //if (drive_path_->isValid()) { // in case that path is given, get location from path
    //    drive_path_->getLocation(trip, location_);
    //}
}


bool Localization::updateLocationUsingBCR()
{
    // try drving bcr, and
//    auto bcr_code = driving_bcr_->readId();
//    if (bcr_code != BcrBL185::INVALID_ID){
//        try{
//            SEP_LOGD("Update location using NODE BCR Code %d ", bcr_code);
//            auto steer = getSteerDir();
//            auto& node = map_.node_map[bcr_code];
//            auto link = map_.getLinkTo(bcr_code);
//            setLocation(link->from, node.id,  link->distance);
//            return true;
//        }catch(...) {
//            SEP_LOGD("NODE BCR Code %d is not found on map!", bcr_code);
//        }
//    }
//    bcr_code = trans_bcr_->readId();
//    if (bcr_code != BcrBL185::INVALID_ID) {
//        try {
//            SEP_LOGD("Update location using Trans BCR Code %d ", bcr_code);
//            setLocation(map_.station_map[bcr_code].parent, map_.station_map[bcr_code].next, map_.station_map[bcr_code].offset);
//        } catch(...) {
//            SEP_LOGD("TRANS BCR Code %d is not found on map!", bcr_code);
//        }
//    }
    return false;
}

void Localization::reset()
{
    node_counter_.reset();
    node_precounter_.reset();
    trans_front_detector_.reset();
    trans_stop_dectector_.reset();

    setMarkType(MarkType::INVALID_MARK);
}

void Localization::resetWindows()
{
    node_counter_.resetWindow();
    node_precounter_.resetWindow();
    trans_front_detector_.resetWindow();
    trans_stop_dectector_.resetWindow();
}

void Localization::countNodes()
{
    auto node_mark_size = config_.node_mark_size;
    //double trip = getPosmm(OHT_WHEEL_F);
    double trip = getPos(OHT_WHEEL_F);

    auto node_count_increased = node_counter_.update(trip, readDi(ioConfig_.getportid("_stop_driving_front")), 0);
    if (node_count_increased){
        //SEP_LOGT("node_counter increased : %d, trip = %.1f", node_counter_.isDetected(), trip);
        //auto bcr_node = driving_bcr_->readId();
       // auto wp = drive_path_->getWayPointById(bcr_node);
       // if (wp && bcr_node != wp->node_id) {
       //     SEP_LOGE("Node id mismatching! expected = %d, actual = %d", wp->node_id, bcr_node);
       // } else {
        //    SEP_LOGT("TODO: location update with bcr code");
       // }
       // SEP_LOGT("BCR code = %d\n", bcr_node);
    }
    node_precounter_.update(trip, readDi(ioConfig_.getportid("_stop_driving_precheck")), 3);
    trans_front_detector_.update(trip, readDi(ioConfig_.getportid("_stop_trans_front")), 1);
    trans_stop_dectector_.update(trip, readDi(ioConfig_.getportid("_stop_trans_rear")), 2);

}

// marking
bool Localization::setMarkType(MarkType mark_type, MarkState init_state )
{
    mark_type_ = mark_type;
    mark_state_ = init_state;
    return false;
}

double Localization::getMarkTrip() const
{
    switch(mark_type_) {
        case NODE_MARK:     return node_counter_.getMarkTrip();
        case STATION_MARK:  return trans_stop_dectector_.getMarkTrip();
        case INVALID_MARK: /* fall through */
        case QR_MARK:
            break;
    }
    return -1;
}

void Localization::updateMarkState()
{
    if (mark_type_ == MarkType::INVALID_MARK) return;

    auto targetReached = [this]() {
        auto cmd = owner_->front_wheel_->getCurrentCommand();
        auto res = (fabs(owner_->front_wheel_->getCurrentCommand().target - getPosmm(OHT_WHEEL_F)) <= config_.inpos[OHT_WHEEL_F]);
        //SEP_LOGT("target reached = %d; cmd.target=%.1f, pos=%.1f inpos=%.1f\n", res, cmd.target, getPosmm(OHT_WHEEL_F),  config_.inpos[OHT_WHEEL_F]);
        //return res;
                return true;
    };

    bool precheck(false), stopcheck(false);
    if (mark_type_ == MarkType::NODE_MARK) {
        precheck = readDi(ioConfig_.getportid("_stop_driving_precheck"));
        stopcheck = readDi(ioConfig_.getportid("_stop_driving_front"));
    } else if (mark_type_ == MarkType::STATION_MARK) {
        precheck = readDi(ioConfig_.getportid("_stop_trans_front"));
        stopcheck = readDi(ioConfig_.getportid("_stop_trans_rear"));
        //SEP_LOGT("precheck = %d, stop = %d\n", precheck, stopcheck);
    }

    const char* mark_string[] = { "NONE", "MARKING", "MARKED", "OVERRUN"};
    switch (mark_state_) {
    case MarkState::NONE:
        if (precheck) {
            mark_state_ = MarkState::MARKING;
            SEP_LOGT("Mark State = %s\n", mark_string[mark_state_]);
        } else if (stopcheck && targetReached()) {
            mark_state_ = MarkState::MARKED;
            SEP_LOGT("Mark State = %s\n", mark_string[mark_state_]);
        }
        break;
    case MarkState::MARKING:
        if (stopcheck && targetReached()) {
            mark_state_ = MarkState::MARKED;
            SEP_LOGT("Mark State = %s\n", mark_string[mark_state_]);
        }
        break;
    case MarkState::MARKED:
        if (!stopcheck) {
            mark_state_ = MarkState::OVERRUN;
            SEP_LOGT("Mark State = %s\n", mark_string[mark_state_]);
        }
        break;
    }

#if 0 // location correction
    // correct location using marking information
    auto curr_link = map_.getLink(location_.from, location_.to);
    if (mark_type_ == MarkType::NODE_MARK) {
        if (mark_state_ == MARKING && location_.offset < curr_link->distance - config_.precheck2stop) {
            location_.offset = curr_link->distance - config_.precheck2stop;
            //SEP_LOGT("Location correction using node mark state\n");
        }
    } else if (mark_type_ == MarkType::STATION_MARK) {
        auto sta = curr_link->getStation(location_.offset, config_.station_mark_front_part, config_.station_mark_size-config_.station_stop_mark_size);
        if (sta != nullptr) {
            if (mark_state_ == MARKING && location_.offset < sta->offset - config_.station_mark_front_part){
                location_.offset = sta->offset - config_.station_mark_front_part;
                //SEP_LOGT("Location correction using station mark state\n");
            }
        }
    }
#endif // correction

}

void Localization::update()
{
    updateLocation();
    countNodes();
    updateMarkState();
}

void MarkUpdater::update()
{
    //double trip = getPosmm(OHT_WHEEL_F);
    double trip = getPos(OHT_WHEEL_F);
    node_counter_.update(trip, (readDi(ioConfig_.getportid("_stop_driving_front"))), 0);
    node_precounter_.update(trip, (readDi(ioConfig_.getportid("_stop_driving_precheck"))), 3);
    trans_front_detector_.update(trip, (readDi(ioConfig_.getportid("_stop_trans_front"))), 1);
    trans_stop_dectector_.update(trip, (readDi(ioConfig_.getportid("_stop_trans_rear"))), 2);
}
