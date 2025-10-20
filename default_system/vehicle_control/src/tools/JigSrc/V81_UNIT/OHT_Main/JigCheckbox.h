//---------------------------------------------------------------------------

#ifndef JigCheckboxH
#define JigCheckboxH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class THoistUnit;
class TSlideUnit;
class THandUnit;
class TShutterUnit;
class TRotateUnit;

class TUnitCheckBox : public TForm
{
__published:	// IDE-managed Components
    TButton *Slide;
	TButton *Shutter;
	TButton *Hand;
    TButton *Rotate;
	TButton *Hoist;


    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall HoistClick(TObject *Sender);
	void __fastcall SlideClick(TObject *Sender);
	void __fastcall HandClick(TObject *Sender);
	void __fastcall RotateClick(TObject *Sender);
	void __fastcall ShutterClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TUnitCheckBox(TComponent* Owner);

	bool m_bInterlock;

	THoistUnit *m_pHoistUnit;
	TSlideUnit *m_pSlideUnit;
	THandUnit *m_pHandUnit;
	TRotateUnit *m_pRotateUnit;
	TShutterUnit *m_pShutterUnit;


};
//---------------------------------------------------------------------------
extern PACKAGE TUnitCheckBox *UnitCheckBox;
//---------------------------------------------------------------------------
#endif
