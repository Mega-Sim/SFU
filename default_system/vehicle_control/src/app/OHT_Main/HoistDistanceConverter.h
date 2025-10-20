#ifndef HoistDistanceConverterH
#define HoistDistanceConverterH
#include <Classes.hpp>
#include "VirtualUnitConverter.h"	// 단위 표준화의 추상 클래스를 상속 받음
#include "Def_OHT.h"

//---------------------------------------------------------------------------
// Example
// 800000Cts에 2200mm 정도를 최대값으로 산정한다.
//   PMAC:  m_dA = -0.0083, m_dA = -2.5417, m_dC = 270.1167, m_dD =  0;
//   AMC:   m_dA =  0.0063, m_dA = -3.1771, m_dC = 304.3292, m_dD = -3;
//---------------------------------------------------------------------------
class HoistDistanceConverter:public VirtualUnitConverter
{
   public:
      HoistDistanceConverter(void);
      ~HoistDistanceConverter(void);

   private:
	  double   m_dA,  m_dB,  m_dC,  m_dD;
	  double   m_dA2,  m_dB2,  m_dC2,  m_dD2;
	  double   m_dMaxCts, m_dMinCts, m_dMaxUser, m_dMinUser;
	  double   m_CenterPosition, m_CenterPulse;
	  double   m_dMinCts_Limit, m_dMinUser_Limit;
	  
#ifdef OHT_HIGH_RAIL_HOIST_PRAM
	  double   m_dA3,  m_dB3,  m_dC3,  m_dD3;
	  double   m_CenterPosition2, m_CenterPulse2;
#endif

   public:
      //800000Cts에 2200mm 정도를 최대값으로 산정한다.
      double   calcCts2User  (double dCts);			// Cts2mm
	  double   calcUser2Cts  (double dDownDistance);	// mm2Cts

	  double   calcSpeedCts2User  (double dCtsSpeed, double dCtsPosition);		// Cts2mm	without time change
	  double   calcSpeedUser2Cts  (double dDownSpeed, double dDownPosition);	// mm2Cts	without time change

	  double   calcAccelCts2User  (double dCtsAccel, double dCtsPosition);		// Cts2mm	without time change
	  double   calcAccelUser2Cts  (double dDownAccel, double dDownPosition);	// mm2Cts	without time change

	  bool     isSameDirection(void);

	  bool     calcCoefficient(double dP1, double dM1, double dP2, double dM2, double dP3, double dM3, double dP4, double dM4);
	  bool     calcCoefficient2(double dP1, double dM1, double dP2, double dM2, double dP3, double dM3, double dP4, double dM4);
#ifdef OHT_HIGH_RAIL_HOIST_PRAM
	  bool     calcCoefficient3(double dP1, double dM1, double dP2, double dM2, double dP3, double dM3, double dP4, double dM4);
#endif
	  double  GetHoistMaxUserPosition();
      double  GetHoistMinUserPosition();
	 // bool     calcCoefficient(double dP12, double dM12, double dP2, double dM2, double dP3, double dM3, double dP4, double dM4);
      int SetNegativeLimit4PulseMoving(double dMinCtsValue);
};
//---------------------------------------------------------------------------
#endif

