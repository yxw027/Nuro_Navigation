#include "LoadDLL_NECVOX.h"

LoadDLL_NECVOX::LoadDLL_NECVOX(nuVOID)
{

}


LoadDLL_NECVOX::~LoadDLL_NECVOX(nuVOID)
{

}

nuBOOL LoadDLL_NECVOX::Initial(const nuTCHAR *tsDLLPath)
{
	nuTCHAR tsPath[512] = {0};
	nuTcscpy(tsPath, tsDLLPath);
	nuTcscat(tsPath, nuTEXT("MemShare.dll"));
	m_hInst = NULL;
	m_hInst = nuLoadLibrary(tsPath);
	if( m_hInst == NULL )
	{
		return nuFALSE;
	}
	m_pfWriteMemBuffer = (WriteMemBufferProc)nuGetProcAddress(m_hInst, "WriteMemBuffer");
	if( m_pfWriteMemBuffer == NULL)
	{
		return nuFALSE;
	}
	m_bSendHUDChange = nuTRUE;
	return nuTRUE;
}
nuVOID LoadDLL_NECVOX::WriteCom(nuBYTE *REGULUSBuf1, nuBYTE *REGULUSBuf2, nuBYTE *REGULUSBuf3)
{
	if(NULL == m_hInst || NULL == m_pfWriteMemBuffer)
	{
		return;
	}
	if(m_bSendHUDChange)
	{
		m_pfWriteMemBuffer(REGULUSBuf1, sizeof(nuBYTE) * 15);
		m_bSendHUDChange = nuFALSE;
	}
	else
	{
		m_pfWriteMemBuffer(REGULUSBuf3, sizeof(nuBYTE) * 27);
		m_bSendHUDChange = nuTRUE;
	}
}
nuVOID LoadDLL_NECVOX::FreeDLL()
{
	if(NULL != m_hInst)
	{
		nuFreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}
