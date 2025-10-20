//---------------------------------------------------------------------------
#ifndef Virtual_HIDH
#define Virtual_HIDH
//---------------------------------------------------------------------------
#include "CommHandler.h"
#include "Def_HW.h"
#include "../Common/Def_DataType.h"

#define  chSTX                      0x02
#define  chETX                      0x03
#define  chRST                      0x1F
#define  chESC                      0x1B


#define  RETRY_HID_MAX_COUNT			5




class Virtual_HID
{


protected:

 //	virtual int AnalysisData(char* chRcvData)				=0;


	int m_nHIDComFailCount; ///< 5번이상 연속으로 안되면 더이상 통신하지 말것.

	int Write(char * chSndData);
	int Read(char *, char);
	int Open(int ComPortNum);
	void Close(void);

	Comm_Port* m_HIDCommPort;
	CRITICAL_SECTION	m_CS;
	HID_DATA      m_HIDData;      ///< HID Data 구조체

public:
	Virtual_HID(void){;}
	virtual ~Virtual_HID(void){;}

   	bool IsHIDOpen();

	virtual int AnalysisData(char* chRcvData)				=0;   //test
	virtual int UpdateData(int nMoveInfo, int nPathInfo)	=0;
	virtual HID_DATA GetHIDData(void)		  				=0;

  //	HID_INFO *m_HIDInfo;
};
//----------------------------------------------------------------------------
bool Virtual_HID::IsHIDOpen()
{
	if(m_HIDCommPort!=NULL)
	{
		if(m_HIDCommPort->m_hCOM != INVALID_HANDLE_VALUE)
			return true;
		else
			return false;
	}
	else
		return false;
}

int Virtual_HID::Open(int ComPortNum)
{
	int nError=NO_ERROR;

	if(m_HIDCommPort == NULL)
		m_HIDCommPort	= new Comm_Port(ComPortNum, 9600);

	if(m_HIDCommPort->m_hCOM == INVALID_HANDLE_VALUE)
	{
		delete m_HIDCommPort;
		m_HIDCommPort = NULL;

		return ERR_HID_OPEN;
	}

	return nError;
}
/**
@brief   HID Comm Port를 Close하는 함수
@author  LSJ
@date    2012.11.04
*/
void Virtual_HID::Close()
{
   if(m_HIDCommPort)
   {
	  delete (Comm_Port *) m_HIDCommPort;
	  m_HIDCommPort = NULL;
   }
	//m_pCommPort->Close();
}

/**
@brief   시리얼로 HID 시리얼에 명령을 보내는 함수
@date    2013.03.12
@param   chSndData : HID에 Write할 문자
@return  정상인 경우 0, 에러 발생한 경우 Error Code 리턴
*/
int Virtual_HID::Write(char * chSndData)
{
   char * chTmpSndData;

   if(m_HIDCommPort == NULL) return ERR_HID_OPEN;
   PurgeComm(m_HIDCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);

   chTmpSndData = chSndData;
   int Len =strlen(chTmpSndData);
   if( Len == (int)(m_HIDCommPort->WriteCommBlock(chTmpSndData,Len)))
	  return NO_ERROR;
   else
	  return ERR_HID_WRITE;
}

/**
@brief   Buff에 읽기위한 함수
@date    2013.03.14
@param    buff, StopTarget
@return  길이 반환
*/
int Virtual_HID::Read(char *RtnReadBuff, char cTarget)
{
   int i, index = 0;
   char chBuff;


	for(i=0; i<255; i++)
   {
		if(m_HIDCommPort == NULL)
		{
			index=0;
			break;
		}
		if(m_HIDCommPort->ReadCommBlock(&chBuff,1)==0)  // One Byte Reading
		{
			if(m_HIDCommPort != NULL) PurgeComm(m_HIDCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
			index=0;
			break;
		}
		*(RtnReadBuff + index ) = chBuff;
		 index++;
		 if(chBuff == cTarget) break;
		 //	Sleep(1);
	}
	return index;
}


#endif
