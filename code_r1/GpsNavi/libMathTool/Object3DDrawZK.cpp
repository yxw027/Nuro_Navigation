// Object3DDrawZK.cpp: implementation of the CObject3DDrawZK class.
//
//////////////////////////////////////////////////////////////////////

#include "Object3DDrawZK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObject3DDrawZK::CObject3DDrawZK()
{
	m_pGdiApi = NULL;
}

CObject3DDrawZK::~CObject3DDrawZK()
{

}

nuBOOL CObject3DDrawZK::Initialize(PGDIAPI pGdiApi)
{
	m_pGdiApi = pGdiApi;
	return nuTRUE;
}

nuUINT CObject3DDrawZK::Poly3DLine(nuroPOINT *pPt, nuLONG nCount)
{
	if( pPt == NULL || nCount < 2 )
	{
		return 0;
	}
	nuroPOINT *ptList = new nuroPOINT[nCount];
	if( ptList == NULL )
	{
		return 0;
	}
	nuLONG nListCount = 0;
	nuroPOINT pt1 = {0}, pt2 = {0}, ptNew = {0};
	pt1 = pt2 = pPt[0];
	nuBOOL bLastOut = nuFALSE, bNowOut = nuFALSE;
	nuLONG z = 0;
	if( pt1.y > m_obj3DCtrl.m_nYmax )
	{
		bLastOut = nuTRUE;
	}
	for(nuINT i = 1; i < nCount; i++)
	{
		pt1 = pt2;
		pt2 = pPt[i];
		if( pt2.y > m_obj3DCtrl.m_nYmax )
		{
			bNowOut = nuTRUE;
		}
		else
		{
			bNowOut = nuFALSE;
		}
		if( !bLastOut )
		{
			ptList[nListCount] = pt1;
			m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
			nListCount ++;
			if( bNowOut )
			{
				ptNew.y = m_obj3DCtrl.m_nYmax;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				ptList[nListCount] = ptNew;
				m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
				nListCount ++;
				if( nListCount > 1 )
				{
					m_pGdiApi->GdiPolyline(ptList, nListCount);
				}
				nListCount = 0;
			}
		}
		else
		{
			if( !bNowOut )
			{
				ptNew.y = m_obj3DCtrl.m_nYmax;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				nListCount = 0;
				ptList[nListCount] = ptNew;
				m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
				nListCount ++;
			}
		}
		bLastOut = bNowOut;
	}
	if( !bNowOut )
	{
		ptList[nListCount] = pt2;
		m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
		nListCount ++;
	}
	if( nListCount > 1 )
	{
		m_pGdiApi->GdiPolyline(ptList, nListCount);
	}
	delete []ptList;
	ptList=NULL;
	return 1;
}

nuUINT CObject3DDrawZK::Poly3DLine(nuroPOINT *pPt, nuLONG nCount, nuLONG nLineWidth)
{
	if( pPt == NULL || nCount < 2 )
	{
		return 0;
	}
	nuroPOINT *ptList = new nuroPOINT[nCount];
	if( ptList == NULL )
	{
		return 0;
	}
	nuLONG nListCount = 0;
	nuLONG z = 0;
	nuroPOINT pt1 = {0}, pt2 = {0}, ptNew = {0};
	pt1 = pt2 = pPt[0];
	nuBOOL bLastOut = nuFALSE, bNowOut = nuFALSE;
	if( pt1.y > m_obj3DCtrl.m_nYmax )
	{
		bLastOut = nuTRUE;
	}
	for(nuINT i = 1; i < nCount; i++)
	{
		pt1 = pt2;
		pt2 = pPt[i];
		if( pt2.y > m_obj3DCtrl.m_nYmax )
		{
			bNowOut = nuTRUE;
		}
		else
		{
			bNowOut = nuFALSE;
		}
		if( !bLastOut )
		{
			ptList[nListCount] = pt1;
			m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
			nListCount ++;
			if( bNowOut )
			{
				ptNew.y = m_obj3DCtrl.m_nYmax;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				ptList[nListCount] = ptNew;
				m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
				nListCount ++;
				if( nListCount > 1 )
				{
					m_pGdiApi->Gdi3DPolyline(ptList, nListCount);
				}
				nListCount = 0;
			}
		}
		else
		{
			if( !bNowOut )
			{
				ptNew.y = m_obj3DCtrl.m_nYmax;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				nListCount = 0;
				ptList[nListCount] = ptNew;
				m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
				nListCount ++;
			}
		}
		bLastOut = bNowOut;
	}
	if( !bNowOut )
	{
		ptList[nListCount] = pt2;
		m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
		nListCount ++;
	}
	if( nListCount > 1 )
	{
		m_pGdiApi->Gdi3DPolyline(ptList, nListCount);
	}
	delete []ptList;
	ptList=NULL;
	return 1;
}

nuUINT CObject3DDrawZK::Poly3DGon(nuroPOINT *pPt, nuLONG nCount)
{
	
	if( pPt == NULL || nCount < 3 )
	{
		return 0;
	}
	nuLONG nListCount = nCount*3/2 + 1;
	nuroPOINT *ptList = new nuroPOINT[nListCount];
	if( ptList == NULL )
	{
		return 0;
	}
	nListCount = 0;
	nuroPOINT pt1 = {0}, pt2 = {0}, ptNew = {0};
	nuBOOL bAddTwoPt = nuFALSE;
	pt2 = pPt[0];
	nuLONG z = 0;
	for(nuINT i = 1; i <= nCount; i++)
	{
		bAddTwoPt = nuFALSE;
		pt1 = pt2;
		if( i == nCount )
		{
			pt2 = pPt[0];
		}
		else
		{
			pt2 = pPt[i];
		}
		if( pt1.y > m_obj3DCtrl.m_nYmax && pt2.y > m_obj3DCtrl.m_nYmax )
		{
			continue;
		}
		else if( pt1.y > m_obj3DCtrl.m_nYmax && pt2.y <= m_obj3DCtrl.m_nYmax )
		{
			ptNew.y = m_obj3DCtrl.m_nYmax;
			ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
			pt1 = ptNew;
		}
		else if( pt1.y <= m_obj3DCtrl.m_nYmax && pt2.y > m_obj3DCtrl.m_nYmax )
		{
			ptNew.y = m_obj3DCtrl.m_nYmax;
			ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
			bAddTwoPt = nuTRUE;
		}
		ptList[nListCount] = pt1;
		m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
		nListCount++;
		if( bAddTwoPt )
		{
			ptList[nListCount] = ptNew;
			m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
			nListCount++;
		}
	}
	if( nListCount > 2 )
	{
		m_pGdiApi->GdiPolygon(ptList, nListCount);
	}
	delete []ptList;
	ptList=NULL;
	return 1;
}

nuUINT CObject3DDrawZK::Dash3DLine(nuroPOINT *pPt, nuLONG nCount, nuINT nDashLine)
{
	if( pPt == NULL || nCount < 2 )
	{
		return nuFALSE;
	}
	nuroPOINT *ptList = new nuroPOINT[nCount];
	if( ptList == NULL )
	{
		return nuFALSE;
	}
	nuLONG nListCount = 0;
	nuroPOINT pt1 = {0}, pt2 = {0}, ptNew = {0};
	pt1 = pt2 = pPt[0];
	nuBOOL bLastOut = nuFALSE, bNowOut = nuFALSE;
	nuLONG z = 0;
	if( pt1.y > m_obj3DCtrl.m_nYmax )
	{
		bLastOut = nuTRUE;
	}
	for(nuINT i = 1; i < nCount; i++)
	{
		pt1 = pt2;
		pt2 = pPt[i];
		if( pt2.y > m_obj3DCtrl.m_nYmax )
		{
			bNowOut = nuTRUE;
		}
		else
		{
			bNowOut = nuFALSE;
		}
		if( !bLastOut )
		{
			ptList[nListCount] = pt1;
			m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
			nListCount ++;
			if( bNowOut )
			{
				ptNew.y = m_obj3DCtrl.m_nYmax;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				ptList[nListCount] = ptNew;
				m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
				nListCount ++;
				if( nListCount > 1 )
				{
					m_pGdiApi->GdiDashLine(ptList, nListCount, nDashLine);
				}
				nListCount = 0;
			}
		}
		else
		{
			if( !bNowOut )
			{
				ptNew.y = m_obj3DCtrl.m_nYmax;
				ptNew.x = (ptNew.y - pt1.y)*(pt2.x - pt1.x)/(pt2.y - pt1.y) + pt1.x;
				nListCount = 0;
				ptList[nListCount] = ptNew;
				m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
				nListCount ++;
			}
		}
		bLastOut = bNowOut;
	}
	if( !bNowOut )
	{
		ptList[nListCount] = pt2;
		m_obj3DCtrl.Screen2Dto3D(ptList[nListCount].x, ptList[nListCount].y, z);
		nListCount ++;
	}
	if( nListCount > 1 )
	{
		m_pGdiApi->GdiDashLine(ptList, nListCount, nDashLine);
	}
	delete []ptList;
	ptList=NULL;
	return 1;
}

