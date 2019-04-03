// FileRn.h: interface for the CFileRn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILERN_H__21249658_FDD5_4CC6_B2B4_4788F6F79340__INCLUDED_)
#define AFX_FILERN_H__21249658_FDD5_4CC6_B2B4_4788F6F79340__INCLUDED_

#include "NuroDefine.h"
#include "NuroConstDefined.h"
#include "NuroEngineStructs.h"

class CFileRn  
{
public:
	CFileRn();
	virtual ~CFileRn();

	nuBOOL Initialize();
	nuVOID Free();

	nuBOOL GetRoadName( nuWORD nDwIdx, nuLONG nNameAddr, nuINT nRoadClass, nuPVOID pNameBuf, 
		                nuWORD nBufLen, PRNEXTEND pRnEx );
	nuVOID RoadNameProc(nuWCHAR* pNameBuf, nuWORD nNameNum, PRNEXTEND pRnEx);

protected:
	nuWCHAR	m_wsLane;
	nuWCHAR	m_wsAlley;

};

#endif // !defined(AFX_FILERN_H__21249658_FDD5_4CC6_B2B4_4788F6F79340__INCLUDED_)
