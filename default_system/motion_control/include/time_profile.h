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
// Created by sem on 18. 5. 16.
//

#ifndef SEPMASTER_TIME_PROFILE_H
#define SEPMASTER_TIME_PROFILE_H
#include <chrono>
#include <limits>
#include <string>
#include <cmath>
namespace sephi {

    struct TimeProfileItem {
        std::chrono::system_clock::time_point ts;  ///< time stamp
        double min;         ///< min of diff (pt[k], pt[k+1])
        double max;         ///< max of diff
        double sum;         ///< sum of diff's
        double sum2;        ///< sum of square of diff's
        int count;
        std::string name;
        TimeProfileItem(): name("probe") { reset();}
        TimeProfileItem(const char* _name): name(_name) {reset();}
        void reset() {
            sum = sum2 = 0.0;
            min = std::numeric_limits<double>::max();
            max = std::numeric_limits<double>::min();
            count = 0;
        }
        void beginInterval(std::chrono::system_clock::time_point& now) {
            ts = now;
        }
        void endInterval(std::chrono::system_clock::time_point& now) {
            probe(now, true);
        }
        void probeCycle(std::chrono::system_clock::time_point& now) {
            probe(now, false);
        }

        void probe(std::chrono::system_clock::time_point& now, bool interval) {
            if (count > 0 || interval) {
                auto diff = (now - ts).count()*1e-9;
                if (diff < min) min = diff;
                else if (diff > max) max = diff;
                sum += diff;
                sum2 += diff*diff;
            }
            ts = now;
            ++count;
            return;
        }
        // stat
        double ave() const { return (count<=0) ? 0 : sum / count; }
        double std() const {
            if (count<=0) return 0;
            auto av = ave();
            return sqrt((sum2 - 2*av*sum + count*av*av)/count);
        }

        char* statistics(char *buff, int buff_size, double multiply) const {
            snprintf(buff, buff_size, "%12s  %4.f  %4.f  %4.f  %4.1f", name.c_str(), min*multiply, ave()*multiply, max*multiply, std()*multiply);
            return buff;
        }

    };

}

#endif //SEPMASTER_TIME_PROFILE_H
