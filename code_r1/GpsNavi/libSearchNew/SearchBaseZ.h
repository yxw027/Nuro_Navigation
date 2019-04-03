/************************************************************************
* @file SearchBaseZ.h
* 
* 
* 包含SearchBaseZ定义	                                                             
************************************************************************/
#if !defined(AFX_SEARCHBASEZ_H__B7486302_1B72_419A_9E3E_466974FD0053__INCLUDED_)
#define AFX_SEARCHBASEZ_H__B7486302_1B72_419A_9E3E_466974FD0053__INCLUDED_

#include "NuroModuleApiStruct.h"
#include "nuSearchDefine.h"
#include "NuroClasses.h"

#define _USE_SEARCH_ROAD_ 1           ///编译道路搜索模块代码
#define _SEARCH_ROAD_MAINROADMODE_ 1  ///定义主道路模式（PANA在用）获取道路时使用带MR后缀的
#define _USE_PANA_MODE_ 1             ///启用pana特别处理：搜索条件的设置有不同的处理

#define _SEARCH_ROAD_FJUDGE_ENDWORD_ '@'  ///fuzzy judge end word
#define _SEARCH_ROAD_FJUDGE_ENDWORD1_ '#'  //added by ouyang 20100120 一条道路可能是只出现弄

/**
* @brief Search基类
*/
class CSearchBaseZ  
{
public:
    CSearchBaseZ();
    virtual ~CSearchBaseZ();
    
    /**
    * @brief  初始化依赖的模块
    *
    * @param pApiAddr 包含各模块API
    * @return 返回值表示是否初始化成功 \n
    *	true: 成功\n
    *	false: 失败\n
    */
    static nuBOOL InitData(PAPISTRUCTADDR pApiAddr);
    
    /**
    * 
    */
    static nuVOID FreeData();
    
    /**
    * @brief 获取运行状态
    *
    * @return 是否运行\n
    *	true: 正在运行\n 
    *	false: 未运行
    */
    nuBOOL IsRun();
    
    /**
    * @brief 缓存序列控制转换
    *
    * @param type 搜索类型\n
    *	NURO_SEARCH_NEXT：非法的NEXT会被转化为NURO_SEARCH_NEW实现\n
    *	NURO_SEARCH_RELOAD：buffer容量过小会返回false\n
    *	NURO_SEARCH_PRE：会填充满buffer（nMax限制存在）\n
    *	...
    * @param nMax 最大允许限定
    * @param lastPos 最后队列标记位置
    * @param lastCount 最后次搜索获取的数量
    * @retval pTranResTagPos 返回队列搜索起始位置
    * @retval pTranResMaxNum 返回允许最大数量
    * @return 转换是否成功
    */
    static nuBOOL TranslateSS(const NURO_SEARCH_TYPE &type, 
        const nuUINT &nMax, 
        const nuUINT &lastPos, 
        const nuUINT &lastCount, 
        nuUINT *pTranResTagPos, 
        nuUINT *pTranResMaxNum);
    
    /** 
    * @brief 缓存队列排序
    * 
    * @param pBuffer 指向缓存队列
    * @param max 缓存队列中元素数量
    * @param type 排列类型\n
    *	1:city\n
    *	2:stroke+ name\n
    *	3:nameaddr\n
    *	4:city+ town+ stroke+ name
    * @return 操作是否成功\n
    *	true:成功\n
    *	false:失败
    */
    static nuBOOL QueueBUffer(SEARCH_BUFFER *pBuffer, nuUINT max, nuBYTE type);
    static nuBOOL QueueBUfferRoad(SEARCH_BUFFER_ROAD *pBuffer, nuUINT max, nuBYTE type);
    
protected:
    
    nuBOOL	m_bRun; ///运行状态
    
protected:
    static NURO_SEARCHINFO m_searchInfo;  ///设置信息
    static PFILESYSAPI	m_pFsApi;         ///文件系统模块
    static PMEMMGRAPI_SH	m_pMmApi;           ///内存管理模块
    static nuUINT	m_nCityTotal;           ///载入地图的city数量
    static nuUINT	m_nCBInfoLen;           ///载入地图的CBFILE信息长度
    static nuUINT	m_nStrokeLimIdx;        ///笔划上下时的索引定位	
};

#endif // !defined(AFX_SEARCHBASEZ_H__B7486302_1B72_419A_9E3E_466974FD0053__INCLUDED_)
