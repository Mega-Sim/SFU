// ---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <dos.h>
#include <Filectrl.hpp>
#pragma hdrstop

#include "LogDownUnit.h"
#include "FileUnit.h"
#include "LogFileUnit.h"
#include "Define.h"
#include "ConnectUnit.h"
#include "Def_TP.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLogDownForm *LogDownForm;

// ---------------------------------------------------------------------------
__fastcall TLogDownForm::TLogDownForm(TComponent* Owner) : TForm(Owner)
{
    Label1->Caption = "Mt";
    Label2->Caption = "Day";
    Label3->Caption = "Hr";

    bIsOHTExecLogSave = true;
}

// ---------------------------------------------------------------------------
void __fastcall TLogDownForm::btnDownloadStartClick(TObject *Sender)
{
    struct date today;
    struct time currenttime;
    String strToday, strFDay;
    int nM, nD, nT;

    getdate(&today);
    gettime(&currenttime);

    switch(rgLogSelect->ItemIndex)
    {
    case 0:
        // if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
        if(bIsOHTExecLogSave == false)
            LogFileForm->FileDownStart(DATAFILE_LOG, "*.log");
        else
            FileForm->FileDownStart(DATAFILE_LOG, "*.log");
        break;
        
    case 1:
        strToday.sprintf(L"*%04d%02d%02d*.log", today.da_year, today.da_mon, today.da_day);
        // if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
        if(bIsOHTExecLogSave == false)
            LogFileForm->FileDownStart(DATAFILE_LOG, strToday);
        else
            FileForm->FileDownStart(DATAFILE_LOG, strToday);
        break;
        
    case 2:
        nM = StrToIntDef(edtMonth->Text, 0);
        nD = StrToIntDef(edtDay->Text, 0);
        nT = StrToIntDef(edtHour->Text, 0);
        if(nM != 0 && nD != 0)
        {
            strFDay.sprintf(L"*%04d%02d%02d%02d*.log", today.da_year, nM, nD, nT);
            // if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
            if(bIsOHTExecLogSave == false)
                LogFileForm->FileDownStart(DATAFILE_LOG, strFDay);
            else
                FileForm->FileDownStart(DATAFILE_LOG, strFDay);
        }
        break;
        
    case 3: // 원하는 시간대를 골라서 다운로드 받는 경우        
        LogDown_SelectTime();
        break;
    }
    Close();
}

// ---------------------------------------------------------------------------

int TLogDownForm::LogDown_SelectTime(void)
{
    int nMonth, nDay;
    int nError = NO_ERR;

    struct date today;
    struct time currenttime;

    getdate(&today);
    gettime(&currenttime);

    nMonth = StrToIntDef(edtSpecifyMonth->Text, 0);
    nDay = StrToIntDef(edtSpecifyDay->Text, 0);

    TCheckBox *chkTemp;
    String strChkHour;
    String strFileName;

    for(int i = 0; i < 24; i++)
    {
        strChkHour = "chkHour" + IntToStr(i);
        chkTemp = (TCheckBox*)this->FindComponent(strChkHour);

        if(chkTemp->Checked)
        {
            strFileName.sprintf(L"*%04d%02d%02d%02d*.log", today.da_year, nMonth, nDay, i);

            if(bIsOHTExecLogSave == false)
                LogFileForm->FileDownStart(DATAFILE_LOG, strFileName);
            else
                FileForm->FileDownStart(DATAFILE_LOG, strFileName);

            if((LogFileForm->bFileCancel == true) || (FileForm->bFileCancel == true))
                return nError;
        }

    }

    return nError;

}

// ---------------------------------------------------------------------------
void __fastcall TLogDownForm::btnDownloadCancelClick(TObject *Sender)
{
    // if(gm_OHTStatus->g_nIsOHTBoot==FROMCFC)
    if(bIsOHTExecLogSave == false)
        LogFileForm->bFileCancel = true;
    else
        FileForm->bFileCancel = true;
    Close();
}
// ---------------------------------------------------------------------------

void __fastcall TLogDownForm::edtMonthMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    rgLogSelect->ItemIndex = 2;
}
// ---------------------------------------------------------------------------

void __fastcall TLogDownForm::edtDayMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    rgLogSelect->ItemIndex = 2;
}
// ---------------------------------------------------------------------------

void __fastcall TLogDownForm::edtHourMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    rgLogSelect->ItemIndex = 2;
}
// ---------------------------------------------------------------------------
