// NaviData.h: interface for the CNaviData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVIDATA_H__9BBB1B23_8BF4_4EAD_8E5D_62C58133055E__INCLUDED_)
#define AFX_NAVIDATA_H__9BBB1B23_8BF4_4EAD_8E5D_62C58133055E__INCLUDED_

#include "NuroDefine.h"	// Added by ClassView
#include "NuroConstDefined.h"
#include "NuroEngineStructs.h"
#include "nuRouteData.h"
#include "NaviLineCollect.h"
#include "NuroNaviConst.h"
#include "LIF.h"
#include "MWP.h"
#include "SPT.h"
#include "LaneGuide.h"
#define  TMCMAXDIS 100000
//------------------SDK-------------//
#include "NURO_DEF.h"
//-----------------SDK-------------//
/*typedef struct tagSOUNDPLAY
{
	nuBYTE nPlayType;
	nuWORD nDistance;
	nuWORD nRoadtype;
	nuWORD nTurning;
	nuWORD nOthers;
	nuWCHAR* pName;
	nuWORD nLen;
}SOUNDPLAY, *PSOUNDPLAY;*/

#define  BOTH_PLAY_DIS 160

typedef struct tagNAVINSS
{
	PNAVIFROMTOCLASS	pNFTC;
	PNAVIRTBLOCKCLASS	pNRBC;
	PNAVISUBCLASS		pNSS;
	nuULONG		nIndex;
}NAVINSS, *PNAVINSS;

typedef struct tagSOUNDPLAY
{
	nuLONG	nDistance;
	nuLONG	nTurning;
	nuLONG	nChangeCode;
	nuWCHAR    wsRoadName[MAX_LISTNAME_NUM];
	NAVINSS NaviInfo;
}SOUNDPLAY, *PSOUNDPLAY;

typedef struct TagSEARCHIUDGEINFO
{
	PNAVIFROMTOCLASS	pNFTC;
	PNAVIRTBLOCKCLASS   pNRBC;
	nuINT		index;   //NSS index
	nuINT		nFindCrossNum;
	nuBOOL	bFindNextCross;
	nuBOOL	bFindNextTown;
	nuBOOL	bIngore;    
	nuBOOL    bInTraf;
	nuBOOL	bIngoreNext;
	nuBOOL    bFindNextNFTC;
	nuBYTE	nZoomNum;
	nuBYTE  nTrafNum:4;
	nuBYTE  nFindHighwayCross:4;
	nuLONG	nLenToNext;
	nuLONG    nSecLen;
	nuLONG    n3DPic;
	nuLONG    nDisToNextNFTC;
	SOUNDPLAY SoundPlayFirst;
	SOUNDPLAY SoundPlayNext;
}SEARCHJUDGEINFO,*PSEARCHJUDGEINFO;

typedef struct tagHIGHWAYLASTINFO
{
	nuLONG    nDWIndex;
	nuBOOL      bEndPt;
	nuDWORD   nBlkIndex;
	nuDWORD   nRdIndex;
	nuLONG      nAngle;
	NUROPOINT ptMap;
	NUROPOINT ptNextCoor;
}HIGHWAYLASTINFO,*PHIGHWAYLASTINFO;


class CNaviData  
{
public:
	typedef struct tagPOINTINNAVILINE
	{
		PNAVIFROMTOCLASS	pNFTC;
		PNAVIRTBLOCKCLASS	pNRBC;
		nuDWORD				nSubIdx;
		nuLONG				nPtIdx;
		nuroPOINT			ptMapped;
	}POINTINNAVILINE, *LPPOINTINNAVILINE, *PPOINTINNAVILINE;
	typedef struct tagCROSSPOINT
	{
		PNUROPOINT  ptArray;
		nuBYTE	    nPtNum;
		nuBYTE      nCenterIndex; 
	}CROSSPOINT,  *PCROSSPOINT;
public:
	nuBOOL ChangeIcon(nuLONG &nIcon);
	nuBOOL bRestNaviData2();
	nuBOOL FixOverpassRoad(  PNAVINSS pNowNSS,  PNAVINSS  pNextNSS);
	nuBOOL AddExternStr(nuLONG nChangeCode,nuWCHAR *wsSource,nuINT nStrLen);
	nuLONG GetRoutingListCodeEMG(PNAVISUBCLASS pNowNSS, PNAVISUBCLASS pNextNSS);
	nuLONG GetArrowLen();
	nuBOOL GetCarInfoForCCD( PCCDINFO pCcdInfo);
	nuBOOL DrawArrow2(nuBYTE bZoom);
	nuBOOL ColHighWayCrossData(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo,nuLONG nLenToNextCross);
	nuBOOL GetRtListNew();
	nuBYTE JudgeZoomRoad1();
	nuLONG GetNssFirstAngle(PNAVISUBCLASS pNss);
	nuBOOL AddNameStr(nuWCHAR *wsName);
	nuBOOL ChangNCCName(PNAVIFROMTOCLASS pNFTC,PNAVIRTBLOCKCLASS pNRBC );
	
	nuBOOL GetPTSName(nuWORD nDwIdx, nuLONG nNameAddr, nuPVOID pNameBuf, nuWORD nBufLen);
	nuBYTE JudgeRoadType(  PSEEKEDROADDATA pNowRoadInfo );
	nuBOOL GetHighExternList();
	nuLONG GetVoiceType(nuLONG nNowVoiceType, nuLONG nNowClass, nuLONG nNextVoiceType, nuLONG nNextClass);
	nuBOOL GetHighWayExtern(PCROSSFOLLOWED pFollowCross, nuBOOL bIngoreFirst, nuBYTE& nNumGet,SHOWNODE *NodeArray, nuBYTE nArrayNum);
	nuBOOL bColCombinCrossData(NAVINSS* pNowNSSInfo, NAVINSS* pNextNSSInfo,nuLONG &nLenToNextCross,nuBOOL bReCalNextCross = true );
	nuBOOL bJudgeHighWayCross( PNAVINSS pNowNSS, PNAVINSS pNextNSS );
	nuLONG GetRoutingListCodeNew(PNAVISUBCLASS pNowNSS, PNAVISUBCLASS pNextNSS);
	nuBOOL SelPt(PCROSSPOINT pCrossArrayIn, PCROSSPOINT pCrossArrayOut, nuLONG nLenDraw,nuBOOL bChangeCross = true );
	nuBOOL GetPointFromNSS2(PNAVISUBCLASS pNSS, nuBOOL bFrist, PNUROPOINT pPoint, nuBYTE &nNowPtNum, nuLONG &nRemainLen);
	nuBOOL GetPointFromNSS( PNAVISUBCLASS pNSS,nuBOOL bFrist,PNUROPOINT pPoint,nuBYTE& nNowPtNum,nuLONG &nRemainLen);
	nuBOOL GetNaviArrowPoint(NAVINSS *pNaviNss, CROSSNODE *pCrossNode);
	nuBOOL GetHighWayList();
	nuBOOL JudgeCross(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo);
	nuBOOL JudgeArrowNSS(nuBYTE bZoom);
	nuBOOL DrawArrow( nuBYTE bZoom );
	nuUINT GetRoutingList( PROUTINGLISTNODE pRTlist, nuINT nNum, nuINT nIndex );
	nuVOID SoundAfterWork(nuWCHAR * wsSoundName, NAVINSS* pNowNSSInfo);//added by daniel for LCMM 20120111
	nuBOOL GetCarInfoForKM(PKMINFO pKMInfo);
	nuVOID Set3DPAfterWork(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo);
	CNaviData();
	virtual ~CNaviData();

	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL ResetNaviData();
	nuVOID FreeNaviData();
	
	nuBOOL	GetSimuData(nuBOOL bReStart, nuLONG nLen, nuPVOID lpOut);
	nuBOOL  GetSimulationData(nuBOOL bReStart, nuLONG nLen, nuPVOID lpOut);
	nuBOOL  DrawNaviRoad(nuBYTE bZoom);
	nuBOOL	DrawNaviRoad2(nuBYTE bZoom);
//	nuBOOL    DrawZoomNaviRoad();
	nuUINT  PointToNaviLine(nuLONG x, nuLONG y, nuLONG nCoverDis, nuSHORT nAngle, PSEEKEDROADDATA pRoadData);
	nuPVOID CheckScreenRoad(nuroRECT rtRange, nuBOOL bNow = nuFALSE);
	nuBOOL	ColNaviInfo();
	nuBOOL	PlayNaviSound( );
	nuBYTE  JudgeZoomRoad( );

	nuBOOL	ColNaviLineZ( nuBOOL bNaviPtStart = nuFALSE);
	//
	nuBOOL NewGetRoutingListZ(PROUTINGLISTNODE pRTlist, nuINT nNum, nuINT nIndex, nuBYTE nType);

#ifdef LCMM
	nuLONG TMCRoute(TMC_ROUTER_DATA *pTMCData, nuLONG TMCDataCount, TARTGETINFO NewStart, nuLONG RouteRule);
	nuVOID EstimateTimeAndDistance(nuLONG *TargetDis, nuLONG *TargetTime);
	nuVOID EstimateTimeAndDistanceLU(nuLONG &TargetDis, nuLONG &TargetTime, nuLONG TargetIdx);
	nuVOID ChooseRouteLineth(nuLONG RouteLineth);
	nuVOID DisplayRoutePath(nuLONG SelectRoutePath);
	nuVOID JudegeLaneGuideInfo();
#endif
	nuVOID SetSimulation();
	nuBOOL GetSimuTunnelData(nuBOOL bReStart, nuLONG nLen, nuPVOID lpData);
	nuBOOL TMCNaviPathData();
	nuBOOL InitTMCNaviPathData();
	nuVOID FreeTMCNaviPathData();
//---------------------SDK-------------------------------------------------------//
	nuPVOID GetRoutingListEx(PROUTINGLISTNODE *ppRTList, nuINT *pNum, nuINT *pIndex);	
//---------------------SDK-------------------------------------------------------//
public:
	CLIF m_Lif;
	CLaneGuide	m_CLaneGuide;
protected:
	nuVOID FreeLocalRouteData();
	nuVOID AddLocalRouteData(PNAVIRTBLOCKCLASS pNRBC);

	nuBOOL IsRTBUnroute(PNAVIRTBLOCKCLASS pNRBC);

protected:
	nuBOOL ColNextVoice(PNAVINSS pNowNSSInfo,PNAVINSS pNextNSSInfo,nuLONG nLenToNext, SOUNDPLAY& SoundPlay);
	POINTINNAVILINE		m_ptSimu;
	POINTINNAVILINE		m_ptNavi;
	NAVINSS				m_ptNextCross;
	NAVINSS				m_ptLastNextCross;
	PLOCALROUTEDATA		m_pLocalRouteData;
	SEARCHJUDGEINFO		m_struSeachInfo;
	CMWP				m_MwpFile;
	CSPT                m_SptFile;
private:
	nuBOOL bSimMode();
	nuBOOL bChangeSymbols( nuWCHAR *wsRoadName,nuWCHAR *wsOut);
	nuBYTE GetTurnFlag(PNAVISUBCLASS pNowNSS, PNAVISUBCLASS pNextNSS);
	nuBOOL GetPreNSS(NAVINSS NowNSSInfo,PNAVINSS pPreNSS);
	nuBOOL JudgeDraw3DPic();
	nuBOOL GetRTBName(nuLONG MapIdx,nuLONG RTBID,nuWCHAR *RTBName);
	nuBOOL ColZoomCrossData(NAVINSS NowNSSInfo,NAVINSS NextNSSInfo);
	nuBOOL ColSecCrossData(NAVINSS NowNSSInfo,NAVINSS NextNSSInfo);
	nuINT  GetCarAngle();
	nuBOOL CombinedSeach(PSEARCHJUDGEINFO pSeachInfo);
	nuBOOL JudgeDis(PSOUNDPLAY pSoundPlay,  nuWORD *pDisPlay, nuLONG nDistance, nuBOOL *pbFastWay);
	
	nuBOOL GetRoadName(PNAVISUBCLASS pNowNSS,nuWCHAR *sRoadName);
	nuVOID AddListNode(SHOWNODE shownode, nuBOOL bInOutHighWay);
	PNAVISUBCLASS m_pBeforeNSS;
	nuBOOL GetNextNSS( NAVINSS NowNSSInfo ,PNAVINSS pNextNSSInfo);
	nuVOID GetRTList();
	nuBOOL GetCrossData(nuWCHAR *wsName,PNAVISUBCLASS pNowNSS,PNAVISUBCLASS pNextNSS, nuLONG nChangCode,PNAVIRTBLOCKCLASS pNRBC);
	nuBOOL ColNextCrossData(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo,nuLONG nLenToNextCross);
	nuBOOL ColNextCityTown(PNAVISUBCLASS pNowNSS, PNAVISUBCLASS pNextNSS, nuLONG nLenToNextTown);
	nuLONG GetRoutingListCode(PNAVISUBCLASS pNowNSS, PNAVISUBCLASS pNextNSS);
	nuBOOL CheckIsRVoiceL(nuLONG RoutingListCode);
	nuVOID CalMinAndMaxCoor_D(NUROPOINT point);//added by daniel
	nuLONG GetTurnFlagNum(nuLONG VoiceType,PNAVISUBCLASS NowNSSInfo);//2012/10/22 HUDCallBack
	nuBOOL JudgeSameRoadName(NAVINSS NowNSSInfo, NAVINSS NextNSSInfo);
	nuVOID SoundNamefilter(nuWCHAR *wsRoadName, nuINT nWordCount);
	nuLONG CalDraw3DPic(NAVINSS NowNSSInfo, nuDWORD nDisNowToEnd);
#ifdef ECI
	nuVOID JudgeEci3DPic(NAVINSS NowNSSInfo, nuDWORD nDisNowToEnd);
	nuLONG JudgeDraw3DPic(NAVINSS NowNSSInfo, nuDWORD nDisNowToEnd,NAVINSS &PicNSSInfo);
	nuBOOL ColNextDis(NAVINSS NowNSSInfo,nuDWORD &nPreDis);
#endif
	nuBOOL SetNaviDTIColor(nuUINT trafficevent, NAVILINESETTING &RSet/* = NULL*/);
	nuVOID TMCNaviCrowdDis();
//-----------------SDK----------------------------------------------------------------------------------------//
	nuVOID Initial_D();
	nuBOOL GetRouteListCount_D(nuINT &NuroRoadCount, nuINT &ShowRoadCount);
	nuLONG GetRouteListData_D(NURO_NAVI_LIST *pShowRoadBuf, nuINT ShowRoadCount, nuINT ShowRoadListIndex);
	nuBOOL GetRoadListIndex_D(nuINT &DisplayIndex, nuINT &nRoadListIndex);
//-----------------SDK----------------------------------------------------------------------------------------//

	nuBOOL  m_bHasPlayVoice[3];
	NAVINSS m_NaviNSS;
	NAVINSS m_NaviNextNSSIgnore;
	NAVINSS m_NaviForZoom;
	NAVINSS m_NaviForZoomPre;
	nuLONG	m_nLastCrossToTar;
	nuLONG	m_nBackCrossToTar;
	nuLONG	m_nHighSpeed ;
	nuLONG	m_nSlowSpeed ;
	nuLONG	m_nNowLen;
	nuLONG	m_nNextLen;
	nuBYTE	m_nTraffNum;
	NAVINSS m_NaviForArrow;
	nuBYTE  m_nArrowCrossNum;
	nuBYTE  m_nMaxLocalTimers;
	nuDWORD m_nPreTime;
	PNAVIRTBLOCKCLASS	m_pNRBC;
	NAVINSS				m_ptNext;

	nuLONG	m_nNaviLineType;
	nuBOOL    m_bInPassNode;
	//add 09-01-21
	NUROPOINT m_ptDraw[CROSS_ARROW_POINT_NUM];
	CROSSPOINT m_CrossPoint;
	nuBOOL m_bDrawBefoeArrow;
	nuBYTE m_nCrossIndexInSouce;

	nuLONG m_nLenSlowWayAdd;
	nuLONG m_nLenHighWayAdd;
	nuLONG m_nLenRTB;
	nuLONG m_nNowSpeed;

    #define SAVENODENUM   3 
	HIGHWAYLASTINFO m_StrHighWayLast;
	HIGHWAYLASTINFO m_StrHighWayCmp;
	SHOWNODE m_NodeArr[SAVENODENUM];
	nuBYTE m_nSaveNum;
	nuLONG m_nHeighwayExtern; //控制是否有扩展高速列表的功能
	nuDWORD m_nSaveIndex[SAVENODENUM];
	nuBOOL m_bFirstGetMRouteData;

	nuLONG m_nTHCTimer;
	nuLONG m_nFristTotalLen;
	nuLONG m_nFristTotalTime;
	nuLONG m_nArrowLineWidth1;
	nuLONG m_nArrowLineWidth2;
	nuLONG m_nShowHighWayPic;
	//--------------Set Navi Line Color---------------------//
	nuLONG	m_nNaviLineRed;
	nuLONG	m_nNaviLineGreen;
	nuLONG	m_nNaviLineBlue;
	nuLONG  m_nNaviArrawRed;
	nuLONG  m_nNaviArrawGreen;
	nuLONG  m_nNaviArrawBlue;
	//--------------Set Navi Line Color---------------------//

	nuBOOL m_bFindHighwayRealPic;
	nuLONG m_nHighwayRealPic;
	nuLONG m_nGoNavi;
	nuLONG m_nBmpArrowLen;
	nuBYTE m_nPreScal;
	nuLONG m_nSameRoadLen;
	nuBOOL m_bFirstSound;
	nuBOOL m_bCombinDrawArrow;
	nuDWORD m_nCombinLen;
	nuBOOL m_bEstimation;
    nuBOOL m_bInFreeWay;
	nuBOOL m_bFirstSpeed;
	nuLONG m_lCarSpeed;
	nuLONG m_nRoyaltekSleep;
#ifdef VALUE_EMGRT
	nuBOOL m_bSoundAfore;
	nuBOOL m_bFreeWayFlag;
#endif
//<<<<<<< NaviData.h
//2010.04.06
	nuLONG m_nSimuRdRealLen;
	nuLONG m_nSimuRdSegLen;
	nuLONG m_nSimuRdNowLen;
//=======

	nuLONG XShift;
	nuLONG YShift;
//>>>>>>> 1.15
	nuLONG m_nScalShowArrow; 
	
	nuLONG m_lTHCRealDis[5];
	nuLONG m_lTHCWeightDis[5];
	nuBOOL m_bFirstCalDis;//added by daniel
#ifdef LCMM
	nuLONG m_lChoosePath;
	nuBOOL m_bSelectRouteRule;
    nuLONG  m_nNowDis;//20121214
#endif
#ifdef ZENRIN
	NAVINSS	m_pTempNSSInfo;
	NAVINSS m_pVoiceNSSInfo;
	nuBOOL  m_bRoadCross;
	nuBOOL  m_bVoiceRoadCross;
#endif
public:
	NUROPOINT m_PtRouteEndMin;//added by daniel
	NUROPOINT m_PtRouteEndMax;
	nuWORD    m_lTMCMemIdx;
	//===========SDK===============//
	nuLONG    m_nDisplayCount;
	nuLONG    m_nRoadListCount;
	PNURO_NAVI_LIST m_pShowRoadBuf;
	//===========SDK==============//
};

#endif // !defined(AFX_NAVIDATA_H__9BBB1B23_8BF4_4EAD_8E5D_62C58133055E__INCLUDED_)































































