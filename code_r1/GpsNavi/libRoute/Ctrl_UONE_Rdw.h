 // Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/18 05:52:00 $
// $Revision: 1.3 $
/* 本檔為Draw(DW)檔的資料控制元件  CopyRight LouisLin 20050131 */
	#ifndef Def_UONEDwCtrl
		#define Def_UONEDwCtrl

		#include "ConstSet.h"
		#include "NuroDefine.h"

		typedef struct Tag_UONEDWHeader//固定的表頭
		{
			nuULONG	ClassCnt;
			nuLONG**			BlockClassStart;//道路起始位置串
		}UONEDWHeader,*pUONEDWHeader;

		typedef struct TagUONERoadData
		{		
			nuUSHORT	RoadCommonIndex;//道路共同資料索引值
			nuUSHORT	VertexCount;//節點總數
			nuULONG	    StartVertexIndex :24;//起始節點索引值
			nuDWORD		OneWay:2;
			nuDWORD		bGPSState:1;
			nuDWORD		nLevel:5;
			nuLONG		HouseNumberAddr;//門牌資訊位置
		}UONERoadData;
		typedef struct TagUONERoadCommonData
		{		
			nuLONG	    RoadNameAddr;//路名位置
			nuULONG	    RoadNameLen:8;//路名長度
			nuULONG 	RoadType:4;//道路型態
			nuULONG 	SpeedLimit:4;//道路型態
			nuULONG 	CityID : 6;//城市碼
			nuULONG     TownID : 10;//鄉鎮碼
		}UONERoadCommonData;

		typedef struct Tag_UONEBlockRoadData
		{
			nuULONG		RoadCount;//道路總數
			nuULONG		RdCommCount;//道路總數
			nuULONG		VertexCount;//節點總數
			UONERoadData		**RdData;//道路資料
			UONERoadCommonData	**RdCommData;//共同資料
			NURORECT			**RdBoundary;//道路資料
			NUROSPOINT			**VertexCoor;//節點座標串
		}UONEBlockRoadData,*pUONEBlockRoadData;

		class UONERDwCtrl
		{
			public:
				UONERDwCtrl();
				virtual ~UONERDwCtrl();

				nuBOOL InitClass();
				nuVOID CloseClass();

				nuBOOL	Reset();
				nuBOOL	Read_Header();//讀取表頭資料
				nuVOID	Remove_Header();//移除表頭資料
				nuBOOL	Read_BlockData(nuULONG Index);
				nuVOID	Remove_BlockData(nuULONG Index);	
				
				nuBOOL	GetCityTownID(nuULONG Now_BlockIdx,nuULONG Now_RoadIdx,nuULONG &NowCityID,nuULONG &NowTownID);

			private:
				nuVOID	InitConst();
				nuBOOL	CreateMainStru(nuULONG Count);
				nuVOID	ReleaseMainStru();
				nuBOOL	Read_RoadData(nuULONG Index);//讀取道路資料
				nuVOID	Remove_RoadData(nuULONG Index);//移除道路資料
			public:
				nuFILE				*DWFile;
				UONEDWHeader		Header;
				UONEBlockRoadData	**Rd;
				nuTCHAR				FName[NURO_MAX_PATH];
				nuLONG				RDWFileIdx;
			#ifdef _DEBUG
				int  UoneRdwAlocCount;
				int  UoneRdwFreeCount;
			#endif
			private:
				nuBOOL	ReadHeaderOK;//判斷檔頭讀取與否 只有在已讀取檔頭的情況下 才可以操控資料
				nuBOOL	**ReadBlockOK;//判斷是否有讀取Block資料
		};

	#endif
