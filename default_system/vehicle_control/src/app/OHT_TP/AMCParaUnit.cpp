//---------------------------------------------------------------------------

#include <vcl.h>
#include <IniFiles.hpp>
#include <tchar.h>
#pragma hdrstop

#include "AMCParaUnit.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAMCParaForm *AMCParaForm;
//---------------------------------------------------------------------------
__fastcall TAMCParaForm::TAMCParaForm(TComponent* Owner)
   : TForm(Owner)
{
   g_strIniFileName="";
}
//---------------------------------------------------------------------------
void __fastcall TAMCParaForm::FormCreate(TObject *Sender)
{
   int i;
   for(i=0; i<4; i++)
   {
      t_AMCParameter[i] = new TAMCPARA;
   }
}
//---------------------------------------------------------------------------
void __fastcall TAMCParaForm::FormDestroy(TObject *Sender)
{
   int i;
   for(i=0; i<4; i++)
   {
      delete t_AMCParameter[i];
   }
}
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------
TAMCPARA::TAMCPARA()
{
   Init();
}
//----------------------------------------------------------------------------
TAMCPARA::~TAMCPARA()
{

}
//----------------------------------------------------------------------------
int TAMCPARA::Init()
{
   int nError=0;

   VPGAIN="";
   VIGAIN="";
   VDGAIN="";
   VFGAIN="";
   VILIMIT="";
   PGAIN="";
   IGAIN="";
   DGAIN="";
   FGAIN="";
   ILIMIT="";
   VEL_LIMIT="";
   ACC_LIMIT="";
   SWLOWER_LIMIT="";
   SWUPPER_LIMIT="";
   IN_POSITION="";
   ERROR_LIMIT="";
   SWUPPER_LIMIT_ST="";
   SWLOWER_LIMIT_ST="";
   POS_LEVEL="";
   NEG_LEVEL="";
   HOME_LEVEL="";
   AMP_LEVEL="";
   AMP_RESET_LEVEL="";
   ERROR_LIMIT_ST="";
   STOP_RATE="";
   ESTOP_RATE="";
   ENCODER_DIR="";
   MOTOR_PAUSE="";
   AMP_ON_LEVEL="";
   MOTOR_PAUSE_LEVEL="";
   MOTOR_PAUSE_CHECKBIT="";
   CONTROL_CFG="";
   LOOP_CFG="";
   VOLTAGE_CFG="";
   POS_IMODE="";
   VEL_IMODE="";
   PULSE_MODE="";
   VTRACKING_FACTOR="";
   ENCODER_OFFSET="";
   POS_NOTCH_FREQ="";
   POS_LPF_FREQ="";
   VEL_NOTCH_FREQ="";
   VEL_LPF_FREQ="";
   GEAR_RATIO="";
   HWLOWER_LIMIT="";
   HWUPPER_LIMIT="";
   MOTORLYPE="";
   ENCODER_CFG="";
   HOME_INDEX="";
   IO_INT_ENABLE="";
   INT_EVENT_ST="";
   AMP_FAULT_ST="";
   INPOS_LEVEL="";
   USERIO_BOOTMODE="";
   USERIO_BOOTVALUE="";
   POS_LEVEL_ST="";
   NEG_LEVEL_ST="";
   HOME_LEVEL_ST="";


   return nError;
}
//----------------------------------------------------------------------------
bool TAMCParaForm::IsNumber( String SrcString )
{
  int cnt = SrcString.Length();
  if ( cnt == 0 ) return false;

  int i;
  String tmp;
  bool _result = true;
  for ( i = 1 ; i < cnt + 1 ; i++ )
  {
    tmp = SrcString.SubString( i, 1 );
    if ( tmp < "0" || tmp > "9"  )
    {
      _result = false;
    }
    if ( tmp =="." || tmp =="+" || tmp =="-" && i == 1)
      _result = true;

  }
  return _result;
}

//----------------------------------------------------------------------------
void __fastcall TAMCParaForm::edtVEL_LIMITChange(TObject *Sender)
{
   String strData;
   String strEdtName;

   strData = dynamic_cast<TEdit*>(Sender)->Text;
   strEdtName = dynamic_cast<TEdit*>(Sender)->Name;

   if(IsNumber(strData)==false)
      dynamic_cast<TEdit*>(Sender)->Color=COLOR_RED;
   else
      dynamic_cast<TEdit*>(Sender)->Color=COLOR_YELLOW;
}
//---------------------------------------------------------------------------
int TAMCParaForm::Int_edtColor(void)
{
   edtVPGAIN->Color=COLOR_WHITE;
   edtVIGAIN->Color=COLOR_WHITE;
   edtVDGAIN->Color=COLOR_WHITE;
   edtVFGAIN->Color=COLOR_WHITE;
   edtVILIMIT->Color=COLOR_WHITE;
   edtPGAIN->Color=COLOR_WHITE;
   edtIGAIN->Color=COLOR_WHITE;
   edtDGAIN->Color=COLOR_WHITE;
   edtFGAIN->Color=COLOR_WHITE;
   edtILIMIT->Color=COLOR_WHITE;
   edtVEL_LIMIT->Color=COLOR_WHITE;
   edtACC_LIMIT->Color=COLOR_WHITE;
   edtSWLOWER_LIMIT->Color=COLOR_WHITE;
   edtSWUPPER_LIMIT->Color=COLOR_WHITE;
   edtIN_POSITION->Color=COLOR_WHITE;
   edtERROR_LIMIT->Color=COLOR_WHITE;
   edtSWUPPER_LIMIT_ST->Color=COLOR_WHITE;
   edtSWLOWER_LIMIT_ST->Color=COLOR_WHITE;
   edtPOS_LEVEL->Color=COLOR_WHITE;
   edtNEG_LEVEL->Color=COLOR_WHITE;
   edtHOME_LEVEL->Color=COLOR_WHITE;
   edtAMP_LEVEL->Color=COLOR_WHITE;
   edtAMP_RESET_LEVEL->Color=COLOR_WHITE;
   edtERROR_LIMIT_ST->Color=COLOR_WHITE;
   edtSTOP_RATE->Color=COLOR_WHITE;
   edtESTOP_RATE->Color=COLOR_WHITE;
   edtENCODER_DIR->Color=COLOR_WHITE;
   edtMOTOR_PAUSE->Color=COLOR_WHITE;
   edtAMP_ON_LEVEL->Color=COLOR_WHITE;
   edtMOTOR_PAUSE_LEVEL->Color=COLOR_WHITE;
   edtMOTOR_PAUSE_CHECKBIT->Color=COLOR_WHITE;
   edtCONTROL_CFG->Color=COLOR_WHITE;
   edtLOOP_CFG->Color=COLOR_WHITE;
   edtVOLTAGE_CFG->Color=COLOR_WHITE;
   edtPOS_IMODE->Color=COLOR_WHITE;
   edtVEL_IMODE->Color=COLOR_WHITE;
   edtPULSE_MODE->Color=COLOR_WHITE;
   edtVTRACKING_FACTOR->Color=COLOR_WHITE;
   edtENCODER_OFFSET->Color=COLOR_WHITE;
   edtPOS_NOTCH_FREQ->Color=COLOR_WHITE;
   edtPOS_LPF_FREQ->Color=COLOR_WHITE;
   edtVEL_NOTCH_FREQ->Color=COLOR_WHITE;
   edtVEL_LPF_FREQ->Color=COLOR_WHITE;
   edtGEAR_RATIO->Color=COLOR_WHITE;
   edtHWLOWER_LIMIT->Color=COLOR_WHITE;
   edtHWUPPER_LIMIT->Color=COLOR_WHITE;
   edtMOTORLYPE->Color=COLOR_WHITE;
   edtENCODER_CFG->Color=COLOR_WHITE;
   edtHOME_INDEX->Color=COLOR_WHITE;
   edtIO_INT_ENABLE->Color=COLOR_WHITE;
   edtINT_EVENT_ST->Color=COLOR_WHITE;
   edtAMP_FAULT_ST->Color=COLOR_WHITE;
   edtINPOS_LEVEL->Color=COLOR_WHITE;
   edtUSERIO_BOOTMODE->Color=COLOR_WHITE;
   edtUSERIO_BOOTVALUE->Color=COLOR_WHITE;
   edtPOS_LEVEL_ST->Color=COLOR_WHITE;
   edtNEG_LEVEL_ST->Color=COLOR_WHITE;
   edtHOME_LEVEL_ST->Color=COLOR_WHITE;

   return 0;
}

//---------------------------------------------------------------------------

int TAMCParaForm::Load_IniData(String strFileName)
{
   int nError=0;
   int i;
   String strAxis;

   if((strFileName.Length()==0)||(!FileExists(strFileName))) return nError;

   TIniFile *AMCParaIni = new TIniFile (strFileName);


   for(i=0; i<4; i++)
   {
      strAxis.sprintf(L"AXIS_%d", i+1);
      t_AMCParameter[i]->VPGAIN = AMCParaIni->ReadString(strAxis,"VPGAIN","?");
      t_AMCParameter[i]->VIGAIN =AMCParaIni->ReadString(strAxis,"VIGAIN","?");
      t_AMCParameter[i]->VDGAIN =AMCParaIni->ReadString(strAxis,"VDGAIN","?");
      t_AMCParameter[i]->VFGAIN =AMCParaIni->ReadString(strAxis,"VFGAIN","?");
      t_AMCParameter[i]->VILIMIT =AMCParaIni->ReadString(strAxis,"VILIMIT","?");
      t_AMCParameter[i]->PGAIN =AMCParaIni->ReadString(strAxis,"PGAIN","?");
      t_AMCParameter[i]->IGAIN =AMCParaIni->ReadString(strAxis,"IGAIN","?");
      t_AMCParameter[i]->DGAIN =AMCParaIni->ReadString(strAxis,"DGAIN","?");
      t_AMCParameter[i]->FGAIN =AMCParaIni->ReadString(strAxis,"FGAIN","?");
      t_AMCParameter[i]->ILIMIT =AMCParaIni->ReadString(strAxis,"ILIMIT","?");
      t_AMCParameter[i]->VEL_LIMIT =AMCParaIni->ReadString(strAxis,"VEL_LIMIT","?");
      t_AMCParameter[i]->ACC_LIMIT =AMCParaIni->ReadString(strAxis,"ACC_LIMIT","?");
      t_AMCParameter[i]->SWLOWER_LIMIT =AMCParaIni->ReadString(strAxis,"SWLOWER_LIMIT","?");
      t_AMCParameter[i]->SWUPPER_LIMIT =AMCParaIni->ReadString(strAxis,"SWUPPER_LIMIT","?");
      t_AMCParameter[i]->IN_POSITION =AMCParaIni->ReadString(strAxis,"IN_POSITION","?");
      t_AMCParameter[i]->ERROR_LIMIT =AMCParaIni->ReadString(strAxis,"ERROR_LIMIT","?");
      t_AMCParameter[i]->SWUPPER_LIMIT_ST=AMCParaIni->ReadString(strAxis,"SWUPPER_LIMIT_ST","?");
      t_AMCParameter[i]->SWLOWER_LIMIT_ST=AMCParaIni->ReadString(strAxis,"SWLOWER_LIMIT_ST","?");
      t_AMCParameter[i]->POS_LEVEL=AMCParaIni->ReadString(strAxis,"POS_LEVEL","?");
      t_AMCParameter[i]->NEG_LEVEL=AMCParaIni->ReadString(strAxis,"NEG_LEVEL","?");
      t_AMCParameter[i]->HOME_LEVEL=AMCParaIni->ReadString(strAxis,"HOME_LEVEL","?");
      t_AMCParameter[i]->AMP_LEVEL=AMCParaIni->ReadString(strAxis,"AMP_LEVEL","?");
      t_AMCParameter[i]->AMP_RESET_LEVEL=AMCParaIni->ReadString(strAxis,"AMP_RESET_LEVEL","?");
      t_AMCParameter[i]->ERROR_LIMIT_ST=AMCParaIni->ReadString(strAxis,"ERROR_LIMIT_ST","?");
      t_AMCParameter[i]->STOP_RATE=AMCParaIni->ReadString(strAxis,"STOP_RATE","?");
      t_AMCParameter[i]->ESTOP_RATE=AMCParaIni->ReadString(strAxis,"ESTOP_RATE","?");
      t_AMCParameter[i]->ENCODER_DIR=AMCParaIni->ReadString(strAxis,"ENCODER_DIR","?");
      t_AMCParameter[i]->MOTOR_PAUSE=AMCParaIni->ReadString(strAxis,"MOTOR_PAUSE","?");
      t_AMCParameter[i]->AMP_ON_LEVEL=AMCParaIni->ReadString(strAxis,"AMP_ON_LEVEL","?");
      t_AMCParameter[i]->MOTOR_PAUSE_LEVEL=AMCParaIni->ReadString(strAxis,"MOTOR_PAUSE_LEVEL","?");
      t_AMCParameter[i]->MOTOR_PAUSE_CHECKBIT=AMCParaIni->ReadString(strAxis,"MOTOR_PAUSE_CHECKBIT","?");
      t_AMCParameter[i]->CONTROL_CFG=AMCParaIni->ReadString(strAxis,"CONTROL_CFG","?");
      t_AMCParameter[i]->LOOP_CFG=AMCParaIni->ReadString(strAxis,"LOOP_CFG","?");
      t_AMCParameter[i]->VOLTAGE_CFG=AMCParaIni->ReadString(strAxis,"VOLTAGE_CFG","?");
      t_AMCParameter[i]->POS_IMODE=AMCParaIni->ReadString(strAxis,"POS_IMODE","?");
      t_AMCParameter[i]->VEL_IMODE=AMCParaIni->ReadString(strAxis,"VEL_IMODE","?");
      t_AMCParameter[i]->PULSE_MODE=AMCParaIni->ReadString(strAxis,"PULSE_MODE","?");
      t_AMCParameter[i]->VTRACKING_FACTOR=AMCParaIni->ReadString(strAxis,"VTRACKING_FACTOR","?");
      t_AMCParameter[i]->ENCODER_OFFSET=AMCParaIni->ReadString(strAxis,"ENCODER_OFFSET","?");
      t_AMCParameter[i]->POS_NOTCH_FREQ=AMCParaIni->ReadString(strAxis,"POS_NOTCH_FREQ","?");
      t_AMCParameter[i]->POS_LPF_FREQ=AMCParaIni->ReadString(strAxis,"POS_LPF_FREQ","?");
      t_AMCParameter[i]->VEL_NOTCH_FREQ=AMCParaIni->ReadString(strAxis,"VEL_NOTCH_FREQ","?");
      t_AMCParameter[i]->VEL_LPF_FREQ=AMCParaIni->ReadString(strAxis,"VEL_LPF_FREQ","?");
      t_AMCParameter[i]->GEAR_RATIO=AMCParaIni->ReadString(strAxis,"GEAR_RATIO","?");
      t_AMCParameter[i]->HWLOWER_LIMIT=AMCParaIni->ReadString(strAxis,"HWLOWER_LIMIT","?");
      t_AMCParameter[i]->HWUPPER_LIMIT=AMCParaIni->ReadString(strAxis,"HWUPPER_LIMIT","?");
      t_AMCParameter[i]->MOTORLYPE=AMCParaIni->ReadString(strAxis,"MOTORLYPE","?");
      t_AMCParameter[i]->ENCODER_CFG=AMCParaIni->ReadString(strAxis,"ENCODER_CFG","?");
      t_AMCParameter[i]->HOME_INDEX=AMCParaIni->ReadString(strAxis,"HOME_INDEX","?");
      t_AMCParameter[i]->IO_INT_ENABLE=AMCParaIni->ReadString(strAxis,"IO_INT_ENABLE","?");
      t_AMCParameter[i]->INT_EVENT_ST=AMCParaIni->ReadString(strAxis,"INT_EVENT_ST","?");
      t_AMCParameter[i]->AMP_FAULT_ST=AMCParaIni->ReadString(strAxis,"AMP_FAULT_ST","?");
      t_AMCParameter[i]->INPOS_LEVEL=AMCParaIni->ReadString(strAxis,"INPOS_LEVEL","?");
      t_AMCParameter[i]->USERIO_BOOTMODE=AMCParaIni->ReadString(strAxis,"USERIO_BOOTMODE","?");
      t_AMCParameter[i]->USERIO_BOOTVALUE=AMCParaIni->ReadString(strAxis,"USERIO_BOOTVALUE","?");
      t_AMCParameter[i]->POS_LEVEL_ST=AMCParaIni->ReadString(strAxis,"POS_LEVEL_ST","?");
      t_AMCParameter[i]->NEG_LEVEL_ST=AMCParaIni->ReadString(strAxis,"NEG_LEVEL_ST","?");
      t_AMCParameter[i]->HOME_LEVEL_ST=AMCParaIni->ReadString(strAxis,"HOME_LEVEL_ST","?");
   }


   return nError;
}
//---------------------------------------------------------------------------
int TAMCParaForm::Save_IniData(String strFileName)
{
   int nError=0;
   int i;
   String strAxis;

   if(strFileName.Length()==0) return 1;

   TIniFile *AMCParaIni = new TIniFile (strFileName);


   for(i=0; i<4; i++)
   {
      strAxis.sprintf(L"AXIS_%d", i+1);

      if(t_AMCParameter[i]->VPGAIN != "?") AMCParaIni->WriteString(strAxis,"VPGAIN"," "+t_AMCParameter[i]->VPGAIN);
      if(t_AMCParameter[i]->VIGAIN != "?") AMCParaIni->WriteString(strAxis,"VIGAIN"," "+t_AMCParameter[i]->VIGAIN);
      if(t_AMCParameter[i]->VDGAIN != "?") AMCParaIni->WriteString(strAxis,"VDGAIN"," "+t_AMCParameter[i]->VDGAIN);
      if(t_AMCParameter[i]->VFGAIN != "?") AMCParaIni->WriteString(strAxis,"VFGAIN"," "+t_AMCParameter[i]->VFGAIN);
      if(t_AMCParameter[i]->VILIMIT != "?") AMCParaIni->WriteString(strAxis,"VILIMIT"," "+t_AMCParameter[i]->VILIMIT);
      if(t_AMCParameter[i]->PGAIN != "?") AMCParaIni->WriteString(strAxis,"PGAIN"," "+t_AMCParameter[i]->PGAIN);
      if(t_AMCParameter[i]->IGAIN != "?") AMCParaIni->WriteString(strAxis,"IGAIN"," "+t_AMCParameter[i]->IGAIN);
      if(t_AMCParameter[i]->DGAIN != "?") AMCParaIni->WriteString(strAxis,"DGAIN"," "+t_AMCParameter[i]->DGAIN);
      if(t_AMCParameter[i]->FGAIN != "?") AMCParaIni->WriteString(strAxis,"FGAIN"," "+t_AMCParameter[i]->FGAIN);
      if(t_AMCParameter[i]->ILIMIT != "?") AMCParaIni->WriteString(strAxis,"ILIMIT"," "+t_AMCParameter[i]->ILIMIT);
      if(t_AMCParameter[i]->VEL_LIMIT != "?") AMCParaIni->WriteString(strAxis,"VEL_LIMIT"," "+t_AMCParameter[i]->VEL_LIMIT);
      if(t_AMCParameter[i]->ACC_LIMIT != "?") AMCParaIni->WriteString(strAxis,"ACC_LIMIT"," "+t_AMCParameter[i]->ACC_LIMIT);
      if(t_AMCParameter[i]->SWLOWER_LIMIT != "?") AMCParaIni->WriteString(strAxis,"SWLOWER_LIMIT"," "+t_AMCParameter[i]->SWLOWER_LIMIT);
      if(t_AMCParameter[i]->SWUPPER_LIMIT != "?") AMCParaIni->WriteString(strAxis,"SWUPPER_LIMIT"," "+t_AMCParameter[i]->SWUPPER_LIMIT);
      if(t_AMCParameter[i]->IN_POSITION != "?") AMCParaIni->WriteString(strAxis,"IN_POSITION"," "+t_AMCParameter[i]->IN_POSITION);
      if(t_AMCParameter[i]->ERROR_LIMIT != "?") AMCParaIni->WriteString(strAxis,"ERROR_LIMIT"," "+t_AMCParameter[i]->ERROR_LIMIT);
      if(t_AMCParameter[i]->SWUPPER_LIMIT_ST != "?") AMCParaIni->WriteString(strAxis,"SWUPPER_LIMIT_ST"," "+t_AMCParameter[i]->SWUPPER_LIMIT_ST);
      if(t_AMCParameter[i]->SWLOWER_LIMIT_ST != "?") AMCParaIni->WriteString(strAxis,"SWLOWER_LIMIT_ST"," "+t_AMCParameter[i]->SWLOWER_LIMIT_ST);
      if(t_AMCParameter[i]->POS_LEVEL != "?") AMCParaIni->WriteString(strAxis,"POS_LEVEL"," "+t_AMCParameter[i]->POS_LEVEL);
      if(t_AMCParameter[i]->NEG_LEVEL != "?") AMCParaIni->WriteString(strAxis,"NEG_LEVEL"," "+t_AMCParameter[i]->NEG_LEVEL);
      if(t_AMCParameter[i]->HOME_LEVEL != "?") AMCParaIni->WriteString(strAxis,"HOME_LEVEL"," "+t_AMCParameter[i]->HOME_LEVEL);
      if(t_AMCParameter[i]->AMP_LEVEL != "?") AMCParaIni->WriteString(strAxis,"AMP_LEVEL"," "+t_AMCParameter[i]->AMP_LEVEL);
      if(t_AMCParameter[i]->AMP_RESET_LEVEL != "?") AMCParaIni->WriteString(strAxis,"AMP_RESET_LEVEL"," "+t_AMCParameter[i]->AMP_RESET_LEVEL);
      if(t_AMCParameter[i]->ERROR_LIMIT_ST != "?") AMCParaIni->WriteString(strAxis,"ERROR_LIMIT_ST"," "+t_AMCParameter[i]->ERROR_LIMIT_ST);
      if(t_AMCParameter[i]->STOP_RATE != "?") AMCParaIni->WriteString(strAxis,"STOP_RATE"," "+t_AMCParameter[i]->STOP_RATE);
      if(t_AMCParameter[i]->ESTOP_RATE != "?") AMCParaIni->WriteString(strAxis,"ESTOP_RATE"," "+t_AMCParameter[i]->ESTOP_RATE);
      if(t_AMCParameter[i]->ENCODER_DIR != "?") AMCParaIni->WriteString(strAxis,"ENCODER_DIR"," "+t_AMCParameter[i]->ENCODER_DIR);
      if(t_AMCParameter[i]->MOTOR_PAUSE != "?") AMCParaIni->WriteString(strAxis,"MOTOR_PAUSE"," "+t_AMCParameter[i]->MOTOR_PAUSE);
      if(t_AMCParameter[i]->AMP_ON_LEVEL != "?") AMCParaIni->WriteString(strAxis,"AMP_ON_LEVEL"," "+t_AMCParameter[i]->AMP_ON_LEVEL);
      if(t_AMCParameter[i]->MOTOR_PAUSE_LEVEL != "?") AMCParaIni->WriteString(strAxis,"MOTOR_PAUSE_LEVEL"," "+t_AMCParameter[i]->MOTOR_PAUSE_LEVEL);
      if(t_AMCParameter[i]->MOTOR_PAUSE_CHECKBIT != "?") AMCParaIni->WriteString(strAxis,"MOTOR_PAUSE_CHECKBIT"," "+t_AMCParameter[i]->MOTOR_PAUSE_CHECKBIT);
      if(t_AMCParameter[i]->CONTROL_CFG != "?") AMCParaIni->WriteString(strAxis,"CONTROL_CFG"," "+t_AMCParameter[i]->CONTROL_CFG);
      if(t_AMCParameter[i]->LOOP_CFG != "?") AMCParaIni->WriteString(strAxis,"LOOP_CFG"," "+t_AMCParameter[i]->LOOP_CFG);
      if(t_AMCParameter[i]->VOLTAGE_CFG != "?") AMCParaIni->WriteString(strAxis,"VOLTAGE_CFG"," "+t_AMCParameter[i]->VOLTAGE_CFG);
      if(t_AMCParameter[i]->POS_IMODE != "?") AMCParaIni->WriteString(strAxis,"POS_IMODE"," "+t_AMCParameter[i]->POS_IMODE);
      if(t_AMCParameter[i]->VEL_IMODE != "?") AMCParaIni->WriteString(strAxis,"VEL_IMODE"," "+t_AMCParameter[i]->VEL_IMODE);
      if(t_AMCParameter[i]->PULSE_MODE != "?") AMCParaIni->WriteString(strAxis,"PULSE_MODE"," "+t_AMCParameter[i]->PULSE_MODE);
      if(t_AMCParameter[i]->VTRACKING_FACTOR != "?") AMCParaIni->WriteString(strAxis,"VTRACKING_FACTOR"," "+t_AMCParameter[i]->VTRACKING_FACTOR);
      if(t_AMCParameter[i]->ENCODER_OFFSET != "?") AMCParaIni->WriteString(strAxis,"ENCODER_OFFSET"," "+t_AMCParameter[i]->ENCODER_OFFSET);
      if(t_AMCParameter[i]->POS_NOTCH_FREQ != "?") AMCParaIni->WriteString(strAxis,"POS_NOTCH_FREQ"," "+t_AMCParameter[i]->POS_NOTCH_FREQ);
      if(t_AMCParameter[i]->POS_LPF_FREQ != "?") AMCParaIni->WriteString(strAxis,"POS_LPF_FREQ"," "+t_AMCParameter[i]->POS_LPF_FREQ);
      if(t_AMCParameter[i]->VEL_NOTCH_FREQ != "?") AMCParaIni->WriteString(strAxis,"VEL_NOTCH_FREQ"," "+t_AMCParameter[i]->VEL_NOTCH_FREQ);
      if(t_AMCParameter[i]->VEL_LPF_FREQ != "?") AMCParaIni->WriteString(strAxis,"VEL_LPF_FREQ"," "+t_AMCParameter[i]->VEL_LPF_FREQ);
      if(t_AMCParameter[i]->GEAR_RATIO != "?") AMCParaIni->WriteString(strAxis,"GEAR_RATIO"," "+t_AMCParameter[i]->GEAR_RATIO);
      if(t_AMCParameter[i]->HWLOWER_LIMIT != "?") AMCParaIni->WriteString(strAxis,"HWLOWER_LIMIT"," "+t_AMCParameter[i]->HWLOWER_LIMIT);
      if(t_AMCParameter[i]->HWUPPER_LIMIT != "?") AMCParaIni->WriteString(strAxis,"HWUPPER_LIMIT"," "+t_AMCParameter[i]->HWUPPER_LIMIT);
      if(t_AMCParameter[i]->MOTORLYPE != "?") AMCParaIni->WriteString(strAxis,"MOTORLYPE"," "+t_AMCParameter[i]->MOTORLYPE);
      if(t_AMCParameter[i]->ENCODER_CFG != "?") AMCParaIni->WriteString(strAxis,"ENCODER_CFG"," "+t_AMCParameter[i]->ENCODER_CFG);
      if(t_AMCParameter[i]->HOME_INDEX != "?") AMCParaIni->WriteString(strAxis,"HOME_INDEX"," "+t_AMCParameter[i]->HOME_INDEX);
      if(t_AMCParameter[i]->IO_INT_ENABLE != "?") AMCParaIni->WriteString(strAxis,"IO_INT_ENABLE"," "+t_AMCParameter[i]->IO_INT_ENABLE);
      if(t_AMCParameter[i]->INT_EVENT_ST != "?") AMCParaIni->WriteString(strAxis,"INT_EVENT_ST"," "+t_AMCParameter[i]->INT_EVENT_ST);
      if(t_AMCParameter[i]->AMP_FAULT_ST != "?") AMCParaIni->WriteString(strAxis,"AMP_FAULT_ST"," "+t_AMCParameter[i]->AMP_FAULT_ST);
      if(t_AMCParameter[i]->INPOS_LEVEL != "?") AMCParaIni->WriteString(strAxis,"INPOS_LEVEL"," "+t_AMCParameter[i]->INPOS_LEVEL);
      if(t_AMCParameter[i]->USERIO_BOOTMODE != "?") AMCParaIni->WriteString(strAxis,"USERIO_BOOTMODE"," "+t_AMCParameter[i]->USERIO_BOOTMODE);
      if(t_AMCParameter[i]->USERIO_BOOTVALUE != "?") AMCParaIni->WriteString(strAxis,"USERIO_BOOTVALUE"," "+t_AMCParameter[i]->USERIO_BOOTVALUE);
      if(t_AMCParameter[i]->POS_LEVEL_ST != "?") AMCParaIni->WriteString(strAxis,"POS_LEVEL_ST"," "+t_AMCParameter[i]->POS_LEVEL_ST);
      if(t_AMCParameter[i]->NEG_LEVEL_ST != "?") AMCParaIni->WriteString(strAxis,"NEG_LEVEL_ST"," "+t_AMCParameter[i]->NEG_LEVEL_ST);
      if(t_AMCParameter[i]->HOME_LEVEL_ST != "?") AMCParaIni->WriteString(strAxis,"HOME_LEVEL_ST"," "+t_AMCParameter[i]->HOME_LEVEL_ST);
   }
   return nError;


}
//---------------------------------------------------------------------------

int TAMCParaForm::Display_IniData(int i)
{
   edtVPGAIN->Text=t_AMCParameter[i]->VPGAIN;
   edtVIGAIN->Text=t_AMCParameter[i]->VIGAIN;
   edtVDGAIN->Text=t_AMCParameter[i]->VDGAIN;
   edtVFGAIN->Text=t_AMCParameter[i]->VFGAIN;
   edtVILIMIT->Text=t_AMCParameter[i]->VILIMIT;
   edtPGAIN->Text=t_AMCParameter[i]->PGAIN;
   edtIGAIN->Text=t_AMCParameter[i]->IGAIN;
   edtDGAIN->Text=t_AMCParameter[i]->DGAIN;
   edtFGAIN->Text=t_AMCParameter[i]->FGAIN;
   edtILIMIT->Text=t_AMCParameter[i]->ILIMIT;
   edtVEL_LIMIT->Text=t_AMCParameter[i]->VEL_LIMIT;
   edtACC_LIMIT->Text=t_AMCParameter[i]->ACC_LIMIT;
   edtSWLOWER_LIMIT->Text=t_AMCParameter[i]->SWLOWER_LIMIT;
   edtSWUPPER_LIMIT->Text=t_AMCParameter[i]->SWUPPER_LIMIT;
   edtIN_POSITION->Text=t_AMCParameter[i]->IN_POSITION;
   edtERROR_LIMIT->Text=t_AMCParameter[i]->ERROR_LIMIT;
   edtSWUPPER_LIMIT_ST->Text=t_AMCParameter[i]->SWUPPER_LIMIT_ST;
   edtSWLOWER_LIMIT_ST->Text=t_AMCParameter[i]->SWLOWER_LIMIT_ST;
   edtPOS_LEVEL->Text=t_AMCParameter[i]->POS_LEVEL;
   edtNEG_LEVEL->Text=t_AMCParameter[i]->NEG_LEVEL;
   edtHOME_LEVEL->Text=t_AMCParameter[i]->HOME_LEVEL;
   edtAMP_LEVEL->Text=t_AMCParameter[i]->AMP_LEVEL;
   edtAMP_RESET_LEVEL->Text=t_AMCParameter[i]->AMP_RESET_LEVEL;
   edtERROR_LIMIT_ST->Text=t_AMCParameter[i]->ERROR_LIMIT_ST;
   edtSTOP_RATE->Text=t_AMCParameter[i]->STOP_RATE;
   edtESTOP_RATE->Text=t_AMCParameter[i]->ESTOP_RATE;
   edtENCODER_DIR->Text=t_AMCParameter[i]->ENCODER_DIR;
   edtMOTOR_PAUSE->Text=t_AMCParameter[i]->MOTOR_PAUSE;
   edtAMP_ON_LEVEL->Text=t_AMCParameter[i]->AMP_ON_LEVEL;
   edtMOTOR_PAUSE_LEVEL->Text=t_AMCParameter[i]->MOTOR_PAUSE_LEVEL;
   edtMOTOR_PAUSE_CHECKBIT->Text=t_AMCParameter[i]->MOTOR_PAUSE_CHECKBIT;
   edtCONTROL_CFG->Text=t_AMCParameter[i]->CONTROL_CFG;
   edtLOOP_CFG->Text=t_AMCParameter[i]->LOOP_CFG;
   edtVOLTAGE_CFG->Text=t_AMCParameter[i]->VOLTAGE_CFG;
   edtPOS_IMODE->Text=t_AMCParameter[i]->POS_IMODE;
   edtVEL_IMODE->Text=t_AMCParameter[i]->VEL_IMODE;
   edtPULSE_MODE->Text=t_AMCParameter[i]->PULSE_MODE;
   edtVTRACKING_FACTOR->Text=t_AMCParameter[i]->VTRACKING_FACTOR;
   edtENCODER_OFFSET->Text=t_AMCParameter[i]->ENCODER_OFFSET;
   edtPOS_NOTCH_FREQ->Text=t_AMCParameter[i]->POS_NOTCH_FREQ;
   edtPOS_LPF_FREQ->Text=t_AMCParameter[i]->POS_LPF_FREQ;
   edtVEL_NOTCH_FREQ->Text=t_AMCParameter[i]->VEL_NOTCH_FREQ;
   edtVEL_LPF_FREQ->Text=t_AMCParameter[i]->VEL_LPF_FREQ;
   edtGEAR_RATIO->Text=t_AMCParameter[i]->GEAR_RATIO;
   edtHWLOWER_LIMIT->Text=t_AMCParameter[i]->HWLOWER_LIMIT;
   edtHWUPPER_LIMIT->Text=t_AMCParameter[i]->HWUPPER_LIMIT;
   edtMOTORLYPE->Text=t_AMCParameter[i]->MOTORLYPE;
   edtENCODER_CFG->Text=t_AMCParameter[i]->ENCODER_CFG;
   edtHOME_INDEX->Text=t_AMCParameter[i]->HOME_INDEX;
   edtIO_INT_ENABLE->Text=t_AMCParameter[i]->IO_INT_ENABLE;
   edtINT_EVENT_ST->Text=t_AMCParameter[i]->INT_EVENT_ST;
   edtAMP_FAULT_ST->Text=t_AMCParameter[i]->AMP_FAULT_ST;
   edtINPOS_LEVEL->Text=t_AMCParameter[i]->INPOS_LEVEL;
   edtUSERIO_BOOTMODE->Text=t_AMCParameter[i]->USERIO_BOOTMODE;
   edtUSERIO_BOOTVALUE->Text=t_AMCParameter[i]->USERIO_BOOTVALUE;
   edtPOS_LEVEL_ST->Text=t_AMCParameter[i]->POS_LEVEL_ST;
   edtNEG_LEVEL_ST->Text=t_AMCParameter[i]->NEG_LEVEL_ST;
   edtHOME_LEVEL_ST->Text=t_AMCParameter[i]->HOME_LEVEL_ST;

   return 0;
}
//---------------------------------------------------------------------------
int TAMCParaForm::Set_Display_IniData(int i)
{
   if(edtVPGAIN->Color!=COLOR_RED) t_AMCParameter[i]->VPGAIN=edtVPGAIN->Text;
   if(edtVIGAIN->Color!=COLOR_RED) t_AMCParameter[i]->VIGAIN=edtVIGAIN->Text;
   if(edtVDGAIN->Color!=COLOR_RED) t_AMCParameter[i]->VDGAIN=edtVDGAIN->Text;
   if(edtVFGAIN->Color!=COLOR_RED) t_AMCParameter[i]->VFGAIN=edtVFGAIN->Text;
   if(edtVILIMIT->Color!=COLOR_RED) t_AMCParameter[i]->VILIMIT=edtVILIMIT->Text;
   if(edtPGAIN->Color!=COLOR_RED) t_AMCParameter[i]->PGAIN=edtPGAIN->Text;
   if(edtIGAIN->Color!=COLOR_RED) t_AMCParameter[i]->IGAIN=edtIGAIN->Text;
   if(edtDGAIN->Color!=COLOR_RED) t_AMCParameter[i]->DGAIN=edtDGAIN->Text;
   if(edtFGAIN->Color!=COLOR_RED) t_AMCParameter[i]->FGAIN=edtFGAIN->Text;
   if(edtILIMIT->Color!=COLOR_RED) t_AMCParameter[i]->ILIMIT=edtILIMIT->Text;
   if(edtVEL_LIMIT->Color!=COLOR_RED) t_AMCParameter[i]->VEL_LIMIT=edtVEL_LIMIT->Text;
   if(edtACC_LIMIT->Color!=COLOR_RED) t_AMCParameter[i]->ACC_LIMIT=edtACC_LIMIT->Text;
   if(edtSWLOWER_LIMIT->Color!=COLOR_RED) t_AMCParameter[i]->SWLOWER_LIMIT=edtSWLOWER_LIMIT->Text;
   if(edtSWUPPER_LIMIT->Color!=COLOR_RED) t_AMCParameter[i]->SWUPPER_LIMIT=edtSWUPPER_LIMIT->Text;
   if(edtIN_POSITION->Color!=COLOR_RED) t_AMCParameter[i]->IN_POSITION=edtIN_POSITION->Text;
   if(edtERROR_LIMIT->Color!=COLOR_RED) t_AMCParameter[i]->ERROR_LIMIT=edtERROR_LIMIT->Text;
   if(edtSWUPPER_LIMIT_ST->Color!=COLOR_RED) t_AMCParameter[i]->SWUPPER_LIMIT_ST=edtSWUPPER_LIMIT_ST->Text;
   if(edtSWLOWER_LIMIT_ST->Color!=COLOR_RED) t_AMCParameter[i]->SWLOWER_LIMIT_ST=edtSWLOWER_LIMIT_ST->Text;
   if(edtPOS_LEVEL->Color!=COLOR_RED) t_AMCParameter[i]->POS_LEVEL=edtPOS_LEVEL->Text;
   if(edtNEG_LEVEL->Color!=COLOR_RED) t_AMCParameter[i]->NEG_LEVEL=edtNEG_LEVEL->Text;
   if(edtHOME_LEVEL->Color!=COLOR_RED) t_AMCParameter[i]->HOME_LEVEL=edtHOME_LEVEL->Text;
   if(edtAMP_LEVEL->Color!=COLOR_RED) t_AMCParameter[i]->AMP_LEVEL=edtAMP_LEVEL->Text;
   if(edtAMP_RESET_LEVEL->Color!=COLOR_RED) t_AMCParameter[i]->AMP_RESET_LEVEL=edtAMP_RESET_LEVEL->Text;
   if(edtERROR_LIMIT_ST->Color!=COLOR_RED) t_AMCParameter[i]->ERROR_LIMIT_ST=edtERROR_LIMIT_ST->Text;
   if(edtSTOP_RATE->Color!=COLOR_RED) t_AMCParameter[i]->STOP_RATE=edtSTOP_RATE->Text;
   if(edtESTOP_RATE->Color!=COLOR_RED) t_AMCParameter[i]->ESTOP_RATE=edtESTOP_RATE->Text;
   if(edtENCODER_DIR->Color!=COLOR_RED) t_AMCParameter[i]->ENCODER_DIR=edtENCODER_DIR->Text;
   if(edtMOTOR_PAUSE->Color!=COLOR_RED) t_AMCParameter[i]->MOTOR_PAUSE=edtMOTOR_PAUSE->Text;
   if(edtAMP_ON_LEVEL->Color!=COLOR_RED) t_AMCParameter[i]->AMP_ON_LEVEL=edtAMP_ON_LEVEL->Text;
   if(edtMOTOR_PAUSE_LEVEL->Color!=COLOR_RED) t_AMCParameter[i]->MOTOR_PAUSE_LEVEL=edtMOTOR_PAUSE_LEVEL->Text;
   if(edtMOTOR_PAUSE_CHECKBIT->Color!=COLOR_RED) t_AMCParameter[i]->MOTOR_PAUSE_CHECKBIT=edtMOTOR_PAUSE_CHECKBIT->Text;
   if(edtCONTROL_CFG->Color!=COLOR_RED) t_AMCParameter[i]->CONTROL_CFG=edtCONTROL_CFG->Text;
   if(edtLOOP_CFG->Color!=COLOR_RED) t_AMCParameter[i]->LOOP_CFG=edtLOOP_CFG->Text;
   if(edtVOLTAGE_CFG->Color!=COLOR_RED) t_AMCParameter[i]->VOLTAGE_CFG=edtVOLTAGE_CFG->Text;
   if(edtPOS_IMODE->Color!=COLOR_RED) t_AMCParameter[i]->POS_IMODE=edtPOS_IMODE->Text;
   if(edtVEL_IMODE->Color!=COLOR_RED) t_AMCParameter[i]->VEL_IMODE=edtVEL_IMODE->Text;
   if(edtPULSE_MODE->Color!=COLOR_RED) t_AMCParameter[i]->PULSE_MODE=edtPULSE_MODE->Text;
   if(edtVTRACKING_FACTOR->Color!=COLOR_RED) t_AMCParameter[i]->VTRACKING_FACTOR=edtVTRACKING_FACTOR->Text;
   if(edtENCODER_OFFSET->Color!=COLOR_RED) t_AMCParameter[i]->ENCODER_OFFSET=edtENCODER_OFFSET->Text;
   if(edtPOS_NOTCH_FREQ->Color!=COLOR_RED) t_AMCParameter[i]->POS_NOTCH_FREQ=edtPOS_NOTCH_FREQ->Text;
   if(edtPOS_LPF_FREQ->Color!=COLOR_RED) t_AMCParameter[i]->POS_LPF_FREQ=edtPOS_LPF_FREQ->Text;
   if(edtVEL_NOTCH_FREQ->Color!=COLOR_RED) t_AMCParameter[i]->VEL_NOTCH_FREQ=edtVEL_NOTCH_FREQ->Text;
   if(edtVEL_LPF_FREQ->Color!=COLOR_RED) t_AMCParameter[i]->VEL_LPF_FREQ=edtVEL_LPF_FREQ->Text;
   if(edtGEAR_RATIO->Color!=COLOR_RED) t_AMCParameter[i]->GEAR_RATIO=edtGEAR_RATIO->Text;
   if(edtHWLOWER_LIMIT->Color!=COLOR_RED) t_AMCParameter[i]->HWLOWER_LIMIT=edtHWLOWER_LIMIT->Text;
   if(edtHWUPPER_LIMIT->Color!=COLOR_RED) t_AMCParameter[i]->HWUPPER_LIMIT=edtHWUPPER_LIMIT->Text;
   if(edtMOTORLYPE->Color!=COLOR_RED) t_AMCParameter[i]->MOTORLYPE=edtMOTORLYPE->Text;
   if(edtENCODER_CFG->Color!=COLOR_RED) t_AMCParameter[i]->ENCODER_CFG=edtENCODER_CFG->Text;
   if(edtHOME_INDEX->Color!=COLOR_RED) t_AMCParameter[i]->HOME_INDEX=edtHOME_INDEX->Text;
   if(edtIO_INT_ENABLE->Color!=COLOR_RED) t_AMCParameter[i]->IO_INT_ENABLE=edtIO_INT_ENABLE->Text;
   if(edtINT_EVENT_ST->Color!=COLOR_RED) t_AMCParameter[i]->INT_EVENT_ST=edtINT_EVENT_ST->Text;
   if(edtAMP_FAULT_ST->Color!=COLOR_RED) t_AMCParameter[i]->AMP_FAULT_ST=edtAMP_FAULT_ST->Text;
   if(edtINPOS_LEVEL->Color!=COLOR_RED) t_AMCParameter[i]->INPOS_LEVEL=edtINPOS_LEVEL->Text;
   if(edtUSERIO_BOOTMODE->Color!=COLOR_RED) t_AMCParameter[i]->USERIO_BOOTMODE=edtUSERIO_BOOTMODE->Text;
   if(edtUSERIO_BOOTVALUE->Color!=COLOR_RED) t_AMCParameter[i]->USERIO_BOOTVALUE=edtUSERIO_BOOTVALUE->Text;
   if(edtPOS_LEVEL_ST->Color!=COLOR_RED) t_AMCParameter[i]->POS_LEVEL_ST=edtPOS_LEVEL_ST->Text;
   if(edtNEG_LEVEL_ST->Color!=COLOR_RED) t_AMCParameter[i]->NEG_LEVEL_ST=edtNEG_LEVEL_ST->Text;
   if(edtHOME_LEVEL_ST->Color!=COLOR_RED) t_AMCParameter[i]->HOME_LEVEL_ST=edtHOME_LEVEL_ST->Text;


   return 0;

}
//---------------------------------------------------------------------------
void __fastcall TAMCParaForm::ComboBox2Change(TObject *Sender)
{
   static int nPreIndex=-1;

   if((nPreIndex!=-1)&&(nPreIndex!=ComboBox2->ItemIndex)) Set_Display_IniData(nPreIndex);
   Display_IniData(ComboBox2->ItemIndex);
   Int_edtColor();
   nPreIndex = ComboBox2->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TAMCParaForm::btnOpenClick(TObject *Sender)
{
   String strTmpFilter;
   strTmpFilter = "AMCPara Data(*.ini)|+;*.ini";
   OpenDialog2->Filter = strTmpFilter;
   OpenDialog2->FileName="Select Data";

   g_strIniFileName="";
   if(OpenDialog2->Execute())
   {
      g_strIniFileName = OpenDialog2->FileName;
      AMCParaForm->Caption = g_strIniFileName;
      Load_IniData(g_strIniFileName);
      ComboBox2->ItemIndex=0;
      Display_IniData(0);
      Int_edtColor();

   }
   
}
//---------------------------------------------------------------------------

void __fastcall TAMCParaForm::btnSaveClick(TObject *Sender)
{
   int nError=0;
   if(g_strIniFileName=="") return;
   if(!FileExists(g_strIniFileName)) return;
   if(ComboBox2->ItemIndex!=-1) Set_Display_IniData(ComboBox2->ItemIndex);
   nError = Save_IniData(g_strIniFileName);
   if(nError==0) Int_edtColor();   
}
//---------------------------------------------------------------------------

void __fastcall TAMCParaForm::btnSaveAsClick(TObject *Sender)
{
   int nError=0;
   String strTmpFilter;
   strTmpFilter = "AMCPara Data(*.ini)|+;*.ini";
   SaveDialog1->Filter = strTmpFilter;
   SaveDialog1->FileName="AMCPARAM.INI";
   String strToFile="";

   if(g_strIniFileName=="") return;
   if(ComboBox2->ItemIndex!=-1) Set_Display_IniData(ComboBox2->ItemIndex);
   if(SaveDialog1->Execute())
   {
      strToFile=SaveDialog1->FileName;
      if(g_strIniFileName != strToFile)
      {
         if(CopyFile(g_strIniFileName.w_str(),strToFile.w_str(),FALSE)!=0)
         {
            g_strIniFileName=strToFile;
            AMCParaForm->Caption = g_strIniFileName;
            nError = Save_IniData(g_strIniFileName);
         }
      }
      else
      {
         g_strIniFileName=strToFile;
         AMCParaForm->Caption = g_strIniFileName;
         nError = Save_IniData(g_strIniFileName);
      }
   }
   if(nError==0) Int_edtColor();   
}
//---------------------------------------------------------------------------


