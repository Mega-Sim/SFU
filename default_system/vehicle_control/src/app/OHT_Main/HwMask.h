/**
@file    HwMask.h
@version OHT 7.0
@brief   HwMask Class Header File
*/

#ifndef HwMaskH
#define HwMaskH

#include <stdio.h>

template <typename T>
struct	MASK
{
	bool	bUse;		///< Use/NoUse 정보
	T		Value;		///< On/Off 정보
	
	MASK()
	{
		bUse	= false;
		Value	= 0;
	}
};

typedef MASK<bool>	MASK_INPUT;
typedef MASK<bool>	MASK_OUTPUT;

typedef	struct	MASK_AXIS_
{
	MASK<double>	Position;		///< 위치 정보
	MASK<bool>		AmpEnable;		///< AMP Eable 정보
	MASK<bool>		Running;		///< 동작여부 정보
	MASK<int>		ErrorCode;		///< 에러 코드
    // 2016-12-08, Add by Labeler, Rotate 축의 경우 Origin 정보 필요로 인해 추가
    MASK<int>       Origin;         ///< Motor Origin 정보
	MASK<int>	Speed;
//	MASK<double>	Speed;


}MASK_AXIS;

/**
@class   HwMask
@date
@author  ehoto
@brief   HW를 Masking 하는 구조체 클래스
*/

class HwMask
{
private:
	unsigned int	m_nCountOfInput;
	unsigned int	m_nCountOfOutput;
	unsigned int	m_nCountOfAxis;
	
public:
	/// I/O 관련 변수 ----------------------------------
	bool	m_bMakeTraceLog;

	/// I/O 관련 변수 ----------------------------------
	MASK_INPUT	*m_aInput;	///< Input  Mask의 동적 배열 포인터
	MASK_OUTPUT	*m_aOutput;	///< Output Mask의 동적 배열 포인터
	MASK_AXIS	*m_aAxis;	///< Axis   Mask의 동적 배열 포인터

	/// 생성자 & 소멸자 --------------------------------
	HwMask(int nCountOfInput, int nCountOfOutput, int nCountOfAxis);
	~HwMask();
	
	MASK_INPUT*		getMaskOfInput	(int nNum);	///< Input  Mask의 포인터를 받아오는 함수
//	MASK_OUTPUT*	getMaskOfOutput	(int nNum);	///< Input  Mask의 포인터를 받아오는 함수         //미사용
	MASK_AXIS*		getMaskOfAxis	(int nNum);	///< Input  Mask의 포인터를 받아오는 함수

//	void	requsetToMakeTraceLog();	///< Trace Log 저장을 요청하는 함수                       //미사용

};

#endif  //HwMaskH
