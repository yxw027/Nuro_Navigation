// ReadLastPos.h: interface for the CReadLastPos class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_READLASTPOS_H__B0C33862_048A_4030_88E0_8B6037DBCFA7__INCLUDED_)
#define AFX_READLASTPOS_H__B0C33862_048A_4030_88E0_8B6037DBCFA7__INCLUDED_

#include "NuroDefine.h"

#define FILE_LASTPOS				nuTEXT("LastPosition.dat")
#define FILE_LASTPOS2				nuTEXT("LastPosition2.dat")
class CReadLastPos  
{
public:
	CReadLastPos();
	virtual ~CReadLastPos();

//	static nuBOOL ReadLastPos(const TCHAR *tsPath, nuLONG& x, nuLONG& y, nuSHORT& angle);
	static nuBOOL SaveLastGps(const nuTCHAR *tsPath, nuPVOID lpGPS);
	static nuBOOL NewReadLastPos(const nuTCHAR *tsPath, nuLONG&x, nuLONG &y, nuSHORT& angle);
	static nuBOOL ReadLastPos(const nuTCHAR *tsFile, nuLONG &x, nuLONG &y, nuSHORT &angle);

protected:
	static nuroPOINT	m_ptLastPoint;
	static nuINT		m_nSaveTimer;
};

#endif // !defined(AFX_READLASTPOS_H__B0C33862_048A_4030_88E0_8B6037DBCFA7__INCLUDED_)
