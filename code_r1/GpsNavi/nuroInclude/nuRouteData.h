#ifndef _NURO_ROUTE_DATA_20080521
#define _NURO_ROUTE_DATA_20080521

#include "NuroDefine.h"

typedef struct tagNAVICROSSCLASS
{
	nuWORD	CrossBlockIdx;//Block index
	nuWORD	CrossRoadIdx;//Road index in block
	long	CrossRoadNameAddr;//Road Name address
	short	CrossAngle;//
	nuWORD  nFlag:4;//0:³õÊ¼»¯ 1:È¡µÄ³É¹¦ 2:»ñÈ¡Ê§°Ü
	nuWORD  nRoadNameLen:12;
	long	CrossVoiceType;//
	long	CrossClass;//
}NAVICROSSCLASS, *PNAVICROSSCLASS;

typedef struct tagNAVISUBCLASS
{
	nuWORD	BlockIdx;//©Ò¦bªºDw°Ï¶ô
	nuWORD	RoadIdx;//¹D¸ôID
	nuLONG	RoadNameAddr;//¹D¸ô¦WºÙ¦ì¸m
	nuDWORD	RoadLength;//¹D¸ôªø«×
	nuLONG	RoadVoiceType;//»y­µ¼½³ø¸ô«¬

	nuDWORD	RoadClass : 16;
	nuDWORD    SPTFlag: 1; //SignPost Flag
	nuDWORD	 LaneGuideFlag:1; //¨®¹D°T®§ºX¼Ð
	nuDWORD    Reserve: 14; //Reserve
	nuDWORD CrossCount;//¸ô¬q²×ÂIªº¥æ¤e¸ô¤f¼Æ(¥i³q¦æ)
	nuWORD	CityID;//«°¥«½X
	nuWORD	TownID;//¶mÂí½X
	NAVICROSSCLASS *NCC;//¥æ¤e¸ô¤f¸ê®Æ
	NUROPOINT	StartCoor;//¸ô¬q°_ÂI®y¼Ð
	NUROPOINT	EndCoor;//¸ô¬q²×ÂI®y¼Ð
	nuLONG	Flag;//¦æ¬°ºX¼Ð(¤W¤U°ª¬[¾ô ¦a¤U¹D)
	nuLONG	TurnFlag;//¤è¦VºX¼Ð(¥ª¥kÂà °jÂà)
	nuLONG	TurnAngle;//¨¤«×
	nuDWORD	ArcPtCount;//²Õ¦¨¹D¸ôªº®y¼ÐÂIÁ`¼Æ
	NURORECT	Bound;//¹D¸ôÃä¬É(µe¾É¯è¸ô®|®É§PÂ_)
	NUROPOINT	*ArcPt;//²Õ¦¨¹D¸ôªº®y¼ÐÂI
	nuLONG	Real3DPic;//3D¹ê´º¹Ï¼Æ¦rIID
	nuLONG PTSNameAddr;//PTSÃû·QÎ»ÖÃ
	nuLONG ShowIndex;
	nuBOOL bGPSState;
	nuBYTE	nOneWay;
	nuBYTE	nReserve[2];
}NAVISUBCLASS, *PNAVISUBCLASS;

typedef struct tagPTFIXTOROADSTRU
{
	nuDWORD		MapIdx;//ÀÉ®×½s¸¹
	nuDWORD		MapID;//³Ìªñ¹D¸ô©Ò¦bªºÀÉ®×ID
	nuDWORD		RTBID;//³Ìªñ¹D¸ô©Ò¦bªº°Ï¶ôID
	NUROPOINT	FixCoor;//©Ô¦^®y¼Ð
	nuDWORD		FixNode1;//³Ìªñ¹D¸ôªº¸`ÂIID1
	nuDWORD		FixNode2;//³Ìªñ¹D¸ôªº¸`ÂIID2
	long		FixDis;//³W¹ºÂI»P³Ìªñ¹D¸ôªº¶ZÂ÷
}PTFIXTOROADSTRU,*PPTFIXTOROADSTRU;

typedef struct tagNAVIRTBLOCKCLASS
{
	nuDWORD NSSCount;//³W¹º¥X¨Óªº¸ô¬qÁ`¼Æ
	long	RealDis;//¯u¹ê¶ZÂ÷
	long	WeightDis;//¥[Åv¶ZÂ÷
	PTFIXTOROADSTRU FInfo;//°_ÂI¬ÛÃö¸ê°T
	PTFIXTOROADSTRU TInfo;//²×ÂI¬ÛÃö¸ê°T
	nuDWORD *NodeIDList;//³~¸gªº¸`ÂI¸ê®Æ
	nuDWORD NodeIDCount;//³~¸gªº¸`ÂIÁ`¼Æ
	nuBYTE RTBStateFlag;//0:¤w¸Ñ¶}ªºRTB 1:¥¼¸Ñ¶}ªºRTB(³~¸gXXX) 2:¥¼¸Ñ¶}ªºTHC(¶i¤J¥þ°ê¸ôºô). 
	nuBYTE RTBWMPFixFlag;//0 :WMP¥¼?? 1:??§¹. 
//	nuWORD RTBStateFlag;//0:ÒÑ½âé_µÄRTB 1:Î´½âé_µÄRTB(Í¾?XXX) 2:Î´½âé_µÄTHC(ßMÈëÈ«‡øÂ·¾W)
	nuWORD FileIdx;//™n°¸ID
//	long FileIdx;//ÀÉ®×ID
	nuDWORD RTBIdx;//Rt°Ï¶ôID
	NAVISUBCLASS	*NSS;//³W¹º¥X¨Óªº¹D¸ô¤lµ²ºc
	tagNAVIRTBLOCKCLASS *NextRTB;
}NAVIRTBLOCKCLASS,*PNAVIRTBLOCKCLASS;

typedef struct tagNAVIFROMTOCLASS
{
	bool    DisPaly;
	bool	RunPass;//¥»¬q³W¹º§¹¦¨»P§_
	long	Reserve:24;
	NUROPOINT	FCoor;//¥Xµo¦a®y¼Ð
	long	FromFileID;//¥Xµo¦a©Ò¦b¹Ï¸êID
	long	FromRtBID;//°_ÂI©Ò¦b°Ï¶ô
	NUROPOINT	TCoor;//¥Øªº¦a®y¼Ð
	long	ToFileID;//¥Øªº¦a©Ò¦b¹Ï¸êID
	long	ToRtBID;//²×ÂI©Ò¦b°Ï¶ô
	long	NRBCCount;//³W¹º¦æ¸gªº°Ï¶ô¼Æ
	NAVIRTBLOCKCLASS	*NRBC;//Rt°Ï¶ô¸ê®Æµ²ºc
	tagNAVIFROMTOCLASS	*NextFT;
}NAVIFROMTOCLASS,*PNAVIFROMTOCLASS;

typedef struct tagROUTINGDATA
{
	long	NFTCCount;//°_¸g¨´µ²ºcÁ`¼Æ
#ifdef LCMM
	NAVIFROMTOCLASS	*NFTC[MAX_RouteRule];	
#else
	NAVIFROMTOCLASS	*NFTC;//°_¸g¨´¸ê®Æµ²ºc
#endif
}ROUTINGDATA,*PROUTINGDATA;


typedef struct tagLOCALROUTEDATA
{
	NAVIRTBLOCKCLASS*	pNRBC;
	tagLOCALROUTEDATA*	pNext;
}LOCALROUTEDATA, *PLOCALROUTEDATA;

//the result number of Route
#define ROUTE_ERROR						0
#define ROUTE_SUCESS					1
#define ROUTE_SAMEPT					2
#define ROUTE_OUTROUTELIMIT				3
#define ROUTE_NOFTPT					4
#define ROUTE_THRERROR					5//¸ó°Ï³W¹º¥¢±Ñ
#define ROUTE_THRLOCALERROR				6//³æ°Ï³W¹º¥¢±Ñ
#define ROUTE_CLOSEERROR				7//°Ï°ì³W¹º¸ê®Æ¾ã²z¥¢±Ñ
#define ROUTE_READSRTFALSE				8//Åª¨ú°_ÂIRT¥¢±Ñ
#define ROUTE_READERTFALSE				9//Åª¨ú²×ÂIRT¥¢±Ñ
#define ROUTE_READRTFALSE				10//Åª¨úRT¥X¿ù
#define ROUTE_READRDWFALSE				11//Åª¨úRDW¥X¿ù
#define ROUTE_READTHRFALSE				12//Åª¨úThr¥¢±Ñ
#define ROUTE_USEALLASTARBUFFER			13//¹Bºâ¶q¶W¹LAStar­­¨î
#define ROUTE_NOFOUNDROADTOENDPOINT		14//¨S¦³§ä¨ì¥i³q¨ì²×ÂIªº¸ô®|
#define ROUTE_APPROACHFALSE				15//¦^®Ò¸ô®|¥¢±Ñ
#define ROUTE_CREATEROUTEDATAFASLE		16//²£¥Í³W¹ºÀx¦s¸ê®Æ¥¢±Ñ
#define ROUTE_SUCCESSSTEP1				17//¦¨¥\«á¾ã²z¸ê®Æ²Ä¤@¨B(³B²zRT)
#define ROUTE_SUCCESSSTEP2				18//¦¨¥\«á¾ã²z¸ê®Æ²Ä¤G¨B(³B²zdw)
#define ROUTE_SUCCESSSTEP3				19//°Ï°ì³W¹º¸ê®Æ¾ã²z¥¢±Ñ
#define ROUTE_NEWASTARSTRUCTFALSE		20//²£¥ÍAStarµ²ºc¥¢±Ñ
#define ROUTE_READMRTFALSE				21//Åª¨úThc¥¢±Ñ
#define ROUTE_MRT_ASTARFALSE			22//MRT AStar False
#define ROUTE_MRT_CREATEROUTEDATAFASLE	23//²£¥Í³W¹ºÀx¦s¸ê®Æ¥¢±Ñ
#define ROUTE_MRT_APPROACHFALSE			24//²£¥Í³W¹ºÀx¦s¸ê®Æ¥¢±Ñ



/////////////////////////////////////////////////////////////////////////////////
typedef struct tagROADCROSSDATA
{
	nuDWORD	BlockIdx;//
	nuWORD	nMapIdx;
	nuWORD	RoadIdx;//
	short	RoadAngle;//
	short	RoadLength;
	nuWORD	RoadVoiceType;//
	nuWORD	RoadClass;//
	long	nNameAddr;
}ROADCROSSDATA, *PROADCROSSDATA;

typedef struct tagROADSEGDATA
{
	nuDWORD	BlockIdx;//
	nuWORD	nMapIdx;
	nuWORD	RoadIdx;//
	short	RoadLength;//
	short	RoadAngle;
	nuWORD	RoadVoiceType;//
	nuWORD	RoadClass;
	long	nNameAddr;
	nuWORD	nCrossCount;
	ROADCROSSDATA	*pRdCross;//
	NUROPOINT		EndCoor;//
}ROADSEGDATA, *PROADSEGDATA;

typedef struct tagCROSSFOLLOWED
{
	nuDWORD			nRdSegCount;
	PROADSEGDATA	pRdSegList;
}CROSSFOLLOWED, *PCROSSFOLLOWED;

//2011.05.24
typedef struct tag_TMC_INFOROUTER
{
	nuBOOL  bTMCPass;
	nuSHORT	sTMCRouteWeighting;
	nuSHORT	sTMCRouteTime;
	nuSHORT	sTMCNodeCount;
	nuSHORT sTMCEventID;
	nuLONG	lTMCLocationAddr;
}TMC_ROUTER_DATA, *PTMC_ROUTER_DATA;

typedef struct tag_TMC_Router
{
	nuINT   nCount;
	PTMC_ROUTER_DATA pPTMC_INFORouter;	 
}TMC_Info_Router;

typedef struct Strcture_KMInfoData
{
	nuLONG  lBlockIdx;
	nuLONG  lRoadIdx;
	nuBYTE  nClass;
	nuBYTE  nOneWay;
	nuWORD  KmName;
	nuLONG  X;
	nuLONG  Y;
	nuLONG	NameLen;
	nuLONG	NameAddr;
}KMInfoData;
typedef struct tagTMCRoadInfoStru
{
	nuCHAR  Traffic;
	nuCHAR  Dir;
}TMCRoadInfoStru,*pTMCRoadInfoStru;
#endif
