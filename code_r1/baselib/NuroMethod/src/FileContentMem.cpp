// FileContentMem.cpp: implementation of the CFileContentMem class.
//
//////////////////////////////////////////////////////////////////////

#include "../FileContentMem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileContentMem::CFileContentMem()
{
   m_pMem    = NULL;
   m_nMemLen = 0;
   m_nMemPos = 0;
}

CFileContentMem::~CFileContentMem()
{
	FreeContentMem();
}

nuBOOL CFileContentMem::ReadFileContent(nuCHAR *pFile)
{
	if( !pFile )
	{
		return nuFALSE;
	}
    
    if( m_pMem )
    {
		return nuTRUE;
    }

	nuFILE *p = nuFopen( pFile, NURO_FS_RB );
	if ( !p )
	{
		return nuFALSE;
	}
	m_nMemLen = nuFgetlen( p );
	m_pMem = nuMalloc(sizeof (nuBYTE) * (m_nMemLen + 2)); //new nuBYTE[m_nMemLen + 2];
	if ( !m_pMem )
	{
		return nuFALSE;
	}
    nuMemset( m_pMem, 0, m_nMemLen + 2 );
    //一次读 1K的buffer
	nuLONG nSize = 1024;
	nuLONG nTotalLen = m_nMemLen;
	while( nuTRUE )
	{	
		if( 1 != nuFread( (nuCHAR *)m_pMem + m_nMemLen - nTotalLen, NURO_MIN(nSize,nTotalLen), 1, p ) )
		{
			return nuFALSE;
		}
		nTotalLen -= nSize;
		if ( nTotalLen <= 0 )
		{
			break;
		}

	}

    nuFclose( p );
	return nuTRUE;
}

nuBOOL CFileContentMem::ReadFileContent(nuWCHAR *pFile)
{
	if( !pFile )
	{
		return nuFALSE;
	}
    
    if( m_pMem )
    {
		return nuTRUE;
    }

	nuFILE *p = nuWfopen( pFile, NURO_FS_RB );
	if ( !p )
	{
		return nuFALSE;
	}
	m_nMemLen = nuFgetlen( p );
	m_pMem = nuMalloc(sizeof (nuBYTE) * (m_nMemLen + 2)); //new nuBYTE[m_nMemLen + 2];
	if ( !m_pMem )
	{
		return nuFALSE;
	}
    nuMemset( m_pMem, 0, m_nMemLen + 2 );
    //一次读 1K的buffer
	nuLONG nSize = 1024;
	nuLONG nTotalLen = m_nMemLen;
	while( nuTRUE )
	{	
		if( 1 != nuFread( (nuCHAR *)m_pMem + m_nMemLen - nTotalLen, NURO_MIN(nSize,nTotalLen), 1, p ) )
		{
			return nuFALSE;
		}
		nTotalLen -= nSize;
		if ( nTotalLen <= 0 )
		{
			break;
		}

	}

    nuFclose( p );
	return nuTRUE;
}

nuBOOL CFileContentMem::GetLineContentW(nuWCHAR *pInOut, nuLONG nLenStr)
{
	if ( !m_pMem || m_nMemPos >= m_nMemLen )
	{
		return nuFALSE;
	}
	//找回车换行符
	nuWCHAR wsEnd[3] ={ 0x0D, 0x0A };
	nuWCHAR *pRet = NULL;
	nuWCHAR *pStart = ((nuWCHAR *)m_pMem + m_nMemPos/2);
    if ( !( pRet = nuWcsstr( pStart, wsEnd )) )
    {
		if ( m_nMemLen - m_nMemPos >= nLenStr  )
		{
           return nuFALSE;
		}
		nuMemcpy( pInOut, pStart, (m_nMemLen - m_nMemPos)  );
		m_nMemPos = m_nMemLen;
    }
	else
	{
		if ( ( pRet - pStart) * 2 >= nLenStr || !pRet )
		{
			return nuFALSE;
		}
		nuMemcpy( pInOut, pStart, ( pRet - pStart + 2) * 2 );
		m_nMemPos += (pRet - pStart + 2) * 2;
	}
	return nuTRUE;
}

nuBOOL CFileContentMem::GetLineContentC(nuCHAR *pInOut, nuLONG nLenStr)
{
	if ( !m_pMem || m_nMemPos >= m_nMemLen )
	{
		return nuFALSE;
	}
	//找回车换行符
	nuCHAR wsEnd[2] ={ 0x0D, 0x0A };
	nuCHAR *pRet = NULL;
	nuCHAR *pStart = ((nuCHAR *)m_pMem + m_nMemPos);

	nuLONG nLen = nuStrlen( pStart );
	nuINT nIndex = 0;
	while( nLen )
	{
		if ( 0 == nuMemcmp( pStart + nIndex, wsEnd, sizeof(wsEnd) ))
		{
			pRet = pStart + nIndex;
			break;
		}
		++nIndex;
		--nLen;
	}

    if ( !( pRet /*= nuStrstr( pStart, wsEnd )*/) )
    {
		if ( m_nMemLen - m_nMemPos >= nLenStr  )
		{
			return nuFALSE;
		}
		nuMemcpy( pInOut, pStart, (m_nMemLen - m_nMemPos)  );
		m_nMemPos = m_nMemLen;
    }
	else
	{
		if ( pRet - pStart >= nLenStr )
		{
			return nuFALSE;
		}
		nuMemcpy( pInOut, pStart, ( pRet - pStart + 2)  );
		m_nMemPos += (pRet - pStart + 2) ;
	}
	return nuTRUE;
}
nuBOOL CFileContentMem::FreeContentMem()
{
    if ( m_pMem )
	{
		nuFree(m_pMem);
		m_pMem = NULL;
		m_nMemPos = 0;
	}
	return nuTRUE;
}
nuBOOL CFileContentMem::ResetMemPos()
{
	m_nMemPos = 0;
	return nuTRUE;
}
