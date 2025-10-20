// ---------------------------------------------------------------------------

#pragma hdrstop

#include "Sysinfo.h"

// ---------------------------------------------------------------------------
/**
 @brief   Get_IPCSysInfo 함수
 @author  puting
 @date    2013.03.28
 @param   m_TempIPCSysInfo : data를 얻기 위한 함수
 @return  정상이면 true, 에러 발생 시 false 반환
 */
// ---------------------------------------------------------------------------
int Get_IsLogDriveAlive(TCHAR *sysinfo)
{

    int DrvNo;
    TCHAR DrvRoot[4] = L"A:\\";
    int Drv;
    TCHAR VolName[MAX_PATH] =
    {0, }, FileSysName[10] =
    {0, };
    DWORD VolSerial;
    int nDrvType;
    bool bFind = false;
    String strFileFormat = "";

    DWORD Drives = GetLogicalDrives();
    for(Drv = 0; Drv < 26; Drv++)
    {
        if((Drives >> Drv) & 0x00000001)
        {
            if((TCHAR)(Drv + 'A') == sysinfo[0])
            {
                bFind = true;
                break;
            }
        }
    }

    if(bFind == false)
        return 0;

    DrvRoot[0] += Drv;
    if(!GetVolumeInformation(DrvRoot, VolName, sizeof(VolName), &VolSerial, NULL, NULL, FileSysName, sizeof(FileSysName)))
        return 0;

    strFileFormat = FileSysName;
    if((strFileFormat == "FAT32") || (strFileFormat == "NTFS"))
        return 1;
    else
        return 0;
}

// -------------------------------------------------------------------
unsigned long PreventOverflowGetTimeMs(unsigned long precalc)
{
	double tmpPrecalc = (double)precalc;
	double postcalc = (double)timeGetTime();

	if(tmpPrecalc > postcalc) // 증가시간이므로, 이전에 얻은 시간값보다 향후 얻은 시간값이 작으면 오버플로우된 상태다.
	{
		tmpPrecalc = tmpPrecalc - (double)0x100000000;
	}

	return (unsigned long)(postcalc - tmpPrecalc);
}

int Get_OHTDriveType(void)
{
    int DrvNo;
    TCHAR DrvRoot[4] = L"C:\\";
    int Drv;
    TCHAR VolName[MAX_PATH], FileSysName[10];
    DWORD VolSerial;
    int nDrvType;

    DWORD Drives = GetLogicalDrives();

    if((Drives & 0x0004) == 0)
    {
        nDrvType = NOT_EXIST_C_DRIVE;
        return nDrvType;
    }

    GetVolumeInformation(DrvRoot, VolName, sizeof(VolName), &VolSerial, NULL, NULL, FileSysName, sizeof(FileSysName));
    switch(GetDriveType(DrvRoot))
    {
    case 0:
    case 1:
    case DRIVE_FIXED:
        nDrvType = FIXE_DISK;
        break;
    case DRIVE_REMOTE:
    case DRIVE_CDROM:
    case DRIVE_RAMDISK:
        nDrvType = NOT_CFCARD_TYPE;
        break;
    case DRIVE_REMOVABLE:
        nDrvType = REMOVABLE_DISK;
        break;
    }
    return nDrvType;
}

String Get_FileAccessTime(String strFileName)
{
    struct _wffblk wffblk;
    int done;
    TDateTime LastDate, LastTime, CurrentDate;
    int Year, Month, Day, Hour, Min, Sec;
    TCHAR FileName[1024];
    String strFName, strTmpFullName;

    CurrentDate = Now();

    strFName = strFileName;

    done = _wfindfirst(strFName.w_str(), &wffblk, 0);
    if(!done)
    {
        Year = 1980 + ((wffblk.ff_fdate >> 9) & 127);
        Month = (wffblk.ff_fdate >> 5) & 15;
        Day = (wffblk.ff_fdate) & 31;
        Hour = (wffblk.ff_ftime >> 11) & 31;
        Min = (wffblk.ff_ftime >> 5) & 63;
        // Sec = 2*((wffblk.ff_ftime)&31);
        strTmpFullName.sprintf(L"%04d%02d%02d%02d%02d", Year, Month, Day, Hour, Min);
    }
    else
        strTmpFullName.sprintf(L"%04d%02d%02d%02d%02d", 0, 0, 0, 0, 0);

    return strTmpFullName;
}

// wchar를 char로 컨버트
char* WCharToChar(const wchar_t* pwstrSrc)
{
#if !defined _DEBUG
    int len = 0;
    len = (wcslen(pwstrSrc) + 1) * 2;
    char* pstr = (char*) malloc(sizeof(char) * len);

    WideCharToMultiByte(949, 0, pwstrSrc, -1, pstr, len, NULL, NULL);
#else

    int nLen = wcslen(pwstrSrc);

    char* pstr = (char*) malloc(sizeof(char) * nLen + 1);
    wcstombs(pstr, pwstrSrc, nLen + 1);
#endif

    return pstr;
}

// char를 wchar로 컨버트
wchar_t* CharToWChar(const char* pstrSrc)
{
    int nLen = strlen(pstrSrc) + 1;
    wchar_t* pwstr = (LPWSTR) malloc(sizeof(wchar_t) * nLen);
    mbstowcs(pwstr, pstrSrc, nLen);
    return pwstr;
}

#pragma package(smart_init)
