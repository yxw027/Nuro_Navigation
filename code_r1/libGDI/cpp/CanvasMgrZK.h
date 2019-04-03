// CanvasMgrZK.h: interface for the CCanvasMgrZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CANVASMGRZK_H__A664F3DB_2E0C_4B75_A2C5_B5867DC889CF__INCLUDED_)
#define AFX_CANVASMGRZK_H__A664F3DB_2E0C_4B75_A2C5_B5867DC889CF__INCLUDED_

#include "GdiBaseZK.h"
#include "DrawIcon.h"
#include "DrawPoi.h"
#include "GdiMethodZK.h"
#include "NuroClasses.h"
#include "CPngCtrl.h"

#include "nurofont.h"
#ifdef NURO_FONT_ENGINE_USE_FREETYPE
#include "nurofontengine.h"
#else
#include "GdiFontZK.h"
#endif

typedef struct tagCANVAS_LIST
{
    PNURO_CANVAS	 pNuroCvs;
    //	tagCANVAS_LIST*	 pNextCvs;
}CANVAS_LIST, *PCANVAS_LIST;
#define _GDI_CANVAS_MAX_COUNT			10


class CCanvasMgrZK : public CGdiBaseZK  
{
public:
    PNURO_CANVAS GetCanvas(nuINT nCvsIdx);
    nuBOOL SetCvsLeftTop(PNURO_CANVAS pCvs, nuINT *pLeft, nuINT* pTop);
    
public:
    CCanvasMgrZK();
    virtual ~CCanvasMgrZK();
    
    nuBOOL Initialize(PNURO_SCREEN pScreen);
	nuBOOL ReInitialize(nuWORD nWidth, nuWORD nHeight); // Added by Damon 20120615
    nuVOID Free();
    
    nuVOID CvsSet3DParam(Get3DThickProc Get3DThickFuc);
    
    nuBOOL LoadFontInfo(nuTCHAR* fontfile);
	//add by chang for load png
	//--------------------------
    nuBOOL LoadUIPng(PPNGCTRLSTRU &p_png,nuINT &PngIndex,nuTCHAR* pngfile = NULL,nuINT pngId = 0);
	nuBOOL CleanUIPng(nuINT PngIndex,nuBOOL bCleanAll);
	//--------------------------
    inline nuVOID CheckCanvas(NURO_CANVAS** canvas);
    
    nuVOID CvsInit(PNURO_CANVAS pCvs);
    nuBOOL CvsCreate(PNURO_CANVAS pCvs, nuWORD nWidth, nuWORD nHeight);
    nuVOID CvsDestroy(PNURO_CANVAS pCvs);
    
    nuINT       CvsSetDrawMode(NURO_CANVAS* canvas, nuINT mode);
    nuINT       CvsSetTextBkMode(NURO_CANVAS* canvas, nuINT mode);
    nuCOLORREF  CvsSetTextBkColor(NURO_CANVAS* canvas, nuCOLORREF color);
    nuCOLORREF  CvsSetTextColor(NURO_CANVAS* canvas, nuCOLORREF color);
    nuCOLORREF  CvsSetTextEdgeColor(NURO_CANVAS* canvas, nuCOLORREF color);
    nuBOOL      CvsSetTextUseEdgeColor(NURO_CANVAS* canvas, nuBOOL use);
    nuBOOL      CvsSetPen(NURO_CANVAS* canvas, NUROPEN& pen, NUROPEN* oldPen);
    nuBOOL      CvsSetFont(NURO_CANVAS* canvas, NUROFONT& font, NUROFONT* oldFont);
    nuBOOL      CvsSetBrush(NURO_CANVAS* canvas, NUROBRUSH& brush, NUROBRUSH* oldBrush);
    
    nuINT	CvsFillRect(NURO_CANVAS* canvas, nuINT x, nuINT y, nuINT w, nuINT h);
    nuINT	CvsPolyline(NURO_CANVAS* pCvs, const NUROPOINT* pPoint, nuINT nCount);
    nuINT	CvsPolygonEx(NURO_CANVAS* pCvs, const NUROPOINT* pPoint, nuINT nCount, nuINT nMode); 
    nuINT	CvsEllipse(NURO_CANVAS* pCvs, nuINT nLeft, nuINT nTop, nuINT nRight, nuINT nBottom); 
    
    nuINT	CvsTextOut(NURO_CANVAS* pCv, nuINT X, nuINT Y, nuWCHAR *wStr, nuUINT nCount,nuINT parent);
	nuINT	CvsDrawText(NURO_CANVAS* pCvs, nuWCHAR* wStr, nuINT nCount, NURORECT* pRect, nuUINT nFormat,nuINT parent);   
	nuINT	CvsDrawTextNew(NURO_CANVAS* pCvs, nuWCHAR* wStr, nuINT nCount, NURORECT* pRect, nuUINT nFormat,nuINT parent);   
    nuBOOL	CvsTextOutRotary(NURO_CANVAS* canvas, nuINT X, nuINT Y,  float centerscalex, float centerscaley, long RotaryAngle, nuWCHAR *wStr, nuUINT nCount,nuINT parent);
     
    
    nuBOOL	CvsPolyDashLine(NURO_CANVAS* pCvs, const NUROPOINT* pt, nuINT ptCount, nuINT nDashLen);
    nuBOOL  CvsPolyline3D(NURO_CANVAS* pCvs, const NUROPOINT* pPt, nuINT nCount); 
    nuBOOL	CvsPolygonBMP(NURO_CANVAS* pCvs, const NUROPOINT* pPt, nuINT nCount, PNURO_BMP pBmp, nuBOOL bHasPenEdge);  
    nuBOOL	CvsDrawBMP(NURO_CANVAS* canvas,nuINT nDesX, nuINT nDesY, nuINT width, nuINT height, NURO_BMP* bitmap, nuINT nSrcX, nuINT nSrcY);
	nuBOOL	CvsDrawPNG(NURO_CANVAS* canvas,nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, nuINT PngID, nuINT nSrcX, nuINT nSrcY);//Louis add 20120828
	nuBOOL	CvsDrawPNG2(NURO_CANVAS* canvas,nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, PPNGCTRLSTRU p_png, nuINT nSrcX, nuINT nSrcY);//Louis add 20120828
	nuBOOL	CvsDrawBmp2(NURO_CANVAS* canvas,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
                               , const PNURO_BMP pBmp, nuINT nSrcX, nuINT nSrcY, nuINT nSrcW, nuINT nSrcH);
	nuBOOL	CvsZoomBmp(PNURO_BMP pDesBmp,  nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight
		                       , const PNURO_BMP pSrcBmp, nuINT nSrcX, nuINT nSrcY, nuINT nSrcW, nuINT nSrcH);
    nuBOOL	CvsPaintBMP(NURO_CANVAS* canvas,nuINT nDesX, nuINT nDesY, nuINT width, nuINT height, NURO_BMP* bitmap, nuINT nSrcX, nuINT nSrcY);
    nuBOOL	CvsSaveBMP(NURO_BMP* bitmap,nuINT nDesX, nuINT nDesY, nuINT width, nuINT height, const NURO_CANVAS* canvas, nuINT nSrcX, nuINT nSrcY);

	nuBOOL  CvsBmpGradient(NURO_CANVAS* canvas, nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, const NURO_BMP* pBmp, nuINT nSrcX, nuINT nSrcY, nuINT nMode); //Added by xiaoqin @ 2012.11.12
    
    nuVOID  LockScreen();
    nuVOID  UnlockScreen();
	nuBOOL CvsShowStartupBmp();
	nuBOOL CvsLoadNuroBMP(PNURO_BMP pNuroBmp, const nuTCHAR *tsFile);
	nuVOID CvsDelNuroBMP(PNURO_BMP pNuroBmp);
    
    nuBOOL CvsFlush( nuINT nScreenX, nuINT nScreenY, nuINT nWidth, nuINT nHeight, PNURO_CANVAS pSrcCvs, nuINT nSrcX, nuINT nSrcY );
protected:
    nuINT PushCanvas(PNURO_CANVAS pCvs);
    nuINT PopCanvas(const PNURO_CANVAS pCvs);
public:
    NURO_CANVAS		m_cvsGdi;
    NURO_CANVAS		m_cvsBuffer;
	CPngCtrlLU		m_pngctrl;
protected:
    NURO_SCREEN	    m_Screen;
    CANVAS_LIST		m_pCvsList[_GDI_CANVAS_MAX_COUNT];
    nuWORD			m_nCvsCount;
    nuBOOL		m_bFlush;  
#ifdef NURO_FONT_ENGINE_USE_FREETYPE
   // nuroFontEngine m_fontmgr;
#else
    CGdiFontZK m_fontmgr;
#endif
    Get3DThickProc m_Get3DThickFuc; 
};
class CCanvasMgrAutoLock
{
public:
    CCanvasMgrAutoLock(CCanvasMgrZK& cm):_detail(cm)
    {
        _detail.LockScreen();
    }
    ~CCanvasMgrAutoLock()
    {
        _detail.UnlockScreen();
    }
private:
    CCanvasMgrZK& _detail;
};
#endif // !defined(AFX_CANVASMGRZK_H__A664F3DB_2E0C_4B75_A2C5_B5867DC889CF__INCLUDED_)
