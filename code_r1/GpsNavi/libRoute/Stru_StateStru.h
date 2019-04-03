// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: louis $
// $Date: 2009/09/18 05:52:00 $
// $Revision: 1.2 $
#ifndef Def_Stru_StateStru
	#define Def_Stru_StateStru
	typedef struct Tag_StateStru
	{
		nuULONG MapID;//圖資發行時所賦予的索引值
		nuLONG  MapIdx;//FileNameSystem所賦予的索引值
		nuULONG RtBlockID;//圖資發行時所賦予的RtBlock索引值
		nuLONG  RtBlockIdx;//運算過程中所賦予的RtBlock索引值(理論上會與RtBlockID相等)
		nuLONG  DwBlockID;//GlobeBlockID
		nuLONG  DwBlockIdx;//運算過程中所賦予的Block索引值
	}StateStru,*pStateStru;
#endif