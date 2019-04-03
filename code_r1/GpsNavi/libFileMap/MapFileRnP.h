#if !defined(AFX_MapFileRnP_H__B0BE59B7_7F03_41A7_8513_2E3540F8F97D__INCLUDED_)
#define AFX_MapFileRnP_H__B0BE59B7_7F03_41A7_8513_2E3540F8F97D__INCLUDED_

#include "FileMapBaseZ.h"

class CMapFileRnP : public CFileMapBaseZ  
{
public:
	CMapFileRnP();
	virtual ~CMapFileRnP();

	nuBOOL  Open(long nMapID);
	nuUINT  GetLength();
	nuBOOL  ReadData(nuUINT addr, nuVOID *p, nuUINT len);
	nuBOOL  IsOpen();
	nuVOID  Close();

/*	nuBOOL  GetRNInfo();*/
	nuBOOL  GetRoadName(nuUINT addr, nuVOID* pBuffer, nuUINT nMaxLen, nuUINT *pResLen);
	nuBOOL  GetRoadDanyin(nuUINT addr, nuVOID* pBuffer, nuUINT nMaxLen, nuUINT *pResLen);

private:
	nuCFile file;
};

#endif // !defined(AFX_MapFileRnP_H__B0BE59B7_7F03_41A7_8513_2E3540F8F97D__INCLUDED_)
