//---------------------------------------------------------------------------
/**
@file    TrackBufferDetect.cpp
@version OHT 7.0
@brief   TrackBufferDetect Class 소스 File
*/

#pragma hdrstop

#include "TrackBufferDetect.h"


/**
@brief   TrackBufferDetect Class 생성자
@author	 LSJ
@date	 2012.10.22
@param   pHWCommon : HWCommon 클래스 포인터
*/
TrackBufferDetect::TrackBufferDetect(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   TrackBufferDetect Class 소멸자
@author	 LSJ
@date	 2012.10.22
*/
TrackBufferDetect::~TrackBufferDetect()
{

}

/**
@brief	 Left STB에 Foup이 있는지 없는지 체크하는 함수
@author	 LSJ
@date	 2012.10.22
@return	 Left STB에 Foup이 있으면 true, 없으면 false.(센서는 반사판을 감지하는 포토 센서임.)
*/
bool TrackBufferDetect::IsLeftSTBFoupDetect()
{
	bool bResult = false;

// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86) || (OHT_NAME == OHT_NAME_P4_MASK))
#else
	if(m_pHWCommon->m_Input.STB_Detect_Left == ON)
		bResult = true;
	else
		bResult = false;
#endif
	return bResult;
}

/**
@brief	 Right STB에 Foup이 있는지 없는지 체크하는 함수
@author	 LSJ
@date	 2012.10.22
@return	 Right STB에 반송물이 있으면 true, 없으면 false.(센서는 반사판을 감지하는 포토 센서임)
*/
bool TrackBufferDetect::IsRightSTBFoupDetect()
{
	bool bResult = false;

// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86) || (OHT_NAME == OHT_NAME_P4_MASK))
#else
    // by zzang9un 2013.07.09 : 센서 결과에 따라 반송물 존재 유무 판단
	if(m_pHWCommon->m_Input.STB_Detect_Right == ON)
		bResult = true;
	else
		bResult = false;
#endif

	return bResult;
}

/**
@brief	 UTB에 Foup이 있는지 없는지 체크하는 함수
@author	 LSJ
@date	 2012.10.22
@return	 UTB에 반송물이 있으면 true, 없으면 false.(센서는 반사판을 감지하는 포토 센서임.)
*/
bool TrackBufferDetect::IsUTBFoupDetect()
{
	bool bResult = false;

// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE))         //  || (OHT_NAME == OHT_NAME_STD_V85)
#else
    // by zzang9un 2013.07.09 : 센서 결과에 따라 반송물 존재 유무 판단
	if(m_pHWCommon->m_Input.UTB_Detect == ON)
		bResult = true;
	else
		bResult = false;
#endif
	return bResult;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
