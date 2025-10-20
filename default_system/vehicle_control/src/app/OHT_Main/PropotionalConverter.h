#ifndef PropotionalConverterH
#define PropotionalConverterH
#include <Classes.hpp>
#include "VirtualUnitConverter.h"

//---------------------------------------------------------------------------
class PropotionalConverter:public VirtualUnitConverter
{
public:
	PropotionalConverter(void);
	PropotionalConverter(double dRatioUser2Cts);
	~PropotionalConverter(void);

private:
	double   m_dRatioUser2Cts;

public:
	double   calcCts2User  (double dCts);				// Cts2mm
	double   calcUser2Cts  (double dUserUnit);	// mm2Cts
	bool     isSameDirection(void);

	bool     setCoefficient(double dRatioUser2Cts);
//	void     getCoefficient(double& dRatioUser2Cts);         //¹Ì»ç¿ë
};
//---------------------------------------------------------------------------
#endif

