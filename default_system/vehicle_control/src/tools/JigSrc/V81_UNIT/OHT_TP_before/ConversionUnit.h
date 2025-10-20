//---------------------------------------------------------------------------

#ifndef ConversionUnitH
#define ConversionUnitH
//---------------------------------------------------------------------------
class TCONVERSION
{
 public :
   TCONVERSION();
   ~TCONVERSION();

   double MM_to_CTS(double dMM, double dCount_1Rotate, double dDist_1Rotate_mm);
   double CTS_to_MM(double dCTS, double dCount_1Rotate, double dDist_1Rotate_mm);

   double CTSperSEC_to_CTSperMSEC(double dCTSperSEC);
   double CTSperMSEC_to_CTSperSEC(double dCTSperMSEC);

   double RPM_to_CTSperSEC(double drpm, double dCount_1Rotate);
   double RPM_to_CTSperMSEC(double drpm, double dCount_1Rotate);
   double RPM_to_MperSEC(double drpm, double dDist_1Rotate_mm);

   double CTSperSEC_to_RPM(double dCTSperSEC, double dCount_1Rotate);
   double CTSperMSEC_to_RPM(double dCTSperMSEC, double dCount_1Rotate);
   double MperSEC_to_RPM(double dMperSEC, double dDist_1Rotate_mm);

   double MperSEC_to_CTSperSEC(double dMperSEC, double dCount_1Rotate, double dDist_1Rotate_mm);
   double MperSEC_to_CTSperMSEC(double dMperSEC, double dCount_1Rotate, double dDist_1Rotate_mm);

   double CTSperSEC_to_MperSEC(double dCTSperSEC, double dCount_1Rotate, double dDist_1Rotate_mm);
   double CTSperMSEC_to_MperSEC(double dCTSperMSEC, double dCount_1Rotate, double dDist_1Rotate_mm);

   double MperSec2_to_CTSperSEC2(double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm);
   double MperSec2_to_CTSperMSEC2(double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm);

   double CTSperSEC2_to_MperSec2(double dCTSperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm);
   double CTSperMSEC2_to_MperSec2(double dCTSperMSEC2, double dCount_1Rotate, double dDist_1Rotate_mm);

   double GetTimeMSEC_CTSperMSEC_MperSEC2(double dCTSperMSEC, double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm);
   double GetTimeSEC_CTSperMSEC_MperSEC2(double dCTSperMSEC, double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm);

   double GetTimeMSEC_CTSperSEC_MperSEC2(double dCTSperSEC, double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm);
   double GetTimeSEC_CTSperSEC_MperSEC2(double dCTSperSEC, double dMperSEC2, double dCount_1Rotate, double dDist_1Rotate_mm);

};

#endif
