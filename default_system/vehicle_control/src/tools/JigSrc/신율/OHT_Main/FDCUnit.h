#ifndef FDCUnitH
#define FDCUnitH


#include <stdio.h>
#include <windows.h>
#include "List.h"
#include "../Common/PUtily.h"
//#include "FDCManager.h"
//#include "OHTMainStatus.h"

typedef struct _FDC_PACKET
{

	int		nLength;
	int 	nTransmit_Count;
	int 	nErrorcode;

	SYSTEMTIME LocalTime;
	DWORD 	dwTransmitTime;

	char	*pcValue;
}FDC_PACKET;


class FDCUnit
{
private:
	char 			*m_szFDCName;

	LList 			m_FDC_List;
	LList 			m_WARN_List;

	unsigned int 	m_nMaxSizeOfFDC;		///< Log List에 저장가능한 Char(BYTE)의 총량(Length, Time 정보 포함)
	unsigned int 	m_nSize;				///< Log List에 저장된 char(BYTE)의 총량(Length, Time 정보 포함)

	bool 		    m_bIsAutoMode;

	int 			m_nOCSErrorcode;

	unsigned int 	m_nCountOfLostLog;

	bool			m_bUseFDC;

//	OHTMainStatus 	*m_pOHTMainStatus;
public:
	FDCUnit(char *szFDCName, unsigned int nMaxSizeOfFDC);
	virtual ~FDCUnit(void);
	bool 	FDC_put(int nLength, char *sFDC);  ///< FDC Log를 입력하는 함수(nLength: sLog길이, dwDrivingTime:작성시간, sLog: 로그, nError: 에러) / 성공하면 true 리턴
    bool 	FDC_put_WARN(int nLength, char *sFDC);  ///< FDC Log를 입력하는 함수(nLength: sLog길이, dwDrivingTime:작성시간, sLog: 로그, nError: 에러) / 성공하면 true 리턴
	void 	Transmitted(FDC_PACKET *pFDCPacket);
	void	Transmit_Insert(FDC_PACKET *pFDCPacket);
	int 	Transmit_Count(FDC_PACKET *pFDCPacket);
	void    Transmit_pop(FDC_PACKET *pFDCPacket, unsigned int nIndex);

	int 	ExtensionData(int nLength, char *pFDC);

	void	changeAutoMode();
	void	changeManualMode(int error);
	void 	changeUseMode();
	void	changeNoUseMode();

	bool	IsAuto();

	bool  	CheckTransmitted(int *preErrorcode, char *pFDC, SYSTEMTIME *nPreErrorcodeTime, int *Errorcode, bool *bIsWARN);

	int 	getOCSErrorcode();

	void	WARN_Insert();

	bool	Check_WARN(FDC_PACKET *pFDCPacket);

	int 	getFDCSize();
	int 	getFDCListSize();

protected:
	bool	FDC_get(LList *pList, int *nError, int*pnCountOfLostLog);

	friend class FDCManager;
};







#endif
