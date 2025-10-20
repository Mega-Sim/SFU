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

#ifndef SEPMASTER_OHT_OBSTACLE_H
#define SEPMASTER_OHT_OBSTACLE_H
#include "oht_map.h"
#include "oht_actor.h"
#include "velocity_profile.h"
namespace sephi
{
    namespace oht_sim {
        class OtherOHT : public  OHTActor {
        public:
            OtherOHT();
            void setMap(OHTMap& map) { map_ = &map;}
            void enable() { enabled_ = true;}
            void disable() { enabled_  = false;}
            bool isEnabled() const { return enabled_; }
            bool setTarget(uint32_t start_node, uint32_t target_node, float v_max = -1);
            void update() override ;
            void pause();
            void resume();
            double velocity();
            const OHTMap::Travel& travel() { return travel_;}
        private:
            void setProfile();
            OHTMap *map_;
            OHTMap::Path path_;
            OHTMap::Travel travel_;
            TrVelocityProfile speed_profile_;
            double profile_t_;
            float v_max_;
            bool enabled_;
            bool paused_;
            std::mutex lock_;
        };
    }
}
#endif //SEPMASTER_OHT_OBSTACLE_H
