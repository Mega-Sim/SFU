#ifndef TestLogH
#define TestLogH

#include <stdio.h>
#include <windows.h>
#include "Def_OHT.h"

//// 로그 작성 함수
char	g_szTestLog[1024]	= {0,};

void writeTestLog(bool bUse, char* pLog)
{
	if(bUse==false) return;

	SYSTEMTIME	CurTime;
	GetLocalTime(&CurTime);
	FILE *fp;
	fp = fopen("log\\Test.log", "a");

	if (fp != NULL)
	{
	  try
	  {
		 fprintf(fp, "[%02d:%02d:%02d.%02d]%s\n", 
			CurTime.wHour, CurTime.wMinute, CurTime.wSecond, CurTime.wMilliseconds, pLog);
		 fflush(fp);
	  }
	  catch (Exception &e)
	  {
		 fclose(fp);
		 fp = NULL;
		 return;
	  }
	  fclose(fp);
	  return;
	}
	return;
}

void resetTestLog()
{
	FILE *fp;
	fp = fopen("log\\Test.log", "w");
	fclose(fp);
}

#define ADD_TEST_LOG(bUse, ...)	memset(&g_szTestLog, 0, sizeof(g_szTestLog));\
						sprintf(g_szTestLog, __VA_ARGS__);\
						writeTestLog(bUse, g_szTestLog);

#define RESET_TEST_LOG   resetTestLog();

#endif
