
#include "stdAfxRoute.h"
	#include "Ctrl_MRT.h"
#ifdef _DEBUG
#include "stdio.h"
#endif
	CMRTCtrl::CMRTCtrl()
	{
	#ifdef _DEBUG
		MRTFreeCount = 0;
		MRTAlocCount = 0;
	#endif
		IniConst();
	}

	CMRTCtrl::~CMRTCtrl()
	{
	#ifdef _DEBUG
	#ifdef RINTFMEMORYUSED_D
		FILE *pfile = fopen("D:\\RouteMapTest\\Mem.txt", "a");
		if(pfile)
		{
			fprintf(pfile,"MRT_MemUesd %d\n", MRTAlocCount - MRTFreeCount);
			fclose(pfile);
		}
	#endif
	#endif
		CloseClass();
	}

	nuBOOL CMRTCtrl::InitClass()
	{
		nuMemset(FName, 0, sizeof(nuTCHAR) * NURO_MAX_PATH);
		return nuTRUE;
	}
	nuVOID CMRTCtrl::CloseClass()
	{
		ReSet();
	}

	nuBOOL CMRTCtrl::Read_Header(nuLONG StartAddr)
	{
		RtBFile=nuTfopen(FName,NURO_FS_RB);
		if(RtBFile==NULL)	return nuFALSE;
		if(0!=nuFseek(RtBFile,StartAddr,NURO_SEEK_SET))
		{	Remove_Header();	return nuFALSE;	}
		if(1!=nuFread(&(RtBHeader),sizeof(MRTData_Header_Stru),1,RtBFile))
		{	Remove_Header();	return nuFALSE;	}
		//育賢修正變數共用問題
		//RtBHeader.TotalRoadCount=RtBHeader.RtConnectStruCount;

		nuFclose(RtBFile);
		RtBFile=NULL;
		ReadHeaderOK=nuTRUE;
		m_StartAddr=StartAddr;
		return nuTRUE;
	}
	nuVOID CMRTCtrl::Remove_Header()
	{
		RtBHeader.RtConnectStruStartAddr=-1;
		RtBHeader.RtIndexStruStartAddr=-1;
		RtBHeader.RtConnectStruCount=-1;
		RtBHeader.TotalNodeCount=-1;
		RtBHeader.TotalRoadCount=-1;
		ReadHeaderOK=nuFALSE;
		m_StartAddr=0;
		if(RtBFile!=NULL){	nuFclose(RtBFile);	RtBFile=NULL;	}
	}

	nuBOOL CMRTCtrl::Read_MainData()
	{
		if(ReadHeaderOK==nuFALSE) return nuFALSE;
		RtBMain=(MRTData_MainData_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_MRTData_MainData_Stru)*RtBHeader.TotalNodeCount);
		if(RtBMain==NULL)
		{	
			return nuFALSE;	
		}
		else
		{
#ifdef _DEBUG
			MRTAlocCount++;
#endif
		}

		RtBFile=nuTfopen(FName,NURO_FS_RB);
		if(RtBFile==NULL)	return nuFALSE;
		if(0!=nuFseek(RtBFile,m_StartAddr+sizeof(MRTData_Header_Stru),NURO_SEEK_SET))
		{	Remove_MainData();	return nuFALSE;	}
		if(RtBHeader.TotalNodeCount!=nuFread(*RtBMain,sizeof(Tag_MRTData_MainData_Stru),RtBHeader.TotalNodeCount,RtBFile))
		{	Remove_MainData();	return nuFALSE;	}

		nuFclose(RtBFile);
		RtBFile=NULL;
		nuSleep(_SLEEPTICKCOUNT);
		return nuTRUE;
	}
	nuVOID CMRTCtrl::Remove_MainData()
	{
		if(RtBMain!=NULL)
		{
		#ifdef _DEBUG
			MRTFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)RtBMain);
			RtBMain=NULL;
		}
	
		if(RtBFile!=NULL){	nuFclose(RtBFile);	RtBFile=NULL;	}
	}

	nuBOOL CMRTCtrl::Read_Connect()//產生聯通資料儲存陣列
	{
		if(ReadHeaderOK==nuFALSE) return nuFALSE;
		RtBConnect=(MRTData_Connect_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_MRTData_Connect_Stru)*RtBHeader.RtConnectStruCount);

		if(RtBConnect==NULL)
		{	
			return nuFALSE;	
		}
		else
		{
		#ifdef _DEBUG
			MRTAlocCount++;
		#endif
		}
		RtBFile=nuTfopen(FName,NURO_FS_RB);
		if(RtBFile==NULL)	return nuFALSE;
		if(0!=nuFseek(RtBFile,RtBHeader.RtConnectStruStartAddr,NURO_SEEK_SET))
		{	Remove_Connect();	return nuFALSE;	}
		if(RtBHeader.RtConnectStruCount!=nuFread(*RtBConnect,sizeof(Tag_MRTData_Connect_Stru),RtBHeader.RtConnectStruCount,RtBFile))
		{	Remove_Connect();	return nuFALSE;	}
		nuFclose(RtBFile);
		RtBFile=NULL;

		nuSleep(_SLEEPTICKCOUNT);
		return nuTRUE;
	}

	nuVOID CMRTCtrl::Remove_Connect()//移除聯通資料儲存陣列
	{
		if(RtBConnect!=NULL)
		{
		#ifdef _DEBUG
			MRTFreeCount++;
		#endif
			g_pRtMMApi->MM_FreeMem((nuPVOID*)RtBConnect);
			RtBConnect=NULL;
		}
	
		if(RtBFile!=NULL){	nuFclose(RtBFile);	RtBFile=NULL;	}
	}

	nuBOOL CMRTCtrl::Read_Index()
	{
		if(ReadHeaderOK==nuFALSE) return nuFALSE;
		RtBIdx=(MRTData_Index_Stru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_MRTData_Index_Stru)*RtBHeader.TotalRoadCount);
		if(RtBIdx==NULL)
		{	
			return nuFALSE;	
		}
		else
		{
		#ifdef _DEBUG
			MRTAlocCount++;
		#endif
		}
		RtBFile=nuTfopen(FName,NURO_FS_RB);
		if(RtBFile==NULL)	return nuFALSE;
		if(0!=nuFseek(RtBFile,RtBHeader.RtIndexStruStartAddr,NURO_SEEK_SET))
		{	Remove_Index();	return nuFALSE;	}
		if(RtBHeader.TotalRoadCount!=nuFread(*RtBIdx,sizeof(MRTData_Index_Stru),RtBHeader.TotalRoadCount,RtBFile))
		{	Remove_Index();	return nuFALSE;	}
		nuFclose(RtBFile);
		RtBFile=NULL;
		nuSleep(_SLEEPTICKCOUNT);

		return nuTRUE;
	}
	nuVOID CMRTCtrl::Remove_Index()
	{
		if(RtBIdx!=NULL)
		{
		#ifdef _DEBUG
			MRTFreeCount++;
		#endif	
			g_pRtMMApi->MM_FreeMem((nuPVOID*)RtBIdx);
			RtBIdx=NULL;
		}
	
		if(RtBFile!=NULL){	nuFclose(RtBFile);	RtBFile=NULL;	}
	}

	nuBOOL CMRTCtrl::ReSet()//移除Rt檔
	{
		Remove_Index();
		Remove_Connect();
		Remove_MainData();
		Remove_Header();
		IniConst();
		return nuTRUE;
	}

	nuVOID CMRTCtrl::IniConst()
	{
		ReadHeaderOK=nuFALSE;
		m_StartAddr=0;
		
		RtBMain=NULL;//Rt中的主資料
		RtBConnect=NULL;//Rt中的連結資料
		RtBIdx=NULL;//Rt中RoadID與NodeID的對應資料
		RtBFile=NULL;
	}
