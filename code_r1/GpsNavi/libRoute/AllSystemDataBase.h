
#ifndef Def_AllSystemDataBase
	#define Def_AllSystemDataBase
	#include "Ctrl_3dp.h"
	#include "Ctrl_NT.h"
	#include "Ctrl_TT.h"
	#include "Ctrl_Ref.h"
	#include "Ctrl_RuleSet.h"
	#include "Ctrl_Thc.h"
	#include "Ctrl_Thr.h"
	#include "Ctrl_UONE_Rdw.h"
	#include "Ctrl_UONE_BH.h"
	#include "Ctrl_UONE_Rt.h"
	#include "CRoutingDataCtrl.h"
	#include "CDontToHere.h"
	#include "Ctrl_PTS.h"
	#include "Ctrl_Dst.h"//新加入的Dst檔
	#include "Ctrl_Tmc.h"
	#include "Ctrl_AR.h"
	#include "Ctrl_ECI.h"

	extern UONERDwCtrl		*g_pDB_DwCtrl;
	extern UONEBHCtrl		*g_pUOneBH;

	extern UONERtBCtrl		*g_pLocalRtB[2];//起訖點RTBlock的資料暫存區
	extern UONERtBCtrl		*g_pRtB_F;
	extern UONERtBCtrl		*g_pRtB_T;

	extern CDontToHere	    *g_pCDTH ;
	extern CRoutingDataCtrl *g_pCRDC;

	typedef struct Tag_MapIDtoMapIdx
	{
		nuULONG    MapCount;
		nuULONG    **MapIDList;
		nuULONG    **MapIdxList;
	}MapIDtoMapIdx,*pMapIDtoMapIdx;

	extern MapIDtoMapIdx	m_MapIDtoMapIdx;//MapID與MapIdx對應表的暫存區

	nuBOOL CheckPtCloseLU(NUROPOINT Pt1, NUROPOINT Pt2, nuLONG CloseLen);

#endif