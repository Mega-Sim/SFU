//---------------------------------------------------------------------------

#ifndef ErrorSearchUnitH
#define ErrorSearchUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Grids.hpp>
#include <IniFiles.hpp>

#define ERRCODE_STRGRID_COL     3
#define ERRCODE_COUNT         445
//---------------------------------------------------------------------------
class TErrorSearchForm : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *grpdgrp1;
    TBitBtn *btnSearch;
    TLabel *lbl1;
    TEdit *edtErrorCode;
    TStringGrid *strngrdErrorCode;
    TBitBtn *btnTotalSearch;
    void __fastcall btnSearchClick(TObject *Sender);
    void __fastcall btnOpenClick(TObject *Sender);
    void __fastcall btnSaveClick(TObject *Sender);
    void __fastcall btnTotalSearchClick(TObject *Sender);
    void __fastcall edtErrorCodeChange(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

private:	// User declarations
    TIniFile *m_pErrorIni;

    TStringList *m_pListErrNum;    ///< 에러 번호 String List
    TStringList *m_pListErrName;   ///< 에러 설명 String List
    
public:		// User declarations
    __fastcall TErrorSearchForm(TComponent* Owner);
    void InitErrorCodeStrGrid(int RowCount);
    void FileSearchingRead(int,String);
    void ReadErrorCode();
    void DisplayErrorCode();
};
//---------------------------------------------------------------------------
extern PACKAGE TErrorSearchForm *ErrorSearchForm;
//---------------------------------------------------------------------------
#endif
