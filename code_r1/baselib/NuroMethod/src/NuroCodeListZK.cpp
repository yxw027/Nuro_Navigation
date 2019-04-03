// NuroCodeListZK.cpp: implementation of the CNuroCodeListZK class.
//
//////////////////////////////////////////////////////////////////////

#include "../NuroCodeListZK.h"
#include "../../NuroDefine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNuroCodeListZK::CNuroCodeListZK()
{
    m_pCodeList     = NULL;
    m_nCodeCount    = 0;
}

CNuroCodeListZK::~CNuroCodeListZK()
{
    DeleteCodeList();
}

nuBOOL CNuroCodeListZK::CreateCodeList(nuUINT nCodeCount /* = _CODE_NODE_DEFAULT_COUNT */)
{
    m_pCodeList = new NURO_CODE_NODE[nCodeCount];
    if( NULL == m_pCodeList )
    {
        return nuFALSE;
    }
    m_nCodeCount    = nCodeCount;
    m_nFIndex       = 0;
    m_nLIndex       = 0;
    m_nNowCount     = 0;
    m_bWorking      = nuFALSE;
    return nuTRUE;
}

nuVOID CNuroCodeListZK::DeleteCodeList()
{
    if( NULL != m_pCodeList )
    {
        delete[] m_pCodeList;
        m_pCodeList     = NULL;
        m_nCodeCount    = 0;
    }
}

nuBOOL CNuroCodeListZK::PushNode(const NURO_CODE_NODE& codeNode, nuBYTE nAcType /* = _CODE_ACTION_AT_END */)
{
    if( NULL == m_pCodeList )
    {
        return nuFALSE;
    }
    nuBOOL bRes = nuTRUE;
    while( m_bWorking )
    {
        nuSleep(25);
    }
    m_bWorking = nuTRUE;
    if( _CODE_ACTION_DEL_ALL_OTHERS == nAcType )
    {
        m_nNowCount = 0;
        m_nFIndex   = 0;
        m_nLIndex   = 0;
        m_pCodeList[m_nLIndex] = codeNode;
        ++m_nLIndex;
        ++m_nNowCount;
    }
    else if( _CODE_ACTION_AT_HEAD == nAcType )
    {
        if( 0 == m_nFIndex )
        {
            m_nFIndex = m_nCodeCount - 1;
        }
        else
        {
            --m_nFIndex;
        }
        m_pCodeList[m_nFIndex]  = codeNode;
        //////////////////////////////////////////
        if( m_nNowCount < m_nCodeCount )
        {
            ++m_nNowCount;
        }
        else
        {
            m_nLIndex = m_nFIndex;
        }
        ///////////////////////////////////////////
    }
    else if( _CODE_ACTION_FULL_COVER == nAcType )
    {
        if( m_nNowCount == m_nCodeCount )
        {
            m_pCodeList[m_nLIndex] = codeNode;
            m_nLIndex   = (m_nLIndex + 1) % m_nCodeCount;
            m_nFIndex   = m_nLIndex;
        }
        else
        {
            m_pCodeList[m_nLIndex]  = codeNode;
            ++m_nNowCount;
            m_nLIndex   = (m_nLIndex + 1) % m_nCodeCount;
        }
    }
    else if( _CODE_ACTION_FULL_DEL == nAcType )
    {
        if( m_nNowCount == m_nCodeCount )
        {
            m_nNowCount = 0;
            m_nFIndex   = 0;
            m_nLIndex   = 0;
            m_pCodeList[m_nLIndex] = codeNode;
            ++m_nLIndex;
            ++m_nNowCount;
        }
        else
        {
            m_pCodeList[m_nLIndex]  = codeNode;
            ++m_nNowCount;
            m_nLIndex   = (m_nLIndex + 1) % m_nCodeCount;
        }
    }
    else
    {
        if( m_nNowCount == m_nCodeCount )
        {
            bRes = nuFALSE;
        }
        else
        {
            m_pCodeList[m_nLIndex]  = codeNode;
            ++m_nNowCount;
            m_nLIndex   = (m_nLIndex + 1) % m_nCodeCount;
        }
    }
    m_bWorking = nuFALSE;
    return bRes;
}

nuBOOL CNuroCodeListZK::PopNode(NURO_CODE_NODE& codeNode, nuBYTE nAcType /* = _CODE_ACTION_AT_HEAD */)
{
    if( NULL == m_pCodeList || 0 == m_nNowCount )
    {
        return nuFALSE;
    }
    while( m_bWorking )
    {
        nuSleep(25);
    }
    m_bWorking = nuTRUE;
    if( _CODE_ACTION_AT_END == nAcType )
    {
        if( 0 == m_nLIndex )
        {
            m_nLIndex = m_nCodeCount - 1;
        }
        else
        {
            --m_nLIndex;
        }
        codeNode = m_pCodeList[m_nLIndex];
        --m_nNowCount;
    }
    else
    {
        codeNode = m_pCodeList[m_nFIndex];
        --m_nNowCount;
        m_nFIndex = (m_nFIndex + 1) % m_nCodeCount;
    }
    m_bWorking = nuFALSE;
    return nuTRUE;
}