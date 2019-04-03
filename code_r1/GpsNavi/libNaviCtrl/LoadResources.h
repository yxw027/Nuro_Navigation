// LoadResources.h: interface for the CLoadResources class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADRESOURCES_H__5488EE78_57B9_4C64_BB11_B9AF962884AB__INCLUDED_)
#define AFX_LOADRESOURCES_H__5488EE78_57B9_4C64_BB11_B9AF962884AB__INCLUDED_

#include "NuroDefine.h"
#include "NuroModuleApiStruct.h"
#include "NuroNaviCtrlConfig.h"


class CLoadResources  
{
public:
	//typedef bool(*RS_InitResourcesProc)(PAPISTRUCTADDR);
	typedef nuBOOL(*RS_InitResourcesProc)(nuPVOID lpOutRS);
	typedef void(*RS_FreeResourcesProc)();
public:
	CLoadResources();
	virtual ~CLoadResources();

	//bool InitModule(PAPISTRUCTADDR pApiAddr);
	nuBOOL InitModule(nuPVOID lpOutRS);
	void UnLoadModule();

protected:
    nuHINSTANCE m_hInst; 
};

#endif // !defined(AFX_LOADRESOURCES_H__5488EE78_57B9_4C64_BB11_B9AF962884AB__INCLUDED_)
