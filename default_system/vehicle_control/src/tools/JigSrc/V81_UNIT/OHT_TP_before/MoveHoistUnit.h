//---------------------------------------------------------------------------

#ifndef MoveHoistUnitH
#define MoveHoistUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TMHoistForm : public TForm
{
__published:	// IDE-managed Components
   TGroupBox *GroupBox2;
   TLabel *Label5;
   TPanel *Panel10;
   TLabel *lblMotor2;
   TShape *shp2Run;
    TBitBtn *btnHoistMoveDST;
   TLabel *Dist;
   TPanel *Panel1;
   TLabel *lblDist;
   TLabel *Label2;
   TLabel *Label3;
   TEdit *edtSpeed;
   TLabel *Label4;
   TLabel *Label6;
   TEdit *edtAccel;
   TLabel *Label7;
    TBitBtn *btnCancel;
   void __fastcall btnHoistMoveDSTClick(TObject *Sender);
   void __fastcall btnCancelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TMHoistForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMHoistForm *MHoistForm;
//---------------------------------------------------------------------------
#endif
