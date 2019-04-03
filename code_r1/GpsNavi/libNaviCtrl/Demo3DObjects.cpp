// Demo3DObjects.cpp: implementation of the CDemo3DObjects class.
//
//////////////////////////////////////////////////////////////////////

#include "Demo3DObjects.h"
#include "NuroModuleApiStruct.h"

//extern FILESYSAPI					g_fileSysApi;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDemo3DObjects::CDemo3DObjects()
{
    nuMemset(&m_3dHead, 0, sizeof(m_3dHead));
    nuMemset(&m_3dNode, 0, sizeof(m_3dNode));
    m_nNowIndex = 0;
    m_pFile     = NULL;
}

CDemo3DObjects::~CDemo3DObjects()
{
    Close3DDemo();
}

nuBOOL CDemo3DObjects::Start3DDemo(const nuTCHAR* ptsName)
{
    if( NULL == ptsName )
    {
        return nuFALSE;
    }
    if( NULL != m_pFile )
    {
        Close3DDemo();
    }
    m_pFile = nuTfopen(ptsName, NURO_FS_RB);
    if( NULL == m_pFile )
    {
        return nuFALSE;
    }
    if( nuFseek(m_pFile, 0, NURO_SEEK_SET) != 0 || 
        nuFread(&m_3dHead, sizeof(m_3dHead), 1, m_pFile) != 1 ||
        nuFread(&m_3dNode, sizeof(m_3dNode), 1, m_pFile) != 1 )
    {
        Close3DDemo();
        return nuFALSE;
    }
    m_nNowIndex = 0;
    m_nNowTimes = 0;
    nuBYTE nAngle = 0;
    if( m_3dHead.nPerAngle < 0 )
    {
        nAngle = -m_3dHead.nPerAngle;
    }
    else if( m_3dHead.nPerAngle == 0 )
    {
        nAngle = 1;
    }
    else
    {
        nAngle = m_3dHead.nPerAngle;
    }
    m_nTotalTimes   = 360 / nAngle * m_3dHead.nCircle;
    return nuTRUE;
}

nuBOOL CDemo3DObjects::Close3DDemo()
{
    if( m_pFile != NULL )
    {
        nuFclose(m_pFile);
        m_pFile = NULL;
		return nuTRUE;
    }
	return nuFALSE;
}

nuBOOL CDemo3DObjects::Demo3D(DEMO_3D_DATA &demo3D)
{
    if( NULL != m_pFile )
    {
        if( m_nNowTimes >= m_nTotalTimes )
        {
            m_nNowIndex = (m_nNowIndex + 1) % m_3dHead.nPtCount;
            m_nNowTimes = 0;
            if( nuFseek(m_pFile, sizeof(DEMO_3D_HEAD) + m_nNowIndex * sizeof(DEMO_3D_NODE), NURO_SEEK_SET) != 0 ||
                nuFread(&m_3dNode, sizeof(DEMO_3D_NODE), 1, m_pFile) != 1)
            {
                return nuFALSE;
            }
        }
        else
        {
            ++m_nNowTimes;
        }
        demo3D.point.x  = m_3dNode.nX;
        demo3D.point.y  = m_3dNode.nY;
        demo3D.nAngle   = m_3dNode.nAngle + m_nNowTimes * m_3dHead.nPerAngle;
        return nuTRUE;
    }
    return nuFALSE;
}
