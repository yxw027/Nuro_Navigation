#ifndef Def_ClassND6Ctrl
	#define Def_ClassND6Ctrl

	#include	"ConstSet.h"
	#include	"Stru_StateStru.h"
	#include	"NuroDefine.h"

	#define	    _RESERVESIZE 96

	typedef struct Tag_ND6_Data_Stru
	{
		nuDWORD NODEID : 20;
		nuDWORD RtBID  : 12;
		NUROPOINT NodeCoor;
	}ND6_Data_Stru,*pND6_Data_Stru;

	typedef struct Tag_ND6_Stru
	{
		nuLONG DataCnt;
		nuBYTE  Reserve[_RESERVESIZE];
		pND6_Data_Stru *ppND6Data;
	}ND6_Stru,*pND6_Stru;

	class Class_ND6Ctrl : public ND6_Stru
	{
		public:
			Class_ND6Ctrl();
			virtual ~Class_ND6Ctrl();

			nuBOOL InitClass();
			nuVOID CloseClass();
			nuVOID Release();

			nuBOOL ReadFile();
			nuBOOL MappingFile(nuDWORD RtbID, nuDWORD NodeID, NUROPOINT Coor);

			nuFILE	*m_pND6File;
			nuTCHAR	m_tsFName[NURO_MAX_PATH];
	};
#endif