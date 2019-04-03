// LoadGdiZK.h: interface for the CLoadGdiZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADGDIZK_H__80DC86A4_F048_4953_87F1_23B17F6713F2__INCLUDED_)
#define AFX_LOADGDIZK_H__80DC86A4_F048_4953_87F1_23B17F6713F2__INCLUDED_

#include "NuroDefine.h"

#ifdef USE_DYM_DLL
#define _USE_GDI_DLL
#else
#define _USE_GDI_LIB
#endif

#ifdef _USE_GDI_DLL
#include "NuroLoadDll.h"
#else
#include "../libGDI/libGDI.h"
#endif

#include "NuroCommonApi.h"

#ifdef _USE_GDI_DLL
class CLoadGdiZK : public CNuroLoadDll
#else
class CLoadGdiZK
#endif
{
public:
	CLoadGdiZK();
	virtual ~CLoadGdiZK();

    nuBOOL  LoadGdi(const nuTCHAR* ptsDllFold, NURO_SCREEN* pScreen, nuTCHAR* ptsFontFile);
    nuVOID  FreeGdi();

public:
    PNURO_API_GDI       m_pGdi;

};

#endif // !defined(AFX_LOADGDIZK_H__80DC86A4_F048_4953_87F1_23B17F6713F2__INCLUDED_)
