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

#ifndef SEPMASTER_OHT_INFO_H
#define SEPMASTER_OHT_INFO_H

#include "math_utility.h"
#include "oht_map.h"
#include "oht_hoist_unit_conversion.h"
namespace sephi
{
    namespace oht_sim
    {
        enum class HandOpenCloseState : int {
            OPEN,
            TRANSIENT,
            CLOSE,
            ORIGIN = int(CLOSE),
        };
        enum class ShutterOpenCloseState : int {
            OPEN,
            TRANSIENT,
            CLOSE,
            ORIGIN = int(OPEN),
        };
        enum class SteeringState : int {
            LEFT,
            RIGHT,
            TRANSIENT
        };

        struct OHTInfo{
            enum {WHEEL, HOIST, SLIDE};
            enum OHT_CONSTANTS {
                OHT_LENGTH_MM = 938, //
                OBS_SENS_LONG_MM = 5000,
                OBS_SENS_MIDDLE_MM = 2875,
                OBS_SENS_SHORT_MM = 750,
                OHT_SENS_RANGE_1 = 350,
                OHT_SENS_RANGE_3 = 750,
                OHT_SENS_RANGE_4 = 1813, //1812.5
                OHT_SENS_RANGE_5 = 2875,
                OHT_SENS_RANGE_6 = 3938, // 3937.5
                OHT_SENS_RANGE_7 = 5000,
            };
            double hoist;   // in mm
            double slide;   // in mm

            OhtLocation location; // first=link ptr, second= offset in mm
            bool has_foup;
            HoistUnitConverter hoist_unit_conv;
            HandOpenCloseState hand_state;
            ShutterOpenCloseState shutter_state;
            SteeringState steer_state;
        public:
            void moveDistance(oht_sim::OHTMap& map, double disp);
            void setHoistEncoder(int32_t pos) {
                hoist = hoist_unit_conv.pulse2mm(pos>>7);// pos는 2^20/rev, hoist coef는 2^13bit용
            }
            void setSlideEncoder(int32_t pos) { slide = offset[SLIDE] + pos/ppmm[SLIDE]; }
            std::array<double,3> ppmm; // pulse per millimeter
            std::array<double,3> offset; // offset of hoist & slide
            std::array<double,3> inpos; // in position margin in pulse
        };
    }
}

#endif //SEPMASTER_OHT_INFO_H
