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
// Created by root on 17. 12. 11.
//

#ifndef SEPMASTER_OHT_LOCALIZATION_H
#define SEPMASTER_OHT_LOCALIZATION_H
#include "oht_behavior_base.h"
#include "oht_mark_detector.h"
#include "oht_io_config.h"

namespace sephi {
    namespace oht {

        class Localization: public Perception {
        public:
            enum MarkState {
                NONE,
                MARKING,
                MARKED,
                OVERRUN
            };
            enum MarkType {
                INVALID_MARK,
                NODE_MARK,
                STATION_MARK,
                QR_MARK,
            };
            using parent = Perception;
        public:

            Localization(OHTController *controller);
            bool initialize() override; // load map
            void update() override;
            void setLocation(uint32_t fr, uint32_t to, double displacement = 0);
            bool setMarkType(MarkType mark_type, MarkState initState = MarkState::NONE);
            void reset();
            void resetWindows();

            void resetCounters();
            const MarkDetector& nodeCounter() const { return node_counter_;}
            const MarkDetector& nodePrecheckCounter() const { return node_precounter_;}
            const MarkDetector& transFrontDetector() const { return trans_front_detector_;}
            const MarkDetector& transStopDetector() const { return trans_stop_dectector_;}
            int getMarkingState() const { return mark_state_; }
            int getMarkType() const { return mark_type_;}
            double getMarkTrip() const;
            //SteerDir getSteerDir();

            const OHTLocation& getLocation() const { return location_; }
            bool updateLocationUsingBCR();
            void updateMarkState();
            OhtIoConfig ioConfig_;

        protected:
            void updateLocation();
            void countNodes();


            OHTLocation location_;
            //DrivePath* drive_path_;
            MarkDetector node_counter_;
            MarkDetector node_precounter_;
            MarkDetector trans_front_detector_;
            MarkDetector trans_stop_dectector_;
            MarkState mark_state_;
            MarkType mark_type_;
            BcrBL185* trans_bcr_;
            BcrBL185* driving_bcr_;

            friend class Driving;
        };

        class MarkUpdater: public Localization {
            using parent = Localization;
        public:
            MarkUpdater(OHTController* controller): parent(controller) {}
            void update() override;

        };
    }
}
#endif //SEPMASTER_OHT_LOCALIZATION_H
