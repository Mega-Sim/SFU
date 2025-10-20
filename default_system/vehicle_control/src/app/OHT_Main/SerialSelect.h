/**
@file    SerialSelect.h
@version OHT 7.0
@brief   SerialSelect Class Header File
*/

#ifndef SerialSelectH
#define SerialSelectH


#include "HWCommon.h"

/**
@class   SerialSelect
@date    2012.10.22
@author
@brief   SerialSelect ฐüทร Class
*/
class SerialSelect
{

private:

	HWCommon *m_pHWCommon;
	//Output OUT_SERIAL_SELECT_1;
	//Output OUT_SERIAL_SELECT_2;

public:
	SerialSelect(HWCommon *pHWCommon);
	~SerialSelect();


	int Init();
	SetSerial(int nDevice);

};


#endif //SerialSelectH
