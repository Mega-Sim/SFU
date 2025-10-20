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
// Created by root on 17. 12. 8.
//

#include "oht_info.h"
using namespace sephi;
using namespace sephi::oht_sim;

void OHTInfo::moveDistance(OHTMap& map, double displacement)
{
    auto& node_map = map.node_map;
    // move forward
    if (displacement > 0) {
        while (displacement > 0) {
            auto link = location.first;
            // inter link movement
            if (link->distance - location.second > displacement) {
                location.second += displacement;
                return;
            }
            // extra link movement
            displacement -= link->distance - location.second;
            location.second = 0;
            auto& next_node = node_map[link->to];
            if (node_map[link->to].link_count == 1) {
                location.first = next_node.link[0];
            } else {
                if (steer_state == SteeringState::LEFT) {
                    location.first = (next_node.link[0]->is_left) ? next_node.link[0] : next_node.link[1];
                } else {
                    location.first = (next_node.link[0]->is_left) ? next_node.link[1] : next_node.link[0];
                }
            }
        }
    } else {
        while (displacement < 0) {
            auto link = location.first;
            // inter link movement
            if (location.second + displacement >= 0) {
                location.second += displacement;
                return;
            }
            // extra link movement
            displacement += location.second;
            OHTMap::LinkData* prev_link = map.getLinkTo(link->from);// throw if multiple incoming nodes are found
            location.second = prev_link->distance;
            location.first = prev_link;
        }
    }
}
