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

#ifndef SEPMASTER_OHT_BEHAVIOR_BASE_H
#define SEPMASTER_OHT_BEHAVIOR_BASE_H

#include "emul/oht/oht_map.h"
#include "emul/oht/oht_io_map.h"
#include "oht_amc_axis.h"
#include "oht_onboard_io.h"
#include "oht_serial_devices.h"
#include "oht_io_port_map.h"
#include "oht_hoist_unit_conversion.h"
#include <algorithm>

namespace sephi {
    namespace oht {
        //using namespace oht_sim;
        using oht_sim::OHTMap;
        using oht_sim::OHTLocation;
        using OHTPath = std::deque<uint32_t>;
        using std::make_tuple;
        using std::make_pair;
        using std::this_thread::sleep_for;
        using std::chrono::milliseconds;
        using namespace std::chrono_literals;
        typedef oht_sim::OhtOnboardDO OnboardDI;
        typedef oht_sim::OhtOnboardDI OnboardDO;

        struct OhtConfig{
            HoistUnitConverter hoist_unit_conv;
            std::array<double,4> ppmm; // pulse per millimeter
            std::array<double,4> offset; // offset of hoist & slide
            std::array<double,4> inpos; // in position margin in pulse
            std::array<Range,4>  pos_limit; // position limit
            std::array<Range,4>  pos_margin; // position margin, first: negative direction, second : positive direction
            std::array<double,4> v_max; // max velocity
            std::array<Range,4> a_max; // acc range
            std::array<double,4> f_err; // following error
            std::array<double,4> f_rawerr; // following error
            double precheck2stop;   // distance from precheck sensor to driving stop sensor
            double node_mark_size;       // reflector size for node marking
            double station_mark_size;    // reflector size for station marking
            double station_mark_front_part; // front part of reflector size for station marking
            double station_stop_mark_size; //reflector size for station stop marking
            double premark_start; // premarking starts before target (first met point)
            double node_mark_start; // marking starts before target  (second met point)
            double station_mark_start; // marking starts before target (second met point)
            double node_premark_speed; // speed at premarking
            double station_premark_speed; // speed at premarking
            double premark_acc;
            double node_mark_speed; // speed at marking
            double station_mark_speed; // speed at marking
            double mark_acc;
            double localization_drive_limit; // how long
            OHTLocation initial_location;
        };

        namespace behavior {
            enum ReturnCode : int {
                TIMEOUT_ERROR = -1,
                ERROR = -1,   // any negative state code will stop processing steps
                NO_ERROR = 0,    // next step will be called immediate after current step
                WAITING = 1,    // waiting timed job result
                PREVIOUS_STEP,      // back to previous step
            };
        }

        typedef std::function<int(void)> StepHandler;
        struct TaskStepHandlerSet {
            struct TaskStep {
                StepHandler handler;
                std::string name;
                int no;
                TaskStep() = default;
                TaskStep(const StepHandler& _handler, const char* _name, int _no)
                        : handler{_handler}, name{_name}, no{_no} { }
            };
            struct StepState {
                int step_no; // step number
                int target_step_no; // target step number for execution step by step
                bool error;
            };

            std::deque<TaskStep> handlers;
            StepState state;
            string job_name;
        public:
            TaskStepHandlerSet() { resetState(); }
            int call() {return (state.step_no >=0 && state.step_no < handlers.size()) ? handlers[state.step_no].handler() : 0; }
            int operator()() { return call(); }
            bool isTargetStep() { return  (state.target_step_no == state.step_no); }
            bool isTargetStepDone() { return  (state.target_step_no >= 0 &&  state.target_step_no < state.step_no); }
            bool isSequenceDone() { return state.step_no >= handlers.size(); }
            int getCurrStepNo() { return state.step_no; }
            int getTargetStepNo() { return state.step_no; }
            int increaseTargetStepNo() { return setTargetStepNo(state.step_no + 1); }
            int setTargetStepNo(int target_step) {
                state.target_step_no = (target_step < handlers.size() ) ? target_step : handlers.size()-1;
                return state.target_step_no;
            }
            const char* currStepName() { return handlers[state.step_no].name.c_str(); }
            bool isError() { return state.error; }
            void resetState() { state = {0, -1, false}; }
            void goNextStep() { if (state.step_no<handlers.size()) state.step_no += 1; }
            void goPreviousStep() { if (state.step_no>0) state.step_no -= 1; }
            int getStepNoByName(const char* name) {
                for (auto& step: handlers) if (step.name == name) return step.no;
                return -1;
            }
        };

        struct TaskTimer {
            TaskTimer() : valid_(false) {}
            bool isValid() { return valid_; }
            bool isTimeout() {return (std::chrono::system_clock::now() >= ts_end_);}
            void reset() { valid_ = false; }
            void setTs(int timeout_ms) {
                ts_begin_ = std::chrono::system_clock::now();
                ts_end_  = ts_begin_ + std::chrono::milliseconds(timeout_ms);
                valid_ = true;
            }
        private:
            time_point_ns ts_begin_, ts_end_;
            bool valid_;
        };
        // x-fer task struct
        struct TransferTask {
            TaskStepHandlerSet *job;
            int task;
            bool is_loading;
            int state;
            OHTMap::StationData* sta;
            // speed levels
            double slide_speed_level; // speed level : [0,1]
            double hoist_speed_level;
            double hoist_target_mm;
        public:
            void initialize(TaskStepHandlerSet* job, OHTMap::StationData *sta = nullptr, bool load = false);
            void reset();
            void setSpeedLevel(int axis, double speed_level); // axis : OHT_HOIST or OHT_SLIDE
            bool useEqPio() const { return sta!=nullptr;}
            bool isRunning() const { return job!=nullptr;}
        };
        
        class ActiveObject{
            enum {UPDATE_CYCLE_US = 1000};
        public:
            ActiveObject();
            virtual ~ActiveObject(){ finalize(); }
            virtual bool initialize(){ initialized_ = true; return true; }
            virtual void update() = 0; // time update
            virtual void finalize() {}
            // run
            bool runAsThread(int cycle_us = 1000);
            bool isPaused();
            bool pauseThread();
            bool resumeThread();
            void stopThread();
            bool isThreaded() const {return th_ != nullptr;}

        protected:
            double update_cycletime_;
            bool thread_stop_;
            bool thread_paused_;
            bool initialized_;
            std::unique_ptr<std::thread> th_;
            std::mutex lock_;
        };

        class OHTController;
        class Behavior : public ActiveObject {
        public:
            Behavior() = delete;
            Behavior(OHTController *controller);
            void setName(const char* name) { name_ = name;}

        protected:
            string name_; // behavior name
            OHTController* owner_;
            OHTMap& map_;
            OhtOnboardIO& onboard_io_;
            EcOHTFrontIo& front_io_;
            EcOHTRearIo& rear_io_;
            OhtConfig& config_;
            TaskTimer timer_;

            bool readRawDi(unsigned portid); // port_id = board<<24 + port <<16 + bit
            bool readRawDo(unsigned portid); // port_id = board<<24 + port <<16 + bit
            bool writeRawDo(unsigned portid, bool value); // port_id = board<<24 + port <<16 + bit
            bool readDi(unsigned portid); // port_id = board<<24 + port <<16 + bit
            bool readDo(unsigned portid); // port_id = board<<24 + port <<16 + bit
            bool writeDo(unsigned portid, bool value);
            bool checkRawDi(unsigned portid, bool expected_value);
            bool checkDi(unsigned portid, bool expected_value); // positive logic
            bool checkDiAny(std::initializer_list<std::pair<unsigned,bool>>& sensors);
            bool checkDiAll(std::initializer_list<std::pair<unsigned,bool>>& sensors);
            int  pollSensors(int timeout_ms,std::initializer_list<std::pair<unsigned,bool>> sensors, bool all = true);
            double getPosmm(int axis);
            double getPos(int axis);
        };

        class Perception : public Behavior {
            using parent = Behavior;
        public:
            Perception(OHTController *controller): parent (controller){ name_ = "Perception";}
        };

        class Actuation : public Behavior {
            using parent = Behavior;
        public:
            Actuation(OHTController *controller): parent (controller){ name_ = "Actuation";}
        };
    }
}
#endif //SEPMASTER_OHT_BEHAVIOR_BASE_H
