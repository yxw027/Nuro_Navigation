#include "EglRender.h"


CEglRender::CEglRender()
{
    m_bEglWork = nuFALSE;
}

CEglRender::~CEglRender()
{
    Free();
}

nuBOOL CEglRender::Init(SYSDIFF_SCREEN sysDiff)
{
    if (!nuCheckSysDiffScreen(&sysDiff))
    {
        return nuFALSE;
    }
    Free();

    m_eglDisplay = eglGetDisplay(reinterpret_cast<NativeDisplayType>(nuGetSysDiffScreenDC(&sysDiff)));
    if (EGL_NO_DISPLAY == m_eglDisplay)
    {
        return nuFALSE;
    }
    if (eglInitialize(m_eglDisplay, nuNULL, nuNULL) == EGL_FALSE)
    {
        eglTerminate(m_eglDisplay);
        return nuFALSE;
    }
    EGLint anCfgAttribList[] = {EGL_BUFFER_SIZE, 16, EGL_DEPTH_SIZE, 16, EGL_NONE};
    EGLint nCfgCnt;
    if (eglChooseConfig(m_eglDisplay, anCfgAttribList, &m_eglConfig, 1, &nCfgCnt) == EGL_FALSE || 1 != nCfgCnt)
    {
        eglTerminate(m_eglDisplay);
        return nuFALSE;
    }
    
#ifdef NURO_OS_LINUX
    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, reinterpret_cast<NativeWindowType>(nuGetSysDiffScreenWnd(&sysDiff)), 
            nuNULL, s_width = 800, s_height = 480);
    stmp         = NULL;
#endif
#ifdef NURO_OS_WINDOWS
    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, reinterpret_cast<NativeWindowType>(nuGetSysDiffScreenWnd(&sysDiff)), NULL);
#endif
    
    if (EGL_NO_SURFACE == m_eglSurface)
    {
        eglTerminate(m_eglDisplay);
        return nuFALSE;
    }
    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, nuNULL, nuNULL);
    if (EGL_NO_CONTEXT == m_eglContext)
    {
        eglDestroySurface(m_eglDisplay, m_eglSurface);
        eglTerminate(m_eglDisplay);
        return nuFALSE;
    }
    if (eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext) == EGL_FALSE)
    {
        eglDestroyContext(m_eglDisplay, m_eglContext);
        eglDestroySurface(m_eglDisplay, m_eglSurface);
        eglTerminate(m_eglDisplay);
        return nuFALSE;
    }
    m_bEglWork = nuTRUE;
    InitEffect();
    return nuTRUE;
}

nuVOID CEglRender::Free()
{
    if (m_bEglWork)
    {
        m_bEglWork = nuFALSE;
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);        
        eglDestroyContext(m_eglDisplay, m_eglContext);
        eglDestroySurface(m_eglDisplay, m_eglSurface);
        eglTerminate(m_eglDisplay);        
    }
}

nuBOOL CEglRender::SetViewOption(PPARAMETER_VIEW_OPTION pstParamView)
{
    if (!m_bEglWork || nuNULL == pstParamView)
    {
        return nuFALSE;
    }

    glViewport(0, 0, pstParamView->screenW, pstParamView->screenH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspectivef(45.0f, static_cast<GLfloat>(pstParamView->screenW) / pstParamView->screenH, _RM_ZNEAR, _RM_ZFAR);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAtf(pstParamView->fLookAtX, pstParamView->fLookAtY, pstParamView->fLookAtZ
              ,pstParamView->fLookToX, pstParamView->fLookToY, pstParamView->fLookToZ
              ,pstParamView->fUpX,     pstParamView->fUpY,     pstParamView->fUpZ);
#ifdef KDEBUG
    glScalef(1.0f,1.0f,1.0f); //,ぃ笆
	m_3dScale =pstParamView->fScale;
#else
    glScalef(pstParamView->fScale, pstParamView->fScale, pstParamView->fScale);
#endif
    m_fRot = pstParamView->fAngle;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return nuTRUE;
}

nuBOOL CEglRender::GetBitmapData(PPARAMETER_BITMAP pstParamBitmap)
{
    if (!m_bEglWork || nuNULL == pstParamBitmap || nuNULL == pstParamBitmap->pBitsBuff)
    {
        return nuFALSE;
    }

    glFinish();
#ifndef NURO_OS_LINUX
    glReadPixels(0
                ,0
                ,pstParamBitmap->nWidth
                ,pstParamBitmap->nHeight
                ,GL_RGB
                ,GL_UNSIGNED_SHORT_5_6_5
                ,pstParamBitmap->pBitsBuff);
#else
    if (stmp == NULL)
    {   
        s_width = pstParamBitmap->nWidth;
        s_height = pstParamBitmap->nHeight;
        stmp = (unsigned short *)nuMalloc(s_width * s_height * sizeof (short));
    }   
    else
    {   
        //nuFree(stmp);
        //s_width = pstParamBitmap->nWidth;
        //s_height = pstParamBitmap->nHeight;
        //stmp = (unsigned short *)nuMalloc(s_width * s_height * sizeof (short));
    }

    glReadPixels(0
            ,0  
            ,s_width
            ,s_height
            ,GL_RGB
            ,GL_UNSIGNED_SHORT_5_6_5
            ,stmp);
    
    {
        unsigned short *p = (unsigned short *)pstParamBitmap->pBitsBuff;
        int i, j;
        for (i = 0; i < s_width; i++)
        {   
            for (j = 0; j < s_height; j++)
            {   
                p[i + (s_height - j - 1) * s_width] = stmp[i + j * s_width];
            }   
        }   
    }   

#endif

    return nuTRUE;
}

GLuint CEglRender::CreateTexture(const _3DR_BMP &stBmp)
{
    if (!m_bEglWork || nuNULL == stBmp.pbyBuf)
    {
        return 0;
    }

    GLuint nTexId = 0;
    glGenTextures(1, &nTexId);
    if (0 == nTexId)
    {
        return 0;
    }
    glBindTexture(GL_TEXTURE_2D, nTexId);
    glTexImage2D(GL_TEXTURE_2D
                ,0
                ,GL_RGB
                ,stBmp.nWidth
                ,stBmp.nHeight
                ,0
                ,GL_RGB
                ,GL_UNSIGNED_SHORT_5_6_5
                ,stBmp.pbyBmpData);
                //,stBmp.pbyBuf);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return nTexId;
}

nuVOID CEglRender::DeleteTexture(GLuint nTexId)
{
    if (m_bEglWork && 0 != nTexId)
    {
        glDeleteTextures(1, &nTexId);
    }
}

nuBOOL CEglRender::Draw3dr(GLint nX, GLint nY, const _3DR_MDL &stMdl)
{
#ifdef  KDEBUG
	if ((nY>=244)) {return false;}
#endif

    if (!m_bEglWork || nuNULL == stMdl.stData.pafxVertex || nuNULL == stMdl.stData.pafxTriTexCoord)
    {
        return nuFALSE;
    }

    nuBOOL bState = nuTRUE;

    glPushMatrix();
    glTranslatef(static_cast<GLfloat>(nX), static_cast<GLfloat>(nY), 0.0f);
    glRotatef(m_fRot, 0.0f, 0.0f, 1.0f);
#ifdef KDEBUG
		glScalef(m_3dScale*1.0f, m_3dScale*1.0f, m_3dScale*1.0f);
#endif
    glGetFixedv(GL_MODELVIEW_MATRIX, m_afxModeMat);
#ifdef _SHOW_3DR_PART
    if (AABBAtFront(stMdl.stData.aafxAABB))
    {
        glBindTexture(GL_TEXTURE_2D, stMdl.nTexId);
        glVertexPointer(3, GL_FIXED, 0, stMdl.stData.pafxTriVertex);
        glTexCoordPointer(2, GL_FIXED, 0, stMdl.stData.pafxTriTexCoord);
        glDrawArrays(GL_TRIANGLES, 0, stMdl.stData.nTriangleCnt * 3);
    }
    else
    {
        bState = Calc3drIdxClip(stMdl.stData);
        if (bState)
        {
            glBindTexture(GL_TEXTURE_2D, stMdl.nTexId);
            glVertexPointer(3, GL_FIXED, 0, stMdl.stData.pafxTriVertex);
            glTexCoordPointer(2, GL_FIXED, 0, stMdl.stData.pafxTriTexCoord);
            glDrawElements(GL_TRIANGLES, m_nTriIdxCnt, GL_UNSIGNED_SHORT, m_asTriIdx);
        }
    }
#else
    if( AABBAheadOfNear(stMdl.stData.aafxAABB) && 
		AABBAheadOfFar(stMdl.stData.aafxAABB) )
    {
        glBindTexture(GL_TEXTURE_2D, stMdl.nTexId);
        glVertexPointer(3, GL_FIXED, 0, stMdl.stData.pafxTriVertex);
        glTexCoordPointer(2, GL_FIXED, 0, stMdl.stData.pafxTriTexCoord);
        glDrawArrays(GL_TRIANGLES, 0, stMdl.stData.nTriangleCnt * 3);
    }
#endif

    glPopMatrix();

    return bState;
}

//////////////////////////////////////////////////////////////////////////

nuVOID CEglRender::InitEffect()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
}

nuBOOL CEglRender::Calc3drIdxClip(const _3DR_DATA &stData)
{
    if (stData.nVertexCnt > _3DR_MAX_VERTEX_CNT || stData.nTriangleCnt > _3DR_MAX_TRIANGLE_CNT)
    {
        return nuFALSE;
    }
    /// 判断顶点是否在视点正方向
    for (nuUINT i = 0; i < stData.nVertexCnt; i++)
    {
        m_abVertexAtFront[i] = VertexAtFront(stData.pafxVertex[i]);
    }
    /// 生成三角形索引
    m_nTriIdxCnt = 0;
    for (nuUINT i = 0; i < stData.nTriangleCnt * 3; i += 3)
    {
        if (m_abVertexAtFront[stData.psVertexIdx[i]] && m_abVertexAtFront[stData.psVertexIdx[i+1]] && m_abVertexAtFront[stData.psVertexIdx[i+2]])
        {
            m_asTriIdx[m_nTriIdxCnt]   = i;
            m_asTriIdx[m_nTriIdxCnt+1] = i + 1;
            m_asTriIdx[m_nTriIdxCnt+2] = i + 2;
            m_nTriIdxCnt += 3;
        }
    }
    return nuTRUE;
}

nuBOOL CEglRender::AABBAtFront(const _3drVertex_t pafxAABB[8])
{
    for (nuUINT i = 0; i < 8; i++)
    {
        if (!VertexAtFront(pafxAABB[i]))
        {
            return nuFALSE;
        }
    }
    return nuTRUE;
}

nuBOOL CEglRender::AABBAheadOfNear(const _3drVertex_t pafxAABB[8])
{
	for (nuUINT i = 0; i < 8; i++)
	{
		if (!VertexAheadOfNear(pafxAABB[i]))
		{
			return nuFALSE;
		}
	}
	return nuTRUE;
}

nuBOOL CEglRender::AABBAheadOfFar(const _3drVertex_t pafxAABB[8])
{
	for (nuUINT i = 0; i < 8; i++)
	{
		if (!VertexAheadOfFar(pafxAABB[i]))
		{
			return nuFALSE;
		}
	}
	return nuTRUE;
}