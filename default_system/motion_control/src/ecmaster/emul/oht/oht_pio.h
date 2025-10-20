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

#ifndef SEPMASTER_OHT_PIO_H
#define SEPMASTER_OHT_PIO_H

#include "oht_map.h"
#include "oht_actor.h"
namespace sephi
{
    namespace oht_sim {
        using time_point_ns = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;
        using duration_ns = std::chrono::system_clock::duration;

        // PIO - passive device; ex. EQ
        class PIOPassiveDevice : public OHTActor {
            enum TP_INDEX {
                TP1, // tp[0] : L/U req on ~ Tr req on
                TP2, // tp[1] : ready on ~ busy on
                TP3, // tp[2] : busy on ~ carrir detect/remove
                TP4, // tp[3] : L/U req off ~ busy off
                TP5, // tp[4] : ready off ~ valid off
                TP6, // tp[5] : valid off ~ valid on : continuous handoff
                TP_INVALID,
                RTP1 = 0,   // response_time[0] : valid ~ load req
                RTP2,       // response_time[1] : tr req ~ ready
                RTP3,       // response_time[2] : detect ~ load req off
                RTP4,       // response_time[3] : complete ~ ready off
            };

            struct TimeoutJob {
                TimeoutJob(int tpindex, int timeout_ms, void (*func)(PIOPassiveDevice*));
                int tp_index; // time point index
                time_point_ns timeout;
                void (*func)(PIOPassiveDevice*);
                bool isTimeOut();
            };
            struct PIOActiveState {
                unsigned cs0:1;
                unsigned cs1:1;
                unsigned cs2:1;
                unsigned cs3:1;
                unsigned valid:1;
                unsigned tr_req:1;
                unsigned busy:1;
                unsigned complete:1;
            };
        public:
            PIOPassiveDevice();
            void setCarrierState(bool has) { has_carrier_ = has; }

            // set functions : OHT to EQ
            void connect();
            void disconnect();
            void setCSn(int n, bool on);
            void setValid(bool on);
            void setTrReq(bool on);
            void setBusy(bool on);
            void setComplete(bool on);
            bool triggerCarrierXferState(); // EQ detects load or unload; if changed, returns true
            // states
            bool hasCarrier() const { return has_carrier_;}
            bool inLoadingSeq();

            // get functions
            bool getLoadUnloadReq() const { return transfer_request_;}
            bool getReady() const { return ready_; }
            bool isError() const { return error_; }

            // update function
            void update();

            // aux functions
            void printStatus();

            static const std::array<uint32_t, 6> tp_max_ms;
            static const std::array<uint32_t, 4> response_time_ms;
        private:
            void resetIo();
            void checkTimeoutError();
            void doDelayedJobs();
            bool error_;
            bool transfer_request_; // load/unload request
            bool ready_;

            int tp_index_;
            time_point_ns tp_start_;
            std::list<TimeoutJob> delayed_jobs_;
            PIOActiveState oht_pio_state_;
            bool has_carrier_;
            bool in_loading_seq_;
            bool connected_;
        };
    }

}
#endif //SEPMASTER_OHT_PIO_H
