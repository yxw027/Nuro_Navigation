
#if !defined(AFX_NUCFILE_H__88E56C8E_AF0E_472E_ACC9_53BE7015E773__INCLUDED_)
#define AFX_NUCFILE_H__88E56C8E_AF0E_472E_ACC9_53BE7015E773__INCLUDED_

#include "../NuroDefine.h"

class nuCFile  
{
public:
	nuCFile();
	virtual ~nuCFile();	
	virtual	nuBOOL  Open(const nuTCHAR *fileName, nuINT mode);
	virtual nuBOOL  IsOpen();
	virtual nuVOID  Close();
	virtual nuINT   Seek(nuLONG nDis, nuINT nType);
	virtual nuSIZE	Tell();
	virtual nuSIZE	Write(const nuVOID *pBuff, nuSIZE size, nuSIZE count);
	virtual nuSIZE	Read(nuVOID *pBuff, nuSIZE size, nuSIZE count);	
	virtual nuSIZE	GetLength();
	virtual nuSIZE	ReadLineW(nuWCHAR *pBuff,  nuSIZE nMaxLen);
	virtual nuSIZE	ReadLineA(nuCHAR *pBuff,  nuSIZE nMaxLen);
	virtual nuBOOL	ReadData(nuUINT nDataStartAddr, nuVOID *pBuffer, nuUINT nLen);

private:
	nuFILE* fp;
};

#endif // !defined(AFX_NUCFILE_H__88E56C8E_AF0E_472E_ACC9_53BE7015E773__INCLUDED_)
