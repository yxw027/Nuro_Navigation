// SearchBaseZK.h: interface for the CSearchBaseZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEARCHBASEZK_H__D7E4DD2F_1AA8_4F53_BD20_4CAF8FDC239E__INCLUDED_)
#define AFX_SEARCHBASEZK_H__D7E4DD2F_1AA8_4F53_BD20_4CAF8FDC239E__INCLUDED_

#include "NuroDefine.h"
#include "NuroModuleApiStruct.h"


typedef struct tagSEARCHSET
{
	nuUINT	nMode;
	nuWORD	nMapIdx;
	nuWORD	nCityID;
	nuWORD	nTownID;
}SEARCHSET, *PSEARCHSET;

class CSearchBaseZK  
{
public:
	CSearchBaseZK();
	virtual ~CSearchBaseZK();

	nuBOOL Initialize();
	nuVOID Free();

	nuVOID InitSearchSet();
	nuBOOL AddKeyMask(nuWCHAR nKey);


	static nuBOOL InitData(PAPISTRUCTADDR pApiAddr);
	static nuVOID FreeData();

	static nuUINT SetSearchMode(nuUINT nMode) { m_searchSet.nMode = nMode; }

//	virtual nuUINT SetSearchRegion(nuBYTE nSetMode, nuDWORD nData);
	

protected:
	static SEARCHSET	m_searchSet;
	DYKEYMASK	m_keyMask;
	static PMEMMGRAPI_SH		m_pMmApi;

	//////////////////////////////////////////////////////////////////////////
};

#endif // !defined(AFX_SEARCHBASEZK_H__D7E4DD2F_1AA8_4F53_BD20_4CAF8FDC239E__INCLUDED_)
