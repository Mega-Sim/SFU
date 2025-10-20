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

#include "oht_mark_detector.h"
using namespace sephi;
using namespace sephi::oht;

MarkDetector::MarkDetector(const char* name) : name_(name)
{
    reset();
    pulsetomm_ = 75.7245469;
    EnableChangeNodeMode_ = 0;
    NodeType_ = 0;
    ChangeNodeSetFlag_ = 0;
    ChangeNodeComboSetFlag_ = 0;
    SetAreaPosition_for_combo_ = 0;
}

void MarkDetector::reset()
{
    mark_met_trip_ = 0;//-300;
    mark_count_trip_ = 0; //-300;
    mark_temp_trip_ = 0;
    count_ = 0;
    resetWindow();
    EdgeFlag = false;
}
void MarkDetector::resetWindow()
{
    window_.clear();
}

/** \brief Mark 센서값을 업데이트 하여 mark 상태와 count를 증가 시킨다.
 *  marking 상태
 *      - signal off
 *      - signal on
 *          - mark-met : trigger시점 (노드당 1회 체크를 위해 이전 count trip 보다 MIN_MARK_DISTANCE_MM이상 차이나는 지 확인)
 *          - counted : 3회 이상 signal on일 때 (노드당 1회 체크 위해 이전 count trip 보다 MIN_MARK_DISTANCE_MM이상 차이나는 지 확인)
 * @param trip 함수가 불린 시점의 trip(encoder 값)
 * @param flag sensor 값. detected일 때 true.
 * @return count가 증가하면 true
 */
bool MarkDetector::update(double trip, bool flag, int num)
{
    bool  counted(false);
    // 최초 시작 시
    if (window_.getmarklimit() == 0) {
        if (num == 1 || num == 2) {     // 이적재
            NegMarkdistLimit = -5*pulsetomm_ + trip;
            PosMarkdistLimit = 5*pulsetomm_ + trip;
        } else {            // 주행
            if (flag) {
                // 센서가 감지 상태면
                NegMarkdistLimit = -100*pulsetomm_ + trip;
                PosMarkdistLimit = 140*pulsetomm_ + trip;
            } else {
                // 센서가 미감지 상태면
                NegMarkdistLimit = -5*pulsetomm_ + trip;
                PosMarkdistLimit = 5*pulsetomm_ + trip;
            }
        }
        //SEP_LOGT("[Mark %s] Set Limit %.0f / %.0f", name_.c_str(), NegMarkdistLimit, PosMarkdistLimit);
        window_.setmarklimit();
    }

    if (flag) {
        if (!isDetected()) {
            //SEP_LOGT("[Mark %s] countLastN(3) %d, Edge %d", name_.c_str(), window_.countLastN(3), EdgeFlag);
            raw_count_ = window_.countLastN(3);
        }

        bool overlimit = !((trip > NegMarkdistLimit) && (trip < PosMarkdistLimit));
        // 최초 Edge 감지 0 -> 1, 최소 이동거리 만족
        if (window_.countLastN(1) == 0 && overlimit) {
            mark_temp_trip_ = trip;
            EdgeFlag = true;
            //SEP_LOGT("[Mark %s] Mark met. %.1f / %d",
            //         name_.c_str(), trip, overlimit);
        } else if (isDetected() && EdgeFlag) {
            if (overlimit) {
                if  (((num == 0 || num == 3) && !EnableChangeNodeMode_) || (num == 1 || num == 2)) {
                    mark_met_trip_ = mark_temp_trip_;   // 카운트 업데이트 시 Edge Position 업데이트, 헌팅 시 Edge Position 중복 검출됨
                    ++count_;
                    counted = true;

                    NegMarkdistLimit = -100*pulsetomm_ + mark_met_trip_;
                    switch(num) {
                        case 0: // Driving Front
                            PosMarkdistLimit = 140*pulsetomm_ + mark_met_trip_;
                            break;
                        case 1: // Trans Front
                            PosMarkdistLimit = 280*pulsetomm_ + mark_met_trip_;
                            break;
                        case 2: // Trans Rear
                            PosMarkdistLimit = 105*pulsetomm_ + mark_met_trip_;
                            break;
                        case 3: // Driving Precheck
                            PosMarkdistLimit = 140*pulsetomm_ + mark_met_trip_;
                            break;
                    }
                    //SEP_LOGT("[Mark %s] Set Limit %.0f / %.0f", name_.c_str(), NegMarkdistLimit, PosMarkdistLimit);
                    //SEP_LOGT("[Mark %s] Count Update %d, Trip : %.1f", name_.c_str(), count_, mark_met_trip_);
                }
                if (num == 0 && EnableChangeNodeMode_) {
                    if (NodeType_ == 0x05) {    // OPT_TAG
                        ChangeNodeSetFlag_ = true;
                        //SEP_LOGT("[Mark %s] Set flag!", name_.c_str());
                    } else if (NodeType_ == 0x07) {   // OPT_COMBO
                        SetAreaPosition_for_combo_ = trip;
                        ChangeNodeComboSetFlag_ = true;
                        //SEP_LOGT("[Mark %s] Set combo flag!", name_.c_str());
                    }
                }
                EdgeFlag = false;
            }
        }
    }
    window_.push(flag);
    return counted;
}

bool MarkDetector::isDetected() const
{
    return (window_.countLastN(3) == 3);
}
