// Ctrl_NT.h: interface for the CCtrl_TT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTRL_TT_H__A8D3703C_0E57_4D38_B19F_D734DDD7F6C4__INCLUDED_)
#define AFX_CTRL_TT_H__A8D3703C_0E57_4D38_B19F_D734DDD7F6C4__INCLUDED_

	#include "NuroDefine.h"
	#include "ConstSet.h"

	typedef  struct  Tag_Stru_TT_A
	{
		nuULONG EndNodeID : 20;//起始NodeID
		nuULONG EndRtBID : 12;
		nuULONG PassNodeID : 20;//起始NodeID
		nuULONG PassRtBID : 12;
		nuULONG B_addr;
	}Stru_TT_A,*pStru_TT_A;

	typedef  struct  Tag_Stru_TT_B
	{
		nuULONG nCount:4;
		nuULONG nNodeCountList1:4;
		nuULONG nNodeCountList2:4;
		nuULONG nNodeCountList3:4;
		nuULONG nNodeCountList4:4;
		nuULONG nNodeCountList5:4;
		nuULONG nNodeCountList6:4;
		nuULONG nNodeCountList7:4;
	}Stru_TT_B,*pStru_TT_B;

	typedef struct Tag_StruTT
	{
		nuULONG TotalNoTurnCount;
		nuULONG SizeofB;
		Stru_TT_A **m_TTItem;//Sort by PassNode,StartNode,EndNode
		nuBYTE **m_TTItemB;
	}StruTT,*pStruTT;

	typedef  struct  Tag_Stru_TT_NODEID
	{
		nuULONG NodeID : 20;//起始NodeID
		nuULONG RtBID : 12;
	}Stru_TT_NODEID,*pStru_TT_NODEID;

	class CCtrl_TT : public StruTT
	{
		public:
			CCtrl_TT();
			virtual ~CCtrl_TT();

			nuBOOL InitClass();
			nuVOID CloseClass();

			nuBOOL ReadStruct();
			nuVOID ReleaseStruct();
			nuVOID InitTTStruct();
			nuFILE				*TTFile;
			nuTCHAR				FName[NURO_MAX_PATH];
			nuBOOL				m_bUSETT;

			nuULONG GetData(nuBOOL bCheck,nuULONG Addr,nuULONG DataIdx);//DataIdx從0開始
			Stru_TT_NODEID TTIDList[16];

			nuULONG m_BaseShift;
		//#ifdef _USETTI
			nuLONG	TTI_Address;
		//#endif
		#ifdef _DEBUG
			int  TTAlocCount;
			int  TTFreeCount;
		#endif
	};

#endif // !defined(AFX_CTRL_NT_H__A8D3703C_0E57_4D38_B19F_D734DDD7F6C4__INCLUDED_)
