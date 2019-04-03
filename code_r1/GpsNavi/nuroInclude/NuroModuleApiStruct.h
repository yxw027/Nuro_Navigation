#ifndef __NURO_MODULE_API_STRUCT_20080825
#define __NURO_MODULE_API_STRUCT_20080825

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"
#include "NuroEngineStructs.h"
#include "NuroCommonStructs.h"
#include "NuroMapFormat.h"
#include "nuSearchDefine.h"
#include "GApiOpenResource.h"

//GpsOpen Api
typedef nuBOOL(*GPS_OpenProc)();
typedef nuBOOL(*GPS_CloseProc)();
typedef nuBOOL(*GPS_RecvDataProc)(const nuCHAR*, nuINT);
typedef nuBOOL(*GPS_GetNewDataProc)(PGPSDATA*);
typedef nuVOID(*GPS_TOBEDEBUGINFOProc)(PGPSMODINFO*);
typedef nuVOID(*GPS_TOBERESETGYROProc)();
typedef nuVOID(*GPS_TOBEGETVERSIONProc)();

typedef nuBOOL(*GPS_GetGpsXYProc)(nuLONG*, nuLONG*);
typedef nuBYTE(*GPS_GetStateProc)();
typedef nuVOID(*GPS_FeedBackInfoProc)(nuLONG nRoadx, nuLONG nRoady, nuSHORT dx, nuSHORT dy, nuSHORT nSpeed, nuINT nTime);
typedef nuVOID(*GPS_FeedBackProc)(const FEEDBACKDATA* pFeedBackInfo);
typedef nuBOOL (*GPS_ReSetProc)( PGPSCFG pGpsSetData);
typedef nuVOID (*GPS_SetGPSChinaProc)(nuPVOID lpFunc);
typedef nuVOID (*GPS_GetRawDataProc)(PGPSDATA);//added by daniel @ 20120204
typedef nuBOOL (*GPS_GetGPSComConfigProc)(nuLONG& GPSCommPort, nuLONG& GPSBaudrate);//Added by Damon 20120705
typedef nuBOOL (*GPS_GPS_ReSetProc)(PGPSCFG pGpsSetData);//Added by Damon 20120705
typedef nuVOID (*GPS_RoadDelayProc)(nuBOOL bDelay, nuBOOL b3DMode);
//typedef unsigned nuINT (*GPS_WgToChinaProc)(nuINT wg_flag, unsigned nuINT wg_lng, unsigned nuINT wg_lat, nuINT wg_heit, nuINT wg_week, unsigned nuINT wg_time, unsigned  nuINT *china_lng, unsigned nuINT *china_lat);
typedef nuBOOL (*GPS_ELEAD_TRANProc)(const GYROGPSDATA_EL* pEL_GyraData);	
typedef nuVOID (*GPS_WriteComProc)(nuBYTE *Data, nuLONG nLen);

typedef struct tagGPSOPENAPI
{
	GPS_OpenProc			GPS_Open;
	GPS_CloseProc			GPS_Close;
	GPS_RecvDataProc		GPS_RecvData;
	GPS_GetNewDataProc		GPS_GetNewData;
	GPS_TOBEDEBUGINFOProc	GPS_TOBEDEBUGINFO;
	GPS_TOBERESETGYROProc	GPS_TOBERESETGYRO;
	GPS_TOBEGETVERSIONProc	GPS_TOBEGETVERSION;
	GPS_GetRawDataProc		GPS_GetRawData;
	GPS_GetGPSComConfigProc GPS_GetGPSComConfig; //Added by Damon 20120705
	GPS_GPS_ReSetProc       GPS_GPS_ReSet; //Added by Damon 20120705

	GPS_GetGpsXYProc		GPS_GetGpsXY;
	GPS_GetStateProc		GPS_GetState;
	GPS_FeedBackInfoProc	GPS_FeedBackInfo;
	GPS_FeedBackProc		GPS_FeedBack;
	GPS_ReSetProc           GPS_ReSet;
	GPS_SetGPSChinaProc     GPS_SetGPSChina;
	GPS_RoadDelayProc		GPS_RoadDelay;
	GPS_ELEAD_TRANProc		GPS_ELEAD_TRAN;
	GPS_WriteComProc		GPS_WriteCom;

	#ifdef ANDROID_GPS
    GPS_RecvDataProc	  	GPS_JNI_RecvData;
    #endif
}GPSOPENAPI, *PGPSOPENAPI;


typedef nuBOOL(*COM_OpenProc)();
typedef nuBOOL(*COM_CloseProc)();
typedef nuBOOL(*COM_RecvDataProc)(const nuCHAR*, nuINT);
typedef nuBOOL(*COM_GetXYProc)(nuLONG*, nuLONG*);

typedef struct tagCOMOPENAPI
{
	COM_OpenProc			COM_Open;
	COM_CloseProc			COM_Close;
	COM_RecvDataProc		COM_RecvData;
	COM_GetXYProc			COM_GetXY;
}COMOPENAPI, *PCOMOPENAPI;

//DrawMap Api
typedef nuBOOL(*DM_LoadMapProc)(nuPVOID);
typedef nuBOOL(*DM_SeekForProc)(nuLONG, nuLONG, nuLONG, PSEEKEDROADDATA pRoadData);
typedef nuBOOL(*DM_CarToRoadProc)(nuLONG, nuLONG, nuLONG, nuLONG, PSEEKEDROADDATA );
typedef nuBOOL(*DM_StartTravelProc)(nuWCHAR *);
typedef nuBOOL(*DM_PendTravelProc)(nuroPOINT&);
typedef nuBOOL(*DM_StopTravelProc)(nuBOOL);
typedef nuBOOL(*DM_ShowTravelProc)(nuWORD, nuBOOL);
typedef nuBOOL(*DM_DrawMapProc)(nuPVOID lpData, nuWORD nMode);
#define DRAWMAP_MODE_DEFAULT					0x01
#define DRAWMAP_MODE_GLIDEMOVE					0x02
#define DRAWMAP_MODE_JUMPMOVE					0x03
#define DRAWMAP_MODE_ZOOMFAKE					0x04
#define DRAWMAP_MODE_JUMPFAKE					0x05
typedef nuBOOL(*DM_FreeDataMemProc)();
typedef nuUINT(*DM_StartOpenGLProc)(SYSDIFF_SCREEN sysDiffScreen, const NURORECT* pRtScreen);
typedef nuVOID(*DM_Reset3DModelProc)(nuBOOL, nuWORD, nuWORD);
typedef nuBOOL(*DM_GetNearKiloProc)(nuLONG* pDis, nuWCHAR* pwsKilos); //added by danile 20121205
typedef nuVOID(*DM_FreeDrawDataProc)(nuLONG nFreeData);
//-----------------------SDK---------------------//
typedef nuUINT(*DM_DrawMapCarProc)(NUROPOINT ptCar);
typedef nuBOOL(*DM_DrawMapPicProc)(NURORECT Rect, NURO_BMP Bmp, nuSHORT *pBmp);
typedef nuVOID(*DM_CheckSkyBmpProc)();
//-----------------------SDK---------------------//
typedef struct tagDRAWMAPAPI
{
	DM_LoadMapProc	DM_LoadMap;
	DM_LoadMapProc	DM_DrawMapBmp1;
	DM_LoadMapProc	DM_DrawMapBmp2;
	DM_LoadMapProc	DM_DrawZoomRoad;
	DM_LoadMapProc	DM_DrawZoomFake;
	DM_LoadMapProc	DM_DrawFlag;
	DM_LoadMapProc	DM_DrawRawCar;
	DM_SeekForProc	DM_SeekForRoad;
	DM_SeekForProc	DM_SeekForPOI;
	DM_CarToRoadProc	DM_CarToRoad;
	DM_StartTravelProc	DM_StartTravel;
	DM_PendTravelProc	DM_PendTravel;
	DM_StopTravelProc	DM_StopTravel;
	DM_ShowTravelProc	DM_ShowTravel;
	DM_DrawMapProc		DM_DrawMap;
	DM_FreeDataMemProc	DM_FreeDataMem;
	DM_StartOpenGLProc	DM_StartOpenGL;
	DM_Reset3DModelProc DM_Reset3DModel;
	DM_GetNearKiloProc  DM_GetNearKilo;//added by daniel 20121205
	DM_FreeDrawDataProc  DM_FreeDrawData;
//-----------------------SDK---------------------//
	DM_DrawMapCarProc   DM_DrawMapCar;
	DM_DrawMapPicProc   DM_DrawMapPic;
	DM_CheckSkyBmpProc  DM_CheckSkyBmp;
//-----------------------SDK---------------------//
}DRAWMAPAPI, *PDRAWMAPAPI;
//DrawInfoMap Api
//--- for the parameter of LibIFM_SetState -------------------------------------------
#define STATE_BUT_HIDE					BUTTON_HIDE
#define STATE_BUT_DEFAULT				BUTTON_DEFAULT
#define STATE_BUT_DOWN					BUTTON_DOWN
#define STATE_BUT_FOCUS					BUTTON_FOCUS
#define STATE_BUT_DISABLE				BUTTON_DISABLE
//--- for the return value of LibIFM_SetState() --------------------------------------
#define STATE_UPDATA_NOBUT				0
#define STATE_UPDATA_NOCHANGE			1
#define STATE_UPDATA_SHOWMAP			3
#define STATE_UPDATA_DRAWINFO			2
//

typedef nuBOOL(*IFM_DrawItemsProc)(nuHDC, nuPVOID);
typedef nuUINT(*IFM_ItemsJudgeProc)(const nuroPOINT& pt, nuBYTE);
typedef PNUROMENUINFO(*IFM_DrawMenuProc)();
typedef nuUINT(*IFM_MenuJudgeProc)(const nuroPOINT&, nuBYTE);
typedef nuBOOL(*IFM_DrawAnimationProc)(PANIMATIONDATA pAni);
typedef PMESSAGEBOXDATA(*IFM_DrawMsgBoxProc)(nuINT );
typedef nuUINT(*IFM_MsgBoxJudgeProc)(const nuroPOINT&, nuBYTE);
typedef nuUINT(*IFM_SetStateProc)(nuUINT, nuBYTE);
typedef nuBOOL(*IFM_DrawCarProc)(const nuroPOINT&, nuPVOID);
typedef nuUINT(*IFM_MouseProcProc)(const nuroPOINT&, nuBYTE, nuUINT*);
typedef nuUINT(*IFM_KeyProcProc)(nuUINT, nuBYTE, nuUINT *);
typedef nuBOOL(*IFM_DrawMoveButtonProc)(nuHDC hDC, nuPVOID lpData);
typedef nuUINT (*IFM_MoveButtonJudgeProc)(const nuroPOINT& pt, nuBYTE nMouseMode);
typedef nuUINT (*IFM_SetStateArrayProc)(PDFMSETTING pSetData);
typedef nuBOOL(*IFM_GetStateProc)(nuUINT nCodeID, nuBYTE& nState);
typedef nuUINT(*IFM_SetRangeProc)(nuUINT nCodeID, nuroSRECT& rtRect, nuSHORT& nAlign);
typedef nuBOOL(*IFM_DrawCenterTargetProc)(const nuroPOINT&, nuBYTE);
typedef nuBOOL(*IFM_SetDataProc)(nuUINT nCode, nuPVOID pData);
typedef nuINT (*IFM_GetPtScaleProc)(nuroPOINT pt);
typedef struct tagDARWINFOMAPAPI
{
	PNUROMENUINFO				pMenuInfo;
	PMESSAGEBOXDATA			pMsgBox;
	IFM_DrawItemsProc		IFM_DrawItems;
	IFM_ItemsJudgeProc		IFM_ItemsJudge;
	IFM_DrawMenuProc		IFM_DrawMenu;
	IFM_MenuJudgeProc		IFM_MenuJudge;
	IFM_DrawAnimationProc	IFM_DrawAnimation;
	IFM_DrawMsgBoxProc		IFM_DrawMsgBox;
	IFM_MsgBoxJudgeProc		IFM_MsgBoxJudge;
	IFM_SetStateProc		IFM_SetState;
	IFM_DrawCarProc			IFM_DrawCar;
	IFM_MouseProcProc		IFM_MouseProc;
	IFM_KeyProcProc			IFM_KeyProc;
	IFM_DrawMoveButtonProc	IFM_DrawMoveButton;
	IFM_MoveButtonJudgeProc	IFM_MoveButtonJudge;
	IFM_SetStateArrayProc	IFM_SetStateArray;
	IFM_GetStateProc		IFM_GetState;
	IFM_SetRangeProc		IFM_SetRange;
	IFM_DrawCenterTargetProc	IFM_DrawCenterTarget;
	IFM_SetDataProc			IFM_SetData;
	IFM_GetPtScaleProc		IFM_GetPtScale;
}DRAWINFOMAPAPI, *PDRAWINFOMAPAPI;
//DrawInfoNavi Api

typedef nuBOOL(*IFN_DrawItemsProc)(nuPVOID lpData);
typedef nuUINT (*IFN_SetStateArrayProc)(PDFMSETTING pSetData);
typedef struct tagDRAWINFONAVIAPI
{
	IFN_DrawItemsProc		IFN_DrawItems;
	IFN_SetStateArrayProc	IFN_SetStateArray;
}DRAWINFONAVIAPI, *PDRAWINFONAVIAPI;
//InNavi Api
typedef nuBOOL(*IN_DrawNaviRoadProc)(nuBYTE);
typedef nuBOOL(*IN_GetSimuDataProc)(nuBOOL, nuLONG, nuPVOID);
typedef nuUINT(*IN_PointToNaviLineProc)(nuLONG, nuLONG, nuLONG, nuSHORT, PSEEKEDROADDATA);
typedef nuBOOL(*IN_ColNaviInfoProc)();
typedef nuPVOID(*IN_CheckScreenRoadProc)(const nuroRECT&);
typedef nuUINT(*IN_GetRoutingListProc)(PROUTINGLISTNODE , nuINT , nuINT );
typedef nuBOOL(*IN_PlayNaviSoundProc)();
typedef nuBYTE(*IN_JudgeZoomRoadProc)();
typedef nuBOOL(*IN_DrawArrowProc)(nuBYTE);
typedef nuBOOL(*IN_NaviProc)();
typedef nuBYTE(*IN_JudgeRoadTypeProc)(PSEEKEDROADDATA pNowRoadInfo);
typedef nuBOOL (*IN_GetCCDRoadInfoProc)(PCCDINFO pCcdInfo);
typedef nuVOID (*IN_GetMinAndMaxCoor_DProc)(NUROPOINT *MaxPoint, NUROPOINT *MinPoint);//added by daniel
#ifdef LCMM
	typedef nuVOID (*IN_ChooseRouteLinethProc)(nuLONG RouteLineth);//added by daniel for LCMM
	typedef nuVOID (*IN_EstimateTimeAndDistanceProc)(nuLONG *TargetDis, nuLONG *TargetTime);//added by daniel for LCMM
	typedef nuVOID (*IN_DisplayRoutePathProc)(nuLONG SelectRoutePath);
	typedef nuLONG (*IN_TMCRouteProc)(TMC_ROUTER_DATA *pTMCData, nuLONG TMCDataCount,TARTGETINFO NewStart,nuLONG RouteRule);
#endif
typedef nuBOOL (*IN_GetCarInfoForKMProc)(PKMINFO pKMInfo);//added by daniel
typedef nuVOID (*IN_SetSimulationProc)();
typedef nuBOOL(*IN_TMCNaviPathDataProc)();
typedef nuVOID (*IN_FreeTMCNaviPathDataProc)();
typedef nuVOID (*IN_GetCorssRoadDataProc)(CROSSROADINFO &CrossRoadInfo);
//----------------------SDK-------------------------------------------------------------------//
typedef nuPVOID (*IN_GetRoutingListExProc)(PROUTINGLISTNODE pRTlist, nuINT *pNum, nuINT *pIndex);
//----------------------SDK-------------------------------------------------------------------//
typedef struct tagINNAVIAPI
{
	IN_GetMinAndMaxCoor_DProc IN_GetMinAndMaxCoor_D;//added by daniel
	IN_GetCarInfoForKMProc  IN_GetCarInfoForKM;
#ifdef LCMM
	IN_EstimateTimeAndDistanceProc IN_EstimateTimeAndDistance;
	IN_ChooseRouteLinethProc IN_ChooseRouteLineth;//added by daniel for LCMM
	IN_DisplayRoutePathProc  IN_DisplayRoutePath;
	IN_TMCRouteProc          IN_TMCRoute;
#endif
	IN_DrawNaviRoadProc		IN_DrawNaviRoad;
	IN_GetSimuDataProc		IN_GetSimuData;
	IN_PointToNaviLineProc	IN_PointToNaviLine;
	IN_ColNaviInfoProc		IN_ColNaviInfo;
	IN_CheckScreenRoadProc	IN_CheckScreenRoad;
	IN_GetRoutingListProc	IN_GetRoutingList;
	IN_PlayNaviSoundProc    IN_PlayNaviSound;
	IN_JudgeZoomRoadProc	IN_JudgeZoomRoad;
	IN_DrawArrowProc		IN_DrawArrow;
	IN_NaviProc				IN_NaviStart;
	IN_NaviProc				IN_NaviStop;
	IN_JudgeRoadTypeProc    IN_JudgeRoadType;
	IN_GetCCDRoadInfoProc   IN_GetCCDRoadInfo;
	IN_SetSimulationProc    IN_SetSimulation;
	IN_GetSimuDataProc		IN_GetSimuTunnelData;
	IN_TMCNaviPathDataProc	IN_TMCNaviPathData;
	IN_FreeTMCNaviPathDataProc IN_FreeTMCNaviPathData;
	IN_GetCorssRoadDataProc IN_GetCorssRoadData;
//----------------------SDK-------------------------------------------------------------------//
	IN_GetRoutingListExProc	IN_GetRoutingListEx;
//----------------------SDK-------------------------------------------------------------------//
}INNAVIAPI, *PINNAVIAPI;
//Route Api
typedef nuLONG(*RT_StartRouteProc)(PROUTINGDATA);
typedef nuBOOL(*RT_ReleaseRouteProc)(PROUTINGDATA);
typedef nuLONG(*RT_LocalRouteProc)(NAVIRTBLOCKCLASS**);
typedef nuLONG(*RT_ReRouteProc)(ROUTINGDATA*, TARTGETINFO);
typedef nuLONG(*RT_CongestionRouteProc)(ROUTINGDATA*, TARTGETINFO, nuLONG);
typedef nuBOOL(*RT_GetNextCrossDataProc)(nuLONG MapID, nuLONG BlockID, nuLONG RoadID, nuLONG CarAngle, 
		NUROPOINT NowCoor, NUROPOINT TNodeCoor, CROSSFOLLOWED **stru_Cross);
typedef nuBOOL(*RT_ReleaseNextCrossDataProc)(CROSSFOLLOWED **stru_Cross);
typedef nuBOOL(*RT_GetMRouteDataProc)(PNAVIFROMTOCLASS pNFTC);
typedef nuVOID(*RT_Route_DemoProc)(nuSHORT DemoWR,nuLONG DemoNumber);//added by daniel for demo 2010.08.17
typedef nuBOOL(*RT_ReadCoorProc)(NUROPOINT *FCoor,NUROPOINT *TCoor,nuLONG DemoNumber);//added by daniel for demo 2010.08.26
typedef nuVOID(*RT_ReleaseNFTCProc)(PNAVIFROMTOCLASS pNFTC);//added by daniel for LCMM
#ifdef LCMM
typedef nuVOID(*RT_SetRouteRuleProc)(nuLONG *RouteRule, nuLONG RuleCount);//added by daniel for LCMM 2011.04.12
typedef nuLONG(*RT_TMCRouteProc)(NAVIFROMTOCLASS *pNFTC, PNAVIRTBLOCKCLASS pNRBC,TARTGETINFO NewStart, TMC_ROUTER_DATA *TMCData, nuLONG TMCDataCount, nuLONG RouteRule);
#endif
typedef struct tagROUTEAPI
{
	RT_ReleaseNFTCProc      	RT_ReleaseNFTC;//added by daniel for LCMM
	RT_ReadCoorProc			RT_ReadCoor;//added by daniel for demo 2010.08.26
	RT_Route_DemoProc		RT_Route_Demo;//added by daniel for demo 2010.08.17	
	RT_StartRouteProc		RT_StartRoute;
	RT_ReleaseRouteProc		RT_ReleaseRoute;
	RT_LocalRouteProc		RT_LocalRoute;
	RT_ReRouteProc			RT_ReRoute;
	RT_CongestionRouteProc	RT_CongestionRoute;
	RT_GetNextCrossDataProc	RT_GetNextCrossData;
	RT_ReleaseNextCrossDataProc	RT_ReleaseNextCrossData;
	RT_GetMRouteDataProc		RT_GetMRouteData;
#ifdef LCMM
	RT_SetRouteRuleProc     RT_SetRouteRule; //added by daniel for LCMM 2011.04.12
	RT_TMCRouteProc         RT_TMCRoute;
#endif
}ROUTEAPI,  *PROUTEAPI;

//MathTool Api
typedef nuBOOL(*MT_MapToBmpProc)(nuLONG, nuLONG, nuLONG*, nuLONG*);
typedef nuBOOL(*MT_SaveOrShowBmpProc)(nuBYTE);
typedef nuINT(*MT_SelectObjectProc)(nuBYTE, nuBYTE, const nuVOID*, nuLONG*);
typedef nuBOOL(*MT_DrawObjectProc)(nuINT, PNUROPOINT, nuINT, nuLONG);
//typedef nuBOOL(*MT_SetBitmapProc)(PNUROBITMAP);
typedef nuBOOL(*MT_SetBitmapProc)(PNURO_BMP);
#define _3D_PARAM_TYPE_KEEP				0xFF
#define _3D_PARAM_TYPE_SCALE			0x00
#define _3D_PARAM_TYPE_MAX				0x01
#define _3D_PARAM_TYPE_STEP_IN			0x02
#define _3D_PARAM_TYPE_STEP_OUT			0x03
typedef nuBOOL(*MT_Set3DParamProc)(nuWORD nType, nuSHORT nStep);
typedef nuVOID(*MT_Bmp2Dto3DProc)(nuLONG&, nuLONG&, nuBYTE);
typedef struct tagCTRL_3D_PARAM
{
	nuSHORT	nLookAtx;
	nuSHORT	nLookAty;
	nuSHORT	nLookAtz;
	nuSHORT	nLookTox;
	nuSHORT	nLookToy;
	nuSHORT	nLookToz;
	nuSHORT	nDz;
	nuBOOL	bScale;
	nuBYTE	nScaleC;
	nuBYTE	nScaleD;
}CTRL_3D_PARAM, *PCTRL_3D_PARAM;
typedef nuVOID(*MT_Get3DParamProc)(PCTRL_3D_PARAM pCtrl3DParam);
typedef struct tagMATHTOOLAPI
{
	MT_MapToBmpProc		MT_MapToBmp;
	MT_MapToBmpProc		MT_BmpToMap;
	MT_MapToBmpProc		MT_ZoomMapToBmp;
	MT_MapToBmpProc		MT_ZoomBmpToMap;
	MT_SaveOrShowBmpProc MT_SaveOrShowBmp;
	MT_SelectObjectProc	MT_SelectObject;
	MT_DrawObjectProc	MT_DrawObject;
	MT_SetBitmapProc	MT_SetBitmap;
	MT_Set3DParamProc	MT_Set3DParam;
	MT_Bmp2Dto3DProc	MT_Bmp2Dto3D;
	MT_Bmp2Dto3DProc	MT_Bmp3Dto2D;
	MT_Get3DParamProc	MT_Get3DParam;
}MATHTOOLAPI, *PMATHTOOLAPI;
//FileSystem Api
typedef nuBOOL (*FS_SaveUserConfigProc)();
typedef nuBOOL (*FS_ReLoadMapConfigProc)();
typedef nuBOOL (*FS_LoadResidentDataProc)(nuDWORD);
typedef nuPVOID (*FS_GetDataProc)(nuBYTE);
typedef nuBOOL(*FS_GetFilePathProc)(nuWORD, nuTCHAR*, nuWORD);
typedef nuBOOL(*FS_FindFileWholePathProc)(nuLONG, nuTCHAR*, nuWORD);
typedef nuWORD(*FS_GetMapCountProc)();
typedef nuBOOL(*FS_PtInBoundaryProc)(nuroPOINT);
typedef nuBOOL(*FS_LoadBlockProc)(nuPVOID);
typedef nuPVOID(*FS_GetScreenMapProc)();
typedef nuBOOL(*FS_GetRoadNameProc)(nuWORD, nuLONG, nuINT, nuPVOID, nuWORD, nuPVOID);
typedef nuBOOL(*FS_GetRoadIconInfoProc)(nuWORD, nuLONG, nuLONG, nuPVOID, nuWORD*); 
typedef nuBOOL(*FS_GetPoiNameProc)(nuWORD, nuLONG, nuPVOID, nuPWORD);
typedef nuBOOL(*FS_SavePtFileProc)(nuINT DwIdx);
typedef nuBOOL(*FS_SavePtFileCoverForDFTProc)(nuINT DwIdx);
typedef const nuPVOID(*FS_GetPtDataAddrProc)(nuLONG, nuLONG*);
typedef const nuPVOID(*FS_GetPtNodeByTypeProc)(nuLONG, nuBYTE, nuBYTE);
typedef nuBOOL(*FS_GetInfoByTypeProc)(nuLONG, nuLONG, nuLONG*, nuLONG*, nuLONG*, nuLONG*);
typedef nuINT(*FS_ReadConstWcsProc)(nuBYTE, nuLONG, nuWCHAR*, nuWORD);
typedef nuINT(*FS_ReadWcsFromFileProc)(nuTCHAR*, nuDWORD, nuWCHAR*, nuWORD);
typedef nuBOOL(*FS_ReadInfoMapCenterNameProc)();
typedef nuPVOID(*FS_GetCCDDataProc)(const nuroRECT& );
typedef nuUINT(*FS_GetCCDNameProc)(nuLONG nMapIdx, nuDWORD nNameAddr, nuPVOID pBuff, nuUINT nBuffLen);
typedef nuBOOL(*FS_SeekForRoadProc)(nuLONG, nuLONG, nuDWORD, nuLONG, PSEEKEDROADDATA);
typedef nuBOOL(*FS_PaddingRoadProc)(struct tagSEEKEDROADDATA*);
typedef nuBOOL(*FS_SeekForPoiProc)(nuLONG, nuLONG, nuDWORD, nuLONG, PSEEKEDPOIDATA);
typedef nuBOOL(*FS_SaveLastPosProc)(nuPVOID);
typedef nuBOOL(*FS_ReadCityTownNameProc)(nuLONG, nuDWORD, nuWCHAR*, nuWORD);
typedef nuBOOL(*FS_ReadPaDataProc)(nuLONG, nuDWORD, PPOIPANODE);
#define MAP_ID_TO_INDEX					0x01
#define MAP_INDEX_TO_ID					0x02
typedef nuLONG(*FS_MapIDIndexProc)(nuLONG nID, nuBYTE nType);
typedef nuPVOID(*FS_LoadDwBlockProc)(const nuroRECT& rtMap, nuDWORD nDataMode);
typedef nuVOID(*FS_FreeDwBlockProc)(nuDWORD nDataMode);
typedef nuVOID(FS_RDNameProcProc)(nuWCHAR* pWsName, nuWORD nNameNum);
typedef nuBOOL(*FS_SeekForRoadsProc)(PSEEK_ROADS_IN pSeekRdsIn, PSEEK_ROADS_OUT pSeekRdsOut, nuBOOL bSetMarket);
typedef struct tagROADFOREEYES
{
	nuWORD	nDwIdx;
	nuBYTE	nRoadClass;
	nuBYTE	nOneWay;
}ROADFOREEYES, *PROADFOREEYES;
typedef nuBOOL(*FS_CheckEEyesProc)(nuroPOINT point, nuLONG nDis, nuSHORT nAngle, PROADFOREEYES pRdForEEyes);
typedef nuBOOL(*FS_GetRoadSegDataProc)(nuWORD nDwIdx, PROAD_SEG_DATA pRoadSegData);
typedef nuUINT(*FS_SetWaypointsProc)(nuWORD idx, nuLONG x, nuLONG y, nuWCHAR* pwsName);
typedef nuUINT(*FS_DelWaypointsProc)(nuWORD idx);
typedef struct tagPARAM_GETWAYPOINTS
{
	nuWORD			nNowCount;
	nuroPOINT		ptNextCoor;
	PWAYPOINTS_NODE	pWaypointsList;
}PARAM_GETWAYPOINTS, *PPARAM_GETWAYPOINTS;
typedef nuUINT(*FS_GetWaypointsProc)(PPARAM_GETWAYPOINTS pParam);
typedef nuUINT(*FS_KptWaypointsProc)(nuLONG x, nuLONG y);
//forSearchDy
typedef nuUINT(*FS_SHDYStartProc)();
typedef nuUINT(*FS_SHDYStopProc)();
typedef nuUINT(*FS_SHDYSetRegionProc)(nuBYTE nSetMode, nuDWORD nData);
typedef nuUINT(*FS_SHDYSetDanyinProc)(nuWCHAR *pWsDy, nuBYTE nWsNum);
typedef nuUINT(*FS_SHDYGetHeadProc)(PDYCOMMONFORUI pDyHead);
typedef nuUINT(*FS_SHDYGetPageDataProc)(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx);
typedef nuUINT(*FS_SHDYGetPageStrokeProc)(PDYDATAFORUI pDyDataForUI, nuBYTE nStroke);
typedef nuUINT(*FS_SHDYGetOneDetailProc)(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
typedef nuUINT(*FS_SHDYCityTownNameProc)(PDYCITYTOWNNAME pDyCTname, nuLONG nCityID);
typedef nuPVOID(*FS_GetMpnDataProc)();
typedef struct tagCCD2_CHECK
{
	nuroPOINT	ptCar;
	nuSHORT		nCarAngle;
	nuWORD		nCcdMapIdx;
	nuDWORD		nCcdBlkIdx;
	nuDWORD		nCcdRdIdx;
}CCD2_CHECK,*PCCD2_CHECK;
/*
typedef struct tagLOOKFOR_NAVI_ROAD_IN
{
	nuroPOINT	point;
	nuWORD		nCoverDis;
	nuSHORT		nAngle;
	nuSHORT		nPointType;//Poi, Road, MapCenter and so on...
}LOOKFOR_NAVI_ROAD_IN, *PLOOKFOR_NAVI_ROAD_IN;
typedef nuUINT(*FS_LookForNaviRoadProc)(PLOOKFOR_NAVI_ROAD_IN pNaviIn, PSEEKEDROADDATA pRoadData);
*/
typedef nuUINT(*FS_SeekForCcdInfoProc)(nuroRECT rtMap, PCCDINFO pCcdInfo);
typedef nuUINT(*FS_CheckNewCcdProc)(PCCDINFO pCcdInfo);

typedef nuUINT(*FS_Get_TMC_VD_Proc)(nuCHAR*);                    //prosper 
typedef nuBOOL(*FS_Get_TMC_FM_XML_Proc)(nuUINT& nTotal_Count,nuUINT* nFm_Count,nuUINT& nXML_Count,nuUINT* nXML_Complex_Count); //prosper 06.16
typedef nuBOOL(*FS_Get_VD_Proc)(nuVOID* ,const nuUINT);
typedef nuBOOL(*FS_Get_TMC_Router_Proc)(nuVOID* ,const nuUINT);
typedef nuBOOL(*FS_Get_FM_XML_Proc)(nuVOID* ,const nuUINT);
typedef nuBOOL(*FS_Get_XML_Proc)(nuVOID* ,const nuUINT,nuUINT);     ////prosper 06.16
typedef nuBOOL(*FS_Set_Byte_Proc)(nuVOID* );					////prosper 07.25
typedef nuBOOL(*FS_ChangePoiInfoProc)(/*InPut*/NUROPOINT PoiCoor ,/*output*/POI_Info_Data& PoiData); //Shyanx 2012.02.13

//-----------added by daniel 20121205-----------------------//
typedef KMInfoData*(*FS_GetKMDataDLProc)(nuLONG BlockIdx, /*output*/nuLONG &Counter);
typedef KMInfoData*(*FS_GetKMDataMappingProc)(const SEEKEDROADDATA &RoadData, nuLONG &Counter);
typedef nuUINT(*FS_SeekForKMInfoProc)(nuroRECT rtMap, PKMINFO pKMInfo);
typedef nuBOOL(*FS_CheckKMInfoProc)(PKMINFO pKMInfo, nuWCHAR *wsKilometer, nuLONG &lKMNum,nuLONG &lKMDis, nuWCHAR *wsKMName);
typedef nuLONG(*FS_FindSLPProc)(nuLONG lMapIdx,nuDWORD nBlkIdx,nuDWORD nRoadIdx,PSLPDATA TempData);
typedef nuBOOL(*FS_SearchETGProc)(nuLONG lBlockIdx, nuLONG RoadIdx, NUROPOINT ptRoad, NUROPOINT ptCar, NUROPOINT &ptEtagPoint);
typedef nuBOOL(*FS_SeekForNextRoadInfoProc)(nuroRECT rtMap, PNEXTROADINFO pNextRoadInfo);
typedef nuBOOL(*FS_SearchICTProc)(nuLONG lBlockID, nuLONG lRoadIdx, NUROPOINT ptCar, NUROPOINT &ptEtagPoint, nuLONG &HighwayID, nuLONG &HghwayCrossID);
typedef nuUINT(*FS_SeekForCrossRoadInfoProc)(nuroRECT rtMap, PCROSSROADINFO pCorssRoadInfo);
//-----------added by daniel 20121205-----------------------//
typedef nuUINT(*FS_GetTMCTrafficInfoProc)(nuBYTE nDWIdx, nuUINT BlockID, nuUINT RoadID);
typedef nuUINT(*FS_GetTMCTrafficInfoByNodeIDProc)(nuBYTE nDWIdx, nuUINT NodeID1, nuUINT NodeID2);
typedef nuUINT(*FS_GetTMCDirInfoProc)(nuBYTE nDWIdx, nuUINT BlockID, nuUINT RoadID);
typedef nuUINT(*FS_GetTMCDirInfoByNodeIDProc)(nuBYTE nDWIdx, nuUINT NodeID1, nuUINT NodeID2);
typedef nuBOOL(*FS_COPY_TMC_Event_DataProc)(nuBYTE nDWIdx);
typedef TMCRoadInfoStru*(*FS_GetTMCDataInfoProc)(nuBYTE nDWIdx, nuUINT BlockID, nuUINT RoadID);
typedef nuBOOL(*FS_Initial_TMC_Event_DataProc)(nuBYTE nDWIdx);
typedef nuVOID(*FS_Release_TMC_Event_DataProc)(nuBYTE nDWIdx);
typedef nuBOOL(*FS_Initial_TMC_DataProc)(nuINT nCount);
typedef nuVOID(*FS_Release_TMC_DataProc)();
typedef struct tagFILESYSAPI
{
	FS_SaveUserConfigProc	FS_SaveUserConfig;
	FS_SaveUserConfigProc	FS_CoverFromDefault;
	FS_ReLoadMapConfigProc	FS_ReLoadMapConfig;
	FS_LoadResidentDataProc	FS_LoadResidentData;
	FS_GetDataProc	FS_GetData;
	FS_GetFilePathProc	FS_GetFilePath;
	FS_FindFileWholePathProc	FS_FindFileWholePath;
	FS_GetMapCountProc	FS_GetMapCount;
	FS_PtInBoundaryProc FS_PtInBoundary;
	FS_LoadBlockProc	FS_LoadBlock;
	FS_GetScreenMapProc FS_GetScreenMap;
	FS_GetRoadNameProc	FS_GetRoadName;
	FS_GetRoadIconInfoProc FS_GetRoadIconInfo; //add @ 2012.04.06
	FS_GetPoiNameProc	FS_GetPoiName;
	FS_SavePtFileProc	FS_SavePtFile;
	FS_SavePtFileCoverForDFTProc FS_SavePtFileCoverForDFT;
	FS_GetPtDataAddrProc	FS_GetPtDataAddr;
	FS_GetPtNodeByTypeProc	FS_GetPtNodeByType;
	FS_GetInfoByTypeProc	FS_GetInfoBySourceType;
	FS_GetInfoByTypeProc	FS_GetInfoByNuroType;
	FS_ReadConstWcsProc	FS_ReadConstWcs;
	FS_ReadWcsFromFileProc	FS_ReadWcsFromFile;
	FS_ReadInfoMapCenterNameProc	FS_ReadInfoMapCenterName;
	FS_GetCCDDataProc	FS_GetCCDData;
	FS_GetCCDNameProc	FS_GetCCDName;
	FS_SeekForRoadProc	FS_SeekForRoad;
	FS_PaddingRoadProc	FS_PaddingRoad;
	FS_SeekForPoiProc	FS_SeekForPoi;
	FS_SaveLastPosProc	FS_SaveLastPos;
	FS_ReadCityTownNameProc	FS_ReadCityTownName;
	FS_ReadPaDataProc	FS_ReadPaData;
	FS_MapIDIndexProc	FS_MapIDIndex;
	FS_LoadDwBlockProc	FS_LoadDwBlock;
	FS_FreeDwBlockProc	FS_FreeDwBlock;
	FS_RDNameProcProc	FS_RDNameProc;
	FS_SeekForRoadsProc	FS_SeekForRoads;
	FS_CheckEEyesProc	FS_CheckEEyes;
	FS_GetRoadSegDataProc	FS_GetRoadSegData;
	FS_GetMpnDataProc	FS_GetMpnData;
	FS_SetWaypointsProc	FS_SetWaypoints;
	FS_DelWaypointsProc	FS_DelWaypoints;
	FS_GetWaypointsProc	FS_GetWaypoints;
	FS_KptWaypointsProc	FS_KptWaypoints;
	FS_SeekForCcdInfoProc	FS_SeekForCcdInfo;
	FS_CheckNewCcdProc	FS_CheckNewCcd;

	FS_Get_TMC_VD_Proc		FS_Get_TMC_VD_SIZE;          //prosper 
	FS_Get_TMC_FM_XML_Proc	FS_Get_TMC_FM_XML_SIZE;
	FS_Get_VD_Proc			FS_Get_TMC_VD;
    FS_Get_FM_XML_Proc		FS_Get_TMC_FM_XML;
	FS_Get_XML_Proc			FS_Get_TMC_FM;
	FS_Get_XML_Proc			FS_Get_TMC_XML;
	FS_Get_XML_Proc			FS_Get_TMC_XML_L;
	FS_Set_Byte_Proc		FS_Set_TMC_Byte;
	FS_Get_TMC_Router_Proc  FS_Get_TMC_Router;
	FS_ChangePoiInfoProc    FS_ChangePoiInfo;      //Shyanx 2012.02.13
	//----added by daniel 20121205-----------------//
	FS_GetKMDataDLProc			 FS_GetKMDataDL;
	FS_GetKMDataMappingProc		 FS_GetKMDataMapping;
	FS_SeekForKMInfoProc		 FS_SeekForKMInfo;
	FS_CheckKMInfoProc			 FS_CheckKMInfo;
	FS_FindSLPProc				 FS_FindSLP;
	FS_SeekForPoiProc			 FS_SeekForPoiType;
	FS_SearchETGProc			 FS_SearchETG;
	FS_SeekForNextRoadInfoProc	 FS_SeekForNextRoadInfo;
	FS_SearchICTProc			 FS_SearchICT;
	FS_SeekForCrossRoadInfoProc  FS_SeekForCorssRoadInfo;
	//----added by daniel 20121205-----------------//
	FS_GetTMCTrafficInfoProc             FS_GetTMCTrafficInfo;
    FS_GetTMCTrafficInfoByNodeIDProc     FS_GetTMCTrafficInfoByNodeID;
	FS_GetTMCDirInfoProc                 FS_GetTMCDirInfo;
	FS_GetTMCDirInfoByNodeIDProc         FS_GetTMCDirInfoByNodeID;
	FS_COPY_TMC_Event_DataProc			 FS_COPY_TMC_Event_Data;
	FS_GetTMCDataInfoProc                FS_GetTMCDataInfo;
    FS_Initial_TMC_Event_DataProc        FS_Initial_TMC_Event_Data;
	FS_Release_TMC_Event_DataProc        FS_Release_TMC_Event_Data;
    FS_Initial_TMC_DataProc              FS_Initial_TMC_Data;
	FS_Release_TMC_DataProc              FS_Release_TMC_Data;
//	FS_LookForNaviRoadProc		FS_LookForNaviRoad;

//	FS_SHDYStartProc			FS_SHDYStart;
//	FS_SHDYStopProc				FS_SHDYStop;
//	FS_SHDYSetRegionProc		FS_SHDYSetRegion;
//	FS_SHDYSetDanyinProc		FS_SHDYSetDanyin;
//	FS_SHDYGetHeadProc			FS_SHDYGetHead;
//	FS_SHDYGetPageDataProc		FS_SHDYGetPageData;
//	FS_SHDYGetPageStrokeProc	FS_SHDYGetPageStroke;
//	FS_SHDYGetOneDetailProc		FS_SHDYGetOneDetail;
//	FS_SHDYCityTownNameProc		FS_SHDYCityTownName;


	PGLOBALENGINEDATA	pGdata;//Global data pointer in NaviCtrl
	PMAPCONFIG			pMapCfg;
	PDRAWINFO			pDrawInfo;
	PUSERCONFIG			pUserCfg;
}FILESYSAPI, *PFILESYSAPI;

//GDI Api
typedef nuVOID(*gdiShowStringProc)(const nuTCHAR*);
typedef nuVOID(*gdiPrintfNumProc)(nuLONG);
typedef nuBOOL(*GdiInitDCProc)(nuHDC, nuINT, nuINT);
typedef nuVOID(*GdiFreeDCProc)();
typedef nuBOOL(*GdiInitBMPProc)(nuUINT, nuUINT);
typedef nuVOID(*GdiFreeBMPProc)();
typedef nuBOOL(*GdiLoadNuroBMPProc)(PNURO_BMP, const nuTCHAR *);
typedef nuBOOL(*GdiCreateNuroBMPProc)(PNURO_BMP);
typedef nuVOID(*GdiDelNuroBMPProc)(PNURO_BMP);
typedef PNURO_BMP(*GdiSelectCanvasBMPProc)(PNURO_BMP);
typedef nuBOOL(*GdiSetPenProc)(PCNUROPEN);
typedef nuVOID(*GdiDelPenProc)();
typedef nuBOOL(*GdiSetBrushProc)(PCNUROBRUSH);
typedef nuVOID(*GdiDelBrushProc)();
typedef nuBOOL(*GdiSetFontProc)(PNUROFONT pNuroFont);
typedef nuVOID(*GdiDelFontProc)();
typedef nuBOOL(*GdiFlushProc)(nuINT, nuINT, nuINT, nuINT, nuINT, nuINT);
typedef nuBOOL(*GdiDrawlineProc)(nuINT, nuINT, nuINT, nuINT, nuINT, nuINT, nuINT);
typedef nuBOOL(*GdiPolylineProc)(const nuroPOINT*, nuINT);
typedef nuBOOL(*GdiPolygonProc)(const nuroPOINT* pPt, nuINT nCount);
typedef nuBOOL(*GdiEllipseProc)(nuINT, nuINT, nuINT, nuINT);
typedef nuCOLORREF(*GdiSetTextColorProc)(nuCOLORREF);
typedef nuCOLORREF(*GdiSetBKColorProc)(nuCOLORREF);
typedef nuINT(*GdiSetBkModeProc)(nuINT);
typedef nuBOOL(*GdiExtTextOutWProc)(nuINT, nuINT, nuWCHAR*, nuUINT);
typedef nuINT(*GdiDrawTextWProc)(nuWCHAR*, nuINT, PNURORECT, nuUINT);
typedef nuBOOL(*GdiExtTextOutWExProc)(nuINT, nuINT, nuWCHAR*, nuUINT, nuBOOL, nuCOLORREF);
typedef nuINT(*GdiDrawTextWExProc)(nuWCHAR*, nuINT, PNURORECT, nuUINT, nuBOOL, nuCOLORREF);
typedef nuBOOL(*GdiDrawIconProc)(nuINT, nuINT, nuLONG);
typedef nuBOOL(*GdiDrawTargetFlagIconProc)(nuINT x, nuINT y, nuLONG Icon);
typedef nuBOOL(*GdiDrawIconExternProc)(nuINT, nuINT, nuLONG,nuLONG);
typedef nuBOOL(*GdiDrawIconSpProc)(nuINT, nuINT, nuLONG, nuBYTE);
typedef nuBOOL(*GdiDashLineProc)(const nuroPOINT*, nuINT, nuINT);
typedef nuBOOL(*GdiSaveBMPProc)(PNURO_BMP, nuINT, nuINT, nuINT, nuINT, nuINT, nuINT);
typedef nuBOOL(*GdiDrawBMPProc)(nuINT, nuINT, nuINT, nuINT, const NURO_BMP*, nuINT, nuINT);
typedef nuBOOL(*GdiDrawPNGProc)(nuINT, nuINT, nuINT, nuINT, nuINT, nuINT, nuINT);
typedef nuBOOL(*GdiDrawPNG2Proc)(nuINT, nuINT, nuINT, nuINT, PPNGCTRLSTRU, nuINT, nuINT);
typedef nuBOOL(*GdiPaintBMPProc)(nuINT, nuINT, nuINT, nuINT, const PNURO_BMP);
typedef nuUINT(*GdiDrawPoiProc)(nuINT, nuINT, nuLONG, nuUINT, nuBOOL);
typedef nuBOOL(*GdiDrawIconExProc)(nuINT, nuINT, nuLONG, nuBYTE, nuBYTE);
typedef nuBOOL(*GdiZoomBmpProc)(PNURO_BMP pDesBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
			                      , const PNURO_BMP pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT nWidthSrc, nuINT nHeightSrc);
#define NURO_RTPOS_LT		0x00
#define NURO_RTPOS_LC		0x01
#define NURO_RTPOS_LB		0x02
#define NURO_RTPOS_CT		0x03
#define NURO_RTPOS_CC		0x04
#define NURO_RTPOS_CB		0x05
#define NURO_RTPOS_RT		0x06
#define NURO_RTPOS_RC		0x07
#define NURO_RTPOS_RB		0x08
//
typedef nuINT(*GdiSetROP2Proc)(nuINT);
typedef nuBOOL(*GdiPolyExProc)(const nuroPOINT*, nuINT, nuINT);

typedef nuVOID(*GdiSet3DParaProc)(const PFUNC_FOR_GDI pForGDI);
typedef nuBOOL(*Gdi3DPolylineProc)(const NUROPOINT* pPt, nuINT nCount);

typedef PNURO_BMP(*GdiGetCanvasBmpProc)();
typedef nuBOOL(*GdiBmpGradientProc)(nuINT nDesX, 
								  nuINT nDesY, 
								  nuINT nWidth, 
								  nuINT nHeight, 
								  const NURO_BMP* pBmp, 
								  nuINT nSrcX, 
								  nuINT nSrcY,
								  nuINT nMode);

typedef struct tagGDIAPI
{
	////////////////////////////////////////
	gdiShowStringProc		gdiShowString;
	gdiPrintfNumProc		gdiPrintfNum;
	GdiInitDCProc			GdiInitDC;
	GdiFreeDCProc			GdiFreeDC;
	GdiInitBMPProc			GdiInitBMP;
	GdiFreeBMPProc			GdiFreeBMP;
	GdiLoadNuroBMPProc		GdiLoadNuroBMP;
	GdiCreateNuroBMPProc		GdiCreateNuroBMP;
	GdiDelNuroBMPProc		GdiDelNuroBMP;
	GdiSelectCanvasBMPProc		GdiSelectCanvasBMP;
	GdiSetPenProc			GdiSetPen;
	GdiDelPenProc			GdiDelPen;
	GdiSetBrushProc			GdiSetBrush;
	GdiDelBrushProc			GdiDelBrush;
	GdiSetFontProc			GdiSetFont;
	GdiDelFontProc			GdiDelFont;
	GdiFlushProc			GdiFlush;
    GdiDrawlineProc			GdiDrawLine;
	GdiPolylineProc			GdiPolyline;
	GdiPolygonProc			GdiPolygon;
	GdiPolygonProc			GdiPolygonX;
	GdiEllipseProc			GdiEllipse;
	GdiSetTextColorProc		GdiSetTextColor;
	GdiSetBKColorProc		GdiSetBKColor;
	GdiSetBkModeProc		GdiSetBkMode;
	GdiExtTextOutWProc		GdiExtTextOutW;
	GdiDrawTextWProc		GdiDrawTextW;
	GdiDrawIconProc			GdiDrawIcon;
	GdiDrawTargetFlagIconProc	GdiDrawTargetFlagIcon;
	GdiDrawIconExternProc	GdiDrawIconExtern;
	GdiDrawIconSpProc		GdiDrawIconSp;
	GdiDashLineProc			GdiDashLine;
	GdiSaveBMPProc			GdiSaveBMP;
	GdiDrawBMPProc			GdiDrawBMP;
	GdiDrawPNGProc			GdiDrawPNG;
	GdiDrawPNG2Proc			GdiDrawPNG2;
	GdiPaintBMPProc			GdiPaintBMP;
	GdiDrawPoiProc			GdiDrawPoi;
	GdiSetROP2Proc			GdiSetROP2;
	GdiPolyExProc			GdiPolylineEx;
	GdiPolyExProc			GdiPolygonEx;
	GdiExtTextOutWExProc	GdiExtTextOutWEx;
	GdiDrawTextWExProc		GdiDrawTextWEx;
	GdiSet3DParaProc		GdiSet3DPara;
	Gdi3DPolylineProc		Gdi3DPolyline;
	Gdi3DPolylineProc		Gdi3DPolylineX;
	GdiGetCanvasBmpProc		GdiGetCanvasBmp;
	GdiDrawIconExProc       GdiDrawIconEx;
	GdiBmpGradientProc		GdiBmpGradient;
	GdiZoomBmpProc			GdiZoomBmp;
}GDIAPI, *PGDIAPI;

//MemMgr Api
typedef nuPVOID(*MM_GetStaticMemMassProc)(nuDWORD);
typedef nuBOOL(*MM_RelStaticMemMassAfterProc)(nuBYTE*);
typedef nuPVOID(*MM_GetTmpStaticMemMassProc)(nuDWORD);
typedef nuPVOID(*MM_GetDataMemMassProc)(nuDWORD, nuPWORD);
typedef nuBOOL(*MM_RelDataMemMassProc)(nuPWORD);
typedef nuPVOID(*MM_GetDataMassAddrProc)(nuWORD);
typedef nuVOID**(*MM_AllocMemProc)(nuDWORD);
typedef nuBOOL(*MM_FreeMemProc)(nuVOID**);
typedef nuBOOL(*MM_CollectDataMemProc)(nuBYTE nType);
typedef nuVOID(*MM_VoidVoidProc)();
typedef nuVOID(*MM_GetNowMemoryUsedProc)(nuDWORD *MemoryUsedCount);//added by daniel
typedef struct tagMEMMGRAPI
{
	//llllllllllllllll
	MM_GetNowMemoryUsedProc			MM_GetNowMemoryUsed;//added by daniel
	MM_GetStaticMemMassProc			MM_GetStaticMemMass;
	MM_RelStaticMemMassAfterProc	MM_RelStaticMemMassAfter;
	MM_GetTmpStaticMemMassProc		MM_GetTmpStaticMemMass;
	MM_GetDataMemMassProc			MM_GetDataMemMass;
	MM_RelDataMemMassProc			MM_RelDataMemMass;
	MM_GetDataMassAddrProc			MM_GetDataMassAddr;
	MM_AllocMemProc					MM_AllocMem;
	MM_FreeMemProc					MM_FreeMem;
	MM_CollectDataMemProc			MM_CollectDataMem;
	MM_VoidVoidProc					MM_LockDataMemMove;
	MM_VoidVoidProc					MM_FreeDataMemMove;
	nuPVOID							pServe[20-11];
}MEMMGRAPI, *PMEMMGRAPI;
typedef struct tagMEMMGRAPI_SH
{
	MM_GetNowMemoryUsedProc			MM_GetNowMemoryUsed;//added by daniel
	MM_GetStaticMemMassProc			MM_GetStaticMemMass;
	MM_RelStaticMemMassAfterProc	MM_RelStaticMemMassAfter;
	MM_GetTmpStaticMemMassProc		MM_GetTmpStaticMemMass;
	MM_GetDataMemMassProc			MM_GetDataMemMass;
	MM_RelDataMemMassProc			MM_RelDataMemMass;
	MM_GetDataMassAddrProc			MM_GetDataMassAddr;
	MM_AllocMemProc					MM_AllocMem;
	MM_FreeMemProc					MM_FreeMem;
	MM_CollectDataMemProc			MM_CollectDataMem;
	MM_VoidVoidProc					MM_LockDataMemMove;
	MM_VoidVoidProc					MM_FreeDataMemMove;

	nuPVOID							pServe[20-11];
}MEMMGRAPI_SH, *PMEMMGRAPI_SH;
//Search
typedef nuBOOL(*SH_StartSearchProc)(nuWORD nMMIdx, nuUINT nMMLen);
typedef nuVOID(*SH_EndSearchProc)();
typedef nuINT  (*SH_GETXXLISTProc)(NURO_SEARCH_TYPE st, nuWORD nMaxRes, nuVOID* pBuffer, nuUINT nMaxLen);

typedef nuBOOL (*SH_GetPoiCityNameProc)(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx);
typedef nuBOOL (*SH_GetPoiTownNameProc)(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx);
typedef nuBOOL (*SH_GetPoiByPhoneProc)(const nuWCHAR *pPhone, SEARCH_POIINFO *pInfo);
typedef nuBOOL (*SH_GetPoiMoreInfoProc)(nuUINT nPoiListIdx, SEARCH_PA_INFO *pPaInfo, nuBOOL bRealIdx);
typedef nuBOOL (*SH_GetPoiPosProc)(nuUINT nPoiListIdx, NUROPOINT* pos, nuBOOL bRealIdx);
typedef nuUINT (*SH_GetChildRoadTotal_MRProc)(nuUINT nMainRoadIdx, nuBOOL bRealIdx);
typedef nuBOOL(*SH_GetRoadDoorInfoProc)(nuVOID *pBuffer, nuUINT nMaxLen);
typedef nuBOOL (*SH_GetRoadCrossPosProc)(nuUINT nResIdx, NUROPOINT* pos);
typedef nuBOOL (*SH_GetRoadCrossTownNameProc)(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen);
typedef nuBOOL (*SH_GetRoadPosProc)(NUROPOINT* pos);
typedef nuBOOL (*SH_GetRoadTownNameProc)(nuWORD *pBuffer, nuUINT nMaxLen);
typedef nuBOOL (*SH_SetMapIdProc)(nuLONG id);
typedef nuBOOL (*SH_SetCityIdProc)(nuWORD resIdx);
typedef nuBOOL (*SH_SetTownIdProc)(nuWORD resIdx);
typedef nuBOOL (*SH_SetPoiTP1Proc)(nuUINT resIdx);


typedef nuBOOL (*SH_SetPoiTP2Proc)(nuUINT resIdx);
typedef nuBOOL (*SH_SetPoiKeyDanyinProc)(nuWCHAR *pDanyin,nuBOOL bComplete);
typedef nuBOOL (*SH_SetPoiKeyNameProc)(nuWCHAR *pName, nuBOOL bFuzzy);
typedef nuBOOL (*SH_SetPoiKeyPhoneProc)(nuWCHAR *pPhone);
typedef nuBOOL (*SH_SetSearchAreaProc)(NURORECT *pRect);
typedef nuBOOL (*SH_SetRoadSZSLimitProc)(nuUINT nMin, nuUINT nMax);
typedef nuBOOL (*SH_SetRoadSZZhuyinProc)(nuWORD zhuyin);

typedef nuBOOL (*SH_SetRoadSZProc)(nuUINT nResIdx);

typedef nuBOOL (*SH_SetRoadKeyDanyinProc)(nuWCHAR *pDanyin);
typedef nuBOOL (*SH_SetRoadKeyNameProc)(nuWORD *pKeyWord, nuBOOL bIsFromBeging );
typedef nuBOOL (*SH_SetRoadNameProc)(nuUINT nResIdx, nuBOOL bRealIdx);
typedef	nuUINT (*SH_GetXXXTotalProc)();

typedef nuINT (*SH_GetXXXAllProc)(nuVOID *pBuffer, nuUINT nMaxLen);
typedef nuINT  (*SH_GetRoadNameALL_MRProc)(nuUINT nMainRoadIdx, nuVOID* pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx);
typedef nuBOOL (*SH_SetRoadName_MRProc)(nuUINT nMainRoadIdx, nuUINT nRoadIdx, nuBOOL bRealIdx);
typedef nuINT  (*SH_GetFilterResAllProc)(const nuWCHAR *pKey, nuVOID *pBuffer, nuUINT nMaxLen);

typedef nuUINT (*SH_SHDYStartProc)();
typedef nuUINT (*SH_SHDYStopProc)();
typedef nuUINT (*SH_SHDYSetRegionProc)(nuBYTE nSetMode, nuDWORD nData);
typedef nuUINT (*SH_SHDYSetDanyinProc)(nuWCHAR *pWsDy, nuBYTE nWsNum);
typedef nuUINT (*SH_SHDYSetDanyinKeynameProc)(nuWCHAR *pWsDy, nuBYTE nWsNum);
typedef nuUINT (*SH_SHDYGetHeadProc)(PDYCOMMONFORUI pDyHead);
typedef nuUINT (*SH_SHDYGetPageDataProc)(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx);
typedef nuUINT (*SH_SHDYGetPageStrokeProc)(PDYDATAFORUI pDyDataForUI, nuBYTE nStroke);
typedef nuUINT (*SH_SHDYGetOneDetailProc)(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
typedef nuUINT (*SH_SHDYCityTownNameProc)(PDYCITYTOWNNAME pDyCTname, nuLONG nCityID);
typedef nuUINT (*SH_SHDYGetWordDataProc)(PDYWORDFORUI pDyData); //Prosper Add 20121216

typedef nuVOID (*SH_SetStrokeLimIdxProc)(nuUINT nResIdx);

typedef nuUINT (*SH_GetPoiCarFacAllProc)(nuVOID *pBuffer, nuUINT nMaxLen);
typedef nuBOOL (*SH_SetPoiCarFacProc)(nuUINT nResIdx);
typedef nuUINT (*SH_GetPoiSPTypeAllProc)(nuVOID *pBuffer, nuUINT nMaxLen);
typedef nuBOOL (*SH_SetPoiSPTypeProc)(nuUINT nResIdx);

typedef nuUINT (*SH_CollectNextWordAllProc)(nuWORD* pHeadWords, nuWORD *pBuffer, nuUINT nMaxWords);

typedef nuBOOL (*SH_LDCreateDataFileProc)(nuTCHAR *pFileName
										  , nuUINT nMax, nuUINT nEachRecordLen, nuINT mode);
typedef nuBOOL (*SH_LDOpenFileProc)(nuTCHAR *pFileName);
typedef nuVOID (*SH_LDCloseFileProc)();
typedef nuBOOL (*SH_LDGetFileHeaderProc)(LOGDATAFILEHEADER *fileHeader);
typedef nuBOOL (*SH_LDAddRecordProc)(nuVOID *pBuffer, nuUINT nLen);
typedef nuVOID (*SH_LDDelRecordProc)(nuUINT idx);
typedef nuVOID (*SH_LDClearRecordProc)();
typedef nuUINT (*SH_LDGetRecordProc)(nuUINT sIdx, nuUINT nMax
									 , nuVOID *pBuffer, nuUINT nBufferLen);
typedef nuBOOL (*SH_GetMapInfoProc)(nuLONG mapid, SEARCH_MAP_INFO &mapinfo);

typedef nuBOOL  (*SH_StartSKBSearchProc)(nuUINT nType);
typedef nuVOID  (*SH_StopSKBSearchProc)();
typedef nuINT   (*SH_GetSKBAllProc)(nuCHAR* keyStr, nuCHAR* pBuffer, nuINT nMaxLen, nuINT* resCount);
typedef nuBOOL  (*SH_NEWSEARCH_SDKProc)(PCALL_BACK_PARAM pParam);

//@zhikun 2009.07.16
#define SH_BEGIN_TYPE_NO					0x00
#define SH_BEGIN_TYPE_ROAD					0x01
#define SH_BEGIN_TYPE_CROSS					0x02
#define SH_BEGIN_TYPE_POI					0x03
#define SH_BEGIN_TYPE_PY					0x04
#define SH_BEGIN_TYPE_SPDOORNUM				0x05
typedef nuUINT (*SH_ShBeginZProc)(nuUINT nType);
typedef nuUINT (*SH_ShEndZProc)();
//@zhikun 2009.07.27
#define SH_ENTER_MODE_NO					0x00//NO Search
#define SH_ENTER_MODE_DY					0x01//Tw:Danyin; China:Pinyin;
#define SH_ENTER_MODE_NAME					0x02//name string
#define SH_ENTER_MODE_NURO					(1<<16)
#define SH_ENTER_MODE_PANA					(2<<16)
typedef nuUINT (*SH_ShSetModeZProc)(nuUINT nMode);
#define SH_INPUT_FUZZY						0x01
#define SH_INPUT_STROKE						0x02
#define SH_INPUT_KEYWORD					0x03//Taiwan is DY; China is PY;
typedef nuUINT (*SH_ShSetInputProc)(nuUINT nInputType);
#define SH_REGION_MAP						0x00
#define SH_REGION_CITY						0x01
#define SH_REGION_TOWN						0x02
#define SH_REGION_NONAMENUMBER				0x03 //Search_NoNumberData 
#define SH_REGION_NAMENUMBER				0x04 //Search_NoNumberData
typedef nuUINT (*SH_ShSetRegionZProc)(nuBYTE nSetMode, nuDWORD nData);
typedef nuUINT (*SH_ShSetStringZProc)(nuCHAR *pBuff, nuWORD nBuffLen);
typedef struct tagSH_HEADER_FORUI
{
	PDYKEYMASK	pDyKeyMask;
	nuDWORD		nTotalCount;//TotalCount 全省
}SH_HEADER_FORUI, *PSH_HEADER_FORUI;
typedef nuUINT (*SH_ShGetHeadZProc)(PSH_HEADER_FORUI pShHeader);

#define SH_NAME_FORUI_MAX_COUNT				8//一次能取回来的最大资料笔数
#define SH_NAME_FORUI_NUM					30//一个名字的最大字符数
#define SH_NAME_FORUI_CITYORTOWN_NUM		11//一个City或town名字的最大字符数
#define SH_NAME_FORUI_OTHER_NUM				250
typedef struct tagSH_ITEM_FORUI
{
	nuBYTE	nStroke;
	nuBYTE	nType;
	nuWORD	nCityID:6;
	nuWORD	nTownID:10;
	nuDWORD	reserve[4];
	nuWCHAR	wsName[SH_NAME_FORUI_NUM];
	nuWCHAR	wsCityName[SH_NAME_FORUI_CITYORTOWN_NUM];
	nuWCHAR	wsTownName[SH_NAME_FORUI_CITYORTOWN_NUM];
}SH_ITEM_FORUI, *PSH_ITEM_FORUI;
typedef struct tagSH_PAGE_FORUI
{
	nuBYTE  nPageItemCount;//In 一页的最大记录数
	nuBYTE	nThisPageCount;//In/out 传入的是现在显示页有几条记录，传出的是最新页有几条记录
	nuBYTE	nIdxSelected;//In/out 当前选中Item的Idx
	nuBYTE	nMinStroke;//Out
	nuBYTE	nMaxStroke;//Out搜所到的结果的最大和最小笔划数
	nuBYTE	nNowCityID;//In/out; -1 means all cities
	nuWORD	nNowTownID;//In/out; -1 means all towns
	nuWORD	nNowPageItemIdx;//In/out 当前页第一条是所有记录中的第几条，翻页可以由这个参数控制
	nuDWORD nTotalCount;//out 总共找到的笔数,和这次要求资料的模式有关
	nuDWORD	pNodeIdxList[SH_NAME_FORUI_MAX_COUNT];
	SH_ITEM_FORUI	dyNodeList[SH_NAME_FORUI_MAX_COUNT];//out
}SH_PAGE_FORUI, *PSH_PAGE_FORUI;
typedef struct tagSH_DETAIL_FORUI
{
	nuroPOINT	point;
	nuBYTE		nPhoneLen;
	nuBYTE		nSubBranchLen;
	nuBYTE		nFaxLen;
	nuBYTE		nAddressLen;
	nuBYTE		nWebLen;
	nuBYTE		nOpenTimeLen;
	nuWCHAR		wsOther[SH_NAME_FORUI_OTHER_NUM];
}SH_DETAIL_FORUI, *PSH_DETAIL_FORUI;
typedef nuUINT (*SH_ShGetPageDataZProc)(PSH_PAGE_FORUI pPageData, nuDWORD nItemIdx);
typedef nuUINT (*SH_ShGetPagePinyinZProc)(PSH_PAGE_FORUI pPageData, nuBYTE nPinyin);
typedef nuUINT (*SH_ShGetOneDetailZProc)(PSH_DETAIL_FORUI pOneDetail, nuDWORD nIdx);


#define SH_NAME_CITYTOWN_MAX_NUM					11
#define SH_NAME_CITYTOWN_MAX_COUNT					50
//@* nType
#define SH_CITYTOWN_TYPE_CITY_ALL					0x01
#define SH_CITYTOWN_TYPE_CITY_USED					0x02
#define SH_CITYTOWN_TYPE_TOWN_ALL					0x11
#define SH_CITYTOWN_TYPE_TOWN_USED					0x12
typedef struct tagSH_CITYTOWN_FORUI
{
	nuBYTE	nType;//in
	nuBYTE	nPageCount;//in
	nuWORD	nIDStart;//in
	nuWORD	nCityID;//in. It is usefull in Getting Town Name.
	nuBYTE	nTotalCount;//out
	nuBYTE  nNowCount;
	nuWORD	pIdList[SH_NAME_CITYTOWN_MAX_COUNT];
	nuWCHAR	ppName[SH_NAME_CITYTOWN_MAX_COUNT][SH_NAME_CITYTOWN_MAX_NUM];
}SH_CITYTOWN_FORUI, *PSH_CITYTOWN_FORUI;
typedef nuUINT (*SH_ShCityTownNameZProc)(PSH_CITYTOWN_FORUI pCityTown);
typedef struct tagSH_ROAD_DETAIL
{
	nuDWORD	nDrNumCount;
	nuDWORD nMinDrNum;
	nuDWORD nMaxDrnum;
}SH_ROAD_DETAIL, *PSH_ROAD_DETAIL;
typedef nuUINT (*SH_ShGetRoadDetailZProc)(PSH_ROAD_DETAIL pOneRoad, nuDWORD nIdx);
typedef nuUINT (*SH_ShGetDrNumCoorZProc)(nuDWORD nIdx, nuDWORD nDoorNum, nuroPOINT* point);

typedef nuUINT(*SH_GetPoiNearDisProc)(nuUINT nResIdx, nuBOOL bRealIdx);  // Added by Damon 20100201

typedef nuLONG(*SH_GetPoiCityIDProc)(nuUINT nResIdx, nuBOOL bRealIdx); // Added by Damon 20100309

typedef nuBOOL (*SH_SetPoiPZZhuyinProc)(nuWORD zhuyin);//prosper
typedef nuBOOL (*SH_SetPoiPZProc)(nuUINT nResIdx);//prosper

typedef nuVOID (*SH_Stroke_GetNextWordData)(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT &WordCont,nuVOID* pCaidy, const nuUINT candylen, const nuBOOL ReSort); // Added by Prosper 201105
typedef nuVOID (*SH_ZhuYinForSmart_SecondWords)(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont, const nuUINT candylen, const nuBOOL ReSort);//(nuVOID* pBuffer, nuINT nMaxLen,nuVOID* pBuffer2,nuINT n,nuINT* iCmpIdx);              // Added by Prosper 201105
typedef nuBOOL (*SH_ZhuYinForSmart_OtherWords) (nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort);   // Added by Prosper 201105

typedef nuINT  (*SH_Smart_GETXXLISTProc)(NURO_SEARCH_TYPE st, nuWORD nMaxRes, nuVOID* pBuffer, nuUINT nMaxLen,nuUINT word);        // Added by Prosper 201105

typedef nuVOID (*SH_Stroke_Poi_GetNextWordData)(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT &WordCont,nuVOID* pCaidy, const nuUINT candylen, const nuBOOL ReSort); // Added by Prosper 201105
typedef nuVOID (*SH_ZhuYinForSmart_Poi_SecondWords)(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont, const nuUINT candylen, const nuBOOL ReSort);//(nuVOID* pBuffer, nuINT nMaxLen,nuVOID* pBuffer2,nuINT n,nuINT* iCmpIdx);              // Added by Prosper 201105
typedef nuBOOL (*SH_ZhuYinForSmart_Poi_OtherWords) (nuVOID* pPoidBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort);   // Added by Prosper 201105

typedef nuBOOL (*SH_GetRoadSKBProc)(nuVOID* pSKBBuf, nuUINT& nCount); //Prosper 20111226
typedef nuBOOL (*SH_GetPoiSKBProc)(nuVOID* pSKBBuf, nuUINT& nCount);  //Prosper 20111226
typedef nuINT  (*SH_GetRoadTownIDProc)();

typedef nuBOOL (*SH_VoicePoiSetMapProc)(nuINT nIdx);
typedef nuBOOL (*SH_VoicePoiPosNameProc)(nuVOID* pCityName,nuVOID* pTownName, nuDWORD nIdx);
typedef nuBOOL (*SH_VoicePoiKeySearchProc)(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
typedef nuBOOL (*SH_VoicePoiEndProc)();
typedef nuBOOL (*SH_VoiceRoadNameProc)(VOICEROADFORUI data,nuVOID *wChangeName,nuVOID *wRoadName,nuroPOINT &point);
typedef nuINT  (*SH_ShGetListProc)(nuWORD& nMax,nuVOID* pBuffer, nuUINT nMaxLen);
typedef nuINT  (*SH_ShGetNextWordsProc)(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& n_iCount,nuVOID *wKeyWord, const nuUINT candylen, const nuBOOL ReSort);
typedef nuBOOL (*SH_ShSetNoNameProc) (nuBOOL bRes);
typedef nuUINT (*SH_SHDYSetSearchModeProc)(nuINT nMode);
typedef nuUINT (*SH_SHDYSetKeyProc)(nuWCHAR *pWsDy, nuBYTE nWsNum);
typedef nuBOOL (*SH_GetRoadWordDataProc)(nuINT &nTotal,nuVOID* pBuffer, nuUINT nMaxLen);
typedef nuUINT (*SH_ShGetWordDataProc)(PDYWORDFORUI pDyData);
typedef nuINT  (*SH_GetDYPoiTotalProc)();
typedef nuBOOL (*SH_ShGetDoorInfoProc)(SH_ROAD_DETAIL &data);
typedef nuINT  (*SH_ShGetDoorInfoDataProc)(nuVOID* pRoadBuf);
typedef nuWORD (*SH_GetRoadSZProc)(nuVOID* pRoadBuf);
typedef nuWORD (*SH_SearchNNRRoadSZProc)();

typedef nuINT (*SH_SetNNRRoadSZProc)(nuUINT nResIdx);

typedef struct tagSEARCHAPI
{
	SH_StartSearchProc	SH_StartSearch;//0
	SH_EndSearchProc	SH_EndSearch;//1
	SH_SetMapIdProc		SH_SetMapId;//2
	SH_SetCityIdProc	SH_SetCityId;//3
	SH_SetTownIdProc	SH_SetTownId;//4
	SH_SetPoiTP1Proc	SH_SetPoiTP1;//5
	SH_SetPoiTP2Proc	SH_SetPoiTP2;//6
	SH_SetPoiKeyDanyinProc	SH_SetPoiKeyDanyin;//7
	SH_SetPoiKeyNameProc	SH_SetPoiKeyName;//8
	SH_SetPoiKeyPhoneProc	SH_SetPoiKeyPhone;//9
	SH_SetSearchAreaProc	SH_SetSearchArea;//10
	SH_SetRoadSZSLimitProc	SH_SetRoadSZSLimit;//11
	SH_SetRoadSZZhuyinProc	SH_SetRoadSZZhuyin;//12
	SH_SetRoadSZProc		SH_SetRoadSZ;//13
	SH_SetRoadKeyDanyinProc	SH_SetRoadKeyDanyin;//14
	SH_SetRoadKeyNameProc	SH_SetRoadKeyName;//15
	SH_SetRoadNameProc		SH_SetRoadName;	//16
	SH_SetStrokeLimIdxProc	SH_SetStrokeLimIdx;//17

	SH_GetXXXTotalProc	SH_GetCityTotal;//18
	SH_GetXXXTotalProc	SH_GetTownTotal;//19
	SH_GetXXXTotalProc	SH_GetPoiTotal;//20
	SH_GetXXXTotalProc	SH_GetTP1Total;//21
	SH_GetXXXTotalProc	SH_GetRoadSZTotal;//22
    

	SH_GetXXXTotalProc	SH_GetRoadTotal;//23
	SH_GetXXXTotalProc	SH_GetRoadCrossTotal;//24
	SH_GetXXXAllProc	SH_GetMapNameAll;//...25
	SH_GetXXXAllProc	SH_GetCityNameAll;//26
	SH_GetXXXAllProc	SH_GetTownNameAll;//27
	SH_GetXXXAllProc	SH_GetPoiTP1All;//28
	SH_GetXXXAllProc	SH_GetPoiTP2All;//29
	SH_GETXXLISTProc	SH_GetPoiList;//30
	SH_GETXXLISTProc	SH_GetRoadSZList;//31
	
	SH_GETXXLISTProc	SH_GetRoadNameList;//32
	SH_GETXXLISTProc	SH_GetRoadCrossList;//33
	SH_GetPoiCityNameProc	SH_GetPoiCityName;//34
	SH_GetPoiTownNameProc	SH_GetPoiTownName;//35
	SH_GetPoiByPhoneProc	SH_GetPoiByPhone;//36
	SH_GetPoiMoreInfoProc	SH_GetPoiMoreInfo;//37
	SH_GetPoiPosProc	SH_GetPoiPos;//38
	SH_GetRoadDoorInfoProc	SH_GetRoadDoorInfo;//39
	SH_GetRoadCrossPosProc	SH_GetRoadCrossPos;//40
	SH_GetRoadCrossTownNameProc	SH_GetRoadCrossTownName;//41
	SH_GetRoadCrossTownNameProc	SH_GetRoadCrossCityName;//42
	SH_GetRoadPosProc		SH_GetRoadPos;//43
	SH_GetRoadTownNameProc	SH_GetRoadTownName;//44
	SH_GetRoadTownNameProc	SH_GetRoadCityName;//45

	SH_GetXXXTotalProc		SH_GetRoadTotal_MR;//46
	SH_GETXXLISTProc		SH_GetRoadNameList_MR;//47
	SH_SetRoadName_MRProc	SH_SetRoadName_MR;//48
	SH_GetRoadNameALL_MRProc	SH_GetRoadNameALL_MR;//49

	SH_GetFilterResAllProc	SH_GetFilterResAll;//50

	SH_SHDYStartProc	SH_SHDYStart;//51
	SH_SHDYStopProc		SH_SHDYStop;//52
	SH_SHDYSetRegionProc	SH_SHDYSetRegion;//53
	SH_SHDYSetDanyinProc	SH_SHDYSetDanyin;//54
	SH_SHDYGetHeadProc		SH_SHDYGetHead;//55
	SH_SHDYGetPageDataProc	SH_SHDYGetPageData;//56
	SH_SHDYGetPageStrokeProc	SH_SHDYGetPageStroke;//57
	SH_SHDYGetOneDetailProc		SH_SHDYGetOneDetail;//58
	SH_SHDYCityTownNameProc		SH_SHDYCityTownName;//59
	
	SH_GetPoiCarFacAllProc	SH_GetPoiCarFacAll;//60
	SH_SetPoiCarFacProc		SH_SetPoiCarFac;//61
	SH_CollectNextWordAllProc	SH_CollectNextWordAll;//62

	SH_LDCreateDataFileProc SH_LDCreateDataFile;//63
	SH_LDOpenFileProc	SH_LDOpenFile;//64
	SH_LDCloseFileProc SH_LDCloseFile;//65
	SH_LDGetFileHeaderProc SH_LDGetFileHeader;//66
	SH_LDAddRecordProc SH_LDAddRecord;//67
	SH_LDDelRecordProc SH_LDDelRecord;//68
	SH_LDClearRecordProc SH_LDClearRecord;//69
	SH_LDGetRecordProc SH_LDGetRecord;//70

	SH_GetPoiSPTypeAllProc	SH_GetPoiSPTypeAll;//71
	SH_SetPoiSPTypeProc		SH_SetPoiSPType;//72
	SH_GetChildRoadTotal_MRProc	SH_GetChildRoadTotal_MR;//73
	SH_GetXXXTotalProc		SH_GetRoadDoorTotal;//74
    SH_GetMapInfoProc       SH_GetMapInfo;//75

    SH_StartSKBSearchProc   SH_StartSKBSearch;//76
    SH_StopSKBSearchProc    SH_StopSKBSearch;//77
    SH_GetSKBAllProc        SH_GetSKBAll;//78
	//@zhikun 2009.07.16
	SH_ShBeginZProc			SH_ShBeginZ;//79
	SH_ShEndZProc			SH_ShEndZ;//80
	SH_ShSetModeZProc		SH_ShSetModeZ;//81
	SH_ShSetInputProc		SH_ShSetInput;//82
	SH_ShSetRegionZProc		SH_ShSetRegionZ;//83
	SH_ShSetStringZProc		SH_ShSetStringZ;//84
	SH_ShGetHeadZProc		SH_ShGetHeadZ;//85
	SH_ShGetPageDataZProc	SH_ShGetPageDataZ;//86
	SH_ShGetPagePinyinZProc	SH_ShGetPagePinyinZ;//87
	SH_ShGetOneDetailZProc	SH_ShGetOneDetailZ;//88
	SH_ShCityTownNameZProc	SH_ShCityTownNameZ;//89
	SH_ShGetRoadDetailZProc	SH_ShGetRoadDetailZ;//90
	SH_ShGetDrNumCoorZProc	SH_ShGetDrNumCoorZ;//91
	SH_GetPoiNearDisProc    SH_GetPoiNearDis;  // 92 // Added by Damon 20100201
	SH_GetPoiCityIDProc     SH_GetPoiCityID;  // 93 // Added by Damon 20100309
	
    SH_SetPoiPZZhuyinProc	SH_SetPoiPZZhuyin;	//94    //prosper
    SH_SetPoiPZProc			SH_SetPoiPZ;		//95    //prosper
	SH_GetXXXTotalProc		SH_GetPoiPZTotal;	//96	//prosper
	SH_GETXXLISTProc		SH_GetPoiPZList;	//97	//prosper 

    SH_Stroke_GetNextWordData SH_Stroke_GetNextWord;    //98	// Added by Prosper 201105
    SH_ZhuYinForSmart_SecondWords SH_Smart_SecondWords; //99	// Added by Prosper 201105
	SH_ZhuYinForSmart_OtherWords SH_Smart_OtherWords;   //100	// Added by Prosper 201105
	SH_Smart_GETXXLISTProc	SH_Smart_GetPoiList;        //101	// Added by Prosper 201105
	SH_Stroke_Poi_GetNextWordData SH_Stroke_Poi_GetNextWord;    //102	// Added by Prosper 201105
    SH_ZhuYinForSmart_Poi_SecondWords SH_Smart_Poi_SecondWords; //103	// Added by Prosper 201105
	SH_ZhuYinForSmart_Poi_OtherWords  SH_Smart_Poi_OtherWords;   //104	// Added by Prosper 201105
    SH_SetPoiKeyNameProc	SH_SetPoiKeyNameP;

	SH_GetRoadSKBProc SH_GetRoadSKB;			//Prosper 20111226
	SH_GetPoiSKBProc  SH_GetPoiSKB;	          //Prosper 20111226
	SH_GetRoadTownIDProc  SH_GetRoadTownID;
	
	SH_VoicePoiSetMapProc    SH_VoicePoiSetMap;
	SH_VoicePoiKeySearchProc SH_VoicePoiKeySearch;
	SH_VoicePoiPosNameProc   SH_VoicePoiPosName;
	SH_VoicePoiEndProc		 SH_VoicePoiEnd;

	SH_VoiceRoadNameProc	 SH_VoiceRoadName;

	SH_SHDYGetWordDataProc   SH_DYGetWordData;
	SH_SHDYSetDanyinKeynameProc  SH_DYSetDanyinKeyname;
	SH_SHDYGetPageDataProc   SH_DYGetPageData_WORD;
	SH_ShGetListProc		 SH_ShGetList;
	SH_ShGetNextWordsProc	 SH_ShGetNextWords;
	SH_ShSetNoNameProc		 SH_ShSetNoName;
	SH_SHDYSetSearchModeProc SH_SHDYSetSearchMode;
	SH_SHDYSetKeyProc        SH_SHDYSetKey;

	SH_GetRoadWordDataProc    SH_GetRoadWordData;
	SH_ShGetWordDataProc  SH_ShGetWordData;
	SH_SHDYSetDanyinProc  SH_SHDYSetDanyin2;     // Prosper 2013 0131
	SH_SetRoadKeyDanyinProc	SH_SetRoadKeyDanyin2;//
	SH_SetPoiKeyDanyinProc	SH_SetPoiKeyDanyin2;//
    SH_GetDYPoiTotalProc   SH_GetDYPoiTotal;
	SH_GetDYPoiTotalProc   SH_GetDYDataCOUNT;
	SH_ShGetDoorInfoProc   SH_ShGetDoorInfo;
	SH_ShGetDoorInfoDataProc SH_ShGetDoorInfoData;
	
	
	SH_SHDYStartProc	SH_SHNewRNStart;//51
	SH_SHDYStopProc		SH_SHNewRNStop;//52
	SH_SHDYSetRegionProc	SH_SHNewRNSetRegion;//53
	SH_SHDYSetDanyinProc	SH_SHNewRNSetDanyin;//54
	SH_SHDYSetDanyinProc	SH_SHNewRNSetDanyin2;//54
	SH_SHDYGetHeadProc		SH_SHNewRNGetHead;//55
	//SH_SHDYGetPageDataProc	SH_SHDYGetPageData;//56
	//SH_SHDYGetPageStrokeProc	SH_SHDYGetPageStroke;//57
	SH_SHDYGetOneDetailProc		SH_SHNewRNGetOneDetail;//58
	//SH_SHDYCityTownNameProc		SH_SHDYCityTownName;//59


	SH_SHDYGetWordDataProc   SH_SHNewRNGetWordData;
	SH_SHDYSetDanyinKeynameProc  SH_SHNewRNSetDanyinKeyname;
	SH_SHDYGetPageDataProc   SH_SHNewRNGetPageData_WORD;
	SH_SHDYSetSearchModeProc SH_SHNewRNSetSearchMode;
	SH_SHDYSetKeyProc        SH_SHNewRNSetKey; //KEEP
	SH_GetDYPoiTotalProc	 SH_GetNewRNPoiTotal;
	SH_GetDYPoiTotalProc	 SH_GetNewRNDataCOUNT;
	
	SH_GetRoadSZProc		 SH_GetRoadSZ;
	
	SH_SetRoadSZZhuyinProc	SH_SetNNRRoadSZZhuyin;
	SH_SetRoadSZSLimitProc	SH_SetNNRRoadSZSLimit;	
	SH_GETXXLISTProc		SH_GetNNRRoadSZList;

	SH_SetNNRRoadSZProc		SH_SetNNRRoadSZ;
	SH_SearchNNRRoadSZProc	SH_SearchNNRRoadSZ;
	SH_ZhuYinForSmart_OtherWords SH_NNRGET_OtherWords;
	SH_GETXXLISTProc		SH_GetNNRRoadNameList;
	SH_NEWSEARCH_SDKProc	SH_NEWSEARCH_SDK;

	nuPVOID					reserve[120-101];
}SEARCHAPI, *PSEARCHAPI;

typedef struct tagMAPDATAAPI
{
	nuPVOID			nApiReserve[100];
}MAPDATAAPI, *PMAPDATAAPI;

//-------------------------------------------------------------------		
//OpenResources Api
//@xiaoqin 2011.03.28
typedef OPR_PACK* (*RS_LoadPackProc)(const nuTCHAR* ptsPackage, nuINT nLoadType, nuUINT& nTotalLen);
typedef nuVOID (*RS_UnloadPackProc)(OPR_PACK* nPackID);
typedef OPR_PACK_FILE* (*RS_FindFileProc)(OPR_PACK* nPackID, const nuCHAR* ptsFile);
typedef nuUINT (*RS_GetFileLengthProc)(OPR_PACK* nPackID, OPR_PACK_FILE* file);
typedef nuBOOL (*RS_LoadFileDataProc)(OPR_PACK* nPackID, OPR_PACK_FILE* file, nuBYTE *pDataBuffer, nuUINT nBufLen);
typedef nuBOOL (*RS_LoadBmpProc)(OPR_PACK* nPackID, OPR_PACK_FILE* file, NURO_BMP& bitmap);
typedef nuVOID (*RS_UnLoadBmpProc)(NURO_BMP& bitmap);

typedef struct tagOPENRSAPI	
{
	RS_LoadPackProc        RS_LoadPack;
	RS_UnloadPackProc      RS_UnloadPack;
	RS_FindFileProc        RS_FindFile;
	RS_GetFileLengthProc   RS_GetFileLength;
	RS_LoadFileDataProc    RS_LoadFileData;
	RS_LoadBmpProc         RS_LoadBmp;
	RS_UnLoadBmpProc       RS_UnLoadBmp;
}OPENRSAPI, *POPENRSAPI;
//-------------------------------------------------------------------

typedef struct tagAPISTRUCTADDR
{
	PMEMMGRAPI		pMmApi;
	PMEMMGRAPI_SH		pMm_SHApi;
	PGDIAPI			pGdiApi;
	PFILESYSAPI		pFsApi;
	PMATHTOOLAPI		pMtApi;
	PROUTEAPI		pRtApi;
	PINNAVIAPI		pInApi;
	PDRAWMAPAPI		pDmApi;
	PDRAWINFOMAPAPI		pIfmApi;
	PGPSOPENAPI		pGpsApi;
	PDRAWINFONAVIAPI	pIfnApi;
	PSEARCHAPI		pShApi;
	PMAPDATAAPI		pMdApi;
	PCOMOPENAPI		pComApi;
	//@xiaoqin 2011.03.28
	POPENRSAPI       	pRsApi;
}APISTRUCTADDR, *PAPISTRUCTADDR;

//NaviCtrl API
typedef nuBOOL(*NC_OnPaintProc)();
typedef nuUINT(*NC_KeyProc)(nuUINT);
typedef nuBOOL(*NC_SetUserCfgProc)();
typedef nuBOOL(*NC_ZoomProc)(nuBOOL);
typedef nuUINT(*NC_MouseProc)(nuLONG, nuLONG);
typedef nuBOOL(*NC_SetMarketPtProc)(nuPVOID);
typedef nuBOOL(*NC_GpsSendInProc)(nuCHAR *, nuUINT);
typedef nuBOOL(*NC_ShilftNaviThreadProc)(nuBOOL);
typedef nuBOOL(*NC_SoundPlayProc)(nuBYTE, nuWORD, nuWORD, nuWORD, nuWORD, nuWCHAR *);
typedef nuBOOL(*NC_SetVoiceProc)(nuWORD, nuBOOL);
typedef nuBOOL(*NC_SetTimerProc)(nuPVOID, nuDWORD);
typedef nuPVOID(*NC_GetRoutingListProc)(nuINT, nuINT*);
typedef nuPVOID(*NC_GetGPSDataProc)();
typedef nuPVOID(*NC_GetMapInfoProc)(nuUINT);
typedef nuBOOL(*NC_MessageBoxProc)(nuUINT, nuPVOID);
typedef nuLONG*(*NC_GetScaleListProc)(nuLONG*);
typedef nuBOOL(*NC_SetScaleProc)(nuLONG);
typedef nuBOOL(*NC_GetTripInfoProc)(nuPVOID);//add by chang 2012.7.3
typedef nuBOOL(*NC_ReSetTripInfoProc)(nuPVOID);//add by chang 2012.7.3
#define NAVI_THD_WORK				0x00
#define NAVI_THD_STOP_ALLL			0x01
#define NAVI_THD_STOP_PREDRAW		0x02
#define NAVI_THD_STOP_DRAW			0x03
typedef nuBOOL(*NC_SetNaviThreadProc)(nuBYTE nNaviState);
typedef bool (*GPS_JNI_RecvDataProc)(const char*, int);
typedef bool(*NC_JNI_GPSDATAProc)(nuBOOL bFixedPos, GPSDATA *TmpGpsData);
typedef nuUINT(*NC_HUDCallBackProc)(nuLONG n1, nuLONG n2, nuLONG n3, nuLONG n4, nuLONG n5, nuLONG n6,nuLONG n7,nuLONG n8,nuLONG n9,nuBYTE *pBuffer);//HUDCallBackFunction added by daniel 20121022
typedef nuUINT(*NC_DRCallBackProc)(nuLONG nLat,nuLONG nLng,nuDOUBLE nAngle,nuLONG nLatFix,nuLONG nLngFix,nuDOUBLE nAngleFix,nuBOOL bTunnel,nuLONG nDRAngle,nuLONG nSpeed);//DRCallBackFunction added by prosper 20121022
typedef nuUINT(*NC_SOUNDCallBackProc)(nuTCHAR *tsWavPath, nuWCHAR *wsTTS);
typedef nuVOID(*NC_STOPSOUNDPLAYINGProc)();
typedef nuBOOL(*NC_SetNaviParserProc)(nuINT nMode,nuINT nType); 
typedef nuBYTE(*NC_GetNaviStateProc)();
typedef nuBYTE(*NC_SetNaviStateProc)(nuBYTE NaviState);
typedef nuBOOL(*NC_NT_ExitProc)();
typedef struct tagNAVICTRL
{
	NC_OnPaintProc		NC_OnPaint;
	NC_KeyProc				NC_KeyDown;
	NC_KeyProc				NC_KeyUp;
	NC_SetUserCfgProc		NC_SetUserCfg;
	NC_ZoomProc				NC_Zoom;
	NC_MouseProc			NC_MouseDown;
	NC_MouseProc			NC_MouseUp;
	NC_MouseProc			NC_MouseMove;
	NC_SetMarketPtProc		NC_SetMarketPt;
	NC_GpsSendInProc		NC_GpsSendIn;
	NC_ShilftNaviThreadProc	NC_ShilftNaviThread;
	NC_SoundPlayProc		NC_SoundPlay;
	NC_SoundPlayProc	    NC_Navi_SoundPlay;
	NC_Navi_CleanSoundProc  NC_Navi_CleanSound;
	NC_SetVoiceProc			NC_SetVoice;
	NC_SetTimerProc			NC_SetTimer;
	NC_GetRoutingListProc	NC_GetRoutingList;
	NC_GetGPSDataProc		NC_GetGPSData;
	NC_GetMapInfoProc		NC_GetMapInfo;
	NC_MessageBoxProc		NC_MessageBox;
	NC_SetNaviThreadProc	NC_SetNaviThread;
	NC_GetScaleListProc		NC_GetScaleList;
	NC_SetScaleProc			NC_SetScale;
	NC_ReSetTripInfoProc	NC_ReSetTripInfo;//add by chang 2012.7.3
	NC_GetTripInfoProc		NC_GetTripInfo;//add by chang 2012.7.3
	//SetNaviThread(nState);
	//the API from DrawInfo
	IFM_SetDataProc			IFM_SetData;
	IFN_SetStateArrayProc	IFN_SetStateArray;
	//the API from GPSOpen
	GPS_ReSetProc           GPS_ReSet;
    GPS_JNI_RecvDataProc    GPS_JNI_RecvData;
    NC_JNI_GPSDATAProc		NC_JNI_GPSDATA;
	NC_HUDCallBackProc      NC_HUDCallBack;
	NC_DRCallBackProc       NC_DRCallBack;
	NC_SOUNDCallBackProc    NC_SOUNDCallBack;
	NC_STOPSOUNDPLAYINGProc NC_StopSoundPlaying;
	NC_SetNaviParserProc    NC_SetNaviParser;
	NC_GetNaviStateProc		NC_GetNaviState;
	NC_SetNaviStateProc		NC_SetNaviState;
	NC_NT_ExitProc			NC_NT_Exit;
}NAVICTRL, *PNAVICTRL;
//For UI Api
typedef struct tagFILENAMESYS
{
	FS_GetFilePathProc		FS_GetFilePath;
	FS_GetRoadNameProc		FS_GetRoadName;
	FS_GetPoiNameProc		FS_GetPoiName;
	FS_ReadConstWcsProc		FS_ReadConstWcs;
	FS_ReadWcsFromFileProc	FS_ReadWcsFromFile;
	FS_ReadCityTownNameProc	FS_ReadCityTownName;
	FS_ReadPaDataProc		FS_ReadPaData;
	FS_MapIDIndexProc		FS_MapIDIndex;
	FS_RDNameProcProc		FS_RDNameProc;
	FS_SavePtFileProc		FS_SavePtFile;
	FS_SavePtFileCoverForDFTProc FS_SavePtFileCoverForDFT;
	FS_GetPtDataAddrProc	FS_GetPtDataAddr;
	FS_PtInBoundaryProc		FS_PtInBoundary;
	FS_GetMapCountProc		FS_GetMapCount;
	FS_GetMpnDataProc		FS_GetMpnData;
//	FS_SHDYStartProc			FS_SHDYStart;
//	FS_SHDYStopProc				FS_SHDYStop;
//	FS_SHDYSetRegionProc		FS_SHDYSetRegion;
	FS_SHDYSetDanyinProc		FS_SHDYSetDanyin;
	FS_SHDYGetHeadProc			FS_SHDYGetHead;
	FS_SHDYGetPageDataProc		FS_SHDYGetPageData;
	FS_SHDYGetPageStrokeProc	FS_SHDYGetPageStroke;
	FS_SHDYGetOneDetailProc		FS_SHDYGetOneDetail;
	FS_SHDYCityTownNameProc		FS_SHDYCityTownName;
	FS_Get_TMC_VD_Proc          FS_Get_TMC_VD_SIZE;          //prosper 
	FS_Get_TMC_FM_XML_Proc      FS_Get_TMC_FM_XML_SIZE;
	FS_Get_VD_Proc              FS_Get_TMC_VD;
    FS_Get_FM_XML_Proc          FS_Get_TMC_FM_XML;
	FS_Get_XML_Proc				FS_Get_TMC_FM;
	FS_Get_XML_Proc				FS_Get_TMC_XML;
	FS_Get_XML_Proc				FS_Get_TMC_XML_L;
	FS_Get_TMC_Router_Proc		FS_Get_TMC_Router;
	FS_Set_Byte_Proc			FS_Set_TMC_Byte;

}FILENAMESYS, *PFILENAMESYS;

typedef struct tagAPIFORUI
{
//	WGToChina_LbProc	WGToChina_Lb;
	NAVICTRL	naviCtrl;
	GDIAPI		gidApi;
	MEMMGRAPI	memApi;
	FILENAMESYS	filenameApi;
	SEARCHAPI	searchApi;
	ROUTEAPI    routeApi;	//added by daniel for demo 2010.08.26
	//added by xiaoqin for UI MoveMap
	MATHTOOLAPI mathtoolApi; 
	DRAWMAPAPI  drawmapApi;  
	FILESYSAPI  filesysApi;
	INNAVIAPI	innaviApi;
}APIFORUI, *PAPIFORUI;



#endif
