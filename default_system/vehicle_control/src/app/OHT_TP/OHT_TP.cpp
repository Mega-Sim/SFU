// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
// ---------------------------------------------------------------------------
USEFORM("MainUnit.cpp", MainForm);
USEFORM("MapMakerUnit.cpp", MapMakerForm);
USEFORM("MaskOnlineCMDUnit.cpp", MaskOnlineCMDForm);
USEFORM("M1VelTimeUnit.cpp", M1VelForm);
USEFORM("M24VelTimeUnit.cpp", M24VelForm);
USEFORM("M2AUnit.cpp", M2AForm);
USEFORM("MultiJogDistUnit.cpp", MultiJogDistForm);
USEFORM("OHTSysInfoUnit.cpp", SysInfoForm);
USEFORM("MoveHoistUnit.cpp", MHoistForm);
USEFORM("IOMapUnit.cpp", IOMapForm);
USEFORM("LogDownUnit.cpp", LogDownForm);
USEFORM("LogFileUnit.cpp", LogFileForm);
USEFORM("LongRunUnit.cpp", LongRunForm);
USEFORM("IOTest.cpp", IOTestForm);
USEFORM("LocMSave.cpp", MoveForm);
USEFORM("TransUnit.cpp", TransForm);
USEFORM("UDP.cpp", frmUDP);
USEFORM("UserSelectUnit.cpp", UserSelectForm);
USEFORM("StatusUnit.cpp", StatusForm);
USEFORM("TeachingUnit.cpp", TeachingForm);
USEFORM("TimeSettingUnit.cpp", TimeSettingForm);
USEFORM("VibRouteSetUnit.cpp", VibRouteSet);
USEFORM("WaitUnit.cpp", WaitForm);
USEFORM("WarnningUnit.cpp", WarnningForm);
USEFORM("VelGraphUnit.cpp", VelGraphForm);
USEFORM("VibrationMeasureUnit.cpp", frmVibrationMeasurement);
USEFORM("OptionUnit.cpp", OptionForm);
USEFORM("PIOUnit.cpp", PIOForm);
USEFORM("ProcessUnit.cpp", ProcessForm);
USEFORM("Option.cpp", frmOption);
USEFORM("SetCurnodeNUnit.cpp", SetCurForm);
USEFORM("SocketUnit.cpp", SocketForm);
USEFORM("SpeedHelpUnit.cpp", SpeedHelpForm);
USEFORM("RebootUnit.cpp", RebootForm);
USEFORM("RotateOriginUnit.cpp", RotateOrgForm);
USEFORM("SensorSettingUnit.cpp", OBS_UBG_Form);
USEFORM("HoistParamUnit.cpp", HoistParamForm);
USEFORM("APTimeUnit.cpp", APTimeForm);
USEFORM("AssistantUnit.cpp", AssistantForm);
USEFORM("AutoBayTestMapViewLoad.cpp", frm_MapViewLoad);
USEFORM("AmcDebugMain.cpp", AmcDebugForm);
USEFORM("AMCParaUnit.cpp", AMCParaForm);
USEFORM("APMonitoringUnit.cpp", APMonitoringForm);
USEFORM("CheckerUnit.cpp", UnitChecker);
USEFORM("CidCheckerUnit.cpp", frmCidChecker);
USEFORM("AutoBayTestRouteRequestUnit.cpp", frm_RouteRequest);
USEFORM("AutoBayTestUnit.cpp", frm_AutoBayTest);
USEFORM("BayTestUnit.cpp", frm_Bay_Test);
USEFORM("About.cpp", AboutBox);
USEFORM("FileUploadAlarm.cpp", FileUploadAlarmForm);
USEFORM("FirmwareUpdateUnit.cpp", FirmwareUpdateForm);
USEFORM("FoupOnlineCMDUnit.cpp", FoupOnlineCMDForm);
USEFORM("FFTViewer.cpp", FFTViewerForm);
USEFORM("FileListUnit.cpp", FileListForm);
USEFORM("FileUnit.cpp", FileForm);
USEFORM("HIDVelSecUnit.cpp", HIDVelSecForm);
USEFORM("HIDVelUnit.cpp", HIDVelForm);
USEFORM("HandOpenUnit.cpp", HandOpenForm);
USEFORM("HIDGraphUnit.cpp", HIDGraphForm);
USEFORM("HidLogGet.cpp", frmHidLogGet);
USEFORM("ConnectUnit.cpp", ConnectForm);
USEFORM("DataTransperReq.cpp", frmDataTransperReq);
USEFORM("CmdWinUnit.cpp", CmdWinForm);
USEFORM("CommDisplayUnit.cpp", CommDisplayForm);
USEFORM("ConMainUnit.cpp", ConversionForm);
USEFORM("ErrorSearchUnit.cpp", ErrorSearchForm);
USEFORM("ErrorUnit.cpp", ErrorForm);
USEFORM("EUVOnlineCMDUnit.cpp", EUVOnlineCMDForm);
USEFORM("DistanceSensorViewer.cpp", DistanceSensorViewrForm);
USEFORM("DisUnit.cpp", DisForm);
USEFORM("E23PIOUnit.cpp", E23PIOForm);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    try
    {
        Application->Initialize();
        Application->MainFormOnTaskBar = true;
        Application->Title             = "OHT_TP";
        Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TMapMakerForm), &MapMakerForm);
		Application->CreateForm(__classid(TMultiJogDistForm), &MultiJogDistForm);
		Application->CreateForm(__classid(TSocketForm), &SocketForm);
		Application->CreateForm(__classid(TConnectForm), &ConnectForm);
		Application->CreateForm(__classid(TLogFileForm), &LogFileForm);
		Application->CreateForm(__classid(TStatusForm), &StatusForm);
		Application->CreateForm(__classid(TProcessForm), &ProcessForm);
		Application->CreateForm(__classid(TCommDisplayForm), &CommDisplayForm);
		Application->CreateForm(__classid(TIOMapForm), &IOMapForm);
		Application->CreateForm(__classid(TIOTestForm), &IOTestForm);
		Application->CreateForm(__classid(TAssistantForm), &AssistantForm);
		Application->CreateForm(__classid(TDisForm), &DisForm);
		Application->CreateForm(__classid(TSpeedHelpForm), &SpeedHelpForm);
		Application->CreateForm(__classid(TLogDownForm), &LogDownForm);
		Application->CreateForm(__classid(TSysInfoForm), &SysInfoForm);
		Application->CreateForm(__classid(TWarnningForm), &WarnningForm);
		Application->CreateForm(__classid(TWaitForm), &WaitForm);
		Application->CreateForm(__classid(TM2AForm), &M2AForm);
		Application->CreateForm(__classid(THIDGraphForm), &HIDGraphForm);
		Application->CreateForm(__classid(TTimeSettingForm), &TimeSettingForm);
		Application->CreateForm(__classid(TVelGraphForm), &VelGraphForm);
		Application->CreateForm(__classid(TM1VelForm), &M1VelForm);
		Application->CreateForm(__classid(TM24VelForm), &M24VelForm);
		Application->CreateForm(__classid(TRebootForm), &RebootForm);
		Application->CreateForm(__classid(TMoveForm), &MoveForm);
		Application->CreateForm(__classid(TCmdWinForm), &CmdWinForm);
		Application->CreateForm(__classid(TTeachingForm), &TeachingForm);
		Application->CreateForm(__classid(TUserSelectForm), &UserSelectForm);
		Application->CreateForm(__classid(TSetCurForm), &SetCurForm);
		Application->CreateForm(__classid(TFileForm), &FileForm);
		Application->CreateForm(__classid(TFileListForm), &FileListForm);
		Application->CreateForm(__classid(TRotateOrgForm), &RotateOrgForm);
		Application->CreateForm(__classid(TMHoistForm), &MHoistForm);
		Application->CreateForm(__classid(TAMCParaForm), &AMCParaForm);
		Application->CreateForm(__classid(TConversionForm), &ConversionForm);
		Application->CreateForm(__classid(TPIOForm), &PIOForm);
		Application->CreateForm(__classid(TErrorForm), &ErrorForm);
		Application->CreateForm(__classid(TLongRunForm), &LongRunForm);
		Application->CreateForm(__classid(THIDVelForm), &HIDVelForm);
		Application->CreateForm(__classid(THIDVelSecForm), &HIDVelSecForm);
		Application->CreateForm(__classid(TAmcDebugForm), &AmcDebugForm);
		Application->CreateForm(__classid(THoistParamForm), &HoistParamForm);
		Application->CreateForm(__classid(TFileUploadAlarmForm), &FileUploadAlarmForm);
		Application->CreateForm(__classid(TAPMonitoringForm), &APMonitoringForm);
		Application->CreateForm(__classid(TAPTimeForm), &APTimeForm);
		Application->CreateForm(__classid(TTransForm), &TransForm);
		Application->CreateForm(__classid(TErrorSearchForm), &ErrorSearchForm);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		Application->CreateForm(__classid(TOptionForm), &OptionForm);
		Application->CreateForm(__classid(TE23PIOForm), &E23PIOForm);
		Application->CreateForm(__classid(THandOpenForm), &HandOpenForm);
		Application->CreateForm(__classid(TFoupOnlineCMDForm), &FoupOnlineCMDForm);
		Application->CreateForm(__classid(TFirmwareUpdateForm), &FirmwareUpdateForm);
		Application->CreateForm(__classid(TfrmOption), &frmOption);
		Application->CreateForm(__classid(TfrmUDP), &frmUDP);
		Application->CreateForm(__classid(TfrmOption), &frmOption);
		Application->CreateForm(__classid(TfrmDataTransperReq), &frmDataTransperReq);
		Application->CreateForm(__classid(TfrmHidLogGet), &frmHidLogGet);
		Application->CreateForm(__classid(TEUVOnlineCMDForm), &EUVOnlineCMDForm);
		Application->CreateForm(__classid(TfrmCidChecker), &frmCidChecker);
		Application->CreateForm(__classid(Tfrm_Bay_Test), &frm_Bay_Test);
		Application->CreateForm(__classid(TOBS_UBG_Form), &OBS_UBG_Form);
		Application->CreateForm(__classid(TDistanceSensorViewrForm), &DistanceSensorViewrForm);
		Application->CreateForm(__classid(TUnitChecker), &UnitChecker);
		Application->CreateForm(__classid(TFFTViewerForm), &FFTViewerForm);
		Application->CreateForm(__classid(TfrmVibrationMeasurement), &frmVibrationMeasurement);
		Application->CreateForm(__classid(TVibRouteSet), &VibRouteSet);
		Application->CreateForm(__classid(Tfrm_RouteRequest), &frm_RouteRequest);
		Application->CreateForm(__classid(Tfrm_AutoBayTest), &frm_AutoBayTest);
		Application->CreateForm(__classid(Tfrm_MapViewLoad), &frm_MapViewLoad);
		Application->CreateForm(__classid(TMaskOnlineCMDForm), &MaskOnlineCMDForm);
		Application->Run();
    }
	catch(Exception &exception)
    {
		Application->ShowException(&exception);
    }
    catch(...)
    {
        try
        {
            throw Exception("");
        }
        catch(Exception &exception)
        {
            Application->ShowException(&exception);
        }
    }
    return 0;
}
// ---------------------------------------------------------------------------
