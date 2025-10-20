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

#include "velocity_profile.h"

#define VEL_TRT(...) //SEP_LOGT(__VA_ARGS__)
#define VEL_TRW(...) SEP_LOGW(__VA_ARGS__)
#define VEL_TRI(...) //SEP_LOGI(__VA_ARGS__)

namespace sephi
{
    TrVelocityProfile::TrVelocityProfile(double _maxvel, double _maxacc, double _maxdec)
    {
        validated = false;
        if (_maxdec < 0) {
            SetMax(_maxvel, _maxacc);
        } else {
            SetMax(_maxvel, _maxacc, _maxdec);
        }
    }

    bool TrVelocityProfile::ComputeTimeOptimalSolutionRetry()
    {
        if (!ComputeTimeOptimalSolution()){
            isAFP = !isAFP;
            InvertParameters();
            return ComputeTimeOptimalSolution();
        }
        return true;
    }

    void TrVelocityProfile::SetProfile(double pos1, double pos2, double vel1, double vel2)
    {
        Initlialize(pos1, vel1, pos2, vel2);
        CheckIfPlanningFails(ComputeTimeOptimalSolutionRetry());
    }

    void TrVelocityProfile::SetProfileDuration(double pos1, double pos2, double newduration, double vel1, double vel2)
    {
        Initlialize(pos1, vel1, pos2, vel2);
        CheckIfPlanningFails(ComputeTimeFixedSolution(newduration), newduration);
    }

    void TrVelocityProfile::SetProfileVelocity(double pos1, double pos2, double newvelocity, double vel1, double vel2)
    {
        maxV = scaledV = newvelocity;
        Initlialize(pos1, vel1, pos2, vel2);
        CheckIfPlanningFails(ComputeTimeOptimalSolution());
    }

    void TrVelocityProfile::SetMax(double _maxvel, double _maxacc)
    {
        scaledA = maxA = fabs(_maxacc);
        scaledD = maxD = fabs(_maxacc);
        scaledV = maxV = fabs(_maxvel);
    }

    void TrVelocityProfile::SetMax(double _maxvel, double _maxdec, double _maxacc)
    {
        scaledA = maxA = fabs(_maxacc);
        scaledD = maxD = fabs(_maxdec);
        scaledV = maxV = fabs(_maxvel);
    }

    double TrVelocityProfile::Duration() const
    {
        return duration;
    }

    double TrVelocityProfile::Pos(double tSec) const
    {
        //if (tSec < 0) {
        //    tSec = tSec;
        //}
        SEP_REQUIRE_RET(tSec >= 0, -1);

        using MotionState = Motion::MotionState;

        double pos;
        // 종료 됐을 때
        if (tSec <= t1) { // 가속구간
            pos = p0 + v0*tSec + 0.5*scaledA*tSec*tSec;
            vel_state_ = Motion::IN_ACC;
        } else if (tSec <= t2){ // 등속구간
            double v1 = v0 + scaledA*t1;
            double p1 = p0 + v0*t1 + 0.5*scaledA*t1*t1;
            double tauUs = tSec - t1;
            pos = p1 + v1*tauUs;
            vel_state_ = Motion::IN_CONST_V;
        } else if (tSec < t3) { // 감속구간
            double v1 = v0 + scaledA*t1;
            double p1 = p0 + v0*t1 + 0.5*scaledA*t1*t1;
            double p2 = p1 + v1*(t2-t1);
            double tauUs = tSec - t2;
            pos = p2 + v1*tauUs - 0.5*scaledD*tauUs*tauUs;
            vel_state_ = Motion::IN_DEC;
        } else {
            pos = pf + (tSec - duration)*vf; // 종료 후엔 등속으로 이동한다고 가정
            vel_state_ = Motion::IDLE;
        }

        //if (!isAFP) {
        //    pos = -pos;
       // }
        return pos;
    }

    double TrVelocityProfile::Vel(double tSec) const
    {
        SEP_REQUIRE_RET(tSec >= 0, -1);
        double vel;
        // 종료 됐을 때
        if (tSec <= t1) { // 가속구간
            vel = v0 + scaledA*tSec;
            vel_state_ = Motion::IN_ACC;
        } else if (tSec <= t2){ // 등속구간
            double v1 = v0 + scaledA*t1;
            vel = v1;
            vel_state_ = Motion::IN_CONST_V;
        } else if (tSec < t3) { // 감속구간
            double v1 = v0 + scaledA*t1;
            double tauUs = tSec - t2;

            vel = v1 - scaledD*tauUs;
            vel_state_ = Motion::IN_DEC;
        } else {
            vel = vf;
            vel_state_ = Motion::IDLE;
        }

        //if (!isAFP) {
        //    vel = -vel;
       // }
        return vel;
    }

    double TrVelocityProfile::Acc(double tSec) const
    {
        SEP_REQUIRE_RET(tSec >= 0, -1);
        double acc;
        // 종료 됐을 때
        if (tSec <= t1) { // 가속구간
            acc = scaledA;
            vel_state_ = Motion::IN_ACC;
        } else if (tSec <= t2){ // 등속구간
            acc = 0;
            vel_state_ = Motion::IN_CONST_V;
        } else if (tSec < t3) { // 감속구간
            acc = -scaledD;
            vel_state_ = Motion::IN_DEC;
        } else {
            acc = 0;
            vel_state_ = Motion::IDLE;
        }

        if (!isAFP) {
            acc = -acc;
        }
        return acc;
    }

    TrVelocityProfile::~TrVelocityProfile()
    {

    }

    int TrVelocityProfile::Initlialize(double _p0, double _v0, double _pf, double _vf)
    {
        p0 = _p0;
        v0 = _v0;
        pf = _pf;
        vf = _vf;
        duration = t1 = t2 = t3 = 0;
        validated = false;
        isAFP = true;

        if (pf - p0 < 0){
			//SEP_LOGT("invert param");
			scaledA = maxA = -1.0 * fabs(maxA);
			scaledD = maxD = -1.0 * fabs(maxD);
			scaledV = maxV = -1.0 * fabs(maxV);
		}
		else
		{
			scaledA = maxA = fabs(maxA);
			scaledD = maxD = fabs(maxD);
			scaledV = maxV = fabs(maxV);
		}
        return 0;
    }

    void TrVelocityProfile::CheckIfPlanningFails(bool success, double fixedT)
    {
        // when planning fails, make the profile not work
        //SEP_LOGT("[%s] timings: t1=%f, t2=%f, t3=%f dur=%f, success=%d\n",  name_.c_str(), t1, t2, t3, duration, success);
        // when generating time-fixed profile, it may fail if fixedT is near optimal duration
        if (!success && fabs(fixedT-duration) > 1e-2 ) {
            duration = t1 = t2 = t3 = 0;
        }
    }

    /// 위치 제어- 가감속 우선 결정
    /**
        가속/감속 수행순서를 결정한다.
        \return isAFP		 :   가속먼저수행(true)
    */
    bool TrVelocityProfile::CheckAFP(double _duration)
    {
        if (!isAFP) {
            VEL_TRT("Inverting parameter 0\n");
            InvertParameters();
        }

        if (_duration < 0) {
            double cAcc = (vf >= v0) ? maxA : -maxD; // 속도 변화을 위한 가속도
            double cDist = 0.5*(vf*vf - v0*v0)/cAcc; // 속도 변화에 필요한 이동거리

            // 현재 속도를 유지했을 때, 다음 sampling time에서 속도 변화에 필요한 거리보다 남은 거리가 더 길다면 가속도 우선
            isAFP = (pf-p0 >= cDist) ? true : false;
        }
        else { // 시간이 주어진 경우,  중간 속도로 주행했을 때 주행거리보다 짧으면 가속구간이 먼저 와야한다.
            isAFP = (0.5*(v0+vf)*_duration <= pf-p0) ? true : false;
        }
        VEL_TRT("[%s] isAFP = %d(v0, vf, p0, pf: %f %f %f %f), dur: %f\n", name_.c_str(), isAFP, v0, vf, p0, pf, _duration);

        if (!isAFP){
            VEL_TRT("Inverting parameter 1\n");
            InvertParameters();
        }

        return isAFP;
    }

    void TrVelocityProfile::InvertParameters()
    {
//        double tmp = maxA;
//        maxA = maxD;
//        maxD = tmp;
        p0 = -p0;
        v0 = -v0;
        pf = -pf;
        vf = -vf;
    }

    /// 위치 제어- time-optimal solution
    /**
       time-optimal 조건하에서 파라미터 변경 구간 시간을 구한다.(t1,t2,t3)
       초기 조건에 따라서 등속구간이 존재하지 않는 톱니 모양의 속도프로파일이 생성될 수도 있다.
       구간별 위치, 속도, 가속도식과 경계치 조건을 이용하여 t1,t2,t3에 관한 방정식의 해를 구한다.

    \par 문제 정의
    최대 가/감속도 및 최대속도가 \f$A,D,V\f$인 제한조건 하에서, 초기 위치 및 속도가 \f$p_0, v_0\f$이고
    목표 위치 및 속도가 \f$p_f, v_f\f$가 되는 최소시간 위치 프로파일을 생성.

    \par 프로파일 형상
    Time-optimality 특성으로 인해 결과 프로파일은 뱅뱅제어 결과물의 형태를 띠게 된다.
    즉, 가속->등속->감속 or 감속->등속->가속. <br>
    참고) duality : 감속->등속->가속 프로파일은 듀얼 problem인 \f$ D, A, V, -p_0, -v_0, -p_f, -v_f \f$의 제한 조건을 가지는 문제의
     가속->등속->감속 프로파일과 sign 대칭이 된다. 즉, 모든 문제는 가속구간이우선 프로파일로 변환 가능.<br>
    참고) 가속구간이 먼저 시작되어야 하는지 여부 : 단위 sampling시간 후에 남은 거리가 속도변화에 필요한 거리보다 멀다면 가속이 먼저 필요하고,
    짧다면 감속이 먼저되어야 한다.


    \par 미지수 결정
    \li 결정해야할 미지수 : \f$ x, \hat x, \bar x \f$
    \li 조건 1 : 이동 거리 조건 (\f$ v_p \f$는 peak \f$ v \f$, i.e. \f$ v_1 \f$)
    \f[
        L = p_f - p_0 = \frac{v_0 + v_f}{2}x + v_p \hat x + \frac{v_p + v_f}{2} \bar x
    \f]
    \li 조건 2 : peak velocity : \f$ v_p = v_0 + Ax = v_f + D \bar x \f$ <br>
     - 등속 구간이 존재할 경우 : \f$ v_p = V \f$ <br>
         연립하면
         \f[
               x = \frac{V - v_0}{A}
          \f]
          \f[
               \bar x = \frac{V-v_f}{D}
          \f]
          \f[
               \hat x = \frac{2L - (v_0 + V)x - (V+v_f)\bar x}{2V}
         \f]
         만약, \f$ \hat x \geq 0 \f$ 이면 등속구간이 존재하지 않는 경우이므로, 아래 조건으로부터 해를 구해야한다.
     - 등속 구간이 부재할 경우 : \f$ \hat x = 0, \bar x = \frac{1}{D} (v_0 - v_f + Ax) \f$
        연립하면,
        \f[
            (A + \frac{A^2}{D})x^2 + 2v_0( 1 + \frac{A}{D}) x + \frac{v_0^2 - v_f^2}{D} - 2L = 0
        \f]
        단, \f$ x \f$는 두 근 중에서 \f$ x \geq 0 , \bar x \geq 0 \f$ 만족하는 근.


    */
    bool TrVelocityProfile::CalculateX(double *x)
    {
        //v10229 기존에 사용 하던 2차 방정식 계수가 맞지 않아 수정 //CIS
        double len = pf - p0;
        double acc = maxA;  
        double dcc = maxD;
        double v_0 = v0;
        
        double A = acc/2 + acc*acc/(dcc*2);
        double B = v_0 + (v_0*acc)/dcc;
        double C = (v_0*v_0)/dcc/2 - len;
        
        double c[3] = {A, B, C};

        return solveQuadraticEquation(c, x);
    }

    bool TrVelocityProfile::ComputeTimeOptimalSolution()
    {
        double len = pf - p0; // displacement
        double x;   // 0  to t1
        double xh;  // t1 to t2
        double xb;  // t2 to t3
        double xb_stop; //  바로 감속만해서 정지해야 할 경우 시간
        int profile_type = 0;
        //double pre_A = maxA;    //로그 기록용
        //double pre_D = maxD;    //로그 기록용

        //VEL_TRW("%s\n", __FUNCTION__);

        if (validated) {
            return true;
        }
        
        if(len == 0 && v0 ==0)
			return false;

        // 1. 등속 구간이 존재한다고 가정하고 x, xb 구함cf
        x = fabs(maxV - v0)/fabs(maxA);
        xb = fabs(maxV - vf)/fabs(maxD);
        //xb_stop = fabs(v0 - vf)/fabs(maxD);

        double constVelSection = len - (v0+maxV)*x*0.5 - (maxV+vf)*xb*0.5;
        //SEP_LOGT("[%s] constVelSection : %f, len : %f, x :%f, xb : %f, maxV : %f, maxA : %f, maxD : %f, v0 : %f, vf : %f\n", name_.c_str(), constVelSection,len, x, xb, maxV, maxA, maxD, v0, vf);
        if (constVelSection * len > 0) { //len과 부호가 같고 0보다 클 결우// 210219 yjchoe, constVelSection이 0이 되는 경우는 등속이 없는 가속/감속 삼각형 프로파일임, 이로인해 이상 모션 생성
            xh = fabs(constVelSection/maxV);
            profile_type = 1;
        }
        /*else if(fabs(len) <= fabs((v0+vf)*xb_stop*0.5)){
			x = 0;
			xh = 0;
			xb = fabs(len*2/v0);
			maxD = (v0/xb);
			//SEP_LOGW("No ACC    xb:%f  maxD:%f", xb, maxD);
			profile_type = 2;
			//SEP_LOGT("No Acc");		
		}*/
        else { // 2. 등속 구간이 부재할 때
            double r[2];
            r[0] = 0;
            r[1] = 0;

            int root_count = CalculateX(r);
			//SEP_LOGT("[%s] r[0]:%f, r[1]:%f",name_.c_str(), r[0], r[1]);
            if(root_count > 0){
				profile_type = 3;
                if(r[0] > 0)
                    x = r[0];
                else if(r[1] > 0)
                    x = r[1];
                else
                    x = 0;
                  
            }
            else{
                x = 0;
                profile_type = 4;
            }

            if(x == 0){
				if(v0 != 0){
					xb = fabs(len*2/v0);
					maxD = (v0/xb);
					//SEP_LOGW("x==0    xb:%f  maxD:%f", xb, maxD);
				}
				else{
					xb=0;
					//SEP_LOGW("profile fail");
				}
			}
			else{
				xb = fabs(((v0+maxA*x) - vf)/maxD);
				//SEP_LOGW("x: %f   xb Before: %f ",x,  xb);
				//double dist = 0.5*maxA*x*x + 0.5*maxD*xb*xb     
			}
            

            auto temp_x = x;
            auto temp_xb = xb;

            if (x < 0 || xb < 0) {
                x = 0;
                //xb = abs((v0+maxA*x) - vf)/maxD;
                xb = fabs(len*2/v0);
                maxD = v0/xb;
                //xb = ((fabs(v0)+maxA*x) - fabs(vf))/maxD;
                //VEL_TRW("root_count=%d, r=%f,%f, x/xb before = %e/%e after = %e/%e\n", root_count, r[0], r[1], temp_x, temp_xb, x, xb);
                SEP_REQUIRE_RET(x >= 0 && xb >= 0, false);
            }

            if (x < 0) x = 0;
            if (xb < 0) xb = 0;

            xh = 0;
        }
        t1 = x;
        t2 = x+xh;
        t3 = t2 + xb;
        duration = t3;
        validated = true;

        // parameter inversion이 있었을 수 있으므로,
        scaledA = maxA;
        scaledD = maxD;
        scaledV = maxV;
        return true;

    }

    /// 위치 제어- 이산시스템에서 속도 프로파일
    /**
       quantization time delay 조건에서 파라미터 변경 구간 시간을 구한다.(t1,t2,t3)
       가감속도를 scaling하는 경우는 제외하고, 최대 속도를 scaling 하는 경우만 고려한다.(maxV'=beta*maxV, 0<beta<=1)
       beta에 관한 이차방정식을 풀고 t1,t2,t3를 계산한다.

       \param fixedT	:	quantization 을 고려한 전체 모션 시간

    \par 문제 정의
    최대 가/감속도 및 최대속도가 \f$A,D,V\f$인 제한조건 하에서, 초기 위치 및 속도가 \f$p_0, v_0\f$이고
    목표 위치 및 속도가 \f$p_f, v_f\f$이며, 전체 주행시간이 \f$ T \f$인  위치 프로파일을 생성.

    \par 미지수 결정
    주행시간이 주어진 경우도, 시간최적 프로파일과 동일한 형상의 가속도 제한 프로파일로 만들 수 있다.
    가속 구간, 등속구간, 감속구간 각각의 길이를 \f$ x, \hat x, \bar x \f$ 이라고 두면,
    \f$ \hat x  = T - x - \bar x \f$이므로, 다음과 같이 문제를 풀 수 있다.

    \li 결정해야할 미지수 : \f$ x, \bar x \f$
    \li 조건 1 : 이동 거리 조건 (\f$ v_p \f$는 peak \f$ v \f$, i.e. \f$ v_1 \f$)
        \f[
            L = p_f - p_0 = \frac{v_0 + v_f}{2}x + v_p (T - x - \bar x) + \frac{v_p + v_f}{2} \bar x
        \f]
    \li 조건 2 : 최대 속도 조건. scaling factor \f$ \beta \f$를 도입하여, \f$ v_p = \beta V\f$, 단 \f$ 0 \leq \beta \leq 1 \f$로 두면,
        \f[
        \begin{array}
        {l}
           x = \frac{v_p - v_0}{A} \\
           \bar x = \frac{v_0 - v_f + Ax}{D} \\
           v_p = \beta V
        \end{array}\
        \f]
        연립하여 풀면,
        \f[
            -V^2(\frac{1}{A} + \frac{1}{D} ) \beta^2 +  2V \left(T + \frac{v_0}{A} + \frac{v_f}{D} \right) \beta
                -2L - \frac{v_0^2}{A} - \frac{v_f^2}{D} = 0
        \f]
        단, \f$x \geq 0, \bar x \geq 0, (T-x-\bar x) \geq 0 \f$.
    \li 참고) Time-fixed 조건의 특성상(constraint가 하나 증가) 해가 존재하지 않는 경우가 있음.
            이경우 time-optimal 조건 그대로 이용함.
    */
    bool TrVelocityProfile::ComputeTimeFixedSolution(double fixedT)
    {
        // 최도 속도 scaling solution - 항상 등속구간이 존재함
        double len = pf - p0; // displacement
        double x;   // 0 to t1
        double xh;  // t1 to t2
        double xb;  // t2 to t3

        //VEL_TRW("%s, fixT = %f\n", __FUNCTION__, fixedT);

        // 사전 조건. 미리 프로파일 형상을 찾음
        ComputeTimeOptimalSolutionRetry();

        //SEP_LOGT("[%s] time optimal timings: t1=%f, t2=%f, t3=%f dur=%f\n", name_.c_str(), t1, t2, t3,  duration);

        double c[3] = {
                        -maxV*maxV*(1/maxA + 1/maxD),
                        2*maxV*(fixedT + v0/maxA + vf/maxD),
                        -2*len - v0*v0/maxA - vf*vf/maxD
                    };
        double r[2];
        int roots;
        bool found = false;
        //VEL_TRW("c' = %f, %f, %f\n", c[0], c[1], c[2]);
        SEP_REQUIRE_RET( (roots = solveQuadraticEquation(c, r)) > 0, false);
        //VEL_TRW("roots = %d, r=%f,%f\n", roots, r[0], r[1]);
        for(int i = 0; i < roots; i++) {
            double beta = r[i];
            if (beta < 0 || beta > 1) continue;
            x = (beta*maxV-v0)/maxA;
            xb = (beta*maxV-vf)/maxD;
            xh = fixedT - x - xb;
            if (x >= 0 && xb >= 0 && xh >= 0) {
                scaledV = beta * maxV;
                found = true;
                break;
            }
            VEL_TRT("[%s] beta = %f, x = %f, xb = %f, xh = %f\n", name_.c_str(), beta, x, xb, xh);
        }

        if (!found) { // 가/감속 scaling으로 문제를 풀어야 함.
            SEP_LOGW("[%s] Fail to get fixed time solution!!! Using optimal solution time or acc scaled solution\n", name_.c_str());
            return false;
        }

        t1 = x;
        t2 = x+xh;
        t3 = t2 + xb;
        duration = t3;
        validated = true;
        return true;
    }

}
