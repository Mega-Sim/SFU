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
// Created by sem2 on 17. 12. 28.
//

#ifndef SEPMASTER_HOIST_UNIT_CONVERSION_H
#define SEPMASTER_HOIST_UNIT_CONVERSION_H

#include "sephi_ipc.h"
#include "math_utility.h"
namespace sephi
{
    class HoistUnitConverter {
        using Range = std::pair<double,double>;
        std::array<Range,2> pulse_range_;
        std::array<Range,2> mm_range_;
        std::array<std::array<double,4>,2> coef_;
        double mmps2pps_slope_;
        std::pair<double,double> center_value_; // first=pulse, second=mm

        double bisection(const std::array<double,4>& coef, const double min_pulse, const double max_pulse, double mm) {
            double mean_pulse = (max_pulse+min_pulse)/2;
            if (fabs(max_pulse-min_pulse) < 5) return mean_pulse;  // recursion finishing condition
            double mean_mm = pulse2mm(mean_pulse);
            if (mm < mean_mm) return bisection(coef, min_pulse, mean_pulse, mm);
            else return bisection(coef, mean_pulse, max_pulse, mm);
        };

    public:
        HoistUnitConverter()
            : pulse_range_{{{-45000, 500000},{500000, 1130000}}}, mm_range_{{{-175, 1470}, {1470, 5500}}},
              coef_{{{0.0063,-3.1771,304.3292,-3},{0.0063,-3.1771,304.3292,-3}}}
        {
            mmps2pps_slope_ = (mm_range_[1].second - mm_range_[0].first)/(pulse_range_[1].second - pulse_range_[0].first);
        }
        bool initialize(std::array<double,7>&pulselist, std::array<double,7>&mmlist);
        double pulse2mm(double pulse) {
            auto& pulse_range = (pulse <= center_value_.first) ? pulse_range_[0] : pulse_range_[1];
            auto& mm_range = (pulse <= center_value_.first) ? mm_range_[0] : mm_range_[1];
            auto& coef = (pulse <= center_value_.first) ? coef_[0] : coef_[1];

            if (pulse < pulse_range.first) return mm_range.first;
            else if (pulse > pulse_range.second) return mm_range.second;
            else return evaluatePolynomial(coef, pulse/100000);
        }
        double mm2pulse(double mm) {
            auto& pulse_range = (mm <= center_value_.second) ? pulse_range_[0] : pulse_range_[1];
            auto& mm_range = (mm <= center_value_.second) ? mm_range_[0] : mm_range_[1];
            auto& coef = (mm <= center_value_.second) ? coef_[0] : coef_[1];

            if (mm < mm_range.first) return pulse_range.first;
            else if (mm > mm_range.second) return pulse_range.second;
            return bisection(coef, pulse_range.first, pulse_range.second, mm);
        }
        double pps2mmps(double pulse_per_sec) { return pulse_per_sec * mmps2pps_slope_; }
        double mmps2pps(double mm_per_sec) {return mm_per_sec / mmps2pps_slope_; }
        double ppss2mmpss(double pulse_per_secsec) {return pulse_per_secsec * mmps2pps_slope_; }
        double mmpss2ppss(double mm_per_secsec) { return mm_per_secsec / mmps2pps_slope_;}

    };
}

#endif //SEPMASTER_HOIST_UNIT_CONVERSION_H
