// AreaCollect.h: interface for the CAreaCollect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AREACOLLECT_H__6B0344D6_4320_40A1_A6CB_ABA88F806696__INCLUDED_)
#define AFX_AREACOLLECT_H__6B0344D6_4320_40A1_A6CB_ABA88F806696__INCLUDED_

#include "NuroDefine.h"

typedef struct tagAREANODE
{
	nuBYTE	    nScreenDwIdx;
	nuBYTE	    nReserve;
	nuWORD    	nScreenBlkIdx;
	nuDWORD	    nScreenSegIdx;
	tagAREANODE *pNext;
}AREANODE, *PAREANODE;

typedef struct tagAREATYPE
{
	PAREANODE	pAreaHead;
	PAREANODE	pAreaEnd;
}AREATYPE, *PAREATYPE;

#define BG_AREA_COL_MEM_SIZE		 (500 * sizeof(AREANODE))
class CAreaCollect  
{
public:
	CAreaCollect();
	virtual ~CAreaCollect();

	nuBOOL Initialize(nuUINT nAreaType, nuDWORD nMemSize = BG_AREA_COL_MEM_SIZE );
	nuVOID Free();

	nuBOOL AddAreaSeg(nuUINT nScreenDwIdx, nuUINT nScreenBlkIdx, nuUINT nScreenSegIdx, nuUINT nType); 
	nuVOID DelAreaSeg();

protected:

public:
	PAREATYPE	m_pAreaList;
	nuWORD		m_nAreaType;

protected:
	nuWORD		m_nMemIdx;
	nuPBYTE		m_pMemStartAddr;
	nuDWORD		m_nMemUsed;
	nuDWORD		m_nMemSize;
};

#endif // !defined(AFX_AREACOLLECT_H__6B0344D6_4320_40A1_A6CB_ABA88F806696__INCLUDED_)
