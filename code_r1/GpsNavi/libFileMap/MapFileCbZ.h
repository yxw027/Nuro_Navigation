#if !defined(AFX_MAPFILECBZ_H__BF925B01_6688_4476_AFCB_F23AC5F1388B__INCLUDED_)
#define AFX_MAPFILECBZ_H__BF925B01_6688_4476_AFCB_F23AC5F1388B__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"
#include "nuSearchDefine.h"
class CMapFileCbZ : public CFileMapBaseZ
{
public:
	CMapFileCbZ();
	virtual ~CMapFileCbZ();
	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();
	nuUINT GetLength();
	nuUINT GetCityNum() const;
	nuUINT GetTownNum() const;
	nuUINT GetCityName(nuUINT nStartCID, nuUINT nNum, nuVOID* pBuffer, nuUINT nBufferLen);

	nuUINT	GetTownInfo_CTIdx(nuVOID* pBuffer, nuUINT nBufferLen);
	nuWORD	GetTownInfo_CityCode(nuUINT nTownAddr);
	nuWORD	GetTownInfo_TownCode(nuUINT nTownAddr);
	nuBOOL	GetTownInfo(nuUINT nTownAddr, SEARCH_CB_TOWNINFO* pTownInfo );
	nuBOOL	ReadData(nuUINT nDataAddr, nuVOID *pBuffer, nuUINT nLen);
private:
	nuCFile file;
	SEARCH_CB_HEADER cbHeader;
};

#endif // !defined(AFX_MAPFILECBZ_H__BF925B01_6688_4476_AFCB_F23AC5F1388B__INCLUDED_)
