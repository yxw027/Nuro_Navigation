// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/21 08:03:44 $
// $Revision: 1.3 $
#ifndef Def_ClassThrCtrl
	#define Def_ClassThrCtrl

	#include	"ConstSet.h"
	#include	"Stru_StateStru.h"
	#include	"NuroDefine.h"

	typedef struct Tag_F_Thr_Header_Stru
	{
		nuULONG TotalNodeCount;//總Node數
		nuULONG RtConnectStruCount;//總連結數
		nuLONG RtConnectStruStartAddr;//連結結構起始位置
	}F_Thr_Header_Stru,*pF_Thr_Header_Stru;

	typedef struct Tag_F_Thr_MainData_Stru
	{
		nuULONG FNodeID : 20;//起始NodeID
		nuULONG FRtBID : 12;
		nuULONG ConnectStruIndex : 22;//連結資料索引值
		nuULONG ConnectCount : 10;//連結總數
		NUROPOINT NodeCoor;
	}F_Thr_MainData_Stru,*pF_Thr_MainData_Stru;

	typedef struct Tag_F_Thr_Connect_Stru
	{
		nuULONG TNodeID : 20;//起始NodeID
		nuULONG TRtBID : 12;
		nuULONG TMapLayer : 2;
		nuULONG Class : 2;
		nuULONG Length : 28;//道路長度
		nuULONG WeightingLength;//加權長度
	}F_Thr_Connect_Stru,*pF_Thr_Connect_Stru;

	typedef struct Tag_Stru_ThrEachMap
	{
		F_Thr_Header_Stru ThrHeader;//Rt的表頭
		F_Thr_MainData_Stru **ThrMain;//Rt中的主資料
		F_Thr_Connect_Stru **ThrConnect;//Rt中的連結資料
	}Stru_ThrEachMap,*pStru_ThrEachMap;

	typedef struct Tag_Stru_RTBConnectHeader
	{
		nuUSHORT	RTBID;
		nuUSHORT	RTBConnCount;
	}Stru_RTBConnectHeader,*pStru_RTBConnectHeader;

	typedef struct Tag_Stru_RTBConnect
	{
		Stru_RTBConnectHeader Connheader;
		nuULONG	**ConnnectID;
	}Stru_RTBConnect,*Stru_pRTBConnect;

	typedef struct Tag_Stru_ThrHeader
	{
		nuUSHORT MapID;
		nuUSHORT RTBCount;
	}Stru_ThrHeader,*pStru_ThrHeader;

	typedef struct Tag_Stru_Thr
	{
		Stru_ThrHeader	ThrHeader;
		Stru_RTBConnect	**RTBConnect;
		Stru_ThrEachMap	pThrEachMap;
	}Stru_Thr,*pStru_Thr;

	class Class_ThrCtrl : public Stru_Thr
	{
		public:
			Class_ThrCtrl();
			virtual ~Class_ThrCtrl();

			nuBOOL InitClass(nuLONG mapidx,nuLONG rtrule);
			nuVOID CloseClass();

			nuBOOL	Read_Header();//讀取表頭資料
			nuBOOL	Remove_Header();//移除表頭資料

			nuBOOL	ReSet();//移除Rt檔

			nuFILE	*ThrFile;
			nuTCHAR	FName[NURO_MAX_PATH];
		#ifdef _DEBUG
			int  ThrAlocCount;
			int  ThrFreeCount;
		#endif
		private:
			nuVOID	IniConst();

			nuLONG	MapIdx;
			nuLONG	l_RoutingRule;
	};

#endif