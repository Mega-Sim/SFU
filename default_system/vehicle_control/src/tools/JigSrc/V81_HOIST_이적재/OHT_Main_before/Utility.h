//---------------------------------------------------------------------------

#ifndef UtilityH
#define UtilityH

#include <stdio.h>
//#include <ScktComp.hpp>
#include <vcl.h>

#include "Def_OHT.h"
#include "../Common/Def_DataType.h"

//---------------------------------------------------------------------------
class OHTUTILITY
{
 public :
   OHTUTILITY();
   ~OHTUTILITY();

   static void CheckDirectory();
   static void Check_DeleteLogFile(AnsiString, int);
   static int DeleteFile_SizeZero(AnsiString);
   static int Get_IsLogDriveAlive(AnsiString);
   static int GetNum_LogFile(void);
   static int File_Delete_All(AnsiString);

	static bool Check_DDrive();
	static bool Check_LogFolder();

	static AnsiString Get_FileAccessTime(AnsiString);
	static int Check_VirusDirectory(void);
	static unsigned long PreventOverflowGetTimeMs(unsigned long precalc);

	static int GetDiskInfo(DISK_INFO* pDiskInfo, char cDrive);
};

//---------------------------------------------------------------------------

#endif
