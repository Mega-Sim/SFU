//---------------------------------------------------------------------------

#ifndef IOMapUnitH
#define IOMapUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <IniFiles.hpp>

#include "../Common/Def_DataType.h"
//---------------------------------------------------------------------------
// Grid Default 값
#define INPUTGRID_COL           4   ///< Input Grid 열
#define INPUTGRID_ROW           64  ///< Input Grid 행
#define OUTPUTGRID_COL          3   ///< Output Grid 열
#define OUTPUTGRID_ROW          64  ///< Output Grid 행
#define CHECKONOFF_COL          3   ///< CheckONOFF Grid 열
#define CHECKONOFF_ROW          34  ///< CheckONOFF Grid 행
#define MOTORGRID_COL           3   ///< Motor Grid 열
#define MOTORGRID_ROW           40  ///< Motor Grid 행

#define COL_INPUTNAME           1   ///< Input name이 출력되는 열의 Index
#define COL_INPUTONOFF          2   ///< Input ON/OFF가 출력되는 열의 Index
#define COL_INPUTDATA           3   ///< Input 실제값이 출력되는 열의 Index

#define COL_OUTPUTNAME          1   ///< Input name이 출력되는 열의 Index
#define COL_OUTPUTDATA          2   ///< Input 실제값이 출력되는 열의 Index

// Ini 파일 Key 값
#define INI_KEY_INPUT           L"INPUT"
#define INI_KEY_OUTPUT          L"OUTPUT"
//---------------------------------------------------------------------------

/**
@class   TIOMapForm
@date    2013.02.18
@author  zzang9un
@brief   IOMap을 보여주는 Form
*/
class TIOMapForm : public TForm
{
__published:	// IDE-managed Components
    TPopupMenu *popupCheckONOFF;
    TMenuItem *mniSetON;
    TMenuItem *mniSetOFF;
    TPopupMenu *popupOutput;
    TMenuItem *mniSetTrue;
    TMenuItem *mniSetFalse;
    TPageControl *pgcIOMap;
    TTabSheet *tabInput;
    TStringGrid *strGridInput;
    TTabSheet *tabOutput;
    TStringGrid *strGridOutput;
    TTabSheet *tabCheckONOFF;
    TStringGrid *strgridUseOnOff;
    TTabSheet *tabMotor;
    TStringGrid *strGridMotor;
   void __fastcall strGridInputDrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
   void __fastcall strGridOutputDrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
   void __fastcall strgridUseOnOffDrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
   void __fastcall popupCheckONOFFPopup(TObject *Sender);
   void __fastcall strgridUseOnOffClick(TObject *Sender);
   void __fastcall mniSetTrueClick(TObject *Sender);
   void __fastcall mniSetOFFClick(TObject *Sender);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall strGridMotorDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
   void __fastcall strGridOutputClick(TObject *Sender);
   void __fastcall popupOutputPopup(TObject *Sender);
   void __fastcall mniSetFalseClick(TObject *Sender);
   void __fastcall mniSetONClick(TObject *Sender);

private:	// User declarations
	TIniFile *IOMapIni; ///< IOMap.ini 파일을 불러오긴 위한 변수
	IO_MAP_FIELD	*m_IOMapField;
	int iInputNumCount;
	int iOutputNumCount;

public:		// User declarations
   __fastcall TIOMapForm(TComponent* Owner);
   void IO_Display();
   void SendUseONOFF(int, int);
	void InitIOMap(IO_MAP_FIELD*);
};

//---------------------------------------------------------------------------
extern PACKAGE TIOMapForm *IOMapForm;
//---------------------------------------------------------------------------
#endif





