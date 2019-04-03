// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: OpenGLProcessZK.cpp,v 1.17 2010/04/15 01:30:20 zealass Exp $
// $Author: zealass $
// $Date: 2010/04/15 01:30:20 $
// $Locker:  $
// $Revision: 1.17 $
// $Source: /home/nuCode/libRenderModel/OpenGLProcessZK.cpp,v $
/////////////////////////////////////////////////////////////////////////

#ifdef _USE_OPENGLES
//#include <windows.h>
#include "OpenGLProcessZK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "glues.c"//For glu* API; C code

COpenGLProcessZK::COpenGLProcessZK()
{
    m_eglDisplay	= EGL_NO_DISPLAY;
    m_eglConfig		= NULL;
    m_eglSurface	= EGL_NO_SURFACE;
    m_eglContext	= EGL_NO_CONTEXT;
    m_major	= 1;
    m_minor = 0;
    m_bOpenGLWork	= false;
    m_fAngle		= 0.0f;
}

COpenGLProcessZK::~COpenGLProcessZK()
{
    FreeOpenGLES();
}
#ifdef NURO_OS_LINUX
GLuint COpenGLProcessZK::InitOpenGLES(HWND hWnd, HDC hDC, const RECT& rtScreen )
{
    //	RECT rtS;
    //	::GetWindowRect(hWnd, &rtS);
    //	::ScreenToClient(hWnd, )

    hWnd = g_x11_window;
    hDC = g_x11_display;

    m_eglDisplay = eglGetDisplay( (NativeDisplayType)(hDC) );

    if( !m_eglDisplay )
    {
        return 0;
    }
    if( eglInitialize(m_eglDisplay, &m_major, &m_minor) == EGL_FALSE )
    {
        return 0;
    }

    /*if(!eglBindAPI(EGL_OPENGL_ES_API))
    {   
        printf("Error: eglBindAPI () failed.\n");
        exit(1);
    }*/  

    // choose config
    EGLint cfg_attr_list[] = { EGL_BUFFER_SIZE, 16, EGL_NONE};
    int num = 0;
    if ( eglChooseConfig(m_eglDisplay, cfg_attr_list, &m_eglConfig, 1, &num) == EGL_FALSE || 
        0 == num )
    {
        return 0;
    }
    // create surface
    // EGLint sf_attr_list[] = {};
    /*
    HBITMAP hBitmap =  (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP); 
    m_eglSurface = eglCreatePixmapSurface(m_eglDisplay, m_eglConfig, (NativePixmapType)hBitmap, 0);
    */
    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, (NativeWindowType)hWnd, 0);
    if ( m_eglSurface == EGL_NO_SURFACE )
    {
        return 0;
    } 
    // create context
    // EGLint ctx_attr_list[] = { EGL_NONE };
    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, NULL, NULL);
    if( EGL_NO_CONTEXT == m_eglContext )
    {
        return 0;
    } 
    // active context (make current)
    if( eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext) == EGL_FALSE )
    {
        return 0;
    }
    m_bOpenGLWork = true;
    //test gl
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    return 1;
}
#else
GLuint COpenGLProcessZK::InitOpenGLES(HWND hWnd, HDC hDC, const RECT& rtScreen )
{
    //	RECT rtS;
    //	::GetWindowRect(hWnd, &rtS);
    //	::ScreenToClient(hWnd, )
    m_eglDisplay = eglGetDisplay( (NativeDisplayType)(hDC) );
    if( !m_eglDisplay )
    {
        return 0;
    }
    if( eglInitialize(m_eglDisplay, &m_major, &m_minor) == EGL_FALSE )
    {
        return 0;
    }
    // choose config
    EGLint cfg_attr_list[] = {EGL_BUFFER_SIZE, 16, EGL_NONE};
    int num = 0;
    if ( eglChooseConfig(m_eglDisplay, cfg_attr_list, &m_eglConfig, 1, &num) == EGL_FALSE || 
        0 == num )
    {
        return 0;
    }
    // create surface
    // EGLint sf_attr_list[] = {};
    /*
    HBITMAP hBitmap =  (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP); 
    m_eglSurface = eglCreatePixmapSurface(m_eglDisplay, m_eglConfig, (NativePixmapType)hBitmap, 0);
    */
    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, (NativeWindowType)hWnd, 0);
    if ( m_eglSurface == EGL_NO_SURFACE )
    {
        return 0;
    } 
    // create context
    // EGLint ctx_attr_list[] = { EGL_NONE };
    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, NULL, NULL);
    if( EGL_NO_CONTEXT == m_eglContext )
    {
        return 0;
    } 
    // active context (make current)
    if( eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext) == EGL_FALSE )
    {
        return 0;
    }
    m_bOpenGLWork = true;
    //test gl
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    return 1;
}
#endif

GLvoid COpenGLProcessZK::FreeOpenGLES()
{
    if( m_bOpenGLWork )
    {
        //		eglSwapBuffers(m_eglDisplay, m_eglSurface);
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    // destroy context
    if ( m_eglContext != EGL_NO_CONTEXT )
    {
        eglDestroyContext( m_eglDisplay, m_eglContext );
        m_eglContext = EGL_NO_CONTEXT;
    }
    // destroy surface
    if ( m_eglSurface != EGL_NO_SURFACE )
    {
        eglDestroySurface( m_eglDisplay, m_eglSurface );
        m_eglSurface = EGL_NO_SURFACE;
    }
    // terminate display
    if( m_eglDisplay !=	EGL_NO_DISPLAY )
    {
        eglTerminate( m_eglDisplay );
        m_eglDisplay = EGL_NO_DISPLAY;
    }
}

GLboolean COpenGLProcessZK::SwapBuffers()
{
    return eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

GLuint COpenGLProcessZK::LoadTexture(FILE_3DR_BMP& bmp3DR)
{
    unsigned int texName;
    glGenTextures(1, &texName);
    if( GL_INVALID_VALUE  == texName )
    {
        return 0;
    }
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexImage2D( GL_TEXTURE_2D, 
        0, 
        GL_RGB, 
        bmp3DR.nWidth,
        bmp3DR.nHeight,
        0, 
        GL_RGB, 
        GL_UNSIGNED_SHORT_5_6_5,
        bmp3DR.pColorBuff );
    
#ifdef _USE_OPENGL_DG
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif
    glBindTexture(GL_TEXTURE_2D, 0);
    return texName;
}

GLvoid COpenGLProcessZK::FreeTexture(GLuint nTexture)
{
    if( 1 != nTexture )
    {
        glDeleteTextures(1, &nTexture);
    }
}

GLuint COpenGLProcessZK::SetViewOption(PPARAMETER_VIEW_OPTION pParamView)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float f1 
        = (float)( pParamView->screenW * 1.0 / pParamView->screenH );
#ifdef _USE_FIXED
    gluPerspectivex( Float2Fixed(80.0f), Float2Fixed(f1), Float2Fixed(10.0f), Float2Fixed(1000.0f) );
#else
    //gluPerspectivef( 80.0f, f1, 1.0f, 1000.0f );
    //gluPerspectivef( 30.0f, f1, 1.0f, 1000.0f );
    gluPerspectivef( 45.0f, f1, 1.0f, 1000.0f );
#endif    
    glViewport(0, 0, pParamView->screenW, pParamView->screenH);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#ifdef _USE_FIXED
    glClearColorx(Float2Fixed(1.0f), Float2Fixed(0.0f), Float2Fixed(0.0f), Float2Fixed(1.0f));
    glClearDepthx(Float2Fixed(1.0));
#else
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClearDepthf(1.0f);
#endif
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#ifdef _USE_FIXED
    gluLookAtx( Float2Fixed(pParamView->fLookAtX),
        Float2Fixed(pParamView->fLookAtY),
        Float2Fixed(pParamView->fLookAtZ),
        Float2Fixed(pParamView->fLookToX),
        Float2Fixed(pParamView->fLookToY),
        Float2Fixed(pParamView->fLookToZ),
        Float2Fixed(pParamView->fUpX),
        Float2Fixed(pParamView->fUpY),
        Float2Fixed(pParamView->fUpZ) );
    glScalex(   Float2Fixed(pParamView->fScale), 
        Float2Fixed(pParamView->fScale), 
        Float2Fixed(pParamView->fScale) );
#else
    gluLookAtf( 
        pParamView->fLookAtX,
        pParamView->fLookAtY,
        pParamView->fLookAtZ,
        pParamView->fLookToX,
        pParamView->fLookToY,
        pParamView->fLookToZ,
        pParamView->fUpX,
        pParamView->fUpY,
        pParamView->fUpZ );
    glScalef( pParamView->fScale, 
        pParamView->fScale, 
        pParamView->fScale );
#endif
    m_fAngle = pParamView->fAngle;
    return 1;
}

GLboolean COpenGLProcessZK::Draw3DModel(PMODEL_3DMAX_DATA p3DMdl, long realHeight, long limitHeight)
{
    if( NULL == p3DMdl )
    {
        return false;
    }
    //glMatrixMode(GL_MODELVIEW);
    glPushMatrix();//keep M matrix
#ifdef _USE_FIXED
    glTranslatex(0, 0, 0);
    glRotatex( Float2Fixed(m_fAngle), 0, 0, Float2Fixed(1.0f) );
#else
    // 原来是glTranslatef(p3DMdl->nX, p3DMdl->nY, 0.0f);
    glTranslatef(-p3DMdl->nX, p3DMdl->nY, 0.0f); 
    glRotatef( m_fAngle, 0.0f, 0.0f, 1.0f );
    
#endif
    //
    for(nuDWORD i = 0; i < p3DMdl->stufileHeader.nGroupCount; ++i)
    {        
        if( 0 == p3DMdl->pstuVarOpenGLArray[i].nBmpTexture )
        {
            continue;
        }
        nuDWORD nCount = p3DMdl->pstuGroupArray[i].nFaceCount * 3;
        nuDWORD realCount = 0;
        bool needFree = false;
        FILE_3DR_TEXTURE_X  *tx = NULL;
        FILE_3DR_VERTEX_X   *vx = NULL;
        if (realHeight <= limitHeight)
        {
            tx
                = (FILE_3DR_TEXTURE_X*)(p3DMdl->pstuVarOpenGLArray[i].pDataBuff + sizeof(FILE_3DR_VERTEX_X) * nCount);
            vx
                = (FILE_3DR_VERTEX_X*)(p3DMdl->pstuVarOpenGLArray[i].pDataBuff);
            realCount = nCount;
        }
        else
        {
            tx = new FILE_3DR_TEXTURE_X[nCount];
            vx = new FILE_3DR_VERTEX_X[nCount];
            if (tx && vx)
            {
                needFree = true;
                FILE_3DR_TEXTURE_X  *tx0 
                    = (FILE_3DR_TEXTURE_X*)(p3DMdl->pstuVarOpenGLArray[i].pDataBuff + sizeof(FILE_3DR_VERTEX_X) * nCount);
                FILE_3DR_VERTEX_X   *vx0
                    = (FILE_3DR_VERTEX_X*)(p3DMdl->pstuVarOpenGLArray[i].pDataBuff);
                bool continueNext = false;
                nuDWORD tempTag = 0;
                nuDWORD triangleNum = nCount/3;
                nuLONG  limitHeight2 = limitHeight*0.70;
                for (nuDWORD i1=0; i1<triangleNum; ++i1)
                {
                    tempTag = i1*3;
                    if ( ( (vx0[tempTag].vZ>>16) < limitHeight )
                        && ( (vx0[tempTag+1].vZ>>16) < limitHeight )
                        && ( (vx0[tempTag+2].vZ>>16) < limitHeight ) )
                    {
                        tx[realCount] = tx0[tempTag];
                        vx[realCount] = vx0[tempTag];
                        ++realCount;
                        
                        tx[realCount] = tx0[tempTag+1];
                        vx[realCount] = vx0[tempTag+1];
                        ++realCount;
                        
                        tx[realCount] = tx0[tempTag+2];
                        vx[realCount] = vx0[tempTag+2];
                        ++realCount;
                    }
                    else
                    {
                        if ( ( (vx0[tempTag].vZ>>16) < limitHeight2 )
                            || ( (vx0[tempTag+1].vZ>>16) < limitHeight2 )
                            || ( (vx0[tempTag+2].vZ>>16) < limitHeight2 ) )
                        {
                            continueNext = true;
                            break;
                        }
                    }
                }
                if (continueNext)
                {
                    delete tx;
                    delete vx;
                    continue;
                }                
            }
            else
            {
                if (tx)
                {
                    delete tx;
                }
                if (vx)
                {
                    delete vx;
                }
                continue;
            }
        }
        if( p3DMdl->pstuGroupArray[i].illum )
        {
            GLfloat fList[4];
            fList[3] = 1.0;
            memcpy(fList, &p3DMdl->pstuGroupArray[i].fKa, 3 * sizeof(GLfloat));
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fList);
            memcpy(fList, &p3DMdl->pstuGroupArray[i].fKd, 3 * sizeof(GLfloat));
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fList);
            memcpy(fList, &p3DMdl->pstuGroupArray[i].fKs, 3 * sizeof(GLfloat));
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fList);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, p3DMdl->pstuGroupArray[i].fNs);
        }
        bool bUseNormal = false;
        if( p3DMdl->pstuGroupArray[i].nNormalCount )
        {
            bUseNormal = true;
        }
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, p3DMdl->pstuVarOpenGLArray[i].nBmpTexture);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        if( bUseNormal )
        {
            glEnableClientState(GL_NORMAL_ARRAY);
        }
        //..
        
        BYTE*	pBuff = p3DMdl->pstuVarOpenGLArray[i].pDataBuff;
        if( bUseNormal )
        {
            pBuff = p3DMdl->pstuVarOpenGLArray[i].pDataBuff + 
                sizeof(FILE_3DR_VERTEX_X) * nCount + 
                sizeof(FILE_3DR_TEXTURE_X) * nCount;
#ifdef _ZK_USE_FIXED_VERTEX
            glNormalPointer(GL_FIXED, 0, pBuff);
#else
            glNormalPointer(GL_FLOAT, 0, pBuff);
#endif
        }
        {
            //pBuff = p3DMdl->pstuVarOpenGLArray[i].pDataBuff + sizeof(FILE_3DR_VERTEX_X) * nCount;
#ifdef _ZK_USE_FIXED_VERTEX
            //glTexCoordPointer(2, GL_FIXED, 0, pBuff);
            glTexCoordPointer(2, GL_FIXED, 0, tx);
#else
            glTexCoordPointer(2, GL_FLOAT, 0, tx);
#endif
            //
            //pBuff = p3DMdl->pstuVarOpenGLArray[i].pDataBuff;
#ifdef _ZK_USE_FIXED_VERTEX
            //glVertexPointer(3, GL_FIXED, 0, pBuff);
            glVertexPointer(3, GL_FIXED, 0, vx);
#else
            glVertexPointer(3, GL_FLOAT, 0, vx);
#endif
            //glDrawArrays(GL_TRIANGLES, 0, nCount);
            glDrawArrays(GL_TRIANGLES, 0, realCount);  
            
            if (needFree)
            {
                delete tx;
                delete vx;
            }
        }
        if( bUseNormal )
        {
            glDisableClientState(GL_NORMAL_ARRAY);
        }
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glBindTexture(GL_TEXTURE_2D, 0);
        glEnable(GL_TEXTURE_2D);
  }
  glPopMatrix();//recover M Matrix
  return true;
}

GLuint COpenGLProcessZK::GetBitmapData(PPARAMETER_BITMAP pParamBitmap)
{
    if( NULL == pParamBitmap || NULL == pParamBitmap->pBitsBuff )
    {
        return 0;
    }
    glFlush();
    glFinish();	
    glPixelStorei(GL_PACK_ALIGNMENT,4); 
    glReadPixels( 0, 
        0, 
        pParamBitmap->nWidth, 
        pParamBitmap->nHeight, 
        GL_RGB, 
        GL_UNSIGNED_SHORT_5_6_5, 
        pParamBitmap->pBitsBuff );
    return 1;
}
#endif
