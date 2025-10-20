//---------------------------------------------------------------------------
/**
@file    CPUandMemInfo.h
$Rev:: 467           $:  Revision of last commit
$Author:: puting   $:  Author of last commit
$Date:: 2013-03-28 1#$:  Date of last commit
@version OHT
@brief   CPUandMemInfo Class Header File
*/
#ifndef CPUandMemInfoH
#define CPUandMemInfoH
//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <mmsystem.h>


#include "ErrorCode.h"
#include "Def_HW.h"
#include "../Common/Def_DataType.h"

#include <pdh.h>
#include <pdhmsg.h>
#include "List.h"

//---------------------------------------------------------------------------
#define MAX_RAW_VALUES 20

#define min(a,b)     (((a)<(b))?(a):(b))

//// DEFINES FOR COUNTER NAMES ////
#define CNTR_CPU "\\Processor(_Total)\\% Processor Time" // % of cpu in use
#define CNTR_MEMINUSE_BYTES "\\Memory\\Committed Bytes" // mem in use measured in bytes
#define CNTR_MEMAVAIL_BYTES "\\Memory\\Available Bytes" // mem available measured in bytes
#define CNTR_MEMAVAIL_KB "\\Memory\\Available KBytes" // mem avail in kilobytes
#define CNTR_MEMAVAIL_MB "\\Memory\\Available MBytes" // mem avail in megabytes
#define CNTR_MEMINUSE_PERCENT "\\Memory\\% Committed Bytes In Use" // % of mem in use
#define CNTR_MEMLIMIT_BYTES "\\Memory\\Commit Limit" // commit limit on memory in bytes

typedef struct _tag_PDHCounterStruct {
	int nIndex;				// The index of this counter, returned by AddCounter()
	LONG lValue;			// The current value of this counter
    HCOUNTER hCounter;      // Handle to the counter - given to use by PDH Library
    int nNextIndex;         // element to get the next raw value
    int nOldestIndex;       // element containing the oldes raw value
    int nRawCount;          // number of elements containing raw values
    PDH_RAW_COUNTER a_RawValue[MAX_RAW_VALUES]; // Ring buffer to contain raw values
} PDHCOUNTERSTRUCT, *PPDHCOUNTERSTRUCT;




/**
@brief   CPUandMemInfo Å¬·¡½º
@author  puting
@date    2013.03.28
*/
class CPUandMemInfo
{

private:

	//// SETUP ////
	BOOL Initialize(void);
	void Uninitialize(void);

	//// COUNTERS ////
	int AddCounter(const char *pszCounterName);

	//// DATA ////
	BOOL CollectQueryData(void);
	BOOL GetStatistics(long *nMin, long *nMax, long *nMean, int nIndex);
	long GetCounterValue(int nIndex);


	//// COUNTERS ////
	PPDHCOUNTERSTRUCT GetCounterStruct(int nIndex);

	//// VALUES ////
	BOOL UpdateValue(PPDHCOUNTERSTRUCT pCounter);
	BOOL UpdateRawValue(PPDHCOUNTERSTRUCT pCounter);

	//// VARIABLES ////
	HQUERY m_hQuery; // the query to the PDH
   //	CArray<PPDHCOUNTERSTRUCT, PPDHCOUNTERSTRUCT> m_aCounters; // the current counters
    LList *m_aCounters;
	int m_nNextIndex;

private:
	long m_nCPU;
	long m_nMEM;

public:
	CPUandMemInfo();
	~CPUandMemInfo();
	bool Get_CPUandMem(CPU_INFO* cpuInfo);


};
#endif
