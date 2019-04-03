// LineCollect.cpp: implementation of the CLineCollect class.
//
//////////////////////////////////////////////////////////////////////

#include "LineCollect.h"
#include "libDrawMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLineCollect::CLineCollect()
{
	m_pLineList		= NULL;
	m_nClassCount	= 0;
	m_nMemIdx		= -1;
}

CLineCollect::~CLineCollect()
{
	Free();
}

nuBOOL CLineCollect::Initialize(nuWORD nClassCount, nuDWORD nMemSize /* = ROAD_COL_MEM_SIZE */)
{
	m_pLineList = (PCLASSLINE)g_pDMLibMM->MM_GetStaticMemMass(nClassCount * sizeof(CLASSLINE));
	if( m_pLineList == NULL )
	{
		return nuFALSE;
	}
	nuMemset(m_pLineList, 0, nClassCount * sizeof(CLASSLINE));
	m_nClassCount	= nClassCount;
	m_nLineSegCount = 0;
	m_nMemSize		= nMemSize;
	m_nMemUsed		= 0;
	m_pMemStartAddr	= NULL;
	return nuTRUE;
}

nuVOID CLineCollect::Free()
{
	DelLineList();
	if( m_nMemIdx != -1 )
	{
		g_pDMLibMM->MM_RelDataMemMass(&m_nMemIdx);
		m_nMemSize	= 0;
		m_nMemIdx	= -1;
	}
	m_pLineList		= NULL;
	m_nClassCount	= 0;
}

nuBOOL CLineCollect::AddLineSeg(nuLONG nScreenDwIdx, nuLONG nScreenBlkIdx, nuLONG nLineSegIdx, const NUROPOINT& ptFrom, 
							  const NUROPOINT &ptTo, nuWORD nClass)
{
	if( m_pLineList == NULL || nClass >= m_nClassCount || m_nMemUsed + sizeof(LINESEG) > m_nMemSize )
	{
		return nuFALSE;
	}
	PLINESEG pLineSeg = (PLINESEG)(m_pMemStartAddr + m_nMemUsed);//new LINESEG;
	m_nMemUsed += sizeof(LINESEG);
	/*
	if( pLineSeg == NULL )
	{
		return nuFALSE;
	}
	*/
	//
	PCONJLINE pConjLine = NULL;
	nuBYTE nConjWay = CONJ_ERROR;
	pConjLine = GetConjLine(&m_pLineList[nClass], ptFrom, ptTo, &nConjWay);
	if( pConjLine == NULL )
	{
//		delete pLineSeg;
		m_nMemUsed -= sizeof(LINESEG);
		return nuFALSE;
	}
	//
	pLineSeg->nScreenDwIdx	= (nuBYTE)nScreenDwIdx;
	pLineSeg->nScreenBlkIdx	= (nuWORD)nScreenBlkIdx;
	pLineSeg->nScreenRoadIdx= nLineSegIdx;
	pLineSeg->pNext			= NULL;
	if( nConjWay == CONJ_FIRSTLINE || pConjLine->pLineHead == NULL )
	{
		pLineSeg->nConjWay		= CONJ_FROM_TO;
		pConjLine->pLineHead	= pConjLine->pLineEnd = pLineSeg;
		pConjLine->ptFrom		= ptFrom;
		pConjLine->ptTo			= ptTo;
		pConjLine->nLineSegCount= 1;
	}
	else if( nConjWay == CONJ_FROM_FROM )
	{
		pLineSeg->nConjWay		= CONJ_TO_FROM;
		pLineSeg->pNext			= pConjLine->pLineHead;
		pConjLine->pLineHead	= pLineSeg;
		pConjLine->ptFrom		= ptTo;
		pConjLine->nLineSegCount ++;
	}
	else if( nConjWay == CONJ_FROM_TO )
	{
		pLineSeg->nConjWay		= CONJ_FROM_TO;
		pLineSeg->pNext			= pConjLine->pLineHead;
		pConjLine->pLineHead	= pLineSeg;
		pConjLine->ptFrom		= ptFrom;
		pConjLine->nLineSegCount ++;
	}
	else if( nConjWay == CONJ_TO_FROM )
	{
		pLineSeg->nConjWay			= CONJ_FROM_TO;
		pConjLine->pLineEnd->pNext	= pLineSeg;
		pConjLine->pLineEnd			= pLineSeg;
		pConjLine->ptTo				= ptTo;
		pConjLine->nLineSegCount ++;
	}
	else if( nConjWay == CONJ_TO_TO )
	{
		pLineSeg->nConjWay			= CONJ_TO_FROM;
		pConjLine->pLineEnd->pNext	= pLineSeg;
		pConjLine->pLineEnd			= pLineSeg;
		pConjLine->ptTo				= ptFrom;
		pConjLine->nLineSegCount ++;
	}
	else//Can't be here. it is wring if the procedure enters here.
	{
//		delete pLineSeg;
		m_nMemUsed -= sizeof(LINESEG);
		return nuFALSE;
	}
//	ColClassLine(&m_pLineList[nClass]);
	return nuTRUE;
}

PCONJLINE CLineCollect::GetConjLine(PCLASSLINE pClassLine, const NUROPOINT &ptFrom, const NUROPOINT& ptTo, nuPBYTE pConjWay)
{
	if( pClassLine == NULL )
	{
		return NULL;
	}
	PCONJLINE pTmp = pClassLine->pConjHead;
	nuLONG dx = 0, dy = 0;
	while( pTmp != NULL )
	{
		dx = pTmp->ptFrom.x - ptFrom.x;
		dy = pTmp->ptFrom.y - ptFrom.y;
		if( NURO_ABS(dx) + NURO_ABS(dy) < DIS_CONJIONT )
		{
			*pConjWay = CONJ_FROM_FROM;
			break;
		}
		dx = pTmp->ptFrom.x - ptTo.x;
		dy = pTmp->ptFrom.y - ptTo.y;
		if( NURO_ABS(dx) + NURO_ABS(dy) < DIS_CONJIONT )
		{
			*pConjWay = CONJ_FROM_TO;
			break;
		}
		dx = pTmp->ptTo.x - ptFrom.x;
		dy = pTmp->ptTo.y - ptFrom.y;
		if( NURO_ABS(dx) + NURO_ABS(dy) < DIS_CONJIONT )
		{
			*pConjWay = CONJ_TO_FROM;
			break;
		}
		dx = pTmp->ptTo.x - ptTo.x;
		dy = pTmp->ptTo.y - ptTo.y;
		if( NURO_ABS(dx) + NURO_ABS(dy) < DIS_CONJIONT )
		{
			*pConjWay = CONJ_TO_TO;
			break;
		}
		pTmp = pTmp->pNext;
	}
	if( pTmp == NULL && m_nMemUsed + sizeof(CONJLINE) <= m_nMemSize )
	{
		pTmp = (PCONJLINE)(m_pMemStartAddr + m_nMemUsed);//new CONJLINE;
		m_nMemUsed += sizeof(CONJLINE);
		/*
		if( pTmp == NULL )
		{
			return NULL;
		}
		*/
		pTmp->nLineSegCount	= 0;
		pTmp->pLineHead		= NULL;
		pTmp->pLineEnd		= NULL;
		pTmp->pNext			= NULL;
		pTmp->ptFrom		= ptFrom;
		pTmp->ptTo			= ptTo;
		*pConjWay			= CONJ_FIRSTLINE;
		if( pClassLine->pConjHead == NULL )
		{
			pClassLine->pConjEnd = pClassLine->pConjHead = pTmp;
		}
		else
		{
			pClassLine->pConjEnd->pNext = pTmp;
			pClassLine->pConjEnd = pTmp;
		}
		pClassLine->nConjLineCount ++;
		m_nLineSegCount ++;
	}
	return pTmp;
}

nuVOID CLineCollect::FreeDataMem()
{
	g_pDMLibMM->MM_RelDataMemMass(&m_nMemIdx);
	m_pMemStartAddr = NULL;
	m_nMemUsed = m_nMemSize;
}

nuVOID CLineCollect::DelLineList()
{
	for(nuWORD i = 0; i < m_nClassCount; i++)
	{
		DelClassLine(&m_pLineList[i]);
	}
//	DEBUGNUM(m_nLineSegCount);
	m_nLineSegCount = 0;

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
nuVOID CLineCollect::DelClassLine(PCLASSLINE pClassLine)
{
	/* used [new] operator
	while( pClassLine->pConjHead != NULL )
	{
		pClassLine->pConjEnd = pClassLine->pConjHead->pNext;
		DelConjLine(pClassLine->pConjHead);
		delete pClassLine->pConjHead;
		pClassLine->pConjHead = pClassLine->pConjEnd;
	}
	*/
	pClassLine->pConjHead	= pClassLine->pConjEnd = NULL;
	//
	pClassLine->nConjLineCount = 0;
}

nuVOID CLineCollect::DelConjLine(PCONJLINE pConjLine)
{
	/*
	while( pConjLine->pLineHead != NULL )
	{
		pConjLine->pLineEnd = pConjLine->pLineHead->pNext;
		delete pConjLine->pLineHead;
		pConjLine->pLineHead = pConjLine->pLineEnd;
	}
	*/
	pConjLine->pLineHead	= pConjLine->pLineEnd = NULL;
	pConjLine->nLineSegCount	= 0;
}

nuVOID CLineCollect::ColClassLine(PCLASSLINE pClassLine)
{
	PCONJLINE pConjLine2, pConjLine2Last;
	PCONJLINE pConjLine = pClassLine->pConjHead;
	nuBOOL bNext = nuFALSE;
	while( pConjLine != NULL )
	{
		pConjLine2Last = pConjLine;
		pConjLine2 = pConjLine->pNext;
		bNext = nuTRUE;
		while( pConjLine2 != NULL )
		{
			if( IsConjiontOfTwoConjLines(pConjLine, pConjLine2) == CONJ_ERROR )
			{
				pConjLine2Last = pConjLine2;
				pConjLine2 = pConjLine2->pNext;
			}
			else
			{
				pConjLine2Last->pNext = pConjLine2->pNext;
				pClassLine->nConjLineCount--;
				if( pClassLine->pConjEnd == pConjLine2 )
				{
					pClassLine->pConjEnd = pConjLine2Last;
				}
//				delete pConjLine2;
				pConjLine2 = NULL;
				bNext = nuFALSE;
			}
		}
		if( bNext )
		{
			pConjLine = pConjLine->pNext;
		}
	}
}
nuLONG CLineCollect::IsConjiontOfTwoConjLines(PCONJLINE pFConjLine, PCONJLINE pSConjLine)
{
	if( pFConjLine == NULL || pSConjLine == NULL )
	{
		return CONJ_ERROR;
	}
	PLINESEG pLineSeg = NULL;
	nuLONG dx = 0, dy = 0 ;
	dx = pFConjLine->ptFrom.x - pSConjLine->ptFrom.x;
	dy = pFConjLine->ptFrom.y - pSConjLine->ptFrom.y;
	if( NURO_ABS(dx) + NURO_ABS(dy) < DIS_CONJIONT )
	{
		pLineSeg = pFConjLine->pLineHead;
		while( pSConjLine->pLineHead != NULL )
		{
			pFConjLine->pLineHead = pSConjLine->pLineHead;
			pSConjLine->pLineHead = pSConjLine->pLineHead->pNext;
			pFConjLine->pLineHead->pNext = pLineSeg;
			pLineSeg = pFConjLine->pLineHead;
			if( pLineSeg->nConjWay == CONJ_FROM_TO )
			{
				pLineSeg->nConjWay = CONJ_TO_FROM;
			}
			else
			{
				pLineSeg->nConjWay = CONJ_FROM_TO;
			}
			pFConjLine->nLineSegCount ++;
		}
		pFConjLine->ptFrom = pSConjLine->ptTo;
		//... Delete pSConjLine
		return CONJ_FROM_FROM;
	}
	dx = pFConjLine->ptFrom.x - pSConjLine->ptTo.x;
	dy = pFConjLine->ptFrom.y - pSConjLine->ptTo.y;
	if( NURO_ABS(dx) + NURO_ABS(dy) < DIS_CONJIONT )
	{
		pSConjLine->pLineEnd->pNext = pFConjLine->pLineHead;
		pFConjLine->pLineHead		= pSConjLine->pLineHead;
		pFConjLine->ptFrom			= pSConjLine->ptFrom;
		pFConjLine->nLineSegCount	+= pSConjLine->nLineSegCount;
		//... Delete pSConjLine
		return CONJ_FROM_TO;
	}
	dx = pFConjLine->ptTo.x - pSConjLine->ptFrom.x;
	dy = pFConjLine->ptTo.y - pSConjLine->ptFrom.y;
	if( NURO_ABS(dx) + NURO_ABS(dy) < DIS_CONJIONT )
	{
		pFConjLine->pLineEnd->pNext	= pSConjLine->pLineHead;
		pFConjLine->pLineEnd		= pSConjLine->pLineEnd;
		pFConjLine->ptTo			= pSConjLine->ptTo;
		pFConjLine->nLineSegCount	+= pSConjLine->nLineSegCount;
		//... Delete pSConjLine
		return CONJ_TO_FROM;
	}
	dx = pFConjLine->ptTo.x - pSConjLine->ptTo.x;
	dy = pFConjLine->ptTo.y - pSConjLine->ptTo.y;
	if( NURO_ABS(dx) + NURO_ABS(dy) < DIS_CONJIONT )
	{
//		return CONJ_ERROR;
		pFConjLine->ptTo = pSConjLine->ptFrom;
		do
		{
			pSConjLine->pLineEnd->pNext = NULL;
			pFConjLine->pLineEnd->pNext = pSConjLine->pLineEnd;
			pFConjLine->pLineEnd = pSConjLine->pLineEnd;
			if( pFConjLine->pLineEnd->nConjWay == CONJ_FROM_TO )
			{
				pFConjLine->pLineEnd->nConjWay = CONJ_TO_FROM;
			}
			else
			{
				pFConjLine->pLineEnd->nConjWay = CONJ_FROM_TO;
			}
			//
			pLineSeg = pSConjLine->pLineHead;
			while( pLineSeg != NULL )
			{
				if( pLineSeg->pNext == pSConjLine->pLineEnd )
				{
					break;
				}
				pLineSeg = pLineSeg->pNext;
			}
			pSConjLine->pLineEnd = pLineSeg;
		}
		while( pSConjLine->pLineEnd != NULL );
		//... Delete pSConjLine
		return CONJ_TO_TO;
	}
	return CONJ_ERROR;
}

nuBOOL CLineCollect::AddCrossLineSeg( nuLONG nScreenDwIdx, nuLONG nScreenBlkIdx, nuLONG nLineSegIdx,
								    const NUROPOINT& ptFrom, const NUROPOINT& ptTo, nuWORD nClass )
{
	if( nClass >= m_nClassCount )
	{
		return nuFALSE;
	}
	PCONJLINE pCjLine = m_pLineList[nClass].pConjHead;
	while ( pCjLine != NULL )
	{
		PLINESEG pLineSeg = pCjLine->pLineHead;
		while( pLineSeg != NULL )
		{
			if( pLineSeg->nScreenDwIdx	== nScreenDwIdx &&
				pLineSeg->nScreenBlkIdx	== nScreenBlkIdx &&
				pLineSeg->nScreenRoadIdx== (nuDWORD)nLineSegIdx)
			{
				return nuFALSE;
			}
			pLineSeg = pLineSeg->pNext;
		}
		pCjLine = pCjLine->pNext;
	}
	//
	pCjLine = m_pLineList[nClass].pConjHead;
	while( pCjLine != NULL )
	{
		if( (ptFrom.x == pCjLine->ptFrom.x && ptFrom.y == pCjLine->ptFrom.y) ||
			(ptFrom.x == pCjLine->ptTo.x && ptFrom.y == pCjLine->ptTo.y) || 
			(ptTo.x == pCjLine->ptFrom.x && ptTo.y == pCjLine->ptFrom.y) ||
			(ptTo.x == pCjLine->ptTo.x && ptTo.y == pCjLine->ptTo.y) )
		{
			return AddLineSeg(nScreenDwIdx, nScreenBlkIdx, nLineSegIdx, ptFrom, ptTo, nClass);
		}
		pCjLine = pCjLine->pNext;
	}
	return nuFALSE;
}
