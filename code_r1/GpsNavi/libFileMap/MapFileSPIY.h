// MapFileSPIY.h: interface for the CMapFileSPIY class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPFILESPIY_H__99E6B07F_6BEF_4649_829E_BE7F46A738EB__INCLUDED_)
#define AFX_MAPFILESPIY_H__99E6B07F_6BEF_4649_829E_BE7F46A738EB__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

class CMapFileSPIY : public CFileMapBaseZ 
{
public:
	CMapFileSPIY();
	virtual ~CMapFileSPIY();
	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();
	
	nuUINT	GetTypeNum();
	nuUINT	GetLength();
	nuBOOL	ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT len);
	nuUINT	GetSPTypeNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
private:
	nuUINT	typeCount;
	nuCFile	file;
};

#endif // !defined(AFX_MAPFILESPIY_H__99E6B07F_6BEF_4649_829E_BE7F46A738EB__INCLUDED_)
