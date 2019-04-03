#pragma once
#include "NuroDefine.h"

class LoadDLL_NECVOX
{
public:
	typedef nuVOID (*ReadMemBufferProc)(nuVOID* pData, nuDWORD dwSize);
	typedef nuVOID (*WriteMemBufferProc)(const nuVOID* pData, nuDWORD dwSize);
	ReadMemBufferProc m_pfReadMemBuffer;
	WriteMemBufferProc m_pfWriteMemBuffer;
public:
	LoadDLL_NECVOX(nuVOID);
	~LoadDLL_NECVOX(nuVOID);
	nuBOOL Initial(const nuTCHAR *tsDLLPath);
	nuVOID WriteCom(nuBYTE *REGULUSBuf1, nuBYTE *REGULUSBuf2, nuBYTE *REGULUSBuf3);
	nuVOID FreeDLL();
protected:
	nuHINSTANCE	m_hInst;
	nuBOOL	m_bSendHUDChange;
};
