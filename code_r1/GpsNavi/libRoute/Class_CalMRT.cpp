
#include "stdAfxRoute.h"
#include "Class_CalMRT.h"
#include "Ctrl_RuleSet.h"
#include "CRoutingDataCtrl.h"
#include "AllSystemDataBase.h"

//#include "AllSystemDataBase.h"

Class_CalMRT::Class_CalMRT()
{
	ClassWeighting=0;//等級權值(分子)
	AveLatitude=0;
	MRTGDis=0;
	MRTRDis=0;
	TargetClass=0;
}

Class_CalMRT::~Class_CalMRT()
{
	ClassWeighting=0;//等級權值(分子)
	AveLatitude=0;
	MRTGDis=0;
	MRTRDis=0;
	TargetClass=0;
	Tmp_Approach.ReleaseApproach();//Astar規畫完之後留下來的資料(approach結束)
}

nuBOOL Class_CalMRT::AStar(AStarWork &m_AStar, nuULONG FMRTNodeID, nuULONG TMRTNodeID)//規劃
{
	LpAVLTree p_SAVLTree = NULL;
	AStarData m_AStarData = {0}, tmp_SAVLData = {0};
	NUROPOINT CLRS_F,CLRS_T = {0};
	nuLONG tmp_GuessDis = 0;
	WEIGHTINGSTRU BestChoose = {0}, NewWeighting = {0};
	nuULONG LimitNodeID_6 = -1;
	
	CLRS_F=(*m_mrt.RtBMain)[FMRTNodeID].NodeCoor;
	CLRS_T=(*m_mrt.RtBMain)[TMRTNodeID].NodeCoor;
	
	AveLatitude=nulCos(NURO_ABS((CLRS_T.y + CLRS_F.y) / 2) / _EACHDEGREECOORSIZE) * 1113 / _LONLATBASE;
/*
	if(!FindNearEnd6Net(m_AStar, CLRS_T, Dis_Rd6,TMRTNodeID,LimitNodeID_6, Class6_pass))//有必要才加上去
	{//如果從終點接回去沒有資料表示錯誤，表示並無連接終點的道路
		return nuFALSE;
	}
*/
	// 初始化AStar暫存資料結構
	m_AStar.IniData();
	//起點兩端NodeID相同(在端點上)	則直接由這一點擴散
	//起點兩端NodeID不同(在路上)	則往兩端通行均可
	m_AStarData.FMapID = 0;	m_AStarData.FRtBID = 0;	m_AStarData.FNodeID = FMRTNodeID;
	m_AStarData.NMapID = 0;	m_AStarData.NRtBID = 0;	m_AStarData.NNodeID = FMRTNodeID;
	m_AStarData.TMapID = 0;	m_AStarData.TRtBID = 0;	m_AStarData.TNodeID = FMRTNodeID;
	m_AStarData.FMapLayer = m_AStarData.NMapLayer = m_AStarData.TMapLayer   = _MAPLAYER_SRT;
	m_AStarData.Class		= _GENERALRDLEVEL;
	m_AStarData.FNoTurnFlag = 0;
	m_AStarData.NNoTurnFlag = 0;
	m_AStarData.TNoTurnFlag = 0;
	m_AStarData.RDis = 0;

	tmp_GuessDis=UONEGetGuessDistance((*m_mrt.RtBMain)[m_AStarData.TNodeID].NodeCoor,CLRS_T);
	
	NewWeighting.RealDis=0;
	m_AStarData.NowDis=0;
	m_AStarData.RDis=0;
	NewWeighting.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
	m_AStarData.FNoTurnFlag=0;
	m_AStarData.NNoTurnFlag=0;
	m_AStarData.TNoTurnFlag=0;
	m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,NewWeighting);
	
	//加入第一筆的技巧很重要!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//明天修正
	//加入第一筆資料進入open端
	while(nuTRUE)
	{
		if(b_StopRoute) return nuFALSE;
		//取得最優Open端資料 填入Close端
		if(!m_AStar.GetBestOpenToClose(p_SAVLTree,BestChoose)){
			MRTGDis=-1;
			MRTRDis=-1;
			TargetClass=-1;
			return nuFALSE;
		}

		if( p_SAVLTree->Data.TNodeID==TMRTNodeID ){//終點碰到就結束	
			nuMemcpy(&m_ApprocahAStarData,&(p_SAVLTree->Data),MappingCheckByte_l);//用來取得最後一筆資料 到時候要用在回推上
			m_ApprocahAStarData.NMapLayer=p_SAVLTree->Data.NMapLayer;
			m_ApprocahAStarData.TMapLayer=p_SAVLTree->Data.TMapLayer;
			MRTGDis=BestChoose.TotalDis;
			MRTRDis=BestChoose.RealDis;
			TargetClass=p_SAVLTree->Data.Class;
			return nuTRUE;
		}
		nuMemcpy(&(tmp_SAVLData),&(p_SAVLTree->Data),sizeof(TagAStarData));
		Astar_SRT(CLRS_T,tmp_SAVLData,m_AStar,FMRTNodeID,TMRTNodeID,BestChoose);
		//如果使用這個方式 則需要資料輔助		Astar_SRT(CLRS_T,p_SAVLTree->Data,m_AStar,FMRTNodeID,TMRTNodeID);
	}
	return nuFALSE;
}

//希望能找出最近的六號路網點(class 0~6)
nuBOOL Class_CalMRT::FindNearEnd6Net(AStarWork &m_AStar, NUROPOINT &CLRS_T, nuLONG &Dis_Rd6, nuULONG &TMRTNodeID, nuULONG &LimitNodeID_6)
{
	LpAVLTree m_SAVLTree = {0};
	AStarData m_AStarData = {0};
	nuULONG i = 0;
	nuBOOL b_GotoNext=nuTRUE;
	m_AStar.IniData();
	nuLONG tmp_GuessDis = 0, NowConnectIdx = 0;
	WEIGHTINGSTRU BestChoose = {0}, NewWeighting = {0};
	
	m_AStarData.FMapID=0;	m_AStarData.FRtBID=0;	m_AStarData.FNodeID=TMRTNodeID;
	m_AStarData.NMapID=0;	m_AStarData.NRtBID=0;	m_AStarData.NNodeID=TMRTNodeID;

	//先加上兩筆資料從終點出發
	for(i=0;i<(*m_mrt.RtBMain)[m_AStarData.NNodeID].ConnectCount;i++)
	{
		NowConnectIdx=(*m_mrt.RtBMain)[m_AStarData.NNodeID].ConnectStruIndex+i;
		m_AStarData.TMapID=0;					
		m_AStarData.TRtBID=0;
		m_AStarData.TNodeID=(*m_mrt.RtBConnect)[NowConnectIdx].TNodeID;
		if(	m_AStarData.FNodeID==m_AStarData.TNodeID ) continue;
		//禁止原路掉頭
		m_AStarData.FMapLayer=m_AStarData.NMapLayer=m_AStarData.TMapLayer = _MAPLAYER_SRT;
		tmp_GuessDis=UONEGetGuessDistance((*m_mrt.RtBMain)[m_AStarData.TNodeID].NodeCoor,CLRS_T);

		NewWeighting.RealDis=(*m_mrt.RtBConnect)[NowConnectIdx].RoadLength;
		m_AStarData.Class=(*m_mrt.RtBConnect)[NowConnectIdx].RoadClass;
		m_AStarData.FNoTurnFlag=1;
		m_AStarData.NNoTurnFlag=1;
		m_AStarData.TNoTurnFlag=1;
		
		if(!GetWeighting(m_AStarData.Class)) continue;
		m_AStarData.NowDis = ((*m_mrt.RtBConnect)[NowConnectIdx].RoadLength*ClassWeighting) / _WEIGHTINGBASE;
		m_AStarData.RDis   = 0;

		NewWeighting.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
		m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,NewWeighting);
	}
	//取得最優Open端資料 填入Close端
	if(!m_AStar.GetBestOpenToClose(m_SAVLTree,BestChoose))
	{
		return nuFALSE;
	}
	if(m_SAVLTree->Data.Class <= 6)
	{
		Dis_Rd6=BestChoose.RealDis;
		LimitNodeID_6=m_SAVLTree->Data.FNodeID;
		return nuTRUE;
	}
	return nuFALSE;
}

nuVOID  Class_CalMRT::Astar_SRT(NUROPOINT TargetCoor, TagAStarData &m_SAVLData, AStarWork &m_AStar, nuULONG FMRTNodeID, nuULONG TMRTNodeID, WEIGHTINGSTRU BestChoose)
{
	nuLONG tmp_GuessDis = 0, NowConnectIdx = 0;
	nuDWORD i = 0;
	AStarData m_AStarData = {0};
	WEIGHTINGSTRU NewWeighting = {0};

	m_AStarData.FMapID=m_SAVLData.NMapID;	m_AStarData.FRtBID=m_SAVLData.NRtBID;	m_AStarData.FNodeID=m_SAVLData.NNodeID;
	m_AStarData.NMapID=m_SAVLData.TMapID;	m_AStarData.NRtBID=m_SAVLData.TRtBID;	m_AStarData.NNodeID=m_SAVLData.TNodeID;
	
	for(i=0;i<(*m_mrt.RtBMain)[m_SAVLData.TNodeID].ConnectCount;i++)
	{
		NowConnectIdx=(*m_mrt.RtBMain)[m_SAVLData.TNodeID].ConnectStruIndex+i;
		m_AStarData.TMapID=0;					
		m_AStarData.TRtBID=0;					
		m_AStarData.TNodeID=(*m_mrt.RtBConnect)[NowConnectIdx].TNodeID;
		if(	m_AStarData.FNodeID==m_AStarData.TNodeID ) continue;
		//禁止原路掉頭
		m_AStarData.FMapLayer = m_AStarData.NMapLayer = m_AStarData.TMapLayer = _MAPLAYER_SRT;
		tmp_GuessDis=UONEGetGuessDistance((*m_mrt.RtBMain)[m_AStarData.TNodeID].NodeCoor,TargetCoor);
//		tmp_GuessDis=0;
		NewWeighting.RealDis=BestChoose.RealDis+(*m_mrt.RtBConnect)[NowConnectIdx].RoadLength;
		m_AStarData.Class=(*m_mrt.RtBConnect)[NowConnectIdx].RoadClass;

//**************************2009.12.30 daniel因應省道增加而修改***********************************************//	
		if((m_SAVLData.Class > 5) && (NewWeighting.RealDis > 500000 && tmp_GuessDis > 500000))//此法是設計起訖點兩端各500KM內才可用省道
		{//起點到終點500Km，終點到起點的500Km裡，才可用省道，如果都超過了500Km了，則須改走更高等級的道路
			continue;			
		}
//**************************2009.12.30 daniel因應省道增加而修改***********************************************//

		m_AStarData.FNoTurnFlag=1;
		m_AStarData.NNoTurnFlag=1;
		m_AStarData.TNoTurnFlag=1;
		//if(m_AStarData.Class==1)		GetWeighting(0);
		//else if(m_AStarData.Class==2)	GetWeighting(4);
		if(!GetWeighting(m_AStarData.Class)) continue;
		m_AStarData.NowDis = ((*m_mrt.RtBConnect)[NowConnectIdx].RoadLength * ClassWeighting) / _WEIGHTINGBASE;
		m_AStarData.RDis=m_AStarData.NowDis+m_SAVLData.RDis+1;
		NewWeighting.TotalDis=tmp_GuessDis+m_AStarData.NowDis+m_AStarData.RDis;
		m_AStar.AddToOpen(m_AStarData,m_AStarData.TMapLayer,NewWeighting);	
	}
}

nuLONG Class_CalMRT::UONEGetGuessDistance(NUROPOINT Pt1, NUROPOINT PT2)//計算預估距離(現在公式與計算距離相同 如果有更好的預估公式 那當然就改變計算的方式)
{
	nuLONG X = 0, Y = 0;
	X = NURO_ABS(Pt1.x - PT2.x);
	Y = NURO_ABS(Pt1.y - PT2.y);

	if(X > 1000000)	
	{
		X = ((X / 100) * AveLatitude / nuGetlBase()) * 100;
	}
	else	
	{
		X = X * AveLatitude / nuGetlBase();
	}

	if(Y > 1000000)
	{
		Y = (Y / 100) * 1109 / 10; // ((Y/100)*1109/1000)*100;
	}
	else		
	{
		Y = Y * 1109 / _LONLATBASE;
	}

	return nulSqrt(X,Y);
/*
	nuLONG X,Y;
	X=NURO_ABS(Pt1.x-PT2.x)*AveLatitude/1000;
	Y=NURO_ABS(Pt1.y-PT2.y)*1109/1000;
	return ((X+Y)*7)/10;
*/
}

nuBOOL Class_CalMRT::GetWeighting(nuLONG Class)
{
	if(g_pRoutingRule->l_TotalSupportCount==0)//使用舊方式
	{
		switch(Class)
		{
			case 0:	ClassWeighting=10;	break;// 110/110
			case 1:	ClassWeighting=14;	break;// 110/80
			case 2:	ClassWeighting=16;	break;// 110/70
			case 3:	ClassWeighting=16;	break;// 110/70
			case 4:	ClassWeighting=22;	break;// 110/50
			case 5:	ClassWeighting=28;	break;// 110/40
			case 6:	ClassWeighting=37;	break;// 110/30
			case 7:	ClassWeighting=37;	break;// 110/30
			case 8:	ClassWeighting=37;	break;// 110/30
			case 9:	ClassWeighting=37;	break;// 110/30
			case 10:	ClassWeighting=37;	break;// 110/30
			default:	ClassWeighting=37;	break;// 110/30
		}
	}
	else
	{
		if(Class>=g_pRoutingRule->l_TotalSupportCount)
		{
			ClassWeighting=*(g_pRoutingRule->l_Weighting)[g_pRoutingRule->l_TotalSupportCount-1];
		}
		else
		{
			ClassWeighting=*(g_pRoutingRule->l_Weighting)[Class];
		}
	}
	if(ClassWeighting==0) return nuFALSE;
	return nuTRUE;
}


nuBOOL Class_CalMRT::GetNodeIDList(AStarWork &m_AStar)
{
	nuULONG i = 0, CheckCount = 0;
	nuULONG TmpCount = 0;
	LpAVLTree p_AVLTree=NULL;
	NodeListStru **TmpList=NULL;

	/*** 初始化存儲結構 ***/
	Tmp_Approach.ReleaseApproach();

	/*** 產生存放NodeList的過程結構 ***/
	TmpCount=0;
	if(m_AStar.lUsedCloseCount>2000){
		CheckCount=1000+m_AStar.lUsedCloseCount/2;
		TmpList=(NodeListStru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_NodeListStru)*CheckCount);
	}
	else{
		CheckCount=m_AStar.lUsedCloseCount+2;
		TmpList=(NodeListStru**)g_pRtMMApi->MM_AllocMem(sizeof(Tag_NodeListStru)*CheckCount);
	}

	if(TmpList==NULL){
		return nuFALSE;
	}

	/*** 取得通往終端Node的資料 ***/
	m_ApprocahAStarData.FNoTurnFlag=1;
	m_ApprocahAStarData.NNoTurnFlag=1;
	m_ApprocahAStarData.TNoTurnFlag=1;
	if(!m_AStar.GetGivenData(m_ApprocahAStarData,p_AVLTree,m_ApprocahAStarData.TMapLayer))
	{	g_pRtMMApi->MM_FreeMem((nuPVOID*)TmpList);	TmpList=NULL;	return nuFALSE;	}

	/*** 補尾的動作處理完了 就要開始整理完整的過程囉 ***/
	(*TmpList)[TmpCount].NodeID=p_AVLTree->Data.TNodeID;	(*TmpList)[TmpCount].RtBID=p_AVLTree->Data.TRtBID;	(*TmpList)[TmpCount].MapID=p_AVLTree->Data.TMapID;	(*TmpList)[TmpCount].MapLayer=p_AVLTree->Data.TMapLayer;
	TmpCount++;
	(*TmpList)[TmpCount].NodeID=p_AVLTree->Data.NNodeID;	(*TmpList)[TmpCount].RtBID=p_AVLTree->Data.NRtBID;	(*TmpList)[TmpCount].MapID=p_AVLTree->Data.NMapID;	(*TmpList)[TmpCount].MapLayer=p_AVLTree->Data.NMapLayer;
	TmpCount++;

	while(!(p_AVLTree->Data.FNodeID==p_AVLTree->Data.NNodeID && p_AVLTree->Data.FRtBID==p_AVLTree->Data.NRtBID && p_AVLTree->Data.FMapID==p_AVLTree->Data.NMapID))
	{
		m_ApprocahAStarData.NMapID=p_AVLTree->Data.FMapID;
		m_ApprocahAStarData.NRtBID=p_AVLTree->Data.FRtBID;				m_ApprocahAStarData.NNodeID=p_AVLTree->Data.FNodeID;
		m_ApprocahAStarData.NMapLayer=p_AVLTree->Data.FMapLayer;
		m_ApprocahAStarData.TMapID=p_AVLTree->Data.NMapID;
		m_ApprocahAStarData.TRtBID=p_AVLTree->Data.NRtBID;				m_ApprocahAStarData.TNodeID=p_AVLTree->Data.NNodeID;
		m_ApprocahAStarData.TMapLayer=p_AVLTree->Data.NMapLayer;
		m_ApprocahAStarData.FNoTurnFlag=1;
		m_ApprocahAStarData.NNoTurnFlag=1;
		m_ApprocahAStarData.TNoTurnFlag=1;

		if(!m_AStar.GetGivenData(m_ApprocahAStarData,p_AVLTree,m_ApprocahAStarData.TMapLayer))
		{
			m_ApprocahAStarData.FNoTurnFlag=0;
			m_ApprocahAStarData.NNoTurnFlag=0;
			m_ApprocahAStarData.TNoTurnFlag=0;
			if(!m_AStar.GetGivenData(m_ApprocahAStarData,p_AVLTree,m_ApprocahAStarData.TMapLayer))
			{
				g_pRtMMApi->MM_FreeMem((nuPVOID*)TmpList);	TmpList=NULL;	return nuFALSE;
			}
		}

		if(CheckCount<=TmpCount){
			return nuFALSE;
		}

		(*TmpList)[TmpCount].NodeID=p_AVLTree->Data.NNodeID;
		(*TmpList)[TmpCount].RtBID=p_AVLTree->Data.NRtBID;
		(*TmpList)[TmpCount].MapID=p_AVLTree->Data.NMapID;
		(*TmpList)[TmpCount].MapLayer=p_AVLTree->Data.NMapLayer;
//		(*TmpList)[TmpCount].RunDis=p_AVLTree->Data.RealDis;
		TmpCount++;
	}

//FILE *fff;
//fff=fopen("c:\\MRTTracelog.txt","wb");
	if(nuFALSE==Tmp_Approach.CreateApproach(TmpCount-1))
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)TmpList);	(*TmpList)=NULL;	return nuFALSE;	}

	for(i=0;i<TmpCount;i++){
		(*Tmp_Approach.p_Approach)[i].FMapID	=	(*TmpList)[TmpCount-i-1].MapID;
		(*Tmp_Approach.p_Approach)[i].FMapLayer	=	(*TmpList)[TmpCount-i-1].MapLayer;
		(*Tmp_Approach.p_Approach)[i].FRtBID	=	(*TmpList)[TmpCount-i-1].RtBID;
		(*Tmp_Approach.p_Approach)[i].FNodeID	=	(*TmpList)[TmpCount-i-1].NodeID;
		(*Tmp_Approach.p_Approach)[i].TMapID	=	(*TmpList)[TmpCount-i-2].MapID;
		(*Tmp_Approach.p_Approach)[i].TMapLayer	=	(*TmpList)[TmpCount-i-2].MapLayer;
		(*Tmp_Approach.p_Approach)[i].TRtBID	=	(*TmpList)[TmpCount-i-2].RtBID;
		(*Tmp_Approach.p_Approach)[i].TNodeID	=	(*TmpList)[TmpCount-i-2].NodeID;

//fprintf(fff,"%ld %ld\n",(*Tmp_Approach.p_Approach)[i].FNodeID,(*Tmp_Approach.p_Approach)[i].TNodeID);
	}
//fclose(fff);

	// 將過程資料移除
	if(TmpList!=NULL)
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)TmpList);
		TmpList=NULL;
	}
	TmpCount=0;
	return nuTRUE;
}

nuBOOL Class_CalMRT::CreateLocalRoutingData(PNAVIRTBLOCKCLASS TmpNaviRtBlockClass)
{
	nuLONG i,TmpRtBCount=0,TmpNSSCount=0;
	TmpRtBCount=0;
	TmpNSSCount=0;
	if(!g_pCRDC->Create_NaviSubClass(TmpNaviRtBlockClass,Tmp_Approach.ApproachCount)){
		g_pCRDC->Release_NaviSubClass(TmpNaviRtBlockClass);
		return nuFALSE;
	}
	if(!g_pCRDC->Create_NodeIDList(TmpNaviRtBlockClass,Tmp_Approach.ApproachCount+1)){
		g_pCRDC->Release_NodeIDList(TmpNaviRtBlockClass);
		return nuFALSE;
	}

	TmpRtBCount=0;
	TmpNSSCount=0;
	//開始填資料
	for(i=0;i<Tmp_Approach.ApproachCount;i++){
		if(i==0){
			TmpNaviRtBlockClass->FInfo.MapID = (*Tmp_Approach.p_Approach)[i].FMapID;
			TmpNaviRtBlockClass->FInfo.RTBID = (*Tmp_Approach.p_Approach)[i].FRtBID;
			TmpNaviRtBlockClass->FInfo.FixNode1 = (*Tmp_Approach.p_Approach)[i].FNodeID;
			TmpNaviRtBlockClass->FInfo.FixNode2 = (*Tmp_Approach.p_Approach)[i].TNodeID;
			TmpNaviRtBlockClass->FInfo.FixCoor.x =	(*m_mrt.RtBMain)[(*Tmp_Approach.p_Approach)[i].FNodeID].NodeCoor.x;
			TmpNaviRtBlockClass->FInfo.FixCoor.y =	(*m_mrt.RtBMain)[(*Tmp_Approach.p_Approach)[i].FNodeID].NodeCoor.y;
			TmpNaviRtBlockClass->FInfo.FixDis = 0;
				
			TmpNSSCount=0;
			TmpNaviRtBlockClass->NodeIDList[TmpNSSCount]=(*Tmp_Approach.p_Approach)[i].FNodeID;
			TmpNSSCount++;
		}
		TmpNaviRtBlockClass->TInfo.MapID = (*Tmp_Approach.p_Approach)[i].TMapID;
		TmpNaviRtBlockClass->TInfo.RTBID = (*Tmp_Approach.p_Approach)[i].TRtBID;
		TmpNaviRtBlockClass->TInfo.FixNode1 = (*Tmp_Approach.p_Approach)[i].TNodeID;
		TmpNaviRtBlockClass->TInfo.FixNode2 = (*Tmp_Approach.p_Approach)[i].FNodeID;
		TmpNaviRtBlockClass->TInfo.FixCoor.x =	(*m_mrt.RtBMain)[(*Tmp_Approach.p_Approach)[i].TNodeID].NodeCoor.x;
		TmpNaviRtBlockClass->TInfo.FixCoor.y =	(*m_mrt.RtBMain)[(*Tmp_Approach.p_Approach)[i].TNodeID].NodeCoor.y;
		TmpNaviRtBlockClass->NodeIDList[TmpNSSCount]=(*Tmp_Approach.p_Approach)[i].TNodeID;
		TmpNSSCount++;
		TmpNaviRtBlockClass->TInfo.FixDis = 0;
	}
	return nuTRUE;
}

nuBOOL Class_CalMRT::LoadCalMRTRoutingData(nuLONG l_RoutingRule)
{
	//掛載RoutingRule資料
	if(!g_pRoutingRule->ReadRoutingRule(l_RoutingRule))
	{
		g_pRoutingRule->ReleaseRoutingRule(); 
		return nuFALSE;
	}

	//掛載MRT資料
	#ifdef DEF_MAKETHRROUTE
		nuTcscpy(m_mrt.FName, MakeThr_MapPath);
		nuTcscat(m_mrt.FName, nuTEXT(".mrt"));
	#else
		nuTcscpy(m_mrt.FName, nuTEXT(".mrt"));
		g_pRtFSApi->FS_FindFileWholePath(-1,m_mrt.FName,NURO_MAX_PATH);
	#endif
	m_mrt.Read_Header(0);
	m_mrt.Read_MainData();
	m_mrt.Read_Connect();
	m_mrt.Read_Index();

	#ifdef DEF_MAKETHRROUTE
		nuTcscpy(m_mdw.FName, MakeThr_MapPath);
		nuTcscat(m_mdw.FName, nuTEXT(".mdw"));
	#else
		nuTcscpy(m_mdw.FName, nuTEXT(".mdw"));
		g_pRtFSApi->FS_FindFileWholePath(-1,m_mdw.FName,NURO_MAX_PATH);
	#endif
	m_mdw.Read_Header();
	for(nuDWORD i=0;i<m_mdw.Header.m_TotalBlockCount;i++)
	{
		m_mdw.Read_BlockData(i);
	}

	return nuTRUE;
}

nuVOID Class_CalMRT::FreeCalMRTRoutingData()
{
	for(nuDWORD i=0;i<m_mdw.Header.m_TotalBlockCount;i++)
	{
		m_mdw.Remove_BlockData(i);
	}
	m_mdw.Remove_Header();

	m_mrt.Remove_Connect();
	m_mrt.Remove_MainData();
	m_mrt.Remove_Header();
	g_pRoutingRule->ReleaseRoutingRule();
}

//計算權值
nuLONG Class_CalMRT::CalMRTRouting_NodeID(nuLONG MRTID_F, nuLONG MRTID_T, nuLONG &m_MRTGDis, nuLONG &m_MRTRDis)
{
	nuULONG FMRTNodeID=0,TMRTNodeID=0;
	AStarWork		m_AStar;//AStar運算 核心物件
	m_AStar.m_AVLTree.CreateBaseAVLTreeArray();
	m_MRTGDis=-1;
	m_MRTRDis=-1;

	if(nuFALSE==AStar(m_AStar,MRTID_F,MRTID_T)){
//		g_pRoutingRule->ReleaseRoutingRule();
		LeaveCalMRTRouting(m_AStar);
		return nuFALSE;
	}

	m_MRTGDis=MRTGDis;
	m_MRTRDis=MRTRDis;

	LeaveCalMRTRouting(m_AStar);//這個class要讓外界承接 內部則不進行掌控
	return ROUTE_SUCESS;
}


nuLONG Class_CalMRT::NewCalMRTRouting_NodeID(nuLONG MRTID_F, nuLONG MRTID_T, THCROUTEDATA &stuThcRouteData)
{
	nuLONG  i = 0;
	nuDWORD j = 0;
	nuULONG FMRTNodeID=0,TMRTNodeID=0;
	nuUSHORT TmpBlockIdx = 0, TmpRoadIdx = 0, SaveRTBIdx = 0, SaveMapIdx = 0;
	AStarWork		m_AStar;//AStar運算 核心物件
	m_AStar.m_AVLTree.CreateBaseAVLTreeArray();
	if(nuFALSE==AStar(m_AStar,MRTID_F,MRTID_T))
	{
		LeaveCalMRTRouting(m_AStar);
		return nuFALSE;
	}
	if(!GetNodeIDList(m_AStar)){	LeaveCalMRTRouting(m_AStar);	return ROUTE_MRT_APPROACHFALSE;		}

	SaveMapIdx = _MAXUNSHORT;
	SaveRTBIdx = _MAXUNSHORT;
	stuThcRouteData.PassRTBCount=0;
	stuThcRouteData.stuPassRTBData[stuThcRouteData.PassRTBCount].MapID=0;
	stuThcRouteData.stuPassRTBData[stuThcRouteData.PassRTBCount].RTBID=0;
	stuThcRouteData.stuPassRTBData[stuThcRouteData.PassRTBCount].MRTNODEID=(*Tmp_Approach.p_Approach)[0].FNodeID;
	stuThcRouteData.PassRTBCount++;
	for(i=1;i<Tmp_Approach.ApproachCount;i++)
	{
		TmpBlockIdx = _MAXUNSHORT;
		TmpRoadIdx  = _MAXUNSHORT;
		GetBlockRoadIdx(&TmpBlockIdx,&TmpRoadIdx,(*Tmp_Approach.p_Approach)[i].FNodeID,(*Tmp_Approach.p_Approach)[i].TNodeID);

		for(j=0;j<m_mdw.Header.m_TotalBlockCount;j++){
			if(TmpBlockIdx==(*m_mdw.Header.p_GBlockID)[j]){
				if(SaveRTBIdx==(*(*m_mdw.Rd)[j].p_RdData)[TmpRoadIdx].RTBID && SaveMapIdx==(*(*m_mdw.Rd)[j].p_RdData)[TmpRoadIdx].MapID)
				{
					stuThcRouteData.stuPassRTBData[stuThcRouteData.PassRTBCount-1].MRTNODEID=(*Tmp_Approach.p_Approach)[i].TNodeID;
				}
				else
				{
					SaveRTBIdx=(*(*m_mdw.Rd)[j].p_RdData)[TmpRoadIdx].RTBID;
					SaveMapIdx=(*(*m_mdw.Rd)[j].p_RdData)[TmpRoadIdx].MapID;
					stuThcRouteData.stuPassRTBData[stuThcRouteData.PassRTBCount].MapID=(*(*m_mdw.Rd)[j].p_RdData)[TmpRoadIdx].MapID;
					stuThcRouteData.stuPassRTBData[stuThcRouteData.PassRTBCount].RTBID=(*(*m_mdw.Rd)[j].p_RdData)[TmpRoadIdx].RTBID;
					stuThcRouteData.stuPassRTBData[stuThcRouteData.PassRTBCount].MRTNODEID=(*Tmp_Approach.p_Approach)[i].TNodeID;
					stuThcRouteData.PassRTBCount++;
					/*if(stuThcRouteData.PassRTBCount >= 256)
					{
						stuThcRouteData.PassRTBCount=0;
						LeaveCalMRTRouting(m_AStar);
						return nuFALSE;
					}*/
				}
			}
		}
	}
	stuThcRouteData.RealDis		=	MRTRDis;
	stuThcRouteData.WeightDis	=	MRTGDis;
	LeaveCalMRTRouting(m_AStar);
	return ROUTE_SUCESS;
}


nuLONG Class_CalMRT::CalMRTRouting_Coor(NUROPOINT PT_F, NUROPOINT PT_T, nuLONG &m_MRTGDis, nuLONG &m_MRTRDis)
{
	nuULONG FMRTNodeID=900000000,TMRTNodeID=900000000, i = 0;
	nuLONG m_Dif_X = 0, m_Dif_Y = 0;
	AStarWork		m_AStar;//AStar運算 核心物件
	m_AStar.m_AVLTree.CreateBaseAVLTreeArray();
	m_MRTGDis=-1;
	m_MRTRDis=-1;

	//由座標比對出MRTID
	for(i=0;i<m_mrt.RtBHeader.TotalNodeCount;i++){
		m_Dif_X=NURO_ABS((*m_mrt.RtBMain)[i].NodeCoor.x-PT_F.x);
		m_Dif_Y=NURO_ABS((*m_mrt.RtBMain)[i].NodeCoor.y-PT_F.y);
		if( 3>(m_Dif_X+m_Dif_Y) ){
			FMRTNodeID=i;
			break;
		}
	}

	for(i=0;i<m_mrt.RtBHeader.TotalNodeCount;i++){
		m_Dif_X=NURO_ABS((*m_mrt.RtBMain)[i].NodeCoor.x-PT_T.x);
		m_Dif_Y=NURO_ABS((*m_mrt.RtBMain)[i].NodeCoor.y-PT_T.y);
		if( 3>(m_Dif_X+m_Dif_Y) ){
			TMRTNodeID=i;
			break;
		}
	}

	if(FMRTNodeID==900000000 || TMRTNodeID==900000000){
		LeaveCalMRTRouting(m_AStar);
		return nuFALSE;
	}
	if(nuFALSE==AStar(m_AStar,FMRTNodeID,TMRTNodeID)){
		g_pRoutingRule->ReleaseRoutingRule();
		LeaveCalMRTRouting(m_AStar);
		return nuFALSE;
	}

	m_MRTGDis=MRTGDis;
	m_MRTRDis=MRTRDis;
	LeaveCalMRTRouting(m_AStar);//這個class要讓外界承接 內部則不進行掌控

	return ROUTE_SUCESS;
}

nuVOID Class_CalMRT::LeaveCalMRTRouting(AStarWork &m_AStar)
{
	Tmp_Approach.ReleaseApproach();
	m_AStar.IniData();
	m_AStar.m_AVLTree.RemoveBaseAVLTreeArray();//071015 Louis降低記憶體用量
}


nuLONG Class_CalMRT::MRTRouting(PNAVIRTBLOCKCLASS pNRBC, nuLONG MRTID_F, nuLONG MRTID_T, nuLONG l_RoutingRule)
{
	#ifdef	DEFNEWTHCFORMAT
		AStarWork		m_AStar;//AStar運算 核心物件

		if(MRTID_T<2)
		{
			return ROUTE_SUCESS;
		}

		//取得mrt完整路徑
		#ifdef DEF_MAKETHRROUTE
			nuTcscpy(m_mrt.FName, MakeThr_MapPath);
			nuTcscat(m_mrt.FName, nuTEXT(".mrt"));
		#else
			nuTcscpy(m_mrt.FName, nuTEXT(".mrt"));
			g_pRtFSApi->FS_FindFileWholePath(-1,m_mrt.FName,NURO_MAX_PATH);
		#endif

		//取得MRT資料
		if(!m_mrt.Read_Header(0)){	LeaveMRTRouting(m_AStar);	return ROUTE_READMRTFALSE;	}
		if(!m_mrt.Read_MainData()){	LeaveMRTRouting(m_AStar);	return ROUTE_READMRTFALSE;	}

		g_pRtMMApi->MM_CollectDataMem(1);
		ppclsThc=(CCtrl_Thc**)g_pRtMMApi->MM_AllocMem(sizeof(CCtrl_Thc));

		(*ppclsThc)->InitClass(l_RoutingRule);
		(*ppclsThc)->Read();
		(*ppclsThc)->ReadViaNodeData(MRTID_F,MRTID_T);

//		ppclsGThr=(Class_ThrCtrl**)g_pRtMMApi->MM_AllocMem(sizeof(Class_ThrCtrl));

		nuLONG i = 0, tmpmapid = 0;
		PNAVIRTBLOCKCLASS pSaveNaviRTBClass = NULL, pLastSaveNaviRTBClass = NULL, pTmpNaviRTBClass = NULL;
		tmpmapid=-1;
		pSaveNaviRTBClass=NULL;
		pLastSaveNaviRTBClass=NULL;
		pTmpNaviRTBClass=NULL;

		for(i=1;i<MRTID_T;i++)
		{
			if(pSaveNaviRTBClass==NULL)
			{
				pTmpNaviRTBClass=NULL;
				g_pCRDC->Create_NaviRtBlockClass(pTmpNaviRTBClass);
				pSaveNaviRTBClass=pTmpNaviRTBClass;
				pLastSaveNaviRTBClass=pTmpNaviRTBClass;
			}
			else
			{
				pTmpNaviRTBClass=NULL;
				g_pCRDC->Create_NaviRtBlockClass(pTmpNaviRTBClass);
				pLastSaveNaviRTBClass->NextRTB=pTmpNaviRTBClass;
				pLastSaveNaviRTBClass=pTmpNaviRTBClass;
			}
			pLastSaveNaviRTBClass->FInfo.MapID=(*(*ppclsThc)->ppstuViaNodeData)[i].MAPID;
			pLastSaveNaviRTBClass->TInfo.MapID=(*(*ppclsThc)->ppstuViaNodeData)[i].MAPID;
			pLastSaveNaviRTBClass->FInfo.RTBID=(*(*ppclsThc)->ppstuViaNodeData)[i].RTBID;
			pLastSaveNaviRTBClass->TInfo.RTBID=(*(*ppclsThc)->ppstuViaNodeData)[i].RTBID;
			pLastSaveNaviRTBClass->FInfo.FixCoor=(*m_mrt.RtBMain)[(*(*ppclsThc)->ppstuViaNodeData)[i-1].MRTNODEID].NodeCoor;
			pLastSaveNaviRTBClass->TInfo.FixCoor=(*m_mrt.RtBMain)[(*(*ppclsThc)->ppstuViaNodeData)[i].MRTNODEID].NodeCoor;
/*
			if((*(*ppclsThc)->ppstuViaNodeData)[i].MAPID!=tmpmapid)
			{
				tmpmapid=(*(*ppclsThc)->ppstuViaNodeData)[i].MAPID;
				for(j=0;j<m_MapIDtoMapIdx.MapCount;j++)
				{
					if( (*m_MapIDtoMapIdx.MapIDList)[j]==tmpmapid )
					{
						(*ppclsGThr)->ReSet();
						(*ppclsGThr)->InitClass(j,l_RoutingRule);
						(*ppclsGThr)->Read_Header();
						break;
					}
				}
			}

			for(j=0;j<(*ppclsGThr)->pThrEachMap.ThrHeader.TotalNodeCount;j++)
			{
				if(CheckPtCloseLU(pLastSaveNaviRTBClass->FInfo.FixCoor,(*(*ppclsGThr)->pThrEachMap.ThrMain)[j].NodeCoor,5))
				{
					for(k=0;k<(*(*ppclsGThr)->pThrEachMap.ThrMain)[j].ConnectCount;k++)
					{
						connectidx=k+(*(*ppclsGThr)->pThrEachMap.ThrMain)[j].ConnectStruIndex;
						if(	(*(*ppclsGThr)->pThrEachMap.ThrConnect)[connectidx].TMapLayer==2 && CheckPtCloseLU((*(*ppclsGThr)->pThrEachMap.ThrMain)[(*(*ppclsGThr)->pThrEachMap.ThrConnect)[connectidx].TNodeID].NodeCoor,pLastSaveNaviRTBClass->TInfo.FixCoor,5)	)
						{
								pLastSaveNaviRTBClass->RealDis=(*(*ppclsGThr)->pThrEachMap.ThrConnect)[connectidx].Length;
								break;
						}
					}
					break;
				}
			}
*/
			pLastSaveNaviRTBClass->NSSCount=1;
			g_pCRDC->Create_NaviSubClass(pLastSaveNaviRTBClass,pLastSaveNaviRTBClass->NSSCount);
			pLastSaveNaviRTBClass->NSS[0].ArcPtCount=2;
			g_pCRDC->Create_ArcPt(pLastSaveNaviRTBClass->NSS[0],pLastSaveNaviRTBClass->NSS[0].ArcPtCount);
			pLastSaveNaviRTBClass->NSS[0].ArcPt[0]=pLastSaveNaviRTBClass->FInfo.FixCoor;
			pLastSaveNaviRTBClass->NSS[0].ArcPt[1]=pLastSaveNaviRTBClass->TInfo.FixCoor;
			pLastSaveNaviRTBClass->NSS[0].RoadLength=pLastSaveNaviRTBClass->RealDis;
			pLastSaveNaviRTBClass->NSS[0].CityID=0;
			pLastSaveNaviRTBClass->NSS[0].TownID=0;
			pLastSaveNaviRTBClass->NSS[0].RoadNameAddr=0;
			pLastSaveNaviRTBClass->NSS[0].StartCoor=pLastSaveNaviRTBClass->FInfo.FixCoor;
			pLastSaveNaviRTBClass->NSS[0].EndCoor=pLastSaveNaviRTBClass->TInfo.FixCoor;
			
			if(pLastSaveNaviRTBClass->NSS[0].ArcPt[0].x>pLastSaveNaviRTBClass->NSS[0].ArcPt[1].x)
			{
				pLastSaveNaviRTBClass->NSS[0].Bound.left=pLastSaveNaviRTBClass->NSS[0].ArcPt[1].x;
				pLastSaveNaviRTBClass->NSS[0].Bound.right=pLastSaveNaviRTBClass->NSS[0].ArcPt[0].x;
			}
			else
			{
				pLastSaveNaviRTBClass->NSS[0].Bound.left=pLastSaveNaviRTBClass->NSS[0].ArcPt[0].x;
				pLastSaveNaviRTBClass->NSS[0].Bound.right=pLastSaveNaviRTBClass->NSS[0].ArcPt[1].x;
			}
			if(pLastSaveNaviRTBClass->NSS[0].ArcPt[0].y>pLastSaveNaviRTBClass->NSS[0].ArcPt[1].y)
			{
				pLastSaveNaviRTBClass->NSS[0].Bound.top=pLastSaveNaviRTBClass->NSS[0].ArcPt[1].y;
				pLastSaveNaviRTBClass->NSS[0].Bound.bottom=pLastSaveNaviRTBClass->NSS[0].ArcPt[0].y;
			}
			else
			{
				pLastSaveNaviRTBClass->NSS[0].Bound.top=pLastSaveNaviRTBClass->NSS[0].ArcPt[0].y;
				pLastSaveNaviRTBClass->NSS[0].Bound.bottom=pLastSaveNaviRTBClass->NSS[0].ArcPt[1].y;
			}
		}
		pTmpNaviRTBClass=pNRBC->NextRTB;
		pNRBC->NextRTB=pSaveNaviRTBClass;
		pLastSaveNaviRTBClass->NextRTB=pTmpNaviRTBClass;
//		(*ppclsGThr)->ReSet();
//		g_pRtMMApi->MM_FreeMem((nuPVOID*)ppclsGThr);
		LeaveMRTRouting(m_AStar);//這個class要讓外界承接 內部則不進行掌控
		return ROUTE_SUCESS;
	#else
		AStarWork		m_AStar;//AStar運算 核心物件
		if(!m_AStar.m_AVLTree.CreateBaseAVLTreeArray()){	return ROUTE_NEWASTARSTRUCTFALSE;	}
		//一開始進來 結構應該是已經存在的 但結構會隨規劃的結果而被改變 有可能會變成一個串列
		if(pNRBC==NULL){	LeaveMRTRouting(m_AStar);	return ROUTE_ERROR;	}

		//取得mrt完整路徑
		#ifdef DEF_MAKETHRROUTE
			nuTcscpy(m_mrt.FName, MakeThr_MapPath);
			nuTcscat(m_mrt.FName, nuTEXT(".mrt"));
		#else
			nuTcscpy(m_mrt.FName, nuTEXT(".mrt"));
			g_pRtFSApi->FS_FindFileWholePath(-1,m_mrt.FName,NURO_MAX_PATH);
		#endif

		//取得MRT資料
		if(!m_mrt.Read_Header(0)){	LeaveMRTRouting(m_AStar);	return ROUTE_READMRTFALSE;	}
		if(!m_mrt.Read_MainData()){	LeaveMRTRouting(m_AStar);	return ROUTE_READMRTFALSE;	}
		if(!m_mrt.Read_Connect()){	LeaveMRTRouting(m_AStar);	return ROUTE_READMRTFALSE;	}

		//計算資料
		if(nuFALSE==AStar(m_AStar,MRTID_F,MRTID_T)){	LeaveMRTRouting(m_AStar);	return ROUTE_MRT_ASTARFALSE;		}

		if(!GetNodeIDList(m_AStar)){	LeaveMRTRouting(m_AStar);	return ROUTE_MRT_APPROACHFALSE;		}
		//不要管中間的岔路作為
		if(!CreateLocalRoutingData(pNRBC)){	LeaveMRTRouting(m_AStar);	return ROUTE_MRT_CREATEROUTEDATAFASLE;	}

		if(!LocalAStarSuccessWorkStep1(pNRBC)){	LeaveMRTRouting(m_AStar);	return ROUTE_ERROR;		}
		if(!LocalAStarSuccessWorkStep2(pNRBC)){	LeaveMRTRouting(m_AStar);	return ROUTE_ERROR;		}
		if(!LocalAStarSuccessWorkStep3(pNRBC)){	LeaveMRTRouting(m_AStar);	return ROUTE_ERROR;		}

		//最後要留下pNFTC的資訊 其他的資料要完全清空 恢復原貌
		LeaveMRTRouting(m_AStar);//這個class要讓外界承接 內部則不進行掌控
		return ROUTE_SUCESS;
	#endif
}

nuVOID Class_CalMRT::LeaveMRTRouting(AStarWork &m_AStar)
{
	if(ppclsThc)
	{
		(*ppclsThc)->Remove();
		g_pRtMMApi->MM_FreeMem((nuPVOID*)ppclsThc);
		ppclsThc=NULL;
	}

	//remove MRT data
	m_mrt.Remove_Connect();
	m_mrt.Remove_MainData();
	m_mrt.Remove_Header();

	Tmp_Approach.ReleaseApproach();
	m_AStar.IniData();
	m_AStar.m_AVLTree.RemoveBaseAVLTreeArray();//071015 Louis降低記憶體用量
}

nuBOOL Class_CalMRT::LocalAStarSuccessWorkStep1(PNAVIRTBLOCKCLASS pNRBC)//AStar規劃成功的動作
{
	nuULONG i = 0, j = 0;
	nuLONG TmpConnectIdx = 0;
	nuUCHAR WeightingClass = {0};
	//取得路段起訖Node座標 並取得ToNode的叉路數(不包含起訖這一段路)
	for(i=0;i<pNRBC->NSSCount;i++){
		pNRBC->NSS[i].StartCoor.x=(*m_mrt.RtBMain)[pNRBC->NodeIDList[i]].NodeCoor.x;
		pNRBC->NSS[i].StartCoor.y=(*m_mrt.RtBMain)[pNRBC->NodeIDList[i]].NodeCoor.y;
		pNRBC->NSS[i].EndCoor.x=(*m_mrt.RtBMain)[pNRBC->NodeIDList[i+1]].NodeCoor.x;
		pNRBC->NSS[i].EndCoor.y=(*m_mrt.RtBMain)[pNRBC->NodeIDList[i+1]].NodeCoor.y;
		g_pCRDC->Create_CrossData(pNRBC->NSS[i],0);
	}
		
	//並蒐集行徑路段及交叉路口的長度和型態
	for(i=0;i<pNRBC->NSSCount;i++)
	{
		WeightingClass			 = 0;
		pNRBC->NSS[i].RoadLength = _ROUTEDATABASERDLEN;
		//取得有效的(起訖點相符)最短的行徑資料
		for(j=0;j<(*m_mrt.RtBMain)[pNRBC->NodeIDList[i]].ConnectCount;j++)
		{
			TmpConnectIdx=(*m_mrt.RtBMain)[pNRBC->NodeIDList[i]].ConnectStruIndex+j;
			if((*m_mrt.RtBConnect)[TmpConnectIdx].TNodeID==pNRBC->NodeIDList[i+1])
			{
				if(	pNRBC->NSS[i].RoadLength > (*m_mrt.RtBConnect)[TmpConnectIdx].RoadLength)
				{
					pNRBC->NSS[i].RoadLength	=	(*m_mrt.RtBConnect)[TmpConnectIdx].RoadLength;
					pNRBC->NSS[i].RoadVoiceType =	0;
					WeightingClass				=	(*m_mrt.RtBConnect)[TmpConnectIdx].RoadClass;
				}
			}
		}
		if(i!=0 && i!=(pNRBC->NSSCount-1)){//計算加權
			if(!GetWeighting(WeightingClass)) continue;
			pNRBC->WeightDis +=	pNRBC->NSS[i].RoadLength * ClassWeighting / _WEIGHTINGBASE;
		}
	}
	m_mrt.Remove_Connect();
	m_mrt.Remove_MainData();

	if(!m_mrt.Read_Index()) return nuFALSE;
	//取得路段RoadID
	for(i=0;i<pNRBC->NSSCount;i++)
	{
		//由起訖NodeID得到RoadID
		pNRBC->NSS[i].BlockIdx = _MAXUNSHORT;	
		pNRBC->NSS[i].RoadIdx  = _MAXUNSHORT;
		GetBlockRoadIdx(&(pNRBC->NSS[i].BlockIdx),&(pNRBC->NSS[i].RoadIdx),pNRBC->NodeIDList[i],pNRBC->NodeIDList[i+1]);
	}
	m_mrt.Remove_Index();
	return nuTRUE;
}

nuBOOL Class_CalMRT::LocalAStarSuccessWorkStep2(PNAVIRTBLOCKCLASS pNRBC)//區域規劃(Local AStar)成功的後續資料整理
{
	#ifdef DEF_MAKETHRROUTE
		nuTcscpy(m_mdw.FName, MakeThr_MapPath);
		nuTcscat(m_mdw.FName, nuTEXT(".mdw"));
	#else
		nuTcscpy(m_mdw.FName, nuTEXT(".mdw"));
		g_pRtFSApi->FS_FindFileWholePath(-1,m_mdw.FName,NURO_MAX_PATH);
	#endif
	//先要設法取得mdw名稱
	if(!m_mdw.Read_Header()){	m_mdw.Remove_Header();	return nuFALSE;	}
	nuDWORD i = 0, k = 0, TmpSaveVertexCount = 0;
	nuUSHORT j = 0;
	nuLONG TmpSaveCount = 0;
	MDWRoadData RD  = {0};
	NUROPOINT tempPt = {0};
	nuLONG m_TmpStartIdx = 0;
	//透過BlockID找出BlockIdx
	for(i=0;i<pNRBC->NSSCount;i++){
		for(j=0;j<m_mdw.Header.m_TotalBlockCount;j++){
			if(pNRBC->NSS[i].BlockIdx==(*m_mdw.Header.p_GBlockID)[j]){
				pNRBC->NSS[i].BlockIdx=j;
				break;
			}
		}
	}
	//經由BlockID ,RoadID 取得路名位置 路段座標點數 路段座標點 邊界(RoadNameAddr ArcPtCount Bound ArcPt)
	for(i=0;i<pNRBC->NSSCount;i++){
		TmpSaveCount=0;
		TmpSaveVertexCount=100000;

		if(!m_mdw.Read_BlockData(pNRBC->NSS[i].BlockIdx)){
			return nuFALSE;
		}
			RD=(*(*m_mdw.Rd)[pNRBC->NSS[i].BlockIdx].p_RdData)[pNRBC->NSS[i].RoadIdx];

			if(TmpSaveVertexCount>RD.VertexCount){
				TmpSaveVertexCount=RD.VertexCount;
				pNRBC->NSS[i].BlockIdx=pNRBC->NSS[i].BlockIdx;
				pNRBC->NSS[i].RoadIdx=pNRBC->NSS[i].RoadIdx;
				pNRBC->NSS[i].CityID=0;
				pNRBC->NSS[i].TownID=0;
				pNRBC->NSS[i].RoadNameAddr=0;
				//Louis081117 增加資料來保存區域資訊
				pNRBC->NSS[i].TownID=RD.MapID;
				pNRBC->NSS[i].RoadNameAddr=RD.RTBID;

				pNRBC->NSS[i].RoadClass=RD.RoadClass;
				g_pCRDC->Create_ArcPt(pNRBC->NSS[i],RD.VertexCount);

				m_TmpStartIdx=RD.StartVertexIndex_high+RD.StartVertexIndex_Low*_COORLEVELNUM;
				tempPt.x = (*(*m_mdw.Rd)[pNRBC->NSS[i].BlockIdx].p_VertexCoor)[m_TmpStartIdx].x;
				tempPt.y = (*(*m_mdw.Rd)[pNRBC->NSS[i].BlockIdx].p_VertexCoor)[m_TmpStartIdx].y;

				pNRBC->NSS[i].Bound.left=tempPt.x;
				pNRBC->NSS[i].Bound.top=tempPt.y;
				pNRBC->NSS[i].Bound.right=tempPt.x;
				pNRBC->NSS[i].Bound.bottom=tempPt.y;


				if(3>GetDistance(tempPt,pNRBC->NSS[i].StartCoor)){//判定本段道路某一端點是否與前一道路相連接
					for(k=0;k<RD.VertexCount;k++){
						pNRBC->NSS[i].ArcPt[k]=(*(*m_mdw.Rd)[pNRBC->NSS[i].BlockIdx].p_VertexCoor)[m_TmpStartIdx+k];
						if(pNRBC->NSS[i].Bound.left>pNRBC->NSS[i].ArcPt[k].x)	pNRBC->NSS[i].Bound.left=pNRBC->NSS[i].ArcPt[k].x;
						if(pNRBC->NSS[i].Bound.top>pNRBC->NSS[i].ArcPt[k].y)	pNRBC->NSS[i].Bound.top=pNRBC->NSS[i].ArcPt[k].y;
						if(pNRBC->NSS[i].Bound.right<pNRBC->NSS[i].ArcPt[k].x)	pNRBC->NSS[i].Bound.right=pNRBC->NSS[i].ArcPt[k].x;
						if(pNRBC->NSS[i].Bound.bottom<pNRBC->NSS[i].ArcPt[k].y)	pNRBC->NSS[i].Bound.bottom=pNRBC->NSS[i].ArcPt[k].y;
					}
				}
				else{
					for(k=0;k<RD.VertexCount;k++){
						pNRBC->NSS[i].ArcPt[k]=(*(*m_mdw.Rd)[pNRBC->NSS[i].BlockIdx].p_VertexCoor)[m_TmpStartIdx+RD.VertexCount-1-k];
						if(pNRBC->NSS[i].Bound.left>pNRBC->NSS[i].ArcPt[k].x)	pNRBC->NSS[i].Bound.left=pNRBC->NSS[i].ArcPt[k].x;
						if(pNRBC->NSS[i].Bound.top>pNRBC->NSS[i].ArcPt[k].y)	pNRBC->NSS[i].Bound.top=pNRBC->NSS[i].ArcPt[k].y;
						if(pNRBC->NSS[i].Bound.right<pNRBC->NSS[i].ArcPt[k].x)	pNRBC->NSS[i].Bound.right=pNRBC->NSS[i].ArcPt[k].x;
						if(pNRBC->NSS[i].Bound.bottom<pNRBC->NSS[i].ArcPt[k].y)	pNRBC->NSS[i].Bound.bottom=pNRBC->NSS[i].ArcPt[k].y;
					}
				}
			}
	}
	//透過座標 換算路口的角度
	NUROPOINT Pt[3] = {0};
	for(i=0;i<(pNRBC->NSSCount-1);i++)
	{
		Pt[1]=pNRBC->NSS[i].EndCoor;
		for(j=1;j<pNRBC->NSS[i].ArcPtCount;j++)
		{
			Pt[0]=pNRBC->NSS[i].ArcPt[pNRBC->NSS[i].ArcPtCount-1-j];
			if((NURO_ABS(Pt[0].x - Pt[1].x) + NURO_ABS(Pt[0].y - Pt[1].y)) > _TURNFLAGDIFFDIS)
			{
				break;
			}
		}
		for(j = 1; j < pNRBC->NSS[i+1].ArcPtCount; j++)
		{
			Pt[2] = pNRBC->NSS[i+1].ArcPt[j];
			if((NURO_ABS(Pt[2].x - Pt[1].x) + NURO_ABS(Pt[2].y - Pt[1].y)) > _TURNFLAGDIFFDIS)
			{
				break;
			}
		}
		pNRBC->NSS[i].TurnAngle = GetAngle(Pt[0],Pt[1],Pt[2]);
	}
	pNRBC->NSS[pNRBC->NSSCount-1].TurnAngle = 0;

	m_mdw.Reset();
	return nuTRUE;
}

nuBOOL Class_CalMRT::LocalAStarSuccessWorkStep3(PNAVIRTBLOCKCLASS pNRBC)//區域規劃(Local AStar)成功的後續資料整理
{
	//設法讓這些資料能重新建構自己的RTB
	//需補完的內容有
	//nuLONG	RealDis;//真實距離
	//nuLONG	WeightDis;//加權距離
	//PTFIXTOROADSTRU FInfo;//起點相關資訊
	//PTFIXTOROADSTRU TInfo;//終點相關資訊
	//tagNAVIRTBLOCKCLASS *NextRTB;
	//特別要記住 FInfo,TInfo 這裡面的NodeID一定要填為0 這樣才能啟動自動搜尋功能
	//但是還需要注意
	nuLONG MapID = 0, RTBID = 0, SaveIndex = 0;
	nuDWORD i = 0;
	PNAVIRTBLOCKCLASS pSaveNaviRTBClass = NULL, pLastSaveNaviRTBClass = NULL, pTmpNaviRTBClass = NULL;
	MapID=-1;
	RTBID=-1;
	pSaveNaviRTBClass=NULL;
	pLastSaveNaviRTBClass=NULL;
	pTmpNaviRTBClass=NULL;
	for(i=0;i<pNRBC->NSSCount;i++){	
		if( pNRBC->NSS[i].TownID==MapID && pNRBC->NSS[i].RoadNameAddr==RTBID ){
			pLastSaveNaviRTBClass->NSSCount++;
			pLastSaveNaviRTBClass->TInfo.FixCoor=pNRBC->NSS[i].EndCoor;
		}
		else{
			MapID=pNRBC->NSS[i].TownID;
			RTBID=pNRBC->NSS[i].RoadNameAddr;
			if(pSaveNaviRTBClass==NULL){
				pTmpNaviRTBClass=NULL;
				g_pCRDC->Create_NaviRtBlockClass(pTmpNaviRTBClass);
				pSaveNaviRTBClass=pTmpNaviRTBClass;
				pLastSaveNaviRTBClass=pTmpNaviRTBClass;
				pLastSaveNaviRTBClass->NSSCount=1;

				pLastSaveNaviRTBClass->FInfo.MapID=pNRBC->NSS[i].TownID;
				pLastSaveNaviRTBClass->TInfo.MapID=pNRBC->NSS[i].TownID;
				pLastSaveNaviRTBClass->FInfo.RTBID=pNRBC->NSS[i].RoadNameAddr;
				pLastSaveNaviRTBClass->TInfo.RTBID=pNRBC->NSS[i].RoadNameAddr;
				pLastSaveNaviRTBClass->FInfo.FixCoor=pNRBC->NSS[i].StartCoor;
				pLastSaveNaviRTBClass->TInfo.FixCoor=pNRBC->NSS[i].EndCoor;
			}
			else{
				pTmpNaviRTBClass=NULL;
				g_pCRDC->Create_NaviRtBlockClass(pTmpNaviRTBClass);
				pLastSaveNaviRTBClass->NextRTB=pTmpNaviRTBClass;
				pLastSaveNaviRTBClass=pTmpNaviRTBClass;
				pLastSaveNaviRTBClass->NSSCount=1;

				pLastSaveNaviRTBClass->FInfo.MapID=pNRBC->NSS[i].TownID;
				pLastSaveNaviRTBClass->TInfo.MapID=pNRBC->NSS[i].TownID;
				pLastSaveNaviRTBClass->FInfo.RTBID=pNRBC->NSS[i].RoadNameAddr;
				pLastSaveNaviRTBClass->TInfo.RTBID=pNRBC->NSS[i].RoadNameAddr;
				pLastSaveNaviRTBClass->FInfo.FixCoor=pNRBC->NSS[i].StartCoor;
				pLastSaveNaviRTBClass->TInfo.FixCoor=pNRBC->NSS[i].EndCoor;
			}
		}
	}

	pTmpNaviRTBClass=pSaveNaviRTBClass;
	while(pTmpNaviRTBClass!=NULL){
		g_pCRDC->Create_NaviSubClass(pTmpNaviRTBClass,pTmpNaviRTBClass->NSSCount);
		pTmpNaviRTBClass=pTmpNaviRTBClass->NextRTB;
	}
	pTmpNaviRTBClass=NULL;

	//要注意座標的添加要怎麼處理
	MapID=-1;
	RTBID=-1;
	pLastSaveNaviRTBClass=NULL;
	for(i=0;i<pNRBC->NSSCount;i++){	
		if( pNRBC->NSS[i].TownID==MapID && pNRBC->NSS[i].RoadNameAddr==RTBID ){
			SaveIndex++;
			pLastSaveNaviRTBClass->NSS[SaveIndex]=pNRBC->NSS[i];
			pLastSaveNaviRTBClass->RealDis+=pNRBC->NSS[i].RoadLength;
			g_pCRDC->Create_ArcPt(pLastSaveNaviRTBClass->NSS[SaveIndex],pLastSaveNaviRTBClass->NSS[SaveIndex].ArcPtCount);
			nuMemcpy(pLastSaveNaviRTBClass->NSS[SaveIndex].ArcPt,pNRBC->NSS[i].ArcPt,pLastSaveNaviRTBClass->NSS[SaveIndex].ArcPtCount*sizeof(NUROPOINT));
		}
		else{
			MapID=pNRBC->NSS[i].TownID;
			RTBID=pNRBC->NSS[i].RoadNameAddr;
			if(pLastSaveNaviRTBClass==NULL){
				pLastSaveNaviRTBClass=pSaveNaviRTBClass;
				SaveIndex=0;
				pLastSaveNaviRTBClass->NSS[SaveIndex]=pNRBC->NSS[i];
				pLastSaveNaviRTBClass->RealDis=pNRBC->NSS[i].RoadLength;
				g_pCRDC->Create_ArcPt(pLastSaveNaviRTBClass->NSS[SaveIndex],pLastSaveNaviRTBClass->NSS[SaveIndex].ArcPtCount);
				nuMemcpy(pLastSaveNaviRTBClass->NSS[SaveIndex].ArcPt,pNRBC->NSS[i].ArcPt,pLastSaveNaviRTBClass->NSS[SaveIndex].ArcPtCount*sizeof(NUROPOINT));
			}
			else{
				pLastSaveNaviRTBClass=pLastSaveNaviRTBClass->NextRTB;
				SaveIndex=0;
				pLastSaveNaviRTBClass->NSS[SaveIndex]=pNRBC->NSS[i];
				pLastSaveNaviRTBClass->RealDis=pNRBC->NSS[i].RoadLength;
				g_pCRDC->Create_ArcPt(pLastSaveNaviRTBClass->NSS[SaveIndex],pLastSaveNaviRTBClass->NSS[SaveIndex].ArcPtCount);
				nuMemcpy(pLastSaveNaviRTBClass->NSS[SaveIndex].ArcPt,pNRBC->NSS[i].ArcPt,pLastSaveNaviRTBClass->NSS[SaveIndex].ArcPtCount*sizeof(NUROPOINT));
			}
		}
		pLastSaveNaviRTBClass->NSS[SaveIndex].TownID=0;
		pLastSaveNaviRTBClass->NSS[SaveIndex].RoadNameAddr=0;
	}

	//設法使pLastSaveNaviRTBClass跟pNRBC連結
	pTmpNaviRTBClass=pNRBC->NextRTB;
	pNRBC->NextRTB=pSaveNaviRTBClass;
	while(nuTRUE){
		if(NULL==pSaveNaviRTBClass->NextRTB){
			pSaveNaviRTBClass->NextRTB=pTmpNaviRTBClass;
			break;
		}
		pSaveNaviRTBClass=pSaveNaviRTBClass->NextRTB;
	}

	return nuTRUE;
}

/*** 透過起訖點NodeID找出該路段在Dw中的BlockIdx與RoadIdx ***/
nuBOOL Class_CalMRT::GetBlockRoadIdx(nuUSHORT *BlockIdx, nuUSHORT *RoadIdx, nuULONG FNodeID, nuULONG TNodeID)
{
	/*** 使用二分法 對索引表進行兩次二分搜尋法 找出對應的數值 如果沒有搜尋到 回傳nuFALSE並將數值初始化 ***/
	*RoadIdx = _MAXUNSHORT;
	nuLONG IdxRange = 0;
	nuLONG m_Min=-1;
	nuLONG m_Max=m_mrt.RtBHeader.TotalRoadCount;
	nuLONG m_Mid=(m_Max+m_Min)/2;
	IdxRange=1;

	//只要能以快速的方式尋找 並排除掉不可用的點 那就是好方法
	while(nuTRUE){
		if(FNodeID>(*m_mrt.RtBIdx)[m_Mid].FNodeID)
		{	m_Min=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
		else if(FNodeID<(*m_mrt.RtBIdx)[m_Mid].FNodeID)
		{	m_Max=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
		else{
			if(TNodeID>(*m_mrt.RtBIdx)[m_Mid].TNodeID)
			{	m_Min=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
			else if(TNodeID<(*m_mrt.RtBIdx)[m_Mid].TNodeID)
			{	m_Max=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
			else{
				*BlockIdx=(*m_mrt.RtBIdx)[m_Mid].BlockIdx;
				*RoadIdx=(*m_mrt.RtBIdx)[m_Mid].RoadIdx;
				return nuTRUE;
			}
		}
		if((m_Max-m_Min)<=IdxRange) break;
	}

	m_Min=-1;
	m_Max=m_mrt.RtBHeader.TotalRoadCount;
	m_Mid=(m_Max+m_Min)/2;
	while(nuTRUE){
		if(TNodeID>(*m_mrt.RtBIdx)[m_Mid].FNodeID)
		{	m_Min=m_Mid;	m_Mid=(m_Max+m_Min)/2;		}
		else if(TNodeID<(*m_mrt.RtBIdx)[m_Mid].FNodeID)
		{	m_Max=m_Mid;	m_Mid=(m_Max+m_Min)/2;		}
		else
		{
			if(FNodeID>(*m_mrt.RtBIdx)[m_Mid].TNodeID)
			{	m_Min=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
			else if(FNodeID<(*m_mrt.RtBIdx)[m_Mid].TNodeID)
			{	m_Max=m_Mid;	m_Mid=(m_Max+m_Min)/2;	}
			else{
				*BlockIdx=(*m_mrt.RtBIdx)[m_Mid].BlockIdx;
				*RoadIdx=(*m_mrt.RtBIdx)[m_Mid].RoadIdx;
				return nuTRUE;
			}
		}
		if((m_Max-m_Min)<=IdxRange) break;
	}
	return nuFALSE;
}

nuLONG Class_CalMRT::GetDistance(NUROPOINT Pt1, NUROPOINT PT2)
{
	if(Pt1.x==PT2.x && Pt1.y==PT2.y) return 0;
	nuLONG X,Y;
	X = NURO_ABS(Pt1.x-PT2.x) * nulCos(NURO_ABS((Pt1.y+PT2.y)/2) / _EACHDEGREECOORSIZE) / nuGetlBase() * 1113 / _LONLATBASE;
	Y = NURO_ABS(Pt1.y-PT2.y) * 1109 / _LONLATBASE;
	return nulSqrt(X,Y);
}

nuLONG Class_CalMRT::GetAngle(NUROPOINT FPt, NUROPOINT NPt, NUROPOINT TPt)
{
	nuLONG  ct = 0, ct1 = 0, ct2 = 0;
	ct1=nulAtan2(FPt.y-NPt.y,FPt.x-NPt.x);
	ct2=nulAtan2(NPt.y-TPt.y,NPt.x-TPt.x);

	ct = ct2-ct1;
	if(ct>180){
		ct=ct-360;
	}
	else if(ct<(-180)){
		ct=ct+360;
	}
	return ct;
}
