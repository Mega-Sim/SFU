//---------------------------------------------------------------------------
//AOHC-302 맵기반 주행경로 자동탐색
#include <vcl.h>
#pragma hdrstop

#include "VibRouteSetUnit.h"
#include "BayTestUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVibRouteSet *VibRouteSet;
//---------------------------------------------------------------------------
__fastcall TVibRouteSet::TVibRouteSet(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TVibRouteSet::SetClick(TObject *Sender)
{

	int nWayPointNode1 = edtWayPoint1->Text.ToIntDef(0);
	int nWayPointNode2 = edtWayPoint2->Text.ToIntDef(0);
	int Round = edtRound->Text.ToIntDef(0);

	frm_Bay_Test->nVibWayPointNode1 = nWayPointNode1;
	frm_Bay_Test->nVibWayPointNode2 = nWayPointNode2;
	frm_Bay_Test->nVibRound = Round;
	frm_Bay_Test->VibLongrunSetting();

	this->Close();
}
//---------------------------------------------------------------------------
