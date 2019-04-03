// AreaCollect.cpp: implementation of the CAreaCollect class.
//
//////////////////////////////////////////////////////////////////////

#include "AreaCollect.h"
#include "libDrawMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAreaCollect::CAreaCollect()
{
	m_pAreaList	= NULL;
	m_nAreaType	= 0;
	m_nMemIdx	= -1;
	m_pMemStartAddr = NULL;
}

CAreaCollect::~CAreaCollect()
{
	Free();
}

nuBOOL CAreaCollect::Initialize(nuUINT nAreaType, nuDWORD nMemSize /* = BG_AREA_COL_MEM_SIZE */ )
{
	m_pAreaList = (PAREATYPE)g_pDMLibMM->MM_GetStaticMemMass(sizeof(AREATYPE)*nAreaType);
	if( m_pAreaList == NULL )
	{
		return nuFALSE;
	}
	m_nAreaType = (nuWORD)nAreaType;
	nuMemset(m_pAreaList, 0, sizeof(AREATYPE)*m_nAreaType);
	m_nMemSize	= nMemSize;
	m_nMemIdx	= -1;
	m_pMemStartAddr = NULL;
	return nuTRUE;
}

nuVOID CAreaCollect::Free()
{
	DelAreaSeg();
	if( m_nMemIdx != -1 )
	{
		g_pDMLibMM->MM_RelDataMemMass(&m_nMemIdx);
		m_nMemSize = 0;
		m_nMemIdx = -1;
	}
	m_pAreaList	= NULL;
	m_nAreaType	= 0;
}

nuBOOL CAreaCollect::AddAreaSeg(nuUINT nScreenDwIdx, nuUINT nScreenBlkIdx, nuUINT nScreenSegIdx, nuUINT nType)
{
	if( nType >= m_nAreaType || m_nMemUsed + sizeof(AREANODE) > m_nMemSize )
	{
		return nuFALSE;
	}
	PAREANODE pNode = (PAREANODE)(m_pMemStartAddr + m_nMemUsed);//new AREANODE;
	m_nMemUsed += sizeof(AREANODE);
	/*
	if( pNode == NULL )
	{
		return nuFALSE;
	}
	*/
	pNode->nScreenDwIdx		= nScreenDwIdx;
	pNode->nScreenBlkIdx	= nScreenBlkIdx;
	pNode->nScreenSegIdx	= nScreenSegIdx;
	pNode->pNext			= NULL;
	//
	if( m_pAreaList[nType].pAreaHead == NULL )
	{
		m_pAreaList[nType].pAreaEnd = m_pAreaList[nType].pAreaHead = pNode;
	}
	else
	{
		m_pAreaList[nType].pAreaEnd->pNext = pNode;
		m_pAreaList[nType].pAreaEnd	= pNode;
	}
	return nuTRUE;
}

nuVOID CAreaCollect::DelAreaSeg()
{
	for(nuUINT i = 0; i < m_nAreaType; i++)
	{
		/*
		while( m_pAreaList[i].pAreaHead != NULL )
		{
			m_pAreaList[i].pAreaEnd = m_pAreaList[i].pAreaHead->pNext;
			delete m_pAreaList[i].pAreaHead;
			m_pAreaList[i].pAreaHead = m_pAreaList[i].pAreaEnd;
		}
		*/
		m_pAreaList[i].pAreaHead = m_pAreaList[i].pAreaEnd	= NULL;
	}
	m_pMemStartAddr = (nuPBYTE)g_pDMLibMM->MM_GetDataMassAddr(m_nMemIdx);
	if( m_pMemStartAddr == NULL )
	{
		m_pMemStartAddr = (nuPBYTE)g_pDMLibMM->MM_GetDataMemMass(m_nMemSize, &m_nMemIdx);
	}
	if( m_pMemStartAddr == NULL )
	{
		m_nMemUsed = m_nMemSize;
	}
	else
	{
		m_nMemUsed = 0;
	}
}