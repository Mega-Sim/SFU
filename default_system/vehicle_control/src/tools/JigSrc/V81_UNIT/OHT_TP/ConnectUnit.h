//---------------------------------------------------------------------------

#ifndef ConnectUnitH
#define ConnectUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>

#include "Def_Protocol.h"
//---------------------------------------------------------------------------
class TConnectForm : public TForm
{
__published:	// IDE-managed Components
   TPanel *Panel1;
   TLabel *Label1;
    TButton *btnConnectEdit;
   TComboBox *ConnectName;
   TEdit *ConnectIP;
    TButton *btnConnect;
    TButton *btnCancel;
    TPopupMenu *pmOHTInfo;
   TMenuItem *pmnuAdd;
   TMenuItem *pmnuAddYES;
   TMenuItem *pmnuEdit;
   TMenuItem *pmnuEditYES;
   TMenuItem *pmnuDelete;
   TMenuItem *pmnuDeleteYES;
	TButton *btnConnectAssistant;
   TPanel *LinePickupColor;
   TButton *btnIPEdit;
   TButton *btnIPLoad;
   void __fastcall ConnectNameClick(TObject *Sender);
   void __fastcall btnConnectClick(TObject *Sender);
   void __fastcall btnCancelClick(TObject *Sender);
   void __fastcall pmnuAddYESClick(TObject *Sender);
   void __fastcall pmnuEditYESClick(TObject *Sender);
   void __fastcall pmnuDeleteYESClick(TObject *Sender);
   void __fastcall FormCreate(TObject *Sender);
   void __fastcall btnConnectEditMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall btnConnectAssistantClick(TObject *Sender);
   void __fastcall LinePickupColorClick(TObject *Sender);
   void __fastcall btnIPEditClick(TObject *Sender);
   void __fastcall btnIPLoadClick(TObject *Sender);
	void __fastcall ConnectNameEnter(TObject *Sender);
	void __fastcall ConnectIPKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
private:	// User declarations
public:		// User declarations
   void Warnning_Display(TColor, String);
   __fastcall TConnectForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TConnectForm *ConnectForm;
//---------------------------------------------------------------------------
#endif
