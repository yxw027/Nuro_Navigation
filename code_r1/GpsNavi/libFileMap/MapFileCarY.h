#if !defined(AFX_MAPFILECARY_H__13941B8B_B5A0_45D0_8B0F_DD570326C8FF__INCLUDED_)
#define AFX_MAPFILECARY_H__13941B8B_B5A0_45D0_8B0F_DD570326C8FF__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

class CMapFileCarY : public CFileMapBaseZ  
{
public:
	CMapFileCarY();
	virtual ~CMapFileCarY();
	
	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();

	nuUINT	GetTypeNum();
	nuUINT	GetLength();
	nuBOOL	ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT len);
	nuUINT	GetCarFacNameAll(nuVOID *pBuffer, nuUINT nMaxLen);
private:
	nuUINT	typeCount;
	nuCFile	file;
};

#endif // !defined(AFX_MAPFILECARY_H__13941B8B_B5A0_45D0_8B0F_DD570326C8FF__INCLUDED_)
