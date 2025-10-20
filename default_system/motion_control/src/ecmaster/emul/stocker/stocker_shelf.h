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

#ifndef SEPMASTER_STOCKER_SHELF_H
#define SEPMASTER_STOCKER_SHELF_H

#include "sephi_ipc.h"
namespace sephi
{
    class StockerSystem;
    namespace stk_sim {
        struct Shelf {
            enum Const {
                SHELF_TYPE = 0,
                PORT_TYPE = 1,
                LEFT_SIDE = 0,
                RIGHT_SIDE = 1,
            };
            enum Collision {
                NOT_HERE = 0x00,
                IN_SHELF = 0x01,
                IN_FORK_EXTENDED = 0x02,
                IN_UNLOAD_POS = 0x04,
                IN_LOAD_POS = 0x08,
                IN_PORT = 0x10,
                CRASHED = 0x20
            };

            Shelf() : has_foup(false), owner_(nullptr) {}

            Shelf(StockerSystem *owner, const string id, double z, double x, double r, double y);

            bool checkCollision();

            bool inLoadUnloadPosition();

            string id;
            int16_t type; // shelf_type, port_type
            int16_t side; // left_side, right size
            int32_t row; // layer
            int32_t col; // column
            std::array<double, 4> loc; // location (z,x,r,y)
            bool has_foup;
            StockerSystem *owner_;

            friend class Robot;

            friend class StockerSystem;
        };
    }
}
#endif //SEPMASTER_STOCKER_SHELF_H
