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

#ifndef SEPMASTER_MATH_UTILITY_H
#define SEPMASTER_MATH_UTILITY_H

#include <array>
#include <utility>
#include <cmath>

namespace sephi
{
    int solveLinearEquation(const double *C, double * x);
    int solveQuadraticEquation(const double *C, double * x);
    int solveCubicEquation(const double *C, double * x);
    int solve4thOrderEquation(const double *C, double * x);

    template <typename T, size_t N>
    T evaluatePolynomial(const std::array<T, N> &coeff, T x) {
        // c[N-1] + c[N-2]*x + ... + c[0]*x^(N-1)
        T ret = 0, xx = 1;
        for(int i = 0; i < coeff.size(); ++i) { //
            ret += xx * coeff[coeff.size()-1-i];
            xx *= x;
        }
        return ret;
    };

    template <typename T, size_t N>
    std::array<T,N-1> differentiatePolynomial(const std::array<T,N>& coeff) {
        // c[N-1] + c[N-2]*x + ... + c[0]*x^(N-1)
        std::array<T,N-1> ret;
        for(int i = 0; i < coeff.size()-1; ++i) {
            ret[i] = (N-1-i)*coeff[i];
        }
        return ret;
    };

    // displacement이동 거리 내에 종단 속도에 이를 수 있는 지 여부
    inline bool isReachableEndSpeed(double displacement, double acc, double v_end, double v0 = 0) {
        double t = (v_end - v0) / acc;
        return (v0*t + 0.5*acc*t*t < displacement);
    }
}

#endif //SEPMASTER_MATH_UTILITY_H
