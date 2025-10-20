#ifndef Def_TimeCheckH
#define Def_TimeCheckH

#include <stdio.h>
#include "Def_Variables.h"

/**
@brief	 변경 시각 및 유지 시간 확인 구조체
@author  ehoto
@date	 2013.06.19
*/
typedef struct TIME_CHECK_
{
	bool 	bFlag;	///< 변경 발생 확인 Flag
	DWORD	dwTime;	///< 변경 발생 시각
}TIME_CHECK;

#endif
