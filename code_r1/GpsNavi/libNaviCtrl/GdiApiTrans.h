// GdiApiTrans.h: interface for the CGdiApiTrans class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIAPITRANS_H__DA423345_9C0B_4E96_A354_99382F3D0688__INCLUDED_)
#define AFX_GDIAPITRANS_H__DA423345_9C0B_4E96_A354_99382F3D0688__INCLUDED_

#include "NuroModuleApiStruct.h"
#include "NuroCommonApi.h"

#define ICONALIGN_LEFTTOP				1
#define ICONALIGN_LEFTCENTER			2
#define ICONALIGN_LEFTBOTTOM			3
#define ICONALIGN_RIGHTTOP				4
#define	ICONALIGN_RIGHTCENTER			5
#define ICONALIGN_RIGHTBOTTOM			6
#define ICONALIGN_CENTERTOP				7
#define ICONALIGN_CENTERCENTER			8
#define ICONALIGN_CENTERBOTTOM			9

class CGdiApiTrans  
{
public:
	CGdiApiTrans();
	virtual ~CGdiApiTrans();

	nuBOOL nuGdiLoadConfig();
	nuVOID nuGdiDelConfig();

	//GDI Api
	nuBOOL nuGdiInit(NURO_API_GDI* lpInNewGdi, nuPVOID lpOutGDIApi);
	nuVOID nuGdiFree();

    static nuBOOL nuGdiInitDC(nuHDC hDC, nuINT nSW, nuINT nSH);
	static nuVOID nuGdiFreeDC();
	static nuBOOL nuGdiInitBMP(nuUINT nWith, nuUINT nHeight);
	static nuVOID nuGdiFreeBMP();

	static nuBOOL nuGdiLoadNuroBMP(PNURO_BMP pNuroBmp, const nuTCHAR *tsFile);

	static nuBOOL nuGdiCreateNuroBMP(PNURO_BMP pNuroBmp);
	static nuVOID nuGdiDelNuroBMP(PNURO_BMP pNuroBmp);

	static NURO_BMP* nuGdiSelectCanvasBMP(NURO_BMP* pNuroBmp);//return is the NUROBITMAP* which is canvas had.

	static nuBOOL nuGdiDrawBMP( nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
		                const NURO_BMP* pNuroBmp, nuINT nSrcX, nuINT nSrcY );
	static nuBOOL nuGdiDrawPNG( nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, nuINT PngID, nuINT nSrcX, nuINT nSrcY );
	static nuBOOL nuGdiDrawPNG2( nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, PPNGCTRLSTRU p_png, nuINT nSrcX, nuINT nSrcY );
	static nuBOOL nuGdiDrawBMPTagColor( nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, const NURO_BMP* pNuroBmp, 
		                          nuINT nSrcX, nuINT nSrcY, nuCOLORREF color, nuINT nMode );
	static nuBOOL nuGdiSaveBMP(PNURO_BMP pNuroBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, nuINT nSrcX, nuINT nSrcY);

	static nuBOOL nuGdiFlush(nuINT nStartX, nuINT nStartY, nuINT nWidth, nuINT nHeight, nuINT nSourceX, nuINT nSourceY);//show on the screen

	static nuBOOL nuGdiPaintBMP(nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, const PNURO_BMP nuBitmap);

	static nuBOOL  nuGdiSetPen(PCNUROPEN pNuroPen);
	static nuVOID  nuGdiDelPen();
	static nuBOOL  nuGdiSetBrush(PCNUROBRUSH pNuroBrush);
	static nuVOID  nuGdiDelBrush();
	static nuBOOL  nuGdiSetFont(PNUROFONT pNuroFont);
	static nuVOID  nuGdiDelFont();

	static nuCOLORREF nuGdiSetTextColor(nuCOLORREF color);
	static nuCOLORREF nuGdiSetBKColor(nuCOLORREF color);
	static nuINT nuGdiSetBkMode(nuINT nMode);
	static nuINT nuGdiSetROP2(nuINT nMode);

	static nuVOID nuGdiFillRect(nuINT x, nuINT y, nuINT w, nuINT h);
	static nuBOOL nuGdiDrawLine(nuINT x1, nuINT y1, nuINT x2, nuINT y2, nuINT wMin, nuINT wAddMax, nuINT nMode);

	static nuBOOL nuGdiPolyline(const NUROPOINT* pPt, nuINT nCount);
	static nuBOOL nuGdiPolylineEx(const NUROPOINT* pPt, nuINT nCount, nuINT nMode);
	static nuBOOL nuGdiPolygon(const NUROPOINT* pPt, nuINT nCount);
	static nuBOOL nuGdiPolygonEx(const NUROPOINT* pPt, nuINT nCount, nuINT nMode);
	static nuBOOL nuGdiPolygonBMPEx(const NUROPOINT* pPt, nuINT nCount, PNURO_BMP pBmp, nuINT nMode, nuBOOL bHasPenEdge);

	static nuBOOL nuGdiEllipse(nuINT nLeft, nuINT nTop, nuINT nRight, nuINT nBottom);
	static nuBOOL nuGdiEllipseEx(nuINT nLeft, nuINT nTop, nuINT nRight, nuINT nBottom, nuINT nMode);

	static nuBOOL nuGdiDashLine(const NUROPOINT* pPt, nuINT nCount, nuINT nDashLen);

	static nuBOOL nuGdiExtTextOutW(nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount);
	static nuBOOL nuGdiExtTextOutWEx( nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount, nuBOOL bUseEdgeColor, 
		                              nuCOLORREF edgeColor );
	static nuBOOL nuGdiExtTextOutWRotaryEx( nuINT X,
                                            nuINT Y, 
                                            nuFLOAT startscalex,
                                            nuFLOAT startscaley,
                                            nuLONG RotaryAngle,
                                            nuWCHAR *wStr,
                                            nuUINT nCount,
                                            nuBOOL bUseEdgeColor,
                                            nuCOLORREF edgeColor );
	
	static nuINT nuGdiDrawTextW(nuWCHAR* wStr, nuINT nCount, PNURORECT pRect, nuUINT nFormat);
	static nuINT nuGdiDrawTextWEx( nuWCHAR* wStr, nuINT nCount, PNURORECT pRect, nuUINT nFormat, 
		                           nuBOOL bUseEdgeColor, nuCOLORREF edgeColor );
	static nuBOOL nuGdiDrawIcon(nuINT x, nuINT y, nuLONG Icon);
	static nuBOOL nuGdiDrawTargetFlagIcon(nuINT x, nuINT y, nuLONG Icon);
	static nuBOOL nuGdiDrawIconExtern(nuINT x, nuINT y, nuLONG Icon ,nuLONG nRotationAngle);
	static nuBOOL nuGdiDrawIconEx( nuINT x, nuINT y, nuLONG Icon, 
		nuBYTE nbmpType = NURO_BMP_TYPE_USE_TRANSCOLOR, nuBYTE nPos = ICONALIGN_LEFTTOP );
	static nuBOOL nuGdiDrawIconSp(nuINT x, nuINT y, nuLONG Icon, nuBYTE dayOrNight);	
	static nuUINT nuGdiDrawPoi(nuINT x, nuINT y, nuLONG Poi, nuUINT nRTPOSStyle, nuBOOL bSmallIcon = nuFALSE);
	static nuVOID nuGdiSet3DPara(const PFUNC_FOR_GDI pForGDI);
	static nuBOOL nuGdi3DPolyline(const NUROPOINT* pPt, nuINT nCount);
	static PNURO_BMP nuGdiGetCanvasBmp();
	static nuBOOL nuGdiBmpGradient( nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, 
		                            const NURO_BMP* pBmp, nuINT nSrcX, nuINT nSrcY, nuINT nMode );
	static nuBOOL nuGdiZoomBmp(PNURO_BMP pDesBmp, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
			                      , const PNURO_BMP pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT nWidthSrc, nuINT nHeightSrc);
	static PNURO_CANVAS nuGdiGetCanvas();

protected:
	NURO_API_GDI    m_GdiApiForNew;

public:
	static PNURO_CANVAS g_pCanvas;
};

#endif // !defined(AFX_GDIAPITRANS_H__DA423345_9C0B_4E96_A354_99382F3D0688__INCLUDED_)N

