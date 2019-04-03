#if !defined(AFX_SEARCHY_H__B70EFCBE_9CCC_4004_BC5D_29B00AA0F1B4__INCLUDED_)
#define AFX_SEARCHY_H__B70EFCBE_9CCC_4004_BC5D_29B00AA0F1B4__INCLUDED_

 
#include "NuroDefine.h"
#include "nuSearchDefine.h"
#include "SearchBaseZ.h"
#include "SearchArea.h"
#include "SearchPoi.h"
#include "SearchRoad.h"	
#include "MapFileSKB.h"
/**
 * @brief 地图资料搜索类
 * 包含模块：POI搜索 地图搜索 道路搜索
*/
class CSearchY : public CSearchBaseZ
{
public:
    CSearchY();
    virtual ~CSearchY();    
    /**
     * @brief 初始化搜索
     * @param nidx 为搜索分配的内存索引
     * @param nLen 为搜索分配的内存长度
     * @return nuTRUE: 成功 nuFALSE: 失败
    */
    nuBOOL Init(nuWORD nIdx, nuUINT nLen);
    nuVOID Free();
    nuBOOL SetMapId(nuLONG resIdx);
    nuBOOL SetCityId(nuWORD resIdx);
    nuBOOL SetTownId(nuWORD resIdx);
    nuBOOL SetPoiTP1(nuUINT resIdx);
   // nuBOOL SetPoiTP1(nuUINT resIdx,nuUINT word);

    nuBOOL SetPoiTP2(nuUINT resIdx);
    nuBOOL SetPoiKeyDanyin( nuWCHAR *pDanyin, nuBOOL bComplete);
    nuBOOL SetPoiKeyDanyin2( nuWCHAR *pDanyin, nuBOOL bComplete);   
	nuBOOL SetPoiKeyName(nuWCHAR *pName, nuBOOL bFuzzy);
    nuBOOL SetPoiKeyPhone(nuWCHAR *pPhone);
    nuBOOL SetSearchArea(NURORECT *pRect);
    nuBOOL SetRoadSZSLimit(nuUINT nMin, nuUINT nMax);
    nuBOOL SetRoadSZZhuyin(nuWORD zhuyin);
    nuBOOL SetRoadSZ(nuUINT nResIdx);
    nuBOOL SetRoadKeyDanyin(nuWORD *pDanyin);
    nuBOOL SetRoadKeyDanyin2(nuWORD *pDanyin);
	nuBOOL SetRoadKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging = nuFALSE);
    nuBOOL SetRoadName(nuUINT nResIdx, nuBOOL bRealIdx);
    nuVOID SetStrokeLimIdx(nuUINT nResIdx); ///设置笔划上下时的索引起始
    /**
     * @brief 汽车服务据点搜索 获取厂家类别
    */
    nuUINT	GetPoiCarFacAll(nuVOID *pBuffer, nuUINT nMaxLen);
    nuBOOL	SetPoiCarFac(nuUINT nResIdx);
    /**
     * @brief 主题搜索 获取所所有主题类型
    */
    nuUINT  GetPoiSPTypeAll(nuVOID *pBuffer, nuUINT nMaxLen);
    nuBOOL	SetPoiSPType(nuUINT nResIdx);    
    nuUINT	GetCityTotal();
    nuUINT	GetTownTotal();
    nuUINT	GetPoiTotal();
    nuUINT	GetTP1Total();
    nuUINT	GetRoadSZTotal();//获取道路首字总数
    nuUINT	GetRoadTotal();
    nuUINT	GetRoadCrossTotal();
    /**GetRoadTownID
     * @brief 结果记录的次字搜索 
     * 对搜索结果进行首字过滤 
     *
     * @param pHeadWords 首字串
     * @param pBuffer 次字搜集保存的内存
     * @param nMaxWords 次字搜索的数量限制
     *
     * @return 返回搜集到的次字数量
    */
    nuUINT	GetNextWordAll(nuWORD* pHeadWords, nuWORD *pBuffer, nuUINT nMaxWords);    
    nuINT GetMapNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
    nuINT GetCityNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
    nuINT GetTownNameAll(nuVOID *pBuffer, nuUINT nMaxLen);    
    /**
     * @brief 结果记录的关键字过滤-二次搜索
     *
     * @param pKey 关键字限定
     * @param pBuffer 保存结果的内存 返回内容按NURO_SEARCH_FILTERRES格式化
     * @param nMaxLen 内存长度
     *
     * @return 返回搜集到的结果数量
    */
    nuINT GetFilterResAll(const nuWCHAR *pKey, nuVOID *pBuffer, nuUINT nMaxLen);    
    /**
     * @brief 获得POI搜索结果
     *
     * @param type 搜索类型
     * @param nMax 结果数量限制
     * @param pBuffer 保存结果的内存 返回内容按NURO_SEARCH_NORMALRES格式化
     * @param nMaxLen 内存长度
     *
     * @return 返回搜集到的结果数量
    */
    nuINT  GetPoiNameList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen);
//	nuINT  GetPoiNameList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen,nuUINT word);
    nuINT  GetPoiTP1All(nuVOID *pBuffer, nuUINT nMaxLen);
    nuINT  GetPoiTP2All(nuVOID *pBuffer, nuUINT nMaxLen);
    nuBOOL GetPoiMoreInfo(nuUINT nResIdx, SEARCH_PA_INFO *pPaInfo, nuBOOL bRealIdx);
    nuBOOL GetPoiPos(nuUINT nResIdx, NUROPOINT* pos, nuBOOL bRealIdx);
    nuBOOL GetPoiCityName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx);
	nuLONG GetPoiCityID(nuUINT nResIdx, nuBOOL bRealIdx); // Added by Damon 20100309
    nuBOOL GetPoiTownName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx);
	nuUINT GetPoiNearDis(nuUINT nResIdx, nuBOOL bRealIdx);
    /**
     * @brief 按电话精确搜索POI
    */
    nuBOOL GetPoiByPhone(const nuWCHAR *pPhone, SEARCH_POIINFO *pInfo);    
    nuINT	 GetRoadSZList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen);
    /**
     * @brief 获得ROAD搜索结果
     *
     * @param type 搜索类型
     * @param nMax 结果数量限制
     * @param pBuffer 保存结果的内存 返回内容按NURO_SEARCH_NORMALRES格式化
     * @param nMaxLen 内存长度
     *
     * @return 返回搜集到的结果数量
    */
    nuINT	 GetRoadNameList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen);	
    nuBOOL GetRoadPos(NUROPOINT* pos);
    nuBOOL GetRoadTownName(nuWORD *pBuffer, nuUINT nMaxLen);
    nuBOOL GetRoadCityName(nuWORD *pBuffer, nuUINT nMaxLen);
    nuUINT	GetRoadDoorTotal();
    /**
     * @brief 获取门牌号信息
     * 需先设置要搜索的道路
    */
    nuBOOL GetRoadDoorInfo(nuVOID *pBuffer, nuUINT nMaxLen);
    /**
     * @brief 获得ROADCROSS搜索结果
     *
     * @param type 搜索类型
     * @param nMax 结果数量限制
     * @param pBuffer 保存结果的内存 返回内容按NURO_SEARCH_NORMALRES格式化
     * @param nMaxLen 内存长度
     *
     * @return 返回搜集到的结果数量
    */
    nuINT	 GetRoadCrossList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen);
    nuBOOL GetRoadCrossPos(nuUINT nResIdx, NUROPOINT* pos);
    nuBOOL GetRoadCrossTownName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen);
    nuBOOL GetRoadCrossCityName(nuUINT nResIdx, nuWORD *pBuffer, nuUINT nMaxLen);    
    nuUINT	GetMainRoadTotal();///主道路模式API
    nuUINT	GetChildRoadTotal(nuUINT nMainRoadIdx, nuBOOL bRealIdx);///主道路模式API
    nuINT		GetRoadNameList_MR(NURO_SEARCH_TYPE type, nuWORD nMax, nuVOID* pBuffer, nuUINT nMaxLen);///主道路模式API
    nuUINT	GetRoadNameALL_MR(nuUINT nMainRoadIdx, nuVOID* pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx);///主道路模式API
    nuBOOL	SetRoadName_MR(nuUINT nMainRoadIdx, nuUINT nRoadIdx, nuBOOL bRealIdx);///主道路模式API

    //add prosper.05
    nuVOID Stroke_GetNextWordData(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* candi, const nuUINT candylen, const nuBOOL ReSort);
    nuVOID ZhuYinForSmart_SecondWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont, const nuUINT candylen, const nuBOOL ReSort);
    nuBOOL ZhuYinForSmart_OtherWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort);
    nuVOID Stroke_Poi_GetNextWordData(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* candi, const nuUINT candylen, const nuBOOL ReSort);
    nuVOID ZhuYinForSmart_Poi_SecondWords(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont, const nuUINT candylen, const nuBOOL ReSort);
    nuBOOL ZhuYinForSmart_Poi_OtherWords(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort);
  
	nuINT GetPoiPZList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen);
    nuUINT	GetPoiPZTotal();
    nuBOOL SetPoiPZZhuyin(nuWORD zhuyin);
	//nuUINT	GetPoiTotal(nuUINT temp);
	nuBOOL SetPoiPZ(nuUINT nResIdx);
	nuBOOL SetPoiKeyNameP(nuWCHAR *pName, nuBOOL bFuzzy);
	nuINT  GetRoadTownID();

	nuUINT SetAreaCenterPos(nuroPOINT pos);
    //内存控制 SKB状态控制
    	nuBOOL    StartSKBSearch(nuUINT nType);
        nuVOID    StopSKBSearch();
    /**
    * @brief 获得首拼音查询列表
    *   内存分配在road搜索的相关资料前 CT之后
    * @param keyStr 传入拼音列表
    * @param pBuffer 查询结果列表
    * @param nMaxLen pBuffer长度
    * @param resCount 结果涉及的道路总数
    * @return 获得的拼音数量
    */
     nuINT     GetSKBAll(nuCHAR* keyStr, nuCHAR* pBuffer, nuINT nMaxLen, nuINT* resCount);
     nuINT GetNDYSKBAll(nuCHAR* keyStr, nuCHAR* pBuffer, nuINT nMaxLen, nuINT* resCount);
     nuINT    GetSKBInBuffer(nuCHAR* pBuffer, nuINT nMaxLen);
	 nuBOOL   JudgeSKB(nuCHAR* strDes, nuCHAR* strSrc, nuINT nDesLen, nuINT nSrcLen, nuWORD townID);
	nuLONG    m_SKBMapIDT;
    nuWORD  m_SKBCityIDT;
    nuWORD  m_SKBTownIDT;
    nuUINT  m_SKBLoadLen;
    nuUINT  m_SKBResT;
    SEARCH_SKB_CITYINFO infoT;
    nuCHAR    m_SKBKeyStrT[128];
    nuBYTE  m_SKBBuffer[128];
   

	nuBOOL GetMode(VOICEROADFORUI& data,nuVOID* wName,nuBOOL &bMode);
	nuBOOL GetPosInfo(nuBOOL &bNumChange,nuINT &nDoorNum,nuINT nDoorNum2,nuroPOINT &point);
	//nuBOOL SetVoiceSPDKeyName(VOICEROADFORUI data,nuVOID* wRoad,nuroPOINT &point);
	nuBOOL SetNoName(nuBOOL bRes);

	nuBOOL GetRoadSKB(nuVOID* pSKBBuf, nuUINT& nCount); //Prosper 2011 1216
	nuBOOL GetPoiSKB(nuVOID* pSKBBuf, nuUINT& nCount);	//Prosper 2011 1216

	nuBOOL GetRoadWordData(nuINT &nTotal,nuVOID* pBuffer, nuUINT nMaxLen);
	nuBOOL  LoadSKB(nuVOID* pBuffer,nuVOID* pBuffer2);
	nuBOOL GetNEXTAll(nuVOID* pSKBBuf, nuUINT& nCount);
private:
    /**
     * @brief 重置所有子模块搜索结果
    */
    nuVOID ResetAllSearch();
    //nuBOOL StartSearchPoi(nuUINT);//prosper
	nuWORD g_nSKBMMIdx;
	nuWORD g_nSKBMMIdx2;

	nuBOOL StartSearchPoi();
    nuVOID QuitSearchPoi();
    nuBOOL StartSearchRoad();
    nuVOID QuitSearchRoad();
private:
    CSearchArea	m_SearchArea;//地区搜索
    CSearchPoi	m_SearchPoi;//POI搜索
#ifdef _USE_SEARCH_ROAD_
    CSearchRoad m_SearchRoad;//道路搜索
#endif
    nuWORD	m_nLoadBufferIdx;//为SEARCH模块提供的内存索引
    nuUINT	m_nLoadBufferMaxLen;//为SEARCH模块提供的内存大小
    nuUINT	m_nLB_CTLen;//载入的CITY TOWN信息长度
	nuINT temp; //prosper

	 nuUINT count;
	   nuWORD townID;
	  nuroPOINT m_pos;
	  // nuCHAR *pSkb,*pSkb1;
};

#endif // !defined(AFX_SEARCHY_H__B70EFCBE_9CCC_4004_BC5D_29B00AA0F1B4__INCLUDED_)
