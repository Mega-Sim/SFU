//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "JigCheckbox.h"
#include "JigHoistUnit.h"
#include "JigSlideUnit.h"
#include "JigHandUnit.h"
#include "JigShutterUnit.h"
#include "JigRotateUnit.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma resource "*.dfm"


TUnitCheckBox *UnitCheckBox;

//---------------------------------------------------------------------------
__fastcall TUnitCheckBox::TUnitCheckBox(TComponent* Owner)
    : TForm(Owner)
{
	m_bInterlock = false;

//	m_pHoistUnit = NULL;
//	m_pSlideUnit = NULL;
//	m_pRotateUnit = NULL;
//	m_pHandUnit = NULL;
//	m_pShutterUnit = NULL;

	m_pHoistUnit = new THoistUnit(this);
	m_pSlideUnit = new TSlideUnit(this);
	m_pHandUnit = new THandUnit(this);
	m_pRotateUnit = new TRotateUnit(this);
//	m_pShutterUnit = new TShutterUnit(this);

}
//---------------------------------------------------------------------------
void __fastcall TUnitCheckBox::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = true;
	return;
}
//---------------------------------------------------------------------------

void __fastcall TUnitCheckBox::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose =true;

    return;
}
//---------------------------------------------------------------------------


void __fastcall TUnitCheckBox::HoistClick(TObject *Sender)
{
//	if(m_pHoistUnit == NULL)
//	{
//		m_pHoistUnit = new THoistUnit(this);
//		m_pHoistUnit->Show();
//	}
	m_pHoistUnit->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUnitCheckBox::SlideClick(TObject *Sender)
{
//	if(m_pSlideUnit == NULL)
//	{
//		m_pSlideUnit = new TSlideUnit(this);
//		m_pSlideUnit->Show();
//	}
	m_pSlideUnit->Show();
}
//---------------------------------------------------------------------------


void __fastcall TUnitCheckBox::HandClick(TObject *Sender)
{

//	if(m_pHandUnit == NULL)
//	{
//		m_pHandUnit = new THandUnit(this);
//		m_pHandUnit->Show();
//	}

	m_pHandUnit->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUnitCheckBox::RotateClick(TObject *Sender)
{

//	if(m_pRotateUnit == NULL)
//	{
//		m_pRotateUnit = new TRotateUnit(this);
//		m_pRotateUnit->Show();
//	}

	m_pRotateUnit->Show();
}
//---------------------------------------------------------------------------

void __fastcall TUnitCheckBox::ShutterClick(TObject *Sender)
{
//	if(m_pShutterUnit == NULL)
//	{
//		m_pShutterUnit = new TShutterUnit(this);
//		m_pShutterUnit->Show();
//	}
//	m_pShutterUnit->Show();
}
//---------------------------------------------------------------------------

