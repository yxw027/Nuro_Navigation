// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/18 05:52:00 $
// $Revision: 1.2 $
#ifndef Def_RefCtrl
	#define Def_RefCtrl
	//提供 CityID+TownID 與 RtAreaID 的對應關係
	#include "NuroDefine.h"

	typedef struct Tag_RefStru
	{
		nuUSHORT	CityID;
		nuUSHORT	TownID;
		nuULONG	RtAreaID;
	}RefStru;

	class RefCtrl
	{
		public:
			RefCtrl();
			virtual ~RefCtrl();

			nuBOOL InitClass();
			nuBOOL ReadRefFile();
			nuVOID ReleaseRef();
			nuVOID CloseClass();
		private:
		public:
			nuUINT RefStruCount;
			RefStru **pRefStru;
			nuFILE	*RefFile;
			nuTCHAR	FName[NURO_MAX_PATH];
		#ifdef _DEBUG
			nuINT  RefAlocCount;
			nuINT  RefFreeCount;
		#endif
		private:
	};


#endif