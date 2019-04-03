#if !defined(AFX_LOADCOMOPEN_H__E7F93736_A105_473E_806E_AF6B64BE3BA4__INCLUDED_)
#define AFX_LOADCOMOPEN_H__E7F93736_A105_473E_806E_AF6B64BE3BA4__INCLUDED_

#include "NuroDefine.h"
//#include "nurUIConst.h"
#include "NuroOpenedDefine.h"
#include "NuroModuleApiStruct.h"


#ifdef _WINDOWS
#define _USE_COMOPEN_DLL
#endif

class CLoadComOpen 
{
public:
    CLoadComOpen();
    virtual ~CLoadComOpen();

    bool LoadDll();
    void FreeDll();
    bool IsLoaden();
public:
    typedef  bool(*COM_InitComOpenProc)(/*PAPISTRUCTADDR pApiAddr*/);
    COM_InitComOpenProc COM_InitComOpen;
    typedef  void(*COM_FreeComOpenProc)();
    COM_FreeComOpenProc COM_FreeComOpen;
    typedef  bool(*COM_OpenProc)();
    COM_OpenProc COM_Open;
    typedef  bool(*COM_CloseProc)();
    COM_CloseProc COM_Close;
    typedef  int(*COM_RecvDataProc)(const char *, int);
    COM_RecvDataProc COM_RecvData;
    typedef  bool(*COM_GetXYProc)(char* wInstruct,char wInstructM[10][5],long *pX, long *pY,long pXM[10], long pYM[10]);
	COM_GetXYProc COM_GetXY;
	typedef  bool(*COM_ReturnXYProc)(long *pX, long *pY);
    COM_ReturnXYProc COM_ReturnXY;

protected:
    nuHINSTANCE    m_hInst;
};

#endif // !defined(AFX_LOADNC_H__E7F93736_A105_473E_806E_AF6B64BE3BA4__INCLUDED_)
