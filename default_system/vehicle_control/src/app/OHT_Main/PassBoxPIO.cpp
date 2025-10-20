//---------------------------------------------------------------------------

#pragma hdrstop

#include "PassBoxPIO.h"
#include "Utility.h"

//---------------------------------------------------------------------------

/**
@brief   PassBoxPIO
@author  KKS
@date    2015.01.27
@param   pHWCommon : HWCommon ClassAC ¨¡¡ÀAIAI
@note    
*/
PassBoxPIO::PassBoxPIO(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   PassBoxPIO ¨ùO¢¬eAU
@author  KKS
@date    2015.01.27
@param   
@note    
*/
PassBoxPIO::~PassBoxPIO()
{
}

/**
@brief   PIOEnable
@author  KKS
@date    2015.01.27
@param   
@note    
*/
int PassBoxPIO::PIOEnable()
{
    int nError = NO_ERR;

	// Left Side
    if(nError == NO_ERR)
   	{
        nError = m_pHWCommon->SetBit(OUT_EQPIO_LEFT_SEL, OFF);
   	}		

    return nError;
}

/**
@brief   AllPIOOff
@author  KKS
@date    2015.01.27
@param   
@note    
*/
int PassBoxPIO::AllPIOOff()
{
    int nError = NO_ERR;

    if(nError == NO_ERR)
   	{
        nError = m_pHWCommon->SetBit(OUT_EQPIO_VALID, OFF);
   	}		
    if(nError == NO_ERR)
   	{
        nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, OFF);
   	}		
    if(nError == NO_ERR)
   	{
        nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_1, OFF);
   	}		
    if(nError == NO_ERR)
   	{
        nError = m_pHWCommon->SetBit(OUT_EQPIO_TR_REQ, OFF);
   	}		
    if(nError == NO_ERR)
   	{
        nError = m_pHWCommon->SetBit(OUT_EQPIO_BUSY, OFF);
   	}		
    if(nError == NO_ERR)
   	{
        nError = m_pHWCommon->SetBit(OUT_EQPIO_COMPLETE, OFF);
   	}		

    return nError;
}

/**
@brief   PIODisable
@author  KKS
@date    2015.01.27
@param	 Aa¡¤A ¢¬©£¥ìI Off ¨öAA©øAo ¨ù¡¾AA(1 : A¡þ¢¬¢ç¨úi, 0 : ¡íoAA A?Ao)
@note    
*/
int PassBoxPIO::PIODisable(bool bOutClear)
{
    int nError = NO_ERR;

    if(bOutClear == true)
    {
    	// Left
		if(m_pHWCommon->m_Output.EQPIO_Left_Select == ON)
		{
			nError = m_pHWCommon->SetBit(OUT_EQPIO_LEFT_SEL, OFF);
		}			
		
		if(nError == NO_ERR)
		{
			nError = AllPIOOff();
		}			
    }
    else
    {
        if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Valid == ON))
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_VALID, OFF);
       	}
        if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_CS_0 == ON))
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, OFF);
       	}			
        if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_CS_1 == ON))
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_1, OFF);
       	}			

        if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_TR_Req == ON))
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_TR_REQ, OFF);
       	}			
        if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Busy == ON))
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_BUSY, OFF);
       	}			
        if((nError == NO_ERR) && (m_pHWCommon->m_Output.EQPIO_Complete == ON))
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_COMPLETE, OFF);
       	}			
		
        Sleep(50);
	}

	// Left
    if(nError == NO_ERR)
	{
        nError = m_pHWCommon->SetBit(OUT_EQPIO_LEFT_SEL, ON);
   	}		

    return nError;
}

/**
@brief   IsPBAVBLOn
@author  KKS
@date    2015.01.27
@param	 
@note    
*/
bool PassBoxPIO::IsPBAVBLOn()
{
	if(m_pHWCommon->m_Input.EQPIO_HO_AVBL == ON)
	{
        return true;
	}		
    else
   	{
        return false;
   	}		
}

/**
@brief   IsGoSignalOn
@author  KKS
@date    2015.01.27
@param	 
@note    
*/
bool PassBoxPIO::IsESOn()
{
    if(m_pHWCommon->m_Input.EQPIO_ES == ON)
   	{
        return true;
   	}		
    else
   	{
        return false;
   	}		
}

/**
@brief   IsGoSignalOn
@author  KKS
@date    2015.01.27
@param	 
@note    
*/
bool PassBoxPIO::IsGoSignalOn()
{
#if((OHT_NAME != OHT_NAME_STD_V85) && (OHT_NAME != OHT_NAME_STD_V81S)&& (OHT_NAME != OHT_NAME_STD_V85S)&&(OHT_NAME != OHT_NAME_STD_V82)&&(OHT_NAME != OHT_NAME_STD_V86)&&(OHT_NAME != OHT_NAME_P4_MASK))
	if((m_pHWCommon->m_Input.EQPIO_Left_Go == ON)||(m_pHWCommon->m_Input.EQPIO_Right_Go == ON))
	{
		return true;
	}		
	else
	{
		return false;
	}
#else
	if(m_pHWCommon->m_Input.EQPIO_Left_Go == ON)
	{
		return true;
	}
	else
	{
		return false;
	}
#endif
}

/**
@brief   IsCloseOn
@author  KKS
@date    2015.01.28
@param	 
@note    
*/
bool PassBoxPIO::IsCloseOn()
{
    if(m_pHWCommon->m_Input.EQPIO_Load_REQ == ON)
   	{
   		return true;
   	}
    else
   	{
		return false;
   	}
}

/**
@brief   IsOpenOn
@author  KKS
@date    2015.01.28
@param	 
@note    
*/
bool PassBoxPIO::IsOpenOn()
{
    if(m_pHWCommon->m_Input.EQPIO_Unload_REQ == ON)
   	{
        return true;
   	}		
    else
   	{
   		return false;
   	}		
}

/**
@brief   IsBusyOn
@author  KKS
@date    2015.01.28
@param	 
@note    
*/
bool PassBoxPIO::IsBusyOn()
{
    if(m_pHWCommon->m_Input.EQPIO_Ready == ON)
   	{
        return true;
   	}		
    else
   	{
        return false;
   	}		
}

/**
@brief   OHTStatusOnOff
@author  KKS
@date    2015.01.28
@param	 
@note    
*/
int PassBoxPIO::OHTStatusOnOff(bool bOnOff)
{
    int nError = NO_ERR;

    if(bOnOff == ON)
    {
        if(nError == NO_ERR)
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_VALID, ON);
       	}			
    }
    else
    {
        if(nError == NO_ERR)
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_VALID, OFF);
       	}			
    }

    return nError;
}

/**
@brief   DoorSelectOnOff
@author  KKS
@date    2015.01.28
@param	 
@note    
*/
int PassBoxPIO::DoorSelectOnOff(bool bOnOff)
{
    int nError = NO_ERR;

    if(bOnOff == ON)
    {
        if(nError == NO_ERR)
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, ON);
       	}			
    }
    else
    {
        if(nError == NO_ERR)
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_CS_0, OFF);
       	}			
    }

    return nError;
}

/**
@brief   OpenReqOnOff
@author  KKS
@date    2015.01.28
@param	 
@note    
*/
int PassBoxPIO::OpenReqOnOff(bool bOnOff)
{
    int nError = NO_ERR;

    if(bOnOff == ON)
    {
        if(nError == NO_ERR)
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_TR_REQ, ON);
       	}			
    }
    else
    {
        if(nError == NO_ERR)
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_TR_REQ, OFF);
       	}			
    }

    return nError;
}

/**
@brief   ReadyOnOff
@author  KKS
@date    2015.01.28
@param	 
@note    
*/
int PassBoxPIO::ReadyOnOff(bool bOnOff)
{
    int nError = NO_ERR;

    if(bOnOff == ON)
    {
        if(nError == NO_ERR)
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_BUSY, ON);
       	}			
    }
    else
    {
        if(nError == NO_ERR)
       	{
            nError = m_pHWCommon->SetBit(OUT_EQPIO_BUSY, OFF);
       	}			
    }

    return nError;
}

/**
@brief   Check_PB_AVBL_OffTime
@author  KKS
@date    2015.01.28
@param	 
@note    
*/
int PassBoxPIO::Check_PB_AVBL_OffTime(int nPeriodTime)
{
	int nError = RTN_OK;

	static bool bPB_AVBL_WaitTimeCheckFlag = false;	
	static DWORD m_PB_AVBL_AbnormalOffWaitTime = timeGetTime();
	
	if(IsPBAVBLOn() == false)
	{
		if(bPB_AVBL_WaitTimeCheckFlag == false)
		{
			m_PB_AVBL_AbnormalOffWaitTime = timeGetTime();
			bPB_AVBL_WaitTimeCheckFlag = true;
		}

		if(bPB_AVBL_WaitTimeCheckFlag == true && OHTUTILITY::PreventOverflowGetTimeMs(m_PB_AVBL_AbnormalOffWaitTime) > nPeriodTime)
		{
			bPB_AVBL_WaitTimeCheckFlag = false;
			return RTN_FAIL;			
		}
	}
	else
	{
		bPB_AVBL_WaitTimeCheckFlag = false;
	}

	return nError;
}

/**
@brief   Check_ESOffTime
@author  KKS
@date    2015.01.28
@param	 
@note    
*/
int PassBoxPIO::Check_ESOffTime(int nPeriodTime)
{
	int nError = RTN_OK;

	static bool bESWaitTimeCheckFlag = false;	
	static DWORD m_ESAbnormalOffWaitTime = timeGetTime();

	if(IsESOn() == false)
	{
		if(bESWaitTimeCheckFlag == false)
		{
			m_ESAbnormalOffWaitTime = timeGetTime();
			bESWaitTimeCheckFlag = true;
		}

		if(bESWaitTimeCheckFlag == true && OHTUTILITY::PreventOverflowGetTimeMs(m_ESAbnormalOffWaitTime) > nPeriodTime)
		{
			bESWaitTimeCheckFlag = false;
			return RTN_FAIL;			
		}
	}
	else
	{
		bESWaitTimeCheckFlag = false;
	}

	return nError;
}


/**
@brief   Check_GoOffTime
@author  KKS
@date    2015.01.28
@param	 
@note    
*/
int PassBoxPIO::Check_GoOffTime(int nPeriodTime)
{
	int nError = RTN_OK;

	static bool bGoWaitTimeCheckFlag = false;	
	static DWORD m_GoAbnormalOffWaitTime = timeGetTime();
	
	if(IsGoSignalOn() == false)
	{
		if(bGoWaitTimeCheckFlag == false)
		{
			m_GoAbnormalOffWaitTime = timeGetTime();
			bGoWaitTimeCheckFlag = true;
		}

		if(bGoWaitTimeCheckFlag == true && OHTUTILITY::PreventOverflowGetTimeMs(m_GoAbnormalOffWaitTime) > nPeriodTime)
		{
			bGoWaitTimeCheckFlag = false;
			return RTN_FAIL;			
		}
	}
	else
	{
		bGoWaitTimeCheckFlag = false;
	}

	return nError;
}

#pragma package(smart_init)

