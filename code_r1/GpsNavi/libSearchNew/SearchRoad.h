#if !defined(AFX_SEARCHROAD_H__69340EF9_C168_4655_A955_5629DF66D5DA__INCLUDED_)
#define AFX_SEARCHROAD_H__69340EF9_C168_4655_A955_5629DF66D5DA__INCLUDED_

#include "SearchBaseZ.h"
#include "NuroClasses.h"

class CMapFileBhY;
class CMapFileRdwY;
class CSearchArea;
const nuINT _CROSS_COL_RN_LIMIT_ = 16;
const nuINT _CROSS_COL_PT_LIMIT_ = 1024;
const nuINT _SAST_JUDGE_RN_DIS_ = 6000;//RN file check NAMEADDR_DIS limit
const nuINT _SNST_COL_LIMIT_ = 128;

struct nuroMapArea
{
    nuLONG	nMapID;
    nuWORD	nCityID;
    nuWORD	nTownID;
};

/**
* @class  CSearchRoad  
* @brief  道路搜索  \n
*   模块内存主分配读取内容顺序（未使用的数据长度为0） \n
*   CT、SZY、RN、RNC、TownBLKInfo、RTInfo
*   由于载入资料和搜索结果使用了同一份区域标记，在清除每部分资料时会同步清除对应搜索结果 包含清除下层搜索内容
*/
class CSearchRoad : public CSearchBaseZ
{
public:
    CSearchRoad();
    virtual ~CSearchRoad();
    /**
    * @brief  初始化searchroad
    * @param nMMIdx 内存标志
    * @param nMMMaxLen 内存长度
    * @param nCBLen 加载的map的CITYTOWN信息长度
    * @param pSearchArea
    */
    nuBOOL  Init(nuWORD nMMIdx, nuUINT nMMMaxLen, nuUINT nCBLen, CSearchArea* pSearchArea);
    nuVOID  Free();
    nuBOOL	SetRoadSZSLimit(nuUINT nMin, nuUINT nMax);
    nuBOOL	SetRoadSZZhuyin(nuWORD zhuyin);
    nuUINT	GetRoadSZTotal();
    nuINT	GetRoadSZList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen); 

    nuBOOL	SetRoadSZ(nuUINT nResIdx);
    nuBOOL	SetRoadKeyDanyin(nuWORD *pDanyin);
	 nuBOOL	SetRoadKeyDanyin2(nuWORD *pDanyin);
    nuBOOL	SetRoadKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging = false);
    nuUINT	GetRoadTotal();
    /**
    * @brief 获得road搜索结果                       \n
    *   结果以NURO_SEARCH_NORMALRES解释             \n
    * @return 获得的road数量                        \n
    *   --在使用分页时，返回值的高16位为结果所在的页数，低16位为poi数量
    */
    nuINT	GetRoadNameList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen);

	//prosper 05
	nuVOID  Stroke_GetNextWordData(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* candi, const nuUINT candylen, const nuBOOL ReSort);
	
    nuVOID ZhuYinForSmart_SecondWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont, const nuUINT candylen, const nuBOOL ReSort);
	nuBOOL ZhuYinForSmart_OtherWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort);

	
	/**
    * @brief 对道路结果进行二次搜索                 \n
    *   在MR模式下 仅支持对主道路进行二次搜索       \n
    *   返回能pBuffer能容纳的结果                   \n
    *   pBuffer中的数据以NURO_SEARCH_FILTERRES解释  \n
    *   结果中携带真实的内部idx                     \n
    *   要使用该结果中的道路进行设置，需要传入此内部idx 并标记bRealIdx为true
    */
    nuINT	GetRoadFilterResAll(const nuWCHAR *pKey, nuVOID *pBuffer, nuUINT nMaxLen);
    nuBOOL	SetRoadName(nuUINT nResIdx, nuBOOL bRealIdx=0);
    /**
    * @brief  获取主道路总数
    */
    nuUINT  GetRoadTotal_MR();
    /**
    * @brief  获取子道路数量
    */
    nuUINT	GetChildRoadTotal(nuUINT nMainRoadIdx, nuBOOL bRealIdx=0);
    /**
    * @brief  获取主道路信息                        \n
    *   结果以NURO_SEARCH_NORMALRES解释
    * @return 获得的road数量                        \n
    *   --在使用分页时，返回值的高16位为结果所在的页数，低16位为poi数量
    */
    nuUINT	GetRoadNameList_MR(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen);
    /**
    * @brief  获取子道路信息[全返回]    \n
    *   若内存不足，返回可容纳的子道路信息
    * @param nMainRoadIdx 主道路索引 依赖GetRoadNameList_MR
    * @param bRealIdx 标记nMainRoadIdx是否为内部真实idx
    * @return 返回真实获取的子道路信息数量
    */
    nuUINT	GetRoadNameALL_MR(nuUINT nMainRoadIdx, nuBYTE* pBuffer, nuUINT nMaxLen, nuBOOL bRealIdx=0);
    /**
    * @brief  设置路名[主道路模式]
    * @param bRealIdx 标记nMainRoadIdx是否为内部真实idx
    */
    nuBOOL	SetRoadName_MR(nuUINT nMainRoadIdx, nuUINT nRoadIdx, nuBOOL bRealIdx=0);
    /**
    * @brief  次字搜索 对全结果进行次字搜索
    */
    nuUINT	GetNextWordAll(nuWORD* pHeadWords, nuWORD *pBuffer, nuUINT nMaxWords);
    /**
    * @brief  获得道路的门牌信息总数
    */
    nuUINT	GetRoadDoorTotal();
    /**
    * @brief  获得道路的门牌信息[全获取]
    * @return 内存不足时 获取门牌信息将失败
    */
    nuBOOL	GetRoadDoorInfo(nuVOID *pBuffer, nuUINT nMaxLen);
    nuBOOL	GetRoadPos(NUROPOINT* pos);
    nuWORD	GetRoadTownID();
    nuWORD	GetRoadCityID();
    nuUINT	GetRoadCrossTotal();
    nuINT	GetRoadCrossList(NURO_SEARCH_TYPE type, nuWORD nMax, nuBYTE* pBuffer, nuUINT nMaxLen);
    nuBOOL	GetRoadCrossPos(nuUINT nResIdx, NUROPOINT* pos);
    nuWORD	GetRoadCrossTownID(nuUINT nResIdx);
    nuWORD	GetRoadCrossCityID(nuUINT nResIdx);
	nuBOOL  SetNoName(nuBOOL bRes);
   
    nuVOID	ClearSZYInfo();
	nuBOOL	GetRoadSKB(void* pSKBBuf, nuUINT& nCount);  //Prosper 20111226
	nuBOOL	GetRoadWordData(nuINT &nTotal,nuVOID* pBuffer, nuUINT nMaxLen);
	nuBOOL	NAME_Order(nuINT n);
	
	nuBOOL GetNEXTWORD(nuVOID* pKEYBuf, nuUINT& nCount) ;
	nuWCHAR	   m_wsNEXT_WORD_Data[NEXT_WORD_MAXCOUNT];
	nuINT      nNEXT_WORD_COUNT;
	nuBOOL nuWcsFuzzyJudge_O_FORSEARCHROAD(nuWCHAR* pDes, nuWCHAR* pKey, nuUINT desLen, nuUINT keyLen
						  , nuINT& startpos, nuINT& diffpos);
private:
    typedef struct tagSEARCH_MAIN_ROAD  ///主道路信息
    {
        nuWORD tag;     ///主道路在结果中的起始位置
        nuWORD total;   ///子道路数量
    }SEARCH_MAIN_ROAD;
    typedef struct tagSEARCH_SNST_DATA  ///同名收集信息
    {
        nuUINT addr1;   ///目标路名地址
        nuUINT addr2;   ///与目标同名的地址
    }SEARCH_SNST_DATA;
    /**
    * @brief  载入道路首字搜索资料  \n
    *   依赖SZY FILE
    */
    nuUINT	FlushSZYInfo();
    nuVOID	ResetSZSearch();
    /**
    * @brief  道路首字搜索 \n
    *   遍历载入的SZY资料
    */
    nuWORD	SearchRoadSZ();
    /**
    * @brief  载入道路搜索资料  \n
    *   依赖RN RNC FILE
    */
    nuUINT	FlushRNInfo();
	nuUINT	FlushRNInfo2(); //Prosper add by 20130606, for load before input
    nuVOID	ClearRNInfo();
    nuVOID	ResetRNSearch();
    /**
    * @brief  获取道路名称资料  \n
    *   前提：RN 资料被载入  \n
    * @param addr 路名地址
    * @param pBuffer 指针引用 在nMaxLen为0时，将内存中的路名资料地址赋给pBuffer \n
    *   其他情况时 将路名复制到pBuffer指向的地址
    * @param nMaxLen pBuffer的长度，为0时说明需要传出路名资料地址（而非路名内容）
    * @param pStrokes 返回路名笔划数（NULL时不返回）
    * @return 路名长度 nuWcslen()
    */
    nuUINT	GetRoadName(nuUINT addr, nuWORD *&pBuffer, nuUINT nMaxLen=EACH_ROAD_NAME_LEN, nuBYTE *pStrokes=NULL);
    /**
    * @brief  获取道路单音资料  \n
    *   前提：RN资料被载入  由于单音资料处于内存奇地址上 仅处理为返回单音内容\n
    * @param addr 路名地址
    * @param pBuffer 将路名单音内容复制到pBuffer指向的地址
    * @param nMaxLen pBuffer的长度
    * @return 单音长度 nuWcslen()
    */
    nuUINT	GetRoadDanyin(nuUINT addr, nuVOID* pBuffer, nuUINT nMaxLen);
    /**
    * @brief  判断2个路名地址是否同名
    */
    nuBOOL	IsSameRoadName(nuUINT addr1, nuUINT addr2);
    /**
    * @brief  道路搜索                  \n
    *   最小搜索单位 CITY 由m_nCityIDTag标记下一个要搜索的CITYID    \n
    *   依赖文件 同FlushRNInfo()        \n
    *   按照RNC中的道路索引进行遍历     \n
    *   排除同区同名道路 收集到m_SASTDATA中
    * @param tag 需要填充的结果m_SearchRoadRes中的起始位置
    */
    nuUINT	SearchNextRN(nuUINT tag);
	nuUINT	SearchNextRN2(nuUINT tag); //Prosper add by 20130606, for load before input
	  /**
    * @brief  判断2个路名地址是否同主路
    */
    nuBOOL	IsSameMainRoad(nuUINT addr1, nuUINT addr2);
    /**
    * @brief  填充主道路结果    \n
    *   判断出同一主路的内容 填充主路信息至m_MainRoad
    * @param total 搜索出的道路总数
    * @return 主道路数量
    */
    nuUINT	FillMainRoad(nuUINT total);
    /**
    * @brief  同名道路地址搜索  \n
    *   使用在门牌信息搜索时
    * @param pNameAddr 路名资料地址
    * @param nMaxLim 最大搜集数量
    */
    nuUINT	CollectSameRoad(nuUINT *pNameAddr, nuUINT nMaxLim);
    /**
    * @brief  获得道路坐标信息并填充至m_SearchRoadRes中 \n
    *   依赖FlushCrossInfo()                            \n
    *   依赖BH RDW 文件                                 \n
    *   对每个BLOCK，遍历RDW中的A1 A2信息查到到该道路则返回 \n
    *   在setroad之后被调用
    */
    nuBOOL    LoadRoadPos();
    /**
    * @brief  载入道路路口搜索资料  \n
    *   依赖 CT RT BH
    */
    nuUINT	FlushCrossInfo();
    nuVOID	ClearCrossInfo();
    nuVOID	ResetCrossSearch();
    /**
    * @brief  查找blk的idx  \n
    *   依赖 BH中的blk Idx
    */
    nuUINT  BlkIDToIdx(nuUINT nBlkID);
    /**
    * @brief  在一个RTblock中查找道路的交叉路口 
    *   依赖 FlushCrossInfo()、RDW              \n    
    *   1.收集同名同区路名                      \n
    *   2.收集道路在block中的端点     --RDW     \n
    *   ---遍历RT-A3                            \n
    *   1.检查端点连接[端点相连则继续判断]      \n
    *   2.获得道路信息[RT-A1/A2]                \n
    *   3.同名排除                              \n
    *   4.已添加道路排除
    * @param blkID
    * @param tag 结果填充在m_SearchCrossRes的起始位置
    * @return 该block搜索到的交叉路口数量
    */
    nuUINT	SearchRCBlock(nuUINT blkID, nuUINT tag, CMapFileBhY& bhfile, CMapFileRdwY& rdwfile);
    
	nuBOOL RoadNameSort(nuVOID *pWord,nuINT nLens);
	nuBOOL CheckRoadNoNumber(nuVOID *pWord,nuINT nLens);
	nuINT  TransCode(nuINT code);
private:

	//Prosper 2011.12.07
	bool    Sort_KEY_index(nuWORD);
	nuINT 	nKeyCount;
	nuWCHAR	KEY_Data[37];

    CSearchArea *m_pSearchArea;     ///searcharea 通过他获得town blkinfo
    nuWORD m_nMMIdx;                ///search使用的内存块标志
    nuUINT m_nMMMaxLen;             ///search使用的内存块长度
    nuUINT m_nMMCTLen;              ///加载map时读取的CITYTOWN信息，始终放在内存区最开始的位置
    nuUINT	m_nSzyMin;              ///设定的首字最小笔划
    nuUINT	m_nSzyMax;              ///设定的首字最大笔划
    nuWORD  m_nSzyZhuyin;           ///设定的首字注音
    nuUINT	m_nSzyMinT;             ///标记首字搜索结果依赖的首字最小笔划
    nuUINT	m_nSzyMaxT;             ///标记首字搜索结果依赖的首字最大笔划
    nuWORD  m_nSzyZhuyinT;          ///标记首字搜索结果依赖的首字注音
    nuUINT	m_nSzyResTotal;         ///首字首字搜索结果总数
    nuUINT	m_nMMSzyLen;            ///读取的首字搜索资料长度
    nuWORD	m_nSzyLoadNum;          ///读取的首字数量
    nuLONG	m_nSzySearchMapID;      ///标记首字搜索结果依赖的mapid
    nuWORD	m_nSzySearchCityID;     ///标记首字搜索结果依赖的cityid
    nuWORD	m_nSzySearchTownID;     ///标记首字搜索结果依赖的townid
    nuUINT	m_nSzySearchTag;        ///标记UI对结果记录集中的读取点
    nuUINT	m_nSzySearchLastCount;  ///上一次UI获取首字结果数量
    nuWORD	m_nSzyWordbuffer[NURO_SEARCH_BUFFER_MAX_ROADSZ];    ///保存搜索出的首字 
    nuWORD	m_nSzyWord;             ///设定的首字
    nuWORD	m_nSzyWordT;            ///标记道路搜索结果依赖的首字
    nuWORD  m_nSearchRoadKeyDanyin[EACH_ROAD_NAME_LEN>>1];      ///设定的道路keydanyin
    nuWORD  m_nSearchRoadKeyDanyinT[EACH_ROAD_NAME_LEN>>1];     ///标记道路搜索结果依赖的keydanyin
    nuWORD  m_nSearchRoadKeyName[EACH_ROAD_NAME_LEN>>1];        ///设定的道路keyname
    nuWORD  m_nSearchRoadKeyNameT[EACH_ROAD_NAME_LEN>>1];       ///标记道路搜索结果依赖的keyname
    nuWORD	m_nCityIDTag;           ///标记要搜索的cityid
    nuWORD	m_nCityIDMax;           ///标记要搜索的最大的cityid
    nuUINT	m_nMMRNCLen;            ///读取的RNC FILE资料长度
    nuUINT	m_nMMRNLen;             ///读取的RN FILE资料长度    
    nuUINT	m_nResTotal;            ///搜索到的道路总数
    nuLONG	m_nRNCSearchMapID;      ///标记道路搜索结果依赖的mapid
    nuWORD	m_nRNCSearchCityID;     ///标记道路搜索结果依赖的cityid
    nuWORD	m_nRNCSearchTownID;     ///标记道路搜索结果依赖的townid
    nuUINT	m_nRNCSearchTag;        ///标记UI对道路结果的读取点
    nuUINT	m_nRNCSearchLastCount;  ///上一次UI读取道路结果数量
    nuUINT	m_nMRSearchTag;         ///标记UI对主道路记录的读取点
    nuUINT	m_nMRSearchLastCount;   ///上一次UI获取主道路结果数量
    nuUINT	m_nMainRoadTotal;       ///搜索到的主道路总数
    nuUINT	m_nSearchRoadNameAddr;  ///设定的道路名地址
    nuWORD	m_nSearchRoadCityID;    ///设定的道路的cityid
    nuWORD	m_nSearchRoadTownID;    ///设定的道路的townid
    nuWORD	m_nSearchRoadIdx;       ///设定的道路在结果集中的索引
    SEARCH_PDN_NODE_R m_nSearchRDN; ///设定的道路的门牌信息概要
    nuWORD	m_nSearchRoadDoorNum;   ///设定的道路的门牌号数量
    nuUINT	m_nSearchRoadDoorPos;   ///设定的道路的门牌信息在PDN文件的地址
    nuUINT	m_nRC_RTLen;            ///读取的RT档资料长度 -搜索CROSS
    nuUINT  m_nTownBlkInfoLen;      ///town的blkinfo长度
    nuUINT  m_nBHTotal;             ///blk总数
    nuUINT  m_nBHIdxLen;            ///读取的blkidx长度
    nuUINT	m_RCTotal;              ///搜索到的交叉路口总数
    nuLONG	m_nRCSearchMapID;       ///标记交叉路口结果依赖的mapid
    nuWORD	m_nRCSearchCityID;      ///标记交叉路口结果依赖的cityid
    nuWORD	m_nRCSearchTownID;      ///标记交叉路口结果依赖的townid
    nuUINT	m_nRCSearchNameAddr;    ///标记交叉路口结果依赖的nameaddr
    nuWORD  m_nRCSearchTag;         ///标记UI对交叉路口的读取点
    nuWORD  m_nRCSearchLastCount;   ///上一次UI获得交叉路口信息的数量
    nuUINT	m_nBLIdx;               ///标记TOWN BLK INFO的第1索引
    nuUINT	m_nBLIdxM;              ///标记TOWN BLK INFO的第2索引 与第1索引一起标记当前搜索的blk ID
    nuINT     m_nSASTDATA;            ///m_SASTDATA的实际收集数量
	nuBOOL    m_bIsFromBeging;        ///眖秨﹍ゑ癸
    SEARCH_SNST_DATA m_SASTDATA[_SNST_COL_LIMIT_];  ///同town 同name 不同nameaddr信息记录
    SEARCH_BUFFER_ROAD m_SearchRoadRes[NURO_SEARCH_BUFFER_MAX_ROAD];    ///道路搜索结果

#ifdef _SEARCH_ROAD_MAINROADMODE_
    SEARCH_MAIN_ROAD m_MainRoad[NURO_SEARCH_BUFFER_MAX_ROAD];           ///主道路搜索结果集
#endif
    SEARCH_BUFFER_ROAD m_SearchCrossRes[NURO_SEARCH_BUFFER_MAX_CROSS];  ///交叉路口结果

    //for searchDanyin extend
public:
    nuUINT GetRoadKeyDanyinMatchWords(nuWORD** buffer);
private:
    nuWORD      m_danYinMatchWordsTag[EACH_ROAD_NAME_LEN>>1];
    nuroMapArea m_danYinMatchWordsAreaTag;
    nuUINT      m_danYinMatchWordsCount;
    nuWORD      m_danYinMatchWordsBuffer[16*200];
	nuBOOL		m_bKickNumber ;
	nuINT		m_nNameIdx;
	nuINT		m_nDYWordIdx;
	nuINT		nSortLimt;
	
	typedef struct tag_ListName				//Unicode 锣 Big5 ┮惠挡篶 
	{
		nuWCHAR wsListName2[EACH_ROAD_NAME_LEN>>1];
	}ListName;

	
	typedef struct tag_Code				//Unicode 锣 Big5 ┮惠挡篶 
	{

		nuINT	nBig;
		nuINT	nUniCode;    

	}Trans_DATA;
	Trans_DATA *data;
	nuBOOL m_bDYData;
	nuBOOL m_bSort;
	nuWORD m_nListNameIdx;
	nuWORD m_nSearchResIdx;
	nuWORD m_nTownIdx;


	nuINT  m_nTempMap;
	nuINT  m_nTempCity;
	nuBOOL bLoadAfter;
	nuWORD m_nTownInfoMemIdx;
};
#endif // !defined(AFX_SEARCHROAD_H__69340EF9_C168_4655_A955_5629DF66D5DA__INCLUDED_)
