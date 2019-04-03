// FileMapBaseZ.cpp: implementation of the CFileMapBaseZ class.
//
//////////////////////////////////////////////////////////////////////
#include "FileMapBaseZ.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PMEMMGRAPI	CFileMapBaseZ::m_pMmApi	= NULL;
PMEMMGRAPI_SH	CFileMapBaseZ::m_pMm_SHApi	= NULL;
PFILESYSAPI	CFileMapBaseZ::m_pFsApi	= NULL;

CFileMapBaseZ::CFileMapBaseZ()
{

}

CFileMapBaseZ::~CFileMapBaseZ()
{

}

nuBOOL CFileMapBaseZ::InitData(PAPISTRUCTADDR pApiAddr)
{
	m_pMmApi	= pApiAddr->pMmApi;
	m_pFsApi	= pApiAddr->pFsApi;
	m_pMm_SHApi    = pApiAddr->pMm_SHApi;
	return nuTRUE;
}

nuVOID CFileMapBaseZ::FreeData()
{
}

nuBOOL CFileMapBaseZ::Initialize()
{
	return nuTRUE;
}

nuVOID CFileMapBaseZ::Free()
{
	;
}
