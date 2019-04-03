// Copyright (c) 2005 PLink Corp.  All rights reserved.
// $Author: daniel $
// $Date: 2011/06/02 09:55:48 $
// $Revision: 1.11 $
#include "stdAfxRoute.h"
#include "Class_PtFixToRoad.h"
#include "RouteBase.h"
#include "NuroModuleApiStruct.h"
#include "AllSystemDataBase.h"
#include "NuroClasses.h"
#include "NuroConstDefined.h"

Class_PtFixToRoad::Class_PtFixToRoad()
{
	WorkNow=nuFALSE;
	ppclsRef=(RefCtrl**)g_pRtMMApi->MM_AllocMem(sizeof(RefCtrl));
}

Class_PtFixToRoad::~Class_PtFixToRoad()
{
	if(ppclsRef)
	{
		g_pRtMMApi->MM_FreeMem((nuPVOID*)ppclsRef);
		ppclsRef=NULL;
	}
}

nuBOOL Class_PtFixToRoad::PtFixToRoad(nuLONG CoorX,nuLONG CoorY,nuLONG FileIdx,nuULONG RTBlockID,nuLONG RideAngle,nuBOOL b_UseAngle,PTFIXTOROADSTRU &PtFixedInfo)
{
	if(WorkNow==nuTRUE) return nuFALSE;
	WorkNow=nuTRUE;

	nuWORD	k = 0;
	nuLONG	i = 0, MCount = 0;
	nuDWORD	j = 0, TmpDwBlockID[4] = {0};
	nuFILE *RefFile=NULL;

	//設定可採用的資料區塊
	TmpDwBlockID[0] = GetBlockID(CoorX,CoorY);
	TmpDwBlockID[1] = GetBlockID(CoorX - __DATAABOVERANGE_WS,CoorY);
	TmpDwBlockID[2] = GetBlockID(CoorX,CoorY -__DATAABOVERANGE_WS);
	TmpDwBlockID[3] = GetBlockID(CoorX - __DATAABOVERANGE_WS,CoorY - __DATAABOVERANGE_WS);

	PtFixedInfo.FixDis = _ROUTEFIXDIS;
	//MCount=m_Lib_FileName.GetMainNameCount();
	#ifdef DEF_MAKETHRROUTE
		MCount=1;
	#else
		MCount=g_pRtFSApi->FS_GetMapCount();
	#endif

	for(i=0;i<MCount;i++)
	{
		//如果外部限定目標檔案Idx 且 檔案Idx與現在要開啟的不相等 那就不處理
		if(FileIdx!=-1 && FileIdx!=i) continue;

		#ifndef DEF_MAKETHRROUTE
			g_pUOneBH->InitClass();
			nuTcscpy(g_pUOneBH->FName, nuTEXT(".bh"));
			g_pRtFSApi->FS_FindFileWholePath(i,g_pUOneBH->FName,NURO_MAX_PATH);
			g_pUOneBH->ReadBH();

			g_pDB_DwCtrl->InitClass();
			nuTcscpy(g_pDB_DwCtrl->FName, nuTEXT(".rdw"));
			g_pRtFSApi->FS_FindFileWholePath(i,g_pDB_DwCtrl->FName,NURO_MAX_PATH);
			if(nuFALSE==g_pDB_DwCtrl->Read_Header()){ g_pDB_DwCtrl->Reset();	continue;	}

			(*ppclsRef)->InitClass();
			nuTcscpy((*ppclsRef)->FName, nuTEXT(".ref"));
			g_pRtFSApi->FS_FindFileWholePath(i,(*ppclsRef)->FName,NURO_MAX_PATH);
			if(nuFALSE==(*ppclsRef)->ReadRefFile()){	g_pDB_DwCtrl->Reset();	continue;	}

			TmpRtCtrl.InitClass();
			nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
			g_pRtFSApi->FS_FindFileWholePath(i,TmpRtCtrl.FName,NURO_MAX_PATH);
		#else
			g_pUOneBH->InitClass();
			nuTcscpy(g_pUOneBH->FName, MakeThr_MapPath);
			nuTcscat(g_pUOneBH->FName, nuTEXT(".bh"));
			g_pUOneBH->ReadBH();

			g_pDB_DwCtrl->InitClass();
			nuTcscpy(g_pDB_DwCtrl->FName, MakeThr_MapPath);
			nuTcscat(g_pDB_DwCtrl->FName, nuTEXT(".rdw"));
			if(nuFALSE==g_pDB_DwCtrl->Read_Header()){ g_pDB_DwCtrl->Reset();	continue;	}

			(*ppclsRef)->InitClass();
			nuTcscpy((*ppclsRef)->FName, MakeThr_MapPath);
			nuTcscat((*ppclsRef)->FName, nuTEXT(".ref"));
			if(nuFALSE==(*ppclsRef)->ReadRefFile()){	g_pDB_DwCtrl->Reset();	continue;	}

			TmpRtCtrl.InitClass();
			nuTcscpy(TmpRtCtrl.FName, MakeThr_MapPath);
			nuTcscat(TmpRtCtrl.FName, nuTEXT(".rt"));
		#endif

		if(nuFALSE==TmpRtCtrl.Read_Header()){	g_pDB_DwCtrl->Reset();	(*ppclsRef)->ReleaseRef();	continue;	}//讀取RT的Header

		//深入每一個Block去判斷資料並取得投影值
		for(j=0;j<g_pUOneBH->m_BH.m_BHH.BlockCount;j++)
		{
			for(k=0;k<4;k++)
			{
//				if((*m_UOneBH.m_BH.GlobalBlockID)[j]==TmpDwBlockID[k])
				if(g_pUOneBH->GlobalBlockID(j)==TmpDwBlockID[k])
				{
					if(nuFALSE==g_pDB_DwCtrl->Read_BlockData(j)) continue;//取得所在block的道路資料
					GetFixInfo(CoorX,CoorY,j,RTBlockID,RideAngle,b_UseAngle,PtFixedInfo,g_pDB_DwCtrl,*ppclsRef,TmpRtCtrl);
					g_pDB_DwCtrl->Remove_BlockData(j);
				}
			}
		}
		//清除中間過程所用的所有資料
		for(j=0;j<TmpRtCtrl.Rt_Block_Count;j++)
		{
			TmpRtCtrl.ReSetRTB(j);
		}
		TmpRtCtrl.Remove_Header();

		(*ppclsRef)->ReleaseRef();
		g_pDB_DwCtrl->Reset();
		g_pUOneBH->ReleaseBH();
	}

	WorkNow=nuFALSE;
	if((PtFixedInfo.FixDis) >= _ROUTEFIXDIS)
	{	PtFixedInfo.FixDis=-1;	PtFixedInfo.FixCoor.x=-1;	PtFixedInfo.FixCoor.y=-1;	PtFixedInfo.RTBID=-1;	return nuFALSE;	}
	else
	{	return nuTRUE;	}
}

	nuBOOL Class_PtFixToRoad::GetFixInfo(nuLONG CoorX,nuLONG CoorY,nuULONG DwBlockIdx,nuULONG RtBlockID,nuLONG RideAngle,nuBOOL b_UseAngle,PTFIXTOROADSTRU &PtFixedInfo,UONERDwCtrl *TmpDwCtrl,RefCtrl *TmpRef,UONERtCtrl &TmpRtCtrl)
	{
		nuLONG l = 0, TmpRtBIdx = 0, TmpDis = 0, TempDis = 0, TmpAngle = 0;
		nuUINT m = 0;
		nuULONG k = 0, TmpFixNode1 = 0, TmpFixNode2 = 0, TmpRtBID = 0;
		NURORECT Tmp_Rect = {0};
		NUROPOINT Tmp_Pt = {0}, Tmp_Coor = {0}, TempPt[2] = {0}, BHCoor = {0};

		Tmp_Pt.x=CoorX;
		Tmp_Pt.y=CoorY;
		TempDis=PtFixedInfo.FixDis;

		BHCoor=g_pUOneBH->BlockBaseCoor(DwBlockIdx);
		for(k=0;k<((*(TmpDwCtrl->Rd))[DwBlockIdx].RoadCount);k++)//道路資料處理
		{
			/*** 放大道路的矩形框 進行框選 ***/
			Tmp_Rect.left	=	(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdBoundary)[k].left	-	TempDis;
			Tmp_Rect.top	=	(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdBoundary)[k].top		-	TempDis;
			Tmp_Rect.right	=	(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdBoundary)[k].right	+	TempDis;
			Tmp_Rect.bottom	=	(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdBoundary)[k].bottom	+	TempDis;
			if(nuFALSE==nuPointInRect(&Tmp_Pt,&Tmp_Rect))	continue;//有交集才會視為可拉回道路

			for(l=1;l<(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].VertexCount;l++)
			{
				/*** 放大路段的矩形框 進行框選 ***/
//				Tmp_Rect.left=(*m_UOneBH.m_BH.BlockBaseCoor)[DwBlockIdx].x+NURO_MIN((*(*TmpDwCtrl.Rd)[DwBlockIdx].VertexCoor)[(*(*TmpDwCtrl.Rd)[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].x,(*(*TmpDwCtrl.Rd)[DwBlockIdx].VertexCoor)[(*(*TmpDwCtrl.Rd)[DwBlockIdx].RdData)[k].StartVertexIndex+l].x)-TempDis;
//				Tmp_Rect.top=(*m_UOneBH.m_BH.BlockBaseCoor)[DwBlockIdx].y+NURO_MIN((*(*TmpDwCtrl.Rd)[DwBlockIdx].VertexCoor)[(*(*TmpDwCtrl.Rd)[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].y,(*(*TmpDwCtrl.Rd)[DwBlockIdx].VertexCoor)[(*(*TmpDwCtrl.Rd)[DwBlockIdx].RdData)[k].StartVertexIndex+l].y)-TempDis;
//				Tmp_Rect.right=(*m_UOneBH.m_BH.BlockBaseCoor)[DwBlockIdx].x+NURO_MAX((*(*TmpDwCtrl.Rd)[DwBlockIdx].VertexCoor)[(*(*TmpDwCtrl.Rd)[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].x,(*(*TmpDwCtrl.Rd)[DwBlockIdx].VertexCoor)[(*(*TmpDwCtrl.Rd)[DwBlockIdx].RdData)[k].StartVertexIndex+l].x)+TempDis;
//				Tmp_Rect.bottom=(*m_UOneBH.m_BH.BlockBaseCoor)[DwBlockIdx].y+NURO_MAX((*(*TmpDwCtrl.Rd)[DwBlockIdx].VertexCoor)[(*(*TmpDwCtrl.Rd)[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].y,(*(*TmpDwCtrl.Rd)[DwBlockIdx].VertexCoor)[(*(*TmpDwCtrl.Rd)[DwBlockIdx].RdData)[k].StartVertexIndex+l].y)+TempDis;
				Tmp_Rect.left=BHCoor.x+NURO_MIN((*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].x,(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].x)-TempDis;
				Tmp_Rect.top=BHCoor.y+NURO_MIN((*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].y,(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].y)-TempDis;
				Tmp_Rect.right=BHCoor.x+NURO_MAX((*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].x,(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].x)+TempDis;
				Tmp_Rect.bottom=BHCoor.y+NURO_MAX((*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].y,(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].y)+TempDis;
				if(nuFALSE==nuPointInRect(&Tmp_Pt,&Tmp_Rect))	continue;//有交集才會視為可拉回道路

				/*** 進行單行道的角度篩選工作 ***/
				if(b_UseAngle==nuTRUE)//角度判定
				{
					if((*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].OneWay==1)
					{
						TmpAngle = nulAtan2(	(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].x - (*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].x,
															(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].y - (*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].y
															);
						
						if(!CoverAngle(RideAngle,TmpAngle))	continue;
					}
					else if((*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].OneWay==2)
					{
						TmpAngle = nulAtan2(	(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].x - (*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].x,
															(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].y - (*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].y
															);
						if(!CoverAngle(RideAngle,TmpAngle))	continue;
					}
				}
				
				/*** 取得投影值 並判斷rtblock的關係 如果正確 就更新投影數據 ***/
				TempPt[0].x=BHCoor.x+(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].x;
				TempPt[0].y=BHCoor.y+(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].y;
				TempPt[1].x=BHCoor.x+(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].x;
				TempPt[1].y=BHCoor.y+(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].y;
				nuPtFixToLineSeg(Tmp_Pt,TempPt[0],TempPt[1],&Tmp_Coor,&TmpDis);

				if(TmpDis<=PtFixedInfo.FixDis)
				{
					if(PtFixedInfo.FixDis==0 && TmpDis==0)
					{
						if( !(	(Tmp_Coor.x==(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex].x && Tmp_Coor.y==(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex].y) ||
								(Tmp_Coor.x==(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].VertexCount-1].x && Tmp_Coor.y==(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].VertexCount-1].y))) continue;
					}


					if(k == _MAXUNLONG)
					{
						continue;
					}
					TmpRtBID = _MAXUNLONG;
					for(m=0;m<TmpRef->RefStruCount;m++)
					{
						if(	(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdCommData)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].RoadCommonIndex].CityID==(*(TmpRef->pRefStru))[m].CityID &&
							(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdCommData)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].RoadCommonIndex].TownID==(*(TmpRef->pRefStru))[m].TownID	)
						{
							if(!GetFixInfo_CheckRTBID(DwBlockIdx,k,(*(TmpRef->pRefStru))[m].RtAreaID,TmpRtCtrl,TmpRtBIdx,TmpFixNode1,TmpFixNode2)) continue;
							TmpRtBID=(*(TmpRef->pRefStru))[m].RtAreaID;
							break;//資料檔 能少開就少開
						}
					}
					if(TmpRtBID == _MAXUNLONG) 
					{
						continue;
					}
					if(RtBlockID != _MAXUNLONG && RtBlockID!=TmpRtBID)
					{
						continue;
					}
					PtFixedInfo.FixDis=TmpDis;
					TempDis=TmpDis+10;
					PtFixedInfo.MapID=TmpRtCtrl.MapID;
					PtFixedInfo.RTBID=TmpRtBID;
					PtFixedInfo.FixCoor.x=Tmp_Coor.x;
					PtFixedInfo.FixCoor.y=Tmp_Coor.y;
					PtFixedInfo.FixNode1=TmpFixNode1;
					PtFixedInfo.FixNode2=TmpFixNode2;

					/*** 進行判斷行車方向的工作 ***/
					if(b_UseAngle==nuTRUE)//角度判定
					{
						TmpAngle = nulAtan2(	(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].x - (*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].x,
															(*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l].y - (*(*(TmpDwCtrl->Rd))[DwBlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[DwBlockIdx].RdData)[k].StartVertexIndex+l-1].y
															);
					}
				}
			}
		}
		return nuTRUE;
	}

nuBOOL Class_PtFixToRoad::NewPtFixToRoad_NodeID(nuLONG Now_FileIdx,nuLONG Now_BlockIdx,nuLONG Now_RoadIdx,nuLONG CityID,nuLONG TownID,NUROPOINT Now_FixPt,nuULONG RTBlockID,nuLONG RideAngle,nuBOOL b_UseAngle,PTFIXTOROADSTRU &PtFixedInfo)
{
	nuULONG j = 0;
	PtFixedInfo.FixDis = _ROUTEFIXDIS;
	nuFILE *RefFile=NULL;

	#ifdef DEF_MAKETHRROUTE
		if(g_pUOneBH->BHFileIdx!=Now_FileIdx)
		{
			g_pUOneBH->ReleaseBH();
			nuTcscpy(g_pUOneBH->FName, MakeThr_MapPath);
			nuTcscat(g_pUOneBH->FName, nuTEXT(".bh"));
			g_pUOneBH->ReadBH();
			g_pUOneBH->BHFileIdx=Now_FileIdx;
		}

		g_pDB_DwCtrl->InitClass();
		nuTcscpy(g_pDB_DwCtrl->FName, MakeThr_MapPath);
		nuTcscat(g_pDB_DwCtrl->FName, nuTEXT(".rdw"));

		(*ppclsRef)->InitClass();
		nuTcscpy((*ppclsRef)->FName, MakeThr_MapPath);
		nuTcscat((*ppclsRef)->FName, nuTEXT(".ref"));
		if(nuFALSE==(*ppclsRef)->ReadRefFile()){	g_pDB_DwCtrl->Reset();	g_pUOneBH->ReleaseBH();	return nuFALSE;	}

		TmpRtCtrl.InitClass();
		nuTcscpy(TmpRtCtrl.FName, MakeThr_MapPath);
		nuTcscat(TmpRtCtrl.FName, nuTEXT(".rt"));
		if(nuFALSE==TmpRtCtrl.Read_Header()){		g_pDB_DwCtrl->Reset();	(*ppclsRef)->ReleaseRef();	g_pUOneBH->ReleaseBH();	return nuFALSE;	}//讀取RT的Header
	#else
//		if(CityID == 0 && TownID == 0)
		{
			if(g_pUOneBH->BHFileIdx!=Now_FileIdx)
			{
				g_pUOneBH->ReleaseBH();
				nuTcscpy(g_pUOneBH->FName, nuTEXT(".bh"));
				g_pRtFSApi->FS_FindFileWholePath(Now_FileIdx,g_pUOneBH->FName,NURO_MAX_PATH);
				g_pUOneBH->ReadBH();
				g_pUOneBH->BHFileIdx=Now_FileIdx;

				g_pDB_DwCtrl->InitClass();
				nuTcscpy(g_pDB_DwCtrl->FName, nuTEXT(".rdw"));
				g_pRtFSApi->FS_FindFileWholePath(Now_FileIdx,g_pDB_DwCtrl->FName,NURO_MAX_PATH);
			}
		}
		(*ppclsRef)->InitClass();
		nuTcscpy((*ppclsRef)->FName, nuTEXT(".ref"));
		g_pRtFSApi->FS_FindFileWholePath(Now_FileIdx,(*ppclsRef)->FName,NURO_MAX_PATH);
		if(nuFALSE==(*ppclsRef)->ReadRefFile()){	g_pDB_DwCtrl->Reset();	g_pUOneBH->ReleaseBH();	return nuFALSE;	}

		TmpRtCtrl.InitClass();
		nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
		g_pRtFSApi->FS_FindFileWholePath(Now_FileIdx,TmpRtCtrl.FName,NURO_MAX_PATH);
		if(nuFALSE==TmpRtCtrl.Read_Header()){		g_pDB_DwCtrl->Reset();	(*ppclsRef)->ReleaseRef();	g_pUOneBH->ReleaseBH();	return nuFALSE;	}//讀取RT的Header
	#endif

	NewGetFixInfo_NodeID(Now_BlockIdx,Now_RoadIdx,CityID,TownID,Now_FixPt,RTBlockID,RideAngle,b_UseAngle,
		PtFixedInfo,g_pDB_DwCtrl,*ppclsRef,TmpRtCtrl);

	//清除中間過程所用的所有資料
	for(j=0;j<TmpRtCtrl.Rt_Block_Count;j++){
		TmpRtCtrl.ReSetRTB(j);
	}
	TmpRtCtrl.Remove_Header();
	(*ppclsRef)->ReleaseRef();
	g_pDB_DwCtrl->Reset();

	WorkNow=nuFALSE;

	if((PtFixedInfo.FixDis) >= _ROUTEFIXDIS)
	{	PtFixedInfo.FixDis=-1;	PtFixedInfo.FixCoor.x=-1;	PtFixedInfo.FixCoor.y=-1;	PtFixedInfo.RTBID=-1;	return nuFALSE;	}
	else
	{	return nuTRUE;	}
}

nuBOOL Class_PtFixToRoad::NewPtFixToRoad(nuLONG Now_FileIdx,nuLONG Now_BlockIdx,nuLONG Now_RoadIdx,NUROPOINT Now_FixPt,nuULONG RTBlockID,nuLONG RideAngle,nuBOOL b_UseAngle,PTFIXTOROADSTRU &PtFixedInfo)
{
	nuULONG j = 0;
	PtFixedInfo.FixDis = _ROUTEFIXDIS;
	nuFILE *RefFile=NULL;
	nuBOOL bFix = nuTRUE;
	#ifdef DEF_MAKETHRROUTE
		g_pUOneBH->InitClass();
		nuTcscpy(g_pUOneBH->FName, MakeThr_MapPath);
		nuTcscat(g_pUOneBH->FName, nuTEXT(".bh"));
		g_pUOneBH->ReadBH();

		g_pDB_DwCtrl->InitClass();
		nuTcscpy(g_pDB_DwCtrl->FName, MakeThr_MapPath);
		nuTcscat(g_pDB_DwCtrl->FName, nuTEXT(".rdw"));
		if(nuFALSE == g_pDB_DwCtrl->Read_Header())
		{ 
			g_pDB_DwCtrl->Reset();	
			g_pUOneBH->ReleaseBH();	
			return nuFALSE;	
		}

		(*ppclsRef)->InitClass();
		nuTcscpy((*ppclsRef)->FName, MakeThr_MapPath);
		nuTcscat((*ppclsRef)->FName, nuTEXT(".ref"));
		if(nuFALSE == (*ppclsRef)->ReadRefFile())
		{	
			g_pDB_DwCtrl->Reset();	
			g_pUOneBH->ReleaseBH();	
			return nuFALSE;	
		}

		TmpRtCtrl.InitClass();
		nuTcscpy(TmpRtCtrl.FName, MakeThr_MapPath);
		nuTcscat(TmpRtCtrl.FName, nuTEXT(".rt"));
		if(nuFALSE == TmpRtCtrl.Read_Header())
		{		
			g_pDB_DwCtrl->Reset();	
			(*ppclsRef)->ReleaseRef();	
			g_pUOneBH->ReleaseBH();	
			return nuFALSE;	
		}//讀取RT的Header
	#else
		g_pUOneBH->InitClass();
		nuTcscpy(g_pUOneBH->FName, nuTEXT(".bh"));
		g_pRtFSApi->FS_FindFileWholePath(Now_FileIdx,g_pUOneBH->FName,NURO_MAX_PATH);
		g_pUOneBH->ReadBH();

		g_pDB_DwCtrl->InitClass();
		nuTcscpy(g_pDB_DwCtrl->FName, nuTEXT(".rdw"));
		g_pRtFSApi->FS_FindFileWholePath(Now_FileIdx,g_pDB_DwCtrl->FName,NURO_MAX_PATH);
		if(nuFALSE == g_pDB_DwCtrl->Read_Header())
		{ 
			g_pDB_DwCtrl->Reset();	
			g_pUOneBH->ReleaseBH();	
			return nuFALSE;	
		}

		(*ppclsRef)->InitClass();
		nuTcscpy((*ppclsRef)->FName, nuTEXT(".ref"));
		g_pRtFSApi->FS_FindFileWholePath(Now_FileIdx,(*ppclsRef)->FName,NURO_MAX_PATH);
		if(nuFALSE == (*ppclsRef)->ReadRefFile())
		{	
			g_pDB_DwCtrl->Reset();	
			g_pUOneBH->ReleaseBH();	
			return nuFALSE;	
		}

		TmpRtCtrl.InitClass();
		nuTcscpy(TmpRtCtrl.FName, nuTEXT(".rt"));
		g_pRtFSApi->FS_FindFileWholePath(Now_FileIdx,TmpRtCtrl.FName,NURO_MAX_PATH);
		if(nuFALSE == TmpRtCtrl.Read_Header())
		{		
			g_pDB_DwCtrl->Reset();	
			(*ppclsRef)->ReleaseRef();	
			g_pUOneBH->ReleaseBH();	
			return nuFALSE;	
		}//讀取RT的Header
	#endif

	//深入每一個Block去判斷資料並取得投影值
	if(g_pDB_DwCtrl->Read_BlockData(Now_BlockIdx))
	{
		bFix = NewGetFixInfo(Now_BlockIdx,Now_RoadIdx,Now_FixPt,RTBlockID,RideAngle,b_UseAngle,PtFixedInfo,g_pDB_DwCtrl,*ppclsRef,TmpRtCtrl);
		g_pDB_DwCtrl->Remove_BlockData(Now_BlockIdx);
	}

	//清除中間過程所用的所有資料
	for(j=0;j<TmpRtCtrl.Rt_Block_Count;j++)
	{
		TmpRtCtrl.ReSetRTB(j);
	}
	TmpRtCtrl.Remove_Header();
	(*ppclsRef)->ReleaseRef();
	g_pDB_DwCtrl->Reset();
	g_pUOneBH->ReleaseBH();

	WorkNow = nuFALSE;

	if((PtFixedInfo.FixDis) >= _ROUTEFIXDIS || !bFix)
	{	
		PtFixedInfo.FixDis    = -1;	
		PtFixedInfo.FixCoor.x = -1;	
		PtFixedInfo.FixCoor.y = -1;	
		PtFixedInfo.RTBID	  = -1;
		return nuFALSE;
	}
	else
	{	
		return nuTRUE;	
	}
}

	nuBOOL Class_PtFixToRoad::NewGetFixInfo(nuLONG Now_BlockIdx,nuLONG Now_RoadIdx,NUROPOINT Now_FixPt,nuULONG RtBlockID,nuLONG RideAngle,nuBOOL b_UseAngle,PTFIXTOROADSTRU &PtFixedInfo,class UONERDwCtrl *TmpDwCtrl,RefCtrl *TmpRef,UONERtCtrl &TmpRtCtrl)
	{
		nuLONG TmpRtBIdx = 0, TmpAngle = 0;
		nuUINT m = 0;
		nuULONG TmpFixNode1 = 0, TmpFixNode2 = 0, TmpRtBID = 0;
		TmpRtBID = _MAXUNLONG;

		nuLONG l = 0, TmpDis = 0;
		NUROPOINT Tmp_Coor = {0}, TempPt[2] = {0}, BHCoor = {0};

		BHCoor=g_pUOneBH->BlockBaseCoor(Now_BlockIdx);
		//找出行車方向節點
		if(b_UseAngle==nuTRUE)//角度判定
		{
			for(l=1;l<(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].VertexCount;l++)
			{			
				TempPt[0].x=	BHCoor.x+(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex+l-1].x;
				TempPt[0].y=	BHCoor.y+(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex+l-1].y;
				TempPt[1].x=	BHCoor.x+(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex+l].x;
				TempPt[1].y=	BHCoor.y+(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex+l].y;
				nuPtFixToLineSeg(Now_FixPt,TempPt[0],TempPt[1],&Tmp_Coor,&TmpDis);

				if(TmpDis<=PtFixedInfo.FixDis)
				{
					if(PtFixedInfo.FixDis==0 && TmpDis==0)
					{
						if( !(	(Tmp_Coor.x==(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex].x && Tmp_Coor.y==(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex].y) ||
								(Tmp_Coor.x==(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex+(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].VertexCount-1].x && Tmp_Coor.y==(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex+(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].VertexCount-1].y))) continue;
					}
					PtFixedInfo.FixDis=TmpDis;

					TmpAngle = nulAtan2(	(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex+l].x - (*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex+l-1].x,
														(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex+l].y - (*(*(TmpDwCtrl->Rd))[Now_BlockIdx].VertexCoor)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].StartVertexIndex+l-1].y
														);
				}
			}
		}


		for(m=0;m<TmpRef->RefStruCount;m++)
		{
			if(	(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdCommData)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].RoadCommonIndex].CityID==(*(TmpRef->pRefStru))[m].CityID &&
				(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdCommData)[(*(*(TmpDwCtrl->Rd))[Now_BlockIdx].RdData)[Now_RoadIdx].RoadCommonIndex].TownID==(*(TmpRef->pRefStru))[m].TownID	)
			{
				if(!GetFixInfo_CheckRTBID(Now_BlockIdx,Now_RoadIdx,(*(TmpRef->pRefStru))[m].RtAreaID,TmpRtCtrl,TmpRtBIdx,TmpFixNode1,TmpFixNode2)) continue;
				TmpRtBID=(*(TmpRef->pRefStru))[m].RtAreaID;
				break;//資料檔 能少開就少開
			}
		}
		if(TmpRtBID == _MAXUNLONG)
		{
			return nuFALSE;
		}
		PtFixedInfo.FixDis=0;
		PtFixedInfo.MapID=TmpRtCtrl.MapID;
		PtFixedInfo.RTBID=TmpRtBID;
		PtFixedInfo.FixCoor.x=Now_FixPt.x;
		PtFixedInfo.FixCoor.y=Now_FixPt.y;
		PtFixedInfo.FixNode1=TmpFixNode1;
		PtFixedInfo.FixNode2=TmpFixNode2;	
		return nuTRUE;
	}

	nuBOOL Class_PtFixToRoad::NewGetFixInfo_NodeID(nuLONG Now_BlockIdx,nuLONG Now_RoadIdx,nuLONG CityID,nuLONG TownID,NUROPOINT Now_FixPt,nuULONG RtBlockID,nuLONG RideAngle,nuBOOL b_UseAngle,PTFIXTOROADSTRU &PtFixedInfo,class UONERDwCtrl *TmpDwCtrl,RefCtrl *TmpRef,UONERtCtrl &TmpRtCtrl)
	{
		nuLONG  TmpRtBIdx = 0;
		nuUINT  m = 0;
		nuULONG TmpFixNode1 = 0, TmpFixNode2 = 0, TmpRtBID = 0;
		nuULONG NowCityID = CityID,NowTownID = TownID;
		TmpRtBID = _MAXUNLONG;
		//if(NowCityID == 0 && NowTownID == 0)
		{
			TmpDwCtrl->GetCityTownID(Now_BlockIdx,Now_RoadIdx,NowCityID,NowTownID);
		}

		for(m=0;m<TmpRef->RefStruCount;m++){
			if(	NowCityID==(*(TmpRef->pRefStru))[m].CityID && NowTownID==(*(TmpRef->pRefStru))[m].TownID ){
				if(!GetFixInfo_CheckRTBID(Now_BlockIdx,Now_RoadIdx,(*(TmpRef->pRefStru))[m].RtAreaID,TmpRtCtrl,TmpRtBIdx,TmpFixNode1,TmpFixNode2)) continue;
				TmpRtBID=(*(TmpRef->pRefStru))[m].RtAreaID;
				break;//資料檔 能少開就少開
			}
		}
		if(TmpRtBID == _MAXUNLONG)
		{
			return nuFALSE;
		}
		PtFixedInfo.FixDis=0;
		PtFixedInfo.MapID=TmpRtCtrl.MapID;
		PtFixedInfo.RTBID=TmpRtBID;
		PtFixedInfo.FixNode1=TmpFixNode1;
		PtFixedInfo.FixNode2=TmpFixNode2;	
		return nuTRUE;
	}

	nuBOOL Class_PtFixToRoad::GetFixInfo_CheckRTBID(nuLONG DWBlockIdx,nuLONG DWRoadIdx,nuULONG RTBlockID,UONERtCtrl &TmpRtCtrl,nuLONG &RTBIdx,nuULONG &FixNodeID1,nuULONG &FixNodeID2)
	{
		RTBIdx=-1;
		nuLONG TmpRTBIdx=-1;
		nuULONG i;
		nuBOOL Rebool=nuFALSE;
		/*** 確認RTB值 並取得索引表資料 ***/
		for(i=0;i<TmpRtCtrl.Rt_Block_Count;i++)
		{	if((*TmpRtCtrl.Rt_Block_ID_List)[i]==RTBlockID)	TmpRTBIdx=i;	}//確認BlockIdx值
		if(TmpRTBIdx==-1) return nuFALSE;

		if((*TmpRtCtrl.TmpRtB)[TmpRTBIdx].RtBIdx==NULL)
		{	if(!TmpRtCtrl.ReadRTB(TmpRTBIdx,nuFALSE,nuTRUE))	{	TmpRtCtrl.ReSetRTB(TmpRTBIdx); return nuFALSE;	}	}

		/*** 比對資料 取得NodeID 這裡若能改善 找點的時間應該可以減少一秒鐘 ***/
		for(i=0;i<(*TmpRtCtrl.TmpRtB)[TmpRTBIdx].RtBHeader.TotalRoadCount;i++){
			if((*(*TmpRtCtrl.TmpRtB)[TmpRTBIdx].RtBIdx)[i].BlockIdx==DWBlockIdx && 
				(*(*TmpRtCtrl.TmpRtB)[TmpRTBIdx].RtBIdx)[i].RoadIdx==DWRoadIdx){
				Rebool=nuTRUE;
				RTBIdx=TmpRTBIdx;
				FixNodeID1=(*(*TmpRtCtrl.TmpRtB)[TmpRTBIdx].RtBIdx)[i].FNodeID;
				FixNodeID2=(*(*TmpRtCtrl.TmpRtB)[TmpRTBIdx].RtBIdx)[i].TNodeID;
				break;
			}
		}
		return Rebool;
	}


/*
nuBOOL Class_PtFixToRoad::CoverAngle(double TargetAngle,double SourceAngle)
{
	double TmpDiffAngle;
	if(TargetAngle>(2*PI)) TargetAngle -= (2*PI);
	if(TargetAngle<(-2*PI)) TargetAngle += (2*PI);
	
	if(SourceAngle>(2*PI)) SourceAngle -= (2*PI);
	if(SourceAngle<(-2*PI)) SourceAngle += (2*PI);

	TmpDiffAngle=TargetAngle-SourceAngle;

	if( (-PI/4)<TmpDiffAngle && TmpDiffAngle<(PI/4) )
	{	return nuTRUE;	}
	else if( TmpDiffAngle<((-2*PI)+(PI/4)) )
	{	return nuTRUE;	}
	else if( ((2*PI)-(PI/4))<TmpDiffAngle )
	{	return nuTRUE;	}
	else
	{	return nuFALSE;	}
}
*/
nuBOOL Class_PtFixToRoad::CoverAngle(nuLONG TargetAngle,nuLONG SourceAngle)
{
	nuLONG TmpDiffAngle = 0;
	if(TargetAngle>360) TargetAngle -= 360;
	if(TargetAngle<-360) TargetAngle += 360;
	
	if(SourceAngle>360) SourceAngle -= 360;
	if(SourceAngle<360) SourceAngle += 360;

	TmpDiffAngle=TargetAngle-SourceAngle;

	if( -45<TmpDiffAngle && TmpDiffAngle<45 )
	{	return nuTRUE;	}
	else if( TmpDiffAngle<(-360+45) )
	{	return nuTRUE;	}
	else if( (360-45)<TmpDiffAngle )
	{	return nuTRUE;	}
	else
	{	return nuFALSE;	}
}

nuLONG Class_PtFixToRoad::GetBlockID(nuLONG CoorX,nuLONG CoorY)
{
	nuLONG TmpX=(CoorX+(XBLOCKNUM/2)*EACHBLOCKSIZE)/EACHBLOCKSIZE;
	nuLONG TmpY=((CoorY+(YBLOCKNUM/2)*EACHBLOCKSIZE)/EACHBLOCKSIZE)*XBLOCKNUM;
	return (TmpX+TmpY);
}