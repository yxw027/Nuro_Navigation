// Ctrl_Rel.h: interface for the CCtrl_Rel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTRL_Rel_H__A8D3703C_0E57_4D38_B19F_D734DDD7F6C4__INCLUDED_)
#define AFX_CTRL_Rel_H__A8D3703C_0E57_4D38_B19F_D734DDD7F6C4__INCLUDED_

	#include "NuroDefine.h"

	typedef struct Tag_StruDstdata
	{
		nuWORD F_YY;
		nuCHAR F_MM;
		nuCHAR F_DD;
		nuWORD T_YY;
		nuCHAR T_MM;
		nuCHAR T_DD;
		nuLONG S_NODEID : 20;//°_ÂINodeID
		nuLONG T_NODEID : 20;//²×ÂINodeID
	}StruDstdata;

	typedef struct Tag_StruDst
	{
		nuUINT m_nTotalDstCount;
		Tag_StruDstdata **Dstdata;	
	}StruDst;

	class CCtrl_Dst : public  StruDst 
	{
		public:
			CCtrl_Dst();
			virtual ~CCtrl_Dst();

			nuBOOL InitClass();
			nuVOID CloseClass();

			nuBOOL ReadStruct();
			nuVOID ReleaseStruct();
			nuVOID InitDstStruct();
			nuFILE				*DstFile;
			nuTCHAR				FName[NURO_MAX_PATH];
	#ifdef _DEBUG
		int  DstAlocCount;
		int  DstFreeCount;
	#endif
	};
#endif
