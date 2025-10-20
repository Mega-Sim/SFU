//---------------------------------------------------------------------------

#ifndef M24VelTimeUnitH
#define M24VelTimeUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TM24VelForm : public TForm
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
   __fastcall TM24VelForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TM24VelForm *M24VelForm;
//---------------------------------------------------------------------------
#endif
