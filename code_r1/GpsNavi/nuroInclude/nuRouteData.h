#ifndef _NURO_ROUTE_DATA_20080521
#define _NURO_ROUTE_DATA_20080521

#include "NuroDefine.h"

typedef struct tagNAVICROSSCLASS
{
	nuWORD	CrossBlockIdx;//Block index
	nuWORD	CrossRoadIdx;//Road index in block
	long	CrossRoadNameAddr;//Road Name address
	short	CrossAngle;//
	nuWORD  nFlag:4;//0:場宎趙 1:�△議伄� 2:鳳�﹋妍�
	nuWORD  nRoadNameLen:12;
	long	CrossVoiceType;//
	long	CrossClass;//
}NAVICROSSCLASS, *PNAVICROSSCLASS;

typedef struct tagNAVISUBCLASS
{
	nuWORD	BlockIdx;//所在的Dw區塊
	nuWORD	RoadIdx;//道路ID
	nuLONG	RoadNameAddr;//道路名稱位置
	nuDWORD	RoadLength;//道路長度
	nuLONG	RoadVoiceType;//語音播報路型

	nuDWORD	RoadClass : 16;
	nuDWORD    SPTFlag: 1; //SignPost Flag
	nuDWORD	 LaneGuideFlag:1; //車道訊息旗標
	nuDWORD    Reserve: 14; //Reserve
	nuDWORD CrossCount;//路段終點的交叉路口數(可通行)
	nuWORD	CityID;//城市碼
	nuWORD	TownID;//鄉鎮碼
	NAVICROSSCLASS *NCC;//交叉路口資料
	NUROPOINT	StartCoor;//路段起點座標
	NUROPOINT	EndCoor;//路段終點座標
	nuLONG	Flag;//行為旗標(上下高架橋 地下道)
	nuLONG	TurnFlag;//方向旗標(左右轉 迴轉)
	nuLONG	TurnAngle;//角度
	nuDWORD	ArcPtCount;//組成道路的座標點總數
	NURORECT	Bound;//道路邊界(畫導航路徑時判斷)
	NUROPOINT	*ArcPt;//組成道路的座標點
	nuLONG	Real3DPic;//3D實景圖數字IID
	nuLONG PTSNameAddr;//PTS靡想弇离
	nuLONG ShowIndex;
	nuBOOL bGPSState;
	nuBYTE	nOneWay;
	nuBYTE	nReserve[2];
}NAVISUBCLASS, *PNAVISUBCLASS;

typedef struct tagPTFIXTOROADSTRU
{
	nuDWORD		MapIdx;//檔案編號
	nuDWORD		MapID;//最近道路所在的檔案ID
	nuDWORD		RTBID;//最近道路所在的區塊ID
	NUROPOINT	FixCoor;//拉回座標
	nuDWORD		FixNode1;//最近道路的節點ID1
	nuDWORD		FixNode2;//最近道路的節點ID2
	long		FixDis;//規劃點與最近道路的距離
}PTFIXTOROADSTRU,*PPTFIXTOROADSTRU;

typedef struct tagNAVIRTBLOCKCLASS
{
	nuDWORD NSSCount;//規劃出來的路段總數
	long	RealDis;//真實距離
	long	WeightDis;//加權距離
	PTFIXTOROADSTRU FInfo;//起點相關資訊
	PTFIXTOROADSTRU TInfo;//終點相關資訊
	nuDWORD *NodeIDList;//途經的節點資料
	nuDWORD NodeIDCount;//途經的節點總數
	nuBYTE RTBStateFlag;//0:已解開的RTB 1:未解開的RTB(途經XXX) 2:未解開的THC(進入全國路網). 
	nuBYTE RTBWMPFixFlag;//0 :WMP未?? 1:??完. 
//	nuWORD RTBStateFlag;//0:眒賤嶱腔RTB 1:帤賤嶱腔RTB(芴?XXX) 2:帤賤嶱腔THC(筳�躽��讕溝W)
	nuWORD FileIdx;//�n偶ID
//	long FileIdx;//檔案ID
	nuDWORD RTBIdx;//Rt區塊ID
	NAVISUBCLASS	*NSS;//規劃出來的道路子結構
	tagNAVIRTBLOCKCLASS *NextRTB;
}NAVIRTBLOCKCLASS,*PNAVIRTBLOCKCLASS;

typedef struct tagNAVIFROMTOCLASS
{
	bool    DisPaly;
	bool	RunPass;//本段規劃完成與否
	long	Reserve:24;
	NUROPOINT	FCoor;//出發地座標
	long	FromFileID;//出發地所在圖資ID
	long	FromRtBID;//起點所在區塊
	NUROPOINT	TCoor;//目的地座標
	long	ToFileID;//目的地所在圖資ID
	long	ToRtBID;//終點所在區塊
	long	NRBCCount;//規劃行經的區塊數
	NAVIRTBLOCKCLASS	*NRBC;//Rt區塊資料結構
	tagNAVIFROMTOCLASS	*NextFT;
}NAVIFROMTOCLASS,*PNAVIFROMTOCLASS;

typedef struct tagROUTINGDATA
{
	long	NFTCCount;//起經迄結構總數
#ifdef LCMM
	NAVIFROMTOCLASS	*NFTC[MAX_RouteRule];	
#else
	NAVIFROMTOCLASS	*NFTC;//起經迄資料結構
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
#define ROUTE_THRERROR					5//跨區規劃失敗
#define ROUTE_THRLOCALERROR				6//單區規劃失敗
#define ROUTE_CLOSEERROR				7//區域規劃資料整理失敗
#define ROUTE_READSRTFALSE				8//讀取起點RT失敗
#define ROUTE_READERTFALSE				9//讀取終點RT失敗
#define ROUTE_READRTFALSE				10//讀取RT出錯
#define ROUTE_READRDWFALSE				11//讀取RDW出錯
#define ROUTE_READTHRFALSE				12//讀取Thr失敗
#define ROUTE_USEALLASTARBUFFER			13//運算量超過AStar限制
#define ROUTE_NOFOUNDROADTOENDPOINT		14//沒有找到可通到終點的路徑
#define ROUTE_APPROACHFALSE				15//回朔路徑失敗
#define ROUTE_CREATEROUTEDATAFASLE		16//產生規劃儲存資料失敗
#define ROUTE_SUCCESSSTEP1				17//成功後整理資料第一步(處理RT)
#define ROUTE_SUCCESSSTEP2				18//成功後整理資料第二步(處理dw)
#define ROUTE_SUCCESSSTEP3				19//區域規劃資料整理失敗
#define ROUTE_NEWASTARSTRUCTFALSE		20//產生AStar結構失敗
#define ROUTE_READMRTFALSE				21//讀取Thc失敗
#define ROUTE_MRT_ASTARFALSE			22//MRT AStar False
#define ROUTE_MRT_CREATEROUTEDATAFASLE	23//產生規劃儲存資料失敗
#define ROUTE_MRT_APPROACHFALSE			24//產生規劃儲存資料失敗



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
