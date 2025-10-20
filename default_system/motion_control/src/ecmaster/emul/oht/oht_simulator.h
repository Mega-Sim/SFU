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
// Created by root on 17. 11. 9.
//

#ifndef SEPMASTER_OHT_SIMULATOR_H
#define SEPMASTER_OHT_SIMULATOR_H

#include "emul/simulation_model.h"
#include "oht_io_map.h"
#include "oht_station.h"
#include "oht_serial_sensors.h"
#include "oht_obstacle.h"

namespace sephi {
    /// @defgroup grp_ecmaster_emul
    /// @{
    class OHTSystem : public SimulatorBase
    {
        enum OHT_SERIAL_PORT_ID {
            ONBOARD_OBS     = 3,
            ONBOARD_OHT_DET = 4,    // OHT detector
            ONBOARD_QRR1    = 5,
            ONBOARD_QRR2    = 6,
            ONBOARD_BCR1    = 7,
            ONBOARD_BCR2    = 8,

            REARIO_EQ_PIO1  = 0,
            REARIO_EQ_PIO2  = 1,
            REARIO_REMOCON  = 2,
            REARIO_COOL_MUSCLE_ROTATE = 3,

            FRONTIO_HID     = 0,
            FRONTIO_CID     = 2,
            FRONTIO_LD      = 3,   // look down
        };

        struct SizeParam {
            double precheck2stop;   // distance from precheck sensor to driving stop sensor
            double node_mark;       // reflector size for node marking
            double station_mark;    // reflector size for station marking
            double station_mark_front; // front part of reflector size for station marking
            double station_stop_mark; //reflector size for station stop marking
        };
        struct TimeoutJob {
            enum {
                RTP_SHUTTER = 1000,     // shutter on-off time ms
                RTP_GRIP = 1000,        // grip on-off time
                RTP_STEER = 500,        // steering time
            };
            TimeoutJob(int timeout_ms, void (*func)(OHTSystem*, int), int intarg);
            time_point_ns timeout;
            int int_arg;
            void (*func)(OHTSystem*, int);
            bool isTimeOut();
        };
        std::list<TimeoutJob> delayed_jobs_;
        void triggerGrip();
        void triggerShutter();
        void triggerSteer();
        void doDelayedJobs();
    public:
        string systemName() { return "OHT"; }
        bool initialize(ECatEmulator &emul, const string &model_file);
        void update();
        void finalize();
        void printState();
        void changeEmoState(bool newstate) { emo_press_state_ = newstate; }
        void setOhtLocation(uint32_t parent, uint32_t next, double offset);
        string getStatusAsYaml(); // oht information as yaml format for simulator viewer

    private:
        bool loadConfiguration(const string &model_file);
        void initStations();
        void validateCurrentStation();
        void clearPioSignal();
        void updateOhtLocation();
        void updateDrivngSensors();
        void updateEQportSensors();
        void updateSTBportSensors();
        void updateTransSensors();
        void updateAuxSensors();

        // parameters
        SizeParam param_;

        // Axes
        std::array<YaskawaServoModel *, 4> axes_; // left, right, hoist, slide axis each
        // Io
        OHTFrontIOModel *front_io_;
        OHTRearIOModel *rear_io_;
        // io map; viewpoint of simulator
        oht_sim::OhtFrontDI *front_di_;
        oht_sim::OhtFrontDO *front_do_;
        oht_sim::OhtRearDI *rear_di_;
        oht_sim::OhtRearDO *rear_do_;
        oht_sim::OhtOnboardDO *onboard_do_;
        oht_sim::OhtOnboardDI *onboard_di_;


        // serial sensors
        std::unique_ptr<oht_sim::BL_185> driving_bcr_;
        std::unique_ptr<oht_sim::BL_185> trans_bcr_;

        // external event
        std::map<string, bool> ext_event_map_; // external events: estop, ...

        // world model
        oht_sim::OHTMap map_;

        // object list
        oht_sim::OHTInfo oht_;
        oht_sim::OtherOHT other_oht_;
        std::map<uint32_t, oht_sim::OHTStation> station_map_;
        std::list<uint32_t> foup_list_; // foup locations. list of station id's, except robot = 0xFFFFFFFF
        int current_station_; // current PIO-comm target station id
        bool emo_press_state_;

        // aux
        friend class EcEmulatorMonitor;
        friend class CleanUp;
    };

    ///@}
}
#endif //SEPMASTER_OHT_SIMULATOR_H
