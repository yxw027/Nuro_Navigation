#if !defined(AFX_SEARCHPOI_H__E7F89252_746D_4834_9F2C_C55FACACFCF1__INCLUDED_)
#define AFX_SEARCHPOI_H__E7F89252_746D_4834_9F2C_C55FACACFCF1__INCLUDED_

#include "SearchBaseZ.h"
class CMapFilePnY;

/**
* @class CSearchPoi
* @brief POI搜索 \n
*   模块内存主分配读取内容顺序（未使用的数据长度为0） \n
*   AREA搜索--CT、BHIdx \n
*   非AREA搜索--CT、PI、PN  \n
*   由于载入资料和搜索结果使用了同一份区域标记，在清除每部分资料时会同步清除对应搜索结果
*/
class CSearchPoi : public CSearchBaseZ  
{
public:
    CSearchPoi();
    virtual ~CSearchPoi();
    nuBOOL Init(nuWORD nIdx, nuUINT nLen, nuUINT nIdleAddr);
    nuVOID Free();    
    nuBOOL SetPoiKeyDanyin(nuWCHAR *pDanyin,nuBOOL bComplete = nuFALSE ); // @bComplete added 20100118 by ouyang为了单音索引获取完全一致的字串
	nuBOOL SetPoiKeyDanyin2(nuWCHAR *pDanyin,nuBOOL bComplete = nuFALSE ); // @bComplete added 20100118 by ouyang为了单音索引获取完全一致的字串

	nuBOOL SetSearchArea(NURORECT *pRect);
    nuBOOL SetPoiKeyPhone(nuWCHAR *pPhone);    
    nuUINT	GetTP1Total();
    nuUINT	GetPoiTotal();    
    nuBOOL	GetPoiByPhone(const nuWCHAR *pPhone, SEARCH_POIINFO *pInfo);
    
	nuUINT  FlushPZYInfo();
	nuVOID    ClearPZYInfo();
    nuVOID    ResetPZSearch();
	nuVOID	ClearRNInfo();
	nuUINT  GetPoiPZTotal();
	nuINT     GetPoiPZList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen);
    nuWORD  SearchPoiPZ();
    nuBOOL    SetPoiPZZhuyin(nuWORD zhuyin);
   
	nuUINT GetPoiTotal(nuUINT);

	nuVOID    SetKeyNameFuzzy(const nuBOOL c_bFuzzy);
    /**
    * @brief 获得poi搜索结果                        \n
    *   结果以NURO_SEARCH_NORMALRES解释             \n
    * @return 获得的poi数量                         \n
    *   --在使用分页时，返回值的高16位为结果所在的页数，低16位为poi数量
    */
    nuINT		GetPoiNameList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen);   
	
	//prosper
	nuVOID ZhuYinForSmart_SecondWords(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& m_iCount, const nuUINT SelectWord, const nuBOOL ReSort);
	nuBOOL ZhuYinForSmart_OtherWords(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& m_iCount,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort);
	nuVOID Stroke_GetNextWordData(nuVOID* pPoiBuf, const nuUINT PoiBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* candi, const nuUINT candylen, const nuBOOL ReSort);
    nuBOOL SetPoiKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging);

    nuBOOL    SetPoiPZ(nuUINT nResIdx);
    
	//PROSPER 2011.1207
	nuBOOL     Sort_KEY_index(nuWORD);
	nuBOOL     GetPoiSKB(nuVOID* pSKBBuf, nuUINT& nCount);
	nuWCHAR	   KEY_Data[100];
	nuINT      nKeyCount;
	nuINT	   m_nCityID;
	nuINT	   m_nMAPID;




    /**
    * @brief 对结果进行二次搜索                     \n
    *   返回能pBuffer能容纳的结果                   \n
    *   pBuffer中的数据以NURO_SEARCH_FILTERRES解释  \n
    *   结果中携带真实的内部idx                     \n
    *   要使用该结果中的道路进行设置，需要传入此内部idx 并标记bRealIdx为nuTRUE
    */
    nuINT		GetPoiFilterResAll(const nuWCHAR *pKey, nuVOID *pBuffer, nuUINT nMaxLen);    
    nuUINT	GetPoiTP1All(nuVOID *pBuffer, nuUINT nMaxLen);
    nuUINT	GetPoiTP2All(nuVOID *pBuffer, nuUINT nMaxLen);    
    nuUINT	GetPoiCarFacAll(nuVOID *pBuffer, nuUINT nMaxLen);
    nuBOOL    SetPoiCarFac(nuUINT nResIdx);
    nuUINT  GetPoiSPTypeAll(nuVOID *pBuffer, nuUINT nMaxLen);
    nuBOOL  	SetPoiSPType(nuUINT nResIdx);    
    nuBOOL    GetPoiMoreInfo(nuUINT nResIdx, SEARCH_PA_INFO *pPaInfo, nuBOOL bRealIdx=0);
    nuBOOL  	GetPoiPos(nuUINT nResIdx, NUROPOINT* pos, nuBOOL bRealIdx=0);
    nuWORD	GetPoiCityID(nuUINT nResIdx, nuBOOL bRealIdx=0);
    nuWORD	GetPoiTownID(nuUINT nResIdx, nuBOOL bRealIdx=0); 
    nuUINT  GetNextWordAll(nuWORD* pHeadWords, nuWORD *pBuffer, nuUINT nMaxWords);
	nuUINT  GetPoiNearDis(nuUINT nResIdx, nuBOOL bRealIdx=0);


	

    nuVOID ClearSearchInfo();

	nuINT	TransCode(nuINT code);
	nuVOID  SortByStroke(nuINT nTotal);
	nuUINT	Load(); // PROSPER 2013,05 add
	nuUINT SetAreaCenterPos(nuroPOINT pos);
	nuBOOL GetNEXTWORD(nuVOID* pKEYBuf, nuUINT& nCount) ;
private:
    /**
    * @brief 读取按area搜索poi时需要的资料  依赖BH档
    */
    nuBOOL LoadSearchInfo_AREA();
    /**
    * @brief 按cityid读取search需要的资料  依赖PI、PN档
    * @param cityID 
    * @param bLoadPN 是否需要loadPN档资料 在预计POI结果会很少时 可以不完整读取CITY的PN信息
    */
    nuBOOL LoadSearchInfo_CITY(nuWORD cityID, nuBOOL bLoadPN=nuTRUE);    
    /**
    * @brief  读取按电话搜索时需要的资料 \n
    * 由SearchNextPoi_PHONE 调用  停用
    */
    //nuBOOL LoadSearchInfo_PHONE(nuWORD cityID); 
    nuVOID ResetPoiSearch();
    /**
    * @brief  电话搜索 \n
    *   电话搜索由此API单独处理 以替代SearchNextPoi_PHONE实现   \n
    *   不支持笔划上下 POI的信息是在取得单笔资料时去加载
    * @param type 搜索类型
    * @param nMax 最多返回数量
    * @param pBuffer 保存结果的内存
    * @param nMaxLen 内存长度
    */
    nuINT		GetPoiList_PHONE(NURO_SEARCH_TYPE type, nuWORD nMax, 
        nuBYTE* pBuffer, nuUINT nMaxLen);    
    nuUINT	SearchNextPoi(NURO_SKT skt, nuUINT tag);
    /**
    * @brief  按danyin/name搜索 \n
    *        依赖LoadSearchInfo_CITY
    * @param tag 内部缓存索引地址 用以保存结果
    */
    nuUINT	SearchNextPoi_NAME(nuUINT tag);
	nuUINT	SearchNextPoi_NAME_CN(nuUINT tag);

    /**
    * @brief  按TP1/TP2搜索 \n
    *        依赖LoadSearchInfo_CITY
    * @param tag 内部缓存索引地址 用以保存结果
    */
    nuUINT	SearchNextPoi_TYPE(nuUINT tag);

	//add prosper 05
//	nuUINT	SearchNextPoi_TYPE2(nuUINT tag,nuUINT word);
    
	//nuUINT	SearchNextPoi_PHONE(nuUINT tag);//实现被GetPoiList_PHONE取代
    /**
    * @brief  按AREA搜索\n
    *       依赖LoadSearchInfo_AREA 完整搜索 依赖PDW FILE
    */
    nuUINT	SearchNextPoi_AREA();
    /**
    * @brief  按车厂类型搜索依赖此实现 完整搜索 
    */
    nuUINT	SearchNextPoi_Car();
    /**
    * @brief  按SPI类型搜索依赖此实现 完整搜索 
    */
    nuUINT	SearchNextPoi_SPI();
    nuUINT  BlkIDToIdx(nuUINT nBlkID);

	

	// Added by damon 20100202
	#define LONEFFECTCONST						1.113
	#define LATEFFECTCONST						1.109
	#define EFFECTBASE							1.000
	#define DEGBASE								100000

	double getLatEffect(nuroPOINT pt)
	{
		return (nuCos(pt.y/DEGBASE)*((pt.y*10/DEGBASE)%10) + nuCos(pt.y/DEGBASE+1)*(10-((pt.y*10/DEGBASE)%10)))/10;
	}
	// ---------------------

	

private:
    //nuBOOL    nuPhoneJudge(nuWCHAR* pDes, nuWCHAR* pKey, nuUINT desLen, nuUINT keyLen);

    /**
    * @brief  电话信息比较 忽略'-'
    * @param pDes 电话1
    * @param pSrc 电话2
    * @param desLen 电话1长度
    * @param keyLen 电话2长度
    */
    nuINT     nuPhoneCompare(nuWCHAR* pDes, nuWCHAR* pSrc, nuUINT desLen, nuUINT keyLen);
    
    /**
    * @brief  获得POI Name \n
    *       在PN由读取回来时进行内存中读取、其他情况直接从PN file获取name信息
    * @param idx 内部POI结果集索引
    * @param pNameBuffer 保存结果的内存
    * @param nMaxLen 内存长度
    * @param pnfile PN文件
    */
    nuBOOL    GetPoiName(nuUINT idx, nuBYTE* pNameBuffer, nuUINT nMaxLen, CMapFilePnY &pnfile);
    nuBYTE* GetPoiNameInfo(nuUINT addr);
    nuBOOL  	FillResBuffer(nuUINT tag, SEARCH_PI_POIINFO *pPoiInfo, nuBYTE strokes=-1);
    nuUINT  FindPhoneMatch();
    nuWORD	m_nLoadBufferIdx;   ///search使用的内存块标志
    nuUINT	m_nLoadBufferMaxLen;///search使用的内存块长度
    nuWORD	m_nCityIDTag;       ///city搜索时、标记当前搜索到的cityid
    nuWORD	m_nCityIDMax;       ///city搜索时、标记最大允许的cityid
    nuWORD	m_nCityIDLoad;      ///city搜索时、标记load回的资料cityid
    nuUINT	m_nLoadPoiCount;    ///保持load回的资料中的poi数
    nuUINT	m_nLB_CTLen;        ///加载map时读取的CITYTOWN信息，始终放在内存区最开始的位置
    nuUINT	m_nLB_PILen;        ///读取回的PI长度 若>0 读取的PI信息在内存区跟在m_nLB_CTLen后
    nuUINT	m_nLB_PNLen;        ///读取回的PN长度 若>0 读取的PI信息在内存区跟在m_nLB_PILen后
    nuUINT	m_nLoadPnBaseAddr;  ///load的PN资料相对PN文件的偏址 用于转换nameaddr到内存中的地址
    nuUINT  m_nLB_PILen2;
	nuUINT	m_nLoadPoiCount2;
    nuUINT	m_nLB_PNLen2; 
	nuUINT  m_nLoadPnBaseAddr2;
	
    nuLONG    m_nLB_PAMapID;      ///标记读取的PA资料的MAPID
//     nuUINT	m_nLB_PALen;    ///读取的PA资料的长度     
//     nuLONG    m_nLB_PI_B_MapID;
//     nuUINT  m_nLB_PI_B_Len;
//     nuUINT  m_nLB_PI_B_Total;
    nuUINT  m_firstPos;         ///标记电话搜索结果的起始位置
    nuUINT  m_lastPos;          ///标记电话搜索结果的结束位置    
    nuWORD	m_nSearchPoiKeyDanyin[EACH_POI_NAME_LEN>>1];    ///设置的keydanyin
    nuWORD	m_nSearchPoiKeyDanyinT[EACH_POI_NAME_LEN>>1];   ///标记结果依赖的keydanyin条件
    nuWORD	m_nSearchPhone[32];         ///设置的keyphone
    nuWORD	m_nSearchPhoneT[32];        ///标记结果依赖的keyphone条件
    NURO_SEARCHINFO m_SearchBufferInfo; ///标记结果依赖的searchinfo条件    
    nuWORD	m_nSearchTagPos;            ///标记UI对结果记录集中的读取点
    nuUINT	m_nLastGetCount;            ///上一次UI获取namelist结果数量
    nuUINT	m_nResTotal;                ///搜索出的总POI数量 为-1时表示无结果
    nuUINT  m_nCarFac;                  ///设置的车厂类型
    nuUINT  m_nCarFacT;                 ///标记结果依赖的车厂类型条件    
    nuUINT  m_nSPType;                  ///设置的SPI类型
    nuUINT  m_nSPTypeT;                 ///标记结果依赖的SPI类型    
    nuBOOL    m_bSA;                      ///表示将使用AREA搜索
    nuBOOL	m_bSAT;                     ///标记结果是否依赖AREA搜索
    NURORECT	m_sAreaRect;            ///设置的AREA条件
    NURORECT	m_sAreaRectT;           ///标记结果依赖的AREA条件
    nuUINT	    m_nBHIdxLen;            ///读取回的BH信息长度 按AREA搜索时使用 读取资料 \n
                                        ///在m_nLB_CTLen后    
    SEARCH_BUFFER m_SearchRes[NURO_SEARCH_BUFFER_MAX_POI];///缓存队列
    nuWORD  m_nSearchPoiKeyName[EACH_POI_NAME_LEN>>1];
	nuWORD  m_nSearchPoiKeyNameT[EACH_POI_NAME_LEN>>1];
	nuBOOL    m_bIsFromBeging;            ///眖秨﹍ゑ癸;

	nuBOOL     m_bComCompare; //added 20100118 by ouyang 标记单音索引搜索是不是采用完全比对的方式，返回完全比对的字串
	nuBOOL     m_bKeyNameFuzzy;
	
	nuBOOL     check;
	nuINT      temp;
	nuINT      num;

	nuINT CheckWordIsNumber(nuWORD WordCode); //CheckWord for Sort , prosper add by 20130701

   nuUINT m_nMMPzyLen;

	nuUINT m_nPzyLoadNum;
    nuUINT m_nPzySearchCityID;
    nuUINT m_nPzySearchMapID;

	nuUINT m_nPzySearchTag;
    nuUINT m_nPzySearchLastCount;

    nuUINT m_nPzyMinT ;
    nuUINT m_nPzyMaxT ;
    nuUINT m_nPzyZhuyinT;
    
    nuUINT m_nPzySearchTownID ;
    nuUINT m_nPzyResTotal;
	nuWORD m_nPzyWordbuffer[NURO_SEARCH_BUFFER_MAX_ROADSZ];    ///保存搜索出的首字

    nuUINT m_nPzyZhuyin;
	nuUINT m_nPzyMin;
	nuUINT m_nPzyMax;
    nuUINT Pzy_count;

	nuWORD  m_nDY_countIdx;
	nuWORD  m_nsort_countsIdx;
	nuBOOL  m_bcheck;
    

    nuWORD	m_nPzyWord;             ///KEYWORD
    nuINT 	nFirstSort ; 
	

	typedef struct tag_Code				//Unicode 锣 Big5 ┮惠挡篶 
	{

		nuINT	nBig;
		nuINT	nUniCode;    

	}Trans_DATA;
	Trans_DATA *data;
	nuINT		m_nWordIdx;
	nuINT		nSortLimt;
	nuBOOL		m_bNameSort;
	nuBOOL      m_bSort;
	
	//nuINT	    *m_nWordStartIdx;//[NURO_SEARCH_BUFFER_MAX_DYPOI];
	//nuINT	    *m_nWordLens;//[NURO_SEARCH_BUFFER_MAX_DYPOI];
	nuBOOL		m_bIndexSort;

	nuWORD	m_nWordStartIdxMemIdx;
	nuWORD	m_nWordLensMemIdx;
	nuWCHAR	   m_wsNEXT_WORD_Data[NEXT_WORD_MAXCOUNT];
	nuINT      nNEXT_WORD_COUNT;
	
	nuBOOL	m_bSetAreaCenterPos;
	nuroPOINT m_TypeCenterPos;
	nuINT		m_POI_DY_COUNT;

};

#endif // !defined(AFX_SEARCHPOI_H__E7F89252_746D_4834_9F2C_C55FACACFCF1__INCLUDED_)
