//---------------------------------------------------------------------------

#ifndef UtilityH
#define UtilityH

#include <stdio.h>
//#include <ScktComp.hpp>
#include <vcl.h>

#include "Def_OHT.h"
#include "../Common/Def_DataType.h"

#define STRINGIFY(x)               #x // 변수 이름을 문자열로 변환

//---------------------------------------------------------------------------
class OHTUTILITY
{
 public :
   OHTUTILITY();
   ~OHTUTILITY();

   static void CheckDirectory();
//   static void Check_DeleteLogFile(AnsiString, int);   //미사용
//   static int DeleteFile_SizeZero(AnsiString);         //미사용
   static int Get_IsLogDriveAlive(AnsiString);
   static int GetNum_LogFile(void);
//   static int File_Delete_All(AnsiString);             //미사용

	static bool Check_DDrive();
	static bool Check_LogFolder();

	static AnsiString Get_FileAccessTime(AnsiString);
	static int Check_VirusDirectory(void);
	static unsigned long PreventOverflowGetTimeMs(unsigned long precalc);

	static int GetDiskInfo(DISK_INFO* pDiskInfo, char cDrive);
	static void CompareVariables(AnsiString name, double val1, double val2);
};

//---------------------------------------------------------------------------

enum TimeUnit { Second, Minute, Hour, Day, Week };

class PeriodicCounter {
private:
    int nCount;                   // count 변수
    TimeUnit timeUnit;            // 시간 단위
    TDateTime checkTime;          // 최초 시작 시간
	TDateTime durationTime;		  // 기준 시간
    bool bAdjustFirst;            // 최초 주기의 시간 조정 여부

    TDateTime getDuration(int nMultiplier) const; // 단위별 지속 시간 반환
	TDateTime getNextPeriod(bool bInit = false) const; // 다음 기준 시점 반환

    void reset(bool bInit = false); // reset count, time
    void resetIfNeeded(); // 현재 시간이 기준 시간을 초과했는지 확인 후 리셋

public:
    PeriodicCounter(TimeUnit unit, int multiplier = 1, bool bAdjustFirst = false); // 생성자: 초기 시간 저장 + count 초기화

    int increment(); // count 증가

    int getCount(); // 현재 count 조회

    void resetCountOnly(); // reset count (Error 발생 이후 초기화 용도)

    bool updateAndCheckThreshold(int nThreshold, int *nCnt); // count 증가 및 한계치 도달 여부 확인
};
//---------------------------------------------------------------------------

#endif
