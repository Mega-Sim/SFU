//---------------------------------------------------------------------------
/**
@file    CPUandMemInfo.cpp
$Rev:: 602           $:  Revision of last commit
$Author:: puting     $:  Author of last commit
$Date:: 2013-03-28 0#$:  Date of last commit
@version OHT
@brief   CPUandMemInfo Class CPP File
*/

#pragma hdrstop

#include "CPUandMemInfo.h"

//---------------------------------------------------------------------------
/**
@brief   CPUandMemInfo 생성자
@author  puting
@date    2013.03.28
*/
CPUandMemInfo::CPUandMemInfo()
 {

 	m_nNextIndex = 0;
	m_aCounters = new LList();

	Initialize();

	m_nCPU = AddCounter(CNTR_CPU);
	m_nMEM = AddCounter(CNTR_MEMINUSE_PERCENT);

 }
//-----------------------------------------------------------------------------
/**
@brief   CPUandMemInfo 소멸자
@author  puting
@date    2013.03.28
*/
CPUandMemInfo::~CPUandMemInfo()
{
   if(m_aCounters != NULL)
   {
	  int i;
	  int nCount = m_aCounters->getCount();
	  for(i=0 ; i<nCount; i++)
	  {
		 delete (PPDHCOUNTERSTRUCT *) m_aCounters->refer(i);
	  }
	  m_aCounters->clear();

	  delete m_aCounters;

	  m_aCounters = NULL;
   }
	Uninitialize();
}
//-----------------------------------------------------------------------------
 /**
@brief   Get_CPUandMem 함수
@author  puting
@date    2013.03.28
@param   m_TempCPUSysInfo : data를 얻기 위한 함수
@return  정상이면 true, 에러 발생 시 false 반환
*/
bool CPUandMemInfo::Get_CPUandMem(CPU_INFO* m_TempCPUSysInfo)
{
	long lMin = 0, lMax = 0, lMean = 0;

	if(!CollectQueryData())
	{
		return  false;
	}

	long lCPU = GetCounterValue(m_nCPU);
	long lMem = GetCounterValue(m_nMEM);

	if((lCPU != -999)&&(lMem != -999))
	{
		if(GetStatistics(&lMin, &lMax, &lMean, m_nCPU))
		{
			m_TempCPUSysInfo->nCPU = lMean;
			if (GetStatistics(&lMin, &lMax, &lMean, m_nMEM))
			{
				m_TempCPUSysInfo->lMem = lMean;
				return true;
			}
		}
	}
	else
		return false;

	return false;

}BOOL CPUandMemInfo::Initialize()
{
	if (PdhOpenQuery(NULL, 1, &m_hQuery) != ERROR_SUCCESS)
		return false;

	return true;

}

//---------------------------------------------------------------------------

void CPUandMemInfo::Uninitialize()
{
	PdhCloseQuery(&m_hQuery);

}

//---------------------------------------------------------------------------
// 구조체를 만들고 초기화 하고 인덱스를 넘겨준다.
int CPUandMemInfo::AddCounter(const char *pszCounterName)
{
	PPDHCOUNTERSTRUCT pCounter;
	pCounter = new PDHCOUNTERSTRUCT;
	if (!pCounter) return -1;

	// add to current query
	if (PdhAddCounterA(m_hQuery, pszCounterName, (DWORD)pCounter, &(pCounter->hCounter)) != ERROR_SUCCESS)
	{
		delete pCounter; // clean mem
//		MessageBoxA(NULL, "할당 실패","Corer",NULL);
		return -1;
	}

	// insert counter into array(s)
	pCounter->nIndex = m_nNextIndex++; // 클래스 생성시에 0으로 set
	pCounter->lValue = 0;
	pCounter->nNextIndex = 0;
	pCounter->nOldestIndex = 0;
	pCounter->nRawCount = 0;
	m_aCounters->insertFront(pCounter);

	return pCounter->nIndex;
}
//---------------------------------------------------------------------------



BOOL CPUandMemInfo::CollectQueryData()
{
	if (PdhCollectQueryData(m_hQuery) != ERROR_SUCCESS) return false;

	return true;
}

//---------------------------------------------------------------------------

BOOL CPUandMemInfo::UpdateValue(PPDHCOUNTERSTRUCT pCounter)
{
	PDH_FMT_COUNTERVALUE pdhFormattedValue;

	// get the value from the PDH
	if (PdhGetFormattedCounterValue(pCounter->hCounter, PDH_FMT_LONG, NULL, &pdhFormattedValue) != ERROR_SUCCESS)
		return false;

	// test the value for validity
	if (pdhFormattedValue.CStatus != ERROR_SUCCESS)
		return false;

	// set value
	pCounter->lValue = pdhFormattedValue.longValue;

	return true;
}
//---------------------------------------------------------------------------


BOOL CPUandMemInfo::UpdateRawValue(PPDHCOUNTERSTRUCT pCounter)
{
    PPDH_RAW_COUNTER ppdhRawCounter;

    // Assign the next value into the array
    ppdhRawCounter = &(pCounter->a_RawValue[pCounter->nNextIndex]);

	if (PdhGetRawCounterValue(pCounter->hCounter, NULL, ppdhRawCounter) != ERROR_SUCCESS)
		return false;

	// update raw counter - up to MAX_RAW_VALUES
	pCounter->nRawCount = min(pCounter->nRawCount + 1, MAX_RAW_VALUES);

    // Update next index - rolls back to zero upon reaching MAX_RAW_VALUES
	pCounter->nNextIndex = (pCounter->nNextIndex + 1) % MAX_RAW_VALUES;

	// The Oldest index remains zero until the array is filled.
    // It will now be the same as the 'next' index since it was previously assigned.
    if (pCounter->nRawCount >= MAX_RAW_VALUES)
        pCounter->nOldestIndex = pCounter->nNextIndex;

	return true;
}
//---------------------------------------------------------------------------

BOOL CPUandMemInfo::GetStatistics(long *nMin, long *nMax, long *nMean, int nIndex)
{
	PDH_STATISTICS pdhStats;
	PPDHCOUNTERSTRUCT pCounter = GetCounterStruct(nIndex);
	if (!pCounter) return false;

	if (PdhComputeCounterStatistics(pCounter->hCounter, PDH_FMT_LONG, pCounter->nOldestIndex, pCounter->nRawCount, pCounter->a_RawValue, &pdhStats) != ERROR_SUCCESS)
		return false;

	// set values
	if (pdhStats.min.CStatus != ERROR_SUCCESS)
		*nMin = 0;
	else
		*nMin = pdhStats.min.longValue;

	if (pdhStats.max.CStatus != ERROR_SUCCESS)
		*nMax = 0;
	else
		*nMax = pdhStats.max.longValue;

	if (pdhStats.mean.CStatus != ERROR_SUCCESS)
		*nMean = 0;
	else
		*nMean = pdhStats.mean.longValue;

	return true;
}
//---------------------------------------------------------------------------


long CPUandMemInfo::GetCounterValue(int nIndex)
{
	PPDHCOUNTERSTRUCT pCounter = GetCounterStruct(nIndex);
	if (!pCounter) return -999L;

	// update the value(s)
	if (!UpdateValue(pCounter)) return -999L;
	if (!UpdateRawValue(pCounter)) return -999L;

	// return the value
	return pCounter->lValue;
}
//---------------------------------------------------------------------------


PPDHCOUNTERSTRUCT CPUandMemInfo::GetCounterStruct(int nIndex)
{
	PPDHCOUNTERSTRUCT pps;

	int nCount = m_aCounters->getCount();
	for (int i=0;i<nCount;i++)
	{
		pps =(PPDHCOUNTERSTRUCT) m_aCounters->refer(i);
		if (pps->nIndex == nIndex){
			return pps;
        }
	}

	return NULL;
}


#pragma package(smart_init)
