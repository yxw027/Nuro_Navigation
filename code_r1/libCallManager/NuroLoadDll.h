// NuroLoadDll.h: interface for the CNuroLoadDll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUROLOADDLL_H__11102DDC_691D_49A8_AE6C_81A0DB13F66C__INCLUDED_)
#define AFX_NUROLOADDLL_H__11102DDC_691D_49A8_AE6C_81A0DB13F66C__INCLUDED_

#include "NuroDefine.h"

class CNuroLoadDll  
{
public:
    typedef nuPVOID(*DLL_InitModule)(nuPVOID);
    typedef nuVOID(*DLL_FreeModule)();
public:
	CNuroLoadDll();
    virtual ~CNuroLoadDll();

protected:
    nuPVOID LoadDll(const nuTCHAR* ptsDllName, const nuCHAR* ptsFcInit, nuPVOID pLpParam);
    nuVOID  FreeDll(const nuCHAR* ptsFcFree);

protected:
    nuHINSTANCE         m_hInst;

};

#endif // !defined(AFX_NUROLOADDLL_H__11102DDC_691D_49A8_AE6C_81A0DB13F66C__INCLUDED_)
