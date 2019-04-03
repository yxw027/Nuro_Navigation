// ReadWcstring.cpp: implementation of the CReadWcstring class.
//
//////////////////////////////////////////////////////////////////////

#include "ReadWcstring.h"
#include "libFileSystem.h"


#define	CONSTSTR_NATIONWAY				4
#define CONSTSTR_FASTWAY				(CONSTSTR_NATIONWAY+1)
#define CONSTSTR_SLIPWAY				(CONSTSTR_NATIONWAY+2)
#define CONSTSTR_PROVWAY				(CONSTSTR_NATIONWAY+3)
#define CONSTSTR_CITYWAY				(CONSTSTR_NATIONWAY+4)
#define CONSTSTR_TOWNWAY				(CONSTSTR_NATIONWAY+5)
#define COSNTSTR_COMMWAY				(CONSTSTR_NATIONWAY+6)
#define COSNTSTR_OTHERWAY				(CONSTSTR_NATIONWAY+7)
#define CONSTSTR_LANEWAY				(CONSTSTR_NATIONWAY+8)
#define CONSTSTR_ALLEY					(CONSTSTR_NATIONWAY+9)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReadWcstring::CReadWcstring()
{
	m_pGdata = NULL;
}

CReadWcstring::~CReadWcstring()
{
	Free();
}

nuBOOL CReadWcstring::Initialize(PGLOBALENGINEDATA pGdata)
{
	m_pGdata = pGdata;
	return nuTRUE;
}
void CReadWcstring::Free()
{
	m_pGdata = NULL;
}


nuINT CReadWcstring::ReadConstWcs(nuBYTE nWcsType, nuLONG nRoadClass, nuWCHAR *wsBuf, nuWORD nLen)
{
	nuTCHAR sFile[NURO_MAX_PATH] = {0};
	nuTCHAR sNum[10] = {0};
	nuDWORD index = 0;
	if( nWcsType == WCS_TYPE_ROAD_NAME )
	{
		if( nRoadClass < 4 )
		{
			index = CONSTSTR_NATIONWAY;
		}
		else if( nRoadClass == 4 )
		{
			index = CONSTSTR_FASTWAY;
		}
		else if( nRoadClass == 5 )
		{
			index = CONSTSTR_SLIPWAY;
		}
		else if( nRoadClass == 6 )
		{
			index = CONSTSTR_PROVWAY;
		}
		else if( nRoadClass == 7 )
		{
			index = CONSTSTR_CITYWAY;
		}
		else if( nRoadClass == 8 )
		{
			index = CONSTSTR_TOWNWAY;
		}
		else if( nRoadClass == 9 )
		{
			index = COSNTSTR_COMMWAY;
		}
		else
		{
			index = COSNTSTR_OTHERWAY;
		}
	}
	else if( nWcsType == WCS_TYPE_DES_NAME )
	{
		index = 0;
	}
	else if( nWcsType == WCS_TYPE_PASS_NAME )
	{
		index = 3;
	}
	else if( nWcsType == WCS_TYPE_ROAD_POSTFIX )
	{
		index = nRoadClass + CONSTSTR_LANEWAY;
	}
	else
	{
		return 0;
	}
	nuItot(m_pGdata->uiSetData.nLanguage, sNum, 10);
	nuTcscpy(sFile, m_pGdata->pTsPath);
	nuTcscat(sFile, nuTEXT("Media\\File\\"));
	nuTcscat(sFile, sNum);
	nuTcscat(sFile, nuTEXT("\\ConstStr.txt"));
	return ReadConstTxt(index, wsBuf, nLen, sFile);
}

nuINT CReadWcstring::ReadConstTxt(nuDWORD index, nuWCHAR *wsBuf, nuWORD nLen, nuTCHAR* tFile)
{
	nuFILE *pFile = nuTfopen(tFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return 0;
	}
	nuFseek(pFile, 2, NURO_SEEK_SET);
	nuSIZE len = 0;
	while( nuFreadLine(wsBuf, nLen/2, pFile) != 0 )
//	while( fgetws(wsBuf, nLen/2, (FILE*)pFile) != NULL )
	{
		if(index == 0)
		{
			len  = nuWcslen(wsBuf);
//			len -= 2;
			wsBuf[len] = 0;
			break;
		}
		else
		{
			index--;
		}
	}
	nuFclose(pFile);
	return len*2;
}
