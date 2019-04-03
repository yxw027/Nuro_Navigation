#ifndef Def_RouteBase
	#define Def_RouteBase
	#include "NuroDefine.h"

	#ifdef DEF_MAKETHRROUTE
		extern nuTCHAR			MakeThr_MapPath[NURO_MAX_PATH];
	#endif

	typedef struct tagPASSRTBDATA
	{
//		nuroPOINT	PtCoor;
		nuLONG		MRTNODEID;
		nuLONG		MapID;
		nuLONG		RTBID;
	}PASSRTBDATA,*PPASSRTBDATA,*LPPASSRTBDATA;

	typedef struct tagTHCROUTEDATA
	{
		nuLONG RealDis;
		nuLONG WeightDis;
		nuLONG PassRTBCount;
		PASSRTBDATA	stuPassRTBData[256];
	}THCROUTEDATA,*PTHCROUTEDATA,*LPTHCROUTEDATA;

	extern nuTCHAR				RootPath[NURO_MAX_PATH];
	extern nuBOOL				b_Drive;
	extern nuBOOL				b_StopRoute;//中斷規劃
	nuVOID GetRootPathStep2();

	typedef struct Tag_NodeListStru
	{
		nuULONG NodeID : 20;//要嘗試的NodeID(To)
		nuULONG RtBID : 12;
		nuULONG MapID : 16;
		nuULONG MapLayer:16;//起始NodeID
		nuULONG RunDis;//起始NodeID
	}NodeListStru,*pNodeListStru;


#endif