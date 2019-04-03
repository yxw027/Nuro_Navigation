// LoadRenderModelZK.h: interface for the CLoadRenderModelZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADRENDERMODELZK_H__EC0B8B68_F353_441B_B352_BA20B601904E__INCLUDED_)
#define AFX_LOADRENDERMODELZK_H__EC0B8B68_F353_441B_B352_BA20B601904E__INCLUDED_

#include "NuroDefine.h"
#include "NuroRenderModel.h"

#ifdef  USE_DYM_DLL
#define _USE_RENDERMODEL_DLL
#else
#define  _USE_RENDERMODEL_LIB
#endif

class CLoadRenderModelZK  
{
public:
	typedef nuBOOL (*RM_InitRenderModelProc)(PAPI_RENDERMODEL pApiRM, nuPVOID pApiForRM);
	typedef nuVOID (*RM_FreeRenderModelProc)();
public:
	CLoadRenderModelZK();
	virtual ~CLoadRenderModelZK();

	nuBOOL LoadModule(nuPVOID lpApiOut, nuPVOID lpApiIn);
	//nuVOID FreeModule();
	nuVOID FreeModuleH();
protected:
	nuHINSTANCE	m_hInst;
};

#endif // !defined(AFX_LOADRENDERMODELZK_H__EC0B8B68_F353_441B_B352_BA20B601904E__INCLUDED_)
