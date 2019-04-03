#pragma once

#include "NuroDefine.h"
#include "NuroConstDefined.h"
extern PGDIAPI         g_pDMLibGDI;
extern PMEMMGRAPI      g_pDMLibMM;
extern PFILESYSAPI     g_pDMLibFS;
extern PMATHTOOLAPI    g_pDMLibMT;
extern PGLOBALENGINEDATA   g_pDMGdata;
extern PMAPCONFIG          g_pDMMapCfg;
nuVOID DrawDebugCoorText(nuBOOL bMapCoor, nuWCHAR *wsText, nuLONG X, nuLONG Y, nuLONG Top, nuLONG Left, nuLONG Bottom, nuLONG Right)
{
	nuLONG ptX = X, ptY = Y;
	if(bMapCoor)
	{
		g_pDMLibMT->MT_MapToBmp(X, Y, &ptX, &ptY);
	}
	/*else
	{
		g_pDMLibMT->MT_BmpToMap(X, Y, &ptX, &ptY);
		g_pDMLibMT->MT_Bmp2Dto3D(ptX, ptY,MAP_DEFAULT);
	}*/
	
	NUROFONT lf = {0};
	nuCOLORREF crTx = {0}, crBk = {0};
	nuWCHAR wsTempX[10]  = {0}, wsTempY[10]  = {0};
	nuWCHAR wsWrite[30] = {0};
	nuWcscpy(wsWrite, wsText);
	nuroRECT tmpRect  = {0};
	lf.lfHeight = 25;
	lf.lfWidth = 15;
	lf.lfWeight = 1;
	lf.lfEscapement = 0;
	g_pDMLibGDI->GdiSetFont(&lf);
	crTx = nuRGB( 255,
	  255,
	  255 );
	g_pDMLibGDI->GdiSetTextColor(crTx);
	crBk = nuRGB( 255,
	  255,
	  255 );

	tmpRect.left   = Left;
	tmpRect.top    = Top; 
	tmpRect.right  = Right + Left+50;
	tmpRect.bottom = Bottom + Top;
	 
	nuItow(ptX, wsTempX, 10);
	nuItow(ptY, wsTempY, 10);
	nuWcscat(wsWrite, (nuWCHAR*)nuTEXT("("));
	nuWcscat(wsWrite, wsTempX);
	nuWcscat(wsWrite, (nuWCHAR*)nuTEXT(","));
	nuWcscat(wsWrite, wsTempY);
	nuWcscat(wsWrite, (nuWCHAR*)nuTEXT(")"));
	g_pDMLibGDI->GdiDrawTextWEx(wsWrite, -1, &tmpRect, 41, 0, 0);
};
nuVOID DrawDebugText(nuWCHAR *wsText, nuLONG Value, nuLONG Top, nuLONG Left, nuLONG Bottom, nuLONG Right)
{	
	NUROFONT lf = {0};
	nuCOLORREF crTx = {0}, crBk = {0};
	nuWCHAR wsTemp[10]  = {0};
	nuWCHAR wsWrite[30] = {0};
	nuWcscpy(wsWrite, wsText);
	nuroRECT tmpRect  = {0};
	lf.lfHeight = 25;
	lf.lfWidth = 10;
	lf.lfWeight = 1;
	lf.lfEscapement = 0;
	g_pDMLibGDI->GdiSetFont(&lf);
	crTx = nuRGB( 0,
	  0,
	  0 );
	g_pDMLibGDI->GdiSetTextColor(crTx);
	crBk = nuRGB( 0,
	  0,
	  0 );

	tmpRect.left   = Left;
	tmpRect.top    = Top; 
	tmpRect.right  = Right + Left;
	tmpRect.bottom = Bottom + Top;
	 
	nuItow(Value, wsTemp, 10);
	//nuWcscat(wsWrite, nuTEXT(" : "));
	nuWcscat(wsWrite, wsTemp);
	g_pDMLibGDI->GdiDrawTextWEx(wsWrite, -1, &tmpRect, 400, 0, 0);
};

nuVOID DrawDebugText1(nuWCHAR *wsText, nuLONG Top, nuLONG Left, nuLONG Bottom, nuLONG Right)
{	
	NUROFONT lf = {0};
	nuCOLORREF crTx = {0}, crBk = {0};
	nuWCHAR wsTemp[10]  = {0};
	nuWCHAR wsWrite[30] = {0};
	nuWcscpy(wsWrite, wsText);
	nuroRECT tmpRect  = {0};
	lf.lfHeight = 25;
	lf.lfWidth = 10;
	lf.lfWeight = 1;
	lf.lfEscapement = 0;
	g_pDMLibGDI->GdiSetFont(&lf);
	crTx = nuRGB( 0,
	  0,
	  0 );
	g_pDMLibGDI->GdiSetTextColor(crTx);
	crBk = nuRGB( 0,
	  0,
	  0 );

	tmpRect.left   = Left;
	tmpRect.top    = Top; 
	tmpRect.right  = Right + Left;
	tmpRect.bottom = Bottom + Top;
	 
	//nuItow(Value, wsTemp, 10);
	//nuWcscat(wsWrite, nuTEXT(" : "));
	nuWcscat(wsWrite, wsTemp);
	g_pDMLibGDI->GdiDrawTextWEx(wsWrite, -1, &tmpRect, 400, 0, 0);
};

nuVOID DrawCross(nuroPOINT point)
{
	nuroPOINT pt[12] = {0};
    NUROPEN nuPen = {0};
    NUROBRUSH nuBrush = {0};
    
    if( point.y < 0 || point.y > g_pDMGdata->transfData.nBmpHeight )
    {
        return;
    }
    if( g_pDMGdata->uiSetData.b3DMode )
    {
       // g_pDMLibMT->MT_Bmp2Dto3D(point.x, point.y, MAP_DEFAULT);
    }
    point.x -= g_pDMGdata->transfData.nBmpLTx;
    point.y -= g_pDMGdata->transfData.nBmpLTy;
    pt[10].x = pt[9].x = point.x - 3;
    pt[0].x = pt[11].x = pt[8].x = pt[7].x = point.x - 1;
    pt[1].x = pt[2].x = pt[5].x = pt[6].x = point.x + 1;
    pt[3].x = pt[4].x = point.x + 3;
    pt[0].y = pt[1].y = point.y - 3;
    pt[10].y = pt[11].y = pt[2].y = pt[3].y = point.y - 1;
    pt[9].y = pt[8].y = pt[5].y = pt[4].y = point.y + 1;
    pt[7].y = pt[6].y = point.y + 3;
    //
    nuPen.nRed		= 0;
    nuPen.nGreen	= 255;
    nuPen.nBlue		= 0;
    nuPen.nStyle	= NURO_PS_SOLID;
    nuPen.nWidth	= 1;
    g_pDMLibGDI->GdiSetPen(&nuPen);
    nuBrush.nRed	= 0;
    nuBrush.nGreen	= 0;
    nuBrush.nBlue	= 255;
    g_pDMLibGDI->GdiSetBrush(&nuBrush);
    g_pDMLibGDI->GdiPolygon(pt, 12);
    //	g_pDMLibGDI->GdiEllipse(point.x - 5, point.y - 5, point.x + 5, point.y + 5);
    /*
    g_pDMLibMT->MT_MapToBmp(g_pDMGdata->carInfo.ptCarFixed.x, g_pDMGdata->carInfo.ptCarFixed.y, &point.x, &point.y);
    if( g_pDMGdata->uiSetData.b3DMode )
    {
    g_pDMLibMT->MT_Bmp2Dto3D(point.x, point.y, MAP_DEFAULT);
    }
    pt[10].x = pt[9].x = point.x - 3;
    pt[0].x = pt[11].x = pt[8].x = pt[7].x = point.x - 1;
    pt[1].x = pt[2].x = pt[5].x = pt[6].x = point.x + 1;
    pt[3].x = pt[4].x = point.x + 3;
    pt[0].y = pt[1].y = point.y - 3;
    pt[10].y = pt[11].y = pt[2].y = pt[3].y = point.y - 1;
    pt[9].y = pt[8].y = pt[5].y = pt[4].y = point.y + 1;
    pt[7].y = pt[6].y = point.y + 3;
    //
    nuPen.nRed		= 255;
    nuPen.nGreen	= 0;
    nuPen.nBlue		= 0;
    nuPen.nStyle	= NURO_PS_SOLID;
    nuPen.nWidth	= 1;
    g_pDMLibGDI->SetGdiPen(&nuPen);
    nuBrush.nRed	= 0;
    nuBrush.nGreen	= 0;
    nuBrush.nBlue	= 255;
    g_pDMLibGDI->SetGdiBrush(&nuBrush);
    g_pDMLibGDI->Polygon(pt, 12);*/
}
nuVOID DrawDebugCenterLine(nuLONG DividLen)
{
	nuroPOINT CorssPt = {0};
	CorssPt.x =  g_pDMLibFS->pGdata->transfData.nBmpWidth / 2;
	for(nuINT i = 1; i < DividLen; i++)
	{
		CorssPt.y = g_pDMLibFS->pGdata->transfData.nBmpHeight / DividLen * i;
		DrawCross(CorssPt);
	}
};
nuVOID DrawDebugCenterLineWidth(nuLONG DividLen)
{
	nuroPOINT CorssPt = {0};
	CorssPt.y =  g_pDMLibFS->pGdata->transfData.nBmpHeight / 2;
	for(nuINT i = 1; i < DividLen; i++)
	{
		CorssPt.x = g_pDMLibFS->pGdata->transfData.nBmpWidth / DividLen * i;
		DrawCross(CorssPt);
	}
};


/*nuVOID GetObjPara(nuLONG lSizeCase)
{
	if(lSizeCase <= 55)//FULL HD
	{
		switch(lSizeCase)
		{
		case 50:
			break;
		case 55:
			break;
		}
	}
	else if(lSizeCase <= 95)//WXGA
	{
		switch(lSizeCase)
		{
		case 70:
			break;
		case 75:
			break;
		case 80:
			break;
		case 85:
			break;
		case 90:
			break;
		case 95:
			break;
		}
	}
	else if(lSizeCase <= 130)//HD
	{
		switch(lSizeCase)
		{
		case 100:
			break;
		case 105:
			break;
		case 110:
			break;
		case 115:
			break;
		case 120:
			break;
		case 125:
			break;
		case 130:
			break;
		}
	}
	else if(lSizeCase <= 150)//WVGA
	{
		switch(lSizeCase)
		{
		case 135:
			break;
		case 140:
			break;
		case 145:
			break;
		case 150:
			break;
		}
	}
	else if(lSizeCase <= 155)//HVGA
	{
		switch(lSizeCase)
		{
		case 155:
			break;
		}
	}
	else if(lSizeCase <= 160)//QVGA
	{
		switch(lSizeCase)
		{
		case 160:
			break;
		}
	}
};

nuVOID FindViewSize(nuLONG Width, nuLONG Hight, )
{
	nuLONG lSizeCase = -1;
	if(1848 == Width && 1200 == Hight)
	{
		lSizeCase = 50;
	}
	else if(1776 == Width && 1080 == Hight)
	{
		lSizeCase = 55;
	}
	else if(1280 == Width && 800 == Hight)
	{
		lSizeCase = 70;
	}
	else if(1280 == Width && 720 == Hight)
	{
		lSizeCase = 75;
	}
	else if(1232 == Width && 800 == Hight)
	{
		lSizeCase = 80;
	}
	else if(1216 == Width && 800 == Hight)
	{
		lSizeCase = 85;
	}
	else if(1205 == Width && 800 == Hight)
	{
		lSizeCase = 90;
	}
	else if(1184 == Width && 720 == Hight)
	{
		lSizeCase = 95;
	}
	else if(1024 == Width && 768 == Hight)
	{
		lSizeCase = 100;
	}
	else if(1024 == Width && 600 == Hight)
	{
		lSizeCase = 105;
	}
	else if(1024 == Width && 480 == Hight)
	{
		lSizeCase = 110;
	}
	else if(976 == Width && 600 == Hight)
	{
		lSizeCase = 115;
	}
	else if(960 == Width && 540 == Hight)
	{
		lSizeCase = 120;
	}
	else if(960 == Width && 480 == Hight)
	{
		lSizeCase = 125;
	}
	else if(888 == Width && 540 == Hight)
	{
		lSizeCase = 130;
	}
	else if(854 == Width && 480 == Hight)
	{
		lSizeCase = 135;
	}
	else if(800 == Width && 480 == Hight)
	{
		lSizeCase = 140;
	}
	else if(728 == Width && 480 == Hight)
	{
		lSizeCase = 145;
	}
	else if(640 == Width && 480 == Hight)
	{
		lSizeCase = 150;
	}
	else if(480 == Width && 320 == Hight)
	{
		lSizeCase = 155;
	}
	else if(320 == Width && 240 == Hight)
	{
		lSizeCase = 160;
	}
	GetObjPara(lSizeCase);
};*/
