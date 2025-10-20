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
// Created by sem1 on 17. 4. 28.
//  Catesian Path Motion

#ifndef SEPMASTER_ROBOT_MOTION_H
#define SEPMASTER_ROBOT_MOTION_H

#include "sephi_ipc.h"
#include "event_host.h"
#include "servo_motion.h"
#include "servo_motor.h"

using namespace KDL;
using std::unique_ptr;
using std::make_unique;

namespace sephi
{

    /** \ingroup grp_motion
     *  \defgroup grp_robot_motion Robot Motion
     *
     * \brief  Robot Motion은 다축 로봇의 동작에 대한 추상 객체
     * \details
     *  end-effector path와 speed profile로부터 servo motor trajectory를 생성 가능하게 한다.
     *
     *  Robot Motion 계층 구조
     *
     *  RobotMotion
     *   - LineTo
     *   - ArcTo
     *   - Circle
     *   -
     */
    class RobotMotion : public Motion {

    };
    /// \}
}

#endif //SEPMASTER_ROBOT_MOTION_H
