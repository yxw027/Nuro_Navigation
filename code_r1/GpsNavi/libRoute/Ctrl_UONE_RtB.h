// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2010/10/21 09:54:16 $
// $Revision: 1.5 $
	#ifndef Def_UONERtBCtrl
		#define Def_UONERtBCtrl

		#include	"Stru_StateStru.h"
		#include	"ConstSet.h"
		#include	"NuroDefine.h"

		typedef struct Tag_NUROUSPOINT
		{
			nuUSHORT  x;
			nuUSHORT  y;
		}NUROUSPOINT,*pNUROUSPOINT;

		typedef struct Tag_UONEF_RtB_Header_Stru
		{
			nuULONG  TotalNodeCount;//總Node數
			nuULONG  TotalRoadCount;//總Road數
			nuULONG  RtConnectStruCount;//總連結數
			nuLONG   RtIndexStruStartAddr;//所以資料起始位置
			nuLONG   RtConnectStruStartAddr;//連結結構起始位置
		}UONEF_RtB_Header_Stru,*pUONEF_RtB_Header_Stru;

#ifdef VALUE_EMGRT
		typedef struct Tag_UONEF_RtB_MainData_Stru
		{
			nuULONG   FNodeID : 20;//起始NodeID
			nuULONG   FRtBID : 12;
			nuULONG   ConnectStruIndex : 20;//連結資料索引值
			nuULONG   SPTFlag : 1;    //高速列表
			nuULONG   PTSFlag  : 1;   //收費站
			nuULONG   ARFlag   : 1;   //相鄰城市
			nuULONG   Reverse  : 1;   //保留
			nuULONG   ConnectCount : 6;//連結總數
			nuULONG   NoTurnItem : 1;//轉彎標示
			nuULONG   TrafficLight : 1;//紅綠燈
			NUROPOINT NodeCoor;
		}UONEF_RtB_MainData_Stru,*pUONEF_RtB_MainData_Stru;
#else
		typedef struct Tag_UONEF_RtB_MainData_Stru
		{
			nuULONG  FNodeID : 20;//起始NodeID
			nuULONG  FRtBID : 12;
			nuULONG  ConnectStruIndex : 20;//連結資料索引值
			nuULONG  CoorLVX : 2;
			nuULONG  CoorLVY : 2;
			nuULONG  ConnectCount : 4;//連結總數
			nuULONG  TrafficLight:1;//紅綠燈
			nuULONG  Reverse :1;//保留
			nuULONG  NoTurnItem : 1;//轉彎標示
			nuULONG  LaneGuideFlag:1;//車道訊息
			NUROUSPOINT NodeCoor;
		}UONEF_RtB_MainData_Stru,*pUONEF_RtB_MainData_Stru;
#endif

		typedef struct Tag_UONEF_RtB_Connect_Stru
		{
			nuULONG   TNodeID : 20;//起始NodeID
			nuULONG   TRtBID : 12;
			nuULONG   RoadLength:16;//道路長度
			nuULONG   RoadClass:8;//道路等級
			nuULONG   Connecttype : 2;//default:0
			nuULONG   VoiceRoadType : 6;//語音播報路型
		}UONEF_RtB_Connect_Stru,*pUONEF_RtB_Connect_Stru;

		typedef struct Tag_UONEF_RtB_Index_Stru
		{
			nuUSHORT  BlockIdx;//道路代碼
			nuUSHORT  RoadIdx;//道路代碼
			nuULONG   FNodeID : 20;//起始NodeID
			nuULONG   FRtBID : 12;
			nuULONG   TNodeID : 20;//起始NodeID
			nuULONG   TRtBID : 12;
		}UONEF_RtB_Index_Stru,*pUONEF_RtB_Index_Stru;

		typedef struct Tag_UONERtBlockData
		{
			StateStru				MyState;
			NUROPOINT					OrgCoor;
			UONEF_RtB_Header_Stru		RtBHeader;//Rt的表頭
			pUONEF_RtB_MainData_Stru	*RtBMain;//Rt中的主資料
			pUONEF_RtB_Connect_Stru		*RtBConnect;//Rt中的連結資料
			pUONEF_RtB_Index_Stru		*RtBIdx;//Rt中RoadID與NodeID的對應資料
		}UONERtBData,*pUONERtBData;

		class UONERtBCtrl : public UONERtBData
		{
			public:
				UONERtBCtrl();
				virtual ~UONERtBCtrl();
				nuBOOL	InitClass();
				nuVOID	CloseClass();
				nuBOOL	Read_Header(nuLONG StartAddr);//,FILE *RtFile);//讀取表頭資料
				nuBOOL	Read_MainData();//產生主資料儲存陣列
				nuBOOL	Read_Connect();//產生聯通資料儲存陣列
				nuBOOL	Read_Index();//讀取地標點資料
				nuBOOL	ReSet();//移除Rt檔
				nuVOID	Remove_Index();//讀取背景線資料

			private:
				nuVOID	Remove_Header();//移除表頭資料
				nuVOID	Remove_MainData();//移除主資料儲存陣列
				nuVOID	Remove_Connect();//移除聯通資料儲存陣列
				nuVOID	IniConst();

			public:
				nuFILE	*RtBFile;
				nuTCHAR	FName[NURO_MAX_PATH];

			#ifdef _DEBUG
				nuINT  UoneRtBAlocCount;
				nuINT  UoneRtBFreeCount;
			#endif

			private:
				nuBOOL	ReadHeaderOK;//判斷檔頭讀取與否 只有在已讀取檔頭的情況下 才可以操控資料
				nuLONG	m_StartAddr;
		};
	#endif
