// Demo3DObjects.h: interface for the CDemo3DObjects class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEMO3DOBJECTS_H__61482A5E_BEB8_44E4_AA4D_DF99167C03EF__INCLUDED_)
#define AFX_DEMO3DOBJECTS_H__61482A5E_BEB8_44E4_AA4D_DF99167C03EF__INCLUDED_

#include "NuroDefine.h"

typedef struct tagDEMO_3D_DATA
{
    nuroPOINT   point;
    nuINT       nAngle;
}DEMO_3D_DATA, *PDEMO_3D_DATA;

class CDemo3DObjects
{
public:
    typedef struct tagDEMO_3D_HEAD
    {
        nuWORD  nPtCount;
        nuBYTE  nCircle;
        nuCHAR  nPerAngle;
    }DEMO_3D_HEAD, *PDEMO_3D_HEAD;
    typedef struct tagDEMO_3D_NODE
    {
        nuLONG  nX;
        nuLONG  nY;
        nuLONG  nAngle;
    }DEMO_3D_NODE, *PDEMO_3D_NODE;
public:
	CDemo3DObjects();
	virtual ~CDemo3DObjects();

    nuBOOL  Start3DDemo(const nuTCHAR* ptsName);
    nuBOOL  Close3DDemo();
    nuBOOL  Demo3D(DEMO_3D_DATA &demo3D);

protected:
    DEMO_3D_HEAD    m_3dHead;
    DEMO_3D_NODE    m_3dNode;
    nuUINT          m_nNowIndex;
    nuUINT          m_nTotalTimes;
    nuUINT          m_nNowTimes;
    nuFILE*         m_pFile;
};

#endif // !defined(AFX_DEMO3DOBJECTS_H__61482A5E_BEB8_44E4_AA4D_DF99167C03EF__INCLUDED_)
