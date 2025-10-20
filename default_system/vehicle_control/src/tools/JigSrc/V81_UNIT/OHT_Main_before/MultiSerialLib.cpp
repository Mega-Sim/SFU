//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "MultiSerialLib.h"
#include "Def_OHT.h"
#include "ErrorCode.h"
#include "OHTMain.h"

#define AMC_MS_INTERRUPT            5
#define AMC_MS_RETRY				5
#define  NAME_SERIAL_CFG            "SerialConfig.cfg"



//---------------------------------------------------------------------------
HINSTANCE MultiSerialLibrary::R_RunTimeLink()
{
//   HINSTANCE g_hSerialLib;
	String strOHTPath = "", strTmpFileName = "";

    strOHTPath = ExtractFileDir(Application->ExeName);
    strTmpFileName = AnsiString(strOHTPath + STR_DATAFILE_PATH) + "amcms2.dll";

    // Multi Serial DLL 파일 Load
    g_hSerialLib = LoadLibrary(strTmpFileName.c_str());

   if(g_hSerialLib == NULL)
   {
      Sleep(1000);
	  g_hSerialLib = LoadLibrary(strTmpFileName.c_str());
	  if(g_hSerialLib == NULL) 
		return NULL;	
   }

      DeviceSerialOpen        = (OPENSERIALDEVICE)             GetProcAddress(g_hSerialLib, "amcms_initialize");
      DeviceSerialClose       = (CLOSESERIALDEVICE)            GetProcAddress(g_hSerialLib, "amcms_release");
      DeviceSerialLoadSetting = (LOADSETTINGSERIALDEVICE)      GetProcAddress(g_hSerialLib, "amcms_load_setting");
/*
      DeviceSerialOpen        : Hardware 적으로 Serial Port Open
      DeviceSerialClose       : Port Close
      DeviceSerialLoadSetting : 전체 Port에 대해 Serial 통신 값 setting...
*/

      if(!DeviceSerialOpen || !DeviceSerialClose || !DeviceSerialLoadSetting)
      {
        return NULL;
      }
      else
      {
         return g_hSerialLib;
      }

}
//---------------------------------------------------------------------------
MultiSerialLibrary::MultiSerialLibrary()
{
    Init();
}
//---------------------------------------------------------------------------
/**
@brief   Multi Serial Library를 초기화하는 함수
@date    2012.12.03
@return  에러 코드
@warning 로그를 남기거나 변수를 하나 더 선언하지 않을 경우 OHT 초기화가 안됨
@warning 원인은 아직 파악 못함
*/
int MultiSerialLibrary::Init()
{
    int nError = 0;

    g_hSerialLib = NULL;      //handlea
    g_bSerialDriverOpen = false; //device open flag

	if(!g_hSerialLib)
        g_hSerialLib = R_RunTimeLink();
    
    if(g_hSerialLib == NULL) 
        nError = ERR_SERIAL_DLL_INIT;

    if((nError == NO_ERR) && (!g_bSerialDriverOpen))
    {
        nError = SerialSettingFileLoad();

        if(nError == NO_ERR)
        {
			int nRetValue = 0;

			for(int i=0; i < AMC_MS_RETRY; i++)
			{
              	nRetValue = DeviceSerialOpen(AMC_MS_INTERRUPT);     // Serial 에 사용할 PC interrupt No : 5.. 변경 가능...
				if(nRetValue == 0)
				{
				  g_bSerialDriverOpen = true;
				  break;
				}
				else
					g_bSerialDriverOpen = false; // 요기 return int를 bool로 변경...

				Sleep(2000);
            }
			///@todo AMC Init 유무 확인 추가 필요
            //if((!g_bSerialDriverOpen)&&(gm_OHTStatus->bAMCInitialized==true))
             if(!g_bSerialDriverOpen)
                    nError = ERR_SERIAL_DLL_INIT;
        }
    }

// by zzang9un 2012.12.03 : 원인을 알 수 없음
#ifdef USE_LOG_MEMO
    if(nError == NO_ERR)
    {
        ADD_MEMO_T("Serial Dll Load... OK!..\r\n");
    }
    else
    {
        ADD_MEMO_T("Serial Dll Load... Fail!..\r\n");
    }
#endif /* USE_LOG_MEMO */    

    return nError;
}
//---------------------------------------------------------------------------
MultiSerialLibrary::~MultiSerialLibrary()
{
   if(g_bSerialDriverOpen)
   {
	  DeviceSerialClose();
   }
   if(g_hSerialLib)
   {
	  FreeLibrary(g_hSerialLib);
      g_hSerialLib=NULL;
   }
   g_bSerialDriverOpen = false;
}
//---------------------------------------------------------------------------
int MultiSerialLibrary::SerialSettingFileLoad(void)
{
   int nError=0;
   AnsiString strTmpFileName = "";
   AnsiString strOHTPath=".";

   strOHTPath=ExtractFileDir(Application->ExeName);
   strTmpFileName = AnsiString(strOHTPath+STR_DATAFILE_PATH) + NAME_SERIAL_CFG;

   if(strTmpFileName.Length() <= 0) return ERR_SERIAL_DLL_FILE_NAME;

   if(!FileExists(strTmpFileName)) return  ERR_SERIAL_DLL_FILE_NO_EXIST;

   DeviceSerialLoadSetting(strTmpFileName.c_str());

   return nError;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
















