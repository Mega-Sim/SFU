/**
@file    SplashUnit.CPP
$Rev:: 735           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-03-22 1#$:  Date of last commit
@version OHT 7.0
@brief   OHT TP 프로그램 초기화면을 담당하는 SplashForm CPP 파일
*/

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <mmsystem.h>

#include "SplashUnit.h"
#include "Def_TP.h"
#include "MainUnit.h"
#include "SocketUnit.h"

#include "StationManagerUnit.h" // by zzang9un 2013.04.22 : For Test
#include "OHTInfoUnit.h"        // by zzang9un 2013.04.23 : For Test
#include "Define.h"             // by zzang9un 2013.04.23 : For Test
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSplashForm *SplashForm;
//---------------------------------------------------------------------------
#define TIMER_INTERVAL      500     ///< Timer Interval
#define WAITTIME_CONNECT    5000    ///< 연결 대기 시간 10초
#define MAXLENGTH_ID        20      ///< ID 문자열 최대 길이
#define MAXLENGTH_PW        20      ///< Password 문자열 최대 길이
//---------------------------------------------------------------------------

/**
@brief   SplashForm 생성자
@author  zzang9un
@date    2013.04.18
*/
__fastcall TSplashForm::TSplashForm(TComponent* Owner)
    : TForm(Owner)
{
    // Active Paqge는 Login page로 고정
    pgcSplash->ActivePageIndex = 0;

    // timer 초기화
    timerSplash->Enabled = false;
    timerSplash->Interval = TIMER_INTERVAL;

    // 메모 초기화
    memoSplash->Clear();

    // progressbar 초기화
    pbSplash->Max = STATE_END;

    m_strID = NULL;
    m_strPassword = NULL;

    // SplashForm Show
    //Show();
}
//---------------------------------------------------------------------------

/**
@brief   SplashForm Timer

         OHT TP 프로그램 초기화 루틴을 이 함수에서 구현한다.
@author  zzang9un
@date    2013.04.19
*/
void __fastcall TSplashForm::timerSplashTimer(TObject *Sender)
{
    static INIT_STATE Step = STATE_NONE;
    static DWORD BeginTime = 0;
    String strLog = NULL;

    pbSplash->Position = Step;

    if(Step == STATE_MAX)
    {
        // 마지막 Step인 경우는 Splash 종료
        timerSplash->Enabled = false;
        Close();
        MainForm->Show();
    }

    switch(Step)
    {
    case STATE_NONE:
        BeginTime = timeGetTime(); // 시작 시작 기록
        strLog.sprintf(L"Data Server(%s) 연결 시도 중...", m_strDataServerIPAddr);
        PrintMemo(COLOR_BLACK, strLog);
        Step = STATE_CONNECTCHECK;
        break;

    case STATE_CONNECTCHECK:    // Data Server Connect 체크
        if(SocketForm->DataServerSocket->Socket->Connected)
        {
            strLog.sprintf(L"Data Server(%s) 연결 성공...", m_strDataServerIPAddr);
            PrintMemo(COLOR_BLACK, strLog);
            Step = STATE_DATAVERREQUEST;
        }

        if((Step == STATE_CONNECTCHECK) && (timeGetTime() - BeginTime > WAITTIME_CONNECT))
        {
            Step = STATE_CONNECTFAIL;
        }

        break;

    case STATE_DATAVERREQUEST:  // Data 버전 요청
        // Data Sever에 버전 요청 정보 전송

        PrintMemo(COLOR_BLACK, "Data 버전 정보 요청...");
        Step = STATE_STATIONSYNC;
        break;

    case STATE_STATIONSYNC:     // Station Map Data 다운로딩
        PrintMemo(COLOR_BLACK, "Station Map Data 다운로딩...");
        Step = STATE_STATIONSYNCDONE;
        break;

    case STATE_STATIONSYNCDONE: // Station Map Data 다운로딩 완료
        PrintMemo(COLOR_BLACK, "Station Map Data 다운로딩 완료...");
        Step = STATE_TEACHINGSYNC;
        break;

    case STATE_STATIONSYNCFAIL: // Station Map Data 다운로딩 Fail
        PrintMemo(COLOR_BLACK, "Station Map Data 다운로딩 실패...");
        Step = STATE_TEACHINGSYNC;
        break;

    case STATE_TEACHINGSYNC:     // Teaching Data 다운로딩
        PrintMemo(COLOR_BLACK, "Teaching Data 다운로딩...");
        Step = STATE_TEACHINGSYNCDONE;
        break;

    case STATE_TEACHINGSYNCDONE: // Teaching Data 다운로딩 완료
        PrintMemo(COLOR_BLACK, "Teaching Data 다운로딩 완료...");
        Step = STATE_DATASYNCCOMPLETE;
        break;

    case STATE_TEACHINGSYNCFAIL: // Teaching Data 다운로딩 Fail
        PrintMemo(COLOR_BLACK, "Teaching Data 다운로딩 실패...");
        Step = STATE_TEACHINGSYNC;
        break;

    case STATE_DATASYNCCOMPLETE:
        PrintMemo(COLOR_BLACK, "Data 동기화 완료...");
        Step = STATE_END;
        break;

    case STATE_DATASYNCFAIL:
        PrintMemo(COLOR_RED, "Data 동기화 실패...");
        Step = STATE_END;
        break;

    case STATE_CONNECTFAIL:
        PrintMemo(COLOR_RED, "Data Server 연결 실패...");
        Step = STATE_END;
        break;

    case STATE_END:
        PrintMemo(COLOR_BLUE, "초기화 완료...");
        Step = STATE_MAX;
        break;
    }
}
//---------------------------------------------------------------------------

/**
@brief   SplashForm 메모에서 현재 TTextAttributes를 리턴하는 함수
@author  zzang9un
@date    2013.04.19
@return  TTextAttributes    
*/
TTextAttributes *__fastcall TSplashForm::CurrText(void)
{
	return memoSplash->SelAttributes;
}
//---------------------------------------------------------------------------

/**
@brief   SplashForm 메모에 출력하는 함수
@author  zzang9un
@date    2013.04.18
@param   TmpFontColor : 텍스트 색상
@param   strDisplayData : 메모에 출력할 문자열
*/
void TSplashForm::PrintMemo(TColor TmpFontColor, String strDisplayData)
{
	if(memoSplash->Lines->Count>500)
		memoSplash->Clear();

	CurrText()->Color = TmpFontColor;
	memoSplash->Lines->Add(strDisplayData);
}
//---------------------------------------------------------------------------

/**
@brief   Login 버튼 클릭 시 호출되는 이벤트 함수
@author  zzang9un
@date    2013.04.19
*/
void __fastcall TSplashForm::btnLoginClick(TObject *Sender)
{
    m_strID = edtID->Text;
    m_strPassword = edtPassword->Text;
    m_strDataServerIPAddr = edtDataServerIPAddr->Text;

    // 에러 처리
    if(m_strID.Length() > MAXLENGTH_ID)
    {
        Application->MessageBoxW(L"ID는 20자 이내로 입력하세요.", L"Error", MB_OKCANCEL);
        return;
    }

    if(m_strPassword.Length() > MAXLENGTH_PW)
    {
        Application->MessageBoxW(L"Password는 20자 이내로 입력하세요.", L"Error", MB_OKCANCEL);
        return;
    }

    if(IsAbailableIP(m_strDataServerIPAddr) == false)
    {
        Application->MessageBoxW(L"IP 주소 형식이 틀립니다.", L"Error", MB_OKCANCEL);
        return;
    }

    // Data Server에 로그인 처리
    SocketForm->ConnectDataServer(m_strDataServerIPAddr);
    timerSplash->Enabled = true;

    // by zzang9un 2013.04.23 : 무조건 개발자로 로그인되도록 임시 변경
    gm_OHTStatus->nProgram_Mode = MODE_SETUP;
    MainForm->panUser->Caption = "개발자";

    // Active Paqge는 memo가 있는 페이지로 변경
    pgcSplash->ActivePageIndex = 1;
}
//---------------------------------------------------------------------------

/**
@brief   문자열이 IP 주소로 사용될 수 있는 지를 체크하는 함수
@author  zzang9un
@date    2013.04.19
@param   strIPAddr : 체크할 문자열
@return  IP 주소로 사용할 수 있는 문자열이면 true, 그렇지 않은 경우 false를 리턴
*/
bool TSplashForm::IsAbailableIP(String strIPAddr)
{
    if(strIPAddr == NULL )
        return FALSE;
        
    int len = strIPAddr.Length();
 
    // 7자( 1.1.1.1 ) 이상&& 15자( 123.123.123.123 ) 이하
    if(len > 15 || len < 7)
        return FALSE;
 
    int nNumCount = 0;
    int nDotCount = 0;

    // 유효성검사
    for(int i = 1; i < len + 1; i++)
    {
        TCHAR ch = strIPAddr[i];

        if(ch < '0' || ch > '9')
        {
            if('.' == ch)
            {
                ++nDotCount;
                nNumCount = 0;    
            }
            else
                return FALSE;
        }
        else
        {
            if(++nNumCount > 3)
                return FALSE;
        }
    }
 
    if(nDotCount != 3)
        return FALSE;
 
    return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TSplashForm::imgOHTClick(TObject *Sender)
{
    // by zzang9un 2013.04.22 : For Test
    StationManagerForm->Show();
}
//---------------------------------------------------------------------------

