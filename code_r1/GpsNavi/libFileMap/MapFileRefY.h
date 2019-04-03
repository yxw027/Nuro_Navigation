#if !defined(AFX_MAPFILEREFY_H__08207871_6C20_49EA_A5DA_CDF5F16E2E18__INCLUDED_)
#define AFX_MAPFILEREFY_H__08207871_6C20_49EA_A5DA_CDF5F16E2E18__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"
#include "nuSearchDefine.h"

class CMapFileRefY : public CFileMapBaseZ  
{
public:
	CMapFileRefY();
	virtual ~CMapFileRefY();
	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();
	nuUINT GetTotalNum();
	nuUINT GetRtInfo(const nuWORD &nCityID, const nuWORD &nTownID, nuVOID *pBuffer, nuUINT nMaxLen);
	static nuUINT GetRtIdxEx(const nuLONG &nMapID, const nuWORD &nCityID, const nuWORD &nTownID);
private:
	nuCFile file;
	nuUINT total;
};

#endif // !defined(AFX_MAPFILEREFY_H__08207871_6C20_49EA_A5DA_CDF5F16E2E18__INCLUDED_)
