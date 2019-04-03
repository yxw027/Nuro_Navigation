// Object3DCtrlZK.cpp: implementation of the CObject3DCtrlZK class.
//
//////////////////////////////////////////////////////////////////////

#include "Object3DCtrlZK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObject3DCtrlZK::CObject3DCtrlZK()
{
	m_bScale	= nuFALSE;
	m_nTopY = 0;
}

CObject3DCtrlZK::~CObject3DCtrlZK()
{

}

nuBOOL CObject3DCtrlZK::SetUp3D(nuINT nSW /* = 480 */, 
							    nuINT nSH /* = 272 */, 
							    nuINT nDy /* = 240 */, 
							    nuINT nDz /* = 0 */, 
							    nuINT nViewX /* = -1 */, 
							    nuINT nViewY /* = -1 */, 
							    nuINT nViewDis /* = 10 */)
{

	if( -1 == nDy )
	{
		nDy = nSH * DYDS;
	}
	m_nScreenW	= nSW;
	m_nScreenH	= nSH;
	m_nDy		= nDy;
	m_nDz		= nDz;
	if( -1 == nViewX )
	{
		nViewX = nSW / 2;
	}
	m_nViewX	= nViewX;
	if( -1 == nViewY )
	{
		nuDOUBLE fDyz = nuSqrt(m_nDy * m_nDy + m_nDz * m_nDz);
		m_nViewY	= (nuINT)(nViewDis * m_nDz / fDyz + 0.5) + m_nDy;
		m_nViewZ	= (nuINT)(nViewDis * m_nDy / fDyz + 0.5);
	}
	else
	{
		m_nViewY = nViewY;
		m_nViewZ = nViewDis;
	}
	//
	nuINT nTotal = m_nViewY * m_nDz + m_nViewZ * m_nDy;
	m_nVDy = nTotal / m_nDz;
	m_nVDz = nTotal / m_nDy;
	m_nScaleD = 1;
	m_nScaleN = SCALExN;
	m_nScaleC = m_nScaleN - m_nScaleD;
	m_bScale = nuTRUE;
	nuLONG x = m_nViewX;
	nuLONG y = m_nScreenH + 20;
	nuLONG z = 0;
	Screen3Dto2D(x, y, z);
	m_nYmax = y;
	m_nTopY	= (m_nDy * m_nDz - m_nDy * m_nViewZ) / m_nDz;
	x = m_nViewX;
	y = 0;
	z = 0;
	Screen2Dto3D(x, y, z);
	if( y > 0 )
	{
		m_nSkyH = y;
	}
	else
	{
		m_nSkyH = 0;
	}
	return nuTRUE;
}

nuBOOL CObject3DCtrlZK::Screen2Dto3D(nuLONG& x, nuLONG& y, nuLONG& z)
{
	nuDOUBLE dDz = m_nDz, dDy = m_nDy, dViewY = m_nViewY, dViewZ = m_nViewZ, ddTmp = 0;
	nuDOUBLE dTmpx = 0, dTmpy = 0;
	if( 0 == m_nDz )
	{
		return nuTRUE;
	}
	if( m_bScale )
	{
		x = (m_nScaleN * x - m_nScaleC * m_nViewX) / m_nScaleD;
		y = (m_nScaleN * y - m_nScaleC * m_nDy) / m_nScaleD;
		z = z * m_nScaleN / m_nScaleD;
	}
	nuDOUBLE dx = x - m_nViewX;
	nuDOUBLE dy = y - m_nViewY;
	nuDOUBLE dz = z - m_nViewZ;
	nuDOUBLE dTmp = dDz * dy + dDy * dz;

	if( 0 == dTmp )
	{
		return nuFALSE;
	}

	dTmpy = (dDz*dDy*dy + dDy*dViewY*dz - dDy*dViewZ*dy);
	y = dTmpy / dTmp;
	dTmpx = (dDz*dDy - dDz*dViewY - dDy*dViewZ) / dTmp;
	x = m_nViewX + dx*dTmpx;//m_nViewX + dx*(fy - m_nViewY) / dy;
	m_nTopY = (m_nDy * m_nDz - m_nViewZ * m_nDy) / m_nDz;
	return nuTRUE;
}

nuBOOL CObject3DCtrlZK::Screen3Dto2D(nuLONG& x, nuLONG& y, nuLONG& z)
{
	if( y <= m_nTopY )
	{
		return nuFALSE;
	}
	nuINT dx = x - m_nViewX;
	nuINT dy = y - m_nViewY;
	nuINT nTmp = m_nDz*m_nDy-y*m_nDz-m_nViewZ*m_nDy;
	if( nTmp == 0 )
	{
		return nuFALSE;
	}
	x = m_nViewX - m_nViewZ*m_nDy*dx / nTmp;
	y = m_nViewY - m_nViewZ*m_nDy*dy / nTmp;
	if( m_bScale )
	{
		x = (m_nScaleD*x + m_nScaleC*m_nViewX) / m_nScaleN;
		y = (m_nScaleD*y + m_nScaleC*m_nDy) / m_nScaleN;
	}
	z = 0;
	return nuTRUE;
}

nuLONG CObject3DCtrlZK::Get3DThick(nuLONG nThick, nuLONG y)
{
	if( y < m_nTopY )
	{
		return 0;
	}
	if( m_bScale )
	{
		return m_nScaleN * nThick * (y - m_nTopY) / ((m_nDy - m_nTopY)*m_nScaleD);
	}
	else
	{
		return nThick * (y - m_nTopY) / (m_nDy - m_nTopY);
	}
}
