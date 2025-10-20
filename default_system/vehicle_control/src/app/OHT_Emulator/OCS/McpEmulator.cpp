//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("OcsStatusReport.cpp", frmOcsStatusReport);
USEFORM("OcsVersionRequest.cpp", frmOcsVersionRequest);
USEFORM("OcsPassConfirmResponse.cpp", frmOcsPassConfirmResponse);
USEFORM("OcsFloorMoveOrder.cpp", frmOcsFloorMoveOrder);
USEFORM("OcsInputRequestResponse.cpp", frmOcsInputRequestResponse);
USEFORM("RunningStatus.cpp", frmRunningStatus);
USEFORM("UDP.cpp", frmUDP);
USEFORM("Option.cpp", frmOption);
USEFORM("ResponsePasspermitOCS.cpp", frmResponsePasspermitOCS);
USEFORM("AutoJob.cpp", frmAutoJob);
USEFORM("About.cpp", AboutBox);
USEFORM("OcsDataTransperOrder.cpp", frmOcsDataTransperOrder);
USEFORM("McpMain.cpp", frmMcpMain);
USEFORM("DiagSvrCmd.cpp", frmDiagSvrCmd);
USEFORM("JobOrderOCS.cpp", frmJobOrderOCS);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TfrmMcpMain), &frmMcpMain);
		Application->CreateForm(__classid(TfrmUDP), &frmUDP);
		Application->CreateForm(__classid(TfrmOption), &frmOption);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
