//---------------------------------------------------------------------------

#ifndef LocMSaveH
#define LocMSaveH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TMoveForm : public TForm
{
__published:	// IDE-managed Components
   TEdit *Edit1;
   TLabel *lblAxis;
   TBitBtn *BitBtn1;
   TBitBtn *BitBtn2;
   TLabel *lblWho;
   void __fastcall FormShow(TObject *Sender);
   void __fastcall BitBtn1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TMoveForm(TComponent* Owner);

   int SetWhatMove(int, int);

public:
   int nIsWhatMove;
   int nIsAxis;
};
//---------------------------------------------------------------------------
extern PACKAGE TMoveForm *MoveForm;
//---------------------------------------------------------------------------
#endif
