//---------------------------------------------------------------------------
/**
@file    OHTSysInfo.h
$Rev:: 467           $:  Revision of last commit
$Author:: puting   $:  Author of last commit
$Date:: 2013-03-27 1#$:  Date of last commit
@version OHT
@brief   OHTSysInfo Class Header File
*/

#ifndef OHTSysInfoH
#define OHTSysInfoH

//---------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <dos.h>
#include <vcl.h>

#include "ErrorCode.h"
#include "Def_HW.h"
#include "../Common/Def_DataType.h"



/***********************************************************************************/
// Function Name : Get_IPCSysInfo
// Release Date: 2013.03.28, ╧зеб©М
/***********************************************************************************/

int Get_IPCSysInfo(IPC_SYS_INFO *ipcsysinfo);

#endif
