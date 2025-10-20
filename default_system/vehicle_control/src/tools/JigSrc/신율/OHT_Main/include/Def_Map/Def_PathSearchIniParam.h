#ifndef Def_PathSearchIniParamH
#define Def_PathSearchIniParamH

#include <stdio.h>
#include "MinMaxCtrl.h"	//MIN_MAX_VALUE


/**
@brief	Path Search 및 통과허가 관련 파라미터 
@date    2016.11.26
@author  박태욱
*/
struct PATHSEARCH_PARAM_SET
{
	int		nMapSearchMax;
	
	int		nPassPermitPeriod;      ///< 통과허가 요청 주기 
	int		nPassPermitCount;		///< 통과허가 요청 거리 마진
	double	dPassPermitOpenMargin;	///< 통과허가 개방 거리 마진
	int     nPassPermitRollbackTime;
	int     nPassPermitRollbackFinshTime;
	double	dPassPermitMaxMargin;	///< 통과허가 개방 거리 마진
	double	dPassPermitRollbackMargin;	///< 통과허가 개방 거리 마진
	int     nIsCurveSpeedUp;    //재하가 있을 경우, 곡선부 속도 0.7m/s로 적용 유무

	int 	nIsShoutCurveSpeedUpValue;
	int 	nIsCurveSpeedUpValue;

	char 	Version[8];
};

/**
@brief	 UBG Check 위한 노드 파라미터 받아오기
@author  soming
@date	 2018.10.15
*/
#define MAX_JIKJIN_NODE_CHECK_CNT	2000

struct PATHSEARCH_UBG_CHECK_PARAM_SET
{
	int  ArrNode[MAX_JIKJIN_NODE_CHECK_CNT];
	int  nNodeCnt;

	PATHSEARCH_UBG_CHECK_PARAM_SET()
	{	ZeroMemory(this, sizeof(*this));
	}


	bool ExistsCheck(int nNode)
	{

		for(int i=0 ; i<nNodeCnt ; i++)
		{
			if(ArrNode[i] == nNode)
				return true;
		}
		return false;
	}


};

struct PATHSEARCH_NODE_LANCUT_PARAM_SET
{
	int  ArrNode[MAX_JIKJIN_NODE_CHECK_CNT];
	int  nNodeCnt;

	PATHSEARCH_NODE_LANCUT_PARAM_SET()
	{	ZeroMemory(this, sizeof(*this));
	}


	bool ExistsCheck(int nNode)
	{

		for(int i=0 ; i<nNodeCnt ; i++)
		{
			if(ArrNode[i] == nNode)
				return true;
		}
		return false;
	}


};


#endif
