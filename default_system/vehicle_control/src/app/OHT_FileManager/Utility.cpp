// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Utility.h"
#include <stdio.h>
#include <dos.h>
#include <Filectrl.hpp>
#define DRIVE_D		3
// ---------------------------------------------------------------------------
OHTLogFile::OHTLogFile()
{
}

// ---------------------------------------------------------------------------
OHTLogFile::~OHTLogFile()
{
}

// ---------------------------------------------------------------------------
bool OHTLogFile::WriteLogFile(String strTmpFileName, String strTmpLogData)
{
    String strTmpFullName, strTPPath;
    FILE *fp;
    TDateTime CurTime = Now();
    WORD year, month, day, hour, min, second, msec;

    DecodeDate(CurTime, year, month, day);
    DecodeTime(CurTime, hour, min, second, msec);

    strTPPath = ExtractFileDir(Application->ExeName);
	// 화일이름을 저장

	//디렉토리 확인
	CheckDirectory();

    strTmpFullName.sprintf(L"%s\\Log\\%s%04d%02d%02d%02d.log", strTPPath, strTmpFileName, year, month, day, hour);
   // if(fp != NULL) fclose(fp);
    fp = _wfopen(strTmpFullName.w_str(), L"a");

    if(fp != NULL)
    {
        try
        {
            fwprintf(fp, L"[%02d:%02d:%02d:%03d]%s\n", hour, min, second, msec, strTmpLogData);
            fflush(fp);
        }
        catch(Exception &e)
        {
            fclose(fp);
            fp = NULL;
            return false;
        }
        fclose(fp);
        return true;
    }
    fclose(fp);
    return false;
}
bool OHTLogFile::WriteLogFileAtD(String strTmpFileName, String strTmpLogData, String Path)
{
    String strTmpFullName, strTPPath;
    FILE *fp;
    TDateTime CurTime = Now();
    WORD year, month, day, hour, min, second, msec;

    DecodeDate(CurTime, year, month, day);
    DecodeTime(CurTime, hour, min, second, msec);

	//strTPPath = ExtractFileDir(Application->ExeName);
    strTPPath = Path;
	// 화일이름을 저장

	//디렉토리 확인
	CheckDirectory();

	strTmpFullName.sprintf(L"%s\\Log\\%s%04d%02d%02d%02d.log", strTPPath, strTmpFileName, year, month, day, hour);
   // if(fp != NULL) fclose(fp);
    fp = _wfopen(strTmpFullName.w_str(), L"a");

    if(fp != NULL)
    {
        try
        {
            fwprintf(fp, L"[%02d:%02d:%02d:%03d]%s\n", hour, min, second, msec, strTmpLogData);
            fflush(fp);
        }
        catch(Exception &e)
        {
            fclose(fp);
            fp = NULL;
            return false;
        }
        fclose(fp);
        return true;
    }
    fclose(fp);
    return false;
}
// ---------------------------------------------------------------------------
// 2002.11.27 JYB
// 현재날짜/시간비교하여 하루전 같은 시간대의 Log File을 Check하여 Log File지움..
// ---------------------------------------------------------------------------
void OHTLogFile::DeleteLogFile()
{
    AnsiString strFullLogFile, strMonDay;
    bool bYun;
    struct date today;
    struct time currenttime;
    int nYear, nMon, nDay;
    int nSData;

    getdate(&today);
    gettime(&currenttime);

    strMonDay.sprintf("%02d%02d", today.da_mon, today.da_day);

    if(strMonDay == "0101")// 1월1일이면 이전년도 12월31일 LogFile찾음..
    {
        nYear = today.da_year - 1;
        nMon = 12;
        nDay = 31;
    }
    else if(strMonDay == "0301")
    {
        nYear = today.da_year;
        nMon = 02;
      // 윤년계산
        if(!(today.da_year % 4) && today.da_year % 100 || !(today.da_year % 400))
            nDay = 29;
        else
            nDay = 28;
    }
    else if((strMonDay == "0201") || (strMonDay == "0401") || (strMonDay == "0601") || (strMonDay == "0801") ||
        (strMonDay == "0901") || (strMonDay == "1101"))
    {
        nYear = today.da_year;
        nMon = today.da_mon - 1;
        nDay = 31;
    }
    else if((strMonDay == "0501") || (strMonDay == "0701") || (strMonDay == "1001") || (strMonDay == "1201"))
    {
        nYear = today.da_year;
        nMon = today.da_mon - 1;
        nDay = 30;
    }
    else
    {
        nYear = today.da_year;
        nMon = today.da_mon;
        nDay = today.da_day - 1;
    }
    strFullLogFile.sprintf("%04d%02d%02d%02d", nYear, nMon, nDay, currenttime.ti_hour);
    nSData = strFullLogFile.ToInt();

    FileExistCheck(".\\Log\\*.log", nSData);
}

// ---------------------------------------------------------------------------
void OHTLogFile::FileExistCheck(AnsiString strTmpPath, int nNowData)
{
    TSearchRec sr;
    AnsiString strTmpFileName;

    if(FindFirst(strTmpPath, faArchive | faReadOnly | faSysFile | faHidden, sr) == 0)
    {
    // 디렉토리를 제외하고 파일만을 검색한다
        if(sr.Attr == faArchive || faReadOnly || faSysFile || faHidden)
        {
      // 검색한 내용이 파일이면
            if(!((sr.Name == ".") || (sr.Name == "..")))
            {
                strTmpFileName = sr.Name;
                Check_DeleteLogFile(strTmpFileName, nNowData);
            }
        }
        while(FindNext(sr) == 0)
        {
            if(sr.Attr == faArchive || faReadOnly || faSysFile || faHidden)
            {
                if((sr.Name == ".") || (sr.Name == ".."))
                    continue;
                strTmpFileName = sr.Name;
                Check_DeleteLogFile(strTmpFileName, nNowData);
            }
        }
    }
    FindClose(sr);
}

// ---------------------------------------------------------------------------
void OHTLogFile::Check_DeleteLogFile(AnsiString strTmpFile, int nNowData)
{
    int nPos, nPreData;
    AnsiString strTmpS;

    nPos = strTmpFile.Pos(".log");
    strTmpS = strTmpFile.SubString(nPos - 10, 10);
    nPreData = strTmpS.ToInt();

    if(nPreData < nNowData)
        DeleteFile(".\\Log\\" + strTmpFile);
}

// ---------------------------------------------------------------------------
void OHTLogFile::CheckDirectory()
{
    AnsiString strTPPath = ExtractFileDir(Application->ExeName);
    AnsiString strLogPath;

    strLogPath.sprintf("%s\\Log", strTPPath);

    if(!DirectoryExists(strLogPath))
    {
        CreateDir(strLogPath);
    }
}

// ---------------------------------------------------------------------------
void Snooze(float numSec)
{
    MSG msg;
    DWORD timeout = (numSec * 1000UL) + GetCurrentTime();
    while(GetCurrentTime() < timeout)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                PostQuitMessage(0);
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
// ---------------------------------------------------------------------------

__int64 __fastcall GetLocalFileSize(String FileName)
{
    DWORD dwLow;
    DWORD dwHigh = 0;
    __int64 FileSize = 0;
    
    HANDLE hFile = ::CreateFile( FileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL );

    if(hFile != INVALID_HANDLE_VALUE)
    {
        dwLow = ::GetFileSize( hFile, &dwHigh );
        
        if((dwLow != 0xFFFFFFFF) || GetLastError() == NO_ERROR)
            FileSize = (__int64)dwHigh << 32 | dwLow;

        ::CloseHandle( hFile );
    }
    
    return FileSize;
}
// ---------------------------------------------------------------------------

#pragma package(smart_init)
