//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ConversionUnit.h"
//---------------------------------------------------------------------------
/*****
 RPM        CTS/SEC            CTS/MSEC
 -----------------------------------------
 0          0.000              0.000
 -----------------------------------------
 100        13653.333          13.653
 -----------------------------------------
 200        27306.667          27.307
 -----------------------------------------
 300        40960.000          40.960
 -----------------------------------------
 400        54613.333          54.613
 -----------------------------------------
 500        68266.667          68.267
 -----------------------------------------
 600        81920.000          81.920
 -----------------------------------------
 700        95573.333          95.573
 -----------------------------------------
 800        109226.667         109.227
 -----------------------------------------
 900        122880.000         122.880
 -----------------------------------------
 1000       136533.333         136.533
 -----------------------------------------
 1100       150186.667         150.187
 -----------------------------------------
 1200       163840.000         163.840
 -----------------------------------------
 1300       177493.333         177.493
 -----------------------------------------
 1400       191146.667         191.147
 -----------------------------------------
 1500       204800.000         204.800
 -----------------------------------------
 1600       218453.333         218.453
 -----------------------------------------
 1700       232106.667         232.107
 -----------------------------------------
 1800       245760.000         245.760
 -----------------------------------------
 1900       259413.333         259.413
 -----------------------------------------
 2000       273066.667         273.067
 -----------------------------------------
 2100       286720.000         286.720
 -----------------------------------------
 2200       300373.333         300.373
 -----------------------------------------
 2300       314026.667         314.027
 -----------------------------------------
 2400       327680.000         327.680
 -----------------------------------------
 2500       341333.333         341.333
 -----------------------------------------
 2600       354986.667         354.987
 -----------------------------------------
 2700       368640.000         368.640
 -----------------------------------------
 2800       382293.333         382.293
 -----------------------------------------
 2900       395946.667         395.947
 -----------------------------------------
 3000       409600.000         409.600
*****/
//---------------------------------------------------------------------------
TCONVERSION::TCONVERSION()
{
}

//---------------------------------------------------------------------------
TCONVERSION::~TCONVERSION()
{
}
//---------------------------------------------------------------------------
//#define  DIST_1ROTATE_MM      65.68    //[mm]
//#define  DIST_1ROTATE_CM      6.568    //[cm]
//#define  COUNT_1ROTATE        8192.0   //[cts]
//---------------------------------------------------------------------------
double TCONVERSION::MM_to_CTS(double dMM)
{
   return (dMM*COUNT_1ROTATE/DIST_1ROTATE_MM);
}
//---------------------------------------------------------------------------
double TCONVERSION::CTS_to_MM(double dCTS)
{
   return (dCTS*DIST_1ROTATE_MM/COUNT_1ROTATE);
}
//---------------------------------------------------------------------------
double TCONVERSION::CTSperSEC_to_CTSperMSEC(double dCTSperSEC)
{
   return (dCTSperSEC/1000.0);
}
//---------------------------------------------------------------------------
double TCONVERSION::CTSperMSEC_to_CTSperSEC(double dCTSperMSEC)
{
   return (dCTSperMSEC*1000.0);
}
//---------------------------------------------------------------------------
double TCONVERSION::RPM_to_CTSperSEC(double drpm)
{
   return (drpm*COUNT_1ROTATE/60.0);
}
//---------------------------------------------------------------------------
double TCONVERSION::RPM_to_CTSperMSEC(double drpm)
{
   return (drpm*COUNT_1ROTATE/(60.0*1000.0));
}
//---------------------------------------------------------------------------
double TCONVERSION::RPM_to_MperSEC(double drpm)
{
   return (drpm*DIST_1ROTATE_MM/(60.0*1000.0));
}
//---------------------------------------------------------------------------
double TCONVERSION::CTSperSEC_to_RPM(double dCTSperSEC)
{
   return (dCTSperSEC*60.0/COUNT_1ROTATE);
}
//---------------------------------------------------------------------------
double TCONVERSION::CTSperMSEC_to_RPM(double dCTSperMSEC)
{
   return (dCTSperMSEC*60.0*1000.0/COUNT_1ROTATE);
}
//---------------------------------------------------------------------------
double TCONVERSION::MperSEC_to_RPM(double dMperSEC)
{
   return (dMperSEC*60.0*1000.0/DIST_1ROTATE_MM);
}
//---------------------------------------------------------------------------
double TCONVERSION::MperSEC_to_CTSperSEC(double dMperSEC)
{
   return (dMperSEC*COUNT_1ROTATE*1000.0/DIST_1ROTATE_MM);
}
//---------------------------------------------------------------------------
double TCONVERSION::MperSEC_to_CTSperMSEC(double dMperSEC)
{
   return (dMperSEC*COUNT_1ROTATE/DIST_1ROTATE_MM);
}
//---------------------------------------------------------------------------
double TCONVERSION::CTSperSEC_to_MperSEC(double dCTSperSEC)
{
   return (dCTSperSEC*DIST_1ROTATE_MM/(COUNT_1ROTATE*1000.0));
}
//---------------------------------------------------------------------------
double TCONVERSION::CTSperMSEC_to_MperSEC(double dCTSperMSEC)
{
   return (dCTSperMSEC*DIST_1ROTATE_MM/COUNT_1ROTATE);
}
//---------------------------------------------------------------------------
double TCONVERSION::MperSec2_to_CTSperSEC2(double dMperSEC2)
{
   return (dMperSEC2*COUNT_1ROTATE*1000.0/DIST_1ROTATE_MM);
}
//---------------------------------------------------------------------------
double TCONVERSION::MperSec2_to_CTSperMSEC2(double dMperSEC2)
{
   return (dMperSEC2*COUNT_1ROTATE/(DIST_1ROTATE_MM*1000.0));
}
//---------------------------------------------------------------------------
double TCONVERSION::CTSperSEC2_to_MperSec2(double dCTSperSEC2)
{
   return (dCTSperSEC2*DIST_1ROTATE_MM/(COUNT_1ROTATE*1000.0));
}
//---------------------------------------------------------------------------
double TCONVERSION::CTSperMSEC2_to_MperSec2(double dCTSperMSEC2)
{
   return (dCTSperMSEC2*DIST_1ROTATE_MM*1000.0/COUNT_1ROTATE);
}
//---------------------------------------------------------------------------
double TCONVERSION::GetTimeMSEC_CTSperMSEC_MperSEC2(double dCTSperMSEC, double dMperSEC2)
{
   return (dCTSperMSEC*DIST_1ROTATE_MM*1000.0/(dMperSEC2*COUNT_1ROTATE));
}
//---------------------------------------------------------------------------
double TCONVERSION::GetTimeSEC_CTSperMSEC_MperSEC2(double dCTSperMSEC, double dMperSEC2)
{
   return (dCTSperMSEC*DIST_1ROTATE_MM/(dMperSEC2*COUNT_1ROTATE));
}
//---------------------------------------------------------------------------
double TCONVERSION::GetTimeMSEC_CTSperSEC_MperSEC2(double dCTSperSEC, double dMperSEC2)
{
   return (dCTSperSEC*DIST_1ROTATE_MM/(dMperSEC2*COUNT_1ROTATE));
}
//---------------------------------------------------------------------------
double TCONVERSION::GetTimeSEC_CTSperSEC_MperSEC2(double dCTSperSEC, double dMperSEC2)
{
   return (dCTSperSEC*DIST_1ROTATE_MM/(dMperSEC2*COUNT_1ROTATE*1000.0));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#pragma package(smart_init)
