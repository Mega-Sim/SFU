/**
@file    STB_Reader.h
@version OHT 7.0
@brief   STB_Reader Class Header File
*/

#ifndef STB_ReaderH
#define STB_ReaderH

//#include "VirtualIdReader.h"
//#include "VirtualIdAndOffsetReader.h"
#include "HWCommon.h"
#include "CommHandler.h"
#include "ErrorCode.h"


/**
@class   STB_Reader
@date
@author
@brief   STB_Reader 관련 Class
*/
class STB_Reader//: public VirtualIdAndOffsetReader
{
private:

	LogUnit *pLogSTBTest;      ///< QRTest 로그 생성을 위한 변수
    Comm_Port *m_pCommPort;
    HWCommon *m_pHWCommon;
    int m_nPortNum;
	//int m_nIndex;
    int m_nDirection;

	double m_dLatestOffsetX;
	double m_dLatestOffsetY;
	double m_dLatestOffsetZ;

	double* m_pdResultOffsetX;
	double* m_pdResultOffsetY;
	double* m_pdResultOffsetZ;
	//int m_MakerType;

	DWORD dChecked62;
	bool b_Checked62;
    char m_szTempReadBuff[1024]; // AOHC-206 QR Class Static 변수 중복 제거

public:
	STB_Reader(HWCommon *pHWCommon, int nComPortNum, bool nDirectionPlus);
    ~STB_Reader();

	//int SetSTBMaker(int nMakerType);
	int GetnOffset(double *pdOffsetX, double *pdOffsetY, double *pdOffsetZ);
	int ParseData(char* szData, double *pdOffsetX, double *pdOffsetY, double *pdOffsetZ);
	int ParseData(char* szData, double *pdOffsetX, double *pdOffsetY, double *pdOffsetZ, double *dX1, double *dX2, double *dX3, double *dX4, double *dY1, double *dY2, double *dY3, double *dY4);

	int SendTriggerOn();
	int ReadBuffer();

	double m_dstSTBOffsetX;
	double m_dstSTBOffsetY;
	double m_dstSTBOffsetZ;


	int Enable();
	void Disable();
	int ReStart();
	bool Connect();
	bool m_bflag;

	int ReadData();

};

#endif  // STB_ReaderH

