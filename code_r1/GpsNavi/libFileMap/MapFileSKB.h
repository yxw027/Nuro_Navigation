// MapFileSKB.h: interface for the CMapFileSKB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPFILESKB_H__CE63E872_E253_4D80_8A44_F4C6AA457204__INCLUDED_)
#define AFX_MAPFILESKB_H__CE63E872_E253_4D80_8A44_F4C6AA457204__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"

typedef struct tagSEARCH_SKB_CITYINFO
{
    nuUINT cityID;
    nuUINT poiCount;
    nuUINT poiPos;
    nuUINT roadCount;
    nuUINT roadPos;
}SEARCH_SKB_CITYINFO;
typedef struct tagSEARCH_SKB_DYDATA
{
    
}SEARCH_SKB_DYDATA;

class CMapFileSKB : public CFileMapBaseZ  
{
public:
	CMapFileSKB();
	virtual ~CMapFileSKB();
    nuBOOL Open(nuLONG nMapID);
    nuBOOL GetCityInfo(nuWORD cityID, SEARCH_SKB_CITYINFO& info);
    nuBOOL ReadData(nuUINT nDataStartAddr, nuVOID *pBuffer, nuUINT nLen);
    
    nuBOOL IsOpen();
	nuVOID Close();

private:
    nuCFile file;
    nuUINT  cityCount;
};

#endif // !defined(AFX_MAPFILESKB_H__CE63E872_E253_4D80_8A44_F4C6AA457204__INCLUDED_)
