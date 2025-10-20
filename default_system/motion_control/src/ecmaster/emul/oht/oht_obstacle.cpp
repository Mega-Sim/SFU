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

#include "oht_obstacle.h"
#include "math_utility.h"

using namespace sephi;
using namespace sephi::oht_sim;

// given v0 및 acc 조건 하에서 displacement를 이동하는 데 걸리는 최소 시간 시간
// displacement = v0*t + 0.5 * acc * t^2 ; 그 시점의 속도는 v0+a*t
static double calculateMinAccTime(double dispisplacement, double v0, double acc)
{
    double coef[] = { 0.5* acc, v0, -dispisplacement};
    double r[2] = {0,0};
    solveQuadraticEquation(coef, r);
    return r[0] > 0 ? r[0] : r[1];
}

OtherOHT::OtherOHT()
    : profile_t_(0), speed_profile_(5000, 1000, 1000), enabled_(false)
{
}

/// generates speed profile of current link
void OtherOHT::setProfile()
{
    std::lock_guard<std::mutex> guard(lock_);

    if (travel_.isEnd()) return;
    double v0 = speed_profile_.Vel(profile_t_);
    auto link = travel_.getLink();
    auto nextlink = travel_.getNextLink();
    if (nextlink == nullptr) { // last link
        //SEP_TRACE("link = null\n");
        speed_profile_.SetProfileVelocity(0, link->distance, std::min(link->speed, v_max_), v0, 0);
    } else if (paused_){
        //SEP_TRACE("paused\n");
        double dec = speed_profile_.GetMaxDec();
        double stop_time = v0 / dec;
        double stop_distance = 0.5 * dec * (stop_time*stop_time);
        speed_profile_.SetProfileVelocity(0, stop_distance, link->speed, v0, 0);
        paused_ = false;
    } else {
        //SEP_TRACE("normal\n");
        double endspeed = std::min(link->speed, nextlink->speed);
        double distance = link->distance - travel_.getLocation().displacement;
        endspeed = std::min(endspeed, double(v_max_));

        if (endspeed >= v0) {
            double t = calculateMinAccTime(distance, v0, speed_profile_.GetMaxAcc());
            if (endspeed > v0 + speed_profile_.GetMaxAcc() * t) {
                endspeed = v0 + speed_profile_.GetMaxAcc() * t;
            }
        } else {
            double t = calculateMinAccTime(distance, v0, -speed_profile_.GetMaxDec());
            if (endspeed < v0 - speed_profile_.GetMaxAcc() * t) {
                endspeed = v0 - speed_profile_.GetMaxAcc() * t;
            }
        }

        SEP_LOGT("Node from %d to %d: v0=%f next.speed=%f realend =%f \n", link->from, link->to, v0, nextlink->speed, endspeed);
        speed_profile_.SetProfileVelocity(0, distance, std::min(link->speed, v_max_), v0, endspeed);
    }
    profile_t_ = 0;
}

bool OtherOHT::setTarget(uint32_t start_node, uint32_t target_node, float v_max)
{
    path_.clear();
    SEP_REQUIRE_RET(map_->findPath(start_node, target_node, path_), false);
    travel_.initialize(*map_, path_);
    v_max_ = (v_max > 0) ? v_max : std::numeric_limits<float>::max();
    paused_ = false;
    setProfile();
    return true;
}

void OtherOHT::update()
{
    if (!enabled_) return;

    // \todo : CID processing
    if (!travel_.isEnd()) {
        auto from = speed_profile_.Pos(profile_t_);
        profile_t_ += update_cycletime_;
        auto to = speed_profile_.Pos(profile_t_);
        auto node = travel_.getLocation().from;
        travel_.moveDistance(to-from);
        if (node != travel_.getLocation().from) { // node changed
            setProfile();
        }
    }
}
void OtherOHT::pause()
{
    paused_ = true;
    setProfile();
}

void OtherOHT::resume()
{
    paused_ = false;
    setProfile();
}

double OtherOHT::velocity()
{
    return speed_profile_.Vel(profile_t_);
}
