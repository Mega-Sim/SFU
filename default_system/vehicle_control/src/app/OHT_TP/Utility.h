//---------------------------------------------------------------------------

#ifndef UtilityH
#define UtilityH
//---------------------------------------------------------------------------
class TOHTLOGFILE
{
 public :
   TOHTLOGFILE();
   ~TOHTLOGFILE();
   bool WriteLogFile(String, String);
   void DeleteLogFile();
   void CheckDirectory();
   void FileExistCheck(String, int);
   void Check_DeleteLogFile(String, int);

};

void Snooze(float numSec);


#endif
