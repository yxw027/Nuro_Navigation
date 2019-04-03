#if !defined(AFX_MAPFILEPNY_H__52DA06B0_EE9C_4874_8F36_81E5C3A48AE4__INCLUDED_)
#define AFX_MAPFILEPNY_H__52DA06B0_EE9C_4874_8F36_81E5C3A48AE4__INCLUDED_

#include "FileMapBaseZ.h"
#include "NuroClasses.h"
struct NURO_PN_CITYINFO
{
	nuUINT nCityID;
	nuUINT nPoiCount;
	nuUINT nStartPos;
};
class CMapFilePnY : public CFileMapBaseZ  
{
public:
	CMapFilePnY();
	virtual ~CMapFilePnY();	
	nuBOOL Open(nuLONG nMapID);
	nuBOOL IsOpen();
	nuVOID Close();

	nuUINT	GetLength();
	nuBOOL	ReadData(nuUINT nDataStartAddr, nuVOID *pBuffer, nuUINT nLen);

	nuUINT GetCityNum();
	nuBOOL   GetCityInfo(nuWORD nCityID, NURO_PN_CITYINFO* pInfo);
	nuUINT GetStrokes(nuUINT addr);
	nuUINT GetName(nuUINT addr, nuVOID *pBuffer, nuUINT nMaxLen, nuBYTE *pStrokes=NULL);
	nuUINT GetPoiInfoLen_City(nuWORD nCityID);
	nuUINT GetPoiInfo_CITY(nuWORD nCityID, nuBYTE* pBuffer, nuUINT nMaxLen, nuUINT* pBaseAddr);
private:
	nuCFile file;
	nuUINT	m_nCityCount;
};

#endif // !defined(AFX_MAPFILEPNY_H__52DA06B0_EE9C_4874_8F36_81E5C3A48AE4__INCLUDED_)
