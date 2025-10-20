/**
@file    HwMask.cpp
@version OHT 7.0
@brief   HwMask Class CPP File
*/

#include "HwMask.h"

/**
 @brief   HwMask Class 생성자
 @author  //
 @date    //
 @return  //
 */
HwMask::HwMask(int nCountOfInput, int nCountOfOutput, int nCountOfAxis)
{
	m_nCountOfInput		=	nCountOfInput;
	m_nCountOfOutput	=	nCountOfOutput;
	m_nCountOfAxis		=	nCountOfAxis;	

	m_bMakeTraceLog		=	false;
	
	m_aInput	= new MASK_INPUT	[m_nCountOfInput		];	
	m_aOutput	= new MASK_OUTPUT	[m_nCountOfOutput	];	
	m_aAxis		= new MASK_AXIS		[m_nCountOfAxis		];		
}

/**
 @brief   HwMask Class 소멸자
 @author  //
 @date    //
 @return  //
 */
HwMask::~HwMask() 
{
	delete [] m_aInput	;
	delete [] m_aOutput	;
	delete [] m_aAxis	;
}

/**
 @brief		Input Mask를 돌려주는 함수
 @author	ehoto
 @date		2013.02.16
 @param		nNum : Input Mask의 번호
 @return	해당 Mask가 있을 경우 Mask의 포인터 반환, 아닐 경우 NULL 반환
 */
MASK_INPUT*		HwMask::getMaskOfInput	(int nNum)
{
	MASK_INPUT	*pMask = NULL;
	if(nNum<m_nCountOfInput)
	{
		pMask = &m_aInput[nNum];
	}
	return pMask;
}

/**
 @brief		Output Mask를 돌려주는 함수
 @author	ehoto
 @date		2013.02.16
 @param		nNum : Output Mask의 번호
 @return	해당 Mask가 있을 경우 Mask의 포인터 반환, 아닐 경우 NULL 반환
 */
MASK_OUTPUT*		HwMask::getMaskOfOutput	(int nNum)
{
	MASK_OUTPUT	*pMask = NULL;
	if(nNum<m_nCountOfOutput)
	{
		pMask = &m_aOutput[nNum];
	}
	return pMask;
}

/**
 @brief		Axis Mask를 돌려주는 함수
 @author	ehoto
 @date		2013.02.16
 @param		nNum : Axis Mask의 번호
 @return	해당 Mask가 있을 경우 Mask의 포인터 반환, 아닐 경우 NULL 반환
 */
MASK_AXIS*		HwMask::getMaskOfAxis	(int nNum)
{
	MASK_AXIS	*pMask = NULL;
	if(nNum<m_nCountOfAxis)
	{
		pMask = &m_aAxis[nNum];
	}
	return pMask;
}


/**
 @brief	Trace Log 저장을 요청하는 함수
 @author	임태웅
 @date	2013.03.05
 */
void	HwMask::requsetToMakeTraceLog()
{
	m_bMakeTraceLog = true;
}

