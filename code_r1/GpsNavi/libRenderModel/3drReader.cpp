#include "3drReader.h"
#include "NuroDefine.h"
#include "NuroModuleApiStruct.h"

C3drReader::C3drReader()
:m_ptzPackRPath(nuTEXT("Model\\TW_3DR.pck"))
{
    m_bInited = nuFALSE;
}

C3drReader::~C3drReader()
{
    Free();
}

nuBOOL C3drReader::Init(nuPVOID pcRsApi)
{
    if (nuNULL == pcRsApi)
    {
        return nuFALSE;
    }
    Free();
    
    m_pcRsApi = (POPENRSAPI)pcRsApi;   
    __android_log_print(ANDROID_LOG_INFO, "navi", "libNC C3drReader pRsApi %x", pcRsApi); 
    m_bInited = nuTRUE;

    return nuTRUE;
}

nuVOID C3drReader::Free()
{
    if (m_bInited)
    {
        m_bInited = nuFALSE;
    }
}

nuBOOL C3drReader::Read3dr(const nuCHAR *pszName, _3DR_DATA &stData, _3DR_BMP &stBmp)
{
    if (!m_bInited || NULL == pszName)
    {
        return nuFALSE;
    }
    if (!LoadPack())
    {
        return nuFALSE;
    }

    nuCHAR szTexName[_3DR_TEX_NAME_BUF_LEN] = "";

    if (!Read3drData(pszName, stData, szTexName))
    {
	    UnLoadPack();
        return nuFALSE;
    }
    if (!Read3drBmp(szTexName, stBmp))
    {
        Free3drData(stData);
	    UnLoadPack();
        return nuFALSE;
    }
    UnLoadPack();
    return nuTRUE;
}

nuVOID C3drReader::Free3drData(_3DR_DATA &stData) const
{
    if (m_bInited)
    {
        delete[] stData.pbyBuf;
        nuMemset(&stData, 0, sizeof(stData));
    }
}

nuVOID C3drReader::Free3drBmp(_3DR_BMP &stBmp) const
{
    if (m_bInited)
    {
        delete[] stBmp.pbyBuf;
        nuMemset(&stBmp, 0, sizeof(stBmp));
    }
}

//////////////////////////////////////////////////////////////////////////

nuBOOL C3drReader::LoadPack()
{
    nuTCHAR tzPackAPath[NURO_MAX_PATH];
    if (nuGetModulePath(nuNULL, tzPackAPath, NURO_MAX_PATH - nuTcslen(m_ptzPackRPath)) == 0)
    {
        return nuFALSE;
    }
    nuTcscat(tzPackAPath, m_ptzPackRPath);
    nuUINT nPackLen;
    m_pstPack = m_pcRsApi->RS_LoadPack(tzPackAPath, LOAD_PACK_FILE, nPackLen);
    if (nuNULL == m_pstPack)
    {
        return nuFALSE;
    }
    return nuTRUE;
}

nuVOID C3drReader::UnLoadPack()
{
    if (nuNULL != m_pstPack)
    {
        m_pcRsApi->RS_UnloadPack(m_pstPack);
        m_pstPack = nuNULL;
    }
}
#include <stdio.h>
nuBOOL C3drReader::Read3drData(const nuCHAR *pszName, _3DR_DATA &stData, nuCHAR *pszTexName) const
{
    /// 读取数据
    nuCHAR sz3drName[NURO_MAX_PATH] = "/";
    nuVOID *pvFile = m_pcRsApi->RS_FindFile(m_pstPack, nuStrcat(sz3drName, pszName));
    if (nuNULL == pvFile)
    {
        return nuFALSE;
    }
    nuUINT nFileLen = m_pcRsApi->RS_GetFileLength(m_pstPack, pvFile);
    stData.pbyBuf = new(std::nothrow) nuBYTE[nFileLen];
    if (nuNULL == stData.pbyBuf)
    {
        return nuFALSE;
    }
    if (!m_pcRsApi->RS_LoadFileData(m_pstPack, pvFile, stData.pbyBuf, nFileLen))
    {
        delete[] stData.pbyBuf;        
        return nuFALSE;
    }

    /// 解析数据
    const _3DR_HEADER *pst3drHeader = reinterpret_cast<const _3DR_HEADER *>(stData.pbyBuf);
    if (_3DR_MAGIC != pst3drHeader->dwMagic)
    {
        delete[] stData.pbyBuf;
        return nuFALSE;
    }
    stData.nTriangleCnt    = pst3drHeader->dwTriangleCnt;
    stData.pafxTriVertex   = reinterpret_cast<_3drVertex_t *>(stData.pbyBuf + pst3drHeader->dwTriVertexOfs);
    stData.pafxTriTexCoord = reinterpret_cast<_3drTexCoord_t *>(stData.pbyBuf + pst3drHeader->dwTriTexCoordOfs);
    nuMemcpy(stData.aafxAABB, pst3drHeader->aafxAABB, sizeof(_3drVertex_t) * 8);
    stData.nVertexCnt      = pst3drHeader->dwVertexCnt;
    stData.pafxVertex      = reinterpret_cast<_3drVertex_t *>(stData.pbyBuf + pst3drHeader->dwVertexOfs);
    stData.psVertexIdx     = reinterpret_cast<GLushort *>(stData.pbyBuf + pst3drHeader->dwVertexIdxOfs);

    nuStrncpy(pszTexName, pst3drHeader->szTexName, _3DR_TEX_NAME_BUF_LEN);

    return nuTRUE;
}

nuBOOL C3drReader::Read3drBmp(const nuCHAR *pszName, _3DR_BMP &stBmp) const
{
    /// 读取数据
    nuCHAR szRbpName[NURO_MAX_PATH] = "/";
    nuVOID *pvFile = m_pcRsApi->RS_FindFile(m_pstPack, nuStrcat(szRbpName, pszName));
    if (nuNULL == pvFile)
    {
        return nuFALSE;
    }
    nuUINT nFileLen = m_pcRsApi->RS_GetFileLength(m_pstPack, pvFile);
    stBmp.pbyBuf = new(std::nothrow) nuBYTE[nFileLen];
    if (nuNULL == stBmp.pbyBuf)
    {
        return nuFALSE;
    }
    if (!m_pcRsApi->RS_LoadFileData(m_pstPack, pvFile, stBmp.pbyBuf, nFileLen))
    {
        delete[] stBmp.pbyBuf;
        return nuFALSE;
    }

    /// 解析数据
    const BMP_HEADER *pstBmpHeader = reinterpret_cast<const BMP_HEADER *>(stBmp.pbyBuf);
    stBmp.nHeight    = pstBmpHeader->dwHeight;
    stBmp.nWidth     = pstBmpHeader->dwWidth;
    stBmp.pbyBmpData = reinterpret_cast<GLbyte *>(stBmp.pbyBuf + sizeof(BMP_HEADER));

    //printf("&&& %d %d\n", stBmp.nWidth, stBmp.nHeight);
#if 1
	short *p = (short *)stBmp.pbyBmpData;

	{
		//static short ss = 0;
		for (int i = 0; i < 256 * 256; i++)
        {
            // 
            //p[i] &= 0xf7de;//fff;
        }
		
		//for (int i = 0; i < 256 * 256; i++)
		{
            //1111 0000 0001 1111
            //p[i] |= ~0xf7de;
			//p[i] = 0xe800;    //i;//ss;//0x07e0;
			//if (p[i] & ~0xfbff)
            //{
                // 1111 0111 1101 1110 
            //    printf("%x\n", p[i] & ~0xfbff);    
            //}
		}
		//ss++;
	}
#endif
	

    return nuTRUE;
}
