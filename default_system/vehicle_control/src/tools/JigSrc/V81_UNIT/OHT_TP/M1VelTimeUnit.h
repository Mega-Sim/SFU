//---------------------------------------------------------------------------

#ifndef M1VelTimeUnitH
#define M1VelTimeUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TM1VelForm : public TForm
{
__published:	// IDE-managed Components
   TLabel *Label1;
   TLabel *Label2;
   TEdit *Edit1;
   TBitBtn *BitBtn1;
   TBitBtn *BitBtn2;
   void __fastcall BitBtn1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TM1VelForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TM1VelForm *M1VelForm;
//---------------------------------------------------------------------------
#endif
