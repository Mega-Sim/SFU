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

#ifndef SEPMASTER_STOCKER_IO_MAP_H
#define SEPMASTER_STOCKER_IO_MAP_H
namespace sephi {
    namespace stk_sim {
        // digital input bit field; input is machine's view, that is, controller's output
        struct StockerDI {
            unsigned reserved_P0_0_to_P0_2 :3;        // (0,0~2)
            unsigned light_curtain_sensor_muting_0:1; // (0,3); (turn off light curtain) : active high
            unsigned light_curtain_sensor_muting_1:1; // (0,4); active high
            unsigned reserved_P0_5_to_P1_7:11;        // (0, 5~7), (1,0~7)
            unsigned auto_teaching_sensor_pwr:1;      // (2,0)
            unsigned empty_unload_detector:1;         // (2,1) ; active high
            unsigned double_load_detector:1;          // (2,2) ; active high
            unsigned reserved_P2_3:1;                 // (2,3)
            unsigned safety_reset:1;                  // (2,4) ; active high
            unsigned mc_control:1;                    // (2,5) ; active high
            unsigned safety_door_lock:1;              // (2,6) ; active high
        }__attribute__((packed));

        // digital output bit field; output is machine's view, that is, controller's input
        struct StockerDO {
            unsigned reserved_P0_0:1;                 // (0,0)
            unsigned _mc_status:1;                    // (0,1)  ; active low
            unsigned _estop_status:1;                 // (0,2) ; active low
            unsigned interlock_disable_key_status:1;  // (0,3) ; active high
            unsigned reserved_P0_4_to_P0_5:2;         // (0, 4~5)
            unsigned _fork_load_sensor_r:1;           // (0,6) ; active low
            unsigned _fork_load_sensor_l:1;           // (0,7) ; active low
            unsigned reserved_P1_0:1;                 // (1,0)
            unsigned cooling_fan_status:1;            // (1,1)
            unsigned _safety_door_lock_status:1;      // (1,2) ; active low
            unsigned _safety_door_close_status:1;     // (1,3) ; active low
            unsigned reserved_P1_4_to_P2_0:5;         // (1,4~7), (2,0)
            unsigned _empty_unload_status:1;          // (2,1) ; active low
            unsigned double_load_status:1;            // (2,2) ; active high
            unsigned reserved_P2_3:1;                 // (2,3)
            unsigned _safety_plc_status:1;            // (2,4) ; active low
            unsigned _light_curtain_status:1;         // (2,5)   ; active low
            unsigned safety_plc_alarm:1;              // (2,6) ; active high
        }__attribute__((packed));
    }
}
#endif //SEPMASTER_STOCKER_IO_MAP_H
