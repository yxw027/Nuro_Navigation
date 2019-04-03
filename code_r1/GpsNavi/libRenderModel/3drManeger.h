#ifndef _EGL_3DR_NY_20111101_H_
#define _EGL_3DR_NY_20111101_H_

#include "NuroDefine.h"
#include "NuroModuleApiStruct.h"
#include "3drDef.h"
#include "EglRender.h"
#include "3drReader.h"

#define _3DR_MAX_CNT        15

class C3drManeger
{
public:
    C3drManeger();
    ~C3drManeger();

    nuBOOL  Init(nuPVOID pcRsApi, CEglRender *pcEglRender);
    nuVOID  Free();

    nuBOOL  Add3dr(nuLONG lID, nuLONG lX, nuLONG lY, nuCHAR *pszName);
    nuVOID  Del3dr(nuLONG lID);
    nuBOOL  Draw3dr() const;

private:
    typedef struct tag_3DR_INFO
    {
        nuLONG	lID;
        nuLONG	lX;
        nuLONG	lY;
    } _3DR_INFO;
    //
    typedef struct tag_3DR
    {
        _3DR_INFO   stInfo;
        _3DR_MDL    stMdl;
    } _3DR;

private:
    nuBOOL  Init3drArr();
    nuVOID  Free3drArr();

    _3DR   *Insert3dr();
    nuVOID  Remove3dr(_3DR *pst3dr);
    _3DR   *Find3dr(nuLONG lID) const;

private:
    CEglRender *m_pcEglRender;
    C3drReader  m_c3drReader;
    nuUINT      m_n3drCnt;
    _3DR       *m_apst3dr[_3DR_MAX_CNT];
    _3DR        m_ast3dr[_3DR_MAX_CNT];    
    nuBOOL      m_bInited;
};

#endif
