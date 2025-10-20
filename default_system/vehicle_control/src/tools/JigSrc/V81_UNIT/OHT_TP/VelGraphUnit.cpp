// ---------------------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#pragma hdrstop

#include "VelGraphUnit.h"
#include "OHTInfoUnit.h"
#include "M1VelTimeUnit.h"
#include "M24VelTimeUnit.h"
#include "Define.h"
#include "Utility.h"
#include "Def_TP.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVelGraphForm *VelGraphForm;

// ---------------------------------------------------------------------------
__fastcall TVelGraphForm::TVelGraphForm(TComponent* Owner) : TForm(Owner)
{
	nTmpCount1 = 0;
    fTmpCount1 = 0.0;
	nTmpCount24 = 0;
    fTmpCount24 = 0.0;

}

// ---------------------------------------------------------------------------
void __fastcall TVelGraphForm::FormCreate(TObject *Sender)
{
    Motor1Timer->Enabled = false;
    Motor24Timer->Enabled = false;
}

// ---------------------------------------------------------------------------
void __fastcall TVelGraphForm::FormDestroy(TObject *Sender)
{
    VelTimer->Enabled = false;
    Motor1Timer->Enabled = false;
    Motor24Timer->Enabled = false;
}

// ---------------------------------------------------------------------------
void __fastcall TVelGraphForm::VelTimerTimer(TObject *Sender)
{
    String strVelData;
    int i;
    static double fMaxVel[4] =
    {0.0, 0.0, 0.0, 0.0}, fMinVel[4] =
    {0.0, 0.0, 0.0, 0.0};
    double fMotorVel_rpm[4];

    fMotorVel_rpm[0] = gm_OHTStatus->Motor_Driving_F->dVelocity_rpm;
    fMotorVel_rpm[1] = gm_OHTStatus->Motor_Hoist->dVelocity_rpm;
    fMotorVel_rpm[2] = gm_OHTStatus->Motor_Shift->dVelocity_rpm;
    fMotorVel_rpm[3] = gm_OHTStatus->Motor_Rotate->dVelocity_rpm;

    for(i = 0; i < 4; i++)
    {
        if(fMaxVel[i] < fMotorVel_rpm[i])
            fMaxVel[i] = fMotorVel_rpm[i];

        if(fMinVel[i] == 0.0)
            fMinVel[i] = fMotorVel_rpm[i];
        else if(fMinVel[i] > fMotorVel_rpm[i])
            fMinVel[i] = fMotorVel_rpm[i];

    }

    // *** #1 Vel ***
    strVelData.sprintf(L"%.05f", fMotorVel_rpm[0]);
    lblMotor1_1->Caption = strVelData;

	strVelData.sprintf(L"%.02f", gm_OHTStatus->Motor_Driving_F->dVelocity_mpersec);
	lblMotor1_1_msec->Caption = strVelData;

    strVelData.sprintf(L"%.05f", fMaxVel[0]);
    lblMotor1_2->Caption = strVelData;

    strVelData.sprintf(L"%.05f", fMinVel[0]);
    lblMotor1_3->Caption = strVelData;

    // *** #2 Vel ***
	//strVelData.sprintf(L"%.05f", fMotorVel_rpm[1]);
	//lblMotor2_1->Caption = strVelData;

	strVelData.sprintf(L"%.05f", gm_OHTStatus->Motor_Hoist->dVelocity_mpersec);
	lblMotor2_1->Caption = strVelData;

    strVelData.sprintf(L"%.05f", fMaxVel[1]);
    lblMotor2_2->Caption = strVelData;

    strVelData.sprintf(L"%.05f", fMinVel[1]);
    lblMotor2_3->Caption = strVelData;

    // *** #3 Vel ***
	//strVelData.sprintf(L"%.05f", fMotorVel_rpm[2]);
	//lblMotor3_1->Caption = strVelData;

	strVelData.sprintf(L"%.05f", gm_OHTStatus->Motor_Shift->dVelocity_mpersec);
	lblMotor3_1->Caption = strVelData;

    strVelData.sprintf(L"%.05f", fMaxVel[2]);
    lblMotor3_2->Caption = strVelData;

    strVelData.sprintf(L"%.05f", fMinVel[2]);
    lblMotor3_3->Caption = strVelData;

    // *** #4 Vel ***
	//strVelData.sprintf(L"%.05f", fMotorVel_rpm[3]);
	//lblMotor4_1->Caption = strVelData;

	strVelData.sprintf(L"%.05f", gm_OHTStatus->Motor_Rotate->dVelocity_mpersec);
	lblMotor4_1->Caption = strVelData;

    strVelData.sprintf(L"%.05f", fMaxVel[3]);
    lblMotor4_2->Caption = strVelData;

    strVelData.sprintf(L"%.05f", fMinVel[3]);
    lblMotor4_3->Caption = strVelData;

}

// ---------------------------------------------------------------------------
void TVelGraphForm::Graph_Start1(int nTime_min1)
{
    fastlineDriving_F->Clear();
    fastlineDriving_R->Clear();

    chartDrivingSpeed->BottomAxis->Automatic = true;
	nxTime1 = nTime_min1 * 60;
    chartDrivingSpeed->BottomAxis->Automatic = false;
    chartDrivingSpeed->BottomAxis->Minimum = 0.0;
    chartDrivingSpeed->BottomAxis->Maximum = (float)nxTime1;

	nTmpCount1 = 0;
    fTmpCount1 = 0.0;
    Motor1Timer->Enabled = true;
}

// ---------------------------------------------------------------------------
void TVelGraphForm::Graph_Start24(int nTime_min24)
{
	fastlineHoist->Clear();

	chartHoistSpeed->BottomAxis->Automatic = true;
	nxTime24 = nTime_min24 * 60;
	chartHoistSpeed->BottomAxis->Automatic = false;
	chartHoistSpeed->BottomAxis->Minimum = 0.0;
	chartHoistSpeed->BottomAxis->Maximum = (float)nxTime24;

	nTmpCount24 = 0;
	fTmpCount24 = 0.0;
	Motor24Timer->Enabled = true;
}
// ---------------------------------------------------------------------------

void __fastcall TVelGraphForm::Motor1TimerTimer(TObject *Sender)
{
    String strHIDValue, strNode;
	nTmpCount1++;

    if(CheckBox2->Checked == true)
    {
        if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_MANUAL) &&
            (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ERROR))
            Motor1Timer->Enabled = false;
    }
    fTmpCount1 = fTmpCount1 + 0.2;

    if(fTmpCount1 >= (float)nxTime1)
    {
        chartDrivingSpeed->BottomAxis->Automatic = true;
		fastlineDriving_F->Delete(0);
		if(chkDisplayBothDrv->Checked == true)
            fastlineDriving_R->Delete(0);
    }

	if(chkTime->Checked == false)
	{
		// strNode.sprintf(L"%06X", gm_OHTStatus->m_OHTStatus.Data.CurNodeID);
		strNode.sprintf(L"%06d", gm_OHTStatus->m_OHTStatus.Data.CurNodeID);
		fastlineDriving_F->AddXY(fTmpCount1, fabs(gm_OHTStatus->Motor_Driving_F->dVelocity_mpersec), strNode, COLOR_BLACK);
		if(chkDisplayBothDrv->Checked == true)
			fastlineDriving_R->AddXY(fTmpCount1, fabs(gm_OHTStatus->Motor_Driving_R->dVelocity_mpersec), strNode, COLOR_BLACK);
	}
	else
    {
        fastlineDriving_F->AddXY(fTmpCount1, fabs(gm_OHTStatus->Motor_Driving_F->dVelocity_mpersec), "", COLOR_BLACK);
		if(chkDisplayBothDrv->Checked == true)
			fastlineDriving_R->AddXY(fTmpCount1, fabs(gm_OHTStatus->Motor_Driving_R->dVelocity_mpersec), "", COLOR_BLACK);
	}

    if(chkLogSave1->Checked == true)
    {
        TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
        String strLog;
        strLog.sprintf(L"%.02f", gm_OHTStatus->Motor_Driving_F->dVelocity_mpersec);
        m_LogFile->WriteLogFile("AASpeed", strLog);
        delete m_LogFile;
    }

}

// ---------------------------------------------------------------------------
void __fastcall TVelGraphForm::N1Stop1Click(TObject *Sender)
{
    VelTimer->Enabled = true;
    Motor1Timer->Enabled = false;
}

// ---------------------------------------------------------------------------
void __fastcall TVelGraphForm::N1Start1Click(TObject *Sender)
{
	VelTimer->Enabled = true;
    M1VelForm->ShowModal();
}

// ---------------------------------------------------------------------------
void __fastcall TVelGraphForm::N24Start1Click(TObject *Sender)
{
	VelTimer->Enabled = true;
    M24VelForm->ShowModal();
}

// ---------------------------------------------------------------------------
void __fastcall TVelGraphForm::N24Stop1Click(TObject *Sender)
{
    VelTimer->Enabled = true;
    Motor24Timer->Enabled = false;
}

// ---------------------------------------------------------------------------
void __fastcall TVelGraphForm::Motor24TimerTimer(TObject *Sender)
{
	String strNode;

	nTmpCount24++;

	fTmpCount24 = fTmpCount24 + 0.2;

    if(fTmpCount24 >= (float)nxTime24)
	{
		chartHoistSpeed->BottomAxis->Automatic = true;
		fastlineHoist->Delete(0);
	}

	fastlineHoist->AddXY(fTmpCount24, fabs(gm_OHTStatus->Motor_Hoist->dVelocity_mpersec), "", COLOR_BLACK);
}

// ---------------------------------------------------------------------------
void __fastcall TVelGraphForm::Motor1Graph1Click(TObject *Sender)
{
	String strBmpName;

    if(OpenDialog1->Execute())
    {
        strBmpName = OpenDialog1->FileName;
        if((strBmpName.Pos(".bmp") == 0) && (strBmpName.Pos(".bmp") == 0))
            strBmpName += ".bmp";
        chartDrivingSpeed->SaveToBitmapFile(strBmpName);
    }

}

// ---------------------------------------------------------------------------
void __fastcall TVelGraphForm::Motor24Graph1Click(TObject *Sender)
{
    String strBmpName;

    if(OpenDialog2->Execute())
    {
        strBmpName = OpenDialog2->FileName;
        if((strBmpName.Pos(".bmp") == 0) && (strBmpName.Pos(".bmp") == 0))
            strBmpName += ".bmp";
//        Chart2->SaveToBitmapFile(strBmpName);
    }
    Sleep(1000);

}

// ---------------------------------------------------------------------------
void __fastcall TVelGraphForm::Open1Click(TObject *Sender)
{
    String strTmpName, strTmpBuffer, strNode;
    int i;
    TStringList *tVelData = new TStringList;
    double dVel_rpm, dVel_mpersec;
    int nPos;

    if(OpenDialog3->Execute())
    {
        fastlineDriving_F->Clear();
        Motor1Timer->Enabled = false;
        strTmpName = OpenDialog3->FileName;
        tVelData->LoadFromFile(strTmpName);
        for(i = 0; i < tVelData->Count; i++)
        {
            strTmpBuffer = tVelData->Strings[i];
            if(strTmpBuffer.Pos("$s") == 0)
                continue;
            strNode.sprintf(L"N%s", strTmpBuffer.SubString(25, 6));
            nPos = strTmpBuffer.Pos(",");
            strTmpBuffer.Delete(1, nPos);
            nPos = strTmpBuffer.Pos(",");
            strTmpBuffer.Delete(1, nPos);
            nPos = strTmpBuffer.Pos("B");

            dVel_mpersec = StrToFloat(strTmpBuffer.SubString(1, nPos - 1));

            // dVel_mpersec = dVel_rpm*0.01*DIST_1ROTATE_CM/60.0;
            if(dVel_mpersec < 3.5)
                fastlineDriving_F->AddXY(i, dVel_mpersec, strNode, COLOR_BLACK);
        }
    }
}
// ---------------------------------------------------------------------------

