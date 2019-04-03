// Copyright (c) 2009 Nuro Corp.  All rights reserved.
// $Id: Read3DRModelZK.cpp,v 1.15 2010/04/15 01:30:20 zealass Exp $
// $Author: zealass $
// $Date: 2010/04/15 01:30:20 $
// $Locker:  $
// $Revision: 1.15 $
// $Source: /home/nuCode/libRenderModel/Read3DRModelZK.cpp,v $
/////////////////////////////////////////////////////////////////////////

#ifdef _USE_OPENGLES
#include "Read3DRModelZK.h"
#include "nuClib.h"
#include "nuApiInterFace.h"
#include "OpenGLProcessZK.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRead3DRModelZK::CRead3DRModelZK()
{
    m_pOpenGL		= NULL;
    m_nNowMdlCount	= 0;
    nuMemset( m_pstuMdlArray, 0, sizeof(m_pstuMdlArray) );
}
CRead3DRModelZK::~CRead3DRModelZK()
{
    Free();
}
bool CRead3DRModelZK::Initialize(class COpenGLProcessZK* pOpenGL)
{
    m_nNowMdlCount	= 0;
    m_pOpenGL		= pOpenGL;
    return true;
}
void CRead3DRModelZK::Free()
{
    for(nuBYTE i = 0; i < m_nNowMdlCount; ++i)
    {
        DeltOneModel(&m_pstuMdlArray[i]);
    }
    nuMemset(m_pstuMdlArray, 0, sizeof(m_pstuMdlArray));
    m_nNowMdlCount = 0;
}
nuUINT CRead3DRModelZK::AddModel(long nID, long x, long y, const char* psz3DR
                                 , long rx, long ry, long ty, int height, int dCenD, float fs, float fz
                                 , PMATHTOOLAPI pMT)
{
    if( NULL == psz3DR )
    {
        return 0;
    }
    nuroPOINT point;
    for(nuBYTE i = 0; i < m_nNowMdlCount; ++i)
    {
        if( nID == m_pstuMdlArray[i].nID )
        {
            m_pstuMdlArray[i].nX = x;
            m_pstuMdlArray[i].nY = y;            
            m_pstuMdlArray[i].rX  = rx;
            m_pstuMdlArray[i].rY  = ry;
            m_pstuMdlArray[i].tY  = ty;
            m_pstuMdlArray[i].height = height;
            m_pstuMdlArray[i].fSclRef = fs;
            m_pstuMdlArray[i].fZhRef = fz;
            m_pstuMdlArray[i].dCenD = dCenD;            
            m_pstuMdlArray[i].lY = 0;
            //LibMT_BmpToMap
            pMT->MT_MapToBmp( m_pstuMdlArray[i].rX+m_pstuMdlArray[i].xmin
                , m_pstuMdlArray[i].rY+m_pstuMdlArray[i].ymin
                , &point.x, &point.y);
            if (m_pstuMdlArray[i].lY < point.y)
            {
                m_pstuMdlArray[i].lY = point.y;
            }
            pMT->MT_MapToBmp( m_pstuMdlArray[i].rX+m_pstuMdlArray[i].xmin
                , m_pstuMdlArray[i].rY+m_pstuMdlArray[i].ymax
                , &point.x, &point.y);
            if (m_pstuMdlArray[i].lY < point.y)
            {
                m_pstuMdlArray[i].lY = point.y;
            }
            pMT->MT_MapToBmp( m_pstuMdlArray[i].rX+m_pstuMdlArray[i].xmax
                , m_pstuMdlArray[i].rY+m_pstuMdlArray[i].ymin
                , &point.x, &point.y);
            if (m_pstuMdlArray[i].lY < point.y)
            {
                m_pstuMdlArray[i].lY = point.y;
            }
            pMT->MT_MapToBmp( m_pstuMdlArray[i].rX+m_pstuMdlArray[i].xmax
                , m_pstuMdlArray[i].rY+m_pstuMdlArray[i].ymax
                , &point.x, &point.y);
            if (m_pstuMdlArray[i].lY < point.y)
            {
                m_pstuMdlArray[i].lY = point.y;
            }
            m_pstuMdlArray[i].lY 
                = m_pstuMdlArray[i].tY - m_pstuMdlArray[i].lY;
            return 1;
        }
    }
    if (_ZK_MAX_COUNT_MODEL_LOADEN == m_nNowMdlCount)
    {
        return 0;
    }
    if( Read3DRModel(psz3DR) )
    {
        m_pstuMdlArray[m_nNowMdlCount].nX	= x;
        m_pstuMdlArray[m_nNowMdlCount].nY	= y;
        m_pstuMdlArray[m_nNowMdlCount].nID	= nID;        
        m_pstuMdlArray[m_nNowMdlCount].rX  = rx;
        m_pstuMdlArray[m_nNowMdlCount].rY  = ry;
        m_pstuMdlArray[m_nNowMdlCount].tY  = ty;
        m_pstuMdlArray[m_nNowMdlCount].height = height;
        m_pstuMdlArray[m_nNowMdlCount].fSclRef = fs;
        m_pstuMdlArray[m_nNowMdlCount].fZhRef = fz;
        m_pstuMdlArray[m_nNowMdlCount].dCenD = dCenD;        
        long xmin = 0;
        long xmax = 0;
        long ymin = 0;
        long ymax = 0;
        long zmin = 0;
        long zmax = 0;
        long zabsh=0;
        for(unsigned long i = 0; i < m_pstuMdlArray[m_nNowMdlCount].stufileHeader.nGroupCount; ++i)
        {
            nuDWORD nCount = m_pstuMdlArray[m_nNowMdlCount].pstuGroupArray[i].nFaceCount * 3;
            FILE_3DR_VERTEX_X* ppp 
                = (FILE_3DR_VERTEX_X*)(m_pstuMdlArray[m_nNowMdlCount].pstuVarOpenGLArray[i].pDataBuff);
            int xt=0, yt=0, zt=0;
            for(nuDWORD ttt=0; ttt<nCount; ++ttt)
            {
                xt = ppp[ttt].vX >> 16;
                yt = ppp[ttt].vY >> 16;
                zt = ppp[ttt].vZ >> 16;
                if (xt>xmax)
                {
                    xmax = xt;
                }
                else if (xt<xmin)
                {
                    xmin = xt;
                }            
                if (yt>ymax)
                {
                    ymax = yt;
                }
                else if (yt<ymin)
                {
                    ymin = yt;
                }            
                if (zt>zmax)
                {
                    zmax = zt;
                }
                else if (zt<zmin)
                {
                    zmin = zt;
                }                
                zmin = abs(zmin);
                if (zabsh < zmin)
                {
                    zabsh = zmin;
                }
                zmax = abs(zmax);
                if (zabsh < zmax)
                {
                    zabsh = zmax;
                }
            }
        }
        m_pstuMdlArray[m_nNowMdlCount].xmax = (long)(xmax*_3D_SCALE_SET_);
        m_pstuMdlArray[m_nNowMdlCount].xmin = (long)(xmin*_3D_SCALE_SET_);
        m_pstuMdlArray[m_nNowMdlCount].ymax = (long)(ymax*_3D_SCALE_SET_);
        m_pstuMdlArray[m_nNowMdlCount].ymin = (long)(ymin*_3D_SCALE_SET_);
        m_pstuMdlArray[m_nNowMdlCount].lY = 0;
        pMT->MT_MapToBmp( m_pstuMdlArray[m_nNowMdlCount].rX+xmin
            , m_pstuMdlArray[m_nNowMdlCount].rY+ymin
            , &point.x
            , &point.y);
        if (m_pstuMdlArray[m_nNowMdlCount].lY < point.y)
        {
            m_pstuMdlArray[m_nNowMdlCount].lY = point.y;
        }
        pMT->MT_MapToBmp( m_pstuMdlArray[m_nNowMdlCount].rX+xmin
            , m_pstuMdlArray[m_nNowMdlCount].rY+ymax
            , &point.x
            , &point.y);
        if (m_pstuMdlArray[m_nNowMdlCount].lY < point.y)
        {
            m_pstuMdlArray[m_nNowMdlCount].lY = point.y;
        }
        pMT->MT_MapToBmp( m_pstuMdlArray[m_nNowMdlCount].rX+xmax
            , m_pstuMdlArray[m_nNowMdlCount].rY+ymin
            , &point.x
            , &point.y);
        if (m_pstuMdlArray[m_nNowMdlCount].lY < point.y)
        {
            m_pstuMdlArray[m_nNowMdlCount].lY = point.y;
        }
        pMT->MT_MapToBmp( m_pstuMdlArray[m_nNowMdlCount].rX+xmax
            , m_pstuMdlArray[m_nNowMdlCount].rY+ymax
            , &point.x
            , &point.y);
        if (m_pstuMdlArray[m_nNowMdlCount].lY < point.y)
        {
            m_pstuMdlArray[m_nNowMdlCount].lY = point.y;
        }
        m_pstuMdlArray[m_nNowMdlCount].lY 
            = m_pstuMdlArray[m_nNowMdlCount].tY - m_pstuMdlArray[m_nNowMdlCount].lY;
        m_pstuMdlArray[m_nNowMdlCount].maxHeight = zabsh;
        ++ m_nNowMdlCount;
        return 1;
    }
    return 0;
}
nuUINT CRead3DRModelZK::DelModel(long nID, long x, long y)
{
    if( 0 == m_nNowMdlCount )
    {
        return 0;
    }
    nuBYTE delPos = _ZK_MAX_COUNT_MODEL_LOADEN;
    for(nuBYTE i = 0; i < m_nNowMdlCount; ++i)
    {
        if( m_pstuMdlArray[i].nID == nID )
        {
            delPos = i;
            DeltOneModel(&m_pstuMdlArray[i]);
            break;
        }
    }
    if( _ZK_MAX_COUNT_MODEL_LOADEN !=  delPos)
    {
        if (delPos!=(m_nNowMdlCount-1))
        {
            nuMemmove(&m_pstuMdlArray[delPos], &m_pstuMdlArray[delPos+1], sizeof(MODEL_3DMAX_DATA)*(m_nNowMdlCount-1-delPos));
            nuMemset(&m_pstuMdlArray[m_nNowMdlCount-1], 0, sizeof(MODEL_3DMAX_DATA));
        }
        else
        {
            nuMemset(&m_pstuMdlArray[m_nNowMdlCount-1], 0, sizeof(MODEL_3DMAX_DATA));
        }
        --m_nNowMdlCount;
        return 1;
    }
    return 0;
}
bool CRead3DRModelZK::Read3DRModel(const char* psz3DR)
{
    //-->Open File @Zhikun
    TCHAR tzFileName[256];
    nuTcscpy(tzFileName, s_tzPath);
    int i = nuTcslen(tzFileName);
    int j = 0;
    while( i < 255 && '\0' != psz3DR[j] )
    {
        tzFileName[i++] = psz3DR[j++];
    }
    tzFileName[i] = '\0';

    //	MessageBox(NULL, tzFileName, TEXT(""), 0);
    //nuPrintf(".... %s ...\n", tzFileName);

    if (nuStrcasecmp(tzFileName, "./MODEL/obj310000134.3DR") != 0)
    {
        return false;
    }

    nuFILE *pf3DR = nuTfopen(tzFileName, NURO_FS_RB);
    if( NULL == pf3DR )
    {
        return false;
    }
    //<--Open File 
    bool bRes = true;
    if( !Read3DRData(pf3DR) || 
        !MakeOpenGLTexture(pf3DR) )
    {
        DeltOneModel( &m_pstuMdlArray[m_nNowMdlCount] );
        bRes = false;
    }
    nuFclose(pf3DR);
    return bRes;
}
bool CRead3DRModelZK::Read3DRData(nuFILE *pf3DR)
{
    //Read file header
    if( nuFseek(pf3DR, 0, NURO_SEEK_SET) != 0 ||
        nuFread(&m_pstuMdlArray[m_nNowMdlCount].stufileHeader, sizeof(FILE_3DR_HEADER), 1, pf3DR) != 1 )
    {
        return false;
    }
    //Read Group data
    m_pstuMdlArray[m_nNowMdlCount].pstuGroupArray	= 
        new FILE_3DR_GROUP[ m_pstuMdlArray[ m_nNowMdlCount ].stufileHeader.nGroupCount ];
    if( NULL == m_pstuMdlArray[m_nNowMdlCount].pstuGroupArray )
    {
        return false;
    }
    nuMemset( m_pstuMdlArray[m_nNowMdlCount].pstuGroupArray,
        0, 
        sizeof(FILE_3DR_GROUP) * m_pstuMdlArray[ m_nNowMdlCount ].stufileHeader.nGroupCount );
    m_pstuMdlArray[m_nNowMdlCount].pstuVarOpenGLArray =
        new MODEL_VAR_FROM_OPENGL[ m_pstuMdlArray[ m_nNowMdlCount ].stufileHeader.nGroupCount ];
    if( NULL == m_pstuMdlArray[m_nNowMdlCount].pstuVarOpenGLArray )
    {
        return false;
    }
    nuMemset( m_pstuMdlArray[m_nNowMdlCount].pstuVarOpenGLArray,
        0,
        sizeof(MODEL_VAR_FROM_OPENGL) * m_pstuMdlArray[ m_nNowMdlCount ].stufileHeader.nGroupCount );
    if( nuFread(m_pstuMdlArray[m_nNowMdlCount].pstuGroupArray,
        sizeof(FILE_3DR_GROUP),
        m_pstuMdlArray[ m_nNowMdlCount ].stufileHeader.nGroupCount,
        pf3DR) != m_pstuMdlArray[ m_nNowMdlCount ].stufileHeader.nGroupCount )
    {
        return false;
    }
    //Read triangle data
    for(nuBYTE i = 0; i < m_pstuMdlArray[ m_nNowMdlCount ].stufileHeader.nGroupCount; ++i )
    {
        nuDWORD nCount = m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nFaceCount * 3;
        m_pstuMdlArray[ m_nNowMdlCount ].pstuVarOpenGLArray[i].nDataLen = 0;
        if( m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nVertexCount )//Vertex
        {
            m_pstuMdlArray[ m_nNowMdlCount ].pstuVarOpenGLArray[i].nDataLen += 
                sizeof(FILE_3DR_VERTEX_X) * nCount;
        }
        if( m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nTextureCount )//texture
        {
            m_pstuMdlArray[ m_nNowMdlCount ].pstuVarOpenGLArray[i].nDataLen += 
                sizeof(FILE_3DR_TEXTURE_X) * nCount;
        }
        if( m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nNormalCount )//Normal
        {
            m_pstuMdlArray[ m_nNowMdlCount ].pstuVarOpenGLArray[i].nDataLen += 
                sizeof(FILE_3DR_NORMAL_X) * nCount;
        }
        m_pstuMdlArray[ m_nNowMdlCount ].pstuVarOpenGLArray[i].pDataBuff = 
            new nuBYTE[ m_pstuMdlArray[ m_nNowMdlCount ].pstuVarOpenGLArray[i].nDataLen ];
        if( NULL == m_pstuMdlArray[ m_nNowMdlCount ].pstuVarOpenGLArray[i].pDataBuff )
        {
            return false;
        }
        nuWORD*	pFaceIndex = new nuWORD[nCount];
        if( NULL == pFaceIndex )
        {
            return false;
        }
        nuDWORD nBuffUsed = 0;
        nuBYTE* pBuffStart = m_pstuMdlArray[ m_nNowMdlCount ].pstuVarOpenGLArray[i].pDataBuff;
        if( m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nVertexCount )//Read vertext data 
        {
            PFILE_3DR_VERTEX_X pVertex = new FILE_3DR_VERTEX_X[ 1 + m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nVertexCount ];
            if( NULL == pVertex )
            {
                delete[] pFaceIndex;
                return false;
            }
            pVertex[0].vX = pVertex[0].vY = pVertex[0].vZ = 0;
            if( nuFread( &pVertex[1], 
                sizeof(FILE_3DR_VERTEX_X),
                m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nVertexCount,
                pf3DR ) != m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nVertexCount ||
                nuFread( pFaceIndex, 
                2,
                nCount,
                pf3DR ) != nCount )
            {
                delete[] pVertex;
                delete[] pFaceIndex;
                return false;
            }
            PFILE_3DR_VERTEX_X pTrangleV = (PFILE_3DR_VERTEX_X)(pBuffStart + nBuffUsed);
            nBuffUsed += sizeof(FILE_3DR_VERTEX_X) * nCount;
            for(nuDWORD j = 0; j < nCount; j += 3)
            {
                pTrangleV[j]		= pVertex[ pFaceIndex[j] ];
                pTrangleV[j + 1]	= pVertex[ pFaceIndex[j + 1] ];
                pTrangleV[j + 2]	= pVertex[ pFaceIndex[j + 2] ];
            }
            delete[] pVertex;
        }
        if( m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nTextureCount )//Read texture data
        {
            PFILE_3DR_TEXTURE_X pTexture = new FILE_3DR_TEXTURE_X[1 + m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nTextureCount];
            if( NULL == pTexture )
            {
                delete[] pFaceIndex;
                return false;
            }
            pTexture[0].tX = pTexture[0].tY = 0;
            if( nuFread( &pTexture[1], 
                sizeof(FILE_3DR_TEXTURE_X),
                m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nTextureCount,
                pf3DR ) != m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nTextureCount ||
                nuFread( pFaceIndex, 
                2,
                nCount,
                pf3DR ) != nCount )
            {
                delete[] pTexture;
                delete[] pFaceIndex;
                return false;
            }
            PFILE_3DR_TEXTURE_X pTrangleT = (PFILE_3DR_TEXTURE_X)(pBuffStart + nBuffUsed);
            nBuffUsed += sizeof(FILE_3DR_TEXTURE_X) * nCount;
            for(nuDWORD j = 0; j < nCount; j += 3)
            {
                pTrangleT[j]		= pTexture[ pFaceIndex[j] ];
                pTrangleT[j + 1]	= pTexture[ pFaceIndex[j + 1] ];
                pTrangleT[j + 2]	= pTexture[ pFaceIndex[j + 2] ];
            }
            delete[] pTexture;
        }
        if( m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nNormalCount )//Read normal data
        {
            PFILE_3DR_NORMAL_X pNormal = new FILE_3DR_NORMAL_X[ 1 + m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nNormalCount ];
            if( NULL == pNormal )
            {
                delete[] pFaceIndex;
                return false;
            }
            pNormal[0].nX = pNormal[0].nY = pNormal[0].nZ = 0;
            if( nuFread( &pNormal[1],
                sizeof(FILE_3DR_NORMAL_X),
                m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nNormalCount,
                pf3DR ) != m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nNormalCount ||
                nuFread( pFaceIndex, 2, nCount, pf3DR) != nCount ) 
            {
                delete[] pNormal;
                delete[] pFaceIndex;
                return false;
            }
            PFILE_3DR_NORMAL_X pTrangleN = (PFILE_3DR_NORMAL_X)(pBuffStart + nBuffUsed);
            for(nuDWORD j = 0; j < nCount; j += 3)
            {
                pTrangleN[j]		= pNormal[ pFaceIndex[j] ];
                pTrangleN[j + 1]	= pNormal[ pFaceIndex[j + 1] ];
                pTrangleN[j + 2]	= pNormal[ pFaceIndex[j + 1] ];
            }
            delete[] pNormal;
        }
        delete[] pFaceIndex;
    }
    return true;
}
void CRead3DRModelZK::DeltOneModel(PMODEL_3DMAX_DATA pstuMdl)
{
    if( NULL != pstuMdl->pstuGroupArray )
    {
        delete[] pstuMdl->pstuGroupArray;
        pstuMdl->pstuGroupArray = NULL;
    }
    if( NULL != pstuMdl->pstuVarOpenGLArray )
    {
        for(nuDWORD i = 0; i < pstuMdl->stufileHeader.nGroupCount; ++i)
        {
            if( NULL != pstuMdl->pstuVarOpenGLArray[i].pDataBuff )
            {
                delete[] pstuMdl->pstuVarOpenGLArray[i].pDataBuff;
                pstuMdl->pstuVarOpenGLArray[i].pDataBuff = NULL;
            }
            if( pstuMdl->pstuVarOpenGLArray[i].nBmpTexture )
            {
                FreeOpenGLTexture(pstuMdl->pstuVarOpenGLArray[i].nBmpTexture);
                pstuMdl->pstuVarOpenGLArray[i].nBmpTexture = 0;
            }
        }
        delete[] pstuMdl->pstuVarOpenGLArray;
        pstuMdl->pstuVarOpenGLArray = NULL;
    }
}
bool CRead3DRModelZK::MakeOpenGLTexture(nuFILE *pf3DR)
{
    if( NULL == m_pOpenGL )
    {
        return false;
    }
    FILE_3DR_BMP	bmp3DR;
    for(nuBYTE i = 0; i < m_pstuMdlArray[ m_nNowMdlCount ].stufileHeader.nGroupCount; ++i )
    {
        if( nuFseek( pf3DR, m_pstuMdlArray[ m_nNowMdlCount ].pstuGroupArray[i].nBmpAddr, NURO_SEEK_SET ) != 0 ||
            nuFread( &bmp3DR, sizeof(FILE_3DR_BMP) - 4, 1, pf3DR ) != 1 )
        {
            return false;
        }
        bmp3DR.pColorBuff = new char[bmp3DR.nBuffLen];
        if( NULL == bmp3DR.pColorBuff )
        {
            return false;
        }
        if( nuFread(bmp3DR.pColorBuff, bmp3DR.nBuffLen, 1, pf3DR) != 1 )
        {
            delete[] bmp3DR.pColorBuff;
            return false;
        }
        m_pstuMdlArray[ m_nNowMdlCount ].pstuVarOpenGLArray[i].nBmpTexture = m_pOpenGL->LoadTexture(bmp3DR);
        delete[] bmp3DR.pColorBuff;
    }
    return true;
}
void CRead3DRModelZK::FreeOpenGLTexture(nuUINT nTexture)
{
    if( m_pOpenGL )
    {
        m_pOpenGL->FreeTexture(nTexture);
    }
}
nuUINT CRead3DRModelZK::DrawModel()
{
    if( NULL == m_pOpenGL )
    {
        return 0;
    }    
    MODEL_3DMAX_DATA* p3DMdl = NULL;
    for(nuBYTE i = 0; i < m_nNowMdlCount; ++i)
    {        
        p3DMdl = &m_pstuMdlArray[i];
        nuLONG y = p3DMdl->lY - p3DMdl->dCenD;
//         nuLONG h = (nuLONG)(y*fScaley+187*p3DMdl->height/272);
//         float fScaley =1.15+(y*1.0f *272/ (p3DMdl->height*2000));
//         nuLONG h = (nuLONG)(y*fScaley+420*p3DMdl->height/272);
//         float fScaley = 2.4*272/ (p3DMdl->height);
        float fScaley = (float)1.17*272 / p3DMdl->height-(y*1.0f*272/(p3DMdl->height*2000));
        nuLONG h = (nuLONG)(y*fScaley+240);//*p3DMdl->height/272
        //int sss = (508*p3DMdl->fSclRef/p3DMdl->fZhRef-y*fScaley);
        h = (nuLONG)( h*p3DMdl->fZhRef/p3DMdl->fSclRef );        
        if (h<p3DMdl->maxHeight &&  (y<(-p3DMdl->height/2)))
        {
            continue;
        }
        if (y<(-p3DMdl->height))
        {
            continue; 
        }
        //m_pOpenGL->Draw3DModel(p3DMdl, p3DMdl->maxHeight, p3DMdl->maxHeight);
        m_pOpenGL->Draw3DModel(p3DMdl, p3DMdl->maxHeight, h);
    }
    //	m_pOpenGL->SwapBuffers();
    return 1;
}

#endif
