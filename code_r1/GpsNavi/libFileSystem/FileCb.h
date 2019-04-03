// FileCb.h: interface for the CFileCb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILECB_H__E58AFB40_0763_4F76_BCF9_8250658B9853__INCLUDED_)
#define AFX_FILECB_H__E58AFB40_0763_4F76_BCF9_8250658B9853__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"

class CFileCb  
{
public:
	CFileCb();
	virtual ~CFileCb();

	nuBOOL	Initialize();
	nuVOID	Free();

	static nuBOOL ReadCityTownName(nuLONG nMapIdx, nuDWORD nCityTownCode, nuWCHAR *wsName, nuWORD nWsNum);

protected:

};

#endif // !defined(AFX_FILECB_H__E58AFB40_0763_4F76_BCF9_8250658B9853__INCLUDED_)
