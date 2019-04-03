// LoadNC.cpp: implementation of the CLoadNC class.
//
//////////////////////////////////////////////////////////////////////
//#include "windows.h"
#include "LoadExtAPI.h"
#include "NuroDefine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadExtApi::CLoadExtApi()
{
#ifdef NURO_USE_PNAVI
	m_hInst = NULL;
#endif
}

CLoadExtApi::~CLoadExtApi()
{
	FreeDll();
}

bool CLoadExtApi::LoadDll()
{
#ifdef NURO_USE_PNAVI
	m_hInst = nuLoadLibrary(nuTEXT("\\Windows\\ExtAPI.dll"));
	if( m_hInst == NULL )
	{
		return false;
	}
	NC_LcdScreenShiftAdj  = (NC_LcdScreenShiftAdjPro)nuGetProcAddress(m_hInst, nuTEXT("LcdScreenShiftAdj"));
	NC_LcdScreenShiftSave = (NC_LcdScreenShiftSavePro)nuGetProcAddress(m_hInst, nuTEXT("LcdScreenShiftSave"));
#endif
	
	return true;
}

void CLoadExtApi::FreeDll()
{
	if( IsLoaden() )
	{
#ifdef NURO_USE_PNAVI
		nuFreeLibrary(m_hInst);
#endif
		m_hInst = NULL;
	}
}

bool CLoadExtApi::IsLoaden()
{
	if( m_hInst )
	{
		return true;
	}
	else
	{
		return false;
	}
}
