//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("FileManagerMain.cpp", FileManagerForm);
USEFORM("..\COMMON\UDP.cpp", frmUDP);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	HANDLE Mutex;

	try
	{
		// check mutex to avoid second instance
		const TCHAR *ProgMutex = L"OHT FileManager";
		Mutex = OpenMutex(MUTEX_ALL_ACCESS, false, ProgMutex);
		if(!Mutex)
            CreateMutex(NULL, true, ProgMutex);
        else
        {
            ReleaseMutex(Mutex);
            return 0;
		}

		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TFileManagerForm), &FileManagerForm);
		Application->CreateForm(__classid(TfrmUDP), &frmUDP);
		Application->ShowMainForm = true;
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

	ReleaseMutex(Mutex);
	return 0;
}
//---------------------------------------------------------------------------
