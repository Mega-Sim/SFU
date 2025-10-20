// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OHTSysInfoUnit.h"
#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include <Filectrl.hpp>
#include "OHTSysInfoUnit.h"
#include "Def_TPCommand.h"
#include "CMDControlUnit.h"
#include "RebootUnit.h"
#include "MainUnit.h"
#include "OHTInfoUnit.h"
#include "Define.h"
#include "LogFileUnit.h"
#include "StatusUnit.h"
#include <shlwapi.h>
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#pragma link "shlwapi.lib"
TSysInfoForm *SysInfoForm;

// ---------------------------------------------------------------------------
__fastcall TSysInfoForm::TSysInfoForm(TComponent *Owner)
    :
    TForm(Owner)
{
}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
void TSysInfoForm::Analysis_OHTSysInfo(String strOHTSysInfo)
{
//    String strDrvType, strFileSys, strFree, strTotal, strTime, strOHT, strMap, strLoc;
//    String strPLC;
//    String strTmpFree, strTmpUse;
//    int nPos;
//    int i;
//    int nPos1;
//    String strOHTVer = "", strAMCVer = "";
//
//    nPos             = strOHTSysInfo.Pos("/");
//    strDrvType       = strOHTSysInfo.SubString(1, nPos - 1);
//    labType->Caption = strDrvType;
//
//    if(strDrvType.Pos("Hard") != 0)
//    {
//        // MainForm->panFrom->Caption="HDD";
//        gm_OHTStatus->g_nIsOHTBoot = FROMHDD;
//    }
//    else
//    {
//        // MainForm->panFrom->Caption="CFC";
//        gm_OHTStatus->g_nIsOHTBoot = FROMCFC;
//    }
//
//    strOHTSysInfo.Delete(1, nPos);
//
//    nPos                = strOHTSysInfo.Pos("/");
//    strFileSys          = strOHTSysInfo.SubString(1, nPos - 1);
//    labFileSys->Caption = strFileSys;
//    strOHTSysInfo.Delete(1, nPos);
//
//    nPos    = strOHTSysInfo.Pos("/");
//    strFree = strOHTSysInfo.SubString(1, nPos - 1);
//    strOHTSysInfo.Delete(1, nPos);
//
//    nPos     = strOHTSysInfo.Pos("/");
//    strTotal = strOHTSysInfo.SubString(1, nPos - 1);
//    strOHTSysInfo.Delete(1, nPos);
//
//	labUsedSpace->Caption = IntToStr(StrToInt(strTotal.c_str()) - StrToInt(strFree.c_str())) + " KB"; ;
//    labFreeSpace->Caption = strFree + " KB"; ;
//    labCapacity->Caption  = strTotal + " KB"; ;
//    // chart
//    strTmpFree.sprintf(L"%d", (int)(StrToFloat(strFree) *100.0 / StrToFloat(strTotal)));
//    strTmpUse.sprintf(L"%d", (int)((StrToFloat(strTotal) - StrToFloat(strFree)) *100.0 / StrToFloat(strTotal)));
//
//    // OHT Time
//    nPos    = strOHTSysInfo.Pos("/");
//    strTime = strOHTSysInfo.SubString(1, nPos - 1);
//    strTime.Insert(".", 5);
//    strTime.Insert(".", 8);
//    strTime.Insert(". ", 11);
//    strTime.Insert(":", 15);
//    labOHTTime->Caption              = strTime;
//    LogFileForm->lblOHTTime->Caption = strTime;
//    strOHTSysInfo.Delete(1, nPos);
//
//    // TP Time
//    struct date today;
//    struct time currenttime;
//    String strTmpFullName;
//
//    getdate(&today);
//    gettime(&currenttime);
//
//    strTmpFullName.sprintf(L"%04d.%02d.%02d. %02d:%02d", today.da_year, today.da_mon, today.da_day, currenttime.ti_hour,
//        currenttime.ti_min);
//    labTPTime->Caption              = strTmpFullName;
//    LogFileForm->lblPCTime->Caption = strTmpFullName;
//
//    // Main Ver
//    nPos = strOHTSysInfo.Pos("OHT ver");
//    if(nPos != 0)
//    {
//        nPos1     = strOHTSysInfo.Pos("/");
//        strOHTVer = strOHTSysInfo.SubString(1, nPos1 - 1);
//        strOHTSysInfo.Delete(1, nPos1);
//
//        // AMC Ver
//        nPos = strOHTSysInfo.Pos("[AMC");
//        if(nPos != 0)
//        {
//            nPos1     = strOHTSysInfo.Pos("/");
//            strAMCVer = strOHTSysInfo.SubString(1, nPos1 - 1);
//            strOHTSysInfo.Delete(1, nPos1);
//        }
//
//        MainForm->StatusBarVersionInfo->Panels->Items[0]->Text = "  " + strOHTVer + strAMCVer;
//    }

}

void __fastcall TSysInfoForm::Update1Click(TObject *Sender)
{
    uTP_CMD* data = new uTP_CMD;
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTPCmd2AST(TPCMD_OHT_ADDITIONAL_STATUS, data);
	delete m_CMDControl;
}
// ---------------------------------------------------------------------------

void __fastcall TSysInfoForm::btnChangeOHTTimeClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    struct date today;
    struct time currenttime;
    String strTmpFullName;

    uTP_CMD SndTPCmd; // TP Command
    memset(&SndTPCmd, 0x00, sizeof(uTP_CMD));

    getdate(&today);
    gettime(&currenttime);

	SndTPCmd.TPCmd.Data1 = today.da_year*10000 + today.da_mon*100 + today.da_day;
	SndTPCmd.TPCmd.Data2 = currenttime.ti_hour*10000 + currenttime.ti_min*100 + currenttime.ti_sec;

	m_CMDControl->MakeTPCmd2AST(TPCMD_OHT_TIMESETTING, &SndTPCmd);
    delete m_CMDControl;
}
// ---------------------------------------------------------------------------

void __fastcall TSysInfoForm::btnRebootClick(TObject *Sender)
{
    RebootForm->ShowModal();
}
// ---------------------------------------------------------------------------

/**
@brief		OHT의 시스템정보를 셋팅함
@author  	doori.shin
@date    	2013.12.31
@param		info : OHT 부가정보
*/
void TSysInfoForm::SetOHTAdditionalInfo(OHT_ADDITIONAL_INFO *info)
{
	memcpy(&SystemDiskInfo, &info->SystemDiskInfo, sizeof(DISK_INFO));
	memcpy(&LogDiskInfo, &info->LogDiskInfo, sizeof(DISK_INFO));

//	IsExistLogDisk = info->LogDriveStatus;
	IsExistLogDisk = (info->LogDiskInfo.TotalSpace == -1) ? 0x00 : 0x01;

	SetDiskInfo();
}

/**
@brief		Assistant로부터 받은 메인프로그램 버전을 표시
@author  	doori.shin
@date    	2013.12.31
@param		versioni : OHT 메인프로그램 버전
*/
void TSysInfoForm::SetOHTCurrentVersion(OHT_VERSION& version)
{
	String strVersion;
	strVersion.sprintf(L"%d.%d.%d", version.Version.Major, version.Version.Minor, version.Version.Build);
	MainForm->StatusBarVersionInfo->Panels->Items[0]->Text = strVersion;
}

/**
@brief		OHT시스템의 현재시간을 저장
@author  	doori.shin
@date    	2013.12.31
@param		strTime : Assistant로 부터 받은 OHT시간
*/
void TSysInfoForm::SetOHTCurrentTime(String strTime)
{
	int nY, nMon, nD, nH, nM, nS;

	ohttime.year = StrToIntDef(strTime.SubString(1, 4), 0);
	ohttime.month = StrToIntDef(strTime.SubString(5, 2), 0);
	ohttime.day = StrToIntDef(strTime.SubString(7, 2), 0);
	ohttime.hour = StrToIntDef(strTime.SubString(9, 2), 0);
	ohttime.min = StrToIntDef(strTime.SubString(11, 2), 0);
	ohttime.sec = StrToIntDef(strTime.SubString(13, 2), 0);
}


/**
@brief		타이머에서의 OHT시스템과 현재 시스템의 시간 흐름을 표시
@author  	doori.shin
@date    	2013.12.31
*/
void __fastcall TSysInfoForm::tmrTimeTimer(TObject *Sender)
{
	String strOHTTime, strTPTime;
	ohttime.sec++;

	//초
	if(ohttime.sec >= 60)
	{
		ohttime.min++;
		ohttime.sec = 0;

		//분
		if(ohttime.min >= 60)
		{
			//시간
			ohttime.hour++;
			ohttime.min = 0;

			if(ohttime.hour >= 24)
			{
				ohttime.day++;
				ohttime.hour = 0;
            }
		}
	}

	strOHTTime.sprintf(L"%04d.%02d.%02d  %02d:%02d:%02d",
					ohttime.year,
					ohttime.month,
					ohttime.day,
					ohttime.hour,
					ohttime.min,
					ohttime.sec);

	//TP를 실행한 시스템의 시간을 구함
	struct date curdate;
	struct time curtime;
	gettime(&curtime);
	getdate(&curdate);

	strTPTime.sprintf(L"%04d.%02d.%02d  %02d:%02d:%02d",
					curdate.da_year,
					curdate.da_mon,
					curdate.da_day,
					curtime.ti_hour,
					curtime.ti_min,
					curtime.ti_sec);

	lbOHTtime->Caption = strOHTTime;
	lbTPtime->Caption = strTPTime;
}
//---------------------------------------------------------------------------
/**
@brief      바이트로된 디스크 공간을 표준화 함
@author  	doori.shin
@date    	2013.12.31
@param		size : 디스크 크기
@return		표준화된 디스크 크기
*/
TCHAR* TSysInfoForm::ConvertDiskFormattedSize(UINT64 size)
{
	TCHAR* formattedSize = new TCHAR[10];
	return formattedSize = StrFormatByteSizeW(size, formattedSize, 32);
}
void __fastcall TSysInfoForm::FormShow(TObject *Sender)
{
	tmrTime->Enabled = true;
}
//---------------------------------------------------------------------------
/**
@brief		폼을 닫을때 타이머를 해제함
@author  	doori.shin
@date    	2013.12.31
*/
void __fastcall TSysInfoForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	tmrTime->Enabled = false;
}
//---------------------------------------------------------------------------
/**
@brief		OHT시스템의 디스크정보를 출력
@author  	doori.shin
@date    	2013.12.31
*/
void TSysInfoForm::SetDiskInfo()
{
	//System disk info
	lbTotalSpace->Caption = ConvertDiskFormattedSize(SystemDiskInfo.TotalSpace);
	lbFreeSpace->Caption = ConvertDiskFormattedSize(SystemDiskInfo.FreeSpace);
	lbUsedSpace->Caption = ConvertDiskFormattedSize(SystemDiskInfo.UsedSpace);

	float freeSpace, usedSpace;
	freeSpace = (float)SystemDiskInfo.FreeSpace / (float)SystemDiskInfo.TotalSpace * 100;
	usedSpace = (float)SystemDiskInfo.UsedSpace / (float)SystemDiskInfo.TotalSpace * 100;

	chartDisk_System->Series[0]->Clear();
	chartDisk_System->Series[0]->Add(freeSpace,"Free" ,clFuchsia);
	chartDisk_System->Series[0]->Add(usedSpace,"Used" ,clBlue);

	//Log disk info
	if(IsExistLogDisk != 0)
	{
		lbLogTotalSpace->Caption = ConvertDiskFormattedSize(LogDiskInfo.TotalSpace);
		lbLogFreeSpace->Caption = ConvertDiskFormattedSize(LogDiskInfo.FreeSpace);
		lbLogUsedSpace->Caption = ConvertDiskFormattedSize(LogDiskInfo.UsedSpace);

		freeSpace = (float)LogDiskInfo.FreeSpace / (float)LogDiskInfo.TotalSpace * 100;
		usedSpace = (float)LogDiskInfo.UsedSpace / (float)LogDiskInfo.TotalSpace * 100;

		chartDisk_Log->Series[0]->Clear();
		chartDisk_Log->Series[0]->Add(freeSpace,"Free" ,clFuchsia);
		chartDisk_Log->Series[0]->Add(usedSpace,"Used" ,clBlue);
	}
	else
	{
        //cannot connect with log disk....
    }
}


