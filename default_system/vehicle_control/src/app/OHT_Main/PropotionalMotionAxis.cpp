/**
@file    PropotionalMotionAxis.cpp
@version OHT 7.0
@brief   PropotionalMotionAxis Class 소스 File
*/
#include "PropotionalMotionAxis.h"
//#include "LogHandler.h"


/**
@brief   PropotionalMotionAxis Class 생성자
@date
@author
@param   pHWCommon : HWCommon 클래스 포인터
@param   nAxisNum : Motor에 사용될 축 번호
*/
PropotionalMotionAxis::PropotionalMotionAxis(HWCommon *pHWCommon, int nAxisNum, bool bAbsoluteEncoder, double dGearRatio):MotionAxis(pHWCommon, nAxisNum, bAbsoluteEncoder)
{
	m_GearRatio.dIdealGearRatio = dGearRatio;
	m_GearRatio.dIdeal2RealRatio = 1.0;
	m_pPropotionalConverter = new PropotionalConverter(dGearRatio);
	EnableUnitConverter();
}
/**
@brief   PropotionalMotionAxis Class 소멸자
@date
@author
*/
PropotionalMotionAxis::~PropotionalMotionAxis()
{
	DisableUnitConverter();
	delete m_pPropotionalConverter;
	m_pPropotionalConverter = NULL;
}

/**
@brief   PropotionalMotionAxis 단위 변환 기능 사용 결정 함수

	기어비 설정은 m_pPropotionalConverter를 이용한다.
@date    // 입력
@author  // 입력
@param   // 입력(없을 경우 삭제)
@return  Unit 사용 가능 하면 true Return
*/
bool PropotionalMotionAxis::EnableUnitConverter(void)
{
	if(m_pPropotionalConverter==NULL)
	{
		return false;
	}
	else
	{
		m_pUnitConverter = m_pPropotionalConverter;
		return true;
	}
}

/**
@brief   기어비 설정 함수
@author  임태웅
@date	 2013.06.23
@return  성공 여부
*/
bool PropotionalMotionAxis::SetGearRatio(PROPOTINAL_GEAR_RATIO GearRatio)
{
	double dGearRatio = GearRatio.dIdealGearRatio * GearRatio.dIdeal2RealRatio;
	if((m_pPropotionalConverter!=NULL)&&(dGearRatio>0))
	{
		m_GearRatio = GearRatio;
		return m_pPropotionalConverter->setCoefficient(dGearRatio);
    }
    else
    {
    	return false;
    }
}

/**
@brief   기어비 획득 함수
@author  임태웅
@date	 2013.06.23
@return  성공 여부

PROPOTINAL_GEAR_RATIO	PropotionalMotionAxis::GetGearRatio()
{
	return m_GearRatio;
}
*/
