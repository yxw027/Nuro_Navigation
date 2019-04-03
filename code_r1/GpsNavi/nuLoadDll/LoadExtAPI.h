// LoadExtApi.h: interface for the CLoadNC class.
//
//////////////////////////////////////////////////////////////////////

#ifndef LOADEXTAPI
#define LOADEXTAPI

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"

#define DIR_LEFT		0x25
#define DIR_UP			0x26
#define DIR_RIGHT		0x27
#define DIR_DOWN		0x28

class CLoadExtApi
{
public:
	CLoadExtApi();
	virtual ~CLoadExtApi();

	bool LoadDll();
	void FreeDll();

	bool IsLoaden();
public:
	typedef bool(*NC_LcdScreenShiftAdjPro)(nuBYTE);
	NC_LcdScreenShiftAdjPro NC_LcdScreenShiftAdj;
	typedef bool(*NC_LcdScreenShiftSavePro)(void);
	NC_LcdScreenShiftSavePro NC_LcdScreenShiftSave;

protected:
	nuHINSTANCE	m_hInst;
};

#endif // 
