//---------------------------------------------------------------------------
/**
@file    TransUnit.h
$Rev:: 299           $:  Revision of last commit
$Author:: puting     $:  Author of last commit
$Date:: 2012-11-29 1#$:  Date of last commit
@version OHT 7.0
@brief   이적재를 수행하기 위한 Form Header File
*/

#ifndef TransUnitH
#define TransUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Menus.hpp>

#include "../Common/Def_DataType.h"
#include "../Common/Protocol_Define_Modify.h"
//---------------------------------------------------------------------------
class TTransForm : public TForm
{
__published:	// IDE-managed Components
    TPanel *panStringGrid;
    TStringGrid *strGridTransSetting;
    TPanel *panTransType;
    TBitBtn *btnDoTrans;
    TBitBtn *btnCancelTrans;
    TPopupMenu *popMenuOnOff;
    TMenuItem *menuItemON;
    TMenuItem *menuItemOFF;
    TTimer *tmUpdateUI;
    TCheckBox *chkHandDetectEQ;
    TComboBox *cbbPIODirection;
    TComboBox *cbbHoistSpeedLevel;
    TComboBox *cbbShiftSpeedLevel;
    TLabel *lbPIO;
    TLabel *lbHoistSpeed;
    TLabel *lbShiftSpeed;
    void __fastcall strGridTransSettingDrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall strGridTransSettingClick(TObject *Sender);
    void __fastcall menuItemONClick(TObject *Sender);
    void __fastcall menuItemOFFClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall tmUpdateUITimer(TObject *Sender);
    void __fastcall btnCancelTransClick(TObject *Sender);
    void __fastcall btnDoTransClick(TObject *Sender);
    
private:	// User declarations
    BYTE m_TransCmdData; /// 이적재 명령 종류
    BYTE m_PortType;     /// 이적재 Port Type

    String m_strTransCmdType; /// Command Type 화면 표시를 위한 String
    String m_strPortType;     /// Port Type 화면 표시를 위한 String

    // UI 관련 함수
    void InitGridDisplay();
    void UpdateUI();

    // Trans UseControl 관련 함수
    void ChangeUseControl(int UseNum, bool OnOff);

    // Trans Command를 생성하는 함수
    void MakeTransCmd(TRANS_CMD *pTransCmd);
	void MakeTransPkt(PKT_TP2OHT_TRANS_LOAD_UNLOAD *pTrans);

public:		// User declarations
    __fastcall TTransForm(TComponent *Owner);

    void SetTrans(BYTE TransCmdData, BYTE porttype);

    // by zzang9un 2013.02.05 : 이적재 롱런 테스트를 위한 함수(외부 클래스에서 호출함)
    void SendTransCmd(BYTE TransCmdData, BYTE PortType);
};
//---------------------------------------------------------------------------
extern PACKAGE TTransForm *TransForm;
//---------------------------------------------------------------------------
#endif

