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

#ifndef SEPMASTER_STOCKER_ROBOT_H
#define SEPMASTER_STOCKER_ROBOT_H

#include "sephi_ipc.h"
#include "servo_motor.h"
namespace sephi
{
    namespace stk_sim {
        class Robot {
        public:
            Robot() : has_foup(false) {}
            void setPPMM(int axis, double ppmm, int32_t offs_puls, double frame_size_); // pulse per mm, offset pulse
            void setSize(double w, double l); // width, length
            void updateEncoder(int32_t z, int32_t x, int32_t r, int32_t y);
            double z() const { return pos_[0]; }
            double x() const { return pos_[1]; }
            double r() const { return pos_[2]; }
            double y() const { return pos_[3]; }
            bool movingDown() const { return pos_[0] < prev_pos_[0]; }
            bool movingUp() const { return pos_[0] > prev_pos_[0]; }
            bool checkCrash();

        public:
            bool has_foup;
        private:
            std::array<double, 4> ppmm_; //pulse per mm, mm, deg, mm; z, x, r, y
            std::array<int32_t, 4> offs_; //offset pulse; z, x, r, y
            std::array<double, 4> pos_; // location in mm, mm, deg, mm
            std::array<double, 4> prev_pos_; // previous location in mm, mm, deg, mm
            double width_, legth_; // x-size, y-size
            std::array<Range, 4> ranges_;
        };
    }
}
#endif //SEPMASTER_STOCKER_ROBOT_H
