#ifndef AvsControlH
#define AvsControlH

#include "../Common/Protocol_Define_Modify.h"
#include "../Common/UdpCom.h"
#include "../Common/UdpDiag.h"
#include "PassPermit.h"

#include "Def_OHT.h"
#include "../Common/Def_Protocol.h"
#include "Def_HW.h"
#include "Def_DefaultIniParam.h"

//#include "avsdll.h"


#define AVSDLL_FILENAME  L"AVS.dll"

/*
#ifdef __cplusplus
extern "C" {
#endif
*/
#define ERROR_NONE								0
#define ERROR_BAD_ID_STRING						0x10
#define ERROR_DATA_NOT_READY					0x20
#define ERROR_DATA_OVERFLOW						0x21
#define ERROR_DATA_DROP							0x22
#define ERROR_BT_LINK_MODULE_FAILED				0x30
#define ERROR_INCOMPATIBLE_DEVICE				0x1000
#define ERROR_COMMUNICATION_TIMEOUT				0x2000
#define ERROR_AVS_NOT_READY						0x3000

#define AVS_SUCESS  1
#define AVS_WAIT_DATA 0

//C++ Interface to the AVS.DLL



#define CLOSE_AVS_DLL { FreeLibrary(hAVSDLL);}
/*
#ifdef  __cplusplus
	}
#endif
*/

typedef struct vibration_data_
{
	float x;
	float y;
	float z;
}vibration_data;

//---------------------------------------------------------------------------

class AvsControl
{
	private:
		HINSTANCE	hAVSDLL;
		int batt,temp,data_size;
		float fX[NUM_OF_VIB_SAMPLE];
		float fY[NUM_OF_VIB_SAMPLE];
		float fZ[NUM_OF_VIB_SAMPLE];
		String strDllFileName;
		AVS_VIBRATION_DATA_INFO mVibrationData;
		void printError(int err);
		UdpCom *m_pUDPCommunication;
		int m_nAliveCounter;
		int m_nPreAliveCounter;
		DWORD m_dwCheckCounter;
		DEFAULT_PARAM_SET *m_defualtparam;

		bool bUseAVS;
	public:
		AvsControl();
		~AvsControl();
		bool m_bIsStarted;
		int LinkSensor(void);
		void StartReceiveVibData(void);
		void StopReceiveVibData(void);
		int EndReceiveVibData(void);
		int ReceiveVibData(void);
		void ReceiveAlivePacket();
		void CheckTPConnected();

		bool bDllLoadFail;
};
#endif
