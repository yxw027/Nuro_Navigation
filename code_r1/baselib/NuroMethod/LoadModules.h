// LoadModules.h: interface for the CLoadModules class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADMODULES_H__AE809D67_71BA_4359_A33A_29C447207160__INCLUDED_)
#define AFX_LOADMODULES_H__AE809D67_71BA_4359_A33A_29C447207160__INCLUDED_

#include "../NuroDefine.h"

class CLoadModules  
{
public:
    typedef nuPVOID(*DLL_InitModel)(nuPVOID);
    typedef nuVOID(*DLL_FreeModel)();
public:
	CLoadModules();
	virtual ~CLoadModules();

    static nuVOID SetPath(const nuTCHAR* ptsPath);
    nuPVOID  LoadDll(nuTCHAR* pTsDllName, nuCHAR* pTsInitFc, nuPVOID pLpParam);
    nuVOID   FreeDll(nuCHAR* pTsInitFc);

protected:
    static nuTCHAR        s_tsPath[NURO_MAX_PATH];
    nuHINSTANCE           m_hInst;

};

#endif // !defined(AFX_LOADMODULES_H__AE809D67_71BA_4359_A33A_29C447207160__INCLUDED_)
