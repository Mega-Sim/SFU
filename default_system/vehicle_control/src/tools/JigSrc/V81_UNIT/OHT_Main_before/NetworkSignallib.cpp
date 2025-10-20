//---------------------------------------------------------------------------
/**
@file    NetworkSignallib.cpp
$Rev:: 467           $:  Revision of last commit
$Author:: puting   $:  Author of last commit
$Date:: 2013-03-26 1#$:  Date of last commit
@version OHT ver
@brief   NetworkSignallib Class 소스 File
*/

#include "NetworkSignallib.h"


//#define WirelessNetwork_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\WirelessNet.log", __VA_ARGS__)
//수정
#define WirelessNetwork_LOG(...)   WRITE_LOG_UNIT(pLogWirelessNet, __VA_ARGS__)


#define ERRORNetConnectInfo 8
#define ConnectAP			0
#define INetInfoPointNull 	-1
#define DisconnectAP 		-2
#define INIT_STATUS			-3
//---------------------------------------------------------------------------
/**
@brief   NETWORK Library 생성자
@author  puting
@date    2013.03.25
*/
NetworkSinalLib::NetworkSinalLib()
{
	pLogWirelessNet = getLogUnit("D:\\log\\WirelessNet.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	//NETWORKInit();
	//DisconnectFlag = false;
	m_bNETWORKReady = false;
	m_bDisconnectFlag = false;
	m_nNetworkAdapterIndex = -1;
	memset(&m_Info, 0x00, sizeof(INetConnectInfo));
}
/**
@brief   NETWORK Library 소멸자
@author  puting
@date    2013.03.25
*/
NetworkSinalLib::~NetworkSinalLib()
{
	/// @todo	추후 필요에 따라 Sync Gain저장하는 기능 추가
	NETWORKClose();


}
/**
@brief   NETWORK 초기화 함수
@author  puting
@date    2013.03.25
@return  nError
*/
int NetworkSinalLib::NETWORKInit(void)
{
	int nError =0;
	int iNumberOfDevice = 0;
	char* strDevName="";
	LPWSTR lpDevName;

	INetInfo* pInfo;
	m_nNetworkAdapterIndex =-1;
	m_bNETWORKReady = false;

	iNumberOfDevice = NetworkAdapterControl_Initialize();

	WirelessNetwork_LOG("Network Device Num : %d",  iNumberOfDevice);

	if(iNumberOfDevice < 0)
	{
		m_nNetworkAdapterIndex=-1;
		m_bNETWORKReady = false;
		DisconnectFlag = true;
		nError = ERR_NETWORKAPDATER_INIT;
	}
	else
	{
	  for(int i = 0; i < iNumberOfDevice; i++)
	  {
		pInfo = NetworkAdapterControl_GetINetInfo(i);
		lpDevName = pInfo->szDevName;
		strDevName = WCharToChar(lpDevName);

		WirelessNetwork_LOG("Net Name : %s", strDevName);

#if(WirelessClient_TYPE == XR5_WIRELESS_CLIENT)
		if(my_strstr(strDevName, "Wireless")!=NULL)
		{
			m_nNetworkAdapterIndex = i;
			m_bNETWORKReady = true;
			break;
		}
#else
        // FXE-3000 무선 Client의 경우 Ethernet 포트에 연결
        // FXE-3000 Ethernet 포트 이름 : Realtek PCIe FE Family Controller
		// IOT Ethernet 포트 이름 : Realtek RTL8139 Family PCI Fast Ethernet NIC
		AnsiString strTmp;
		strTmp = strDevName;
		strTmp = strTmp.UpperCase();

		if(strTmp.AnsiPos("CONTROLLER") != 0)
		// if(my_strstr(strDevName, "Controller")!=NULL)
        {
        	WirelessNetwork_LOG("Controller found!");
		
            // 2016.12.19 sh17.jo 유선포트 인식 상태 확인
            // 유선 포트가 2개 인식되므로 2개에 대해서 Enable 필요 (추후 구현)
            m_nNetworkAdapterIndex = i;
			m_bNETWORKReady = true;
			break;
        }
#endif
	  }

	  if(m_bNETWORKReady==true)
	  {
		nError = Network_Adapter_Enable();
		DisconnectFlag = false;
	  }
	  else
		nError = ERR_NETWORKAPDATER_INIT;
	}

#if(WirelessClient_TYPE == XR5_WIRELESS_CLIENT)
	if(nError==0)
	{
		WirelessNetwork_LOG("Wireless NetworkAdapter Init Succeced!");
	}
	else
	{
		WirelessNetwork_LOG("Wireless NetworkAdapter Init Failed.. ERR_NETWORKAPDATER_INIT");
	}
#else
	if(nError==0)
	{
		WirelessNetwork_LOG("Wire NetworkAdapter Init Succeced!");
	}
	else
	{
		WirelessNetwork_LOG("Wire NetworkAdapter Init Failed.. ERR_NETWORKAPDATER_INIT");
	}
#endif
	return nError;
}
//----------------------------------------------------------------------------
/**
@brief   NETWORK 종료 함수
@date    2013.03.26
@param   m_bNETWORKReady
@return
*/
void NetworkSinalLib::NETWORKClose(void)
{
	m_bNETWORKReady = false;
	m_nNetworkAdapterIndex = -1;
	NetWork_Adapter_Disable();
	NetworkAdapterControl_DeInitialize();
}
//----------------------------------------------------------------------------
/**
@brief   Network_Adapter_Enable 함수
@date    2013.03.26
@param   g_nNetworkAdapterIndex
@return  nError
*/
int NetworkSinalLib::Network_Adapter_Enable(void)
{
   int nError=0;

   if(m_nNetworkAdapterIndex!=-1)
   {
	  nError = NetworkAdapterControl_ControlAdaptor(m_nNetworkAdapterIndex, TRUE);
   }

   return nError;

}
//----------------------------------------------------------------------------
/**
@brief   NetWork_Adapter_Disable 함수
@date    2013.03.26
@param   g_nNetworkAdapterIndex
@return  nError
*/
int NetworkSinalLib::NetWork_Adapter_Disable(void)
{
   int nError=0;

   if(m_nNetworkAdapterIndex!=-1)
   {
	  nError = NetworkAdapterControl_ControlAdaptor(m_nNetworkAdapterIndex, FALSE);
   }

   return nError;

}
//------------------------------------------------------------------------------
/**
@brief   WLanInfo 얻는 함수
@date    2013.03.26
@param   MaxAddress, APSignal, APChannel, APSiganlLevel, SSID
@return  정상이면 true, 에러 발생 시 false 반환
*/
int NetworkSinalLib::Get_WLanInfo(APSIGNAL_DATA* APSignalData)
{
	int nWirelessStatus = INIT_STATUS;
	memset(&m_Info, 0x00, sizeof(INetConnectInfo));
	nWirelessStatus = NetworkAdapterControl_GetINetConnectInfo(m_nNetworkAdapterIndex, &m_Info);
	if(nWirelessStatus == ConnectAP)
   {
		//구조체에 SSID Data 저장
		strcpy(APSignalData->szSSID, m_Info.szSSID);
		trim(APSignalData->szSSID);

		//구조체에 APMaxAddress 저장
		memmove(&APSignalData->szAPMacAddress, m_Info.cAPMAC, sizeof(m_Info.cAPMAC));

		//구조체에 AP채널 및 시그널  저장
		APSignalData->nAPSignal = m_Info.iRssi;
		APSignalData->nChannel = m_Info.iChannel;
		APSignalData->nAPSignalLevel  = m_Info.iRssi;

		if(m_Info.iRssi > -40)
			APSignalData->nAPSignalLevel = -40;
		else if(m_Info.iRssi < -90)
			APSignalData->nAPSignalLevel = -90;

		//구조체에 AP시그널레벨  저장
		APSignalData->nAPSignalLevel = (int)(((double)(APSignalData->nAPSignalLevel - (-90))) / ((-40) - (-90)) * 10 + 0.5);
		return NO_ERR;
   }
   else if(nWirelessStatus == DisconnectAP)
   {
		//구조체에 SSID Data 저장
		// SSID : Null

		//구조체에 APMaxAddress 저장
		// Mac Address : 00-00-00-00-00-00

		//구조체에 AP채널 및 시그널  저장
		APSignalData->nChannel = 0;
		APSignalData->nAPSignalLevel  = 0;
		return NO_ERR;
   }
   else
		return ERRORNetConnectInfo;
}
//------------------------------------------------------------------------------\
