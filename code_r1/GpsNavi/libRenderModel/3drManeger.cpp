#include "3drManeger.h"
#include "NuroDefine.h"

C3drManeger::C3drManeger()
{
    m_bInited = nuFALSE;
	m_n3drCnt=0;
}

C3drManeger::~C3drManeger()
{
    Free();
}

nuBOOL C3drManeger::Init(nuPVOID pcRsApi, CEglRender *pcEglRender)
{
	m_n3drCnt=0;
    if (NULL == pcRsApi || NULL == pcEglRender)
    {
        return nuFALSE;
    }
    Free();

    m_pcEglRender = pcEglRender;
    if (!m_c3drReader.Init(pcRsApi))
    {
        return nuFALSE;
    }
    if (!Init3drArr())
    {
        m_c3drReader.Free();
        return nuFALSE;
    }
    m_bInited = nuTRUE;

    return nuTRUE;
}

nuVOID C3drManeger::Free()
{
    if (m_bInited)
    {
        m_bInited = nuFALSE;
        for (nuUINT i = 0; i < m_n3drCnt; i++)
        {
            m_pcEglRender->DeleteTexture(m_apst3dr[i]->stMdl.nTexId);
            m_c3drReader.Free3drData(m_apst3dr[i]->stMdl.stData);
        }
        Free3drArr();
        m_c3drReader.Free();
    }
}

nuBOOL C3drManeger::Add3dr(nuLONG lID, nuLONG lX, nuLONG lY, nuCHAR *pszName)
{
    if (!m_bInited)
    {
        return nuFALSE;
    }
    _3DR *pst3dr = Find3dr(lID);
    if (nuNULL == pst3dr)
    {
        /// 插入模型
        pst3dr = Insert3dr();
        if (nuNULL == pst3dr)
        {
            return nuFALSE;
        }
        /// 读取数据
        _3DR_BMP stBmp;
        if (!m_c3drReader.Read3dr(pszName, pst3dr->stMdl.stData, stBmp))
        {
            Remove3dr(pst3dr);
			pst3dr=NULL;
            return nuFALSE;
        }
        /// 创建纹理
        pst3dr->stMdl.nTexId = m_pcEglRender->CreateTexture(stBmp);
        m_c3drReader.Free3drBmp(stBmp);
        if (0 == pst3dr->stMdl.nTexId)
        {
            m_c3drReader.Free3drData(pst3dr->stMdl.stData);
            Remove3dr(pst3dr);
			pst3dr=NULL;
            return nuFALSE;
        }
    }
    pst3dr->stInfo.lID = lID;
    pst3dr->stInfo.lX  = lX;
    pst3dr->stInfo.lY  = lY;
    
    return nuTRUE;
}

nuVOID C3drManeger::Del3dr(nuLONG lID)
{
    if (!m_bInited)
    {
        return;
    }
    _3DR *pst3dr = Find3dr(lID);
    if (nuNULL == pst3dr)
    {
        return;
    }
    m_pcEglRender->DeleteTexture(pst3dr->stMdl.nTexId);
    m_c3drReader.Free3drData(pst3dr->stMdl.stData);
    Remove3dr(pst3dr);
}

nuBOOL C3drManeger::Draw3dr() const
{
    if (!m_bInited)
    {
        return nuFALSE;
    }
    for (nuUINT i = 0; i < m_n3drCnt; i++)
    {
        m_pcEglRender->Draw3dr(m_apst3dr[i]->stInfo.lX, m_apst3dr[i]->stInfo.lY, m_apst3dr[i]->stMdl);
    }
    return nuTRUE;
}

//////////////////////////////////////////////////////////////////////////

nuBOOL C3drManeger::Init3drArr()
{
    for (nuUINT i = 0; i < _3DR_MAX_CNT; i++)
    {
        m_apst3dr[i] = &m_ast3dr[i];
    }
    m_n3drCnt = 0;
    return nuTRUE;
}

nuVOID C3drManeger::Free3drArr()
{
}

C3drManeger::_3DR *C3drManeger::Insert3dr()
{
    if (_3DR_MAX_CNT == m_n3drCnt)
    {
        return nuNULL;
    }
    return m_apst3dr[m_n3drCnt++];
}

nuVOID C3drManeger::Remove3dr(_3DR *pst3dr)
{
    for (nuUINT i = 0; i < m_n3drCnt; i++)
    {
        if (m_apst3dr[i] == pst3dr)
        {
            for (nuUINT j = i; j < m_n3drCnt - 1; j++)
            {
                m_apst3dr[j] = m_apst3dr[j + 1];
            }
            m_apst3dr[m_n3drCnt - 1] = pst3dr;
            m_n3drCnt--;
            break;
        }
    }
}

C3drManeger::_3DR *C3drManeger::Find3dr(nuLONG lID) const
{
    for (nuUINT i = 0; i < m_n3drCnt; i++)
    {
        if (m_apst3dr[i]->stInfo.lID == lID)
        {
            return m_apst3dr[i];
        }
    }
    return nuNULL;
}
