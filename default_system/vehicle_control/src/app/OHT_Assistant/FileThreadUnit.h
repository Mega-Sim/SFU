//---------------------------------------------------------------------------

#ifndef FileThreadUnitH
#define FileThreadUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class FileThreadUnit : public TThread
{
private:
protected:
	void __fastcall Execute();
public:
	__fastcall FileThreadUnit(bool CreateSuspended);

   bool DeleteLogFile(int nDaysBefore, AnsiString strOHTPath);
   bool DeleteLogZip(int nDaysBefore, AnsiString strOHTPath);
   DWORD dwSTTime;

};
//---------------------------------------------------------------------------
extern FileThreadUnit *ptrFileThread;
//---------------------------------------------------------------------------
#endif
