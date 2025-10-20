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
// Created by root on 18. 1. 23.
//

#ifndef SEPMASTER_OHT_MARK_DETECTOR_H_H
#define SEPMASTER_OHT_MARK_DETECTOR_H_H
#include "sephi_ipc.h"

namespace sephi {
    namespace oht {
        template<int WINDOW_SIZE = 3>
        class CountWindow {
        public:
            CountWindow(): index_(0){ data_.fill(false);}
            bool push(bool val) {
                data_[index_++] = val;
                index_ %= WINDOW_SIZE;
            }
            void clear() {
                for(auto& v: data_) v = false;
                setmark_ = 0;
            }
            int count() {
                int cnt = 0;
                for(auto v: data_) cnt += (v) ? 1 : 0;
                return cnt;
            }
            int countLastN(int n) const {
                if (n <= 0) return 0;
                if (n > WINDOW_SIZE) n = WINDOW_SIZE;
                int index = index_;
                int cnt = 0;
                while(n > 0) {
                    index = (index - 1 + WINDOW_SIZE) % WINDOW_SIZE;
                    if (data_[index]) ++cnt;
                    --n;
                }
                return cnt;
            }
            int setmarklimit() { return setmark_ = 1; }
            int getmarklimit()  { return setmark_; }
            int size() const { return WINDOW_SIZE; }
        private:
            std::array<bool,WINDOW_SIZE> data_;
            int index_;
            int setmark_;
        };

        /** \brief Marking 센서 값을 모니터링하면서 mark detection여부를 감지한다.
         *
         *  _stop_driving_front, _stop_trans_front, _stop_trans_rear등 node 및 station marking에
         *  필요한 센서들의 상태 업데이트 하면서 감지 여부를 판단한다.
         */
        class MarkDetector {
        public:
            enum Constants : int {MIN_MARK_DISTANCE_MM = 200};
        public:
            MarkDetector(const char* name);
            void reset();
            void resetWindow();
            void setCount(int new_count) const { count_ = new_count; }
            bool update(double trip, bool flag, int num);
            bool isDetected() const;
            double getMarkTrip() const { return mark_met_trip_; } ///< 가장 최근 mark trigger 시점의 trip을 가져온다.
            int getCount() const { return count_; } ///< mark 카운트 회수를 반환
            const char* getName() const { return name_.c_str(); }
            void setOptionalNode(int set_node) const { EnableChangeNodeMode_ = set_node; }
            void setNodeType(char Type) const { NodeType_ = Type;}
            void setPulsetoMM(double gear_ratio) const { pulsetomm_ = gear_ratio; }
            //double getMarkdistPulse() const { return Min_Markdist_Pulse; }
            //void setMarkdistMM(double mm) const { Min_Markdist_Pulse = mm * pulsetomm_; }
            void setChangeNodeSetFlag(int flag) const { ChangeNodeSetFlag_ = flag; }
            void setChangeNodeComboSetFlag(int flag) const { ChangeNodeComboSetFlag_ = flag; }
            int getChangeNodeSetFlag() const { return ChangeNodeSetFlag_; }
            int getChangeNodeComboSetFlag() const { return ChangeNodeComboSetFlag_; }
            int getAreaPosition_for_combo() const { return SetAreaPosition_for_combo_; }
            void setAreaPosition_for_combo(int pulse) const { SetAreaPosition_for_combo_ = pulse; }
            int getRawCount() const { return raw_count_; }
        private:
            CountWindow<5> window_;
            double mark_met_trip_;      ///< mark sensor가 trigger된 시점의 trip
            double mark_count_trip_;    ///< mark counting이 된 시점의 trip
            mutable int count_;
            string name_;
            bool EdgeFlag;
            double NegMarkdistLimit;
            double PosMarkdistLimit;
            double mark_temp_trip_;
            mutable int EnableChangeNodeMode_;
            mutable char NodeType_;
            mutable int ChangeNodeSetFlag_;
            mutable int ChangeNodeComboSetFlag_;
            mutable int SetAreaPosition_for_combo_;
            mutable double pulsetomm_;
            mutable int raw_count_;
        };
    }
}
#endif //SEPMASTER_OHT_MARK_DETECTOR_H_H
