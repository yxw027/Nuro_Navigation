// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/18 05:52:00 $
// $Revision: 1.2 $
/* 本檔為FileBlock(FB)檔的資料控制元件  CopyRight LouisLin 20050131 */
	#ifndef Def_BHCtrl
		#define Def_BHCtrl
		#include "NuroDefine.h"

		typedef struct Tag_BHHeaderStru//檔案描述區
		{
			nuBYTE Version[6];//圖資版本
			nuBYTE Reverser[8];//初始座標
			nuBYTE Builder[8];//圖資建構日期mm/dd/yy
			nuBYTE MapID[6];//檔案識別碼(國碼3+州碼2+其他1)
			nuULONG BlockCount;//總區間數
		}BHHeaderStru,*pBHHeaderStru;

		typedef struct Tag_BlockHeaderStru//Block段落
		{
			BHHeaderStru m_BHH;
			nuBYTE **BHdata;//區間唯一碼
			//主要資料區(主要分成五大類:區間定義,點,線,背景線,背景面)
/*
			nuULONG **GlobalBlockID;//區間唯一碼

			nuLONG **POIDataAddr;//地標點資料位置
			nuLONG **POIDataSize;//地標點資料大小

			nuLONG **RoadDataAddr;//道路資料位置
			nuLONG **RoadAddCount;//道路累積量
			nuLONG **RoadDataSize;//道路資料大小

			nuLONG **BgLDataAddr;//背景線資料位置
			nuLONG **BgLDataSize;//背景線資料大小

			nuLONG **BgADataAddr;//背景面資料位置
			nuLONG **BgADataSize;//背景面資料大小

			NUROPOINT **BlockBaseCoor;//區間基準座標
*/
		}BlockHeaderStru,*pBlockHeaderStru;

		class UONEBHCtrl
		{
			public:
				UONEBHCtrl();
				virtual ~UONEBHCtrl();

				nuBOOL InitClass();
				nuVOID CloseClass();

				nuBOOL ReadBH();//讀取FB檔資料
				nuVOID ReleaseBH();

				nuULONG GlobalBlockID(nuLONG index);//區間唯一碼
				nuLONG POIDataAddr(nuLONG index);//地標點資料位置
				nuLONG POIDataSize(nuLONG index);//地標點資料大小
				nuLONG RoadDataAddr(nuLONG index);//道路資料位置
				nuLONG RoadAddCount(nuLONG index);//道路累積量
				nuLONG RoadDataSize(nuLONG index);//道路資料大小
				nuLONG BgLDataAddr(nuLONG index);//背景線資料位置
				nuLONG BgLDataSize(nuLONG index);//背景線資料大小
				nuLONG BgADataAddr(nuLONG index);//背景面資料位置
				nuLONG BgADataSize(nuLONG index);//背景面資料大小
				NUROPOINT BlockBaseCoor(nuLONG index);//區間基準座標

			private:
			public:
				BlockHeaderStru	m_BH;//Block段落
				nuFILE *p_FBH;
				nuTCHAR	FName[NURO_MAX_PATH];
				nuLONG BHFileIdx;
			#ifdef _DEBUG
				int  UoneBHAlocCount;
				int  UoneBHFreeCount;
			#endif
			private:
		};

	//	extern UONEBHCtrl			m_UOneBH;
	//extern UONEBHCtrl m_UOneBH;
	#endif
