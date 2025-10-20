#ifndef HoistDistanceConverterH
#define HoistDistanceConverterH
#include <Classes.hpp>
//---------------------------------------------------------------------------
// Example
// 800000Cts에 2200mm 정도를 최대값으로 산정한다.
//   PMAC:  m_dA = -0.0083, m_dA = -2.5417, m_dC = 270.1167, m_dD =  0;
//   AMC:   m_dA =  0.0063, m_dA = -3.1771, m_dC = 304.3292, m_dD = -3;
//---------------------------------------------------------------------------
class CHoistConverter
{
   public:
      CHoistConverter();
      CHoistConverter(double dA,double dB, double dC, double dD);
      ~CHoistConverter(void);

   private:
      double   m_dA,  m_dB,  m_dC,  m_dD;
      double   m_dMAxCts, m_dMinCts, m_dMaxMm, m_dMinMm;

   public:
      //800000Cts에 2200mm 정도를 최대값으로 산정한다.
      double   calcCts2mm  (int nCts);
      int      calcmm2Cts  (double dDownDistance);

      bool     calcCoefficient(double dP1, double dM1, double dP2, double dM2, double dP3, double dM3, double dP4, double dM4);
	  void     getCoefficient(double &dA,double &dB, double &dC, double &dD);
	  static double getHoistOriginLimit(void);
};
extern CHoistConverter *HoistConverter;
//---------------------------------------------------------------------------
#endif

