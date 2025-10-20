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

#ifndef SEPMASTER_STOCKER_SIMULATOR_H
#define SEPMASTER_STOCKER_SIMULATOR_H

#include <array>
#include <map>
#include "emul/simulation_model.h"
#include "stocker_io_map.h"
#include "stocker_shelf.h"
#include "stocker_robot.h"
namespace sephi {
    /// @addtogroup grp_ecmaster_emul
    /// @{

    /** @brief Stocker 기구 동작을 simulation 한다.
        @details Stocker에 외부 동작(eg. pressEStop())이 수행 되거나, 서보 모션으로 로봇의 위치가 업데이트 됐을 때,
        혹은 제어기로부터 output이 전달 됐을 때의 Stocker 상태를 모사한다.
    */
    class StockerSystem : public SimulatorBase {

    public:
        string systemName() { return "Stocker"; }

        bool initialize(ECatEmulator &emul, const string &model_file);

        void update();

        void finalize() {}

        void printState();

        // external event
        void pressEStop();

        void turnInterlockKey(bool disable = true);

        void closeSafetyDoor(bool close = true);

        int getEncoderPulseValue(int axis);
        void setEncoderPulseValue(int axis, int new_pulse);

        // status
        int crashed() const { return crash_status_; }

        uint64_t doutStatus() const { return stk_io_->DI_val(); }

        const stk_sim::Robot &robot() const { return robot_; }

        string getFoupLocations();

    private:
        bool loadConfiguration(const string &model_file);

        stk_sim::Shelf *getCurrentShelf();

        void modelUpdate();

        void outputUpdate();

        // Axes
        std::array<YaskawaServoModel *, 4> axes_; // z, x, r, fork axis each
        // Io
        StockerIOModel *stk_io_;
        // io map; viewpoint of simulator
        stk_sim::StockerDI *di_;
        stk_sim::StockerDO *do_;
        // external event
        std::map<string, bool> ext_event_map_; // external events: estop, interlock_key, safety_door_close

        // geometry
        Vector3 frame_size_; // x,y,z
        Vector3 shelf_size_; // x,y,z
        Vector2 shelf_blank_; // x,y; recessed area of shelf
        Vector2 port_blank_; // x,y
        std::map<string, stk_sim::Shelf> shelf_map_;
        std::list<string> foups_;
        stk_sim::Robot robot_;
        double shelf_plate_thickness_;
        double distance_to_shelf_;
        double x_inposition_margin_;

        // status
        int crash_status_;
        stk_sim::Shelf *curr_shelf_;

        friend class EcEmulatorMonitor;
        friend class stk_sim::Robot;
        friend class stk_sim::Shelf;
    };
    /// @}
}

#endif //SEPMASTER_STOCKER_SIMULATOR_H
