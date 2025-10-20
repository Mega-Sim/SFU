//---------------------------------------------------------------------------

#ifndef StatusUnitH
#define StatusUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>

#include "WaitUnit.h"
#include "CMDControlUnit.h"
#include "TeachingUnit.h"
#include "Utility.h"
#include "ConversionUnit.h"
#include "HoistParamUnit.h"
#include "ErrorUnit.h"
#include "E23PIOUnit.h"
#include "../Common/Def_DataType.h"
#include "../Common/Def_Protocol.h"
#include "OHT_TLV.h"
//---------------------------------------------------------------------------
class TStatusForm : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel13;
    TGroupBox *GroupBox1;
    TPanel *panCMDType;
    TPanel *panMode;
    TPanel *panTPSt;
	TPanel *panCurrentNodeID;
    TPanel *panErrCode;
    TPanel *panCmdNumber;
    TPanel *panRcvID;
    TPanel *panResponse;
	TPanel *panTargetNodeID;
    TPopupMenu *popClear;
    TMenuItem *CmdBuffClear1;
    TMenuItem *N1;
    TMenuItem *CmdBuffSave1;
    TPanel *panOHTVer;
    TMemo *memoStatus;
    TPanel *panStatus;
	TPanel *panCurrentStationID;
	TPanel *panTargetStationID;
    TPanel *panSndCmdID;
    TTimer *tmDisplayUpdate;
	TLabel *lbCommandType;
	TLabel *lbMode;
	TLabel *lbStatus;
	TLabel *lbCurrentNodeID;
	TLabel *lbTPStatus;
	TLabel *lbStopNodeID;
	TLabel *lbCurrentStationID;
	TLabel *lbTargetStationID;
	TLabel *lbErrorCode;
	TLabel *lbCmdNumber;
	TLabel *lbSndCmdID;
	TLabel *lbRcvCmdID;
	TLabel *lbRcvRespon;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall CmdBuffClear1Click(TObject *Sender);
    void __fastcall CmdBuffSave1Click(TObject *Sender);
    void __fastcall tmDisplayUpdateTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    
private:	// User declarations
    void Analysis_AMC_IOData();
    void Analysis_Use_Data(uCMD_USE_DATA *pUseInfo);

    int  Analysis_Com(String, String); //New OHT
    int  Analysis_Motor(String); //New OHT
    int  Analysis_LogSaveInfo(String);


    ///v7.0
    void Analysis_Use_Data(USE_INFO *pUseInfo); //puting 2012.11.29
    // by zzang9un 2012.11.24 : OHT Status를 분석하는 함수
    void AnalyzeOHTStatus(MSG_STATUS_OHT2TP *pCmdStatus); //New OHT
    void AnalysisMotorInfo(MSG_STATUS_OHT2TP *pCmdStatus); //New OHT
    int Analysis_LogSaveInfo7_0(MSG_STATUS_OHT2TP *pCmdStatus);

    // OCS, TP로부터 수신한 Command를 저장하기 위한 구조체
    MSG_STATUS_OHT2TP   *m_CmdStatus;     ///< OHT Status Command
    uTP_CMD             *m_CmdCommonData;
    
    int Set_UBG_Output(int, int, int, int);

public:		// User declarations
    __fastcall TStatusForm(TComponent* Owner);

    bool bReadFlg;
    DWORD g_dwTransStartTime;
};
//---------------------------------------------------------------------------
extern PACKAGE TStatusForm *StatusForm;
//---------------------------------------------------------------------------
#endif
