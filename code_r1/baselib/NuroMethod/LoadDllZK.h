// LoadDllZK.h: interface for the CLoadDllZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADDLLZK_H__BABE7CEF_9064_4204_9FE3_84AD49426E4D__INCLUDED_)
#define AFX_LOADDLLZK_H__BABE7CEF_9064_4204_9FE3_84AD49426E4D__INCLUDED_

#include "../NuroDefine.h"

class CLoadDllZK  
{
public:
    typedef nuPVOID(*DLL_InitModelProc)(nuPVOID);
    typedef nuVOID(*DLL_FreeModelProc)();

public:
	CLoadDllZK(const nuTCHAR* pDllName, const nuCHAR* pDllInitName, const nuCHAR* pDllFreeName);
	virtual ~CLoadDllZK();

    nuPVOID LoadDll(const nuTCHAR* ptsDll, nuPVOID pParam);
    nuVOID  FreeDll();

protected:
    nuHINSTANCE             m_hInst;
    const static nuTCHAR    sc_tsPostfix[];
    const nuTCHAR*          m_pcDllName;
    const nuCHAR*           m_pcDllInitName;
    const nuCHAR*           m_pcDllFreeName;
};

#endif // !defined(AFX_LOADDLLZK_H__BABE7CEF_9064_4204_9FE3_84AD49426E4D__INCLUDED_)
