#ifndef __NURO_LOADCALLMGR_20090721
#define __NURO_LOADCALLMGR_20090721

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"

class LoadCallmgr  
{
public:
	LoadCallmgr();
	virtual ~LoadCallmgr();

	bool LoadDll();
	void FreeDll();

	bool IsLoaden();

	bool	m_bUseDLL;

public:
	typedef bool(*CA_InitCallMgrPro)(nuHDC, nuWORD, nuWORD);
	CA_InitCallMgrPro	 CA_InitCallMgr;
	
	typedef bool(*CA_InitCallMgrNewPro)(nuHWND, nuHDC, nuWORD, nuWORD);
	CA_InitCallMgrNewPro	 CA_InitCallMgrNew;	

	typedef void(*CA_FreeCallMgrPro)();
	CA_FreeCallMgrPro	 CA_FreeCallMgr;
	
	typedef void(*CA_OnPaintPro)();
	CA_OnPaintPro	 CA_OnPaint;

	typedef void(*CA_KeyDownPro)(nuUINT);
	CA_KeyDownPro	 CA_KeyDown;

	typedef void(*CA_KeyUpPro)(nuUINT);
	CA_KeyUpPro	 CA_KeyUp;

	typedef nuUINT(*CA_MouseDownPro)(long, long);
	CA_MouseDownPro	 CA_MouseDown;

	typedef nuUINT(*CA_MouseUpPro)(long, long);
	CA_MouseUpPro	 CA_MouseUp;

	typedef nuUINT(*CA_MouseMovePro)(long, long);
	CA_MouseMovePro	 CA_MouseMove;

#ifdef _WINDOWS
	typedef void(*CA_vRegisterWindowMessagePro)();
	CA_vRegisterWindowMessagePro	 CA_vRegisterWindowMessage;

	typedef bool(*CA_OnCopyDataPro)(COPYDATASTRUCT*);
	CA_OnCopyDataPro	 CA_OnCopyData;

	typedef bool(*CA_WindowProcPro)(UINT , WPARAM , LPARAM );
	CA_WindowProcPro	 CA_WindowProc;

	typedef bool(*CA_OnCommandPro)(WPARAM , LPARAM);
	CA_OnCommandPro	 CA_OnCommand;
#endif

	typedef nuUINT(*CA_SetGPSFuncPro)(nuPVOID pFunc);
	CA_SetGPSFuncPro CA_SetGPSFunc;

protected:
	nuHINSTANCE	m_hInst;
};

#endif
