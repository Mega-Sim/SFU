//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <dir.h>
#include <mmsystem.h>
#include <IniFiles.hpp>

#include "FileThreadUnit.h"
#include "AssistantMain.h"
#include "Define.h"
#include "Utility.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall FileThreadUnit::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------
FileThreadUnit *ptrFileThread = NULL;
//---------------------------------------------------------------------------

__fastcall FileThreadUnit::FileThreadUnit(bool CreateSuspended)
	: TThread(CreateSuspended)
{
   Priority = tpNormal;
   Resume();
   dwSTTime=timeGetTime();
}
//---------------------------------------------------------------------------
void __fastcall FileThreadUnit::Execute()
{
	//---- Place thread code here ----
	while( !Terminated )
	{
		if(timeGetTime()-dwSTTime>AssistantMainForm->m_AstParam.nFileMonitoringInterval)
		{
			DeleteLogFile((int)AssistantMainForm->m_AstParam.nMaxWaitTimeToDelete
						,ExtractFileDir(Application->ExeName));

			dwSTTime=timeGetTime();
		}
	  	WaitForSingleObject((void *)Handle, 1000);
	}
}

//----------------------------------------------------------------------------
bool FileThreadUnit::DeleteLogFile(int nDaysBefore, AnsiString strOHTPath)
{
   struct ffblk ffblk;
   int done;
   TDateTime LastDate, LastTime, CurrentDate;
   int Year, Month, Day, Hour, Min, Sec;
   char FileName[1024];
   AnsiString strFName;
   bool bR;
   int nLogSaveMode = AssistantMainForm->m_AstParam.nLogSaveMode;
   AnsiString strLogPath = AssistantMainForm->m_AstParam.strLogPath;

   if((nLogSaveMode==0)||(nLogSaveMode==3)) return false;

   CurrentDate = Now();

   if(nLogSaveMode==1)
	  strFName.sprintf("%s%s*.log",strOHTPath, STR_LOGFILE_PATH);
   else if(nLogSaveMode==2)
   {
	  strFName.sprintf("%s*.log",strLogPath);
   }

   done = findfirst(strFName.c_str(),&ffblk,0);
   while (!done)
   {
      Year = 1980+((ffblk.ff_fdate >> 9)&127);
      Month = (ffblk.ff_fdate >> 5)&15;
      Day = (ffblk.ff_fdate)&31;
      Hour = (ffblk.ff_ftime>>11)&31;
      Min = (ffblk.ff_ftime>>5)&63;
      Sec = 2*((ffblk.ff_ftime)&31);

      LastTime = TDateTime::TDateTime(Hour, Min, Sec, 0);
      LastDate = TDateTime::TDateTime(Year, Month, Day)+(LastTime-(int)LastTime);

      if (CurrentDate > LastDate + nDaysBefore)
      {
		 if(nLogSaveMode==1)
			strFName.sprintf("%s%s%s", strOHTPath,STR_LOGFILE_PATH,ffblk.ff_name);
		 else if(nLogSaveMode==2)
            strFName.sprintf("%s%s", strLogPath, ffblk.ff_name);

         try
         {
            bR=DeleteFile(strFName);
            if(bR==false) //읽기전용일 경우
            {
			   SetFileAttributesA(strFName.c_str(), FILE_ATTRIBUTE_NORMAL);
               DeleteFile(strFName);
            }
         }
         catch (Exception &e)
		 {
		 }
      }
      done = findnext(&ffblk);
      Sleep(10);
   }

   if(nLogSaveMode==1)
      strFName.sprintf("%s%s*.bmp",strOHTPath,STR_LOGFILE_PATH);
   else if(nLogSaveMode==2)
      strFName.sprintf("%s*.bmp",strLogPath);

   done = findfirst(strFName.c_str(),&ffblk,0);
   while (!done)
   {
      Year = 1980+((ffblk.ff_fdate >> 9)&127);
      Month = (ffblk.ff_fdate >> 5)&15;
      Day = (ffblk.ff_fdate)&31;
      Hour = (ffblk.ff_ftime>>11)&31;
      Min = (ffblk.ff_ftime>>5)&63;
      Sec = 2*((ffblk.ff_ftime)&31);

      LastTime = TDateTime::TDateTime(Hour, Min, Sec, 0);
      LastDate = TDateTime::TDateTime(Year, Month, Day)+(LastTime-(int)LastTime);

      if (CurrentDate > LastDate + nDaysBefore)
      {
         if(nLogSaveMode==1)
            strFName.sprintf("%s%s%s",strOHTPath,STR_LOGFILE_PATH,ffblk.ff_name);
         else if(nLogSaveMode==2)
            strFName.sprintf("%s%s",strLogPath,ffblk.ff_name);

         try
         {
            bR=DeleteFile(strFName);
            if(bR==false)
            {
			   SetFileAttributesA(strFName.c_str(), FILE_ATTRIBUTE_NORMAL);
			   DeleteFile(strFName);
            }
         }
         catch (Exception &e)
         {
         }
      }
      done = findnext(&ffblk);
      Sleep(10);
   }
   return true;
}
bool FileThreadUnit::DeleteLogZip(int nDaysBefore, AnsiString strOHTPath)
{
	struct ffblk ffblk;
	int done;
	TDateTime LastDate, LastTime, CurrentDate;
	int Year, Month, Day, Hour, Min, Sec;
	char FileName[1024];
	AnsiString strFName;
	bool bR;
	int nLogSaveMode = AssistantMainForm->m_AstParam.nLogSaveMode;
	AnsiString strLogPath = AssistantMainForm->m_AstParam.strLogPath;

   if((nLogSaveMode==0)||(nLogSaveMode==3)) return false;

   CurrentDate = Now();


   if(nLogSaveMode==1)
	  strFName.sprintf("%s%s*.zip",strOHTPath, STR_LOGFILE_PATH);
   else if(nLogSaveMode==2)
	  strFName.sprintf("%s*.zip",strLogPath);

   done = findfirst(strFName.c_str(),&ffblk,0);
   while (!done)
   {
      Year = 1980+((ffblk.ff_fdate >> 9)&127);
      Month = (ffblk.ff_fdate >> 5)&15;
      Day = (ffblk.ff_fdate)&31;
      Hour = (ffblk.ff_ftime>>11)&31;
      Min = (ffblk.ff_ftime>>5)&63;
      Sec = 2*((ffblk.ff_ftime)&31);

      LastTime = TDateTime::TDateTime(Hour, Min, Sec, 0);
      LastDate = TDateTime::TDateTime(Year, Month, Day)+(LastTime-(int)LastTime);

      if (CurrentDate > LastDate + nDaysBefore)
      {
         if(nLogSaveMode==1)
            strFName.sprintf("%s%s%s", strOHTPath,STR_LOGFILE_PATH,ffblk.ff_name);
         else if(nLogSaveMode==2)
            strFName.sprintf("%s%s", strLogPath,ffblk.ff_name);

         try
         {
            bR=DeleteFile(strFName);
            if(bR==false) //읽기전용일 경우
            {
			   SetFileAttributesA(strFName.c_str(), FILE_ATTRIBUTE_NORMAL);
               DeleteFile(strFName);
            }
         }
         catch (Exception &e)
		 {
		 }
      }
      done = findnext(&ffblk);
      Sleep(10);
   }

}

