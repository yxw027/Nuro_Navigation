#pragma once

#include "NuroDefine.h"

#define  FILE_CSC_VD			nuTEXT("UIdata\\國道VD.dat")

typedef struct Tag_UNITVD
{
	nuCHAR	VDID[16];
	nuCHAR	VDName[32];
	nuLONG	VDSpeed;
	nuLONG	EventCount;
	nuCHAR	Event1[32];
	nuCHAR	Event2[32];
	nuCHAR	Event3[32];
	nuCHAR	Event4[32];	
}UNITVD,*PUNITVD;


typedef struct Tag_EVENTVD
{
	nuCHAR	EventID[32];
	nuCHAR	Event[256];
}EVENTVD,*PEVENTVD;


class CFileMapTMCLU
{
public:
	CFileMapTMCLU(nuVOID);
	~CFileMapTMCLU(nuVOID);

	nuBOOL ReadVD();
	nuLONG GetVDCount();
	nuBOOL GetVDUnit(nuLONG UnitIndex,UNITVD &VD);
	nuBOOL CleanVDData();
	nuBOOL GetVD(nuVOID*pTmcBuf,const nuUINT size);

	nuLONG nVDCount;
	UNITVD *pVDData;//裝填VD資料的結構指標


	nuBOOL GetVDEVENT(nuCHAR *pEvent,EVENTVD &VDevent);

	nuLONG nVDEventCount;
	EVENTVD *pVDevent;
	nuBOOL bVDInit;
};

extern CFileMapTMCLU cTMC;
