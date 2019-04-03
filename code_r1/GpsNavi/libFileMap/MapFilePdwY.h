#if !defined(AFX_MAPFILEPDWY_H__C86979F7_53A1_4486_8184_91270C60D841__INCLUDED_)
#define AFX_MAPFILEPDWY_H__C86979F7_53A1_4486_8184_91270C60D841__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroDefine.h"
#include "nuSearchDefine.h"

class CMapFilePdwY : public CFileMapBaseZ  
{
public:
	CMapFilePdwY();
	virtual ~CMapFilePdwY();
	nuBOOL Open(nuLONG nMapID);
	nuVOID Close();
	nuBOOL IsOpen();
	nuUINT GetBlkPoiNum(nuUINT blkaddr);
	nuUINT GetBlkPoiInfo(nuUINT blkaddr, nuVOID *pBuffer, nuUINT nMaxLen);
	nuBOOL GetBlkPoiInfo_ByIdx(nuUINT blkaddr, nuUINT idx, SEARCH_PDW_A1 *pInfo);	
private:	
	nuCFile file;
};

#endif // !defined(AFX_MAPFILEPDWY_H__C86979F7_53A1_4486_8184_91270C60D841__INCLUDED_)
