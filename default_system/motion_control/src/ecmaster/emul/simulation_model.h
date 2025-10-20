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

#ifndef SEPMASTER_SIMULATION_MODEL_H
#define SEPMASTER_SIMULATION_MODEL_H
#include "yaskawa_servo_model.h"
#include "stocker_io_model.h"
#include "oht_io_model.h"

namespace sephi {
    class ECatEmulator;
    /// @addtogroup grp_ecmaster_emul
    /// @{

    /// @brief Target system model의 기반 class
    /// @details 타겟 시스템 configuration은 initialize 함수에서 처리할 것.
    ///
    class SimulatorBase {
    public:
        virtual string systemName(){ return ""; }
        virtual bool initialize(ECatEmulator &emul, const string &model_file) { return true; }
        virtual void update() {}
        virtual void finalize() {}
        virtual void printState(){}
    };

    /// @}

    // inline utils
    inline static bool inRange(double p, double center, double margin)
    {
        return (fabs(center-p)<margin);
    }

}
#endif //SEPMASTER_SIMULATION_MODEL_H
