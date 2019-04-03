// FileSLP.h: interface for the CFileSLP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FileSLP_H__20068AC8_305D_426A_9B7C_CA7D30A2A54D__INCLUDED_)
#define AFX_FileSLP_H__20068AC8_305D_426A_9B7C_CA7D30A2A54D__INCLUDED_

//#include "NuroDefine.h"
//#include "NuroMapFormat.h"
#include "FileMapBaseZ.h"

class CFileSLP  : public CFileMapBaseZ
{
public:
	CFileSLP();
	virtual ~CFileSLP();

	nuBOOL LOAD_SLP(nuLONG lMapIdx);

	nuLONG FindSLP(nuLONG lMapIdx,nuDWORD nBlkIdx,nuDWORD nRoadIdx,PSLPDATA TempData);

	nuVOID Free();
private:
	SLPHEARDER m_SLP_Hearder;
	SLPDATA **m_ppSLP_Data;
	
};

#endif // !defined(AFX_FileSLP_H__20068AC8_305D_426A_9B7C_CA7D30A2A54D__INCLUDED_)
