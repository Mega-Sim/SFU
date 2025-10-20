//---------------------------------------------------------------------------

#pragma hdrstop

#include "PUtily.h"
#include "Protocol_Define_Modify.h"

#pragma hdrstop
//---------------------------------------------------------------------------

//char를 wchar로 컨버트
void CharToWChar( const char* pstrSrc, wchar_t pwstrDest[] )
{
        int nLen = ( int )strlen( pstrSrc ) + 1;
        mbstowcs( pwstrDest, pstrSrc, nLen );
}


//wchar를 char로 컨버트
void WCharToChar(const wchar_t* pwstrSrc, char pstrDest[])
{
      int nLen = ( int )wcslen( pwstrSrc );
	  wcstombs( pstrDest, pwstrSrc, nLen + 1 );
}

/**
@brief   Char 문자열 자르는 함수.
@date    2013.03.14
@param   Char 문자열, 시작 위치, 길이
@return  자른 문자열 리턴
*/
//	버그가 있어 삭제하기로 함.  shkim. 2015-11-18. 로컬메모리를 리턴하면 안됨!
char* SubChar(char* chRevData, char* chSndData, int StartPos, int Len)
{
	// char chSndData[64];
	char* chError = "\0";
	int nDataLen;
//	chSndData = (char *)malloc(sizeof(char)*64);
	nDataLen = strlen(chRevData);
	//1. Error  처리
	if((nDataLen< StartPos)||((nDataLen - StartPos) < Len)||(Len==0))
	{
//		free(chSndData);
		return chError;
	}
	//2. char문자열의 시작위치, 길이만큼 저장.
//	memset(chSndData, 0x00, 10);
	memmove(chSndData, &chRevData[StartPos], Len );

	return chSndData;
}

char* SubChar(char* chRevData, char* chSndData, int StartPos, int Len, int ninitLen)
{
	// char chSndData[64];
	char* chError = "\0";
	int nDataLen;
//	chSndData = (char *)malloc(sizeof(char)*64);
	nDataLen = strlen(chRevData);
	//1. Error  처리
	if((nDataLen< StartPos)||((nDataLen - StartPos) < Len)||(Len==0))
	{
//		free(chSndData);
		return chError;
	}
	//2. char문자열의 시작위치, 길이만큼 저장.
	memset(chSndData, 0x00, ninitLen);

	memmove(chSndData, &chRevData[StartPos], Len );


	return chSndData;
}

/**
@brief   Char를 Int로 변환하는 함수
@date    2013.03.14
@param   Char 문자열
@return  Int 리턴
*/
int CharToInt(char* chData)
{
	int nVal=0;
	char aVal[8];
	int nDataLen=0;
	//1. 최대 처리 문자열 check!
	nDataLen= strlen(chData);
	if(nDataLen > 8)	return -1;

	strcpy(aVal, chData);
	nVal= atoi(aVal);

	return nVal;
}


/**
@brief   Char문자열 앞/뒤 공백 제거
@date    2013.03.14
@param   Char 문자열
@return
*/
void trim(char *str)
{
	ltrim(str);
	rtrim(str);
}
//-----------------------------------------------------------------------------
void rtrim(char *str)
{
	int i=0;
	int len = strlen(str);
	int start = 0;
	int end = len;
	while((start < end) && (str[end-1] <= ' '))
	{
		end--;
	}

	if(start > end)
	{
		memset(str, '\0', len);
	}
	for(i = 0 ; i < end - start ; i++)
	{
		str[i] = str[i + start];
	}

	for( ; i < len ; i++)
	{
		str[i] = '\0';
	}
}
//-----------------------------------------------------------------------------
void ltrim(char *str)
{
	int i=0;
	int len = strlen(str);
	int start = 0;
	int end = len;
	while((start < len) && (str[start] <= ' '))
	{
		start++;
	}

	if(start > end)
	{
		memset(str, '\0', len);
	}
	for(i = 0 ; i < end - start ; i++)
	{
		str[i] = str[i + start];
	}

	for( ; i < len ; i++)
	{
		str[i] = '\0';
	}
}


/**
@brief   문자열을 검색하는 함수
@date    2013.03.14
@param   s1: 검색대상이 되는 문자열		s2:검색어가 되는 문자열
@return  char *
*/
char* my_strstr(const char *s1, const char *s2){
	int i; //s1과 s2를 비교할 때 서로 같은 문자열의 개수를 저장할 변수

	if(*s2=='\0')//그냥 리턴이 입력되면..
		return (char *)s1;//소스가 되는 s1을 그대로 리턴

	else{
		for(;*s1;s1++){//검색대상의 끝까지..
			if(*s1==*s2){//검색어의 첫글자와 검색대상의 문자가 서로 같으면

				//검색대상과 검색어가 일치하는 길이 만큼 i증가
				for(i=1;*(s1+i)==*(s2+i);i++);

				if(i==my_strlen(s2))//i와 검색어의 길이가 같으면
					return (char *)s1;//현재의 s1주소값을 리턴
			}
		}
		return NULL;//일치하는 것이 없으면 NULL 반환
	}
}
//------------------------------------------------------------------------------
/**
@brief   문자열을 길이를 반환하는  함수
@date    2013.03.14
@param   s1: 검색 문자열
@return  길이(int)
*/
int my_strlen(const char *s1){
	int i;
	for(i=0;*s1;s1++, i++);//*s1의 값이 NULL(끝)이 될 때까지 i를 증가
	return i;
}


// 1비트만 1이고 나머지는 0이며, 빠짐이 없이 순서대로인
// 경우에 한해, 값을 넣으면 콤보박스 인덱스 (0부터)를 반환한다

// case 1 => hex value input - combo index ouuput
int Get_CboItemIndex(unsigned char var)
{
	switch(var)
	{
		case 0x01	:	return 0;
		case 0x02	:	return 1;
		case 0x04	:	return 2;
		case 0x08	:	return 3;
		case 0x10	:	return 4;
		case 0x20	:	return 5;
		case 0x40	:	return 6;
		case 0x80	:	return 7;

		default:
			ShowMessage(L"GetIndex_UC - error!!");
			return -1;
   }

}

// case 2 => combo index input - hex value output
unsigned char CboIndex_ToHex(int var)
{
	switch(var)
	{
		case 0	: return 0x01;
		case 1 	: return 0x02;
		case 2	: return 0x04;
		case 3	: return 0x08;
		case 4 	: return 0x10;
		case 5	: return 0x20;
		case 6	: return 0x40;
		case 7	: return 0x80;

		default:
			ShowMessage(L"GetIndex_UC - error!!");
			return 0;
   }

}

// 지시 ID를 자동으로 생성한다
unsigned int GetOrderID(void)
{
	static int iOrder = 0;

	iOrder++;	// 변수는 0부터 시작해도 지시 ID는 1부터 나가도록 함

	if (iOrder > 10000)
	{
		iOrder = 1;
	}

	return iOrder;
}

// unsigned int 값을 넣으면 3바이트의 unsigned char값으로 반환한다
void Int2Uchar3(unsigned int iInput,
				unsigned char& cHigh, unsigned char& cMid, unsigned char& cLow)
{
	unsigned int iTmp;
	iTmp = iInput & 0xFF0000;
	cHigh = iTmp >> 16;
	//cHigh = (iInput & 0xFF0000) >> 16;

	iTmp =  iInput & 0xFF00;
	cMid  = iTmp  >>  8;

	cLow  = iInput & 0xFF;
}

// 3바이트의 unsigned char값을 넣으면 int로 리턴한다
int Char2Int(unsigned char cHigh, unsigned char cMid, unsigned char cLow)
{
	return ((cHigh * 256 * 256) + (cMid * 256) + cLow);
}


AnsiString Char2BinString(unsigned char cChar)
{
	AnsiString strTmp;

	if((cChar & bit_7) > 0)
		strTmp = "1";
	else
		strTmp = "0";

	if((cChar & bit_6) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_5) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_4) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_3) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_2) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_1) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_0) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	return strTmp;
}

unsigned long PreventOverflowGetTimeMs(unsigned long precalc)
{
	double tmpPrecalc = (double)precalc;
	double postcalc = (double)timeGetTime();

	if(tmpPrecalc > postcalc) // 증가시간이므로, 이전에 얻은 시간값보다 향후 얻은 시간값이 작으면 오버플로우된 상태다.
	{
		tmpPrecalc = tmpPrecalc - (double)0x100000000;
	}

	return (unsigned long)(postcalc - tmpPrecalc);
}

void ExecProgram(String sFileName)
{
	HINSTANCE hResult;
    int nResult;
	String strFileName;
	String strRes;

	strFileName = ".\\" + sFileName;

	hResult = ShellExecute(NULL, L"open", strFileName.c_str(), NULL, NULL, SW_SHOW);
	nResult = (int)hResult;
	switch(nResult)
    {
    case 0:
		strRes = "ERROR : The operating system is out of memory or resources.";
        break;
    case ERROR_FILE_NOT_FOUND:
        strRes = "ERROR : The specified file was not found.";
        break;
	case ERROR_PATH_NOT_FOUND:
        strRes = "ERROR : The specified path was not found.";
        break;
	case ERROR_BAD_FORMAT:
        strRes = "ERROR : The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).";
        break;
	case SE_ERR_ACCESSDENIED:
        strRes = "ERROR : The operating system denied access to the specified file.";
        break;
    case SE_ERR_ASSOCINCOMPLETE:
        strRes = "ERROR : The filename association is incomplete or invalid.";
        break;
    case SE_ERR_DDEBUSY:
        strRes = "ERROR : The DDE transaction could not be completed because other DDE transactions were being processed.";
        break;
    case SE_ERR_DDEFAIL:
        strRes = "ERROR : The DDE transaction failed.";
        break;
    case SE_ERR_DDETIMEOUT:
		strRes = "ERROR : The DDE transaction could not be completed because the request timed out.";
        break;
    case SE_ERR_DLLNOTFOUND:
        strRes = "ERROR : The specified dynamic-link library was not found.";
        break;
    case SE_ERR_NOASSOC:
        strRes = "ERROR : There is no application associated with the given filename extension.";
        break;
    case SE_ERR_OOM:
        strRes = "ERROR : There was not enough memory to complete the operation.";
        break;
    case SE_ERR_SHARE:
        strRes = "ERROR : A sharing violation occurred.";
		break;
    default:
		strRes = "OHT Execute ... complete..";
		break;
    }
}
