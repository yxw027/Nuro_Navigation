/******************************************
* @file libSearchNew.h
*
* search lib 导出函数
*******************************************/
#ifndef	__NURO_NEW_SEARCH_20081001
#define	__NURO_NEW_SEARCH_20081001

#include "NuroDefine.h"
#include "NuroModuleApiStruct.h"
#include "nuSearchDefine.h"

#ifdef _USRDLL

#ifdef SEARCHNEW_EXPORTS
#define SEARCHNEW_API extern "C" __declspec(dllexport)
#else
#define SEARCHNEW_API extern "C" __declspec(dllimport)
#endif

#else
#define SEARCHNEW_API extern "C"
#endif

/**
* @brief  初始化依赖数据
*
* @param pApiAddr 包含各模块API
* @return 返回值表示是否初始化成功 \n
*	true: 成功\n
*	false: 失败\n
*/
SEARCHNEW_API 
nuBOOL LibSH_InitSearchZ(APISTRUCTADDR* pApiAddr);

/**
* @brief 终止模块
*/
SEARCHNEW_API 
nuVOID LibSH_FreeSearchZ();
/**
* @brief  启动sarch
*
* @param nMMIdx 内存地址-未使用此参数
* @param nMMLen 内存大小
* @return 返回值表示是否初始化成功 \n
*	true: 成功\n
*	false: 失败\n
* 
*　启动search 分配内存
*/
SEARCHNEW_API nuBOOL LibSH_StartSearch(nuWORD nMMIdx, nuUINT nMMLen);
/**
* @brief  终止search （包含终止danyin搜索模块）
* 
*/
SEARCHNEW_API nuVOID LibSH_EndSearch();
/**
* @brief  获取mapinfo
* 获得BH HEAD包含的mapinfo
*/
SEARCHNEW_API nuBOOL LibSH_GetMapInfo(nuLONG mapid, SEARCH_MAP_INFO &mapinfo);
/**
* @brief  设置载入的地图ID
* @param resIdx 地图ID 依赖LibSH_GetMapNameAll
* 将载入CBINFO-city town信息 
* 默认启动search时 将setmapip(0)
*/
SEARCHNEW_API nuBOOL LibSH_SetMapId(nuLONG resIdx);
/**
* @brief  设置搜索的cityid
* @param resIdx cityid 依赖LibSH_GetCityNameAll
*/
SEARCHNEW_API nuBOOL LibSH_SetCityId(nuWORD resIdx);

/**
* @brief  设置搜索的townid
* @param resIdx townid 依赖LibSH_GetTownNameAll
*/
SEARCHNEW_API nuBOOL LibSH_SetTownId(nuWORD resIdx);
/**
* @brief  设置搜索poi的类型1 id
* @param resIdx 由LibSH_GetPoiTP1All生成
*/
SEARCHNEW_API nuBOOL LibSH_SetPoiTP1(nuUINT resIdx); 

/**
* @brief  设置搜索poi的类型2 id
* @param resIdx 依赖LibSH_GetPoiTP2All
*/
SEARCHNEW_API nuBOOL LibSH_SetPoiTP2(nuUINT resIdx);
/**
* @brief  设置搜索poi的keydanyin
* @param pDanyin 单音搜索关键字字符串 nuWCHAR
*/
SEARCHNEW_API nuBOOL LibSH_SetPoiKeyDanyin(nuWCHAR *pDanyin,nuBOOL bComplete);
/**
* @brief  设置搜索poi的keyname
* @param pName 名称搜索关键字字符串 nuWCHAR
*/
SEARCHNEW_API nuBOOL LibSH_SetPoiKeyName(nuWCHAR *pName, nuBOOL bFuzzy);
/**
* @brief  设置搜索poi的phonen
* @param pPhone 电话搜索关键字字符串 nuWCHAR
*/
SEARCHNEW_API nuBOOL LibSH_SetPoiKeyPhone(nuWCHAR *pPhone);
/**
* @brief  设置搜索poi的区域范围
* @param pRect poi搜索范围
*/
SEARCHNEW_API nuBOOL LibSH_SetSearchArea(NURORECT *pRect);
/**
* @brief  设置搜索道路首字的笔划数范围
* @param nMin 最小值
* @param nMax 最大值
* [nMin, nMax]闭区间范围
*/
SEARCHNEW_API nuBOOL LibSH_SetRoadSZSLimit(nuUINT nMin, nuUINT nMax);
/**
* @brief  设置搜索道路首字注音
* @param zhuyin 设置的首字注音 nuWCHAR
*/
SEARCHNEW_API nuBOOL LibSH_SetRoadSZZhuyin(nuWORD zhuyin);
/**
* @brief  设置道路搜索的首字
* @param nResIdx 依赖LibSH_GetRoadSZList
*/
SEARCHNEW_API nuBOOL LibSH_SetRoadSZ(nuUINT nResIdx);
/**
* @brief  设置道路搜索的keydanyin
* @param pKeyWord 道路搜索单音关键字字符串 nuWCHAR
*/
SEARCHNEW_API nuBOOL LibSH_SetRoadKeyDanyin(nuWORD *pKeyWord);
/**
* @brief  设置道路搜索的keyname
* @param pKeyWord 道路搜索名称关键字字符串 nuWCHAR
*/
SEARCHNEW_API nuBOOL LibSH_SetRoadKeyName(nuWORD *pKeyWord, nuBOOL bIsFromBeging = false);
/**
* @brief  设置道路搜索索引- 为进一步获取道路信息的前提【获取交叉路口等】\n
*           主道路模式下使用LibSH_SetRoadName_MR
* @param nResIdx 道路索引 \n
*       依赖获取道路名的操作【LibSH_GetRoadNameList等】
* @param bRealIdx 是否真实索引  \n
*       若为true表明nResIdx的依赖中 携带了道路在内部结果中的真实索引 【如LibSH_GetFilterResAll】
*/
SEARCHNEW_API nuBOOL LibSH_SetRoadName(nuUINT nResIdx, nuBOOL bRealIdx);
/**
* @brief  设置笔划焦点
* @param nResIdx 依赖GETNAMELIST类似操作
*   将返回的namelist中的一个idx值作为笔划焦点 实现笔划上、笔划下的前提 
*/
SEARCHNEW_API nuVOID LibSH_SetStrokeLimIdx(nuUINT nResIdx);


//prosper 05
SEARCHNEW_API nuVOID LibSH_Stroke_GetNextWordData(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* pCady, const nuUINT candylen, const nuBOOL ReSort);
SEARCHNEW_API nuVOID LibSH_ZhuYinForSmart_SecondWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont, const nuUINT candylen, const nuBOOL ReSort);
SEARCHNEW_API nuBOOL LibSH_ZhuYinForSmart_OtherWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort);

SEARCHNEW_API nuVOID LibSH_Stroke_Poi_GetNextWordData(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID* pCady, const nuUINT candylen, const nuBOOL ReSort);
SEARCHNEW_API nuVOID LibSH_ZhuYinForSmart_Poi_SecondWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont, const nuUINT candylen, const nuBOOL ReSort);
SEARCHNEW_API nuBOOL LibSH_ZhuYinForSmart_Poi_OtherWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& WordCont,nuVOID *candi, const nuUINT candylen, const nuBOOL ReSort);

SEARCHNEW_API nuINT  LibSH_Smart_GetPoiList(NURO_SEARCH_TYPE st, nuWORD nMaxPoi, nuVOID* pBuffer, nuUINT nMaxLen,nuUINT word);

SEARCHNEW_API nuINT  LibSH_GetPoiPZList(NURO_SEARCH_TYPE type, nuWORD nMax, nuVOID* pBuffer, nuUINT nMaxLen);
SEARCHNEW_API nuUINT LibSH_GetPoiPZTotal();

SEARCHNEW_API nuBOOL LibSH_SetPoiPZZhuyin(nuWORD zhuyin);
SEARCHNEW_API nuBOOL LibSH_SetPoiPZ(nuUINT nResIdx);

SEARCHNEW_API nuBOOL LibSH_SetPoiKeyNameP(nuWCHAR *pName, nuBOOL bFuzzy);

/**
* @brief  获得汽车服务厂商
* @param pBuffer 名字缓存空间
* @param nMaxLen 空间长度
*/
SEARCHNEW_API nuUINT LibSH_GetPoiCarFacAll(nuVOID *pBuffer, nuUINT nMaxLen);
/**
* @brief  设置搜索POI的汽车服务厂商类型
* @param nResIdx 依赖 LibSH_GetPoiCarFacAll
*/
SEARCHNEW_API nuBOOL LibSH_SetPoiCarFac(nuUINT nResIdx);
/**
* @brief  获得汽车SPI类型
* @param pBuffer 名字缓存空间
* @param nMaxLen 空间长度
*/
SEARCHNEW_API nuUINT LibSH_GetPoiSPTypeAll(nuVOID *pBuffer, nuUINT nMaxLen);
/**
* @brief  设置搜索POI的SPI类型
* @param nResIdx 依赖 LibSH_GetPoiSPTypeAll
*/
SEARCHNEW_API nuBOOL LibSH_SetPoiSPType(nuUINT nResIdx);
/**
* @brief  次字搜集
* @param pHeadWords 前置字集
* @param pBuffer 缓存空间
* @nMaxWords 空间可以保存的nuWCHAR数
*/
SEARCHNEW_API nuUINT LibSH_CollectNextWordAll(nuWORD* pHeadWords, 
                                             nuWORD *pBuffer, nuUINT nMaxWords);
/**
* @brief  获得当前设置地图下的city数量总数
*/
SEARCHNEW_API nuUINT LibSH_GetCityTotal();
/**
* @brief  获得当前设置地图-city下的town数量总数
*/
SEARCHNEW_API nuUINT LibSH_GetTownTotal();
/**
* @brief  获得符合搜索条件的POI总数
*/
SEARCHNEW_API nuUINT LibSH_GetPoiTotal();

SEARCHNEW_API nuUINT LibSH_GetPoiCount(nuUINT);//prosper

SEARCHNEW_API nuUINT LibSH_GetPoiTotal_Word(nuUINT word);//prosper
/**
* @brief  获得当前设置地图下的TP1类型总数
*/
SEARCHNEW_API nuUINT LibSH_GetTP1Total();
/**
* @brief  获得符合首字搜索条件的首字数量
*/
SEARCHNEW_API nuUINT LibSH_GetRoadSZTotal();
/**
* @brief  获得符合搜索条件的道路数量 \n
*       使用主道路模式时见 LibSH_GetRoadTotal_MR
*/
SEARCHNEW_API nuUINT LibSH_GetRoadTotal();
/**
* @brief  获得符合搜索条件的道路的交叉路口总数 
*/
SEARCHNEW_API nuUINT LibSH_GetRoadCrossTotal();
/**
* @brief  获得所有的地图名字
* @param pBuffer 保存名字的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuINT LibSH_GetMapNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
/**
* @brief  获得设定的地图的city名字
* @param pBuffer 保存名字的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuINT LibSH_GetCityNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
/**
* @brief  获得设定的地图-city的town名字
* @param pBuffer 保存名字的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuINT LibSH_GetTownNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
/**
* @brief  获得设定的地图的poi-类型1的名字
* @param pBuffer 保存名字的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuINT  LibSH_GetPoiTP1All(nuVOID *pBuffer, nuUINT nMaxLen);
/**
* @brief  获得设定的地图-TP1的poi-类型2的名字
* @param pBuffer 保存名字的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuINT  LibSH_GetPoiTP2All(nuVOID* pBuffer, nuUINT nMaxLen);
/**
* @brief  获得符合搜索条件的POI结果
* @param st 获取类型
* @param nMaxPoi 最多返回数量
* @param pBuffer 保存结果的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuINT  LibSH_GetPoiList(NURO_SEARCH_TYPE st, nuWORD nMaxPoi, 
                                    nuVOID* pBuffer, nuUINT nMaxLen);
/**
* @brief  获得POI的附加信息
* @param nPoiListIdx 依赖 LibSH_GetPoiList
* @param pPaInfo 保存SEARCH_PA_INFO的内存
* @param bRealIdx nPoiListIdx是否为内部索引值
*/
SEARCHNEW_API nuBOOL LibSH_GetPoiMoreInfo(nuUINT nPoiListIdx, 
                                        SEARCH_PA_INFO *pPaInfo, nuBOOL bRealIdx);
/**
* @brief  获得POI的坐标信息
* @param nPoiListIdx 依赖 LibSH_GetPoiList
* @param pos 保存pos的内存
* @param bRealIdx nPoiListIdx是否为内部索引值
*/
SEARCHNEW_API nuBOOL LibSH_GetPoiPos(nuUINT nPoiListIdx, NUROPOINT* pos, nuBOOL bRealIdx);
/**
* @brief  获得POI的city名字
* @param nResIdx 依赖 LibSH_GetPoiList
* @param pBuffer 保存名称的内存
* @param nMaxLen 内存长度
* @param bRealIdx nPoiListIdx是否为内部索引值
*/
SEARCHNEW_API nuBOOL LibSH_GetPoiCityName(nuUINT nResIdx, nuWORD *pBuffer, 
                                        nuUINT nMaxLen, nuBOOL bRealIdx);

SEARCHNEW_API nuLONG LibSH_GetPoiCityID(nuUINT nResIdx, nuBOOL bRealIdx); // Added by Damon 20100309

/**
* @brief  获得POI的town名字
* @param nResIdx 依赖 LibSH_GetPoiList
* @param pBuffer 保存名称的内存
* @param nMaxLen 内存长度
* @param bRealIdx nPoiListIdx是否为内部索引值
*/
SEARCHNEW_API nuBOOL LibSH_GetPoiTownName(nuUINT nResIdx, nuWORD *pBuffer, 
                                        nuUINT nMaxLen, nuBOOL bRealIdx);
/**
* @brief  通过电话精确搜索POI
* @param pPhone 电话信息
* @param pInfo 保存SEARCH_POIINFO的内存
*/
SEARCHNEW_API nuBOOL LibSH_GetPoiByPhone(const nuWCHAR *pPhone, SEARCH_POIINFO *pInfo);
/**
* @brief  获得符合条件的道路首字
* @param type 获取类型
* @param nMax 最多返回数量
* @param pBuffer 保存结果的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuINT  LibSH_GetRoadSZList(NURO_SEARCH_TYPE type, nuWORD nMax, 
                                       nuVOID* pBuffer, nuUINT nMaxLen);
/**
* @brief  获得符合条件的道路结果
* @param type 获取类型
* @param nMax 最多返回数量
* @param pBuffer 保存结果的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuINT  LibSH_GetRoadNameList(NURO_SEARCH_TYPE type, nuWORD nMax, 
                                         nuVOID* pBuffer, nuUINT nMaxLen);
/**
* @brief  获得符合条件的道路路口结果 依赖 LibSH_SetRoadName
* @param type 获取类型
* @param nMax 最多返回数量
* @param pBuffer 保存结果的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuINT  LibSH_GetRoadCrossList(NURO_SEARCH_TYPE type, nuWORD nMax, 
                                          nuVOID* pBuffer, nuUINT nMaxLen);
/**
* @brief  获得指定道路的门牌号总数 依赖 LibSH_SetRoadName
*/
SEARCHNEW_API nuUINT LibSH_GetRoadDoorTotal();
/**
* @brief  获得指定道路的门牌号信息 依赖 LibSH_SetRoadName
* @param pBuffer 保持结果的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuBOOL LibSH_GetRoadDoorInfo(nuVOID *pBuffer, nuUINT nMaxLen);
/**
* @brief  获得交叉路口的坐标
* @param nResIdx 依赖LibSH_GetRoadCrossList
* @param pos 保持坐标结果
*/
SEARCHNEW_API nuBOOL LibSH_GetRoadCrossPos(nuUINT nResIdx, NUROPOINT* pos);
/**
* @brief  获得交叉路口的town名字 
* @param nResIdx 依赖LibSH_GetRoadCrossList
* @param pBuffer 保持名字的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuBOOL LibSH_GetRoadCrossTownName(nuUINT nResIdx, nuWORD *pBuffer, 
                                              nuUINT nMaxLen);
/**
* @brief  获得交叉路口的city名字
* @param nResIdx 依赖LibSH_GetRoadCrossList
* @param pBuffer 保持名字的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuBOOL LibSH_GetRoadCrossCityName(nuUINT nResIdx, nuWORD *pBuffer, 
                                              nuUINT nMaxLen);
/**
* @brief  获得指定道路的坐标信息
* @param pos 保持结果的内存
*/
SEARCHNEW_API nuBOOL LibSH_GetRoadPos(NUROPOINT* pos);
/**
* @brief  获得指定道路的town名字 依赖 LibSH_SetRoadName
* @param pBuffer 保持名字的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuBOOL LibSH_GetRoadTownName(nuWORD *pBuffer, nuUINT nMaxLen);
/**
* @brief  获得指定道路的city名字 依赖 LibSH_SetRoadName
* @param pBuffer 保持名字的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuBOOL LibSH_GetRoadCityName(nuWORD *pBuffer, nuUINT nMaxLen);
/**
* @brief  获得符合搜索条件的道路数量-使用主道路模式
*/
SEARCHNEW_API nuUINT LibSH_GetRoadTotal_MR();
/**
* @brief  获得指定主道路的子道路数量
* @param nMaxinRoadIdx 依赖 LibSH_GetRoadNameList_MR等获取的主道路namelist
* @param bRealIdx nMaxinRoadIdx是否为内部的索引
*/
SEARCHNEW_API nuUINT LibSH_GetChildRoadTotal_MR(nuUINT nMainRoadIdx, nuBOOL bRealIdx);
/**
* @brief  获得符合条件的道路结果
* @param type 获取类型
* @param nMax 最多返回数量
* @param pBuffer 保存结果的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuINT LibSH_GetRoadNameList_MR(NURO_SEARCH_TYPE type, nuWORD nMax, 
                                           nuVOID* pBuffer, nuUINT nMaxLen);
/**
* @brief  获得指定主道路的子道路
* @param nMainRoadIdx 依赖 LibSH_GetRoadNameList_MR等获取的主道路namelist
* @param pBuffer 保存结果的内存
* @param nMaxLen 内存长度
* @param bRealIdx nMaxinRoadIdx是否为内部的索引
*/
SEARCHNEW_API nuINT LibSH_GetRoadNameALL_MR(nuUINT nMainRoadIdx, nuVOID* pBuffer, 
                                          nuUINT nMaxLen, nuBOOL bRealIdx);
/**
* @brief  设置道路搜索索引- 为进一步获取道路信息的前提【获取交叉路口等】
* @param nMainRoadIdx 道路索引 \n
*       依赖获取主道路名的操作【LibSH_GetRoadNameList_MR等】
* @param nRoadIdx 子道路索引 \n
*       依赖获取子道路名的操作【LibSH_GetRoadNameALL_MR等】
* @param bRealIdx 是否真实索引  \n
*       若为true表明nResIdx的依赖中 携带了道路在内部结果中的真实索引 【如LibSH_GetFilterResAll】
*/
SEARCHNEW_API nuBOOL LibSH_SetRoadName_MR(nuUINT nMainRoadIdx, nuUINT nRoadIdx, nuBOOL bRealIdx);

/**
* @brief  获取按名字过滤结果
* @param pKey 名字关键字
* @param pBuffer 保存结果的内存
* @param nMaxLen 内存长度
*/
SEARCHNEW_API nuINT   LibSH_GetFilterResAll(const nuWCHAR *pKey, nuVOID *pBuffer, nuUINT nMaxLen);
/**
* @brief  启动SKB搜索
*   必须在启动SEARCH ROAD/POI后使用 会清除SEARCH ROAD/POI中所有的搜索结果
*/
SEARCHNEW_API nuBOOL  LibSH_StartSKBSearch(nuUINT nType);
/**
* @brief  终止SKB搜索
*   清除SKB搜索结果
*/
SEARCHNEW_API nuVOID  LibSH_StopSKBSearch();
/**
* @brief 获得首拼音查询列表
*   内存分配在road搜索的相关资料前 CT之后
* @param keyStr 传入拼音列表
* @param pBuffer 查询结果列表
* @param nMaxLen pBuffer长度
* @param resCount 结果涉及的道路总数
* @return 获得的拼音数量
*/
SEARCHNEW_API nuINT   LibSH_GetSKBAll(nuCHAR* keyStr, nuCHAR* pBuffer, nuINT nMaxLen, nuINT* resCount);

SEARCHNEW_API nuUINT LibSH_SHDYStart();
SEARCHNEW_API nuUINT LibSH_SHDYStop();
SEARCHNEW_API nuUINT LibSH_SHDYSetRegion(nuBYTE nSetMode, nuDWORD nData);
SEARCHNEW_API nuUINT LibSH_SHDYSetDanyin(nuWCHAR *pWsDy, nuBYTE nWsNum);
SEARCHNEW_API nuUINT LibSH_SHDYGetHead(PDYCOMMONFORUI pDyHead);
SEARCHNEW_API nuUINT LibSH_SHDYGetPageData(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx);
SEARCHNEW_API nuUINT LibSH_SHDYGetPageStroke(PDYDATAFORUI pDyDataForUI, nuBYTE nStroke);
SEARCHNEW_API nuUINT LibSH_SHDYGetOneDetail(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
SEARCHNEW_API nuUINT LibSH_SHDYCityTownName(PDYCITYTOWNNAME pDyCTname, nuLONG nCityID);
SEARCHNEW_API nuUINT LibSH_SHDYSetSearchMode(nuINT nMode);

//@Prosper 2012.12
SEARCHNEW_API nuUINT LibSH_SHDYGetWordData(PDYWORDFORUI pDyDataForUI);
SEARCHNEW_API nuUINT LibSH_SHDYSetDanyinKeyname(nuWCHAR *pWsDy, nuBYTE nWsNum);
SEARCHNEW_API nuUINT LibSH_SHDYGetPageData_WORD(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx);

SEARCHNEW_API nuUINT LibSH_SHDYSetKey(nuWCHAR *pWsDy, nuBYTE nWsNum);

//@zhikun 2009.07.13
SEARCHNEW_API nuUINT LibSH_ShBeginZ(nuUINT nType);
SEARCHNEW_API nuUINT LibSH_ShEndZ();
//@for search by the pinyin file 
/*
SEARCHNEW_API nuUINT LibSH_PYSetModeZ(nuUINT nMode);
SEARCHNEW_API nuUINT LibSH_PYSetRegionZ(nuBYTE nSetMode, nuDWORD nData);
SEARCHNEW_API nuUINT LibSH_PYSetStringZ(nuCHAR *pBuff, nuWORD nBuffLen);
SEARCHNEW_API nuUINT LibSH_PYGetHeadZ(PSH_HEADER_FORUI pShHeader);
SEARCHNEW_API nuUINT LibSH_PYGetPageDataZ(PSH_PAGE_FORUI pPageData, nuDWORD nItemIdx);
SEARCHNEW_API nuUINT LibSH_PYGetPagePinyinZ(PSH_PAGE_FORUI pPageData, nuBYTE nPinyin);
SEARCHNEW_API nuUINT LibSH_PYGetOneDetailZ(PSH_DETAIL_FORUI pOneDetail, nuDWORD nIdx);
SEARCHNEW_API nuUINT LibSH_PYCityTownNameZ(PSH_CITYTOWN_FORUI pCityTown);
*/
//@for the special door numbers. @zhikun 2009.07.27
SEARCHNEW_API nuUINT LibSH_ShSetModeZ(nuUINT nShMode);
SEARCHNEW_API nuUINT LibSH_ShSetInput(nuUINT nInputType);
SEARCHNEW_API nuUINT LibSH_ShSetRegionZ(nuBYTE nSetMode, nuDWORD nData);
SEARCHNEW_API nuUINT LibSH_ShSetStringZ(nuCHAR *pBuff, nuWORD nBuffLen);
SEARCHNEW_API nuUINT LibSH_ShGetHeadZ(PSH_HEADER_FORUI pShHeader);
SEARCHNEW_API nuUINT LibSH_ShGetPageDataZ(PSH_PAGE_FORUI pPageData, nuDWORD nItemIdx);
SEARCHNEW_API nuUINT LibSH_ShGetPagePinyinZ(PSH_PAGE_FORUI pPageData, nuBYTE nPinyin);
SEARCHNEW_API nuUINT LibSH_ShGetOneDetailZ(PSH_DETAIL_FORUI pOneDetail, nuDWORD nIdx);
SEARCHNEW_API nuUINT LibSH_ShCityTownNameZ(PSH_CITYTOWN_FORUI pCityTown);
SEARCHNEW_API nuUINT LibSH_ShGetRoadDetailZ(PSH_ROAD_DETAIL pOneRoad, nuDWORD nIdx);
SEARCHNEW_API nuUINT LibSH_ShGetDrNumCoorZ(nuDWORD nIdx, nuDWORD nDoorNum, nuroPOINT* point);
SEARCHNEW_API nuINT  LibSH_ShGetList(nuWORD& nMax,nuVOID* pBuffer, nuUINT nMaxLen);
SEARCHNEW_API nuINT  LibSH_ShGetNextWords(nuVOID* pRoadBuf, const nuUINT RoadBufLen,nuVOID* pWordBuf,const nuUINT WordLen,nuUINT& n_iCount,nuVOID *wKeyWord, const nuUINT candylen, const nuBOOL ReSort);
SEARCHNEW_API nuUINT LibSH_ShGetWordData(PDYWORDFORUI pDyData);
#ifndef VALUE_EMGRT
SEARCHNEW_API nuBOOL LibSH_ShGetDoorInfo(SH_ROAD_DETAIL &data);
SEARCHNEW_API nuINT LibSH_ShGetDoorInfoData(nuVOID* pBuffer);
#endif 

SEARCHNEW_API nuBOOL LibSH_LDCreateDataFile(nuTCHAR *pFileName
                                          , nuUINT nMax, nuUINT nEachRecordLen, nuINT mode);
SEARCHNEW_API nuBOOL LibSH_LDOpenFile(nuTCHAR *pFileName);
SEARCHNEW_API nuVOID LibSH_LDCloseFile();
SEARCHNEW_API nuBOOL LibSH_LDGetFileHeader(LOGDATAFILEHEADER *fileHeader);
SEARCHNEW_API nuBOOL LibSH_LDAddRecord(nuVOID *pBuffer, nuUINT nLen);
SEARCHNEW_API nuVOID LibSH_LDDelRecord(nuUINT idx);
SEARCHNEW_API nuVOID LibSH_LDClearRecord();
SEARCHNEW_API nuUINT LibSH_LDGetRecord(nuUINT sIdx, nuUINT nMax
                                       , nuVOID *pBuffer, nuUINT nBufferLen);

SEARCHNEW_API nuUINT LibSH_GetPoiNearDis(nuUINT nResIdx, 
										 nuBOOL   bRealIdx);

SEARCHNEW_API nuINT LibSH_GetRoadTownID();

/*
	//	Prosper Test

	SEARCHNEW_API nuBOOL LibSH_VoiceRoadSearch();
	SEARCHNEW_API nuBOOL LibSH_VoicePoiSearch();

*/

	
SEARCHNEW_API nuBOOL LibSH_VoiceRoadName(VOICEROADFORUI data,nuVOID *wChangeName,nuVOID *wCutName,nuroPOINT &point);

SEARCHNEW_API nuBOOL LibSH_VoicePoiSetMap(nuINT nIdx);
SEARCHNEW_API nuBOOL LibSH_VoicePoiPosName(nuVOID* pCityName,nuVOID* pTownName, nuDWORD nIdx);
SEARCHNEW_API nuBOOL LibSH_VoicePoiKeySearch(PDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
SEARCHNEW_API nuBOOL LibSH_VoicePoiEnd();

SEARCHNEW_API nuBOOL LibSH_SHSetNoName(nuBOOL bRes);

//Prosper ADD SKB ,20130103 
SEARCHNEW_API nuBOOL LibSH_GetRoadSKB(nuVOID* pSKBBuf, nuUINT& WordCont);
SEARCHNEW_API nuBOOL LibSH_GetPoiSKB(nuVOID* pSKBBuf, nuUINT& WordCont);

SEARCHNEW_API nuBOOL LibSH_GetRoadWordData(nuINT &nTotal,nuVOID* pBuffer, nuUINT nMaxLen);
SEARCHNEW_API nuUINT LibSH_SHDYSetDanyin2(nuWCHAR *pWsDy, nuBYTE nWsNum);
SEARCHNEW_API nuBOOL LibSH_SetRoadKeyDanyin2(nuWORD *pKeyWord);
SEARCHNEW_API nuBOOL LibSH_SetPoiKeyDanyin2(nuWCHAR *pDanyin,nuBOOL bComplete);

SEARCHNEW_API nuINT LibSH_GetDYPoiTotal();
#ifdef ZENRIN
SEARCHNEW_API nuINT LibSH_GetDYDataCOUNT();
#endif
SEARCHNEW_API nuBOOL LibSH_NEWSEARCH_SDK(PCALL_BACK_PARAM pParam);

#endif
SEARCHNEW_API nuINT   LibSH_GetNEXTAll(nuVOID* pkeyStr, nuUINT& WordCont);
SEARCHNEW_API nuUINT LibSH_SHNewDYStart();
SEARCHNEW_API nuUINT LibSH_SHNewDYStop();
SEARCHNEW_API nuUINT LibSH_SHNewDYSetSearchMode(nuINT nMode);
SEARCHNEW_API nuUINT LibSH_SHNewDYSetRegion(nuBYTE nSetMode, nuDWORD nData);

SEARCHNEW_API nuUINT LibSH_SHNewDYSetDanyin(nuWCHAR *pWsDy, nuBYTE nWsNum);
SEARCHNEW_API nuUINT LibSH_SHNewDYSetDanyin2(nuWCHAR *pWsDy, nuBYTE nWsNum);
SEARCHNEW_API nuUINT LibSH_SHNewDYGetHead(PDYCOMMONFORUI pDyHead);
SEARCHNEW_API nuUINT LibSH_SHNewDYGetWordData(PDYWORDFORUI pDyDataForUI);
SEARCHNEW_API nuUINT LibSH_SHNewDYSetDanyinKeyname(nuWCHAR *pWsDy, nuBYTE nWsNum);
SEARCHNEW_API nuUINT LibSH_SHNewDYSetKey(nuWCHAR *pWsDy, nuBYTE nWsNum,nuBOOL);
SEARCHNEW_API nuUINT LibSH_SHNewDYGetPageData_WORD(PDYDATAFORUI pDyDataForUI, nuDWORD nItemIdx);
SEARCHNEW_API nuINT LibSH_GetNewDYDataCOUNT();
SEARCHNEW_API nuINT LibSH_GetNewDYPoiTotal();
SEARCHNEW_API nuUINT LibSH_SHNewDYGetOneDetail(PNDYDETAILFORUI pDyDetailForUI, nuDWORD nIdx);
SEARCHNEW_API nuBOOL LibSH_GetNewDYGetALLData(nuPVOID buffer,nuINT &nCount);
SEARCHNEW_API nuUINT LibSH_SHNewDYGetPoiTotal();
 SEARCHNEW_API nuINT   LibSH_GetNEXTAll(nuVOID* pkeyStr, nuUINT& WordCont);
SEARCHNEW_API nuINT LibSH_SetAreaCenterPos(nuroPOINT pos);

