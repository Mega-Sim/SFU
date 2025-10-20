/**
@file    OHTAssistant.cpp
$Rev:: 771           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-03-28 1#$:  Date of last commit
@version OHT 7.0
@brief   OHT Assistan Program main cpp file
*/
// ---------------------------------------------------------------------------

#include <vcl.h>
#include <tchar.h>
#pragma hdrstop

HINSTANCE g_hinst;
// ---------------------------------------------------------------------------

USEFORM("SocketUnit.cpp", SocketForm);
USEFORM("..\COMMON\UDP.cpp", frmUDP);
USEFORM("FBWFManagerUnit.cpp", FBWFManagerForm);
USEFORM("AssistantMain.cpp", AssistantMainForm);
USEFORM("About.cpp", AboutBox);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE hInstance, LPTSTR, int)
{
	HANDLE Mutex;

    try
    {
		// check mutex to avoid second instance
		const TCHAR *ProgMutex = L"OHT Assistant";
		Mutex = OpenMutex(MUTEX_ALL_ACCESS, false, ProgMutex);

		if(!Mutex)
            CreateMutex(NULL, true, ProgMutex);
        else
        {
            ReleaseMutex(Mutex);
            return 0;
		}

		Application->Initialize();
		g_hinst = hInstance;
		Application->Title = "OHT Assistant";
		Application->CreateForm(__classid(TAssistantMainForm), &AssistantMainForm);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		Application->CreateForm(__classid(TFBWFManagerForm), &FBWFManagerForm);
		Application->CreateForm(__classid(TSocketForm), &SocketForm);
		Application->CreateForm(__classid(TfrmUDP), &frmUDP);
		Application->ShowMainForm = false;

        Application->Run();

	}
	catch(Exception &exception)
	{
        Application->ShowException(&exception);
    }

    ReleaseMutex(Mutex);
    return 0;
}
// ---------------------------------------------------------------------------
