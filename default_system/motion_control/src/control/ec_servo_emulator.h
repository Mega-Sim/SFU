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
// Created by sy1135.jeong1696 on 2017-07-27.
//

#ifndef SEPMASTER_EC_SERVO_EMULATOR_H
#define SEPMASTER_EC_SERVO_EMULATOR_H
namespace sephi
{
    class EcServoEmulator {
    public:
        static bool start();
        static bool requestSlave(int axis, Cia402PdoIn*& inpdo, Cia402PdoOut*& outpdo, std::mutex*&mtx);
        static bool setMasterAxis(int axis, int master_axis);
        static bool resetMasterAxis(int axis);
        static bool setServoMode(int axis, ServoOpMode new_mode);
    };
}
#endif //SEPMASTER_EC_SERVO_EMULATOR_H
