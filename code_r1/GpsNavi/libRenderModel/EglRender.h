#ifndef _EGL_RENDER_NY_20111101_H_
#define _EGL_RENDER_NY_20111101_H_

#include "NuroDefine.h"
#include "NuroRenderModel.h"
#include "EglInc.h"
#include "3drDef.h"
#define KDEBUG
#define _MULX(a,b)	((static_cast<__int64>(a)*(b))>>16)
#define _RM_ZNEAR   10.0f
#define _RM_ZFAR    600.0f

#ifdef NURO_OS_LINUX
#define __forceinline inline
typedef long long __int64;
static unsigned short *stmp;
static int s_width, s_height;
#endif

class CEglRender
{
public:
    CEglRender();
    ~CEglRender();

    nuBOOL  Init(SYSDIFF_SCREEN sysDiff);
    nuVOID  Free();

    nuBOOL  SetViewOption(PPARAMETER_VIEW_OPTION pstParamView);
    nuBOOL  GetBitmapData(PPARAMETER_BITMAP pstParamBitmap);

    GLuint  CreateTexture(const _3DR_BMP &stBmp);
    nuVOID  DeleteTexture(GLuint nTexId);

    nuBOOL  Draw3dr(GLint nX, GLint nY, const _3DR_MDL &stMdl);

private:
    nuVOID  InitEffect();
    nuBOOL  Calc3drIdxClip(const _3DR_DATA &stData);
    nuBOOL  AABBAtFront(const _3drVertex_t pafxAABB[8]);
    nuBOOL  AABBAheadOfNear(const _3drVertex_t pafxAABB[8]);
    nuBOOL  VertexAtFront(const _3drVertex_t &afxVertex);
    nuBOOL  VertexAheadOfNear(const _3drVertex_t &afxVertex);
	nuBOOL  VertexAheadOfFar(const _3drVertex_t &afxVertex);
	nuBOOL  AABBAheadOfFar(const _3drVertex_t pafxAABB[8]);

private:
    EGLDisplay  m_eglDisplay;
    EGLConfig	m_eglConfig;
    EGLSurface	m_eglSurface;
    EGLContext	m_eglContext;
    nuFLOAT     m_fRot;
    nuBOOL		m_bEglWork;

private:
    GLfixed     m_afxModeMat[16];
    GLuint      m_nTriIdxCnt;
    GLushort    m_asTriIdx[_3DR_MAX_TRIANGLE_CNT * 3];
    nuBOOL      m_abVertexAtFront[_3DR_MAX_VERTEX_CNT];

private:
	nuFLOAT		m_3dScale;
};

__forceinline nuBOOL CEglRender::VertexAheadOfNear(const _3drVertex_t &afxVertex)
{
    GLfixed fxZ = _MULX(m_afxModeMat[2], afxVertex[0]) + _MULX(m_afxModeMat[6], afxVertex[1]) + _MULX(m_afxModeMat[10], afxVertex[2]) + m_afxModeMat[14];
    GLfixed fxW = _MULX(m_afxModeMat[3], afxVertex[0]) + _MULX(m_afxModeMat[7], afxVertex[1]) + _MULX(m_afxModeMat[11], afxVertex[2]) + m_afxModeMat[15];    
    return -static_cast<float>(fxZ) / fxW > _RM_ZNEAR;
}

__forceinline nuBOOL CEglRender::VertexAheadOfFar(const _3drVertex_t &afxVertex)
{
	GLfixed fxZ = _MULX(m_afxModeMat[2], afxVertex[0]) + _MULX(m_afxModeMat[6], afxVertex[1]) + _MULX(m_afxModeMat[10], afxVertex[2]) + m_afxModeMat[14];
	GLfixed fxW = _MULX(m_afxModeMat[3], afxVertex[0]) + _MULX(m_afxModeMat[7], afxVertex[1]) + _MULX(m_afxModeMat[11], afxVertex[2]) + m_afxModeMat[15];    
	return -static_cast<float>(fxZ) / fxW < _RM_ZFAR;
}

__forceinline nuBOOL CEglRender::VertexAtFront(const _3drVertex_t &afxVertex)
{
    return _MULX(m_afxModeMat[2], afxVertex[0]) + _MULX(m_afxModeMat[6], afxVertex[1]) + _MULX(m_afxModeMat[10], afxVertex[2]) + m_afxModeMat[14] < 0;
}

#endif
