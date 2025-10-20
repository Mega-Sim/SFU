//
// Created by sem1 on 17. 4. 28.
//

#ifndef SEPMASTER_CONTROL_COMMON_H_H
#define SEPMASTER_CONTROL_COMMON_H_H

#include "servo_motor.h"
namespace sephi
{
    using time_point_ns = std::chrono::time_point<std::chrono::system_clock,std::chrono::nanoseconds>;

    /**\ingroup grp_robotic_module
     * \defgroup grp_motion Motion
     * \brief 로봇 및 서보 모션 그룹
     *  \{
     */
    /// 모션 클래스들 조상 클래의
    class Motion {
    public:
        using ControlMode = ServoOpMode;
        enum MotionType : int {
            AMOVE           = 1,    //angular move
            ROTATE,
            TORQUE,
            JOINT_MOTION    = 0x00FF,
            LINE            = 0x0100,
            ARC,
            FORCE,
            CARTESIAN_MOTION= 0xFF00,
        };
        enum MotionEvent : int {
            BEGIN,
            ACC_END,
            DEC_BEGIN,
            END,
        };
        enum MotionState : uint32_t { // least significant nibble means moving or not
            IDLE = 0x000,
            GOING = 0x101,
            IN_ACC = 0x111,
            IN_CONST_V = 0x121,
            IN_DEC = 0x131,
            PAUSED = 0x200,
            IN_PAUSING = 0x211,
            STOPPED = 0x300,
            IN_STOPPING = 0x311,
        };
    public:
        static const char* motionStateString(uint32_t ms) {
            switch (ms) {
                case IDLE:        return "IDLE";
                case GOING:       return "GOING";
                case IN_ACC:      return "IN_ACC";
                case IN_CONST_V:  return "IN_CONST_V";
                case IN_DEC:      return "IN_DEC";
                case PAUSED:      return "PAUSED";
                case IN_PAUSING:  return "IN_PAUSING";
                case STOPPED:     return "STOPPED";
                case IN_STOPPING: return "IN_STOPPING";
                default: break;
            }
            return "Undefined";
        }
    protected:
        int id_; // 모션 구분자
        MotionType type_; // motion type
        ControlMode cmode_; // control mode
        MotionState state_; // motion 상태
    };
    /// \}
}

#endif //SEPMASTER_CONTROL_COMMON_H_H
