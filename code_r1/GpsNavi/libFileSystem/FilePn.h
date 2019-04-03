// FilePn.h: interface for the CFilePn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEPN_H__590C0020_23F0_44F4_B93E_4BCA20291D21__INCLUDED_)
#define AFX_FILEPN_H__590C0020_23F0_44F4_B93E_4BCA20291D21__INCLUDED_

#include "NuroDefine.h"
#include "NuroEngineStructs.h"

class CFilePn  
{
public:
	CFilePn();
	virtual ~CFilePn();

	nuBOOL Initialize();
	nuVOID Free();

	static nuBOOL GetPoiName(nuWORD nDwIdx, nuLONG nNameAddr, nuPVOID pNameBuf, nuPWORD pBufLen);
};

#endif // !defined(AFX_FILEPN_H__590C0020_23F0_44F4_B93E_4BCA20291D21__INCLUDED_)
