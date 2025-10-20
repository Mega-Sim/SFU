#include "HoistDistanceConverter.h"
#include <stdio.h>
#include <math.h>

//#include "LogHandler.h"		// Log를 사용하기 위한 참조

//---------------------------------------------------------------------------
//#define LOG_ERR_HID(...)  WRITE_LOG_ON_THIS_FILE("D:\\log\\HoistCont.log", __VA_ARGS__)

#define MAXROW 4
#define MAXCUL MAXROW
//---------------------------------------------------------------------------
double multiply(double dValue, int nTimes)
{
	double dTmp = 1;
	for(int i=0; i<nTimes; i++)dTmp *= dValue;
	return dTmp;
}
//---------------------------------------------------------------------------
HoistDistanceConverter::HoistDistanceConverter()
{
	m_dA  =	0.0063, m_dB  = -3.1771, m_dC  = 304.3292, m_dD  = -3;	 // AMC
	m_dA2 = 0.0063, m_dB2 = -3.1771, m_dC2 = 304.3292, m_dD2 = -3;	 // AMC2
#ifdef OHT_HIGH_RAIL_HOIST_PRAM
	m_dA3 = 0.0063, m_dB3 = -3.1771, m_dC3 = 304.3292, m_dD3 = -3;	// AMC3
	m_CenterPosition2 = 0.0;
	m_CenterPulse2 = 0.0;
#endif
    // m_dMaxUser = 5500 였으나 euv 는 7200 까지 사용 (단, 현 출하시까지는 5500 유지 후 EUV 사용시 반영 해야함)
	m_dMaxCts = 1130000, m_dMinCts = -45000, m_dMaxUser = 5500, m_dMinUser = -175;

	m_dMinCts_Limit =-2500, m_dMinUser_Limit = -9;
	//puting Center
	m_CenterPosition = 1470.0;
	m_CenterPulse = 500000.0;
}
//---------------------------------------------------------------------------
//HoistDistanceConverter::HoistDistanceConverter(double dA,double dB, double dC, double dD)
//{
//	 m_dA = dA;
//	 m_dB = dB;
//	 m_dC = dC;
//	 m_dD = dD;
//}
//---------------------------------------------------------------------------
HoistDistanceConverter::~HoistDistanceConverter(void)
{
}
//---------------------------------------------------------------------------
double HoistDistanceConverter::calcCts2User(double dCts)
{
	double	d100000Cts = 0.0, dMiliMeter = 0.0;


	if(dCts>m_dMaxCts) return m_dMaxUser;	 // 값을 찾지 못함 9:6개
	if(dCts<m_dMinCts_Limit) return m_dMinUser_Limit;	 // 값을 찾지 못함 9:6개

	d100000Cts = dCts/100000.0;
	if(m_CenterPulse > dCts)
	{
		dMiliMeter = m_dA*multiply(d100000Cts,3)
					+m_dB*multiply(d100000Cts,2)
					+m_dC*multiply(d100000Cts,1)
					+m_dD;
	}
#ifdef OHT_HIGH_RAIL_HOIST_PRAM
	else if((m_CenterPulse <= dCts) && (m_CenterPulse2 > dCts))
	{
		dMiliMeter = m_dA2*multiply(d100000Cts,3)
					+m_dB2*multiply(d100000Cts,2)
					+m_dC2*multiply(d100000Cts,1)
					+m_dD2;
	}
	else
	{
		dMiliMeter = m_dA3*multiply(d100000Cts,3)
					+m_dB3*multiply(d100000Cts,2)
					+m_dC3*multiply(d100000Cts,1)
				    +m_dD3;
	}
#else
    else
	{
		dMiliMeter = m_dA2*multiply(d100000Cts,3)
					+m_dB2*multiply(d100000Cts,2)
					+m_dC2*multiply(d100000Cts,1)
					+m_dD2;
	}
#endif
	return dMiliMeter;
}
//---------------------------------------------------------------------------
double HoistDistanceConverter::calcUser2Cts(double dDownDistance)
{
	//800000Cts에 2200mm 정도를 최대값으로 산정한다.
	double	dMaxCts = m_dMaxCts;
	double	dMinCts = m_dMinCts_Limit;
	double	dTmpCts = (dMaxCts+dMinCts)/2;
	double	dTmpDownDistance = 0.0;

//	LOG_ERR_HID("HoistParamCalcInit[%5.1lf][%5.1lf][%5.1lf]",dDownDistance, m_dMaxCts,m_dMinCts );

	// 값이 범위 밖이면 제거
	if(dDownDistance>m_dMaxUser) return m_dMaxCts;
	if(dDownDistance<m_dMinUser_Limit) return m_dMinCts_Limit;	// 값을 찾지 못함 9:6개

	for(int i=dMaxCts/4; i>0; i=i/2)
	{
		dTmpDownDistance = calcCts2User(dTmpCts);

//		LOG_ERR_HID("HoistParamCalc[%5.1lf]",dTmpDownDistance);

		if(dTmpDownDistance-dDownDistance>=0)
		{
			dMaxCts = dTmpCts;
		}
		else
		{
			dMinCts = dTmpCts;
		}
		dTmpCts = (dMaxCts+dMinCts)/2;
		if((dMaxCts-dMinCts)<0.1)	break;
		if(dTmpCts>m_dMaxCts)return m_dMaxCts;
		if(dTmpCts<m_dMinCts_Limit)return m_dMinCts_Limit;	//-999999;	 // 값을 찾지 못함 9:6개
	}
	return dTmpCts;
}

// 3 차원 첫번째 그래프 ///
//---------------------------------------------------------------------------
bool HoistDistanceConverter::calcCoefficient(double dP1, double dM1, double dP2, double dM2, double dP3, double dM3, double dP4, double dM4)
{

//	WRITE_LOG_ON_THIS_FILE("D:\\log\\INI.log","HoistParam[this:%X] %5.1lf:%5.1lf / %5.1lf:%5.1lf / %5.1lf:%5.1lf / %5.1lf:%5.1lf ",
//		this, dP1, dM1, dP2, dM2, dP3, dM3, dP4, dM4);	//2	미심적은 부분 이 있어 로그를 남겨둠


	int		i = 0,j = 0,k = 0;
	int		irow = 0;
	double	dtemp = 0.0,pivot = 0.0;
	double	a[MAXROW][2*MAXCUL] = {0.0, }; // Gauss-Jordan
	double	a_1[MAXROW][MAXCUL] = {0.0, }; // 역행렬
	double	b[MAXROW] = {0.0, };			 // A*B=Y, B=A_1*Y (A:100000Cts, B:계수, Y:mm)
	double	d100000P[MAXROW] = {0.0, }, dM[MAXROW] = {0.0, };	 // Input Value

	/*..Initialize..*/
	for(i=0;i<MAXROW;i++)
	{
		for(j=0;j<MAXCUL;j++)	a_1[i][j]=0.;
		for(j=0;j<2*MAXCUL;j++) a[i][j]=0.;
	}

	/*..Convert Input Value..*/
	d100000P[0] = dP1/100000.0;	 d100000P[1] = dP2/100000.0;   // 100000Cts
	d100000P[2] = dP3/100000.0;	 d100000P[3] = dP4/100000.0;   // 100000Cts
	dM[0] = dM1;		dM[1] = dM2;		dM[2] = dM3;		dM[3] = dM4;   // Mili Meter

	/*..Get Max/Min Range..*/
	m_dMaxUser	= dM[0],			m_dMinUser	 = dM[0];
	m_dMaxCts	= d100000P[0],	m_dMinCts	 = d100000P[0];
	for(i=1;i<4;i++) 
	{
		m_dMaxUser	= (dM[i]>m_dMaxUser)?  dM[i]:m_dMaxUser;
		m_dMinUser	= (dM[i]<m_dMinUser)?  dM[i]:m_dMinUser;
		m_dMaxCts	= (d100000P[i]>m_dMaxCts)? d100000P[i]:m_dMaxCts;
		m_dMinCts	= (d100000P[i]<m_dMinCts)? d100000P[i]:m_dMinCts;
	}
//	m_dMaxCts	*= 100000.0;
//	m_dMinCts	*= 100000.0;

   //---두 그래프가 만나는 시점---//
   m_CenterPosition = dM4;
   m_CenterPulse = dP4;

   /*..Input Matrix Component..*/
	for(i=0;i<MAXROW;i++)
	{
		for(j=0;j<MAXCUL;j++)
		{
			a[i][j] = multiply(d100000P[i], MAXCUL-1-j); // [ x3 x2 x1 1 ]
			if(i==j)	a[i][j+MAXCUL]=1.;
			else		a[i][j+MAXCUL]=0.;
		}
	}

	/*..Check det(A)..*/

	/*..pivot..*/
	for(i=0;i<MAXROW-1;i++)
	{
		irow=i;
		for(k=i+1;k<MAXROW;k++)
		{
			if(fabs(a[irow][i])<fabs(a[k][i])) irow=k;
		}
		for(j=0;j<2*MAXCUL;j++)
		{
			dtemp=a[i][j];
			a[i][j]=a[irow][j];
			a[irow][j]=dtemp;
		}
	}

	/*..Gauss-Jordan..*/	// 역행렬 계산
	for(i=0;i<MAXROW;i++){
		pivot=a[i][i];
		for(j=0;j<2*MAXROW;j++) a[i][j]/=pivot;
		for(k=0;k<MAXROW;k++){
			if(k==i) continue;
			dtemp=a[k][i];
			for(j=i;j<2*MAXCUL;j++){
				 a[k][j]-=dtemp*a[i][j];
			}
		}
	}

	/*..Inverse Matrix..*/
	for(i=0;i<MAXROW;i++)
	{
		for(j=0;j<MAXCUL;j++) a_1[i][j]=a[i][j+MAXCUL];
	}

	/*..Calulate Coefficient..*/
	for(i=0;i<MAXROW;i++)
	{
		b[i] = 0.0;
		for(j=0;j<MAXCUL;j++) b[i] += a_1[i][j] * dM[j];	// B = A_1 * Y
	}
	m_dA = b[0];   m_dB = b[1];	 m_dC = b[2];  m_dD = 0.0;//	m_dD = b[3];
	return true;
}
// 3 차원 두번째 그래프 ///
//추가된 중복 함수.(3차원 그래프 2개
bool HoistDistanceConverter::calcCoefficient2(double dP1, double dM1, double dP2, double dM2, double dP3, double dM3, double dP4, double dM4)
{

//	WRITE_LOG_ON_THIS_FILE("D:\\log\\INI.log","HoistParam[this:%X] %5.1lf:%5.1lf / %5.1lf:%5.1lf / %5.1lf:%5.1lf / %5.1lf:%5.1lf ",
//		this, dP1, dM1, dP2, dM2, dP3, dM3, dP4, dM4);	//2	미심적은 부분 이 있어 로그를 남겨둠


	int		i = 0,j = 0,k = 0;
	int		irow = 0;
	double	dtemp = 0.0,pivot = 0.0;
	double	a[MAXROW][2*MAXCUL] = {0.0, }; // Gauss-Jordan
	double	a_1[MAXROW][MAXCUL] = {0.0, }; // 역행렬
	double	b[MAXROW] = {0.0, };			 // A*B=Y, B=A_1*Y (A:100000Cts, B:계수, Y:mm)
	double	d100000P[MAXROW] = {0.0, }, dM[MAXROW] = {0.0, };	 // Input Value

	/*..Initialize..*/
	for(i=0;i<MAXROW;i++)
	{
		for(j=0;j<MAXCUL;j++)	a_1[i][j]=0.;
		for(j=0;j<2*MAXCUL;j++) a[i][j]=0.;
	}

	/*..Convert Input Value..*/
	d100000P[0] = dP1/100000.0;	 d100000P[1] = dP2/100000.0;   // 100000Cts
	d100000P[2] = dP3/100000.0;	 d100000P[3] = dP4/100000.0;   // 100000Cts
	dM[0] = dM1;		dM[1] = dM2;		dM[2] = dM3;		dM[3] = dM4;   // Mili Meter

	/*..Get Max/Min Range..*/
	for(i=1;i<4;i++)
	{
		m_dMaxUser	= (dM[i]>m_dMaxUser)?  dM[i]:m_dMaxUser;
		m_dMinUser	= (dM[i]<m_dMinUser)?  dM[i]:m_dMinUser;
		m_dMaxCts	= (d100000P[i]>m_dMaxCts)? d100000P[i]:m_dMaxCts;
		m_dMinCts	= (d100000P[i]<m_dMinCts)? d100000P[i]:m_dMinCts;
	}
#ifdef OHT_HIGH_RAIL_HOIST_PRAM
 	m_CenterPosition2 = dM4;
	m_CenterPulse2 = dP4;
#else
	m_dMaxCts	*= 100000.0;
	m_dMinCts	*= 100000.0;

#endif


   //---두 그래프가 만나는 시점---//
//   m_CenterPosition = dP1;
//   m_CenterPulse = dM1;

   /*..Input Matrix Component..*/
	for(i=0;i<MAXROW;i++)
	{
		for(j=0;j<MAXCUL;j++)
		{
			a[i][j] = multiply(d100000P[i], MAXCUL-1-j); // [ x3 x2 x1 1 ]
			if(i==j)	a[i][j+MAXCUL]=1.;
			else		a[i][j+MAXCUL]=0.;
		}
	}

	/*..Check det(A)..*/

	/*..pivot..*/
	for(i=0;i<MAXROW-1;i++)
	{
		irow=i;
		for(k=i+1;k<MAXROW;k++)
		{
			if(fabs(a[irow][i])<fabs(a[k][i])) irow=k;
		}
		for(j=0;j<2*MAXCUL;j++)
		{
			dtemp=a[i][j];
			a[i][j]=a[irow][j];
			a[irow][j]=dtemp;
		}
	}

	/*..Gauss-Jordan..*/	// 역행렬 계산
	for(i=0;i<MAXROW;i++){
		pivot=a[i][i];
		for(j=0;j<2*MAXROW;j++) a[i][j]/=pivot;
		for(k=0;k<MAXROW;k++){
			if(k==i) continue;
			dtemp=a[k][i];
			for(j=i;j<2*MAXCUL;j++){
				 a[k][j]-=dtemp*a[i][j];
			}
		}
	}

	/*..Inverse Matrix..*/
	for(i=0;i<MAXROW;i++)
	{
		for(j=0;j<MAXCUL;j++) a_1[i][j]=a[i][j+MAXCUL];
	}

	/*..Calulate Coefficient..*/
	for(i=0;i<MAXROW;i++)
	{
		b[i] = 0.0;
		for(j=0;j<MAXCUL;j++) b[i] += a_1[i][j] * dM[j];	// B = A_1 * Y
	}
	m_dA2 = b[0];   m_dB2 = b[1];	 m_dC2 = b[2];	m_dD2 = b[3];
	return true;
}

#ifdef OHT_HIGH_RAIL_HOIST_PRAM

bool HoistDistanceConverter::calcCoefficient3(double dP1, double dM1, double dP2, double dM2, double dP3, double dM3, double dP4, double dM4)
{

//	WRITE_LOG_ON_THIS_FILE("D:\\log\\INI.log","HoistParam[this:%X] %5.1lf:%5.1lf / %5.1lf:%5.1lf / %5.1lf:%5.1lf / %5.1lf:%5.1lf ",
//		this, dP1, dM1, dP2, dM2, dP3, dM3, dP4, dM4);	//2	미심적은 부분 이 있어 로그를 남겨둠


	int		i = 0,j = 0,k = 0;
	int		irow = 0;
	double	dtemp = 0.0,pivot = 0.0;
	double	a[MAXROW][2*MAXCUL] = {0.0, }; // Gauss-Jordan
	double	a_1[MAXROW][MAXCUL] = {0.0, }; // 역행렬
	double	b[MAXROW] = {0.0, };			 // A*B=Y, B=A_1*Y (A:100000Cts, B:계수, Y:mm)
	double	d100000P[MAXROW] = {0.0, }, dM[MAXROW] = {0.0, };	 // Input Value

	/*..Initialize..*/
	for(i=0;i<MAXROW;i++)
	{
		for(j=0;j<MAXCUL;j++)	a_1[i][j]=0.;
		for(j=0;j<2*MAXCUL;j++) a[i][j]=0.;
	}

	/*..Convert Input Value..*/
	d100000P[0] = dP1/100000.0;	 d100000P[1] = dP2/100000.0;   // 100000Cts
	d100000P[2] = dP3/100000.0;	 d100000P[3] = dP4/100000.0;   // 100000Cts
	dM[0] = dM1;		dM[1] = dM2;		dM[2] = dM3;		dM[3] = dM4;   // Mili Meter

	/*..Get Max/Min Range..*/
	for(i=1;i<4;i++)
	{
		m_dMaxUser	= (dM[i]>m_dMaxUser)?  dM[i]:m_dMaxUser;
		m_dMinUser	= (dM[i]<m_dMinUser)?  dM[i]:m_dMinUser;
		m_dMaxCts	= (d100000P[i]>m_dMaxCts)? d100000P[i]:m_dMaxCts;
		m_dMinCts	= (d100000P[i]<m_dMinCts)? d100000P[i]:m_dMinCts;
	}
	m_dMaxCts	*= 100000.0;
	m_dMinCts	*= 100000.0;


   /*..Input Matrix Component..*/
	for(i=0;i<MAXROW;i++)
	{
		for(j=0;j<MAXCUL;j++)
		{
			a[i][j] = multiply(d100000P[i], MAXCUL-1-j); // [ x3 x2 x1 1 ]
			if(i==j)	a[i][j+MAXCUL]=1.;
			else		a[i][j+MAXCUL]=0.;
		}
	}

	/*..Check det(A)..*/

	/*..pivot..*/
	for(i=0;i<MAXROW-1;i++)
	{
		irow=i;
		for(k=i+1;k<MAXROW;k++)
		{
			if(fabs(a[irow][i])<fabs(a[k][i])) irow=k;
		}
		for(j=0;j<2*MAXCUL;j++)
		{
			dtemp=a[i][j];
			a[i][j]=a[irow][j];
			a[irow][j]=dtemp;
		}
	}

	/*..Gauss-Jordan..*/	// 역행렬 계산
	for(i=0;i<MAXROW;i++){
		pivot=a[i][i];
		for(j=0;j<2*MAXROW;j++) a[i][j]/=pivot;
		for(k=0;k<MAXROW;k++){
			if(k==i) continue;
			dtemp=a[k][i];
			for(j=i;j<2*MAXCUL;j++){
				 a[k][j]-=dtemp*a[i][j];
			}
		}
	}

	/*..Inverse Matrix..*/
	for(i=0;i<MAXROW;i++)
	{
		for(j=0;j<MAXCUL;j++) a_1[i][j]=a[i][j+MAXCUL];
	}

	/*..Calulate Coefficient..*/
	for(i=0;i<MAXROW;i++)
	{
		b[i] = 0;
		for(j=0;j<MAXCUL;j++) b[i] += a_1[i][j] * dM[j];	// B = A_1 * Y
	}
	m_dA3 = b[0];   m_dB3 = b[1];	 m_dC3 = b[2];	m_dD3 = b[3];
	return true;
}
#endif

//---------------------------------------------------------------------------
bool HoistDistanceConverter::isSameDirection(void)
{
	double dTmp = (calcUser2Cts(200.0) - calcUser2Cts(100.0))/(200-100);
//	LOG_ERR_HID("isSameDirection[%d]",dTmp );
	if(dTmp>0) return true;
	else return false;
}//---------------------------------------------------------------------------
//void HoistDistanceConverter::getCoefficient(double &dA,double &dB, double &dC, double &dD)
//{
//	 dA = m_dA;
//	 dB = m_dB;
//	 dC = m_dC;
//	 dD = m_dD;
//}
//---------------------------------------------------------------------------
double	 HoistDistanceConverter::calcSpeedCts2User	(double nCtsSpeed, double dCtsPosition)
{
	double dSlope = (m_dMaxUser - m_dMinUser)/(m_dMaxCts - m_dMinCts);
	return (dSlope * nCtsSpeed);

//	 double   d100000CtsPosition, dSlope, dSpeed;
//
//	 d100000CtsPosition = dCtsPosition/100000.0;
//	 dSlope =  (3*m_dA*multiply(d100000CtsPosition,2))
//			+(2*m_dB*multiply(d100000CtsPosition,1))
//			+	m_dC;
//	 dSpeed = dSlope * (nCtsSpeed / 100000.0);
//	 return dSpeed;
}
//---------------------------------------------------------------------------
double HoistDistanceConverter::calcSpeedUser2Cts  (double dDownSpeed, double dDownPosition)
{
	double	dSlope = (m_dMaxCts - m_dMinCts)/(m_dMaxUser - m_dMinUser);
	return (dSlope * dDownSpeed);

   //	double	 d100000CtsPosition, dSlope;
//	 int nSpeed;
//
//	 d100000CtsPosition = calcUser2Cts(dDownPosition)/100000.0;
//	 dSlope =  (3*m_dA*multiply(d100000CtsPosition,2))
//			+(2*m_dB*multiply(d100000CtsPosition,1))
//			+	m_dC;
//	 nSpeed = (dDownSpeed * 100000.0) / dSlope;
//	 return nSpeed;
}
//---------------------------------------------------------------------------
double	 HoistDistanceConverter::calcAccelCts2User	(double dCtsAccel, double dCtsPosition)
{
	double	dSlope = (m_dMaxUser - m_dMinUser)/(m_dMaxCts - m_dMinCts);
	return (dSlope * dCtsAccel);

//	 double   d100000CtsPosition, dSlope, dAccel;
//
//	 d100000CtsPosition = dCtsPosition/100000.0;
//	 dSlope =  (6*m_dA*multiply(d100000CtsPosition,1))
//			+(2*m_dB);
//	 dAccel = dSlope * (dCtsAccel / 100000.0);
//	 return dAccel;
}
//---------------------------------------------------------------------------
double		HoistDistanceConverter::calcAccelUser2Cts  (double dDownAccel, double dDownPosition)
{
	double	dSlope = (m_dMaxCts - m_dMinCts)/(m_dMaxUser - m_dMinUser);
	return (dSlope * dDownAccel);

//	 double   d100000CtsPosition, dSlope;
//	 int nAccel;
//
//	 d100000CtsPosition = calcUser2Cts(dDownPosition)/100000.0;
//	 dSlope =  (6*m_dA*multiply(d100000CtsPosition,1))
//			+(2*m_dB);
//	 nAccel = (dDownAccel * 100000.0) / dSlope;
//	 return nAccel;
}

double HoistDistanceConverter::GetHoistMaxUserPosition()
{
    return m_dMaxUser;
}
double HoistDistanceConverter::GetHoistMinUserPosition()
{
    return m_dMinUser_Limit;
}

