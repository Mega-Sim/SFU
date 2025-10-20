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

#include "math_utility.h"

namespace sephi
{


#define __POW(x, y) ( (x<0) ? (-pow(-(x),(y))) : pow((x),(y)) )
    static inline int __CompareDouble( const void *a, const void *b)
    {
        double &x = *(double*)a, &y = *(double*)b;
        return ((x==y)? 0 : ((x>y)? 1 : -1));
    }

/**
   C[0]x^4 +C[1]x^3... +C[4]= 0의 해를 구한다.

   \param C 4차 방정식의 계수 (높은 차수부터)
   \param x 해가 담길 array
   \return 실근의 개수

   \n\n
   \par Algorithm 설명
   \f$x^4 + ax^3 + bx^2 + cx+d = 0\f$의 해는, 먼저 다음 3차 방정식의 실근 중의 하나를 구함으로써 얻을 수 있다.
   \f[    y^3 + by^2 + (ac-4d)y + (-a^2d + 4bd - c^2) = 0   \f]
   \f$R = \sqrt{ a^2/4 - b + y}\f$라고 가정하자. 만약 \f$R \neq 0\f$이라면,
   \f[   D = \sqrt{ \frac{3a^2}{4} - R^2 - 2b + \frac{4ab - 8c - a^3}{4R}} ,
      E = \sqrt{\frac{3a^2}{4} - R^2 - 2b - \frac{4ab - 8c - a^3}{4R}}   \f]
   라고 정의하고, \f$R = 0\f$이라면,
   \f[   D = \sqrt{ \frac{3a^2}{4}- 2b + 2 \sqrt{y^2-4d}}   ,
      E = \sqrt{\frac{3a^2}{4} - 2b - 2 \sqrt{y^2-4d}}   \f]
   라고 정의한다. \n
   그러면 4차 방정식의 해는 다음과 같다.
   \f[ x = - \frac{a}{4} + \frac{R}{2} \pm \frac{D}{2},  - \frac{a}{4} - \frac{R}{2} \pm \frac{E}{2} \f]

*/
    int solve4thOrderEquation(const double *C, double * x)
    {
#define SWAP_Y(a,b) y=a; b=a; b=y
        if (C[0] == 0) return solveCubicEquation(&C[1],x);
        double a = C[1]/C[0], b = C[2]/C[0], c = C[3]/C[0], d = C[4]/C[0];
        double coef[5] = { 1, -b, a*c-4*d, -a*a*d+4*b*d-c*c };
        double r[3], y;
        double R, R2, D, E;
        int ret;

        // find y
        ret = solveCubicEquation(coef, r);
        y = r[ret-1];

        // Calc R
        R2 = .25*a*a-b+y;
        if (R2<0) return 0;
        if (R2){
            R = sqrt(R2);
            D = .25*3*a*a - R2 -2*b + .25*(4*a*b-8*c-a*a*a)/R;
            E = .25*3*a*a - R2 -2*b - .25*(4*a*b-8*c-a*a*a)/R;
        }
        else{
            R = y*y-4*d;
            if (R < 0) return 0;
            R = sqrt(R);
            D = .25*3*a*a - 2*( b + R);
            E = .25*3*a*a - 2*( b - R);
        }
        // root 결정
#define qrootd(i, sgn) x[i] = (-.25*a + .5*(R sgn D))
#define qroote(i, sgn) x[i] = (-.25*a - .5*(R sgn E))
        if (D<0 && E<0) return 0;
        if (D<0){
            E = sqrt(E);
            if (!E){ qroote(0, +); return 1; }
            else {
                qroote(0,-); qroote(1, +);
                if (x[0]>x[1]){ SWAP_Y(x[0],x[1]); }
                return 2;
            }
        }
        else if (E<0){
            D = sqrt(D);
            if (!D){ qrootd(0, +); return 1; }
            else {
                qrootd(0,-); qrootd(1, +);
                if (x[0]>x[1]){ SWAP_Y(x[0],x[1]); }
                return 2;
            }
        }
        else {
            E = sqrt(E); D = sqrt(D);
            ret = 0;
            if (!E){ qroote(0, +); ret++; }
            else { qroote(0,-); qroote(1, +); ret += 2;}
            if (!D){ qrootd(ret, +); ret++; }
            else { qrootd(ret,-); qrootd(ret+1, +); ret+=2;}
            // root sorting
            qsort(x, ret, sizeof(double), __CompareDouble);
        }
        return ret;
#undef qrootd
#undef qroote
#undef SWAP_Y
    }


/**
   C[0]x^3 +C[1]x^2... +C[3]= 0의 해를 구한다.

   \param C 3차 방정식의 계수 (높은 차수부터)
   \param x 해가 담길 array
   \return 실근의 개수

   \n\n
   \par Algorithm 설명
   3차 방정식 \f$y^3+py^2+qy+r = 0\f$의 해는 먼저
   \f$y = x - p/3, a = (3q-p^2)/3, b = (2p^3-9pq+27r)/27\f$로 치환하여
   제곱항을 없애고 \f$x^3 + ax + b = 0\f$을 얻는데서 출발한다.
   실계수 \f$p, q\f$, 및 \f$r\f$에 대하여 \f$Q = b^2/4 + a^3/27\f$로 정의하자.
   <ul>
   <li>\f$Q>0\f$이면, 하나의 해만 존재하며, 그 해는 다음고 같다.
   \f[ x_1 = \left( -\frac{b}{2} + \sqrt{Q} \right)^{1/3} + \left( -\frac{b}{2} - \sqrt{Q} \right)^{1/3}\f]
   <li>\f$Q = 0\f$이면 세가지 실근을 가지며, 그 중 둘은 중복된다.
   \f[ x_1 = x_2 = \left( \frac{b}{2} \right)^{1/3}, x_3 = -2 \left( \frac{b}{2} \right)^{1/3} \f]
   <li>\f$Q<0\f$이면, 서로다른 세 실근이 존재하는데, \f$-b/2+i \sqrt{-Q} = \rho \exp (i\theta)\f$의
   polar representation을 이용하면 다름과 같이 표현된다.
   \f[
   x_1 = 2\rho^{1/3}\cos(\theta/3),
   x_2 = -\rho^{1/3}( \cos(\theta/3) +\sqrt{3} \sin(\theta/3)),
   x_3 = -\rho^{1/3}( \cos(\theta/3) -\sqrt{3} \sin(\theta/3)),
   \f]
   </ul>
   그러면 원래 방정식의 해는 \f$y_k = x_k - p/3\f$과 같아진다.
*/
    int solveCubicEquation(const double *C, double * x)
    {
        double p, q, r, a, b, Q, rho, theta, co, si, r1, r2, r3;

        if (C[0] == 0) return solveQuadraticEquation(C+1, x);
        p = C[1]/C[0]; q = C[2]/C[0]; r = C[3]/C[0];

        a = (3*q-p*p)/3.0; b = (2*p*p*p-9*p*q+27*r)/27; Q = b*b/4+a*a*a/27;

        if (Q > 0){ // unique real root
            Q=sqrt(Q);
            *x = __POW(-b*.5 + Q , 1.0/3.0) + __POW(-b*.5-Q , 1.0/3.0) - p/3;
            return 1;
        }
        if (Q == 0){ // duplicate real root : 첫번째와 세번째 근과 같은근이 중근
            r1 = __POW(b*.5, 1.0/3.0) - p/3;
            x[2] = r1;
            r3 = -2*__POW(b*.5, 1.0/3.0) - p/3;
            if (r1 < r3){ x[0] = r1; x[1] = r3;}
            else { x[0] = r3; x[1] = r1;}
            return 2;
        }
        // three distinct real roots
        b = - b/2; // real part
        Q = sqrt(-Q); // imanginary part

        // conver to polar form
        rho = sqrt(b*b + Q*Q);
        theta = atan2(Q, b);
        if (theta < 0) theta =  2*M_PI - theta;

        co = cos(theta/3); si = sin(theta/3);

        rho = __POW(rho , 1.0/3.0);
        r1 = 2*rho*co - p/3;
        r2 = -rho*(co + sqrt(3)*si) - p/3;
        r3 = -rho*(co - sqrt(3)*si) - p/3;
        // root sorting
        if (r1 < r2){
            if ( r2 < r3){ x[0] = r1; x[1] = r2; x[2] = r3;}
            else if (r3 < r1){ x[0] = r3; x[1] = r1; x[2] = r2;}
            else{ x[0] = r1; x[1] = r3; x[2] = r2;}
        }
        else{ // r2 < r1
            if (r1 < r3){ x[0] = r2; x[1] = r1; x[2] = r3;}
            else if ( r3 < r2){ x[0] = r3; x[1] = r2; x[2] = r1; }
            else{ x[0] = r2; x[1] = r3; x[2] = r1;}
        }
        return 3;
    }

/**
   C[0]x^2 +C[1]x +C[2]= 0의 해를 구한다.

   \param C 2차 방정식의 계수 (높은 차수부터)
   \param x 해가 담길 array
   \return 실근의 개수
*/
    int solveQuadraticEquation(const double *C, double * x)
    {
#define epsilon 1e-9
        double tmp;
        if (C[0] == 0) return solveLinearEquation(C+1, x);
        double D = C[1]*C[1] - 4*C[0]*C[2];
        //    if (D < -epsilon) return 0; //허근  ---> numerical error때문에 -_-;;
        if (D < 0) return 0;
        if (D <= 0){
            x[0] = -0.5*C[1]/C[0];
            x[1] = x[0];
            return 1; //중근
        }
        D = sqrt(D);
        x[0] = (-C[1]-D)/(2*C[0]);
        x[1] = (-C[1]+D)/(2*C[0]);
        if (x[0]>x[1]){ tmp = x[0]; x[0] = x[1]; x[1]=tmp;}
        return 2;
#undef epsilon
    }

/**
   C[0]x + C[1]= 0의 해를 구한다.

   \param C 1차 방정식의 계수
   \param x 해가 담길 pointer
   \return 실근의 개수
*/
    int solveLinearEquation(const double *C, double * x)
    {
        if (C[0] == 0){
            if (C[1] == 0) return -1; // 근이 무한개.
            return 0; //근이 없음
        }
        *x = -C[1]/C[0]; //실근 1개
        return 1;
    }


}


