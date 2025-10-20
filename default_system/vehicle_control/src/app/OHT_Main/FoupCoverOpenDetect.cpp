//---------------------------------------------------------------------------
#pragma hdrstop

#include "FoupCoverOpenDetect.h"


/**
@brief   FoupCoverOpenDetect 생성자
@author  KKS
@date    2015.01.20
@param   pHWCommon : HWCommon Class의 포인터
*/
FoupCoverOpenDetect::FoupCoverOpenDetect(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   FoupCoverOpenDetect 소멸자
@author  KKS
@date    2015.01.20
*/

 FoupCoverOpenDetect::~FoupCoverOpenDetect()
{
}

/**
@brief   Foup cover open 감지유무를 체크하는 함수
@author  KKS
@date    2015.01.20
@return  Open 감지 true, 그렇지 않은 경우 false
*/
bool FoupCoverOpenDetect::IsDetect()
{
	bool bResult = false;

#if (((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V81S)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)) && (SAMSUNG_COMMON_SPEC==ON))
 	if(m_pHWCommon->m_Input.FOUP_Cover_Open_Detect == ON)
	{
		bResult = true;
	}
#endif

	return bResult;
}

#pragma package(smart_init)
