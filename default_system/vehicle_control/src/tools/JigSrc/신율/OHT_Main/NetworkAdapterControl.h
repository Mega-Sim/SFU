

#ifndef __NETWORK_ADAPTER_CONTROL
#define __NETWORK_ADAPTER_CONTROL

#ifdef _DLL_EXPORTING
#	define DLL_EXPORTING_API extern "C" __declspec(dllexport)
#else
#	define DLL_EXPORTING_API __declspec(dllimport)
#endif

typedef struct _INetInfo
{
      LPWSTR		szDevName;
      LPWSTR		szDevHelpString;
      LPWSTR		szDevPnPNode;
      LPWSTR		szDevId;
      unsigned long	iDevStatus;
      unsigned long	iDevCaps;
} INetInfo;

typedef struct _INetConnectInfo
{
	unsigned char	szSSID[32];
	int				iRssi;
	unsigned int	iChannel;
	unsigned char	cAPMAC[6];
} INetConnectInfo;

extern "C" __declspec(dllexport) int __stdcall NetworkAdapterControl_Initialize();
extern "C" __declspec(dllexport) INetInfo*  __stdcall NetworkAdapterControl_GetINetInfo(unsigned int index);
extern "C" __declspec(dllexport) int __stdcall NetworkAdapterControl_ControlAdaptor(unsigned int index, BOOL bEnable);
extern "C" __declspec(dllexport) int __stdcall NetworkAdapterControl_DeInitialize();
extern "C" __declspec(dllexport) int __stdcall NetworkAdapterControl_GetINetConnectInfo(unsigned int index, INetConnectInfo* pInfo);

#endif
