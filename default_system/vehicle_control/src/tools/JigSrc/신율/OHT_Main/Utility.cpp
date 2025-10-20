//---------------------------------------------------------------------------
#include <vcl.h>
#include <math.h>
#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include <Filectrl.hpp>
#include <mmsystem.h>
#pragma hdrstop

#include "Def_OHT.h"
#include "Utility.h"

#define DRIVE_A		0
#define DRIVE_B		1
#define DRIVE_C		2
#define DRIVE_D		3


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
OHTUTILITY::OHTUTILITY()
{
}

//---------------------------------------------------------------------------
OHTUTILITY::~OHTUTILITY()
{
}


/**
@brief   D drive 존재 유무 확인
@author  ks4423
@date    2014.04.14
@return  D dirve 존재할 경우 true, 아닐 경우 false
*/
bool OHTUTILITY::Check_DDrive()
{
	DWORD Drives = GetLogicalDrives();

	if((Drives >> DRIVE_D) & 0x00000001)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
@brief   D drive내에 Log folder 존재 유무 확인
@author  ks4423
@date    2014.04.14
@return  존재할 경우 true, 아닐 경우 false
*/
bool OHTUTILITY::Check_LogFolder()
{
	bool IsLogFolderExist = false;

	if(DirectoryExists("D:\\Log"))
	{
		IsLogFolderExist = true;
	}
	else
	{
		try
		{
			if(CreateDir("D:\\Log"))
			{
				IsLogFolderExist = true;
			}
			else
			{
				IsLogFolderExist = false;
			}
		}
		catch(...)
		{
		}
	}

	return IsLogFolderExist;
}



//---------------------------------------------------------------------------
int OHTUTILITY::DeleteFile_SizeZero(AnsiString strFileN)
{
   int nError=0;

   TSearchRec sr;

   if(FindFirst(strFileN, faArchive, sr) == 0) //파일이 존재하면
   {
      if(sr.Size == 0)    //파일의 크기가 zero이면 삭제..
      {
         try
         {
            DeleteFile(strFileN);
         }
         catch (...)
         {
            // by zzang9un 2012.11.15 : 주석 처리
//	      	nError = ERR_WINDOWEXCEPTION;
         }
      }
   }
   FindClose(sr);

   return nError;
}

//---------------------------------------------------------------------------
int OHTUTILITY::File_Delete_All(AnsiString strTmpPath)
{
   int nError=0;
   TSearchRec sr;
   AnsiString strTmpFileName = "";

   if(FindFirst(strTmpPath, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
   {
    //디렉토리를 제외하고 파일만을 검색한다
      if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
      {
      //검색한 내용이 파일이면
         if(!((sr.Name == ".") || (sr.Name == "..")))
         {
            strTmpFileName.sprintf("%s\\%s",ExtractFileDir(strTmpPath),sr.Name);
            DeleteFile(strTmpFileName);
         }
      }
      while(FindNext(sr) == 0)
      {
         if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
         {
            if((sr.Name == ".") || (sr.Name == "..")) continue;
            strTmpFileName.sprintf("%s\\%s",ExtractFileDir(strTmpPath),sr.Name);
            DeleteFile(strTmpFileName);
         }
      }
   }
   FindClose(sr);

   return nError;
}
//---------------------------------------------------------------------------
int OHTUTILITY::GetNum_LogFile(void)
{
   TSearchRec sr;
   int nCount=0;
   AnsiString strTmpPath="", strOHTPath="";
   struct date today;
   struct time currenttime;
   strOHTPath=ExtractFileDir(Application->ExeName);
   getdate(&today);
   gettime(&currenttime);

   // by zzang9un 2012.11.15 : 주석 처리
//   strTmpPath.sprintf("%s*%04d%02d%02d%02d.log",gm_OHTTotalData->gm_ParameterData->strLogPath,today.da_year,today.da_mon,today.da_day,currenttime.ti_hour);

   if(FindFirst(strTmpPath, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
   {
    //디렉토리를 제외하고 파일만을 검색한다
      if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
      {
      //검색한 내용이 파일이면

         if(!((sr.Name == ".") || (sr.Name == "..")))
         {
            nCount++;
         }
      }
      while(FindNext(sr) == 0)
      {
         if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
         {
            if((sr.Name == ".") || (sr.Name == "..")) continue;
            nCount++;
         }
      }
   }
   FindClose(sr);

   return nCount;
}
//---------------------------------------------------------------------------
void OHTUTILITY::Check_DeleteLogFile(AnsiString strTmpFile, int nNowData)
{
   int nPos = 0, nPreData = 0;
   AnsiString strTmpS = "";
   AnsiString strOHTPath=".";

   nPos = strTmpFile.Pos(".log");
   strTmpS=strTmpFile.SubString(nPos-10,10);
   nPreData=strTmpS.ToInt();

   strOHTPath=ExtractFileDir(Application->ExeName);

   if(nPreData<nNowData)
   {
        // by zzang9un 2012.11.15 : 주석 처리
//      if(gm_OHTTotalData->gm_ParameterData->nIs_LogSaveMode==LOGMODE_LOCAL)
//         DeleteFile(strOHTPath+"\\Log\\"+strTmpFile);
//      else if(gm_OHTTotalData->gm_ParameterData->nIs_LogSaveMode==LOGMODE_DRIVE)
//         DeleteFile(gm_OHTTotalData->gm_ParameterData->strLogPath+strTmpFile);
   }
}
//---------------------------------------------------------------------------
// by zzang9un 2012.11.15 : 주석 처리
//
//void OHTUTILITY::CheckDirectory()
//{
//   AnsiString strOHTPath=".";
//
//   strOHTPath=ExtractFileDir(Application->ExeName);
//   if (!DirectoryExists(strOHTPath+STR_DATAFILE_PATH))
//   {
//      CreateDir(strOHTPath+STR_DATAFILE_PATH);
//   }
//
//   if (!DirectoryExists(strOHTPath+STR_AMCFILE_PATH))
//   {
//      CreateDir(strOHTPath+STR_AMCFILE_PATH);
//   }
//
//   if (!DirectoryExists(strOHTPath+STR_UPDATE_PATH))
//   {
//      CreateDir(strOHTPath+STR_UPDATE_PATH);
//   }
//   else
//   {
//      File_Delete_All(strOHTPath+STR_UPDATE_PATH+"*.*");
//   }
//
//   if(!DirectoryExists(strOHTPath+STR_LOGFILE_PATH))
//   {
//      CreateDir(strOHTPath+STR_LOGFILE_PATH);
//   }
//
//}

//---------------------------------------------------------------------------
AnsiString OHTUTILITY::Get_FileAccessTime(AnsiString strFileName)
{
   TSearchRec sr;
   TDateTime fDate;
   AnsiString strDate="";
   strDate.sprintf("%04d%02d%02d%02d%02d",0,0,0,0,0);

   if(FindFirst(strFileName, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
   {
    //디렉토리를 제외하고 파일만을 검색한다
      if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
      {
      //검색한 내용이 파일이면
         if(!((sr.Name == ".") || (sr.Name == "..")))
         {
            fDate = FileDateToDateTime(sr.Time);
            strDate = FormatDateTime("yyyymmddhhmm",fDate);
         }
      }
   }
   FindClose(sr);

   return strDate;
}
//---------------------------------------------------------------------------
int OHTUTILITY::Get_IsLogDriveAlive(AnsiString strDrvName)
{
   char DrvRoot[4] = "A:\\";
   int Drv = 0;
   char VolName[MAX_PATH] = {0, }, FileSysName[10] = {0, };
   DWORD VolSerial = 0;
   bool bFind=false;
   AnsiString strTmpPath = "";
   AnsiString strFileFormat="";

   int nPos = strDrvName.Pos(":");
   if(nPos==0) return 0;

   strTmpPath = strDrvName.SubString(1, nPos-1);

   DWORD Drives = GetLogicalDrives();
   for(Drv=0; Drv<26; Drv++)
   {
      if((Drives >> Drv) & 0x00000001)
      {
         if((char)(Drv + 'A') == strTmpPath[1])
         {
            bFind=true;
            break;
         }
      }
   }

   if(bFind==false) return 0;

   DrvRoot[0] += Drv;

   // by zzang9un 2012.11.15 : 주석 처리
//    GetVolumeInformation(DrvRoot, VolName, sizeof(VolName), &VolSerial, NULL, NULL, FileSysName, sizeof(FileSysName));

   strFileFormat=FileSysName;
   if((strFileFormat=="FAT32")||(strFileFormat=="NTFS")) return 1;
   else return 0;

}
//---------------------------------------------------------------------------
int OHTUTILITY::Check_VirusDirectory(void)
{
   int nError=0;
   AnsiString strOHTPath=".";
   strOHTPath=ExtractFileDir(Application->ExeName);
   AnsiString strPath="";

	strPath.sprintf("%s%s",strOHTPath,STR_VIRUS_PATH);
	if(DirectoryExists(strPath))
	{
		nError = 0x1;
	}

	strPath.sprintf("%s%s",STR_LOG_PATH,STR_VIRUS_PATH);
	if(DirectoryExists(strPath))
	{
		nError = nError | 0x2;
	}

   return nError;

}
unsigned long OHTUTILITY::PreventOverflowGetTimeMs(unsigned long precalc)
{
	double tmpPrecalc = (double)precalc;
	double postcalc = (double)timeGetTime();

	if(tmpPrecalc > postcalc) // 증가시간이므로, 이전에 얻은 시간값보다 향후 얻은 시간값이 작으면 오버플로우된 상태다.
	{
		tmpPrecalc = tmpPrecalc - (double)0x100000000;
	}

	return (unsigned long)(postcalc - tmpPrecalc);
}

int OHTUTILITY::GetDiskInfo(DISK_INFO* pDiskInfo, char cDrive)
{
	if(pDiskInfo == NULL)
	{
		return -1;
	}

	memset(pDiskInfo, 0, sizeof(DISK_INFO));

	pDiskInfo->Drive = cDrive;
	pDiskInfo->TotalSpace = DiskSize(cDrive - 0x40);

	//디스크 크기가 -1 일경우 드라이브 에러
	if(pDiskInfo->TotalSpace == -1)
	{
		memset(pDiskInfo, 0, sizeof(DISK_INFO));
		return -1;
	}

	pDiskInfo->FreeSpace = DiskFree(cDrive - 0x40);
	pDiskInfo->UsedSpace = pDiskInfo->TotalSpace - pDiskInfo->FreeSpace;

	return NO_ERR;
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
