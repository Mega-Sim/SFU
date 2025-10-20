//---------------------------------------------------------------------------

#ifndef RotateOriginUnitH
#define RotateOriginUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TRotateOrgForm : public TForm
{
__published:	// IDE-managed Components
   TTimer *Timer1;
   TLabel *Label15;
   TPanel *panAx4_ServoON;
   TPanel *Panel12;
   TLabel *lblMotor4_pos;
   TShape *shp4Run;
   TEdit *Edit1;
   TButton *btnOK;
   TButton *btnClose;
   TLabel *Label1;
   TLabel *Label2;
   TLabel *Label3;
   void __fastcall FormShow(TObject *Sender);
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
   void __fastcall Timer1Timer(TObject *Sender);
   void __fastcall btnCloseClick(TObject *Sender);
   void __fastcall btnOKClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TRotateOrgForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRotateOrgForm *RotateOrgForm;
//---------------------------------------------------------------------------
#endif
