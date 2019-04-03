// FileRn.cpp: implementation of the CFileRn class.
//
//////////////////////////////////////////////////////////////////////

#include "FileRn.h"
#include "libFileSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileRn::CFileRn()
{

}

CFileRn::~CFileRn()
{
	Free();
}

nuBOOL CFileRn::Initialize()
{
	nuWCHAR name[256] = {0};
	LibFS_ReadConstWcs(WCS_TYPE_ROAD_POSTFIX, 0, name, sizeof(name));
	m_wsLane = name[0];
	LibFS_ReadConstWcs(WCS_TYPE_ROAD_POSTFIX, 1, name, sizeof(name));
	m_wsAlley = name[0];
	return true;
}

nuVOID CFileRn::Free()
{
}

nuBOOL CFileRn::GetRoadName(nuWORD nDwIdx, long nNameAddr, int nRoadClass, nuPVOID pNameBuf, nuWORD nBufLen, PRNEXTEND pRnEx)
{
	if( nDwIdx == -1 || pNameBuf == NULL/* || pRnEx == NULL */)
	{
		return nuFALSE;
	}
	nuFILE *pFile = NULL;
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	if( nNameAddr >= 0 )
	{
		nuTcscpy(sFile, nuTEXT(".RN"));
		if( !LibFS_FindFileWholePath(nDwIdx, sFile, NURO_MAX_PATH) )
		{
			return nuFALSE;
		}
		pFile = nuTfopen(sFile, NURO_FS_RB);
		if( pFile == NULL )
		{
			return nuFALSE;
		}
		if( nuFseek(pFile, nNameAddr, NURO_SEEK_SET) != 0 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		if( nuFread(pRnEx, sizeof(RNEXTEND), 1, pFile) != 1 )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		if( pRnEx->nRNLen > nBufLen )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		nuMemset(pNameBuf, 0, nBufLen);
		if( nuFread(pNameBuf, 1, pRnEx->nRNLen, pFile) != pRnEx->nRNLen )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		nuFclose(pFile);
		RoadNameProc((nuWCHAR*)pNameBuf, pRnEx->nRNLen/2, pRnEx);
		/*test
		nuWCHAR wsName[120];
		nuBYTE nLenLen;
		nuMemset(wsName, 0, sizeof(wsName));
		nuFread(&nLenLen, 1, 1, pFile);
		nuFread(wsName, nLenLen, 1, pFile);
		MessageBoxW(NULL, (nuWCHAR*)pNameBuf, L"", 0);
		MessageBoxW(NULL, wsName, L"", 0);
		int kk = 0;*/
	}
	else
	{
		int nLen = (nuBYTE)LibFS_ReadConstWcs(WCS_TYPE_ROAD_NAME, nRoadClass, (nuWCHAR*)pNameBuf, nBufLen);
		pRnEx->nIconType	= NO_NAME_ROAD;
		if( nLen <= 0 )
		{
			pRnEx->nRNLen = 24;
			nuWCHAR ws[5] = {0};
			ws[0] = 0x4E00;
			ws[1] = 0x822C;
			ws[2] = 0x9053;
			ws[3] = 0x8DEF;
			
			//nuAsctoWcs(ws, 20, "No Name Road", 20);
			nuMemcpy(pNameBuf, ws, 24);
		}
		else
		{
			pRnEx->nRNLen		= (nuBYTE)nLen;
		}
	}
	return true;
}

nuVOID CFileRn::RoadNameProc(nuWCHAR* pNameBuf, nuWORD nNameNum, PRNEXTEND pRnEx)
{
//	return ;
//	nuWCHAR *pwsLane = NULL, *pwsAllay = NULL;
	int nAlley = nNameNum;
	int i = 0;
	nuBYTE nCut = 0;
	for( i = nNameNum - 1; i >= 0; --i )
	{
		if( pNameBuf[i] == '#' )
		{
			if( nNameNum - i > 1 )
			{
				int j;
				for(j = i; j < nNameNum - 1; ++j )
				{
					pNameBuf[j] = pNameBuf[j+1];
				}
				pNameBuf[j] = m_wsAlley;
			}
			else
			{
				pNameBuf[i] = '\0';
				nCut += 2;
				nNameNum -= 1;
			}
			nAlley = i;
		}
		else if( pNameBuf[i] == '@' ) 
		{
			if( nAlley - i > 1 )
			{
				int j;
				for(j = i; j < nAlley - 1; ++j)
				{
					pNameBuf[j] = pNameBuf[j+1];
				}
				pNameBuf[j] = m_wsLane;
			}
			else
			{
				int j;
				for( j = i; j < nNameNum; ++j )
				{
					pNameBuf[j] = pNameBuf[j+1];
				}
				nCut += 2;
			}
			break;
		}
	}
	if( pRnEx != NULL )
	{
		pRnEx->nRNLen -= nCut;
	}
}
