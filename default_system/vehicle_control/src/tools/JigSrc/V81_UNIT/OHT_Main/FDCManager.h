//---------------------------------------------------------------------------


#ifndef FDCManagerH
#define FDCManagerH
//---------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>
#include <map>
#include <string>
#include <vcl.h>


#include "../Common/UdpClient.h"
#include "LIniFile.h"
#include "List.h"
#include "FDCUnit.h"

/**
@brief	 이상감지 변수
@author  SEMES MoonHS
@date
*/
typedef struct FDC_PARAM_
{
	char    sIPAddress[16];  	    ///< 이상감지 서버와 연결하는 IP Address
	char    sOHTIPAddress[16];  	///< OHT의 IP Address
	int  	nPortNo;				///< 이상감지 서버와 연결하는 Port No

	char    sFDC_OHT_Type[16];       //< OHT의 종류 ex)FOUP, MGZ
	char    sFDC_OHT_No[16];     	//< OHT의 호기 번호
	char	sFDC_HW_Version[16];     //< OHT의 HW 버전 ex)OHT70, OHT 65

	int 	nFDC_Retry_Count_Max;		//< FDC의 최대 재전송 횟수
	int 	nFDC_Retry_Time;			//< FDC의 재전송 주기

	int 	nFDC_WARN_Send_Delay;			//< FDC의 재전송 주기
	bool	bUseFDC;

}FDC_PARAM;

/**
@brief	 이상감지 Parameter
@author  SEMES MoonHS
@date
*/
struct FDC_PARAM_SET
{

	FDC_PARAM     FDC_Param;
};


class FDCUnit;

class FDCManager
{
private:
	CRITICAL_SECTION	m_CS;						///< LogManager가 LogUnit을 처리할 때와 LogUnit class를 연결하는 부분과의 충돌을 막기위한 Critical Section
	LList				m_FDC_List;
	unsigned int		m_nMaxByteSizeOfFDCSum;
	unsigned int		m_nByteSizeOfFDCSum;
	bool 				m_checkFDCFolder;
	bool				m_bFDCUse;

	std::map<std::string, FDCUnit*> m_FDCUnitMap;

	UdpClient 			*m_UdpClient;

public:
	FDCManager(unsigned int nMaxByteSizeOfFDCSum, bool checkFDCFolder);	///< 생성자(nTag:인식자, sLogName:로그 이름, nMaxByteSizeOfLogSum: Log저장 최대 Size~Byte)
	virtual ~FDCManager(void);

	void ChangeMode();

	bool	connectFDCUnit(FDCUnit *pFDCUnit);

    FDCUnit*  getFDCUnit(char *szSavePath, unsigned int nMaxSizeOfFDC);

	bool handleFDCSum();

	bool Get_FDCFolder();

	bool handleFDC_Transmit_Fun();
	bool FDC_Socket_Connection();
	bool FDC_Check_Transmit(char *Msg,char *szTemp, int *msgLength);
	bool AddDrivingInfo(char *Msg,int nLength,char *rtnMSG);

	bool LoadParameter(char* szFilePath, FDC_PARAM_SET* pParamSet);	///< 파라미터 불러오는 함수
	bool SaveParameter(char* szFilePath, FDC_PARAM_SET* pParamSet);	///< 파라미터 저장하는 함수

	bool FDC_Transmit_Snd(FDCUnit *pFDCUnit);
	bool FDC_Transmit_Rcv(FDCUnit *pFDCUnit, char *MSG);

	FDC_PARAM_SET *m_pParamSet;
	FDC_PARAM_SET *m_pTmpParamSet;

private:
	bool    updateParameter();                                      ///< 파라미터 업데이트 하는 함수

protected:
	friend class FDCUnit;
};


#endif
