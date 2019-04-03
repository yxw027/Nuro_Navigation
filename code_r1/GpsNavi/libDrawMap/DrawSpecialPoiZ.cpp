// DrawSpecialPoiZ.cpp: implementation of the CDrawSpecialPoiZ class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawSpecialPoiZ.h"
#include "NuroModuleApiStruct.h"
#include "NuroClasses.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern PGDIAPI				g_pDMLibGDI;

CDrawSpecialPoiZ::CDrawSpecialPoiZ()
{
	m_pFile	= NULL;
	m_pNowIconBuff	= NULL;
	m_nNowIconType	= -1;
}

CDrawSpecialPoiZ::~CDrawSpecialPoiZ()
{
	if( m_pNowIconBuff != NULL )
	{
		delete []m_pNowIconBuff;
		m_pNowIconBuff=NULL;
	}
	CloseBmpFile();
}

nuBOOL CDrawSpecialPoiZ::OpenBmpFile(const nuTCHAR *pTsPath)
{
	if( m_pFile )
	{
		return nuTRUE;
	}
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, pTsPath);
	nuTcscat(tsFile, NURO_SPECIAL_POI_BMP);
	m_pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( !m_pFile )
	{
		return nuFALSE;
	}
	if ( m_pNowIconBuff == NULL )
	{
		if( nuFseek(m_pFile, 0, NURO_SEEK_SET) != 0 ||
			nuFread(&m_bmpFileHead, sizeof(NURO_BITMAPFILEHEADER), 1, m_pFile) != 1 ||
			nuFread(&m_bmpInfoHead, sizeof(NURO_BITMAPINFOHEADER), 1, m_pFile) != 1 )
		{
			CloseBmpFile();
			return nuFALSE;
		}
		m_nBuffLen = m_bmpInfoHead.biWidth * m_bmpInfoHead.biWidth * 2;
		m_pNowIconBuff = new nuBYTE[m_nBuffLen];
		if( m_pNowIconBuff == NULL )
		{
			CloseBmpFile();
			return nuFALSE;
		}
		m_nNowIconType = -1;
	}
	return nuTRUE;
}

nuVOID CDrawSpecialPoiZ::CloseBmpFile()
{
	if( m_pFile )
	{
		nuFclose(m_pFile);
		m_pFile = NULL;
	}
}

nuINT CDrawSpecialPoiZ::DrawSpPOI(nuINT x, nuINT y, nuLONG nIconNum, nuINT nIconAlignType /* = 0 */)
{
	if( m_pFile == NULL || m_pNowIconBuff == NULL )
	{
		return 0;
	}
	if( m_nNowIconType != nIconNum )
	{
		nuINT nDis = (nIconNum - 1) * m_nBuffLen + m_bmpFileHead.bfOffBits;
		if( nuFseek(m_pFile, nDis, NURO_SEEK_SET) != 0 )
		{
			return 0;
		}
		if( nuFread(m_pNowIconBuff, m_nBuffLen, 1, m_pFile) != 1 )
		{
			return 0;
		}
		m_nNowIconType = nIconNum;
	}
	x -= m_bmpInfoHead.biWidth/2;
	y -= m_bmpInfoHead.biWidth;
	NURO_BMP bmpIcon= {0};
	bmpIcon.bmpHasLoad	= 1;
	bmpIcon.bmpBitCount	= NURO_BMP_BITCOUNT_16;
	bmpIcon.bmpWidth	= m_bmpInfoHead.biWidth;
	bmpIcon.bmpHeight	= m_bmpInfoHead.biWidth;
	bmpIcon.bmpReserve	= NURO_BMP_DIB_BUFFER;
	bmpIcon.bmpBuffLen	= m_nBuffLen;
	bmpIcon.pBmpBuff	= m_pNowIconBuff;
	bmpIcon.bmpType		= NURO_BMP_TYPE_USE_TRANSCOLOR;
	
	g_pDMLibGDI->GdiDrawBMP(x, y, bmpIcon.bmpWidth, bmpIcon.bmpHeight, &bmpIcon, 0, 0);
	return m_bmpInfoHead.biWidth;
}

