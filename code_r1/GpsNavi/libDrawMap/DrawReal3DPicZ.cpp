// DrawReal3DPicZ.cpp: implementation of the CDrawReal3DPicZ class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawReal3DPicZ.h"
#include "NuroModuleApiStruct.h"
#include "NuroClasses.h"

//#include "nuroresource.h"

extern PGDIAPI				g_pDMLibGDI;
extern PMEMMGRAPI			g_pDMLibMM;
extern PFILESYSAPI			g_pDMLibFS;
extern PGLOBALENGINEDATA    g_pDMGdata;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#define  NEW_ALP_FILE
CDrawReal3DPicZ::CDrawReal3DPicZ()
{
	m_bRead3DHead = nuFALSE;
	if( !nuReadConfigValue( "USEALP", &m_nUseAlp ) )
	{
		m_nUseAlp = 0;
	}
}

CDrawReal3DPicZ::~CDrawReal3DPicZ()
{
	Free();
}

nuBOOL CDrawReal3DPicZ::Initialize()
{
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile, g_pDMGdata->pTsPath);
	nuTcscat(tsFile, _REAL_3D_FILE);
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( NULL != pFile )
	{
		Read3DHeader(pFile);
	//	m_real3DHead.p3DPicNodeList = 
		//...
		nuFclose(pFile);
		pFile=NULL;
	}
	return nuTRUE;
}
nuBOOL CDrawReal3DPicZ::Read3DHeader(nuFILE* pFile)
{
	if( NULL == pFile )
	{
		return nuFALSE;
	}
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFread(&m_real3DHead.fileHead, sizeof(REAL3DFILEHEAD), 1, pFile) != 1 )
	{
		return nuFALSE;
	}
	m_bRead3DHead = nuTRUE;
	return nuTRUE;
}

nuVOID CDrawReal3DPicZ::Free()
{
	m_bRead3DHead = nuFALSE;
}

nuBOOL CDrawReal3DPicZ::DrawReal3DPic(nuLONG nReal3DPicID, nuINT x /* = 0 */, nuINT y /* = 0 */)
{
	//
	if( 1 == m_nUseAlp )
	{
#define _NEW_3D_FILE nuTEXT("Media\\3dPic.ALP")
	
		/*nuTCHAR tsFile[NURO_MAX_PATH] = {0};
		NURO_BMP bmp3D = {0};
		nuroResource res;
		nuBOOL bres = nuFALSE;
		nuTcscpy(tsFile,g_pDMGdata->pTsPath);
		nuTcscat(tsFile, _NEW_3D_FILE);	
		if (res.LoadPack(tsFile))
		{
			nuCHAR buf[256] = {0};
			buf[0] = '/';
			nuItoa( nReal3DPicID, &buf[1], 10);
			//nuItoa( 10240, &buf[1], 10);
			
			nuStrcat( buf,".bmp");
			nuVOID* file = res.FindFile(buf);					
			if (file)
			{
				if( res.LoadNuroBMP(&bmp3D, file) )
				{
					bmp3D.bmpType	= NURO_BMP_TYPE_COPY;
					bres = nuTRUE;
					PNURO_BMP pCvsBmp = g_pDMLibGDI->GdiGetCanvasBmp();		
					if( NULL != pCvsBmp ||
						(pCvsBmp->bmpWidth == bmp3D.bmpWidth && pCvsBmp->bmpHeight == bmp3D.bmpHeight) )
					{
						if( !g_pDMLibGDI->GdiDrawBMP(x, y, bmp3D.bmpWidth, bmp3D.bmpHeight, &bmp3D, 0, 0) )
						{
							bres = nuFALSE;
						}
					}		
					else
					{
						nuWORD* pDes = (nuWORD*)pCvsBmp->pBmpBuff;
						nuWORD* pSrc = (nuWORD*)bmp3D.pBmpBuff;
						nuINT ii, jj, iii = 0;
						for(nuWORD i = 0; i < pCvsBmp->bmpHeight; ++i)
						{
							for(nuWORD j = 0; j < pCvsBmp->bmpWidth; ++j)
							{
								jj =  j * bmp3D.bmpWidth / pCvsBmp->bmpWidth; 
								pDes[j] = pSrc[jj];
							}
							ii = i * bmp3D.bmpHeight / pCvsBmp->bmpHeight;
							if( ii > iii )
							{
								pSrc += bmp3D.bmpWidth;
								iii = ii;
							}
							pDes += pCvsBmp->bmpWidth;
						}
					}		
				}
				if( bmp3D.bmpHasLoad )
				{
					res.DeleNuroBmp( &bmp3D);
					//g_pDMLibGDI->GdiDelNuroBMP( &bmp3D );
				}	
			}
			res.UnloadPack();
		}
		//g_pDMLibGDI->GdiDelNuroBMP( &bmp3D );
		return bres;*/
	}
	else
	{	
		nuTCHAR tsFile[NURO_MAX_PATH] = {0};
		nuTcscpy(tsFile,g_pDMGdata->pTsPath);
		nuTcscat(tsFile, _REAL_3D_FILE);
		nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
		if( pFile == NULL )
		{
			return nuFALSE;
		}
		if( !m_bRead3DHead && !Read3DHeader(pFile) )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		//	PREAL3DPICNODE pPicIDList = (PREAL3DPICNODE)g_pDMLibMM->MM_GetTmpStaticMemMass(sizeof(REAL3DPICNODE)*m_real3DHead.fileHead.nIconNum);
		PREAL3DPICNODE pPicIDList = new REAL3DPICNODE[m_real3DHead.fileHead.nIconNum];
		if( pPicIDList == NULL )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		REAL3DPICNODE real3dNode = {0};
		nuUINT i = m_real3DHead.fileHead.nIconNum;
		if( nuFseek(pFile, sizeof(REAL3DFILEHEAD), NURO_SEEK_SET) == 0 && 
			nuFread(pPicIDList, sizeof(REAL3DPICNODE) * m_real3DHead.fileHead.nIconNum, 1, pFile) == 1 )
		{
			for( i = 0; i < m_real3DHead.fileHead.nIconNum; ++i )
			{
				if( nReal3DPicID == (nuLONG)pPicIDList[i].ID )
				{
					real3dNode = pPicIDList[i];
					break;
				}
			}
		}
		//	g_pDMLibMM->MM_RelStaticMemMassAfter((nuPBYTE)pPicIDList);
		delete []pPicIDList;
		pPicIDList=NULL;
		if( i == m_real3DHead.fileHead.nIconNum )
		{
			nuFclose(pFile);
			return nuFALSE;
		}
		NURO_BMP bmp3D = {0};
		nuMemset(&bmp3D, 0, sizeof(NURO_BMP));
		bmp3D.bmpWidth	= real3dNode.nW;
		bmp3D.bmpHeight	= real3dNode.nH;
		bmp3D.bmpType	= NURO_BMP_TYPE_COPY;
		bmp3D.bmpBuffLen	= bmp3D.bmpWidth * bmp3D.bmpHeight * 2;
		//	bmp3D.pBmpBuff	= (nuPBYTE)g_pDMLibMM->MM_GetTmpStaticMemMass(bmp3D.nBuffLen);
		bmp3D.pBmpBuff	= new nuBYTE[bmp3D.bmpBuffLen];	
		if( bmp3D.pBmpBuff == NULL )
		{
			nuFclose(pFile); 
			return nuFALSE;
		}
		bmp3D.bmpHasLoad = 1;
		nuBOOL bres = nuFALSE;
		if( nuFseek(pFile, real3dNode.nStartPos, NURO_SEEK_SET) == 0 &&
			nuFread(bmp3D.pBmpBuff, bmp3D.bmpBuffLen, 1, pFile) == 1 )
		{
			bres = nuTRUE;
		}
		if( bres )
		{
		    /*
		    if( !g_pDMLibGDI->GdiDrawBMP(x, y, bmp3D.bmpWidth, bmp3D.bmpHeight, &bmp3D, 0, 0) )
			{
			    bres = nuFALSE;
	     	}
	    	*/		
			PNURO_BMP pCvsBmp = g_pDMLibGDI->GdiGetCanvasBmp();
			if( NULL == pCvsBmp ||
				(pCvsBmp->bmpWidth == bmp3D.bmpWidth && pCvsBmp->bmpHeight == bmp3D.bmpHeight) )
			{
				if( !g_pDMLibGDI->GdiDrawBMP(x, y, bmp3D.bmpWidth, bmp3D.bmpHeight, &bmp3D, 0, 0) )
				{
					bres = nuFALSE;
				}
			}
			else
			{
				nuWORD* pDes = (nuWORD*)pCvsBmp->pBmpBuff;
				nuWORD* pSrc = (nuWORD*)bmp3D.pBmpBuff;
				nuINT ii, jj, iii = 0;
				for( nuWORD i = 0; i < pCvsBmp->bmpHeight; ++i )
				{
					for(nuWORD j = 0; j < pCvsBmp->bmpWidth; ++j)
					{
						jj =  j * bmp3D.bmpWidth / pCvsBmp->bmpWidth; 
						pDes[j] = pSrc[jj];
					}
					ii = i * bmp3D.bmpHeight / pCvsBmp->bmpHeight;
					if( ii > iii )
					{
						pSrc += bmp3D.bmpWidth;
						iii = ii;
					}
					pDes += pCvsBmp->bmpWidth;
				}
			}
		}
		//	g_pDMLibMM->MM_RelStaticMemMassAfter(bmp3D.pBmpBuff);
		delete[] bmp3D.pBmpBuff;
		bmp3D.pBmpBuff=NULL;
		nuFclose(pFile);
		return bres;
	}
}

nuBOOL CDrawReal3DPicZ::Draw3DRealPic(nuLONG nReal3DPicID, nuLONG nDwIdx, nuINT x /* = 0 */, nuINT y /* = 0 */)
{
	//
	nuTCHAR tsFile[NURO_MAX_PATH] = {0};
	nuTcscpy(tsFile,g_pDMGdata->pTsPath);
	nuTcscat(tsFile, _REAL_3D_FILE);
	nuFILE *pFile = nuTfopen(tsFile, NURO_FS_RB);
	if( pFile == NULL )
	{
		return nuFALSE;
	}
	REAL3DFILEHEAD real3dFileHead = {0};
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFread(&real3dFileHead, sizeof(REAL3DFILEHEAD), 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	NURO_BMP bmp3D = {0};
	nuMemset(&bmp3D, 0, sizeof(NURO_BMP));
	bmp3D.bmpWidth	= real3dFileHead.nW;
	bmp3D.bmpHeight	= real3dFileHead.nH;
	bmp3D.bmpType	= NURO_BMP_TYPE_COPY;
	bmp3D.bmpBuffLen	= bmp3D.bmpWidth * bmp3D.bmpHeight * 2;
	bmp3D.pBmpBuff	= new nuBYTE[bmp3D.bmpBuffLen];
	if( bmp3D.pBmpBuff == NULL )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	bmp3D.bmpHasLoad = 1;
	nuBOOL bres = nuFALSE;
	if( nuFseek(pFile, nReal3DPicID, NURO_SEEK_SET) == 0 &&
		nuFread(bmp3D.pBmpBuff, bmp3D.bmpBuffLen, 1, pFile) == 1 )
	{
		bres = nuTRUE;
	}
	if( bres )
	{
		PNURO_BMP pCvsBmp = g_pDMLibGDI->GdiGetCanvasBmp();
		if( NULL == pCvsBmp ||
			(pCvsBmp->bmpWidth == bmp3D.bmpWidth && pCvsBmp->bmpHeight == bmp3D.bmpHeight) )
		{
			if( !g_pDMLibGDI->GdiDrawBMP(x, y, bmp3D.bmpWidth, bmp3D.bmpHeight, &bmp3D, 0, 0) )
			{
				bres = nuFALSE;
			}
		}
		else
		{
			nuWORD* pDes = (nuWORD*)pCvsBmp->pBmpBuff;
			nuWORD* pSrc = (nuWORD*)bmp3D.pBmpBuff;
			nuINT ii, jj, iii = 0;
			for(nuWORD i = 0; i < pCvsBmp->bmpHeight; ++i)
			{
				for(nuWORD j = 0; j < pCvsBmp->bmpWidth; ++j)
				{
					jj =  j * bmp3D.bmpWidth / pCvsBmp->bmpWidth; 
					pDes[j] = pSrc[jj];
				}
				ii = i * bmp3D.bmpHeight / pCvsBmp->bmpHeight;
				if( iii > ii )
				{
					pSrc += bmp3D.bmpWidth;
					iii = ii;
				}
				pDes += pCvsBmp->bmpWidth;
			}
		}
	}
	//
	delete []bmp3D.pBmpBuff;
	bmp3D.pBmpBuff=NULL;
	nuFclose(pFile);
	return nuTRUE;
}

