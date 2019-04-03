// Ctrl_NT.cpp: implementation of the CCtrl_NT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdAfxRoute.h"

#include "Ctrl_NT.h"
#ifdef _DEBUG
#include "stdio.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtrl_NT::CCtrl_NT()
{
	InitNTStruct();
#ifdef _DEBUG
	NTAlocCount = 0;
	NTFreeCount = 0;
#endif
}

CCtrl_NT::~CCtrl_NT()
{
	#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"NT_MemUesd %d\n", NTAlocCount - NTFreeCount);
			fclose(pfile);
		}
	#endif
	#endif
	CloseClass();
}

nuBOOL CCtrl_NT::InitClass()
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	return nuTRUE;
}
nuVOID CCtrl_NT::CloseClass()
{
	ReleaseStruct();
}


nuBOOL CCtrl_NT::ReadStruct()
{
	NTFile=nuTfopen(FName, NURO_FS_RB);
	if(NTFile==NULL)																	{	ReleaseStruct();	return nuFALSE;	}
	if(0!=nuFseek(NTFile,0,NURO_SEEK_SET))												{	ReleaseStruct();	return nuFALSE;	}
	if(1!=nuFread(&TotalNoTurnCount,sizeof(nuLONG),1,NTFile))								{	ReleaseStruct();	return nuFALSE;	}
	m_NTItem=(StruNTItem**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_StruNTItem)*TotalNoTurnCount);
#ifdef _DEBUG
	NTAlocCount++;
#endif
	if(m_NTItem==NULL)																	{	ReleaseStruct();	return nuFALSE;	}
	if(0!=nuFseek(NTFile,sizeof(nuLONG),NURO_SEEK_SET))									{	ReleaseStruct();	return nuFALSE;	}
	if(TotalNoTurnCount!=nuFread(*m_NTItem,sizeof(Tag_StruNTItem),TotalNoTurnCount,NTFile))	{	ReleaseStruct();	return nuFALSE;	}
	if(NTFile!=NULL)																	{	nuFclose(NTFile);		NTFile=NULL;	}
	return nuTRUE;
}

nuVOID CCtrl_NT::ReleaseStruct()
{
	if(m_NTItem!=NULL)
	{
#ifdef _DEBUG
	NTFreeCount++;
#endif	
	g_pRtMMApi->MM_FreeMem((nuPVOID*)m_NTItem);	
	m_NTItem=NULL;	
	}

	if(NTFile!=NULL){	nuFclose(NTFile);	NTFile=NULL;	}
	TotalNoTurnCount=0;
}

nuVOID CCtrl_NT::InitNTStruct()
{
	TotalNoTurnCount=0;
	m_NTItem=NULL;//Sort by PassNode,StartNode,EndNode
	NTFile=NULL;
}