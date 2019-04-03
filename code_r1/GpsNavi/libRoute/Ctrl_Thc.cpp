// Ctrl_Thc.cpp: implementation of the CCtrl_Thc class.
//
//////////////////////////////////////////////////////////////////////
#include "stdAfxRoute.h"

#include "Ctrl_Thc.h"
#include "RouteBase.h"
#ifdef _DEBUG
#include "stdio.h"
#endif

CCtrl_Thc::CCtrl_Thc()
{
	IniConst();
#ifdef _DEBUG
	ThcAlocCount = 0;
	ThcFreeCount = 0;
#endif
}

CCtrl_Thc::~CCtrl_Thc()
{
	#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"Thc_MemUesd %d\n", ThcAlocCount - ThcFreeCount);
			fclose(pfile);
		}
	#endif
	#endif
	CloseClass();
}

nuBOOL CCtrl_Thc::InitClass(nuLONG rtrule)
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	l_RoutingRule=rtrule;
	#ifdef DEF_MAKETHRROUTE
	#else
		switch(l_RoutingRule)
		{
			case 0:		nuTcscpy(FName, nuTEXT("0.thc"));	break;
			case 1:		nuTcscpy(FName, nuTEXT("1.thc"));	break;//台灣才有(無此項)
			case 2:		nuTcscpy(FName, nuTEXT("2.thc"));	break;//台灣才有(無此項)
			case 3:		nuTcscpy(FName, nuTEXT("3.thc"));	break;
			case 4:		nuTcscpy(FName, nuTEXT("0.thc"));	break;
			case 5:		nuTcscpy(FName, nuTEXT("0.thc"));	break;
			case 6:		nuTcscpy(FName, nuTEXT("6.thc"));	break;//無此項
			default:	nuTcscpy(FName, nuTEXT("0.thc"));	break;
		}		
		g_pRtFSApi->FS_FindFileWholePath(-1,FName,NURO_MAX_PATH);
	#endif
	return nuTRUE;
}
nuVOID CCtrl_Thc::CloseClass()
{
	Remove();
}

nuBOOL CCtrl_Thc::Read()//讀取表頭資料
{
	ThcFile=nuTfopen(FName, NURO_FS_RB);
	if(ThcFile==NULL)
	{
		Remove();
		return nuFALSE;
	}
	if(NULL !=nuFseek(ThcFile,0,NURO_SEEK_SET))
	{
		Remove();
		return nuFALSE;
	}
	if(1!=nuFread(&TotalFNodeCount,sizeof(nuULONG),1,ThcFile))
	{
		Remove();
		return nuFALSE;
	}
	if(1!=nuFread(&TotalConnectCount,sizeof(nuULONG),1,ThcFile))
	{
		Remove();
		return nuFALSE;
	}
	if(1!=nuFread(&ConnectStruStartAddr,sizeof(nuULONG),1,ThcFile))
	{
		Remove();
		return nuFALSE;
	}

	#ifdef	DEFNEWTHCFORMAT
		if(1!=nuFread(&ViaCityAddrStartAddr,sizeof(nuULONG),1,ThcFile))
		{
			Remove();
			return nuFALSE;
		}
	#endif

	p_StruFNode		=	(StruFNode**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_StruFNode)*TotalFNodeCount);
	if(p_StruFNode==NULL)
	{
		Remove();
		return nuFALSE;
	}
#ifdef _DEBUG
	ThcAlocCount++;
#endif
	#ifdef	DEFNEWTHCFORMAT
		if(NULL != nuFseek(ThcFile,sizeof(nuULONG)*4,NURO_SEEK_SET))
		{
			Remove();
			return nuFALSE;
		}
	#else
		if(NULL != nuFseek(ThcFile,sizeof(nuULONG)*3,NURO_SEEK_SET))
		{
			Remove();
			return nuFALSE;
		}
	#endif
	if(TotalFNodeCount!=nuFread(*p_StruFNode,sizeof(Tag_StruFNode),TotalFNodeCount,ThcFile))
	{
		Remove();
		return nuFALSE;
	}
	if(ThcFile)
	{
		nuFclose(ThcFile);
		ThcFile=NULL;
	}
	b_ThcReady			=	nuTRUE;
	nuSleep(_SLEEPTICKCOUNT);
	return nuTRUE;
}

nuBOOL CCtrl_Thc::ReadConnect(nuLONG StartIndex,nuULONG Count)//讀取表頭資料
{
	ThcFile=nuTfopen(FName, NURO_FS_RB);
	if(ThcFile==NULL){
		Remove();
		return nuFALSE;
	}
	if(p_StruConnect!=NULL)
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)p_StruConnect);
		p_StruConnect=NULL;
#ifdef _DEBUG
		ThcFreeCount++;
#endif
	}

	#ifdef	DEFNEWTHCFORMAT
		if(NULL != nuFseek(ThcFile,ConnectStruStartAddr + sizeof(Tag_StruConnect)*StartIndex ,NURO_SEEK_SET)){
			Remove();
			return nuFALSE;
		}
	#else
		if(NULL != nuFseek(ThcFile,sizeof(nuULONG)*3 + sizeof(Tag_StruFNode)*TotalFNodeCount + sizeof(Tag_StruConnect)*StartIndex ,NURO_SEEK_SET)){
			Remove();
			return nuFALSE;
		}
	#endif
	if(Count==0) return nuFALSE;

	p_StruConnect	=	(StruConnect**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_StruConnect)*Count);
	if(p_StruConnect==NULL)	{	Remove();	return nuFALSE;	}
#ifdef _DEBUG
	ThcAlocCount++;
#endif
	if(Count!=nuFread(*p_StruConnect,sizeof(Tag_StruConnect),Count,ThcFile))	{	Remove();	return nuFALSE;	}
	if(ThcFile!=NULL){	nuFclose(ThcFile);	ThcFile=NULL;	}
	return nuTRUE;
}

#ifdef	DEFNEWTHCFORMAT
	nuBOOL CCtrl_Thc::ReadViaNodeData(nuLONG StartIndex,nuULONG Count)//讀取ViaNodeData
	{
		ThcFile=nuTfopen(FName, NURO_FS_RB);
		if(ThcFile==NULL){
			Remove();
			return nuFALSE;
		}
		if(ppstuViaNodeData!=NULL)
		{
			g_pRtMMApi->MM_FreeMem((nuPVOID*)ppstuViaNodeData);
			p_StruConnect=NULL;
#ifdef _DEBUG
		ThcFreeCount++;
#endif
		}

		if(NULL != nuFseek(ThcFile,ViaCityAddrStartAddr + sizeof(VIACITYNODEDATA)*StartIndex ,NURO_SEEK_SET)){
			Remove();
			return nuFALSE;
		}
		if(Count==0) return nuFALSE;

		ppstuViaNodeData	=	(VIACITYNODEDATA**)g_pRtMMApi->MM_AllocMem(sizeof(VIACITYNODEDATA)*Count);
		if(ppstuViaNodeData==NULL)	{	Remove();	return nuFALSE;	}
#ifdef _DEBUG
		ThcAlocCount++;
#endif
		if(Count!=nuFread(*ppstuViaNodeData,sizeof(VIACITYNODEDATA),Count,ThcFile))	{	Remove();	return nuFALSE;	}
		if(ThcFile!=NULL){	nuFclose(ThcFile);	ThcFile=NULL;	}
		return nuTRUE;
	}
#endif

nuBOOL CCtrl_Thc::Remove()//移除表頭資料
{
	if(p_StruFNode!=NULL)
	{
#ifdef _DEBUG
		ThcFreeCount++;
#endif		
		g_pRtMMApi->MM_FreeMem((nuPVOID*)p_StruFNode);		
		p_StruFNode=NULL;		
	}
	if(p_StruConnect!=NULL)
	{	
#ifdef _DEBUG
		ThcFreeCount++;
#endif	
		g_pRtMMApi->MM_FreeMem((nuPVOID*)p_StruConnect);	
		p_StruConnect=NULL;		
	}
	#ifdef	DEFNEWTHCFORMAT
		if(ppstuViaNodeData!=NULL)
		{
		#ifdef _DEBUG
			ThcFreeCount++;
		#endif	
			g_pRtMMApi->MM_FreeMem((nuPVOID*)ppstuViaNodeData);	
			ppstuViaNodeData=NULL;	
		}
	#endif

	TotalFNodeCount			=	0;
	TotalConnectCount		=	0;
	ConnectStruStartAddr	=	0;
	#ifdef	DEFNEWTHCFORMAT
		ViaCityAddrStartAddr	=	0;
	#endif
	if(ThcFile!=NULL){	nuFclose(ThcFile);	ThcFile=NULL;	}
	return nuTRUE;
}


nuVOID CCtrl_Thc::IniConst()
{
	TotalFNodeCount		=	0;
	TotalConnectCount	=	0;
	ConnectStruStartAddr	=	0;
	#ifdef	DEFNEWTHCFORMAT
		ViaCityAddrStartAddr	=	0;
		ppstuViaNodeData		=	NULL;
	#endif
	p_StruFNode			=	NULL;
	p_StruConnect		=	NULL;
	ThcFile				=	NULL;
	b_ThcReady			=	nuFALSE;
	l_RoutingRule=0;
}
