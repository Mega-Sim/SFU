/**
@file    Lamp.cpp
@version OHT 7.0
@brief   Lamp Class CPP File
*/

#include "Lamp.h"

/**
@brief   Lamp Class 생성자
@author
@date
@param   pHWCommon : HWCommon Class의 포인터
*/
Lamp::Lamp(HWCommon *pHWCommon)
{
    m_pHWCommon = pHWCommon;
}

/**
@brief   Lamp Class 소멸자
@author
@date
*/
Lamp::~Lamp()
{
}

/**
@brief   모든 색의 Lamp를 On하는 함수
@author  LSJ
@date    2012.10.31
@param   nLampStatus(Color 및 켜고 있을건지 깜박일 건지 결정)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
@Modify  P-PJT P1_2L OHT I/O추가에 따른 Lamp동작 변경 #JRespect 2018.01.03
*/
int Lamp::LampOn(int nLampStatus)
{
    int nError = NO_ERR;

    switch (nLampStatus)
	{
    case SETLAMP_R_ON:
    case SETLAMP_R_FLICKER:
        nError = m_pHWCommon->SetBit(OUT_LAMP_RED, ON);
        break;
    case SETLAMP_Y_ON:
    case SETLAMP_Y_FLICKER:
		//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW, ON);
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, ON);
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, ON);
        break;
    case SETLAMP_G_ON:
    case SETLAMP_G_FLICKER:
        nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
        break;
    case SETLAMP_ALL_ON:
        nError = m_pHWCommon->SetBit(OUT_LAMP_RED, ON);
        if (nError == NO_ERR)
			//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW, ON);
			 nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, ON);
			 nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
		break;

    // Red 점등, Green 점멸
	case SETLAMP_R_ON_G_FLICKER:
		//nError = m_pHWCommon->SetBit(OUT_LAMP_RED, ON);
			 nError = m_pHWCommon->SetBit(OUT_LAMP_RED, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
		break;

	// Yellow 점멸, Yellow 점멸
	case SETLAMP_G_FLICKER_Y_FLICKER:
			nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, ON);
			nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
		break;

	// Green 점멸, Yellow_LEFT 점멸
	case SETLAMP_G_FLICKER_Y_L_FLICKER:
	   //nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, ON);
        if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
		break;
	// Green 점멸, Yellow_RIGHT 점멸
	case SETLAMP_G_FLICKER_Y_R_FLICKER:
		//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
		break;

    }
    return nError;
}
int Lamp::LampYellowSteerOn(int nLampStatus)
{
    int nError = NO_ERR;

    switch (nLampStatus)
	{
	// Green 점멸, Yellow_LEFT 점멸
	case SETLAMP_G_FLICKER_Y_L_FLICKER:
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, ON);
		break;
	// Green 점멸, Yellow_RIGHT 점멸
	case SETLAMP_G_FLICKER_Y_R_FLICKER:
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, ON);
		break;

    }
    return nError;
}
/**
@brief   모든 색의 Lamp를 Off하는 함수
@author  LSJ
@date    2012.10.31
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
@Modify  P-PJT P1_2L OHT I/O추가에 따른 Lamp동작 변경 #JRespect 2018.01.03
*/
int Lamp::LampOff()
{
    int nError = 0;

    nError = m_pHWCommon->SetBit(OUT_LAMP_RED, OFF);

    if (nError == NO_ERR)
		//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW, OFF);
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, OFF);
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, OFF);

    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, OFF);

    return nError;
}

/**
@brief   Red 를 제외한 Lamp를 Off하는 함수
@date    2016.03.10
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
@Modify  P-PJT P1_2L OHT I/O추가에 따른 Lamp동작 변경 #JRespect 2018.01.03 현재사용안함
**/
int Lamp::LampOff_Except_Red()
{
    int nError = 0;

	//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW, OFF);
	nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, OFF);
	nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, OFF);

    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, OFF);

    return nError;
}
/**
@brief   Yellow Lamp를 Off하는 함수
@author  JRespect
@date    2018.01.09
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
**/
int Lamp::LampOff_Yellow()
{
    int nError = 0;

	//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW, OFF);
	nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, OFF);
	nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, OFF);
    return nError;
}
/**
@brief   Lamp 관련 출력의 On/Off 유무 화인 함수
@author  LSJ
@date    2012.10.31
@param   nColor(Red : 0, Yellow : 1, Green : 2)
@return  출력이 켜져 있으면 true, 꺼져 있으면 false 리턴.
@Modify  P-PJT P1_2L OHT I/O추가에 따른 Lamp동작 변경 #JRespect 2018.01.03
*/
bool Lamp::IsLampOn(int nColor)
{
    bool bResult = false;

    switch (nColor)
    {
    case DEF_RED:
        if (m_pHWCommon->m_Output.LAMP_Red == ON)
            bResult = true;
        else
            bResult = false;
        break;
	case DEF_YELLOW_RIGHT:
		//if (m_pHWCommon->m_Output.LAMP_Yellow == ON)
		if (m_pHWCommon->m_Output.LAMP_Yellow_Right == ON)
			bResult = true;
		else
			bResult = false;
		break;
	case DEF_YELLOW_LEFT:
		//if (m_pHWCommon->m_Output.LAMP_Yellow == ON)
		if (m_pHWCommon->m_Output.LAMP_Yellow_Left == ON)
			bResult = true;
		else
			bResult = false;
		break;
	case DEF_GREEN:
        if (m_pHWCommon->m_Output.LAMP_Green == ON)
            bResult = true;
        else
            bResult = false;
        break;
    }

    return bResult;
}

/**
@brief   OHT의 상태에 따라서 Lamp를 Control하는 함수
@author  LSJ
@date    2012.10.31
@param   nLampStatus(OHT의 Status에 따라서 상태 결정)
@return  SetBit에 문제 없으면 0, 에러 발생 시 에러 코드 Return
@Modify  P-PJT P1_2L OHT I/O추가에 따른 Lamp동작 변경 #JRespect 2018.01.03 현재사용 안함
*/
int Lamp::LampControl(int nLampStatus)
{
    int nError = NO_ERR;
	int nRedOut = 0, nGreenOut = 0;
	//int nYellowOut = 0;
	int nYellowRightOut = 0,  nYellowLeftOut = 0;
    static int nPreLampStatus = -1;

	nRedOut = IsLampOn(DEF_RED);
	//nYellowOut = IsLampOn(DEF_YELLOW);
	nGreenOut = IsLampOn(DEF_GREEN);
	nYellowLeftOut = IsLampOn(DEF_YELLOW_LEFT);
	nYellowRightOut = IsLampOn(DEF_YELLOW_RIGHT);

    switch (nLampStatus)
    {
	case SETLAMP_ALL_OFF:
		//if ((nRedOut == 1) || (nYellowOut == 1) || (nGreenOut == 1))
		if ((nRedOut == 1) || (nYellowLeftOut == 1) || (nGreenOut == 1) || (nYellowRightOut == 1))
			nError = LampOff();
		break;

    case SETLAMP_Y_ON:
		//if ((nYellowOut == 0) || (nPreLampStatus != nLampStatus))
		if ((nYellowRightOut == 0) || (nYellowLeftOut == 0) || (nPreLampStatus != nLampStatus))
            nError = LampOff();
			nError = LampOn(nLampStatus);
        break;
    case SETLAMP_G_ON:
        if ((nGreenOut == 0) || (nPreLampStatus != nLampStatus))
        {
            nError = LampOff();
            nError = LampOn(nLampStatus);
        }
        break;
    case SETLAMP_Y_FLICKER:
		//if ((nYellowOut == 0) && (nPreLampStatus == nLampStatus))
		if ((nYellowRightOut == 0) && (nYellowLeftOut == 0) && (nPreLampStatus == nLampStatus))
            nError = LampOn(nLampStatus);
        else
            nError = LampOff();
        break;
    case SETLAMP_G_FLICKER:
        if ((nGreenOut == 0) && (nPreLampStatus == nLampStatus))
            nError = LampOn(nLampStatus);
        else
            nError = LampOff();
        break;

    case SETLAMP_R_FLICKER:
        if ((nRedOut == 0) && (nPreLampStatus == nLampStatus))
            nError = LampOn(nLampStatus);
        else
            nError = LampOff();
        break;
    }

    nPreLampStatus = nLampStatus;

    return nError;
}
