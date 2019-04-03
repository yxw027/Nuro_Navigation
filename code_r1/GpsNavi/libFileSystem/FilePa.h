// FilePa.h: interface for the CFilePa class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEPA_H__20068AC8_305D_426A_9B7C_CA7D30A2A54D__INCLUDED_)
#define AFX_FILEPA_H__20068AC8_305D_426A_9B7C_CA7D30A2A54D__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"

class CFilePa  
{
public:
	CFilePa();
	virtual ~CFilePa();
	
	static nuBOOL GetPa(nuLONG nMapIdx, nuDWORD nAddress, PPOIPANODE pPaNode);
};

#endif // !defined(AFX_FILEPA_H__20068AC8_305D_426A_9B7C_CA7D30A2A54D__INCLUDED_)
