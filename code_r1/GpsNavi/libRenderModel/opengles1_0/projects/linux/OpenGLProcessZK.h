// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: OpenGLProcessZK.h,v 1.11 2010/04/02 07:27:42 zealass Exp $
// $Author: zealass $
// $Date: 2010/04/02 07:27:42 $
// $Locker:  $
// $Revision: 1.11 $
// $Source: /home/nuCode/libRenderModel/OpenGLProcessZK.h,v $
/////////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLPROCESSZK_H__01302E2B_6BB8_4BD4_A383_101BC3FD52F6__INCLUDED_)
#define AFX_OPENGLPROCESSZK_H__01302E2B_6BB8_4BD4_A383_101BC3FD52F6__INCLUDED_
#include "defines.h"

#ifndef _WIN32_WCE
//#define _USE_OPENGL_DG
#endif

#ifdef _USE_OPENGL_DG

#include "egl.h"
#include "gl.h"

#else

#include "GLES/gl.h"
#include "GLES/egl.h"
#include "glues.h"

#endif

#include "nuro3DModelStruct.h"
#include "nuroRenderModel.h"

//#define _USE_FIXED

#define Float2Fixed(fl) ((GLfixed)((fl)*65536.0f))

#ifdef NURO_OS_LINUX
#include "nuApiDevice.h"
#endif

class COpenGLProcessZK  
{
public:
    COpenGLProcessZK();
    virtual ~COpenGLProcessZK();
    
    GLuint	InitOpenGLES(HWND hWnd, HDC hDC, const RECT& rtScreen );
    GLvoid	FreeOpenGLES();
    
    GLuint	LoadTexture(FILE_3DR_BMP& bmp3DR);
    GLvoid	FreeTexture(GLuint nTexture);
    
    GLuint	SetViewOption(PPARAMETER_VIEW_OPTION pParamView);
    GLboolean	Draw3DModel(PMODEL_3DMAX_DATA p3DMdl, long realHeight, long limitHeight);
    GLuint  GetBitmapData(PPARAMETER_BITMAP pParamBitmap);
    
    GLboolean	SwapBuffers();
protected:
protected:
    float		m_fAngle;
private:
    EGLDisplay	m_eglDisplay;
    EGLConfig	m_eglConfig;
    EGLSurface	m_eglSurface;
    EGLContext	m_eglContext;
    EGLint		m_major;
    EGLint		m_minor;
    bool		m_bOpenGLWork;
};

#endif // !defined(AFX_OPENGLPROCESSZK_H__01302E2B_6BB8_4BD4_A383_101BC3FD52F6__INCLUDED_)
