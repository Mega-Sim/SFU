//---------------------------------------------------------------------------
/**
@file    IPCLibrary.cpp
$Rev:: 467           $:  Revision of last commit
$Author:: puting   $:  Author of last commit
$Date:: 2013-03-28 1#$:  Date of last commit
@version OHT ver
@brief   IPCLibrary Class 소스 File
*/

#include "IPCLoadLib.h"

#define ERRORIPCReady 1
#define ERRORDeviceHWMAvailable 2
#define ERRORErrCount 3
//---------------------------------------------------------------------------
/**
@brief   IPC Library 생성자
@author  puting
@date    2013.03.28
*/
IPCLibrary::IPCLibrary()
{
  //	InitializeCriticalSection(&m_CS);


	IPC_Init();
}
//---------------------------------------------------------------------------
/**
@brief   IPC Library 소멸자
@author  puting
@date    2013.03.28
*/
IPCLibrary::~IPCLibrary()
{
   if(m_bIPCReady==true) DeviceSusiDLLUninit();
   if(m_hIpcLib)
   {
	  FreeLibrary(m_hIpcLib);
	  m_hIpcLib=NULL;
	  m_bDllLoaded = false;
	  m_bIPCReady = false;
   }
}
//---------------------------------------------------------------------------
/**
@brief   IPC 초기화 함수
@author  puting
@date    2013.03.28
*/
void IPCLibrary::IPC_Init(void)
{
   bool bIPCError=false;
   int  nIPCError=-1;

   m_hIpcLib=NULL;      //handle

   m_bDllLoaded = false;			///< 1. IPC DLL Load 정상 완료
   m_bIPCReady = false;		    	///< 2. IPC 사용 가능한 상태

   memset(&m_IPCData, 0x00, sizeof(IPC_DATA_INFO));


   if(!m_hIpcLib) m_hIpcLib = IPC_OpenDLL();

   if((m_hIpcLib!=NULL) &&(m_bDllLoaded !=false))
   {
#if USE_SOM6869
		nIPCError = DeviceSusiDLLInit();
#else
	   bIPCError = DeviceSusiDLLInit();
#endif
   }

   if(bIPCError==true || nIPCError == SUSI_STATUS_SUCCESS)
   {
	///@todo 추후 추가 필요
	  //OHTMainForm->OHTDisplay->Lines->Add("IPC Dll Load..OK");
	  m_bIPCReady = true;
   }

}
//---------------------------------------------------------------------------
/**
@brief   IPC OPEN Library 함수
@author  puting
@date    2013.03.28
@return  HINSTANCE(DLL)
*/
HINSTANCE IPCLibrary::IPC_OpenDLL()
{
   //	EnterCriticalSection(&m_CS);
	HINSTANCE hIpcLib= NULL;
	TCHAR strTmpFullPathFileName[MAX_LENGTH_OF_FULL_PATH]={0,};

#ifdef  UNICODE
	LPCTSTR strTmpFileName = L"DataFiles\\Susi.dll";       //TCHAR
	_wfullpath(strTmpFullPathFileName, strTmpFileName, MAX_LENGTH_OF_FULL_PATH);
#else
	LPCTSTR strTmpFileName = "DataFiles\\Susi.dll";       //TCHAR
	_fullpath(strTmpFullPathFileName, strTmpFileName, MAX_LENGTH_OF_FULL_PATH);
#endif


	//Form1->
	if(m_bDllLoaded==true)
	{
		return NULL;
	}

   hIpcLib = LoadLibrary(strTmpFullPathFileName);


   if(hIpcLib == NULL)
   {
	  return NULL;
   }

#if USE_SOM6869
   DeviceSusiDLLInit      = (INITALIPCDLL6869)				GetProcAddress(hIpcLib, "SusiLibInitialize");
   DeviceSusiDLLUninit    = (UNINITIALIPCDLL6869)			GetProcAddress(hIpcLib, "SusiLibUninitialize");
   DeviceGetBoardValue	  = (GETBOARDVALUE6869)             GetProcAddress(hIpcLib, "SusiBoardGetValue");
#else
   DeviceSusiDLLInit      = (INITIALIPCDLL)                 GetProcAddress(hIpcLib, "SusiDllInit");
   DeviceSusiDLLUninit    = (UNINITIALIPCDLL)               GetProcAddress(hIpcLib, "SusiDllUnInit");

   DeviceHWMAvailable     = (CHECKIPCMONITORDRIVER)         GetProcAddress(hIpcLib, "SusiHWMAvailable");
   DeviceHWMGetTemp       = (GETIPCTEMPERATURE)             GetProcAddress(hIpcLib, "SusiHWMGetTemperature");
   DeviceHWMGetVolt       = (GETIPCVOLTAGE)                 GetProcAddress(hIpcLib, "SusiHWMGetVoltage");
#endif
/*
   DeviceSusiDLLInit        : IPC용 SUSI Library initialization
   DeviceSusiDLLUninit      : IPC용 SUSI Library uninitialization
   DeviceHWMAvailable       : Check if the hardware monitor driver is available
   DeviceHWMGetTemp         : Read the current value of the temperature sensor
*/

#if USE_SOM6869
   if(!DeviceSusiDLLInit || !DeviceSusiDLLUninit || !DeviceGetBoardValue)
#else
   if(!DeviceSusiDLLInit || !DeviceSusiDLLUninit || !DeviceHWMAvailable || !DeviceHWMGetTemp || !DeviceHWMGetVolt)
#endif
   {
		FreeLibrary(hIpcLib);

		return NULL;
   }
   else
   {
		m_bDllLoaded=true;
		return hIpcLib;
   }

}
//---------------------------------------------------------------------------
//int  IPCLibrary::CheckIPCTemp()
//{
//	int nError = NO_ERR;
//	bool RtnVal1=false, RtnVal2=false, RtnVal3=false;
//	float Temp_CPU=0.0, Temp_Sys=0.0, Volt_V5=0.0;
//
//	static int nErrCount=0;
//
//   //if((gm_OHTStatus->strModeToOCS=="A")&&(gm_OHTStatus->strStatusToOCS!="A")) return;
//   if(m_bIPCReady==true)
//   {
//		if(DeviceHWMAvailable() == 1)
//		 {
//			RtnVal1 = DeviceHWMGetTemp(1, &Temp_CPU);
//			RtnVal2 = DeviceHWMGetTemp(2, &Temp_Sys);
//			RtnVal3 = DeviceHWMGetVolt(1<<3, &Volt_V5);
//
//			EnterCriticalSection(&m_CS);
//			//큰값 대입
//			if((RtnVal1 == true)&&(RtnVal2 == true))
//			{
//				 if(Temp_CPU > Temp_Sys)		 m_IPCData.dCPUTemperature = Temp_CPU;
//				 else  					     	 m_IPCData.dCPUTemperature = Temp_Sys;
//			}
//
//			if(RtnVal3==true)
//			{
//			   m_IPCData.dVoltage_Input = Volt_V5;
//
//			}
//
//
//		}
//
//
//
//		if(m_IPCData.dCPUTemperature > IPCTEMPERLIMIT)
//		{
//			nErrCount++;
//			if(nErrCount > 5)
//			{
//				nError = ERR_IPC_OVERTEMPERATURE;
//				nErrCount=0;
//			 }
//		}
//		else  nErrCount=0;
//
//		LeaveCriticalSection(&m_CS);
//
//		return nError;
//
//   }else return ERR_IPCLibrary_READY;
//
//}

//------------------------------------------------------------------------------
/**
@brief   IPC Data Get 함수
@author  puting
@param   IPC_DATA_INFO
@date    2013.03.28
@return  정상이면 true, 에러 발생 Code반환
*/
int IPCLibrary::GetIPCData(IPC_DATA_INFO* IPCData)
{

	bool RtnVal1=false, RtnVal2=false, RtnVal3=false;
#if USE_SOM6869
	unsigned int Temp_CPU=0, Volt_V5=0;
#else
	float Temp_CPU=0.0, Temp_Sys=0.0, Volt_V5=0.0;
#endif

	static int nErrCount=0;

	if(m_bIPCReady==true)
	{
#if USE_SOM6869
		try{
			RtnVal1 = DeviceGetBoardValue(SUSI_ID_HWM_TEMP_CPU, &Temp_CPU);
			RtnVal3 = DeviceGetBoardValue(SUSI_ID_HWM_VOLTAGE_5VSB, &Volt_V5);
			//System Temp, Voltage 5V 없어짐(SUSI4.dll)
//			RtnVal2 = DeviceGetBoardValue(SUSI_ID_HWM_TEMP_SYSTEM, &Temp_Sys);
//			RtnVal3 = DeviceGetBoardValue(SUSI_ID_HWM_VOLTAGE_5V, &Volt_V5);


			//큰값 대입
			if(RtnVal1 == SUSI_STATUS_SUCCESS)
			{
				 IPCData->dCPUTemperature = (float)(Temp_CPU*0.01);
			}
			if(RtnVal3==SUSI_STATUS_SUCCESS)
			{
			   IPCData->dVoltage_Input = (float)(Volt_V5*0.001);
			}

		}catch(...)
		{
		}
#else
		if(DeviceHWMAvailable() == 1)
		{
			RtnVal1 = DeviceHWMGetTemp(1, &Temp_CPU);
			RtnVal2 = DeviceHWMGetTemp(2, &Temp_Sys);
			RtnVal3 = DeviceHWMGetVolt(1<<3, &Volt_V5);

			//큰값 대입
			if((RtnVal1 == true)&&(RtnVal2 == true))
			{
				 if(Temp_CPU > Temp_Sys)		 IPCData->dCPUTemperature = Temp_CPU;
				 else  					     	 IPCData->dCPUTemperature = Temp_Sys;
			}
			if(RtnVal3==true)
			{
			   IPCData->dVoltage_Input = Volt_V5;
			}
		 }
		 else
			return ERRORDeviceHWMAvailable;
#endif

		if(IPCData->dCPUTemperature > IPCTEMPERLIMIT)
		{
			nErrCount++;
			if(nErrCount > 5)
			{
				if(nErrCount > 30)
				{
 	               nErrCount = 10;
                }
				return ERRORErrCount;
			}
		}
		else  nErrCount=0;

		return NO_ERR;

	}
	else
		return ERRORIPCReady;

}

// 2018.10.11 SHCHO : IPC의 온도, 전압정보를 String으로 전달한다.
bool IPCLibrary::GetInfo_String(char *szOut, int nSize)
{
#if USE_SOM6869
	unsigned int dTemp_CPU = -9999;
	unsigned int dTemp_SYS = -9999;
	unsigned int dVolt_V33 = -9999;
	unsigned int dVolt_V50 = -9999;
	unsigned int dVolt_VBS = -9999;
	unsigned int dVolt_VBAT = -9999;
	unsigned int dVolt_VN50 = -9999;

	DeviceGetBoardValue(SUSI_ID_HWM_TEMP_CPU, &dTemp_CPU);
	DeviceGetBoardValue(SUSI_ID_HWM_TEMP_SYSTEM, &dTemp_SYS);
	DeviceGetBoardValue(SUSI_ID_HWM_VOLTAGE_3V3, &dVolt_V33);
	DeviceGetBoardValue(SUSI_ID_HWM_VOLTAGE_5V, &dVolt_V50);
	DeviceGetBoardValue(SUSI_ID_HWM_VOLTAGE_5VSB, &dVolt_VBS);
	DeviceGetBoardValue(SUSI_ID_HWM_VOLTAGE_VBAT, &dVolt_VBAT);
	DeviceGetBoardValue(SUSI_ID_HWM_VOLTAGE_5NV, &dVolt_VN50);

	char szBuf[512];
	ZeroMemory(szBuf, sizeof(szBuf));
	sprintf(szBuf, "C/%.2f/S/%.2f/V33/%.2f/V50/%.2f/VBS/%.2f/VBAT/%.2f/VN50/%.2f/"
			, dTemp_CPU, dTemp_SYS, dVolt_V33, dVolt_V50, dVolt_VBS, dVolt_VBAT, dVolt_VN50);

//	IPC_LOG("04_%d/%d/%d/%d/%d/%d/%d", dTemp_CPU, dTemp_SYS,dVolt_V33,dVolt_V50, dVolt_VBS, dVolt_VBAT, dVolt_VN50);
//	[14:02:09.908]04_2972/-9999/-9999/-9999/5170/3140/-9999


#else
	// SOM-4466 디바이스 드라이버 안맞는 경우
	if(DeviceHWMAvailable() != 1)
		return false;

	float dTemp_CPU = -9999;
	float dTemp_SYS = -9999;
	float dVolt_V33 = -9999;
	float dVolt_V50 = -9999;
	float dVolt_VBS = -9999;
	float dVolt_VBAT = -9999;
	float dVolt_VN50 = -9999;

	// Susi dll 제공 APIs
	// 자세한 내용은 susi_manual.pdf 참고
	DeviceHWMGetTemp(1, &dTemp_CPU);
	DeviceHWMGetTemp(2, &dTemp_SYS);
	DeviceHWMGetVolt(1<<2, &dVolt_V33);
	DeviceHWMGetVolt(1<<3, &dVolt_V50);
	DeviceHWMGetVolt(1<<5, &dVolt_VBS);
	DeviceHWMGetVolt(1<<6, &dVolt_VBAT);
	DeviceHWMGetVolt(1<<7, &dVolt_VN50);

	char szBuf[512];
	ZeroMemory(szBuf, sizeof(szBuf));
	sprintf(szBuf, "C/%.2f/S/%.2f/V33/%.2f/V50/%.2f/VBS/%.2f/VBAT/%.2f/VN50/%.2f/"
			, dTemp_CPU, dTemp_SYS, dVolt_V33, dVolt_V50, dVolt_VBS, dVolt_VBAT, dVolt_VN50);


#endif

	int nLen = strlen(szBuf);
	if(nLen >= nSize)
		return false;

	ZeroMemory(szOut, nSize);
	strcpy(szOut, szBuf);

	return true;

}

//------------------------------------------------------------------------------
bool IPCLibrary::IsIPCReady()
{
 	return m_bIPCReady;
}
//
//
//int IPCLibrary::Get_IsLogDriveAlive(char* szDrvName)
//{
//   char DrvRoot[4] = "A:\\";
//   int Drv;
//   char VolName[MAX_PATH]={0,}, FileSysName[MAX_PATH]={0,};
//   DWORD VolSerial;
//   bool bFind=false;
//
//   char* nPos = strchr(szDrvName, ':');
//   if(nPos==NULL) return 0;
//   DWORD Drives = GetLogicalDrives();
//
//   for(Drv=0; Drv<26; Drv++)
//   {
//	  if((Drives >> Drv) & 0x00000001)
//	  {
//		 if((char)(Drv + 'A') == szDrvName[0])
//		 {
//			bFind=true;
//			break;
//		 }
//	  }
//   }
//
//   if(bFind==false) return 0;
//
//   DrvRoot[0] += Drv;
//   GetVolumeInformationA(DrvRoot, VolName, sizeof(VolName), &VolSerial, NULL, NULL, FileSysName, sizeof(FileSysName));
//
//		기존에 잘못된 방식으로 문자열 비교하여 수정 필요 . puting  //이즈이퀄같은 함수를 통해 비교
//   if((FileSysName== "FAT32")||(FileSysName=="NTFS"))
//		 return 1;
//   else
//		 return 0;
//
//}
