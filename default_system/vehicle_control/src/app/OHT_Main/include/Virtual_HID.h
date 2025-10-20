//---------------------------------------------------------------------------
#ifndef Virtual_HIDH
#define Virtual_HIDH
//---------------------------------------------------------------------------
#include "CommHandler.h"
#include "Def_HW.h"
#include "../Common/Def_DataType.h"
#include "LogHandler.h"

#define  chSTX                      0x02
#define  chETX                      0x03
#define  chRST                      0x1F
#define  chESC                      0x1B


#define  RETRY_HID_MAX_COUNT			5

#define HID_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\HIDRawData.log", __VA_ARGS__)



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
//   HID_LOG("Write 통신 Len=%d, chSndData=%s", Len, chTmpSndData);       //에코용
//	HID_LOG("Write 통신 Len=%d, chSndData= %02x %02x %02x %02x", Len, chTmpSndData[1], chTmpSndData[2], chTmpSndData[3], chTmpSndData[4]);       //에코용
#if((OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
	if(chTmpSndData[1]=='v' && chTmpSndData[2]=='0' && chTmpSndData[3]=='0' && chTmpSndData[4]=='v')      //ECO용으로 인자 바꾸도록
	{
		chTmpSndData[2] = 0;
		chTmpSndData[3] = 0;
	}
	else if(Len == 19) Len = 21;
#endif

	if( Len == (int)(m_HIDCommPort->WriteCommBlock(chTmpSndData,Len)))
	{
//   	  HID_LOG("NO_ERROR == %d", NO_ERROR);
		return NO_ERROR;
	}
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
			HID_LOG("HID Not Open");
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
		//  if(chBuff == cTarget) break;
		 //	Sleep(1);
//		 if(i==0 || i>115) HID_LOG("RtnReadBuff[%d] = %c, Hex=%02x", i, chBuff, chBuff);
#if((OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
		 if(chBuff == cTarget && (i<87 || i>142))    		 //	ECO의 경우 143까지 읽어와야하는데 그 전에 cTarget과 같은 값 존재 가능
		 {
			HID_LOG("Recv END Len = %d", i);
			break;   		 //	ECO?? ??? 143???? ?о??????? ?? ???? cTarget?? ???? ?? ???? ????
		 }
#endif
	}
//	HID_LOG("RtnReadBuff = %s", RtnReadBuff);
	return index;
}


#endif
