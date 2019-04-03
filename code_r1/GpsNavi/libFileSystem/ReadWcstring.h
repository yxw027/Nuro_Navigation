// ReadWcstring.h: interface for the CReadWcstring class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_READWCSTRING_H__DCF7A0B7_258C_4D1D_9658_ECEBB4DDB0E5__INCLUDED_)
#define AFX_READWCSTRING_H__DCF7A0B7_258C_4D1D_9658_ECEBB4DDB0E5__INCLUDED_

#include "NuroDefine.h"
#include "NuroConstDefined.h"
#include "NuroEngineStructs.h"

class CReadWcstring  
{
public:
	CReadWcstring();
	virtual ~CReadWcstring();

	nuBOOL Initialize(PGLOBALENGINEDATA	pGdata);
	nuVOID Free();

	nuINT  ReadConstWcs(nuBYTE nWcsType, nuLONG nRoadClass, nuWCHAR *wsBuf, nuWORD nLen);
	nuINT  ReadConstTxt(nuDWORD index, nuWCHAR *wsBuf, nuWORD nLen, nuTCHAR* tFile);

protected:
	PGLOBALENGINEDATA	m_pGdata;
};

#endif // !defined(AFX_READWCSTRING_H__DCF7A0B7_258C_4D1D_9658_ECEBB4DDB0E5__INCLUDED_)
