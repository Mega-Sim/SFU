//---------------------------------------------------------------------------

#ifndef SNMPH
#define SNMPH
//---------------------------------------------------------------------------
#define MAX_PACKET 1024
#define MIB_CNT 3
#define MAX_MAC 18
#define MAC_SSID 50

enum MIB_TYPE
{
	MIB_NONE,
	MIB_RSSI,
	MIB_MAC,
	MIB_SSID
};

class SNMP
{
	// 멤버 변수
private:
protected:
public:

	// 멤버 함수
private:
protected:
public:
	SNMP()
	{

	}
	~SNMP()
	{

	}
    /* SNMP 패킷 생성 */
	int MakeSnmp(unsigned char *packet, char *mibstring, int *snmplength);

	/* SNMP 수신 데이터 분석 (Type = INT형)*/
	int IntParse(unsigned char *response);

	/* SNMP 수신 데이터 분석 (Type = CHAR형)*/
	int CharParse(unsigned char *response, unsigned char *result);

	/* 수신 MIB Data 분석 (Type = CHAR형)*/
	int MibParsing(unsigned char *mibpacket, char *mibstring, int *miblength);
};
#endif
