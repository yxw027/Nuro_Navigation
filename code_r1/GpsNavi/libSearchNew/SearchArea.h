#if !defined(AFX_SEARCHAREA_H__8B1E23A8_F9DA_4A1A_BB5C_8E326EBDFB73__INCLUDED_)
#define AFX_SEARCHAREA_H__8B1E23A8_F9DA_4A1A_BB5C_8E326EBDFB73__INCLUDED_

#include "SearchBaseZ.h"

class CMapFileCbZ;
class CSearchArea : public CSearchBaseZ  
{
public:
    CSearchArea();
    virtual ~CSearchArea();
    nuBOOL Init();
    nuVOID Free();
    
    nuUINT	LoadCBInfo(nuWORD nMMIdx, nuUINT nMaxLen);
    nuVOID	FreeCBInfo();
    nuUINT	GetCityTotal();
    nuUINT	GetTownTotal();
    nuUINT	GetMapNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
    nuUINT	GetCityNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
    nuUINT	GetTownNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
    nuBOOL	GetCityNameByID(nuWORD cityID, nuVOID *pBuffer, nuUINT nMaxLen);
    nuBOOL	GetTownNameByID(nuWORD cityID, nuWORD townID, nuVOID *pBuffer, nuUINT nMaxLen);
    /**
    * @brief 获得TOWN的BLKINFO 见CB档中towninfo中BlockInfoCount起始部分
    */
    nuUINT	GetTownBlkInfo(nuWORD cityID, nuWORD townID, nuBYTE *pBuffer, nuUINT nMaxLen);
private:
    /**
    * @brief 获得TOWNINFO
    */
    nuBOOL	GetTownInfo(nuBYTE *pCBInfo, nuUINT addr, SEARCH_CB_TOWNINFO *pTI);
    /**
    * @brief 获得TOWNINFO中的cityid
    */
    nuWORD	GetTownInfo_CC(nuBYTE *pCBInfo, nuUINT addr);
    /**
    * @brief 获得TOWNINFO中的townid
    */
    nuWORD	GetTownInfo_TC(nuBYTE *pCBInfo, nuUINT addr);
    /**
    * @brief 搜索ity的首town填入m_nFirstTownIdxBuffer中
    */
    nuWORD	SearchTownFirst(nuWORD nCityIdx,
        nuBYTE *pCBInfo, 
        nuUINT nTownCount);
    nuUINT	m_nTownTotal;       ///town总数
    nuWORD	m_nCBInfoIdx;       ///内存标志
    nuLONG	m_nST_MapID;        ///load资料依赖的mapid
    nuWORD	m_nFirstTownIdxBuffer[NURO_SEARCH_CITYPERMAP_DFT];///每个city的首town位置
};

#endif // !defined(AFX_SEARCHAREA_H__8B1E23A8_F9DA_4A1A_BB5C_8E326EBDFB73__INCLUDED_)
