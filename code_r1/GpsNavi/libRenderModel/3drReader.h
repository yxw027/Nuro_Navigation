#ifndef _3DR_READER_NY_20111102_H_
#define _3DR_READER_NY_20111102_H_

#include "NuroDefine.h"
#include "NuroModuleApiStruct.h"
#include "3drDef.h"

//#define _3DR_MAGIC            'lny2'

//Fix by Dengxu
#define _3DR_MAGIC            (*(nuDWORD *)"2ynl")

//#define _3DR_TEX_NAME_BUF_LEN   16
#define _3DR_TEX_NAME_BUF_LEN   20

#define LOAD_PACK_MEMORY	0
#define LOAD_PACK_FILE		1

class C3drReader
{
public:
    C3drReader();
    ~C3drReader();

    nuBOOL  Init(nuPVOID pcRsApi);
    nuVOID  Free();

    nuBOOL  Read3dr(const nuCHAR *pszName, _3DR_DATA &stData, _3DR_BMP &stBmp);
    nuVOID  Free3drData(_3DR_DATA &stData) const;
    nuVOID  Free3drBmp(_3DR_BMP &stBmp) const;

private:
    nuBOOL  LoadPack();
    nuVOID  UnLoadPack();
    //
    nuBOOL  Read3drData(const nuCHAR *pszName, _3DR_DATA &stData, nuCHAR *pszTexName) const;
    nuBOOL  Read3drBmp(const nuCHAR *pszName, _3DR_BMP &stBmp) const;
    //
    static nuVOID Convert3drData(const nuBYTE *pbyFileBuf, _3DR_DATA &stData);

private:
    typedef struct tag_3DR_HEADER
    {
        nuDWORD dwMagic;
        nuDWORD dwTriangleCnt;
        nuDWORD dwVertexCnt;
        GLfixed aafxAABB[8][3];
        nuDWORD dwTriVertexOfs;
        nuDWORD dwTriTexCoordOfs;
        nuDWORD dwVertexOfs;
        nuDWORD dwVertexIdxOfs;
        nuCHAR  szTexName[_3DR_TEX_NAME_BUF_LEN];
    } _3DR_HEADER;
    //
    typedef struct tagBMP_HEADER
    {
        nuDWORD dwWidth;
        nuDWORD dwHeight;
        nuDWORD dwBufLen;
    } BMP_HEADER;

private:
    POPENRSAPI      	    m_pcRsApi;
    OPR_PACK               *m_pstPack;
    nuBOOL                  m_bInited;
    const nuTCHAR *const    m_ptzPackRPath;
};

#endif
