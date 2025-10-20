/**
@file    InnerFoup.cpp
@version OHT 7.0
@brief   InnerFoup Class CPP File
*/

#include "InnerFoup.h"

/**
@brief   InnerFoup Class 생성자
@author	 LSJ
@date    2012.10.27
@param   pHWCommon : HWCommon Class의 포인터
*/
InnerFoup::InnerFoup(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   InnerFoup Class 소멸자
@author	 LSJ
@date    2012.10.27
*/
InnerFoup::~InnerFoup()
{
}

/**
@brief   Foup 존재 여부를 체크하는 함수
@author  zzang9un
@date    2013.08.05
@return  Foup이 있으면 true, 없으면 false를 리턴
*/
bool InnerFoup::IsFoupDetect()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.FOUP_Detect == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}


bool InnerFoup::IsEuvDetect()
{
	bool bResult = false;

// EUV_1, EUV_2 라는 이름이 다른 OHT define으로는 없으므로 이렇게 해둬야 에러 안난다
// EUV용 이외일 때에는 항상 false를 리턴
#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86))&& (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
#if(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
	if((m_pHWCommon->m_Input.POD_EUV_1 == ON) && (m_pHWCommon->m_Input.POD_EUV_2 == ON))
#else
	if((m_pHWCommon->m_Input.POD_EUV_1 == ON))
#endif
		bResult = true;
	else
		bResult = false;
#endif

	return bResult;
}


#if(OHT_NAME == OHT_NAME_P4_MASK)
/**
@brief   Pod Detect Check용 함수
@author  mgchoi
@date    2023.01.31
@return  POD가 있으면 true, 없으면 false를 리턴
*/
bool InnerFoup::IsPODDetect()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.FOUP_Detect == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   EUV Pod Detect Check용 함수
@author  mgchoi
@date    2022.08.19
@return  EUV Pod가 있으면 true, 없으면 false를 리턴
*/
bool InnerFoup::IsEUVPodDetect()
{
bool bResult = false;

	if(m_pHWCommon->m_Input.EUVPod_Detect == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   RSP150 Detect Check용 함수
@author  mgchoi
@date    2022.08.19
@return  RSP150가 있으면 true, 없으면 false를 리턴
*/
bool InnerFoup::IsRSP150Detect()
{
bool bResult = false;

	if(m_pHWCommon->m_Input.RSP150_Detect == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   RSP200 Detect Check용 함수
@author  mgchoi
@date    2022.08.19
@return  RSP200가 있으면 true, 없으면 false를 리턴
*/
bool InnerFoup::IsRSP200Detect()
{
bool bResult = false;

	if(m_pHWCommon->m_Input.RSP200_Detect == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}
#endif
