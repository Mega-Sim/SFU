//---------------------------------------------------------------------------
/**
@file    OHTSysinfo.cpp
$Rev:: 602           $:  Revision of last commit
$Author:: puting     $:  Author of last commit
$Date:: 2013-03-28 0#$:  Date of last commit
@version OHT
@brief   OHTSysinfo Class CPP File
*/
#include "OHTSysInfo.h"

#define  ERRORGetVolumeInformation 7

 /**
@brief   Get_IPCSysInfo 함수
@author  puting
@date    2013.03.28
@param   m_TempIPCSysInfo : data를 얻기 위한 함수
@return  정상이면 true, 에러 발생 시 false 반환
*/
//---------------------------------------------------------------------------
int Get_IPCSysInfo(IPC_SYS_INFO* m_TempIPCSysInfo)
{
	char DrvRoot[4] = "A:\\";
	int Drv = 0;
	int tmep = 0;
	char VolName[MAX_PATH]={0,}, FileSysName[10]={0,};
	DWORD VolSerial;

	struct date today;
	struct time currenttime;

	//드라이브 정보 얻기(모니터링 항목)
	DWORD Drives = GetLogicalDrives();
	for(Drv=0; Drv<26; Drv++)
	{
	  if((Drives >> Drv) & 0x00000001)
      {
		 if((char)(Drv + 'A') == 'C')
		 {
			m_TempIPCSysInfo->nfreeSpace = DiskFree(Drv+1) >> 10; //여유공간
			m_TempIPCSysInfo->nTotalSpace = DiskSize(Drv+1) >> 10; //전체공간
            break;
         }
	  }
	}
	DrvRoot[0] += Drv;
	GetVolumeInformationA(DrvRoot, VolName, sizeof(VolName), &VolSerial, NULL, NULL, FileSysName, sizeof(FileSysName));

	//Drive Type
	switch(GetDriveTypeA(DrvRoot))
	{
	  case 0: 				   m_TempIPCSysInfo->DrvType = 0x00;		break;
	  case 1:                  m_TempIPCSysInfo->DrvType = 0x01;		break;
	  case DRIVE_REMOVABLE:    m_TempIPCSysInfo->DrvType = 0x02;		break;
	  case DRIVE_FIXED:        m_TempIPCSysInfo->DrvType = 0x03;		break;
	  case DRIVE_REMOTE:       m_TempIPCSysInfo->DrvType = 0x04;		break;
	  case DRIVE_CDROM:        m_TempIPCSysInfo->DrvType = 0x05;		break;
	  case DRIVE_RAMDISK:      m_TempIPCSysInfo->DrvType = 0x06;		break;
	}

	//File System Name
	strcpy(m_TempIPCSysInfo->szFileSysName, FileSysName);

    //시간정보 얻기(모니터링 항목)
	getdate(&today);
	gettime(&currenttime);

	m_TempIPCSysInfo->szOHTSysTime[0] = (today.da_year /1000)+ 0x30;  //2013
	m_TempIPCSysInfo->szOHTSysTime[1] = ((today.da_year /100)%10)+ 0x30;  //2013
	m_TempIPCSysInfo->szOHTSysTime[2] = ((today.da_year /10)%10)+ 0x30;  //2013
	m_TempIPCSysInfo->szOHTSysTime[3] = (today.da_year % 10)+ 0x30 ;

	//OHT Time Stamp Month data
	tmep = today.da_mon;
	m_TempIPCSysInfo->szOHTSysTime[4] = (tmep / 10) + 0x30;
	m_TempIPCSysInfo->szOHTSysTime[5] = (tmep % 10) +0x30;

	//OHT Time Stamp Date Data
	tmep = today.da_day;
	m_TempIPCSysInfo->szOHTSysTime[6] = (tmep / 10) +0x30;
	m_TempIPCSysInfo->szOHTSysTime[7] = (tmep % 10) +0x30;

	//OHT Time Stamp Hour Data
	tmep = currenttime.ti_hour;
	m_TempIPCSysInfo->szOHTSysTime[8] = (tmep / 10) +0x30;
	m_TempIPCSysInfo->szOHTSysTime[9] = (tmep % 10) +0x30;

	//OHT Time Stamp Minute Data
	tmep = currenttime.ti_min;
	m_TempIPCSysInfo->szOHTSysTime[10] = (tmep / 10) +0x30;
	m_TempIPCSysInfo->szOHTSysTime[11] = (tmep % 10) +0x30;

	//OHT Time Stamp Second Data
	tmep = currenttime.ti_sec;
	m_TempIPCSysInfo->szOHTSysTime[12] = (tmep / 10) +0x30;
	m_TempIPCSysInfo->szOHTSysTime[13] = (tmep % 10) +0x30;

	return NO_ERR;
}


#pragma package(smart_init)
