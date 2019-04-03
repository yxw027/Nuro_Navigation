// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/18 05:52:00 $
// $Revision: 1.2 $
/* 本檔為FileBlock(FB)檔的資料控制元件  CopyRight LouisLin 20050131 */
#ifndef Def_FBCtrl
	#define Def_FBCtrl
	#include "NuroDefine.h"

	typedef struct Tag_BlockSegmentStru//Block段落
	{
		nuULONG FromBlockID;
		nuULONG ToBlockID;
	}BlockSegmentStru,*pBlockSegmentStru;

	class FBCtrl
	{
		public:
			FBCtrl();
			virtual ~FBCtrl();

			nuBOOL InitClass();
			nuVOID CloseClass();

			nuBOOL ReadFB();//讀取FB檔資料
			nuVOID ReleaseFB();
		private:
		public:
			nuULONG SegmentCount;//段落數
			BlockSegmentStru	**BS;//Block段落
			nuFILE *FB;
			nuTCHAR				FName[NURO_MAX_PATH];
		private:
	};

#endif