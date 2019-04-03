// MemoMgrZ.cpp: implementation of the CMemoMgrZ class.
//
//////////////////////////////////////////////////////////////////////

#include "MemoMgrZ.h"
#include "NuroModuleApiStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern PGDIAPI			g_pDMLibGDI;
extern PFILESYSAPI		g_pDMLibFS;
CMemoMgrZ::CMemoMgrZ()
{
	nuMemset(&m_bmpMemo, 0, sizeof(m_bmpMemo));
	nuMemset(m_memoList, 0, sizeof(m_memoList));
	m_nMemoCount = 0;
}

CMemoMgrZ::~CMemoMgrZ()
{

}

nuBOOL CMemoMgrZ::Initialize()
{
	nuTCHAR tsFile[NURO_MAX_PATH]={0};
	nuTcscpy(tsFile, g_pDMLibFS->pGdata->pTsPath);
	nuTcscat(tsFile, _MEMO_BMP_FILE_NAME);
	g_pDMLibGDI->GdiLoadNuroBMP(&m_bmpMemo, tsFile);
	return nuTRUE;
}

nuVOID CMemoMgrZ::Free()
{
	g_pDMLibGDI->GdiDelNuroBMP(&m_bmpMemo);
}

nuBOOL CMemoMgrZ::RemoveOutMemo(nuroRECT* pRange)
{
	nuBYTE nDel = 0;
	nuINT nIdxDel = -1;
	nuINT i = 0;
	for(i = 0; i < m_nMemoCount; ++i)
	{
		if( nuPointInRect(&m_memoList[i].point, pRange) )
		{
			m_memoList[i].nState = 1;
		}
		else
		{
			++nDel;
			m_memoList[i].nState = 0;
			if( nIdxDel == -1 )
			{
				nIdxDel = i;
			}
		}
	}
	if( nDel < m_nMemoCount && nDel > 0 )
	{
		for( i = nIdxDel; i < m_nMemoCount; ++i )
		{
			if( m_memoList[i].nState )
			{
				m_memoList[nIdxDel++] = m_memoList[i];
			}
		}
	}
	m_nMemoCount -= nDel;
	return nuTRUE;
}

nuBOOL CMemoMgrZ::AddVoiceMemo(nuLONG x, nuLONG y)
{
	for( nuINT i = 0; i < m_nMemoCount; ++i )
	{
		if( m_memoList[i].point.x == x &&
			m_memoList[i].point.y == y )
		{
			return nuFALSE;
		}
	}
	if( m_nMemoCount < _MEMO_VOICED_LIST_MAX )
	{
		m_memoList[m_nMemoCount].point.x = x;
		m_memoList[m_nMemoCount].point.y = y;
		++m_nMemoCount;
	}
	return nuTRUE;
}

nuBOOL CMemoMgrZ::DrawMemoIcon(nuLONG x, nuLONG y, nuLONG nIconIdx)
{
	if( !m_bmpMemo.bmpHasLoad || nIconIdx < 0 || (nIconIdx * m_bmpMemo.bmpWidth) >= m_bmpMemo.bmpHeight )
	{
		return nuFALSE;
	}
	x -= m_bmpMemo.bmpWidth / 2;
	y -= m_bmpMemo.bmpWidth;
	NURO_BMP bmpTmp  = {0};
	bmpTmp.bmpHasLoad	= 1;
	bmpTmp.bmpBitCount	= NURO_BMP_BITCOUNT_16;
	bmpTmp.bmpWidth		= bmpTmp.bmpHeight = m_bmpMemo.bmpWidth;
	bmpTmp.bmpType		= NURO_BMP_TYPE_USE_TRANSCOLOR;
	bmpTmp.bmpBuffLen	= bmpTmp.bmpWidth * bmpTmp.bmpHeight * 2;
	bmpTmp.pBmpBuff		= m_bmpMemo.pBmpBuff + bmpTmp.bmpBuffLen * nIconIdx;
	bmpTmp.bytesPerLine = 2 * bmpTmp.bmpWidth; //add by xiaoqin @2011.05.19
	bmpTmp.bmpTransfColor = NURO_BMP_TRANSCOLOR_565;
	g_pDMLibGDI->GdiDrawBMP(x, y, bmpTmp.bmpWidth, bmpTmp.bmpHeight, &bmpTmp, 0, 0);
	return nuTRUE;
}