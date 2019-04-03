#ifndef __NURO_MAP_FORMAT_20080201
#define __NURO_MAP_FORMAT_20080201
#include "NuroDefine.h"
//Map file common structs
#define NURO_CHAR			nuWCHAR

typedef struct tagBLOCKIDINFO
{
	nuDWORD	nBlockIDFrom;
	nuDWORD nBlockIDTo;
}BLOCKIDINFO, *PBLOCKIDINFO;

//Bh file structs
typedef struct tagBHSTATICHEAD
{
	nuBYTE	nVersion[6];
	nuBYTE	nDefaultXY[8];
	nuBYTE	nBuilderTime[8];
	nuBYTE	nMapID[6];
	nuDWORD	nBlockCount;
}BHSTATICHEAD, *PBHSTATICHEAD;
typedef struct tagBHFILE
{
	BHSTATICHEAD	bhStaticHead;
	nuDWORD*		pBlockIDList;
}BHFILE, *PBHFILE;

//Rdw file structs
typedef struct tagDWROADHEAD
{
	nuDWORD	nRoadCount;
	nuDWORD	nRoadCommonCount;
	nuDWORD	nVertexCount;
}DWROADHEAD, FAR *PDWROADHEAD, FAR **PPDWROADHEAD;
typedef struct tagDWROADDATA
{
	nuWORD	nRoadCommonIdx;
	nuWORD	nVertexCount;
	nuDWORD	nVertexAddrIdx:24;
	nuDWORD	nOneWay:2;
	nuDWORD	bGPSState:1;
#ifndef ZENRIN
	nuDWORD	nLevel:5;
#else
	nuDWORD nSlope:2;
	nuDWORD	nLevel:3;
#endif
	nuDWORD	nHouseNumAddr;
}DWROADDATA, FAR *PDWROADDATA, FAR **PPDWROADDATA;
typedef struct tagDWROADCOMMON
{
	long	nRoadNameAddr;
	nuBYTE	nRoadNameLen;
	nuBYTE	nRoadType:4;
	nuBYTE	nSpeedLimit:4;
	short	nCityID:6;
	short	nTownID:10;
}DWROADCOMMON, FAR *PDWROADCOMMON, FAR **PPDWROADCOMMON;
typedef struct tagDWROADBLK
{
	PDWROADHEAD		pDwRoadHead;
	PDWROADDATA		pDwRoadData;
	PDWROADCOMMON	pDwRoadCommon;
	PNUROSPOINT		pSPointCoor;
	PNUROSRECT		pRoadRect;
}DWROADBLK, FAR *PDWROADBLK,  FAR **PPDWROADBLK;
//Pdw file structs
typedef struct tagDWPOIHEAD
{
	nuDWORD	nPOICount;
}DWPOIHEAD, FAR *PDWPOIHEAD, FAR **PPDWPOIHEAD;
typedef struct tagDWPOIDATA
{
	nuWORD	nCityID;
	nuWORD	nTownID;
	nuBYTE	nPOIType1;
	nuBYTE	nPOIType2;
	nuBYTE	nPOINameLen;
	nuBYTE	nReserve;
	long	nNameAddr;
	nuDWORD	OnterInfoAddr;
	NUROSPOINT	spCoor;
	NUROSPOINT NaviPos;
}DWPOIDATA, FAR *PDWPOIDATA, FAR **PPDWPOIDATA;
typedef struct tagDWPOIBLK
{
	PDWPOIHEAD	pDwPOIHead;
	PDWPOIDATA	pDwPOIData;
}DWPOIBLK, FAR *PDWPOIBLK, FAR **PPDWPOIBLK;
//Bdw file structs
//BgLine
typedef struct tagDWBGLINEHEAD
{
	nuDWORD	nBgLineCount;
	nuDWORD	nVertexCount;
}DWBGLINEHEAD, FAR *PDWBGLINEHEAD, FAR **PPDWBGLINEHEAD;
typedef struct tagDWBGLINEDATA
{
	NUROSRECT	sRectBound;
	long	nBgLineNameAddr;
	nuWORD	nBgLineType;
	nuWORD	nVertexCount;
	nuDWORD	nVertexIdx;
}DWBGLINEDATA, FAR *PDWBGLINEDATA, FAR **PPDWBGLINEDATA;
typedef struct tagDWBGLINEBLK
{
	PDWBGLINEHEAD	pDwBgLineHead;
	PDWBGLINEDATA	pDwBgLineData;
	PNUROSPOINT		pSPointCoor;
}DWBGLINEBLK, FAR *PDWBGLINEBLK, FAR **PPDWBGLINEBLK;
//BgArea
typedef struct tagDWBGAREAHEAD
{
	nuDWORD	nBgAreaCount;
	nuDWORD	nVertexCount;
}DWBGAREAHEAD, FAR *PDWBGAREAHEAD, FAR **PPDWBGAREAHEAD;
typedef struct tagDWBGAREADATA
{
	NUROSRECT	sRectBount;
	nuDWORD	sBgAreaNameAddr;
	nuWORD	nBgAreaType;
	nuWORD	nVertexCount;
	nuDWORD	nVertexIdx;
}DWBGAREADATA, FAR *PDWBGAREADATA, FAR **PPDWBGAREADATA;
typedef struct tagDWBGAREABLK
{
	PDWBGAREAHEAD	pDwBgAreaHead;
	PDWBGAREADATA	pDwBgAreaData;
	PNUROSPOINT		pSPointCoor;
}DWBGAREABLK, *PDWBGAREABLK, **PPDWBGAREABLK;

//Blocks dw structs for DrawMap DLL
#define MAX_BLOCKS_OF_EACH_DW				120
typedef struct tagBLOCKDATA
{
	nuBYTE	bCover:1;//0:Not cover; 1:Cover
	nuBYTE	bBlockExtended:1;//skip read poi data in extended blocks;
	nuBYTE	bBlockZoom:1;
	nuBYTE	bBlockZoomExtend:1;
	nuWORD	nRdwMassIdx;
	nuWORD	nPdwMassIdx;
	nuWORD	nBdwLineMassIdx;
	nuWORD	nBdwAreaMassIdx;
	nuDWORD	nBlockIdx;
	nuDWORD	nBlockID;
	tagBLOCKDATA *pNext;
}BLOCKDATA, *PBLOCKDATA, **PPBLOCKDATA;
typedef struct tagDWDATA
{
	nuBYTE	bCover:1;
	nuWORD	nNumOfBlocksCovered;
	PBLOCKDATA	pBlockList;
}DWDATA, *PDWDATA;
//
typedef struct tagBLOCKNODE
{
	nuDWORD			nBlockIdx;
	nuDWORD			nBlockID;
	DWROADBLK		roadBlk;//Road data
	DWPOIBLK		poiBlk;//POI data
	DWBGLINEBLK		bgLineBlk;//BgLine Data
	DWBGAREABLK		bgAreaBlk;//BgArea Data
}BLOCKNODE, FAR *PBLOCKNODE, FAR **PPBLOCKNODE;
typedef struct tagMAPNODE
{
	nuWORD			nDwIdx;
	nuWORD			nNumOfBlocks;
	PBLOCKNODE		pBlockList;
}MAPNODE, FAR *PMAPNODE, FAR **PPMAPNODE;
typedef struct tagSCREENMAP
{
	nuWORD			nDwCount;
	PMAPNODE		pDwList;
}SCREENMAP, FAR *PSCREENMAP, FAR **PPSCREENMAP;
typedef const SCREENMAP*	PCSCREENMAP;

#define MAX_DW_BLOCKS_LOADEN				100
#define MAX_DW_BLOCKS_SMALL					16

#define DW_DATA_FROM_NO						0x00
#define DW_DATA_FROM_READ					0x01
#define DW_DATA_FROM_COPY					0x02
typedef struct tagDWBLOCKNODE
{
	nuWORD	nCover:2;
	nuWORD  nDataFrom:6;
	nuWORD	nReserve2:8;
	nuWORD	nDwIdx;
	nuDWORD	nBlkID;
	nuDWORD nBlkIdx;
	nuWORD	nPdwMemIdx;
	nuWORD	nRdwMemIdx;
	nuWORD	nBLdwMemIdx;
	nuWORD	nBAdwMemIdx;
	nuDWORD nPdwBlkAddr;
	nuDWORD nRdwBlkAddr;
	nuDWORD nBLdwBlkAddr;
	nuDWORD nBAdwBlkAddr;
}DWBLOCKNODE, *PDWBLOCKNODE;
typedef struct tagDWBLKDATA
{
	nuBYTE			nBlkCount;
	DWBLOCKNODE		pDwList[MAX_DW_BLOCKS_LOADEN];
}DWBLKDATA, *PDWBLKDATA;
typedef struct tagDWBLKDATASMALL
{
	nuBYTE			nBlkCount;
	DWBLOCKNODE		pDwList[MAX_DW_BLOCKS_SMALL];
}DWBLKDATASMALL, *PDWBLKDATASMALL;
#define DW_DATA_MODE_PDW				0x01
#define DW_DATA_MODE_RDW				0x02
#define DW_DATA_MODE_BLDW				0x04
#define DW_DATA_MODE_BADW				0x08

#define DW_DATA_MODE_SMALL				0x0100
#define DW_DATA_MODE_BIG				0x0200


//CCD file structs


#define MAX_CCD_BLOCK				8
#define MAX_EEYES_BLOCK_COUNT		8

#define EMG_ET_TYPE_CM				0x07

typedef struct tagE_EYE_HEAD
{
	nuDWORD nEEyesCount;
}E_EYE_HEAD, *PE_EYE_HEAD;
typedef struct tagE_EYE_NODE
{
	nuLONG	nNameAddr;
	nuLONG	x;
	nuLONG	y;
	nuBYTE	angle;
	nuBYTE	nCheckDir:4;
	nuBYTE	nSpeed:4;
	nuBYTE	nClass;
	nuBYTE	nEType;
}E_EYE_NODE, *PE_EYE_NODE, **PPE_EYE_NODE;

typedef struct tagE_EYE_BLOCK
{
	PE_EYE_HEAD	pEEyeHead;
	PE_EYE_NODE	pEEyeNode;
}E_EYE_BLOCK, *PE_EYE_BLOCK;

typedef struct tagEEYES_BLK
{
	nuWORD		nMemIdx;
	nuWORD		nDwIdx;
	nuDWORD		nBlkID;
	nuDWORD		nBlkIdx;
}EEYES_BLK, *PEEYES_BLK;

typedef struct tagEEYES_DATA
{
	nuWORD		nEEyesBlkCount;
	nuWORD		nKeptBit;
	EEYES_BLK	pEEyesBlkList[MAX_EEYES_BLOCK_COUNT];
}EEYES_DATA, *PEEYES_DATA;

#define MAX_NUM_OF_EEYES_PLAYED						10
typedef struct tagEEYES_PLAYED
{
	nuBYTE		bUsed;
	nuroPOINT	point;
}EEYES_PLAYED, *PEEYES_PLAYED;

typedef struct tagCCDNODE
{
	nuLONG	nNameAddr;
	nuLONG	x;
	nuLONG	y;
	nuSHORT	angle;
	nuSHORT	oneway;
}CCDNODE, *PCCDNODE, **PPCCDNODE;

typedef struct tagCCDBLOCK
{
	nuWORD		bUsed:1;
	nuWORD		nDwIdx;
	nuDWORD		nBlockID;
	nuDWORD		nBlockIdx;
	nuDWORD		nCCDCount;
	PPCCDNODE	ppCCDNodeList;
}CCDBLOCK, *PCCDBLOCK, *PPCCDBLOCK;

typedef struct tagCCDDATA
{
	nuWORD		nCCDBlockCount;
	CCDBLOCK	pCCDBlockList[MAX_CCD_BLOCK];
}CCDDATA, *PCCDDATA;

//CB file Structs
typedef struct tagCB_HEADER
{
	nuDWORD	nCityCount;
	nuDWORD nTownCount;
}CB_HEADER, *PCB_HEADER;

#define TOWN_NAME_NUM				50
#define CITY_NAME_NUM				50
#define EACH_CTNAME_LEN				100
#define EACH_TNNAME_LEN				100
typedef struct tagCB_TOWNINFO
{
	nuWORD		nCityCode;
	nuWORD		nTownCode;
	NURO_CHAR	wsTownName[TOWN_NAME_NUM];
	nuDWORD		nTownBlkInfoCount;
	PBLOCKIDINFO	pBlockIDInfo;
}CB_TOWNINFO, *PCB_TOWNINFO;
//PA file Structs

#define PA_PHONENUM_MAX_NUM				50
#define PA_NAME_MAX_NUM					100
typedef struct tagPOIPANODE
{
	nuBYTE	nPhoneLen;
	nuBYTE	nSubBranchLen;
	nuBYTE	nFaxLen;
	nuBYTE	nAddressLen;
	nuBYTE	nWebLen;
	nuBYTE	nOpenTimeLen;
	nuWCHAR	wsPhone[PA_PHONENUM_MAX_NUM];
	nuWCHAR	wsFax[PA_PHONENUM_MAX_NUM];
	nuWCHAR	wsSubBranch[PA_NAME_MAX_NUM];
	nuWCHAR	wsAddress[PA_NAME_MAX_NUM];
	nuWCHAR	wsWeb[PA_NAME_MAX_NUM];
	nuWCHAR	wsOpenTime[PA_NAME_MAX_NUM];
}POIPANODE, *PPOIPANODE;

//MPN file structs
#define MPN_FILENAME_LEN			40
#define MPN_FRIENDNAME_LEN			72
typedef struct tagMPNDATA
{
	nuDWORD		nMapID;
	nuTCHAR		tsFoldName[MPN_FILENAME_LEN/sizeof(nuTCHAR)];
	nuWCHAR		wsFriendName[MPN_FRIENDNAME_LEN/sizeof(nuWCHAR)];
	nuroPOINT	point;
}MPNDATA, *PMPNDATA;

//MDW file structs
#define MAX_MDW_BLKS_LOADEN				64
typedef struct tagMDWROAD
{
	nuWORD	nVertexCount;
	nuWORD  nVertexAddLow;
	nuDWORD	nVertexAddHigh:8;
	nuDWORD	nClass:3;
	nuDWORD nMapID:9;
	nuDWORD	nRTBID:12;
}MDWROAD, *PMDWROAD;
typedef struct tagMDWHEAD
{
	nuDWORD	nRoadCount;
	nuDWORD	nVertexCount;
}MDWHEAD, *PMDWHEAD;
typedef struct tagMDWPOINT
{
	nuLONG	x;
	nuLONG	y;
}MDWPOINT, *PMDWPOINT;
typedef struct tagMDWBLKCOL
{
	nuDWORD		nTmpIdx;
	nuroRECT	rtBlock;
	PMDWHEAD	pMdwHead;
	PMDWROAD	pMdwRoad;
	PMDWPOINT	ptVertext;
}MDWBLKCOL, *PMDWBLKCOL;

typedef struct tagMDWNEEDNODE
{
	nuWORD	nCover:2;//0:deleted; 1:Loaden
	nuWORD	nMemIdx;
	nuDWORD nBlkID;
	nuDWORD	nBlkAddr;
}MDWNEEDNODE, *PMDWNEEDNODE;
typedef struct tagMDWDATA
{
	nuBYTE			nBlkCount;
	nuBYTE			nClassCnt;
	MDWNEEDNODE		pMdwNode[MAX_MDW_BLKS_LOADEN];
}MDWDATA, *PMDWDATA;
//
//BGL file structs
#define MAX_BGL_BLKS_LOADEN					64
typedef struct tagBGLPOINT
{
	nuSHORT x;
	nuSHORT y;
}BGLPOINT, *PBGLPOINT;
typedef struct tagBGLFILEHEAD
{
	nuDWORD nDivisor;
	nuDWORD nTotalCount;
}BGLFILEHEAD, *PBGLFILEHEAD;
typedef struct tagBGLBLKHEAD
{
	nuDWORD	nBglCount;
	nuDWORD nVertexCount;
}BGLBLKHEAD, *PBGLBLKHEAD;
typedef struct tagBGLBLK
{
	nuWORD	nVertextCount;
	nuWORD	nVertexAddressLo;
	nuDWORD	nVertexAddRessHi:8;
	nuDWORD nClass:4;
	nuDWORD nRouteType:4;
	nuDWORD nRouteIndex:16;
}BGLBLK, *PBGLBLK;

typedef struct tagBGLDATANODE
{
	nuWORD	nCover:2;
	nuWORD	nReseve:14;
	nuWORD	nMemIdx;
	nuDWORD	nBlkID;
	nuDWORD nBlkAddress;
}BGLDATANODE, *PBGLDATANODE;
typedef struct tagBGLDATA
{
	nuBYTE	nBlkCount;
	nuBYTE	nClsCnt;
	nuWORD	bDataChanged:1;
	nuDWORD		nDivisor;
	nuLONG	nBlkSize;
	BGLDATANODE	pBglNode[MAX_BGL_BLKS_LOADEN];
}BGLDATA, *PBGLDATA;

typedef struct tagBGLBLKCOL
{
	nuDWORD		nTmpClsIdx;
	NURORECT	rtBlock;
	PBGLBLKHEAD	pHead;
	PBGLBLK		pData;
	PBGLPOINT	ptVertext;
}BGLBLKCOL, *PBGLBLKCOL;


//Sea File Structs
typedef struct tagSEAFILEHEAD
{
	nuDWORD nScaleCnt;
}SEAFILEHEAD, *PSEAFILEHEAD;
typedef struct tagSEASCALEDATA
{
	nuDWORD nScaleValue;
	nuDWORD	nBlockSize;
	nuDWORD nDivisor;
	nuDWORD nBlockCount;
	nuDWORD nStartAddress;
	nuLONG	nXStart;
	nuLONG	nYStart;
	nuLONG	nXEnd;
	nuLONG	nYEnd;
}SEASCALEDATA, *PSEASCALEDATA;

typedef struct tagSEABLKHEAD
{
	nuDWORD	nSeaAreaCount;
	nuDWORD nSeaVertexCount;
}SEABLKHEAD, *PSEABLKHEAD;

typedef struct tagSEABLKLISTNODE
{
	nuLONG	nBlockAddr;
	nuLONG	nBlockSize;
}SEABLKLISTNODE, *PSEABLKLISTNODE;
typedef struct tagSEABLKDATA
{
	nuDWORD nAreaType:8;
	nuDWORD nVertexCount:24;
	nuDWORD nVertexIdex;
}SEABLKDATA, *PSEABLKDATA;

typedef struct tagSEAPOINT
{
	nuSHORT x;
	nuSHORT y;
}SEAPOINT, *PSEAPOINT;

#define MAX_SEA_BLKS_LOADEN				64
typedef struct tagSEANODE
{
	nuWORD	nCover:2;
	nuWORD	nMemIdx;
	nuDWORD	nBlkID;
	nuDWORD nBlkAddress;
}SEANODE, *PSEANODE;
typedef struct tagSEADATA
{
	nuBYTE			nBlkCount;
	nuBYTE			nClsCount;
	SEASCALEDATA	seaScaleData;
	SEANODE			pSeaList[MAX_SEA_BLKS_LOADEN];
}SEADATA, *PSEADATA;

typedef struct tagSEABLKCOL
{
	nuDWORD		nTmpClsIdx;
	NURORECT	rtBlock;
	PSEABLKHEAD	pSeaHead;
	PSEABLKDATA	pSeaData;
	PSEAPOINT	ptVertex;
}SEABLKCOL, *PSEABLKCOL;

//Danyin file struct
#define DY_A_NAME_LEN				40
#define DY_B_NAME_LEN				40


//NewDanyin file struct
#define NewDY_A_NAME_LEN				128
#define NewDY_B_NAME_LEN				128

#define DY_CITY_NAME_LEN			8
#define DY_TOWN_NAME_LEN			8
typedef struct tagDANYINHEAD
{
	nuDWORD	nTotalCount;
	nuDWORD	nCityCount;
	nuDWORD nTownCount;
	nuDWORD nFNAddr;
	nuDWORD nBiHuaAddr;
	nuDWORD nOthersAddrAddr;
	nuDWORD nCoorAddr;
	nuDWORD nCityNameAddr;
	nuDWORD nTownNameAddr;
}DANYINHEAD, *PDANYINHEAD;
typedef struct tagDANYIN_A
{
	nuBYTE	pName[DY_A_NAME_LEN];
}DANYIN_A, *PDANYIN_A;
typedef struct tagDYNODE_B_1
{
	nuBYTE	nName[DY_B_NAME_LEN];
}DYNODE_B_1, *PDYNODE_B_1;
typedef struct tagDYNODE_B_2
{
	nuBYTE	nStroke;
	nuBYTE  nType;
	nuWORD	nCityID:6;
	nuWORD	nTownID:10;
}DYNODE_B_2, *PDYNODE_B_2;
typedef struct tagDYNODE_B_3
{
	nuDWORD nOtherAddr;
}DYNODE_B_3, *PDYNODE_B_3;
typedef struct tagDYNODE_C
{
	nuroPOINT point;
}DYNODE_C, &PDYNODE_C;

typedef struct tagNEWDANYINHEAD
{
	nuDWORD	nTotalCount;
	nuDWORD nMaxNameLen;
	nuDWORD	nCityCount;
	nuDWORD nTownCount;
	nuDWORD nNameLenStartAddr;
	nuDWORD nDanYinNameStartAddr;
	nuDWORD nNameStartAddr;
	nuDWORD nFeatCodeStart;
	nuDWORD nInfoAddrStart;
	nuDWORD nCoordiateStart;
	nuDWORD nCityStart;
	nuDWORD nTownStart;
	nuDWORD nInfoDataStart;
	nuBYTE Reserver[148];
}NEWDANYINHEAD, *PNEWDANYINHEAD;

//PDY file struct
typedef struct tagPDY_HEADER
{
	nuDWORD	nTotalCount;
	nuDWORD	nCityCount;
	nuDWORD	nTownCount;
	nuDWORD	nLengthAddr;
	nuDWORD	nDanYinAddr;
	nuDWORD	nNameAddr;
	nuDWORD	nDataAddr;
	nuDWORD	nPIInfoAddr;
	nuDWORD	nCityAddr;
	nuDWORD	nTownAddr;
}PDY_HEADER, *PPDY_HEADER;

typedef struct tagPDY_D_NODE
{
	nuBYTE  nPinYin;
	nuBYTE	nType;
	nuWORD	nCityID:6;
	nuWORD	nTownID:10;
}PDY_D_NODE, *PPDY_D_NODE;

#define PDY_CITYTOWN_NAME_LEN				20

//PI file struct

typedef struct tagPI_A_NODE
{
	nuDWORD nNameAddress;
	nuWORD nCityID;
	nuWORD nTownID;
	nuWORD nType1ID;
	nuWORD nType2ID;
	nuDWORD nOtherInfoAddr;
	nuDWORD nX;
	nuDWORD nY;
}PI_A_NODE, *PPI_A_NODE;

//define For Special Door Number
typedef struct tagRN2_HEADER
{
	nuDWORD nRoadCount;
	nuDWORD nZhuyinAddr;
	nuDWORD nRoadNameAddr;
	nuDWORD nCityTownAddr;
}RN2_HEADER,*PRN2_HEADER;

typedef struct tagRN2_D_NODE
{
	nuBYTE nCityID;
	nuBYTE nTownID;
}RN2_D_NODE,*PRN2_D_NODE;

typedef struct tagRN2_F_NODE
{
	nuWORD nMinNum;
	nuWORD nDrNumCount;
	nuDWORD nMinNumX;
	nuDWORD nMinNumY;
	nuDWORD pOtherDrNumAddr;
}RN2_F_NODE,*PRN2_F_NODE;

typedef struct tagRN2_H_NODE
{
	nuWORD nDrNumDis;
	nuSHORT nDrNumXDis;
	nuSHORT nDrNumYDis;
}RN2_H_NODE,*PRN2_H_NODE;

//New CCD file
#define _MAX_CCD_CHECK_DIS						500
#define _MAX_CCD_CHECK_ANGLE					30
#define _MAX_CCD_BLOCKS_FOR_DRAWING				6
#define _MAX_CCD_BLOCKS_FOR_CHECKING			4

#define _CCD2_BLOCK_SIZE						20000
#define _CCD2_BLOCK_X_COUNT						1800

typedef struct tagCCD2_FILE_HEAD
{
	nuDWORD			nBlockCount;
	nuDWORD			nXStart;
	nuDWORD			nYStart;
	nuDWORD			nXEnd;
	nuDWORD			nYEnd;
}CCD2_FILE_HEAD, *PCCD2_FILE_HEAD;

typedef struct tagCCD2_BODY_A1
{
	nuDWORD			nNameAddr;
	nuSHORT			nX;
	nuSHORT			nY;
	nuDWORD			nBlkIdx;
	nuWORD			nRdIdx;
	nuSHORT			nAngle;
	nuBYTE			nCheckDir:4;
	nuBYTE			nSpeed:4;
	nuBYTE			nType;
	nuWORD			nReserve;
}CCD2_BODY_A1,*PCCD2_BODY_A1;

typedef struct tagCCD2_DATA
{
	nuBYTE			bUsedNow;
	nuBYTE			nReadType;
	nuDWORD			nBlockID;
	nuWORD			nMapIdx;
	nuWORD			nMemIdx;
}CCD2_DATA, *PCCD2_DATA;

#define _CCD2_MAX_CHECK_COUNT				10
typedef struct tagCCD2_PLAYED
{
	nuBYTE			bUsed;
	nuroPOINT		point;
}CCD2_PLAYED, *PCCD2_PLAYED;

typedef struct tag_POI_Info_Data		               //Shyanx 2012.02.13
{ 
	nuWORD  MapIndex;
	nuWORD  Reserve;
	nuDWORD BlockIndex;
	nuDWORD RoadIndex;
	nuDWORD RoadNameAddr;
	nuWORD  CityID;
	nuWORD  TownID; 
	long    x;
	long    y;

} POI_Info_Data,*PPOI_Info_Data;

typedef struct tagSLPHEARDER
{
	nuDWORD	nSLP_Count;
	nuBYTE  nReserve[196];
}SLPHEARDER, *PSLPHEARDER;

typedef struct tagSLPDATA
{
	nuWORD  nSlope_BkIdx;
	nuWORD  nSlope_RdIdx;
	nuWORD  nLink_BkIdx;
	nuWORD  nLink_RdIdx;
	nuDWORD nSlope_Value:2;
	nuDWORD nLink_Level:3;
	nuDWORD nReserve:27;
}SLPDATA, *PSLPDATA;
typedef struct tagNEWRNHEAD
{
	nuDWORD	nTotalCount;
	nuDWORD nMaxNameLen;
	nuDWORD	nCityCount;
	nuDWORD nTownCount;
	nuDWORD nNameLenStartAddr;
	nuDWORD nDanYinNameStartAddr;
	nuDWORD nNameStartAddr;
	nuDWORD nFeatCodeStart;
	nuDWORD nInfoAddrStart;
	nuDWORD nCoordiateStart;
	nuDWORD nCityStart;
	nuDWORD nTownStart;
	nuDWORD nInfoDataStart;
	nuDWORD nNRN_SZYStart;
	nuBYTE	Reserver[144];
}NEWRNHEAD, *PNEWRNHEAD;

#endif

