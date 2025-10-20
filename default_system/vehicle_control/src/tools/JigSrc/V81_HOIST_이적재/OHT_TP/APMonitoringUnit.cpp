// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

#include "APMonitoringUnit.h"
#include "APTimeUnit.h"
#include "OHTInfoUnit.h"
#include "Def_TP.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAPMonitoringForm *APMonitoringForm;

// ---------------------------------------------------------------------------
__fastcall TAPMonitoringForm::TAPMonitoringForm(TComponent* Owner) : TForm(Owner)
{
    fTmpCount1 = 0.0;
}

// ---------------------------------------------------------------------------
void __fastcall TAPMonitoringForm::mniStartClick(TObject *Sender)
{
    APTimeForm->ShowModal();
}

// ---------------------------------------------------------------------------
void __fastcall TAPMonitoringForm::mniStopClick(TObject *Sender)
{
    timerAPSignal->Enabled = false;
}

// ---------------------------------------------------------------------------
void __fastcall TAPMonitoringForm::timerAPSignalTimer(TObject *Sender)
{
    String strHIDValue, strNode, Mac;
    int i = 0;
    static BYTE ApMac[6] = {0,};
    static int cnt = 0;
    bool bRoaming = false;

    fTmpCount1 = fTmpCount1 + 0.2;

    if(fTmpCount1 >= (float)nxTime1)
    {
        chartAPSignal->BottomAxis->Automatic = true;
        seriseAPSignal->Delete(0);
    }

    float fAPSignal = StrToFloat(gm_OHTStatus->m_OHTStatus.APSignalLevel);
    strNode.sprintf(L"N%06d", gm_OHTStatus->m_OHTStatus.Data.CurNodeID);
    seriseAPSignal->AddXY(fTmpCount1, fAPSignal, strNode, COLOR_BLACK);

    // OHT가 연결된 AP가 다른 경우 로밍으로 판단
    for(i = 0; i<6; i++)
    {
        if(ApMac[i] != gm_OHTStatus->m_OHTStatus.APMacAddress[i])
        {
            bRoaming = true;
            break;
        }
    }
    if(bRoaming)
    {
        AP_MAC->Color = clLime;
    }
    else
    {
        if(cnt > 2)
        {
            AP_MAC->Color = clYellow;
            cnt = 0;
        }
        else
            cnt++;
    }
    AP_MAC->Caption = String().sprintf(L"%02X:%02X:%02X:%02X:%02X:%02X", gm_OHTStatus->m_OHTStatus.APMacAddress[0], gm_OHTStatus->m_OHTStatus.APMacAddress[1], gm_OHTStatus->m_OHTStatus.APMacAddress[2], gm_OHTStatus->m_OHTStatus.APMacAddress[3], gm_OHTStatus->m_OHTStatus.APMacAddress[4], gm_OHTStatus->m_OHTStatus.APMacAddress[5]);
    for(i = 0; i<6; i++)
    {
        ApMac[i] = gm_OHTStatus->m_OHTStatus.APMacAddress[i];
    }
    // memset(ApMac, gm_OHTStatus->m_OHTStatus.APMacAddress, sizeof(ApMac));
}

// ---------------------------------------------------------------------------
void TAPMonitoringForm::Graph_Start1(int nTime_min1)
{
    seriseAPSignal->Clear();

    chartAPSignal->BottomAxis->Automatic = true;
    nxTime1 = nTime_min1 * 60;
    chartAPSignal->BottomAxis->Automatic = false;
    chartAPSignal->BottomAxis->Minimum = 0.0;
    chartAPSignal->BottomAxis->Maximum = (float)nxTime1;

    fTmpCount1 = 0.0;
    timerAPSignal->Enabled = true;
    AP_MAC->Color = clWhite;
}

// ---------------------------------------------------------------------------
void __fastcall TAPMonitoringForm::FormCreate(TObject *Sender)
{
    timerAPSignal->Enabled = false;
}

// ---------------------------------------------------------------------------
void __fastcall TAPMonitoringForm::FormDestroy(TObject *Sender)
{
    timerAPSignal->Enabled = false;
}

// ---------------------------------------------------------------------------
void __fastcall TAPMonitoringForm::mniCaptureClick(TObject *Sender)
{
    String strBmpName;

    if(dlgOpenFile->Execute())
    {
        strBmpName = dlgOpenFile->FileName;
        if((strBmpName.Pos(".bmp") == 0) && (strBmpName.Pos(".bmp") == 0))
            strBmpName += ".bmp";
        chartAPSignal->SaveToBitmapFile(strBmpName);
    }
}
// ---------------------------------------------------------------------------
void __fastcall TAPMonitoringForm::FormClose(TObject *Sender, TCloseAction &Action)

{
    AP_MAC->Color = clWhite;
    timerAPSignal->Enabled = false;
    seriseAPSignal->Clear();
}
//---------------------------------------------------------------------------

