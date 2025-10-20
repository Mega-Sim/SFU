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
// Created by sem1 on 17. 5. 3.
//

#ifndef SEPMASTER_VELOCITY_PROFILE_H
#define SEPMASTER_VELOCITY_PROFILE_H

#include "math_utility.h"
#include "control_common.h"
#include <velocityprofile_trap.hpp>
#include <velocityprofile_traphalf.hpp>
#include <velocityprofile_spline.hpp>
#include <velocityprofile_rect.hpp>
#include <string>
namespace sephi
{
    /**\ingroup grp_motion
     * \defgroup grp_vel_profile Velocity Profile
     * \brief 속도 프로파일
     * \details
     * 로봇 모션 및 서보 모션의 속도 프로파일을 생성한다.
     * \{
     */
    /// trapezoidal velocity profile with boundary speed
    class TrVelocityProfile {
    public:
        TrVelocityProfile(double _maxvel = 0, double _maxacc = 0, double _maxdec = 0);
        ~TrVelocityProfile();
        void SetName(const char* name) { name_ = name; }
        void SetProfile(double pos1, double pos2, double v1 = 0, double v2 = 0);
        void SetProfileDuration(double pos1, double pos2, double newduration, double v1 = 0, double v2 = 0);
        void SetProfileVelocity(double pos1, double pos2, double newvelocity, double v1 = 0, double v2 = 0);
        void SetMax(double _maxvel, double _maxacc);
        void SetMax(double _maxvel, double _maxdec, double _maxacc);
        double GetMaxVel() const { return maxV; }
        double GetMaxAcc() const { return maxA; }
        double GetMaxDec() const { return maxD; }

        double Duration() const;
        bool isValid() const { return validated; }
        double Pos(double time) const;
        double Vel(double time) const;
        double Acc(double time) const;

    private:
        int Initlialize(double p0_, double v0_, double pf_, double vf_);
        bool CheckAFP(double totalT = -1.0);
        void InvertParameters();
        bool ComputeTimeOptimalSolution();                       ///< 최소시간 position plan을 위한 계수 계산
        bool ComputeTimeOptimalSolutionRetry();                  ///< time optimal 계산 실패 시
        bool ComputeTimeFixedSolution(double fixedduration);     ///< time optimal solution 계산 후에 호출
        void CheckIfPlanningFails(bool success, double fixedT = -1);
        bool CalculateX(double *x);
        double maxA;        ///< 최대 가속도
        double maxD;        ///< 최대 감속도
        double maxV;        ///< 최대 속도

        double scaledA;     ///< 프로파일 계산에 사용되는 실제 가속도
        double scaledD;     ///< 프로파일 계산에 사용되는 실제 감속도
        double scaledV;     ///< 프로파일 계산에 사용되는 실제 최대속도
        double duration;        ///< duration in sec.
        bool validated;         ///< Compute()함수를 호출해도 되는 지 여부

        double p0, v0, pf, vf;
        double t1, t2, t3;
        bool isAFP; ///< if acceleration-first-profile
        std::string name_;
        mutable Motion::MotionState vel_state_;
        friend class PPMotion;
    };
    /// \}
}

#endif //SEPMASTER_VELOCITY_PROFILE_H
