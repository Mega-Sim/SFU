//---------------------------------------------------------------------------

#ifndef PUtilyH
#define PUtilyH
//---------------------------------------------------------------------------
#include <windows.h>
#include <math.h>
#include <vcl.h>
#include <iostream>
#include <algorithm>
#include <mmsystem.h>

void CharToWChar( const char* pstrSrc, wchar_t pwstrDest[] );  //char를 wchar로 컨버트
void WCharToChar(const wchar_t* pwstrSrc, char pstrDest[]); //wchar를 char로  컨버트
char* SubChar(char* chRevData, char* chSndData, int StartPos, int Len);
char* SubChar(char* chRevData, char* chSndData, int StartPos, int Len, int ninitLen);
int CharToInt(char* chData);

void trim(char *);
void rtrim(char *);
void ltrim(char *);

char* my_strstr(const char *s1, const char *s2);   ///< s1에서 s2문자열 검색
int my_strlen(const char *s1);                     ///< 문자열 길이 반환

// shkim
int Get_CboItemIndex(unsigned char var);
unsigned char CboIndex_ToHex(int var);
unsigned int GetOrderID(void);

void Int2Uchar3(unsigned int iInput, unsigned char& cHigh, unsigned char& cMid, unsigned char& cLow);
int Char2Int(unsigned char cHigh, unsigned char cMid, unsigned char cLow);
AnsiString Char2BinString(unsigned char cChar);
unsigned long PreventOverflowGetTimeMs(unsigned long precalc);

void ExecProgram(String sFileName);


#endif

