// SearchBaseZK.cpp: implementation of the CSearchBaseZK class.
//
//////////////////////////////////////////////////////////////////////
#include "SearchBaseZK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PMEMMGRAPI_SH CSearchBaseZK::m_pMmApi = NULL;
SEARCHSET CSearchBaseZK::m_searchSet;
CSearchBaseZK::CSearchBaseZK()
{
	nuMemset(&m_searchSet, 0, sizeof(SEARCHSET));
}

CSearchBaseZK::~CSearchBaseZK()
{
}

nuBOOL CSearchBaseZK::InitData(PAPISTRUCTADDR pApiAddr)
{
	m_pMmApi = pApiAddr->pMm_SHApi;
	return nuTRUE;
}
nuVOID CSearchBaseZK::FreeData()
{
	;
}

nuBOOL CSearchBaseZK::Initialize()
{
	nuMemset(&m_searchSet, 0, sizeof(SEARCHSET));
	return nuTRUE;
}

nuVOID CSearchBaseZK::Free()
{

}

nuVOID CSearchBaseZK::InitSearchSet()
{
	nuMemset(&m_searchSet, 0, sizeof(SEARCHSET));
}

nuBOOL CSearchBaseZK::AddKeyMask(nuWCHAR nKey)
{
	if( m_keyMask.nNowCount == DY_KEY_MAX_NUM )
	{
		return nuFALSE;
	}
	for(nuBYTE i = 0; i < m_keyMask.nNowCount; ++i)
	{
		if( m_keyMask.pKeyEnable[i] == nKey )
		{
			return nuFALSE;
		}
	}
	m_keyMask.pKeyEnable[ m_keyMask.nNowCount++ ] = nKey;
	return nuTRUE;
}
/*
nuUINT CSearchBaseZK::SetSearchRegion(nuBYTE nSetMode, nuDWORD nData)
{
	return 0;
}
*/