#if !defined(AFX_MAPFILERNCY_H__89D2D939_061B_43E2_B90C_3DB7F8331775__INCLUDED_)
#define AFX_MAPFILERNCY_H__89D2D939_061B_43E2_B90C_3DB7F8331775__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

class CMapFileRncY : public CFileMapBaseZ  
{
public:
	CMapFileRncY();
	virtual ~CMapFileRncY();

	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();
	
	nuUINT GetCityCount();
	nuUINT GetTownCount(nuINT nCity , nuDWORD &nStartaddr);
	nuUINT GetLength();
	nuBOOL ReadData(nuUINT addr, nuVOID *pBuffer, nuUINT nLen);
	nuUINT GetRNCInfo(nuWORD nCityID, nuWORD nTownID, nuBYTE* pBuffer, nuUINT nMaxLen);
private:
	nuCFile file;
	SEARCH_RNC_HEADER header;
	nuUINT citycount;
};

#endif // !defined(AFX_MAPFILERNCY_H__89D2D939_061B_43E2_B90C_3DB7F8331775__INCLUDED_)
