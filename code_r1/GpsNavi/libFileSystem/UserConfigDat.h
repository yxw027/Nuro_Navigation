// UserConfigDat.h: interface for the CUserConfigDat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERCONFIGDAT_H__54B4D00E_2AF2_4D31_8816_54560B853C89__INCLUDED_)
#define AFX_USERCONFIGDAT_H__54B4D00E_2AF2_4D31_8816_54560B853C89__INCLUDED_

#include "NuroDefine.h"
#include "NuroOpenedDefine.h"

class CUserConfigDat  
{
public:
	CUserConfigDat();
	virtual ~CUserConfigDat();

	nuBOOL	Initialize();
	nuVOID	Free();

	nuBOOL  SaveUserCfgDat(nuTCHAR *tsFile);
	nuBOOL	CoverFromDefault();

protected:
	nuBOOL	ReadUserCfgDat(nuTCHAR *tsFile);

public:
	USERCONFIG		m_userCfg;
};

#endif // !defined(AFX_USERCONFIGDAT_H__54B4D00E_2AF2_4D31_8816_54560B853C89__INCLUDED_)
