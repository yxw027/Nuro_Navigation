// DrawMdwZ.h: interface for the CDrawMdwZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWMDWZ_H__D4A19A98_CE4F_413F_8600_9F6F9A62ACC7__INCLUDED_)
#define AFX_DRAWMDWZ_H__D4A19A98_CE4F_413F_8600_9F6F9A62ACC7__INCLUDED_

#include "NuroDefine.h"
#include "NuroMapFormat.h"
#include "MapFileMdwZ.h"

//#define _USE_MDW_DRAW

class CDrawMdwZ
{
public:
	CDrawMdwZ();
	virtual ~CDrawMdwZ();

	nuBOOL initialize();
	nuVOID free();

	nuBOOL LoadMdwData(nuroRECT& rtMapSize);
	nuVOID FreeMdwData();
	nuBOOL DrawMdwZ();

	nuBOOL ColMdwData();

protected:
	CMapFileMdwZ	m_fileMdw;
	PMDWDATA		m_pMdwData;
	nuDWORD			m_nMdwSizeLoaden;
};

#endif // !defined(AFX_DRAWMDWZ_H__D4A19A98_CE4F_413F_8600_9F6F9A62ACC7__INCLUDED_)
