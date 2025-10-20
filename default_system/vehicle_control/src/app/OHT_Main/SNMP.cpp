//---------------------------------------------------------------------------


#pragma hdrstop

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <winsock2.h>
#include "SNMP.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
/* SNMP 패킷 생성 */
int SNMP::MakeSnmp(unsigned char *packet, char *mibstring, int *snmplength)
{
	// 변수 선언
	unsigned char mib[MAX_PACKET] = { 0, };
	char community[] = "public";
	int snmplen, comlen, orgmiblen, cnt, temp, i, pos, index, datasize, mibindex, miblen, mibSize;
	int *pmibvals;
	bool bCheckOID;

	// 변수 초기화
	cnt = 0;
	pos = 0;
	orgmiblen = 0;
	comlen = strlen(community);
	index = -1;
	mibindex = 0;
	miblen = 1;
	bCheckOID = true;
	mibSize = strlen(mibstring);

    // MIB Data에서 특수 문자(.) 기준으로 MIB 값 분리
	for (i = 0; i<mibSize; i++)
	{
		if (mibstring[i] == '.')
		{
			bCheckOID = false;
			miblen++;
		}
	}
	if (bCheckOID)
	{
		return -1;
	}
	else
	{
		orgmiblen = miblen;
		pmibvals = (int *)malloc(sizeof(int) * orgmiblen);

		for (i = 0; i<mibSize; i++)
		{
			if (mibstring[i] == '.')
			{
				datasize = i - index;
                // MIB 데이터는 항목은 최대 3자리까지 가능
				if (datasize > 1 && datasize < 5)
				{
					if (datasize == 2)
					{
						pmibvals[mibindex] = mibstring[i - 1] - '0';
						mibindex++;
					}
					else if (datasize == 3)
					{
						pmibvals[mibindex] = (mibstring[i - 2] - '0') * 10 + mibstring[i - 1] - '0';
						mibindex++;
					}
					else if (datasize == 4)
					{
						pmibvals[mibindex] = (mibstring[i - 3] - '0') * 100 + (mibstring[i - 2] - '0') * 10 + mibstring[i - 1] - '0';
						mibindex++;
					}
					index = i;
				}
				else
				{
					bCheckOID = true;
				}
			}
			else if (i == mibSize - 1)
			{
				datasize = i - index;
				if (datasize > 0 && datasize < 4)
				{
					if (datasize == 1)
					{
						pmibvals[mibindex] = mibstring[i] - '0';
						mibindex++;
					}
					else if (datasize == 2)
					{
						pmibvals[mibindex] = (mibstring[i - 1] - '0') * 10 + mibstring[i] - '0';
						mibindex++;
					}
					else if (datasize == 3)
					{
						pmibvals[mibindex] = (mibstring[i - 2] - '0') * 100 + (mibstring[i - 1] - '0') * 10 + mibstring[i] - '0';
						mibindex++;
					}
					index = i;
				}
				else
				{
					bCheckOID = true;
				}
			}
			if (bCheckOID)
            {
                free(pmibvals);
                return -1;
            }
		}

        // MIB 데이터 1Byte로 분리
		for (i = 0; i<orgmiblen; i++)
		{
			temp = pmibvals[i];

			if (temp >127)
			{
				mib[cnt] = (char)(128 + (temp / 128));
				mib[cnt + 1] = (char)(temp - ((temp / 128) * 128));
				cnt += 2;
				miblen++;
			}
			else
			{
				mib[cnt] = (char)temp;
				cnt++;
			}
		}
		snmplen = 34 + comlen + miblen;		            // Length of entire SNMP packet
		*snmplength = snmplen;

		//The SNMP sequence start
		packet[pos++] = 0x30;							//Sequence start
        packet[pos++] = 0x82;
        packet[pos++] = 0x00;
		packet[pos++] = (char)(snmplen - 4);		    //sequence size

        //SNMP version
		packet[pos++] = 0x02; 							//Integer type
		packet[pos++] = 0x01; 							//length
		packet[pos++] = 0x00; 							//SNMP version 1

        //Community name
		packet[pos++] = 0x04; 							// String type
		packet[pos++] = (char)comlen;				    //length

        //Convert community name to byte array
		for (i = 0; i< comlen; i++)
		{
			packet[pos++] = community[i];
		}

		//GetRequest Mode
		packet[pos++] = 0xA0;
        packet[pos++] = 0x82;
        packet[pos++] = 0x00;
		packet[pos++] = (char)(miblen + 21);	        //Size of total MIB

        //Request ID
		packet[pos++] = 0x02; 							//Integer type
		packet[pos++] = 0x02; 							//length
		packet[pos++] = 0x23; 							//SNMP request ID
		packet[pos++] = 0x8C;

		//Error status
		packet[pos++] = 0x02; 							//Integer type
		packet[pos++] = 0x01; 							//length
		packet[pos++] = 0x00; 							//SNMP error status

        //Error index
		packet[pos++] = 0x02; 							//Integer type
		packet[pos++] = 0x01; 							//length
		packet[pos++] = 0x00; 							//SNMP error index

        //Start of variable bindings
		packet[pos++] = 0x30; 							//Start of variable bindings sequence
        packet[pos++] = 0x82;
        packet[pos++] = 0x00;
		packet[pos++] = (char)(miblen + 7);		        // Size of variable binding
		packet[pos++] = 0x30; 							//Start of first variable bindings sequence
        packet[pos++] = 0x82;
        packet[pos++] = 0x00;
		packet[pos++] = (char)(miblen + 3);             // size
		packet[pos++] = 0x06; 							//Object type
		packet[pos++] = (char)(miblen - 1);			    //length

        //Start of MIB
		packet[pos++] = 0x2b;
		//Place MIB array in packet
		for (i = 2; i < miblen; i++)
		{
			packet[pos++] = (char)(mib[i]);
		}
		packet[pos++] = 0x05;							//Null object value
		packet[pos++] = 0x00;							//Null
		free(pmibvals);
		return 0;
	}
}

/* SNMP 수신 데이터 분석 (Type = INT형)*/
int SNMP::IntParse(unsigned char *response)
{
	int result = 0;
	int commlength, miblength, datatype, datalength, datastart;

	if (response[0] == 0xff)
	{
		return -1;
	}

	// If response, get the community name and MIB lengths
	commlength = (int)(response[6]);
	miblength = (int)(response[24 + commlength]);

	// Extract the MIB data from the SNMP response
	datatype = (int)(response[23 + commlength + miblength]);
	datalength = (int)(response[26 + commlength + miblength]);
	datastart = 27 + commlength + miblength;

	while (datalength > 0)
	{
		result = (result << 8) + response[datastart++];
		datalength--;
	}
	return result;
}

/* SNMP 수신 데이터 분석 (Type = CHAR형)*/
int SNMP::CharParse(unsigned char *response, unsigned char *result)
{
	int commlength, miblength, datatype, datalength, datastart;

	if (response[0] == 0xff)
	{
		return -1;
	}

	// If response, get the community name and MIB lengths
	commlength = (int)(response[6]);
	miblength = (int)(response[24 + commlength]);

	// Extract the MIB data from the SNMP response
	datatype = (int)(response[23 + commlength + miblength]);
	datalength = (int)(response[26 + commlength + miblength]);
	datastart = 27 + commlength + miblength;

	for (int i = 0; i<datalength; i++)
	{
		result[i] = response[datastart + i];
	}
	return datalength;
}

/* 수신 MIB Data 분석*/
int SNMP::MibParsing(unsigned char *mibpacket, char *mibstring, int *miblength)
{
	// 변수 선언
	char community[] = "public";
	int comlen, orgmiblen, cnt, temp, i, pos, index, datasize, mibindex, miblen, mibSize;
	int *pmibvals;
	bool bCheckOID;

	// 변수 초기화
	cnt = 0;
	pos = 0;
	orgmiblen = 0;
	comlen = strlen(community);
	index = -1;
	mibindex = 0;
	miblen = 1;
	bCheckOID = true;
	mibSize = strlen(mibstring);

    // MIB Data에서 특수 문자(.) 기준으로 MIB 값 분리
	for (i = 0; i<mibSize; i++)
	{
		if (mibstring[i] == '.')
		{
			bCheckOID = false;
			miblen++;
		}
	}
	if (bCheckOID)
	{
		return -1;
	}
	else
	{
		orgmiblen = miblen;
		pmibvals = (int *)malloc(sizeof(int) * orgmiblen);

		for (i = 0; i<mibSize; i++)
		{
			if (mibstring[i] == '.')
			{
				datasize = i - index;
                // MIB 데이터는 항목은 최대 3자리까지 가능
				if (datasize > 1 && datasize < 5)
				{
					if (datasize == 2)
					{
						pmibvals[mibindex] = mibstring[i - 1] - '0';
						mibindex++;
					}
					else if (datasize == 3)
					{
						pmibvals[mibindex] = (mibstring[i - 2] - '0') * 10 + mibstring[i - 1] - '0';
						mibindex++;
					}
					else if (datasize == 4)
					{
						pmibvals[mibindex] = (mibstring[i - 3] - '0') * 100 + (mibstring[i - 2] - '0') * 10 + mibstring[i - 1] - '0';
						mibindex++;
					}
					index = i;
				}
				else
				{
					bCheckOID = true;
				}
			}
			else if (i == mibSize - 1)
			{
				datasize = i - index;
				if (datasize > 0 && datasize < 4)
				{
					if (datasize == 1)
					{
						pmibvals[mibindex] = mibstring[i] - '0';
						mibindex++;
					}
					else if (datasize == 2)
					{
						pmibvals[mibindex] = (mibstring[i - 1] - '0') * 10 + mibstring[i] - '0';
						mibindex++;
					}
					else if (datasize == 3)
					{
						pmibvals[mibindex] = (mibstring[i - 2] - '0') * 100 + (mibstring[i - 1] - '0') * 10 + mibstring[i] - '0';
						mibindex++;
					}
					index = i;
				}
				else
				{
					bCheckOID = true;
				}
			}
			if (bCheckOID)
            {
                free(pmibvals);
                return -1;
            }
		}

		for (i = 0; i<orgmiblen; i++)
		{
			temp = pmibvals[i];

			if (temp >127)
			{
				mibpacket[cnt] = (char)(128 + (temp / 128));
				mibpacket[cnt + 1] = (char)(temp - ((temp / 128) * 128));
				cnt += 2;
				miblen++;
			}
			else
			{
				mibpacket[cnt] = (char)temp;
				cnt++;
			}
		}
		*miblength = miblen;
		free(pmibvals);
	}
	return 0;
}