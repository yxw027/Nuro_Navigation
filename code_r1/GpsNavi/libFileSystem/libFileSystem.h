#ifndef __NURO_FILE_SYSTEM_20080128
#define __NURO_FILE_SYSTEM_20080128

#include "NuroDefine.h"
#include "NuroEngineStructs.h"
#include "NuroMapFormat.h"
#include "NuroModuleApiStruct.h"

#ifdef _USRDLL
#ifdef FILESYSTEM_EXPORTS
#define FILESYSTEM_API extern "C" __declspec(dllexport)
#else
#define FILESYSTEM_API extern "C" __declspec(dllimport)
#endif
#else
#define FILESYSTEM_API extern "C" 
#endif


FILESYSTEM_API nuBOOL  LibFS_InitFileSystem(nuPVOID lpInMM, nuPVOID lpInGDI, nuPVOID lpOutFs);
FILESYSTEM_API nuVOID  LibFS_FreeFileSystem();

FILESYSTEM_API nuBOOL  LibFS_SaveUserConfig();
FILESYSTEM_API nuBOOL  LibFS_CoverFromDefault();
FILESYSTEM_API nuBOOL  LibFS_ReLoadMapConfig();
FILESYSTEM_API nuBOOL  LibFS_LoadResidentData(nuDWORD nDataType);

FILESYSTEM_API nuPVOID LibFS_GetData(nuBYTE byDataType);

FILESYSTEM_API nuBOOL  LibFS_GetFilePath(nuWORD nFileType, nuTCHAR *sWholePath, nuWORD len);
FILESYSTEM_API nuBOOL  LibFS_FindFileWholePath(nuLONG nMapIdx, nuTCHAR *sWholePath, nuWORD len);
FILESYSTEM_API nuWORD  LibFS_GetMapCount();
FILESYSTEM_API nuBOOL  LibFS_PtInBoundary(nuroPOINT pt);
FILESYSTEM_API nuBOOL  LibFS_LoadBlock(nuPVOID lpVoid);
FILESYSTEM_API nuPVOID LibFS_GetScreenMap();
FILESYSTEM_API nuBOOL  LibFS_GetRoadName( nuWORD nDwIdx, nuLONG nNameAddr, nuINT nRoadClass, 
										  nuPVOID pNameBuf, nuWORD nBufLen, nuPVOID pRnEx );
FILESYSTEM_API nuBOOL  LibFS_GetRoadIconInfo( nuWORD nDwIdx, nuLONG nScaleValue, nuLONG nBGLStartScale, 
											  nuPVOID pRnEx, nuWORD *pRoadIconInfo ); 
FILESYSTEM_API nuBOOL  LibFS_GetPoiName(nuWORD nDwIdx, nuLONG nNameAddr, nuPVOID pNameBuf, nuPWORD pBufLen);

FILESYSTEM_API nuBOOL  LibFS_SavePtFile(nuINT DwIdx);
FILESYSTEM_API nuBOOL  LibFS_SavePtFileCoverForDFT(nuINT DwIdx);
FILESYSTEM_API const nuPVOID LibFS_GetPtDataAddr(nuLONG nDWIdx, nuLONG* pCount);
FILESYSTEM_API const nuPVOID LibFS_GetPtNodeByType(nuLONG nDWIdx, nuBYTE, nuBYTE);
FILESYSTEM_API nuBOOL  LibFS_GetInfoBySourceType(nuLONG nSTypeID1, nuLONG nSTypeID2, nuLONG* pTypeID1, nuLONG* pTypeID2, nuLONG* pDisPlay, nuLONG* pIcon);
FILESYSTEM_API nuBOOL  LibFS_GetInfoByNuroType(nuLONG nTypeID1, nuLONG nTypeID2, nuLONG* pSTypeID1, nuLONG* pSTypeID2, nuLONG* pDisPlay, nuLONG* pIcon);

FILESYSTEM_API nuINT   LibFS_ReadConstWcs(nuBYTE nWcsType, nuLONG nIndex, nuWCHAR* pBuf, nuWORD nLen);
FILESYSTEM_API nuINT   LibFS_ReadWcsFromFile(nuTCHAR *tsFile, nuDWORD nLineIdx, nuWCHAR* pBuf, nuWORD nLen);

FILESYSTEM_API nuBOOL  LibFS_ReadInfoMapCenterName();

FILESYSTEM_API nuPVOID LibFS_GetCCDData(const nuroRECT& rtRange);
FILESYSTEM_API nuUINT  LibFS_GetCCDName(nuLONG nMapIdx, nuDWORD nNameAddr, nuPVOID pBuff, nuUINT nBuffLen);

FILESYSTEM_API nuBOOL  LibFS_SeekForRoad(nuLONG x, nuLONG y, nuDWORD nCoverDis, nuLONG nAngle, PSEEKEDROADDATA pRoadData);
FILESYSTEM_API nuBOOL  LibFS_PaddingRoad(PSEEKEDROADDATA pRoadData);
FILESYSTEM_API nuBOOL  LibFS_SeekForPoi(nuLONG x, nuLONG y, nuDWORD nCoverDis, nuLONG nAngle, PSEEKEDPOIDATA pPoiData);
FILESYSTEM_API nuBOOL  LibFS_SeekForRoads(PSEEK_ROADS_IN pSeekRdsIn, PSEEK_ROADS_OUT pSeekRdsOut, nuBOOL bSetMarket);

FILESYSTEM_API nuBOOL  LibFS_SaveLastPos(nuPVOID lpVoid);

FILESYSTEM_API nuBOOL  LibFS_ReadCityTownName(nuLONG nMapIdx, nuDWORD nCityTownCode, nuWCHAR *wsName, nuWORD nWsNum);
FILESYSTEM_API nuBOOL  LibFS_ReadPaData(nuLONG nMapIdx, nuDWORD nAddress, PPOIPANODE pBuff);

FILESYSTEM_API nuLONG  LibFS_MapIDIndex(nuLONG nId, nuBYTE nType);

FILESYSTEM_API nuPVOID LibFS_LoadDwBlock(const nuroRECT& rtMap, nuDWORD nDataMode);
FILESYSTEM_API nuVOID  LibFS_FreeDwBlock(nuDWORD nDataMode);

FILESYSTEM_API nuVOID  LibFS_RDNameProc(nuWCHAR *pWsName, nuWORD nLen);

FILESYSTEM_API nuBOOL  LibFS_CheckEEyes(nuroPOINT point, nuLONG nDis, short nAngle, PROADFOREEYES pRdForEEyes);
FILESYSTEM_API nuBOOL  LibFS_GetRoadSegData(nuWORD nDwIdx, PROAD_SEG_DATA pRoadSegData);
FILESYSTEM_API nuPVOID LibFS_GetMpnData();

FILESYSTEM_API nuUINT  LibFS_SetWaypoints(nuWORD idx, nuLONG x, nuLONG y, nuWCHAR* pwsName);
FILESYSTEM_API nuUINT  LibFS_DelWaypoints(nuWORD idx);
FILESYSTEM_API nuUINT  LibFS_GetWaypoints(PPARAM_GETWAYPOINTS pParam);
FILESYSTEM_API nuUINT  LibFS_KptWaypoints(nuLONG x, nuLONG y);

FILESYSTEM_API nuUINT  LibFS_SeekForCcdInfo(nuroRECT rtMap, PCCDINFO pCcdInfo);
FILESYSTEM_API nuUINT  LibFS_CheckNewCcd(PCCDINFO pCcdInfo);

FILESYSTEM_API nuUINT  LibFS_FS_Get_TMC_VD_SIZE(nuCHAR *sTime);                              //prosper
FILESYSTEM_API nuBOOL  LibFS_FS_Get_TMC_VD(nuVOID*pTmcBuf,const nuUINT size);
FILESYSTEM_API nuBOOL  LibFS_Get_TMC_FM_XML(nuVOID*pTmcBuf,const nuUINT size);

FILESYSTEM_API nuBOOL  LibFS_Get_TMC_FM_XML_SIZE(nuUINT& nTotal_Count,nuUINT* nFm_Count,nuUINT& nXML_Count,nuUINT* nXML_Complex_Count); //prosper 08.
FILESYSTEM_API nuBOOL  LibFS_Get_TMC_FM(nuVOID*pTmcBuf,const nuUINT size,nuUINT nmode);
FILESYSTEM_API nuBOOL  LibFS_Get_TMC_XML(nuVOID*pTmcBuf,const nuUINT size,nuUINT nmode);    
FILESYSTEM_API nuBOOL  LibFS_Get_TMC_XML_L(nuVOID*pTmcBuf,const nuUINT size,nuUINT nmode); 
FILESYSTEM_API nuBOOL  LibFS_Set_TMC_Byte(nuVOID*pTmcBuf);
FILESYSTEM_API nuBOOL  LibFS_Get_TMC_Router(nuVOID*pTmcBuf,const nuUINT size);
FILESYSTEM_API nuBOOL  LibFS_ChangePoiInfo(/*InPut*/NUROPOINT PoiCoor ,/*output*/POI_Info_Data& PoiData);//Shyanx 2012.02

//---------------added by daniel 20121205---------------//
FILESYSTEM_API KMInfoData* LibFS_GetKMDataDL(nuLONG BlockIdx, /*output*/nuLONG &Counter);
FILESYSTEM_API KMInfoData* LibFS_GetKMDataMapping(const SEEKEDROADDATA &RoadData, nuLONG &Counter);
FILESYSTEM_API nuUINT LibFS_SeekForKMInfo(nuroRECT rtMap, PKMINFO pKMInfo);
FILESYSTEM_API nuBOOL LibFS_CheckKMInfo(PKMINFO pKMInfo, nuWCHAR *wsKilometer, nuLONG &lKMNum, nuLONG &lKMDis, nuWCHAR *wsKMName);
//---------------added by daniel 20121205---------------//
FILESYSTEM_API nuUINT LibFS_GetTMCTrafficInfo(nuBYTE nDWIdx, nuUINT BlockID, nuUINT RoadID);
FILESYSTEM_API nuUINT LibFS_GetTMCTrafficInfoByNodeID(nuBYTE nDWIdx, nuUINT NodeID1, nuUINT NodeID2);
FILESYSTEM_API nuUINT LibFS_GetTMCDirInfo(nuBYTE nDWIdx, nuUINT BlockID, nuUINT RoadID);
FILESYSTEM_API nuUINT LibFS_GetTMCDirInfoByNodeID(nuBYTE nDWIdx, nuUINT NodeID1, nuUINT NodeID2);
FILESYSTEM_API nuBOOL LibFS_COPY_TMC_Event_Data(nuBYTE nDWIdx);
FILESYSTEM_API TMCRoadInfoStru* LibFS_GetTMCDataInfo(nuBYTE nDWIdx, nuUINT BlockID, nuUINT RoadID);
FILESYSTEM_API nuBOOL LibFS_Initial_TMC_Event_Data(nuBYTE nDWIdx);
FILESYSTEM_API nuVOID LibFS_Release_TMC_Event_Data(nuBYTE nDWIdx);
FILESYSTEM_API nuBOOL LibFS_Initial_TMC_Data(nuINT nCount);
FILESYSTEM_API nuVOID LibFS_Release_TMC_Data();
//FILESYSTEM_API nuUINT LibFS_LookForNaviRoad(PLOOKFOR_NAVI_ROAD_IN pNaviIn, PSEEKEDROADDATA pRoadData);

//@zhikun 2009.08.26 
//FILESYSTEM_API nuUINT	LibFS_FixedToRoad(PSEEK_ROADS_IN pSeekRdsIn, );

//For Search API
/*
FILESYSTEM_API nuUINT LibFS_SHDYStart();
FILESYSTEM_API nuUINT LibFS_SHDYStop();
FILESYSTEM_API nuUINT LibFS_SHDYSetRegion(nuBYTE nSetMode, nuDWORD nData);
FILESYSTEM_API nuUINT LibFS_SHDYSetDanyin(nuWCHAR *pWsDy, nuBYTE nWsNum);
FILESYSTEM_API nuUINT LibFS_SHDYGetHead(PDYCOMMONFORUI pDyHead);
FILESYSTEM_API nuUINT LibFS_SHDYGetPageData(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx);
FILESYSTEM_API nuUINT LibFS_SHDYGetPageStroke(PDYDATAFORUI pDyDataForUI, nuBYTE nStroke);
FILESYSTEM_API nuUINT LibFS_SHDYGetOneDetail(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
FILESYSTEM_API nuUINT LibFS_SHDYCityTownName(PDYCITYTOWNNAME pDyCTname, nuLONG nCityID);
*/

#endif
