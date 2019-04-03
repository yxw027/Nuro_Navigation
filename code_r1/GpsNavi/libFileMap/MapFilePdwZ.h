// MapFilePdwZ.h: interface for the CMapFilePdwZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPFILEPDWZ_H__EA2BE9AC_F05B_4FDB_9416_7DB7A581EBDC__INCLUDED_)
#define AFX_MAPFILEPDWZ_H__EA2BE9AC_F05B_4FDB_9416_7DB7A581EBDC__INCLUDED_

#include "FileMapBaseZ.h"

class CMapFilePdwZ : public CFileMapBaseZ
{
public:
	CMapFilePdwZ();
	virtual ~CMapFilePdwZ();
	nuBOOL ReadPdwBlk(nuWORD nDwIdx, nuDWORD nBlkAddr, nuPWORD pMemIdx);
	
	nuVOID CloseFile();
	nuBOOL OpenFile();

protected:
	nuFILE*		m_pFile;
	nuWORD		m_nMapIdx;
};

#endif // !defined(AFX_MAPFILEPDWZ_H__EA2BE9AC_F05B_4FDB_9416_7DB7A581EBDC__INCLUDED_)
