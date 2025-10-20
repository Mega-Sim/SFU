//---------------------------------------------------------------------------

#ifndef UtilityH
#define UtilityH
//---------------------------------------------------------------------------
class OHTLogFile
{
 private :

 public :
   OHTLogFile();
   ~OHTLogFile();
   bool WriteLogFile(String strTmpFileName, String strTmpLogData);
   bool WriteLogFileAtD(String strTmpFileName, String strTmpLogData, String Path);
   void DeleteLogFile();
   void CheckDirectory();
   void FileExistCheck(AnsiString, int);
   void Check_DeleteLogFile(AnsiString, int);
};

void Snooze(float numSec);
__int64 __fastcall GetLocalFileSize(String FileName);

#endif
