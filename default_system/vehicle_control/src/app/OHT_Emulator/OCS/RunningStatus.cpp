//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RunningStatus.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmRunningStatus *frmRunningStatus;
//---------------------------------------------------------------------------
__fastcall TfrmRunningStatus::TfrmRunningStatus(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmRunningStatus::btnClearClick(TObject *Sender)
{
	mBSLog->Lines->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmRunningStatus::btnHideClick(TObject *Sender)
{
	ShowWindow(this->Handle, SW_HIDE);
}
//---------------------------------------------------------------------------
void __fastcall TfrmRunningStatus::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------
void TfrmRunningStatus::DisplayOHT(PKT_STATUS_REPORT_ID_03	CmdPkt)
{
	// 가동상태
	if(CmdPkt.STATUS.WorkingStatus.NowRunning)			Work_0->Color = clRed;
	else												Work_0->Color = clGray;

	if(CmdPkt.STATUS.WorkingStatus.LayoutChangeRun)		Work_1->Color = clRed;
	else												Work_1->Color = clGray;

	if(CmdPkt.STATUS.WorkingStatus.AbnormalTermination)	Work_2->Color = clRed;
	else												Work_2->Color = clGray;

	if(CmdPkt.STATUS.WorkingStatus.Reserved_3)			Work_3->Color = clRed;
	else												Work_3->Color = clGray;

	if(CmdPkt.STATUS.WorkingStatus.Auto)				Work_4->Color = clRed;
	else												Work_4->Color = clGray;

	if(CmdPkt.STATUS.WorkingStatus.ReturnPossible)		Work_5->Color = clRed;
	else												Work_5->Color = clGray;

	if(CmdPkt.STATUS.WorkingStatus.Reserved_6)			Work_6->Color = clRed;
	else												Work_6->Color = clGray;

	if(CmdPkt.STATUS.WorkingStatus.Reserved_7)			Work_7->Color = clRed;
	else												Work_7->Color = clGray;

	// 주행상태
	if(CmdPkt.STATUS.RunningStatus.CorrectPositionStop)	Running_0->Color = clRed;
	else												Running_0->Color = clGray;

	if(CmdPkt.STATUS.RunningStatus.Running)				Running_1->Color = clRed;
	else												Running_1->Color = clGray;

	if(CmdPkt.STATUS.RunningStatus.BzWait)				Running_2->Color = clRed;
	else												Running_2->Color = clGray;

	if(CmdPkt.STATUS.RunningStatus.CrashAvoidWait)		Running_3->Color = clRed;
	else												Running_3->Color = clGray;

	if(CmdPkt.STATUS.RunningStatus.SlowDownStop)		Running_4->Color = clRed;
	else												Running_4->Color = clGray;

	if(CmdPkt.STATUS.RunningStatus.Traffic_Sapche)		Running_5->Color = clRed;
	else												Running_5->Color = clGray;

	if(CmdPkt.STATUS.RunningStatus.Traffic_Jungche)		Running_6->Color = clRed;
	else												Running_6->Color = clGray;

	if(CmdPkt.STATUS.RunningStatus.CorrectPositionOvrRun)	Running_7->Color = clRed;
	else												Running_7->Color = clGray;

	//if(this->Visible)
	//	this->BringToFront();
}

//---------------------------------------------------------------------------
void TfrmRunningStatus::Log(AnsiString strLog)
{
 	mBSLog->Lines->Add(strLog);
}
//---------------------------------------------------------------------------
