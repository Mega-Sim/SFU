#ifndef MinMaxCtrlH
#define MinMaxCtrlH

#include <stdio.h>


template <typename T>
T applyMinLimit(T OriginValue, T MinValue)
{
	return (OriginValue>MinValue)?OriginValue:MinValue;
}

template <typename T>
T applyMaxLimit(T OriginValue, T MaxValue)
{
	return (OriginValue<MaxValue)?OriginValue:MaxValue;
}

template <typename T>
bool applyMinMaxLimit(T OriginValue, T MinValue, T MaxValue, T *Result)
{
	bool bReturn = false;
	T	TmpResult = OriginValue;
	
	if(MinValue<=MaxValue) 
	{
		TmpResult = applyMinLimit(OriginValue, MinValue);
		*Result = applyMaxLimit(TmpResult, MaxValue);
		bReturn = true;
	}
	
	return bReturn;
}



template <typename T>
bool applyMinLimitCheck(T OriginValue, T MinValue)
{
	return (OriginValue<MinValue)?false:true;
}

template <typename T>
bool applyMaxLimitCheck(T OriginValue, T MaxValue)
{
	return (OriginValue>MaxValue)?false:true;
}

template <typename T>
bool applyMinMaxLimitCheck(T DefualtnValue,T OriginValue, T MinValue, T MaxValue, T *Result)
{
	bool bReturn = false;

	if(MinValue<=MaxValue)
	{
		if(applyMinLimitCheck(OriginValue, MinValue) && applyMaxLimitCheck(OriginValue, MaxValue))
		{
			*Result = OriginValue;
			 bReturn = true;
		}else
		{
			*Result = DefualtnValue;
		}
	}else
	{
    	*Result = DefualtnValue;
    }

	return bReturn;
}


template <typename T>
struct MIN_MAX_VALUE
{
	T MinValue;
	T MaxValue;
};

#endif