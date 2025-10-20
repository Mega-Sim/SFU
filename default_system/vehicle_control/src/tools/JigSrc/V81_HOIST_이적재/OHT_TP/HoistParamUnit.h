//---------------------------------------------------------------------------

#ifndef HoistParamUnitH
#define HoistParamUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "Protocol_Define_Modify.h"

//---------------------------------------------------------------------------
class THoistParamForm : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *edtPulse_0;
	TEdit *edtPosMM_0;
	TButton *btnMove_0;
	TEdit *edtPulse_1;
	TButton *btnMove_1;
	TEdit *edtPosMM_1;
	TEdit *edtPulse_2;
	TButton *btnMove_2;
	TEdit *edtPosMM_2;
	TEdit *edtPulse_3;
	TButton *btnMove_3;
	TEdit *edtPosMM_3;
	TEdit *edtPulse_4;
	TButton *btnMove_4;
	TEdit *edtPosMM_4;
	TEdit *edtPulse_5;
	TButton *btnMove_5;
	TEdit *edtPosMM_5;
	TEdit *edtPulse_6;
	TButton *btnMove_6;
	TEdit *edtPosMM_6;
	TButton *btnParamLoad;
	TButton *btnParamSave;
	TEdit *edtPulse_7;
	TButton *btnMove_7;
	TEdit *edtPosMM_7;
	TButton *btnMove_8;
	TEdit *edtPosMM_8;
	TEdit *edtPulse_8;
	TButton *btnMove_9;
	TEdit *edtPosMM_9;
	TEdit *edtPulse_9;
	TButton *btnMove_zero;
	TLabel *Label1;
	TEdit *edtRealMM_master;
	TEdit *edtRealMM_zero;
	TEdit *edtRealMM_0;
	TEdit *edtRealMM_1;
	TEdit *edtRealMM_2;
	TEdit *edtRealMM_3;
	TEdit *edtRealMM_4;
	TEdit *edtRealMM_5;
	TEdit *edtRealMM_6;
	TEdit *edtRealMM_7;
	TEdit *edtRealMM_8;
	TEdit *edtRealMM_9;
	TLabel *Label4;
	TEdit *edtPosMM_master;
	TLabel *Label5;
	TLabel *Label6;
	TButton *btnConfirm_zero;
	TButton *btnConfirm_0;
	TButton *btnConfirm_1;
	TButton *btnConfirm_2;
	TButton *btnConfirm_3;
	TButton *btnConfirm_4;
	TButton *btnConfirm_5;
	TButton *btnConfirm_6;
	TButton *btnConfirm_7;
	TButton *btnConfirm_8;
	TButton *btnConfirm_9;
	TButton *btnCancel_zero;
	TButton *btnCancel_0;
	TButton *btnCancel_1;
	TButton *btnCancel_2;
	TButton *btnCancel_3;
	TButton *btnCancel_4;
	TButton *btnCancel_5;
	TButton *btnCancel_6;
	TButton *btnCancel_7;
	TButton *btnCancel_8;
	TButton *btnCancel_9;
	TButton *btnConfirm_master;
	TButton *btnCancel_master;
   void __fastcall FormShow(TObject *Sender);
	void __fastcall btnMove_0Click(TObject *Sender);
	void __fastcall btnMove_1Click(TObject *Sender);
	void __fastcall btnMove_2Click(TObject *Sender);
	void __fastcall btnMove_3Click(TObject *Sender);
	void __fastcall btnMove_4Click(TObject *Sender);
	void __fastcall btnMove_5Click(TObject *Sender);
	void __fastcall btnMove_6Click(TObject *Sender);
	void __fastcall btnParamLoadClick(TObject *Sender);
	void __fastcall btnParamSaveClick(TObject *Sender);
	void __fastcall btnMove_7Click(TObject *Sender);
	void __fastcall btnMove_8Click(TObject *Sender);
	void __fastcall btnMove_9Click(TObject *Sender);
	void __fastcall edtRealMM_masterKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall btnConfirm_masterClick(TObject *Sender);
	void __fastcall btnCancel_masterClick(TObject *Sender);
	void __fastcall btnConfirm_zeroClick(TObject *Sender);
	void __fastcall btnCancel_zeroClick(TObject *Sender);
	void __fastcall btnConfirm_0Click(TObject *Sender);
	void __fastcall btnCancel_0Click(TObject *Sender);
	void __fastcall btnConfirm_1Click(TObject *Sender);
	void __fastcall btnCancel_1Click(TObject *Sender);
	void __fastcall btnConfirm_2Click(TObject *Sender);
	void __fastcall btnCancel_2Click(TObject *Sender);
	void __fastcall btnConfirm_3Click(TObject *Sender);
	void __fastcall btnCancel_3Click(TObject *Sender);
	void __fastcall btnConfirm_4Click(TObject *Sender);
	void __fastcall btnCancel_4Click(TObject *Sender);
	void __fastcall btnConfirm_5Click(TObject *Sender);
	void __fastcall btnCancel_5Click(TObject *Sender);
	void __fastcall btnConfirm_6Click(TObject *Sender);
	void __fastcall btnCancel_6Click(TObject *Sender);
	void __fastcall btnConfirm_7Click(TObject *Sender);
	void __fastcall btnCancel_7Click(TObject *Sender);
	void __fastcall btnConfirm_8Click(TObject *Sender);
	void __fastcall btnCancel_8Click(TObject *Sender);
	void __fastcall btnConfirm_9Click(TObject *Sender);
	void __fastcall btnCancel_9Click(TObject *Sender);


protected:

public:		// User declarations
   __fastcall THoistParamForm(TComponent* Owner);
   int HoistParamSave(int);
   void HoistParamRead();
   //void OHTNameFind(void);
   String m_strOHT;
   bool bParamOK;
   struct Hoist_Param
   {
      String strOHTNum;
      double dDriving;
      double dHoist1;
      double dHoist2;
      double dHoist3;
      double dHoist4;
      double dHoist5;
      double dHoist6;
      double dHoist7;
      double dHoist8;
      double dSlide;
      double dRotate;
      double dMPSlide;
   }HoistParam;
/*   struct Hoist_Param
   {
	  String strOHTNum;
      float fData[11];
   }HoistParam[500];
*/

public:		// User declarations
	void Display_Result(PKT_TP2OHT_HOIST_TEACH_COMMAND PKT);

private:
	void HoistMoveByPulse(double dblHoistPos);
	void InitHoistParam();
	void InitConfirm();
};
//---------------------------------------------------------------------------
extern PACKAGE THoistParamForm *HoistParamForm;
//---------------------------------------------------------------------------
#endif
