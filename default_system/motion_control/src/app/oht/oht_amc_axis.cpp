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
// Created by root on 18. 1. 12.
//
#include <limits>
#include "oht_amc_axis.h"
using namespace sephi;

RangeI32 SingleAxisAmc::getPosLimitPulse(int ax)
{
    const Range &lim = motors_[ax]->getPosLimit();
    if (lim.first == lim.second) {
        // v1.02.30  1<<29로 하면 pulse로 바꾸는 과정에서 overflow 발생, 24로 해야 안남  //CIS
        return RangeI32 {-(1<<24), (1<<24)-1};//std::numeric_limits<int32_t>::min() / 1<<2, std::numeric_limits<int32_t>::max() / 1<<2};
    } else {
        return RangeI32 {mm2pulse(lim.first), mm2pulse(lim.second)};
    }
}


bool SingleAxisAmc::setPosLimitPulse(int32_t pos_min, int32_t pos_max)
{
    double min = pulse2mm(pos_min);
    double max = pulse2mm(pos_max);
    return setPosLimit(min, max);
}

int SingleAxisAmc::moveDistancePulse(double dist, double max_v, double max_a, double max_d, double duration)
{
    //SEP_LOGD("%s moveDistancePulse dist pulse = %f, mm = %f, pos = %f, v_max=%.1f=mm/s a=%.1f=mm d=%.1f=mm",
    //         name_.c_str(), dist, pulse2mm(dist), getPos(), pulse2mm(max_v), pulse2mm(max_a), pulse2mm(max_d));
    dist = pulse2mm(dist);
    max_v = pulse2mm(max_v);
    max_a = pulse2mm(max_a);
    max_d = pulse2mm(max_d);
    return moveDistance(dist, max_v, max_a, max_d, duration);
}

int SingleAxisAmc::moveDistanceIterativePulse(double dist, double v_end, double max_v, double max_a, double max_d)
{
    dist = pulse2mm(dist);
    v_end = pulse2mm(v_end);
    max_v = pulse2mm(max_v);
    max_a = pulse2mm(max_a);
    max_d = pulse2mm(max_d);
    return moveDistanceIterative(dist, v_end, max_v, max_a, max_d);
}

int SingleAxisAmc::moveToPulse(double pos, double max_v, double max_a, double max_d, double duration)
{
    pos = pulse2mm(pos);
    max_v = pulse2mm(max_v);
    max_a = pulse2mm(max_a);
    max_d = pulse2mm(max_d);
    return moveTo(pos, max_v, max_a, max_d, duration);
}

int SingleAxisAmc::rotateAtPulse(double vel, double max_v, double max_a, double max_d, double duration)
{
    vel = pulse2mm(vel);
    max_v = pulse2mm(max_v);
    max_a = pulse2mm(max_a);
    max_d = pulse2mm(max_d);
    return rotateAt(vel, max_v, max_a, max_d, duration);
}

bool SingleAxisAmc::stopPulse(double dec, bool emergent)
{
    dec = pulse2mm(dec);
    return stop(dec, emergent);
}


void SingleAxisAmcDrive::resetSmallAdd()
{
    target_limit_ = std::numeric_limits<int32_t>::lowest();
    small_add_enabled_ = false;
    small_add_distance_actual_ = 0;
    small_add_distance_ = 0;
    small_add_v_ = 0;
    small_add_a_ = 0;
    small_add_d_ = 0;
    if (detector_) {
        ((oht::MarkDetector *) detector_)->resetWindow();
    }
}

void SingleAxisAmcDrive::setDetector(const oht::MarkDetector* detector)
{
    detector_ = detector;
}

int32_t SingleAxisAmcDrive::getMarkPosition()
{
//    SEP_REQUIRE_RET(detector_ != nullptr, 0);
    if(detector_ == nullptr) return 0;
    if (detector_->isDetected()) {
        auto mark =  detector_->getMarkTrip();
        int32_t mark_in_pulse = mm2pulse(mark);

        if(small_add_enabled_) {
            SEP_LOGT("[SingleAxisAmcDrive] mark pulse = %d, sm dist = %d, curr pos pulse = %d", mark_in_pulse, small_add_distance_, (int)getPosPulse());
            completeSmallAdd(mark_in_pulse + small_add_distance_);
            //resetSmallAdd();
        }
        return (mark_in_pulse == 0) ? 1 : mark_in_pulse;
    } else {
        return 0;
    }
}

bool SingleAxisAmcDrive::isSameSmallAddMotion(int32_t dist)
{
    return (small_add_distance_ == dist);
}

/** \brief OHT main으로부터 주어지는 small add 모션 명령을 처리하기 위한 함수
 *   기존 AMC제어기 구조에서는 Node  및 Station 정위치 정지를 위해서 매 주기마다 small add motion 명령을 주고,
 *   marking이 되면 더이상 명령을 내리지 않아 마지막 명령 target위치에 정지하는 구조로 동작한다.\n
 *   차세대 제어기의 경우 robot api와 motion 수행 thread가 분리되어 있어서
 *   매 사이클마다 motion planning을 시도할 경우 정상적으로 실행되지 못한다.
 *   그래서 차세대 제어기는 주어신 속도 제약 조건으로 target_limit까지 저속 모션을 1회만 실행하고,
 *   completeSmallAdd()를 통해 small add 모션을 완료 한다.
 *   completeSmallAdd()의 destination은 mark_met + 반사판크기/2로 주어진다.
 * @param target_limit small add 주행이 허용된 limit 거리
 * @param dist small add 거리 (보통 노드/스테이션 반사판크기의 1/2)
 * @param max_v small add 속도
 * @param max_a small add 가속도
 * @param max_d samll add 감속도
 * @return
 */
int SingleAxisAmcDrive::moveDistanceSmallAdd(int32_t target_limit, int32_t dist, int32_t max_v, int32_t max_a, int32_t max_d)
{
    if (!(small_add_enabled_ && isSameSmallAddMotion(dist))) {
        resetSmallAdd();
        small_add_enabled_ = true;
        target_limit_ = target_limit;
        small_add_distance_actual_ = target_limit - (int32_t)getPosPulse();
        small_add_distance_ = dist;
        small_add_v_ = max_v;
        small_add_a_ = max_a;
        small_add_d_ = max_d;
        int32_t curr_pos_pulse =  getPosPulse();
        SEP_LOGT("[SingleAxisAmcDrive] v=%.1f, a=%.1f, d=%.1f, limit=%d",small_add_v_, small_add_a_, small_add_d_, target_limit_);
        SEP_LOGT("[SingleAxisAmcDrive] moveDistanceSmallAdd, curr pos=%d pulse, sm dist = %d pulse", curr_pos_pulse, small_add_distance_);
        return moveDistancePulse(small_add_distance_actual_, small_add_v_, small_add_a_, small_add_d_);
    }
    return getCurrentMotionId();
}

int SingleAxisAmcDrive::completeSmallAdd(int32_t destination)  //stop at destination & disable small add
{
    small_add_enabled_ = false;
    int32_t curr_pos_pulse =  getPosPulse();
    SEP_LOGT("[SingleAxisAmcDrive] completeSmallAdd. curr pos = %d pulse, dest = %d pulse, vel = %d", curr_pos_pulse, destination, small_add_v_);
    //return moveToPulse(destination, small_add_v_, small_add_a_, small_add_d_);
    return moveToPulse(destination, small_add_v_, small_add_a_*2, small_add_d_*2);
}



bool SingleAxisAmcDriveRear::follow(int master_id)
{
    master_id_ = master_id;
    return motors_[0]->setMasterAxis(master_id_);
}
