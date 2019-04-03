// MapFileRdwZ.h: interface for the CMapFileRdwZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPFILERDWZ_H__7E054883_4348_4CF6_A2C3_81D7D5AA8A59__INCLUDED_)
#define AFX_MAPFILERDWZ_H__7E054883_4348_4CF6_A2C3_81D7D5AA8A59__INCLUDED_

#include "FileMapBaseZ.h"

class CMapFileRdwZ : public CFileMapBaseZ
{
public:
public:
	CMapFileRdwZ();
	virtual ~CMapFileRdwZ();

	nuBOOL ReadRdwBlk(nuWORD nDwIdx, nuDWORD nBlkAddr, nuPWORD pMemIdx);	
	nuBOOL GetRoadSegData(nuWORD nDwIdx, PROAD_SEG_DATA pRoadSegData);

	nuVOID CloseFile();
	nuBOOL OpenFile();

protected:
	nuFILE*		m_pFile;
	nuWORD		m_nMapIdx;
};

#endif // !defined(AFX_MAPFILERDWZ_H__7E054883_4348_4CF6_A2C3_81D7D5AA8A59__INCLUDED_)
