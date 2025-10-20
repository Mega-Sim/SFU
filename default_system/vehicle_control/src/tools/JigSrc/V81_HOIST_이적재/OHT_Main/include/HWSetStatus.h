/**
@file    HWSetStatus.h
@version OHT 7.0
@brief   공유 구조체 선언 Header File
*/

#ifndef HWSetStatusH
#define HWSetStatusH

/**
@brief   HW Thread와 Status Thread가 사용할 공유 구조체
@author  zzang9un
@date    2012.10.18
*/
typedef struct HWSetStatus_
{
    int ErrorCode; ///< Error Code
    //OHTCMD Cmd_Remocon; ///< Remocon에서 받은 Command
} HWSetStatus;

#endif  //HWSetStatusH
