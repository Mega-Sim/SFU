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

#ifndef SEPMASTER_OHT_STATION_H
#define SEPMASTER_OHT_STATION_H
#include "oht_map.h"
#include "oht_actor.h"
#include "oht_pio.h"
#include "oht_info.h"
namespace sephi
{
    namespace oht_sim {
        // station model : STB, EQ

        class OHTStation {
        public:
            enum StationID{
                INVALID_STATION_ID = -1,
            };
            enum {
                IN_POSITION_MARGIN_UM = 1000, // in-position margin in micrometer scale
                IN_SLIDE_MARGIN_UM = 1000,
                IN_HOIST_MARGIN_UM = 1000,
                IN_ROT_MARGIN_DEG = 1000,
            };
            OHTStation() = default;
            OHTStation(OHTMap::StationData* st, OHTMap::TeachData* teach);
            void setFoupState(bool has);

            PIOPassiveDevice pio;
            bool hasFoup();
            uint32_t getID() const { return st_data_->id;}
            double getHoistOffset() { return st_data_->teach->hoist; }
            double getSlideOffset() { return st_data_->teach->shift; }
            bool inLoadSequence();
            bool inUnloadSequence();
            bool isOhtInStation(double oht_offset);
            bool isValidStation();
            bool isHandPushed(const OHTInfo& oht);
            bool checkCarrierTransfer(const OHTInfo& oht, std::list<uint32_t>& foup_list);
        private:
            bool is_loading_;
            OHTMap::StationData* st_data_;
            OHTMap::TeachData* teach_data_;
        };
    }
}
#endif //SEPMASTER_OHT_STATION_H
