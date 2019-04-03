// DrawCarXQ.cpp: implementation of the CDrawCarXQ class.
//
//////////////////////////////////////////////////////////////////////

#include "DrawCarXQ.h"
//#include "NuroClib.h"
#include "NuroModuleApiStruct.h"
#include "libMethods.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern PGDIAPI		  g_pDMLibGDI;
extern PFILESYSAPI    g_pDMLibFS;
#define  WIDTH     200
#define  HEIGHT    200
nuWORD g_RAA[WIDTH][HEIGHT];
nuWORD g_RAA1[WIDTH][HEIGHT];
nuWORD g_RBA[WIDTH][HEIGHT];
nuWORD g_RBA1[WIDTH][HEIGHT];
CDrawCarXQ::CDrawCarXQ()
{
	nuMemset(&m_bmpCar, 0, sizeof(m_bmpCar));
	m_OldCarBmpState = 0;
	m_3DMode         = nuFALSE;
	m_nOldRotAngle   = 0;
	m_nOldCarAngle   = -1;
	m_byIconType	 = 0;
}

CDrawCarXQ::~CDrawCarXQ()
{

}


nuBOOL CDrawCarXQ::Initialize()
{
	return nuTRUE;
}

nuVOID CDrawCarXQ::Free()
{
	g_pDMLibGDI->GdiDelNuroBMP(&m_bmpCar);
}

nuUINT CDrawCarXQ::LoadCarIcon(nuBYTE byIconType)
{
	nuBOOL b3DMode  = g_pDMLibFS->pGdata->uiSetData.b3DMode;
	nuSHORT nCarAngle = g_pDMLibFS->pGdata->carInfo.nCarAngle;
	nuSHORT nMapAngle = g_pDMLibFS->pGdata->carInfo.nMapAngle;

	nuINT nAngle = (nCarAngle - (nMapAngle - 90) + 360 ) % 360;
	nAngle = (nAngle + 1) % 360;
	//
	nuWORD nBmpState = 0;
	nuWORD nRoateAngle = 0;
	//
	nuUINT n = 0;
	nuUINT nOld = 0;
	//
	if( g_pDMLibFS->pGdata->uiSetData.nMapDirectMode == MAP_DIRECT_CAR )
	{
		nAngle = 90;
	}
	if( nAngle != m_nOldCarAngle )
	{
		//计算Car Icon旋转角度和象限 
		if( nAngle <= 9 )
		{
			n = 4;
			nRoateAngle = 270;
		}
		else if( nAngle <= 99 ) //第一象限
		{
			n = 1;
			nRoateAngle = 0;
		}
		else if( nAngle <= 189 ) //第二象限
		{
			n = 2;
			nRoateAngle = 90;
		}
		else if( nAngle <= 279 ) //第三象限
		{
			n = 3;
			nRoateAngle = 180;
		}
		else //第四象限
		{
			n = 4;
			nRoateAngle = 270;
		}
		//计算图片状态
		if( nAngle <= 9 )
		{
			nBmpState = 1;
		}
		if( nAngle - (n - 1) * 90 <= 27 )
		{	
			nBmpState = 5; //图片5
		}
		else if( nAngle - (n - 1) * 90 <= 45 )
		{	
			nBmpState = 4; //图片4
		}
		else if( nAngle - (n -1) * 90 <= 63 )
		{	
			nBmpState = 3; //图片3
		}
		else if( nAngle - (n -1) * 90 <= 81 )
		{	
			nBmpState = 2; //图片2
		}
		else if( nAngle - (n -1) * 90 <= 99 )
		{	
			nBmpState = 1; //图片1	
		}
		else
		{
			nBmpState = 1;
		}
		//Load Car Icon
		if( nBmpState != m_OldCarBmpState || b3DMode != m_3DMode || nRoateAngle != m_nOldRotAngle || m_byIconType != byIconType)
		{
			m_byIconType = byIconType;
			m_OldCarBmpState = nBmpState;
			m_nOldRotAngle = nRoateAngle;
			m_3DMode = b3DMode;
			//
			nuTCHAR tsFile[NURO_MAX_PATH] = {0};
			nuTcscpy(tsFile, g_pDMLibFS->pGdata->pTsPath);
			switch(m_byIconType)
			{
				case 0:
					nuTcscat(tsFile, _CAR_BMP_FILE_PATH_);
					break;
				case 1:
					nuTcscat(tsFile, _CAR_B_BMP_FILE_PATH_);
					break;
				case 2:
					nuTcscat(tsFile, _CAR_L_BMP_FILE_PATH_);
					break;
				default:
					nuTcscat(tsFile, _CAR_BMP_FILE_PATH_);
					break;
			}
			nuTcscat(tsFile, nuTEXT("Car_"));			
			if( b3DMode )
			{
				nuTcscat(tsFile, nuTEXT("3D_"));
			}
			else
			{
				nuTcscat(tsFile, nuTEXT("2D_"));
			}
			//
			if( 1 == nBmpState )
			{
				nuTcscat(tsFile, nuTEXT("1.BMP"));
			}
			else if( 2 == nBmpState )
			{
				nuTcscat(tsFile, nuTEXT("2.BMP"));
			}
			else if( 3 == nBmpState )
			{
				nuTcscat(tsFile, nuTEXT("3.BMP"));
			}
			else if( 4 == nBmpState )
			{
				nuTcscat(tsFile, nuTEXT("4.BMP"));
			}
			else if( 5 == nBmpState )
			{
				nuTcscat(tsFile, nuTEXT("5.BMP"));
			}
			else
			{
				return 0;
			}
			if( NULL != m_bmpCar.pBmpBuff )
			{
				g_pDMLibGDI->GdiDelNuroBMP(&m_bmpCar);
			}	
			if(!g_pDMLibGDI->GdiLoadNuroBMP(&m_bmpCar, tsFile))
			{
				return 0;
			}
			//
			if( 0 != nRoateAngle )
			{
				if( NULL != m_bmpCar.pBmpBuff )
				{
					RotationBmp(m_bmpCar.pBmpBuff, m_bmpCar.bmpWidth, m_bmpCar.bmpHeight, nRoateAngle);	
				}		
			}
		}		
	}
	return 1;
}

nuUINT CDrawCarXQ::DrawCar(nuLONG nCarInScreenX, nuLONG nCarInScreenY, nuBYTE byIconType)
{
	if(!LoadCarIcon(byIconType))
	{
		return 0;
	}
	//
	m_bmpCar.bmpHasLoad	    = 1;
	m_bmpCar.bmpBitCount    = NURO_BMP_BITCOUNT_16;
	m_bmpCar.bmpType        = NURO_BMP_TYPE_USE_TRANSCOLOR;
	m_bmpCar.bmpBuffLen     = m_bmpCar.bmpWidth * m_bmpCar.bmpHeight * 2;
	m_bmpCar.bytesPerLine   = 2 * m_bmpCar.bmpWidth; 
	m_bmpCar.bmpTransfColor = NURO_BMP_TRANSCOLOR_565;
	if( !m_3DMode ) //2D
	{
		g_pDMLibGDI->GdiDrawBMP(nCarInScreenX - m_bmpCar.bmpWidth/2, nCarInScreenY - m_bmpCar.bmpHeight/2, 
			m_bmpCar.bmpWidth, m_bmpCar.bmpHeight, &m_bmpCar, 0, 0);
	}
	else //3D
	{
		nuroSRECT rect = {0};
		rect.left = 0;
		rect.right = g_pDMLibFS->pGdata->uiSetData.nScreenWidth;
		rect.top = 0;
		rect.bottom = g_pDMLibFS->pGdata->uiSetData.nScreenHeight;
		rect.top += g_pDMLibFS->pGdata->uiSetData.nSkyHeight;
		nuroPOINT pt = {0};
		pt.x = nCarInScreenX;
		pt.y = nCarInScreenY;
		if( nuPtInSRect(pt, rect) )
		{
			g_pDMLibGDI->GdiDrawBMP(nCarInScreenX - m_bmpCar.bmpWidth/2, nCarInScreenY - m_bmpCar.bmpHeight/2, 
				m_bmpCar.bmpWidth, m_bmpCar.bmpHeight, &m_bmpCar, 0, 0);
		}
	}
	return 1;
}


nuUINT CDrawCarXQ::RotationBmp(nuBYTE *pBuff, nuWORD nWidth, nuWORD nHeight, nuINT nAngle)
{
	if( nAngle %90 != 0 || nAngle > 360 || nAngle < 0 )
	{
		return 0;
	}	

	
	nuMemset(g_RAA, 0, sizeof(nuWORD) * WIDTH * HEIGHT);
	nuMemset(g_RAA1, 0, sizeof(nuWORD) * WIDTH * HEIGHT);
	/*for( nuINT row1 = 0; row1 < WIDTH; row1++ )
	{
		for( nuINT column1 = 0; column1 < HEIGHT; column1++ )
		{
			a[row1][column1] = 0;
			a1[row1][column1] = 0;
		}
	}*/
	
	nuMemset(g_RBA, 0, sizeof(nuWORD) * WIDTH * HEIGHT);
	nuMemset(g_RBA1, 0, sizeof(nuWORD) * WIDTH * HEIGHT);
	/*for( nuINT row2 = 0; row2 < WIDTH; row2++ )
	{
		for( nuINT column2 = 0; column2 < HEIGHT; column2++ )
		{
			b[row2][column2] = 0;			
			b1[row2][column2] = 0;
		}
	}*/
	for( nuINT i = 0; i < nWidth; i++ )
	{
		for( nuINT j = 0; j < nHeight; j++ )
		{
			g_RAA[i][j] = *( pBuff + (i * nHeight + j) * 2 );
			g_RAA1[i][j] = *( pBuff + (i * nHeight + j) * 2 + 1 );
		}				
	}
	for( nuINT i1 = 0; i1 < nHeight; i1++ )
	{
		for( nuINT j1 = 0; j1 < nWidth; j1++ )
		{
			if( nAngle == 90 )
			{
				g_RBA[i1][j1] = g_RAA[j1][nHeight-i1-1];
				g_RBA1[i1][j1] = g_RAA1[j1][nHeight-i1-1];
			}
			else if( nAngle == 180 )
			{
				g_RBA[i1][j1] = g_RAA[nHeight-i1-1][nHeight-j1-1];
				g_RBA1[i1][j1] = g_RAA1[nHeight-i1-1][nHeight-j1-1];
			}
			else if( nAngle == 270 )
			{
				g_RBA[j1][nHeight-i1-1] = g_RAA[i1][j1];
				g_RBA1[j1][nHeight-i1-1] = g_RAA1[i1][j1];
			}
			else
			{
				return 0;
			}	
		}
	}
	for( nuINT i2 = 0; i2 < nHeight; i2++ )
	{
		for( nuINT j2 = 0; j2 < nWidth; j2++ )
		{
			*( pBuff + (i2 * nWidth + j2) * 2 ) = g_RBA[i2][j2];
			*( pBuff + (i2 * nWidth + j2) * 2 + 1 ) = g_RBA1[i2][j2];
		}
	}
	return 1;
}
