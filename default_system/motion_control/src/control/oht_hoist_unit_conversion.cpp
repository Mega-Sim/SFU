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
// Created by sem2 on 17. 12. 28.
//

#include <Eigen/Dense>
#include "oht_hoist_unit_conversion.h"
using namespace sephi;
using namespace Eigen;

static double pow(double x, int p)
{
    double ret = 1;
    for(int i = 0; i < p; ++i)  ret *= x;
    return ret;
}

bool HoistUnitConverter::initialize(std::array<double,7>&pulselist, std::array<double,7>&mmlist)
{
    auto calcCoef = [](const double* pulse, const double* mm, std::array<double,4>& coef) {
        Matrix4d pulsemat;
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j<4; ++j) pulsemat(i,j) = pow(pulse[i]*1e-5,3-j);
        }
        Vector4d mmvec{mm[0],mm[1],mm[2],mm[3]};
        auto coefvec = pulsemat.inverse() * mmvec;
        // \todo check inverse matrix is nan
        for(int i = 0; i < 4; ++i) coef[i] = coefvec(i);
        std::cout << "Coefficient vector: " << coefvec << std::endl;
        return true;
    };

    center_value_ = {pulselist[3], mmlist[3]};
    SEP_REQUIRE_RET(calcCoef(&pulselist[0], &mmlist[0], coef_[0]), false);
    SEP_REQUIRE_RET(calcCoef(&pulselist[3], &mmlist[3], coef_[1]), false);
}
