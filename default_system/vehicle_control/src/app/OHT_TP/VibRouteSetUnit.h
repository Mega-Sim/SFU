//---------------------------------------------------------------------------

#ifndef VibRouteSetUnitH
#define VibRouteSetUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TVibRouteSet : public TForm
{
__published:	// IDE-managed Components
	TEdit *edtWayPoint1;
	TEdit *edtWayPoint2;
	TEdit *edtRound;
	TLabel *lblStartNode;
	TLabel *lblEndNode;
	TLabel *lblRound;
	TButton *Set;
	void __fastcall SetClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVibRouteSet(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TVibRouteSet *VibRouteSet;
//---------------------------------------------------------------------------
#endif
