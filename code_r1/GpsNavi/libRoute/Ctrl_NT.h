// Ctrl_NT.h: interface for the CCtrl_NT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTRL_NT_H__A8D3703C_0E57_4D38_B19F_D734DDD7F6C4__INCLUDED_)
#define AFX_CTRL_NT_H__A8D3703C_0E57_4D38_B19F_D734DDD7F6C4__INCLUDED_

	#include "NuroDefine.h"

	typedef  struct  Tag_StruNTItem
	{
		nuLONG PassNodeID : 20;//起始NodeID
		nuLONG PassRtBID : 12;
		nuLONG StartNodeID : 20;//起始NodeID
		nuLONG StartRtBID : 12;
		nuLONG EndNodeID : 20;//起始NodeID
		nuLONG EndRtBID : 12;
	}StruNTItem,*pStruNTItem;

	typedef struct Tag_StruNT
	{
		nuUINT     TotalNoTurnCount;
		StruNTItem **m_NTItem;//Sort by PassNode,StartNode,EndNode
	}StruNT,*pStruNT;

	class CCtrl_NT : public StruNT
	{
		public:
			CCtrl_NT();
			virtual ~CCtrl_NT();

			nuBOOL InitClass();
			nuVOID CloseClass();

			nuBOOL ReadStruct();
			nuVOID ReleaseStruct();
			nuVOID InitNTStruct();
			nuFILE				*NTFile;
			nuTCHAR				FName[NURO_MAX_PATH];
		#ifdef _DEBUG
			int  NTAlocCount;
			int  NTFreeCount;
		#endif
	};

#endif // !defined(AFX_CTRL_NT_H__A8D3703C_0E57_4D38_B19F_D734DDD7F6C4__INCLUDED_)
