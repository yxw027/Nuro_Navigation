// Ctrl_RuleSet.cpp: implementation of the CCtrl_RuleSet class.
//
//////////////////////////////////////////////////////////////////////
#include "stdAfxRoute.h"
#include "RouteBase.h"

#include "Ctrl_RuleSet.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCtrl_RuleSet	*g_pRoutingRule = NULL;

CCtrl_RuleSet::CCtrl_RuleSet()
{
	l_TotalSupportCount=0;
	l_HighestSpeed=0;
	pRuleItem=NULL;

	l_SupportMode=0;
	l_Weighting=NULL;
	BaseSpeed_KMH2MS=0;

	RuleSetFile = NULL;
#ifdef _DEBUG
	RuleAlocCount = 0;
	RuleFreeCount = 0;
#endif
}

CCtrl_RuleSet::~CCtrl_RuleSet()
{
	ReleaseRoutingRule();
	#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"Rule_MemUesd %d\n", RuleAlocCount - RuleFreeCount);
			fclose(pfile);
		}
	#endif
	#endif
}

nuBOOL CCtrl_RuleSet::ReadRoutingRule(nuLONG l_RoutingRule)
{
	if(l_RoutingRule < 0 || l_RoutingRule > 6)
	{
		l_RoutingRule = 0;
	}
	nuLONG HighWayRule=0,l;
	
	ReleaseRoutingRule();
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	nuTcscpy(FName, RootPath);
	nuTcscat(FName, nuTEXT("Setting\\RuleSet.dat"));

	RuleSetFile=nuTfopen(FName, NURO_FS_RB);//檔名要確認好
	if(RuleSetFile==NULL){
		ReleaseRoutingRule();
		return nuFALSE;
	}
	if(NULL != nuFseek(RuleSetFile,0,NURO_SEEK_SET)){ReleaseRoutingRule(); return nuFALSE;}
	if(1 != nuFread(&l_TotalSupportCount,sizeof(nuLONG),1,RuleSetFile)){ReleaseRoutingRule(); return nuFALSE;}
	if(1 != nuFread(&l_SupportMode,sizeof(nuLONG),1,RuleSetFile)){ReleaseRoutingRule(); return nuFALSE;}
	if(1 != nuFread(&l_HighestSpeed,sizeof(nuLONG),1,RuleSetFile)){ReleaseRoutingRule(); return nuFALSE;}
	if(1 != nuFread(&l_3dp3ds,sizeof(nuLONG),1,RuleSetFile)){ReleaseRoutingRule(); return nuFALSE;}
	pRuleItem = (Tag_RuleSetItemStru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_RuleSetItemStru)*l_TotalSupportCount*l_SupportMode);
	l_Weighting = (nuSHORT**)g_pRtMMApi->MM_AllocMem(sizeof(nuSHORT)*l_TotalSupportCount);

	if(pRuleItem==NULL || l_Weighting==NULL){
		ReleaseRoutingRule();
		return nuFALSE;
	}
#ifdef _DEBUG
	RuleAlocCount += 2;
#endif
	if(NULL != nuFseek(RuleSetFile,sizeof(nuLONG)*4,NURO_SEEK_SET)){ReleaseRoutingRule(); return nuFALSE;}
	if((nuDWORD)(l_TotalSupportCount*l_SupportMode) != nuFread(*pRuleItem,sizeof(Tag_RuleSetItemStru),l_TotalSupportCount*l_SupportMode,RuleSetFile))
	{ReleaseRoutingRule(); return nuFALSE;}

	//變數轉換工作
//	HighWayRule=l_RoutingRule%4;
	HighWayRule=l_RoutingRule;
	if(HighWayRule>l_SupportMode) HighWayRule=0;
	for(l=0;l<l_TotalSupportCount;l++){
		if(l_HighestSpeed<(*pRuleItem)[HighWayRule*l_TotalSupportCount+l].Speed)
			l_HighestSpeed=(*pRuleItem)[HighWayRule*l_TotalSupportCount+l].Speed;
	}
	BaseSpeed_KMH2MS=l_HighestSpeed*10/36;
	for(l=0;l<l_TotalSupportCount;l++){
		if((*pRuleItem)[HighWayRule*l_TotalSupportCount+l].Speed==0 && (*pRuleItem)[HighWayRule*l_TotalSupportCount+l].Speed==1){
			(*pRuleItem)[HighWayRule*l_TotalSupportCount+l].Speed=1;
			(*l_Weighting)[l] = 0;
		}
		else{
			(*l_Weighting)[l] = (l_HighestSpeed*10)/(*pRuleItem)[HighWayRule*l_TotalSupportCount+l].Speed;
		}
	}
	return nuTRUE;
}

nuVOID CCtrl_RuleSet::ReleaseRoutingRule()
{
	l_TotalSupportCount = 0;
	l_HighestSpeed		= 0;
	l_SupportMode		= 0;
	l_3dp3ds			= 0;
	BaseSpeed_KMH2MS	= 0;
	if(l_Weighting!=NULL)
	{
#ifdef _DEBUG
		RuleFreeCount++;
#endif	
		g_pRtMMApi->MM_FreeMem((nuPVOID*)l_Weighting);		
		l_Weighting=NULL;
	}
	if(pRuleItem!=NULL)
	{	
#ifdef _DEBUG
		RuleFreeCount++;
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)pRuleItem);		
		pRuleItem=NULL;
	}
	if(RuleSetFile != NULL)
	{
		nuFclose(RuleSetFile);
		RuleSetFile = NULL;
	}
}
