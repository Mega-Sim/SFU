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
namespace sephi
{
    using namespace stk_sim;

    Shelf::Shelf(StockerSystem* owner, const string _id, double z, double x, double r, double y)
    {
        owner_ = owner;
        id = _id;
        loc = {z,x,r,y};
        char ch = id.c_str()[0]; // L, R, P
        string cols = id.substr(1,3);
        string rows = id.substr(4,3);
        type = (ch == 'P') ? PORT_TYPE : SHELF_TYPE;
        side = (ch == 'L') ? LEFT_SIDE : RIGHT_SIDE;
        row = std::atoi(rows.c_str());
        col = std::atoi(cols.c_str());
        has_foup = false;
    }

    bool Shelf::checkCollision()
    {
        auto& robot = owner_->robot_;
        auto x_inpos_margin = owner_->x_inposition_margin_;
        auto y_margin = owner_->distance_to_shelf_;
        auto z_margin = owner_->shelf_plate_thickness_/2;
        auto x_frame_margin = (owner_->shelf_size_[0] - owner_->shelf_plate_thickness_)/2;

        if (robot.y() < y_margin) { // fork is in folded state
            return  false;
        }

        if (inRange(robot.z(), loc[0], z_margin)) { // may collide with plate
            if (!inRange(robot.x(), loc[1], x_inpos_margin)) {
                SEP_LOGE("Robot has crashed to %s's plate\n", id.c_str());
                return true;
            }
        } else if (robot.has_foup && robot.z() < loc[0] ){ // robot is in load position with having foup already
            SEP_LOGE("Foup has crashed to %s's plate\n", id.c_str());
            return true; // collision of foup and plate
        } else if (robot.has_foup && robot.z() > loc[0] + z_margin){ // \todo validate abuse of z_margin
            SEP_LOGE("Foup has crashed to %s's upper frame\n", id.c_str());
            return true; // collision of foup and upper frame
        } else { // may collide to shelf frame
            if (!inRange(robot.x(), loc[1], x_frame_margin)) {
                SEP_LOGE("Robot has crashed to %s's side frame\n", id.c_str());
            }
        }
        return false;
    }


    bool Shelf::inLoadUnloadPosition()
    {
        auto& robot = owner_->robot_;
        double detect_margin = 2;
        if (inRange(robot.x(), loc[1], detect_margin)) {
            if (inRange(robot.z(), loc[0]+18, detect_margin)) { // check if in load position
                return true;
            } else if(inRange(robot.z(), loc[0]-18, detect_margin)) { // chekc if in unload position
                return true;
            }
        }
        return false;
    }

}