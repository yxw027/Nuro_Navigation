// FileContentMem.h: interface for the CFileContentMem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILECONTENTMEM_H__4646EC0F_8271_41DD_9937_35305E0F67C3__INCLUDED_)
#define AFX_FILECONTENTMEM_H__4646EC0F_8271_41DD_9937_35305E0F67C3__INCLUDED_

#include "../NuroDefine.h"
class CFileContentMem  
{
public:
	CFileContentMem();
	virtual ~CFileContentMem();

	nuBOOL  ReadFileContent(nuCHAR *pFile);
	nuBOOL  ReadFileContent(nuWCHAR *pFile);
	nuBOOL  GetLineContentW(nuWCHAR *pInOut, nuLONG nLenStr);
	nuBOOL  GetLineContentC(nuCHAR *pInOut, nuLONG nLenStr);
	nuBOOL  FreeContentMem();
	nuBOOL  ResetMemPos();

private:
	nuVOID    *m_pMem;
	nuLONG    m_nMemLen;
	nuLONG    m_nMemPos;
};

#endif // !defined(AFX_FILECONTENTMEM_H__4646EC0F_8271_41DD_9937_35305E0F67C3__INCLUDED_)
