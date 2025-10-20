/**
@file    VirtualTransControl.h
@version OHT 7.0
@brief   TransControl이 사용하는 Vitual TransControl 함수
*/

#ifndef VirtualTransControlH
#define VirtualTransControlH

#include <stdio.h>
#include "VirtualModuleControl.h"

/**
@brief   매뉴얼에서 사용할 주행부 class
@author  zzang9un
@date    2013.06.28
*/
class VirtualTransControl : public VirtualModuleControl
{
public:
    VirtualTransControl(void) : VirtualModuleControl()    /// 생성자, 변수 초기화
    {
        
    }
    virtual ~VirtualTransControl(void){;} /// 가상 소멸자

public:
    virtual int ChangeUse(int UseIndex, bool YesOrNo) = 0;    ///< Use 변수 변경함수
    virtual TRANSE_DIFFERENCE getDifference() = 0;	///< Rail 부터 Jig 바닥면까지의 Offset
};
//---------------------------------------------------------------------------

#endif


