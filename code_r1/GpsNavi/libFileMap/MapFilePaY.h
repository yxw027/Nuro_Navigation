#if !defined(AFX_MAPFILEPAY_H__9B43904E_259D_47F7_9CAC_32FBF909A6F6__INCLUDED_)
#define AFX_MAPFILEPAY_H__9B43904E_259D_47F7_9CAC_32FBF909A6F6__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

class CMapFilePaY : public CFileMapBaseZ  
{
public:
	CMapFilePaY();
	virtual ~CMapFilePaY();
	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();
	nuUINT GetLength();
	nuBOOL ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT len);
	nuBOOL GetPaInfo(nuUINT infoAddr, SEARCH_PA_INFO *pPaInfo);
private:
	nuCFile file;
};

#endif // !defined(AFX_MAPFILEPAY_H__9B43904E_259D_47F7_9CAC_32FBF909A6F6__INCLUDED_)
