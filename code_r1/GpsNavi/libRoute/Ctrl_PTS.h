#ifndef Def_PTSCtrl
	#define Def_PTSfCtrl
	//提供 CityID+TownID 與 RtAreaID 的對應關係
	#include "NuroDefine.h"

	typedef struct Tag_PTSStru
	{
		nuULONG	NodeID : 20;//起始NodeID
		nuULONG	RtBID : 12;
		nuULONG	NameAddr;
	}PTSStru;

	class CCtrl_PTS
	{
		public:
			CCtrl_PTS();
			virtual ~CCtrl_PTS();

			nuBOOL InitClass();
			nuBOOL ReadPTSFile();
			nuVOID ReleasePTS();
			nuVOID CloseClass();
		private:
		public:
			nuUINT PTSStruCount;
			PTSStru **pPTSStru;
			nuFILE	*PTSFile;
			nuTCHAR	FName[NURO_MAX_PATH];
		#ifdef _DEBUG
			int  PTSAlocCount;
			int  PTSFreeCount;
		#endif
		private:
	};


#endif