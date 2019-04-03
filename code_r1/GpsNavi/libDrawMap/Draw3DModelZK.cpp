// Draw3DModelZK.cpp: implementation of the CDraw3DModelZK class.
//
//////////////////////////////////////////////////////////////////////

#include "Draw3DModelZK.h"
#include "GApiOpenResource.h"
#include "NuroClasses.h"

const nuTCHAR _ZK_C_3D_MODEL_FOLDER[] = nuTEXT("Model\\");
extern PGLOBALENGINEDATA   g_pDMGdata;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDraw3DModelZK::CDraw3DModelZK()
{
	//////////////////////////////////////////////////////////////////
	//  Before the 'main' function begins, it is more stable to 
	//  use the static initialization than to use 'nuMemset' function.
	//  
	//  Fixed by Dengxu @ 2012 11 05
	//////////////////////////////////////////////////////////////////
	//nuMemset(&m_apiRM, 0, sizeof(m_apiRM));
	m_apiRM.RM_AddModel = NULL;
	m_apiRM.RM_DelModel = NULL;
	m_apiRM.RM_DrawBitmap = NULL;
	m_apiRM.RM_SetViewOption = NULL;
	m_apiRM.RM_StartRenderModel = NULL;
    
	m_bOpenGLStart = nuFALSE;
	//////////////////////////////////////////////////////////////////////////
	// Added by Dengxu @ 2012 11 05
#ifdef USING_FILE_SETTING_3D_PARAM_VIEW
	m_fLookAtX = 0.0f;
	m_fLookAtY = 0.0f;
	m_fLookAtZ = 0.0f;
#endif
}

CDraw3DModelZK::~CDraw3DModelZK()
{
    Free();
}

nuBOOL CDraw3DModelZK::Inialize(const nuTCHAR* pTzPath, nuPVOID pRsApi, PMATHTOOLAPI pLibMT /*= NULL*/)
{
    nuMemset( m_3dModelList, 0, sizeof(m_3dModelList) );
    m_nNowCount = 0;
    m_pTzPath = pTzPath;
    m_pLibMT	= pLibMT;

	m_pRsApi    = (POPENRSAPI) pRsApi;

	//////////////////////////////////////////////////////////////////////////
	// Added by Dengxu @ 2012 11 05
#ifdef USING_FILE_SETTING_3D_PARAM_VIEW
	
	nuLONG nLookAtX;
	nuLONG nLookAtY;
	nuLONG nLookAtZ;

	//if (nuFALSE == nuReadConfigValue("3D_LOOK_AT_X", &nLookAtX))
	{
		nLookAtX = 0;
	}

	//if (nuFALSE == nuReadConfigValue("3D_LOOK_AT_Y", &nLookAtY))
	{
		nLookAtY = 172;
	}

	//if (nuFALSE == nuReadConfigValue("3D_LOOK_AT_Z", &nLookAtZ))
	{
		nLookAtZ = 71;
	}

	m_fLookAtX = static_cast<nuFLOAT>(nLookAtX);
	m_fLookAtY = static_cast<nuFLOAT>(-nLookAtY);
	m_fLookAtZ = static_cast<nuFLOAT>(nLookAtZ);

#endif

    return nuTRUE;
}

nuVOID CDraw3DModelZK::Free()
{
    m_loadRM.FreeModuleH();
    m_bOpenGLStart = nuFALSE;
}

nuUINT CDraw3DModelZK::StartOpenGLes()
{
    if( m_bOpenGLStart )
    {
        return 1;
    }
    if( !m_loadRM.LoadModule(&m_apiRM, m_pRsApi) )
    {
        return 0;
    }
	PARAMETER_START_OPTION paramStart = {0};
    paramStart.sysDiff	= m_sysDiff;
    nuMemcpy(&paramStart.rtScreen, &m_rtScreen, sizeof(NURORECT));
    nuUINT nRes = m_apiRM.RM_StartRenderModel(&paramStart);
    if( nRes )
    {
        m_bOpenGLStart = nuTRUE;
    }
    return nRes;
}

nuUINT CDraw3DModelZK::StartOpenGL(SYSDIFF_SCREEN sysDiff, const nuroRECT* pRtScreen)
{
    m_rtScreen = *pRtScreen;
	if( m_bOpenGLStart )
	{
		PARAMETER_START_OPTION paramStart = {0};
		paramStart.sysDiff	= sysDiff;
		nuMemcpy(&paramStart.rtScreen, &m_rtScreen, sizeof(NURORECT));
		nuUINT nRes = m_apiRM.RM_StartRenderModel(&paramStart);
	}
	else
	{
		m_sysDiff = sysDiff;
	}
    return 1;
}

nuUINT CDraw3DModelZK::Check3DModel(PNURORECT pRtMap)
{
    if( !m_bOpenGLStart )
    {
        nuTCHAR tsFile[NURO_MAX_PATH] = {0};
        nuTcscpy(tsFile, m_pTzPath);
        nuTcscat(tsFile, _ZK_C_3D_MODEL_FOLDER);
        nuTcscat(tsFile, nuTEXT("TW.3DM"));
        if( !m_file3dm.Read3dmCoor(tsFile) )
        {
            return 0;
        }
        StartOpenGLes();
    }
    RemoveModelOut(pRtMap);
    SeekForModel(pRtMap);
    //AddModelToList(1, 0, 0, pRtMap, "TW_KW0417.3DR");
    return 1;
}

#ifdef USING_FILE_SETTING_3D_PARAM_VIEW
// 3D View file setting
nuUINT CDraw3DModelZK::SetViewOption(nuSHORT nMapAngle, nuFLOAT fScale, nuPVOID lpApiMT, nuLONG w, nuLONG h)
{
	if( !m_bOpenGLStart )
	{
		return 0;
	}
	m_pLibMT->MT_Get3DParam(&m_3d_param);

	PARAMETER_VIEW_OPTION paramVeiw = {0};


	paramVeiw.fAngle	=  (90 - nMapAngle) * 1.0f;//nAngle * 1.0f; +180
	paramVeiw.fScale	= fScale * _3D_SCALE_SET_;

	paramVeiw.fLookAtX = m_fLookAtX;
	paramVeiw.fLookAtY = m_fLookAtY;
	paramVeiw.fLookAtZ = m_fLookAtZ;

	paramVeiw.fLookToX	= 0;
	paramVeiw.fLookToY	= 0;
	paramVeiw.fLookToZ	= 0;

	paramVeiw.fUpX		= 0.0f;
	paramVeiw.fUpY		= 0.0f;
	paramVeiw.fUpZ		= 1.0f;

	//2012-11-29 modified by kris -Begin  
    paramVeiw.fLookAtX	= w/2;
    paramVeiw.fLookAtY	= -50;
    paramVeiw.fLookAtZ	= 100;
    paramVeiw.fLookToX	= w/2;
    paramVeiw.fLookToY	= 120;
    paramVeiw.fLookToZ	= 0;
	//2012-11-29 modified by kris -End  

	if (w && h)
	{
		paramVeiw.screenW = w;
		paramVeiw.screenH = h;
		paramVeiw.fScale = paramVeiw.fScale*272/h;
	}
	else
	{
		paramVeiw.screenW = 480;
		paramVeiw.screenH = 272;
	}

	m_apiRM.RM_SetViewOption(&paramVeiw);
	AddModelToOpenGL(&paramVeiw);
	return 1;
}

#else

nuUINT CDraw3DModelZK::SetViewOption(nuSHORT nMapAngle, nuFLOAT fScale, nuPVOID lpApiMT, nuLONG w, nuLONG h)
{
    if( !m_bOpenGLStart )
    {
        return 0;
    }
    m_pLibMT->MT_Get3DParam(&m_3d_param);	
    PARAMETER_VIEW_OPTION paramVeiw;
    paramVeiw.fAngle	=  (90 - nMapAngle) * 1.0f;//nAngle * 1.0f; +180
    paramVeiw.fScale	= fScale * _3D_SCALE_SET_;  
    
	paramVeiw.fLookAtX	= 0;//(nuFLOAT)(m_3d_param.nLookAtx - m_3d_param.nLookTox);
	//     paramVeiw.fLookAtY	= -128;//(nuFLOAT)(m_3d_param.nLookToy - m_3d_param.nLookAty * 11 / 8);
	//     paramVeiw.fLookAtZ	= 80;//m_3d_param.nLookAtz
	paramVeiw.fLookAtY	= -200;//(nuFLOAT)(m_3d_param.nLookToy - m_3d_param.nLookAty * 11 / 8);//-128;//
	paramVeiw.fLookAtZ	= 80;//m_3d_param.nLookAtz;//82;
    
	paramVeiw.fLookToX	= 0;
    paramVeiw.fLookToY	= 0;
    paramVeiw.fLookToZ	= 0;
    
	paramVeiw.fUpX		= 0.0f;
    paramVeiw.fUpY		= 0.0f;
    paramVeiw.fUpZ		= 1.0f;
    if (w && h)
    {
        paramVeiw.screenW = w;
        paramVeiw.screenH = h;
        paramVeiw.fScale = paramVeiw.fScale*272/h;
    }
    else
    {
        paramVeiw.screenW = 480;
        paramVeiw.screenH = 272;
    }
    m_apiRM.RM_SetViewOption(&paramVeiw);
    AddModelToOpenGL(&paramVeiw);
    return 1;
}

#endif

//#define SMEM
nuUINT CDraw3DModelZK::Draw3DLayer(nuINT nDesX, nuINT nDesY, nuINT nWidth, nuINT nHeight, const PNURO_BMP pBitmap)
{
    if( !m_bOpenGLStart )
    {
		return 0;
    }
    nWidth	= m_rtScreen.right - m_rtScreen.left;
    nHeight = m_rtScreen.bottom - m_rtScreen.top;
    const nuWORD clr = 0xF800;
    nuINT nSrcX = 0;
    nuINT nSrcY = 0;
#ifdef SMEM
    static PARAMETER_BITMAP paramBmp;
#else
    PARAMETER_BITMAP paramBmp;
#endif
    paramBmp.nType		= 1;
    paramBmp.nWidth		= (nuWORD)(nWidth);
    paramBmp.nHeight	= (nuWORD)(nHeight);
    paramBmp.nBuffLen	= paramBmp.nWidth * paramBmp.nHeight * 2;
    if( !AdjustPosition(nDesX, nDesY, nWidth, nHeight, nSrcX, nSrcY, paramBmp.nWidth, paramBmp.nHeight, pBitmap->bmpWidth, pBitmap->bmpHeight) )
    {
        return 0;
    }
#ifdef SMEM
    if (!paramBmp.pBitsBuff)
    {
#endif
        paramBmp.pBitsBuff	= new nuBYTE[paramBmp.nBuffLen];
#ifdef SMEM
    }
#endif
    if( NULL == paramBmp.pBitsBuff )
    {
        return 0;
    }
    m_apiRM.RM_DrawBitmap(&paramBmp);
    nuWORD *pClrDes = (nuWORD*)pBitmap->pBmpBuff;
    nuWORD *pClrSrc = (nuWORD*)paramBmp.pBitsBuff;
    pClrDes += nDesY * pBitmap->bmpWidth;
    pClrSrc += (paramBmp.nHeight - 1 - nSrcY) * paramBmp.nWidth;
    for( nuINT i = 0; i < nHeight; ++i )
    {
        for( nuINT j = 0; j < nWidth; ++j )
        {
            if( pClrSrc[j+nSrcX] != clr )
            {
                pClrDes[j+nDesX] = pClrSrc[j+nSrcX];
            }
        }
        pClrDes += pBitmap->bmpWidth;
        pClrSrc -= paramBmp.nWidth;
    }
    
#ifdef SMEM
#else
    delete[] paramBmp.pBitsBuff;
	paramBmp.pBitsBuff = NULL;
#endif
    return 1;
}

inline nuBOOL CDraw3DModelZK::AdjustPosition( nuINT& nDesX,
                                              nuINT& nDesY,
                                              nuINT& nWidth,
                                              nuINT& nHeight,
                                              nuINT& nSrcX,
                                              nuINT& nSrcY,
                                              nuINT nDesW,
                                              nuINT nDesH,
                                              nuINT nSrcW,
                                              nuINT nSrcH )
{
    if( nDesX < 0 )
    {
        nWidth += nDesX;
        nSrcX -= nDesX;
        nDesX = 0;
    }
    if( nDesY < 0 )
    {
        nHeight += nDesY;
        nSrcY -= nDesY;
        nDesY = 0;
    }
    if( nSrcX < 0 )
    {
        nWidth += nSrcX;
        nDesX -= nSrcX;
        nSrcX = 0;
    }
    if( nSrcY < 0 )
    {
        nHeight += nSrcY;
        nDesY -= nSrcY;
        nSrcY = 0;
    }
    if( nWidth < 0	||
        nHeight < 0 ||
        nDesX >= nDesW	||
        nDesY >= nDesH	||
        nSrcX >= nSrcW	||
        nSrcY >= nSrcH )
    {
        return nuFALSE;
    }
    nuINT nTmp = 0;
    if( nDesX + nWidth > nDesW )
    {
        nTmp = nDesX + nWidth - nDesW;
        nWidth -= nTmp;
    }
    if( nSrcX + nWidth > nSrcW )
    {
        nTmp = nSrcX + nWidth - nSrcW;
        nWidth -= nTmp;
    }
    if( nWidth <= 0 )
    {
        return nuFALSE;
    }
    if( nDesY + nHeight > nDesH )
    {
        nTmp = nDesY + nHeight - nDesH;
        nHeight -= nTmp;
    }
    if( nSrcY + nHeight > nSrcH )
    {
        nTmp = nSrcY + nHeight - nSrcH;
        nHeight -= nTmp;
    }
    if( nHeight <= 0 )
    {
        return nuFALSE;
    }
    return nuTRUE;
}

inline nuVOID CDraw3DModelZK::RemoveModelOut(PNURORECT pRtMap)
{
    nuINT j = -1;
    for( nuINT i = 0; i < m_nNowCount; ++i )
    {
		if( !nuRectCoverRect( &m_3dModelList[i].rtBoundary, pRtMap ) )
        {
            m_apiRM.RM_DelModel( m_3dModelList[i].nID, m_3dModelList[i].x, m_3dModelList[i].y );
            if( -1 == j )
            {
                j = i;
            }
			else
			{
				j++;
			}
        }
        else
        {
            if( -1 != j )
            {
                m_3dModelList[j] = m_3dModelList[i];
                ++j;
            }
        }
 	}
    if( -1 != j )
    {        
        m_3dModelList[m_nNowCount-1].nID = 0;
        m_nNowCount = j;
    }
}

inline nuBOOL CDraw3DModelZK::SeekForModel(PNURORECT pRtMap)
{
	/*OPR_PACK* nPackID;
	nuTCHAR tzPack[NURO_MAX_PATH] = {0};
	nuTcscpy(tzPack, m_pTzPath);
	//nuTcscat(tzPack, nuTEXT("Model.pck"));
	nuUINT nTotalLen;
	nPackID = CNrResourceBase::LoadPackageEx(nuTEXT("Model.pck"), tzPack);
	//nPackID = m_pApiOpenRs->IOprLoadPack(tzPack, m_pApiOpenRs->LOAD_PACK_FILE, nTotalLen);
	if( nPackID == NULL )
	{
		return nuFALSE;
	}*/
	//
    if( NULL == pRtMap ||
        NULL == m_file3dm.m_pPtCoor )
    {
        return nuFALSE;
    }
    nuroRECT rtBoundary = {0};
    nuCHAR sName[56] = {0};
    nuTCHAR tsFile[NURO_MAX_PATH] = {0};
    nuTcscpy(tsFile, m_pTzPath);
    nuTcscat(tsFile, _ZK_C_3D_MODEL_FOLDER);
    nuTcscat(tsFile, nuTEXT("TW.3DM"));
    for(nuDWORD i = 0; i < m_file3dm.m_nTotalCount; ++i)
    {
        rtBoundary.left		= m_file3dm.m_pPtCoor[i].x - 100;
        rtBoundary.right	= m_file3dm.m_pPtCoor[i].x + 100;
        rtBoundary.top		= m_file3dm.m_pPtCoor[i].y - 100;
        rtBoundary.bottom	= m_file3dm.m_pPtCoor[i].y + 100;
        if( nuRectCoverRect(&rtBoundary, pRtMap) &&
            m_file3dm.Read3drName(tsFile, i, sName, 56) )
        {
            nuStrcat(sName, ".3DR");
            AddModelToList( i+1,
                m_file3dm.m_pPtCoor[i].x,
                m_file3dm.m_pPtCoor[i].y,
                &rtBoundary,
                sName );
        }
    }
    return nuTRUE;
}
inline nuBOOL CDraw3DModelZK::AddModelToList( const nuLONG& nID,
                                              const nuLONG& x,
                                              const nuLONG& y,
                                              nuroRECT* prtBoundary,
                                              const nuCHAR* pszName )
{
    for( nuINT i = 0; i < m_nNowCount; ++i )
    {
        if( nID == m_3dModelList[i].nID )
        {
            m_3dModelList[i].x	= x;
            m_3dModelList[i].y	= y;
            return nuFALSE;
        }
    }
    if( _3D_MODEL_SHOW_MAX == m_nNowCount )
    {
        return nuTRUE;
    }
    m_3dModelList[m_nNowCount].nID	= nID;
    m_3dModelList[m_nNowCount].x	= x;
    m_3dModelList[m_nNowCount].y	= y;
    m_3dModelList[m_nNowCount].rtBoundary	= *prtBoundary;
    nuStrncpy( m_3dModelList[m_nNowCount].szName, pszName, _3D_MODEL_NAME_MAX_NUM );
    ++m_nNowCount;
    return nuFALSE;
}

inline nuVOID CDraw3DModelZK::AddModelToOpenGL(PARAMETER_VIEW_OPTION* op)
{  
    if (m_3d_param.bScale)
    {  
        nuFLOAT fScalex = 0;
        nuFLOAT fScaley = 0;
        nuLONG x = 0;
        nuLONG y = 0;
        nuFLOAT ffixs = (nuFLOAT)(op->fScale/0.96);
		nuroPOINT point = {0};
        nuFLOAT fz = (op->fLookAtY-op->fLookToY)/(op->fLookAtZ-op->fLookToZ);
        if (fz<0)
        {
            fz = -fz;
        }
        for(nuINT i = 0; i < m_nNowCount; ++i)
        {
            m_pLibMT->MT_MapToBmp( m_3dModelList[i].x, m_3dModelList[i].y, &point.x, &point.y);


			//2012-11-29 modified by kris -Begin
			nuFLOAT fY_2d = 0; //螢幕相對座標y
			fY_2d=(480-point.y)-112;
			nuFLOAT fY_sft=0;//螢幕位移座標y

			//[-17,43],[-12,47],[-5,51],[43,81],[72,99],[110,120],[372,244],[449,272]
			if(fY_2d<-17)						fY_sft= 40+( 43- 40)*(fY_2d+ 19)/(-12+ 17);
			if((fY_2d>=-17)&&(fY_2d<-12))		fY_sft= 43+( 47- 43)*(fY_2d+ 17)/(-12+ 17);
			if((fY_2d>=-12)&&(fY_2d< -5))		fY_sft= 47+( 51- 47)*(fY_2d+ 12)/( -5+ 12);
			if((fY_2d>= -5)&&(fY_2d< 43))		fY_sft= 51+( 81- 51)*(fY_2d+  5)/( 43+  5);		
			if((fY_2d>= 43)&&(fY_2d< 72))		fY_sft= 81+( 99- 81)*(fY_2d- 43)/( 72- 43);
			if((fY_2d>= 72)&&(fY_2d<110))		fY_sft= 99+(120- 99)*(fY_2d- 72)/(110- 72);		
			if((fY_2d>=110)&&(fY_2d<372))		fY_sft=120+(244-120)*(fY_2d-110)/(372-110);
			if((fY_2d>=372)&&(fY_2d<500))		fY_sft=244+(272-244)*(fY_2d-372)/(449-372);
			if((fY_2d>=500)&&(fY_2d<=1500))		fY_sft=272+(272-244)*(fY_2d-500)/(449-372);//
		
			nuFLOAT fX_sft =(point.x-(op->screenW/2)) /2.2f ;//螢幕位移座標x
			if((fY_2d>=-17)&&(fY_2d<-12))			fX_sft=fX_sft*1.18f;
			if((fY_2d>=-12)&&(fY_2d< -2))			fX_sft=fX_sft*1.16f;
			if((fY_2d>= -2)&&(fY_2d< 8))			fX_sft=fX_sft*1.14f;
			if((fY_2d>= 8 )&&(fY_2d< 20))			fX_sft=fX_sft*1.12f;
			if((fY_2d>= 20)&&(fY_2d< 43))			fX_sft=fX_sft*1.11f;
			if((fY_2d>= 43)&&(fY_2d< 72))			fX_sft=fX_sft*1.09f;//兩邊ok
			if((fY_2d>= 72)&&(fY_2d<110))			fX_sft=fX_sft*1.05f;//兩邊ok
			if((fY_2d>=110)&&(fY_2d<150))			fX_sft=fX_sft*1.03f;//兩邊ok
			if((fY_2d>=150)&&(fY_2d<200))			fX_sft=fX_sft*1.00f;
			if((fY_2d>=200)&&(fY_2d<220))			fX_sft=fX_sft*0.97f;//左太內,右太外
			if((fY_2d>=220)&&(fY_2d<250))			fX_sft=fX_sft*0.94f;//兩邊ok
			if((fY_2d>=250)&&(fY_2d<372))			fX_sft=fX_sft*0.91f;//兩邊ok
			if((fY_2d>=372)&&(fY_2d<500))			fX_sft=fX_sft*0.86f;//兩邊ok
			if((fY_2d>=500)&&(fY_2d<1500))			fX_sft=fX_sft*0.82f;//xx

			y=(nuLONG)fY_sft;
			x=(op->screenW/2)+(nuLONG)fX_sft;

#ifdef DispDEBUG
				 NUROFONT lf = {0};
				 lf.lfHeight = 50;
				 lf.lfWidth = 50;
				 lf.lfWeight = 1;
				 lf.lfEscapement = 0;
				 g_pDMLibGDI->GdiSetFont(&lf);
				 nuCOLORREF crTx, crBk;
				 crTx = nuRGB( 0,
				  0,
				  0 );
				 g_pDMLibGDI->GdiSetTextColor(crTx);
				 crBk = nuRGB( 255,
				  0,
				  0 );
				 nuroRECT tmpRect;
			
				tmpRect.left   = 0;
				tmpRect.top    = 100; 
				tmpRect.right  = 650+50;
				tmpRect.bottom = 350+50;
				 WCHAR wsTemp[10] = {0}, wsDot[2] = {0};
				 WCHAR wsWrite[30] = {0};
				 nuItow(yy, wsWrite, 10);
				 nuMemset(wsTemp, 0, sizeof(wsTemp));
				 nuItow(xx, wsTemp, 10);
				 nuAsctoWcs(wsDot, 10, ",", nuStrlen(","));
				 nuWcscat(wsWrite, wsDot);
				 nuWcscat(wsWrite, wsTemp);
				 g_pDMLibGDI->GdiDrawTextWEx(wsWrite, -1, &tmpRect, 41, 0, 0);
#endif
			//2012-11-29 modified by kris -End  
          
            m_apiRM.RM_AddModel( m_3dModelList[i].nID, x, y, m_3dModelList[i].szName
                , m_3dModelList[i].x
                , m_3dModelList[i].y
                , m_3d_param.nLookToy
                , op->screenH
                , op->screenH/4
                , ffixs
                , fz
                , m_pLibMT);
        }
    }
    else
    {
        //     for(nuINT i = 0; i < m_nNowCount; ++i)
        //     {
        //       m_apiRM.RM_AddModel( m_3dModelList[i].nID,
        // 							 m_3dModelList[i].x,
        //                m_3dModelList[i].y,
        //                m_3dModelList[i].szName );
        // 	  }
    }
}
