// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ConversionUnit.h"
#include "Define.h"

// ---------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------
TCONVERSION::TCONVERSION()
{
}

// ---------------------------------------------------------------------------
TCONVERSION::~TCONVERSION()
{
}

// ---------------------------------------------------------------------------
double TCONVERSION::MM_to_CTS(double dMM, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dMM * dCount_1Rotate / dDist_1Rotate_mm);
}

// ---------------------------------------------------------------------------
double TCONVERSION::CTS_to_MM(double dCTS, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dCTS * dDist_1Rotate_mm / dCount_1Rotate);
}

// ---------------------------------------------------------------------------
double TCONVERSION::CTSperSEC_to_CTSperMSEC(double dCTSperSEC)
{
    return (dCTSperSEC / 1000.0);
}

// ---------------------------------------------------------------------------
double TCONVERSION::CTSperMSEC_to_CTSperSEC(double dCTSperMSEC)
{
    return (dCTSperMSEC * 1000.0);
}

// ---------------------------------------------------------------------------
double TCONVERSION::RPM_to_CTSperSEC(double drpm, double dCount_1Rotate)
{
    return (drpm * dCount_1Rotate / 60.0);
}

// ---------------------------------------------------------------------------
double TCONVERSION::RPM_to_CTSperMSEC(double drpm, double dCount_1Rotate)
{
    return (drpm * dCount_1Rotate / (60.0 * 1000.0));
}

// ---------------------------------------------------------------------------
double TCONVERSION::RPM_to_MperSEC(double drpm, double dDist_1Rotate_mm)
{
    return (drpm * dDist_1Rotate_mm / (60.0 * 1000.0));
}

// ---------------------------------------------------------------------------
double TCONVERSION::CTSperSEC_to_RPM(double dCTSperSEC, double dCount_1Rotate)
{
    return (dCTSperSEC * 60.0 / dCount_1Rotate);
}

// ---------------------------------------------------------------------------
double TCONVERSION::CTSperMSEC_to_RPM(double dCTSperMSEC, double dCount_1Rotate)
{
    return (dCTSperMSEC * 60.0 * 1000.0 / dCount_1Rotate);
}

// ---------------------------------------------------------------------------
double TCONVERSION::MperSEC_to_RPM(double dMperSEC, double dDist_1Rotate_mm)
{
    return (dMperSEC * 60.0 * 1000.0 / dDist_1Rotate_mm);
}

// ---------------------------------------------------------------------------
double TCONVERSION::MperSEC_to_CTSperSEC(double dMperSEC, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dMperSEC * dCount_1Rotate * 1000.0 / dDist_1Rotate_mm);
}

// ---------------------------------------------------------------------------
double TCONVERSION::MperSEC_to_CTSperMSEC(double dMperSEC, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dMperSEC * dCount_1Rotate / dDist_1Rotate_mm);
}

// ---------------------------------------------------------------------------
double TCONVERSION::CTSperSEC_to_MperSEC(double dCTSperSEC, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dCTSperSEC * dDist_1Rotate_mm / (dCount_1Rotate * 1000.0));
}

// ---------------------------------------------------------------------------
double TCONVERSION::CTSperMSEC_to_MperSEC(double dCTSperMSEC, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dCTSperMSEC * dDist_1Rotate_mm / dCount_1Rotate);
}

// ---------------------------------------------------------------------------
double TCONVERSION::MperSec2_to_CTSperSEC2(double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dMperSEC2 * dCount_1Rotate * 1000.0 / dDist_1Rotate_mm);
}

// ---------------------------------------------------------------------------
double TCONVERSION::MperSec2_to_CTSperMSEC2(double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dMperSEC2 * dCount_1Rotate / (dDist_1Rotate_mm * 1000.0));
}

// ---------------------------------------------------------------------------
double TCONVERSION::CTSperSEC2_to_MperSec2(double dCTSperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dCTSperSEC2 * dDist_1Rotate_mm / (dCount_1Rotate * 1000.0));
}

// ---------------------------------------------------------------------------
double TCONVERSION::CTSperMSEC2_to_MperSec2(double dCTSperMSEC2, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dCTSperMSEC2 * dDist_1Rotate_mm * 1000.0 / dCount_1Rotate);
}

// ---------------------------------------------------------------------------
double TCONVERSION::GetTimeMSEC_CTSperMSEC_MperSEC2(double dCTSperMSEC, double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dCTSperMSEC * dDist_1Rotate_mm * 1000.0 / (dMperSEC2 * dCount_1Rotate));
}

// ---------------------------------------------------------------------------
double TCONVERSION::GetTimeSEC_CTSperMSEC_MperSEC2(double dCTSperMSEC, double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dCTSperMSEC * dDist_1Rotate_mm / (dMperSEC2 * dCount_1Rotate));
}

// ---------------------------------------------------------------------------
double TCONVERSION::GetTimeMSEC_CTSperSEC_MperSEC2(double dCTSperSEC, double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dCTSperSEC * dDist_1Rotate_mm / (dMperSEC2 * dCount_1Rotate));
}

// ---------------------------------------------------------------------------
double TCONVERSION::GetTimeSEC_CTSperSEC_MperSEC2(double dCTSperSEC, double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm)
{
    return (dCTSperSEC * dDist_1Rotate_mm / (dMperSEC2 * dCount_1Rotate * 1000.0));
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#pragma package(smart_init)
