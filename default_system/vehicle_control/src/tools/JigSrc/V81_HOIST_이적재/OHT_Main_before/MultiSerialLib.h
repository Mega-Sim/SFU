//---------------------------------------------------------------------------

#ifndef SerialLibH
#define SerialLibH
//---------------------------------------------------------------------------
#include <string.h>
//---------------------------------------------------------------------------
class MultiSerialLibrary
{
private :
   typedef int       (_stdcall* OPENSERIALDEVICE)(int intr);
   typedef void      (_stdcall* CLOSESERIALDEVICE)();
   typedef void      (_stdcall* LOADSETTINGSERIALDEVICE)(const char *path);

   HINSTANCE R_RunTimeLink();
   HINSTANCE g_hSerialLib;

public:

   MultiSerialLibrary();
   ~MultiSerialLibrary();

   OPENSERIALDEVICE                 DeviceSerialOpen;
   CLOSESERIALDEVICE                DeviceSerialClose;
   LOADSETTINGSERIALDEVICE          DeviceSerialLoadSetting;

   bool g_bSerialDriverOpen;
   int SerialSettingFileLoad(void);
   int Init(void);
};

//---------------------------------------------------------------------------
#endif



