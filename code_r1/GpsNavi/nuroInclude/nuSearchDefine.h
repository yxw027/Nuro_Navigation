#ifndef _NURO_SEARCH_DEFINE_20081003
#define _NURO_SEARCH_DEFINE_20081003

#include "NuroDefine.h"

#define NEXT_WORD_MAXCOUNT	128
	
#define _SEARCH_LKBPMS_ 3

#define EACH_MAP_NAME_LEN				72
#define EACH_CITY_NAME_LEN				100
#define EACH_TOWN_NAME_LEN				100
#define EACH_TYPE_NAME_LEN				136
#define EACH_BUFFER_NAME_LEN			64
#define EACH_POI_NAME_LEN				(EACH_BUFFER_NAME_LEN<<1)
#define EACH_ROAD_NAME_LEN				EACH_POI_NAME_LEN
#define EACH_SZY_NAME_LEN				sizeof(nuWORD)
#define NDY_CITYTOWN_NAME_LEN			40
#define PROTWONMAXCOUNT					48//


#define NURO_SEARCH_BUFFER_MAX			2000L //no need
#define NURO_SEARCH_BUFFER_MAX_DYPOI	1000L
#define NURO_SEARCH_BUFFER_MAX_POI		1000L
#define NURO_SEARCH_BUFFER_MAX_ROADSZ	1000L
#define NURO_SEARCH_BUFFER_MAX_ROAD		5000L
#define NURO_SEARCH_BUFFER_MAX_CROSS	128L
#define NURO_SEARCH_CITYPERMAP_DFT		256L
#define NURO_SEARCH_TOWNPERCITY_DFT		256L
#define NURO_SEARCH_SORT_NAME           200L

#define SEARCH_TYPE_STOP			0x0000
#define SEARCH_TYPE_POI				0x0001
#define SEARCH_TYPE_ROAD			0x0002

const nuWORD _LOGDATAFILE_DATA_MAXNUM = 512; //table's size
typedef struct tagLOGDATAFILEHEADER
{
    nuWORD max;
    nuWORD now;
    nuWORD mode;
    nuWORD size; //<65535
}LOGDATAFILEHEADER;

typedef struct tagSEARCH_MPN_INFO
{
    nuLONG mapID;
    char logName[40];
    nuWORD realName[36];
    nuUINT x;
    nuUINT y;
}SEARCH_MPN_INFO;

typedef struct tagSEARCH_AREA_SET
{
    nuLONG	nMapIdx;
    nuWORD	nCityID;
    nuWORD	nTownID;
}SEARCH_AREA_SET;
typedef struct tagSEACHMAIN
{
    nuWORD	nSearchType;
    nuWORD	nStep;
    nuLONG	nMapIdx;
    nuLONG	nCityID;
    nuLONG	nTownID;
}SEARCHMAIN, *PSEARCHMAIN;
typedef enum tagNURO_SEARCH_TYPE
{
    NURO_SEARCH_NEW = 0,
        NURO_SEARCH_NEXT,
        NURO_SEARCH_PRE,
        NURO_SEARCH_RELOAD,
        NURO_SEARCH_LAST,
        NURO_SEARCH_NEXT_SC,
        NURO_SEARCH_PRE_SC,
        NURO_SEARCH_ALL,
		NURO_SEARCH_NONUMBER //Prosper add, for get noNumber namelist 
		//###INSIDE ONLY
}NURO_SEARCH_TYPE;
typedef struct tagNURO_SEARCH_FILTERRES
{
    nuWORD idx;
    nuBYTE strokes;
    nuBYTE res;
    nuWORD name[EACH_BUFFER_NAME_LEN];
}NURO_SEARCH_FILTERRES;
typedef struct tagNURO_SEARCH_NORMALRES
{
    nuWORD name[EACH_BUFFER_NAME_LEN-1];
    nuWORD strokes;
}NURO_SEARCH_NORMALRES;
typedef struct tag_ListName				//Unicode 轉 Big5 所需結構 
{
	nuWCHAR wsListName2[EACH_ROAD_NAME_LEN>>1];
}ListName;
typedef enum tagNURO_SKT
{
    NURO_SKT_TYPE= 1,
        NURO_SKT_NAME,
        NURO_SKT_AREA,
        NURO_SKT_PHONE,
        NURO_SKT_CARFAC,
        NURO_SKT_SPI
}NURO_SKT;

typedef struct tagNURO_SEARCHINFO
{
    nuLONG	nMapID;
    nuWORD	nCityID;
    nuWORD	nTownID;
    nuBYTE	nTP1ID;
    nuBYTE	nTP2ID;
    nuWCHAR	name[256];
}NURO_SEARCHINFO;

typedef struct tagSEARCH_CB_HEADER
{
    nuDWORD	nCityCount;
    nuDWORD nTownCount;
}SEARCH_CB_HEADER;
typedef struct tagSEARCH_CB_TOWNINFO
{
    nuWORD	nCityCode;
    nuWORD	nTownCode;
    nuWCHAR	wsTownName[EACH_TOWN_NAME_LEN>>1];
    nuDWORD	nTownBlkInfoCount;
}SEARCH_CB_TOWNINFO;

#ifdef NURO_OS_WINDOWS
#pragma pack(push) 
#pragma pack(1)
#endif
__ATTRIBUTE_PRE struct tagSEARCH_BH_HEAD
{
    nuBYTE version[6];
    nuBYTE reverser[8];
    nuBYTE buildData[8];
    nuBYTE mapId[6];
}__ATTRIBUTE_PACKED__;
typedef struct tagSEARCH_BH_HEAD    SEARCH_BH_HEAD;
typedef struct tagSEARCH_BH_HEAD    SEARCH_MAP_INFO;
#ifdef NURO_OS_WINDOWS
#pragma pack(pop) 
#endif

typedef struct tagNURO_CAR_FACTYPE
{
    nuWORD name[68];
    nuUINT addr;
    nuUINT count;
}NURO_CAR_FACTYPE;
typedef struct tagNURO_CAR_DATA
{
    nuUINT x;
    nuUINT y;
    nuWORD city;
    nuWORD town;
    nuUINT nameAddr;
    nuUINT extAddr;
}NURO_CAR_DATA;

typedef struct tagSEARCH_SZY_CITYINFO
{
    nuWORD	nCityCode;
    nuWORD	nWordCount;
    nuUINT	nStartAddr;
}SEARCH_SZY_CITYINFO;

typedef struct tagSEARCH_SZY_STROKE
{
    nuWORD nStrokes;	
    nuWORD nTownID;	
    nuWORD nWord;		
    nuWORD nZhuyin;
}SEARCH_SZY_STROKE;

typedef struct tagSEARCH_PI_CITYINFO
{
    nuUINT nCityID;
    nuUINT nPoiCount;
    nuUINT nStartPos;
}SEARCH_PI_CITYINFO;
typedef struct tagSEARCH_PI_POIINFO
{
    nuUINT	nNameAddr;
    nuWORD	nCityID;
    nuWORD	nTownID;
    nuWORD	nTP1ID;
    nuWORD	nTP2ID;
    nuUINT	nOtherInfoAddr;
    nuUINT	nX;
    nuUINT	nY;
}SEARCH_PI_POIINFO;
typedef struct tagSEARCH_PA_INFO
{
    nuWORD phone[32];
    nuWORD fex[32];
    nuWORD nameext[128];
    nuWORD addr[128];
    nuWORD web[128];
    nuWORD time[128];
}SEARCH_PA_INFO;
typedef struct tagSEARCH_PDW_A1
{
    nuWORD	nCityID;
    nuWORD	nTownID;
    nuBYTE	nTP1ID;
    nuBYTE	nTP2ID;
    nuBYTE	nNameLen;
    nuBYTE	nRes;
    nuUINT	nNameAddr;
    nuUINT	nMoreInfoAddr;
    NUROSPOINT pos;
	NUROSPOINT NaviPos;
}SEARCH_PDW_A1;
typedef struct tagSEARCH_SPOIINFO
{
    nuLONG	nMapID;
    nuWORD	nCityID;
    nuWORD	nTownID;
    nuWORD	nTP1ID;
    nuWORD	nTP2ID;
    nuUINT	nX;
    nuUINT	nY;
    nuUINT	nPAAddr;
    nuUINT	nNameAddr;
    nuBYTE	nIsFill;
    nuBYTE	nRes[3];
}SEARCH_SPOIINFO;
#define SEARCH_BT_POI 1
#define SEARCH_BT_ROAD 2
typedef struct tagSEARCH_BUFFER
{
    nuBYTE	type;//0, poi, road
    nuBYTE	strokes;
    nuBYTE	ex1c;
    nuBYTE	ex2c;
    nuLONG	mapID;
    nuWORD	cityID;
    nuWORD	townID;
    nuUINT	x;
    nuUINT	y;
    nuUINT	extAddr;//paaddr, pdnaddr_house.number
    nuUINT	nameAddr;
	nuUINT  nFindCount;
	nuUINT  nNearDis;
    nuWORD	name[EACH_BUFFER_NAME_LEN];
	nuUINT  nDistance;
    // 	nuUINT	blkIdx;
    // 	nuUINT	blkAddr;
    // 	nuUINT	tagIdx;//road idx, poi idx , @blk
}SEARCH_BUFFER;
typedef struct tagSEARCH_BUFFER_ROAD
{
    nuBYTE	type;//0, poi, road
    nuBYTE	strokes;
    nuBYTE	ex1c;
    nuBYTE	ex2c;
    nuLONG	mapID;
    nuWORD	cityID;
    nuWORD	townID;
    nuUINT	x;
    nuUINT	y;
    nuUINT	extAddr;
    nuUINT	nameAddr;
    //nuWORD	name[EACH_BUFFER_NAME_LEN];
}SEARCH_BUFFER_ROAD;
typedef struct tagSEARCH_POIINFO
{
    nuLONG	nMapID;
    nuWORD	nCityID;
    nuWORD	nTownID;
    nuWORD	nTP1ID;
    nuWORD	nTP2ID;
    nuUINT	nX;
    nuUINT	nY;
    SEARCH_PA_INFO moreinfo;
    nuWCHAR	name[EACH_POI_NAME_LEN>>1];
}SEARCH_POIINFO;
typedef struct tagSEARCH_RNC_HEADER
{
    nuUINT	nRNCount;
    nuUINT	nRNCCount;
    nuUINT	nCTInfoStartAddr;
}SEARCH_RNC_HEADER;

typedef struct tagSEARCH_RNC_CITYINFO
{
    nuWORD	nCityCode;
    nuWORD	nTownCount;
    nuUINT	nNextCityAddr;
}SEARCH_RNC_CITYINFO;

typedef struct tagSEARCH_RNC_TOWNINFO
{
    nuUINT	nTownCode;
    nuUINT	nRoadCount;
    nuUINT	nRNStartAddr;
}SEARCH_RNC_TOWNINFO;

typedef struct tagSEARCH_BH_INFO
{
    nuUINT BlockID;
    nuUINT POIDataAddr;
    nuUINT POI_DataSize;
    nuUINT RoadDataAddr;	
    nuUINT RoadAddCount;
    nuUINT Road_DataSize;
    nuUINT BglineDataAddr;
    nuUINT BGL_DataSiez;
    nuUINT BgAreaDataAddr;
    nuUINT BGA_DataSiez;
}SEARCH_BH_INFO;
typedef struct tagSEARCH_CB_BIL
{
    nuUINT fromBlkID;
    nuUINT toBlkID;
}SEARCH_CB_BIL;
typedef struct tagSEARCH_PDN_NODE_DI
{
	nuUINT doorNumber:20;
	nuUINT doorNumber2:12;
	NUROPOINT pos;
}SEARCH_PDN_NODE_DI;
typedef struct tagSEARCH_PDN_NODE
{
    nuWORD CityID;	
    nuWORD TownID;	
    nuUINT RoadNameAddr;	
    nuWORD DoorRangeMin;	
    nuWORD DoorRangeMax;	
    nuUINT doorNoCount;
    SEARCH_PDN_NODE_DI di[1];
}SEARCH_PDN_NODE;

typedef struct tagSEARCH_PDN_NODE_R
{
    nuWORD CityID;	
    nuWORD TownID;	
    nuUINT RoadNameAddr;	
    nuWORD DoorRangeMin;	
    nuWORD DoorRangeMax;	
    nuUINT doorNoCount;
    nuUINT bAddr;
}SEARCH_PDN_NODE_R;

typedef struct tagSEARCH_RDW_BLKINFO
{
    nuUINT RoadCount;		
    nuUINT RoadCommonCount;
    nuUINT VertexCount;	
}SEARCH_RDW_BLKINFO;

typedef struct tagSEARCH_RDW_A1
{
    nuWORD RoadCommonIndex;
    nuWORD VertexCount;
    nuUINT VertexAddCount:24;
    nuUINT OneWay:2;
    nuUINT LineNum:6;
    nuUINT HouseNumAddr;
}SEARCH_RDW_A1;

typedef struct tagSEARCH_RDW_A2
{
    nuUINT RoadNameAddr;
    nuBYTE RoadNameLen;	
    nuBYTE RoadType:4;
    nuBYTE SpeedLimit:4;
    nuWORD CityID:6;
    nuWORD TownID:10;
}SEARCH_RDW_A2;

typedef struct tagSEARCH_RTINFO
{
    nuUINT TotalNodeCount;	
    nuUINT TotalRoadCount;
    nuUINT RtConnectStruCount;
    nuUINT RtIndexStruStartAddr;
    nuUINT RtConnectStruStartAddr;
}SEARCH_RTINFO;
typedef struct tagSEARCH_RTINFO_M
{
    nuUINT baseX;
    nuUINT baseY;
    nuUINT a1count;
    nuUINT a2count;
    nuUINT a3count;
    nuUINT a1addr;
    nuUINT a2addr;
    nuUINT a3addrM;
}SEARCH_RTINFO_M;
#ifdef VALUE_EMGRT
typedef struct tagSEARCH_RTINFO_A1
{
    nuUINT NodeID:20;
    nuUINT RT_BlockID:12;
    nuUINT RtConnectStruIndex:20;
    nuUINT coor_level:4;
    nuUINT ConnectCount:6;
    nuUINT LeftTurnMark:2;
    nuUINT x;
    nuUINT y;
}SEARCH_RTINFO_A1;
#else
typedef struct tagSEARCH_RTINFO_A1
{
    nuUINT NodeID:20;
    nuUINT RT_BlockID:12;
    nuUINT RtConnectStruIndex:20;
    nuUINT coor_level:4;
    nuUINT ConnectCount:6;
    nuUINT LeftTurnMark:2;
    nuWORD x;
    nuWORD y;
}SEARCH_RTINFO_A1;
#endif
typedef struct tagSEARCH_RTINFO_A2
{
    nuUINT NodeID:20;
    nuUINT RT_BlockID:12;
    nuWORD RoadLength;
    nuBYTE RoadClass;
    nuBYTE ConnectType:2;
    nuBYTE VoiceRoadType:6;
}SEARCH_RTINFO_A2;
typedef struct tagSEARCH_RTINFO_A3
{
    nuWORD BlockIndex;
    nuWORD RoadIndex;
    nuUINT FNodeID:20;
    nuUINT FRT_BlockID:12;
    nuUINT TNodeID:20;
    nuUINT TRT_BlockID:12;
}SEARCH_RTINFO_A3;
typedef struct tagSEARCH_REF_INFO
{
    nuWORD cityID;
    nuWORD townID;
    nuUINT rtAreaID;
}SEARCH_REF_INFO;
typedef struct tagNURO_SEARCH_SDN_LIST
{
	nuWORD	name[EACH_BUFFER_NAME_LEN-1];
	nuDWORD nIdx;
}NURO_SEARCH_SDN_LIST;
#endif
