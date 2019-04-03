
#include "CDontToHere.h"

CDontToHere::CDontToHere()
{
	InitConst();
}

CDontToHere::~CDontToHere()
{
}

nuVOID CDontToHere::InitConst()
{
	nuMemset(DontToPt, 0, sizeof(DontToNodeID)*MaxDontToHereCount);
	CheckCount=0;
	Indexvalue=0;
	b_CongestionRoute=nuFALSE;
	CongestionIDCount=0;
}

nuBOOL CDontToHere::CheckCongestionID(nuDWORD NodeID)
{
	nuLONG i = 0;
	if(!b_CongestionRoute) return nuFALSE;
	for(i=0;i<CongestionIDCount;i++)
	{
		if(NodeID==CongestionID[i]) return nuTRUE;
	}
	return nuFALSE;
}
nuVOID CDontToHere::JoinNewCongestionID(nuDWORD NodeID)
{
	if(!b_CongestionRoute) return;
	if(CongestionIDCount>=MaxCongestionIDCount) return;
	CongestionID[CongestionIDCount]=NodeID;
	CongestionIDCount++;
}
nuVOID CDontToHere::CleanCongestionID()
{
	b_CongestionRoute=nuFALSE;
	CongestionIDCount=0;
}


nuVOID CDontToHere::SystemReRouting()
{
	CheckCount=0;
	Indexvalue++;
}

nuLONG CDontToHere::CheckDontTo(nuULONG MapID, nuULONG RtBID, nuULONG NodeID)
{
	return 0;
	nuLONG i = 0;
	if(CheckCount>DontToWorkingCount)		return 0;
	CheckCount++;

	for(i=0;i<MaxDontToHereCount;i++)
	{
		if(DontToPt[i].UseCount==0) continue;
		if(DontToPt[i].MapID==MapID && DontToPt[i].RtBID==RtBID && DontToPt[i].NodeID==NodeID)
		{
			return DontToPt[i].UseCount;
		}
	}
	return 0;
}

nuVOID CDontToHere::JoinNewDontTo(nuULONG MapID, nuULONG RtBID, nuULONG NodeID, nuLONG Order)
{
	nuLONG i = 0;
	//有找到 就更新舊的
	for(i=0;i<MaxDontToHereCount;i++)
	{
		if(DontToPt[i].UseCount==0) continue;
		if(DontToPt[i].MapID==MapID && DontToPt[i].RtBID==RtBID && DontToPt[i].NodeID==NodeID)
		{
			DontToPt[i].UseCount++;
			DontToPt[i].Index=Indexvalue;
//			DontToPt[i].UseCount += OrderFixCount(Order);
//			DontToPt[i].WeightingFix += OrderFixWeighting(Order);
			return;
		}
	}

	nuLONG limitindex = 1000000, WorkIndex = 0;
	for(i=0;i<MaxDontToHereCount;i++)
	{
		//尋找Index最小的位置
		if(limitindex>DontToPt[i].Index)
		{
			limitindex=DontToPt[i].Index;
			WorkIndex=i;
		}
	}
	DontToPt[WorkIndex].MapID=MapID;
	DontToPt[WorkIndex].RtBID=RtBID;
	DontToPt[WorkIndex].NodeID=NodeID;
	DontToPt[WorkIndex].Index=Indexvalue;
	DontToPt[WorkIndex].UseCount=1;
	return;

/*
	//沒有找到 就填補控缺
	for(i=0;i<MaxDontToHereCount;i++)
	{
		if(DontToPt[i].UseCount==0)
		{
			DontToPt[i].MapID=MapID;
			DontToPt[i].RtBID=RtBID;
			DontToPt[i].NodeID=NodeID;
			DontToPt[i].UseCount++;
//			DontToPt[i].UseCount=OrderFixCount(Order);
//			DontToPt[i].WeightingFix=OrderFixWeighting(Order);
			return;
		}
	}
*/
}

nuLONG CDontToHere::OrderFixWeighting(nuLONG Order)
{
	switch(Order)
	{
		case 1:		return 50;
		case 2:		return 45;
		case 3:		return 40;
		case 4:		return 35;
		case 5:		return 30;
		case 6:		return 25;
		case 7:		return 20;
		case 8:		return 15;
		case 9:		return 10;
		case 10:	return 5;
		default:	return 0;
	}
}

nuLONG CDontToHere::OrderFixCount(nuLONG Order)
{
	switch(Order)
	{
		case 1:		return 6;//10;
		case 2:		return 6;//9;
		case 3:		return 5;//8;
		case 4:		return 5;//7;
		case 5:		return 4;//6;
		case 6:		return 4;//5;
		case 7:		return 3;//4;
		case 8:		return 3;//3;
		case 9:		return 2;//2;
		case 10:	return 2;//1;
		default:	return 0;
	}
}