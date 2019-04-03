// Ctrl_Tmc.h: interface for the CCtrl_Tmc class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_CTRL_Tmc_H__A8D3703C_0E57_4D38_B19F_D734DDD7F6C4__INCLUDED_)
#define AFX_CTRL_Tmc_H__A8D3703C_0E57_4D38_B19F_D734DDD7F6C4__INCLUDED_

#include "NuroDefine.h"
#include "nuRouteData.h"

	typedef struct Tag_StructTMCItemC_EventName
	{
		nuSHORT TMC_Num;
		nuSHORT TMC_NameLen;
		nuSHORT Weight;
		nuSHORT Time;
	}StructTMCItemC_EventName;

	typedef struct Tag_StructTMCItemC
	{
		StructTMCItemC_EventName TMC_Event;
		nuWCHAR  *TMC_EventName;  
	}StructTMCItemC;

	typedef struct Tag_StructTMCItemB
	{
		nuLONG NameAddr;
		nuLONG FNodeID:20;
		nuLONG FRTBID:12;
		nuLONG TNodeID:20;
		nuLONG TRTBID:12;
		nuLONG reserve;
	}StructTMCItemB;

	typedef struct Tag_StructTMCItemA
	{
		nuSHORT LocationCode;
		nuSHORT LocationCodeNum;
		nuLONG  LocationVertexAddr;
		nuLONG  LocationUpAddr;
		nuLONG  LocationDownAddr;
	}StructTMCItemA;

	typedef struct Tag_StructTMCData
	{
		nuULONG DataCnt;
		nuUSHORT TrafficCnt;	
		nuULONG TMCDataAddr;
		nuULONG TrafficDataAddr;
		nuUSHORT TMCReserve;

		StructTMCItemA TMCItemA;
		StructTMCItemB **TMCItemB;
		StructTMCItemC **TMCItemC;
	}StructTMCData;

	typedef struct Tag_StructTMCNodeIDList
	{
		nuLONG NodeID:20;
		nuLONG RTBID:12;
		nuBYTE Time:8;
		nuBYTE Weighting:8;
	}StructTMCNodeIDList, *pTMCNodeIDList;

	class CCtrl_Tmc : public StructTMCData
	{
			public:
				CCtrl_Tmc();
				virtual ~CCtrl_Tmc();

				nuBOOL InitClass();
				nuVOID CloseClass();
				
				nuBOOL ReadTMC(TMC_ROUTER_DATA *TMCPassData, nuSHORT TMCEventCount, nuBOOL *bTMCRTB);
				nuBOOL ReadTMC_BTable(nuULONG i,nuULONG Index);
				nuBOOL ReleaseTMC_BTable(nuULONG i);
				//nuBOOL B_Table_CountingSort();
				nuBOOL B_Table_shellSort();
				nuVOID ReleaseStruct();

			public:
				nuLONG TMCFileaddr;
				nuLONG TMCNodeIDCount;
				nuFILE			*TMCFile;
				nuTCHAR			FName[NURO_MAX_PATH];
				pTMCNodeIDList  *m_ppTMCNodeIDList;
				TMC_ROUTER_DATA *pTMCPassData;
		#ifdef _DEBUG
				nuINT TmcAlocCount;
				nuINT TmcFreeCount;
		#endif
	};


#endif

