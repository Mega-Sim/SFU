//---------------------------------------------------------------------------
/**
@file    LIniFile.cpp
@version OHT ver
@brief   NetworkSignallib Class 소스 File
*/
#include <io.h>
#include <string.h>
#include "LIniFile.h"
#include "MinMaxCtrl.h"


//#define ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\INI.log", __VA_ARGS__);\

#define ADD_LOG(...)       WRITE_LOG_UNIT(m_pLogINI, __VA_ARGS__)

//---------------------------------------------------------------------------
/**
@brief   LIniFile 생성자
@author  puting
@date    2013.04.03
*/
LIniFile::LIniFile(char* szIniFileName)
{
	m_pLogINI = getLogUnit("D:\\log\\INI.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	char szTmpFullPathFileName[_MAX_PATH]={0,};
	char* pFullPathResult = NULL;
	
	pFullPathResult = _fullpath(szTmpFullPathFileName, szIniFileName, _MAX_PATH);

	//1. 초기화
	memset(m_szTmpbuffer, 0, 256);
	memset(m_szTmpValue, 0, 256);
	memset(m_szIniFileName, 0, _MAX_PATH);

	if((strlen(szTmpFullPathFileName)!=0)&&(pFullPathResult!=NULL))
	{
		memcpy(m_szIniFileName, szTmpFullPathFileName, strlen(szTmpFullPathFileName));
	}

	//file Check
	if((strlen(szTmpFullPathFileName)!=0)&&(access(szTmpFullPathFileName,00)==0)&&(pFullPathResult!=NULL))
	{
		m_bIniFileExist = true;
	}
	else
	{
		m_bIniFileExist = false;
	}

}
/**
@brief   LIniFile 소멸자
@author  puting
@date    2013.04.03
*/

LIniFile::~LIniFile()
{
	m_bIniFileExist = false;
//	m_szIniFileName="";
}
/**
@brief   LIniFile 유무를 확인하는 함수
@author  puting
@date    2013.04.03
@param   m_bIniFileExist
@return  정상이면 true, 에러 발생 시 false 반환
*/
bool LIniFile::GetIniFileExist()
{
	return  m_bIniFileExist;
}

/**
@brief   LIniFile Write하기 위한 함수
@author  puting
@date    2013.04.03
@param   Section, Ident, Value
@return  정상이면 true, 에러 발생 시 false 반환
*/
bool LIniFile::WriteInteger(char* szSection, char* szIdent, int nValue)
{

	memset(m_szTmpValue, 0, 256);

	sprintf(m_szTmpValue,"%d",nValue);
	ADD_LOG("WriteInteger");
	ADD_LOG("WriteInteger [%s] [%s] = %d", szSection, szIdent, nValue);

	CreateFileofWrite();

	if(strlen(m_szIniFileName)!=0)
	{
		bool bchk;
		bchk = WritePrivateProfileStringA(szSection, szIdent, m_szTmpValue, m_szIniFileName);
		return true;
	}
		return false;
}
bool LIniFile::WriteDouble(char* szSection, char* szIdent, double dValue)
{
	memset(m_szTmpValue, 0, 256);

	sprintf(m_szTmpValue,"%lf",dValue);
	ADD_LOG("WriteDouble [%s] [%s] = %lf", szSection, szIdent, dValue);

	CreateFileofWrite();

	if(strlen(m_szIniFileName)!=0)
	{
		bool bchk;
		bchk = WritePrivateProfileStringA(szSection, szIdent, m_szTmpValue, m_szIniFileName);
		return true;
	}
		return false;

}
bool LIniFile::WriteString(char* szSection, char* szIdent, char* szValue)
{
	CreateFileofWrite();
	ADD_LOG("WriteString [%s] [%s] = %s", szSection, szIdent, szValue);

	if(strlen(m_szIniFileName)!=0)
	{
		bool bchk;
		bchk = WritePrivateProfileStringA(szSection, szIdent, szValue, m_szIniFileName);
		return true;
	}
		return false;
}
bool LIniFile::WriteBool(char* szSection, char* szIdent, bool bValue)
{

	memset(m_szTmpValue, 0, 256);

	sprintf(m_szTmpValue,"%d",bValue);
	ADD_LOG("WriteBool [%s] [%s] = %d", szSection, szIdent, bValue);

	CreateFileofWrite();

	if(strlen(m_szIniFileName)!=0)
	{
		bool bchk;
		bchk = WritePrivateProfileStringA(szSection, szIdent, m_szTmpValue, m_szIniFileName);
		return true;
	}
		return false;
}

/**
@brief   LIniFile Read하기 위한 함수
@author  puting
@date    2013.04.03
@param   Section, Ident, Defualt_Value, Value
@return  정상이면 true, 에러 발생 시 false 반환
*/
bool LIniFile::ReadInteger(char* szSection, char* szIdent, int nDefaultValue, int* pnValue)
{

	if(strlen(m_szIniFileName)!=0)
	{
		 int TempCheck = GetPrivateProfileIntA(szSection, szIdent, 0x0FFFFFFF, m_szIniFileName);
		if(TempCheck == 0x0FFFFFFF)
		{
			*pnValue = nDefaultValue;
			return false;

        }
		else
		{
			*pnValue = TempCheck;
			ADD_LOG("ReadInteger [%s] [%s] = %d", szSection, szIdent, *pnValue);
			return true;
        }

	}
		return false;
}

bool LIniFile::ReadIntegerMinCheck(char* szSection, char* szIdent, int nDefaultValue, int nMinValue, int* pnValue)
{
	int nTmpValue; 
	bool bReturn = ReadInteger(szSection, szIdent, nDefaultValue, &nTmpValue);
	*pnValue = applyMinLimit(nTmpValue, nMinValue);
	return bReturn;
}

bool LIniFile::ReadIntegerMaxCheck(char* szSection, char* szIdent, int nDefaultValue, int nMaxValue, int* pnValue)
{
	int nTmpValue; 
	bool bReturn = ReadInteger(szSection, szIdent, nDefaultValue, &nTmpValue);
	*pnValue = applyMaxLimit(nTmpValue, nMaxValue);
	return bReturn;
}

bool LIniFile::ReadIntegerMinMaxCheck(char* szSection, char* szIdent, int nDefaultValue, int nMinValue, int nMaxValue, int* pnValue)
{
	int nTmpValue, nResult; 
	bool bReturn = ReadInteger(szSection, szIdent, nDefaultValue, &nTmpValue);
	if(applyMinMaxLimit(nTmpValue, nMinValue, nMaxValue, &nResult)==true)
	{
		*pnValue = nResult;
	}
	else
	{
		*pnValue = nTmpValue;
		bReturn = false;
	}
	return bReturn;
}



bool LIniFile::ReadDouble(char* szSection, char* szIdent, double dDefaultValue, double* pdValue)
{
	memset(m_szTmpbuffer, 0, 256);
	memset(m_szTmpValue, 0, 256);

	sprintf(m_szTmpValue,"%lf",dDefaultValue);

	if(strlen(m_szIniFileName)!=0)
	{
		bool bchk;
		int nchk;
		nchk = GetPrivateProfileStringA(szSection, szIdent, NULL, m_szTmpbuffer, sizeof(m_szTmpbuffer), m_szIniFileName);
		if(nchk != 0)
		{

			*pdValue = atof(m_szTmpbuffer);
			//ADD_LOG("ReadDouble [%s] [%s] = %lf", szSection, szIdent, *pdValue);
			return  true;
		}else
		{
           *pdValue = atof(m_szTmpValue);
        }
	}
	return false;
}

bool LIniFile::ReadDoubleMinCheck(char* szSection, char* szIdent, double dDefaultValue, double dMinValue, double* pdValue)
{
	double dTmpValue; 
	bool bReturn = ReadDouble(szSection, szIdent, dDefaultValue, &dTmpValue);
	*pdValue = applyMinLimit(dTmpValue, dMinValue);
	return bReturn;
}

bool LIniFile::ReadDoubleMaxCheck(char* szSection, char* szIdent, double dDefaultValue, double dMaxValue, double* pdValue)
{
	double dTmpValue; 
	bool bReturn = ReadDouble(szSection, szIdent, dDefaultValue, &dTmpValue);
	*pdValue = applyMaxLimit(dTmpValue, dMaxValue);
	return bReturn;
}

bool LIniFile::ReadDoubleMinMaxCheck(char* szSection, char* szIdent, double dDefaultValue, double dMinValue, double dMaxValue, double* pdValue)
{
	double dTmpValue, dResult; 
	bool bReturn = ReadDouble(szSection, szIdent, dDefaultValue, &dTmpValue);
	if(applyMinMaxLimit(dTmpValue, dMinValue, dMaxValue, &dResult)==true)
	{
		*pdValue = dResult;
	}
	else
	{
		*pdValue = dTmpValue;
		bReturn = false;
	}
	return bReturn;
}


bool LIniFile::ReadString(char* szSection, char* szIdent, char* szDefaultValue, char* szValue)
{
	memset(m_szTmpbuffer, 0, 256);

	if(strlen(m_szIniFileName)!=0)
	{
		bool bchk;
		bchk = GetPrivateProfileStringA(szSection, szIdent, szDefaultValue, m_szTmpbuffer, sizeof(m_szTmpbuffer), m_szIniFileName);
		if(bchk == true)
		{
			memcpy(szValue,m_szTmpbuffer, strlen(m_szTmpbuffer));
			ADD_LOG("ReadString [%s] [%s] = %s", szSection, szIdent, szValue);
			return  true;

		}
	}
		return false;

}
bool LIniFile::ReadBool(char* szSection, char* szIdent, bool bDefaultValue, bool* bValue)
{
	if(strlen(m_szIniFileName)!=0)
	{
		int nValue;
		nValue = GetPrivateProfileIntA(szSection, szIdent, bDefaultValue, m_szIniFileName);
		if(nValue == 0)	{*bValue = false; return false;}
		else if(nValue == 1)
		{
			*bValue = true; 
			ADD_LOG("ReadBool [%s] [%s] = %d", szSection, szIdent, *bValue);
			return true;
		}
		else return false;
	}
		return false;
}


bool LIniFile::ReadDoubleMinMaxVerification(char* szSection, char* szIdent, double dDefaultValue, double dMinValue, double dMaxValue, double* pdValue)
{
	double dTmpValue, dResult;
	bool bReturn = ReadDouble(szSection, szIdent, dDefaultValue, &dTmpValue);

	if(bReturn ==true)
	{
		bReturn = applyMinMaxLimitCheck(dDefaultValue,dTmpValue, dMinValue, dMaxValue, &dResult);
		*pdValue = dResult;
	}
	else
	{
		*pdValue = dDefaultValue;
    }

	return bReturn;
}

bool LIniFile::ReadIntegerMinMaxVerification(char* szSection, char* szIdent, int nDefaultValue, int nMinValue, int nMaxValue, int* pnValue)
{
	int nTmpValue, nResult;
	bool bReturn = ReadInteger(szSection, szIdent, nDefaultValue, &nTmpValue);

	if(bReturn ==true)
	{
		bReturn = applyMinMaxLimitCheck(nDefaultValue,nTmpValue, nMinValue, nMaxValue, &nResult);
		*pnValue = nResult;
	}
	else
	{
		*pnValue = nDefaultValue;
	}

	return bReturn;
}


bool LIniFile::ReadBoolVerification(char* szSection, char* szIdent, bool bDefaultValue, bool* bValue)
{
	if(strlen(m_szIniFileName)!=0)
	{
		int nValue;
		nValue = GetPrivateProfileIntA(szSection, szIdent, -1, m_szIniFileName);
		if(nValue == 0)
		{
			*bValue = false;
			return true;
		}
		else if(nValue == 1)
		{
			*bValue = true;
			ADD_LOG("ReadBool [%s] [%s] = %d", szSection, szIdent, *bValue);
			return true;
		}
		else
		{
			*bValue = bDefaultValue;
			return false;
		}
	}

	return false;
}


bool LIniFile::WriteDiffInteger(char* szSection, char* szIdent, int nValue, int nDefaultValue)
{
	if(nValue !=  nDefaultValue)
	{
		 return WriteInteger(szSection, szIdent, nValue);
	}
	else
	{
		return false;
	}
}
bool LIniFile::WriteDiffDouble(char* szSection, char* szIdent, double dValue, double dDefaultValue)
{
	if(dValue !=  dDefaultValue)
	{
		return WriteDouble(szSection, szIdent, dValue);
	}
	else
	{
		return false;
	}
}
bool LIniFile::WriteDiffString(char* szSection, char* szIdent, char* szValue, char* szDefaultValue)
{
	if(strcmp(szValue,szDefaultValue) != 0)
	{
		 return WriteString(szSection, szIdent, szValue);
	}
	else
	{
		return false;
	}
}
bool LIniFile::WriteDiffBool(char* szSection, char* szIdent, bool bValue, bool bDefaultValue)
{
	if(bValue !=  bDefaultValue)
	{
		 return WriteBool(szSection, szIdent, bValue);
	}
	else
	{
		return false;
	}
}


void LIniFile::DeleteIniFile()
{
	if(GetIniFileExist())
	{
		DeleteFileA(m_szIniFileName);
		m_bIniFileExist = false;
	}
}

void LIniFile::CreateFileofWrite()
{
	if(!GetIniFileExist())
	{
		FILE *fp;

		fp = fopen(m_szIniFileName, "a");

		if(fp!=NULL)
		{
			fflush(fp);
			fclose(fp);
			fp = NULL;
		}			

		m_bIniFileExist = true;
    }
}
