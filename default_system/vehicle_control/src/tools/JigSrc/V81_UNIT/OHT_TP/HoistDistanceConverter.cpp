#include "HoistDistanceConverter.h"
#include <stdio.h>
#include <math.h>
//---------------------------------------------------------------------------

#include "OHTInfoUnit.h"
#define MAXROW 4
#define MAXCUL MAXROW
//---------------------------------------------------------------------------
CHoistConverter *HoistConverter;
//---------------------------------------------------------------------------
double multiply(double dValue, int nTimes)
{
   double dTmp = 1;
   for(int i=0; i<nTimes; i++)dTmp *= dValue;
   return dTmp;
}
//---------------------------------------------------------------------------
CHoistConverter::CHoistConverter()
{
   m_dA  =  0.0063, m_dB  = -3.1771, m_dC  = 304.3292, m_dD  = -3;   // AMC
   m_dMAxCts = 900000, m_dMinCts = -45000, m_dMaxMm = 3500, m_dMinMm = -175;
}
//---------------------------------------------------------------------------
CHoistConverter::CHoistConverter(double dA,double dB, double dC, double dD)
{
   m_dA = dA;
   m_dB = dB;
   m_dC = dC;
   m_dD = dD;
}
//---------------------------------------------------------------------------
CHoistConverter::~CHoistConverter(void)
{

}
//---------------------------------------------------------------------------
double CHoistConverter::calcCts2mm(int nCts)
{
   double   d100000Cts, dMiliMeter;

   if(nCts>m_dMAxCts || nCts<m_dMinCts) return -999999;   // 값을 찾지 못함 9:6개

   d100000Cts = nCts/100000.0;
   dMiliMeter = m_dA*multiply(d100000Cts,3)
               +m_dB*multiply(d100000Cts,2)
               +m_dC*multiply(d100000Cts,1)
               +m_dD;
   return dMiliMeter;
}
//---------------------------------------------------------------------------
int CHoistConverter::calcmm2Cts(double dDownDistance)
{
   //800000Cts에 2200mm 정도를 최대값으로 산정한다.
   int      nMaxCts = m_dMAxCts;
   int      nTmpCts = m_dMAxCts/2;
   int      nAddSign;
   double   dTmpDownDistance;

   // 값이 범위 밖이면 제거
   if(dDownDistance>m_dMaxMm || dDownDistance<m_dMinMm) return -999999;   // 값을 찾지 못함 9:6개

   for(int i=nMaxCts/4; i>0; i=i/2)
   {
      dTmpDownDistance = calcCts2mm(nTmpCts);
      nAddSign = (dTmpDownDistance-dDownDistance>=0)?-1:1;
      nTmpCts  = nTmpCts+(nAddSign*i);
      if(dTmpDownDistance>m_dMaxMm || dTmpDownDistance<m_dMinMm ) return -999999;   // 값을 찾지 못함 9:6개
   }
   return nTmpCts;
}
//---------------------------------------------------------------------------
bool CHoistConverter::calcCoefficient(double dP1, double dM1, double dP2, double dM2, double dP3, double dM3, double dP4, double dM4)
{
   int      i,j,k;
   int      irow;
   double   dtemp,pivot;
   double   a[MAXROW][2*MAXCUL]; // Gauss-Jordan
   double   a_1[MAXROW][MAXCUL]; // 역행렬
   double   b[MAXROW];           // A*B=Y, B=A_1*Y (A:100000Cts, B:계수, Y:mm)
   double   d100000P[MAXROW], dM[MAXROW];    // Input Value

    /*..Initialize..*/
   for(i=0;i<MAXROW;i++){
      for(j=0;j<MAXCUL;j++)   a_1[i][j]=0.;
      for(j=0;j<2*MAXCUL;j++) a[i][j]=0.;
   }

    /*..Convert Input Value..*/
   d100000P[0] = dP1/100000.0;   d100000P[1] = dP2/100000.0;   // 100000Cts
   d100000P[2] = dP3/100000.0;   d100000P[3] = dP4/100000.0;   // 100000Cts
   dM[0] = dM1;   dM[1] = dM2;   dM[2] = dM3;   dM[3] = dM4;   // Mili Meter

   /*..Get Max/Min Range..*/
   m_dMaxMm    = dM[0],          m_dMinMm    = dM[0];
   m_dMAxCts   = d100000P[0],    m_dMinCts   = d100000P[0];
   for(i=1;i<4;i++) {
      m_dMaxMm    = (dM[i]>m_dMaxMm)?  dM[i]:m_dMaxMm;
      m_dMinMm    = (dM[i]<m_dMinMm)?  dM[i]:m_dMinMm;
      m_dMAxCts   = (d100000P[i]>m_dMAxCts)? d100000P[i]:m_dMAxCts;
      m_dMinCts   = (d100000P[i]<m_dMinCts)? d100000P[i]:m_dMinCts;
   }
   m_dMAxCts   *= 100000.0;
   m_dMinCts   *= 100000.0;

   /*..Input Matrix Component..*/
   for(i=0;i<MAXROW;i++){
      for(j=0;j<MAXCUL;j++){
         a[i][j] = multiply(d100000P[i], MAXCUL-1-j); // [ x3 x2 x1 1 ]
         if(i==j) a[i][j+MAXCUL]=1.;
         else     a[i][j+MAXCUL]=0.;
      }
   }

   /*..Check det(A)..*/

   /*..pivot..*/
   for(i=0;i<MAXROW-1;i++){
        irow=i;
        for(k=i+1;k<MAXROW;k++){
             if(fabs(a[irow][i])<fabs(a[k][i])) irow=k;
        }
        for(j=0;j<2*MAXCUL;j++){
             dtemp=a[i][j];
             a[i][j]=a[irow][j];
             a[irow][j]=dtemp;
       }
   }

   /*..Gauss-Jordan..*/    // 역행렬 계산
   for(i=0;i<MAXROW;i++){
       pivot=a[i][i];
       for(j=0;j<2*MAXROW;j++) a[i][j]/=pivot;
       for(k=0;k<MAXROW;k++){
            if(k==i) continue;
            dtemp=a[k][i];
            for(j=i;j<2*MAXCUL;j++){
                 a[k][j]-=dtemp*a[i][j];
            }
       }
   }

   /*..Inverse Matrix..*/
   for(i=0;i<MAXROW;i++)
      for(j=0;j<MAXCUL;j++) a_1[i][j]=a[i][j+MAXCUL];

   /*..Calulate Coefficient..*/
   for(i=0;i<MAXROW;i++){
      b[i] = 0;
      for(j=0;j<MAXCUL;j++) b[i] += a_1[i][j] * dM[j];  // B = A_1 * Y
   }
   m_dA = b[0];   m_dB = b[1];   m_dC = b[2];   m_dD = b[3];
   return true;
}
//---------------------------------------------------------------------------
void CHoistConverter::getCoefficient(double &dA,double &dB, double &dC, double &dD)
{
   dA = m_dA;
   dB = m_dB;
   dC = m_dC;
   dD = m_dD;
}
//---------------------------------------------------------------------------

double CHoistConverter::getHoistOriginLimit(void)
{
	bool bHoistHome;
	if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_85){
		bHoistHome = gm_OHTStatus->IOInput->uInput.Input_EUV.HOIST_Home;
		if(bHoistHome == true) return 8.0; // 6mm(smalladd)+2mm(margin)
		else return 0.0; // error 발생
	}
	else return 2.0;
}
