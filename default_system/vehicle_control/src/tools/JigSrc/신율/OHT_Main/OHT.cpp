// ---------------------------------------------------------------------------
/** mainpage
@mainpage OHT 7.0 Prototype Project
@section  Overview
- 차기라인 대응 OHT 7.0 개발 프로젝트
@section list 구성
- 문서 구성
    - Class Overview
    - Class Member Function
    - Class Member Variable
- 기타
    -# Source Code View
    -# Call Graph
*/

/**
@file  OHT.cpp
$Rev:: 2225          $
$Author:: doori.shin $
$Date:: 2014-04-11 1#$
@brief OHT Main Program Main file
*/

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
// ---------------------------------------------------------------------------
USEFORM("OHTMain.cpp", OHTMainForm);
USEFORM("JigSlideUnit.cpp", SlideUnit);
USEFORM("JigHoistUnit.cpp", HoistUnit);
USEFORM("JigCheckbox.cpp", UnitCheckBox);
USEFORM("JigShutterUnit.cpp", ShutterUnit);
USEFORM("JigRotateUnit.cpp", RotateUnit);
USEFORM("JigHandUnit.cpp", HandUnit);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
   HANDLE Mutex;

    try
    {
      // check mutex to avoid second instance
	  const TCHAR ProgMutex[] = L"OHT_Main";
      Mutex=OpenMutex(MUTEX_ALL_ACCESS, false, ProgMutex);

      if(!Mutex)
         CreateMutex(NULL, true, ProgMutex);
	  else{
		 ReleaseMutex(Mutex);
         return 0;
      }


		Application->Initialize();
        Application->MainFormOnTaskBar = true;
		Application->Title = "OHT_Main";
		Application->CreateForm(__classid(TOHTMainForm), &OHTMainForm);
//		Application->CreateForm(__classid(TShutterUnit), &ShutterUnit);
//		Application->CreateForm(__classid(TRotateUnit), &RotateUnit);
//		Application->CreateForm(__classid(THandUnit), &HandUnit);
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
