//---------------------------------------------------------------------------

#ifndef TimeSettingUnitH
#define TimeSettingUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TTimeSettingForm : public TForm
{
__published:	// IDE-managed Components
   TLabel *Label1;
   TEdit *Edit1;
   TLabel *Label2;
   TBitBtn *BitBtn1;
   TBitBtn *BitBtn2;
   void __fastcall BitBtn1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TTimeSettingForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTimeSettingForm *TimeSettingForm;
//---------------------------------------------------------------------------
#endif
