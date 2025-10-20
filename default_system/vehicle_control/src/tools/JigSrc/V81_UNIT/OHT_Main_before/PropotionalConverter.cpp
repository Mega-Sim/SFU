#include <stdio.h>
#include <math.h>
#include "PropotionalConverter.h"
//#include "LogHandler.h"
//---------------------------------------------------------------------------

PropotionalConverter::PropotionalConverter()
{
	setCoefficient(1.0);
}
//---------------------------------------------------------------------------
PropotionalConverter::PropotionalConverter(double dRatioUser2Cts)
{
	setCoefficient(dRatioUser2Cts);
}
//---------------------------------------------------------------------------
PropotionalConverter::~PropotionalConverter(void)
{
}
//---------------------------------------------------------------------------
double PropotionalConverter::calcCts2User(double dCts)
{
	double dUserUnit = ((double)dCts)/m_dRatioUser2Cts;
	return dUserUnit;
}
//---------------------------------------------------------------------------
double PropotionalConverter::calcUser2Cts(double dUserUnit)
{
	return (dUserUnit*m_dRatioUser2Cts);
}
//---------------------------------------------------------------------------
bool PropotionalConverter::isSameDirection(void)
{
	if(m_dRatioUser2Cts>0) return true;
	else return false;
}
//---------------------------------------------------------------------------
bool PropotionalConverter::setCoefficient(double dRatioUser2Cts)
{
	bool bReturn = false;
	
	if(dRatioUser2Cts==0.0)
	{
		m_dRatioUser2Cts = 1.0;
		bReturn = false;
	}
	else
	{
		m_dRatioUser2Cts = dRatioUser2Cts;
		bReturn = true;
	}
	return bReturn;
}
//---------------------------------------------------------------------------
void PropotionalConverter::getCoefficient(double& dRatioUser2Cts)
{
	dRatioUser2Cts = m_dRatioUser2Cts;
}
//---------------------------------------------------------------------------
