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
// Created by master on 17. 3. 29.
// ethercat master 설정

#ifndef SEPMASTER_EC_CONFIG_H
#define SEPMASTER_EC_CONFIG_H
#include <memory>
#include <thread>
#include "sephi_ipc.h"

namespace sephi {


    enum ECatContants {
        EM_MSG_Q_SIZE           = 512,
        EM_SLAVE_MSG_Q_SIZE     = 16,
        EM_MSG_Q_DATA_SIZE      = 16,
        EM_SHM_SECTION_SIZE     = 4096*10,
        EM_SHM_SIZE             = EM_SHM_SECTION_SIZE * 3, // in/out/share
        EM_RPC_PORT             = 50051,
        RF2217_ECAT_SLAVE_PORT  = 43000,
        ECAT_SERIAL_SLAVE_ENABLE = 1, // 1: enable, 0: disable
        ECAT_STATE_CHANGE_TIMEOUT_MS = 15000, // timeout in ms
        ECAT_SCANBUS_TIMEOUT_MS = 5000, // timeout in ms
        COE_SDO_TIMEOUT_MS      = 3000, // timeout in ms


        MAX_LINKLAYER           = 5,

        DCM_ENABLED             = 1, // change this value to 1 when enable
        DCM_CTRL_SET_VAL        = 500000, //DCMCtlSetVal: DCM controller set value in nanosec
        DCM_SYNC0_SHIFT_NS      = 125000, // DC SYNC shift of first slave

        // timer setting
        CYCLE_TIME              = 1,           /* 1 msec */
        BASE_PERIOD             = 1000,        /* 1000 usec */

        // static EtherCAT master configuration values
        MASTER_CFG_ECAT_CMD_MAX_RETRIES         = 5,    /* maximum retries to send pending ethercat command frames */
        MASTER_CFG_EOE_TIMEOUT                  = 1000,    /* timeout sending EoE frames */
        MASTER_CFG_FOE_BUSY_TIMEOUT             = 250,    /* timeout FoE busy */

        MASTER_CFG_MAX_QUEUED_ETH_FRAMES        = 32,    /* max number of queued frames, 127 = the absolute maximum number */
        MASTER_CFG_MAX_SENT_QUFRM_PER_CYC       = 3,    /* max number of queued frames sent with eUsrJob_SendAcycFrames within one cycle */
        MASTER_CFG_MAX_SLAVECMD_PER_FRAME       = 32,    /* max number of ecat telegrams per frame (0=maximum possible) */
        MASTER_CFG_ECAT_MAX_BUS_SLAVES          = 256,    /* max number of pre-allocated bus slave objects */

#ifndef ETHERCAT_DC_TIMEOUT
        // DC time constants
        ETHERCAT_DC_TIMEOUT             = 12000,   /* DC initialization timeout in ms */
        ETHERCAT_DC_ARMW_BURSTCYCLES    = 10000,   /* DC burst cycles (static drift compensation) */
        ETHERCAT_DC_ARMW_BURSTSPP       = 12,      /* DC burst bulk (static drift compensation) */
        ETHERCAT_DC_DEV_LIMIT           = 13,      /* DC deviation limit (highest bit tolerate by the broadcast read) */
        ETHERCAT_DC_SETTLE_TIME         = 1500,    /* DC settle time in ms */
#endif // ETHERCAT_DC_TIMEOUT
    };

#define EC_SPEED_EPSILON (1e-9) // deg/s

    enum SlaveType {
        CIA402_COMPLIANT_SERVO,
        CIA402_COMPLIANT_SERVO2,
        CIA402_COMPLIANT_SERVO_D,
        CIA402_COMPLIANT_SERVO_T,
        FINE_OHT_FRONT_IO,
        FINE_OHT_REAR_IO,
        FINE_STK_IO,
        FINE_SERIAL,
        UNKNOWN,
    };

    enum class EcMasterError {
        NO_ERROR,
        SLAVE_MISMATCH,
    };


    inline const char* EM_PDO_LOCK_KEY(){ return "sephi_ecat_pdo_lock";}
    inline const char* EM_MSG_Q_KEY() { return "sephi_ecat_master";}
    inline const char* EM_SHM_KEY() { return  "sephi_ecat_master_shm";}
    inline const char* EM_CYCLIC_EVENT_KEY() { return "sephi_ecat_cyclic_event";}
    inline  string EM_MSG_Q_KEY_SLAVE(int slave_id) {
        char key[32];
        snprintf(key, sizeof(key), "sephi_ecat_salve_%d", slave_id);
        return string(key);
    }
    inline const char* VEL_DISPLAY_HOST() { return "localhost"; }
    inline const char* VEL_DISPLAY_PORT() { return "11112"; }
    inline const char* ACONTIS_LIC_DB_FILE() { return "./license_db.yml"; }
    inline const char* OHT_ONBOARD_IO_SHM_KEY(){ return "sephi_oht_onbard_io";}

}
#endif //SEPMASTER_EC_CONFIG_H
