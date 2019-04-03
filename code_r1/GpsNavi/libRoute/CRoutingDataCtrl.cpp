
#include "CRoutingDataCtrl.h"
#include "NuroDefine.h"

CRoutingDataCtrl::CRoutingDataCtrl()
{
}

CRoutingDataCtrl::~CRoutingDataCtrl()
{
}

#ifdef LCMM
nuVOID CRoutingDataCtrl::Release_RoutingData(PROUTINGDATA &NaviData)
{
	NAVIFROMTOCLASS* tmpNaviFT = NULL;
	if(NaviData==NULL) return;
	int RouteCounter_LCMM = 0;
	while(RouteCounter_LCMM < 3)
	{
		while(NaviData->NFTC[RouteCounter_LCMM]!=NULL){
			tmpNaviFT=NaviData->NFTC[RouteCounter_LCMM];
			NaviData->NFTC[RouteCounter_LCMM]=tmpNaviFT->NextFT;
			Release_NaviFromToClass(tmpNaviFT);//結構體下一層資料清空
		}
		RouteCounter_LCMM++;
	}
	NaviData->NFTCCount=0;
}

nuBOOL CRoutingDataCtrl::Create_NaviFromToClass(PNAVIFROMTOCLASS &NFTC)
{
	Release_NaviFromToClass(NFTC);//清理舊的資料
	NFTC=new NAVIFROMTOCLASS;
	if(NFTC){
		NFTC->RunPass=nuFALSE;
		NFTC->Reserve=0;
		NFTC->FCoor.x=0;//-1;
		NFTC->FCoor.y=0;//-1;
		NFTC->FromFileID=0;
		NFTC->NRBCCount=0;
		NFTC->NRBC=NULL;
		NFTC->TCoor.x=0;//-1;
		NFTC->TCoor.y=0;//-1;
		NFTC->ToFileID=-1;
		NFTC->FromRtBID=-1;
		NFTC->ToRtBID=-1;
		NFTC->NextFT=NULL;
		return nuTRUE;
	}
	else{
		return nuFALSE;
	}
}
nuVOID CRoutingDataCtrl::Release_NaviFromToClass(PNAVIFROMTOCLASS &NFTC)
{
	NAVIRTBLOCKCLASS* tmpNaviRTB = NULL;
	if(NFTC==NULL) return;
	while(NFTC->NRBC!=NULL)
	{
		tmpNaviRTB=NFTC->NRBC;
		NFTC->NRBC=tmpNaviRTB->NextRTB;
		Release_NodeIDList(tmpNaviRTB);
		Release_NaviRtBlockClass(tmpNaviRTB);
	}
	delete NFTC;
	NFTC=NULL;
}

nuBOOL CRoutingDataCtrl::Create_NaviRtBlockClass(PNAVIRTBLOCKCLASS &NRBC)
{
	Release_NaviRtBlockClass(NRBC);

	NRBC=new NAVIRTBLOCKCLASS;
	if(NRBC){
		nuMemset(NRBC,0,sizeof(NAVIRTBLOCKCLASS));
		NRBC->RTBStateFlag=1;
		NRBC->RTBWMPFixFlag=0;
		return nuTRUE;
	}
	else{
		return nuFALSE;
	}
}
nuVOID CRoutingDataCtrl::Release_NaviRtBlockClass(PNAVIRTBLOCKCLASS &NRBC)
{
	if(NRBC==NULL) return;
	Release_NaviSubClass(NRBC);
	delete NRBC;
	NRBC=NULL;
}

nuBOOL CRoutingDataCtrl::Create_NaviSubClass(PNAVIRTBLOCKCLASS &NRBC,nuULONG Count)
{
	Release_NaviSubClass(NRBC);
	NRBC->NSSCount=Count;
	new_NaviSubClass(NRBC->NSS,Count);
	if(NRBC->NSS==NULL)	{	NRBC->NSSCount=0;		return nuFALSE;	}
	else				{	return nuTRUE;	}
}
nuVOID CRoutingDataCtrl::Release_NaviSubClass(PNAVIRTBLOCKCLASS &NRBC)
{
	nuULONG i = 0;
	if(NRBC->NSS==NULL) return;
	for(i=0;i<NRBC->NSSCount;i++){
		Release_ArcPt(NRBC->NSS[i]);
		Release_CrossData(NRBC->NSS[i]);
	}
	delete_NaviSubClass(NRBC->NSS);
}
nuBOOL CRoutingDataCtrl::new_NaviSubClass(PNAVISUBCLASS &NSS,nuLONG Count)
{
	nuLONG i = 0;
	if(Count<=0) return nuFALSE;
	delete_NaviSubClass(NSS);//清理舊的資料
	NSS=new NAVISUBCLASS[Count];
	if(NSS==NULL) return nuFALSE;
	for(i=0;i<Count;i++){
		NSS[i].BlockIdx=0;
		NSS[i].RoadIdx=0;
		NSS[i].RoadNameAddr=-1;
		NSS[i].RoadLength=0;
		NSS[i].RoadVoiceType=-1;
		NSS[i].RoadClass=0;
		NSS[i].CrossCount=0;
		NSS[i].CityID=0;
		NSS[i].TownID=0;
		NSS[i].NCC=NULL;
		NSS[i].StartCoor.x=-1;
		NSS[i].StartCoor.y=-1;
		NSS[i].EndCoor.x=-1;
		NSS[i].EndCoor.y=-1;
		NSS[i].Flag=-1;
		NSS[i].TurnFlag=-1;
		NSS[i].TurnAngle=-1;
		NSS[i].ArcPtCount=0;
		NSS[i].Bound.left=-1;
		NSS[i].Bound.top=-1;
		NSS[i].Bound.right=-1;
		NSS[i].Bound.bottom=-1;
		NSS[i].ArcPt=NULL;
		NSS[i].Real3DPic=0;
	}
	return nuTRUE;
}
nuVOID CRoutingDataCtrl::delete_NaviSubClass(PNAVISUBCLASS &NSS)
{
	if(NSS==NULL) return;
	delete [] NSS;
	NSS=NULL;
}

nuBOOL CRoutingDataCtrl::Create_NodeIDList(PNAVIRTBLOCKCLASS &NRBC,nuULONG Count)
{
	if(Count<=0) return nuFALSE;
	NRBC->NodeIDList = new nuULONG[Count];
	if(NRBC->NodeIDList==NULL)	{	NRBC->NodeIDCount=0;	return nuFALSE;	}
	else						{	NRBC->NodeIDCount=Count;	return nuTRUE;	}
}
nuVOID CRoutingDataCtrl::Release_NodeIDList(PNAVIRTBLOCKCLASS &NRBC)
{
	NRBC->NodeIDCount=0;
	if(NRBC->NodeIDList==NULL) return;
	delete [] NRBC->NodeIDList;
	NRBC->NodeIDList=NULL;
}

nuBOOL CRoutingDataCtrl::Create_CrossData(NAVISUBCLASS &NSS,nuLONG Count)
{
	Release_CrossData(NSS);
	if(Count==-1 || Count==0)	return nuTRUE;
	NSS.NCC	=	new NAVICROSSCLASS[Count];
	if(NSS.NCC!=NULL){
		NSS.CrossCount=Count;
		nuMemset(NSS.NCC,0,sizeof(NAVICROSSCLASS)*Count);
		return nuTRUE;
	}
	else{
		NSS.CrossCount=0;
		return nuFALSE;
	}
}
nuVOID CRoutingDataCtrl::Release_CrossData(NAVISUBCLASS &NSS)
{
	NSS.CrossCount=0;
	if(NSS.NCC==NULL) return;
	delete [] NSS.NCC;
	NSS.NCC=NULL;
}
nuBOOL CRoutingDataCtrl::Create_ArcPt(NAVISUBCLASS &NSS,nuLONG Count)
{
	NSS.ArcPt	=	new NUROPOINT[Count];
	if(NSS.ArcPt==NULL){
		NSS.ArcPtCount=0;	return nuFALSE;	}
	else{
		NSS.ArcPtCount=Count;
		nuMemset(NSS.ArcPt,1,sizeof(NUROPOINT)*Count);
		return nuTRUE;
	}
}
nuVOID CRoutingDataCtrl::Release_ArcPt(NAVISUBCLASS &NSS)
{
	NSS.ArcPtCount=0;
	if(NSS.ArcPt==NULL) return;
	delete [] NSS.ArcPt;
	NSS.ArcPt=NULL;
}
#else
nuVOID CRoutingDataCtrl::Release_RoutingData(PROUTINGDATA &NaviData)
{
	NAVIFROMTOCLASS* tmpNaviFT = NULL;
	if(NaviData==NULL) return;

	while(NaviData->NFTC!=NULL){
		tmpNaviFT=NaviData->NFTC;
		NaviData->NFTC=tmpNaviFT->NextFT;
		Release_NaviFromToClass(tmpNaviFT);//結構體下一層資料清空
	}
	NaviData->NFTCCount=0;
}

nuBOOL CRoutingDataCtrl::Create_NaviFromToClass(PNAVIFROMTOCLASS &NFTC)
{
	Release_NaviFromToClass(NFTC);//清理舊的資料
	NFTC=new NAVIFROMTOCLASS;
	if(NFTC){
		NFTC->FCoor.x=0;//-1;
		NFTC->FCoor.y=0;//-1;
		NFTC->FromFileID=0;
		NFTC->NRBCCount=0;
		NFTC->NRBC=NULL;
		NFTC->RunPass=nuFALSE;
		NFTC->TCoor.x=0;//-1;
		NFTC->TCoor.y=0;//-1;
		NFTC->ToFileID=-1;
		NFTC->FromRtBID=-1;
		NFTC->ToRtBID=-1;
		NFTC->NextFT=NULL;
		return nuTRUE;
	}
	else{
		return nuFALSE;
	}
}
nuVOID CRoutingDataCtrl::Release_NaviFromToClass(PNAVIFROMTOCLASS &NFTC)
{
	NAVIRTBLOCKCLASS* tmpNaviRTB = NULL;
	if(NFTC==NULL) return;
	while(NFTC->NRBC!=NULL)
	{
		tmpNaviRTB=NFTC->NRBC;
		NFTC->NRBC=tmpNaviRTB->NextRTB;
		Release_NodeIDList(tmpNaviRTB);
		Release_NaviRtBlockClass(tmpNaviRTB);
	}
	delete NFTC;
	NFTC=NULL;
}

nuBOOL CRoutingDataCtrl::Create_NaviRtBlockClass(PNAVIRTBLOCKCLASS &NRBC)
{
	Release_NaviRtBlockClass(NRBC);

	NRBC=new NAVIRTBLOCKCLASS;
	if(NRBC){
		nuMemset(NRBC,0,sizeof(NAVIRTBLOCKCLASS));
		NRBC->RTBStateFlag=1;
		NRBC->RTBWMPFixFlag=0;
		return nuTRUE;
	}
	else{
		return nuFALSE;
	}
}
nuVOID CRoutingDataCtrl::Release_NaviRtBlockClass(PNAVIRTBLOCKCLASS &NRBC)
{
	if(NRBC==NULL) return;
	Release_NaviSubClass(NRBC);
	delete NRBC;
	NRBC=NULL;
}

nuBOOL CRoutingDataCtrl::Create_NaviSubClass(PNAVIRTBLOCKCLASS &NRBC,nuULONG Count)
{
	Release_NaviSubClass(NRBC);
	NRBC->NSSCount=Count;
	new_NaviSubClass(NRBC->NSS,Count);
	if(NRBC->NSS==NULL)	{	NRBC->NSSCount=0;		return nuFALSE;	}
	else				{	return nuTRUE;	}
}
nuVOID CRoutingDataCtrl::Release_NaviSubClass(PNAVIRTBLOCKCLASS &NRBC)
{
	nuULONG i = 0;
	if(NRBC->NSS==NULL) return;
	for(i=0;i<NRBC->NSSCount;i++){
		Release_ArcPt(NRBC->NSS[i]);
		Release_CrossData(NRBC->NSS[i]);
	}
	delete_NaviSubClass(NRBC->NSS);
}
nuBOOL CRoutingDataCtrl::new_NaviSubClass(PNAVISUBCLASS &NSS,nuLONG Count)
{
	nuLONG i = 0;
	if(Count<=0) return nuFALSE;
	delete_NaviSubClass(NSS);//清理舊的資料
	NSS=new NAVISUBCLASS[Count];
	if(NSS==NULL) return nuFALSE;
	for(i=0;i<Count;i++){
		NSS[i].BlockIdx=0;
		NSS[i].RoadIdx=0;
		NSS[i].RoadNameAddr=-1;
		NSS[i].RoadLength=0;
		NSS[i].RoadVoiceType=-1;
		NSS[i].RoadClass=0;
		NSS[i].CrossCount=0;
		NSS[i].CityID=0;
		NSS[i].TownID=0;
		NSS[i].NCC=NULL;
		NSS[i].StartCoor.x=-1;
		NSS[i].StartCoor.y=-1;
		NSS[i].EndCoor.x=-1;
		NSS[i].EndCoor.y=-1;
		NSS[i].Flag=-1;
		NSS[i].TurnFlag=-1;
		NSS[i].TurnAngle=-1;
		NSS[i].ArcPtCount=0;
		NSS[i].Bound.left=-1;
		NSS[i].Bound.top=-1;
		NSS[i].Bound.right=-1;
		NSS[i].Bound.bottom=-1;
		NSS[i].ArcPt=NULL;
		NSS[i].Real3DPic=0;
	}
	return nuTRUE;
}
nuVOID CRoutingDataCtrl::delete_NaviSubClass(PNAVISUBCLASS &NSS)
{
	if(NSS==NULL) return;
	delete [] NSS;
	NSS=NULL;
}

nuBOOL CRoutingDataCtrl::Create_NodeIDList(PNAVIRTBLOCKCLASS &NRBC,nuULONG Count)
{
	if(Count<=0) return nuFALSE;
	NRBC->NodeIDList	=	new nuULONG[Count];
	if(NRBC->NodeIDList==NULL)	{	NRBC->NodeIDCount=0;	return nuFALSE;	}
	else						{	NRBC->NodeIDCount=Count;	return nuTRUE;	}
}
nuVOID CRoutingDataCtrl::Release_NodeIDList(PNAVIRTBLOCKCLASS &NRBC)
{
	NRBC->NodeIDCount=0;
	if(NRBC->NodeIDList==NULL) return;
	delete [] NRBC->NodeIDList;
	NRBC->NodeIDList=NULL;
}

nuBOOL CRoutingDataCtrl::Create_CrossData(NAVISUBCLASS &NSS,nuLONG Count)
{
	Release_CrossData(NSS);
	if(Count==-1 || Count==0)	return nuTRUE;
	NSS.NCC	=	new NAVICROSSCLASS[Count];
	if(NSS.NCC!=NULL){
		NSS.CrossCount=Count;
		nuMemset(NSS.NCC,0,sizeof(NAVICROSSCLASS)*Count);
		return nuTRUE;
	}
	else{
		NSS.CrossCount=0;
		return nuFALSE;
	}
}
nuVOID CRoutingDataCtrl::Release_CrossData(NAVISUBCLASS &NSS)
{
	if(NSS.NCC==NULL) return;
	delete [] NSS.NCC;
	NSS.NCC=NULL;
	NSS.CrossCount=0;
}
nuBOOL CRoutingDataCtrl::Create_ArcPt(NAVISUBCLASS &NSS,nuLONG Count)
{
	NSS.ArcPt	=	new NUROPOINT[Count];
	if(NSS.ArcPt==NULL){
		NSS.ArcPtCount=0;	return nuFALSE;	}
	else{
		NSS.ArcPtCount=Count;
		nuMemset(NSS.ArcPt,1,sizeof(NUROPOINT)*Count);
		return nuTRUE;
	}
}
nuVOID CRoutingDataCtrl::Release_ArcPt(NAVISUBCLASS &NSS)
{
	NSS.ArcPtCount=0;
	if(NSS.ArcPt==NULL) return;
	delete [] NSS.ArcPt;
	NSS.ArcPt=NULL;
}
#endif
//--------added by daniel for LCMM-----------------------------------------//

/*nuVOID CRoutingDataCtrl::Release_NaviFromToClass(PNAVIFROMTOCLASS &NFTC)
{
	NAVIRTBLOCKCLASS* tmpNaviRTB;
	if(NFTC==NULL) return;
	while(NFTC->NRBC!=NULL)
	{
		tmpNaviRTB=NFTC->NRBC;
		NFTC->NRBC=tmpNaviRTB->NextRTB;
		Release_NodeIDList(tmpNaviRTB);
		Release_NaviRtBlockClass(tmpNaviRTB);
	}
	delete NFTC;
	NFTC=NULL;
}*/
