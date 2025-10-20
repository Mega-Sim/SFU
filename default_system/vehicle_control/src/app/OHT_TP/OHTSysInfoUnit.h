//---------------------------------------------------------------------------

#ifndef OHTSysInfoUnitH
#define OHTSysInfoUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Menus.hpp>

#include "../Common/Def_DataType.h"
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TSysInfoForm : public TForm
{
__published:	// IDE-managed Components
    TPanel *panMain;
    TGroupBox *grpTimeInformation;
	TLabel *lbOHT;
   TLabel *labCapacity1;
    TGroupBox *grpOHTDiskInformation;
	TLabel *lbUsed;
	TLabel *lbFree;
	TLabel *lnTP;
	TButton *btnChangeOHTTime;
   TLabel *labOHTTime;
   TLabel *labTPTime;
   TMainMenu *MainMenu1;
   TMenuItem *Update1;
    TButton *btnReboot;
	TLabel *lbTotal;
	TTimer *tmrTime;
	TLabel *lbOHTtime;
	TLabel *lbTPtime;
	TLabel *lbTotalSpace;
	TLabel *lbUsedSpace;
	TLabel *lbFreeSpace;
    TChart *chartDisk_System;
	TPieSeries *psrsDiskSpace;
    TPanel *panDisk_System;
    TGroupBox *grpDisk_System;
    TGroupBox *grpDisk_Log;
    TChart *chartDisk_Log;
	TPieSeries *PieSeries1;
    TPanel *panDisk_Log;
	TLabel *Label1;
	TLabel *lbLogFreeSpace;
	TLabel *Label3;
	TLabel *lbLogTotalSpace;
	TLabel *Label5;
	TLabel *lbLogUsedSpace;
   void __fastcall Update1Click(TObject *Sender);
   void __fastcall btnChangeOHTTimeClick(TObject *Sender);
   void __fastcall btnRebootClick(TObject *Sender);
	void __fastcall tmrTimeTimer(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
	typedef struct TIME_VALUE_
	{
		int year;
		int month;
		int day;
		int hour;
		int min;
		int sec;
	}TIME_VALUE;

	TIME_VALUE ohttime;
   DISK_INFO SystemDiskInfo;
   DISK_INFO LogDiskInfo;
   int IsExistLogDisk;
public:		// User declarations
   __fastcall TSysInfoForm(TComponent* Owner);
   void Analysis_OHTSysInfo(String);
   void SetOHTAdditionalInfo(OHT_ADDITIONAL_INFO *info);
   void SetOHTCurrentTime(String strTime);
   TCHAR* ConvertDiskFormattedSize(UINT64 size);
   void SetDiskInfo();
   void SetOHTCurrentVersion(OHT_VERSION &version);
};
//---------------------------------------------------------------------------
extern PACKAGE TSysInfoForm *SysInfoForm;
//---------------------------------------------------------------------------
#endif
