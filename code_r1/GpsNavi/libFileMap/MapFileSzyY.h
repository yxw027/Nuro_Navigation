#if !defined(AFX_MAPFILESZYY_H__54521677_EC2E_45CD_AAEC_CFB1CA4E31C0__INCLUDED_)
#define AFX_MAPFILESZYY_H__54521677_EC2E_45CD_AAEC_CFB1CA4E31C0__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

class CMapFileSzyY : public CFileMapBaseZ  
{
public:
	CMapFileSzyY();
	virtual ~CMapFileSzyY();

	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();
	
	nuUINT GetLength();
	nuBOOL ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT len);
	nuUINT GetSzyInfo_CITY(nuWORD nCityID, nuBYTE* pBuffer, nuUINT nMaxLen);
private:
	nuCFile file;
	nuWORD totalcount;
};

#endif // !defined(AFX_MAPFILESZYY_H__54521677_EC2E_45CD_AAEC_CFB1CA4E31C0__INCLUDED_)
