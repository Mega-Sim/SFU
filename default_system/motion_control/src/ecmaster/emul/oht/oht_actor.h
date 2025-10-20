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

#ifndef SEPMASTER_OHT_ACTIVE_OBJECT_H
#define SEPMASTER_OHT_ACTIVE_OBJECT_H
#include "sephi_ipc.h"

namespace sephi {
    namespace oht_sim {

        class OHTObject {
        public:
            virtual string getStatus() { return ""; } // json style status
        };

        class OHTActor : OHTObject {
        public:
            OHTActor();
            virtual bool initialize(){ return true; }
            virtual void update() = 0; // time update
            virtual void finalize() {}
            // run
            bool runAsThread();
            void stopThread();

        protected:
            double update_cycletime_;
            bool thread_stop_;
            std::unique_ptr<std::thread> th_;
        };

    }
}
#endif //SEPMASTER_OHT_ACTIVE_OBJECT_H
