// Ctrl_3dp.cpp: implementation of the CCtrl_3dp class.
//
//////////////////////////////////////////////////////////////////////
#include "stdAfxRoute.h"

#include "Ctrl_3dp.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtrl_3dp::CCtrl_3dp()
{
	TotalCnt=0;//總數
	p_M3dp=NULL;
	f3dp=NULL;
#ifdef _DEBUG
	PicFreeCount = 0;
	PicAlocCount = 0;
#endif
}

CCtrl_3dp::~CCtrl_3dp()
{
#ifdef _DEBUG
#ifdef RINTFMEMORYUSED_D
	FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
	if(pfile)
	{
		fprintf(pfile,"3DP_MemUesd %d\n", PicAlocCount - PicFreeCount);
		fclose(pfile);
	}
#endif
#endif
	CloseClass();
}

nuBOOL CCtrl_3dp::InitClass()
{
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	return nuTRUE;
}
nuVOID CCtrl_3dp::CloseClass()
{
	ReleaseFile();
}

nuBOOL CCtrl_3dp::ReadFile()
{
	ReleaseFile();
	f3dp=nuTfopen(FName, NURO_FS_RB);
	if(f3dp==NULL){
		return nuFALSE;
	}
	if(0!=nuFseek(f3dp,0,NURO_SEEK_SET)){
		ReleaseFile();
		return nuFALSE;
	}
	if(1!=nuFread(&TotalCnt,sizeof(nuLONG),1,f3dp)){
		ReleaseFile();
		return nuFALSE;
	}
	p_M3dp=(StruMain3dp**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_StruMain3dp)*TotalCnt);
	if(p_M3dp==NULL){
		ReleaseFile();
		return nuFALSE;
	}
#ifdef _DEBUG
	PicAlocCount++;
#endif
	if(TotalCnt!=nuFread(*p_M3dp,sizeof(Tag_StruMain3dp),TotalCnt,f3dp)){
		ReleaseFile();
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID CCtrl_3dp::ReleaseFile()
{
	if(p_M3dp)
	{
#ifdef _DEBUG
	PicFreeCount++;
#endif	
		g_pRtMMApi->MM_FreeMem((nuPVOID*)p_M3dp);	
		p_M3dp=NULL;	
	}
	if(f3dp){	nuFclose(f3dp);		f3dp=NULL;	}
	TotalCnt=0;
}

nuLONG CCtrl_3dp::CheckFNTCoor(NUROPOINT FCoor,NUROPOINT NCoor,NUROPOINT TCoor)
{
	nuULONG i = 0;
	for(i=0;i<TotalCnt;i++)
	{
		if(FCoor.x<=((*p_M3dp)[i].FCoor.x+1) && FCoor.x>=((*p_M3dp)[i].FCoor.x-1))
			if(FCoor.y<=((*p_M3dp)[i].FCoor.y+1) && FCoor.y>=((*p_M3dp)[i].FCoor.y-1))
				if(NCoor.x<=((*p_M3dp)[i].NCoor.x+1) && NCoor.x>=((*p_M3dp)[i].NCoor.x-1))
					if(NCoor.y<=((*p_M3dp)[i].NCoor.y+1) && NCoor.y>=((*p_M3dp)[i].NCoor.y-1))
						if(TCoor.x<=((*p_M3dp)[i].TCoor.x+1) && TCoor.x>=((*p_M3dp)[i].TCoor.x-1))
							if(TCoor.y<=((*p_M3dp)[i].TCoor.y+1) && TCoor.y>=((*p_M3dp)[i].TCoor.y-1))
								return (*p_M3dp)[i].JpgName;
	}
	return 0;
/*
	//先用二分法找到擴張範圍的起始點
	//使用二分法處理
	nuLONG l_Mid,l_High,l_Low;
	l_High = TotalCnt;
	l_Low = -1;
	l_Mid = (l_High+l_Low)/2;
	while(nuTRUE)
	{
		if(l_High-l_Low <= 1) break;

		if(FCoor.x==(*p_M3dp)[l_Mid].FCoor.x)
		{
			if(FCoor.y==(*p_M3dp)[l_Mid].FCoor.y)
			{
				if(NCoor.x==(*p_M3dp)[l_Mid].NCoor.x)
				{
					if(NCoor.y==(*p_M3dp)[l_Mid].NCoor.y)
					{
						if(TCoor.x==(*p_M3dp)[l_Mid].TCoor.x)
						{
							if(TCoor.y==(*p_M3dp)[l_Mid].TCoor.y)
							{	return (*p_M3dp)[l_Mid].JpgName; 	}
							else if(TCoor.y>(*p_M3dp)[l_Mid].TCoor.y)
							{	l_Low=l_Mid;	}
							else
							{	l_High=l_Mid;	}
						}
						else if(TCoor.x>(*p_M3dp)[l_Mid].TCoor.x)
						{	l_Low=l_Mid;	}
						else
						{	l_High=l_Mid;	}
					}
					else if(NCoor.y>(*p_M3dp)[l_Mid].NCoor.y)
					{	l_Low=l_Mid;	}
					else
					{	l_High=l_Mid;	}
				}
				if(NCoor.x>(*p_M3dp)[l_Mid].NCoor.x)
				{	l_Low=l_Mid;	}
				else
				{	l_High=l_Mid;	}
			}
			else if(FCoor.y>(*p_M3dp)[l_Mid].FCoor.y)
			{	l_Low=l_Mid;	}
			else
			{	l_High=l_Mid;	}
		}
		else if(FCoor.x>(*p_M3dp)[l_Mid].FCoor.x)
		{	l_Low=l_Mid;	}
		else
		{	l_High=l_Mid;	}
		l_Mid = (l_High+l_Low)/2;
	}
	return 0;
*/
}
