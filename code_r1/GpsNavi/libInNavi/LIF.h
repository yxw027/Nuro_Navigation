// LIF.h: interface for the CLIF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIF_H__9950F05D_D3BE_4E93_8E3D_90AA9F2926D3__INCLUDED_)
#define AFX_LIF_H__9950F05D_D3BE_4E93_8E3D_90AA9F2926D3__INCLUDED_

#include "NuroDefine.h"
#include "NuroEngineStructs.h"

typedef struct  tagLIFHEADER
{
	nuWORD	 nMapID;
	nuWORD	 nRT_Area_Count;
	nuDWORD  nBlockCount;
	//nuDWORD* pBlockAddressList;
}LIFHEADER,*PLIFHEADER;

typedef struct tagLIFBLOCKHEADER
{
	nuDWORD nTotalRdSegs;
	nuDWORD nC1Total;
	nuDWORD nC2Total;
//	nuDWORD *pRdIdxList;
}LIFBLOCKHEADER;

typedef struct tagLIFBLOCKRD_B
{
	nuBYTE nLANES;
	nuBYTE nCLANES;
	nuBYTE nARROWS;
	nuBYTE nCTYPE;
	nuDWORD B_address;
}LIFBLOCKRD_B;

typedef struct tagLIFBLOCKC1
{
	nuDWORD nRoadIdx;
	nuBYTE  nLANE_NO;
	nuBYTE  nLANE_TYPE:4;
	nuBYTE  nAS:4;
	nuBYTE  nCT:4;
	nuBYTE  nUTURNFLAG:4;
	nuBYTE  nRESERVER;
}LIFBLOCKC1;

typedef struct tagLIFBLOCKC2
{
	nuDWORD nBlkIdx;
	LIFBLOCKC1 BLOCKC1Info;
}LIFBLOCKC2;
 
/*******************返回的数据结构*****************/
/*
#define   ARROW_MAX   6
#define   LANS_MAX	  16
typedef struct tagARROW_STRU
{
	nuBYTE nAS;
	nuBYTE nResver[3];
}ARROW_STRU;
typedef struct tagARROW_SET
{
	nuBYTE nArrowNum;
	ARROW_STRU nArrow[ARROW_MAX];
}ARROW_SET;

typedef struct tagLANS_CROSS_STRU
{
	nuBYTE nLANE_NO;
	nuBYTE nLANE_TYPE:4;
	nuBYTE nCT:4;
	ARROW_SET ArrowSet;
}LANS_CROSS_STRU;

typedef struct tagLANS_SET
{
	nuBYTE nLANSNum;
	LANS_CROSS_STRU LansStru[LANS_MAX];
}LANS_SET;
*/
class CLIF  
{
public:
	nuBOOL bJudgeHighLight(nuBYTE nArrow, nuLONG nAngle);
	nuBOOL bFindRoad(LIFBLOCKHEADER Blkheader, nuULONG nRoadIndex, nuLONG nBlkAddres, nuLONG *pRoadAddr, nuFILE *pFile);
	nuBOOL GetLIFInfo(nuULONG nMapID, nuULONG nBlockIndex, nuULONG nRoadIndex, nuLONG nTurnAngle, NOW_LAME_INFO *pLans, nuTCHAR *sFileName);
	CLIF();
	virtual ~CLIF();

private:
	nuBOOL AddArrowInfo(LIFBLOCKC1 *pBlkC1, nuLONG nTurnAngle, NOW_LAME_INFO *pLans);
	nuBOOL GetArrowInfo(nuFILE *pFile, LIFBLOCKRD_B *pBlockRD_b, nuLONG nTurnAngle, NOW_LAME_INFO *pLans);
	nuBOOL GetRD_BInfo(nuFILE *pFile, nuLONG nAddB, LIFBLOCKRD_B *pBkRD_B);
	nuBOOL GetRoadAddres(nuFILE *pFile, nuLONG *pRoadAddres, nuLONG nRoadIndex, nuLONG nBlockBaseAdd );
	nuBOOL GetLIFBLOCKHEADER(nuFILE *pFile, nuLONG nStartAdd, LIFBLOCKHEADER*pBlkHeader);
	nuBOOL GetBlockAddres(nuFILE *pFile, nuLONG *pBlkAddres, nuLONG nBlkIndex);
	nuBOOL GetLIFHeader(nuFILE *pFile, LIFHEADER *pLIFHeader);
	nuBOOL OpenFile(nuUSHORT nMapID, nuFILE **ppFile);

private:
	nuLONG    m_nMapID;
	nuLONG    m_nBlockIndex;
	nuLONG    m_nRoadIndex;
	nuTCHAR   m_sFileName[256];
};

#endif // !defined(AFX_LIF_H__9950F05D_D3BE_4E93_8E3D_90AA9F2926D3__INCLUDED_)
