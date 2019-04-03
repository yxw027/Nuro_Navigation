#ifndef Def_ClassTTICtrl
	#define Def_ClassND6Ctrl

	#include	"ConstSet.h"
	#include	"Stru_StateStru.h"
	#include	"NuroDefine.h"
	#define		_TTI_TIME_MAX	4
	#define	    _TTI_RESERVESIZE 96

	typedef struct Tag_TTI_TimeData_Stru
	{
		nuLONG SHOUR : 16;
		nuLONG EHOUR : 16;
		nuLONG SDATE : 16;
		nuLONG EDATE : 16;
	}TTI_TimeData_Stru,*pTTI_TimeData_Stru;

	typedef struct Tag_TTI_Data_Stru
	{
		nuLONG  DAY:16;
		nuLONG  CARTYPE:16;
		TTI_TimeData_Stru TimeData[_TTI_TIME_MAX];
	}TTI_Data_Stru,*pTTI_Data_Stru;

	typedef struct Tag_TTI_Stru
	{
		nuLONG TTICnt;
		nuBYTE Reserve[_TTI_RESERVESIZE];
		pTTI_Data_Stru *ppTTIData;
	}TTI_Stru,*pTTI_Stru;

	class Class_TTICtrl : public TTI_Stru
	{
		public:
			Class_TTICtrl();
			virtual ~Class_TTICtrl();

			nuBOOL InitClass();
			nuVOID CloseClass();
			nuVOID Release();

			nuBOOL ReadFile();
			nuBOOL GetTTIData(nuLONG Addr, TTI_Data_Stru &TTIData);

			nuFILE	*m_pTTIFile;
			nuTCHAR	m_tsFName[NURO_MAX_PATH];
		#ifdef _DEBUG
			nuLONG TTIAlocCount;
			nuLONG TTIFreeCount;
		#endif
	};
#endif