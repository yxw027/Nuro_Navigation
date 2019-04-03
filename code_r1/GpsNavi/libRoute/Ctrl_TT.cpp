// Ctrl_NT.cpp: implementation of the CCtrl_TT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdAfxRoute.h"
#include "Ctrl_TT.h"
#ifdef _DEBUG
#include "stdio.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtrl_TT::CCtrl_TT()
{
#ifdef _DEBUG
	TTAlocCount = 0;
	TTFreeCount  = 0;
#endif
	InitClass();
}

CCtrl_TT::~CCtrl_TT()
{
	#ifdef _DEBUG
		#ifdef RINTFMEMORYUSED_D
			FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
			if(pfile)
			{
				fprintf(pfile,"TT_MemUesd %d\n", TTAlocCount - TTFreeCount);
				fclose(pfile);
			}
		#endif
	#endif
	CloseClass();
}

nuBOOL CCtrl_TT::InitClass()
{
	InitTTStruct();
	nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
	return nuTRUE;
}
nuVOID CCtrl_TT::CloseClass()
{
	ReleaseStruct();
}


nuBOOL CCtrl_TT::ReadStruct()
{
	TTFile=nuTfopen(FName, NURO_FS_RB);
	if(TTFile==NULL)																	{	ReleaseStruct();	return nuFALSE;	}
	if(NULL != nuFseek(TTFile,0,NURO_SEEK_SET))												{	ReleaseStruct();	return nuFALSE;	}
	if(1!=nuFread(&TotalNoTurnCount,sizeof(nuLONG),1,TTFile))									{	ReleaseStruct();	return nuFALSE;	}
	if(1!=nuFread(&SizeofB,sizeof(nuLONG),1,TTFile))										{	ReleaseStruct();	return nuFALSE;	}

	m_BaseShift=sizeof(nuLONG)*2+sizeof(Tag_Stru_TT_A)*TotalNoTurnCount;

	m_TTItem=(Tag_Stru_TT_A**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_Stru_TT_A)*TotalNoTurnCount);
	if(m_TTItem==NULL)																	{	ReleaseStruct();	return nuFALSE;	}
#ifdef _DEBUG
	TTAlocCount++;
#endif
	m_TTItemB=(nuBYTE**)g_pRtMMApi->MM_AllocMem(SizeofB);
	if(m_TTItemB==NULL)																	{	ReleaseStruct();	return nuFALSE;	}
#ifdef _DEBUG
	TTAlocCount++;
#endif
	if(NULL != nuFseek(TTFile,sizeof(nuLONG)*2,NURO_SEEK_SET))									{	ReleaseStruct();	return nuFALSE;	}
	if(TotalNoTurnCount!=nuFread(*m_TTItem,sizeof(Tag_Stru_TT_A),TotalNoTurnCount,TTFile))		{	ReleaseStruct();	return nuFALSE;	}
	if(SizeofB!=nuFread(*m_TTItemB,1,SizeofB,TTFile))									{	ReleaseStruct();	return nuFALSE;	}
	if(TTFile!=NULL)																	{	nuFclose(TTFile);	TTFile=NULL;	}
	return nuTRUE;
}

nuVOID CCtrl_TT::ReleaseStruct()
{
	if(m_TTItemB!=NULL)
	{
#ifdef _DEBUG
	TTFreeCount++;
#endif	
		g_pRtMMApi->MM_FreeMem((nuPVOID*)m_TTItemB);	m_TTItemB=NULL;	
	}
	if(m_TTItem!=NULL)
	{	
#ifdef _DEBUG
	TTFreeCount++;
#endif
		g_pRtMMApi->MM_FreeMem((nuPVOID*)m_TTItem);	
		m_TTItem=NULL;	
	}

	if(TTFile!=NULL){	nuFclose(TTFile);	TTFile=NULL;	}
	SizeofB=0;
	TotalNoTurnCount=0;
	m_BaseShift=0;
}

nuVOID CCtrl_TT::InitTTStruct()
{
	m_BaseShift=0;
	TotalNoTurnCount=0;
	SizeofB=0;
	m_TTItem=NULL;//Sort by PassNode,StartNode,EndNode
	m_TTItemB=NULL;
	TTFile=NULL;
}


nuDWORD CCtrl_TT::GetData(nuBOOL bCheck,nuDWORD Addr,nuDWORD DataIdx)//DataIdx從0開始
{
	Stru_TT_B TTB = {0};
	nuDWORD addcount = 0;
	nuDWORD CalAddr  = 0;
	addcount=0;
	m_bUSETT = bCheck;
//#ifdef _USETTI
	if(!m_bUSETT)
	{
		TTI_Address = -1;
	}
//#endif
	nuMemcpy(&TTB,*m_TTItemB+(Addr-m_BaseShift),sizeof(Tag_Stru_TT_B));//用來取得最後一筆資料 到時候要用在回推上
	nuMemset(TTIDList, 0, sizeof(Stru_TT_NODEID));
	if(DataIdx<TTB.nCount){
		switch(DataIdx){
			case 0:
				addcount = 0;
				CalAddr  = Addr-m_BaseShift+4;
				nuMemcpy(TTIDList,*m_TTItemB + CalAddr, TTB.nNodeCountList1*sizeof(Stru_TT_NODEID));//用來取得最後一筆資料 到時候要用在回推上
			//#ifdef _USETTI
				if(!m_bUSETT)
				{
				CalAddr += TTB.nNodeCountList1*sizeof(Stru_TT_NODEID);
				nuMemcpy(&TTI_Address,*m_TTItemB + CalAddr ,sizeof(nuLONG));
				}
			//#endif	
				return TTB.nNodeCountList1;
			case 1:
				addcount = TTB.nNodeCountList1;
				CalAddr  = Addr-m_BaseShift+4+addcount*sizeof(Stru_TT_NODEID);
			//#ifdef _USETTI
				if(!m_bUSETT)
				{
				CalAddr += sizeof(nuLONG);//Add TTI address, 4byte
				}
			//#endif	
				nuMemcpy(TTIDList,*m_TTItemB + CalAddr, TTB.nNodeCountList2*sizeof(Stru_TT_NODEID));//用來取得最後一筆資料 到時候要用在回推上
			//#ifdef _USETTI	
				if(!m_bUSETT)
				{
				CalAddr += TTB.nNodeCountList2*sizeof(Stru_TT_NODEID);
				nuMemcpy(&TTI_Address,*m_TTItemB + CalAddr, sizeof(nuLONG));
				}
			//#endif	
				return TTB.nNodeCountList2;
			case 2:
				addcount = TTB.nNodeCountList1+TTB.nNodeCountList2;
				CalAddr  = Addr-m_BaseShift+4+addcount*sizeof(Stru_TT_NODEID);
			//#ifdef _USETTI
				if(!m_bUSETT)
				{
				CalAddr += 2 * sizeof(nuLONG);//Add TTI address, 4byte
				}
			//#endif
				nuMemcpy(TTIDList,*m_TTItemB + CalAddr,TTB.nNodeCountList3*sizeof(Stru_TT_NODEID));//用來取得最後一筆資料 到時候要用在回推上
			//#ifdef _USETTI
				if(!m_bUSETT)
				{
				CalAddr += TTB.nNodeCountList3*sizeof(Stru_TT_NODEID);
				nuMemcpy(&TTI_Address,*m_TTItemB + CalAddr, sizeof(nuLONG));
				}
			//#endif	
				return TTB.nNodeCountList3;
			case 3:
				addcount = TTB.nNodeCountList1+TTB.nNodeCountList2+TTB.nNodeCountList3;
				CalAddr  = Addr-m_BaseShift+4+addcount*sizeof(Stru_TT_NODEID);
			//#ifdef _USETTI
				if(!m_bUSETT)
				{
				CalAddr += 3 * sizeof(nuLONG);//Add TTI address, 4byte
				}
			//#endif
				nuMemcpy(TTIDList,*m_TTItemB + CalAddr, TTB.nNodeCountList4*sizeof(Stru_TT_NODEID));//用來取得最後一筆資料 到時候要用在回推上
			//#ifdef _USETTI
				if(!m_bUSETT)
				{
				CalAddr += TTB.nNodeCountList4*sizeof(Stru_TT_NODEID);
				nuMemcpy(&TTI_Address,*m_TTItemB + CalAddr , sizeof(nuLONG));
				}
			//#endif	
				return TTB.nNodeCountList4;
			case 4:
				addcount = TTB.nNodeCountList1+TTB.nNodeCountList2+TTB.nNodeCountList3+TTB.nNodeCountList4;
				CalAddr  = Addr-m_BaseShift+4+addcount*sizeof(Stru_TT_NODEID);
			//#ifdef _USETTI
				if(!m_bUSETT)
				{
				CalAddr += 4 * sizeof(nuLONG);//Add TTI address, 4byte
				}
			//#endif
				nuMemcpy(TTIDList,*m_TTItemB + CalAddr, TTB.nNodeCountList5*sizeof(Stru_TT_NODEID));//用來取得最後一筆資料 到時候要用在回推上
			//#ifdef _USETTI
				if(!m_bUSETT)
				{
				CalAddr += TTB.nNodeCountList5*sizeof(Stru_TT_NODEID);
				nuMemcpy(&TTI_Address,*m_TTItemB + CalAddr, sizeof(nuLONG));
				}
			//#endif	
				return TTB.nNodeCountList5;
			case 5:
				addcount = TTB.nNodeCountList1+TTB.nNodeCountList2+TTB.nNodeCountList3+TTB.nNodeCountList4+TTB.nNodeCountList5;
				CalAddr  = Addr-m_BaseShift+4+addcount*sizeof(Stru_TT_NODEID);
			//#ifdef _USETTI
				if(!m_bUSETT)
				{
				CalAddr += 5 * sizeof(nuLONG);//Add TTI address, 4byte
				}
			//#endif
				nuMemcpy(TTIDList,*m_TTItemB + CalAddr, TTB.nNodeCountList6*sizeof(Stru_TT_NODEID));//用來取得最後一筆資料 到時候要用在回推上
			//#ifdef _USETTI	
				if(!m_bUSETT)
				{
				CalAddr += TTB.nNodeCountList6*sizeof(Stru_TT_NODEID);
				nuMemcpy(&TTI_Address,*m_TTItemB + CalAddr, sizeof(nuLONG));
				}
			//#endif	
				return TTB.nNodeCountList6;
			case 6:
				addcount=TTB.nNodeCountList1+TTB.nNodeCountList2+TTB.nNodeCountList3+TTB.nNodeCountList4+TTB.nNodeCountList5+TTB.nNodeCountList6;
				CalAddr =Addr-m_BaseShift+4+addcount*sizeof(Stru_TT_NODEID);
			//#ifdef _USETTI
				if(!m_bUSETT)
				{
				CalAddr += 6 * sizeof(nuLONG);//Add TTI address, 4byte
				}
			//#endif
				nuMemcpy(TTIDList,*m_TTItemB + CalAddr ,TTB.nNodeCountList7*sizeof(Stru_TT_NODEID));//用來取得最後一筆資料 到時候要用在回推上

			//#ifdef _USETTI
				if(!m_bUSETT)
				{
				CalAddr += TTB.nNodeCountList7*sizeof(Stru_TT_NODEID);
				nuMemcpy(&TTI_Address,*m_TTItemB + CalAddr ,sizeof(nuLONG));
				}
			//#endif	
				return TTB.nNodeCountList7;
			default:
				return 0;
		}
	}
	else{
		return 0;
	}
}
