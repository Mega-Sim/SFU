//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("ProgressReport.cpp", frmProgressReport);
USEFORM("UDP.cpp", frmUDP);
USEFORM("Option.cpp", frmOption);
USEFORM("OhtMain.cpp", frmOHTMain);
USEFORM("ClwJobOrderResponse.cpp", frmJobOrderResponse);
USEFORM("ClwPassConfirm.cpp", frmClwPassConfirm);
USEFORM("ClwDataTransperResponse.cpp", frmClwDataTransperResponse);
USEFORM("About.cpp", AboutBox);
USEFORM("ClwDiagAnswer.cpp", frmDiagAnswer);
USEFORM("ClwVersionResponse.cpp", frmClwVersionRequest);
USEFORM("Oht2TP.cpp", frmOht2TP);
USEFORM("ClwStatusReport.cpp", frmClwStatusReport);
USEFORM("ClwPassConfirmRequest.cpp", frmClwPassConfirmRequest);
USEFORM("ClwPassConfirmStatusReport.cpp", frmClwPassConfirmStatusReport);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TfrmOHTMain), &frmOHTMain);
		Application->CreateForm(__classid(TfrmOption), &frmOption);
		Application->CreateForm(__classid(TfrmUDP), &frmUDP);
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
