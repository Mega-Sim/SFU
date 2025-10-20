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
// Created by root on 17. 11. 29.
//

#include "stocker_simulator.h"
namespace sephi {
    using namespace stk_sim;

// pulse per mm, offset pulse
    void Robot::setPPMM(int axis, double ppmm, int32_t offs_puls, double frame_size_) {
        ppmm_[axis] = ppmm * (1 << 7); // compensate resolution diff:  2^13(IPC) vs 2^20(Master)
        offs_[axis] = offs_puls;
        // set work range
        ranges_[axis].first =
            -offs_puls / fabs(ppmm_[axis]) - 100; // use only scale factor of ppmm except direction factor
        ranges_[axis].second = frame_size_ - ranges_[axis].first;
    }

    void Robot::setSize(double w, double l) {
        width_ = w;
        legth_ = l;
    }

    void Robot::updateEncoder(int32_t z, int32_t x, int32_t r, int32_t y) {
        prev_pos_ = pos_;
        pos_[0] = z / ppmm_[0];
        pos_[1] = x / ppmm_[1];
        pos_[2] = r / ppmm_[2];
        pos_[3] = y / ppmm_[3];
    }

    bool Robot::checkCrash() {
        for (int i = 0; i < pos_.size(); i++) {
            if (pos_[i] < ranges_[i].first || pos_[i] > ranges_[i].second) return true;
        }
        return false;
    }

}