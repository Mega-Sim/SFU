#pragma once
#include <iostream>

// Author : SHCHO 2019.03
// NumericSystem Class  
// 10진수를 N진수로 바꿔서 Byte 수를 줄일수 있다.
// Pure C++ Code
// 모든 C++ 플랫폼에서 사용 가능
//
/* USAGE # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

	슬러시는 구분자, 나머지는 90진법으로 변환
	90진법 Table은 키보드에서 입력할수 있는 글자로만 함. 몇가지 -, %, Z, / 등등 예약어 제외
	아스키코드값으로 하지 않는 이유는 윈도우 언어설정, 파일 인코딩 방식에 따라 값이 깨질수 있기 때문
	0이 많이 연속할수록 효율이 좋음 -> 0/0/0/0 과 같이 0이 연속하면 Z4 로 변환함
	값이 큰수일수록 효율이 좋음. INT_MAX 값도 5자 이내 표현가능.  2147483647 -> w#)DB
	
	주의할 점
	16진수나 문자열이 값에 포함되어있으면 안됨.


 사용 예제

char orgData[] = "/1111/-371/-2504/0/1/1910889/1913072/0/-1/1100/250000/250000/0/0/30801920/4320880/0/-869269219/-2005090179/8388736/9/9/1820851/1834943/0/1787929/329/510/1199/1199/0/0/0/4000/4000/0/0";
//char p[] = "/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0";
char szNSysData[4096];
char szDecData[4096];
memset(szNSysData, 0, sizeof(szNSysData));
memset(szDecData, 0, sizeof(szDecData));

// 로그 인코딩
NumericSystem::EncodingLog(orgData, szNSysData, sizeof(szNSysData));

// 로그 디코딩
NumericSystem::DecodingLog(szNSysData, szDecData, sizeof(szDecData));

// 원래 값을 인코딩 -> 디코딩 시킨 값은 동일해야함. nCmp 값은 0이 됨
int nCmp = strcmp(szDecData, orgData);

// szNSysData 결과 : "/b</-3/-q\"/0/1/2y,i/2zh~/0/-1/b\\/s:g/s:g/Z2/5yS7/0/-bV#f[/-q_-cG/aD:H/9/9/2oN4/2qeR/0/2k%4/3O/5J/c;/c;/Z3/H1/Z2"
*/


const char TABLE_CHAR90[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', '`', '~', '!', '@', '#', '$', '^', '&', '*', '(', ')', '_', '=', '+', '\\', '|', '[', '{', ']', '}', ';', ':', '\'', '\"', ',', '<', '.', '>', '?' };

class NumericSystem
{
public:
	
	static int TableChar90(char sz)
	{
		switch (sz)
		{
		case '0':	return 0;
		case '1':	return 1;
		case '2':	return 2;
		case '3':	return 3;
		case '4':	return 4;
		case '5':	return 5;
		case '6':	return 6;
		case '7':	return 7;
		case '8':	return 8;
		case '9':	return 9;
		case 'a': return 10;
		case 'b': return 11;
		case 'c': return 12;
		case 'd': return 13;
		case 'e': return 14;
		case 'f': return 15;
		case 'g': return 16;
		case 'h': return 17;
		case 'i': return 18;
		case 'j': return 19;
		case 'k': return 20;
		case 'l': return 21;
		case 'm': return 22;
		case 'n': return 23;
		case 'o': return 24;
		case 'p': return 25;
		case 'q': return 26;
		case 'r': return 27;
		case 's': return 28;
		case 't': return 29;
		case 'u': return 30;
		case 'v': return 31;
		case 'w': return 32;
		case 'x': return 33;
		case 'y': return 34;
		case 'z': return 35;
		case 'A': return 36;
		case 'B': return 37;
		case 'C': return 38;
		case 'D': return 39;
		case 'E': return 40;
		case 'F': return 41;
		case 'G': return 42;
		case 'H': return 43;
		case 'I': return 44;
		case 'J': return 45;
		case 'K': return 46;
		case 'L': return 47;
		case 'M': return 48;
		case 'N': return 49;
		case 'O': return 50;
		case 'P': return 51;
		case 'Q': return 52;
		case 'R': return 53;
		case 'S': return 54;
		case 'T': return 55;
		case 'U': return 56;
		case 'V': return 57;
		case 'W': return 58;
		case 'X': return 59;
		case 'Y': return 60;
		case '`': return 61;
		case '~': return 62;
		case '!': return 63;
		case '@': return 64;
		case '#': return 65;
		case '$': return 66;
		case '^': return 67;
		case '&': return 68;
		case '*': return 69;
		case '(': return 70;
		case ')': return 71;
		case '_': return 72;
		case '=': return 73;
		case '+': return 74;
		case '\\': return 75;
		case '|': return 76;
		case '[': return 77;
		case '{': return 78;
		case ']': return 79;
		case '}': return 80;
		case ';': return 81;
		case ':': return 82;
		case '\'': return 83;
		case '\"': return 84;
		case ',': return 85;
		case '<': return 86;
		case '.': return 87;
		case '>': return 88;
		case '?': return 89;
		}
		return 0;
	}
	
	// N진수를 10 진수로
	static int N2Dec(char *pData, int nSize)
	{
		int nSystem = sizeof(TABLE_CHAR90);
		int nSum = 0;
		bool bMinus = false;
		for (int i = 0; i < nSize; i++)
		{
			char sz = pData[i];
			if (sz == '-' && i == 0)
			{
				bMinus = true;
				continue;
			}
			int nPow = nSize - i - 1;
			long nVal = TableChar90(sz);
			nSum += nVal * pow(nSystem, nPow);
		}
		return nSum * (bMinus ? (-1) : 1);
	}

	// 진법 변환 10진법 -> N진법
	static void Dec2N(const int dec, char *pOut, int nBufSize)
	{
		int n = -100;

		memset(pOut, 0, nBufSize);

		int nArrMod[100];    // 나머지를 저장한 배열
		memset(nArrMod, 0, sizeof(nArrMod));
		int nSys = sizeof(TABLE_CHAR90);    // 몇 진법

		bool bMinus = dec < 0;
		int nVal = abs(dec);    // 나눈 값
		int nMod = 0;    // 나머지
		int nPos = 0;    // 배열 인덱스

						 // 4진법일 경우 4진법 보다 나눈값이 크면, 나눈다
		while (nVal >= nSys)
		{
			int nMod = nVal % nSys;
			nVal = nVal / nSys;


			nArrMod[nPos] = nMod;    // 배열에 저장
			nPos++;
		}

		// 마지막 몫을 배열 맨 뒤에 넣는다
		nArrMod[nPos++] = nVal;

		// 저장한 배열을 반대로 돌면서 pOut에 TABLE 코드표 대로 넣는다
		int nI = 0;
		for (int i = nPos - 1; i >= 0; i--)
		{
			if (bMinus && nI == 0)
				pOut[nI++] = '-';	// 음수처리

			int nSysIdx = nArrMod[i];
			pOut[nI++] = TABLE_CHAR90[nSysIdx];
		}

		pOut[nI] = '\0';    // 마지막 널문자
	}

	// SHCHO 2019 : FindStringPos. Pure C++
	// pSrc = "She is beautiful", pFind = "beau"를 주면 7을 리턴. 없으면 -1
	// 문자열 제한 길이는 1024
	static int FindStringPos(char *pSrc, char *pFind)
	{
		if (pSrc == NULL || pFind == NULL)
			return -1;

		int nLen = strlen(pSrc);
		int nFindLen = strlen(pFind);

		// 
		if (nLen > 1024 || nFindLen > 1024 || nFindLen > nLen || nLen == 0 || nFindLen == 0)
			return -1;

		int nMatchCnt = 0;
		int nStartPos = -1;
		for (int i = 0; i < nLen; i++)
		{
			char cO = pSrc[i];
			char cF = pFind[nMatchCnt];
			if (cO == cF)
			{
				if (nMatchCnt == 0)
					nStartPos = i;	// 시작위치

				if (nMatchCnt == nFindLen - 1)	// 모두 맞음
					return nStartPos;

				nMatchCnt++;
			}
			else
			{
				nMatchCnt = 0;
				nStartPos = -1;
			}
		}
		return -1;
	}

	// SHCHO 2019 GetThisValue  - Pure C++
	//  문자열에서 특정 값을 읽어냄
	// ex)
	// Px.1=5000에서 =을 주면 5000을 읽어냄
	static int GetThisValue(char *pSrc, char *chFind, int *pnVal, double *pdVal)
	{
		int nLen = strlen(pSrc);
		int nPos = -1;

		nPos = FindStringPos(pSrc, chFind);

		if (nPos < 0)
			return -1;

		nPos += strlen(chFind);

		char szVal[64];
		memset(szVal, 0, sizeof(szVal));
		int nAddPos = 0;
		bool bDot = false;
		// 하나씩 검색하면서 숫자 or 부호가 아닌 char 가 나올때 까지 reading 한다
		for (int i = nPos; i < nLen; i++)
		{
			if (sizeof(szVal) - 1 < nAddPos)     // over flow 방지
				return false;

			char c = pSrc[i];
			if (c >= '0' && c <= '9')
			{
				szVal[nAddPos++] = c;
			}
			else if (nAddPos == 0 && c == '-')	// 첫글자는 음수 부호가 올수도 있다
			{
				szVal[nAddPos++] = c;
			}
			else if (bDot == false && c == '.')  // 소수점이 한번도 안쓰였으면 한번은 쓰일수있다
			{
				bDot = true;
				szVal[nAddPos++] = c;
			}
			else
				break;
		}


		//if (nAddPos == 0)	// 읽은 값이 없다
		//	return nPos;

		szVal[nAddPos] = '\0';		// null char

		if (pnVal)
		{
			if (szVal[0] == '.')
				return false;
			*pnVal = atoi(szVal);
		}

		if (pdVal)
			*pdVal = atof(szVal);


		return nPos;
	}

	static int GetThisValueString(char *pSrc, char chSpliter, char *szOutBuf, int nOutBufSize)
	{
		int nLen = strlen(pSrc);
		int nPos = -1;
		char s[2];
		sprintf(s, "%c", chSpliter);
		nPos = FindStringPos(pSrc, s);

		if (nPos < 0)
			return -1;

		nPos += 1;

		char szVal[64];
		memset(szVal, 0, sizeof(szVal));
		int nAddPos = 0;
		bool bDot = false;
		// 하나씩 검색하면서 다음 spliter가 나올때까지 검색
		for (int i = nPos; i < nLen; i++)
		{
			if (sizeof(szVal) - 1 < nAddPos)     // over flow 방지
				return -1;

			char c = pSrc[i];
			if (c == chSpliter)
				break;

			szVal[nAddPos++] = c;
		}


		//if (nAddPos == 0)	// 읽은 값이 없다
		//	return nPos;

		szVal[nAddPos] = '\0';		// null char
		strcpy(szOutBuf, szVal);

		return nPos;
	}

	// /1111/-371/-2504/0/1/1910889/1913072/0/-1/1100/250000/250000/0/0/30801920/4320880/0/-869269219/-2005090179/8388736/9/9/1820851/1834943/0/1787929/329/510/1199/1199/0/0/0/4000/4000/0/0
	static bool EncodingLog(char *pSrc, char *pOut, int nOutSize)
	{
		int nLen = strlen(pSrc);
		int nOffsetPos = 0;
		static const int ARR_SIZE = 1024;
		int nArrData[ARR_SIZE];
		int nArrIdx = 0;

		// [/]슬러시 기준으로 데이터를 파싱해서 int 형 배열에 저장한다
		while (true)
		{
			int nVal = 0;

			int pos = GetThisValue(pSrc + nOffsetPos, "/", &nVal, NULL);
			if (pos < 0)
				break;

			if (nArrIdx >= ARR_SIZE)	// buffer overflow
				return false;

			nArrData[nArrIdx++] = nVal;

			nOffsetPos += pos + 1;
			if (nLen <= nOffsetPos)
				break;
		}

		// 저장된 int 배열을 진법 변환하여 char저장
		char tmpBuf[64];
		memset(tmpBuf, 0, sizeof(tmpBuf));

		char newLogLine[4096];
		memset(newLogLine, 0, sizeof(newLogLine));

		for (int i = 0; i < nArrIdx; i++)
		{
			// 0이 몇번 연속하는지 조사
			int nZeroCnt = 0;
			while (i < nArrIdx)
			{
				if (nArrData[i] != 0)
					break;
				nZeroCnt++;
				i++;
			}

			// 0이 한번 초과하게 연속하면 Z로 치환한다. 한개면 치환안함. 한개는 치환하는게 오히려 손해
			if (nZeroCnt == 1)
				i--;

			else if (nZeroCnt > 1)
			{
				Dec2N(nZeroCnt, tmpBuf, sizeof(tmpBuf));
				if (strlen(tmpBuf) >= 1)
				{
					strcat(newLogLine, "/Z");	//Z몇 <- 이런식.
					strcat(newLogLine, tmpBuf);
					i--;
					continue;
				}
			}

			Dec2N(nArrData[i], tmpBuf, sizeof(tmpBuf));
			strcat(newLogLine, "/");
			strcat(newLogLine, tmpBuf);
		}
		int nNewLen = strlen(newLogLine);
		if (nOutSize < nNewLen)
			return false;

		strcpy(pOut, newLogLine);
		return true;
	}

	static bool DecodingLog(char *pSrc, char *pOut, int nOutSize)
	{
		int nLen = strlen(pSrc);
		int nOffsetPos = 0;
		static const int ARR_SIZE = 1024;
		int nArrData[ARR_SIZE];
		int nArrIdx = 0;

		// [/]슬러시 기준으로 데이터를 파싱해서 int 형 배열에 저장한다
		while (true)
		{
			char buf[64];
			memset(buf, 0, sizeof(buf));
			int pos = GetThisValueString(pSrc + nOffsetPos, '/', buf, sizeof(buf));
			if (pos < 0)
				break;

			if (nArrIdx >= ARR_SIZE)	// buffer overflow
				return false;

			//Zb 같이 나온경우 Z는 0이 연속한다는 얘기고, b는 변환값 11이니까, 0이 11개 연속하게 만들어줌
			if (strlen(buf) > 1)
			{
				if (buf[0] == 'Z')
				{
					char bufZero[64];
					memset(bufZero, 0, sizeof(bufZero));
					strcpy(bufZero, buf + 1);
					int nZeroCnt = N2Dec(bufZero, strlen(bufZero));
					for (int y = 0; y < nZeroCnt; y++)
					{
						nArrData[nArrIdx++] = 0;
					}
					nOffsetPos += pos + 1;
					if (nLen <= nOffsetPos)
						break;
					continue;
				}
			}

			int nVal = 0;

			// 진법 변환하여 int 배열에 저장
			nVal = N2Dec(buf, strlen(buf));
			nArrData[nArrIdx++] = nVal;

			nOffsetPos += pos + 1;
			if (nLen <= nOffsetPos)
				break;
		}


		char newLogLine[4096];
		memset(newLogLine, 0, sizeof(newLogLine));

		for (int i = 0; i < nArrIdx; i++)
		{
			char tmpp[64];
			sprintf(tmpp, "/%d", nArrData[i]);

			int nLen = strlen(newLogLine) + strlen(tmpp) + 1;
			if (nLen > sizeof(newLogLine))
				break;		// buffer over flow case

			strcat(newLogLine, tmpp);
		}


		if (nOutSize < strlen(newLogLine))
			return false;

		strcpy(pOut, newLogLine);
		return true;
	}

};
