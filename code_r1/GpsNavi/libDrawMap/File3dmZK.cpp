// File3dmZK.cpp: implementation of the CFile3dmZK class.
//
//////////////////////////////////////////////////////////////////////

#include "File3dmZK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define _3DR_NAME_MAX_LEN				12

CFile3dmZK::CFile3dmZK()
{
	m_nTotalCount = 0;
	m_pPtCoor	= NULL;
}

CFile3dmZK::~CFile3dmZK()
{
	Delete3dmCoor();
}

nuBOOL CFile3dmZK::Read3dmCoor(nuTCHAR* ptzName)
{
	if( NULL != m_pPtCoor )
	{
		return nuTRUE;
	}
	if( NULL == ptzName )
	{
		return nuFALSE;
	}
	nuFILE* pFile = nuTfopen(ptzName, NURO_FS_RB);
	if( NULL == pFile )
	{
		return nuFALSE;
	}
	if( nuFseek(pFile, 0, NURO_SEEK_SET) != 0 ||
		nuFread(&m_nTotalCount, 4, 1, pFile) != 1 )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	m_pPtCoor = new nuroPOINT[m_nTotalCount];
	if( NULL == m_pPtCoor )
	{
		nuFclose(pFile);
		return nuFALSE;
	}
	if( nuFread(m_pPtCoor, sizeof(nuroPOINT)*m_nTotalCount, 1, pFile) != 1 )
	{
		delete []m_pPtCoor;
		m_pPtCoor = NULL;
		nuFclose(pFile);
		return nuFALSE;
	}
	nuFclose(pFile);
	return nuTRUE;
}

nuVOID CFile3dmZK::Delete3dmCoor()
{
	if( NULL != m_pPtCoor )
	{
		delete[] m_pPtCoor;
		m_pPtCoor		= NULL;
		m_nTotalCount	= 0;
	}
}

nuBOOL CFile3dmZK::Read3drName(nuTCHAR* pTz3dm, nuDWORD nIdx3dr, nuCHAR* ps3dr, nuWORD nBuflen)
{
	if( NULL == pTz3dm || NULL == ps3dr )
	{
		return nuFALSE;
	}
	nuFILE* pf3dm = nuTfopen(pTz3dm, NURO_FS_RB);
	if( NULL == pf3dm )
	{
		return nuFALSE;
	}
	nIdx3dr = nIdx3dr * _3DR_NAME_MAX_LEN + 4 + m_nTotalCount * sizeof(nuroPOINT);
    nuMemset(ps3dr, 0, nBuflen);
	if( nuFseek(pf3dm, nIdx3dr, NURO_SEEK_SET)		!= 0 ||
		nuFread(ps3dr, _3DR_NAME_MAX_LEN, 1, pf3dm) != 1 )
	{
		nuFclose(pf3dm);
		return nuFALSE;
	}
	ps3dr[_3DR_NAME_MAX_LEN] = 0;
    nuFclose(pf3dm);
	return nuTRUE;
}
