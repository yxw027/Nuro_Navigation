	#ifndef Def_CMPCtrl
		#define Def_CMPCtrl

		#include "NuroDefine.h"
		#include "NuroModuleApiStruct.h"
		#include "stdio.h"

		typedef struct TagCMPHeaderStru
		{
			nuUINT DataCnt;
			nuCHAR Reserve[196];
		}CMPHeaderStru,*pCMPHeaderStru;

		typedef struct TagCMPMainDataStru
		{
			nuUINT x;
			nuUINT y;
			nuUINT thcid;
		}CMPMainDataStru,*pCMPMainDataStru;

		typedef struct TagCMPStru
		{
			CMPHeaderStru CMPHeader;
			CMPMainDataStru *pCMPMainData;
		}CMPData,*pCMPData;

		class CCMPCtrl : public CMPData
		{
			public:
				CCMPCtrl();
				virtual ~CCMPCtrl();
				nuBOOL	InitClass();
				nuVOID	CloseClass();
				nuBOOL	ReadFile();
				nuVOID	Release();
				nuFILE  *m_pCMPFile;			
		};

	#endif