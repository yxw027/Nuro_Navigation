// FileETG.h: interface for the CFileETG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEETG_H__INCLUDED_)
#define AFX_FILEETG_H__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"

class CFileETG  
{
public:
	CFileETG();
	virtual ~CFileETG();

	nuBOOL	Initialize();
	nuVOID	Free();
	
	nuBOOL  SearchETG(nuLONG lBlockIdx, nuLONG lRoadIdx, NUROPOINT ptRoad, NUROPOINT ptCar, NUROPOINT &ptEtagPoint);
	nuFILE	*m_pFile;
	nuLONG	m_lTotoalCount;
	nuLONG	m_lIndex;
	nuLONG  m_lMappingRoadIdx;
	nuLONG  m_lMappingBlockIdx;
	nuBOOL  m_bSendEtag;
	NUROPOINT *m_pptETG;
protected:

};

#endif 