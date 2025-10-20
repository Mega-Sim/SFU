//---------------------------------------------------------------------------

#ifndef MyToolsH
#define MyToolsH


#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>


#include <vcl.h>
#include <sstream>
#include <iostream>
#include <string>

#include <vector>
#include <string>



// ***************************************
// MyTools
//
// All Codes Written by SHCHO
//
// to Support on C++ Builder
//
// From 2018.04 ~
//
//
//
//

#define RLOG_UBG_CHK		"D:\\log\\UBGPatternComp"
#define MAIN_EXIT_LOG        "D:\\log\\OHTTerminate"
#define IPC_STAT  			"D:\\log\\IPCVoltTemp"
#define UBG_ABNORMAL_CHECK_LOG        "D:\\log\\UBGAbnormalCheck"
#define OBS_AREA_LOG                  "D:\\log\\OBSAreaCheck"


#define OACIS_LOG_STEER           	  "C:\\User\\OHT\\DataFiles\\OACIS\\Steering.dat"
#define OACIS_LOG_VHLDETECT           "C:\\User\\OHT\\DataFiles\\OACIS\\VHLDetect.dat"
#define OACIS_LOG_OBSDETECT           "C:\\User\\OHT\\DataFiles\\OACIS\\OBSDetect.dat"
#define OACIS_LOG_SHIFTHOME           "C:\\User\\OHT\\DataFiles\\OACIS\\ShiftHome.dat"
#define OACIS_LOG_HOISTHOME           "C:\\User\\OHT\\DataFiles\\OACIS\\HoistHome.dat"
#define OACIS_LOG_SMPSVOLTAGE         "C:\\User\\OHT\\DataFiles\\OACIS\\SMPSVoltage.dat"
#define OACIS_LOG_ALL					"C:\\User\\OHT\\DataFiles\\OACIS\\UnitCheck.dat"


// Rapid Log . 호출되는 즉시 저장됨. Queuing 하지 않음
// pFilePath_withoutExt : 확장자를 제외한 파일경로. 확장자 .log로 알아서 붙임.
// bTime : 로그에 시간을 기록할지 여부.
// bTimeOnFileName : 로그파일에 날짜와시간까지 넣을지 여부.
// 		true 일경우 ex) TestLog 만 줘도 TestLog2018072316.log 파일에 기록함

// 해당 함수 file write/close 하는 부분에서 Hang 발생하여 에러 발생 (AMOHTV80F-1293)
// Auto 주행 중인 상황에서 해당 함수 사용하지 않도록 수정하였음

// void RapidLog(char *pFilePath_withoutExt, char *szBuf, bool bTime = true, bool bTimeOnFileName = true);

// OACISLog 함수도 RapidLog 함수와 동일한 구조이나, Auto 주행 중에 사용하지 않으므로, 변경하지 않음.

void OACISLog(char *pFilePath_withoutExt, char *szBuf, bool bTime = true);
char* OACISType(int TypeNum);
char* OACISResult(int Result);

//void TRACE(char* szMessage, ...);          //미사용

//bool GET_BIT(void* pMem, UINT nBit);                 //미사용
//void SET_BIT(void* pMem, UINT nBit, bool bSet);      //미사용


//void __fastcall SplitString(AnsiString Original, AnsiString Delim, TStrings *List);       //미사용
// shcho
//  StringToHexStream
// ex) [in: ABCDE]   [seperator: ',']   use0x : 사이사이에 0x 넣을지 말지
//     [outout: 65,66,67,68,69]
//bool StringToHexStream(char *in, AnsiString *pOut, char seperator, bool bUse0x);
bool StringToHexStream(char *in, char *pOut, int nOutSize, char seperator, bool bUse0x);
// shcho
//bool __fastcall HexToInt(char *szHex, int *pRet);       //미사용


int GetThisCount_inString(char *str);
int GetThisCount_inString(AnsiString *str);


// SHCHO 2019 : FindStringPos. Pure C++ (검증완료)
// pSrc = "She is beautiful", pFind = "beau"를 주면 7을 리턴. 없으면 -1
// 문자열 제한 길이는 1024
int FindStringPos(char *pSrc, char *pFind);

// SHCHO 2019 GetThisValue  - Pure C++ (검증완료)
//  문자열에서 특정 값을 읽어냄
// ex)
// Px.1=5000.999에서 Px1.=을 주면 5000.999을 읽어내서 int로 혹은 double 둘다 변환시킴
bool GetThisValue(char *pSrc, char *chFind, int *pnVal = NULL, double *pdVal = NULL);


bool IsDigit(char *sz, int nSize);
int CountOfTokens(char *sz, int nSize);
bool IsIpAddress(char *szAddr, int nSize);


// SHCHO
// StrReplace
// pStr에서 from을 찾아서, to로 바꾼다
bool StrReplace(std::string* pStr, const std::string& from, const std::string& to);

// shcho
// char 문자열을 token으로 쪼개서 std::string 스트링 배열로 리턴(벡터)
// include  <vector>, <string> 필요
std::vector<std::string> StrSplit(char *str, char token);

// SHCHO
// 문자열에서 정수 추출
// ex) 1,2,3,4,5  문자열에서 각 숫자 추출해서 vector 정수로 리턴
std::vector<int> ExtractNumber_inString(std::string strInput, char token);


////////////////////////////////////////////////////////////////////////////////
// class CStepDelay
// 단순 Delay
////////////////////////////////////////////////////////////////////////////////
// 사용 예
// CStepDelay Delay1;
// .....
// case 120 : if (NextStepCOnditionOK) {Delay1.Clear();step++;}
//		break;
// .....
// case 121 : if (Delay1.Delay(100)) step++;
//		break;
//

class CStepDelay
{

protected :

	bool bScan;// 0 : TickCount사용,  1 : Scan 단위
	DWORD PreTickTime;
	inline DWORD GetCurTickTime(){return GetTickCount();}
	bool    bPaused;

public :
	bool Out;
	DWORD SetTime;// [ms] 설정 시간
	DWORD CurTime;// [ms] 현재 시간 ( 0 -> 설정시간 )

	static int ScanTime;// [ms]
public :

	//Functions.
	CStepDelay(bool _bScan=true)
	{
		bScan = _bScan;
		bPaused = false;
		SetTime = 0;
		Clear();
	}

	void Clear(void)
	{
		CurTime = 0;
		PreTickTime = GetCurTickTime();
		Out = false;
	}

	bool Delay(int _SetTime)
	{
		SetTime = _SetTime;
		DWORD CurTick = GetCurTickTime();

		if (bScan && !bPaused)CurTime+=ScanTime;
		else if(!bPaused)CurTime += CurTick - PreTickTime;

		if (CurTime>=SetTime)
		{
			CurTime = 0;
			Out = true;
		}
		else
		{
			Out = false;
		}
		PreTickTime = CurTick;
		return Out;
	}

	void Pause()
	{
		bPaused = true;
	}

	void Resume()
	{
		bPaused = false;
	}
};

////////////////////////////////////////////////////////////////////////////////
// class CStepOnDelayTimer
// 단순 Delay + Input조건 검사
////////////////////////////////////////////////////////////////////////////////
class CStepTimer
{

public :
	bool Out;
	DWORD SetTime;// [ms] 설정 시간
	DWORD CurTime;// [ms] 현재 시간 ( 0 -> 설정시간 )

protected :
	bool bScan;// 0 : TickCount사용,  1 : Scan 단위
	DWORD PreTickTime;
	inline DWORD GetCurTickTime(){return GetTickCount();}
	DWORD   PauseTickTime;
	bool    bPaused;

public :

	//Functions.
	CStepTimer(bool _bScan=false)
	{
		bScan = _bScan;
		bPaused = false;
		SetTime = 0;
		PauseTickTime = 0;
		Clear();
	}

	void SetScan(bool _bScan)
	{
		bScan = _bScan;
	}
	void Clear(void)
	{
		OnDelay(false);
	}

	bool OnDelay(bool SeqInput, DWORD _SetTime)
	{
		SetTime = _SetTime;
		return OnDelay(SeqInput);
	}

	bool OnDelay(bool SeqInput)
	{

		DWORD CurTick = GetCurTickTime();
		if (SeqInput && !bPaused)
		{
			if (bScan)CurTime+=CStepDelay::ScanTime;
			else  CurTime += CurTick - PreTickTime;
			if (CurTime>=SetTime)
			{
				CurTime = SetTime;
				Out = true;
			}
			else
			{
				Out = false;
			}
		}
		else
		{
			CurTime = 0;
			Out = false;
		}
		PreTickTime = CurTick;
		return Out;
	}


	void Pause()
	{
		bPaused = true;
		//if (bPaused == false)
		//{
		//PauseTickTime = GetCurTickTime();
		//bPaused = true;
		//}
	}

	void Resume()
	{
		bPaused = false;
		//if (bPaused)
		//{
		//SetTime += GetCurTickTime() - PauseTickTime;
		//bPaused = false;
		//}
	}
};

class CStepClass
{

public:

	CStepClass(){ Step =0;ErrorStep = 0;StepComment = ""; bAllDone = false; nResult = nRetryCnt = 0;}
	int Step;
	AnsiString StepComment;
	int ErrorStep;
	CStepTimer TimeOver;
	CStepTimer StepDelay;

	int nRetryCnt;
    bool bAllDone;
    int nResult;
	
	void SetErrStep()	{ErrorStep = Step;}
};




// GetEternetInfo(..) 함수에서 쓰이는 객체
typedef struct sEthernetInfo
{
	char Name[50];
	char IP[50];
	char SubnetMask[50];
	char Gateway[50];

	sEthernetInfo()	{	ZeroMemory(this, sizeof(*this));	}
} sEthernetInfo;

// SHCHO 2018.10.17  GetEternetInfo(...)
// 각 이더넷카드별로 ip, subnet, gateway를 알아낸다.
// ipconfig를 파싱하여 알아냄 (한글, 영문 윈도우 모두 테스트 완료)
// 랜카드가 여러개일 경우 windows APIs 를 통해서 이더넷 정보를 얻는것 보다 유용함
//
// argement : arrEternetInfo : 랜카드 객체 배열 주소를 전달. 리턴 객체
//          nArrCnt       : 입력 배열 개수
// return : 찾아낸 이더넷 카드 개수
int GetEternetInfo(sEthernetInfo *arrEternetInfo, int nArrCnt);




//2018.10.15 SHCHO : TP가 몇개 접속 되었는지 관리해주는 클래스
#define MAX_TP_CONNECTION	10
#define TP_DISCON_TIME_LIMIT_MS		1000
class MyTPAddrMan
{
	//
	AnsiString 	ArrIPAddr[MAX_TP_CONNECTION];
	DWORD 		LastAddTick[MAX_TP_CONNECTION];
	AnsiString	LastReqAddr;
	int m_nCount;

public:


	MyTPAddrMan(){m_nCount = 0;memset(LastAddTick, 0x00, sizeof(LastAddTick));}

	bool Add(AnsiString ip)
	{
		Refresh();
		if(ip.Length() < 5)
			return false;

		LastReqAddr = ip;

		DWORD curTick = GetTickCount();

		for(int i=0; i<m_nCount ; i++)
		{
			if(ArrIPAddr[i] == ip)
			{
				LastAddTick[i] = curTick;  //tick 갱신
				return true;
            }
		}
		if(m_nCount == MAX_TP_CONNECTION)
			return false;

		// 여기까지 왔다는 것은 중복되는 ip 없음. 새로 접속된 tp이다.
		ArrIPAddr[m_nCount] = ip;
		LastAddTick[m_nCount] = curTick;
		m_nCount++;

		return true;

	}

	int GetCount()
   {
		Refresh();
		return m_nCount;

   }

   bool bIsFirstTP()
   {
		if(m_nCount <= 0)
			return false;

		return LastReqAddr == ArrIPAddr[0];
   }

private:

	void Refresh()
	{
		if(m_nCount == 0)
			return;

		DWORD curTick = GetTickCount();

		for(int i=m_nCount-1 ; i>=0 ; i--)
		{
			// 시간 초과시 IP 지움
			if(curTick - LastAddTick[i] > TP_DISCON_TIME_LIMIT_MS)
			{
				ArrIPAddr[i] = "";
				LastAddTick[i] = 0;
				// 하나 지워졌으니까 배열 앞으로 땡기자
				for(int k=i; k < m_nCount-1 ; k++)
				{
					ArrIPAddr[k] = ArrIPAddr[k+1];
					LastAddTick[k] = LastAddTick[k+1];
				}
				m_nCount--;
			}
		}

		//for debugging
		//for(int i=0; i<m_nCount; i++)
		//	TRACE("TP[%d] : %s", i+1, ArrIPAddr[i]);

	}
};

// Created by SHCHO 2018.10.24 : GetTickCount()를 사용하여 특정부분 실행시간을 측정하고자 할때
// 소스코드가 지저분해 지는데, 이를 가독성도 편하고 쉽게 사용하게 하기 위함

// 사용 예

/*
	TimeChk::Start(0);	// id = 0
	QuickSort(...);
	TimeChk::End(0);

	TimeChk::Start(1);	// id = 1
	BubleSort(...);
	TimeChk::End(1);

	printf("Quick=%d, Bubble=%d", TimeChk::Get(0), TimeChk::Get(1));
*/

// 헤더파일에 정의
class TimeChk
{
public:

	const static int MaxEventCnt = 30;	// 최대 이벤트

	// 인터페이스
	void Start(int nEventID);
	void End(int nEventID);
	DWORD Get(int nEventID);	// msec
	void Init(int nEventID = -1);		// nEventID == -1  전체 타이머 초기화, else 해당 타이머 초기화

	TimeChk();
private:

	DWORD nArrStart[MaxEventCnt];
	DWORD nArrEnd[MaxEventCnt];


};

class SWTimer
{
public:
	SWTimer();

	void Start();

	DWORD GetElipseTime(DWORD nCurT);

private:
	DWORD m_nStartT;
	bool m_ChekEnable;
};



class DTimer
{
	private:
		DWORD dwStartTick;
		bool bAtFirstEnable;

	public:

		DTimer();

		bool Enable(bool bEnable, DWORD dwTime_ms = 0);

		void Init();

};

class DStep
{
public:
	DStep(void);

	int ErrStep;
	int RetryCnt;
	int Result;
	bool bAllDone;
	int TimeOut_ms;
	DTimer Timer;
	DTimer TimeOver;

	int Step;
};

//---------------------------------------------------------------------------
#endif


